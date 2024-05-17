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

    std::string textures[6] = {
        "../textures/Skybox/px.png",
        "../textures/Skybox/nx.png",
        "../textures/Skybox/py.png",
        "../textures/Skybox/ny.png",
        "../textures/Skybox/pz.png",
        "../textures/Skybox/nz.png"
    };
    std::copy(std::begin(textures), std::end(textures), std::begin(this->pathTextures));
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

void Skybox::loadCubemap(){
    glGenTextures(1, &(this->textureID));
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < 6; i++)
    {
        unsigned char *data = stbi_load(this->pathTextures[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            //std::cout << "Text " << i << " : width = " << width << ", height = " << height << ", nr = " << nrChannels << "\n";
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << this->pathTextures[i].c_str() << std::endl;
            stbi_image_free(data);
            return;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Axe x
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Axe y
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // Axe z
}

void Skybox::bindCubemap(GLenum TextureUnit, int unit){
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureID);
    glUniform1i(glGetUniformLocation(this->skybox.Program, "skyboxTexture"), unit);
}
