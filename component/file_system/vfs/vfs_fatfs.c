#include <stdio.h>
#include "vfs.h"
#include "vfs_fatfs.h"
#include "ff.h"
#include "time.h"
#include "os_wrapper.h"
#include "diag.h"

int fatfs_mount_flag = 0;
static struct dirent *fatfs_ent;

// return drv_num assigned
int FATFS_RegisterDiskDriver(ll_diskio_drv *drv)
{
	unsigned char drv_num = -1;

	if (disk.nbr < _VOLUMES) {
		drv->drv_num = disk.nbr;	// record driver number for a specific disk
		disk.drv[disk.nbr] = drv;
		disk.nbr++;
		drv_num = drv->drv_num;
	}
	VFS_DBG(VFS_INFO, "FATFS Register: disk driver %d ", drv_num);
	return drv_num;
}

int FATFS_UnRegisterDiskDriver(unsigned char drv_num)
{
	int index;

	if (disk.nbr >= 1) {
		for (index = 0; index < _VOLUMES; index++) {
			if (disk.drv[index]) {
				if (disk.drv[index]->drv_num == drv_num) {
					disk.drv[index] = 0;
					disk.nbr--;
					return 0;
				}
			}
		}
		return -1; // fail
	}
	return -1; // no disk driver registered
}


/**
  * @brief  Gets number of linked drivers to the FatFs module.
  * @param  None
  * @retval Number of attached drivers.
  */
int FATFS_getDrivernum(const char *TAG)
{
	ll_diskio_drv *drv;
	unsigned int index;

	for (index = 0; index < disk.nbr; index++) {
		drv = disk.drv[index];
		if (!strcmp((char const *)drv->TAG, (char const *)TAG)) {
			return drv->drv_num;
		}
	}
	return -1;
}


int fatfs_get_interface(int interface)
{
	int drv_id = 0;
	if (interface == VFS_INF_SD) {
		drv_id = FATFS_getDrivernum("SD");
	} else if (interface == VFS_INF_RAM) {
		drv_id = FATFS_getDrivernum("RAM");
	} else if (interface == VFS_INF_FLASH) {
		drv_id = FATFS_getDrivernum("FLASH");
	}
#ifdef CONFIG_FATFS_SECONDARY_FLASH
	else if (interface == VFS_INF_SECONDARY_FLASH) {
		drv_id = FATFS_getDrivernum("SECONDARY_FLASH");
	}
#endif
	else {
		return -1;
	}
	return drv_id;
}

int fatfs_open(void *fs, const char *filename, const char *mode, vfs_file *finfo)
{
	(void) fs;
	FIL *fil = rtos_mem_malloc(sizeof(FIL));
	uint8_t mode_mapping = 0;
	FRESULT res = FR_OK;
	if (fil == NULL) {
		return -1;
	}

	if (strchr(mode, 'r')) {
		mode_mapping |= FA_READ;
	}
	if (strchr(mode, 'w')) {
		if (strchr(mode, 'x')) {
			mode_mapping |= (FA_CREATE_NEW | FA_WRITE);
		} else {
			mode_mapping |= (FA_CREATE_ALWAYS | FA_WRITE);
		}
	}
	if (strchr(mode, 'a')) {
		mode_mapping |= (FA_OPEN_APPEND | FA_WRITE);
	}
	if (strchr(mode, '+')) {
		mode_mapping |= (FA_READ | FA_WRITE);
	}

	res = f_open(fil, filename, mode_mapping);
	if (res != 0) {
		if (res == FR_NO_FILE) {
			VFS_DBG(VFS_WARNING, "file is not exist");
		} else {
			VFS_DBG(VFS_ERROR, "vfs-fatfs fopen error %d \r\n", res);
		}
		rtos_mem_free(fil);
		return -1;
	}
	finfo->file = (void *)fil;
	return res;
}

int fatfs_read(void *fs, unsigned char *buf, unsigned int size, unsigned int count, vfs_file *finfo)
{
	(void) fs;
	FIL *fil = (FIL *)finfo->file;
	size_t br;
	FRESULT res = f_read(fil, buf, size * count, (UINT *)&br);
	if (res > 0) {
		VFS_DBG(VFS_ERROR, "vfs-fatfs fread error %d \r\n", res);
		return -res;
	}
	return br;
}

int fatfs_write(void *fs, unsigned char *buf, unsigned int size, unsigned int count, vfs_file *finfo)
{
	(void) fs;
	FIL *fil = (FIL *)finfo->file;
	size_t bw;
	FRESULT res = f_write(fil, buf, size * count, (UINT *)&bw);
	if (res > 0) {
		VFS_DBG(VFS_ERROR, "vfs-fatfs fwrite error %d \r\n", res);
		return -res;
	}
	return bw;
}

int fatfs_close(void *fs, vfs_file *finfo)
{
	(void) fs;
	FIL *fil = (FIL *)finfo->file;
	FRESULT res = f_close(fil);
	rtos_mem_free(fil);
	if (res > 0) {
		VFS_DBG(VFS_ERROR, "vfs-fatfs fclose error %d \r\n", res);
		return -1;
	}
	return 0;
}

int fatfs_seek(void *fs, long int offset, int origin, vfs_file *finfo)
{
	(void) fs;
	FIL *fil = (FIL *)finfo->file;
	int size = f_size(fil);
	int curr = f_tell(fil);
	FRESULT res = FR_INT_ERR;
	int pos = 0;
	switch (origin) {
	case SEEK_SET:
		res = f_lseek(fil, offset);
		pos = offset;
		break;
	case SEEK_CUR:
		res = f_lseek(fil, curr + offset);
		pos = curr + offset;
		break;
	case SEEK_END:
		res = f_lseek(fil, size - offset);
		pos = size - offset;
		break;
	}
	if (res > 0) {
		VFS_DBG(VFS_ERROR, "vfs-fatfs fseek error %d \r\n", res);
		return -1;
	}
	return pos;
}

void fatfs_rewind(void *fs, vfs_file *finfo)
{
	(void) fs;
	FIL *fil = (FIL *)finfo->file;
	f_lseek(fil, 0);
}

int fatfs_fgetops(void *fs, vfs_file *finfo)
{
	(void) fs;
	FIL *fil = (FIL *)finfo->file;
	int value = 0;
	value = f_tell(fil);
	return value;
}

int fatfs_fsetops(void *fs, unsigned int offset, vfs_file *finfo)
{
	(void) fs;
	FIL *fil = (FIL *)finfo->file;
	int value = 0;
	value = f_lseek(fil, offset);

	if (value > 0) {
		VFS_DBG(VFS_ERROR, "vfs-fatfs fsetops error %d \r\n", value);
	}

	return value;
}

int fatfs_fflush(void *fs, vfs_file *finfo)
{
	(void) fs;
	FIL *fil = (FIL *)finfo->file;
	FRESULT res = f_sync(fil);

	if (res > 0) {
		VFS_DBG(VFS_ERROR, "vfs-fatfs fflush error %d \r\n", res);
	}

	return -res;
}

int fatfs_remove(void *fs, const char *name)
{
	(void) fs;
	FRESULT res = f_unlink(name);

	if (res > 0) {
		VFS_DBG(VFS_ERROR, "vfs-fatfs remove error %d \r\n", res);
	}

	return -res;
}

int fatfs_rename(void *fs, const char *old_name, const char *new_name)
{
	(void) fs;
	FRESULT res = f_rename(old_name, new_name);

	if (res > 0) {
		VFS_DBG(VFS_ERROR, "vfs-fatfs remove error %d \r\n", res);
	}

	return -res;
}

int fatfs_feof(void *fs, vfs_file *finfo)
{
	(void) fs;
	FIL *fil = (FIL *)finfo->file;
	return f_eof(fil);
}

int fatfs_ferror(vfs_file *finfo)
{
	FIL *fil = (FIL *)finfo->file;
	return f_error(fil);
}

int fatfs_ftell(void *fs, vfs_file *finfo)
{
	(void) fs;
	FIL *fil = (FIL *)finfo->file;
	return f_tell(fil);
}

int fatfs_ftruncate(void *fs, vfs_file *finfo, off_t length)
{
	(void) fs;
	FIL *fil = (FIL *)finfo->file;
	FRESULT res = FR_INT_ERR;
	res = f_lseek(fil, length);
	if (res > 0) {
		return -1;
	}
	res = f_truncate(fil);
	if (res > 0) {
		VFS_DBG(VFS_ERROR, "vfs-fatfs ftruncate error %d \r\n", res);
		return -1;
	}
	return 0;
}

int fatfs_opendir(void *fs, const char *name, vfs_file *finfo)
{
	(void) fs;
	DIR *pdir = rtos_mem_malloc(sizeof(DIR));
	FRESULT res = FR_OK;
	if (pdir == NULL) {
		return -1;
	}
	finfo->file = (void *)pdir;
	res = f_opendir(pdir, name);
	if (res > 0) {
		VFS_DBG(VFS_ERROR, "vfs-fatfs opendir error %d \r\n", res);
	}
	return res;
}

struct dirent *fatfs_readdir(void *fs, vfs_file *finfo)
{
	(void) fs;
	DIR *pdir = (DIR *)finfo->file;
	FRESULT res;
	char *fn;
	if (pdir == NULL) {
		return NULL;
	}
	if (fatfs_ent == NULL) {
		fatfs_ent = rtos_mem_malloc(sizeof(struct dirent));
		if (fatfs_ent == NULL) {
			return NULL;
		}
	}
	FILINFO m_fileinfo;

	res = f_readdir(pdir, &m_fileinfo);
	if (res != FR_OK) {
		rtos_mem_free(fatfs_ent);
		fatfs_ent = NULL;
		VFS_DBG(VFS_ERROR, "vfs-fatfs readdir: error (%d)", res);
		return NULL;
	}

	if (m_fileinfo.fname[0] == 0) {
		rtos_mem_free(fatfs_ent);
		fatfs_ent = NULL;
		return NULL;
	}

	fn = m_fileinfo.fname;
	fatfs_ent->d_ino = 0;
	fatfs_ent->d_off = 0;
	fatfs_ent->d_reclen = m_fileinfo.fsize;
	if (m_fileinfo.fattrib & AM_DIR) {
		fatfs_ent->d_type = DT_DIR;    // directory
	} else {
		fatfs_ent->d_type = DT_REG;    // regular file
	}

	fn = m_fileinfo.fname;
	sprintf(fatfs_ent->d_name, "%s", fn);
	return fatfs_ent;
}

int fatfs_closedir(void *fs, vfs_file *finfo)
{
	(void) fs;
	DIR *pdir = (DIR *)finfo->file;
	FRESULT res = f_closedir(pdir);
	rtos_mem_free(pdir);
	if (fatfs_ent != NULL) {
		rtos_mem_free(fatfs_ent);
		fatfs_ent = NULL;
	}
	if (res > 0) {
		VFS_DBG(VFS_ERROR, "vfs-fatfs closedir error %d \r\n", res);
		return -1;
	} else {
		return 0;
	}
}

int fatfs_mkdir(void *fs, const char *pathname)
{
	(void) fs;
	FRESULT res = f_mkdir(pathname);
	if (res > 0 && res != FR_EXIST) {
		VFS_DBG(VFS_ERROR, "vfs-fatfs mkdir error %d \r\n", res);
	}
	return -res;
}

int fatfs_rmdir(void *fs, const char *path)
{
	(void) fs;
	FRESULT res = f_unlink(path);
	if (res > 0) {
		VFS_DBG(VFS_ERROR, "vfs-fatfs rmdir error %d \r\n", res);
	}
	return -res;
}

int fatfs_access(void *fs, const char *pathname, int mode)
{
	(void) fs;
	FRESULT res;
	FILINFO finfo;

	res = f_stat(pathname, &finfo);

	VFS_DBG(VFS_INFO, "%s, check 0x%x", pathname, mode);

	if (res == FR_NO_FILE) {
		return -1;
	}



	if (mode & R_OK) {
		return 0;
	}

	if (mode & W_OK) {
		if (finfo.fattrib & AM_RDO) {
			return -1;
		} else {
			return 0;
		}
	}

	if (mode & X_OK) {
		if (finfo.fattrib & AM_RDO) {
			return -1;
		} else {
			return 0;
		}
	}
	return 0;
}

int fatfs_stat(void *fs, char *path, struct stat *buf)
{
	(void) fs;
	FILINFO finfo;

	FRESULT res = f_stat(path, &finfo);
	if (res == FR_NO_FILE) {
		return -1;
	}

	buf->st_dev = 0;
	buf->st_ino = 0;

	mode_t st_mode = 0;
	if (finfo.fattrib & AM_DIR) {
		if (finfo.fattrib & AM_RDO) {
			st_mode |= (S_IFDIR | 0x555);
		} else {
			st_mode |= (S_IFDIR | 0x777);
		}
	} else {
		if (finfo.fattrib & AM_RDO) {
			st_mode |= (S_IFREG | 0x555);
		} else {
			st_mode |= (S_IFREG | 0x777);
		}
	}

	buf->st_mode = st_mode;
	buf->st_nlink = 0;
	buf->st_uid = 0;
	buf->st_gid = 0;
	buf->st_rdev = 0;
	buf->st_size = finfo.fsize;
	buf->st_blksize = 0;
	buf->st_blocks = 0;

	struct tm tm  = {0};
	int year = (finfo.fdate >> 9) + 1980;
	int month = (finfo.fdate >> 5) & 15;
	int day = finfo.fdate & 31;
	int hour = finfo.ftime >> 11;
	int minute = (finfo.ftime >> 5) & 63;
	char timestr[512];
	snprintf(timestr, sizeof(timestr), "%d/%d/%d %d:%d", year, month, day, hour, minute);

	tm.tm_year = year - 1900; /* years since 1900 */
	tm.tm_mon = month - 1;
	tm.tm_mday = day;
	tm.tm_hour = hour;
	tm.tm_min = minute;

	time_t t = mktime(&tm);

	buf->st_atime = t;
	buf->st_mtime = t;
	buf->st_ctime = t;
	return 0;
}

int fatfs_mount(int interface)
{
	int ret = -1;
	if (interface == VFS_INF_SD) {
		VFS_DBG(VFS_INFO, "sd mount");
#if defined(CONFIG_FATFS_DISK_SD) && CONFIG_FATFS_DISK_SD
		ret = fatfs_sd_init();
#endif
	} else if (interface != VFS_INF_RAM) {
		VFS_DBG(VFS_INFO, "flash mount");
#if defined(CONFIG_FATFS_DISK_FLASH) && CONFIG_FATFS_DISK_FLASH
		ret = fatfs_flash_init(interface);
#endif
	} else {
		VFS_DBG(VFS_ERROR, "It don't support the interface %d", interface);
	}

	if (ret) {
		fatfs_mount_flag = -1;
	} else {
		fatfs_mount_flag = 1;
	}

	return ret;
}

int fatfs_ummount(int interface)
{
	int ret = 0;
	if (interface == VFS_INF_SD) {
		VFS_DBG(VFS_INFO, "sd unmount");
#if defined(CONFIG_FATFS_DISK_SD) && CONFIG_FATFS_DISK_SD
		ret = fatfs_sd_close();
#endif
	} else if (interface != VFS_INF_RAM) {
		VFS_DBG(VFS_INFO, "flash unmount");
#if defined(CONFIG_FATFS_DISK_FLASH) && CONFIG_FATFS_DISK_FLASH
		ret = fatfs_flash_close();
#endif
	} else {
		VFS_DBG(VFS_ERROR, "It don't support the interface %d", interface);
		return -1;
	}
	return ret;
}

vfs_opt fatfs_drv = {
#if !FF_FS_READONLY
	.write = fatfs_write,
	.fflush = fatfs_fflush,
	.remove = fatfs_remove,
	.rename = fatfs_rename,
	.ftruncate = fatfs_ftruncate,
	.mkdir = fatfs_mkdir,
	.rmdir = fatfs_rmdir,
#endif
	.open = fatfs_open,
	.read = fatfs_read,
	.close = fatfs_close,
	.seek  = fatfs_seek,
	.rewind = fatfs_rewind,
	.fgetpos = fatfs_fgetops,
	.fsetpos = fatfs_fsetops,
	.eof   = fatfs_feof,
	.error = fatfs_ferror, //ferror
	.tell  = fatfs_ftell,
	.opendir = fatfs_opendir,
	.readdir = fatfs_readdir,
	.closedir = fatfs_closedir,
	.stat = fatfs_stat,
	.access = fatfs_access,
	.mount = fatfs_mount,
	.unmount = fatfs_ummount,
	.get_interface = fatfs_get_interface,
	.TAG	= "fatfs"
};
