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
        glm::vec3 leftCornerPlane;
        double width;
        double height;

    public :
        Plane(double width, double height, glm::vec3 leftCornerPlane);
        void buildPlane(double width, double height, glm::vec3 leftCornerPlane);
        std::vector<unsigned short> getIndices(); 
        std::vector<glm::vec3> getIndexedVertices();

        glm::vec3 getCenter();
};