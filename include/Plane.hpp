#pragma once

#include <VBO.hpp>
#include <VAO.hpp>
#include <EBO.hpp>
#include <Shader.hpp>

class Plane {
    private:
        float x, y, z;
        std::vector<float> verts;
        std::vector<unsigned int> indices;

        VBO vb;
        EBO eb;
        VAO va;

        float size;
        int div;
        int ex;
        int texIter;

        Shader plane;

    public:
        Plane(float size, int div);
        void createPlane();
        void createPlaneWithNoise(double bruit);
        void subdivisediv(int div);

        void updatePlane(GLenum mode);
        void delPlane();
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

        // // Texture
        // void setTexture(const char* texPath, std::string texName, bool texFlip);
        // void useTexture(GLenum en, const GLchar* name, GLint v0);
};