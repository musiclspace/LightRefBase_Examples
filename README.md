

@[TOC](LightRefBase 轻指针的实现原理)
## 前言
本文参考了百问科技韦东山先生C++课程中对于智能指针的讲解，在此基础上根据自己的理解，形成本文。特此说明。
正文中的所有源码可以从Github获取。

## 1. 问题引出：指针的内存泄漏
与Java自带内存回收机制不同，C++使用new方法创建类的对象后，如果不执行delete函数，这个对象会一直保留在内存的HEAP区域，导致内存泄漏。
我们来看一个内存泄漏的简单实例：
首先声明一个类：
```cpp
//Person.h
#ifndef PERSON_H
#define PERSON_H
#include <iostream>
class Person
{
public:
    Person();
    ~Person();
    void showinfo(void);
};
#endif
```
然后实现这个类

```cpp
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
```
在main.cpp中创建这个类的指针

```cpp
#include "Person.h"
void test_func(void)
{
    Person *p = new Person();
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
```
然后编辑一个Makefile来编译

```bash
Person : main.o Person.o
	g++ -o $@ $^
%o : %.cpp
	g++ -o $@ -c $<
clean :
	rm -rf *.o Person
```
编译后的运行结果为：
> Person()
showinfo()
Person()
showinfo()

由此可见调用了两次Person的构造函数来创建对象，但是执行完成后并没有调用类的析构函数。导致内存泄漏。原因是使用new方法创建的对象，位于内存的HEAP区域，而HEAP区域是不会像STACK区域那样结束自动回收的。
## 2.  解决思路：类的析构
我们需要考虑如何自动回收指针。一个可行的思路是将指向Person的这个指针，定义为另外一个类的数据成员，通过这个类来创建这个指针，这样类被销毁时，利用析构函数将指针回收。
于是我们重新定义一个sp类来实现这个功能
```cpp
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
```
```cpp
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
```
```cpp
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
```
编译运行
>Person()
sp person * other
showinfo()
~sp()
~Person()
Person()
sp person * other
showinfo()
~sp()
~Person()


可见sp类的析构函数成功将指针以及Person对象释放了。

## 3.  新的问题：拷贝构造引发的重复析构错误
在第二部分的sp类中，已经预先定义了一个拷贝构造函数，在此基础上修改main.cpp 重载一个test_func来调用拷贝构造函数。
修改后的main.cpp如下

```cpp
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
```
编译执行后：报以下错误，重复的内存释放动作。原因就是拷贝构造的Person对象是以形参方式传入的，函数调用结束后，形参会被销毁，而拷贝构造对象与main函数中创建的对象指向同一块内存，这样main函数中的sp other对象被销毁，导致第二次调用时，找不到这个对象。
> *** Error in `./Person': double free or corruption (fasttop): 0x0000000000d98c20 ***
## 4.  引入构造计数解决重复析构（类Java内存回收机制）
如何解决重复析构的问题呢，我们可以参考一下Java的内存回收机制，Java在HEAP中new出来的对象，每关联一个指针就会在内部的计数值+1， 当这个计数值清空，意味着没有指针再指向这部分HEAP时，就会启动回收机制进行回收动作，那么C++当然可以借鉴这个方法。再构造一个计数的类。命名为RefBase

```cpp
//Refbase.h
#ifndef REFBASE_H
#define REFBASE_H
class Refbase {
    int m_count;
public:
    Refbase() : m_count(0) {}
    void incStrong();
    void decStrong();
    int getStrongCnt();
};
#endif
```
```cpp
//Refbase.cpp
#include "Refbase.h"
void Refbase::incStrong()
{
    m_count++; 
}
void Refbase::decStrong()
{
    m_count--;
}
int Refbase::getStrongCnt()
{
    return m_count;
}
```
然后让Person类来继承这个类

```cpp
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
```
同时处理sp类析构过程中判断构造计数

```cpp
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
```
编译后执行
> Person()
sp person * other
showinfo()
~sp()
~Person()
showinfo()
~sp()
~sp()

## 5.  引入类模板来提高代码的可重用性

sp类需要创建一个Person类的指针成员，sp类与目标类存在很强的关联性，因此可以将sp类设置为类模板，将Person类作为一个模板参数传入，使得sp类可以更加通用。
==**注意模板类的声明和实现都必须放到 头文件中**==

```cpp
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
```
然后重写一下main函数中的调用形式

```cpp
#include "Person.h"
#include "sp.h"
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
```
编译之后运行得到了与上一部分完全一致的运行结果。

## 6. 新的问题：线程安全性
考虑到一个进程中如果存在多个线程，那么上述针对指针的保护可能会失效。这是因为在RefBase中对于m_count的操作不是原子的，是可能被打断的，一旦被打断就会引起不可预料的结果。因此我们需要将m_count的操作变为原子级别。这就引申出了本文的主角
LightRefBase类
从Android源码路径
> frameworks/rs/cpp

中可以找到三个文件
RefBase.h
StrongPointer.h
TypeHelpers.h

我们直接将其包含到工程中，删除掉我们自己写的Refbase 和 sp 类文件
这样以来Person类改写为

```cpp
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
```
main.cpp中代码无须更改，直接运行就可以得到正常的结果。从而如果我们想使用智能指针只需要将我们的目标类继承LightRefBase模板，然后使用sp类来创建对象就可以实现简单的自动回收。
