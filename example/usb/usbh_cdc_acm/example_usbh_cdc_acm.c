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
#include "usbh_cdc_acm.h"

/* Private defines -----------------------------------------------------------*/

#define CONFIG_USBH_CDC_ACM_HOT_PLUG_TEST 1      /* Hot plug / memory leak test */
#define CONFIG_USBH_CDC_ACM_STRESS_TEST   0      /* Stress / long run test */
#define CONFIG_USBH_CDC_ACM_CHECK_DATA    0      /* Check TRX data */

#define CONFIG_USBH_CDC_ACM_SPEED_TEST    0      /* Tx Rx fast transfer test */

/**
* @brief Test the CDC ACM Notify(INTR IN transfer)
* @note set CONFIG_USBH_CDC_ACM_NOTIFY=1 in usbh_cdc_acm.h to enable the host support the notify
*       set notify_en in the device side usbd_cdc_acm_config_t to enable the device report the notify
*/
#if CONFIG_USBH_CDC_ACM_NOTIFY
#define USBH_CDC_ACM_NOTIFY_BUF_SIZE      256    /* Buffer size for notify test*/
#endif
#define USBH_CDC_ACM_LOOPBACK_BUF_SIZE    1024   /* Buffer size for loopback test, which should match with device loopback buffer size */
#define USBH_CDC_ACM_LOOPBACK_CNT         100    /* Loopback test round */

/* Poll interval and budget of the "wait until the class is set up" helper.
   Polling rather than blocking on a sema because the three tests run one after
   another in the same thread and each needs the same edge, which a binary sema
   can only hand out once. The delay keeps the wait off the CPU, which matters
   under SMP: a tight spin here can starve the core the USB main task runs on. */
#define USBH_CDC_ACM_READY_POLL_MS        10U
#define USBH_CDC_ACM_READY_TIMEOUT_MS     10000U

/* Budget for one BULK/INTR transfer completion callback. Bounded rather than
   RTOS_SEMA_MAX_COUNT so a device that is unplugged mid-transfer, and therefore
   never completes it, does not park the caller forever holding up the test. */
#define USBH_CDC_ACM_XFER_TIMEOUT_MS      2000U

// Thread Priorities
#define CONFIG_USBH_CDC_ACM_INIT_THREAD_PRIORITY             1U
#define CONFIG_USBH_CDC_ACM_MAIN_TASK_PRIORITY               5U
#define CONFIG_USBH_CDC_ACM_HOTPLUG_THREAD_PRIORITY          6U
#define CONFIG_USBH_CDC_ACM_BULK_XFER_THREAD_PRIORITY        3U
#define CONFIG_USBH_CDC_ACM_NOTIFY_THREAD_PRIORITY           2U

// Thread Stack Sizes
#define CONFIG_USBH_CDC_ACM_INIT_THREAD_STACK_SIZE             1024U
#define CONFIG_USBH_CDC_ACM_MAIN_TASK_STACK_SIZE               768U
#define CONFIG_USBH_CDC_ACM_HOTPLUG_THREAD_STACK_SIZE          1024U
#define CONFIG_USBH_CDC_ACM_BULK_XFER_THREAD_STACK_SIZE        (1024U * 2)
#define CONFIG_USBH_CDC_ACM_NOTIFY_THREAD_STACK_SIZE           1024U

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int cdc_acm_cb_init(void);
static void cdc_acm_cb_deinit(void);
static void cdc_acm_cb_attached(void);
static void cdc_acm_cb_detached(void);
static void cdc_acm_cb_setup(void);
static void cdc_acm_cb_transmitted(u8 status);
static void cdc_acm_cb_received(u8 *buf, u32 len, u8 status);
static void cdc_acm_cb_line_coding_changed(usb_cdc_acm_line_coding_t *line_coding);
static void cdc_acm_cb_process(usb_host_t *host, u8 msg);
#if CONFIG_USBH_CDC_ACM_NOTIFY
static void cdc_acm_cb_notify(u8 *buf, u32 len, u8 status);
static void cdc_acm_notify_test(void);
static void example_usbh_cdc_acm_notify_thread(void *param);
#endif
#if CONFIG_USBH_CDC_ACM_SPEED_TEST
static void cdc_acm_speed_loopback_test(void);
#else
static void cdc_acm_loopback_test(void);
#endif
static void cdc_acm_request_test(void);
static int cdc_acm_lock_stack(void);
static void cdc_acm_unlock_stack(void);
static void cdc_acm_drain_sema(rtos_sema_t sema);
static int cdc_acm_wait_ready(void);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "ACM";

static u8 cdc_acm_loopback_tx_buf[USBH_CDC_ACM_LOOPBACK_BUF_SIZE] USB_DMA_ALIGNED;
static u8 cdc_acm_loopback_rx_buf[USBH_CDC_ACM_LOOPBACK_BUF_SIZE] USB_DMA_ALIGNED;
#if CONFIG_USBH_CDC_ACM_NOTIFY
static u8 cdc_acm_notify_rx_buf[USBH_CDC_ACM_NOTIFY_BUF_SIZE] USB_DMA_ALIGNED;
static rtos_sema_t cdc_acm_notify_sema;
/* Written by the class callback, read by the notify thread, possibly on another core */
static volatile u8 cdc_acm_notify_status;
#endif
static rtos_sema_t cdc_acm_detach_sema;
static rtos_sema_t cdc_acm_attach_sema;
static rtos_sema_t cdc_acm_receive_sema;
static rtos_sema_t cdc_acm_send_sema;

static __IO int cdc_acm_total_rx_len = 0;
static __IO int cdc_acm_is_ready = 0;

/* Serializes every class API call of the worker threads against the hotplug
   thread's deinit/reinit. usbh_cdc_acm_deinit() frees the line-coding buffers
   and drops cdc->host, and usbh_deinit() memset()s the whole host handle; the
   class API only checks host != NULL on entry, so under SMP a worker on core 1
   can be past that check and inside usbh_notify() while core 0 is freeing the
   very tables it walks. The workers must therefore be excluded by lock, not by
   core.
   Created before the stack comes up and never deleted: the worker threads can
   not be joined by the example, so there is no point in time at which this mutex
   is provably unreferenced. cdc_acm_stack_ready is the gate that keeps callers
   off the stack instead. */
static rtos_mutex_t cdc_acm_state_mutex = NULL;

/* 1: usbh_init() and usbh_cdc_acm_init() are both done and the class API may be
   called. Cleared before any teardown, so a non-zero value also implies
   cdc_acm_state_mutex is valid. */
static volatile u8 cdc_acm_stack_ready = 0;

/* Set by the hotplug thread BEFORE it takes the lock, so a worker already inside
   a transfer loop sees it, gives up its round and stops contending. Without that
   ordering a worker would keep re-taking the lock between its own retries and
   the hotplug thread could be starved out of a teardown it must perform. */
static volatile u8 cdc_acm_teardown = 0;

/* Set by the hotplug thread when the stack can not be rebuilt. Tells every
   worker to stop for good rather than spin against a stack that is gone. */
static volatile u8 cdc_acm_stack_fatal = 0;

static const usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
#if CONFIG_USBH_CDC_ACM_NOTIFY
	.ext_intr_enable = USBH_SOF_INTR,
#endif
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = CONFIG_USBH_CDC_ACM_MAIN_TASK_STACK_SIZE,
	.main_task_priority = CONFIG_USBH_CDC_ACM_MAIN_TASK_PRIORITY,
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

static const usbh_cdc_acm_cb_t cdc_acm_usr_cb = {
	.init   = cdc_acm_cb_init,
	.deinit = cdc_acm_cb_deinit,
	.attached = cdc_acm_cb_attached,
	.detached = cdc_acm_cb_detached,
	.setup  = cdc_acm_cb_setup,
	.transmitted = cdc_acm_cb_transmitted,
	.received  = cdc_acm_cb_received,
#if CONFIG_USBH_CDC_ACM_NOTIFY
	.notify   = cdc_acm_cb_notify,
#endif
	.line_coding_changed = cdc_acm_cb_line_coding_changed
};

static const usbh_user_cb_t usbh_usr_cb = {
	.process = cdc_acm_cb_process
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Take the stack lock if the class API may be called right now
  * @note   Gates on cdc_acm_stack_ready BEFORE touching the mutex: the handle is
  *         still NULL if the example never started, and the flag is already
  *         cleared if a teardown is pending or the stack is gone for good. A
  *         hotplug right after this check is harmless - the mutex stays valid and
  *         the re-check under it catches the loss.
  * @retval HAL_OK when the lock is held and the stack is usable, HAL_ERR_HW when
  *         the caller must give up without unlocking
  */
static int cdc_acm_lock_stack(void)
{
	if ((cdc_acm_stack_ready == 0U) || (cdc_acm_teardown != 0U)) {
		return HAL_ERR_HW;
	}

	rtos_mutex_take(cdc_acm_state_mutex, RTOS_MAX_TIMEOUT);

	if ((cdc_acm_stack_ready == 0U) || (cdc_acm_teardown != 0U)) {
		/* Lost the race against a teardown */
		rtos_mutex_give(cdc_acm_state_mutex);
		return HAL_ERR_HW;
	}

	return HAL_OK;
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
static void cdc_acm_drain_sema(rtos_sema_t sema)
{
	while (rtos_sema_take(sema, 0) == RTK_SUCCESS) {
	}
}

/**
  * @brief  Release the stack lock taken by cdc_acm_lock_stack()
  * @note   The lock deliberately does NOT cover the wait for the completion
  *         callback that follows a transmit/receive: the callback never arrives
  *         for a device that was just unplugged, so holding the lock across it
  *         would block the hotplug thread for the whole transfer budget.
  * @retval None
  */
static void cdc_acm_unlock_stack(void)
{
	rtos_mutex_give(cdc_acm_state_mutex);
}

/**
  * @brief  Wait until the class driver has finished its setup phase
  * @note   Bounded, and aborts early on a fatal stack, so a test can not hang
  *         when no device is ever plugged in or the stack failed to come back.
  * @retval HAL_OK once the class is set up, HAL_TIMEOUT on expiry or fatal stack
  */
static int cdc_acm_wait_ready(void)
{
	u32 waited = 0U;

	while (waited < USBH_CDC_ACM_READY_TIMEOUT_MS) {
		if (cdc_acm_stack_fatal != 0U) {
			return HAL_TIMEOUT;
		}
		if ((cdc_acm_is_ready != 0) && (cdc_acm_stack_ready != 0U)) {
			rtos_time_delay_ms(USBH_CDC_ACM_READY_POLL_MS);
			return HAL_OK;
		}
		rtos_time_delay_ms(USBH_CDC_ACM_READY_POLL_MS);
		waited += USBH_CDC_ACM_READY_POLL_MS;
	}

	RTK_LOGS(TAG, RTK_LOG_ERROR, "Wait ready timeout\n");
	return HAL_TIMEOUT;
}

/**
  * @brief  Free the objects shared by the example threads
  * @note   Only called by the last running thread, after the USB stack is fully
  *         deinited, so that no class callback can touch these objects any more.
  *         cdc_acm_state_mutex is deliberately kept, see its declaration.
  * @retval None
  */
static void cdc_acm_free_resource(void)
{
	rtos_sema_delete(cdc_acm_detach_sema);
	cdc_acm_detach_sema = NULL;
	rtos_sema_delete(cdc_acm_attach_sema);
	cdc_acm_attach_sema = NULL;
	rtos_sema_delete(cdc_acm_receive_sema);
	cdc_acm_receive_sema = NULL;
	rtos_sema_delete(cdc_acm_send_sema);
	cdc_acm_send_sema = NULL;
#if CONFIG_USBH_CDC_ACM_NOTIFY
	rtos_sema_delete(cdc_acm_notify_sema);
	cdc_acm_notify_sema = NULL;
#endif
}

static int cdc_acm_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
	return HAL_OK;
}

static void cdc_acm_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DEINIT\n");
}

static void cdc_acm_cb_attached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
	rtos_sema_give(cdc_acm_attach_sema);
}

static void cdc_acm_cb_detached(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
#if CONFIG_USBH_CDC_ACM_HOT_PLUG_TEST
	rtos_sema_give(cdc_acm_detach_sema);
#endif
}

static void cdc_acm_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
	cdc_acm_is_ready = 1;
}

#if CONFIG_USBH_CDC_ACM_NOTIFY
static void cdc_acm_cb_notify(u8 *buf, u32 len, u8 status)
{
	UNUSED(buf);
	UNUSED(len);

	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Notify fail: %d\n", status);
	}
	cdc_acm_notify_status = status;
	rtos_sema_give(cdc_acm_notify_sema);
}
#endif

static void cdc_acm_cb_received(u8 *buf, u32 len, u8 status)
{
	UNUSED(buf);
	if (status == HAL_OK) {
		u16 cdc_acm_bulk_in_mps = usbh_cdc_acm_get_bulk_ep_mps();
		if ((len > 0) && ((cdc_acm_total_rx_len + len) <= USBH_CDC_ACM_LOOPBACK_BUF_SIZE)) {
			//memcpy(cdc_acm_loopback_rx_buf + cdc_acm_total_rx_len, buf, len);
		}
		cdc_acm_total_rx_len += len;
		//ZLP or short packet
		if ((len == 0) || (len % cdc_acm_bulk_in_mps)
			|| ((len % cdc_acm_bulk_in_mps == 0) && (len < USBH_CDC_ACM_LOOPBACK_BUF_SIZE))
			|| (cdc_acm_total_rx_len > USBH_CDC_ACM_LOOPBACK_BUF_SIZE)) {
			cdc_acm_total_rx_len = 0;
			rtos_sema_give(cdc_acm_receive_sema);
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "RX fail: %d\n", status);
	}
}

static void cdc_acm_cb_transmitted(u8 status)
{
	if (status == HAL_OK) {
		/*TX done*/
		rtos_sema_give(cdc_acm_send_sema);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "TX fail: %d\n", status);
	}
}

static void cdc_acm_cb_line_coding_changed(usb_cdc_acm_line_coding_t *line_coding)
{
	UNUSED(line_coding);
}

static void cdc_acm_cb_process(usb_host_t *host, u8 msg)
{
	UNUSED(host);

	switch (msg) {
	case USBH_MSG_DISCONNECTED:
		cdc_acm_is_ready = 0;
		break;

	case USBH_MSG_CONNECTED:
		break;

	default:
		break;
	}
}

#if CONFIG_USBH_CDC_ACM_SPEED_TEST
static u32 cdc_acm_loopback_tx_idx = 0;
static volatile u64 tx_loop_cnt = 0, tx_loop_sub_cnt = 0;
static volatile u64 rx_loop_cnt = 0, rx_loop_sub_cnt = 0;
#define TASK_DUMP_CNT  1
static void example_usbh_bulk_tx_thread(void *param)
{
	u32 i;
	int ret;
	UNUSED(param);
	if (1) {
#if CONFIG_USBH_CDC_ACM_STRESS_TEST
		while (1) {
#endif
			for (i = 0; i < USBH_CDC_ACM_LOOPBACK_CNT;) {
				usb_os_memset((void *)cdc_acm_loopback_tx_buf, (u8)(cdc_acm_loopback_tx_idx & 0xFF), USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
				*((u32 *)cdc_acm_loopback_tx_buf) = cdc_acm_loopback_tx_idx;
				if ((!cdc_acm_is_ready) || (cdc_acm_teardown != 0U) || (cdc_acm_stack_fatal != 0U)) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Device disconnect\n");
					goto tx_exit;
				}

				/* Submit under the lock, then release it before waiting for the
				   completion callback: the callback never comes for a device
				   that was just unplugged, and the hotplug thread must not be
				   blocked for the whole transfer budget. */
				if (cdc_acm_lock_stack() != HAL_OK) {
					goto tx_exit;
				}
				cdc_acm_drain_sema(cdc_acm_send_sema);
				ret = usbh_cdc_acm_transmit(cdc_acm_loopback_tx_buf, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
				cdc_acm_unlock_stack();

				if (ret == HAL_OK) {
					if (rtos_sema_take(cdc_acm_send_sema, USBH_CDC_ACM_XFER_TIMEOUT_MS) != RTK_SUCCESS) {
						/* No completion: the device is gone or wedged. Drop the
						   round and let the checks above decide what to do. */
						continue;
					}
					i++;
					tx_loop_sub_cnt = i;
					cdc_acm_loopback_tx_idx++;
				} else {
					usb_os_delay_us(1);
				}
			}

			tx_loop_sub_cnt = 0;
			tx_loop_cnt ++;
			if ((tx_loop_cnt % TASK_DUMP_CNT) == 0) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Bulk loopback tx test PASS: rx(%u-%u) tx(%u-%u)\n", (u32)rx_loop_cnt, (u32)rx_loop_sub_cnt, (u32)tx_loop_cnt,
						 (u32)tx_loop_sub_cnt);
			}
#if CONFIG_USBH_CDC_ACM_STRESS_TEST
		}
#endif
	}

tx_exit:
	rtos_task_delete(NULL);
}
static void example_usbh_bulk_rx_thread(void *param)
{
	u32 i;
	int ret;
	UNUSED(param);
	if (1) {
#if CONFIG_USBH_CDC_ACM_STRESS_TEST
		while (1) {
#endif
			for (i = 0; i < USBH_CDC_ACM_LOOPBACK_CNT;) {
				usb_os_memset((void *)cdc_acm_loopback_rx_buf, 0, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
				if ((!cdc_acm_is_ready) || (cdc_acm_teardown != 0U) || (cdc_acm_stack_fatal != 0U)) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Device disconnect\n");
					goto rx_exit;
				}

				/* Lock covers the submit only, see the tx thread for why. */
				if (cdc_acm_lock_stack() != HAL_OK) {
					goto rx_exit;
				}
				cdc_acm_drain_sema(cdc_acm_receive_sema);
				ret = usbh_cdc_acm_receive(cdc_acm_loopback_rx_buf, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
				cdc_acm_unlock_stack();

				if (ret == HAL_OK) {
					if (rtos_sema_take(cdc_acm_receive_sema, USBH_CDC_ACM_XFER_TIMEOUT_MS) != RTK_SUCCESS) {
						continue;
					}
					i++;
					rx_loop_sub_cnt = i;
				} else {
					usb_os_delay_us(1);
				}
			}
			rx_loop_sub_cnt = 0;
			rx_loop_cnt ++;
			if ((rx_loop_cnt % TASK_DUMP_CNT) == 0) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Bulk loopback rx test PASS: rx(%u-%u) tx(%u-%u)\n", (u32)rx_loop_cnt, (u32)rx_loop_sub_cnt, (u32)tx_loop_cnt,
						 (u32)tx_loop_sub_cnt);
			}
#if CONFIG_USBH_CDC_ACM_STRESS_TEST
		}
#endif
	}

rx_exit:
	rtos_task_delete(NULL);
}
static void cdc_acm_speed_loopback_test(void)
{
	int ret;
	rtos_task_t tx_task;
	rtos_task_t rx_task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "Wait for device attach\n");

	if (cdc_acm_wait_ready() != HAL_OK) {
		return;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Bulk loopback test start, times:%d, size: %d\n", USBH_CDC_ACM_LOOPBACK_CNT, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);

	ret = rtos_task_create(&rx_task, "usbh_acm_rx_thread", example_usbh_bulk_rx_thread, NULL,
						   CONFIG_USBH_CDC_ACM_BULK_XFER_THREAD_STACK_SIZE, CONFIG_USBH_CDC_ACM_BULK_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create rx_task fail\n");
		return;
	}
	//start two task, one for tx, one for rx
	ret = rtos_task_create(&tx_task, "usbh_acm_tx_thread", example_usbh_bulk_tx_thread, NULL,
						   CONFIG_USBH_CDC_ACM_BULK_XFER_THREAD_STACK_SIZE, CONFIG_USBH_CDC_ACM_BULK_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create tx_task fail\n");
		goto exit_delete_rx_task;
	}

	return;

exit_delete_rx_task:
	rtos_task_delete(rx_task);
}
#else
static void cdc_acm_loopback_test(void)
{
	int i;
	int ret;
#if CONFIG_USBH_CDC_ACM_STRESS_TEST
	u8 j = 0;
#endif

	usb_os_memset((void *)cdc_acm_loopback_tx_buf, 0, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);

	RTK_LOGS(TAG, RTK_LOG_INFO, "Wait for device attach\n");

	if (cdc_acm_wait_ready() != HAL_OK) {
		return;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Bulk loopback test start, times:%d, size: %d\n", USBH_CDC_ACM_LOOPBACK_CNT, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);

#if CONFIG_USBH_CDC_ACM_STRESS_TEST
	while (1) {
		usb_os_memset((void *)cdc_acm_loopback_tx_buf, j, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
#endif
		for (i = 0; i < USBH_CDC_ACM_LOOPBACK_CNT; i++) {
			usb_os_memset((void *)cdc_acm_loopback_rx_buf, 0, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
			if ((!cdc_acm_is_ready) || (cdc_acm_teardown != 0U) || (cdc_acm_stack_fatal != 0U)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Device disconnect\n");
				return;
			}

			/* Submit under the lock, wait outside it: a device unplugged mid
			   transfer never completes it, and the hotplug thread must not be
			   held off for the whole transfer budget. */
			if (cdc_acm_lock_stack() != HAL_OK) {
				return;
			}
			cdc_acm_drain_sema(cdc_acm_send_sema);
			ret = usbh_cdc_acm_transmit(cdc_acm_loopback_tx_buf, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
			cdc_acm_unlock_stack();
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "TX fail: %d\n", ret);
				return;
			}

			if (rtos_sema_take(cdc_acm_send_sema, USBH_CDC_ACM_XFER_TIMEOUT_MS) != RTK_SUCCESS) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "TX timeout\n");
				return;
			}

			if (cdc_acm_lock_stack() != HAL_OK) {
				return;
			}
			cdc_acm_drain_sema(cdc_acm_receive_sema);
			ret = usbh_cdc_acm_receive(cdc_acm_loopback_rx_buf, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
			cdc_acm_unlock_stack();
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "RX fail: %d\n", ret);
				return;
			}

			if (rtos_sema_take(cdc_acm_receive_sema, USBH_CDC_ACM_XFER_TIMEOUT_MS) == RTK_SUCCESS) {
#if CONFIG_USBH_CDC_ACM_CHECK_DATA
				/* Check TRX data*/
				if (!(memcmp(cdc_acm_loopback_rx_buf, cdc_acm_loopback_tx_buf, USBH_CDC_ACM_LOOPBACK_BUF_SIZE) == 0)) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Loopback test FAIL: %d, TX:%d-RX: %d\n", i, cdc_acm_loopback_tx_buf[0], cdc_acm_loopback_rx_buf[0]);
				}
#endif
			} else {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "RX timeout\n");
				return;
			}
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "Bulk loopback test PASS\n");
#if CONFIG_USBH_CDC_ACM_STRESS_TEST
	}
#endif
}
#endif

#if CONFIG_USBH_CDC_ACM_NOTIFY
static void example_usbh_cdc_acm_notify_thread(void *param)
{
	UNUSED(param);
	int ret = HAL_OK;

	if (cdc_acm_wait_ready() != HAL_OK) {
		rtos_task_delete(NULL);
		return;
	}

	if (cdc_acm_lock_stack() == HAL_OK) {
		usbh_cdc_acm_set_control_line_state(0x01U); /* DTR=1, RTS=0 */
		cdc_acm_unlock_stack();
	}
	//wait for set control line finish
	rtos_time_delay_ms(2000);

	int i = 0;
	for (i = 0; i < USBH_CDC_ACM_LOOPBACK_CNT; i++) {
		/* Lock covers the submit only, see cdc_acm_unlock_stack(). */
		if (cdc_acm_lock_stack() != HAL_OK) {
			ret = HAL_ERR_UNKNOWN;
			break;
		}
		cdc_acm_drain_sema(cdc_acm_notify_sema);
		(void)usbh_cdc_acm_notify_receive(cdc_acm_notify_rx_buf, USBH_CDC_ACM_NOTIFY_BUF_SIZE);
		cdc_acm_unlock_stack();

		if (rtos_sema_take(cdc_acm_notify_sema, USBH_CDC_ACM_XFER_TIMEOUT_MS) == RTK_SUCCESS) {
			ret |= cdc_acm_notify_status;
			if (cdc_acm_notify_status == HAL_OK) {
				/*Notify data received, for example, Serial State notification*/
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "Intr rx success(0x%02x 0x%02x)\n", cdc_acm_notify_rx_buf[USB_CDC_NOTIFY_HDR_LEN + 1],
						 cdc_acm_notify_rx_buf[USB_CDC_NOTIFY_HDR_LEN]);
			}
		} else {
			ret = HAL_ERR_UNKNOWN;
			break;
		}
	}
	if (ret) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Intr notify test FAIL\n");
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Intr notify test PASS\n");
	}
	rtos_task_delete(NULL);
}

static void cdc_acm_notify_test(void)
{
	int status;
	rtos_task_t task;
	status = rtos_task_create(&task, "usbh_acm_notify_thread",
							  example_usbh_cdc_acm_notify_thread, NULL,
							  CONFIG_USBH_CDC_ACM_NOTIFY_THREAD_STACK_SIZE, CONFIG_USBH_CDC_ACM_NOTIFY_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create notify thread fail(%d)\n", status);
	}
}
#endif

static void cdc_acm_request_test(void)
{
	int ret;
	usb_cdc_acm_line_coding_t line_coding;
	usb_cdc_acm_line_coding_t new_line_coding;

	RTK_LOGS(TAG, RTK_LOG_INFO, "Wait for device attach\n");

	if (cdc_acm_wait_ready() != HAL_OK) {
		return;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Control test start\n");

	RTK_LOGS(TAG, RTK_LOG_INFO, "GET_LINE_CODING:");
	if (cdc_acm_lock_stack() != HAL_OK) {
		return;
	}
	ret = usbh_cdc_acm_get_line_coding(&line_coding);
	cdc_acm_unlock_stack();
	if (ret == HAL_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "DteRate: %u\nCharFormat: %d\nParityType: %d\nDataBits: %d\n",
				 line_coding.b.dwDteRate, line_coding.b.bCharFormat, line_coding.b.bParityType, line_coding.b.bDataBits);
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "fail\n");
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "SET_LINE_CODING:\nDteRate: 38400\nCharFormat: 0\nParityType: 0\nDataBits: 8\n");
	line_coding.b.dwDteRate = 38400;
	line_coding.b.bCharFormat = LINE_CODING_CHAR_FORMAT_1_STOP_BITS;
	line_coding.b.bParityType = LINE_CODING_PARITY_NO;
	line_coding.b.bDataBits = 8;
	if (cdc_acm_lock_stack() != HAL_OK) {
		return;
	}
	ret = usbh_cdc_acm_set_line_coding(&line_coding);
	cdc_acm_unlock_stack();
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "SET_LINE_CODING fail\n");
	}

	rtos_time_delay_ms(10); // delay for wait set line coding done

	RTK_LOGS(TAG, RTK_LOG_INFO, "GET_LINE_CODING:");
	if (cdc_acm_lock_stack() != HAL_OK) {
		return;
	}
	ret = usbh_cdc_acm_get_line_coding(&new_line_coding);
	cdc_acm_unlock_stack();
	if (ret == HAL_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "DteRate: %u\nCharFormat: %d\nParityType: %d\nDataBits: %d\n",
				 new_line_coding.b.dwDteRate, new_line_coding.b.bCharFormat, new_line_coding.b.bParityType, new_line_coding.b.bDataBits);
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "fail\n");
	}

	if ((new_line_coding.b.dwDteRate == line_coding.b.dwDteRate) &&
		(new_line_coding.b.bCharFormat == line_coding.b.bCharFormat) &&
		(new_line_coding.b.bParityType == line_coding.b.bParityType) &&
		(new_line_coding.b.bDataBits == line_coding.b.bDataBits)) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Ctrl test PASS\n");
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Ctrl test FAIL\n");
	}
}

#if CONFIG_USBH_CDC_ACM_HOT_PLUG_TEST
static void example_usbh_cdc_acm_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(cdc_acm_detach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			rtos_time_delay_ms(100);

			/* Announce the teardown BEFORE contending for the lock, so a worker
			   inside a transfer loop gives up its round instead of re-taking the
			   lock between its own retries and starving this thread out. */
			cdc_acm_teardown = 1U;

			rtos_mutex_take(cdc_acm_state_mutex, RTOS_MAX_TIMEOUT);
			/* Close the gate under the lock: no class API call is in flight here,
			   and any later one gives up on the cleared flag. */
			cdc_acm_stack_ready = 0U;

			/* Clear the setup flag here rather than relying on the DISCONNECTED
			   message: the class is about to be torn down, and a stale 1 left
			   over from this session would make cdc_acm_wait_ready() return
			   immediately after the re-init, before the new device has actually
			   been set up. */
			cdc_acm_is_ready = 0;

			usbh_stop();
			usbh_cdc_acm_deinit();
			usbh_deinit();
			rtos_mutex_give(cdc_acm_state_mutex);

			rtos_time_delay_ms(10);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

			rtos_mutex_take(cdc_acm_state_mutex, RTOS_MAX_TIMEOUT);

			ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
			if (ret != HAL_OK) {
				rtos_mutex_give(cdc_acm_state_mutex);
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBH fail\n");
				break;
			}

			ret = usbh_cdc_acm_init(&cdc_acm_usr_cb);
			if (ret != HAL_OK) {
				usbh_deinit();
				rtos_mutex_give(cdc_acm_state_mutex);
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Init CDC ACM fail\n");
				break;
			}

			/* Re-arm USB TRX after the re-init. */
			usbh_start();

			/* Open the gate as the very last step, so no worker runs against a
			   half-rebuilt stack. Clear teardown only after ready is set, or a
			   worker could slip through seeing neither flag. */
			cdc_acm_stack_ready = 1U;
			cdc_acm_teardown = 0U;
			rtos_mutex_give(cdc_acm_state_mutex);

			RTK_LOGS(TAG, RTK_LOG_INFO, "Reinit done\n");
		}
	}

	/* Either a re-init failed or the loop was asked to stop; in both cases the
	   stack is gone for good. Keep the workers out and wake the ones parked on a
	   completion sema so they observe the flag and quit. The semas are left alive
	   on purpose: a worker may still be about to take one, and freeing them here
	   would pull them out from under it. They are a fixed, bounded set of objects
	   for the lifetime of the firmware, not a leak that grows. */
	cdc_acm_stack_fatal = 1U;
	cdc_acm_stack_ready = 0U;
	cdc_acm_teardown = 1U;
	rtos_sema_give(cdc_acm_send_sema);
	rtos_sema_give(cdc_acm_receive_sema);
	rtos_sema_give(cdc_acm_attach_sema);
#if CONFIG_USBH_CDC_ACM_NOTIFY
	rtos_sema_give(cdc_acm_notify_sema);
#endif
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread exit\n");

	rtos_task_delete(NULL);
}
#endif

static void example_usbh_cdc_acm_thread(void *param)
{
	int ret;
#if CONFIG_USBH_CDC_ACM_HOT_PLUG_TEST
	rtos_task_t task;
#endif

	UNUSED(param);

	/* Created before the stack comes up and never deleted, see the declaration */
	if (cdc_acm_state_mutex == NULL) {
		ret = rtos_mutex_create(&cdc_acm_state_mutex);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Create state mutex fail\n");
			goto error_exit;
		}
	}

	ret = rtos_sema_create(&cdc_acm_detach_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto error_exit;
	}

	ret = rtos_sema_create(&cdc_acm_attach_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto error_exit;
	}

	ret = rtos_sema_create(&cdc_acm_receive_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto error_exit;
	}

	ret = rtos_sema_create(&cdc_acm_send_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto error_exit;
	}
#if CONFIG_USBH_CDC_ACM_NOTIFY
	ret = rtos_sema_create(&cdc_acm_notify_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto error_exit;
	}
#endif
	ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (ret != HAL_OK) {
		goto error_exit;
	}

	ret = usbh_cdc_acm_init(&cdc_acm_usr_cb);  /*0 means use default transfer size, and it can not exceed 65536*/
	if (ret != HAL_OK) {
		usbh_deinit();
		goto error_exit;
	}

	/* All class drivers registered; start USB TRX so enumeration can run. */
	usbh_start();

	/* The stack is fully up: allow the worker threads to call the class API. */
	cdc_acm_stack_ready = 1U;

#if CONFIG_USBH_CDC_ACM_HOT_PLUG_TEST
	ret = rtos_task_create(&task, "usbh_acm_hotplug_thread",
						   example_usbh_cdc_acm_hotplug_thread, NULL,
						   CONFIG_USBH_CDC_ACM_HOTPLUG_THREAD_STACK_SIZE, CONFIG_USBH_CDC_ACM_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		/* The gate is already open, so hold it shut under the lock for this
		   rollback too, exactly as the hotplug thread does. */
		cdc_acm_teardown = 1U;
		rtos_mutex_take(cdc_acm_state_mutex, RTOS_MAX_TIMEOUT);
		cdc_acm_stack_ready = 0U;
		usbh_stop();
		usbh_cdc_acm_deinit();
		usbh_deinit();
		rtos_mutex_give(cdc_acm_state_mutex);
		goto error_exit;
	}
#if defined(CONFIG_SMP)
	/* The USB OTG ISR is delivered on CPU0: the GIC pins every SPI to core 0
	   (ITARGETSR is written to 0x01010101 in arm_gic.c gic_dist_init). Pinning
	   the hotplug/deinit thread to CPU0 puts it on the same core as the ISR, so
	   the local interrupt disables inside the teardown are meaningful again. The
	   worker threads stay unaffined: a pending teardown is excluded from them by
	   cdc_acm_state_mutex, not by core. */
	rtos_task_set_affinity(task, 0);
#endif
#endif

	/* Bounded: a fatal stack, or simply no device ever plugged in, must not park
	   this thread forever. cdc_acm_wait_ready() inside each test re-checks the
	   gate anyway, so a spurious wake costs nothing. */
	if (rtos_sema_take(cdc_acm_attach_sema, USBH_CDC_ACM_READY_TIMEOUT_MS) == RTK_SUCCESS) {
		cdc_acm_request_test();
#if  CONFIG_USBH_CDC_ACM_SPEED_TEST
		cdc_acm_speed_loopback_test();
#else
		cdc_acm_loopback_test();
#endif
#if CONFIG_USBH_CDC_ACM_NOTIFY
		cdc_acm_notify_test();
#endif
	}

	goto example_exit;
error_exit:
	/* Nothing was ever started, so no worker thread exists to race this: the
	   semas can be released here. cdc_acm_state_mutex is deliberately kept, see
	   its declaration. */
	cdc_acm_stack_fatal = 1U;
	cdc_acm_free_resource();
example_exit:
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbh_cdc_acm(void)
{
	int ret;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH CDC ACM demo start\n");

	ret = rtos_task_create(&task, "usbh_acm_thread", example_usbh_cdc_acm_thread, NULL,
						   CONFIG_USBH_CDC_ACM_INIT_THREAD_STACK_SIZE, CONFIG_USBH_CDC_ACM_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
	}
}
