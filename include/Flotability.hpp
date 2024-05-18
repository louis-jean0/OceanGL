#pragma once

#include <VBO.hpp>
#include <VAO.hpp>
#include <EBO.hpp>
#include <Shader.hpp>

#include <Headers.hpp>

class Flotability {
    private:
        std::vector<float> indexed_vertices;
        std::vector<unsigned int> indices;
        std::vector<glm::vec3> positionsObjets;
        std::vector<float> massesObjets;
        std::vector<int> decalageIndices;
        std::vector<short> stateObjets; // 0 dans l'air, 1 sous son cycle, 2 dans son cycle
        std::vector<float> forceCycle;
        std::vector<float> avance;
        VBO vbObjets;
        VAO vaObjets;
        EBO ebObjets;
        Shader shaderObjetsFlottants;
        float l_pressed;
        float hauteurApparition;
        int nbFlottingObject;

    public:
        Flotability(float hauteurApparition);
        void buildObject(int nX, int nY, double rayon); // Pour l'instant ça ne construit que des sphères
        void drawObjets(glm::mat4 model, glm::mat4 view, glm::mat4 projection, float Amplitude, float L, float S);
        void set_l_pressed(float value);
        void resetObjets();
        void setHeightSpawn(float heightSpawn);
        int* getRefToNbFlottingObject();
};
