/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_uvc.h"


/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

extern usbh_uvc_host_t uvc_host;

static int usbh_uvc_attach(usb_host_t *host);
static int usbh_uvc_detach(usb_host_t *host);
static int usbh_uvc_process(usb_host_t *host, usbh_event_t *event);
static int usbh_uvc_setup(usb_host_t *host);
#if (USBH_UVC_USE_HW == 0)
static int usbh_uvc_sof(usb_host_t *host);
static int usbh_uvc_completed(usb_host_t *host, u8 pipe_num);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "UVC";

/* USB UVC device identification */
static const usbh_dev_id_t uvc_devs[] = {
	{
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS,
		.idVendor = 0x0bdb,
		.bInterfaceClass = USBH_UVC_CLASS_CODE,
	},
	{
	},
};

/* USB Host UVC class driver */
static usbh_class_driver_t usbh_uvc_driver = {
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
	usbh_ep_desc_t *ep = NULL;
	usbh_pipe_t *pipe = NULL;
	int status = HAL_ERR_PARA;
	int i = 0;
	u32 xfer_len = 0;

	uvc->host = host;

	usbh_uvc_desc_init();
	status = usbh_uvc_parse_cfgdesc(host);
	if (status) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Parse cfg desc fail\n");
		return status;
	}

	/* find the first alt setting and enpoint as default for each vs interface */
	for (i = 0; i < uvc->uvc_desc.vs_num; i ++) {
		stream = &uvc->stream[i];
		cur_set = &stream->cur_setting;
		cur_set->cur_vs_intf = &uvc->uvc_desc.vs_intf[i];
		alt_set = &cur_set->cur_vs_intf->altsetting[0];
		if (alt_set == NULL || alt_set->p == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "S[%d] no alt\n", i);
			continue;
		}
		cur_set->altsetting = alt_set;
		cur_set->bAlternateSetting = ((usbh_itf_desc_t *)alt_set->p)->bAlternateSetting;
		cur_set->bInterfaceNumber = cur_set->cur_vs_intf->bInterfaceNumber;

		pipe = &cur_set->pipe;
		ep = alt_set->endpoint;
		if (ep == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "S[%d] %d/%d no ep\n", i, cur_set->bInterfaceNumber, cur_set->bAlternateSetting);
			cur_set->valid = 0;
			pipe->ep_addr = 0;
			pipe->xfer_len = 0;
			continue;
		}
		xfer_len = ep->wMaxPacketSize;

		if (host->dev_speed == USB_SPEED_HIGH) {
			xfer_len = (xfer_len & 0x07ff) * (1 + ((xfer_len >> 11) & 3));
		} else {
			xfer_len = xfer_len & 0x07ff;
		}

		//Note: vc may has a interrupt endpoint, vs may has a bulk endpoint for still image data. not support now.
		pipe->ep_addr = ep->bEndpointAddress;
		pipe->ep_mps = ep->wMaxPacketSize & 0x7ff;
		pipe->ep_interval = ep->bInterval;
		pipe->ep_type = ep->bmAttributes & USB_EP_XFER_TYPE_MASK;
		pipe->xfer_len = xfer_len;
		cur_set->valid = 1;

#if USBH_UVC_DEBUG
		RTK_LOGS(TAG, RTK_LOG_INFO, "S[%d] Itf/alt:%d/%d ", i, cur_set->bInterfaceNumber, cur_set->bAlternateSetting);
		RTK_LOGS(TAG, RTK_LOG_INFO, "EP:%d-%d-%d-%d-%d-%d\n", pipe->ep_addr, pipe->xfer_len, pipe->ep_mps, pipe->ep_interval, pipe->ep_type);
#endif
	}

	if ((uvc->cb != NULL) && (uvc->cb->attach != NULL)) {
		uvc->cb->attach();
	}

	status = HAL_OK;
	return status;
}

/**
  * @brief  Detach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uvc_detach(usb_host_t *host)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream;
	usbh_uvc_vs_t *vs_intf;
	int vs_num;
	int i;

	UNUSED(host);
	uvc->state = UVC_STATE_IDLE;
	vs_num = uvc->uvc_desc.vs_num;

	for (i = 0; i < vs_num; i ++) {
		stream = &uvc->stream[i];
		stream->state = STREAM_STATE_CTRL_IDLE;
		if (stream->stream_state == STREAMING_ON) {
			usbh_uvc_stream_stop(stream);
		}
	}

	usbh_uvc_desc_deinit();

	for (i = 0; i < vs_num; i ++) {
		vs_intf = uvc->stream[i].vs_intf;
		if (vs_intf->format) {
			usb_os_mfree(vs_intf->format);
			vs_intf->format = NULL;
		}
	}

	if ((uvc->cb != NULL) && (uvc->cb->detach != NULL)) {
		uvc->cb->detach();
	}
	return HAL_OK;
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
	int i;
	u32 xfer_len = 0;
	u32 best_len;
	u32 XferSize = stream->stream_ctrl.dwMaxPayloadTransferSize;
	u32 max_ep_size = UINT_MAX;

	cur_set->valid = 0;

	for (i = 0; i < stream->vs_intf->alt_num; i++) {
		ep = stream->vs_intf->altsetting[i].endpoint;
		if (ep == NULL) {
			continue;
		}

		xfer_len = ep->wMaxPacketSize;

		if (host->dev_speed == USB_SPEED_HIGH) {
			xfer_len = (xfer_len & USB_EP_MPS_SIZE_MASK) * (1 + ((xfer_len & USB_EP_MPS_TRANS_MASK) >> USB_EP_MPS_TRANS_POS));
		} else {
			xfer_len = xfer_len & USB_EP_MPS_SIZE_MASK;
		}

		if ((xfer_len >= XferSize) && (xfer_len <= max_ep_size)) {
			cur_set->altsetting = &stream->vs_intf->altsetting[i];
			cur_set->bAlternateSetting = ((usbh_itf_desc_t *)cur_set->altsetting->p)->bAlternateSetting;
			cur_set->bInterfaceNumber = cur_set->cur_vs_intf->bInterfaceNumber;
			cur_set->valid = 1;

			pipe = &cur_set->pipe;
			pipe->ep_addr = ep->bEndpointAddress;
			pipe->ep_mps = ep->wMaxPacketSize & 0x7ff;
			pipe->ep_interval = ep->bInterval;
			pipe->ep_type = ep->bmAttributes & USB_EP_XFER_TYPE_MASK;
			pipe->xfer_len = xfer_len;

			max_ep_size = xfer_len;
		}
	}

	/* Fallback: if no alt satisfies xfer_len >= XferSize, use the alt with the largest MPS
	 * rather than proceeding with ep_mps=0 which would break the HW decoder. */
	if (!cur_set->valid) {
		best_len = 0;
		for (i = 0; i < stream->vs_intf->alt_num; i++) {
			ep = stream->vs_intf->altsetting[i].endpoint;
			if (ep == NULL) {
				continue;
			}
			xfer_len = ep->wMaxPacketSize;
			if (host->dev_speed == USB_SPEED_HIGH) {
				xfer_len = (xfer_len & USB_EP_MPS_SIZE_MASK) * (1 + ((xfer_len & USB_EP_MPS_TRANS_MASK) >> USB_EP_MPS_TRANS_POS));
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
				pipe->ep_mps = ep->wMaxPacketSize & 0x7ff;
				pipe->ep_interval = ep->bInterval;
				pipe->ep_type = ep->bmAttributes & USB_EP_XFER_TYPE_MASK;
				pipe->xfer_len = xfer_len;
			}
		}
		if (best_len > 0) {
			cur_set->valid = 1;
			RTK_LOGS(TAG, RTK_LOG_WARN, "Fallback alt %d req %u mps %u\n",
					 cur_set->bAlternateSetting, XferSize, best_len);
		}
	}

#if USBH_UVC_DEBUG
	RTK_LOGS(TAG, RTK_LOG_INFO, "F Itf/alt:%d/%d\n", cur_set->bInterfaceNumber, cur_set->bAlternateSetting);
#endif

	uvc->state = UVC_STATE_CTRL;
	stream->state = STREAM_STATE_SET_ALT;
	uvc->stream_ctrl_idx = stream->stream_idx;
	usbh_notify_class_state_change(uvc->host, 0);
}

/**
  * @brief  Standard control requests handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uvc_setup(usb_host_t *host)
{
	int i;
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream;
	UNUSED(host);

	for (i = 0; i < uvc->uvc_desc.vs_num; i ++) {
		stream = &uvc->stream[i];
		usbh_uvc_video_init(stream);
	}

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
	if (stream->cur_setting.valid == 1U) {
		/* Open pipe */
		stream->state = STREAM_STATE_CTRL_IDLE;
		stream->set_alt = 0x0;
		stream->set_alt_retry = 0;
		usbh_open_pipe(uvc->host, &stream->cur_setting.pipe,
					   stream->cur_setting.altsetting->endpoint);
#if USBH_UVC_DEBUG
		usbh_uvc_setting_t *cur_set = &stream->cur_setting;
		RTK_LOGS(TAG, RTK_LOG_INFO,
				 "Alt %d: ep_addr=0x%02X, mps=%d, interval=%d, type=%d, xfer_len=%d\n",
				 cur_set->bAlternateSetting,
				 cur_set->pipe.ep_addr,
				 cur_set->pipe.ep_mps,
				 cur_set->pipe.ep_interval,
				 cur_set->pipe.ep_type,
				 cur_set->pipe.xfer_len);
#endif
		uvc->state = UVC_STATE_TRANSFER;
		if (uvc->cb && uvc->cb->set_param) {
			uvc->cb->set_param(HAL_OK);
		}
	} else {
		/* Don't need clear feature , just stop all statemachine */
		uvc->state = UVC_STATE_IDLE;
		stream->state = STREAM_STATE_CTRL_IDLE;
		if (uvc->cb && uvc->cb->set_param) {
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

	stream->state = STREAM_STATE_CTRL_IDLE;
	uvc->state = UVC_STATE_ERROR;
	if (uvc->cb && uvc->cb->set_param) {
		uvc->cb->set_param(HAL_ERR_HW);
	}
}

/**
  * @brief  State machine handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uvc_process_ctrl(usb_host_t *host, usbh_event_t *event)
{
	int ret = HAL_OK;
	int ret_status = HAL_BUSY;
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = NULL;
	u32 ctrl_struct_size;
	u8 stream_idx = uvc->stream_ctrl_idx;
	u8 size;
	UNUSED(event);

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
		stream->set_alt = 0x1;
		stream->set_alt_retry = 0;
		stream->state = STREAM_STATE_RESET_ALT;
		break;

	case STREAM_STATE_RESET_ALT:
		ret = usbh_ctrl_set_interface(host, stream->cur_setting.bInterfaceNumber, 0);

		if (ret == HAL_OK) {
			if (stream->set_alt == 1) {
				stream->state = STREAM_STATE_PROBE_NEGOTIATE;
			} else {
				uvc->state = UVC_STATE_IDLE;
				stream->state = STREAM_STATE_CTRL_IDLE;
			}
			ret_status = HAL_OK;
		} else if (ret == HAL_BUSY) {
			/* Retry limit for busy control pipe — prevents infinite spin */
			if (stream->set_alt_retry++ >= USBH_UVC_SET_ALT_RETRY_MAX) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Rset alt busy\n");
				stream->set_alt_retry = 0;
				if (stream->set_alt == 1) {
					stream->state = STREAM_STATE_PROBE_NEGOTIATE;
				} else {
					uvc->state = UVC_STATE_IDLE;
					stream->state = STREAM_STATE_CTRL_IDLE;
				}
				ret_status = HAL_OK;
			}
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set %d/0 err%d\n", stream->cur_setting.bInterfaceNumber, ret);
			if (stream->set_alt == 1) {
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
		ret = usbh_uvc_set_video(stream, 1);
		if (ret == HAL_OK) {
			stream->state = STREAM_STATE_PROBE_UPDATE;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "P1 err\n");
			stream->state = STREAM_STATE_PROBE_UPDATE;
			ret_status = HAL_OK;
		}
		break;

	case STREAM_STATE_PROBE_UPDATE:
		ret = usbh_uvc_get_video(stream, 1, USBH_UVC_GET_CUR);
		if (ret == HAL_OK) {
			stream->state = STREAM_STATE_PROBE_FINAL;
			usbh_uvc_stream_control_t *ctrl = &stream->stream_ctrl;
			size = usbh_uvc_get_ctrl_len_by_version(uvc->uvc_desc.vc_intf.vcheader->bcdUVC);
			ctrl_struct_size = sizeof(usbh_uvc_stream_control_t);
			if (uvc->request_buf) {
				DCache_Invalidate((u32)uvc->request_buf, size);
				usb_os_memcpy((void *) ctrl, (void *)uvc->request_buf, (size < ctrl_struct_size) ? size : ctrl_struct_size);
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
			}
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "P2 err\n");
			stream->state = STREAM_STATE_PROBE_FINAL;
			ret_status = HAL_OK;
		}
		break;

	case STREAM_STATE_PROBE_FINAL:
		ret = usbh_uvc_set_video(stream, 1);
		if (ret == HAL_OK) {
			stream->state = STREAM_STATE_COMMIT;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "P3 err\n");
			stream->state = STREAM_STATE_COMMIT;
			ret_status = HAL_OK;
		}
		break;

	/* Commit flow */
	case STREAM_STATE_COMMIT:
		ret = usbh_uvc_set_video(stream, 0);
		if (ret == HAL_OK) {
			if (stream->set_alt == 1) {
				stream->state = STREAM_STATE_FIND_ALT;
			} else {
				uvc->state = UVC_STATE_IDLE;
				ret_status = HAL_OK;
			}
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "C err\n");
			if (stream->set_alt == 1) {
				stream->state = STREAM_STATE_FIND_ALT;
			} else {
				uvc->state = UVC_STATE_IDLE;
				ret_status = HAL_OK;
			}
		}
		break;

	/* Find Alt */
	case STREAM_STATE_FIND_ALT:
		usbh_uvc_find_alt(stream);
		ret_status = HAL_OK;
		break;

	/* Set Alt : interface / altesetting */
	case STREAM_STATE_SET_ALT:
		ret = usbh_ctrl_set_interface(host, stream->cur_setting.bInterfaceNumber,
									  stream->cur_setting.bAlternateSetting);
		if (ret == HAL_OK) {
			stream->set_alt = 0x0;
			stream->set_alt_retry = 0;
			/* Open pipe after SET_INTERFACE succeeds (align with Linux uvc_init_video_isoc) */
			usbh_uvc_ctrl_set_alt_done(uvc, stream);
		} else if (ret != HAL_BUSY) {
			/* USB 2.0 spec 9.4.10: device with single altsetting may STALL SET_INTERFACE. */
			/* Fallback for isoc in uvc device, bulk in device (bAlternateSetting =0) not support yet*/
			if (stream->vs_intf->alt_num == 1) {
				stream->set_alt = 0x0;
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
static int usbh_uvc_process(usb_host_t *host, usbh_event_t *event)
{
	int ret = HAL_OK;
	usbh_uvc_host_t *uvc = &uvc_host;

	switch (uvc->state) {
	case UVC_STATE_STOP:  /* Intentional fallthrough: same handler as CTRL */
	case UVC_STATE_CTRL:
		if (event) {
			if (event->pipe_num == 0x00) {
				ret = usbh_uvc_process_ctrl(host, event);
			} else {
				usbh_notify_class_state_change(host, 0);
			}
		}
		break;

	case UVC_STATE_TRANSFER:
		// do nothing, need this state to start isoc in in sof cb
		break;

	case UVC_STATE_ERROR:
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UVC err\n");
		/* Do not send clear-feature if device is disconnected; the ctrl request would block. */
		if ((uvc->host != NULL) && (uvc->host->connect_state >= USBH_STATE_SETUP)) {
			ret = usbh_ctrl_clear_feature(host, 0x00U);
			if (ret == HAL_OK) {
				uvc->err_retry_cnt = 0;
				uvc->state = UVC_STATE_IDLE;
			} else {
				/* Clear-feature is best-effort. Force IDLE after max retries to avoid deadlock. */
				if (uvc->err_retry_cnt++ >= USBH_UVC_ERROR_CLEAR_RETRY_MAX) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Clear-feature timeout\n");
					uvc->err_retry_cnt = 0;
					uvc->state = UVC_STATE_IDLE;
				}
				/* Otherwise stay in ERROR and retry next cycle. */
			}
		} else {
			uvc->state = UVC_STATE_IDLE;
		}
		break;

	case UVC_STATE_IDLE:
	default:
		/* main task in idle/default status, sleep to release CPU */
		usb_os_sleep_ms(1);
		break;
	}

	return ret;
}

#if (USBH_UVC_USE_HW == 0)
/**
  * @brief  SOF callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uvc_sof(usb_host_t *host)
{
	usbh_uvc_process_sof(host);
	return HAL_OK;
}

/**
  * @brief  Pipe transfer completed callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  host: Host handle
  * @param  pipe_num: Pipe number whose transfer completed
  * @retval Status
  */
static int usbh_uvc_completed(usb_host_t *host, u8 pipe_num)
{
	usbh_uvc_process_completed(host, pipe_num);
	return HAL_OK;
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
  * @retval void
  */
void usbh_uvc_class_deinit(void)
{
	usbh_unregister_class(&usbh_uvc_driver);
}
