#include "platform_autoconf.h"
#include "vfs.h"
#include "vfs_fatfs.h"
#include "os_wrapper.h"

int fatfs_flash_close(int interface)
{
	fatfs_params_t *fatfs_param;
	if (interface == VFS_INF_FLASH) {
		fatfs_param = &fatfs_flash_param;
	}
#if defined(CONFIG_FATFS_SECOND_FLASH) && CONFIG_FATFS_SECOND_FLASH
	if (interface == VFS_INF_SECOND_FLASH) {
		fatfs_param = &fatfs_second_flash_param;
	}
#endif

	if ((interface == VFS_INF_FLASH && fatfs_mount_flag == 1) || (interface == VFS_INF_SECOND_FLASH && fatfs2_mount_flag == 1)) {
		if (f_unmount(fatfs_param->drv) != FR_OK) {
			VFS_DBG(VFS_ERROR, "FATFS unmount flash logical drive fail.");
		}
	}

	if (FATFS_UnRegisterDiskDriver(fatfs_param->drv_num)) {
		VFS_DBG(VFS_ERROR, "Unregister flash disk driver from FATFS fail.");
	}

	return 0;
}

int fatfs_flash_init(int interface)
{
	int ret = 0;
	FRESULT res1, res2;
	fatfs_params_t *fatfs_param;

	if (interface == VFS_INF_FLASH) {
		fatfs_param = &fatfs_flash_param;
	}
#if defined(CONFIG_FATFS_SECOND_FLASH) && CONFIG_FATFS_SECOND_FLASH
	if (interface == VFS_INF_SECOND_FLASH) {
		fatfs_param = &fatfs_second_flash_param;
	}
#endif

	if ((interface == VFS_INF_FLASH && fatfs_mount_flag != 1) || (interface == VFS_INF_SECOND_FLASH && fatfs2_mount_flag != 1)) {
		// Register disk driver to Fatfs
		VFS_DBG(VFS_INFO, "Register flash disk driver to Fatfs.");
#if defined(CONFIG_FATFS_SECOND_FLASH) && CONFIG_FATFS_SECOND_FLASH
		if (interface == VFS_INF_SECOND_FLASH) {
			fatfs_param->drv_num = FATFS_RegisterDiskDriver(&FLASH_second_disk_Driver);
		}
#endif
		if (interface == VFS_INF_FLASH) {
			fatfs_param->drv_num = FATFS_RegisterDiskDriver(&FLASH_disk_Driver);
		}

		if (fatfs_param->drv_num < 0) {
			VFS_DBG(VFS_ERROR, "Register flash disk driver to FATFS fail.");
			ret = -1;
			goto fatfs_init_err;
		} else {
			fatfs_param->drv[0] = fatfs_param->drv_num + '0';
			fatfs_param->drv[1] = ':';
			fatfs_param->drv[2] = '/';
			fatfs_param->drv[3] = 0;
			VFS_DBG(VFS_INFO, "Flash drive path: %s ", fatfs_param->drv);
		}

		res1 = f_mount(&fatfs_param->fs, fatfs_param->drv, 1);

#if !FF_FS_READONLY
		// test flash
		char path[64];
		char flash_test_fn[64] = "flash.txt";
		FIL fatfs_flash_file;
		VFS_DBG(VFS_INFO, "Test flash drive (file: %s)", flash_test_fn);
		memset(path, 0, sizeof(path));
		strcpy(path, fatfs_param->drv);
		sprintf(&path[strlen(path)], "%s", flash_test_fn);
		res2 = f_open(&fatfs_flash_file, path, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
#else
		res2 = 0;
#endif

		if (res1 || res2) {
#if FF_FS_READONLY
			goto fatfs_init_err;
#else
			VFS_DBG(VFS_INFO, "Mount existed FATFS failed, try to mkfs");

			MKFS_PARM opt;
			opt.fmt = FM_ANY;
			opt.au_size = 0;
			ret = f_mkfs(fatfs_param->drv, &opt, NULL, 4096);
			if (ret != FR_OK) {
				VFS_DBG(VFS_ERROR, "Create FAT volume on Flash fail. (%d) ", ret);
				goto fatfs_init_err;
			}

			ret = f_mount(&fatfs_param->fs, fatfs_param->drv, 0);
			if (ret != FR_OK) {
				VFS_DBG(VFS_ERROR, "FATFS mount logical drive on Flash fail.");
				goto fatfs_init_err;
			}
			VFS_DBG(VFS_INFO, "flash mkfs and mount OK");
#endif
		} else {
			VFS_DBG(VFS_INFO, "flash mount OK");
		}
	} else {
		VFS_DBG(VFS_INFO, "FATFS interface : %d already initialized\r\n", interface);
	}

	return 0;

fatfs_init_err:
	fatfs_flash_close(interface);
	return ret;
}
