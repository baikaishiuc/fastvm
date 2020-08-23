
#ifndef __vm_h__
#define __vm_h__ 1

#include "config.h"
#include "mcore/mcore.h"
#include <fcntl.h>
#include <io.h>

#ifdef _WIN32
#include <windows.h>
#define inline      __inline
#define strdup      _strdup
#define itoa        _itoa
#define read        _read
#define write       _write
#define lseek       _lseek

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

#define ALIGN_UP(v, a)          ((v + a - 1) & ~(a - 1))
#define ALIGN_BOTTOM(v, a)      (v & ~(a - 1))

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

#include "libdobc.h"
#include "elf.h"
#include "pcode.h"

#define DOBC_TARGET_ARM         /* ARMv4 code generator */

#define TARGET_DEFS_ONLY
#ifdef DOBC_TARGET_ARM
#include "arm-link.c"
#else
#error "unknwon target"
#endif
#undef TARGET_DEFS_ONLY


#if !defined(DOBC_TARGET_I386) && !defined(DOBC_TARGET_ARM) && \
    !defined(DOBC_TARGET_ARM64) && !defined(DOBC_TARGET_C67) && \
    !defined(DOBC_TARGET_X86_64) && !defined(DOBC_TARGET_RISCV64)

#endif

typedef struct CString {
    int size;
    char *data;     /* either char * or nwchar_t */
    int size_allocated;
} CString;

/* Section definition */
typedef struct Section {
    unsigned long data_offset;
    unsigned char *data;
    unsigned long data_allocated;
    VMState *s1;
    int sh_name;        /* elf section name (only used during output) */
    int sh_num;         /* elf section number */
    int sh_type;        /* elf section type */
    int sh_flags;       /* elf section flags */
    int sh_info;        /* elf section info */
    int sh_addralign;   /* elf section alignment */
    int sh_entsize;     /* elf entry size */
    int sh_link;
    unsigned long sh_size;  /* section size (only used during output ) */
    addr_t sh_addr;
    unsigned long sh_offset;
    int nb_hashed_syms;
    struct Section *link;   /* link to another section */
    struct Section *reloc;  /* corresponding section for relocation, if any */
    struct Section *hash;   /* previous section on section stack */
    struct Section *prev;   /* previous sction on section stack */
    char name[1];           /* section name */
} Section;

typedef struct DLLReference {
    int level;
    void *handle;
    char name[1];
} DLLReference;

/* extend symbol attribute (not in symbol table) */
struct sym_attr {
    unsigned got_offset;
    unsigned plt_offset;
    int plt_sym;
    int dyn_index;
#ifdef DOBC_TARGET_ARM
    unsigned char plt_thumb_stub : 1;
#endif
};

struct preproc_define {
    char *name;
    char *value;
};

#include "slghsymbol.h"

struct VMState {
	unsigned long funcaddr;

    void *error_opaque;
    void (*error_func)(void *opaque, const char *msg);

    unsigned char* filedata;
    char *filename;
    int filelen;

    int verbose;
    int do_debug;

    /* output type, see DOBC_OUTPUT_XXX */
    int output_type;
    /* output format, see DOBC_OUTPUT_FORMAT_XXX */
    int output_format;

    addr_t  text_addr;  /* address of text section */
    unsigned long has_text_addr;

    int static_link;

    unsigned section_align;

    struct dynarray sections;
    struct dynarray priv_sections;

    char *init_symbol; /* symbols to call at load-time */
    char *fini_symbol; /* symbols to call at unload-time  */

    Section *got;
    Section *plt;

    Section *text_section;
    Section *data_section;
    Section *bss_section;
    Section *common_section;
    Section *cur_text_section;
    Section *symtab_section;
    Section *stab_section;

    Section *dynsymtab_section;
    /* 加载.a文件时，符号表是symtab_section，加载.so文件时，符号表是 dynsymtab_section
    symtab_secton1指向当前加载文件的符号表，以匹配不同的加载文件类型，他是 symtab_section和dynsymtab_section
    的软连接
    */
    Section *symtab_section1;
    Section *symtab;
    Section *stab;

    struct dynarray sym_attrs;

    struct pcode_ctx   pctx;

    /* SleighCompiler start ----- */

    /* 这里之所以不把SleighCompile整个进行模块化是为了写代码方便，后期假如需要提取，可以把
    start和end之间的部分提取出来放到新的文件里。

    PS. 过于模块化的代码导致上层往下层传递数据时，要携带大量信息，解耦本身带来的开销在这里我感觉超过它带来好处 */
    struct {
        int counts;
    } preproc_defines;

    struct dynarray     contexttable;   // FieldContext
    struct dynarray     macrotable;     // ConstructTpl
    struct dynarray     tokentable;     // Token
    struct dynarray     tables;         // SubtableSymbol

    /* Sleight Compiler end ----- */
};

#define VM_SET_STATE(fn)    fn        

#define vm_error_noabort    VM_SET_STATE(_vm_error_noabort)
#define vm_error(fmt, ...)  _vm_error(fmt " %s:%d", ##__VA_ARGS__, __FILE__, __LINE__)            
#define vm_warn             VM_SET_STATE(_vm_warning)

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

#define OPT_DUMP_ELF_HEADER         10
#define OPT_DUMP_ELF_PROG_HEADER    11
#define OPT_DUMP_ELF_SECTION        12
#define OPT_DUMP_ELF_REL            13
#define OPT_DUMP_ELF_DYNSYM         14

#define OPT_DECODE_ELF              20
#define OPT_DECODE_FUNC             21
#define OPT_TEST                    22

#define AFF_BINTYPE_REL         1
#define AFF_BINTYPE_DYN         2
#define AFF_BINTYPE_AR          3
#define AFF_BINTYPE_C67         4

int dobc_object_type(ElfW(Ehdr) *h);
int dobc_load_object_file(VMState *s1, int fd, unsigned long file_offset);
int dobc_load_archive(VMState *s1, int fd, int alacarte);
int dobc_load_dll(VMState *s1);

void *vm_realloc(void *ptr, unsigned long size);
void *vm_malloc(unsigned long size);
void *vm_mallocz(unsigned long size);
void vm_free(void *ptr);

int dobc_load_file(VMState *s1);

/* dobcelf.c */

#define DOBC_OUTPUT_FORMAT_ELF    0 /* default output format: ELF */
#define DOBC_OUTPUT_FORMAT_BINARY 1 /* binary image output */
#define DOBC_OUTPUT_FORMAT_COFF   2 /* COFF */

#define ARMAG  "!<arch>\012"    /* For COFF and a.out archives */

void dobcelf_new(VMState *s);
void dobcelf_delete(VMState *s);
int dobc_output_file(VMState *s1, const char *filename);

Section *new_section(VMState *s1, const char *name, int sh_type, int sh_flags);
void free_section(Section *sec);
void section_realloc(Section *sec, unsigned long new_size);
size_t section_add(Section *sec, addr_t size, int align);
void *section_ptr_add(Section *sec, addr_t size);
void section_reserve(Section *sec, unsigned long size);
Section *find_section(VMState *s1, const char *name);
Section *new_symtab(VMState *s1, const char *symtab_name, int sh_type, int sh_flags, const char *strtab_name, const char *hash_name, int hash_sh_flags);

struct sym_attr *get_sym_attr(VMState *s1, int index, int alloc);


#define text_section(s)             s->text_section            
#define data_section(s)             s->data_section
#define bss_section(s)              s->bss_section
#define common_section(s)           s->common_section
#define cur_text_section(s)         s->cur_text_section
#define symtab_section(s)           s->symtab_section
#define stab_section(s)             s->stab_section
#define stabstr_section(s)          stab_section(s)->link
#define gnu_ext(s)                  s->gnu_ext

#define CSTR_INIT(c)               CString c = {0}
CString *           cstr_cat(CString *cstr, const char *str, int len);
void                cstr_ccat(CString *cstr, int ch);
void                cstr_init(CString *cstr);
void                cstr_free(CString *cstr);
CString*            cstr_new(char *src, int len);
void                cstr_delete(CString *cstr);

char* str_new(char *src, int len);
void str_free(char *);

#endif