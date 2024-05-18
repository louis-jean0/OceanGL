#include <glad/glad.h>
#include <Flotability_2.hpp>

// Les objets apparaissent par ligne de 10
Flotability_2::Flotability_2(float heightSpawn){
    this->nbFlottingObject = 15; // Nombre par défaut d'objets qui flotteront (ils se placent en ligne de 10)
    this->heightSpawn = heightSpawn;
    this->resetObjets();
    this->shaderObjetsFlottants.setShader("../shaders/flottabilite_vertexShader.vert", "../shaders/flottabilite_fragmentShader.frag");
	this->createBuffer();
	std::cout << "ICICICICICICI = " << this->indices.size() << "\n";
}

Sphere* Flotability_2::createSphere(glm::vec3 position, int nX, int nY){
	Sphere *sphere = new Sphere;
	sphere->position = position;
	sphere->radius = 1.0f;
	
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
    	//shift += sphere->indexed_vertices.size();
    }
    
    this->vbObjets.genVBO(this->indexed_vertices);
    this->vaObjets.genVAOCubemap();
    this->ebObjets.genEBO(this->indices);
}

void Flotability_2::updateSphere(){
	std::cout << "Update sphere\n";
}

void Flotability_2::drawSphere(glm::mat4 model, glm::mat4 view, glm::mat4 projection, float Amplitude, float L, float S){
    this->shaderObjetsFlottants.useShader();
    this->shaderObjetsFlottants.setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
    this->shaderObjetsFlottants.setBindMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
    this->shaderObjetsFlottants.setBindMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(this->vaObjets.getVAO()); 
    
    this->updateSphere();
}

void Flotability_2::set_l_pressed(float value){
    this->l_pressed = value;
}

void Flotability_2::resetObjets(){
	this->listSphere.clear();
    for (int i = 0 ; i < this->nbFlottingObject ; i++){
    	Sphere* sphere = createSphere(glm::vec3(i%10*2.0, this->heightSpawn, i/10*2.0), 20, 20);
    	this->listSphere.push_back(sphere);
    }
}

void Flotability_2::setHeightSpawn(float heightSpawn){
    this->heightSpawn = heightSpawn;
}

int* Flotability_2::getRefToNbFlottingObject(){
    return &(this->nbFlottingObject);
}





