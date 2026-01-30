/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_composite_cdc_acm.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/

#define USB_BULK_OUT_MAX_TIMEOUT_TICK          8000 //sof
#define USB_BULK_IN_MAX_TIMEOUT_TICK           100  //sof
#define USB_INTR_MAX_TIMEOUT_TICK              1000
#define USBH_CDC_ACM_NOTIFY_BUF_SIZE           256
#define USBH_CDC_ACM_LOOPBACK_BUF_SIZE         1024

/* CDC ACM state */
typedef enum {
	CDC_ACM_STATE_IDLE = 0U,
	CDC_ACM_STATE_TRANSFER,
	CDC_ACM_STATE_ERROR,
} usbh_composite_cdc_acm_state_t;

typedef enum {
	CDC_ACM_STATE_CTRL_IDLE  = 0U,
	CDC_ACM_STATE_SET_LINE_CODING,
	CDC_ACM_STATE_GET_LINE_CODING,
	CDC_ACM_STATE_CTRL_SETTING_MAX,
} usbh_composite_cdc_acm_ctrl_state_t;

/* Private types -------------------------------------------------------------*/
static u8 usbh_composite_cdc_acm_tx_buf[USBH_CDC_ACM_LOOPBACK_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 usbh_composite_cdc_acm_notify_rx_buf[USBH_CDC_ACM_NOTIFY_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static int usbh_composite_cdc_acm_attach(usb_host_t *host);
static int usbh_composite_cdc_acm_detach(usb_host_t *host);
static int usbh_composite_cdc_acm_process(usb_host_t *host, u32 msg);
static int usbh_composite_cdc_acm_sof(usb_host_t *host);
static int usbh_composite_cdc_acm_setup(usb_host_t *host);
static int usbh_composite_cdc_acm_get_line_coding(usb_host_t *host, usb_cdc_line_coding_t *linecoding);
static int usbh_composite_cdc_acm_set_line_coding(usb_host_t *host, usb_cdc_line_coding_t *linecoding);
static int usbh_composite_cdc_acm_process_tx(usb_host_t *host);
static int usbh_composite_cdc_acm_process_rx(usb_host_t *host);
static int usbh_composite_cdc_acm_process_intr_rx(usb_host_t *host);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "ACM";

/* USB Standard Device Descriptor */
const usbh_class_driver_t usbh_composite_cdc_acm_driver = {
	.attach = usbh_composite_cdc_acm_attach,
	.detach = usbh_composite_cdc_acm_detach,
	.setup = usbh_composite_cdc_acm_setup,
	.process = usbh_composite_cdc_acm_process,
	.sof = usbh_composite_cdc_acm_sof,
};

static usbh_composite_cdc_acm_host_t usbh_composite_cdc_acm_host;

/* Private functions ---------------------------------------------------------*/
static void usbh_composite_cdc_acm_dump_desc(void)
{
#if 1
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usbh_pipe_t *pipe_info;

	RTK_LOGS(TAG, RTK_LOG_INFO, "--------------------Dump Descriptor Start------------------\n");

	//dump cdc control
	pipe_info = &(cdc->intr_in);
	RTK_LOGS(TAG, RTK_LOG_INFO, "INTR pipe(%d)interval(%d)\n", pipe_info->pipe_num, pipe_info->ep_interval);

	//dump ac, data
	pipe_info = &(cdc->bulk_out);
	RTK_LOGS(TAG, RTK_LOG_INFO, "BULK out:pipe(%d)interval(%d)\n", pipe_info->pipe_num, pipe_info->ep_interval);

	pipe_info = &(cdc->bulk_in);
	RTK_LOGS(TAG, RTK_LOG_INFO, "BULK in:pipe(%d)interval(%d)\n", pipe_info->pipe_num, pipe_info->ep_interval);

	RTK_LOGS(TAG, RTK_LOG_INFO, "---------------------Dump End-----------------------------\n");
#endif
}

/**
  * @brief  Start to receive data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
static int usbh_composite_cdc_acm_receive(u8 *buf, u32 len)
{
	int ret = HAL_BUSY;
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usb_host_t *host = cdc->driver->host;
	usbh_pipe_t *pipe = &cdc->bulk_in;

	if ((cdc->state == CDC_ACM_STATE_IDLE) || (cdc->state == CDC_ACM_STATE_TRANSFER)) {
		if (pipe->xfer_state == USBH_EP_XFER_IDLE || pipe->xfer_state == USBH_EP_XFER_ERROR) {
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
static int usbh_composite_cdc_acm_notify_receive(u8 *buf, u32 len)
{
	int ret = HAL_BUSY;
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usb_host_t *host = cdc->driver->host;
	usbh_pipe_t *pipe = &cdc->intr_in;

	if ((cdc->state == CDC_ACM_STATE_IDLE) || (cdc->state == CDC_ACM_STATE_TRANSFER)) {
		pipe->xfer_buf = buf;
		pipe->xfer_len = pipe->ep_mps;

		/* The user buf len < MPS, update the xfer length */
		if (len < pipe->ep_mps) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Pls inc inbuf len %d-%d\n", len, pipe->ep_mps);
			pipe->xfer_len = len;
		}

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

	return ret;
}

/**
  * @brief  Attach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_cdc_acm_attach(usb_host_t *host)
{
	int status = HAL_ERR_UNKNOWN;
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usbh_itf_data_t *itf_data;
	usbh_itf_desc_t *comm_if_desc;
	usbh_ep_desc_t *ep_desc;
	usbh_pipe_t *bulk_out = &cdc->bulk_out;
	usbh_pipe_t *bulk_in = &cdc->bulk_in;
	usbh_pipe_t *intr_in = &cdc->intr_in;
	usbh_dev_id_t dev_id_info = {0,};

	/*
		Get Communication Interface

	*/
#if USB_4G_DONGLE_SUPPORT
	usbh_composite_cdc_acm_param_t *data = cdc->priv_param;
	u16 vid = host->dev_desc->idVendor;
	u16 pid = host->dev_desc->idProduct;
	u8 ep_id;

	cdc->param_item = NULL;
	while (data->vid != 0) {
		if ((data->vid == vid) && (data->pid == pid)) { /*rtk dongle*/
			cdc->param_item = data;
			break;
		}
		data++;
	}

	if (cdc->param_item == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Can not find acm cfg\n");
		return status;
	}

	dev_id_info.bInterfaceClass = 0xFF;
	dev_id_info.bInterfaceSubClass = 0x00;
	dev_id_info.bInterfaceProtocol = 0x00;
	dev_id_info.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id_info);
	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get comm itf fail\n");
		return status;
	} else 	{
		while (itf_data) {
			comm_if_desc = itf_data->itf_desc_array;
			if ((comm_if_desc->bInterfaceNumber == cdc->param_item->at_line_idx) && (comm_if_desc->bAlternateSetting == 0)) {
				if (comm_if_desc->bNumEndpoints != 3) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "EP err\n");
				}

				for (ep_id = 0 ; ep_id < comm_if_desc->bNumEndpoints ; ep_id++) {
					ep_desc = &comm_if_desc->ep_desc_array[ep_id];

					if (ep_desc->bmAttributes == USB_CH_EP_TYPE_BULK) {
						if (USB_EP_IS_IN(ep_desc->bEndpointAddress)) {
							usbh_open_pipe(host, bulk_in, ep_desc);
							bulk_in->max_timeout_tick = USB_BULK_IN_MAX_TIMEOUT_TICK;
						} else {
							usbh_open_pipe(host, bulk_out, ep_desc);
							bulk_out->max_timeout_tick = USB_BULK_IN_MAX_TIMEOUT_TICK;
						}
					} else if (ep_desc->bmAttributes == USB_CH_EP_TYPE_INTR) {
						usbh_open_pipe(host, intr_in, ep_desc);
						intr_in->max_timeout_tick = USB_INTR_MAX_TIMEOUT_TICK;
					} else {
						RTK_LOGS(TAG, RTK_LOG_INFO,  "Unknown support transfer type(%d)\n", ep_desc->bmAttributes);
					}
				}

			}
			itf_data = itf_data->next;
		}
	}
#else
	usbh_itf_desc_t *data_if_desc;
	dev_id_info.bInterfaceClass = CDC_COMM_INTERFACE_CLASS_CODE;
	dev_id_info.bInterfaceSubClass = CDC_ABSTRACT_CONTROL_MODEL;
	dev_id_info.bInterfaceProtocol = CDC_CTRL_PROTOCOL_COMMON_AT_COMMAND;
	dev_id_info.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;

	itf_data = usbh_get_interface_descriptor(host, &dev_id_info);
	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get comm itf fail\n");
		return status;
	} else 	{
		comm_if_desc = itf_data->alt_set_all;
		/* Set notification endpoint */
		ep_desc = &comm_if_desc->ep_desc_all[0];
		if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
			usbh_open_pipe(host, intr_in, ep_desc);
			intr_in->max_timeout_tick = USB_INTR_MAX_TIMEOUT_TICK;
		}
	}

	/* Get Data Interface */
	dev_id_info.bInterfaceClass = CDC_DATA_INTERFACE_CLASS_CODE;
	dev_id_info.bInterfaceSubClass = CDC_RESERVED;
	dev_id_info.bInterfaceProtocol = CDC_CTRL_PROTOCOL_NO_CLASS_SPECIFIC;
	dev_id_info.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id_info);
	if (itf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get data itf fail\n");
		return status;
	} else {
		data_if_desc = itf_data->alt_set_all;
		/* Set data in/out endpoints */
		for (int i = 0; i < 2; i++) {
			ep_desc = &data_if_desc->ep_desc_all[i];
			if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
				usbh_open_pipe(host, bulk_in, ep_desc);
				bulk_in->max_timeout_tick = USB_BULK_IN_MAX_TIMEOUT_TICK;
			} else {
				usbh_open_pipe(host, bulk_out, ep_desc);
				bulk_out->max_timeout_tick = USB_BULK_OUT_MAX_TIMEOUT_TICK;
			}
		}
	}
#endif

	usbh_composite_cdc_acm_dump_desc();
	cdc->state = CDC_ACM_STATE_IDLE;
	cdc->sub_status = CDC_ACM_STATE_SET_LINE_CODING;

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
static int usbh_composite_cdc_acm_detach(usb_host_t *host)
{
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
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
static int usbh_composite_cdc_acm_setup(usb_host_t *host)
{
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;

	if ((cdc->cb != NULL) && (cdc->cb->setup != NULL)) {
		cdc->cb->setup();
	}

	cdc->state = CDC_ACM_STATE_TRANSFER;
	usbh_notify_class_state_change(host, 0);

	return HAL_OK;
}

/**
* @brief  State machine handling callback
* @param  host:Host handle
* @retval Status
*/
static int usbh_composite_cdc_acm_process(usb_host_t *host, u32 msg)
{
	int status = HAL_BUSY;
	u8 req_status = HAL_OK;
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usbh_event_t event;
	event.d32 = msg;

	switch (cdc->state) {

	case CDC_ACM_STATE_IDLE:
		status = HAL_OK;
		break;

	case CDC_ACM_STATE_TRANSFER:
		if (event.msg.pipe_num == cdc->bulk_out.pipe_num) {
			usbh_composite_cdc_acm_process_tx(host);
		} else if (event.msg.pipe_num == cdc->bulk_in.pipe_num) {
			usbh_composite_cdc_acm_process_rx(host);
		} else if (event.msg.pipe_num == cdc->intr_in.pipe_num) {
			usbh_composite_cdc_acm_process_intr_rx(host);
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

static int usbh_composite_cdc_acm_sof(usb_host_t *host)
{
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usbh_pipe_t *pipe = &(cdc->intr_in);

	/*
		if cur_frame - last frame_num  >= interval, means we should trigger a xfer asap
	*/
	if (usbh_get_elapsed_frame_cnt(host, pipe->frame_num) >= pipe->ep_interval) {
		usbh_composite_cdc_acm_notify_receive(usbh_composite_cdc_acm_notify_rx_buf, USBH_CDC_ACM_NOTIFY_BUF_SIZE);
	}

	pipe = &(cdc->bulk_in);
	if ((pipe->xfer_state == USBH_EP_XFER_IDLE) || (pipe->xfer_state == USBH_EP_XFER_ERROR)) {
		usbh_composite_cdc_acm_receive(usbh_composite_cdc_acm_tx_buf, USBH_CDC_ACM_LOOPBACK_BUF_SIZE);
	}

	return HAL_OK;
}

/**
  * @brief  Get line coding
  * @param  host: Host handle
  * @param  linecoding: Line coding data pointer
  * @retval Status
  */
static int usbh_composite_cdc_acm_get_line_coding(usb_host_t *host, usb_cdc_line_coding_t *linecoding)
{
	usbh_setup_req_t setup;
	u16 windex = 0;
#if USB_4G_DONGLE_SUPPORT
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	if (cdc->param_item) {
		windex = cdc->param_item->at_line_idx;
	}
#endif

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_CDC_ACM_GET_LINE_CODING;
	setup.req.wValue = 0U;
	setup.req.wIndex = windex;
	setup.req.wLength = USB_CDC_ACM_LINE_CODING_SIZE;

	return usbh_ctrl_request(host, &setup, linecoding->d8);
}

/**
  * @brief  Set line coding
  * @param  host: Host handle
  * @param  linecoding: Line coding data pointer
  * @retval Status
  */
static int usbh_composite_cdc_acm_set_line_coding(usb_host_t *host, usb_cdc_line_coding_t *linecoding)
{
	usbh_setup_req_t setup;
	u16 windex = 0;
#if USB_4G_DONGLE_SUPPORT
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	if (cdc->param_item) {
		windex = cdc->param_item->at_line_idx;
	}
#endif

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_CDC_ACM_SET_LINE_CODING;
	setup.req.wValue = 0U;
	setup.req.wIndex = windex;
	setup.req.wLength = USB_CDC_ACM_LINE_CODING_SIZE;

	return usbh_ctrl_request(host, &setup, linecoding->d8);
}

/**
  * @brief  TX handling
  * @param  host: Host handle
  * @retval Status
*/
static int usbh_composite_cdc_acm_process_tx(usb_host_t *host)
{
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usbh_pipe_t *bulk_out = &cdc->bulk_out;

	int status = usbh_transfer_process(host, bulk_out);

	if ((status == HAL_OK) && (bulk_out->xfer_state == USBH_EP_XFER_IDLE)) {
		if ((cdc->cb != NULL) && (cdc->cb->bulk_send != NULL)) {
			cdc->cb->bulk_send(status);
		}
	} else if (bulk_out->xfer_state == USBH_EP_XFER_START) {
		usbh_notify_class_state_change(host, bulk_out->pipe_num);
	} else if (bulk_out->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK TX fail: %d\n", usbh_get_urb_state(host, bulk_out));
		if ((cdc->cb != NULL) && (cdc->cb->bulk_send != NULL)) {
			cdc->cb->bulk_send(status);
		}
	}

	return HAL_OK;
}

/**
  * @brief  RX handling
  * @param  host: Host handle
  * @retval None
*/
static int usbh_composite_cdc_acm_process_rx(usb_host_t *host)
{
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usbh_pipe_t *bulk_in = &cdc->bulk_in;

	int status = usbh_transfer_process(host, bulk_in);
	u32 rx_len = usbh_get_last_transfer_size(host, bulk_in);
	if ((status == HAL_OK) && (bulk_in->xfer_state == USBH_EP_XFER_IDLE)) {
		if ((cdc->cb != NULL) && (cdc->cb->bulk_received != NULL)) {
			cdc->cb->bulk_received(bulk_in->xfer_buf, rx_len);
		}
	} else if (bulk_in->xfer_state == USBH_EP_XFER_START) {
		usbh_notify_class_state_change(host, bulk_in->pipe_num);
	} else if (bulk_in->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK RX fail: %d\n", usbh_get_urb_state(host, bulk_in));
		if ((cdc->cb != NULL) && (cdc->cb->bulk_received != NULL)) {
			cdc->cb->bulk_received(bulk_in->xfer_buf, rx_len);
		}
	}

	return HAL_OK;
}

/**
  * @brief  Intr in function
  * @param  host: Host handle
  * @retval Status
*/
static int usbh_composite_cdc_acm_process_intr_rx(usb_host_t *host)
{
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usbh_pipe_t *intr_in = &cdc->intr_in;

	int status = usbh_transfer_process(host, intr_in);
	u32 rx_len = usbh_get_last_transfer_size(host, intr_in);
	if ((status == HAL_OK) && (intr_in->xfer_state == USBH_EP_XFER_IDLE)) {
		if ((cdc->cb != NULL) && (cdc->cb->intr_received != NULL)) {
			cdc->cb->intr_received(intr_in->xfer_buf, rx_len);
		}
	} else if ((intr_in->xfer_state == USBH_EP_XFER_START)) {
		usbh_notify_class_state_change(host, intr_in->pipe_num);
	} else if (intr_in->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR RX fail: %d\n", usbh_get_urb_state(host, intr_in));
		if ((cdc->cb != NULL) && (cdc->cb->intr_received != NULL)) {
			cdc->cb->intr_received(intr_in->xfer_buf, rx_len);
		}
	}

	return HAL_OK;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init CDC ACM class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_composite_cdc_acm_init(usbh_composite_host_t *driver, usbh_composite_cdc_acm_usr_cb_t *cb)
{
	int ret = HAL_OK;
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usb_os_memset(cdc, 0x00, sizeof(usbh_composite_cdc_acm_host_t));

	cdc->priv_param = cb->priv;
	cdc->driver = driver;

#if USB_4G_DONGLE_SUPPORT
	if (NULL == cdc->priv_param) {
		return HAL_ERR_PARA;
	}
#endif

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

	/* set acm line coding */
	cdc->user_line_coding->b.dwDteRate = 115200;
	cdc->user_line_coding->b.bCharFormat = LINE_CODING_CHAR_FORMAT_1_STOP_BITS;
	cdc->user_line_coding->b.bParityType = LINE_CODING_PARITY_NO;
	cdc->user_line_coding->b.bDataBits = 8;

	return ret;
}

/**
  * @brief  Deinit CDC ACM class
  * @retval Status
  */
int usbh_composite_cdc_acm_deinit(void)
{
	int ret = HAL_OK;
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usb_host_t *host = cdc->driver->host;

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

	return ret;
}

int usbh_composite_cdc_acm_ctrl_setting(usb_host_t *host)
{
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	u8 ret_state = HAL_ERR_UNKNOWN;
	u8 state = HAL_OK;

	switch (cdc->sub_status) {
	case CDC_ACM_STATE_CTRL_IDLE:
		ret_state = HAL_OK;
		break;
	case CDC_ACM_STATE_SET_LINE_CODING:
		state = usbh_composite_cdc_acm_set_line_coding(host, cdc->user_line_coding);
		if (state == HAL_OK) {
			cdc->sub_status = CDC_ACM_STATE_GET_LINE_CODING;
		} else if (state != HAL_BUSY) {
			cdc->sub_status = CDC_ACM_STATE_CTRL_IDLE;
		}
		usbh_notify_class_state_change(host, 0);
		break;

	case CDC_ACM_STATE_GET_LINE_CODING:
		state = usbh_composite_cdc_acm_get_line_coding(host, cdc->line_coding);
		if (state == HAL_OK) {
			cdc->sub_status = CDC_ACM_STATE_CTRL_IDLE;
			if ((cdc->line_coding->b.bCharFormat == cdc->user_line_coding->b.bCharFormat) &&
				(cdc->line_coding->b.bDataBits == cdc->user_line_coding->b.bDataBits) &&
				(cdc->line_coding->b.bParityType == cdc->user_line_coding->b.bParityType) &&
				(cdc->line_coding->b.dwDteRate == cdc->user_line_coding->b.dwDteRate)) {
				if ((cdc->cb != NULL) && (cdc->cb->line_coding_changed != NULL)) {
					cdc->cb->line_coding_changed(cdc->line_coding);
				}
			}
			ret_state = HAL_OK;
		} else if (state != HAL_BUSY) {
			cdc->sub_status = CDC_ACM_STATE_CTRL_IDLE;
		}
		usbh_notify_class_state_change(host, 0);
		break;
	default:
		ret_state = HAL_OK;
		break;
	}

	return ret_state;
}

/**
  * @brief  Start to transfer data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbh_composite_cdc_acm_transmit(u8 *buf, u32 len)
{
	int ret = HAL_BUSY;
	usbh_composite_cdc_acm_host_t *cdc = &usbh_composite_cdc_acm_host;
	usb_host_t *host = cdc->driver->host;
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
