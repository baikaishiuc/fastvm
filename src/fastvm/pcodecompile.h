
#ifndef __pcodecompile_h__
#define __pcodecompile_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "slghsymbol.h"
#include "semantics.h"

typedef struct Location Location;
typedef struct StarQuality StarQuality;
typedef struct ExpTree ExpTree;
typedef struct PcodeCompile PcodeCompile;

struct Location {
    char *filename;
    int fileno;
};

Location*   Location_new();
Location*   Location_new2(const char *name, int line);
void        Location_delete(Location *l);

struct StarQuality {
    ConstTpl *id;
    uint32_t size;
} ;

StarQuality*        StarQuality_new();
void                StarQuality_delete(StarQuality *sq);

/* 在原始的ghidra代码中，outvn是值复制的，这里我改成了引用 */
struct ExpTree {
    struct dynarray *ops;
    VarnodeTpl *outvn;
};

ExpTree*            ExpTree_new();
ExpTree*            ExpTree_newV(VarnodeTpl *vn);
ExpTree*            ExpTree_newO(OpTpl *op);
void                ExpTree_delete(ExpTree *);

void                ExpTree_setOutput(ExpTree *e, VarnodeTpl *newout);
struct dynarray*    ExpTree_toVector(ExpTree *e);
ConstTpl*           ExpTree_getSize(ExpTree *e);
struct dynarray*    ExpTree_appendParams(OpTpl *op, struct dynarray *params);
#define ExpTree_getSize(e)      e->outvn->size

/* pcodecompil 模块是用来生成pcode的，而不是用来生成语法树的，所以虽然他里面有很多看起来语法树的代码
但是都是为了辅助生成pcode，一旦生成了pcode，就会删除ExpTree */
typedef struct PcodeCompile {
    AddrSpace *defaultspace;
    AddrSpace *constantspace;
    AddrSpace *uniqspace;
    uint32_t local_labelcount;
    bool enforceLocalKey;

    void *slgh;
    uintb (*allocateTemp)(void *slgh);
    void  (*addSymbol)(void *slgh, SleighSymbol *sym);
} PcodeCompile;

PcodeCompile*       PcodeCompile_new(void *slgh, 
                                    uintb (* allocateTemp)(void *slgh),
                                    void  (* addSymbol)(void *slgh, SleighSymbol *sym));
void                PcodeCompile_delete(PcodeCompile *n);

#define PcodeCompile_resetLabelCount(p)         (p)->local_labelcount = 0
#define PcodeCompile_setDefaultSpace(p, sp)     (p)->defaultspace = sp
#define PcodeCompile_setConstantSpace(p ,sp)    (p)->constantspace = sp
#define PcodeCompile_setUniqueSpace(p, sp)      (p)->uniqspace = sp
#define PcodeCompile_setEnforceLocalKey(p, k)   (p)->enforceLocalKey = val
#define PcodeCompile_getDefaultSpace(p)         (p)->defaultspace
#define PcodeCompile_getConstantSpace(p)        (p)->constantspace

struct dynarray*    PcodeCompile_placeLabel(PcodeCompile *p, LabelSymbol *sym);
struct dynarray*    PcodeCompile_newOutput(PcodeCompile *p, bool usesLocalKey, ExpTree *rhs, char *varname, uint32_t size);

//ExpTree*    PcodeCompile_createOp(PcodeCompile *p, OpCode opc, ExpTree *vn);
//ExpTree*    PcodeCompile_createOp2(PcodeCompile *p,  OpCode opc, ExpTree *vn1, ExpTree *vn2);

ExpTree*    PcodeCompile_createOpOut(PcodeCompile *p, VarnodeTpl *outvn, OpCode opc, ExpTree *vn);
ExpTree*    PcodeCompile_createOp2Out(PcodeCompile *p, VarnodeTpl *outvn, OpCode opc, ExpTree *vn1, ExpTree *vn2);
#define PcodeCompile_createOp(p, o, v)   PcodeCompile_createOpOut(p, NULL, o, v)
#define PcodeCompile_createOp2(p, o, v1, v2)   PcodeCompile_createOp2Out(p, NULL, o, v1, v2)

struct dynarray*    PcodeCompile_createOpNoOut(PcodeCompile *p, OpCode opc, ExpTree *vn);
struct dynarray*    PcodeCompile_createOpNoOut2(PcodeCompile *p, OpCode opc, ExpTree *vn1, ExpTree *vn2);
struct dynarray*    PcodeCompile_createOpConst(PcodeCompile *p, OpCode opc, uintb val);

ExpTree*            PcodeCompile_createLoad(PcodeCompile *p,StarQuality *qual, ExpTree *ptr);
struct dynarray*    PcodeCompile_createStore(PcodeCompile *p, StarQuality *qual, ExpTree *ptr, ExpTree *val);
ExpTree*            PcodeCompile_createUserOp(PcodeCompile *p, UserOpSymbol *sym, struct dynarray *param);
struct dynarray*    PcodeCompile_createUserOpNoOut(PcodeCompile *p, UserOpSymbol *sym, struct dynarray *param);
LabelSymbol*        PcodeCompile_defineLabel(PcodeCompile *p, char *name);
void                PcodeCompile_newLocalDefinition(PcodeCompile *p, char *name, uint32_t size);
struct dynarray*    PcodeCompile_assignBitRange(PcodeCompile *p, VarnodeTpl *vn, uint32_t bitoffset, uint32_t numbits, ExpTree *rhs);
ExpTree*            PcodeCompile_createBitRange(PcodeCompile *p, SpecificSymbol *sym, u4 bitoffset, u4 numbits);
ExpTree*            PcodeCompile_createVariadic(PcodeCompile *p, OpCode opc, struct dynarray *param);
VarnodeTpl*         PcodeCompile_addressOf(PcodeCompile *p, VarnodeTpl *var, u4 size);

#ifdef __cplusplus
}
#endif

#endif