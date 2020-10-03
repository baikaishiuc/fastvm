
#include "vm.h"
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

uintb       coveringmask(uintb val)
{
    uintb res = val;
    int sz = 1;
    while (sz < 8 * sizeof(uintb)) {
        res = res | (res >> sz);
        sz <<= 1;
    }

    return res;
}

Address*        Address_newV(void) 
{
    return vm_mallocz(sizeof(Address));
}

Address*        Address_clone(Address *op2) 
{
    Address *a = vm_mallocz(sizeof(a[0]));

    a->base = op2->base;
    a->offset = op2->offset;

    return a;
}

Address*        Address_new1(int ex) 
{
    return NULL;
}

Address*        Address_new2(AddrSpace *id, uintb off) 
{
    Address *a = vm_mallocz(sizeof(a[0]));

    a->base = id;
    a->offset = off;

    return a;
}

void            Address_delete(Address *op) 
{
    vm_free(op);
}

char*           Address_printRaw(Address *a, CString *s)
{
    return NULL;
}
