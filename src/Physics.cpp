#include <Physics.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 PhysicsEntity::getMovementMatrix(const GLdouble dt)
{
    glm::mat4 res(1.0f);
    position = position + linearVelocity * (GLfloat)dt +
                linearAcceleration * (GLfloat)dt * (GLfloat)dt;
    
    res = res * glm::translate(res, position);

    return res;
}

void updatePosition(glm::fvec3& oldPosition,
                    PhysicsEntity& correspondingEntity,
                    const GLdouble dt)
{
    oldPosition = correspondingEntity.getMovementMatrix(dt) * 
                  (glm::fvec4)(oldPosition, 1.0f);
}