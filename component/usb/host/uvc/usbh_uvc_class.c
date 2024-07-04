/**
  ******************************************************************************
  * @file    usbh_uvc_class.c
  * @author  Realsil WLAN5 Team
  * @brief   This file provides the functionalities of the USB UVC Class
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "usbh_uvc.h"


/* Private defines -----------------------------------------------------------*/
#define USBH_UVC_DEBUG 0

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

extern usbh_uvc_host_t uvc_host;

static int usbh_uvc_attach(usb_host_t *host);
static int usbh_uvc_detach(usb_host_t *host);
static int usbh_uvc_process(usb_host_t *host);
static int usbh_uvc_sof(usb_host_t *host);
static int usbh_uvc_setup(usb_host_t *host);

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "UVC";

/* USB Standard Device Descriptor */
static usbh_class_driver_t usbh_uvc_driver = {
	.class_code = UVC_CLASS_CODE,
	.attach = usbh_uvc_attach,
	.detach = usbh_uvc_detach,
	.setup = usbh_uvc_setup,
	.process = usbh_uvc_process,
	.sof = usbh_uvc_sof
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Attach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uvc_attach(usb_host_t *host)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	uvc_stream_t *stream;
	int status = HAL_ERR_UNKNOWN;
	int i = 0;
	u32 ep_size = 0;

	uvc->host = host;

	status = usbh_uvc_parse_cfgdesc(host);
	if (status) {
		RTK_LOGS(TAG, "[UVC] Parse uvc desc fail\n");
		return status;
	}

	/* find the first alt setting and enpoint as default for each vs interface */
	for (i = 0; i < uvc->uvc_desc.vs_num; i ++) {
		stream = &uvc->stream[i];
		stream->cur_setting.cur_vs_intf = &uvc->uvc_desc.vs_intf[i];
		ep_size = uvc->uvc_desc.vs_intf[i].altsetting[0].endpoint->wMaxPacketSize;

		if (host->config.speed == USB_SPEED_HIGH) {
			ep_size = (ep_size & 0x07ff) * (1 + ((ep_size >> 11) & 3));
		} else {
			ep_size = ep_size & 0x07ff;
		}

		//Note: vc may has a interrupt endpoint, vs may has a bulk endpoint for still image data. not support now.

		stream->cur_setting.altsetting = &uvc->uvc_desc.vs_intf[i].altsetting[0];
		stream->cur_setting.bAlternateSetting = ((usbh_if_desc_t *)stream->cur_setting.altsetting->p)->bAlternateSetting;
		stream->cur_setting.ep_addr = stream->cur_setting.altsetting->endpoint->bEndpointAddress;
		stream->cur_setting.ep_size = ep_size;
		stream->cur_setting.mps = stream->cur_setting.altsetting->endpoint->wMaxPacketSize & 0x7ff;
		stream->cur_setting.interval = stream->cur_setting.altsetting->endpoint->bInterval;
		stream->cur_setting.ep_type = stream->cur_setting.altsetting->endpoint->bmAttributes & USB_EP_XFER_TYPE_MASK;
		stream->cur_setting.bInterfaceNumber = stream->cur_setting.cur_vs_intf->bInterfaceNumber;
		stream->cur_setting.valid = 1;
		stream->cur_setting.pipe = usbh_alloc_pipe(host, stream->cur_setting.ep_addr);

#if USBH_UVC_DEBUG
		RTK_LOGS(TAG, "[UVC] Stream[%d]->cur_set.altsetting:%d\n", i, stream->cur_setting.altsetting);
		TK_LOGS(TAG, "[UVC] Stream[%d]->cur_set.bAlternateSetting:%d\n", i, stream->cur_setting.bAlternateSetting);
		RTK_LOGS(TAG, "[UVC] Stream[%d]->cur_set.ep_addr:%d\n", i, stream->cur_setting.ep_addr);
		RTK_LOGS(TAG, "[UVC] Stream[%d]->cur_set.ep_size:%d\n", i, stream->cur_setting.ep_size);
		RTK_LOGS(TAG, "[UVC] Stream[%d]->cur_set.mps:%d\n", i, stream->cur_setting.mps);
		RTK_LOGS(TAG, "[UVC] Stream[%d]->cur_set.interval:%d\n", i, stream->cur_setting.interval);
		RTK_LOGS(TAG, "[UVC] Stream[%d]->cur_set.ep_type:%d\n", i, stream->cur_setting.ep_type);
		RTK_LOGS(TAG, "[UVC] Stream[%d]->cur_set.bInterfaceNumber:%d\n", i, stream->cur_setting.bInterfaceNumber);
#endif
	}

	stream->stream_data_state = STREAM_STATE_IDLE;

	status = HAL_OK;
	return status;
}

/**
  * @brief  Detach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uvc_detach(usb_host_t *host)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	uvc_stream_t *stream ;
	int i;

	for (i = 0; i < uvc->uvc_desc.vs_num; i ++) {
		stream = &uvc->stream[i];
		if (stream->cur_setting.pipe) {
			usbh_close_pipe(host, stream->cur_setting.pipe);
			usbh_free_pipe(host, stream->cur_setting.pipe);
			stream->cur_setting.pipe = 0U;
		}
	}

	if ((uvc->cb != NULL) && (uvc->cb->detach != NULL)) {
		uvc->cb->detach();
	}

	return HAL_OK;
}

/**
  * @brief  Standard control requests handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uvc_setup(usb_host_t *host)
{
	int status = HAL_BUSY;
	int i;
	usbh_uvc_host_t *uvc = &uvc_host;
	uvc_stream_t *stream;

	for (i = 0; i < uvc->uvc_desc.vs_num; i ++) {
		stream = &uvc->stream[i];
		do {
			status = usbh_ctrl_set_interface(host, stream->cur_setting.bInterfaceNumber, 0);
		} while (status != HAL_OK);

		usbh_uvc_video_init(stream);
	}

	if ((uvc->cb != NULL) && (uvc->cb->attach != NULL)) {
		uvc->cb->attach();
	}

	return HAL_OK;
}

/**
  * @brief  State machine handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uvc_process(usb_host_t *host)
{
	int status = HAL_BUSY;
	usbh_uvc_host_t *uvc = &uvc_host;
	int i;
	UNUSED(host);

	for (i = 0; i < uvc->uvc_desc.vs_num; i ++) {
		if (uvc->stream[i].stream_data_state == STREAM_DATA_IN) {
			status = usbh_uvc_process_rx(&uvc->stream[i]);
			if (status) {
				RTK_LOGS(TAG, "[UVC] Process rx err\n");
			}
		}
	}

	return status;
}

/**
  * @brief  SOF callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uvc_sof(usb_host_t *host)
{
	usbh_uvc_process_sof(host);

	return HAL_OK;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init UVC class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_uvc_class_init(void)
{
	int ret = HAL_OK;
	ret = usbh_register_class(&usbh_uvc_driver);

	return ret;
}

/**
  * @brief  Deinit UVC class
  * @retval void
  */
void usbh_uvc_class_deinit(void)
{
	usbh_unregister_class(&usbh_uvc_driver);
}

