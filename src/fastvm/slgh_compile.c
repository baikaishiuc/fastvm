
#include "slgh_compile.h"

SleighCompile *slgh = NULL;

SpaceQuality*   SpaceQuality_new(char *name)
{
    return NULL;
}

SleighCompile*  SleighCompile_new(SleighArchType arch)
{
    return NULL;
}

void            SleighCompile_delete()
{
}

SleighCompile*  SleighArch_register()
{
    return NULL;
}

void            SleighCompile_resetConstructor(SleighCompile *s)
{
}

void            SleighCompile_setEndian(SleighCompile *s, int e)
{
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
    return TRUE;
}

void            SleighCompile_newSpace(SleighCompile *s, SpaceQuality *quad)
{
}

void            SleighCompile_defineVarnodes(SleighCompile *s, SpaceSymbol *sym, uintb off, uintb size, struct dynarray *names)
{
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

void            SleighCompile_buildMacro(SleighCompile *s, MacroSymbol *sym, ConstructTpl *tpl)
{

}

bool            SleighCompile_parsePreprocMacro(SleighCompile *s)
{
    return true;
}

void            SleighCompile_pushWith(SleighCompile *s, SubtableSymbol *sym, PatternEquation *pateq, struct dynarray *contvec)
{

}

void            SleighCompile_popWith(SleighCompile *s)
{

}

SubtableSymbol* SleighCompile_newTable(SleighCompile *s, const char *name)
{
    return NULL;
}

MacroSymbol*    SleighCompile_createMacro(SleighCompile *s, const char *name, struct dynarray *param)
{
    return NULL;
}

SectionVector*  SleighCompile_standaloneSection(SleighCompile *s, ConstructTpl *main)
{
    return NULL;
}

SectionVector*  SleighCompile_firstNamedSection(SleighCompile *s, ConstructTpl *main, SectionSymbol *sym)
{
    return NULL;
}

SectionVector*  SleighCompile_nextNamedSection(SleighCompile *s, SectionVector *vec, ConstructTpl *section, SectionSymbol *sym)
{
    return NULL;
}

void            SleighCompile_buildConstructor(SleighCompile *s, Constructor *big, PatternEquation *pateq, struct dynarray *contvec, SectionVector *vec)
{

}

void            SleighCompile_newOperand(SleighCompile *s, Constructor *c, const char *name)
{

}

Constructor*        SleighCompile_createConstructor(SleighCompile *s, SubtableSymbol *sym)
{
    return NULL;
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
    return NULL;
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

ConstructTpl*       SleighCompile_setResultVarnode(SleighCompile *s, ConstructTpl *ct, VarnodeTpl *vn)
{
    return NULL;
}

ConstructTpl*       SleighCompile_setResultStarVarnode(SleighCompile *s, ConstructTpl *ct, StarQuality *star, VarnodeTpl *vn)
{
    return NULL;
}

struct dynarray*    SleighCompile_createCrossBuild(SleighCompile *s, VarnodeTpl *addr, SectionSymbol *sym)
{
    return NULL;
}

struct dynarray*    SleighCompile_createMacroUse(SleighCompile *s, MacroSymbol *sym, struct dynarray *param)
{
    return NULL;
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

void                SleighCompile_parseFromNewFile(SleighCompile *s, const char *filename)
{

}

void                SleighCompile_parseFileFinished(SleighCompile *s)
{
}

char*               SleighCompile_grabCurrentFilePath(SleighCompile *s)
{
    return NULL;
}

SleighSymbol*       SleighCompile_findSymbol(SleighCompile *s, char *name)
{
    return NULL;
}

void                SleighCompile_nextLine(SleighCompile *s)
{
}

void                SleighCompile_calcContextLayout(SleighCompile *s)
{
}

FieldQuality*   FieldQuality_new(const char *name, uintb l, uintb h)
{
    return NULL;
}

SectionVector*  SleighCompile_finalNamedSection(SleighCompile *s, SectionVector *vec, ConstructTpl *section)
{
    return NULL;
}