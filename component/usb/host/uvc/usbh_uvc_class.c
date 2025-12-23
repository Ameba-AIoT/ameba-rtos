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

/* Private function prototypes -----------------------------------------------*/

extern usbh_uvc_host_t uvc_host;

static int usbh_uvc_attach(usb_host_t *host);
static int usbh_uvc_detach(usb_host_t *host);
static int usbh_uvc_process(usb_host_t *host, u32 msg);
static int usbh_uvc_sof(usb_host_t *host);
static int usbh_uvc_setup(usb_host_t *host);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "UVC";

/* USB UVC device identification */
static const usbh_dev_id_t uvc_devs[] = {
	{
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS,
		.idVendor = 0x0bdb,
		.bInterfaceClass = USBH_UVC_CLASS_CODE,
	},
	{
	},
};

/* USB Host UVC class driver */
static usbh_class_driver_t usbh_uvc_driver = {
	.id_table = uvc_devs,
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
	usbh_uvc_stream_t *stream = NULL;
	usbh_uvc_setting_t *cur_set = NULL;
	usbh_uvc_alt_t *alt_set = NULL;
	usbh_ep_desc_t *ep = NULL;
	usbh_pipe_t *pipe;
	int status = HAL_ERR_UNKNOWN;
	int i = 0;
	u32 xfer_len = 0;

	uvc->host = host;

	status = usbh_uvc_parse_cfgdesc(host);
	if (status) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Parse uvc desc fail\n");
		return status;
	}

	/* find the first alt setting and enpoint as default for each vs interface */
	for (i = 0; i < uvc->uvc_desc.vs_num; i ++) {
		stream = &uvc->stream[i];
		cur_set = &stream->cur_setting;
		cur_set->cur_vs_intf = &uvc->uvc_desc.vs_intf[i];
		alt_set = &cur_set->cur_vs_intf->altsetting[0];
		cur_set->altsetting = alt_set;
		cur_set->bAlternateSetting = ((usbh_itf_desc_t *)alt_set->p)->bAlternateSetting;
		cur_set->bInterfaceNumber = cur_set->cur_vs_intf->bInterfaceNumber;

		pipe = &cur_set->pipe;
		ep = alt_set->endpoint;
		xfer_len = ep->wMaxPacketSize;

		if (host->dev_speed == USB_SPEED_HIGH) {
			xfer_len = (xfer_len & 0x07ff) * (1 + ((xfer_len >> 11) & 3));
		} else {
			xfer_len = xfer_len & 0x07ff;
		}

		//Note: vc may has a interrupt endpoint, vs may has a bulk endpoint for still image data. not support now.
		pipe->ep_addr = ep->bEndpointAddress;
		pipe->ep_mps = ep->wMaxPacketSize & 0x7ff;
		pipe->ep_interval = ep->bInterval;
		pipe->ep_type = ep->bmAttributes & USB_EP_XFER_TYPE_MASK;
		pipe->xfer_len = xfer_len;
		cur_set->valid = 1;

#if USBH_UVC_DEBUG
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream[%d]->cur_set.altsetting:%d\n", i, cur_set->altsetting);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream[%d]->cur_set.bInterfaceNumber:%d\n", i, cur_set->bInterfaceNumber);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream[%d]->cur_set.bAlternateSetting:%d\n", i, cur_set->bAlternateSetting);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream[%d]->cur_set.pipe.ep_addr:%d\n", i, pipe->ep_addr);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream[%d]->cur_set.pipe.xfer_len:%d\n", i, pipe->xfer_len);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream[%d]->cur_set.pipe.ep_mps:%d\n", i, pipe->ep_mps);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream[%d]->cur_set.pipe.ep_interval:%d\n", i, pipe->ep_interval);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream[%d]->cur_set.pipe.ep_type:%d\n", i, pipe->ep_type);
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
	usbh_uvc_stream_t *stream;
	usbh_pipe_t *pipe;
	int i;

	for (i = 0; i < uvc->uvc_desc.vs_num; i ++) {
		stream = &uvc->stream[i];
		pipe = &stream->cur_setting.pipe;
		if (pipe->pipe_num) {
			usbh_close_pipe(host, pipe);
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
	int i, retry_cnt;
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_stream_t *stream;

	for (i = 0; i < uvc->uvc_desc.vs_num; i ++) {
		retry_cnt = 0;
		stream = &uvc->stream[i];
		do {
			status = usbh_ctrl_set_interface(host, stream->cur_setting.bInterfaceNumber, 0);
			if (status == HAL_OK) {
				break;
			} else {
				retry_cnt++;
				usb_os_sleep_ms(10);
			}
		} while (retry_cnt < 10);

		if (status != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Setup err\n");
			return HAL_ERR_HW;
		} else {
			usbh_uvc_video_init(stream);
		}
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
static int usbh_uvc_process(usb_host_t *host, u32 msg)
{
	int status = HAL_BUSY;
	usbh_uvc_host_t *uvc = &uvc_host;
	int i;
	UNUSED(host);
	UNUSED(msg);

	for (i = 0; i < uvc->uvc_desc.vs_num; i ++) {
		if (uvc->stream[i].stream_data_state == STREAM_DATA_IN) {
			status = usbh_uvc_process_rx(&uvc->stream[i]);
			if (status) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Process rx err\n");
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

