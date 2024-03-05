#include "Plane.h"

int idxForSquare(int i,int j, int nX, int nY){
    return i * nY + j;
}

Plane::Plane(double width, double height, glm::vec3 leftCornerPlane){
    this->width = width;
    this->height = height;
    this->leftCornerPlane = leftCornerPlane;
    buildPlane(width, height, leftCornerPlane);
}

void Plane::buildPlane(double width, double height, glm::vec3 leftCornerPlane){
    std::cout << "Plane is building\n";

    /*
    this->indexed_vertices.clear();
    this->indices.clear();
    */
    
    // 16x16 vertices
    double widthBetweenVertex = width / (16-1);
    double heightBetweenVertex = height / (16-1);

    for (int i = 0 ; i < 16 ; i++){
        for (int j = 0 ; j < 16 ; j++ ){
            float x = leftCornerPlane[0] + i*widthBetweenVertex;
            float y = leftCornerPlane[1] + 0.0;
            float z = leftCornerPlane[2] + j*heightBetweenVertex;
            this->indexed_vertices.push_back(glm::vec3(x,y,z));
        }
    }

    for (int i = 0 ; i < 16-1; i++){
    	for (int j = 0 ; j < 16-1; j++){
        	int i0 = idxForSquare(i,j,16,16);
            int i1 = idxForSquare(i+1,j,16,16);
            int i2 = idxForSquare(i+1,j+1,16,16);
            int i3 = idxForSquare(i,j+1,16,16);
           
            this->indices.push_back(i0);
            this->indices.push_back(i1);
            this->indices.push_back(i2);
            this->indices.push_back(i0);
            this->indices.push_back(i2);
            this->indices.push_back(i3);
        }
    }
}

std::vector<unsigned short> Plane::getIndices(){
    return this->indices;
}

std::vector<glm::vec3> Plane::getIndexedVertices(){
    return this->indexed_vertices;
}

glm::vec3 Plane::getCenter(){
    return glm::vec3(this->leftCornerPlane[0] + this->width/2, this->leftCornerPlane[1], this->leftCornerPlane[2] + this->height/2);
}
