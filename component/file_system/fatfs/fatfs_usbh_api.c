#include "platform_autoconf.h"
#include "vfs.h"
#include "vfs_fatfs.h"
#include "ameba.h"
#ifdef CONFIG_FATFS_USB_HOST

extern ll_diskio_drv USB_disk_Driver;

int fatfs_usbh_close(void)
{
	if (fatfs2_mount_flag == 1) {
		if (f_unmount(fatfs_second_flash_param.drv) != FR_OK) {
			VFS_DBG(VFS_ERROR, "FATFS unmount logical drive fail.");
		}
	}

	if (FATFS_UnRegisterDiskDriver(fatfs_second_flash_param.drv_num)) {
		VFS_DBG(VFS_ERROR, "Unregister usbh driver from FATFS fail.");
	}

	return 0;
}

int fatfs_usbh_init(void)
{
	int ret = 0;

	if (fatfs2_mount_flag != 1) {
		int Fatfs_ok = 0;
		FRESULT res;
		// Register disk driver to Fatfs
		VFS_DBG(VFS_INFO, "Register usbh driver to Fatfs.");

		fatfs_second_flash_param.drv_num = FATFS_RegisterDiskDriver(&USB_disk_Driver);

		if (fatfs_second_flash_param.drv_num < 0) {
			VFS_DBG(VFS_ERROR, "Rigester usbh driver to FATFS fail.");
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
			VFS_DBG(VFS_ERROR, "f_mount error here, please insert usb device or format usb device to FAT32/exfat");
			//res = f_mkfs(fatfs_second_flash_param.drv,0,0);
			if (f_mount(&fatfs_second_flash_param.fs, fatfs_second_flash_param.drv, 0) != FR_OK) {
				VFS_DBG(VFS_ERROR, "FATFS mount logical drive on usb device fail.");
				ret = -2;
				goto fatfs_init_err;
			}
		}
	} else {
		VFS_DBG(VFS_INFO, "FATFS usbh already initialized\r\n");
	}
	return 0;

fatfs_init_err:
	fatfs_usbh_close();
	return ret;
}

#endif
