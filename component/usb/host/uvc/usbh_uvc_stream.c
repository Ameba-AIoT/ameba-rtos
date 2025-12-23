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
static int uvc_frame_num_le(u32 frame1, u32 frame2)
{
	return ((frame2 - frame1) & HFNUM_MAX_FRNUM) <= (HFNUM_MAX_FRNUM >> 1);
}
#endif

/**
  * @brief	Config uvc urb buffer
  * @param	stream: uvc stream interface
  * @retval None
  */
static void usbh_uvc_set_urb(usbh_uvc_stream_t *stream)
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

	for (i = 0; i < USBH_UVC_URB_NUMS; i ++) {
		stream->urb[i] = (usbh_uvc_urb_t *)usb_os_malloc(sizeof(usbh_uvc_urb_t) + (pkt_cnt + 1) * sizeof(usbh_uvc_packet_desc_t));
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

#if (USBH_UVC_USE_HW == 0)
	for (i = 1; i < USBH_UVC_URB_NUMS; i ++) {
		if (usb_os_queue_send(stream->urb_giveback_queue, (void *)&stream->urb[i], RTOS_MAX_TIMEOUT) != HAL_OK) {
			usb_os_queue_delete(stream->urb_wait_queue);
			usb_os_queue_delete(stream->urb_giveback_queue);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Push to giveback Q fail\n");
			return;
		}
	}
#endif
}

/**
  * @brief	Reset uvc urb buffer
  * @param	stream: uvc stream interface
  * @retval None
  */
static void usbh_uvc_reset_urb(usbh_uvc_stream_t *stream)
{
	int i;

	for (i = 0; i < USBH_UVC_URB_NUMS; i++) {
		stream->urb[i]->addr = NULL;
		stream->urb[i]->packet_num = 0;
		stream->urb[i]->index = 0;
		stream->urb[i]->packet_length = 0;
		usb_os_mfree(stream->urb[i]);
	}

	usb_os_mfree(stream->urb_buffer);
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
			 tag, empty_len, ready_len, USBH_UVC_VIDEO_MAX_FRAME);

	for (int i = 0; i < USBH_UVC_VIDEO_MAX_FRAME; i++) {
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
	usbh_uvc_host_t *uvc = &uvc_host;

	stream->complete_on = 1;
	while (stream->complete_on) {
		if (uvc->host->connect_state < USBH_STATE_ATTACH) {
			//wait to delete task
			while (1);
		}

		if (usb_os_queue_receive(stream->urb_wait_queue, (void *)&urb_tmp, wait_recv_timeout) != HAL_OK) {
			//usb_os_sleep_ms(1);
			continue;
		}

		usbh_uvc_decode_urb(stream, urb_tmp);

		while (usb_os_queue_send(stream->urb_giveback_queue, (void *)&urb_tmp, giveback_send_timeout) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to push urb\n");
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
	int i;
	usbh_uvc_urb_t *urb_tmp;

	if (stream->decode_task != NULL) {
		rtos_task_delete(stream->decode_task);
	}

	stream->complete_flag = 0;
	stream->complete_on = 0;

	for (i = 0; i < USBH_UVC_URB_NUMS; i ++) {
		usb_os_queue_receive(stream->urb_wait_queue, (void *)&urb_tmp, 0);
		usb_os_queue_receive(stream->urb_giveback_queue, (void *)&urb_tmp, 0);
	}

	usb_os_queue_delete(stream->urb_wait_queue);
	usb_os_queue_delete(stream->urb_giveback_queue);
}


/**
  * @brief	Init UVC decode thread
  * @param	stream: uvc stream interface
  * @retval Status
  */
static int usbh_uvc_decode_thread_init(usbh_uvc_stream_t *stream)
{
	if (usb_os_queue_create(&stream->urb_wait_queue, sizeof(usbh_uvc_urb_t *), USBH_UVC_URB_NUMS) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init wait Q fail\n");
		return -1;
	}

	if (usb_os_queue_create(&stream->urb_giveback_queue, sizeof(usbh_uvc_urb_t *), USBH_UVC_URB_NUMS) != HAL_OK) {
		usb_os_queue_delete(stream->urb_wait_queue);
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init giveback Q fail\n");
		return -1;
	}

	if (rtos_task_create(&stream->decode_task, "usbh_uvc_decode_thread", usbh_uvc_decode_thread, (void *)stream, USBH_UVC_DECODE_TASK_STACK,
						 USBH_UVC_DECODE_TASK_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to create uvc decode thread\n");
		return -1;
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
			return;
		}
	}

	for (i = 0; i < (int)urb->packet_num; i ++) {

		data = urb->addr + urb->packet_info[i].offset;
		length = urb->packet_info[i].length;
		payload = data + data[0];

		header = (usbh_uvc_vs_payload_header_t *)data;

		if (length < header->bHeaderLength) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Err: payload len(%dd) < header len(%d)\n", length, header->bHeaderLength);
			return;
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
					return;
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
			frame_buffer->timestamp = usb_hal_get_timestamp_ms();
		}

		maxlen = stream->frame_buffer_size - frame_buffer->byteused;
		bytes = MIN(maxlen, (length - data[0]));

		if (bytes != 0) {
			usb_os_memcpy(frame_buffer->buf + frame_buffer->byteused, payload, bytes);
			frame_buffer->byteused += bytes;
		}
	}

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

	urb_state = usbh_get_urb_state(uvc->host, pipe);

	if (urb_state == USBH_URB_DONE) {

		rx_len = usbh_get_last_transfer_size(uvc->host, pipe);
		//RTK_LOGS(TAG, RTK_LOG_DEBUG, "rx%d:%d\n",stream->stream_num,  rx_len);

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
		pipe->xfer_buf = stream->urb[*urb_index]->addr + stream->urb[*urb_index]->packet_info[*packet_index].offset;

		usbh_transfer_data(uvc->host, pipe);
		stream->cur_packet_state = 1;
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
		cur_setting = &stream->cur_setting;
		pipe = &cur_setting->pipe;
		if (stream->stream_data_state == STREAM_DATA_IN) {
			rtos_critical_enter(RTOS_CRITICAL_USB);
			if (uvc_frame_num_le(cur_setting->last_frame + pipe->ep_interval, frame_num) && \
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
	usbh_uvc_host_t *uvc = &uvc_host;

	if (uvc->host->connect_state < USBH_STATE_ATTACH) {
		return NULL;
	}

	if (!stream->complete_flag) {
		return NULL;
	}

	if (usb_os_queue_send(stream->urb_wait_queue, (void *)&urb_tmp, wait_send_timeout) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to push urb\n");
		return NULL;
	}

	urb_tmp = NULL;

	if (usb_os_queue_receive(stream->urb_giveback_queue, (void *)&urb_tmp, giveback_recv_timeout) != HAL_OK) {
		if (!urb_tmp) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to pop urb\n");
		}
	}

	//usb_os_sleep_ms(1);

	return urb_tmp;
}

/**
  * @brief	Send SET_CUR request
  * @param	stream: uvc stream interface
  			probe: indicate probe or commit
  * @retval Status
  */
int usbh_uvc_set_video(usbh_uvc_stream_t *stream, int probe)
{
	usbh_setup_req_t setup;
	usbh_uvc_host_t *uvc = &uvc_host;
	usb_host_t *host = uvc->host;
	usbh_uvc_stream_control_t *ctrl = &stream->stream_ctrl;
	int ret;
	u8 *data;
	u8 *ptr = (u8 *)uvc->uvc_desc.vc_intf.vcheader;
	u16 size;

	/*note that for uvc1.5 wLength=48, not support now*/
	size = ((ptr[3] | ptr[4] << 8) >= 0x0110) ? 34 : 26;

	data = (u8 *)usb_os_malloc(size);
	usb_os_memcpy(data, (void *) ctrl, size);

	setup.req.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE | \
							  USB_REQ_TYPE_CLASS;
	setup.req.bRequest = USBH_UVC_SET_CUR;

	if (probe) {
		setup.req.wValue = VS_PROBE_CONTROL;
	} else {
		setup.req.wValue = VS_COMMIT_CONTROL;
	}

	setup.req.wLength = size;
	setup.req.wIndex = stream->cur_setting.bInterfaceNumber;

	do {
		ret = usbh_ctrl_request(host, &setup, data);
	} while (ret == HAL_BUSY);

	usb_os_mfree(data);

	return HAL_OK;
}

/**
  * @brief	Send GET request, such as GET_CUR, GET_DEF
  * @param	stream: uvc stream interface
  			probe: indicate probe or commit
  			request: indicate which request
  * @retval Status
  */
int usbh_uvc_get_video(usbh_uvc_stream_t *stream, int probe, u16 request)
{
	usbh_setup_req_t setup;
	usbh_uvc_host_t *uvc = &uvc_host;
	usb_host_t *host = uvc->host;
	usbh_uvc_stream_control_t *ctrl = &stream->stream_ctrl;
	int ret;
	u8 *data;
	u16 size;

	/*note that for uvc1.5 wLength=48, not support now*/
	size = (uvc->uvc_desc.vc_intf.vcheader->bcdUVC >= 0x110) ? 34 : 26;

	data = (u8 *)usb_os_malloc(size);
	usb_os_memcpy(data, (void *) ctrl, size);

	setup.req.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | \
							  USB_REQ_TYPE_CLASS;
	setup.req.bRequest = request;

	if (probe) {
		setup.req.wValue = VS_PROBE_CONTROL;
	} else {
		setup.req.wValue = VS_COMMIT_CONTROL;
	}

	setup.req.wLength = size;
	setup.req.wIndex = stream->cur_setting.bInterfaceNumber;

	do {
		ret = usbh_ctrl_request(host, &setup, data);
	} while (ret == HAL_BUSY);

	/* Note: Some broken devices may return wrong dwMaxVideoFrameSize and dwMaxPayloadTransferSize */

	usb_os_memcpy((void *) ctrl, (void *)data, size);
	usb_os_mfree(data);

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

	return HAL_OK;
}

/**
  * @brief	Probe video process
  * @param	stream: uvc stream interface
  * @retval Status
  */
int usbh_uvc_probe_video(usbh_uvc_stream_t *stream)
{
	/*probe video setting*/
	usbh_uvc_set_video(stream, 1);
	usbh_uvc_get_video(stream, 1, USBH_UVC_GET_CUR);
	usbh_uvc_set_video(stream, 1);

	return HAL_OK;
}

/**
  * @brief	Commit video process
  * @param	stream: uvc stream interface
  * @retval Status
  */
int usbh_uvc_commit_video(usbh_uvc_stream_t *stream)
{
	/*commit video setting*/
	usbh_uvc_set_video(stream, 0);

	return HAL_OK;
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

	/*probe flow*/
	/*get default video setting*/
	usbh_uvc_get_video(stream, 1, USBH_UVC_GET_DEF);

	/*set defalt video setting*/
	usbh_uvc_set_video(stream, 1);

	/*get current video setting*/
	usbh_uvc_get_video(stream, 1, USBH_UVC_GET_CUR);

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
	int i;

	usb_os_sema_create(&stream->frame_sema);

	/*init frame buffer*/
	INIT_LIST_HEAD(&stream->frame_chain);
	INIT_LIST_HEAD(&stream->frame_empty);
	stream->frame_buffer_size = CACHE_LINE_ALIGNMENT(USBH_UVC_VIDEO_FRAME_SIZE);
	stream->frame_buf = (u8 *)usb_os_malloc(USBH_UVC_VIDEO_MAX_FRAME * stream->frame_buffer_size);

	for (i = 0; i < USBH_UVC_VIDEO_MAX_FRAME; i++) {
		frame = &stream->frame_buffer[i];
		frame->buf = stream->frame_buf + i * stream->frame_buffer_size;
		frame->index = i;
		frame->state = UVC_FRAME_INIT;
		usbh_uvc_reset_frame(frame);
		INIT_LIST_HEAD(&frame->list);
		list_add_tail(&frame->list, &stream->frame_empty);
	}

	stream->cur_frame_buf = NULL;
	stream->last_fid = 0xFF;
	stream->drop_cnt = 0;
#if (USBH_UVC_USE_HW == 0)
	/* init decode thread */
	usbh_uvc_decode_thread_init(stream);
#endif

	/* set urb buffer */
	usbh_uvc_set_urb(stream);

#if (USBH_UVC_USE_HW == 1)
	stream->uvc_dec = usbh_uvc_dec_alloc_channel();
	rtos_sema_create_binary(&stream->uvc_dec->dec_sema);

	for (int i = 0; i < USBH_UVC_VIDEO_MAX_FRAME; i ++) {
		stream->uvc_dec->uvc_dec_buf[i].buf_start_addr = (u32)stream->frame_buffer[i].buf;
		stream->uvc_dec->uvc_dec_buf[i].buf_size = USBH_UVC_VIDEO_FRAME_SIZE;
	}

	stream->uvc_dec->dev_addr = uvc->host->dev_addr;
	stream->uvc_dec->xfer_len = pipe->xfer_len;
	stream->uvc_dec->ep_num = pipe->ep_addr & 0x7FU;
	stream->uvc_dec->binterval = pipe->ep_interval;
	stream->uvc_dec->ep_mps = pipe->ep_mps;
	stream->uvc_dec->pipe_num = pipe->pipe_num;

	usbh_uvc_dec_init(stream->uvc_dec);
	usbh_uvc_dec_start(stream->uvc_dec);
#endif

#if ((USBH_UVC_USE_SOF == 0)&&(USBH_UVC_USE_HW == 0))
	pipe->xfer_buf = stream->urb[stream->cur_urb]->addr + stream->urb[stream->cur_urb]->packet_info[stream->cur_packet].offset;
	usbh_transfer_data(uvc->host, pipe);
#endif
	return HAL_OK;
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
	int i;
	int status;

#if USBH_UVC_USE_HW
	usbh_uvc_dec_stop(stream->uvc_dec);
	usbh_uvc_dec_deinit(stream->uvc_dec);
	usbh_uvc_dec_free_channel(stream->uvc_dec);
	rtos_sema_delete(stream->uvc_dec->dec_sema);
	stream->uvc_dec = NULL;
#endif

	do {
		status = usbh_ctrl_set_interface(uvc->host, cur_setting->bInterfaceNumber, 0);
	} while (status == HAL_BUSY);

	/* close pipe */
	usbh_close_pipe(uvc->host, &cur_setting->pipe);

	/* reset urb buffer */
	usbh_uvc_reset_urb(stream);

	/* delete decode task */
#if (USBH_UVC_USE_HW == 0)
	usbh_uvc_decode_thread_deinit(stream);
#endif

	INIT_LIST_HEAD(&stream->frame_chain);
	INIT_LIST_HEAD(&stream->frame_empty);

	for (i = 0; i < USBH_UVC_VIDEO_MAX_FRAME; i++) {
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

	usb_os_sema_delete(stream->frame_sema);

	usb_os_mfree(stream->frame_buf);
}

