#pragma once

//#include <Logger.hpp>

template <class T>
class DynMemPool
{
public:
    DynMemPool();
    DynMemPool(int Size);

    void init(int Size);

    void* getMem();
    void freeMem(void* ptr);

    int size() const;

private:
    unsigned char* mem;
    unsigned char* nxtFreeSpace;

    void** returned;
    void** retEnd;

    int poolSize;
};





//#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#[]#

template<class T>
DynMemPool<T>::DynMemPool()
{
    poolSize = 0;
}

template <class T>
DynMemPool<T>::DynMemPool(int Size)
    {init(Size);}


template <class T>
void DynMemPool<T>::init(int Size)
{
    poolSize = Size;

    mem = (unsigned char*)malloc(sizeof(T)*poolSize);
    returned = (void**)malloc(sizeof(void*)*poolSize);

    nxtFreeSpace = mem;
    retEnd = returned;
}



template <class T>
inline void* DynMemPool<T>::getMem()
{
    //Log << "Another one!\n";

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


template <class T>
inline void DynMemPool<T>::freeMem(void* ptr)
{
    //Log << "He's free!\n";

    *(retEnd++) = ptr;
}



template <class T>
inline int DynMemPool<T>::size() const
{
    return poolSize;
}


