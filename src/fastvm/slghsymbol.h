
#ifndef __SLIGHSYMBOL_H__
#define __SLIGHSYMBOL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "semantics.h"
#include "slghpatexpress.h"

#define CODE_ADDRESS            0x01
#define OFFSET_IRREL            0x02
#define VARIABLE_LEN            0x04
#define MARKED                  0x08

typedef struct SleighSymbol SleighSymbol, SpaceSymbol, TokenSymbol, SectionSymbol, UserOpSymbol, TripleSymbol, FamilySymbol,
PatternlessSymbol, EpsilonSymbol, ValueSymbol, ValueMapSymbol, NameSymbol, VarnodeSymbol, BitRangeSymbol,
ContextSymbol, VarnodeListSymbol, OperandSymbol, StartSymbol, EndSymbol, MacroSymbol, SubtableSymbol, LabelSymbol,
BitrangeSymbol, SpecificSymbol;

typedef struct SymbolTable  SymbolTable;
typedef struct SymbolScope  SymbolScope;
typedef struct DecisionNode DecisionNode;
typedef struct ContextChange ContextChange, ContextOp, ContextCommit;

struct DecisionNode {
    struct dynarray list;
    struct dynarray children;
    int num;
    bool contextdecision;
    int startbit;
    int bitsize;
    DecisionNode *parent;
};

DecisionNode*   DecisionNode_new(DecisionNode *d);
void            DecisionNode_delete(DecisionNode *d);

struct SleighSymbol {
    enum {
        empty,
        space_symbol,
        token_symbol,
        userop_symbol,
        value_symbol,
        valuemap_symbol,
        name_symbol,
        varnode_symbol,
        varnodelist_symbol,
        operand_symbol,
        start_symbol,
        end_symbol,
        subtable_symbol,
        macro_symbol,
        section_symbol,
        bitrange_symbol,
        context_symbol,
        epsilon_symbol,
        label_symbol,
        dummy_symbol,
        flow_dest_symbol,
        flow_ref_symbol,
    } type;

    union {
        struct {
            AddrSpace *space;
        } space;

        struct {
            Token *tok;
        } token;

        struct {
            struct dynarray *nametable;
            bool tableisfilled;
            PatternValue *patval;
        } nameS;

        struct {
            int templateid;     // Index into the constructTpl array
            int define_count;   // Number of definitions of this named section
            int ref_count;      // Number of references to this named section
        } section;

        int index;          // A user-defined symbol

        struct {
            PatternValue *patval;
        } value;

        struct {
            uint32_t    reloffset;
            int32_t     offsetbase;     
            int32_t     minimumlength;
            /* 不理解为什么NASA的人要把参数的索引叫hand，不叫index，这个地方第一次看很头晕，感觉很多地方他们都自己发明了一套术语 */
            int32_t     hand;
            OperandValue *localexp;
            TripleSymbol *triple;
            PatternExpression *defexp;
            uint32_t    flags;
        } operand;

        struct {
            ConstantValue *patexp;
            AddrSpace *const_space;
        } epsilon;

        struct {
            ConstantValue *patexp;
            VarnodeData fix;
            bool context_bits;
        } varnode;

        struct {
            struct dynarray varnode_table;
            bool tableisfilled;
            PatternValue *patval;
        } varnodeList;

        struct {
            AddrSpace *const_space;
            PatternExpression *patexp;
        } start, end;

        struct {
            AddrSpace *const_space;
        } flow_dest, flow_ref;

        struct {
            u4 index;
            bool isplaced;
            int refcount;
        } label;

        struct {
            VarnodeSymbol *varsym;
            u4 bitoffset;
            u4 numbits;
        } bitrange;

        struct {
            int index;
            ConstructTpl *construct;
            struct dynarray operands;
        } macro;

        struct {
            int index;
        } userop;

        struct {
            TokenPattern *pattern;
            bool beingbuild;
            bool erros;
            struct dynarray construct;
            DecisionNode *decisiontree;
        } subtable;

        struct {
            VarnodeSymbol *vn;
            int low;
            int high;
            bool flow;
            PatternValue *patval;
        } context;
    };

    int id;
    int scopeid;
    struct rb_node in_scope;

    /* 在源文件中的位置，方便调试 */
    int lineno;
    char *filename;
    char name[1];
};

char*           SymbolTypeStr(int type);
void            SleighSymbol_delete(SleighSymbol *sym);
SleighSymbol*   SpaceSymbol_new(AddrSpace *spc);
SleighSymbol*   SectionSymbol_new(const char *name, int id);

SleighSymbol*   SubtableSymbol_new(const char *name);
void            SubtableSymbol_addConstructor(SubtableSymbol *sym, Constructor *ct);

StartSymbol*    StartSymbol_new(const char *name, AddrSpace *spc);
EndSymbol*      EndSymbol_new(const char *name, AddrSpace *spc);
EpsilonSymbol*  EpsilonSymbol_new(const char *name, AddrSpace *spc);
VarnodeSymbol*  VarnodeSymbol_new(const char *name, AddrSpace *base, uintb offset, int size);
MacroSymbol*    MacroSymbol_new(const char *name, int i);

LabelSymbol*    LabelSymbol_new(const char *name, int i);

UserOpSymbol*   UserOpSymbol_new(const char *name);

OperandSymbol*  OperandSymbol_new(const char *name, int index, Constructor *ct);
void            OperandSymbol_defineOperand(OperandSymbol *sym, PatternExpression *pe);
void            OperandSymbol_defineOperandS(OperandSymbol *sym, SleighSymbol *dsym);

NameSymbol*     NameSymbol_new(const char *name, PatternValue *pv, struct dynarray *nt);
TokenSymbol*    TokenSymbol_new(Token *t);
ValueSymbol*    ValueSymbol_new(const char *name, PatternValue *pv);

VarnodeListSymbol*  VarnodeListSymbol_new(char *name, PatternValue *pv, struct dynarray *vt);

ContextSymbol*  ContextSymbol_new(const char *name, ContextField *pate, VarnodeSymbol *v, int l, int h, bool fl);

void MacroSymbol_addOperand(MacroSymbol *sym, OperandSymbol *operand);
#define MacroSymbol_getOperand(sym, ind)        sym->macro.operands.ptab[ind]
#define MacroSymbol_getNumOperands(sym)     sym->macro.operands.len
#define OperandSymbol_isCodeAddress(sym)      (sym->operand.flags&CODE_ADDRESS)

PatternValue*       SleighSymbol_getPatternValue(SleighSymbol *s);
PatternExpression*  SleighSymbol_getPatternExpression(SleighSymbol *s);

VarnodeTpl*     SleighSymbol_getVarnode(SleighSymbol *sym);
VarnodeTpl*     VarnodeSymbol_getVarnode(SleighSymbol *sym);
VarnodeTpl*     StartSymbol_getVarnode(StartSymbol *sym);
VarnodeTpl*     EndSymbol_getVarnode(EndSymbol *sym);
VarnodeTpl*     SpecificSymbol_getVarnode(SpecificSymbol *sym);

#define OperandSymbol_getDefiningSymbol(s) (((s)->type == operand_symbol) ? s->operand.triple:NULL)


#define BitrangeSymbol_getParentSymbol(sym)             sym->bitrange.varsym
#define BitrangeSymbol_getBitOffset(sym)                sym->bitrange.bitoffset
#define BitrangeSymbol_numBits(sym)                     sym->bitrange.numbits
#define OperandSymbol_getIndex(sym)                     sym->operand.hand
#define LabelSymbol_getIndex(sym)                       sym->label.index


inline AddrSpace*   SleighSymbol_getSpace(SleighSymbol *sym) {
    assert(sym->type == space_symbol);

    return sym->space.space;
}

inline void         SleighSymbol_setCodeAddress(SleighSymbol *sym) {
    assert(sym->type == operand_symbol);

    sym->operand.flags |= CODE_ADDRESS;
}

inline void         SleighSymbol_incrementRefCount(SleighSymbol *sym) {
    assert(sym->type == label_symbol);

    sym->label.refcount++;
}

#define SleighSymbol_getName(sym)           sym->name 

typedef int (*SymbolCompare)(const SleighSymbol *a, const SleighSymbol *b);

struct SymbolScope {
    SymbolScope *parent;
    struct rb_root tree;
    int id;
};

struct SymbolTable {
    struct dynarray symbolist;
    struct dynarray table;
    SymbolScope *curscope;
    SymbolScope *skipScope;
};

SymbolTable*    SymbolTable_new(void);
void            SymbolTable_delete(SymbolTable *s);

SleighSymbol*   SymbolTable_findSymbolInternal(SymbolTable *s, SymbolScope *scope, const char *name);
#define SymbolTable_getCurrentScope(s)  (s)->curscope
#define SymbolTable_getGlobalScope(s)   (s)->table.ptab[0]
#define SymbolTable_setCurrentScope(s, sc)  (s)->curscope = sc 

void            SymbolTable_addScope(SymbolTable *s);
void            SymbolTable_popScope(SymbolTable *s);
SymbolScope*    SymbolTable_skipScope(SymbolTable *s, int i);

void            SymbolTable_addGlobalSymbol(SymbolTable *s, SleighSymbol *a);
void            SymbolTable_addSymbol(SymbolTable *s, SleighSymbol *a);
SleighSymbol*   SymbolTable_findSymbol(SymbolTable *s, const char *name);
SleighSymbol*   SymbolTable_findSymbolSkip(SymbolTable *s, const char *name, int skip);
SleighSymbol*   SymbolTable_findGlobalSymbol(SymbolTable *s, const char *name);
SleighSymbol*   SymbolTable_findSymbolById(SymbolTable *s, int id);
void            SymbolTable_replaceSymbol(SymbolTable *s, SleighSymbol *a, SleighSymbol *b);

struct Constructor {
    TokenPattern *pattern;
    SubtableSymbol *parent;
    PatternEquation *pateq;
    struct dynarray operands;
    struct dynarray printpiece;
    struct dynarray *context;
    ConstructTpl *templ;
    struct dynarray namedtempl;
    int minimumlength;
    uintm id;
    int firstwhitespace;
    int flowthruindex;
    int lineno;
    bool inerror;
};

Constructor*    Constructor_new();
Constructor*    Constructor_newS(SubtableSymbol *sym);
void            Constructor_delete(Constructor *c);

void            Constructor_addSyntax(Constructor *c, const char *syn);
void            Constructor_markSubtableOperands(Constructor *c, struct dynarray *check);
void            Constructor_setNamedSection(Constructor *c, ConstructTpl *tpl, int id);
void            Constructor_addEquation(Constructor *c, PatternEquation *pe);
void            Constructor_removeTrailingSpace(Constructor *c);
void            Constructor_addInvisibleOperand(Constructor *c, OperandSymbol *sym);
void            Constructor_addOperand(Constructor *c, OperandSymbol *sym);
#define Constructor_getParent(ct)           (ct)->parent
#define Constructor_setMainSection(ct, tpl) (ct)->templ = tpl

struct ContextChange {
    enum {
        context_op,
        context_commit
    } type;

    union {
        struct {
            PatternExpression *patexp;
            int num;
            uintm mask;
            int shift;
        } op;

        struct {
            TripleSymbol *sym;
            int num;
            uintm mask;
            bool flow;
        } commit;
    };
};

ContextOp*      ContextOp_new(int sratbit, int endbit, PatternExpression *pe);

ContextCommit*  ContextCommit_new(TripleSymbol *s, int sbit, int ebit, bool fl);

ContextChange*  ContextChange_clone(ContextChange *cc);


void            ContextChange_delete(ContextChange *cc);



#ifdef __cplusplus
}
#endif

#endif