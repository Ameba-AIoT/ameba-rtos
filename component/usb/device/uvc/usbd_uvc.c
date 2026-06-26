/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include "usbd.h"
#include "usbd_uvc.h"
#include "usbd_video.h"
/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static u16 usbd_uvc_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
static int usbd_uvc_set_config(usb_dev_t *dev, u8 config);
static int usbd_uvc_clear_config(usb_dev_t *dev, u8 config);
static int usbd_uvc_setup(usb_dev_t *dev, usb_setup_req_t *req);
static int usbd_uvc_handle_ep0_data_out(usb_dev_t *dev);
static int usbd_uvc_handle_ep0_data_in(usb_dev_t *dev, u8 status);
static int usbd_uvc_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_uvc_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static u8 usbd_uvc_set_interface(usb_dev_t *dev, u8 interface, u8 alt);

/* Private variables ---------------------------------------------------------*/

static usbd_uvc_dev_t usbd_uvc_dev;

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif


static void usbd_uvc_get_frame_handler(void *parm);


static const char *const TAG = "USBD_UVC";

static const u8 usbd_uvc_dev_desc[USB_LEN_DEV_DESC] USB_DMA_ALIGNED = {
	USB_LEN_DEV_DESC,                               /* bLength */
	USB_DESC_TYPE_DEVICE,                           /* bDescriptorType */
	0x00,                                           /* bcdUSB */
	0x02,
	0xef,                                           /* bDeviceClass */
	0x02,                                           /* bDeviceSubClass */
	0x01,                                           /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,                               /* bMaxPacketSize */
	USB_LOW_BYTE(USBD_UVC_WEBCAM_VENDOR_ID),                      /* idVendor */
	USB_HIGH_BYTE(USBD_UVC_WEBCAM_VENDOR_ID),
	USB_LOW_BYTE(USBD_UVC_WEBCAM_PRODUCT_ID),                      /* idProduct */
	USB_HIGH_BYTE(USBD_UVC_WEBCAM_PRODUCT_ID),
	USB_LOW_BYTE(USBD_UVC_WEBCAM_DEVICE_BCD),                    /* bcdDevice */
	USB_HIGH_BYTE(USBD_UVC_WEBCAM_DEVICE_BCD),
	USBD_IDX_MFC_STR,                               /* iManufacturer */
	USBD_IDX_PRODUCT_STR,                           /* iProduct */
	USBD_IDX_SERIAL_STR,                            /* iSerialNumber */
	0x01                                            /* bNumConfigurations */
};

/* USB Standard String Descriptor 0 */
static const u8 usbd_uvc_lang_id_desc[USB_LEN_LANGID_STR_DESC] USB_DMA_ALIGNED = {
	USB_LEN_LANGID_STR_DESC,                        /* bLength */
	USB_DESC_TYPE_STRING,                           /* bDescriptorType */
	USB_LOW_BYTE(USBD_UVC_LANGID_STRING),           /* wLANGID */
	USB_HIGH_BYTE(USBD_UVC_LANGID_STRING),
};  /* usbd_uvc_lang_id_desc */

#ifndef CONFIG_USB_FS
/* USB Standard Device Qualifier Descriptor */
static const u8 usbd_uvc_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] USB_DMA_ALIGNED = {
	USB_LEN_DEV_QUALIFIER_DESC,                     /* bLength */
	USB_DESC_TYPE_DEVICE_QUALIFIER,                 /* bDescriptorType */
	0x00,                                           /* bcdUSB */
	0x02,
	0x00,                                           /* bDeviceClass */
	0x00,                                           /* bDeviceSubClass */
	0x00,                                           /* bDeviceProtocol */
	0x40,                                           /* bMaxPacketSize */
	0x01,                                           /* bNumConfigurations */
	0x00,                                           /* Reserved */
};  /* usbd_uvc_device_qualifier_desc */
#endif

/* UVC Class Driver */
usbd_class_driver_t usbd_uvc_driver = {
	.get_descriptor = usbd_uvc_get_descriptor,
	.set_config = usbd_uvc_set_config,
	.clear_config = usbd_uvc_clear_config,
	.setup = usbd_uvc_setup,
	.ep0_data_out = usbd_uvc_handle_ep0_data_out,
	.ep0_data_in = usbd_uvc_handle_ep0_data_in,
	.ep_data_in =  usbd_uvc_handle_ep_data_in,
	.ep_data_out = usbd_uvc_handle_ep_data_out,
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initialize UVC command queue
  * @retval None
  */
void usbd_uvc_cmd_queue_init(void)
{
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	if (rtos_queue_create(&cdev->uvc_cmd_queue, 8, sizeof(usbd_uvc_req_data_t)) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Queue create failed\r\n");
		return;
	}
}
/**
  * @brief  UVC command handler task
  *         Process UVC control and event messages from queue
  * @param  parm Task parameter (unused)
  * @retval None
  */
void usbd_uvc_cmd_handler(void *parm)
{
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	usbd_uvc_req_data_t req_data;
	(void)parm;

	while (1) {
		if (rtos_queue_receive(cdev->uvc_cmd_queue, &req_data, 0xffffffff) == RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Receive type=%d len=%d\n", req_data.type, req_data.uvc_data.length);
		}
		usbd_uvc_events_process(cdev, &req_data);
	}
	rtos_task_delete(NULL);
}

/**
  * @brief  Encode UVC payload header for video streaming
  * @param  video  UVC video context
  * @param  buf    UVC buffer containing video data
  * @param  data   USB transmit buffer
  * @param  len    Available buffer length
  * @retval Header length in bytes
  */
static int
usbd_uvc_video_encode_header(usbd_uvc_video_t *video, usbd_uvc_buffer_t *buf, u8 *data, u32 len)
{
	data[0] = 2;
	data[1] = USBD_UVC_STREAM_EOH | video->fid;

	if (buf->bytesused - video->buf_used <= len - 2) {
		data[1] |= USBD_UVC_STREAM_EOF;
	}

	return 2;
}
/**
  * @brief  Copy video payload data into USB buffer
  * @param  video  UVC video context
  * @param  buf    Video frame buffer
  * @param  data   USB transmit buffer
  * @param  len    Maximum copy length
  * @retval Number of bytes copied
  */
static int
usbd_uvc_video_encode_data(usbd_uvc_video_t *video, usbd_uvc_buffer_t *buf, u8 *data, int len)
{
	u32 nbytes;
	void *mem;

	/* Copy video data to the USB buffer. */
	mem = buf->mem + video->buf_used;
	nbytes = min((u32)len, buf->bytesused - video->buf_used);
	memcpy(data, mem, nbytes);
	video->buf_used += nbytes;
	return nbytes;
}

/**
  * @brief  Handle isochronous video streaming transmission
  *         Prepare UVC payload and submit USB transfer
  * @param  dev    USB device instance
  * @param  video  UVC video context
  * @param  buf    Current video buffer
  * @retval None
  */
static void
usbd_uvc_video_encode_isoc(usb_dev_t *dev, usbd_uvc_video_t *video, usbd_uvc_buffer_t *buf)
{
	u8 *mem ;
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	u32 len = USBD_UVC_IN_BUF_SIZE;
	int ret;
	usbd_uvc_dev_t *uvc_ctx = &usbd_uvc_dev;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "encode_isoc: buf=%p, mem=%p, bytesused=%u, buf_used=%u\n",
			 buf, (buf ? buf->mem : NULL), (buf ? buf->bytesused : 0), video->buf_used);

	if (buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "encode_isoc: buf is NULL!\n");
		return;
	}
	if (buf->mem == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "encode_isoc: buf->mem is NULL! bytesused=%u\n", buf->bytesused);
		return;
	}
	if (buf->bytesused == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "encode_isoc: buf->bytesused is 0!\n");
		return;
	}

	if (uvc_ctx->frame_done) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "encode_isoc: frame_done, return buffer\n");
		uvc_ctx->frame_done = 0;
		usb_os_sema_give(usbd_uvc_dev.video.output_frame_sema);
		return;
	}

	mem = cdev->uvc_in_buf;
	video->req_size = USBD_UVC_IN_BUF_SIZE;

	ret = usbd_uvc_video_encode_header(video, buf, mem, len);
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "encode header: ret=%d, len=%d\n", ret, len);
	mem += ret;
	len -= ret;

	/* Process video data. */
	ret = usbd_uvc_video_encode_data(video, buf, mem, len);
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "encode data: ret=%d, len=%d\n", ret, len);

	len -= ret;

	mem -= 2;


	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	ep_isoc_in->xfer_len = video->req_size - len;
	ep_isoc_in->xfer_buf = cdev->uvc_in_buf;
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "iso transmit: xfer_len=%u, frame_done=%d\n",
			 ep_isoc_in->xfer_len, (buf->bytesused == video->buf_used));
	usbd_ep_transmit(dev, ep_isoc_in);

	if (buf->bytesused == video->buf_used) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "encode_isoc: frame complete! bytesused=%u\n", buf->bytesused);
		video->buf_used = 0;
		uvc_ctx->frame_done = 1;
		video->fid ^= USBD_UVC_STREAM_FID;
	}
}

usbd_uvc_buffer_t *usbd_uvc_video_in_stream_queue(usbd_uvc_dev_t *uvc_ctx);
void usbd_uvc_video_put_out_stream_queue(usbd_uvc_buffer_t *payload);
/**
  * @brief  Free UVC buffer and return it to output queue
  * @param  video UVC video context
  * @retval None
  */
void usbd_uvc_free_uvcd_list_buffer(usbd_uvc_video_t *video)
{
	if (list_empty(&video->output_queue)) {
		usb_os_lock(video->output_lock);
		list_add_tail(&video->uvc_buffer.buffer_list, &video->output_queue);
		usb_os_unlock(video->output_lock);
		usb_os_sema_give(usbd_uvc_dev.video.output_queue_sema);
		usb_os_sema_give(usbd_uvc_dev.video.output_frame_sema);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Free the buffer\r\n");
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "No buffer to free\r\n");
	}
}
/**
  * @brief  Put buffer back to output queue
  *         Used when frame transmission is completed
  * @param  payload UVC buffer
  * @retval None
  */
void usbd_uvc_video_put_out_stream_queue(usbd_uvc_buffer_t *payload)
{
	usbd_uvc_dev_t *uvc_ctx = &usbd_uvc_dev;
	usb_os_lock(uvc_ctx->video.output_lock);
	list_add_tail(&payload->buffer_list, &uvc_ctx->video.output_queue);
	usb_os_unlock(uvc_ctx->video.output_lock);
}
/**
  * @brief  Put buffer into input queue
  *         Buffer contains new video frame data
  * @param  payload UVC buffer
  * @retval None
  */
void usbd_uvc_video_put_in_stream_queue(usbd_uvc_buffer_t *payload)
{
	usbd_uvc_dev_t *uvc_ctx = &usbd_uvc_dev;
	usb_os_lock(uvc_ctx->video.input_lock);
	list_add_tail(&payload->buffer_list, &uvc_ctx->video.input_queue);
	usb_os_unlock(uvc_ctx->video.input_lock);
}
/**
  * @brief  Get buffer from output queue
  * @retval Pointer to UVC buffer or NULL if empty
  */
usbd_uvc_buffer_t *usbd_uvc_video_out_stream_queue(void)
{
	usbd_uvc_buffer_t *payload = NULL;
	usbd_uvc_dev_t *uvc_ctx = &usbd_uvc_dev;
	if (!list_empty(&uvc_ctx->video.output_queue)) {
		usb_os_lock(uvc_ctx->video.output_lock);
		payload = list_first_entry(&uvc_ctx->video.output_queue, usbd_uvc_buffer_t, buffer_list);
		uvc_ctx->video.uvc_buffer = *payload;
		list_del_init(&payload->buffer_list);
		usb_os_unlock(uvc_ctx->video.output_lock);
	}
	return payload;
}

/**
  * @brief  Get one video buffer from input queue
  * @param  uvc_ctx UVC device context
  * @retval Pointer to UVC buffer or NULL if empty
  */
usbd_uvc_buffer_t *usbd_uvc_video_in_stream_queue(usbd_uvc_dev_t *uvc_ctx)
{
	usbd_uvc_buffer_t *payload = NULL;
	if (!list_empty(&uvc_ctx->video.input_queue)) {
		usb_os_lock(uvc_ctx->video.input_lock);
		payload = list_first_entry(&uvc_ctx->video.input_queue, usbd_uvc_buffer_t, buffer_list);
		uvc_ctx->video.uvc_buffer = *payload;
		list_del_init(&payload->buffer_list);
		usb_os_unlock(uvc_ctx->video.input_lock);
	}
	return payload;
}
/**
  * @brief  Wait until current UVC video frame transmission is completed
  *         Block the caller until the output frame semaphore is released,
  *         indicating that one video frame has been fully sent.
  * @retval None
  */
void usbd_uvc_wait_frame_down(void)
{
	usbd_uvc_dev_t *uvc_ctx = &usbd_uvc_dev;
	usb_os_sema_take(uvc_ctx->video.output_frame_sema, RTOS_MAX_DELAY);
	usbd_uvc_video_put_out_stream_queue(&uvc_ctx->video.uvc_buffer);
	usb_os_sema_give(usbd_uvc_dev.video.output_queue_sema);
}
/**
  * @brief  Get private UVC device context
  *         Return internal UVC device structure pointer
  * @retval Pointer to UVC device context
  */
usbd_uvc_dev_t *get_private_usbd_uvcd(void)
{
	return &usbd_uvc_dev;
}
/**
  * @brief  Get UVC running status
  * @retval 1 if UVC streaming is running, otherwise 0
  */
int usbd_uvc_get_status(void)
{
	return usbd_uvc_dev.running;
}
/**
  * @brief  Handle EP0 OUT data stage (Control OUT)
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  *         Receive class-specific control data from host
  * @param  dev USB device instance
  * @retval HAL status
  */
static int usbd_uvc_handle_ep0_data_out(usb_dev_t *dev)
{
	int ret = HAL_OK;
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	usbd_uvc_req_data_t req_data;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	req_data.type = USBD_UVC_EVENT_DATA;
	DCache_Invalidate((u32)ep0_out->xfer_buf, cdev->ctrl_data_len);
	memcpy(req_data.uvc_data.data, ep0_out->xfer_buf, cdev->ctrl_data_len);
	req_data.uvc_data.length = cdev->ctrl_data_len;
	usbd_uvc_events_process(cdev, &req_data);

	return ret;
}
/**
  * @brief  Handle EP0 IN data stage
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev USB device instance
  * @param  status Transfer status
  * @retval HAL status
  */
static int usbd_uvc_handle_ep0_data_in(usb_dev_t *dev, u8 status)
{
	(void)dev;
	(void)status;
	return 0;
}
/**
  * @brief  Handle isochronous IN endpoint transfer complete
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  *         Trigger next video packet transmission
  * @param  dev     USB device instance
  * @param  ep_addr Endpoint address
  * @param  status  Transfer status
  * @retval HAL status
  */
static int usbd_uvc_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	int ret = HAL_OK;
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;

	// Check if UVC is initialized
	if (!cdev->init_done) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ep_data_in: UVC not initialized! init_done=%d\n", cdev->init_done);
		return ret;
	}

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "ep_data_in: ep=0x%02X, status=%d, running=%d\n",
			 ep_addr, status, cdev->running);

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "ep_data_in: uvc_buffer ptr=%p, mem=%p, bytesused=%u, buf_used=%u\n",
			 &cdev->video.uvc_buffer,
			 (cdev->video.uvc_buffer.mem ? cdev->video.uvc_buffer.mem : NULL),
			 cdev->video.uvc_buffer.bytesused,
			 cdev->video.buf_used);

	if (cdev->running == 0) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "ep_data_in: not running, skip\n");
		return ret;
	}

	usbd_uvc_video_encode_isoc(dev, &cdev->video, &cdev->video.uvc_buffer);

	return ret;
}
/**
  * @brief  Handle USB data OUT endpoint callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  *         This function is called when data is received from host
  *         on a non-control OUT endpoint.
  *         (Currently not used for UVC video streaming)
  * @param  dev     USB device instance
  * @param  ep_addr Endpoint address
  * @param  len     Length of received data
  * @retval HAL status
  */
static int usbd_uvc_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len)
{
	int ret = HAL_OK;
	(void)ep_addr;
	(void)len;
	(void)dev;
	return ret;
}
/**
  * @brief  Get USB descriptor callback for UVC device
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev   USB device instance
  * @param  req   USB setup request
  * @param  buf   Buffer to store descriptor data
  * @retval Length of descriptor returned
  */
static u16 usbd_uvc_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "%s %d %x\r\n", __FUNCTION__, __LINE__, (req->wValue >> 8) & 0xFF);
	u16 len = 0;
	usb_speed_type_t speed = dev->dev_speed;
	u8 *desc = NULL;
	switch ((req->wValue >> 8) & 0xFF) {

	case USB_DESC_TYPE_DEVICE:
		RTK_LOGS(TAG, RTK_LOG_INFO, "Get descriptor USB_DESC_TYPE_DEVICE\n");
		len = sizeof(usbd_uvc_dev_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_uvc_dev_desc, len);

		break;

	case USB_DESC_TYPE_CONFIGURATION:
		RTK_LOGS(TAG, RTK_LOG_INFO, "Get descriptor USB_DESC_TYPE_CONFIGURATION\n");

		desc = (u8 *)usbd_uvc_descriptors;
		len  = usbd_uvc_descriptors_size;

		RTK_LOGS(TAG, RTK_LOG_INFO, "desc_self %p len %d\r\n", desc, len);
		usb_os_memcpy((void *)buf, (void *)desc, len);
		break;

#ifndef CONFIG_USB_FS
	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		RTK_LOGS(TAG, RTK_LOG_INFO, "Get descriptor USB_DESC_TYPE_DEVICE_QUALIFIER\n");

		len = sizeof(usbd_uvc_device_qualifier_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_uvc_device_qualifier_desc, len);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		RTK_LOGS(TAG, RTK_LOG_INFO, "Get descriptor USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION\n");

		desc = (u8 *)usbd_uvc_descriptors;
		len  = usbd_uvc_descriptors_size;

		RTK_LOGS(TAG, RTK_LOG_INFO, "Use the array for uvc descriptors\r\n");

		usb_os_memcpy((void *)buf, (void *)desc, len);
		break;
#endif

	case USB_DESC_TYPE_STRING:
		switch (req->wValue & 0xFF) {
		case USBD_IDX_LANGID_STR:
			RTK_LOGS(TAG, RTK_LOG_INFO, "Get descriptor USBD_IDX_LANGID_STR\n");

			len = sizeof(usbd_uvc_lang_id_desc);
			usb_os_memcpy((void *)buf, (void *)usbd_uvc_lang_id_desc, len);
			break;
		case USBD_IDX_MFC_STR:
			RTK_LOGS(TAG, RTK_LOG_INFO, "Get descriptor USBD_IDX_MFC_STR\n");

			len = usbd_get_str_desc(USBD_UVC_MFG_STRING, buf);
			break;
		case USBD_IDX_PRODUCT_STR:
			RTK_LOGS(TAG, RTK_LOG_INFO, "Get descriptor USBD_IDX_PRODUCT_STR\n");

			if (speed == USB_SPEED_HIGH) {
				len = usbd_get_str_desc(USBD_UVC_MFG_HS_STRING, buf);
			} else {
				len = usbd_get_str_desc(USBD_UVC_MFG_FS_STRING, buf);
			}
			break;
		case USBD_IDX_SERIAL_STR:
			RTK_LOGS(TAG, RTK_LOG_INFO, "Get descriptor USBD_IDX_SERIAL_STR\n");

			len = usbd_get_str_desc(USBD_UVC_SN_STRING, buf);
			break;
		default:
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Get descriptor failed, invalid string index %d\n", req->wValue & 0xFF);

			break;
		}
		break;

	default:
		break;
	}

	/* return buf; */
	return len;
}

/**
  * @brief  Set UVC device configuration
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  *         Initialize isochronous IN endpoint for video streaming
  * @param  dev     USB device instance
  * @param  config  Configuration index
  * @retval HAL status
  */
static int usbd_uvc_set_config(usb_dev_t *dev, u8 config)
{
	int ret = HAL_OK;
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	cdev->dev = dev;
	RTK_LOGS(TAG, RTK_LOG_INFO, "%s %d %d\r\n", __FUNCTION__, __LINE__, config);
	/* Init ISOC IN EP */
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usb_ep_info_t *info = &ep_isoc_in->info;

	info->mps = 1024;
	info->binterval = 1;
	usbd_ep_init(dev, ep_isoc_in);
	return ret;
}

/**
  * @brief  Clear UVC device configuration
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  *         De-initialize isochronous endpoint
  * @param  dev     USB device instance
  * @param  config  Configuration index
  * @retval HAL status
  */
static int usbd_uvc_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = 0;//HAL_OK;
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	usbd_ep_t *ep_bulk_in = &cdev->ep_isoc_in;
	usbd_ep_deinit(dev, ep_bulk_in);
	RTK_LOGS(TAG, RTK_LOG_INFO, "%s %d %d\r\n", __FUNCTION__, __LINE__, config);
	return ret;
}
/**
  * @brief  Handle USB setup requests for UVC class and standard requests
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev  USB device instance
  * @param  req  USB setup request
  * @retval HAL status
  */
static int usbd_uvc_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	int ret = HAL_OK;
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	usbd_ep_t *ep0_in = &dev->ep0_in;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "usbd_uvc_setup bmRequestType=0x%02X bRequest=0x%02X wLength=0x%04X wValue=%x wIndex %x\n",
			 req->bmRequestType,
			 req->bRequest,
			 req->wLength,
			 req->wValue,
			 req->wIndex
			);

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state != USBD_STATE_CONFIGURED) {
				ret = HAL_ERR_PARA;
			} else {
				usbd_uvc_set_interface(dev, req->wIndex, req->wValue);
			}
			RTK_LOGS(TAG, RTK_LOG_INFO, "USB_REQ_SET_INTERFACE\r\n");
			break;
		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_len = 1U;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_PARA;
			}
			RTK_LOGS(TAG, RTK_LOG_INFO, "USB_REQ_GET_INTERFACE\r\n");
			break;
		case USB_REQ_GET_STATUS:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_PARA;
			}
			RTK_LOGS(TAG, RTK_LOG_INFO, "USB_REQ_GET_STATUS\r\n");
			break;
		}
		break;
	case USB_REQ_TYPE_CLASS :
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "USB_REQ_TYPE_CLASS\r\n");
		usbd_uvc_req_data_t req_data;
		memset(&req_data, 0x00, sizeof(req_data));
		req_data.type = USBD_UVC_EVENT_SETUP;
		memcpy(&req_data.req, req, sizeof(req_data.req));
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "USB_REQ_TYPE_CLASS\r\n");
		if (req->bmRequestType & 0x80U) {

		} else {
			cdev->ctrl_req = req->bRequest;
			cdev->ctrl_data_len = req->wLength;
		}
		if (req->bmRequestType == 0x21) {
			usbd_ep_t *ep0_out = &dev->ep0_out;
			ep0_out->xfer_len = req->wLength;
			usbd_ep_receive(dev, ep0_out);
		} else {
			//usbd_uvc_events_process(cdev, &req_data);
			//rtos_queue_send(cdev->uvc_cmd_queue, &req_data, 0);
		}
		usbd_uvc_events_process(cdev, &req_data);
		break;
	default:
		ret = HAL_ERR_HW;
		break;
	}
	return ret;
}
/**
  * @brief  Handle SET_INTERFACE request for UVC streaming
  *         - Alt 1: Start video streaming
  *         - Alt 0: Stop video streaming and release buffers
  * @param  dev       USB device instance
  * @param  interface Interface number
  * @param  alt       Alternate setting value
  * @retval Status
  */
static u8 usbd_uvc_set_interface(usb_dev_t *dev, u8 interface, u8 alt)
{
	usbd_uvc_dev_t *cdev = &usbd_uvc_dev;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usb_ep_info_t *info = &ep_isoc_in->info;
	// Check if UVC is properly initialized
	if (!cdev->init_done) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "set_interface: UVC not initialized! init_done=%d\n", cdev->init_done);
		return 1;
	}

	if (interface == 1 && alt == 1) {
		if (cdev->running == 0) {
			cdev->frame_done = 0;
			info->mps = 1024;
			info->binterval = 1;
			usbd_ep_init(dev, ep_isoc_in);
			RTK_LOGS(TAG, RTK_LOG_INFO, "start to send the frame\r\n");
			cdev->running = 1;
			usb_os_sema_give(usbd_uvc_dev.video.output_queue_sema);
			if (usbd_uvc_dev.change_parm_cb) {
				cdev->change_parm_cb(cdev->uvc_format_ptr);
			}
		}
	} else if (interface == 1 && alt == 0) {
		if (cdev->running) {
			cdev->running = 0;
			usbd_ep_deinit(dev, ep_isoc_in);

			if (list_empty(&cdev->video.output_queue)) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "output queue empty\r\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "output queue full\r\n");
			}

			if (list_empty(&cdev->video.input_queue)) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "input queue empty\r\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "input queue full\r\n");
			}
			usbd_uvc_free_uvcd_list_buffer(&cdev->video);
			cdev->video.uvc_buffer.bytesused = 0;//For get the new data
			cdev->video.buf_used = 0;//reset the buffer offset

			usb_os_sema_give(usbd_uvc_dev.video.output_frame_sema);
			cdev->frame_done = 0;
		}
		cdev->running = 0;
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "interface %d alt %d\r\n", interface, alt);
	return 0;
}

/**
  * @brief  UVC video transmit handler task
  *         This task retrieves prepared video buffers from the input queue
  *         and sends them to host via isochronous IN endpoint.
  * @param  parm Task parameter (unused)
  * @retval None
  */
static void usbd_uvc_get_frame_handler(void *parm)
{
	(void)parm;

	usbd_uvc_buffer_t *payload = NULL;
	RTK_LOGS(TAG, RTK_LOG_INFO, "usbd_uvcd_get_frame_handler started\r\n");
	while (1) {
		usbd_uvc_dev_t *cdev = &usbd_uvc_dev;

		// Check if UVC is still initialized
		if (!cdev->init_done) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "get_frame: UVC not initialized, exit\n");
			break;
		}

		RTK_LOGS(TAG, RTK_LOG_DEBUG, "get_frame: wait sema, running=%d\n", cdev->running);
		usb_os_sema_take(cdev->video.output_queue_sema, RTOS_MAX_DELAY);
		if (usbd_uvc_get_status() == 0) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "get_frame: not running, continue\n");
			continue;
		}
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "get_frame: try to get payload from queue\n");
		for (int i = 0; i < 100000; i++) {
			payload = usbd_uvc_video_in_stream_queue(cdev);
			if (payload) {
				cdev->video.uvc_buffer = *payload;
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "get_frame: got payload %p, mem=%p, bytesused=%u\n",
						 payload, payload->mem, payload->bytesused);
				break;
			}
			rtos_time_delay_ms(1);
			if ((cdev->running == 0) && (i > 30)) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "get_frame: timeout, running=%d, i=%d\n", cdev->running, i);
				break;
			}
		}

		// Skip if no valid payload from queue
		if (payload == NULL) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "get_frame: no payload, skip frame\n");
			continue;
		}

		RTK_LOGS(TAG, RTK_LOG_DEBUG, "get_frame: call encode, uvc_buffer mem=%p bytesused=%u\n",
				 cdev->video.uvc_buffer.mem, cdev->video.uvc_buffer.bytesused);
		usbd_uvc_video_encode_isoc(cdev->dev, &cdev->video, &cdev->video.uvc_buffer);
	}
	rtos_task_delete(NULL);
}

/**
  * @brief  Init of UVC Extension Handle
  */
__weak void usbd_ext_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "initialization of extension unit handle\r\n");
}

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Initialize USB UVC device and related resources
  *         - Initialize USB device stack
  *         - Register UVC class driver
  *         - Create queues, semaphores, and tasks
  *         - Prepare video streaming environment
  * @retval HAL_OK if success, otherwise error code
  */
int usbd_uvc_init(void)
{
	int ret = HAL_OK;
	usbd_uvc_dev_t *dev = &usbd_uvc_dev;
	usb_ep_info_t *info = &dev->ep_isoc_in.info;

	usbd_uvc_parameter_init();
	usbd_ext_init();
	memset(dev, 0, sizeof(usbd_uvc_dev_t));
	dev->probe = usbd_uvc_probe;
	dev->commit = usbd_uvc_commit;

	usbd_uvc_cmd_queue_init();

	usbd_uvc_probe.bFormatIndex = 1;
	usbd_uvc_probe.bFrameIndex = 1;
	usbd_uvc_commit.bFormatIndex = 1;
	usbd_uvc_commit.bFrameIndex = 1;

	INIT_LIST_HEAD(&dev->video.input_queue);
	INIT_LIST_HEAD(&dev->video.output_queue);
	usb_os_lock_create(&dev->video.input_lock);
	usb_os_lock_create(&dev->video.output_lock);

	INIT_LIST_HEAD(&dev->bod_list);
	usb_os_lock_create(&dev->bod_mutex);


	usb_os_lock_create(&dev->lock);

	usb_os_sema_create(&dev->uvc_Cmd_wakeup_sema);
	usb_os_sema_create(&dev->video.output_queue_sema);
	usb_os_sema_create(&dev->video.output_frame_sema);

	usb_os_queue_create(&dev->video.complete_bf_req, 10, sizeof(int));

	info->addr = USBD_UVC_ISO_IN_EP;
	info->type = USB_CH_EP_TYPE_ISOC;
	info->binterval = 1;

	usbd_uvc_dev.uvc_format_ptr = malloc(sizeof(usbd_uvc_format_t));
	memset(usbd_uvc_dev.uvc_format_ptr, 0, sizeof(usbd_uvc_format_t));
	usb_os_sema_create(&usbd_uvc_dev.uvc_format_ptr->uvcd_change_sema);

	// Mark as initialized before creating tasks, so tasks can run properly
	usbd_uvc_dev.init_done = 1;

	ret = rtos_task_create(NULL, "usbd_uvc_cmd_handler", usbd_uvc_cmd_handler, NULL, 1024U, 5);
	if (ret != SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Create USBD USBD_UVC CMD thread fail\n", __FUNCTION__);
		ret = -1;
		goto exit;
	}

	ret = rtos_task_create(NULL, "usbd_uvc_get_frame_handler", usbd_uvc_get_frame_handler, NULL, 1024U, 5);
	if (ret != SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Create USBD USBD_UVC GET FRAME thread fail\n", __FUNCTION__);
		ret = -1;
		goto exit;
	}

	dev->uvc_in_buf = malloc(USBD_UVC_IN_BUF_SIZE);
	usbd_register_class(&usbd_uvc_driver);
exit:
	return ret;
}

/**
  * @brief  De-Initialize UVC device
  *         - Unregister UVC class
  *         - Delete tasks, queues, semaphores, and locks
  *         - Reset internal buffer lists
  * @retval None
  */
void usbd_uvc_deinit(void)
{
	usbd_unregister_class();
	usb_os_lock_delete(usbd_uvc_dev.lock);
	usb_os_sema_delete(usbd_uvc_dev.uvc_Cmd_wakeup_sema);
	usb_os_sema_delete(usbd_uvc_dev.video.output_queue_sema);
	usb_os_queue_delete(usbd_uvc_dev.video.complete_bf_req);
	usb_os_lock_delete(usbd_uvc_dev.bod_mutex);
	usb_os_lock_delete(usbd_uvc_dev.video.input_lock);
	usb_os_lock_delete(usbd_uvc_dev.video.output_lock);
	INIT_LIST_HEAD(&usbd_uvc_dev.video.input_queue);
	INIT_LIST_HEAD(&usbd_uvc_dev.video.output_queue);
	INIT_LIST_HEAD(&usbd_uvc_dev.bod_list);
	usbd_uvc_dev.init_done = 0;
	if (usbd_uvc_dev.uvc_in_buf) {
		free(usbd_uvc_dev.uvc_in_buf);
		usbd_uvc_dev.uvc_in_buf = NULL;
	}
}
/**
  * @brief  Register UVC parameter change callback
  *         - Set callback function for UVC parameter updates
  *         - Triggered when format, resolution, or FPS changes
  * @param  cb: Callback function pointer (void (*)(void *))
  * @retval None
  * @note
  *         - Callback should be non-blocking
  *         - May be called from control or streaming context
  */
void usbd_uvc_set_change_parm_cb(int cb)
{
	usbd_uvc_dev.change_parm_cb = (void (*)(void *))cb;
}
/**
  * @brief  Get current UVC format configuration
  *         - Return pointer to internal UVC format structure
  *         - Contains resolution, format, FPS, and ISP settings
  * @retval Pointer to usbd_uvc_format_t
  * @note
  *         - Do not free returned pointer
  *         - Ensure UVC is initialized before calling
  *         - Shared resource, may require synchronization
  */
usbd_uvc_format_t *usbd_uvc_get_format(void)
{
	return usbd_uvc_dev.uvc_format_ptr;
}
