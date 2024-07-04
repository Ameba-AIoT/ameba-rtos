/**
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "platform_stdlib.h"
#include "basic_types.h"
#include "os_wrapper.h"
#include "usbh_cdc_acm.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/

#define CONFIG_USBH_CDC_ACM_HOT_PLUG_TEST 1     /* Hot plug / memory leak test */
#define CONFIG_USBH_CDC_ACM_STRESS_TEST   0     /* Stress / long run test */

#define CONFIG_USBH_CDC_ACM_SPEED_TEST    0     /* Tx Rx fast transfer test */

#define USBH_CDC_ACM_NOTIFY_BUF_SIZE      256   /* Buffer size for notify test*/
#define USBH_CDC_ACM_LOOPBACK_BUF_SIZE    1024  /* Buffer size for loopback test, which should match with device loopback buffer size */
#define USBH_CDC_ACM_LOOPBACK_CNT         100   /* Loopback test round */


/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int cdc_acm_cb_init(void);
static int cdc_acm_cb_deinit(void);
static int cdc_acm_cb_attach(void);
static int cdc_acm_cb_detach(void);
static int cdc_acm_cb_setup(void);
static int cdc_acm_cb_transmit(usbh_urb_state_t state);
static int cdc_acm_cb_receive(u8 *pbuf, u32 Len);
static int cdc_acm_cb_notify(u8 *pbuf, u32 Len);
static int cdc_acm_cb_line_coding_changed(usbh_cdc_acm_line_coding_t *line_coding);
static int cdc_acm_cb_process(usb_host_t *host, u8 id);
static void cdc_acm_notify_test(void);
static void cdc_acm_request_test(void);
static void cdc_acm_notify_test_thread(void *param);
#if CONFIG_USBH_CDC_ACM_SPEED_TEST
static void cdc_acm_speed_loopback_test(void);
#else
static void cdc_acm_loopback_test(void);
#endif

/* Private variables ---------------------------------------------------------*/
static const char *TAG = "ACM";

static u8 cdc_acm_loopback_tx_buf[USBH_CDC_ACM_LOOPBACK_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 cdc_acm_loopback_rx_buf[USBH_CDC_ACM_LOOPBACK_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 cdc_acm_notify_rx_buf[USBH_CDC_ACM_NOTIFY_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

static rtos_sema_t cdc_acm_detach_sema;
static rtos_sema_t cdc_acm_attach_sema;
static rtos_sema_t cdc_acm_receive_sema;
static rtos_sema_t cdc_acm_notify_sema;
static rtos_sema_t cdc_acm_send_sema;

static __IO int cdc_acm_total_rx_len = 0;
static __IO int cdc_acm_is_ready = 0;

static usbh_config_t usbh_cfg = {
	.pipes = 5U,
	.speed = USB_SPEED_HIGH,
	.dma_enable = 1U,
	.main_task_priority = 4U,
	.isr_task_priority  = 5U,
};

static usbh_cdc_acm_cb_t cdc_acm_usr_cb = {
	.init   = cdc_acm_cb_init,
	.deinit = cdc_acm_cb_deinit,
	.attach = cdc_acm_cb_attach,
	.detach = cdc_acm_cb_detach,
	.setup  = cdc_acm_cb_setup,
	.transmit = cdc_acm_cb_transmit,
	.receive  = cdc_acm_cb_receive,
	.notify   = cdc_acm_cb_notify,
	.line_coding_changed = cdc_acm_cb_line_coding_changed
};

static usbh_user_cb_t usbh_usr_cb = {
	.process = cdc_acm_cb_process
};

/* Private functions ---------------------------------------------------------*/

static int cdc_acm_cb_init(void)
{
	RTK_LOGS(TAG, "[ACM] INIT\n");
	return HAL_OK;
}

static int cdc_acm_cb_deinit(void)
{
	RTK_LOGS(TAG, "[ACM] DEINIT\n");
	return HAL_OK;
}

static int cdc_acm_cb_attach(void)
{
	RTK_LOGS(TAG, "[ACM] ATTACH\n");
	rtos_sema_give(cdc_acm_attach_sema);
	return HAL_OK;
}

static int cdc_acm_cb_detach(void)
{
	RTK_LOGS(TAG, "[ACM] DETACH\n");
#if CONFIG_USBH_CDC_ACM_HOT_PLUG_TEST
	rtos_sema_give(cdc_acm_detach_sema);
#endif
	return HAL_OK;
}

static int cdc_acm_cb_setup(void)
{
	RTK_LOGS(TAG, "[ACM] SETUP\n");
	cdc_acm_is_ready = 1;
	return HAL_OK;
}

static int cdc_acm_cb_notify(u8 *buf, u32 length)
{
	UNUSED(buf);
	UNUSED(length);
	rtos_sema_give(cdc_acm_notify_sema);
	return HAL_OK;
}

static int cdc_acm_cb_receive(u8 *buf, u32 length)
{
	//limited the copy length
	if ((length > 0) && ((cdc_acm_total_rx_len + length) <= USBH_CDC_ACM_LOOPBACK_BUF_SIZE)) {
		memcpy(cdc_acm_loopback_rx_buf + cdc_acm_total_rx_len, buf, length);
	}
	cdc_acm_total_rx_len += length;

	//transaction size > 0 and short packet
	if ((length == 0) || ((length < usbh_cdc_acm_get_bulk_ep_mps()) && (cdc_acm_total_rx_len > 0))
		|| (cdc_acm_total_rx_len > USBH_CDC_ACM_LOOPBACK_BUF_SIZE)) { //
		cdc_acm_total_rx_len = 0;
		rtos_sema_give(cdc_acm_receive_sema);
	}
	return HAL_OK;
}

static int cdc_acm_cb_transmit(usbh_urb_state_t state)
{
	if (state == USBH_URB_DONE) {
		/*TX done*/
		rtos_sema_give(cdc_acm_send_sema);
	} else {
		RTK_LOGS(TAG, "[ACM] TX fail: %d\n", state);
	}
	return HAL_OK;
}

static int cdc_acm_cb_line_coding_changed(usbh_cdc_acm_line_coding_t *line_coding)
{
	UNUSED(line_coding);
	return HAL_OK;
}

static int cdc_acm_cb_process(usb_host_t *host, u8 id)
{
	UNUSED(host);

	switch (id) {
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

#if  CONFIG_USBH_CDC_ACM_SPEED_TEST
static u32 cdc_acm_loopback_tx_idx = 0;
static volatile u64 tx_loop_cnt = 0, tx_loop_sub_cnt = 0;
static volatile u64 rx_loop_cnt = 0, rx_loop_sub_cnt = 0;
#define TASK_DUMP_CNT  1
static void bulk_tx_thread(void *param)
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
					RTK_LOGS(TAG, "[ACM] Device disconnect\n");
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
				RTK_LOGS(TAG, "[ACM] Bulk loopback tx test PASS rx(%d-%d) tx(%d-%d)\n", rx_loop_cnt, rx_loop_sub_cnt, tx_loop_cnt, tx_loop_sub_cnt);
			}
#if CONFIG_USBH_CDC_ACM_STRESS_TEST
		}
#endif
	}

	rtos_task_delete(NULL);
}
static void bulk_rx_thread(void *param)
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
					RTK_LOGS(TAG, "[ACM] Device disconnect\n");
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
				RTK_LOGS(TAG, "[ACM] Bulk loopback rx test PASS rx(%d-%d) tx(%d-%d)\n", rx_loop_cnt, rx_loop_sub_cnt, tx_loop_cnt, tx_loop_sub_cnt);
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

	RTK_LOGS(TAG, "[ACM] Wait for device attach\n");

	while (1) {
		if (cdc_acm_is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}

	RTK_LOGS(TAG, "[ACM] Bulk loopback test start, times:%d, size: %d\n", USBH_CDC_ACM_LOOPBACK_CNT, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);

	ret = rtos_task_create(&rx_task, "rx_task", bulk_rx_thread, NULL, 1024U * 2, 3U);
	if (ret != SUCCESS) {
		RTK_LOGS(TAG, "[ACM] Create rx_task fail\n");
	}
	//start two task, one for tx, one for rx
	ret = rtos_task_create(&tx_task, "tx_task", bulk_tx_thread, NULL, 1024U * 2, 3U);
	if (ret != SUCCESS) {
		RTK_LOGS(TAG, "[ACM] Create tx_task fail\n");
	}
}
#else
static void cdc_acm_loopback_test(void)
{
	int i, j, k;
	int ret;

	for (i = 0; i < USBH_CDC_ACM_LOOPBACK_BUF_SIZE; i++) {
		cdc_acm_loopback_tx_buf[i] = i % 128;
	}

	RTK_LOGS(TAG, "[ACM] Wait for device attach\n");

	while (1) {
		if (cdc_acm_is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}

	RTK_LOGS(TAG, "[ACM] Bulk loopback test start, times:%d, size: %d\n", USBH_CDC_ACM_LOOPBACK_CNT, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);

#if CONFIG_USBH_CDC_ACM_STRESS_TEST
	while (1) {
#endif
		for (i = 0; i < USBH_CDC_ACM_LOOPBACK_CNT; i++) {
			memset(cdc_acm_loopback_rx_buf, 0, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
			if (!cdc_acm_is_ready) {
				RTK_LOGS(TAG, "[ACM] Device disconnect\n");
				return;
			}
			ret = usbh_cdc_acm_transmit(cdc_acm_loopback_tx_buf, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
			if (ret < 0) {
				RTK_LOGS(TAG, "[ACM] TX fail: %d\n", ret);
				return;
			}

			if (rtos_sema_take(cdc_acm_send_sema, RTOS_SEMA_MAX_COUNT) == SUCCESS) {
				usbh_cdc_acm_receive(cdc_acm_loopback_rx_buf, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
			}

			if (rtos_sema_take(cdc_acm_receive_sema, RTOS_SEMA_MAX_COUNT) == SUCCESS) {
				/*check rx data*/
				for (k = 0 ; k < USBH_CDC_ACM_LOOPBACK_BUF_SIZE; k++) {
					if (cdc_acm_loopback_rx_buf[k] != k % 128) {
						RTK_LOGS(TAG, "[ACM] Loopback test FAIL: %d, %d, %d\n", k, k % 128, cdc_acm_loopback_rx_buf[k]);
						for (j = k ; j < k + 200; j++) {
							RTK_LOGS(TAG, "%d ", cdc_acm_loopback_rx_buf[j % USBH_CDC_ACM_LOOPBACK_BUF_SIZE]);
						}
						return;
					}
				}
			}
		}
		RTK_LOGS(TAG, "[ACM] Bulk loopback test PASS\n");
#if CONFIG_USBH_CDC_ACM_STRESS_TEST
	}
#endif
}
#endif

static void cdc_acm_notify_test_thread(void *param)
{
	UNUSED(param);

	while (1) {
		if (cdc_acm_is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}

	usbh_cdc_acm_set_control_line_state();
	//wait for set control line finish
	rtos_time_delay_ms(2000);

	int i = 0;
	for (i = 0; i < USBH_CDC_ACM_LOOPBACK_CNT; i++) {
		usbh_cdc_acm_notify_receive(cdc_acm_notify_rx_buf, USBH_CDC_ACM_NOTIFY_BUF_SIZE);

		if (rtos_sema_take(cdc_acm_notify_sema, RTOS_SEMA_MAX_COUNT) == SUCCESS) {
			RTK_LOGS(TAG, "[ACM] Intr rx success(0x%02x%02x)\n", cdc_acm_notify_rx_buf[9], cdc_acm_notify_rx_buf[8]);
		}
	}
	RTK_LOGS(TAG, "[ACM] Intr notify test PASS\n");

	rtos_task_delete(NULL);
}

/*
* this function used to test the CDC ACM Notify
* it is the INTR USB IN transfer type
* the function will send the INTR INTOKEN
* set CONFIG_CDC_ACM_NOTIFY=1 in usbd_cdc_acm.h to enable the device report the notify
* and set CONFIG_CDC_ACM_NOTIFY_LOOP_TEST=1 to enable the notify loop many times
*/
static void cdc_acm_notify_test(void)
{
	int status;
	rtos_task_t task;
	status = rtos_task_create(&task, "notify_task", cdc_acm_notify_test_thread, NULL, 1024U, 2U);
	if (status != SUCCESS) {
		RTK_LOGS(TAG, "[ACM] Create notify thread fail(%d)\n", status);
	}
}

static void cdc_acm_request_test(void)
{
	int ret;
	usbh_cdc_acm_line_coding_t line_coding;
	usbh_cdc_acm_line_coding_t new_line_coding;

	RTK_LOGS(TAG, "[ACM] Wait for device attach\n");

	while (1) {
		if (cdc_acm_is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}

	RTK_LOGS(TAG, "[ACM] Control test start\n");

	RTK_LOGS(TAG, "[ACM] GET_LINE_CODING:");
	ret = usbh_cdc_acm_get_line_coding(&line_coding);
	if (ret == HAL_OK) {
		RTK_LOGS(TAG, "DteRate: %d\nCharFormat: %d\nParityType: %d\nDataBits: %d\n",
				 line_coding.b.dwDteRate, line_coding.b.bCharFormat, line_coding.b.bParityType, line_coding.b.bDataBits);
	} else {
		RTK_LOGS(TAG, "fail\n");
	}

	RTK_LOGS(TAG, "[ACM] SET_LINE_CODING:\nDteRate: 38400\nCharFormat: 0\nParityType: 0\nDataBits: 8\n");
	line_coding.b.dwDteRate = 38400;
	line_coding.b.bCharFormat = CDC_ACM_LINE_CODING_CHAR_FORMAT_1_STOP_BITS;
	line_coding.b.bParityType = CDC_ACM_LINE_CODING_PARITY_NO;
	line_coding.b.bDataBits = 8;
	ret = usbh_cdc_acm_set_line_coding(&line_coding);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, "[ACM] SET_LINE_CODING fail\n");
	}

	rtos_time_delay_ms(10); // delay for wait set line coding done

	RTK_LOGS(TAG, "[ACM] GET_LINE_CODING:");
	ret = usbh_cdc_acm_get_line_coding(&new_line_coding);
	if (ret == HAL_OK) {
		RTK_LOGS(TAG, "DteRate: %d\nCharFormat: %d\nParityType: %d\nDataBits: %d\n",
				 new_line_coding.b.dwDteRate, new_line_coding.b.bCharFormat, new_line_coding.b.bParityType, new_line_coding.b.bDataBits);
	} else {
		RTK_LOGS(TAG, "fail\n");
	}

	if ((new_line_coding.b.dwDteRate == line_coding.b.dwDteRate) &&
		(new_line_coding.b.bCharFormat == line_coding.b.bCharFormat) &&
		(new_line_coding.b.bParityType == line_coding.b.bParityType) &&
		(new_line_coding.b.bDataBits == line_coding.b.bDataBits)) {
		RTK_LOGS(TAG, "[ACM] Ctrl test PASS\n");
	} else {
		RTK_LOGS(TAG, "[ACM] Ctrl test FAIL\n");
	}
}

#if CONFIG_USBH_CDC_ACM_HOT_PLUG_TEST
static void cdc_acm_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(cdc_acm_detach_sema, RTOS_SEMA_MAX_COUNT) == SUCCESS) {
			rtos_time_delay_ms(100);
			usbh_cdc_acm_deinit();
			usbh_deinit();
			rtos_time_delay_ms(10);
			RTK_LOGS(TAG, "[ACM] Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

			ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, "[ACM] Init USBH fail\n");
				break;
			}

			ret = usbh_cdc_acm_init(&cdc_acm_usr_cb);
			if (ret < 0) {
				RTK_LOGS(TAG, "[ACM] Init CDC ACM fail\n");
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
	int status;
#if CONFIG_USBH_CDC_ACM_HOT_PLUG_TEST
	rtos_task_t task;
#endif

	UNUSED(param);

	rtos_sema_create(&cdc_acm_detach_sema, 0U, 1U);
	rtos_sema_create(&cdc_acm_attach_sema, 0U, 1U);
	rtos_sema_create(&cdc_acm_receive_sema, 0U, 1U);
	rtos_sema_create(&cdc_acm_notify_sema, 0U, 1U);
	rtos_sema_create(&cdc_acm_send_sema, 0U, 1U);

	status = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (status != HAL_OK) {
		goto error_exit;
	}

	status = usbh_cdc_acm_init(&cdc_acm_usr_cb);  /*0 means use default transfer size, and it can not exceed 65536*/
	if (status != HAL_OK) {
		usbh_deinit();
		goto error_exit;
	}

#if CONFIG_USBH_CDC_ACM_HOT_PLUG_TEST
	status = rtos_task_create(&task, "cdc_acm_hotplug_thread", cdc_acm_hotplug_thread, NULL, 1024U, 3U);
	if (status != SUCCESS) {
		usbh_cdc_acm_deinit();
		usbh_deinit();
		goto error_exit;
	}
#endif

	if (rtos_sema_take(cdc_acm_attach_sema, RTOS_SEMA_MAX_COUNT) == SUCCESS) {
		cdc_acm_request_test();
#if  CONFIG_USBH_CDC_ACM_SPEED_TEST
		cdc_acm_speed_loopback_test();
#else
		cdc_acm_loopback_test();
#endif
		cdc_acm_notify_test();
	}

	goto example_exit;
error_exit:
	rtos_sema_delete(cdc_acm_detach_sema);
	rtos_sema_delete(cdc_acm_attach_sema);
	rtos_sema_delete(cdc_acm_receive_sema);
	rtos_sema_delete(cdc_acm_notify_sema);
	rtos_sema_delete(cdc_acm_send_sema);
example_exit:
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbh_cdc_acm(void)
{
	int status;
	rtos_task_t task;

	RTK_LOGS(TAG, "[ACM] USBH CDC ACM demo start\n");

	status = rtos_task_create(&task, "example_usbh_cdc_acm_thread", example_usbh_cdc_acm_thread, NULL, 1024U, 1U);
	if (status != SUCCESS) {
		RTK_LOGS(TAG, "[ACM] Create thread fail\n");
	}
}

