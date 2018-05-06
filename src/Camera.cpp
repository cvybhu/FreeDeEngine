#include "Camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "utils.hpp"

glm::mat4 Camera::getViewMatrix()
{
    glm::vec3 up(0, 0, 1);
    glm::vec3 front(0, -1, 0);
    glm::vec3 right = glm::cross(up, front);

    front = glm::rotate(front, rot[0], up); //yaw
    right = glm::rotate(right, rot[0], up);


    front = glm::rotate(front, rot[1], right); //pitch
    up = glm::rotate(up, rot[1], right);

    up = glm::rotate(up, rot[2], front); //roll

    return glm::lookAt(pos, pos + front, up);
}


void Camera::moveForward(float deltaTime)
{
    pos += speed*deltaTime*glm::rotate(glm::vec3(0, -1, 0), rot[0], glm::vec3(0, 0, 1));
}

void Camera::moveBack(float deltaTime)
{
    moveForward(-deltaTime);
}


void Camera::moveRight(float deltaTime)
{
    pos += speed*deltaTime*glm::rotate(glm::vec3(-1, 0, 0), rot[0], glm::vec3(0, 0, 1));
}

void Camera::moveLeft(float deltaTime)
{
    moveRight(-deltaTime);
}

void Camera::moveUp(float deltaTime)
{
    pos.z += speed*deltaTime;
}

void Camera::moveDown(float deltaTime)
{
    moveUp(-deltaTime);
}

#include "WindowManager.hpp"

namespace Globals{extern WindowManager window;}

#include <iostream>
using namespace std;

void Camera::handleMovement(float deltaTime)
{
    rot[0] -= Globals::window.getMouseDeltaX()*0.005;

    rot[1] -= Globals::window.getMouseDeltaY()*0.005;

    float almost90 = glm::radians(89.5);
    if(rot[1] < -almost90)rot[1] = -almost90;
    if(rot[1] > almost90)rot[1] = almost90;

    if(Globals::window.isPressed(GLFW_KEY_W))moveForward(deltaTime);
    if(Globals::window.isPressed(GLFW_KEY_S))moveBack(deltaTime);
    if(Globals::window.isPressed(GLFW_KEY_A))moveLeft(deltaTime);
    if(Globals::window.isPressed(GLFW_KEY_D))moveRight(deltaTime);
    if(Globals::window.isPressed(GLFW_KEY_SPACE))moveUp(deltaTime);
    if(Globals::window.isPressed(GLFW_KEY_LEFT_SHIFT))moveDown(deltaTime);
}




