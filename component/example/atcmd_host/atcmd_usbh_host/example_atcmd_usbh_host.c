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
#include "serial_api.h"
#include "serial_ex_api.h"
#include "ringbuffer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "atcmd_service.h"

/* Private defines -----------------------------------------------------------*/

#define CONFIG_USBH_CDC_ACM_HOT_PLUG_TEST 1     /* Hot plug / memory leak test */

#define USBH_CDC_ACM_LOOPBACK_BUF_SIZE    2048  /* Buffer size for loopback test, which should match with device loopback buffer size */
#define USBH_CDC_ACM_NOTIFY_BUF_SIZE      256   /* Buffer size for notify test*/

#define MAX_CMD_LEN 2000
#define FORMAT_LEN 128

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

void uart_send_string(serial_t *sobj, char *pstr, u32 len);
void uart_format_string_output(const char *fmt, ...);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "ACM";

static u8 cdc_acm_loopback_tx_buf[USBH_CDC_ACM_LOOPBACK_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 cdc_acm_loopback_rx_buf[USBH_CDC_ACM_LOOPBACK_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 cdc_acm_notify_rx_buf[USBH_CDC_ACM_NOTIFY_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

u8 uart_show_buf[USBH_CDC_ACM_LOOPBACK_BUF_SIZE] = {0};
char uart_format_buffer[FORMAT_LEN];

static void *cdc_acm_detach_sema;
static void *cdc_acm_attach_sema;
static void *cdc_acm_receive_sema;
static void *cdc_acm_send_sema;
static void *cdc_acm_notify_sema;

static void *tt_mode_tx_sema;

static void *uart_show_sema;
static void *uart_irq_handle_sema;
static void *usbh_rx_ringbuf_mutex = NULL;

static __IO int atcmd_usbh_notify_len = 0;
static __IO int atcmd_usbh_rx_len = 0;
RingBuffer *at_usbh_rx_ring_buf = NULL;
static u8 tt_mode_task_start = 0;

static __IO int cdc_acm_is_ready = 0;


serial_t sobj;
char uart_irq_buffer[MAX_CMD_LEN] = {0};
u32 uart_irq_count = 0;

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
	return HAL_OK;
}

static int cdc_acm_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DEINIT\n");
	return HAL_OK;
}

static int cdc_acm_cb_attach(void)
{
	BaseType_t task_woken = pdFALSE;

	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
	xSemaphoreGiveFromISR(cdc_acm_attach_sema, &task_woken);
	portEND_SWITCHING_ISR(task_woken);
	return HAL_OK;
}

static int cdc_acm_cb_detach(void)
{
	BaseType_t task_woken = pdFALSE;

	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
	xSemaphoreGiveFromISR(cdc_acm_detach_sema, &task_woken);
	portEND_SWITCHING_ISR(task_woken);
	return HAL_OK;
}

static int cdc_acm_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
	cdc_acm_is_ready = 1;
	return HAL_OK;
}

static int cdc_acm_cb_receive(u8 *buf, u32 length)
{
	UNUSED(buf);
	BaseType_t task_woken = pdFALSE;

	atcmd_usbh_rx_len = length > USBH_CDC_ACM_LOOPBACK_BUF_SIZE ? USBH_CDC_ACM_LOOPBACK_BUF_SIZE : length;
	xSemaphoreGiveFromISR(cdc_acm_receive_sema, &task_woken);
	portEND_SWITCHING_ISR(task_woken);

	return HAL_OK;
}

static int cdc_acm_cb_transmit(usbh_urb_state_t state)
{
	BaseType_t task_woken = pdFALSE;

	if (state == USBH_URB_DONE) {
		/*TX done*/
		xSemaphoreGiveFromISR(cdc_acm_send_sema, &task_woken);
		portEND_SWITCHING_ISR(task_woken);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "TX fail: %d\n", state);
	}
	return HAL_OK;
}

static int cdc_acm_cb_notify(u8 *buf, u32 length)
{
	UNUSED(buf);
	BaseType_t task_woken = pdFALSE;

	atcmd_usbh_notify_len = length > USBH_CDC_ACM_NOTIFY_BUF_SIZE ? USBH_CDC_ACM_NOTIFY_BUF_SIZE : length;
	xSemaphoreGiveFromISR(cdc_acm_notify_sema, &task_woken);
	portEND_SWITCHING_ISR(task_woken);

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

static void tt_mode_test_task(void *param)
{
	UNUSED(param);
	u32 tt_len = 10 * 1024 * 1024;
	u32 send_len;
	u8 *tt_tx_buf = pvPortMalloc(USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
	int ret;

	while (tt_len > 0) {
		xSemaphoreTake(tt_mode_tx_sema, 0xFFFFFFFF);
		send_len = tt_len > (USBH_CDC_ACM_LOOPBACK_BUF_SIZE) ? (USBH_CDC_ACM_LOOPBACK_BUF_SIZE) : tt_len;
		ret = usbh_cdc_acm_transmit(tt_tx_buf, send_len);
		while (ret != HAL_OK) {
			vTaskDelay(1);
			ret = usbh_cdc_acm_transmit(tt_tx_buf, send_len);
		}
		xSemaphoreTake(cdc_acm_send_sema, 0xFFFFFFFF);
		tt_len -= send_len;
		xSemaphoreGive(tt_mode_tx_sema);
	}

	vPortFree(tt_tx_buf);
	tt_mode_task_start = 0;
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "atcmd_tt_mode_task end\r\n");
	vTaskDelete(NULL);
}

static void usbh_tx_task(void *param)
{
	UNUSED(param);
	int ret;
	u32 send_len;

	RTK_LOGS(TAG, RTK_LOG_INFO, "Wait for device attach\n");

WAIT_CONNECT:
	while (1) {
		if (cdc_acm_is_ready) {
			vTaskDelay(10);
			break;
		}
	}

	while (1) {
		xSemaphoreTake(uart_irq_handle_sema, 0xFFFFFFFF);
		memcpy(cdc_acm_loopback_tx_buf, uart_irq_buffer, uart_irq_count);
		memset(uart_irq_buffer, 0, 2000);
		send_len = uart_irq_count;
		uart_irq_count = 0;

		if (!cdc_acm_is_ready) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Device disconnect\n");
			goto WAIT_CONNECT;
		}

		ret = usbh_cdc_acm_transmit(cdc_acm_loopback_tx_buf, send_len);
		while (ret != HAL_OK) {
			vTaskDelay(1);
			ret = usbh_cdc_acm_transmit(cdc_acm_loopback_tx_buf, send_len);
		}

		xSemaphoreTake(cdc_acm_send_sema, 0xFFFFFFFF);
	}

	vTaskDelete(NULL);
}

static void usbh_notify_task(void *param)
{
	UNUSED(param);
	int ret;
WAIT_CONNECT:
	while (1) {
		if (cdc_acm_is_ready) {
			vTaskDelay(10);
			break;
		}
	}

	while (1) {
		if (!cdc_acm_is_ready) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Device disconnect\n");
			goto WAIT_CONNECT;
		}

		ret = usbh_cdc_acm_notify_receive(cdc_acm_notify_rx_buf, USBH_CDC_ACM_NOTIFY_BUF_SIZE);
		while (ret != HAL_OK) {
			vTaskDelay(1);
			ret = usbh_cdc_acm_notify_receive(cdc_acm_notify_rx_buf, USBH_CDC_ACM_NOTIFY_BUF_SIZE);
		}

		xSemaphoreTake(cdc_acm_notify_sema, 0xFFFFFFFF);

		if (atcmd_usbh_notify_len == 0) {
			continue;
		}

		if (memcmp(&cdc_acm_notify_rx_buf[8], "HW", 2) == 0) {
			xSemaphoreTake(tt_mode_tx_sema, 0xFFFFFFFF);
			uart_send_string(&sobj, ATCMD_TT_MODE_HIGH_WATERMARK_STR, strlen(ATCMD_TT_MODE_HIGH_WATERMARK_STR));
		} else if (memcmp(&cdc_acm_notify_rx_buf[8], "LW", 2) == 0) {
			xSemaphoreGive(tt_mode_tx_sema);
			uart_send_string(&sobj, ATCMD_TT_MODE_LOW_WATERMARK_STR, strlen(ATCMD_TT_MODE_LOW_WATERMARK_STR));
		}
	}
}

static void usbh_rx_task(void *param)
{
	int ret;
	static u32 start_time = 0, end_time = 0, ds_count = 0;
	char *str_find = NULL;
	UNUSED(param);

WAIT_CONNECT:
	while (1) {
		if (cdc_acm_is_ready) {
			vTaskDelay(10);
			break;
		}
	}

	while (1) {
		if (!cdc_acm_is_ready) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Device disconnect\n");
			goto WAIT_CONNECT;
		}

		ret = usbh_cdc_acm_receive(cdc_acm_loopback_rx_buf, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
		while (ret != HAL_OK) {
			vTaskDelay(1);
			ret = usbh_cdc_acm_receive(cdc_acm_loopback_rx_buf, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
		}

		xSemaphoreTake(cdc_acm_receive_sema, 0xFFFFFFFF);

		if (atcmd_usbh_rx_len == 0) {
			continue;
		}

		if (atcmd_usbh_rx_len == 5 && strncmp((char *)cdc_acm_loopback_rx_buf, ">>>", 3) == 0) {
			xTaskCreate((void *)tt_mode_test_task, ((const char *)"tt_mode_test_task"), 1024 / sizeof(portSTACK_TYPE), NULL, 1, NULL);
			tt_mode_task_start = 1;
		}

		if (tt_mode_task_start) {
			xSemaphoreTake(usbh_rx_ringbuf_mutex, 0xFFFFFFFF);
			RingBuffer_Write(at_usbh_rx_ring_buf, cdc_acm_loopback_rx_buf, atcmd_usbh_rx_len);
			xSemaphoreGive(usbh_rx_ringbuf_mutex);
			xSemaphoreGive(uart_show_sema);

		} else {
			if (memcmp(cdc_acm_loopback_rx_buf, ATCMD_DOWNSTREAM_TEST_START_STR, strlen(ATCMD_DOWNSTREAM_TEST_START_STR)) == 0) {
				start_time = xTaskGetTickCount();
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, ATCMD_DOWNSTREAM_TEST_START_STR);
			}

			if (start_time != 0) {
				str_find = strstr((char *)cdc_acm_loopback_rx_buf, ATCMD_DOWNSTREAM_TEST_END_STR);
				if (str_find != NULL) {
					RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, ATCMD_DOWNSTREAM_TEST_END_STR);
					end_time = xTaskGetTickCount();
					ds_count = ds_count + ((u8 *)str_find - cdc_acm_loopback_rx_buf);
					uart_format_string_output("downstream test: Receive %lu KBytes in %lu ms, %d Kbits/sec\n\r", ds_count / 1024,
											  (end_time - start_time), (int)((ds_count * 8) / (end_time - start_time)));
					start_time = 0;
					end_time = 0;
					ds_count = 0;

					if (strstr((char *)cdc_acm_loopback_rx_buf, "OK")) {
						uart_send_string(&sobj, "\r\nOK\r\n", strlen("\r\nOK\r\n"));
					}
				} else {
					ds_count += atcmd_usbh_rx_len;
				}
			} else {
				xSemaphoreTake(usbh_rx_ringbuf_mutex, 0xFFFFFFFF);
				RingBuffer_Write(at_usbh_rx_ring_buf, cdc_acm_loopback_rx_buf, atcmd_usbh_rx_len);
				xSemaphoreGive(usbh_rx_ringbuf_mutex);
				xSemaphoreGive(uart_show_sema);
			}
		}
	}
	vTaskDelete(NULL);
}

static void uart_show_rx_data_task(void *param)
{
	u32 actual_len, show_len;
	UNUSED(param);

	while (1) {
		xSemaphoreTake(uart_show_sema, 0xFFFFFFFF);

		actual_len = RingBuffer_Available(at_usbh_rx_ring_buf);
		while (actual_len > 0) {
			show_len = actual_len > USBH_CDC_ACM_LOOPBACK_BUF_SIZE ? USBH_CDC_ACM_LOOPBACK_BUF_SIZE : actual_len;

			xSemaphoreTake(usbh_rx_ringbuf_mutex, 0xFFFFFFFF);
			RingBuffer_Read(at_usbh_rx_ring_buf, uart_show_buf, show_len);
			xSemaphoreGive(usbh_rx_ringbuf_mutex);

			uart_send_string(&sobj, (char *)uart_show_buf, show_len);
			actual_len -= show_len;
		}
	}

	vTaskDelete(NULL);
}

#if CONFIG_USBH_CDC_ACM_HOT_PLUG_TEST
static void cdc_acm_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (xSemaphoreTake(cdc_acm_detach_sema, 0xFFFFFFFF) == pdPASS) {
			vTaskDelay(100);
			usbh_cdc_acm_deinit();
			usbh_deinit();
			vTaskDelay(10);

			ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBH fail\n");
				break;
			}

			ret = usbh_cdc_acm_init(&cdc_acm_usr_cb);
			if (ret < 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Init CDC ACM fail\n");
				usbh_deinit();
				break;
			}
		}
	}

	vTaskDelete(NULL);
}
#endif

static void atcmd_usbh_cdc_acm_task(void *param)
{
	int status;

	UNUSED(param);

	cdc_acm_detach_sema = (void *)xSemaphoreCreateCounting(1, 0);
	cdc_acm_attach_sema = (void *)xSemaphoreCreateCounting(1, 0);
	cdc_acm_receive_sema = (void *)xSemaphoreCreateCounting(1, 0);
	cdc_acm_send_sema = (void *)xSemaphoreCreateCounting(1, 0);
	cdc_acm_notify_sema = (void *)xSemaphoreCreateCounting(1, 0);

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

	status = xTaskCreate((void *)cdc_acm_hotplug_thread, ((const char *)"cdc_acm_hotplug_thread"), 1024 / sizeof(portSTACK_TYPE), NULL, 3, NULL);
	if (status != pdPASS) {
		usbh_cdc_acm_deinit();
		usbh_deinit();
		goto error_exit;
	}
#endif

	if (xSemaphoreTake(cdc_acm_attach_sema, 0xFFFFFFFF) == pdPASS) {
		xTaskCreate((void *)usbh_tx_task, ((const char *)"usbh_tx_task"), 1024 / sizeof(portSTACK_TYPE), NULL, 3, NULL);
		xTaskCreate((void *)usbh_rx_task, ((const char *)"usbh_rx_task"), 1024 / sizeof(portSTACK_TYPE), NULL, 4, NULL);
		xTaskCreate((void *)usbh_notify_task, ((const char *)"usbh_notify_task"), 1024 / sizeof(portSTACK_TYPE), NULL, 4, NULL);
		xTaskCreate((void *)uart_show_rx_data_task, ((const char *)"uart_show_rx_data_task"), 1024 / sizeof(portSTACK_TYPE), NULL, 1, NULL);
	}

	goto example_exit;
error_exit:
	vSemaphoreDelete(cdc_acm_detach_sema);
	vSemaphoreDelete(cdc_acm_attach_sema);
	vSemaphoreDelete(cdc_acm_receive_sema);
	vSemaphoreDelete(cdc_acm_send_sema);
	vSemaphoreDelete(cdc_acm_notify_sema);
example_exit:
	vTaskDelete(NULL);
}

void uart_irq(uint32_t id, SerialIrq event)
{
	serial_t *sobj = (void *)id;
	BaseType_t task_woken = pdFALSE;

	if (event == RxIrq) {
		while (serial_readable(sobj)) {
			if (uart_irq_count == MAX_CMD_LEN) {
				break;
			}

			uart_irq_buffer[uart_irq_count++] = serial_getc(sobj);
			serial_putc(sobj, uart_irq_buffer[uart_irq_count - 1]);
		}

		if ((uart_irq_count > 1 && uart_irq_buffer[uart_irq_count - 1] == '\n' && uart_irq_buffer[uart_irq_count - 2] == '\r')
			|| (uart_irq_count == MAX_CMD_LEN)) {
			xSemaphoreGiveFromISR(uart_irq_handle_sema, &task_woken);
			portEND_SWITCHING_ISR(task_woken);
		}
	}
}

void uart_send_string(serial_t *sobj, char *pstr, u32 len)
{
	unsigned int i = 0;
	while (*(pstr + i) != 0 && i < len) {
		serial_putc(sobj, *(pstr + i));
		i++;
	}
}

void uart_format_string_output(const char *fmt, ...)
{
	int len_fmt = 0;
	va_list ap;
	va_start(ap, fmt);
	len_fmt = vsnprintf(uart_format_buffer, FORMAT_LEN, fmt, ap);
	va_end(ap);

	if (len_fmt < 0 || len_fmt >= FORMAT_LEN) {
		return;
	}

	uart_send_string(&sobj, (char *)uart_format_buffer, len_fmt);
}

/* Exported functions --------------------------------------------------------*/

void example_atcmd_usbh_host(void)
{
	uart_irq_handle_sema = (void *)xSemaphoreCreateCounting(1, 0);
	uart_show_sema = (void *)xSemaphoreCreateCounting(0xFFFF, 0);
	tt_mode_tx_sema = (void *)xSemaphoreCreateCounting(1, 1);
	usbh_rx_ringbuf_mutex = (void *)xSemaphoreCreateMutex();

	at_usbh_rx_ring_buf = RingBuffer_Create(NULL, 4 * 1024, LOCAL_RINGBUFF, 1);

	/*init uart*/
	sobj.uart_idx = 0;
	serial_init(&sobj, PA_3, PA_2);
	serial_baud(&sobj, 115200);
	serial_format(&sobj, 8, ParityNone, 1);

	uart_send_string(&sobj, "UART Send Command Demo...\r\n", strlen("UART Send Command Demo...\r\n"));
	serial_irq_handler(&sobj, uart_irq, (uint32_t)&sobj);
	serial_irq_set(&sobj, RxIrq, 1);
	serial_irq_set(&sobj, TxIrq, 1);

	if (xTaskCreate((void *)atcmd_usbh_cdc_acm_task, ((const char *)"atcmd_usbh_cdc_acm_task"), 1024 / sizeof(portSTACK_TYPE), NULL, 1, NULL) != pdPASS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
	}
}

