
#ifndef __context_h__
#define __context_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "space.h"
#include "address.h"

typedef struct Token {
    int size;
    int index;
    int bigendian;
    char name[1];
} Token;

Token*          Token_new(const char *name, int sz, int be, int index);
void            Token_delete(Token *t);

typedef struct FixedHandle {
    AddrSpace *space;
} _FixedHandle;

#ifdef __cplusplus
}
#endif

#endif