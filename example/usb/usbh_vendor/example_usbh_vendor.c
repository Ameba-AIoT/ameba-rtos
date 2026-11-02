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
#include "usbh_vendor.h"

/* Private defines -----------------------------------------------------------*/

#define CONFIG_USBH_VENDOR_HOT_PLUG_TEST           1/* Hot plug / memory leak test */

#define USBH_VENDOR_BULK_LOOPBACK_CNT              100
#define USBH_VENDOR_INTR_LOOPBACK_CNT              100
#define USBH_VENDOR_ISOC_TEST_CNT                  100
#define USBH_VENDOR_ISOC_DONE_TIMEOUT_MS           2000U

/* Poll interval and budget of the "wait until the class is set up" helper.
   Polling rather than blocking on a sema because three test threads each need
   the same edge, which a binary sema can only hand out once. The delay keeps
   the wait off the CPU, which matters under SMP: a tight spin here can starve
   the core the USB main task runs on. */
#define USBH_VENDOR_READY_POLL_MS                  10U
#define USBH_VENDOR_READY_TIMEOUT_MS               10000U

/* Budget for one BULK/INTR transfer completion callback. Bounded rather than
   RTOS_SEMA_MAX_COUNT so a device unplugged mid-transfer, which therefore never
   completes it, does not park the caller forever. */
#define USBH_VENDOR_XFER_TIMEOUT_MS                2000U

#define USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE         512
#define USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE         1024
#define USBH_VENDOR_ISOC_TEST_BUF_SIZE             1024

// Thread priorities
#define CONFIG_USBH_VENDOR_INIT_THREAD_PRIORITY           1U
#define CONFIG_USBH_VENDOR_MAIN_TASK_PRIORITY             3U
#define CONFIG_USBH_VENDOR_HOTPLUG_THREAD_PRIORITY        6U
#define CONFIG_USBH_VENDOR_TEST_THREAD_PRIORITY           5U
// Thread stack sizes
#define CONFIG_USBH_VENDOR_INIT_THREAD_STACK_SIZE           (1024U * 2)
#define CONFIG_USBH_VENDOR_MAIN_TASK_STACK_SIZE             1024U
#define CONFIG_USBH_VENDOR_HOTPLUG_THREAD_STACK_SIZE        (1024U * 2)
#define CONFIG_USBH_VENDOR_TEST_THREAD_STACK_SIZE           (1024U * 2)

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void vendor_cb_attached(void);
static void vendor_cb_detached(void);
static void vendor_cb_setup(void);
static void vendor_cb_process(usb_host_t *host, u8 msg);
static void vendor_cb_transmitted(u8 ep_type);
static void vendor_cb_received(u8 ep_type, u8 *buf, u32 len, int status);
static int vendor_lock_stack(void);
static void vendor_unlock_stack(void);
static void vendor_drain_sema(rtos_sema_t sema);
static int vendor_wait_ready(void);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "VND";

static u8 vendor_bulk_loopback_tx_buf[USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE] USB_DMA_ALIGNED;
static u8 vendor_bulk_loopback_rx_buf[USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE] USB_DMA_ALIGNED;
static u8 vendor_intr_loopback_tx_buf[USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE] USB_DMA_ALIGNED;
static u8 vendor_intr_loopback_rx_buf[USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE] USB_DMA_ALIGNED;
static u8 vendor_isoc_tx_buf[USBH_VENDOR_ISOC_TEST_BUF_SIZE] USB_DMA_ALIGNED;
static u8 vendor_isoc_rx_buf[USBH_VENDOR_ISOC_TEST_BUF_SIZE] USB_DMA_ALIGNED;

static rtos_sema_t vendor_detach_sema;
static rtos_sema_t vendor_attach_sema;
static rtos_sema_t vendor_bulk_receive_sema;
static rtos_sema_t vendor_bulk_send_sema;
static rtos_sema_t vendor_intr_receive_sema;
static rtos_sema_t vendor_intr_send_sema;
static rtos_sema_t vendor_isoc_txdone_sema;
static rtos_sema_t vendor_isoc_rxdone_sema;
static rtos_sema_t vendor_done_sema;

static __IO int vendor_is_ready = 0;
static __IO int vendor_bulk_total_rx_len = 0;
static __IO int vendor_intr_total_rx_len = 0;

/* Serializes every class API call of the three test threads against the hotplug
   thread's deinit/reinit. usbh_vendor_deinit() closes all pipes and frees the
   per-xfer buffers, and usbh_deinit() memset()s the whole host handle; the class
   API only checks its own state on entry, so under SMP a test thread on core 1
   can be past that check and inside usbh_notify() while core 0 frees the very
   tables it walks. The test threads must be excluded by lock, not by core.
   One lock for all three directions: arming a transfer is short, and the wait for
   the completion callback is deliberately left outside the lock.
   Created before the stack comes up and never deleted: the test threads can not
   be joined by the example, so there is no point in time at which this mutex is
   provably unreferenced. vendor_stack_ready is the gate that keeps callers off
   the stack instead. */
static rtos_mutex_t vendor_state_mutex = NULL;

/* 1: usbh_init() and usbh_vendor_init() are both done and the class API may be
   called. Cleared before any teardown, so a non-zero value also implies
   vendor_state_mutex is valid. */
static volatile u8 vendor_stack_ready = 0;

/* Set by the hotplug thread BEFORE it takes the lock, so a test thread inside a
   loop sees it, gives up its round and stops contending. Without that ordering a
   test thread would keep re-taking the lock between its own rounds and the
   hotplug thread could be starved out of a teardown it must perform. */
static volatile u8 vendor_teardown = 0;

/* Set by the hotplug thread when the stack can not be rebuilt. Tells every test
   thread to stop for good rather than spin against a stack that is gone. */
static volatile u8 vendor_stack_fatal = 0;

/* 1 while the hotplug thread may still touch the semas. Cleared by the thread as
   its very last access, so the bootstrap thread may free them once it reads 0. */
static volatile u8 vendor_hotplug_alive = 0;

static const usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = CONFIG_USBH_VENDOR_MAIN_TASK_STACK_SIZE,
	.main_task_priority = CONFIG_USBH_VENDOR_MAIN_TASK_PRIORITY,
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

static const usbh_vendor_cb_t vendor_usr_cb = {
	.attached = vendor_cb_attached,
	.detached = vendor_cb_detached,
	.setup = vendor_cb_setup,
	.transmitted = vendor_cb_transmitted,
	.received  = vendor_cb_received,
};

static const usbh_user_cb_t usbh_usr_cb = {
	.process = vendor_cb_process
};

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Take the stack lock if the class API may be called right now
  * @note   Gates on vendor_stack_ready BEFORE touching the mutex: the handle is
  *         still NULL if the example never started, and the flag is already
  *         cleared if a teardown is pending or the stack is gone for good. A
  *         hotplug right after this check is harmless - the mutex stays valid and
  *         the re-check under it catches the loss.
  * @retval RTK_SUCCESS when the lock is held and the stack is usable, -RTK_ERR_IO when
  *         the caller must give up without unlocking
  */
static int vendor_lock_stack(void)
{
	if ((vendor_stack_ready == 0U) || (vendor_teardown != 0U)) {
		return -RTK_ERR_IO;
	}

	rtos_mutex_take(vendor_state_mutex, RTOS_MAX_TIMEOUT);

	if ((vendor_stack_ready == 0U) || (vendor_teardown != 0U)) {
		/* Lost the race against a teardown */
		rtos_mutex_give(vendor_state_mutex);
		return -RTK_ERR_IO;
	}

	return RTK_SUCCESS;
}

/**
  * @brief  Release the stack lock taken by vendor_lock_stack()
  * @note   The lock deliberately does NOT cover the wait for the completion
  *         callback that follows a transmit/receive: the callback never arrives
  *         for a device that was just unplugged, so holding the lock across it
  *         would block the hotplug thread for the whole transfer budget.
  * @retval None
  */
static void vendor_unlock_stack(void)
{
	rtos_mutex_give(vendor_state_mutex);
}

/**
  * @brief  Drop any completion left over from an abandoned transfer
  * @note   A bounded wait can time out and still have its callback fire later,
  *         leaving a count on the sema. The next transfer would then take that
  *         stale count instead of its own completion and run one round out of
  *         step with the device. Draining right before arming keeps the sema
  *         meaning "the transfer I just started has finished".
  * @param  sema: Completion sema to drain
  * @retval None
  */
static void vendor_drain_sema(rtos_sema_t sema)
{
	while (rtos_sema_take(sema, 0) == RTK_SUCCESS) {
	}
}

/**
  * @brief  Wait until the class driver has finished its setup phase
  * @note   Bounded, and aborts early on a fatal stack, so a test can not hang
  *         when no device is ever plugged in or the stack failed to come back.
  * @retval RTK_SUCCESS once the class is set up, -RTK_ERR_TIMEOUT on expiry or fatal stack
  */
static int vendor_wait_ready(void)
{
	u32 waited = 0U;

	while (waited < USBH_VENDOR_READY_TIMEOUT_MS) {
		if (vendor_stack_fatal != 0U) {
			return -RTK_ERR_TIMEOUT;
		}
		if ((vendor_is_ready != 0) && (vendor_stack_ready != 0U)) {
			rtos_time_delay_ms(USBH_VENDOR_READY_POLL_MS);
			return RTK_SUCCESS;
		}
		rtos_time_delay_ms(USBH_VENDOR_READY_POLL_MS);
		waited += USBH_VENDOR_READY_POLL_MS;
	}

	RTK_LOGS(TAG, RTK_LOG_ERROR, "Wait ready timeout\n");
	return -RTK_ERR_TIMEOUT;
}

/**
  * @brief  Vendor attach callback
  */
static void vendor_cb_attached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
	rtos_sema_give(vendor_attach_sema);
}

/**
  * @brief  Vendor detach callback
  */
static void vendor_cb_detached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
#if CONFIG_USBH_VENDOR_HOT_PLUG_TEST
	rtos_sema_give(vendor_detach_sema);
#endif
}

/**
  * @brief  Vendor setup callback
  */
static void vendor_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
	vendor_is_ready = 1;
}

/**
  * @brief  Vendor receive callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  ep_type: Endpoint type
  * @param  buf: RX buffer
  * @param  len: RX data length (in bytes)
  * @param  status: Transfer status
  */
static void vendor_cb_received(u8 ep_type, u8 *buf, u32 len, int status)
{
	UNUSED(buf);
	u16 vendor_bulk_in_mps = usbh_vendor_get_bulk_ep_mps();
	u16 vendor_intr_in_mps = usbh_vendor_get_intr_ep_mps();

	switch (ep_type) {
	case USB_CH_EP_TYPE_BULK:
		if (status == RTK_SUCCESS) {
			//limited the copy len
			if ((len > 0) && ((vendor_bulk_total_rx_len + len) <= USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE)) {
				//memcpy(vendor_bulk_loopback_rx_buf + vendor_bulk_total_rx_len, buf, len);
			}
			vendor_bulk_total_rx_len += len;
			//ZLP or short packet
			if ((len == 0) || (len % vendor_bulk_in_mps)
				|| ((len % vendor_bulk_in_mps == 0) && (len < USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE))
				|| (vendor_bulk_total_rx_len > USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE)) {
				vendor_bulk_total_rx_len = 0;
				rtos_sema_give(vendor_bulk_receive_sema);
			}
		}
		//else {
		//	RTK_LOGS(TAG, RTK_LOG_ERROR, "%d RX fail: %d\n", ep_type, status);
		//}

		break;
	case USB_CH_EP_TYPE_INTR:
		if (status == RTK_SUCCESS) {
			//limited the copy len
			if ((len > 0) && ((vendor_intr_total_rx_len + len) <= USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE)) {
				//memcpy(vendor_intr_loopback_rx_buf + vendor_intr_total_rx_len, buf, len);
			}
			vendor_intr_total_rx_len += len;

			//transaction size > 0 and short packet
			if (((len < vendor_intr_in_mps) && (vendor_intr_total_rx_len > 0))
				|| (vendor_intr_total_rx_len >= USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE)) { //
				vendor_intr_total_rx_len = 0;
				rtos_sema_give(vendor_intr_receive_sema);
			}
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "%d RX fail: %d\n", ep_type, status);
		}

		break;
	case USB_CH_EP_TYPE_ISOC:
		rtos_sema_give(vendor_isoc_rxdone_sema);
		break;
	default:
		break;
	}
}

/**
  * @brief  Vendor transmit complete callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  ep_type: Endpoint type
  */
static void vendor_cb_transmitted(u8 ep_type)
{
	switch (ep_type) {
	case USB_CH_EP_TYPE_BULK:
		rtos_sema_give(vendor_bulk_send_sema);
		break;
	case USB_CH_EP_TYPE_INTR:
		rtos_sema_give(vendor_intr_send_sema);
		break;
	case USB_CH_EP_TYPE_ISOC:
		rtos_sema_give(vendor_isoc_txdone_sema);
		break;
	default:
		break;
	}
}

static void vendor_intr_loopback_test(void)
{
	int i, j;
	int ret;
	int vendor_intr_rx_errcnt = 0;

	if (vendor_wait_ready() != RTK_SUCCESS) {
		rtos_sema_give(vendor_done_sema);
		return;
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "INTR loopback test start, times:%d, size: %d\n", USBH_VENDOR_INTR_LOOPBACK_CNT, USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE);

	for (i = 0; i < USBH_VENDOR_INTR_LOOPBACK_CNT; i++) {
		usb_os_memset((void *)vendor_intr_loopback_rx_buf, 0, USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE);
		if ((!vendor_is_ready) || (vendor_teardown != 0U) || (vendor_stack_fatal != 0U)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Device disconnect\n");
			break;
		}

		usb_os_memset((void *)vendor_intr_loopback_tx_buf, i, USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE);

		/* Arm under the lock, then release it before waiting for the completion
		   callback: the callback never comes for a device that was just
		   unplugged, and the hotplug thread must not be blocked that long. */
		if (vendor_lock_stack() != RTK_SUCCESS) {
			break;
		}
		vendor_drain_sema(vendor_intr_send_sema);
		ret = usbh_vendor_intr_transmit(vendor_intr_loopback_tx_buf, USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE, USBH_VENDOR_INTR_LOOPBACK_CNT);
		vendor_unlock_stack();
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR TX start fail: %d\n", ret);
			break;
		}

		if (rtos_sema_take(vendor_intr_send_sema, USBH_VENDOR_XFER_TIMEOUT_MS) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR TX timeout\n");
			break;
		}

		if (vendor_lock_stack() != RTK_SUCCESS) {
			break;
		}
		vendor_drain_sema(vendor_intr_receive_sema);
		ret = usbh_vendor_intr_receive(vendor_intr_loopback_rx_buf, USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE, USBH_VENDOR_INTR_LOOPBACK_CNT);
		vendor_unlock_stack();
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR RX start fail: %d\n", ret);
			break;
		}

		if (rtos_sema_take(vendor_intr_receive_sema, USBH_VENDOR_XFER_TIMEOUT_MS) == RTK_SUCCESS) {
			/*check rx data*/
			for (j = 0; j < USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE; j++) {
				if (vendor_intr_loopback_rx_buf[j] != i) {
					vendor_intr_rx_errcnt++;
					RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR loopback test FAIL: %d, %d, errcnt %d\n", i, vendor_intr_loopback_rx_buf[j], vendor_intr_rx_errcnt);
				}
			}
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR RX timeout\n");
			break;
		}
	}

	rtos_sema_give(vendor_done_sema);
	RTK_LOGS(TAG, RTK_LOG_INFO, "INTR loopback test PASS %d/%d\n", USBH_VENDOR_INTR_LOOPBACK_CNT, (USBH_VENDOR_INTR_LOOPBACK_CNT - vendor_intr_rx_errcnt));
}

static void vendor_bulk_loopback_test(void)
{
	int i, j;
	int ret;
	int vendor_bulk_rx_errcnt = 0;

	if (vendor_wait_ready() != RTK_SUCCESS) {
		rtos_sema_give(vendor_done_sema);
		return;
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "BULK loopback test start, times:%d, size: %d\n", USBH_VENDOR_BULK_LOOPBACK_CNT, USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE);

	for (i = 0; i < USBH_VENDOR_BULK_LOOPBACK_CNT; i++) {
		usb_os_memset((void *)vendor_bulk_loopback_rx_buf, 0, USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE);
		if ((!vendor_is_ready) || (vendor_teardown != 0U) || (vendor_stack_fatal != 0U)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Device disconnect\n");
			break;
		}

		usb_os_memset((void *)vendor_bulk_loopback_tx_buf, i, USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE);

		/* Lock covers the arming only, see the INTR test for why. */
		if (vendor_lock_stack() != RTK_SUCCESS) {
			break;
		}
		vendor_drain_sema(vendor_bulk_send_sema);
		ret = usbh_vendor_bulk_transmit(vendor_bulk_loopback_tx_buf, USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE, USBH_VENDOR_BULK_LOOPBACK_CNT);
		vendor_unlock_stack();
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK TX start fail: %d\n", ret);
			break;
		}

		if (rtos_sema_take(vendor_bulk_send_sema, USBH_VENDOR_XFER_TIMEOUT_MS) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK TX timeout\n");
			break;
		}

		if (vendor_lock_stack() != RTK_SUCCESS) {
			break;
		}
		vendor_drain_sema(vendor_bulk_receive_sema);
		ret = usbh_vendor_bulk_receive(vendor_bulk_loopback_rx_buf, USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE, USBH_VENDOR_BULK_LOOPBACK_CNT);
		vendor_unlock_stack();
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK RX start fail: %d\n", ret);
			break;
		}

		if (rtos_sema_take(vendor_bulk_receive_sema, USBH_VENDOR_XFER_TIMEOUT_MS) == RTK_SUCCESS) {
			/*check rx data*/
			for (j = 0; j < USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE; j++) {
				if (vendor_bulk_loopback_rx_buf[j] != i) {
					vendor_bulk_rx_errcnt++;
					RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK loopback test FAIL: %d, %d, errcnt %d\n", i, vendor_bulk_loopback_rx_buf[j], vendor_bulk_rx_errcnt);
				}
			}
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK RX timeout\n");
			break;
		}
	}

	rtos_sema_give(vendor_done_sema);
	RTK_LOGS(TAG, RTK_LOG_INFO, "BULK loopback test PASS %d/%d\n", USBH_VENDOR_BULK_LOOPBACK_CNT, (USBH_VENDOR_BULK_LOOPBACK_CNT - vendor_bulk_rx_errcnt));
}

static void vendor_isoc_test(void)
{
	int ret;

	if (vendor_wait_ready() != RTK_SUCCESS) {
		goto isoc_exit;
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "ISOC test start, times:%d, size: %d\n", USBH_VENDOR_ISOC_TEST_CNT, USBH_VENDOR_ISOC_TEST_BUF_SIZE);

	/* handle isoc trx flow in class process cb.
	 * ISOC has no handshake or retry, so the done signal only arrives once the
	 * SOF-paced loop count is exhausted. A device without an ISOC endpoint is
	 * rejected by the start call itself, but a device unplugged mid run never
	 * finishes the loop, so a bounded wait - not just the return code - is what
	 * keeps this from blocking forever and stopping the bulk/intr tests from ever
	 * starting.
	 * Both directions are armed under one lock hold: the done semas are only
	 * given after the whole SOF-paced loop finishes, so waiting for them inside
	 * the lock would block a hotplug for the entire ISOC run. */
	if (vendor_lock_stack() != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Device disconnect\n");
		goto isoc_exit;
	}

	vendor_drain_sema(vendor_isoc_txdone_sema);
	vendor_drain_sema(vendor_isoc_rxdone_sema);

	ret = usbh_vendor_isoc_transmit(vendor_isoc_tx_buf, USBH_VENDOR_ISOC_TEST_BUF_SIZE, USBH_VENDOR_ISOC_TEST_CNT);
	if (ret != RTK_SUCCESS) {
		vendor_unlock_stack();
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ISOC TX start fail: %d\n", ret);
		goto isoc_exit;
	}

	ret = usbh_vendor_isoc_receive(vendor_isoc_rx_buf, USBH_VENDOR_ISOC_TEST_BUF_SIZE, USBH_VENDOR_ISOC_TEST_CNT);
	vendor_unlock_stack();
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ISOC RX start fail: %d\n", ret);
		goto isoc_exit;
	}

	if (rtos_sema_take(vendor_isoc_txdone_sema, USBH_VENDOR_ISOC_DONE_TIMEOUT_MS) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ISOC TX timeout\n");
		goto isoc_exit;
	}

	if (rtos_sema_take(vendor_isoc_rxdone_sema, USBH_VENDOR_ISOC_DONE_TIMEOUT_MS) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ISOC RX timeout\n");
		goto isoc_exit;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "ISOC test PASS\n");

isoc_exit:
	/* Every exit must release the counter: example_usbh_vendor_thread() waits for
	 * one give per started test and would otherwise never finish. */
	rtos_sema_give(vendor_done_sema);
}

static void vendor_cb_process(usb_host_t *host, u8 msg)
{
	UNUSED(host);

	switch (msg) {
	case USBH_MSG_DISCONNECTED:
		vendor_is_ready = 0;
		break;

	case USBH_MSG_CONNECTED:
		break;

	default:
		break;
	}
}

#if CONFIG_USBH_VENDOR_HOT_PLUG_TEST
static void example_usbh_vendor_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(vendor_detach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			/* Woken by the bootstrap thread to exit, not by a detach */
			if (vendor_stack_fatal != 0U) {
				break;
			}
			rtos_time_delay_ms(100);//make sure disconnect handle finish before deinit.

			/* Announce the teardown BEFORE contending for the lock, so a test
			   thread inside a loop gives up its round instead of re-taking the
			   lock between rounds and starving this thread out. */
			vendor_teardown = 1U;

			rtos_mutex_take(vendor_state_mutex, RTOS_MAX_TIMEOUT);
			/* Close the gate under the lock: no class API call is in flight here,
			   and any later one gives up on the cleared flag. */
			vendor_stack_ready = 0U;

			/* Clear the setup flag here rather than relying on the DISCONNECTED
			   message: the class is about to be torn down, and a stale 1 left
			   over from this session would make vendor_wait_ready() return
			   immediately after the re-init, before the new device is set up. */
			vendor_is_ready = 0;

			usbh_stop();
			usbh_vendor_deinit();
			usbh_deinit();
			rtos_mutex_give(vendor_state_mutex);

			rtos_time_delay_ms(10);

			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

			rtos_mutex_take(vendor_state_mutex, RTOS_MAX_TIMEOUT);

			ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
			if (ret != RTK_SUCCESS) {
				rtos_mutex_give(vendor_state_mutex);
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Reinit fail: %d\n", ret);
				break;
			}

			ret = usbh_vendor_init(&vendor_usr_cb);
			if (ret != RTK_SUCCESS) {
				usbh_deinit();
				rtos_mutex_give(vendor_state_mutex);
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Reinit fail: %d\n", ret);
				break;
			}

			/* Re-arm USB TRX after the re-init. */
			usbh_start();

			/* Open the gate as the very last step, so no test thread runs against
			   a half-rebuilt stack. Clear teardown only after ready is set, or a
			   thread could slip through seeing neither flag. */
			vendor_stack_ready = 1U;
			vendor_teardown = 0U;
			rtos_mutex_give(vendor_state_mutex);

			RTK_LOGS(TAG, RTK_LOG_INFO, "Reinit done\n");
		}
	}

	/* A re-init failed, so the stack is gone for good. Keep the test threads out
	   and wake the ones parked on a completion sema so they observe the flag,
	   leave their loops and release vendor_done_sema, which the bootstrap thread
	   is counting. The semas are left alive on purpose: a test thread may still
	   be about to take one, and freeing them here would pull them out from under
	   it. They are a fixed, bounded set of objects for the lifetime of the
	   firmware, not a leak that grows. */
	vendor_stack_fatal = 1U;
	vendor_stack_ready = 0U;
	vendor_teardown = 1U;
	rtos_sema_give(vendor_bulk_send_sema);
	rtos_sema_give(vendor_bulk_receive_sema);
	rtos_sema_give(vendor_intr_send_sema);
	rtos_sema_give(vendor_intr_receive_sema);
	rtos_sema_give(vendor_isoc_txdone_sema);
	rtos_sema_give(vendor_isoc_rxdone_sema);
	rtos_sema_give(vendor_attach_sema);
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread exit\n");
	vendor_hotplug_alive = 0U;

	rtos_task_delete(NULL);
}
#endif

static void example_usbh_vendor_bulk_test(void *param)
{
	UNUSED(param);
	vendor_bulk_loopback_test();
	rtos_task_delete(NULL);
}

static void example_usbh_vendor_intr_test(void *param)
{
	UNUSED(param);
	vendor_intr_loopback_test();
	rtos_task_delete(NULL);
}

static void example_usbh_vendor_thread(void *param)
{
	int ret;
#if CONFIG_USBH_VENDOR_HOT_PLUG_TEST
	rtos_task_t task = NULL;
#endif
	int started = 0;
	int i;
#if CONFIG_USBH_VENDOR_HOT_PLUG_TEST
	u32 waited;
#endif

	UNUSED(param);

	/* Created before the stack comes up and never deleted, see the declaration */
	if (vendor_state_mutex == NULL) {
		ret = rtos_mutex_create(&vendor_state_mutex);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create state mutex fail\n");
			goto error_exit;
		}
	}

	ret = rtos_sema_create(&vendor_detach_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto error_exit;
	}

	ret = rtos_sema_create(&vendor_attach_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto error_exit;
	}

	ret = rtos_sema_create(&vendor_intr_receive_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto error_exit;
	}

	ret = rtos_sema_create(&vendor_intr_send_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto error_exit;
	}

	ret = rtos_sema_create(&vendor_bulk_receive_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto error_exit;
	}

	ret = rtos_sema_create(&vendor_bulk_send_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto error_exit;
	}

	ret = rtos_sema_create(&vendor_isoc_txdone_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto error_exit;
	}

	ret = rtos_sema_create(&vendor_isoc_rxdone_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto error_exit;
	}

	ret = rtos_sema_create(&vendor_done_sema, 0U, 3U);
	if (ret != RTK_SUCCESS) {
		goto error_exit;
	}

	ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (ret != RTK_SUCCESS) {
		goto error_exit;
	}

	ret = usbh_vendor_init(&vendor_usr_cb);
	if (ret != RTK_SUCCESS) {
		usbh_deinit();
		goto error_exit;
	}

	/* All class drivers registered; start USB TRX so enumeration can run. */
	usbh_start();

	/* The stack is fully up: allow the test threads to call the class API. */
	vendor_stack_ready = 1U;

#if CONFIG_USBH_VENDOR_HOT_PLUG_TEST
	vendor_hotplug_alive = 1U;
	ret = rtos_task_create(&task, "usbh_vendor_hotplug_thread",
						   example_usbh_vendor_hotplug_thread, NULL,
						   CONFIG_USBH_VENDOR_HOTPLUG_THREAD_STACK_SIZE, CONFIG_USBH_VENDOR_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		vendor_hotplug_alive = 0U;
		/* The gate is already open, so hold it shut under the lock for this
		   rollback too, exactly as the hotplug thread does. */
		vendor_teardown = 1U;
		rtos_mutex_take(vendor_state_mutex, RTOS_MAX_TIMEOUT);
		vendor_stack_ready = 0U;
		usbh_stop();
		usbh_vendor_deinit();
		usbh_deinit();
		rtos_mutex_give(vendor_state_mutex);
		goto error_exit;
	}
#if defined(RTOS_NUM_CORES) && (RTOS_NUM_CORES > 1)
	/* The USB OTG ISR is delivered on CPU0: the GIC pins every SPI to core 0
	   (ITARGETSR is written to 0x01010101 in arm_gic.c gic_dist_init). Pinning
	   the hotplug/deinit thread to CPU0 puts it on the same core as the ISR, so
	   the local interrupt disables inside the teardown are meaningful again. The
	   test threads stay unaffined: a pending teardown is excluded from them by
	   vendor_state_mutex, not by core. */
	rtos_task_set_affinity(task, 0);
#endif
#endif

	/* Bounded: a fatal stack, or simply no device ever plugged in, must not park
	   this thread forever. Each test re-checks the gate through
	   vendor_wait_ready() anyway, so a spurious wake costs nothing. */
	if (rtos_sema_take(vendor_attach_sema, USBH_VENDOR_READY_TIMEOUT_MS) == RTK_SUCCESS) {
		/* Count the tests that actually got started, and wait for exactly that
		   many below. Waiting for a fixed count regardless
		   would hang here when no device attached or a thread failed to spawn,
		   because nobody is left to give the missing counts. */
		vendor_isoc_test();
		started++;

		ret = rtos_task_create(NULL, "usbh_vendor_bulk_test_thread", example_usbh_vendor_bulk_test, NULL,
							   CONFIG_USBH_VENDOR_TEST_THREAD_STACK_SIZE, CONFIG_USBH_VENDOR_TEST_THREAD_PRIORITY);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create bulk test thread fail\n");
			goto deinit_exit;
		}
		started++;

		ret = rtos_task_create(NULL, "usbh_vendor_intr_test_thread", example_usbh_vendor_intr_test, NULL,
							   CONFIG_USBH_VENDOR_TEST_THREAD_STACK_SIZE, CONFIG_USBH_VENDOR_TEST_THREAD_PRIORITY);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create intr test thread fail\n");
			goto deinit_exit;
		}
		started++;
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Wait attach timeout\n");
	}

	/* Bounded as well: a test thread killed by a stack teardown may never reach
	   its give, and the example must still be able to report and exit. */
	for (i = 0; i < started; i++) {
		if (rtos_sema_take(vendor_done_sema, USBH_VENDOR_READY_TIMEOUT_MS) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Wait test done timeout\n");
			break;
		}
	}

	if (started != 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "USBH VENDOR demo done\n");
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USBH VENDOR demo aborted\n");
	}
	goto example_exit;

deinit_exit:
	/* Close the gate before anything else, so a test thread that did get created
	   leaves its loop instead of running against the stack being torn down here.
	   Set outside the lock on purpose: a thread already inside a class API holds
	   the lock, and this flag is what makes it stop asking for it again. */
	vendor_stack_fatal = 1U;
	vendor_teardown = 1U;

	/* Never delete a running thread: it may hold vendor_state_mutex, which a
	   deleted task never gives back, and the take below would block forever.
	   Each started test sees the flags, leaves within one transfer timeout and
	   gives vendor_done_sema. If one does not, it may still use the semas and
	   the stack, so leave both alone. */
	for (i = 0; i < started; i++) {
		if (rtos_sema_take(vendor_done_sema, USBH_VENDOR_READY_TIMEOUT_MS) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Wait test exit timeout\n");
			goto example_exit;
		}
	}

#if CONFIG_USBH_VENDOR_HOT_PLUG_TEST
	/* Wake the hotplug thread so it sees vendor_stack_fatal and exits, and wait
	   for it before the semaphores it uses are freed. */
	rtos_sema_give(vendor_detach_sema);
	waited = 0U;
	while ((vendor_hotplug_alive != 0U) && (waited < USBH_VENDOR_READY_TIMEOUT_MS)) {
		rtos_time_delay_ms(USBH_VENDOR_READY_POLL_MS);
		waited += USBH_VENDOR_READY_POLL_MS;
	}
	if (vendor_hotplug_alive != 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Wait hotplug exit timeout\n");
		goto example_exit;
	}
#endif
	rtos_mutex_take(vendor_state_mutex, RTOS_MAX_TIMEOUT);
	vendor_stack_ready = 0U;
	usbh_stop();
	usbh_vendor_deinit();
	usbh_deinit();
	rtos_mutex_give(vendor_state_mutex);

error_exit:
	rtos_sema_delete(vendor_detach_sema);
	rtos_sema_delete(vendor_attach_sema);
	rtos_sema_delete(vendor_intr_receive_sema);
	rtos_sema_delete(vendor_intr_send_sema);
	rtos_sema_delete(vendor_bulk_receive_sema);
	rtos_sema_delete(vendor_bulk_send_sema);
	rtos_sema_delete(vendor_isoc_rxdone_sema);
	rtos_sema_delete(vendor_isoc_txdone_sema);
	rtos_sema_delete(vendor_done_sema);
	RTK_LOGS(TAG, RTK_LOG_ERROR, "USBH VENDOR demo aborted\n");
example_exit:
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbh_vendor(void)
{
	int ret;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH VENDOR demo start\n");

	ret = rtos_task_create(&task, "usbh_vendor_thread", example_usbh_vendor_thread, NULL,
						   CONFIG_USBH_VENDOR_INIT_THREAD_STACK_SIZE, CONFIG_USBH_VENDOR_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBH VENDOR thread fail\n");
	}
}
