
#ifndef __vm_h__
#define __vm_h__ 1

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
};

#endif