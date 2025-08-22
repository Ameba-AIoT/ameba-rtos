/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_vendor.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

typedef int(* usbh_xfer_func)(usb_host_t *host, u8 *buf, u16 len, u8 pipe_num);

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbh_vendor_attach(usb_host_t *host);
static int usbh_vendor_detach(usb_host_t *host);
static int usbh_vendor_process(usb_host_t *host, u32 msg);
static int usbh_vendor_setup(usb_host_t *host);
static int usbh_vendor_sof(usb_host_t *host);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "VEN";

/* USB Standard Device Descriptor */
static usbh_class_driver_t usbh_vendor_driver = {
	.class_code = VENDOR_CLASS_CODE,
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
  * @param  xfer: Transfer structure.
  * @retval The type text.
  */
static char *usbh_get_xfer_type_text(usbh_vendor_xfer_t *xfer)
{
	char *text = NULL;

	switch (xfer->ep_type) {
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
  * @brief  Get transfer function.
  * @param  xfer: Transfer structure.
  * @retval Transfer function.
  */
static usbh_xfer_func usbh_get_xfer_func(usbh_vendor_xfer_t *xfer)
{
	usbh_xfer_func func = NULL;
	u8 is_in_xfer = USB_EP_IS_IN(xfer->ep_num);

	if (is_in_xfer) {
		switch (xfer->ep_type) {
		case USB_CH_EP_TYPE_BULK:
			func = usbh_bulk_receive_data;
			break;
		case USB_CH_EP_TYPE_INTR:
			func = usbh_intr_receive_data;
			break;
		case USB_CH_EP_TYPE_ISOC:
			func = usbh_isoc_receive_data;
			break;
		default:
			break;
		}
	} else {
		switch (xfer->ep_type) {
		case USB_CH_EP_TYPE_BULK:
			func = usbh_bulk_send_data;
			break;
		case USB_CH_EP_TYPE_INTR:
			func = usbh_intr_send_data;
			break;
		case USB_CH_EP_TYPE_ISOC:
			func = usbh_isoc_send_data;
			break;
		default:
			break;
		}
	}

	return func;
}

/**
  * @brief  Initialize endpoint.
  * @param  xfer: Transfer structure.
  * @param  ep_desc: Endpoint descriptor.
  * @retval Status.
  */
static int usbh_vendor_init_ep(usbh_vendor_xfer_t *xfer, usbh_ep_desc_t *ep_desc)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	char *xfer_type = usbh_get_xfer_type_text(xfer);

	xfer->ep_num = ep_desc->bEndpointAddress;
	xfer->ep_mps = ep_desc->wMaxPacketSize;
	xfer->ep_type = ep_desc->bmAttributes & USB_EP_XFER_TYPE_MASK;
	xfer->ep_interval = usbh_get_interval(vendor->host, xfer->ep_type, ep_desc->bInterval);

	RTK_LOGS(TAG, RTK_LOG_INFO, "%s EP%02x MPS %d intv %d %d\n", xfer_type, xfer->ep_num, xfer->ep_mps, ep_desc->bInterval, xfer->ep_interval);

	return HAL_OK;
}

/**
  * @brief  Get endpoint details.
  * @param  intf: Interface descriptor.
  * @retval None.
  */
static void usbh_vendor_get_endpoints(usbh_if_desc_t *intf)
{
	u32 tmp = 0;
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_ep_desc_t *ep_desc;
	usbh_vendor_xfer_t *xfer;
	u8 ep_type;
	u8 ep_in;

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
			usbh_vendor_init_ep(xfer, ep_desc);
			break;

		case USB_CH_EP_TYPE_INTR:
			if (ep_in) {
				xfer = &vendor->intr_in_xfer;
			} else {
				xfer = &vendor->intr_out_xfer;
			}
			usbh_vendor_init_ep(xfer, ep_desc);
			break;

		case USB_CH_EP_TYPE_ISOC:
			if (ep_in) {
				xfer = &vendor->isoc_in_xfer;
			} else {
				xfer = &vendor->isoc_out_xfer;
			}
			usbh_vendor_init_ep(xfer, ep_desc);

			break;

		default:
			break;
		}
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
  * @brief  Initialize transfer.
  * @param  host: USB host structure.
  * @param  xfer: Transfer structure.
  * @retval Status.
  */
static int usbh_vendor_init_xfer(usb_host_t *host, usbh_vendor_xfer_t *xfer)
{
	int status = HAL_OK;
	u8 pipe_num;
	char *xfer_type = usbh_get_xfer_type_text(xfer);

	pipe_num = usbh_alloc_pipe(host, xfer->ep_num);
	if (pipe_num != 0xFFU) {
		xfer->pipe_num = pipe_num;
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to alloc pipe for %s EP%02x\n", xfer_type, xfer->ep_num);
		status = HAL_ERR_MEM;
		goto exit;
	}

	//RTK_LOGS(TAG, RTK_LOG_DEBUG, "Alloc pipe %d for %s EP%02x\n", pipe_num, xfer_type, xfer->ep_num);

	xfer->xfer_buf = NULL;
	xfer->xfer_bk_buf = NULL;
	xfer->test_buf = NULL;
	xfer->xfer_len = 0;
	xfer->xfer_max_len = 0;
	xfer->xfer_cnt = 0;
	xfer->tick = 0;
	xfer->state = VENDOR_STATE_IDLE;
	xfer->retry_time = 0;
	xfer->ep_state = USBH_VENDOR_IDLE;

	status = usbh_open_pipe(host, xfer->pipe_num, xfer->ep_num, xfer->ep_type, xfer->ep_mps);
	if (status != HAL_OK) {
		status = HAL_ERR_HW;
		goto open_pipe_fail_exit;
	}

	return status;

open_pipe_fail_exit:
	usbh_free_pipe(host, xfer->pipe_num);
	xfer->pipe_num = 0;
exit:
	return status;
}

/**
  * @brief  Deinitialize transfer.
  * @param  host: USB host structure.
  * @param  xfer: Transfer structure.
  * @retval None.
  */
static void usbh_vendor_deinit_xfer(usb_host_t *host, usbh_vendor_xfer_t *xfer)
{
	if (xfer->pipe_num) {
		usbh_close_pipe(host, xfer->pipe_num);
		usbh_free_pipe(host, xfer->pipe_num);
		xfer->pipe_num = 0U;
	}

	if (xfer->test_buf != NULL) {
		usb_os_mfree(xfer->test_buf);
		xfer->test_buf = NULL;
	}

	xfer->xfer_len = 0;
	xfer->xfer_max_len = 0;
	xfer->xfer_cnt = 0;
	xfer->xfer_max_cnt = 0;
	xfer->xfer_buf = NULL;
	xfer->xfer_bk_buf = NULL;
	xfer->ep_state = USBH_VENDOR_IDLE;
}

/**
  * @brief  Set USB interface.
  * @param  host: Host handle.
  * @param  if_num: Interface number.
  * @param  if_alt: Alternative setting.
  * @retval Status.
  */
static int usbh_vendor_set_interface(usb_host_t *host, u8 if_num, u8 if_alt)
{
	int ret;
	do {
		ret = usbh_ctrl_set_interface(host, if_num, if_alt);
		if (ret == HAL_OK) {
			return HAL_OK;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set CTRL itf fail %d\n", ret);
			return ret;
		}
	} while (1);

	return HAL_OK;
}

/**
  * @brief  Attach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_vendor_attach(usb_host_t *host)
{
	int status = HAL_OK;
	u8 interface;
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_if_desc_t *vendor_if_desc;

	/* Get interface index */
	interface = usbh_get_interface(host, VENDOR_CLASS_CODE, VENDOR_SUBCLASS_CODE, VENDOR_PROTOCOL);
	if (interface == 0xFFU) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get itf fail\n");
		return HAL_ERR_PARA;
	}

	vendor->host = host;
	vendor->state = VENDOR_STATE_XFER;

	usbh_set_interface(host, interface);

	vendor->interface_id = 0U;

	status = usbh_vendor_set_interface(host, interface, vendor->interface_id);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Set itf fail\n");
		return HAL_ERR_UNKNOWN;
	}
	vendor_if_desc = usbh_get_interface_descriptor(host, interface, vendor->interface_id);
	if (vendor_if_desc == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get itf desc fail\n");
		return HAL_ERR_PARA;
	}

	/* Get data in/out endpoints */
	usbh_vendor_get_endpoints(vendor_if_desc);

	vendor->ctrl_buf = (u8 *)usb_os_malloc(USB_CTRL_BUF_LENGTH);
	if (vendor->ctrl_buf == NULL) {
		status = HAL_ERR_MEM;
		goto final_exit;
	}

	status = usbh_vendor_init_xfer(host, &vendor->bulk_in_xfer);
	if (status != HAL_OK) {
		goto bulk_in_fail_exit;
	}

	status = usbh_vendor_init_xfer(host, &vendor->bulk_out_xfer);
	if (status != HAL_OK) {
		goto bulk_out_fail_exit;
	}

	status = usbh_vendor_init_xfer(host, &vendor->intr_in_xfer);
	if (status != HAL_OK) {
		goto intr_in_fail_exit;
	}

	status = usbh_vendor_init_xfer(host, &vendor->intr_out_xfer);
	if (status != HAL_OK) {
		goto intr_out_fail_exit;
	}

	status = usbh_vendor_init_xfer(host, &vendor->isoc_in_xfer);
	if (status != HAL_OK) {
		goto isoc_in_fail_exit;
	}

	status = usbh_vendor_init_xfer(host, &vendor->isoc_out_xfer);
	if (status != HAL_OK) {
		goto isoc_out_fail_exit;
	}

	if ((vendor->cb != NULL) && (vendor->cb->attach != NULL)) {
		vendor->cb->attach();
	}

	goto final_exit;

isoc_out_fail_exit:
	usbh_vendor_deinit_xfer(host, &vendor->isoc_in_xfer);
isoc_in_fail_exit:
	usbh_vendor_deinit_xfer(host, &vendor->intr_out_xfer);
intr_out_fail_exit:
	usbh_vendor_deinit_xfer(host, &vendor->intr_in_xfer);
intr_in_fail_exit:
	usbh_vendor_deinit_xfer(host, &vendor->bulk_out_xfer);
bulk_out_fail_exit:
	usbh_vendor_deinit_xfer(host, &vendor->bulk_in_xfer);
bulk_in_fail_exit:
	if (vendor->ctrl_buf != NULL) {
		usb_os_mfree(vendor->ctrl_buf);
		vendor->ctrl_buf = NULL;
	}
final_exit:
	return status;
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

	if (vendor->bulk_in_xfer.pipe_num) {
		usbh_close_pipe(host, vendor->bulk_in_xfer.pipe_num);
		usbh_free_pipe(host, vendor->bulk_in_xfer.pipe_num);
		vendor->bulk_in_xfer.pipe_num = 0U;
	}

	if (vendor->bulk_out_xfer.pipe_num) {
		usbh_close_pipe(host, vendor->bulk_out_xfer.pipe_num);
		usbh_free_pipe(host, vendor->bulk_out_xfer.pipe_num);
		vendor->bulk_out_xfer.pipe_num = 0U;
	}

	if (vendor->intr_in_xfer.pipe_num) {
		usbh_close_pipe(host, vendor->intr_in_xfer.pipe_num);
		usbh_free_pipe(host, vendor->intr_in_xfer.pipe_num);
		vendor->intr_in_xfer.pipe_num = 0U;
	}

	if (vendor->intr_out_xfer.pipe_num) {
		usbh_close_pipe(host, vendor->intr_out_xfer.pipe_num);
		usbh_free_pipe(host, vendor->intr_out_xfer.pipe_num);
		vendor->intr_out_xfer.pipe_num = 0U;
	}

	if (vendor->isoc_in_xfer.pipe_num) {
		usbh_close_pipe(host, vendor->isoc_in_xfer.pipe_num);
		usbh_free_pipe(host, vendor->isoc_in_xfer.pipe_num);
		vendor->isoc_in_xfer.pipe_num = 0U;
	}

	if (vendor->isoc_out_xfer.pipe_num) {
		usbh_close_pipe(host, vendor->isoc_out_xfer.pipe_num);
		usbh_free_pipe(host, vendor->isoc_out_xfer.pipe_num);
		vendor->isoc_out_xfer.pipe_num = 0U;
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

	usbh_vendor_deinit_xfer(host, &vendor->isoc_in_xfer);
	usbh_vendor_deinit_xfer(host, &vendor->isoc_out_xfer);
	usbh_vendor_deinit_xfer(host, &vendor->bulk_in_xfer);
	usbh_vendor_deinit_xfer(host, &vendor->bulk_out_xfer);
	usbh_vendor_deinit_xfer(host, &vendor->intr_in_xfer);
	usbh_vendor_deinit_xfer(host, &vendor->intr_out_xfer);
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
  * @retval Void
  */
static void usbh_vendor_next_xfer(usbh_vendor_xfer_t *xfer)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	u8 is_in = USB_EP_IS_IN(xfer->ep_num);
	if (xfer == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "xfer pointer is NULL");
		return;
	}

	if (xfer->xfer_cnt >= xfer->xfer_max_cnt) {
		xfer->state = VENDOR_STATE_IDLE;
		xfer->ep_state = USBH_VENDOR_IDLE;
		RTK_LOGS(TAG, RTK_LOG_INFO, "ISOC %s test finish %d/%d: \n", (is_in ? ("IN") : ("OUT")), xfer->xfer_cnt, xfer->xfer_max_cnt);
		usbh_vendor_dump_buf(xfer->test_buf, xfer->xfer_max_cnt);

		if (is_in == 0) {
			if (vendor->cb != NULL && vendor->cb->transmit != NULL) {
				vendor->cb->transmit(xfer->ep_type);
			}
		} else {
			if (vendor->cb != NULL && vendor->cb->receive != NULL) {
				vendor->cb->receive(xfer->ep_type, NULL, 0);
			}
		}
	} else {
		xfer->xfer_len = xfer->xfer_max_len;
		xfer->xfer_buf = xfer->xfer_bk_buf;
		xfer->state = VENDOR_STATE_XFER;

		if (is_in == 0) {
			xfer->ep_state = USBH_VENDOR_TX;
			usb_os_memset((void *)xfer->xfer_buf, (u8)(xfer->xfer_cnt & 0xFF), xfer->xfer_max_len);
		} else {
			xfer->ep_state = USBH_VENDOR_RX;
			usb_os_memset((void *)xfer->xfer_buf, 0x00, xfer->xfer_max_len);
		}
	}
}

/**
  * @brief  Usb sof callback function.
  * @param  host: Host handle
  * @retval return status
  */
static int usbh_vendor_sof(usb_host_t *host)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_vendor_xfer_t *out_xfer = &vendor->isoc_out_xfer;
	int cur_frame = usbh_get_current_frame(host);
	int ret = HAL_OK;

	if (out_xfer->cur_frame == 0) {
		out_xfer->cur_frame = cur_frame;
	}

	if (out_xfer->ep_state == USBH_VENDOR_TX_WAIT_SOF) {
		if ((cur_frame - out_xfer->cur_frame) % out_xfer->ep_interval == 0) {
			//isoc tx in 2^(ep_interval -1) sof
			out_xfer->xfer_cnt++;
			usbh_vendor_next_xfer(out_xfer);
			usbh_notify_class_state_change(host, 0);
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
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	usbh_xfer_func xfer_func = NULL;
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_vendor_xfer_t *in_xfer = &vendor->bulk_in_xfer;
	u32 rx_len;
	int status;

	if ((in_xfer->test_mask & vendor->ep_mask) == 0) {
		usbh_notify_class_state_change(host, 0);
		return;
	}

	xfer_func = usbh_get_xfer_func(in_xfer);

	switch (in_xfer->ep_state) {
	case USBH_VENDOR_RX:
		status = xfer_func(host, in_xfer->xfer_buf, in_xfer->xfer_len, in_xfer->pipe_num);
		in_xfer->tick = usbh_get_tick(host);
		if (status == HAL_OK) {
			in_xfer->ep_state = USBH_VENDOR_RX_BUSY;
		}
		break;

	case USBH_VENDOR_RX_BUSY:
		urb_state = usbh_get_urb_state(host, in_xfer->pipe_num);
		if (urb_state == USBH_URB_DONE) {
			rx_len = usbh_get_last_transfer_size(host, in_xfer->pipe_num);

			if ((vendor->cb != NULL) && (vendor->cb->receive != NULL)) {
				vendor->cb->receive(in_xfer->ep_type, in_xfer->xfer_buf, rx_len);
			}

			if ((rx_len > 0) && ((rx_len % in_xfer->ep_mps) == 0)) { //N*MPS
				if (in_xfer->xfer_len > rx_len) {
					in_xfer->xfer_cnt++;
					in_xfer->ep_state = USBH_VENDOR_IDLE;//Premature ZLP
					vendor->ep_mask &= ~(in_xfer->test_mask);
				} else if ((in_xfer->xfer_len == rx_len)) {
					in_xfer->xfer_len = 0;/* Last ZLP for multi-MPS */
					in_xfer->ep_state = USBH_VENDOR_RX;
				} else {
					in_xfer->ep_state = USBH_VENDOR_IDLE;
					vendor->ep_mask &= ~(in_xfer->test_mask);
				}
			} else {//ZLP or short
				in_xfer->xfer_cnt++;
				in_xfer->ep_state = USBH_VENDOR_IDLE;
				vendor->ep_mask &= ~(in_xfer->test_mask);
			}

			if (in_xfer->xfer_cnt >= in_xfer->xfer_max_cnt) {
				in_xfer->state = VENDOR_STATE_IDLE;
				in_xfer->ep_state = USBH_VENDOR_IDLE;
			}

			usbh_notify_class_state_change(host, 0);
		} else if ((urb_state == USBH_URB_BUSY) || (urb_state == USBH_URB_IDLE)) {
			if (usbh_get_elapsed_ticks(host, in_xfer->tick) > 100) {
				in_xfer->retry_time++;
				if (in_xfer->retry_time < 5000) {
					in_xfer->ep_state = USBH_VENDOR_RX;
				} else {
					in_xfer->retry_time = 0;
					in_xfer->ep_state = USBH_VENDOR_IDLE;
					RTK_LOGS(TAG, RTK_LOG_INFO, "bulk rx%d timeout\n", in_xfer->xfer_cnt);
				}
			}

			usbh_notify_class_state_change(host, 0);
		} else if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
			in_xfer->state = VENDOR_STATE_IDLE;
			RTK_LOGS(TAG, RTK_LOG_INFO, "bulk rx err%d\n", in_xfer->xfer_cnt);
			usbh_notify_class_state_change(host, 0);
		}
		break;

	default:
		break;
	}
}


/**
  * @brief  BULK TX handling
  * @param  host: Host handle
  * @retval None
*/
static void usbh_vendor_bulk_process_tx(usb_host_t *host)
{
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_vendor_xfer_t *out_xfer = &vendor->bulk_out_xfer;
	usbh_xfer_func xfer_func = usbh_get_xfer_func(out_xfer);

	if ((out_xfer->test_mask & vendor->ep_mask) == 0) {
		usbh_notify_class_state_change(host, out_xfer->pipe_num);
		return;
	}

	switch (out_xfer->ep_state) {
	case USBH_VENDOR_TX:
		xfer_func(host, out_xfer->xfer_buf, out_xfer->xfer_len, out_xfer->pipe_num);
		out_xfer->tick = usbh_get_tick(host);
		out_xfer->ep_state = USBH_VENDOR_TX_BUSY;
		break;

	case USBH_VENDOR_TX_BUSY:
		urb_state = usbh_get_urb_state(host, out_xfer->pipe_num);
		if (urb_state == USBH_URB_DONE) {
			out_xfer->retry_time = 0;
			if (out_xfer->xfer_len >= out_xfer->ep_mps) {
				out_xfer->xfer_len -= out_xfer->ep_mps;
				out_xfer->xfer_buf += out_xfer->ep_mps;
				out_xfer->ep_state = USBH_VENDOR_TX;
			} else {
				// Complete the transfer including ZLP
				out_xfer->xfer_cnt++;
				out_xfer->xfer_len = 0U;
				out_xfer->ep_state = USBH_VENDOR_IDLE;
				vendor->ep_mask &= ~(out_xfer->test_mask);
				if (vendor->cb != NULL && vendor->cb->transmit != NULL) {
					vendor->cb->transmit(out_xfer->ep_type);
				}
			}
			usbh_notify_class_state_change(host, out_xfer->pipe_num);
		} else if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
			out_xfer->ep_state = USBH_VENDOR_IDLE;
			usbh_notify_class_state_change(host, out_xfer->pipe_num);
			RTK_LOGS(TAG, RTK_LOG_INFO, "bulk tx%d err\n", out_xfer->xfer_cnt);
		} else if ((urb_state == USBH_URB_BUSY) || (urb_state == USBH_URB_IDLE)) {
			if (usbh_get_elapsed_ticks(host, out_xfer->tick) >= 8000) {
				out_xfer->retry_time++;
				if (out_xfer->retry_time < 50) {
					out_xfer->ep_state = USBH_VENDOR_TX;
				} else {
					out_xfer->retry_time = 0;;
					out_xfer->ep_state = USBH_VENDOR_IDLE;
					RTK_LOGS(TAG, RTK_LOG_INFO, "bulk tx%d timeout\n", out_xfer->xfer_cnt);
				}
			}
			usbh_notify_class_state_change(host, out_xfer->pipe_num);
		}
		break;

	default:
		break;
	}
}

/**
  * @brief  INTR RX handling
  * @param  host: Host handle
  * @retval None
*/
static void usbh_vendor_intr_process_rx(usb_host_t *host)
{
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	usbh_xfer_func xfer_func = NULL;
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_vendor_xfer_t *in_xfer = &vendor->intr_in_xfer;
	u32 rx_len;
	int status;

	if ((in_xfer->test_mask & vendor->ep_mask) == 0) {
		usbh_notify_class_state_change(host, in_xfer->pipe_num);
		return;
	}

	xfer_func = usbh_get_xfer_func(in_xfer);

	switch (in_xfer->ep_state) {
	case USBH_VENDOR_RX:
		status = xfer_func(host, in_xfer->xfer_buf, in_xfer->xfer_len, in_xfer->pipe_num);
		in_xfer->tick = usbh_get_tick(host);
		if (status == HAL_OK) {
			in_xfer->ep_state = USBH_VENDOR_RX_BUSY;
		}
		break;

	case USBH_VENDOR_RX_BUSY:
		urb_state = usbh_get_urb_state(host, in_xfer->pipe_num);
		if (urb_state == USBH_URB_DONE) {
			in_xfer->retry_time = 0;
			rx_len = usbh_get_last_transfer_size(host, in_xfer->pipe_num);

			if ((vendor->cb != NULL) && (vendor->cb->receive != NULL)) {
				vendor->cb->receive(in_xfer->ep_type, in_xfer->xfer_buf, rx_len);
			}

			if ((in_xfer->xfer_len >= rx_len) && (in_xfer->xfer_len >= in_xfer->ep_mps)) {
				in_xfer->xfer_len -= rx_len;
				in_xfer->xfer_buf += rx_len;
				in_xfer->ep_state = USBH_VENDOR_RX;
			} else {
				if (in_xfer->xfer_len % in_xfer->ep_mps != 0 || in_xfer->xfer_len == 0) {
					in_xfer->xfer_cnt++;
				}
				in_xfer->xfer_len = 0;
				in_xfer->ep_state = USBH_VENDOR_IDLE;
				vendor->ep_mask &= ~(in_xfer->test_mask);
			}

			usbh_notify_class_state_change(host, in_xfer->pipe_num);
		} else if ((urb_state == USBH_URB_BUSY) || (urb_state == USBH_URB_IDLE)) {
			if (usbh_get_elapsed_ticks(host, in_xfer->tick) > in_xfer->ep_interval) {
				in_xfer->retry_time++;
				if (in_xfer->retry_time < 10) {
					in_xfer->ep_state = USBH_VENDOR_RX;
				} else {
					in_xfer->retry_time = 0;
					in_xfer->ep_state = USBH_VENDOR_IDLE;
					RTK_LOGS(TAG, RTK_LOG_INFO, "intr rx%d timeout\n", in_xfer->xfer_cnt);
				}
			}
			usbh_notify_class_state_change(host, in_xfer->pipe_num);
		} else if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
			in_xfer->state = VENDOR_STATE_IDLE;
			RTK_LOGS(TAG, RTK_LOG_INFO, "intr rx%d err\n", in_xfer->xfer_cnt);
			usbh_notify_class_state_change(host, in_xfer->pipe_num);
		}
		break;

	default:
		break;
	}
}


/**
  * @brief  INTR TX handling
  * @param  host: Host handle
  * @retval None
*/
static void usbh_vendor_intr_process_tx(usb_host_t *host)
{
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_vendor_xfer_t *out_xfer = &vendor->intr_out_xfer;
	usbh_xfer_func xfer_func = usbh_get_xfer_func(out_xfer);
	int status;

	if ((out_xfer->test_mask & vendor->ep_mask) == 0) {
		usbh_notify_class_state_change(host, out_xfer->pipe_num);
		return;
	}

	switch (out_xfer->ep_state) {
	case USBH_VENDOR_TX:
		status = xfer_func(host, out_xfer->xfer_buf, out_xfer->xfer_len, out_xfer->pipe_num);
		out_xfer->tick = usbh_get_tick(host);
		if (status == HAL_OK) {
			out_xfer->ep_state = USBH_VENDOR_TX_BUSY;
		}
		break;

	case USBH_VENDOR_TX_BUSY:
		urb_state = usbh_get_urb_state(host, out_xfer->pipe_num);
		if (urb_state == USBH_URB_DONE) {
			out_xfer->retry_time = 0;
			if (out_xfer->xfer_len >= out_xfer->ep_mps) {
				out_xfer->xfer_len -= out_xfer->ep_mps;
				out_xfer->xfer_buf += out_xfer->ep_mps;
				out_xfer->ep_state = USBH_VENDOR_TX;
			} else {
				out_xfer->xfer_cnt++;
				out_xfer->xfer_len = 0U;
				out_xfer->ep_state = USBH_VENDOR_IDLE;
				vendor->ep_mask &= ~(out_xfer->test_mask);
				if (vendor->cb != NULL && vendor->cb->transmit != NULL) {
					vendor->cb->transmit(out_xfer->ep_type);
				}
			}
			usbh_notify_class_state_change(host, out_xfer->pipe_num);
		} else if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
			out_xfer->ep_state = USBH_VENDOR_IDLE;
			usbh_notify_class_state_change(host, out_xfer->pipe_num);
			RTK_LOGS(TAG, RTK_LOG_INFO, "intr tx%d err\n", out_xfer->xfer_cnt);
		} else if ((urb_state == USBH_URB_BUSY) || (urb_state == USBH_URB_IDLE)) {
			if (usbh_get_elapsed_ticks(host, out_xfer->tick) > out_xfer->ep_interval) {
				out_xfer->retry_time++;
				if (out_xfer->retry_time < 10) {
					out_xfer->ep_state = USBH_VENDOR_TX;
				} else {
					out_xfer->retry_time = 0;
					out_xfer->ep_state = USBH_VENDOR_IDLE;
					vendor->ep_mask &= ~(out_xfer->test_mask);
					RTK_LOGS(TAG, RTK_LOG_INFO, "intr tx%d timeout\n", out_xfer->xfer_cnt);
				}
			}
			usbh_notify_class_state_change(host, out_xfer->pipe_num);
		}
		break;

	default:
		break;
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
	usbh_xfer_func xfer_func = usbh_get_xfer_func(in_xfer);
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	u32 rx_len;
	int status;

	if ((in_xfer->test_mask & vendor->ep_mask) == 0) {
		usbh_notify_class_state_change(host, in_xfer->pipe_num);
		return;
	}

	switch (in_xfer->ep_state) {
	case USBH_VENDOR_RX:
		status = xfer_func(host, in_xfer->xfer_buf, in_xfer->xfer_len, in_xfer->pipe_num);
		in_xfer->tick = usbh_get_tick(host);
		if (status == HAL_OK) {
			in_xfer->ep_state = USBH_VENDOR_RX_BUSY;
		}
		break;

	case USBH_VENDOR_RX_BUSY:
		urb_state = usbh_get_urb_state(host, in_xfer->pipe_num);
		if (urb_state == USBH_URB_DONE) {
			rx_len = usbh_get_last_transfer_size(host, in_xfer->pipe_num);
			if (rx_len == 0) {/* ISOC IN ZLP*/
				in_xfer->retry_time++;
				if (in_xfer->retry_time < 5) {
					in_xfer->ep_state = USBH_VENDOR_RX;
				} else {
					in_xfer->retry_time = 0;
					in_xfer->test_buf[in_xfer->xfer_cnt] = 0xFF;
					in_xfer->xfer_cnt++;
					vendor->ep_mask &= ~(in_xfer->test_mask);
					usbh_vendor_next_xfer(in_xfer);
				}
			} else {
				in_xfer->test_buf[in_xfer->xfer_cnt] = in_xfer->xfer_buf[0];
				in_xfer->retry_time = 0;
				in_xfer->xfer_cnt ++;
				vendor->ep_mask &= ~(in_xfer->test_mask);
				usbh_vendor_next_xfer(in_xfer);
			}
			usbh_notify_class_state_change(host, in_xfer->pipe_num);
		} else if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
			in_xfer->ep_state = USBH_VENDOR_IDLE;
			usbh_notify_class_state_change(host, in_xfer->pipe_num);
		} else if ((urb_state == USBH_URB_BUSY) || (urb_state == USBH_URB_IDLE)) {
			/* ISOC not support retry */
			if (usbh_get_elapsed_ticks(host, in_xfer->tick) > 100) {
				in_xfer->retry_time = 0;
				in_xfer->xfer_cnt ++;
				vendor->ep_mask &= ~(in_xfer->test_mask);
				usbh_vendor_next_xfer(in_xfer);
			}
		}
		break;
	default:
		break;
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
	usbh_xfer_func xfer_func = usbh_get_xfer_func(out_xfer);
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	u32 tx_len;
	int status;

	if ((out_xfer->test_mask & vendor->ep_mask) == 0) {
		usbh_notify_class_state_change(host, out_xfer->pipe_num);
		return;
	}

	switch (out_xfer->ep_state) {
	case USBH_VENDOR_TX:
		if (usbh_get_device_speed(host) == USB_SPEED_HIGH) {
			tx_len = out_xfer->xfer_len;
		} else {
			tx_len = (out_xfer->xfer_len >= out_xfer->ep_mps) ? out_xfer->ep_mps : out_xfer->xfer_len;
		}

		out_xfer->test_buf[out_xfer->xfer_cnt] = out_xfer->xfer_buf[0];
		status = xfer_func(host, out_xfer->xfer_buf, tx_len, out_xfer->pipe_num);

		out_xfer->tick = usbh_get_tick(host);
		if (status == HAL_OK) {
			out_xfer->ep_state = USBH_VENDOR_TX_BUSY;
		}
		usbh_notify_class_state_change(host, out_xfer->pipe_num);
		break;

	case USBH_VENDOR_TX_BUSY:
		urb_state = usbh_get_urb_state(host, out_xfer->pipe_num);
		if (urb_state == USBH_URB_DONE) {
			if (usbh_get_device_speed(host) == USB_SPEED_HIGH) {
				if (out_xfer->xfer_cnt < out_xfer->xfer_max_cnt) {
					vendor->ep_mask &= ~(out_xfer->test_mask);
					out_xfer->ep_state = USBH_VENDOR_TX_WAIT_SOF;
				} else {
					usbh_vendor_next_xfer(out_xfer);
				}
			} else {
				if (out_xfer->xfer_len > out_xfer->ep_mps) {
					out_xfer->xfer_len -= out_xfer->ep_mps;
					out_xfer->xfer_buf += out_xfer->ep_mps;
					out_xfer->ep_state = USBH_VENDOR_TX;
				} else {
					vendor->ep_mask &= ~(out_xfer->test_mask);
					out_xfer->ep_state = USBH_VENDOR_TX_WAIT_SOF;
				}
			}
			usbh_notify_class_state_change(host, out_xfer->pipe_num);
		} else if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
			out_xfer->ep_state = USBH_VENDOR_IDLE;
			usbh_notify_class_state_change(host, out_xfer->pipe_num);
		} else if ((urb_state == USBH_URB_BUSY) || (urb_state == USBH_URB_IDLE)) {
			/* ISOC not support retry */
		}
		break;
	default:
		break;
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

	switch (vendor->state) {

	case VENDOR_STATE_IDLE:
		break;

	case VENDOR_STATE_XFER:
		if (msg == vendor->bulk_in_xfer.pipe_num) {
			usbh_vendor_bulk_process_rx(host);
		} else if (msg == vendor->bulk_out_xfer.pipe_num) {
			usbh_vendor_bulk_process_tx(host);
		} else if (msg == vendor->intr_in_xfer.pipe_num) {
			usbh_vendor_intr_process_rx(host);
		} else if (msg == vendor->intr_out_xfer.pipe_num) {
			usbh_vendor_intr_process_tx(host);
		} else if (msg == vendor->isoc_in_xfer.pipe_num) {
			usbh_vendor_isoc_process_rx(host);
		} else if (msg == vendor->isoc_out_xfer.pipe_num) {
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
  * @brief  Start to transfer bulk data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbh_vendor_bulk_transmit(u8 *buf, u32 len, u32 test_cnt)
{
	int ret = HAL_BUSY;
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usb_host_t *host = vendor->host;
	usbh_vendor_xfer_t *out_xfer = &vendor->bulk_out_xfer;

	if ((out_xfer->ep_state == USBH_VENDOR_IDLE)
		&& ((vendor->state == VENDOR_STATE_IDLE) || (vendor->state == VENDOR_STATE_XFER))) {
		out_xfer->xfer_len = len;
		out_xfer->xfer_max_len = len;
		out_xfer->xfer_buf = buf;
		out_xfer->xfer_bk_buf = buf;
		out_xfer->xfer_max_cnt = test_cnt;
		out_xfer->test_mask = USBH_VENDOR_MASK_BULK_OUT;
		vendor->state = VENDOR_STATE_XFER;
		out_xfer->ep_state = USBH_VENDOR_TX;
		usbh_notify_class_state_change(host, out_xfer->pipe_num);
		ret = HAL_OK;
	}

	return ret;
}

/**
  * @brief  Start to receive bulk data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbh_vendor_bulk_receive(u8 *buf, u32 len, u32 test_cnt)
{
	int ret = HAL_BUSY;
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usb_host_t *host = vendor->host;
	usbh_vendor_xfer_t *in_xfer = &vendor->bulk_in_xfer;

	if ((vendor->state == VENDOR_STATE_IDLE) || (vendor->state == VENDOR_STATE_XFER)) {
		if (in_xfer->ep_state == USBH_VENDOR_IDLE) {
			in_xfer->xfer_len = len;
			in_xfer->xfer_max_len = len;
			in_xfer->xfer_buf = buf;
			in_xfer->xfer_bk_buf = buf;
			in_xfer->xfer_max_cnt = test_cnt;
			in_xfer->test_mask = USBH_VENDOR_MASK_BULK_IN;
			vendor->state = VENDOR_STATE_XFER;
			in_xfer->ep_state = USBH_VENDOR_RX;
			usbh_notify_class_state_change(host, in_xfer->pipe_num);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Start to transfer intr data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbh_vendor_intr_transmit(u8 *buf, u32 len, u32 test_cnt)
{
	int ret = HAL_BUSY;
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usb_host_t *host = vendor->host;
	usbh_vendor_xfer_t *out_xfer = &vendor->intr_out_xfer;

	if ((out_xfer->ep_state == USBH_VENDOR_IDLE)
		&& ((vendor->state == VENDOR_STATE_IDLE) || (vendor->state == VENDOR_STATE_XFER))) {
		out_xfer->xfer_len = len;
		out_xfer->xfer_max_len = len;
		out_xfer->xfer_buf = buf;
		out_xfer->xfer_bk_buf = buf;
		out_xfer->xfer_max_cnt = test_cnt;
		out_xfer->test_mask = USBH_VENDOR_MASK_INTR_OUT;
		vendor->state = VENDOR_STATE_XFER;
		out_xfer->ep_state = USBH_VENDOR_TX;
		usbh_notify_class_state_change(host, out_xfer->pipe_num);
		ret = HAL_OK;
	}

	return ret;
}

/**
  * @brief  Start to receive intr data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbh_vendor_intr_receive(u8 *buf, u32 len, u32 test_cnt)
{
	int ret = HAL_BUSY;
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usb_host_t *host = vendor->host;
	usbh_vendor_xfer_t *in_xfer = &vendor->intr_in_xfer;

	if ((vendor->state == VENDOR_STATE_IDLE) || (vendor->state == VENDOR_STATE_XFER)) {
		if (in_xfer->ep_state == USBH_VENDOR_IDLE) {
			in_xfer->xfer_buf = buf;
			in_xfer->xfer_bk_buf = buf;
			in_xfer->xfer_len = len;
			in_xfer->xfer_max_len = len;
			in_xfer->xfer_max_cnt = test_cnt;
			in_xfer->test_mask = USBH_VENDOR_MASK_INTR_IN;
			vendor->state = VENDOR_STATE_XFER;
			in_xfer->ep_state = USBH_VENDOR_RX;
			usbh_notify_class_state_change(host, in_xfer->pipe_num);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Start to transfer isoc data
  * @param  test_cnt: Test Count
  * @param  len: Data length
  * @retval Status
  */
int usbh_vendor_isoc_transmit(u8 *buf, u32 len, u32 test_cnt)
{
	int ret = HAL_BUSY;
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usb_host_t *host = vendor->host;
	usbh_vendor_xfer_t *out_xfer = &vendor->isoc_out_xfer;

	if ((out_xfer->ep_state == USBH_VENDOR_IDLE)
		&& ((vendor->state == VENDOR_STATE_IDLE) || (vendor->state == VENDOR_STATE_XFER))) {
		out_xfer->test_buf = (u8 *)usb_os_malloc(test_cnt);
		if (out_xfer->test_buf == NULL) {
			goto exit;
		}
		out_xfer->xfer_buf = buf;
		out_xfer->xfer_bk_buf = buf;
		out_xfer->xfer_len = len;
		out_xfer->xfer_max_len = len;
		out_xfer->xfer_max_cnt = test_cnt;
		vendor->state = VENDOR_STATE_XFER;
		out_xfer->ep_state = USBH_VENDOR_TX;
		usbh_notify_class_state_change(host, out_xfer->pipe_num);
		ret = HAL_OK;
	}

	return ret;
exit:
	return ret;
}

/**
  * @brief  Start to receive isoc data
  * @param  test_cnt: Test Count
  * @param  len: Data length
  * @retval Status
  */
int usbh_vendor_isoc_receive(u8 *buf, u32 len, u32 test_cnt)
{
	int ret = HAL_BUSY;
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usb_host_t *host = vendor->host;
	usbh_vendor_xfer_t *in_xfer = &vendor->isoc_in_xfer;

	if ((vendor->state == VENDOR_STATE_IDLE) || (vendor->state == VENDOR_STATE_XFER)) {
		if (in_xfer->ep_state == USBH_VENDOR_IDLE) {
			in_xfer->test_buf = (u8 *)usb_os_malloc(test_cnt);
			if (in_xfer->test_buf == NULL) {
				goto exit;
			}
			in_xfer->xfer_buf = buf;
			in_xfer->xfer_bk_buf = buf;
			in_xfer->xfer_len = len;
			in_xfer->xfer_max_len = len;
			in_xfer->xfer_max_cnt = test_cnt;
			vendor->state = VENDOR_STATE_XFER;
			in_xfer->ep_state = USBH_VENDOR_RX;
			usbh_notify_class_state_change(host, in_xfer->pipe_num);
			ret = HAL_OK;
		}
	}

	return ret;

exit:
	return ret;
}

/**
  * @brief  Get the BULK IN max packet size
  * @retval packet size
  */
u16 usbh_vendor_get_bulk_ep_mps(void)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	return vendor->bulk_in_xfer.ep_mps;
}

/**
  * @brief  Get the INTR IN max packet size
  * @retval packet size
  */
u16 usbh_vendor_get_intr_ep_mps(void)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	return vendor->intr_in_xfer.ep_mps;
}