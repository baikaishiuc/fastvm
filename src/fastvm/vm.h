
#ifndef __vm_h__
#define __vm_h__ 1

#include "config.h"
#include "mcore/mcore.h"

#ifdef _WIN32
#include <windows.h>
#define inline      __inline
#define strdup      _strdup
#define itoa        _itoa

#ifndef __GNUC__
#define strtold     (long double)strtod
#define strtof      (float)strtod
#define strtoll     _strtoi64
#define strtoull    _strtoui64
#endif /* __GNUC__ */

#endif /* _WIN32 */

#ifndef offsetof
#define offsetof(type,field)    ((size_t)&((type *)0)->field)
#endif

#ifndef countof
#define countof(tab)    (sizeof(tab) / sizeof ((tab)[0]))
#endif

#ifdef _MSC_VER
#define NORETURN        __declspec(noreturn)
#define ALIGNED(x)      __declspec(align(x))
#else
#define NORETURN        __attribute__((noreturn))
#define ALIGNED(x)      __attribute__((aligned(x)))
#endif /* _MSC_VER */

#define PTR_SIZE    4

#if PTR_SIZE == 8
#define ELFCLASSW   ELFCLASS64
#define ElfW(type)  Elf##64##_##type
#define ELFW(type)  Elf##64##_##type
#define ElfW_Rel    ElfW(Rela)
#define SHT_RELX    SHT_RELA
#define REL_SECTION_FMT     ".rela%s"
#else
#define ELFCLASSW   ELFCLASS32
#define ElfW(type)  Elf##32##_##type
#define ELFW(type)  ELF##32##_##type
#define ElfW_Rel    ElfW(Rela)
#define SHT_RELX    SHT_REL
#define REL_SECTION_FMT     ".rel%s"
#endif /* PTR_SIZE */

#define addr_t  ElfW(Addr)
#define ElfSym  ElfW(Sym)

#if PTR_SIZE == 8 && !defined DOBC_TARGET_PE
# define LONG_SIZE  8
#else
# define LONG_SIZE  4
#endif

#include "elf.h"

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

typedef struct VMState {

	unsigned long funcaddr;

    void *error_opaque;
    void (*error_func)(void *opaque, const char *msg);

    unsigned char* filedata;
    char *filename;
    int filelen;

    struct dynarray sections;
    struct dynarray priv_sections;

    Section *got;
    Section *plt;

    Section *text_section;
    Section *data_section;
    Section *bss_section;
    Section *cur_text_section;

    Section *symtab_section;
    Section *stab_section;
} VMState;

#define VM_SET_STATE(fn)    fn        

#define vm_error_noabort    VM_SET_STATE(_vm_error_noabort)
#define vm_error            VM_SET_STATE(_vm_error)
#define vm_warning          VM_SET_STATE(_vm_warning)

void _vm_error_noabort(const char *fmt, ...);
void _vm_error(const char *fmt, ...);
void _vm_warning(const char *fmt, ...);

#define MSTACK_IS_EMPTY(s)      (s##_top == -1)
#define MSTACK_TOP(s)           s[s##_top]
#define MSTACK_POP(s)           s[s##_top--]
#define MSTACK_PUSH(s, e)       s[++s##_top] = e
#define MSTACK_INIT(s)          s##_top = -1

/* fastvm parse args return code */
#define OPT_HELP                    1
#define OPT_HELP2                   2
#define OPT_V                       3
#define OPT_DUMP_ELF_HEADER         4
#define OPT_DUMP_ELF_PROG_HEADER    5
#define OPT_DUMP_ELF_SECTION        6
#define OPT_DUMP_ELF_DYNSYM         7
#define OPT_DECODE_ELF              8
#define OPT_DECODE_FUNC             9


struct section {
    unsigned long data_offset;
    unsigned char *data;
    unsigned long data_allocated;

    VMState *s1;
    int sh_name;            /* elf section name (only used during output) */
    int sh_num;             /* elf section number */
    int sh_type;            /* elf section type */
    int sh_flags;           /* elf section flags */
    int sh_info;            /* elf section info */
    int sh_addralign;       /* elf section alignment */
    int sh_entsize;         /* elf entry size */
    unsigned long sh_size;  /* section size (only used during output) */
    addr_t sh_addr;         /* address at which the section is reached */
    unsigned long sh_offset;    /* file offset */
    int nb_hashed_syms;     /* used to resize the hash table */
    struct section *link;   /* link to another section */
    struct section *reloc;  /* corresponding section for relocation, if any */
    struct section *hash;   /* hash table for symbols */
    struct section *prev;   /* previous section on section stack */
    char name[1];           /* section name */
};

#define AFF_BINTYPE_REL         1
#define AFF_BINTYPE_DYN         2
#define AFF_BINTYPE_AR          3
#define AFF_BINTYPE_C67         4

int dobc_object_type(int fd, ElfW(Ehdr) *h);
int dobc_load_object_file(VMState *s1, int fd, unsigned long file_offset);
int dobc_load_archive(VMState *s1, int fd, int alacarte);

void *vm_mallocz(unsigned long size);
void vm_free(char *ptr);

#endif