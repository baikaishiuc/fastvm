
#include "mcore/mcore.h"
#include "vm.h"

void help()
{
    printf("Usage: fastvm [filename]\n");
}

int fastvm_run(int argc, char **argv)
{
    struct vmelf *ve;

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