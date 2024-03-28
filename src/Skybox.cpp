#include <Skybox.hpp>

Skybox::Skybox(int sizeFace, glm::vec3 position){
    this->sizeFace = sizeFace;
    this->backBottomLeftCorner = position;
    this->buildFaces();
}

void Skybox::buildFaces(){
    std::cout << "Construction des faces de la skybox\n";

    float size = 10.f;
    int div = 1;
    int numVertices = (div + 1) * (div + 1);
    float scale = size / div;

    for (int h = 0; h < div + 1; h++) {
        for (int w = 0; w < div + 1; w++) {
            float x = (float)w * scale - size / 2.0f;
            float y = 0.0f;
            float z = (float)h * scale - size / 2.0f; 

            this->verts.push_back(x);
            this->verts.push_back(y);
            this->verts.push_back(z);
            
            this->verts.push_back((float)w / div);
            this->verts.push_back((float)h / div);
 

            glm::vec3 normal(0., 1., 0.);

            this->verts.push_back(normal.x);
            this->verts.push_back(normal.y);
            this->verts.push_back(normal.z);
        }
    }

    for (int h = 0; h < div; h++) {
        for (int w = 0; w < div; w++) {
            int index = h * (div + 1) + w;
            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(index + div + 1);
            indices.push_back(index + 1);
            indices.push_back(index + div + 2);
            indices.push_back(index + div + 1);
        }
    }

    vb.genVBO(verts);
    va.genVAO();
    eb.genEBO(indices);

    /*
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
    */
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