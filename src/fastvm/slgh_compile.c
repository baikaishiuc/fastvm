
#include "slgh_compile.h"

SleighCompile *slgh = NULL;
extern int yydebug;
extern FILE *yyin;
extern int yyparse(void);
int yylex_destroy(void) {
    return 0;
}


#define slgh_lineno(s)                  ((struct slgh_preproc *)dynarray_back(&s->preproc))->lineno
#define slgh_filename(s)                ((struct slgh_preproc *)dynarray_back(&s->preproc))->fullpath.data


RtlPair*        RtlPair_newV()
{
    RtlPair *rp = vm_mallocz(sizeof(rp[0]));

    return rp;
}

RtlPair*        RtlPair_new(ConstructTpl *ct, SymbolScope *scope)
{
    RtlPair* rp = vm_mallocz(sizeof(rp[0]));

    rp->section = ct;
    rp->scope = scope;

    return rp;
}

void            RtlPair_delete(RtlPair *rp)
{
    vm_free(rp);
}

struct slgh_macro* slgh_get_macro1(SleighCompile *slgh, const char *sym)
{
    struct slgh_macro *macro;
    int i;
    mlist_for_each(slgh->defines, macro, in_list, i) {
        if (!strcmp(macro->name, sym))
            return macro;
    }

    return NULL;
}

bool                slgh_get_macro(SleighCompile *s, char *name, char **val)
{
    struct slgh_macro *m = slgh_get_macro1(slgh, name);

    if (m) {
        if (val)
            *val = m->value;
        return true;
    }

    return false;
}

void                slgh_set_macro(SleighCompile *s, char *name, char *val)
{
    int len1, len2;
    struct slgh_macro *macro;
    if ((macro = slgh_get_macro1(slgh, name))) {
        printf("redefine macro %s, before defined in %s:%d, now in %s:%d\n", 
            name, basename(macro->filename), macro->lineno, basename(slgh_filename(s)), slgh_lineno(s));
        slgh_del_macro(s, name);
    }

    val = val ? val : "\0";

    len1 = strlen(name);
    len2 = strlen(val);

    macro = vm_mallocz(sizeof (macro[0]) + len1 + len2 + 1);
    strcpy(macro->name, name);
    macro->value = macro->name + len1 + 1;
    strcpy(macro->value, val);

    macro->filename = slgh_filename(s);
    macro->lineno = slgh_lineno(s);

    mlist_add(slgh->defines, macro, in_list);
}

bool                slgh_del_macro(SleighCompile *s, char *name)
{
    struct slgh_macro *macro = slgh_get_macro1(slgh, name);

    if (!macro) {
        vm_warn("undefined macro[%s] not exist", name);
        return false;
    }

    mlist_del(slgh->defines, macro, in_list);
    free(macro);

    return true;
}

SpaceQuality*   SpaceQuality_new(char *name)
{
    SpaceQuality *sq = vm_mallocz(sizeof (sq[0]) + strlen(name));

    strcpy(sq->name, name);
    sq->wordsize = 1;
    sq->type = RAMTYPE;

    return sq;
}

void            SpaceQuality_delete(SpaceQuality *sp)
{
    vm_free(sp);
}

SectionVector*  SectionVector_new(ConstructTpl *rtl, SymbolScope *scope)
{
    SectionVector *sv = vm_mallocz(sizeof(sv[0]));

    sv->nextindex = -1;
    sv->main.section = rtl;
    sv->main.scope = scope;

    return sv;
}

void            SectionVector_delete(SectionVector *sv)
{
    vm_free(sv);
}

void            SectionVector_append(SectionVector *sv, ConstructTpl *rtl, SymbolScope *scope)
{
    RtlPair *rp;

    while (sv->named.len <= sv->nextindex)
        dynarray_push(&sv->named, RtlPair_newV());

    rp = dynarray_back(&sv->named);
    rp->scope = scope;
    rp->section = rtl;

}

SleighCompile*  SleighCompile_new()
{
    SleighCompile *slgh = vm_mallocz(sizeof (slgh[0]));

    slgh->symtab = SymbolTable_new();

    slgh->pcode = PcodeCompile_new(slgh, SleighCompile_getUniqueAddr, SleighCompile_addSymbol);

    return slgh;
}

void            SleighCompile_delete(SleighCompile *slgh)
{
    struct slgh_macro *macro;

    while ((macro = slgh->defines.list)) {
        mlist_del(slgh->defines, macro, in_list);
        free(macro);
    }

    free(slgh);
}

SleighCompile*  SleighArch_register()
{
    return NULL;
}

void            SleighCompile_resetConstructor(SleighCompile *s)
{
}

void            SleighCompile_assignShortcut(SleighCompile *s, AddrSpace *spc)
{
}

void            SleighCompile_insertSpace(SleighCompile *s, AddrSpace *spc)
{
    bool nameTypeMismatch = false;
    bool duplicateName = false;
    bool duplicateId = false;

    switch (spc->type) {
    case IPTR_CONSTANT:
        if (strcmp(spc->name, "const"))
            nameTypeMismatch = true;
        if (spc->index != constant_space_index)
            vm_error("const space must be assigned index 0");
        s->constantspace = spc;
        break;

    case IPTR_INTERNAL:
        if (strcmp(spc->name, "unique"))
            nameTypeMismatch = true;
        if (s->uniqspace)
            duplicateName = true;

        s->uniqspace = spc;
        break;

    case IPTR_FSPEC:
        if (strcmp(spc->name, "fspec"))
            nameTypeMismatch = true;
        if (s->fspecspace)
            duplicateName = true;
        s->fspecspace = spc;
        break;

    case IPTR_JOIN:
        if (strcmp(spc->name, "join"))
            nameTypeMismatch = true;
        if (s->joinspace)
            duplicateName = true;
        s->joinspace = spc;
        break;

    case IPTR_IOP:
        if (strcmp(spc->name, "iop"))
            nameTypeMismatch = true;
        if (s->iopspace)
            duplicateName = true;
        s->iopspace = spc;
        break;

    case IPTR_SPACEBASE:
        if (!strcmp(spc->name, "stack")) {
            if (s->stackspace) {
                duplicateName = true;
            }

            s->stackspace = spc;
        }

    case IPTR_PROCESSOR:
        if (AddrSpace_isOverlay(spc)) {
            spc->baseSpace->flags |= overlaybase;
        }
        else if (spc->flags & is_otherspace) {
            if (spc->index != is_otherspace)
                vm_error("Other Space must be assigned index");
        }
        break;
    }

    if (s->baselist.len <= spc->index)
        dynarray_add(&s->baselist, NULL);

    duplicateId = s->baselist.ptab[spc->index] != 0;

    if (!nameTypeMismatch && !duplicateName && !duplicateId) {
        dynarray_add(&s->name2Space, spc);
    }

    if (nameTypeMismatch || duplicateId || duplicateName) {
        if (spc->refcount == 0)
            AddrSpace_delete(spc);
        spc = NULL;
    }

    if (nameTypeMismatch)
        vm_error("Space %s was initialized with wrong type", spc->name);
    if (duplicateName)
        vm_error("Space %s was initialized more than once", spc->name);
    if (duplicateId)
        vm_error("Space %s was assigned as id duplicating", ((AddrSpace *)s->name2Space.ptab[spc->index])->name);

    s->baselist.ptab[spc->index] = spc;
    spc->refcount += 1;
}

void            SleighCompile_predefinedSymbols(SleighCompile *s)
{
    SymbolTable_addScope(s->symtab);

    s->root = SubtableSymbol_new("instruction");
    SymbolTable_addSymbol(s->symtab, s->root);
    SleighCompile_insertSpace(s, ConstantSpace_new(s, "const", constant_space_index));

    SpaceSymbol *spacesym = SpaceSymbol_new(s->constantspace);
    SymbolTable_addSymbol(s->symtab, spacesym);

    OtherSpace *otherSpace = OtherSpace_new(s, "OTHER", other_space_index);
    SleighCompile_insertSpace(s, otherSpace);
    spacesym = SpaceSymbol_new(otherSpace);
    SymbolTable_addSymbol(s->symtab, spacesym);

    SleighCompile_insertSpace(s, UniqueSpace_new(s, "unique", s->baselist.len, 0));
    spacesym = SpaceSymbol_new(s->uniqspace);
    SymbolTable_addSymbol(s->symtab, spacesym);

    StartSymbol *startsym = StartSymbol_new("inst_start", s->constantspace);
    SymbolTable_addSymbol(s->symtab, startsym);

    EndSymbol *endsym = EndSymbol_new("inst_next", s->constantspace);
    SymbolTable_addSymbol(s->symtab, endsym);

    EpsilonSymbol *epsilon = EpsilonSymbol_new("epsilon", s->constantspace);
    SymbolTable_addSymbol(s->symtab, epsilon);

    PcodeCompile_setConstantSpace(s->pcode, s->constantspace);
    PcodeCompile_setUniqueSpace(s->pcode, s->uniqspace);
}


void            SleighCompile_setEndian(SleighCompile *s, int e)
{
    s->bigendian = e;
    SleighCompile_predefinedSymbols(s);
}

void            SleighCompile_setAlignment(SleighCompile *s, int e)
{
}

TokenSymbol*    SleighCompile_defineToken(SleighCompile *s, const char *name, intb sz)
{
    return NULL;
}

void            SleighCompile_addTokenField(SleighCompile *s, TokenSymbol *sym, FieldQuality *qual)
{
}

bool            SleighCompile_addContextField(SleighCompile *s, VarnodeSymbol *sym, FieldQuality *qual)
{
    if (s->contextlock)
        return false;

    dynarray_add(&s->contexttable, FieldContext_new(sym, qual));
    return TRUE;
}

void            SleighCompile_setDefaultCodeSpace(SleighCompile *s, int index)
{
    if (s->defaultcodespace)
        vm_error("Default space set multi times");
    if (index >= s->baselist.len || !s->baselist.ptab[index])
        vm_error("bad index for default space");

    s->defaultcodespace = s->baselist.ptab[index];
    s->defaultdataspace = s->defaultcodespace;
}

void            SleighCompile_addSymbol(SleighCompile *s, SleighSymbol *sym)
{
    struct slgh_preproc *proc = dynarray_back(&s->preproc);
    sym->filename = basename(proc->fullpath.data);
    sym->lineno = proc->lineno;

    SymbolTable_addSymbol(s->symtab, sym);
}

void            SleighCompile_newSpace(SleighCompile *s, SpaceQuality *quad)
{
    struct slgh_preproc *proc = dynarray_back(&s->preproc);

    if (quad->size == 0) {
        vm_error("%s:%d Space definition %s missing size attribute", basename(proc->fullpath.data), proc->lineno, quad->name);
    }

    int delay = (quad->type == REGISTERTYPE) ? 0 : 1;
    AddrSpace *spc = AddrSpace_new8(s, IPTR_PROCESSOR, quad->name, quad->size, quad->wordsize, s->baselist.len, hasphysical, delay);
    SleighCompile_insertSpace(s, spc);
    if (quad->isdefault) {
        if (s->defaultcodespace)
            vm_error("%s:%d Multiple default spaces -- %s, %s", s->defaultcodespace->name, quad->name);
        else {
            SleighCompile_setDefaultCodeSpace(s, spc->index);
            PcodeCompile_setDefaultSpace(s->pcode, spc);
        }
    }

    SleighCompile_addSymbol(s, SpaceSymbol_new(spc));
}

void            SleighCompile_defineVarnodes(SleighCompile *s, SpaceSymbol *sym, uintb off, uintb size, struct dynarray *names)
{
    CString *cstr;
    AddrSpace *spc = sym->space.space;
    int i;

    for (i = 0; i < names->len; i++) {
        cstr = names->ptab[i];
        SleighCompile_addSymbol(s, VarnodeSymbol_new(cstr->data, spc, off, (int)size));
        off += size;
    }
}

void            SleighCompile_defineBitrange(SleighCompile *s, const char *name, VarnodeSymbol *sym, uint32_t bitoffset, uint32_t numb)
{

}

void            SleighCompile_defineOperand(SleighCompile *s, OperandSymbol *sym, PatternExpression *exp)
{

}

void            SleighCompile_addUserOp(SleighCompile *s, struct dynarray *names)
{

}

void            SleighCompile_attachValues(SleighCompile *s, struct dynarray *symlist, struct dynarray *numlist)
{

}

void            SleighCompile_attachNames(SleighCompile *s, struct dynarray *symlist, struct dynarray *names)
{

}

void            SleighCompile_attachVarnodes(SleighCompile *s, struct dynarray *symlist, struct dynarray *varlist)
{

}

void            SleighCompile_buildMacro(SleighCompile *s, MacroSymbol *sym, ConstructTpl *rtl)
{
    SleighCompile_checkSymbols(s, s->symtab->curscope);

    if (!SleighCompile_expandMacros(s, rtl, &s->macrotable))
        vm_error("%s:%d Could not expand submacro in defintion of macro %s", slgh_filename(s), slgh_lineno(s), sym->name);

    PcodeCompile_propagateSize(s->pcode, rtl);
    sym->macro.construct = rtl;
    SymbolTable_popScope(s->symtab);
    dynarray_add(&s->macrotable, rtl);
}

void            SleighCompile_pushWith(SleighCompile *s, SubtableSymbol *sym, PatternEquation *pateq, struct dynarray *contvec)
{

}

void            SleighCompile_popWith(SleighCompile *s)
{

}

SubtableSymbol* SleighCompile_newTable(SleighCompile *s, const char *name)
{
    SubtableSymbol *sym = SubtableSymbol_new(name);

    SleighCompile_addSymbol(s, sym);
    dynarray_add(&s->tables, sym);
    return sym;
}

MacroSymbol*    SleighCompile_createMacro(SleighCompile *s, const char *name, struct dynarray *params)
{
    int i;
    s->curct = NULL;
    s->curmacro = MacroSymbol_new(name, s->macrotable.len);

    SleighCompile_addSymbol(s, s->curmacro);

    SymbolTable_addScope(s->symtab);
    PcodeCompile_resetLabelCount(s->pcode);

    for (i = 0; i < params->len; i++) {
        CString *param = params->ptab[i];
        OperandSymbol *oper = OperandSymbol_new(param->data, i, NULL);
        SleighCompile_addSymbol(s, oper);
        MacroSymbol_addOperand(s->curmacro, oper);
    }

    return s->curmacro;
}

SectionVector*  SleighCompile_standaloneSection(SleighCompile *s, ConstructTpl *main)
{
    SectionVector *res = SectionVector_new(main, SymbolTable_getCurrentScope(s->symtab));
    return res;
}

SectionVector*  SleighCompile_firstNamedSection(SleighCompile *s, ConstructTpl *main, SectionSymbol *sym)
{
    return NULL;
}

SectionVector*  SleighCompile_nextNamedSection(SleighCompile *s, SectionVector *vec, ConstructTpl *section, SectionSymbol *sym)
{
    return NULL;
}

#define sym_container_of(_node)    (SleighSymbol *)((char *)(_node) - offsetof(SleighSymbol, in_scope))

/* 一般的编译器把错误信息统一收集起来，最后在结束时，统一打印出来，方便写程序人统一修改
 我这里为了方便，有了错误，直接打印 */
void            SleighCompile_checkSymbols(SleighCompile *s, SymbolScope *scope)
{
    struct rb_node *node;

    for (node = rb_first(&scope->tree); node; node = rb_next(node)) {
        SleighSymbol *sym = sym_container_of(node);
        if (sym->type != label_symbol) continue;
        if (!sym->label.refcount)
            vm_error("%s:%d Label %s was placed but not used", slgh_filename(s), slgh_lineno(s), sym->name);
        else if (sym->label.isplaced)
            vm_error("%s:%d Label %s was referenced but never placed", slgh_filename(s), slgh_lineno(s), sym->name);
    }
}

bool MacroBuilder_transferOp(MacroBuilder *m, OpTpl *op, struct dynarray *params)
{
    /* 这个好像是把macro inline时*/
    VarnodeTpl *outvn = OpTpl_getOut(op);
    int handleIndex = 0;
    int i, plus;
    bool hasrealsize = false;
    uintb realsize = 0;

    if (outvn) {
        plus = VarnodeTpl_transfer(outvn, params);
        if (plus > 0)
            vm_error("Cannot currently assign to bitrange of macro parameter that is a temporary");
    }

    for (i = 0; i < op->input.len; i++) {
        VarnodeTpl *vn = OpTpl_getIn(op, i);
        if (vn->offset->type == handle) {
            handleIndex = vn->offset->value.handle_index;
            hasrealsize = (vn->size->type == real);
            realsize = vn->size->value_real;
        }

        plus = VarnodeTpl_transfer(vn, params);
        if (plus > 0) {
            if (hasrealsize)
                vm_error("Problem with bit range operator in macro");
        }

        uintb newtemp = SleighCompile_getUniqueAddr(slgh);
        OpTpl *subpieceop = OpTpl_new(CPUI_SUBPIECE);
        VarnodeTpl *newvn = VarnodeTpl_new(ConstTpl_newA(slgh->uniqspace), ConstTpl_new2(real, newtemp),
                                        ConstTpl_new2(real, realsize));
        OpTpl_setOutput(subpieceop, newvn);
        HandleTpl *hand = params->ptab[handleIndex];
        VarnodeTpl *origvn = VarnodeTpl_new3(hand->space, hand->ptroffset, hand->size);
        OpTpl_addInput(subpieceop, origvn);
        VarnodeTpl *plusvn = VarnodeTpl_new3(ConstTpl_newA(slgh->constantspace), ConstTpl_new2(real, plus),
                                            ConstTpl_new2(real, 4));
        OpTpl_addInput(subpieceop, plusvn);
        dynarray_add(m->outvec, subpieceop);

        VarnodeTpl_delete(vn);
        op->input.ptab[i] = newvn;
    }
    dynarray_add(m->outvec, op);
    return true;
}

void            MacroBuilder_dump(MacroBuilder *m, OpTpl *o)
{
    OpTpl *clone;
    VarnodeTpl *v_clone, *vn;
    int i;

    clone = OpTpl_new1(o->opc);
    vn = o->output;
    if (vn) {
        v_clone = VarnodeTpl_clone(vn);
        OpTpl_setOutput(clone, v_clone);
    }

    for (i = 0; i < o->input.len; i++) {
        vn = OpTpl_getIn(o, i);
        v_clone = VarnodeTpl_clone(vn);
        if (VarnodeTpl_isRelative(v_clone)) {
            uintb val = v_clone->offset->value_real + m->pb->labelbase;
            v_clone->offset->value_real = val;
        }
        OpTpl_addInput(clone, v_clone);
    }

    if (!MacroBuilder_transferOp(m, clone, &m->params))
        OpTpl_delete(clone);
}

void            MacroBuilder_appendBuild(MacroBuilder *m, OpTpl *bld, int secnum)
{
    MacroBuilder_dump(m, bld);
}

void            MacroBuilder_delaySlot(MacroBuilder *m, OpTpl *bld)
{
    MacroBuilder_dump(m, bld);
}

void            MacroBuilder_setLabel(MacroBuilder *m, OpTpl *op)
{
    // 当我们调用Macro的时候，实际上是一个inline的过程，这个inline以后
    // 跳转的Label实际位置都会发生变化，比如原代码
    // 
    // macroA 123
    // label1: 
    //       goto label1
    // 
    // 假如macroA 123 这个宏内部也有label，当他被展开以后，假如有2个label，那么
    // 原先的label1的位置会变成label3

    OpTpl *clone;
    VarnodeTpl *v_clone;

    clone = OpTpl_new(op->opc);
    v_clone = VarnodeTpl_clone(OpTpl_getIn(op, 0));
    uintb val = v_clone->offset->value_real + m->pb->labelbase;
    VarnodeTpl_setOffset(v_clone, val);
    OpTpl_addInput(clone, v_clone);
    dynarray_add(m->outvec, clone);
}

bool            SleighCompile_expandMacros(SleighCompile *s, ConstructTpl *ctpl, struct dynarray *macrotable)
{
    struct dynarray newvec = { 0 };
    OpTpl *op;
    int i;

    for (i = 0; i < ctpl->vec.len; i++) {
        op = ctpl->vec.ptab[i];
        if (op->opc == MACROBUILD) {
            MacroBuilder *m = MacroBuilder_new(s, &newvec, ctpl->numlabels);

            int index = (int)(OpTpl_getIn(op, 0)->offset->value_real);
            if (index >= macrotable->len)
                return false;

            MacroBuilder_setMacroOp(m, op);
        }
    }

    return true;
}

/* 这明明是find_offset，为什么会是find_size?*/
static  VarnodeTpl *find_size(ConstTpl *offset, ConstructTpl *ct)
{
    struct dynarray *ops = &ct->vec;
    VarnodeTpl *vn;
    OpTpl *op;
    int i, j;

    for (i = 0; i < ops->len; i++) {
        op = ops->ptab[i];
        vn = op->output;
        if (vn && VarnodeTpl_isLocalTemp(vn)) {
            if (ConstTpl_isEqual(vn->offset, offset))
                return vn;
        }

        for (j = 0; j < op->input.len; j++) {
            vn = op->input.ptab[j];
            if (VarnodeTpl_isLocalTemp(vn) && ConstTpl_isEqual(vn->offset, offset))
                return vn;
        }
    }

    return NULL;
}

static bool force_exportsize(ConstructTpl *ct)
{
    HandleTpl *result = ct->result;
    if (!result) return true;

    VarnodeTpl *vt;

    if (ConstTpl_isUniqueSpace(result->ptrspace) && ConstTpl_isZero(result->ptrsize)) {
        vt =find_size(result->ptroffset, ct);
        if (!vt) return false;
        result->ptrsize = vt->size;
    }
    else if (ConstTpl_isUniqueSpace(result->space) && ConstTpl_isZero(result->size)) {
        vt = find_size(result->ptroffset, ct);
        if (!vt) return false;
        result->size = vt->size;
    }

    return true;
}

/* 当一个Constructor结束时，做最后的收尾工作，符号检查，符号扩展，链接pcode */
bool            SleighCompile_finalizeSections(SleighCompile *s, Constructor *big, SectionVector *vec)
{
    RtlPair *cur = &vec->main;
    int i = -1;
    int max = vec->named.len;
    struct dynarray check = { 0 };
    while (1) {
        SleighCompile_checkSymbols(s, cur->scope);

        if (!SleighCompile_expandMacros(s, cur->section, &s->macrotable))
            vm_error("    Maion Section: could not expand macros");

        dynarray_reset(&check);
        Constructor_markSubtableOperands(big, &check);
        int res = ConstructTpl_fillinBuild(cur->section, &check, s->constantspace);
        /* BUILD 指令貌似一个semain section内只能有一条，因为BUILD会干扰 pcode的解析顺序
        它一定是第一个解析的，只有一个第一 */
        if (res == 1)
            vm_error("    Main Section: Duplicate Build statement");
        /*这里可能是因为当执行BUILD指令时，后面只能接 subtable 符号，它必须是可以展开的  */
        else if (res == 2)
            vm_error("    Main Section: Unnecessary BUILD statement");

        PcodeCompile_propagateSize(s->pcode, cur->section);

        if (i < 0) {
            if (cur->section->result) {
                if (big->parent == s->root)
                    vm_error("    Main Section Cannot have export statment in root constructor");
                else if (!force_exportsize(cur->section))
                    vm_error("    Main Section Size of export is unknown");
            }
        }

        if (cur->section->delayslot) {
            if (s->root != big->parent) {
                vm_error(" Delay slot used in non-root constructor");
            }

            if (cur->section->delayslot > s->maxdelayslotbytes)
                s->maxdelayslotbytes = cur->section->delayslot;
        }

        do {
            i += 1;
            if (i >= max)
                break;
            cur = vec->named.ptab[i];
        } while (!cur->section);

        if (i >= max)
            break;
    } 

    return true;
}

void            SleighCompile_buildConstructor(SleighCompile *s, Constructor *big, PatternEquation *pateq, struct dynarray *contvec, SectionVector *vec)
{
    bool noerros = true;
    PatternEquation *pateq1;
    struct dynarray *contvec1;
    int i;
    if (vec) {
        noerros = SleighCompile_finalizeSections(s, big, vec);
        if (noerros) {
            Constructor_setMainSection(big, vec->main.section);
            int max = vec->named.len;
            for (i = 0; i < max; ++i) {
                ConstructTpl *section = vec->named.ptab[i];
                if (section)
                    Constructor_setNamedSection(big, section, i);
            }
        }
    }
    
    if (noerros) {
        pateq1 = WithBlock_collectAndPrependPattern(&s->withstack, pateq);
        contvec1 = WithBlock_collectAndPrependContext(&s->withstack, contvec);

        Constructor_addEquation(big, pateq1);
        Constructor_removeTrailingSpace(big);
        if (contvec1) {
            big->context = contvec1;
        }
    }

    SymbolTable_popScope(s->symtab);
}

void            SleighCompile_newOperand(SleighCompile *s, Constructor *c, const char *name)
{

}

WithBlock*      WithBlock_new()
{
    WithBlock*  wb = vm_mallocz(sizeof (wb[0]));

    return  wb;
}

void            WithBlock_delete(WithBlock *w)
{
    vm_free(w);
}
PatternEquation*    WithBlock_collectAndPrependPattern(struct dynarray *stack, PatternEquation *pateq)
{
    int i;

    for (i = 0; i < stack->len; i++) {
        PatternEquation *witheq = stack->ptab[i];
        if (witheq)
            pateq = EquationAnd_new(witheq, pateq);
    }

    return pateq;
}

struct dynarray*    WithBlock_collectAndPrependContext(struct dynarray *stack, struct dynarray *contvec)
{
    struct dynarray *res = dynarray_new(NULL, NULL);
    WithBlock *w;
    struct dynarray *changelist;
    int i, j;

    for (i = 0; i < stack->len; i++) {
        w = stack->ptab[i];
        changelist = &w->contvec;
        if (changelist->len == 0) continue;
        if (!res) 
            res = dynarray_new(NULL, NULL);

        for (j = 0; j < changelist->len; j++) {
            dynarray_add(res, ContextChange_clone(changelist->ptab[j]));
        }
    }

    if (contvec) {
        if (contvec->len) {
            if (!res) res = dynarray_new(NULL, NULL);

            for (i = 0; i < contvec->len; i++) {
                dynarray_add(res, contvec->ptab[i]);
            }
        }
    }

    return res;
}

SubtableSymbol*     WithBlock_getCurrentSubtable(SleighCompile *s)
{
    int i;
    for (i = 0; i < s->withstack.len; i++) {
        WithBlock *w = s->withstack.ptab[i];
        if (w->ss)
            return w->ss;
    }

    return NULL;
}

MacroBuilder*       MacroBuilder_new(SleighCompile *s, struct dynarray *ovec, int labelcount)
{
    MacroBuilder *m = vm_mallocz(sizeof(m[0]));

    m->slgh = s;
    m->pb = PcodeBuilder_new(m);
    m->pb->dump = MacroBuilder_dump;
    m->pb->appendBuild = MacroBuilder_appendBuild;
    m->pb->appendCrossBuild = MacroBuilder_appendBuild;
    m->pb->setLabel = MacroBuilder_setLabel;
    m->pb->delaySlot = MacroBuilder_delaySlot;

    return m;
}

void                MacroBuilder_delete(MacroBuilder *m)
{
    PcodeBuilder_delete(m->pb);
    vm_free(m);
}

void                MacroBuilder_free(MacroBuilder *m)
{
    int i;

    for (i = 0; i < m->params.len; i++) {
        HandleTpl_delete(m->params.ptab[i]);
    }

    dynarray_reset(&m->params);
}

void                MacroBuilder_setMacroOp(MacroBuilder *m, OpTpl *op)
{
    VarnodeTpl *vn;
    HandleTpl *hand;
    int i;

    for (i = 1; i < op->input.len; i++) {
        vn = OpTpl_getIn(op, i);
        hand = HandleTpl_newV(vn);
        dynarray_add(&m->params, hand);
    }
}

Constructor*        SleighCompile_createConstructor(SleighCompile *s, SubtableSymbol *sym)
{
    if (!sym)
        sym = WithBlock_getCurrentSubtable(s);
    if (!sym)
        sym = s->root;

    s->curmacro = NULL;
    s->curct = Constructor_newS(sym);
    s->curct->lineno = slgh_lineno(s);
    // ctorLocationMap[curct] = *getCurrentLocation();
    SubtableSymbol_addConstructor(sym, s->curct);
    SymbolTable_addScope(s->symtab);
    s->pcode->local_labelcount = 0;

    return s->curct;
}

void        SleighCompile_resetConstructors(SleighCompile *s)
{
    SymbolTable_setCurrentScope(s->symtab, SymbolTable_getGlobalScope(s->symtab));
}

PatternEquation*    SleighCompile_constrainOperand(SleighCompile *s, OperandSymbol *sym, PatternExpression *patexp)
{
    return NULL;
}

void                SleighCompile_selfDefine(SleighCompile *s, OperandSymbol *sym)
{

}

PatternEquation*    SleighCompile_defineInvisibleOperand(SleighCompile *s, TripleSymbol *sym)
{
    int index = s->curct->operands.len;
    OperandSymbol *opsym = OperandSymbol_new(sym->name, index, s->curct);

    SleighCompile_addSymbol(s, opsym);
    Constructor_addInvisibleOperand(s->curct, opsym);
    OperandEquation *res = OperandEquation_new(opsym->operand.hand);
    int tp = sym->type;

    if ((tp == value_symbol) || (tp == context_symbol)) {
        OperandSymbol_defineOperand(opsym, SleighSymbol_getPatternExpression(sym));
    }
    else {
        OperandSymbol_defineOperandS(opsym, sym);
    }

    return res;
}

bool                SleighCompile_contextMod(SleighCompile *s, struct dynarray *vec, ContextSymbol *sym, PatternExpression *pe)
{
    return true;
}

void                SleighCompile_contextSet(SleighCompile *s, struct dynarray *vec, TripleSymbol *sym, ContextSymbol *ctxSym)
{
}

SectionSymbol*      SleighCompile_newSectionSymbol(SleighCompile *s, char *name)
{
    return NULL;
}

void                SleighCompile_recordNop(SleighCompile *s)
{

}

void                SleighCompile_addRegister(SleighCompile *s, const char *name, AddrSpace *base, uintb offset, int size)
{

}

const char*         SleighCompile_getRegisterName(SleighCompile *s, AddrSpace *base, uintb off, int size)
{
    return NULL;
}

void                SleighCompile_getAllRegister(SleighCompile *s, struct dynarray *d)
{
}

void                SleighCompile_getUserOpNames(SleighCompile *s, struct dynarray *d)
{

}

uintb               SleighCompile_getUniqueAddr(SleighCompile *s)
{
    uintb base = s->unique_base;

    s->unique_base += 16;

    return base;
}

ConstructTpl*       SleighCompile_setResultVarnode(SleighCompile *s, ConstructTpl *ct, VarnodeTpl *vn)
{
    HandleTpl *res = HandleTpl_newV(vn);

    ct->result = res;

    return ct;
}

ConstructTpl*       SleighCompile_setResultStarVarnode(SleighCompile *s, ConstructTpl *ct, StarQuality *star, VarnodeTpl *vn)
{
    return NULL;
}

struct dynarray*    SleighCompile_createCrossBuild(SleighCompile *s, VarnodeTpl *addr, SectionSymbol *sym)
{
    return NULL;
}

void slgh_cmp_macro_params(SleighCompile *s, MacroSymbol *sym, struct dynarray *params)
{
    int i;
    ExpTree *e;
    VarnodeTpl *vn;

    for (i = 0; i < params->len; i++) {
        e = params->ptab[i];
        vn = e->outvn;
        if (vn == 0) continue;

        int hand = vn->offset->value.handle_index;

        OperandSymbol *macrop = MacroSymbol_getOperand(sym, hand);
        OperandSymbol *parentop;
        if (s->curct == NULL)
            parentop = MacroSymbol_getOperand(s->curmacro, hand);
        else
            parentop = s->curct->operands.ptab[hand];

        if (OperandSymbol_isCodeAddress(macrop))
            parentop->operand.flags |= CODE_ADDRESS;
    }
}

struct dynarray*    SleighCompile_createMacroUse(SleighCompile *s, MacroSymbol *sym, struct dynarray *params)
{
    struct slgh_preproc *last = dynarray_back(&s->preproc);
    if (sym->macro.operands.len != params->len) {
        vm_error("%s:%d Macro(%s) params number mismatch", last->fullpath.data, last->lineno, sym->name);
    }

    slgh_cmp_macro_params(s, sym, params);
    OpTpl *op = OpTpl_new(MACROBUILD);
    VarnodeTpl *idvn = VarnodeTpl_new3(ConstTpl_newA(s->constantspace),
                                        ConstTpl_new2(real, sym->macro.index),
                                        ConstTpl_new2(real, 4));
    OpTpl_addInput(op, idvn);

    return ExpTree_appendParams(op, params);
}

bool                SleighCompile_getPreprocValue(SleighCompile *s, char *name, char **value)
{
    return true;
}

void                SleighCompile_setPreprocValue(SleighCompile *s, char *name, char *value)
{
}

bool                SleighCompile_undefinePreprocValue(SleighCompile *s, char *name)
{
    return false;
}

void                SleighCompile_parseFromNewFile(SleighCompile *s, const char *fname)
{
    struct slgh_preproc *proc = vm_mallocz(sizeof (proc[0]));
    char *base;

    proc->lineno = 1;

    if (slgh->preproc.len && !IS_ABSPATH(fname)) {
        struct slgh_preproc *last = dynarray_back(&slgh->preproc);
        base = basename(last->fullpath.data);
        cstr_cat(&proc->fullpath, last->fullpath.data, base - last->fullpath.data);
    }

    cstr_cat(&proc->fullpath, fname, strlen(fname));

    dynarray_add(&s->preproc, proc);
}

void                SleighCompile_parsePreprocMacro(SleighCompile *s)
{
    struct slgh_preproc *last = dynarray_back(&s->preproc);
    struct slgh_preproc *proc = vm_mallocz(sizeof (proc[0]));

    cstr_cat(&proc->fullpath, last->fullpath.data, last->fullpath.size);
    cstr_cat(&proc->fullpath, ":macro", 0);

    proc->lineno = last->lineno;
    dynarray_add(&s->preproc, proc);
}

void                SleighCompile_parseFileFinished(SleighCompile *s)
{
    vm_free(dynarray_back(&s->preproc));
    dynarray_pop(&s->preproc);
}

char*               SleighCompile_grabCurrentFilePath(SleighCompile *s)
{
    if (!s->preproc.len) return "";
    char *s1 = ((struct slgh_preproc *)dynarray_back(&s->preproc))->fullpath.data;

    return s1;
}

SleighSymbol*       SleighCompile_findSymbol(SleighCompile *s, char *name)
{
    SleighSymbol *sym = SymbolTable_findSymbol(s->symtab, name);

    if (sym && sym->filename)
        printf("%s:%d sym->name = %s\n", basename(sym->filename), sym->lineno, sym->name);
    return sym;
}

void                SleighCompile_nextLine(SleighCompile *s)
{
    struct slgh_preproc *proc = dynarray_back(&s->preproc);

    proc->lineno++;

    printf("*****scan %s line %d\n", basename(proc->fullpath.data), proc->lineno);
}

int                 SleighCompile_calcContextVarLayout(SleighCompile *s, int start, int sz, int numbits)
{
    VarnodeSymbol *sym = ((FieldContext *)s->contexttable.ptab[start])->sym;
    FieldQuality *qual;
    int i, j;
    int maxbits;

    if (sym->varnode.fix.size % 4) {
        vm_error("Invalid size of context register %s:%d", sym->name, sym->varnode.fix.size);
    }

    maxbits = sym->varnode.fix.size * 8 - 1;
    i = 0;
    while(i < sz) {
        qual = ((FieldContext *)s->contexttable.ptab[i])->qual;
        int min = qual->low;
        int max = qual->high;
        if ((max - min) > (8 * sizeof(uintm))) 
            vm_error("Sizeof of bitfield %s larger than %d bits", qual->name, 8 * sizeof(uintm));
        if (max > maxbits)
            vm_error("Scope of bitfield %s extends beyond the size of context register", qual->name);

        j = i + 1;
        while (j < sz) {
            qual = ((FieldContext *)s->contexttable.ptab[j])->qual;
            if (qual->low <= max) {
                if (qual->high > max)
                    max = qual->high;
            }
            else
                break;
            j = j + 1;
        }

        int alloc = max - min + 1;
        int startword = numbits / (8 * sizeof(uintm));
        int endword = (numbits + alloc - 1) / (8 * sizeof(uintm));
        if (startword != endword)
            numbits = endword * 8 * sizeof(uintm);

        int low = numbits;
        numbits += alloc;

        for (; i < j; i++) {
            qual = ((FieldContext *)s->contexttable.ptab[i])->qual;
            int l = qual->low - min + low;
            int h = numbits - 1 - (max - qual->high);
            ContextField *field = ContextField_new(qual->signext, l, h);
            SleighCompile_addSymbol(s, ContextSymbol_new(qual->name, field, sym, qual->low, qual->high, qual->flow));
        }
    }

    sym->varnode.context_bits = 1;
    return numbits;
}

void                SleighCompile_calcContextLayout(SleighCompile *s)
{
    if (s->contextlock) return;

    int context_offset = 0;
    int begin, sz, i;

    qsort(s->contexttable.ptab, s->contexttable.len, sizeof (FieldContext *), FieldContext_cmp);

    /* 计算context寄存器的时候，判断地址是否有冲突，这个分析不是在define了context寄存器后面的域后立即进行的
    是碰到第一个 : 时开始计算的，他是动态的 */
    begin = 0;
    while (begin < s->contexttable.len) {
        sz = 1;
        while (((begin + sz) < s->contexttable.len) && ((FieldContext *)s->contexttable.ptab[begin + sz])->sym == ((FieldContext *)s->contexttable.ptab[begin])->sym)
            sz += 1;

        context_offset = SleighCompile_calcContextVarLayout(s, begin, sz, context_offset);
        begin += sz;
    }

    for (i = 0; i < s->contexttable.len; i++) {
        FieldQuality_delete(s->contexttable.ptab[i]);
    }
    dynarray_reset(&s->contexttable);
}

FieldQuality*   FieldQuality_new(const char *name, uintb l, uintb h)
{
    FieldQuality *f = vm_mallocz(sizeof (f[0]) + strlen(name));

    f->low = (int)l;
    f->high = (int)h;
    f->signext = true;
    f->flow = true;
    f->hex = true;
    strcpy(f->name, name);

    return f;
}

void            FieldQuality_delete(FieldQuality *f)
{
    vm_free(f);
}

FieldContext*           FieldContext_new(VarnodeSymbol *v, FieldQuality *qual)
{
    FieldContext*   fc = vm_mallocz(sizeof(fc[0]));

    fc->qual = qual;
    fc->sym = v;

    return fc;
}

void                    FieldContext_delete(FieldContext *fc)
{
    vm_free(fc);
}

int                     FieldContext_cmp(void const *a, void const *b)
{
    FieldContext *lhs = *(FieldContext **)a;
    FieldContext *rhs = *(FieldContext **)b;
    int ret = strcmp(lhs->sym->name, rhs->sym->name);
    if (ret)    return ret;

    return (int)(lhs->qual->low - rhs->qual->low);
}

SectionVector*  SleighCompile_finalNamedSection(SleighCompile *s, SectionVector *vec, ConstructTpl *section)
{
    SectionVector_append(vec, section, s->symtab->curscope);
    SymbolTable_popScope(s->symtab);
    return vec;
}

void    SleighCompile_checkNops(SleighCompile *s)
{
}

void    SleighCompile_checkConsistency(SleighCompile *s)
{
}

void    SleighCompile_process(SleighCompile *s)
{
    SleighCompile_checkNops(s);
    if (!s->defaultcodespace)
        vm_error("No default code space");
    if (s->errors > 0) return;
}

static char help[] = {
    "USAGE: sleigh [-x] [-dNAME=VALUE] inputfile [outputfile]\n" 
    "   -a              scan for all slaspec files recursively where inputfile is a directory\n" 
    "   -x              turns on parser debugging\n" 
    "   -u              print warnings for unnecessary pcode instructions\n"
    "   -l              report pattern conflicts\n" 
    "   -n              print warnings for all NOP constructors\n" 
    "   -t              print warnings for dead temporaries\n" 
    "   -e              enforce use of 'local' keyword for temporaries\n" 
    "   -c              print warnings for all constructors with colliding operands\n" 
    "   -DNAME=VALUE    defines a preprocessor macro NAME with value VALUE\n" 
};

int                 slgh_run(SleighCompile *s, const char *filein, const char *fileout)
{
    slgh = s;

    SleighCompile_parseFromNewFile(s, filein);

    yyin = fopen(filein, "r");
    if (NULL == yyin) 
        vm_error("unable to open specfile: %s", filein);

    int parseres = yyparse();
    fclose(yyin);

    FILE *out = fopen(fileout, "w");
    if (!out)
        vm_error("fopen(%s) failure", fileout);
    slgh->saveXml(s, out);
    fclose(out);

    yylex_destroy();

    return 0;
}

int             slgh_run_xml(SleighCompile *s, const char *filein)
{
    return 0;
}

#if defined(SLEIGH_EXE)
int                 main(int argc, char **argv)
{
    int i;
    bool compileAll = false;
    SleighCompile *slgh = SleighCompile_new(0);
    char *p, *newstr, *sym, *val;

    if (argc < 2) {
        puts(help);
        exit(2);
    }

    for (i = 1; i < argc; i++) {
        p = argv[i];
        if (*p != '-') break;
        else if (p[1] == 'a')
            compileAll = true;
        else if (p[1] == 'D') {
            newstr = strdup(p);
            val = strchr(newstr, '=');
            sym = p + 2;
            if (val) {
                *val = 0;
                val++;
            }
            slgh_define_macro(slgh, sym, val);

            free(p);
        }
        else if (p[1] == 'u')
            slgh->warnunnecessarypcode = true;
        else if (p[1] == 'l')
            slgh->lenientconfliciterros = true;
        else if (p[1] == 'c')
            slgh->warnallocalcollisions = true;
        else if (p[1] == 'n')
            slgh->warnallnops = true;
        else if (p[1] == 't')
            slgh->warndeadtemps = true;
        else if (p[1] == 'e')
            PcodeCompile_setEnforceLocalKey(slgh->pcode, true);
        else if (p[1] == 'x')
            yydebug = 1;
        else {
            printf("Unknown option %s\n", p);
            return;
        }
    }

    char *filein = argv[i];
    CString cs = {0};

    if (compileAll) {
    }
    else {
        char *ext = strrchr(filein, '.');
        cstr_cat(&cs, filein, ext - filein);
        cstr_cat(&cs, ".sla", 4);
        if (!strcmp(ext, ".xml")) {
        }
        else if (!strcmp(ext, ".slaspec")) {
            return slgh_run(slgh, filein, cs.data);
        }
        else {
            printf("Unkown file %s\n", filein);
            return -1;
        }
    }
    cstr_free(&cs);

    SleighCompile_delete(slgh);

    return 0;
}
#endif