#pragma once

#include <Headers.hpp>
#include <vector>

struct Face {
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indicesTriangles;
};

class Skybox {
    private:
        std::vector<Face*> facesSkybox;
        int sizeFace; // La skybox est composé de 6 faces carrées de taille sizeFace x sizeFace
        int resolutionFace;
        glm::vec3 backBottomLeftCorner; // Ce point permet de placer la skybox

        GLuint vertexbuffer;
        GLuint elementbuffer;
    public:
        Skybox(int sizeFace, int resolutionFace, glm::vec3 position);
        void buildFaces();
        void loadSkyboxBuffer();
        void sendToBuffer();
};