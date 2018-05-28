#include <Renderer.hpp>

#include <Window.hpp>
#include <Storage.hpp>
#include <Utils.hpp>
#include <Logger.hpp>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer()
{
    dirLight.color = {0, 0, 0};
}

void Renderer::init(glm::ivec2 renderResolution, int maxSpritesNum)
{
    renderRes = renderResolution;
    bloomRes = renderRes/4;
    setupShaders();
    createMainFramebuff();
    createBloomFramebuffs();
    createDefaultTextures();
    setupDrawOntoGeometry();
    spritePool.init(maxSpritesNum);
}

void Renderer::setupShaders()
{
//UBOs
    glGenBuffers(1, &shaderPosDataUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, shaderPosDataUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, shaderPosDataUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(ShaderPosData), NULL, GL_DYNAMIC_DRAW);
        
    glGenBuffers(1, &mainLightDataUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, mainLightDataUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, mainLightDataUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(MainShaderLightData), NULL, GL_DYNAMIC_DRAW);

//Main
    shaders.main = Storage::getShader("src/shaders/main");
    shaders.main.use();
    shaders.main.set1Int("diffTexture", 0);
    shaders.main.set1Int("specTexture", 1);
    shaders.main.set1Int("normalTexture", 2);
    shaders.main.set1Int("dispTex", 3);
    shaders.main.set1Int("ambientOccTex", 4);
    shaders.main.set1Int("dirLightShadow", 5);
    shaders.main.set1Int("shadowPointDepth[0]", 6);
    shaders.main.set1Int("shadowPointDepth[1]", 7);

    shaders.main.setUBO("posData", 0);
    shaders.main.setUBO("lightData", 1);


//Skybox
    shaders.skybox = Storage::getShader("src/shaders/skybox");
    shaders.skybox.use();
    shaders.skybox.setUBO("posData", 0);

//justColor
    shaders.justColor = Storage::getShader("src/shaders/justColorMTR");
    shaders.justColor.use();
    shaders.justColor.setUBO("posData", 0);
    shaders.justColor.setUBO("lightData", 1);

//point light shadows
    shaders.pointLightShadow = Storage::getShader("src/shaders/pointLightShadow");

//dir light shadows
    shaders.dirLightShadow = Storage::getShader("src/shaders/dirLightShadow");

//gaus blur (for bloom)
    shaders.gausBlur = Storage::getShader("src/shaders/gausBlur");

//Postprocess
    shaders.postProcess = Storage::getShader("src/shaders/postProcess");
    shaders.postProcess.use();
    shaders.postProcess.set1Int("screenTex", 0);
    shaders.postProcess.set1Int("bloomTex", 1);
}


void Renderer::createMainFramebuff()
{
    glGenFramebuffers(1, &mainFbuff.index);
    glBindFramebuffer(GL_FRAMEBUFFER, mainFbuff.index);

    //Color tex
    glGenTextures(1, &mainFbuff.color);
    glBindTexture(GL_TEXTURE_2D, mainFbuff.color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderRes.x, renderRes.y, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mainFbuff.color, 0);

    //Depth rbo
    glGenRenderbuffers(1, &mainFbuff.depth);
    glBindRenderbuffer(GL_RENDERBUFFER, mainFbuff.depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, renderRes.x, renderRes.y);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mainFbuff.depth);


    //Bloom color tex
    glGenTextures(1, &mainFbuff.bloom);
    glBindTexture(GL_TEXTURE_2D, mainFbuff.bloom);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderRes.x, renderRes.y, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderRes.x, renderRes.y, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mainFbuff.bloom, 0);
}


void Renderer::createBloomFramebuffs()
{
    for(int i = 0; i < 2; i++)
    {
        glGenFramebuffers(1, &bloomFbuffs[i].index);
        glBindFramebuffer(GL_FRAMEBUFFER, bloomFbuffs[i].index);

        glGenTextures(1, &bloomFbuffs[i].color);
        glBindTexture(GL_TEXTURE_2D, bloomFbuffs[i].color);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, bloomRes.x, bloomRes.y, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomFbuffs[i].color, 0);
    }
}


void Renderer::setupDrawOntoGeometry()
{
    float screenQuadVerts[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
        };

    glGenVertexArrays(1, &screenQuad.VAO);
    glGenBuffers(1, &screenQuad.VBO);
    glBindVertexArray(screenQuad.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenQuad.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVerts), &screenQuadVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));



    float skyboxVertices[] = {
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

        glGenVertexArrays(1, &skyboxCube.VAO);
        glGenBuffers(1, &skyboxCube.VBO);
        glBindVertexArray(skyboxCube.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxCube.VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

GLuint Renderer::create1x1Texture(glm::ivec3 color)
{
    GLuint tex;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    char texData[] = {(char)color.x, (char)color.y, (char)color.z};

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);

    return tex;
}

void Renderer::createDefaultTextures()
{
    defaultTexs.color = create1x1Texture({128, 128, 128});
    defaultTexs.normal = create1x1Texture({128, 128, 255});
    defaultTexs.displacement = create1x1Texture({0, 0, 0});
    defaultTexs.ambientOcc = create1x1Texture({255, 255, 255});
}


void Renderer::setupMeshForDraw(const Mesh& mesh)
{
    glActiveTexture(GL_TEXTURE0);
    if(mesh.diffTex != nullptr)
        glBindTexture(GL_TEXTURE_2D, mesh.diffTex->glIndx);
    else
        glBindTexture(GL_TEXTURE_2D, defaultTexs.color);


    glActiveTexture(GL_TEXTURE1);
    if(mesh.specTex != nullptr)
        glBindTexture(GL_TEXTURE_2D, mesh.specTex->glIndx);
    else
        glBindTexture(GL_TEXTURE_2D, defaultTexs.color);


    glActiveTexture(GL_TEXTURE2);
    if(mesh.normalTex != nullptr)
        glBindTexture(GL_TEXTURE_2D, mesh.normalTex->glIndx);
    else
        glBindTexture(GL_TEXTURE_2D, defaultTexs.normal);


    glActiveTexture(GL_TEXTURE3);
    if(mesh.dispTex != nullptr)
        glBindTexture(GL_TEXTURE_2D, mesh.dispTex->glIndx);
    else
        glBindTexture(GL_TEXTURE_2D, defaultTexs.displacement);

    glActiveTexture(GL_TEXTURE4);
    if(mesh.ambientOccTex != nullptr)
        glBindTexture(GL_TEXTURE_2D, mesh.ambientOccTex->glIndx);
    else
        glBindTexture(GL_TEXTURE_2D, defaultTexs.ambientOcc);

    glBindVertexArray(mesh.VAO);
}

void Renderer::drawMesh(Mesh& mesh, const glm::mat4& modelMatrix, Shader& shader)
{
    shader.setMat4("model", modelMatrix);
    glDrawArrays(GL_TRIANGLES, 0, mesh.vertsNum);
}


void Renderer::loadUBOs()
{
    glBindBuffer(GL_UNIFORM_BUFFER, shaderPosDataUBO);
    GLvoid* uboDataPtr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(uboDataPtr, &shaderPosData, sizeof(ShaderPosData));
    glUnmapBuffer(GL_UNIFORM_BUFFER);

    glBindBuffer(GL_UNIFORM_BUFFER, mainLightDataUBO);
    uboDataPtr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(uboDataPtr, &mainLightData, sizeof(MainShaderLightData));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void Renderer::draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    glm::vec3 viewPos = glm::vec4(0, 0, 0, 1) * transpose(inverse(viewMatrix));

//Shadows
    //Dir Light shadow
    if(dirLight.shadow.active)
    {
        shaders.dirLightShadow.use();

        dirLight.setupShadowRendering(shaders.dirLightShadow);
        glActiveTexture(GL_TEXTURE0);

        for(Sprite3D* sprite : sprites)
        {
            if(sprite->myMesh->diffTex != nullptr)
                glBindTexture(GL_TEXTURE_2D, sprite->myMesh->diffTex->glIndx);
            else
                glBindTexture(GL_TEXTURE_2D, defaultTexs.color);

            glBindVertexArray(sprite->myMesh->VAO);

            shaders.dirLightShadow.setMat4("model", sprite->model);
            glDrawArrays(GL_TRIANGLES, 0, sprite->myMesh->vertsNum);   
        }
    }


    //Point light shadows
    shaders.pointLightShadow.use();

    for(PointLight* light : pointLights)
        if(light->shadow.active)
        {
            light->setupShadowRendering(shaders.pointLightShadow);
            glActiveTexture(GL_TEXTURE0);

            for(Sprite3D* sprite : sprites)
            {
                if(sprite->myMesh->diffTex != nullptr)
                    glBindTexture(GL_TEXTURE_2D, sprite->myMesh->diffTex->glIndx);
                else
                    glBindTexture(GL_TEXTURE_2D, defaultTexs.color);

                glBindVertexArray(sprite->myMesh->VAO);

                shaders.pointLightShadow.setMat4("model", sprite->model);
                glDrawArrays(GL_TRIANGLES, 0, sprite->myMesh->vertsNum);
            }
        }

    //UBOs
    shaderPosData.view = viewMatrix;
    shaderPosData.projection = projectionMatrix;
    shaderPosData.projView = projectionMatrix * viewMatrix;
    shaderPosData.viewPos = viewPos;

    loadPointLights2Shader();
    loadDirLight2Shader();

    if(dirLight.shadow.active)
        shaderPosData.dirLightSpace = dirLight.getLightSpaceMat();

    mainLightData.bloomMinBright = bloomMinBrightness;
    loadUBOs();

//Main framebuffer setup
    glBindFramebuffer(GL_FRAMEBUFFER, mainFbuff.index);
    glViewport(0, 0, renderRes.x, renderRes.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glDrawBuffers(1, mainFbuff.renderTargets); //only color no bloom

    //Skybox
    if(currentSkybox != nullptr)
        drawSkybox(viewMatrix, projectionMatrix);



    glDrawBuffers(2, mainFbuff.renderTargets); //color & bloom

    //Drawing pointLights
    shaders.justColor.use();

    Mesh& lightMesh = Storage::getMesh("mesh/light.obj");
    glBindVertexArray(lightMesh.VAO);

    for(PointLight* light : pointLights)
    {
        shaders.justColor.setMat4("model", glm::scale(glm::translate(glm::mat4(1), light->pos), glm::vec3(0.7)));
        shaders.justColor.setVec3("color", light->color);
        glDrawArrays(GL_TRIANGLES, 0, lightMesh.vertsNum);
    }


    //Main render
    shaders.main.use();


    if(dirLight.shadow.active)
    {
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, dirLight.shadow.depth);
    }

    auto curPointShadowTexture = GL_TEXTURE6;

    for(PointLight* light : pointLights)
    {
        if(light->shadow.active)
        {
            glActiveTexture(curPointShadowTexture);
            glBindTexture(GL_TEXTURE_CUBE_MAP, light->shadow.cubeMap);

            curPointShadowTexture ++;
        }
    }
    

    for(Sprite3D* sprite : sprites)
    {
        setupMeshForDraw(*sprite->myMesh);
        drawMesh(*sprite->myMesh, sprite->model, shaders.main);
    }


//Post processing
    //Bloom
    doBloom();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::width, Window::height);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);


    shaders.postProcess.use();
    shaders.postProcess.set1Float("exposure", exposure);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mainFbuff.color);
    //glBindTexture(GL_TEXTURE_2D, dirLight.shadow.depth);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bloomFbuffs[0].color);

    glBindVertexArray(screenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::doBloom()
{   
    int passes = 2;

    glBindFramebuffer(GL_READ_FRAMEBUFFER, mainFbuff.index);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, bloomFbuffs[0].index);
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, renderRes.x, renderRes.y, 0, 0, bloomRes.x, bloomRes.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);


    glDisable(GL_DEPTH_TEST);

    shaders.gausBlur.use();
    glBindVertexArray(screenQuad.VAO);
    glViewport(0, 0, bloomRes.x, bloomRes.y);

    for(int i = 0; i < 2*passes; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, bloomFbuffs[(i+1)%2].index);

        shaders.gausBlur.set1Int("isHorizontal", (i%2 == 0));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bloomFbuffs[i%2].color);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void Renderer::drawSkybox(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    shaders.skybox.use();
    shaders.skybox.set1Float("exposure", exposure);

    glDepthMask(GL_FALSE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, currentSkybox->glIndx);

    glBindVertexArray(skyboxCube.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthMask(GL_TRUE);
}

void Renderer::loadPointLights2Shader()
{
    int pointLightsNum = 0;
    int shadowPointLightsNum = 0;

    for(PointLight* light : pointLights)
        if(!light->shadow.active)
        {
            mainLightData.pointLights[pointLightsNum].pos = light->pos;
            mainLightData.pointLights[pointLightsNum].color = light->color;
            mainLightData.pointLights[pointLightsNum].constant = light->constant;
            mainLightData.pointLights[pointLightsNum].linear = light->linear;
            mainLightData.pointLights[pointLightsNum].quadratic = light->quadratic;

            pointLightsNum++;
        }
        else
        {
            mainLightData.shadowPointLights[shadowPointLightsNum].pos = light->pos;
            mainLightData.shadowPointLights[shadowPointLightsNum].color = light->color;
            mainLightData.shadowPointLights[shadowPointLightsNum].constant = light->constant;
            mainLightData.shadowPointLights[shadowPointLightsNum].linear = light->linear;
            mainLightData.shadowPointLights[shadowPointLightsNum].quadratic = light->quadratic;

            mainLightData.shadowPointFarPlanes[shadowPointLightsNum] = light->shadow.farPlane;

            shadowPointLightsNum++;
        }

    mainLightData.pointLightsNum = pointLightsNum;
    mainLightData.shadowPointLightsNum = shadowPointLightsNum;

    //std::cout << "pointLightsNum: " << pointLightsNum << '\n';
    //std::cout << "shadowsNum: " << shadowPointLightsNum << '\n';
}

void Renderer::loadDirLight2Shader()
{
    mainLightData.dirLight.color = dirLight.color;
    mainLightData.dirLight.dir = dirLight.dir;
    mainLightData.isDirShadowActive = (dirLight.shadow.active ? 1 : 0);

    if(dirLight.shadow.active)
    {
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, dirLight.shadow.depth);
    }
}

/*void Renderer::setRenderRes(glm::ivec2 newRenderRes)
{
    //TODO - realoc buffers

    renderRes = newRenderRes;
}

//void Renderer
*/

Sprite3D* Renderer::addSprite3D(Mesh& itsMesh)
{
    Sprite3D* sprite = new Sprite3D(&itsMesh);
    sprites.emplace_back(sprite);
    return sprite;
}

void Renderer::removeSprite3D(Sprite3D* sprite)
{
    //spritePool.freeMem(sprite);

    sprites.erase(find(sprites.begin(), sprites.end(), sprite));
}

PointLight* Renderer::addPointLight()
{
    PointLight* light = new PointLight;
    pointLights.emplace_back(light);
    return light;
}

void Renderer::removePointLight(PointLight* light)
{
    pointLights.erase(std::find(pointLights.begin(), pointLights.end(), light));
    delete light;
}


void Renderer::vec3_16::operator=(const glm::vec3& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
}