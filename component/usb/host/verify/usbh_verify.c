/**
  ******************************************************************************
  * @file    usbh_verify.c
  * @author  Realsil WLAN5 Team
  * @brief   This file provides the functionalities of the USB Verify Class
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ameba_soc.h"
#include "usbh.h"
#include "usbh_verify.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/
typedef int(* usbh_xfer_func)(usb_host_t *host, u8 *buf, u16 len, u8 pipe_id);

/* Private macros ------------------------------------------------------------*/
#define DUMP_DATA_MAX_LEN (32)

#define USBH_DUMP_EP_MSG    0

/* Private function prototypes -----------------------------------------------*/

static int usbh_verify_attach(usb_host_t *host);
static int usbh_verify_detach(usb_host_t *host);
static int usbh_verify_process(usb_host_t *host);
static int usbh_verify_setup(usb_host_t *host);
//static int usbh_verify_sof(usb_host_t *host);
//static int usbh_verify_nak(usb_host_t *host, u8 pipe_id);
static int usbh_verify_usb_status_check(void);

/* Private variables ---------------------------------------------------------*/
static const char *TAG = "VFY";
static const char *TEXT_CTRL = "CTRL";
static const char *TEXT_BULK = "BULK";
static const char *TEXT_INTR = "INTR";
static const char *TEXT_ISOC = "ISOC";

/*
	0:idle
	1:running
	2:exit the task, switch to idle
*/
static __IO u8 usbh_verifty_status_task = 0 ;
static __IO u8 usbh_verifty_test_task = 0 ;
/* USB Standard Device Descriptor */
static usbh_class_driver_t usbh_verify_driver = {
	.class_code = VERIFY_CLASS_CODE,
	.attach = usbh_verify_attach,
	.detach = usbh_verify_detach,
	.setup = usbh_verify_setup,
	.process = usbh_verify_process,
	.sof = NULL,
	.nak = NULL,
};

static usbh_verify_host_t usbh_verify_host;

/* Private functions ---------------------------------------------------------*/

void usbh_verify_set_test_count_max(u32 count)
{
	usbh_verify_host.test_count_limit = count;
}

static char *usbh_verify_get_xfer_type_text(usbh_verify_xfer_t *xfer)
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

static usbh_xfer_func usbh_verify_get_xfer_func(usbh_verify_xfer_t *xfer)
{
	usbh_xfer_func func = NULL;

	if (USB_EP_IS_IN(xfer->ep_addr)) {
		switch (xfer->ep_type) {
		case USB_CH_EP_TYPE_BULK:
			func = (usbh_xfer_func)usbh_bulk_receive_data;
			break;
		case USB_CH_EP_TYPE_INTR:
			func = (usbh_xfer_func)usbh_intr_receive_data;
			break;
		case USB_CH_EP_TYPE_ISOC:
			func = (usbh_xfer_func)usbh_isoc_receive_data;
			break;
		default:
			break;
		}
	} else {
		switch (xfer->ep_type) {
		case USB_CH_EP_TYPE_BULK:
			func = (usbh_xfer_func)usbh_bulk_send_data;
			break;
		case USB_CH_EP_TYPE_INTR:
			func = (usbh_xfer_func)usbh_intr_send_data;
			break;
		case USB_CH_EP_TYPE_ISOC:
			func = (usbh_xfer_func)usbh_isoc_send_data;
			break;
		default:
			break;
		}
	}

	return func;
}

static void usbh_verify_dump_ep_info(usbh_verify_xfer_t *xfer)
{
#if USBH_DUMP_EP_MSG
	char *xfer_type = usbh_verify_get_xfer_type_text(xfer);
	char *direction = (USB_EP_IS_IN(xfer->ep_addr)) ? ("IN") : ("OUT") ;
	RTK_LOGS(TAG, "[VFY] Dump: %s-%s mask=0x%08x state=%d EP%02x/pipe=%d MPS=%d transize=%d type=%d interval=%d\n",
			 xfer_type, direction, xfer->test_mask,
			 xfer->state, xfer->ep_addr,
			 xfer->pipe_id, xfer->ep_mps, xfer->trans_size,
			 xfer->ep_type, xfer->ep_interval);
#else
	UNUSED(xfer);
#endif
}

static void usbh_verify_dump_buf(u8 *buf, u32 len)
{
	u32 i;
	if (len > DUMP_DATA_MAX_LEN) {
		len = DUMP_DATA_MAX_LEN ;
	}
	for (i = 0; i < len;) {
		if (i + 10 <= len) {
			RTK_LOGS(TAG, " %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n", buf[i], buf[i + 1], buf[i + 2], buf[i + 3], buf[i + 4], buf[i + 5], buf[i + 6], buf[i + 7],
					 buf[i + 8],
					 buf[i + 9]);
			i += 10;
		} else {
			for (; i < len; i++) {
				RTK_LOGS(TAG, " %3d ", buf[i]);
			}
			RTK_LOGS(TAG, "\n");
			break;
		}
	}
}

static int usbh_verify_add_ep(usbh_ep_desc_t *ep_desc)
{
	u16 mps;
	usbh_verify_xfer_t *ep;
	u8 epaddr, ep_type, ep_index;
	usbh_verify_host_t *verify = &usbh_verify_host;

	if (verify->ep_count >= USBH_EP_COUNT_MAX) {
		RTK_LOGS(TAG, "[VFY] Too much EP defined, just support %d EPs\n", USBH_EP_COUNT_MAX);
		return HAL_ERR_MEM;
	}

	epaddr = ep_desc->bEndpointAddress;
	mps = ep_desc->wMaxPacketSize;
	ep_type = ep_desc->bmAttributes & USB_EP_XFER_TYPE_MASK;

	for (ep_index = 0 ; ep_index < verify->ep_count; ep_index++) {
		ep = verify->ep_array + ep_index;
		if (ep && (epaddr == ep->ep_addr)) {
			break;
		}
	}
	if (ep_index == verify->ep_count) { //a new one
		ep = verify->ep_array + verify->ep_count;
		verify->ep_count ++;
		usb_os_memset(ep, 0x00, sizeof(usbh_verify_xfer_t));
		ep->ep_addr = epaddr;
	}
	ep->pipe_id = USBH_PIPE_VALID_VALUE;
	ep->ep_mps = mps;
	ep->ep_type = ep_type;
	ep->ep_interval = usbh_get_interval(verify->host, ep_type, ep_desc->bInterval);

	usbh_verify_dump_ep_info(ep);

	return HAL_OK;
}

static void usbh_verify_get_endpoints(usbh_if_desc_t *intf)
{
	u32 tmp = 0;
	usbh_ep_desc_t *ep_desc;

	if (intf->bNumEndpoints > USBH_EP_COUNT_MAX) {
		RTK_LOGS(TAG, "[VFY] Too much EP cnt\n");
		intf->bNumEndpoints = USBH_EP_COUNT_MAX;
	}

	for (tmp = 0; tmp < intf->bNumEndpoints; tmp++) {
		ep_desc = &intf->ep_desc_array[tmp];
		usbh_verify_add_ep(ep_desc);
	}
}

static int usbh_verify_all_out_finish(void)
{
	int ret = 1;
	u8 ep_index;
	usbh_verify_xfer_t *ep;
	usbh_verify_host_t *verify = &usbh_verify_host;
	if (verify->finish_tick) { //USBH_FORCE_IN_EXIT_SOF_COUNT_MAX tick count
		if ((verify->host->tick - verify->finish_tick) < USBH_FORCE_IN_EXIT_SOF_COUNT_MAX) {
			ret = 0;
		} else {
			RTK_LOGS(TAG, "[VFY] Force all USB TX finish tick %d\n", verify->host->tick);
		}
	} else {
		for (ep_index = 0 ; ep_index < verify->ep_count; ep_index++) {
			ep = verify->ep_array + ep_index;
			if (PIPE_IS_VALID(ep->pipe_id) && USB_EP_IS_OUT(ep->ep_addr) && (ep->xfer_cnt < verify->test_count_limit)) {
				ret = 0 ;
			}
		}
		//if ret =1 means all usb out finish,update the finish tick value
		if ((1 == ret) && (0 == verify->finish_tick)) {
			RTK_LOGS(TAG, "[VFY] All USB TX finish tick %d\n", verify->host->tick);
			verify->finish_tick = verify->host->tick;
			ret = 0 ;
		}
	}

	return ret;
}

static inline void usbh_verify_success(usbh_verify_xfer_t *xfer)
{
	xfer->xfer_cnt ++;
	xfer->total_xfer_cnt ++;
}

static void usbh_verify_check_indata_valid(usbh_verify_xfer_t *rx_xfer)
{
	if (rx_xfer) {
		if (rx_xfer->expect_data != rx_xfer->xfer_buf[0]) {
			rx_xfer->error_cnt ++;
			RTK_LOGS(TAG, "[VFY] %s err %d/want %d get %d\n", usbh_verify_get_xfer_type_text(rx_xfer),
					 rx_xfer->error_cnt, rx_xfer->expect_data, rx_xfer->xfer_buf[0]);
		}
		rx_xfer->expect_data = rx_xfer->xfer_buf[0] + 1;
	}
}

static void usbh_verify_tx_transmit(usbh_verify_xfer_t *in_xfer, usbh_xfer_func xfer_func)
{
	usbh_verify_host_t *verify = &usbh_verify_host;
	usb_host_t *host = verify->host;

	xfer_func(host, in_xfer->xfer_buf, in_xfer->trans_size, in_xfer->pipe_id);//in token
	in_xfer->state = VERIFY_STATE_BUSY;
	in_xfer->idle_count = 0 ;
}

int usbh_verify_xfer_process(usbh_verify_xfer_t *xfer)
{
	usbh_verify_xfer_t *in_xfer = NULL;
	usbh_verify_xfer_t *out_xfer = NULL ;
	usbh_xfer_func xfer_func = NULL;
	usbh_urb_state_t xfer_urb_state = USBH_URB_IDLE;
	usbh_verify_host_t *verify = &usbh_verify_host;
	usb_host_t *host = verify->host;
	int is_timeout = 0;
	int status;
	int loop_ctl = 1;
	int is_ptx;
	u32 len;
	u32 ticks;

	char *xfer_type = usbh_verify_get_xfer_type_text(xfer);
	xfer_func = usbh_verify_get_xfer_func(xfer);
	if (NULL == xfer_func) {
		return HAL_OK;
	}

	if (USB_EP_IS_IN(xfer->ep_addr)) {//in
		in_xfer = xfer;
		if (usbh_verify_all_out_finish() || (in_xfer->xfer_cnt >= verify->test_count_limit)) {
			RTK_LOGS(TAG, "[VFY] %s test result: force IN Finish %d/%d flag 0x%08x\n", xfer_type, in_xfer->xfer_cnt, verify->test_count_limit, verify->ep_test);
			in_xfer->state = VERIFY_STATE_IDLE;
			verify->ep_test &= ~(in_xfer->test_mask);
			return HAL_OK;
		}
		switch (in_xfer->state) {
		case VERIFY_STATE_XFER:
			usbh_verify_tx_transmit(in_xfer, xfer_func);
			break;

		case VERIFY_STATE_BUSY:
			xfer_urb_state = usbh_get_urb_state(host, in_xfer->pipe_id);
			//RTK_LOGS(TAG, "[VFY] %s-in TX busy,state %d\n",xfer_type,xfer_urb_state);
			if (xfer_urb_state == USBH_URB_DONE) {
				in_xfer->idle_count = 0;
				len = usbh_get_last_transfer_size(host, in_xfer->pipe_id);
				if (len == 0) {//ZLP
					//RTK_LOGS(TAG, "[VFY] %s in ZLP\n",xfer_type);
					usbh_verify_tx_transmit(in_xfer, xfer_func);
					break;
				}
				//RTK_LOGS(TAG, "[VFY] %s in done %02x %d data=%d len=%d-%d\n", xfer_type, in_xfer->ep_addr,
				//		 in_xfer->xfer_cnt, in_xfer->xfer_buf[0],
				//		 len, in_xfer->xfer_len);
				//usbh_verify_dump_buf(in_xfer->xfer_buf, len);
				if (((in_xfer->xfer_len - len) > 0U) && (len >= in_xfer->ep_mps)) {
					in_xfer->xfer_len -= len ;
					in_xfer->xfer_buf += len;
					in_xfer->state = VERIFY_STATE_XFER;
				} else {
					usbh_verify_success(in_xfer);
					usbh_verify_check_indata_valid(in_xfer);
					//RTK_LOGS(TAG, "[VFY] In xferlen %d/%d,mps %d cnt %d/%d\n", in_xfer->xfer_len, len, in_xfer->ep_mps, in_xfer->xfer_cnt, verify->test_count_limit);
					if (usbh_verify_all_out_finish() || (in_xfer->xfer_cnt >= verify->test_count_limit)) {
						in_xfer->state = VERIFY_STATE_IDLE;
						verify->ep_test &= ~(in_xfer->test_mask);
						RTK_LOGS(TAG, "[VFY] %s-n test finish %d/%d flag %d\n", xfer_type, in_xfer->xfer_cnt, verify->test_count_limit, verify->ep_test);
					} else {
						in_xfer->xfer_len = in_xfer->trans_size;
						in_xfer->xfer_buf = in_xfer->xfer_bk_buf;

						//start next intoken
						usbh_verify_tx_transmit(in_xfer, xfer_func);
					}
				}
			} else if ((xfer_urb_state == USBH_URB_ERROR) || (xfer_urb_state == USBH_URB_STALL)) {
				RTK_LOGS(TAG, "[VFY] %s in err force exit\n", xfer_type);
				in_xfer->state = VERIFY_STATE_IDLE;
				loop_ctl = 0;
			} else if (xfer_urb_state == USBH_URB_BUSY)  {//busy
				///check finish issue
				//RTK_LOGS(TAG, "[VFY]%s in busy state, continue wait, idle=%d\n", xfer_type, in_xfer->idle_count);
				in_xfer->idle_count ++;
				//check the time is better then the loop count
				if ((in_xfer->ep_type == USB_CH_EP_TYPE_ISOC) && (in_xfer->idle_count >= 1000)) {
					//RTK_LOGS(TAG, "[VFY] %s busy too long, reset\n", xfer_type);
					in_xfer->state = VERIFY_STATE_XFER;
					in_xfer->idle_count = 0 ;
				}
			} else { //idle
				/*
								We assume that,while call usbh_xx_receive_data,
								The intoken will be sent out,
								So just wait for usb status change

								//while run usbh_receive_data , but the interrupt not happen
								in_xfer->idle_count ++ ;
								if (in_xfer->idle_count > 2000) {
									//RTK_LOGS(TAG, "[VFY] %s in maybe some wrong, try again\n", xfer_type);
									in_xfer->idle_count = 0  ;
									in_xfer->state = VERIFY_STATE_XFER;
								}
				*/
			}
			break;
		default:
			//RTK_LOGS(TAG, "[VFY] %s in default, switch %d to idle\n", xfer_type, in_xfer->state);
			in_xfer->state = VERIFY_STATE_IDLE;
			break;
		}
	} else { //usb out
		is_ptx = ((xfer->ep_type == USB_CH_EP_TYPE_INTR) || (xfer->ep_type == USB_CH_EP_TYPE_ISOC));
		out_xfer = xfer;
		switch (out_xfer->state) {
		case VERIFY_STATE_XFER:
			if (is_ptx) {
				ticks = host->tick;
				if ((ticks - out_xfer->tick) >= out_xfer->ep_interval) {
					out_xfer->tick = ticks;
					is_timeout = 1;
				}
			}

			if ((!is_ptx) || is_timeout) {
				out_xfer->idle_count = 0 ;
				//RTK_LOGS(TAG, "[VFY] %s out will run cmd len=%d-%d/mps=%d\n", xfer_type, out_xfer->xfer_len, out_xfer->xfer_buf[0], out_xfer->ep_mps);
				if (out_xfer->xfer_len > out_xfer->ep_mps) {
					status = xfer_func(host, out_xfer->xfer_buf, out_xfer->ep_mps, out_xfer->pipe_id);
				} else {
					status = xfer_func(host, out_xfer->xfer_buf, out_xfer->xfer_len, out_xfer->pipe_id);
				}

				if (status == HAL_OK) {
					out_xfer->state = VERIFY_STATE_BUSY;
				}
			}
			break;

		case VERIFY_STATE_BUSY:
			xfer_urb_state = usbh_get_urb_state(host, out_xfer->pipe_id);
			if (xfer_urb_state == USBH_URB_DONE) {
				out_xfer->idle_count = 0 ;
				//RTK_LOGS(TAG, "[VFY] %s out xferlen=%d,mps=%d %d[%d] done\n", xfer_type, out_xfer->xfer_len, out_xfer->ep_mps, out_xfer->xfer_cnt, verify->test_count_limit);
				if (out_xfer->xfer_len > out_xfer->ep_mps) {//continue
					out_xfer->xfer_len -= out_xfer->ep_mps;
					out_xfer->xfer_buf += out_xfer->ep_mps;
					out_xfer->state = VERIFY_STATE_XFER;
				} else if (out_xfer->xfer_len == out_xfer->ep_mps) { //ZLP
					out_xfer->xfer_len = 0U;
					out_xfer->xfer_buf = NULL;
					out_xfer->state = VERIFY_STATE_XFER;
				} else {//finish
					usbh_verify_success(out_xfer);
					if (out_xfer->xfer_cnt >= verify->test_count_limit) {
						RTK_LOGS(TAG, "[VFY] %s-out test finish %d/%d\n", xfer_type, out_xfer->xfer_cnt, verify->test_count_limit);
						verify->ep_test &= ~(out_xfer->test_mask);
						out_xfer->state = VERIFY_STATE_IDLE;
						loop_ctl = 0;
					} else {
						out_xfer->xfer_len = out_xfer->trans_size;
						out_xfer->xfer_buf = out_xfer->xfer_bk_buf;
						out_xfer->state = VERIFY_STATE_XFER;
						usb_os_memset((void *)out_xfer->xfer_buf, (u8)(out_xfer->xfer_cnt & 0xFF), out_xfer->xfer_len);
					}
				}
			} else if (xfer_urb_state == USBH_URB_ERROR) {
				RTK_LOGS(TAG, "[VFY] %s out err\n", xfer_type);
				out_xfer->state = VERIFY_STATE_XFER;
			} else {//busy&idle
				out_xfer->idle_count ++;
				if ((out_xfer->ep_type == USB_CH_EP_TYPE_ISOC) && (out_xfer->idle_count >= 1000)) {
					//RTK_LOGS(TAG, "[VFY] %s out busy too long, tx again\n", xfer_type);
					out_xfer->idle_count = 0 ;

					out_xfer->xfer_len = out_xfer->trans_size;
					out_xfer->xfer_buf = out_xfer->xfer_bk_buf;
					out_xfer->state = VERIFY_STATE_XFER;
					usb_os_memset((void *)out_xfer->xfer_buf, (u8)out_xfer->xfer_cnt, out_xfer->xfer_len);
				}
			}
			break;
		default:
			//RTK_LOGS(TAG, "[VFY] %s out default, switch %d to idle\n", xfer_type, out_xfer->state);
			out_xfer->state = VERIFY_STATE_IDLE;
			break;
		}
	}
	if (loop_ctl) {
		verify->next_transfor = 1;
	}

	return HAL_OK;
}

static void usbh_verify_ctrl_process_rx(usb_host_t *host)
{
	usbh_setup_req_t setup;
	int ret = HAL_BUSY;
	int loop_ctl = 1;
	usbh_verify_host_t *verify = &usbh_verify_host;
	usbh_verify_xfer_t *ep ;
	ep = &(verify->ctrl_in_xfer);
	if (USBH_VERIFY_CTRL_TX & verify->ctrl_transfer_idle) {
		return ;
	}
	if (USBH_VERIFY_CTRL_IDLE == verify->ctrl_transfer_idle) {
		verify->ctrl_transfer_idle = USBH_VERIFY_CTRL_RX;
	}
	//RTK_LOGS(TAG, "[VFY] CTRL in Type state=%d/len=%d\n", ep->state, ep->xfer_len);
	switch (ep->state) {
	case VERIFY_STATE_XFER:
		setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_VENDOR ;
		setup.b.bRequest = USB_REQ_GET_DESCRIPTOR;
		setup.b.wValue = 0x0000U;
		setup.b.wIndex = 0x00U;
		setup.b.wLength = ep->xfer_len;
		ret = usbh_ctrl_request(host, &setup, ep->xfer_buf);
		//RTK_LOGS(TAG, "[VFY] Enter ctrl RX state=%d/ret=%d\n", ep->state, ret);
		if (ret == HAL_OK) {
			usbh_verify_success(ep);
			usbh_verify_check_indata_valid(ep);
			verify->ctrl_transfer_idle = USBH_VERIFY_CTRL_IDLE;
			//RTK_LOGS(TAG, "[VFY] CTRL RX xferlen=%d,count=%d/%d\n", ep->xfer_len, ep->xfer_cnt, verify->test_count_limit);
			usbh_verify_dump_buf(ep->xfer_buf, ep->xfer_len);
			usb_os_memset((void *)ep->xfer_buf, (u8)ep->xfer_cnt, ep->xfer_len);
			if (ep->xfer_cnt >= verify->test_count_limit) {
				RTK_LOGS(TAG, "[VFY] CTRL RX test finish(%d/%d/%d)\n", ep->xfer_cnt, verify->test_count_limit, verify->ep_test);
				ep->state = VERIFY_STATE_IDLE;
				verify->ep_test &= ~(ep->test_mask);
				loop_ctl = 0;
			} else {
				ep->state = VERIFY_STATE_XFER;
			}

		} else if (ret != HAL_BUSY) {
			ep->state = VERIFY_STATE_ERROR;
			RTK_LOGS(TAG, "[VFY] Fail to TX CTRL req, ret %d\n", ret);
		}
		break;

	case VERIFY_STATE_ERROR:
		RTK_LOGS(TAG, "[VFY] Err, finish RX Test %d/%d\n", ep->xfer_cnt, verify->test_count_limit);
		ret = usbh_ctrl_clear_feature(host, 0x00U);
		if (ret == HAL_OK) {
			ep->state = VERIFY_STATE_IDLE;
		}
		loop_ctl = 0;
		break;

	default:
		break;
	}
	if (loop_ctl) {
		verify->next_transfor = 1;
	}
}

static void usbh_verify_ctrl_process_tx(usb_host_t *host)
{
	usbh_setup_req_t setup;
	u8 ret = HAL_BUSY;
	u8 loop_ctl = 1;
	usbh_verify_host_t *verify = &usbh_verify_host;
	usbh_verify_xfer_t *ep ;
	ep = &(verify->ctrl_out_xfer);
	if (USBH_VERIFY_CTRL_RX & verify->ctrl_transfer_idle) {
		return ;
	}
	if (USBH_VERIFY_CTRL_IDLE == verify->ctrl_transfer_idle) {
		verify->ctrl_transfer_idle = USBH_VERIFY_CTRL_TX;
	}

	switch (ep->state) {
	case VERIFY_STATE_XFER:
		setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_VENDOR ;
		setup.b.bRequest = VERIFY_CTRL_REQUEST_TYPE;
		setup.b.wValue = 0x0000U;
		setup.b.wIndex = 0x00U;
		setup.b.wLength = ep->xfer_len;
		ret = usbh_ctrl_request(host, &setup, ep->xfer_buf);
		if (ret == HAL_OK) {
			usbh_verify_success(ep);
			verify->ctrl_transfer_idle = USBH_VERIFY_CTRL_IDLE;
			//RTK_LOGS(TAG, "[VFY] Xferlen %d count %d/%d\n", ep->xfer_len, ep->xfer_cnt, verify->test_count_limit);
			usb_os_memset((void *)ep->xfer_buf, (u8)ep->xfer_cnt, ep->xfer_len);
			if (ep->xfer_cnt >= verify->test_count_limit) {
				RTK_LOGS(TAG, "[VFY] CTRL TX test finish %d/%d/%d\n", ep->xfer_cnt, verify->test_count_limit, verify->ep_test);
				ep->state = VERIFY_STATE_IDLE;
				verify->ep_test &= ~(ep->test_mask);
				loop_ctl = 0;
			} else {
				ep->state = VERIFY_STATE_XFER;
			}
		} else if (ret != HAL_BUSY) {
			ep->state = VERIFY_STATE_ERROR;
			RTK_LOGS(TAG, "[VFY] Fail to TX CTRL req, ret %d\n", ret);
		}
		break;

	case VERIFY_STATE_ERROR:
		RTK_LOGS(TAG, "[VFY] Err, finish TX test %d/%d\n", ep->xfer_cnt, verify->test_count_limit);
		ret = usbh_ctrl_clear_feature(host, 0x00U);
		if (ret == HAL_OK) {
			ep->state = VERIFY_STATE_IDLE;
		}
		loop_ctl = 0;
		break;
	default:
		break;
	}

	if (loop_ctl) {
		verify->next_transfor = 1;
	}
}

int usbh_verify_switch_interface(u8 if_num, u8 if_alt)
{
	int ret;
	usbh_verify_host_t *verify = &usbh_verify_host;
	usb_host_t *host = verify->host;
	usbh_if_desc_t *verify_if_desc;
	verify->interface_id = if_num;
	verify->alterset_id = if_alt;
	if (NULL == host) {
		return HAL_BUSY;
	}
	usbh_set_interface(host, if_num);
	do {
		ret = usbh_ctrl_set_interface(host, if_num, if_alt);
		if (ret == HAL_OK) {
			break;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, "[VFY] Fail to TX Ctrl req, ret %d\n", ret);
			return ret;
		}
	} while (1);

	verify_if_desc = usbh_get_interface_descriptor(host, verify->interface_id, verify->interface_id);
	if (verify_if_desc == NULL) {
		RTK_LOGS(TAG, "[VFY] Fail to get itf desc");
		return HAL_ERR_PARA;
	}

	return HAL_OK;
}

#if  0
/**
  * @brief  Usb Sof callback function.
  * @param  host: Host handle
  * @retval HAL_OK
  */
static int usbh_verify_sof(usb_host_t *host)
{
	u8 i = 0;
	usbh_verify_host_t *cdc = &usbh_verify_host;
	for (i = 0U; i < host->config.pipes; i++) {
		if (cdc->host_pipe[i]) {
			cdc->host_pipe[i] = 0;
			usbh_reactivate_pipe(host, i);
		}
	}

	return HAL_OK;
}

/**
  * @brief  Usb nak callback function.
  * @param  host: Host handle
  * @param  pipe_id: Pipe number
  * @retval return OK while success, else return HAL_ERR_UNKNOWN
  */
static int usbh_verify_nak(usb_host_t *host, u8 pipe_id)
{
	u8 ep_type ;
	usbh_verify_host_t *cdc = &usbh_verify_host;
	if (pipe_id > USBH_MAX_PIPES_NUM) {
		return HAL_ERR_PARA ;
	}

	ep_type = usbh_get_ep_type(host, pipe_id);
	//RTK_LOGS(TAG, "[VFY] nak type=%d/pipe_id=%d\n",ep_type,pipe_id);
	if (ep_type == USB_CH_EP_TYPE_BULK || ep_type == USB_CH_EP_TYPE_INTR) {
		//do not retransmit in this loop, it will do in next SOF
		cdc->host_pipe[pipe_id] = 1;
		return HAL_OK;
	}

	return HAL_ERR_UNKNOWN ;
}
#endif

/**
  * @brief  Attach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_verify_attach(usb_host_t *host)
{
	int status = HAL_OK;
	u8 interface_id;
	usbh_verify_host_t *verify = &usbh_verify_host;
	verify->usbh_state = USBH_VERIFY_ATTACH;
	usbh_if_desc_t *usbh_if_desc;

	/* Get interface index */
	interface_id = usbh_get_interface(host, VERIFY_CLASS_CODE, VERIFY_SUBCLASS_CODE, VERIFY_PROTOCOL);
	if (interface_id == 0xFFU) {
		RTK_LOGS(TAG, "[VFY] Fail to find itf");
		return HAL_ERR_PARA;
	}
	verify->host = host;

	status = usbh_verify_switch_interface(interface_id, 0);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, "[VFY] Fail to set itf");
		return HAL_ERR_UNKNOWN;
	}

	usbh_if_desc = usbh_get_interface_descriptor(host, interface_id, 0);
	if (usbh_if_desc == NULL) {
		RTK_LOGS(TAG, "[VFY] Fail to get itf desc\n");
		return HAL_ERR_PARA;
	}

	/* Get data in/out endpoints */
	usbh_verify_get_endpoints(usbh_if_desc);

	return status;
}

/**
  * @brief  Detach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_verify_detach(usb_host_t *host)
{
	UNUSED(host);
	usbh_verify_host_t *verify = &usbh_verify_host;
	verify->usbh_state = USBH_VERIFY_DETACHED;
	if ((verify->cb != NULL) && (verify->cb->detach != NULL)) {
		verify->cb->detach();
	}

	return HAL_OK;
}

/**
  * @brief  Standard control requests handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_verify_setup(usb_host_t *host)
{
	usbh_verify_host_t *verify = &usbh_verify_host;
	verify->usbh_state = USBH_VERIFY_SETUP;
	UNUSED(host);

	if ((verify->cb != NULL) && (verify->cb->setup != NULL)) {
		verify->cb->setup();
	}

	return HAL_OK;
}

/**
  * @brief  State machine handling callback
  * @param  host: Host handle
  * @retval Status
  */
static void usbh_verify_dump_testmask(void)
{
	u8 ep_index;
	usbh_verify_host_t *verify = &usbh_verify_host;
	usbh_verify_xfer_t *xfer;
	if (verify->ep_test) {
		RTK_LOGS(TAG, "[VFY] Ep flag=0x%08x\n", verify->ep_test);
		for (ep_index = 0 ; ep_index < verify->ep_count; ep_index++) {
			xfer = verify->ep_array + ep_index;
			if (xfer->test_mask & verify->ep_test) {
				RTK_LOGS(TAG, "[VFY] USBH %s-0x%08x", usbh_verify_get_xfer_type_text(xfer), xfer->test_mask);
			}
		}
	}
}

static int usbh_verify_process(usb_host_t *host)
{
	int status = HAL_OK;
	u8 ep_index;
	usbh_verify_host_t *verify = &usbh_verify_host;
	verify->loopcount ++;
	if (0 == verify->start_process) {
		usb_os_sleep_ms(1000);
		if (verify->loopcount % 60 == 0) {
			RTK_LOGS(TAG, "[VFY] Main process idle mask 0x%08x\n", verify->ep_test);
		}
		return status;
	}

	if (verify->ep_test) {
		//ctrl tx
		if ((verify->ctrl_out_xfer.state != VERIFY_STATE_IDLE)
			&& (verify->ctrl_out_xfer.test_mask & verify->ep_test)) {
			usbh_verify_ctrl_process_tx(host);
		}
		//ctrl rx
		if ((verify->ctrl_in_xfer.state != VERIFY_STATE_IDLE)
			&& (verify->ctrl_in_xfer.test_mask & verify->ep_test)) {
			usbh_verify_ctrl_process_rx(host);
		}

		//loop all ep,do the loop
		for (ep_index = 0 ; ep_index < verify->ep_count; ep_index++) {
			if ((verify->ep_array[ep_index].state != VERIFY_STATE_IDLE)
				&& (PIPE_IS_VALID(verify->ep_array[ep_index].pipe_id))
				&& (verify->ep_array[ep_index].test_mask & verify->ep_test)) {
				usbh_verify_xfer_process(verify->ep_array + ep_index);
			}
		}
	}
	if (verify->loopcount > USBH_MAIN_PROCESS_MONITOR_COUNT) {
		verify->loopcount = 0;
		RTK_LOGS(TAG, "[VFY] Process running,nexttransfor %d, testmask 0x%08x\n", verify->next_transfor, verify->ep_test);
		usbh_verify_dump_testmask();
	}
	if (verify->next_transfor) {
		verify->next_transfor = 0 ;
		usbh_notify_class_state_change(host);
	} else {
		RTK_LOGS(TAG, "[VFY] Main Process switch to idle, transfor %d, testmask 0x%08x\n", verify->next_transfor, verify->ep_test);
		usbh_verify_dump_testmask();
		verify->start_process = 0 ;
	}

	return status;
}

static int usbh_verify_test_flag(usbh_verify_host_t *verify, usbh_verify_xfer_t *ep)
{
	u32 ep_test_old = verify->ep_all_mask;
	if (ep->test_mask == 0) {
		if (ep_test_old) {
			verify->ep_all_mask *= 2;
		}
		verify->ep_all_mask += 1;
		ep->test_mask = verify->ep_all_mask - ep_test_old;
	}
	verify->ep_test |= ep->test_mask;
	return HAL_OK;
}

static int usbh_verify_usb_status_check(void)
{
	usbh_verify_host_t *cdc = &usbh_verify_host;

	if (cdc->usbh_state < USBH_VERIFY_SETUP) {
		RTK_LOGS(TAG, "[VFY] USB not setup, wait\n");
		usb_os_sleep_ms(1000);
		return HAL_BUSY;
	}

	return HAL_OK;
}

static int usbh_verify_transmit_data(usbh_verify_xfer_t *ep)
{
	int ret = HAL_BUSY;
	usbh_verify_host_t *verify = &usbh_verify_host;
	if (usbh_verify_usb_status_check() != HAL_OK) {
		return ret;
	}

	if (ep->state == VERIFY_STATE_IDLE) {
		ep->state = VERIFY_STATE_XFER;
		usbh_verify_test_flag(verify, ep);
		ret = HAL_OK;
	} else {
		RTK_LOGS(TAG, "[VFY] %s out busy %d ep=%d\n", usbh_verify_get_xfer_type_text(ep), ep->state, ep->ep_addr);
		usb_os_sleep_ms(1);
	}

	return ret;
}

static int usbh_verify_receive_data(usbh_verify_xfer_t *ep)
{
	int ret = HAL_BUSY;
	usbh_verify_host_t *verify = &usbh_verify_host;

	if (usbh_verify_usb_status_check() != HAL_OK) {
		return ret;
	}

	if (ep->state == VERIFY_STATE_IDLE) {
		ep->state = VERIFY_STATE_XFER;
		usbh_verify_test_flag(verify, ep);
		ret = HAL_OK;
	} else {
		RTK_LOGS(TAG, "[VFY] %s in busy %d\n", usbh_verify_get_xfer_type_text(ep), ep->state);
		usb_os_sleep_ms(1);
	}

	return ret;
}

static void usbh_verify_ep_transfer(usbh_verify_xfer_t *ep, u16 size)
{
	usbh_verify_wait_enum_attach();

	ep->trans_size = size;
	usbh_verify_ep_enable(ep);
	//ep->error_cnt = 0 ;
	ep->xfer_cnt = 0 ;
	ep->idle_count = 0 ;
	if (USB_EP_IS_IN(ep->ep_addr)) {
		usbh_verify_receive_data(ep);
	} else {
		usbh_verify_transmit_data(ep);
	}
	usbh_verify_dump_ep_info(ep);
}

/* long run task*/
static void usbh_verify_longrun_thread(void *param)
{
	UNUSED(param);
	usbh_verify_host_t *verify = &usbh_verify_host;
	int loop = 0 ;
	usbh_verifty_test_task = 1;
	usbh_verify_wait_enum_attach();
	do {
		if (verify->start_process) {
			usb_os_sleep_ms(1000);
		} else {
			usbh_verify_transfer_by_eptype(10, loop);
			usbh_verify_start_process();
			loop ++;
			loop = loop % (USB_CH_EP_TYPE_INTR + 1);
		}
	} while (usbh_verifty_test_task == 1);
	usbh_verifty_test_task = 0;
	RTK_LOGS(TAG, "[VFY] Exit the long run task\n");
	rtos_task_delete(NULL);
}

/*dump status */
static void usbh_verify_dump_allep(void)
{
	usbh_verify_host_t *verify = &usbh_verify_host;
	usbh_verify_xfer_t *ep;
	u8 ep_index;

	RTK_LOGS(TAG, "[VFY] Out/In complete issue\n");

	ep = &verify->ctrl_out_xfer;
	RTK_LOGS(TAG, "[VFY] CTRL out done%d/err%d mask %08x\n", ep->total_xfer_cnt, ep->error_cnt, ep->test_mask);
	ep = &verify->ctrl_in_xfer;
	RTK_LOGS(TAG, "[VFY] CTRL in done%d/err%d mask%08x\n", ep->total_xfer_cnt, ep->error_cnt, ep->test_mask);

	/* Loop All EP */
	for (ep_index = 0 ; ep_index < verify->ep_count; ep_index++) {
		ep = verify->ep_array + ep_index;
		RTK_LOGS(TAG, "[VFY] \t%s%3s done%d/err%d", usbh_verify_get_xfer_type_text(ep),
				 (USB_EP_IS_IN(ep->ep_addr) ? ("IN") : ("OUT")),
				 ep->total_xfer_cnt, ep->error_cnt);

		RTK_LOGS(TAG, "[VFY] State %d mask %08x EP%02x/pipe %d MPS %d transize %d type %d interval %d\n",
				 ep->state, ep->test_mask, ep->ep_addr, ep->pipe_id,
				 ep->ep_mps, ep->trans_size, ep->ep_type, ep->ep_interval);
	}
}

static void usbh_verify_ep_status_thread(void *param)
{
	UNUSED(param);
	int loop = 0 ;
	usbh_verify_host_t *verify = &usbh_verify_host;
	usbh_verifty_status_task = 1;
	usbh_verify_wait_enum_attach();

	do {
		loop++;
		if (loop >= 3) {
			loop = 0 ;
			usbh_verify_dump_allep();
		}
		RTK_LOGS(TAG, "[VFY] Next_transfor %d, testmask 0x%08x\n", verify->next_transfor, verify->ep_test);
		rtos_time_delay_ms(5 * 1000); //1s*n
	} while (usbh_verifty_status_task == 1);
	usbh_verifty_status_task = 0;
	RTK_LOGS(TAG, "[VFY] Ep status task exit\n");
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

int usbh_verify_init(usbh_verify_cb_t *cb)
{
	int ret = HAL_ERR_PARA;
	usbh_verify_host_t *verify = &usbh_verify_host;

	usb_os_memset((void *)verify, 0x00, sizeof(usbh_verify_host_t));
	verify->test_count_limit = 100;//default
	verify->ctrl_transfer_idle = USBH_VERIFY_CTRL_IDLE;

	if (cb != NULL) {
		verify->cb = cb;
		ret = usbh_register_class(&usbh_verify_driver);
	}

	return ret;
}

/**
  * @brief  Deinit verify class
  * @retval Status
  */
int usbh_verify_deinit(void)
{
	usbh_verify_host_t *verify = &usbh_verify_host;
	usb_host_t *host = verify->host;
	u8 ep_index;

	if (verify->cb != NULL) {
		verify->cb = NULL;
	}

	if ((host != NULL) && (host->state == USBH_CLASS_READY)) {
		verify->usbh_state = USBH_VERIFY_IDLE;
		for (ep_index = 0 ; ep_index < verify->ep_count; ep_index++) {
			usbh_verify_ep_disable(host, verify->ep_array + ep_index);
		}
	}

	usbh_unregister_class(&usbh_verify_driver);

	return HAL_OK;
}

int usbh_verify_ep_enable(usbh_verify_xfer_t *xfer)
{
	int status = HAL_OK;
	u8 pipe_id;
	usbh_verify_host_t *verify = &usbh_verify_host;
	usb_host_t *host = verify->host;

	if (xfer->init_done == 1) {
		goto param_reinit;
	}

	pipe_id = usbh_alloc_pipe(host, xfer->ep_addr);
	if (PIPE_IS_VALID(pipe_id)) {
		xfer->pipe_id = pipe_id;
	} else {
		RTK_LOGS(TAG, "[VFY] Fail to alloc pipe for %s EP%02x\n", usbh_verify_get_xfer_type_text(xfer), xfer->ep_addr);
		status = HAL_ERR_MEM;
		goto exit;
	}

	xfer->xfer_len = xfer->trans_size;
	xfer->xfer_buf = (u8 *)usb_os_malloc(xfer->ep_mps);
	if (xfer->xfer_buf == NULL) {
		status = HAL_ERR_MEM;
		goto alloc_xfer_buf_fail_exit;
	}

	status = usbh_open_pipe(host, xfer->pipe_id, xfer->ep_addr, xfer->ep_type, xfer->ep_mps);
	if (status != HAL_OK) {
		status = HAL_ERR_HW;
		RTK_LOGS(TAG, "[VFY] Open pipe %02x fail\n", xfer->pipe_id);
		goto open_pipe_fail_exit;
	}
	xfer->init_done = 1;

param_reinit:
	usb_os_memset((void *)xfer->xfer_buf, TRANSFER_DATA_VALUE_BASE, xfer->ep_mps);
	xfer->xfer_bk_buf = xfer->xfer_buf;
	xfer->tick = 0;
	xfer->state = VERIFY_STATE_IDLE;
	xfer->expect_data = TRANSFER_DATA_VALUE_BASE;

	return status;

open_pipe_fail_exit:
	usb_os_mfree(xfer->xfer_buf);
	xfer->xfer_buf = NULL;
alloc_xfer_buf_fail_exit:
	usbh_free_pipe(host, xfer->pipe_id);
	xfer->pipe_id = USBH_PIPE_VALID_VALUE;
exit:
	if (status != HAL_OK) {
		RTK_LOGS(TAG, "[VFY] Ep enable fail %d\n", status);
	}
	return status;
}

void usbh_verify_ep_disable(usb_host_t *host, usbh_verify_xfer_t *xfer)
{
	if (PIPE_IS_VALID(xfer->pipe_id)) {
		usbh_close_pipe(host, xfer->pipe_id);
		usbh_free_pipe(host, xfer->pipe_id);
		xfer->pipe_id = USBH_PIPE_VALID_VALUE;
	}
	if (xfer->xfer_buf != NULL) {
		usb_os_mfree(xfer->xfer_buf);
		xfer->xfer_buf = NULL;
	}
	xfer->xfer_bk_buf = NULL;
	xfer->xfer_len = 0;
	xfer->init_done = 0;
}

void usbh_verify_start_process(void)
{
	usbh_verify_host_t *verify = &usbh_verify_host;
	usb_host_t *host = verify->host;
	if (host) {
		verify->finish_tick = 0;
		verify->start_process = 1;
		usbh_notify_class_state_change(host);
		//RTK_LOGS(TAG, "[VFY] Process start\n");
	}
}

void usbh_verify_wait_enum_attach(void)
{
	do {
		if (usbh_verify_usb_status_check() == HAL_OK) {
			break;
		}
	} while (1);
}

void usbh_verify_ctrl_transfer(u16 size, u8 is_out)
{
	usbh_verify_host_t *verify = &usbh_verify_host;
	usbh_verify_xfer_t *ep ;
	usbh_verify_wait_enum_attach();

	if (is_out) {
		ep = &verify->ctrl_out_xfer;
	} else {
		ep = &verify->ctrl_in_xfer;
	}
	ep->xfer_cnt = 0 ;
	ep->idle_count = 0 ;
	ep->expect_data = TRANSFER_DATA_VALUE_BASE;
	ep->trans_size = size;
	ep->xfer_len = size ;
	if (ep->xfer_buf) {
		usb_os_mfree(ep->xfer_buf);
	}
	ep->xfer_buf = (u8 *)usb_os_malloc(USBH_VERIFY_FIFO_SIZE_ALIGNED4B(ep->xfer_len));
	usb_os_memset((void *)ep->xfer_buf, TRANSFER_DATA_VALUE_BASE, ep->xfer_len);
	if (is_out) {
		usbh_verify_transmit_data(ep);
	} else {
		usbh_verify_receive_data(ep);
	}
}

void usbh_verify_transfer_by_eptype(u16 size, u8 type)
{
	u8 ep_index;
	usbh_verify_xfer_t *ep;
	usbh_verify_host_t *verify = &usbh_verify_host;

	if (type == USB_CH_EP_TYPE_CTRL) {
		ep = &(verify->ctrl_out_xfer);
		usbh_verify_ctrl_transfer(size, 1);

		ep = &(verify->ctrl_in_xfer);
		usbh_verify_ctrl_transfer(size, 0);
	} else {
		for (ep_index = 0 ; ep_index < verify->ep_count; ep_index++) {
			ep = verify->ep_array + ep_index;
			if (ep && USB_EP_IS_IN(ep->ep_addr) && (type == ep->ep_type)) {
				usbh_verify_ep_transfer(ep, size);
			}
		}
		for (ep_index = 0 ; ep_index < verify->ep_count; ep_index++) {
			ep = verify->ep_array + ep_index;
			if (ep && USB_EP_IS_OUT(ep->ep_addr) && (type == ep->ep_type)) {
				usbh_verify_ep_transfer(ep, size);
			}
		}
	}
}

void usbh_verify_allep_transfer(u16 size)
{
	u8 ep_index;
	usbh_verify_xfer_t *ep;
	usbh_verify_host_t *verify = &usbh_verify_host;

	usbh_verify_transfer_by_eptype(size, USB_CH_EP_TYPE_CTRL);

	for (ep_index = 0 ; ep_index < verify->ep_count; ep_index++) {
		ep = verify->ep_array + ep_index;
		if (ep && USB_EP_IS_IN(ep->ep_addr)) {
			usbh_verify_ep_transfer(ep, size);
		}
	}
	for (ep_index = 0 ; ep_index < verify->ep_count; ep_index++) {
		ep = verify->ep_array + ep_index;
		if (ep && USB_EP_IS_OUT(ep->ep_addr)) {
			usbh_verify_ep_transfer(ep, size);
		}
	}
}

void usbh_verify_longrun_thread_stop(void)
{
	if (usbh_verifty_test_task == 1) {
		usbh_verifty_test_task = 2;
	}
	do {
		RTK_LOGS(TAG, "[VFY] Host,w ait longrun task exit\n");
		if (usbh_verifty_test_task == 0) {
			break;
		} else {
			usb_os_sleep_ms(1000);
		}
	} while (usbh_verifty_test_task);
}

void usbh_verify_looprun_test(u16 size)
{
	usb_os_task_t task;
	int status;

	UNUSED(size);

	if (0 == usbh_verifty_test_task) {
		status = rtos_task_create(&task, "usbd_ep_thread", usbh_verify_longrun_thread, NULL, 1024U, 2U);
		if (status != SUCCESS) {
			RTK_LOGS(TAG, "[VFY] Fail to create USBH EP status task\n");
			usbh_verifty_test_task = 0;
		}
	}
}

void usbh_verify_ep_status_stop(void)
{
	if (usbh_verifty_status_task == 1) {
		usbh_verifty_status_task = 2;
	}
	do {
		if (usbh_verifty_status_task == 0) {
			break;
		} else {
			usb_os_sleep_ms(1000);
		}
	} while (usbh_verifty_status_task);
}

void usbh_verify_ep_status_start(void)
{
	usb_os_task_t task;
	u8 status;
	if (0 == usbh_verifty_status_task) {
		status = rtos_task_create(&task, "usbd_ep_thread", usbh_verify_ep_status_thread, NULL, 1024U, 2U);
		if (status != SUCCESS) {
			RTK_LOGS(TAG, "[VFY] Fail to create USBH EP status task\n");
			usbh_verifty_status_task = 0;
		}
	}
}

void usbh_verify_ep_debug_dump(void)
{
	usbh_verify_host_t *verify = &usbh_verify_host;

	RTK_LOGS(TAG, "[VFY] StartFlag %d\n", verify->start_process);
	RTK_LOGS(TAG, "[VFY] Test mask %d\n", verify->ep_test);
	RTK_LOGS(TAG, "[VFY] Next trans %d\n", verify->next_transfor);
	RTK_LOGS(TAG, "[VFY] Looptimes %d\n", verify->test_count_limit);
	RTK_LOGS(TAG, "[VFY] Ctrl status %d\n", verify->ctrl_transfer_idle);

	usbh_verify_dump_allep();
}
