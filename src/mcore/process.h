
#ifndef __process_h__
#define __process_h__

#if defined(__cplusplus)
extern "C" {
#endif/* defined(__cplusplus) */

	/*
	@return		0		failure
				>0		process id
	*/
	int		process_find(char *name);
	char*	process_name(unsigned int pid, char *name, int size);
	void*	process_hwnd(unsigned int pid);
	int		process_inject(int pid, const wchar_t *dllname);
	void*   process_module(int pid, const wchar_t* lpModuleName);

	int		process_kill(void *handle);
	int		process_kill_by_name(const wchar_t *name);
	/* process_memory_search 内存搜索 
	搜索全部内存，包括代码段和地址段 
	@pattern	模式
	@plen		模式长度
	@idx		有多个匹配时，返回第 idx 个，计数从1开始，假如填0，自动返回第1个
	@return NULL	没有找到
			>0		pattern找到地址
	*/
	void*	process_memory_search(const char *pattern, int plen, int idx);
	DWORD process_code_search(DWORD startAddress, int searchSize, unsigned char* pCode, int codeLen, HANDLE processHandle);

	/*
	process_memory_search4 和process_memory_search功能一样，只是
	专门为4地址对齐得整数搜索做了优化
	*/
	void*	process_memory_search4(int pattern, int idx);

#if defined(__cplusplus)
}
#endif/* defined(__cplusplus) */

#endif
