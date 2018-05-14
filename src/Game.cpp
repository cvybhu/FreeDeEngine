#include <Render.hpp>
#include <Utils.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <iostream>


namespace Game
{
    std::vector<PointLight> pointLights;

    FreeCam cam;


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

    void update(float deltaTime)
    {
        cam.handleMovement(deltaTime);

        pointLights[0].pos = glm::rotate(pointLights[0].pos, deltaTime, glm::vec3(0, 0, 1));
        pointLights[1].pos = glm::rotate(pointLights[1].pos, deltaTime, glm::vec3(1, 1, 0));


        if(Window::isPressed(GLFW_KEY_P))
            std::cout << "Camera pos: (" << cam.pos.x << ' ' << cam.pos.y << ' ' << cam.pos.z << "\n";
    }



    void loadPointLightsToShader(std::vector<PointLight>& lights, Shader& shader)
    {
        shader.set1Int("pointLightsNum", lights.size());

        for(unsigned i = 0; i < lights.size(); i++)
        {
            auto& light = lights[i];
            std::string&& iAsString = convert2String(i);

            shader.setVec3(("pointLights[" + iAsString + "].pos").c_str(), light.pos);
            shader.setVec3(("pointLights[" + iAsString + "].color").c_str(), light.color);
            shader.set1Float(("pointLights[" + iAsString + "].constant").c_str(), light.constant);
            shader.set1Float(("pointLights[" + iAsString + "].linear").c_str(), light.linear);
            shader.set1Float(("pointLights[" + iAsString + "].quadratic").c_str(), light.quadratic);
        }
    }


    void drawLights(std::vector<PointLight>& lights, glm::mat4& projectionMatrix, glm::mat4& viewMatrix)
    {
        Shader& allWhite = Storage::getShader("src/shaders/allWhite");;

        allWhite.use();

        allWhite.setMat4("projection", projectionMatrix);
        allWhite.setMat4("view", viewMatrix);

        Mesh& lightMesh = Storage::getMesh("mesh/light.obj");
        glBindVertexArray(lightMesh.VAO);

        for(PointLight& light : lights)
        {
            allWhite.setMat4("model", glm::translate(glm::mat4(1), light.pos));
            glDrawArrays(GL_TRIANGLES, 0, lightMesh.vertsNum);
        }

    }


    void draw()
    {
        glm::mat4 projectionMatrix = cam.getProjectionMatrix();
        glm::mat4 viewMatrix = cam.getViewMatrix();

        drawLights(pointLights, projectionMatrix, viewMatrix);


        Shader& lightShader = Storage::getShader("src/shaders/light");

        lightShader.use();


        loadPointLightsToShader(pointLights, lightShader);

        lightShader.setMat4("view", viewMatrix);
        lightShader.setMat4("projection", projectionMatrix);

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
        lightShader.setMat4("model", glm::rotate(glm::mat4(1), (float)glm::radians(90.0), glm::vec3(0, 0, 1)));
        glDrawArrays(GL_TRIANGLES, 0, planeMesh.vertsNum);



        Mesh& stonePlace = Storage::getMesh("mesh/stonePlace.obj");

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, stonePlace.diffTexture->glIndx);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, stonePlace.specTexture->glIndx);

        glBindVertexArray(stonePlace.VAO);
        lightShader.setMat4("model", glm::translate(glm::mat4(1), glm::vec3(0, -12, 0)));
        glDrawArrays(GL_TRIANGLES, 0, stonePlace.vertsNum);



        Mesh& grass = Storage::getMesh("mesh/grass.obj");

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grass.diffTexture->glIndx);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, grass.specTexture->glIndx);

        glBindVertexArray(grass.VAO);

        for(glm::vec3 pos : {glm::vec3(-3, -8, -1), glm::vec3(-2, -7, -1), glm::vec3(-4, -6.3, -1)})
        {
            lightShader.setMat4("model", glm::translate(glm::mat4(1), pos));
            glDrawArrays(GL_TRIANGLES, 0, grass.vertsNum);
        }
    }
}
