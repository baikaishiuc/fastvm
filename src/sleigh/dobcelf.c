
#include "mcore/mcore.h"
#include "vm.h"

#define SHF_PRIVATE         0x80000000
#define SHF_DYNSYM          0x40000000

struct dyn_inf {
    Section *dynamic;
    Section *dynstr;
    unsigned long data_offset;
    addr_t rel_addr;
    addr_t rel_size;
#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
    addr_t bss_addr;
    addr_t bss_size;
#endif
};

void dobcelf_new(VMState *s)
{
    dynarray_add(&s->sections, NULL);

#if 0
    text_section(s) = new_section(s, ".text", SHT_PROGBITS, SHF_ALLOC | SHF_EXECINSTR);
    data_section(s) = new_section(s, ".data", SHT_PROGBITS, SHF_ALLOC | SHF_WRITE);
    bss_section(s) = new_section(s, ".bss", SHT_NOBITS, SHF_ALLOC | SHF_WRITE);
    common_section(s) = new_section(s, ".common", SHT_NOBITS, SHF_PRIVATE);
    common_section(s)->sh_num = SHN_COMMON;

    symtab_section(s) = new_symtab(s, ".symtab", SHT_SYMTAB, 0, ".strtab", ".hashtab", SHF_PRIVATE);
    s->symtab = symtab_section(s);

    s->dynsymtab_section = new_symtab(s, ".dynsymtab", SHT_SYMTAB, SHF_PRIVATE | SHF_DYNSYM,
                                      ".dynstrtab",
                                      ".dynhashtab", SHF_PRIVATE);
#endif

    get_sym_attr(s, 0, 1);
}

void dobcelf_delete(VMState *s)
{
    int i;

    /* free all sections */
    for (i = 1; i < s->sections.len; i++) {
        free_section(s->sections.ptab[i]);
    }
    dynarray_reset(&s->sections);

    for (i = 0; i < s->priv_sections.len; i++) {
        free_section(s->priv_sections.ptab[i]);
    }
    dynarray_reset(&s->priv_sections);

    for (i = 0; i < s->sym_attrs.len; i++) {
        if (s->sym_attrs.ptab[i])
            free(s->sym_attrs.ptab[i]);
    }
    dynarray_reset(&s->sym_attrs);

    symtab_section(s) = NULL;
}

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

void free_section(Section *sec)
{
    vm_free(sec->data);
    vm_free(sec);
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
    p = (ElfW(Sym) *)(s->data);
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
    int shnum, file_type, offset, i, size;
    Section *s;
    ElfW(Ehdr) ehdr;
    ElfW(Shdr) shdr, *sh;

    file_type = s1->output_type;

    shnum = s1->sections.len;
    
    memset(&ehdr, 0, sizeof (ehdr));

    if (phnum > 0) {
        ehdr.e_phentsize = sizeof(ElfW(Phdr));
        ehdr.e_phnum = phnum;
        ehdr.e_phoff = sizeof(ElfW(Ehdr));
    }

    file_offset = (file_offset + 3) & -4;

    memcpy(&ehdr, s1->filedata, sizeof (ehdr));

    ehdr.e_shoff = file_offset;
    ehdr.e_ehsize = sizeof(ElfW(Ehdr));
    ehdr.e_shentsize = sizeof(ElfW(Shdr));
    ehdr.e_shnum = shnum;
    ehdr.e_shstrndx = shnum - 1;

    fwrite(&ehdr, 1, sizeof (ElfW(Ehdr)), f);
    fwrite(phdr, 1, phnum * sizeof (ElfW(Phdr)), f);
    offset = sizeof(ElfW(Ehdr)) + phnum * sizeof (ElfW(Phdr));

    /* 共享库里面的symtab为空，可以不用处理 */
    if (symtab_section(s1))
        sort_syms(s1, symtab_section(s1));
    for (i = 1; i < s1->sections.len; i++) {
        s = s1->sections.ptab[sec_order[i]];
        if (s->sh_type != SHT_NOBITS) {
            while (offset < (int)s->sh_offset) {
                fputc(0, f);
                offset++;
            }
            size = s->sh_size;
            if (size)
                fwrite(s->data, 1, size, f);
            offset += size;
        }
    }

    while (offset < (int)ehdr.e_shoff) {
        fputc(0, f);
        offset++;
    }

    for (i = 0; i < s1->sections.len; i++) {
        sh = &shdr;
        memset(sh, 0, sizeof (ElfW(Shdr)));
        s = s1->sections.ptab[i];
        if (s) {
            sh->sh_name = s->sh_name;
            sh->sh_type = s->sh_type;
            sh->sh_flags = s->sh_flags;
            sh->sh_entsize = s->sh_entsize;
            sh->sh_info = s->sh_info;
            if (s->link)
                sh->sh_link = s->link->sh_num;

            sh->sh_addralign = s->sh_addralign;
            sh->sh_addr = s->sh_addr;
            sh->sh_offset = s->sh_offset;
            sh->sh_size = s->sh_size;
        }
        fwrite(sh, 1, sizeof (ElfW(Shdr)), f);
    }
}

static Section *find_section_create(VMState *s1, const char *name, int create)
{
    Section *sec;
    int i;

    for (i = 1; i < s1->sections.len; i++) {
        sec = s1->sections.ptab[i];
        if (!strcmp(name, sec->name))
            return sec;
    }

    return create ? new_section(s1, name, SHT_PROGBITS, SHF_ALLOC):NULL;
}

/* return a reference to a section, and create it if it does not exists */
Section *find_section(VMState *s1, const char *name)
{
    return find_section_create(s1, name, 1);
}

/* write an elf, coff or "binary" file */
static int dobc_write_elf_file(VMState *s1, const char *filename, int phnum, ElfW(Phdr) *phdr, int file_offset, int *sec_order)
{
    FILE *f;

    f = fopen(filename, "wb");

    dobc_output_elf(s1, f, phnum, phdr, file_offset, sec_order);

    fclose(f);

    return 0;
}

/* realloc section and set its content to zero */
void section_realloc(Section *sec, unsigned long new_size)
{
    unsigned long size;
    unsigned char *data;

    size = sec->data_allocated;
    if (size == 0)
        size = 1;
    while (size < new_size)
        size = size * 2;

    data = vm_realloc(sec->data, size);
    memset(data + sec->data_allocated, 0, size - sec->data_allocated);
    sec->data = data;
    sec->data_allocated = size;
}

/* reserve at least 'size' bytes aligned per 'align' in section
   'sec' from current offset, and return the aligned offset */
size_t section_add(Section *sec, addr_t size, int align)
{
    size_t offset, offset1;

    offset = (sec->data_offset + align - 1) & -align;
    offset1 = offset + size;
    if ((sec->sh_type != SHT_NOBITS) && (offset1 > sec->data_allocated))
        section_realloc(sec, offset1);
    sec->data_offset = offset1;
    if (align > sec->sh_addralign)
        sec->sh_addralign = align;
    return offset;
}

void *section_ptr_add(Section *sec, addr_t size)
{
    size_t offset = section_add(sec, size, 1);
    return sec->data + offset;
}

int put_elf_str(Section *s, const char *sym)
{
    int offset, len;
    char *ptr;

    len = strlen(sym) + 1;
    offset = s->data_offset;
    ptr = section_ptr_add(s, len);
    memmove(ptr, sym, len);
    return offset;
}

/* Allocate strings for section names and decide if an unallocated section
   should be output.
   NOTE: the strsec section comes last, so its size is also correct */
static int alloc_sec_names(VMState *s1, int file_type, Section *strsec)
{
    int i;
    Section *s;
    int textrel = 0;

    /* Allocate strings for section names */
    for (i = 1; i < s1->sections.len; i++) {
        s = s1->sections.ptab[i];
        /* when generating a DLL, we include relocations but we may patch them */
        if (s1->do_debug && (s->sh_type != SHT_RELX) 
            || (file_type == DOBC_OUTPUT_OBJ) 
            || (s->sh_flags & SHF_ALLOC) || (i == (s1->sections.len - 1))) {
            s->sh_size = s->data_offset;
        }

        if (s->sh_size || (s->sh_flags & SHF_ALLOC)) {
            s->sh_name = put_elf_str(strsec, s->name);
        }
    }

    strsec->sh_size = strsec->data_offset;

    return textrel;
}

static int rebuild_got(VMState *s1)
{
    ElfW(Ehdr) *eh = (ElfW(Ehdr) *)s1->filedata;
    ElfW(Shdr) *sh, *link_sch, *got_sh, *shstr;
    ElfW(Rel) *rel, *rel_new;
    Section *s, *got_section;
    int i, rel_count, j;
    const char *name;

    link_sch = (ElfW(Shdr) *)(s1->filedata + eh->e_shoff) + s1->symtab_section1->sh_link;

    shstr = (ElfW(Shdr) *)(s1->filedata + eh->e_shoff) + eh->e_shstrndx;
    for (i = 1; i < eh->e_shnum; i++) {
        sh = (ElfW(Shdr) *)(s1->filedata + eh->e_shoff) + i;
        name = (char *)s1->filedata + shstr->sh_offset + sh->sh_name;
        if (!strcmp(name, ".got")) {
            got_sh = sh;
            got_section = s1->sections.ptab[i];
            break;
        }
    }

    for (i = 1; i < s1->sections.len; i++) {
        s = s1->sections.ptab[i];
        sh = (ElfW(Shdr) *)(s1->filedata + eh->e_shoff) + i;
        if (s->sh_type != SHT_REL)
            continue;

        rel_count = sh->sh_size / sh->sh_entsize;
        for (j = 0; j < rel_count; j++) {
            rel = ((ElfW(Rel) *)(s1->filedata + sh->sh_offset)) + j;
            rel_new = ((ElfW(Rel) *)s->data) + j;

            rel_new->r_offset = (int)(rel->r_offset - got_sh->sh_offset) + got_section->sh_offset;
        }
    }

    return 0;
}

/* Assign sections to segments and decide how are section laid out when loaded in memory*/
static int layout_sections(VMState *s1, ElfW(Phdr) *phdr, int phnum,
                           Section *interp, Section *strsec, 
                           struct dyn_inf *dyninf, int *sec_order)
{
    int i, j, k, file_type, sh_order_index, file_offset;
    unsigned long s_align;
    long long tmp;
    addr_t addr;
    ElfW(Phdr) *ph;
    Section *s;

    file_type = s1->output_type;
    sh_order_index = 1;
    file_offset = 0;
    if (s1->output_format == DOBC_OUTPUT_FORMAT_ELF)
        file_offset = sizeof(ElfW(Ehdr)) + phnum * sizeof (ElfW(Phdr));

    s_align = ELF_PAGE_SIZE;
    if (s1->section_align)
        s_align = s1->section_align;

    if (phnum > 0) {
        if (s1->has_text_addr) {
        }
        else {
            if (file_type == DOBC_OUTPUT_DLL)
                addr = 0;
            else
                addr = ELF_START_ADDR;

            addr += (file_offset & (s_align - 1));
        }

        ph = &phdr[0];
        /* leave one program headers for the program interpreter and one for 
           the program header table itself if need. These are done later as 
           they required section layout to be done first. */
        if (interp)
            ph += 2;

        for (j = 0; j < 2; j++) {
            ph->p_type = PT_LOAD;
            if (j == 0)
                ph->p_flags = PF_R | PF_X;
            else
                ph->p_flags = PF_R | PF_W;

            ph->p_align = s_align;
            /* Decide the layout of section loaded in memory. This must 
               be done before program headers are filled since they contain
               info about the layout. We do the following ordering: interp,
               symbol tables, relocations, progbits, nobits */
            for (k = 0; k < 5; k++) {
                for (i = 1; i < s1->sections.len; i++) {
                    s = s1->sections.ptab[i];
                    if (j == 0) {
                        if ((s->sh_flags & (SHF_ALLOC | SHF_WRITE)) != SHF_ALLOC) {
                            continue;
                        }
                    }
                    else {
                        if ((s->sh_flags & (SHF_ALLOC | SHF_WRITE)) != (SHF_ALLOC | SHF_WRITE)) {
                            continue;
                        }
                    }

                    if (s == interp) {
                        if (k) continue;
                    }
                    else if ((s->sh_type == SHT_DYNSYM
                        || s->sh_type == SHT_STRTAB
                        || s->sh_type == SHT_HASH)
                        && !strstr(s->name, ".stab")) {
                        if (k != 1)
                            continue;
                    }
                    else if (s->sh_type == SHT_RELX) {
                        if (k != 2)
                            continue;
                    }
                    else if (s->sh_type == SHT_NOBITS) {
                        if (k != 4)
                            continue;
                    }
                    else {
                        if (k != 3)
                            continue;
                    }

                    sec_order[sh_order_index++] = i;

                    /* section matches: we align it and add its size */
                    tmp = addr;
                    addr = (addr + s->sh_addralign - 1) & ~(s->sh_addralign - 1);
                    file_offset += (int)(addr - tmp);
                    s->sh_offset = file_offset;
                    s->sh_addr = addr;

                    /* update program header info */
                    if (ph->p_offset == 0) {
                        ph->p_offset = file_offset;
                        ph->p_vaddr = addr;
                        ph->p_paddr = ph->p_vaddr;
                    }

                    /* update dynamic relocation infos */
                    if (s->sh_type == SHT_RELX) {
                        if (dyninf->rel_size == 0)
                            dyninf->rel_addr = addr;
                        dyninf->rel_size += s->sh_size;
                    }

                    addr += s->sh_size;
                    if (s->sh_type != SHT_NOBITS)
                        file_offset += s->sh_size;
                }
            }

            if (j == 0) {
                /* Make the first PT_LOAD segment include the program 
                   headers itself (and the ELF headers as well), it'll 
                   come out with same memory use but will make various 
                   tools like binutils strip work better. */

                ph->p_offset &= ~(ph->p_align - 1);
                ph->p_vaddr &= ~(ph->p_align - 1);
                ph->p_paddr &= ~(ph->p_align - 1);
            }

            ph->p_filesz = file_offset - ph->p_offset;
            ph->p_memsz = addr - ph->p_vaddr;
            ph++;
            if (j == 0) {
                if (s1->output_format == DOBC_OUTPUT_FORMAT_ELF) {
                    /* if in the middle of a page, we duplicate the page in 
                       memory so that one copy is RX and the other is RW */
                    if ((addr & (s_align - 1)))
                        addr += s_align;
                }
                else {
                    addr = (addr + s_align - 1) & ~(s_align - 1);
                    file_offset = (file_offset + s_align - 1) & ~(s_align - 1);
                }
            }
        }
    }

    for (i = 1; i < s1->sections.len; i++) {
        s = s1->sections.ptab[i];
        if (phnum > 0 && (s->sh_flags & SHF_ALLOC))
            continue;

        sec_order[sh_order_index++] = i;

        file_offset = (file_offset + s->sh_addralign - 1) & ~(s->sh_addralign - 1);
        s->sh_offset = file_offset;
        if (s->sh_type != SHT_NOBITS)
            file_offset += s->sh_size;
    }

    return file_offset;
}

/*
老的tcc中的layout_sections看不懂···,它针对dll只分配了3个segment，然后按照一个它默认的顺序去分配它
我重新实现了以后，改成读取原生的so中的映射顺序，不在改动

*/
static int layout_sections2(VMState *s1, ElfW(Phdr) *phdr, int phnum, int *sec_order)
{
    ElfW(Ehdr) *ehdr;
    ElfW(Phdr) *ph, *ph_new;
    ElfW(Shdr) *sh;
    int i, j, file_offset, sh_order_index = 1, s_align, t, *sec_to_seg, data_start_section = 0;
    addr_t addr;
    Section *s;

    ehdr = (ElfW(Ehdr) *)s1->filedata;

    sec_to_seg = vm_malloc(s1->sections.len * sizeof (sec_to_seg[0]));
    memset(sec_to_seg, -1, s1->sections.len * sizeof (sec_to_seg[0]));
    //file_offset = sizeof(ElfW(Ehdr)) + phnum * sizeof(ElfW(Phdr));
    // phdr的segment 4字节对齐

    for (i = 1; i < s1->sections.len; i++) {
        sec_order[sh_order_index++] = i;
    }

    for (i = 0; i < phnum; i++) {
        ph = (ElfW(Phdr) *)(s1->filedata + ehdr->e_phoff) + i;
        for (j = 1; j < ehdr->e_shnum; j++) {
            sh = (ElfW(Shdr) *)(s1->filedata + ehdr->e_shoff) + j;
            if (ELF_SECTION_IN_SEGMENT(sh, ph)) {
                sec_to_seg[j] = i;

                if ((ph->p_type == PT_LOAD) && (ph->p_flags & PF_W) && !data_start_section) {
                    data_start_section = j;
                    s_align = ph->p_align;
                }
            }
        }
    }

    file_offset = sizeof(ElfW(Ehdr)) + phnum * sizeof (phdr[0]);
    addr = (file_offset & ~3);
    for (j = 1; j < ehdr->e_shnum; j++) {
        s = s1->sections.ptab[j];

        addr = ALIGN_UP(addr, s->sh_addralign);
        file_offset = ALIGN_UP(file_offset, s->sh_addralign);
        
        /* 有些特殊的section，他的addr为空，看起来是不需要载入到内存中 */
        if (strcmp(s->name, ".comment")
            && strcmp(s->name, ".note.gnu.gold-version")
            && strcmp(s->name, ".ARM.attributes")
            && strcmp(s->name, ".shstrtab")) {

            if ((data_start_section == j) && (addr & (s_align - 1))) {
                addr += s_align;
            }

            s->sh_addr = addr;
        }

        s->sh_offset = file_offset;

        addr += s->sh_size;

        if (s->sh_type != SHT_NOBITS)
            file_offset += s->sh_size;

        printf("name = %s, section = %d, sh_size = %d, phdr = %d\n", s->name, j, s->sh_size, sec_to_seg[j]);
    }

    for (i = 0; i < phnum; i++) {
        ph = (ElfW(Phdr) *)(s1->filedata + ehdr->e_phoff) + i;
        ph_new = &phdr[i];

        ph_new->p_align = ph->p_align;
        ph_new->p_flags = ph->p_flags;
        ph_new->p_type = ph->p_type;

        file_offset = 0;
        addr = 0;

        for (j = 1; j < ehdr->e_shnum; j++) {
            sh = (ElfW(Shdr) *)(s1->filedata + ehdr->e_shoff) + j;
            s = s1->sections.ptab[j];
            if (ELF_SECTION_IN_SEGMENT(sh, ph)) {
                if (0 == ph_new->p_offset) {
                    addr = s->sh_addr;
                    file_offset = s->sh_offset;

                    ph_new->p_offset = file_offset;
                    ph_new->p_vaddr = addr;
                    ph_new->p_paddr = ph_new->p_vaddr;
                }
                addr = ALIGN_UP(s->sh_addr, s->sh_addralign) + s->sh_size;
                if (s->sh_type != SHT_NOBITS)
                    file_offset = ALIGN_UP(file_offset, s->sh_addralign) + s->sh_size;
            }
        }

        /* PHDR segment */
        if (i == 0) {
            file_offset = sizeof(ElfW(Ehdr));
            addr = ALIGN_BOTTOM(file_offset, 4);

            ph_new->p_offset = addr;
            ph_new->p_paddr = addr;
            ph_new->p_vaddr = addr;

            t = phnum * sizeof (ElfW(Phdr));
            addr += t;
            file_offset += t;
        }
        /* the first PT_LOAD segment include the program itself */
        else if (i == 1) {
            ph_new->p_offset &= ~(ph_new->p_align - 1);
            ph_new->p_vaddr &= ~(ph_new->p_align - 1);
            ph_new->p_paddr &= ~(ph_new->p_align - 1);
        }

        ph_new->p_filesz = file_offset - ph_new->p_offset;
        ph_new->p_memsz = addr - ph_new->p_vaddr;
    }

    s = (s1->sections.ptab[s1->sections.len - 1]);

    return ALIGN_UP(s->sh_offset + s->sh_size, 4);
}

static int elf_output_file(VMState *s1, const char *filename)
{
    int ret, phnum, shnum, file_type, file_offset, *sec_order, i;
    struct dyn_inf dyninf = { 0 };
    ElfW(Phdr) *phdr;
    ElfW(Ehdr) ehdr;
    Section *strsec, *interp, *dynamic, *dynstr, *s;

    file_type = s1->output_type;
    phdr = NULL;
    sec_order = NULL;
    interp = dynamic = dynstr = NULL;

    /* we add a section for symbols */
    if (!(strsec = find_section_create(s1, ".shstrtab", 0))) {
        strsec = new_section(s1, ".shstrtab", SHT_STRTAB, 0);
        put_elf_str(strsec, "");
    }

    /* Allocate strings for sections names */
    //ret = alloc_sec_names(s1, file_type, strsec);
    for (i = 1; i < s1->sections.len; i++) {
        s = s1->sections.ptab[i];
        s->sh_size = s->data_offset;
    }

    memcpy(&ehdr, s1->filedata, sizeof (ehdr));
    phnum = ehdr.e_phnum;

    /* allocate program segment headers */
    phdr = vm_mallocz(phnum * sizeof (ElfW(Phdr)));

    /* compute number of section */
    shnum = s1->sections.len;

    /* this array is used to reorder sections in the output file */
    sec_order = vm_malloc(sizeof (int) * shnum);

    /* compute section to program header mapping */
    //file_offset = layout_sections(s1, phdr, phnum, interp, strsec, &dyninf, sec_order);
    file_offset = layout_sections2(s1, phdr, phnum, sec_order);

    rebuild_got(s1);

    ret = dobc_write_elf_file(s1, filename, phnum, phdr, file_offset, sec_order);

    vm_free(sec_order);
    vm_free(phdr);

    return 0;
}

int full_read(int fd, void *buf, size_t count) 
{
    char *cbuf = buf;
    int rnum = 0;
    while (1) {
        int num = read(fd, cbuf, count - rnum);
        if (num < 0) return num;
        if (num == 0) return rnum;
        rnum += num;
        cbuf += num;
    }
}

static void *load_data(int fd, unsigned long file_offset, unsigned long size)
{
    void *data;
    data = vm_malloc(size);
    lseek(fd, file_offset, SEEK_SET);
    full_read(fd, data, size);
    return data;
}

struct versym_info {
    int nb_versyms;
    ElfW(Verdef) *verdef;
    ElfW(Verneed) *verneed;
    ElfW(Half) *versym;
    int nb_local_ver, *local_ver;
};

static unsigned long elf_hash(const unsigned char *name)
{
    unsigned long h = 0, g;

    while (*name) {
        h = (h << 4) + *name++;
        g = h & 0xf0000000;
        if (g)
            h ^= g >> 24;
        h &= ~g;
    }

    return h;
}

int find_elf_sym(Section *s, const char *name)
{
    ElfW(Sym) *sym;
    Section *hs;
    int nbuckets, sym_index, h;
    const char *name1;

    hs = s->hash;
    if (!hs)
        return 0;

    nbuckets = ((int *)hs->data)[0];
    h = elf_hash((unsigned char *)name) % nbuckets;
    sym_index = ((int *)hs->data)[2 + h];
    while (sym_index) {
        sym = &((ElfW(Sym) *)s->data)[sym_index];
        name1 = (char *)s->link->data + sym->st_name;
        if (!strcmp(name, name1))
            return sym_index;
        sym_index = ((int *)hs->data)[2 + nbuckets + sym_index];
    }

    return sym_index;
}

/* rebuild hash table of section s */
/* NOTE: we do factorize the hash table code to go faster */
static void rebuild_hash(Section *s, unsigned int nb_buckets)
{
    ElfW(Sym) *sym;
    int *ptr, *hash, nb_syms, sym_index, h;
    unsigned char *strtab;

    strtab = s->link->data;
    nb_syms = s->data_offset / sizeof(ElfW(Sym));

    if (!nb_buckets)
        nb_buckets = ((int *)s->hash->data)[0];

    s->hash->data_offset = 0;
    ptr = section_ptr_add(s->hash, (2 + nb_buckets + nb_syms) * sizeof (int));
    ptr[0] = nb_buckets;
    ptr[1] = nb_syms;
    ptr += 2;
    hash = ptr;
    memset(hash, 0, (nb_buckets + 1) * sizeof (int));
    ptr += nb_buckets + 1;

    sym = (ElfW(Sym) *)s->data + 1;
    for (sym_index = 1; sym_index < nb_syms; sym_index ++) {
        if (ELFW(ST_BIND)(sym->st_info) != STB_LOCAL) {
            h = elf_hash(strtab + sym->st_name) % nb_buckets;
            *ptr = hash[h];
            hash[h] = sym_index;
        }
        else {
            *ptr = 0;
        }
        ptr++;
        sym++;
    }
}

/* return the symbol number */
int put_elf_sym(Section *s, addr_t value, unsigned long size,
    int info, int other, int shndx, const char *name)
{
    int name_offset, sym_index;
    int nbuckets, h;
    ElfW(Sym) *sym;
    Section *hs;

    sym = section_ptr_add(s, sizeof (ElfW(Sym)));
    if (name && name[0])
        name_offset = put_elf_str(s->link, name);
    else
        name_offset = 0;

    sym->st_name = name_offset;
    sym->st_value = value;
    sym->st_size = size;
    sym->st_info = info;
    sym->st_other = other;
    sym->st_shndx = shndx;
    sym_index = sym - (ElfW(Sym) *)s->data;
    hs = s->hash;
    if (hs) {
        int *ptr, *base;
        ptr = section_ptr_add(hs, sizeof (int));
        base = (int *)hs->data;
        if (ELFW(ST_BIND)(info) != STB_LOCAL) {
            /* add another hashing entry */
            nbuckets = base[0];
            h = elf_hash((unsigned char *)s->link->data + name_offset) % nbuckets;
            *ptr = base[h + 2];
            base[h + 2] = sym_index;
            base[1]++;
            /* we resize the hash table */
            hs->nb_hashed_syms++;
            if (hs->nb_hashed_syms > 2 * nbuckets)
                rebuild_hash(s, 2 * nbuckets);
        }
        else {
            *ptr = 0;
            base[1]++;
        }
    }

    return sym_index;
}

/* add an elf symbol : check if it is already defined and patch it. 
   Return symbol index. Note that sh_num can be SH_UNDEF */
int set_elf_sym(Section *s, addr_t value, unsigned long size, 
                int info, int other, int shndx, const char *name)
{
    VMState *s1 = s->s1;
    ElfW(Sym) *esym;
    int sym_bind, sym_index, sym_type, esym_bind;
    unsigned char sym_vis, esym_vis, new_vis;

    sym_bind = ELFW(ST_BIND)(info);
    sym_type = ELFW(ST_TYPE)(info);
    sym_vis = ELFW(ST_VISIBILITY)(other);

    if (sym_bind != STB_LOCAL) {
        /* we search global or weak symbols */
        sym_index = find_elf_sym(s, name);
        if (!sym_index)
            goto do_def;
        esym = &((ElfW(Sym) *)s->data)[sym_index];
        /* FIXME:长度相等，只是修改了几个字节，也是需要update */
        if (esym->st_value == value && esym->st_size == esym->st_size && esym->st_info == info
            && esym->st_other == other && esym->st_shndx == shndx)
            return sym_index;

        if (esym->st_shndx != SHN_UNDEF) {
            esym_bind = ELFW(ST_BIND)(esym->st_info);
            /* propagate the most constraining visibility */
            /* STV_DEFAULT(0)<STV_PROTECTED(3)<STV_HIDDEN(2)<STV_INTERNAL(1) */
            /* 作用域跟新时，假如变小就停止跟新，变大就跟新 */
            esym_vis = ELFW(ST_VISIBILITY)(esym->st_other);
            if (esym_vis == STV_DEFAULT) {
                new_vis = sym_vis;
            }
            else if (sym_vis == STV_DEFAULT) {
                new_vis = esym_vis;
            }
            else {
                new_vis = (esym_vis < sym_vis) ? esym_vis : sym_vis;
            }
            esym->st_other = (esym->st_other & ~ELFW(ST_VISIBILITY)(-1)) | new_vis;
            other = esym->st_other;
            if (shndx == SHN_UNDEF) {
                /* ignore adding of undefined symbol if the 
                   corresponding  symbol is already defined */
            }
            else if (sym_bind == STB_GLOBAL && esym_bind == STB_WEAK) {
                /* global overrides weak, so patch */
                goto do_patch;
            }
            else if (sym_bind == STB_WEAK && esym_bind == STB_GLOBAL) {
                /* weak is ignored if already global */
            }
            else if (sym_bind == STB_WEAK && esym_bind == STB_WEAK) {
                /* keep first-found weak definitions, ignore subsequents */
            }
            else if (sym_vis == STV_HIDDEN || sym_vis == STV_INTERNAL) {
                /* ignore hidden symbols after */
            }
            else if ((esym->st_shndx == SHN_COMMON || esym->st_shndx == bss_section(s1)->sh_num)
                && (shndx < SHN_LORESERVE && shndx != bss_section(s1)->sh_num)) {
                /* data symbols gets precedence over common/bss */
                goto do_patch;
            }
            else if (shndx == SHN_COMMON || shndx == bss_section(s1)->sh_num) {
                /* data symbols keeps precedence over common/bss */
            }
            else if (s->sh_flags & SHF_DYNSYM) {
                /* we accept that two DLL define the same symbol */
            }
            //else if (esym->st_other & ST_ASM_SET)
            else {
                vm_error("%s defined twice", name);
            }
        }
        else {
        do_patch:
            esym->st_info = ELFW(ST_INFO)(sym_bind, sym_type);
            esym->st_shndx = shndx;
            // s1->new_undef_sym = 1;
            esym->st_value = value;
            esym->st_size = size;
            esym->st_other = other;
        }
    }
    else {
    do_def:
        sym_index = put_elf_sym(s, value, size, 
                                ELFW(ST_INFO)(sym_bind, sym_type), other, shndx, name);
    }

    return sym_index;
}

/* load a DLL and all referenced DLLs. 'level = 0 ' means that the DLL 
   is referenced by the user (so it should be added as DT_NEEDED in 
   the generated ELF file)*/
int dobc_load_dll(VMState *s1)
{
    ElfW(Ehdr) ehdr;
    ElfW(Shdr) *shdr, *sh, *sh1, *shstrtab;
    int i, nb_syms, nb_dts, sym_bind, j, size;
    ElfW(Sym) *sym, *dynsym;
    ElfW(Dyn) *dynamic;
    Section *s, *hash_sec;

    char *dynstr;
    int sym_index;
    const char *name, *sh_name;
    struct versym_info v;

    memcpy((char *)&ehdr, s1->filedata, sizeof (ehdr));

    if ((ehdr.e_ident[5] != ELFDATA2LSB) || (ehdr.e_machine != EM_ARM))
        vm_error("load dll failure, bad architecture");

    /* read section */
    shdr = (ElfW(Shdr) *)(s1->filedata + ehdr.e_shoff);
    shstrtab = &shdr[ehdr.e_shstrndx];

    /* load dynamic section and dynamic symbols */
    nb_syms = 0;
    nb_dts = 0;
    dynamic = NULL;
    dynsym = NULL; /* avoid warning */
    dynstr = NULL; /* avoid warning */
    memset(&v, 0, sizeof (v));

    for (i = 0, sh = shdr; i < ehdr.e_shnum; i++, sh++) {
        switch (sh->sh_type) {
        case SHT_DYNAMIC:
            nb_dts = sh->sh_size / sizeof(ElfW(Dyn));
            dynamic = (ElfW(Dyn) *)(s1->filedata + sh->sh_offset);
            break;

        case SHT_DYNSYM:
            nb_syms = sh->sh_size / sizeof(ElfW(Sym));
            dynsym = (ElfW(Sym) *)(s1->filedata + sh->sh_offset);
            sh1 = &shdr[sh->sh_link];
            dynstr = s1->filedata + sh1->sh_offset;
            break;
        }
    }

    for (i = 1; i < ehdr.e_shnum; i++) {
        sh = &shdr[i];
        sh_name = s1->filedata + shstrtab->sh_offset + sh->sh_name;
        for (j = 1; j < s1->sections.len; j++) {
            s = s1->sections.ptab[j];
            if (!strcmp(s->name, sh_name)) {
                goto found;
            }
        }

        /* not found: create new section */
        s = new_section(s1, sh_name, sh->sh_type, sh->sh_flags & ~SHF_GROUP);

        /* take as much info as possible from the section. sh_link and
           sh_info will be updated later */
        s->sh_addralign = sh->sh_addralign;
        s->sh_entsize = sh->sh_entsize;
        s->sh_info = sh->sh_info;
        s->sh_link = sh->sh_link;
        s->sh_name = sh->sh_name;
found:
        if (sh->sh_type != s->sh_type) {
            vm_error("invalid section type %d", sh->sh_type);
        }

        if (sh->sh_type == SHT_DYNSYM) {
            s1->dynsymtab_section = s;
            s1->symtab_section1 = s;
        }
        else if (sh->sh_type == SHT_HASH)       hash_sec = s;

        /* align start of section */
        s->data_offset += s->data_offset & (sh->sh_addralign - 1);
        if ((int)sh->sh_addralign > s->sh_addralign)
            s->sh_addralign = sh->sh_addralign;

        size = sh->sh_size;
        if (sh->sh_type != SHT_NOBITS) {
            unsigned char *ptr;
            ptr = section_ptr_add(s, size);
            memcpy(ptr, s1->filedata + sh->sh_offset, size);
        }
        else {
            s->data_offset += size;
        }
    }

    /* second short pass to update sh_link */
    for (i = 1; i < ehdr.e_shnum; i++) {
        sh = &shdr[i];
        s = s1->sections.ptab[i];
        if (sh->sh_link) {
            s->link = s1->sections.ptab[sh->sh_link];
        }
    }

    if (s1->dynsymtab_section) {
        s1->dynsymtab_section->hash = hash_sec;
    }

    for (i = 1, sym = dynsym + 1; i < nb_syms; i++, sym++) {
        sym_bind = ELFW(ST_BIND)(sym->st_info);
        if (sym_bind == STB_LOCAL)
            continue;

        name = dynstr + sym->st_name;
        sym_index = set_elf_sym(s1->dynsymtab_section, sym->st_value, sym->st_size, 
                                sym->st_info, sym->st_other, sym->st_shndx, name);
    }

    /* load symtab and strtab */

    return 0;
}

int dobc_object_type(ElfW(Ehdr) *h)
{
    if (0 == memcmp(h, ELFMAG, 4)) {
        if (h->e_type == ET_REL)
            return AFF_BINTYPE_REL;
        if (h->e_type == ET_DYN)
            return AFF_BINTYPE_DYN;
    }
    else if (0 == memcmp(h, ARMAG, 8)) {
        return AFF_BINTYPE_AR;
    }

    return 0;
}

struct sym_attr *get_sym_attr(VMState *s1, int index, int alloc)
{
    struct sym_attr *tab;

    if (index >= s1->sym_attrs.len) {
        if (!alloc)
            return s1->sym_attrs.ptab[0];

        tab = vm_mallocz(sizeof (tab[0]));
        dynarray_add(&s1->sym_attrs, tab);
    }

    return s1->sym_attrs.ptab[index];
}

Section *new_symtab(VMState *s1,
    const char *symtab_name, int sh_type, int sh_flags,
    const char *strtab_name,
    const char *hash_name, int hash_sh_flags)
{
    Section *symtab, *strtab, *hash;
    int *ptr, nb_buckets;

    symtab = new_section(s1, symtab_name, sh_type, sh_flags);
    symtab->sh_entsize = sizeof(ElfW(Sym));
    strtab = new_section(s1, strtab_name, SHT_STRTAB, sh_flags);
    put_elf_str(strtab, "");
    symtab->link = strtab;
    put_elf_sym(symtab, 0, 0, 0, 0, 0, NULL);

    nb_buckets = 1;

    hash = new_section(s1, hash_name, SHT_HASH, hash_sh_flags);
    hash->sh_entsize = sizeof(int);
    symtab->hash = hash;
    hash->link = symtab;

    ptr = section_ptr_add(hash, (2 + nb_buckets + 1) * sizeof (int));
    ptr[0] = nb_buckets;
    ptr[1] = 1;
    memset(ptr + 2, 0, (nb_buckets + 1) * sizeof (int));
    return symtab;
}

int dobc_output_file(VMState *s1, const char *filename)
{
    return elf_output_file(s1, filename);
}
