#include "vm.h"
#include "slghsymbol.h"

PatternExpression*  SleighSymbol_getPatternExpression(SleighSymbol *s)
{
    switch (s->type) { 
        case value_symbol:
            return s->value.patval;

        case operand_symbol:
            return s->operand.localexp;

        case start_symbol:
            return s->start.patexp;

        case end_symbol:
            return s->end.patexp;

        default:
            vm_error("Cannot use symbol in pattern");
            return NULL;
    }
}

PatternValue*       SleighSymbol_getPatternValue(SleighSymbol *s)
{
    switch (s->type) { 
        case value_symbol:
            return s->value.patval;

        case operand_symbol:
            return s->operand.localexp;

        case start_symbol:
            return s->start.patexp;

        case end_symbol:
            return s->end.patexp;

        default:
            vm_error("Cannot get value in symbol:%d", s->type);
            return NULL;
    }
}

Constructor*    Constructor_new()
{
    return NULL;
}

void            Constructor_delete(Constructor *c)
{
}

void            Constructor_addSyntax(Constructor *c, const char *syn)
{
}

void            SleighSymbol_delete(SleighSymbol *sym)
{
}

SleighSymbol*   SpaceSymbol_new(AddrSpace *spc)
{
    return NULL;
}

SleighSymbol*   SectionSymbol_new(const char *name, int id)
{
    return NULL;
}

SleighSymbol*   SubtableSymbol_new(const char *name)
{
    return NULL;
}

VarnodeTpl*     SleighSymbol_getVarnode(SleighSymbol *sym)
{
    switch (sym->type) {

    case operand_symbol:
        break;

    case varnode_symbol:
        break;

    case start_symbol:
        break;

    case end_symbol:
        break;

    default:
        break;
    }

    return NULL;
}

SymbolScope *SymbolScope_new(SymbolScope *parent, int id)
{
    SymbolScope *scope = vm_mallocz(sizeof(scope[0]));

    scope->parent = parent;
    scope->id = id;

    return scope;
}

void SymbolScope_delete(SymbolScope *p)
{
    vm_free(p);
}

SymbolTable*    SymbolTable_new()
{
    SymbolTable *tab = vm_mallocz(sizeof(tab[0]));

    return tab;
}

void            SymbolTable_delete(SymbolTable *s)
{
    int i;
    for (i = 0; i < s->table.len; i++) {
    }
    vm_free(s);
}

SleighSymbol*   SymbolTable_findSymbolInternal(SymbolTable *s, SymbolScope *scope, const char *name)
{
    return NULL;
}

void            SymbolTable_addScope(SymbolTable *s)
{
    s->curscope = vm_mallocz(sizeof(s->curscope));
    dynarray_add(&s->table, s->curscope);
}

void            SymbolTable_popScope(SymbolTable *s)
{
    if (s->curscope)
        s->curscope = s->curscope->parent;
}

SymbolScope*    SymbolTable_skipScope(SymbolTable *s, int i)
{
    SymbolScope *res = s->curscope;
    while (i > 0) {
        if (NULL == res->parent) return res;
        res = res->parent;
        --i;
    }
    return res;
}

#define _container_of(_node)    (SleighSymbol *)((char *)(_node) - offsetof(SleighSymbol, in_scope))

SleighSymbol*   SymbolScope_addSymbol(SymbolScope *scope, SleighSymbol *a)
{
    struct rb_node **n = &(scope->tree.rb_node), *parent = NULL;
    int ret;

    while (*n) {
        SleighSymbol *t = _container_of(*n);
        ret = strcmp(a->name, t->name);
        parent = *n;
        if (ret < 0)
            n = &((*n)->rb_left);
        else if (ret > 0)
            n = &((*n)->rb_right);
        else
            return t;
    }

    rb_link_node(&a->in_scope, parent, n);
    rb_insert_color(&a->in_scope, &scope->tree);

    return a;
}

SleighSymbol*   SymbolScope_findSymbolInternal(SymbolScope *s, const char *name)
{
    struct rb_node *n = s->tree.rb_node;
    int ret;

    while (n) {
        SleighSymbol *t = _container_of(n);
        ret = strcmp(name, t->name);
        if (ret < 0)
            n = n->rb_left;
        else if (ret > 0)
            n = n->rb_right;
        else
            return t;
    }

    return NULL;
}

void        SymbolScope_removeSymbol(SymbolScope *scope, SleighSymbol *a)
{
    rb_erase(&a->in_scope, &scope->tree);
}

void            SymbolTable_addGlobalSymbol(SymbolTable *s, SleighSymbol *a)
{
    a->id = s->symbolist.len;
    dynarray_add(&s->symbolist, a);

    SymbolScope *scope = s->table.ptab[0];
    a->scopeid = scope->id;
    SleighSymbol *res = SymbolScope_addSymbol(scope, a);
    if (res != a)
        vm_error("duplicate symbol name %s", a->name);
}

void            SymbolTable_addSymbol(SymbolTable *s, SleighSymbol *a)
{
    a->id = s->symbolist.len;
    dynarray_add(&s->symbolist, a);
    a->scopeid = s->curscope->id;
    SleighSymbol *res = SymbolScope_addSymbol(s->curscope, a);
    if (res != a)
        vm_error("duplicate symbol name %s", a->name);
}


SleighSymbol*   SymbolTable_findSymbol(SymbolTable *s, const char *name)
{
    return SymbolScope_findSymbolInternal(s->curscope, name);
}

SleighSymbol*   SymbolTable_findSymbolSkip(SymbolTable *s, const char *name, int skip)
{
    return SymbolScope_findSymbolInternal(SymbolTable_skipScope(s, skip), name);
}

SleighSymbol*   SymbolTable_findGlobalSymbol(SymbolTable *s, const char *name)
{
    return SymbolScope_findSymbolInternal(s->table.ptab[0], name);
}

SleighSymbol*   SymbolTable_findSymbolById(SymbolTable *s, int id)
{
    return s->symbolist.ptab[id];
}

void            SymbolTable_replaceSymbol(SymbolTable *s, SleighSymbol *a, SleighSymbol *b)
{
    SleighSymbol *sym;
    int i;

    for (i = s->table.len - 1; i >= 0; i--) {
        sym = SymbolScope_findSymbolInternal(s->table.ptab[i], a->name);
        if (sym == a) {
            SymbolScope_removeSymbol(s->table.ptab[i], a);
            b->id = a->id;
            b->scopeid = a->scopeid;
            s->symbolist.ptab[b->id] = b;
            SleighSymbol_delete(a);
            return;
        }
    }
}
