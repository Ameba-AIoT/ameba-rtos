/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include <platform_autoconf.h>
#include "os_wrapper.h"
#include "usbd_msc.h"
#include "usbh_msc.h"
#include "vfs_fatfs.h"
#include "ff.h"
#include "atcmd_service.h"

/* Private defines -----------------------------------------------------------*/

#if defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RLE1509)
#define MSC_BULK_IN_EP                              0x82U
#define MSC_BULK_OUT_EP                             0x02U
#else
#define MSC_BULK_IN_EP                              0x81U
#define MSC_BULK_OUT_EP                             0x02U
#endif

// USB speed
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#error	"USB DRD example only supports USB high speed"
#endif

#define MSC_USB_SPEED                            USB_SPEED_HIGH

// Thread priorities
#define MSC_MAIN_TASK_PRIORITY                  3
#define MSC_XFER_THREAD_PRIORITY                5
#define MSC_XFER_THREAD_STACK_SIZE              11776U

#define USBH_MSC_TEST_BUF_SIZE					4096
#define USBH_MSC_TEST_ROUNDS					20
#define USBH_MSC_CHECK_DATA                     1

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void usbd_msc_cb_status_changed(u8 old_status, u8 status);
static void usbh_msc_cb_attached(void);
static void usbh_msc_cb_setup(void);
static void usbh_msc_cb_process(usb_host_t *host, u8 msg);

static void usbd_msc_cmd_test(u16 argc, char **argv);
static void usbh_msc_cmd_test(u16 argc, char **argv);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "DRD";

static const usbd_config_t usbd_msc_cfg = {
	.info = {
		.prod_str = "Realtek MSC",
	},
	.isr_priority = INT_PRI_MIDDLE,
#if defined(CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 708U,
	.ptx_fifo_depth = {16U, 256U, },
#elif defined(CONFIG_RLE1509)
	.rx_fifo_depth = 672U,
	.ptx_fifo_depth = {16U, 256U, },
#elif defined (CONFIG_AMEBAPRO3)
	/*DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD*/
	.rx_fifo_depth = 1680U,
	.ptx_fifo_depth = {256U, 16U, 16U},
#endif
};

static const usbd_msc_config_t msc_class_cfg = {
	.bulk_in_addr = MSC_BULK_IN_EP,
	.bulk_out_addr = MSC_BULK_OUT_EP,
};

static const usbd_msc_cb_t usbd_msc_cb = {
	.status_changed = usbd_msc_cb_status_changed
};

static rtos_sema_t usbh_msc_attach_sema;
static __IO int usbh_msc_is_rdy = 0;
static u8 usbd_msc_inited = 0;
static u8 usbh_msc_inited = 0;

/* Serializes the shell's `usbh msc deinit` against the FatFs I/O path.
   Every f_read/f_write lands in USB_disk_read/USB_disk_write, which call
   usbh_msc_read/usbh_msc_write; those poll the BOT state machine for the whole
   duration of a sector transfer. Meanwhile usbh_msc_deinit() frees the CBW/CSW
   and bounce buffers that poll loop walks, and usbh_deinit() memset()s the host
   handle. The class API only checks host != NULL and the unit state on entry, so
   under SMP the test thread on core 1 can be deep inside a transfer while the
   shell thread on core 0 frees it. The test thread must be excluded by lock, not
   by core.
   Created on the first `usbh msc init` and never deleted: the R/W test thread can
   not be joined by the shell, so there is no point in time at which this mutex is
   provably unreferenced. usbh_msc_stack_ready is the gate that keeps callers off
   the stack instead. */
static rtos_mutex_t usbh_msc_state_mutex = NULL;

/* 1: usbh_init() and usbh_msc_init() are both done and the class API may be
   reached. Cleared before any teardown, so a non-zero value also implies
   usbh_msc_state_mutex is valid. */
static volatile u8 usbh_msc_stack_ready = 0;

/* Set by the shell BEFORE it takes the lock, so the test thread abandons its run
   between FatFs operations instead of re-taking the lock for its next sector and
   starving the shell out of a teardown it must perform. */
static volatile u8 usbh_msc_teardown = 0;
static u8 file_cnt = 5;
/* 1 while the R/W test thread runs: set by the shell before the thread is created and cleared
   by the thread as it leaves, so a second rw_test is rejected instead of racing the first */
static volatile u8 usbh_msc_trx_running = 0;

static const usbh_config_t usbh_cfg = {
	.speed = MSC_USB_SPEED,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_priority = MSC_MAIN_TASK_PRIORITY,
	.tick_source = USBH_SOF_TICK,
#if defined(CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined(CONFIG_RLE1509)
	.rx_fifo_depth = 464,
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

static const usbh_msc_cb_t usbh_msc_usr_cb = {
	.attached = usbh_msc_cb_attached,
	.setup = usbh_msc_cb_setup,
};

static const usbh_msc_config_t usbh_msc_class_cfg = {
	/* buf_size left 0 takes the class default. */
};

static const usbh_user_cb_t usbh_usr_cb = {
	.process = usbh_msc_cb_process
};

/* Exported variables --------------------------------------------------------*/

const log_item_t usb_drd_cmd_table[] = {
	{"+USBHMSC", usbh_msc_cmd_test},
	{"+USBDMSC", usbd_msc_cmd_test},
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Handle attach status change notifications from the USB stack
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  old_status: Previous attach status
  * @param  status: New attach status
  * @retval None
  */
static void usbd_msc_cb_status_changed(u8 old_status, u8 status)
{
	UNUSED(old_status);
	UNUSED(status);
}

static void usbh_msc_cb_attached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Host attach\n");
	rtos_sema_give(usbh_msc_attach_sema);
}

static void usbh_msc_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Host setup\n");
	usbh_msc_is_rdy = 1;
}

static void usbh_msc_cb_process(usb_host_t *host, u8 msg)
{
	UNUSED(host);

	switch (msg) {
	case USBH_MSG_DISCONNECTED:
	case USBH_MSG_RESET:
	case USBH_MSG_ERROR:
		/* Not ready again until setup: the device is gone, being reset, or given up. */
		usbh_msc_is_rdy = 0;
		break;
	case USBH_MSG_CONNECTED:
		break;
	default:
		break;
	}
}

static void usbd_msc_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+USBDMSC=<command>\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<command>:\tinit: init device msc driver\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<command>:\tdeinit: deinit device msc driver\r\n");
}

static void usbd_msc_cmd_test(u16 argc, char **argv)
{
	int error_no = HAL_OK;
	int ret = HAL_OK;
	const char *cmd;

	if (argc < 2) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid USB argument\n");
		error_no = HAL_ERR_PARA;
		goto end;
	}

	cmd = (const char *)argv[1];

	if (_stricmp(cmd, "init") == 0) {
		if (usbd_msc_inited != 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "MSC device already inited\n");
			error_no = HAL_ERR_PARA;
			goto end;
		}
		ret = usbd_msc_disk_init();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init disk: %d\n", ret);
			error_no = ret;
			goto end;
		}
		ret = usbd_init(&usbd_msc_cfg);
		if (ret != HAL_OK) {
			usbd_msc_disk_deinit();
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init USBD: %d\n", ret);
			error_no = ret;
			goto end;
		}
		ret = usbd_msc_init(&usbd_msc_cb, &msc_class_cfg);
		if (ret != HAL_OK) {
			usbd_deinit();
			usbd_msc_disk_deinit();
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init MSC\n");
			error_no = ret;
			goto end;
		}
		usbd_msc_inited = 1;
		RTK_LOGS(TAG, RTK_LOG_INFO, "MSC device session start\n");
	} else if (_stricmp(cmd, "deinit") == 0) {
		if (usbd_msc_inited == 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "MSC device not inited\n");
			error_no = HAL_ERR_PARA;
			goto end;
		}
		usbd_msc_inited = 0;
		usbd_msc_deinit();
		usbd_deinit();
		usbd_msc_disk_deinit();
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Input cmd err\n");
		error_no = HAL_ERR_PARA;
	}

end:
	if (error_no == HAL_OK) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		usbd_msc_help();
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

/**
  * @brief  Take the stack lock if the MSC class API may be reached right now
  * @note   Gates on usbh_msc_stack_ready BEFORE touching the mutex: the handle is
  *         still NULL before the first `usbh msc init`, and the flag is already
  *         cleared once a teardown is pending. A deinit right after this check is
  *         harmless, the mutex stays valid and the re-check under it catches it.
  * @retval HAL_OK when the lock is held and the stack is usable, HAL_ERR_HW when
  *         the caller must give up without unlocking
  */
static int usbh_msc_lock_stack(void)
{
	if ((usbh_msc_stack_ready == 0U) || (usbh_msc_teardown != 0U) || (usbh_msc_state_mutex == NULL)) {
		return HAL_ERR_HW;
	}

	rtos_mutex_take(usbh_msc_state_mutex, RTOS_MAX_TIMEOUT);

	if ((usbh_msc_stack_ready == 0U) || (usbh_msc_teardown != 0U)) {
		/* Lost the race against a teardown */
		rtos_mutex_give(usbh_msc_state_mutex);
		return HAL_ERR_HW;
	}

	return HAL_OK;
}

/**
  * @brief  Release the lock taken by usbh_msc_lock_stack()
  * @retval None
  */
static void usbh_msc_unlock_stack(void)
{
	rtos_mutex_give(usbh_msc_state_mutex);
}

/*
 * Locked FatFs wrappers.
 *
 * Each of these reaches the MSC class through the disk driver, so each has to be
 * excluded from a concurrent teardown. They wrap one FatFs operation rather than
 * the whole test, because holding the lock for a full multi-file run would stall
 * `usbh msc deinit` for the entire test duration. One operation is the right
 * granularity: it is bounded by the per-transfer timeout inside
 * usbh_msc_read/usbh_msc_write.
 *
 * On a lost gate they return FR_DISK_ERR, which every call site below already
 * treats as a failure, so no new error path is needed.
 *
 * Unmount and close take the lock unconditionally instead of going through
 * usbh_msc_lock_stack(): they also drop FatFs' own bookkeeping (the mount points
 * at a FATFS object on the caller's stack, the FIL holds open-file state), so
 * skipping them on a closed gate would leave FatFs believing the volume is still
 * mounted with a dangling pointer once this thread returns. The lock itself is
 * always valid once created, and holding it means no teardown is in flight.
 */
static FRESULT usbh_msc_fs_mount(FATFS *fs, const TCHAR *path, BYTE opt)
{
	FRESULT res;

	if (usbh_msc_lock_stack() != HAL_OK) {
		return FR_DISK_ERR;
	}
	res = f_mount(fs, path, opt);
	usbh_msc_unlock_stack();

	return res;
}

static FRESULT usbh_msc_fs_unmount(const TCHAR *path)
{
	FRESULT res;

	rtos_mutex_take(usbh_msc_state_mutex, RTOS_MAX_TIMEOUT);
	res = f_unmount(path);
	rtos_mutex_give(usbh_msc_state_mutex);

	return res;
}

static FRESULT usbh_msc_fs_open(FIL *fp, const TCHAR *path, BYTE mode)
{
	FRESULT res;

	if (usbh_msc_lock_stack() != HAL_OK) {
		return FR_DISK_ERR;
	}
	res = f_open(fp, path, mode);
	usbh_msc_unlock_stack();

	return res;
}

static FRESULT usbh_msc_fs_close(FIL *fp)
{
	FRESULT res;

	/* Unconditional lock, see the note above. */
	rtos_mutex_take(usbh_msc_state_mutex, RTOS_MAX_TIMEOUT);
	res = f_close(fp);
	rtos_mutex_give(usbh_msc_state_mutex);

	return res;
}

static FRESULT usbh_msc_fs_lseek(FIL *fp, FSIZE_t ofs)
{
	FRESULT res;

	if (usbh_msc_lock_stack() != HAL_OK) {
		return FR_DISK_ERR;
	}
	res = f_lseek(fp, ofs);
	usbh_msc_unlock_stack();

	return res;
}

static FRESULT usbh_msc_fs_write(FIL *fp, const void *buf, UINT btw, UINT *bw)
{
	FRESULT res;

	if (usbh_msc_lock_stack() != HAL_OK) {
		*bw = 0U;
		return FR_DISK_ERR;
	}
	res = f_write(fp, buf, btw, bw);
	usbh_msc_unlock_stack();

	return res;
}

static FRESULT usbh_msc_fs_read(FIL *fp, void *buf, UINT btr, UINT *br)
{
	FRESULT res;

	if (usbh_msc_lock_stack() != HAL_OK) {
		*br = 0U;
		return FR_DISK_ERR;
	}
	res = f_read(fp, buf, btr, br);
	usbh_msc_unlock_stack();

	return res;
}

static void example_usb_drd_msc_trx_test(void *param)
{
	FATFS fs;
	FIL f;
	int drv_num = 0;
	FRESULT res;
	char logical_drv[4];
	char path[64] = {'0'};
	int ret = 0;
	int filenum = 0;
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
	/* Owned by this thread only, never shared with another rw_test run */
	u8 *msc_wt_buf = NULL;
	u8 *msc_rd_buf = NULL;

	UNUSED(param);

	msc_wt_buf = (u8 *)usb_os_malloc(USBH_MSC_TEST_BUF_SIZE);
	if (msc_wt_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to alloc test buf\n");
		goto exit;
	}

	msc_rd_buf = (u8 *)usb_os_malloc(USBH_MSC_TEST_BUF_SIZE);
	if (msc_rd_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to alloc test buf\n");
		goto exit_free;
	}

	// Register USB disk driver to fatfs
	RTK_LOGS(TAG, RTK_LOG_INFO, "Register USB disk\n");
	drv_num = FATFS_RegisterDiskDriver(&USB_disk_Driver);
	if (drv_num < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to register\n");
		goto exit_free;
	}

	logical_drv[0] = drv_num + '0';
	logical_drv[1] = ':';
	logical_drv[2] = '/';
	logical_drv[3] = 0;

	RTK_LOGS(TAG, RTK_LOG_INFO, "FatFS USB W/R performance test start...\n");

	while (1) {
		if (usbh_msc_is_rdy) {
			break;
		}
		if ((usbh_msc_teardown != 0U) || (usbh_msc_stack_ready == 0U)) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Stack gone before ready\n");
			goto exit_unregister;
		}
		rtos_time_delay_ms(10);
	}

	res = usbh_msc_fs_mount(&fs, logical_drv, 1);
	if (res != FR_OK) {
		/* rc: 1 FR_DISK_ERR, 3 FR_NOT_READY, 13 FR_NO_FILESYSTEM */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to mount logical drive, rc=%d\n", res);
		goto exit_unregister;
	}

	strcpy(path, logical_drv);
	while (1) {
		if (rtos_sema_take(usbh_msc_attach_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to take sema\n");
			goto exit_unmount;
		}

		while (1) {
			if (usbh_msc_is_rdy) {
				break;
			}
			if ((usbh_msc_teardown != 0U) || (usbh_msc_stack_ready == 0U)) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "Stack gone, stop testing\n");
				goto exit_unmount;
			}
			rtos_time_delay_ms(10);
		}

next_file:
		sprintf(&path[3], "TEST%d.DAT", filenum);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Open file: %s\n", path);
		/* Open test file */
		res = usbh_msc_fs_open(&f, path, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
		if (res) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to open file: TEST%d.DAT\n", filenum);
			goto exit_unmount;
		}
#if USBH_MSC_CHECK_DATA
		/* change write data */
		data = _rand() % 0xFF;
		usb_os_memset((void *)msc_wt_buf, data, USBH_MSC_TEST_BUF_SIZE);
#endif

		for (i = 0; i < sizeof(test_sizes) / sizeof(test_sizes[0]); ++i) {
			test_size = test_sizes[i];
			if (test_size > USBH_MSC_TEST_BUF_SIZE) {
				break;
			}

			RTK_LOGS(TAG, RTK_LOG_INFO, "W test: size %d, round %d...\n", test_size, USBH_MSC_TEST_ROUNDS);
			start = SYSTIMER_TickGet();

			for (round = 0; round < USBH_MSC_TEST_ROUNDS; ++round) {
				res = usbh_msc_fs_write(&f, (void *)msc_wt_buf, test_size, (UINT *)&bw);
				if (res || (bw < test_size)) {
					(void)usbh_msc_fs_lseek(&f, 0);
					RTK_LOGS(TAG, RTK_LOG_ERROR, "W err bw=%d, rc=%d\n", bw, res);
					ret = 1;
					break;
				}
			}
			if (ret) {
				break;
			}

			/* A run shorter than the 1 ms timer resolution reads as 0 */
			elapse = SYSTIMER_GetPassTime(start);
			if (elapse == 0U) {
				elapse = 1U;
			}
			perf = (round * test_size * 10000 / 1024) / elapse;
			RTK_LOGS(TAG, RTK_LOG_INFO, "W rate %d.%d KB/s for %d round @ %d ms\n", perf / 10, perf % 10, round, elapse);

			/* Move the file pointer to the file head */
			res = usbh_msc_fs_lseek(&f, 0);

			RTK_LOGS(TAG, RTK_LOG_INFO, "R test: size = %d round = %d...\n", test_size, USBH_MSC_TEST_ROUNDS);
			start = SYSTIMER_TickGet();

			for (round = 0; round < USBH_MSC_TEST_ROUNDS; ++round) {
				res = usbh_msc_fs_read(&f, (void *)msc_rd_buf, test_size, (UINT *)&br);
				if (res || (br < test_size)) {
					(void)usbh_msc_fs_lseek(&f, 0);
					RTK_LOGS(TAG, RTK_LOG_ERROR, "R err br=%d, rc=%d\n", br, res);
					ret = 1;
					break;
				}
			}
			if (ret) {
				break;
			}

			elapse = SYSTIMER_GetPassTime(start);
			if (elapse == 0U) {
				elapse = 1U;
			}
			perf = (round * test_size * 10000 / 1024) / elapse;
			RTK_LOGS(TAG, RTK_LOG_INFO, "R rate %d.%d KB/s for %d round @ %d ms\n", perf / 10, perf % 10, round, elapse);

			/* Move the file pointer to the file head */
			res = usbh_msc_fs_lseek(&f, 0);

#if USBH_MSC_CHECK_DATA
			/* Check TRX data*/
			if (!(memcmp(msc_wt_buf, msc_rd_buf, test_size) == 0)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "WR%d check err: %x-%x-%x-%x vs %x-%x-%x-%x\n", test_size,
						 msc_wt_buf[0], msc_wt_buf[1], msc_wt_buf[test_size - 2], msc_wt_buf[test_size - 1],
						 msc_rd_buf[0], msc_rd_buf[1], msc_rd_buf[test_size - 2], msc_rd_buf[test_size - 1]);
				ret = HAL_ERR_HW;
				break;
			}
#endif
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "FatFS USB W/R performance test %s\n", (ret == 0) ? "done" : "abort");

		/* Close source file */
		res = usbh_msc_fs_close(&f);
		if (res) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "File close fail\n");
			ret = 1;
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "File close OK\n");
		}

		if ((!ret) && (++filenum < file_cnt)) {
			goto next_file;
		} else {
			break;
		}
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Test %d over: %d\n", filenum, ret);
exit_unmount:
	if (usbh_msc_fs_unmount(logical_drv) != FR_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to unmount logical drive\n");
	}
exit_unregister:
	if (FATFS_UnRegisterDiskDriver(drv_num)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to unregister disk driver from FATFS\n");
	}
exit_free:
	usb_os_mfree((void *)msc_rd_buf);
	usb_os_mfree((void *)msc_wt_buf);
exit:
	usbh_msc_trx_running = 0U;
	rtos_task_delete(NULL);
}

static void usbh_msc_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "AT+USBHMSC=<command>[,<file_cnt>]\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<command>:\tinit: init host msc driver\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<command>:\tdeinit: deinit host msc driver\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<command>:\trw_test: file read and write test\r\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t<file_cnt>:\trw test file cnt 1~255, default: 5\r\n");
}

static void usbh_msc_cmd_test(u16 argc, char **argv)
{
	int error_no = HAL_OK;
	int ret = HAL_OK;
	const char *cmd;
	usb_os_task_t task;
	u32 cnt;

	if (argc < 2) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid USB argument\n");
		error_no = HAL_ERR_PARA;
		goto end;
	}

	cmd = (const char *)argv[1];

	if (_stricmp(cmd, "init") == 0) {
		if (usbh_msc_inited != 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "MSC host already inited\n");
			error_no = HAL_ERR_PARA;
			goto end;
		}
		/* Created on the first init and never deleted, see the declaration */
		if (usbh_msc_state_mutex == NULL) {
			ret = rtos_mutex_create(&usbh_msc_state_mutex);
			if (ret != RTK_SUCCESS) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to create state mutex\n");
				error_no = HAL_ERR_MEM;
				goto end;
			}
		}

		/* Kept across a deinit/init cycle for the same reason it is not deleted
		   there: a blocked waiter may still reference it. Drain any count a prior
		   session left behind so this session does not start out already signalled. */
		if (usbh_msc_attach_sema == NULL) {
			ret = rtos_sema_create(&usbh_msc_attach_sema, 0U, 1U);
			if (ret != RTK_SUCCESS) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to create sema\n");
				error_no = HAL_ERR_MEM;
				goto end;
			}
		} else {
			while (rtos_sema_take(usbh_msc_attach_sema, 0) == RTK_SUCCESS) {
			}
		}

		ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init USBH: %d\n", ret);
			error_no = ret;
			goto end;
		}
		ret = usbh_msc_init(&usbh_msc_usr_cb, &usbh_msc_class_cfg);
		if (ret != HAL_OK) {
			usbh_deinit();
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init MSC: %d\n", ret);
			error_no = ret;
			goto end;
		}
		/* All class drivers registered; start USB TRX so enumeration can run. */
		usbh_start();

		/* Open the gate as the very last step, so the R/W test thread never runs
		   against a half built stack. Clear teardown only after ready is set, or a
		   caller could slip through seeing neither flag. */
		usbh_msc_stack_ready = 1U;
		usbh_msc_teardown = 0U;

		usbh_msc_inited = 1;
		RTK_LOGS(TAG, RTK_LOG_INFO, "MSC host session start\n");
	} else if (_stricmp(cmd, "deinit") == 0) {
		if (usbh_msc_inited == 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "MSC host not inited\n");
			error_no = HAL_ERR_PARA;
			goto end;
		}
		usbh_msc_inited = 0;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Deinit MSC host driver\n");

		/* Announce the teardown BEFORE contending for the lock, so the R/W test
		   thread abandons its run between FatFs operations instead of re-taking the
		   lock for its next sector and starving this command out. */
		usbh_msc_teardown = 1U;

		rtos_mutex_take(usbh_msc_state_mutex, RTOS_MAX_TIMEOUT);
		/* Close the gate under the lock: no FatFs operation is in flight here, and
		   any later one gives up on the cleared flag. */
		usbh_msc_stack_ready = 0U;
		usbh_msc_is_rdy = 0;

		usbh_stop();
		usbh_msc_deinit();
		usbh_deinit();
		rtos_mutex_give(usbh_msc_state_mutex);

		/* The sema is deliberately NOT deleted here. The R/W test thread may be
		   blocked in rtos_sema_take() on it right now, and it cannot be joined from
		   the shell, so freeing it would pull the object out from under a waiter.
		   Give it instead: the thread wakes, sees usbh_msc_is_rdy cleared and the
		   gate shut, and unwinds on its own. The handle is reused by the next
		   `init` (which only creates it when still NULL), so nothing accumulates. */
		rtos_sema_give(usbh_msc_attach_sema);
	} else if (_stricmp(cmd, "rw_test") == 0) {
		if (usbh_msc_inited == 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "MSC host not inited\n");
			error_no = HAL_ERR_PARA;
			goto end;
		}
		/* The shell is the only caller, so the check and the set below can not race */
		if (usbh_msc_trx_running != 0U) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "R&W test already running\n");
			error_no = HAL_ERR_PARA;
			goto end;
		}
		if ((argc > 2) && (argv[2] != NULL)) {
			cnt = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
			if ((cnt == 0U) || (cnt > 0xFFU)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid file_cnt %u, 1~255\n", cnt);
				error_no = HAL_ERR_PARA;
				goto end;
			}
			file_cnt = (u8)cnt;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "USB host MSC R&W test started\n");

		usbh_msc_trx_running = 1U;
		ret = rtos_task_create(&task, "usb_drd_msc_trx_thread", example_usb_drd_msc_trx_test, NULL,
							   MSC_XFER_THREAD_STACK_SIZE, MSC_XFER_THREAD_PRIORITY);
		if (ret != RTK_SUCCESS) {
			usbh_msc_trx_running = 0U;
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to create USB host MSC R&W test thread\n");
			error_no = HAL_ERR_MEM;
		}
		/* The R/W thread is deliberately left unaffined: it is excluded from a
		   teardown by usbh_msc_state_mutex, not by core.
		   No rtos_task_set_affinity() anywhere in this example: the teardown runs on
		   the AT command thread, which this example does not create and so must not
		   re-pin - doing so would move every other AT command onto CPU0 as a side
		   effect. The lock is what makes the teardown safe here; the usual "pin the
		   deinit thread to the ISR's core" trick is unavailable by construction. */
	} else {
		error_no = HAL_ERR_PARA;
	}
end:
	if (error_no == HAL_OK) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		usbh_msc_help();
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

/*
AT+USBDMSC=init
AT+USBDMSC=deinit
AT+USBHMSC=init
AT+USBHMSC=deinit
AT+USBHMSC=rw_test[, <file_cnt>]
*/
ATCMD_TABLE_DATA_SECTION

/* Exported functions --------------------------------------------------------*/

void example_usb_drd(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USB DRD demo start\n");

}
