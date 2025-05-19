#ifndef _VFS_FATFS_DEVICE_H
#define _VFS_FATFS_DEVICE_H

#include "ff.h"
#include "diskio.h"

#define __I     volatile const
#define __O     volatile
#define __IO    volatile


typedef struct {
	DSTATUS(*disk_initialize)(void);                       /*!< Initialize Disk Drive                     */
	DSTATUS(*disk_deinitialize)(void);                       /*!< Deinitialize Disk Drive                     */
	DSTATUS(*disk_status)(void);                           /*!< Get Disk Status                           */
	DRESULT(*disk_read)(BYTE *, DWORD, UINT);              /*!< Read Sector(s)                            */
#if _USE_WRITE == 1
	DRESULT(*disk_write)(const BYTE *, DWORD, UINT);       /*!< Write Sector(s) when _USE_WRITE = 0       */
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
	DRESULT(*disk_ioctl)(BYTE, void *);                    /*!< I/O control operation when _USE_IOCTL = 1 */
#endif /* _USE_IOCTL == 1 */
	unsigned char	*TAG;
	unsigned char drv_num;
} ll_diskio_drv;

typedef struct {
	ll_diskio_drv	*drv[_VOLUMES];
	unsigned int            nbr;
} ff_disk_drv;

extern ff_disk_drv  disk;
extern ll_diskio_drv SD_disk_Driver;
extern ll_diskio_drv FLASH_disk_Driver;

typedef struct fatfs_flash_param_s {
	int drv_num;
	char drv[4];
	FATFS fs;
} fatfs_flash_params_t;

typedef struct fatfs_sd_param_s {
	int drv_num;
	char drv[4];
	FATFS fs;
} fatfs_sd_params_t;

int fatfs_sd_init(void);
int fatfs_sd_close(void);

int fatfs_flash_init(void);
int fatfs_flash_close(void);

int FATFS_RegisterDiskDriver(ll_diskio_drv *disk_drv);
int FATFS_UnRegisterDiskDriver(unsigned char drv_num);
int FATFS_getDrivernum(const char *TAG);

#endif