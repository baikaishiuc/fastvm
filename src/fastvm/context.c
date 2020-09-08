
#include "vm.h"
#include "context.h"

Token*          Token_new(const char *name, int be, int index)
{
    Token *t = vm_mallocz(sizeof(t[0]) + strlen(name));

    t->bigendian = be;
    t->index = index;
    strcpy(t->name, name);

    return t;
}

void            Token_delete(Token *t)
{
    vm_free(t);
}