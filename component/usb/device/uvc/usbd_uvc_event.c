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
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/
/* VC entity type IDs */
#define UVC_VC_ITT_CAMERA       0x01
#define UVC_VC_PROCESS_UNIT     0x02
#define UVC_VC_EXTENSION_UNIT   0x03

/* Processing Unit (Brightness) control default values */
#define USBD_UVC_PU_BRIGHTNESS_CUR  100
#define USBD_UVC_PU_BRIGHTNESS_MAX  0xFF
#define USBD_UVC_PU_BRIGHTNESS_DEF  0x80
#define USBD_UVC_PU_BRIGHTNESS_INFO 0x0F
#define USBD_UVC_PU_BRIGHTNESS_LEN  2

/* Extension Unit control default values */
#define USBD_UVC_XU_DATA_CUR    0x1F00FF00
#define USBD_UVC_XU_DATA_MAX    0x1FFFFFFF
#define USBD_UVC_XU_DATA_INFO   0x03
#define USBD_UVC_XU_DATA_LEN    0x40

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static int usbd_uvc_send_response(usbd_uvc_dev_t *uvc, usbd_uvc_request_data_t *data);
static int usbd_uvc_receive_response(usbd_uvc_dev_t *uvc, usbd_uvc_req_data_t *uvc_event);
static void usbd_uvc_events_process_standard(usbd_uvc_dev_t *dev, usb_setup_req_t *ctrl, usbd_uvc_request_data_t *resp);
static void usbd_uvc_events_process_control(usbd_uvc_dev_t *dev, usb_setup_req_t *ctrl, usbd_uvc_request_data_t *resp);
static void usbd_uvc_dump_uvc_format(void);
static void usbd_uvc_fill_streaming_control(usbd_uvc_dev_t *dev, usbd_uvc_streaming_control_t *ctrl, int iframe, int iformat);
static void usbd_uvc_events_process_streaming(usbd_uvc_dev_t *dev, usb_setup_req_t *ctrl, usbd_uvc_request_data_t *resp);
static void usbd_uvc_events_process_class(usbd_uvc_dev_t *dev, usb_setup_req_t *ctrl, usbd_uvc_request_data_t *resp);
static void usbd_uvc_events_process_setup(usbd_uvc_dev_t *dev, usb_setup_req_t *ctrl, usbd_uvc_request_data_t *resp);
static void usbd_uvc_control_process_data(usbd_uvc_dev_t *dev, usbd_uvc_request_data_t *data);
static void usbd_uvc_streaming_process_data(usbd_uvc_dev_t *dev, usbd_uvc_request_data_t *data);
static void usbd_uvc_events_process_data(usbd_uvc_dev_t *dev, usbd_uvc_request_data_t *data);

usbd_uvc_process_unit_t p_data;
usbd_uvc_process_unit_t x_data;
static const char *const TAG = "UVC";

/**
  * @brief  Initialize UVC control parameters
  *         Initialize default/min/max/current values for
  *         Processing Unit and Extension Unit controls.
  * @retval 0 on success
  */
int usbd_uvc_parameter_init(void)
{
	p_data.cur = USBD_UVC_PU_BRIGHTNESS_CUR;
	p_data.min = 0;
	p_data.max = USBD_UVC_PU_BRIGHTNESS_MAX;
	p_data.res = 1;
	p_data.def = USBD_UVC_PU_BRIGHTNESS_DEF;
	p_data.info = USBD_UVC_PU_BRIGHTNESS_INFO;
	p_data.len = USBD_UVC_PU_BRIGHTNESS_LEN;

	x_data.cur = USBD_UVC_XU_DATA_CUR;
	x_data.min = 0;
	x_data.max = USBD_UVC_XU_DATA_MAX;
	x_data.res = 1;
	x_data.def = USBD_UVC_XU_DATA_CUR;
	x_data.info = USBD_UVC_XU_DATA_INFO;
	x_data.len = USBD_UVC_XU_DATA_LEN;
	return HAL_OK;
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
		usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)data->data, data->length);
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
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Standard request\n");
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
	if (dev->command_entity == UVC_VC_PROCESS_UNIT) {
		switch (ctrl->bRequest) {
		case USBD_UVC_SET_CUR:
			break;

		case USBD_UVC_GET_CUR:
			usb_os_memcpy((void *)resp->data, (const void *)&p_data.cur, ctrl->wLength);
			break;

		case USBD_UVC_GET_MIN:
			usb_os_memcpy((void *)resp->data, (const void *)&p_data.min, ctrl->wLength);
			break;
		case USBD_UVC_GET_MAX:
			usb_os_memcpy((void *)resp->data, (const void *)&p_data.max, ctrl->wLength);
			break;
		case USBD_UVC_GET_DEF:
			usb_os_memcpy((void *)resp->data, (const void *)&p_data.def, ctrl->wLength);
			break;

		case USBD_UVC_GET_RES:
			usb_os_memcpy((void *)resp->data, (const void *)&p_data.res, ctrl->wLength);
			break;

		case USBD_UVC_GET_LEN:
			usb_os_memcpy((void *)resp->data, (const void *)&p_data.len, ctrl->wLength);
			break;

		case USBD_UVC_GET_INFO:
			usb_os_memcpy((void *)resp->data, (const void *)&p_data.info, ctrl->wLength);
			break;
		}
	} else if (dev->command_entity == UVC_VC_EXTENSION_UNIT) {
		switch (ctrl->bRequest) {
		case USBD_UVC_SET_CUR:
			break;
		case USBD_UVC_GET_CUR:
			usb_os_memcpy((void *)resp->data, (const void *)&x_data.cur, ctrl->wLength);
			break;
		case USBD_UVC_GET_MIN:
			usb_os_memcpy((void *)resp->data, (const void *)&x_data.min, ctrl->wLength);
			break;
		case USBD_UVC_GET_MAX:
			usb_os_memcpy((void *)resp->data, (const void *)&x_data.max, ctrl->wLength);
			break;
		case USBD_UVC_GET_DEF:
			usb_os_memcpy((void *)resp->data, (const void *)&x_data.def, ctrl->wLength);
			break;
		case USBD_UVC_GET_RES:
			usb_os_memcpy((void *)resp->data, (const void *)&x_data.res, ctrl->wLength);
			break;
		case USBD_UVC_GET_LEN:
			usb_os_memcpy((void *)resp->data, (const void *)&x_data.len, ctrl->wLength);
			break;
		case USBD_UVC_GET_INFO:
			usb_os_memcpy((void *)resp->data, (const void *)&x_data.info, ctrl->wLength);
			break;
		}
	}
}
/**
  * @brief  Handle UVC Extension Unit GET/SET requests
  *         - Process class-specific control requests from host
  *         - Return current/min/max/default/resolution/length/info values
  *         - Data is copied from internal x_data structure
  * @param  dev: Pointer to UVC device context
  * @param  ctrl: USB setup request (contains bRequest type)
  * @param  resp: Response buffer to send back to host
  * @retval None
  * @note
  *         - This is a weak function and can be overridden by user
  *         - Only GET requests are handled here, SET_CUR is empty by default
  *         - Ensure resp->data buffer is large enough for copied data
  *         - x_data should be properly initialized before use
  */
__weak void get_command_extension_unit(usbd_uvc_dev_t *dev, usb_setup_req_t *ctrl, usbd_uvc_request_data_t *resp)
{
	(void)dev;
	switch (ctrl->bRequest) {
	case USBD_UVC_SET_CUR:
		break;
	case USBD_UVC_GET_CUR:
		usb_os_memcpy((void *)resp->data, (const void *)&x_data.cur, sizeof(int));
		break;
	case USBD_UVC_GET_MIN:
		usb_os_memcpy((void *)resp->data, (const void *)&x_data.min, sizeof(int));
		break;
	case USBD_UVC_GET_MAX:
		usb_os_memcpy((void *)resp->data, (const void *)&x_data.max, sizeof(int));
		break;
	case USBD_UVC_GET_DEF:
		usb_os_memcpy((void *)resp->data, (const void *)&x_data.def, sizeof(int));
		break;
	case USBD_UVC_GET_RES:
		usb_os_memcpy((void *)resp->data, (const void *)&x_data.res, sizeof(int));
		break;
	case USBD_UVC_GET_LEN:
		usb_os_memcpy((void *)resp->data, (const void *)&x_data.len, sizeof(int));
		break;
	case USBD_UVC_GET_INFO:
		usb_os_memcpy((void *)resp->data, (const void *)&x_data.info, sizeof(int));
		break;
	}
}

/**
  * @brief  Handle UVC Extension Unit SET request
  *         - Receive data from host and update internal parameters
  *         - Typically used for vendor-specific control commands
  * @param  dev: Pointer to UVC device context
  * @param  data: Received control data from host
  * @retval None
  * @note
  *         - This is a weak function and should be implemented by user
  *         - Called when host issues SET_CUR request
  *         - User should parse data->data and update corresponding parameters
  *         - Ensure proper validation to avoid invalid memory access
  */
__weak void set_command_extension_unit(usbd_uvc_dev_t *dev, usbd_uvc_request_data_t *data)
{
	(void)dev;
	(void)data;
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
	if (dev->command_entity == UVC_VC_PROCESS_UNIT) {
		usbd_uvc_get_command_process_unit(dev, ctrl, resp);
	} else if (dev->command_entity == UVC_VC_ITT_CAMERA) {
		//get_command_camera(dev, ctrl, resp);
	}
#if USBD_UVC_USE_EXTENSION_UNIT
	else if (dev->command_entity == UVC_VC_EXTENSION_UNIT) {
		get_command_extension_unit(dev, ctrl, resp);
	}
#endif
}
/**
  * @brief  Dump supported UVC frame intervals for debug
  * @retval None
  */
static void usbd_uvc_dump_uvc_format(void)
{
	const usbd_uvc_format_info_t *format = &uvcd_formats[0];
	int i = 0;

	for (i = 0; i < (int)USBD_UVC_FRAME_INTERVAL_CNT; i++) {
		if (format->frames->intervals[i] == 0U) {
			break;  /* 0 is the end-of-list sentinel, not a real interval */
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "FPS %d\n", format->frames->intervals[i]);
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
	u32 nframes;
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

	usb_os_memset(ctrl, 0U, (u32)sizeof(*ctrl));

	/* memset cleared dwClockFrequency; restore the real 48 MHz device clock so
	   GET_MIN/MAX/DEF report it (UVC 1.5 4.3.1.1). GET_CUR is unaffected as it
	   returns dev->probe/commit, which already carry USBD_UVC_CLOCK_FREQUENCY. */
	ctrl->dwClockFrequency = USBD_UVC_CLOCK_FREQUENCY;
	ctrl->bmHint = USBD_UVC_PROBE_BMHINT;
	ctrl->bFormatIndex = iformat + 1;
	ctrl->bFrameIndex = iframe + 1;
	ctrl->dwFrameInterval = frame->intervals[0];
	switch (format->fcc) {
	case USBD_UVC_FORMAT_TYPE_YUY2:
		ctrl->dwMaxVideoFrameSize = frame->width * frame->height * USBD_UVC_YUY2_BPP;
		break;
	case USBD_UVC_FORMAT_TYPE_NV12:
		ctrl->dwMaxVideoFrameSize = (frame->width * frame->height * USBD_UVC_NV12_SIZE_NUM) / USBD_UVC_NV12_SIZE_DEN;
		break;
	case USBD_UVC_FORMAT_TYPE_MJPEG:
		ctrl->dwMaxVideoFrameSize = frame->width * frame->height;
		break;
	case USBD_UVC_FORMAT_TYPE_H264:
		ctrl->dwMaxVideoFrameSize = frame->width * frame->height ;
		break;
	case USBD_UVC_FORMAT_TYPE_H265:
		ctrl->dwMaxVideoFrameSize = frame->width * frame->height;
		break;
	}
	ctrl->dwMaxPayloadTransferSize = USBD_UVC_IN_BUF_SIZE; /* FIX UVC1.5 4.3.1.1: GET_MIN/MAX/DEF path must also report valid max payload (=isoc mps 1024), not 0 */
	RTK_LOGS(TAG, RTK_LOG_INFO, "Type %d\n", format->fcc);
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
	u32 cs = ctrl->wValue >> 8;
	u32 req = ctrl->bRequest;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Streaming request req 0x%08x cs 0x%08x\n", req, cs);

	if (cs != USBD_UVC_VS_PROBE_CONTROL && cs != USBD_UVC_VS_COMMIT_CONTROL) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Unknown cs 0x%08x wValue 0x%08x\n", cs, ctrl->wValue);
		return;
	}

	ctrl_stream = (usbd_uvc_streaming_control_t *)&resp->data;
	resp->length = sizeof * ctrl_stream;
	resp->length = ctrl->wLength;
	switch (req) {
	case USBD_UVC_SET_CUR:
		dev->control = cs;
		break;

	case USBD_UVC_GET_CUR:
		if (cs == USBD_UVC_VS_PROBE_CONTROL) {
			usb_os_memcpy((void *)ctrl_stream, (const void *)&dev->probe, sizeof * ctrl_stream);
			RTK_LOGS(TAG, RTK_LOG_INFO, "probe\r\n");
		} else {
			usb_os_memcpy((void *)ctrl_stream, (const void *)&dev->commit, sizeof * ctrl_stream);
			RTK_LOGS(TAG, RTK_LOG_INFO, "commit\r\n");
		}
		break;

	case USBD_UVC_GET_MIN:
	case USBD_UVC_GET_MAX:
	case USBD_UVC_GET_DEF:
		/* Report the format/frame currently being negotiated (dev->probe), not a
		   hardcoded format 0. bFormatIndex/bFrameIndex are 1-based and already
		   range-validated in the SET_CUR path; fill takes 0-based indices. */
		usbd_uvc_fill_streaming_control(dev, ctrl_stream,
										(int)dev->probe.bFrameIndex - 1,
										(int)dev->probe.bFormatIndex - 1);
		break;

	case USBD_UVC_GET_RES:
		usb_os_memset(ctrl_stream, 0U, (u32)sizeof(*ctrl_stream));
		break;

	case USBD_UVC_GET_LEN:
		/* UVC 1.5 Table 4-51: GET_LEN returns the data length of the control (=sizeof control struct), not the host-requested wLength */
		resp->data[0] = (u8)(sizeof(usbd_uvc_streaming_control_t) & 0xFFU);
		resp->data[1] = (u8)((sizeof(usbd_uvc_streaming_control_t) >> 8) & 0xFFU);
		resp->length = USBD_UVC_GET_LEN_RESP_SIZE;
		break;

	case USBD_UVC_GET_INFO:
		resp->data[0] = USBD_UVC_GET_INFO_CAPS;
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

/**
  * @brief  Handle SET_CUR request for Processing Unit
  * @param  dev   UVC device context
  * @param  data  Received control data
  * @retval None
  */
void usbd_uvc_set_command_process_unit(usbd_uvc_dev_t *dev, usbd_uvc_request_data_t *data)
{
	(void)dev;
	(void)data;
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
#if USBD_UVC_USE_EXTENSION_UNIT
	case UVC_VC_EXTENSION_UNIT:
		set_command_extension_unit(dev, data);
		break;
#endif
	default:
		RTK_LOGS(TAG, RTK_LOG_INFO, "Setting unknown control, length = %d\n", data->length);
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
	usbd_uvc_format_t *fmt = dev->uvc_format_ptr;
	usbd_uvc_video_t *video = &dev->video;
	const u32 *interval;
	u32 iformat;
	u32 iframe;
	u32 nframes;

	switch (dev->control) {
	case USBD_UVC_VS_PROBE_CONTROL:
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Setting probe control, length = %d\n", data->length);
		target = &dev->probe;
		break;

	case USBD_UVC_VS_COMMIT_CONTROL:
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Setting commit control, length = %d\n", data->length);
		target = &dev->commit;
		break;
	default:
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Setting unknown control, length = %d\n", data->length);
		return;
	}

	ctrl = (usbd_uvc_streaming_control_t *)&data->data;
	iformat = ctrl->bFormatIndex;
	if (iformat < 1U || iformat > uvcd_nformats) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Invalid bFormatIndex %d\n", iformat);
		return;
	}

	format = &uvcd_formats[iformat - 1];
	nframes = 0;
	while (format->frames[nframes].width != 0) {
		++nframes;
	}

	iframe = ctrl->bFrameIndex;
	if (iframe < 1U || iframe > nframes) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Invalid bFrameIndex %d\n", iframe);
		return;
	}
	frame = &format->frames[iframe - 1];
	interval = frame->intervals;
	while (*(interval + 1U) != 0U && *interval < ctrl->dwFrameInterval) {
		++interval;
	}

	target->bFormatIndex = iformat;
	target->bFrameIndex = iframe;
	switch (format->fcc) {

	case USBD_UVC_FORMAT_TYPE_YUY2:
		target->dwMaxVideoFrameSize = frame->width * frame->height * USBD_UVC_YUY2_BPP;
		break;
	case USBD_UVC_FORMAT_TYPE_NV12:
		target->dwMaxVideoFrameSize = (frame->width * frame->height * USBD_UVC_NV12_SIZE_NUM) / USBD_UVC_NV12_SIZE_DEN;
		break;
	case USBD_UVC_FORMAT_TYPE_MJPEG:
		target->dwMaxVideoFrameSize = frame->width * frame->height;
		break;
	case USBD_UVC_FORMAT_TYPE_H264:
		target->dwMaxVideoFrameSize = frame->width *
									  frame->height;  /* FIX: was ctrl-> (host input buf); must write target(dev->probe/commit), else committed H264/H265 dwMaxVideoFrameSize stays 0 */
		break;
	case USBD_UVC_FORMAT_TYPE_H265:
		target->dwMaxVideoFrameSize = frame->width *
									  frame->height;  /* FIX: was ctrl-> (host input buf); must write target(dev->probe/commit), else committed H264/H265 dwMaxVideoFrameSize stays 0 */
		break;
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Format = %d w = %d h = %d fps = %d\n", format->fcc, frame->width, frame->height, *interval);

	fmt->format = format->fcc;
	fmt->width = frame->width;
	fmt->height = frame->height;
	fmt->fps = USBD_UVC_FRAME_INTERVAL_UNIT / (*interval);

	target->dwFrameInterval = *interval;
	/* HS high-bandwidth ISOC IN sends up to 3 x 1024 per microframe = one payload.
	   Must match the committed transfer size the device actually emits (was 1024,
	   inconsistent with the GET_MIN/MAX/DEF path which reports USBD_UVC_IN_BUF_SIZE). */
	target->dwMaxPayloadTransferSize = USBD_UVC_IN_BUF_SIZE;

	if (dev->control == USBD_UVC_VS_COMMIT_CONTROL) {
		video->fcc = format->fcc;
		video->width = frame->width;
		video->height = frame->height;
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
		usbd_uvc_streaming_process_data(dev, data);
		break;
	default:
		RTK_LOGS(TAG, RTK_LOG_INFO, "Setting unknown control, length = %d\n", data->length);
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

	usb_os_memset(&resp, 0U, (u32)sizeof(resp));

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

	if ((uvc_event->req.bmRequestType & USBD_UVC_BMREQTYPE_DIR_IN) != 0U) {
		ret = usbd_uvc_send_response(dev, &resp);
	} else {
		ret = usbd_uvc_receive_response(dev, uvc_event);
	}
	if (ret < 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "UVCIOC_S_EVENT failed\n");
		return;
	}
}
