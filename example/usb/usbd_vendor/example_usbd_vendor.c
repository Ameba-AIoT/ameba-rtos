/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include <platform_autoconf.h>
#include "usbd_vendor.h"

#include "os_wrapper.h"
#include "usb_app.h"
#include "example_usbd_vendor.h"

/* Private defines -----------------------------------------------------------*/

// Endpoint address
#if defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RLE1509)
#define VENDOR_BULK_IN_EP                         0x86U
#define VENDOR_BULK_OUT_EP                        0x03U
#define VENDOR_ISOC_IN_EP                         0x82U
#define VENDOR_ISOC_OUT_EP                        0x02U
#define VENDOR_INTR_IN_EP                         0x84U
#define VENDOR_INTR_OUT_EP                        0x05U
#elif defined (CONFIG_AMEBAL2)
#define VENDOR_BULK_IN_EP                         0x84U
#define VENDOR_BULK_OUT_EP                        0x04U
#define VENDOR_ISOC_IN_EP                         0x81U
#define VENDOR_ISOC_OUT_EP                        0x01U
#define VENDOR_INTR_IN_EP                         0x82U
#define VENDOR_INTR_OUT_EP                        0x02U
#else
#define VENDOR_BULK_IN_EP                         0x81U
#define VENDOR_BULK_OUT_EP                        0x02U
#define VENDOR_ISOC_IN_EP                         0x83U
#define VENDOR_ISOC_OUT_EP                        0x04U
#define VENDOR_INTR_IN_EP                         0x85U
#define VENDOR_INTR_OUT_EP                        0x05U
#endif

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
// while test suspend/resume, hotplug should be disabled
#define VENDOR_HOTPLUG                          1

// Loopback ISOC data in an async thread
#define VENDOR_ISOC_ASYNC_XFER                  0
// Loopback INTR data in an async thread
#define VENDOR_INTR_ASYNC_XFER                  0
// Loopback BULK data in an async thread
#define VENDOR_BULK_ASYNC_XFER                  0
// Any loopback runs in a thread of its own, i.e. outside the USB ISR
#define VENDOR_ASYNC_XFER                       (VENDOR_ISOC_ASYNC_XFER || VENDOR_INTR_ASYNC_XFER || VENDOR_BULK_ASYNC_XFER)
// Number of async xfer threads
#define VENDOR_XFER_THREAD_NUM                  (VENDOR_ISOC_ASYNC_XFER + VENDOR_INTR_ASYNC_XFER + VENDOR_BULK_ASYNC_XFER)

// Thread priorities
#define VENDOR_INIT_THREAD_PRIORITY             5
#define VENDOR_HOTPLUG_THREAD_PRIORITY          8
#define VENDOR_XFER_THREAD_PRIORITY             6

// Thread stack sizes
/* Sized for usbd_get_uuid_sn(): reading the chip UUID costs about 4KB of stack on some SoCs.
 * 1024 is enough for an application that does not call it. */
#define VENDOR_INIT_THREAD_STACK_SIZE           6144U
#define VENDOR_HOTPLUG_THREAD_STACK_SIZE        1024U
#define VENDOR_XFER_THREAD_STACK_SIZE           1024U

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int vendor_cb_init(void);
static int vendor_cb_deinit(void);
static int vendor_cb_setup(usb_setup_req_t *req, u8 *buf);
static int vendor_cb_set_config(void);
static int vendor_cb_bulk_received(u8 *buf, u32 len);
static int vendor_cb_intr_received(u8 *buf, u32 len);
static int vendor_cb_isoc_received(u8 *buf, u32 len);
static void vendor_cb_status_changed(u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "VND";

/* Serial number built from the chip UUID in vendor_start(). Referenced by
   vendor_cfg.info.sn_str, so it has to outlive usbd_deinit(). */
static char vendor_sn[USBD_SN_STR_LEN + 1];

static usbd_config_t vendor_cfg = {
	.info = {
		.prod_str = "Realtek Vendor Device",
		/* .sn_str is filled in at runtime, see vendor_start() */
	},
	.isr_priority = INT_PRI_MIDDLE,
#if defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBAD) || defined(CONFIG_AMEBADPLUS)
	.nptx_max_epmis_cnt = 100U,
	.intr_use_ptx_fifo = 0U,
#elif defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_RLE1509)
	/*DFIFO total 1024 DWORD, resv 12 DWORD for DMA addr and EP0 fixed 32 DWORD*/
	.rx_fifo_depth = 340U,
	.ptx_fifo_depth = {0U, 256U, 0U, 256U, 128U, },
#elif defined (CONFIG_AMEBAL2)
	/*DFIFO total 1024 DWORD, resv 11 DWORD for DMA addr and EP0 fixed 32 DWORD*/
	.rx_fifo_depth = 405U,
	.ptx_fifo_depth = {256U, 256U, 32U, 32U},
#elif defined (CONFIG_AMEBAPRO3)
	/*DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD*/
	.rx_fifo_depth = 1424U,
	.ptx_fifo_depth = {256U, 256U, 32U},
#endif
};

static const usbd_vendor_config_t vendor_class_cfg = {
	.bulk_in_xfer_size = USBD_VENDOR_HS_BULK_MPS,
	.bulk_out_xfer_size = USBD_VENDOR_HS_BULK_MPS,
	.isoc_in_xfer_size = USBD_VENDOR_HS_ISOC_MPS,
	.isoc_out_xfer_size = USBD_VENDOR_HS_ISOC_MPS,
	.intr_in_xfer_size = USBD_VENDOR_HS_INTR_MPS,
	.intr_out_xfer_size = USBD_VENDOR_HS_INTR_MPS,
	.bulk_in_addr  = VENDOR_BULK_IN_EP,
	.bulk_out_addr = VENDOR_BULK_OUT_EP,
	.isoc_in_addr  = VENDOR_ISOC_IN_EP,
	.isoc_out_addr = VENDOR_ISOC_OUT_EP,
	.intr_in_addr  = VENDOR_INTR_IN_EP,
	.intr_out_addr = VENDOR_INTR_OUT_EP,
	/* Every interval field is left 0 to take the class default. */
};

static const usbd_vendor_cb_t vendor_cb = {
	.init = vendor_cb_init,
	.deinit = vendor_cb_deinit,
	.setup = vendor_cb_setup,
	.set_config = vendor_cb_set_config,
	.bulk_received = vendor_cb_bulk_received,
	.isoc_received = vendor_cb_isoc_received,
	.intr_received = vendor_cb_intr_received,
	.status_changed = vendor_cb_status_changed,
};

#if VENDOR_ISOC_ASYNC_XFER
/* Written by the ISR, read by the xfer thread, possibly on another core */
static u8 *volatile vendor_isoc_tx_buf = NULL;
static volatile u32 vendor_isoc_tx_len = 0;
static rtos_sema_t vendor_isoc_async_xfer_sema;
#endif

#if VENDOR_INTR_ASYNC_XFER
/* Written by the ISR, read by the xfer thread, possibly on another core */
static u8 *volatile vendor_intr_tx_buf = NULL;
static volatile u32 vendor_intr_tx_len = 0;
static rtos_sema_t vendor_intr_async_xfer_sema;
#endif

#if VENDOR_BULK_ASYNC_XFER
/* Written by the ISR, read by the xfer thread, possibly on another core */
static u8 *volatile vendor_bulk_tx_buf = NULL;
static volatile u32 vendor_bulk_tx_len = 0;
static rtos_sema_t vendor_bulk_async_xfer_sema;
#endif

#if VENDOR_HOTPLUG
/* Written by the ISR, read by the hotplug thread, possibly on another core */
static volatile u8 vendor_attach_status;
static rtos_sema_t vendor_attach_status_changed_sema;
/* Raised by the hotplug thread when the stack can not be recovered: it is already
   deinited, so the stop owner must not deinit it again. */
static volatile u8 vendor_stack_fatal;
#endif

/* Start/stop state: example_usbd_vendor_stop() (or the hotplug thread once the stack is
   dead) owns the teardown, the worker threads leave through quit and are joined */
static usb_app_ctx_t vendor_app;

#if VENDOR_ASYNC_XFER
/* Serialize the USB stack bring up/tear down against the async xfer threads: each
   of them holds vendor_xfer_lock while it is inside a class API, and the hotplug
   thread takes the lock before it deinits the stack. One lock for all three
   directions: a transmit only arms an EP, so the hold time is short.
   Created once and kept across start/stop: a stop that times out leaves a worker
   that may still take it. vendor_stack_ready is the gate that keeps callers off
   the stack instead. */
static rtos_mutex_t vendor_xfer_lock = NULL;
/* 1: usbd_init() and usbd_vendor_init() both done, the class APIs may be called.
   Cleared before any tear down, so a non-zero value also implies the lock is valid.
   usbd_vendor_deinit() frees the EP xfer buffers that usbd_vendor_transmit_xxx_data()
   memcpy into, and the class internal is_ready check only guards the entry; under SMP
   an xfer thread on the other core can already be past it while the hotplug thread
   deinits, so the threads must be excluded from the whole teardown. */
static volatile u8 vendor_stack_ready;
#endif

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Free the objects shared by the example threads
  * @note   Only called by the teardown owner, after every worker is joined and the
  *         USB stack is fully deinited, so that nothing can touch these objects any more
  * @retval None
  */
static void vendor_free_resource(void)
{
#if VENDOR_HOTPLUG
	rtos_sema_delete(vendor_attach_status_changed_sema);
	vendor_attach_status_changed_sema = NULL;
#endif
#if VENDOR_INTR_ASYNC_XFER
	rtos_sema_delete(vendor_intr_async_xfer_sema);
	vendor_intr_async_xfer_sema = NULL;
#endif
#if VENDOR_ISOC_ASYNC_XFER
	rtos_sema_delete(vendor_isoc_async_xfer_sema);
	vendor_isoc_async_xfer_sema = NULL;
#endif
#if VENDOR_BULK_ASYNC_XFER
	rtos_sema_delete(vendor_bulk_async_xfer_sema);
	vendor_bulk_async_xfer_sema = NULL;
#endif
}

#if VENDOR_ASYNC_XFER
/**
  * @brief  Wake the async xfer threads blocked on their semaphores, so that they
  *         see vendor_app.quit and leave
  * @retval None
  */
static void vendor_wake_xfer_threads(void)
{
#if VENDOR_INTR_ASYNC_XFER
	rtos_sema_give(vendor_intr_async_xfer_sema);
#endif
#if VENDOR_ISOC_ASYNC_XFER
	rtos_sema_give(vendor_isoc_async_xfer_sema);
#endif
#if VENDOR_BULK_ASYNC_XFER
	rtos_sema_give(vendor_bulk_async_xfer_sema);
#endif
}
#endif // VENDOR_ASYNC_XFER

/**
  * @brief  Handle the vendor class control requests
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  *
  *         This demo defines no class/vendor command, so every nonstandard request is
  *         rejected and the USB stack STALLs EP0, as required by USB 2.0 9.2.6.1. Do not
  *         return RTK_SUCCESS for an unhandled request: for a D2H request the class transmits
  *         the whole response window, so accepting a request without filling the window
  *         would return meaningless data to the host.
  *
  *         When adding a real command, validate the full request before touching buf:
  *         bmRequestType (type, direction and recipient), bRequest, wValue, wIndex and
  *         wLength, fill exactly the bytes the command defines, and keep every write
  *         inside the wLength window.
  * @param  req: Setup request packet
  * @param  buf: D2H: zero-initialized response window, wLength bytes at most
  *              H2D: received payload, NULL if the request has no data stage
  * @retval Status
  */
static int vendor_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	UNUSED(req);
	UNUSED(buf);

	return -RTK_ERR_BADARG;
}

/**
  * @brief  Initializes vendor application layer
  * @param  None
  * @retval Status
  */
static int vendor_cb_init(void)
{
	return RTK_SUCCESS;
}

/**
  * @brief  DeInitializes vendor application layer
  * @param  None
  * @retval Status
  */
static int vendor_cb_deinit(void)
{
	return RTK_SUCCESS;
}

/**
  * @brief  Set config callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  None
  * @retval Status
  */
static int vendor_cb_set_config(void)
{
	return RTK_SUCCESS;
}

/**
  * @brief  Data received over USB INTR OUT endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  buf: RX buffer
  * @param  len: RX data length (in bytes)
  * @retval Status
  */
static int vendor_cb_intr_received(u8 *buf, u32 len)
{
#if VENDOR_INTR_ASYNC_XFER
	vendor_intr_tx_buf = buf;
	vendor_intr_tx_len = len;
	rtos_sema_give(vendor_intr_async_xfer_sema);
	return RTK_SUCCESS;
#else
	return usbd_vendor_transmit_intr_data(buf, len);
#endif // VENDOR_INTR_ASYNC_XFER
}

#if VENDOR_INTR_ASYNC_XFER
static void example_usbd_vendor_intr_xfer_thread(void *param)
{
	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(vendor_intr_async_xfer_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (vendor_app.quit != 0U) {
				/* Stop requested or the stack is dead: leave, the stop owner frees the objects */
				break;
			}
			/* Gate on vendor_stack_ready BEFORE touching the mutex: it is still NULL if
			   the loopback got a request before init finished. Then recheck under the
			   lock, the stack may have gone down while waiting for it. */
			if (vendor_stack_ready == 0U) {
				continue;
			}
			rtos_mutex_take(vendor_xfer_lock, RTOS_MAX_TIMEOUT);
			if ((vendor_stack_ready != 0U) && (vendor_intr_tx_buf != NULL) && (vendor_intr_tx_len != 0)) {
				usbd_vendor_transmit_intr_data(vendor_intr_tx_buf, vendor_intr_tx_len);
			}
			rtos_mutex_give(vendor_xfer_lock);
		}
	}
	usb_app_ctx_leave(&vendor_app);
	rtos_task_delete(NULL);
}
#endif // VENDOR_INTR_ASYNC_XFER


/**
  * @brief  Data received over USB ISOC OUT endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  buf: RX buffer
  * @param  len: RX data length (in bytes)
  * @retval Status
  */
static int vendor_cb_isoc_received(u8 *buf, u32 len)
{
#if VENDOR_ISOC_ASYNC_XFER
	vendor_isoc_tx_buf = buf;
	vendor_isoc_tx_len = len;
	rtos_sema_give(vendor_isoc_async_xfer_sema);
	return RTK_SUCCESS;
#else
	return usbd_vendor_transmit_isoc_data(buf, len);
#endif // VENDOR_ISOC_ASYNC_XFER
}

#if VENDOR_ISOC_ASYNC_XFER
static void example_usbd_vendor_isoc_xfer_thread(void *param)
{
	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(vendor_isoc_async_xfer_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (vendor_app.quit != 0U) {
				/* Stop requested or the stack is dead: leave, the stop owner frees the objects */
				break;
			}
			/* See the INTR thread for why the gate comes before the mutex. */
			if (vendor_stack_ready == 0U) {
				continue;
			}
			rtos_mutex_take(vendor_xfer_lock, RTOS_MAX_TIMEOUT);
			if ((vendor_stack_ready != 0U) && (vendor_isoc_tx_buf != NULL) && (vendor_isoc_tx_len != 0)) {
				usbd_vendor_transmit_isoc_data(vendor_isoc_tx_buf, vendor_isoc_tx_len);
			}
			rtos_mutex_give(vendor_xfer_lock);
		}
	}
	usb_app_ctx_leave(&vendor_app);
	rtos_task_delete(NULL);
}
#endif // VENDOR_ISOC_ASYNC_XFER

/**
  * @brief  Data received over USB BULK OUT endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  buf: RX buffer
  * @param  len: RX data length (in bytes)
  * @retval Status
  */
static int vendor_cb_bulk_received(u8 *buf, u32 len)
{
#if VENDOR_BULK_ASYNC_XFER
	vendor_bulk_tx_buf = buf;
	vendor_bulk_tx_len = len;
	rtos_sema_give(vendor_bulk_async_xfer_sema);
	return RTK_SUCCESS;
#else
	return usbd_vendor_transmit_bulk_data(buf, len);
#endif // VENDOR_BULK_ASYNC_XFER
}

#if VENDOR_BULK_ASYNC_XFER
static void example_usbd_vendor_bulk_xfer_thread(void *param)
{
	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(vendor_bulk_async_xfer_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (vendor_app.quit != 0U) {
				/* Stop requested or the stack is dead: leave, the stop owner frees the objects */
				break;
			}
			/* See the INTR thread for why the gate comes before the mutex. */
			if (vendor_stack_ready == 0U) {
				continue;
			}
			rtos_mutex_take(vendor_xfer_lock, RTOS_MAX_TIMEOUT);
			if ((vendor_stack_ready != 0U) && (vendor_bulk_tx_buf != NULL) && (vendor_bulk_tx_len != 0)) {
				usbd_vendor_transmit_bulk_data(vendor_bulk_tx_buf, vendor_bulk_tx_len);
			}
			rtos_mutex_give(vendor_xfer_lock);
		}
	}
	usb_app_ctx_leave(&vendor_app);
	rtos_task_delete(NULL);
}
#endif // VENDOR_BULK_ASYNC_XFER

/**
  * @brief  Handle vendor class attach status change notifications from the USB stack
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  old_status: Previous attach status
  * @param  status: New attach status
  * @retval None
  */
static void vendor_cb_status_changed(u8 old_status, u8 status)
{
	UNUSED(old_status);

#if VENDOR_HOTPLUG
	vendor_attach_status = status;
	rtos_sema_give(vendor_attach_status_changed_sema);
#else
	UNUSED(status);
#endif
}

#if VENDOR_HOTPLUG
/**
  * @brief  Keep the async xfer threads out of the stack while it is torn down and
  *         brought back up, see vendor_xfer_lock. No-op if no loopback runs in a
  *         thread of its own, the ISR callbacks are already serialized against
  *         deinit by its local interrupt disable.
  * @retval None
  */
static void vendor_xfer_lock_acquire(void)
{
#if VENDOR_ASYNC_XFER
	rtos_mutex_take(vendor_xfer_lock, RTOS_MAX_TIMEOUT);
	vendor_stack_ready = 0;
#endif
}

static void vendor_xfer_lock_release(u8 ready)
{
#if VENDOR_ASYNC_XFER
	vendor_stack_ready = ready;
	rtos_mutex_give(vendor_xfer_lock);
#else
	UNUSED(ready);
#endif
}

static void example_usbd_vendor_hotplug_thread(void *param)
{
	int ret = 0;
	u8 fatal = 0U;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(vendor_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (vendor_app.quit != 0U) {
				/* Stop requested: the stack is still up, example_usbd_vendor_stop() tears it down */
				break;
			}
			if (vendor_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");
				vendor_xfer_lock_acquire();
				usbd_vendor_deinit();
				usbd_deinit();
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
				ret = usbd_init(&vendor_cfg);
				if (ret != 0) {
					vendor_xfer_lock_release(0);
					fatal = 1U;
					break;
				}
				ret = usbd_vendor_init(&vendor_cb, &vendor_class_cfg);
				if (ret != 0) {
					usbd_deinit();
					vendor_xfer_lock_release(0);
					fatal = 1U;
					break;
				}
				vendor_xfer_lock_release(1);
			} else if (vendor_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
			}
		}
	}

	if (fatal != 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread fail\n");
		/* The stack is fully deinited here, no more ISR callback: tell the stop owner not to deinit it again */
		vendor_stack_fatal = 1U;
		if (usb_app_ctx_claim_stop(&vendor_app) != 0U) {
			/* No stop is in flight: this thread finishes the teardown on its own */
#if VENDOR_ASYNC_XFER
			vendor_wake_xfer_threads();
			if (usb_app_ctx_join(&vendor_app, VENDOR_XFER_THREAD_NUM) != RTK_SUCCESS) {
				/* An xfer thread may still touch the shared objects: leak them */
				rtos_task_delete(NULL);
			}
#endif
			vendor_free_resource();
			usb_app_ctx_end(&vendor_app);
			rtos_task_delete(NULL);
		}
	}
	usb_app_ctx_leave(&vendor_app);
	rtos_task_delete(NULL);
}
#endif // VENDOR_HOTPLUG

/**
  * @brief  Bring the stack and the worker threads up
  * @retval RTK_SUCCESS, or the first error, with everything already created rolled back
  */
static int vendor_start(void)
{
	int ret;
#if VENDOR_HOTPLUG || VENDOR_ASYNC_XFER
	u32 workers = 0U;
#endif
#if VENDOR_HOTPLUG
	rtos_task_t check_status_task;
#endif
#if VENDOR_ASYNC_XFER
	rtos_task_t xfer_task;
#endif

	ret = usb_app_ctx_begin(&vendor_app);
	if (ret != RTK_SUCCESS) {
		return ret;
	}

#if VENDOR_ASYNC_XFER
	/* Created once and never deleted, see its declaration. */
	if (vendor_xfer_lock == NULL) {
		ret = rtos_mutex_create(&vendor_xfer_lock);
		if (ret != RTK_SUCCESS) {
			goto exit;
		}
	}
#endif

#if VENDOR_HOTPLUG
	vendor_stack_fatal = 0U;
	ret = rtos_sema_create(&vendor_attach_status_changed_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}
#endif

#if VENDOR_INTR_ASYNC_XFER
	vendor_intr_tx_buf = NULL;
	vendor_intr_tx_len = 0;
	ret = rtos_sema_create(&vendor_intr_async_xfer_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}
#endif
#if VENDOR_ISOC_ASYNC_XFER
	vendor_isoc_tx_buf = NULL;
	vendor_isoc_tx_len = 0;
	ret = rtos_sema_create(&vendor_isoc_async_xfer_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}
#endif
#if VENDOR_BULK_ASYNC_XFER
	vendor_bulk_tx_buf = NULL;
	vendor_bulk_tx_len = 0;
	ret = rtos_sema_create(&vendor_bulk_async_xfer_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}
#endif

	/* Report a serial number unique to this chip. Done here rather than inside usbd_init(),
	   because reading the chip UUID needs about 4KB of stack on some SoCs, which the core
	   cannot impose on every caller. Drop these three lines to report no serial number,
	   which is what the other examples do. */
	if (usbd_get_uuid_sn(vendor_sn, sizeof(vendor_sn)) != 0U) {
		vendor_cfg.info.sn_str = vendor_sn;
	}

	ret = usbd_init(&vendor_cfg);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}

	ret = usbd_vendor_init(&vendor_cb, &vendor_class_cfg);
	if (ret != RTK_SUCCESS) {
		goto clear_usb_driver_exit;
	}
#if VENDOR_ASYNC_XFER
	vendor_stack_ready = 1;
#endif

#if VENDOR_INTR_ASYNC_XFER
	// The priority of transfer thread shall be lower than USB isr priority
	ret = rtos_task_create(&xfer_task, "usbd_vendor_intr_xfer_thread",
						   example_usbd_vendor_intr_xfer_thread, NULL,
						   VENDOR_XFER_THREAD_STACK_SIZE, VENDOR_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto clear_task_exit;
	}
	workers++;
#endif // VENDOR_INTR_ASYNC_XFER
#if VENDOR_ISOC_ASYNC_XFER
	// The priority of transfer thread shall be lower than USB isr priority
	ret = rtos_task_create(&xfer_task, "usbd_vendor_isoc_xfer_thread",
						   example_usbd_vendor_isoc_xfer_thread, NULL,
						   VENDOR_XFER_THREAD_STACK_SIZE, VENDOR_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto clear_task_exit;
	}
	workers++;
#endif // VENDOR_ISOC_ASYNC_XFER
#if VENDOR_BULK_ASYNC_XFER
	// The priority of transfer thread shall be lower than USB isr priority
	ret = rtos_task_create(&xfer_task, "usbd_vendor_bulk_xfer_thread",
						   example_usbd_vendor_bulk_xfer_thread, NULL,
						   VENDOR_XFER_THREAD_STACK_SIZE, VENDOR_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto clear_task_exit;
	}
	workers++;
#endif // VENDOR_BULK_ASYNC_XFER

#if VENDOR_HOTPLUG
	/* Created last: it owns the stack once running, so nothing after it can fail */
	ret = rtos_task_create(&check_status_task, "usbd_vendor_hotplug_thread",
						   example_usbd_vendor_hotplug_thread, NULL,
						   VENDOR_HOTPLUG_THREAD_STACK_SIZE, VENDOR_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto clear_task_exit;
	}
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* C-2: the USB OTG ISR is delivered on CPU0 (GIC ITARGETSR pins every SPI to core 0).
	   Pinning the hotplug/deinit thread to CPU0 puts it on the same core as the ISR,
	   so deinit's local interrupt disable is meaningful again under SMP. The async
	   xfer threads stay unaffined: a pending deinit is excluded from them by
	   vendor_xfer_lock, not by core. */
	rtos_task_set_affinity(check_status_task, 0);
#endif
#endif // VENDOR_HOTPLUG

	return RTK_SUCCESS;

#if VENDOR_HOTPLUG || VENDOR_ASYNC_XFER
clear_task_exit:
	/* Wake and join the workers already running before the stack goes away */
	vendor_app.quit = 1U;
#if VENDOR_ASYNC_XFER
	vendor_wake_xfer_threads();
#endif
	if (usb_app_ctx_join(&vendor_app, workers) != RTK_SUCCESS) {
		/* A worker may still run on the stack: keep it, the app stays not idle */
		vendor_app.state = USB_APP_STATE_STOPPING;
		return -RTK_ERR_TIMEOUT;
	}
#endif
#if VENDOR_ASYNC_XFER
	vendor_stack_ready = 0;
#endif
	usbd_vendor_deinit();

clear_usb_driver_exit:
	usbd_deinit();

exit:
	vendor_free_resource();
	usb_app_ctx_end(&vendor_app);

	return ret;
}

static void example_usbd_vendor_thread(void *param)
{
	UNUSED(param);

	if (vendor_start() != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USBD VENDOR demo aborted\n");
	}
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Start the vendor device, used by the USB app manager
  * @note   Task context only, the caller needs about 4KB of stack for usbd_get_uuid_sn()
  * @retval RTK_SUCCESS, -RTK_ERR_BUSY if already running, or the init error
  */
int example_usbd_vendor_start(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD VENDOR demo start\n");

	return vendor_start();
}

/**
  * @brief  Stop the vendor device and release everything it owns
  * @note   Task context only. Safe to call when not running.
  * @retval RTK_SUCCESS once idle, -RTK_ERR_TIMEOUT if a worker did not leave in time
  */
int example_usbd_vendor_stop(void)
{
	if (usb_app_ctx_claim_stop(&vendor_app) == 0U) {
		/* Idle, or the hotplug thread found the stack dead and finishes the teardown itself */
		return usb_app_ctx_wait_idle(&vendor_app);
	}

#if VENDOR_HOTPLUG
	rtos_sema_give(vendor_attach_status_changed_sema);
#endif
#if VENDOR_ASYNC_XFER
	vendor_wake_xfer_threads();
#endif
	if (usb_app_ctx_join(&vendor_app, VENDOR_HOTPLUG + VENDOR_XFER_THREAD_NUM) != RTK_SUCCESS) {
		/* A worker is still on the stack: keep everything, the app stays STOPPING */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stop timeout\n");
		return -RTK_ERR_TIMEOUT;
	}

	/* Every worker is gone: this caller is the only one left on the stack */
#if VENDOR_HOTPLUG
	/* A reinit that failed while this stop was in flight already deinited the stack */
	if (vendor_stack_fatal == 0U)
#endif
	{
#if VENDOR_ASYNC_XFER
		vendor_stack_ready = 0;
#endif
		usbd_vendor_deinit();
		usbd_deinit();
	}
	vendor_free_resource();
	usb_app_ctx_end(&vendor_app);
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD VENDOR demo stop\n");

	return RTK_SUCCESS;
}

/**
  * @brief  Example entry: start the vendor device from its own thread
  * @retval None
  */
void example_usbd_vendor(void)
{
	int ret;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD VENDOR demo start\n");

	ret = rtos_task_create(&task, "usbd_vendor_thread", example_usbd_vendor_thread, NULL,
						   VENDOR_INIT_THREAD_STACK_SIZE, VENDOR_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD VENDOR thread fail\n");
	}
}
