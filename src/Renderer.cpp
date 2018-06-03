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
    createDeffBuff();
    createMainFramebuff();
    createBloomFramebuffs();
    createDefaultTextures();
    setupDrawOntoGeometry();
    generateBRDFLUTTex();
    spritePool.init(maxSpritesNum);

    envTex.loadToRAM("tex/Alexs_Apt_2k.hdr");
    //envTex.loadToRAM("tex/Tokyo_BigSight_3k.hdr");
    envTex.loadToGPU();
    envTex.generateCubeMaps(1024);
}

void Renderer::setupShaders()
{
//UBOs
    auto genUBO = [](GLuint& ubo, GLuint size, GLuint index){
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBindBufferBase(GL_UNIFORM_BUFFER, index, ubo);
        glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
    };

    genUBO(shaderPosDataUBO, sizeof(ShaderPosData), 0);
    genUBO(shaderLightDataUBO, sizeof(ShaderLightData), 1);

//deffered   
    shaders.deffered = Storage::getShader("src/shaders/deffered");
    shaders.deffered.use();
    shaders.deffered.set1Int("albedoTex", 0);
    shaders.deffered.set1Int("metallicTex", 1);
    shaders.deffered.set1Int("roughnessTex", 2);
    shaders.deffered.set1Int("normalTex", 3);
    shaders.deffered.set1Int("ambientOccTex", 4);
    shaders.deffered.set1Int("displacementTex", 5);
    shaders.deffered.set1Int("emmisionTex", 6);

    shaders.deffered.setUBO("posData", 0);

//environtment - IBL

    shaders.environment = Storage::getShader("src/shaders/environment");
    shaders.environment.use();

    shaders.environment.set1Int("albedoMetal", 0);
    shaders.environment.set1Int("posRoughness", 1);
    shaders.environment.set1Int("normalAmbientOcc", 2);
    shaders.environment.set1Int("envIrradiance", 4);
    shaders.environment.set1Int("prefilterMap", 5);
    shaders.environment.set1Int("brdfLUTTex", 6);
    shaders.environment.set1Int("skybox", 7);

    shaders.environment.set1Int("posData", 0);

//deffLight
    shaders.deffLight = Storage::getShader("src/shaders/defferedLight");
    shaders.deffLight.use();
    shaders.deffLight.set1Int("albedoMetal", 0);
    shaders.deffLight.set1Int("posRoughness", 1);
    shaders.deffLight.set1Int("normalAmbientOcc", 2);


    shaders.deffLight.setUBO("posData", 0);

//skybox
    shaders.skybox = Storage::getShader("src/shaders/skybox");
    shaders.skybox.use();
    shaders.skybox.set1Int("normalAmbientOcc", 2);
    shaders.skybox.set1Int("skybox", 7);

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

void Renderer::createDeffBuff()
{
    glGenFramebuffers(1, &deffBuff.index);
    glBindFramebuffer(GL_FRAMEBUFFER, deffBuff.index);

    auto createTex = [](GLuint& tex, int attachIndex, glm::ivec2 res){
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, res.x, res.y, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachIndex, GL_TEXTURE_2D, tex, 0);
    };

    createTex(deffBuff.albedoMetal, 0, renderRes);
    createTex(deffBuff.posRoughness, 1, renderRes);
    createTex(deffBuff.normalAmbientOcc, 2, renderRes);

    //Depth rbo
    glGenRenderbuffers(1, &deffBuff.depth);
    glBindRenderbuffer(GL_RENDERBUFFER, deffBuff.depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, renderRes.x, renderRes.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, deffBuff.depth);
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
    defaultTexs.albedo = create1x1Texture({128, 128, 128});
    defaultTexs.metallic = create1x1Texture({0, 0, 0});
    defaultTexs.roughness = create1x1Texture({128, 128, 128});
    defaultTexs.normal = create1x1Texture({128, 128, 255});
    defaultTexs.displacement = create1x1Texture({0, 0, 0});
    defaultTexs.ambientOcc = create1x1Texture({255, 255, 255});
}


void Renderer::setupMeshForDraw(const Mesh& mesh)
{

//Albedo
    glActiveTexture(GL_TEXTURE0);
    if(mesh.albedoTex != nullptr)
        glBindTexture(GL_TEXTURE_2D, mesh.albedoTex->glIndx);
    else
    {
        glBindTexture(GL_TEXTURE_2D, defaultTexs.albedo);
        //std::cout << "No albedo!!\n";

    }

//Metallic
    glActiveTexture(GL_TEXTURE1);
    if(mesh.metallicTex != nullptr)
        glBindTexture(GL_TEXTURE_2D, mesh.metallicTex->glIndx);
    else
        glBindTexture(GL_TEXTURE_2D, defaultTexs.metallic);

//Roughness
    glActiveTexture(GL_TEXTURE2);
    if(mesh.roughnessTex != nullptr)
        glBindTexture(GL_TEXTURE_2D, mesh.roughnessTex->glIndx);
    else
        glBindTexture(GL_TEXTURE_2D, defaultTexs.roughness);

//Normals
    glActiveTexture(GL_TEXTURE3);
    if(mesh.normalTex != nullptr)
        glBindTexture(GL_TEXTURE_2D, mesh.normalTex->glIndx);
    else
        glBindTexture(GL_TEXTURE_2D, defaultTexs.normal);

//ambientOcc
    glActiveTexture(GL_TEXTURE4);
    if(mesh.ambientOccTex != nullptr)
        glBindTexture(GL_TEXTURE_2D, mesh.ambientOccTex->glIndx);
    else
        glBindTexture(GL_TEXTURE_2D, defaultTexs.ambientOcc);

//displacement
    glActiveTexture(GL_TEXTURE5);
    if(mesh.displacementTex != nullptr)
        glBindTexture(GL_TEXTURE_2D, mesh.displacementTex->glIndx);
    else
        glBindTexture(GL_TEXTURE_2D, defaultTexs.displacement);

//emmsion
    glActiveTexture(GL_TEXTURE6);


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

    /*glBindBuffer(GL_UNIFORM_BUFFER, mainLightDataUBO);
    uboDataPtr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(uboDataPtr, &mainLightData, sizeof(MainShaderLightData));
    glUnmapBuffer(GL_UNIFORM_BUFFER);*/
}


void Renderer::renderShadows()
{
     //Dir Light shadow
    if(dirLight.shadow.active)
    {
        shaders.dirLightShadow.use();

        dirLight.setupShadowRendering(shaders.dirLightShadow);
        glActiveTexture(GL_TEXTURE0);

        for(Sprite3D* sprite : sprites)
        {
            if(sprite->myMesh->albedoTex != nullptr)
                glBindTexture(GL_TEXTURE_2D, sprite->myMesh->albedoTex->glIndx);
            else
                glBindTexture(GL_TEXTURE_2D, defaultTexs.albedo);

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
                if(sprite->myMesh->albedoTex != nullptr)
                    glBindTexture(GL_TEXTURE_2D, sprite->myMesh->albedoTex->glIndx);
                else
                    glBindTexture(GL_TEXTURE_2D, defaultTexs.albedo);

                glBindVertexArray(sprite->myMesh->VAO);

                shaders.pointLightShadow.setMat4("model", sprite->model);
                glDrawArrays(GL_TRIANGLES, 0, sprite->myMesh->vertsNum);
            }
        }
}





void Renderer::draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    glm::vec3 viewPos = glm::vec4(0, 0, 0, 1) * transpose(inverse(viewMatrix));

    //UBOs
    shaderPosData.view = viewMatrix;
    shaderPosData.projection = projectionMatrix;
    shaderPosData.projView = projectionMatrix * viewMatrix;
    shaderPosData.viewPos = viewPos;

    loadUBOs();

//Deffered pass
    glBindFramebuffer(GL_FRAMEBUFFER, deffBuff.index);
    glViewport(0, 0, renderRes.x, renderRes.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glDrawBuffers(3, deffBuff.renderTargets);

    shaders.deffered.use();

    for(Sprite3D* sprite : sprites)
    {
        setupMeshForDraw(*sprite->myMesh);
        drawMesh(*sprite->myMesh, sprite->model, shaders.deffered);
    }

//Light pass
    glBindFramebuffer(GL_FRAMEBUFFER, mainFbuff.index);
    //glViewport(0, 0, renderRes.x, renderRes.y);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    glDrawBuffers(1, mainFbuff.renderTargets); //only color

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, deffBuff.albedoMetal);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, deffBuff.posRoughness);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, deffBuff.normalAmbientOcc);

    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP, currentSkybox->glIndx);

    shaders.environment.use();
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envTex.diffRadianceMap);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envTex.prefilterMap);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

    /*shaders.deffLight.use();

    //quick lighting solution to test PBR - gotta do it completely another deffered way
    shaders.deffLight.setVec3("dirLight.color", dirLight.color);
    shaders.deffLight.setVec3("dirLight.dir", dirLight.dir);
    shaders.deffLight.setVec3("pointLights[0].color", pointLights[0]->color);
    shaders.deffLight.setVec3("pointLights[0].pos", pointLights[0]->pos);
    shaders.deffLight.setVec3("pointLights[1].color", pointLights[1]->color);
    shaders.deffLight.setVec3("pointLights[1].pos", pointLights[1]->pos);
    */

    glBindVertexArray(screenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    drawSkybox(viewMatrix, projectionMatrix);
    //here will be bloom 
    //Bloom
    //doBloom();


//Post processing
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::width, Window::height);
    glClear(GL_COLOR_BUFFER_BIT);
    //glDisable(GL_DEPTH_TEST);

    shaders.postProcess.use();
    shaders.postProcess.set1Float("exposure", exposure);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mainFbuff.color);
    //glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
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

    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envTex.prefilterMap);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, currentSkybox->glIndx);


    glBindVertexArray(skyboxCube.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::loadPointLights2Shader()
{
    int pointLightsNum = 0;
    int shadowPointLightsNum = 0;
    /*
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
    */
    //std::cout << "pointLightsNum: " << pointLightsNum << '\n';
    //std::cout << "shadowsNum: " << shadowPointLightsNum << '\n';
}

void Renderer::loadDirLight2Shader()
{
    //mainLightData.dirLight.color = dirLight.color;
    //mainLightData.dirLight.dir = dirLight.dir;
    //mainLightData.isDirShadowActive = (dirLight.shadow.active ? 1 : 0);

    if(dirLight.shadow.active)
    {
        //glActiveTexture(GL_TEXTURE5);
        //glBindTexture(GL_TEXTURE_2D, dirLight.shadow.depth);
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

void Renderer::generateBRDFLUTTex()
{
    glGenTextures(1, &brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

    GLuint fbuff;
    glGenFramebuffers(1, &fbuff);
    glBindFramebuffer(GL_FRAMEBUFFER, fbuff);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    Shader& genShader = Storage::getShader("src/shaders/brdfLUTGen");
    genShader.use();

    glViewport(0, 0, 512, 512);
    glBindVertexArray(screenQuad.VAO);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}