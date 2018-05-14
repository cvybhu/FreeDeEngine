#pragma once
#include <Render.hpp>
#include <glm/glm.hpp>

struct PhysicsEntity
{
    glm::fvec3 position;
    glm::fvec3 linearVelocity;
    glm::fvec3 linearAcceleration;

    GLuint entityID;//NOTE(Stanisz): not used yet,
                    // we need to talk about some way of identifying all the entities.
                    // using mesh doesnt work when a mesh is used multiple times,
                    // think boi. for now you can declare a list of entities and identify 
                    // their physics entities by the index in the list.
                    // for example: vector<ass> asses;
                    // vector<PhysicsEntity> assesPhysics;
                    // asses[i] ~ entities[i];
                    // this is maybe even cool (?). i like it xd maybe we wont need to
                    // upgrade.

    glm::fmat4 getMovementMatrix(const GLdouble dt); // returns a matrix that will translate
                                                     // position when multiplied by vectors corresponding to
                                                     // acceleration and velocity.
                                                     // also updates THIS entity position
};

void updatePosition(glm::fvec3& oldPosition,
                    PhysicsEntity& correspondingEntity,
                    const GLdouble dt); //multiplies oldPosition with movementMatrix