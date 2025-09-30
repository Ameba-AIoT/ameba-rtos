#include "platform_autoconf.h"
#include "vfs.h"
#include "vfs_fatfs.h"
#if defined(CONFIG_FATFS_DISK_SD) && CONFIG_FATFS_DISK_SD

static fatfs_sd_params_t fatfs_sd_param;
static uint8_t fatfs_sd_init_done = 0;

int fatfs_sd_close(void)
{
	if (fatfs_sd_init_done) {
		if (f_unmount(fatfs_sd_param.drv) != FR_OK) {
			VFS_DBG(VFS_ERROR, "FATFS unmount logical drive fail.");
		}

		if (FATFS_UnRegisterDiskDriver(fatfs_sd_param.drv_num)) {
			VFS_DBG(VFS_ERROR, "Unregister disk driver from FATFS fail.");
		}

		fatfs_sd_init_done = 0;
	}
	return 0;
}

int fatfs_sd_init(void)
{
	int ret = 0;

	if (!fatfs_sd_init_done) {
		fatfs_sd_init_done = 1;
		int Fatfs_ok = 0;
		FRESULT res;
		// Register disk driver to Fatfs
		VFS_DBG(VFS_INFO, "Register disk driver to Fatfs.");
#if defined(CONFIG_FATFS_SD_SPI_MODE) && CONFIG_FATFS_SD_SPI_MODE
		fatfs_sd_param.drv_num = FATFS_RegisterDiskDriver(&SD_disk_spi_Driver);
#else
		fatfs_sd_param.drv_num = FATFS_RegisterDiskDriver(&SD_disk_Driver);
#endif

		if (fatfs_sd_param.drv_num < 0) {
			VFS_DBG(VFS_ERROR, "Rigester disk driver to FATFS fail.");
		} else {
			Fatfs_ok = 1;
			fatfs_sd_param.drv[0] = fatfs_sd_param.drv_num + '0';
			fatfs_sd_param.drv[1] = ':';
			fatfs_sd_param.drv[2] = '/';
			fatfs_sd_param.drv[3] = 0;
		}
		if (!Fatfs_ok) {
			ret = -1;
			goto fatfs_init_err;
		}

		res = f_mount(&fatfs_sd_param.fs, fatfs_sd_param.drv, 1);
		if (res) {
			VFS_DBG(VFS_ERROR, "f_mount error here, please insert SD card or use f_mkfs to format SD card to FAT32");
			//res = f_mkfs(fatfs_sd_param.drv,0,0);
			if (f_mount(&fatfs_sd_param.fs, fatfs_sd_param.drv, 0) != FR_OK) {
				VFS_DBG(VFS_ERROR, "FATFS mount logical drive on sd card fail.");
				ret = -2;
				goto fatfs_init_err;
			} else {
				fatfs_sd_init_done = 1;
			}
		} else {
			fatfs_sd_init_done = 1;
		}
	} else {

	}
	return 0;

fatfs_init_err:
	fatfs_sd_close();
	return ret;
}

#endif //CONFIG_FATFS_DISK_SD
