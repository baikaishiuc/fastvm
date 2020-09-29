
#ifndef __ADDRESS_H__
#define __ADDRESS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "space.h"

typedef struct Address      Address;

struct Address {
    AddrSpace *base;
    intb offset;

    enum {
        m_minimal,
        m_maximal,
    } mach_extreme;
};

Address*        Address_newV(void);
Address*        Address_clone(Address *op2);
Address*        Address_new1(int ex);
Address*        Address_new2(AddrSpace *id, uintb off);
void            Address_delete(Address *op);

#define Address_isInvalid(a)    (a->base == NULL)
#define Address_getAddrSize(a)  AddrSpace_getAddrSize(a->base)
#define Address_isBigEndian(a)  AddrSpace_isBigEndian(a->base)

inline Address* Address_assign(Address *op1, Address *op2) 
{
    op1->base = op2->base;
    op1->offset = op2->offset;

    return op1;
}

inline bool Address_equal(Address *op1, Address *op2)
{
    return (op1->base == op2->base) && (op1->offset == op2->offset);
}

inline bool Address_unequal(Address *op1, Address *op2)
{
    return (op1->base != op2->base) || (op1->offset != op2->offset);
}

inline bool Address_less(Address *op1, Address *op2)
{
    if (op1->base != op2->base) {
        if (op1->base == NULL) return true;
        else if (op1->base == (AddrSpace *)-1) return false;
        else if (op2->base == NULL) return false;
        else if (op2->base == (AddrSpace *)-1) return true;

        return AddrSpace_getIndex(op1->base) < AddrSpace_getIndex(op2->base);
    }

    if (op1->offset != op2->offset) return op1->offset < op2->offset;
    return false;
}

inline bool Address_lessThan(Address *op1, Address *op2)
{
    if (op1->base != op2->base) {
        if (op1->base == NULL) return true;
        else if (op1->base == (AddrSpace *)-1) return false;
        else if (op2->base == NULL) return false;
        else if (op2->base == (AddrSpace *)-1) return true;

        return AddrSpace_getIndex(op1->base) < AddrSpace_getIndex(op2->base);
    }

    if (op1->offset != op2->offset) return op1->offset < op2->offset;
    return true;
}

inline Address* Address_add(Address *op1, int off)
{
    /* 这里的意义已经和原始的代码不一致了，在原先的Ghidra中有涉及拷贝函数，
    他在这里重新new了一个对象，然后赋值给外面 */
    op1->offset = AddrSpace_wrapOffset(op1->base, off);

    return op1;
}

inline uintb calc_mask(int size) {
    uintb uintbmasks[9] = { 
        0, 
        0xff, 
        0xffff, 
        0xffffff, 
        0xffffffff, 
        0xffffffffffLL, 
        0xffffffffffffLL, 
        0xffffffffffffffLL, 
        0xffffffffffffffffLL 
    };

    return uintbmasks[(size < 8) ? size : 8];
}


void        sign_extend(intb *val, int bit);
void        zero_extend(intb *val, int bit);
void        byte_swap(intb *val, int size);
uintb       coveringmask(uintb val);

#ifdef __cplusplus
}
#endif

#endif