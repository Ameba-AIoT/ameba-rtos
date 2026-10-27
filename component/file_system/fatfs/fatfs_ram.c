/*
 *  Routines to associate the VFS RAM disk with FatFs
 *
 *  Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "platform_autoconf.h"
#include "vfs.h"
#include "vfs_fatfs.h"
#include "vfs_ram.h"
#include "os_wrapper.h"

#if defined(VFS_RAM_FATFS)

#define RAM_SECTOR_SIZE		512

static fatfs_params_t *fatfs_ram_param;		/* ~4.6 KB, allocated only while mounted */

static DSTATUS RAM_disk_status(void)
{
	return RES_OK;
}

static DSTATUS RAM_disk_initialize(void)
{
	return RES_OK;
}

static DSTATUS RAM_disk_deinitialize(void)
{
	return RES_OK;
}

static DRESULT RAM_disk_read(BYTE *buff, DWORD sector, UINT count)
{
	return vfs_ram_read(sector * RAM_SECTOR_SIZE, buff, count * RAM_SECTOR_SIZE) ? RES_PARERR : RES_OK;
}

static DRESULT RAM_disk_write(const BYTE *buff, DWORD sector, UINT count)
{
	return vfs_ram_write(sector * RAM_SECTOR_SIZE, buff, count * RAM_SECTOR_SIZE) ? RES_PARERR : RES_OK;
}

static DRESULT RAM_disk_ioctl(BYTE cmd, void *buff)
{
	switch (cmd) {
	case CTRL_SYNC:
		return RES_OK;
	case GET_SECTOR_COUNT:
		*(DWORD *)buff = vfs_ram_size() / RAM_SECTOR_SIZE;
		return RES_OK;
	case GET_SECTOR_SIZE:
		*(WORD *)buff = RAM_SECTOR_SIZE;
		return RES_OK;
	case GET_BLOCK_SIZE:
		*(DWORD *)buff = 1;		/* in sectors; RAM has no erase block */
		return RES_OK;
	default:
		return RES_PARERR;
	}
}

ll_diskio_drv RAM_disk_Driver = {
	.disk_initialize = RAM_disk_initialize,
	.disk_status = RAM_disk_status,
	.disk_read = RAM_disk_read,
	.disk_deinitialize = RAM_disk_deinitialize,
	.disk_write = RAM_disk_write,
	.disk_ioctl = RAM_disk_ioctl,
	.TAG	= (unsigned char *)"RAM"
};

int fatfs_ram_close(void)
{
	if (fatfs_ram_param != NULL) {
		if (fatfs_ram_param->drv_num >= 0) {
			/* f_mount keeps the volume registered even when mounting fails */
			f_unmount(fatfs_ram_param->drv);
			FATFS_UnRegisterDiskDriver(fatfs_ram_param->drv_num);
		}
		rtos_mem_free(fatfs_ram_param);
		fatfs_ram_param = NULL;
	}
	vfs_ram_close();		/* also clears ram_mount_flag */
	return 0;
}

/* volatile disk: allocate, then always mkfs + mount */
int fatfs_ram_init(void)
{
	MKFS_PARM opt = {0};	/* opt.align must be 0, see fatfs/CLAUDE.md */
	FRESULT res;

	if ((u32)CONFIG_VFS_RAM_SIZE_KB < 64) {
		RTK_LOGE("VFS", "ram size invalid\n");
		return -1;
	}
	if (vfs_ram_open(VFS_FATFS)) {
		return -1;
	}
	fatfs_ram_param = (fatfs_params_t *)rtos_mem_zmalloc(sizeof(fatfs_params_t));
	if (fatfs_ram_param == NULL) {
		RTK_LOGE("VFS", "ram alloc fail\n");
		goto err;
	}
	fatfs_ram_param->drv_num = FATFS_RegisterDiskDriver(&RAM_disk_Driver);
	if (fatfs_ram_param->drv_num < 0) {
		goto err;
	}
	fatfs_ram_param->drv[0] = fatfs_ram_param->drv_num + '0';
	fatfs_ram_param->drv[1] = ':';
	fatfs_ram_param->drv[2] = '/';
	fatfs_ram_param->drv[3] = 0;

	opt.fmt = FM_ANY | FM_SFD;
	res = f_mkfs(fatfs_ram_param->drv, &opt, NULL, RAM_SECTOR_SIZE);
	if (res == FR_OK) {
		res = f_mount(&fatfs_ram_param->fs, fatfs_ram_param->drv, 1);
	}
	if (res != FR_OK) {
		RTK_LOGE("VFS", "ram mkfs fail %d\n", res);
		goto err;
	}
	ram_mount_flag = 1;
	return 0;

err:
	fatfs_ram_close();
	return -1;
}

#endif
