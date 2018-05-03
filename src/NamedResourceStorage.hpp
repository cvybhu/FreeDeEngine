#pragma once

#include <vector>
#include <map>

#include <iostream>

template <class ResourceType, int maxObjectNum>
class NamedResourceStorage
{
public:
    ResourceType& operator[](const int& index);
    ResourceType& operator[](const std::string& filePath);

    int getIndex(const std::string& filePath);

    std::string& getFilePath(const int& index);


    NamedResourceStorage(std::vector<std::string>& filePathsToNamedElements);


private:
    ResourceType data[maxObjectNum+2];
    int nxtFreeData = 0;

    std::vector<std::string>& filePaths;

    std::map<std::string, int> filePathToIndex;
};




//#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#


template <class ResourceType, int maxObjectNum>
NamedResourceStorage<ResourceType, maxObjectNum>::NamedResourceStorage(std::vector<std::string>& filePathsToNamedElements) : filePaths{filePathsToNamedElements}
{
    for(int i = 0; i < filePaths.size(); i++)
        filePathToIndex[filePaths[i]] = i;

    filePaths.resize(maxObjectNum+2);
}

template <class ResourceType, int maxObjectNum>
ResourceType& NamedResourceStorage<ResourceType, maxObjectNum>::operator[](const int& index)
{
    return data[index];
}

template <class ResourceType, int maxObjectNum>
ResourceType& NamedResourceStorage<ResourceType, maxObjectNum>::operator[](const std::string& filePath)
{
    return data[getIndex(filePath)];
}

template <class ResourceType, int maxObjectNum>
int NamedResourceStorage<ResourceType, maxObjectNum>::getIndex(const std::string& filePath)
{
    int& index = filePathToIndex[filePath];

    if(index == 0)
    {
        index = nxtFreeData++;
        filePaths[index] = filePath;
    }

    return index;
}

template <class ResourceType, int maxObjectNum>
std::string& NamedResourceStorage<ResourceType, maxObjectNum>::getFilePath(const int& index)
{
    return filePaths[index];
}
