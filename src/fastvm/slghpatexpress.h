
#ifndef __SLGHPATEXPRESS_H__
#define __SLGHPATEXPRESS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "slghpattern.h"
#include "slghsymbol.h"

typedef struct TokenPattern TokenPattern, tokpat;

struct TokenPattern {
    Pattern *pattern;
    struct dynarray toklist;
    int leftellipsis;
    int rightellipsis;
};

TokenPattern*   TokenPattern_newV(void);
TokenPattern*   TokenPattern_new(Pattern *pat);
TokenPattern*   TokenPattern_newT(Token *token);
TokenPattern*   TokenPattern_new4(Token *tok, intb value, int bitstart, int bitend);
PatternBlock*   TokenPattern_buildBigBlock(TokenPattern *tp, int4 size, int4 bitstart, int4 bitend, intb value);
PatternBlock*   TokenPattern_buildLittleBlock(TokenPattern *a, int4 size, int4 bitstart, int4 bitend, intb value);
PatternBlock*   TokenPattern_buildSingle(TokenPattern *a, int4 startbit, int4 endbit, uintm byteval);

TokenPattern*   TokenPattern_clone(TokenPattern *op1, TokenPattern *op2);

TokenPattern*   TokenPattern_doAnd(const TokenPattern *op1, const TokenPattern *op2);
TokenPattern*   TokenPattern_doOr(const TokenPattern *op1, const TokenPattern *op2);
TokenPattern*   TokenPattern_doCat(const TokenPattern *op1, const TokenPattern *op2);
TokenPattern*   TokenPattern_commobSubPattern(const TokenPattern *pat, const TokenPattern *subpat);
int             TokenPattern_getMinimumLength(TokenPattern *tp);
#define TokenPattern_alwaysTrue(t)          Pattern_alwaysTrue((t)->pattern)
#define TokenPattern_alwaysFalse(t)         Pattern_alwaysFalse((t)->pattern)

/* PatternExpression 是 算术表达式，叫 arithmetic expression 更合适 

PatternEquation 是 逻辑表达式，叫 logical expression 更合适，它现在的命名其实我是晕的
*/

typedef struct PatternExpression  PatternExpression, ArithmeticExpression, PatternValue, ValueExpression, ConstantValue, OperandValue,
            StartInstructionValue, EndInstructionValue, ContextField, TokenField, patexp;

struct PatternExpression {
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

        struct {
            PatternExpression * unary;
        } not, minus;

        PatternValue *value;

        /* tokenField看起来不允许超过64位 */
        struct {
            Token *tok;
            bool bigendian;
            bool signbit;
            int bitstart;   // Bits within the token, 0 bit is LEAST significant
            int bitend;
            int bytestart;  // Bytes to read to get value
            int byteend;
            int shift;      // Amount to shift to align value (bitstart % 8)
        } tokenField;

        struct {
            bool signbit;
            int startbit;   // Bits within the token, 0 bit is LEAST significant
            int endbit;
            int startbyte;  // Bytes to read to get value
            int endbyte;
            int shift;      // Amount to shift to align value (bitstart % 8)
        } contextField;

        struct {
            int8_t val;
        } constantValue;

        struct {
            int reserved;
        } startInstructionValue;

        struct {
            int index;
            Constructor *ct;
        } operandValue;
    };
};

PatternExpression*  PatternExpression_new(int type, ...);
void                PatternExpression_delete(PatternExpression *p);
void                PatternExpression_saveXml(PatternExpression *p, FILE *o);
intb                PatternExpression_getValue(PatternExpression *pe, ParserWalker *walker);
TokenPattern*       PatternExpression_genMinPattern(patexp *p, struct dynarray *ops);

ConstantValue*      ConstantValue_new(void);
ConstantValue*      ConstantValue_newB(intb b);

ContextField*       ContextField_new(bool s, int sbit, int ebit);

TokenField*         TokenField_new(Token *tk, bool s, int bstart, int bend);
#define TokenField_getValue     PatternExpression_getValue


StartInstructionValue*  StartInstructionValue_new();
EndInstructionValue*    EndInstructionValue_new();
#define PatternExpression_layClaim(e)       e->refcount++

OperandValue*       OperandValue_new(int index, Constructor *ct);

intb                PatternValue_minValue(PatternValue *pv);
intb                PatternValue_maxValue(PatternValue *pv);

typedef struct OperandResolve {
    struct dynarray     *operands;
    int     base;
    int     offset;
    int     cur_rightmost;
    int     size;
} OperandResolve;

typedef struct PatternEquation  PatternEquation, LogicalExpression, EquationAnd, OperandEquation, pateq;

typedef struct PatternEquation {
    int refcount;
    TokenPattern *resultpattern;
    int index;

    enum {
        a_operandEq,
        a_unconstrainedEq,
        a_equalEq,
        a_notEqualEq,
        a_lessEq,
        a_lessEqualEq,
        a_greaterEq,
        a_greaterEqualEq,
        a_andEq,
        a_orEq,
        a_catEq,
        a_leftEllipsisEq,
        a_rightEllipsisEq,
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
        } and, or, cat;

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

EquationAnd*        EquationAnd_new(PatternEquation *l, PatternEquation *r);
OperandEquation*    OperandEquation_new(int index);

void                PatternEquation_genPattern(pateq *p, struct dynarray *ops);


#ifdef __cplusplus
}
#endif

#endif