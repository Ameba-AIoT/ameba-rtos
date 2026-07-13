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
#include "usbd.h"
#include "usbd_dfu.h"

/* Private defines -----------------------------------------------------------*/

static const char *const TAG = "DFU";

#define USBD_DFU_DEMO_BUF_SIZE      (USBD_DFU_XFER_SIZE * 4U)  /* 4 KB */
#define USBD_DFU_DEMO_WRITE_POLL_MS 1U  /* write_task needs a scheduler tick even for RAM memcpy */

// Thread priorities
#define USBD_DFU_INIT_THREAD_PRIORITY             5
#define USBD_DFU_RECONF_THREAD_PRIORITY           4
// Thread stack sizes
#define USBD_DFU_INIT_THREAD_STACK_SIZE           768U
#define USBD_DFU_RECONF_THREAD_STACK_SIZE         768U
/* Private variables ---------------------------------------------------------*/

static const usbd_config_t dfu_cfg = {
	.speed = USB_SPEED_HIGH,
	.isr_priority = INT_PRI_MIDDLE,
	.diag_enable = 1,
#if defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBAD) || defined(CONFIG_AMEBADPLUS)
	.nptx_max_epmis_cnt = 1U,
#elif defined(CONFIG_AMEBAGREEN2)
	/* DFIFO total 1024 DWORD, resv 12 DWORD for DMA addr, EP0 fixed 32 DWORD */
	.rx_fifo_depth  = 644U,
	.ptx_fifo_depth = {16U, 256U, 32U, 16U, 16U,},
#elif defined(CONFIG_AMEBAL2)
	.rx_fifo_depth  = 661U,
	.ptx_fifo_depth = {256U, 16U, 32U, 16U,},
#elif defined(CONFIG_AMEBAPRO3)
	.rx_fifo_depth  = 1664U,
	.ptx_fifo_depth = {256U, 32U, 16U,},
#endif
};

static u8  dfu_demo_fw_buf[USBD_DFU_DEMO_BUF_SIZE];
static u32 dfu_demo_fw_len;

#if CONFIG_USBD_DFU_WILL_DETACH
static rtos_sema_t dfu_reconf_sema;
#endif

/* Private functions ---------------------------------------------------------*/

static int dfu_cb_init(void)
{
	memset(dfu_demo_fw_buf, 0, USBD_DFU_DEMO_BUF_SIZE);
	dfu_demo_fw_len = 0U;
	RTK_LOGS(TAG, RTK_LOG_INFO, "Init\n");
	return HAL_OK;
}

static int dfu_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Deinit\n");
	return HAL_OK;
}

/*
 * write() is called from the DFU write_task context — blocking is permitted.
 */
static int dfu_cb_write(u16 block_num, u8 *buf, u32 len)
{
	u32 offset = (u32)block_num * USBD_DFU_XFER_SIZE;
	u32 end;

	if (offset + len > USBD_DFU_DEMO_BUF_SIZE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Write out of range: blk=%u len=%u\n", block_num, len);
		return HAL_ERR_PARA;
	}
	memcpy(dfu_demo_fw_buf + offset, buf, len);
	end = offset + len;
	if (end > dfu_demo_fw_len) {
		dfu_demo_fw_len = end;
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Write blk %u offset %u len %u (total %u B)\n",
			 block_num, offset, len, dfu_demo_fw_len);
	return HAL_OK;
}

/*
 * read() is called from ISR context (UPLOAD SETUP handler) — must not block.
 */
static int dfu_cb_read(u16 block_num, u8 *buf, u32 max_len)
{
	u32 offset = (u32)block_num * USBD_DFU_XFER_SIZE;
	u32 remaining;

	if (offset >= dfu_demo_fw_len) {
		return 0;  /* EOF */
	}
	remaining = dfu_demo_fw_len - offset;
	RTK_LOGS(TAG, RTK_LOG_INFO, "Read blk %u offset %u max %u (remain %u B)\n",
			 block_num, offset, max_len, remaining);
	if (remaining > max_len) {
		remaining = max_len;
	}
	memcpy(buf, dfu_demo_fw_buf + offset, remaining);
	return (int)remaining;
}

/*
 * manifest() is called from manifest_task context — blocking is permitted.
 */
static void dfu_cb_manifest(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Manifest OK (%u B in RAM)\n", dfu_demo_fw_len);
}

/*
 * detach() is called from ISR context when the host issues DFU_DETACH in
 * Run-Time mode (appIDLE).
 */
static void dfu_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DFU detach\n");
}

#if CONFIG_USBD_DFU_WILL_DETACH
/*
 * reconf() is called from the driver's reconf_task (task context) after the
 * GETSTATUS response has been transmitted.  Signal usbd_dfu_reconf_thread to
 * perform the deinit/init re-enumeration cycle.
 */
static void dfu_cb_reconf(void)
{
	rtos_sema_give(dfu_reconf_sema);
}
#endif /* CONFIG_USBD_DFU_WILL_DETACH */

static usbd_dfu_cb_t dfu_cb = {
	.init           = dfu_cb_init,
	.deinit         = dfu_cb_deinit,
	.write          = dfu_cb_write,
	.read           = dfu_cb_read,
	.manifest       = dfu_cb_manifest,
	.detach         = dfu_cb_detach,
#if CONFIG_USBD_DFU_WILL_DETACH
	.reconf         = dfu_cb_reconf,
#endif
};

#if CONFIG_USBD_DFU_WILL_DETACH
static void usbd_dfu_reconf_thread(void *param)
{
	int ret;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(dfu_reconf_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			break;
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "Reconf: re-enumerating\n");
		usbd_dfu_deinit();
		ret = usbd_deinit();
		if (ret != HAL_OK) {
			break;
		}
		//RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
		ret = usbd_init(&dfu_cfg);
		if (ret != HAL_OK) {
			break;
		}
		ret = usbd_dfu_init(&dfu_cb);
		if (ret != HAL_OK) {
			usbd_deinit();
			break;
		}
		usbd_dfu_set_write_poll_ms(USBD_DFU_DEMO_WRITE_POLL_MS);
	}

	RTK_LOGS(TAG, RTK_LOG_ERROR, "Reconf thread fail\n");
	rtos_sema_delete(dfu_reconf_sema);
	rtos_task_delete(NULL);
}
#endif /* CONFIG_USBD_DFU_WILL_DETACH */

static void example_usbd_dfu_thread(void *param)
{
	int ret;
#if CONFIG_USBD_DFU_WILL_DETACH
	rtos_task_t reconf_task;
#endif

	UNUSED(param);

#if CONFIG_USBD_DFU_WILL_DETACH
	rtos_sema_create(&dfu_reconf_sema, 0U, 1U);
#endif

	ret = usbd_init(&dfu_cfg);
	if (ret != HAL_OK) {
		goto exit;
	}

	ret = usbd_dfu_init(&dfu_cb);
	if (ret != HAL_OK) {
		usbd_deinit();
		goto exit;
	}

	usbd_dfu_set_write_poll_ms(USBD_DFU_DEMO_WRITE_POLL_MS);

#if CONFIG_USBD_DFU_WILL_DETACH
	ret = rtos_task_create(&reconf_task, "usbd_dfu_reconf_thread", usbd_dfu_reconf_thread, NULL,
						   USBD_DFU_RECONF_THREAD_STACK_SIZE, USBD_DFU_RECONF_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		usbd_dfu_deinit();
		usbd_deinit();
		goto exit;
	}
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD DFU demo start\n");
	rtos_task_delete(NULL);
	return;

exit:
	RTK_LOGS(TAG, RTK_LOG_ERROR, "USBD DFU demo fail\n");
#if CONFIG_USBD_DFU_WILL_DETACH
	rtos_sema_delete(dfu_reconf_sema);
#endif
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbd_dfu(void)
{
	int status;
	rtos_task_t task;

	status = rtos_task_create(&task, "example_usbd_dfu_thread", example_usbd_dfu_thread, NULL,
							  USBD_DFU_INIT_THREAD_STACK_SIZE, USBD_DFU_INIT_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
	}
}
