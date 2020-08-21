
#include "semantics.h"

ConstructTpl*   ConstructTpl_new()
{
    return NULL;
}

void            ConstructTpl_delete(ConstructTpl *c)
{
}

bool            ConstructTpl_addOpList(ConstructTpl *c, struct dynarray *oplist)
{
    return false;
}

ConstTpl*   ConstTpl_clone(ConstTpl *a)
{
    return NULL;
}

ConstTpl*   ConstTpl_newA(AddrSpace *space)
{
    return NULL;
}

ConstTpl*   ConstTpl_new0(void)
{
    return NULL;
}

ConstTpl*   ConstTpl_new1(const_type tp)
{
    return NULL;
}

ConstTpl*   ConstTpl_new2(const_type tp, uintb val)
{
    return NULL;
}

ConstTpl*   ConstTpl_new3(const_type tp, int4 ht, v_field vf)
{
    return NULL;
}

ConstTpl*   ConstTpl_new4(const_type tp, int4 ht, v_field vf, uintb plus)
{
    return NULL;
}

void        ConstTpl_delete(ConstTpl *);
void        ConstTpl_printHandleSelector(FILE *fout, v_field val);
v_field     ConstTpl_readHandleSelector(const char *name);

VarnodeTpl*     VarnodeTpl_new()
{
    return NULL;
}

VarnodeTpl*     VarnodeTpl_new1(VarnodeTpl *vn)
{
    return NULL;
}

VarnodeTpl*     VarnodeTpl_new2(int hand, bool zerosize)
{
    return NULL;
}

VarnodeTpl*     VarnodeTpl_new3(ConstTpl *sp, ConstTpl *off, ConstTpl *sz)
{
    return NULL;
}

void            VarnodeTpl_delete(VarnodeTpl *vn)
{
}
