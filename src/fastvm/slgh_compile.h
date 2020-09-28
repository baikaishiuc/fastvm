
#ifndef __slgh_compile_h_
#define __slgh_compile_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "pcodecompile.h"
#include "slghsymbol.h"

typedef struct SpaceQuality SpaceQuality;
typedef struct RtlPair RtlPair;
typedef struct FieldQuality FieldQuality;
typedef struct SleighCompile SleighCompile, AddrSpaceManager, Translator;

struct RtlPair {
    ConstructTpl *section;
    SymbolScope     *scope;
};

RtlPair*        RtlPair_newV();
RtlPair*        RtlPair_new(ConstructTpl *ct, SymbolScope *scope);
void            RtlPair_delete(RtlPair *rp);

#define RAMTYPE             0
#define REGISTERTYPE        1

struct SpaceQuality {
    int type;
    int size;
    int wordsize;
    bool isdefault;
    char name[1];
};

SpaceQuality*   SpaceQuality_new(char *name);
void            SpaceQuality_delete(SpaceQuality *sp);

struct FieldQuality {
    int low, high;
    bool signext;
    bool flow;
    bool hex;
    char name[1];
};

FieldQuality*   FieldQuality_new(const char *name, uintb l, uintb h);
void            FieldQuality_delete(FieldQuality *f);

typedef struct FieldContext     FieldContext;

struct FieldContext {
    VarnodeSymbol *sym;
    FieldQuality *qual;
};

FieldContext*           FieldContext_new(VarnodeSymbol *v, FieldQuality *qual);
void                    FieldContext_delete(FieldContext *fc);
int                     FieldContext_cmp(void const *a, void const *b);

typedef struct SectionVector {
    int4 nextindex;
    RtlPair main;
    struct dynarray named;  // RtlMain
} SectionVector;

SectionVector*  SectionVector_new(ConstructTpl *rtl, SymbolScope *scope);
void            SectionVector_delete(SectionVector *sv);
void            SectionVector_append(SectionVector *sv, ConstructTpl *rtl, SymbolScope *scope);

struct slgh_macro;
struct slgh_macro {
    struct {
        struct slgh_macro *next;
        struct slgh_macro *prev;
    } in_list;

    char *filename;
    int lineno;

    char *value;
    char name[1];
};

typedef struct WithBlock        WithBlock;
typedef struct MacroBuilder     MacroBuilder;

struct WithBlock {
    SubtableSymbol *ss;
    PatternEquation *pateq;
    struct dynarray contvec;
};

WithBlock*          WithBlock_new();
void                WithBlock_delete(WithBlock *w);
PatternEquation*    WithBlock_collectAndPrependPattern(struct dynarray *stack, PatternEquation *pateq);
struct dynarray*    WithBlock_collectAndPrependContext(struct dynarray *stack, struct dynarray *contvec);

struct MacroBuilder {
    SleighCompile *slgh;
    PcodeBuilder *pb;
    bool haserror;
    struct dynarray *outvec;
    struct dynarray params;
};

MacroBuilder*       MacroBuilder_new(SleighCompile *s, struct dynarray *ovec, int labelcount);
void                MacroBuilder_delete(MacroBuilder *m);
void                MacroBuilder_setMacroOp(MacroBuilder *m, OpTpl *op);
void                MacroBuilder_dump(MacroBuilder *m, OpTpl *o);


struct slgh_preproc
{
    CString fullpath;
    int lineno;
};

struct SleighCompile {
    int SLA_FORMAT_VERSION;

    bool    bigendian;
    uintm   unique_base;
    int     alignment;
    struct dynarray     floatformats;

    struct {
        int counts;
        struct slgh_macro *list;
    } defines;

    struct dynarray     baselist;
    struct dynarray     resolvelist;

    struct dynarray     name2Space;

    struct {
        int counts;
    } shortcut2Space;

    AddrSpace   *constantspace;
    AddrSpace   *defaultcodespace;
    AddrSpace   *defaultdataspace;
    AddrSpace   *iopspace;
    AddrSpace   *fspecspace;
    AddrSpace   *joinspace;
    AddrSpace   *stackspace;
    AddrSpace   *uniqspace;
    u8          joinallocate;

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
    int userop_count;

    bool warnunnecessarypcode;
    bool warndeadtemps;
    bool lenientconfliciterros;
    bool warnallocalcollisions;
    bool warnallnops;
    struct dynarray     noplist;

    int errors;
    struct dynarray     preproc;
    struct dynarray     withstack;
    Constructor*        curct;
    MacroSymbol*        curmacro;
    bool                contextlock;

    void(*saveXml)(SleighCompile *s, FILE *out);

    struct {
        bool     parse;
    } debug;
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

int             SleighCompile_new(SleighCompile *s, int debug);
void            SleighCompile_delete(SleighCompile *s);

SleighCompile*  SleighArch_register();

void            SleighCompile_resetConstructor(SleighCompile *s);
void            SleighCompile_setEndian(SleighCompile *s, int e);
void            SleighCompile_setAlignment(SleighCompile *s, int e);
TokenSymbol*    SleighCompile_defineToken(SleighCompile *s, const char *name, intb sz);
void            SleighCompile_addTokenField(SleighCompile *s, TokenSymbol *sym, FieldQuality *qual);
bool            SleighCompile_addContextField(SleighCompile *s, VarnodeSymbol *sym, FieldQuality *qual);
void            SleighCompile_newSpace(SleighCompile *s, SpaceQuality *quad);

void            SleighCompile_defineVarnodes(SleighCompile *s, SpaceSymbol *sym, uintb off, uintb size, struct dynarray *names);
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
uintb               SleighCompile_getUniqueAddr(SleighCompile *s);
void            SleighCompile_addSymbol(SleighCompile *s, SleighSymbol *sym);

ConstructTpl*       SleighCompile_setResultVarnode(SleighCompile *s, ConstructTpl *ct, VarnodeTpl *vn);
ConstructTpl*       SleighCompile_setResultStarVarnode(SleighCompile *s, ConstructTpl *ct, StarQuality *star, VarnodeTpl *vn);

struct dynarray*    SleighCompile_createCrossBuild(SleighCompile *s, VarnodeTpl *addr, SectionSymbol *sym);
struct dynarray*    SleighCompile_createMacroUse(SleighCompile *s, MacroSymbol *sym, struct dynarray *param);

bool                slgh_get_macro(SleighCompile *s, char *name, char **value);
void                slgh_set_macro(SleighCompile *s, char *name, char *value);
bool                slgh_del_macro(SleighCompile *s, char *name);
#define slgh_define_macro       slgh_set_macro      
#define slgh_undefine_macro     slgh_del_macro      

void                SleighCompile_parseFromNewFile(SleighCompile *s, const char *filename);
void                SleighCompile_parsePreprocMacro(SleighCompile *s);
void                SleighCompile_parseFileFinished(SleighCompile *s);

char*               SleighCompile_grabCurrentFilePath(SleighCompile *s);
SleighSymbol*       SleighCompile_findSymbol(SleighCompile *s, char *name);
void                SleighCompile_nextLine(SleighCompile *s);
void                SleighCompile_calcContextLayout(SleighCompile *s);
void                SleighCompile_checkSymbols(SleighCompile *s, SymbolScope *scope);
bool                SleighCompile_expandMacros(SleighCompile *s, ConstructTpl *ctpl, struct dynarray *macrotable);

#define SleighCompile_isInRoot(s, ct)           (s->root == Constructor_getParent(ct))
#define SleighCompile_getDefaultCodeSpace(s)    s->defaultcodespace
#define SleighCompile_getConstantSpace(s)       s->constantspace
#define SleighCompile_curLineNo(s)             ((struct slgh_preproc *)dynarray_back(&s->preproc))->lineno

extern SleighCompile*   slgh;

#ifdef __cplusplus
}
#endif

#endif