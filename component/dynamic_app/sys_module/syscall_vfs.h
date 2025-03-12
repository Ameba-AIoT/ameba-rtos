/**
 ******************************************************************************
 * @file    syscall_stdc.h
 * @brief   Contains APIs for system calls from the APP
 ******************************************************************************
*/

#ifndef _SYSCALL_VFS_H_
#define _SYSCALL_VFS_H_

void *syscall_fopen(const char *filename, const char *mode);
int syscall_fclose(void *stream);
int syscall_fread(void *ptr, int size, int count, void *stream);
int syscall_fwrite(const void *ptr, int size, int count, void *stream);
int syscall_fseek(void *stream, long int offset, int origin);
int syscall_fflush(void *stream);
int syscall_remove(const char *filename);
int syscall_rename(const char *oldname, const char *newname);
int syscall_feof(void *stream);
int syscall_ferror(void *stream);
long int syscall_ftell(void *stream);
long int syscall_ftruncate(void *stream, int length);
int syscall_fputc(int character, void *stream);
int syscall_fputs(const char *str, void *stream);
char *syscall_fgets(char *str, int num, void *stream);
void *syscall_opendir(const char *name);
void *syscall_readdir(void *pdir);
int syscall_closedir(void *dirp);
int syscall_rmdir(const char *path);
int syscall_mkdir(const char *pathname, int mode);
int syscall_stat(const char *path, void *buf);
#endif