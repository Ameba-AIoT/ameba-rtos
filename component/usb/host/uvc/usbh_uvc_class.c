/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_uvc.h"


/* Private defines -----------------------------------------------------------*/
#define USBH_UVC_DEBUG 0

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

extern usbh_uvc_host_t uvc_host;

static int usbh_uvc_attach(usb_host_t *host);
static int usbh_uvc_detach(usb_host_t *host);
static int usbh_uvc_process(usb_host_t *host, u32 msg);
static int usbh_uvc_sof(usb_host_t *host);
static int usbh_uvc_setup(usb_host_t *host);

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
	.sof = usbh_uvc_sof
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
	usbh_pipe_t *pipe;
	int status = HAL_ERR_UNKNOWN;
	int i = 0;
	u32 xfer_len = 0;

	uvc->host = host;

	status = usbh_uvc_parse_cfgdesc(host);
	if (status) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Parse uvc desc fail\n");
		return status;
	}

	/* find the first alt setting and enpoint as default for each vs interface */
	for (i = 0; i < uvc->uvc_desc.vs_num; i ++) {
		stream = &uvc->stream[i];
		cur_set = &stream->cur_setting;
		cur_set->cur_vs_intf = &uvc->uvc_desc.vs_intf[i];
		alt_set = &cur_set->cur_vs_intf->altsetting[0];
		if (alt_set == NULL || alt_set->p == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Stream[%d] Alt is NULL\n", i);
			continue;
		}
		cur_set->altsetting = alt_set;
		cur_set->bAlternateSetting = ((usbh_itf_desc_t *)alt_set->p)->bAlternateSetting;
		cur_set->bInterfaceNumber = cur_set->cur_vs_intf->bInterfaceNumber;

		pipe = &cur_set->pipe;
		ep = alt_set->endpoint;
		if (ep == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Stream[%d] %d/%d no ep\n", i, cur_set->bInterfaceNumber, cur_set->bAlternateSetting);
			cur_set->valid = 0;
			pipe->ep_addr = 0;
			pipe->xfer_len = 0;
			stream->stream_data_state = STREAM_STATE_IDLE;
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream[%d]->cur_set.altsetting:%d\n", i, cur_set->altsetting);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream[%d]->cur_set.bInterfaceNumber:%d\n", i, cur_set->bInterfaceNumber);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream[%d]->cur_set.bAlternateSetting:%d\n", i, cur_set->bAlternateSetting);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream[%d]->cur_set.pipe.ep_addr:%d\n", i, pipe->ep_addr);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream[%d]->cur_set.pipe.xfer_len:%d\n", i, pipe->xfer_len);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream[%d]->cur_set.pipe.ep_mps:%d\n", i, pipe->ep_mps);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream[%d]->cur_set.pipe.ep_interval:%d\n", i, pipe->ep_interval);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream[%d]->cur_set.pipe.ep_type:%d\n", i, pipe->ep_type);
#endif
		stream->stream_data_state = STREAM_STATE_IDLE;
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
	usbh_pipe_t *pipe;
	int i;

	uvc->state = UVC_STATE_IDLE;
	for (i = 0; i < uvc->uvc_desc.vs_num; i ++) {
		stream = &uvc->stream[i];
		pipe = &stream->cur_setting.pipe;
		if (pipe->pipe_num) {
			usbh_close_pipe(host, pipe);
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
	usbh_ep_desc_t *ep = NULL;
	usbh_pipe_t *pipe;
	int i;
	u32 xfer_len;
	u32 XferSize = stream->stream_ctrl.dwMaxPayloadTransferSize;
	u32 max_ep_size = UINT_MAX;

	for (i = 0; i < stream->vs_intf->alt_num; i++) {

		xfer_len = stream->vs_intf->altsetting[i].endpoint->wMaxPacketSize;

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

			ep = cur_set->altsetting->endpoint;
			pipe = &cur_set->pipe;
			usbh_open_pipe(uvc->host, pipe, ep);

			max_ep_size = xfer_len;
			pipe->xfer_len = xfer_len;
		}
	}

#if USBH_UVC_DEBUG
	RTK_LOGS(TAG, RTK_LOG_INFO, "Steam->cur_set.altsetting:%x\n",  cur_set->altsetting);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Steam->cur_set.bInterfaceNumber:%d\n", cur_set->bInterfaceNumber);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Steam->cur_set.bAlternateSetting:%d\n", cur_set->bAlternateSetting);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Steam->cur_set.pipe.ep_addr:%d\n", pipe->ep_addr);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Steam->cur_set.pipe.xfer_len:%d\n", pipe->xfer_len);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Steam->cur_set.pipe.ep_mps:%d\n", pipe->ep_mps);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Steam->cur_set.pipe.ep_interval:%d\n", pipe->ep_interval);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Steam->cur_set.pipe.ep_type:%d\n", pipe->ep_type);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Steam->cur_set.pipe.pipe_num:%d\n", pipe->pipe_num);
#endif
	uvc->state = UVC_STATE_CTRL;
	stream->state = STREAM_STATE_SET_ALT;
	uvc->stream_in_ctrl = stream->stream_idx;
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
		stream->request_buf = (u8 *)usb_os_malloc(UBSH_UVC_REQUEST_BUF_LEN);
		usbh_uvc_video_init(stream);
	}

	if ((uvc->cb != NULL) && (uvc->cb->setup != NULL)) {
		uvc->cb->setup();
	}

	return HAL_OK;
}

/**
  * @brief  State machine handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uvc_process_ctrl(usb_host_t *host, u32 msg)
{
	int ret = HAL_OK;
	int ret_status = HAL_BUSY;
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = NULL;
	u8 stream_idx = uvc->stream_in_ctrl;
	u8 size;
	UNUSED(msg);

	if (stream_idx >= uvc->uvc_desc.vs_num) {// actual vs num
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Err STREAM[%d]\n", stream_idx);
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
		stream->state = STREAM_STATE_PROBE_NEGOTIATE;
		break;

	/* Probe flow */
	case STREAM_STATE_PROBE_NEGOTIATE:
		ret = usbh_uvc_set_video(stream, 1);
		if (ret == HAL_OK) {
			stream->state = STREAM_STATE_PROBE_UPDATE;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Probe1 err\n");
			stream->state = STREAM_STATE_PROBE_UPDATE;
			ret_status = HAL_OK;
		}
		break;

	case STREAM_STATE_PROBE_UPDATE:
		ret = usbh_uvc_get_video(stream, 1, USBH_UVC_GET_CUR);
		if (ret == HAL_OK) {
			stream->state = STREAM_STATE_PROBE_FINAL;
			usbh_uvc_stream_control_t *ctrl = &stream->stream_ctrl;
			size = (uvc->uvc_desc.vc_intf.vcheader->bcdUVC >= 0x110) ? 34 : 26;
			usb_os_memcpy((void *) ctrl, (void *)stream->request_buf, size);
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
			RTK_LOGS(TAG, RTK_LOG_INFO, "bPreferedVersion: %d\n", ctrl->bPreferedVersion);
			RTK_LOGS(TAG, RTK_LOG_INFO, "bMinVersion: %d\n", ctrl->bMinVersion);
			RTK_LOGS(TAG, RTK_LOG_INFO, "bMaxVersion: %d\n", ctrl->bMaxVersion);
#endif

		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Probe2 err\n");
			stream->state = STREAM_STATE_PROBE_FINAL;
			ret_status = HAL_OK;
		}
		break;

	case STREAM_STATE_PROBE_FINAL:
		ret = usbh_uvc_set_video(stream, 1);
		if (ret == HAL_OK) {
			stream->state = STREAM_STATE_COMMIT;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Probe3 err\n");
			stream->state = STREAM_STATE_COMMIT;
			ret_status = HAL_OK;
		}
		break;

	/* Commit flow */
	case STREAM_STATE_COMMIT:
		ret = usbh_uvc_set_video(stream, 0); // Commit
		if (ret == HAL_OK) {
			if (stream->set_alt == 1) {
				stream->state = STREAM_STATE_FIND_ALT;
			} else {
				uvc->state = UVC_STATE_IDLE;
				ret_status = HAL_OK;
			}
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Commit err\n");
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
			uvc->state = UVC_STATE_TRANSFER;
			ret_status = HAL_OK;
			// notify app to start stream on
			if ((uvc->cb != NULL) && (uvc->cb->setparam != NULL)) {
				uvc->cb->setparam();
			}
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set alt err\n");
			uvc->state = UVC_STATE_TRANSFER;
			ret_status = HAL_OK;
		}
		break;

	/* Set Alt : interface / 0 */
	case STREAM_STATE_SET_CTRL:
		ret = usbh_ctrl_set_interface(host, stream->cur_setting.bInterfaceNumber, 0);
		if (ret == HAL_OK) {
			stream->set_alt = 0x0;
			uvc->state = UVC_STATE_IDLE;
			ret_status = HAL_OK;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set ctrl err\n");
			uvc->state = UVC_STATE_IDLE;
			ret_status = HAL_OK;
		}
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
static int usbh_uvc_process(usb_host_t *host, u32 msg)
{
	int i, ret = HAL_OK;
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_event_t event;
	usbh_uvc_stream_t *stream = NULL;
	event.d32 = msg;

	switch (uvc->state) {
	case UVC_STATE_CTRL:
		if (event.msg.pipe_num == 0x00) {
			ret = usbh_uvc_process_ctrl(host, msg);
		} else {
			usbh_notify_class_state_change(host, 0);
		}

		break;

	case UVC_STATE_TRANSFER:
		for (i = 0; i < uvc->uvc_desc.vs_num; i++) {
			if (uvc->stream[i].cur_setting.pipe.pipe_num == event.msg.pipe_num) {
				stream = &uvc->stream[i];
				break;
			}
		}

		if ((stream != NULL) && (stream->stream_data_state == STREAM_DATA_IN)) {
			usbh_uvc_process_rx(stream);
		}
		break;

	case UVC_STATE_ERROR:
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UVC err\n");
		ret = usbh_ctrl_clear_feature(host, 0x00U);
		if (ret == HAL_OK) {
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

/**
  * @brief  SOF callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uvc_sof(usb_host_t *host)
{
	usbh_uvc_process_sof(host);

	return HAL_OK;
}

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

