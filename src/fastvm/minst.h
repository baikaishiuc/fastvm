

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
    } flag;

    void *reg_node;
};

struct minst_var {
    /* 堆栈深度，指明这个临时变量和哪块堆栈内存绑定 */
    int     top; 
    /* 临时变量 id */
    int     t;
};

struct minst_blk*   minst_blk_new(char *funcname);
void                minst_blk_delete(struct minst_blk *mblk);

void                minst_blk_init(struct minst_blk *blk, char *funcname);
void                minst_blk_uninit(struct minst_blk *blk);

struct minst*       minst_new(struct minst_blk *blk, unsigned char *code, int len, void *reg_node);
void                minst_delete(struct minst *inst);

struct minst*       minst_blk_find(struct minst_blk *blk, char *addr);

void                minst_succ_add(struct minst *minst, struct minst *succ);
void                minst_pred_add(struct minst *minst, struct minst *pred);

/* 当我们往堆栈里push一个值时，就调用此函数，生成一个临时变量

@top        stack depth
@return     != NULL         temporary variable
            NULL            error
*/
struct minst_var*   minst_blk_new_stack_var(struct minst_blk *blk, int top);

struct minst_var*   minst_blk_find_stack_var(struct minst_blk *blk, int top);
struct minst_var*   minst_blk_top_stack_var(struct minst_blk *blk);

/* 当调用比如pop弹出堆栈时，也删除对应的临时变量 */
int                 minst_blk_delete_stack_var(struct minst_blk *blk, int top);

int                 minst_blk_liveness_calc(struct minst_blk *blk);

#ifdef __cplusplus
}
#endif

#endif