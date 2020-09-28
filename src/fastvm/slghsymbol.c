#include "vm.h"
#include "slghsymbol.h"

DecisionNode*   DecisionNode_new(DecisionNode *p)
{
    DecisionNode *dn = vm_mallocz(sizeof(dn[0]));

    dn->parent = p;

    return dn;
}

void            DecisionNode_delete(DecisionNode *d)
{
}

void            DecisionNode_addConstructorPair(DecisionNode *dnode, DisjointPattern *pat, Constructor *ct)
{
}

void            DecisionNode_split(DecisionNode *node, DecisionProperties *props)
{
}

char*           SymbolTypeStr(int type)
{
    switch (type) {
    case empty:                 return "empty";
    case space_symbol:          return "space_symbol";
    case token_symbol:          return "token_symbol";
    case userop_symbol:         return "usreop_symbol";
    case value_symbol:          return "value_symbol";
    case valuemap_symbol:       return "valuemap_symbol";
    case name_symbol:           return "name_symbol";
    case varnode_symbol:        return "varnode_symbol";
    case varnodelist_symbol:    return "varnodelist_symbol";
    case operand_symbol:        return "operand_symbol";
    case start_symbol:          return "start_symbol";
    case end_symbol:            return "end_symbol";
    case subtable_symbol:       return "subtable_symbol";
    case macro_symbol:          return "macro_symbol";
    case section_symbol:        return "section_symbol";
    case bitrange_symbol:       return "bitrange_symbol";
    case context_symbol:        return "context_symbol";
    case epsilon_symbol:        return "epsilon_symbol";
    case label_symbol:          return "label_symbol";
    case dummy_symbol:          return "dummy_symbol";
    case flow_dest_symbol:      return "flow_dest_symbol";
    case flow_ref_symbol:       return "flow_ref_symbol";
    default:
        vm_error("unsupport symboltype %d", type);
        return NULL;
    }
}

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

        case context_symbol:
            return s->context.patval;

        case epsilon_symbol:
            return s->epsilon.patexp;

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

        case varnodelist_symbol:
            return s->varnodeList.patval;

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

Constructor*    Constructor_newS(SubtableSymbol *sym)
{
    Constructor *c = vm_mallocz(sizeof(c[0]));

    c->parent = sym;
    c->firstwhitespace = -1;

    return c;
}

void            Constructor_delete(Constructor *c)
{
    vm_free(c);
}

void            Constructor_addSyntax(Constructor *c, const char *syn)
{
    int i, len;
    bool hasNonSpace = false;
    char *syntrim;
    CString *back;

    if (!syn || !syn[0]) return;

    len = strlen(syn);
    for (i = 0; i < len; i++) {
        if (syn[i] != ' ') {
            hasNonSpace = true;
            break;
        }
    }

    if (hasNonSpace)
        syntrim = (char *)syn;
    else
        syntrim = " ";
    if ((c->firstwhitespace == -1) && !strcmp(syntrim, " "))
        c->firstwhitespace = c->printpiece.len;
    if (!c->printpiece.len)
        dynarray_add(&c->printpiece, cstr_new(syntrim, strlen(syntrim)));
    else if ((back = dynarray_back(&c->printpiece)) && !strcmp(back->data, " ") && !strcmp(syntrim, " ")) {
    }
    else if (back->data[0] == '\n' || !strcmp(back->data, " ") || !strcmp(syntrim, " ")) {
        dynarray_add(&c->printpiece, cstr_new(syntrim, strlen(syntrim)));
    }
    else {
        cstr_cat(back, syntrim, strlen(syntrim));
    }
}

void            Constructor_markSubtableOperands(Constructor *c, struct dynarray *check)
{
    int i;

    while (check->len < c->operands.len)
        dynarray_add(check, NULL);

    for (i = 0; i < c->operands.len; i++) {
        SleighSymbol *s = c->operands.ptab[i];
        TripleSymbol *sym = OperandSymbol_getDefiningSymbol(s);
        check->ptab[i] = (sym && (sym->type == subtable_symbol)) ? 0 : (void *)2;
    }
}

void            Constructor_setNamedSection(Constructor *c, ConstructTpl *tpl, int id)
{
    while (c->namedtempl.len < id) 
        dynarray_add(&c->namedtempl, NULL);

    c->namedtempl.ptab[id] = tpl;
}

void            Constructor_addEquation(Constructor *c, PatternEquation *pe)
{
    c->pateq = pe;
    c->pateq->refcount++;
}

void            Constructor_removeTrailingSpace(Constructor *c)
{
    if (c->printpiece.len) {
        CString *cs = dynarray_back(&c->printpiece);
        if (!strcmp(cs->data, " ")) {
            dynarray_pop(&c->printpiece);
            cstr_delete(cs);
        }
    }
}

void            Constructor_addInvisibleOperand(Constructor *c, OperandSymbol *sym)
{
    dynarray_add(&c->operands, sym);
}

void            Constructor_addOperand(Constructor *c, OperandSymbol *sym)
{
    CString *cstr = cstr_new("\n ", 2);
    cstr_ccat(cstr, 'A' + c->operands.len);

    dynarray_add(&c->operands, sym);
    dynarray_add(&c->printpiece, cstr);
}

void            SleighSymbol_delete(SleighSymbol *sym)
{
    vm_free(sym);
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

void            SubtableSymbol_buildDecisionTree(SubtableSymbol *sym, DecisionProperties *props)
{
    int i, j;
    if (!sym->subtable.pattern) return;

    Pattern *pat;

    sym->subtable.decisiontree = DecisionNode_new(NULL);
    for (i = 0; i < sym->subtable.construct.len; i++) {
        Constructor *ct = sym->subtable.construct.ptab[i];
        pat = ct->pattern->pattern;
        if (Pattern_numDisjoint(pat) == 0)
            DecisionNode_addConstructorPair(sym->subtable.decisiontree, pat, ct);
        else { // 假如不为0，则一定是orlist
            for (j = 0; j < pat->or.orlist.len; j++) {
                DecisionNode_addConstructorPair(sym->subtable.decisiontree, pat->or.orlist.ptab[j], ct);
            }
        }
    }

    DecisionNode_split(sym->subtable.decisiontree, props);
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
    EpsilonSymbol *sym = SleighSymbol_new(epsilon_symbol, name);

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

UserOpSymbol*   UserOpSymbol_new(const char *name)
{
    UserOpSymbol* sym = SleighSymbol_new(userop_symbol, name);

    return sym;
}

MacroSymbol*    MacroSymbol_new(const char *name, int i)
{
    MacroSymbol * sym = SleighSymbol_new(macro_symbol, name);

    sym->macro.index = i;

    return sym;
}

LabelSymbol*    LabelSymbol_new(const char *name, int i)
{
    LabelSymbol* sym = SleighSymbol_new(label_symbol, name);

    sym->label.index = i;

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

void            OperandSymbol_defineOperand(OperandSymbol *sym, PatternExpression *pe)
{
    if (sym->operand.defexp || sym->operand.triple)
        vm_error("Redefining operand:%s %s:%d", sym->name, basename(sym->filename), sym->lineno);

    sym->operand.defexp = pe;
    pe->refcount++;
}

void            OperandSymbol_defineOperandS(OperandSymbol *sym, SleighSymbol *dsym)
{
    if (sym->operand.defexp || sym->operand.triple)
        vm_error("Redefining operand:%s %s:%d", sym->name, basename(sym->filename), sym->lineno);

    sym->operand.triple = dsym;
}

void            NameSymbol_checkTableFill(NameSymbol *sym)
{
    intb min = PatternValue_minValue(sym->nameS.patval);
    intb max = PatternValue_maxValue(sym->nameS.patval);
    sym->nameS.tableisfilled = (min >= 0) && (max < sym->nameS.nametable->len);
    int i;
    for (i = 0; i < sym->nameS.nametable->len; i++) {
        CString *cstr = sym->nameS.nametable->ptab[i];
        if (!strcmp(cstr->data, "_") || !strcmp(cstr->data, "\t")) {
            cstr->data[0] = '\t';
            sym->nameS.tableisfilled = false;
            break;
        }
    }
}

NameSymbol*     NameSymbol_new(const char *name, PatternValue *pv, struct dynarray *nt)
{
    NameSymbol* sym = SleighSymbol_new(name_symbol, name);

    sym->nameS.patval = pv;
    pv->refcount++;
    sym->nameS.nametable = nt;
    NameSymbol_checkTableFill(sym);
    return sym;
}

TokenSymbol*    TokenSymbol_new(Token *t)
{
    TokenSymbol *sym = SleighSymbol_new(token_symbol, t->name);

    sym->token.tok = t;

    return sym;
}

ValueSymbol*    ValueSymbol_new(const char *name, PatternValue *pv)
{
    ValueSymbol *sym = SleighSymbol_new(value_symbol, name);

    sym->value.patval = pv;

    pv->refcount++;

    return sym;
}

void                VarnodeListSymbol_checkTableFill(VarnodeListSymbol *vl)
{
    intb min = PatternValue_minValue(vl->varnodeList.patval);
    intb max = PatternValue_maxValue(vl->varnodeList.patval);
    int i;
    vl->varnodeList.tableisfilled = (min >= 0) && (max < vl->varnodeList.varnode_table.len);
    for (i = 0; i < vl->varnodeList.varnode_table.len; i++) {
        if (vl->varnodeList.varnode_table.ptab[i] == 0) {
            vl->varnodeList.tableisfilled = false;
            break;
        }
    }
}

VarnodeListSymbol*  VarnodeListSymbol_new(char *name, PatternValue *pv, struct dynarray *vt)
{
    VarnodeListSymbol *v = SleighSymbol_new(varnodelist_symbol, name);

    v->varnodeList.patval = pv;
    pv->refcount++;
    v->type = varnodelist_symbol;

    dynarray_insert(&v->varnodeList.varnode_table, vt);
    VarnodeListSymbol_checkTableFill(v);

    return v;
}

void            OperandSymbol_delete(OperandSymbol *sym)
{
    vm_free(sym);
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

int         SymbolTable_new(SymbolTable *s)
{
    return 0;
}

void            SymbolTable_delete(SymbolTable *s)
{
    int i;
    for (i = 0; i < s->table.len; i++) {
    }
}

SleighSymbol*   SymbolTable_findSymbolInternal(SymbolTable *s, SymbolScope *scope, const char *name)
{
    return NULL;
}

void            SymbolTable_addScope(SymbolTable *s)
{
    SymbolScope *scope = SymbolScope_new(s->curscope, s->table.len);;

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

            SymbolScope_addSymbol(s->table.ptab[b->scopeid], b);
            SleighSymbol_delete(a);
            return;
        }
    }
}

void            SymbolTable_saveXml(SymbolTable *s, FILE *o)
{
    int i;
    fprintf(o, "<symbol_table");
    fprintf(o, " scopesize=\"%d\"", s->table.len);
    fprintf(o, " symbolsize=\"%d\">\n", s->symbolist.len);
    for (i = 0; i < s->table.len; i++) {
        SymbolScope *scope = s->table.ptab[i];
        SymbolScope *parent = scope->parent;
        fprintf(o, "<scope id=\"0x%x\" parent=\"0x%x\"/>\n", scope->id, parent?parent->id:0);
    }

    for (i = 0; i < s->symbolist.len; i++) {
        SleighSymbol *sym = s->symbolist.ptab[i];
        SleighSymbol_saveXmlHeader(sym, o);
    }

    for (i = 0; i < s->symbolist.len; i++) {
        SleighSymbol *sym = s->symbolist.ptab[i];
        SleighSymbol_saveXml(sym, o);
    }

    fprintf(o, "</symbol_table>\n");
}

static void calc_maskword(int sbit, int ebit, int *num, int *shift, uintm *mask)
{
    int msize = 8 * sizeof(uintm);

    num[0] = sbit / msize;
    if (num[0] != ebit / msize)
        vm_error("Contxt field not contained within one machine int");

    sbit -= num[0] * msize;
    ebit -= num[0] * msize;
    shift[0] = msize - ebit - 1;
    mask[0] = (~((uintm)0)) >> (sbit + shift[0]);
    mask[0] <<= shift[0];
}

ContextOp*      ContextOp_new(int sbit, int ebit, PatternExpression *pe)
{
    ContextOp *co = vm_mallocz(sizeof(co[0]));

    co->type = context_op;
    calc_maskword(sbit, ebit, &co->op.num, &co->op.shift, &co->op.mask);
    co->op.patexp = pe;
    pe->refcount++;
    return co;
}

ContextCommit*  ContextCommit_new(TripleSymbol *s, int sbit, int ebit, bool fl)
{
    ContextCommit *cc = vm_mallocz(sizeof(cc[0]));

    cc->type = context_commit;
    cc->commit.sym = s;
    cc->commit.flow = fl;

    int shift;

    calc_maskword(sbit, ebit, &cc->commit.num, &shift, &cc->commit.mask);

    return cc;
}

ContextChange*  ContextChange_clone(ContextChange *cc)
{
    ContextChange *clone = vm_mallocz(sizeof(cc[0]));

    *clone = *cc;
    if (clone->type == context_op)
        clone->op.patexp->refcount++;

    return clone;
}

void            ContextChange_delete(ContextChange *cc)
{
    vm_free(cc);
}

void            ContextChange_saveXml(ContextChange *cc, FILE *o)
{
    if (cc->type == context_op) {
        fprintf(o, "<context_op i=\"%d\" shift=\"%d\" mask=\"0x%x\"", cc->op.num, cc->op.shift, cc->op.mask);
        PatternExpression_saveXml(cc->op.patexp, o);
        fprintf(o, "</context_op>\n");
    }
    else { // context_commit
        fprintf(o, "<commit ");
        a_v_u(o, "id", cc->commit.sym->id);
        a_v_i(o, "num", cc->commit.num);
        a_v_u(o, "mask", cc->commit.mask);
        a_v_b(o, "flow", cc->commit.flow);
        fprintf(o, "/>\n");
    }
}

void            SleighSymbol_saveXmlHeaderAttr(SleighSymbol *s, FILE *o)
{
    fprintf(o, " name=\"%s\"", s->name);
    fprintf(o, " id=\"0x%x\"", s->id);
    fprintf(o, " scope=\"0x%x\"", s->scopeid);
}

void            SleighSymbol_saveXmlHeader(SleighSymbol *s, FILE *o)
{
    switch (s->type) {
    case space_symbol:          fprintf(o, "<space_sym_head");      break;
    case userop_symbol:         fprintf(o, "<userop_head");         break;
    case epsilon_symbol:        fprintf(o, "<epsilon_sym_head");    break;
    case value_symbol:          fprintf(o, "<value_sym_head");      break;
    case valuemap_symbol:       fprintf(o, "<valuemap_sym_head");   break;
    case name_symbol:           fprintf(o, "<name_sym_head");       break;
    case varnode_symbol:        fprintf(o, "<varnode_sym_head");    break;
    case context_symbol:        fprintf(o, "<context_sym_head");    break;
    case varnodelist_symbol:    fprintf(o, "<varlist_sym_head");    break;
    case operand_symbol:        fprintf(o, "<operand_sym_head");    break;
    case start_symbol:          fprintf(o, "<start_sym_head");      break;
    case end_symbol:            fprintf(o, "<end_sym_head");        break;
    case flow_dest_symbol:      fprintf(o, "<flowdest_sym_head");   break;
    case flow_ref_symbol:       fprintf(o, "<flowref_sym_head");    break;
    case subtable_symbol:       fprintf(o, "<subtable_sym_head");   break;
    default: 
        //vm_error("unknown symbol type[%d]", s->type);
        //break;
        return;
    }

    SleighSymbol_saveXmlHeaderAttr(s, o);

    fprintf(o, "/>\n");
}

void        Constructor_saveXml(Constructor *ct, FILE *o)
{
    int i;
    fprintf(o, "<constructor");
    fprintf(o, "parent=\"0x%x\"", ct->parent->id);
    fprintf(o, " first=\"%d\"", ct->firstwhitespace);
    fprintf(o, " lenght=\"%d\"", ct->minimumlength);
    fprintf(o, " line=\"%d\"", ct->lineno);
    fprintf(o, " filename=\"%s\">\n", ct->filename);
    for (i = 0; i < ct->operands.len; i++) {
        OperandSymbol *sym = ct->operands.ptab[i];
        fprintf(o, "\t<operand id=\"0x%x\" name=\"%s\"/>\n", sym->id, sym->name);
    }

    for (i = 0; i < ct->printpiece.len; i++) {
        CString *cs = ct->printpiece.ptab[i];

        if (cs->data[0] == '\n') {
            int index = cs->data[1] - 'A';
            fprintf(o, "\t<opprint id=\"%d\" />\n", index);
        }
        else {
            fprintf(o, "\t<print piece=\"");
            xml_escape_out(o, cs->data);
            fprintf(o, "\"/>\n");
        }
    }
    for (i = 0; i < ct->context->size; i++) {
        struct ContextChange *cc = ct->context->ptab[i];
        ContextChange_saveXml(cc, o);
    }
    fprintf(o, "</constructor>\n");
}

void            Constructor_printMnemonic(Constructor *ct, CString *s, ParserWalker *walker)
{
    if (ct->flowthruindex) {
        SubtableSymbol *sym = ((OperandSymbol *)ct->operands.ptab[ct->flowthruindex])->operand.triple;
        if (sym->type == subtable_symbol) {
            ParserWalker_pushOperand(walker, ct->flowthruindex);
            Constructor_printBody(walker->point->ct, s, walker);
            ParserWalker_popOperand(walker);
            return;
        }
    }

    if (ct->firstwhitespace == -1)
        return;         

    int i;
    for (i = ct->firstwhitespace + 1; i < ct->printpiece.len; i++) {
        CString *p = ct->printpiece.ptab[i];
        if (p->data[0] == '\n') {
            int index = p->data[1] - 'A';
            SleighSymbol_print(ct->operands.ptab[index], s, walker);
        }
        else
            cstr_cat(s, p->data, p->size);
    }
}

void            Constructor_printBody(Constructor *ct, CString *s, ParserWalker *walker)
{
    if (ct->flowthruindex == -1) {
        SubtableSymbol *sym = ((OperandSymbol *)ct->operands.ptab[ct->flowthruindex])->operand.triple;
        if (sym->type == subtable_symbol) {
            ParserWalker_pushOperand(walker, ct->flowthruindex);
            Constructor_printBody(walker->point->ct, s, walker);
            ParserWalker_popOperand(walker);
            return;
        }
    }

    if (ct->firstwhitespace == -1)
        return;

    int i;
    for (i = ct->firstwhitespace + 1; i < ct->printpiece.len; i++) {
        CString *p = ct->printpiece.ptab[i];
        if (p->data[0] == '\n') {
            int index = p->data[1] - 'A';
            SleighSymbol_print(ct->operands.ptab[index], s, walker);
        }
        else
            cstr_cat(s, p->data, p->size);
    }
}

void            Constructor_print(Constructor *ct, CString *cs, ParserWalker *walker)
{
    int i;
    for (i = 0; i < ct->printpiece.len; i++) {
        CString *cs1 = ct->printpiece.ptab[i];
        if (cs1->data[0] == '\n') {
            int index = cs1->data[1] - 'A';
            SleighSymbol_print(ct->operands.ptab[index], cs, walker);
        }
        else {
            cstr_cat(cs, cs1->data, cs1->size);
        }
    }
}

void        DecisionNode_saveXml(DecisionNode *d, FILE *o)
{
}

void        SleighSymbol_saveXml(SleighSymbol *s, FILE *o)
{
    int i;
    switch (s->type) {
    case space_symbol:
        fprintf(o, "<space_sym");       
        SleighSymbol_saveXmlHeaderAttr(s, o);
        fprintf(o, "/>\n");
        break;

    case userop_symbol:     
        fprintf(o, "<userop");       
        SleighSymbol_saveXmlHeaderAttr(s, o);
        fprintf(o, "/>\n");
        break;

    case epsilon_symbol:    
        fprintf(o, "<epsilon_sym");  
        SleighSymbol_saveXmlHeaderAttr(s, o);
        fprintf(o, "/>\n");
        break;

    case value_symbol:
        fprintf(o, "<value_sym");
        SleighSymbol_saveXmlHeaderAttr(s, o);
        fprintf(o, ">\n");

        PatternExpression_saveXml(s->value.patval, o);
        fprintf(o, "</value_sym>\n");
        break;

    case valuemap_symbol:
        fprintf(o, "<valuemap_sym");
        SleighSymbol_saveXmlHeaderAttr(s, o);
        fprintf(o, ">\n");
        PatternExpression_saveXml(s->valuemap.patval, o);
        for (i = 0; i < s->valuemap.valuetable.len; i++) {
            fprintf(o, "<valuetab val=\"%lld\"/>\n", *((intb *)(s->valuemap.valuetable.ptab[i])));
        }
        fprintf(o, "</valuemap_sym>\n");
        break;

    case name_symbol:
        fprintf(o, "<name_sym");
        SleighSymbol_saveXmlHeaderAttr(s, o);
        fprintf(o, ">\n");
        PatternExpression_saveXml(s->nameS.patval, o);
        for (i = 0; i < s->nameS.nametable->len; i++) {
            CString *cs = s->nameS.nametable->ptab[i];
            if (cs && cs->size) {
                fprintf(o, "<nametab name=\"%s\" />\n", cs->data);
            }
            else {
                fprintf(o, "<nametab/>\n");
            }
        }
        fprintf(o, "</name_sym>\n");
        break;

    case varnode_symbol:
        fprintf(o, "<varnode_sym");
        SleighSymbol_saveXmlHeaderAttr(s, o);
        fprintf(o, " space=\"%s\"", s->varnode.fix.space->name);
        fprintf(o, " offset=\"0x%llx\"", s->varnode.fix.offset);
        fprintf(o, " size=\"%d\">\n", s->varnode.fix.size);
        // PatternlessSymbol::saveXml
        fprintf(o, "</varnode_sym>\n");
        break;

    case context_symbol:
        fprintf(o, "<context_sym");
        SleighSymbol_saveXmlHeaderAttr(s, o);
        fprintf(o, " varnode=\"0x%x\"", s->context.vn->id);
        fprintf(o, " low=\"%d\"", s->context.low);
        fprintf(o, " high=\"%d\"", s->context.high);
        a_v_b(o, "flow", s->context.flow);
        fprintf(o, ">\n");
        PatternExpression_saveXml(s->context.patval, o);
        fprintf(o, "</context_sym>\n");
        break;

    case varnodelist_symbol:
        fprintf(o, "<varlist_sym");
        SleighSymbol_saveXmlHeaderAttr(s, o);
        fprintf(o, ">\n");
        PatternExpression_saveXml(s->varnodeList.patval, o);
        for (i = 0; i < s->varnodeList.varnode_table.len; i++) {
            VarnodeSymbol *var = s->varnodeList.varnode_table.ptab[i];
            if (var)
                fprintf(o, "<var id=\"0x%x\" />\n", var->id);
            else
                fprintf(o, "<null/>\n");
        }
        fprintf(o, "</varlist_sym>");
        break;

    case operand_symbol:
        fprintf(o, "<operand_sym");
        SleighSymbol_saveXmlHeaderAttr(s, o);
        if (s->operand.triple)
            fprintf(o, " subsym=\"0x%x\"", s->operand.triple->id);
        fprintf(o, " off=\"%d\"", s->operand.reloffset);
        fprintf(o, " base=\"%d\"", s->operand.offsetbase);
        fprintf(o, " minlen=\"%d\"", s->operand.minimumlength);
        a_v_b(o, "code", OperandSymbol_isCodeAddress(s));
        fprintf(o, " index=\"%d\"", s->operand.hand);
        PatternExpression_saveXml(s->operand.localexp, o);
        if (s->operand.defexp)
            PatternExpression_saveXml(s->operand.defexp, o);
        fprintf(o, "</operand_sym>");
        break;

    case start_symbol:
        fprintf(o, "<start_sym");
        SleighSymbol_saveXmlHeaderAttr(s, o);
        fprintf(o, "/>\n");
        break;

    case end_symbol:
        fprintf(o, "<end_sym");
        SleighSymbol_saveXmlHeaderAttr(s, o);
        fprintf(o, "/>\n");
        break;

    case flow_dest_symbol:
        fprintf(o, "<flowdest_sym");
        SleighSymbol_saveXmlHeaderAttr(s, o);
        fprintf(o, "/>\n");
        break;

    case flow_ref_symbol:
        fprintf(o, "<flowref_sym");
        SleighSymbol_saveXmlHeaderAttr(s, o);
        fprintf(o, "/>\n");
        break;

    case subtable_symbol:
        fprintf(o, "<subtable_sym");
        SleighSymbol_saveXmlHeaderAttr(s, o);
        fprintf(o, " numct=\"%d\" >\n", s->subtable.construct.len);
        for (i = 0; i < s->subtable.construct.len; i++) {
            Constructor_saveXml(s->subtable.construct.ptab[i], o);
        }
        DecisionNode_saveXml(s->subtable.decisiontree, o);
        fprintf(o, "</subtable_sym>");
        break;

    default:
        return;
        //vm_error("unknown sym type[%d]", s->type);
        //break;
    }
}

void            SleighSymbol_print(SleighSymbol *s, CString *cs, ParserWalker *walker)
{
    char buf[32];
    intb val;
    int ind;
    CString *cs1;

    switch(s->type) {
    case epsilon_symbol:
        cstr_cat(cs, "0", 0);
        break;

    case value_symbol:
        val = TokenField_getValue(s->value.patval, walker);
        sprintf(buf, "%s%llx", val > 0?"0x":"-0x", val > 0?val:-val);
        cstr_cat(cs, buf, 0);
        break;

    case valuemap_symbol:
        ind = (int)TokenField_getValue(s->valuemap.patval, walker);
        val = *((intb *)s->valuemap.valuetable.ptab[ind]);
        sprintf(buf, "%s%llx", val > 0?"0x":"-0x", val > 0?val:-val);
        cstr_cat(cs, buf, 0);
        break;

    case name_symbol:
        ind = (int)TokenField_getValue(s->nameS.patval, walker);
        cs1 = s->nameS.nametable->ptab[ind];
        cstr_cat(cs, cs1->data, cs1->size);
        break;

    case varnodelist_symbol:
        ind = (int)TokenField_getValue(s->varnodeList.patval, walker);
        if (ind >= s->varnodeList.varnode_table.len)
            vm_error("Value out of range for varnode table");
        cs1 = s->varnodeList.varnode_table.ptab[ind];
        cstr_cat(cs, cs1->data, cs1->size);
        break;

    case operand_symbol:
        ParserWalker_pushOperand(walker, s->operand.hand);
        if (s->operand.triple) {
            if (s->operand.triple->type == subtable_symbol) {
                Constructor_print(walker->point->ct, cs, walker);
            }
            else
                SleighSymbol_print(s->operand.triple, cs, walker);
        }
        else {
            val = TokenField_getValue(s->operand.defexp, walker);
        }
        ParserWalker_popOperand(walker);
        break;
    }
}
