#include "platform_autoconf.h"
#include "vfs.h"
#include "vfs_fatfs.h"
#if defined(CONFIG_FATFS_DISK_FLASH) && CONFIG_FATFS_DISK_FLASH
#include "os_wrapper.h"

static fatfs_flash_params_t fatfs_flash_param;
static uint8_t fatfs_flash_init_done = 0;
static FIL     fatfs_flash_file;

int fatfs_flash_close(void)
{
	if (fatfs_flash_init_done) {
		if (f_unmount(fatfs_flash_param.drv) != FR_OK) {
			VFS_DBG(VFS_ERROR, "FATFS unmount flash logical drive fail.");
		}

		if (FATFS_UnRegisterDiskDriver(fatfs_flash_param.drv_num)) {
			VFS_DBG(VFS_ERROR, "Unregister flash disk driver from FATFS fail.");
		}

		fatfs_flash_init_done = 0;
	}
	return 0;
}

int fatfs_flash_init(void)
{
	int ret = 0;
	if (!fatfs_flash_init_done) {
		int Fatfs_ok = 0;
		FRESULT res1, res2;
		char path[64];
		char flash_test_fn[64] = "flash.txt";
		uint8_t *work_buffer = (uint8_t *)rtos_mem_malloc(4096);

		// Register disk driver to Fatfs
		VFS_DBG(VFS_INFO, "Register flash disk driver to Fatfs.");
		fatfs_flash_param.drv_num = FATFS_RegisterDiskDriver(&FLASH_disk_Driver);

		if (fatfs_flash_param.drv_num < 0) {
			VFS_DBG(VFS_ERROR, "Register flash disk driver to FATFS fail.");
		} else {
			Fatfs_ok = 1;
			fatfs_flash_param.drv[0] = fatfs_flash_param.drv_num + '0';
			fatfs_flash_param.drv[1] = ':';
			fatfs_flash_param.drv[2] = '/';
			fatfs_flash_param.drv[3] = 0;
			VFS_DBG(VFS_INFO, "Flash drive path: %s ", fatfs_flash_param.drv);
		}
		if (!Fatfs_ok) {
			ret = -1;
			goto fatfs_init_err;
		}

		res1 = f_mount(&fatfs_flash_param.fs, fatfs_flash_param.drv, 1);

		// test flash
		VFS_DBG(VFS_INFO, "Test flash drive (file: %s)", flash_test_fn);
		memset(path, 0, sizeof(path));
		strcpy(path, fatfs_flash_param.drv);

		sprintf(&path[strlen(path)], "%s", flash_test_fn);
		res2 = f_open(&fatfs_flash_file, path, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

		if (res1 || res2) {
			VFS_DBG(VFS_INFO, "Mount existed FATFS failed, try to mkfs");
			MKFS_PARM opt;
			opt.fmt = FM_ANY;
			opt.au_size = 0;
			ret = f_mkfs(fatfs_flash_param.drv, &opt, work_buffer, 4096);
			if (ret != FR_OK) {
				VFS_DBG(VFS_ERROR, "Create FAT volume on Flash fail. (%d) ", ret);
				goto fatfs_init_err;
			}

			ret = f_mount(&fatfs_flash_param.fs, fatfs_flash_param.drv, 0);
			if (ret != FR_OK) {
				VFS_DBG(VFS_ERROR, "FATFS mount logical drive on Flash fail.");
				goto fatfs_init_err;
			}
			VFS_DBG(VFS_INFO, "flash mkfs and mount OK");
		} else {
			VFS_DBG(VFS_INFO, "flash mount OK");
		}
		fatfs_flash_init_done = 1;
	} else {
		ret = -2;
	}

	return 0;

fatfs_init_err:
	fatfs_flash_close();
	return ret;
}

#endif //CONFIG_FATFS_DISK_FLASH
