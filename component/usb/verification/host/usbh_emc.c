/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "platform_stdlib.h"
#include "basic_types.h"
#include "os_wrapper.h"
#include "ff.h"
#include "vfs_fatfs.h"
#include "usbh_msc.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "EMC";

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int msc_cb_attach(void);
static int msc_cb_detach(void);
static int msc_cb_setup(void);
static int msc_cb_process(usb_host_t *host, u8 id);

/* Private variables ---------------------------------------------------------*/

static usb_os_sema_t msc_attach_sema;
static __IO int msc_is_ready = 0;

static usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.dma_enable = FALSE,
	.main_task_priority = 3U,
	.isr_task_priority = 4U,
};

static usbh_msc_cb_t msc_usr_cb = {
	.attach = msc_cb_attach,
	.detach = msc_cb_detach,
	.setup = msc_cb_setup,
};

static usbh_user_cb_t usbh_usr_cb = {
	.process = msc_cb_process
};

static const u8 msc_wt_buf[] = "1234567890";
static u8 msc_rd_buf[16];

/* Private functions ---------------------------------------------------------*/

static int msc_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "MSC device attached\n");
	usb_os_sema_give(msc_attach_sema);
	return HAL_OK;
}

static int msc_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "MSC device detached\n");
	return HAL_OK;
}

static int msc_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "MSC device ready\n");
	msc_is_ready = 1;
	return HAL_OK;
}

static int msc_cb_process(usb_host_t *host, u8 id)
{
	UNUSED(host);

	switch (id) {
	case USBH_MSG_DISCONNECTED:
		msc_is_ready = 0;
		RTK_LOGS(TAG, RTK_LOG_INFO, "MSC device disconnected\n");
		break;
	case USBH_MSG_CONNECTED:
		RTK_LOGS(TAG, RTK_LOG_INFO, "MSC device connected\n");
		break;
	default:
		break;
	}

	return HAL_OK;
}

static void usbh_emc_test(void *param)
{
	FATFS fs;
	FIL f;
	u32 filenum = 0;
	int drv_num = 0;
	FRESULT res;
	char logical_drv[4];
	char path[64];
	int ret = 0;
	u32 bw;
	u32 round = 0;
	u32 start;
	u32 elapse;
	u32 i;
	u32 len;

	UNUSED(param);

	usb_os_sema_create(&msc_attach_sema);

	ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init USB\n");
		goto exit;
	}

	usbh_msc_init(&msc_usr_cb);

	// Register USB disk driver to fatfs
	RTK_LOGS(TAG, RTK_LOG_INFO, "Register USB disk driver\n");
	drv_num = FATFS_RegisterDiskDriver(&USB_disk_Driver);
	if (drv_num < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to register USB disk driver\n");
		goto exit_deinit;
	}

	logical_drv[0] = drv_num + '0';
	logical_drv[1] = ':';
	logical_drv[2] = '/';
	logical_drv[3] = 0;

	RTK_LOGS(TAG, RTK_LOG_INFO, "EMC test started, please make sure MSC device is attached\n");

	while (1) {
		if (msc_is_ready) {
			usb_os_sleep_ms(10);
			break;
		}
	}

	if (f_mount(&fs, logical_drv, 1) != FR_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to mount logical drive\n");
		goto exit_unregister;
	}

	strcpy(path, logical_drv);

	while (1) {
		usb_os_sema_take(msc_attach_sema, USB_OS_SEMA_TIMEOUT);

		while (1) {
			if (msc_is_ready) {
				usb_os_sleep_ms(10);
				break;
			}
		}

		sprintf(&path[3], "TEST%ld.DAT", filenum);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Open test file '%s'\n", path);

		// open test file
		res = f_open(&f, path, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
		if (res) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to open file: TEST%d.DAT\n", filenum);
			goto exit_unmount;
		}

		start = SYSTIMER_TickGet();
		len = strlen((const char *)msc_wt_buf);
		while (msc_is_ready) {
			++round;

			elapse = SYSTIMER_GetPassTime(start);
			if (elapse >= 1000) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Test round %lu#\n", round);
				start = SYSTIMER_TickGet();
			}

			ret = HAL_OK;
			res = f_lseek(&f, 0);

			do {
				res = f_write(&f, msc_wt_buf, len, &bw);
				if (res) {
					f_lseek(&f, 0);
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Write error bw=%d, rc=%d\n", bw, res);
					ret = HAL_ERR_HW;
					break;
				}
			} while (bw < len);

			if (ret != HAL_OK) {
				break;
			}

			f_sync(&f);

			res = f_lseek(&f, 0);

			do {
				res = f_read(&f, msc_rd_buf, len, &bw);
				if (res) {
					f_lseek(&f, 0);
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Read error bw=%d, rc=%d\n", bw, res);
					ret = HAL_ERR_HW;
					break;
				}
			} while (bw < len);

			if (ret != HAL_OK) {
				break;
			}

			for (i = 0; i < len; i++) {
				if (msc_wt_buf[i] != msc_rd_buf[i]) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Data check error\n");
					ret = HAL_ERR_HW;
					break;
				}
			}

			if (ret != HAL_OK) {
				break;
			}

			usb_os_memset(msc_rd_buf, 0, len);

			usb_os_sleep_ms(1);
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Test aborted, round %d\n", round);

		// close source file
		res = f_close(&f);
		if (res) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to close file: %d\n", res);
			ret = 1;
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Test file closed\n");
		}

		if (!ret) {
			filenum++;
		}
		ret = 0;
	}

exit_unmount:
	if (f_unmount(logical_drv) != FR_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to unmount logical drive\n");
	}
exit_unregister:
	if (FATFS_UnRegisterDiskDriver(drv_num)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to unregister disk driver from FATFS\n");
	}
exit_deinit:
	usbh_msc_deinit();
	usbh_deinit();
exit:
	usb_os_sema_delete(msc_attach_sema);
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

int cmd_usbh_emc_test(void)
{
	int ret;
	usb_os_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USB host EMC test started\n");

	ret = rtos_task_create(&task, "usbh_emc_test", usbh_emc_test, NULL, 1024U * 4, 2U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to create USB host EMC test thread\n");
	}

	return ret;
}

