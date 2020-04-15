
#include <stdio.h>
#include "elf.h"

#define count_of_array(_a)  (sizeof (_a) / sizeof (_a[0]))

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

unsigned char *elf_getsec(int type, unsigned char *data, int len)
{
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

