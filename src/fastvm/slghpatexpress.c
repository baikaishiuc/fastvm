
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
    return NULL;
}

void                PatternEquation_delete(PatternEquation *p)
{
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
