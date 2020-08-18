
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

struct minst_blk*   minst_blk_new(char *funcname, unsigned char *code, int code_len)
{
    struct minst_blk *blk;

    blk = (struct minst_blk *)calloc(1, sizeof (blk[0]));
    if (NULL == blk)
        vm_error("minst_blk_new() calloc failure");

    minst_blk_init(blk, funcname, NULL, NULL);

    return blk;
}

void                minst_blk_delete(struct minst_blk *blk)
{
    if (!blk)   return;

    minst_blk_uninit(blk);

    free(blk);
}

void                minst_blk_init(struct minst_blk *blk, char *funcname, minst_parse_callback callback, void *emu)
{
    memset(blk, 0, sizeof (blk[0]));

    blk->funcname = strdup(funcname);

    blk->allinst.cmp = minst_cmp;

    blk->trace_top = -1;

    blk->minst_do = callback;

    blk->emu = emu;
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
    dynarray_reset(&blk->allinst);

    for (i = 0; i < blk->allcfg.len; i++) {
        minst_cfg_delete(blk->allcfg.ptab[i]);
    }
    dynarray_reset(&blk->allcfg);

    memset(blk, 0, sizeof (blk[0]));
}

void                minst_blk_add_funcend(struct minst_blk *blk, struct minst *m)
{
    m->flag.funcend = 1;
    bitset_set(&blk->funcends, m->id, 1);
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

struct minst*       minst_new_copy(struct minst_cfg *cfg, struct minst *src)
{
    struct minst_blk *blk = cfg->blk;
    struct minst *dst = minst_new(blk, NULL, 0, NULL);

    dst->addr = blk->text_sec.data + blk->text_sec.len;
    blk->text_sec.len += src->len;

    dst->len = src->len;
    dst->reg_node = src->reg_node;
    bitset_clone(&dst->def, &src->def);
    bitset_clone(&dst->use, &src->use);
    dst->flag.is_const = src->flag.is_const;
    dst->ld_imm = src->ld_imm;
    dst->cfg = cfg;
    dst->copy_from = src;
    dst->type = src->type;
    dst->temp = src->temp;
    memcpy(dst->addr, src->addr, src->len);

    if (!cfg->start) cfg->start = dst;
    else minst_add_edge(cfg->end, dst);
    cfg->end = dst;

    return dst;
}

struct minst*       minst_new_t(struct minst_cfg *cfg, enum minst_type type, void *reg_node, unsigned char *code, int len)
{
    struct minst_blk *blk = cfg->blk;
    struct minst *minst = minst_new(blk, NULL, 0, NULL);

    minst->addr = blk->text_sec.data + blk->text_sec.len;
    minst->len = len;
    blk->text_sec.len += len;
    memcpy(minst->addr, code, len);

    minst->type = type;
    minst->reg_node = reg_node;
    minst->cfg = cfg;

    if (!cfg->start) cfg->start = minst;
    else minst_add_edge(cfg->end, minst);
    cfg->end = minst;

    return minst;
}

struct minst*       minst_change(struct minst *minst, enum minst_type type, void *reg_node, unsigned char *code, int len)
{
    struct minst_blk *blk = minst->cfg->blk;
    minst->addr = blk->text_sec.data + blk->text_sec.len;

    minst->addr = blk->text_sec.data + blk->text_sec.len;
    minst->len = len;
    blk->text_sec.len += len;
    memcpy(minst->addr, code, len);

    minst->type = type;
    minst->reg_node = reg_node;

    return minst;
}

void                minst_restore(struct minst *minst)
{
    minst->flag.is_trace = 0;

    if (minst->flag.is_const) return;

    minst->flag.b_cond_passed = 0;
}

int                 minst_succs_count(struct minst *minst)
{
    int i;
    struct minst_node *succ_node = &minst->succs;

    for (i = 0; succ_node; succ_node = succ_node->next) {
        if (succ_node->minst) i++;
    }

    return i;
}

int                 minst_preds_count(struct minst *minst)
{
    int i;
    struct minst_node *pred_node;

    if (!minst) return 0;

    pred_node = &minst->preds;

    for (i = 0; pred_node; pred_node = pred_node->next) {
        if (pred_node->minst) i++;
    }

    return i;
}

struct minst_cfg*   minst_cfg_new(struct minst_blk *blk, struct minst *start, struct minst *end)
{
    struct minst_cfg *cfg = calloc(1, sizeof (cfg[0]));

    if (NULL == cfg)
        vm_error("minst_cfg_new() failure");

    cfg->id = blk->allcfg.len;
    cfg->start = start;
    cfg->end = start;
    cfg->blk = blk;
    dynarray_add(&blk->allcfg, cfg);

    return cfg;
}

void                minst_cfg_delete(struct minst_cfg *cfg)
{
    free(cfg);
}

struct minst*       minst_blk_find(struct minst_blk *blk, unsigned long addr)
{
    return dynarray_find(&blk->allinst, (void *)addr);
}

void                minst_succ_add(struct minst *minst, struct minst *succ, int truel)
{
    struct minst_node *tnode;
    if (!minst || !succ)
        return;

    minst_succs_foreach(minst, tnode) {
        if (tnode->minst && (tnode->f.true_label == truel))
            vm_error("minst[%d] already have %s label", minst->id, truel?"true":"false");
    }

    if (!minst->succs.minst) {
        tnode = &minst->succs;
    }
    else {
        tnode = calloc(1, sizeof (tnode[0]));
        if (!tnode)
            vm_error("minst_succ_add() calloc failure");

        tnode->next = minst->succs.next;
        minst->succs.next = tnode;
    }

    tnode->minst = succ;
    tnode->f.true_label = truel;
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
            vm_error("minst_pred_add() calloc failure");

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
            for (; succ_node->next; prev_node = succ_node, succ_node = succ_node->next) {
                succ_node->minst = succ_node->next->minst;
                succ_node->f = succ_node->next->f;
            }

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

void                minst_del_from_cfg(struct minst *minst)
{
    struct minst_cfg *cfg = minst->cfg;
    struct minst_node *pred_node, *succ_node;
    struct minst *pred, *succ = minst->succs.minst;
    int inst_count = minst_cfg_inst_count(cfg);

    if (minst_succs_count(minst) > 1)
        vm_error("cant delete succs > 1 minst[%d]\n", minst->id);

    if (minst->flag.dead_code)
        vm_error("minst[%d] already be delete", minst->id);

    if (inst_count == 1) {
        cfg->flag.dead_code = 1;
        cfg->start = NULL;
        cfg->end = NULL;
    }
    else if (cfg->start == minst){
        cfg->start = minst->succs.minst;
    }
    else if (cfg->end == minst) {
        cfg->end = minst->preds.minst;
    }

    /* 删除一个节点以后，需要把这个节点的所有前驱节点，连接到他的后继节点以后 */
    minst_preds_foreach(minst, pred_node) {
        if (!(pred = pred_node->minst)) continue;
        minst_succs_foreach(pred, succ_node) {
            if (succ_node->minst == minst) {
                succ_node->minst = succ;
                minst_pred_add(succ, pred);
                break;
            }
        }
    }

    while ((pred = minst->preds.minst)) {
        minst_pred_del(minst, pred);
    }

    minst_del_edge(minst, succ);

    /* 打上dead_code的标志 */
    minst->flag.dead_code = 1;
}

void                minst_replace_edge(struct minst *from, struct minst *to, struct minst *rep)
{
    struct minst_node *succ_node;

    minst_succs_foreach(from, succ_node) {
        if (succ_node->minst == to) {
            succ_node->minst = rep;
        }
    }

    minst_pred_del(to, from);
    minst_pred_add(rep, from);
}

void                minst_blk_gen_cfg(struct minst_blk *blk)
{
    int i, start = 0;
    struct minst *minst, *cur_grp_minst, *prev_minst;
    struct minst_cfg *cfg;

    for (i = 0; i < blk->allinst.len; i++) {
        minst = blk->allinst.ptab[i];

        if (!start) {
            cur_grp_minst = minst;
            start = 1;
            cfg = minst_cfg_new(blk, minst, NULL);
            cfg->flag.prologue = 1;
        }
        /* 1. 物理前指令是跳转指令，自动切块
           2. 当前节点有多个前节点 
           3. 前节点不是物理前节点 */
        else if (
            //!minst->flag.in_it_block && !prev_minst->flag.in_it_block && 
            (minst_is_b0(prev_minst) 
                || prev_minst->succs.next 
                || minst->preds.next 
                || minst->preds.minst != blk->allinst.ptab[i - 1])
                || (prev_minst->flag.prologue != minst->flag.prologue)
                || (prev_minst->flag.epilogue != minst->flag.epilogue)) {
            cfg = minst_cfg_new(blk, minst, NULL);
            cur_grp_minst = minst;

            if (minst->flag.epilogue) cfg->flag.epilogue = 1;
        }

        minst->cfg = cfg;
        cfg->end = minst;
        prev_minst = minst;
    }

    cfg->end = prev_minst;
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

static int arm_pro_def_reglist[] = {
    ARM_REG_R0,
    ARM_REG_R1,
    ARM_REG_R2,
    ARM_REG_R3,
    ARM_REG_R13,
    ARM_REG_R14,
    ARM_REG_R15,
};

static int arm_epi_use_reglist[] = {
    ARM_REG_R0,
    ARM_REG_R1,
    ARM_REG_R13,
    ARM_REG_R14,
    ARM_REG_R15,
};

void                minst_blk_live_prologue_add(struct minst_blk *mblk, int sp_val)
{
    int i;
    struct minst *minst = minst_new(mblk, NULL, 0, NULL);

    minst->flag.prologue = 1;

    for (i = 0; i < 16; i++) {
        live_def_set(mblk, i);
    }

    minst = minst_new(mblk, NULL, 0, NULL);
    minst->flag.prologue = 1;
    minst->flag.is_const = 1;
    minst->ld_imm = sp_val;
    live_def_set(mblk, ARM_REG_SP);
}

void                minst_blk_live_epilogue_add(struct minst_blk *blk)
{
    int i, j, len;
    struct minst *minst = blk->allinst.ptab[blk->allinst.len - 1];
    struct minst *m, *succ;

    if (!minst->flag.funcend) {
        minst_blk_add_funcend(blk, minst);
    }

    for (i = 0; i < blk->allinst.len; i++) {
        m = blk->allinst.ptab[i];
        if (m->flag.prologue) {
            minst_add_edge(m, blk->allinst.ptab[i+1]);
            continue;
        }
        break;
    }

    bitset_foreach(&blk->funcends, i) {
        m = blk->allinst.ptab[i];
        /* 给funcend节点，添加epilogue时，需要删除他和以前的后继节点的关联*/
        if ((succ = m->succs.minst))
            minst_del_edge(m, succ);

        minst = minst_new(blk, NULL, 0, NULL);
        minst->addr = blk->text_sec.data + blk->text_sec.len;
        blk->text_sec.len += 1;
        minst->len = 1;

        minst->flag.epilogue = 1;

        live_use_set(blk, ARM_REG_R0);
        live_use_set(blk, ARM_REG_SP);

        len = blk->allinst.len;

        minst_add_edge(m, minst);

        if (m->type == mtype_bl) {
            bitset_clear(&m->use);
        }

        /* pop 指令非常特殊 */
        while (m->type == mtype_pop) {
            bitset_foreach(&m->def, j) {
                live_use_set(blk, j);
            }

            m = m->preds.minst;
        }
    }
}


int                 minst_blk_liveness_calc(struct minst_blk *blk)
{
    struct minst_node *succ;
    struct minst *minst;
    struct bitset in = { 0 }, out = { 0 }, use = {0};
    int i, changed = 1;

    for (i = blk->allinst.len - 1; i >= 0; i--) {
        minst = blk->allinst.ptab[i];

        bitset_clear(&minst->in);
        bitset_clear(&minst->out);
    }

    while (changed) {
        changed = 0;
        for (i = blk->allinst.len - 1; i >= 0; i--) {
            minst = blk->allinst.ptab[i];
            if (minst->flag.dead_code || (minst->cfg && minst->cfg->flag.dead_code)) continue;

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
    struct minst_cfg *cfg;
    struct minst *minst;
    int changed = 1, i, ret = 0;
    struct bitset def = { 0 };

    /* FIXME: 删除死代码以后，liveness的计算没有把死代码计算进去 */
    while (changed) {
        changed = 0;
        for (i = 0; i < blk->allinst.len; i++) {
            minst = blk->allinst.ptab[i];
            /* 
            1. 死代码删除只在四元式中进行，函数不参与死代码删除计算
            2. str指令不参与计算，因为有很强的副作用 */
            if (minst->flag.dead_code || minst->flag.prologue || minst->flag.epilogue 
                || (minst->type == mtype_bl)
                || (minst->type == mtype_str)
                || (minst->type == mtype_ldr))
                continue;

            /* 四元式一定有def的，没有def的指令一般是 bl, it, cmp等等*/
            if (bitset_is_empty(&minst->def))
                continue;

            bitset_clone(&def, &minst->def);
            if (!changed && bitset_is_empty(bitset_and(&def, &minst->out))) {
                minst_del_from_cfg(minst);
                //printf("dead code elim, inst[%d]\n", minst->id);
                ret = changed = 1;
            }
        }

        minst_blk_liveness_calc(blk);
    }

    /* 删除一个节点且这个节点就是跳转指令的的cfg*/
    for (i = 0; i < blk->allcfg.len; i++) {
        cfg = blk->allcfg.ptab[i];
        if (cfg->flag.dead_code) continue;
        if (cfg->start != cfg->end) continue;
        if (cfg->start->type != mtype_b) continue;
        if ((minst_preds_count(cfg->start) == 1) && (minst_succs_count(cfg->end) == 1)) {
            minst_del_from_cfg(cfg->start);
        }
    }

    bitset_uninit(&def);

    return ret;
}

int                 minst_get_def(struct minst *minst)
{
    if (minst->ld == -1)
        return minst->ld;

    return minst->ld = bitset_next_bit_pos(&minst->def, 0);
}

int                 minst_get_use(struct minst *minst)
{
    int pos = bitset_next_bit_pos(&((minst)->use), 0);

    if (pos != ARM_REG_APSR) return pos;

    int pos1 = bitset_next_bit_pos(&((minst)->use), pos + 1);

    if (pos1 != -1) return pos1;

    return pos;
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

        bitset_clear(&minst->rd_in);
        bitset_clear(&minst->rd_out);
        bitset_clear(&minst->kills);

        if (minst->flag.epilogue || minst->flag.dead_code || minst->cfg->flag.dead_code || (def = minst_get_def(minst)) < 0)
            continue;
        bitset_clone(&minst->kills, &blk->defs[def]);
        bitset_set(&minst->kills, minst->id, 0);
    }

    while (changed) {
        changed = 0;

        for (i = 0; i < blk->allinst.len; i++) {
            minst = blk->allinst.ptab[i];
            if (minst->flag.epilogue || minst->flag.dead_code || minst->cfg->flag.dead_code)
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
    int i, use, changed = 1, ret = 0;
    struct minst *m, *def_m;
    BITSET_INITS(defs, blk->allinst.len);

    while (changed) {
        changed = 0;
        for (i = 0; i < blk->allinst.len; i++) {
            m = blk->allinst.ptab[i];
            if ((m->type == mtype_mov_reg) || (m->type == mtype_ldr)) {
                use = minst_get_use(m);
                bitset_clone(&defs, &blk->defs[use]);
                bitset_and(&defs, &m->rd_in);

                if ((bitset_count(&defs) == 1)) {
                    def_m = blk->allinst.ptab[bitset_1th(&defs)];
                    if (m->preds.minst != def_m) continue;

                    if ((m->type == mtype_mov_reg) && (def_m->type == mtype_mov_reg)
                        && (minst_get_def(m) == minst_get_use(def_m)) && (minst_get_use(m) == minst_get_def(def_m))) {
                        minst_del_from_cfg(m);
                        ret = changed = 1; break;
                    }

                    if ((m->type == mtype_ldr) && (def_m->type == mtype_str)
                        && (minst_get_def(m) == minst_get_use(def_m)) && (minst_get_use(m) == minst_get_def(def_m))) {
                        minst_del_from_cfg(m);
                        ret = changed = 1;  break;
                    }
                }
            }
        }

        minst_blk_liveness_calc(blk);
        minst_blk_gen_reaching_definitions(blk);
    }

    bitset_uninit(&defs);
    return ret;
}

int                 minst_blk_regalloc(struct minst_blk *blk)
{
    return 0;
}

int                 minst_conditional_const_propagation(struct minst_blk *blk)
{
    struct minst_cfg *cfg;
    int i;

    for (i = 0; i < blk->allcfg.len; i++) {
        cfg = blk->allcfg.ptab[i];
    }
    return 0;
}

struct minst*       minst_get_last_const_definition(struct minst_blk *blk, struct minst *minst, int regm)
{
    int pos, count, imm, i, j;
    BITSET_INIT(bs);
    BITSET_INIT(bs2);
    struct minst *const_minst = NULL, *t, *n, *cm = NULL;

    bitset_clone(&bs, &minst->rd_in);
    bitset_and(&bs, &blk->defs[regm]);

    count = bitset_count(&bs);
    if (!count) goto exit;

    if (count == 1) {
        pos = bitset_next_bit_pos(&bs, 0);
        cm  = blk->allinst.ptab[pos];
    } else {
        pos = bitset_next_bit_pos(&bs, 0);

        bitset_foreach(&bs, pos) {
            const_minst = blk->allinst.ptab[pos];
            if (const_minst->flag.is_const)
                break;
        }

        if (!const_minst->flag.is_const) goto fail_label;

        cm = const_minst;
        imm = const_minst->ld_imm;

        bitset_foreach(&bs, pos) {
            const_minst = blk->allinst.ptab[pos];
            if (!const_minst->flag.is_const) {
                if (const_minst->type != mtype_mov_reg) {
                    bitset_uninit(&bs);
                    return NULL;
                }

                int use = minst_get_use(const_minst);
                bitset_clear(&bs2);
                bitset_clone(&bs2, &blk->defs[use]);
                bitset_and(&bs2, &const_minst->rd_in);
                bitset_foreach(&bs2, i) {
                    t = blk->allinst.ptab[i];

                    if ((t->type != mtype_mov_reg) || (minst_get_use(t) != regm))  goto fail_label;

                    bitset_foreach(&bs, j) {
                        n = blk->allinst.ptab[j];
                        if (!n->flag.is_const && minst_get_use(n) == use) continue;
                        if (n->flag.is_const && (n->ld_imm == imm)) continue;

                        goto fail_label;
                    }
                }
            }
            else if (const_minst->ld_imm != imm) {
                goto fail_label;
            }
        }
    }
exit:
    bitset_uninit(&bs);
    bitset_uninit(&bs2);
    return (cm && cm->flag.is_const) ? cm : NULL;

fail_label:
    bitset_uninit(&bs);
    bitset_uninit(&bs2);
    return NULL;
}

struct minst*       minst_get_last_def(struct minst_blk *blk, struct minst *minst, int regm)
{
    int pos, count;
    BITSET_INIT(bs);
    struct minst *def_minst = NULL;

    bitset_clone(&bs, &minst->rd_in);
    bitset_and(&bs, &blk->defs[regm]);

    count = bitset_count(&bs);
    if (!count) return NULL;

    if (count == 1) {
        pos = bitset_next_bit_pos(&bs, 0);
        def_minst = blk->allinst.ptab[pos];
    }

    return def_minst;
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

int                 minst_blk_out_of_order(struct minst_blk *blk)
{
    struct minst_cfg *cfg;
    struct minst *minst, *cminst;
    struct minst_node *succ_node;
    struct bitset live_out, tmp_in;
    int i, pos;

    bitset_init(&live_out, blk->allinst.len);
    bitset_init(&tmp_in, blk->allinst.len);

    for (i = 0; i < blk->allcfg.len; i++) {
        cfg = blk->allcfg.ptab[i];

        /* 不计算单节点的乱序情况 */
        if (!cfg->end->succs.next) continue;
        /* 不处理it指令导致的跳转 */
        if (!minst_is_b0(cfg->end)) continue;

        bitset_clone(&live_out, &cfg->end->out);
        bitset_sub(&live_out, &cfg->start->in);

        bitset_foreach(&live_out, pos) {
            for (succ_node = &cfg->end->succs; succ_node; succ_node = succ_node->next) {
                if (!(minst = succ_node->minst)) continue;

                /* 可以乱序的条件
                1. 不在跳转cfg的入口活跃节点中 
                2. 是常量
                3. 在当前cfg内被定义过，但从没在定义处到当前cfg末尾处使用过
                */
                if (!bitset_get(&minst->in, pos) && (cminst = minst_get_last_const_definition(blk, cfg->end, pos)))
                    printf("minst[%d] reg:r%d = 0x%x need out of order\n", cfg->end->id, pos, cminst->ld_imm);
            }
        }
    }

    bitset_uninit(&tmp_in);
    bitset_uninit(&live_out);

    return 0;
}

struct minst*       minst_trace_get_def(struct minst_blk *blk, int regm, int *index, int before)
{
    struct minst *m;
    int i;

    i = (before > 0) ? before : (before + blk->trace_top);

    for (; i >= 0; i--) {
        m = blk->trace[i];
        if (minst_get_def(m) == regm) {
            if (index) *index = i;
            return blk->trace[i];
        }
    }

    return minst_get_last_const_definition(blk, ((struct minst *)blk->trace[0])->cfg->end, regm);
    //return NULL;
}

int                 minst_trace_get_defs(struct minst_blk *blk, int regm, int before, struct bitset *defs)
{
    struct minst *m, *end, *m1;
    BITSET_INITS(all, blk->allinst.len);
    struct minst_node *succ_node;
    struct minst *stack[512];
    int stack_top = -1;

    before = (before > 0) ? before : (blk->trace_top + before);

    end = blk->trace[before];

    m = blk->trace[0];
    MSTACK_PUSH(stack, m);

    while (!MSTACK_IS_EMPTY(stack)) {
        m = MSTACK_POP(stack);
        bitset_set(&all, m->id, 1);
        if (m == end) continue;

        minst_succs_foreach(m, succ_node) {
            m1 = succ_node->minst;
            if (m1 && !bitset_get(&all, m1->id))
                MSTACK_PUSH(stack, m1);
        }
    }

    bitset_clear(defs);
    /* 取出所有对regm定值的语句 */
    bitset_clone(defs, &blk->defs[regm]);
    /* 在end入口点活跃的regm的定制语句*/
    bitset_and(defs, &end->rd_in);
    if (regm == 9) {
        bitset_dump(&blk->defs[regm]);
        bitset_dump(defs);
        //bitset_dump(&all);
    }
    /* 在当前trace流中活跃的regm定值语句 */
    bitset_and(defs, &all);

    return bitset_count(defs);
}

struct minst*       minst_trace_get_str(struct minst_blk *blk, long memaddr, int before)
{
    int i;
    struct minst *m;

    i = before > 0 ? before : (blk->trace_top + before);
    for (; i >= 0; i--) {
        m = blk->trace[i];
        if ((m->type == mtype_str) && m->temp && (m->temp->addr == memaddr))
            return m;
    }

    return NULL;
}

struct minst_cfg*   minst_trace_find_prev_cfg(struct minst_blk *blk, int *index, int before)
{
    struct minst *minst;
    struct minst_cfg *cfg;

    if (!before) before = blk->trace_top;

    cfg = ((struct minst *)blk->trace[before])->cfg;

    for (; before >= 0; before--) {
        minst = blk->trace[before];
        if (minst->cfg != cfg) {
            if (index) *index = before;
            return minst->cfg;
        }
    }

    return NULL;
}

struct minst*       minst_trace_find_prev_undefined_bcond(struct minst_blk *blk, int *index, int before)
{
    struct minst *minst;
    int i, k = 0;

    i = (before > 0) ? before : (blk->trace_top + before);

    for (; i >= 0; i--) {
        minst = blk->trace[i];
        if (minst_succs_count(minst) > 1) {
            if (minst->flag.undefined_bcond || !minst_is_tconst(minst)) {
                *index = i;
                return minst;
            }
        }
    }

    return NULL;
}

struct minst*       minst_trace_find_prev_bcond(struct minst_blk *blk, int *index, int before)
{
    struct minst *minst;
    int i;

    i = (before > 0) ? before : (blk->trace_top + before);

    for (; i >= 0; i--) {
        minst = blk->trace[i];
        if (minst_succs_count(minst) > 1) {
            *index = i;
            return minst;
        }
    }

    return NULL;
}

struct minst*       minst_cfg_get_last_def(struct minst_cfg *cfg, struct minst *minst, int reg_def)
{
    for (; minst; minst = minst->preds.minst) {
        if (minst_get_def(minst) == reg_def) return minst;

        if (minst == cfg->start) break;
    }

    return NULL;
}

int                 minst_cfg_is_csm(struct minst_cfg *cfg)
{
    return minst_preds_count(cfg->start) >= 7;
}

int                 minst_cfg_classify(struct minst_blk *blk)
{
    struct minst *m;
    struct minst_cfg *cfg, *tcfg, *pcfg;
    struct minst_cfg *stack[256];
    struct minst_node *succ_node, *pred_node;
    int stack_top = -1;
    int i;

    BITSET_INITS(visitall, blk->allcfg.len);

    cfg = blk->csm.cfg;
    printf("csm[%d:%d-%d]\n", cfg->id, cfg->start->id, cfg->end->id);
    MSTACK_PUSH(stack, cfg);
    bitset_set(&visitall, cfg->id, 1);
    while (!MSTACK_IS_EMPTY(stack)) {
        cfg = MSTACK_POP(stack);

        minst_succs_foreach(cfg->end, succ_node) {
            if (!succ_node->minst) continue;
            tcfg = (succ_node->minst->cfg);
            if (!tcfg || tcfg->flag.dead_code) continue;
            if (bitset_get(&visitall, tcfg->id)) continue;

            MSTACK_PUSH(stack, tcfg);
            bitset_set(&visitall, tcfg->id, 1);
        }
    }

    bitset_foreach(&visitall, i) {
        cfg = blk->allcfg.ptab[i];
        cfg->csm = CSM;
    }

    bitset_not(&visitall);
    bitset_foreach(&visitall, i) {
        cfg = blk->allcfg.ptab[i];
        cfg->csm = CSM_IN;
    }

    /* 扫描不可达csm的节点，逆向扫描
    
    1. 从epilogue的前一个节点开始，标记为不可达csm节点,入栈
    2. 弹出栈顶节点，假如栈顶节点的所有前序节点的所有后续节点都为不可达节点，则入栈，持续2
    */
    bitset_clear(&visitall);
    bitset_foreach(&blk->funcends, i) {
        m = blk->allinst.ptab[i];
        MSTACK_PUSH(stack, m->cfg);
        bitset_set(&visitall, m->cfg->id, 1);
    }
    while (!MSTACK_IS_EMPTY(stack)) {
        cfg = MSTACK_POP(stack);
        if (cfg->id == 0) continue;

        minst_preds_foreach(cfg->start, pred_node) {
            if (!pred_node->minst) continue;
            pcfg = pred_node->minst->cfg;

            if (bitset_get(&visitall, pcfg->id)) continue;

            minst_succs_foreach(pcfg->end, succ_node) {
                tcfg = succ_node->minst->cfg;
                if (!bitset_get(&visitall, tcfg->id)) break;
            }

            if (!succ_node) {
                MSTACK_PUSH(stack, pcfg);
                bitset_set(&visitall, pcfg->id, 1);
            }
        }
    }

    bitset_foreach(&visitall, i) {
        cfg = blk->allcfg.ptab[i];
        cfg->csm = CSM_OUT;
    }

    return 0;
}

int                 minst_cfg_inst_count(struct minst_cfg *cfg)
{
    struct minst *minst;
    int cts = 0;

    for (minst = cfg->start; minst != cfg->end; minst = minst->succs.minst) {
        if (minst->flag.dead_code) continue;
        cts++;
    }

    cts++;

    return cts;
}

struct minst* minst_cfg_apsr_get_overdefine_reg(struct minst_cfg *cfg, int *use_reg)
{
    struct minst_blk *blk = cfg->blk;
    struct minst *end = cfg->end, *pred, *minst;

    if (minst_succs_count(cfg->end) <= 1)
        return NULL;

    if (end->flag.is_const) return NULL;

    for (pred = end->preds.minst; pred; pred = pred->preds.minst) {
        if (pred->type == mtype_cmp)
            break;

        if (minst_preds_count(pred) != 1)
            return NULL;
    }
    
    if (pred->type != mtype_cmp)
        return NULL;

    struct minst *lm_minst = minst_get_last_const_definition(blk, pred, pred->cmp.lm);
    struct minst *ln_minst = minst_get_last_const_definition(blk, pred, pred->cmp.ln);

    if ((lm_minst && ln_minst) || (!lm_minst && !lm_minst))
        return NULL;

    int reg = lm_minst ? pred->cmp.ln:pred->cmp.lm;
    minst = minst_get_last_def(blk, pred, reg);
    if (NULL == minst) {
        *use_reg = reg;
        return minst;
    }

    if (minst->type != mtype_mov_reg)
        return NULL;

    if (use_reg)
        *use_reg = minst_get_use(minst);

    return minst;
}

int                 minst_bcond_symbo_exec(struct minst_cfg *cfg, struct minst *def_val)
{
    return 0;
}

int                 minst_blk_del_unreachable(struct minst_blk *blk)
{
    struct minst *succ;
    struct minst_node *succ_node;
    struct minst_cfg *stack[128], *cfg;
    int stack_top = -1, pos, changed = 0;
    BITSET_INITS(visit, blk->allcfg.len);

    MSTACK_PUSH(stack, blk->allcfg.ptab[0]);

    while (!MSTACK_IS_EMPTY(stack)) {
        cfg = MSTACK_POP(stack);
        bitset_set(&visit, cfg->id, 1);

        minst_succs_foreach(cfg->end, succ_node) {
            if (!(succ = succ_node->minst) || !succ->cfg) continue;
            if (bitset_get(&visit, succ->cfg->id)) continue;

            MSTACK_PUSH(stack, succ->cfg);
        }
    }

    /* 取反就是所有没访问过的节点 */
    bitset_not(&visit);
    bitset_foreach(&visit, pos) {
        cfg = blk->allcfg.ptab[pos];
        if (!cfg->flag.dead_code) {
            changed = 1;
            cfg->flag.dead_code = 1;
            printf("cfg[%d] is be deleted\n", cfg->id);
        }
    }

    return changed;
}

int                 minst_edge_clear_visited(struct minst_blk *blk)
{
    struct minst_node *succ_node;
    struct minst *m;
    int i;

    for (i = 0; i < blk->allinst.len; i++) {
        m = blk->allinst.ptab[i];
        minst_succs_foreach(m, succ_node) {
            succ_node->f.visited = 0;
        }
    }

    return 0;
}

int                 minst_edge_set_visited(struct minst *from, struct minst *to)
{
    struct minst_node *succ_node;

    minst_succs_foreach(from, succ_node) {
        if (succ_node->minst == to) {
            succ_node->f.visited = 1;
        }
    }

    return 0;
}

struct minst_temp * minst_temp_alloc(struct minst_blk *blk, unsigned long addr)
{
    struct minst_temp *temp;

    if ((temp = minst_temp_get(blk, addr))) return temp;

    temp = calloc(1, sizeof (temp[0]));
    if (!temp)
        vm_error("minst_temp_alloc() alloc failure");

    temp->addr = addr;
    /* 前32个是系统保留寄存器变量 */
    temp->tid = (blk->tvar_id++) + 32;
    dynarray_add(&blk->tvar, temp);

    return temp;
}

struct minst_temp * minst_temp_get(struct minst_blk *blk, unsigned long addr)
{
    struct minst_temp *temp;
    int i;

    for (i = 0; i < blk->tvar.len; i++) {
        temp = blk->tvar.ptab[i];
        if (temp->addr == addr)
            return temp;
    }

    return NULL;
}

void    minst_dump_defs(struct minst_blk *blk, int inst_id, int reg_def)
{
    struct minst *minst, *def_minst;
    BITSET_INIT(defs);
    int pos;

    if (inst_id >= blk->allinst.len)
        return;

    minst = blk->allinst.ptab[inst_id];

    bitset_clone(&defs, &blk->defs[reg_def]);
    bitset_and(&defs, &minst->rd_in);

    printf("[inst_id:%d] %s def list\n", inst_id, arm_reg2str(reg_def));
    bitset_foreach(&defs, pos) {
        def_minst = blk->allinst.ptab[pos];
        if (def_minst->flag.is_const)
            printf("%d const=0x%x\n", pos, def_minst->ld_imm);
        else if (def_minst->type == mtype_def)
            printf("%d is type_def, skipped\n", pos);
        else
            printf("%d unknown\n", pos);
    }
    printf("\n");

    bitset_uninit(&defs);
}

int minst_dob_analyze(struct minst_blk *blk)
{
    int i, count, max = -1;
    struct minst_cfg *cfg, *csm_cfg;
    struct minst *cmp, *m;

    blk->csm.st_reg = -1;
    blk->csm.save_reg = -1;

    /* FIXME: 现在选择有最大前驱节的cfg做为csm.cfg */
    for (i = 0; i < blk->allcfg.len; i++) {
        cfg = blk->allcfg.ptab[i];
        if (((count = minst_preds_count(cfg->start)) >= 7) && (count > max)) {
            csm_cfg = cfg;
            max = count;
        }
    }

    if (!csm_cfg || (csm_cfg->end->type != mtype_bcond))
        vm_error("not found const state machine");

    blk->csm.cfg = csm_cfg;

    cmp = minst_cfg_get_last_def(csm_cfg, csm_cfg->end, ARM_REG_APSR);

    blk->csm.base_reg = cmp->cmp.lm;
    blk->csm.st_reg = cmp->cmp.ln;

    m = minst_cfg_get_last_def(csm_cfg, cmp, blk->csm.st_reg);
    if (!m || (m->type != mtype_mov_reg)) {
        printf("csm[%d] not found save reg\n", cmp->id);
    }
    else
        blk->csm.save_reg = minst_get_use(m);

    blk->csm.trace_reg = (blk->csm.save_reg == -1) ? blk->csm.st_reg : blk->csm.save_reg;

    return 1;
}

int minst_dump_csm(struct minst_blk *blk)
{
    BITSET_INIT(defs);
    struct minst *m;
    int i;

    bitset_clone(&defs, &blk->defs[blk->csm.trace_reg]);
    bitset_and(&defs, &blk->csm.cfg->start->rd_in);

    printf("csm[%d] base_reg[r%d] st_reg[r%d] save_reg[%d]\n", 
        blk->csm.cfg->id, blk->csm.base_reg, blk->csm.st_reg, blk->csm.save_reg);

    bitset_foreach(&defs, i) {
        m = blk->allinst.ptab[i];
        if (m->flag.is_const) {
            if (m->ld_imm > 0)
                printf("minist[%d] = 0x%x\n", m->id, m->ld_imm);
            else
                printf("minist[%d] = %d\n", m->id, m->ld_imm);
        }
        else if (m->type == mtype_mov_reg) {
            if (minst_get_use(m) != blk->csm.st_reg) {
                minst_dump_defs(blk, m->id, minst_get_use(m));
            }
        }
        else if (m->type == mtype_ldr){
            minst_dump_defs(blk, m->id, minst_get_use(m));
        }
        else {
            printf("Not support csm assign minst[%d]\n", m->id);
            //vm_error("Not support csm assign minst[%d]", m->id);
        }
    }

    return 0;
}

int minst_get_all_const_definition(struct minst_blk *blk, struct minst *m, struct dynarray *d)
{
    struct minst *stack[128], *t, *t2;
    int stack_top = -1, i, use;
    BITSET_INIT(defs);
    BITSET_INIT(rlist);

    dynarray_reset(d);

    MSTACK_PUSH(stack, m);

    while (!MSTACK_IS_EMPTY(stack)) {
        t = MSTACK_POP(stack);
        if (t->flag.is_const) {
            for (i = 0; i < d->len;i ++) {
                if (((struct minst *)d->ptab[i])->ld_imm == t->ld_imm)
                    break;
            }

            if (i == d->len)
                dynarray_add(d, t);
            continue;
        }

        switch (t->type) {
        case mtype_mov_reg:
        case mtype_ldr:
        case mtype_str:
            use = minst_get_use(t);
            bitset_clone(&defs, &blk->defs[use]);
            bitset_and(&defs, &t->rd_in);

            bitset_foreach(&defs, i) {
                t2 = blk->allinst.ptab[i];
                MSTACK_PUSH(stack, t2);
            }
            break;

        case mtype_def:
            break;

        default:
            return -1;
        }
    }

    return 0;
}

int minst_get_all_const_definition2(struct minst_blk *blk, struct minst *m, int regm, struct dynarray *d)
{
    struct minst *t;
    BITSET_INIT(defs);
    int i, j;

    dynarray_reset(d);

    bitset_clone(&defs, &blk->defs[regm]);
    bitset_and(&defs, &m->rd_in);

    bitset_foreach(&defs, i) {
        t = blk->allinst.ptab[i];

        if (!t->flag.is_const)
            vm_error("get const definition failure");

        for (j = 0; j < d->len; j++) {
            if (((struct minst *)d->ptab[j])->ld_imm == t->ld_imm)
                break;
        }

        if (j == d->len)
            dynarray_add(d, t);
    }

    return 0;
}

int minst_get_free_reg(struct minst *m)
{
    int i;
    BITSET_INIT(def);

    bitset_clone(&def, &m->out);
    bitset_not(&def);
    bitset_foreach(&def, i) {
        if (i < 16)
            break;
    }

    bitset_uninit(&def);

    return (i < 16) ? i : -1;
}
