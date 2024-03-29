#pragma once

#include <iostream>
#include <vector>

class VAO {
    private:
        unsigned int VertexAttribObject;
    public:
        VAO();
        void genVAO();
        void genVAOCubemap();
        void delVAO();

        unsigned int getVAO();
};