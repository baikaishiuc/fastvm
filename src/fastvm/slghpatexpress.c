
#include "vm.h"
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
