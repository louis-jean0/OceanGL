#include <glad/glad.h>
#include <Flotability.hpp>

// Les objets apparaissent par ligne de 10
Flotability::Flotability(float hauteurApparition){
    this->hauteurApparition = hauteurApparition;
    this->nbFlottingObject = 15; // Nombre par défaut d'objets qui flotteront
    this->resetObjets();
    this->shaderObjetsFlottants.setShader("../shaders/flottabilite_vertexShader.vert", "../shaders/flottabilite_fragmentShader.frag");
}

void Flotability::buildObject(int nX, int nY, double rayon){
    this->indexed_vertices.clear();
    this->indices.clear();
    this->decalageIndices.clear();

    float theta, phi;
    float thetaStep = glm::pi<float>() / nX;
    float phiStep = 2.0f * glm::pi<float>() / nY;

    for (int n = 0 ; n < this->positionsObjets.size() ; n++){
        int nbVertexSphere = 0;
        for (int i = 0; i <= nX; ++i) {
            theta = i * thetaStep;
            for (int j = 0; j <= nY; ++j) {
                phi = j * phiStep;
                float x = rayon * sin(theta) * cos(phi) + this->positionsObjets[n].x;
                float y = rayon * sin(theta) * sin(phi) + this->positionsObjets[n].y;
                float z = rayon * cos(theta) + this->positionsObjets[n].z;
                this->indexed_vertices.push_back(x);
                this->indexed_vertices.push_back(y);
                this->indexed_vertices.push_back(z);
                nbVertexSphere++;
            }
        }

        int decalage = 0;
        for (int i = 0 ; i < this->decalageIndices.size() ; i++){
            decalage+=this->decalageIndices[i];
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
                this->indices.push_back(idx0+decalage);
                this->indices.push_back(idx2+decalage);
                this->indices.push_back(idx1+decalage);

                // Deuxième triangle
                this->indices.push_back(idx1+decalage);
                this->indices.push_back(idx2+decalage);
                this->indices.push_back(idx3+decalage);
            }
        }
        this->decalageIndices.push_back(nbVertexSphere);
    }
    
    this->vbObjets.genVBO(this->indexed_vertices);
    this->vaObjets.genVAOCubemap();
    this->ebObjets.genEBO(this->indices);
}

void Flotability::drawObjets(glm::mat4 model, glm::mat4 view, glm::mat4 projection, float Amplitude, float L, float S){
    this->shaderObjetsFlottants.useShader();
    this->shaderObjetsFlottants.setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
    this->shaderObjetsFlottants.setBindMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
    this->shaderObjetsFlottants.setBindMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(this->vaObjets.getVAO()); 
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0); 
    

    float w = (2.0f * M_PI) / L;
    //float phi_t = (S * w) * glfwGetTime();
    for (int i = 0 ; i < this->positionsObjets.size() ; i++){
        if (this->stateObjets[i]==2){ // Dans son cyle
            float phi_t = S*w*this->avance[i];
            this->positionsObjets[i].y = Amplitude * sin(this->massesObjets[i] * phi_t);
            if (this->avance[i] <= (3*M_PI)/2){
                this->avance[i] += 0.01;
            }else{
                this->avance[i] = (-M_PI)/2;
            }
        }else if (this->stateObjets[i]==1){ // Sous son cycle (doit remonter à la hauteur de son cycle)
            this->positionsObjets[i].y -= this->forceCycle[i];
            this->forceCycle[i] -= 0.2f;
            if (this->positionsObjets[i].y >= -Amplitude){ // l'objet a atteint la hauteur minimale de son cycle
                this->stateObjets[i]=2;
            }
        }else { // L'état est à 0, c'est-à-dire que l'objet est dans l'air
            if (this->positionsObjets[i].y > -Amplitude){ // L'objet est dans l'air, alors soit il passe sous son cycle, soit il continue de chuter (ici le 4 devra changer, selon la vitesse de sa chute au moment où il change d'état)
                this->positionsObjets[i].y -= pow((glfwGetTime()-this->l_pressed),1.5) * 0.5;
            }else{
                this->stateObjets[i] = 1; // Il est passé sous son cycle, donc l'état passe à 1
                this->forceCycle[i] = pow((glfwGetTime()-this->l_pressed),1.5) * 0.5;
            }
        }
    }
    this->buildObject(20,20,1.0f);
}

void Flotability::set_l_pressed(float value){
    this->l_pressed = value;
}

void Flotability::resetObjets(){
    this->positionsObjets.clear();
    this->massesObjets.clear();
    this->stateObjets.clear();
    this->forceCycle.clear();
    this->avance.clear();

    for (int i = 0 ; i < this->nbFlottingObject ; i++){
        this->positionsObjets.push_back(glm::vec3(i%10 * 2,this->hauteurApparition,i/10 * 2));
        this->massesObjets.push_back(0.1 + (rand()%10)/10.0); // Masse entre 0.1 et 1.0
        this->stateObjets.push_back(0);
        this->forceCycle.push_back(0.0f);
        this->avance.push_back((-M_PI)/2);
    }
}

void Flotability::setHeightSpawn(float heightSpawn){
    this->hauteurApparition = heightSpawn;
}

int* Flotability::getRefToNbFlottingObject(){
    return &(this->nbFlottingObject);
}