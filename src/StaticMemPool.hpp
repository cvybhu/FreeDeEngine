#pragma once

#include <vector>

template <class T, int SIZE>
class StaticMemPool
{
public:
    StaticMemPool();

    void* getMem();
    void freeMem(const void*& ptr);

    int size() const;

private:
    unsigned char mem[sizeof(T)*SIZE];
    unsigned char* nxtFreeSpace;
    std::vector<void*> freePlaces;
};







//#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#

template <class T, int SIZE>
StaticMemPool<T, SIZE>::StaticMemPool()
{
    nxtFreeSpace = mem;
}


template <class T, int SIZE>
void* StaticMemPool<T, SIZE>::getMem()
{
    void* res;

    if(freePlaces.empty())
    {
        res = nxtFreeSpace;
        nxtFreeSpace += sizeof(T);
    }
    else
    {
        res = freePlaces.back();
        freePlaces.pop_back();
    }

    return res;
}


template <class T, int SIZE>
void StaticMemPool<T, SIZE>::freeMem(const void*& ptr)
{
    freePlaces.emplace_back(ptr);
}


template <class T, int SIZE>
int StaticMemPool<T, SIZE>::size() const
{
    return SIZE;
}
