
#include "mcore/mcore.h"
#include "vm.h"
#include "minst.h"

#define TVAR_BASE       32

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
    if (!blk)   return;

    minst_blk_uninit(blk);

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
    int i;

    if (blk->funcname)  free(blk->funcname);

    for (i = 0; i < blk->tvar.len; i++) {
        free(blk->tvar.ptab[i]);
    }
    dynarray_reset(&blk->tvar);

    for (i = 0; i < blk->allinst.len; i++) {
        minst_delete(blk->allinst.ptab[i]);
    }

    memset(blk, 0, sizeof (blk[0]));
}

struct minst*       minst_new(struct minst_blk *blk, unsigned char *code, int len, void *reg_node)
{
    struct minst *minst;

    minst = calloc(1, sizeof (minst[0]));
    if (!minst)
        vm_error("minst_new() failure");

    minst->addr = code;
    minst->len = len;
    minst->reg_node = reg_node;

    dynarray_add(&blk->allinst, minst);
    bitset_init(&minst->use, 32);
    bitset_init(&minst->def, 32);
    bitset_init(&minst->in, 32);
    bitset_init(&minst->out, 32);

    return minst;
}

void                minst_delete(struct minst *minst)
{
    struct minst_node *node, *next;

    bitset_uninit(&minst->use);
    bitset_uninit(&minst->def);
    bitset_uninit(&minst->in);
    bitset_uninit(&minst->out);

    node = minst->succs.next;
    while (node) {
        next = node->next;
        free(node);
        node = next;
    }

    node = minst->preds.next;
    while (node) {
        next = node->next;
        free(node);
        node = next;
    }

    free(minst);
}

struct minst*       minst_blk_find(struct minst_blk *blk, unsigned long addr)
{
    return dynarray_find(&blk->allinst, (void *)addr);
}

void                minst_succ_add(struct minst *minst, struct minst *succ)
{
    struct minst_node *tnode;
    if (!minst || !succ)
        return;

    if (!minst->succs.minst)
        minst->succs.minst = succ;
    else {
        tnode = calloc(1, sizeof (tnode[0]));
        if (!tnode)
            vm_error("minst_succ_add() calloc failure");

        tnode->minst = succ;
        tnode->next = minst->succs.next;

        minst->succs.next = tnode;
    }
}

void                 minst_pred_add(struct minst *minst, struct minst *pred)
{
    struct minst_node *tnode;

    if (!minst || !pred)
        return;

    if (!minst->preds.minst)
        minst->preds.minst = pred;
    else {
        tnode = calloc(1, sizeof (tnode[0]));
        if (!tnode)
            vm_error("minst_succ_add() calloc failure");

        tnode->minst = pred;
        tnode->next = minst->preds.next;

        minst->preds.next = tnode;
    }
}


int                 minst_blk_liveness_calc(struct minst_blk *blk)
{
    struct minst_node *succ;
    struct minst *minst;
    struct bitset in = { 0 }, out = { 0 }, use = {0};
    int i, changed = 1;

    while (changed) {
        changed = 0;
        for (i = blk->allinst.len - 1; i >= 0; i--) {
            minst = blk->allinst.ptab[i];

            bitset_clone(&in, &minst->in);
            bitset_clone(&out, &minst->out);
            bitset_clone(&use, &minst->use);

            bitset_clone(&minst->in, bitset_or(&use, bitset_sub(&minst->out, &minst->def)));

            for (succ = &minst->succs; succ; succ = succ->next) {
                if (succ->minst)
                    bitset_or(&minst->out, &succ->minst->in);
            }

            if (!bitset_is_equal(&in, &minst->in) || !bitset_is_equal(&out, &minst->out))
                changed = 1;
        }
    }

    bitset_uninit(&in);
    bitset_uninit(&out);
    bitset_uninit(&use);

    return 0;
}

int                 minst_blk_dead_code_elim(struct minst_blk *blk)
{
    struct minst *minst;
    int changed = 1, i;
    struct bitset def = { 0 };

    while (changed) {
        changed = 0;
        minst_blk_liveness_calc(blk);
        for (i = 0; i < blk->allinst.len; i++) {
            minst = blk->allinst.ptab[i];
            if (minst->flag.dead_code)
                continue;

            if (bitset_is_empty(&minst->def))
                continue;

            bitset_clone(&def, &minst->def);
            if (!bitset_is_equal(bitset_and(&def, &minst->out), &minst->def)) {
                minst->flag.dead_code = 1;
                changed = 1;
            }
        }
    }

    bitset_uninit(&def);

    return 0;
}
