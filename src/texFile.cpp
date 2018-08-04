#include <vector>
#include <stb_image.h>
#include <Logger.hpp>
#include <glad/glad.h>
#include <Utils.hpp>
#include <Shader.hpp>
#include <Storage.hpp>
#include <GLFW/glfw3.h>
#include <Window.hpp>

namespace texFile
{
    int create(const char* imagePath, const char* texFilePath)
    {
        bool isNormalMap = false;

//Loading from file
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        void* imageData = stbi_load(imagePath, &width, &height, &nrChannels, 0);

        if(!imageData)
        {
            say << "[ERROR(TEXCREATE)] Failed to load " << imagePath << "!!\n";
            return 0;
        }

        auto internalFormat = getTexInternalFormat(nrChannels, false);

        /*
//Generating mipmaps
        //Load texture to GPU
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat.first, width, height, 0, internalFormat.second, GL_UNSIGNED_BYTE, imageData);

        //Framebuffer creation  
        GLuint fbuff, renderTex;
        glGenFramebuffers(1, &fbuff);
        glBindFramebuffer(GL_FRAMEBUFFER, fbuff);

        //Render texture creation
        glGenTextures(1, &renderTex);
        glBindTexture(GL_TEXTURE_2D, renderTex);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat.first, width, height, 0, internalFormat.second, GL_UNSIGNED_BYTE, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);

        //Screen quad to render onto
        struct
        {
            float verts[12] = {-1,1, -1,-1, 1,-1, -1,1, 1,-1,  1,1};
            GLuint VAO, VBO;
        } screenQuad;

        glGenVertexArrays(1, &screenQuad.VAO);
        glBindVertexArray(screenQuad.VAO);

        glGenBuffers(1, &screenQuad.VBO);
        glBindBuffer(GL_ARRAY_BUFFER, screenQuad.VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuad.verts), &screenQuad.verts, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

        checkGlError();

        auto texFilterShader = Storage<Shader>::get("src/shaders/texFilter/texFilter");
        texFilterShader.use();

        std::vector<void*> mipmaps;
        for(int curDiv = 2; std::min(width,height) / curDiv > 0; curDiv *= 2)
        {
            int curWidth = width/curDiv;
            int curHeight = height/curDiv;

            mipmaps.emplace_back(malloc(sizeof(unsigned char)*curWidth*curHeight*nrChannels));

            glViewport(0, 0, curWidth, curHeight);
            glDisable(GL_DEPTH_TEST); 
            glBindVertexArray(screenQuad.VAO);

            glActiveTexture(GL_TEXTURE0);
            //glBindTexture(GL_TEXTURE_2D, );

            glDrawArrays(GL_TRIANGLES, 0, 6);
            glFinish();
        }




        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glReadPixels(0, 0, width, height, internalFormat.second, GL_UNSIGNED_BYTE, imageData);
        */

        auto file = fopen(texFilePath, "wb");
        fwrite(&width, sizeof(int), 1, file);
        fwrite(&height, sizeof(int), 1, file);
        fwrite(&nrChannels, sizeof(int), 1, file);
        fwrite(imageData, sizeof(char), width*height*nrChannels, file);
        fclose(file);

        say << "[TEXCREATE] - succesfully created " << texFilePath << " \n";

        stbi_image_free(imageData);

        return 1;
    }

    int load(const char* filePath, std::vector<void*>& data, int& width, int& height, int& nrChannels)
    {
        auto file = fopen(filePath, "rb");

        if(!file)
        {
            say << "[ERROR] failed to load " << filePath << "!!\n";
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