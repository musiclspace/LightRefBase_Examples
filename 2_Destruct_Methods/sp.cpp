//sp.cpp
#include "sp.h"
using namespace std;
sp::sp()
{
}
sp::sp(Person *other)
{
    cout << "sp person * other" << endl;
    p = other;
}
sp::~sp()
{
    cout << "~sp()" << endl;
    if(p)
        delete p;
}
Person * sp::operator->(void)
{
    return p;
}