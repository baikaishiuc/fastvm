
#ifndef __slgh_compile_h_
#define __slgh_compile_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "vm.h"
#include "pcodecompile.h"
#include "slghsymbol.h"

typedef struct SpaceQuality SpaceQuality;
typedef struct RtlPair RtlPair;
typedef struct FieldQuality FieldQuality;
typedef struct SleighCompile SleighCompile;

struct RtlPair {
    //ConstructTpl *section;
    SymbolScope     *scope;
};


#define RAMTYPE             0
#define REGISTERTYPE        1

struct SpaceQuality {
    const char *name;
    int type;
    int size;
    int wordsize;
    bool isdefault;
};

SpaceQuality*   SpaceQuality_new(char *name);

struct FieldQuality {
    const char *name;
    unsigned int low, high;
    bool signext;
    bool flow;
    bool hex;
};

FieldQuality*   FieldQuality_new(const char *name, uintb *l, uintb *h);

typedef struct SectionVector {
    int4 nextindex;
    RtlPair main;
    struct dynarray named;  // RtlMain
} SectionVector;

SectionVector*  SectionVector_new(ConstructTpl *rtl, SymbolScope *scope);
void            SectionVector_delete(SectionVector *sv);

struct SleighCompile {
    int SLA_FORMAT_VERSION;

    struct {
        int counts;
    } preproc_defines;
    struct dynarray     contexttable;   // FieldContext
    struct dynarray     macrotable;     // ConstructTpl
    struct dynarray     tokentable;     // Token
    struct dynarray     tables;         // SubtableSymbol

    PcodeCompile *pcode;

    SubtableSymbol *root;
    SymbolTable symtab;
    uint32_t maxdelayslotbytes;
    uint32_t unique_allocatemask;
    uint32_t numSections;

    void(*saveXml)(FILE *out);
};

typedef enum SleighArchType {
    arch_arm,
    arch_x86
} SleighArchType;

typedef struct SleighCompileClass {
    void    (*addRegister)(SleighCompile *s, char *name, AddrSpace *base, uintb offset, int size);
    char*   (*getRegisterName)(SleighCompile *s, AddrSpace *base, uintb offset, int size);
    void    (*getAllRegisters)(SleighCompile *s, struct dynarray *d);
    void    (*getUserOpNames)(SleighCompile *s, struct dynarray *d);

    char name[1];
} SleighCompileClass;


SleighCompile*  SleighCompile_new(SleighArchType arch);
void            SleighCompile_delete();

SleighCompile*  SleighArch_register();

void            SleighCompile_resetConstructor(SleighCompile *s);
void            SleighCompile_setEndian(SleighCompile *s, int e);
void            SleighCompile_setAlignment(SleighCompile *s, int e);
TokenSymbol*    SleighCompile_defineToken(SleighCompile *s, const char *name, uint64_t *sz);
void            SleighCompile_addTokenField(SleighCompile *s, TokenSymbol *sym, FieldQuality *qual);
bool            SleighCompile_addContextField(SleighCompile *s, VarnodeSymbol *sym, FieldQuality *qual);
void            SleighCompile_newSpace(SleighCompile *s, SpaceQuality *quad);

void            SleighCompile_defineVarnodes(SleighCompile *s, SpaceSymbol *sym, uintb *off, uintb *size, struct dynarray *names);
void            SleighCompile_defineBitrange(SleighCompile *s, const char *name, VarnodeSymbol *sym, uint32_t bitoffset, uint32_t numb);
void            SleighCompile_defineOperand(SleighCompile *s, OperandSymbol *sym, PatternExpression *exp);

void            SleighCompile_addUserOp(SleighCompile *s, struct dynarray *names);
void            SleighCompile_attachValues(SleighCompile *s, struct dynarray *symlist, struct dynarray *numlist);
void            SleighCompile_attachNames(SleighCompile *s, struct dynarray *symlist, struct dynarray *names);
void            SleighCompile_attachVarnodes(SleighCompile *s, struct dynarray *symlist, struct dynarray *varlist);
void            SleighCompile_buildMacro(SleighCompile *s, MacroSymbol *sym, ConstructTpl *tpl);
void            SleighCompile_pushWith(SleighCompile *s, SubtableSymbol *sym, PatternEquation *pateq, struct dynarray *contvec);
void            SleighCompile_popWith(SleighCompile *s);
SubtableSymbol* SleighCompile_newTable(SleighCompile *s, const char *name);
MacroSymbol*    SleighCompile_createMacro(SleighCompile *s, const char *name, struct dynarray *param);

SectionVector*  SleighCompile_standaloneSection(SleighCompile *s, ConstructTpl *main);
SectionVector*  SleighCompile_firstNamedSection(SleighCompile *s, ConstructTpl *main, SectionSymbol *sym);
SectionVector*  SleighCompile_nextNamedSection(SleighCompile *s, SectionVector *vec, ConstructTpl *section, SectionSymbol *sym);
SectionVector*  SleighCompile_finalNamedSection(SleighCompile *s, SectionVector *vec, ConstructTpl *section);

void            SleighCompile_buildConstructor(SleighCompile *s, Constructor *big, PatternEquation *pateq, struct dynarray *contvec, SectionVector *vec);
void            SleighCompile_newOperand(SleighCompile *s, Constructor *c, const char *name);
Constructor*        SleighCompile_createConstructor(SleighCompile *s, SubtableSymbol *sym);
PatternEquation*    SleighCompile_constrainOperand(SleighCompile *s, OperandSymbol *sym, PatternExpression *patexp);
void                SleighCompile_selfDefine(SleighCompile *s, OperandSymbol *sym);
PatternEquation*    SleighCompile_defineInvisibleOperand(SleighCompile *s, TripleSymbol *sym);
bool                SleighCompile_contextMod(SleighCompile *s, struct dynarray *vec, ContextSymbol *sym, PatternExpression *pe);
void                SleighCompile_contextSet(SleighCompile *s, struct dynarray *vec, TripleSymbol *sym, ContextSymbol *ctxSym);
SectionSymbol*      SleighCompile_newSectionSymbol(SleighCompile *s, char *name);
void                SleighCompile_recordNop(SleighCompile *s);
void                SleighCompile_addRegister(SleighCompile *s, const char *name, AddrSpace *base, uintb offset, int size);
const char*         SleighCompile_getRegisterName(SleighCompile *s, AddrSpace *base, uintb off, int size);
void                SleighCompile_getAllRegister(SleighCompile *s, struct dynarray *d);
void                SleighCompile_getUserOpNames(SleighCompile *s, struct dynarray *d);

ConstructTpl*       SleighCompile_setResultVarnode(SleighCompile *s, ConstructTpl *ct, VarnodeTpl *vn);
ConstructTpl*       SleighCompile_setResultStarVarnode(SleighCompile *s, ConstructTpl *ct, StarQuality *star, VarnodeTpl *vn);

struct dynarray*    SleighCompile_createCrossBuild(SleighCompile *s, VarnodeTpl *addr, SectionSymbol *sym);
struct dynarray*    SleighCompile_createMacroUse(SleighCompile *s, MacroSymbol *sym, struct dynarray *param);

bool                SleighCompile_getPreprocValue(SleighCompile *s, CString *name, CString *value);
void                SleighCompile_parseFromNewFile(SleighCompile *s, const char *filename);
char*               SleighCompile_grabCurrentFilePath(SleighCompile *s);
SleighSymbol*       SleighSympile_findSymbol(SleighCompile *s, char *name);
void                SleighCompile_nextLine(SleighCompile *s);

#define  SleighCompile_isInRoot(s, ct)      (s->root == Construct_getParent(ct))


#ifdef __cplusplus
}
#endif

#endif