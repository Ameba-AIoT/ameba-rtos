#include "platform_autoconf.h"
#include "vfs.h"
#include "vfs_fatfs.h"
#include "ameba.h"
#if defined(CONFIG_FATFS_SD_MODE) || defined(CONFIG_FATFS_SD_SPI_MODE)

#if defined(CONFIG_FATFS_SD_MODE) && CONFIG_FATFS_SD_MODE
#include "ameba_sd.h"
#endif

int fatfs_sd_close(void)
{
	if (fatfs2_mount_flag == 1) {
		if (f_unmount(fatfs_second_flash_param.drv) != FR_OK) {
			VFS_DBG(VFS_ERROR, "FATFS unmount logical drive fail.");
		}

		if (FATFS_UnRegisterDiskDriver(fatfs_second_flash_param.drv_num)) {
			VFS_DBG(VFS_ERROR, "Unregister sd driver from FATFS fail.");
		}
	}
	return 0;
}

int fatfs_sd_init(int interface)
{
	int ret = 0;

	if (fatfs2_mount_flag != 1) {
		int Fatfs_ok = 0;
		FRESULT res;
		// Register disk driver to Fatfs
		VFS_DBG(VFS_INFO, "Register sd driver to Fatfs.");

#ifdef CONFIG_FATFS_SD_SPI_MODE
		if (interface == VFS_INF_SD_SPI) {
			fatfs_second_flash_param.drv_num = FATFS_RegisterDiskDriver(&SD_disk_spi_Driver);
		}
#endif
#ifdef CONFIG_FATFS_SD_MODE
		if (interface == VFS_INF_SD) {
			fatfs_second_flash_param.drv_num = FATFS_RegisterDiskDriver(&SD_disk_Driver);
		}
#endif

		if (fatfs_second_flash_param.drv_num < 0) {
			VFS_DBG(VFS_ERROR, "Rigester sd driver to FATFS fail.");
		} else {
			Fatfs_ok = 1;
			fatfs_second_flash_param.drv[0] = fatfs_second_flash_param.drv_num + '0';
			fatfs_second_flash_param.drv[1] = ':';
			fatfs_second_flash_param.drv[2] = '/';
			fatfs_second_flash_param.drv[3] = 0;
		}
		if (!Fatfs_ok) {
			ret = -1;
			goto fatfs_init_err;
		}

		res = f_mount(&fatfs_second_flash_param.fs, fatfs_second_flash_param.drv, 1);
		if (res) {
			VFS_DBG(VFS_ERROR, "f_mount error here, please insert SD card or use f_mkfs to format SD card to FAT32");
			//res = f_mkfs(fatfs_second_flash_param.drv,0,0);
			if (f_mount(&fatfs_second_flash_param.fs, fatfs_second_flash_param.drv, 0) != FR_OK) {
				VFS_DBG(VFS_ERROR, "FATFS mount logical drive on sd card fail.");
				ret = -2;
				goto fatfs_init_err;
			}
		}
	} else {
		VFS_DBG(VFS_INFO, "FATFS SD already initialized\r\n");
	}
	return 0;

fatfs_init_err:
	fatfs_sd_close();
	return ret;
}

#endif
