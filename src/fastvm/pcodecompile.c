
#include "pcodecompile.h"

ExpTree*            ExpTree_new()
{
    return NULL;
}

ExpTree*            ExpTree_newV(VarnodeTpl *vn)
{
    return NULL;
}

ExpTree*            ExpTree_newO(OpTpl *op)
{
    return NULL;
}

PcodeCompile*       PcodeCompile_new()
{
    return NULL;
}

void                PcodeCompile_delete(PcodeCompile *n)
{
}

struct dynarray*    PcodeCompile_placeLabel(PcodeCompile *p, LabelSymbol *sym)
{
    return NULL;
}

struct dynarray*    PcodeCompile_newOutput(PcodeCompile *p, bool usesLocalKey, ExpTree *rhs, char *varname, uint32_t size)
{
    return NULL;
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