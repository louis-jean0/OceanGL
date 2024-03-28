#include <glad/glad.h>
#include <Plane.hpp>


Plane::Plane(float size, int div) {
    this->size = size;
    this->div = div;
    this->ex = this->div;
    this->texIter = this->div;
}

void Plane::createPlane() {
    this->div = div;
    this->va.delVAO();
    this->vb.delVBO();
    this->eb.delEBO();
    delArrays();

    int numVertices = (div + 1) * (div + 1);
    float scale = size / div;

    for (int h = 0; h < this->div + 1; h++) {
        for (int w = 0; w < this->div + 1; w++) {
            float x = (float)w * scale - size / 2.0f;
            float y = 0.0f;
            float z = (float)h * scale - size / 2.0f; 

            this->verts.push_back(x);
            this->verts.push_back(y);
            this->verts.push_back(z);
            
            this->verts.push_back((float)w / div);
            this->verts.push_back((float)h / div);
 

            glm::vec3 normal(0., -1., 0.);
            glm::vec3 tangente(1., 0., 0.); 
            glm::vec3 bitangente(0., 0., 1.); 

            this->verts.push_back(normal.x);
            this->verts.push_back(normal.y);
            this->verts.push_back(normal.z);

            this->verts.push_back(tangente.x);
            this->verts.push_back(tangente.y);
            this->verts.push_back(tangente.z);

            this->verts.push_back(bitangente.x);
            this->verts.push_back(bitangente.y);
            this->verts.push_back(bitangente.z);
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
}

void Plane::subdivisediv(int div) {
    this->div = div;

    this->va.delVAO();
    this->vb.delVBO();
    this->eb.delEBO();
    delArrays();

    int numVertices = (div + 1) * (div + 1);
    float scale = size / div;

    for (int h = 0; h < this->div + 1; h++) {
        for (int w = 0; w < this->div + 1; w++) {
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
}

void Plane::updateSize(float nouvelleTaille) {
    this->size = nouvelleTaille;
    this->subdivisediv(this->div);
}


// Pour le moment, seul les GL_TRIANGLES sont pris en compte
void Plane::updatePlane(GLenum mode) {
    glBindVertexArray(this->va.getVAO()); 
    glDrawElements(mode, this->indices.size(), GL_UNSIGNED_INT, 0); 
}

void Plane::delPlane() {
    delArrays();
    this->plane.deleteShader();
    this->va.delVAO();
    this->vb.delVBO();
    this->eb.delEBO();
}

void Plane::delArrays() {
    this->verts.clear();
    this->indices.clear();
}

// Shader
void Plane::useShader() {
    this->plane.useShader();
}

void Plane::attachShader(const GLchar* vertexPath, const GLchar* fragmentPath) {
    this->plane.setShader(vertexPath, fragmentPath);
}

void Plane::detachShader() {
    this->plane.Program = NULL;
}

void Plane::Shaderbind1f(const GLchar* name, GLfloat v0) {
    this->plane.setBind1f(name, v0);
}

void Plane::Shaderbind3f(const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2) {
    this->plane.setBind3f(name, v0, v1, v2);
}

void Plane::ShaderbindMatrix4fv(const GLchar* name, const GLfloat *value) {
    this->plane.setBindMatrix4fv(name, 1, GL_FALSE, value);
}

void Plane::ShaderbindMatrix3fv(const GLchar* name, const GLfloat *value) {
    this->plane.setBindMatrix3fv(name, 1, GL_FALSE, value);
}

void Plane::Shaderbind1i(const GLchar* name, GLint v0) {
    this->plane.setBind1i(name, v0);
}

Shader Plane::getShader() {
    return this->plane;
}