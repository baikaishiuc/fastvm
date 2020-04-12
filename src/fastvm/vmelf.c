
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

    free (elf);
}

void elf32_dump(struct VMElf *elf)
{
    Elf_Indent *indent = (Elf_Indent *)elf->data;
    Elf32_Ehdr *hdr = (Elf32_Ehdr *)elf->data;

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

void elf64_dump(struct VMElf *elf)
{
    printf("Elf64 not support\n");
}

void vmelf_dump(struct VMElf *elf)
{
    int i;
    Elf_Indent *indent = (Elf_Indent *)elf->data;

    if (memcmp(indent->magic, "\x7f""elf", 4)) {
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