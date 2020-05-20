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
    p->incStrong();
}
sp::~sp()
{
    cout << "~sp()" << endl;
    p->decStrong();
    if(p->getStrongCnt() == 0)
        delete p;
}
Person * sp::operator->(void)
{
    return p;
}