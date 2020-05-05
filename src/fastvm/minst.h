

#ifndef __minst_h__
#define __minst_h__

#ifdef __cplusplus
extern "C" {
#endif

struct minst_blk {
    char *funcname;

    struct {
        int     counts;
    } minsts;

    /* 生成IR时，会产生大量的临时变量，这个是临时变量计数器 */
    int         tvar_id;
    struct dynarray tvar;

    /* 当程序按顺序解析所有指令时，把所有指令放入此数组，记得此数组要
    严格按照地址顺序排列 */
    struct dynarray allinst;
};

struct minst_node {
    struct minst *minst;
    struct minst_node *next;
};

struct minst {
    unsigned char *addr;
    int len;

    struct bitset use;
    struct bitset def;
    struct bitset in;
    struct bitset out;

    struct minst_node preds;
    struct minst_node succs;

    struct {
        unsigned b : 1;         // is jump inst?
        unsigned b_al : 1;      // jmp always
        unsigned b_need_fixed : 1;   
        unsigned dead_code : 1;
        unsigned prologue : 1;
        unsigned epilogue : 1;
    } flag;

    unsigned long host_addr;            // jump address, need be fixed in second pass

    struct minst *cfg_node;

    void *reg_node;
};

#define live_def_set(reg)       bitset_set(&minst->def, reg, 1)
#define live_use_set(reg)       bitset_set(&minst->use, reg, 1)
#define live_use_clear(_m)      bitset_clear(&_m->use)

#define liveness_set2(_def, _use, _use1)    do { \
        bitset_set(&minst->def, _def, 1); \
        bitset_set(&minst->use, _use, 1); \
        bitset_set(&minst->use, _use1, 1); \
    } while (0)

#define liveness_set(_def, _use)    do { \
        bitset_set(&minst->def, _def, 1); \
        bitset_set(&minst->use, _use, 1); \
    } while (0)

struct minst_blk*   minst_blk_new(char *funcname);
void                minst_blk_delete(struct minst_blk *mblk);

void                minst_blk_init(struct minst_blk *blk, char *funcname);
void                minst_blk_uninit(struct minst_blk *blk);

struct minst*       minst_new(struct minst_blk *blk, unsigned char *code, int len, void *reg_node);
void                minst_delete(struct minst *inst);

struct minst*       minst_blk_find(struct minst_blk *blk, unsigned long addr);

void                minst_succ_add(struct minst *minst, struct minst *succ);
void                minst_pred_add(struct minst *minst, struct minst *pred);

/* 做活跃性分析的时候，需要加上liveness专用的prologue和epilogue
live prologue 把所有的寄存器设置为def
live epilogue 把所有的寄存器设置为use
不这样做他会把function proglogue和epilog识别为死代码
*/
void                minst_blk_live_prologue_add(struct minst_blk *blk);
void                minst_blk_live_epilogue_add(struct minst_blk *blk);
int                 minst_blk_liveness_calc(struct minst_blk *blk);

/* 死代码删除 */
int                 minst_blk_dead_code_elim(struct minst_blk *blk);

#ifdef __cplusplus
}
#endif

#endif