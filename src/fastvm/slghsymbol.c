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

        case context_symbol:
            return s->context.patval;

        default:
            vm_error("Cannot get value in symbol:%d", s->type);
            return NULL;
    }
}

Constructor*    Constructor_new()
{
    Constructor *c = vm_mallocz(sizeof(c[0]));

    c->firstwhitespace = -1;
    c->flowthruindex = -1;

    return c;
}

void            Constructor_delete(Constructor *c)
{
    vm_free(c);
}

void            Constructor_addSyntax(Constructor *c, const char *syn)
{
}

void            SleighSymbol_delete(SleighSymbol *sym)
{
}

SleighSymbol *SleighSymbol_new(int type, const char *name)
{
    SleighSymbol *sym = vm_mallocz(sizeof(sym[0]) + strlen(name));

    sym->type = type;
    strcpy(sym->name, name);

    return sym;
}

SleighSymbol*   SpaceSymbol_new(AddrSpace *spc)
{
    SleighSymbol *sym = vm_mallocz(sizeof(sym[0]) + strlen(spc->name));

    sym->type = space_symbol;
    sym->space.space = spc;
    strcpy(sym->name, spc->name);

    return sym;
}

SleighSymbol*   SectionSymbol_new(const char *name, int id)
{
    return NULL;
}

SleighSymbol*   SubtableSymbol_new(const char *name)
{
    SubtableSymbol *sym = vm_mallocz(sizeof(sym[0]) + strlen(name));

    sym->type = subtable_symbol;
    strcpy(sym->name, name);

    return sym;
}

void            SubtableSymbol_addConstructor(SubtableSymbol *sym, Constructor *ct)
{
    ct->id = sym->subtable.construct.len;
    dynarray_add(&sym->subtable.construct, ct);
}

StartSymbol*    StartSymbol_new(const char *name, AddrSpace *spc)
{
    StartSymbol *sym = vm_mallocz(sizeof (sym[0]) + strlen(name));

    sym->type = start_symbol;
    strcpy(sym->name, name);

    sym->start.const_space = spc;
    sym->start.patexp = StartInstructionValue_new();
    sym->start.patexp->refcount++;

    return sym;
}

EndSymbol*      EndSymbol_new(const char *name, AddrSpace *spc)
{
    EndSymbol *sym = vm_mallocz(sizeof(sym[0]) + strlen(name));

    sym->type = end_symbol;
    strcpy(sym->name, name);

    sym->end.const_space = spc;
    sym->end.patexp = EndInstructionValue_new();
    sym->end.patexp->refcount++;

    return sym;
}

EpsilonSymbol*  EpsilonSymbol_new(const char *name, AddrSpace *spc)
{
    EpsilonSymbol *sym = vm_mallocz(sizeof(sym[0]) + strlen(name));

    sym->type = epsilon_symbol;
    sym->epsilon.const_space = spc;

    return sym;
}

VarnodeSymbol*  VarnodeSymbol_new(const char *name, AddrSpace *base, uintb offset, int size)
{
    VarnodeSymbol *sym = SleighSymbol_new(varnode_symbol, name);

    sym->varnode.fix.space = base;
    sym->varnode.fix.offset = offset;
    sym->varnode.fix.size = size;

    return sym;
}

MacroSymbol*    MacroSymbol_new(const char *name, int i)
{
    MacroSymbol * sym = SleighSymbol_new(macro_symbol, name);

    sym->macro.index = i;

    return sym;
}

void MacroSymbol_addOperand(MacroSymbol *sym, OperandSymbol *operand)
{
    assert(sym->type == macro_symbol);

    dynarray_add(&sym->macro.operands, operand);
}

OperandSymbol*  OperandSymbol_new(const char *name, int index, Constructor *ct)
{
    OperandSymbol *sym = SleighSymbol_new(operand_symbol, name);

    sym->operand.hand = index;
    sym->operand.localexp = OperandValue_new(index, ct);
    PatternExpression_layClaim(sym->operand.localexp);

    return sym;
}

ContextSymbol*  ContextSymbol_new(const char *name, ContextField *pate, VarnodeSymbol *v, int l, int h, bool fl)
{
    ContextSymbol *sym = SleighSymbol_new(context_symbol, name);

    sym->context.vn = v;
    sym->context.low = l;
    sym->context.high = h;
    sym->context.flow = fl;
    sym->context.patval = pate;
    pate->refcount++;

    return sym;
}

VarnodeTpl*     SleighSymbol_getVarnode(SleighSymbol *sym)
{
    VarnodeTpl *res;

    switch (sym->type) {
    case operand_symbol:
        if (sym->operand.defexp)
            return VarnodeTpl_new2(sym->operand.hand, true);
        else {
            SleighSymbol *triple = sym->operand.triple;

            if (triple && (res = SleighSymbol_getVarnode(triple)))
                return res;

            if (triple && (triple->type == valuemap_symbol || triple->type == name_symbol))
                return VarnodeTpl_new2(sym->operand.hand, true);
            else
                return VarnodeTpl_new2(sym->operand.hand, false);
        }
        break;

    case varnode_symbol:
        return VarnodeTpl_new3(ConstTpl_newA(sym->varnode.fix.space),
                               ConstTpl_new2(real, sym->varnode.fix.offset),
                               ConstTpl_new2(real, sym->varnode.fix.size));

    case start_symbol:
        return VarnodeTpl_new3(ConstTpl_newA(sym->start.const_space),
                               ConstTpl_new1(j_start),
                               ConstTpl_new0());

    case end_symbol:
        return VarnodeTpl_new3(ConstTpl_newA(sym->start.const_space),
                               ConstTpl_new1(j_next),
                               ConstTpl_new0());

    case flow_dest_symbol:
        return VarnodeTpl_new3(ConstTpl_newA(sym->start.const_space),
                               ConstTpl_new1(j_flowdest),
                               ConstTpl_new0());

    case flow_ref_symbol:
        return VarnodeTpl_new3(ConstTpl_newA(sym->start.const_space),
                               ConstTpl_new1(j_flowref),
                               ConstTpl_new0());

    case epsilon_symbol:
        return VarnodeTpl_new3(ConstTpl_newA(sym->start.const_space),
                               ConstTpl_new2(real, 0),
                               ConstTpl_new2(real, 0));

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
    SymbolScope *scope;

    scope = vm_mallocz(sizeof(struct SymbolScope));
    scope->parent = s->curscope;
    s->curscope = scope;
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
SleighSymbol*   SymbolScope__findSymbol(SymbolScope *s, const char *name)
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

SleighSymbol*   SymbolScope_findSymbolInternal(SymbolScope *scope, const char *name)
{
    SleighSymbol *res;

    while (scope) {
        res = SymbolScope__findSymbol(scope, name);
        if (res)
            return res;

        scope = scope->parent;
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
