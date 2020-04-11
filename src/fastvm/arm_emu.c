
#include "arm_emu.h"

struct arm_emu {
    struct {
        u1* data;
        s4  len;
        s4  pos;
    } code;
};

struct arm_emu   *arm_emu_create(struct arm_emu_create_param *param)
{
    struct arm_emu *emu;

    emu = calloc(1, sizeof (emu[0]));
    if (!emu) {
    }

    return emu;
}

int             arm_emu_destroy(struct arm_emu *)
{
}


int             arm_emu_run(struct arm_emu *vm)
{
}

int             arm_emu_run_once(struct arm_emu *vm, u1 *code, s4 code_len)
{
}


