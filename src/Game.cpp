#include <Game.hpp>

#include <Window.hpp>
#include <Storage.hpp>

#include <Renderer.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Camera.hpp>
#include <Logger.hpp>
#include <Utils.hpp>

namespace Game
{
    std::vector<PointLight> pointLights;
    DirLight sun;

    Renderer render;
    FreeCam cam;


    GLuint postProcFramebuff;   //not multisampled buff for postprocessing
    GLuint postProcFramebuffColorTex;
    GLuint postProcBloomTex;

    const GLuint MSAASamples = 4;
    GLuint multiSampleFramebuff; //main render buff
    GLuint multiSampleColorTex;
    GLuint multiSampleBloomTex;
    GLuint depthStencMultisampleRenderbuff;

    GLuint screenQuadVAO, screenQuadVBO;

    GLuint skyboxIndex;
    GLuint skyboxVAO, skyboxVBO;


    GLuint defaultDiffTex; //also specular
    GLuint defaultNormalTex;
    GLuint defaultDispTex;
    GLuint defaultAmbientOccTex;

    glm::ivec2 shadowRes = glm::ivec2(1024);

    GLuint dirLightShadowFBO;
    GLuint dirLightShadowDepth;
    glm::mat4 dirLightSpaceMat;

    PointLight shadowPointLight;
    GLuint shadowPLFBO;  //PL == point light
    GLuint shadowPLCubeMap;
    std::vector<glm::mat4> shadowPLTransforms;
    float shadowPLFarPlane;

    float exposure = 0.5;

    GLuint bloomFramebuffs[2];
    GLuint bloomColorTexs[2];

    glm::ivec2 bloomRes = {Window::width/4, Window::height/4};
    float bloomMinBrightness = 2.0;

    void initFramebuffer()
    {
        glEnable(GL_MULTISAMPLE);
        glGenFramebuffers(1, &multiSampleFramebuff);
        glBindFramebuffer(GL_FRAMEBUFFER, multiSampleFramebuff);

        glGenTextures(1, &multiSampleColorTex);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multiSampleColorTex);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAASamples, GL_RGB16F, Window::width, Window::height, GL_TRUE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //glBindTexture(GL_TEXTURE_2D, 0); //is this really necessary?

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multiSampleColorTex, 0);


        glGenTextures(1, &multiSampleBloomTex);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multiSampleBloomTex);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAASamples, GL_RGB16F, Window::width, Window::height, GL_TRUE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, multiSampleBloomTex, 0);



        glGenRenderbuffers(1, &depthStencMultisampleRenderbuff);
        glBindRenderbuffer(GL_RENDERBUFFER, depthStencMultisampleRenderbuff);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAASamples, GL_DEPTH24_STENCIL8, Window::width, Window::height);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencMultisampleRenderbuff);

        GLenum drawAttachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, drawAttachments);



        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Failed to setup multisampled framebuffer!!!\n";


        glGenFramebuffers(1, &postProcFramebuff);
        glBindFramebuffer(GL_FRAMEBUFFER, postProcFramebuff);

        glGenTextures(1, &postProcFramebuffColorTex);
        glBindTexture(GL_TEXTURE_2D, postProcFramebuffColorTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Window::width, Window::height, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcFramebuffColorTex, 0);



        //glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenFramebuffers(2, bloomFramebuffs);
        glGenTextures(2, bloomColorTexs);

        for(int i = 0; i < 2; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, bloomFramebuffs[i]);
            glBindTexture(GL_TEXTURE_2D, bloomColorTexs[i]);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, bloomRes.x, bloomRes.y, 0, GL_RGB, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomColorTexs[i], 0);
        }


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

    void initInstancedParticles()
    {
        glm::vec3 poss[] = {glm::vec3(0, 13, 0), glm::vec3(0, 14, 0), glm::vec3(0, 15, 0), glm::vec3(0, 16, 0), glm::vec3(0, 17, 0)};
        std::vector<glm::mat4> modelMats(sizeof(poss)/sizeof(glm::vec3));

        for(int i = 0; i < (int)modelMats.size(); i++)
            modelMats[i] = glm::translate(glm::mat4(1), poss[i]);

        Mesh& particle = Storage::getMesh("mesh/particle.obj");
        glBindVertexArray(particle.VAO);

        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, modelMats.size() * sizeof(glm::mat4), &modelMats[0], GL_STATIC_DRAW);
        // vertex Attributes
        auto vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
    }

    void initDirLightShadow()
    {
        glGenFramebuffers(1, &dirLightShadowFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, dirLightShadowFBO);

        glGenTextures(1, &dirLightShadowDepth);
        glBindTexture(GL_TEXTURE_2D, dirLightShadowDepth);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowRes.x, shadowRes.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dirLightShadowDepth, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        float near_plane = 1.0f, far_plane = 100.5f;
        float viewSize = 25.f;
        glm::mat4 lightProjection = glm::ortho(-viewSize, viewSize, -viewSize, viewSize, near_plane, far_plane);

        glm::vec3 center = glm::vec3(5, 0, 0);
        glm::vec3 side = glm::cross(sun.dir, glm::vec3(0, 0, 1));
        glm::vec3 up = glm::cross(side, sun.dir);
        if(up.z < 0)up = -up;

        glm::mat4 lightView = glm::lookAt(center - sun.dir*far_plane/2.f, center, up);
        dirLightSpaceMat = lightProjection * lightView;
    }

    void initShadowPointLight()
    {
        shadowPointLight.pos = glm::vec3(20.5, -1, -1);
        shadowPointLight.color = glm::vec3(2.8);//glm::vec3(1.0, 0.2, 0.5)*0.8f;
        shadowPointLight.constant = 1.f;
        shadowPointLight.linear = 0.1f;
        shadowPointLight.quadratic = 0.03f;

        glGenFramebuffers(1, &shadowPLFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowPLFBO);


        glGenTextures(1, &shadowPLCubeMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadowPLCubeMap);

        for(int i = 0; i < 6; i++)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowRes.x, shadowRes.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowPLCubeMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);


        float aspect = (float)shadowRes.x/(float)shadowRes.y;
        float near = 0.1f;
        shadowPLFarPlane = 25.f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, shadowPLFarPlane);

        auto lightPos = shadowPointLight.pos;

        shadowPLTransforms = {
            shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
            shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
            shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
            shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)),
            shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)),
            shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0))
        };
    }

    GLuint create1x1Texture(glm::ivec3 color)
    {
        GLuint tex;

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        char texData[] = {(char)color.x, (char)color.y, (char)color.z};

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);

        return tex;
    }


    void initDefaultTextures()
    {
        defaultDiffTex = create1x1Texture({128, 128, 128});
        defaultNormalTex = create1x1Texture({128, 128, 255});
        defaultDispTex = create1x1Texture({0, 0, 0});
        defaultAmbientOccTex = create1x1Texture({255, 255, 255});
    }

    Sprite3D* plane;

    void init()
    {
        render.setRenderRes({Window::width, Window::height});
        render.init(10);

        initFramebuffer();
        initSkyboxVAO();
        initInstancedParticles();
        initDefaultTextures();

        CubeTexture& skyboxMountLake = Storage::getCubeTex("src/tex/mountainsCube");
        skyboxMountLake.loadToRAM("tex/mountainsCube");
        skyboxMountLake.loadToGPU(true);
        render.currentSkybox = &skyboxMountLake;

        plane = render.addSprite3D(Storage::getMesh("mesh/spacePlane.obj"));
        plane->model = glm::rotate(glm::translate(glm::mat4(1), glm::vec3(4, -4, 2)), (float)glfwGetTime(), glm::vec3(0, 0, 1));

        Sprite3D* stonePlace = render.addSprite3D(Storage::getMesh("mesh/stonePlace.obj"));
        stonePlace->model = glm::translate(glm::mat4(1), glm::vec3(0, -12, 0));

        Sprite3D* pointShadowTest = render.addSprite3D(Storage::getMesh("mesh/pointShadowTest.obj"));
        pointShadowTest->model = glm::translate(glm::mat4(1), glm::vec3(20, 0, 0));

        PointLight* light = render.addPointLight();
        light->pos = {5, -5, -0.5};
        light->color = glm::vec3(1.0, 0.2, 0.5); light->color *= 8;
        light->constant = 1.f;
        light->linear = 0.5f;
        light->quadratic = 0.8f;

        PointLight* light2 = render.addPointLight();
        light2->pos = {-5, -10, 3};
        light2->color = glm::vec3(0.3, 0.4, 0.9); light2->color *= 2;
        light2->constant = 1.f;
        light2->linear = 0.07f;
        light2->quadratic = 0.04f;



        skyboxIndex = skyboxMountLake.glIndx;
        sun.color = glm::vec3(glm::vec2(0.5), 0.4) * 5.f;
        sun.dir = {0, -1.0f, -1.0f};

        initDirLightShadow();
        initShadowPointLight();

        //pointLights.emplace_back(light); pointLights.emplace_back(light2);
    }

    void exposureTesting(float deltaTime)
    {

        float exposureSensitivity = 0.5;
        if(Window::isPressed(GLFW_KEY_1))
        {
            exposure -= deltaTime * exposureSensitivity;
            if(exposure < 0)
                exposure = 0;
            std::cout << "Exposure: " << exposure << '\n';
        }

        if(Window::isPressed(GLFW_KEY_2))
        {
            exposure += deltaTime * exposureSensitivity;
            if(exposure > 1)
                exposure = 1;
            std::cout << "Exposure: " << exposure << '\n';
        }
    }

    void update(float deltaTime)
    {
        cam.handleMovement(deltaTime);

        exposureTesting(deltaTime);

        if(Window::isPressed(GLFW_KEY_P))
            std::cout << "Camera pos: (" << cam.pos.x << ' ' << cam.pos.y << ' ' << cam.pos.z << "\n";
    }


    void loadPointLightsToShader(std::vector<PointLight>& lights, Shader& shader)
    {
        shader.set1Int("pointLightsNum", (int)lights.size());

        shader.setVec3("shadowPointLight.pos", shadowPointLight.pos);
        shader.setVec3("shadowPointLight.color", shadowPointLight.color);
        shader.set1Float("shadowPointLight.constant", shadowPointLight.constant);
        shader.set1Float("shadowPointLight.linear", shadowPointLight.linear);
        shader.set1Float("shadowPointLight.quadratic", shadowPointLight.quadratic);

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

    /*
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
    */

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

        skyboxShader.set1Float("exposure", exposure);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxIndex);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, shadowPLCubeMap);

        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

    }

    void setupMeshForDraw(const Mesh& mesh)
    {
        glActiveTexture(GL_TEXTURE0);

        if(mesh.diffTex != nullptr)
            glBindTexture(GL_TEXTURE_2D, mesh.diffTex->glIndx);
        else
            glBindTexture(GL_TEXTURE_2D, defaultDiffTex);


        glActiveTexture(GL_TEXTURE1);

        if(mesh.specTex != nullptr)
            glBindTexture(GL_TEXTURE_2D, mesh.specTex->glIndx);
        else
            glBindTexture(GL_TEXTURE_2D, defaultDiffTex);

        glActiveTexture(GL_TEXTURE2);

        if(mesh.normalTex != nullptr)
            glBindTexture(GL_TEXTURE_2D, mesh.normalTex->glIndx);
        else
            glBindTexture(GL_TEXTURE_2D, defaultNormalTex);


        glActiveTexture(GL_TEXTURE3);
        if(mesh.dispTex != nullptr)
            glBindTexture(GL_TEXTURE_2D, mesh.dispTex->glIndx);
        else
            glBindTexture(GL_TEXTURE_2D, defaultDispTex);

        glActiveTexture(GL_TEXTURE4);
        if(mesh.ambientOccTex != nullptr)
            glBindTexture(GL_TEXTURE_2D, mesh.ambientOccTex->glIndx);
        else
            glBindTexture(GL_TEXTURE_2D, defaultAmbientOccTex);

        glBindVertexArray(mesh.VAO);
    }

    void drawMesh(const Mesh& mesh, const glm::mat4& modelMatrix, Shader& shader)
    {
        shader.setMat4("model", modelMatrix);
        glDrawArrays(GL_TRIANGLES, 0, mesh.vertsNum);
    }

    void blurTheBloom(int passes)
    {
        glDisable(GL_DEPTH_TEST);

        Shader& blurShader = Storage::getShader("src/shaders/gausBlur");
        blurShader.use();

        glBindVertexArray(screenQuadVAO);

        glViewport(0, 0, bloomRes.x, bloomRes.y);
        for(int i = 0; i < 2*passes; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, bloomFramebuffs[(i+1)%2]);

            blurShader.set1Int("isHorizontal", (i%2 == 0));
            //blurShader.set1Int("prevTex", 0)

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, bloomColorTexs[i%2]);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }


    void draw()
    {
        glm::mat4 projectionMatrix = cam.getProjectionMatrix();
        glm::mat4 viewMatrix = cam.getViewMatrix();

        render.draw(viewMatrix, projectionMatrix);
        return;

        //Dir Light Shadows
        glViewport(0, 0, shadowRes.x, shadowRes.y);
        glBindFramebuffer(GL_FRAMEBUFFER, dirLightShadowFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        Shader& dirLightShadow = Storage::getShader("src/shaders/dirLightShadow");
        dirLightShadow.use();

        dirLightShadow.setMat4("lightTrans", dirLightSpaceMat);
        dirLightShadow.set1Int("diffTex", 0);

        Mesh& planeMesh = Storage::getMesh("mesh/spacePlane.obj");
        Mesh& stonePlace = Storage::getMesh("mesh/stonePlace.obj");
        Mesh& grass = Storage::getMesh("mesh/grass.obj");
        Mesh& particle = Storage::getMesh("mesh/particle.obj");
        Mesh& pointShadowTest = Storage::getMesh("mesh/pointShadowTest.obj");

        glm::mat4 planeModel = glm::rotate(glm::translate(glm::mat4(1), glm::vec3(4, -4, 2)), (float)glfwGetTime(), glm::vec3(0, 0, 1));
        auto grassPoss = {glm::vec3(-3, -8, -1), glm::vec3(-2, -7, -1), glm::vec3(-3, -6.3, -1)};
        glm::mat4 pointShadowTestModel = glm::translate(glm::mat4(1), glm::vec3(20, 0, 0));

        setupMeshForDraw(planeMesh);
        drawMesh(planeMesh, planeModel, dirLightShadow);



        setupMeshForDraw(stonePlace);
        glm::mat4 stonePlaceModel = glm::translate(glm::mat4(1), glm::vec3(0, -12, 0));
        drawMesh(stonePlace, stonePlaceModel, dirLightShadow);

        setupMeshForDraw(grass);
        glDisable(GL_CULL_FACE);
        for(glm::vec3 pos : grassPoss)
            drawMesh(grass, glm::rotate(glm::translate(glm::mat4(1), pos), glm::radians(180.f), glm::vec3(0, 0, 1)), dirLightShadow);
        glEnable(GL_CULL_FACE);

        glDisable(GL_CULL_FACE);
        setupMeshForDraw(pointShadowTest);
        drawMesh(pointShadowTest, pointShadowTestModel, dirLightShadow);
        glEnable(GL_CULL_FACE);


        //Point light Shadows
        glViewport(0, 0, shadowRes.x, shadowRes.y);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowPLFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        Shader& pointLightShadow = Storage::getShader("src/shaders/pointLightShadow");
        pointLightShadow.use();

        for(int i = 0; i < 6; i++)
            pointLightShadow.setMat4(("shadowMats[" + convert2String(i) + "]").c_str(), shadowPLTransforms[i]);
        pointLightShadow.setVec3("lightPos", shadowPointLight.pos);
        pointLightShadow.set1Float("farPlane", shadowPLFarPlane);

        setupMeshForDraw(pointShadowTest);
        drawMesh(pointShadowTest, pointShadowTestModel, pointLightShadow);

        setupMeshForDraw(grass);
        auto grassForShadowModel = glm::rotate(glm::translate(glm::mat4(1), glm::vec3(23, 0, -1)), glm::radians(270.f), glm::vec3(0, 0, 1));
        drawMesh(grass, grassForShadowModel, pointLightShadow);


        //The render
        glViewport(0, 0, Window::width, Window::height);
        glBindFramebuffer(GL_FRAMEBUFFER, multiSampleFramebuff);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glClearColor(1, 1, 1, 1);

        drawSkybox(viewMatrix, projectionMatrix);

        Shader& lightUseShader = Storage::getShader("src/shaders/lightUse");

        lightUseShader.use();

        loadPointLightsToShader(pointLights, lightUseShader);
        //loadDirLightsToShader(dirLights, lightUseShader);
        lightUseShader.setVec3("dirLight.color", sun.color);
        lightUseShader.setVec3("dirLight.dir", sun.dir);

        lightUseShader.setMat4("view", viewMatrix);
        lightUseShader.setMat4("projection", projectionMatrix);
        lightUseShader.setMat4("dirLightSpace", dirLightSpaceMat);


        lightUseShader.setVec3("ambientLight", glm::vec3(0.05));
        lightUseShader.setVec3("viewPos", cam.pos);

        lightUseShader.set1Int("diffTexture", 0);
        lightUseShader.set1Int("specTexture", 1);
        lightUseShader.set1Int("normalTexture", 2);
        lightUseShader.set1Int("dispTex", 3);
        lightUseShader.set1Int("ambientOccTex", 4);

        lightUseShader.set1Int("dirLightShadow", 5);
        lightUseShader.set1Int("shadowPointDepth", 6);
        lightUseShader.set1Float("bloomMinBright", bloomMinBrightness);



        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, dirLightShadowDepth);

        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadowPLCubeMap);

        lightUseShader.set1Float("shadowPLFarPlane", shadowPLFarPlane);

        setupMeshForDraw(planeMesh);
        drawMesh(planeMesh, planeModel, lightUseShader);



        setupMeshForDraw(stonePlace);
        drawMesh(stonePlace, stonePlaceModel, lightUseShader);

        setupMeshForDraw(grass);
        glDisable(GL_CULL_FACE);
        for(glm::vec3 pos : grassPoss)
            drawMesh(grass, glm::rotate(glm::translate(glm::mat4(1), pos), glm::radians(180.f), glm::vec3(0, 0, 1)), lightUseShader);
        drawMesh(grass, grassForShadowModel, lightUseShader);
        glEnable(GL_CULL_FACE);

        setupMeshForDraw(pointShadowTest);
        drawMesh(pointShadowTest, pointShadowTestModel, lightUseShader);


        Shader& instanceShader = Storage::getShader("src/shaders/instance");

        instanceShader.use();
        instanceShader.setMat4("view", viewMatrix);
        instanceShader.setMat4("projection", projectionMatrix);

        setupMeshForDraw(particle);

        glDrawArraysInstanced(GL_TRIANGLES, 0, particle.vertsNum, 5);



        Shader& oneColor = Storage::getShader("src/shaders/oneColor");

        oneColor.use();

        oneColor.setMat4("projection", projectionMatrix);
        oneColor.setMat4("view", viewMatrix);
        oneColor.set1Float("bloomMinBright", bloomMinBrightness);

        Mesh& lightMesh = Storage::getMesh("mesh/light.obj");
        glBindVertexArray(lightMesh.VAO);

        oneColor.setVec3("color", shadowPointLight.color);
        oneColor.setMat4("model", glm::translate(glm::mat4(1), shadowPointLight.pos));
        glDrawArrays(GL_TRIANGLES, 0, lightMesh.vertsNum);



        //drawLights(pointLights, projectionMatrix, viewMatrix);

        /*
        Shader& showTBN = Storage::getShader("src/shaders/showTBN");
        showTBN.use();

        showTBN.setMat4("projView", projectionMatrix * viewMatrix);

        setupMeshForDraw(stonePlace);
        drawMesh(stonePlace, stonePlaceModel, showTBN);
        */


        //Post processing
        Shader& postProcess = Storage::getShader("src/shaders/postProcess");



        //first blit MSAA->color-(downscale)->bloomBuff
        glBindFramebuffer(GL_READ_FRAMEBUFFER, multiSampleFramebuff);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcFramebuff);
        glReadBuffer(GL_COLOR_ATTACHMENT1);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glBlitFramebuffer(0, 0, Window::width, Window::height, 0, 0, Window::width, Window::height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, postProcFramebuff);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, bloomFramebuffs[0]);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glBlitFramebuffer(0, 0, Window::width, Window::height, 0, 0, bloomRes.x, bloomRes.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        //now MSAA of color
        glBindFramebuffer(GL_READ_FRAMEBUFFER, multiSampleFramebuff);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcFramebuff);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glBlitFramebuffer(0, 0, Window::width, Window::height, 0, 0, Window::width, Window::height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        GLenum blit_error = GL_NO_ERROR;
        blit_error = glGetError();
        if ( blit_error != GL_NO_ERROR )
        {
            //std::cout << "BlitFramebuffer failed with error: " <<  blit_error << '\n';
        }

        blurTheBloom(1);

        glViewport(0, 0, Window::width, Window::height);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClear(GL_COLOR_BUFFER_BIT);

        postProcess.use();
        postProcess.set1Float("exposure", exposure);
        postProcess.set1Int("screenTex", 0);
        postProcess.set1Int("bloomTex", 1);
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, postProcFramebuffColorTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, bloomColorTexs[0]);

        //glBindTexture(GL_TEXTURE_2D, dirLightShadowDepth);
        glBindVertexArray(screenQuadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

