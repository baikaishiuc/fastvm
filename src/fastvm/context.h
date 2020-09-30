
#ifndef __context_h__
#define __context_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "globalcontext.h"
#include "space.h"

typedef struct FixedHandle      FixedHandle;
typedef struct ConstructState   ConstructState;
typedef struct ContextSet       ContextSet;
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

ConstructState*         ConstructState_newV(void);
void                    ConstructState_delete(ConstructState *cs);

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
    struct dynarray contextcommit;  // ContextSet

    Address *addr;
    Address *naddr;
    Address *calladdr;

    ConstructState *state;
    ConstructState *base_state;
    int alloc;
    int delayslot;
};

ParserContext*  ParserContext_new(ContextCache *ccache);
void            ParserContext_delete(ParserContext *pc);
void            ParserContext_initialize(ParserContext *pc, int maxstate, int maxparam, AddrSpace *spc);
uintm           ParserContext_getInstructionBytes(ParserContext *pc, int bytestart, int size, int off);
uintm           ParserContext_getContextBytes(ParserContext *pc, int bytestart, int size);
uintm           ParserContext_getInstructionBits(ParserContext *pc, int startbit, int size, int off);
uintm           ParserContext_getContextBits(ParserContext *pc, int startbit, int size);
void            ParserContext_clearCommits(ParserContext *pc);
void            ParserContext_loadContext(ParserContext *pc);
void            ParserContext_setContextWord(ParserContext *pc, int i, uintm val, uintm mask);
void            ParserContext_addCommit(ParserContext *pc, SleighSymbol *sym, int num, int mask, bool flow, ConstructState *point);
#define ParserContext_getLength(p)      p->base_state->length

void     ParserContext_deallocateState(ParserContext *pc, ParserWalker *walker);

struct ParserWalker {
    ParserContext   *const_context;
    ParserContext   *cross_context;
    ParserContext   *context;

    ConstructState *point;
    int depth;
    int breadcrumb[32];
};

ParserWalker*           ParserWalker_new(ParserContext *c);
ParserWalker*           ParserWalker_new2(ParserContext *c, ParserContext *cross);
void                    ParserWalker_delete(ParserWalker *p);
void                    ParserWalker_baseState(ParserWalker *p);
#define ParserWalker_getConstructor(p)      (p)->point->ct
#define ParserWalker_setOffset(p,v)         (p)->point->offset = v

void                    ParserWalker_pushOperand(ParserWalker *p, int i);
void                    ParserWalker_popOperand(ParserWalker *p);
void                    ParserWalker_setOutOfBandState(ParserWalker *p, Constructor *ct, int index,
                    ConstructState *tempstate, ParserWalker *otherwalker);
uint4                   ParserWalker_getOffset(ParserWalker *walker, int i);
#define ParserWalker_getInstructionBytes(p, byteoff, numbytes)      ParserContext_getInstructionBytes(p->const_context, byteoff, numbytes, p->point->offset)
#define ParserWalker_getContextBytes(p, byteoff, numbytes)          ParserContext_getContextBytes(p->const_context, byteoff, numbytes)

#define ParserWalker_getInstructionBits(p, startbit, size)  ParserContext_getInstructionBits(p->const_context, startbit, size, p->point->offset)
#define ParserWalker_getContextBits(p, startbit, size)      ParserContext_getContextBits(p->const_context, startbit, size)

const Address*  ParserWalker_getAddr(ParserWalker *p);
const Address*  ParserWalker_getNaddr(ParserWalker *p);
const Address*  ParserWalker_getRefAddr(ParserWalker *p);
const Address*  ParserWalker_getDestAddr(ParserWalker *p);

#ifdef __cplusplus
}
#endif

#endif