#pragma once

#include <GLFW/glfw3.h>
#include <iostream>


class ComputeShader
{
    public:
        GLuint Program;

        ComputeShader();
        void setComputeShader(const GLchar* computePath);        
        void useComputeShader();
        void deleteComputeShader();

        void setBind1f(const GLchar* name, GLfloat v0);
        void setBindMatrix4fv(const GLchar* name, GLsizei count, GLboolean transpose, const GLfloat *value);
        void setBind1i(const GLchar* name, GLint v0);
};

