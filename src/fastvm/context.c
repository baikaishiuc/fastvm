
#include "vm.h"
#include "context.h"

Token*          Token_new(const char *name, int sz, int be, int index)
{
    Token *t = vm_mallocz(sizeof(t[0]) + strlen(name));

    t->size = sz;
    t->bigendian = be;
    t->index = index;
    strcpy(t->name, name);

    return t;
}

void            Token_delete(Token *t)
{
    vm_free(t);
}

ConstructState*         ConstructState_newV(void)
{
    return vm_mallocz(sizeof(ConstructState));
}

void                    ConstructState_delete(ConstructState *cs)
{
    vm_free(cs);
}

ParserContext*  ParserContext_new(ContextCache *ccache)
{
    ParserContext *pc = vm_mallocz(sizeof(pc[0]));

    pc->parsestate = 0;
    pc->contcache = ccache;
    if (ccache) {
    }

    return pc;
}

void            ParserContext_delete(ParserContext *pc)
{
    vm_free(pc);
}

void            ParserContext_initialize(ParserContext *pc, int maxstate, int maxparam, AddrSpace *spc)
{
    pc->const_space = spc;
    pc->state = vm_mallocz(maxstate * sizeof(pc->state[0]));

    int i;
    for (i = 0; i < maxstate; i++) {
        ConstructState *cs = &pc->state[i];
        dynarray_resize(&cs->resolve, maxparam);
    }

    pc->base_state = &pc->state[0];
}

uintm       ParserContext_getInstructionBytes(ParserContext *pc, int bytestart, int size, int off)
{
    off += bytestart;
    if (off >= sizeof(pc->buf))
        vm_error("Instruction is using more that 16 bytes");
    const uint1 *ptr = pc->buf + off;
    uintm res = 0;
    int i;

    for (i = 0; i < size; i++) {
        res <<= 8;
        res |= ptr[i];
    }

    return res;
}

uintm       ParserContext_getContextBytes(ParserContext *pc, int bytestart, int size)
{
    int intstart = bytestart / sizeof(uintm);
    uintm res = pc->context[intstart];
    int byteoffset = bytestart / sizeof(uintm);
    int unusedBytes = sizeof(uintm) - size;
    res <<= byteoffset * 8;
    res >>= unusedBytes * 8;
    int remaining = size - sizeof(uintm) + byteoffset;
    if ((remaining > 0) && (++intstart < pc->contextsize)) {
        uintm res2 = pc->context[intstart];
        unusedBytes = sizeof(uintm) - remaining;
        res2 >>= unusedBytes * 8;
        res |= res2;
    }

    return res;
}

void            ParserContext_loadContext(ParserContext *pc)
{
}

void            ParserContext_setContextWord(ParserContext *pc, int i, uintm val, uintm mask)
{
    pc->context[i] = (pc->context[i] & ~mask) | (mask & val);
}

void            ParserContext_addCommit(ParserContext *pc, SleighSymbol *sym, int num, int mask, bool flow, ConstructState *point)
{
    ContextSet *cs = vm_mallocz(sizeof(cs[0]));

    cs->sym = sym;
    cs->point = point;
    cs->num = num;
    cs->mask = mask;
    cs->value = pc->context[num] & mask;
    cs->flow = flow;

    dynarray_add(&pc->contextcommit, cs);
}

uintm           ParserContext_getInstructionBits(ParserContext *pc, int startbit, int size, int off)
{
    off += (startbit / 8);
    if (off >= 16)
        vm_error("Instruction is using more than 16 bytes");

    uint1 *ptr = pc->buf + off;
    startbit = startbit % 8;
    int bytesize = (startbit + size - 1) / 8 + 1;
    uintm res = 0;
    int i;
    for (i = 0; i < bytesize; i++) {
        res <<= 8;
        res |= ptr[i];
    }

    res <<= 8 * (sizeof(uintm) - bytesize) + startbit;
    res >>= 8 * sizeof(uintm) - size;
    return res;
}

#define uintm_bits      (sizeof(uintm) * 8)

uintm           ParserContext_getContextBits(ParserContext *pc, int startbit, int size)
{
    int intstart = startbit / uintm_bits;
    uintm res = pc->context[intstart];
    int bitOffset = startbit % uintm_bits;
    int unusedBits = uintm_bits - size;
    res <<= bitOffset;
    res >>= unusedBits;
    int remaining = size - uintm_bits + bitOffset;
    if ((remaining > 0) && (++intstart < pc->contextsize)) {
        uintm res2 = pc->context[intstart];
        unusedBits = uintm_bits - remaining;
        res2 >>= unusedBits;
        res |= res2;
    }

    return res;
}

void            ParserContext_clearCommits(ParserContext *pc)
{
    int i;
    for (i = 0; i < pc->contextcommit.len; i++) {
        vm_free(pc->contextcommit.ptab[i]);
    }
    dynarray_reset(&pc->contextcommit);
}

void     ParserContext_deallocateState(ParserContext *pc, ParserWalker *walker) 
{
    pc->alloc = 1;
    walker->context = pc;
    ParserWalker_baseState(walker);
}

uintm       PaserContext_getInstructionBits(ParserContext *pc, int startbit, int size, int off)
{
    off += (startbit / 8);
    if (off >= 16)
        vm_error("Instruction is using more than 16 bytes");

    const uint1 *ptr = pc->buf + off;
    startbit = startbit % 8;
    int bytesize = (startbit + size - 1) / 8 + 1, i;
    uintm res = 0;
    for (i = 0; i < bytesize; i++) {
        res <<= 8;
        res |= ptr[i];
    }

    // 上面的res操作会导致传入了部分不需要的值，下面需要对这部分的值做trim
    res <<= 8 * (sizeof(uintm) - bytesize) + startbit;
    res >>= 8 * sizeof(uintm) - size;

    return res;
}

uintm           ParserContxt_getContextBytes(ParserContext *pc, int bytestart, int size)
{
    int intstart = bytestart / sizeof(uintm);
    uintm res = pc->context[intstart];
    int byteOffset = bytestart / sizeof(uintm);
    int unusedBytes = sizeof(uintm) - size;

    res <<= byteOffset * 8;
    res >>= unusedBytes * 8;

    int remaining = size - sizeof(uintm) + byteOffset;
    if ((remaining > 0) && (++intstart < pc->contextsize)) {
        uintm res2 = pc->context[intstart];
        unusedBytes = sizeof(uintm) - remaining;
        res2 >>= unusedBytes * 8;
        res |= res2;
    }

    return res;
}

ParserWalker*           ParserWalker_new(ParserContext *c)
{
    ParserWalker *pw = vm_mallocz(sizeof(pw[0]));

    pw->const_context = c;

    return pw;
}

ParserWalker*           ParserWalker_new2(ParserContext *c, ParserContext *cross)
{
    ParserWalker *pw = vm_mallocz(sizeof(ParserWalker));

    pw->const_context = c;
    pw->cross_context = cross;

    return pw;
}

void                    ParserWalker_delete(ParserWalker *p)
{
    vm_free(p);
}

void                    ParserWalker_baseState(ParserWalker *p)
{
    p->point = p->const_context->base_state;
    p->depth = 0;
    p->breadcrumb[0] = 0;
}

void                    ParserWalker_pushOperand(ParserWalker *p, int i)
{
    p->breadcrumb[p->depth++] = i + 1;
    p->point = p->point->resolve.ptab[i];
    p->breadcrumb[p->depth] = 0;
}

void                    ParserWalker_popOperand(ParserWalker *p)
{
    p->point = p->point->parent;
    p->depth -= 1;
}

void                    ParserWalker_setOutOfBandState(ParserWalker *p, Constructor *ct, int index, ConstructState *tempstate, ParserWalker *otherwalker)
{
    ConstructState *pt = otherwalker->point;
    int curdepth = otherwalker->depth;
    while (pt->ct != ct) {
        if (curdepth <= 0) return;
        curdepth -= 1;
        pt = pt->parent;

    }
    OperandSymbol *sym = ct->operands.ptab[index];
    int i = sym->operand.offsetbase;

    if (i < 0)
        tempstate->offset = pt->offset + sym->operand.reloffset;
    else
        tempstate->offset = ((ConstructState *)pt->resolve.ptab[index])->offset;

    tempstate->ct = ct;
    tempstate->length = pt->length;
    p->point = tempstate;
    p->depth = 0;
    p->breadcrumb[0] = 0;
}

uint4                   ParserWalker_getOffset(ParserWalker *walker, int i)
{
    if (i < 0)  return walker->point->offset;

    ConstructState *op = walker->point->resolve.ptab[i];

    return op->offset + op->length;
}

void                    ParserContext_allocateOperand(ParserContext *pc, int i, ParserWalker *walker)
{
    ConstructState *opstate = &pc->state[pc->alloc++];
    opstate->parent = walker->point;
    opstate->ct = NULL;
    walker->point->resolve.ptab[i] = opstate;
    walker->breadcrumb[walker->depth++] += 1;
    walker->point = opstate;
    walker->breadcrumb[walker->depth] = 0;
}

const Address*  ParserWalker_getAddr(ParserWalker *p)
{
    if (p->cross_context)
        return &p->cross_context->addr;

    return &p->const_context->addr;
}
const Address*  ParserWalker_getNaddr(ParserWalker *p)
{
    return p->cross_context ? &p->cross_context->naddr : &p->const_context->naddr;
}

const Address*  ParserWalker_getRefAddr(ParserWalker *p)
{
    return p->cross_context ? &p->cross_context->calladdr : &p->const_context->calladdr;
}

const Address*  ParserWalker_getDestAddr(ParserWalker *p)
{
    return p->cross_context ? &p->cross_context->calladdr : &p->const_context->calladdr;
}

void            ParserWalker_calcCurrentLength(ParserWalker *p, int length, int numopers)
{
    int i;
    length += p->point->offset;
    for (i = 0; i < numopers; i++) {
        ConstructState *subpoint = p->point->resolve.ptab[i];
        int sublength = subpoint->length + subpoint->offset;
        if (sublength > length)
            length = sublength;
    }

    p->point->length = length - p->point->offset;
}
