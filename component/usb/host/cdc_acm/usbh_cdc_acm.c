/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_cdc_acm.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/

#define USB_BULK_OUT_MAX_TIMEOUT_TICK          8000 //sof
#define USB_BULK_IN_MAX_TIMEOUT_TICK           100  //sof
#define USB_INTR_MAX_TIMEOUT_TICK              1000

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbh_cdc_acm_attach(usb_host_t *host);
static int usbh_cdc_acm_detach(usb_host_t *host);
static int usbh_cdc_acm_process(usb_host_t *host, u32 msg);
static int usbh_cdc_acm_setup(usb_host_t *host);
static int usbh_cdc_acm_process_get_line_coding(usb_host_t *host, usbh_cdc_acm_line_coding_t *linecoding);
static int usbh_cdc_acm_process_set_line_coding(usb_host_t *host, usbh_cdc_acm_line_coding_t *linecoding);
static int usbh_cdc_acm_process_set_control_line_state(usb_host_t *host);
static void usbh_cdc_acm_process_tx(usb_host_t *host);
static void usbh_cdc_acm_process_rx(usb_host_t *host);
static void usbh_cdc_acm_process_intr_rx(usb_host_t *host);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "ACM";

/* USB CDC ACM device identification */
static const usbh_dev_id_t cdc_acm_devs[] = {
	{
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS,
		.bInterfaceClass = CDC_CLASS_CODE,
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
};

static usbh_cdc_acm_host_t usbh_cdc_acm_host;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Attach callback.
  * @param  host: Host handle
  * @retval Status
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
	usbh_pipe_t *intr_in = &cdc->intr_in;
	usbh_dev_id_t dev_id = {0,};

	/* Get Communication Interface */
	dev_id.bInterfaceClass = CDC_COMM_INTERFACE_CLASS_CODE;
	dev_id.bInterfaceSubClass = CDC_ABSTRACT_CONTROL_MODEL;
	dev_id.bInterfaceProtocol = CDC_CTRL_PROTOCOL_COMMON_AT_COMMAND;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);
	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get comm itf fail\n");
		return status;
	} else 	{
		comm_itf_desc = itf_data->itf_desc_array;
		/* Set notification endpoint */
		ep_desc = &comm_itf_desc->ep_desc_array[0];
		if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
			usbh_open_pipe(host, intr_in, ep_desc);
			intr_in->max_timeout_tick = USB_INTR_MAX_TIMEOUT_TICK;
		}
	}

	/* Get Data Interface */
	dev_id.bInterfaceClass = CDC_DATA_INTERFACE_CLASS_CODE;
	dev_id.bInterfaceSubClass = CDC_RESERVED;
	dev_id.bInterfaceProtocol = CDC_CTRL_PROTOCOL_NO_CLASS_SPECIFIC;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);
	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get data itf fail\n");
		return status;
	} else {
		data_itf_desc = itf_data->itf_desc_array;
		/* Set data in/out endpoints */
		for (int i = 0; i < 2; i++) {
			ep_desc = &data_itf_desc->ep_desc_array[i];
			if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
				usbh_open_pipe(host, bulk_in, ep_desc);
				bulk_in->max_timeout_tick = USB_BULK_IN_MAX_TIMEOUT_TICK;
			} else {
				usbh_open_pipe(host, bulk_out, ep_desc);
				bulk_out->max_timeout_tick = USB_BULK_OUT_MAX_TIMEOUT_TICK;
			}
		}
	}
	cdc->host = host;
	cdc->state = CDC_ACM_STATE_IDLE;

	if ((cdc->cb != NULL) && (cdc->cb->attach != NULL)) {
		cdc->cb->attach();
	}

	return HAL_OK;
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
	usbh_pipe_t *intr_in = &cdc->intr_in;

	if ((cdc->cb != NULL) && (cdc->cb->detach != NULL)) {
		cdc->cb->detach();
	}

	if (intr_in->pipe_num) {
		usbh_close_pipe(host, intr_in);
	}

	if (bulk_in->pipe_num) {
		usbh_close_pipe(host, bulk_in);
	}

	if (bulk_out->pipe_num) {
		usbh_close_pipe(host, bulk_out);
	}

	return HAL_OK;
}

/**
  * @brief  Standard control requests handling callback
  * @param  host: Host handle
  * @retval Status
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

/**
* @brief  State machine handling callback
* @param  host:Host handle
* @param  msg: Message data
* @retval Status
*/
static int usbh_cdc_acm_process(usb_host_t *host, u32 msg)
{
	int status = HAL_BUSY;
	u8 req_status = HAL_OK;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_event_t event;
	event.d32 = msg;

	switch (cdc->state) {

	case CDC_ACM_STATE_IDLE:
		status = HAL_OK;
		break;

	case CDC_ACM_STATE_SET_CONTROL_LINE_STATE:
		req_status = usbh_cdc_acm_process_set_control_line_state(host);
		if (req_status == HAL_OK) {
			cdc->state = CDC_ACM_STATE_IDLE;
		} else if (req_status != HAL_BUSY) {
			cdc->state = CDC_ACM_STATE_ERROR;
		}
		usbh_notify_class_state_change(host, 0);
		break;

	case CDC_ACM_STATE_SET_LINE_CODING:
		req_status = usbh_cdc_acm_process_set_line_coding(host, cdc->user_line_coding);
		if (req_status == HAL_OK) {
			cdc->state = CDC_ACM_STATE_GET_LINE_CODING;
		} else if (req_status != HAL_BUSY) {
			cdc->state = CDC_ACM_STATE_ERROR;
		}
		usbh_notify_class_state_change(host, 0);
		break;

	case CDC_ACM_STATE_GET_LINE_CODING:
		req_status = usbh_cdc_acm_process_get_line_coding(host, cdc->line_coding);
		if (req_status == HAL_OK) {
			cdc->state = CDC_ACM_STATE_IDLE;
			if ((cdc->line_coding->b.bCharFormat == cdc->user_line_coding->b.bCharFormat) &&
				(cdc->line_coding->b.bDataBits == cdc->user_line_coding->b.bDataBits) &&
				(cdc->line_coding->b.bParityType == cdc->user_line_coding->b.bParityType) &&
				(cdc->line_coding->b.dwDteRate == cdc->user_line_coding->b.dwDteRate)) {
				if ((cdc->cb != NULL) && (cdc->cb->line_coding_changed != NULL)) {
					cdc->cb->line_coding_changed(cdc->line_coding);
				}
			}
		} else if (req_status != HAL_BUSY) {
			cdc->state = CDC_ACM_STATE_ERROR;
		}
		usbh_notify_class_state_change(host, 0);
		break;

	case CDC_ACM_STATE_TRANSFER:
		if (event.msg.pipe_num == cdc->bulk_out.pipe_num) {
			usbh_cdc_acm_process_tx(host);
		} else if (event.msg.pipe_num == cdc->bulk_in.pipe_num) {
			usbh_cdc_acm_process_rx(host);
		} else if (event.msg.pipe_num == cdc->intr_in.pipe_num) {
			usbh_cdc_acm_process_intr_rx(host);
		}
		break;

	case CDC_ACM_STATE_ERROR:
		req_status = usbh_ctrl_clear_feature(host, 0x00U);
		if (req_status == HAL_OK) {
			cdc->state = CDC_ACM_STATE_IDLE;
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
static int usbh_cdc_acm_process_get_line_coding(usb_host_t *host, usbh_cdc_acm_line_coding_t *linecoding)
{
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = CDC_GET_LINE_CODING;
	setup.req.wValue = 0U;
	setup.req.wIndex = 0U;
	setup.req.wLength = CDC_LINE_CODING_DATA_LEN;

	return usbh_ctrl_request(host, &setup, linecoding->d8);
}

/**
  * @brief  Set line coding
  * @param  host: Host handle
  * @param  linecoding: Line coding data pointer
  * @retval Status
  */
static int usbh_cdc_acm_process_set_line_coding(usb_host_t *host, usbh_cdc_acm_line_coding_t *linecoding)
{
	usbh_setup_req_t setup;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = CDC_SET_LINE_CODING;
	setup.req.wValue = 0U;
	setup.req.wIndex = 0U;
	setup.req.wLength = CDC_LINE_CODING_DATA_LEN;

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

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = CDC_SET_CONTROL_LINE_STATE;
	setup.req.wValue = 1U;
	setup.req.wIndex = 0U;
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
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK RX fail: %d\n", usbh_get_urb_state(host, bulk_in));
		if ((cdc->cb != NULL) && (cdc->cb->receive != NULL)) {
			cdc->cb->receive(NULL, 0, status);
		}
	}
}

/**
  * @brief  Intr in function
  * @param  host: Host handle
  * @retval Status
*/
static void usbh_cdc_acm_process_intr_rx(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_pipe_t *intr_in = &cdc->intr_in;
	u32 len;

	int status = usbh_transfer_process(host, intr_in);

	if ((status == HAL_OK) && (intr_in->xfer_state == USBH_EP_XFER_IDLE)) {
		len = usbh_get_last_transfer_size(host, intr_in);
		if ((cdc->cb != NULL) && (cdc->cb->receive != NULL)) {
			cdc->cb->notify(intr_in->xfer_buf, len, status);
		}
	} else if ((intr_in->xfer_state == USBH_EP_XFER_START)) {
		usbh_notify_class_state_change(host, intr_in->pipe_num);
	} else if (intr_in->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR RX fail: %d\n", usbh_get_urb_state(host, intr_in));
		if ((cdc->cb != NULL) && (cdc->cb->receive != NULL)) {
			cdc->cb->notify(NULL, 0, status);
		}
	}
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init CDC ACM class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_cdc_acm_init(usbh_cdc_acm_cb_t *cb)
{
	int ret = HAL_OK;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	if (cb != NULL) {
		cdc->cb = cb;
		if (cb->init != NULL) {
			ret = cb->init();
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "User init err %d\n", ret);
				return ret;
			}
		}
	}

	cdc->line_coding = (usbh_cdc_acm_line_coding_t *)usb_os_malloc(sizeof(usbh_cdc_acm_line_coding_t));
	if (cdc->line_coding == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc line code fail\n");
		return HAL_ERR_MEM;
	}
	cdc->user_line_coding = (usbh_cdc_acm_line_coding_t *)usb_os_malloc(sizeof(usbh_cdc_acm_line_coding_t));
	if (cdc->user_line_coding == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc user line code fail\n");
		usb_os_mfree(cdc->line_coding);
		cdc->line_coding = NULL;
		return HAL_ERR_MEM;
	}

	usbh_register_class(&usbh_cdc_acm_driver);

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
		cdc->state = CDC_ACM_STATE_IDLE;

		usbh_close_pipe(host, &cdc->intr_in);
		usbh_close_pipe(host, &cdc->bulk_in);
		usbh_close_pipe(host, &cdc->bulk_out);
	}

	if (cdc->line_coding != NULL) {
		usb_os_mfree(cdc->line_coding);
		cdc->line_coding = NULL;
	}
	if (cdc->user_line_coding != NULL) {
		usb_os_mfree(cdc->user_line_coding);
		cdc->user_line_coding = NULL;
	}

	usbh_unregister_class(&usbh_cdc_acm_driver);

	return ret;
}

/**
* @brief  Set the control line status
* @retval Status
*/
int usbh_cdc_acm_set_control_line_state(void)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	int ret = HAL_ERR_UNKNOWN;

	if (host->connect_state == USBH_STATE_SETUP) {
		cdc->state = CDC_ACM_STATE_SET_CONTROL_LINE_STATE;
		usbh_notify_class_state_change(host, 0);
		ret = HAL_OK;
	}

	return ret;
}

/**
* @brief  Prepares the state before issuing the class specific commands
* @param  line_coding: Line coding data
* @retval Status
*/
int usbh_cdc_acm_set_line_coding(usbh_cdc_acm_line_coding_t *line_coding)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	int ret = HAL_ERR_UNKNOWN;

	if (host->connect_state == USBH_STATE_SETUP) {
		cdc->state = CDC_ACM_STATE_SET_LINE_CODING;
		cdc->user_line_coding->b.dwDteRate = line_coding->b.dwDteRate;
		cdc->user_line_coding->b.bCharFormat = line_coding->b.bCharFormat;
		cdc->user_line_coding->b.bParityType = line_coding->b.bParityType;
		cdc->user_line_coding->b.bDataBits = line_coding->b.bDataBits;
		usbh_notify_class_state_change(host, 0);
		ret = HAL_OK;
	}

	return ret;
}

/**
* @brief  Prepares the state before issuing the class specific commands
* @param  line_coding: Line coding data
* @retval Status
*/
int usbh_cdc_acm_get_line_coding(usbh_cdc_acm_line_coding_t *line_coding)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	int ret = HAL_ERR_UNKNOWN;

	if ((host->connect_state == USBH_STATE_SETUP) || (host->connect_state == USBH_STATE_ATTACH)) {
		line_coding->b.dwDteRate = cdc->line_coding->b.dwDteRate;
		line_coding->b.bCharFormat = cdc->line_coding->b.bCharFormat;
		line_coding->b.bParityType = cdc->line_coding->b.bParityType;
		line_coding->b.bDataBits = cdc->line_coding->b.bDataBits;
		ret = HAL_OK;
	}

	return ret;
}

/**
  * @brief  Start to transfer data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbh_cdc_acm_transmit(u8 *buf, u32 len)
{
	int ret = HAL_BUSY;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	usbh_pipe_t *pipe = &cdc->bulk_out;

	if ((cdc->state == CDC_ACM_STATE_IDLE) || (cdc->state == CDC_ACM_STATE_TRANSFER)) {
		if (pipe->xfer_state == USBH_EP_XFER_IDLE) {
			pipe->xfer_buf = buf;
			pipe->xfer_len = len;

			if ((pipe->xfer_len > 0) && (pipe->xfer_len % pipe->ep_mps) == 0) { //ZLP
				pipe->trx_zlp = 1;
			} else {
				pipe->trx_zlp = 0;
			}

			cdc->state = CDC_ACM_STATE_TRANSFER;
			pipe->xfer_state = USBH_EP_XFER_START;
			usbh_notify_class_state_change(host, pipe->pipe_num);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Start to receive data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbh_cdc_acm_receive(u8 *buf, u32 len)
{
	int ret = HAL_BUSY;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	usbh_pipe_t *pipe = &cdc->bulk_in;

	if ((cdc->state == CDC_ACM_STATE_IDLE) || (cdc->state == CDC_ACM_STATE_TRANSFER)) {
		if (pipe->xfer_state == USBH_EP_XFER_IDLE) {
			pipe->xfer_buf = buf;
			pipe->xfer_len = len;

			if ((pipe->xfer_len > 0) && (pipe->xfer_len % pipe->ep_mps) == 0) { //ZLP
				pipe->trx_zlp = 1;
			} else {
				pipe->trx_zlp = 0;
			}

			cdc->state = CDC_ACM_STATE_TRANSFER;
			pipe->xfer_state = USBH_EP_XFER_START;
			usbh_notify_class_state_change(host, pipe->pipe_num);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Start to receive notify data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbh_cdc_acm_notify_receive(u8 *buf, u32 len)
{
	int ret = HAL_BUSY;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	usbh_pipe_t *pipe = &cdc->intr_in;

	if ((cdc->state == CDC_ACM_STATE_IDLE) || (cdc->state == CDC_ACM_STATE_TRANSFER)) {
		pipe->xfer_buf = buf;
		pipe->xfer_len = pipe->ep_mps;

		/* The user buf len < MPS, update the xfer length */
		if (len < pipe->ep_mps) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Pls inc inbuf len %d-%d\n", len, pipe->ep_mps);
			pipe->xfer_len = len;
		}

		cdc->state = CDC_ACM_STATE_TRANSFER;
		pipe->xfer_state = USBH_EP_XFER_START;
		usbh_notify_class_state_change(host, pipe->pipe_num);
		ret = HAL_OK;
	}

	return ret;
}

/**
  * @brief  Get the BULK IN max packet size
  * @retval Max packet size
  */
u16 usbh_cdc_acm_get_bulk_ep_mps(void)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	return cdc->bulk_in.ep_mps;
}

