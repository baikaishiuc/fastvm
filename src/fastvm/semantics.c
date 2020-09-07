
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

/* 只要输入和输出有一个为0大小，则为0 */
bool        OpTpl_isZeroSize(OpTpl *o)
{
    int i;
    if (o->output) {
        if (VarnodeTpl_isZeroSize(o->output))
            return true;
    }

    for (i = 0; i < o->input.len; i++) {
        if (VarnodeTpl_isZeroSize((VarnodeTpl *)o->input.ptab[i]))
            return true;
    }

    return false;
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

int             ConstructTpl_fillinBuild(ConstructTpl *c, struct dynarray *check, AddrSpace *space)
{
    OpTpl *op;
    VarnodeTpl *indvn;
    int i;

    for (i = 0; i < c->vec.len; i++) {
        op = c->vec.ptab[i];
        if (op->opc == BUILD) {
            intb index = OpTpl_getIn(op, 0)->offset->value_real;
            if (check->ptab[index])
                return (int)check->ptab[index];
            check->ptab[index] = (void *)1;
        }
    }

    for (i = 0; i < check->len; i++) {
        if (check->ptab[i] == 0) {
            op = OpTpl_new(BUILD);
            indvn = VarnodeTpl_new3(ConstTpl_newA(space),
                                    ConstTpl_new2(real, i),
                                    ConstTpl_new2(real, 4));
            OpTpl_addInput(op, indvn);
            dynarray_insertB(&c->vec, 0, op);
        }
    }

    return 0;
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
    ConstTpl *c = vm_malloc(sizeof(c[0]));

    c->type = tp;

    return c;
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
    assert(NULL);
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

void        ConstTpl_transfer(ConstTpl *c, struct dynarray *params)
{
    // replace old handles with new handles
    if (c->type != handle)  return;
    HandleTpl *newhandle = params->ptab[c->value.handle_index];
    uintb tmp;

    switch (c->select) {
    case v_space:
        *c = *newhandle->space;
        break;

    case v_offset:
        *c = *newhandle->ptroffset;
        break;

    case v_offset_plus:
        tmp = c->value_real;
        *c = *newhandle->ptroffset;
        if (c->type == real) {
            c->value_real += (tmp & 0xffff);
        }
        else if ((c->type == handle) && (c->select == v_offset)) {
            c->select = v_offset_plus;
            c->value_real = tmp;
        }
        else
            vm_error("Cannot truncate macro input in this way");
        break;

    case v_size:
        *c = *newhandle->size;
        break;
    }
}

bool        ConstTpl_isUniqueSpace(ConstTpl *c)
{
    if (c->type == spaceid)
        return c->value.spaceid->type == IPTR_INTERNAL;
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
    assert(0);
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

void            VarnodeTpl_setOffset(VarnodeTpl *vn, uintb val)
{
    // if vn->offset == NULL?
    vn->offset->value_real = val;
}

int             VarnodeTpl_transfer(VarnodeTpl *vn, struct dynarray *params)
{
    bool doesOffsetPlus = false;
    int handleIndex;
    int plus;
    if ((vn->offset->type == handle) && (vn->offset->select == v_offset_plus)) {
        handleIndex = vn->offset->value.handle_index;
        plus = (int)vn->offset->value_real;
        doesOffsetPlus = true;
    }

    ConstTpl_transfer(vn->space, params);
    ConstTpl_transfer(vn->offset, params);
    ConstTpl_transfer(vn->size, params);
    if (doesOffsetPlus) {
        if (VarnodeTpl_isLocalTemp(vn))
            return plus;
        if (ConstTpl_isZero(((HandleTpl *)params->ptab[handleIndex])->size))
            return plus;
    }

    return -1;
}

HandleTpl*          HandleTpl_new()
{
    HandleTpl *h = vm_mallocz(sizeof(h[0]));

    return h;
}

HandleTpl*          HandleTpl_newV(VarnodeTpl *vn)
{
    HandleTpl *h = vm_mallocz(sizeof(h[0]));

    h->space = vn->space;
    h->size = vn->size;
    h->ptrspace = ConstTpl_new2(real, 0);
    h->ptroffset = vn->offset;

    return h;
}

HandleTpl*          HandleTpl_new5(ConstTpl *spc, ConstTpl *sz, VarnodeTpl *vn, AddrSpace *t_space, uintb offset)
{
    HandleTpl *h = vm_mallocz(sizeof (h[0]));

    h->space = spc;
    h->size = sz;
    h->ptrspace = vn->space;
    h->ptroffset = vn->offset;
    h->ptrsize = vn->size;
    h->temp_space = ConstTpl_newA(t_space);
    h->temp_offset = ConstTpl_new2(real, offset);

    return h;
}

void                HandleTpl_delete(HandleTpl *h)
{
    vm_free(h);
}


PcodeBuilder*       PcodeBuilder_new(void *ref)
{
    PcodeBuilder *pb = vm_mallocz(sizeof(pb[0]));

    pb->ref = ref;

    return pb;
}

void                PcodeBuilder_delete(PcodeBuilder *p)
{
    vm_free(p);
}

void                PcodeBuilder_build(PcodeBuilder *pb, ConstructTpl *construct, int secnum)
{
    if (!construct)
        vm_error("construct is empty");

    int oldbase = pb->labelbase;
    pb->labelbase = pb->labelcount;
    pb->labelcount += construct->numlabels;

    OpTpl *op;
    int i;

    for (i = 0; i < construct->vec.len; i++) {
        op = construct->vec.ptab[i];
        switch (op->opc) {
        case BUILD:
            pb->appendBuild(pb->ref, op, secnum);
            break;

        case DELAY_SLOT:
            pb->delaySlot(pb->ref, op);
            break;

        case LABELBUILD:
            pb->setLabel(pb->ref, op);
            break;

        case CROSSBUILD:
            pb->appendCrossBuild(pb->ref, op, secnum);
            break;

        default:
            pb->dump(pb->ref, op);
            break;
        }
    }

    pb->labelbase = oldbase;
}
