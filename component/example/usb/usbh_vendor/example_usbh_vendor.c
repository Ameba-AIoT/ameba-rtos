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
#include "usbh_vendor.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/
#define CONFIG_USBH_VENDOR_HOT_PLUG_TEST           1/* Hot plug / memory leak test */

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#define UBSH_VENDOR_TEST_TASK_CNT                  3
#define USBH_VENDOR_BULK_LOOPBACK_CNT              100
#define USBH_VENDOR_INTR_LOOPBACK_CNT              100
#define USBH_VENDOR_ISOC_TEST_CNT                  100

#define USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE         512
#define USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE         1024
#define USBH_VENDOR_ISOC_TEST_BUF_SIZE             1024
/* Private function prototypes -----------------------------------------------*/

static int vendor_cb_attach(void);
static int vendor_cb_detach(void);
static int vendor_cb_setup(void);
static int vendor_cb_process(usb_host_t *host, u8 id);
static int vendor_cb_transmit(u8 ep_type);
static int vendor_cb_receive(u8 ep_type, u8 *buf, u32 Len);
/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "VND";

static u8 vendor_bulk_loopback_tx_buf[USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 vendor_bulk_loopback_rx_buf[USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 vendor_intr_loopback_tx_buf[USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 vendor_intr_loopback_rx_buf[USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 vendor_isoc_tx_buf[USBH_VENDOR_ISOC_TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 vendor_isoc_rx_buf[USBH_VENDOR_ISOC_TEST_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

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

static usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.dma_enable = 1U,
	.ext_intr_en = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.isr_task_priority  = 4U,
	.main_task_priority = 3U,
	.sof_tick_en = 1U,

#if defined (CONFIG_AMEBAGREEN2)
	/*FIFO total depth is 1024, reserve 12 for DMA addr*/
	.rx_fifo_depth = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBASMARTPLUS)
	/*FIFO total depth is 1280 DWORD, reserve 14 DWORD for DMA addr*/
	.rx_fifo_depth = 754,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBAL2)
	/*FIFO total depth is 1024 DWORD, reserve 11 DWORD for DMA addr*/
	.rx_fifo_depth = 501,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#endif
};

static usbh_vendor_cb_t vendor_usr_cb = {
	.attach = vendor_cb_attach,
	.detach = vendor_cb_detach,
	.setup = vendor_cb_setup,
	.transmit = vendor_cb_transmit,
	.receive  = vendor_cb_receive,
};

static usbh_user_cb_t usbh_usr_cb = {
	.process = vendor_cb_process
};

/* Private functions ---------------------------------------------------------*/

static int vendor_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
	rtos_sema_give(vendor_attach_sema);
	return HAL_OK;
}

static int vendor_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
#if CONFIG_USBH_VENDOR_HOT_PLUG_TEST
	rtos_sema_give(vendor_detach_sema);
#endif
	return HAL_OK;
}

static int vendor_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
	vendor_is_ready = 1;
	return HAL_OK;
}

static int vendor_cb_receive(u8 ep_type, u8 *buf, u32 len)
{
	switch (ep_type) {
	case USB_CH_EP_TYPE_BULK:
		//limited the copy len
		if ((len > 0) && ((vendor_bulk_total_rx_len + len) <= USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE)) {
			memcpy(vendor_bulk_loopback_rx_buf + vendor_bulk_total_rx_len, buf, len);
		}
		vendor_bulk_total_rx_len += len;

		//transaction size > 0 and short packet
		if ((len == 0) || ((len < usbh_vendor_get_bulk_ep_mps()) && (vendor_bulk_total_rx_len > 0))
			|| (vendor_bulk_total_rx_len > USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE)) { //
			vendor_bulk_total_rx_len = 0;
			rtos_sema_give(vendor_bulk_receive_sema);
		}
		break;
	case USB_CH_EP_TYPE_INTR:
		//limited the copy len
		if ((len > 0) && ((vendor_intr_total_rx_len + len) <= USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE)) {
			memcpy(vendor_intr_loopback_rx_buf + vendor_intr_total_rx_len, buf, len);
		}
		vendor_intr_total_rx_len += len;

		//transaction size > 0 and short packet
		if ((len == 0) || ((len < usbh_vendor_get_intr_ep_mps()) && (vendor_intr_total_rx_len > 0))
			|| (vendor_intr_total_rx_len > USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE)) { //
			vendor_intr_total_rx_len = 0;
			rtos_sema_give(vendor_intr_receive_sema);
		}
		break;
	case USB_CH_EP_TYPE_ISOC:
		rtos_sema_give(vendor_isoc_rxdone_sema);
		break;
	default:
		break;
	}
	return HAL_OK;
}

static int vendor_cb_transmit(u8 ep_type)
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

	return HAL_OK;
}

static void vendor_intr_loopback_test(void)
{
	int i, j;
	int vendor_intr_rx_errcnt = 0;

	while (1) {
		if (vendor_is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "INTR loopback test start, times:%d, size: %d\n", USBH_VENDOR_INTR_LOOPBACK_CNT, USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE);

	for (i = 0; i < USBH_VENDOR_INTR_LOOPBACK_CNT; i++) {
		memset(vendor_intr_loopback_rx_buf, 0, USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE);
		if (!vendor_is_ready) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Device disconnect\n");
			return;
		}

		memset(vendor_intr_loopback_tx_buf, i, USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE);
		usbh_vendor_intr_transmit(vendor_intr_loopback_tx_buf, USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE, USBH_VENDOR_INTR_LOOPBACK_CNT);

		if (rtos_sema_take(vendor_intr_send_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			usbh_vendor_intr_receive(vendor_intr_loopback_rx_buf, USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE, USBH_VENDOR_INTR_LOOPBACK_CNT);
		}

		if (rtos_sema_take(vendor_intr_receive_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			/*check rx data*/
			for (j = 0; j < USBH_VENDOR_INTR_LOOPBACK_BUF_SIZE; j++) {
				if (vendor_intr_loopback_rx_buf[j] != i) {
					vendor_intr_rx_errcnt++;
					RTK_LOGS(TAG, RTK_LOG_INFO, "INTR Loopback test FAIL: %d, %d, errcnt %d", i, vendor_intr_loopback_rx_buf[j], vendor_intr_rx_errcnt);
				}
			}
		}
	}

	rtos_sema_give(vendor_done_sema);
	RTK_LOGS(TAG, RTK_LOG_INFO, "INTR loopback test PASS %d/%d \n", USBH_VENDOR_INTR_LOOPBACK_CNT, (USBH_VENDOR_INTR_LOOPBACK_CNT - vendor_intr_rx_errcnt));
}

static void vendor_bulk_loopback_test(void)
{
	int i, j;
	int vendor_bulk_rx_errcnt = 0;

	while (1) {
		if (vendor_is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "BULK loopback test start, times:%d, size: %d\n", USBH_VENDOR_BULK_LOOPBACK_CNT, USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE);

	for (i = 0; i < USBH_VENDOR_BULK_LOOPBACK_CNT; i++) {
		memset(vendor_bulk_loopback_rx_buf, 0, USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE);
		if (!vendor_is_ready) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Device disconnect\n");
			return;
		}

		memset(vendor_bulk_loopback_tx_buf, i, USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE);

		usbh_vendor_bulk_transmit(vendor_bulk_loopback_tx_buf, USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE, USBH_VENDOR_BULK_LOOPBACK_CNT);

		if (rtos_sema_take(vendor_bulk_send_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			usbh_vendor_bulk_receive(vendor_bulk_loopback_rx_buf, USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE, USBH_VENDOR_BULK_LOOPBACK_CNT);
		}

		if (rtos_sema_take(vendor_bulk_receive_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			/*check rx data*/
			for (j = 0; j < USBH_VENDOR_BULK_LOOPBACK_BUF_SIZE; j++) {
				if (vendor_bulk_loopback_rx_buf[j] != i) {
					vendor_bulk_rx_errcnt++;
					RTK_LOGS(TAG, RTK_LOG_INFO, TAG, RTK_LOG_INFO, "BULK Loopback test FAIL: %d, %d, errcnt %d", i, vendor_bulk_loopback_rx_buf[j], vendor_bulk_rx_errcnt);
				}
			}
		}
	}

	rtos_sema_give(vendor_done_sema);
	RTK_LOGS(TAG, RTK_LOG_INFO, "BULK loopback test PASS %d/%d \n", USBH_VENDOR_BULK_LOOPBACK_CNT, (USBH_VENDOR_BULK_LOOPBACK_CNT - vendor_bulk_rx_errcnt));
}

static void vendor_isoc_test(void)
{
	while (1) {
		if (vendor_is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "ISOC test start, times:%d, size: %d\n", USBH_VENDOR_ISOC_TEST_CNT, USBH_VENDOR_ISOC_TEST_BUF_SIZE);

	if (!vendor_is_ready) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Device disconnect\n");
		return;
	}

	// handle isoc trx flow in class process cb
	usbh_vendor_isoc_transmit(vendor_isoc_tx_buf, USBH_VENDOR_ISOC_TEST_BUF_SIZE, USBH_VENDOR_ISOC_TEST_CNT);

	usbh_vendor_isoc_receive(vendor_isoc_rx_buf, USBH_VENDOR_ISOC_TEST_BUF_SIZE, USBH_VENDOR_ISOC_TEST_CNT);

	if (rtos_sema_take(vendor_isoc_txdone_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
	}

	if (rtos_sema_take(vendor_isoc_rxdone_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
	}

	rtos_sema_give(vendor_done_sema);
	RTK_LOGS(TAG, RTK_LOG_INFO, "ISOC test PASS\n");
}

static int vendor_cb_process(usb_host_t *host, u8 id)
{
	UNUSED(host);

	switch (id) {
	case USBH_MSG_DISCONNECTED:
		vendor_is_ready = 0;
		break;

	case USBH_MSG_CONNECTED:
		break;

	default:
		break;
	}

	return HAL_OK;
}

#if CONFIG_USBH_VENDOR_HOT_PLUG_TEST
static void vendor_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(vendor_detach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			rtos_time_delay_ms(100);//make sure disconnect handle finish before deinit.
			usbh_vendor_deinit();
			usbh_deinit();

			rtos_time_delay_ms(10);

			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%08x\n", rtos_mem_get_free_heap_size());

			ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBH fail: %d\n", ret);
				break;
			}

			ret = usbh_vendor_init(&vendor_usr_cb);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Init vendor fail: %d\n", ret);
				usbh_deinit();
				break;
			}
		}
	}

	rtos_task_delete(NULL);
}
#endif

static void vendor_bulk_test_task(void *param)
{
	UNUSED(param);
	vendor_bulk_loopback_test();
	rtos_task_delete(NULL);
}

static void vendor_intr_test_task(void *param)
{
	UNUSED(param);
	vendor_intr_loopback_test();
	rtos_task_delete(NULL);
}

void example_usbh_vendor_thread(void *param)
{
	int status;
#if CONFIG_USBH_VENDOR_HOT_PLUG_TEST
	rtos_task_t task;
#endif

	UNUSED(param);

	rtos_sema_create(&vendor_detach_sema, 0U, 1U);
	rtos_sema_create(&vendor_attach_sema, 0U, 1U);
	rtos_sema_create(&vendor_intr_receive_sema, 0U, 1U);
	rtos_sema_create(&vendor_intr_send_sema, 0U, 1U);
	rtos_sema_create(&vendor_bulk_receive_sema, 0U, 1U);
	rtos_sema_create(&vendor_bulk_send_sema, 0U, 1U);
	rtos_sema_create(&vendor_isoc_txdone_sema, 0U, 1U);
	rtos_sema_create(&vendor_isoc_rxdone_sema, 0U, 1U);
	rtos_sema_create(&vendor_done_sema, 0U, 3U);

	status = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (status != HAL_OK) {
		goto error_exit;
	}

	status = usbh_vendor_init(&vendor_usr_cb);
	if (status < 0) {
		usbh_deinit();
		goto error_exit;
	}

#if CONFIG_USBH_VENDOR_HOT_PLUG_TEST
	status = rtos_task_create(&task, "vendor_hotplug_thread", vendor_hotplug_thread, NULL, 1024U * 2, 3U);
	if (status != RTK_SUCCESS) {
		usbh_vendor_deinit();
		usbh_deinit();
		goto error_exit;
	}
#endif

	if (rtos_sema_take(vendor_attach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
		vendor_isoc_test();

		if (rtos_task_create(&task, "vendor_bulk_test", vendor_bulk_test_task, NULL, 1024U * 2, 5U) != RTK_SUCCESS) {
			goto error_exit;
		}

		if (rtos_task_create(&task, "vendor_intr_test", vendor_intr_test_task, NULL, 1024U * 2, 5U) != RTK_SUCCESS) {
			goto error_exit;
		}
	}

	for (int i = 0; i < UBSH_VENDOR_TEST_TASK_CNT; i++) {
		rtos_sema_take(vendor_done_sema, RTOS_SEMA_MAX_COUNT);
	}

	goto example_exit;

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
example_exit:
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH vendor demo stop\n");
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbh_vendor(void)
{
	int status;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH vendor demo start\n");
	status = rtos_task_create(&task, "example_usbh_vendor_thread", example_usbh_vendor_thread, NULL, 1024U * 2, 1U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
	}
}

