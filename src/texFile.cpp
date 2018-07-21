#include <vector>
#include <stb_image.h>
#include <Logger.hpp>
#include <glad/glad.h>
#include <Utils.hpp>
#include <ImageProcessor.hpp>
#include <Shader.hpp>
#include <Storage.hpp>
#include <GLFW/glfw3.h>
#include <Window.hpp>

namespace texFile
{
    int create(const char* imagePath, const char* texFilePath)
    {
        bool isNormalMap = false;

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        void* imageData = stbi_load(imagePath, &width, &height, &nrChannels, 0);

        if(!imageData)
        {
            std::cout << "[ERROR(TEXCREATE)] Failed to load " << imagePath << "!!\n";
            return 0;
        }

        auto internalFormat = getTexInternalFormat(nrChannels, false);

        GLuint texIndx;
        glGenTextures(1, &texIndx);
        glBindTexture(GL_TEXTURE_2D, texIndx);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat.first, width, height, 0, internalFormat.second, GL_UNSIGNED_BYTE, imageData);
        checkGlError();

        ImageProcessor imageProcessor(width, height, internalFormat.first, internalFormat.second, GL_UNSIGNED_BYTE, imageData);
        auto texFilterShader = Storage::getShader("src/shaders/texFilter/texFilter");
        texFilterShader.use();
        imageProcessor.render();
        
        glBindTexture(GL_TEXTURE_2D, imageProcessor.renderTexture);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        //glReadPixels(0, 0, width, height, internalFormat.second, GL_UNSIGNED_BYTE, imageData);



        auto file = fopen(texFilePath, "wb");
        fwrite(&width, sizeof(int), 1, file);
        fwrite(&height, sizeof(int), 1, file);
        fwrite(&nrChannels, sizeof(int), 1, file);
        fwrite(imageData, sizeof(char), width*height*nrChannels, file);
        fclose(file);

        std::cout << "[TEXCREATE] - succesfully created " << texFilePath << " \n";

        stbi_image_free(imageData);

        return 1;
    }

    int load(const char* filePath, std::vector<void*>& data, int& width, int& height, int& nrChannels)
    {
        auto file = fopen(filePath, "rb");

        if(!file)
        {
            std::cout << "[ERROR] failed to load " << filePath << "!!\n";
            return 0;
        }

        fread(&width, sizeof(int), 1, file);
        fread(&height, sizeof(int), 1, file);
        fread(&nrChannels, sizeof(int), 1, file);
        
        data.emplace_back(malloc(width*height*nrChannels));
        fread(data[0], sizeof(char), width*height*nrChannels, file);
        fclose(file);

        return 1;
    }
}