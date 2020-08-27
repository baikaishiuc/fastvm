
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

void                ExpTree_setOutput(ExpTree *e, VarnodeTpl *newout)
{

}

struct dynarray*    ExpTree_toVector(ExpTree *e)
{
    return NULL;
}

PcodeCompile*       PcodeCompile_new()
{
    PcodeCompile *pcode = vm_mallocz(sizeof(pcode[0]));

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