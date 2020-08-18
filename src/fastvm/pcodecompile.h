
#ifndef __pcodecompile_h__
#define __pcodecompile_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "slghsymbol.h"

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
    ConstTpl id;
    uint32_t size;
} ;

struct ExpTree {
    struct dynarray *ops;
    VarnodeTpl *outvn;
};

void                ExpTree_setOutput(ExpTree *e, VarnodeTpl *newout);
struct dynarray*    ExpTree_toVector(ExpTree *e);

typedef struct PcodeCompile {
    AddrSpace *defaultspace;
    AddrSpace *constantspace;
    AddrSpace *uniqspace;
    uint32_t local_labelcount;
    bool enforceLocalKey;
} PcodeCompile;

PcodeCompile*       PcodeCompile_new();
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

ExpTree*    PcodeCompile_createOp(PcodeCompile *p, OpCode opc, ExpTree *vn);
ExpTree*    PcodeCompile_createOp2(PcodeCompile *p,  OpCode opc, ExpTree *vn1, ExpTree *vn2);
ExpTree*    PcodeCompile_createOpOut(PcodeCompile *p, VarnodeTpl *outvn, OpCode opc, ExpTree *vn1, ExpTree *vn2);
ExpTree*    PcodeCompile_createOpOutUnary(PcodeCompile *p, VarnodeTpl *outvn, OpCode opc, ExpTree *vn1, ExpTree *vn2);

struct dynarray*    PcodeCompile_createOpNoOut(PcodeCompile *p, OpCode opc, ExpTree *vn);
struct dynarray*    PcodeCompile_createOpNoOut2(PcodeCompile *p, OpCode opc, ExpTree *vn1, ExpTree *vn2);
struct dynarray*    PcodeCompile_createOpConst(PcodeCompile *p, OpCode opc, uintb val);

ExpTree*            PcodeCompile_createLoad(PcodeCompile *p,StarQuality *qual, ExpTree *ptr);
struct dynarray*    PcodeCompile_createStore(PcodeCompile *p, StarQuality *qual, ExpTree *ptr, ExpTree *val);
ExpTree*            PcodeCompile_createUserOp(PcodeCompile *p, UserOpSymbol *sym, struct dynarray *param);

#ifdef __cplusplus
}
#endif

#endif