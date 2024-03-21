#pragma once

#include <Headers.hpp>
#include <vector>
#include <Shader.hpp>

struct Face {
    std::vector<glm::vec3> vertices;
    std::vector<unsigned short> indicesTriangles;

    GLuint vertexbuffer;
    GLuint elementbuffer;
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
        void loadSkyboxBuffer();
        void sendToBuffer();

        void useShader();
        void attachShader(const GLchar* vertexPath, const GLchar* fragmentPath);
        void detachShader();
};