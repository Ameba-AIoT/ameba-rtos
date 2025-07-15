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
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/

#define USBH_CDC_ACM_NOTIFY_BUF_SIZE      256   /* Buffer size for notify test*/
#define USBH_CDC_ACM_TP_LOOP_CNT          50    /* Test round for calculate TP */

/* Private types -------------------------------------------------------------*/
typedef struct {
	rtos_sema_t detach_sema;//For hotplug test
	rtos_sema_t notify_sema;
	rtos_sema_t rx_sema;
	rtos_sema_t tx_sema;
	u8 *notify_rx_buf;
	u8 *tx_buf;
	u8 *rx_buf;
	u32 xfer_size;/* Buffer size for loopback test, which should match with device loopback buffer size */
	u32 tx_start;
	u32 tx_elapse;
	u32 tx_perf;
	u32 rx_start;
	u32 rx_elapse;
	u32 rx_perf;
	u32 max_rx_perf;
	u32 max_tx_perf;
	u16 test_round;
	__IO u32 total_rx_len;
	__IO u8 is_init: 1;
	__IO u8 is_ready: 1;
	__IO u8 hotplug_en: 1; //For hotplug test
} cdc_acm_usbh_t;
/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int cdc_acm_cb_init(void);
static int cdc_acm_cb_deinit(void);
static int cdc_acm_cb_attach(void);
static int cdc_acm_cb_detach(void);
static int cdc_acm_cb_setup(void);
static int cdc_acm_cb_notify(u8 *pbuf, u32 Len);
static int cdc_acm_cb_line_coding_changed(usbh_cdc_acm_line_coding_t *line_coding);
static int cdc_acm_cb_transmit(usbh_urb_state_t state);
static int cdc_acm_cb_receive(u8 *pbuf, u32 Len);
static int cdc_acm_cb_process(usb_host_t *host, u8 id);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "ACM";

static cdc_acm_usbh_t usbh_cdc = {
	.xfer_size = 2048,
	.test_round = 50,
};

static usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.dma_enable = 1U,
	.ext_intr_en = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.isr_task_priority  = 5U,
	.main_task_priority = 4U,
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
	RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
	usbh_cdc.is_init = 1;
	return HAL_OK;
}

static int cdc_acm_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DEINIT\n");
	usbh_cdc.is_init = 0;
	return HAL_OK;
}

static int cdc_acm_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
	return HAL_OK;
}

static int cdc_acm_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
	if (usbh_cdc.hotplug_en) {
		rtos_sema_give(usbh_cdc.detach_sema);
	}
	return HAL_OK;
}

static int cdc_acm_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
	usbh_cdc.is_ready = 1;
	return HAL_OK;
}

static int cdc_acm_cb_notify(u8 *buf, u32 length)
{
	UNUSED(buf);
	UNUSED(length);
	rtos_sema_give(usbh_cdc.notify_sema);
	return HAL_OK;
}

static int cdc_acm_cb_line_coding_changed(usbh_cdc_acm_line_coding_t *line_coding)
{
	UNUSED(line_coding);
	return HAL_OK;
}

static int cdc_acm_cb_transmit(usbh_urb_state_t state)
{
	if (state == USBH_URB_DONE) {
		/*TX done*/
		rtos_sema_give(usbh_cdc.tx_sema);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "TX fail: %d\n", state);
	}
	return HAL_OK;
}

static int cdc_acm_cb_receive(u8 *buf, u32 length)
{
	UNUSED(buf);
	//limited the copy length
	if ((length > 0) && ((usbh_cdc.total_rx_len + length) <= usbh_cdc.xfer_size)) {
		//memcpy(usbh_cdc.rx_buf + usbh_cdc.total_rx_len, buf, length);//rm memcpy for high TP
	}
	usbh_cdc.total_rx_len += length;
	//ZLP or short packet
	if ((length == 0) || (length % usbh_cdc_acm_get_bulk_ep_mps())) {
		usbh_cdc.total_rx_len = 0;
		rtos_sema_give(usbh_cdc.rx_sema);
	}
	return HAL_OK;
}

static int cdc_acm_cb_process(usb_host_t *host, u8 id)
{
	UNUSED(host);

	switch (id) {
	case USBH_MSG_DISCONNECTED:
		usbh_cdc.is_ready = 0;
		break;

	case USBH_MSG_CONNECTED:
		break;

	default:
		break;
	}

	return HAL_OK;
}

static void bulk_tx_thread(void *param)
{
	static u32 round;
	int ret;
	u32 i;
	UNUSED(param);

	usbh_cdc.tx_start = SYSTIMER_TickGet();

	for (i = 0; i < usbh_cdc.test_round;) {
		ret = usbh_cdc_acm_transmit(usbh_cdc.tx_buf, usbh_cdc.xfer_size);

		if (ret == HAL_OK) {
			rtos_sema_take(usbh_cdc.tx_sema, RTOS_SEMA_MAX_COUNT);
			round++;
			if (round % USBH_CDC_ACM_TP_LOOP_CNT == 0) {
				usbh_cdc.tx_elapse = SYSTIMER_GetPassTime(usbh_cdc.tx_start);//ms
				usbh_cdc.tx_perf = USBH_CDC_ACM_TP_LOOP_CNT * usbh_cdc.xfer_size / 1024 * 1000 / usbh_cdc.tx_elapse;
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "W %d KB/s %d ms\n", usbh_cdc.tx_perf, usbh_cdc.tx_elapse);
				usbh_cdc.max_tx_perf = MAX(usbh_cdc.max_tx_perf, usbh_cdc.tx_perf);
				usbh_cdc.tx_start = SYSTIMER_TickGet();
				i++;
			}
		} else {
			usb_os_delay_us(1);
		}
	}

	usbh_cdc.max_tx_perf = usbh_cdc.max_tx_perf * 10 / 1024;
	RTK_LOGS(TAG, RTK_LOG_INFO, "Write %d loop %d done, max TP %d.%d MB/s\n", usbh_cdc.xfer_size, usbh_cdc.test_round, usbh_cdc.max_tx_perf / 10,
			 usbh_cdc.max_tx_perf % 10);
	usbh_cdc.max_tx_perf = 0;
	rtos_task_delete(NULL);
}

static void cdc_acm_tx_test(void)
{
	int ret;
	rtos_task_t tx_task;

	while (1) {
		if (usbh_cdc.is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}

	ret = rtos_task_create(&tx_task, "tx_task", bulk_tx_thread, NULL, 1024U * 2, 3U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create tx_task fail\n");
	}
}

static void bulk_rx_thread(void *param)
{
	static u32 round;
	int ret;
	u32 i;
	UNUSED(param);

	usbh_cdc.rx_start = SYSTIMER_TickGet();

	for (i = 0; i < usbh_cdc.test_round;) {
		ret = usbh_cdc_acm_receive(usbh_cdc.rx_buf, usbh_cdc.xfer_size);

		if (ret == HAL_OK) {
			rtos_sema_take(usbh_cdc.rx_sema, RTOS_SEMA_MAX_COUNT);
			round++;
			if (round % USBH_CDC_ACM_TP_LOOP_CNT == 0) {
				usbh_cdc.rx_elapse = SYSTIMER_GetPassTime(usbh_cdc.rx_start);
				usbh_cdc.rx_perf = USBH_CDC_ACM_TP_LOOP_CNT * usbh_cdc.xfer_size / 1024 * 1000 / usbh_cdc.rx_elapse;
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "R %d KB/s @ %d ms\n", usbh_cdc.rx_perf, usbh_cdc.rx_elapse);
				usbh_cdc.max_rx_perf = MAX(usbh_cdc.max_rx_perf, usbh_cdc.rx_perf);
				usbh_cdc.rx_start = SYSTIMER_TickGet();
				i++;
			}
		} else {
			usb_os_delay_us(1);
		}
	}

	usbh_cdc.max_rx_perf = usbh_cdc.max_rx_perf * 10 / 1024;
	RTK_LOGS(TAG, RTK_LOG_INFO, "Read %d loop %d done, max TP %d.%d MB/s\n", usbh_cdc.xfer_size, usbh_cdc.test_round, usbh_cdc.max_rx_perf / 10,
			 usbh_cdc.max_rx_perf % 10);
	usbh_cdc.max_rx_perf = 0;
	rtos_task_delete(NULL);
}

static void cdc_acm_rx_test(void)
{
	int ret;
	rtos_task_t rx_task;
	while (1) {
		if (usbh_cdc.is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}

	ret = rtos_task_create(&rx_task, "rx_task", bulk_rx_thread, NULL, 1024U * 2, 3U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create rx_task fail\n");
	}
}

/*
* this function used to test the CDC ACM Notify
* it is the INTR USB IN transfer type
* the function will send the INTR INTOKEN
* set CONFIG_CDC_ACM_NOTIFY=1 in usbd_cdc_acm.h to enable the device report the notify
* and set CONFIG_CDC_ACM_NOTIFY_LOOP_TEST=1 in usbd_cdc_acm.h to enable the notify loop many times
*/
static void cdc_acm_notify_test(void)
{
	rtos_sema_create(&usbh_cdc.notify_sema, 0U, 1U);

	while (1) {
		if (usbh_cdc.is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}

	usbh_cdc_acm_set_control_line_state();

	RTK_LOGS(TAG, RTK_LOG_INFO, "Wait for set control line finish\n");
	rtos_time_delay_ms(2000);

	usbh_cdc_acm_notify_receive(usbh_cdc.notify_rx_buf, USBH_CDC_ACM_NOTIFY_BUF_SIZE);

	if (rtos_sema_take(usbh_cdc.notify_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Intr rx success(0x%02x 0x%02x)\n", usbh_cdc.notify_rx_buf[9], usbh_cdc.notify_rx_buf[8]);
	} else {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Intr notify TO\n");
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Intr notify test PASS\n");

	rtos_sema_delete(usbh_cdc.notify_sema);
}

static void cdc_acm_request_test(void)
{
	int ret;
	usbh_cdc_acm_line_coding_t line_coding;
	usbh_cdc_acm_line_coding_t new_line_coding;

	while (1) {
		if (usbh_cdc.is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Control test start\n");

	RTK_LOGS(TAG, RTK_LOG_INFO, "GET_LINE_CODING:");
	ret = usbh_cdc_acm_get_line_coding(&line_coding);
	if (ret == HAL_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "DteRate: %d\nCharFormat: %d\nParityType: %d\nDataBits: %d\n",
				 line_coding.b.dwDteRate, line_coding.b.bCharFormat, line_coding.b.bParityType, line_coding.b.bDataBits);
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "fail\n");
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "SET_LINE_CODING:\nDteRate: 38400\nCharFormat: 0\nParityType: 0\nDataBits: 8\n");
	line_coding.b.dwDteRate = 38400;
	line_coding.b.bCharFormat = CDC_ACM_LINE_CODING_CHAR_FORMAT_1_STOP_BITS;
	line_coding.b.bParityType = CDC_ACM_LINE_CODING_PARITY_NO;
	line_coding.b.bDataBits = 8;
	ret = usbh_cdc_acm_set_line_coding(&line_coding);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "SET_LINE_CODING fail\n");
	}

	rtos_time_delay_ms(10); // delay for wait set line coding done

	RTK_LOGS(TAG, RTK_LOG_INFO, "GET_LINE_CODING:");
	ret = usbh_cdc_acm_get_line_coding(&new_line_coding);
	if (ret == HAL_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "DteRate: %d\nCharFormat: %d\nParityType: %d\nDataBits: %d\n",
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

static int cdc_acm_verify_init(void)
{
	int ret;
	if (usbh_cdc.hotplug_en == 0) {
		rtos_sema_create(&usbh_cdc.detach_sema, 0U, 1U);
		rtos_sema_create(&usbh_cdc.rx_sema, 0U, 1U);
		rtos_sema_create(&usbh_cdc.tx_sema, 0U, 1U);

		usbh_cdc.notify_rx_buf = (u8 *)usb_os_malloc(USBH_CDC_ACM_NOTIFY_BUF_SIZE);
		if (usbh_cdc.notify_rx_buf == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to alloc notify_rx_buf\n");
			goto exit_free;
		}

		usbh_cdc.tx_buf = (u8 *)usb_os_malloc(usbh_cdc.xfer_size);
		if (usbh_cdc.tx_buf == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to alloc tx_buf\n");
			goto exit_free;
		}

		usbh_cdc.rx_buf = (u8 *)usb_os_malloc(usbh_cdc.xfer_size);
		if (usbh_cdc.rx_buf == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to alloc rx_buf\n");
			goto exit_free;
		}
	}

	ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBH fail\n");
		goto error_exit;
	}

	ret = usbh_cdc_acm_init(&cdc_acm_usr_cb);  /*0 means use default transfer size, and it can not exceed 65536*/
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init CDC ACM fail\n");
		usbh_deinit();
		goto error_exit;
	}
	return ret;

exit_free:
	if (usbh_cdc.rx_buf == NULL) {
		rtos_mem_free(usbh_cdc.rx_buf);
	}
	if (usbh_cdc.tx_buf == NULL) {
		rtos_mem_free(usbh_cdc.tx_buf);
	}
	if (usbh_cdc.notify_rx_buf == NULL) {
		rtos_mem_free(usbh_cdc.notify_rx_buf);
	}
error_exit:
	if (usbh_cdc.hotplug_en == 0) {
		rtos_sema_delete(usbh_cdc.detach_sema);
		rtos_sema_delete(usbh_cdc.rx_sema);
		rtos_sema_delete(usbh_cdc.tx_sema);
	}
	return HAL_ERR_HW;
}

static void cdc_acm_verify_deinit(void)
{
	if (usbh_cdc.hotplug_en == 0) {
		rtos_sema_delete(usbh_cdc.detach_sema);
		rtos_sema_delete(usbh_cdc.rx_sema);
		rtos_sema_delete(usbh_cdc.tx_sema);
	}
	usbh_cdc_acm_deinit();
	usbh_deinit();
}

static void cdc_acm_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(usbh_cdc.detach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (usbh_cdc.hotplug_en) {
				cdc_acm_verify_deinit();
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

				ret = cdc_acm_verify_init();
				if (ret < 0) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "hotplug Init fail\n");
					usbh_cdc.hotplug_en = 0;
					break;
				}
			} else {
				break;
			}

		}
	}

	rtos_task_delete(NULL);
}

/**
  * @brief  USB download de-initialize
  * @param  None
  * @retval Result of the operation: 0 if success else fail
  */
static int cdc_acm_hotplug_test(void)
{
	int ret = HAL_OK;
	void *task;

	if (usbh_cdc.hotplug_en) {
		ret = rtos_task_create(&task, "cdc_acm_hotplug_thread", cdc_acm_hotplug_thread, NULL, 1024U, 3U);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to create cdc_acm_hotplug_thread\n");

		}
	}

	return ret;
}

static void cdc_acm_verify_usage(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Invalid arguments, usage:\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbh acm xlen <xfer length in byte>\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbh acm speed <0: high, 1: full>\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbh acm dma <en/dis>\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbh acm loop <test count>\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbh acm init\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbh acm deinit\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbh acm rx/tx/trx\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbh acm test req/notify\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbh acm test hotplug <0/1>\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbh acm dump\n");
}

/* Exported functions --------------------------------------------------------*/

int cmd_usbh_cdc_acm_test(u16 argc, u8 *argv[])
{
	const char *cmd;
	int ret = HAL_OK;
	u32 xfer_size;
	u8 speed;

	if (argc < 2) {
		cdc_acm_verify_usage();
		return HAL_ERR_PARA;
	}

	cmd = (const char *)argv[1];

	if (_stricmp(cmd, "speed") == 0) {
		if (argc < 3) {
			cdc_acm_verify_usage();
			return HAL_ERR_PARA;
		}
		if (usbh_cdc.is_init) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Already init, please deinit first\n");
		} else {
			speed = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
			if ((speed == USB_SPEED_HIGH) || (speed == USB_SPEED_HIGH_IN_FULL)) {
				usbh_cfg.speed = speed;
				RTK_LOGS(TAG, RTK_LOG_INFO, "Speed %d\n", speed);
			} else {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid speed\n");
				ret = HAL_ERR_PARA;
			}
		}
	} else if (_stricmp(cmd, "dma") == 0) {
		if (argc < 3) {
			cdc_acm_verify_usage();
			return HAL_ERR_PARA;
		}
		if (usbh_cdc.is_init) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Already init, please deinit first\n");
		} else {
			if ((_stricmp((const char *)argv[2], (const char *)"en") == 0)) {
				usbh_cfg.dma_enable = 1U;
				RTK_LOGS(TAG, RTK_LOG_INFO, "DMA enabled\n");
			} else if ((_stricmp((const char *)argv[2], (const char *)"dis") == 0)) {
				usbh_cfg.dma_enable = 0U;
				RTK_LOGS(TAG, RTK_LOG_INFO, "DMA disabled\n");
			} else {
				cdc_acm_verify_usage();
				ret = HAL_ERR_PARA;
			}
		}
	} else if (_stricmp(cmd, "loop") == 0) {
		if (argc < 3) {
			cdc_acm_verify_usage();
			return HAL_ERR_PARA;
		}
		usbh_cdc.test_round = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
	} else if (_stricmp(cmd, "xlen") == 0) {
		if (argc < 3) {
			cdc_acm_verify_usage();
			return HAL_ERR_PARA;
		}
		xfer_size = (u32)_strtoul((const char *)(argv[2]), (char **)NULL, 10);
		if ((xfer_size > 0) && (xfer_size < 0xFFFF)) {
			usbh_cdc.xfer_size = (u16)xfer_size & 0xFFFF;
			RTK_LOGS(TAG, RTK_LOG_INFO, "Xfer size %d\n", usbh_cdc.xfer_size);
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid xfer size %d\n", xfer_size);
			ret = HAL_ERR_PARA;
		}
	} else if (_stricmp(cmd, "init") == 0) {
		if (usbh_cdc.is_init) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Already init, please deinit first\n");
		} else {
			ret = cdc_acm_verify_init();
		}
	} else if (_stricmp(cmd, "deinit") == 0) {
		if (usbh_cdc.is_init) {
			cdc_acm_verify_deinit();
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Not init, ignore\n");
		}
	} else if (_stricmp(cmd, "rx") == 0) {
		if (usbh_cdc.is_init) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "RX mode\n");
			cdc_acm_rx_test();
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Not init\n");
			ret = HAL_ERR_PARA;
		}
	} else if (_stricmp(cmd, "tx") == 0) {
		if (usbh_cdc.is_init) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "TX mode\n");
			cdc_acm_tx_test();
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Not init\n");
			ret = HAL_ERR_PARA;
		}
	} else if (_stricmp(cmd, "trx") == 0) {
		if (usbh_cdc.is_init) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "TRX mode\n");
			cdc_acm_tx_test();
			cdc_acm_rx_test();
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Not init\n");
			ret = HAL_ERR_PARA;
		}
	} else if (_stricmp(cmd, "test") == 0) {
		if ((usbh_cdc.is_init == 0) || (argc < 3)) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Not init or para invalid\n");
		} else {
			const char *subcmd = (const char *)argv[2];
			if (_stricmp(subcmd, "hotplug") == 0) {
				if (usbh_cdc.hotplug_en == 0) {
					usbh_cdc.hotplug_en = 1;
					cdc_acm_hotplug_test();
				}
			} else if (_stricmp(subcmd, "notify") == 0) {
				cdc_acm_notify_test();
			} else if (_stricmp(subcmd, "req") == 0) {
				cdc_acm_request_test();
			}
		}
	} else if (_stricmp(cmd, "dump") == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "DMA: %d\n", usbh_cfg.dma_enable);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Speed: %d\n", usbh_cfg.speed);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Size: %d\n", usbh_cdc.xfer_size);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Round: %d\n", usbh_cdc.test_round);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Hotplug: %d\n", usbh_cdc.hotplug_en);
	} else {
		cdc_acm_verify_usage();
		ret = HAL_ERR_PARA;
	}

	return ret;
}

