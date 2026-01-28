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

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "UVC";

usbh_uvc_host_t uvc_host;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Find suitable format and frame index
  * @param  context: user configuration
  			format_index: pointer to format index
  			frame_index: pointer to frame index
  * @retval Status
  */
static int usbh_uvc_find_format_frame(usbh_uvc_stream_t *stream, uvc_config_t *context,
									  u32 *format_index, u32 *frame_index)
{
	u32 type, h, w, nformat, nframe, tmp1, tmp2;
	int i;
	int found = 0;
	usbh_uvc_vs_t *vs = stream->vs_intf;
	usbh_uvc_vs_format_t *format = vs->format;
	usbh_uvc_vs_frame_t *frame;
	nformat = vs->nformat;

	type = context->fmt_type;
	h = context->height;
	w = context->width;

	for (i = 0; i < (int) nformat; i ++) {
		if ((format + i)->type == type) {
			format += i;
			*format_index = format->index;
			found = 1;
			break;
		}
	}

	if (found == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to find format\n");
		return HAL_ERR_PARA;
	}

	found = 0;
	nframe = format->nframes;
	frame = format->frame;

	for (i = 0; i < (int) nframe; i ++) {
		if (((frame + i)->wWidth == w) && ((frame + i)->wHeight == h)) {
			frame += i;
			*frame_index = frame->bFrameIndex;
			found = 1;
			break;
		}
	}

	/* Fallback policy:
		- When exact w x h is not supported, pick the "closest not exceeding" frame by area.
		- Measure closeness using area (width * height).
		- Select the largest area that is <= target area (w * h).
		To do:
		- Aspect ratio differences are not considered.
	*/

	if (found == 0) {
		*frame_index = 1;
		tmp1 = 0;
		for (i = 0; i < (int) nframe; i ++) {
			tmp2 = (frame + i)->wWidth * (frame + i)->wHeight;
			if ((tmp2 < h * w) && (tmp2 > tmp1)) {
				tmp1 = (frame + i)->wWidth * (frame + i)->wHeight;
				frame += i;
				*frame_index = frame->bFrameIndex;
			}
		}
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Find passed_in frame @%d*%d fail\n", w, h);

		context->height = frame->wHeight;
		context->width = frame->wWidth;

		RTK_LOGS(TAG, RTK_LOG_INFO, "Use closest frame @%d*%d\n", frame->wWidth, frame->wHeight);
	}

	return HAL_OK;
}

/**
  * @brief	Find suitable frame rate
  * @param	context: user configuration
  			intv: pointer to bInterval value
			format_index: pointer to format index
			frame_index: pointer to frame index
  * @retval None
  */
static void usbh_uvc_find_frame_rate(usbh_uvc_stream_t *stream, uvc_config_t *context,
									 u32 *intv, u32 *format_index, u32 *frame_index)
{
	u32 fps = 10000000 / context->frame_rate;   //unit: us
	int i;

	u32 best = UINT_MAX;
	u32 min, max, step, dist;
	usbh_uvc_vs_t *vs = stream->vs_intf;
	usbh_uvc_vs_format_t *format = vs->format + *format_index - 1;
	usbh_uvc_vs_frame_t *frame = format->frame + *frame_index - 1;

	if (frame->bFrameIntervalType) {
		/*discrete frame interval*/
		for (i = 0; i < frame->bFrameIntervalType; ++i) {
			dist = fps > frame->dwFrameInterval[i]
				   ? fps - frame->dwFrameInterval[i]
				   : frame->dwFrameInterval[i] - fps;

			if (dist > best) {
				break;
			}

			best = dist;
		}

		fps = frame->dwFrameInterval[i - 1];
	} else {
		/*continuous frame interval*/
		min = frame->dwFrameInterval[0];
		max = frame->dwFrameInterval[1];
		step = frame->dwFrameInterval[2];
		fps = min + (fps - min + step / 2) / step * step;

		if (fps > max) {
			fps = max;
		}
	}

	context->frame_rate = 10000000 / fps;
	*intv = fps;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief	Init host video class
  * @param	cb: user callback function
  * @retval Status
  */
int usbh_uvc_init(usbh_uvc_cb_t *cb)
{
	int ret;
	usbh_uvc_host_t *uvc = &uvc_host;

	usb_os_memset(uvc, 0, sizeof(usbh_uvc_host_t));
	uvc->stream[0].stream_num = 0;
	uvc->stream[1].stream_num = 1;

	usbh_uvc_desc_init();

	if (cb != NULL) {
		uvc->cb = cb;
		if (cb->init != NULL) {
			ret = cb->init();
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "User cb init err: %d", ret);
				goto init_fail;
			}
		}
	}

	ret = usbh_uvc_class_init();
	if (ret) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Class init err: %d", ret);
		goto init_fail;
	}

	return HAL_OK;

init_fail:
	usbh_uvc_deinit();
	return ret;
}

/**
  * @brief	Deinit host video class
  * @param	None
  * @retval None
  */
void usbh_uvc_deinit(void)
{
	int ret;
	int i;
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_cb_t *cb = uvc->cb;

	for (i = 0; i < uvc->uvc_desc.vs_num; i++) {
		usbh_uvc_stream_t *stream = &uvc->stream[i];
		if (stream->stream_state == STREAMING_ON) {
			usbh_uvc_stream_off(i);
		}
	}

	usbh_uvc_desc_deinit();

	usbh_uvc_class_deinit();

	if (cb != NULL) {
		uvc->cb = cb;
		if (cb->deinit != NULL) {
			ret = cb->deinit();
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "User cb deinit err: %d", ret);
			}
		}
	}

	uvc->cb = NULL;
}

/**
  * @brief	Enable video streaming
  * @param	None
  * @retval Status
  */
int usbh_uvc_stream_on(u32 if_num)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = &uvc->stream[if_num];

	if (stream->stream_state == STREAMING_ON) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream %d is already on\n", if_num);
		return HAL_OK;
	}

	usbh_uvc_stream_init(stream);

	stream->stream_state = STREAMING_ON;
#if (USBH_UVC_USE_HW == 0)
	stream->cur_setting.last_frame = usbh_get_current_frame(uvc->host);
	stream->stream_data_state = STREAM_DATA_IN;
#endif
	return HAL_OK;
}

/**
  * @brief	Disable video streaming
  * @param	None
  * @retval Status
  */
int usbh_uvc_stream_off(u32 if_num)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = &uvc->stream[if_num];

	if (stream->stream_state == STREAMING_OFF) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream %d is already off\n", if_num);
		return HAL_OK;
	}

	stream->stream_state = STREAMING_OFF;
	stream->stream_data_state = STREAM_STATE_IDLE;

	usbh_uvc_stream_deinit(stream);

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
	usbh_uvc_setting_t *cur_setting = &stream->cur_setting;
	usbh_ep_desc_t *ep = NULL;
	int i;
	u32 xfer_size;
	u32 XferSize = stream->stream_ctrl.dwMaxPayloadTransferSize;
	u32 max_ep_size = UINT_MAX;
	for (i = 0; i < stream->vs_intf->alt_num; i++) {
		xfer_size = stream->vs_intf->altsetting[i].endpoint->wMaxPacketSize;
		if (host->config.speed == USB_SPEED_HIGH) {
			xfer_size = (xfer_size & 0x07ff) * (1 + ((xfer_size >> 11) & 3));
		} else {
			xfer_size = xfer_size & 0x07ff;
		}
		if ((xfer_size >= XferSize) && (xfer_size <= max_ep_size)) {
			cur_setting->altsetting = &stream->vs_intf->altsetting[i];
			cur_setting->bAlternateSetting = ((usbh_if_desc_t *)cur_setting->altsetting->p)->bAlternateSetting;
			ep = cur_setting->altsetting->endpoint;
			cur_setting->ep_addr = ep->bEndpointAddress;
			cur_setting->xfer_size = xfer_size;
			cur_setting->interval = ep->bInterval;
			cur_setting->ep_type = ep->bmAttributes & USB_EP_XFER_TYPE_MASK;
			cur_setting->bInterfaceNumber = cur_setting->cur_vs_intf->bInterfaceNumber;
			cur_setting->mps = ep->wMaxPacketSize & 0x7ff;
			cur_setting->valid = 1;
			max_ep_size = xfer_size;
		}
	}
#if USBH_UVC_DEBUG
	RTK_LOGS(TAG, RTK_LOG_INFO, "Steam->cur_set.altsetting:%x\n",  cur_setting->altsetting);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Steam->cur_set.bAlternateSetting:%d\n", cur_setting->bAlternateSetting);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Steam->cur_set.ep_addr:%d\n", cur_setting->ep_addr);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Steam->cur_set.xfer_size:%d\n", cur_setting->xfer_size);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Steam->cur_set.mps:%d\n", cur_setting->mps);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Steam->cur_set.interval:%d\n", cur_setting->interval);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Steam->cur_set.ep_type:%d\n", cur_setting->ep_type);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Steam->cur_set.bInterfaceNumber:%d\n", cur_setting->bInterfaceNumber);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Steam->cur_set.pipe:%d\n", cur_setting->pipe);
#endif
}

/**
  * @brief	Set video parameter
  * @param	para: user parameter, such as FPS, resolution
  * @retval Status
  * The CFG may have been rewritten to the closest configuration supported by the device.
  * Consumers, such as RTSP, need to proceed with subsequent operations based on the updated CFG
  */
int usbh_uvc_set_param(uvc_config_t *para, u32 if_num)
{
	int ret;
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = &uvc->stream[if_num];
	usbh_uvc_stream_control_t *ctrl = &stream->stream_ctrl;
	usbh_uvc_setting_t *cur_setting = &stream->cur_setting;
	u32 format_index, frame_index;
	u32 frame_intv = 0;

	if ((para->fmt_type != USBH_UVC_FORMAT_MJPEG) && (para->fmt_type != USBH_UVC_FORMAT_YUV) \
		&& (para->fmt_type != USBH_UVC_FORMAT_H264)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Format not support, only support MJEPG, UNCOMPRESSED and H264\n");
		return HAL_ERR_PARA;
	}

	/*Find format and closest resolution*/
	ret = usbh_uvc_find_format_frame(stream, para, &format_index, &frame_index);
	if (ret) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Find format or frame fail\n");
		return HAL_ERR_PARA;
	}

	/*Find closest frame rate*/
	usbh_uvc_find_frame_rate(stream, para, &frame_intv, &format_index, &frame_index);

	usb_os_memset(ctrl, 0, sizeof(usbh_uvc_stream_control_t));
	ctrl->bmHint = 1;  /* dwFrameInterval */
	ctrl->bFormatIndex = format_index;
	ctrl->bFrameIndex = frame_index;
	ctrl->dwFrameInterval = frame_intv;
	ctrl->dwMaxVideoFrameSize = stream->stream_ctrl.dwMaxVideoFrameSize;

	usbh_uvc_probe_video(stream);

	usbh_uvc_commit_video(stream);

	/* find current alt setting */
	usbh_uvc_find_alt(stream);

	do {
		/* set alt setting for current interface */
		ret = usbh_ctrl_set_interface(uvc->host, cur_setting->bInterfaceNumber, \
									  cur_setting->bAlternateSetting);
	} while (ret == HAL_BUSY);

	return HAL_OK;
}

/**
  * @brief	Get a frame from video streaming
  * @param	None
  * @retval Status
  */
usbh_uvc_frame_t *usbh_uvc_get_frame(u32 if_num)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_frame_t *frame;
	usbh_uvc_stream_t *stream = &uvc->stream[if_num];

#if (USBH_UVC_USE_HW == 0)
	if (usb_os_sema_take(stream->frame_sema, USBH_UVC_GET_FRAME_TIMEOUT) == HAL_OK) {
		if (list_empty(&stream->frame_chain)) {
			/*should not reach here*/
			RTK_LOGS(TAG, RTK_LOG_INFO, "No frame in frame_chain\n");
			return NULL;
		} else {
			frame = list_first_entry(&stream->frame_chain, usbh_uvc_frame_t, list);
			list_del_init(&frame->list);
			frame->state = UVC_FRAME_INUSE;
#if USBH_UVC_DEBUG
			RTK_LOGS(TAG, RTK_LOG_INFO, "get %d-%d\n", frame->timestamp, usb_hal_get_timestamp_ms());
#endif
			return frame;
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to down frame sema\n");
		return NULL;
	}
#else
	if (usb_os_sema_take(stream->uvc_dec->dec_sema, USBH_UVC_GET_FRAME_TIMEOUT) == HAL_OK) {
		stream->frame_buffer[stream->uvc_dec->frame_done_num].byteused = stream->uvc_dec->frame_done_size;
		frame = &stream->frame_buffer[stream->uvc_dec->frame_done_num];
		return frame;
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to down frame sema\n");
		return NULL;
	}
#endif
}

/**
  * @brief	Put frame buffer to video streaming
  * @param	frame: uvc frame buffer to put
  * @retval None
  */
void usbh_uvc_put_frame(usbh_uvc_frame_t *frame, u32 if_num)
{
#if (USBH_UVC_USE_HW == 0)
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = &uvc->stream[if_num];

	if (frame != NULL) {
		list_del_init(&frame->list);
		frame->err = 0;
		frame->byteused = 0;
		frame->timestamp = 0;
		frame->state = UVC_FRAME_INIT;
		list_add_tail(&frame->list, &stream->frame_empty);
	}
#else
	UNUSED(frame);
	UNUSED(if_num);
#endif
}

