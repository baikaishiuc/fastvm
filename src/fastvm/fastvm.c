
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

int fastvm_run(int argc, char **argv)
{
    struct VMElf *ve;

    ve = vmelf_load(argv[1]);

    vmelf_dump(ve);

    vmelf_unload(ve);

    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: fastvm [filename]\n");
        return -1;
    }

    fastvm_run(argc, argv);
    return 0;
}