
#if defined(_MSC_VER)
#include <Windows.h>
#include <process.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <Shlwapi.h>
#include "merror.h"
#include "print_util.h"
#include "process.h"

char *process_name(unsigned int id, char *name, int size)
{
#undef  func_format_s
#undef  func_format
#define func_format_s   "process_name(id:%d) "
#define func_format()	id
	if (!id || !name || (size < sizeof ("<unknown>")))
		return NULL;

	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, id);

	strcpy(name, "<unkown>");

	if (NULL != hProcess) {
		HMODULE hMod;
		DWORD need;

		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &need)) {
			GetModuleBaseNameA(hProcess, hMod, name, size);
		}
		else {
			print_err("failed when (%s)EnumProcessModules().", merr_msg1());
		}
	}

	CloseHandle(hProcess);
	return name;
}

int		process_find(char *name)
{
#undef  func_format_s
#undef  func_format
#define func_format_s   "process_find(name:%s) "
#define func_format()	name
	DWORD aProcesses[1024], needed, cProcess, i;
	char buf[128];

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &needed) || !name) {
		return -1;
	}

	cProcess = needed / sizeof(DWORD);

	for (i = 0; i < cProcess; i++) {
		if (aProcesses[i] && process_name(aProcesses[i], buf, sizeof (buf)) && !strcmp (name, buf)) {
			return aProcesses[i];
		}

		print_info("process name [%s].", buf);
	}

	return 0;
}

HWND g_HWND = NULL;

BOOL CALLBACK EnumWindowProc(HWND hwnd, LPARAM lParam)
{
	DWORD pid;
	GetWindowThreadProcessId(hwnd, &pid);
	if (pid == lParam) {
		g_HWND = hwnd;
		return FALSE;
	}

	return TRUE;
}

HWND	process_hwnd(DWORD pid)
{
	if (!pid)	return NULL;

	if (EnumWindows(EnumWindowProc, (LPARAM)pid) == FALSE) {
		return g_HWND;
	}

	return NULL;
}

void* process_module(int pid, const wchar_t* lpModuleName) 
{
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, (DWORD)pid);
	if (handle == INVALID_HANDLE_VALUE) {
		return NULL;
	}
	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);
	BOOL success = Module32First(handle, &me32);
	while (success) {//遍历模块
		if (wcscmp(me32.szModule, lpModuleName) == 0) {
			CloseHandle(handle);
			return (void *)me32.hModule;
		}
		success = Module32NextW(handle, &me32);
	}
	CloseHandle(handle);
	return NULL;

}

int RemoteThreadInjectDll(HANDLE processHandle, const WCHAR* dllPath) {
#undef  func_format_s
#undef  func_format
#define func_format_s   "RemoteThreadInjectDll(hand:%p, path:%ls) "
#define func_format()	processHandle, dllPath
	DWORD dllPathSize = wcslen(dllPath) * sizeof(WCHAR);
	// 申请内存用来存放DLL路径
	void* remoteMemory = VirtualAllocEx(processHandle, NULL, dllPathSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (remoteMemory == NULL) {
		print_err("failed with (%s)VirtualAllocEx().", merr_msg1());
		return -1;
	}
	// 写入DLL路径
	if (!WriteProcessMemory(processHandle, remoteMemory, dllPath, dllPathSize, NULL)) {
		print_err("failed with (%s)WriteProcessMemory().", merr_msg1());
		return NULL;
	}
	// 创建远线程调用LoadLibrary
	HANDLE remoteThread = CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, remoteMemory, 0, NULL);
	if (remoteThread == NULL) {
		print_err("failed with (%s)CreateRemoteThread().", merr_msg1());
		return -1;
	}
	// 等待远线程结束
	WaitForSingleObject(remoteThread, INFINITE);
	// 取DLL在目标进程的句柄
	//DWORD remoteModule;
	//GetExitCodeThread(remoteThread, &remoteModule);
	// 释放
	CloseHandle(remoteThread);
	VirtualFreeEx(processHandle, remoteMemory, 0, MEM_RELEASE);
	return 0; //(HMODULE)remoteModule;
}

int		process_inject(int pid, const wchar_t *dllname)
{
#undef  func_format_s
#undef  func_format
#define func_format_s   "process_inject(pid:%d, dllname:%ls) "
#define func_format()	pid, dllname
	if (process_module (pid, dllname)==NULL) {
		HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		if (processHandle != NULL) {
			if (!PathFileExistsW(dllname)) {
				print_err("failed with path not exist.");
				return -1;
			}

			if (RemoteThreadInjectDll(processHandle, dllname)) {
				return ERR_SYSTEM;
			}
			CloseHandle(processHandle);
			return ERR_OK;
		}
	}
	else {
		print_err("failed with module already exist.");
		return ERR_MOD_EXIST;
	}

	return ERR_SYSTEM;
}

int		process_kill(void *handle)
{
	return TerminateProcess(handle, 0);
}

int		process_kill_by_name(const wchar_t *filename)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes)
	{
		if (wcscmp(pEntry.szExeFile, filename) == 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
				(DWORD)pEntry.th32ProcessID);
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
			}
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);

	return 0;
}

void*	process_memory_search(const char *pattern, int plen, int idx)
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	int j;
	SIZE_T i;

	if (!idx) idx = 1;

	__int64 end = (__int64)sysInfo.lpMaximumApplicationAddress;

	char *currentChunk = 0;
	char *match = NULL;
	SIZE_T bytesRead;
	MEMORY_BASIC_INFORMATION mbi;

	while (currentChunk < (char *)end)
	{
		HANDLE process = GetCurrentProcess();

		if (!VirtualQueryEx(process, currentChunk, &mbi, sizeof(mbi))) {
			return 0;
		}

		char *buffer = 0;

		if (mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS) {
			buffer = (char *)malloc(mbi.RegionSize);
			if (ReadProcessMemory(process, mbi.BaseAddress, buffer, mbi.RegionSize, &bytesRead))
			{
				// 搜索
				for (i = 0; i < (mbi.RegionSize - plen); i++) {
					for (j = 0; j < plen; j++) {
						if (pattern[j] != buffer[i + j]) {
							break;
						}
					}

					if (j == plen) {
						if (idx == 1) {
							match = currentChunk + i;
							break;
						}

						idx--;
					}
				}

			}
			free(buffer);
		}

		currentChunk += mbi.RegionSize;

		if (match)
			break;
	}

	return match ? match : NULL;
}

void*	process_memory_search4(int pattern, int idx)
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	SIZE_T i, len;

	if (!idx) idx = 1;

	__int64 end = (__int64)sysInfo.lpMaximumApplicationAddress;

	char *currentChunk = 0;
	char *match = NULL;
	SIZE_T bytesRead;
	MEMORY_BASIC_INFORMATION mbi;

	while (currentChunk < (char *)end)
	{
		HANDLE process = GetCurrentProcess();

		if (!VirtualQueryEx(process, currentChunk, &mbi, sizeof(mbi))) {
			return 0;
		}

		char *buffer = 0;
		int *ibuf = 0;

		if ((mbi.State == MEM_COMMIT)) {
			buffer = (char *)malloc(mbi.RegionSize);
			ibuf = (int *)buffer;
			if (ReadProcessMemory(process, mbi.BaseAddress, buffer, mbi.RegionSize, &bytesRead))
			{
				len = mbi.RegionSize / 4;
				for (i = 0; i < len; i++) {
					if (pattern == ibuf[i]) {
						if (idx == 1) {
							match = currentChunk + i * 4;
							break;
						}

						idx--;
					}
				}
			}
			free(buffer);
		}

		currentChunk += mbi.RegionSize;
		if (match)
			break;
	}

	return match ? match : NULL;
}

DWORD process_code_search(DWORD startAddress, int searchSize, unsigned char* pCode, int codeLen, HANDLE processHandle)
{
	if (searchSize <= 0 || codeLen <= 0 || searchSize - codeLen < 0) {
		return 0;
	}

	if (processHandle == NULL || processHandle == GetCurrentProcess()) {
		for (int i = 0; i < searchSize - codeLen; i++) {
			if (memcmp((void*)(startAddress + i), pCode, codeLen) == 0) {
				return startAddress + i;
			}
		}
	}
	else {
		BYTE* bytes = (BYTE *)malloc(codeLen);
		if (ReadProcessMemory(processHandle, (LPVOID)startAddress, bytes, codeLen, NULL)) {

			for (int i = 0; i < searchSize - codeLen; i++) {
				for (int j = 0; j < codeLen; j++) {
					if (pCode[j] != bytes[(i + j) % codeLen]) {
						break;
					}
					if (j + 1 == codeLen) {
						free(bytes);
						return startAddress + i;
					}
				}
				ReadProcessMemory(processHandle, (LPVOID)(startAddress + i + codeLen), bytes + i % codeLen, 1, NULL);
			}
		}
		free(bytes);
	}
	return 0;
}


#endif

