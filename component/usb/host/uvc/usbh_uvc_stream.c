/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_uvc.h"
#include "usbh_uvc_stream.h"
#include "usbh_uvc_parse.h"

/* Private defines -----------------------------------------------------------*/
#define USBH_UVC_PAYLOAD_HEADER_MIN_LEN     (2U)   /**< Minimum valid UVC payload header length in bytes. */
#define USBH_UVC_DEBUG_LOOP_TIME_MS         (2000U)
#define USB_OTG_HFNUM_FRNUM_MAX             (0x3FFFUL)
#define USBH_UVC_URB_FLUSH_TIMEOUT_MS       (1U)   /* ms: flush residual URBs below watermark */
/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#if (USBH_UVC_USE_HW == 0)
static void usbh_uvc_combine_thread(void *param);
static void usbh_uvc_combine_thread_deinit(usbh_uvc_stream_t *stream);
static int usbh_uvc_combine_thread_init(usbh_uvc_stream_t *stream);
static usbh_uvc_frame_t *usbh_uvc_next_frame_buffer(usbh_uvc_stream_t *stream, usbh_uvc_frame_t *buf);
static void usbh_uvc_combine_urb(usbh_uvc_stream_t *stream, usbh_uvc_urb_t *urb);
static void usbh_uvc_isoc_in_process_xfer(usbh_uvc_stream_t *stream, u32 cur_frame);
#if USBH_UVC_DEBUG
static void usbh_uvc_dump_simple(usbh_uvc_stream_t *stream);
#endif
#endif
/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "UVC";

extern usbh_uvc_host_t uvc_host;
/* Private functions ---------------------------------------------------------*/

/**
  * @brief	Check uvc usb status
  * @param	None
  * @retval None
  */
static int usbh_uvc_usb_status_check(void)
{
	usbh_uvc_host_t *uvc = &uvc_host;

	if ((uvc->host != NULL) && (uvc->host->connect_state >= USBH_STATE_SETUP)) {
		return HAL_OK;
	}

	return HAL_BUSY;
}

static inline void usbh_uvc_reset_frame(usbh_uvc_frame_t *frame)
{
	if (frame != NULL) {
		frame->err = 0;
		frame->byteused = 0;
		frame->timestamp = 0;
	}
}

#if (USBH_UVC_USE_HW == 0)
#if USBH_UVC_DEBUG
/**
  * @brief  Dump UVC SW status.
  */
static void usbh_uvc_sw_status_dump(void)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	u32 head;
	u32 tail;
	u32 ring_used;
	u8 freecnt = 0U;
	u8 flycnt = 0U;
	u8 readycnt = 0U;
	u8 i;
	u8 own;

	if ((usbh_uvc_usb_status_check() == HAL_OK)) {
		if ((uvc != NULL) && (uvc->host != NULL)) {
			usbh_uvc_stream_t *stream = &uvc->stream[0];
			if (stream->stream_state == UVC_STREAM_ACTIVE) {
				for (i = 0U; i < USBH_UVC_URB_NUMS; i++) {
					own = stream->urb[i]->owner;
					if (own == 0U) {
						freecnt++;
					} else if (own == 1U) {
						flycnt++;
					} else {
						readycnt++;
					}
				}
				head = stream->urb_head;
				tail = stream->cur_urb;
				ring_used = (tail - head + USBH_UVC_URB_NUMS) % USBH_UVC_URB_NUMS;
				usbh_uvc_dump_simple(stream);
				RTK_LOGS(NOTAG, RTK_LOG_INFO,
						 "urb cost=%d own[%d/%d/%d] ring h=%d t=%d used=%d reuse=%d\n",
						 uvc->max_combine_cost_us, freecnt, flycnt, readycnt,
						 head, tail, ring_used, stream->reuse_cnt);
				RTK_LOGS(NOTAG, RTK_LOG_INFO,
						 "batch cnt=%d drain=%d us mcpy=%d pub=%d us hold=%d ms\n",
						 uvc->max_batch_cnt, uvc->max_batch_drain_us,
						 uvc->max_memcpy_cost_us, uvc->max_publish_cost_us,
						 uvc->max_hold_frame_ms);
				RTK_LOGS(NOTAG, RTK_LOG_INFO,
						 "frm rx=%d err=%d drop=%d nobuf[dec=%d foi=%d eof=%d next=%d]\n",
						 stream->rx_frame_cnt, stream->err_frame_cnt, stream->drop_frame_cnt,
						 stream->dec_no_buf_cnt, stream->foi_no_buf_cnt,
						 stream->eof_no_buf_cnt, stream->next_no_buf_cnt);
				RTK_LOGS(NOTAG, RTK_LOG_INFO,
						 "isoc rx[s=%d d=%d p=%d] empty=%d xfer=%d sof=%d\n",
						 uvc->isoc_rx_start_cnt, uvc->isoc_rx_done_cnt,
						 uvc->isoc_rx_process_cnt, uvc->isoc_in_empty_cnt,
						 uvc->isoc_xfer_interval_cnt, uvc->sof_cnt);

			}
		}
	}
}

/**
  * @brief  sw uvc status dump thread.
  * @param  param: Pointer to parameters.
  * @retval None.
  */
void usbh_uvc_sw_status_dump_thread(void *param)
{
	usbh_uvc_host_t *uvc = &uvc_host;

	UNUSED(param);

	uvc->sw_dump_task_alive = 1U;

	while (uvc->sw_dump_task_exit != 1U) { // Loop while NOT exit
		usbh_uvc_sw_status_dump();

		if (uvc->sw_dump_task_exit == 1U) {
			break;
		}
		rtos_time_delay_ms(USBH_UVC_DEBUG_LOOP_TIME_MS);
	}

	uvc->sw_dump_task_alive = 0U;
	rtos_task_delete(NULL);
}

static const char *usbh_uvc_frame_state_str(usbh_uvc_frame_state_t state)
{
	switch (state) {
	case UVC_FRAME_INIT:
		return "INIT";
	case UVC_FRAME_FLYING:
		return "FLYING";
	case UVC_FRAME_READY:
		return "READY";
	case UVC_FRAME_INUSE:
		return "USING";
	default:
		return "UNKNOWN";
	}
}

static u8 usbh_uvc_list_len(struct list_head *head)
{
	struct list_head *pos;
	u8 n = 0;

	list_for_each(pos, head) {
		n++;
	}
	return n;
}

static u8 usbh_uvc_in_list(struct list_head *node, struct list_head *head)
{
	struct list_head *pos;
	list_for_each(pos, head) {
		if (pos == node) {
			return 1;
		}
	}
	return 0;
}

static void usbh_uvc_dump_simple(usbh_uvc_stream_t *stream)
{
	usbh_uvc_frame_t *frame = NULL;
	u8 empty_len = usbh_uvc_list_len(&stream->frame_empty);
	u8 ready_len = usbh_uvc_list_len(&stream->frame_chain);
	u8 i;
	u8 in_empty;
	u8 in_ready;

	RTK_LOGS(TAG, RTK_LOG_INFO, "e=%d r=%d t=%d\n",
			 empty_len, ready_len, USBH_UVC_VIDEO_FRAME_NUMS);

	for (i = 0; i < USBH_UVC_VIDEO_FRAME_NUMS; i++) {
		frame = &stream->frame_buffer[i];

		in_empty = usbh_uvc_in_list(&frame->list, &stream->frame_empty);
		in_ready = usbh_uvc_in_list(&frame->list, &stream->frame_chain);

		RTK_LOGS(TAG, RTK_LOG_INFO,
				 "i=%d s=%s in_e=%d in_c=%d used=%u err=%u ts=%u\n",
				 i, usbh_uvc_frame_state_str(frame->state),
				 in_empty ? 1U : 0U,
				 in_ready ? 1U : 0U,
				 frame->byteused, frame->err, frame->timestamp);
	}
}
#endif// debug end
/**
  * @brief	Config uvc urb buffer
  * @param	stream: uvc stream interface
  * @retval None
  */
static int usbh_uvc_set_urb(usbh_uvc_stream_t *stream)
{
	u32 i;
	u32 j;
	u32 max_pkt_size;
	u32 pkt_cnt;
	u32 pkt_stride;
	u32 k;

	max_pkt_size = stream->cur_setting.pipe.xfer_len;
	if (max_pkt_size == 0U) {
		return HAL_ERR_PARA;
	}

	pkt_stride = CACHE_LINE_ALIGNMENT(max_pkt_size);
	pkt_cnt = USBH_UVC_URB_PACKET_NUM; /* single mps packet per URB slot */

	/*init urb*/
	stream->urb_buffer_size = pkt_cnt * pkt_stride;
	stream->urb_buffer = (u8 *)usb_os_malloc(USBH_UVC_URB_NUMS * stream->urb_buffer_size);
	if (stream->urb_buffer == NULL) {
		return HAL_ERR_MEM;
	}

	for (i = 0U; i < USBH_UVC_URB_NUMS; i ++) {
		stream->urb[i] = (usbh_uvc_urb_t *)usb_os_malloc(sizeof(usbh_uvc_urb_t));
		if (stream->urb[i] == NULL) {

			k = i;
			while (k > 0U) {
				k--;
				if (stream->urb[k] != NULL) {
					usb_os_mfree(stream->urb[k]);
					stream->urb[k] = NULL;
				}
			}

			if (stream->urb_buffer != NULL) {
				usb_os_mfree(stream->urb_buffer);
				stream->urb_buffer = NULL;
			}
			return HAL_ERR_MEM;
		}
		stream->urb[i]->addr = stream->urb_buffer + (i * stream->urb_buffer_size);
		stream->urb[i]->packet_num = pkt_cnt;
		stream->urb[i]->index = i;
		stream->urb[i]->packet_length = max_pkt_size;

		for (j = 0U; j < pkt_cnt; j ++) {
			stream->urb[i]->packet_info[j].length = max_pkt_size;
			stream->urb[i]->packet_info[j].offset = j * pkt_stride;
		}
	}

	stream->cur_urb = 0U;
	stream->urb_head = 0U;
	stream->cur_packet = 0U;

	return HAL_OK;
}

/**
  * @brief	Reset uvc urb buffer
  * @param	stream: uvc stream interface
  * @retval None
  */
static void usbh_uvc_reset_urb(usbh_uvc_stream_t *stream)
{
	u8 i;

	for (i = 0U; i < USBH_UVC_URB_NUMS; i++) {
		if (stream->urb[i] != NULL) {
			stream->urb[i]->addr = NULL;
			stream->urb[i]->packet_num = 0U;
			stream->urb[i]->index = 0U;
			stream->urb[i]->packet_length = 0U;
#if USBH_UVC_DEBUG
			stream->urb[i]->get_us = 0U;
			stream->urb[i]->submit_us = 0U;
			stream->urb[i]->process_us = 0U;
			stream->urb[i]->regive_us = 0U;
			stream->urb[i]->owner = 0U;
#endif
			usb_os_mfree(stream->urb[i]);
			stream->urb[i] = NULL;
		}
	}

	if (stream->urb_buffer != NULL) {
		usb_os_mfree(stream->urb_buffer);
		stream->urb_buffer = NULL;
		stream->urb_buffer_size = 0U;
	}
}


/**
  * @brief	Hand off a completed URB slot to the combine thread via the SPSC ring.
  *           Called from ISR context. Non-blocking: if the ring is full, the
  *           current slot is reused (newest packet dropped) and NULL is returned.
  * @param	stream: uvc stream interface
  * @param	urb: the just-completed URB (== stream->urb[cur_urb])
  * @retval Next URB slot to fill (urb[new tail]), or NULL to reuse current slot
  */
static usbh_uvc_urb_t *usbh_uvc_urb_complete(usbh_uvc_stream_t *stream, usbh_uvc_urb_t *urb)
{
	u8 new_tail;
	u8 head;
	u8 ready_count;
#if USBH_UVC_DEBUG == 0
	UNUSED(urb);
#endif
	if (usbh_uvc_usb_status_check() != HAL_OK) {
		return NULL;
	}

	if ((stream->is_resource_safe == 0U) || (stream->complete_flag == 0U)) {
		return NULL;
	}

	if (stream->urb_ready_sema == NULL) {
		return NULL;
	}

	/* SPSC full check: ring is full when advancing tail would collide with head */
	new_tail = (stream->cur_urb + 1U) % USBH_UVC_URB_NUMS;
	head = stream->urb_head; /* single consumer: safe plain read */

	if (new_tail == head) {
#if USBH_UVC_DEBUG
		/* Ring full — drop incoming packet, caller will reuse current slot */
		usbh_uvc_host_t *uvc = &uvc_host;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Rx ring full, cost=%d urb=%d pkt=%d reuse=%d\n",
				 uvc->max_combine_cost_us, stream->cur_urb, stream->cur_packet, stream->reuse_cnt);
#endif
		return NULL;
	}

#if USBH_UVC_DEBUG
	/* Commit: mark current slot data-ready, advance SPSC tail */
	urb->submit_us = usb_os_get_timestamp_us();
	urb->owner = 2U; /* data ready */
#endif
	stream->cur_urb = new_tail;

	/* ready_count = filled slots in ring after this commit */
	ready_count = (new_tail - head + USBH_UVC_URB_NUMS) % USBH_UVC_URB_NUMS;

	/* Wake combine thread only at watermark (amortise OS overhead) */
	if (ready_count >= USBH_UVC_URB_WATERMARK) {
		usb_os_sema_give(stream->urb_ready_sema);
	}

	/* Return the new in-flight slot to the ISR */
#if USBH_UVC_DEBUG
	stream->urb[new_tail]->get_us = usb_os_get_timestamp_us();
	stream->urb[new_tail]->owner = 1U; /* in-flight */
#endif
	return stream->urb[new_tail];
}

/**
  * @brief	UVC combine thread
  * @param	param: task parameter
  * @retval None
  */
static void usbh_uvc_combine_thread(void *param)
{
	usbh_uvc_stream_t *stream = (usbh_uvc_stream_t *)param;
	usbh_uvc_urb_t *urb_tmp = NULL;
#if USBH_UVC_DEBUG
	usbh_uvc_host_t *uvc = &uvc_host;
	u32 batch_us;
	u32 batch_cnt;
	u32 coms_ts;
	u32 cost;
	u32 batch_elapsed_us;
#endif

	stream->complete_on = 1U;
	while (stream->complete_on == 1U) {
		if (stream->is_resource_safe == 0U) {
			/* Paused during URB free/alloc (format switch). Keep the
			 * thread alive; only complete_on==0 tears it down. */
			usb_os_sleep_ms(2U);
			continue;
		}

		if (usbh_uvc_usb_status_check() != HAL_OK) {
			usb_os_sleep_ms(1U);
			continue;
		}

		/* Block until watermark hit or flush-timeout; both paths fall through to drain */
		rtos_sema_take(stream->urb_ready_sema, USBH_UVC_URB_FLUSH_TIMEOUT_MS);
		if (stream->complete_on == 0U) {
			/* real teardown */
			break;
		}
		if (stream->is_resource_safe == 0U) {
			/* paused mid-realloc, not teardown */
			continue;
		}
#if USBH_UVC_DEBUG
		batch_us = usb_os_get_timestamp_us();
		batch_cnt = 0U;
#endif

		/* Batch-drain: consume all ready URB slots in one shot */
		while ((stream->urb_head != stream->cur_urb) && (stream->is_resource_safe == 1U)) {
			urb_tmp = stream->urb[stream->urb_head];
#if USBH_UVC_DEBUG
			urb_tmp->process_us = usb_os_get_timestamp_us();
			coms_ts = usb_os_get_timestamp_us();
#endif
			usbh_uvc_combine_urb(stream, urb_tmp);

#if USBH_UVC_DEBUG
			cost = usb_os_get_timestamp_us() - coms_ts;
			if (cost > uvc->max_combine_cost_us) {
				uvc->max_combine_cost_us = cost;
			}
			batch_cnt++;

			urb_tmp->regive_us = usb_os_get_timestamp_us();
			urb_tmp->owner = 0U; /* free */
#endif

			stream->urb_head = (stream->urb_head + 1U) % USBH_UVC_URB_NUMS;
		}

#if USBH_UVC_DEBUG
		batch_elapsed_us = usb_os_get_timestamp_us() - batch_us;
		if (batch_elapsed_us > uvc->max_batch_drain_us) {
			uvc->max_batch_drain_us = batch_elapsed_us;
		}
		if (batch_cnt > uvc->max_batch_cnt) {
			uvc->max_batch_cnt = batch_cnt;
		}
#endif
	}

	stream->combine_task = NULL;
	rtos_task_delete(NULL);
}

/**
  * @brief	Deinit UVC combine thread
  * @param	stream: uvc stream interface
  * @retval None
  */
static void usbh_uvc_combine_thread_deinit(usbh_uvc_stream_t *stream)
{
	stream->complete_flag = 0U;
	stream->complete_on = 0U;
	stream->is_resource_safe = 0U;

	if (stream->urb_ready_sema != NULL) {
		usb_os_sema_give(stream->urb_ready_sema);
	}
}

/**
  * @brief	Free UVC combine thread resources
  * @param	stream: uvc stream interface
  * @retval None
  */
static void usbh_uvc_free_combine_resources(usbh_uvc_stream_t *stream)
{
	u8 retry_cnt = 100U;

	while ((retry_cnt != 0U) && (stream->combine_task != NULL)) {
		usb_os_sleep_ms(10U);
		retry_cnt--;
	}

	if (stream->urb_ready_sema != NULL) {
		usb_os_sema_delete(stream->urb_ready_sema);
		stream->urb_ready_sema = NULL;
	}
}

/**
  * @brief	Init UVC combine thread
  * @param	stream: uvc stream interface
  * @retval Status
  */
static int usbh_uvc_combine_thread_init(usbh_uvc_stream_t *stream)
{
	if (usb_os_sema_create(&stream->urb_ready_sema) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to create urb ready sema\n");
		return HAL_ERR_MEM;
	}
	if (rtos_task_create(&stream->combine_task, "usbh_uvc_combine_thread", usbh_uvc_combine_thread, (void *)stream, USBH_UVC_COMBINE_TASK_STACK,
						 USBH_UVC_COMBINE_TASK_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to create combine task\n");
		return HAL_ERR_MEM;
	}

	stream->complete_flag = 1U;
	return HAL_OK;
}

/**
  * @brief	Discard the oldest frame in the frame_chain queue to reclaim a free buffer
  * @param	stream: uvc stream interface
  * @retval true: Successfully dropped the oldest ready frame and moved it to the frame_empty queue;
  *         false: frame_chain queue is empty or reclaim failed; no frame was discarded
  */
static int usbh_uvc_discard_oldest_frame(usbh_uvc_stream_t *stream)
{
	usbh_uvc_frame_t *target = NULL;
	usbh_uvc_frame_t *n = NULL;
	int ret = HAL_OK;

	if (list_empty(&stream->frame_chain)) {
		// Chain is empty, nothing to discard.
		return HAL_ERR_HW;
	}

	/* Iterate through the frame_chain safely to find a victim */
	list_for_each_entry_safe(target, n, &stream->frame_chain, list, usbh_uvc_frame_t) {

		/* Critical Check: Skip frames that are currently transferring or held by user app */
		if ((target->state == UVC_FRAME_FLYING) || (target->state == UVC_FRAME_INUSE)) {
			continue;
		}

		/* Found 'target' is the oldest discardable frame. */
		/* Remove from ready chain */
		list_del_init(&target->list);
		usb_os_sema_take(stream->frame_sema, 0);
		/* Reset state and move to empty pool */
		usbh_uvc_reset_frame(target);
		target->state = UVC_FRAME_INIT;
		list_add_tail(&target->list, &stream->frame_empty);
#if USBH_UVC_DEBUG
		stream->drop_frame_cnt++;
#endif
		return ret;
	}

	return HAL_ERR_HW;
}

/**
  * @brief	Get next frame buffer to fill video data
  * @param	stream: uvc stream interface
  * @param  buf: the frame which has been filled
  * @retval Pointer of next frame buffer to be filled
  */
static usbh_uvc_frame_t *usbh_uvc_next_frame_buffer(usbh_uvc_stream_t *stream, usbh_uvc_frame_t *buf)
{
	usbh_uvc_frame_t *frame_buffer;
	u8 notify_app = 0U;

	if (buf == NULL) {
		return NULL;
	}

	usb_os_lock(stream->frame_mutex);

	// handle current frame
	if ((buf->err == 1U) || (buf->byteused == 0U)) {
		/* drop it, add this buf in frame_empty */
		buf->state = UVC_FRAME_INIT;
		usbh_uvc_reset_frame(buf);
		list_add_tail(&buf->list, &stream->frame_empty);
	} else {
		/* Valid frame: Submit to Ready Chain for App consumption */
		buf->state = UVC_FRAME_READY;
#if USBH_UVC_DEBUG
		buf->submit_frame_ts = usb_os_get_timestamp_us();
#endif
		list_add_tail(&buf->list, &stream->frame_chain);
		notify_app = 1U;
	}

	/**
	  * Optimization Strategy:
	  * Prefetch only new empty frames when it is indeed necessary to write the next packet of data;
	  * When empty, first attempt to discard the oldest READY frame for recycling. If the oldest READY
	  * frame is still in using, continue to try recycling the second oldest READY frame;
	  * If it remains unavailable, do not preempt; return NULL and wait to acquire it again when the
	  * next actual write operation is needed.
	  */
	if (list_empty(&stream->frame_empty)) {
		if (usbh_uvc_discard_oldest_frame(stream) != HAL_OK) {
			usb_os_unlock(stream->frame_mutex);
#if USBH_UVC_DEBUG
			stream->next_no_buf_cnt++;
#endif
			// even get empty fail ,also submit valid frame
			if (notify_app == 1U) {
				usb_os_sema_give(stream->frame_sema);
			}
			return NULL;
		}
	}

	/* avoid accessing empty list */
	if (list_empty(&stream->frame_empty)) {
		/* should not reach here */
		usb_os_unlock(stream->frame_mutex);
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Reclaim empty fail(next)\n");
		if (notify_app == 1U) {
			usb_os_sema_give(stream->frame_sema);
		}
		return NULL;
	}

	/* get a fresh buf from empty list */
	frame_buffer = list_first_entry(&stream->frame_empty, usbh_uvc_frame_t, list);
	list_del_init(&frame_buffer->list);

	usbh_uvc_reset_frame(frame_buffer);
	frame_buffer->state = UVC_FRAME_FLYING;

	usb_os_unlock(stream->frame_mutex);
#if USBH_UVC_DEBUG
	stream->rx_frame_cnt ++;
#endif
	/* don't give sema in lock*/
	if (notify_app == 1U) {
		usb_os_sema_give(stream->frame_sema);
	}

	return frame_buffer;
}

/**
  * @brief	COMBINE a given URB
  * @param  stream: uvc stream interface
  * @param  urb: URB to combine
  * @retval None
  */
static void usbh_uvc_combine_urb(usbh_uvc_stream_t *stream, usbh_uvc_urb_t *urb)
{
	u32 length;
	usbh_uvc_vs_payload_header_t *header;
	usbh_uvc_frame_t *frame_buffer = NULL;
	u8 *data;
	u32 bytes;
	u32 maxlen;
	u32 payload_len;
	u8 fid;
	u8 err = 0U;
	u8 header_len;
	u8 i;

	if (usbh_uvc_usb_status_check() != HAL_OK) {
		return;
	}

	if (stream->next_xfer == 0U) {
		return;
	}

	/* avoid continue getting frame_buffer, only first enter combine urb should reach here */
	frame_buffer = stream->cur_frame_buf;
	if (frame_buffer == NULL) {
		usb_os_lock(stream->frame_mutex);

		/* Check if empty list has available buffers */
		if (list_empty(&stream->frame_empty)) {
			/* CRITICAL FIX: Try to discard the oldest frame to to prevent the deadlock loop. */
			if (usbh_uvc_discard_oldest_frame(stream) != HAL_OK) {
				/* Still no buffer (all buffers are held by user or flying). We must drop this packet. */
				usb_os_unlock(stream->frame_mutex);
#if USBH_UVC_DEBUG
				stream->dec_no_buf_cnt++;
#endif
				return;
			}

			/* should not reach here */
			if (list_empty(&stream->frame_empty)) {
				usb_os_unlock(stream->frame_mutex);
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Rec err\n");
				return;
			}
		}

		/* Get a fresh buffer */
		frame_buffer = list_first_entry(&stream->frame_empty, usbh_uvc_frame_t, list);
		list_del_init(&frame_buffer->list);
		usbh_uvc_reset_frame(frame_buffer);

		usb_os_unlock(stream->frame_mutex);

		stream->cur_frame_buf = frame_buffer;
	}

	for (i = 0U; i < urb->packet_num; i ++) {

		data = urb->addr + urb->packet_info[i].offset;
		length = urb->packet_info[i].length;

		if (length == 0U) {
			continue;
		}

		header_len = data[0];
		header = (usbh_uvc_vs_payload_header_t *)(void *)data;

		if ((header_len < USBH_UVC_PAYLOAD_HEADER_MIN_LEN) || (length < header_len)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Err: payload len(%d) < header len(%d)\n", length, header_len);
			continue;
		}

		if (header->bmHeaderInfo.b.err == 1U) {
			/* Note: it need to send request if want to get error reason */
			err = 1U;
#if USBH_UVC_DEBUG
			stream->err_frame_cnt ++;
#endif
			stream->last_fid = header->bmHeaderInfo.b.fid; // Sync FID to avoid false toggle
			continue;
		}

		fid = header->bmHeaderInfo.b.fid;
		/* A: check FID */
		if (fid != stream->last_fid) {
			if (frame_buffer->byteused != 0U) {
				/* Previous frame had data, submit it now */
				frame_buffer->err = err ? 1U : 0U;
				frame_buffer = usbh_uvc_next_frame_buffer(stream, frame_buffer);
				if (frame_buffer == NULL) {
					stream->cur_frame_buf = NULL;
#if USBH_UVC_DEBUG
					stream->foi_no_buf_cnt ++;
#endif
					return;
				}
				/* a new frame，update info*/
				stream->cur_frame_buf = frame_buffer;
				stream->last_fid = fid;
				err = 0U;
			} else {
				/* no data but fid changes, first urb of first frame after stream on will reach here */
				stream->last_fid = fid;
				err = 0U;
			}
		}

		payload_len = length - header_len;
		if (payload_len > 0U) {
			/* For first payload of a frame arrival, record timestamp */
			/* Note: host monotonic tick; consider replacing with device PTS/SCR if available and realiable */
			if (frame_buffer->byteused == 0U) {
				frame_buffer->timestamp = usb_os_get_timestamp_ms();
			}

			maxlen = stream->frame_buffer_size - frame_buffer->byteused;
			bytes = MIN(maxlen, payload_len);

			usb_os_memcpy((frame_buffer->buf + frame_buffer->byteused), (data + header_len), bytes);
			frame_buffer->byteused += bytes;
		}

		/* B: check EOF */
		if (header->bmHeaderInfo.b.eof != 0U) {
			if (frame_buffer->byteused > 0U) {
				frame_buffer->err = err ? 1U : 0U;
				/* Submit current frame and prepare a fresh one for the next frame */
				frame_buffer = usbh_uvc_next_frame_buffer(stream, frame_buffer);
				if (frame_buffer == NULL) {
#if USBH_UVC_DEBUG
					stream->eof_no_buf_cnt ++;
#endif
					stream->cur_frame_buf = NULL;
					return;
				}

				/* a new frame，update info*/
				stream->cur_frame_buf = frame_buffer;
				/* don't change fid here, logic A will handle it */
				err = 0U;
			}
		}
	}

}

static inline u32 usbh_uvc_frame_num_dec(u32 cur_num, u32 start_num)
{
	if (cur_num >= start_num) {
		return cur_num - start_num;
	} else {
		return (USB_OTG_HFNUM_FRNUM_MAX - start_num + 1U + cur_num);
	}
}

/*
 * Increments frame by the amount specified by inc. The addition is done
 * modulo USB_OTG_HFNUM_FRNUM_MAX. Returns the incremented value.
 *
 * send the token in next frame
 */
static inline u32 usbh_uvc_frame_num_inc(u32 cur_num, u32 inc_num)
{
	return (cur_num + inc_num) & USB_OTG_HFNUM_FRNUM_MAX;
}

static void usbh_uvc_isoc_in_process_xfer(usbh_uvc_stream_t *stream, u32 cur_frame)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_setting_t *cur_setting;
	usbh_pipe_t *pipe;
	u8 urb_index;
	u8 packet_index;

	if (stream->next_xfer != 0U) {
#if USBH_UVC_DEBUG
		uvc->isoc_rx_start_cnt ++;
#endif
		cur_setting = &stream->cur_setting;
		pipe = &cur_setting->pipe;
		urb_index = stream->cur_urb;
		packet_index = stream->cur_packet;
		pipe->frame_num = usbh_uvc_frame_num_inc(cur_frame, 1U);
		pipe->xfer_buf = stream->urb[urb_index]->addr + stream->urb[urb_index]->packet_info[packet_index].offset;

		usbh_transfer_data(uvc->host, pipe);
		pipe->xfer_state = USBH_EP_XFER_BUSY;
	} else {
		// stop
#if USBH_UVC_DEBUG
		uvc->isoc_in_empty_cnt ++;
#endif
	}

}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Xfer complete callback
  * @param  host: Host handle
  * @param  pipe_num: pipe number
  * @retval Status
  */
int usbh_uvc_stream_process_completed(usb_host_t *host, u8 pipe_num)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream;
	usbh_pipe_t *pipe;
	usbh_uvc_urb_t *next_urb = NULL;
	u32 cur_frame = usbh_get_current_frame_number(host);
	u8 *urb_index;
	u8 *packet_index;
	u32 rx_len;
	u32 elapsed_num;
	u8 urb_state;
	u8 i;
	u8 j;

	if (uvc->state == UVC_STATE_TRANSFER) {
		for (i = 0U; i < uvc->uvc_desc.vs_num; i++) {
			stream = &uvc->stream[i];
			pipe = &(stream->cur_setting.pipe);

			if (pipe->pipe_num == pipe_num) {
				urb_index = &(stream->cur_urb);
				packet_index = &(stream->cur_packet);
				// drop remain data when dettach
				if ((stream->next_xfer == 0U) ||
					(stream->is_resource_safe == 0U) ||
					(stream->complete_flag == 0U)) {
					if ((*packet_index > 0U) && (stream->urb[*urb_index] != NULL)) {
						usbh_uvc_urb_t *cur_urb = stream->urb[*urb_index];
						for (j = 0U; j < *packet_index; j++) {
							cur_urb->packet_info[j].length = 0U;
						}
						*packet_index = 0U;
					}

					pipe->xfer_state = USBH_EP_XFER_IDLE;

					return HAL_OK;
				}

				urb_state = usbh_get_urb_state(host, pipe);
				if (urb_state == USBH_URB_DONE) {

#if (USBH_UVC_USE_HW == 0) && USBH_UVC_DEBUG
					uvc->isoc_rx_done_cnt ++;
#endif
					rx_len = usbh_get_last_transfer_size(host, pipe);

					/* some cameras send payload header without any valid payload packets, to reduce cpu loading just filter it */
					//if (rx_len > 12) {
#if (USBH_UVC_USE_HW == 0) && USBH_UVC_DEBUG
					uvc->isoc_rx_process_cnt ++;
#endif
					stream->urb[*urb_index]->packet_info[*packet_index].length = rx_len;

					/* don't submit cur urb without any valid payload, only 17% transfers with valid payload in total transfers */
					(*packet_index) ++;

					if (*packet_index >= stream->urb[*urb_index]->packet_num) {// a filled urb
						*packet_index = 0U;
#if (USBH_UVC_USE_HW == 0) && USBH_UVC_DEBUG
						stream->urb[*urb_index]->submit_us = usb_os_get_timestamp_us();
						stream->urb[*urb_index]->owner = 1U;
#endif
						next_urb = usbh_uvc_urb_complete(stream, stream->urb[*urb_index]);
						if (next_urb != NULL) {
							// update urb
							*urb_index = next_urb->index;
#if (USBH_UVC_USE_HW == 0) && USBH_UVC_DEBUG
							next_urb->get_us = usb_os_get_timestamp_us();
							next_urb->owner = 1U;// flying
#endif
						} else {
#if (USBH_UVC_USE_HW == 0) && USBH_UVC_DEBUG
							//record reuse cnt
							stream->reuse_cnt++;
#endif
						}// else: get free urb fail, reuse current urb

						if (usbh_uvc_usb_status_check() != HAL_OK) {
							pipe->xfer_state = USBH_EP_XFER_IDLE;
							return HAL_OK;
						}

						*packet_index = 0U;
					}

				} else {// urb stall/ err
					pipe->xfer_state = USBH_EP_XFER_IDLE;
				}

				if (stream->next_xfer != 0U) {
					elapsed_num = usbh_uvc_frame_num_dec(usbh_uvc_frame_num_inc(cur_frame, 1U), pipe->frame_num);
					if (elapsed_num >= pipe->ep_interval) {
#if (USBH_UVC_USE_HW == 0) && USBH_UVC_DEBUG
						uvc->isoc_com_start_rx ++;
#endif
						usbh_uvc_isoc_in_process_xfer(stream, cur_frame);
					} else {
						pipe->xfer_state = USBH_EP_XFER_WAIT_SOF;
					}
				} else {
					/* stop */
					pipe->xfer_state = USBH_EP_XFER_IDLE;
				}
				break;
			}
		}
	}

	return HAL_OK;
}

/**
  * @brief	Process sof interrupt
  * @param	stream: usb host instance
  * @retval Status
  */
void usbh_uvc_stream_process_sof(usb_host_t *host)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream;
	usbh_uvc_setting_t *cur_setting = NULL;
	u32 cur_frame = usbh_get_current_frame_number(host);
	usbh_pipe_t *pipe;
	u8 i;

#if USBH_UVC_DEBUG
	uvc->sof_cnt ++;
#endif

	if (uvc->state == UVC_STATE_TRANSFER) {
		for (i = 0U; i < uvc->uvc_desc.vs_num; i++) {
			stream = &uvc->stream[i];

			if (stream->next_xfer == 1U) {
				cur_setting = &stream->cur_setting;
				pipe = &cur_setting->pipe;

				if ((usbh_get_elapsed_frame_cnt(host, pipe->frame_num) >= pipe->ep_interval) ||
					((pipe->xfer_state == USBH_EP_XFER_WAIT_SOF) &&
					 (usbh_uvc_frame_num_dec(usbh_uvc_frame_num_inc(cur_frame, 1U), pipe->frame_num) >= pipe->ep_interval))) {
#if USBH_UVC_DEBUG
					uvc->isoc_sof_start_rx ++;
#endif
					usbh_uvc_isoc_in_process_xfer(stream, cur_frame);
				} else { // interval
#if USBH_UVC_DEBUG
					if (pipe->xfer_state == USBH_EP_XFER_IDLE) {
						uvc->isoc_xfer_interval_cnt ++;
					}
#endif
				}
			}

			if (stream->state != STREAM_STATE_CTRL_IDLE) {
				usbh_notify_class_state_change(host, 0x00);
			}
		}
	}
}

/**
  * @brief	free URB buffers (sized for old altsetting MPS).
  * @retval Void
  */
void usbh_uvc_stream_free_urb_buffer(usbh_uvc_stream_t *stream)
{
	/* stop ISOC submission + combine-thread data path */
	stream->is_resource_safe = 0U;
	usb_os_delay_us(2000U);        /* let in-flight ISOC + combine thread drain */

	usbh_uvc_reset_urb(stream);   /* free urb[i] + urb_buffer */

	stream->cur_urb = 0U;
	stream->urb_head = 0U;
	stream->cur_packet = 0U;
}

/**
  * @brief	alloc URB buffers sized to new pipe->xfer_len
  * @retval Status
  */
int usbh_uvc_stream_alloc_urb_buffer(usbh_uvc_stream_t *stream)
{
	int ret = usbh_uvc_set_urb(stream);   /* uses cur_setting.pipe.xfer_len (new MPS) */
	if (ret == HAL_OK) {
		stream->is_resource_safe = 1U;
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "URB alloc fail\n");
	}
	return ret;
}

#endif

#if (USBH_UVC_USE_HW == 1)
/**
  * @brief  UVC HW Error Recorder
  * @param  err: Error type
  */
static void usbh_uvc_err_record(u32 err)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_err_t *curr = &uvc->hw_stats;
	usbh_hw_uvc_dec_t *dec = NULL;
	u8 err_flag = 0U;
	u8 ch0_active = 0U;
	u8 ch1_active = 0U;
	u8 i;

	/* Mirror the SW path: only accumulate a channel's errors while its
	 * stream is actively transferring (stream_state == UVC_STREAM_ACTIVE).
	 * Errors observed outside the active window (flush / teardown / before
	 * EPAuto is enabled) are transient and must not pollute the
	 * customer-visible statistics. Header errors that occur while the stream
	 * is already ACTIVE (e.g. cold-boot camera sync) are genuine and kept. */
	for (i = 0U; i < uvc->uvc_desc.vs_num; i++) {
		dec = uvc->stream[i].uvc_dec;
		if ((dec != NULL) && (uvc->stream[i].stream_state == UVC_STREAM_ACTIVE)) {
			if (dec->ch == 0U) {
				ch0_active = 1U;
			} else if (dec->ch == 1U) {
				ch1_active = 1U;
			} else {
				/* no other channel */
			}
		}
	}

	if (ch0_active != 0U) {
		if (err & USBH_HW_UVC_CH0_BUF0_OVERSIZE) {
			curr->err_ch0_buf0_ov++;
			err_flag = 1U;
		}
		if (err & USBH_HW_UVC_CH0_BUF1_OVERSIZE) {
			curr->err_ch0_buf1_ov++;
			err_flag = 1U;
		}
		if (err & USBH_HW_UVC_CH0_BUF0_HEADER) {
			curr->err_ch0_buf0_h++;
			err_flag = 1U;
		}
		if (err & USBH_HW_UVC_CH0_BUF1_HEADER) {
			curr->err_ch0_buf1_h++;
			err_flag = 1U;
		}
	}

	if (ch1_active != 0U) {
		if (err & USBH_HW_UVC_CH1_BUF0_OVERSIZE) {
			curr->err_ch1_buf0_ov++;
			err_flag = 1U;
		}
		if (err & USBH_HW_UVC_CH1_BUF1_OVERSIZE) {
			curr->err_ch1_buf1_ov++;
			err_flag = 1U;
		}
		if (err & USBH_HW_UVC_CH1_BUF0_HEADER) {
			curr->err_ch1_buf0_h++;
			err_flag = 1U;
		}
		if (err & USBH_HW_UVC_CH1_BUF1_HEADER) {
			curr->err_ch1_buf1_h++;
			err_flag = 1U;
		}
	}

#if USBH_UVC_DEBUG
	if ((err_flag != 0U) && (uvc->hw_dump_sema != NULL) && (uvc->hw_dump_task_exit == 0U)) {
		usb_os_sema_give(uvc->hw_dump_sema);
	}
#else
	UNUSED(err_flag);
#endif
}

#if USBH_UVC_DEBUG
static void usbh_uvc_hw_status_dump(void)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_err_t *curr = NULL;
	usbh_uvc_err_t *last = NULL;
	usbh_hw_uvc_dec_t *dec = NULL;
	u8 i;

	curr = &uvc->hw_stats;
	last = &uvc->hw_stats_shadow;

	RTK_LOGS(TAG, RTK_LOG_WARN,
			 "CH0 ERR %d-%d-%d-%d, CH1 ERR %d-%d-%d-%d\n",
			 curr->err_ch0_buf0_ov, curr->err_ch0_buf1_ov,
			 curr->err_ch0_buf0_h,  curr->err_ch0_buf1_h,
			 curr->err_ch1_buf0_ov, curr->err_ch1_buf1_ov,
			 curr->err_ch1_buf0_h,  curr->err_ch1_buf1_h);
	*last = *curr;

	/* Per-stream frame stats so the customer can perceive dropped frames.
	 * Only the HW decoder's own struct is read here (one-way class->hw access);
	 * drops are counted by the ISR safe-drop path in usbh_hw_uvc.c (R4). */
	for (i = 0U; i < uvc->uvc_desc.vs_num; i++) {
		dec = uvc->stream[i].uvc_dec;
		if (dec != NULL) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "S%d frm rx=%d drop=%d\n",
					 i, dec->frame_rx_cnt, dec->frame_drop_cnt);
		}
	}
}

/**
  * @brief HW UVC status dump thread
  */
void usbh_uvc_hw_status_dump_thread(void *param)
{
	usbh_uvc_host_t *uvc = &uvc_host;

	UNUSED(param);

	uvc->hw_dump_task_alive = 1U;
	while (uvc->hw_dump_task_exit == 0U) {
		/* Wake on a HW error event (sema given) OR every USBH_UVC_DEBUG_LOOP_TIME_MS
		 * so per-stream frame/drop counters are dumped periodically even when no
		 * HW error occurs. */
		usb_os_sema_take(uvc->hw_dump_sema, USBH_UVC_DEBUG_LOOP_TIME_MS);
		if (uvc->hw_dump_task_exit != 0U) {
			break;
		}
		usbh_uvc_hw_status_dump();
	}

	if (uvc->hw_dump_sema != NULL) {
		usb_os_sema_delete(uvc->hw_dump_sema);
		uvc->hw_dump_sema = NULL;
	}

	uvc->hw_dump_task_alive = 0U;
	rtos_task_delete(NULL);
}
#endif
#endif /* USBH_UVC_USE_HW == 1 */

/**
  * @brief	Send SET_CUR request
  * @param	stream: uvc stream interface
  * @param	probe: indicate probe or commit
  * @retval Status
  */
int usbh_uvc_stream_ctrl_set_video(usbh_uvc_stream_t *stream, u8 probe)
{
	usbh_setup_req_t setup;
	usbh_uvc_host_t *uvc = &uvc_host;
	usb_host_t *host = uvc->host;
	usbh_uvc_stream_control_t *ctrl = &stream->stream_ctrl;
	u32 ctrl_struct_size;
	u16 size;

	if (uvc->uvc_desc.vc_intf.vcheader == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "VC header not parsed\n");
		return HAL_ERR_PARA;
	}
	size = usbh_uvc_get_ctrl_len_by_version(uvc->uvc_desc.vc_intf.vcheader->bcdUVC);
	ctrl_struct_size = sizeof(usbh_uvc_stream_control_t);

	/* minimal UVC 1.5 compat: zero buf so offset 34-47 are 0, then copy known fields */
	usb_os_memset(uvc->request_buf, 0U, size);
	usb_os_memcpy(uvc->request_buf, (void *) ctrl, (size < ctrl_struct_size) ? size : ctrl_struct_size);

	if (USB_IS_MEM_DMA_ALIGNED(uvc->request_buf)) {
		DCache_Clean((u32)uvc->request_buf, size);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Buf align err %x\n", (u32)uvc->request_buf);
		return HAL_ERR_MEM;
	}

	setup.req.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE | \
							  USB_REQ_TYPE_CLASS;
	setup.req.bRequest = USBH_UVC_SET_CUR;

	if (probe != 0U) {
		setup.req.wValue = USBH_UVC_VS_PROBE_CONTROL;
	} else {
		setup.req.wValue = USBH_UVC_VS_COMMIT_CONTROL;
	}

	setup.req.wLength = size;
	setup.req.wIndex = stream->cur_setting.bInterfaceNumber;

	return usbh_ctrl_request(host, &setup, (void *)uvc->request_buf);
}

/**
  * @brief	Send GET request, such as GET_CUR, GET_DEF
  * @param	stream: uvc stream interface
  * @param	probe: indicate probe or commit
  * @param	request: indicate which request
  * @retval Status
  */
int usbh_uvc_stream_ctrl_get_video(usbh_uvc_stream_t *stream, u8 probe, u16 request)
{
	usbh_setup_req_t setup;
	usbh_uvc_host_t *uvc = &uvc_host;
	usb_host_t *host = uvc->host;
	u16 size;

	if (uvc->uvc_desc.vc_intf.vcheader == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "VC header not parsed\n");
		return HAL_ERR_PARA;
	}
	size = usbh_uvc_get_ctrl_len_by_version(uvc->uvc_desc.vc_intf.vcheader->bcdUVC);

	setup.req.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | \
							  USB_REQ_TYPE_CLASS;
	setup.req.bRequest = request;

	if (probe != 0U) {
		setup.req.wValue = USBH_UVC_VS_PROBE_CONTROL;
	} else {
		setup.req.wValue = USBH_UVC_VS_COMMIT_CONTROL;
	}

	setup.req.wLength = size;
	setup.req.wIndex = stream->cur_setting.bInterfaceNumber;

	return usbh_ctrl_request(host, &setup, uvc->request_buf);
}

/**
  * @brief	Apply the negotiated stream control: resolve the committed
  *         bFormatIndex/bFrameIndex into the concrete format/frame descriptor
  *         pointers and bind them to the stream (def_format/cur_format/cur_frame).
  * @param	stream: uvc stream interface
  * @retval Status
  */
int usbh_uvc_stream_ctrl_apply(usbh_uvc_stream_t *stream)
{
	usbh_uvc_stream_control_t *ctrl = &stream->stream_ctrl;
	usbh_uvc_vs_t *vs = stream->vs_intf;
	usbh_uvc_vs_format_t *format = NULL;
	usbh_uvc_vs_frame_t *frame = NULL;
	u8 format_num;
	u8 frame_num;
	u8 i;
	u8 found_format = 0;
	u8 found_frame = 0;

	if (vs == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "vs_intf NULL\n");
		return HAL_ERR_PARA;
	}
	if (vs->format == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "vs_fmt NULL\n");
		return HAL_ERR_PARA;
	}
	format_num = vs->format_num;

	/*select matched format */
	for (i = 0; i < format_num; i ++) {
		format = &vs->format[i];
		if (format->index == ctrl->bFormatIndex) {
			found_format = 1U;
			break;
		}
	}

	if (found_format == 1U) {
		if (format->frame == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "fmt_frame NULL\n");
			return HAL_ERR_PARA;
		}
		/*select matched frame */
		frame_num = format->frame_num;
		for (i = 0U; i < frame_num; i ++) {

			frame = &format->frame[i];
			if (frame->bFrameIndex == ctrl->bFrameIndex) {
				found_frame = 1;
				break;
			}
		}

		stream->def_format = format;
		stream->cur_format = format;

		if (found_frame == 1U) {
			stream->cur_frame = frame;
		}
	}

	return HAL_OK;
}

/**
  * @brief  Allocate and initialize all runtime resources of a UVC stream.
  *         Creates the frame buffers, frame lists, semaphores/mutexes and the
  *         worker threads for one stream instance. This is the resource-layer
  *         (stream mechanic) back-end of the public usbh_uvc_open(); on any step
  *         failure it rolls back whatever was already allocated.
  * @param	stream: uvc stream interface
  * @retval HAL_OK on success, non-zero on failure
  */
int usbh_uvc_stream_open(usbh_uvc_stream_t *stream)
{
	usbh_uvc_frame_t *frame = NULL;
	u32 frame_buf_size = CACHE_LINE_ALIGNMENT(stream->frame_buffer_size);
	u8 i;
#if (USBH_UVC_USE_HW == 0)
	int status;
#endif
#if (USBH_UVC_USE_HW == 1)
	usbh_uvc_host_t *uvc;
	usbh_hw_uvc_dec_t *uvc_dec;
	u8 j;
#endif

#if (USBH_UVC_USE_HW == 0)
	INIT_LIST_HEAD(&stream->frame_chain);
	INIT_LIST_HEAD(&stream->frame_empty);

	rtos_sema_create(&stream->frame_sema, 0U, USBH_UVC_VIDEO_FRAME_NUMS);
	usb_os_lock_create(&stream->frame_mutex);
#endif

	/*init frame buffer*/
	stream->frame_buf = (u8 *)usb_os_malloc(USBH_UVC_VIDEO_FRAME_NUMS * frame_buf_size);
	if (stream->frame_buf == NULL) {
		goto exit;
	}

	for (i = 0; i < USBH_UVC_VIDEO_FRAME_NUMS; i++) {
		frame = &stream->frame_buffer[i];
		frame->buf = stream->frame_buf + (i * frame_buf_size);
		frame->state = UVC_FRAME_INIT;
		usbh_uvc_reset_frame(frame);
#if (USBH_UVC_USE_HW == 0)
		INIT_LIST_HEAD(&frame->list);
		list_add_tail(&frame->list, &stream->frame_empty);
#endif
	}
#if (USBH_UVC_USE_HW == 0)
	stream->cur_frame_buf = NULL;
	stream->last_fid = 0xFFU;

	/* init combine thread */
	status = usbh_uvc_combine_thread_init(stream);
	if (status != HAL_OK) {
		goto exit;
	}
#endif

#if (USBH_UVC_USE_HW == 1)
	uvc = &uvc_host;

	_Static_assert(USBH_UVC_VIDEO_FRAME_NUMS == USBH_HW_UVC_MAX_BUF_NUM,
				   "USBH_UVC_VIDEO_FRAME_NUMS must be 3");

	stream->uvc_dec = usbh_hw_uvc_alloc_channel();
	if (stream->uvc_dec == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc ch err\n");
		goto exit;
	}

	uvc_dec = stream->uvc_dec;
	uvc_dec->dev_addr = uvc->host->dev_addr;
	for (j = 0; j < USBH_UVC_VIDEO_FRAME_NUMS; j ++) {
		uvc_dec->buf[j].buf_start_addr = (u32)stream->frame_buffer[j].buf;
		uvc_dec->buf[j].buf_size = frame_buf_size;
	}

	uvc->hw_error = usbh_uvc_err_record;
	stream->uvc_dec->err_cb = uvc->hw_error;
	usbh_hw_uvc_init(stream->uvc_dec);	/* create dec_sema once; prepare+start deferred to stream_start */

	rtos_critical_enter(RTOS_CRITICAL_USB);
	if (uvc->hw_irq_ref_cnt == 0U) {
		usbh_hw_uvc_irq_en(uvc->hw_isr_pri);
	}
	uvc->hw_irq_ref_cnt++;
	rtos_critical_exit(RTOS_CRITICAL_USB);

#endif

	stream->stream_state = UVC_STREAM_READY;
	return HAL_OK;

exit:
	/* Set state so usbh_uvc_stream_close performs full cleanup; it skips cleanup
	 * when stream_state is UVC_STREAM_OFF (the initial state during init failure). */
	stream->stream_state = UVC_STREAM_READY;
	usbh_uvc_stream_close(stream);
	return HAL_ERR_MEM;
}

/**
  * @brief  Wait for get-frame thread to exit safely
  * @param  stream: uvc stream interface
  * @retval None
  */
static void usbh_uvc_exit_get_frame(usbh_uvc_stream_t *stream)
{
	/* only in dettach */
	if (usbh_uvc_usb_status_check() != HAL_OK) {
		while ((stream->get_valid != 0U) && (stream->get_exit == 0U)) {
			rtos_time_delay_ms(1);
		}
		stream->get_valid = 0;
		stream->get_exit  = 0;
	}
}

/**
  * @brief  Release all runtime resources of a UVC stream.
  *         Stops the worker threads, frees the frame buffers/frame lists and
  *         destroys the semaphores/mutexes allocated by usbh_uvc_stream_open().
  *         Resource-layer (stream mechanic) back-end of the public usbh_uvc_close().
  * @param	stream: uvc stream interface
  * @retval None
  */
void usbh_uvc_stream_close(usbh_uvc_stream_t *stream)
{
	usbh_uvc_frame_t *frame = NULL;
#if (USBH_UVC_USE_HW == 1)
	usbh_uvc_host_t *uvc;
	usbh_hw_uvc_dec_t *dec;
#endif
	u8 i = 0;

	if (stream->stream_state == UVC_STREAM_OFF) {
		return;
	}
#if (USBH_UVC_USE_HW == 0)
	stream->next_xfer = 0U;
#endif
	stream->stream_state = UVC_STREAM_OFF;

#if (USBH_UVC_USE_HW == 1)
	uvc = &uvc_host;
	dec = stream->uvc_dec;
	if (dec != NULL) {
		rtos_critical_enter(RTOS_CRITICAL_USB);
		if (uvc->hw_irq_ref_cnt > 0U) {
			uvc->hw_irq_ref_cnt--;
			if (uvc->hw_irq_ref_cnt == 0U) {
				usbh_hw_uvc_irq_dis();
			}
		}
		rtos_critical_exit(RTOS_CRITICAL_USB);

		usbh_hw_uvc_stop(dec);
		usbh_hw_uvc_deinit(dec);

		if (dec->dec_sema != NULL) {
			usb_os_sema_give(dec->dec_sema);
		}
		usbh_uvc_exit_get_frame(stream);

		usbh_hw_uvc_free_channel(dec);
		stream->uvc_dec = NULL;
	}
#else

	if (stream->frame_sema != NULL) {
		usb_os_sema_give(stream->frame_sema);
	}
	usbh_uvc_exit_get_frame(stream);

	/* delete combine task */
	usbh_uvc_combine_thread_deinit(stream);
	usb_os_delay_us(2000);

	usbh_uvc_free_combine_resources(stream);

	if (stream->frame_sema != NULL) {
		usb_os_sema_delete(stream->frame_sema);
		stream->frame_sema = NULL;
	}

	if (stream->frame_mutex != NULL) {
		usb_os_lock_delete(stream->frame_mutex);
		stream->frame_mutex = NULL;
	}

	/* reset urb buffer */
	usbh_uvc_reset_urb(stream);
	INIT_LIST_HEAD(&stream->frame_chain);
	INIT_LIST_HEAD(&stream->frame_empty);

	stream->cur_urb = 0U;
	stream->last_fid = 0x0U;
	stream->cur_frame_buf = NULL;
	stream->urb_buffer_size = 0U;
#endif

	for (i = 0; i < USBH_UVC_VIDEO_FRAME_NUMS; i++) {
		frame = &stream->frame_buffer[i];
		INIT_LIST_HEAD(&frame->list);
		usbh_uvc_reset_frame(frame);
		frame->buf = NULL;
		frame->state = UVC_FRAME_INIT;
	}

	if (stream->frame_buf != NULL) {
		usb_os_mfree(stream->frame_buf);
		stream->frame_buf = NULL;
	}
}

/**
  * @brief  Synchronously flush all per-round runtime state.
  *         Pauses the data path, drains semaphores, resets frame lists and URB ring
  *         cursors. Called from stream_start before every round to ensure a clean
  *         start regardless of residual state from the previous round.
  * @param  stream: uvc stream interface
  * @retval None
  */
void usbh_uvc_stream_flush(usbh_uvc_stream_t *stream)
{
#if (USBH_UVC_USE_HW == 0)
	u8 i;

	stream->is_resource_safe = 0;
	usb_os_delay_us(2000); /* let in-flight ISR callbacks and combine_urb drain */

	/* Drain stale notification tokens */
	while (usb_os_sema_take(stream->frame_sema, 0) == HAL_OK) {
		;
	}
	while (usb_os_sema_take(stream->urb_ready_sema, 0) == HAL_OK) {
		;
	}

	/* Reset frame lists*/
	INIT_LIST_HEAD(&stream->frame_chain);
	INIT_LIST_HEAD(&stream->frame_empty);
	for (i = 0; i < USBH_UVC_VIDEO_FRAME_NUMS; i++) {
		usbh_uvc_frame_t *frame = &stream->frame_buffer[i];
		frame->state = UVC_FRAME_INIT;
		usbh_uvc_reset_frame(frame);
		INIT_LIST_HEAD(&frame->list);
		list_add_tail(&frame->list, &stream->frame_empty);
	}
	stream->cur_frame_buf = NULL;

	/* Reset URB ring cursors and decode state */
	stream->urb_head = 0;
	stream->cur_urb = 0;
	stream->cur_packet = 0;
	stream->last_fid = 0xFFU;

	stream->is_resource_safe = 1;
#else
	/* HW path: per-round state reset only. free_buf_cnt must be restored to its
	 * initial value (2) so the 3-buffer rotation in usbh_hw_uvc_handle_frame_done()
	 * starts from a known position, and stale dec_sema tokens are drained. */
	if (stream->uvc_dec != NULL) {
		/* usbh_hw_uvc_flush() discards any HW frame-done/error status latched in
		 * the previous round and resets the buffer rotation (analogous to the SW
		 * branch resetting decode cursors); then drain stale dec_sema tokens. */
		usbh_hw_uvc_flush(stream->uvc_dec);
		while ((stream->uvc_dec->dec_sema != NULL) &&
			   usb_os_sema_take(stream->uvc_dec->dec_sema, 0) == HAL_OK) {
			;
		}
	}
#endif
}

/**
  * @brief  Start the data path of a UVC stream (READY -> ACTIVE).
  *         Flushes residual per-round state, validates the streaming pipe, kicks
  *         off the isochronous transfer and notifies the class state machine.
  *         Data-flow (stream mechanic) back-end of the public usbh_uvc_start().
  * @param	stream: uvc stream interface
  * @retval HAL_OK on success or if the stream is already active,
  *         HAL_ERR_HW if the stream is not in the ready state or the pipe is invalid
  */
int usbh_uvc_stream_start(usbh_uvc_stream_t *stream)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_setting_t *cur_setting = NULL;
	usbh_pipe_t *pipe = NULL;

	if (stream->stream_state == UVC_STREAM_ACTIVE) {
		return HAL_OK;
	}

	if (stream->stream_state != UVC_STREAM_READY) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "S%d start in wrong state(%d)\n", stream->stream_idx, stream->stream_state);
		return HAL_ERR_HW;
	}

	usbh_uvc_stream_flush(stream);

	cur_setting = &stream->cur_setting;
	pipe = &cur_setting->pipe;
	if (pipe->ep_mps == 0U) {
		return HAL_ERR_HW;
	}

	stream->stream_state = UVC_STREAM_ACTIVE;
#if (USBH_UVC_USE_HW == 0)
	stream->next_xfer = 1U;
#else
	usbh_hw_uvc_prepare(stream->uvc_dec, pipe);
	usbh_hw_uvc_start(stream->uvc_dec);
#endif

	usbh_notify_class_state_change(uvc->host, pipe->pipe_num);
	return HAL_OK;
}

/**
  * @brief  Stop the data path of a UVC stream (ACTIVE -> READY).
  *         Halts the isochronous transfer but keeps all resources allocated, so
  *         the stream can be re-started without re-opening.
  *         Data-flow (stream mechanic) back-end of the public usbh_uvc_stop().
  * @param	stream: uvc stream interface
  * @retval HAL_OK on success or if the stream is not currently active
  */
int usbh_uvc_stream_stop(usbh_uvc_stream_t *stream)
{
	if (stream->stream_state != UVC_STREAM_ACTIVE) {
		return HAL_OK;
	}
#if (USBH_UVC_USE_HW == 0)
	stream->next_xfer = 0U;
#else
	usbh_hw_uvc_stop(stream->uvc_dec);
#endif
	stream->stream_state = UVC_STREAM_READY;

	return HAL_OK;
}