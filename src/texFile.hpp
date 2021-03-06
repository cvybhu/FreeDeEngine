#pragma once
#include <vector>
#include <glad/glad.h>

namespace texFile
{
    int create(const char* imagePath, const char* texFilePath);
    int load(const char* filePath, unsigned char*& data, std::vector<unsigned char*>& mipmaps, int& width, int& height, int& nrChannels);
}
