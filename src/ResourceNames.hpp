#pragma once

#define createNamedResourceStorageBase(namespaceName)                    \
 namespace namespaceName                                             \
 {                                                                   \
    std::vector<std::string> filePaths = {"this time indexing from 1 :C"};\
 }

#define addEnum(theNamespace, enumName) namespace theNamespace{ enum{enumName = __COUNTER__};}
#define addToVector(theVector, value, uniqueVariableName) namespace{char xXx##uniqueVariableName##xXx = (theVector.emplace_back(value), 69);}
#define addResource(theNamespace, enumName, path) addEnum(theNamespace, enumName)  addToVector(theNamespace::filePaths, path, enumName)



#include "TexNames.hpp"
#include "MeshNames.hpp"


#undef addEnum
#undef addToVector
#undef addResource
