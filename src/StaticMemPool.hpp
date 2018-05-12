#pragma once

template <class T, int SIZE>
class StaticMemPool
{
public:
    StaticMemPool();

    void* getMem();
    void freeMem(const void*& ptr);

private:
    unsigned char mem[sizeof(T)*SIZE];
    unsigned char* nxtFreeSpace;

    void* returned[SIZE];
    void** retEnd;
};



//#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#

template <class T, int SIZE>
StaticMemPool<T, SIZE>::StaticMemPool()
{
    nxtFreeSpace = mem;
    retEnd = returned;
}


template <class T, int SIZE>
void* StaticMemPool<T, SIZE>::getMem()
{
    void* res;

    if(retEnd == returned)
    {
        res = nxtFreeSpace;
        nxtFreeSpace += sizeof(T);
    }
    else
    {
        res = *(--retEnd);
    }

    return res;
}


template <class T, int SIZE>
void StaticMemPool<T, SIZE>::freeMem(const void*& ptr)
{
    *(retEnd++) = ptr;
}
