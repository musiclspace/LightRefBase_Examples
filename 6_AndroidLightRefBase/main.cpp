#include "Person.h"

template<typename T>
void test_func(void)
{
    sp<T>p = new T();
    p->showinfo();
}
template<typename T>
void test_func(const sp<T> &other)
{
    sp<T> s = other;
    s->showinfo();
}
int main(int argc, char **argv)
{
    int i;
    sp<Person>other = new Person();
    for(i = 0; i < 2;i++)
    {
        test_func(other);
    }
    return 0;
}