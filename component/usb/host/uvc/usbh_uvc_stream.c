/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_uvc.h"

/* Private defines -----------------------------------------------------------*/
#define USBH_UVC_DEBUG                      0
#define USB_OTG_HFNUM_FRNUM_MAX             (0x3FFFUL)
/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#if (USBH_UVC_USE_HW == 0)
static void usbh_uvc_decode_thread(void *param);
static void usbh_uvc_decode_thread_deinit(usbh_uvc_stream_t *stream);
static int usbh_uvc_decode_thread_init(usbh_uvc_stream_t *stream);
static usbh_uvc_frame_t *usbh_uvc_next_frame_buffer(usbh_uvc_stream_t *stream, usbh_uvc_frame_t *buf);
static void usbh_uvc_decode_urb(usbh_uvc_stream_t *stream, usbh_uvc_urb_t *urb);
#endif
/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "UVC";

extern usbh_uvc_host_t uvc_host;
#if (USBH_UVC_USE_HW == 0)
static u32 wait_recv_timeout = RTOS_MAX_DELAY;
static u32 giveback_send_timeout = RTOS_MAX_DELAY;
#endif
static u32 wait_send_timeout = RTOS_MAX_DELAY;
static u32 giveback_recv_timeout = RTOS_MAX_DELAY;
/* Private functions ---------------------------------------------------------*/

/**
  * @brief	Compare frame number
  * @param	frame1: frame number 1 to compare
  *         frame2: frame number 2 to compare
  * @retval Compare result
  */
#if ((USBH_UVC_USE_SOF == 1) && (USBH_UVC_USE_HW == 0))
static int usbh_uvc_frame_num_le(u32 frame1, u32 frame2)
{
	return ((frame2 - frame1) & USB_OTG_HFNUM_FRNUM_MAX) <= (USB_OTG_HFNUM_FRNUM_MAX >> 1);
}
#endif

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

/**
  * @brief	Config uvc urb buffer
  * @param	stream: uvc stream interface
  * @retval None
  */
static int usbh_uvc_set_urb(usbh_uvc_stream_t *stream)
{
	int i, j;
	u32 max_pkt_size, pkt_cnt, pkt_stride;

	max_pkt_size = stream->cur_setting.pipe.xfer_len;
	pkt_stride = CACHE_LINE_ALIGNMENT(max_pkt_size);
	pkt_cnt = USBH_UVC_URB_SIZE / max_pkt_size;

	//RTK_LOGS(TAG, RTK_LOG_DEBUG, "MPS:%d, pkt_cnt:%d\n", max_pkt_size, pkt_cnt);

	/*init urb*/
	stream->urb_buffer_size = pkt_cnt * pkt_stride;
	stream->urb_buffer = (u8 *)usb_os_malloc(USBH_UVC_URB_NUMS * stream->urb_buffer_size);
	if (stream->urb_buffer == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc urb buf fail\n");
		return HAL_ERR_MEM;
	}

	for (i = 0; i < USBH_UVC_URB_NUMS; i ++) {
		stream->urb[i] = (usbh_uvc_urb_t *)usb_os_malloc(sizeof(usbh_uvc_urb_t) + (pkt_cnt + 1) * sizeof(usbh_uvc_packet_desc_t));
		if (stream->urb[i] == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc urb bufi fail\n");
			return HAL_ERR_MEM;
		}
		stream->urb[i]->addr = (u8 *)((u32)stream->urb_buffer + i * stream->urb_buffer_size);
		stream->urb[i]->packet_num = pkt_cnt;
		stream->urb[i]->index = i;
		stream->urb[i]->packet_length = max_pkt_size;

		for (j = 0; j < (int)pkt_cnt; j ++) {
			stream->urb[i]->packet_info[j].length = max_pkt_size;
			stream->urb[i]->packet_info[j].offset = j * pkt_stride;
		}
	}

	stream->cur_urb = 0;
	stream->cur_packet = 0;
	stream->cur_packet_state = 0;

	return HAL_OK;
}

#if (USBH_UVC_USE_HW == 0)
/**
  * @brief	Fill uvc decode giveback queue
  * @param	stream: uvc stream interface
  * @retval None
  */
static int usbh_uvc_fill_giveback_queue(usbh_uvc_stream_t *stream)
{
	int i = 0;
	for (i = 1; i < USBH_UVC_URB_NUMS; i ++) {
		if (usb_os_queue_send(stream->urb_giveback_queue, (void *)&stream->urb[i], RTOS_MAX_TIMEOUT) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Push to giveback Q %d fail\n", i);
			return HAL_ERR_UNKNOWN;
		}
	}
	return HAL_OK;
}
#endif

/**
  * @brief	Reset uvc urb buffer
  * @param	stream: uvc stream interface
  * @retval None
  */
static void usbh_uvc_reset_urb(usbh_uvc_stream_t *stream)
{
	int i;

	for (i = 0; i < USBH_UVC_URB_NUMS; i++) {
		if (stream->urb[i] != NULL) {
			stream->urb[i]->addr = NULL;
			stream->urb[i]->packet_num = 0;
			stream->urb[i]->index = 0;
			stream->urb[i]->packet_length = 0;
			usb_os_mfree(stream->urb[i]);
			stream->urb[i] = NULL;
		}
	}

	if (stream->urb_buffer != NULL) {
		usb_os_mfree(stream->urb_buffer);
		stream->urb_buffer = NULL;
	}
}

static inline void usbh_uvc_reset_frame(usbh_uvc_frame_t *frame)
{
	if (frame) {
		frame->err = 0;
		frame->byteused = 0;
		frame->timestamp = 0;
	}
}

#if (USBH_UVC_USE_HW == 0)

#if USBH_UVC_DEBUG
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

static int usbh_uvc_list_len(struct list_head *head)
{
	int n = 0;
	struct list_head *pos;
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

static void usbh_uvc_dump_simple(usbh_uvc_stream_t *stream, const char *tag)
{
	int empty_len = usbh_uvc_list_len(&stream->frame_empty);
	int ready_len = usbh_uvc_list_len(&stream->frame_chain);

	RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] empty=%d ready=%d total=%d\n",
			 tag, empty_len, ready_len, USBH_UVC_VIDEO_FRAME_NUMS);

	for (int i = 0; i < USBH_UVC_VIDEO_FRAME_NUMS; i++) {
		usbh_uvc_frame_t *frame = &stream->frame_buffer[i];

		u8 in_empty = usbh_uvc_in_list(&frame->list, &stream->frame_empty);
		u8 in_ready = usbh_uvc_in_list(&frame->list, &stream->frame_chain);

		RTK_LOGS(TAG, RTK_LOG_INFO,
				 "[%s] idx=%d state=%s in_empty=%d in_chain=%d byteused=%u err=%u ts=%u\n",
				 tag, i, usbh_uvc_frame_state_str(frame->state),
				 in_empty ? 1 : 0,
				 in_ready ? 1 : 0,
				 frame->byteused, frame->err, frame->timestamp);
	}
}
#endif

/**
  * @brief	UVC decode thread
  * @param	param: task parameter
  * @retval None
  */
static void usbh_uvc_decode_thread(void *param)
{
	usbh_uvc_urb_t *urb_tmp;
	usbh_uvc_stream_t *stream = (usbh_uvc_stream_t *)param;

	stream->complete_on = 1;
	while (stream->complete_on) {
		if (stream->is_resource_safe == 0) {
			break;
		}

		if (usbh_uvc_usb_status_check() != HAL_OK) {
			usb_os_sleep_ms(10);
			continue;
		}

		if (usb_os_queue_receive(stream->urb_wait_queue, (void *)&urb_tmp, wait_recv_timeout) != HAL_OK) {
			continue;
		}

		if (stream->is_resource_safe == 0) {
			break;
		}

		usbh_uvc_decode_urb(stream, urb_tmp);

		while (stream->is_resource_safe && usb_os_queue_send(stream->urb_giveback_queue, (void *)&urb_tmp, giveback_send_timeout) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to push urb back\n");
			if (stream->is_resource_safe == 0) {
				break;
			}
			usb_os_sleep_ms(1);
		}
	}

	rtos_task_delete(NULL);
}

/**
  * @brief	Deinit UVC decode thread
  * @param	stream: uvc stream interface
  * @retval None
  */
static void usbh_uvc_decode_thread_deinit(usbh_uvc_stream_t *stream)
{
	usbh_uvc_urb_t *urb_tmp;

	if (stream->decode_task != NULL) {
		rtos_task_delete(stream->decode_task);
		stream->decode_task = NULL;
	}

	stream->complete_flag = 0;
	stream->complete_on = 0;

	if (stream->urb_wait_queue != NULL) {
		/*
		 * Drain the queue to ensure a clean state.
		 * Note: The queue holds pointers, and the memory is managed by 'reset_urb'.
		 * While direct deletion is safe here, draining is good practice to handle
		 * any residual items if the queue type changes in the future.
		 */
		while (usb_os_queue_receive(stream->urb_wait_queue, (void *)&urb_tmp, 0) == HAL_OK) {
			// just drop it
		}
		usb_os_queue_delete(stream->urb_wait_queue);
		stream->urb_wait_queue = NULL;
	}

	if (stream->urb_giveback_queue != NULL) {
		while (usb_os_queue_receive(stream->urb_giveback_queue, (void *)&urb_tmp, 0) == HAL_OK) {
			// just drop it
		}
		usb_os_queue_delete(stream->urb_giveback_queue);
		stream->urb_giveback_queue = NULL;
	}
}

/**
  * @brief	Init UVC decode resources
  * @param	stream: uvc stream interface
  * @retval Status
  */
static int usbh_uvc_decode_resources_init(usbh_uvc_stream_t *stream)
{
	if (usb_os_queue_create(&stream->urb_wait_queue, sizeof(usbh_uvc_urb_t *), USBH_UVC_URB_NUMS) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init wait Q fail\n");
		return HAL_ERR_UNKNOWN;
	}

	if (usb_os_queue_create(&stream->urb_giveback_queue, sizeof(usbh_uvc_urb_t *), USBH_UVC_URB_NUMS) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init giveback Q fail\n");
		usb_os_queue_delete(stream->urb_wait_queue);
		return HAL_ERR_UNKNOWN;
	}
	return HAL_OK;
}

/**
  * @brief	Init UVC decode thread
  * @param	stream: uvc stream interface
  * @retval Status
  */
static int usbh_uvc_decode_thread_init(usbh_uvc_stream_t *stream)
{
	if (rtos_task_create(&stream->decode_task, "usbh_uvc_decode_thread", usbh_uvc_decode_thread, (void *)stream, USBH_UVC_DECODE_TASK_STACK,
						 USBH_UVC_DECODE_TASK_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to create uvc decode thread\n");
		return HAL_ERR_UNKNOWN;
	}

	stream->complete_flag = 1;
	return HAL_OK;
}

/**
  * @brief	Discard the oldest frame in the frame_chain queue to reclaim a free buffer
  * @param	stream: uvc stream interface
  * @retval true: Successfully dropped the oldest ready frame and moved it to the frame_empty queue;
  *         false: frame_chain queue is empty or reclaim failed; no frame was discarded
  */
static inline bool usbh_uvc_discard_oldest_frame(usbh_uvc_stream_t *stream)
{
	usbh_uvc_frame_t *target = NULL;
	usbh_uvc_frame_t *head = NULL;

	if (list_empty(&stream->frame_chain)) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "frame_chain empty");
		return false;
	}

	/* get head */
	head = list_first_entry(&stream->frame_chain, usbh_uvc_frame_t, list);

	/* if head in using , try second */
	if (head->state == UVC_FRAME_FLYING || head->state == UVC_FRAME_INUSE) {
		/* get next */
		if (head->list.next != &stream->frame_chain) {
			usbh_uvc_frame_t *second = list_entry(head->list.next, usbh_uvc_frame_t, list);

			if (second->state != UVC_FRAME_FLYING &&
				second->state != UVC_FRAME_INUSE) {
				target = second;
			} else {
#if USBH_UVC_DEBUG
				/* if second also in used, skip; should not reach here */
				RTK_LOGS(TAG, RTK_LOG_INFO,
						 "discard_oldest: head=%u in-use, second=%u in-use, skip\n",
						 head->index, second->index);
#endif
				return false;
			}
		} else {
#if USBH_UVC_DEBUG
			/* only head in frame_chain, skip; should not reach here */
			RTK_LOGS(TAG, RTK_LOG_INFO,
					 "discard_oldest: only head=%u in-use, skip\n", head->index);
#endif
			return false;
		}
	} else {
		target = head;
	}

	/* drop */
	list_del_init(&target->list);
	usbh_uvc_reset_frame(target);
	target->state = UVC_FRAME_INIT;
	list_add_tail(&target->list, &stream->frame_empty);
#if USBH_UVC_DEBUG
	usbh_uvc_dump_simple(stream, "discard");
#endif
	stream->drop_cnt++;

#if USBH_UVC_DEBUG
	RTK_LOGS(TAG, RTK_LOG_INFO, "drop frame idx=%u 0x%x\n",
			 target->index, target);
#endif
	return true;
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

	if (buf == NULL) {
		return NULL;
	}

	if (buf->err == 1 || buf->byteused == 0) {
		/* drop it, add this buf in frame_empty */
		buf->state = UVC_FRAME_INIT;
		usbh_uvc_reset_frame(buf);
		list_add_tail(&buf->list, &stream->frame_empty);
	} else {
		/* buf not in empty add not in chain! just add it to frame_chain*/
		buf->state = UVC_FRAME_READY;
		list_add_tail(&buf->list, &stream->frame_chain);
#if USBH_UVC_DEBUG
		usbh_uvc_dump_simple(stream, "periodic");
#endif
		usb_os_sema_give(stream->frame_sema);
	}

	/* since app has a low priority, task yield here to let app go */
	usb_os_sleep_ms(1);

	/**
	  * Optimization Strategy:
	  * Prefetch only new empty frames when it is indeed necessary to write the next packet of data;
	  * When empty, first attempt to discard the oldest READY frame for recycling. If the oldest READY
	  * frame is still in using, continue to try recycling the second oldest READY frame;
	  * If it remains unavailable, do not preempt; return NULL and wait to acquire it again when the
	  * next actual write operation is needed.
	  */
	if (list_empty(&stream->frame_empty)) {
		if (usbh_uvc_discard_oldest_frame(stream)) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "drop %d\n", stream->drop_cnt);
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "No free buf3\n");
			return NULL;
		}
	}

	/* avoid accessing empty list */
	/* should not reach here */
	if (list_empty(&stream->frame_empty)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Reclaim empty fail\n");
		return NULL;
	}

	frame_buffer = list_first_entry(&stream->frame_empty, usbh_uvc_frame_t, list);
	list_del_init(&frame_buffer->list);

	usbh_uvc_reset_frame(frame_buffer);
	frame_buffer->state = UVC_FRAME_INIT;
	stream->frame_cnt ++;

	return frame_buffer;
}

/**
  * @brief	Decode a given URB
  * @param  stream: uvc stream interface
  * @param  urb: URB to decode
  * @retval None
  */
static void usbh_uvc_decode_urb(usbh_uvc_stream_t *stream, usbh_uvc_urb_t *urb)
{
	int i;
	u32 length;
	usbh_uvc_vs_payload_header_t *header;
	usbh_uvc_frame_t *frame_buffer = NULL;
	u8 *data;
	u8 *payload;
	u32 bytes, maxlen;
	u8 fid = 0;
	u8 err = 0;

	if (usbh_uvc_usb_status_check() != HAL_OK) {
		return;
	}

	if ((stream->next_xfer == 0) || (stream->is_resource_safe == 0)) {
		return;
	}

	stream->decoder_busy = 1;

	/* double check */
	if (stream->is_resource_safe == 0) {
		stream->decoder_busy = 0;
		return;
	}

	/* avoid continue getting frame_buffer, only first enter decode urb should reach here */
	frame_buffer = stream->cur_frame_buf;
	if (!frame_buffer) {
		if (!list_empty(&stream->frame_empty)) {
			frame_buffer = list_first_entry(&stream->frame_empty, usbh_uvc_frame_t, list);
			list_del_init(&frame_buffer->list);
			usbh_uvc_reset_frame(frame_buffer);
			stream->cur_frame_buf = frame_buffer;
			stream->last_fid = fid;
#if USBH_UVC_DEBUG
			usbh_uvc_dump_simple(stream, "decode");
#endif
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "No free buf1\n");
			goto exit_decoder;
		}
	}

	for (i = 0; i < (int)urb->packet_num; i ++) {

		data = urb->addr + urb->packet_info[i].offset;
		length = urb->packet_info[i].length;
		payload = data + data[0];

		header = (usbh_uvc_vs_payload_header_t *)data;

		if (length < header->bHeaderLength) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Err: payload len(%dd) < header len(%d)\n", length, header->bHeaderLength);
			goto exit_decoder;
		}

		if (header->bmHeaderInfo.b.err == 1) {
			/* Note: it need to send request if want to get error reason */
			RTK_LOGS(TAG, RTK_LOG_INFO, "Drop pkt:bmHeaderInfo.b.err = 1\n");
			err = 1;
			continue;
		}

		fid = header->bmHeaderInfo.b.fid;

		if (fid != stream->last_fid) {
			if (frame_buffer->byteused != 0) {
				frame_buffer->err = err ? 1 : 0;
				frame_buffer = usbh_uvc_next_frame_buffer(stream, frame_buffer);
				if (!frame_buffer) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "No free buf2\n");
					stream->cur_frame_buf = NULL;
					goto exit_decoder;
				}
				/* a new frame ，update info*/
				stream->cur_frame_buf = frame_buffer;
				stream->last_fid = fid;
				err = 0;
			} else {
				/* no data but fid changes, first urb of first frame after stream on will reach here */
				stream->last_fid = fid;
				err = 0;
			}
		}

		/* For first payload of a frame arrival, record timestamp */
		/* Note: host monotonic tick; consider replacing with device PTS/SCR if available and realiable */
		if (frame_buffer->byteused == 0) {
			frame_buffer->timestamp = usb_os_get_timestamp_ms();
		}

		maxlen = stream->frame_buffer_size - frame_buffer->byteused;
		bytes = MIN(maxlen, (length - data[0]));

		if (bytes != 0) {
			usb_os_memcpy(frame_buffer->buf + frame_buffer->byteused, payload, bytes);
			frame_buffer->byteused += bytes;
		}
	}

exit_decoder:
	stream->decoder_busy = 0;
}
#endif

/* Exported functions --------------------------------------------------------*/

/**
  * @brief	Process received rx video data
  * @param	stream: uvc stream interface
  * @retval Status
  */
int usbh_uvc_process_rx(usbh_uvc_stream_t *stream)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_urb_t *urb = NULL;
	usbh_uvc_setting_t *cur_setting = &stream->cur_setting;
	usbh_pipe_t *pipe = &cur_setting->pipe;
	u32 *urb_index;
	u32 *packet_index;
	u32 rx_len;
	u8 urb_state;

	if (stream->is_resource_safe == 0) {
		return HAL_OK;
	}

	urb_state = usbh_get_urb_state(uvc->host, pipe);

	if (urb_state == USBH_URB_DONE) {

		rx_len = usbh_get_last_transfer_size(uvc->host, pipe);
#if USBH_UVC_DEBUG
		if (rx_len > 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "stream[%d]:rx%d\n", stream->stream_idx,  rx_len);
		}
#endif
		urb_index = &(stream->cur_urb);
		packet_index = &(stream->cur_packet);

		/* some camera may send zero length packet, just drop it */
		if (rx_len == 0) {
			stream->cur_packet_state = 0;
#if (USBH_UVC_USE_SOF == 0)
			goto isoc_rev;
#else
			return HAL_OK;
#endif
		}

		if (stream->is_resource_safe == 0) {
			return HAL_OK;
		}

		stream->urb[*urb_index]->packet_info[*packet_index].length = rx_len;

		/* some cameras send payload header without any valid payload packets, decode_urb will hanle it */

		(*packet_index) ++;

		if (*packet_index == stream->urb[*urb_index]->packet_num) {
			*packet_index = 0;
			urb = usbh_uvc_urb_complete(stream, stream->urb[*urb_index]);
			if (!urb) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "URB complete fail\n");
				return HAL_TIMEOUT;
			}

			*urb_index = urb->index;
			*packet_index = 0;
		}

		stream->cur_packet_state = 0;
#if (USBH_UVC_USE_SOF == 0)
isoc_rev:
		if (stream->is_resource_safe && stream->next_xfer) {
			pipe->xfer_buf = stream->urb[*urb_index]->addr + stream->urb[*urb_index]->packet_info[*packet_index].offset;
			usbh_transfer_data(uvc->host, pipe);
			stream->cur_packet_state = 1;
		} else {
			// stop
			stream->cur_packet_state = 0;
		}
#endif
	}

	return HAL_OK;
}

/**
  * @brief	Process sof interrupt
  * @param	stream: usb host instance
  * @retval Status
  */
void usbh_uvc_process_sof(usb_host_t *host)
{
#if ((USBH_UVC_USE_SOF == 1) && (USBH_UVC_USE_HW == 0))
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream;
	usbh_uvc_setting_t *cur_setting = NULL;
	int i;
	u32 urb_index;
	u32 packet_index;
	u32 frame_num;
	usbh_pipe_t *pipe;

	frame_num = usbh_get_current_frame_number(host);

	for (i = 0; i < uvc->uvc_desc.vs_num; i++) {
		stream = &uvc->stream[i];
		if (stream->next_xfer == 1) {
			cur_setting = &stream->cur_setting;
			pipe = &cur_setting->pipe;
			if (stream->stream_data_state == STREAM_DATA_IN) {
				rtos_critical_enter(RTOS_CRITICAL_USB);
				if (usbh_uvc_frame_num_le(cur_setting->last_frame + pipe->ep_interval, frame_num) && \
					(stream->cur_packet_state == 0)) {
					stream->cur_packet_state = 1;
					urb_index = stream->cur_urb;
					packet_index = stream->cur_packet;
					cur_setting->last_frame = frame_num;
					pipe->xfer_buf = stream->urb[urb_index]->addr + stream->urb[urb_index]->packet_info[packet_index].offset;
					usbh_transfer_data(host, pipe);
				}
				rtos_critical_exit(RTOS_CRITICAL_USB);
			}
		}
	}
#else
	UNUSED(host);
#endif
}

/**
  * @brief	Push an completed URB to decode thread
  * @param	stream: uvc stream interface
  *         urb: the completed URB
  * @retval URB has been decoded and returned from decode thread
  */
usbh_uvc_urb_t *usbh_uvc_urb_complete(usbh_uvc_stream_t *stream, usbh_uvc_urb_t *urb)
{
	usbh_uvc_urb_t *urb_tmp = urb;

	if (usbh_uvc_usb_status_check() != HAL_OK) {
		return NULL;
	}

	if ((stream->is_resource_safe == 0) || (!stream->complete_flag)) {
		return NULL;
	}

	if (stream->next_xfer == 1) {
		if (usb_os_queue_send(stream->urb_wait_queue, (void *)&urb_tmp, wait_send_timeout) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to push urb\n");
			return NULL;
		}
	} else {
		return NULL;
	}

	urb_tmp = NULL;
	if (stream->next_xfer == 1) {
		if (usb_os_queue_receive(stream->urb_giveback_queue, (void *)&urb_tmp, giveback_recv_timeout) != HAL_OK) {
			if (!urb_tmp) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to pop urb\n");
				return NULL;
			}
		}
	}

	return urb_tmp;
}

/**
  * @brief	Send SET_CUR request
  * @param	stream: uvc stream interface
  * @param	probe: indicate probe or commit
  * @retval Status
  */
int usbh_uvc_set_video(usbh_uvc_stream_t *stream, int probe)
{
	usbh_setup_req_t setup;
	usbh_uvc_host_t *uvc = &uvc_host;
	usb_host_t *host = uvc->host;
	usbh_uvc_stream_control_t *ctrl = &stream->stream_ctrl;
	u8 *ptr = (u8 *)uvc->uvc_desc.vc_intf.vcheader;
	u16 size;

	/*note that for uvc1.5 wLength=48, not support now*/
	size = ((ptr[3] | ptr[4] << 8) >= 0x0110) ? 34 : 26;

	usb_os_memcpy(uvc->request_buf, (void *) ctrl, size);

	if (USB_IS_MEM_DMA_ALIGNED(uvc->request_buf)) {
		DCache_Clean((u32)uvc->request_buf, size);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "req buf align err!\n", uvc->request_buf);
		return HAL_ERR_MEM;
	}

	setup.req.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE | \
							  USB_REQ_TYPE_CLASS;
	setup.req.bRequest = USBH_UVC_SET_CUR;

	if (probe) {
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
int usbh_uvc_get_video(usbh_uvc_stream_t *stream, int probe, u16 request)
{
	usbh_setup_req_t setup;
	usbh_uvc_host_t *uvc = &uvc_host;
	usb_host_t *host = uvc->host;
	u16 size;

	/*note that for uvc1.5 wLength=48, not support now*/
	size = (uvc->uvc_desc.vc_intf.vcheader->bcdUVC >= 0x110) ? 34 : 26;

	setup.req.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | \
							  USB_REQ_TYPE_CLASS;
	setup.req.bRequest = request;

	if (probe) {
		setup.req.wValue = USBH_UVC_VS_PROBE_CONTROL;
	} else {
		setup.req.wValue = USBH_UVC_VS_COMMIT_CONTROL;
	}

	setup.req.wLength = size;
	setup.req.wIndex = stream->cur_setting.bInterfaceNumber;

	return usbh_ctrl_request(host, &setup, uvc->request_buf);
}

/**
  * @brief	Init video data
  * @param	stream: uvc stream interface
  * @retval Status
  */
int usbh_uvc_video_init(usbh_uvc_stream_t *stream)
{
	usbh_uvc_stream_control_t *ctrl = &stream->stream_ctrl;
	usbh_uvc_vs_t *vs = stream->vs_intf;
	u32 num_format = vs->nformat;
	u32 num_frame;
	usbh_uvc_vs_format_t *format = NULL;
	usbh_uvc_vs_frame_t *frame = NULL;
	int i;
	int found_format = 0;
	int found_frame = 0;

	/*select matched format */
	for (i = 0; i < (int)num_format; i ++) {
		format = &vs->format[i];

		if (format->index == ctrl->bFormatIndex) {
			found_format = 1;
			break;
		}
	}

	if (found_format) {
		/*select matched frame */
		num_frame = format->nframes;
		for (i = 0; i < (int)num_frame; i ++) {

			frame = &format->frame[i];
			if (frame->bFrameIndex == ctrl->bFrameIndex) {
				found_frame = 1;
				break;
			}
		}

		stream->def_format = format;
		stream->cur_format = format;

		if (found_frame) {
			stream->cur_frame = frame;
		}
	}

	return HAL_OK;
}


static void usbh_uvc_free_decode_resources(usbh_uvc_stream_t *stream)
{
	int i;

	if (stream->decode_task != NULL) {
		rtos_task_delete(stream->decode_task);
		stream->decode_task = NULL;
		stream->complete_flag = 0;
	}

	if (stream->urb_wait_queue != NULL) {
		usb_os_queue_delete(stream->urb_wait_queue);
		stream->urb_wait_queue = NULL;
	}

	if (stream->urb_giveback_queue != NULL) {
		usb_os_queue_delete(stream->urb_giveback_queue);
		stream->urb_giveback_queue = NULL;
	}

	for (i = 0; i < USBH_UVC_URB_NUMS; i++) {
		if (stream->urb[i] != NULL) {
			usb_os_mfree(stream->urb[i]);
			stream->urb[i] = NULL;
		}
	}

	if (stream->urb_buffer != NULL) {
		usb_os_mfree(stream->urb_buffer);
		stream->urb_buffer = NULL;
		stream->urb_buffer_size = 0;
	}

	stream->is_resource_safe = 0;
	stream->cur_urb = 0;
}

#if (USBH_UVC_USE_HW == 1)
/**
  * @brief  UVC HW Error Recorder
  * @param  err: Error type
  */
static void usbh_uvc_err_record(usbh_hw_uvc_err_status_t err)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_err_t *curr = &uvc->hw_stats;
	u8 err_flag = 0;

	if (err & USBH_HW_UVC_CH0_BUF0_OVERSIZE) {
		curr->err_ch0_buf0_ov++;
		err_flag = 1;
	}
	if (err & USBH_HW_UVC_CH0_BUF1_OVERSIZE) {
		curr->err_ch0_buf1_ov++;
		err_flag = 1;
	}
	if (err & USBH_HW_UVC_CH0_BUF0_HEADER) {
		curr->err_ch0_buf0_h++;
		err_flag = 1;
	}
	if (err & USBH_HW_UVC_CH0_BUF1_HEADER) {
		curr->err_ch0_buf1_h++;
		err_flag = 1;
	}

	if (err & USBH_HW_UVC_CH1_BUF0_OVERSIZE) {
		curr->err_ch1_buf0_ov++;
		err_flag = 1;
	}
	if (err & USBH_HW_UVC_CH1_BUF1_OVERSIZE) {
		curr->err_ch1_buf1_ov++;
		err_flag = 1;
	}
	if (err & USBH_HW_UVC_CH1_BUF0_HEADER) {
		curr->err_ch1_buf0_h++;
		err_flag = 1;
	}
	if (err & USBH_HW_UVC_CH1_BUF1_HEADER) {
		curr->err_ch1_buf1_h++;
		err_flag = 1;
	}

#if USBH_HW_UVC_DUMP_ERR
	if (err_flag && (uvc->dump_sema) && (!uvc->dump_task_exit)) {
		rtos_sema_give(uvc->dump_sema);
	}
#else
	UNUSED(err_flag);
#endif
}

#if USBH_HW_UVC_DUMP_ERR
static void usbh_uvc_status_dump(void)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_err_t *curr = NULL;
	usbh_uvc_err_t *last = NULL;

	curr = &uvc->hw_stats;
	last = &uvc->hw_stats_shadow;

	RTK_LOGS(TAG, RTK_LOG_WARN,
			 "CH0 ERR %d-%d-%d-%d, CH1 ERR %d-%d-%d-%d\n",
			 curr->err_ch0_buf0_ov, curr->err_ch0_buf1_ov,
			 curr->err_ch0_buf0_h,  curr->err_ch0_buf1_h,
			 curr->err_ch1_buf0_ov, curr->err_ch1_buf1_ov,
			 curr->err_ch1_buf0_h,  curr->err_ch1_buf1_h);
	*last = *curr;

}

/**
  * @brief  UVC status dump thread
  */
void usbh_uvc_status_dump_thread(void *param)
{
	UNUSED(param);
	usbh_uvc_host_t *uvc = &uvc_host;

	uvc->dump_task_alive = 1;
	while (!uvc->dump_task_exit) {
		if (rtos_sema_take(uvc->dump_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (uvc->dump_task_exit) {
				break;
			}
			usbh_uvc_status_dump();
		}
	}

	if (uvc->dump_sema) {
		rtos_sema_delete(uvc->dump_sema);
		uvc->dump_sema = NULL;
	}

	uvc->dump_task_alive = 0;
	rtos_task_delete(NULL);
}
#endif
#endif

/**
  * @brief  Init streaming data
  * @param	stream: uvc stream interface
  * @retval Status
  */
int usbh_uvc_stream_init(usbh_uvc_stream_t *stream)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_setting_t *cur_setting = &stream->cur_setting;
	usbh_pipe_t *pipe = &cur_setting->pipe;
	usbh_uvc_frame_t *frame = NULL;
	u32 frame_buf_size = CACHE_LINE_ALIGNMENT(stream->frame_buffer_size);
	int i;

	usb_os_sema_create(&stream->frame_sema);

	/*init frame buffer*/
	INIT_LIST_HEAD(&stream->frame_chain);
	INIT_LIST_HEAD(&stream->frame_empty);
	stream->frame_buf = (u8 *)usb_os_malloc(USBH_UVC_VIDEO_FRAME_NUMS * frame_buf_size);
	if (stream->frame_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc frame buf fail\n");
		return HAL_ERR_MEM;
	}

	for (i = 0; i < USBH_UVC_VIDEO_FRAME_NUMS; i++) {
		frame = &stream->frame_buffer[i];
		frame->buf = stream->frame_buf + i * frame_buf_size;
		frame->index = i;
		frame->state = UVC_FRAME_INIT;
		usbh_uvc_reset_frame(frame);
		INIT_LIST_HEAD(&frame->list);
		list_add_tail(&frame->list, &stream->frame_empty);
	}

	stream->cur_frame_buf = NULL;
	stream->last_fid = 0xFF;
	stream->drop_cnt = 0;

	/* set urb buffer */
	if (usbh_uvc_set_urb(stream) != HAL_OK) {
		goto clean_decode_exit;
	}

#if (USBH_UVC_USE_HW == 0)
	int status;

	stream->decoder_busy = 0;
	status = usbh_uvc_decode_resources_init(stream);
	if (status != HAL_OK) {
		goto clean_decode_exit;
	}

	status = usbh_uvc_fill_giveback_queue(stream);
	if (status != HAL_OK) {
		goto clean_decode_exit;
	}

	stream->is_resource_safe = 1;

	/* init decode thread */
	status = usbh_uvc_decode_thread_init(stream);
	if (status != HAL_OK) {
		goto clean_decode_exit;
	}
#endif

#if (USBH_UVC_USE_HW == 1)

	stream->uvc_dec = usbh_hw_uvc_alloc_channel();
	if (stream->uvc_dec == NULL) {
		return HAL_ERR_PARA;
	}

	stream->is_resource_safe = 1;

	usbh_hw_uvc_dec_t *uvc_dec = stream->uvc_dec;
	uvc_dec->dev_addr = uvc->host->dev_addr;
	for (int i = 0; i < USBH_UVC_VIDEO_FRAME_NUMS; i ++) {
		uvc_dec->buf[i].buf_start_addr = (u32)stream->frame_buffer[i].buf;
		uvc_dec->buf[i].buf_size = frame_buf_size;
	}

	uvc->hw_error = usbh_uvc_err_record;
	stream->uvc_dec->err_cb = uvc->hw_error;
	usbh_hw_uvc_init(stream->uvc_dec, pipe);

	rtos_critical_enter(RTOS_CRITICAL_USB);
	if (uvc->hw_irq_ref_cnt == 0) {
		usbh_hw_uvc_irq_en(uvc->hw_isr_pri);
	}
	uvc->hw_irq_ref_cnt++;
	rtos_critical_exit(RTOS_CRITICAL_USB);

	usbh_hw_uvc_start(stream->uvc_dec);
#endif

#if ((USBH_UVC_USE_SOF == 0)&&(USBH_UVC_USE_HW == 0))
	pipe->xfer_buf = stream->urb[stream->cur_urb]->addr + stream->urb[stream->cur_urb]->packet_info[stream->cur_packet].offset;
	usbh_transfer_data(uvc->host, pipe);
#endif
	return HAL_OK;

clean_decode_exit:
	usbh_uvc_free_decode_resources(stream);
	return HAL_ERR_MEM;
}

/**
  * @brief  Deinit streaming data
  * @param	stream: uvc stream interface
  * @retval Status
  */
void usbh_uvc_stream_deinit(usbh_uvc_stream_t *stream)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_setting_t *cur_setting = &stream->cur_setting;
	usbh_uvc_frame_t *frame = NULL;
	int i = 0;

#if USBH_UVC_USE_HW
	if (stream->uvc_dec) {
		usbh_hw_uvc_stop(stream->uvc_dec);
		usbh_hw_uvc_deinit(stream->uvc_dec);
		usbh_hw_uvc_free_channel(stream->uvc_dec);
		stream->uvc_dec = NULL;

		rtos_critical_enter(RTOS_CRITICAL_USB);
		if (uvc->hw_irq_ref_cnt > 0) {
			uvc->hw_irq_ref_cnt--;
			if (uvc->hw_irq_ref_cnt == 0) {
				usbh_hw_uvc_irq_dis();
			}
		}
		rtos_critical_exit(RTOS_CRITICAL_USB);
	}
#endif

	/* close pipe */
	usbh_close_pipe(uvc->host, &cur_setting->pipe);

	/* delete decode task */
#if (USBH_UVC_USE_HW == 0)
	usbh_uvc_decode_thread_deinit(stream);
#endif

	/* reset urb buffer */
	usbh_uvc_reset_urb(stream);

	INIT_LIST_HEAD(&stream->frame_chain);
	INIT_LIST_HEAD(&stream->frame_empty);

	for (i = 0; i < USBH_UVC_VIDEO_FRAME_NUMS; i++) {
		frame = &stream->frame_buffer[i];
		INIT_LIST_HEAD(&frame->list);
		usbh_uvc_reset_frame(frame);
		frame->index = 0;
		frame->buf = NULL;
		frame->state = UVC_FRAME_INIT;
	}

	stream->cur_urb = 0;
	stream->last_fid = 0;
	stream->cur_frame_buf = 0;
	stream->urb_buffer_size = 0;
	stream->drop_cnt = 0;

	if (stream->frame_sema) {
		rtos_sema_give(stream->frame_sema);
		rtos_sema_delete(stream->frame_sema);
		stream->frame_sema = NULL;
	}

	if (stream->frame_buf) {
		usb_os_mfree(stream->frame_buf);
		stream->frame_buf = NULL;
	}
}

