#pragma once
#include <Render.hpp>
#include <glm/glm.hpp>

struct PhysicsEntity
{
    glm::fvec3 position;
    glm::fvec3 linearVelocity;
    glm::fvec3 linearAcceleration;

    void update(const double dt);                                       

private:

    glm::fmat4 getMovementMatrix(const double dt);

    void updateLinearVelocity(const double dt);   
   
    void updatePosition(const double dt);      
};
