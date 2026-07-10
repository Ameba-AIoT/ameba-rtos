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
  * @param  stream: uvc stream interface
  * @param  context: user configuration (width/height/fmt_type)
  * @param  format_index: pointer to format index (output)
  * @param  frame_index: pointer to frame index (output)
  * @retval Status
  */
static int usbh_uvc_find_format_frame(usbh_uvc_stream_t *stream, usbh_uvc_s_ctx_t *context,
									  u32 *format_index, u32 *frame_index)
{
	usbh_uvc_vs_format_t *format;
	usbh_uvc_vs_frame_t  *frame;
	usbh_uvc_vs_frame_t  *best;
	usbh_uvc_vs_t *vs;
	u32 w;
	u32 h;
	u32 i;
	u32 dist;
	u32 best_dist;
	u32 dw;
	u32 dh;

	if (!stream || !context || !format_index || !frame_index) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Err args %x-%x\n", (u32)stream, (u32)context);
		return HAL_ERR_PARA;
	}

	vs = stream->vs_intf;
	if (!vs || !vs->format) {
		return HAL_ERR_PARA;
	}

	w = context->width;
	h = context->height;

	/* Step 1: locate format by subtype. */
	format = NULL;
	for (i = 0U; i < vs->nformat; i++) {
		if (vs->format[i].type == context->fmt_type) {
			format = &vs->format[i];
			break;
		}
	}
	if ((format == NULL) || (format->frame == NULL)) {
		return HAL_ERR_PARA;
	}

	/* Step 2: sync Linux UVC, pick frame minimizing |Δw| + |Δh|. */
	best = NULL;
	best_dist = (u32) - 1;

	for (i = 0u; i < format->nframes; i++) {
		frame = &format->frame[i];

		dw = (frame->wWidth > w) ? (frame->wWidth - w) : (w - frame->wWidth);
		dh = (frame->wHeight > h) ? (frame->wHeight - h) : (h - frame->wHeight);
		dist = dw + dh;

		if (dist < best_dist) {
			best_dist = dist;
			best = frame;
			if (dist == 0u) {
				break;/* exact match */
			}
		}
	}

	if (best == NULL) {
		return HAL_ERR_PARA;
	}

	*format_index = format->index;
	*frame_index = best->bFrameIndex;

	/* Write negotiated values back so the caller learns what was actually selected */
	context->width  = best->wWidth;
	context->height = best->wHeight;

	return HAL_OK;
}

/**
  * @brief	Find suitable frame rate
  * @param context: user configuration
  * @param intv: pointer to bInterval value
  * @param format_index: pointer to format index
  * @param frame_index: pointer to frame index
  * @retval Status
  */
static int usbh_uvc_find_frame_rate(usbh_uvc_stream_t *stream, usbh_uvc_s_ctx_t *context,
									u32 *intv, u32 *format_index, u32 *frame_index)
{
	usbh_uvc_vs_format_t *format;
	usbh_uvc_vs_frame_t  *frame;
	u32 i;
	u32 fps;
	u32 cur;
	u32 dist;
	u32 best;
	u32 min_intv;
	u32 max_intv;
	u32 step;

	if ((stream == NULL) || (context == NULL) || (intv == NULL) ||
		(format_index == NULL) || (frame_index == NULL)) {
		return HAL_ERR_PARA;
	}
	if (context->frame_rate == 0u) {
		return HAL_ERR_PARA;
	}

	/* Guard against NULL vs_intf (e.g. called before attach or after detach). */
	if (stream->vs_intf == NULL || stream->vs_intf->format == NULL) {
		return HAL_ERR_PARA;
	}

	/* Locate format. */
	format = NULL;
	for (i = 0u; i < stream->vs_intf->nformat; i++) {
		if (stream->vs_intf->format[i].index == *format_index) {
			format = &stream->vs_intf->format[i];
			break;
		}
	}
	if (format == NULL) {
		return HAL_ERR_PARA;
	}

	/* Locate frame. */
	frame = NULL;
	for (i = 0u; i < format->nframes; i++) {
		if (format->frame[i].bFrameIndex == *frame_index) {
			frame = &format->frame[i];
			break;
		}
	}
	if (frame == NULL) {
		return HAL_ERR_PARA;
	}

	fps = USBH_UVC_FRAME_INTERVAL_UNIT_NS / context->frame_rate;

	if (frame->bFrameIntervalType != 0u) {
		/* Discrete: dwFrameInterval[] sorted ascending. */
		best = (u32) - 1;
		for (i = 0u; i < frame->bFrameIntervalType; i++) {
			cur  = frame->dwFrameInterval[i];
			dist = (fps > cur) ? (fps - cur) : (cur - fps);

			if (dist > best) {
				break;
			}
			best = dist;
		}
		/* Guard i == 0 (single-entry list / immediate-break edge). */
		fps = frame->dwFrameInterval[(i == 0u) ? 0u : (i - 1U)];
	} else {
		/* Continuous: { min, max, step }. */
		min_intv = frame->dwFrameInterval[0];
		max_intv = frame->dwFrameInterval[1];
		step = frame->dwFrameInterval[2];

		if (fps < min_intv) {
			fps = min_intv;
		} else if (fps > max_intv) {
			fps = max_intv;
		} else if (step == 0u) {
			fps = min_intv;
		} else {
			fps = min_intv + (fps - min_intv + step / 2U) / step * step;
			if (fps > max_intv) {
				fps = max_intv;
			}
		}
	}

	/* Guard against a malformed descriptor reporting a 0 interval (div-by-zero). */
	if (fps == 0u) {
		return HAL_ERR_PARA;
	}

	*intv = fps;
	context->frame_rate = USBH_UVC_FRAME_INTERVAL_UNIT_NS / fps;
	return HAL_OK;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief	Init host video class
  * @param	cb: user callback function
  * @retval Status
  */
int usbh_uvc_init(const usbh_uvc_ctx_t *cfg, const usbh_uvc_cb_t *cb)
{
	int i;
	int ret = HAL_OK;
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = NULL;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	usb_os_memset(uvc, 0x00, sizeof(usbh_uvc_host_t));
	uvc->request_buf = (u8 *)usb_os_malloc(USBH_UVC_REQUEST_BUF_LEN);
	if (uvc->request_buf == NULL) {
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

	usbh_uvc_class_init();

#if USBH_UVC_USE_HW && USBH_UVC_DEBUG
	usb_os_sema_create(&uvc->hw_dump_sema);
	uvc->hw_dump_task_exit = 0;
	if (rtos_task_create(&(uvc->hw_dump_task),
						 ((const char *)"usbh_uvc_hw_status_dump_thread"),
						 usbh_uvc_hw_status_dump_thread,
						 NULL,
						 512U,
						 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create HW dump task fail\n");
	}
#endif

#if (USBH_UVC_USE_HW == 0) && USBH_UVC_DEBUG
	uvc->max_hold_frame_ts = 0;
	uvc->sw_dump_task_exit = 0;
	if (rtos_task_create(&(uvc->sw_dump_task),
						 ((const char *)"usbh_uvc_sw_status_dump_thread"),
						 usbh_uvc_sw_status_dump_thread,
						 NULL,
						 768U,
						 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create SW dump task fail\n");
	}
#endif

	uvc->cb = cb;
	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "User init err %d\n", ret);
			/* Roll back driver resources allocated above so a failed user init
			 * does not leak request_buf or leave the class registered.
			 * (Any DEBUG dump task is torn down by usbh_uvc_deinit if needed.) */
			usbh_uvc_class_deinit();
			if (uvc->request_buf != NULL) {
				usb_os_mfree(uvc->request_buf);
				uvc->request_buf = NULL;
			}
			return ret;
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
	usbh_uvc_stream_t *stream;

	usbh_uvc_class_deinit();

	for (i = 0; i < uvc->uvc_desc.vs_num; i++) {
		stream = &uvc->stream[i];
		usbh_uvc_stream_deinit(stream);
	}

	if (uvc->request_buf != NULL) {
		usb_os_mfree(uvc->request_buf);
		uvc->request_buf = NULL;
	}

#if USBH_UVC_USE_HW && USBH_UVC_DEBUG
	if (uvc->hw_dump_task_alive) {
		uvc->hw_dump_task_exit  = 1;

		if (uvc->hw_dump_sema) {
			usb_os_sema_give(uvc->hw_dump_sema);
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
		usb_os_sema_delete(uvc->hw_dump_sema);
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

	uvc->uvc_desc.vs_num = 0;

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
	usbh_uvc_stream_t *stream;
	int ret;

	if ((itf_num >= USBH_UVC_VS_DESC_MAX_NUM) || ((para == NULL) || (para->frame_buf_size == 0))) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Paras err\n");
		return HAL_ERR_PARA;
	}

#if USBH_UVC_USE_HW
	if (para->frame_buf_size <= USBH_HW_UVC_ISOC_MPS) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Buf size err\n");
		return HAL_ERR_PARA;
	}
#endif

	stream = &uvc->stream[itf_num];
	if (stream->stream_state == STREAMING_ON) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "S%d was on\n", itf_num);
		return HAL_OK;
	}

	stream->frame_buffer_size = CACHE_LINE_ALIGNMENT(para->frame_buf_size);
	ret = usbh_uvc_stream_init(stream);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "S%d init fail(%d)\n", itf_num, ret);
		stream->stream_state = STREAMING_OFF;
		return ret;
	}

	stream->stream_state = STREAMING_ON;
#if (USBH_UVC_USE_HW == 0)
	stream->next_xfer = 1;
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
	usbh_uvc_stream_t *stream = NULL;

	if (itf_num >= USBH_UVC_VS_DESC_MAX_NUM) {
		return HAL_ERR_PARA;
	}

	stream = &uvc->stream[itf_num];

	usbh_uvc_stream_stop(stream);
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
	usbh_uvc_stream_t *stream = NULL;
	usbh_uvc_stream_control_t *ctrl = NULL;
	u32 format_index = 0;
	u32 frame_index = 0;
	u32 frame_intv = 0;
	u32 max_frame_size = 0;
	u32 max_xfer_size = 0;

	if (!para) {
		return HAL_ERR_PARA;
	}

	if ((para->fmt_type != USBH_UVC_FORMAT_MJPEG) && (para->fmt_type != USBH_UVC_FORMAT_YUV) \
		&& (para->fmt_type != USBH_UVC_FORMAT_H264)) {
		return HAL_ERR_PARA;
	}

	if (itf_num >= USBH_UVC_VS_DESC_MAX_NUM) {
		return HAL_ERR_PARA;
	}

	stream = &uvc->stream[itf_num];
	uvc->stream_ctrl_idx = stream->stream_idx;
	/*Find format and closest resolution*/
	ret = usbh_uvc_find_format_frame(stream, para, &format_index, &frame_index);
	if (ret) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Find fmt frm fail\n");
		return HAL_ERR_PARA;
	}

	/*Find closest frame rate*/
	ret = usbh_uvc_find_frame_rate(stream, para, &frame_intv, &format_index, &frame_index);
	if (ret) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Find fmt rate fail\n");
		return HAL_ERR_PARA;
	}

	ctrl = &stream->stream_ctrl;
	max_frame_size = ctrl->dwMaxVideoFrameSize;
	max_xfer_size = ctrl->dwMaxPayloadTransferSize;
	usb_os_memset(ctrl, 0, sizeof(usbh_uvc_stream_control_t));
	ctrl->bmHint = 1;  /* dwFrameInterval */
	ctrl->bFormatIndex = format_index;
	ctrl->bFrameIndex = frame_index;
	ctrl->dwFrameInterval = frame_intv;
	/* Restore values negotiated in previous PROBE round for cameras that validate these fields */
	ctrl->dwMaxVideoFrameSize = max_frame_size;
	ctrl->dwMaxPayloadTransferSize = max_xfer_size;

	uvc->state = UVC_STATE_CTRL;
	stream->state = STREAM_STATE_SET_PARA;
	if (uvc->host != NULL) {
		usbh_notify_class_state_change(uvc->host, 0);
	}

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
	usbh_uvc_stream_t *stream = NULL;

	if (itf_num >= USBH_UVC_VS_DESC_MAX_NUM) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Intf num ovfl\n");
		return NULL;
	}

	stream = &uvc->stream[itf_num];

	if (!stream->get_valid) {
		stream->get_valid = 1;
	}

	if (stream->stream_state != STREAMING_ON) {
		goto exit;
	}

#if (USBH_UVC_USE_HW == 0)
	usb_os_sema_t frame_sema  = stream->frame_sema;
	usb_os_lock_t frame_mutex = stream->frame_mutex;
	if (!frame_sema) {
		goto exit;
	}

	if (usb_os_sema_take(frame_sema, USBH_UVC_GET_FRAME_TIMEOUT) == HAL_OK) {

		usb_os_lock(frame_mutex);

		if (stream->stream_state != STREAMING_ON) {
			usb_os_unlock(frame_mutex);
			goto exit;
		}

		if (list_empty(&stream->frame_chain)) {
			/*should not reach here*/
			usb_os_unlock(frame_mutex);
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "No frm in c\n");
			return NULL;
		} else {
			frame = list_first_entry(&stream->frame_chain, usbh_uvc_frame_t, list);
			list_del_init(&frame->list);
			frame->state = UVC_FRAME_INUSE;
			usb_os_unlock(frame_mutex);
#if USBH_UVC_DEBUG
			frame->get_frame_ts = usb_os_get_timestamp_ms();
#endif
			return frame;
		}
	} else {
		goto exit;
	}
#else

	usbh_hw_uvc_dec_t *uvc_dec = stream->uvc_dec;
	if (!uvc_dec) {
		goto exit;
	}

	usb_os_sema_t dec_sema = uvc_dec->dec_sema;
	if (!dec_sema) {
		goto exit;
	}

	if (usb_os_sema_take(dec_sema, USBH_UVC_GET_FRAME_TIMEOUT) == HAL_OK) {
		if ((stream->stream_state != STREAMING_ON) || !uvc_dec) {
			goto exit;
		}

		uvc_dec = stream->uvc_dec;
		u32 frame_done_cnt  = uvc_dec->frame_done_cnt;
		u32 frame_done_size = uvc_dec->frame_done_size;
		/* dec_sema take (xQueueSemaphoreTake) + give (xQueueGiveFromISR) share the
		 * same task_lock spinlock internally, providing a happens-before barrier
		 * that makes the ISR writes of frame_done_cnt/size visible here. */

		frame = &stream->frame_buffer[frame_done_cnt];
		frame->byteused = frame_done_size;

		/* cache invalidate — HW decoder writes to PSRAM, CPU cache stale */
		DCache_Invalidate((u32)frame->buf, CACHE_LINE_ALIGNMENT(frame->byteused));
		return frame;
	} else {
		goto exit;
	}
#endif

exit:
	stream->get_valid = 0;
	if (stream->stream_state != STREAMING_ON) {
		stream->get_exit = 1;
	} else {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Get fail\n");
	}
	return NULL;
}

/**
  * @brief	Put frame buffer back to video streaming empty list
  * @param	frame: uvc frame buffer to put
  * @param	itf_num: Interface number
  * @retval HAL_OK on success, HAL_ERR_PARA on invalid parameters
  */
int usbh_uvc_put_frame(usbh_uvc_frame_t *frame, u32 itf_num)
{
#if (USBH_UVC_USE_HW == 0)
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = NULL;

	if ((!frame) || (itf_num >= USBH_UVC_VS_DESC_MAX_NUM)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Intf num ovfl\n");
		return HAL_ERR_PARA;
	}

	stream = &uvc->stream[itf_num];
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
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Slow,h:%dms\n", hold_time);
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
	return HAL_OK;
#else
	UNUSED(frame);
	UNUSED(itf_num);
	return HAL_OK;
#endif
}

/**
  * @brief  Dump dev capability info (format, frame, resolution, fps).
  * @retval None.
  */
void usbh_uvc_dump_dev_info(void)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = NULL;
	usbh_uvc_vs_t *vs = NULL;
	usbh_uvc_vs_format_t *fmt = NULL;
	usbh_uvc_vs_frame_t *frm = NULL;
	const char *fmt_name = NULL;
	u32 si = 0;
	u32 i = 0;
	u32 j = 0;

	for (si = 0; si < USBH_UVC_VS_DESC_MAX_NUM; si++) {
		stream = &uvc->stream[si];
		vs = stream->vs_intf;
		if (vs == NULL) {
			continue;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Intf %d:\n", si);
		for (i = 0; i < vs->nformat; i++) {
			fmt = &vs->format[i];
			if (fmt->type == 4) {
				fmt_name = "UNCOMPRESSED";
			} else if (fmt->type == 6) {
				fmt_name = "MJPEG";
			} else if (fmt->type == 0x10) {
				fmt_name = "H264";
			} else {
				fmt_name = "UNKNOWN";
			}

			RTK_LOGS(TAG, RTK_LOG_INFO, "Format %d: %s (%d frame%s)\n",
					 (u32)i, fmt_name, fmt->nframes,
					 (fmt->nframes > 1) ? "s" : "");

			for (j = 0; j < fmt->nframes; j++) {
				u32 fps;
				frm = &fmt->frame[j];
				/* Avoid div-by-zero if a descriptor reports a 0 default interval. */
				fps = (frm->dwFrameInterval && frm->dwFrameInterval[0]) ?
					  (USBH_UVC_FRAME_INTERVAL_UNIT_NS / frm->dwFrameInterval[0]) : 0;
				RTK_LOGS(TAG, RTK_LOG_INFO, "  %d x %d @ %d fps\n", frm->wWidth, frm->wHeight, fps);
			}
		}
	}
}