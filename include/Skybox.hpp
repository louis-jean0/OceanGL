#pragma once

#include <Headers.hpp>
#include <vector>
#include <Shader.hpp>

struct Face {
    std::vector<float> vertices;
    std::vector<unsigned int> indicesTriangles;

    VBO vb;
    EBO eb;
    VAO va;

};

class Skybox {
    private:
        int sizeFace; // La skybox est composé de 6 faces carrées de taille sizeFace x sizeFace
        int resolutionFace;
        glm::vec3 backBottomLeftCorner; // Ce point permet de placer la skybox
        std::vector<Face*> facesSkybox;

        Shader shader;
    public:
        Skybox(int sizeFace, int resolutionFace, glm::vec3 position);
        void buildFaces();

        void useShader();
        void attachShader(const GLchar* vertexPath, const GLchar* fragmentPath);
        void detachShader();

        void updateSkybox(GLenum mode);
};