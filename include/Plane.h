#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

class Plane {
    private : 
        std::vector<unsigned short> indices; 
        std::vector<glm::vec3> indexed_vertices;

    public :
        Plane(double width, double height, glm::vec3 leftCornerPlane);
        void buildPlane(double width, double height, glm::vec3 leftCornerPlane);
        std::vector<unsigned short> getIndices(); 
        std::vector<glm::vec3> getIndexedVertices();
};