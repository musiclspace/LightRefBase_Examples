//sp.h
#ifndef SP_H
#define SP_H
#include "Person.h"
#include <iostream>
using namespace std;
template<typename T>
class sp
{
    T *p;
public:
    sp();
    sp(T * other);
    ~sp();
    T * operator->(void);
};
#endif

template<typename T>
sp<T>::sp()
{
}
template<typename T>
sp<T>::sp(T *other)
{
    cout << "sp person * other" << endl;
    p = other;
    p->incStrong();
}
template<typename T>
sp<T>::~sp()
{
    cout << "~sp()" << endl;
    p->decStrong();
    if(p->getStrongCnt() == 0)
        delete p;
}
template<typename T>
T * sp<T>::operator->(void)
{
    return p;
}