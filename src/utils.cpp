#include "utils.hpp"


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace utils
{
    glm::mat4 getYawPitchRollMat(float yaw, float pitch, float roll)
    {
        glm::mat4 res(1);

        glm::vec3 up(0, 0, 1);
        glm::vec3 front(0, -1, 0);
        glm::vec3 right = glm::cross(front, up);


        //yaw
        front = glm::rotate(front, yaw, up);
        right = glm::rotate(right, yaw, up);

        //pitch
        front = glm::rotate(front, pitch, right);

        res = glm::rotate(res, roll, front); //roll
        res = glm::rotate(res, pitch, right); //pitch
        res = glm::rotate(res, yaw, up);        //yaw

        return res;
    }
}
