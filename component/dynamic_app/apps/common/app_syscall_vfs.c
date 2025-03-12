/**
 ******************************************************************************
 * @file    app_syscall_vfs.c
 * @brief   Contains APIs for system calls from the APP
 ******************************************************************************
 * @attention
 *
*/
// #include "basic_types.h"
#include "app_syscall.h"
#include "vfs.h"

FILE *__wrap_fopen(const char *filename, const char *mode)
{
	return (FILE *)sys->fopen(filename, mode);
}

int __wrap_fclose(FILE *stream)
{
	return sys->fclose(stream);
}

int __wrap_fread(void *ptr, int size, int count, FILE *stream)
{
	return sys->fread(ptr, size, count, stream);
}

int __wrap_fwrite(const void *ptr, int size, int count, FILE *stream)
{
	return sys->fwrite(ptr, size, count, stream);
}

int __wrap_fseek(FILE *stream, long int offset, int origin)
{
	return sys->fseek(stream, offset, origin);
}

int __wrap_fflush(FILE *stream)
{
	return sys->fflush(stream);
}

int __wrap_remove(const char *filename)
{
	return sys->remove(filename);
}

int __wrap_rename(const char *oldname, const char *newname)
{
	return sys->rename(oldname, newname);
}

int __wrap_feof(FILE *stream)
{
	return sys->feof(stream);
}

int __wrap_ferror(FILE *stream)
{
	return sys->ferror(stream);
}

long int __wrap_ftell(FILE *stream)
{
	return sys->ftell(stream);
}

long int __wrap_ftruncate(FILE *stream, int length)
{
	return sys->ftruncate(stream, (off_t)length);
}

int __wrap_fputc(int character, FILE *stream)
{
	return sys->fputc(character, stream);
}

int __wrap_fputs(const char *str, FILE *stream)
{
	return sys->fputs(str, stream);
}

char *__wrap_fgets(char *str, int num, FILE *stream)
{
	return sys->fgets(str, num, stream);
}

DIR *__wrap_opendir(const char *name)
{
	return (DIR *)sys->opendir(name);
}

struct dirent *__wrap_readdir(DIR *pdir)
{
	return (struct dirent *)sys->readdir(pdir);
}

int __wrap_closedir(DIR *dirp)
{
	return sys->closedir(dirp);
}

int __wrap_rmdir(const char *path)
{
	return sys->rmdir(path);
}

int __wrap_mkdir(const char *pathname, int mode)
{
	return sys->mkdir(pathname, mode);
}

int __wrap_stat(const char *path, struct stat *buf)
{
	return sys->stat(path, buf);
}