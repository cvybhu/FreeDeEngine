#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>


class FreeCam
{
public:
    FreeCam(glm::vec3 position = {0, 0, 0}, glm::vec2 rotation = {0, 0}, float speed = 10);

    glm::vec3 pos;
    glm::vec2 rot;  //yaw pitch

    float fov;

    float sensitivity;
    float speed;



    void handleCameraRot(glm::vec2 mouseDelta);
    void handleMovement(float deltaTime);

    void moveForward(float deltaTime);
    void moveBack(float deltaTime);
    void moveRight(float deltaTime);
    void moveLeft(float deltaTime);
    void moveUp(float deltaTime);
    void moveDown(float deltaTime);


//openGL stuff
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
};
