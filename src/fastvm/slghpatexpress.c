
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
