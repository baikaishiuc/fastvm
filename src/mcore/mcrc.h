
#if !defined(__mcrc_h__)
#define __mcrc_h__

#if defined(__cplusplus)
extern "C" {
#endif

/*!
func    mcrc_32
\brief  calculate crc-32bits checksum
\param	crc[in]             the prev part crc value
\param  data[in]            the data
\param  len[in]             the length of the data
\return crc value
*/
unsigned long mcrc_32(unsigned long crc, unsigned char* data, unsigned long len);

/*!
func    mcrc_8
\brief  calculate crc-8bits checksum
\param	crc[in]             the prev part crc value
\param  data[in]            the data
\param  len[in]             the length of the data
\return crc value
*/
unsigned char mcrc_8(unsigned char crc, unsigned char *data, unsigned long len);

#if defined(__cplusplus)
}
#endif

#endif /* !defined(__mcrc_h__) */

