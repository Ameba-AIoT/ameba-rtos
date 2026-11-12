#include "platform_autoconf.h"
#include "vfs.h"
#include "vfs_fatfs.h"
#include "os_wrapper.h"

int fatfs_flash_close(int target)
{
	fatfs_params_t *fatfs_param;
	ll_diskio_drv *disk_drv;
	volatile uint8_t *mount_flag;

	switch (target) {
	case VFS_TARGET_FLASH_VFS2:
	case VFS_TARGET_FLASH_APP_FATFS:
		fatfs_param = &fatfs_flash_param;
		disk_drv = &FLASH_disk_Driver;
		mount_flag = &fatfs_mount_flag;
		break;
#if defined(CONFIG_FATFS_SECOND_FLASH) && CONFIG_FATFS_SECOND_FLASH
	case VFS_TARGET_SECOND_FLASH:
		fatfs_param = &fatfs_second_flash_param;
		disk_drv = &FLASH_second_disk_Driver;
		mount_flag = &fatfs2_mount_flag;
		break;
#endif
	default:
		return -1;
	}

	if (*mount_flag == 1) {
		if (f_unmount(fatfs_param->drv) != FR_OK) {
			VFS_DBG(VFS_ERROR, "FATFS unmount flash logical drive fail.");
		}
		disk_drv->disk_deinitialize();
	}

	if (FATFS_UnRegisterDiskDriver(fatfs_param->drv_num)) {
		VFS_DBG(VFS_ERROR, "Unregister flash disk driver from FATFS fail.");
	}

	return 0;
}

int fatfs_flash_init(int target)
{
	int ret = 0;
	fatfs_params_t *fatfs_param;
	ll_diskio_drv *disk_drv;
	volatile uint8_t *mount_flag;

	switch (target) {
	case VFS_TARGET_FLASH_VFS2:
	case VFS_TARGET_FLASH_APP_FATFS:
		fatfs_param = &fatfs_flash_param;
		disk_drv = &FLASH_disk_Driver;
		mount_flag = &fatfs_mount_flag;
		break;
#if defined(CONFIG_FATFS_SECOND_FLASH) && CONFIG_FATFS_SECOND_FLASH
	case VFS_TARGET_SECOND_FLASH:
		fatfs_param = &fatfs_second_flash_param;
		disk_drv = &FLASH_second_disk_Driver;
		mount_flag = &fatfs2_mount_flag;
		break;
#endif
	default:
		return -1;
	}

	if (*mount_flag != 1) {
		// Register disk driver to Fatfs
		VFS_DBG(VFS_INFO, "Register flash disk driver to Fatfs.");
		fatfs_param->drv_num = FATFS_RegisterDiskDriver(disk_drv);

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

		ret = f_mount(&fatfs_param->fs, fatfs_param->drv, 1);
		if (ret) {
			/* APP images are read-only even in a writable FATFS build. */
			if (target == VFS_TARGET_FLASH_APP_FATFS) {
				goto fatfs_init_err;
			}
#if FF_FS_READONLY
			goto fatfs_init_err;
#else
			VFS_DBG(VFS_INFO, "Mount existed FATFS failed, try to mkfs");

			MKFS_PARM opt = {0};
			opt.fmt = FM_ANY;
			ret = f_mkfs(fatfs_param->drv, &opt, NULL, 4096);
			if (ret != FR_OK) {
				VFS_DBG(VFS_ERROR, "Create FAT volume on Flash fail. (%d) ", ret);
				goto fatfs_init_err;
			}

			ret = f_mount(&fatfs_param->fs, fatfs_param->drv, 1);
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
		VFS_DBG(VFS_INFO, "FATFS target %d already initialized\r\n", target);
	}

	return 0;

fatfs_init_err:
	if (fatfs_param->drv_num >= 0) {
		/* f_mount registers the volume even when its immediate mount fails. */
		f_unmount(fatfs_param->drv);
		disk_drv->disk_deinitialize();
	}
	fatfs_flash_close(target);
	return ret;
}
