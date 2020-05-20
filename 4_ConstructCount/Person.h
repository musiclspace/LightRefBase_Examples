//Person.h
#ifndef PERSON_H
#define PERSON_H
#include "Refbase.h"
#include <iostream>
class Person : public Refbase
{
public:
    Person();
    ~Person();
    void showinfo(void);
};
#endif