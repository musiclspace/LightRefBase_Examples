//Person.h
#ifndef PERSON_H
#define PERSON_H
#include "RefBase.h"
#include <iostream>
using namespace std;
using namespace android::RSC;

class Person : public LightRefBase<Person>
{
public:
    Person();
    ~Person();
    void showinfo(void);
};
#endif