/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "os_wrapper.h"
#include "usbd.h"
#include "usbh.h"
#include "usbd_msc.h"
#include "usbh_msc.h"
#include "vfs_fatfs.h"
#include "ff.h"

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "DRD";

// USB speed
#ifdef CONFIG_USB_FS
#error	"USB DRD example only supports USB high speed"
#endif

#define USB_DRD_SPEED							USB_SPEED_HIGH

// Thread priorities
#define USB_DRD_INIT_THREAD_PRIORITY			5U
#define USBH_MSC_THREAD_STACK_SIZE				(1024*8)
#define USBH_MSC_TEST_BUF_SIZE					4096
#define USBH_MSC_TEST_ROUNDS					20
#define USBH_MSC_TEST_SEED						0xA5

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void usbd_msc_cb_status_changed(u8 status);
static int usbh_msc_cb_attach(void);
static int usbh_msc_cb_setup(void);
static int usbh_msc_cb_process(usb_host_t *host, u8 id);

/* Private variables ---------------------------------------------------------*/

static usbd_config_t usbd_msc_cfg = {
	.speed = USB_DRD_SPEED,
	.dma_enable = 1U,
	.isr_priority = INT_PRI_MIDDLE,
	.nptx_max_err_cnt = {0U, 2000U, },
#if defined (CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 708U,
	.ptx_fifo_depth = {16U, 256U, },
#endif
};

static usbd_msc_cb_t usbd_msc_cb = {
	.status_changed = usbd_msc_cb_status_changed
};

static rtos_sema_t usbh_msc_attach_sema;
static __IO int usbh_msc_is_ready = 0;

static usbh_config_t usbh_cfg = {
	.pipes = 5U,
	.speed = USB_DRD_SPEED,
	.dma_enable = FALSE,
	.main_task_priority = 3U,
	.isr_task_priority  = 4U,
};

static usbh_msc_cb_t usbh_msc_usr_cb = {
	.attach = usbh_msc_cb_attach,
	.setup = usbh_msc_cb_setup,
};

static usbh_user_cb_t usbh_usr_cb = {
	.process = usbh_msc_cb_process
};

/* Private functions ---------------------------------------------------------*/

static void usbd_msc_cb_status_changed(u8 status)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Device status change: %d\n", status);
}

static int usbh_msc_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Host attach\n");
	rtos_sema_give(usbh_msc_attach_sema);
	return HAL_OK;
}

static int usbh_msc_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Host setup\n");
	usbh_msc_is_ready = 1;
	return HAL_OK;
}

static int usbh_msc_cb_process(usb_host_t *host, u8 id)
{
	UNUSED(host);

	switch (id) {
	case USBH_MSG_DISCONNECTED:
		usbh_msc_is_ready = 0;
		break;
	case USBH_MSG_CONNECTED:
		break;
	default:
		break;
	}

	return HAL_OK;
}

void example_usb_drd_thread(void *param)
{
	FATFS fs;
	FIL f;
	int drv_num = 0;
	FRESULT res;
	char logical_drv[4];
	char path[64];
	int ret = 0;
	u32 filenum = 0;
	u32 br;
	u32 bw;
	u32 round = 0;
	u32 start;
	u32 elapse;
	u32 perf = 0;
	u32 test_sizes[] = {512, 1024, 2048, 4096, 8192};
	u32 test_size;
	u32 i;
	u8 *buf = NULL;

	UNUSED(param);

	RTK_LOGS(TAG, RTK_LOG_INFO, "USB DRD demo start\n");

	RTK_LOGS(TAG, RTK_LOG_INFO, "Init device driver\n");
	ret = usbd_init(&usbd_msc_cfg);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init device driver\n");
		goto exit;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Init MSC device class\n");
	ret = usbd_msc_init(&usbd_msc_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init MSC device class\n");
		usbd_deinit();
		goto exit;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "MSC device session start\n");

	rtos_time_delay_ms(20000);

	RTK_LOGS(TAG, RTK_LOG_INFO, "Role switch\n");

	RTK_LOGS(TAG, RTK_LOG_INFO, "Deinit MSC device class\n");
	usbh_msc_deinit();

	RTK_LOGS(TAG, RTK_LOG_INFO, "Deinit device driver\n");
	usbd_deinit();

	rtos_sema_create(&usbh_msc_attach_sema, 0U, 1U);

	buf = (u8 *)rtos_mem_zmalloc(USBH_MSC_TEST_BUF_SIZE);
	if (buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to alloc test buf\n");
		goto exit_usbh_sema;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Init host driver\n");
	ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init host driver\n");
		goto exit_usbh_malloc;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Init MSC host driver\n");
	ret = usbh_msc_init(&usbh_msc_usr_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init MSC host driver\n");
		goto exit_usbh;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "MSC host session start\n");

	// Register USB disk driver to fatfs
	RTK_LOGS(TAG, RTK_LOG_INFO, "Register USB disk\n");
	drv_num = FATFS_RegisterDiskDriver(&USB_disk_Driver);
	if (drv_num < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to register\n");
		goto exit_usbh_msc;
	}

	logical_drv[0] = drv_num + '0';
	logical_drv[1] = ':';
	logical_drv[2] = '/';
	logical_drv[3] = 0;

	RTK_LOGS(TAG, RTK_LOG_INFO, "FatFS USB W/R performance test start...\n");

	while (1) {
		if (usbh_msc_is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}

	if (f_mount(&fs, logical_drv, 1) != FR_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to mount logical drive\n");
		goto exit_usbh_fatfs;
	}

	strcpy(path, logical_drv);

	while (1) {
		if (rtos_sema_take(usbh_msc_attach_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to take sema\n");
			continue;
		}

		while (1) {
			if (usbh_msc_is_ready) {
				rtos_time_delay_ms(10);
				break;
			}
		}

		sprintf(&path[3], "TEST%ld.DAT", filenum);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Open file: %s\n", path);
		// open test file
		res = f_open(&f, path, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
		if (res) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to open file: TEST%d.DAT\n", filenum);
			goto exit_usbh_fatfs_mount;
		}
		// clean write and read buffer
		memset(buf, USBH_MSC_TEST_SEED, USBH_MSC_TEST_BUF_SIZE);

		for (i = 0; i < sizeof(test_sizes) / sizeof(test_sizes[0]); ++i) {
			test_size = test_sizes[i];
			if (test_size > USBH_MSC_TEST_BUF_SIZE) {
				break;
			}

			RTK_LOGS(TAG, RTK_LOG_INFO, "W test: size %d, round %d...\n", test_size, USBH_MSC_TEST_ROUNDS);
			start = SYSTIMER_TickGet();

			for (round = 0; round < USBH_MSC_TEST_ROUNDS; ++round) {
				res = f_write(&f, (void *)buf, test_size, (UINT *)&bw);
				if (res || (bw < test_size)) {
					f_lseek(&f, 0);
					RTK_LOGS(TAG, RTK_LOG_ERROR, "W err bw=%d, rc=%d\n", bw, res);
					ret = 1;
					break;
				}
			}

			elapse = SYSTIMER_GetPassTime(start);
			perf = (round * test_size * 10000 / 1024) / elapse;
			RTK_LOGS(TAG, RTK_LOG_INFO, "W rate %d.%d KB/s for %d round @ %d ms\n", perf / 10, perf % 10, round, elapse);

			/* move the file pointer to the file head*/
			res = f_lseek(&f, 0);

			RTK_LOGS(TAG, RTK_LOG_INFO, "R test: size = %d round = %d...\n", test_size, USBH_MSC_TEST_ROUNDS);
			start = SYSTIMER_TickGet();

			for (round = 0; round < USBH_MSC_TEST_ROUNDS; ++round) {
				res = f_read(&f, (void *)buf, test_size, (UINT *)&br);
				if (res || (br < test_size)) {
					f_lseek(&f, 0);
					RTK_LOGS(TAG, RTK_LOG_ERROR, "R err br=%d, rc=%d\n", br, res);
					ret = 1;
					break;
				}
			}

			elapse = SYSTIMER_GetPassTime(start);
			perf = (round * test_size * 10000 / 1024) / elapse;
			RTK_LOGS(TAG, RTK_LOG_INFO, "R rate %d.%d KB/s for %d round @ %d ms\n", perf / 10, perf % 10, round, elapse);

			/* move the file pointer to the file head*/
			res = f_lseek(&f, 0);
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "FatFS USB W/R performance test %s\n", (ret == 0) ? "done" : "abort");

		// close source file
		res = f_close(&f);
		if (res) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "File close fail\n");
			ret = 1;
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "File close OK\n");
		}

		if (!ret) {
			filenum++;
		}
		ret = 0;
	}

exit_usbh_fatfs_mount:
	if (f_unmount(logical_drv) != FR_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to unmount logical drive\n");
	}
exit_usbh_fatfs:
	if (FATFS_UnRegisterDiskDriver(drv_num)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to unregister disk driver from FATFS\n");
	}
exit_usbh_msc:
	usbh_msc_deinit();
exit_usbh:
	usbh_deinit();
exit_usbh_malloc:
	rtos_mem_free(buf);
exit_usbh_sema:
	rtos_sema_delete(usbh_msc_attach_sema);
exit:
	RTK_LOGS(TAG, RTK_LOG_INFO, "Test aborted\n");
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usb_drd(void)
{
	int ret;
	rtos_task_t task;

	ret = rtos_task_create(&task, "example_usb_drd_thread", example_usb_drd_thread, NULL, 2048, USB_DRD_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USB DRD thread fail\n");
	}
}

