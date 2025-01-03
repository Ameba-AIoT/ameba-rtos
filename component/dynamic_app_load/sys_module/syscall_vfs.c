/**
 ******************************************************************************
 * @file    syscall_vfs.c
 * @brief   Contains APIs for system calls from the APP
 ******************************************************************************
 * @attention
 *
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "syscall_vfs.h"
#include "vfs.h"

#ifdef KERNEL

void *syscall_fopen(const char *filename, const char *mode)
{
	return fopen(filename, mode);
}

int syscall_fclose(void *stream)
{
	return fclose((FILE *)stream);
}

int syscall_fread(void *ptr, int size, int count, void *stream)
{
	return fread(ptr, size, count, (FILE *)stream);
}

int syscall_fwrite(const void *ptr, int size, int count, void *stream)
{
	return fwrite(ptr, size, count, (FILE *)stream);
}

int syscall_fseek(void *stream, long int offset, int origin)
{
	return fseek((FILE *)stream, offset, origin);
}

int syscall_fflush(void *stream)
{
	return fflush((FILE *)stream);
}

int syscall_remove(const char *filename)
{
	return remove(filename);
}

int syscall_rename(const char *oldname, const char *newname)
{
	return rename(oldname, newname);
}

int syscall_feof(void *stream)
{
	return feof((FILE *)stream);
}

int syscall_ferror(void *stream)
{
	return ferror((FILE *)stream);
}

long int syscall_ftell(void *stream)
{
	return ftell((FILE *)stream);
}

long int syscall_ftruncate(void *stream, int length)
{
	return ftruncate((int)stream, (off_t)length);
}

int syscall_fputc(int character, void *stream)
{
	return fputc(character, (FILE *)stream);
}

int syscall_fputs(const char *str, void *stream)
{
	return fputs(str, (FILE *)stream);
}

char *syscall_fgets(char *str, int num, void *stream)
{
	return fgets(str, num, (FILE *)stream);
}

void *syscall_opendir(const char *name)
{
	return opendir(name);
}

void *syscall_readdir(void *pdir)
{
	return readdir((DIR *)pdir);
}

int syscall_closedir(void *dirp)
{
	return closedir((DIR *)dirp);
}

int syscall_rmdir(const char *path)
{
	return rmdir(path);
}

int syscall_mkdir(const char *pathname, int mode)
{
	return mkdir(pathname, (mode_t)mode);
}

int syscall_stat(const char *path, void *buf)
{
	return stat(path, (struct stat *)buf);
}

#endif