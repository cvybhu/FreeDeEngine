#include <Render.hpp>
#include <Physics.hpp>
#include <Utils.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <iostream>


namespace Game
{
    std::vector<PointLight> pointLights;

    FreeCam cam;


    GLuint framebuffer;
    GLuint texColorBuffer;
    GLuint depthStencRenderBuff;
    GLuint screenQuadVBO, screenQuadVAO;



    void initFramebuffer()
    {
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glGenTextures(1, &texColorBuffer);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window::width, Window::height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //glBindTexture(GL_TEXTURE_2D, 0); //is this really necessary?

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

        glGenRenderbuffers(1, &depthStencRenderBuff);
        glBindRenderbuffer(GL_RENDERBUFFER, depthStencRenderBuff);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Window::width, Window::height);
        //glBindRenderbuffer(GL_RENDERBUFFER, 0); //is this really necessary?

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencRenderBuff);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Failed to setup framebuffer!!!\n";

        //glBindFramebuffer(GL_FRAMEBUFFER, 0);

        float screenQuadVerts[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
        };

        glGenVertexArrays(1, &screenQuadVAO);
        glGenBuffers(1, &screenQuadVBO);
        glBindVertexArray(screenQuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVerts), &screenQuadVerts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }


    void init()
    {
        initFramebuffer();

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

        PhysicsEntity lightPhys;
        lightPhys.position = light.pos;
        lightPhys.linearVelocity = glm::fvec3(0.3, 0.2, 0.01);
        lightPhys.linearAcceleration = glm::fvec3(0.0, 0.8, 0.0);
        lightPhys.entityID = 0;
        physicsEntities.emplace_back(lightPhys);

        PhysicsEntity lightPhys2;
        lightPhys2.position = light2.pos;
        lightPhys2.linearVelocity = glm::fvec3(0.3, 0.2, 0.51);
        lightPhys2.linearAcceleration = glm::fvec3(0.0, 0.8, 0.0);
        lightPhys2.entityID = 1;
        physicsEntities.emplace_back(lightPhys2);
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


    void draw(const GLdouble deltaTime)
    {
        glm::mat4 projectionMatrix = cam.getProjectionMatrix();
        glm::mat4 viewMatrix = cam.getViewMatrix();

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);


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

        for (GLuint i = 0; i<pointLights.size(); i++)
        {
            updatePosition(pointLights[i].pos, physicsEntities[i], deltaTime);
        }

        drawLights(pointLights, projectionMatrix, viewMatrix);


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


        Shader& postProcess = Storage::getShader("src/shaders/postProcessTest");

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        postProcess.use();
        glBindVertexArray(screenQuadVAO);
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}
