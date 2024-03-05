#include "Camera.h"

Camera::Camera(glm::vec3 camera_position, glm::vec3 camera_target, glm::vec3 camera_up) {
    this->camera_position = camera_position;
    this->camera_target = camera_target;
    this->camera_up = camera_up;
}

glm::vec3 Camera::getPosition(){
    return this->camera_position;
}
glm::vec3 Camera::getTarget(){
    return this->camera_target;
}
glm::vec3 Camera::getUp(){
    return this->camera_up;
}

void Camera::setTarget(glm::vec3 camera_target){
    this->camera_target = camera_target;
}