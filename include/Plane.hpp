#pragma once

#include <VBO.hpp>
#include <VAO.hpp>
#include <EBO.hpp>
#include <Shader.hpp>
#include <ComputeShader.hpp>

#include <Headers.hpp>

class Plane {
    private:
        float x, y, z;
        std::vector<float> verts;
        std::vector<unsigned int> indices;

        VBO vb;
        EBO eb;
        VAO va;

        // Normales
        VBO vb_normals;
        EBO eb_normals;
        VAO va_normals;

        std::vector<float> verts_normals;
        std::vector<unsigned int> indices_normals;


        float size;
        int div;
        int ex;
        int texIter;

        Shader plane;
        ComputeShader planeComp;

        std::string pathTextures[6];
        GLuint textureID;

    public:
        Plane(float size, int div);
        void createPlane();
        void createPlaneWithNoise(double bruit);
        void subdivisediv(int div);
        void updateSize(float nouvelleTaille);

        void updatePlane(GLenum mode);
        void delPlane();
        void delArrays();

        // Shader
        void useShader();
        void useComputeShader();

        void attachShader(const GLchar* vertexPath, const GLchar* fragmentPath);
        void attachShaderComp(const GLchar* compPath);
        void detachShader();
        Shader getShader();
        ComputeShader getShaderComp();


        void Shaderbind1f(const GLchar* name, GLfloat v0);
        void Shaderbind3f(const GLchar* name, GLfloat v0, GLfloat v1, GLfloat v2);
        void ShaderbindMatrix4fv(const GLchar* name, const GLfloat *value);
        void ShaderbindMatrix3fv(const GLchar* name, const GLfloat *value);
        void Shaderbind1i(const GLchar* name, GLint v0);

        void ComputeWorkGroup();
        void DispatchWorkGroup(int width, int height, int wkw, int wkh);

        void loadCubemap();
        void bindCubemap(GLenum TextureUnit, int unit);
};