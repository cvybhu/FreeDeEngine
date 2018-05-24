#include <Camera.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <Window.hpp>


FreeCam::FreeCam(glm::vec3 position, glm::vec2 rotation, float Speed)
{
    pos = position;
    rot = rotation;
    speed = Speed;
    fastSpeed = speed*5;
    fov = 45.0f;
    sensitivity = 0.003;
}


void FreeCam::handleCameraRot(glm::vec2 mouseDelta)
{
    rot[0] -= mouseDelta.x*sensitivity;
    rot[1] += mouseDelta.y*sensitivity;

    float almost90 = glm::radians(89.5);
    if(rot[1] < -almost90)rot[1] = -almost90;
    if(rot[1] > almost90)rot[1] = almost90;
}



void FreeCam::handleMovement(float deltaTime)
{
    glm::vec3 forward = glm::rotate(glm::vec3(0, -1, 0), rot[0], glm::vec3(0, 0, 1));
    glm::vec3 right = glm::rotate(glm::vec3(-1, 0, 0), rot[0], glm::vec3(0, 0, 1));
    glm::vec3 up = glm::vec3(0, 0, 1);

    glm::vec3 curMove = {0, 0, 0};

    if(Window::isPressed(GLFW_KEY_W))curMove += forward;
    if(Window::isPressed(GLFW_KEY_S))curMove -= forward;
    if(Window::isPressed(GLFW_KEY_A))curMove -= right;
    if(Window::isPressed(GLFW_KEY_D))curMove += right;
    if(Window::isPressed(GLFW_KEY_SPACE))curMove += up;
    if(Window::isPressed(GLFW_KEY_LEFT_SHIFT))curMove -= up;

    float curSpeed = (Window::isPressed(GLFW_KEY_LEFT_CONTROL) ? fastSpeed : speed);

    if(curMove != glm::vec3(0, 0, 0))
    {
        curMove = glm::normalize(curMove);
        curMove *= deltaTime * curSpeed;
        pos += curMove;
    }

    handleCameraRot(Window::mouseDelta);
}


glm::mat4 FreeCam::getViewMatrix()
{
    glm::vec3 up(0, 0, 1);
    glm::vec3 front(0, -1, 0);
    glm::vec3 right = glm::cross(up, front);

    front = glm::rotate(front, rot[0], up); //yaw
    right = glm::rotate(right, rot[0], up);

    front = glm::rotate(front, rot[1], right); //pitch
    up = glm::rotate(up, rot[1], right);

    return glm::lookAt(pos, pos + front, up);
}


glm::mat4 FreeCam::getProjectionMatrix()
{
    return glm::perspective(glm::radians(fov), (float)Window::width / (float)Window::height, 0.1f, 500.0f);
}
