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
#include "atcmd_service.h"
#include "at_intf_usbd.h"

#ifdef CONFIG_USB_DEVICE_EN
/* Private defines -----------------------------------------------------------*/

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
// while test suspend/resume, hotplug should be disabled
#define CONFIG_USBD_CDC_ACM_HOTPLUG					0

// USB speed
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define CONFIG_USBD_CDC_ACM_SPEED					USB_SPEED_FULL
#else
#define CONFIG_USBD_CDC_ACM_SPEED					USB_SPEED_HIGH
#endif

// Do not change the settings unless indeed necessary
#define CONFIG_CDC_ACM_BULK_IN_XFER_SIZE			2048U
#define CONFIG_CDC_ACM_BULK_OUT_XFER_SIZE			2048U

// Thread priorities
#define CONFIG_CDC_ACM_HOTPLUG_THREAD_PRIORITY		8
#define CONFIG_CDC_ACM_XFER_THREAD_PRIORITY			5

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int atcmd_usbd_init(void);
static int atcmd_usbd_deinit(void);
static int atcmd_usbd_setup(usb_setup_req_t *req, u8 *buf);
static int atcmd_usbd_received(u8 *buf, u32 Len);
static void atcmd_usbd_transmitted(u8 status);
static void atcmd_usbd_status_changed(u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "AT_USBD";

static usbd_cdc_acm_cb_t atcmd_usbd_cb = {
	.init = atcmd_usbd_init,
	.deinit = atcmd_usbd_deinit,
	.setup = atcmd_usbd_setup,
	.received = atcmd_usbd_received,
	.transmitted = atcmd_usbd_transmitted,
	.status_changed = atcmd_usbd_status_changed,
};

static usbd_cdc_acm_line_coding_t atcmd_usbd_line_coding;

static u16 atcmd_usbd_ctrl_line_state;

static usbd_config_t atcmd_usbd_cfg = {
	.speed = CONFIG_USBD_CDC_ACM_SPEED,
	.isr_priority = INT_PRI_MIDDLE,
	.intr_use_ptx_fifo  = 0U,
#if defined(CONFIG_AMEBASMART)
	.nptx_max_epmis_cnt = 1U,
	.ext_intr_enable        = USBD_EPMIS_INTR,
#elif defined (CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 644U,
	.ptx_fifo_depth = {16U, 256U, 32U, 16U, 16U, },
#elif defined (CONFIG_AMEBAL2)
	.rx_fifo_depth = 661U,
	.ptx_fifo_depth = {256U, 16U, 32U, 16U, },
#endif
};

static u8 usbd_tx_buf[ATCMD_USBD_MAX_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
static rtos_sema_t atcmd_usbd_tx_sema;

static u8 atcmd_usbd_attach_status;

rtos_sema_t atcmd_usbd_tx_done_sema;
rtos_sema_t atcmd_usbd_rx_sema;
rtos_sema_t atcmd_usbd_notify_sema;

RingBuffer *at_usbd_rx_ring_buf = NULL;
RingBuffer *at_usbd_tx_ring_buf = NULL;
RingBuffer *at_usbd_notify_ring_buf = NULL;

extern volatile UART_LOG_CTL shell_ctl;
extern UART_LOG_BUF shell_rxbuf;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CDC media layer
  * @param  None
  * @retval Status
  */
static int atcmd_usbd_init(void)
{
	usbd_cdc_acm_line_coding_t *lc = &atcmd_usbd_line_coding;

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
static int atcmd_usbd_deinit(void)
{
	return HAL_OK;
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface through this function.
  * @param  Buf: RX buffer
  * @param  Len: RX data length (in bytes)
  * @retval Status
  */
static int atcmd_usbd_received(u8 *buf, u32 len)
{
	int ret = HAL_OK;
	u32 space = 0;

	if (g_tt_mode) {
		space = RingBuffer_Space(atcmd_tt_mode_rx_ring_buf);

		if (len > RingBuffer_Size(atcmd_tt_mode_rx_ring_buf)) {
			g_tt_mode_stop_flag = 1;
			g_tt_mode_stop_char_cnt = 0;
			rtos_sema_give(atcmd_tt_mode_sema);
			RTK_LOGE(TAG, "recv_len is larger than tt mode buffer size\n");
			return ret;
		}

		if (g_tt_mode_check_watermark) {
			if (space - len < MAX_TT_HEAP_SIZE * (1 - TT_MODE_HIGH_WATERMARK) && g_tt_mode_indicate_high_watermark == 0) {
				g_tt_mode_indicate_high_watermark = 1;
				g_tt_mode_indicate_low_watermark = 0;
				at_printf(ATCMD_TT_MODE_HIGH_WATERMARK_STR);
			}
		}

		/*recv stop char under tt mode*/
		if (len == 1 && buf[0] == '<') {
			g_tt_mode_stop_char_cnt++;
		} else {
			g_tt_mode_stop_char_cnt = 0;

			if (space >= len) {
				RingBuffer_Write(atcmd_tt_mode_rx_ring_buf, buf, len);
				rtos_sema_give(atcmd_tt_mode_sema);
			} else if (space > 0) {
				RingBuffer_Write(atcmd_tt_mode_rx_ring_buf, buf, space);
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
		space = RingBuffer_Space(at_usbd_rx_ring_buf);
		if (space >= len) {
			RingBuffer_Write(at_usbd_rx_ring_buf, buf, len);
			rtos_sema_give(atcmd_usbd_rx_sema);
		} else {
			RTK_LOGW(TAG, "at_usbd_rx_ring_buf is full, drop data\n");
			ret = HAL_BUSY;
		}
	}

	return ret;
}

void atcmd_usbd_transmitted(u8 status)
{
	if (status == HAL_OK) {
		rtos_sema_give(atcmd_usbd_tx_done_sema);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "TX fail: %d\n", status);
	}
}

/**
  * @brief  Handle the CDC class control requests
  * @param  cmd: Command code
  * @param  buf: Buffer containing command data (request parameters)
  * @param  len: Number of data to be sent (in bytes)
  * @retval Status
  */
static int atcmd_usbd_setup(usb_setup_req_t *req, u8 *buf)
{
	usbd_cdc_acm_line_coding_t *lc = &atcmd_usbd_line_coding;

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
		atcmd_usbd_ctrl_line_state = req->wValue;
		if (atcmd_usbd_ctrl_line_state & 0x01) {
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

static void atcmd_usbd_status_changed(u8 old_status, u8 status)
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

	atcmd_usbd_attach_status = status;
}

static void atcmd_usbd_tx_task(void *param)
{
	int ret;
	u32 send_len, remain_len;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(atcmd_usbd_tx_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			remain_len = RingBuffer_Available(at_usbd_tx_ring_buf);
			while (remain_len > 0) {
				send_len = (remain_len > ATCMD_USBD_MAX_SIZE) ? ATCMD_USBD_MAX_SIZE : remain_len;

				RingBuffer_Read(at_usbd_tx_ring_buf, usbd_tx_buf, send_len);
				ret = usbd_cdc_acm_transmit(usbd_tx_buf, send_len);
				while (ret != HAL_OK) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Xfer busy, retry[2]\r\n");
					rtos_time_delay_ms(1);
					ret = usbd_cdc_acm_transmit(usbd_tx_buf, send_len);
				}
				rtos_sema_take(atcmd_usbd_tx_done_sema, RTOS_SEMA_MAX_COUNT);

				remain_len -= send_len;
			}
		}
	}

	rtos_task_delete(NULL);
}

static void atcmd_usbd_notify_task(void *param)
{
	int ret;
	u32 send_len, remain_len;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(atcmd_usbd_notify_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			remain_len = RingBuffer_Available(at_usbd_notify_ring_buf);
			while (remain_len > 0) {
				send_len = (remain_len > 2) ? 2 : remain_len;

				RingBuffer_Read(at_usbd_notify_ring_buf, usbd_tx_buf, send_len);
				ret = usbd_cdc_acm_notify_serial_state(*((u16 *)usbd_tx_buf));
				while (ret != HAL_OK) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Xfer busy, retry[2]\r\n");
					rtos_time_delay_ms(1);
					ret = usbd_cdc_acm_notify_serial_state(*((u16 *)usbd_tx_buf));
				}
				remain_len -= send_len;
			}
		}
	}

	rtos_task_delete(NULL);
}


static int atcmd_usbd_thread_init(void)
{
	int ret = 0;

	rtos_sema_create(&atcmd_usbd_tx_sema, 0, 0xFFFF);

	ret = usbd_init(&atcmd_usbd_cfg);
	if (ret != HAL_OK) {
		goto exit_usbd_init_fail;
	}

	ret = usbd_cdc_acm_init(CONFIG_CDC_ACM_BULK_OUT_XFER_SIZE, CONFIG_CDC_ACM_BULK_IN_XFER_SIZE, &atcmd_usbd_cb);
	if (ret != HAL_OK) {
		goto exit_usbd_cdc_acm_init_fail;
	}

	// The priority of transfer thread shall be lower than USB isr priority
	ret = rtos_task_create(NULL, "atcmd_usbd_tx_task", atcmd_usbd_tx_task, NULL, 1024, CONFIG_CDC_ACM_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto exit_usbd_cdc_acm_init_fail;
	}

	ret = rtos_task_create(NULL, "atcmd_usbd_notify_task", atcmd_usbd_notify_task, NULL, 1024, CONFIG_CDC_ACM_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto exit_usbd_cdc_acm_init_fail;
	}

	rtos_time_delay_ms(100);

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD CDC ACM demo start\n");

	return ret;

exit_usbd_cdc_acm_init_fail:
	usbd_deinit();

exit_usbd_init_fail:
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD CDC ACM demo stop\n");
	rtos_sema_delete(atcmd_usbd_tx_sema);

	return ret;
}

void atcmd_usbd_input_handler_task(void)
{
	PUART_LOG_BUF pShellRxBuf = &shell_rxbuf;
	u32 i = 0, actual_len = 0;
	while (1) {
		pShellRxBuf->BufCount = 0;
		i = 0;

		rtos_sema_take(atcmd_usbd_rx_sema, 0xFFFFFFFF);

		actual_len = RingBuffer_Available(at_usbd_rx_ring_buf);
		if (actual_len == 0) {
			continue;
		}

		actual_len = actual_len > CMD_BUFLEN ? CMD_BUFLEN : actual_len;
		RingBuffer_Read(at_usbd_rx_ring_buf, pShellRxBuf->UARTLogBuf, actual_len);

		pShellRxBuf->BufCount = actual_len;

recv_again:
		if (shell_cmd_chk(pShellRxBuf->UARTLogBuf[i++], (UART_LOG_CTL *)&shell_ctl, ENABLE) == 2) {
			if (shell_ctl.pTmpLogBuf != NULL) {
				shell_ctl.ExecuteCmd = TRUE;

				if (shell_ctl.shell_task_rdy) {
					if (RingBuffer_Available(at_usbd_rx_ring_buf) > 0) {
						RingBuffer_Reset(at_usbd_rx_ring_buf);
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

static void atio_usbd_output(char *buf, int len)
{
	int space = 0;
	int send_len;

	u8 *buf_tmp;
	rtos_sema_t sema_tmp;
	RingBuffer *ringbuf_tmp;
	u8 notify_data[2];

	if (g_tt_mode) {
		sema_tmp = atcmd_usbd_notify_sema;
		ringbuf_tmp = at_usbd_notify_ring_buf;
		buf_tmp = notify_data;
		if (memcmp(buf, ATCMD_TT_MODE_HIGH_WATERMARK_STR, strlen(ATCMD_TT_MODE_HIGH_WATERMARK_STR)) == 0) {
			notify_data[0] = 'H';
			notify_data[1] = 'W';
		} else if (memcmp(buf, ATCMD_TT_MODE_LOW_WATERMARK_STR, strlen(ATCMD_TT_MODE_LOW_WATERMARK_STR)) == 0) {
			notify_data[0] = 'L';
			notify_data[1] = 'W';
		}
		send_len = 2;
	} else {
		sema_tmp = atcmd_usbd_tx_sema;
		ringbuf_tmp = at_usbd_tx_ring_buf;
		buf_tmp = (u8 *)buf;
		send_len = len;
	}

	while (1) {
		space = RingBuffer_Space(ringbuf_tmp);
		if (space >= send_len) {
			RingBuffer_Write(ringbuf_tmp, buf_tmp, send_len);
			break;
		} else if (space > 0) {
			RingBuffer_Write(ringbuf_tmp, buf_tmp, space);
			send_len -= space;
			buf_tmp += space;
		}

		rtos_time_delay_ms(1);
	}

	if (atcmd_usbd_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
		rtos_sema_give(sema_tmp);
	}

}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  USB download de-initialize
  * @param  None
  * @retval Result of the operation: 0 if success else fail
  */
int atio_usbd_init(void)
{
	rtos_sema_create(&atcmd_usbd_rx_sema, 0, 0xFFFF);
	rtos_sema_create(&atcmd_usbd_notify_sema, 0, 0xFFFF);
	rtos_sema_create(&atcmd_usbd_tx_done_sema, 0, 0xFFFF);

	at_usbd_rx_ring_buf = RingBuffer_Create(NULL, 4 * 1024, LOCAL_RINGBUFF, 1);
	at_usbd_tx_ring_buf = RingBuffer_Create(NULL, 32 * 1024, LOCAL_RINGBUFF, 1);
	at_usbd_notify_ring_buf = RingBuffer_Create(NULL, 256, LOCAL_RINGBUFF, 1);

	if (atcmd_usbd_rx_sema == NULL || at_usbd_rx_ring_buf == NULL || at_usbd_tx_ring_buf == NULL) {
		return -1;
	}

	out_buffer = atio_usbd_output;

	if (atcmd_usbd_thread_init() != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ATCMD usbd init failed\n");
		return -1;
	}

	if (rtos_task_create(NULL, ((const char *)"atcmd_usbd_input_handler_task"), (rtos_task_t)atcmd_usbd_input_handler_task, NULL, 1024, 5) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "\n\r%s rtos_task_create(atcmd_usbd_input_handler_task) failed", __FUNCTION__);
		return -1;
	}

	return 0;
}
#endif
