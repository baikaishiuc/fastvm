
#include "vm.h"
#include "slghpatexpress.h"

PatternExpression*  PatternExpression_new(int type, PatternExpression *left, PatternExpression *right) 
{
    PatternExpression *pat = vm_mallocz(sizeof (pat[0]));

    pat->type = type;
    switch (type) {
        case a_plusExp:
            pat->and.left = left;
            pat->and.right = right;
            break;

        default:
            vm_error("un-support PatternExpression(%d)", type);
    }

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
