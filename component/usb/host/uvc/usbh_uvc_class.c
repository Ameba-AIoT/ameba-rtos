/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_uvc.h"
#include "usbh_uvc_class.h"
#include "usbh_uvc_parse.h"
#include "usbh_uvc_stream.h"

/* Private defines -----------------------------------------------------------*/

/* Probe/Commit size of UVC 1.0, the shortest one: it ends with dwMaxPayloadTransferSize */
#define USBH_UVC_PROBE_MIN_LEN                 (26U)

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbh_uvc_attach(usb_host_t *host);
static void usbh_uvc_detach(usb_host_t *host);
static void usbh_uvc_process(usb_host_t *host, usbh_drv_msg_t *msg);
static int usbh_uvc_setup(usb_host_t *host);
#if (USBH_UVC_USE_HW == 0)
static void usbh_uvc_sof(usb_host_t *host);
static void usbh_uvc_completed(usb_host_t *host, u8 pipe_num);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "UVC";

/* USB UVC device identification */
static const usbh_dev_id_t uvc_devs[] = {
	{
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS,
		.idVendor = 0x0BDA,
		.bInterfaceClass = USB_VIDEO_CLASS_CODE,
	},
	{
	},
};

extern usbh_uvc_host_t uvc_host;

/* Exported variables --------------------------------------------------------*/

/* USB Host UVC class driver */
const usbh_class_driver_t usbh_uvc_driver = {
	.id_table = uvc_devs,
	.attach = usbh_uvc_attach,
	.detach = usbh_uvc_detach,
	.setup = usbh_uvc_setup,
	.process = usbh_uvc_process,
#if (USBH_UVC_USE_HW == 0)
	.sof = usbh_uvc_sof,
	.completed = usbh_uvc_completed
#endif
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Attach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uvc_attach(usb_host_t *host)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = NULL;
	usbh_uvc_setting_t *cur_set = NULL;
	usbh_uvc_alt_t *alt_set = NULL;
	usbh_uvc_vs_t *vs_intf = NULL;
	usbh_ep_desc_t *ep = NULL;
	usbh_pipe_t *pipe = NULL;
	int status = HAL_OK;
	u32 xfer_len = 0;
	u8 i;

	uvc->host = host;

	usbh_uvc_desc_init();
	status = usbh_uvc_parse_cfgdesc(host);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Parse cfg desc fail\n");
		/* Roll back whatever usbh_uvc_parse_cfgdesc() already built before it
		 * failed (VC entity_list and any already-parsed VS format arrays).
		 * attach() failure means core never marks this class attached, so
		 * detach() will never run to do this cleanup for us. */
		usbh_uvc_desc_deinit();
		for (i = 0U; i < uvc->uvc_desc.vs_num; i ++) {
			vs_intf = uvc->stream[i].vs_intf;
			if (vs_intf != NULL) {
				usb_os_mfree((void *)vs_intf->format);
				vs_intf->format = NULL;
				vs_intf->format_num = 0U;
				uvc->stream[i].vs_intf = NULL;
			}
		}
		uvc->uvc_desc.vs_num = 0U;
		return status;
	}

	/* find the first alt setting and endpoint as default for each vs interface */
	for (i = 0U; i < uvc->uvc_desc.vs_num; i ++) {
		stream = &uvc->stream[i];
		cur_set = &stream->cur_setting;
		cur_set->cur_vs_intf = &uvc->uvc_desc.vs_intf[i];
		/* Start from this interface with no usable alt, so that a stream skipped below never
		 * keeps the interface number or the endpoint of a previous attach */
		cur_set->bInterfaceNumber = cur_set->cur_vs_intf->bInterfaceNumber;
		cur_set->bAlternateSetting = 0U;
		cur_set->altsetting = NULL;
		cur_set->valid = 0U;
		pipe = &cur_set->pipe;
		pipe->ep_addr = 0U;
		pipe->ep_mps = 0U;
		pipe->xfer_len = 0U;

		/* Alt 0 is never recorded, and an alt keeps a NULL endpoint unless it streams through
		 * an isochronous IN one, refer to usbh_uvc_parse_vs() */
		alt_set = &cur_set->cur_vs_intf->altsetting[0];
		if (alt_set->p == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "S[%d] no alt\n", i);
			continue;
		}
		cur_set->altsetting = alt_set;
		cur_set->bAlternateSetting = ((usbh_itf_desc_t *)alt_set->p)->bAlternateSetting;

		ep = alt_set->endpoint;
		if (ep == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "S[%d] %d/%d no ep\n", i, cur_set->bInterfaceNumber, cur_set->bAlternateSetting);
			continue;
		}
		xfer_len = ep->wMaxPacketSize;

		if (host->dev_speed == USB_SPEED_HIGH) {
			/* 11b additional transactions is reserved, Ref USB 2.0 9.6.6 Table 9-13 */
			if ((xfer_len & USB_EP_MPS_TRANS_MASK) == USB_EP_MPS_TRANS_MASK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "S[%d] %d/%d bad mult\n", i, cur_set->bInterfaceNumber, cur_set->bAlternateSetting);
				continue;
			}
			xfer_len = (xfer_len & USB_EP_MPS_SIZE_MASK) * (1U + ((xfer_len & USB_EP_MPS_TRANS_MASK) >> USB_EP_MPS_TRANS_POS));
		} else {
			xfer_len = xfer_len & USB_EP_MPS_SIZE_MASK;
		}

		//Note: vc may has a interrupt endpoint, vs may has a bulk endpoint for still image data. not support now.
		pipe->ep_addr = ep->bEndpointAddress;
		pipe->ep_mps = ep->wMaxPacketSize & USB_EP_MPS_SIZE_MASK;
		pipe->ep_interval = ep->bInterval;
		pipe->ep_type = ep->bmAttributes & USB_EP_XFER_TYPE_MASK;
		pipe->xfer_len = xfer_len;
		cur_set->valid = 1;

#if USBH_UVC_DEBUG
		RTK_LOGS(TAG, RTK_LOG_INFO, "S[%d] Itf/alt:%d/%d ", i, cur_set->bInterfaceNumber, cur_set->bAlternateSetting);
		RTK_LOGS(TAG, RTK_LOG_INFO, "EP:%d-%d-%d-%d-%d\n", pipe->ep_addr, pipe->xfer_len, pipe->ep_mps, pipe->ep_interval, pipe->ep_type);
#endif
	}

	if ((uvc->cb != NULL) && (uvc->cb->attached != NULL)) {
		uvc->cb->attached();
	}

	return status;
}

/**
  * @brief  Detach callback.
  * @param  host: Host handle
  * @retval None
  */
static void usbh_uvc_detach(usb_host_t *host)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream;
	usbh_uvc_vs_t *vs_intf;
	u8 vs_num;
	u8 i;

	uvc->state = UVC_STATE_IDLE;
	vs_num = uvc->uvc_desc.vs_num;

	for (i = 0U; i < vs_num; i ++) {
		stream = &uvc->stream[i];
		stream->state = STREAM_STATE_CTRL_IDLE;
		stream->alt_active = 0U;
		stream->start_pending = 0U;
		/* The setting names this device's descriptors, freed below: it must be found again */
		stream->cur_setting.valid = 0U;
		if (stream->stream_state == UVC_STREAM_ACTIVE) {
			usbh_uvc_stream_stop(stream);
		}
		/* Halt the streaming channel and free its slot after the stream (and the HW decoder)
		 * stopped: on a re-enumeration the core closes only EP0, so the channel would stay
		 * enabled. usbh_close_pipe() also zeroes pipe_num, so the next find_alt cannot close a
		 * recycled channel. */
		if (stream->cur_setting.pipe.pipe_num != 0U) {
			usbh_close_pipe(host, &stream->cur_setting.pipe);
		}
#if (USBH_UVC_USE_HW == 0)
		/* The URBs are sized for this device's alt: a later device must not inherit them,
		 * refer to the allocation in usbh_uvc_ctrl_set_alt_done() */
		if (stream->urb_buffer != NULL) {
			usbh_uvc_stream_free_urb_buffer(stream);
		}
#endif
	}

	usbh_uvc_desc_deinit();

	for (i = 0U; i < vs_num; i ++) {
		vs_intf = uvc->stream[i].vs_intf;
		if (vs_intf != NULL) {
			usb_os_mfree((void *)vs_intf->format);
			vs_intf->format = NULL;
			vs_intf->format_num = 0U;
			uvc->stream[i].vs_intf = NULL;
		}
	}

	/* Reset VS count so a re-enumeration re-parses fully instead of hitting the
	 * "Ovrl vs" early-return and leaving format=NULL / format_num stale (Data Abort
	 * on hot-replug when usbh_uvc_dump_dev_info walks &vs->format[i]). */
	uvc->uvc_desc.vs_num = 0U;

	uvc->host = NULL;

	if ((uvc->cb != NULL) && (uvc->cb->detached != NULL)) {
		uvc->cb->detached();
	}
}

/**
  * @brief	Find out best alt setting
  * @param	stream: uvc stream interface
  * @retval None
  */
static void usbh_uvc_find_alt(usbh_uvc_stream_t *stream)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usb_host_t *host = uvc->host;
	usbh_uvc_setting_t *cur_set = &stream->cur_setting;
	usbh_pipe_t *pipe = NULL;
	usbh_ep_desc_t *ep = NULL;
	u32 xfer_len = 0;
	u32 best_len;
	u32 XferSize = stream->stream_ctrl.dwMaxPayloadTransferSize;
	u32 max_ep_size = UINT_MAX;
	u8 i;

	cur_set->valid = 0;

	for (i = 0U; i < stream->vs_intf->alt_num; i++) {
		ep = stream->vs_intf->altsetting[i].endpoint;
		/* Skip an alt whose 11b additional transactions is reserved, Ref USB 2.0 9.6.6 Table 9-13 */
		if ((ep == NULL) || ((host->dev_speed == USB_SPEED_HIGH) && ((ep->wMaxPacketSize & USB_EP_MPS_TRANS_MASK) == USB_EP_MPS_TRANS_MASK))) {
			continue;
		}

		xfer_len = ep->wMaxPacketSize;

		if (host->dev_speed == USB_SPEED_HIGH) {
			xfer_len = (xfer_len & USB_EP_MPS_SIZE_MASK) * (1U + ((xfer_len & USB_EP_MPS_TRANS_MASK) >> USB_EP_MPS_TRANS_POS));
		} else {
			xfer_len = xfer_len & USB_EP_MPS_SIZE_MASK;
		}

		if ((xfer_len >= XferSize) && (xfer_len <= max_ep_size)) {
			cur_set->altsetting = &stream->vs_intf->altsetting[i];
			cur_set->bAlternateSetting = ((usbh_itf_desc_t *)cur_set->altsetting->p)->bAlternateSetting;
			cur_set->bInterfaceNumber = cur_set->cur_vs_intf->bInterfaceNumber;
			cur_set->valid = 1U;

			pipe = &cur_set->pipe;
			pipe->ep_addr = ep->bEndpointAddress;
			pipe->ep_mps = ep->wMaxPacketSize & USB_EP_MPS_SIZE_MASK;
			pipe->ep_interval = ep->bInterval;
			pipe->ep_type = ep->bmAttributes & USB_EP_XFER_TYPE_MASK;
			pipe->xfer_len = xfer_len;

			max_ep_size = xfer_len;
		}
	}

	/* Fallback: if no alt satisfies xfer_len >= XferSize, use the alt with the largest MPS
	 * rather than proceeding with ep_mps=0 which would break the HW decoder. */
	if (cur_set->valid == 0U) {
		best_len = 0;
		for (i = 0U; i < stream->vs_intf->alt_num; i++) {
			ep = stream->vs_intf->altsetting[i].endpoint;
			if ((ep == NULL) || ((host->dev_speed == USB_SPEED_HIGH) && ((ep->wMaxPacketSize & USB_EP_MPS_TRANS_MASK) == USB_EP_MPS_TRANS_MASK))) {
				continue;
			}
			xfer_len = ep->wMaxPacketSize;
			if (host->dev_speed == USB_SPEED_HIGH) {
				xfer_len = (xfer_len & USB_EP_MPS_SIZE_MASK) * (1U + ((xfer_len & USB_EP_MPS_TRANS_MASK) >> USB_EP_MPS_TRANS_POS));
			} else {
				xfer_len = xfer_len & USB_EP_MPS_SIZE_MASK;
			}
			if (xfer_len > best_len) {
				best_len = xfer_len;
				cur_set->altsetting = &stream->vs_intf->altsetting[i];
				cur_set->bAlternateSetting = ((usbh_itf_desc_t *)cur_set->altsetting->p)->bAlternateSetting;
				cur_set->bInterfaceNumber = cur_set->cur_vs_intf->bInterfaceNumber;

				pipe = &cur_set->pipe;
				pipe->ep_addr = ep->bEndpointAddress;
				pipe->ep_mps = ep->wMaxPacketSize & USB_EP_MPS_SIZE_MASK;
				pipe->ep_interval = ep->bInterval;
				pipe->ep_type = ep->bmAttributes & USB_EP_XFER_TYPE_MASK;
				pipe->xfer_len = xfer_len;
			}
		}
		if (best_len > 0U) {
			cur_set->valid = 1;
			RTK_LOGS(TAG, RTK_LOG_WARN, "Fallback alt %d req %u mps %u\n",
					 cur_set->bAlternateSetting, XferSize, best_len);
		}
	}

	if ((pipe != NULL) && (pipe->pipe_num != 0U)) {
		usbh_close_pipe(host, pipe);
		pipe->pipe_num = 0U;
	}

#if USBH_UVC_DEBUG
	RTK_LOGS(TAG, RTK_LOG_INFO, "F Itf/alt:%d/%d\n", cur_set->bInterfaceNumber, cur_set->bAlternateSetting);
#endif
}

/**
  * @brief  Standard control requests handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uvc_setup(usb_host_t *host)
{
	usbh_uvc_host_t *uvc = &uvc_host;

	UNUSED(host);

	if ((uvc->cb != NULL) && (uvc->cb->setup != NULL)) {
		uvc->cb->setup();
	}

	return HAL_OK;
}

/**
  * @brief  Ctrl Request set alt done handle
  * @param  host: Host handle
  * @param	stream: uvc stream interface
  * @retval void
  */
static void usbh_uvc_ctrl_set_alt_done(usbh_uvc_host_t *uvc, usbh_uvc_stream_t *stream)
{
	usbh_uvc_setting_t *cur_set = &stream->cur_setting;
	/* A restart driven by usbh_uvc_start() must not report through the set_param callback:
	 * the application waits on that callback for its next usbh_uvc_set_param(), and an extra
	 * notification would make the following wait succeed before the negotiation is done. */
	u8 restart = stream->start_pending;

	stream->start_pending = 0U;

	if (cur_set->valid == 1U) {
		/* Open pipe */
		stream->state = STREAM_STATE_CTRL_IDLE;
		stream->set_alt = 0x0;
		stream->set_alt_retry = 0;
#if (USBH_UVC_USE_HW == 0)
		/* The URBs are sized from pipe->xfer_len of this alt: a negotiation freed the old ones,
		 * a start() restart kept them. Allocated here, on every path to the streaming alt, so
		 * that the SOF ISR never submits into a missing URB. */
		if ((stream->urb_buffer == NULL) && (usbh_uvc_stream_alloc_urb_buffer(stream) != HAL_OK)) {
			stream->alt_active = 0U;
			uvc->state = UVC_STATE_IDLE;
			if ((uvc->cb != NULL) && (uvc->cb->set_param != NULL) && (restart == 0U)) {
				uvc->cb->set_param(HAL_ERR_MEM);
			}
			return;
		}
#endif
		if (usbh_open_pipe(uvc->host, &cur_set->pipe, cur_set->altsetting->endpoint, &usbh_uvc_driver) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Open isoc pipe fail\n");
			stream->alt_active = 0U;
			uvc->state = UVC_STATE_IDLE;
			if ((uvc->cb != NULL) && (uvc->cb->set_param != NULL) && (restart == 0U)) {
				uvc->cb->set_param(HAL_ERR_HW);
			}
			return;
		}
#if USBH_UVC_DEBUG
		RTK_LOGS(TAG, RTK_LOG_INFO,
				 "Alt %d: ep_addr=0x%02x, mps=%d, interval=%d, type=%d, xfer_len=%d\n",
				 cur_set->bAlternateSetting,
				 cur_set->pipe.ep_addr,
				 cur_set->pipe.ep_mps,
				 cur_set->pipe.ep_interval,
				 cur_set->pipe.ep_type,
				 cur_set->pipe.xfer_len);
#endif
		stream->alt_active = 1U;
		uvc->state = UVC_STATE_TRANSFER;
		if (restart != 0U) {
			/* Interface is back on its streaming alt: resume the data path that
			 * usbh_uvc_stop() halted. Resources were kept, so this only re-arms it.
			 * usbh_uvc_start() already returned, so a failure can only be reported here. */
			if (usbh_uvc_stream_start(stream) != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "S%d restart fail\n", stream->stream_idx);
			}
		} else if ((uvc->cb != NULL) && (uvc->cb->set_param != NULL)) {
			uvc->cb->set_param(HAL_OK);
		} else {
			/* no consumer for the result */
		}
	} else {
		/* Don't need clear feature , just stop all statemachine */
		stream->alt_active = 0U;
		uvc->state = UVC_STATE_IDLE;
		stream->state = STREAM_STATE_CTRL_IDLE;
		if ((uvc->cb != NULL) && (uvc->cb->set_param != NULL) && (restart == 0U)) {
			uvc->cb->set_param(HAL_ERR_HW);
		}
	}
}

/**
  * @brief  Ctrl Request set alt err handle
  * @param  host: Host handle
  * @param	stream: uvc stream interface
  * @retval void
  */
static void usbh_uvc_ctrl_set_alt_error(usbh_uvc_host_t *uvc, usbh_uvc_stream_t *stream)
{
	/* Try to release isochronous bandwidth by resetting VS interface to alt 0.
	 * Fire-and-forget: if this fails, proceed to error state anyway.
	 * Skip if device is disconnected to avoid blocking on ctrl request. */
	if ((uvc->host != NULL) && (uvc->host->connect_state >= USBH_STATE_SETUP)) {
		(void)usbh_ctrl_set_interface(uvc->host, stream->cur_setting.bInterfaceNumber, 0);
	}

	stream->alt_active = 0U;
	stream->state = STREAM_STATE_CTRL_IDLE;
	uvc->state = UVC_STATE_ERROR;
	if ((uvc->cb != NULL) && (uvc->cb->set_param != NULL) && (stream->start_pending == 0U)) {
		uvc->cb->set_param(HAL_ERR_HW);
	}
	stream->start_pending = 0U;
}

/**
  * @brief  State machine handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uvc_process_ctrl(usb_host_t *host, usbh_drv_msg_t *msg)
{
	int ret = HAL_OK;
	int ret_status = HAL_BUSY;
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = NULL;
	u32 ctrl_struct_size;
	u32 rx_len;
	u32 copy_len;
	u16 size;
	u8 stream_idx = uvc->stream_ctrl_idx;

	UNUSED(msg);

	if (stream_idx >= uvc->uvc_desc.vs_num) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Err S[%d]\n", stream_idx);
		return HAL_OK;
	}

	stream = &uvc->stream[stream_idx];

	switch (stream->state) {
	case STREAM_STATE_CTRL_IDLE:
		ret_status = HAL_OK;
		break;

	/* For setting parameters and enabling the stream. */
	/* This will trigger the flow: Probe -> Commit -> Set Alt. */
	/* (If Config Only, skip this and enter STREAM_STATE_PROBE_NEGOTIATE directly) */
	case STREAM_STATE_SET_PARA:
		stream->set_alt = 1U;
		stream->set_alt_retry = 0;
		/* 1. STREAMOFF: stop stream, halt its channel, free old URBs, then SET_INTERFACE(0).
		 * The channel is closed before the URBs are freed: a transfer submitted just before
		 * the stop may still be DMAing into one of them. find_alt() then sees pipe_num 0. */
		usbh_uvc_stream_stop(stream);
		if (stream->cur_setting.pipe.pipe_num != 0U) {
			usbh_close_pipe(uvc->host, &stream->cur_setting.pipe);
		}
#if (USBH_UVC_USE_HW == 0)
		usbh_uvc_stream_free_urb_buffer(stream);
#endif
		stream->state = STREAM_STATE_RESET_ALT;
		usbh_notify(uvc->host, 0, &usbh_uvc_driver);   /* transfer-less kick */
		break;

	/* Set Alt : interface / 0 */
	case STREAM_STATE_RESET_ALT:
		ret = usbh_ctrl_set_interface(host, stream->cur_setting.bInterfaceNumber, 0);

		if (ret == HAL_OK) {
			if (stream->set_alt == 1U) {
				/* 2: now at alt0, start probe/commit */
				stream->state = STREAM_STATE_PROBE_NEGOTIATE;
			} else {
				uvc->state = UVC_STATE_IDLE;
				stream->state = STREAM_STATE_CTRL_IDLE;
			}
			ret_status = HAL_OK;
		} else if (ret == HAL_BUSY) {
			/* Request in flight. Not counted: HAL_BUSY is a poll, not a failure, and giving
			   up on it would leave the request holding EP0 while the next one is built. The
			   host core bounds it (USBH_CTRL_XFER_TIMEOUT_MS) and reports an error below. */
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set %d/0 err%d\n", stream->cur_setting.bInterfaceNumber, ret);
			if (stream->set_alt == 1U) {
				/* SET_INTERFACE to alt 0 may fail (e.g., device already at alt 0).
				 * Continue to PROBE/Commit — SET_ALT is the authoritative indicator. */
				stream->state = STREAM_STATE_PROBE_NEGOTIATE;
			} else {
				uvc->state = UVC_STATE_IDLE;
				stream->state = STREAM_STATE_CTRL_IDLE;
			}
			ret_status = HAL_OK;
		}
		break;

	case STREAM_STATE_PROBE_NEGOTIATE:
		/* Probe/Commit flow — RESET_ALT, Probe, Commit, and PROBE_UPDATE/PROBE_FINAL
		 * may all fail without indicating overall failure.  Execution continues to
		 * the next state; the final SET_ALT result is the authoritative pass/fail
		 * indicator.  Only SET_ALT retry exhaustion notifies the app layer via
		 * set_param(HAL_ERR_HW). */
		ret = usbh_uvc_stream_ctrl_set_video(stream, 1U);
		if (ret == HAL_OK) {
			stream->state = STREAM_STATE_PROBE_UPDATE;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "P1 err\n");
			stream->state = STREAM_STATE_PROBE_UPDATE;
			ret_status = HAL_OK;
		} else {
			/* HAL_BUSY: request in progress, retry on next cycle */
		}
		break;

	case STREAM_STATE_PROBE_UPDATE:
		ret = usbh_uvc_stream_ctrl_get_video(stream, 1U, USBH_UVC_GET_CUR);
		if (ret == HAL_OK) {
			stream->state = STREAM_STATE_PROBE_FINAL;
			usbh_uvc_stream_control_t *ctrl = &stream->stream_ctrl;
			size = usbh_uvc_get_ctrl_len_by_version(uvc->uvc_desc.vc_intf.vcheader->bcdUVC);
			ctrl_struct_size = sizeof(usbh_uvc_stream_control_t);
			/* The tail of request_buf still holds the SET_CUR just sent: only what the device
			 * returned may be taken. Below the UVC 1.0 size, dwMaxPayloadTransferSize is missing,
			 * so the reply is handled as a failed GET_CUR, ref UVC 1.5 4.3.1.1 */
			rx_len = usbh_get_last_ctrl_in_size(host);
			if (rx_len < USBH_UVC_PROBE_MIN_LEN) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "P2 short %d\n", rx_len);
				ret_status = HAL_OK;
			} else if (uvc->request_buf != NULL) {
				copy_len = MIN(MIN(rx_len, (u32)size), ctrl_struct_size);
				DCache_Invalidate((u32)uvc->request_buf, size);
				usb_os_memcpy((void *)ctrl, (const void *)uvc->request_buf, copy_len);
#if USBH_UVC_DEBUG
				RTK_LOGS(TAG, RTK_LOG_INFO, "bmHint: %d\n", ctrl->bmHint);
				RTK_LOGS(TAG, RTK_LOG_INFO, "bFormatIndex: %d\n", ctrl->bFormatIndex);
				RTK_LOGS(TAG, RTK_LOG_INFO, "bFrameIndex: %d\n", ctrl->bFrameIndex);
				RTK_LOGS(TAG, RTK_LOG_INFO, "dwFrameInterval: %d\n", ctrl->dwFrameInterval);
				RTK_LOGS(TAG, RTK_LOG_INFO, "wKeyFrameRate: %d\n", ctrl->wKeyFrameRate);
				RTK_LOGS(TAG, RTK_LOG_INFO, "wPFrameRate: %d\n", ctrl->wPFrameRate);
				RTK_LOGS(TAG, RTK_LOG_INFO, "wCompQuality: %d\n", ctrl->wCompQuality);
				RTK_LOGS(TAG, RTK_LOG_INFO, "wCompWindowSize: %d\n", ctrl->wCompWindowSize);
				RTK_LOGS(TAG, RTK_LOG_INFO, "dwMaxVideoFrameSize: %d\n", ctrl->dwMaxVideoFrameSize);
				RTK_LOGS(TAG, RTK_LOG_INFO, "dwMaxPayloadTransferSize: %d\n", ctrl->dwMaxPayloadTransferSize);
				RTK_LOGS(TAG, RTK_LOG_INFO, "dwClockFrequency: %d\n", ctrl->dwClockFrequency);
				RTK_LOGS(TAG, RTK_LOG_INFO, "bmFramingInfo: %d\n", ctrl->bmFramingInfo);
				RTK_LOGS(TAG, RTK_LOG_INFO, "bPreferredVersion: %d\n", ctrl->bPreferredVersion);
				RTK_LOGS(TAG, RTK_LOG_INFO, "bMinVersion: %d\n", ctrl->bMinVersion);
				RTK_LOGS(TAG, RTK_LOG_INFO, "bMaxVersion: %d\n", ctrl->bMaxVersion);
#endif
			} else {
				/* No request buffer: the host values stay in ctrl */
			}
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "P2 err\n");
			stream->state = STREAM_STATE_PROBE_FINAL;
			ret_status = HAL_OK;
		} else {
			/* HAL_BUSY: request in progress, retry on next cycle */
		}
		break;

	case STREAM_STATE_PROBE_FINAL:
		ret = usbh_uvc_stream_ctrl_set_video(stream, 1U);
		if (ret == HAL_OK) {
			stream->state = STREAM_STATE_COMMIT;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "P3 err\n");
			stream->state = STREAM_STATE_COMMIT;
			ret_status = HAL_OK;
		} else {
			/* HAL_BUSY: request in progress, retry on next cycle */
		}
		break;

	/* Commit flow */
	case STREAM_STATE_COMMIT:
		ret = usbh_uvc_stream_ctrl_set_video(stream, 0U);
		if (ret == HAL_OK) {
			/* The frame buffers were sized at usbh_uvc_init() time, long before the device
			 * reported dwMaxVideoFrameSize in PROBE/COMMIT, so they can no longer grow here.
			 * Warn only: many cameras over-report dwMaxVideoFrameSize and still stream fine,
			 * so failing the commit would reject working devices. */
			if (stream->stream_ctrl.dwMaxVideoFrameSize > stream->frame_buffer_size) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "Dev frm size %d > buf %d, frames may be dropped\n",
						 stream->stream_ctrl.dwMaxVideoFrameSize, stream->frame_buffer_size);
			}
			if (stream->set_alt == 1U) {
				stream->state = STREAM_STATE_FIND_ALT;
			} else {
				uvc->state = UVC_STATE_IDLE;
				ret_status = HAL_OK;
			}
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "C err\n");
			if (stream->set_alt == 1U) {
				stream->state = STREAM_STATE_FIND_ALT;
			} else {
				uvc->state = UVC_STATE_IDLE;
				ret_status = HAL_OK;
			}
		} else {
			/* HAL_BUSY: request in progress, retry on next cycle */
		}
		break;

	/* Find Alt */
	case STREAM_STATE_FIND_ALT:
		usbh_uvc_find_alt(stream);
		uvc->state = UVC_STATE_CTRL;
		stream->state = STREAM_STATE_SET_ALT;
		uvc->stream_ctrl_idx = stream->stream_idx;
		usbh_notify(uvc->host, 0, &usbh_uvc_driver);
		ret_status = HAL_OK;
		break;

	/* Set Alt : interface / altesetting */
	case STREAM_STATE_SET_ALT:
		ret = usbh_ctrl_set_interface(host, stream->cur_setting.bInterfaceNumber,
									  stream->cur_setting.bAlternateSetting);
		if (ret == HAL_OK) {
			stream->set_alt = 0;
			stream->set_alt_retry = 0;
			/* ctrl_set_alt_done is the sole URB allocation and pipe-open point for all paths */
			usbh_uvc_ctrl_set_alt_done(uvc, stream);
		} else if (ret != HAL_BUSY) {
			/* USB 2.0 spec 9.4.10: device with single altsetting may STALL SET_INTERFACE. */
			/* Fallback for isoc in uvc device, bulk in device (bAlternateSetting =0) not support yet*/
			if (stream->vs_intf->alt_num == 1U) {
				stream->set_alt = 0;
				stream->set_alt_retry = 0;
				usbh_uvc_ctrl_set_alt_done(uvc, stream);
			} else {
				/* Retry on timeout/error: Hikvision NAKs STATUS IN for several seconds */
				if (stream->set_alt_retry < USBH_UVC_SET_ALT_RETRY_MAX) {
					stream->set_alt_retry++;
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Set %d/%d err%d\n", stream->cur_setting.bInterfaceNumber,
							 stream->cur_setting.bAlternateSetting, stream->set_alt_retry);
					stream->set_alt_retry = 0;
					usbh_uvc_ctrl_set_alt_error(uvc, stream);
				}
			}
		} else {
			/* HAL_BUSY: request in progress, retry on next cycle */
		}
		ret_status = HAL_OK;
		break;

	default:
		ret_status = HAL_OK;
		break;
	}

	return ret_status;
}

/**
  * @brief  UVC Process function (State Machine)
  */
static void usbh_uvc_process(usb_host_t *host, usbh_drv_msg_t *msg)
{
	usbh_uvc_host_t *uvc = &uvc_host;

	switch (uvc->state) {
	case UVC_STATE_STOP:  /* Intentional fallthrough: same handler as CTRL */
	case UVC_STATE_CTRL:
		if (msg != NULL) {
			if (msg->pipe_num == 0x00U) {
				(void)usbh_uvc_process_ctrl(host, msg);
			} else {
				usbh_notify(host, 0, &usbh_uvc_driver);
			}
		}
		break;

	case UVC_STATE_TRANSFER:
		// do nothing, need this state to start isoc-in in sof cb
		break;

	case UVC_STATE_ERROR:
		/* EP0 needs no recovery request: a control pipe clears a protocol STALL by accepting
		 * the next SETUP (USB 2.0 §8.5.3.4) and has no ENDPOINT_HALT feature to clear
		 * (§9.4.5). With no request to issue, neither a disconnect guard nor a retry budget
		 * is needed - the reset to IDLE always completes in one pass. */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UVC err\n");
		uvc->state = UVC_STATE_IDLE;
		break;

	case UVC_STATE_IDLE:
	default:
		/* main task in idle/default status, sleep to release CPU */
		usb_os_sleep_ms(1);
		break;
	}
}

#if (USBH_UVC_USE_HW == 0)
/**
  * @brief  SOF callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  host: Host handle
  * @retval None
  */
static void usbh_uvc_sof(usb_host_t *host)
{
	usbh_uvc_stream_process_sof(host);
}

/**
  * @brief  Pipe transfer completed callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  host: Host handle
  * @param  pipe_num: Pipe number whose transfer completed
  * @retval None
  */
static void usbh_uvc_completed(usb_host_t *host, u8 pipe_num)
{
	usbh_uvc_stream_process_completed(host, pipe_num);
}
#endif

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init UVC class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_uvc_class_init(void)
{
	int ret = HAL_OK;
	ret = usbh_register_class(&usbh_uvc_driver);

	return ret;
}

/**
  * @brief  Deinit UVC class
  * @retval Result of usbh_unregister_class(): on anything but HAL_OK nothing was released
  */
int usbh_uvc_class_deinit(void)
{
	return usbh_unregister_class(&usbh_uvc_driver);
}

