
#ifndef __vm_h__
#define __vm_h__ 1

#include "config.h"

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

#ifdef _MSC_VER
#define NORETURN        __declspec(noreturn)
#define ALIGNED(x)      __declspec(align(x))
#else
#define NORETURN        __attribute__((noreturn))
#define 
#endif /* _MSC_VER */

#define PTR_SIZE    4

#if PTR_SIZE == 8
#define ELFCLASSW   ELFCLASS64
#define ElfW(type)  Elf##64##_##type
#endif /* PTR_SIZE */

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

	int dump_elf_header;
	int dump_elf_prog_header;
	int dump_elf_section;
	int dump_elf_dynsym;
	int dump_elf_code;
	unsigned long code_addr;

    void *error_opaque;
    void (*error_func)(void *opaque, const char *msg);

    unsigned char*  data;
    int data_len;
    char *filename;

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
} VMState;


int vmelf_load(VMState *s);
void vmelf_unload(struct VMState *s);
void vmelf_dump(struct VMState*s);

#define VM_SET_STATE(fn)    fn        

#define vm_error_noabort    VM_SET_STATE(_vm_error_noabort)
#define vm_error            VM_SET_STATE(_vm_error)
#define vm_warning          VM_SET_STATE(_vm_warning)

void _vm_error_noabort(const char *fmt, ...);
void _vm_error(const char *fmt, ...);
void _vm_warning(const char *fmt, ...);

VMState *vm_new(void);
void vm_delete(VMState *s);

#define MSTACK_IS_EMPTY(s)      (s##_top == -1)
#define MSTACK_TOP(s)           s[s##_top]
#define MSTACK_POP(s)           s[s##_top--]
#define MSTACK_PUSH(s, e)       s[++s##_top] = e


#endif