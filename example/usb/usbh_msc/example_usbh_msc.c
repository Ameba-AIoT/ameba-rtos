/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include <platform_autoconf.h>
#include "platform_stdlib.h"
#include "basic_types.h"
#include "os_wrapper.h"
#include "ff.h"
#include "vfs_fatfs.h"
#include "usbh_msc.h"

/* Private defines -----------------------------------------------------------*/

/* Hotplug switch
 *   0 = replug loop:  test files -> stop -> wait for replug -> repeat
 *                      USB stack stays alive; reconnect is handled by the
 *                      core without tearing down the stack
 *   1 = continuous:   test files -> repeat immediately; on detach the
 *                      stack is fully torn down and rebuilt to guarantee
 *                      clean transfer state across plug/unplug cycles
 */
#define CONFIG_USBH_MSC_HOTPLUG              1

// Thread priorities
#define USBH_MSC_INIT_THREAD_PRIORITY        2
#define USBH_MSC_MAIN_TASK_PRIORITY          3U
#define USBH_MSC_HOTPLUG_THREAD_PRIORITY     2

// Thread stack sizes
#define USBH_MSC_INIT_THREAD_STACK_SIZE      11400U
#define USBH_MSC_MAIN_TASK_STACK_SIZE        1024U
#define USBH_MSC_HOTPLUG_THREAD_STACK_SIZE   1024U

#define USBH_MSC_TEST_BUF_SIZE               4096
#define USBH_MSC_TEST_ROUNDS                 20
#define USBH_MSC_CHECK_DATA                  0
#define USBH_MSC_MAX_FILES                   10

/* Bound of the "wait until the class is set up" step. setup() only returns once
   every LUN init has ended, which the class bounds by its own 10 s LUN budget plus
   one BOT Reset Recovery and, as a last resort, a device reset and re-enumeration.
   Twice that keeps this wait from expiring while the class is still recovering. */
#define USBH_MSC_READY_TIMEOUT_MS            40000U

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void msc_cb_attached(void);
static void msc_cb_detached(void);
static void msc_cb_setup(void);
static void msc_cb_process(usb_host_t *host, u8 msg);
static int msc_lock_stack(void);
static void msc_unlock_stack(void);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "MSC";

static rtos_sema_t msc_attach_sema;
static rtos_sema_t msc_ready_sema;
static __IO int msc_is_connected = 0;
static __IO int msc_is_ready = 0;
/* 1 while the host core resets the device (USBH_MSG_RESET): the class detach that follows
   is not an unplug, and the device comes back by itself through attach/setup. */
static __IO u8 msc_reenumerating = 0;
/* 1 once the host core gave the attached device up (USBH_MSG_ERROR): its own resets did not
   bring it back, and a stack rebuild resets it the same way, so only a replug helps. */
static __IO u8 msc_need_replug = 0;
static u32 filenum = 0;
static u8 *msc_wt_buf;
static u8 *msc_rd_buf;

#if CONFIG_USBH_MSC_HOTPLUG
static rtos_sema_t msc_detach_sema;
#endif

/* Serializes the FatFs I/O path against the hotplug thread's deinit/reinit.
   Every f_read/f_write lands in USB_disk_read/USB_disk_write, which call
   usbh_msc_read/usbh_msc_write; those poll the BOT state machine for the whole
   duration of a sector transfer. Meanwhile usbh_msc_deinit() frees the CBW/CSW
   and bounce buffers that poll loop walks, and usbh_deinit() memset()s the host
   handle. The class API only checks host != NULL and the unit state on entry, so
   under SMP the test thread on core 1 can be deep inside a transfer while core 0
   frees it out from under it. The test thread must be excluded by lock, not by
   core.
   Created before the stack comes up and never deleted: the mount/test path can
   not be joined by the example, so there is no point in time at which this mutex
   is provably unreferenced. msc_stack_ready is the gate that keeps callers off
   the stack instead. */
static rtos_mutex_t msc_state_mutex = NULL;

/* 1: usbh_init() and usbh_msc_init() are both done and the class API may be
   called. Cleared before any teardown, so a non-zero value also implies
   msc_state_mutex is valid. */
static volatile u8 msc_stack_ready = 0;

/* Set by the hotplug thread BEFORE it takes the lock, so the test thread sees it
   between file operations, abandons the run and stops contending. Without that
   ordering the test thread would keep re-taking the lock for its next sector and
   the hotplug thread could be starved out of a teardown it must perform. */
static volatile u8 msc_teardown = 0;

/* Set by the hotplug thread when the stack can not be rebuilt. Tells the test
   thread to stop for good rather than wait for an attach that can never come. */
static volatile u8 msc_stack_fatal = 0;

static const usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = USBH_MSC_MAIN_TASK_STACK_SIZE,
	.main_task_priority = USBH_MSC_MAIN_TASK_PRIORITY,
	.tick_source = USBH_SOF_TICK,
#if defined(CONFIG_AMEBAGREEN2)
	/*FIFO total 1024 DWORD, resv 12 DWORD for DMA*/
	.rx_fifo_depth = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined(CONFIG_RLE1509)
	/*FIFO total 1024 DWORD, resv 48 DWORD */
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

static const usbh_msc_cb_t msc_usr_cb = {
	.attached = msc_cb_attached,
	.detached = msc_cb_detached,
	.setup = msc_cb_setup,
};

static const usbh_msc_config_t msc_class_cfg = {
	/* buf_size left 0 takes the class default. */
};

static const usbh_user_cb_t usbh_usr_cb = {
	.process = msc_cb_process
};

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Take the stack lock if the class API may be reached right now
  * @note   Gates on msc_stack_ready BEFORE touching the mutex: the handle is
  *         still NULL if the example never started, and the flag is already
  *         cleared if a teardown is pending. A hotplug right after this check is
  *         harmless - the mutex stays valid and the re-check under it catches the
  *         loss.
  * @note   Wraps the FatFs calls rather than the class API, because the class is
  *         reached indirectly through the disk driver and there is no hook inside
  *         FatFs. The hold time is therefore one FatFs operation, bounded by the
  *         per-transfer timeout inside usbh_msc_read/usbh_msc_write rather than by
  *         a completion callback that may never arrive.
  * @retval RTK_SUCCESS when the lock is held and the stack is usable, -RTK_ERR_IO when
  *         the caller must give up without unlocking
  */
static int msc_lock_stack(void)
{
	if ((msc_stack_ready == 0U) || (msc_teardown != 0U)) {
		return -RTK_ERR_IO;
	}

	rtos_mutex_take(msc_state_mutex, RTOS_MAX_TIMEOUT);

	if ((msc_stack_ready == 0U) || (msc_teardown != 0U)) {
		/* Lost the race against a teardown */
		rtos_mutex_give(msc_state_mutex);
		return -RTK_ERR_IO;
	}

	return RTK_SUCCESS;
}

/**
  * @brief  Release the stack lock taken by msc_lock_stack()
  * @retval None
  */
static void msc_unlock_stack(void)
{
	rtos_mutex_give(msc_state_mutex);
}

/*
 * Locked FatFs wrappers.
 *
 * Every one of these reaches the MSC class through the disk driver, so each has
 * to be excluded from a concurrent teardown. They are thin wrappers rather than
 * one lock around the whole test, because holding the lock for a full 10-file run
 * would stall a hotplug for the entire test duration. One FatFs operation is the
 * right granularity: it is bounded by the per-transfer timeout inside
 * usbh_msc_read/usbh_msc_write.
 *
 * On a lost gate they return FR_DISK_ERR, which is exactly the code this example
 * already treats as "device went away", so no call site needs a second error path.
 */
static FRESULT msc_fs_mount(FATFS *fs, const TCHAR *path, BYTE opt)
{
	FRESULT res;

	if (msc_lock_stack() != RTK_SUCCESS) {
		return FR_DISK_ERR;
	}
	res = f_mount(fs, path, opt);
	msc_unlock_stack();

	return res;
}

/*
 * Unmount and close are the two operations that must run even when the device is
 * already gone, because they also drop FatFs' own bookkeeping: the mount points
 * at a FATFS object on the caller's stack and the FIL holds the open-file state.
 * Skipping them on a closed gate would leave FatFs believing the volume is still
 * mounted, with a dangling pointer once the caller returns.
 *
 * So these two take the lock unconditionally instead of going through
 * msc_lock_stack(). The lock itself is always valid (created once, never
 * deleted), and holding it means no teardown is in flight. If the stack is
 * already down, the write that FatFs issues to flush is rejected by the class on
 * its own state checks and the operation still clears the FatFs side.
 */
static FRESULT msc_fs_unmount(const TCHAR *path)
{
	FRESULT res;

	rtos_mutex_take(msc_state_mutex, RTOS_MAX_TIMEOUT);
	res = f_unmount(path);
	rtos_mutex_give(msc_state_mutex);

	return res;
}

static FRESULT msc_fs_open(FIL *fp, const TCHAR *path, BYTE mode)
{
	FRESULT res;

	if (msc_lock_stack() != RTK_SUCCESS) {
		return FR_DISK_ERR;
	}
	res = f_open(fp, path, mode);
	msc_unlock_stack();

	return res;
}

static FRESULT msc_fs_close(FIL *fp)
{
	FRESULT res;

	/* Unconditional lock, see msc_fs_unmount() above. */
	rtos_mutex_take(msc_state_mutex, RTOS_MAX_TIMEOUT);
	res = f_close(fp);
	rtos_mutex_give(msc_state_mutex);

	return res;
}

static FRESULT msc_fs_lseek(FIL *fp, FSIZE_t ofs)
{
	FRESULT res;

	if (msc_lock_stack() != RTK_SUCCESS) {
		return FR_DISK_ERR;
	}
	res = f_lseek(fp, ofs);
	msc_unlock_stack();

	return res;
}

static FRESULT msc_fs_write(FIL *fp, const void *buf, UINT btw, UINT *bw)
{
	FRESULT res;

	if (msc_lock_stack() != RTK_SUCCESS) {
		*bw = 0U;
		return FR_DISK_ERR;
	}
	res = f_write(fp, buf, btw, bw);
	msc_unlock_stack();

	return res;
}

static FRESULT msc_fs_read(FIL *fp, void *buf, UINT btr, UINT *br)
{
	FRESULT res;

	if (msc_lock_stack() != RTK_SUCCESS) {
		*br = 0U;
		return FR_DISK_ERR;
	}
	res = f_read(fp, buf, btr, br);
	msc_unlock_stack();

	return res;
}

static void msc_cb_attached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
	msc_is_connected = 1;
	if (msc_reenumerating != 0U) {
		/* Re-attach after a device reset: the test thread is still waiting for setup. */
		msc_reenumerating = 0U;
		return;
	}
	rtos_sema_give(msc_attach_sema);
}

static void msc_cb_detached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
	msc_is_ready = 0;
	if (msc_reenumerating != 0U) {
		/* The core is resetting the device, which re-attaches by itself: no rebuild. */
		return;
	}
#if CONFIG_USBH_MSC_HOTPLUG
	rtos_sema_give(msc_detach_sema);
#endif
}

static void msc_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
	msc_is_ready = 1;
	rtos_sema_give(msc_ready_sema);
}

static void msc_cb_process(usb_host_t *host, u8 msg)
{
	UNUSED(host);

	switch (msg) {
	case USBH_MSG_DISCONNECTED:
		/* Also reached when the device never got as far as the class attach, so no
		   detached callback would ever clear the application state. */
		msc_reenumerating = 0U;
		msc_need_replug = 0U;
		msc_is_connected = 0;
		msc_is_ready = 0;
		break;
	case USBH_MSG_RESET:
		msc_reenumerating = 1U;
		msc_is_ready = 0;
		break;
	case USBH_MSG_ERROR:
		/* The core gave the device up (enumeration or reenumerate budget spent): only a replug
		   helps. Wake a test thread waiting for setup, so it stops at once. */
		msc_reenumerating = 0U;
		msc_need_replug = 1U;
		msc_is_ready = 0;
		rtos_sema_give(msc_ready_sema);
		break;
	case USBH_MSG_CONNECTED:
		break;
	default:
		break;
	}
}

/* TEMP DIAG: dump what FatFs sees at sector 0 and at the first MBR partition.
 * check_fs() rejects a sector unless win[510:511] == 0x55AA plus a valid
 * JmpBoot/BPB, so these bytes tell FAT32-vs-garbage apart. Remove once the
 * mount failure is understood. */
static void usbh_msc_dump_boot_sector(void)
{
	u8 *sec;
	u32 lba;
	u32 i;
	DRESULT res;

	sec = (u8 *)usb_os_malloc(512);
	if (sec == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Diag: alloc fail\n");
		return;
	}

	for (i = 0; i < 2U; i++) {
		lba = 0U;
		if (i == 1U) {
			/* MBR partition 1 start LBA: little-endian at offset 0x1C6 */
			lba = ((u32)sec[0x1C9] << 24) | ((u32)sec[0x1C8] << 16) | ((u32)sec[0x1C7] << 8) | (u32)sec[0x1C6];
			RTK_LOGS(TAG, RTK_LOG_INFO, "Diag: PTE1 type %02x lba %d\n", sec[0x1C2], lba);
			if (lba == 0U) {
				break;
			}
		}

		/* Reaches usbh_msc_read() through the disk driver, so it needs the same
		   exclusion as the FatFs path. */
		if (msc_lock_stack() != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Diag: stack gone\n");
			break;
		}
		res = USB_disk_Driver.disk_read(sec, lba, 1);
		msc_unlock_stack();
		if (res != RES_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Diag: read lba %d fail\n", lba);
			break;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Diag: lba %d sig %02x%02x jmp %02x fstype %c%c%c%c%c%c%c%c\n",
				 lba, sec[510], sec[511], sec[0],
				 sec[82], sec[83], sec[84], sec[85], sec[86], sec[87], sec[88], sec[89]);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Diag: bps %02x%02x spc %02x rsvd %02x%02x nfat %02x\n",
				 sec[12], sec[11], sec[13], sec[15], sec[14], sec[16]);
		/* First 16 bytes: tells a zeroed/garbage buffer from a real boot sector */
		RTK_LOGS(TAG, RTK_LOG_INFO, "Diag: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
				 sec[0], sec[1], sec[2], sec[3], sec[4], sec[5], sec[6], sec[7],
				 sec[8], sec[9], sec[10], sec[11], sec[12], sec[13], sec[14], sec[15]);
	}

	usb_os_mfree((void *)sec);
}

/*  I/O test routine (10 files, each with W/R of multiple sizes) */
static int usbh_msc_file_test(void)
{
	FATFS fs;
	FIL f;
	int drv_num = -1;
	FRESULT res;
	char logical_drv[4];
	char path[64] = {0};
	int ret = RTK_SUCCESS;
	u32 br;
	u32 bw;
	u32 round = 0;
	u32 start;
	u32 elapse;
	u32 perf = 0;
	u32 test_sizes[] = {512, 1024, 2048, 4096};
	u32 test_size;
	u32 i;
	u8 data;

	/* Wait for device attach + MSC ready. Unbounded on purpose: with no device
	   plugged in there is nothing to do but wait, and this example's whole point
	   is to run again on the next attach. A dead stack is the one case that must
	   not wait, and the hotplug thread signals that through msc_stack_fatal. */
	RTK_LOGS(TAG, RTK_LOG_INFO, "Wait for device attach...\n");
	if (rtos_sema_take(msc_attach_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Take attach sema fail\n");
		return RTK_FAIL;
	}

	if (msc_stack_fatal != 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stack gone, stop testing\n");
		return RTK_FAIL;
	}

	/* Bounded: a device that attaches but never completes setup must not park
	   this thread forever. Woken by setup, or by a teardown giving the sema. */
	start = rtos_time_get_current_system_time_ms();
	while (!msc_is_ready) {
		if (msc_need_replug != 0U) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Device not responding, replug\n");
			return -RTK_ERR_IO;
		}
		if (((!msc_is_connected) && (msc_reenumerating == 0U)) || (msc_teardown != 0U) || (msc_stack_ready == 0U)) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Device disconnected before ready\n");
			return RTK_FAIL;
		}
		elapse = rtos_time_get_current_system_time_ms() - start;
		if (elapse >= USBH_MSC_READY_TIMEOUT_MS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Wait ready timeout\n");
			return -RTK_ERR_TIMEOUT;
		}
		rtos_sema_take(msc_ready_sema, USBH_MSC_READY_TIMEOUT_MS - elapse);
	}
	if (usbh_msc_unit_is_ready(0U) == 0) {
		/* setup() ran, but the class retired the LUN after its whole recovery. */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Lun 0 unusable\n");
		return -RTK_ERR_IO;
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Device ready\n");

	/* Register disk driver and mount */
	drv_num = FATFS_RegisterDiskDriver(&USB_disk_Driver);
	if (drv_num < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Register disk driver fail\n");
		return RTK_FAIL;
	}

	logical_drv[0] = drv_num + '0';
	logical_drv[1] = ':';
	logical_drv[2] = '/';
	logical_drv[3] = 0;
	strcpy(path, logical_drv);

	res = msc_fs_mount(&fs, logical_drv, 1);
	if (res != FR_OK) {
		/* rc: 1 FR_DISK_ERR, 3 FR_NOT_READY, 13 FR_NO_FILESYSTEM */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Mount logical drive fail, rc=%d\n", res);
		/* A device that left during the mount explains the failure by itself, and its
		   sectors can not be read any more. */
		if ((msc_is_ready != 0) && (msc_teardown == 0U)) {
			usbh_msc_dump_boot_sector();
		}
		FATFS_UnRegisterDiskDriver(drv_num);
		/* A disk without a FAT/exFAT volume is a media problem, not an I/O one: rebuilding
		   the stack reads the same sectors again, so let the caller wait for a replug */
		return (res == FR_NO_FILESYSTEM) ? -RTK_ERR_BADARG : RTK_FAIL;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "FatFS USB W/R performance test start, free heap: 0x%08x\n", rtos_mem_get_free_heap_size());

	/* Write / read up to USBH_MSC_MAX_FILES files */
	for (filenum = 0; filenum < USBH_MSC_MAX_FILES; filenum++) {
#if CONFIG_USBH_MSC_HOTPLUG
		if (!msc_is_ready) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Device detached during test\n");
			ret = RTK_FAIL;
			break;
		}
#endif
		/* Bounded by the room left after the "N:/" drive prefix. DiagSnPrintf has no %u. */
		DiagSnPrintf(&path[3], sizeof(path) - 3U, "TEST%d.DAT", (int)filenum);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Open file: %s\n", path);

		res = msc_fs_open(&f, path, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
		if (res) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Open file fail: %s, rc=%d\n", path, res);
			ret = RTK_FAIL;
			break;
		}

		data = _rand() % 0xFF;
		usb_os_memset((void *)msc_wt_buf, data, USBH_MSC_TEST_BUF_SIZE);

		for (i = 0; i < sizeof(test_sizes) / sizeof(test_sizes[0]); ++i) {
			test_size = test_sizes[i];
			if (test_size > USBH_MSC_TEST_BUF_SIZE) {
				break;
			}

			/* Write */
			RTK_LOGS(TAG, RTK_LOG_INFO, "W test: size %d, round %d...\n", test_size, USBH_MSC_TEST_ROUNDS);
			start = SYSTIMER_TickGet();
			for (round = 0; round < USBH_MSC_TEST_ROUNDS; ++round) {
				res = msc_fs_write(&f, (void *)msc_wt_buf, test_size, (UINT *)&bw);
#if CONFIG_USBH_MSC_HOTPLUG
				if (res == FR_DISK_ERR) {
					RTK_LOGS(TAG, RTK_LOG_WARN, "W err: device disconnected (rc=%d)\n", res);
					break;
				}
#endif
				if (res || (bw < test_size)) {
					(void)msc_fs_lseek(&f, 0);
					RTK_LOGS(TAG, RTK_LOG_ERROR, "W err bw=%d, rc=%d\n", bw, res);
					ret = RTK_FAIL;
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
			(void)msc_fs_lseek(&f, 0);
			RTK_LOGS(TAG, RTK_LOG_INFO, "R test: size = %d round = %d...\n", test_size, USBH_MSC_TEST_ROUNDS);
			start = SYSTIMER_TickGet();
			for (round = 0; round < USBH_MSC_TEST_ROUNDS; ++round) {
				res = msc_fs_read(&f, (void *)msc_rd_buf, test_size, (UINT *)&br);
#if CONFIG_USBH_MSC_HOTPLUG
				if (res == FR_DISK_ERR) {
					RTK_LOGS(TAG, RTK_LOG_WARN, "R err: device disconnected (rc=%d)\n", res);
					break;
				}
#endif
				if (res || (br < test_size)) {
					(void)msc_fs_lseek(&f, 0);
					RTK_LOGS(TAG, RTK_LOG_ERROR, "R err br=%d, rc=%d\n", br, res);
					ret = RTK_FAIL;
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
			(void)msc_fs_lseek(&f, 0);

#if USBH_MSC_CHECK_DATA
			if (!(memcmp(msc_wt_buf, msc_rd_buf, test_size) == 0)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "WR%d check err: %d-%d-%d-%d vs %d-%d-%d-%d\n", test_size,
						 msc_wt_buf[0], msc_wt_buf[1], msc_wt_buf[test_size - 2], msc_wt_buf[test_size - 1],
						 msc_rd_buf[0], msc_rd_buf[1], msc_rd_buf[test_size - 2], msc_rd_buf[test_size - 1]);
				ret = RTK_FAIL;
				break;
			}
#endif
		}

#if CONFIG_USBH_MSC_HOTPLUG
		if (res == FR_DISK_ERR) {
			(void)msc_fs_close(&f);
			ret = RTK_FAIL;
			break;
		}
#endif

		(void)msc_fs_close(&f);
		if (ret) {
			break;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "File %s done\n", path);
		rtos_time_delay_ms(20);
	}

	/* Unmount & unregister */
	if (msc_fs_unmount(logical_drv) != FR_OK) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Unmount fail\n");
	}
	if (FATFS_UnRegisterDiskDriver(drv_num)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Unregister disk driver fail\n");
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "FatFS USB W/R performance test done, free heap: 0x%08x\n", rtos_mem_get_free_heap_size());
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
			rtos_time_delay_ms(100);

			/* Announce the teardown BEFORE contending for the lock, so the test
			   thread abandons its run between FatFs operations instead of
			   re-taking the lock for its next sector and starving this thread
			   out of a teardown it must perform. */
			msc_teardown = 1U;
			/* Wake a test thread parked waiting for setup, so it sees the teardown now. */
			rtos_sema_give(msc_ready_sema);

			rtos_mutex_take(msc_state_mutex, RTOS_MAX_TIMEOUT);
			/* Close the gate under the lock: no FatFs operation is in flight here,
			   and any later one gives up on the cleared flag. */
			msc_stack_ready = 0U;

			usbh_stop();
			usbh_msc_deinit();
			rtos_time_delay_ms(20); /* let USB main task flush its queue before deletion */
			usbh_deinit();

			/* Reset state and drain any transient attach event fired during
			 * the USB controller reset (power cycle causes a brief connect). */
			msc_is_connected = 0;
			msc_is_ready = 0;
			msc_reenumerating = 0U;
			msc_need_replug = 0U;
			rtos_sema_take(msc_attach_sema, 0);
			rtos_sema_take(msc_ready_sema, 0);
			rtos_mutex_give(msc_state_mutex);

			rtos_time_delay_ms(10);

			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

			rtos_mutex_take(msc_state_mutex, RTOS_MAX_TIMEOUT);

			ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
			if (ret != RTK_SUCCESS) {
				rtos_mutex_give(msc_state_mutex);
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Reinit fail\n");
				break;
			}

			ret = usbh_msc_init(&msc_usr_cb, &msc_class_cfg);
			if (ret != RTK_SUCCESS) {
				usbh_deinit();
				rtos_mutex_give(msc_state_mutex);
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Reinit fail\n");
				break;
			}

			/* Re-arm USB TRX after the re-init. */
			usbh_start();

			/* Open the gate as the very last step, so no FatFs operation runs
			   against a half-rebuilt stack. Clear teardown only after ready is
			   set, or the test thread could slip through seeing neither flag. */
			msc_stack_ready = 1U;
			msc_teardown = 0U;
			rtos_mutex_give(msc_state_mutex);

			RTK_LOGS(TAG, RTK_LOG_INFO, "Reinit done\n");
		}
	}

	/* A re-init failed, so the stack is gone for good. Keep the FatFs path out,
	   and wake the test thread if it is parked waiting for an attach that will
	   never come. The semas are left alive on purpose: the test thread may still
	   be about to take one, and freeing them here would pull them out from under
	   it. */
	msc_stack_fatal = 1U;
	msc_stack_ready = 0U;
	msc_teardown = 1U;
	rtos_sema_give(msc_attach_sema);
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread exit\n");

	rtos_task_delete(NULL);
}
#endif

/* Main test thread */
static void example_usbh_msc_thread(void *param)
{
	int ret;

	UNUSED(param);

	/* Created before the stack comes up and never deleted, see the declaration */
	if (msc_state_mutex == NULL) {
		ret = rtos_mutex_create(&msc_state_mutex);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create state mutex fail\n");
			goto exit_free;
		}
	}

	ret = rtos_sema_create(&msc_attach_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto exit_free;
	}
	ret = rtos_sema_create(&msc_ready_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto exit_free;
	}
#if CONFIG_USBH_MSC_HOTPLUG
	ret = rtos_sema_create(&msc_detach_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto exit_free;
	}
#endif

	msc_wt_buf = (u8 *)usb_os_malloc(USBH_MSC_TEST_BUF_SIZE);
	msc_rd_buf = (u8 *)usb_os_malloc(USBH_MSC_TEST_BUF_SIZE);
	if (!msc_wt_buf || !msc_rd_buf) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc test buf fail\n");
		goto exit_free;
	}

	/* Init USB host + MSC class driver */
	ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USBH init fail\n");
		goto exit_free;
	}

	ret = usbh_msc_init(&msc_usr_cb, &msc_class_cfg);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "MSC init fail\n");
		usbh_deinit();
		goto exit_free;
	}

	/* All class drivers registered; start USB TRX so enumeration can run. */
	usbh_start();

	/* The stack is fully up: allow the FatFs path to reach the class. */
	msc_stack_ready = 1U;

#if CONFIG_USBH_MSC_HOTPLUG
	/* Start hotplug thread (waits on detach_sema, re-inits USB stack) */
	{
		rtos_task_t hotplug_task;
		ret = rtos_task_create(&hotplug_task, "usbh_msc_hotplug_thread",
							   example_usbh_msc_hotplug_thread, NULL,
							   USBH_MSC_HOTPLUG_THREAD_STACK_SIZE,
							   USBH_MSC_HOTPLUG_THREAD_PRIORITY);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug thread fail\n");
			/* The gate is already open, so hold it shut under the lock for this
			   rollback too, exactly as the hotplug thread does. */
			msc_teardown = 1U;
			rtos_mutex_take(msc_state_mutex, RTOS_MAX_TIMEOUT);
			msc_stack_ready = 0U;
			usbh_stop();
			usbh_msc_deinit();
			usbh_deinit();
			rtos_mutex_give(msc_state_mutex);
			goto exit_free;
		}
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
		/* The USB OTG ISR is delivered on CPU0: the GIC pins every SPI to core 0
		   (ITARGETSR is written to 0x01010101 in arm_gic.c gic_dist_init). Pinning
		   the hotplug/deinit thread to CPU0 puts it on the same core as the ISR,
		   so the local interrupt disables inside the teardown are meaningful
		   again. The test thread stays unaffined: a pending teardown is excluded
		   from it by msc_state_mutex, not by core. */
		rtos_task_set_affinity(hotplug_task, 0);
#endif
	}
#endif

	/* I/O test loop */
#if CONFIG_USBH_MSC_HOTPLUG
	for (;;) {
		ret = usbh_msc_file_test();
		if (msc_stack_fatal != 0U) {
			/* The stack can not be rebuilt, so no further attach will arrive. */
			break;
		}
		if (ret == RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "All files done, repeat from 0\n");
			/* Device still attached -- give sema so the next call does not
			 * block at rtos_sema_take waiting for a new attach event. */
			rtos_sema_give(msc_attach_sema);
		} else if (ret == -RTK_ERR_BADARG) {
			/* Wait for the next attach, which only an unplug and replug can bring */
			RTK_LOGS(TAG, RTK_LOG_WARN, "No FAT volume, replug a FAT/exFAT disk\n");
		} else if ((msc_is_connected != 0) && (msc_teardown == 0U) && (msc_need_replug == 0U)) {
			/* The device is still attached, so no unplug will start a rebuild: the run
			   failed on a device the class gave up on. Rebuild the stack now, which resets
			   the controller and enumerates the device afresh, as a replug would. */
			RTK_LOGS(TAG, RTK_LOG_WARN, "I/O failed on attached device, rebuild stack\n");
			rtos_sema_give(msc_detach_sema);
		} else {
			RTK_LOGS(TAG, RTK_LOG_WARN, "I/O interrupted, wait for next attach\n");
			/* Hotplug thread tears down + rebuilds the stack and gives
			 * attach_sema after enumeration completes. */
		}
	}
#else
	/* Replug loop: test 10 files, then wait for the next physical replug
	 * event before running again.  usbh_msc_file_test() blocks at
	 * rtos_sema_take(msc_attach_sema) until msc_cb_attached fires. */
	for (;;) {
		ret = usbh_msc_file_test();
		RTK_LOGS(TAG, RTK_LOG_INFO, "Test %s, unplug and replug to repeat\n",
				 ret == RTK_SUCCESS ? "done" : "interrupted");
	}
	/* Unreachable: without the hotplug thread nothing can declare the stack dead,
	   so this loop only ends with the firmware. */
#endif

	/* Cleanup (unreachable in CONFIG_USBH_MSC_HOTPLUG=1 for the test thread, but keeps
	 * the pattern consistent) */
	usbh_stop();
	usbh_msc_deinit();
	usbh_deinit();

exit_free:
	RTK_LOGS(TAG, RTK_LOG_ERROR, "USBH MSC demo aborted\n");
	usb_os_mfree((void *)msc_wt_buf);
	usb_os_mfree((void *)msc_rd_buf);
	rtos_sema_delete(msc_attach_sema);
	rtos_sema_delete(msc_ready_sema);
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

	ret = rtos_task_create(&task, "usbh_msc_thread", example_usbh_msc_thread, NULL,
						   USBH_MSC_INIT_THREAD_STACK_SIZE, USBH_MSC_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBH MSC thread fail\n");
	}
}
