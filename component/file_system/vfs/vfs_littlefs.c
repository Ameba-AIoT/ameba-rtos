#include <stdio.h>
#include "time.h"
#include "os_wrapper.h"
#include <cmsis.h>
#include <stdint.h>
#include <stdlib.h>
#include "section_config.h"

#include "lfs_util.h"
#include "vfs.h"
#include "lfs.h"

extern lfs_t g_lfs;
extern lfs_t g_second_lfs;
extern int rt_lfs_init(lfs_t *lfs);
int lfs_mount_flag = 0;
int lfs2_mount_flag = 0;
static struct dirent *lfs_ent;

int fmodeflags(const char *mode)
{
	int flags;
	if (strchr(mode, '+')) {
		flags = VFS_O_RDWR;
	} else if (*mode == 'r') {
		flags = VFS_O_RDONLY;
	} else {
		flags = VFS_O_WRONLY;
	}
	if (strchr(mode, 'x')) {
		flags |= VFS_O_EXCL;
	}
	if (strchr(mode, 'e')) {
		flags |= VFS_O_CLOEXEC;
	}
	if (*mode != 'r') {
		flags |= VFS_O_CREAT;
	}
	if (*mode == 'w') {
		flags |= VFS_O_TRUNC;
	}
	if (*mode == 'a') {
		flags |= VFS_O_APPEND;
	}
	return flags;
}

int littlefs_open(void *fs, const char *filename, const char *mode, vfs_file *finfo)
{
	int ret = 0;
	int flags = fmodeflags(mode);
	int mode_flag = 0;
	lfs_file_t *file = rtos_mem_malloc(sizeof(lfs_file_t));
	if (file == NULL) {
		return -1;
	}

	if ((flags & 3) == VFS_O_RDONLY) {
		mode_flag |= LFS_O_RDONLY;
	}
	if ((flags & 3) == VFS_O_WRONLY) {
		mode_flag |= LFS_O_WRONLY;
	}
	if ((flags & 3) == VFS_O_RDWR) {
		mode_flag |= LFS_O_RDWR;
	}
	if (flags & VFS_O_CREAT) {
		mode_flag |= LFS_O_CREAT;
	}
	if (flags & VFS_O_EXCL) {
		mode_flag |= LFS_O_EXCL;
	}
	if (flags & VFS_O_TRUNC) {
		mode_flag |= LFS_O_TRUNC;
	}
	if (flags & VFS_O_APPEND) {
		mode_flag |= LFS_O_APPEND;
	}

	ret = lfs_file_open(fs, file, filename, mode_flag);
	if (ret < 0) {
		if (ret == LFS_ERR_NOENT) {
			VFS_DBG(VFS_WARNING, "file is not exist");
		} else if (ret == LFS_ERR_EXIST) {
			VFS_DBG(VFS_WARNING, "file already exist %d", ret);
		} else {
			VFS_DBG(VFS_ERROR, "Open file error %d", ret);
		}
		rtos_mem_free(file);
		return ret;
	}
	VFS_DBG(VFS_INFO, "Open file mode 0x%x", mode_flag);
	finfo->file = (void *)file;
	return ret;
}

int littlefs_read(void *fs, unsigned char *buf, unsigned int size, unsigned int count, vfs_file *finfo)
{
	int ret = 0;
	lfs_file_t *file = (lfs_file_t *)finfo->file;
	ret = lfs_file_read(fs, file, buf, size * count);
	if (ret < 0) {
		VFS_DBG(VFS_ERROR, "vfs-littlefs fread error %d \r\n", ret);
	}
	return ret;
}

int littlefs_write(void *fs, unsigned char *buf, unsigned int size, unsigned int count, vfs_file *finfo)
{
	int ret = 0;
	lfs_file_t *file = (lfs_file_t *)finfo->file;
	ret = lfs_file_write(fs, file, buf, size * count);
	if (ret < 0) {
		VFS_DBG(VFS_ERROR, "vfs-littlefs fwrite error %d \r\n", ret);
	}
	return ret;
}

int littlefs_close(void *fs, vfs_file *finfo)
{
	int ret = 0;
	lfs_file_t *file = (lfs_file_t *)finfo->file;
	ret = lfs_file_close(fs, file);
	if (file) {
		rtos_mem_free(file);
	}

	if (ret < 0) {
		VFS_DBG(VFS_ERROR, "vfs-littlefs fclose error %d \r\n", ret);
	}

	return ret;
}

int littlefs_seek(void *fs, long int offset, int origin, vfs_file *finfo)
{
	int ret = 0;
	lfs_file_t *file = (lfs_file_t *)finfo->file;
	switch (origin) {
	case SEEK_SET:
		ret = lfs_file_seek(fs, file, offset, LFS_SEEK_SET);
		break;
	case SEEK_CUR:
		ret = lfs_file_seek(fs, file, offset, LFS_SEEK_CUR);
		break;
	case SEEK_END:
		ret = lfs_file_seek(fs, file, offset, LFS_SEEK_END);
		break;
	}

	if (ret < 0) {
		VFS_DBG(VFS_ERROR, "vfs-littlefs fseek error %d \r\n", ret);
	}

	return ret;
}

void littlefs_rewind(void *fs, vfs_file *finfo)
{
	lfs_file_t *file = (lfs_file_t *)finfo->file;
	lfs_file_rewind(fs, file);
}

int littlefs_fgetops(void *fs, vfs_file *finfo)
{
	int location = 0;
	lfs_file_t *file = (lfs_file_t *)finfo->file;
	location = lfs_file_tell(fs, file);
	return location;
}

int littlefs_fsetops(void *fs, unsigned int offset, vfs_file *finfo)
{
	int ret = 0;
	lfs_file_t *file = (lfs_file_t *)finfo->file;
	ret = lfs_file_seek(fs, file, offset, LFS_SEEK_SET);

	if (ret < 0) {
		VFS_DBG(VFS_ERROR, "vfs-littlefs fsetops error %d \r\n", ret);
	}

	return ret;
}

int littlefs_fflush(void *fs, vfs_file *finfo)
{
	int ret = 0;
	lfs_file_t *file = (lfs_file_t *)finfo->file;
	ret = lfs_file_sync(fs, file);

	if (ret < 0) {
		VFS_DBG(VFS_ERROR, "vfs-littlefs fflush error %d \r\n", ret);
	}

	return ret;
}

int littlefs_remove(void *fs, const char *name)
{
	int ret = 0;
	ret = lfs_remove(fs, name);

	if (ret == LFS_ERR_NOENT) {
		VFS_DBG(VFS_INFO, "The file to be deleted does not exist.\r\n");
	} else if (ret < 0) {
		VFS_DBG(VFS_ERROR, "vfs-littlefs remove error %d \r\n", ret);
	}

	return ret;
}

int littlefs_rename(void *fs, const char *old_name, const char *new_name)
{
	int ret = 0;
	ret = lfs_rename(fs, old_name, new_name);

	if (ret < 0) {
		VFS_DBG(VFS_ERROR, "vfs-littlefs remove error %d \r\n", ret);
	}

	return ret;
}

int littlefs_feof(void *fs, vfs_file *finfo)
{
	int location = 0;//lfs_file_size
	int size = 0;
	lfs_file_t *file = (lfs_file_t *)finfo->file;
	location = lfs_file_tell(fs, file);
	size = lfs_file_size(fs, file);
	if ((size - location) <= 0) {
		VFS_DBG(VFS_INFO, "End of file");
		return 1;
	} else {
		VFS_DBG(VFS_INFO, "Remain %d", size - location);
		return 0;
	}
}

int littlefs_ferror(vfs_file *finfo)
{
	(void) finfo;
	return 0;
}

int littlefs_ftell(void *fs, vfs_file *finfo)
{
	int location = 0;
	lfs_file_t *file = (lfs_file_t *)finfo->file;
	location = lfs_file_tell(fs, file);
	return location;
}

int littlefs_ftruncate(void *fs, vfs_file *finfo, off_t length)
{
	int ret = 0;
	lfs_file_t *file = (lfs_file_t *)finfo->file;
	ret = lfs_file_truncate(fs, file, length);
	if (ret < 0) {
		VFS_DBG(VFS_ERROR, "vfs-littlefs ftruncate error %d \r\n", ret);
		return -1;
	}
	return 0;
}

int littlefs_opendir(void *fs, const char *name, vfs_file *finfo)
{
	lfs_dir_t *dir = rtos_mem_malloc(sizeof(lfs_dir_t));
	if (dir == NULL) {
		return -1;
	}
	memset(dir, 0, sizeof(lfs_dir_t));

	int err = lfs_dir_open(fs, dir, name);
	if (err) {
		VFS_DBG(VFS_ERROR, "vfs-littlefs opendir error %d \r\n", err);
		rtos_mem_free(dir);
		return err;
	}
	finfo->file = (void *)dir;
	return err;
}

struct dirent *littlefs_readdir(void *fs, vfs_file *finfo)
{
	lfs_dir_t *dir = (lfs_dir_t *)finfo->file;
	struct lfs_info info;
	if (lfs_ent == NULL) {
		lfs_ent = rtos_mem_malloc(sizeof(struct dirent));
		if (lfs_ent == NULL) {
			return NULL;
		}
	}
	memset(lfs_ent, 0, sizeof(struct dirent));
	int err = lfs_dir_read(fs, dir, &info);
	if (err <= 0) {
		return NULL;
	}
	if (info.name[0] == 0) {
		return NULL;
	}
	lfs_ent->d_ino = 0;
	lfs_ent->d_off = 0;
	lfs_ent->d_reclen = info.size;
	if (info.type == LFS_TYPE_DIR) {
		lfs_ent->d_type = DT_DIR;
	} else {
		lfs_ent->d_type = DT_REG;
	}
	sprintf(lfs_ent->d_name, "%s", info.name);
	return lfs_ent;
}

int littlefs_closedir(void *fs, vfs_file *finfo)
{
	int ret = 0;
	lfs_dir_t *dir = (lfs_dir_t *)finfo->file;
	ret = lfs_dir_close(fs, dir);
	rtos_mem_free(dir);
	if (lfs_ent != NULL) {
		rtos_mem_free(lfs_ent);
		lfs_ent = NULL;
	}
	if (ret < 0) {
		VFS_DBG(VFS_ERROR, "vfs-littlefs Close directory fail: %d", ret);
	}
	return ret;
}

int littlefs_mkdir(void *fs, const char *pathname)
{
	int ret = 0;
	ret = lfs_mkdir(fs, pathname);
	if (ret < 0 && ret != LFS_ERR_EXIST) {
		VFS_DBG(VFS_ERROR, "vfs-littlefs mkdir fail: %d", ret);
	}
	return ret;
}

int littlefs_rmdir(void *fs, const char *path)
{
	int ret = 0;
	ret = lfs_remove(fs, path);
	if (ret < 0) {
		VFS_DBG(VFS_ERROR, "vfs-littlefs rmdir fail: %d", ret);
	}
	return ret;
}

int littlefs_access(void *fs, const char *pathname, int mode)
{
	(void) mode;
	struct lfs_info info;
	int ret = 0;
	ret = lfs_stat(fs, pathname, &info);
	if (ret < 0) {
		return -1;
	} else {
		return ret;
	}
}

int littlefs_stat(void *fs, char *path, struct stat *buf)
{
	struct lfs_info info;
	int ret = 0;
	ret = lfs_stat(fs, path, &info);
	if (ret < 0) {
		return -1;
	}
	buf->st_dev = 0;
	buf->st_ino = 0;
	mode_t st_mode = 0;
	if (info.type == LFS_TYPE_DIR) {
		st_mode |= (S_IFDIR | 0x777);
	} else {
		st_mode |= (S_IFREG | 0x777);
	}

	buf->st_mode = st_mode;
	buf->st_nlink = 0;
	buf->st_uid = 0;
	buf->st_gid = 0;
	buf->st_rdev = 0;
	buf->st_size = info.size;
	buf->st_blksize = 0;
	buf->st_blocks = 0;
	buf->st_atime = 0;
	buf->st_mtime = 0;
	buf->st_ctime = 0;
	return ret;
}

int littlefs_mount(int interface)
{
	int ret = 0;

	if (interface == VFS_INF_FLASH) {
		ret = rt_lfs_init(&g_lfs);
	}
#ifdef CONFIG_LITTLEFS_SECOND_FLASH
	if (interface == VFS_INF_SECOND_FLASH) {
		ret = rt_lfs_init(&g_second_lfs);
	}
#endif

	if (ret) {
		VFS_DBG(VFS_ERROR, "Littlefs mount fail, ret is %d", ret);
		if (interface == VFS_INF_SECOND_FLASH) {
			lfs2_mount_flag = -1;
		} else if (interface == VFS_INF_FLASH) {
			lfs_mount_flag = -1;
		}
		return ret;
	}
	VFS_DBG(VFS_INFO, "Littlefs mount");
	if (interface == VFS_INF_SECOND_FLASH) {
		lfs2_mount_flag = 1;
	} else if (interface == VFS_INF_FLASH) {
		lfs_mount_flag = 1;
	}

	return ret;
}

int littlefs_unmount(int interface)
{
	(void) interface;
	int ret = 0;

	if (interface == VFS_INF_FLASH) {
		ret = lfs_unmount(&g_lfs);
	}
#ifdef CONFIG_LITTLEFS_SECOND_FLASH
	if (interface == VFS_INF_SECOND_FLASH) {
		ret = lfs_unmount(&g_second_lfs);
	}
#endif

	VFS_DBG(VFS_INFO, "Littlefs unmount");
	if (interface == VFS_INF_SECOND_FLASH) {
		lfs2_mount_flag = 0;
	} else if (interface == VFS_INF_FLASH) {
		lfs_mount_flag = 0;
	}

	return ret;
}

vfs_opt littlefs_drv = {
	.open = littlefs_open,
	.read = littlefs_read,
	.write = littlefs_write,
	.close = littlefs_close,
	.seek  = littlefs_seek,
	.rewind = littlefs_rewind,
	.fgetpos = littlefs_fgetops,
	.fsetpos = littlefs_fsetops,
	.fflush = littlefs_fflush,
	.remove = littlefs_remove,
	.rename = littlefs_rename,
	.eof   = littlefs_feof,
	.error = littlefs_ferror, //ferror
	.tell  = littlefs_ftell,
	.ftruncate = littlefs_ftruncate,
	.opendir = littlefs_opendir,
	.readdir = littlefs_readdir,
	.closedir = littlefs_closedir,
	.mkdir = littlefs_mkdir,
	.rmdir = littlefs_rmdir,
	.stat = littlefs_stat,
	.access = littlefs_access,
	.mount = littlefs_mount,
	.unmount = littlefs_unmount,
	.TAG	= "littlefs"
};