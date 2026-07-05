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
#include "usbh.h"
#include "usbh_dfu.h"

/* Private defines -----------------------------------------------------------*/

static const char *const TAG = "DFU";

/* Fixed firmware size for DFU download/upload test */
#define USBH_DFU_DEMO_FW_SIZE                    2048U

// Thread priorities
#define USBH_DFU_INIT_THREAD_PRIORITY             1
#define USBH_DFU_MAIN_TASK_PRIORITY               5
// Thread stack sizes
#define USBH_DFU_INIT_THREAD_STACK_SIZE           768U
#define USBH_DFU_MAIN_TASK_STACK_SIZE             768U
/* Private variables ---------------------------------------------------------*/

static usbh_config_t usbh_cfg = {
	.speed                = USB_SPEED_HIGH,
	.isr_priority         = INT_PRI_MIDDLE,
	.main_task_stack_size = USBH_DFU_MAIN_TASK_STACK_SIZE,
	.main_task_priority   = USBH_DFU_MAIN_TASK_PRIORITY,
	.tick_source          = USBH_SOF_TICK,
#if defined(CONFIG_AMEBAGREEN2)
	/* FIFO total depth is 1024, reserve 12 for DMA addr */
	.rx_fifo_depth   = 500,
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

static rtos_sema_t dfu_attach_sema;
static rtos_sema_t dfu_detach_sema;
static rtos_sema_t dfu_download_sema;
static rtos_sema_t dfu_upload_sema;
static int         dfu_download_result;
static int         dfu_upload_result;

/* Private functions ---------------------------------------------------------*/
static int dfu_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DFU device attached\n");
	rtos_sema_give(dfu_attach_sema);
	return HAL_OK;
}

static int dfu_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DFU device detached\n");
	rtos_sema_give(dfu_detach_sema);
	return HAL_OK;
}

static int dfu_cb_get_block(u16 block_num, u8 *buf, u32 max_len)
{
	u32 offset = (u32)block_num * USBH_DFU_XFER_SIZE;
	u32 remaining;

	if (offset >= dfu_demo_fw_size) {
		return 0;  /* EOF */
	}
	remaining = dfu_demo_fw_size - offset;
	if (remaining > max_len) {
		remaining = max_len;
	}
	memcpy(buf, dfu_demo_fw + offset, remaining);
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
	u32 offset = (u32)block_num * USBH_DFU_XFER_SIZE;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Recv blk %u len %u\n", block_num, len);
	/* Verify the uploaded data matches what was downloaded */
	if (offset + len <= dfu_demo_fw_size) {
		if (memcmp(buf, dfu_demo_fw + offset, len) != 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Verify blk %u FAIL \n", block_num);
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Verify blk %u OK \n", block_num);
		}
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

static usbh_dfu_cb_t dfu_cb = {
	.attach        = dfu_cb_attach,
	.detach        = dfu_cb_detach,
	.get_block     = dfu_cb_get_block,
	.download_done = dfu_cb_download_done,
	.recv_block    = dfu_cb_recv_block,
	.upload_done   = dfu_cb_upload_done,
};

static int dfu_cb_process(usb_host_t *host, u8 msg)
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

	return HAL_OK;
}

static usbh_user_cb_t usbh_usr_cb = {
	.process = dfu_cb_process
};

static void example_usbh_dfu_thread(void *param)
{
	int ret;
	u32 i;
	u8 attr;

	UNUSED(param);

	rtos_sema_create(&dfu_attach_sema,   0U, 1U);
	rtos_sema_create(&dfu_detach_sema,   0U, 1U);
	rtos_sema_create(&dfu_download_sema, 0U, 1U);
	rtos_sema_create(&dfu_upload_sema,   0U, 1U);

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

	ret = usbh_dfu_init(&dfu_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "DFU init fail\n");
		usbh_deinit();
		goto exit;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH DFU demo start\n");

	/* Wait for DFU device to attach */
	if (rtos_sema_take(dfu_attach_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
		goto deinit;
	}

	/* Check if device supports download */
	attr = usbh_dfu_get_attr();
	if (!(attr & USB_DFU_ATTR_CAN_DNLOAD)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Device does not support DFU_DNLOAD\n");
		goto deinit;
	}

	/* Download firmware to device */
	RTK_LOGS(TAG, RTK_LOG_INFO, "Downloading %u bytes...\n", dfu_demo_fw_size);
	ret = usbh_dfu_download();
	if (ret == HAL_OK) {
		rtos_sema_take(dfu_download_sema, RTOS_SEMA_MAX_COUNT);
		ret = dfu_download_result;
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
		ret = usbh_dfu_upload();
		if (ret == HAL_OK) {
			rtos_sema_take(dfu_upload_sema, RTOS_SEMA_MAX_COUNT);
			if (dfu_upload_result != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Upload fail: %d\n", dfu_upload_result);
			}
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Upload start fail: %d\n", ret);
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Skip upload: %s\n",
				 (attr & USB_DFU_ATTR_CAN_UPLOAD) ? "non-tolerant device" : "no upload support");
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH DFU demo stop\n");
deinit:
	usbh_dfu_deinit();
	usbh_deinit();

exit:
	rtos_sema_delete(dfu_attach_sema);
	rtos_sema_delete(dfu_detach_sema);
	rtos_sema_delete(dfu_download_sema);
	rtos_sema_delete(dfu_upload_sema);
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbh_dfu(void)
{
	int status;
	rtos_task_t task;

	status = rtos_task_create(&task, "example_usbh_dfu_thread", example_usbh_dfu_thread, NULL,
							  USBH_DFU_INIT_THREAD_STACK_SIZE, USBH_DFU_INIT_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
	}
}
