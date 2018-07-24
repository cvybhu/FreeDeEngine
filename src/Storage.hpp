#pragma once
#include <map>
#include <string>
#include <Logger.hpp>

template <class T>
class Storage
{  
public:
    static T& add(const char* name){return storage[name];}

    template <class... Args>
    static T& add(const char* name, Args&&... args){return (*storage.emplace(name, std::forward<Args>(args)...).first).second;}

    static bool isThere(const char* name){return storage.find(name) != storage.end();}

    static T& get(const char* name)
    {
        auto res = storage.find(name);
        if(res == storage.end())
        {
            say << "[STORAGE ERROR] - " << name << " not found!\n";
            abort();
        }
        else
            return res->second;
    }

//private:
    static std::map<std::string, T> storage;
};


template <class T> std::map<std::string, T> Storage<T>::storage;
