
#if defined(__cplusplus)
extern "C" {
#endif

#ifndef __arm_emu_h__
#define __arm_emu_h__

#include "vm.h"

struct arm_emu_create_param
{
    unsigned char*  code;
    int             code_len;
	void			*user_ctx;
	int(*inst_func)(unsigned char *inst, int len,  char *inst_str, void *user_ctx);
};

struct arm_emu*		arm_emu_create(struct arm_emu_create_param *param);
void				arm_emu_destroy(struct arm_emu *);

/*

 @return	0		success
			1		finish
			<0		error
*/
int					arm_emu_run(struct arm_emu *vm);
int					arm_emu_run_once(struct arm_emu *vm, unsigned char *code, int code_len);

#endif /* __arm_emu_h__ */

#if defined(__cplusplus)
}
#endif