
#include "vm.h"
#include "libdobc.h"
#include "arm_emu.h"

int dobc_run2(VMState *s)
{
    Elf32_Ehdr *hdr = (Elf32_Ehdr *)s->filedata;
    Elf32_Sym *func = elf32_sym_find(hdr, s->funcaddr);
    if (!func) {
        vm_error("not found code addr[%x] symbol\n", s->funcaddr);
    }
    unsigned char *code = s->filedata + s->funcaddr;

    Address *addr = Address_new2(s->slgh.defaultcodespace,  s->funcaddr);
    Address *lastaddr = Address_new2(s->slgh.defaultcodespace, s->funcaddr + func->st_size);

    while (Address_less(addr, lastaddr)) {
        int len = Sleigh_printAssembly(s, addr);
        Address_add(addr, len);
    }

    return 0;
}

int dobc_run(VMState *s)
{
    Elf32_Ehdr *hdr = (Elf32_Ehdr *)s->filedata;
    Elf32_Sym *func = elf32_sym_find(hdr, s->funcaddr);
    if (!func) {
        vm_error("not found code addr[%x] symbol\n", s->funcaddr);
    }
    unsigned char *code = s->filedata + s->funcaddr;

    struct arm_emu_create_param param = {0};
    param.filename = s->filename;
    param.thumb = s->funcaddr & 1;
    param.code = param.thumb ? (code - 1) : code;
    param.code_len = func->st_size;
    param.elf = s->filedata;
    param.elf_len = s->filelen;

    struct arm_emu *emu = arm_emu_create(&param);

    arm_emu_run(emu);

    arm_emu_destroy(emu);

    return 0;
}