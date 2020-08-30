
#include "vm.h"
#include "pcodecompile.h"

StarQuality*        StarQuality_new()
{
    return NULL;
}

void                StarQuality_delete(StarQuality *sq)
{
}

ExpTree*            ExpTree_new()
{
    ExpTree *e = vm_mallocz(sizeof(e[0]));

    e->ops = dynarray_new(NULL, OpTpl_delete);

    return e;
}

ExpTree*            ExpTree_newV(VarnodeTpl *vn)
{
    ExpTree *e = vm_mallocz(sizeof(e[0]));

    e->outvn = vn;
    e->ops = dynarray_new(NULL, OpTpl_delete);

    return e;
}

ExpTree*            ExpTree_newO(OpTpl *op)
{
    ExpTree *e = vm_mallocz(sizeof(e[0]));

    e->ops = dynarray_new(NULL, OpTpl_delete);

    dynarray_add(e->ops, op);
    if (op->output)
        e->outvn = op->output;

    return e;
}

void                ExpTree_delete(ExpTree *e)
{
    if (e->ops) {
        dynarray_delete(e->ops);
    }

    vm_free(e);
}

void                ExpTree_setOutput(ExpTree *e, VarnodeTpl *newout)
{
    OpTpl *op;

    if (!e->outvn)
        vm_error("Express has no output");

    if (e->outvn->unnamed_flag) {
        //VarnodeTpl_delete(e->outvn);

        op = dynarray_back(e->ops);

        OpTpl_clearOutput(op);
        OpTpl_setOutput(op, newout);
    }
    else  {
        op = OpTpl_new1(CPUI_COPY);
        OpTpl_addInput(op, e->outvn);
        OpTpl_setOutput(op, newout);
        dynarray_add(e->ops, op);
    }

    e->outvn = newout;
}

struct dynarray*    ExpTree_toVector(ExpTree *e)
{
    struct dynarray *res = e->ops;
    e->ops = NULL;
    ExpTree_delete(e);

    return res;
}

PcodeCompile*       PcodeCompile_new(void *slgh, 
                                    uintb (* allocateTemp)(void *slgh),
                                    void  (* addSymbol)(void *slgh, SleighSymbol *sym))
{
    PcodeCompile *pcode = vm_mallocz(sizeof(pcode[0]));

    pcode->slgh = slgh;
    pcode->allocateTemp = allocateTemp;
    pcode->addSymbol = addSymbol;

    return pcode;
}

void                PcodeCompile_delete(PcodeCompile *pcode)
{
    vm_free(pcode);
}

struct dynarray*    PcodeCompile_placeLabel(PcodeCompile *p, LabelSymbol *sym)
{
    return NULL;
}

VarnodeTpl*         PcodeCompile_buildTemporary(PcodeCompile *p)
{
    VarnodeTpl *res = VarnodeTpl_new3(ConstTpl_newA(p->constantspace), 
                                      ConstTpl_new2(real, p->allocateTemp(p->slgh)),
                                      ConstTpl_new2(real, 0));

    res->unnamed_flag = true;

    return res;
}

struct dynarray*    PcodeCompile_newOutput(PcodeCompile *p, bool usesLocalKey, ExpTree *rhs, char *varname, uint32_t size)
{
    VarnodeSymbol *sym;
    VarnodeTpl *tmpvn = PcodeCompile_buildTemporary(p);
    if (size)
        VarnodeTpl_setSize(tmpvn, size);
    else if ((ExpTree_getSize(rhs)->type == real) && ExpTree_getSize(rhs)->value_real)
        VarnodeTpl_setSize(tmpvn, ExpTree_getSize(rhs)->value_real);

    ExpTree_setOutput(rhs, tmpvn);

    sym = VarnodeSymbol_new(varname, ConstTpl_getSpace(tmpvn->space), ConstTpl_getReal(tmpvn->offset), (int)ConstTpl_getReal(tmpvn->size));
    p->addSymbol(p->slgh, sym);
    if (!usesLocalKey && p->enforceLocalKey)
        vm_error("%s:%d Must use local keyword to define symbol %s", sym->filename, sym->lineno, sym->name);

    return ExpTree_toVector(rhs);
}

ExpTree*    PcodeCompile_createOp(PcodeCompile *p, OpCode opc, ExpTree *vn)
{
    return NULL;
}

ExpTree*    PcodeCompile_createOp2(PcodeCompile *p, OpCode opc, ExpTree *vn1, ExpTree *vn2)
{
    return NULL;
}

ExpTree*    PcodeCompile_createOpOut(PcodeCompile *p, VarnodeTpl *outvn, OpCode opc, ExpTree *vn1, ExpTree *vn2)
{
    return NULL;
}

ExpTree*    PcodeCompile_createOpOutUnary(PcodeCompile *p, VarnodeTpl *outvn, OpCode opc, ExpTree *vn1, ExpTree *vn2)
{
    return NULL;
}

struct dynarray*    PcodeCompile_createOpNoOut(PcodeCompile *p, OpCode opc, ExpTree *vn)
{
    return NULL;
}

struct dynarray*    PcodeCompile_createOpNoOut2(PcodeCompile *p, OpCode opc, ExpTree *vn1, ExpTree *vn2)
{
    return NULL;
}

struct dynarray*    PcodeCompile_createOpNoOut4(PcodeCompile *p, OpCode opc, ExpTree *vn1, ExpTree *vn2)
{
    return NULL;
}

struct dynarray*    PcodeCompile_createOpConst(PcodeCompile *p, OpCode opc, uintb val)
{
    return NULL;
}

ExpTree*            PcodeCompile_createLoad(PcodeCompile *p, StarQuality *qual, ExpTree *ptr)
{
    return NULL;
}

struct dynarray*    PcodeCompile_createStore(PcodeCompile *p, StarQuality *qual, ExpTree *ptr, ExpTree *val)
{
    return NULL;
}

ExpTree*            PcodeCompile_createUserOp(PcodeCompile *p, UserOpSymbol *sym, struct dynarray *param)
{
    return NULL;
}

struct dynarray*    PcodeCompile_createUserOpNoOut(PcodeCompile *p, UserOpSymbol *sym, struct dynarray *param)
{
    return NULL;
}

LabelSymbol*        PcodeCompile_defineLabel(PcodeCompile *p, char *name)
{
    return NULL;
}

void                PcodeCompile_newLocalDefinition(PcodeCompile *p, char *name, uint32_t size)
{
}

struct dynarray*    PcodeCompile_assignBitRange(PcodeCompile *p, VarnodeTpl *vn, uint32_t bitoffset, uint32_t numbits, ExpTree *rhs)
{
    return NULL;
}

ExpTree*            PcodeCompile_createBitRange(PcodeCompile *p, SpecificSymbol *sym, u4 bitoffset, u4 numbits)
{
    return NULL;
}

ExpTree*            PcodeCompile_createVariadic(PcodeCompile *p, OpCode opc, struct dynarray *param)
{
    return NULL;
}

VarnodeTpl*         PcodeCompile_addressOf(PcodeCompile *p, VarnodeTpl *var, u4 size)
{
    return NULL;
}