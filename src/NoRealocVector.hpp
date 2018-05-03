#pragma once

template <class T, int maxObjectNum2Power = 20>
class NoRealocVector
{
public:
    NoRealocVector();

    template <class... Args>
    NoRealocVector(const int& initialSize, Args&&... initArgs);


    template <class... Args> void emplace_back(Args&&... args);

    T& operator[](const int& index);

    template <class... Args>
    void resize(const int& newSize, Args&&... initArgs);

    int size();

private:
    unsigned char* memBlocks[maxObjectNum2Power];
    int vecSize;
};



template <class T, int maxObjectNum2Power>
NoRealocVector<T, maxObjectNum2Power>::NoRealocVector() : vecSize(0)
{


}

template <class T, int maxObjectNum2Power>
template <class... Args>
NoRealocVector<T, maxObjectNum2Power>::NoRealocVector(const int& initialSize, Args&&... initArgs)
{
    resize(initialSize, std::forward<Args>(initArgs...));
}



template <class T, int maxObjectNum2Power>
template <class... Args>
NoRealocVector<T, maxObjectNum2Power>::resize(const int& newSize, Args&&... initArgs)
{


}
