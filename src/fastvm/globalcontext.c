
#include "vm.h"

ContextBitRange*    ContextBitRange_newV(void)
{
    return vm_mallocz(sizeof(ContextBitRange));
}

ContextBitRange*    ContextBitRange_new(int sbit, int ebit)
{
    ContextBitRange *cb = vm_mallocz(sizeof(ContextBitRange));

    cb->word = sbit / (8 * sizeof(uintm));
    cb->startbit = sbit - cb->word * 8 * sizeof(uintm);
    cb->endbit = ebit - cb->word * 8 * sizeof(uintm);
    cb->shift = 8 * sizeof(uintm) - cb->endbit - 1;
    cb->mask = (~((uintm)0)) >> (cb->startbit + cb->shift);

    return cb;
}

void                ContextBitRange_setValue(ContextBitRange *cb, uintm *vec, uintm val)
{
    uintm newval = vec[cb->word];

    newval &= ~(cb->mask << cb->shift);
    newval |= ((val & cb->mask) << cb->shift);
    vec[cb->word] = newval;
}

uintm               ContextBitRange_getValue(ContextBitRange *cb, uintm *vec)
{
    return (vec[cb->word] >> cb->shift) & cb->mask;
}

FreeArray*      FreeArray_new()
{
    return vm_mallocz(sizeof(FreeArray));
}

void            FreeArray_delete(FreeArray *fa)
{
    vm_free(fa);
}

int             FreeArray_assign(FreeArray *op1, FreeArray *op2)
{
    int i;

    if (op1->size) {
        vm_free(op1->array);
        vm_free(op1->mask);
    }

    op1->array = 0;
    op1->mask = 0;
    op1->size = op2->size;

    if (op1->size) {
        op1->array = vm_mallocz(op1->size * sizeof(op1->array[0]));
        op1->mask = vm_mallocz(op1->size * sizeof(op1->mask[0]));

        memcpy(op1->array, op2->array, sizeof(op1->array[0]) * op1->size);
        //memcpy(op1->mask, op2->mask, sizeof(op2->mask[0]) * op2->size);
    }

    return 0;
}

void            FreeArray_reset(FreeArray *f, int sz)
{
    uintm *newarray = NULL;
    uintm *newmask = NULL;

    if (sz) {
        newarray = vm_mallocz(sizeof(newarray[0]) * sz);
        newmask = vm_mallocz(sizeof(newmask[0]) * sz);
    }
}


ContextDatabase*    ContextDatabase_new()
{
    return vm_mallocz(sizeof(ContextDatabase));
}

void                ContextDatabase_delete(ContextDatabase *cd)
{
    vm_free(cd);
}

void            ContextDatabase_registerVariable(ContextDatabase *cd, char *name, int sbit, int ebit)
{
    if (cd->database.count)
        vm_error("Cannot register new context variables after databse is initialized");

    ContextBitRange *bitrange = ContextBitRange_new(sbit, ebit);
    int sz = sbit / (8 * sizeof(uintm)) + 1;
    if (ebit / (8 * sizeof(uintm) + 1) != sz)
        vm_error("Context  variables does not fit in one word");
    if (sz > cd->size) {
        //((FreeArray *)cd->database.defaultvalue)
    }
}

uintm*              ContextDatabase_getContext(Address *addr)
{
}
