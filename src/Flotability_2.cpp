#include <glad/glad.h>
#include <Flotability_2.hpp>

// Les objets apparaissent par ligne de 10
Flotability_2::Flotability_2(float heightSpawn){
    this->nbFlottingObject = 15; // Nombre par défaut d'objets qui flotteront (ils se placent en ligne de 10)
    this->heightSpawn = heightSpawn;
    this->resetObjets();
    this->shaderObjetsFlottants.setShader("../shaders/flottabilite_vertexShader.vert", "../shaders/flottabilite_fragmentShader.frag");
	this->createBuffer();
	this->coeffRestitution = 1.7f;
}

Sphere* Flotability_2::createSphere(int i, glm::vec3 position, int nX, int nY){
	Sphere *sphere = new Sphere;
	sphere->position = position;
	sphere->velocity = glm::vec3(0.0f,0.0f,0.0f);
	sphere->radius = 1.0f;
	sphere->mass = 0.8 + i/100.0f;
	sphere->inWater = false;
	sphere->hasReachTop = false;
	
	float theta, phi;
    float thetaStep = glm::pi<float>() / nX;
    float phiStep = 2.0f * glm::pi<float>() / nY;

    for (int i = 0; i <= nX; ++i) {
        theta = i * thetaStep;
        for (int j = 0; j <= nY; ++j) {
            phi = j * phiStep;
            float x = sphere->radius * sin(theta) * cos(phi) + position.x;
            float y = sphere->radius * cos(theta) + position.y; 
            float z = sphere->radius * sin(theta) * sin(phi) + position.z;
            sphere->indexed_vertices.push_back(x);
            sphere->indexed_vertices.push_back(y);
            sphere->indexed_vertices.push_back(z);
        }
    }

    // Générer les indices des triangles
    for (int i = 0; i < nX; ++i) {
        for (int j = 0; j < nY; ++j) {
            int nextStack = (i + 1) % (nX + 1);
            int nextSlice = (j + 1) % nY;

            int idx0 = i * nY + j;
            int idx1 = i * nY + nextSlice;
            int idx2 = nextStack * nY + j;
            int idx3 = nextStack * nY + nextSlice;

            // Premier triangle
            sphere->indices.push_back(idx0);
            sphere->indices.push_back(idx2);
            sphere->indices.push_back(idx1);

            // Deuxième triangle
            sphere->indices.push_back(idx1);
            sphere->indices.push_back(idx2);
            sphere->indices.push_back(idx3);
        }
    }
    
	return sphere;
}

void Flotability_2::createBuffer(){
	this->indexed_vertices.clear();
    this->indices.clear();
    int shift = 0; // Décalage à considérer lors de l'accumulation des indices des sphères
    
    for (int i = 0 ; i < this->listSphere.size() ; i++){
    	Sphere* sphere = this->listSphere[i];
    	for (int n = 0 ; n < sphere->indexed_vertices.size() ; n++){
    		this->indexed_vertices.push_back(sphere->indexed_vertices[n]);
    	}
    	for (int m = 0 ; m < sphere->indices.size() ; m++){
    		this->indices.push_back(sphere->indices[m]+shift);
    	}
    	shift += sphere->indexed_vertices.size() / 3;
    }
    
    this->vbObjets.genVBO(this->indexed_vertices);
    this->vaObjets.genVAOCubemap();
    this->ebObjets.genEBO(this->indices);
}

float computeBuoyantForce(float Amplitude, float depthSphere, float radius, bool *inWater, bool *res) {
	*res = false;
	if (depthSphere <= 0){
		return 0;
	}

    // Volume déplacé par la sphère immergée
    float volume_displaced;
    if (depthSphere >= 2*radius){ // La sphère est complètement immergée
        if (!(*inWater)){
        	*res = true; // A ce moment on joue le bruitage
        	*inWater = true;
        }
        volume_displaced = (4.0f / 3.0f) * M_PI * std::pow(radius, 3);
    } else{
        // La sphère est partiellement immergée
        volume_displaced = M_PI * std::pow(depthSphere, 2) * (3.0f * radius - depthSphere) / 3.0f;
        if (*inWater){
        	*inWater = false;
        }
    }
    return density_water * volume_displaced * gravity;
}

bool Flotability_2::updateSphere(float deltaTime, float Amplitude){
	bool switchToWater = false; 
	bool res;
	for (int i = 0 ; i < this->listSphere.size() ; i++){
		Sphere *sphere = this->listSphere[i];
		
		float depthSphere = sphere->radius - sphere->position.y;
		float buoyantForce = computeBuoyantForce(Amplitude, depthSphere, sphere->radius, &(sphere->inWater), &res);
		glm::vec3 gravityForce = glm::vec3(0.0f, -sphere->mass * gravity, 0.0f);
		glm::vec3 forceNet = gravityForce + glm::vec3(0.0f, buoyantForce, 0.0f);
		
		if (res){
			switchToWater = true;
		}
		
		bool onTop = false;
		
		glm::vec3 acceleration = forceNet / sphere->mass;
		sphere->velocity += acceleration * deltaTime;
		if (!(!(sphere->hasReachTop) && sphere->velocity.y > 0.0f && sphere->position.y >= Amplitude)){
			sphere->position += sphere->velocity * deltaTime;
		}else{
			sphere->hasReachTop = true;
			onTop = true;
		}
		
		if(sphere->hasReachTop && sphere->velocity.y < 0.0f){
			sphere->hasReachTop = false;
		}
		
		Sphere *newSphere = createSphere(i, sphere->position, 20, 20);
		newSphere->velocity = sphere->velocity;
		if (onTop){
			newSphere->velocity.y /= this->coeffRestitution;
		}
		newSphere->mass = sphere->mass;
		newSphere->inWater = sphere->inWater;
		newSphere->hasReachTop = sphere->hasReachTop;
		delete sphere;
		this->listSphere[i] = newSphere;
	}
	this->createBuffer(); // Recrée des nouveaux buffers après avoir mis à jour les sphères
	return switchToWater;
}

bool Flotability_2::drawSphere(float deltaTime, glm::mat4 model, glm::mat4 view, glm::mat4 projection, float Amplitude, float L, float S){
    this->shaderObjetsFlottants.useShader();
    this->shaderObjetsFlottants.setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
    this->shaderObjetsFlottants.setBindMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
    this->shaderObjetsFlottants.setBindMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(this->vaObjets.getVAO()); 
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0); 
    
    return this->updateSphere(deltaTime, Amplitude);
}

void Flotability_2::set_l_pressed(float value){
    this->l_pressed = value;
}

void Flotability_2::resetObjets(){
	this->listSphere.clear();
    for (int i = 0 ; i < this->nbFlottingObject ; i++){
    	Sphere* sphere = createSphere(i, glm::vec3(i%10*3.0, this->heightSpawn, i/10*3.0), 20, 20);
    	this->listSphere.push_back(sphere);
    }
}

void Flotability_2::setHeightSpawn(float heightSpawn){
    this->heightSpawn = heightSpawn;
}

int* Flotability_2::getRefToNbFlottingObject(){
    return &(this->nbFlottingObject);
}

float* Flotability_2::getRefToCoeffRestitution(){
	return &(this->coeffRestitution);
}





