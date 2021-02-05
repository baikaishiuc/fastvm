#include "vm.h"
#include "elfloadimage.hh"
 
ElfLoadImage::ElfLoadImage(const char *filename):LoadImageB(filename)
{
    filedata = (unsigned char *)file_load(filename, (int *)&filelen);
    if (!filedata) {
        printf("ElfLoadImage() failed open [%s]", filename);
        exit(0);
    }

#if 1
    // FIXME:我们静态载入elf的时候，没有填got表，这里直接手动填一个了
    // 我们对所有访问got区段的值，都放回
    filedata[0xfe8c] = 0x28;
    filedata[0xfe8c + 1] = 0x60;
    filedata[0xfe8c + 2] = 0x09;
    filedata[0xfe8c + 3] = 0x00;
#endif

    //isdata = bitset_new(filelen);
    cur_sym = -1;
}

ElfLoadImage::~ElfLoadImage()
{
    file_unload((char *)filedata);
}

void ElfLoadImage::loadFill(uint1 *ptr, int size, const Address &addr) 
{
    unsigned start = (unsigned)addr.getOffset();
    if ((start + size) > filelen) {
        /* FIXME: 我们对所有访问的超过空间的地址都返回 0xaabbccdd，这里不是BUG，是因为我们载入so的时候，是直接平铺着载入的
        但是实际在程序加载so的时候，会填充很多结构，并做一些扩展 */
        ptr[0] = 0x11;
        ptr[1] = 0x22;
        ptr[2] = 0x33;
        ptr[3] = 0x44;
        return;
    }

    /* FIXME: .got表我没有生成，这里直接根据IDA的结构，手动写了*/
    if ((start == 0xfe8c) && (size == 4)) {
        ptr[0] = 0x98;
        ptr[1] = 0x57;
        ptr[2] = 0x08;
        ptr[3] = 0x00;
        return;
    }

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

    //bitset_set(isdata, offset / 4, 1);

    return 0;
}

bool ElfLoadImage::isData(const Address &a)
{
    //return bitset_get(isdata, (int)a.getOffset() / 4);
    return false;
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
