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

        GLenum renderTargets[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1}; //main and bloom
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
        Shader justColor;
        Shader postProcess;
        Shader gausBlur;
        //Shader showTBN;
        //Shader basic;
        Shader pointLightShadow;
        Shader dirLightShadow;
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
    };

    ShaderPosData shaderPosData;
    GLuint shaderPosDataUBO;

    struct MainShaderLightData //UBO #1
    {
        struct PointLight{
            vec3_16 pos; //4f
            vec3_16 color; //4f
            float constant;
            float linear;
            float quadratic;
            float padding2; //4f
        };

        struct DirLight{
            vec3_16 color; //4f
            vec3_16 dir; //4f
        };


        vec3_16 ambientLight; //4f
        int pointLightsNum;
        int shadowPointLightsNum;
        int isDirShadowActive;
        float bloomMinBright; //4f

        PointLight pointLights[MAX_POINT_LIGHTS_NUM]; //n*4f
        PointLight shadowPointLights[MAX_SHADOW_POINT_LIGHTS]; //n*4f
        DirLight dirLight; //4f
        float_16 shadowPointFarPlanes[MAX_SHADOW_POINT_LIGHTS]; //n*4f
    };

    MainShaderLightData mainLightData;
    GLuint mainLightDataUBO;



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
    void drawSkybox(const glm::mat4& view, const glm::mat4& projection);
    void loadPointLights2Shader();
    void loadDirLight2Shader();
    void doBloom();
    void loadUBOs();

    //Data storage
    FixedSizeMemPool<Sprite3D> spritePool;
    std::vector<Sprite3D*> sprites;

    FixedSizeMemPool<PointLight> pointLightPool;
    std::vector<PointLight*> pointLights;
};
