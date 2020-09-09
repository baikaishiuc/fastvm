
#include "vm.h"
#include "context.h"

Token*          Token_new(const char *name, int sz, int be, int index)
{
    Token *t = vm_mallocz(sizeof(t[0]) + strlen(name));

    t->size = sz;
    t->bigendian = be;
    t->index = index;
    strcpy(t->name, name);

    return t;
}

void            Token_delete(Token *t)
{
    vm_free(t);
}