
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#include <Windows.h>
#include <Shlwapi.h>
#endif
#include <sys/stat.h>


#include "print_util.h"
#include "file.h"

int file_exist(char *filename)
{
#if defined(_MSC_VER)
	return PathFileExistsA(filename);
#else
    struct stat st;
    return stat(filename, &st) == 0;
#endif
}

int file_save(char *filename, char *buf, int len)
{
#undef  func_format_s
#undef  func_format
#define func_format_s   "file_save(filename:%s) "
#define func_format()	filename
	FILE *fp = fopen(filename, "wb");
	if (!fp) {
		print_err(" failed with fopen()");
		return -1;
	}

	fwrite(buf, len, 1, fp);
	fclose(fp);

	return 0;
}

#if defined(_MSC_VER)
int file_size(const char *filename)
{
	HANDLE hFile = CreateFileA(filename, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return 0; // error condition, could call GetLastError to find out more

	LARGE_INTEGER size;
	if (!GetFileSizeEx(hFile, &size))
	{
		CloseHandle(hFile);
		return 0; // error condition, could call GetLastError to find out more
	}

	CloseHandle(hFile);
	return (int)size.QuadPart;

}
#else
int file_size(const char *filename)
{
	int ret;
	struct stat st;
	ret = _stat(filename, &st);
	return 0;
}
#endif

char* file_load(const char *filename, int *len)
{
	char *data;
	FILE *fp;
	*len = 0;

	int size = file_size(filename);
	if (!size) {
		return NULL;
	}

	*len = size;
	data = (char *)malloc(size);
	if (!data)
		return NULL;

	fp = fopen(filename, "rb");
	if (!fp) {
		free(data);
		return NULL;
	}

	fread(data, size, 1, fp);
	fclose(fp);

	return data;
}


int file_unload(char *data)
{
	if (data) {
		free(data);
	}

	return 0;
}
