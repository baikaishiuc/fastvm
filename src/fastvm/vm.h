
#ifndef __vm_h__
#define __vm_h__ 1

#define PTR_SIZE    4

#if PTR_SIZE == 8
#define ELFCLASSW   ELFCLASS64
#define ElfW(type)  Elf##64##_##type
#else
#endif

#include "elf.h"

typedef struct vm_state {
};

/* Section definition */
typedef struct Section {
    unsigned long data_offset;
    unsigned char *data;
    unsigned long data_allocated;
    int sh_name;        /* elf section name (only used during output) */
    int sh_num;         /* elf section number */
    int sh_type;        /* elf section type */
    int sh_info;        /* elf section flags */
    int sh_addralign;   /* elf section alignment */
    int sh_entsize;     /* elf entry size */
    unsigned long sh_size;  /* section size (only used during output ) */
    void *sh_addr;
    unsigned long sh_offset;
    int nb_hashed_syms;
    struct Section *link;   /* link to another section */
    struct Section *reloc;  /* corresponding section for relocation, if any */
    struct Section *hash;   /* previous section on section stack */
    struct Section *prev;   /* previous sction on section stack */
    char name[1];           /* section name */
} Section;

typedef struct vm_elf {
} vm_elf;

#endif