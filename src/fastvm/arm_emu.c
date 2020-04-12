
#include "arm_emu.h"

struct arm_emu {
    struct {
        unsigned char*  data;
        int             len;
        int             pos;
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

int             arm_emu_destroy(struct arm_emu *vm)
{
    return 0;
}


int             arm_emu_run(struct arm_emu *vm)
{
    return 0;
}

int             arm_emu_run_once(struct arm_emu *vm, unsigned char *code, int code_len)
{
    return 0;
}


