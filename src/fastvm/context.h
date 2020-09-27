
#ifndef __context_h__
#define __context_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "globalcontext.h"
#include "space.h"

typedef struct FixedHandle      FixedHandle;
typedef struct ConstructState   ConstructState;
typedef struct ParserContext    ParserContext;
typedef struct ParserWalker     ParserWalker;
typedef struct Constructor  Constructor;
typedef struct SleighSymbol SleighSymbol, SpaceSymbol, TokenSymbol, SectionSymbol, UserOpSymbol, TripleSymbol, FamilySymbol,
PatternlessSymbol, EpsilonSymbol, ValueSymbol, ValueMapSymbol, NameSymbol, VarnodeSymbol, BitRangeSymbol,
ContextSymbol, VarnodeListSymbol, OperandSymbol, StartSymbol, EndSymbol, MacroSymbol, SubtableSymbol, LabelSymbol,
BitrangeSymbol, SpecificSymbol;

typedef struct Token {
    int size;
    int index;
    int bigendian;
    char name[1];
} Token;

Token*          Token_new(const char *name, int sz, int be, int index);
void            Token_delete(Token *t);

struct FixedHandle {
    AddrSpace *space;
};

struct ConstructState {
    Constructor *ct;
    FixedHandle hand;
    struct dynarray resolve;
    ConstructState  *parent;
    int length;
    uint4 offset;
};

struct ContextSet {
    TripleSymbol *sym;
    ConstructState *point;
    int num;
    uintm mask;
    uintm value;
    bool flow;
};

struct ParserContext {
    enum {
        uninitialized,
        disassembly,
        pcode
    } parsestate;

    AddrSpace *const_space;
    unsigned char buf[16];      // 存放单条指令的buf，x64貌似最大15
    uintm *context; 
    int contextsize;
    ContextCache *contcache;
    ConstructState *base_state;
    int alloc;
    int delayslot;
};

uintm       ParserContext_getInstructionBytes(ParserContext *pc, int bytestart, int size, int off);
uintm       ParserContext_getContextBytes(ParserContext *pc, int bytestart, int size);

struct ParserWalker {
    ParserContext   *const_context;
    ParserContext   *cross_context;

    ConstructState *point;
    int depth;
    int breadcrumb[32];
};

ParserWalker*           ParserWalker_new(ParserContext *c);
ParserWalker*           ParserWalker_new2(ParserContext *c, ParserContext *cross);
void                    ParserWalker_delete(ParserWalker *p);
void                    ParserWalker_pushOperand(ParserWalker *p, int i);
void                    ParserWalker_popOperand(ParserWalker *p);
void                    ParserWalker_setOutOfBandState(ParserWalker *p, Constructor *ct, int index,
                    ConstructState *tempstate, ParserWalker *otherwalker);
#define ParserWalker_getInstructionBytes(p, byteoff, numbytes)      ParserContext_getInstructionBytes(p->const_context, byteoff, numbytes, p->point->offset)
#define ParserWalker_getContextBytes(p, byteoff, numbytes)          ParserContext_getContextBytes(p->const_context, byteoff, numbytes)

#ifdef __cplusplus
}
#endif

#endif