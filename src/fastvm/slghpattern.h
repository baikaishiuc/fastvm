
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

PatternBlock*       PatternBlock_clone(PatternBlock *pb);
bool                PatternBlock_identical(PatternBlock *op1, PatternBlock *op2);
uintm               PatternBlock_getMask(PatternBlock *pb, int startbit, int size);
uintm               PatternBlock_getValue(PatternBlock *pb, int startbit, int size);
void                PatternBlock_normalize(PatternBlock *pb);
#define PatternBlock_getLength(pb)          (pb->offset + pb->nonzerosize)

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

int             Pattern_numDisjoint(Pattern *pat);
Pattern*        Pattern_simplifyClone(Pattern *pat);

#ifdef __cplusplus
}
#endif

#endif