/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usbd_uvc.h"
#include "usbd_video.h"
#include "usb_ch9.h"
#include <stdio.h>
#include <ctype.h>
static usbd_uvc_process_unit_t p_data;
static usbd_uvc_process_unit_t x_data;
static int uvc_format = 0;

#define ITT_CAMERA      0x01
#define PROCESS_UNIT    0X02
#define EXTENSION_UNIT  0X03
static const char *const TAG = "UVCD_EVENT";
/**
  * @brief  Initialize UVC control parameters
  *         Initialize default/min/max/current values for
  *         Processing Unit and Extension Unit controls.
  * @retval 0 on success
  */
int usbd_uvc_parameter_init(void)
{
	p_data.cur = 100;
	p_data.min = 0;
	p_data.max = 0xff;
	p_data.res = 1;
	p_data.def = 0x80;
	p_data.info = 0x0F;
	p_data.len = 0x02;

	x_data.cur = 0x1f00ff00;
	x_data.min = 0;
	x_data.max = 0x1fffffff;
	x_data.res = 1;
	x_data.def = 0x1f00ff00;
	x_data.info = 0x03; //0:SET 1:GET 2:Disalbe due to automatic 3:Autoupdate
	x_data.len = 0X40; //40
	return 0;
}

/**
  * @brief  Send control response data to host via EP0 IN
  * @param  uvc   UVC device context
  * @param  data  Response data to be sent
  * @retval 0 on success
  */
static int usbd_uvc_send_response(usbd_uvc_dev_t *uvc, usbd_uvc_request_data_t *data)
{
	int ret = 0;
	usbd_ep_t *ep0_in = &uvc->dev->ep0_in;
	if (data->length > 0) {
		memcpy(ep0_in->xfer_buf, data->data, data->length);
		ep0_in->xfer_len = data->length;
		usbd_ep_transmit(uvc->dev, ep0_in);
	}
	return ret;
}
/**
  * @brief  Prepare EP0 OUT to receive control data from host
  * @param  uvc        UVC device context
  * @param  uvc_event  UVC request event data
  * @retval 0 on success
  */
static int
usbd_uvc_receive_response(usbd_uvc_dev_t *uvc, usbd_uvc_req_data_t *uvc_event)
{
	int ret = 0;
	usbd_ep_t *ep0_out = &uvc->dev->ep0_out;
	if (uvc_event->req.wLength > 0) {
		ep0_out->xfer_len = uvc_event->req.wLength;
		usbd_ep_receive(uvc->dev, ep0_out);
	}
	return ret;
}
/**
  * @brief  Handle standard USB requests
  * @param  dev   UVC device context
  * @param  ctrl  USB setup request
  * @param  resp  Response buffer
  * @retval None
  */
static void
usbd_uvc_events_process_standard(usbd_uvc_dev_t *dev, usb_setup_req_t *
								 ctrl, usbd_uvc_request_data_t *resp)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "standard requestd\r\n");
	(void)dev;
	(void)ctrl;
	(void)resp;
}
/**
  * @brief  Handle GET requests for Processing Unit and Extension Unit
  * @param  dev   UVC device context
  * @param  ctrl  USB setup request
  * @param  resp  Response buffer
  * @retval None
  */
void usbd_uvc_get_command_process_unit(usbd_uvc_dev_t *dev, usb_setup_req_t *ctrl, usbd_uvc_request_data_t *resp)
{
	if (dev->command_entity == PROCESS_UNIT) {
		switch (ctrl->bRequest) {
		case USBD_UVC_SET_CUR:
			break;

		case USBD_UVC_GET_CUR:
			memcpy(resp->data, (unsigned char *)&p_data.cur, ctrl->wLength);
			break;

		case USBD_UVC_GET_MIN:
			memcpy(resp->data, (unsigned char *)&p_data.min, ctrl->wLength);
			break;
		case USBD_UVC_GET_MAX:
			memcpy(resp->data, (unsigned char *)&p_data.max, ctrl->wLength);
			break;
		case USBD_UVC_GET_DEF:
			memcpy(resp->data, (unsigned char *)&p_data.def, ctrl->wLength);
			break;

		case USBD_UVC_GET_RES:
			memcpy(resp->data, (unsigned char *)&p_data.res, ctrl->wLength);
			break;

		case USBD_UVC_GET_LEN:
			memcpy(resp->data, (unsigned char *)&p_data.len, ctrl->wLength);
			break;

		case USBD_UVC_GET_INFO:
			memcpy(resp->data, (unsigned char *)&p_data.info, ctrl->wLength);
			break;
		}
	} else if (dev->command_entity == EXTENSION_UNIT) {
		switch (ctrl->bRequest) {
		case USBD_UVC_SET_CUR:
			break;
		case USBD_UVC_GET_CUR:
			memcpy(resp->data, (unsigned char *)&x_data.cur, ctrl->wLength);
			break;
		case USBD_UVC_GET_MIN:
			memcpy(resp->data, (unsigned char *)&x_data.min, ctrl->wLength);
			break;
		case USBD_UVC_GET_MAX:
			memcpy(resp->data, (unsigned char *)&x_data.max, ctrl->wLength);
			break;
		case USBD_UVC_GET_DEF:
			memcpy(resp->data, (unsigned char *)&x_data.def, ctrl->wLength);
			break;
		case USBD_UVC_GET_RES:
			memcpy(resp->data, (unsigned char *)&x_data.res, ctrl->wLength);
			break;
		case USBD_UVC_GET_LEN:
			memcpy(resp->data, (unsigned char *)&x_data.len, ctrl->wLength);
			break;
		case USBD_UVC_GET_INFO:
			memcpy(resp->data, (unsigned char *)&x_data.info, ctrl->wLength);
			break;
		}
	}
}
/**
  * @brief  Handle UVC control interface requests
  *         Parse entity, selector, and request type,
  *         then dispatch to corresponding handler.
  * @param  dev   UVC device context
  * @param  ctrl  USB setup request
  * @param  resp  Response buffer
  * @retval None
  */
static void
usbd_uvc_events_process_control(usbd_uvc_dev_t *dev, usb_setup_req_t *ctrl,
								usbd_uvc_request_data_t *resp)
{
	resp->length = ctrl->wLength;
	dev->control = ctrl->wValue >> 8; //stream 0:control 1:stream ,control for selector ex:brightness
	dev->command_interface = ctrl->wIndex & 0xff; //0 for ocntrol 1 for streaming
	dev->command_entity = (ctrl->wIndex >> 8) & 0xff; //2 process unit 3 for extension unit
	if (dev->command_entity == PROCESS_UNIT) {
		usbd_uvc_get_command_process_unit(dev, ctrl, resp);
	} else if (dev->command_entity == ITT_CAMERA) {
		//get_command_camera(dev, ctrl, resp);
	} else if (dev->command_entity == EXTENSION_UNIT) {
		//get_command_extension_unit(dev, ctrl, resp);
	}
}
/**
  * @brief  Dump supported UVC frame intervals for debug
  * @retval None
  */
static void usbd_uvc_dump_uvc_format(void)
{
	const usbd_uvc_format_info_t *format;
	format = &uvcd_formats[0];
	int i = 0;
	for (i = 0; i < 4; i++) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "FPS %d\r\n", format->frames->intervals[i]);
	}
}
/**
  * @brief  Fill UVC streaming control structure
  *         Used for PROBE / COMMIT / GET_MIN / GET_MAX requests.
  * @param  dev     UVC device context
  * @param  ctrl    Streaming control structure
  * @param  iframe  Frame index
  * @param  iformat Format index
  * @retval None
  */
static void
usbd_uvc_fill_streaming_control(usbd_uvc_dev_t *dev,
								usbd_uvc_streaming_control_t *ctrl,
								int iframe, int iformat)
{
	const usbd_uvc_format_info_t *format;
	const usbd_uvc_frame_info_t *frame;
	unsigned int nframes;
	(void)dev;

	format = &uvcd_formats[iformat];
	usbd_uvc_dump_uvc_format();

	nframes = 0;
	while (format->frames[nframes].width != 0) {
		++nframes;
	}

	if (iframe < 0) {
		iframe = nframes + iframe;
	}
	if (iframe < 0 || iframe >= (int)nframes) {
		return;
	}
	frame = &format->frames[iframe];

	memset(ctrl, 0, sizeof * ctrl);

	ctrl->bmHint = 3;
	ctrl->bFormatIndex = iformat + 1;
	ctrl->bFrameIndex = iframe + 1;
	ctrl->dwFrameInterval = frame->intervals[0];
	switch (format->fcc) {
	case USBD_UVC_FORMAT_TYPE_YUY2:
		ctrl->dwMaxVideoFrameSize = frame->width * frame->height * 2;
		break;
	case USBD_UVC_FORMAT_TYPE_NV12:
		ctrl->dwMaxVideoFrameSize = (frame->width * frame->height * 3) / 2;
		break;
	case USBD_UVC_FORMAT_TYPE_MJPEG:
		ctrl->dwMaxVideoFrameSize = 460800;
		break;
	case USBD_UVC_FORMAT_TYPE_H264:
		ctrl->dwMaxVideoFrameSize = 150000;
		break;
	case USBD_UVC_FORMAT_TYPE_H265:
		ctrl->dwMaxVideoFrameSize = 150000;
		break;
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Type %d\r\n", format->fcc);
}
/**
  * @brief  Handle UVC streaming interface requests
  *         Process PROBE and COMMIT control requests.
  * @param  dev   UVC device context
  * @param  ctrl  USB setup request
  * @param  resp  Response buffer
  * @retval None
  */
static void
usbd_uvc_events_process_streaming(usbd_uvc_dev_t *dev, usb_setup_req_t *ctrl,
								  usbd_uvc_request_data_t *resp)
{
	usbd_uvc_streaming_control_t *ctrl_stream;
	unsigned int cs = ctrl->wValue >> 8;
	unsigned int req = ctrl->bRequest;

	RTK_LOGS(TAG, RTK_LOG_INFO, "streaming request (req %02x cs %02x)\n", req, cs);

	if (cs != USBD_UVC_VS_PROBE_CONTROL && cs != USBD_UVC_VS_COMMIT_CONTROL) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "cs = %x ctrl%x\r\n", cs, ctrl->wValue);
		return;
	}

	ctrl_stream = (usbd_uvc_streaming_control_t *)(void*)&resp->data;
	resp->length = sizeof * ctrl_stream;
	resp->length = ctrl->wLength;
	switch (req) {
	case USBD_UVC_SET_CUR:
		dev->control = cs;
		break;

	case USBD_UVC_GET_CUR:
		if (cs == USBD_UVC_VS_PROBE_CONTROL) {
			memcpy(ctrl_stream, &dev->probe, sizeof * ctrl_stream);
			RTK_LOGS(TAG, RTK_LOG_INFO, "probe\r\n");
		} else {
			memcpy(ctrl_stream, &dev->commit, sizeof * ctrl_stream);
			RTK_LOGS(TAG, RTK_LOG_INFO, "commit\r\n");
		}
		break;

	case USBD_UVC_GET_MIN:
	case USBD_UVC_GET_MAX:
	case USBD_UVC_GET_DEF:
		usbd_uvc_fill_streaming_control(dev, ctrl_stream, 0, 0); //Devin
		RTK_LOGS(TAG, RTK_LOG_INFO, "uvc_fill_streaming_control\r\n");
		break;

	case USBD_UVC_GET_RES:
		memset(ctrl, 0, sizeof * ctrl);
		break;

	case USBD_UVC_GET_LEN:
		resp->data[0] = 0x00;
		resp->data[1] = ctrl->wLength;
		resp->length = 2;
		break;

	case USBD_UVC_GET_INFO:
		resp->data[0] = 0x03;
		resp->length = 1;
		break;
	}
}
/**
  * @brief  Handle UVC class-specific requests
  * @param  dev   UVC device context
  * @param  ctrl  USB setup request
  * @param  resp  Response buffer
  * @retval None
  */
static void
usbd_uvc_events_process_class(usbd_uvc_dev_t *dev, usb_setup_req_t *ctrl,
							  usbd_uvc_request_data_t *resp)
{
	if ((ctrl->bmRequestType & USB_REQ_RECIPIENT_MASK) != USB_REQ_RECIPIENT_INTERFACE) {
		return;
	}
	dev->command_interface = ctrl->wIndex & 0xff; //0 for ocntrol 1 for streaming
	dev->command_entity = (ctrl->wIndex >> 8) & 0xff; //2 process unit 3 for extension unit
	dev->control = ctrl->wValue >> 8;

	switch (ctrl->wIndex & 0xff) {
	case USBD_UVC_INTF_CONTROL:
		usbd_uvc_events_process_control(dev, ctrl, resp);
		break;

	case USBD_UVC_INTF_STREAMING:
		usbd_uvc_events_process_streaming(dev, ctrl, resp);
		break;

	default:
		break;
	}
}
/**
  * @brief  Handle USB setup stage for UVC device
  * @param  dev   UVC device context
  * @param  ctrl  USB setup request
  * @param  resp  Response buffer
  * @retval None
  */
static void
usbd_uvc_events_process_setup(usbd_uvc_dev_t *dev, usb_setup_req_t *ctrl,
							  usbd_uvc_request_data_t *resp)
{
	dev->control = 0;

	switch (ctrl->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		usbd_uvc_events_process_standard(dev, ctrl, resp);
		break;

	case USB_REQ_TYPE_CLASS:
		usbd_uvc_events_process_class(dev, ctrl, resp);
		break;

	default:
		break;
	}
}


#define UVC_VC_ITT_CAMERA       0x01
#define UVC_VC_PROCESS_UNIT     0x02
#define UVC_VC_EXTENSION_UNIT   0x03
/**
  * @brief  Handle SET_CUR request for Processing Unit
  * @param  dev   UVC device context
  * @param  data  Received control data
  * @retval None
  */
void usbd_uvc_set_command_process_unit(usbd_uvc_dev_t *dev, usbd_uvc_request_data_t *data)
{
	(void)dev;
	RTK_LOGS(TAG, RTK_LOG_INFO, "set_command_process_unit %d %d\r\n", data->data[0], data->data[1]);
}
/**
  * @brief  Process control interface data stage
  * @param  dev   UVC device context
  * @param  data  Received control data
  * @retval None
  */
static void
usbd_uvc_control_process_data(usbd_uvc_dev_t *dev, usbd_uvc_request_data_t *data)
{
	switch (dev->command_entity) {
	case UVC_VC_ITT_CAMERA:
		//set_command_camera(dev, data);
		break;
	case UVC_VC_PROCESS_UNIT:
		usbd_uvc_set_command_process_unit(dev, data);
		break;
	case UVC_VC_EXTENSION_UNIT:
		//set_command_extension_unit(dev, data);
		break;
	default:
		RTK_LOGS(TAG, RTK_LOG_INFO, "setting unknown control, length = %d\n", data->length);
		return;
	}
}
/**
  * @brief  Process streaming control data stage
  *         Handle PROBE and COMMIT parameter negotiation.
  * @param  dev   UVC device context
  * @param  data  Received streaming control data
  * @retval None
  */
static void
usbd_uvc_streaming_process_data(usbd_uvc_dev_t *dev, usbd_uvc_request_data_t *data)
{
	usbd_uvc_streaming_control_t *target;
	usbd_uvc_streaming_control_t *ctrl;
	const usbd_uvc_format_info_t *format;
	const usbd_uvc_frame_info_t *frame;
	const unsigned int *interval;
	unsigned int iformat, iframe;
	unsigned int nframes;

	switch (dev->control) {
	case USBD_UVC_VS_PROBE_CONTROL:
		target = &dev->probe;
		break;

	case USBD_UVC_VS_COMMIT_CONTROL:
		RTK_LOGS(TAG, RTK_LOG_INFO, "setting commit control, length = %d\r\n", data->length);
		target = &dev->commit;
		break;
	default:
		RTK_LOGS(TAG, RTK_LOG_INFO, "setting unknown control, length = %d\n", data->length);
		return;
	}

	ctrl = (usbd_uvc_streaming_control_t *)(void*)&data->data;
	iformat = ctrl->bFormatIndex;

	format = &uvcd_formats[iformat - 1];
	nframes = 0;
	while (format->frames[nframes].width != 0) {
		++nframes;
	}

	iframe = ctrl->bFrameIndex;
	frame = &format->frames[iframe - 1];
	interval = frame->intervals;
	RTK_LOGS(TAG, RTK_LOG_INFO, "Interval[0] = %d\r\n", interval[0]);
	while (interval[0] < ctrl->dwFrameInterval && interval[1]) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "interval %lu\r\n", ctrl->dwFrameInterval);
		++interval;
	}

	target->bFormatIndex = iformat;
	target->bFrameIndex = iframe;
	switch (format->fcc) {

	case USBD_UVC_FORMAT_TYPE_YUY2:
		target->dwMaxVideoFrameSize = frame->width * frame->height * 2;
		break;
	case USBD_UVC_FORMAT_TYPE_NV12:
		target->dwMaxVideoFrameSize = (frame->width * frame->height * 3) / 2;
		break;
	case USBD_UVC_FORMAT_TYPE_MJPEG:
		target->dwMaxVideoFrameSize = 460800;
		break;
	case USBD_UVC_FORMAT_TYPE_H264:
		ctrl->dwMaxVideoFrameSize = 150000;
		break;
	case USBD_UVC_FORMAT_TYPE_H265:
		ctrl->dwMaxVideoFrameSize = 150000;
		break;
	}
	uvc_format = format->fcc;
	RTK_LOGS(TAG, RTK_LOG_INFO, "format = %d w = %d h = %d fps = %d\r\n", format->fcc, frame->width, frame->height, *interval);

	target->dwFrameInterval = *interval;

	if (dev->control == USBD_UVC_VS_COMMIT_CONTROL) {
		dev->video.fcc = format->fcc;
		dev->video.width = frame->width;
		dev->video.height = frame->height;
	}
}
/**
  * @brief  Dispatch received control data to control or streaming handler
  * @param  dev   UVC device context
  * @param  data  Received control data
  * @retval None
  */
static void
usbd_uvc_events_process_data(usbd_uvc_dev_t *dev, usbd_uvc_request_data_t *data)
{
	switch (dev->command_interface) {
	case USBD_UVC_INTF_CONTROL:
		usbd_uvc_control_process_data(dev, data);
		break;

	case USBD_UVC_INTF_STREAMING:
		RTK_LOGS(TAG, RTK_LOG_INFO, "setting commit control, length = %d\r\n", data->length);
		usbd_uvc_streaming_process_data(dev, data);
		break;
	default:
		RTK_LOGS(TAG, RTK_LOG_INFO, "setting unknown control, length = %d\n", data->length);
		return;
	}
}
/**
  * @brief  Main UVC event processing function
  *         Handle SETUP, DATA, STREAMON, and STREAMOFF events.
  * @param  dev        UVC device context
  * @param  uvc_event  UVC event data
  * @retval None
  */
void usbd_uvc_events_process(usbd_uvc_dev_t *dev, usbd_uvc_req_data_t *uvc_event)
{
	usbd_uvc_request_data_t resp;
	int ret;
	memset(&resp, 0, sizeof resp);


	switch (uvc_event->type) {
	case USBD_UVC_EVENT_CONNECT:
	case USBD_UVC_EVENT_DISCONNECT:
		return;

	case USBD_UVC_EVENT_SETUP:
		usbd_uvc_events_process_setup(dev, &uvc_event->req, &resp);
		break;

	case USBD_UVC_EVENT_DATA:
		usbd_uvc_events_process_data(dev, &uvc_event->uvc_data);
		return;

	case USBD_UVC_EVENT_STREAMON:
		break;

	case USBD_UVC_EVENT_STREAMOFF:
		break;
	}

	if (uvc_event->req.bmRequestType & 0x80U) {
		ret = usbd_uvc_send_response(dev, &resp);
	} else {
		ret = usbd_uvc_receive_response(dev, uvc_event);
	}
	if (ret < 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "UVCIOC_S_EVENT failed:\r\n");
		return;
	}
}
