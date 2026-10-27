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
static void usbh_vendor_detach(usb_host_t *host);
static void usbh_vendor_process(usb_host_t *host, usbh_drv_msg_t *msg);
static int usbh_vendor_setup(usb_host_t *host);
static void usbh_vendor_deinit_all_pipe(void);
static void usbh_vendor_sof(usb_host_t *host);

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
static const usbh_class_driver_t usbh_vendor_driver = {
	.id_table = vendor_devs,
	.attach = usbh_vendor_attach,
	.detach = usbh_vendor_detach,
	.setup = usbh_vendor_setup,
	.process = usbh_vendor_process,
	.sof = usbh_vendor_sof,
};

static usbh_vendor_host_t usbh_vendor_host;

static const char *const TEXT_CTRL = "CTRL";
static const char *const TEXT_BULK = "BULK";
static const char *const TEXT_INTR = "INTR";
static const char *const TEXT_ISOC = "ISOC";

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Get transfer type text.
  * @param  ep_type: Endpoint transfer type, `USB_CH_EP_TYPE_XXX`.
  * @retval The type text.
  */
static const char *usbh_get_transfer_type_text(u8 ep_type)
{
	const char *text = NULL;

	switch (ep_type) {
	case USB_CH_EP_TYPE_CTRL:
		text = TEXT_CTRL;
		break;
	case USB_CH_EP_TYPE_BULK:
		text = TEXT_BULK;
		break;
	case USB_CH_EP_TYPE_INTR:
		text = TEXT_INTR;
		break;
	case USB_CH_EP_TYPE_ISOC:
		text = TEXT_ISOC;
		break;
	default:
		break;
	}

	return text;
}

/**
  * @brief  Get endpoint details.
  * @note   The class owns one pipe per transfer type and direction. A second endpoint of the
  *         same kind is skipped: opening it would overwrite the first pipe's handle and leak
  *         the host channel already bound to it until the device is unplugged.
  * @param  host: Host handle.
  * @param  intf: Interface descriptor.
  * @retval Status.
  */
static int usbh_vendor_get_endpoints(usb_host_t *host, usbh_itf_desc_t *intf)
{
	u32 tmp = 0;
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_ep_desc_t *ep_desc;
	usbh_vendor_xfer_t *xfer;
	u8 ep_type;
	u8 ep_in;
	const char *xfer_type;

	for (tmp = 0; tmp < intf->bNumEndpoints; tmp++) {
		ep_desc = &intf->ep_desc_array[tmp];
		ep_type = ep_desc->bmAttributes & USB_EP_XFER_TYPE_MASK;
		ep_in = USB_EP_IS_IN(ep_desc->bEndpointAddress);
		xfer_type = usbh_get_transfer_type_text(ep_type);
		xfer = NULL;

		switch (ep_type) {
		case USB_CH_EP_TYPE_BULK:
			xfer = (ep_in != 0U) ? &vendor->bulk_in_xfer : &vendor->bulk_out_xfer;
			break;
		case USB_CH_EP_TYPE_INTR:
			xfer = (ep_in != 0U) ? &vendor->intr_in_xfer : &vendor->intr_out_xfer;
			break;
		case USB_CH_EP_TYPE_ISOC:
			xfer = (ep_in != 0U) ? &vendor->isoc_in_xfer : &vendor->isoc_out_xfer;
			break;
		default:
			break;
		}

		if (xfer == NULL) {
			continue;
		}

		if (xfer->pipe.pipe_num != 0U) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Skip extra %s EP%02x\n", xfer_type, ep_desc->bEndpointAddress);
			continue;
		}

		if (usbh_open_pipe(host, &xfer->pipe, ep_desc, &usbh_vendor_driver) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Open %s pipe fail\n", xfer_type);
			usbh_vendor_deinit_all_pipe();
			return HAL_ERR_PARA;
		}
		xfer->pipe.max_timeout_tick = VENDOR_XFER_MAX_TIMEOUT_TICK;
		RTK_LOGS(TAG, RTK_LOG_INFO, "%s EP%02x MPS %d intv %d\n", xfer_type, xfer->pipe.ep_addr, xfer->pipe.ep_mps, xfer->pipe.ep_interval);
	}

	return HAL_OK;
}

/**
  * @brief  Release a transfer claimed by the API.
  * @note   Taken under the critical section so the stores that finished the transfer are
  *         visible to a caller on the other core before it can claim the pipe again.
  * @param  xfer: Transfer structure.
  * @retval None.
  */
static void usbh_vendor_release(usbh_vendor_xfer_t *xfer)
{
	usb_os_enter_critical();
	xfer->busy = 0U;
	usb_os_exit_critical();
}

#if USBH_VENDOR_DEBUG
/**
  * @brief  Dump buffer contents.
  * @note   Only called from task context, refer to usbh_vendor_report_finish.
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
  * @brief  Report a finished ISOC test loop.
  * @note   Called from usbh_vendor_process (task context) only. The ISOC OUT loop finishes in
  *         usbh_vendor_sof(), an ISR callback which must not do lengthy work: one buffer dump
  *         is test_cnt/10 UART lines and would block the USB ISR for tens of ms at 115200
  *         baud, losing SOF/channel interrupts and wedging the in-flight ISOC/INTR pipes.
  *         So the finish only latches xfer_done and the logging happens here. A new run of the
  *         same pipe is refused until then, so the dumped test_buf can not be swapped under it.
  * @param  xfer: Pipe xfer
  * @retval None
  */
static void usbh_vendor_report_finish(usbh_vendor_xfer_t *xfer)
{
	usbh_pipe_t *pipe = &xfer->pipe;
	u8 done;

	usb_os_enter_critical();
	done = xfer->xfer_done;
	usb_os_exit_critical();
	if (done == 0U) {
		return;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "ISOC %s test finish %d/%d: \n", (pipe->ep_is_in ? ("IN") : ("OUT")), xfer->xfer_cnt, xfer->xfer_max_cnt);
	usbh_vendor_dump_buf(xfer->test_buf, xfer->xfer_max_cnt);

	usb_os_enter_critical();
	xfer->xfer_done = 0U;
	usb_os_exit_critical();
}
#endif /* USBH_VENDOR_DEBUG */

/**
  * @brief  Deinitialize transfer.
  * @param  host: USB host structure.
  * @param  xfer: Transfer structure.
  * @retval None.
  */
static void usbh_vendor_deinit_transfer(usb_host_t *host, usbh_vendor_xfer_t *xfer)
{
	usbh_pipe_t *pipe = &xfer->pipe;

	if (pipe->pipe_num && host != NULL) {
		usbh_close_pipe(host, pipe);
	}

	xfer->xfer_max_len = 0;
	xfer->xfer_cnt = 0;
	xfer->xfer_max_cnt = 0;
	xfer->cur_frame = 0;
	xfer->xfer_bk_buf = NULL;
	xfer->busy = 0U;
#if USBH_VENDOR_DEBUG
	usb_os_mfree((void *)xfer->test_buf);
	xfer->test_buf = NULL;
	xfer->xfer_done = 0;
#endif
}

/**
  * @brief  Leave the transfer state ahead of a teardown.
  * @note   Taken under the critical section, so once this returns no SOF callback is inside
  *         its locked section and every later one sees IDLE and leaves: the pipes and buffers
  *         about to be released are no longer reachable from the ISR, even on another core.
  * @retval None.
  */
static void usbh_vendor_stop_xfer(void)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;

	usb_os_enter_critical();
	vendor->state = VENDOR_STATE_IDLE;
	usb_os_exit_critical();
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
	int status;

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

		/* Get data in/out endpoints. An endpoint the interface lacks keeps pipe_num 0,
		 * which the transfer API rejects, so a missing endpoint fails at the call. */
		status = usbh_vendor_get_endpoints(host, itf_data->itf_desc_array);
		if (status != HAL_OK) {
			vendor->host = NULL;
			return status;
		}

		/* sof() is only enabled after attach() returns, so no ISR can see the transfer state yet */
		vendor->state = VENDOR_STATE_XFER;

		if ((vendor->cb != NULL) && (vendor->cb->attached != NULL)) {
			vendor->cb->attached();
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

	if (host == NULL) {
		return;
	}

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
  * @retval None
  */
static void usbh_vendor_detach(usb_host_t *host)
{
	UNUSED(host);
	usbh_vendor_host_t *vendor = &usbh_vendor_host;

	usbh_vendor_stop_xfer();

	usbh_vendor_deinit_all_pipe();
	usbh_vendor_deinit_all_xfer();
	vendor->host = NULL;

	if ((vendor->cb != NULL) && (vendor->cb->detached != NULL)) {
		vendor->cb->detached();
	}
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
  * @brief  Arm the next ISOC IN transfer or finish the loop.
  * @note   Task context only: the ISOC IN loop is completion-driven, the SOF ISR never
  *         touches this pipe. No buffer clear is needed before a receive, the controller
  *         overwrites it with the received packet.
  * @param  host: Host handle
  * @param  xfer: ISOC IN transfer
  * @retval None
  */
static void usbh_vendor_isoc_rx_next(usb_host_t *host, usbh_vendor_xfer_t *xfer)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_pipe_t *pipe = &xfer->pipe;

	if (xfer->xfer_cnt >= xfer->xfer_max_cnt) {
		pipe->xfer_state = USBH_EP_XFER_IDLE;
#if USBH_VENDOR_DEBUG
		usb_os_enter_critical();
		xfer->xfer_done = 1U;
		usb_os_exit_critical();
		usbh_notify(host, 0, &usbh_vendor_driver);
#endif
		usbh_vendor_release(xfer);
		if ((vendor->cb != NULL) && (vendor->cb->received != NULL)) {
			vendor->cb->received(pipe->ep_type, NULL, 0, HAL_OK);
		}
	} else {
		pipe->xfer_len = xfer->xfer_max_len;
		pipe->xfer_buf = xfer->xfer_bk_buf;
		pipe->xfer_state = USBH_EP_XFER_START;
		usbh_notify(host, pipe->pipe_num, &usbh_vendor_driver);
	}
}

/**
  * @brief  Pace an INTR pipe from the SOF ISR.
  * @note   An INTR pipe has no wake source once it is NAKed: the first NAK masks
  *         the NAK interrupt (usbh_hcd_isr.c) and holds the pipe in XFER_BUSY, so
  *         .process is never re-entered and the transfer wedges (observed on the
  *         RLE1509 FPGA: first INTR IN NAK then stop). Mirror usbh_verify_sof:
  *         re-notify the owning pipe so usbh_transfer_process() keeps running.
  *         Only decides; the caller notifies outside its critical section.
  * @param  host: Host handle
  * @param  xfer: INTR transfer structure
  * @retval Pipe number to notify, 0 for none
  */
static u8 usbh_vendor_sof_pace_intr(usb_host_t *host, usbh_vendor_xfer_t *xfer)
{
	usbh_pipe_t *pipe = &xfer->pipe;
	u8 pipe_num = 0U;

	if ((pipe->pipe_num == 0U) || (pipe->ep_type != USB_CH_EP_TYPE_INTR)) {
		return 0U;
	}

	if (pipe->xfer_state == USBH_EP_XFER_BUSY) {
		/* NAK masked the NAK interrupt: poke the main task once the BUSY dwell
		 * exceeds max_timeout_tick so usbh_transfer_process() can run the
		 * timeout -> retry (BUSY -> START) that resubmits the token. */
		if ((pipe->tick != 0U) && (usbh_get_elapsed_ticks(host, pipe->tick) > pipe->max_timeout_tick)) {
			pipe_num = pipe->pipe_num;
		}
	} else if (pipe->xfer_state == USBH_EP_XFER_START) {
		/* Re-arm at the bInterval deadline (or the very first submit, tick == 0). */
		if ((pipe->tick == 0U) || (usbh_get_elapsed_ticks(host, pipe->tick) >= pipe->ep_interval)) {
			pipe_num = pipe->pipe_num;
		}
	} else {
		/* Idle or owned by its completion path */
	}

	return pipe_num;
}

/**
  * @brief  SOF callback function.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @note   The ISOC OUT pipe is handed between this ISR and the host task: the task owns it
  *         in START/BUSY and parks it in WAIT_SOF once a packet is out, this ISR owns it in
  *         WAIT_SOF and hands it back in START. Both sides switch the state under the
  *         critical section, a spinlock on SMP, so the pipe fields written before a switch
  *         are visible to the other core after it. Notifies and user callbacks run after the
  *         lock is dropped.
  * @param  host: Host handle
  * @retval None
  */
static void usbh_vendor_sof(usb_host_t *host)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_vendor_xfer_t *out_xfer = &vendor->isoc_out_xfer;
	usbh_pipe_t *pipe = &out_xfer->pipe;
	u32 cur_frame = usbh_get_current_frame_number(host);
	u32 frame_diff;
	u8 out_pipe = 0U;
	u8 intr_in_pipe;
	u8 intr_out_pipe;
	u8 finished = 0U;

	usb_os_enter_critical();

	/* A teardown is closing the pipes: leave them alone */
	if (vendor->state != VENDOR_STATE_XFER) {
		usb_os_exit_critical();
		return;
	}

	if (out_xfer->cur_frame == 0U) {
		out_xfer->cur_frame = (u16)cur_frame;
	}

	if (pipe->xfer_state == USBH_EP_XFER_WAIT_SOF) {
		/* FRNUM is a 14-bit wrapping counter: mask the unsigned difference so the pacing
		 * survives a wrap, instead of yielding a negative remainder that skips submits. */
		frame_diff = (cur_frame - (u32)out_xfer->cur_frame) & USB_FRAME_NUM_MASK;
		if ((pipe->ep_interval != 0U) && ((frame_diff % (u32)pipe->ep_interval) == 0U)) {
			//isoc tx in 2^(ep_interval -1) sof
			out_xfer->xfer_cnt++;
			if (out_xfer->xfer_cnt >= out_xfer->xfer_max_cnt) {
				pipe->xfer_state = USBH_EP_XFER_IDLE;
				out_xfer->busy = 0U;
#if USBH_VENDOR_DEBUG
				out_xfer->xfer_done = 1U;
#endif
				finished = 1U;
			} else {
				/* The buffer is filled by usbh_vendor_isoc_process_tx(), keeping the copy out of the ISR */
				pipe->xfer_len = out_xfer->xfer_max_len;
				pipe->xfer_buf = out_xfer->xfer_bk_buf;
				pipe->xfer_state = USBH_EP_XFER_START;
				out_pipe = pipe->pipe_num;
			}
			out_xfer->cur_frame = (u16)cur_frame;
		}
	}

	/* INTR IN/OUT have no completion-driven wake source while NAKed; re-poll them here. */
	intr_in_pipe = usbh_vendor_sof_pace_intr(host, &vendor->intr_in_xfer);
	intr_out_pipe = usbh_vendor_sof_pace_intr(host, &vendor->intr_out_xfer);

	usb_os_exit_critical();

	if (out_pipe != 0U) {
		usbh_notify(host, out_pipe, &usbh_vendor_driver);
	}
	if (intr_in_pipe != 0U) {
		usbh_notify(host, intr_in_pipe, &usbh_vendor_driver);
	}
	if (intr_out_pipe != 0U) {
		usbh_notify(host, intr_out_pipe, &usbh_vendor_driver);
	}
	if (finished != 0U) {
#if USBH_VENDOR_DEBUG
		/* Logging is forbidden here, let usbh_vendor_process() report the finished loop */
		usbh_notify(host, 0, &usbh_vendor_driver);
#endif
		if ((vendor->cb != NULL) && (vendor->cb->transmitted != NULL)) {
			vendor->cb->transmitted(USB_CH_EP_TYPE_ISOC);
		}
	}
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
	/* Latched before the pipe can turn idle: once released, the next caller may re-arm xfer_buf */
	u8 *buf = pipe->xfer_buf;
	int status;
	u32 len;

	status = usbh_transfer_process(host, pipe);

	if ((status == HAL_OK) && (pipe->xfer_state == USBH_EP_XFER_IDLE)) {
		len = usbh_get_last_transfer_size(host, pipe);
		in_xfer->xfer_cnt++;
		usbh_vendor_release(in_xfer);
		if ((vendor->cb != NULL) && (vendor->cb->received != NULL)) {
			vendor->cb->received(pipe->ep_type, buf, len, status);
		}
	} else if (pipe->xfer_state == USBH_EP_XFER_START) {
		len = usbh_get_last_transfer_size(host, pipe);
		if ((pipe->xfer_len == 0U) && (vendor->cb != NULL) && (vendor->cb->received != NULL)) {//ZLP
			vendor->cb->received(pipe->ep_type, buf, len, status);
		}
		usbh_notify(host, pipe->pipe_num, &usbh_vendor_driver);
	} else if (pipe->xfer_state == USBH_EP_XFER_ERROR) {
		/* Report once and free the pipe: re-notifying an errored pipe only replays the error */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK RX fail: %d\n", usbh_get_urb_state(host, pipe));
		pipe->xfer_state = USBH_EP_XFER_IDLE;
		usbh_vendor_release(in_xfer);
		if ((vendor->cb != NULL) && (vendor->cb->received != NULL)) {
			vendor->cb->received(pipe->ep_type, NULL, 0, status);
		}
	} else {
		/* Still in flight */
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

	status = usbh_transfer_process(host, pipe);

	if ((status == HAL_OK) && (pipe->xfer_state == USBH_EP_XFER_IDLE)) {
		out_xfer->xfer_cnt++;
		usbh_vendor_release(out_xfer);
		if (vendor->cb != NULL && vendor->cb->transmitted != NULL) {
			vendor->cb->transmitted(pipe->ep_type);
		}
	} else if (pipe->xfer_state == USBH_EP_XFER_START) {
		usbh_notify(host, pipe->pipe_num, &usbh_vendor_driver);
	} else if (pipe->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK TX fail: %d\n", usbh_get_urb_state(host, pipe));
		pipe->xfer_state = USBH_EP_XFER_IDLE;
		usbh_vendor_release(out_xfer);
		if (vendor->cb != NULL && vendor->cb->transmitted != NULL) {
			vendor->cb->transmitted(pipe->ep_type);
		}
	} else {
		/* Still in flight */
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
	/* Latched before the pipe can turn idle, refer to usbh_vendor_bulk_process_rx */
	u8 *buf = pipe->xfer_buf;
	int status;
	u32 len;

	status = usbh_transfer_process(host, pipe);

	if ((status == HAL_OK) && (pipe->xfer_state == USBH_EP_XFER_IDLE)) {
		len = usbh_get_last_transfer_size(host, pipe);
		in_xfer->xfer_cnt++;
		usbh_vendor_release(in_xfer);
		if ((vendor->cb != NULL) && (vendor->cb->received != NULL)) {
			vendor->cb->received(pipe->ep_type, buf, len, status);
		}
	} else if ((pipe->xfer_state == USBH_EP_XFER_START)) {
		usbh_notify(host, pipe->pipe_num, &usbh_vendor_driver);
	} else if (pipe->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR RX fail: %d\n", usbh_get_urb_state(host, pipe));
		pipe->xfer_state = USBH_EP_XFER_IDLE;
		usbh_vendor_release(in_xfer);
		if ((vendor->cb != NULL) && (vendor->cb->received != NULL)) {
			vendor->cb->received(pipe->ep_type, NULL, 0, status);
		}
	} else {
		/* Still in flight */
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

	status = usbh_transfer_process(host, pipe);

	if ((status == HAL_OK) && (pipe->xfer_state == USBH_EP_XFER_IDLE)) {
		out_xfer->xfer_cnt++;
		usbh_vendor_release(out_xfer);
		if (vendor->cb != NULL && vendor->cb->transmitted != NULL) {
			vendor->cb->transmitted(pipe->ep_type);
		}
	} else if (pipe->xfer_state == USBH_EP_XFER_START) {
		usbh_notify(host, pipe->pipe_num, &usbh_vendor_driver);
	} else if (pipe->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR TX fail: %d\n", usbh_get_urb_state(host, pipe));
		pipe->xfer_state = USBH_EP_XFER_IDLE;
		usbh_vendor_release(out_xfer);
		if (vendor->cb != NULL && vendor->cb->transmitted != NULL) {
			vendor->cb->transmitted(pipe->ep_type);
		}
	} else {
		/* Still in flight */
	}
}

/**
  * @brief  ISOC RX handling
  * @note   ISOC has no handshake and no retry (Ref USB 2.0 5.6.4): a failed packet is counted
  *         as lost and the loop goes on with the next service interval.
  * @param  host: Host handle
  * @retval None
*/
static void usbh_vendor_isoc_process_rx(usb_host_t *host)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_vendor_xfer_t *in_xfer = &vendor->isoc_in_xfer;
	usbh_pipe_t *pipe = &in_xfer->pipe;
	int status;
#if USBH_VENDOR_DEBUG
	u32 len;
#endif

	status = usbh_transfer_process(host, pipe);

	if ((status == HAL_OK) && (pipe->xfer_state == USBH_EP_XFER_IDLE)) {
#if USBH_VENDOR_DEBUG
		/* A 0-byte packet is a valid ISOC result and records 0, as the old pre-cleared buffer did */
		len = usbh_get_last_transfer_size(host, pipe);
		if (in_xfer->xfer_cnt < in_xfer->xfer_max_cnt) {
			in_xfer->test_buf[in_xfer->xfer_cnt] = (len != 0U) ? pipe->xfer_buf[0] : 0U;
		}
#endif
		in_xfer->xfer_cnt ++;
		usbh_vendor_isoc_rx_next(host, in_xfer);
	} else if (pipe->xfer_state == USBH_EP_XFER_START) {
		usbh_notify(host, pipe->pipe_num, &usbh_vendor_driver);
	} else if (pipe->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ISOC RX fail: %d\n", usbh_get_urb_state(host, pipe));
#if USBH_VENDOR_DEBUG
		if (in_xfer->xfer_cnt < in_xfer->xfer_max_cnt) {
			in_xfer->test_buf[in_xfer->xfer_cnt] = 0xFF;
		}
#endif
		in_xfer->xfer_cnt++;
		usbh_vendor_isoc_rx_next(host, in_xfer);
	} else {
		/* Still in flight */
	}
}

/**
  * @brief  ISOC TX handling
  * @note   Task side of the ISOC OUT hand-off, refer to usbh_vendor_sof. A sent packet and a
  *         failed one both park the pipe in WAIT_SOF: ISOC has no handshake and no retry
  *         (Ref USB 2.0 5.6.4), so a failed packet is counted as lost and the SOF pacing goes
  *         on, instead of stalling the loop until the caller times out.
  * @param  host: Host handle
  * @retval None
*/
static void usbh_vendor_isoc_process_tx(usb_host_t *host)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usbh_vendor_xfer_t *out_xfer = &vendor->isoc_out_xfer;
	usbh_pipe_t *pipe = &out_xfer->pipe;
	int status;
	u8 xfer_state;

	/* Acquire the hand-off: only START/BUSY belong to the task. A stale message seen in
	 * WAIT_SOF must not run usbh_transfer_process(), which would flag the pipe as ERROR. */
	usb_os_enter_critical();
	xfer_state = pipe->xfer_state;
	usb_os_exit_critical();
	if ((xfer_state != USBH_EP_XFER_START) && (xfer_state != USBH_EP_XFER_BUSY)) {
		return;
	}

	if (xfer_state == USBH_EP_XFER_START) {
		/* The first packet carries the caller's data, every later one the loop index */
		if (out_xfer->xfer_cnt != 0U) {
			usb_os_memset((void *)pipe->xfer_buf, (u8)(out_xfer->xfer_cnt & 0xFFU), out_xfer->xfer_max_len);
		}
		if (host->dev_speed != USB_SPEED_HIGH) {
			pipe->xfer_len = (pipe->xfer_len >= pipe->ep_mps) ? pipe->ep_mps : pipe->xfer_len;
		}
#if USBH_VENDOR_DEBUG
		if (out_xfer->xfer_cnt < out_xfer->xfer_max_cnt) {
			out_xfer->test_buf[out_xfer->xfer_cnt] = pipe->xfer_buf[0];
		}
#endif
	}

	status = usbh_transfer_process(host, pipe);

	if ((status == HAL_OK) && (pipe->xfer_state == USBH_EP_XFER_IDLE)) {
		usb_os_enter_critical();
		pipe->xfer_state = USBH_EP_XFER_WAIT_SOF;
		usb_os_exit_critical();
	} else if (pipe->xfer_state == USBH_EP_XFER_START) {
		usbh_notify(host, pipe->pipe_num, &usbh_vendor_driver);
	} else if (pipe->xfer_state == USBH_EP_XFER_ERROR) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ISOC TX fail: %d\n", usbh_get_urb_state(host, pipe));
#if USBH_VENDOR_DEBUG
		if (out_xfer->xfer_cnt < out_xfer->xfer_max_cnt) {
			out_xfer->test_buf[out_xfer->xfer_cnt] = 0xFF;
		}
#endif
		usb_os_enter_critical();
		pipe->xfer_state = USBH_EP_XFER_WAIT_SOF;
		usb_os_exit_critical();
	} else {
		/* Still in flight */
	}
}

/**
  * @brief  State machine handling callback
  * @param  host: Host handle
  * @param  msg: USB host driver message
  * @retval None
  */
static void usbh_vendor_process(usb_host_t *host, usbh_drv_msg_t *msg)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;

	if (vendor->state != VENDOR_STATE_XFER) {
		return;
	}

#if USBH_VENDOR_DEBUG
	/* Deferred from the SOF ISR, refer to usbh_vendor_sof. */
	usbh_vendor_report_finish(&vendor->isoc_in_xfer);
	usbh_vendor_report_finish(&vendor->isoc_out_xfer);
#endif

	/* Pipe 0 is EP0, which this class never owns: such a message only wakes the reports above */
	if ((msg == NULL) || (msg->pipe_num == 0U)) {
		return;
	}

	if (msg->pipe_num == vendor->bulk_in_xfer.pipe.pipe_num) {
		usbh_vendor_bulk_process_rx(host);
	} else if (msg->pipe_num == vendor->bulk_out_xfer.pipe.pipe_num) {
		usbh_vendor_bulk_process_tx(host);
	} else if (msg->pipe_num == vendor->intr_in_xfer.pipe.pipe_num) {
		usbh_vendor_intr_process_rx(host);
	} else if (msg->pipe_num == vendor->intr_out_xfer.pipe.pipe_num) {
		usbh_vendor_intr_process_tx(host);
	} else if (msg->pipe_num == vendor->isoc_in_xfer.pipe.pipe_num) {
		usbh_vendor_isoc_process_rx(host);
	} else if (msg->pipe_num == vendor->isoc_out_xfer.pipe.pipe_num) {
		usbh_vendor_isoc_process_tx(host);
	} else {
		/* Not one of this class's pipes */
	}
}

/**
  * @brief  Start a transfer loop on one pipe
  * @note   The check and the claim are one step under the critical section: two callers,
  *         possibly on different cores, could otherwise both see the pipe free and overwrite
  *         each other's buffer. The pipe stays claimed until its completion is reported.
  * @param  xfer: Pipe xfer
  * @param  buf: Data buffer
  * @param  len: Data length
  * @param  test_cnt: Test loop count
  * @retval HAL_OK when started, HAL_ERR_PARA for a bad argument or an endpoint the device
  *         does not have, HAL_BUSY while the pipe is still in use or no device is attached
  */
static int usbh_vendor_start_xfer(usbh_vendor_xfer_t *xfer, u8 *buf, u32 len, u32 test_cnt)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;
	usb_host_t *host = vendor->host;
	usbh_pipe_t *pipe = &xfer->pipe;
	u32 buf_len;
	u32 isoc_max;
	int ret = HAL_BUSY;

	/* An unopened pipe has pipe_num 0, which is EP0: arming it would never complete */
	if ((host == NULL) || (buf == NULL) || (pipe->pipe_num == 0U)) {
		return HAL_ERR_PARA;
	}

	if ((pipe->ep_type == USB_CH_EP_TYPE_ISOC) && (test_cnt == 0U)) {
		return HAL_ERR_PARA;
	}

	if (pipe->ep_is_in != 0U) {
		buf_len = len;
		/* The controller receives whole packets, each adjusted to the DWORD boundary: ask for no
		 * more than the packets that fit in buf */
		len = usb_get_max_payload_len(buf_len, pipe->ep_mps);
		/* One ISOC IN transfer is one service interval, of up to ep_trans packets at HS and of a
		 * single packet otherwise, Ref USB 2.0 5.6.3 */
		if (pipe->ep_type == USB_CH_EP_TYPE_ISOC) {
			isoc_max = (u32)pipe->ep_mps * ((host->dev_speed == USB_SPEED_HIGH) ? (u32)pipe->ep_trans : 1U);
			if (len > isoc_max) {
				len = isoc_max;
			}
		}
		if (len == 0U) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "RX buf %u < mps %u\n", buf_len, pipe->ep_mps);
			return HAL_ERR_PARA;
		}
	} else {
		buf_len = 0U;
	}

	usb_os_enter_critical();
	if ((vendor->state == VENDOR_STATE_XFER) && (xfer->busy == 0U) && (pipe->xfer_state == USBH_EP_XFER_IDLE)
#if USBH_VENDOR_DEBUG
		&& (xfer->xfer_done == 0U)
#endif
	   ) {
		xfer->busy = 1U;
		ret = HAL_OK;
	}
	usb_os_exit_critical();
	if (ret != HAL_OK) {
		return ret;
	}

#if USBH_VENDOR_DEBUG
	if (pipe->ep_type == USB_CH_EP_TYPE_ISOC) {
		/* test_buf holds one verification byte per loop, so test_cnt is also its size. */
		usb_os_mfree((void *)xfer->test_buf);
		xfer->test_buf = (u8 *)usb_os_malloc(test_cnt);
		if (xfer->test_buf == NULL) {
			usbh_vendor_release(xfer);
			return HAL_BUSY;
		}
	}
#endif

	usb_os_enter_critical();
	if (vendor->state != VENDOR_STATE_XFER) {
		/* Detached while the claim was being prepared */
		xfer->busy = 0U;
		ret = HAL_BUSY;
	} else {
		pipe->xfer_buf = buf;
		pipe->xfer_len = len;
		pipe->xfer_buf_len = buf_len;
		xfer->xfer_max_len = len;
		xfer->xfer_bk_buf = buf;
		xfer->xfer_max_cnt = test_cnt;
		/* Per-run state: the loop index doubles as the test_buf subscript, so it must restart at 0
		 * together with the freshly sized buffer, and the ISOC OUT SOF pacing must re-latch its
		 * frame baseline instead of keeping the previous run's frame number. */
		xfer->xfer_cnt = 0;
		xfer->cur_frame = 0;
		if ((len > 0U) && ((len % pipe->ep_mps) == 0U) && (pipe->ep_type == USB_CH_EP_TYPE_BULK)) { //ZLP
			pipe->trx_zlp = 1;
		} else {
			pipe->trx_zlp = 0;
		}
		pipe->xfer_state = USBH_EP_XFER_START;
	}
	usb_os_exit_critical();

	if (ret == HAL_OK) {
		usbh_notify(host, pipe->pipe_num, &usbh_vendor_driver);
	}

	return ret;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init vendor class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_vendor_init(const usbh_vendor_cb_t *cb)
{
	int ret = HAL_OK;
	usbh_vendor_host_t *vendor = &usbh_vendor_host;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	usb_os_memset((void *)vendor, 0x00, sizeof(usbh_vendor_host_t));
	vendor->cb = cb;
	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "User init err %d\n", ret);
			vendor->cb = NULL;
			return ret;
		}
	}

	ret = usbh_register_class(&usbh_vendor_driver);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Register class fail %d\n", ret);
		if (cb->deinit != NULL) {
			cb->deinit();
		}
		vendor->cb = NULL;
		return ret;
	}

	return ret;
}

/**
  * @brief  Deinit vendor class
  */
void usbh_vendor_deinit(void)
{
	usbh_vendor_host_t *vendor = &usbh_vendor_host;

	/* First, before anything is freed: on HAL_OK no attach/setup/process/detach of this
	 * class runs any more, an EP0 request it left in flight is halted and a sof() still
	 * running on another core is waited out. Anything else means nothing was released. */
	if (usbh_unregister_class(&usbh_vendor_driver) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Deinit refused\n");
		return;
	}

	if ((vendor->cb != NULL) && (vendor->cb->deinit != NULL)) {
		vendor->cb->deinit();
	}
	usbh_vendor_stop_xfer();

	usbh_vendor_deinit_all_pipe();
	usbh_vendor_deinit_all_xfer();
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
	return usbh_vendor_start_xfer(&vendor->bulk_out_xfer, buf, len, test_cnt);
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
	return usbh_vendor_start_xfer(&vendor->bulk_in_xfer, buf, len, test_cnt);
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
	return usbh_vendor_start_xfer(&vendor->intr_out_xfer, buf, len, test_cnt);
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
	return usbh_vendor_start_xfer(&vendor->intr_in_xfer, buf, len, test_cnt);
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
	return usbh_vendor_start_xfer(&vendor->isoc_out_xfer, buf, len, test_cnt);
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
	return usbh_vendor_start_xfer(&vendor->isoc_in_xfer, buf, len, test_cnt);
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
