#pragma once

#include <glad/glad.h>
#include <glm/vec2.hpp>

#include <Shader.hpp>
#include <Texture.hpp>
#include <Mesh.hpp>
#include <Lights.hpp>
#include <FixedSizeMemPool.hpp>
#include <Sprite3D.hpp>

class Renderer
{
public:
    Renderer();
    void init(int maxSpritesNum);

    void setRenderRes(glm::ivec2 newRenderRes);
    void setBloomRes(glm::ivec2 newBloomRes);

    Sprite3D* addSprite3D(Mesh& itsMesh);
    void removeSprite3D(Sprite3D* sprite);

    PointLight* addPointLight();
    void removePointLight(PointLight* light);

    void draw(glm::mat4& viewMatrix, glm::mat4& projectionMatrix);

    CubeTexture* currentSkybox = nullptr;
    float exposure = 0.5;
    float bloomMinBrightness = 1.0;

    DirLight dirLight;

private:
    //Resolution
    glm::ivec2 renderRes;
    glm::ivec2 bloomRes;


    //Framebuffs
    struct
    {
        GLuint index;
        GLuint color; //RGBA16F
        GLuint bloom; //RGBA16F
        GLuint depth; //32F
    } mainFbuff;

    struct
    {
        GLuint index;
        GLuint color;   //RGBA16F
    } bloomFbuffs[2];

    void createMainFramebuff();
    void createBloomFramebuffs();

    void deleteMainFramebuff();
    void deleteBloomFramebuffs();


    //Shaders
    struct
    {
        Shader main;
        Shader skybox;
        Shader postProcess;
        Shader bloomBlur;
        Shader showTBN;
        Shader basic;
        Shader pointLightShadow;
        Shader dirLightShadow;
    } shaders;

    void setupShaders();


    //DrawOnto geometry
    struct
    {
        GLuint VBO, VAO;
    } screenQuad, skyboxCube;

    void setupDrawOntoGeometry();


    //Default textures
    struct
    {
        GLuint color; //diff & specular
        GLuint normal;
        GLuint displacement;
        GLuint ambientOcc;
    } defaultTexs;

    GLuint create1x1Texture(glm::ivec3 color);
    void createDefaultTextures();


    //Drawing
    void setupMeshForDraw(const Mesh& mesh);
    void drawMesh(Mesh& mesh, const glm::mat4& modelMatrix, Shader& shader);
    void drawSkybox(glm::mat4& view, glm::mat4& projection);
    void loadPointLights2Shader();
    void loadDirLight2Shader();

    //Data storage
    FixedSizeMemPool<Sprite3D> spritePool;
    std::vector<Sprite3D*> sprites;

    FixedSizeMemPool<PointLight> pointLightPool;
    std::vector<PointLight*> pointLights;
};
