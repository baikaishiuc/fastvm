
#include "vm.h"
#include "slghsymbol.h"
#include "slghpatexpress.h"

PatternExpression*  PatternExpression_new(int type, ...) 
{
    va_list ap;
    va_start(ap, type);
    PatternExpression *pat = vm_mallocz(sizeof (pat[0]));

    pat->type = type;
    switch (type) {
        case a_plusExp:
        case a_subExp:
        case a_multExp:
        case a_leftShiftExp:
        case a_rightShiftExp:
        case a_andExp:
        case a_orEq:
        case a_xorExp:
        case a_divExp:
            pat->and.left = va_arg(ap, PatternExpression *);
            pat->and.right = va_arg(ap, PatternExpression *);
            break;

        case a_startInstructionValue:
        case a_endInstructionValue:
            break;

        case a_operandValue:
            pat->operandValue.index = va_arg(ap, int);
            pat->operandValue.ct = va_arg(ap, Constructor *);
            break;

        case a_contextField:
        case a_tokenField:
            break;

        default:
            vm_error("un-support PatternExpression(%d)", type);
    }
    va_end(ap);

    return pat;
}

void                PatternExpression_delete(PatternExpression *p)
{
    vm_free(p);
}

PatternEquation*    PatternEquation_new(int type, ...)
{
    PatternEquation *pe = vm_mallocz(sizeof(pe[0]));

    va_list ap;
    va_start(ap, type);
    switch (type) {
    case a_operandEq:
        pe->operand.index = va_arg(ap, int);
        break;

    case a_unconstrainedEq:
        pe->unconstrained.patex = va_arg(ap, PatternExpression *);
        break;

    case a_equalEq:
    case a_notEqualEq:
    case a_lessEq:
    case a_lessEqualEq:
    case a_greaterEq:
    case a_greaterEqualEq:
        pe->equal.lhs = va_arg(ap, PatternValue *);
        pe->equal.rhs = va_arg(ap, PatternExpression *);
        break;

    case a_andEq:
    case a_orEq:
    case a_catEq:
        pe->and.left = va_arg(ap, PatternEquation *);
        pe->and.right = va_arg(ap, PatternEquation *);
        break;

    case a_leftEllipsisEq:
    case a_rightEllipsisEq:
        pe->leftEllipsis.eq = va_arg(ap, PatternEquation *);
        break;
    }
    va_end(ap);

    return pe;
}

void                PatternEquation_delete(PatternEquation *p)
{
    vm_free(p);
}

EquationAnd*        EquationAnd_new(PatternEquation *l, PatternEquation *r)
{
    EquationAnd *e = PatternEquation_new(a_andEq, l, r);

    e->and.left->refcount++;
    e->and.right->refcount++;

    return e;
}

ConstantValue*      ConstantValue_new(void)
{
    return NULL;
}

ConstantValue*      ConstantValue_newB(intb b)
{
    return NULL;
}

StartInstructionValue*  StartInstructionValue_new()
{
    return PatternExpression_new(a_startInstructionValue);
}

EndInstructionValue*    EndInstructionValue_new()
{
    return PatternExpression_new(a_endInstructionValue);
}

OperandValue*       OperandValue_new(int index, Constructor *ct)
{
    return PatternExpression_new(a_operandValue, index, ct);
}

intb                PatternValue_minValue(PatternValue *pv)
{
    switch (pv->type) {
    case a_tokenField:
    case a_contextField:
    case a_startInstructionValue:
    case a_endInstructionValue:
        return 0;

    case a_constantValue:
        return pv->constantValue.val;

    default:
        vm_error("Unsupport PatternValue type = %d", pv->type);
    }

    return 0;
}

intb                PatternValue_maxValue(PatternValue *pv)
{
    intb res;
    switch (pv->type) {
    case a_tokenField:
        res = 0;
        res = ~res;
        zero_extend(&res, pv->tokenField.bitend - pv->tokenField.bitstart);
        return res;

    case a_contextField:
        res = 0;
        res = ~res;
        zero_extend(&res, pv->contextField.endbit - pv->contextField.startbit);
        return res;

    case a_constantValue:
        return pv->constantValue.val;

    case a_startInstructionValue:
        return 0;

    case a_endInstructionValue:
        return 0;

    default:
        vm_error("Unsupport PatternValue type = %d", pv->type);
    }

    return 0;
}

ContextField*       ContextField_new(bool s, int sbit, int ebit)
{
    ContextField*   c = PatternExpression_new(a_contextField);

    c->contextField.signbit = s;
    c->contextField.startbit = sbit;
    c->contextField.endbit = ebit;
    c->contextField.startbyte = sbit / 8;
    c->contextField.endbyte = ebit / 8;
    c->contextField.shift = 7 - ebit % 8;
    
    return c;
}

TokenField*         TokenField_new(Token *tk, bool s, int bstart, int bend)
{
    TokenField*     t = PatternExpression_new(a_tokenField);

    t->tokenField.tok = tk;
    t->tokenField.bigendian = tk->bigendian;
    t->tokenField.signbit = s;
    t->tokenField.bitstart = bstart;
    t->tokenField.bitend = bend;
    if (t->tokenField.bigendian) {
        t->tokenField.byteend = (tk->size * 8 - bstart - 1) / 8;
        t->tokenField.bytestart = (tk->size * 8 - bend - 1) / 8;
    }
    else {
        t->tokenField.bytestart = bstart / 8;
        t->tokenField.byteend = bend / 8;
    }
    t->tokenField.shift = bstart % 8;

    return t;
}

OperandEquation*    OperandEquation_new(int index)
{
    OperandEquation *eq = vm_mallocz(sizeof(eq[0]));

    eq->type = a_operandEq;
    eq->operand.index = index;

    return eq;
}
