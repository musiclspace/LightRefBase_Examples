//sp.h
#ifndef SP_H
#define SP_H
#include "Person.h"
class sp
{
    Person *p;
public:
    sp();
    sp(Person * other);
    ~sp();
    Person * operator->(void);
};
#endif