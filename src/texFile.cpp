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

//Generating mipmaps
        bool genOnGPU = false;

        std::vector<void*> mipmaps;

        if(!genOnGPU)
        {   //CPU mode

            auto getPix = [nrChannels](void* data, int width, int x, int y)->unsigned char*{
                return (unsigned char*)data + nrChannels*sizeof(unsigned char)*y*width + nrChannels*sizeof(unsigned char)*x;
                };

            for(int curDiv = 2; std::min(width,height) / curDiv > 0; curDiv *= 2)
            {
                int curWidth = width/curDiv;
                int curHeight = height/curDiv;  

                mipmaps.emplace_back(malloc(sizeof(unsigned char)*curWidth*curHeight*4));

                for(int x = 0; x < curWidth; x++)
                for(int y = 0; y < curHeight; y++)
                {
                    auto getPrevPix = [&](int x, int y)->unsigned char*{
                      return getPix((curDiv == 2 ? imageData : mipmaps[mipmaps.size()-2]), width / (curDiv/2), x, y);
                    };

                    unsigned char* newPix = new unsigned char[nrChannels];

                    for(int c = 0; c < nrChannels; c++)
                    {
                        newPix[c] = ((int)getPrevPix(2*x, 2*y)[c]+getPrevPix(2*x+1, 2*y)[c]+getPrevPix(2*x, 2*y+1)[c]
                        +getPrevPix(2*x+1, 2*y+1)[c]) / 4;
                    }


                    memcpy(getPix(mipmaps.back(), curWidth, x, y), 
                    newPix, 
                    nrChannels*sizeof(unsigned char));
                }
            }
        }
        else
        {   //GPU mode
            //Load texture to GPU
            GLuint tex;
            glGenTextures(1, &tex);
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
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

            for(int curDiv = 2; std::min(width,height) / curDiv > 0; curDiv *= 2)
            {
                int curWidth = width/curDiv;
                int curHeight = height/curDiv;  

                mipmaps.emplace_back(malloc(sizeof(unsigned char)*curWidth*curHeight*4));

                glViewport(0, 0, curWidth, curHeight);
                glDisable(GL_DEPTH_TEST); 
                glBindVertexArray(screenQuad.VAO);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, tex);

                glDrawArrays(GL_TRIANGLES, 0, 6);

                glReadBuffer(GL_COLOR_ATTACHMENT0);
                glReadPixels(0, 0, curWidth, curHeight, internalFormat.second, GL_UNSIGNED_BYTE, mipmaps.back());
                glFinish();

                glBindTexture(GL_TEXTURE_2D, tex);
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat.first, curWidth, curHeight, 0, internalFormat.second, GL_UNSIGNED_BYTE, 0);
                glCopyTextureSubImage2D(tex, 0, 0, 0, 0, 0, curWidth, curHeight);
                checkGlError();

                glFinish();
            }

            checkGlError();
        }
        

        if(0) //onlyMipMap (for DEBUG)
        {
            int mipmapLvl = 4;

            auto file = fopen(texFilePath, "wb");
            width /= pow(2, mipmapLvl); height /= pow(2, mipmapLvl);
            fwrite(&width, sizeof(int), 1, file);
            fwrite(&height, sizeof(int), 1, file);
            fwrite(&nrChannels, sizeof(int), 1, file);
            fwrite((mipmapLvl == 0 ? imageData : mipmaps[mipmapLvl-1]), sizeof(char), width*height*nrChannels, file);
            fclose(file);
        }
        else
        {
            auto file = fopen(texFilePath, "wb");
            fwrite(&width, sizeof(int), 1, file);
            fwrite(&height, sizeof(int), 1, file);
            fwrite(&nrChannels, sizeof(int), 1, file);
            fwrite(imageData, sizeof(char), width*height*nrChannels, file);

            auto it = mipmaps.begin();
            for(int curDiv = 2; std::min(width,height) / curDiv > 0; curDiv *= 2)
            {
                int curWidth = width/curDiv;
                int curHeight = height/curDiv;  

                fwrite(*it++, sizeof(unsigned char), curWidth*curHeight*nrChannels, file);
            }
            
            fclose(file);
        }

        //free(imageData);

        //for(auto t : mipmaps)free(t);

        say << "[TEXCREATE] - succesfully created " << texFilePath << " \n";

        stbi_image_free(imageData);

        return 1;
    }

    int load(const char* filePath, unsigned char*& data, std::vector<unsigned char*>& mipmaps, int& width, int& height, int& nrChannels)
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
        
        data = (unsigned char*)malloc(width*height*nrChannels);
        fread(data, sizeof(char), width*height*nrChannels, file);

        for(int curDiv = 2; std::min(width/curDiv, height/curDiv) > 0; curDiv  *= 2)
        {
            int curWidth = width/curDiv;
            int curHeight = height/curDiv;

            mipmaps.emplace_back((unsigned char*)malloc(curWidth*curHeight*sizeof(unsigned char)*4));

            fread(mipmaps.back(), sizeof(unsigned char), curWidth*curHeight*nrChannels, file);
        }

        fclose(file);

        return 1;
    }
}
