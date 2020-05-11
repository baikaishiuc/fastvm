#if defined(__cplusplus)
extern "C" {
#endif

#include "mcrc.h"

typedef unsigned int uint32_t;

/* !!!!! copy to manti_interference_coding.c !!!!! */

/*!
func    mcrc_32
\brief  calculate crc-32bits checksum
\param	crc[in]             the prev part crc value
\param  data[in]            the data
\param  len[in]             the length of the data
\return crc value
*/
unsigned long mcrc_32(unsigned long crc, unsigned char* data, unsigned long len)
{
    static uint32_t     crc_table[256];
    uint32_t            crc_local = (uint32_t)crc;
    unsigned char *data_end = data + len;
    if(0 == crc_table[(sizeof(crc_table) / sizeof(crc_table[0])) - 1])
    {/* need  prepare crc table */
        uint32_t i, k, v, p = 0/* p magic == 0xedb88320 */;
        unsigned char p_list[] = {0,1,2,4,5,7,8,10,11,12,16,22,23,26};
        for(i = 0; i < sizeof(p_list)/sizeof(p_list[0]); i++){ p |= 1 << (31 - p_list[i]);};
        for(i = 0; i < 256; ++i)
        {
            for (v = i, k = 0; k < 8; k++){ v = ((v & 1) ? p ^ (v >> 1) : v >> 1); };
            crc_table[i] = v;
        }
    }
    /* get crc32 value */
    crc_local ^= 0xffffffffL;
    if(data)
    {
        while(data_end > data)
        {
            crc_local = crc_table[(crc_local ^ (*(data++))) & 0xff] ^ (crc_local >> 8);
        }
    }
    return crc_local ^ 0xffffffffL;
}

/*!
func    mcrc_8
\brief  calculate crc-8bits checksum
\param	crc[in]             the prev part crc value
\param  data[in]            the data
\param  len[in]             the length of the data
\return crc value
*/
unsigned char mcrc_8(unsigned char crc, unsigned char *data, unsigned long len)
{
    static unsigned char    crc_table[256];
    unsigned char           crc_local;
    if(0 == crc_table[(sizeof(crc_table) / sizeof(crc_table[0])) - 1])
    {/* need  prepare crc table */
        unsigned long i, j;
        unsigned char value;
        for(i = 0; i < (sizeof(crc_table)/sizeof(crc_table[0])); ++i)
        {
            value = i & 0xff;
            for(j = 0, crc_local = 0; j < 8; j++, value >>= 1)    
            {    
                crc_local = (((crc_local ^ value) & 0x01))?(((crc_local ^ 0x18) >> 1) | 0x80):(crc_local >> 1);
            } 
            crc_table[i] = crc_local;
        }
    }
    
    crc_local = crc;
    while(len--)
    { 
        crc_local = crc_table[crc_local ^ (*(data++))]; 
    }
    return crc_local;
}

#if defined(__cplusplus)
}
#endif
