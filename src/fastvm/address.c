
#include "address.h"

void        zero_extend(intb *val, int bit)
{
    intb mask = 0;
    mask = (~mask) << bit;
    mask <<= 1;
    val[0] &= ~mask;
}