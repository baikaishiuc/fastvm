
#ifndef __context_h__
#define __context_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "space.h"

typedef struct Token {
    const char *name;
    int size;
    int index;
    int bigendian;
} Token;

Token*          Token_new(const char *name, int be, int index);
void            Token_delete(Token *t);

typedef struct FixedHandle {
    AddrSpace *space;
} _FixedHandle;

#ifdef __cplusplus
}
#endif

#endif