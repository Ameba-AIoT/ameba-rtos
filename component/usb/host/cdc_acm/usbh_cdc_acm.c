/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_cdc_acm.h"

/* Private defines -----------------------------------------------------------*/

#define USB_BULK_OUT_MAX_TIMEOUT_TICK          8000 //sof
#define USB_BULK_IN_MAX_TIMEOUT_TICK           100  //sof
#define USB_INTR_MAX_TIMEOUT_TICK              1000
#define USB_INTR_RETRY_MAX_CNT                 10
/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbh_cdc_acm_attach(usb_host_t *host);
static int usbh_cdc_acm_detach(usb_host_t *host);
static int usbh_cdc_acm_process(usb_host_t *host, usbh_event_t *event);
#if CONFIG_USBH_CDC_ACM_NOTIFY
static int usbh_cdc_acm_sof(usb_host_t *host);
static int usbh_cdc_acm_completed(usb_host_t *host, u8 pipe_num);
static void usbh_cdc_acm_process_intr_rx(usb_host_t *host);
#endif
static int usbh_cdc_acm_setup(usb_host_t *host);
static int usbh_cdc_acm_process_get_line_coding(usb_host_t *host, usb_cdc_line_coding_t *linecoding);
static int usbh_cdc_acm_process_set_line_coding(usb_host_t *host, usb_cdc_line_coding_t *linecoding);
static int usbh_cdc_acm_process_set_control_line_state(usb_host_t *host);
static int usbh_cdc_acm_process_send_break(usb_host_t *host);
static void usbh_cdc_acm_process_tx(usb_host_t *host);
static void usbh_cdc_acm_process_rx(usb_host_t *host);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "ACM";

/* USB CDC ACM device identification */
static const usbh_dev_id_t cdc_acm_devs[] = {
	{
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS,
		.bInterfaceClass = USB_CDC_CLASS_CODE,
	},
	{
	},
};

/* USB Host CDC ACM class driver */
static usbh_class_driver_t usbh_cdc_acm_driver = {
	.id_table = cdc_acm_devs,
	.attach = usbh_cdc_acm_attach,
	.detach = usbh_cdc_acm_detach,
	.setup = usbh_cdc_acm_setup,
	.process = usbh_cdc_acm_process,
#if CONFIG_USBH_CDC_ACM_NOTIFY
	.sof = usbh_cdc_acm_sof,
	.completed = usbh_cdc_acm_completed,
#endif
};

static usbh_cdc_acm_host_t usbh_cdc_acm_host;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Attach callback: enumerate interfaces and open pipes
  * @param  host: Host handle
  * @retval HAL_OK on success, others on failure.
  */
static int usbh_cdc_acm_attach(usb_host_t *host)
{
	int status = HAL_ERR_UNKNOWN;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_itf_data_t *itf_data;
	usbh_itf_desc_t *comm_itf_desc;
	usbh_itf_desc_t *data_itf_desc;
	usbh_ep_desc_t *ep_desc;
	usbh_pipe_t *bulk_out = &cdc->bulk_out;
	usbh_pipe_t *bulk_in = &cdc->bulk_in;
	usbh_dev_id_t dev_id = {0,};
#if CONFIG_USBH_CDC_ACM_NOTIFY
	usbh_pipe_t *intr_in = &cdc->intr_in;
#endif

	/* Get Communication Interface — needed unconditionally for class request wIndex (PSTN §6.3).
	 * Match by class+subclass only: the ACM control protocol (bInterfaceProtocol) varies across
	 * spec-compliant devices (0x00 none / 0x01 AT-V.250 / 0x02-0x06 / 0xFF vendor) and does not
	 * identify the interface — matching a fixed protocol would reject valid ACM devices. */
	dev_id.bInterfaceClass = USB_CDC_COMM_INTERFACE_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_SUBCLASS_ACM;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS | USBH_DEV_ID_MATCH_ITF_SUBCLASS;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);
	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get comm itf fail\n");
		return HAL_ERR_PARA;
	}
	comm_itf_desc = itf_data->itf_desc_array;
	cdc->comm_itf_num = comm_itf_desc->bInterfaceNumber;

#if CONFIG_USBH_CDC_ACM_NOTIFY
	/* Set notification endpoint */
	if (comm_itf_desc->bNumEndpoints >= 1) {
		ep_desc = &comm_itf_desc->ep_desc_array[0];
		if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
			if (usbh_open_pipe(host, intr_in, ep_desc) == HAL_OK) {
				intr_in->max_timeout_tick = USB_INTR_MAX_TIMEOUT_TICK;
			} else {
				RTK_LOGS(TAG, RTK_LOG_WARN, "Open intr pipe fail\n");
			}
		}
	}
#endif
	/* Get Data Interface */
	dev_id.bInterfaceClass = USB_CDC_DATA_INTERFACE_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_CDC_SUBCLASS_RESERVED;
	dev_id.bInterfaceProtocol = USB_CDC_CTRL_PROTOCOL_NO_CLASS_SPECIFIC;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);
	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get data itf fail\n");
		goto open_fail;
	} else {
		data_itf_desc = itf_data->itf_desc_array;
		/* Set data in/out endpoints */
		for (int i = 0; i < data_itf_desc->bNumEndpoints && i < 2; i++) {
			ep_desc = &data_itf_desc->ep_desc_array[i];
			if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
				if (usbh_open_pipe(host, bulk_in, ep_desc) != HAL_OK) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Open bulk in pipe fail\n");
					goto open_fail;
				}
				bulk_in->max_timeout_tick = USB_BULK_IN_MAX_TIMEOUT_TICK;
			} else {
				if (usbh_open_pipe(host, bulk_out, ep_desc) != HAL_OK) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Open bulk out pipe fail\n");
					goto open_fail;
				}
				bulk_out->max_timeout_tick = USB_BULK_OUT_MAX_TIMEOUT_TICK;
			}
		}

		/* Both bulk pipes are mandatory for CDC ACM data transfer */
		if ((bulk_in->pipe_num == 0) || (bulk_out->pipe_num == 0)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Missing bulk pipe\n");
			goto open_fail;
		}
	}
	cdc->host = host;
	cdc->state = USBH_CDC_ACM_STATE_IDLE;

	if ((cdc->cb != NULL) && (cdc->cb->attach != NULL)) {
		cdc->cb->attach();
	}

	return HAL_OK;

open_fail:
	/* Roll back any pipe already opened (guard with pipe_num: only close open pipes) */
#if CONFIG_USBH_CDC_ACM_NOTIFY
	if (intr_in->pipe_num) {
		usbh_close_pipe(host, intr_in);
	}
#endif
	if (bulk_in->pipe_num) {
		usbh_close_pipe(host, bulk_in);
	}
	if (bulk_out->pipe_num) {
		usbh_close_pipe(host, bulk_out);
	}
	return status;
}

/**
  * @brief  Detach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_acm_detach(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_pipe_t *bulk_out = &cdc->bulk_out;
	usbh_pipe_t *bulk_in = &cdc->bulk_in;
#if CONFIG_USBH_CDC_ACM_NOTIFY
	usbh_pipe_t *intr_in = &cdc->intr_in;
#endif

	if ((cdc->cb != NULL) && (cdc->cb->detach != NULL)) {
		cdc->cb->detach();
	}
#if CONFIG_USBH_CDC_ACM_NOTIFY
	if (intr_in->pipe_num) {
		usbh_close_pipe(host, intr_in);
	}
#endif

	if (bulk_in->pipe_num) {
		usbh_close_pipe(host, bulk_in);
	}

	if (bulk_out->pipe_num) {
		usbh_close_pipe(host, bulk_out);
	}

	/* Device gone: drop the stale host handle and reset state so post-detach
	   transmit/receive calls are rejected by their host/connect_state checks. */
	cdc->host = NULL;
	cdc->state = USBH_CDC_ACM_STATE_IDLE;

	return HAL_OK;
}

/**
  * @brief  Standard control requests handling callback
  * @param  host: Host handle
  * @retval HAL_OK on success, others on failure.
  */
static int usbh_cdc_acm_setup(usb_host_t *host)
{
	int status = HAL_ERR_UNKNOWN;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	/*Issue the get line coding request*/
	status = usbh_cdc_acm_process_get_line_coding(host, cdc->line_coding);
	if (status == HAL_OK) {
		if ((cdc->cb != NULL) && (cdc->cb->setup != NULL)) {
			cdc->cb->setup();
		}
	}
	return status;
}

#if CONFIG_USBH_CDC_ACM_NOTIFY
/**
  * @brief  SOF callback for CDC ACM - handles interval check and retry
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  host: Host handle
  * @retval Status
  * @note   For INTR transfer:
  *         - BUSY: check interval and retry if elapsed, retry count exceeded then error
  */
static int usbh_cdc_acm_sof(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_pipe_t *intr_in = &cdc->intr_in;
	usbh_urb_state_t urb_state;
	u32 elapsed;

	if (intr_in->pipe_num == 0) {
		return HAL_OK;
	}

	elapsed = usbh_get_elapsed_frame_cnt(host, intr_in->frame_num);

	if (intr_in->xfer_state == USBH_EP_XFER_BUSY) {
		/* Check transfer status */
		urb_state = usbh_get_urb_state(host, intr_in);
		if ((urb_state == USBH_URB_BUSY) || (urb_state == USBH_URB_IDLE)) {
			/* Check whether the next frame will be the xfer frame for retry */
			if (elapsed + 1 >= intr_in->ep_interval) {
				intr_in->retry_cnt++;
				if (intr_in->retry_cnt <= USB_INTR_RETRY_MAX_CNT) {
					intr_in->frame_num = usbh_get_current_frame_number(host);
					intr_in->xfer_state = USBH_EP_XFER_START;
					usbh_notify_class_state_change(host, intr_in->pipe_num);
				} else {
					/* Retry count exceeded, set error */
					intr_in->retry_cnt = 0;
					intr_in->xfer_state = USBH_EP_XFER_ERROR;
					usbh_notify_class_state_change(host, intr_in->pipe_num);
				}
			}
			/* If interval not elapsed, keep BUSY state */
		}
		/* If URB is DONE, wait for process_intr_rx to handle complete */
	}
	return HAL_OK;
}

/**
  * @brief  Transfer completion callback: handle the end of an interrupt IN transfer,
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param[in] host: Pointer to the USB host handle.
  * @param[in] pipe_num: Pipe number of the completed transfer.
  * @return 0 on success, non-zero on failure.
  */
static int usbh_cdc_acm_completed(usb_host_t *host, u8 pipe_num)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_pipe_t *intr_in = &cdc->intr_in;

	if (intr_in->pipe_num && (pipe_num == intr_in->pipe_num) && (intr_in->xfer_state == USBH_EP_XFER_BUSY)) {
		usbh_notify_class_state_change(host, intr_in->pipe_num);
	}

	return HAL_OK;
}
#endif

/**
* @brief  State machine handling callback
* @param  host:Host handle
* @param  event: USB host event
* @retval Status
*/
static int usbh_cdc_acm_process(usb_host_t *host, usbh_event_t *event)
{
	int status = HAL_BUSY;
	u8 req_status = HAL_OK;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	switch (cdc->state) {

	case USBH_CDC_ACM_STATE_IDLE:
		status = HAL_OK;
		break;

	case USBH_CDC_ACM_STATE_SET_CONTROL_LINE_STATE:
		req_status = usbh_cdc_acm_process_set_control_line_state(host);
		if (req_status == HAL_OK) {
			cdc->state = USBH_CDC_ACM_STATE_IDLE;
		} else if (req_status != HAL_BUSY) {
			cdc->state = USBH_CDC_ACM_STATE_ERROR;
		}
		usbh_notify_class_state_change(host, 0);
		break;

	case USBH_CDC_ACM_STATE_SEND_BREAK:
		req_status = usbh_cdc_acm_process_send_break(host);
		if (req_status == HAL_OK) {
			cdc->state = USBH_CDC_ACM_STATE_IDLE;
		} else if (req_status != HAL_BUSY) {
			cdc->state = USBH_CDC_ACM_STATE_ERROR;
		}
		usbh_notify_class_state_change(host, 0);
		break;

	case USBH_CDC_ACM_STATE_SET_LINE_CODING:
		req_status = usbh_cdc_acm_process_set_line_coding(host, cdc->user_line_coding);
		if (req_status == HAL_OK) {
			cdc->state = USBH_CDC_ACM_STATE_GET_LINE_CODING;
		} else if (req_status != HAL_BUSY) {
			cdc->state = USBH_CDC_ACM_STATE_ERROR;
		}
		usbh_notify_class_state_change(host, 0);
		break;

	case USBH_CDC_ACM_STATE_GET_LINE_CODING:
		req_status = usbh_cdc_acm_process_get_line_coding(host, cdc->line_coding);
		if (req_status == HAL_OK) {
			usb_cdc_line_coding_t *lc = cdc->line_coding;
			usb_cdc_line_coding_t *ulc = cdc->user_line_coding;
			cdc->state = USBH_CDC_ACM_STATE_IDLE;
			if ((lc->b.bCharFormat == ulc->b.bCharFormat) &&
				(lc->b.bDataBits == ulc->b.bDataBits) &&
				(lc->b.bParityType == ulc->b.bParityType) &&
				(lc->b.dwDteRate == ulc->b.dwDteRate)) {
				if ((cdc->cb != NULL) && (cdc->cb->line_coding_changed != NULL)) {
					cdc->cb->line_coding_changed(cdc->line_coding);
				}
			}
		} else if (req_status != HAL_BUSY) {
			cdc->state = USBH_CDC_ACM_STATE_ERROR;
		}
		usbh_notify_class_state_change(host, 0);
		break;

	case USBH_CDC_ACM_STATE_TRANSFER:
		if (event) {
			if (event->pipe_num == cdc->bulk_out.pipe_num) {
				usbh_cdc_acm_process_tx(host);
			} else if (event->pipe_num == cdc->bulk_in.pipe_num) {
				usbh_cdc_acm_process_rx(host);
#if CONFIG_USBH_CDC_ACM_NOTIFY
			} else if (event->pipe_num == cdc->intr_in.pipe_num) {
				usbh_cdc_acm_process_intr_rx(host);
#endif
			}
		}
		break;

	case USBH_CDC_ACM_STATE_ERROR:
		req_status = usbh_ctrl_clear_feature(host, 0x00U);
		if (req_status == HAL_OK) {
			cdc->state = USBH_CDC_ACM_STATE_IDLE;
		}
		break;

	default:
		break;

	}

	return status;
}

/**
  * @brief  Get line coding
  * @param  host: Host handle
  * @param  linecoding: Line coding data pointer
  * @retval Status
  */
static int usbh_cdc_acm_process_get_line_coding(usb_host_t *host, usb_cdc_line_coding_t *linecoding)
{
	usbh_setup_req_t setup;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_CDC_ACM_GET_LINE_CODING;
	setup.req.wValue = 0U;
	setup.req.wIndex = cdc->comm_itf_num;
	setup.req.wLength = USB_CDC_ACM_LINE_CODING_SIZE;

	return usbh_ctrl_request(host, &setup, linecoding->d8);
}

/**
  * @brief  Set line coding
  * @param  host: Host handle
  * @param  linecoding: Line coding data pointer
  * @retval Status
  */
static int usbh_cdc_acm_process_set_line_coding(usb_host_t *host, usb_cdc_line_coding_t *linecoding)
{
	usbh_setup_req_t setup;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_CDC_ACM_SET_LINE_CODING;
	setup.req.wValue = 0U;
	setup.req.wIndex = cdc->comm_itf_num;
	setup.req.wLength = USB_CDC_ACM_LINE_CODING_SIZE;

	return usbh_ctrl_request(host, &setup, linecoding->d8);
}

/**
  * @brief  Set control line
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_acm_process_set_control_line_state(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_CDC_ACM_SET_CONTROL_LINE_STATE;
	setup.req.wValue = cdc->ctrl_line_state;
	setup.req.wIndex = cdc->comm_itf_num;
	setup.req.wLength = 0U;
	return usbh_ctrl_request(host, &setup, NULL);
}

/**
  * @brief  Send break
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_acm_process_send_break(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_CDC_ACM_SEND_BREAK;
	setup.req.wValue = cdc->break_duration;
	setup.req.wIndex = cdc->comm_itf_num;
	setup.req.wLength = 0U;
	return usbh_ctrl_request(host, &setup, NULL);
}

/**
  * @brief  TX handling
  * @param  host: Host handle
  * @retval None
*/
static void usbh_cdc_acm_process_tx(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_pipe_t *bulk_out = &cdc->bulk_out;

	int status = usbh_transfer_process(host, bulk_out);

	if ((status == HAL_OK) && (bulk_out->xfer_state == USBH_EP_XFER_IDLE)) {
		if ((cdc->cb != NULL) && (cdc->cb->transmit != NULL)) {
			cdc->cb->transmit(status);
		}
	} else if (bulk_out->xfer_state == USBH_EP_XFER_START) {
		usbh_notify_class_state_change(host, bulk_out->pipe_num);
	} else if (bulk_out->xfer_state == USBH_EP_XFER_ERROR) {
		bulk_out->xfer_state = USBH_EP_XFER_IDLE;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK TX fail: %d\n", usbh_get_urb_state(host, bulk_out));
		if ((cdc->cb != NULL) && (cdc->cb->transmit != NULL)) {
			cdc->cb->transmit(status);
		}
	}
}

/**
  * @brief  RX handling
  * @param  host: Host handle
  * @retval None
*/
static void usbh_cdc_acm_process_rx(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_pipe_t *bulk_in = &cdc->bulk_in;
	u32 len;
	int status;

	status = usbh_transfer_process(host, bulk_in);

	if ((status == HAL_OK) && (bulk_in->xfer_state == USBH_EP_XFER_IDLE)) {
		len = usbh_get_last_transfer_size(host, bulk_in);
		if ((cdc->cb != NULL) && (cdc->cb->receive != NULL)) {
			cdc->cb->receive(bulk_in->xfer_buf, len, status);
		}
	} else if (bulk_in->xfer_state == USBH_EP_XFER_START) {
		len = usbh_get_last_transfer_size(host, bulk_in);
		if ((bulk_in->xfer_len == 0) && (cdc->cb != NULL) && (cdc->cb->receive != NULL)) {
			cdc->cb->receive(bulk_in->xfer_buf, len, status);
		}
		usbh_notify_class_state_change(host, bulk_in->pipe_num);
	} else if (bulk_in->xfer_state == USBH_EP_XFER_ERROR) {
		bulk_in->xfer_state = USBH_EP_XFER_IDLE;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK RX fail: %d\n", usbh_get_urb_state(host, bulk_in));
		if ((cdc->cb != NULL) && (cdc->cb->receive != NULL)) {
			cdc->cb->receive(NULL, 0, status);
		}
	}
}

/**
  * @brief  Intr in function
  * @param  host: Host handle
  */
#if CONFIG_USBH_CDC_ACM_NOTIFY
static void usbh_cdc_acm_process_intr_rx(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_pipe_t *intr_in = &cdc->intr_in;
	u32 len;

	int status = usbh_transfer_process(host, intr_in);

	if ((status == HAL_OK) && (intr_in->xfer_state == USBH_EP_XFER_IDLE)) {
		len = usbh_get_last_transfer_size(host, intr_in);
		if ((cdc->cb != NULL) && (cdc->cb->notify != NULL)) {
			if (len >= 8U) {
				/* Parse CDC notification header: byte1=bNotificationCode, bytes6-7=wLength */
				u8 notif_code = intr_in->xfer_buf[1];
				u16 data_len = (u16)intr_in->xfer_buf[6] | ((u16)intr_in->xfer_buf[7] << 8);
				u8 *payload = intr_in->xfer_buf + 8U;
				switch (notif_code) {
				case USB_CDC_ACM_NOTIFY_SERIAL_STATE:
					if (len >= (8U + (u32)data_len)) {
						cdc->cb->notify(payload, (u32)data_len, status);
					} else {
						cdc->cb->notify(NULL, 0U, status);
					}
					break;
				case USB_CDC_ACM_NOTIFY_RESPONSE_AVAILABLE:
					cdc->cb->notify(NULL, 0U, status);
					break;
				default:
					cdc->cb->notify(payload, (u32)data_len, status);
					break;
				}
			} else {
				cdc->cb->notify(intr_in->xfer_buf, len, status);
			}
		}
	} else if (intr_in->xfer_state == USBH_EP_XFER_START) {
		usbh_notify_class_state_change(host, intr_in->pipe_num);
	} else if (intr_in->xfer_state == USBH_EP_XFER_ERROR) {
		intr_in->xfer_state = USBH_EP_XFER_IDLE;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR RX fail: %d\n", usbh_get_urb_state(host, intr_in));
		if ((cdc->cb != NULL) && (cdc->cb->notify != NULL)) {
			cdc->cb->notify(NULL, 0, status);
		}
	}
}
#endif

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init CDC ACM class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_cdc_acm_init(const usbh_cdc_acm_cb_t *cb)
{
	int ret = HAL_OK;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "User init err %d\n", ret);
			return ret;
		}
	}

	cdc->line_coding = (usb_cdc_line_coding_t *)usb_os_malloc(sizeof(usb_cdc_line_coding_t));
	if (cdc->line_coding == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc line code fail\n");
		return HAL_ERR_MEM;
	}
	cdc->user_line_coding = (usb_cdc_line_coding_t *)usb_os_malloc(sizeof(usb_cdc_line_coding_t));
	if (cdc->user_line_coding == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc user line code fail\n");
		usb_os_mfree(cdc->line_coding);
		cdc->line_coding = NULL;
		return HAL_ERR_MEM;
	}

	ret = usbh_register_class(&usbh_cdc_acm_driver);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Register class fail %d\n", ret);
		usb_os_mfree(cdc->line_coding);
		cdc->line_coding = NULL;
		usb_os_mfree(cdc->user_line_coding);
		cdc->user_line_coding = NULL;
		return ret;
	}

	cdc->ctrl_line_state = 0x01U;
	cdc->cb = cb;

	return ret;
}

/**
  * @brief  Deinit CDC ACM class
  * @retval Status
  */
int usbh_cdc_acm_deinit(void)
{
	int ret = HAL_OK;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;

	if ((cdc->cb != NULL) && (cdc->cb->deinit != NULL)) {
		cdc->cb->deinit();
	}

	if ((host != NULL) && (host->connect_state == USBH_STATE_SETUP)) {
		cdc->state = USBH_CDC_ACM_STATE_IDLE;
#if CONFIG_USBH_CDC_ACM_NOTIFY
		/* Guard with pipe_num: an unopened pipe has pipe_num == 0, and
		   usbh_close_pipe() would otherwise halt/free the control pipe (pipe 0). */
		if (cdc->intr_in.pipe_num) {
			usbh_close_pipe(host, &cdc->intr_in);
		}
#endif
		if (cdc->bulk_in.pipe_num) {
			usbh_close_pipe(host, &cdc->bulk_in);
		}
		if (cdc->bulk_out.pipe_num) {
			usbh_close_pipe(host, &cdc->bulk_out);
		}
	}

	if (cdc->line_coding != NULL) {
		usb_os_mfree(cdc->line_coding);
		cdc->line_coding = NULL;
	}
	if (cdc->user_line_coding != NULL) {
		usb_os_mfree(cdc->user_line_coding);
		cdc->user_line_coding = NULL;
	}

	/* Cover the deinit-while-connected path (no detach ran): drop the host
	   handle so post-deinit transmit/receive calls are rejected. The detach
	   path already cleared it. */
	cdc->host = NULL;

	usbh_unregister_class(&usbh_cdc_acm_driver);

	return ret;
}

/**
* @brief  Set the control line status (PSTN §6.3.12)
* @param  bitmap: Control Signal Bitmap: D0=DTR, D1=RTS
* @retval HAL_OK on success, others on failure.
*/
int usbh_cdc_acm_set_control_line_state(u16 bitmap)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	int ret = HAL_ERR_UNKNOWN;

	if (host == NULL) {
		return HAL_ERR_PARA;
	}

	if (host->connect_state == USBH_STATE_SETUP) {
		cdc->ctrl_line_state = bitmap;
		cdc->state = USBH_CDC_ACM_STATE_SET_CONTROL_LINE_STATE;
		usbh_notify_class_state_change(host, 0);
		ret = HAL_OK;
	}

	return ret;
}

/**
* @brief  Send break signal (PSTN §6.3.13)
* @param  duration_ms: Break duration in ms; 0xFFFF = continuous; 0x0000 = end break
* @retval HAL_OK on success, others on failure.
*/
int usbh_cdc_acm_send_break(u16 duration_ms)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	int ret = HAL_ERR_UNKNOWN;

	if (host == NULL) {
		return HAL_ERR_PARA;
	}

	if (host->connect_state == USBH_STATE_SETUP) {
		cdc->break_duration = duration_ms;
		cdc->state = USBH_CDC_ACM_STATE_SEND_BREAK;
		usbh_notify_class_state_change(host, 0);
		ret = HAL_OK;
	}

	return ret;
}

/**
* @brief  Set the line coding parameters (PSTN §6.3.10 SET_LINE_CODING)
* @param  line_coding: Line coding data to apply
* @retval HAL_OK on success, others on failure.
*/
int usbh_cdc_acm_set_line_coding(usb_cdc_line_coding_t *line_coding)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	int ret = HAL_ERR_UNKNOWN;

	if ((host == NULL) || (line_coding == NULL)) {
		return HAL_ERR_PARA;
	}

	if (host->connect_state == USBH_STATE_SETUP) {
		usb_cdc_line_coding_t *ulc = cdc->user_line_coding;
		usb_cdc_line_coding_t *lc = line_coding;
		cdc->state = USBH_CDC_ACM_STATE_SET_LINE_CODING;
		ulc->b.dwDteRate = lc->b.dwDteRate;
		ulc->b.bCharFormat = lc->b.bCharFormat;
		ulc->b.bParityType = lc->b.bParityType;
		ulc->b.bDataBits = lc->b.bDataBits;
		usbh_notify_class_state_change(host, 0);
		ret = HAL_OK;
	}

	return ret;
}

/**
* @brief  Get the current line coding parameters (PSTN §6.3.11 GET_LINE_CODING)
* @param  line_coding: Pointer to store the retrieved line coding data
* @retval HAL_OK on success, others on failure.
*/
int usbh_cdc_acm_get_line_coding(usb_cdc_line_coding_t *line_coding)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	int ret = HAL_ERR_UNKNOWN;

	if ((host == NULL) || (line_coding == NULL)) {
		return HAL_ERR_PARA;
	}

	if ((host->connect_state == USBH_STATE_SETUP) || (host->connect_state == USBH_STATE_ATTACH)) {
		usb_cdc_line_coding_t *lc = cdc->line_coding;
		usb_cdc_line_coding_t *ulc = line_coding;
		ulc->b.dwDteRate = lc->b.dwDteRate;
		ulc->b.bCharFormat = lc->b.bCharFormat;
		ulc->b.bParityType = lc->b.bParityType;
		ulc->b.bDataBits = lc->b.bDataBits;
		ret = HAL_OK;
	}

	return ret;
}

/**
  * @brief  Start to transfer data via BULK OUT pipe
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval HAL_OK on success, others on failure.
  */
int usbh_cdc_acm_transmit(u8 *buf, u32 len)
{
	int ret = HAL_ERR_UNKNOWN;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	usbh_pipe_t *pipe = &cdc->bulk_out;

	if ((host == NULL) || (buf == NULL)) {
		return HAL_ERR_PARA;
	}

	if ((host->connect_state == USBH_STATE_SETUP) &&
		((cdc->state == USBH_CDC_ACM_STATE_IDLE) || (cdc->state == USBH_CDC_ACM_STATE_TRANSFER))) {
		if (pipe->xfer_state == USBH_EP_XFER_IDLE) {
			pipe->xfer_buf = buf;
			pipe->xfer_len = len;

			if ((pipe->xfer_len > 0) && (pipe->xfer_len % pipe->ep_mps) == 0) { //ZLP
				pipe->trx_zlp = 1;
			} else {
				pipe->trx_zlp = 0;
			}

			cdc->state = USBH_CDC_ACM_STATE_TRANSFER;
			pipe->xfer_state = USBH_EP_XFER_START;
			usbh_notify_class_state_change(host, pipe->pipe_num);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Start to receive data via BULK IN pipe
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval HAL_OK on success, others on failure.
  */
int usbh_cdc_acm_receive(u8 *buf, u32 len)
{
	int ret = HAL_ERR_UNKNOWN;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	usbh_pipe_t *pipe = &cdc->bulk_in;

	if ((host == NULL) || (buf == NULL)) {
		return HAL_ERR_PARA;
	}

	if ((host->connect_state == USBH_STATE_SETUP) &&
		((cdc->state == USBH_CDC_ACM_STATE_IDLE) || (cdc->state == USBH_CDC_ACM_STATE_TRANSFER))) {
		if (pipe->xfer_state == USBH_EP_XFER_IDLE) {
			pipe->xfer_buf = buf;
			pipe->xfer_len = len;

			if ((pipe->xfer_len > 0) && (pipe->xfer_len % pipe->ep_mps) == 0) { //ZLP
				pipe->trx_zlp = 1;
			} else {
				pipe->trx_zlp = 0;
			}

			cdc->state = USBH_CDC_ACM_STATE_TRANSFER;
			pipe->xfer_state = USBH_EP_XFER_START;
			usbh_notify_class_state_change(host, pipe->pipe_num);
			ret = HAL_OK;
		}
	}

	return ret;
}

#if CONFIG_USBH_CDC_ACM_NOTIFY
/**
  * @brief  Start to receive notify data via INTERRUPT IN pipe
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval HAL_OK on success, others on failure.
  */
int usbh_cdc_acm_notify_receive(u8 *buf, u32 len)
{
	int ret = HAL_ERR_UNKNOWN;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	usbh_pipe_t *pipe = &cdc->intr_in;

	if ((host == NULL) || (buf == NULL)) {
		return HAL_ERR_PARA;
	}

	if ((host->connect_state == USBH_STATE_SETUP) &&
		((cdc->state == USBH_CDC_ACM_STATE_IDLE) || (cdc->state == USBH_CDC_ACM_STATE_TRANSFER))) {
		if (pipe->xfer_state == USBH_EP_XFER_IDLE) {
			pipe->xfer_buf = buf;
			pipe->xfer_len = pipe->ep_mps;

			/* The user buf len < MPS, update the xfer length */
			if (len < pipe->ep_mps) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Pls inc inbuf len %u-%u\n", len, pipe->ep_mps);
				pipe->xfer_len = len;
			}

			cdc->state = USBH_CDC_ACM_STATE_TRANSFER;
			pipe->xfer_state = USBH_EP_XFER_START;
			usbh_notify_class_state_change(host, pipe->pipe_num);
			ret = HAL_OK;
		}
	}

	return ret;
}
#endif

/**
  * @brief  Get the BULK IN max packet size
  * @retval Max packet size
  */
u16 usbh_cdc_acm_get_bulk_ep_mps(void)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	return cdc->bulk_in.ep_mps;
}
