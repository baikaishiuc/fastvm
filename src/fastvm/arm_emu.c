
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


int             arm_emu_run(struct arm_emu *emu)
{
	unsigned char *code = emu->code.data;

	return 0;
}

int             arm_emu_run_once(struct arm_emu *vm, unsigned char *code, int code_len)
{
    return 0;
}

void		arm_emu_dump(struct arm_emu *emu)
{
}
