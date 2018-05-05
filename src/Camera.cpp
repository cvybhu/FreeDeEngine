#include "Camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::getViewMatrix()
{
    //copy paste from learnOpenGL for now :C lets make the system working first

    glm::vec3 front;
    front.x = cos(glm::radians(rot.x)) * cos(glm::radians(rot.y));
    front.y = sin(glm::radians(rot.y));
    front.z = sin(glm::radians(rot.x)) * cos(glm::radians(rot.x));
    glm::vec3 Front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    glm::vec3 Right = glm::normalize(glm::cross(Front, glm::vec3(0, 0, 1)));
    glm::vec3 Up    = glm::normalize(glm::cross(Right, Front));

    return glm::lookAt(pos, pos + Front, Up);
}
