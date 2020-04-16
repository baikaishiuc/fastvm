/*
@author		zhengxianwei 
@email		baikaishiuc@gmail.com
*/
#ifndef __aid_h__
#define __aid_h__

#if defined(__cplusplus)
extern "C" {
#endif

	/* Decode arm instruction to human readable string
	@code[in]		inst addr start
	@len[in]		inst remain length
	@obuf[out]		human readable string , end with null terminator
	@return		>0		decode length;
				<0		error
	*/
	int aid_decode(unsigned char *code, int len, char *obuf);


#if defined(__cplusplus)
}
#endif

#endif
