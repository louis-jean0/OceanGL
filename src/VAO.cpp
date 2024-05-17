#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <VAO.hpp>

// Constructeur de la classe
VAO::VAO() {
    this->VertexAttribObject = 0;
}

void VAO::genVAO() {
    glGenVertexArrays(1, &this->VertexAttribObject);
    glBindVertexArray(this->VertexAttribObject);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Textures
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Normales
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void VAO::genVAOCubemap() {
    glGenVertexArrays(1, &this->VertexAttribObject);
    glBindVertexArray(this->VertexAttribObject);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}



unsigned int VAO::getVAO() {
    return this->VertexAttribObject;
}

// Fonction pour supprimer les VAO
void VAO::delVAO() {
    glDeleteVertexArrays(1, &this->VertexAttribObject);
}