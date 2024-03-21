#include <Skybox.hpp>

Skybox::Skybox(int sizeFace, int resolutionFace, glm::vec3 position){
    this->sizeFace = sizeFace;
    this->resolutionFace = resolutionFace;
    this->backBottomLeftCorner = position;
    this->buildFaces();
}

void Skybox::buildFaces(){
    std::cout << "Construction des faces de la skybox\n";

    float step = this->sizeFace / this->resolutionFace;

    for (int i = 0 ; i < 6 ; i++){
        std::cout << "Face " << i << "\n";
        Face *f = new Face;

        for (int h = 0; h < this->resolutionFace + 1; h++) {
            for (int w = 0; w < this->resolutionFace + 1; w++) {
                if (i < 2){ // Faces nx et px
                    float x = this->backBottomLeftCorner[0] + (float)w * step;
                    float y = this->backBottomLeftCorner[1] + i * this->sizeFace;
                    float z = this->backBottomLeftCorner[2] + (float)h * step; 
                    
                    f->vertices.push_back(glm::vec3(x,y,z));
                }else if (i < 4){ // Faces ny et py
                    float x = this->backBottomLeftCorner[0] + (float)w * step;
                    float y = this->backBottomLeftCorner[1] + (float)h * step; 
                    float z = this->backBottomLeftCorner[2] + (i-2) * this->sizeFace;
                    
                    f->vertices.push_back(glm::vec3(x,y,z));
                }else if (i < 6){ // Face nz et pz
                    float x = this->backBottomLeftCorner[0] + (i-4) * this->sizeFace;
                    float y = this->backBottomLeftCorner[1] + (float)w * step;
                    float z = this->backBottomLeftCorner[2] + (float)h * step; 

                    f->vertices.push_back(glm::vec3(x,y,z));
                }
            }
        }

        for (int h = 0; h < this->resolutionFace; h++) {
            for (int w = 0; w < this->resolutionFace; w++) {
                int index = h * (this->resolutionFace + 1) + w;
                f->indicesTriangles.push_back(index);
                f->indicesTriangles.push_back(index + 1);
                f->indicesTriangles.push_back(index + this->resolutionFace + 1);
                f->indicesTriangles.push_back(index + 1);
                f->indicesTriangles.push_back(index + this->resolutionFace + 2);
                f->indicesTriangles.push_back(index + this->resolutionFace + 1);
            }
        }

        this->facesSkybox.push_back(f);
    }
}

void Skybox::loadSkyboxBuffer(){
    glGenBuffers(1, &(this->vertexbuffer));
    glBindBuffer(GL_ARRAY_BUFFER, this->vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, (this->facesSkybox[0])->vertices.size() * sizeof(glm::vec3), &((this->facesSkybox[0])->vertices[0]), GL_STATIC_DRAW);

    // Generate a buffer for the indices as well
    glGenBuffers(1, &(this->elementbuffer));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,  (this->facesSkybox[0])->indicesTriangles.size() * sizeof(unsigned int), &((this->facesSkybox[0])->indicesTriangles[0]) , GL_STATIC_DRAW);
}

void Skybox::sendToBuffer(){
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, this->vertexbuffer);
    glVertexAttribPointer(
                    0,                  // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementbuffer);

    // Draw the triangles !
    glDrawElements(
                    GL_TRIANGLES,      // mode
                    (this->facesSkybox[0])->indicesTriangles.size(),    // count
                    GL_UNSIGNED_INT,   // type
                    (void*)0           // element array buffer offset
                    );

    glDisableVertexAttribArray(0);
}