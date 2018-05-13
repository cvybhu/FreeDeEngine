#include <Render.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace Storage
{
    Texture& getTex(const std::string filePath);
    Mesh& getMesh(const std::string filePath);
    Shader& getShader(const std::string filePath);
}






void loadPointLightsToShader(const std::vector<PointLight>& pointLights, Shader& shader)
{
    //shader.set1Int("pointLightsNum", pointLights.size());

    for(unsigned int i = 0; i < pointLights.size(); i++)
    {
        const PointLight& light = pointLights[i];

        shader.setVec3(("pointLights[" + std::to_string(i) + "].pos").c_str(), light.pos);
        shader.setVec3(("pointLights[" + std::to_string(i) + "].color").c_str(), light.color);
        shader.set1Float(("pointLights[" + std::to_string(i) + "].constant").c_str(), light.constant);
        shader.set1Float(("pointLights[" + std::to_string(i) + "].linear").c_str(), light.linear);
        shader.set1Float(("pointLights[" + std::to_string(i) + "].quadratic").c_str(), light.quadratic);
    }
}


void drawLights(std::vector<PointLight>& pointLights, glm::mat4& projectionMatrix, glm::mat4& viewMatrix)
{
    Shader& allWhite = Storage::getShader("src/shaders/allWhite");;

    allWhite.use();

    allWhite.setMat4("projection", projectionMatrix);
    allWhite.setMat4("view", viewMatrix);

    Mesh& lightMesh = Storage::getMesh("mesh/light.obj");
    glBindVertexArray(lightMesh.VAO);


    for(PointLight& light : pointLights)
    {
        allWhite.setMat4("model", glm::translate(glm::mat4(1), light.pos));
        glDrawArrays(GL_TRIANGLES, 0, lightMesh.vertsNum);
    }

}





namespace Game
{
    std::vector<PointLight> pointLights;

    FreeCam cam;

    void draw()
    {
        glm::mat4 projectionMatrix = cam.getProjectionMatrix();
        glm::mat4 viewMatrix = cam.getViewMatrix();

        Shader& lightShader = Storage::getShader("src/shaders/light");

        lightShader.use();
        loadPointLightsToShader(pointLights, lightShader);

        lightShader.set1Int("pointLightsNum", pointLights.size());

        lightShader.setMat4("view", viewMatrix);
        lightShader.setMat4("projection", projectionMatrix);
        lightShader.setMat4("model", glm::mat4(1));

        lightShader.setVec3("ambientLight", glm::vec3(0.3));

        lightShader.setVec3("viewPos", cam.pos);

        lightShader.set1Int("diffTexture", 0);
        lightShader.set1Int("specTexture", 1);

        Mesh& planeMesh = Storage::getMesh("mesh/spacePlane.obj");

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, planeMesh.diffTexture->glIndx);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, planeMesh.specTexture->glIndx);

        glBindVertexArray(planeMesh.VAO);
        glDrawArrays(GL_TRIANGLES, 0, planeMesh.vertsNum);

        drawLights(pointLights, projectionMatrix, viewMatrix);
    }






    void init()
    {
        PointLight light;
        light.pos = {5, 5, 5};
        light.color = glm::vec3(1.0);
        light.constant = 1.f;
        light.linear = 0.05f;
        light.quadratic = 0.004f;

        PointLight light2;
        light2.pos = {3, 1, 4};
        light2.color = glm::vec3(0.0);
        light2.constant = 1.f;
        light2.linear = 0.07f;
        light2.quadratic = 0.004f;

        pointLights.emplace_back(light);
        pointLights.emplace_back(light2);
    }
}
