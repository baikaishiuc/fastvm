
#include "mcore/mcore.h"
#include "vm.h"
#include "minst.h"

static inline int minst_cmp(void *a, void *b, void *ref)
{
    long l = (unsigned long)a;
    long r = (unsigned long)(((struct minst *)b)->addr);

    return (int)(l - r);
}

struct minst_blk*   minst_blk_new(char *funcname)
{
    struct minst_blk *blk;

    blk = (struct minst_blk *)calloc(1, sizeof (blk[0]));
    if (NULL == blk)
        vm_error("minst_blk_new() calloc failure");

    minst_blk_init(blk, funcname);

    return blk;
}

void                minst_blk_delete(struct minst_blk *blk)
{
    int i;

    if (!blk)   return;

    if (blk->funcname)  free(blk->funcname);

    for (i = 0; i < blk->tvar.len; i++) {
        free(blk->tvar.ptab[i]);
    }

    for (i = 0; i < blk->allinst.len; i++) {
        minst_delete(blk->allinst.ptab[i]);
    }

    free(blk);
}

void                minst_blk_init(struct minst_blk *blk, char *funcname)
{
    memset(blk, 0, sizeof (blk[0]));

    blk->funcname = strdup(funcname);

    blk->allinst.compare_func = minst_cmp;
}

void                minst_blk_uninit(struct minst_blk *blk)
{
    if (blk->funcname)  free(blk->funcname);

    memset(blk, 0, sizeof (blk[0]));
}

struct minst_var*   minst_blk_new_stack_var(struct minst_blk *blk, int top)
{
    struct minst_var *var;
    /* 每次分配堆栈变量都是从栈顶开始分配，所以新分配的变量一定比老的栈顶大 */
    if (blk->tvar.len && (var = blk->tvar.ptab[blk->tvar.len - 1]) && var->top >= top) {
        vm_error("alloc stack variable failed with top error [%d >= %d]", var->top, top);
    }

    var = calloc(1, sizeof (var[0]));
    if (!var)
        vm_error("stack_var calloc failure");

    var->top = top;
    var->t = blk->tvar_id++;

    dynarray_add(&blk->tvar, var);

    return var;
}

struct minst_var*   minst_blk_find_stack_var(struct minst_blk *blk, int top)
{
    struct minst_var *tvar;
    int i;

    for (i = blk->tvar.len - 1; i >= 0; i++) {
        tvar = blk->tvar.ptab[i];
        if (tvar->top < top)
            return NULL;

        if (tvar->top == top)
            return tvar;
    }

    return NULL;
}

/* 当调用比如pop弹出堆栈时，也删除对应的临时变量 */
int                 minst_blk_delete_stack_var(struct minst_blk *blk, int top)
{
    return 0;
}

struct minst*       minst_new(struct minst_blk *blk, char *code, int len)
{
    struct minst *minst;

    minst = calloc(1, sizeof (minst[0]));
    if (!minst)
        vm_error("minst_new() failure");

    dynarray_add(&blk->allinst, minst);
    bitset_init(&minst->use, 32);
    bitset_init(&minst->def, 32);
    bitset_init(&minst->in, 32);
    bitset_init(&minst->out, 32);

    return minst;
}

void                minst_delete(struct minst *minst)
{
    bitset_uninit(&minst->use);
    bitset_uninit(&minst->def);
    bitset_uninit(&minst->in);
    bitset_uninit(&minst->out);

    free(minst);
}

struct minst*       minst_blk_find(struct minst_blk *blk, char *addr)
{
    return dynarray_find(&blk->allinst, addr);
}

int                 minst_blk_liveness_calc(struct minst_blk *blk)
{
    struct minst *minst, *succ;
    struct bitset in = { 0 }, out = { 0 };
    int i, j, update = 1;

    while (update) {
        update = 0;
        for (i = 0; i < blk->allinst.len; i++) {
            minst = blk->allinst.ptab[i];

            bitset_clone(&in, &minst->in);
            bitset_clone(&out, &minst->out);

            bitset_or(&minst->use, bitset_sub(&minst->out, &minst->def));

            for (j = 0; j < minst->succs.len; j ++) {
                succ = minst->succs.ptab[j];
                bitset_or(&minst->out, &succ->in);
            }

            if (!bitset_is_equal(&in, &minst->in) || !bitset_is_equal(&out, &minst->out))
                update = 1;
        }
    }

    return 0;
}

