#include <Skybox.hpp>

Skybox::Skybox(int sizeFace, glm::vec3 position){
    this->sizeFace = sizeFace;
    this->backBottomLeftCorner = position;
    this->buildFaces();
}

void Skybox::buildFaces(){
    std::cout << "Construction des faces de la skybox\n";

    this->va.delVAO();
    this->vb.delVBO();
    this->eb.delEBO();
    this->verts.clear();
    this->indices.clear();

    for (int i = 0 ; i < 6 ; i++){
        for (int h = 0; h < 2 ; h++) {
            for (int w = 0; w < 2; w++) {
                float x, y, z;
                int n = (i%2 == 0 ? 1 : -1);

                 if (i < 2){ // Faces bottom et top
                    x = this->backBottomLeftCorner[0] + (float)w * this->sizeFace;
                    y = this->backBottomLeftCorner[1] + i * this->sizeFace;
                    z = this->backBottomLeftCorner[2] + i*this->sizeFace + ((float)h * this->sizeFace)*n; 
                }else if (i == 2){ // Faces back
                    x = this->backBottomLeftCorner[0] + this->sizeFace * (1-w);
                    y = this->backBottomLeftCorner[1] + (float)h * this->sizeFace; 
                    z = this->backBottomLeftCorner[2];
                }else if (i == 3){ // Faces front
                    x = this->backBottomLeftCorner[0] + (float)w * this->sizeFace;
                    y = this->backBottomLeftCorner[1] + (float)h * this->sizeFace; 
                    z = this->backBottomLeftCorner[2] + this->sizeFace;
                }else if (i == 4){ // Face left
                    x = this->backBottomLeftCorner[0] + (i-4) * this->sizeFace;
                    y = this->backBottomLeftCorner[1] + (float)h * this->sizeFace;
                    z = this->backBottomLeftCorner[2] + (float)w * this->sizeFace; 
                }else if (i == 5){ // Face right
                    x = this->backBottomLeftCorner[0] + (i-4) * this->sizeFace;
                    y = this->backBottomLeftCorner[1] + (float)h * this->sizeFace;
                    z = this->backBottomLeftCorner[2] + this->size * (1-w); 
                }
                this->verts.push_back(x);
                this->verts.push_back(y);
                this->verts.push_back(z);

                this->verts.push_back((float)w);
                this->verts.push_back((float)h);
    
                glm::vec3 normal(0., 1., 0.);

                this->verts.push_back(normal.x);
                this->verts.push_back(normal.y);
                this->verts.push_back(normal.z);
            }
        }

        unsigned int decalage = i*4; 
        this->indices.push_back(decalage + 3);
        this->indices.push_back(decalage + 0);
        this->indices.push_back(decalage + 2);
        this->indices.push_back(decalage + 1);
        this->indices.push_back(decalage + 0);
        this->indices.push_back(decalage + 3);
    }

    vb.genVBO(this->verts);
    va.genVAO();
    eb.genEBO(this->indices);
}

void Skybox::useShader() {
    this->shader.useShader();
}

void Skybox::attachShader(const GLchar* vertexPath, const GLchar* fragmentPath) {
    this->shader.setShader(vertexPath, fragmentPath);
}

void Skybox::detachShader() {
    this->shader.Program = NULL;
}

void Skybox::updateSkybox(GLenum mode){
    glBindVertexArray(this->va.getVAO()); 
    glDrawElements(mode, this->indices.size(), GL_UNSIGNED_INT, 0); 
}