#include <Texture.hpp>
#include <Shader.hpp>
#include <Logger.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <stdio.h>

#include <stb_image.h>
#include <texFile.hpp>


namespace Storage
{
    Shader& getShader(const char*);
}


void Texture::loadToRAM(const char* filePath)
{
    std::string texFilePath(filePath);
    int dotPos = texFilePath.size()-1;
    while(texFilePath[dotPos] != '.')
    {
        texFilePath[dotPos] = 0;
        dotPos--;
    }

    memcpy(&texFilePath[0] + dotPos, ".tex", 4);

    std::vector<void*> dataVec;
    if(!texFile::load(texFilePath.c_str(), dataVec, width, height, nrChannels))
        if(!texFile::create(filePath, texFilePath.c_str()) || !texFile::load(texFilePath.c_str(), dataVec, width, height, nrChannels))
        {
            std::cout << "[ERROR] - Failed to load " << filePath << "!!\n";
            return;
        }
    
    data = (unsigned char*)dataVec[0];

    isOnRAM = true;
    isNormalMap = false;

    std::cout << "[TEXLOAD]Succesfully loaded " << filePath << "!\n";
}

#include <glm/vec3.hpp>


void Texture::loadToRamAsNormalMap(const char* filePath)
{
    stbi_set_flip_vertically_on_load(true);
    //stbi_set_flip_horizontally_on_load(true);
    data = stbi_load(filePath, &width, &height, &nrChannels, 0);

    if (!data)
    {
        std::cout << "[ERROR TEX] Failed to load texture " << filePath << "\n";
        return;
    }

    glm::vec3 average(0);

    for(int i = 0; i < width*height*nrChannels; i += 3)
    {
        char num = (char)((int)data[i] - 128);
        data[i] = reinterpret_cast<unsigned char&>(num);

        num = (char)((int)data[i+1] - 128);
        data[i+1] = reinterpret_cast<unsigned char&>(num);

        num = (char)((int)data[i+2] - 128);
        data[i+2] = reinterpret_cast<unsigned char&>(num);

        glm::vec3 normal(reinterpret_cast<char&>(data[i]), reinterpret_cast<char&>(data[i+1]), reinterpret_cast<char&>(data[i+2]));
        normal = glm::normalize(normal); 
        average += normal;
    }
    isNormalMap = true;

    isOnRAM = true;

    std::cout << glm::normalize(average) << '\n';

    std::cout << "[TEXLOAD]Succesfully loaded " << filePath << "!\n";
}



void Texture::loadToGPU(bool fixGamma)
{
    glGenTextures(1, &glIndx);
    glBindTexture(GL_TEXTURE_2D, glIndx);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);

    GLenum types[2];
    types[0] = types[1] = 0;

    switch(nrChannels)
    {
    case 1:{
        types[0] = GL_R8;
        types[1] = GL_RED;
        break;
    }

    case 2:{
        types[0] = GL_RG8;
        types[1] = GL_RG;
        break;
    }

    case 3:{
        types[0] = fixGamma ? GL_SRGB8 : GL_RGB8;
        types[1] = GL_RGB;
        break;
    }

    case 4:{
        types[0] = fixGamma ? GL_SRGB8_ALPHA8 : GL_RGBA8;
        types[1] = GL_RGBA;
        break;
    }
    };

    if(!isNormalMap)
        glTexImage2D(GL_TEXTURE_2D, 0, types[0], width, height, 0, types[1], GL_UNSIGNED_BYTE, data);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8_SNORM, width, height, 0, GL_RGB, GL_BYTE, data);

    
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    isOnGPU = true;
}


void Texture::unloadFromRAM()
{
    stbi_image_free(data);

    isOnRAM = false;
}

void Texture::unloadFromGPU()
{
    //TODO
    isOnGPU = false;
}

unsigned char* Texture::getPixel(const int& x, const int& y){return data + nrChannels*sizeof(unsigned char)*y*width + nrChannels*sizeof(unsigned char*)*x;}
const unsigned char* Texture::getPixel(const int& x, const int& y) const {return data + nrChannels*sizeof(unsigned char)*y*width + nrChannels*sizeof(unsigned char*)*x;}


void Texture::generateTexFile(const char* fileName)
{


}






void CubeTexture::loadToRAM(const char* fileName)
{
    int  nrChannels;
    stbi_set_flip_vertically_on_load(true);
    data[0] = stbi_load((std::string(fileName) + "_left.jpg").c_str(), &width, &height, &nrChannels, 0); //X+
    data[1] = stbi_load((std::string(fileName) + "_right.jpg").c_str(), &width, &height, &nrChannels, 0);  //X-
    data[2] = stbi_load((std::string(fileName) + "_back.jpg").c_str(), &width, &height, &nrChannels, 0);    //Y+
    data[3] = stbi_load((std::string(fileName) + "_front.jpg").c_str(), &width, &height, &nrChannels, 0);  //Y-
    data[4] = stbi_load((std::string(fileName) + "_up.jpg").c_str(), &width, &height, &nrChannels, 0);  //Z+
    data[5] = stbi_load((std::string(fileName) + "_down.jpg").c_str(), &width, &height, &nrChannels, 0); //Z-

    for(int i = 0; i < 6; i++)
        if(!data[i])
        {
            std::cout << "[TEXLOAD] Failed to load " << fileName << "!!!!!!!!!!!!!!\n";
        }

    isOnRAM = true;
}

void CubeTexture::loadToGPU(bool fixGamma)
{
    glGenTextures(1, &glIndx);
    glBindTexture(GL_TEXTURE_CUBE_MAP, glIndx);

    for(int i = 0; i < 6; i++)
        if(fixGamma)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data[i]);
        else
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data[i]);


    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    isOnGPU = true;
}


void CubeTexture::unloadFromRAM()
{
    for(int i = 0; i < 6; i++)
        stbi_image_free(data[i]);

    isOnRAM = false;
}

void CubeTexture::unloadFromGPU()
{
    //TODO

    isOnGPU = false;
}



EnvironmentTex::EnvironmentTex()
{
    isOnRAM = isOnGPU = false;
};

void EnvironmentTex::loadToRAM(const char* filePath)
{
    stbi_set_flip_vertically_on_load(true);
    int nrComponents;
    data = stbi_loadf(filePath,  &width, &height, &nrComponents, 0);

    if (!data)
        std::cout << "Failed to load "<< filePath << " image." << std::endl;
};

void EnvironmentTex::loadToGPU()
{
    glGenTextures(1, &hdrTex);
    glBindTexture(GL_TEXTURE_2D, hdrTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void createCube(GLuint& VAO, GLuint& VBO)
{
    float cubeVerts[] = {
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

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), &cubeVerts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}



void EnvironmentTex::generateCubeMaps(int resolution)
{
    GLuint toCubeFbuff;
    glGenFramebuffers(1, &toCubeFbuff);
    glBindFramebuffer(GL_FRAMEBUFFER, toCubeFbuff);

    auto generateCubeMap = [](GLuint& tex, int res)
    {
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
        for (int i = 0; i < 6; i++)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F, res, res, 0, GL_RGBA, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    };

    GLuint cubeVAO, cubeVBO;
    createCube(cubeVAO, cubeVBO);

    glm::mat4 convertViews[] = 
    {
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3( 1, 0, 0), glm::vec3(0, 0, 1)), //+x
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1)), //-x
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3( 0, 0,-1), glm::vec3(0, 1, 0)), //+y
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3( 0, 0, 1), glm::vec3(0, -1, 0)),  //-y
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3( 0, 1, 0), glm::vec3(0, 0, 1)), //+z
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3( 0,-1, 0), glm::vec3(0, 0, 1))  //-z
    };
//
    generateCubeMap(cubeMap, resolution);
    Shader& convert = Storage::getShader("src/shaders/streched2cube");
    convert.use();
    convert.set1Int("theTex", 0);
    convert.setMat4("projection", glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTex);

    glDisable(GL_DEPTH_TEST); 
    glViewport(0, 0, resolution, resolution);
    GLenum renderTargets[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, renderTargets);
    for (int i = 0; i < 6; i++)
    {
        convert.setMat4("view", convertViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMap, 0);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Fbuff not complete!!!!!\n";




//
    //GLuint diffRadFbuff;
    //glGenFramebuffers(1, &diffRadFbuff);
    //glBindFramebuffer(GL_FRAMEBUFFER, diffRadFbuff);

    glm::mat4 views[] = 
    {
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3( 1, 0, 0), glm::vec3(0,-1, 0)), //+x
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0)), //-x
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3( 0, 1, 0), glm::vec3(0, 0, 1)), //+y
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3( 0,-1, 0), glm::vec3(0, 0, -1)),  //-y
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3( 0, 0, 1), glm::vec3(0,-1, 0)), //+z
        glm::lookAt(glm::vec3(0, 0, 0), glm::vec3( 0, 0,-1), glm::vec3(0,-1, 0))  //-z
    };


    int diffRadRes = 64;

    generateCubeMap(diffRadianceMap, diffRadRes); 


    Shader& diffRadGen = Storage::getShader("src/shaders/diffRadGen");
    diffRadGen.use();
    diffRadGen.set1Int("envTex", 0);
    diffRadGen.setMat4("projection", glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f));


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

    glViewport(0, 0, diffRadRes, diffRadRes);

    for (int i = 0; i < 6; i++)
    {
        diffRadGen.setMat4("view", views[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, diffRadianceMap, 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Fbuff not complete!!!!!\n";

//
    int prefilterRes = 400;

    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (int i = 0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F, prefilterRes, prefilterRes, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    Shader& prefilterGen = Storage::getShader("src/shaders/prefilterGen");
    prefilterGen.use();
    prefilterGen.setMat4("projection", glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f));
    prefilterGen.set1Int("envTex", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

    unsigned int maxMipLevels = 5;
    for (int mip = 0; mip < (int)maxMipLevels; mip++)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipRes  = prefilterRes * std::pow(0.5, mip);
        glViewport(0, 0, mipRes, mipRes);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterGen.set1Float("roughness", roughness);
        for (int i = 0; i < 6; i++)
        {
            prefilterGen.setMat4("view", views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
}


