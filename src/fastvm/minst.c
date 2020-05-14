
#include "mcore/mcore.h"
#include "vm.h"
#include "arm_emu.h"
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
    minst->id = blk->allinst.len;
    minst->ld = -2;

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

void                minst_pred_add(struct minst *minst, struct minst *pred)
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

void                minst_succ_del(struct minst *minst, struct minst *succ)
{
    struct minst_node *succ_node = &minst->succs, *prev_node;

    for (; succ_node; prev_node = succ_node, succ_node = succ_node->next) {
        if (succ_node->minst == succ) {
            for (; succ_node->next; prev_node = succ_node, succ_node = succ_node->next)
                succ_node->minst = succ_node->next->minst;

            if (succ_node == &minst->succs) {
                succ_node->minst = NULL;
                succ_node->next = NULL;
            }
            else {
                free(prev_node->next);
                prev_node->next = NULL;
            }

            return;
        }
    }
}

void                minst_pred_del(struct minst *minst, struct minst *pred)
{
    struct minst_node *pred_node = &minst->preds, *prev_node = NULL;

    for (; pred_node; prev_node = pred_node, pred_node = pred_node->next) {
        if (pred_node->minst == pred) {
            for (; pred_node->next; prev_node = pred_node, pred_node = pred_node->next)
                pred_node->minst = pred_node->next->minst;

            if (pred_node == &minst->preds) {
                pred_node->minst = NULL;
                pred_node->next = NULL;
            }
            else {
                free(prev_node->next);
                prev_node->next = NULL;
            }

            return;
        }
    }
}

void                minst_cfg_del(struct minst *minst)
{
    struct minst_node *pred_node, *succ_node;
    struct minst *pred, *succ;

    for (pred_node = &minst->preds; pred_node; pred_node = pred_node->next) {
        if (!(pred = pred_node->minst)) continue;
        for (succ_node = &minst->succs; succ_node; succ_node = succ_node->next) {
            if (!(succ = succ_node->minst)) continue;

            minst_succ_add(pred, succ);
            minst_pred_add(succ, pred);

            minst_pred_del(minst, pred);
            minst_succ_del(pred, minst);

            minst_pred_del(succ, minst);
            minst_succ_del(minst, succ);
        }
    }
}

void                minst_blk_gen_cfg(struct minst_blk *blk)
{
    int i, start = 0;
    struct minst *minst, *cur_grp_minst, *prev_minst;

    for (i = 0; i < blk->allinst.len; i++) {
        minst = blk->allinst.ptab[i];

        if (minst->flag.prologue || minst->flag.epilogue)
            continue;

        if (!start) {
            cur_grp_minst = minst;
            start = 1;
        }
        /* 1. 物理前指令是跳转指令，自动切块
           2. 当前节点有多个前节点 
           3. 前节点不是物理前节点 */
        else if (
            //!minst->flag.in_it_block && !prev_minst->flag.in_it_block && 
            (prev_minst->flag.b || prev_minst->succs.next || minst->preds.next || minst->preds.minst != blk->allinst.ptab[i - 1])) {
            cur_grp_minst = minst;
        }

        minst->cfg_node = cur_grp_minst;
        prev_minst = minst;
    }
}

int                 minst_is_bidirect(struct minst *minst, struct minst *succ)
{
    struct minst_node *tnode = &minst->succs;

    /* 在minst的后继节点中找succ*/
    for (; tnode; tnode = tnode->next) {
        if (tnode->minst == succ)
            break;
    }
    
    if (!tnode)
        return 0;

    tnode = &succ->preds;

    /* 在succ的前驱节点中找minst*/
    for (; tnode; tnode = tnode->next) {
        if (tnode->minst == minst)
            return 1;
    }

    return 0;
}

void                minst_blk_live_prologue_add(struct minst_blk *mblk)
{
    int i;
    struct minst *minst = minst_new(mblk, NULL, 0, NULL);

    minst->flag.prologue = 1;

    for (i = 0; i < 16; i++) {
        live_def_set(mblk, i);
    }
}

void                minst_blk_live_epilogue_add(struct minst_blk *mblk)
{
    int i, len;
    struct minst *minst = minst_new(mblk, NULL, 0, NULL);

    minst->flag.epilogue = 1;

    for (i = 0; i < 16; i++) {
        live_use_set(mblk, i);
    }

    minst_succ_add(mblk->allinst.ptab[0], mblk->allinst.ptab[1]);
    minst_pred_add(mblk->allinst.ptab[1], mblk->allinst.ptab[0]);

    len = mblk->allinst.len;
    minst_succ_add(mblk->allinst.ptab[len - 2], mblk->allinst.ptab[len - 1]);
    minst_pred_add(mblk->allinst.ptab[len - 1], mblk->allinst.ptab[len - 2]);
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
            if (minst->flag.dead_code) continue;

            bitset_clone(&in, &minst->in);
            bitset_clone(&out, &minst->out);
            bitset_clone(&use, &minst->use);

            bitset_clone(&minst->in, bitset_or(&use, bitset_sub(&minst->out, &minst->def)));

            for (succ = &minst->succs; succ; succ = succ->next) {
                if (succ->minst)
                    bitset_or(&minst->out, &succ->minst->in);
            }

            if (!changed && (!bitset_is_equal(&in, &minst->in) || !bitset_is_equal(&out, &minst->out)))
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

    /* FIXME: 删除死代码以后，liveness的计算没有把死代码计算进去 */
    while (changed) {
        changed = 0;
        for (i = 0; i < blk->allinst.len; i++) {
            minst = blk->allinst.ptab[i];
            if (minst->flag.dead_code)
                continue;

            if (bitset_is_empty(&minst->def))
                continue;

            bitset_clone(&def, &minst->def);
            if (!changed && !bitset_is_equal(bitset_and(&def, &minst->out), &minst->def)) {
                minst->flag.dead_code = 1;
                changed = 1;
            }
        }
    }

    for (i = 0; i < blk->allinst.len; i++) {
        minst = blk->allinst.ptab[i];
        if (minst->flag.dead_code && !minst->flag.be_del) {
            minst_cfg_del(minst);
            minst->flag.be_del = 1;
        }
    }

    bitset_uninit(&def);

    minst_blk_liveness_calc(blk);
    minst_blk_gen_reaching_definitions(blk);

    return 0;
}

int                 minst_get_def(struct minst *minst)
{
    if (minst->ld == -1)
        return minst->ld;

    return minst->ld = bitset_next_bit_pos(&minst->def, 0);
}


int                 minst_blk_gen_reaching_definitions(struct minst_blk *blk)
{
    struct minst *minst;
    BITSET_INIT(in);
    BITSET_INIT(out);
    struct minst_node *pred_node;
    int i, changed = 1, def;

    for (i = 0; i < blk->allinst.len; i++) {
        minst = blk->allinst.ptab[i];
        if (minst->flag.prologue || minst->flag.epilogue || minst->flag.dead_code || (def = minst_get_def(minst)) < 0)
            continue;

        bitset_clone(&minst->kills, &blk->defs[def]);
        bitset_set(&minst->kills, minst->id, 0);
        bitset_clear(&minst->rd_in);
        bitset_clear(&minst->rd_out);
    }

    while (changed) {
        changed = 0;

        for (i = 0; i < blk->allinst.len; i++) {
            minst = blk->allinst.ptab[i];
            if (minst->flag.prologue || minst->flag.epilogue || minst->flag.dead_code)
                continue;

            bitset_clone(&in, &minst->rd_in);
            bitset_clone(&out, &minst->rd_out);

            for (pred_node = &minst->preds; pred_node; pred_node = pred_node->next) {
                if (!pred_node->minst)  continue;

                bitset_or(&minst->rd_in, &pred_node->minst->rd_out);
            }
            bitset_sub(bitset_clone(&minst->rd_out, &minst->rd_in), &minst->kills);
            if (minst_get_def(minst) >= 0)
                bitset_set(&minst->rd_out, minst->id, 1);

            if (!changed && (!bitset_is_equal(&in, &minst->rd_in) || !bitset_is_equal(&out, &minst->rd_out))) {
                changed = 1;
            }
        }
    }

    bitset_uninit(&in);
    bitset_uninit(&out);

    return 0;
}

int                 minst_blk_value_numbering(struct minst_blk *blk)
{
    return 0;
}

int                 minst_blk_copy_propagation(struct minst_blk *blk)
{
    int i, direct;
    struct minst *minst, *def_minst, *pred;
    struct bitset defs;

    bitset_init(&defs, blk->allinst.len + 1);

    for (i = 0; i < blk->allinst.len; i++) {
        minst = blk->allinst.ptab[i];
        if (minst->flag.dead_code) continue;
        if (minst->flag.is_mov_reg) {
            bitset_clone(&defs, &minst->rd_in);
            bitset_and(&defs, &blk->defs[minst_get_use(minst)]);

            if (bitset_count(&defs) != 1)
                continue;
            
            def_minst = blk->allinst.ptab[bitset_next_bit_pos(&defs, 0)];
            if (!def_minst->flag.is_mov_reg)
                continue;

            /* FIX by value numbering */
            pred = minst->preds.minst;
            for (direct = 0; pred; pred = pred->preds.minst) {
                if (minst_get_def(pred) == minst_get_def(minst)) break;
                if (pred->preds.next) break;

                if (pred == def_minst)
                    direct = 1;
            }

            if (!direct)
                continue;

            if ((minst_get_def(def_minst) == minst_get_use(minst))
                && (minst_get_use(def_minst) == minst_get_def(minst))) {
                minst->flag.dead_code = 1;
            }
        }
    }

    bitset_uninit(&defs);

    return 0;
}

struct minst*       minst_get_last_const_definition(struct minst_blk *blk, struct minst *minst, int regm)
{
    int pos, count, imm;
    BITSET_INIT(bs);
    struct minst *const_minst = NULL;

    bitset_clone(&bs, &minst->rd_in);
    bitset_and(&bs, &blk->defs[regm]);

    count = bitset_count(&bs);
    if (!count) goto exit;

    if (count == 1) {
        pos = bitset_next_bit_pos(&bs, 0);
        const_minst = blk->allinst.ptab[pos];
    } else {
        pos = bitset_next_bit_pos(&bs, 0);
        const_minst = blk->allinst.ptab[pos];
        if (!const_minst->flag.is_const) goto exit;

        imm = const_minst->ld_imm;
        for (; pos >= 0; pos = bitset_next_bit_pos(&bs, pos + 1)) {
            const_minst = blk->allinst.ptab[pos];
            if (!const_minst->flag.is_const || (const_minst->ld_imm != imm)) {
                bitset_uninit(&bs);
                return NULL;
            }
        }
    }
exit:
    bitset_uninit(&bs);
    return (const_minst && const_minst->flag.is_const) ? const_minst : NULL;
}

int                 minst_blk_is_on_start_unique_path(struct minst_blk *blk, struct minst *def, struct minst *use)
{
    int i = 0;
    struct minst *start = blk->allinst.ptab[0];
    struct bitset path;

    bitset_init(&path, blk->allinst.len);

    while (start->succs.minst) {
        if (start->succs.next || bitset_get(&path, start->id)) {
            bitset_uninit(&path);
            return 0;
        }

        if (start->succs.minst == def)
            i = 1;

        if (start->succs.minst == use) {
            bitset_uninit(&path);
            return i == 1;
        }

        bitset_set(&path, start->id, 1);
        start = start->succs.minst;
    }

    bitset_uninit(&path);

    return 0;
}

int       minst_get_last_def_in_cur_cfg_node(struct minst_blk *blk, struct minst *minst, int regm)
{
    return 0;
}

/*
对抗混淆用的多路径常量传播

@cfg[in]            cfg[n]节点中的任意一条指令，用这条指令来代表这个cfg节点
@pass[in]           我们假设某个cfg节点只有2条出边，先暂不考虑switch case导致的多(>2)后继节点的情况，
                    pass代表符合<cond>走入后继节点时，选择的方向
@regm[in]           检测使用的寄存器
@d[out]             检测到的从符合Pass条件的走出的分支入边节点
@id[out]            和d相反 
@return 0           成功
        <0          失败
*/
int                 minst_blk_get_all_branch_reg_const_def(struct minst_blk *blk, struct minst *cfg, int pass, int reg_use, struct dynarray *d, struct dynarray *id)
{
    BITSET_INIT(allvisit);
    BITSET_INIT(iallvisit);
    struct bitset defs;
    struct minst *succ = cfg, *start;
    struct minst *pass_minst, *not_pass_minst;
    struct minst_node *succ_node;

    bitset_init(&defs, blk->allinst.len + 1);
    bitset_clone(&defs, &cfg->rd_in);
    bitset_and(&defs, &blk->defs[reg_use]);

    /* 找到当前cfg的后继出口 */
    for (; succ; succ = succ->succs.minst) {
        if (succ->succs.next) {
            if ((succ->succs.minst->id == (succ->id + 1))) {
                pass_minst = succ->succs.next->minst;
                not_pass_minst = succ->succs.minst;
            }
            else {
                not_pass_minst = succ->succs.next->minst;
                pass_minst = succ->succs.minst;
            }

            start = pass ? pass_minst : not_pass_minst;
            break;
        }
    }

    struct minst *stack[128];
    int stack_top = -1, ret = 0;

#define MSTACK_IS_EMPTY(s)      (s##_top == -1)
#define MSTACK_TOP(s)           s[s##_top]
#define MSTACK_POP(s)           s[s##_top--]
#define MSTACK_PUSH(s, e)       s[++s##_top] = e

    bitset_init(&allvisit, blk->allinst.len + 1);
    bitset_init(&iallvisit, blk->allinst.len + 1);
    MSTACK_PUSH(stack, start);
    bitset_set(&allvisit, start->id, 1);

    /* 就是一个广度优先得搜索，并做了一些特殊处理  */
    while (!MSTACK_IS_EMPTY(stack)) {
        start = MSTACK_POP(stack);

        for (succ_node = &start->succs; succ_node; succ_node = succ_node->next) {
            if (!(succ = succ_node->minst)) continue;

            if (bitset_get(&allvisit, succ->id)) continue;
            if (bitset_get(&defs, succ->id)) {
                if (succ->flag.is_const) {
                    dynarray_add(d, succ);
                    bitset_set(&allvisit, succ->id, 1);
                    continue;
                }
                ret = -1; goto exit;
            }

            MSTACK_PUSH(stack, succ);
            bitset_set(&allvisit, succ->id, 1);
        }
    }

    /* 遍历另外分支的节点 */
    start = pass ? not_pass_minst:pass_minst;
    if (bitset_get(&allvisit, start->id)) {
        ret = -1; goto exit;
    }
    MSTACK_PUSH(stack, start);
    bitset_set(&iallvisit, start->id, 1);

    while (!MSTACK_IS_EMPTY(stack)) {
        start = MSTACK_POP(stack);
        for (succ_node = &start->succs; succ_node; succ_node = succ_node->next) {
            if (!(succ = succ_node->minst)) continue;
            /* 这条边已经访问过，退出 */
            if (bitset_get(&iallvisit, succ->id)) continue;

            if (bitset_get(&allvisit, succ->id) && minst_is_bidirect(start, succ)) {
                ret = -1; goto exit;
            }

            if (bitset_get(&defs, succ->id)) {
                dynarray_add(id, succ);
                bitset_set(&iallvisit, succ->id, 1);
                continue;
            }

            MSTACK_PUSH(stack, succ);
            bitset_set(&allvisit, succ->id, 1);
        }
    }

exit:
    bitset_uninit(&allvisit);
    bitset_uninit(&iallvisit);
    bitset_uninit(&defs);
    return ret;
}

