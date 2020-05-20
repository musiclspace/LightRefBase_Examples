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