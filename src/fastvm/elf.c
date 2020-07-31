
#include <stdio.h>
#include "elf.h"
#include "vm.h"

const char *osabistr[] = {
    "UNIX System V ABI",    /* 0 */
    "HP-UX",                /* 1 */
    "NetBSD",   /* 2 */
    "Object uses GNU ELF extensions.",
    "Sun Solaris",
    "IBM AIX",
    "SGI Irix",
    "FreeBSD",
    "Compaq TRU64 UNIX",
    "Novell Modesto",
    "OpenBSD",
};

const char *objtypestr[] = {
    "NONE (No file type)",
    "REL (Relocatable file)",
    "EXEC (Executable file)",
    "DYN (Shared object file)",
    "CORE (Core file)",
    "NUM (Num Of defined type)"
};

const char *elf_osabi2str(int osabi)
{
    if (osabi <= ELFOSABI_OPENBSD) {
        return osabistr[osabi];
    }

    if (osabi == ELFOSABI_ARM_AEABI)    return "ARM EABI";
    if (osabi == ELFOSABI_ARM)          return "ARM";
    if (osabi == ELFOSABI_STANDALONE)   return "Standalone (embedded) application";

    return "Unknown";
}

const char *elf_objtype2str(int objtype)
{
    if (objtype <= 5)
        return objtypestr[objtype];

    return "Unknown";
}

struct _machinestr {
    int id;
    const char *str;
} machinestr[] = {
    {0, "NONE"},

    {1, "NONE"},
    {2, "M3"},
    {3, "SPARC"},
    {4, "386"},
    {5, "68K"},
    {6, "88K"},  /* Motorola m88k family */
    {7, "860"},  /* Intel 80860 */
    {8, "MIPS"}, /* MIPS R3000 big-endian */
    {9, "S370"}, /* IBM System/370 */
    {10, "MIPS_RS3_LE"},
    {15, "PARISC"},
    {17, "VPP500"},
    {18, "SPARC32PLUS"},
    {19, "960"},
    {20, "PPC"},
    {21, "PPC64"},
    {22, "S390"},
    {36, "V800"},
    {37, "FR20"},
    {38, "RH32"},
    {39, "RCE"},
    {40, "ARM"},
    {41, "FAKE_ALPHA"},
    {42, "SH"},
    {43, "SPARCV9"},
    {44, "TRICORE"},
    {45, "ARC"},
    {46, "H8_300"},
    {47, "H8_300H"},
    {48, "H8S"},
    {49, "H8_500"},
    {50, "IA_64"},
    {51, "MIPS_X"},
    {52, "COLDFIRE"},
    {53, "68HC12"},
    {54, "MMA"},
    {55, "PCP"},
    {56, "NCPU"},
    {57, "NDR1"},
    {58, "STARCORE"},
    {59, "ME16"},
    {60, "ST100"},
    {61, "TINYJ"},
    {62, "X86_64"},
    {63, "PDSP"},
    {66, "FX66"},
    {67, "ST9PLUS"},
    {68, "ST7"},
    {69, "68HC16"},
    {70, "68HC11"},
    {71, "68HC08"},
    {72, "68HC05"},
    {73, "SVX"},
    {74, "ST19"},
    {75, "VAX"},
    {76, "CRIS"},
    {77, "JAVELIN"},
    {78, "FIREPATH"},
    {79, "ZSP"},
    {80, "MMIX"},
    {81, "HUANY"},
    {82, "PRISM"},
    {83, "AVR"},
    {84, "FR30"},
    {85, "D10V"},
    {86, "D30V"},
    {87, "V850"},
    {88, "M32R"},
    {89, "MN10300"},
    {90, "MN10200"},
    {91, "PJ"},
    {92, "OPENRISC"},
    {93, "ARC_A5"},
    {94, "XTENSA"},
    {183, "AARCH64"},
    {188, "TILEPRO"},
    {191, "TILEGX"},
    {243, "RISCV"},
    {253, "NUM"},
    {0x9026, "ALPHA"},
    {0x9c60, "C60"},
};

const char * elf_machine2str(int machine)
{
    int i;

    for (i = 0; i < count_of_array(machinestr); i++) {
        if (machinestr[i].id == machine)
            return machinestr[i].str;
    }

    return "Unknown";
}

const char *elf_flags2str(int flags)
{
    return "Unknown";
}

const char *elf_version2str(int version)
{
    if (version == EV_NONE)     return "NONE";
    if (version == EV_CURRENT)  return "Current";
    if (version == EV_NUM)      return "Num";

    return "Unknown";
}

struct progtype {
    int id;
    const char *str;
} progtypelist[] = {
    {PT_NULL,           "NULL"},
    {PT_LOAD,           "LOAD"},
    {PT_DYNAMIC,        "DYNAMIC"},
    {PT_INTERP,         "INTERP"},
    {PT_NOTE,           "NOTE"},
    {PT_SHLIB,          "SHLIB"},
    {PT_PHDR,           "PHDR"},
    {PT_TLS,            "TLS"},
    {PT_NUM,            "NUM"},
    {PT_LOOS,           "LOOS"},
    {PT_GNU_EH_FRAME,   "GNU_EH_FRARME"},
    {PT_GNU_STACK,      "GNU_STACK"},
    {PT_GNU_RELRO,      "GNU_RELRO"},
    {PT_LOSUNW,         "LOSUNW"},
    {PT_SUNWBSS,        "SUNWBSS"},
    {PT_SUNWSTACK,      "SUNWSTACK"},
    {PT_HISUNW,         "HISUNW"},
    {PT_HIOS,           "HIOS"},
    {PT_LOPROC,         "LOPROC"},
    {PT_HIPROC,         "HIPROC"},
};

const char *elf_progtype2str(int progtype)
{
    int i;
    for (i = 0; i < count_of_array(progtypelist); i++) {
        if (progtypelist[i].id == progtype)
            return progtypelist[i].str;
    }

    return "Unknown";
}

struct {
    char *str;
    int id;
} sectypelist[] = {
    { "NULL",            0 },
    { "PROGBITS",        1 },
    {"SYMTAB",            2 },        
    {"STRTAB",            3 },       
    {"RELA",            4 }, 
    {"HASH",            5 },
    {"DYNAMIC",            6 },
    {"NOTE",            7 },
    {"NOBITS",            8 },
    {"REL",                9 },
    {"SHLIB",            10},
    {"DYNSYM",            11},
    {"INIT_ARRAY",      14},
    {"FINI_ARRAY",      15},
    {"PREINIT_ARRAY",    16},
    {"GROUP",            17},
    {"SYMTAB_SHNDX",    18},
    {"NUM",                19},
    {"LOOS",            0x60000000},
    {"GNU_ATTRIBUTES",    0x6ffffff5},
    {"GNU_HASH",        0x6ffffff6},
    {"GNU_LIBLIST",     0x6ffffff7},
    {"CHECKSUM",        0x6ffffff8},
    {"LOSUNW",            0x6ffffffa},
    {"SUNW_move",        0x6ffffffa},
    {"SUNW_COMDAT",        0x6ffffffb},
    {"SUNW_syminfo",    0x6ffffffc},
    {"GNU_verdef",      0x6ffffffd},
    {"GNU_verneed",     0x6ffffffe},
    {"GNU_versym",      0x6fffffff},
    {"HISUNW",            0x6fffffff },
    {"HIOS",            0x6fffffff},
    {"LOPROC",            0x70000000},
    {"HIPROC",            0x7fffffff},
    {"LOUSER",            0x80000000},
    {"HIUSER",            0x8fffffff},
    {"ARM_EXIDX",        0x70000001},
    {"ARM_ATTRIBUTES",    0x70000003},
};

const char *elf_sectype2str(int sectype)
{
    int i;

    for (i = 0; i < count_of_array(sectypelist); i++) {
        if (sectypelist[i].id == sectype)
            return sectypelist[i].str;
    }

    return "Unknown";
}

const char *elf_secflag2str(int flags)
{
    static char buf[128];
    int i = 0;

    if (flags & SHF_WRITE) buf[i++] = 'W';
    if (flags & SHF_ALLOC) buf[i++] = 'A';
    if (flags & SHF_EXECINSTR) buf[i++] = 'E';
    if (flags & SHF_MERGE) buf[i++] = 'M';
    if (flags & SHF_STRINGS) buf[i++] = 'S';
    if (flags & SHF_INFO_LINK) buf[i++] = 'I';
    if (flags & SHF_LINK_ORDER) buf[i++] = 'L';
    if (flags & SHF_OS_NONCONFORMING) buf[i++] = 'O';

    buf[i] = 0;

    return buf;
}

Elf32_Shdr *elf32_shdr_get(Elf32_Ehdr *hdr, int type)
{
	int i;
	Elf32_Shdr *shdr;

	for (i = 1; i < hdr->e_shnum; i++) {
		shdr = (Elf32_Shdr *)((char *)hdr + hdr->e_shoff) + i;

		if (shdr->sh_type == type)
			return shdr;
	}

    return NULL;
}

struct {
    const char *str;
    int id;
} elf_symtypelist[] = {
    {"NOTYPE",        0},
    {"OBJECT",        1},
    {"FUNC",        2},
    {"SECTION",        3},
    {"FILE",        4},
    {"COMMON",        5},
    {"TLS",            6},
    {"NUM",            7},
    //{"LOOS",    10},
    {"GNU_IFUNC",    10},
    {"HIOS",        12},
    {"LOPROC",        13},
    {"HIPROC",        15},
};

const char *elf_symtype(int type)
{
    int i;
    for (i = 0; i < count_of_array(elf_symtypelist); i++) {
        if (elf_symtypelist[i].id == type)
            return elf_symtypelist[i].str;
    }

    return "Unknown";
}

struct {
    const char *str;
    int id;
} symbindlist[] = {
	{"LOCAL",    0},
    {"GLOBAL",    1},
	{"WEAK",		2},
    {"NUM",			3},
	{"LOOS",		10},
    {"GNU_UNIQUE",  10},      
	{"HIOS",		12},
    {"LOPROC",    13},
    {"HIPROC",    15},        
};

const char *elf_symbindtype(int bindtype)
{
	int i;

	for (i = 0; i < count_of_array(symbindlist); i++) {
		if (symbindlist[i].id == bindtype)
			return symbindlist[i].str;
	}

	return "Unknown";
}

const char *elf_symvis(int visibility)
{
	if (visibility == STV_DEFAULT)		return "DEFAULT";
	if (visibility == STV_HIDDEN)		return "HIDDEN";
	if (visibility == STV_INTERNAL)		return "INTERNAL";
	if (visibility == STV_PROTECTED)	return "PROTECTED";

	return "Unknown";
}

Elf32_Sym *elf32_sym_find(Elf32_Ehdr *hdr, unsigned long sym_val)
{
	Elf32_Shdr *dynsymsh;
	Elf32_Sym *sym;
	int i, num;

	dynsymsh = elf32_shdr_get(hdr, SHT_DYNSYM);
	if (!dynsymsh)
		return NULL;

	num = dynsymsh->sh_size / dynsymsh->sh_entsize;
	for (i = 0; i < num; i++) {
		sym = (Elf32_Sym *)((char *)hdr + dynsymsh->sh_offset) + i;
		if (sym->st_value == sym_val)
			return sym;
	}

	return NULL;
}

int         elf32_sym_count(Elf32_Ehdr *hdr)
{
	Elf32_Shdr *dynsymsh;

	dynsymsh = elf32_shdr_get(hdr, SHT_DYNSYM);
	if (!dynsymsh)
		return 0;

	return dynsymsh->sh_size / dynsymsh->sh_entsize;
}

Elf32_Sym *elf32_sym_geti(Elf32_Ehdr *hdr, int index)
{
	Elf32_Shdr *dynsymsh;

	dynsymsh = elf32_shdr_get(hdr, SHT_DYNSYM);
	if (!dynsymsh)
		return NULL;

    return (Elf32_Sym *)((char *)hdr + dynsymsh->sh_offset) + index;
}

void elf32_dump(char *elf, int opt)
{
    Elf_Indent *indent = (Elf_Indent *)elf;
    Elf32_Ehdr *hdr = (Elf32_Ehdr *)elf;
    Elf32_Phdr *phdr;
	Elf32_Shdr *shdr, *sh, *shstrdr, *dynsymsh, *link_sh;
	Elf32_Sym *sym;
    ElfW(Rel) *rel;
    int i, j, num, rel_count, type, symind;
	const char *name;

	if (opt == OPT_DUMP_ELF_HEADER) {
		printf("  Class:                                Elf32\n");
		printf("  Data:                                 2's complement, %s\n", 
			(indent->class == ELFDATA2LSB)?"little endian":((indent->class == ELFDATA2MSB)?"big endian":"unknown"));
		printf("  Version:                              %d (%s)\n", indent->version, elf_version2str(indent->version));
		printf("  OS/ABI:                               %s\n", elf_osabi2str(indent->osabi));
		printf("  ABI Version:                          %d\n", indent->abiversion);
		printf("  Type:                                 %s\n", elf_objtype2str(hdr->e_type));
		printf("  Machine:                              %s\n", elf_machine2str(hdr->e_machine));
		printf("  Version:                              %d\n", hdr->e_version);
		printf("  Entry point address:                  %d\n", hdr->e_entry);
		printf("  Start of program header:              %d (bytes into file)\n", hdr->e_phoff);
		printf("  Start of section header:              %d (bytes into file)\n", hdr->e_shoff);
		printf("  Flags:                                %08x\n", hdr->e_flags);
		printf("  Size of this header:                  %d (bytes) \n", hdr->e_ehsize);
		printf("  Size of program header:               %d (bytes) \n", hdr->e_phentsize);
		printf("  Number of program header:             %d\n", hdr->e_phnum);
		printf("  Size of section header:               %d\n", hdr->e_shentsize);
		printf("  Number of section header:             %d\n", hdr->e_shnum);
		printf("  Section header string table index:    %d\n", hdr->e_shstrndx);
	}

	if (opt == OPT_DUMP_ELF_PROG_HEADER) {
		printf("\n\n");
		printf("Program Headers:\n");
			printf("  Type            Offset     VirtAddr     PhysAddr   FileSiz MemSiz  Flg Align\n");
		for (i = 0; i < hdr->e_phnum; i++) {
			phdr = (Elf32_Phdr *)(elf + hdr->e_phoff + i * hdr->e_phentsize);

			printf("  %-16s0x%06x   0x%08x   %08x   0x%05x 0x%05x %c%c%c  %x\n", 
				elf_progtype2str(phdr->p_type), phdr->p_offset, phdr->p_vaddr, phdr->p_paddr, phdr->p_filesz, phdr->p_memsz, 
				(phdr->p_flags & PF_R) ? 'R':' ',
				(phdr->p_flags & PF_W) ? 'W':' ',
				(phdr->p_flags & PF_X) ? 'X':' ',
				phdr->p_align);
		}
	}

	if (opt == OPT_DUMP_ELF_SECTION) {
		shstrdr = (Elf32_Shdr *)(elf + hdr->e_shoff) + hdr->e_shstrndx;
		printf("\n\n");
		printf("Section Headers:\n");
		printf("  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al\n");
		printf("  [ 0]                   NULL            00000000 000000 000000 00      0   0  0\n");
		for (i = 1; i < hdr->e_shnum; i++) {
			shdr = (Elf32_Shdr *)(elf + hdr->e_shoff) + i;

			name = (char *)elf + (shstrdr->sh_offset + shdr->sh_name);
			printf("  [%2d] %-16.16s  %-14s  %08x %06x %06x %02x %-3s %2d %2d %2d\n", 
				i, name, elf_sectype2str(shdr->sh_type),
				shdr->sh_addr, shdr->sh_offset, shdr->sh_size, shdr->sh_entsize,
				elf_secflag2str(shdr->sh_flags), 
				shdr->sh_link, shdr->sh_info, shdr->sh_addralign);
		}
	}

    if (opt == OPT_DUMP_ELF_REL) {
		shstrdr = (Elf32_Shdr *)(elf + hdr->e_shoff) + hdr->e_shstrndx;
        dynsymsh = elf32_shdr_get(hdr, SHT_DYNSYM);
		link_sh = (Elf32_Shdr *)(elf + hdr->e_shoff) + dynsymsh->sh_link;

        for (i = 1; i < hdr->e_shnum; i++) {
			sh = (Elf32_Shdr *)(elf + hdr->e_shoff) + i;
            if (sh->sh_type != SHT_REL)
                continue;

            rel_count = sh->sh_size / sh->sh_entsize;
			name = (char *)elf + (shstrdr->sh_offset + shdr->sh_name);

            printf("\n\n");
            printf("Relocation section '%s' at offset %x conatins %d entries:\n", name, sh->sh_addr, rel_count);
            printf(" Offset     Info    Type            Sym.Value  Sym. Name\n");
            for (j = 0; j < rel_count; j++) {
                rel = ((ElfW(Rel) *)(elf + sh->sh_offset)) + j;
                type = ELFW(R_TYPE)(rel->r_info);
                symind = ELFW(R_SYM)(rel->r_info);
                sym = ((ElfW(Sym *))(elf + dynsymsh->sh_offset)) + symind;

                printf("%08x %08x %16x %08x %s\n", rel->r_offset, rel->r_info, type, sym->st_value, (char *)elf + link_sh->sh_offset + sym->st_name);
            }
        }
    }

	if ((opt == OPT_DUMP_ELF_DYNSYM) && (dynsymsh = elf32_shdr_get(hdr, SHT_DYNSYM))) {
		num = dynsymsh->sh_size / dynsymsh->sh_entsize;
		link_sh = (Elf32_Shdr *)(elf + hdr->e_shoff) + dynsymsh->sh_link;
		printf("\n\n");
		printf("Symbol table '.dynsym' contains %d entries\n", num);
		printf(" Num:    Value  Size Type    Bind   Vis      Ndx Name\n");
		for (i = 0; i < num; i++) {
			sym = (Elf32_Sym *)(elf + dynsymsh->sh_offset) + i;
			name = (char *)elf + (link_sh->sh_offset + sym->st_name);
			printf("  %02d: %08x %0-5d %-6s  %s %s  %d %s\n", i, sym->st_value, sym->st_size,
				elf_symtype(ELF32_ST_TYPE(sym->st_info)),
				elf_symbindtype(ELF32_ST_BIND(sym->st_info)),
				elf_symvis(ELF32_ST_VISIBILITY(sym->st_other)),
				sym->st_shndx,
				name);
		}
	}

}

void elf64_dump(char *elf, int opt)
{
    printf("Elf64 not support\n");
}

void elf_dump(char *elf, int elf_len, int opt)
{
    int i;
    Elf_Indent *indent = (Elf_Indent *)elf;

    if (memcmp(indent->magic, "\x7f""ELF", 4)) {
        printf("magic is wrong [%02x %02x %02x %02x]\n", indent->magic[0], indent->magic[1], indent->magic[2], indent->magic[3]);
        return;
    }

	if (opt == OPT_DUMP_ELF_HEADER) {
		printf("ELF Header:\n");
		printf("  Magic:    ");
		for (i = 0; i < 16; i++) {
			printf("%02x ", elf[i]);
		}
		printf("\n");
	}

    if (indent->class == ELFCLASS32) {
        elf32_dump(elf, opt);
    } else if (indent->class == ELFCLASS64) {
        elf64_dump(elf, opt);
    }
    else  {
        printf("not support class type[%d]", indent->class);
    }
}

char *elf_arm_rel_type(int type)
{
    return NULL;
}
