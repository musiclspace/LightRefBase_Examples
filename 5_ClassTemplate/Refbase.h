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
