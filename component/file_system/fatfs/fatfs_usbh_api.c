#include "platform_autoconf.h"
#include "vfs.h"
#include "vfs_fatfs.h"
#include "ameba.h"
#ifdef CONFIG_FATFS_USB_HOST
#include "usbh_msc.h"

extern ll_diskio_drv USB_disk_Driver;

static int fatfs_msc_cb_attach(void);
static int fatfs_msc_cb_setup(void);
static int fatfs_usbh_cb_process(usb_host_t *host, u8 msg);

static __IO int fatfs_msc_is_ready = 0;

static usbh_config_t fatfs_usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_priority = 3U,
	.tick_source = USBH_SOF_TICK,
#if defined (CONFIG_AMEBAGREEN2)
	/*FIFO total depth is 1024, reserve 12 for DMA addr*/
	.rx_fifo_depth = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBAL2)
	/*FIFO total depth is 1024 DWORD, reserve 11 DWORD for DMA addr*/
	.rx_fifo_depth = 501,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#endif
};

static usbh_msc_cb_t fatfs_msc_usr_cb = {
	.attach = fatfs_msc_cb_attach,
	.setup = fatfs_msc_cb_setup,
};

static usbh_user_cb_t fatfs_usbh_usr_cb = {
	.process = fatfs_usbh_cb_process
};

static int fatfs_msc_cb_attach(void)
{
	VFS_DBG(VFS_INFO, "ATTACH\n");
	return HAL_OK;
}

static int fatfs_msc_cb_setup(void)
{
	fatfs_msc_is_ready = 1;
	if (fatfs_hostplug_usr_cb) {
		fatfs_hostplug_usr_cb(HOTPULG_IN);
	}

	VFS_DBG(VFS_INFO, "SETUP\n");
	return HAL_OK;
}

static int fatfs_usbh_cb_process(usb_host_t *host, u8 msg)
{
	UNUSED(host);

	switch (msg) {
	case USBH_MSG_DISCONNECTED:
		fatfs_msc_is_ready = 0;
		if (fatfs_hostplug_usr_cb) {
			fatfs_hostplug_usr_cb(HOTPULG_OUT);
		}
		break;
	case USBH_MSG_CONNECTED:
		break;
	default:
		break;
	}

	return HAL_OK;
}

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

	usbh_msc_deinit();
	usbh_deinit();

	return 0;
}

int fatfs_usbh_init(void)
{
	int ret = 0;
	FRESULT res;

	ret = usbh_init(&fatfs_usbh_cfg, &fatfs_usbh_usr_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Fail to init USBH\n");
		return -1;
	}

	usbh_msc_init(&fatfs_msc_usr_cb);

	while (1) {
		if (fatfs_msc_is_ready) {
			break;
		}
		rtos_time_delay_ms(10);
	}

	if (fatfs2_mount_flag != 1) {
		// Register disk driver to Fatfs
		VFS_DBG(VFS_INFO, "Register usbh driver to Fatfs.");

		fatfs_second_flash_param.drv_num = FATFS_RegisterDiskDriver(&USB_disk_Driver);

		if (fatfs_second_flash_param.drv_num < 0) {
			VFS_DBG(VFS_ERROR, "Rigester usbh driver to FATFS fail.");
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
			VFS_DBG(VFS_ERROR, "f_mount error here, please insert usb device or format usb device to FAT32/exfat");
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
