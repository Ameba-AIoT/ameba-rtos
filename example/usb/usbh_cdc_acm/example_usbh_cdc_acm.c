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
*       set CONFIG_USBD_CDC_ACM_NOTIFY=1 in usbd_cdc_acm.h to enable the device report the notify
*       and set CONFIG_USBD_CDC_ACM_NOTIFY_LOOP_TEST=1 to enable the notify loop many times
*/
#if CONFIG_USBH_CDC_ACM_NOTIFY
#define USBH_CDC_ACM_NOTIFY_BUF_SIZE      256    /* Buffer size for notify test*/
#endif
#define USBH_CDC_ACM_LOOPBACK_BUF_SIZE    1024   /* Buffer size for loopback test, which should match with device loopback buffer size */
#define USBH_CDC_ACM_LOOPBACK_CNT         100    /* Loopback test round */

// Thread Priorities
#define CONFIG_USBH_CDC_ACM_INIT_THREAD_PRIORITY             1U
#define CONFIG_USBH_CDC_ACM_MAIN_TASK_PRIORITY               4U
#define CONFIG_USBH_CDC_ACM_HOTPLUG_THREAD_PRIORITY          3U
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
static int cdc_acm_cb_deinit(void);
static int cdc_acm_cb_attach(void);
static int cdc_acm_cb_detach(void);
static int cdc_acm_cb_setup(void);
static int cdc_acm_cb_transmit(u8 status);
static int cdc_acm_cb_receive(u8 *buf, u32 len, u8 status);
static int cdc_acm_cb_line_coding_changed(usb_cdc_line_coding_t *line_coding);
static int cdc_acm_cb_process(usb_host_t *host, u8 msg);
#if CONFIG_USBH_CDC_ACM_NOTIFY
static int cdc_acm_cb_notify(u8 *buf, u32 len, u8 status);
static void cdc_acm_notify_test(void);
static void example_usbh_cdc_acm_notify_thread(void *param);
#endif
#if CONFIG_USBH_CDC_ACM_SPEED_TEST
static void cdc_acm_speed_loopback_test(void);
#else
static void cdc_acm_loopback_test(void);
#endif
static void cdc_acm_request_test(void);
/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "ACM";

static u8 cdc_acm_loopback_tx_buf[USBH_CDC_ACM_LOOPBACK_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 cdc_acm_loopback_rx_buf[USBH_CDC_ACM_LOOPBACK_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
#if CONFIG_USBH_CDC_ACM_NOTIFY
static u8 cdc_acm_notify_rx_buf[USBH_CDC_ACM_NOTIFY_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
static rtos_sema_t cdc_acm_notify_sema;
static u8 cdc_acm_notify_status;
#endif
static rtos_sema_t cdc_acm_detach_sema;
static rtos_sema_t cdc_acm_attach_sema;
static rtos_sema_t cdc_acm_receive_sema;
static rtos_sema_t cdc_acm_send_sema;

static __IO int cdc_acm_total_rx_len = 0;
static __IO int cdc_acm_is_ready = 0;

static const usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
#if CONFIG_USBH_CDC_ACM_NOTIFY
	.ext_intr_enable = USBH_SOF_INTR,
#endif
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = CONFIG_USBH_CDC_ACM_MAIN_TASK_STACK_SIZE,
	.main_task_priority = CONFIG_USBH_CDC_ACM_MAIN_TASK_PRIORITY,
	.tick_source = USBH_SOF_TICK,
#if defined (CONFIG_AMEBAGREEN2)
	/*FIFO total depth is 1024, reserve 12 for DMA addr*/
	.rx_fifo_depth = 500,
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
	.attach = cdc_acm_cb_attach,
	.detach = cdc_acm_cb_detach,
	.setup  = cdc_acm_cb_setup,
	.transmit = cdc_acm_cb_transmit,
	.receive  = cdc_acm_cb_receive,
#if CONFIG_USBH_CDC_ACM_NOTIFY
	.notify   = cdc_acm_cb_notify,
#endif
	.line_coding_changed = cdc_acm_cb_line_coding_changed
};

static const usbh_user_cb_t usbh_usr_cb = {
	.process = cdc_acm_cb_process
};

/* Private functions ---------------------------------------------------------*/

static int cdc_acm_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
	return HAL_OK;
}

static int cdc_acm_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DEINIT\n");
	return HAL_OK;
}

static int cdc_acm_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
	rtos_sema_give(cdc_acm_attach_sema);
	return HAL_OK;
}

static int cdc_acm_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
#if CONFIG_USBH_CDC_ACM_HOT_PLUG_TEST
	rtos_sema_give(cdc_acm_detach_sema);
#endif
	return HAL_OK;
}

static int cdc_acm_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
	cdc_acm_is_ready = 1;
	return HAL_OK;
}

#if CONFIG_USBH_CDC_ACM_NOTIFY
static int cdc_acm_cb_notify(u8 *buf, u32 len, u8 status)
{
	UNUSED(buf);
	UNUSED(len);

	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Notify fail: %d\n", status);
	}
	cdc_acm_notify_status = status;
	rtos_sema_give(cdc_acm_notify_sema);
	return HAL_OK;
}
#endif

static int cdc_acm_cb_receive(u8 *buf, u32 len, u8 status)
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
	return HAL_OK;
}

static int cdc_acm_cb_transmit(u8 status)
{
	if (status == HAL_OK) {
		/*TX done*/
		rtos_sema_give(cdc_acm_send_sema);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "TX fail: %d\n", status);
	}

	return HAL_OK;
}

static int cdc_acm_cb_line_coding_changed(usb_cdc_line_coding_t *line_coding)
{
	UNUSED(line_coding);
	return HAL_OK;
}

static int cdc_acm_cb_process(usb_host_t *host, u8 msg)
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

	return HAL_OK;
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
				memset(cdc_acm_loopback_tx_buf, (u8)(cdc_acm_loopback_tx_idx & 0xFF), USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
				*((u32 *)cdc_acm_loopback_tx_buf) = cdc_acm_loopback_tx_idx;
				if (!cdc_acm_is_ready) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Device disconnect\n");
					return;
				}

				ret = usbh_cdc_acm_transmit(cdc_acm_loopback_tx_buf, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
				if (ret == HAL_OK) {
					rtos_sema_take(cdc_acm_send_sema, RTOS_SEMA_MAX_COUNT);
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
				memset(cdc_acm_loopback_rx_buf, 0, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
				if (!cdc_acm_is_ready) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Device disconnect\n");
					return;
				}

				ret = usbh_cdc_acm_receive(cdc_acm_loopback_rx_buf, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
				if (ret == HAL_OK) {
					rtos_sema_take(cdc_acm_receive_sema, RTOS_SEMA_MAX_COUNT);
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

	rtos_task_delete(NULL);
}
static void cdc_acm_speed_loopback_test(void)
{
	int ret;
	rtos_task_t tx_task;
	rtos_task_t rx_task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "Wait for device attach\n");

	while (1) {
		if (cdc_acm_is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Bulk loopback test start, times:%d, size: %d\n", USBH_CDC_ACM_LOOPBACK_CNT, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);

	ret = rtos_task_create(&rx_task, "example_usbh_bulk_rx_thread", example_usbh_bulk_rx_thread, NULL,
						   CONFIG_USBH_CDC_ACM_BULK_XFER_THREAD_STACK_SIZE, CONFIG_USBH_CDC_ACM_BULK_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create rx_task fail\n");
	}
	//start two task, one for tx, one for rx
	ret = rtos_task_create(&tx_task, "example_usbh_bulk_tx_thread", example_usbh_bulk_tx_thread, NULL,
						   CONFIG_USBH_CDC_ACM_BULK_XFER_THREAD_STACK_SIZE, CONFIG_USBH_CDC_ACM_BULK_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create tx_task fail\n");
	}
}
#else
static void cdc_acm_loopback_test(void)
{
	int i;
	int ret;
#if CONFIG_USBH_CDC_ACM_STRESS_TEST
	u8 j = 0;
#endif

	memset(cdc_acm_loopback_tx_buf, 0, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);

	RTK_LOGS(TAG, RTK_LOG_INFO, "Wait for device attach\n");

	while (1) {
		if (cdc_acm_is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Bulk loopback test start, times:%d, size: %d\n", USBH_CDC_ACM_LOOPBACK_CNT, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);

#if CONFIG_USBH_CDC_ACM_STRESS_TEST
	while (1) {
		memset(cdc_acm_loopback_tx_buf, j, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
#endif
		for (i = 0; i < USBH_CDC_ACM_LOOPBACK_CNT; i++) {
			memset(cdc_acm_loopback_rx_buf, 0, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
			if (!cdc_acm_is_ready) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Device disconnect\n");
				return;
			}
			ret = usbh_cdc_acm_transmit(cdc_acm_loopback_tx_buf, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "TX fail: %d\n", ret);
				return;
			}

			if (rtos_sema_take(cdc_acm_send_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
				usbh_cdc_acm_receive(cdc_acm_loopback_rx_buf, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
			}

			if (rtos_sema_take(cdc_acm_receive_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
#if CONFIG_USBH_CDC_ACM_CHECK_DATA
				/* Check TRX data*/
				if (!(memcmp(cdc_acm_loopback_rx_buf, cdc_acm_loopback_tx_buf, USBH_CDC_ACM_LOOPBACK_BUF_SIZE) == 0)) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Loopback test FAIL: %d, TX:%d-RX: %d\n", i, cdc_acm_loopback_tx_buf[0], cdc_acm_loopback_rx_buf[0]);
				}
#endif
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

	while (1) {
		if (cdc_acm_is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}

	usbh_cdc_acm_set_control_line_state(0x01U); /* DTR=1, RTS=0 */
	//wait for set control line finish
	rtos_time_delay_ms(2000);

	int i = 0;
	for (i = 0; i < USBH_CDC_ACM_LOOPBACK_CNT; i++) {
		usbh_cdc_acm_notify_receive(cdc_acm_notify_rx_buf, USBH_CDC_ACM_NOTIFY_BUF_SIZE);
		if (rtos_sema_take(cdc_acm_notify_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			ret |= cdc_acm_notify_status;
			if (cdc_acm_notify_status == HAL_OK) {
				/*Notify data received, for example, Serial State notification*/
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "Intr rx success(0x%02x 0x%02x)\n", cdc_acm_notify_rx_buf[9], cdc_acm_notify_rx_buf[8]);
			}
		} else {
			ret = HAL_ERR_UNKNOWN;
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
	status = rtos_task_create(&task, "example_usbh_cdc_acm_notify_thread",
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
	usb_cdc_line_coding_t line_coding;
	usb_cdc_line_coding_t new_line_coding;

	RTK_LOGS(TAG, RTK_LOG_INFO, "Wait for device attach\n");

	while (1) {
		if (cdc_acm_is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Control test start\n");

	RTK_LOGS(TAG, RTK_LOG_INFO, "GET_LINE_CODING:");
	ret = usbh_cdc_acm_get_line_coding(&line_coding);
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
	ret = usbh_cdc_acm_set_line_coding(&line_coding);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "SET_LINE_CODING fail\n");
	}

	rtos_time_delay_ms(10); // delay for wait set line coding done

	RTK_LOGS(TAG, RTK_LOG_INFO, "GET_LINE_CODING:");
	ret = usbh_cdc_acm_get_line_coding(&new_line_coding);
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
			usbh_cdc_acm_deinit();
			usbh_deinit();
			rtos_time_delay_ms(10);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

			ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBH fail\n");
				break;
			}

			ret = usbh_cdc_acm_init(&cdc_acm_usr_cb);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Init CDC ACM fail\n");
				usbh_deinit();
				break;
			}
		}
	}

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

	rtos_sema_create(&cdc_acm_detach_sema, 0U, 1U);
	rtos_sema_create(&cdc_acm_attach_sema, 0U, 1U);
	rtos_sema_create(&cdc_acm_receive_sema, 0U, 1U);
	rtos_sema_create(&cdc_acm_send_sema, 0U, 1U);
#if CONFIG_USBH_CDC_ACM_NOTIFY
	rtos_sema_create(&cdc_acm_notify_sema, 0U, 1U);
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

#if CONFIG_USBH_CDC_ACM_HOT_PLUG_TEST
	ret = rtos_task_create(&task, "example_usbh_cdc_acm_hotplug_thread",
						   example_usbh_cdc_acm_hotplug_thread, NULL,
						   CONFIG_USBH_CDC_ACM_HOTPLUG_THREAD_STACK_SIZE, CONFIG_USBH_CDC_ACM_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		usbh_cdc_acm_deinit();
		usbh_deinit();
		goto error_exit;
	}
#endif

	if (rtos_sema_take(cdc_acm_attach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
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
	rtos_sema_delete(cdc_acm_detach_sema);
	rtos_sema_delete(cdc_acm_attach_sema);
	rtos_sema_delete(cdc_acm_receive_sema);
	rtos_sema_delete(cdc_acm_send_sema);
#if CONFIG_USBH_CDC_ACM_NOTIFY
	rtos_sema_delete(cdc_acm_notify_sema);
#endif
example_exit:
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbh_cdc_acm(void)
{
	int ret;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH CDC ACM demo start\n");

	ret = rtos_task_create(&task, "example_usbh_cdc_acm_thread", example_usbh_cdc_acm_thread, NULL,
						   CONFIG_USBH_CDC_ACM_INIT_THREAD_STACK_SIZE, CONFIG_USBH_CDC_ACM_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
	}
}
