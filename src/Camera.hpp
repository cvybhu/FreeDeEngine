#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

//#include <GLFW/glfw3.h>


class Camera
{
public:
    glm::vec3 pos;
    glm::vec3 rot;  //yaw pitch roll

    float fov;

    float sensitivity;
    float speed;


    void handleMovement(float deltaTime);


    void moveForward(float deltaTime);
    void moveBack(float deltaTime);
    void moveRight(float deltaTime);
    void moveLeft(float deltaTime);
    void moveUp(float deltaTime);
    void moveDown(float deltaTime);

//openGL stuff
    glm::mat4 getViewMatrix();


};
