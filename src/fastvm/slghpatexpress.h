
#ifndef __SLGHPATEXPRESS_H__
#define __SLGHPATEXPRESS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "slghpattern.h"

typedef struct TokenPattern {
    Pattern *pattern;
    struct dynarray toklist;
    int leftellipsis;
    int rightellipsis;
} TokenPattern;

TokenPattern*   TokenPattern_new(Pattern *pat);

PatternBlock*   TokenPattern_buildSingle(TokenPattern *p, int startbit, int endbit, int byteval);
PatternBlock*   TokenPattern_buildBigBlock(TokenPattern *p, int size, int bitstart, int bitend, int value);
PatternBlock*   TokenPattern_buildLittleBlock(TokenPattern *p, int size, int bitstart, int bitend, int value);

TokenPattern*   TokenPattern_doAnd(const TokenPattern *op1, const TokenPattern *op2);
TokenPattern*   TokenPattern_doOr(const TokenPattern *op1, const TokenPattern *op2);
TokenPattern*   TokenPattern_doCat(const TokenPattern *op1, const TokenPattern *op2);
TokenPattern*   TokenPattern_commobSubPattern(const TokenPattern *pat, const TokenPattern *subpat);

typedef struct PatternValue {
    int refcount;

    enum {
        a_token_field,
        a_context_field,
        a_constant_value,
        a_start_instruction_value,
    } type;

    union {
        struct {
            Token *tok;
            bool bigendian;
            bool sighbit;
            int bitstart;   // Bits within the token, 0 bit is LEAST significant
            int bitend;
            int bytestart;  // Bytes to read to get value
            int byteend;
            int shift;      // Amount to shift to align value (bitstart % 8)
        } tokenField;

        struct {
            bool sighbit;
            int bitstart;   // Bits within the token, 0 bit is LEAST significant
            int bitend;
            int bytestart;  // Bytes to read to get value
            int byteend;
            int shift;      // Amount to shift to align value (bitstart % 8)
        } contextField;

        struct {
            int val;
        } constantValue;

        struct {
            int reserved;
        } startInstructionValue;
    };
} PatternValue;

#ifdef __cplusplus
}
#endif

#endif