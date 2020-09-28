
#ifndef __globalcontext_h__
#define __globalcontext_h__

#include "address.h"
#include "partmap.h"

typedef struct ContextDatabase  ContextDatabase, ContextInternal;
typedef struct ContextCache     ContextCache;
typedef struct FreeArray        FreeArray;
typedef struct ContextBitRange  ContextBitRange;
typedef struct TrackedContext   TrackedContext;
typedef struct dynarray         TrakcedSet;

struct ContextBitRange {
    int word;
    int startbit;
    int endbit;
    int shift;
    uintm mask;
};

ContextBitRange*    ContextBitRange_newV(void);
ContextBitRange*    ContextBitRange_new(int sbit, int ebit);
void                ContextBitRange_setValue(ContextBitRange *cb, uintm *vec, uintm val);
uintm               ContextBitRange_getValue(ContextBitRange *cb, uintm *vec);

struct TrackedContext {
    VarnodeData loc;
    uintb   val;
};

struct FreeArray {
    uintm *array;
    uintm *mask;
    int size;
};

FreeArray*      FreeArray_new();
void            FreeArray_delete(FreeArray *fa);
int             FreeArray_assign(FreeArray *op1, FreeArray *op2);
void            FreeArray_reset(FreeArray *f, int sz);

struct ContextDatabase {
    int size;

    struct rb_root  variabels;
    partmap         database;
    partmap         trackbase;
};

ContextDatabase*    ContextDatabase_new();
void                ContextDatabase_delete(ContextDatabase *cd);

void                ContextDatabase_registerVariable(ContextDatabase *cd, char *name, int sbit, int ebit);
uintm*              ContextDatabase_getContext(Address *addr);

struct ContextCache {
    ContextDatabase *database;
    bool allowset;

    AddrSpace *curspace;
    uintb first;
    uintb last;
    uintm *context;
};

#endif