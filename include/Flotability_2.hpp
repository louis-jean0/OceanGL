#pragma once

#include <VBO.hpp>
#include <VAO.hpp>
#include <EBO.hpp>
#include <Shader.hpp>

#include <Headers.hpp>


const float gravity = 9.81f; // Gravité
const float density_water = 1.0f; // Densité de l'eau en kg/m^3

struct Sphere {
    glm::vec3 position;
    glm::vec3 velocity;
    float radius;
    float mass;
    std::vector<float> indexed_vertices;
    std::vector<unsigned int> indices;
    bool inWater;
    bool hasReachTop;
};

class Flotability_2 {
    private:
        VBO vbObjets;
        VAO vaObjets;
        EBO ebObjets;
        std::vector<float> indexed_vertices;
    	std::vector<unsigned int> indices;
        Shader shaderObjetsFlottants;
        float l_pressed;
        int nbFlottingObject;
        std::vector<Sphere*> listSphere;
		float heightSpawn;
		float coeffRestitution;
    public:
        Flotability_2(float heightSpawn);
        Sphere* createSphere(int i, glm::vec3 position, int nX, int nY);
        
        void createBuffer();
        bool updateSphere(float deltaTime, float Amplitude);
        bool drawSphere(float deltaTime, glm::mat4 model, glm::mat4 view, glm::mat4 projection, float Amplitude, float L, float S);
        void set_l_pressed(float value);
        void resetObjets();
        void setHeightSpawn(float heightSpawn);
        int* getRefToNbFlottingObject();
        float* getRefToCoeffRestitution();
};
