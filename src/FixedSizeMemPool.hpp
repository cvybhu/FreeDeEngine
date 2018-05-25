
#pragma once

template <class T>
class FixedSizeMemPool
{
public:
    FixedSizeMemPool();
    FixedSizeMemPool(int newSize);

    void init(int newSize);

    void* getMem();
    void freeMem(void* ptr);
private:
    unsigned char* nxtFreeMem;

    void** freed;
    void** lastFreed;

};

//[]*[]*[]*[]*[]*[]*[]*[]*[]*[]*[]*[]*[]*[]*[]*


template <class T>
FixedSizeMemPool<T>::FixedSizeMemPool(){}

template <class T>
FixedSizeMemPool<T>::FixedSizeMemPool(int newSize){
    init(newSize);
}


template <class T>
void FixedSizeMemPool<T>::init(int newSize)
{
    nxtFreeMem = (unsigned char*)malloc(newSize * sizeof(T));

    freed = (void**)malloc(newSize);
    lastFreed = freed-1;
}


template <class T>
void* FixedSizeMemPool<T>::getMem()
{
    if(lastFreed >= freed)
        return *(lastFreed--);
    else
        return nxtFreeMem++;
}

template <class T>
void FixedSizeMemPool<T>::freeMem(void* ptr)
{
    *(++lastFreed) = ptr;
}
