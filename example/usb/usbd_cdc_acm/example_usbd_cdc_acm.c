/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "basic_types.h"
#include "usbd_cdc_acm.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
// while test suspend/resume, hotplug should be disabled
#define CDC_ACM_HOTPLUG                          1

// Echo asynchronously, for transfer size larger than packet size. While fpr
// transfer size less than packet size, the synchronous way is preferred.
#define CDC_ACM_ASYNC_XFER                       0

// Asynchronous transfer size
#define CDC_ACM_ASYNC_BUF_SIZE                   2048U

// Thread priorities
#define CDC_ACM_INIT_THREAD_PRIORITY             5
#define CDC_ACM_HOTPLUG_THREAD_PRIORITY          8
#define CDC_ACM_XFER_THREAD_PRIORITY             5

// Thread stack sizes
#define CDC_ACM_INIT_THREAD_STACK_SIZE           1024U
#define CDC_ACM_HOTPLUG_THREAD_STACK_SIZE        1024U
#define CDC_ACM_XFER_THREAD_STACK_SIZE           1024U

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int cdc_acm_cb_init(void);
static int cdc_acm_cb_deinit(void);
static int cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf);
static int cdc_acm_cb_received(u8 *buf, u32 Len);
static void cdc_acm_cb_status_changed(u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "ACM";

static const usbd_cdc_acm_cb_t cdc_acm_cb = {
	.init = cdc_acm_cb_init,
	.deinit = cdc_acm_cb_deinit,
	.setup = cdc_acm_cb_setup,
	.received = cdc_acm_cb_received,
	.status_changed = cdc_acm_cb_status_changed,
};

static usb_cdc_acm_line_coding_t cdc_acm_line_coding;

static u16 cdc_acm_ctrl_line_state;

/* Class configuration for CDC ACM */
static const usbd_cdc_acm_config_t cdc_acm_class_cfg = {
#if defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RLE1509)
	.bulk_in_addr  = 0x82U,
#else
	.bulk_in_addr  = 0x81U,
#endif
	.bulk_out_addr = 0x02U,
	.intr_in_addr  = 0x83U,
	.bulk_in_xfer_size  = 2048U,
	.bulk_out_xfer_size = 2048U,
	/* Expose the INTR IN endpoint, needed to report SERIAL_STATE */
	.notify_en = 1,
	/* Transmit straight from the application buffer, which must be USB_DMA_ALIGNED */
	.bulk_in_zero_copy = 1,
};

static const usbd_config_t cdc_acm_cfg = {
	.isr_priority = INT_PRI_MIDDLE,
	.info = {
		.prod_str = "Realtek CDC ACM Device",
	},
#if defined(CONFIG_AMEBASMART)
	.nptx_max_epmis_cnt = 1U,
#elif defined(CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 692U,
	.ptx_fifo_depth = {0U, 256U, 32U, 0U, 0U, },
#elif defined(CONFIG_RLE1509)
	.rx_fifo_depth = 656U,
	.ptx_fifo_depth = {0U, 256U, 32U, 0U, 0U, },
#elif defined (CONFIG_AMEBAL2)
	.rx_fifo_depth = 661U,
	.ptx_fifo_depth = {256U, 16U, 32U, 16U, },
#elif defined (CONFIG_AMEBAPRO3)
	/*DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD*/
	.rx_fifo_depth = 1664U,
	.ptx_fifo_depth = {256U, 32U, 16U, },
#endif
};

#if CDC_ACM_ASYNC_XFER
static u32 cdc_acm_xfer_idx;
static u8 cdc_acm_async_xfer_buf[CDC_ACM_ASYNC_BUF_SIZE] USB_DMA_ALIGNED;
static u16 cdc_acm_async_xfer_buf_pos;
static volatile int cdc_acm_async_xfer_busy;
static rtos_sema_t cdc_acm_async_xfer_sema;
#endif

#if CDC_ACM_HOTPLUG
/* Written by the ISR, read by the hotplug thread, possibly on another core */
static volatile u8 cdc_acm_attach_status;
static rtos_sema_t cdc_acm_attach_status_changed_sema;
/* Set by the hotplug thread when the stack can not be recovered, tells the xfer
   thread to quit so that the last thread standing frees the shared objects. */
static volatile u8 cdc_acm_stack_fatal;
#endif

#if CDC_ACM_HOTPLUG && CDC_ACM_ASYNC_XFER
/* Serializes the xfer thread's transmit batches against the hotplug thread's
   deinit/reinit. The hotplug thread sets cdc_acm_teardown (volatile) BEFORE
   taking the lock, so an in-flight xfer sees it, yields the batch and pauses
   until the stack is rebuilt; without that ordering the xfer would wait on a
   flag the hotplug thread cannot set while blocked on the lock (deadlock). */
static rtos_mutex_t cdc_acm_xfer_lock;
static volatile u8 cdc_acm_teardown;
#endif

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Free the objects shared by the example threads
  * @note   Only called by the last running thread, after the USB stack is fully
  *         deinited, so that no ISR callback can touch these objects any more
  * @retval None
  */
static void cdc_acm_free_resource(void)
{
#if CDC_ACM_HOTPLUG
	rtos_sema_delete(cdc_acm_attach_status_changed_sema);
	cdc_acm_attach_status_changed_sema = NULL;
#endif
#if CDC_ACM_HOTPLUG && CDC_ACM_ASYNC_XFER
	rtos_mutex_delete(cdc_acm_xfer_lock);
	cdc_acm_xfer_lock = NULL;
#endif
#if CDC_ACM_ASYNC_XFER
	rtos_sema_delete(cdc_acm_async_xfer_sema);
	cdc_acm_async_xfer_sema = NULL;
#endif
}

/**
  * @brief  Initializes the CDC media layer
  * @param  None
  * @retval Status
  */
static int cdc_acm_cb_init(void)
{
	usb_cdc_acm_line_coding_t *lc = &cdc_acm_line_coding;

	lc->b.dwDteRate = 150000;
	lc->b.bCharFormat = 0x00;
	lc->b.bParityType = 0x00;
	lc->b.bDataBits = 0x08;

#if CDC_ACM_ASYNC_XFER
	cdc_acm_async_xfer_buf_pos = 0;
	cdc_acm_async_xfer_busy = 0;
#endif

	return HAL_OK;
}

/**
  * @brief  DeInitializes the CDC media layer
  * @param  None
  * @retval Status
  */
static int cdc_acm_cb_deinit(void)
{
#if CDC_ACM_ASYNC_XFER
	cdc_acm_async_xfer_buf_pos = 0;
	cdc_acm_async_xfer_busy = 0;
#endif
	return HAL_OK;
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface through this function.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  Buf: RX buffer
  * @param  Len: RX data length (in bytes)
  * @retval Status
  */
static int cdc_acm_cb_received(u8 *buf, u32 len)
{
#if CDC_ACM_ASYNC_XFER
	int ret = HAL_OK;
	if (0 == cdc_acm_async_xfer_busy) {
		if ((cdc_acm_async_xfer_buf_pos + len) > CDC_ACM_ASYNC_BUF_SIZE) {
			len = CDC_ACM_ASYNC_BUF_SIZE - cdc_acm_async_xfer_buf_pos;  // extra data discarded
		}

		usb_os_memcpy((void *)((u32)cdc_acm_async_xfer_buf + cdc_acm_async_xfer_buf_pos), (const void *)buf, len);
		cdc_acm_async_xfer_buf_pos += len;
		if (cdc_acm_async_xfer_buf_pos >= CDC_ACM_ASYNC_BUF_SIZE) {
			cdc_acm_async_xfer_buf_pos = 0;
			rtos_sema_give(cdc_acm_async_xfer_sema);
		}
	} else {
		// RTK_LOGS(TAG, RTK_LOG_WARN, "Busy, discard %dB\n", len);
		ret = HAL_BUSY;
	}

	return ret;
#else
	return usbd_cdc_acm_transmit(buf, len);
#endif
}

/**
  * @brief  Handle the CDC class control requests
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  cmd: Command code
  * @param  buf: Buffer containing command data (request parameters)
  * @param  len: Number of data to be sent (in bytes)
  * @retval Status
  */
static int cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	usb_cdc_acm_line_coding_t *lc = &cdc_acm_line_coding;
	/* Ref USB 2.0 9.2.7: anything not explicitly accepted below is a request error, so
	   the default status makes the core STALL EP0 instead of ACKing the status stage. */
	int ret = HAL_ERR_PARA;

	switch (req->bRequest) {
	case USB_CDC_ACM_SEND_ENCAPSULATED_COMMAND:
	case USB_CDC_ACM_GET_ENCAPSULATED_RESPONSE:
	case USB_CDC_ACM_SET_COMM_FEATURE:
	case USB_CDC_ACM_GET_COMM_FEATURE:
	case USB_CDC_ACM_CLEAR_COMM_FEATURE:
	case USB_CDC_ACM_SEND_BREAK:
		/* Do nothing */
		ret = HAL_OK;
		break;

	case USB_CDC_ACM_SET_LINE_CODING:
		/* Ref CDC PSTN 1.2 Table 17: the Line Coding structure is exactly 7 bytes, any
		   other wLength must not update the cached line coding. */
		if (req->wLength == USB_CDC_ACM_LINE_CODING_SIZE) {
			lc->b.dwDteRate = (u32)(buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24));
			lc->b.bCharFormat = buf[4];
			lc->b.bParityType = buf[5];
			lc->b.bDataBits = buf[6];
			ret = HAL_OK;
		}
		break;

	case USB_CDC_ACM_GET_LINE_CODING:
		buf[0] = (u8)(lc->b.dwDteRate & 0xFF);
		buf[1] = (u8)((lc->b.dwDteRate >> 8) & 0xFF);
		buf[2] = (u8)((lc->b.dwDteRate >> 16) & 0xFF);
		buf[3] = (u8)((lc->b.dwDteRate >> 24) & 0xFF);
		buf[4] = lc->b.bCharFormat;
		buf[5] = lc->b.bParityType;
		buf[6] = lc->b.bDataBits;
		ret = HAL_OK;
		break;

	case USB_CDC_ACM_SET_CONTROL_LINE_STATE:
		/*
		wValue:	wValue, Control Signal Bitmap
				D2-15:	Reserved, 0
				D1:	RTS, 0 - Deactivate, 1 - Activate
				D0:	DTR, 0 - Not Present, 1 - Present
		*/
		cdc_acm_ctrl_line_state = req->wValue;
		if (cdc_acm_ctrl_line_state & 0x01) {
			/* VCOM port activate. A no-op when notify_en is clear, the class rejects it. */
			USB_DIAG(USB_LAYER_APP, USB_EVT_LINK, 0);
			usbd_cdc_acm_notify_serial_state(USB_CDC_ACM_CTRL_DSR | USB_CDC_ACM_CTRL_DCD);
		}
		ret = HAL_OK;
		break;

	default:
		/* Request error, keep the default status */
		break;
	}

	return ret;
}

/**
  * @brief  Handle CDC ACM attach status change notifications from the USB stack
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  old_status: Previous attach status
  * @param  status: New attach status
  * @retval None
  */
static void cdc_acm_cb_status_changed(u8 old_status, u8 status)
{
	/*
	The scenario of state change is as follows:
		Status 0 to 1: Indicates the initialization of the USB device from a cold boot, transitioning it
		to an attached state.
		Status 1 to 2: Represents transition from attached to detached state; for example, when the device
		is hot-plugged out, the host suspends, or the system enters sleep mode.
		Status 2 to 1: Represents transition from detached to attached state; for example, when the device
		is hot-plugged in, performs a remote wakeup, or the host resumes.
	*/
	UNUSED(old_status);

#if CDC_ACM_HOTPLUG
	cdc_acm_attach_status = status;
	rtos_sema_give(cdc_acm_attach_status_changed_sema);
#else
	UNUSED(status);
#endif
}

#if CDC_ACM_HOTPLUG
static void example_usbd_cdc_acm_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(cdc_acm_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (cdc_acm_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");
#if CDC_ACM_HOTPLUG && CDC_ACM_ASYNC_XFER
				/* Set teardown before taking the lock so an in-flight xfer yields
				   instead of spinning on HAL_BUSY while we free the stack. */
				cdc_acm_teardown = 1U;
				rtos_mutex_take(cdc_acm_xfer_lock, RTOS_SEMA_MAX_COUNT);
#endif
				usbd_cdc_acm_deinit();
				usbd_deinit();
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
				ret = usbd_init(&cdc_acm_cfg);
				if (ret != 0) {
					break;
				}
				ret = usbd_cdc_acm_init(&cdc_acm_cb, &cdc_acm_class_cfg);
				if (ret != 0) {
					usbd_deinit();
					break;
				}
#if CDC_ACM_HOTPLUG && CDC_ACM_ASYNC_XFER
				/* Stack rebuilt: release the lock and let xfer resume. */
				cdc_acm_teardown = 0U;
				rtos_mutex_give(cdc_acm_xfer_lock);
#endif
			} else if (cdc_acm_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
			}
		}
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Hotplug thread exit\n");

	/* The stack is fully deinited here, no more ISR callback. */
#if CDC_ACM_HOTPLUG && CDC_ACM_ASYNC_XFER
	/* The lock is still held (we broke out before the give above). Release it
	   FIRST, then wake xfer: xfer checks stack_fatal before ever taking the
	   lock, so once woken it goes straight to freeing the shared objects as the
	   last thread standing — deleting a lock that is still held would be UB. */
	rtos_mutex_give(cdc_acm_xfer_lock);
#endif
#if CDC_ACM_ASYNC_XFER
	/* Notify the xfer thread to quit, it frees the shared objects as the last
	   thread standing. */
	cdc_acm_stack_fatal = 1U;
	rtos_sema_give(cdc_acm_async_xfer_sema);
#else
	cdc_acm_free_resource();
#endif
	rtos_task_delete(NULL);
}
#endif // CONFIG_USBD_MSC_CHECK_USB_STATUS

#if CDC_ACM_ASYNC_XFER
static void example_usbd_cdc_acm_xfer_thread(void *param)
{
	int ret;
	u8 *xfer_buf;
	u32 xfer_len;
#if CDC_ACM_HOTPLUG && CDC_ACM_ASYNC_XFER
	/* Whether cdc_acm_xfer_lock is currently held by this thread, so the batch
	   is released exactly once on every exit path (normal, teardown, fatal). */
	u8 lock_held = 0U;
#endif

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(cdc_acm_async_xfer_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
#if CDC_ACM_HOTPLUG
			if (cdc_acm_stack_fatal != 0U) {
				/* Fatal hand-off: quit without touching the shared stack, the
				   hotplug thread frees the objects as the last thread standing. */
				break;
			}
#endif
#if CDC_ACM_HOTPLUG && CDC_ACM_ASYNC_XFER
			/* Take the batch lock. A pending hotplug (teardown) is checked inside
			   the loop below so we yield promptly instead of holding the lock
			   across a HAL_BUSY retry while the stack is being freed. */
			rtos_mutex_take(cdc_acm_xfer_lock, RTOS_SEMA_MAX_COUNT);
			lock_held = 1U;
#endif
			xfer_len = CDC_ACM_ASYNC_BUF_SIZE;
			xfer_buf = cdc_acm_async_xfer_buf;
			cdc_acm_async_xfer_busy = 1;
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Start xfer(%dB) idx(%d)\n", CDC_ACM_ASYNC_BUF_SIZE, cdc_acm_xfer_idx);
			while (xfer_len > 0) {
#if CDC_ACM_HOTPLUG && CDC_ACM_ASYNC_XFER
				if (cdc_acm_stack_fatal != 0U) {
					/* Reinit failed while we were mid-batch: release the lock and
					   quit; hotplug frees the shared objects. */
					cdc_acm_async_xfer_busy = 0;
					rtos_mutex_give(cdc_acm_xfer_lock);
					lock_held = 0U;
					goto xfer_abort;
				}
				if (cdc_acm_teardown != 0U) {
					/* Recoverable hotplug: yield the batch so deinit/reinit can
					   run, then wait for fresh data on the next attach. */
					cdc_acm_async_xfer_busy = 0;
					rtos_mutex_give(cdc_acm_xfer_lock);
					lock_held = 0U;
					break;
				}
#endif
				if (xfer_len > cdc_acm_class_cfg.bulk_in_xfer_size) {
					ret = usbd_cdc_acm_transmit(xfer_buf, cdc_acm_class_cfg.bulk_in_xfer_size);
					if (ret == HAL_OK) {
						xfer_len -= cdc_acm_class_cfg.bulk_in_xfer_size;
						xfer_buf += cdc_acm_class_cfg.bulk_in_xfer_size;
					} else { // HAL_BUSY
						RTK_LOGS(TAG, RTK_LOG_INFO, "Xfer busy, retry[1]\n");
						rtos_time_delay_us(200);
					}
				} else {
					ret = usbd_cdc_acm_transmit(xfer_buf, xfer_len);
					if (ret == HAL_OK) {
						xfer_len = 0;
						cdc_acm_async_xfer_busy = 0;
						cdc_acm_xfer_idx++;
						RTK_LOGS(TAG, RTK_LOG_DEBUG, "Xfer done\n");
						break;
					} else { // HAL_BUSY
						RTK_LOGS(TAG, RTK_LOG_INFO, "Xfer busy, retry[2]\n");
						rtos_time_delay_us(200);
					}
				}
			}
#if CDC_ACM_HOTPLUG && CDC_ACM_ASYNC_XFER
			if (lock_held) {
				/* Normal completion still holds the lock; the teardown and fatal
				   paths above already released it. */
				rtos_mutex_give(cdc_acm_xfer_lock);
				lock_held = 0U;
			}
#endif
		}
	}

#if CDC_ACM_HOTPLUG
xfer_abort:
	/* The hotplug thread already deinited the stack and quit, free the shared
	   objects here as the last thread standing. */
	cdc_acm_free_resource();
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Xfer thread abort\n");
#endif
	rtos_task_delete(NULL);
}
#endif

static void example_usbd_cdc_acm_thread(void *param)
{
	int ret = 0;
#if CDC_ACM_HOTPLUG
	rtos_task_t check_task;
#endif
#if CDC_ACM_ASYNC_XFER
	rtos_task_t xfer_task;
#endif

	UNUSED(param);

#if CDC_ACM_ASYNC_XFER
	ret = rtos_sema_create(&cdc_acm_async_xfer_sema, 0, 1);
	if (ret != RTK_SUCCESS) {
		goto exit_usbd_init_fail;
	}
#endif

#if CDC_ACM_HOTPLUG
	ret = rtos_sema_create(&cdc_acm_attach_status_changed_sema, 0, 1);
	if (ret != RTK_SUCCESS) {
		goto exit_usbd_init_fail;
	}
#endif

#if CDC_ACM_HOTPLUG && CDC_ACM_ASYNC_XFER
	ret = rtos_mutex_create(&cdc_acm_xfer_lock);
	if (ret != RTK_SUCCESS) {
		goto exit_usbd_init_fail;
	}
#endif

	ret = usbd_init(&cdc_acm_cfg);
	if (ret != HAL_OK) {
		goto exit_usbd_init_fail;
	}

	ret = usbd_cdc_acm_init(&cdc_acm_cb, &cdc_acm_class_cfg);

	if (ret != HAL_OK) {
		goto exit_usbd_cdc_acm_init_fail;
	}

#if CDC_ACM_HOTPLUG
	ret = rtos_task_create(&check_task, "usbd_cdc_acm_hotplug_thread",
						   example_usbd_cdc_acm_hotplug_thread, NULL,
						   CDC_ACM_HOTPLUG_THREAD_STACK_SIZE, CDC_ACM_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto exit_create_check_task_fail;
	}
#if defined(CONFIG_SMP)
	/* C-2: the USB OTG ISR is delivered on CPU0 (GIC ITARGETSR pins every SPI to
	   core 0, see arm_gic.c gic_dist_init). Pinning the hotplug/deinit thread to
	   CPU0 puts it on the same core as the ISR, so deinit's local interrupt
	   disable is meaningful again under SMP. The xfer thread stays unaffined: a
	   pending deinit is excluded from it by cdc_acm_xfer_lock, not by core. */
	rtos_task_set_affinity(check_task, 0);
#endif
#endif

#if CDC_ACM_ASYNC_XFER
	// The priority of transfer thread shall be lower than USB isr priority
	ret = rtos_task_create(&xfer_task, "usbd_cdc_acm_xfer_thread",
						   example_usbd_cdc_acm_xfer_thread, NULL,
						   CDC_ACM_XFER_THREAD_STACK_SIZE, CDC_ACM_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto exit_create_xfer_task_fail;
	}
#endif

	rtos_time_delay_ms(100);

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD CDC ACM demo start\n");

	rtos_task_delete(NULL);

	return;

#if CDC_ACM_ASYNC_XFER
exit_create_xfer_task_fail:
#if CDC_ACM_HOTPLUG
	rtos_task_delete(check_task);
#endif
#endif

#if CDC_ACM_HOTPLUG
exit_create_check_task_fail:
	usbd_cdc_acm_deinit();
#endif

exit_usbd_cdc_acm_init_fail:
	usbd_deinit();

exit_usbd_init_fail:
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD CDC ACM demo aborted\n");
	cdc_acm_free_resource();

	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  USB download de-initialize
  * @param  None
  * @retval Result of the operation: 0 if success else fail
  */
void example_usbd_cdc_acm(void)
{
	int ret;
	rtos_task_t task;

	ret = rtos_task_create(&task, "usbd_cdc_acm_thread", example_usbd_cdc_acm_thread, NULL,
						   CDC_ACM_INIT_THREAD_STACK_SIZE, CDC_ACM_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD CDC ACM thread fail\n");
	}
}
