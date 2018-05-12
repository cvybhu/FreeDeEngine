#include <Renderer.hpp>
#include <glad/glad.h>
#include <Storage.hpp>
#include <Logger.hpp>
#include <string>

void Renderer::draw(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Shader& light = Storage::shaders[shaderNames::light];
    light.use();

    glActiveTexture(GL_TEXTURE0);


    light.setMat4("view", viewMatrix);
    light.setMat4("projection", projectionMatrix);

    light.setVec3("ambientLight", ambientLight);

    glm::vec3 viewPos = glm::inverse(viewMatrix) * glm::vec4(0, 0, 0, 1);
    light.setVec3("viewPos", viewPos);


    loadLights(light);

    for(int meshIndex = 0; meshIndex <= MESH_MAX_COUNT; meshIndex++)
    {
        std::vector<Sprite3D*>& sprites = meshes[meshIndex];

        if(sprites.empty())
            continue;

        Mesh& mesh = Storage::meshes[meshIndex];

        glBindTexture(GL_TEXTURE_2D, mesh.myTexture->getGLindx());
        glBindVertexArray(mesh.getVAO());

        for(Sprite3D* sprite : sprites)
        {
            light.setMat4("model", sprite->model);
            glDrawArrays(GL_TRIANGLES, 0, mesh.getVertsNum());
        }
    }
}



void Renderer::loadLights(Shader& shader)
{
    shader.set1Int("pointLightsNum", pointLights.size());

    for(int i = 0; i < pointLights.size(); i++)
    {
        PointLight& light = *pointLights[i];

        shader.setVec3(("pointLights[" + std::to_string(i) + "].pos").c_str(), light.pos);
        shader.setVec3(("pointLights[" + std::to_string(i) + "].color").c_str(), light.color);
        shader.set1Float(("pointLights[" + std::to_string(i) + "].constant").c_str(), light.constant);
        shader.set1Float(("pointLights[" + std::to_string(i) + "].linear").c_str(), light.linear);
        shader.set1Float(("pointLights[" + std::to_string(i) + "].quadratic").c_str(), light.quadratic);
    }
}
