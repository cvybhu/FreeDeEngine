#include <Renderer.hpp>

#include <Window.hpp>
#include <Storage.hpp>
#include <Logger.hpp>


Renderer::Renderer()
{
    renderRes = {800, 600};
    bloomRes = renderRes;
}

void Renderer::init()
{
    setupShaders();
    createMainFramebuff();
    createBloomFramebuffs();
    setupDrawOntoGeometry();
}

void Renderer::setupShaders()
{
    shaders.main = Storage::getShader("src/shaders/main");
    //shaders.main.setVec3("dirLight.color", sun.color);
    //shaders.main.setVec3("dirLight.dir", sun.dir);

    //shaders.main.setMat4("dirLightSpace", dirLightSpaceMat);


    shaders.main.setVec3("ambientLight", glm::vec3(0.5));
    //shaders.main.setVec3("viewPos", cam.pos);

    shaders.main.set1Int("diffTexture", 0);
    shaders.main.set1Int("specTexture", 1);
    shaders.main.set1Int("normalTexture", 2);
    shaders.main.set1Int("dispTex", 3);
    shaders.main.set1Int("ambientOccTex", 4);
    shaders.main.set1Float("bloomMinBright", bloomMinBrightness);

    /*
    shaders.main.set1Int("dirLightShadow", 5);
    shaders.main.set1Int("shadowPointDepth", 6);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, dirLightShadowDepth);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadowPLCubeMap);

    shaders.main.set1Float("shadowPLFarPlane", shadowPLFarPlane);
    */

    shaders.skybox = Storage::getShader("src/shaders/skybox");
    shaders.bloomBlur = Storage::getShader("src/shaders/main");
    shaders.showTBN = Storage::getShader("src/shaders/main");
    shaders.basic = Storage::getShader("src/shaders/basic");

    shaders.postProcess = Storage::getShader("src/shaders/postProcess");
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, bloomRes.x, bloomRes.y, 0, GL_RGB, GL_FLOAT, NULL);
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


void Renderer::draw(glm::mat4& viewMatrix, glm::mat4& projectionMatrix)
{
    //Main framebuffer setup
    glBindFramebuffer(GL_FRAMEBUFFER, mainFbuff.index);
    glViewport(0, 0, renderRes.x, renderRes.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    //Skybox
    if(currentSkybox != nullptr)
    {
        shaders.skybox.use();
        shaders.skybox.setMat4("view", glm::mat3(viewMatrix));
        shaders.skybox.setMat4("projection", projectionMatrix);
        shaders.skybox.set1Float("exposure", exposure);

        glDepthMask(GL_FALSE);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, currentSkybox->glIndx);

        glBindVertexArray(skyboxCube.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthMask(GL_TRUE);
    }




    Mesh& planeMesh = Storage::getMesh("mesh/spacePlane.obj");

    shaders.main.use();
    shaders.main.setMat4("view", viewMatrix);
    shaders.main.setMat4("projection", projectionMatrix);

    setupMeshForDraw(planeMesh);
    drawMesh(planeMesh, glm::mat4(1), shaders.main);


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::width, Window::height);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    shaders.postProcess.use();
    shaders.postProcess.set1Float("exposure", 0.5);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mainFbuff.color);

    glBindVertexArray(screenQuad.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


void Renderer::setRenderRes(glm::ivec2 newRenderRes)
{
    //TODO - realoc buffers

    renderRes = newRenderRes;
}