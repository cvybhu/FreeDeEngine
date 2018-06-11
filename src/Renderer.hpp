#define MAX_POINT_LIGHTS_NUM 10
#define MAX_SHADOW_POINT_LIGHTS 2

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
    void init(glm::ivec2 renderResolution, int maxSpritesNum);

    void setRenderRes(glm::ivec2 newRenderRes);
    void setBloomRes(glm::ivec2 newBloomRes);

    Sprite3D* addSprite3D(Mesh& itsMesh);
    void removeSprite3D(Sprite3D* sprite);

    PointLight* addPointLight();
    void removePointLight(PointLight* light);

    void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

    CubeTexture* currentSkybox = nullptr;
    float exposure = 0.5;
    float bloomMinBrightness = 1.0;

    DirLight dirLight;

    EnvironmentTex envTex;

private:
//Resolution
    glm::ivec2 renderRes;
    glm::ivec2 bloomRes;

//Framebuffs
    //deffered buffer (G-Buffer)
    struct 
    {
        GLuint index;
        GLuint albedoMetal; //vec3 albedo + float metallic - RGBA16F
        GLuint posRoughness; //vec3 pos + float roughness - RGBA16F
        GLuint normalAmbientOcc; //vec3 normal + float ambientOcc - RGBA16F
        GLuint depth; //32F renderbuffer

        GLenum renderTargets[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};

    } deffBuff;

    //here go light calculations
    struct
    {
        GLuint index;
        GLuint color; //RGBA16F
        GLuint forBloom;
        //depth is taken from deffBuff

        GLenum renderTargets[1] = {GL_COLOR_ATTACHMENT0};
    } mainFbuff;

    //bloom gaus blur buffs
    struct
    {
        GLuint index;
        GLuint color;   //RGBA16F
    } bloomFbuffs[2];

    void createDeffBuff();
    void createMainFramebuff();
    void createBloomFramebuffs();

    void deleteDeffBuff();
    void deleteMainFramebuff();
    void deleteBloomFramebuffs();

    //Shaders
    struct
    {
        Shader deffered;
        Shader skybox;
        Shader justColor;
        Shader postProcess;
        //Shader showTBN;
        Shader pointLightShadow;
        Shader dirLightShadow;
        Shader IBL;
        Shader pointLight;
        Shader dirLight;
        Shader gausBlur;
        Shader bloomSelect;
    } shaders;

    struct vec3_16
    {
        float x, y, z;
        float _padding;

        void operator=(const glm::vec3& v);
    };

    struct float_16
    {
        float val;
        float _padding[3];

        void operator=(const float& f){val = f;};
    };


    struct ShaderPosData   //UBO #0
    {
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 projView;
        glm::mat4 dirLightSpace;
        vec3_16 viewPos;

    } shaderPosData;

    GLuint shaderPosDataUBO;

    
    struct ShaderLightData //UBO #1
    {
        float gamma;
        float bloomMinBrightness;
        float exposure;

    } shaderLightData;

    GLuint shaderLightDataUBO;


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
        GLuint albedo;
        GLuint metallic;
        GLuint roughness;
        GLuint normal;
        GLuint displacement;
        GLuint ambientOcc;
    } defaultTexs;

    GLuint create1x1Texture(glm::ivec3 color);
    void createDefaultTextures();

    unsigned int brdfLUTTexture;
    void generateBRDFLUTTex();

    //Drawing
    void setupMeshForDraw(const Mesh& mesh);
    void drawMesh(Mesh& mesh, const glm::mat4& modelMatrix, Shader& shader);
    void drawSkybox();
    void renderShadows();
    void doBloom();
    void loadUBOs();

    //Data storage
    FixedSizeMemPool<Sprite3D> spritePool;
    std::vector<Sprite3D*> sprites;

    FixedSizeMemPool<PointLight> pointLightPool;
    std::vector<PointLight*> pointLights;
};
