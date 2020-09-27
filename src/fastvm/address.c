
#include "address.h"

void    sign_extend(intb *val, int bit)
{
    intb mask = 0;

    mask = (~mask) << bit;
    if ((val[0] >> bit) & 1)
        val[0] |= mask;
    else
        val[0] &= mask;
}

void        zero_extend(intb *val, int bit)
{
    intb mask = 0;
    mask = (~mask) << bit;
    mask <<= 1;
    val[0] &= ~mask;
}

void    byte_swap(intb *val, int size)
{
    intb res = 0;
    while (size > 0) {
        res <<= 8;
        res |= (val[0] & 0xff);
        val[0] >>= 8;
        size -= 1;
    }

    val[0] = res;
}