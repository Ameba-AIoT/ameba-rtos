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
static const char *const TAG = "USBH";

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#define USBH_MSC_TEST_BUF_SIZE      4096
/* Private function prototypes -----------------------------------------------*/

static int msc_cb_attach(void);
static int msc_cb_detach(void);
static int msc_cb_setup(void);
static int msc_cb_process(usb_host_t *host, u8 id);

/* Private variables ---------------------------------------------------------*/

static usb_os_sema_t msc_attach_sema;
static __IO int msc_is_ready = 0;
static u8 *msc_wt_buf;
static u8 *msc_rd_buf;

static usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_priority = 3U,
	.sof_tick_enable = 1U,
#if defined (CONFIG_AMEBAGREEN2)
	/*FIFO total depth is 1024, reserve 12 for DMA addr*/
	.rx_fifo_depth = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#endif
};

static usbh_msc_cb_t msc_usr_cb = {
	.attach = msc_cb_attach,
	.detach = msc_cb_detach,
	.setup = msc_cb_setup,
};

static usbh_user_cb_t usbh_usr_cb = {
	.process = msc_cb_process
};

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
	char path[64] = {'0'};
	int ret = 0;
	u32 br;
	u32 bw;
	u32 test_sizes[] = {512, 1024, 2048, 4096, 8192};
	u32 test_size;
	u8 data;
	u32 i;

	UNUSED(param);

	usb_os_sema_create(&msc_attach_sema);
	msc_wt_buf = (u8 *)rtos_mem_zmalloc(USBH_MSC_TEST_BUF_SIZE);
	if (msc_wt_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to alloc test buf\n");
		goto exit;
	}

	msc_rd_buf = (u8 *)rtos_mem_zmalloc(USBH_MSC_TEST_BUF_SIZE);
	if (msc_rd_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to alloc test buf\n");
		goto exit_free;
	}

	ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init USB\n");
		goto exit_free;
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "FatFS USB W/R test start\n");
next_file:
		sprintf(&path[3], "TEST%ld.DAT", filenum);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Open test file '%s'\n", path);

		/* open test file */
		res = f_open(&f, path, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
		if (res) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to open file: TEST%d.DAT\n", filenum);
			goto exit_unmount;
		}

		/* change write data */
		data = _rand() % 0xFF;
		memset(msc_wt_buf, data, USBH_MSC_TEST_BUF_SIZE);

		for (i = 0; i < sizeof(test_sizes) / sizeof(test_sizes[0]); ++i) {
			test_size = test_sizes[i];
			if (test_size > USBH_MSC_TEST_BUF_SIZE) {
				break;
			}
			res = f_write(&f, (void *)msc_wt_buf, test_size, (UINT *)&bw);
			if (res || (bw < test_size)) {
				f_lseek(&f, 0);
				RTK_LOGS(TAG, RTK_LOG_ERROR, "W err bw=%d, rc=%d\n", bw, res);
				ret = HAL_ERR_HW;
				break;
			}

			/* move the file pointer to the file head*/
			res = f_lseek(&f, 0);

			res = f_read(&f, (void *)msc_rd_buf, test_size, (UINT *)&br);
			if (res || (br < test_size)) {
				f_lseek(&f, 0);
				RTK_LOGS(TAG, RTK_LOG_ERROR, "R err br=%d, rc=%d\n", br, res);
				ret = HAL_ERR_HW;
				break;
			}

			/* move the file pointer to the file head*/
			res = f_lseek(&f, 0);

			/* Check TRX data*/
			if (!(memcmp(msc_wt_buf, msc_rd_buf, test_size) == 0)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "W-R check err: %x-%x-%x-%x vs %x-%x-%x-%x\n",
						 msc_wt_buf[0], msc_wt_buf[1], msc_wt_buf[test_size - 2], msc_wt_buf[test_size - 1],
						 msc_rd_buf[0], msc_rd_buf[1], msc_rd_buf[test_size - 2], msc_rd_buf[test_size - 1]);
				ret = HAL_ERR_HW;
				break;
			}
		}

		/* close source file */
		res = f_close(&f);
		if (res) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to close file: %d\n", res);
			ret = 1;
		} else {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Test file closed\n");
		}

		if (!ret) {
			if (filenum++ > 10) {
				filenum = 0;
			}
			goto next_file;
		} else {
			break;
		}
	}

	RTK_LOGS(TAG, RTK_LOG_ERROR, "Test abort: %d\n", ret);
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
exit_free:
	if (msc_wt_buf) {
		rtos_mem_free(msc_wt_buf);
	}
	if (msc_rd_buf) {
		rtos_mem_free(msc_rd_buf);
	}
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

