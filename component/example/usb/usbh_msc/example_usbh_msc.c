/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "platform_stdlib.h"
#include "basic_types.h"
#include "usbh.h"
#include "os_wrapper.h"
#include "ff.h"
#include "vfs_fatfs.h"
#include "usbh_msc.h"

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "MSC";
#define USBH_MSC_THREAD_STACK_SIZE  (1024*8)
#define USBH_MSC_TEST_BUF_SIZE      4096
#define USBH_MSC_TEST_ROUNDS        20
#define USBH_MSC_TEST_SEED          0xA5

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int msc_cb_attach(void);
static int msc_cb_setup(void);
static int msc_cb_process(usb_host_t *host, u8 id);

/* Private variables ---------------------------------------------------------*/

static rtos_sema_t msc_attach_sema;
static __IO int msc_is_ready = 0;
static u32 filenum = 0;

static usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.dma_enable = FALSE,
	.ext_intr_en = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.isr_task_priority  = 4U,
	.main_task_priority = 3U,
	.sof_tick_en = 1U,

#if defined (CONFIG_AMEBAGREEN2)
	/*FIFO total depth is 1024, reserve 12 for DMA addr*/
	.rx_fifo_depth = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBASMARTPLUS)
	/*FIFO total depth is 1280 DWORD, reserve 14 DWORD for DMA addr*/
	.rx_fifo_depth = 754,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBAL2)
	/*FIFO total depth is 1024 DWORD, reserve 11 DWORD for DMA addr*/
	.rx_fifo_depth = 501,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#endif
};

static usbh_msc_cb_t msc_usr_cb = {
	.attach = msc_cb_attach,
	.setup = msc_cb_setup,
};

static usbh_user_cb_t usbh_usr_cb = {
	.process = msc_cb_process
};

/* Private functions ---------------------------------------------------------*/

static int msc_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
	rtos_sema_give(msc_attach_sema);
	return HAL_OK;
}

static int msc_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
	msc_is_ready = 1;
	return HAL_OK;
}

static int msc_cb_process(usb_host_t *host, u8 id)
{
	UNUSED(host);

	switch (id) {
	case USBH_MSG_DISCONNECTED:
		msc_is_ready = 0;
		break;
	case USBH_MSG_CONNECTED:
		break;
	default:
		break;
	}

	return HAL_OK;
}

void example_usbh_msc_thread(void *param)
{
	FATFS fs;
	FIL f;
	int drv_num = 0;
	FRESULT res;
	char logical_drv[4];
	char path[64] = {'0'};
	int ret = 0;
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

	rtos_sema_create(&msc_attach_sema, 0U, 1U);

	buf = (u8 *)rtos_mem_zmalloc(USBH_MSC_TEST_BUF_SIZE);
	if (buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to alloc test buf\n");
		goto exit;
	}

	ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init USBH\n");
		goto exit_free;
	}

	usbh_msc_init(&msc_usr_cb);

	// Register USB disk driver to fatfs
	RTK_LOGS(TAG, RTK_LOG_INFO, "Register USB disk\n");
	drv_num = FATFS_RegisterDiskDriver(&USB_disk_Driver);
	if (drv_num < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to register\n");
		goto exit_deinit;
	}

	logical_drv[0] = drv_num + '0';
	logical_drv[1] = ':';
	logical_drv[2] = '/';
	logical_drv[3] = 0;

	RTK_LOGS(TAG, RTK_LOG_INFO, "FatFS USB W/R performance test start...\n");

	while (1) {
		if (msc_is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}

	if (f_mount(&fs, logical_drv, 1) != FR_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to mount logical drive\n");
		goto exit_unregister;
	}

	strcpy(path, logical_drv);

	while (1) {
		if (rtos_sema_take(msc_attach_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to take sema\n");
			continue;
		}

		while (1) {
			if (msc_is_ready) {
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
			goto exit_unmount;
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
	if (buf) {
		rtos_mem_free(buf);
	}
exit:
	rtos_sema_delete(msc_attach_sema);
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbh_msc(void)
{
	int ret;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH MSC demo start\n");

	ret = rtos_task_create(&task, "example_usbh_msc_thread", example_usbh_msc_thread, NULL, USBH_MSC_THREAD_STACK_SIZE, 2);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
	}
}

