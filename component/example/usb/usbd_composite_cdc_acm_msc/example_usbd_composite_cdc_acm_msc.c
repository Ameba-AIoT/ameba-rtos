/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "usbd.h"
#include "usbd_composite_cdc_acm_msc.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "COMP";
// This configuration is used to enable a thread to check hotplug event
// and reset USB stack to avoid memory leak, only for example.
#define CONFIG_USBD_COMPOSITE_HOTPLUG							1

#if COMP_MSC_RAM_DISK && (CONFIG_USBD_COMPOSITE_HOTPLUG == 1)
#error "Hotplug is not supported when RAM is used for USB MSC media"
#endif

// USB speed
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define CONFIG_USBD_COMPOSITE_SPEED								USB_SPEED_FULL
#else
#define CONFIG_USBD_COMPOSITE_SPEED								USB_SPEED_HIGH
#endif

#define CONFIG_USBD_COMPOSITE_CDC_ACM_MSC_BULK_IN_XFER_SIZE		2048U
#define CONFIG_USBD_COMPOSITE_CDC_ACM_MSC_BULK_OUT_XFER_SIZE	2048U

// Thread priorities
#define CONFIG_USBD_COMPOSITE_INIT_THREAD_PRIORITY				5U
#define CONFIG_USBD_COMPOSITE_ISR_THREAD_PRIORITY				7U
#define CONFIG_USBD_COMPOSITE_HOTPLUG_THREAD_PRIORITY			8U // Should be higher than CONFIG_USBD_COMPOSITE_ISR_THREAD_PRIORITY

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int composite_cdc_acm_cb_init(void);
static int composite_cdc_acm_cb_deinit(void);
static int composite_cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf);
static int composite_cdc_acm_cb_received(u8 *buf, u32 Len);

static void composite_cb_status_changed(u8 old_status, u8 status);

/* Private variables ---------------------------------------------------------*/

static usbd_config_t composite_cfg = {
	.speed = CONFIG_USBD_COMPOSITE_SPEED,
	.dma_enable = 1U,
	.isr_priority = CONFIG_USBD_COMPOSITE_ISR_THREAD_PRIORITY,
	.intr_use_ptx_fifo = 0U,
#if defined (CONFIG_AMEBASMART)
	.ext_intr_enable = USBD_EPMIS_INTR,
	.nptx_max_epmis_cnt = 100U,
#elif defined (CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 420U,
	.ptx_fifo_depth = {16U, 256U, 32U, 256U, },
#endif
};

static usbd_composite_cdc_acm_usr_cb_t composite_cdc_acm_usr_cb = {
	.init = composite_cdc_acm_cb_init,
	.deinit = composite_cdc_acm_cb_deinit,
	.setup = composite_cdc_acm_cb_setup,
	.received = composite_cdc_acm_cb_received
};

static usbd_composite_cdc_acm_line_coding_t composite_cdc_acm_line_coding;

static u16 composite_cdc_acm_ctrl_line_state;

static usbd_composite_cb_t composite_cb = {
	.status_changed = composite_cb_status_changed,
};

#if CONFIG_USBD_COMPOSITE_HOTPLUG
static u8 composite_attach_status;
static rtos_sema_t composite_attach_status_changed_sema;
#endif

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CDC media layer
  * @param  None
  * @retval Status
  */
static int composite_cdc_acm_cb_init(void)
{
	usbd_composite_cdc_acm_line_coding_t *lc = &composite_cdc_acm_line_coding;

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
static int composite_cdc_acm_cb_deinit(void)
{
	return HAL_OK;
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface through this function.
  * @param  Buf: RX buffer
  * @param  Len: RX data length (in bytes)
  * @retval Status
  */
static int composite_cdc_acm_cb_received(u8 *buf, u32 len)
{
	return usbd_composite_cdc_acm_transmit(buf, len);
}

/**
  * @brief  Handle the CDC class control requests
  * @param  cmd: Command code
  * @param  buf: Buffer containing command data (request parameters)
  * @param  len: Number of data to be sent (in bytes)
  * @retval Status
  */
static int composite_cdc_acm_cb_setup(usb_setup_req_t *req, u8 *buf)
{
	int ret = HAL_OK;
	usbd_composite_cdc_acm_line_coding_t *lc = &composite_cdc_acm_line_coding;

	switch (req->bRequest) {
	case COMP_CDC_SEND_ENCAPSULATED_COMMAND:
		/* Do nothing */
		break;

	case COMP_CDC_GET_ENCAPSULATED_RESPONSE:
		/* Do nothing */
		break;

	case COMP_CDC_SET_COMM_FEATURE:
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "COMP_CDC_SET_COMM_FEATURE\n");
		/* Do nothing */
		break;

	case COMP_CDC_GET_COMM_FEATURE:
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "COMP_CDC_GET_COMM_FEATURE\n");
		/* Do nothing */
		break;

	case COMP_CDC_CLEAR_COMM_FEATURE:
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "COMP_CDC_CLEAR_COMM_FEATURE\n");
		/* Do nothing */
		break;

	case COMP_CDC_SET_LINE_CODING:
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "COMP_CDC_SET_LINE_CODING\n");
		if (req->wLength == COMP_CDC_ACM_LINE_CODING_SIZE) {
			lc->bitrate = (u32)(buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24));
			lc->format = buf[4];
			lc->parity_type = buf[5];
			lc->data_type = buf[6];
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "COMP_CDC_SET_LINE_CODING XXX\n");
		}
		break;

	case COMP_CDC_GET_LINE_CODING:
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "COMP_CDC_GET_LINE_CODING\n");
		buf[0] = (u8)(lc->bitrate & 0xFF);
		buf[1] = (u8)((lc->bitrate >> 8) & 0xFF);
		buf[2] = (u8)((lc->bitrate >> 16) & 0xFF);
		buf[3] = (u8)((lc->bitrate >> 24) & 0xFF);
		buf[4] = lc->format;
		buf[5] = lc->parity_type;
		buf[6] = lc->data_type;
		break;

	case COMP_CDC_SET_CONTROL_LINE_STATE:
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "COMP_CDC_SET_CONTROL_LINE_STATE\n");
		/*
		wValue:	wValue, Control Signal Bitmap
				D2-15:	Reserved, 0
				D1:	RTS, 0 - Deactivate, 1 - Activate
				D0:	DTR, 0 - Not Present, 1 - Present
		*/
		composite_cdc_acm_ctrl_line_state = req->wValue;
		if (composite_cdc_acm_ctrl_line_state & 0x01) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "VCOM port activate\n");
		}
		break;

	case COMP_CDC_SEND_BREAK:
		RTK_LOGS(TAG, RTK_LOG_INFO, "COMP_CDC_SEND_BREAK\n");
		/* Do nothing */
		break;

	default:
		RTK_LOGS(TAG, RTK_LOG_WARN, "Invalid CDC bRequest 0x%02x\n", req->bRequest);
		ret = HAL_ERR_PARA;
		break;
	}

	return ret;
}

static void composite_cb_status_changed(u8 old_status, u8 status)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Status change: %d -> %d\n", old_status, status);
#if CONFIG_USBD_COMPOSITE_HOTPLUG
	composite_attach_status = status;
	rtos_sema_give(composite_attach_status_changed_sema);
#endif
}

#if CONFIG_USBD_COMPOSITE_HOTPLUG
static void composite_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(composite_attach_status_changed_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (composite_attach_status == USBD_ATTACH_STATUS_DETACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "DETACHED\n");
				usbd_composite_deinit();
				ret = usbd_deinit();
				if (ret != 0) {
					break;
				}
				usbd_composite_msc_disk_deinit();
				RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());
				usbd_composite_msc_disk_init();
				ret = usbd_init(&composite_cfg);
				if (ret != 0) {
					break;
				}
				ret = usbd_composite_init(CONFIG_USBD_COMPOSITE_CDC_ACM_MSC_BULK_OUT_XFER_SIZE,
										  CONFIG_USBD_COMPOSITE_CDC_ACM_MSC_BULK_IN_XFER_SIZE,
										  &composite_cdc_acm_usr_cb,
										  &composite_cb);
				if (ret != 0) {
					usbd_deinit();
					break;
				}
			} else if (composite_attach_status == USBD_ATTACH_STATUS_ATTACHED) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACHED\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
			}
		}
	}
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread fail\n");
	rtos_task_delete(NULL);
}
#endif // CONFIG_USBD_COMPOSITE_HOTPLUG

static void example_usbd_composite_thread(void *param)
{
	int ret = 0;
#if CONFIG_USBD_COMPOSITE_HOTPLUG
	rtos_task_t task;
#endif

	UNUSED(param);

#if CONFIG_USBD_COMPOSITE_HOTPLUG
	rtos_sema_create(&composite_attach_status_changed_sema, 0U, 1U);
#endif

	ret = usbd_composite_msc_disk_init();
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init disk fail\n");
		goto exit_usbd_composite_msc_disc_init_fail;
	}

	ret = usbd_init(&composite_cfg);
	if (ret != HAL_OK) {
		goto exit_usbd_init_fail;
	}

	ret = usbd_composite_init(CONFIG_USBD_COMPOSITE_CDC_ACM_MSC_BULK_OUT_XFER_SIZE,
							  CONFIG_USBD_COMPOSITE_CDC_ACM_MSC_BULK_IN_XFER_SIZE,
							  &composite_cdc_acm_usr_cb,
							  &composite_cb);
	if (ret != HAL_OK) {
		goto exit_usbd_composite_init_fail;
	}

#if CONFIG_USBD_COMPOSITE_HOTPLUG
	ret = rtos_task_create(&task, "composite_hotplug_thread", composite_hotplug_thread, NULL,
						   1024, CONFIG_USBD_COMPOSITE_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto exit_create_check_task_fail;
	}
#endif

	rtos_time_delay_ms(100);

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD COMP demo started\n");

	rtos_task_delete(NULL);

	return;

#if CONFIG_USBD_COMPOSITE_HOTPLUG
exit_create_check_task_fail:
	usbd_composite_deinit();
#endif

exit_usbd_composite_init_fail:
	usbd_deinit();

exit_usbd_init_fail:
	usbd_composite_msc_disk_deinit();

exit_usbd_composite_msc_disc_init_fail:
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD COMP demo stop\n");
#if CONFIG_USBD_COMPOSITE_HOTPLUG
	rtos_sema_delete(composite_attach_status_changed_sema);
#endif
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  USB download de-initialize
  * @param  None
  * @retval Result of the operation: 0 if success else fail
  */
void example_usbd_composite(void)
{
	int ret;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD COMP demo start...\n");

	ret = rtos_task_create(&task, "example_usbd_composite_thread", example_usbd_composite_thread, NULL, 1024, CONFIG_USBD_COMPOSITE_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD COMP thread fail\n");
	}
}

