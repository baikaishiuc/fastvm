
#include "mcore/mcore.h"
#include "vm.h"

struct VMElf *vmelf_new()
{
    return NULL;
}

struct VMElf *vmelf_load(const char *filename)
{
    struct VMElf *ve;

    ve = (struct VMElf *)calloc(1, sizeof (ve[0]));
    if (!ve) {
        vm_error("calloc failure.");
    }

    ve->data = file_load(filename, &ve->data_len);
    if (!ve->data) {
        vm_error("load file(%s) failure.", filename);
    }

    ve->filename = strdup(filename);

    return ve;
}

void vmelf_unload(struct VMElf *elf)
{
    if (!elf)
        return;

    if (elf->data) {
        free(elf->data);
        elf->data = NULL;
    }

    if (elf->filename) {
        free(elf->filename);
        elf->filename = NULL;
    }

    free (elf);
}

void elf32_dump(struct VMElf *elf)
{
    Elf_Indent *indent = (Elf_Indent *)elf->data;
    Elf32_Ehdr *hdr = (Elf32_Ehdr *)elf->data;
    Elf32_Phdr *phdr;
	Elf32_Shdr *shdr, *shstrdr, *dynsymsh, *link_scn;
	Elf32_Sym *sym;
    int i, num;
	const char *name;

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

    printf("\n\n");
    printf("Program Headers:\n");
        printf("  Type            Offset     VirtAddr     PhysAddr   FileSiz MemSiz  Flg Align\n");
    for (i = 0; i < hdr->e_phnum; i++) {
		phdr = (Elf32_Phdr *)(elf->data + hdr->e_phoff + i * hdr->e_phentsize);

        printf("  %-16s0x%06x   0x%08x   %08x   0x%05x 0x%05x %c%c%c  %x\n", 
            elf_progtype2str(phdr->p_type), phdr->p_offset, phdr->p_vaddr, phdr->p_paddr, phdr->p_filesz, phdr->p_memsz, 
			(phdr->p_flags & PF_R) ? 'R':' ',
			(phdr->p_flags & PF_W) ? 'W':' ',
			(phdr->p_flags & PF_X) ? 'X':' ',
			phdr->p_align);
    }

	shstrdr = (Elf32_Shdr *)(elf->data + hdr->e_shoff) + hdr->e_shstrndx;
	printf("\n\n");
	printf("Section Headers:\n");
	printf("  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al\n");
	printf("  [ 0]                   NULL            00000000 000000 000000 00      0   0  0\n");
	for (i = 1; i < hdr->e_shnum; i++) {
		shdr = (Elf32_Shdr *)(elf->data + hdr->e_shoff) + i;

		name = (char *)elf->data + (shstrdr->sh_offset + shdr->sh_name);
		printf("  [%2d] %-16.16s  %-14s  %08x %06x %06x %02x %-3s %2d %2d %2d\n", 
			i, name, elf_sectype2str(shdr->sh_type),
			shdr->sh_addr, shdr->sh_offset, shdr->sh_size, shdr->sh_entsize,
			elf_secflag2str(shdr->sh_flags), 
			shdr->sh_link, shdr->sh_info, shdr->sh_addralign);

		if (shdr->sh_type == SHT_DYNSYM)	dynsymsh = shdr;
	}

	if (dynsymsh) {
		num = dynsymsh->sh_size / dynsymsh->sh_entsize;
		link_scn = (Elf32_Shdr *)(elf->data + hdr->e_shoff) + dynsymsh->sh_link;
		printf("\n\n");
		printf("Symbol table '.dynsym' contains %d entries\n", num);
		printf(" Num:    Value  Size Type    Bind   Vis      Ndx Name\n");
		for (i = 0; i < num; i++) {
			sym = (Elf32_Sym *)(elf->data + dynsymsh->sh_offset) + i;
			name = (char *)elf->data + (link_scn->sh_offset + sym->st_name);
			printf("  %02d: %08x %0-5d %-6s  %s %s\n", i, sym->st_value, sym->st_size, 
				elf_symtype(ELF32_ST_TYPE(sym->st_info)), 
				elf_symbindtype(ELF32_ST_BIND(sym->st_info)),
				name);
		}
	}
}

void elf64_dump(struct VMElf *elf)
{
    printf("Elf64 not support\n");
}

void vmelf_dump(struct VMElf *elf)
{
    int i;
    Elf_Indent *indent = (Elf_Indent *)elf->data;

    if (memcmp(indent->magic, "\x7f""ELF", 4)) {
        printf("%s magic is wrong [%02x %02x %02x %02x]\n", elf->filename, 
            indent->magic[0], indent->magic[1], indent->magic[2], indent->magic[3]);
        return;
    }

    printf("ELF Header:\n");
    printf("  Magic:    ");
    for (i = 0; i < 16; i++) {
        printf("%02x ", elf->data[i]);
    }
    printf("\n");

    if (indent->class == ELFCLASS32) {
        elf32_dump(elf);
    } else if (indent->class == ELFCLASS64) {
        elf64_dump(elf);
    }
    else  {
        printf("not support class type[%d]", indent->class);
    }
}