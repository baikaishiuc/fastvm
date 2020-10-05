
#include "vm.h"
#include "elfloadimage.hh"

ElfLoadImage::ElfLoadImage(const char *filename):LoadImage(filename)
{
    filedata = (unsigned char *)file_load(filename, &filelen);
    if (!filedata) {
        printf("ElfLoadImage() failed open [%s]", filename);
        exit(0);
    }

    isdata = bitset_new(filelen);
    cur_sym = -1;
}

ElfLoadImage::~ElfLoadImage()
{
    file_unload((char *)filedata);
}

void ElfLoadImage::loadFill(uint1 *ptr, int size, const Address &addr) 
{
    int start = (int)addr.getOffset();
    if ((start + size) > filelen)
        size = filelen - start;

    memcpy(ptr, filedata + start, size);
}

bool ElfLoadImage::getNextSymbol(LoadImageFunc &record) 
{
    Elf32_Shdr *dynsymsh, *link_sh;
    Elf32_Sym *sym;
    Elf32_Ehdr *hdr = (Elf32_Ehdr *)filedata;
    int num;
    const char *name;

    cur_sym++;

    dynsymsh = elf32_shdr_get((Elf32_Ehdr *)filedata, SHT_DYNSYM);
    if (!dynsymsh) 
        vm_error("file[%s] have not .dymsym section", filename.c_str());

    link_sh = (Elf32_Shdr *)(filedata + hdr->e_shoff) + dynsymsh->sh_link;

    num = dynsymsh->sh_size / dynsymsh->sh_entsize;
    if (cur_sym >= num) {
        cur_sym = -1;
        return false;
    }

    sym = (Elf32_Sym *)(filedata + dynsymsh->sh_offset) + cur_sym;
    name = (char *)filedata + (link_sh->sh_offset + sym->st_name);

    record.address = Address(codespace, sym->st_value);
    record.name = string(name);
    record.size = sym->st_size;

    return true;
}

int ElfLoadImage::markData(int offset)
{
    if (offset & 3)
        vm_error("Address[0x%x] not align 4 byte", offset);

    bitset_set(isdata, offset / 4, 1);

    return 0;
}

bool ElfLoadImage::isData(const Address &a)
{
    return bitset_get(isdata, (int)a.getOffset() / 4);
}

int ElfLoadImage::getSymbol(const char *symname, LoadImageFunc &record)
{
    Elf32_Shdr *dynsymsh, *link_sh;
    Elf32_Sym *sym;
    Elf32_Ehdr *hdr = (Elf32_Ehdr *)filedata;
    int i, num;
    const char *name;

    dynsymsh = elf32_shdr_get((Elf32_Ehdr *)filedata, SHT_DYNSYM);
    if (!dynsymsh) 
        vm_error("file[%s] have not .dymsym section", filename.c_str());

    link_sh = (Elf32_Shdr *)(filedata + hdr->e_shoff) + dynsymsh->sh_link;

    num = dynsymsh->sh_size / dynsymsh->sh_entsize;

    for (i = 0; i < num; i++) {
        sym = (Elf32_Sym *)(filedata + dynsymsh->sh_offset) + i;
        name = (char *)filedata + (link_sh->sh_offset + sym->st_name);
        if (!strcmp(name, symname)) {
            record.address = Address(codespace, sym->st_value);
            record.name = string(name);
            record.size = sym->st_size;
            return 0;
        }
    }

    return -1;
}