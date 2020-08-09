
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

typedef struct PatternExpression  PatternExpression, ArithmeticExpression, PatternValue, ValueExpression, ConstantValue, OperandValue; 

typedef struct PatternExpression {
    int refcount;

    enum {
        a_plusExp,
        a_subExp,
        a_multExp,
        a_leftShiftExp,
        a_rightShiftExp,
        a_andExp,
        a_orExp,
        a_xorExp,
        a_divExp,
        a_minusExp,
        a_notExp,

// leaf node
        a_tokenField,
        a_contextField,
        a_constantValue,
        a_startInstructionValue,
        a_endInstructionValue,
        a_operandValue,
    } type;

    union {
        struct {
            PatternExpression *left;
            PatternExpression *right;
        } plus, sub, mult, leftShift, rightShfit, and, or, xor, div;

        PatternExpression *minus, *not;
        PatternValue *value;

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
            int8_t val;
        } constantValue;

        struct {
            int reserved;
        } startInstructionValue;
    };
} PatternExpression, ArithmeticExpression, PatternValue, ValueExpression, ConstantValue, OperandValue;;

PatternExpression*  PatternExpression_new(int type, ...);
void                PatternExpression_delete(PatternExpression *p);

ConstantValue*      ConstantValue_new(intb);
void                ConstantValue_delete(ConstantValue *c);

typedef struct OperandResolve {
    struct dynarray     *operands;
    int     base;
    int     offset;
    int     cur_rightmost;
    int     size;
} OperandResolve;

typedef struct PatternEquation  PatternEquation, LogicalExpression;

typedef struct PatternEquation {
    int refcount;
    TokenPattern resultpattern;
    int index;

    enum {
        a_operand,
        a_unconstrained,
        a_equal,
        a_notEqual,
        a_less,
        a_lessEqual,
        a_greater,
        a_greaterEqual,
        a_and,
        a_or,
        a_cat,
        a_leftEllipsis,
        a_rightEllipsis,
    } type;

    union {
        struct {
            int index;
        } operand;

        struct {
            PatternExpression *patex;
        } unconstrained;

        struct {
            PatternEquation *left;
            PatternEquation *right;
        } and, or, add, cat;

        struct {
            PatternValue *lhs;
            PatternExpression *rhs;
        } equal, notEqual, less, lessEqual, greater, greaterEqual;

        struct {
            PatternEquation *eq;
        } leftEllipsis, rightEllipsis;

    };
} PatternEquation, LogicalExpression;

PatternEquation*    PatternEquation_new(int type, ...);
void                PatternEquation_delete(PatternEquation *p);


#ifdef __cplusplus
}
#endif

#endif