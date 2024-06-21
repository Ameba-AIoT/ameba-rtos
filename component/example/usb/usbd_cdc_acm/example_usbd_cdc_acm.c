/**
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "basic_types.h"
#include "usbd.h"
#include "usbd_cdc_acm.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/

// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
// while test suspend/resume, hotplug should be disabled
#define CONFIG_USBD_CDC_ACM_HOTPLUG					0

// USB speed
#ifdef CONFIG_USB_FS
#define CONFIG_USBD_CDC_ACM_SPEED					USB_SPEED_FULL
#else
#define CONFIG_USBD_CDC_ACM_SPEED					USB_SPEED_HIGH
#endif

// Echo asynchronously, for transfer size larger than packet size. While fpr
// transfer size less than packet size, the synchronous way is preferred.
#define CONFIG_USBD_CDC_ACM_ASYNC_XFER				0

// Asynchronous transfer size
#define CONFIG_CDC_ACM_ASYNC_BUF_SIZE				2048U

// Do not change the settings unless indeed necessary
#define CONFIG_CDC_ACM_BULK_IN_XFER_SIZE			2048U
#define CONFIG_CDC_ACM_BULK_OUT_XFER_SIZE			2048U

// Thread priorities
#define CONFIG_CDC_ACM_INIT_THREAD_PRIORITY			5
#define CONFIG_CDC_ACM_ISR_THREAD_PRIORITY			7
#define CONFIG_CDC_ACM_HOTPLUG_THREAD_PRIORITY		8 // Should be higher than CONFIG_CDC_ACM_ISR_THREAD_PRIORITY
#define CONFIG_CDC_ACM_XFER_THREAD_PRIORITY			5

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int cdc_acm_cb_init(void);
static int cdc_acm_cb_deinit(void);
static int cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf);
static int cdc_acm_cb_received(u8 *buf, u32 Len);
static void cdc_acm_cb_status_changed(u8 status);

/* Private variables ---------------------------------------------------------*/
static const char *TAG = "ACM";

static usbd_cdc_acm_cb_t cdc_acm_cb = {
	.init = cdc_acm_cb_init,
	.deinit = cdc_acm_cb_deinit,
	.setup = cdc_acm_cb_setup,
	.received = cdc_acm_cb_received,
	.status_changed = cdc_acm_cb_status_changed
};

static usbd_cdc_acm_line_coding_t cdc_acm_line_coding;

static u16 cdc_acm_ctrl_line_state;

static usbd_config_t cdc_acm_cfg = {
	.speed = CONFIG_USBD_CDC_ACM_SPEED,
	.dma_enable   = 1U,
	.isr_priority = CONFIG_CDC_ACM_ISR_THREAD_PRIORITY,
	.intr_use_ptx_fifo  = 0U,
	.nptx_max_epmis_cnt = 1U,
	.ext_intr_en        = USBD_EPMIS_INTR,
	.nptx_max_err_cnt   = {0U, 0U, 0U, 2000U, }
};

#if CONFIG_USBD_CDC_ACM_ASYNC_XFER
static u32 cdc_acm_xfer_idx = 0;
static u8 cdc_acm_async_xfer_buf[CONFIG_CDC_ACM_ASYNC_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
static u16 cdc_acm_async_xfer_buf_pos = 0;
static volatile int cdc_acm_async_xfer_busy = 0;
static rtos_sema_t cdc_acm_async_xfer_sema;
#endif

#if CONFIG_USBD_CDC_ACM_HOTPLUG
static u8 cdc_acm_attach_status;
static rtos_sema_t cdc_acm_attach_status_changed_sema;
#endif

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

#if CONFIG_USBD_CDC_ACM_ASYNC_XFER
	cdc_acm_async_xfer_buf_pos = 0;
	cdc_acm_async_xfer_busy = 0;
#endif

	return HAL_OK;
}

/**
  * @brief  DeInitializes the CDC media layer
  * @param  None
  * @retval Status
  */
static int cdc_acm_cb_deinit(void)
{
#if CONFIG_USBD_CDC_ACM_ASYNC_XFER
	cdc_acm_async_xfer_buf_pos = 0;
	cdc_acm_async_xfer_busy = 0;
#endif
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
#if CONFIG_USBD_CDC_ACM_ASYNC_XFER
	int ret = HAL_OK;
	if (0 == cdc_acm_async_xfer_busy) {
		if ((cdc_acm_async_xfer_buf_pos + len) > CONFIG_CDC_ACM_ASYNC_BUF_SIZE) {
			len = CONFIG_CDC_ACM_ASYNC_BUF_SIZE - cdc_acm_async_xfer_buf_pos;  // extra data discarded
		}

		memcpy((void *)((u32)cdc_acm_async_xfer_buf + cdc_acm_async_xfer_buf_pos), buf, len);
		cdc_acm_async_xfer_buf_pos += len;
		if (cdc_acm_async_xfer_buf_pos >= CONFIG_CDC_ACM_ASYNC_BUF_SIZE) {
			cdc_acm_async_xfer_buf_pos = 0;
			rtos_sema_give(cdc_acm_async_xfer_sema);
		}
	} else {
		RTK_LOGS(TAG, "[ACM] Busy, discard %dB\n", len);
		ret = HAL_BUSY;
	}

	return ret;
#else
	return usbd_cdc_acm_transmit(buf, len);
#endif
}

/**
  * @brief  Handle the CDC class control requests
  * @param  cmd: Command code
  * @param  buf: Buffer containing command data (request parameters)
  * @param  len: Number of data to be sent (in bytes)
  * @retval Status
  */
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
			RTK_LOGS(TAG, "[ACM] VCOM port activate\n");
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

static void cdc_acm_cb_status_changed(u8 status)
{
	RTK_LOGS(TAG, "[ACM] Status change: %d\n", status);
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
		if (rtos_sema_take(cdc_acm_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == SUCCESS) {
			if (cdc_acm_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				RTK_LOGS(TAG, "[ACM] DETACHED\n");
				usbd_cdc_acm_deinit();
				ret = usbd_deinit();
				if (ret != 0) {
					break;
				}
				RTK_LOGS(TAG, "[ACM] Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
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
				RTK_LOGS(TAG, "[ACM] ATTACHED\n");
			} else {
				RTK_LOGS(TAG, "[ACM] INIT\n");
			}
		}
	}
	RTK_LOGS(TAG, "[ACM] Hotplug thread exit\n");
	rtos_task_delete(NULL);
}
#endif // CONFIG_USBD_MSC_CHECK_USB_STATUS

#if CONFIG_USBD_CDC_ACM_ASYNC_XFER
static void cdc_acm_xfer_thread(void *param)
{
	int ret;
	u8 *xfer_buf;
	u32 xfer_len;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(cdc_acm_async_xfer_sema, RTOS_SEMA_MAX_COUNT) == SUCCESS) {
			xfer_len = CONFIG_CDC_ACM_ASYNC_BUF_SIZE;
			xfer_buf = cdc_acm_async_xfer_buf;
			cdc_acm_async_xfer_busy = 1;
			RTK_LOGS(TAG, "[ACM] Start xfer(%dB) idx(%d)\n", CONFIG_CDC_ACM_ASYNC_BUF_SIZE, cdc_acm_xfer_idx);
			while (xfer_len > 0) {
				if (xfer_len > CONFIG_CDC_ACM_BULK_IN_XFER_SIZE) {
					ret = usbd_cdc_acm_transmit(xfer_buf, CONFIG_CDC_ACM_BULK_IN_XFER_SIZE);
					if (ret == HAL_OK) {
						xfer_len -= CONFIG_CDC_ACM_BULK_IN_XFER_SIZE;
						xfer_buf += CONFIG_CDC_ACM_BULK_IN_XFER_SIZE;
					} else { // HAL_BUSY
						RTK_LOGS(TAG, "[ACM] Xfer busy, retry[1]\n");
						rtos_time_delay_us(200);
					}
				} else {
					ret = usbd_cdc_acm_transmit(xfer_buf, xfer_len);
					if (ret == HAL_OK) {
						xfer_len = 0;
						cdc_acm_async_xfer_busy = 0;
						cdc_acm_xfer_idx++;
						RTK_LOGS(TAG, "[ACM] Xfer done\n");
						break;
					} else { // HAL_BUSY
						RTK_LOGS(TAG, "[ACM] Xfer busy, retry[2]\n");
						rtos_time_delay_us(200);
					}
				}
			}
		}
	}

	rtos_task_delete(NULL);
}
#endif

static void example_usbd_cdc_acm_thread(void *param)
{
	int ret = 0;
#if CONFIG_USBD_CDC_ACM_HOTPLUG
	rtos_task_t check_task;
#endif
#if CONFIG_USBD_CDC_ACM_ASYNC_XFER
	rtos_task_t xfer_task;
#endif

	UNUSED(param);

#if CONFIG_USBD_CDC_ACM_ASYNC_XFER
	rtos_sema_create(&cdc_acm_async_xfer_sema, 0, 1);
#endif

#if CONFIG_USBD_CDC_ACM_HOTPLUG
	rtos_sema_create(&cdc_acm_attach_status_changed_sema, 0, 1);
#endif

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
	if (ret != SUCCESS) {
		goto exit_create_check_task_fail;
	}
#endif

#if CONFIG_USBD_CDC_ACM_ASYNC_XFER
	// The priority of transfer thread shall be lower than USB isr priority
	ret = rtos_task_create(&xfer_task, "cdc_acm_xfer_thread", cdc_acm_xfer_thread, NULL, 1024, CONFIG_CDC_ACM_XFER_THREAD_PRIORITY);
	if (ret != SUCCESS) {
		goto exit_create_xfer_task_fail;
	}
#endif

	rtos_time_delay_ms(100);

	RTK_LOGS(TAG, "[ACM] USBD CDC ACM demo start\n");

	rtos_task_delete(NULL);

	return;

#if CONFIG_USBD_CDC_ACM_ASYNC_XFER
exit_create_xfer_task_fail:
#if CONFIG_USBD_CDC_ACM_HOTPLUG
	rtos_task_delete(check_task);
#endif
#endif

#if CONFIG_USBD_CDC_ACM_HOTPLUG
exit_create_check_task_fail:
	usbd_cdc_acm_deinit();
#endif

exit_usbd_cdc_acm_init_fail:
	usbd_deinit();

exit_usbd_init_fail:
	RTK_LOGS(TAG, "[ACM] USBD CDC ACM demo stop\n");
#if CONFIG_USBD_CDC_ACM_HOTPLUG
	rtos_sema_delete(cdc_acm_attach_status_changed_sema);
#endif
#if CONFIG_USBD_CDC_ACM_ASYNC_XFER
	rtos_sema_delete(cdc_acm_async_xfer_sema);
#endif

	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  USB download de-initialize
  * @param  None
  * @retval Result of the operation: 0 if success else fail
  */
void example_usbd_cdc_acm(void)
{
	int ret;
	rtos_task_t task;

	ret = rtos_task_create(&task, "example_usbd_cdc_acm_thread", example_usbd_cdc_acm_thread, NULL, 1024U, CONFIG_CDC_ACM_INIT_THREAD_PRIORITY);
	if (ret != SUCCESS) {
		RTK_LOGS(TAG, "[ACM] Create USBD CDC ACM thread fail\n");
	}
}

