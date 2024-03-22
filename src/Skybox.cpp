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

        f->va.delVAO();
        f->vb.delVBO();
        f->eb.delEBO();

        for (int h = 0; h < this->resolutionFace + 1; h++) {
            for (int w = 0; w < this->resolutionFace + 1; w++) {
                float x, y, z;

                if (i < 2){ // Faces nx et px
                    x = this->backBottomLeftCorner[0] + (float)w * step;
                    y = this->backBottomLeftCorner[1] + i * this->sizeFace;
                    z = this->backBottomLeftCorner[2] + (float)h * step; 
                }else if (i < 4){ // Faces ny et py
                    x = this->backBottomLeftCorner[0] + (float)w * step;
                    y = this->backBottomLeftCorner[1] + (float)h * step; 
                    z = this->backBottomLeftCorner[2] + (i-2) * this->sizeFace;
                }else if (i < 6){ // Face nz et pz
                    x = this->backBottomLeftCorner[0] + (i-4) * this->sizeFace;
                    y = this->backBottomLeftCorner[1] + (float)w * step;
                    z = this->backBottomLeftCorner[2] + (float)h * step; 
                }

                f->vertices.push_back(x);
                f->vertices.push_back(y);
                f->vertices.push_back(z);
            }
        }

        for (int h = 0; h < this->resolutionFace; h++) {
            for (int w = 0; w < this->resolutionFace; w++) {
                unsigned short index = h * (this->resolutionFace + 1) + w;
                f->indicesTriangles.push_back(index);
                f->indicesTriangles.push_back(index + 1);
                f->indicesTriangles.push_back(index + this->resolutionFace + 1);
                f->indicesTriangles.push_back(index + 1);
                f->indicesTriangles.push_back(index + this->resolutionFace + 2);
                f->indicesTriangles.push_back(index + this->resolutionFace + 1);
            }
        }

        f->vb.genVBO(f->vertices);
        f->va.genVAO();
        f->eb.genEBO(f->indicesTriangles);

        this->facesSkybox.push_back(f);
    }
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
    for (int i = 0 ; i < 6 ; i++){
        glBindVertexArray(this->facesSkybox[i]->va.getVAO()); 

        glDrawElements(mode, this->facesSkybox[i]->indicesTriangles.size(), GL_UNSIGNED_INT, 0); 

        /*
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, this->facesSkybox[i]->vb.getVBO());
        glVertexAttribPointer(
                    0,                  // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->facesSkybox[i]->eb.getEBO());

        // Draw the triangles !
        glDrawElements(
                    GL_TRIANGLES,      // mode
                    this->facesSkybox[i]->indicesTriangles.size(),    // count
                    GL_UNSIGNED_INT,   // type
                    (void*)0           // element array buffer offset
                    );

        glDisableVertexAttribArray(0);
        */

    }
}