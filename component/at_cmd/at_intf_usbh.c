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

#ifdef CONFIG_USB_HOST_EN

/* Private defines -----------------------------------------------------------*/

#define CONFIG_USBH_CDC_ACM_HOT_PLUG_TEST 1     /* Hot plug / memory leak test */

#define USBH_CDC_ACM_LOOPBACK_BUF_SIZE    2048  /* Buffer size for loopback test, which should match with device loopback buffer size */

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
static int cdc_acm_cb_line_coding_changed(usbh_cdc_acm_line_coding_t *line_coding);
static int cdc_acm_cb_process(usb_host_t *host, u8 id);

void atio_usbd_output(char *buf, int len);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "ACM";

static u8 cdc_acm_loopback_tx_buf[USBH_CDC_ACM_LOOPBACK_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 cdc_acm_loopback_rx_buf[USBH_CDC_ACM_LOOPBACK_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

u8 uart_show_buf[USBH_CDC_ACM_LOOPBACK_BUF_SIZE] = {0};
char uart_format_buffer[FORMAT_LEN];

rtos_sema_t cdc_acm_detach_sema;
rtos_sema_t cdc_acm_attach_sema;
rtos_sema_t cdc_acm_receive_sema;
rtos_sema_t cdc_acm_send_sema;

rtos_sema_t atcmd_usbh_rx_sema;
rtos_sema_t atcmd_usbh_tx_sema;

rtos_sema_t tt_mode_tx_sema;

rtos_sema_t uart_irq_handle_sema;
rtos_mutex_t usbh_rx_ringbuf_mutex = NULL;

static __IO u32 atcmd_usbh_rx_len = 0;
RingBuffer *at_usbh_rx_ring_buf = NULL;
RingBuffer *at_usbh_tx_ring_buf = NULL;

static __IO int cdc_acm_is_ready = 0;

char uart_irq_buffer[MAX_CMD_LEN] = {0};
u32 uart_irq_count = 0;

extern volatile UART_LOG_CTL shell_ctl;
extern UART_LOG_BUF shell_rxbuf;

static usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_priority = 4U,
	.sof_tick_enable = 1U,

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
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
	rtos_sema_give(cdc_acm_attach_sema);
	return HAL_OK;
}

static int cdc_acm_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
	rtos_sema_give(cdc_acm_detach_sema);
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

	if (length != 0) {
		atcmd_usbh_rx_len = length > USBH_CDC_ACM_LOOPBACK_BUF_SIZE ? USBH_CDC_ACM_LOOPBACK_BUF_SIZE : length;
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
		RTK_LOGS(TAG, RTK_LOG_ERROR, "TX fail: %d\n", state);
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

static void usbh_tx_task(void *param)
{
	UNUSED(param);
	int ret;
	u32 send_len, remain_len;

	RTK_LOGS(TAG, RTK_LOG_INFO, "Wait for device attach\n");

WAIT_CONNECT:
	while (1) {
		if (cdc_acm_is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}

	while (1) {
		rtos_sema_take(atcmd_usbh_tx_sema, RTOS_SEMA_MAX_COUNT);
		remain_len = RingBuffer_Available(at_usbh_tx_ring_buf);
		while (remain_len > 0) {
			send_len = (remain_len > USBH_CDC_ACM_LOOPBACK_BUF_SIZE) ? USBH_CDC_ACM_LOOPBACK_BUF_SIZE : remain_len;

			RingBuffer_Read(at_usbh_tx_ring_buf, cdc_acm_loopback_tx_buf, send_len);
			ret = usbh_cdc_acm_transmit(cdc_acm_loopback_tx_buf, send_len);
			while (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Xfer busy, retry[2]\r\n");
				rtos_time_delay_ms(1);
				ret = usbh_cdc_acm_transmit(cdc_acm_loopback_tx_buf, send_len);
			}
			rtos_sema_take(cdc_acm_send_sema, RTOS_SEMA_MAX_COUNT);

			remain_len -= send_len;

			if (!cdc_acm_is_ready) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Device disconnect\n");
				goto WAIT_CONNECT;
			}
		}
	}

	rtos_task_delete(NULL);
}

static void usbh_rx_task(void *param)
{
	int ret;
	u32 space = 0;
	UNUSED(param);

WAIT_CONNECT:
	while (1) {
		if (cdc_acm_is_ready) {
			rtos_time_delay_ms(10);
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
			rtos_time_delay_ms(1);
			ret = usbh_cdc_acm_receive(cdc_acm_loopback_rx_buf, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
		}

		rtos_sema_take(cdc_acm_receive_sema, RTOS_SEMA_MAX_COUNT);

		if (atcmd_usbh_rx_len == 0) {
			continue;
		}

		if (g_tt_mode) {
			space = RingBuffer_Space(atcmd_tt_mode_rx_ring_buf);

			if (atcmd_usbh_rx_len > RingBuffer_Size(atcmd_tt_mode_rx_ring_buf)) {
				g_tt_mode_stop_flag = 1;
				g_tt_mode_stop_char_cnt = 0;
				rtos_sema_give(atcmd_tt_mode_sema);
				RTK_LOGE(TAG, "recv_len is larger than tt mode buffer size\n");
				continue;
			}

			if (g_tt_mode_check_watermark) {
				if (space - atcmd_usbh_rx_len < MAX_TT_HEAP_SIZE * (1 - TT_MODE_HIGH_WATERMARK) && g_tt_mode_indicate_high_watermark == 0) {
					g_tt_mode_indicate_high_watermark = 1;
					g_tt_mode_indicate_low_watermark = 0;
					at_printf(ATCMD_TT_MODE_HIGH_WATERMARK_STR);
				}
			}

			/*recv stop char under tt mode*/
			if (atcmd_usbh_rx_len == 1 && cdc_acm_loopback_rx_buf[0] == '<') {
				g_tt_mode_stop_char_cnt++;
			} else {
				g_tt_mode_stop_char_cnt = 0;

				if (space >= atcmd_usbh_rx_len) {
					RingBuffer_Write(atcmd_tt_mode_rx_ring_buf, cdc_acm_loopback_rx_buf, atcmd_usbh_rx_len);
					rtos_sema_give(atcmd_tt_mode_sema);
				} else if (space > 0) {
					RingBuffer_Write(atcmd_tt_mode_rx_ring_buf, cdc_acm_loopback_rx_buf, space);
					rtos_sema_give(atcmd_tt_mode_sema);
					RTK_LOGW(TAG, "atcmd_tt_mode_rx_ring_buf is full, drop partial data\n");
				} else {
					RTK_LOGW(TAG, "atcmd_tt_mode_rx_ring_buf is full, drop data\n");
				}
			}

			/*cancel tt mode stop timer if recv pkt before timeout*/
			if (g_tt_mode_stop_flag == 0 && rtos_timer_is_timer_active(xTimers_TT_Mode)) {
				rtos_timer_stop(xTimers_TT_Mode, 0);
				g_tt_mode_stop_char_cnt = 0;
			}

			/*start tt mode stop timer once*/
			if (g_tt_mode_stop_char_cnt >= 3) {
				if (rtos_timer_is_timer_active(xTimers_TT_Mode) == 0) {
					rtos_timer_start(xTimers_TT_Mode, 0);
				}
			}

		} else {
			space = RingBuffer_Space(at_usbh_rx_ring_buf);
			if (space >= atcmd_usbh_rx_len) {
				rtos_mutex_take(usbh_rx_ringbuf_mutex, MUTEX_WAIT_TIMEOUT);
				RingBuffer_Write(at_usbh_rx_ring_buf, cdc_acm_loopback_rx_buf, atcmd_usbh_rx_len);
				rtos_mutex_give(usbh_rx_ringbuf_mutex);
				rtos_sema_give(atcmd_usbh_rx_sema);
			} else {
				RTK_LOGW(TAG, "at_usbd_rx_ring_buf is full, drop data\n");
				ret = HAL_BUSY;
			}
		}
	}
	rtos_task_delete(NULL);
}

#if CONFIG_USBH_CDC_ACM_HOT_PLUG_TEST
static void cdc_acm_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(cdc_acm_detach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			rtos_time_delay_ms(100);
			usbh_cdc_acm_deinit();
			usbh_deinit();
			rtos_time_delay_ms(10);

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

	rtos_task_delete(NULL);
}
#endif

static void atcmd_usbh_cdc_acm_task(void *param)
{
	int status;

	UNUSED(param);

	rtos_sema_create(&cdc_acm_detach_sema, 0, 1);
	rtos_sema_create(&cdc_acm_attach_sema, 0, 1);
	rtos_sema_create(&cdc_acm_receive_sema, 0, 1);
	rtos_sema_create(&cdc_acm_send_sema, 0, 1);

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

	if (rtos_sema_take(cdc_acm_attach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
		xTaskCreate((void *)usbh_tx_task, ((const char *)"usbh_tx_task"), 1024 / sizeof(portSTACK_TYPE), NULL, 3, NULL);
		xTaskCreate((void *)usbh_rx_task, ((const char *)"usbh_rx_task"), 1024 / sizeof(portSTACK_TYPE), NULL, 4, NULL);
	}

	goto example_exit;
error_exit:
	rtos_sema_delete(cdc_acm_detach_sema);
	rtos_sema_delete(cdc_acm_attach_sema);
	rtos_sema_delete(cdc_acm_receive_sema);
	rtos_sema_delete(cdc_acm_send_sema);
example_exit:
	rtos_task_delete(NULL);
}

void atcmd_usbh_input_handler_task(void)
{
	PUART_LOG_BUF pShellRxBuf = &shell_rxbuf;
	u32 i = 0, actual_len = 0;
	while (1) {
		pShellRxBuf->BufCount = 0;
		i = 0;

		rtos_sema_take(atcmd_usbh_rx_sema, 0xFFFFFFFF);

		actual_len = RingBuffer_Available(at_usbh_rx_ring_buf);
		if (actual_len == 0) {
			continue;
		}

		actual_len = actual_len > CMD_BUFLEN ? CMD_BUFLEN : actual_len;
		rtos_mutex_take(usbh_rx_ringbuf_mutex, MUTEX_WAIT_TIMEOUT);
		RingBuffer_Read(at_usbh_rx_ring_buf, pShellRxBuf->UARTLogBuf, actual_len);
		rtos_mutex_give(usbh_rx_ringbuf_mutex);

		pShellRxBuf->BufCount = actual_len;

recv_again:
		if (shell_cmd_chk(pShellRxBuf->UARTLogBuf[i++], (UART_LOG_CTL *)&shell_ctl, ENABLE) == 2) {
			if (shell_ctl.pTmpLogBuf != NULL) {
				shell_ctl.ExecuteCmd = TRUE;

				if (shell_ctl.shell_task_rdy) {
					if (RingBuffer_Available(at_usbh_rx_ring_buf) > 0) {
						rtos_mutex_take(usbh_rx_ringbuf_mutex, MUTEX_WAIT_TIMEOUT);
						RingBuffer_Reset(at_usbh_rx_ring_buf);
						rtos_mutex_give(usbh_rx_ringbuf_mutex);
					}
					shell_ctl.GiveSema();
					continue;
				}
			} else {
				memset((u8 *)shell_ctl.pTmpLogBuf->UARTLogBuf, CMD_BUFLEN, '\0');
			}
		}

		/* recv all data one time */
		if ((pShellRxBuf->BufCount != i) && (pShellRxBuf->BufCount != 0)) {
			goto recv_again;
		}
	}
}

void atio_usbd_output(char *buf, int len)
{
	int space = 0;
	int send_len;
	u8 *buf_tmp;
	buf_tmp = (u8 *)buf;
	send_len = len;

	while (1) {
		space = RingBuffer_Space(at_usbh_tx_ring_buf);
		if (space >= send_len) {
			RingBuffer_Write(at_usbh_tx_ring_buf, buf_tmp, send_len);
			break;
		} else if (space > 0) {
			RingBuffer_Write(at_usbh_tx_ring_buf, buf_tmp, space);
			send_len -= space;
			buf_tmp += space;
		}

		rtos_time_delay_ms(1);
	}

	if (cdc_acm_is_ready) {
		rtos_sema_give(atcmd_usbh_tx_sema);
	}
}

void atio_usbh_init(void)
{
	rtos_sema_create(&uart_irq_handle_sema, 0U, 1U);
	rtos_sema_create(&tt_mode_tx_sema, 1U, 1U);
	rtos_mutex_create(&usbh_rx_ringbuf_mutex);
	rtos_sema_create(&atcmd_usbh_rx_sema, 0, 0xFFFF);
	rtos_sema_create(&atcmd_usbh_tx_sema, 0, 0xFFFF);

	at_usbh_rx_ring_buf = RingBuffer_Create(NULL, 4 * 1024, LOCAL_RINGBUFF, 1);
	at_usbh_tx_ring_buf = RingBuffer_Create(NULL, 32 * 1024, LOCAL_RINGBUFF, 1);

	out_buffer = atio_usbd_output;

	if (xTaskCreate((void *)atcmd_usbh_cdc_acm_task, ((const char *)"atcmd_usbh_cdc_acm_task"), 1024 / sizeof(portSTACK_TYPE), NULL, 1, NULL) != pdPASS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
	}

	if (rtos_task_create(NULL, ((const char *)"atcmd_usbh_input_handler_task"), (rtos_task_t)atcmd_usbh_input_handler_task, NULL, 1024, 5) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "\n\r%s rtos_task_create(atcmd_usbh_input_handler_task) failed", __FUNCTION__);
	}
}
#endif

