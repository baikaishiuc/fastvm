
#include "vm.h"
#include "semantics.h"

#define OpTpl_getIn(o, i)           ((VarnodeTpl *)o->input.ptab[i])


OpTpl*      OpTpl_new()
{
    OpTpl *op = vm_mallocz(sizeof (op[0]));

    return op;
}

OpTpl*      OpTpl_new1(OpCode oc)
{
    OpTpl *op = vm_mallocz(sizeof(op[0]));

    op->opc = oc;

    return op;
}
void        OpTpl_delete(OpTpl *o)
{
    vm_free(o);
}

void        OpTpl_clearOutput(OpTpl *o)
{
    VarnodeTpl_delete(o->output);
    o->output = NULL;
}

void        OpTpl_addInput(OpTpl *o, VarnodeTpl *vt)
{
    dynarray_add(&o->input, vt);
}

ConstructTpl*   ConstructTpl_new()
{
    ConstructTpl *t = vm_mallocz(sizeof(t[0]));

    return t;
}

void            ConstructTpl_delete(ConstructTpl *c)
{
}

bool            ConstructTpl_addOpList(ConstructTpl *c, struct dynarray *oplist)
{
    int i;
    for (i = 0; i < oplist->len; i++) {
        if (!ConstructTpl_addOp(c, oplist->ptab[i]))
            return false;
    }

    return true;
}

bool            ConstructTpl_addOp(ConstructTpl *t, OpTpl *ot)
{
    if (ot->opc == DELAY_SLOT) {
        if (t->delayslot)
            return false;

        t->delayslot = (u4)OpTpl_getIn(ot, 0)->offset->value_real;
    }
    else if (ot->opc == LABELBUILD) {
        t->numlabels++;
    }

    dynarray_add(&t->vec, ot);
    return true;
}

ConstTpl*   ConstTpl_clone(ConstTpl *a)
{
    ConstTpl *c = vm_mallocz(sizeof (c[0]));

    c->type = a->type;
    c->value = a->value;
    c->value_real = a->value_real;
    c->select = a->select;

    return c;
}

ConstTpl*   ConstTpl_newA(AddrSpace *space)
{
    ConstTpl *c = vm_mallocz(sizeof(c[0]));

    c->type = spaceid;
    c->value.spaceid = space;
    return c;
}

ConstTpl*   ConstTpl_new0(void)
{
    ConstTpl *c = vm_mallocz(sizeof (c[0]));

    return c;
}

ConstTpl*   ConstTpl_new1(const_type tp)
{
    return NULL;
}

ConstTpl*   ConstTpl_new2(const_type tp, uintb val)
{
    ConstTpl *c = vm_mallocz(sizeof (c[0]));

    c->type = tp;
    c->value_real = val;
    c->select = v_space;

    return c;
}

ConstTpl*   ConstTpl_new3(const_type tp, int4 ht, v_field vf)
{
    ConstTpl *c = vm_mallocz(sizeof(c[0]));

    c->type = tp;
    c->value.handle_index = ht;
    c->select = vf;

    return c;
}

ConstTpl*   ConstTpl_new4(const_type tp, int4 ht, v_field vf, uintb plus)
{
    return NULL;
}

void        ConstTpl_delete(ConstTpl *c)
{
    vm_free(c);
}

void        ConstTpl_printHandleSelector(FILE *fout, v_field val)
{

}
v_field     ConstTpl_readHandleSelector(const char *name)
{
    return v_space;
}

bool        ConstTpl_isEqual(ConstTpl *lhs, ConstTpl *rhs)
{
    if (lhs->type != rhs->type) return false;

    switch (lhs->type) {
    case real:
        return (lhs->value_real == rhs->value_real);

    case handle:
        return (lhs->value.handle_index == rhs->value.handle_index) && (lhs->select == rhs->select);

    case spaceid:
        return (lhs->value.spaceid == rhs->value.spaceid);
    }

    return false;
}

bool    ConstTpl_lessThan(ConstTpl *lhs, ConstTpl *rhs)
{
    if (lhs->type != rhs->type) return (lhs->type < rhs->type);

    switch (lhs->type) {
    case real:
        return (lhs->value_real < rhs->value_real);

    case handle:
        if (lhs->value.handle_index != rhs->value.handle_index)
            return (lhs->value.handle_index < rhs->value.handle_index);

        if (lhs->select != rhs->select) return (lhs->select < rhs->select);
        break;

    case spaceid:
        return (lhs->value.spaceid < rhs->value.spaceid);
    }

    return false;
}

VarnodeTpl*     VarnodeTpl_new()
{
    return NULL;
}

VarnodeTpl*     VarnodeTpl_clone(VarnodeTpl *vn)
{
    VarnodeTpl *v = vm_mallocz(sizeof (v[0]));

    v->space = ConstTpl_clone(vn->space);
    v->size = ConstTpl_clone(vn->size);
    v->offset = ConstTpl_clone(vn->offset);

    return v;
}

VarnodeTpl*     VarnodeTpl_new2(int hand, bool zerosize)
{
    VarnodeTpl *v = vm_mallocz(sizeof (v[0]));

    v->space = ConstTpl_new3(handle, hand, v_space);
    v->offset = ConstTpl_new3(handle, hand, v_offset);

    if (zerosize)
        v->size = ConstTpl_new2(real, 0);
    else
        v->size = ConstTpl_new3(handle, hand, v_size);

    v->unnamed_flag = false;

    return v;
}

VarnodeTpl*     VarnodeTpl_new3(ConstTpl *sp, ConstTpl *off, ConstTpl *sz)
{
    VarnodeTpl *v = vm_mallocz(sizeof (v[0]));

    v->space = sp;
    v->offset = off;
    v->size = sz;

    return v;
}

void            VarnodeTpl_delete(VarnodeTpl *vn)
{
    vm_free(vn);
}

bool     VarnodeTpl_isLocalTemp(VarnodeTpl *vn)
{
    if (vn->space->type != spaceid)   return false;
    /* unique space */
    if (vn->space->value.spaceid->type != IPTR_INTERNAL) return false;

    return true;
}
