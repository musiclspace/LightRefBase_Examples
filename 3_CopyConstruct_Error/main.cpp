#include "Person.h"
#include "sp.h"
void test_func(void)
{
    sp p = new Person();
    p->showinfo();
}
void test_func(const sp &other)
{
    sp s = other;
    s->showinfo();
}
int main(int argc, char **argv)
{
    int i;
    sp other = new Person();
    for(i = 0; i < 2;i++)
    {
        test_func(other);
    }
    return 0;
}