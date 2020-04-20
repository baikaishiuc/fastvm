
#include "mcore/mcore.h"
#include "vm.h"
#include "arm_emu.h"


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

static int thumb_inst_lsl(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int thumb_inst_lsr(uint8_t *code, int len, char *obuf)
{
    return 0;
}

static int thumb_inst_asr(uint8_t *code, int len, char *obuf)
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

struct arm_inst_desc {
    const char *regexp;
    arm_inst_func    funclist[4];
} desclist[]= {
    {"0000    o1    i5 l3 l3",        {thumb_inst_lsl, thumb_inst_lsl}},
    {"0001    0    i5 l3 l3",        {thumb_inst_asr}},
    {"1011    o1    i5 l3 l3",        {thumb_inst_push, thumb_inst_pop}},
    {"1110 1001 0010 1101 0 m1 0 l13", {t2_inst_push}}
};

static int init_inst_map = 0;

struct inst_node {
    int id;
    struct inst_node *parent;
	struct inst_node *childs[3];

    arm_inst_func func;
	struct dynarray set;
    char *exp;
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
		fprintf(fp, "%d [shape=Msquare];\n", root->id);
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

int arm_insteng_add_exp(struct arm_inst_engine *en, const char *exp, arm_inst_func func)
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

    // FIXME:测试完毕可以关闭掉，验证层数是否正确
    height = inst_node_height(root);
    if (height != 16 && height != 32) {
        vm_error("inst express error[%s], not size 16 or 32\n", exp);
    }

    return 0;
}

static struct arm_inst_engine *g_eng = NULL;

static int arm_insteng_gen_dfa(struct arm_inst_engine *eng)
{
    struct inst_node *nfa_root = &eng->enfa.root;

    struct inst_node *dfa_root = &eng->dfa.root;
    struct inst_node *node_stack[512], *droot, *nroot, *dnode1, *nnode;
    int stack_top = -1, i, j;

#define istack_push(a)            (node_stack[++stack_top] = a)
#define istack_pop()            node_stack[stack_top--]        
#define istack_is_empty()        (stack_top == -1)

	dynarray_add(&dfa_root->set, nfa_root);
    istack_push(dfa_root);

    while (!istack_is_empty()) {
        droot = istack_pop();

		for (i = 0; i < 2; i++) {
			dnode1 = NULL;
			for (j = 0; j < droot->set.len; j++) {
				if (!(nroot = (struct inst_node *)droot->set.ptab[j]))
					continue;

				if (nroot->childs[i] || nroot->childs[2]) {
					if (!droot->childs[i]) {
						droot->childs[i] = inst_node_new(&eng->dfa, droot);
						istack_push(droot->childs[i]);
					}

					dnode1 = droot->childs[i];

					if (nroot->childs[i])	dynarray_add(&dnode1->set, nroot->childs[i]);
					if (nroot->childs[2])	dynarray_add(&dnode1->set, nroot->childs[2]);
				}
			}

			/* 检查生成的DFA节点中包含的NFA节点，是否有多个终结态，有的话报错，没有的话把NFA的
			状态拷贝到DFA中去 */
			if (dnode1) {
				for (j = 0; j < dnode1->set.len; j++) {
					if (!(nnode = (struct inst_node *)dnode1->set.ptab[j]) || !nnode->func)	continue;

					if (dnode1->func) {
						vm_error("conflict end state\n");
					}

					inst_node_copy(dnode1, nnode);
				}
			}
		}
    }

    return 0;
}

static void arm_insteng_trav_tree(struct arm_inst_engine *eng, struct inst_node *root)
{
	int i;

	for (i = 0; i < 2; i++) {
		if (!root->childs[i]) continue;

		eng->trans2d[eng->width * i + root->id] = root->childs[i]->id;
        arm_insteng_trav_tree(eng, root->childs[i]);
	}
}

static int arm_insteng_gen_trans2d(struct arm_inst_engine *eng)
{
    eng->width = eng->dfa.counts;
    eng->height = 2;

    eng->trans2d = calloc(1, eng->width * eng->height * sizeof (eng->trans2d[0]));
    if (!eng->trans2d)
        vm_error("trans2d calloc failure");

	arm_insteng_trav_tree(eng, &eng->dfa.root);

    return 0;
}

static int arm_insteng_init()
{
    int i, j, k, m, len;
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

                int pow = bits ^ 2;
                while (pow--) {
                    for (m = 0; m < bits; m++) {
                        buf[k + m] = !!(pow & (1 <<  (bits - m - 1))) + '0';
                    }

                    strcpy(buf + k + bits, exp + j);
                    arm_insteng_add_exp(g_eng, buf, desclist[i].funclist[0]);
                }
                break;
            }
            else {
                buf[k] = exp[j];
            }
        }
        buf[k] = 0;

        if (j == len)
            arm_insteng_add_exp(g_eng, buf, desclist[i].funclist[0]);
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
    int i, j, from, to, bit;
    if (!g_eng) {
        arm_insteng_init();
    }

    /* arm 解码时，假如第一个16bit没有解码到对应的thumb指令终结态，则认为是thumb32，开始进入
    第2轮的解码 */
    for (i = from = 0; i < 2; i++) {
        uint16_t inst = ((uint16_t *)code)[i];
        for (j = 0; j < 16; j++) {
            bit = !!(inst & (1 << (15 - i)));
            to = g_eng->trans2d[g_eng->width * bit + from];

            if (!to) 
                vm_error("arm_insteng_decode() run on un-support state, code[%02x %02x]\n", code[0], code[1]);

            node = ((struct inst_node *)g_eng->dfa.arr.ptab[to]);
            from = node->id;
        }

        if (node->func)
            break;
    }

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
