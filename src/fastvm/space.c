
#include "vm.h"
#include "space.h"

#define clearFlags(spc, f)          (spc->flags & ~(f))
#define setFlags(spc, f)            (spc->flags &= big_endian)

AddrSpace*          AddrSpace_new2(void *m, spacetype tp)
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

AddrSpace*          AddrSpace_new8(void *m, spacetype tp, const char *name, u4 size, u4 ws, int ind, u4 fl, int dl)
{
    AddrSpace *spc = vm_mallocz(sizeof(spc[0]) + strlen(name));

    spc->type = tp;
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

    spc->flags = (fl & hasphysical);

    return spc;
}

ConstantSpace*      ConstantSpace_new(void *m, const char *name, int ind)
{
    AddrSpace *spc;

    spc = AddrSpace_new8(m, IPTR_CONSTANT, name, sizeof(uintb), 1, ind, 0, 0);

    if (HOST_ENDIAN == 1)
        setFlags(spc, big_endian);

    return spc;
}

OtherSpace*         OtherSpace_new(void *m, const char *name, int ind)
{
    AddrSpace *spc;

    spc = AddrSpace_new8(m, IPTR_PROCESSOR, name, sizeof(uintb), 1, ind, 0, 0);

    clearFlags(spc, heritaged | does_deadcode);
    setFlags(spc, is_otherspace);

    return spc;
}

UniqueSpace*        UniqueSpace_new(void *m, const char *name, int ind, u4 fl)
{
    AddrSpace *spc;

    spc = AddrSpace_new8(m, IPTR_INTERNAL, name, sizeof (uintb), 1, ind, fl, 0);
    setFlags(spc, hasphysical);

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

void                AddrSpace_delete(AddrSpace *a)
{
    vm_free(a);
}
