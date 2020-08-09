
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

SpaceQuality*   SpaceQuality_new(const char *name, uint8_t );

struct FieldQuality {
    const char *name;
    unsigned int low, high;
    bool signext;
    bool flow;
    bool hex;
};

FieldQuality*   FieldQuality_new(const char *name, uint8_t *l, uint8_t *h);

typedef struct SectionVector {
    int4 nextindex;
    RtlPair main;
    struct dynarray named;  // RtlMain
} SectionVector;

SectionVector*  SectionVector_new(ConstructTpl *rtl, SymbolScope *scope);
void            SectionVector_delete(SectionVector *sv);

struct SleighCompile {
    struct {
        int counts;
    } preproc_defines;
    struct dynarray     contexttable;   // FieldContext
    struct dynarray     macrotable;     // ConstructTpl
    struct dynarray     tokentable;     // Token
    struct dynarray     tables;         // SubtableSymbol

};

void            SleighCompile_resetConstructor(SleighCompile *s);
void            SleighCompile_setEndian(SleighCompile *s, int e);
void            SleighCompile_setAlignment(SleighCompile *s, int e);
TokenSymbol*    SleighCompile_defineToken(SleighCompile *s, const char *name, uint8_t *sz);
void            SleighCompile_addTokenField(SleighCompile *s, TokenSymbol *sym, FieldQuality *qual);
bool            SleighCompile_addContextField(SleighCompile *s, VarnodeSymbol *sym, FieldQuality *qual);
void            SleighCompile_newSpace(SleighCompile *s, SpaceQuality *quad);
void            SleighCompile_defineVarnodes(SleighCompile *s, SpaceSymbol *sym, uint8_t *off, uint8_t *size, struct dynarray *names);
void            SleighCompile_defineBitrange(SleighCompile *s, const char *name, VarnodeSymbol *sym, uint32_t bitoffset, uint32_t numb);
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
SectionVector*  SleighCompile_firstNamedSection(SleighCompile *s, SectionSymbol *sym);
void            SleighCompile_buildConstructor(SleighCompile *s, Constructor *big, PatternEquation *pateq, struct dynarray *contvec, SectionVector *vec);
void            SleighCompile_newOperand(SleighCompile *s, Constructor *c, const char *name);
Constructor*    SleighCompile_createConstruct(SleighCompile *s, SubtableSymbol *sym);


#define  SleighCompile_isInRoot(s, ct)      (s->root == Construct_getParent(ct))


#ifdef __cplusplus
}
#endif

#endif