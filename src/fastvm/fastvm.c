
#include "mcore/mcore.h"
#include "vm.h"

static const char help[] = {
	"fastvm "FASTVM_VERSION" - Copyright (C) 2020-2025 Zhengxianwei\n"
	"Usage: fastvm [options ...] [-o outfile] [-c] infiles\n"
	"General options:\n"
	"	-c \n"
	"	-o outfile	set output filename\n"
	"	-ds			dump ELF symbol file\n"
	"	-dh			Display the ELF file header\n"
	"	-dl			Display the program headers\n"
	" 	-dS			Display the sections's header\n"
};

void help1()
{
    printf("Usage: fastvm [filename]\n");
}

int fastvm_run(VMState *s, int argc, char **argv)
{
    vmelf_load(s);

    vmelf_dump(s);

    vmelf_unload(s);

    return 0;
}

int fastvm_parse_args(VMState *s, int argc, char **argv)
{
	int i;

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-dh")) {
			s->dump_elf_header = 1;
		}
		else if (!strcmp(argv[i], "-dl")) {
			s->dump_elf_prog_header = 1;
		}
		else if (!strcmp(argv[i], "-dS")) {
			s->dump_elf_section = 1;
		}
		else if (!strcmp(argv[i], "-ds")) {
			s->dump_elf_dynsym = 1;
		}
		else {
			s->filename = strdup(argv[i]);
		}
	}

	return 0;
}

int main(int argc, char **argv)
{
	VMState *s = vm_new();
	fastvm_parse_args(s, argc, argv);

    fastvm_run(s, argc, argv);

    return 0;
}