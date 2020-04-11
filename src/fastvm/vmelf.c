
#include "mcore/mcore.h"
#include "vm.h"

struct vmelf {
    Section  **sections;
    int nb_sections;

    Section **priv_sections;
    int nb_priv_sections;

    Section *got;
    Section *plt;

    Section *text_section;
    Section *data_section;
    Section *bss_section;
    Section *cur_text_section;

    Section *symtab_section;
    Section *stab_section;
};

struct vmelf *vmelf_new()
{
    return NULL;
}

struct vmelf *vmelf_load(const char *filename)
{
    struct vm_elf *ve;
    int len;
    char *data = file_load(filename, &len);
    if (!data) {
        return NULL;
    }

    if (!ve)
        return NULL;

    return ve;
}

void vmelf_unload(struct vmelf *elf)
{
    if (!elf)
        return;

    free (elf);
}

void vmelf_dump(struct vmelf *elf)
{
}