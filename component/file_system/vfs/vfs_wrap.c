#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "time.h"
#include "diag.h"
#include "vfs.h"
#include "os_wrapper.h"

/**************************************************
* FILE api wrap for compiler
*
**************************************************/
/*
--redirect fopen=__wrap_fopen
--redirect fclose=__wrap_fclose
--redirect fread=__wrap_fread
--redirect fwrite=__wrap_fwrite
--redirect fseek=__wrap_fseek
--redirect fsetpos=__wrap_fsetpos
--redirect fgetpos=__wrap_fgetpos
--redirect rewind=__wrap_rewind
--redirect fflush=__wrap_fflush
--redirect remove=__wrap_remove
--redirect rename=__wrap_rename
--redirect feof=__wrap_feof
--redirect ferror=__wrap_ferror
--redirect ftell=__wrap_ftell
--redirect fputc=__wrap_fputc
--redirect fputs=__wrap_fputs
--redirect fgets=__wrap_fgets
*/

static int is_stdio(FILE *stream)
{
#ifndef __ICCARM__
	if (stream == stdout || stream == stderr || stream == stdin) {
		return 1;
	}
#endif
	return 0;
}

FILE *__wrap_fopen(const char *filename, const char *mode)
{
	int prefix_len = 0;
	int drv_id = 0;
	int ret = 0;
	int user_id = 0;
	int vfs_id = find_vfs_number(filename, &prefix_len, &user_id);
	if (vfs_id < 0) {
		VFS_DBG(VFS_ERROR, "It can't find the file system");
		return NULL;
	}

	ret = vfs_check_mount_flag(vfs.drv[vfs_id]->vfs_type, vfs.user[user_id].vfs_interface_type, "fopen");
	if (ret) {
		return NULL;
	}

	if (vfs.user[user_id].vfs_ro_flag && (strchr(mode, 'w') || strchr(mode, '+') || strchr(mode, 'a'))) {
		VFS_DBG(VFS_WARNING, "Vfs is read-only when initializing!");
		return NULL;
	}

	vfs_file *finfo = (vfs_file *)malloc(sizeof(vfs_file));
	if (finfo == NULL) {
		return NULL;
	}
	memset(finfo, 0x00, sizeof(vfs_file));
	finfo->vfs_id = vfs_id;
	finfo->user_id = user_id;

	if (vfs.drv[vfs_id]->vfs_type == VFS_FATFS) {
		drv_id = vfs.drv[vfs_id]->get_interface(vfs.user[user_id].vfs_interface_type);
		char temp[4] = {0};
		temp[0] = drv_id + '0';
		temp[1] = ':';
		temp[2] = '/';
		DiagSnPrintf(finfo->name, sizeof(finfo->name), "%s%s", temp, filename + prefix_len);
	} else {
		DiagSnPrintf(finfo->name, sizeof(finfo->name), "%s", filename + prefix_len);
	}

	ret = vfs.drv[vfs_id]->open(vfs.user[user_id].fs, finfo->name, mode, finfo);
	if (ret < 0) {
		free(finfo);
		finfo = NULL;
	}
	return (FILE *)finfo;
}

int __wrap_fclose(FILE *stream)
{
	int ret = 0;

	vfs_file *finfo = (vfs_file *)stream;
	if (is_stdio(stream)) {
		return 0;
	}

	ret = vfs.drv[finfo->vfs_id]->close(vfs.user[finfo->user_id].fs, (vfs_file *)stream);
	free(finfo);
	return ret;
}

size_t __wrap_fread(void *ptr, size_t size, size_t count, FILE *stream)
{
	int ret = 0;
	vfs_file *finfo = (vfs_file *)stream;


	if (is_stdio(stream)) {
		return 0;
	}

	if (vfs.user[finfo->user_id].vfs_dec_callback != NULL) {
		unsigned char *aesencsw;
		unsigned char iv_len = vfs.user[finfo->user_id].enc_iv_len;
		unsigned short msglen = (size * count / iv_len + 1) * iv_len;

		if (msglen > 16383) {
			VFS_DBG(VFS_ERROR, "Encrypt content length should be smaller than 16383 !!!");
			return -1;
		}

		aesencsw = rtos_mem_calloc(msglen, sizeof(unsigned char));
		ret = vfs.drv[finfo->vfs_id]->read(vfs.user[finfo->user_id].fs, aesencsw, msglen, 1, (vfs_file *)stream);
		vfs.user[finfo->user_id].vfs_dec_callback(aesencsw, ptr, size * count);
		rtos_mem_free(aesencsw);

	} else {
		ret = vfs.drv[finfo->vfs_id]->read(vfs.user[finfo->user_id].fs, ptr, size, count, (vfs_file *)stream);
	}

	return ret;
}

extern size_t _write(int file, const void *ptr, size_t len);
size_t __wrap_fwrite(const void *ptr, size_t size, size_t count, FILE *stream)
{
	int ret = 0;
	vfs_file *finfo = (vfs_file *)stream;

#ifndef  __ICCARM__
	if (stream == stdout) {
		return _write(1, ptr, size * count);
	}
	if (stream == stderr) {
		return _write(2, ptr, size * count);
	}
	if (stream == stdin) {
		return 0;
	}
#endif

	if (vfs.user[finfo->user_id].vfs_enc_callback != NULL) {
		unsigned char *aesencsw;
		unsigned char iv_len = vfs.user[finfo->user_id].enc_iv_len;
		unsigned short msglen = (size * count / iv_len + 1) * iv_len;

		if (msglen > 16383) {
			VFS_DBG(VFS_ERROR, "Encrypt Content length should be smaller than 16383 !!!");
			return -1;
		}

		aesencsw = rtos_mem_calloc(msglen, sizeof(unsigned char));
		vfs.user[finfo->user_id].vfs_enc_callback((void *)ptr, aesencsw, size * count);
		ret = vfs.drv[finfo->vfs_id]->write(vfs.user[finfo->user_id].fs, (void *)aesencsw, msglen, 1, (vfs_file *)stream);
		rtos_mem_free(aesencsw);

	} else {
		ret = vfs.drv[finfo->vfs_id]->write(vfs.user[finfo->user_id].fs, (void *)ptr, size, count, (vfs_file *)stream);
	}

	return ret;
}

int  __wrap_fseek(FILE *stream, long int offset, int origin)
{
	int ret = 0;
	vfs_file *finfo = (vfs_file *)stream;
	if (is_stdio(stream)) {
		return 0;
	}

	ret = vfs.drv[finfo->vfs_id]->seek(vfs.user[finfo->user_id].fs, offset, origin, (vfs_file *)stream);
	return ret;
}

void  __wrap_rewind(FILE *stream)
{
	vfs_file *finfo = (vfs_file *)stream;
	if (is_stdio(stream)) {
		return;
	}
	vfs.drv[finfo->vfs_id]->rewind(vfs.user[finfo->user_id].fs, (vfs_file *)stream);
}

int __wrap_fgetpos(FILE *stream, fpos_t   *p)
{
	vfs_file *finfo = (vfs_file *)stream;
	if (is_stdio(stream)) {
		return 0;
	}
#if defined(__ICCARM__)
	p->_Off = vfs.drv[finfo->vfs_id]->fgetpos(vfs.user[finfo->user_id].fs, (vfs_file *)stream);
#elif defined(__GNUC__)
	*p = vfs.drv[finfo->vfs_id]->fgetpos(vfs.user[finfo->user_id].fs, (vfs_file *)stream);
#endif
	return 0;
}

int __wrap_fsetpos(FILE *stream, fpos_t   *p)
{
	int ret = 0;
	vfs_file *finfo = (vfs_file *)stream;
	if (is_stdio(stream)) {
		return 0;
	}
#if defined(__ICCARM__)
	ret = vfs.drv[finfo->vfs_id]->fsetpos(vfs.user[finfo->user_id].fs, p->_Off, (vfs_file *)stream);
#elif defined(__GNUC__)
	ret = vfs.drv[finfo->vfs_id]->fsetpos(vfs.user[finfo->user_id].fs, (unsigned int) * p, (vfs_file *)stream);
#endif
	return ret;
}

//extern int _fflush_r(struct _reent *ptr, FILE *fp);
int  __wrap_fflush(FILE *stream)
{
	int ret = 0;
	vfs_file *finfo = (vfs_file *)stream;
#ifndef __ICCARM__
	if (is_stdio(stream)) {
		_fflush_r(_REENT, stream);
		return 0;
	}
#endif
	ret = vfs.drv[finfo->vfs_id]->fflush(vfs.user[finfo->user_id].fs, (vfs_file *)stream);
	return ret;
}

int __wrap_remove(const char *filename)
{
	int ret = 0;
	int prefix_len = 0;
	int user_id = 0;
	int vfs_id = find_vfs_number(filename, &prefix_len, &user_id);
	if (vfs_id < 0) {
		VFS_DBG(VFS_ERROR, "It can't find the file system");
		return -1;
	}

	ret = vfs_check_mount_flag(vfs.drv[vfs_id]->vfs_type, vfs.user[user_id].vfs_interface_type, "remove");
	if (ret) {
		return -1;
	}

	if (vfs.user[user_id].vfs_ro_flag) {
		VFS_DBG(VFS_WARNING, "Vfs is read-only when initializing!");
		return -1;
	}

	char *name = (char *)rtos_mem_zmalloc(VFS_PATH_MAX);
	if (name == NULL) {
		VFS_DBG(VFS_ERROR, "Malloc failed");
		return -1;
	}

	if (vfs.drv[vfs_id]->vfs_type == VFS_FATFS) {
		int drv_id = 0;
		drv_id = vfs.drv[vfs_id]->get_interface(vfs.user[user_id].vfs_interface_type);
		char temp[4] = {0};
		temp[0] = drv_id + '0';
		temp[1] = ':';
		temp[2] = '/';
		DiagSnPrintf(name, VFS_PATH_MAX, "%s%s", temp, filename + prefix_len);
	} else {
		DiagSnPrintf(name, VFS_PATH_MAX, "%s", filename + prefix_len);
	}

	ret = vfs.drv[vfs_id]->remove(vfs.user[user_id].fs, name);
	rtos_mem_free(name);
	return ret;
}

int __wrap_rename(const char *oldname, const char *newname)
{
	int ret = 0;
	int prefix_len = 0;
	int user_id = 0;
	int vfs_id = find_vfs_number(oldname, &prefix_len, &user_id);
	if (vfs_id < 0) {
		VFS_DBG(VFS_ERROR, "It can't find the file system");
		return -1;
	}

	ret = vfs_check_mount_flag(vfs.drv[vfs_id]->vfs_type, vfs.user[user_id].vfs_interface_type, "rename");
	if (ret) {
		return -1;
	}

	if (vfs.user[user_id].vfs_ro_flag) {
		VFS_DBG(VFS_WARNING, "Vfs is read-only when initializing!");
		return -1;
	}

	char *old_name = (char *)rtos_mem_zmalloc(VFS_PATH_MAX);
	char *new_name = (char *)rtos_mem_zmalloc(VFS_PATH_MAX);
	if (old_name == NULL || new_name == NULL) {
		VFS_DBG(VFS_ERROR, "Malloc failed");
		if (old_name) {
			rtos_mem_free(old_name);
		}

		if (new_name) {
			rtos_mem_free(new_name);
		}
		return -1;
	}

	if (vfs.drv[vfs_id]->vfs_type == VFS_FATFS) {
		int drv_id = 0;
		drv_id = vfs.drv[vfs_id]->get_interface(vfs.user[user_id].vfs_interface_type);
		char temp[4] = {0};
		temp[0] = drv_id + '0';
		temp[1] = ':';
		temp[2] = '/';
		DiagSnPrintf(old_name, VFS_PATH_MAX, "%s%s", temp, oldname + prefix_len);
		DiagSnPrintf(new_name, VFS_PATH_MAX, "%s%s", temp, newname + prefix_len);
	} else {
		DiagSnPrintf(old_name, VFS_PATH_MAX, "%s", oldname + prefix_len);
		DiagSnPrintf(new_name, VFS_PATH_MAX, "%s", newname + prefix_len);
	}

	ret = vfs.drv[vfs_id]->rename(vfs.user[user_id].fs, old_name, new_name);
	rtos_mem_free(new_name);
	rtos_mem_free(old_name);
	return ret;
}

int __wrap_feof(FILE *stream)
{
	int ret = 0;
	vfs_file *finfo = (vfs_file *)stream;
	if (is_stdio(stream)) {
		return 0;
	}
	ret = vfs.drv[finfo->vfs_id]->eof(vfs.user[finfo->user_id].fs, (vfs_file *)stream);
	return ret;
}

int __wrap_ferror(FILE *stream)
{
	int ret = 0;
	vfs_file *finfo = (vfs_file *)stream;
	if (is_stdio(stream)) {
		return 0;
	}
	ret = vfs.drv[finfo->vfs_id]->error((vfs_file *)stream);
	return ret;
}

long int __wrap_ftell(FILE *stream)
{
	int ret = 0;
	vfs_file *finfo = (vfs_file *)stream;
	if (is_stdio(stream)) {
		return -1;
	}
	ret = vfs.drv[finfo->vfs_id]->tell(vfs.user[finfo->user_id].fs, (vfs_file *)stream);
	return ret;
}

int __wrap_ftruncate(int stream, off_t length)
{
	int ret = 0;
	vfs_file *finfo = (vfs_file *)stream;
	if (is_stdio((FILE *)stream)) {
		return -1;
	}

	ret = vfs.drv[finfo->vfs_id]->ftruncate(vfs.user[finfo->user_id].fs, (vfs_file *)stream, length);
	return ret;
}

int __wrap_fputc(int character, FILE *stream)
{
	(void) character;
	(void) stream;
	//vfs don't implement
	return -1;
}

int __wrap_fputs(const char *str, FILE *stream)
{
	(void) str;
	(void) stream;
	//vfs don't implement
	return -1;
}

char *__wrap_fgets(char *str, int num, FILE *stream)
{
	(void) str;
	(void) num;
	if (is_stdio(stream)) {
		return NULL;
	}

	//vfs don't implement
	return NULL;
}

void *__wrap_opendir(const char *name)
{
	int ret;
	int prefix_len = 0;
	int user_id = 0;
	int vfs_id = find_vfs_number(name, &prefix_len, &user_id);
	if (vfs_id < 0) {
		VFS_DBG(VFS_ERROR, "It can't find the file system");
		return NULL;
	}

	ret = vfs_check_mount_flag(vfs.drv[vfs_id]->vfs_type, vfs.user[user_id].vfs_interface_type, "opendir");
	if (ret) {
		return NULL;
	}

	vfs_file *finfo = (vfs_file *)malloc(sizeof(vfs_file));
	if (finfo == NULL) {
		return NULL;
	}
	memset(finfo, 0x00, sizeof(vfs_file));
	finfo->vfs_id = vfs_id;
	finfo->user_id = user_id;
	if (vfs.drv[vfs_id]->vfs_type == VFS_FATFS) {
		int drv_id = 0;
		drv_id = vfs.drv[vfs_id]->get_interface(vfs.user[user_id].vfs_interface_type);
		char temp[4] = {0};
		temp[0] = drv_id + '0';
		temp[1] = ':';
		temp[2] = '/';
		DiagSnPrintf(finfo->name, sizeof(finfo->name), "%s%s", temp, name + prefix_len);
	} else {
		DiagSnPrintf(finfo->name, sizeof(finfo->name), "%s", name + prefix_len);
	}

	ret = vfs.drv[vfs_id]->opendir(vfs.user[user_id].fs, finfo->name, finfo);
	if (ret != 0) {
		free(finfo);
		finfo = NULL;
	}
	return (void *)finfo;
}

struct dirent *__wrap_readdir(void *pdir)
{
	struct dirent *ent = NULL;
	vfs_file *finfo = (vfs_file *)pdir;
	ent = vfs.drv[finfo->vfs_id]->readdir(vfs.user[finfo->user_id].fs, ((vfs_file *)pdir));
	return ent;
}

int __wrap_closedir(void *dirp)
{
	int ret = 0;
	vfs_file *finfo = (vfs_file *)dirp;
	ret = vfs.drv[finfo->vfs_id]->closedir(vfs.user[finfo->user_id].fs, ((vfs_file *)dirp));
	free(finfo);
	return ret;
}

int __wrap_scandir(const char *dirp, struct dirent ***namelist,
				   int (*filter)(const struct dirent *),
				   int (*compar)(const struct dirent **, const struct dirent **))
{
	(void) dirp;
	(void) filter;
	(void) compar;
	(void) namelist;
	return -1;
}

int __wrap_rmdir(const char *path)
{
	int ret = 0;
	int prefix_len = 0;
	int user_id = 0;
	int vfs_id = find_vfs_number(path, &prefix_len, &user_id);
	if (vfs_id < 0) {
		VFS_DBG(VFS_ERROR, "It can't find the file system");
		return -1;
	}

	ret = vfs_check_mount_flag(vfs.drv[vfs_id]->vfs_type, vfs.user[user_id].vfs_interface_type, "rmdir");
	if (ret) {
		return -1;
	}

	if (vfs.user[user_id].vfs_ro_flag) {
		VFS_DBG(VFS_WARNING, "Vfs is read-only when initializing!");
		return -1;
	}

	char *name = (char *)rtos_mem_zmalloc(VFS_PATH_MAX);
	if (name == NULL) {
		VFS_DBG(VFS_ERROR, "Malloc failed");
		return -1;
	}

	if (vfs.drv[vfs_id]->vfs_type == VFS_FATFS) {
		int drv_id = 0;
		drv_id = vfs.drv[vfs_id]->get_interface(vfs.user[user_id].vfs_interface_type);
		char temp[4] = {0};
		temp[0] = drv_id + '0';
		temp[1] = ':';
		temp[2] = '/';
		DiagSnPrintf(name, VFS_PATH_MAX, "%s%s", temp, path + prefix_len);
	} else {
		DiagSnPrintf(name, VFS_PATH_MAX, "%s", path + prefix_len);
	}

	ret = vfs.drv[vfs_id]->rmdir(vfs.user[user_id].fs, name);
	rtos_mem_free(name);
	return ret;
}

int __wrap_mkdir(const char *pathname, mode_t mode)
{
	(void) mode;
	int ret = 0;
	int prefix_len = 0;
	int user_id = 0;
	int vfs_id = find_vfs_number(pathname, &prefix_len, &user_id);
	if (vfs_id < 0) {
		VFS_DBG(VFS_ERROR, "It can't find the file system");
		return -1;
	}

	ret = vfs_check_mount_flag(vfs.drv[vfs_id]->vfs_type, vfs.user[user_id].vfs_interface_type, "mkdir");
	if (ret) {
		return -1;
	}

	if (vfs.user[user_id].vfs_ro_flag) {
		VFS_DBG(VFS_WARNING, "Vfs is read-only when initializing!");
		return -1;
	}

	char *name = (char *)rtos_mem_zmalloc(VFS_PATH_MAX);
	if (name == NULL) {
		VFS_DBG(VFS_ERROR, "Malloc failed");
		return -1;
	}

	if (vfs.drv[vfs_id]->vfs_type == VFS_FATFS) {
		int drv_id = 0;
		drv_id = vfs.drv[vfs_id]->get_interface(vfs.user[user_id].vfs_interface_type);
		char temp[4] = {0};
		temp[0] = drv_id + '0';
		temp[1] = ':';
		temp[2] = '/';
		DiagSnPrintf(name, VFS_PATH_MAX, "%s%s", temp, pathname + prefix_len);
	} else {
		DiagSnPrintf(name, VFS_PATH_MAX, "%s", pathname + prefix_len);
	}

	ret = vfs.drv[vfs_id]->mkdir(vfs.user[user_id].fs, name);
	rtos_mem_free(name);
	return ret;
}

int __wrap_access(const char *pathname, int mode)
{
	int ret = 0;
	int prefix_len = 0;
	int user_id = 0;
	int vfs_id = find_vfs_number(pathname, &prefix_len, &user_id);
	if (vfs_id < 0) {
		VFS_DBG(VFS_ERROR, "It can't find the file system");
		return -1;
	}

	ret = vfs_check_mount_flag(vfs.drv[vfs_id]->vfs_type, vfs.user[user_id].vfs_interface_type, "access");
	if (ret) {
		return -1;
	}

	char *name = (char *)rtos_mem_zmalloc(VFS_PATH_MAX);
	if (name == NULL) {
		VFS_DBG(VFS_ERROR, "Malloc failed");
		return -1;
	}

	if (vfs.drv[vfs_id]->vfs_type == VFS_FATFS) {
		int drv_id = 0;
		drv_id = vfs.drv[vfs_id]->get_interface(vfs.user[user_id].vfs_interface_type);
		char temp[4] = {0};
		temp[0] = drv_id + '0';
		temp[1] = ':';
		temp[2] = '/';
		DiagSnPrintf(name, VFS_PATH_MAX, "%s%s", temp, pathname + prefix_len);
	} else {
		DiagSnPrintf(name, VFS_PATH_MAX, "%s", pathname + prefix_len);
	}

	ret = vfs.drv[vfs_id]->access(vfs.user[user_id].fs, name, mode);
	rtos_mem_free(name);
	return ret;
}

int __wrap_stat(const char *path, struct stat *buf)
{
	int ret = 0;
	int prefix_len = 0;
	int user_id = 0;
	int vfs_id = find_vfs_number(path, &prefix_len, &user_id);
	if (vfs_id < 0) {
		VFS_DBG(VFS_ERROR, "It can't find the file system");
		return -1;
	}

	ret = vfs_check_mount_flag(vfs.drv[vfs_id]->vfs_type, vfs.user[user_id].vfs_interface_type, "stat");
	if (ret) {
		return -1;
	}

	char *name = (char *)rtos_mem_zmalloc(VFS_PATH_MAX);
	if (name == NULL) {
		VFS_DBG(VFS_ERROR, "Malloc failed");
		return -1;
	}

	if (vfs.drv[vfs_id]->vfs_type == VFS_FATFS) {
		int drv_id = 0;
		drv_id = vfs.drv[vfs_id]->get_interface(vfs.user[user_id].vfs_interface_type);
		char temp[4] = {0};
		temp[0] = drv_id + '0';
		temp[1] = ':';
		temp[2] = '/';
		DiagSnPrintf(name, VFS_PATH_MAX, "%s%s", temp, path + prefix_len);
	} else {
		DiagSnPrintf(name, VFS_PATH_MAX, "%s", path + prefix_len);
	}

	ret = vfs.drv[vfs_id]->stat(vfs.user[user_id].fs, name, buf);
	rtos_mem_free(name);
	return ret;
}
