/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "usbd.h"
#include "usbd_cdc_acm.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/

static const char *const TAG = "USBD";

// USB speed
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define CONFIG_USBD_CDC_ACM_SPEED					USB_SPEED_FULL
#else
#define CONFIG_USBD_CDC_ACM_SPEED					USB_SPEED_HIGH
#endif

// Thread priorities
#define CONFIG_CDC_ACM_INIT_THREAD_PRIORITY			5
#define CONFIG_CDC_ACM_XFER_THREAD_PRIORITY			5

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int cdc_acm_verify_cb_init(void);
static int cdc_acm_verify_cb_deinit(void);
static int cdc_acm_verify_cb_setup(usb_setup_req_t *req, u8 *buf);
static int cdc_acm_verify_cb_received(u8 *buf, u32 Len);
static void cdc_acm_verify_cb_transmitted(u8 status);
static void cdc_acm_verify_cb_status_changed(u8 status);

/* Private variables ---------------------------------------------------------*/

static usbd_cdc_acm_cb_t cdc_acm_verify_cb = {
	.init = cdc_acm_verify_cb_init,
	.deinit = 	cdc_acm_verify_cb_deinit,
	.setup = cdc_acm_verify_cb_setup,
	.received = cdc_acm_verify_cb_received,
	.transmitted = cdc_acm_verify_cb_transmitted,
	.status_changed = cdc_acm_verify_cb_status_changed
};

static usbd_cdc_acm_line_coding_t cdc_acm_verify_line_coding;

static u16 cdc_acm_verify_ctrl_line_state;

static usbd_config_t cdc_acm_verify_cfg = {
	.speed = CONFIG_USBD_CDC_ACM_SPEED,
	.dma_enable = 1U,
	.isr_priority = INT_PRI_MIDDLE,
	.intr_use_ptx_fifo = 0U,
#if defined(CONFIG_AMEBASMART)
	.nptx_max_epmis_cnt = 10U,
	.nptx_max_err_cnt = {0U, 0U, 0U, 2000U, },
	.ext_intr_en =  USBD_EPMIS_INTR,
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

static __IO u8 cdc_acm_verify_ready;
static __IO u8 cdc_acm_verify_rx_en;
static __IO u8 cdc_acm_verify_tx_en;
static u16 cdc_acm_verify_xfer_size = 2048U;
static u8 *cdc_acm_verify_xfer_buf;
static void *cdc_acm_verify_xfer_task;
static void *cdc_acm_verify_xfer_sema;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CDC media layer
  * @param  None
  * @retval Status
  */
static int cdc_acm_verify_cb_init(void)
{
	usbd_cdc_acm_line_coding_t *lc = &cdc_acm_verify_line_coding;

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
static int cdc_acm_verify_cb_deinit(void)
{
	return HAL_OK;
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface through this function.
  * @param  Buf: RX buffer
  * @param  Len: RX data length (in bytes)
  * @retval Status
  */
static int cdc_acm_verify_cb_received(u8 *buf, u32 len)
{
	if ((0 == cdc_acm_verify_rx_en) && (0 == cdc_acm_verify_tx_en)) {
		usbd_cdc_acm_transmit(buf, len);
	}
	return HAL_OK;
}

static void cdc_acm_verify_cb_transmitted(u8 status)
{
	if (status == HAL_OK) {
		if (cdc_acm_verify_tx_en) {
			usb_os_sema_give(cdc_acm_verify_xfer_sema);
		}
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
static int cdc_acm_verify_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	usbd_cdc_acm_line_coding_t *lc = &cdc_acm_verify_line_coding;

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
		cdc_acm_verify_ctrl_line_state = req->wValue;
		if (cdc_acm_verify_ctrl_line_state & 0x01) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "VCOM port activate\n");
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

static void cdc_acm_verify_cb_status_changed(u8 status)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Status change: %d\n", status);
}

static void cdc_acm_verify_xfer_thread(void *param)
{
	UNUSED(param);

	for (;;) {
		if (usb_os_sema_take(cdc_acm_verify_xfer_sema, 0xFFFFFFFFU) == HAL_OK) {
			if (cdc_acm_verify_tx_en) {
				usbd_cdc_acm_transmit(cdc_acm_verify_xfer_buf, cdc_acm_verify_xfer_size);
				if (cdc_acm_verify_xfer_buf[0]++ > '9') {
					cdc_acm_verify_xfer_buf[0] = '0';
				}
			}
		}
	}
}

static void cdc_acm_verify_deinit(void)
{
	cdc_acm_verify_ready = 0U;
	cdc_acm_verify_rx_en = 0U;
	cdc_acm_verify_tx_en = 0U;

	rtos_task_delete(cdc_acm_verify_xfer_task);

	usbd_cdc_acm_deinit();

	usbd_deinit();

	if (cdc_acm_verify_xfer_buf) {
		usb_os_mfree(cdc_acm_verify_xfer_buf);
		cdc_acm_verify_xfer_buf = NULL;
	}

	usb_os_sema_delete(cdc_acm_verify_xfer_sema);
}

static void cdc_acm_verify_init_thread(void *param)
{
	int i;
	int ret = 0;
	u8 *buf = NULL;

	UNUSED(param);

	cdc_acm_verify_rx_en = 0U;
	cdc_acm_verify_tx_en = 0U;

	usb_os_sema_create(&cdc_acm_verify_xfer_sema);

	buf = (u8 *)usb_os_malloc(cdc_acm_verify_xfer_size);
	if (buf != NULL) {
		buf[0] = '0';
		for (i = 1; i < cdc_acm_verify_xfer_size; ++i) {
			buf[i] = (i % 2 == 0) ? 'A' : 'B';
		}
		cdc_acm_verify_xfer_buf = buf;
	} else {
		goto exit_alloc_buffer_fail;
	}

	ret = usbd_init(&cdc_acm_verify_cfg);
	if (ret != HAL_OK) {
		goto exit_usbd_init_fail;
	}

	ret = usbd_cdc_acm_init(cdc_acm_verify_xfer_size, cdc_acm_verify_xfer_size, &cdc_acm_verify_cb);
	if (ret != HAL_OK) {
		goto exit_usbd_cdc_acm_init_fail;
	}

	// The priority of transfer thread shall be lower than USB isr priority
	ret = rtos_task_create(&cdc_acm_verify_xfer_task, "cdc_acm_verify_xfer_thread", cdc_acm_verify_xfer_thread, NULL, 1024, CONFIG_CDC_ACM_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto exit_create_xfer_task_fail;
	}

	cdc_acm_verify_ready = 1U;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD CDC ACM demo start\n");

	rtos_task_delete(NULL);

	return;

exit_create_xfer_task_fail:
	usbd_cdc_acm_deinit();

exit_usbd_cdc_acm_init_fail:
	usbd_deinit();

exit_usbd_init_fail:
	if (cdc_acm_verify_xfer_buf) {
		usb_os_mfree(cdc_acm_verify_xfer_buf);
		cdc_acm_verify_xfer_buf = NULL;
	}

exit_alloc_buffer_fail:
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD CDC ACM demo stop\n");
	usb_os_sema_delete(cdc_acm_verify_xfer_sema);

	rtos_task_delete(NULL);
}

/**
  * @brief  USB download de-initialize
  * @param  None
  * @retval Result of the operation: 0 if success else fail
  */
static int cdc_acm_verify_init(void)
{
	int ret;
	void *task;

	ret = rtos_task_create(&task, "cdc_acm_verify_init_thread", cdc_acm_verify_init_thread, NULL, 1024, CONFIG_CDC_ACM_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to create CDC ACM init thread\n");
	}

	return ret;
}

static void cdc_acm_verify_usage(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Invalid arguments, usage:\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbd acm xlen <xfer length in byte>\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbd acm speed <speed, 0 - high, 1 - high in full, 3 - full>\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbd acm dma <en - enable, dis - disable>\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbd acm init\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbd acm rx\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbd acm tx\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbd acm trx\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbd acm deinit\n");
}

/* Exported functions --------------------------------------------------------*/

int cmd_usbd_cdc_acm(u16 argc, u8 *argv[])
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
		if (cdc_acm_verify_ready) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Already init, please deinit first\n");
		} else {
			speed = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
			if ((speed == USB_SPEED_HIGH) || (speed == USB_SPEED_HIGH_IN_FULL) || (speed == USB_SPEED_FULL)) {
				cdc_acm_verify_cfg.speed = speed;
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
		if (cdc_acm_verify_ready) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Already init, please deinit first\n");
		} else {
			if ((_stricmp((const char *)argv[2], (const char *)"en") == 0)) {
				cdc_acm_verify_cfg.dma_enable = 1U;
				RTK_LOGS(TAG, RTK_LOG_INFO, "DMA enabled\n");
			} else if ((_stricmp((const char *)argv[2], (const char *)"dis") == 0)) {
				cdc_acm_verify_cfg.dma_enable = 0U;
				RTK_LOGS(TAG, RTK_LOG_INFO, "DMA disabled\n");
			} else {
				cdc_acm_verify_usage();
				ret = HAL_ERR_PARA;
			}
		}
	} else if (_stricmp(cmd, "xlen") == 0) {
		if (argc < 3) {
			cdc_acm_verify_usage();
			return HAL_ERR_PARA;
		}
		if (cdc_acm_verify_ready) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Already init, please deinit first\n");
		} else {
			xfer_size = (u32)_strtoul((const char *)(argv[2]), (char **)NULL, 10);
			if ((xfer_size > 0) && (xfer_size < 0xFFFF)) {
				cdc_acm_verify_xfer_size = (u16)xfer_size & 0xFFFF;
				RTK_LOGS(TAG, RTK_LOG_INFO, "Xfer size %d\n", cdc_acm_verify_xfer_size);
			} else {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid xfer size\n");
				ret = HAL_ERR_PARA;
			}
		}
	} else if (_stricmp(cmd, "init") == 0) {
		if (cdc_acm_verify_ready) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Already init, please deinit first\n");
		} else {
			ret = cdc_acm_verify_init();
		}
	} else if (_stricmp(cmd, "rx") == 0) {
		if (cdc_acm_verify_ready) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "RX mode\n");
			cdc_acm_verify_rx_en = 1U;
			cdc_acm_verify_tx_en = 0U;
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Not init\n");
			ret = HAL_ERR_PARA;
		}
	} else if (_stricmp(cmd, "tx") == 0) {
		if (cdc_acm_verify_ready) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "TX mode\n");
			cdc_acm_verify_rx_en = 0U;
			cdc_acm_verify_tx_en = 1U;
			usb_os_sema_give(cdc_acm_verify_xfer_sema);
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Not init\n");
			ret = HAL_ERR_PARA;
		}
	} else if (_stricmp(cmd, "trx") == 0) {
		if (cdc_acm_verify_ready) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "TRX mode\n");
			cdc_acm_verify_rx_en = 1U;
			cdc_acm_verify_tx_en = 1U;
			usb_os_sema_give(cdc_acm_verify_xfer_sema);
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Not init\n");
			ret = HAL_ERR_PARA;
		}
	} else if (_stricmp(cmd, "deinit") == 0) {
		if (cdc_acm_verify_ready) {
			cdc_acm_verify_deinit();
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Not init, ignore\n");
		}
	} else {
		cdc_acm_verify_usage();
		ret = HAL_ERR_PARA;
	}

	return ret;
}

