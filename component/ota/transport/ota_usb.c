/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "ota_api.h"
#include "usbd_dfu.h"

/* Use the SoC default VID/PID (ameba_usb.h); override here if a separate DFU identity is needed */
#define OTA_USB_VID                 USB_VID
#define OTA_USB_PID                 USB_PID

#define OTA_USB_WRITE_POLL_MS       10U     /* bwPollTimeout while a block is programmed */
#define OTA_USB_STATUS_WAIT_MS      2000U   /* Max wait for the host to read the final GETSTATUS */
#define OTA_USB_MANIFEST_WAIT_MS    10000U  /* Max wait for the host's zero-length DNLOAD (manifest) */
#define OTA_USB_STOP_DELAY_MS       100U

#define OTA_USB_RECONF_PRIORITY     4
#define OTA_USB_RECONF_STACK_SIZE   768U

/* Values of ota_usb_t.state */
#define OTA_USB_IDLE                0U
#define OTA_USB_RUNNING             1U
#define OTA_USB_OK                  2U
#define OTA_USB_FAIL                3U

/* DNLOAD hand-off between the DFU write task and the OTA task */
typedef struct {
	rtos_sema_t data_sema;      /* write -> read: a block is published, or EOF requested */
	rtos_sema_t consumed_sema;  /* read/finish -> write: the block is processed, or the session ended */
	rtos_sema_t done_sema;      /* finish -> manifest: the result is available */
	rtos_sema_t manifest_sema;  /* manifest -> finish: the host asked for manifestation */
	rtos_mutex_t usb_lock;      /* Serializes USB start/stop with the reconf re-enumeration */
#ifdef CONFIG_USBD_DFU_WILL_DETACH
	rtos_sema_t reconf_sema;
	rtos_task_t reconf_task;
#endif
	u8 *blk_buf;                /* Class block buffer, valid while write() blocks */
	u32 blk_len;
	u32 blk_off;
	volatile u8 state;
	volatile u8 eof;            /* Read returns EOF: aborted, or the host ended the download */
	volatile u8 aborted;        /* ota_usb_abort() ended the session: no final GETSTATUS to wait for */
	volatile u8 usb_on;
	u8 has_blk;                 /* read holds a block not yet acked */
	u8 got_blk;                 /* The session has received a block */
} ota_usb_t;

static int ota_usb_cb_write(u16 block_num, u8 *buf, u32 len);
static void ota_usb_cb_manifest(void);
#ifdef CONFIG_USBD_DFU_CAN_UPLOAD
static int ota_usb_cb_read(u16 block_num, u8 *buf, u32 max_len);
#endif
#ifdef CONFIG_USBD_DFU_WILL_DETACH
static void ota_usb_cb_reconf(void);
#endif

static const usbd_config_t ota_usb_cfg = {
	.isr_priority = INT_PRI_MIDDLE,
	.diag_enable = 1,
	.info = {
		.prod_str = "Realtek DFU Device",
		.vid = OTA_USB_VID,
		.pid = OTA_USB_PID,
	},
#if defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBAD) || defined(CONFIG_AMEBADPLUS)
	.nptx_max_epmis_cnt = 1U,
#elif defined(CONFIG_AMEBAGREEN2)
	/* DFIFO total 1024 DWORD, resv 12 DWORD for DMA addr, EP0 fixed 32 DWORD */
	.rx_fifo_depth  = 980U,
	.ptx_fifo_depth = {0U, 0U, 0U, 0U, 0U,},
#elif defined(CONFIG_RLE1509)
	/*DFIFO total 1024 DWORD, resv 48 DWORD and EP0 fixed 32 DWORD*/
	.rx_fifo_depth  = 944U,
	.ptx_fifo_depth = {0U, 0U, 0U, 0U, 0U,},
#elif defined(CONFIG_AMEBAL2)
	.rx_fifo_depth  = 661U,
	.ptx_fifo_depth = {256U, 16U, 32U, 16U,},
#elif defined(CONFIG_AMEBAPRO3)
	.rx_fifo_depth  = 1664U,
	.ptx_fifo_depth = {256U, 32U, 16U,},
#endif
};

static const usbd_dfu_cb_t ota_usb_dfu_cb = {
	.write          = ota_usb_cb_write,
	.manifest       = ota_usb_cb_manifest,
#ifdef CONFIG_USBD_DFU_CAN_UPLOAD
	.read           = ota_usb_cb_read,
#endif
#ifdef CONFIG_USBD_DFU_WILL_DETACH
	.reconf         = ota_usb_cb_reconf,
#endif
};

static const usbd_dfu_config_t ota_usb_dfu_cfg = {
};

static ota_usb_t ota_usb;

static int ota_usb_cb_write(u16 block_num, u8 *buf, u32 len)
{
	ota_usb_t *usb = &ota_usb;

	if (usb->state != OTA_USB_RUNNING) {
		return (usb->state == OTA_USB_OK) ? HAL_OK : HAL_ERR_HW;
	}
	if ((block_num == 0U) && (usb->got_blk != 0U)) {
		/* The host restarted the download: fail this session, the host retries on the next one */
		usb->eof = 1U;
		rtos_sema_give(usb->data_sema);
		return HAL_ERR_HW;
	}

	usb->got_blk = 1U;
	usb->blk_buf = buf;
	usb->blk_len = len;
	usb->blk_off = 0U;
	rtos_sema_give(usb->data_sema);
	rtos_sema_take(usb->consumed_sema, RTOS_MAX_TIMEOUT);

	return (usb->state == OTA_USB_FAIL) ? HAL_ERR_HW : HAL_OK;
}

#ifdef CONFIG_USBD_DFU_CAN_UPLOAD
/*
 * CONFIG_USBD_DFU_CAN_UPLOAD defaults to y and usbd_dfu_init() then refuses a callback set
 * without read(). Report EOF for every block: the slot being programmed must not be readable,
 * and init has to succeed even when UPLOAD is left enabled in the configuration.
 */
static int ota_usb_cb_read(u16 block_num, u8 *buf, u32 max_len)
{
	UNUSED(block_num);
	UNUSED(buf);
	UNUSED(max_len);

	return 0;
}
#endif

static void ota_usb_cb_manifest(void)
{
	ota_usb_t *usb = &ota_usb;

	if (usb->state == OTA_USB_IDLE) {
		return;
	}
	if (usb->state == OTA_USB_RUNNING) {
		/* The host ended the download before the whole OTA file was received */
		usb->eof = 1U;
		rtos_sema_give(usb->data_sema);
	}
	rtos_sema_take(usb->done_sema, RTOS_MAX_TIMEOUT);
	/* Keep the result for a repeated manifest */
	rtos_sema_give(usb->done_sema);

	if (usb->state == OTA_USB_OK) {
		rtos_sema_give(usb->manifest_sema);
	}
}

static int ota_usb_start(void)
{
	int ret;

	ret = usbd_init(&ota_usb_cfg);
	if (ret != HAL_OK) {
		return ret;
	}
	ret = usbd_dfu_init(&ota_usb_dfu_cb, &ota_usb_dfu_cfg);
	if (ret != HAL_OK) {
		usbd_deinit();
		return ret;
	}
	usbd_dfu_set_write_poll_ms(OTA_USB_WRITE_POLL_MS);
	return HAL_OK;
}

static void ota_usb_stop(void)
{
	usbd_dfu_deinit();
	usbd_deinit();
}

#ifdef CONFIG_USBD_DFU_WILL_DETACH
/*
 * Called from the class reconf task after DFU_DETACH: re-enumerate in DFU mode from another
 * task, since usbd_dfu_deinit() waits for the reconf task.
 */
static void ota_usb_cb_reconf(void)
{
	rtos_sema_give(ota_usb.reconf_sema);
}

static void ota_usb_reconf_thread(void *param)
{
	ota_usb_t *usb = &ota_usb;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(usb->reconf_sema, RTOS_MAX_TIMEOUT) != RTK_SUCCESS) {
			continue;
		}
		rtos_mutex_take(usb->usb_lock, RTOS_MAX_TIMEOUT);
		/* The session may have stopped USB meanwhile */
		if (usb->usb_on != 0U) {
			RTK_LOGI(OTA_TAG, "[TRANSPORT] USB re-enumerating\n");
			ota_usb_stop();
			if (ota_usb_start() != HAL_OK) {
				usb->usb_on = 0U;
				usb->eof = 1U;
				rtos_sema_give(usb->data_sema);
				RTK_LOGE(OTA_TAG, "[TRANSPORT] USB re-enumeration failed\n");
			}
		}
		rtos_mutex_give(usb->usb_lock);
	}
}
#endif

/*
 * Create the OS objects on the first session and keep them: ota_usb_abort() may run from another
 * task at any time, so they must never be freed under it.
 */
static int ota_usb_res_init(void)
{
	ota_usb_t *usb = &ota_usb;

	if (usb->usb_lock != NULL) {
		return OTA_OK;
	}
	if ((rtos_sema_create(&usb->data_sema, 0U, 1U) != RTK_SUCCESS) ||
		(rtos_sema_create(&usb->consumed_sema, 0U, 1U) != RTK_SUCCESS) ||
		(rtos_sema_create(&usb->done_sema, 0U, 1U) != RTK_SUCCESS) ||
		(rtos_sema_create(&usb->manifest_sema, 0U, 1U) != RTK_SUCCESS)) {
		goto fail;
	}
#ifdef CONFIG_USBD_DFU_WILL_DETACH
	if (rtos_sema_create(&usb->reconf_sema, 0U, 1U) != RTK_SUCCESS) {
		goto fail;
	}
	if (rtos_task_create(&usb->reconf_task, "ota_usb_reconf", ota_usb_reconf_thread, NULL,
						 OTA_USB_RECONF_STACK_SIZE, OTA_USB_RECONF_PRIORITY) != RTK_SUCCESS) {
		goto fail;
	}
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* The USB OTG ISR is delivered on CPU0: deinit/init the stack on the same core, so the
	 * local interrupt disable in deinit is meaningful on a multi-core build */
	rtos_task_set_affinity(usb->reconf_task, 0);
#endif
#endif
	if (rtos_mutex_create(&usb->usb_lock) != RTK_SUCCESS) {
		goto fail;
	}
	return OTA_OK;

fail:
#ifdef CONFIG_USBD_DFU_WILL_DETACH
	if (usb->reconf_task != NULL) {
		rtos_task_delete(usb->reconf_task);
		usb->reconf_task = NULL;
	}
	rtos_sema_delete(usb->reconf_sema);
	usb->reconf_sema = NULL;
#endif
	rtos_sema_delete(usb->data_sema);
	rtos_sema_delete(usb->consumed_sema);
	rtos_sema_delete(usb->done_sema);
	rtos_sema_delete(usb->manifest_sema);
	usb->data_sema = NULL;
	usb->consumed_sema = NULL;
	usb->done_sema = NULL;
	usb->manifest_sema = NULL;
	usb->usb_lock = NULL;
	return OTA_ERR;
}

int ota_usb_open(ota_context_t *ctx)
{
	ota_usb_t *usb = &ota_usb;
	int ret = OTA_OK;

	if (ota_usb_res_init() != OTA_OK) {
		RTK_LOGE(OTA_TAG, "[TRANSPORT] USB resource init failed\n");
		return OTA_ERR;
	}

	rtos_mutex_take(usb->usb_lock, RTOS_MAX_TIMEOUT);
	/* Reset the session under usb_lock: a failed reconf of the previous session sets eof and
	 * wakes the read path from the reconf thread, and must not leak into this one */
	while (rtos_sema_take(usb->data_sema, 0U) == RTK_SUCCESS) {}
	while (rtos_sema_take(usb->consumed_sema, 0U) == RTK_SUCCESS) {}
	while (rtos_sema_take(usb->done_sema, 0U) == RTK_SUCCESS) {}
	while (rtos_sema_take(usb->manifest_sema, 0U) == RTK_SUCCESS) {}
	usb->blk_len = 0U;
	usb->blk_off = 0U;
	usb->has_blk = 0U;
	usb->got_blk = 0U;
	usb->eof = 0U;
	usb->aborted = 0U;
	usb->state = OTA_USB_RUNNING;

	if (ota_usb_start() != HAL_OK) {
		usb->state = OTA_USB_IDLE;
		RTK_LOGE(OTA_TAG, "[TRANSPORT] USB DFU start failed\n");
		ret = OTA_ERR;
	} else {
		usb->usb_on = 1U;
		ctx->fd = 0;  /* ota_transport_read() rejects fd -1 (set by ota_transport_init()) */
		RTK_LOGI(OTA_TAG, "[TRANSPORT] USB DFU Run-Time mode, wait for dfu-util\n");
	}
	rtos_mutex_give(usb->usb_lock);

	return ret;
}

void ota_usb_finish(int result)
{
	ota_usb_t *usb = &ota_usb;
	u32 wait_ms;
	u8 st;

	if (usb->state == OTA_USB_IDLE) {
		return;  /* open failed */
	}

	usb->state = (result == OTA_OK) ? OTA_USB_OK : OTA_USB_FAIL;
	/* Release a write() waiting on a block OTA no longer reads, and the manifest */
	rtos_sema_give(usb->consumed_sema);
	rtos_sema_give(usb->done_sema);

	/* Wait for the host's zero-length DNLOAD, but not forever: a host that disappears after the
	 * last data block must not hang the OTA task. Gate the wait on aborted, not on eof, because
	 * eof is also set when the host manifests early while the last block is still committing. */
	if ((usb->state == OTA_USB_OK) && (usb->aborted == 0U) &&
		(rtos_sema_take(usb->manifest_sema, OTA_USB_MANIFEST_WAIT_MS) == RTK_SUCCESS)) {
		for (wait_ms = 0U; wait_ms < OTA_USB_STATUS_WAIT_MS; wait_ms += 10U) {
			st = usbd_dfu_get_state();
			if ((st != USB_DFU_STATE_MANIFEST) && (st != USB_DFU_STATE_MANIFEST_SYNC)) {
				break;
			}
			rtos_time_delay_ms(10U);
		}
		rtos_time_delay_ms(OTA_USB_STOP_DELAY_MS);
	}

	rtos_mutex_take(usb->usb_lock, RTOS_MAX_TIMEOUT);
	if (usb->usb_on != 0U) {
		usb->usb_on = 0U;
		ota_usb_stop();
	}
	rtos_mutex_give(usb->usb_lock);
	usb->state = OTA_USB_IDLE;
}

int ota_usb_read(ota_context_t *ctx, u8 *buf, int len)
{
	ota_usb_t *usb = &ota_usb;
	u32 n;

	UNUSED(ctx);

	if ((usb->has_blk != 0U) && (usb->blk_off >= usb->blk_len)) {
		usb->has_blk = 0U;
		rtos_sema_give(usb->consumed_sema);
	}
	/* The block fields are only valid once data_sema is taken */
	if (usb->has_blk == 0U) {
		if ((rtos_sema_take(usb->data_sema, RTOS_MAX_TIMEOUT) != RTK_SUCCESS) || (usb->eof != 0U)) {
			return 0;
		}
		usb->has_blk = 1U;
	}

	n = MIN((u32)len, usb->blk_len - usb->blk_off);
	_memcpy((void *)buf, (const void *)(usb->blk_buf + usb->blk_off), n);
	usb->blk_off += n;
	return (int)n;
}

void ota_usb_abort(void)
{
	ota_usb_t *usb = &ota_usb;

	if (usb->usb_lock == NULL) {
		return;
	}
	usb->aborted = 1U;
	usb->eof = 1U;
	rtos_sema_give(usb->data_sema);
	rtos_sema_give(usb->manifest_sema);
}
