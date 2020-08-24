
#include "space.h"
#include "slgh_compile.h"

AddrSpace*          AddrSpace_new2(SleighCompile *m, spacetype tp)
{
    AddrSpace *spc = vm_mallocz(sizeof(spc[0]));

    spc->refcount = 0;
    spc->manage = m;
    spc->trans = m;
    spc->type = tp;
    spc->flags |= heritaged;
    spc->flags |= does_deadcode;
    spc->wordsize = 1;
    spc->minimumPointerSize = 0;
    spc->shortcut = ' ';

    return spc;
}

AddrSpace*          AddrSpace_new8(SleighCompile *m, spacetype tp, const char *name, u4 size, u4 ws, int ind, u4 fl, int dl)
{
    AddrSpace *spc = vm_mallocz(sizeof(spc[0]) + strlen(name));

    spc->refcount = 0;
    spc->manage = m;
    spc->trans = m;
    strcpy(spc->name, name);
    spc->addrsize = size;
    spc->wordsize = ws;
    spc->index = ind;
    spc->delay = dl;
    spc->deadcodedelay = dl;
    spc->minimumPointerSize = 0;
    spc->shortcut = ' ';

    spc->flags = (fl & hashpysical);

    return spc;
}

uintb  AddrSpace_wrapOffset(AddrSpace *s, uintb off)
{
    if (off <= s->highest)
        return off;

    intb mod = (intb)(s->highest + 1);
    intb res = (intb)(off % mod);
    if (res < 0)
        res += mod;
    return (uintb)res;
}

bool VarnodeData_less(const VarnodeData *op1, const VarnodeData *op2) {
    return false;
}
