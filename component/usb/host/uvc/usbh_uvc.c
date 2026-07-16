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
#define USBH_UVC_REQUEST_BUF_LEN                48U /**< Length of the UVC control request buffer. */

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#define USBH_UVC_SLOW_HOLD_MS        (1000U)
/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "UVC";

usbh_uvc_host_t uvc_host;

/* Exported functions --------------------------------------------------------*/

/**
  * @brief	Init host video class
  * @param	cb: user callback function
  * @retval Status
  */
int usbh_uvc_init(const usbh_uvc_ctx_t *cfg, const usbh_uvc_cb_t *cb)
{
	int ret = HAL_OK;
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = NULL;
	u8 i;

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
	uvc->hw_irq_ref_cnt = 0U;
#else
	UNUSED(cfg);
#endif

	usbh_uvc_class_init();

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

#if USBH_UVC_DEBUG
#if (USBH_UVC_USE_HW == 1)
	usb_os_sema_create(&uvc->hw_dump_sema);
	uvc->hw_dump_task_exit = 0U;
	ret = rtos_task_create(&(uvc->hw_dump_task),
						   ((const char *)"usbh_uvc_hw_status_dump_thread"),
						   usbh_uvc_hw_status_dump_thread,
						   NULL,
						   512U,
						   1U);
	if (ret != RTK_SUCCESS) {
		usb_os_sema_delete(uvc->hw_dump_sema);
		uvc->hw_dump_sema = NULL;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create HW dump task fail\n");
	}
#else
	uvc->max_hold_frame_ms = 0U;
	uvc->sw_dump_task_exit = 0U;
	ret = rtos_task_create(&(uvc->sw_dump_task),
						   ((const char *)"usbh_uvc_sw_status_dump_thread"),
						   usbh_uvc_sw_status_dump_thread,
						   NULL,
						   1024U,
						   1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create SW dump task fail\n");
	}
#endif
#endif

	return ret;
}

/**
  * @brief	Deinit host video class
  * @param	None
  * @retval None
  */
void usbh_uvc_deinit(void)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream;
	u8 i;
#if USBH_UVC_DEBUG
	u8 wait_cnt = 0U;
#endif

	usbh_uvc_class_deinit();

	for (i = 0U; i < uvc->uvc_desc.vs_num; i++) {
		stream = &uvc->stream[i];
		usbh_uvc_stream_close(stream);
	}

	if (uvc->request_buf != NULL) {
		usb_os_mfree(uvc->request_buf);
		uvc->request_buf = NULL;
	}

#if USBH_UVC_USE_HW && USBH_UVC_DEBUG
	if (uvc->hw_dump_task_alive != 0U) {
		uvc->hw_dump_task_exit = 1U;

		if (uvc->hw_dump_sema != NULL) {
			usb_os_sema_give(uvc->hw_dump_sema);
			// thread will delete it
		}

		do {
			rtos_time_delay_ms(1U);
			wait_cnt++;
		} while ((uvc->hw_dump_task_alive != 0U) && (wait_cnt < 100U));

		if (wait_cnt >= 100U) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "HW dump task stop timeout!\n");
		}
	}

#endif

#if (USBH_UVC_USE_HW == 0) && USBH_UVC_DEBUG
	if (uvc->sw_dump_task_alive != 0U) {
		uvc->sw_dump_task_exit = 1U;

		do {
			rtos_time_delay_ms(1U);
			wait_cnt++;
		} while ((uvc->sw_dump_task_alive != 0U) && (wait_cnt < 100U));

		if (wait_cnt >= 100U) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "SW dump task stop timeout!\n");
		}
	}
#endif

	uvc->uvc_desc.vs_num = 0U;

	if ((uvc->cb != NULL) && (uvc->cb->deinit != NULL)) {
		uvc->cb->deinit();
	}

}

/**
  * @brief	Open a UVC stream: acquire all runtime resources for the interface.
  *         Resource-axis entry of the public lifecycle; pair with usbh_uvc_close().
  *         Delegates the actual allocation to the stream mechanic usbh_uvc_stream_open().
  * @param	para: user parameter (frame buffer size, etc.)
  * @param	itf_num: Interface number
  * @retval Status
  */
int usbh_uvc_open(usbh_uvc_s_ctx_t *para, u8 itf_num)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream;
	int ret;

	if ((itf_num >= USBH_UVC_VS_DESC_MAX_NUM) || ((para == NULL) || (para->frame_buf_size == 0U))) {
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
	if (stream->stream_state != UVC_STREAM_OFF) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "S%d was on\n", itf_num);
		return HAL_OK;
	}

	stream->frame_buffer_size = CACHE_LINE_ALIGNMENT(para->frame_buf_size);
	ret = usbh_uvc_stream_open(stream);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "S%d open fail(%d)\n", itf_num, ret);
		stream->stream_state = UVC_STREAM_OFF;
		return ret;
	}

	return HAL_OK;
}

/**
  * @brief	Close a UVC stream: release all runtime resources for the interface.
  *         Resource-axis exit of the public lifecycle; pair with usbh_uvc_open().
  *         Delegates the actual release to the stream mechanic usbh_uvc_stream_close().
  * @param	itf_num: Interface number
  * @retval Status
  */
int usbh_uvc_close(u8 itf_num)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = NULL;

	if (itf_num >= USBH_UVC_VS_DESC_MAX_NUM) {
		return HAL_ERR_PARA;
	}

	stream = &uvc->stream[itf_num];

	usbh_uvc_stream_close(stream);

	return HAL_OK;
}

/**
  * @brief	Set video parameter
  * @param	para: user parameter, such as FPS, resolution
  * @retval Status
  * The CFG may have been rewritten to the closest configuration supported by the device.
  * Consumers, such as RTSP, need to proceed with subsequent operations based on the updated CFG
  */
int usbh_uvc_set_param(usbh_uvc_s_ctx_t *para, u8 itf_num)
{
	int ret;
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = NULL;
	usbh_uvc_stream_control_t *ctrl = NULL;
	u32 format_idx = 0U;
	u32 frame_idx = 0U;
	u32 frame_interval = 0U;
	u32 max_frame_size = 0U;
	u32 max_xfer_size = 0U;

	if (para == NULL) {
		return HAL_ERR_PARA;
	}

	if ((para->fmt_type != USBH_UVC_FORMAT_MJPEG) && (para->fmt_type != USBH_UVC_FORMAT_YUV)
		&& (para->fmt_type != USBH_UVC_FORMAT_H264) && (para->fmt_type != USBH_UVC_FORMAT_H265)) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Unknown fmt type, just capture not save\n");
	}

	if (itf_num >= USBH_UVC_VS_DESC_MAX_NUM) {
		return HAL_ERR_PARA;
	}

	stream = &uvc->stream[itf_num];
	uvc->stream_ctrl_idx = stream->stream_idx;
	/*Find format and closest resolution*/
	ret = usbh_uvc_desc_find_format_frame(stream, para, &format_idx, &frame_idx);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Find fmt frm fail\n");
		return HAL_ERR_PARA;
	}

	/*Find closest frame rate*/
	ret = usbh_uvc_desc_find_frame_rate(stream, para, &frame_interval, &format_idx, &frame_idx);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Find fmt rate fail\n");
		return HAL_ERR_PARA;
	}

	ctrl = &stream->stream_ctrl;
	max_frame_size = ctrl->dwMaxVideoFrameSize;
	max_xfer_size = ctrl->dwMaxPayloadTransferSize;
	usb_os_memset(ctrl, 0U, sizeof(usbh_uvc_stream_control_t));
	ctrl->bmHint = 1U;  /* dwFrameInterval */
	ctrl->bFormatIndex = format_idx;
	ctrl->bFrameIndex = frame_idx;
	ctrl->dwFrameInterval = frame_interval;
	/* Restore values negotiated in previous PROBE round for cameras that validate these fields */
	ctrl->dwMaxVideoFrameSize = max_frame_size;
	ctrl->dwMaxPayloadTransferSize = max_xfer_size;

	uvc->state = UVC_STATE_CTRL;
	stream->state = STREAM_STATE_SET_PARA;
	if (uvc->host != NULL) {
		usbh_notify_class_state_change(uvc->host, 0U);
	}

	return HAL_OK;
}

/**
  * @brief	Get a frame from video streaming
  * @param	itf_num: Interface number
  * @retval Status
  */
usbh_uvc_frame_t *usbh_uvc_get_frame(u8 itf_num)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_frame_t *frame;
	usbh_uvc_stream_t *stream = NULL;
#if (USBH_UVC_USE_HW == 0)
	usb_os_sema_t frame_sema = NULL;
	usb_os_lock_t frame_mutex = NULL;
#else
	usbh_hw_uvc_dec_t *uvc_dec = NULL;
	usb_os_sema_t dec_sema = NULL;
	u32 frame_done_idx;
	u32 frame_done_size;
#endif

	if (itf_num >= USBH_UVC_VS_DESC_MAX_NUM) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Intf num ovfl\n");
		return NULL;
	}

	stream = &uvc->stream[itf_num];

	if (stream->get_valid == 0U) {
		stream->get_valid = 1U;
	}

	if (stream->stream_state != UVC_STREAM_ACTIVE) {
		goto exit;
	}

#if (USBH_UVC_USE_HW == 0)
	frame_sema = stream->frame_sema;
	frame_mutex = stream->frame_mutex;
	if ((frame_sema == NULL) || (frame_mutex == NULL)) {
		goto exit;
	}

	if (usb_os_sema_take(frame_sema, USBH_UVC_GET_FRAME_TIMEOUT) == HAL_OK) {

		usb_os_lock(frame_mutex);

		if (stream->stream_state != UVC_STREAM_ACTIVE) {
			usb_os_unlock(frame_mutex);
			goto exit;
		}

		if (list_empty(&stream->frame_chain)) {
			/*should not reach here*/
			usb_os_unlock(frame_mutex);
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "No frm in c\n");
			goto exit;
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

	uvc_dec = stream->uvc_dec;
	if (uvc_dec == NULL) {
		goto exit;
	}

	dec_sema = uvc_dec->dec_sema;
	if (dec_sema == NULL) {
		goto exit;
	}

	if (usb_os_sema_take(dec_sema, USBH_UVC_GET_FRAME_TIMEOUT) == HAL_OK) {
		if (stream->stream_state != UVC_STREAM_ACTIVE) {
			goto exit;
		}

		frame_done_idx = uvc_dec->frame_done_idx;
		if (frame_done_idx >= (u32)USBH_UVC_VIDEO_FRAME_NUMS) {
			goto exit;
		}

		frame_done_size = uvc_dec->frame_done_size;
		/* dec_sema take (xQueueSemaphoreTake) + give (xQueueGiveFromISR) share the
		 * same task_lock spinlock internally, providing a happens-before barrier
		 * that makes the ISR writes of frame_done_idx/size visible here. */

		frame = &stream->frame_buffer[frame_done_idx];
		frame->byteused = frame_done_size;
		/* The buffer was locked in the ISR at hand-off (buf_locked[frame_done_idx]).
		 * It stays locked until the app returns it via usbh_uvc_put_frame(), which
		 * guarantees the HW never overwrites the frame while the app reads it. */

		/* cache invalidate — HW decoder writes to PSRAM, CPU cache stale */
		DCache_Invalidate((u32)frame->buf, CACHE_LINE_ALIGNMENT(frame->byteused));
		return frame;
	} else {
		goto exit;
	}
#endif

exit:
	stream->get_valid = 0U;
	if (stream->stream_state != UVC_STREAM_ACTIVE) {
		stream->get_exit = 1U;
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
int usbh_uvc_put_frame(usbh_uvc_frame_t *frame, u8 itf_num)
{
#if (USBH_UVC_USE_HW == 0)
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = NULL;
#if USBH_UVC_DEBUG
	u32 hold_time;
	u32 put_frame_ts = usb_os_get_timestamp_ms();
	u32 get_frame_ts;
#endif

	if ((frame == NULL) || (itf_num >= USBH_UVC_VS_DESC_MAX_NUM)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Intf num ovfl\n");
		return HAL_ERR_PARA;
	}

	stream = &uvc->stream[itf_num];
#if USBH_UVC_DEBUG
	get_frame_ts = frame->get_frame_ts;

	if (put_frame_ts > get_frame_ts) {
		hold_time = put_frame_ts - get_frame_ts;
	} else {
		hold_time = (UINT32_MAX - get_frame_ts) + put_frame_ts + 1U;
	}

	if (hold_time > uvc->max_hold_frame_ms) {
		uvc->max_hold_frame_ms = hold_time;
		if (hold_time > USBH_UVC_SLOW_HOLD_MS) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Slow,h:%dms\n", hold_time);
		}
	}
#endif

	if (stream->stream_state == UVC_STREAM_ACTIVE) {
		usb_os_lock(stream->frame_mutex);

		list_del_init(&frame->list);
		frame->err = 0U;
		frame->byteused = 0U;
		frame->timestamp = 0U;
#if USBH_UVC_DEBUG
		frame->get_frame_ts = 0U;
#endif
		frame->state = UVC_FRAME_INIT;
		list_add_tail(&frame->list, &stream->frame_empty);

		usb_os_unlock(stream->frame_mutex);
	} else {
		frame->state = UVC_FRAME_INIT;
	}
	return HAL_OK;
#else
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream;
	u32 idx;

	if ((frame == NULL) || (itf_num >= USBH_UVC_VS_DESC_MAX_NUM)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Intf num ovfl\n");
		return HAL_ERR_PARA;
	}

	stream = &uvc->stream[itf_num];
	if (stream->uvc_dec != NULL) {
		/* Release the per-buffer lock set by the ISR at hand-off, so the HW may
		 * recycle this buffer again (R4 safe-drop ownership handshake). */
		idx = (u32)(frame - stream->frame_buffer);
		if (idx < (u32)USBH_HW_UVC_MAX_BUF_NUM) {
			frame->err = 0U;
			frame->byteused = 0U;
			frame->timestamp = 0U;
			usbh_hw_uvc_release_buf(stream->uvc_dec, idx);
		}
	}
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
	u32 i = 0;
	u32 j = 0;
	u32 fps = 0;
	u8 itf_num = 0;

	for (itf_num = 0; itf_num < USBH_UVC_VS_DESC_MAX_NUM; itf_num++) {
		stream = &uvc->stream[itf_num];
		vs = stream->vs_intf;
		if (vs == NULL) {
			continue;
		}
		if (vs->format == NULL) {
			continue;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "Intf %d:\n", itf_num);
		for (i = 0; i < vs->format_num; i++) {
			fmt = &vs->format[i];
			if (fmt->type == USBH_UVC_FORMAT_YUV) {
				fmt_name = "UNCOMPRESSED";
			} else if (fmt->type == USBH_UVC_FORMAT_MJPEG) {
				fmt_name = "MJPEG";
			} else if (fmt->type == USBH_UVC_FORMAT_H264) {
				fmt_name = "H264";
			} else if (fmt->type == USBH_UVC_FORMAT_H265) {
				fmt_name = "H265";
			} else {
				fmt_name = "UNKNOWN";
			}

			RTK_LOGS(TAG, RTK_LOG_INFO, "Format %d: %s (%d frame%s)\n",
					 i, fmt_name, fmt->frame_num,
					 (fmt->frame_num > 1) ? "s" : "");

			for (j = 0U; j < fmt->frame_num; j++) {
				frm = &fmt->frame[j];
				/* Avoid div-by-zero if a descriptor reports a 0 default interval. */
				fps = (frm->dwFrameInterval && frm->dwFrameInterval[0]) ?
					  (USBH_UVC_FRAME_INTERVAL_UNIT_NS / frm->dwFrameInterval[0]) : 0;
				RTK_LOGS(TAG, RTK_LOG_INFO, "  %d x %d @ %d fps\n", frm->wWidth, frm->wHeight, fps);
			}
		}
	}
}

/**
 * @brief    Stop video streaming and return to the ready state (resources retained)
 * @param    itf_num: Interface number
 * @retval HAL_OK on success or if the stream is not active,
 *         HAL_ERR_PARA on invalid interface number
 * @note     Data-flow-axis exit of the public lifecycle; pair with usbh_uvc_start().
 *           Delegates to the stream mechanic usbh_uvc_stream_stop().
 */
int usbh_uvc_stop(u8 itf_num)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = NULL;

	if (itf_num >= USBH_UVC_VS_DESC_MAX_NUM) {
		return HAL_ERR_PARA;
	}

	stream = &uvc->stream[itf_num];

	return usbh_uvc_stream_stop(stream);
}

/**
 * @brief    Start video streaming and kick off isochronous data transfer
 * @param    itf_num: Interface number
 * @retval HAL_OK on success or if the stream is already active,
 *         HAL_ERR_PARA on invalid interface number,
 *         HAL_ERR_HW if the stream is not in the ready state or the pipe is invalid
 * @note     Data-flow-axis entry of the public lifecycle; pair with usbh_uvc_stop().
 *           Delegates to the stream mechanic usbh_uvc_stream_start().
 */
int usbh_uvc_start(u8 itf_num)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = NULL;

	if (itf_num >= USBH_UVC_VS_DESC_MAX_NUM) {
		return HAL_ERR_PARA;
	}

	stream = &uvc->stream[itf_num];

	return usbh_uvc_stream_start(stream);
}

#if (USBH_UVC_USE_HW == 0) && USBH_UVC_DEBUG
/**
 * @brief  Clear class-layer frame and drop statistics via RTK_LOGS.
 *         Call from the application at the start of each measurement round.
 * @param[in] itf_num: Interface number.
 */
void usbh_uvc_clear_stats(u8 itf_num)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = NULL;
	if (itf_num >= USBH_UVC_VS_DESC_MAX_NUM) {
		return;
	}

	stream = &uvc->stream[itf_num];

	/* stream-level debug counters */
	stream->rx_frame_cnt = 1;//this app should be called after get frame access first frame
	stream->err_frame_cnt = 0;
	stream->drop_frame_cnt = 0;
	stream->dec_no_buf_cnt = 0;
	stream->foi_no_buf_cnt = 0;
	stream->eof_no_buf_cnt = 0;
	stream->next_no_buf_cnt = 0;

	/* stream-level (unconditional) */
	stream->reuse_cnt = 0;

	/* host-level timing peaks */
	uvc->max_combine_cost_us = 0;
	uvc->max_memcpy_cost_us = 0;
	uvc->max_publish_cost_us = 0;
	uvc->max_batch_drain_us = 0;
	uvc->max_batch_cnt = 0;
	uvc->max_hold_frame_ms = 0;
}

/**
 * @brief  Print class-layer frame and drop statistics via RTK_LOGS.
 *         Call from the application at the end of each measurement round.
 * @param[in] itf_num: Interface number.
 */
void usbh_uvc_print_stats(u8 itf_num)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream = NULL;
	if (itf_num >= USBH_UVC_VS_DESC_MAX_NUM) {
		return;
	}

	stream = &uvc->stream[itf_num];

	RTK_LOGS(TAG, RTK_LOG_INFO,
			 "class: rx=%d err=%d drop=%d reuse=%d\n",
			 stream->rx_frame_cnt, stream->err_frame_cnt,
			 stream->drop_frame_cnt, stream->reuse_cnt);
	RTK_LOGS(TAG, RTK_LOG_INFO,
			 "nobuf: dec=%d foi=%d eof=%d next=%d\n",
			 stream->dec_no_buf_cnt, stream->foi_no_buf_cnt,
			 stream->eof_no_buf_cnt, stream->next_no_buf_cnt);
	RTK_LOGS(TAG, RTK_LOG_INFO,
			 "peak: combine=%d mcpy=%d pub=%d us\n",
			 uvc->max_combine_cost_us, uvc->max_memcpy_cost_us,
			 uvc->max_publish_cost_us);
}
#endif
