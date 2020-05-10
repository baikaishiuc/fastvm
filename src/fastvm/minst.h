

#ifndef __minst_h__
#define __minst_h__

#ifdef __cplusplus
extern "C" {
#endif

struct minst_blk {
    char *funcname;

    /* 生成IR时，会产生大量的临时变量，这个是临时变量计数器 */
    int         tvar_id;
    struct dynarray tvar;

    /* 当程序按顺序解析所有指令时，把所有指令放入此数组，记得此数组要
    严格按照地址顺序排列 */
    struct dynarray allinst;

    /* 只处理系统寄存器列表 */
    /* 某寄存器所有def指令集合，数据为指令id */
    struct bitset     defs[32];

    /* 某寄存器所有use指令集合，数据为指令id */
    struct bitset     uses[32];

    struct dynarray     const_insts;
};

struct minst_node {
    struct minst *minst;
    struct minst_node *next;
};

struct minst {
    unsigned char *addr;
    int len;

    int id;
    struct bitset use;
    struct bitset def;
    struct bitset in;
    struct bitset out;

    struct bitset rd_in;
    struct bitset rd_out;
    struct bitset kills;

    struct minst_node preds;
    struct minst_node succs;

    struct {
        unsigned b : 1;         // is jump inst?
        unsigned b_al : 1;      // jmp always
        unsigned b_need_fixed : 2;   
        unsigned b_cond_passed : 1;
        unsigned dead_code : 1;
        /* live 前置指令 */
        unsigned prologue : 1;
        /* live 后置指令 */
        unsigned epilogue : 1;
        /* 是否在It块中，是的话，需要调整他的前置节点，为什么呢
        比如, a -> b -> c

        上面是3条顺序执行的指令，但是假如 b.flag.in_it_block == 1，那么需要额外增加一个a->c的跳转。
        a - b -> c
          ----->
        否则会导致他的活跃计算不正确
        */
        unsigned in_it_block : 1;
        unsigned is_lm_const : 1;
        unsigned is_ln_const : 1;
        unsigned is_def_apsr : 1;

        unsigned is_const : 1;
    } flag;

    unsigned long host_addr;            // jump address, need be fixed in second pass

    struct minst *cfg_node;

    /* 调用哪个reg_node去解析内容 */
    void *reg_node;

    int ld;
    union {
        int ld_imm;
        struct arm_cpsr apsr;
    };
};

#define live_def_set(blk, reg)       do { \
        bitset_set(&minst->def, reg, 1); \
        if ((reg < SYS_REG_NUM) && (reg > -1)) \
            bitset_set(&((blk)->defs[reg]), minst->id, 1); \
    } while (0)
#define live_use_set(blk, reg)       do { \
        bitset_set(&minst->use, reg, 1); \
        if ((reg < SYS_REG_NUM) && (reg > -1)) \
            bitset_set(&((blk)->uses[reg]), minst->id, 1); \
    } while (0)

#define liveness_set2(blk, _def, _use, _use1)    do { \
        live_def_set(blk, _def); \
        live_use_set(blk, _use); \
        live_use_set(blk, _use1); \
    } while (0)

#define liveness_set(blk, _def, _use)    do { \
        live_def_set(blk, _def); \
        live_use_set(blk, _use); \
    } while (0)

struct minst_blk*   minst_blk_new(char *funcname);
void                minst_blk_delete(struct minst_blk *mblk);

void                minst_blk_init(struct minst_blk *blk, char *funcname);
void                minst_blk_uninit(struct minst_blk *blk);

struct minst*       minst_new(struct minst_blk *blk, unsigned char *code, int len, void *reg_node);
void                minst_delete(struct minst *inst);
int                 minst_get_def(struct minst *minst);
#define             minst_set_const(m, imm) do { \
        m->flag.is_const = 1; \
        m->ld_imm = imm; \
    } while (0)

struct minst*       minst_blk_find(struct minst_blk *blk, unsigned long addr);

void                minst_succ_add(struct minst *minst, struct minst *succ);
void                minst_pred_add(struct minst *minst, struct minst *pred);
void                minst_succ_del(struct minst *minst, struct minst *succ);
void                minst_pred_del(struct minst *minst, struct minst *pred);

/* 做活跃性分析的时候，需要加上liveness专用的prologue和epilogue
live prologue 把所有的寄存器设置为def
live epilogue 把所有的寄存器设置为use
不这样做他会把function proglogue和epilog识别为死代码
*/
void                minst_blk_live_prologue_add(struct minst_blk *blk);
void                minst_blk_live_epilogue_add(struct minst_blk *blk);

int                 minst_blk_is_on_start_unique_path(struct minst_blk *blk, struct minst *def, struct minst *use);

/* */
int                 minst_blk_liveness_calc(struct minst_blk *blk);

/* 死代码删除 */
int                 minst_blk_dead_code_elim(struct minst_blk *blk);

/* 生成到达定值, generate reaching definitions */
int                 minst_blk_gen_reaching_definitions(struct minst_blk *blk);

struct minst*       minst_get_last_const_definition(struct minst_blk *blk, struct minst *minst, int regm);

#ifdef __cplusplus
}
#endif

#endif