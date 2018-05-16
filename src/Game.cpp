#include <Render.hpp>
#include <Physics.hpp>
#include <Utils.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <iostream>


namespace Game
{
    std::vector<PointLight> pointLights;
    std::vector<DirLight> dirLights;
    std::vector<PhysicsEntity> physicsEntities;

    FreeCam cam;


    GLuint framebuffer;
    GLuint texColorBuffer;
    GLuint depthStencRenderBuff;
    GLuint screenQuadVAO, screenQuadVBO;

    GLuint skyboxIndex;
    GLuint skyboxVAO, skyboxVBO;


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


    void initSkyboxVAO()
    {
        float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
        };

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }


    void init()
    {
        initFramebuffer();
        initSkyboxVAO();

        CubeTexture& skyboxMountLake = Storage::getCubeTex("src/tex/mountainsCube");

        skyboxMountLake.loadToRAM("tex/mountainsCube");
        skyboxMountLake.loadToGPU();

        skyboxIndex = skyboxMountLake.glIndx;

        DirLight sun;
        sun.color = {1, 1, 0};
        sun.dir = {-0.2f, -1.0f, -0.3f};

        dirLights.emplace_back(sun);

        PointLight light;
        light.pos = {5, 5, 5};
        light.color = glm::vec3(1.0, 0.2, 0.5);
        light.constant = 1.f;
        light.linear = 0.05f;
        light.quadratic = 0.004f;

        PointLight light2;
        light2.pos = {3, 1, 4};
        light2.color = glm::vec3(0.3, 0.4, 0.9);
        light2.constant = 1.f;
        light2.linear = 0.07f;
        light2.quadratic = 0.004f;

        pointLights.emplace_back(light); pointLights.emplace_back(light2);

        PhysicsEntity lightPhys;
        lightPhys.position = light.pos;
        lightPhys.linearVelocity = glm::fvec3(0.3, 0.2, 0.01);
        lightPhys.linearAcceleration = glm::fvec3(0.0, 0.8, 0.0);
        physicsEntities.emplace_back(lightPhys);

        PhysicsEntity lightPhys2;
        lightPhys2.position = light2.pos;
        lightPhys2.linearVelocity = glm::fvec3(0.3, 0.2, 0.51);
        lightPhys2.linearAcceleration = glm::fvec3(0.0, 0.8, 0.0);
        physicsEntities.emplace_back(lightPhys2);
    }

    void update(const GLdouble deltaTime)
    {
        cam.handleMovement(deltaTime);

        for (GLuint i = 0; i<pointLights.size(); i++)
        {
            PhysicsEntity& current = physicsEntities[i];
            current.update(deltaTime);

            //pointLights[i].pos = current.position;
        }

        if(Window::isPressed(GLFW_KEY_P))
            std::cout << "Camera pos: (" << cam.pos.x << ' ' << cam.pos.y << ' ' << cam.pos.z << "\n";
    }


    void loadPointLightsToShader(std::vector<PointLight>& lights, Shader& shader)
    {
        shader.set1Int("pointLightsNum", (int)lights.size());

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

    void loadDirLightsToShader(std::vector<DirLight>& lights, Shader& shader)
    {
        shader.set1Int("dirLightsNum", lights.size());

        for(unsigned i = 0; i < lights.size(); i++)
        {
            auto& light = lights[i];
            std::string&& iAsString = convert2String(i);

            shader.setVec3(("dirLights[" + iAsString + "].color").c_str(), light.color);
            shader.setVec3(("dirLights[" + iAsString + "].dir").c_str(), light.dir);
        }
    }


    void drawLights(std::vector<PointLight>& lights, glm::mat4& projectionMatrix, glm::mat4& viewMatrix)
    {
        Shader& oneColor = Storage::getShader("src/shaders/oneColor");

        oneColor.use();

        oneColor.setMat4("projection", projectionMatrix);
        oneColor.setMat4("view", viewMatrix);

        Mesh& lightMesh = Storage::getMesh("mesh/light.obj");
        glBindVertexArray(lightMesh.VAO);

        for(PointLight& light : lights)
        {
            oneColor.setVec3("color", glm::normalize(light.color));
            oneColor.setMat4("model", glm::translate(glm::mat4(1), light.pos));
            glDrawArrays(GL_TRIANGLES, 0, lightMesh.vertsNum);
        }

    }

    void drawSkybox(glm::mat4& viewMatrix, glm::mat4& projectionMatrix)
    {
        Shader& skyboxShader = Storage::getShader("src/shaders/skybox");

        glDepthMask(GL_FALSE);
        skyboxShader.use();

        skyboxShader.setMat4("view", glm::mat3(viewMatrix));
        skyboxShader.setMat4("projection", projectionMatrix);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxIndex);

        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

    }

    void setupMeshForDraw(const Mesh& mesh)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh.diffTexture->glIndx);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mesh.specTexture->glIndx);

        glBindVertexArray(mesh.VAO);

    }

    void drawMesh(const Mesh& mesh, const glm::mat4& modelMatrix, Shader& shader)
    {
        shader.setMat4("model", modelMatrix);
        glDrawArrays(GL_TRIANGLES, 0, mesh.vertsNum);
    }

    void draw()
    {
        glm::mat4 projectionMatrix = cam.getProjectionMatrix();
        glm::mat4 viewMatrix = cam.getViewMatrix();

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glClearColor(1, 1, 1, 1);

        drawSkybox(viewMatrix, projectionMatrix);


        Shader& lightUseShader = Storage::getShader("src/shaders/light");

        lightUseShader.use();


        loadPointLightsToShader(pointLights, lightUseShader);
        loadDirLightsToShader(dirLights, lightUseShader);


        lightUseShader.setMat4("view", viewMatrix);
        lightUseShader.setMat4("projection", projectionMatrix);

        lightUseShader.setVec3("ambientLight", glm::vec3(0.3));
        lightUseShader.setVec3("viewPos", cam.pos);

        lightUseShader.set1Int("diffTexture", 0);
        lightUseShader.set1Int("specTexture", 1);


        Mesh& planeMesh = Storage::getMesh("mesh/spacePlane.obj");
        setupMeshForDraw(planeMesh);
        drawMesh(planeMesh, glm::rotate(glm::mat4(1), glm::radians(90.f), glm::vec3(0, 0, 1)), lightUseShader);


        Mesh& stonePlace = Storage::getMesh("mesh/stonePlace.obj");
        setupMeshForDraw(stonePlace);
        glm::mat4 stonePlaceModel = glm::translate(glm::mat4(1), glm::vec3(0, -12, 0));
        drawMesh(stonePlace, stonePlaceModel, lightUseShader);



        Mesh& grass = Storage::getMesh("mesh/grass.obj");
        setupMeshForDraw(grass);
        for(glm::vec3 pos : {glm::vec3(-3, -8, -1), glm::vec3(-2, -7, -1), glm::vec3(-4, -6.3, -1)})
            drawMesh(grass, glm::translate(glm::mat4(1), pos), lightUseShader);


        drawLights(pointLights, projectionMatrix, viewMatrix);


        Shader& postProcess = Storage::getShader("src/shaders/postProcessTest");

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClear(GL_COLOR_BUFFER_BIT);

        postProcess.use();
        glBindVertexArray(screenQuadVAO);
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}
