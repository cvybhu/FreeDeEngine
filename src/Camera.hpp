#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>


struct FreeCam
{
    FreeCam(glm::vec3 position = {0, 0, 0}, glm::vec2 rotation = {0, 0}, float speed = 15);

    glm::vec3 pos;
    glm::vec2 rot;  //yaw pitch (in radians)

    float fov;  //in degrees

    float sensitivity;
    float speed;
    float fastSpeed;


    void handleCameraRot(glm::vec2 mouseDelta);
    void handleMovement(float deltaTime);

//openGL stuff
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
};