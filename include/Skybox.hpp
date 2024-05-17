#pragma once

#include <VBO.hpp>
#include <VAO.hpp>
#include <EBO.hpp>
#include <Shader.hpp>

#include <Headers.hpp>

class Skybox {
    private:
        float x, y, z;
        std::vector<float> verts;
        std::vector<unsigned int> indices;

        VBO vb;
        EBO eb;
        VAO va;

        Shader skybox;

        std::string pathTextures[6];
        GLuint textureID;

    public:
        Skybox();
        void createSkybox();
        void updateSkybox(GLenum mode);
        void delSkybox();
        void delArrays();

        // Shader
        void useShader();
        void attachShader(const GLchar* vertexPath, const GLchar* fragmentPath);
        void detachShader();
        Shader getShader();

        void Shaderbind1f(const GLchar* name, GLfloat v0);
        void Shaderbind3f(const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2);
        void ShaderbindMatrix4fv(const GLchar* name, const GLfloat *value);
        void ShaderbindMatrix3fv(const GLchar* name, const GLfloat *value);
        void Shaderbind1i(const GLchar* name, GLint v0);

        void loadCubemap();
        void bindCubemap(GLenum TextureUnit, int unit);
};