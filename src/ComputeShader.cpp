#include <glad/glad.h>
#include <ComputeShader.hpp>
#include <fstream>
#include <sstream>
#include <GLFW/glfw3.h>

ComputeShader::ComputeShader()
{

}

void ComputeShader::setComputeShader(const GLchar* computePath)
{
     // Shaders reading
    std::string computeCode;
    std::ifstream cShaderFile;
    cShaderFile.exceptions(std::ifstream::badbit);

    try
    {
        cShaderFile.open(computePath);
        std::stringstream cShaderStream;
        cShaderStream << cShaderFile.rdbuf();
        cShaderFile.close();
        computeCode = cShaderStream.str();

        std::cout << "Shaders successfully loaded !" << std::endl;
    }

    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const GLchar * cShaderCode = computeCode.c_str();

    // Shaders compilation
    GLuint compute;
    GLint success;
    GLchar infoLog[512];

    // Compute Shader
    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &cShaderCode, NULL);
    glCompileShader(compute);
    glGetShaderiv(compute, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(compute, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED \n{}" << computePath << std::endl;
    }
    else
    {
        std::cout << "SHADER::COMPUTE::COMPILATION_COMPLETED \n{}" << computePath << std::endl;
    }


    // Shader Program
    this->Program = glCreateProgram();
    glAttachShader(this->Program, compute);
    glLinkProgram(this->Program);
    glGetProgramiv(this->Program, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(compute, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED \n" << computePath << std::endl;
        std::cout << "Shader compilation error log:\n" << infoLog << std::endl; // Afficher le journal d'erreurs du shader
    }
    else
    {
        std::cout << "SHADER::COMPUTE::COMPILATION_COMPLETED \n" << computePath << std::endl;
    }


    glDeleteShader(compute);
}

void ComputeShader::useComputeShader()
{
    glUseProgram(this->Program);
}

void ComputeShader::deleteComputeShader()
{
    glDeleteProgram(this->Program);
}

void ComputeShader::setBind1f(const GLchar* name, GLfloat v0)
{
    glUniform1f(glGetUniformLocation(this->Program, name), v0);
}

void ComputeShader::setBindMatrix4fv(const GLchar* name, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    glUniformMatrix4fv(glGetUniformLocation(this->Program, name), count, transpose, value);
}

void ComputeShader::setBind1i(const GLchar* name, GLint v0)
{
    glUniform1i(glGetUniformLocation(this->Program, name), v0);
}