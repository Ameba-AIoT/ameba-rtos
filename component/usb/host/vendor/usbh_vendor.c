/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_vendor.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/
#define VENDOR_XFER_MAX_TIMEOUT_TICK    100

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbh_vendor_attach(usb_host_t *host);
static int usbh_vendor_detach(usb_host_t *host);
static int usbh_vendor_process(usb_host_t *host, u32 msg);
static int usbh_vendor_setup(usb_host_t *host);
static int usbh_vendor_sof(usb_host_t *host);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "VEN";

/* USB vendor device identification */
static const usbh_dev_id_t vendor_devs[] = {
	{
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO,
		.bInterfaceClass = VENDOR_CLASS_CODE,
		.bInterfaceSubClass = VENDOR_SUBCLASS_CODE,
		.bInterfaceProtocol = VENDOR_PROTOCOL,
	},
	{
	},
};

/* USB Host vendor class driver */
static usbh_class_driver_t usbh_vendor_driver = {
	.id_table = vendor_devs,
	.attach = usbh_vendor_attach,
	.detach = usbh_vendor_detach,
	.setup = usbh_vendor_setup,
	.process = usbh_vendor_process,
	.sof = usbh_vendor_sof,
};

static usbh_vendor_host_t usbh_vendor_host;

static const char *TEXT_CTRL = "CTRL";
static const char *TEXT_BULK = "BULK";
static const char *TEXT_INTR = "INTR";
static const char *TEXT_ISOC = "ISOC";

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Get transfer type text.
  * @param  pipe: Pointer to Pipe structure.
  * @retval The type text.
  */
static char *usbh_get_transfer_type_text(usbh_pipe_t *pipe)
{
	char *text = NULL;

	switch (pipe->ep_type) {
	case USB_CH_EP_TYPE_CTRL:
		text = (char *)TEXT_CTRL;
		break;
	case USB_CH_EP_TYPE_BULK:
		text = (char *)TEXT_BULK;
		break;
	case USB_CH_EP_TYPE_INTR:
		text = (char *)TEXT_INTR;
		break;
	case USB_CH_EP_TYPE_ISOC:
		text = (char *)TEXT_ISOC;
		break;
	default:
		break;
	}

	return text;
}

/**
  * @brief  Get endpoint details.
  * @param  host: Host handle.
  * @param  intf: Interface descriptor.
  * @retval None.
  */
static void usbh_vendor_get_endpoints(usb_host_t *host, usbh_itf_desc_t *intf)
{
	u32 tmp = 0;
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_ep_desc_t *ep_desc;
	usbh_vendor_xfer_t *xfer;
	u8 ep_type;
	u8 ep_in;
	char *xfer_type;

	for (tmp = 0; tmp < intf->bNumEndpoints; tmp++) {
		ep_desc = &intf->ep_desc_array[tmp];
		ep_type = ep_desc->bmAttributes & USB_EP_XFER_TYPE_MASK;
		ep_in = USB_EP_IS_IN(ep_desc->bEndpointAddress);
		switch (ep_type) {
		case USB_CH_EP_TYPE_BULK:
			if (ep_in) {
				xfer = &vendor->bulk_in_xfer;
			} else {
				xfer = &vendor->bulk_out_xfer;
			}
			usbh_open_pipe(host, &xfer->pipe, ep_desc);
			break;

		case USB_CH_EP_TYPE_INTR:
			if (ep_in) {
				xfer = &vendor->intr_in_xfer;
			} else {
				xfer = &vendor->intr_out_xfer;
			}
			usbh_open_pipe(host, &xfer->pipe, ep_desc);
			break;

		case USB_CH_EP_TYPE_ISOC:
			if (ep_in) {
				xfer = &vendor->isoc_in_xfer;
			} else {
				xfer = &vendor->isoc_out_xfer;
			}
			usbh_open_pipe(host, &xfer->pipe, ep_desc);

			break;

		default:
			break;
		}
		xfer_type = usbh_get_transfer_type_text(&xfer->pipe);
		xfer->pipe.max_timeout_tick = VENDOR_XFER_MAX_TIMEOUT_TICK;
		RTK_LOGS(TAG, RTK_LOG_INFO, "%s EP%02x MPS %d intv %d\n", xfer_type, xfer->pipe.ep_addr, xfer->pipe.ep_mps, xfer->pipe.ep_interval);
	}
}

/**
  * @brief  Dump buffer contents.
  * @param  buf: Data buffer.
  * @param  len: Buffer length.
  * @retval None.
  */
static void usbh_vendor_dump_buf(u8 *buf, u32 len)
{
	u32 i;

	for (i = 0; i < len;) {
		if (i + 10 <= len) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "%3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n", buf[i], buf[i + 1], buf[i + 2], buf[i + 3],
					 buf[i + 4], buf[i + 5], buf[i + 6], buf[i + 7], buf[i + 8], buf[i + 9]);
			i += 10;
		} else {
			for (; i < len; i++) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "0x%x ", buf[i]);
			}
			RTK_LOGS(TAG, RTK_LOG_INFO, "\n");
			break;
		}
	}
}

/**
  * @brief  Deinitialize transfer.
  * @param  host: USB host structure.
  * @param  xfer: Transfer structure.
  * @retval None.
  */
static void usbh_vendor_deinit_transfer(usb_host_t *host, usbh_vendor_xfer_t *xfer)
{
	usbh_pipe_t *pipe = &xfer->pipe;

	if (pipe->pipe_num) {
		usbh_close_pipe(host, pipe);
	}

	if (xfer->test_buf != NULL) {
		usb_os_mfree(xfer->test_buf);
		xfer->test_buf = NULL;
	}

	xfer->xfer_max_len = 0;
	xfer->xfer_cnt = 0;
	xfer->xfer_max_cnt = 0;
	xfer->xfer_bk_buf = NULL;
}

/**
  * @brief  Attach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_vendor_attach(usb_host_t *host)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_itf_data_t *itf_data;
	usbh_dev_id_t dev_id = {0,};

	vendor->ctrl_buf = (u8 *)usb_os_malloc(USB_CTRL_BUF_LENGTH);
	if (vendor->ctrl_buf == NULL) {
		return HAL_ERR_MEM;
	}


	dev_id.bInterfaceClass = VENDOR_CLASS_CODE;
	dev_id.bInterfaceSubClass = VENDOR_SUBCLASS_CODE;
	dev_id.bInterfaceProtocol = VENDOR_PROTOCOL;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	/* Get interface index */
	itf_data = usbh_get_interface_descriptor(host, &dev_id);

	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get itf desc fail\n");

		return HAL_ERR_PARA;
	} else {
		vendor->host = host;
		vendor->state = VENDOR_STATE_XFER;

		/* Get data in/out endpoints */
		usbh_vendor_get_endpoints(host, itf_data->itf_desc_array);

		if ((vendor->cb != NULL) && (vendor->cb->attach != NULL)) {
			vendor->cb->attach();
		}
		return HAL_OK;
	}
}

/**
  * @brief  Deinit all pipe.
  * @param  None.
  * @retval None.
  */
static void usbh_vendor_deinit_all_pipe(void)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usb_host_t *host = vendor->host;
	usbh_pipe_t *pipe;

	pipe = &vendor->bulk_in_xfer.pipe;
	if (pipe->pipe_num) {
		usbh_close_pipe(host, pipe);
	}
	pipe = &vendor->bulk_out_xfer.pipe;
	if (pipe->pipe_num) {
		usbh_close_pipe(host, pipe);
	}
	pipe = &vendor->intr_in_xfer.pipe;
	if (pipe->pipe_num) {
		usbh_close_pipe(host, pipe);
	}
	pipe = &vendor->intr_out_xfer.pipe;
	if (pipe->pipe_num) {
		usbh_close_pipe(host, pipe);
	}
	pipe = &vendor->isoc_in_xfer.pipe;
	if (pipe->pipe_num) {
		usbh_close_pipe(host, pipe);
	}
	pipe = &vendor->isoc_out_xfer.pipe;
	if (pipe->pipe_num) {
		usbh_close_pipe(host, pipe);
	}
}

/**
  * @brief  Deinit all xfer.
  * @param  None.
  * @retval None.
  */
static void usbh_vendor_deinit_all_xfer(void)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usb_host_t *host = vendor->host;

	usbh_vendor_deinit_transfer(host, &vendor->isoc_in_xfer);
	usbh_vendor_deinit_transfer(host, &vendor->isoc_out_xfer);
	usbh_vendor_deinit_transfer(host, &vendor->bulk_in_xfer);
	usbh_vendor_deinit_transfer(host, &vendor->bulk_out_xfer);
	usbh_vendor_deinit_transfer(host, &vendor->intr_in_xfer);
	usbh_vendor_deinit_transfer(host, &vendor->intr_out_xfer);
}

/**
  * @brief  Detach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_vendor_detach(usb_host_t *host)
{
	UNUSED(host);
	usbh_vendor_host_t *vendor = &usbh_vendor_host;

	vendor->state = VENDOR_STATE_IDLE;

	usbh_vendor_deinit_all_pipe();

	if ((vendor->cb != NULL) && (vendor->cb->detach != NULL)) {
		vendor->cb->detach();
	}

	return HAL_OK;
}

/**
  * @brief  Standard control requests handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_vendor_setup(usb_host_t *host)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;

	UNUSED(host);

	if ((vendor->cb != NULL) && (vendor->cb->setup != NULL)) {
		vendor->cb->setup();
	}

	return HAL_OK;
}

/**
  * @brief  Prepare xfer data for next xfer
  * @param  host: Host handle
  * @param  xfer: Pipe xfer
  * @retval None
  */
static void usbh_vendor_next_transfer(usb_host_t *host, usbh_vendor_xfer_t *xfer)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_pipe_t *pipe = &xfer->pipe;

	if (xfer == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "xfer pointer is NULL");
		return;
	}

	if (xfer->xfer_cnt >= xfer->xfer_max_cnt) {
		pipe->xfer_state = USBH_EP_XFER_IDLE;
		vendor->ep_mask &= ~(xfer->test_mask);
		RTK_LOGS(TAG, RTK_LOG_INFO, "ISOC %s test finish %d/%d: \n", (pipe->ep_is_in ? ("IN") : ("OUT")), xfer->xfer_cnt, xfer->xfer_max_cnt);
		usbh_vendor_dump_buf(xfer->test_buf, xfer->xfer_max_cnt);

		if (pipe->ep_is_in == 0) {
			if (vendor->cb != NULL && vendor->cb->transmit != NULL) {
				vendor->cb->transmit(pipe->ep_type);
			}
		} else {
			if (vendor->cb != NULL && vendor->cb->receive != NULL) {
				vendor->cb->receive(pipe->ep_type, NULL, 0, HAL_OK);
			}
		}
	} else {
		pipe->xfer_len = xfer->xfer_max_len;
		pipe->xfer_buf = xfer->xfer_bk_buf;

		if (pipe->ep_is_in == 0) {
			pipe->xfer_state = USBH_EP_XFER_START;
			usb_os_memset((void *)pipe->xfer_buf, (u8)(xfer->xfer_cnt & 0xFF), xfer->xfer_max_len);
		} else {
			pipe->xfer_state = USBH_EP_XFER_START;
			usb_os_memset((void *)pipe->xfer_buf, 0x00, xfer->xfer_max_len);
		}
		usbh_notify_class_state_change(host, pipe->pipe_num);
	}
}

/**
  * @brief  SOF callback function.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_vendor_sof(usb_host_t *host)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_vendor_xfer_t *out_xfer = &vendor->isoc_out_xfer;
	usbh_pipe_t *pipe = &out_xfer->pipe;
	int cur_frame = usbh_get_current_frame_number(host);
	int ret = HAL_OK;

	if (out_xfer->cur_frame == 0) {
		out_xfer->cur_frame = cur_frame;
	}

	if (pipe->xfer_state == USBH_EP_XFER_WAIT_SOF) {
		if ((cur_frame - out_xfer->cur_frame) % pipe->ep_interval == 0) {
			//isoc tx in 2^(ep_interval -1) sof
			out_xfer->xfer_cnt++;
			usbh_vendor_next_transfer(host, out_xfer);
			out_xfer->cur_frame = cur_frame;
		}
	}

	return ret;
}

/**
  * @brief  BULK RX handling
  * @param  host: Host handle
  * @retval None
*/
static void usbh_vendor_bulk_process_rx(usb_host_t *host)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_vendor_xfer_t *in_xfer = &vendor->bulk_in_xfer;
	usbh_pipe_t *pipe = &in_xfer->pipe;
	int status;
	int len;

	if ((in_xfer->test_mask & vendor->ep_mask) == 0) {
		usbh_notify_class_state_change(host, 0);
		return;
	}

	status = usbh_transfer_process(host, pipe);

	if ((status == HAL_OK) && (pipe->xfer_state == USBH_EP_XFER_IDLE)) {
		len = usbh_get_last_transfer_size(host, pipe);
		in_xfer->xfer_cnt++;
		vendor->ep_mask &= ~(in_xfer->test_mask);
		if ((vendor->cb != NULL) && (vendor->cb->receive != NULL)) {
			vendor->cb->receive(pipe->ep_type, pipe->xfer_buf, len, status);
		}
	} else if (pipe->xfer_state == USBH_EP_XFER_START) {
		len = usbh_get_last_transfer_size(host, pipe);
		if ((pipe->xfer_len == 0) && (vendor->cb != NULL) && (vendor->cb->receive != NULL)) {//ZLP
			vendor->cb->receive(pipe->ep_type, pipe->xfer_buf, len, status);
		}
		usbh_notify_class_state_change(host, pipe->pipe_num);
	} else if (pipe->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK RX fail: %d\n", usbh_get_urb_state(host, pipe));
		vendor->ep_mask &= ~(in_xfer->test_mask);
		if ((vendor->cb != NULL) && (vendor->cb->receive != NULL)) {
			vendor->cb->receive(pipe->ep_type, NULL, 0, status);
		}
		usbh_notify_class_state_change(host, pipe->pipe_num);
	}
}

/**
  * @brief  BULK TX handling
  * @param  host: Host handle
  * @retval None
*/
static void usbh_vendor_bulk_process_tx(usb_host_t *host)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_vendor_xfer_t *out_xfer = &vendor->bulk_out_xfer;
	usbh_pipe_t *pipe = &out_xfer->pipe;
	int status;

	if ((out_xfer->test_mask & vendor->ep_mask) == 0) {
		usbh_notify_class_state_change(host, 0);
		return;
	}

	status = usbh_transfer_process(host, pipe);

	if ((status == HAL_OK) && (pipe->xfer_state == USBH_EP_XFER_IDLE)) {
		out_xfer->xfer_cnt++;
		vendor->ep_mask &= ~(out_xfer->test_mask);
		if (vendor->cb != NULL && vendor->cb->transmit != NULL) {
			vendor->cb->transmit(pipe->ep_type);
		}
	} else if (pipe->xfer_state == USBH_EP_XFER_START) {
		usbh_notify_class_state_change(host, pipe->pipe_num);
	} else if (pipe->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK TX fail: %d\n", usbh_get_urb_state(host, pipe));
		vendor->ep_mask &= ~(out_xfer->test_mask);
		if (vendor->cb != NULL && vendor->cb->transmit != NULL) {
			vendor->cb->transmit(pipe->ep_type);
		}
	}
}

/**
  * @brief  INTR RX handling
  * @param  host: Host handle
  * @retval None
*/
static void usbh_vendor_intr_process_rx(usb_host_t *host)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_vendor_xfer_t *in_xfer = &vendor->intr_in_xfer;
	usbh_pipe_t *pipe = &in_xfer->pipe;
	int status;
	int len;

	if ((in_xfer->test_mask & vendor->ep_mask) == 0) {
		usbh_notify_class_state_change(host, 0);
		return;
	}

	status = usbh_transfer_process(host, pipe);

	if ((status == HAL_OK) && (pipe->xfer_state == USBH_EP_XFER_IDLE)) {
		len = usbh_get_last_transfer_size(host, pipe);
		in_xfer->xfer_cnt++;
		vendor->ep_mask &= ~(in_xfer->test_mask);
		if ((vendor->cb != NULL) && (vendor->cb->receive != NULL)) {
			vendor->cb->receive(pipe->ep_type, pipe->xfer_buf, len, status);
		}
	} else if ((pipe->xfer_state == USBH_EP_XFER_START)) {
		len = usbh_get_last_transfer_size(host, pipe);
		if ((pipe->xfer_len == 0) && (vendor->cb != NULL) && (vendor->cb->receive != NULL)) {//ZLP
			vendor->cb->receive(pipe->ep_type, pipe->xfer_buf, len, status);
		}
		usbh_notify_class_state_change(host, pipe->pipe_num);
	} else if (pipe->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR RX fail: %d\n", usbh_get_urb_state(host, pipe));
		vendor->ep_mask &= ~(in_xfer->test_mask);
		if ((vendor->cb != NULL) && (vendor->cb->receive != NULL)) {
			vendor->cb->receive(pipe->ep_type, NULL, 0, status);
		}
	}
}

/**
  * @brief  INTR TX handling
  * @param  host: Host handle
  * @retval None
*/
static void usbh_vendor_intr_process_tx(usb_host_t *host)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_vendor_xfer_t *out_xfer = &vendor->intr_out_xfer;
	usbh_pipe_t *pipe = &out_xfer->pipe;
	int status;

	if ((out_xfer->test_mask & vendor->ep_mask) == 0) {
		usbh_notify_class_state_change(host, 0);
		return;
	}

	status = usbh_transfer_process(host, pipe);

	if ((status == HAL_OK) && (pipe->xfer_state == USBH_EP_XFER_IDLE)) {
		out_xfer->xfer_cnt++;
		vendor->ep_mask &= ~(out_xfer->test_mask);
		if (vendor->cb != NULL && vendor->cb->transmit != NULL) {
			vendor->cb->transmit(pipe->ep_type);
		}
	} else if (pipe->xfer_state == USBH_EP_XFER_START) {
		usbh_notify_class_state_change(host, pipe->pipe_num);
	} else if (pipe->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR TX fail: %d\n", usbh_get_urb_state(host, pipe));
		vendor->ep_mask &= ~(out_xfer->test_mask);
		if (vendor->cb != NULL && vendor->cb->transmit != NULL) {
			vendor->cb->transmit(pipe->ep_type);
		}
	}
}

/**
  * @brief  ISOC RX handling
  * @param  host: Host handle
  * @retval None
*/
static void usbh_vendor_isoc_process_rx(usb_host_t *host)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_vendor_xfer_t *in_xfer = &vendor->isoc_in_xfer;
	usbh_pipe_t *pipe = &in_xfer->pipe;
	int status;

	if ((in_xfer->test_mask & vendor->ep_mask) == 0) {
		usbh_notify_class_state_change(host, 0);
		return;
	}

	status = usbh_transfer_process(host, pipe);

	if ((status == HAL_OK) && (pipe->xfer_state == USBH_EP_XFER_IDLE)) {
		in_xfer->test_buf[in_xfer->xfer_cnt] = pipe->xfer_buf[0];
		in_xfer->xfer_cnt ++;
		vendor->ep_mask &= ~(in_xfer->test_mask);
		usbh_vendor_next_transfer(host, in_xfer);
	} else if (pipe->xfer_state == USBH_EP_XFER_START) {
		usbh_notify_class_state_change(host, pipe->pipe_num);
	} else if (pipe->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ISOC RX fail: %d\n", usbh_get_urb_state(host, pipe));
		in_xfer->test_buf[in_xfer->xfer_cnt] = 0xFF;
		in_xfer->xfer_cnt++;
		vendor->ep_mask &= ~(in_xfer->test_mask);
		usbh_vendor_next_transfer(host, in_xfer);
	}
}

/**
  * @brief  ISOC TX handling
  * @param  host: Host handle
  * @retval None
*/
static void usbh_vendor_isoc_process_tx(usb_host_t *host)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_vendor_xfer_t *out_xfer = &vendor->isoc_out_xfer;
	usbh_pipe_t *pipe = &out_xfer->pipe;
	int status;

	if ((out_xfer->test_mask & vendor->ep_mask) == 0) {
		usbh_notify_class_state_change(host, 0);
		return;
	}

	if (pipe->xfer_state == USBH_EP_XFER_START) {
		if (host->dev_speed != USB_SPEED_HIGH) {
			pipe->xfer_len = (pipe->xfer_len >= pipe->ep_mps) ? pipe->ep_mps : pipe->xfer_len;
		}
		out_xfer->test_buf[out_xfer->xfer_cnt] = pipe->xfer_buf[0];
	}

	status = usbh_transfer_process(host, pipe);

	if ((status == HAL_OK) && (pipe->xfer_state == USBH_EP_XFER_IDLE)) {
		if (host->dev_speed == USB_SPEED_HIGH) {
			if (out_xfer->xfer_cnt < out_xfer->xfer_max_cnt) {
				vendor->ep_mask &= ~(out_xfer->test_mask);
				pipe->xfer_state = USBH_EP_XFER_WAIT_SOF;
			} else {
				usbh_vendor_next_transfer(host, out_xfer);
			}
		} else {
			vendor->ep_mask &= ~(out_xfer->test_mask);
			pipe->xfer_state = USBH_EP_XFER_WAIT_SOF;
		}
	} else if (pipe->xfer_state == USBH_EP_XFER_START) {
		usbh_notify_class_state_change(host, pipe->pipe_num);
	} else if (pipe->xfer_state == USBH_EP_XFER_ERROR) {
		vendor->ep_mask &= ~(out_xfer->test_mask);
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ISOC TX fail: %d\n", usbh_get_urb_state(host, pipe));
	}
}

/**
  * @brief  State machine handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_vendor_process(usb_host_t *host, u32 msg)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_event_t event;
	event.d32 = msg;

	switch (vendor->state) {

	case VENDOR_STATE_IDLE:
		break;

	case VENDOR_STATE_XFER:
		if (event.msg.pipe_num == vendor->bulk_in_xfer.pipe.pipe_num) {
			usbh_vendor_bulk_process_rx(host);
		} else if (event.msg.pipe_num == vendor->bulk_out_xfer.pipe.pipe_num) {
			usbh_vendor_bulk_process_tx(host);
		} else if (event.msg.pipe_num == vendor->intr_in_xfer.pipe.pipe_num) {
			usbh_vendor_intr_process_rx(host);
		} else if (event.msg.pipe_num == vendor->intr_out_xfer.pipe.pipe_num) {
			usbh_vendor_intr_process_tx(host);
		} else if (event.msg.pipe_num == vendor->isoc_in_xfer.pipe.pipe_num) {
			usbh_vendor_isoc_process_rx(host);
		} else if (event.msg.pipe_num == vendor->isoc_out_xfer.pipe.pipe_num) {
			usbh_vendor_isoc_process_tx(host);
		}
		vendor->ep_mask = USBH_VENDOR_MASK_ALL_EPS;
		vendor->isoc_in_xfer.test_mask  = USBH_VENDOR_MASK_ISOC_IN;
		vendor->isoc_out_xfer.test_mask = USBH_VENDOR_MASK_ISOC_OUT;
		break;

	case VENDOR_STATE_ERROR:
		if (usbh_ctrl_clear_feature(host, 0x00U) == HAL_OK) {
			vendor->state = VENDOR_STATE_IDLE;
		}
		break;

	default:
		break;

	}

	return HAL_OK;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init vendor class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_vendor_init(usbh_vendor_cb_t *cb)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;

	if (cb != NULL) {
		vendor->cb = cb;
	}

	usbh_register_class(&usbh_vendor_driver);

	return HAL_OK;
}

/**
  * @brief  Deinit vendor class
  * @retval Status
  */
int usbh_vendor_deinit(void)
{
	int ret = HAL_OK;
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usb_host_t *host = vendor->host;
	UNUSED(host);

	vendor->state = VENDOR_STATE_IDLE;

	usbh_vendor_deinit_all_pipe();
	usbh_vendor_deinit_all_xfer();

	if (vendor->ctrl_buf != NULL) {
		usb_os_mfree(vendor->ctrl_buf);
		vendor->ctrl_buf = NULL;
	}

	usbh_unregister_class(&usbh_vendor_driver);

	return ret;
}

/**
  * @brief  Start to tramsmit data
  * @param  xfer: Pipe xfer
  * @param  buf: Data buffer
  * @param  len: Data length
  * @param  test_cnt: Test loop count
  * @retval Status
  */
static int usbh_vendor_tramsmit(usbh_vendor_xfer_t *xfer, u8 *buf, u32 len, u32 test_cnt)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usb_host_t *host = vendor->host;
	usbh_pipe_t *pipe = &xfer->pipe;
	int ret = HAL_BUSY;

	if ((pipe->xfer_state == USBH_EP_XFER_IDLE)
		&& ((vendor->state == VENDOR_STATE_IDLE) || (vendor->state == VENDOR_STATE_XFER))) {

		if (pipe->ep_type == USB_CH_EP_TYPE_ISOC) {
			xfer->test_buf = (u8 *)usb_os_malloc(test_cnt);
			if (xfer->test_buf == NULL) {
				return HAL_BUSY;
			}
		} else if (pipe->ep_type == USB_CH_EP_TYPE_INTR) {
			xfer->test_mask = USBH_VENDOR_MASK_INTR_OUT;
		} else if (pipe->ep_type == USB_CH_EP_TYPE_BULK) {
			xfer->test_mask = USBH_VENDOR_MASK_BULK_OUT;
		}

		pipe->xfer_buf = buf;
		pipe->xfer_len = len;
		pipe->xfer_state = USBH_EP_XFER_START;
		xfer->xfer_max_len = len;
		xfer->xfer_bk_buf = buf;
		xfer->xfer_max_cnt = test_cnt;
		vendor->state = VENDOR_STATE_XFER;

		if ((pipe->xfer_len > 0) && ((pipe->xfer_len % pipe->ep_mps) == 0)
			&& (pipe->ep_type == USB_CH_EP_TYPE_BULK)) { //ZLP
			pipe->trx_zlp = 1;
		} else {
			pipe->trx_zlp = 0;
		}
		usbh_notify_class_state_change(host, pipe->pipe_num);
		ret = HAL_OK;
	}

	return ret;
}

/**
  * @brief  Start to receive data
  * @param  xfer: Pipe xfer
  * @param  buf: Data buffer
  * @param  len: Data length
  * @param  test_cnt: Test loop count
  * @retval Status
  */
static int usbh_vendor_receive(usbh_vendor_xfer_t *xfer, u8 *buf, u32 len, u32 test_cnt)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usb_host_t *host = vendor->host;
	usbh_pipe_t *pipe = &xfer->pipe;
	int ret = HAL_BUSY;

	if ((vendor->state == VENDOR_STATE_IDLE) || (vendor->state == VENDOR_STATE_XFER)) {
		if (pipe->xfer_state == USBH_EP_XFER_IDLE) {
			if (pipe->ep_type == USB_CH_EP_TYPE_ISOC) {
				xfer->test_buf = (u8 *)usb_os_malloc(test_cnt);
				if (xfer->test_buf == NULL) {
					return HAL_BUSY;
				}
			} else if (pipe->ep_type == USB_CH_EP_TYPE_INTR) {
				xfer->test_mask = USBH_VENDOR_MASK_INTR_IN;
			} else if (pipe->ep_type == USB_CH_EP_TYPE_BULK) {
				xfer->test_mask = USBH_VENDOR_MASK_BULK_IN;
			}

			pipe->xfer_buf = buf;
			pipe->xfer_len = len;
			pipe->xfer_state = USBH_EP_XFER_START;
			xfer->xfer_bk_buf = buf;
			xfer->xfer_max_len = len;
			xfer->xfer_max_cnt = test_cnt;
			vendor->state = VENDOR_STATE_XFER;

			if ((pipe->xfer_len > 0) && ((pipe->xfer_len % pipe->ep_mps) == 0)
				&& (pipe->ep_type == USB_CH_EP_TYPE_BULK)) { //ZLP
				pipe->trx_zlp = 1;
			} else {
				pipe->trx_zlp = 0;
			}
			usbh_notify_class_state_change(host, pipe->pipe_num);
			ret = HAL_OK;
		}
	}
	return ret;
}
/**
  * @brief  Start to transfer bulk data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @param  test_cnt: Test count
  * @retval Status
  */
int usbh_vendor_bulk_transmit(u8 *buf, u32 len, u32 test_cnt)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	return usbh_vendor_tramsmit(&vendor->bulk_out_xfer, buf, len, test_cnt);
}

/**
  * @brief  Start to receive bulk data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @param  test_cnt: Test count
  * @retval Status
  */
int usbh_vendor_bulk_receive(u8 *buf, u32 len, u32 test_cnt)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	return usbh_vendor_receive(&vendor->bulk_in_xfer, buf, len, test_cnt);
}

/**
  * @brief  Start to transfer intr data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @param  test_cnt: Test count
  * @retval Status
  */
int usbh_vendor_intr_transmit(u8 *buf, u32 len, u32 test_cnt)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	return usbh_vendor_tramsmit(&vendor->intr_out_xfer, buf, len, test_cnt);
}

/**
  * @brief  Start to receive intr data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @param  test_cnt: Test count
  * @retval Status
  */
int usbh_vendor_intr_receive(u8 *buf, u32 len, u32 test_cnt)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	return usbh_vendor_receive(&vendor->intr_in_xfer, buf, len, test_cnt);
}

/**
  * @brief  Start to transfer ISOC data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @param  test_cnt: Test count
  * @retval Status
  */
int usbh_vendor_isoc_transmit(u8 *buf, u32 len, u32 test_cnt)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	return usbh_vendor_tramsmit(&vendor->isoc_out_xfer, buf, len, test_cnt);
}

/**
  * @brief  Start to receive ISOC data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @param  test_cnt: Test count
  * @retval Status
  */
int usbh_vendor_isoc_receive(u8 *buf, u32 len, u32 test_cnt)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	return usbh_vendor_receive(&vendor->isoc_in_xfer, buf, len, test_cnt);
}

/**
  * @brief  Get the BULK IN max packet size
  * @retval Max packet size
  */
u16 usbh_vendor_get_bulk_ep_mps(void)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_vendor_xfer_t *xfer = &vendor->bulk_in_xfer;
	return xfer->pipe.ep_mps;
}

/**
  * @brief  Get the INTR IN max packet size
  * @retval Max packet size
  */
u16 usbh_vendor_get_intr_ep_mps(void)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_vendor_xfer_t *xfer = &vendor->intr_in_xfer;
	return xfer->pipe.ep_mps;
}