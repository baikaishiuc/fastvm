
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

struct dynarray*    ExpTree_appendParams(OpTpl *op, struct dynarray *params)
{
    struct dynarray *res = dynarray_new(NULL, NULL);
    int i;
    ExpTree *param;

    for (i = 0; i < params->len; i++) {
        param = params->ptab[i];
        dynarray_insert(res, param->ops);
        dynarray_reset(param->ops);

        OpTpl_addInput(op, param->outvn);
        param->outvn = NULL;
    }
    dynarray_push(res, op);
    return res;
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
        VarnodeTpl_setSize(tmpvn, ConstTpl_new2(real, size));
    else if ((ExpTree_getSize(rhs)->type == real) && ExpTree_getSize(rhs)->value_real)
        VarnodeTpl_setSize(tmpvn, ExpTree_getSize(rhs));

    ExpTree_setOutput(rhs, tmpvn);

    sym = VarnodeSymbol_new(varname, ConstTpl_getSpace(tmpvn->space), ConstTpl_getReal(tmpvn->offset), (int)ConstTpl_getReal(tmpvn->size));
    p->addSymbol(p->slgh, sym);
    if (!usesLocalKey && p->enforceLocalKey)
        vm_error("%s:%d Must use local keyword to define symbol %s", sym->filename, sym->lineno, sym->name);

    return ExpTree_toVector(rhs);
}

ExpTree*    PcodeCompile_createOpOut(PcodeCompile *p, VarnodeTpl *outvn, OpCode opc, ExpTree *vn)
{
    if (!outvn)
        outvn = PcodeCompile_buildTemporary(p);

    OpTpl *op = OpTpl_new1(opc);
    OpTpl_addInput(op, vn->outvn);
    OpTpl_setOutput(op, outvn);
    dynarray_add(vn->ops, op);
    vn->outvn = VarnodeTpl_clone(outvn);
    return vn;
}

ExpTree*    PcodeCompile_createOp2Out(PcodeCompile *p, VarnodeTpl *outvn, OpCode opc, ExpTree *vn1, ExpTree *vn2)
{
    if (!outvn)
        outvn = PcodeCompile_buildTemporary(p);

    OpTpl *op = OpTpl_new(opc);
    dynarray_insert(vn1->ops, vn2->ops);
    dynarray_reset(vn2->ops);

    OpTpl_addInput(op, vn1->outvn);
    OpTpl_addInput(op, vn2->outvn);
    OpTpl_setOutput(op, outvn);
    vn2->outvn = NULL;

    dynarray_add(vn1->ops, op);
    ExpTree_delete(vn2);

    return vn1;
}

struct dynarray*    PcodeCompile_createOpNoOut(PcodeCompile *p, OpCode opc, ExpTree *vn)
{
    OpTpl *op = OpTpl_new(opc);
    OpTpl_addInput(op, vn->outvn);
    vn->outvn = NULL;

    struct dynarray *res = vn->ops;
    dynarray_reset(vn->ops);
    ExpTree_delete(vn);

    dynarray_push(res, op);
    return res;
}

struct dynarray*    PcodeCompile_createOpNoOut2(PcodeCompile *p, OpCode opc, ExpTree *vn1, ExpTree *vn2)
{
    OpTpl *op = OpTpl_new(opc);
    struct dynarray *res = vn1->ops;

    dynarray_insert(vn1->ops, vn2->ops);

    OpTpl_addInput(op, vn1->outvn);
    OpTpl_addInput(op, vn2->outvn);
    vn2->outvn = NULL;

    dynarray_reset(vn1->ops);
    dynarray_reset(vn2->ops);
    ExpTree_delete(vn1);
    ExpTree_delete(vn2);

    dynarray_push(res, op);
    return res;
}

struct dynarray*    PcodeCompile_createOpConst(PcodeCompile *p, OpCode opc, uintb val)
{
    VarnodeTpl *vn = VarnodeTpl_new3(ConstTpl_newA(p->constantspace),
                                    ConstTpl_new2(real, val),
                                    ConstTpl_new2(real, 4));

    struct dynarray *res = dynarray_new(NULL, NULL);

    OpTpl *op = OpTpl_new(opc);
    OpTpl_addInput(op, vn);
    dynarray_add(res, op);

    return res;
}

void                PcodeCompile_forceSize(VarnodeTpl *vt, ConstTpl *size, struct dynarray *ops)
{
    if (vt->size && ((vt->size->type != real) || vt->size->value_real))
        return;

    VarnodeTpl_setSize(vt, size);
    if (!VarnodeTpl_isLocalTemp(vt)) return;

    // If the variable is a local temporary
    // The size may need to be propagated to the various
    // uses of the variable

    OpTpl *op;
    VarnodeTpl *vn;

    int i, j;

    for (i = 0; i < ops->len; i++) {
        op = ops->ptab[i];
        vn = OpTpl_getOut(op);
        if (vn && VarnodeTpl_isLocalTemp(vn) && ConstTpl_isEqual(vn->offset, vt->offset)) {
            /* 看不懂，这段代码来自于 Ghidra，既然vn->size和size的值不等要报错，那么相等为什么需要重新赋值呢？
            可能是因为这个size的结构体被多人引用以后，可以统一修改
            */
            if (!ConstTpl_isEqual(size, vn->size))
                vm_error("Localtemp size mismatch");

            VarnodeTpl_setSize(vn, size);
        }
        for (j = 0; j < op->input.len; j++) {
            vn = op->input.ptab[j];
            if (VarnodeTpl_isLocalTemp(vn) && ConstTpl_isEqual(vn->offset, vt->offset)) {
                if (ConstTpl_isEqual(size, vn->size))
                    vm_error("Localtemp size mismatch");
                VarnodeTpl_setSize(vn, size);
            }
        }
    }
}

// :load4 r1,[r2] is opcode=0x9c & r1 & r2 { r1 = *[other]:2 r2; }
ExpTree*            PcodeCompile_createLoad(PcodeCompile *p, StarQuality *qual, ExpTree *ptr)
{
    VarnodeTpl *outvn = PcodeCompile_buildTemporary(p);
    OpTpl *op = OpTpl_new(CPUI_LOAD);

    VarnodeTpl *spcvn = VarnodeTpl_new3(ConstTpl_newA(p->constantspace), qual->id, ConstTpl_new2(real, 8));

    OpTpl_addInput(op, spcvn);
    OpTpl_addInput(op, ptr->outvn);
    OpTpl_setOutput(op, outvn);
    dynarray_add(ptr->ops, op);
    if (qual->size)
        PcodeCompile_forceSize(outvn, ConstTpl_new2(real, qual->size), ptr->ops);
    /* FIXME:Ghidra这里是一个复制命令 */
    ptr->outvn = outvn;
    return ptr;
}

struct dynarray*    PcodeCompile_createStore(PcodeCompile *p, StarQuality *qual, ExpTree *ptr, ExpTree *val)
{
    struct dynarray *res = ptr->ops;
    ptr->ops = NULL;
    dynarray_add(res, val->ops);
    dynarray_reset(val->ops);

    OpTpl *op = OpTpl_new(CPUI_STORE);
    VarnodeTpl *spcvn = VarnodeTpl_new(ConstTpl_newA(p->constantspace), qual->id, ConstTpl_new2(real, 8));

    /* 这里没有设置out，感觉有点问题，理论上对于如下的表达式:

    store r0, [r1]
    
    应该生成类似于如下的pcode

    temp t = new temp(r1);
    t = r0
    */
    OpTpl_addInput(op, spcvn);
    OpTpl_addInput(op, ptr->outvn);
    OpTpl_addInput(op, val->outvn);
    dynarray_add(res, op);
    PcodeCompile_forceSize(val->outvn, ConstTpl_new2(real, qual->size), res);
    ptr->outvn = NULL;
    val->outvn = NULL;
    ExpTree_delete(ptr);
    ExpTree_delete(val);
    return res;
}

ExpTree*            PcodeCompile_createUserOp(PcodeCompile *p, UserOpSymbol *sym, struct dynarray *param)
{
    return NULL;
}

struct dynarray*    PcodeCompile_createUserOpNoOut(PcodeCompile *p, UserOpSymbol *sym, struct dynarray *params)
{
    OpTpl *op = OpTpl_new1(CPUI_CALLOTHER);
    VarnodeTpl *vn = VarnodeTpl_new(ConstTpl_newA(p->constantspace), 
                                    ConstTpl_new2(real, sym->index),
                                    ConstTpl_new2(real, 4));
    OpTpl_addInput(op, vn);
    return ExpTree_appendParams(op, params);
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