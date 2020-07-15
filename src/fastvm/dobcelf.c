
#include "mcore/mcore.h"
#include "vm.h"
#include "elf.h"

#define SHF_PRIVATE         0x80000000
#define SHF_DYNSYM          0x40000000

struct section *new_section(VMState *s1, const char *name, int sh_type, int sh_flags)
{
    struct section *sec;

    sec = vm_mallocz(sizeof (sec[0]) + strlen(name));
    sec->s1 = s1;
    strcpy(sec->name, name);
    sec->sh_type = sh_type;
    sec->sh_flags = sh_flags;

    switch (sh_type) {
    case SHT_GNU_versym:
        sec->sh_addralign = 2;
        break;

    case SHT_HASH:
    case SHT_REL:
    case SHT_RELA:
    case SHT_DYNSYM:
    case SHT_SYMTAB:
    case SHT_DYNAMIC:
    case SHT_GNU_verneed:
    case SHT_GNU_verdef:
        sec->sh_addralign = PTR_SIZE;
        break;

    case SHT_STRTAB:
        sec->sh_addralign = 1;

    default:
        sec->sh_addralign = PTR_SIZE;   /* gcc/pcc default alignment */
    }

    if (sh_flags & SHF_PRIVATE) {
        dynarray_add(&s1->priv_sections, sec);
    }
    else {
        sec->sh_num = s1->sections.len;
        dynarray_add(&s1->sections, sec);
    }

    return sec;
}

static void free_section(struct section *sec)
{
    free(sec);
}

int dobc_object_type(int fd, ElfW(Ehdr) *h)
{
    return 0;
}

int dobc_load_object_file(VMState *s1, int fd, unsigned long file_offset)
{
    return 0;
}

int dobc_load_archive(VMState *s1, int fd, int alacarte)
{
    return 0;
}

static void dobc_output_elf(VMState *s1, FILE *f, int phnum, ElfW(Phdr) *phdr, int file_offset, int *sec_order)
{
    int shnum, file_type;
    ElfW(Ehdr) ehdr;
    //ElfW(Shdr) shdr, *sh;


    shnum = s1->sections.len;
    
    memset(&ehdr, 0, sizeof (ehdr));

    if (phnum > 0) {
        ehdr.e_phentsize = sizeof(ElfW(Phdr));
        ehdr.e_phnum = phnum;
        ehdr.e_phoff = sizeof(ElfW(Ehdr));
    }

    file_offset = (file_offset + 3) & -4;

    ehdr.e_ident[0] = ELFMAG0;
    ehdr.e_ident[1] = ELFMAG1;
    ehdr.e_ident[2] = ELFMAG2;
    ehdr.e_ident[3] = ELFMAG3;
    ehdr.e_ident[4] = ELFCLASSW;
    ehdr.e_ident[5] = ELFDATA2LSB;
    ehdr.e_ident[6] = EV_CURRENT;

    switch (file_type) {
    }

    return;
}
