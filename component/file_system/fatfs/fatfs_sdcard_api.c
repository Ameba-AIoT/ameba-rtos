#include "platform_autoconf.h"
#include "vfs.h"
#include "vfs_fatfs.h"
#include "ameba.h"
#if defined(CONFIG_FATFS_SD_MODE) || defined(CONFIG_FATFS_SD_SPI_MODE)

#if defined(CONFIG_FATFS_SD_MODE) && CONFIG_FATFS_SD_MODE
#include "ameba_sd.h"
#endif

extern int fatfs_init_status;

#ifdef CONFIG_FATFS_SD_HOTPLUG
rtos_sema_t fatfs_sd_hotplug_sema;
volatile u8 fatfs_sd_change_status = 0;
u8 fatfs_sd_hotplug_flag = 0;

static void fatfs_sd_intr_cb(SD_RESULT res)
{
	int status;

	if (fatfs_init_status != res - 1) {
		fatfs_sd_change_status = res;
		rtos_sema_give(fatfs_sd_hotplug_sema);
	}

	if (fatfs_hostplug_usr_cb) {
		status = (res == SD_NODISK ? HOTPULG_OUT : HOTPULG_IN);
		fatfs_hostplug_usr_cb(status);
	}
}

static void fatfs_sd_hotplug_thread(void *param)
{
	UNUSED(param);

	while (1) {
		if (rtos_sema_take(fatfs_sd_hotplug_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (fatfs_sd_change_status == SD_NODISK) {
				SD_disk_Driver.disk_deinitialize();
			} else if (fatfs_sd_change_status == SD_INSERT) {
				SD_disk_Driver.disk_initialize();
			}
		}
	}
}

static void fatfs_sd_hotplug_enable(int interface)
{
	int ret = 0;

	if (interface == VFS_INF_SD && fatfs_sd_hotplug_flag == 0) {
		ret |= rtos_sema_create(&fatfs_sd_hotplug_sema, 0U, 1U);
		ret |= rtos_task_create(NULL, "fatfs_sd_hotplug_thread", fatfs_sd_hotplug_thread, NULL, 1024U, 8);
		if (ret) {
			VFS_DBG(VFS_ERROR, "FATFS enable sd hotplug failed.");
		} else {
			SD_SetCdCallback(fatfs_sd_intr_cb);
			fatfs_sd_hotplug_flag = 1;
		}
	}
}

#endif

int fatfs_sd_close(void)
{
	if (fatfs2_mount_flag == 1) {
		if (f_unmount(fatfs_second_flash_param.drv) != FR_OK) {
			VFS_DBG(VFS_ERROR, "FATFS unmount logical drive fail.");
		}
	}

	if (FATFS_UnRegisterDiskDriver(fatfs_second_flash_param.drv_num)) {
		VFS_DBG(VFS_ERROR, "Unregister sd driver from FATFS fail.");
	}

	return 0;
}

int fatfs_sd_init(int interface)
{
	int ret = 0;
	FRESULT res;

#ifdef CONFIG_FATFS_SD_HOTPLUG
	fatfs_sd_hotplug_enable(interface);
#endif

	if (fatfs2_mount_flag != 1) {
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
			ret = -1;
			goto fatfs_init_err;
		} else {
			fatfs_second_flash_param.drv[0] = fatfs_second_flash_param.drv_num + '0';
			fatfs_second_flash_param.drv[1] = ':';
			fatfs_second_flash_param.drv[2] = '/';
			fatfs_second_flash_param.drv[3] = 0;
		}

		res = f_mount(&fatfs_second_flash_param.fs, fatfs_second_flash_param.drv, 1);
		if (res) {
			VFS_DBG(VFS_ERROR, "f_mount error here, please insert SD card or use f_mkfs to format SD card to FAT32/EXFAT");
#ifdef CONFIG_FATFS_SD_HOTPLUG
			if (f_mount(&fatfs_second_flash_param.fs, fatfs_second_flash_param.drv, 0) != FR_OK) {
				VFS_DBG(VFS_ERROR, "FATFS mount logical drive on sd card fail.");
				ret = -2;
				goto fatfs_init_err;
			}
#else
			ret = -2;
			goto fatfs_init_err;
#endif
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
