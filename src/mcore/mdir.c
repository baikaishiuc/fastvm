
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mdir.h"

#if defined(_MSC_VER)
#include <windows.h>
struct mdir {
    HANDLE hFind;
	WIN32_FIND_DATAA fd;
	int time;
};

struct mdir*    mdir_open(const char *dir)
{
	struct mdir *m = NULL;
	char buf[MAX_PATH];

	m = (struct mdir *)calloc(1, sizeof (m[0]));
	if (!m)
		return NULL;

	sprintf(buf, "%s/*.*", dir);

	m->hFind = FindFirstFileA(buf, &m->fd);
	if (INVALID_HANDLE_VALUE == m->hFind) {
		free(m);
		return NULL;
	}

    return m;
}

void            mdir_close(struct mdir *m)
{
	if (m) {
		FindClose(m->hFind);
		free(m);
	}
}

char*     mdir_next(struct mdir *d)
{
	if (!d->time) {
		d->time++;
		return d->fd.cFileName;
	}
	else if (FindNextFileA(d->hFind, &d->fd)) {
		return d->fd.cFileName;
	}
	else {
		return NULL;
	}
}

#else
#include <dirent.h>
struct mdir {
    DIR *d;
};

struct mdir*    mdir_open(const char *dir)
{
    struct mdir *mdir1 = calloc (1, sizeof (mdir1[0]));
    if (!mdir1)
        return NULL;

    mdir1->d = opendir(dir);
    if (!mdir1->d)
        return NULL;

    return mdir1;
}

void            mdir_close(struct mdir *d)
{
    if (d) {
        closedir(d->d);

        free (d);
    }
}

char*     mdir_next(struct mdir *d)
{
    if (!d || !d->d)
        return NULL;

    struct dirent *dir = readdir(d->d);
    if (!dir)
        return NULL;

    return dir->d_name;
}
#endif


