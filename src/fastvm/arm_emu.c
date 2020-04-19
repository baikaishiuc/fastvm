
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
		unsigned char*	data;
		int				top;
		int				size;
	} stack;

	struct {
		int				counts;
	} inst;
};

typedef int(*arm_inst_func)	(uint8_t *inst, int inst_len, char *obuf);

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
	arm_inst_func	funclist[4];
} desclist[]= {
	{"0000	o1	i5 l3 l3",		{thumb_inst_lsl, thumb_inst_lsl}},
	{"0001	0	i5 l3 l3",		{thumb_inst_asr}},
	{"1011	o1	i5 l3 l3",		{thumb_inst_push, thumb_inst_pop}},
	{"1110 1001 0010 1101 0M0 r13", {t2_inst_push}}
};

static int init_inst_map = 0;

struct inst_node {
	int id;
	struct inst_node *left;
	struct inst_node *right;
	struct inst_node *rep;

	arm_inst_func func;
	struct inst_node *set[3];
	char *exp;
};

struct inst_tree {
	int counts;
	struct dynarray arr;
	struct inst_node root;
};

struct arm_inst_engine {
	struct inst_tree	enfa;
	struct inst_tree	dfa;

	int width;
	int height;
	int *trans2d;
};

struct inst_node *inst_node_new(struct inst_tree *tree, char *exp, arm_inst_func func)
{
	struct inst_node *node = calloc(1, sizeof (node[0]));

	if (!node)
		vm_error("inst_node_new() failed calloc");

	node->id = tree->counts++;
	dynarray_add(&tree->arr, node);

	return node;
}

void inst_node_copy(struct inst_node *dst, struct inst_node *src)
{
	dst->func = src->func;
	dst->exp = strdup(src->exp);
}

void	inst_node_delete(struct inst_node *root)
{
	if (!root)
		return;

	if (root->left)		inst_node_delete(root->left);
	if (root->right)	inst_node_delete(root->right);
	if (root->rep)		inst_node_delete(root->rep);

	if (root->exp)	free(root->exp);

	free(root);
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
	int j, len = strlen(exp), rep;
	const char *s = exp;

	for (; *s; s++) {
		if (isblank(*s))	continue;
		switch (*s) {
		case '0':
			if (!root->left) {
				root->left = inst_node_new(&en->enfa, NULL, NULL);
			}

			root = root->left;
			break;

		case '1':
			if (!root->right) {
				root->right = inst_node_new(&en->enfa, NULL, NULL);
			}

			root = root->right;
			break;

			/* register */
		case 'i':
		case 'l':
			rep = atoi(&s[1]);
			if (!rep) 
				goto fail_label;

			for (j = 0; j < rep; j++) {
				root->rep = inst_node_new(&en->enfa, NULL, NULL);
				root = root->rep;
			}

			root = root->rep;

			while (s[1] && isdigit(s[1])) s++;
			break;

	fail_label:
		default:
			vm_error("inst expression [%s], position[%d]\n", exp, (exp - s));
			break;
		}
	}

	root->exp = strdup(exp);
	root->func = func;

	return 0;
}

static struct arm_inst_engine *g_eng = NULL;

static int arm_insteng_gen_dfa(struct arm_inst_engine *eng)
{
	struct inst_node *nfa_root = &eng->enfa.root;

	struct inst_node *dfa_root = &eng->dfa.root;
	struct inst_node *node_stack[512], *droot, *nroot, *dnode1;
	int stack_top = -1, i;

#define istack_push(a)		(node_stack[stack_top++] = a)
#define istack_pop()		node_stack[stack_top--]		
#define istack_is_empty()	(stack_top == -1)

	dfa_root->set[0] = nfa_root;
	istack_push(dfa_root);

	while (!istack_is_empty()) {
		droot = istack_pop();

		for (i = 0; i < 3; i++) {
			nroot = droot->set[i];
			if (!nroot)
				continue;

			if (nroot->left || nroot->rep) {
				dnode1 = droot->left = inst_node_new(&eng->dfa, NULL, NULL);

				droot->left->set[0] = nroot->left;
				droot->left->set[1] = nroot->rep;
			}
			else if (nroot->right || nroot->rep) {
				dnode1 = droot->right = inst_node_new(&eng->dfa, NULL, NULL);

				droot->right->set[0] = nroot->right;
				droot->right->set[1] = nroot->rep;
			}

			if (dnode1->set[0]->func && dnode1->set[1]->func) {
				vm_error("Arm Inst expression conflict [%s] [%s]\n", dnode1->set[0]->exp, dnode1->set[1]->exp);
			}
			else if (dnode1->set[0]->func || dnode1->set[1]->func) {
				inst_node_copy(dnode1, dnode1->set[0] ? dnode1->set[0]:dnode1->set[1]);
			}

			istack_push(dnode1);
		}
	}

	return 0;
}

static void arm_insteng_trav_tree(struct arm_inst_engine *eng, struct inst_node *root)
{
	if (root->left) {
		eng->trans2d[root->id] = root->left->id;
		arm_insteng_trav_tree(eng, root->left);
	}
	if (root->right) {
		eng->trans2d[eng->width + root->id] = root->right->id;
		arm_insteng_trav_tree(eng, root->right);
	}
}

static int arm_insteng_gen_trans2d(struct arm_inst_engine *eng)
{
	eng->width = eng->dfa.counts;
	eng->height = 2;

	eng->trans2d = calloc(1, eng->width * eng->height * sizeof (eng->trans2d[0]));
	if (!eng->trans2d)
		vm_error("trans2d calloc failure");

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
	arm_insteng_gen_trans2d(g_eng);

	init_inst_map = 1;

	return 0;
}

static int arm_insteng_uninit()
{
}

static int arm_insteng_decode(struct arm_inst_engine *eng, uint8_t *code, int len)
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
			to = eng->trans2d[eng->width * bit + from];

			if (!to) 
				vm_error("arm_insteng_decode() run on un-support state, code[%02x %02x]\n", code[0], code[1]);

			node = ((struct inst_node *)eng->dfa.arr.ptab[to]);
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

void	arm_emu_destroy(struct arm_emu *vm)
{
	free(vm);
}

int		arm_emu_run(struct arm_emu *emu)
{
	unsigned char *code = emu->code.data + emu->code.pos;
	int len = emu->code.len - emu->code.pos;
	int ret;

	ret = arm_insteng_decode(g_eng, emu->code.data, emu->code.len);
	if (ret < 0) {
		return -1;
	}

	emu->code.pos += ret;

	return 0;
}

int		arm_emu_run_once(struct arm_emu *vm, unsigned char *code, int code_len)
{
    return 0;
}

void		arm_emu_dump(struct arm_emu *emu)
{
}
