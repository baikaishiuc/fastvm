
#include "mcore/mcore.h"
#include "vm.h"
#include "arm_emu.h"
#include <math.h>


struct arm_emu {
    struct {
        unsigned char*  data;
        int             len;
        int             pos;
    } code;

    int(*inst_func)(unsigned char *inst, int len,  char *inst_str, void *user_ctx);
    void *user_ctx;

    /* emulate stack */
    struct {
        unsigned char*    data;
        int                top;
        int                size;
    } stack;

    struct {
        int                counts;
    } inst;
};

typedef int(*arm_inst_func)    (uint8_t *inst, int inst_len, char *obuf);

static int t1_inst_lsl(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t1_inst_lsr(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t1_inst_asr(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int thumb_inst_push(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int thumb_inst_pop(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t2_inst_push(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t1_inst_add(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t1_inst_sub(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t1_inst_mov(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t1_inst_cmp(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t1_inst_and(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t1_inst_eor(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t1_inst_adc(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t1_inst_sbc(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t1_inst_ror(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t1_inst_tst(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t1_inst_neg(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t1_inst_cmn(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t1_inst_orr(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t1_inst_mul(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t1_inst_bic(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t1_inst_mvn(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int t1_inst_cpy(uint8_t *code, int len, char *obuf)
{
    return 0;
}

struct arm_inst_desc {
    const char *regexp;
    arm_inst_func    funclist[4];
    const char        *desc[4];
} desclist[]= {
    {"0000    o1    i5 l3 l3",              {t1_inst_lsl, t1_inst_lsr}, {"lsl", "lsr"}},
    {"0001    0    i5 l3 l3",               {t1_inst_asr}, {"asr"}},
    {"0001    10 o1 l3 l3 l3",              {t1_inst_add, t1_inst_sub}, {"add", "sub"}},
    {"0001    11 o1 i3 l3 l3",              {t1_inst_add, t1_inst_sub}, {"add", "sub"}},
    {"0010    o1 l3 i8",                    {t1_inst_mov, t1_inst_cmp}, {"mov", "cmp"}},
    {"0011    o1 l3 i8",                    {t1_inst_add, t1_inst_sub}, {"add", "sub"}},
    {"0100    0000 o2 l3 l3",               {t1_inst_and, t1_inst_eor, t1_inst_lsl, t1_inst_lsr}, {"and", "eor", "lsl2", "lsr2"}},
    {"0100    0001 o2 l3 l3",               {t1_inst_asr, t1_inst_adc, t1_inst_sbc, t1_inst_ror}, {"asr", "adc", "sbc", "ror"}},
    {"0100    0010 o2 l3 l3",               {t1_inst_tst, t1_inst_neg, t1_inst_cmp, t1_inst_cmn}, {"tst", "neg", "cmp", "cmn"}},
    {"0100    0011 o2 l3 l3",               {t1_inst_orr, t1_inst_mul, t1_inst_bic, t1_inst_mvn}, {"orr", "mul", "bic", "mvn"}},
    {"0100    0110 00 l3 l3",               {t1_inst_cpy}, {"cpy"}},
    {"0100    01 o1 0 01 l3 l3",            {t1_inst_add, t1_inst_mov}, {"add", "mov"}},
    {"1011    o1    i5 l3 l3",            {thumb_inst_push, thumb_inst_pop}, {"push", "pop"}},
    {"1110 1001 0010 1101 0 m1 0 l13",    {t2_inst_push}, "push.w"},
};

static int init_inst_map = 0;

struct inst_node {
    int id;
    struct inst_node *parent;
    struct inst_node *childs[3];

    arm_inst_func func;
    struct dynarray set;
    char *exp;
    char *desc;
};

struct inst_tree {
    int counts;
    struct dynarray arr;
    struct inst_node root;
};

struct arm_inst_engine {
    struct inst_tree    enfa;
    struct inst_tree    dfa;

    int width;
    int height;
    int *trans2d;
};

struct inst_node *inst_node_new(struct inst_tree *tree, struct inst_node *parent)
{
    struct inst_node *node = calloc(1, sizeof (node[0]));

    if (!node)
        vm_error("inst_node_new() failed calloc");

    node->parent = parent;
    node->id = ++tree->counts;
    dynarray_add(&tree->arr, node);

    return node;
}

void inst_node_copy(struct inst_node *dst, struct inst_node *src)
{
    dst->func = src->func;
    dst->exp = strdup(src->exp);
    dst->desc = strdup(src->desc);
}

int inst_node_height(struct inst_node *node)
{
    int i = 0;
    while (node->parent) {
        i++;
        node = node->parent;
    }

    return i;
}

void    inst_node_delete(struct inst_node *root)
{
    int i;
    if (!root)
        return;

    for (i = 0; i < count_of_array(root->childs); i++) {
        inst_node_delete(root->childs[i]);
    }

    if (root->exp)    free(root->exp);
    if (root->desc) free(root->desc);

    free(root);
}

void inst_node__dump_dot(FILE *fp, struct inst_node *root)
{
    int i;

    for (i = 0; i < count_of_array(root->childs); i++) {
        if (!root->childs[i])
            continue;

        fprintf(fp, "%d -> %d [label = \"%d-%d\"]\n", root->id, root->childs[i]->id, i, inst_node_height(root->childs[i]));
        inst_node__dump_dot(fp, root->childs[i]);
    }

    if (root->func) {
        fprintf(fp, "%d [shape=Msquare, label=\"%s\"];\n", root->id, root->desc);
    }
}

void inst_node_dump_dot(const char *filename, struct inst_node *root)
{
    char buf[128];
    FILE *fp = fopen(filename, "w");

    fprintf(fp, "digraph G {\n");

    inst_node__dump_dot(fp, root);

    fprintf(fp, "%d [shape=Mdiamond];\n", root->id);

    fprintf(fp, "}\n");

    fclose(fp);

    sprintf(buf, "dot -Tpng %s -o %s.png", filename, filename);
    system(buf);
}

struct arm_inst_engine *inst_engine_create()
{
    struct arm_inst_engine *engine = NULL;

    engine = calloc(1, sizeof(engine[0]));
    if (!engine)
        vm_error("failed calloc()");

    return engine;
}

int arm_insteng_add_exp(struct arm_inst_engine *en, const char *exp, const char *desc, arm_inst_func func)
{
    struct inst_node *root = &en->enfa.root;
    int j, len = strlen(exp), rep, height, idx;
    const char *s = exp;

    for (; *s; s++) {
        if (isblank(*s))    continue;
        switch (*s) {
        case '0':
        case '1':
            idx = *s - '0';
            if (!root->childs[idx]) {
                root->childs[idx] = inst_node_new(&en->enfa, root);
            }

            root = root->childs[idx];
            break;

            /* immediate */
        case 'i':

            /* register */
        case 'l':

            /* more register，一般是对寄存器列表的补充 */
        case 'm':
            rep = atoi(&s[1]);
            if (!rep) 
                goto fail_label;

            for (j = 0; j < rep; j++) {
                root->childs[2] = inst_node_new(&en->enfa, root);
                root = root->childs[2];
            }

            while (s[1] && isdigit(s[1])) s++;
            break;

    fail_label:
        default:
            vm_error("inst expression [%s], position[%d:%s]\n", exp, (s - exp), s);
            break;
        }
    }

    root->exp = strdup(exp);
    root->func = func;
    root->desc = strdup(desc);

    // FIXME:测试完毕可以关闭掉，验证层数是否正确
    height = inst_node_height(root);
    if (height != 16 && height != 32) {
        vm_error("inst express error[%s], not size 16 or 32\n", exp);
    }

    return 0;
}

static struct arm_inst_engine *g_eng = NULL;

static struct inst_node *inst_node_find(struct inst_tree *tree, struct dynarray *arr)
{
    struct inst_node *node;
    int i;
    for (i = 0; i < tree->arr.len; i++) {
        node = (struct inst_node *)tree->arr.ptab[i];
        if (0 == dynarray_cmp(arr, &node->set))
            return node;
    }

    return NULL;
}

static int arm_insteng_gen_dfa(struct arm_inst_engine *eng)
{
    struct inst_node *nfa_root = &eng->enfa.root;

    struct inst_node *dfa_root = &eng->dfa.root;
    struct inst_node *node_stack[512], *droot, *nroot, *dnode1, *nnode;
    struct dynarray set = {0};
    int stack_top = -1, i, j, need_split;

#define istack_push(a)            (node_stack[++stack_top] = a)
#define istack_pop()            node_stack[stack_top--]        
#define istack_is_empty()        (stack_top == -1)

    dynarray_add(&dfa_root->set, nfa_root);
    istack_push(dfa_root);
    dynarray_reset(&set);

    while (!istack_is_empty()) {
        droot = istack_pop();

        for (i = need_split = 0; i < droot->set.len; i++) {
            nroot = (struct inst_node *)droot->set.ptab[i];
            if (nroot->childs[0] || nroot->childs[1]) {
                need_split = 1;
                break;
            }
        }

        for (i = need_split ? 0:2; i < (3 - need_split); i++) {
            dnode1 = NULL;

            for (j = 0; j < droot->set.len; j++) {
                if (!(nroot = (struct inst_node *)droot->set.ptab[j]))
                    continue;

                if (nroot->childs[i])    dynarray_add(&set, nroot->childs[i]);
                if ((i != 2) && nroot->childs[2])    dynarray_add(&set, nroot->childs[2]);
            }

            if (!dynarray_is_empty(&set)) {
                dnode1 = inst_node_find(&eng->dfa, &set);
                if (!dnode1) {
                    dnode1 = inst_node_new(&eng->dfa, droot);
                    dynarray_copy(&dnode1->set, &set);
                    istack_push(dnode1);
                }

                droot->childs[i] = dnode1;
            }

            /* 检查生成的DFA节点中包含的NFA节点，是否有多个终结态，有的话报错，没有的话把NFA的
            状态拷贝到DFA中去 */
            if (dnode1) {
                for (j = 0; j < dnode1->set.len; j++) {
                    if (!(nnode = (struct inst_node *)dnode1->set.ptab[j]) || !nnode->func)    continue;

                    if (dnode1->func) {
                        vm_error("conflict end state\n");
                    }

                    inst_node_copy(dnode1, nnode);
                }
            }

            dynarray_reset(&set);
        }
    }

    return 0;
}

static int arm_insteng_gen_trans2d(struct arm_inst_engine *eng)
{
    int i;
    struct inst_tree *tree = &eng->dfa;
    struct inst_node *node;
    eng->width = eng->dfa.counts;
    eng->height = 2;

    eng->trans2d = calloc(1, eng->width * eng->height * sizeof (eng->trans2d[0]));
    if (!eng->trans2d)
        vm_error("trans2d calloc failure");

    for (i = 0; i < tree->arr.len; i++) {
        node = (struct inst_node *)tree->arr.ptab[i];

        if (node->childs[2]) {
            eng->trans2d[eng->width + node->id] = node->childs[2]->id;
            eng->trans2d[node->id] = node->childs[2]->id;
            continue;
        }

        if (node->childs[0]) {
            eng->trans2d[node->id] = node->childs[0]->id;
        }

        if (node->childs[1]) {
            eng->trans2d[eng->width + node->id] = node->childs[1]->id;
        }
    }

    return 0;
}

static int arm_insteng_init()
{
    int i, j, k, m, n, len;
    const char *exp;
    char buf[128];

    if (g_eng)
        return 0;

    g_eng = calloc(1, sizeof (g_eng[0]));
    if (!g_eng)
        vm_error("arm_insteng_init() calloc failure");

    dynarray_add(&g_eng->enfa.arr, &g_eng->enfa.root);
    dynarray_add(&g_eng->dfa.arr, &g_eng->dfa.root);

    for (i = 0; i < count_of_array(desclist); i++) {
        exp = desclist[i].regexp;
        len = strlen(exp);
        for (j = k = 0; j < len; k++, j++) {
            if (exp[j] == 'o') {
                int bits = atoi(exp + j + 1);
                if (!bits)
                    vm_error("express error, unknown o token, %s\n", exp);

                while (isdigit(exp[++j]));

                int pow1 = (int)pow(2, bits);
                for (n = 0; n < pow1; n++) {
                    for (m = 0; m < bits; m++) {
                        buf[k + m] = !!(n & (1 <<  (bits - m - 1))) + '0';
                    }

                    strcpy(buf + k + bits, exp + j);
                    arm_insteng_add_exp(g_eng, buf, desclist[i].desc[n], desclist[i].funclist[n]);
                }
                break;
            }
            else {
                buf[k] = exp[j];
            }
        }
        buf[k] = 0;

        if (j == len)
            arm_insteng_add_exp(g_eng, buf, desclist[i].desc[0], desclist[i].funclist[0]);
    }


    arm_insteng_gen_dfa(g_eng);

    inst_node_dump_dot("enfa.dot", &g_eng->enfa.root);
    inst_node_dump_dot("dfa.dot", &g_eng->dfa.root);

    arm_insteng_gen_trans2d(g_eng);

    init_inst_map = 1;

    return 0;
}

static int arm_insteng_uninit()
{
}

static int arm_insteng_decode(uint8_t *code, int len)
{
    struct inst_node *node;
    int i, j, from = 0, to, bit;
    if (!g_eng) {
        arm_insteng_init();
    }

    /* arm 解码时，假如第一个16bit没有解码到对应的thumb指令终结态，则认为是thumb32，开始进入
    第2轮的解码 */

    printf("start state:%d\n", from);
    for (i = from = 0; i < 2; i++) {
        uint16_t inst = ((uint16_t *)code)[i];
        for (j = 0; j < 16; j++) {
            bit = !!(inst & (1 << (15 - j)));
            to = g_eng->trans2d[g_eng->width * bit + from];

            if (!to) 
                vm_error("arm_insteng_decode() run on un-support state, code[%02x %02x]\n", code[0], code[1]);

            node = ((struct inst_node *)g_eng->dfa.arr.ptab[to]);
            from = node->id;

            printf("%d ", from);
        }

        if (node->func)
            break;
    }
    printf("\n");

    if (!node->func) {
        printf("arm_insteng_decode() meet unkown instruction, code[%02x %02x]\n", code[0], code[1]);
        return 1;
    }

    node->func(code, len, NULL);

    return i * 2;
}

struct arm_emu   *arm_emu_create(struct arm_emu_create_param *param)
{
    struct arm_emu *emu;

    emu = calloc(1, sizeof (emu[0]));
    if (!emu) {
    }

    emu->code.data = param->code;
    emu->code.len = param->code_len;

    return emu;
}

void    arm_emu_destroy(struct arm_emu *vm)
{
    free(vm);
}

int        arm_emu_run(struct arm_emu *emu)
{
    unsigned char *code = emu->code.data + emu->code.pos;
    int len = emu->code.len - emu->code.pos;
    int ret;

    ret = arm_insteng_decode(emu->code.data, emu->code.len);
    if (ret < 0) {
        return -1;
    }

    emu->code.pos += ret;

    return 0;
}

int        arm_emu_run_once(struct arm_emu *vm, unsigned char *code, int code_len)
{
    return 0;
}

void        arm_emu_dump(struct arm_emu *emu)
{
}
