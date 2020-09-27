
#ifndef __globalcontext_h__
#define __globalcontext_h__

#include "address.h"
#include "partmap.h"

typedef struct ContextDatabase  ContextDatabase;
typedef struct ContextCache     ContextCache;

struct ContextDatabase {
    int reserved;
};

struct ContextCache {
    ContextDatabase *database;
    bool allowset;

    AddrSpace *curspace;
    uintb first;
    uintb last;
    uintm *context;
};

#endif