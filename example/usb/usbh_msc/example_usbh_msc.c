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

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "MSC";

/* Hotplug switch
 *   0 = replug loop:  test files -> stop -> wait for replug -> repeat
 *                      USB stack stays alive; reconnect is handled by the
 *                      core without tearing down the stack
 *   1 = continuous:   test files -> repeat immediately; on detach the
 *                      stack is fully torn down and rebuilt to guarantee
 *                      clean transfer state across plug/unplug cycles
 */
#define CONFIG_USBH_MSC_HOTPLUG              0

// Thread priorities
#define USBH_MSC_INIT_THREAD_PRIORITY        2
#define USBH_MSC_MAIN_TASK_PRIORITY          3U
#define USBH_MSC_HOTPLUG_THREAD_PRIORITY     2

// Thread stack sizes
#define USBH_MSC_INIT_THREAD_STACK_SIZE      11400U
#define USBH_MSC_MAIN_TASK_STACK_SIZE        768U
#define USBH_MSC_HOTPLUG_THREAD_STACK_SIZE   1024U

#define USBH_MSC_TEST_BUF_SIZE               4096
#define USBH_MSC_TEST_ROUNDS                 20
#define USBH_MSC_CHECK_DATA                  0
#define USBH_MSC_MAX_FILES                   10

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int msc_cb_attach(void);
static int msc_cb_detach(void);
static int msc_cb_setup(void);
static int msc_cb_process(usb_host_t *host, u8 msg);

/* Private variables ---------------------------------------------------------*/

static rtos_sema_t msc_attach_sema;
static __IO int msc_is_connected = 0;
static __IO int msc_is_ready = 0;
static u32 filenum = 0;
static u8 *msc_wt_buf;
static u8 *msc_rd_buf;

#if CONFIG_USBH_MSC_HOTPLUG
static rtos_sema_t msc_detach_sema;
#endif

static const usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = USBH_MSC_MAIN_TASK_STACK_SIZE,
	.main_task_priority = USBH_MSC_MAIN_TASK_PRIORITY,
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
#elif defined (CONFIG_AMEBAPRO3)
	/*FIFO total depth is 2232 DWORD, resv 8 DWORD for DMA addr */
	.rx_fifo_depth = 1712,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#endif
};

static const usbh_msc_cb_t msc_usr_cb = {
	.attach = msc_cb_attach,
	.detach = msc_cb_detach,
	.setup = msc_cb_setup,
};

static const usbh_user_cb_t usbh_usr_cb = {
	.process = msc_cb_process
};

/* Private functions ---------------------------------------------------------*/

static int msc_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
	msc_is_connected = 1;
	rtos_sema_give(msc_attach_sema);
	return HAL_OK;
}

static int msc_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
#if CONFIG_USBH_MSC_HOTPLUG
	rtos_sema_give(msc_detach_sema);
#endif
	return HAL_OK;
}

static int msc_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
	msc_is_ready = 1;
	return HAL_OK;
}

static int msc_cb_process(usb_host_t *host, u8 msg)
{
	UNUSED(host);

	switch (msg) {
	case USBH_MSG_DISCONNECTED:
		msc_is_connected = 0;
		msc_is_ready = 0;
		break;
	case USBH_MSG_CONNECTED:
		break;
	default:
		break;
	}

	return HAL_OK;
}

/* I/O test routine (10 files, each with W/R of multiple sizes) */
static int msc_run_io_test(void)
{
	FATFS fs;
	FIL f;
	int drv_num = -1;
	FRESULT res;
	char logical_drv[4];
	char path[64] = {0};
	int ret = HAL_OK;
	u32 br;
	u32 bw;
	u32 round = 0;
	u32 start;
	u32 elapse;
	u32 perf = 0;
	u32 test_sizes[] = {512, 1024, 2048, 4096, 8192};
	u32 test_size;
	u32 i;
	u8 data;

	/* Wait for device attach + MSC ready */
	RTK_LOGS(TAG, RTK_LOG_INFO, "Wait for device attach...\n");
	if (rtos_sema_take(msc_attach_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Take attach sema fail\n");
		return HAL_ERR_UNKNOWN;
	}

	while (!msc_is_ready) {
		if (!msc_is_connected) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Device disconnected before ready\n");
			return HAL_ERR_UNKNOWN;
		}
		rtos_time_delay_ms(10);
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Device ready\n");

	/* Register disk driver and mount */
	drv_num = FATFS_RegisterDiskDriver(&USB_disk_Driver);
	if (drv_num < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to register disk driver\n");
		return HAL_ERR_UNKNOWN;
	}

	logical_drv[0] = drv_num + '0';
	logical_drv[1] = ':';
	logical_drv[2] = '/';
	logical_drv[3] = 0;
	strcpy(path, logical_drv);

	if (f_mount(&fs, logical_drv, 1) != FR_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to mount logical drive\n");
		FATFS_UnRegisterDiskDriver(drv_num);
		return HAL_ERR_UNKNOWN;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%08x\n", rtos_mem_get_free_heap_size());

	/* Write / read up to USBH_MSC_MAX_FILES files */
	for (filenum = 0; filenum < USBH_MSC_MAX_FILES; filenum++) {
#if CONFIG_USBH_MSC_HOTPLUG
		if (!msc_is_ready) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Device detached during test\n");
			ret = HAL_ERR_UNKNOWN;
			break;
		}
#endif
		sprintf(&path[3], "TEST%ld.DAT", filenum);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Open file: %s\n", path);

		res = f_open(&f, path, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
		if (res) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Open file fail: %s, rc=%d\n", path, res);
			ret = HAL_ERR_UNKNOWN;
			break;
		}

		data = _rand() % 0xFF;
		memset(msc_wt_buf, data, USBH_MSC_TEST_BUF_SIZE);

		for (i = 0; i < sizeof(test_sizes) / sizeof(test_sizes[0]); ++i) {
			test_size = test_sizes[i];
			if (test_size > USBH_MSC_TEST_BUF_SIZE) {
				break;
			}

			/* Write */
			RTK_LOGS(TAG, RTK_LOG_INFO, "W test: size %d, round %d...\n", test_size, USBH_MSC_TEST_ROUNDS);
			start = SYSTIMER_TickGet();
			for (round = 0; round < USBH_MSC_TEST_ROUNDS; ++round) {
				res = f_write(&f, (void *)msc_wt_buf, test_size, (UINT *)&bw);
#if CONFIG_USBH_MSC_HOTPLUG
				if (res == FR_DISK_ERR) {
					RTK_LOGS(TAG, RTK_LOG_WARN, "W err: device disconnected (rc=%d)\n", res);
					break;
				}
#endif
				if (res || (bw < test_size)) {
					f_lseek(&f, 0);
					RTK_LOGS(TAG, RTK_LOG_ERROR, "W err bw=%d, rc=%d\n", bw, res);
					ret = HAL_ERR_UNKNOWN;
					break;
				}
			}
#if CONFIG_USBH_MSC_HOTPLUG
			if (res == FR_DISK_ERR) {
				break;
			}
#endif
			if (ret) {
				break;
			}

			elapse = SYSTIMER_GetPassTime(start);
			perf = (round * test_size * 10000 / 1024) / elapse;
			RTK_LOGS(TAG, RTK_LOG_INFO, "W rate %d.%d KB/s for %d round @ %d ms\n", perf / 10, perf % 10, round, elapse);

			/* Read */
			f_lseek(&f, 0);
			RTK_LOGS(TAG, RTK_LOG_INFO, "R test: size = %d round = %d...\n", test_size, USBH_MSC_TEST_ROUNDS);
			start = SYSTIMER_TickGet();
			for (round = 0; round < USBH_MSC_TEST_ROUNDS; ++round) {
				res = f_read(&f, (void *)msc_rd_buf, test_size, (UINT *)&br);
#if CONFIG_USBH_MSC_HOTPLUG
				if (res == FR_DISK_ERR) {
					RTK_LOGS(TAG, RTK_LOG_WARN, "R err: device disconnected (rc=%d)\n", res);
					break;
				}
#endif
				if (res || (br < test_size)) {
					f_lseek(&f, 0);
					RTK_LOGS(TAG, RTK_LOG_ERROR, "R err br=%d, rc=%d\n", br, res);
					ret = HAL_ERR_UNKNOWN;
					break;
				}
			}
#if CONFIG_USBH_MSC_HOTPLUG
			if (res == FR_DISK_ERR) {
				break;
			}
#endif
			if (ret) {
				break;
			}

			elapse = SYSTIMER_GetPassTime(start);
			perf = (round * test_size * 10000 / 1024) / elapse;
			RTK_LOGS(TAG, RTK_LOG_INFO, "R rate %d.%d KB/s for %d round @ %d ms\n", perf / 10, perf % 10, round, elapse);
			f_lseek(&f, 0);

#if USBH_MSC_CHECK_DATA
			if (!(memcmp(msc_wt_buf, msc_rd_buf, test_size) == 0)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "WR%d check err: %d-%d-%d-%d vs %d-%d-%d-%d\n", test_size,
						 msc_wt_buf[0], msc_wt_buf[1], msc_wt_buf[test_size - 2], msc_wt_buf[test_size - 1],
						 msc_rd_buf[0], msc_rd_buf[1], msc_rd_buf[test_size - 2], msc_rd_buf[test_size - 1]);
				ret = HAL_ERR_UNKNOWN;
				break;
			}
#endif
		}

#if CONFIG_USBH_MSC_HOTPLUG
		if (res == FR_DISK_ERR) {
			f_close(&f);
			ret = HAL_ERR_UNKNOWN;
			break;
		}
#endif

		f_close(&f);
		if (ret) {
			break;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "File %s done\n", path);
		rtos_time_delay_ms(20);
	}

	/* Unmount & unregister */
	if (f_unmount(logical_drv) != FR_OK) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Unmount fail\n");
	}
	if (FATFS_UnRegisterDiskDriver(drv_num)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Unregister disk driver fail\n");
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%08x\n", rtos_mem_get_free_heap_size());
	return ret;
}

#if CONFIG_USBH_MSC_HOTPLUG
/* Hotplug thread: re-init USB stack after detach */
static void example_usbh_msc_hotplug_thread(void *param)
{
	int ret;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(msc_detach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Hotplug: deinit USB stack\n");
			rtos_time_delay_ms(100);

			usbh_msc_deinit();
			rtos_time_delay_ms(20); /* let USB main task flush its queue before deletion */
			usbh_deinit();

			/* Reset state and drain any transient attach event fired during
			 * the USB controller reset (power cycle causes a brief connect). */
			msc_is_connected = 0;
			msc_is_ready = 0;
			rtos_sema_take(msc_attach_sema, 0);

			rtos_time_delay_ms(10);

			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%08x\n", rtos_mem_get_free_heap_size());

			ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug: USBH init fail\n");
				break;
			}

			ret = usbh_msc_init(&msc_usr_cb);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug: MSC init fail\n");
				usbh_deinit();
				break;
			}

			RTK_LOGS(TAG, RTK_LOG_INFO, "Hotplug: USB stack re-initialized\n");
		}
	}

	rtos_task_delete(NULL);
}
#endif

/* Main test thread */
void example_usbh_msc_thread(void *param)
{
	int ret;

	UNUSED(param);

	rtos_sema_create(&msc_attach_sema, 0U, 1U);
#if CONFIG_USBH_MSC_HOTPLUG
	rtos_sema_create(&msc_detach_sema, 0U, 1U);
#endif

	msc_wt_buf = (u8 *)rtos_mem_zmalloc(USBH_MSC_TEST_BUF_SIZE);
	msc_rd_buf = (u8 *)rtos_mem_zmalloc(USBH_MSC_TEST_BUF_SIZE);
	if (!msc_wt_buf || !msc_rd_buf) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to alloc test buf\n");
		goto exit_free;
	}

	/* Init USB host + MSC class driver */
	ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USBH init fail\n");
		goto exit_free;
	}

	ret = usbh_msc_init(&msc_usr_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "MSC init fail\n");
		usbh_deinit();
		goto exit_free;
	}

#if CONFIG_USBH_MSC_HOTPLUG
	/* Start hotplug thread (waits on detach_sema, re-inits USB stack) */
	{
		rtos_task_t hotplug_task;
		ret = rtos_task_create(&hotplug_task, "example_usbh_msc_hotplug_thread",
							   example_usbh_msc_hotplug_thread, NULL,
							   USBH_MSC_HOTPLUG_THREAD_STACK_SIZE,
							   USBH_MSC_HOTPLUG_THREAD_PRIORITY);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\n");
			usbh_msc_deinit();
			usbh_deinit();
			goto exit_free;
		}
	}
#endif

	/* I/O test loop */
#if CONFIG_USBH_MSC_HOTPLUG
	for (;;) {
		ret = msc_run_io_test();
		if (ret == HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "All files done, repeat from 0\n");
			/* Device still attached -- give sema so the next call does not
			 * block at rtos_sema_take waiting for a new attach event. */
			rtos_sema_give(msc_attach_sema);
		} else {
			RTK_LOGS(TAG, RTK_LOG_WARN, "I/O interrupted, wait for next attach\n");
			/* Hotplug thread tears down + rebuilds the stack and gives
			 * attach_sema after enumeration completes. */
		}
	}
#else
	/* Replug loop: test 10 files, then wait for the next physical replug
	 * event before running again.  msc_run_io_test() blocks at
	 * rtos_sema_take(msc_attach_sema) until msc_cb_attach fires. */
	for (;;) {
		ret = msc_run_io_test();
		RTK_LOGS(TAG, RTK_LOG_INFO, "Test %s, unplug and replug to repeat\n",
				 ret == HAL_OK ? "done" : "interrupted");
	}
#endif

	/* Cleanup (unreachable in CONFIG_USBH_MSC_HOTPLUG=1 for the test thread, but keeps
	 * the pattern consistent) */
	usbh_msc_deinit();
	usbh_deinit();

exit_free:
	if (msc_wt_buf) {
		rtos_mem_free(msc_wt_buf);
	}
	if (msc_rd_buf) {
		rtos_mem_free(msc_rd_buf);
	}
	rtos_sema_delete(msc_attach_sema);
#if CONFIG_USBH_MSC_HOTPLUG
	rtos_sema_delete(msc_detach_sema);
#endif
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbh_msc(void)
{
	int ret;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH MSC demo start\n");

	ret = rtos_task_create(&task, "example_usbh_msc_thread", example_usbh_msc_thread, NULL,
						   USBH_MSC_INIT_THREAD_STACK_SIZE, USBH_MSC_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
	}
}
