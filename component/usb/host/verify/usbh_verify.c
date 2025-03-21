/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
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
static int usbh_verify_nak(usb_host_t *host, u8 pipe_id);
static int usbh_verify_usb_status_check(void);


static int usbh_verify_all_out_finish(void);
static inline void usbh_verify_success(usbh_verify_xfer_t *xfer);
/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "VFY";
static const char *TEXT_CTRL = "CTRL";
static const char *TEXT_BULK = "BULK";
static const char *TEXT_INTR = "INTR";
static const char *TEXT_ISOC = "ISOC";

/*
	0:idle
	1:running
	2:exit the task, switch to idle
*/
static __IO u8 usbh_verify_status_task = 0;
static __IO u8 usbh_verify_test_task = 0;
/* USB Standard Device Descriptor */
static usbh_class_driver_t usbh_verify_driver = {
	.class_code = VERIFY_CLASS_CODE,
	.attach = usbh_verify_attach,
	.detach = usbh_verify_detach,
	.setup = usbh_verify_setup,
	.process = usbh_verify_process,
	.sof = NULL,
	.nak = usbh_verify_nak,
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
	char *direction = (USB_EP_IS_IN(xfer->ep_addr)) ? "IN" : "OUT";
	RTK_LOGS(TAG, RTK_LOG_INFO, "Dump: %s-%s mask=0x%08x state=%d EP%02x/pipe=%d MPS=%d xlen=%d intv=%d\n",
			 xfer_type, direction, xfer->test_mask,
			 xfer->state, xfer->ep_addr, xfer->pipe_id,
			 xfer->ep_mps, xfer->trans_size, xfer->ep_interval);
#else
	UNUSED(xfer);
#endif
}

#if 0
static void usbh_verify_dump_buf(u8 *buf, u32 len)
{
	u32 i;
	if (len > DUMP_DATA_MAX_LEN) {
		len = DUMP_DATA_MAX_LEN;
	}
	for (i = 0; i < len;) {
		if (i + 10 <= len) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, " %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n", buf[i], buf[i + 1], buf[i + 2], buf[i + 3], buf[i + 4], buf[i + 5], buf[i + 6],
					 buf[i + 7],
					 buf[i + 8],
					 buf[i + 9]);
			i += 10;
		} else {
			for (; i < len; i++) {
				RTK_LOGS(NOTAG, RTK_LOG_INFO, " %3d ", buf[i]);
			}
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
			break;
		}
	}
}
#endif

static int usbh_verify_add_ep(usbh_ep_desc_t *ep_desc)
{
	u16 mps;
	usbh_verify_xfer_t *ep;
	u8 epaddr, ep_type, ep_index;
	usbh_verify_host_t *verify = &usbh_verify_host;

	if (verify->ep_count >= USBH_EP_COUNT_MAX) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Only support %d EPs\n", USBH_EP_COUNT_MAX);
		return HAL_ERR_MEM;
	}

	epaddr = ep_desc->bEndpointAddress;
	mps = ep_desc->wMaxPacketSize;
	ep_type = ep_desc->bmAttributes & USB_EP_XFER_TYPE_MASK;

	for (ep_index = 0; ep_index < verify->ep_count; ep_index++) {
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
	ep->ep_match_addr = 0x00;

	usbh_verify_dump_ep_info(ep);

	return HAL_OK;
}

static int usbh_verify_add_ep_bind(usbh_verify_xfer_t *ep)
{
	int ret = HAL_ERR_PARA;
	u8 ep_index;
	usbh_verify_xfer_t *ep_match;
	usbh_verify_host_t *verify = &usbh_verify_host;

	/* Loop All EP */
	for (ep_index = 0; ep_index < verify->ep_count; ep_index++) {
		ep_match = verify->ep_array + ep_index;
		/* Find another same type EP with different direction */
		if (ep_match && (ep_match->ep_type == ep->ep_type) && (USB_EP_IS_IN(ep->ep_addr) != USB_EP_IS_IN(ep_match->ep_addr))) {
			if ((ep_match->ep_match_addr == 0) && (ep->ep_match_addr == 0)) {
				ep->ep_match_addr = ep_match->ep_addr;
				ep_match->ep_match_addr = ep->ep_addr;
				RTK_LOGS(TAG, RTK_LOG_INFO, "Bind EP%02x and EP%02x together\n", ep->ep_addr, ep_match->ep_addr);
				ret = HAL_OK;
			}
		}
	}
	return ret;
}

static void usbh_verify_get_eps(usbh_if_desc_t *intf)
{
	u32 tmp = 0;
	usbh_ep_desc_t *ep_desc;
	u8 ep_index;
	usbh_verify_xfer_t *ep;
	usbh_verify_host_t *verify = &usbh_verify_host;

	if (intf->bNumEndpoints > USBH_EP_COUNT_MAX) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Too much EP cnt\n");
		intf->bNumEndpoints = USBH_EP_COUNT_MAX;
	}

	for (tmp = 0; tmp < intf->bNumEndpoints; tmp++) {
		ep_desc = &intf->ep_desc_array[tmp];
		usbh_verify_add_ep(ep_desc);
	}

	for (ep_index = 0; ep_index < verify->ep_count; ep_index++) {
		ep = verify->ep_array + ep_index;
		usbh_verify_add_ep_bind(ep);/* Match first same type EP with different direction */
	}
}

static int usbh_verify_all_out_finish(void)
{
	int ret = 1;
	if ((usbh_verify_xfer.p_in_only) || (usbh_verify_xfer.bulk_in_only)) {
		ret = 0;
	} else {
		u8 ep_index;
		usbh_verify_xfer_t *ep;
		usbh_verify_host_t *verify = &usbh_verify_host;
		if (verify->finish_tick) { //USBH_FORCE_IN_EXIT_SOF_COUNT_MAX tick count
			if ((verify->host->tick - verify->finish_tick) < USBH_FORCE_IN_EXIT_SOF_COUNT_MAX) {
				ret = 0;
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Force all USB TX finish tick(%d)\n", verify->host->tick);
			}
		} else {
			for (ep_index = 0; ep_index < verify->ep_count; ep_index++) {
				ep = verify->ep_array + ep_index;
				if (PIPE_IS_VALID(ep->pipe_id) && USB_EP_IS_OUT(ep->ep_addr) &&
					(ep->test_mask & verify->ep_mask) && (ep->xfer_cnt < verify->test_count_limit)) {
					ret = 0;
					break;
				}
			}
			//if ret =1 means all usb out finish,update the finish tick value
			if ((1 == ret) && (0 == verify->finish_tick)) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "All USB OUT transmit finish tick(%d)\n\n", verify->host->tick);
				verify->finish_tick = verify->host->tick;
				ret = 0;
			}
		}
	}
	return ret;
}

static inline void usbh_verify_success(usbh_verify_xfer_t *xfer)
{
	xfer->xfer_cnt ++;
	xfer->total_xfer_cnt ++;
}

static void usbh_verify_check_in_data(usbh_verify_xfer_t *rx_xfer)
{
	if ((rx_xfer) && (rx_xfer->xfer_len > 0)) {
		if (rx_xfer->expect_data != rx_xfer->xfer_buf[0]) {
			rx_xfer->error_cnt ++;
			RTK_LOGS(TAG, RTK_LOG_INFO, "%s err %d/want %d get %d\n", usbh_verify_get_xfer_type_text(rx_xfer),
					 rx_xfer->error_cnt, rx_xfer->expect_data, rx_xfer->xfer_buf[0]);
		}
		rx_xfer->expect_data = rx_xfer->xfer_buf[0] + 1;
	}
}

static void usbh_verify_xfer_dir_change(usbh_verify_xfer_t *xfer)
{
	usbh_verify_host_t *verify = &usbh_verify_host;
	u8 ep_index;
	usbh_verify_xfer_t *ep;

	/* Loopback: switch to peer-ep xfer. else IDLE */
	if (USB_EP_IS_OUT(xfer->ep_addr)) {
		usb_os_memset((void *)xfer->xfer_buf, (u8)(xfer->xfer_cnt & 0xFF), xfer->trans_size);
		if (xfer->ep_type == USB_CH_EP_TYPE_BULK) {
			if (usbh_verify_xfer.bulk_loopback) {
				xfer->ep_state = USBH_VERIFY_RX;
				for (ep_index = 0; ep_index < verify->ep_count; ep_index++) {
					ep = verify->ep_array + ep_index;
					if ((ep->ep_match_addr == xfer->ep_addr) && (ep->ep_type == xfer->ep_type)) {
						ep->ep_state = USBH_VERIFY_RX;
					}
				}
			} else {//usbh_verify_xfer.bulk_out_only
				xfer->ep_state = USBH_VERIFY_TRX_IDLE;
			}
		} else if ((xfer->ep_type == USB_CH_EP_TYPE_INTR) || (xfer->ep_type == USB_CH_EP_TYPE_ISOC)) {
			if (usbh_verify_xfer.p_loopback) {
				xfer->ep_state = USBH_VERIFY_RX;
				for (ep_index = 0; ep_index < verify->ep_count; ep_index++) {
					ep = verify->ep_array + ep_index;
					if ((ep->ep_match_addr == xfer->ep_addr) && (ep->ep_type == xfer->ep_type)) {
						ep->ep_state = USBH_VERIFY_RX;
					}
				}
			} else {//usbh_verify_xfer.p_out_only
				xfer->ep_state = USBH_VERIFY_TRX_IDLE;
			}
		}
	} else {//IN
		usb_os_memset((void *)xfer->xfer_buf, 0x00, xfer->trans_size);
		if (xfer->ep_type == USB_CH_EP_TYPE_BULK) {
			if (usbh_verify_xfer.bulk_loopback) {
				xfer->ep_state = USBH_VERIFY_TX;
				for (ep_index = 0; ep_index < verify->ep_count; ep_index++) {
					ep = verify->ep_array + ep_index;
					if ((ep->ep_match_addr == xfer->ep_addr) && (ep->ep_type == xfer->ep_type)) {
						ep->ep_state = USBH_VERIFY_TX;
					}
				}
			} else {//usbh_verify_xfer.bulk_in_only
				xfer->ep_state = USBH_VERIFY_TRX_IDLE;
			}
		} else if ((xfer->ep_type == USB_CH_EP_TYPE_INTR) || (xfer->ep_type == USB_CH_EP_TYPE_ISOC)) {
			if (usbh_verify_xfer.p_loopback) {
				xfer->ep_state = USBH_VERIFY_TX;
				for (ep_index = 0; ep_index < verify->ep_count; ep_index++) {
					ep = verify->ep_array + ep_index;
					if ((ep->ep_match_addr == xfer->ep_addr) && (ep->ep_type == xfer->ep_type)) {
						ep->ep_state = USBH_VERIFY_TX;
					}
				}
			} else {//usbh_verify_xfer.p_in_only
				xfer->ep_state = USBH_VERIFY_TRX_IDLE;
			}
		}
	}
}

static void usbh_verify_check_for_next_loop(usbh_verify_xfer_t *xfer)
{
	usbh_verify_host_t *verify = &usbh_verify_host;
	char *xfer_type = usbh_verify_get_xfer_type_text(xfer);
	char *direction = (USB_EP_IS_IN(xfer->ep_addr)) ? "IN" : "OUT";
	u8 ep_index;
	usbh_verify_xfer_t *ep;
	if (usbh_fpga_no_sof) {
		usb_host_t *host = verify->host;
		host->tick = DTimestamp_Get();
	}
	if (usbh_verify_all_out_finish() || (xfer->xfer_cnt >= verify->test_count_limit)) {
		xfer->state = VERIFY_STATE_IDLE;
		verify->ep_mask &= ~(xfer->test_mask);
		RTK_LOGS(TAG, RTK_LOG_INFO, "%s %s test finish %d/%d/%x\n", xfer_type, direction, xfer->xfer_cnt, verify->test_count_limit, verify->ep_mask);
		xfer->ep_state = USBH_VERIFY_TRX_IDLE;
		if ((usbh_verify_xfer.p_loopback) || (usbh_verify_xfer.bulk_loopback)) {
			for (ep_index = 0; ep_index < verify->ep_count; ep_index++) {
				ep = verify->ep_array + ep_index;
				if ((ep->ep_match_addr == xfer->ep_addr) && (ep->ep_type == xfer->ep_type)) {
					ep->ep_state = USBH_VERIFY_TRX_IDLE;
				}
			}
		}
	} else {
		xfer->xfer_len = xfer->trans_size;
		xfer->xfer_buf = xfer->xfer_bk_buf;
		xfer->state = VERIFY_STATE_XFER;
		usbh_verify_xfer_dir_change(xfer);
	}
}

static void usbh_verify_bulk_process_rx(usbh_verify_xfer_t *xfer)
{
	usbh_verify_host_t *verify = &usbh_verify_host;
	usb_host_t *host = verify->host;
	usbh_urb_state_t xfer_urb_state = USBH_URB_IDLE;
	usbh_xfer_func xfer_func = NULL;
	u32 rx_len;
	int status;

	xfer_func = usbh_verify_get_xfer_func(xfer);

	if ((xfer->ep_type == USB_CH_EP_TYPE_BULK) && (xfer->ep_state == USBH_VERIFY_TX)) {
		return;
	}
	if ((xfer->ep_type == USB_CH_EP_TYPE_BULK) && (xfer->ep_state == USBH_VERIFY_TRX_IDLE)) {
		xfer->ep_state = USBH_VERIFY_RX;
	}

	switch (xfer->state) {
	case VERIFY_STATE_XFER:
		status = xfer_func(host, xfer->xfer_buf, xfer->xfer_len, xfer->pipe_id);
		xfer->tick = host->tick;
		if (status == HAL_OK) {
			xfer->state = VERIFY_STATE_BUSY;
		}
		break;

	case VERIFY_STATE_BUSY:
		xfer_urb_state = usbh_get_urb_state(host, xfer->pipe_id);
		if (xfer_urb_state == USBH_URB_DONE) {
			xfer->retry_time = 0;
			rx_len = usbh_get_last_transfer_size(host, xfer->pipe_id);

			/* Handle the ZLP packet */
			if ((xfer->xfer_len > rx_len) && (xfer->xfer_len > xfer->ep_mps)) {
				xfer->xfer_len -= rx_len;
				xfer->xfer_buf += rx_len;
				xfer->state = VERIFY_STATE_XFER;
			} else if ((xfer->xfer_len > 0) && (xfer->xfer_len % xfer->ep_mps) == 0) {
				xfer->xfer_len = 0;/* Last ZLP for multi-MPS */
				xfer->state = VERIFY_STATE_XFER;
			} else {
				usbh_verify_success(xfer);
				usbh_verify_check_in_data(xfer);
				usbh_verify_check_for_next_loop(xfer);
			}
		} else if ((xfer_urb_state == USBH_URB_BUSY) || (xfer_urb_state == USBH_URB_IDLE)) {
			if (usbh_get_elapsed_ticks(host, xfer->tick) > 50) {
				xfer->retry_time++;
				if (xfer->retry_time < 5) {
					xfer->state = VERIFY_STATE_XFER;
				} else {
					xfer->retry_time = 0;
					xfer->xfer_cnt ++;
					usbh_verify_check_for_next_loop(xfer);
				}
			}
		} else if ((xfer_urb_state == USBH_URB_ERROR) || (xfer_urb_state == USBH_URB_STALL)) {
			xfer->state = VERIFY_STATE_IDLE;
		}
		break;

	default:
		break;
	}
}

static void usbh_verify_bulk_process_tx(usbh_verify_xfer_t *xfer)
{
	usbh_verify_host_t *verify = &usbh_verify_host;
	usb_host_t *host = verify->host;
	usbh_urb_state_t xfer_urb_state = USBH_URB_IDLE;
	usbh_xfer_func xfer_func = NULL;
	int status;

	xfer_func = usbh_verify_get_xfer_func(xfer);

	if ((xfer->ep_type == USB_CH_EP_TYPE_BULK) && (xfer->ep_state == USBH_VERIFY_RX)) {
		return;
	}
	if ((xfer->ep_type == USB_CH_EP_TYPE_BULK) && (xfer->ep_state == USBH_VERIFY_TRX_IDLE)) {
		xfer->ep_state = USBH_VERIFY_TX;
	}

	switch (xfer->state) {
	case VERIFY_STATE_XFER:
		status = xfer_func(host, xfer->xfer_buf, xfer->xfer_len, xfer->pipe_id);
		xfer->tick = host->tick;
		if (status == HAL_OK) {
			xfer->state = VERIFY_STATE_BUSY;
		}
		break;

	case VERIFY_STATE_BUSY:
		xfer_urb_state = usbh_get_urb_state(host, xfer->pipe_id);
		if (xfer_urb_state == USBH_URB_DONE) {
			xfer->retry_time = 0;
			if ((xfer->xfer_len > 0) && ((xfer->trans_size % xfer->ep_mps) == 0)) {
				xfer->xfer_len = 0;/* Last ZLP for multi-MPS */
				xfer->xfer_buf = NULL;
				xfer->state = VERIFY_STATE_XFER;
			} else {/* Short or ZLP done */
				usbh_verify_success(xfer);
				usbh_verify_check_for_next_loop(xfer);
			}
		} else if ((xfer_urb_state == USBH_URB_ERROR) || (xfer_urb_state == USBH_URB_STALL)) {
			xfer->state = VERIFY_STATE_IDLE;
		} else if ((xfer_urb_state == USBH_URB_BUSY) || (xfer_urb_state == USBH_URB_IDLE)) {
			if (usbh_get_elapsed_ticks(host, xfer->tick) >= 50) {
				xfer->retry_time++;
				if (xfer->retry_time < 5) {
					xfer->state = VERIFY_STATE_XFER;
				} else {
					xfer->retry_time = 0;
					xfer->xfer_cnt ++;
					usbh_verify_check_for_next_loop(xfer);
				}
			}
		}
		break;

	default:
		xfer->state = VERIFY_STATE_IDLE;
		break;
	}
}

static void usbh_verify_intr_process_rx(usbh_verify_xfer_t *xfer)
{
	usbh_verify_host_t *verify = &usbh_verify_host;
	usb_host_t *host = verify->host;
	usbh_urb_state_t xfer_urb_state = USBH_URB_IDLE;
	usbh_xfer_func xfer_func = NULL;
	int status;

	xfer_func = usbh_verify_get_xfer_func(xfer);

	if ((xfer->ep_type == USB_CH_EP_TYPE_INTR) && (xfer->ep_state == USBH_VERIFY_TX)) {
		return;
	}
	if ((xfer->ep_type == USB_CH_EP_TYPE_INTR) && (xfer->ep_state == USBH_VERIFY_TRX_IDLE)) {
		xfer->ep_state = USBH_VERIFY_RX;
	}

	switch (xfer->state) {
	case VERIFY_STATE_XFER:
		status = xfer_func(host, xfer->xfer_buf, xfer->xfer_len, xfer->pipe_id);
		if (usbh_fpga_no_sof) {
			host->tick = DTimestamp_Get();
		}
		xfer->tick = host->tick;
		if (status == HAL_OK) {
			xfer->state = VERIFY_STATE_BUSY;
		}
		break;

	case VERIFY_STATE_BUSY:
		xfer_urb_state = usbh_get_urb_state(host, xfer->pipe_id);

		if (xfer_urb_state == USBH_URB_DONE) {
			xfer->retry_time = 0;
			/* Handle the ZLP packet */
			if ((xfer->xfer_len > 0) && ((xfer->xfer_len % xfer->ep_mps) == 0)) {
				xfer->xfer_len = 0;/* Last ZLP for multi-MPS */
				xfer->state = VERIFY_STATE_XFER;
			} else {
				usbh_verify_success(xfer);
				usbh_verify_check_in_data(xfer);
				usbh_verify_check_for_next_loop(xfer);
			}
		} else if ((xfer_urb_state == USBH_URB_BUSY) || (xfer_urb_state == USBH_URB_IDLE)) {
			if (usbh_fpga_no_sof) {
				host->tick = DTimestamp_Get();
			}
			if ((usbh_fpga_no_sof && (((usbh_get_device_speed(host) == USB_SPEED_HIGH) && (usbh_get_elapsed_ticks(host, xfer->tick) > xfer->ep_interval * 250))
									  || ((usbh_get_device_speed(host) != USB_SPEED_HIGH) && (usbh_get_elapsed_ticks(host, xfer->tick) > xfer->ep_interval * 1000))))
				|| ((usbh_fpga_no_sof == 0) && (usbh_get_elapsed_ticks(host, xfer->tick) > xfer->ep_interval))) { /*Next interval req if NAK*/
				xfer->retry_time++;
				if (xfer->retry_time < 10) {
					xfer->state = VERIFY_STATE_XFER;
				} else {
					xfer->retry_time = 0;
					xfer->xfer_cnt ++;
					usbh_verify_check_for_next_loop(xfer);
				}
			}
		} else if ((xfer_urb_state == USBH_URB_ERROR) || (xfer_urb_state == USBH_URB_STALL)) {
			/*retry less than twice if RX data err*/
			xfer->state = VERIFY_STATE_IDLE;
		}
		break;

	default:
		break;
	}
}


static void usbh_verify_intr_process_tx(usbh_verify_xfer_t *xfer)
{
	usbh_verify_host_t *verify = &usbh_verify_host;
	usb_host_t *host = verify->host;
	usbh_urb_state_t xfer_urb_state = USBH_URB_IDLE;
	usbh_xfer_func xfer_func = NULL;
	int status;

	xfer_func = usbh_verify_get_xfer_func(xfer);

	if ((xfer->ep_type == USB_CH_EP_TYPE_INTR) && (xfer->ep_state == USBH_VERIFY_RX)) {
		return;
	}
	if ((xfer->ep_type == USB_CH_EP_TYPE_INTR) && (xfer->ep_state == USBH_VERIFY_TRX_IDLE)) {
		xfer->ep_state = USBH_VERIFY_TX;
	}

	switch (xfer->state) {
	case VERIFY_STATE_XFER:
		status = xfer_func(host, xfer->xfer_buf, xfer->xfer_len, xfer->pipe_id);
		if (usbh_fpga_no_sof) {
			host->tick = DTimestamp_Get();
		}
		xfer->tick = host->tick;
		if (status == HAL_OK) {
			xfer->state = VERIFY_STATE_BUSY;
		}
		break;

	case VERIFY_STATE_BUSY:
		xfer_urb_state = usbh_get_urb_state(host, xfer->pipe_id);
		if (xfer_urb_state == USBH_URB_DONE) {
			xfer->retry_time = 0;
			if ((xfer->xfer_len > 0) && ((xfer->trans_size % xfer->ep_mps) == 0)) {
				xfer->xfer_len = 0;/* Last ZLP for multi-MPS */
				xfer->xfer_buf = NULL;
				xfer->state = VERIFY_STATE_XFER;
			} else {
				usbh_verify_success(xfer);
				usbh_verify_check_for_next_loop(xfer);
			}
		} else if ((xfer_urb_state == USBH_URB_ERROR) || (xfer_urb_state == USBH_URB_STALL)) {
			xfer->state = VERIFY_STATE_IDLE;
		} else if ((xfer_urb_state == USBH_URB_BUSY) || (xfer_urb_state == USBH_URB_IDLE)) {
			/*If the endpoint NAKs a transaction during a microframe, the host controller
			must not issue further transactions for that endpoint until the next period.*/
			if (usbh_fpga_no_sof) {
				host->tick = DTimestamp_Get();
			}
			if ((usbh_fpga_no_sof && (((usbh_get_device_speed(host) == USB_SPEED_HIGH) && (usbh_get_elapsed_ticks(host, xfer->tick) > xfer->ep_interval * 125))
									  || ((usbh_get_device_speed(host) != USB_SPEED_HIGH) && (usbh_get_elapsed_ticks(host, xfer->tick) > xfer->ep_interval * 1000))))
				|| ((usbh_fpga_no_sof == 0) && (usbh_get_elapsed_ticks(host, xfer->tick) > xfer->ep_interval))) {
				xfer->retry_time ++;
				if (xfer->retry_time < 10) {
					xfer->state = VERIFY_STATE_XFER;
				} else {
					xfer->retry_time = 0;
					xfer->xfer_cnt ++;
					usbh_verify_check_for_next_loop(xfer);
				}
			}
		}
		break;

	default:
		xfer->state = VERIFY_STATE_IDLE;
		break;
	}
}

static void usbh_verify_isoc_process_rx(usbh_verify_xfer_t *xfer)
{
	usbh_verify_host_t *verify = &usbh_verify_host;
	usb_host_t *host = verify->host;
	usbh_urb_state_t xfer_urb_state = USBH_URB_IDLE;
	usbh_xfer_func xfer_func = NULL;
	u32 rx_len;
	int status;

	xfer_func = usbh_verify_get_xfer_func(xfer);

	if (xfer->ep_state == USBH_VERIFY_TX) {
		return;
	}
	if (xfer->ep_state == USBH_VERIFY_TRX_IDLE) {
		xfer->ep_state = USBH_VERIFY_RX;
	}

	switch (xfer->state) {
	case VERIFY_STATE_XFER:
		status = xfer_func(host, xfer->xfer_buf, xfer->xfer_len, xfer->pipe_id);
		xfer->tick = host->tick;
		if (status == HAL_OK) {
			xfer->state = VERIFY_STATE_BUSY;
		}
		break;

	case VERIFY_STATE_BUSY:
		xfer_urb_state = usbh_get_urb_state(host, xfer->pipe_id);
		if (xfer_urb_state == USBH_URB_DONE) {
			rx_len = usbh_get_last_transfer_size(host, xfer->pipe_id);
			if (rx_len == 0) {/* ISOC IN ZLP*/
				xfer->retry_time++;
				if (xfer->retry_time < 100) {
					xfer->state = VERIFY_STATE_XFER;
				} else {
					xfer->retry_time = 0;
					usbh_verify_check_for_next_loop(xfer);
				}
				break;
			} else {
				xfer->retry_time = 0;
				usbh_verify_success(xfer);
				usbh_verify_check_in_data(xfer);
				usbh_verify_check_for_next_loop(xfer);
			}
		} else if ((xfer_urb_state == USBH_URB_ERROR) || (xfer_urb_state == USBH_URB_STALL)) {
			xfer->state = VERIFY_STATE_IDLE;
		} else if ((xfer_urb_state == USBH_URB_BUSY) || (xfer_urb_state == USBH_URB_IDLE)) {
			/* ISOC not support retry */
			if (usbh_get_elapsed_ticks(host, xfer->tick) > 100) {
				xfer->retry_time = 0;
				xfer->xfer_cnt ++;
				usbh_verify_check_for_next_loop(xfer);
			}
		}
		break;
	default:
		xfer->state = VERIFY_STATE_IDLE;
		break;
	}
}


static void usbh_verify_isoc_process_tx(usbh_verify_xfer_t *xfer)
{
	usbh_verify_host_t *verify = &usbh_verify_host;
	usb_host_t *host = verify->host;
	usbh_urb_state_t xfer_urb_state = USBH_URB_IDLE;
	usbh_xfer_func xfer_func = NULL;
	u32 tx_len;
	int status;

	xfer_func = usbh_verify_get_xfer_func(xfer);

	if (xfer->ep_state == USBH_VERIFY_RX) {
		return;
	}
	if (xfer->ep_state == USBH_VERIFY_TRX_IDLE) {
		xfer->ep_state = USBH_VERIFY_TX;
	}

	switch (xfer->state) {
	case VERIFY_STATE_XFER:
		if (usbh_get_device_speed(host) == USB_SPEED_HIGH) {
			tx_len = xfer->xfer_len;
		} else {
			tx_len = (xfer->xfer_len >= xfer->ep_mps) ? xfer->ep_mps : xfer->xfer_len;
		}

		status = xfer_func(host, xfer->xfer_buf, tx_len, xfer->pipe_id);
		xfer->tick = host->tick;
		if (status == HAL_OK) {
			xfer->state = VERIFY_STATE_BUSY;
		}
		break;

	case VERIFY_STATE_BUSY:
		xfer_urb_state = usbh_get_urb_state(host, xfer->pipe_id);
		if (xfer_urb_state == USBH_URB_DONE) {
			if (usbh_get_device_speed(host) == USB_SPEED_HIGH) {
				usbh_verify_success(xfer);
				usbh_verify_check_for_next_loop(xfer);
			} else {
				if (xfer->xfer_len > xfer->ep_mps) {
					xfer->xfer_len -= xfer->ep_mps;
					xfer->xfer_buf += xfer->ep_mps;
					xfer->state = VERIFY_STATE_XFER;
				} else {
					usbh_verify_success(xfer);
					usbh_verify_check_for_next_loop(xfer);
				}
			}
		} else if ((xfer_urb_state == USBH_URB_ERROR) || (xfer_urb_state == USBH_URB_STALL)) {
			xfer->state = VERIFY_STATE_IDLE;
		} else if ((xfer_urb_state == USBH_URB_BUSY) || (xfer_urb_state == USBH_URB_IDLE)) {
			/* ISOC not support retry */
			if (usbh_get_elapsed_ticks(host, xfer->tick) > 100) {
				xfer->xfer_cnt++;
				usbh_verify_check_for_next_loop(xfer);
			}
		}
		break;
	default:
		xfer->state = VERIFY_STATE_IDLE;
		break;
	}
}

static void usbh_verify_ctrl_process_rx(usb_host_t *host)
{
	usbh_setup_req_t setup;
	int ret = HAL_BUSY;
	int loop_ctl = 1;
	usbh_verify_host_t *verify = &usbh_verify_host;
	usbh_verify_xfer_t *ep;
	ep = &(verify->ctrl_in_xfer);
	if (USBH_VERIFY_TX == verify->ctrl_trx_state) {
		return;
	}
	if (USBH_VERIFY_TRX_IDLE == verify->ctrl_trx_state) {
		verify->ctrl_trx_state = USBH_VERIFY_RX;
	}
	//RTK_LOGS(TAG, RTK_LOG_DEBUG, "CTRL in Type state=%d/len=%d\n", ep->state, ep->xfer_len);
	switch (ep->state) {
	case VERIFY_STATE_XFER:
		setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_VENDOR;
		setup.b.bRequest = USB_REQ_GET_DESCRIPTOR;
		setup.b.wValue = 0x0000U;
		setup.b.wIndex = 0x00U;
		setup.b.wLength = ep->xfer_len;
		ret = usbh_ctrl_request(host, &setup, ep->xfer_buf);
		//RTK_LOGS(TAG, RTK_LOG_DEBUG, "Enter ctrl RX state=%d/ret=%d\n", ep->state, ret);
		if (ret == HAL_OK) {
			usbh_verify_success(ep);
			if (usbh_verify_xfer.ctrl_in_only == 0) {
				usbh_verify_check_in_data(ep);
			}
			verify->ctrl_trx_state = USBH_VERIFY_TRX_IDLE;
			//RTK_LOGS(TAG, RTK_LOG_DEBUG, "CTRL RX xferlen=%d,count=%d/%d\n", ep->xfer_len, ep->xfer_cnt, verify->test_count_limit);
			//usbh_verify_dump_buf(ep->xfer_buf, ep->xfer_len);
			usb_os_memset((void *)ep->xfer_buf, (u8)ep->xfer_cnt, ep->xfer_len);
			if (ep->xfer_cnt >= verify->test_count_limit) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "CTRL RX test finish(%d/%d/%x)\n", ep->xfer_cnt, verify->test_count_limit, verify->ep_mask);
				ep->state = VERIFY_STATE_IDLE;
				verify->ep_mask &= ~(ep->test_mask);
				loop_ctl = 0;
			} else {
				ep->state = VERIFY_STATE_XFER;
			}

		} else if (ret != HAL_BUSY) {
			ep->state = VERIFY_STATE_ERROR;
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to TX CTRL req, ret %d\n", ret);
		}
		break;

	case VERIFY_STATE_ERROR:
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Err, finish RX Test %d/%d\n", ep->xfer_cnt, verify->test_count_limit);
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
		verify->next_xfer = 1;
	}
}

static void usbh_verify_ctrl_process_tx(usb_host_t *host)
{
	usbh_setup_req_t setup;
	u8 ret = HAL_BUSY;
	u8 loop_ctl = 1;
	usbh_verify_host_t *verify = &usbh_verify_host;
	usbh_verify_xfer_t *ep;
	ep = &(verify->ctrl_out_xfer);
	if (USBH_VERIFY_RX == verify->ctrl_trx_state) {
		return;
	}
	if (USBH_VERIFY_TRX_IDLE == verify->ctrl_trx_state) {
		verify->ctrl_trx_state = USBH_VERIFY_TX;
	}

	switch (ep->state) {
	case VERIFY_STATE_XFER:
		setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_VENDOR;
		setup.b.bRequest = VERIFY_CTRL_REQUEST_TYPE;
		setup.b.wValue = 0x0000U;
		setup.b.wIndex = 0x00U;
		setup.b.wLength = ep->xfer_len;
		ret = usbh_ctrl_request(host, &setup, ep->xfer_buf);
		if (ret == HAL_OK) {
			usbh_verify_success(ep);
			verify->ctrl_trx_state = USBH_VERIFY_TRX_IDLE;
			//RTK_LOGS(TAG, RTK_LOG_DEBUG, "Xferlen %d count %d/%d\n", ep->xfer_len, ep->xfer_cnt, verify->test_count_limit);
			usb_os_memset((void *)ep->xfer_buf, (u8)ep->xfer_cnt, ep->xfer_len);
			if (ep->xfer_cnt >= verify->test_count_limit) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "CTRL TX test finish %d/%d/%x\n", ep->xfer_cnt, verify->test_count_limit, verify->ep_mask);
				ep->state = VERIFY_STATE_IDLE;
				verify->ep_mask &= ~(ep->test_mask);
				loop_ctl = 0;
			} else {
				ep->state = VERIFY_STATE_XFER;
			}
		} else if (ret != HAL_BUSY) {
			ep->state = VERIFY_STATE_ERROR;
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to TX CTRL req, ret %d\n", ret);
		}
		break;

	case VERIFY_STATE_ERROR:
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Err, finish TX test %d/%d\n", ep->xfer_cnt, verify->test_count_limit);
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
		verify->next_xfer = 1;
	}
}

int usbh_verify_xfer_process(usbh_verify_xfer_t *xfer)
{
	usbh_xfer_func xfer_func = NULL;
	usbh_verify_host_t *verify = &usbh_verify_host;

	xfer_func = usbh_verify_get_xfer_func(xfer);
	if (NULL == xfer_func) {
		return HAL_OK;
	}

	if (USB_EP_IS_OUT(xfer->ep_addr)) {
		switch (xfer->ep_type) {
		case USB_CH_EP_TYPE_BULK:
			usbh_verify_bulk_process_tx(xfer);
			break;
		case USB_CH_EP_TYPE_INTR:
			usbh_verify_intr_process_tx(xfer);
			break;
		case USB_CH_EP_TYPE_ISOC:
			usbh_verify_isoc_process_tx(xfer);
			break;
		}
	} else {
		switch (xfer->ep_type) {
		case USB_CH_EP_TYPE_BULK:
			usbh_verify_bulk_process_rx(xfer);
			break;
		case USB_CH_EP_TYPE_INTR:
			usbh_verify_intr_process_rx(xfer);
			break;
		case USB_CH_EP_TYPE_ISOC:
			usbh_verify_isoc_process_rx(xfer);
			break;
		}
	}
	verify->next_xfer = 1;
	return HAL_OK;
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
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to TX Ctrl req, ret %d\n", ret);
			return ret;
		}
	} while (1);

	verify_if_desc = usbh_get_interface_descriptor(host, verify->interface_id, verify->interface_id);
	if (verify_if_desc == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to get itf desc");
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
#endif

/**
  * @brief  Usb nak callback function.
  * @param  host: Host handle
  * @param  pipe_id: Pipe number
  * @retval return OK while success, else return HAL_ERR_UNKNOWN
  */
static int usbh_verify_nak(usb_host_t *host, u8 pipe_id)
{
	u8 ep_type;
	if (pipe_id > USB_MAX_PIPES) {
		return HAL_ERR_PARA;
	}

	ep_type = usbh_get_ep_type(host, pipe_id);
	//RTK_LOGS(TAG, RTK_LOG_INFO, "nak type=%d/pipe_id=%d\n",ep_type,pipe_id);
	if (ep_type == USB_CH_EP_TYPE_INTR) {
		/* Do not retransmit in this loop, it will do in next SOF */
		return HAL_OK;
	}

	return HAL_ERR_UNKNOWN;
}


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
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to find itf");
		return HAL_ERR_PARA;
	}
	verify->host = host;

	status = usbh_verify_switch_interface(interface_id, 0);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to set itf");
		return HAL_ERR_UNKNOWN;
	}

	usbh_if_desc = usbh_get_interface_descriptor(host, interface_id, 0);
	if (usbh_if_desc == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to get itf desc\n");
		return HAL_ERR_PARA;
	}

	/* Get data in/out endpoints */
	usbh_verify_get_eps(usbh_if_desc);

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
static void usbh_verify_dump_test_mask(void)
{
	u8 ep_index;
	usbh_verify_host_t *verify = &usbh_verify_host;
	usbh_verify_xfer_t *xfer;
	if (verify->ep_mask) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Ep flag=0x%08x\n", verify->ep_mask);
		for (ep_index = 0; ep_index < verify->ep_count; ep_index++) {
			xfer = verify->ep_array + ep_index;
			if (xfer->test_mask & verify->ep_mask) {
				RTK_LOGS(NOTAG, RTK_LOG_DEBUG, "%s %s-0x%08x", usbh_verify_get_xfer_type_text(xfer), (USB_EP_IS_IN(xfer->ep_addr) ? "IN" : "OUT"), xfer->test_mask);
			}
		}
		RTK_LOGS(NOTAG, RTK_LOG_DEBUG, "\n");
	}
}

static int usbh_verify_process(usb_host_t *host)
{
	int status = HAL_OK;
	u8 ep_index;
	usbh_verify_host_t *verify = &usbh_verify_host;
	verify->loopcount ++;
	if (0 == verify->start_flag) {
		usb_os_sleep_ms(1000);
		if (verify->loopcount % 60 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Idle: test_mask 0x%08x\n", verify->ep_mask);
		}
		return status;
	}

	if (verify->ep_mask) {
		if ((verify->ctrl_out_xfer.state != VERIFY_STATE_IDLE)
			&& (verify->ctrl_out_xfer.test_mask & verify->ep_mask) && usbh_verify_xfer.ctrl_out_only) {
			usbh_verify_ctrl_process_tx(host);
		} else if ((verify->ctrl_in_xfer.state != VERIFY_STATE_IDLE)
				   && (verify->ctrl_in_xfer.test_mask & verify->ep_mask) && usbh_verify_xfer.ctrl_in_only) {
			usbh_verify_ctrl_process_rx(host);
		} else {
			//ctrl tx
			usb_os_delay_us(200);//To avoid ctrl halt issue
			if ((verify->ctrl_out_xfer.state != VERIFY_STATE_IDLE)
				&& (verify->ctrl_out_xfer.test_mask & verify->ep_mask)) {
				usbh_verify_ctrl_process_tx(host);
			}
			usb_os_delay_us(200);//To avoid ctrl halt issue
			//ctrl rx
			if ((verify->ctrl_in_xfer.state != VERIFY_STATE_IDLE)
				&& (verify->ctrl_in_xfer.test_mask & verify->ep_mask)) {
				usbh_verify_ctrl_process_rx(host);
			}
		}

		//loop all ep,do the loop
		for (ep_index = 0; ep_index < verify->ep_count; ep_index++) {
			if ((verify->ep_array[ep_index].state != VERIFY_STATE_IDLE)
				&& (PIPE_IS_VALID(verify->ep_array[ep_index].pipe_id))
				&& (verify->ep_array[ep_index].test_mask & verify->ep_mask)) {
				usbh_verify_xfer_process(verify->ep_array + ep_index);
			}
		}
	}
	if (verify->loopcount > USBH_MAIN_PROCESS_MONITOR_COUNT) {
		verify->loopcount = 0;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Process: next_xfer %d, test_mask 0x%08x\n", verify->next_xfer, verify->ep_mask);
		usbh_verify_dump_test_mask();
	}
	if (verify->next_xfer) {
		verify->next_xfer = 0;
		usbh_notify_class_state_change(host);
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Idle: next_xfer %d, test_mask 0x%08x\n", verify->next_xfer, verify->ep_mask);
		usbh_verify_dump_test_mask();
		verify->start_flag = 0;
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
	verify->ep_mask |= ep->test_mask;
	return HAL_OK;
}

static int usbh_verify_usb_status_check(void)
{
	usbh_verify_host_t *cdc = &usbh_verify_host;

	if (cdc->usbh_state < USBH_VERIFY_SETUP) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB not setup, wait\n");
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "%s out busy %d ep=%d\n", usbh_verify_get_xfer_type_text(ep), ep->state, ep->ep_addr);
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "%s in busy %d\n", usbh_verify_get_xfer_type_text(ep), ep->state);
		usb_os_sleep_ms(1);
	}

	return ret;
}

static void usbh_verify_ep_transfer(usbh_verify_xfer_t *ep, u16 size)
{
	usbh_verify_wait_enum_attach();

	ep->trans_size = size;
	usbh_verify_ep_enable(ep);
	//ep->error_cnt = 0;
	ep->xfer_cnt = 0;
	ep->idle_count = 0;
	if (USB_EP_IS_IN(ep->ep_addr)) {
		if (((usbh_verify_xfer.p_out_only) && ((USB_CH_EP_TYPE_ISOC == ep->ep_type) || (USB_CH_EP_TYPE_INTR == ep->ep_type)))
			|| ((usbh_verify_xfer.bulk_out_only) && (USB_CH_EP_TYPE_BULK == ep->ep_type))) {
			return;
		}

		usbh_verify_receive_data(ep);
	} else {
		if (((usbh_verify_xfer.p_in_only) && ((USB_CH_EP_TYPE_ISOC == ep->ep_type) || (USB_CH_EP_TYPE_INTR == ep->ep_type)))
			|| ((usbh_verify_xfer.bulk_in_only) && (USB_CH_EP_TYPE_BULK == ep->ep_type))) {
			return;
		}
		usbh_verify_transmit_data(ep);
	}
	usbh_verify_dump_ep_info(ep);
}

/* long run task*/
static void usbh_verify_longrun_thread(void *param)
{
	UNUSED(param);
	usbh_verify_host_t *verify = &usbh_verify_host;
	int loop = 0;
	usbh_verify_test_task = 1;
	usbh_verify_wait_enum_attach();
	do {
		if (verify->start_flag) {
			usb_os_sleep_ms(1000);
		} else {
			usbh_verify_transfer_by_eptype(10, loop);
			usbh_verify_start_process();
			loop ++;
			loop = loop % (USB_CH_EP_TYPE_INTR + 1);
		}
	} while (usbh_verify_test_task == 1);
	usbh_verify_test_task = 0;
	RTK_LOGS(TAG, RTK_LOG_INFO, "Exit long run task\n");
	rtos_task_delete(NULL);
}

/*dump status */
static void usbh_verify_dump_all_ep(void)
{
	usbh_verify_host_t *verify = &usbh_verify_host;
	usbh_verify_xfer_t *ep;
	u8 ep_index;

	RTK_LOGS(TAG, RTK_LOG_INFO, "OUT/IN complete issue\n");

	ep = &verify->ctrl_out_xfer;
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tCTRL OUT done%d/err%d mask %08x len %d\n", ep->total_xfer_cnt, ep->error_cnt, ep->test_mask, ep->xfer_len);
	ep = &verify->ctrl_in_xfer;
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tCTRL  IN done%d/err%d mask %08x len %d\n", ep->total_xfer_cnt, ep->error_cnt, ep->test_mask, ep->xfer_len);

	/* Loop All EP */
	for (ep_index = 0; ep_index < verify->ep_count; ep_index++) {
		ep = verify->ep_array + ep_index;
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\t%s %3s done%d/err%d mask %08x EP%02x/pipe %d MPS %d xlen %d intv %d\n", usbh_verify_get_xfer_type_text(ep),
				 (USB_EP_IS_IN(ep->ep_addr) ? "IN" : "OUT"),
				 ep->total_xfer_cnt, ep->error_cnt,
				 ep->test_mask, ep->ep_addr, ep->pipe_id,
				 ep->ep_mps, ep->trans_size, ep->ep_interval);
	}
}

static void usbh_verify_ep_status_thread(void *param)
{
	UNUSED(param);
	int loop = 0;
	usbh_verify_host_t *verify = &usbh_verify_host;
	usbh_verify_status_task = 1;
	usbh_verify_wait_enum_attach();

	do {
		loop++;
		if (loop >= 3) {
			loop = 0;
			usbh_verify_dump_all_ep();
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "next_xfer %d, test_mask 0x%08x\n", verify->next_xfer, verify->ep_mask);
		rtos_time_delay_ms(5 * 1000); //1s*n
	} while (usbh_verify_status_task == 1);
	usbh_verify_status_task = 0;
	RTK_LOGS(TAG, RTK_LOG_INFO, "Ep status task exit\n");
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

int usbh_verify_init(usbh_verify_cb_t *cb)
{
	int ret = HAL_ERR_PARA;
	usbh_verify_host_t *verify = &usbh_verify_host;

	usb_os_memset((void *)verify, 0x00, sizeof(usbh_verify_host_t));
	verify->test_count_limit = 100;//default
	usbh_verify_xfer.p_loopback = 1;
	usbh_verify_xfer.bulk_loopback = 1;
	verify->ctrl_trx_state = USBH_VERIFY_TRX_IDLE;

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
		for (ep_index = 0; ep_index < verify->ep_count; ep_index++) {
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
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to alloc pipe for %s EP%02x\n", usbh_verify_get_xfer_type_text(xfer), xfer->ep_addr);
		status = HAL_ERR_MEM;
		goto exit;
	}

	status = usbh_open_pipe(host, xfer->pipe_id, xfer->ep_addr, xfer->ep_type, xfer->ep_mps);
	if (status != HAL_OK) {
		status = HAL_ERR_HW;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Open pipe %02x fail\n", xfer->pipe_id);
		goto open_pipe_fail_exit;
	}

param_reinit:
	xfer->xfer_len = xfer->trans_size;
	if (xfer->xfer_buf) {
		usb_os_mfree(xfer->xfer_buf);
		xfer->xfer_buf = NULL;
	}

	xfer->xfer_buf = (u8 *)usb_os_malloc(xfer->xfer_len);
	if (xfer->xfer_buf == NULL) {
		status = HAL_ERR_MEM;
		goto alloc_xfer_buf_fail_exit;
	}
	xfer->init_done = 1;/* Only init pipe once, paras can re-init */

	usb_os_memset((void *)xfer->xfer_buf, TRANSFER_DATA_VALUE_BASE, xfer->xfer_len);
	xfer->xfer_bk_buf = xfer->xfer_buf;
	xfer->tick = 0;
	xfer->state = VERIFY_STATE_IDLE;
	xfer->expect_data = TRANSFER_DATA_VALUE_BASE;
	xfer->retry_time = 0;
	xfer->ep_state = USBH_VERIFY_TRX_IDLE;

	return status;

alloc_xfer_buf_fail_exit:
	usb_os_mfree(xfer->xfer_buf);
	xfer->xfer_buf = NULL;
open_pipe_fail_exit:
	usbh_free_pipe(host, xfer->pipe_id);
	xfer->pipe_id = USBH_PIPE_VALID_VALUE;
exit:
	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Ep enable fail %d\n", status);
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
		verify->start_flag = 1;
		usbh_notify_class_state_change(host);
		//RTK_LOGS(TAG, RTK_LOG_INFO, "Process start\n");
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
	usbh_verify_xfer_t *ep;
	usbh_verify_wait_enum_attach();

	if (is_out) {
		ep = &verify->ctrl_out_xfer;
	} else {
		ep = &verify->ctrl_in_xfer;
	}
	ep->xfer_cnt = 0;
	ep->idle_count = 0;
	ep->expect_data = TRANSFER_DATA_VALUE_BASE;
	ep->trans_size = size;
	ep->xfer_len = size;
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
		if (usbh_verify_xfer.ctrl_out_only) {
			ep = &(verify->ctrl_out_xfer);
			usbh_verify_ctrl_transfer(size, 1);
		} else if (usbh_verify_xfer.ctrl_in_only) {
			ep = &(verify->ctrl_in_xfer);
			usbh_verify_ctrl_transfer(size, 0);
		} else {
			usb_os_delay_us(200);//To avoid ctrl halt issue
			ep = &(verify->ctrl_out_xfer);
			usbh_verify_ctrl_transfer(size, 1);
			usb_os_delay_us(200);//To avoid ctrl halt issue
			ep = &(verify->ctrl_in_xfer);
			usbh_verify_ctrl_transfer(size, 0);
		}
	} else {
		for (ep_index = 0; ep_index < verify->ep_count; ep_index++) {
			ep = verify->ep_array + ep_index;
			if (ep && USB_EP_IS_IN(ep->ep_addr) && (type == ep->ep_type)) {
				usbh_verify_ep_transfer(ep, size);
			}
		}
		for (ep_index = 0; ep_index < verify->ep_count; ep_index++) {
			ep = verify->ep_array + ep_index;
			if (ep && USB_EP_IS_OUT(ep->ep_addr) && (type == ep->ep_type)) {
				usbh_verify_ep_transfer(ep, size);
			}
		}
	}
}

void usbh_verify_all_ep_transfer(u16 size)
{
	u8 ep_index;
	usbh_verify_xfer_t *ep;
	usbh_verify_host_t *verify = &usbh_verify_host;

	usbh_verify_transfer_by_eptype(size, USB_CH_EP_TYPE_CTRL);

	for (ep_index = 0; ep_index < verify->ep_count; ep_index++) {
		ep = verify->ep_array + ep_index;
		if (ep && USB_EP_IS_IN(ep->ep_addr)) {
			usbh_verify_ep_transfer(ep, size);
		}
	}
	for (ep_index = 0; ep_index < verify->ep_count; ep_index++) {
		ep = verify->ep_array + ep_index;
		if (ep && USB_EP_IS_OUT(ep->ep_addr)) {
			usbh_verify_ep_transfer(ep, size);
		}
	}
}

void usbh_verify_longrun_thread_stop(void)
{
	if (usbh_verify_test_task == 1) {
		usbh_verify_test_task = 2;
	}
	do {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Host,wait longrun task exit\n");
		if (usbh_verify_test_task == 0) {
			break;
		} else {
			usb_os_sleep_ms(1000);
		}
	} while (usbh_verify_test_task);
}

void usbh_verify_looprun_test(u16 size)
{
	usb_os_task_t task;
	int status;

	UNUSED(size);

	if (0 == usbh_verify_test_task) {
		status = rtos_task_create(&task, "usbd_ep_thread", usbh_verify_longrun_thread, NULL, 1024U, 2U);
		if (status != SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to create USBH longrun task\n");
			usbh_verify_test_task = 0;
		}
	}
}

void usbh_verify_ep_status_stop(void)
{
	if (usbh_verify_status_task == 1) {
		usbh_verify_status_task = 2;
	}
	do {
		if (usbh_verify_status_task == 0) {
			break;
		} else {
			usb_os_sleep_ms(1000);
		}
	} while (usbh_verify_status_task);
}

void usbh_verify_ep_status_start(void)
{
	usb_os_task_t task;
	u8 status;
	if (0 == usbh_verify_status_task) {
		status = rtos_task_create(&task, "usbd_ep_thread", usbh_verify_ep_status_thread, NULL, 1024U, 2U);
		if (status != SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to create USBH status task\n");
			usbh_verify_status_task = 0;
		}
	}
}

void usbh_verify_ep_debug_dump(void)
{
	usbh_verify_host_t *verify = &usbh_verify_host;
	u8 ep_index;
	usbh_verify_xfer_t *ep;

	RTK_LOGS(TAG, RTK_LOG_INFO, "StartFlag %d\n", verify->start_flag);
	RTK_LOGS(TAG, RTK_LOG_INFO, "TestMask %d\n", verify->ep_mask);
	RTK_LOGS(TAG, RTK_LOG_INFO, "NextXfer %d\n", verify->next_xfer);
	RTK_LOGS(TAG, RTK_LOG_INFO, "LoopCount %d\n", verify->test_count_limit);
	RTK_LOGS(TAG, RTK_LOG_INFO, "CtrlState %d\n", verify->ctrl_trx_state);

	for (ep_index = 0; ep_index < verify->ep_count; ep_index++) {
		ep = verify->ep_array + ep_index;
		RTK_LOGS(TAG, RTK_LOG_INFO, "EP%02x %s state %d, xfer %d, cnt %d\n", ep->ep_addr,
				 ((USB_EP_IS_IN(ep->ep_addr)) ? "IN" : "OUT"), ep->ep_state, ep->state, ep->xfer_cnt);
	}
	usbh_verify_dump_all_ep();
}
