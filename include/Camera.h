#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class Camera {
    private : 
        glm::vec3 camera_position;
        glm::vec3 camera_target;
        glm::vec3 camera_up;

    public :
        Camera(glm::vec3 camera_position, glm::vec3 camera_target, glm::vec3 camera_up);
        glm::vec3 getPosition();
        glm::vec3 getTarget();
        glm::vec3 getUp();
        
        void setTarget(glm::vec3 camera_target);
};
