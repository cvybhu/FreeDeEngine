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

    //envTex.loadToRAM("tex/Alexs_Apt_2k.hdr");
    envTex.loadToRAM("tex/Tokyo_BigSight_3k.hdr");
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
    shaders.deffered = Storage<Shader>::get("src/shaders/deffered");
    shaders.deffered.use();
    shaders.deffered.set1Int("albedoTex", 0);
    shaders.deffered.set1Int("metallicTex", 1);
    shaders.deffered.set1Int("roughnessTex", 2);
    shaders.deffered.set1Int("normalTex", 3);
    shaders.deffered.set1Int("ambientOccTex", 4);
    shaders.deffered.set1Int("displacementTex", 5);
    shaders.deffered.set1Int("emmisionTex", 6);

    shaders.deffered.setUBO("posData", 0);

//IBL

    shaders.IBL = Storage<Shader>::get("src/shaders/IBL");
    shaders.IBL.use();

    shaders.IBL.set1Int("albedoMetal", 0);
    shaders.IBL.set1Int("posRoughness", 1);
    shaders.IBL.set1Int("normalAmbientOcc", 2);

    shaders.IBL.set1Int("envIrradiance", 4);
    shaders.IBL.set1Int("prefilterMap", 5);
    shaders.IBL.set1Int("brdfLUTTex", 6);

    shaders.IBL.set1Int("skybox", 7);

    shaders.IBL.setUBO("posData", 0);

//dirLight
    shaders.dirLight = Storage<Shader>::get("src/shaders/dirLight");
    shaders.dirLight.use();

    shaders.dirLight.set1Int("albedoMetal", 0);
    shaders.dirLight.set1Int("posRoughness", 1);
    shaders.dirLight.set1Int("normalAmbientOcc", 2);

    shaders.dirLight.setUBO("posData", 0);

//pointLight
    shaders.pointLight = Storage<Shader>::get("src/shaders/pointLight");
    shaders.pointLight.use();

    shaders.pointLight.set1Int("albedoMetal", 0);
    shaders.pointLight.set1Int("posRoughness", 1);
    shaders.pointLight.set1Int("normalAmbientOcc", 2);
    shaders.pointLight.set1Int("shadowDepth", 4);

    shaders.pointLight.setUBO("posData", 0);

//skybox
    shaders.skybox = Storage<Shader>::get("src/shaders/skybox");
    shaders.skybox.use();
    shaders.skybox.set1Int("normalAmbientOcc", 2);
    shaders.skybox.set1Int("skybox", 7);

//justColor
    shaders.justColor = Storage<Shader>::get("src/shaders/justColor");
    shaders.justColor.use();

    shaders.justColor.setUBO("posData", 0);

//point light shadows
    shaders.pointLightShadow = Storage<Shader>::get("src/shaders/pointLightShadow");

//dir light shadows
    shaders.dirLightShadow = Storage<Shader>::get("src/shaders/dirLightShadow");

//bloomSelect
    shaders.bloomSelect = Storage<Shader>::get("src/shaders/bloomSelect");
    shaders.bloomSelect.use();
    shaders.bloomSelect.set1Int("screenTex", 4);

//gaus blur (for bloom)
    shaders.gausBlur = Storage<Shader>::get("src/shaders/gausBlur");

//Postprocess
    shaders.postProcess = Storage<Shader>::get("src/shaders/postProcess");
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

    //For bright color selection for bloom
    glGenTextures(1, &mainFbuff.forBloom);
    glBindTexture(GL_TEXTURE_2D, mainFbuff.forBloom);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderRes.x, renderRes.y, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mainFbuff.forBloom, 0);


    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, deffBuff.depth);
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
    defaultTexs.roughness = create1x1Texture({192, 192, 192});
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
        //say << "No albedo!!\n";

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
     glCullFace(GL_FRONT);
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
    glCullFace(GL_BACK);


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
    checkGlError();

    renderShadows();
    checkGlError();


//Deffered pass
    glBindFramebuffer(GL_FRAMEBUFFER, deffBuff.index);
    glViewport(0, 0, renderRes.x, renderRes.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    glDrawBuffers(3, deffBuff.renderTargets);
    checkGlError();


    shaders.deffered.use();

    for(Sprite3D* sprite : sprites)
    {
        setupMeshForDraw(*sprite->myMesh);
        drawMesh(*sprite->myMesh, sprite->model, shaders.deffered);
    }
    checkGlError();


//Light pass
    glBindFramebuffer(GL_FRAMEBUFFER, mainFbuff.index);
    //glViewport(0, 0, renderRes.x, renderRes.y);
    glDrawBuffers(1, mainFbuff.renderTargets); //only color
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);


    //Deffered data setup
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, deffBuff.albedoMetal);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, deffBuff.posRoughness);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, deffBuff.normalAmbientOcc);
    checkGlError();


    /*
    //Dir light
    shaders.dirLight.use();
    glBindVertexArray(screenQuad.VAO);

    shaders.dirLight.setVec3("lightDir", dirLight.dir);
    shaders.dirLight.setVec3("lightColor", dirLight.color);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    checkGlError();
    */

    
    //point lights
    shaders.pointLight.use();
    Mesh& lightBall = Storage<Mesh>::get("mesh/lightBallForShading.obj");
    glBindVertexArray(lightBall.VAO);
    glCullFace(GL_FRONT);

    glActiveTexture(GL_TEXTURE4);
    for(PointLight* light : pointLights)
    {
        shaders.pointLight.setVec3("lightPos", light->pos);
        shaders.pointLight.setVec3("lightColor", light->color);

        
        if(!light->shadow.active)
        {
            shaders.pointLight.set1Int("hasShadow", 0);
        }
        else
        {
            shaders.pointLight.set1Int("hasShadow", 1);
            shaders.pointLight.set1Float("shadowFarPlane", light->shadow.farPlane);
            glBindTexture(GL_TEXTURE_CUBE_MAP, light->shadow.cubeMap);
        }
        
        glDrawArrays(GL_TRIANGLES, 0, lightBall.vertsNum);
    }
    glCullFace(GL_BACK);
    checkGlError();
    
    //IBL
    shaders.IBL.use();
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envTex.diffRadianceMap);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envTex.prefilterMap);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

    glBindVertexArray(screenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    checkGlError();



//Forward drawing
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    //point lights drawing
    shaders.justColor.use();

    Mesh& lightMesh = Storage<Mesh>::get("mesh/light.obj");
    glBindVertexArray(lightMesh.VAO);


    for(PointLight* light : pointLights)
    {
        shaders.justColor.setVec3("color", light->color * 3.1415f);
        shaders.justColor.setMat4("model", glm::scale(glm::translate(glm::mat4(1), light->pos), glm::vec3(0.3)));
        glDrawArrays(GL_TRIANGLES, 0, lightMesh.vertsNum);
    }
    checkGlError();


//Bloom 1/2

    shaders.bloomSelect.use();
    shaders.bloomSelect.set1Float("bloomMinBrightness", bloomMinBrightness);

    glDisable(GL_DEPTH_TEST); glDisable(GL_CULL_FACE);
    glDrawBuffers(1, mainFbuff.renderTargets+1);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, mainFbuff.color);

    glBindVertexArray(screenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mainFbuff.index); 
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, bloomFbuffs[0].index); 
    glReadBuffer(GL_COLOR_ATTACHMENT1); 
    glDrawBuffer(GL_COLOR_ATTACHMENT0); 
    
    glBlitFramebuffer(0, 0, renderRes.x, renderRes.y, 0, 0, bloomRes.x, bloomRes.y, GL_COLOR_BUFFER_BIT, GL_LINEAR); 
    


//Skybox
    glBindFramebuffer(GL_FRAMEBUFFER, mainFbuff.index);
    glDrawBuffers(1, mainFbuff.renderTargets);
    glEnable(GL_DEPTH_TEST); glDisable(GL_BLEND);
    glViewport(0, 0, renderRes.x, renderRes.y);
    drawSkybox();
    checkGlError();

//Bloom 2/2
    blurBloom();

//Post processing
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::width, Window::height);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    checkGlError();

    shaders.postProcess.use();
    shaders.postProcess.set1Float("exposure", exposure);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mainFbuff.color);
    //glBindTexture(GL_TEXTURE_2D, Storage<Mesh>::get("mesh/pbrCube.obj").albedoTex->glIndx);
    //glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bloomFbuffs[0].color);

    glBindVertexArray(screenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    checkGlError();
}

void Renderer::blurBloom()
{   
    int passes = 1;

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

void Renderer::drawSkybox()
{
    shaders.skybox.use();
    shaders.skybox.set1Float("exposure", exposure);

    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envTex.cubeMap);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, pointLights[0]->shadow.cubeMap);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, currentSkybox->glIndx);

    glBindVertexArray(skyboxCube.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}


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

    Shader& genShader = Storage<Shader>::get("src/shaders/brdfLUTGen");
    genShader.use();

    glViewport(0, 0, 512, 512);
    glBindVertexArray(screenQuad.VAO);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}