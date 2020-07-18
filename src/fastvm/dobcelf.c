
#include "mcore/mcore.h"
#include "vm.h"
#include "elf.h"

#define SHF_PRIVATE         0x80000000
#define SHF_DYNSYM          0x40000000

Section *new_section(VMState *s1, const char *name, int sh_type, int sh_flags)
{
    Section *sec;

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

/* Browse each elem of type <type> in section <sec> starting at elem <startoff>
   using variable <elem>. */
#define for_each_elem(sec, startoff, elem, type) \
    for (elem = (type *) sec->data + startoff; \
         elem < (type *)(sec->data + sec->data_offset); elem++) 


/* In an ELF file symbol table, the local symbols must appear below 
   the global and weak ones. Since dobc cannot sort it while generating 
   the code, we must do it after. All the relocation tables are also 
   modified to take in account the symbol table sorting. */
static void sort_syms(VMState *s1, Section *s)
{
    int *old_to_new_syms;
    ElfW(Sym)   *new_syms;
    int nb_syms, i;
    ElfW(Sym)   *p, *q;
    ElfW_Rel *rel;
    Section *sr;
    int type, sym_index;

    nb_syms = s->data_offset / sizeof(ElfW(Sym));
    new_syms = vm_malloc(nb_syms * sizeof (ElfW(Sym)));
    old_to_new_syms = vm_malloc(nb_syms * sizeof (int));

    /* first pass for local symbols */
    p = (ElfW(Sym) *)(s->data);
    q = new_syms;
    for (i = 0; i < nb_syms; i++, p++) {
        if (ELFW(ST_BIND)(p->st_info) == STB_LOCAL) {
            old_to_new_syms[i] = q - new_syms;
            *q++ = *p;
        }
    }

    /* save the number of local symbols in section header */
    if (s->sh_size)
        s->sh_info = q - new_syms;

    /* the second pass for non local symbols */
    for (i = 0; i < nb_syms; i++, p++) {
        if (ELFW(ST_BIND)(p->st_info) != STB_LOCAL) {
            old_to_new_syms[i] = q - new_syms;
            *q++ = *p;
        }
    }

    /* we copy the new symbols to the old */
    memcpy(s->data, new_syms, nb_syms * sizeof (ElfW(Sym)));
    vm_free(new_syms);

    /* now we modify all the relocation */
    for (i = 1; i < s1->sections.len; i++) {
        sr = s1->sections.ptab[i];
        if ((sr->sh_type == SHT_RELX) && (sr->link == s)) {
            for_each_elem(sr, 0, rel, ElfW_Rel) {
                sym_index = ELFW(R_SYM)(rel->r_info);
                type = ELFW(R_TYPE)(rel->r_info);
                sym_index = old_to_new_syms[sym_index];
                rel->r_info = ELFW(R_INFO)(sym_index, type);
            }
        }
    }

    vm_free(old_to_new_syms);
}

static void dobc_output_elf(VMState *s1, FILE *f, int phnum, ElfW(Phdr) *phdr, int file_offset, int *sec_order)
{
    int shnum, file_type, offset;
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

#if !defined(DOBC_TARGET_) && (defined(__FreeBSD__) || defined(__FreeBSD_kernel))
    ehdr.e_indent[EI_OSABI] = ELFOSABI_FREEBSD;
#endif // (defined(__FreeBSD__) || defined(__FreeBSD_kernel))

#ifdef DOBC_TARGET_ARM

#ifdef DOBC_ARM_EABI
    ehdr.e_ident[EI_OSABI] = 0;
    ehdr.e_flags = EF_ARM_EABI_VER4;
#else // DOBC_ARM_EABI
    ehdr.e_ident[EI_OSABI] = ELFOSABI_ARM;
#endif // DOBC_ARM_EABI

#endif // DOBC_TARGET_ARM

    ehdr.e_type = ET_DYN;
    ehdr.e_machine = EM_ARM;
    ehdr.e_version = EV_CURRENT;
    ehdr.e_shoff = file_offset;
    ehdr.e_ehsize = sizeof(ElfW(Ehdr));
    ehdr.e_shentsize = sizeof(ElfW(Shdr));
    ehdr.e_shnum = shnum;
    ehdr.e_shstrndx = shnum - 1;

    fwrite(&ehdr, 1, sizeof (ElfW(Ehdr)), f);
    fwrite(phdr, 1, phnum * sizeof (ElfW(Phdr)), f);
    offset = sizeof(ElfW(Ehdr)) + phnum * sizeof (ElfW(Phdr));

    //sort_syms(s1, );

    return;
}
