#pragma once
#include <map>
#include <string>

template <class T>
class Storage
{  
public:
    static T& add(const char* name){return storage[name];}

    template <class... Args>
    static T& add(const char* name, Args&&... args){return (*storage.emplace(name, std::forward<Args>(args)...).first).second;}

    static T& get(const char* name){return storage[name];}

private:
    static std::map<std::string, T> storage;
};


template <class T> std::map<std::string, T> Storage<T>::storage;