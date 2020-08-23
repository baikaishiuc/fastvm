
#include "mcore/mcore.h"
#include "vm.h"

static const char help[] = {
    "fastvm "FASTVM_VERSION" - Copyright (C) 2020-2025 Zhengxianwei\n"
    "Usage: fastvm [options ...] [-o outfile] [-c] infiles\n"
    "General options:\n"
    "    -c \n"
    "    -h             display this help information\n"    
    "    -o outfile     set output filename\n"
    "    -ds            dump ELF symbol file\n"
    "    -dh            Display the ELF file header\n"
    "    -dl            Display the program headers\n"
    "    -dS            Display the sections's header\n"
    "    -dc [addr]     Display code on addr(hex)\n"
    "\n"
    "    -d             decode elf file, gernerate all deobfuse function analysis\n"
    "    -df            decode one function, gernerate deobfuse function analysis\n"
};

static const char version[] =
"fastvm version "FASTVM_VERSION" ("
#ifdef FASTVM_TARGET_ARM
"ARM"
#endif
")\n";

#if defined(FASTVM_EXE)
int main(int argc, char **argv)
{
    int opt;
    VMState *s = dobc_new();
    opt = dobc_parse_args(s, argc, argv);

    dobc_load_file(s);

    if (opt == OPT_HELP)
        return fputs(help, stdout), 0;
    else if (opt == OPT_V)
        return fputs(version, stdout), 0;
    else if ((OPT_DUMP_ELF_HEADER == opt)
        || (OPT_DUMP_ELF_PROG_HEADER == opt)
        || (OPT_DUMP_ELF_SECTION == opt)
        || (OPT_DUMP_ELF_DYNSYM == opt))
        return elf_dump(s->filedata, s->filelen, opt), 0;
    else if (OPT_DECODE_ELF == opt) {
        vm_error("not support decode elf file");
    }
    else if (OPT_DECODE_FUNC == opt) {
        dobc_run(s);
    }

    dobc_delete(s);

    return 0;
}
#endif