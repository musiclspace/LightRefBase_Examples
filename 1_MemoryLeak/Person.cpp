//Person.cpp
#include "Person.h"
using namespace std;

Person::Person()
{
    cout << "Person()" << endl;
}
Person::~Person()
{
    cout << "~Person()" << endl;
}
void Person::showinfo(void)
{
    cout << "showinfo()" << endl;
}