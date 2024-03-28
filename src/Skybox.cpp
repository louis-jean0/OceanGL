#include <glad/glad.h>
#include <Skybox.hpp>

Skybox::Skybox() {

}

void Skybox::createSkybox() {
  
    // Vertices
    this->verts.push_back( -1.0f );     this->verts.push_back( -1.0f );     this->verts.push_back( 1.0f );
    this->verts.push_back( 1.0f );     this->verts.push_back( -1.0f );     this->verts.push_back( 1.0f );
    this->verts.push_back( 1.0f );     this->verts.push_back( -1.0f );     this->verts.push_back( -1.0f );
    this->verts.push_back( -1.0f );     this->verts.push_back( -1.0f );     this->verts.push_back( -1.0f );
    this->verts.push_back( -1.0f );     this->verts.push_back( 1.0f );     this->verts.push_back( 1.0f );
    this->verts.push_back( 1.0f );     this->verts.push_back( 1.0f );     this->verts.push_back( 1.0f );
    this->verts.push_back( 1.0f );     this->verts.push_back( 1.0f );     this->verts.push_back( -1.0f );
    this->verts.push_back( -1.0f );     this->verts.push_back( 1.0f );     this->verts.push_back( -1.0f );


    this->indices.push_back(1);     this->indices.push_back(2);     this->indices.push_back(6);
    this->indices.push_back(6);     this->indices.push_back(5);     this->indices.push_back(1);
    this->indices.push_back(0);     this->indices.push_back(4);     this->indices.push_back(7);
    this->indices.push_back(7);     this->indices.push_back(3);     this->indices.push_back(0);
    this->indices.push_back(4);     this->indices.push_back(5);     this->indices.push_back(6);
    this->indices.push_back(6);     this->indices.push_back(7);     this->indices.push_back(4);
    this->indices.push_back(0);     this->indices.push_back(3);     this->indices.push_back(2);
    this->indices.push_back(2);     this->indices.push_back(1);     this->indices.push_back(0);
    this->indices.push_back(0);     this->indices.push_back(1);     this->indices.push_back(5);
    this->indices.push_back(5);     this->indices.push_back(4);     this->indices.push_back(0);
    this->indices.push_back(3);     this->indices.push_back(7);     this->indices.push_back(6);
    this->indices.push_back(6);     this->indices.push_back(2);     this->indices.push_back(3);


    this->vb.genVBO(this->verts);
    this->va.genVAOCubemap();
    this->eb.genEBO(this->indices);
}

void Skybox::updateSkybox(GLenum mode) {
    glBindVertexArray(this->va.getVAO()); 
    glDrawElements(mode, this->indices.size(), GL_UNSIGNED_INT, 0);
}

void Skybox::delSkybox() {
    this->skybox.deleteShader();
    this->va.delVAO();
    this->vb.delVBO();
    this->eb.delEBO();
}

void Skybox::delArrays() {
    this->verts.clear();
    this->indices.clear();
}

// Basic Shaders
void Skybox::attachShader(const GLchar* vertexPath, const GLchar* fragmentPath) {
    this->skybox.setShader(vertexPath, fragmentPath);
}

void Skybox::useShader() {
    this->skybox.useShader();
}

Shader Skybox::getShader() {
    return this->skybox;
}

void Skybox::Shaderbind1f(const GLchar* name, GLfloat v0) {
    this->skybox.setBind1f(name, v0);
}

void Skybox::ShaderbindMatrix4fv(const GLchar* name, const GLfloat *value) {
    this->skybox.setBindMatrix4fv(name, 1, GL_FALSE, value);
}

void Skybox::ShaderbindMatrix3fv(const GLchar* name, const GLfloat *value) {
    this->skybox.setBindMatrix3fv(name, 1, GL_FALSE, value);
}

void Skybox::Shaderbind1i(const GLchar* name, GLint v0) {
    this->skybox.setBind1i(name, v0);
}
