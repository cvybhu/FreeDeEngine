#pragma once
#include <iostream>
#include <fstream>
using namespace std;


template <class T> ostream& _debWrite(ostream& os, const T& a)
    {return os << a;}

template <class T, class... Args> ostream& _debWrite(ostream& os, const T& a, const Args... args)
{
    os << a;
    return _debWrite(os, args...);
}


template <class A, class B> ostream& operator<<(ostream& os, const pair<A,B>& p)
    {return _debWrite(os, "(", p.first, ", ", p.second, ")");}


template <class T, class I = typename T::const_iterator>
typename enable_if<!is_same<T, string>::value, ostream&>::type
operator<<(ostream& os, const T& thing)
{
    os << "{";

    auto it = thing.begin();

    while(it != thing.end())
    {
        _debWrite(os, *it);

        it++;

        if(it != thing.end())
        {
            os << ' ';
        }
    }

    return os << "}";
}

template <class T> void _debWriteInLines(ostream& os, const T& thing)
{
    os << "{\n";
    int i = 0;
    for(auto&& a : thing)
        _debWrite(os, i++, ": ", a, '\n');
    os << "}\n";
}


#define NL cout<<endl;
#define L(a) _debWrite(cout, #a " = ", a, "   ");
#define LN(a) {_debWrite(cout, #a " = ", a); NL;}
#define LOG(a) {_debWrite(cout, #a " = "); _debWriteInLines(cout, a); NL}


