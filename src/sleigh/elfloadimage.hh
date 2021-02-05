
#include "loadimage.hh"

class ElfLoadImage : public LoadImageB {
    long long baseaddr;
    unsigned char*  filedata;
    int				filelen;

    int is64bit;
    FILE *fp;
    int cur_sym;
    AddrSpace *codespace;
    //struct bitset *isdata;

public:
    ElfLoadImage(const char *filename);
    virtual ~ElfLoadImage();

    void setCodeSpace(AddrSpace *a) { codespace = a; }
    /* 再扫描elf的符号表时，会出现数据紧跟在代码区域后面，这部分的数据不应该在解析了，
    我们这里假设这个跟随的数据区是4字节为一个单位的，所以我们用一个 filelen/32 大小的bit数组来
    标识是否是数据还是代码 */
    int markData(int offset);
    bool isData(const Address &a);
    virtual void loadFill(uint1 *ptr,int4 size,const Address &addr);
    virtual string getArchType(void) const { return is64bit?"Elf64":"Elf32"; }
    virtual bool getNextSymbol(LoadImageFunc &record); 
    virtual void adjustVma(long adjust) { }
    int getSymbol(const char *symname, LoadImageFunc &record);
};
