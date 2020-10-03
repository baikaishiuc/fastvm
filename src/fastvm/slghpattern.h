
#ifndef __SLGHPATTERN_H__
#define __SLGHPATTERN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mcore/mcore.h"
#include "context.h"

typedef struct Pattern  CombinePattern, ContextPattern, InstructionPattern, Pattern, DisjointPattern;

    // a=[2,3]
typedef struct PatternBlock {
    int offset;         // offset to non-zero byte of mask
    int nonzerosize;    // Last byte(+1) containing nonzero mask
    struct dynarray maskvec;    // Mask
    struct dynarray valvec;
} PatternBlock;

PatternBlock*       PatternBlock_newB(bool tf);
PatternBlock*       PatternBlock_new3(int off, uintm mask, uintm val);
PatternBlock*       PatternBlock_clone(PatternBlock *pb);
void                PatternBlock_delete(PatternBlock *a);
PatternBlock*       PatternBlock_intersect(PatternBlock *a, PatternBlock *b);

bool                PatternBlock_identical(PatternBlock *op1, PatternBlock *op2);
uintm               PatternBlock_getMask(PatternBlock *pb, int startbit, int size);
uintm               PatternBlock_getValue(PatternBlock *pb, int startbit, int size);
void                PatternBlock_normalize(PatternBlock *pb);
#define PatternBlock_getLength(pb)          (pb->offset + pb->nonzerosize)
#define PatternBlock_alwaysTrue(pb)         ((pb)->nonzerosize == 0)
#define PatternBlock_alwaysFalse(pb)        ((pb)->nonzerosize == -1)

struct Pattern {
    enum {
        a_disjointPattern,
        a_instructionPattern,
        a_contextPattern,
        a_combinePattern,
        a_orPattern
    } type;

    union {
        struct {
            PatternBlock *maskvalue;
        } instruction;

        struct {
            PatternBlock *maskvalue;
        } context;

        struct {
            ContextPattern *context;
            InstructionPattern *instr;
        } combine;

        struct {
            struct dynarray orlist;
        } or;
    } ;
};

InstructionPattern*     InstructionPattern_new(void);
InstructionPattern*     InstructionPattern_newP(PatternBlock *pb);
InstructionPattern*     InstructionPattern_newB(bool tf);
void                    Pattern_delete(Pattern *p);

bool            Pattern_isMatch(Pattern *pat, ParserWalker *walker);
int             Pattern_numDisjoint(Pattern *pat);
Pattern*        Pattern_simplifyClone(Pattern *pat);
Pattern*        Pattern_commonSubPattern(Pattern *a,Pattern * b, int sa);
bool            Pattern_alwaysTrue(Pattern *a);
bool            Pattern_alwaysFalse(Pattern *a);

#ifdef __cplusplus
}
#endif

#endif