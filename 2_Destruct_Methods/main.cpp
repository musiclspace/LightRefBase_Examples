#include "Person.h"
#include "sp.h"
void test_func(void)
{
    sp p = new Person();
    p->showinfo();
}
int main(int argc, char **argv)
{
    int i;
    for(i = 0; i < 2;i++)
    {
        test_func();
    }
    return 0;
}