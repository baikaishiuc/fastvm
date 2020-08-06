
#ifndef __SLGHPATTERN_H__
#define __SLGHPATTERN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mcore/mcore.h"
#include "context.h"

    // a=[2,3]
typedef struct PatternBlock {
    int offset;         // offset to non-zero byte of mask
    int nonzerosize;    // Last byte(+1) containing nonzero mask
    struct dynarray maskvec;    // Mask
    struct dynarray valvec;
} PatternBlock;

typedef struct Pattern {
    int reserved;
} Pattern;

#ifdef __cplusplus
}
#endif

#endif