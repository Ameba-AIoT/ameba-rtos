/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "basic_types.h"
#include "usbd.h"
#include "usbd_cdc_acm.h"
#include "os_wrapper.h"
#include "serial_api.h"
#include "serial_ex_api.h"
#include "atcmd_service.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/* Private defines -----------------------------------------------------------*/

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
// while test suspend/resume, hotplug should be disabled
#define CONFIG_USBD_CDC_ACM_HOTPLUG					1

// USB speed
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define CONFIG_USBD_CDC_ACM_SPEED					USB_SPEED_FULL
#else
#define CONFIG_USBD_CDC_ACM_SPEED					USB_SPEED_HIGH
#endif

// Asynchronous transfer size
#define CONFIG_CDC_ACM_ASYNC_BUF_SIZE				2048U

// Do not change the settings unless indeed necessary
#define CONFIG_CDC_ACM_BULK_IN_XFER_SIZE			2048U
#define CONFIG_CDC_ACM_BULK_OUT_XFER_SIZE			2048U

// Thread priorities
#define CONFIG_CDC_ACM_INIT_THREAD_PRIORITY			5
#define CONFIG_CDC_ACM_HOTPLUG_THREAD_PRIORITY		8
#define CONFIG_CDC_ACM_XFER_THREAD_PRIORITY			5

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int cdc_acm_cb_init(void);
static int cdc_acm_cb_deinit(void);
static int cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf);
static int cdc_acm_cb_received(u8 *buf, u32 Len);
static void cdc_acm_cb_transmitted(u8 status);
static void cdc_acm_cb_status_changed(u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "ACM";

static usbd_cdc_acm_cb_t cdc_acm_cb = {
	.init = cdc_acm_cb_init,
	.deinit = cdc_acm_cb_deinit,
	.setup = cdc_acm_cb_setup,
	.received = cdc_acm_cb_received,
	.transmitted = cdc_acm_cb_transmitted,
	.status_changed = cdc_acm_cb_status_changed,
};

static usbd_cdc_acm_line_coding_t cdc_acm_line_coding;

static u16 cdc_acm_ctrl_line_state;

static usbd_config_t cdc_acm_cfg = {
	.speed = CONFIG_USBD_CDC_ACM_SPEED,
	.dma_enable   = 1U,
	.isr_priority = INT_PRI_MIDDLE,
	.intr_use_ptx_fifo  = 0U,
#if defined(CONFIG_AMEBASMART)
	.nptx_max_epmis_cnt = 1U,
	.ext_intr_en        = USBD_EPMIS_INTR,
#elif defined (CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 644U,
	.ptx_fifo_depth = {16U, 256U, 32U, 16U, 16U, },
#elif defined (CONFIG_AMEBASMARTPLUS)
	.rx_fifo_depth = 898U,
	.ptx_fifo_depth = {256U, 16U, 32U, 16U, 16U, },
#elif defined (CONFIG_AMEBAL2)
	.rx_fifo_depth = 661U,
	.ptx_fifo_depth = {256U, 16U, 32U, 16U, },
#endif
};

#if CONFIG_USBD_CDC_ACM_HOTPLUG
static u8 cdc_acm_attach_status;
static rtos_sema_t cdc_acm_attach_status_changed_sema;
#endif

#define MAX_CMD_LEN 2000
#define FORMAT_LEN 128
serial_t sobj;
rtos_sema_t atcmd_usbd_tx_done_sema;
rtos_sema_t tt_mode_tx_sema;
void *uart_irq_handle_sema;
char uart_format_buffer[FORMAT_LEN];
u8 uart_irq_buffer[MAX_CMD_LEN] = {0};
u32 uart_irq_count = 0;
static u8 tt_mode_task_start = 0;
volatile u8 high_water_mark = 0;

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

void uart_send_string(serial_t *sobj, char *pstr, u16 len)
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

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CDC media layer
  * @param  None
  * @retval Status
  */
static int cdc_acm_cb_init(void)
{
	usbd_cdc_acm_line_coding_t *lc = &cdc_acm_line_coding;

	lc->bitrate = 150000;
	lc->format = 0x00;
	lc->parity_type = 0x00;
	lc->data_type = 0x08;

	return HAL_OK;
}

/**
  * @brief  DeInitializes the CDC media layer
  * @param  None
  * @retval Status
  */
static int cdc_acm_cb_deinit(void)
{
	return HAL_OK;
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface through this function.
  * @param  Buf: RX buffer
  * @param  Len: RX data length (in bytes)
  * @retval Status
  */
static int cdc_acm_cb_received(u8 *buf, u32 len)
{
	static u32 start_time = 0, end_time = 0, ds_count = 0;
	char *str_find = NULL;

	if (len == 0) {
		return 0;
	}

	if (len == 5 && strncmp((char *)buf, ">>>", 3) == 0) {
		tt_mode_task_start = 1;
		rtos_sema_give(tt_mode_tx_sema);
		printf("tt mode start\r\n");
	}

	if (tt_mode_task_start) {
		if (memcmp(buf, ATCMD_TT_MODE_HIGH_WATERMARK_STR, strlen(ATCMD_TT_MODE_HIGH_WATERMARK_STR)) == 0) {
			high_water_mark = 1;
		} else if (memcmp(buf, ATCMD_TT_MODE_LOW_WATERMARK_STR, strlen(ATCMD_TT_MODE_LOW_WATERMARK_STR)) == 0) {
			high_water_mark = 0;
		}
		uart_send_string(&sobj, (char *)buf, len);
	} else {
		if (memcmp(buf, ATCMD_DOWNSTREAM_TEST_START_STR, strlen(ATCMD_DOWNSTREAM_TEST_START_STR)) == 0) {
			start_time = xTaskGetTickCount();
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, ATCMD_DOWNSTREAM_TEST_START_STR);
		}

		if (start_time != 0) {
			str_find = strstr((char *)buf, ATCMD_DOWNSTREAM_TEST_END_STR);
			if (str_find != NULL) {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, ATCMD_DOWNSTREAM_TEST_END_STR);
				end_time = xTaskGetTickCount();
				ds_count = ds_count + ((u8 *)str_find - buf);
				uart_format_string_output("downstream test: Receive %lu KBytes in %lu ms, %d Kbits/sec\n\r", ds_count / 1024,
										  (end_time - start_time), (int)((ds_count * 8) / (end_time - start_time)));
				start_time = 0;
				end_time = 0;
				ds_count = 0;

				if (strstr((char *)buf, "OK")) {
					uart_send_string(&sobj, "\r\nOK\r\n", strlen("\r\nOK\r\n"));
				}
			} else {
				ds_count += len;
			}
		} else {
			uart_send_string(&sobj, (char *)buf, len);
		}
	}

	return 0;
}

void cdc_acm_cb_transmitted(u8 status)
{
	if (status == HAL_OK) {
		rtos_sema_give(atcmd_usbd_tx_done_sema);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "TX fail: %d\n", status);
	}
}

static int cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	usbd_cdc_acm_line_coding_t *lc = &cdc_acm_line_coding;

	switch (req->bRequest) {
	case CDC_SEND_ENCAPSULATED_COMMAND:
		/* Do nothing */
		break;

	case CDC_GET_ENCAPSULATED_RESPONSE:
		/* Do nothing */
		break;

	case CDC_SET_COMM_FEATURE:
		/* Do nothing */
		break;

	case CDC_GET_COMM_FEATURE:
		/* Do nothing */
		break;

	case CDC_CLEAR_COMM_FEATURE:
		/* Do nothing */
		break;

	case CDC_SET_LINE_CODING:
		if (req->wLength == CDC_ACM_LINE_CODING_SIZE) {
			lc->bitrate = (u32)(buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24));
			lc->format = buf[4];
			lc->parity_type = buf[5];
			lc->data_type = buf[6];
		}
		break;

	case CDC_GET_LINE_CODING:
		buf[0] = (u8)(lc->bitrate & 0xFF);
		buf[1] = (u8)((lc->bitrate >> 8) & 0xFF);
		buf[2] = (u8)((lc->bitrate >> 16) & 0xFF);
		buf[3] = (u8)((lc->bitrate >> 24) & 0xFF);
		buf[4] = lc->format;
		buf[5] = lc->parity_type;
		buf[6] = lc->data_type;
		break;

	case CDC_SET_CONTROL_LINE_STATE:
		/*
		wValue:	wValue, Control Signal Bitmap
				D2-15:	Reserved, 0
				D1:	RTS, 0 - Deactivate, 1 - Activate
				D0:	DTR, 0 - Not Present, 1 - Present
		*/
		cdc_acm_ctrl_line_state = req->wValue;
		if (cdc_acm_ctrl_line_state & 0x01) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "VCOM port activate\n");
#if CONFIG_CDC_ACM_NOTIFY
			usbd_cdc_acm_notify_serial_state(CDC_ACM_CTRL_DSR | CDC_ACM_CTRL_DCD);
#endif
		}
		break;

	case CDC_SEND_BREAK:
		/* Do nothing */
		break;

	default:
		break;
	}

	return HAL_OK;
}

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
	RTK_LOGS(TAG, RTK_LOG_INFO, "Status change: %d -> %d \n", old_status, status);

#if CONFIG_USBD_CDC_ACM_HOTPLUG
	cdc_acm_attach_status = status;
	rtos_sema_give(cdc_acm_attach_status_changed_sema);
#endif
}

#if CONFIG_USBD_CDC_ACM_HOTPLUG
static void cdc_acm_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(cdc_acm_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (cdc_acm_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");
				usbd_cdc_acm_deinit();
				ret = usbd_deinit();
				if (ret != 0) {
					break;
				}
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
				ret = usbd_init(&cdc_acm_cfg);
				if (ret != 0) {
					break;
				}
				ret = usbd_cdc_acm_init(CONFIG_CDC_ACM_BULK_OUT_XFER_SIZE, CONFIG_CDC_ACM_BULK_IN_XFER_SIZE, &cdc_acm_cb);
				if (ret != 0) {
					usbd_deinit();
					break;
				}
			} else if (cdc_acm_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
			}
		}
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Hotplug thread exit\n");
	rtos_task_delete(NULL);
}
#endif // CONFIG_USBD_MSC_CHECK_USB_STATUS

static void example_usbd_cdc_acm_thread(void *param)
{
	int ret = 0;
#if CONFIG_USBD_CDC_ACM_HOTPLUG
	rtos_task_t check_task;
#endif

	UNUSED(param);

#if CONFIG_USBD_CDC_ACM_HOTPLUG
	rtos_sema_create(&cdc_acm_attach_status_changed_sema, 0, 1);
#endif

	/*init uart*/
	sobj.uart_idx = 0;
	serial_init(&sobj, PA_25, PA_26);
	serial_baud(&sobj, 115200);
	serial_format(&sobj, 8, ParityNone, 1);

	uart_send_string(&sobj, "UART Send Command Demo...\r\n", strlen("UART Send Command Demo...\r\n"));
	serial_irq_handler(&sobj, uart_irq, (uint32_t)&sobj);
	serial_irq_set(&sobj, RxIrq, 1);
	serial_irq_set(&sobj, TxIrq, 1);

	ret = usbd_init(&cdc_acm_cfg);
	if (ret != HAL_OK) {
		goto exit_usbd_init_fail;
	}

	ret = usbd_cdc_acm_init(CONFIG_CDC_ACM_BULK_OUT_XFER_SIZE, CONFIG_CDC_ACM_BULK_IN_XFER_SIZE, &cdc_acm_cb);
	if (ret != HAL_OK) {
		goto exit_usbd_cdc_acm_init_fail;
	}

#if CONFIG_USBD_CDC_ACM_HOTPLUG
	ret = rtos_task_create(&check_task, "cdc_acm_hotplug_thread", cdc_acm_hotplug_thread, NULL, 1024, CONFIG_CDC_ACM_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto exit_create_check_task_fail;
	}
#endif

	rtos_time_delay_ms(100);

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD CDC ACM demo start\n");

	rtos_task_delete(NULL);

	return;

#if CONFIG_USBD_CDC_ACM_HOTPLUG
exit_create_check_task_fail:
	usbd_cdc_acm_deinit();
#endif

exit_usbd_cdc_acm_init_fail:
	usbd_deinit();

exit_usbd_init_fail:
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD CDC ACM demo stop\n");
#if CONFIG_USBD_CDC_ACM_HOTPLUG
	rtos_sema_delete(cdc_acm_attach_status_changed_sema);
#endif

	rtos_task_delete(NULL);
}

static void tt_mode_test_task(void *param)
{
	UNUSED(param);
	u32 tt_len = 10 * 1024 * 1024;
	u32 send_len;
	u8 *tt_tx_buf = pvPortMalloc(CONFIG_CDC_ACM_BULK_OUT_XFER_SIZE);
	int ret;

	while (1) {
		while (tt_len > 0) {
			if (high_water_mark == 0) {
				xSemaphoreTake(tt_mode_tx_sema, 0xFFFFFFFF);
				send_len = tt_len > (CONFIG_CDC_ACM_BULK_OUT_XFER_SIZE) ? (CONFIG_CDC_ACM_BULK_OUT_XFER_SIZE) : tt_len;
				ret = usbd_cdc_acm_transmit(tt_tx_buf, send_len);
				while (ret != HAL_OK) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Xfer busy, retry[2]\r\n");
					rtos_time_delay_ms(1);
					ret = usbd_cdc_acm_transmit(tt_tx_buf, send_len);
				}
				xSemaphoreTake(atcmd_usbd_tx_done_sema, 0xFFFFFFFF);
				tt_len -= send_len;
				xSemaphoreGive(tt_mode_tx_sema);
			} else {
				vTaskDelay(20);
			}
		}

		printf("tt mode end\r\n");
		xSemaphoreTake(tt_mode_tx_sema, 0xFFFFFFFF);
		tt_len = 10 * 1024 * 1024;
		tt_mode_task_start = 0;
	}


	vPortFree(tt_tx_buf);
	vTaskDelete(NULL);
}

void uart_irq_handle_task(void)
{
	int ret;
	while (1) {
		xSemaphoreTake(uart_irq_handle_sema, 0xFFFFFFFF);
		ret = usbd_cdc_acm_transmit(uart_irq_buffer, uart_irq_count);
		while (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Xfer busy, retry[2]\r\n");
			rtos_time_delay_ms(1);
			ret = usbd_cdc_acm_transmit(uart_irq_buffer, uart_irq_count);
		}
		rtos_sema_take(atcmd_usbd_tx_done_sema, RTOS_SEMA_MAX_COUNT);
		memset(uart_irq_buffer, 0, MAX_CMD_LEN);
		uart_irq_count = 0;
	}
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  USB download de-initialize
  * @param  None
  * @retval Result of the operation: 0 if success else fail
  */
void example_atcmd_usbd_host(void)
{
	int ret;
	rtos_task_t task;

	uart_irq_handle_sema = (void *)xSemaphoreCreateCounting(0xFFFF, 0);
	tt_mode_tx_sema = (void *)xSemaphoreCreateCounting(1, 0);
	rtos_sema_create(&atcmd_usbd_tx_done_sema, 0, 0xFFFF);

	ret = rtos_task_create(&task, "example_usbd_cdc_acm_thread", example_usbd_cdc_acm_thread, NULL, 1024U, CONFIG_CDC_ACM_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD CDC ACM thread fail\n");
	}

	if (xTaskCreate((void *)uart_irq_handle_task, ((const char *)"uart_irq_handle_task"), 1024 / sizeof(portSTACK_TYPE), NULL, 5, NULL) != pdPASS) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r%s rtos_task_create(atcmd_spi_host_task) failed", __FUNCTION__);
	}

	xTaskCreate((void *)tt_mode_test_task, ((const char *)"tt_mode_test_task"), 1024 / sizeof(portSTACK_TYPE), NULL, 1, NULL);
}

