#include <Orbital.hpp>

float Orbital::z_axis(float x, float y, float RADIUS){
    float z = 0; 
    if(sqrt((x * x) + (y * y)) <= RADIUS) z = (float)sqrt((RADIUS * RADIUS) - (x * x) - (y * y)); 
    return z;
}

glm::vec3 Orbital::getUnitVector(glm::vec3 vectr){
    float magnitude1;
    glm::vec3 unitVector; 
    magnitude1 = (vectr.x * vectr.x) + (vectr.y * vectr.y) + (vectr.z * vectr.z); 
    magnitude1 = sqrt(magnitude1);
    if(magnitude1 == 0){
        unitVector.x = 0;
        unitVector.y = 0;
        unitVector.z = 0;
            }
    else {
    unitVector.x = vectr.x / magnitude1; 
    unitVector.y = vectr.y / magnitude1;
    unitVector.z = vectr.z / magnitude1;
}
    return unitVector;    
}

float Orbital::dotProduct(){
    float result = (startPosUnitVector.x * currentPosUnitVector.x) + (startPosUnitVector.y * currentPosUnitVector.y) + (startPosUnitVector.z * currentPosUnitVector.z);
    return result;
}

void Orbital::rotation(){
    startPosUnitVector = getUnitVector(startPos);
    currentPosUnitVector = getUnitVector(currentPos);
    currentQuaternion.axis = glm::cross(startPos, currentPos);
    currentQuaternion.axis = getUnitVector(currentQuaternion.axis);
    
    cosValue = dotProduct(); 
    if(cosValue > 1) cosValue = 1;
    theta = (acos(cosValue) * 180 / 3.1416);
    currentQuaternion.cosine = cos((theta / 2) * 3.1416 / 180);

    currentQuaternion.axis.x = currentQuaternion.axis.x * sin((theta / 2) * 3.1416 / 180);
    currentQuaternion.axis.y = currentQuaternion.axis.y * sin((theta / 2) * 3.1416 / 180);
    currentQuaternion.axis.z = currentQuaternion.axis.z * sin((theta / 2) * 3.1416 / 180);
    
    cosValue_2 = (currentQuaternion.cosine * lastQuaternion.cosine)
                         - glm::dot(currentQuaternion.axis, lastQuaternion.axis);
    
    
    glm::vec3 temporaryVector;

    temporaryVector = glm::cross(currentQuaternion.axis, lastQuaternion.axis);
    

    rotationalAxis_2.x = (currentQuaternion.cosine * lastQuaternion.axis.x) + 
                            (lastQuaternion.cosine * currentQuaternion.axis.x ) +
                            temporaryVector.x;

    rotationalAxis_2.y = (currentQuaternion.cosine * lastQuaternion.axis.y) + 
                            (lastQuaternion.cosine * currentQuaternion.axis.y ) +
                            temporaryVector.y;

    rotationalAxis_2.z = (currentQuaternion.cosine * lastQuaternion.axis.z) + 
                            (lastQuaternion.cosine * currentQuaternion.axis.z ) +
                            temporaryVector.z;
    
    angle = (acos(cosValue_2) * 180 / 3.1416) * 2;

    rotationalAxis.x = rotationalAxis_2.x / sin((angle / 2) * 3.1416 / 180);
    rotationalAxis.y = rotationalAxis_2.y / sin((angle / 2) * 3.1416 / 180);
    rotationalAxis.z = rotationalAxis_2.z / sin((angle / 2) * 3.1416 / 180);
}

void Orbital::replace(){
    lastQuaternion.cosine = cosValue_2;
    lastQuaternion.axis = rotationalAxis_2;
}
