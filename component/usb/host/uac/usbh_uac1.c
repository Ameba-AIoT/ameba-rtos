/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/
#include "usbh.h"
#include "usbh_uac1.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#define USB_UAC_FEATURE_UNIT                0x06
#define USB_DT_CS_INTERFACE            		0x24
#define USB_DT_CS_ENDPOINT             		0x25

#define ENABLE_DUMP_DESCRIPYOT_PARSE (1)
//#undef ENABLE_DUMP_DESCRIPYOT_PARSE

typedef struct {
	u8  bLength;
	u8  bDescriptorType;
} usb_descriptor_header_t;

/* Private function prototypes -----------------------------------------------*/

static int usbh_uac_cb_attach(usb_host_t *host);
static int usbh_uac_cb_detach(usb_host_t *host);
static int usbh_uac_cb_process(usb_host_t *host, u32 msg);
static int usbh_uac_ctrl_setting(usb_host_t *host, u32 msg);
static int usbh_uac_cb_setup(usb_host_t *host);
static int usbh_uac_cb_sof(usb_host_t *host);
static int usbh_uac_process_set_out_alt(usb_host_t *host);
static int usbh_uac_process_set_out_freq(usb_host_t *host);
static void usbh_uac_deinit_all_pipe(void);
/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "USBH";

/* USB Standard Device Descriptor */
static usbh_class_driver_t usbh_uac_driver = {
	.class_code = USB_CLASS_AUDIO,
	.attach = usbh_uac_cb_attach,
	.detach = usbh_uac_cb_detach,
	.setup = usbh_uac_cb_setup,
	.process = usbh_uac_cb_process,
	.sof = usbh_uac_cb_sof,
};

static usbh_uac_host_t usbh_uac_host;

static void usbh_uac_dump_cfgdesc(void)
{
#ifdef ENABLE_DUMP_DESCRIPYOT_PARSE
	int i, j, k;
	usbh_uac_host_t *uac = &usbh_uac_host;
	uac_cfg_t *puacdesc = &uac->uac_desc;
	RTK_LOGS(TAG, RTK_LOG_INFO, "--------------------Dump Start------------------------------\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, "As Count=%d[%d-%d]\n", puacdesc->as_count, puacdesc->cur_as_index[USBH_UAC_ISOC_IN_IDX],
			 puacdesc->cur_as_index[USBH_UAC_ISOC_OUT_IDX]);

	//dump uac ac
	//dump uac as
	for (i = 0; i < puacdesc->as_count; i ++) {
		uac_as_t *pasintf = &puacdesc->as_intf[i];
		RTK_LOGS(TAG, RTK_LOG_INFO, "interNum=%d[AltCount=%d,type=%d]\n", pasintf->bInterfaceNumber, pasintf->alt_count, pasintf->msg_type);
		for (j = 0; j < pasintf->alt_count; j ++) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "[ep=0x%x,maxsize=%d]\n",
					 pasintf->altsetting[j].audio_ep->bEndpointAddress, pasintf->altsetting[j].audio_ep->wMaxPacketSize);
			RTK_LOGS(TAG, RTK_LOG_INFO, "[ch=%d,bitwidth=%d,freqCount=%d]\n", pasintf->altsetting[j].format->bNrChannels,
					 pasintf->altsetting[j].format->bBitResolution, pasintf->altsetting[j].format->bSamFreqType);
			for (k = 0; k < pasintf->altsetting[j].format->bSamFreqType; k++) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "fre:id=%d ,freq=%d-%d-%d ", k, (u32)pasintf->altsetting[j].format->tSamFreq[k][0]
						 , ((u32)pasintf->altsetting[j].format->tSamFreq[k][1]) << 8, ((u32)pasintf->altsetting[j].format->tSamFreq[k][2]) << 16);
				RTK_LOGS(TAG, RTK_LOG_INFO, "fre:id=%d ,freq=%d\n", k, (u32)pasintf->altsetting[j].format->tSamFreq[k][0]
						 | ((u32)pasintf->altsetting[j].format->tSamFreq[k][1]) << 8 | ((u32)pasintf->altsetting[j].format->tSamFreq[k][2]) << 16);
			}
		}
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "---------------------Dump End-----------------------------\n");
#endif
}

static void usbh_uac_dump_cur_setting_cfgdesc(void)
{
#ifdef ENABLE_DUMP_DESCRIPYOT_PARSE
	int i;
	usbh_uac_host_t *uac = &usbh_uac_host;

	RTK_LOGS(TAG, RTK_LOG_INFO, "--------------------Dump Start------------------------------\n");
	for (i = 0; i < USBH_UAC_ISOC_NUM; i ++) {
		uac_setting_t *psetting = &uac->cur_setting[i];
		RTK_LOGS(TAG, RTK_LOG_INFO, "interNum=%d[bAlternateSetting=%d,freId=%d]\n",
				 psetting->bInterfaceNumber, psetting->bAlternateSetting, psetting->bFreqId);
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "---------------------Dump End-----------------------------\n");
#endif
}

static void usbh_uac_deinit_all_pipe(void)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	usb_host_t *host = uac->host;

	if (uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info.isoc_pipe) {
		usbh_close_pipe(host, uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info.isoc_pipe);
		usbh_free_pipe(host, uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info.isoc_pipe);
		uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info.isoc_pipe = 0U;
	}

	if (uac->cur_setting[USBH_UAC_ISOC_IN_IDX].ep_info.isoc_pipe) {
		usbh_close_pipe(host, uac->cur_setting[USBH_UAC_ISOC_IN_IDX].ep_info.isoc_pipe);
		usbh_free_pipe(host, uac->cur_setting[USBH_UAC_ISOC_IN_IDX].ep_info.isoc_pipe);
		uac->cur_setting[USBH_UAC_ISOC_IN_IDX].ep_info.isoc_pipe = 0U;
	}
}


/**
  * @brief	Find next standard descriptor
  * @param	pbuf: given buffer to find
			len: given buffer length
  * @retval Pointer of next standard descriptor
  */
static u8 *usbh_uac_find_next_stdesc(u8 *pbuf, u32 *len)
{
	u8 *desc = pbuf;

	while (*len > 0) {
		if (((usb_descriptor_header_t *)desc)->bDescriptorType == USB_DESC_TYPE_INTERFACE) {
			return (u8 *) desc;
		} else {
			*len -= ((usb_descriptor_header_t *)desc)->bLength;
			desc += ((usb_descriptor_header_t *)desc)->bLength;
		}
	}

	return NULL;
}


/**
  * @brief	Parse audio control interface
  * @param	desc: given descriptor buffer
			length: lenghth of given buffer
  * @retval Status
  */
static int usbh_uac_parse_ac(u8 *pbuf, u32 *length)
{
	*length = ((usb_descriptor_header_t *) pbuf)->bLength;
	RTK_LOGS(TAG, RTK_LOG_INFO, "enter usbh_uac_parse_ac *length=%d\n", *length);
	//Handle AC command
	return  0;
}


/**
  * @brief	Parse audio streaming interface
  * @param	pbuf: given descriptor buffer
			length: lenghth of given buffer
  * @retval Status
  */
static int usbh_uac_parse_as(u8 *pbuf, u32 *length)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	uac_as_t *as_intf = NULL;
	struct uac_format_type_i_discrete_descriptor *fmt = NULL;
	u8 *desc = pbuf;
	u32 freq;
	u16 len = 0;
	u8 bAlternateSetting;

	//uac alt setting 0
	as_intf = &uac->uac_desc.as_intf[uac->uac_desc.as_count];
	as_intf->p = desc;
	as_intf->bInterfaceNumber = desc[2];
	*length = ((usb_descriptor_header_t *) desc)->bLength;
	desc = pbuf + *length;

	//for next alt setting type: in out , it means two
	uac->uac_desc.as_count++;

	//finish setting change
	while (1) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "enter loop type:0x%x\n", ((usb_descriptor_header_t *) desc)->bDescriptorType);
		switch (((usb_descriptor_header_t *) desc)->bDescriptorType) {
		case USB_DESC_TYPE_INTERFACE:
			if (((usbh_if_desc_t *)desc)->bInterfaceNumber != as_intf->bInterfaceNumber) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "newnum=%d:old=%d, return\n", ((usbh_if_desc_t *)desc)->bInterfaceNumber, as_intf->bInterfaceNumber);
				return 0;
			}
			bAlternateSetting = ((usbh_if_desc_t *)desc)->bAlternateSetting;
			if (bAlternateSetting != 0) {//not setting 0
				as_intf->altsetting[bAlternateSetting - 1].p = desc;
				as_intf->alt_count++;
				as_intf->altsetting[bAlternateSetting - 1].desc = (usbh_if_desc_t *)desc;

				len = ((usb_descriptor_header_t *) desc)->bLength;
				desc = desc + len;
				*length += len;
			} else {
				return 0;
			}
			break;

		case USB_DT_CS_INTERFACE: {
			struct uac_format_type_i_discrete_descriptor *psubtype = (struct uac_format_type_i_discrete_descriptor *)desc;
			if (UAC_FORMAT_TYPE_II == psubtype->bDescriptorSubtype) {
				as_intf->altsetting[bAlternateSetting - 1].format = (struct uac_format_type_i_discrete_descriptor *)desc;
			} else {
				as_intf->altsetting[bAlternateSetting - 1].header = (struct uac1_as_header_descriptor *)desc;
			}
			len = ((usb_descriptor_header_t *) desc)->bLength;
			desc = desc + len;
			*length += len;
		}
		break;

		case USB_DESC_TYPE_ENDPOINT:
			if (bAlternateSetting != 0) {
				as_intf->altsetting[bAlternateSetting - 1].audio_ep = (struct usb_audio_endpoint_descriptor *)desc;
				if (as_intf->altsetting[bAlternateSetting - 1].audio_ep->bEndpointAddress & 0x80) {
					as_intf->msg_type = USBH_UAC_ISOC_IN_IDX;
				} else {
					as_intf->msg_type = USBH_UAC_ISOC_OUT_IDX;
				}
				len = ((usb_descriptor_header_t *) desc)->bLength;
				desc = desc + len;
				*length += len;
			} else {
				return 0;
			}
			break;

		case USB_DT_CS_ENDPOINT:
			if (bAlternateSetting != 0) {
				as_intf->altsetting[bAlternateSetting - 1].iso_ep = (struct uac_iso_endpoint_descriptor *)desc;
				len = ((usb_descriptor_header_t *) desc)->bLength;
				desc = desc + len;
				*length += len;
				fmt = as_intf->altsetting[bAlternateSetting - 1].format;
				freq = (fmt->tSamFreq[0][2] << 16) | (fmt->tSamFreq[0][1] << 8) | fmt->tSamFreq[0][0];
				RTK_LOGS(TAG, RTK_LOG_INFO, "parse finish !,dump %d/%d format bitwidth %d channel %d freqtype%d freq%d \n",
						 as_intf->bInterfaceNumber, bAlternateSetting, fmt->bNrChannels, fmt->bBitResolution, fmt->bSamFreqType, freq);
			} else {
				return 0;
			}
			break;

		default:
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "bDescriptorType: %d\n", ((usb_descriptor_header_t *) desc)->bDescriptorType);
			return 0;
		}
	}
}

/**
  * @brief	Parse configuration descriptor
  * @param	host: usb host structure
  * @retval Status
  */
int usbh_uac_parse_cfgdesc(usb_host_t *host)
{
	int ret = HAL_OK;
	usbh_cfg_desc_t *desc = (usbh_cfg_desc_t *)usbh_get_raw_configuration_descriptor(host);
	u32 cfglen = (u32)desc->wTotalLength;
	usbh_if_desc_t *pbuf = (usbh_if_desc_t *) desc;
	u32 len = 0;

	while (1) {
		pbuf = (usbh_if_desc_t *)usbh_uac_find_next_stdesc((u8 *)pbuf, &cfglen);
		if (pbuf == NULL) {
			break;
		}

		if (pbuf->bInterfaceProtocol != 0x00) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC parse configuration fail: only support UAC1.0!\n");
			ret = HAL_ERR_UNKNOWN;
			return ret;
		}

		if (pbuf->bInterfaceClass == USB_CLASS_AUDIO) {
			switch (pbuf->bInterfaceSubClass) {
			case USB_SUBCLASS_AUDIOCONTROL: //audio control
				ret = usbh_uac_parse_ac((u8 *)pbuf, &len);
				if (ret) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "UAC parse audio control fail\n");
					return ret;
				}
				pbuf = (usbh_if_desc_t *)((u8 *) pbuf + len);
				cfglen -= len;
				len = 0;
				break;

			case USB_SUBCLASS_AUDIOSTREAMING: //audio streaming
				RTK_LOGS(TAG, RTK_LOG_INFO, "setting=%d,len=%d,type=%d\n",
						 pbuf->bAlternateSetting, pbuf->bLength, pbuf->bDescriptorType);
				if (pbuf->bAlternateSetting == 0) { //setting 0
					ret = usbh_uac_parse_as((u8 *)pbuf, &len);
					if (ret) {
						RTK_LOGS(TAG, RTK_LOG_INFO, "UAC parse audio streaming fail\n");
						return ret;
					}

					pbuf = (usbh_if_desc_t *)((u8 *) pbuf + len);
					cfglen -= len;
					len = 0;
				}
				break;

			default:
				RTK_LOGS(TAG, RTK_LOG_INFO, "subclass(%d) is not AC or AS\n", pbuf->bInterfaceClass);
				return HAL_ERR_PARA;

			}
		} else {
			//skip no audio descriptor
			RTK_LOGS(TAG, RTK_LOG_INFO, "skip ((usb_descriptor_header_t *)desc)->bDescriptorType=0x%x,len=%d\n", ((usb_descriptor_header_t *)desc)->bDescriptorType,
					 ((usb_descriptor_header_t *)desc)->bLength);
			cfglen -= ((usb_descriptor_header_t *)desc)->bLength;
			pbuf = (usbh_if_desc_t *)((u8 *) pbuf + ((usb_descriptor_header_t *)desc)->bLength);
		}
	}

	return ret;
}

int usbh_uac_setup_out(u8 id)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	usb_host_t *host = uac->host;
	int ret = HAL_ERR_UNKNOWN;

	uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].bFreqId = id;

	if ((host->state == USBH_CLASS_REQUEST) || (host->state == USBH_CLASS_READY)) {
		uac->sub_state = UAC_STATE_SET_OUT_ALT;
		usbh_notify_class_state_change(host, 0);
		ret = HAL_OK;
	}

	return ret;
}

int usbh_uac_setup_in(u8 id)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	usb_host_t *host = uac->host;
	int ret = HAL_ERR_UNKNOWN;
	uac->cur_setting[USBH_UAC_ISOC_IN_IDX].bFreqId = id;

	if ((host->state == USBH_CLASS_REQUEST) || (host->state == USBH_CLASS_READY)) {
		uac->sub_state = UAC_STATE_SET_IN_ALT;
		usbh_notify_class_state_change(host, 0);
		ret = HAL_OK;
	}

	return ret;
}

static void usbh_uac_dump_req_struxt(usb_setup_req_t *ctrl)
{
	UNUSED(ctrl);

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "[UAC] bmRequestType=0x%02X bRequest=0x%02X wValue=0x%04X wIndex=0x%04X wLength=0x%02X\n",
			 ctrl->bmRequestType,
			 ctrl->bRequest,
			 ctrl->wValue,
			 ctrl->wIndex,
			 ctrl->wLength);
}

static int usbh_uac_process_set_out_alt(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_uac_host_t *uac = &usbh_uac_host;

	setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE;
	setup.b.bRequest = USB_REQ_SET_INTERFACE;
	setup.b.wValue = uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].bAlternateSetting;
	setup.b.wIndex = uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].bInterfaceNumber;
	setup.b.wLength = 0U;
	usbh_uac_dump_req_struxt(&setup.b);

	return usbh_ctrl_request(host, &setup, NULL);
}

static int usbh_uac_process_set_in_alt(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_uac_host_t *uac = &usbh_uac_host;

	setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE;
	setup.b.bRequest = USB_REQ_SET_INTERFACE;
	setup.b.wValue = uac->cur_setting[USBH_UAC_ISOC_IN_IDX].bAlternateSetting;
	setup.b.wIndex = uac->cur_setting[USBH_UAC_ISOC_IN_IDX].bInterfaceNumber;
	setup.b.wLength = 0U;
	usbh_uac_dump_req_struxt(&setup.b);

	return usbh_ctrl_request(host, &setup, NULL);
}

static int usbh_uac_process_set_out_freq(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_uac_host_t *uac = &usbh_uac_host;
	u8 id = uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].bFreqId;

	setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_ENDPOINT;
	setup.b.bRequest = UAC_SET_CUR;
	setup.b.wValue = USBH_UAC_SAMPLING_FREQ_CONTROL;
	setup.b.wIndex = uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info.isoc_ep_addr;
	setup.b.wLength = 3U;
	usbh_uac_dump_req_struxt(&setup.b);

	return usbh_ctrl_request(host, &setup, uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].format_info.format_freq[id]);
}

static int usbh_uac_process_set_in_freq(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_uac_host_t *uac = &usbh_uac_host;
	u8 id = uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].bFreqId;

	setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_ENDPOINT;
	setup.b.bRequest = UAC_SET_CUR;
	setup.b.wValue = USBH_UAC_SAMPLING_FREQ_CONTROL;
	setup.b.wIndex = uac->cur_setting[USBH_UAC_ISOC_IN_IDX].ep_info.isoc_ep_addr;
	setup.b.wLength = 3U;
	usbh_uac_dump_req_struxt(&setup.b);

	return usbh_ctrl_request(host, &setup, uac->cur_setting[USBH_UAC_ISOC_IN_IDX].format_info.format_freq[id]);
}

/**
  * @brief  Iso in test
  * @param  host: Host handle
  * @retval None
*/
static void usbh_uac_isoc_in_process(usb_host_t *host)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	uac_ep_cfg_t *ep = &(uac->cur_setting[USBH_UAC_ISOC_IN_IDX].ep_info);
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	u32 len;

	switch (ep->isoc_state) {
	case UAC_TRANSFER_STATE_PROCESS:
		if ((host->tick - ep->isoc_tick) >= ep->isoc_interval) {
			ep->isoc_tick = host->tick;
			usbh_isoc_receive_data(host,
								   ep->isoc_buf,
								   ep->isoc_packet_size,
								   ep->isoc_pipe);

			ep->isoc_state = UAC_TRANSFER_STATE_PROCESS_BUSY;
		}
		usbh_notify_class_state_change(host, 0);
		break;

	case UAC_TRANSFER_STATE_PROCESS_BUSY:
		urb_state = usbh_get_urb_state(host, ep->isoc_pipe);
		if (urb_state == USBH_URB_DONE) {
			len = usbh_get_last_transfer_size(host, ep->isoc_pipe);
			if ((uac->cb != NULL) && (uac->cb->isoc_received != NULL)) {
				uac->cb->isoc_received(ep->isoc_buf, len);
			}
			ep->isoc_state = UAC_TRANSFER_STATE_IDLE;
			RTK_LOGD(TAG, "ISOC IN ok\n");
		} else if (urb_state == USBH_URB_ERROR) {
			ep->isoc_state = UAC_TRANSFER_STATE_PROCESS;
			RTK_LOGS(TAG, RTK_LOG_INFO, "ISOC IN failed\n");
		}
		usbh_notify_class_state_change(host, 0);
		break;

	default:
		break;
	}
}

/**
  * @brief  Iso out test
  * @param  host: Host handle
  * @retval None
*/
static void usbh_uac_isoc_out_process(usb_host_t *host)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	uac_ep_cfg_t *ep = &(uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info);
	usbh_urb_state_t urb_state = USBH_URB_IDLE;

	switch (ep->isoc_state) {
	case UAC_TRANSFER_STATE_PROCESS:
		if ((host->tick - ep->isoc_tick) >= ep->isoc_interval) {
			ep->isoc_tick = host->tick;
			if (ep->isoc_len > ep->isoc_packet_size) {
				usbh_isoc_send_data(host,
									ep->isoc_buf,
									ep->isoc_packet_size,
									ep->isoc_pipe);
			} else {
				usbh_isoc_send_data(host,
									ep->isoc_buf,
									(u16)ep->isoc_len,
									ep->isoc_pipe);
			}

			ep->isoc_state = UAC_TRANSFER_STATE_PROCESS_BUSY;
		}
		usbh_notify_class_state_change(host, 0);
		break;

	case UAC_TRANSFER_STATE_PROCESS_BUSY:
		urb_state = usbh_get_urb_state(host, ep->isoc_pipe);
		if (urb_state == USBH_URB_DONE) {
			if (ep->isoc_len > ep->isoc_packet_size) {
				ep->isoc_len -= ep->isoc_packet_size;
				ep->isoc_buf += ep->isoc_packet_size;
			} else {
				ep->isoc_len = 0U;
			}

			if (ep->isoc_len > 0U) {
				ep->isoc_state = UAC_TRANSFER_STATE_PROCESS;
			} else {
				RTK_LOGD(TAG, "ISOC OUT ok\n");
				ep->isoc_state = UAC_TRANSFER_STATE_IDLE;
				if ((uac->cb != NULL) && (uac->cb->isoc_transmitted != NULL)) {
					uac->cb->isoc_transmitted(urb_state);
				}
			}
		} else if (urb_state == USBH_URB_ERROR) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "ISOC OUT failed\n");
			ep->isoc_state = UAC_TRANSFER_STATE_IDLE;
			if ((uac->cb != NULL) && (uac->cb->isoc_transmitted != NULL)) {
				uac->cb->isoc_transmitted(urb_state);
			}
		}
		usbh_notify_class_state_change(host, 0);
		break;

	default:
		break;
	}
}

/**
  * @brief  Attach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uac_cb_attach(usb_host_t *host)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	usbh_cfg_desc_t *desc = NULL;
	int status = HAL_ERR_UNKNOWN;
	int j, k;
	u32 ep_size, cur_ep_size = 0;
	u32 max_ep_size;
	u8 intf_num;
	//u32 rx_fifo_size = host->config.rx_fifo_size;

	uac->host = host;

	if (host->config.speed == USB_SPEED_HIGH) {
		max_ep_size = 1024;
	} else {
		max_ep_size = 1023;
	}
	//max_ep_size = max_ep_size > rx_fifo_size ? rx_fifo_size : max_ep_size;

	status = usbh_uac_parse_cfgdesc(host);
	if (status) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Fail to parse uac class specific descriptor\n");
		return status;
	}
	usbh_uac_dump_cfgdesc();

	//select audio streaming interface 0 as current interface both of usb in/out
	uac->uac_desc.cur_as_index[USBH_UAC_ISOC_IN_IDX] = 0;
	uac->uac_desc.cur_as_index[USBH_UAC_ISOC_OUT_IDX] = 0;

	/*find best alt setting and enpoint*/
	desc = (usbh_cfg_desc_t *)usbh_get_raw_configuration_descriptor(host);
	RTK_LOGS(TAG, RTK_LOG_INFO, "bNumInterfaces %d\n", desc->bNumInterfaces);

	//only support isoc out driver and composite driver
	intf_num = (desc->bNumInterfaces - 1) > USBH_UAC_ISOC_NUM ? USBH_UAC_ISOC_NUM : (desc->bNumInterfaces - 1);

	for (j = 0; j < intf_num; j++) { // fixed
		uac->cur_setting[j].cur_altsetting = &uac->uac_desc.as_intf[j].altsetting[uac->uac_desc.cur_as_index[j]];
		uac_ep_cfg_t *ep = &uac->cur_setting[j].ep_info;
		uac_format_cfg_t *format = &uac->cur_setting[j].format_info;
		uac_alt_t *cur_alt = uac->cur_setting[j].cur_altsetting;

		RTK_LOGS(TAG, RTK_LOG_INFO, "Index=%d,num=%d,alt_count%d,type=%d\n",
				 uac->uac_desc.cur_as_index[j],
				 uac->uac_desc.as_intf[j].bInterfaceNumber,
				 uac->uac_desc.as_intf[j].alt_count,
				 uac->uac_desc.as_intf[j].msg_type);

		//format
		format->bFormatType = cur_alt->format->bFormatType;
		format->bBitResolution = cur_alt->format->bBitResolution;
		format->bNrChannels = cur_alt->format->bNrChannels;
		format->bSubframeSize = cur_alt->format->bSubframeSize;
		format->bSamFreqType = cur_alt->format->bSamFreqType;

		for (k = 0; k < format->bSamFreqType; k++) {
			format->format_freq[k][0] = cur_alt->format->tSamFreq[k][0];
			format->format_freq[k][1] = cur_alt->format->tSamFreq[k][1];
			format->format_freq[k][2] = cur_alt->format->tSamFreq[k][2];
		}

		//to do: add check before choose bFreqId
		//choose the first freqId
		uac->cur_setting[j].bFreqId = 0;
		uac->cur_setting[j].bInterfaceNumber = cur_alt->desc->bInterfaceNumber;
		uac->cur_setting[j].bAlternateSetting = cur_alt->desc->bAlternateSetting;

		//ep
		ep_size = cur_alt->audio_ep->wMaxPacketSize;
		RTK_LOGS(TAG, RTK_LOG_INFO, "EP size info %d,%d,%d\n", ep_size, cur_ep_size, max_ep_size);
		if ((ep_size >= cur_ep_size) && (ep_size <= max_ep_size)) {

			ep->isoc_ep_addr = cur_alt->audio_ep->bEndpointAddress;
			ep->isoc_packet_size = cur_alt->audio_ep->wMaxPacketSize;
			ep->isoc_interval = cur_alt->audio_ep->bInterval;

			ep->isoc_state = UAC_TRANSFER_STATE_IDLE;

			RTK_LOGS(TAG, RTK_LOG_INFO, "address =0x%x,size=%d\n", ep->isoc_ep_addr, ep->isoc_packet_size);
		}
	}

	uac->cur_setting[USBH_UAC_ISOC_IN_IDX].ep_info.isoc_pipe = usbh_alloc_pipe(host, uac->cur_setting[USBH_UAC_ISOC_IN_IDX].ep_info.isoc_ep_addr);
	uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info.isoc_pipe = usbh_alloc_pipe(host, uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info.isoc_ep_addr);

	usbh_uac_dump_cur_setting_cfgdesc();

	usbh_open_pipe(host,
				   uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info.isoc_pipe,
				   uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info.isoc_ep_addr,
				   USB_CH_EP_TYPE_ISOC,
				   uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info.isoc_packet_size);
	usbh_open_pipe(host,
				   uac->cur_setting[USBH_UAC_ISOC_IN_IDX].ep_info.isoc_pipe,
				   uac->cur_setting[USBH_UAC_ISOC_IN_IDX].ep_info.isoc_ep_addr,
				   USB_CH_EP_TYPE_ISOC,
				   uac->cur_setting[USBH_UAC_ISOC_IN_IDX].ep_info.isoc_packet_size);

	if ((uac->cb != NULL) && (uac->cb->attach != NULL)) {
		uac->cb->attach();
	}

	return HAL_OK;
}

/**
  * @brief  Detach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uac_cb_detach(usb_host_t *host)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	UNUSED(host);

	uac->state = UAC_STATE_IDLE;

	usbh_uac_deinit_all_pipe();

	if ((uac->cb != NULL) && (uac->cb->detach != NULL)) {
		uac->cb->detach();
	}

	return HAL_OK;
}

/**
  * @brief  Standard control requests handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uac_cb_setup(usb_host_t *host)
{
	usbh_uac_host_t *uac = &usbh_uac_host;

	UNUSED(host);

	if ((uac->cb != NULL) && (uac->cb->setup != NULL)) {
		uac->cb->setup();
	}

	return HAL_OK;
}

/**
  * @brief  Sof callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uac_cb_sof(usb_host_t *host)
{
	UNUSED(host);
	return 0;
}

/**
  * @brief  Ctrl State machine handling
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uac_ctrl_setting(usb_host_t *host, u32 msg)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	int ret = HAL_OK;
	UNUSED(host);
	UNUSED(msg);

	switch (uac->sub_state) {
	case UAC_STATE_CTRL_IDLE:
		usb_os_sleep_ms(1000);
		break;

	case UAC_STATE_SET_IN_ALT:
		RTK_LOGS(TAG, RTK_LOG_INFO, "Uac SETUP IN alt\n");
		ret = usbh_uac_process_set_in_alt(host);
		if (ret == HAL_OK) {
			uac->sub_state = UAC_STATE_SET_IN_FREQ;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Uac process IN Alt error\n");
			usb_os_sleep_ms(100);
		}
		break;
	case UAC_STATE_SET_IN_FREQ:
		RTK_LOGS(TAG, RTK_LOG_INFO, "Uac SETUP IN freq\n");
		ret = usbh_uac_process_set_in_freq(host);
		if (ret == HAL_OK) {
			uac->sub_state = UAC_STATE_SET_OUT_ALT;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Uac process IN freq error\n");
			usb_os_sleep_ms(100);
		}
		break;

	// if device don't support isoc in, shoud start from here
	case UAC_STATE_SET_OUT_ALT:
		RTK_LOGS(TAG, RTK_LOG_INFO, "Uac SETUP OUT alt\n");
		ret = usbh_uac_process_set_out_alt(host);
		if (ret == HAL_OK) {
			uac->sub_state = UAC_STATE_SET_IN_FREQ;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Uac process OUT Alt error\n");
			usb_os_sleep_ms(100);
		}
		break;
	case UAC_STATE_SET_OUT_FREQ:
		RTK_LOGS(TAG, RTK_LOG_INFO, "Uac SETUP OUT freq\n");
		ret = usbh_uac_process_set_out_freq(host);
		if (ret == HAL_OK) {
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Uac process IN freq error\n");
			usb_os_sleep_ms(100);
		}
		break;

	default:
		ret = HAL_OK;
		break;
	}

	return ret;
}

/**
  * @brief  State machine handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_uac_cb_process(usb_host_t *host, u32 msg)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	int ret = HAL_OK;
	UNUSED(host);
	UNUSED(msg);

	switch (uac->state) {
	case UAC_STATE_CTRL_SETTING:
		ret = usbh_uac_ctrl_setting(host, 0);
		if (ret == HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "UAC ctrl setting finish\n");
			uac->state = UAC_STATE_TRANSFER;
		}
		usbh_notify_class_state_change(host, 0);
		break;

	case UAC_STATE_TRANSFER:
		usbh_uac_isoc_out_process(host);
		usbh_uac_isoc_in_process(host);
		break;
	case UAC_STATE_ERROR:
		RTK_LOGS(TAG, RTK_LOG_INFO, "Uac ERROR\n");
		ret = usbh_ctrl_clear_feature(host, 0x00U);
		if (ret == HAL_OK) {
			uac->state = UAC_STATE_IDLE;
		}
		break;

	case UAC_STATE_IDLE:
	default:
		/* main task in idle/default status, sleep to release CPU */
		usb_os_sleep_ms(1);
		break;
	}

	return ret;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init uac class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_uac_init(usbh_uac_cb_t *cb)
{
	int ret;
	usbh_uac_host_t *uac = &usbh_uac_host;

	usb_os_memset(uac, 0x00, sizeof(usbh_uac_host_t));

	if (cb != NULL) {
		uac->cb = cb;
		if (cb->init != NULL) {
			ret = cb->init();
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "USBH uac user init fail\n");
				return ret;
			}
		}
	}

	usbh_register_class(&usbh_uac_driver);

	return HAL_OK;
}

/**
  * @brief  Deinit uac class
  * @retval Status
  */
int usbh_uac_deinit(void)
{
	int ret = HAL_OK;
	usbh_uac_host_t *uac = &usbh_uac_host;

	usbh_uac_deinit_all_pipe();

	if ((uac->cb != NULL) && (uac->cb->deinit != NULL)) {
		uac->cb->deinit();
	}

	usbh_unregister_class(&usbh_uac_driver);

	return ret;
}

/**
  * @brief  Start to transfer ISOC data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbh_uac_isoc_transmit(u8 *buf, u32 len)
{
	int ret = HAL_BUSY;
	usbh_uac_host_t *uac = &usbh_uac_host;
	usb_host_t *host = uac->host;
	uac_ep_cfg_t *ep = &(uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info);

	if ((uac->state == UAC_STATE_IDLE) || (uac->state == UAC_STATE_TRANSFER)) {
		if (ep->isoc_state == UAC_TRANSFER_STATE_IDLE) {
			ep->isoc_buf = buf;
			ep->isoc_len = len;
			ep->isoc_state = UAC_TRANSFER_STATE_PROCESS;
			uac->state = UAC_STATE_TRANSFER;
			usbh_notify_class_state_change(host, 0);
			ret = HAL_OK;
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "ISOC OUT busy uac->state =%d ep->state =%d\n", uac->state, ep->isoc_state);
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "ISOC OUT not ready\n");
	}

	return ret;
}

/**
  * @brief  Start to receive ISOC data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbh_uac_isoc_receive(u8 *buf, u32 len)
{
	int ret = HAL_BUSY;
	usbh_uac_host_t *uac = &usbh_uac_host;
	usb_host_t *host = uac->host;
	uac_ep_cfg_t *ep = &(uac->cur_setting[USBH_UAC_ISOC_IN_IDX].ep_info);
	UNUSED(len);
	if ((uac->state == UAC_STATE_IDLE) || (uac->state == UAC_STATE_TRANSFER)) {
		if (ep->isoc_state == UAC_TRANSFER_STATE_IDLE) {
			ep->isoc_buf = buf;
			ep->isoc_len = ep->isoc_packet_size;
			ep->isoc_state = UAC_TRANSFER_STATE_PROCESS;
			uac->state = UAC_STATE_TRANSFER;
			usbh_notify_class_state_change(host, 0);
			ret = HAL_OK;
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "ISOC IN busy uac->state =%d\n", ep->isoc_state);
			ret = HAL_BUSY;
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "ISOC IN not ready\n");
		ret = HAL_BUSY;
	}

	return ret;
}

/**
  * @brief  Find a matched altsetting
  * @param  channels: Channels
  * @param  bytewidth: Bytewidth
  * @param  sample_freq: Sample_freq
  * @retval Status
  */
int usbh_uac_find_altsetting(u8 channels, u8 bytewidth, u32 sample_freq)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	struct uac_format_type_i_discrete_descriptor *desc;
	uac_as_t *as_intf = &uac->uac_desc.as_intf[USBH_UAC_ISOC_OUT_IDX];
	usb_host_t *host = uac->host;
	int i, j;
	int find_flag = 0;
	int ret;
	u32 freq;
	//get out ep interface id and alt num
	u8 intf_num = uac->uac_desc.as_intf[USBH_UAC_ISOC_OUT_IDX].bInterfaceNumber;
	u8 alt_num = uac->uac_desc.as_intf[USBH_UAC_ISOC_OUT_IDX].alt_count;


	RTK_LOGS(TAG, RTK_LOG_DEBUG, "serach alt interface%daltsetting%d \n", intf_num, alt_num);

	//actuaclly search from alt 1
	for (i = 0; i < alt_num; i++) {
		desc = as_intf->altsetting[i].format;

		// Check format type, channels, and sample frequency
		if (desc->bBitResolution != bytewidth || desc->bNrChannels != channels) {
			continue;
		}

		// Check sample frequency
		for (j = 0; j < desc->bSamFreqType; j++) {
			freq = (desc->tSamFreq[j][2] << 16) |
				   (desc->tSamFreq[j][1] << 8) |
				   desc->tSamFreq[j][0];
			if (freq == sample_freq) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Found matching altsetting: Interface %d, Altsetting %d, Frequency ID %d\n",
						 intf_num,
						 i + 1,
						 j);
				find_flag = 1;// Return as soon as we find a match
				break;
			}
		}

		if (find_flag) {
			break;
		}
	}

	if (find_flag) {
		//update cur_setting
		uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].bFreqId = j;
		uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].bInterfaceNumber = intf_num;
		uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].bAlternateSetting = (i + 1);

		uac->state = UAC_STATE_CTRL_SETTING;
		uac->sub_state = UAC_STATE_SET_OUT_ALT;
		usbh_notify_class_state_change(host, 0);
		ret = HAL_OK;
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "No matching altsetting found.\n");
		ret = HAL_ERR_PARA;
	}

	return ret;
}

/**
  * @brief  Stop play
  * @retval Status
  */
void usbh_uac_stop_play(void)
{

	usbh_uac_host_t *uac = &usbh_uac_host;
	usb_host_t *host = uac->host;

	uac->state = UAC_STATE_IDLE;
	usbh_notify_class_state_change(host, 0);
}