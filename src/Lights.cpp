#include <Lights.hpp>
#include <glm/gtc/matrix_transform.hpp>

void PointLight::setupShadow(int resolution, float farPlane)
{
    shadow.resolution = resolution;
    shadow.farPlane = farPlane;

    glGenFramebuffers(1, &shadow.fbuff);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow.fbuff);


    glGenTextures(1, &shadow.cubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadow.cubeMap);

    for(int i = 0; i < 6; i++)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadow.resolution, shadow.resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow.cubeMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

void PointLight::setupShadowRendering(Shader& shader)
{
    glBindFramebuffer(GL_FRAMEBUFFER, shadow.fbuff);
    glViewport(0, 0, shadow.resolution, shadow.resolution);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);


    float aspect = 1.f;
    float near = 0.1f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, shadow.farPlane);


    shader.setMat4("shadowMats[0]", shadowProj * glm::lookAt(pos, pos + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
    shader.setMat4("shadowMats[1]", shadowProj * glm::lookAt(pos, pos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
    shader.setMat4("shadowMats[2]", shadowProj * glm::lookAt(pos, pos + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    shader.setMat4("shadowMats[3]", shadowProj * glm::lookAt(pos, pos + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
    shader.setMat4("shadowMats[4]", shadowProj * glm::lookAt(pos, pos + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
    shader.setMat4("shadowMats[5]", shadowProj * glm::lookAt(pos, pos + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));    

    shader.setVec3("lightPos", pos);
    shader.set1Float("farPlane", shadow.farPlane);
}

//void PointLight::destroyShadow(){}

void PointLight::activateShadow()
{
    shadow.active = true;
}

void PointLight::deactivateShadow()
{
    shadow.active = false;
}



void DirLight::setupShadow(glm::vec2 viewSize, glm::vec3 center, float farPlane, glm::ivec2 resolution)
{
    shadow.resolution = resolution;
    shadow.center = center;
    shadow.farPlane = farPlane;
    shadow.viewSize = viewSize;

    glGenFramebuffers(1, &shadow.fbuff);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow.fbuff);

    glGenTextures(1, &shadow.depth);
    glBindTexture(GL_TEXTURE_2D, shadow.depth);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow.resolution.x, shadow.resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow.depth, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}


void DirLight::setupShadowRendering(Shader& shader)
{
    glBindFramebuffer(GL_FRAMEBUFFER, shadow.fbuff);
    glViewport(0, 0, shadow.resolution.x, shadow.resolution.y);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glClear(GL_DEPTH_BUFFER_BIT);

    shader.setMat4("lightTrans", getLightSpaceMat());
}


glm::mat4 DirLight::getLightSpaceMat()
{
    float near = 0.1f;

    glm::mat4 lightProjection = glm::ortho(-shadow.viewSize.x/2, shadow.viewSize.x/2, -shadow.viewSize.y/2, shadow.viewSize.y/2, near, shadow.farPlane);

    glm::vec3 side = glm::cross(dir, glm::vec3(0, 0, 1));
    glm::vec3 up = glm::cross(side, dir);
    if(up.z < 0)up = -up;

    glm::mat4 lightView = glm::lookAt(shadow.center - dir*shadow.farPlane/2.f, shadow.center, up);

    return lightProjection * lightView;
}


void DirLight::activateShadow()
{
    shadow.active = true;
}