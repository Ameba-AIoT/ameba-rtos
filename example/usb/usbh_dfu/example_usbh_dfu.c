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
#include "usbh_dfu.h"

/* Private defines -----------------------------------------------------------*/

/* Fixed firmware size for DFU download/upload test */
#define USBH_DFU_DEMO_FW_SIZE                    2048U

/* Budget for a device to attach, and for one download/upload run to report back.
   Bounded rather than RTOS_SEMA_MAX_COUNT so a device that never shows up, or one that
   hangs mid-transfer (an unplug is reported through the done callback), still lets the
   example tear the stack down and exit instead of parking forever. */
#define USBH_DFU_ATTACH_TIMEOUT_MS               10000U
#define USBH_DFU_XFER_TIMEOUT_MS                 30000U

// Thread priorities
#define USBH_DFU_INIT_THREAD_PRIORITY             1
#define USBH_DFU_MAIN_TASK_PRIORITY               5
// Thread stack sizes
#define USBH_DFU_INIT_THREAD_STACK_SIZE           768U
#define USBH_DFU_MAIN_TASK_STACK_SIZE             1024U

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void dfu_cb_attached(void);
static void dfu_cb_detached(void);
static int dfu_cb_get_block(u16 block_num, u8 *buf, u32 max_len);
static void dfu_cb_download_done(int status);
static int dfu_cb_recv_block(u16 block_num, u8 *buf, u32 len);
static void dfu_cb_upload_done(int status);
static void dfu_cb_process(usb_host_t *host, u8 msg);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "DFU";

static const usbh_config_t usbh_cfg = {
	.speed                = USB_SPEED_HIGH,
	.isr_priority         = INT_PRI_MIDDLE,
	.main_task_stack_size = USBH_DFU_MAIN_TASK_STACK_SIZE,
	.main_task_priority   = USBH_DFU_MAIN_TASK_PRIORITY,
	.tick_source          = USBH_SOF_TICK,
#if defined(CONFIG_AMEBAGREEN2)
	/* FIFO total 1024 DWORD, resv 12 DWORD for DMA */
	.rx_fifo_depth   = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth  = 256,
#elif defined(CONFIG_RLE1509)
	/* FIFO total 1024 DWORD, resv 48 DWORD */
	.rx_fifo_depth   = 464,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth  = 256,
#elif defined(CONFIG_AMEBAL2)
	/* FIFO total depth is 1024 DWORD, reserve 11 DWORD for DMA addr */
	.rx_fifo_depth   = 501,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth  = 256,
#elif defined(CONFIG_AMEBAPRO3)
	/* FIFO total depth is 2232 DWORD, resv 8 DWORD for DMA addr */
	.rx_fifo_depth   = 1712,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth  = 256,
#endif
};

static u8  dfu_demo_fw[USBH_DFU_DEMO_FW_SIZE];
static u32 dfu_demo_fw_size;
/* Upload block size, learned from block 0. Blocks are wTransferSize bytes, which the device
   chooses and may be smaller than USBH_DFU_DEFAULT_XFER_SIZE; only the last one is short. */
static u32 dfu_demo_up_blk_size;

static rtos_sema_t dfu_attach_sema;
static rtos_sema_t dfu_download_sema;
static rtos_sema_t dfu_upload_sema;
/* Written by the class callbacks, read by the test thread, possibly on another core */
static volatile int dfu_download_result;
static volatile int dfu_upload_result;
/* Set by recv_block on a block that does not match the downloaded image */
static volatile u8 dfu_verify_err;

static const usbh_dfu_cb_t dfu_cb = {
	.attached        = dfu_cb_attached,
	.detached        = dfu_cb_detached,
	.get_block     = dfu_cb_get_block,
	.download_done = dfu_cb_download_done,
	.recv_block    = dfu_cb_recv_block,
	.upload_done   = dfu_cb_upload_done,
};

static const usbh_dfu_config_t dfu_class_cfg = {
	/* xfer_size left 0 takes the class default. */
};

static const usbh_user_cb_t usbh_usr_cb = {
	.process = dfu_cb_process
};

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

static void dfu_cb_attached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
	rtos_sema_give(dfu_attach_sema);
}

static void dfu_cb_detached(void)
{
	/* A transfer in progress is failed by the class through its done callback, which wakes the test thread */
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
}

static int dfu_cb_get_block(u16 block_num, u8 *buf, u32 max_len)
{
	/* max_len is the negotiated wTransferSize, the same for every block of a download, and
	   every block but the last is full, so it also gives the image offset. */
	u32 offset = (u32)block_num * max_len;
	u32 remaining;

	if (offset >= dfu_demo_fw_size) {
		return 0;  /* EOF */
	}
	remaining = dfu_demo_fw_size - offset;
	if (remaining > max_len) {
		remaining = max_len;
	}
	usb_os_memcpy((void *)buf, (const void *)(dfu_demo_fw + offset), remaining);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Send blk %u offset %u/%u B\n",
			 block_num, offset, dfu_demo_fw_size);
	return (int)remaining;
}

static void dfu_cb_download_done(int status)
{
	dfu_download_result = status;
	if (status == HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Download OK\n");
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Download FAIL: %d\n", status);
	}
	rtos_sema_give(dfu_download_sema);
}

static int dfu_cb_recv_block(u16 block_num, u8 *buf, u32 len)
{
	u32 offset;

	/* A short block 0 ends the upload, so no later block depends on its length. */
	if (block_num == 0U) {
		dfu_demo_up_blk_size = len;
	}
	offset = (u32)block_num * dfu_demo_up_blk_size;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Recv blk %u len %u\n", block_num, len);
	/* Verify the uploaded data matches what was downloaded. Data past the end of the image
	   is a mismatch too: the device returned more than it was given. */
	if ((offset + len > dfu_demo_fw_size) || (memcmp(buf, dfu_demo_fw + offset, len) != 0)) {
		dfu_verify_err = 1U;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Verify blk %u FAIL\n", block_num);
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Verify blk %u OK\n", block_num);
	}
	return HAL_OK;
}

static void dfu_cb_upload_done(int status)
{
	dfu_upload_result = status;
	if (status == HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Upload OK\n");
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Upload FAIL: %d\n", status);
	}
	rtos_sema_give(dfu_upload_sema);
}

static void dfu_cb_process(usb_host_t *host, u8 msg)
{
	UNUSED(host);

	switch (msg) {
	case USBH_MSG_DISCONNECTED:
		break;
	case USBH_MSG_CONNECTED:
		break;
	default:
		break;
	}
}

static void example_usbh_dfu_thread(void *param)
{
	int ret;
	u32 i;
	u8 attr;

	UNUSED(param);

	ret = rtos_sema_create(&dfu_attach_sema,   0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto exit;
	}

	ret = rtos_sema_create(&dfu_download_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto exit;
	}

	ret = rtos_sema_create(&dfu_upload_sema,   0U, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create sema fail\n");
		goto exit;
	}

	/* Use fixed firmware size for DFU download/upload test */
	dfu_demo_fw_size = USBH_DFU_DEMO_FW_SIZE;
	RTK_LOGS(TAG, RTK_LOG_INFO, "FW size: %u B\n", dfu_demo_fw_size);
	for (i = 0; i < dfu_demo_fw_size; i++) {
		dfu_demo_fw[i] = (u8)(i & 0xFFU);
	}

	ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USBH init fail\n");
		goto exit;
	}

	ret = usbh_dfu_init(&dfu_cb, &dfu_class_cfg);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "DFU init fail\n");
		usbh_deinit();
		goto exit;
	}

	/* All class drivers registered; start USB TRX so enumeration can run. */
	usbh_start();

	/* Wait for DFU device to attach. Bounded: with no device present there is
	 * nothing to test, and this thread owns the teardown below, so it must stay
	 * reachable rather than block forever. */
	if (rtos_sema_take(dfu_attach_sema, USBH_DFU_ATTACH_TIMEOUT_MS) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Wait attach timeout\n");
		ret = HAL_TIMEOUT;
		goto deinit;
	}

	/* Check if device supports download */
	attr = usbh_dfu_get_attr();
	if (!(attr & USB_DFU_ATTR_CAN_DNLOAD)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Device does not support DFU_DNLOAD\n");
		ret = HAL_ERR_PARA;
		goto deinit;
	}

	/* Download firmware to device */
	RTK_LOGS(TAG, RTK_LOG_INFO, "Downloading %u bytes...\n", dfu_demo_fw_size);
	ret = usbh_dfu_download();
	if (ret == HAL_OK) {
		/* Bounded: download_done also reports an unplug, the bound only covers a device that hangs. */
		if (rtos_sema_take(dfu_download_sema, USBH_DFU_XFER_TIMEOUT_MS) == RTK_SUCCESS) {
			ret = dfu_download_result;
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Download timeout\n");
			ret = HAL_TIMEOUT;
		}
	}
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Download fail: %d, skip upload\n", ret);
		goto deinit;
	}

	/* Upload firmware back from device for verification.
	 * Only possible when device supports upload AND is manifestation tolerant
	 * (TOL=1: device returns to dfuIDLE after manifest; TOL=0: device enters
	 * MANIFEST-WAIT-RESET and detaches, upload not possible). */
	if ((attr & USB_DFU_ATTR_MANIFEST_TOL) && (attr & USB_DFU_ATTR_CAN_UPLOAD)) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Uploading for verification...\n");
		dfu_verify_err = 0U;
		ret = usbh_dfu_upload();
		if (ret == HAL_OK) {
			/* Bounded, see the download wait above. */
			if (rtos_sema_take(dfu_upload_sema, USBH_DFU_XFER_TIMEOUT_MS) != RTK_SUCCESS) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Upload timeout\n");
				ret = HAL_TIMEOUT;
			} else if (dfu_upload_result != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Upload fail: %d\n", dfu_upload_result);
				ret = dfu_upload_result;
			} else if (dfu_verify_err != 0U) {
				/* The transfer itself succeeded, so the class reported OK: only the data differs. */
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Upload verify fail\n");
				ret = HAL_ERR_UNKNOWN;
			} else {
				/* Upload and verify passed */
			}
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Upload start fail: %d\n", ret);
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Skip upload: %s\n",
				 (attr & USB_DFU_ATTR_CAN_UPLOAD) ? "non-tolerant device" : "no upload support");
	}

deinit:
	usbh_stop();
	usbh_dfu_deinit();
	usbh_deinit();

exit:
	rtos_sema_delete(dfu_attach_sema);
	rtos_sema_delete(dfu_download_sema);
	rtos_sema_delete(dfu_upload_sema);
	/* One verdict line for every run that printed "demo start", whichever step ended it. */
	if (ret == HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "USBH DFU demo done\n");
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USBH DFU demo aborted\n");
	}
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbh_dfu(void)
{
	int status;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH DFU demo start\n");

	status = rtos_task_create(&task, "usbh_dfu_thread", example_usbh_dfu_thread, NULL,
							  USBH_DFU_INIT_THREAD_STACK_SIZE, USBH_DFU_INIT_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBH DFU thread fail\n");
		return;
	}
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* The USB OTG ISR is delivered on CPU0: the GIC pins every SPI to core 0
	   (ITARGETSR is written to 0x01010101 in arm_gic.c gic_dist_init). This single
	   thread both drives the transfers and runs the teardown, so pinning it to
	   CPU0 puts it on the same core as the ISR and makes the local interrupt
	   disables inside the teardown meaningful again.
	   No state lock is needed in this example: there is no second thread that
	   calls the DFU class API, so the teardown can not race a caller. */
	rtos_task_set_affinity(task, 0);
#endif
}
