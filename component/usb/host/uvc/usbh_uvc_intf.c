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

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "UVC";

usbh_uvc_host_t uvc_host;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Find suitable format and frame index
  * @param context: user configuration
  * @param format_index: pointer to format index
  * @param frame_index: pointer to frame index
  * @retval Status
  */
static int usbh_uvc_find_format_frame(usbh_uvc_stream_t *stream, usbh_uvc_s_ctx_t *context,
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
		int best_idx = 0;
		tmp1 = 0;

		for (i = 0; i < (int) nframe; i ++) {
			tmp2 = (frame + i)->wWidth * (frame + i)->wHeight;

			if ((tmp2 < h * w) && (tmp2 > tmp1)) {
				tmp1 = tmp2;
				best_idx = i;
			}
		}

		frame += best_idx;

		*frame_index = frame->bFrameIndex;
		context->height = frame->wHeight;
		context->width = frame->wWidth;

		RTK_LOGS(TAG, RTK_LOG_INFO, "Fallback: Use @%d*%d\n", frame->wWidth, frame->wHeight);

	}

	return HAL_OK;
}

/**
  * @brief	Find suitable frame rate
  * @param context: user configuration
  * @param intv: pointer to bInterval value
  * @param format_index: pointer to format index
  * @param frame_index: pointer to frame index
  * @retval None
  */
static void usbh_uvc_find_frame_rate(usbh_uvc_stream_t *stream, usbh_uvc_s_ctx_t *context,
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
int usbh_uvc_init(usbh_uvc_ctx_t *cfg, usbh_uvc_cb_t *cb)
{
	int i, ret = HAL_OK;
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = NULL;

	usb_os_memset(uvc, 0x00, sizeof(usbh_uvc_host_t));
	uvc->request_buf = (u8 *)usb_os_malloc(UBSH_UVC_REQUEST_BUF_LEN);
	if (uvc->request_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc req buf fail\n");
		return HAL_ERR_MEM;
	}

	for (i = 0; i < USBH_UVC_VS_DESC_MAX_NUM; i++) {
		stream = &uvc->stream[i];
		stream->stream_idx = i;
	}

#if USBH_UVC_USE_HW
	uvc->hw_isr_pri = cfg->hw_isr_pri;
	uvc->hw_irq_ref_cnt = 0;
#else
	UNUSED(cfg);
#endif

	usbh_uvc_desc_init();

	usbh_uvc_class_init();

#if USBH_UVC_USE_HW && USBH_UVC_DEBUG
	rtos_sema_create(&uvc->hw_dump_sema, 0, 1);
	uvc->hw_dump_task_exit = 0;
	if (rtos_task_create(&(uvc->hw_dump_task),
						 ((const char *)"uvc_hw_mon"),
						 usbh_uvc_hw_status_dump_thread,
						 NULL,
						 1024U,
						 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create HW dump task fail\n");
	}
#endif

#if (USBH_UVC_USE_HW == 0) && USBH_UVC_DEBUG
	uvc->max_hold_frame_ts = 0;
	uvc->sw_dump_task_exit = 0;
	if (rtos_task_create(&(uvc->sw_dump_task),
						 ((const char *)"uvc_sw_mon"),
						 usbh_uvc_sw_status_dump_thread,
						 NULL,
						 1024U,
						 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create SW dump task fail\n");
	}
#endif

	if (cb != NULL) {
		uvc->cb = cb;
		if (cb->init != NULL) {
			ret = cb->init();
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "User cb init err: %d", ret);
				return ret;
			}
		}
	}

	return ret;
}

/**
  * @brief	Deinit host video class
  * @param	None
  * @retval None
  */
void usbh_uvc_deinit(void)
{
	int i;
	usbh_uvc_host_t *uvc = &uvc_host;

	/* */
	for (i = 0; i < uvc->uvc_desc.vs_num; i++) {
		usbh_uvc_stream_t *stream = &uvc->stream[i];
		stream->next_xfer = 0;
		if (stream->is_resource_safe) {
			rtos_time_delay_ms(10);
			stream->is_resource_safe = 0;
		}
	}

	usbh_uvc_class_deinit();

	for (i = 0; i < uvc->uvc_desc.vs_num; i++) {
		usbh_uvc_stream_t *stream = &uvc->stream[i];
		if (stream->stream_state == STREAMING_ON) {
			usbh_uvc_stream_off(i);
		}
	}

	usbh_uvc_desc_deinit();

	if (uvc->request_buf != NULL) {
		usb_os_mfree(uvc->request_buf);
		uvc->request_buf = NULL;
	}

#if USBH_UVC_USE_HW && USBH_UVC_DEBUG
	if (uvc->hw_dump_task_alive) {
		uvc->hw_dump_task_exit  = 1;

		if (uvc->hw_dump_sema) {
			rtos_sema_give(uvc->hw_dump_sema);
			//thread will detete it
		}

		int wait_cnt = 0;
		do {
			rtos_time_delay_ms(1);
			wait_cnt++;
		} while (uvc->hw_dump_task_alive && wait_cnt < 100);

		if (wait_cnt >= 100) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "HW dump task stop timeout!\n");
		}
	}

	if (uvc->hw_dump_sema) {
		rtos_sema_delete(uvc->hw_dump_sema);
		uvc->hw_dump_sema = NULL;
	}
#endif

#if (USBH_UVC_USE_HW == 0) && USBH_UVC_DEBUG
	if (uvc->sw_dump_task_alive) {
		uvc->sw_dump_task_exit = 1;
		int wait_cnt = 0;
		do {
			rtos_time_delay_ms(1);
			wait_cnt++;
		} while (uvc->sw_dump_task_alive && wait_cnt < 100);

		if (wait_cnt >= 100) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "SW dump task stop timeout!\n");
		}
	}
#endif

	if ((uvc->cb != NULL) && (uvc->cb->deinit != NULL)) {
		uvc->cb->deinit();
	}

}

/**
  * @brief	Enable video streaming
  * @param	None
  * @retval Status
  */
int usbh_uvc_stream_on(usbh_uvc_s_ctx_t *para, u32 itf_num)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = &uvc->stream[itf_num];

	if (stream->stream_state == STREAMING_ON) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream %d is already on\n", itf_num);
		return HAL_OK;
	}

	stream->frame_buffer_size = CACHE_LINE_ALIGNMENT(para->frame_buf_size);
	usbh_uvc_stream_init(stream);

	stream->stream_state = STREAMING_ON;
	stream->next_xfer = 1;
#if (USBH_UVC_USE_HW == 0)
	stream->cur_setting.last_frame = usbh_get_current_frame_number(uvc->host);
	stream->stream_data_state = STREAM_DATA_IN;
#endif
	return HAL_OK;
}

/**
  * @brief	Disable video streaming
  * @param	None
  * @retval Status
  */
int usbh_uvc_stream_off(u32 itf_num)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = &uvc->stream[itf_num];

	if (stream->stream_state == STREAMING_OFF) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream %d is already off\n", itf_num);
		return HAL_OK;
	}

	stream->stream_state = STREAMING_OFF;
	stream->stream_data_state = STREAM_STATE_IDLE;

	usbh_uvc_stream_deinit(stream);

	return HAL_OK;
}

/**
  * @brief	Set video parameter
  * @param	para: user parameter, such as FPS, resolution
  * @retval Status
  * The CFG may have been rewritten to the closest configuration supported by the device.
  * Consumers, such as RTSP, need to proceed with subsequent operations based on the updated CFG
  */
int usbh_uvc_set_param(usbh_uvc_s_ctx_t *para, u32 itf_num)
{
	int ret;
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = &uvc->stream[itf_num];
	usbh_uvc_stream_control_t *ctrl = &stream->stream_ctrl;
	u32 format_index, frame_index;
	u32 frame_intv = 0;

	if ((para->fmt_type != USBH_UVC_FORMAT_MJPEG) && (para->fmt_type != USBH_UVC_FORMAT_YUV) \
		&& (para->fmt_type != USBH_UVC_FORMAT_H264)) {
		return HAL_ERR_PARA;
	}

	/*Find format and closest resolution*/
	ret = usbh_uvc_find_format_frame(stream, para, &format_index, &frame_index);
	if (ret) {
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

	uvc->state = UVC_STATE_CTRL;
	stream->state = STREAM_STATE_SET_PARA;
	uvc->stream_in_ctrl = stream->stream_idx;
	usbh_notify_class_state_change(uvc->host, 0);

	return HAL_OK;
}

/**
  * @brief	Get a frame from video streaming
  * @param	itf_num: Interface number
  * @retval Status
  */
usbh_uvc_frame_t *usbh_uvc_get_frame(u32 itf_num)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_frame_t *frame;
	usbh_uvc_stream_t *stream = &uvc->stream[itf_num];

#if (USBH_UVC_USE_HW == 0)
	if (usb_os_sema_take(stream->frame_sema, USBH_UVC_GET_FRAME_TIMEOUT) == HAL_OK) {

		usb_os_lock(stream->frame_mutex);

		if (stream->stream_state != STREAMING_ON) {
			usb_os_unlock(stream->frame_mutex);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Abort get frame\n");
			return NULL;
		}

		if (list_empty(&stream->frame_chain)) {
			/*should not reach here*/
			usb_os_unlock(stream->frame_mutex);
			RTK_LOGS(TAG, RTK_LOG_INFO, "No frame in frame_chain\n");
			return NULL;
		} else {
			frame = list_first_entry(&stream->frame_chain, usbh_uvc_frame_t, list);
			list_del_init(&frame->list);
			frame->state = UVC_FRAME_INUSE;
			usb_os_unlock(stream->frame_mutex);
#if USBH_UVC_DEBUG
			frame->get_frame_ts = usb_os_get_timestamp_ms();
#endif
			return frame;
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to down frame sema\n");
		return NULL;
	}
#else
	if (usb_os_sema_take(stream->uvc_dec->dec_sema, USBH_UVC_GET_FRAME_TIMEOUT) == HAL_OK) {
		if (stream->stream_state != STREAMING_ON) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Abort get frame\n");
			return NULL;
		}
		stream->frame_buffer[stream->uvc_dec->frame_done_num].byteused = stream->uvc_dec->frame_done_size;
		frame = &stream->frame_buffer[stream->uvc_dec->frame_done_num];
		DCache_Invalidate((u32)frame->buf, (u32)frame->byteused);
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
  * @param	itf_num: Interface number
  * @retval None
  */
void usbh_uvc_put_frame(usbh_uvc_frame_t *frame, u32 itf_num)
{
#if (USBH_UVC_USE_HW == 0)
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = &uvc->stream[itf_num];

	if (!frame) {
		return;
	}

#if USBH_UVC_DEBUG
	u32 hold_time;
	u32 put_frame_ts = usb_os_get_timestamp_ms();
	u32 get_frame_ts = frame->get_frame_ts;

	if (put_frame_ts > get_frame_ts) {
		hold_time = put_frame_ts - get_frame_ts;
	} else {
		hold_time = (0xFFFFFFFF - get_frame_ts) + put_frame_ts + 1;
	}

	if (hold_time > uvc->max_hold_frame_ts) {
		uvc->max_hold_frame_ts = hold_time;
		if (hold_time > 1000) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Slow consumer! Hold: %d ms\n", hold_time);
		}
	}
#endif

	if ((stream->stream_state == STREAMING_ON) && (frame != NULL)) {
		usb_os_lock(stream->frame_mutex);

		list_del_init(&frame->list);
		frame->err = 0;
		frame->byteused = 0;
		frame->timestamp = 0;
#if USBH_UVC_DEBUG
		frame->get_frame_ts = 0;
#endif
		frame->state = UVC_FRAME_INIT;
		list_add_tail(&frame->list, &stream->frame_empty);

		usb_os_unlock(stream->frame_mutex);
	}
#else
	UNUSED(frame);
	UNUSED(itf_num);
#endif
}

