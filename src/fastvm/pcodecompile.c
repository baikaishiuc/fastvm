
#include "vm.h"
#include "pcodecompile.h"

StarQuality*        StarQuality_new()
{
    StarQuality *sq = vm_mallocz(sizeof(sq[0]));

    return sq;
}

void                StarQuality_delete(StarQuality *sq)
{
    vm_free(sq);
}

ExpTree*            ExpTree_new()
{
    ExpTree *e = vm_mallocz(sizeof(e[0]));

    e->ops = dynarray_new(NULL, NULL);

    return e;
}

ExpTree*            ExpTree_newV(VarnodeTpl *vn)
{
    ExpTree *e = vm_mallocz(sizeof(e[0]));

    e->outvn = vn;
    e->ops = dynarray_new(NULL, NULL);

    return e;
}

ExpTree*            ExpTree_newO(OpTpl *op)
{
    ExpTree *e = vm_mallocz(sizeof(e[0]));

    e->ops = dynarray_new(NULL, NULL);

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

    struct dynarray *res = vn->ops;

    vn->outvn = NULL;
    vn->ops = NULL;
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

    vn1->outvn = NULL;
    vn1->ops = NULL;
    vn2->outvn = NULL;
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
    assert(NULL);
    return NULL;
}

VarnodeTpl*         PcodeCompile_buildTruncatedVarnode(PcodeCompile *p, VarnodeTpl *basevn, int bitoffset, int numbits)
{
    int byteoffset = bitoffset / 8;
    int numbytes = numbits / 8;
    intb fullsz = 0;

    if (basevn->size->type == real) {
        fullsz = basevn->size->value_real;
        if (!fullsz)
            return NULL;

        if ((byteoffset + numbytes) > fullsz)
            vm_error("Requested bit range out of bounds");
    }

    if (bitoffset % 8)
        return NULL;
    if (numbits % 8)
        return NULL;

    if (ConstTpl_isUniqueSpace(basevn->space))
        return NULL;

    int offset_type = basevn->offset->type;
    if ((offset_type != real) && (offset_type != handle))
        return 0;

    ConstTpl *off;
    if (offset_type == handle) {
        off = ConstTpl_new4(handle, basevn->offset->value.handle_index, v_offset_plus, byteoffset);
    }
    else {
        if (basevn->size->type != real)
            vm_error("Could not construct request bit range");

        intb plus;
        if (AddrSpace_isBigEndian(p->defaultspace))
            plus = fullsz - (byteoffset + numbytes);
        else
            plus = byteoffset;

        off = ConstTpl_new2(real, basevn->offset->value_real + plus);
    }

    VarnodeTpl *res = VarnodeTpl_new(basevn->space, off, ConstTpl_new2(real, numbytes));
    return res;
}

ExpTree*            PcodeCompile_createBitRange(PcodeCompile *p, SpecificSymbol *sym, int bitoffset, int numbits)
{
    VarnodeTpl *vn = SleighSymbol_getVarnode(sym);

    if (!numbits)
        vm_error("Size of bitrange is zero");

    int finalsize = (numbits + 7) / 8;
    int truncshift = 0;
    bool maskneeded = (numbits + 7) / 8;
    bool truncneeded = true;

    if (!bitoffset && !maskneeded) {
        if ((vn->size->type == handle) && VarnodeTpl_isZeroSize(vn)) {
            VarnodeTpl_setSize(vn, ConstTpl_new2(real, finalsize));
            return ExpTree_new(vn);
        }
    }

    VarnodeTpl *truncvn = PcodeCompile_buildTruncatedVarnode(p, vn, bitoffset, numbits);
    if (truncvn) 
        return ExpTree_new(truncvn);

    if (vn->size->type == real) {
        int insize = (int)vn->size->value_real;
        if (insize > 0) {
            truncneeded = (finalsize < insize);
            insize *= 8;
            if (bitoffset >= insize || (bitoffset + numbits) > insize)
                vm_error("Bitrange is bad");
            if (maskneeded && ((bitoffset + numbits) == insize))
                maskneeded = false;
        }
    }

    uintb mask = 2;
    mask = (mask << (numbits - 1)) - 1;

    if (truncneeded && ((bitoffset % 8) == 0)) {
        truncshift = bitoffset / 8;
        bitoffset = 0;
    }

    if ((bitoffset == 0) && !truncneeded && !maskneeded)
        vm_error("Superfluous bitrang");

    if (maskneeded && (finalsize > 8))
        vm_error("Illegal masked bitrange producing varnode larger than 64bits: %s", sym->name);

    ExpTree *res = ExpTree_new(vn);

    if (bitoffset)
        PcodeCompile_appendOp(p, CPUI_INT_RIGHT, res, bitoffset, 4);
    if (truncneeded)
        PcodeCompile_appendOp(p, CPUI_SUBPIECE, res, truncshift, 4);
    if (maskneeded)
        PcodeCompile_appendOp(p, CPUI_INT_AND, res, mask, finalsize);
    PcodeCompile_forceSize(res->outvn, ConstTpl_new2(real, finalsize), res->ops);
    return res;
}

ExpTree*            PcodeCompile_createVariadic(PcodeCompile *p, OpCode opc, struct dynarray *param)
{
    return NULL;
}

VarnodeTpl*         PcodeCompile_addressOf(PcodeCompile *p, VarnodeTpl *var, u4 size)
{
    return NULL;
}

/* 
@j      <0      output 
        >=0     input index
*/
void                PcodeCompile_matchSize(PcodeCompile *p, int j, OpTpl *op, bool inputonly, struct dynarray *ops)
{
    // 当op的某个input或output size为0时，从这个op的其他input和Output中找到
    // 一个非0的进行填充 
    VarnodeTpl *match = 0;
    VarnodeTpl *vt, *in;
    int i;

    vt = (j == -1) ? op->output : op->input.ptab[j];
    if (!inputonly) {
        if (op->output && !VarnodeTpl_isZeroSize(op->output))
            match = op->output;
    }
    for (i = 0; i < op->input.len; i++) {
        if (match)
            break;
        in = op->input.ptab[i];
        if (VarnodeTpl_isZeroSize(in))
            continue;
        match = in;
    }

    if (match)
        PcodeCompile_forceSize(vt, match->size, ops);
}

void                PcodeCompile_filleinZero(PcodeCompile *p, OpTpl *op, struct dynarray *ops)
{
    VarnodeTpl *in, *v;
    int i, inputsize;
    /* */
    switch (op->opc) {
        case CPUI_COPY: // 这部分的指令input和output都是一样size的
        case CPUI_INT_ADD:
        case CPUI_INT_SUB:
        case CPUI_INT_2COMP:
        case CPUI_INT_NEGATE:
        case CPUI_INT_XOR:
        case CPUI_INT_AND:
        case CPUI_INT_OR:
        case CPUI_INT_MULT:
        case CPUI_INT_DIV:
        case CPUI_INT_SDIV:
        case CPUI_INT_REM:
        case CPUI_INT_SREM:
        case CPUI_FLOAT_ADD:
        case CPUI_FLOAT_DIV:
        case CPUI_FLOAT_MULT:
        case CPUI_FLOAT_SUB:
        case CPUI_FLOAT_NEG:
        case CPUI_FLOAT_ABS:
        case CPUI_FLOAT_SQRT:
        case CPUI_FLOAT_CEIL:
        case CPUI_FLOAT_FLOOR:
        case CPUI_FLOAT_ROUND:
            if (op->output && VarnodeTpl_isZeroSize(op->output))
                PcodeCompile_matchSize(p, -1, op, false, ops);
            inputsize = op->input.len;
            for (i = 0; i < inputsize; i++) {
                in = op->input.ptab[i];
                if (VarnodeTpl_isZeroSize(in))
                    PcodeCompile_matchSize(p, i, op, false, ops);
            }
            break;

        case CPUI_INT_EQUAL: // Instructions with bool output
        case CPUI_INT_NOTEQUAL:
        case CPUI_INT_SLESS:
        case CPUI_INT_SLESSEQUAL:
        case CPUI_INT_LESS:
        case CPUI_INT_LESSEQUAL:
        case CPUI_INT_CARRY:
        case CPUI_INT_SCARRY:
        case CPUI_INT_SBORROW:
        case CPUI_FLOAT_EQUAL:
        case CPUI_FLOAT_NOTEQUAL:
        case CPUI_FLOAT_LESS:
        case CPUI_FLOAT_LESSEQUAL:
        case CPUI_FLOAT_NAN:
        case CPUI_BOOL_NEGATE:
        case CPUI_BOOL_XOR:
        case CPUI_BOOL_AND:
        case CPUI_BOOL_OR:
            if (VarnodeTpl_isZeroSize(op->output))
                PcodeCompile_forceSize(op->output, ConstTpl_new2(real, 1), ops);
            inputsize = op->input.len;
            for (i = 0; i < inputsize; i++) {
                in = op->input.ptab[i];
                /* 这里我没看懂，为什么inputonly是true?*/
                if (VarnodeTpl_isZeroSize(in))
                    PcodeCompile_matchSize(p, i, op, true, ops);
            }
            break;

            // The shift amount does not necessarily have to be the same size
            // But if no size is specified, assume it is the same size
        case CPUI_INT_LEFT:
        case CPUI_INT_RIGHT:
        case CPUI_INT_SRIGHT:
            in = op->input.ptab[0];
            if (VarnodeTpl_isZeroSize(op->output)) {
                if (!VarnodeTpl_isZeroSize(in))
                    PcodeCompile_forceSize(op->output, in->size, ops);
            }
            else if (VarnodeTpl_isZeroSize(in)) {
                PcodeCompile_forceSize(in, op->output->size, ops);
            }
            // fallthru to subpiece constant check

        case CPUI_SUBPIECE:
            in = op->input.ptab[1];
            if (VarnodeTpl_isZeroSize(in))
                PcodeCompile_forceSize(in, ConstTpl_new2(real, 4), ops);
            break;
        case CPUI_CPOOLREF:
            in = op->input.ptab[0];
            if (VarnodeTpl_isZeroSize(op->output) && !VarnodeTpl_isZeroSize(in))
                PcodeCompile_forceSize(op->output, in->size, ops);
            if (VarnodeTpl_isZeroSize(in) && !VarnodeTpl_isZeroSize(op->output))
                PcodeCompile_forceSize(in, op->output->size, ops);

            for (i = 1; i < op->input.len; i++) {
                v = op->input.ptab[i];
                if (VarnodeTpl_isZeroSize(v))
                    PcodeCompile_forceSize(v, ConstTpl_new2(real, sizeof(uintb)), ops);
            }
        default:
            break;
    }
}

bool                PcodeCompile_propagateSize(PcodeCompile *p, ConstructTpl *ct)
{
    struct dynarray zerovec = { 0 }, zerovec2 = { 0 };
    int i, lastsize;
    OpTpl *op;

    for (i = 0; i < ct->vec.len; i++) {
        op = ct->vec.ptab[i];
        if (OpTpl_isZeroSize(op)) {
            PcodeCompile_filleinZero(p, op, &ct->vec);
            if (OpTpl_isZeroSize(op))
                dynarray_add(&zerovec, op);
        }
    }
    lastsize = zerovec.len + 1;
    while (zerovec.len < lastsize) {
        lastsize = zerovec.len;
        dynarray_reset(&zerovec2);
        for (i = 0; i < zerovec.len; i++) {
            op = zerovec.ptab[i];
            PcodeCompile_filleinZero(p, op, &ct->vec);
            if (OpTpl_isZeroSize(op))
                dynarray_add(&zerovec2, op);
        }
        dynarray_reset(&zerovec);
        dynarray_insert(&zerovec, &zerovec2);
    }

    dynarray_reset(&zerovec);
    dynarray_reset(&zerovec2);
    if (lastsize)
        return false;
    return true;
}

void                PcodeCompile_appendOp(PcodeCompile *p, OpCode opc, ExpTree *res, uintb constval, int constsz)
{
    OpTpl *op = OpTpl_new(opc);
    VarnodeTpl *vn = VarnodeTpl_new3(ConstTpl_newA(p->constantspace),
                                    ConstTpl_new2(real, constval),
                                    ConstTpl_new2(real, constsz));
    VarnodeTpl *outvn = PcodeCompile_buildTemporary(p);
    OpTpl_addInput(op, res->outvn);
    OpTpl_addInput(op, vn);
    OpTpl_setOutput(op, outvn);
    dynarray_add(res->ops, op);
    res->outvn = outvn;
}