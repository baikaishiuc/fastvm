
#ifndef __SLIGHSYMBOL_H__
#define __SLIGHSYMBOL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "space.h"
#include "context.h"

typedef struct SleighSymbol {
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
    } type;

    union {
        struct {
            AddrSpace *space;
        } spaceSym;

        struct {
            Token *tok;
        } tokenSym;

        struct {
            int templateid;     // Index into the constructTpl array
            int define_count;   // Number of definitions of this named section
            int ref_count;      // Number of references to this named section
        } sectionSym;

        struct {
            int index;          // A user-defined symbol
        } userOpSym;
    };

    int id;
    int scopeid;
    char name[1];
} SleighSymbol, SpaceSymbol, SectionSymbol;

SleighSymbol*   SpaceSymbol_new(AddrSpace *spc);
void            SpaceSymbol_delete(SleighSymbol *s);

SleighSymbol*   SectionSymbol_new(const char *name, int id);
void            SectionSymbol_delete(SleighSymbol *s);

typedef int(*SymbolCompare)(const SleighSymbol *a, const SleighSymbol *b);

typedef struct SymbolTree {
    SymbolCompare cmp;
} SymbolTree;

typedef struct SymbolScope {
    SymbolTable *tab;
} SymbolScope;

typedef struct SymbolTable {
    struct dynarray symbolist;
    struct dynarray table;
    SymbolScope *curscope;
    SymbolScope *skipScope;
} SymbolTable;

SymbolTable*    SymbolTable_new();
void            SymbolTable_delete(SymbolTable *s);

SleighSymbol*   SymbolTable_findSymbolInternal(SymbolTable *s, SymbolScope *scope, const char *name);
#define SymbolTable_getCurrentScope(s)  s->curscope
#define SymbolTable_getGlobalScope(s)   s->table[0]
#define SymbolTable_setCurrentScope(s, sc)  s->curscope = scope

void            SymbolTable_addScope(SymbolTable *s);
void            SymbolTable_popScope(SymbolTable *s);
void            SymbolTable_addGlobalSymbol(SymbolTable *s, SleighSymbol *a);
void            SymoblTable_addSymbol(SymbolTable *s, SleighSymbol *a);
SleighSymbol*   SymbolTable_findSymbol(SymbolTable *s, const char *name);
SleighSymbol*   SymbolTable_findSymbolSkip(SymbolTable *s, const char *name, int skip);
SleighSymbol*   SymbolTable_findGlobalSymbol(SymbolTable *s, const char *name);
SleighSymbol*   SymbolTable_findSymbolById(SymbolTable *s, int id);
void            SymbolTable_replaceSymbol(SleighSymbol *a, SleighSymbol *b);




#ifdef __cplusplus
}
#endif

#endif