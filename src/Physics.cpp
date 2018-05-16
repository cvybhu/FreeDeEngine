#include <Physics.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 PhysicsEntity::getMovementMatrix(const double dt)
{
    glm::mat4 res(1.0f);
    position = position + linearVelocity * (float)dt +
                linearAcceleration * (float)dt * (float)dt;
    
    res = res * glm::translate(res, position);

    return res;
}

void PhysicsEntity::updateLinearVelocity(const double dt)
{
    linearVelocity += (float)dt * linearAcceleration;
}

void PhysicsEntity::updatePosition(const double dt)
{
    glm::fvec4 posVec4 = {position.x, position.y, position.z, 1.0f};
    position = getMovementMatrix(dt) * posVec4;
}

void PhysicsEntity::update(const double dt)
{
    updateLinearVelocity(dt);
    updatePosition(dt);
}