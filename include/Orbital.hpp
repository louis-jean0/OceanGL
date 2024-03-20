#pragma once

#include <Headers.hpp>

// Structure définissant un quaternion
struct Quaternion {
    float cosine;
    glm::vec3 axis;
};

class Orbital {
    public:

        glm::vec3 position = glm::vec3(0.0f, 0.0f, -10.0f);
        glm::vec3 startPos;
        glm::vec3 currentPos = startPos;
        glm::vec3 startPosUnitVector;
        glm::vec3 currentPosUnitVector;

        Quaternion currentQuaternion;
        Quaternion lastQuaternion = {0.0f, glm::vec3(1.0f, 0.0f, 0.0f)};
        
        float cosValue, cosValue_2;
        float theta;
        float angle = 180.0f;
        glm::vec3 rotationalAxis = glm::vec3(1.0f, 0.0f, 0.0f);                       
        glm::vec3 rotationalAxis_2;
        Orbital (){};
        float z_axis(float x, float y, float RADIUS);
        glm::vec3 getUnitVector(glm::vec3);
        float dotProduct();
        void rotation();
        void replace();
};