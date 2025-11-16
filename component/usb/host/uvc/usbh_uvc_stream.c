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
  * @brief	Find out best alt setting
  * @param	stream: uvc stream interface
  * @retval None
  */
static void usbh_uvc_find_alt(usbh_uvc_stream_t *stream)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usb_host_t *host = uvc->host;
	usbh_uvc_setting_t *cur_setting = &stream->cur_setting;
	usbh_ep_desc_t *ep = cur_setting->altsetting->endpoint;
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
  * @brief	Config uvc urb buffer
  * @param	stream: uvc stream interface
  * @retval None
  */
static void usbh_uvc_set_buf(usbh_uvc_stream_t *stream)
{
	int i, j;
	u32 max_pkt_size, pkt_cnt, pkt_stride;

	max_pkt_size = stream->cur_setting.xfer_size;
	pkt_stride = CACHE_LINE_ALIGNMENT(max_pkt_size);
	pkt_cnt = USBH_UVC_URB_SIZE / max_pkt_size;

	//RTK_LOGS(TAG, RTK_LOG_DEBUG, "MPS:%d, pkt_cnt:%d\n", max_pkt_size, pkt_cnt);

	/*init urb*/
	stream->urb_buffer_size = pkt_cnt * pkt_stride;
	stream->uvc_buffer = (u8 *)usb_os_malloc(USBH_UVC_URB_NUMS * stream->urb_buffer_size);

	for (i = 0; i < USBH_UVC_URB_NUMS; i ++) {
		stream->urb[i] = (usbh_uvc_urb_t *)usb_os_malloc(sizeof(usbh_uvc_urb_t) + (pkt_cnt + 1) * sizeof(usbh_uvc_packet_desc_t));
		stream->urb[i]->addr = (u8 *)((u32)stream->uvc_buffer + i * stream->urb_buffer_size);
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
static void usbh_uvc_reset_buf(usbh_uvc_stream_t *stream)
{
	int i;

	for (i = 0; i < USBH_UVC_URB_NUMS; i++) {
		stream->urb[i]->addr = NULL;
		stream->urb[i]->packet_num = 0;
		stream->urb[i]->index = 0;
		stream->urb[i]->packet_length = 0;
		usb_os_mfree(stream->urb[i]);
	}

	usb_os_mfree(stream->uvc_buffer);
}
#if (USBH_UVC_USE_HW == 0)
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
		if (!usbh_get_status()) {
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
  * @brief	Get next frame buffer to fill video data
  * @param	stream: uvc stream interface
  *         buf: the frame which has been filled
  * @retval Pointer of next frame buffer to be filled
  */
static usbh_uvc_frame_t *usbh_uvc_next_frame_buffer(usbh_uvc_stream_t *stream, usbh_uvc_frame_t *buf)
{
	usbh_uvc_frame_t *frame_buffer;

	if (buf->err == 1) {
		buf->err = 0;
		buf->byteused = 0;
		stream->err_frame_cnt ++;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Drop err frame\n");
		return buf;
	}

	list_del_init(&buf->list);
	list_add(&buf->list, &stream->frame_chain);
	usb_os_sema_give(stream->frame_sema);

	/* since app has a low priority, task yield here to let app go */
	usb_os_sleep_ms(1);

	if (!list_empty(&stream->frame_empty)) {
		frame_buffer = list_first_entry(&stream->frame_empty, usbh_uvc_frame_t, list);
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "No free uvc buffer 3\n");
		return NULL;
	}

	frame_buffer->byteused = 0;
	stream->frame_cnt ++;

	return frame_buffer;
}

/**
  * @brief	Decode a given URB
  * @param  stream: uvc stream interface
  *         urb: URB to decode
  * @retval None
  */
static void usbh_uvc_decode_urb(usbh_uvc_stream_t *stream, usbh_uvc_urb_t *urb)
{
	int i;
	u32 length;
	usbh_uvc_vs_payload_header_t *header;
	usbh_uvc_frame_t *frame_buffer;
	u8 *data;
	u8 *payload;
	u32 bytes, maxlen;
	u8 fid;

	if (!list_empty(&stream->frame_empty)) {
		frame_buffer = list_first_entry(&stream->frame_empty, usbh_uvc_frame_t, list);
	} else {
		usb_os_sema_give(stream->frame_sema);
		RTK_LOGS(TAG, RTK_LOG_INFO, "No free uvc buffer\n");
		return;
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
			frame_buffer->err = 1;
			continue;
		}

		fid = header->bmHeaderInfo.b.fid;

		if (fid != stream->last_fid) {
			if (frame_buffer->byteused != 0) {
				frame_buffer = usbh_uvc_next_frame_buffer(stream, frame_buffer);
				if (!frame_buffer) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "No free uvc buffer 1\n");
					return;
				}
			}
			stream->last_fid = fid;
		}

		maxlen = stream->frame_buffer_size - frame_buffer->byteused;
		bytes = MIN(maxlen, (length - data[0]));

		if (bytes != 0) {
			usb_os_memcpy(frame_buffer->buf + frame_buffer->byteused, payload, bytes);
			frame_buffer->byteused += bytes;
		}

#if USBH_UVC_DETECT_EOF
		if (header->bmHeaderInfo.b.eof == 1) {
			frame_buffer = usbh_uvc_next_frame_buffer(stream, frame_buffer);
			if (!frame_buffer) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "No free uvc buffer 2\n");
				return;
			}
			stream->last_fid = fid;
		}
#endif
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
	u32 *urb_index;
	u32 *packet_index;
	u32 rx_len;
	usbh_uvc_urb_t *urb;
	usbh_uvc_setting_t *cur_setting = &stream->cur_setting;
	u8 urb_state;

	urb_state = usbh_get_urb_state(uvc->host, cur_setting->pipe);

	if (urb_state == USBH_URB_DONE) {

		rx_len = usbh_get_last_transfer_size(uvc->host, cur_setting->pipe);
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
		usbh_isoc_receive_data(uvc->host,
							   stream->urb[*urb_index]->addr + stream->urb[*urb_index]->packet_info[*packet_index].offset,
							   cur_setting->xfer_size,
							   cur_setting->pipe);
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
	int i;
	u8 *addr;
	u32 urb_index;
	u32 packet_index;
	u32 frame_num;

	frame_num = usbh_get_current_frame(host);

	for (i = 0; i < uvc->uvc_desc.vs_num; i++) {
		stream = &uvc->stream[i];
		if (stream->stream_data_state == STREAM_DATA_IN) {
			rtos_critical_enter(RTOS_CRITICAL_USB);
			if (uvc_frame_num_le(cur_setting->last_frame + cur_setting->interval, frame_num) && \
				(stream->cur_packet_state == 0)) {
				stream->cur_packet_state = 1;
				urb_index = stream->cur_urb;
				packet_index = stream->cur_packet;
				addr = stream->urb[urb_index]->addr + stream->urb[urb_index]->packet_info[packet_index].offset;
				cur_setting->last_frame = frame_num;
				usbh_isoc_receive_data(host,
									   addr,
									   cur_setting->xfer_size,
									   cur_setting->pipe);

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

	if (!usbh_get_status()) {
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
	int ret, retry_cnt;
	u8 *data;
	u8 *ptr = (u8 *)uvc->uvc_desc.vc_intf.vcheader;
	u16 size;

	/*note that for uvc1.5 wLength=48, not support now*/
	size = ((ptr[3] | ptr[4] << 8) >= 0x0110) ? 34 : 26;

	data = (u8 *)usb_os_malloc(size);
	usb_os_memcpy(data, (void *) ctrl, size);

	setup.b.bmRequestType = USB_H2D | USB_REQ_RECIPIENT_INTERFACE | \
							USB_REQ_TYPE_CLASS;
	setup.b.bRequest = USBH_UVC_SET_CUR;

	if (probe) {
		setup.b.wValue = VS_PROBE_CONTROL;
	} else {
		setup.b.wValue = VS_COMMIT_CONTROL;
	}

	setup.b.wLength = size;
	setup.b.wIndex = stream->cur_setting.bInterfaceNumber;

	do {
		ret = usbh_ctrl_request(host, &setup, data);
		if (ret == HAL_OK) {
			break;
		} else {
			retry_cnt++;
			usb_os_sleep_ms(10);
		}
	} while (retry_cnt < 10);

	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Set err\n");
		return HAL_ERR_HW;
	}

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
	int ret, retry_cnt;
	u8 *data;
	u16 size;

	/*note that for uvc1.5 wLength=48, not support now*/
	size = (uvc->uvc_desc.vc_intf.vcheader->bcdUVC >= 0x110) ? 34 : 26;

	data = (u8 *)usb_os_malloc(size);
	usb_os_memcpy(data, (void *) ctrl, size);

	setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_INTERFACE | \
							USB_REQ_TYPE_CLASS;
	setup.b.bRequest = request;

	if (probe) {
		setup.b.wValue = VS_PROBE_CONTROL;
	} else {
		setup.b.wValue = VS_COMMIT_CONTROL;
	}

	setup.b.wLength = size;
	setup.b.wIndex = stream->cur_setting.bInterfaceNumber;

	do {
		ret = usbh_ctrl_request(host, &setup, data);
		if (ret == HAL_OK) {
			break;
		} else {
			retry_cnt++;
			usb_os_sleep_ms(10);
		}
	} while (retry_cnt < 10);

	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "GET err\n");
		return HAL_ERR_HW;
	}

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
	int i;
	usbh_uvc_stream_control_t *ctrl = &stream->stream_ctrl;
	usbh_uvc_vs_t *vs = stream->vs_intf;
	u32 num_format = vs->nformat;
	u32 num_frame;
	usbh_uvc_vs_format_t *format = NULL;
	usbh_uvc_vs_frame_t *frame = NULL;

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
			break;
		}
	}

	if (format != NULL) {
		/*select matched frame */
		num_frame = format->nframes;
		for (i = 0; i < (int)num_frame; i ++) {

			frame = &format->frame[i];
			if (frame->bFrameIndex == ctrl->bFrameIndex) {
				break;
			}
		}

		stream->def_format = format;
		stream->cur_format = format;

		if (frame != NULL) {
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
	int i;
	int status;
	int retry_cnt = 0;

	usb_os_sema_create(&stream->frame_sema);

	/*init frame buffer*/
	INIT_LIST_HEAD(&stream->frame_chain);
	INIT_LIST_HEAD(&stream->frame_empty);
	stream->frame_buf = (u8 *)usb_os_malloc(USBH_UVC_VIDEO_MAX_FRAME * USBH_UVC_VIDEO_FRAME_SIZE);
	stream->frame_buffer_size = USBH_UVC_VIDEO_FRAME_SIZE;

	for (i = 0; i < USBH_UVC_VIDEO_MAX_FRAME; i++) {
		stream->frame_buffer[i].buf = stream->frame_buf + i * USBH_UVC_VIDEO_FRAME_SIZE;
		stream->frame_buffer[i].byteused = 0;
		INIT_LIST_HEAD(&stream->frame_buffer[i].list);
		list_add(&stream->frame_buffer[i].list, &stream->frame_empty);
	}

#if (USBH_UVC_USE_HW == 0)
	/* init decode thread */
	usbh_uvc_decode_thread_init(stream);
#endif

	/* only need commit video because probe/commit flow is finish when usbh_uvc_set_param */
	usbh_uvc_commit_video(stream);

	/* find current alt setting */
	usbh_uvc_find_alt(stream);

	/* set urb buffer */
	usbh_uvc_set_buf(stream);

	/* open pipe for current setting */
	usbh_open_pipe(uvc->host,
				   cur_setting->pipe,
				   cur_setting->ep_addr,
				   cur_setting->ep_type,
				   cur_setting->mps);

	usbh_set_toggle(uvc->host, cur_setting->pipe, 0);

	do {
		/* set alt setting for current interface */
		status = usbh_ctrl_set_interface(uvc->host, cur_setting->bInterfaceNumber, \
										 cur_setting->bAlternateSetting);
		if (status == HAL_OK) {
			break;
		} else {
			retry_cnt++;
			usb_os_sleep_ms(10);
		}
	} while (retry_cnt < 10);

	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Set alt err\n");
		return HAL_ERR_HW;
	}

#if (USBH_UVC_USE_HW == 1)
	stream->uvc_dec = usbh_uvc_dec_alloc_channel();
	rtos_sema_create_binary(&stream->uvc_dec->dec_sema);

	for (int i = 0; i < USBH_UVC_VIDEO_MAX_FRAME; i ++) {
		stream->uvc_dec->uvc_dec_buf[i].buf_start_addr = (u32)stream->frame_buffer[i].buf;
		stream->uvc_dec->uvc_dec_buf[i].buf_size = USBH_UVC_VIDEO_FRAME_SIZE;
	}

	stream->uvc_dec->dev_addr = usbh_get_dev_address();
	stream->uvc_dec->ep_num = cur_setting->ep_addr;
	stream->uvc_dec->binterval = cur_setting->interval;
	stream->uvc_dec->xfer_size = cur_setting->xfer_size;
	stream->uvc_dec->mps = cur_setting->mps;
	stream->uvc_dec->pipe_num = cur_setting->pipe;

	usbh_uvc_dec_init(stream->uvc_dec);
	usbh_uvc_dec_start(stream->uvc_dec);
#endif

#if ((USBH_UVC_USE_SOF == 0)&&(USBH_UVC_USE_HW == 0))
	usbh_isoc_receive_data(uvc->host,
						   stream->urb[stream->cur_urb]->addr + stream->urb[stream->cur_urb]->packet_info[stream->cur_packet].offset,
						   cur_setting->xfer_size,
						   cur_setting->pipe);
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
	int i;
	int status;
	int retry_cnt = 0;

#if USBH_UVC_USE_HW
	usbh_uvc_dec_stop(stream->uvc_dec);
	usbh_uvc_dec_deinit(stream->uvc_dec);
	usbh_uvc_dec_free_channel(stream->uvc_dec);
	rtos_sema_delete(stream->uvc_dec->dec_sema);
	stream->uvc_dec = NULL;
#endif

	do {
		/* reset interface */
		status = usbh_ctrl_set_interface(uvc->host, cur_setting->bInterfaceNumber, 0);
		if (status == HAL_OK) {
			break;
		} else {
			retry_cnt++;
			usb_os_sleep_ms(10);
		}
	} while (retry_cnt < 10);

	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Reset intf err\n");
	}

	/* close pipe */
	usbh_close_pipe(uvc->host, cur_setting->pipe);

	/* reset urb buffer */
	usbh_uvc_reset_buf(stream);

	/* delete decode task */
#if (USBH_UVC_USE_HW == 0)
	usbh_uvc_decode_thread_deinit(stream);
#endif

	INIT_LIST_HEAD(&stream->frame_chain);
	INIT_LIST_HEAD(&stream->frame_empty);

	for (i = 0; i < USBH_UVC_VIDEO_MAX_FRAME; i++) {
		INIT_LIST_HEAD(&stream->frame_buffer[i].list);
		stream->frame_buffer[i].byteused = 0;
		stream->frame_buffer[i].buf = NULL;
	}

	stream->cur_urb = 0;
	stream->last_fid = 0;
	stream->urb_buffer_size = 0;

	usb_os_sema_delete(stream->frame_sema);

	usb_os_mfree(stream->frame_buf);
}

