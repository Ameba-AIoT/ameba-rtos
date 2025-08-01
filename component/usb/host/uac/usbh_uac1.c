/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/
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
	usbh_uac_cfg_t *puacdesc = &uac->uac_desc;
	struct uac_format_type_i_discrete_descriptor *fmt = NULL;
	struct usb_audio_endpoint_descriptor *audio_ep = NULL;
	usbh_uac_as_t *pasintf = NULL;

	RTK_LOGS(TAG, RTK_LOG_INFO, "--------------------Dump Start------------------------------\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, "As Count=%d[%d-%d]\n", puacdesc->as_count, puacdesc->cur_as_index[USBH_UAC_ISOC_IN_IDX],
			 puacdesc->cur_as_index[USBH_UAC_ISOC_OUT_IDX]);
	//dump uac ac
	//dump uac as
	for (i = 0; i < puacdesc->as_count; i ++) {
		pasintf = &puacdesc->as_intf[i];
		RTK_LOGS(TAG, RTK_LOG_INFO, "Intf=%d[alts=%d,type=%d]\n", pasintf->intf_num, pasintf->alt_count, pasintf->msg_type);
		for (j = 0; j < pasintf->alt_count; j ++) {
			fmt = pasintf->alt_setting[j].format;
			audio_ep = pasintf->alt_setting[j].audio_ep;
			RTK_LOGS(TAG, RTK_LOG_INFO, "Ep 0x%x,mps %d\n", audio_ep->bEndpointAddress, audio_ep->wMaxPacketSize);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Ch=%d,bitwidth=%d,freqCount=%d]\n", fmt->bNrChannels,
					 fmt->bBitResolution, fmt->bSamFreqType);
			for (k = 0; k < fmt->bSamFreqType; k++) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Fre:id=%d, %dhz\n", k, (u32)fmt->tSamFreq[k][0]
						 | ((u32)fmt->tSamFreq[k][1]) << 8 | ((u32)fmt->tSamFreq[k][2]) << 16);
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
		usbh_uac_setting_t *psetting = &uac->cur_setting[i];
		RTK_LOGS(TAG, RTK_LOG_INFO, "Alt %d/%d,freqid %d]\n",
				 psetting->intf_num, psetting->alt_setting, psetting->freq_id);
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
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "AC len %d\n", *length);
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
	usbh_uac_as_t *as_intf = NULL;
	struct uac_format_type_i_discrete_descriptor *fmt = NULL;
	u8 *desc = pbuf;
	u32 freq;
	u16 len = 0;
	u8 alt_setting;

	//uac alt setting 0
	as_intf = &uac->uac_desc.as_intf[uac->uac_desc.as_count];
	as_intf->desc = desc;
	as_intf->intf_num = desc[2];
	*length = ((usb_descriptor_header_t *) desc)->bLength;
	desc = pbuf + *length;

	//for next alt setting type: in out , it means two
	uac->uac_desc.as_count++;

	//finish setting change
	while (1) {
		switch (((usb_descriptor_header_t *) desc)->bDescriptorType) {
		case USB_DESC_TYPE_INTERFACE:
			if (((usbh_if_desc_t *)desc)->bInterfaceNumber != as_intf->intf_num) {
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "AS intf new %d:old %d, return\n", ((usbh_if_desc_t *)desc)->bInterfaceNumber, as_intf->intf_num);
				return 0;
			}
			alt_setting = ((usbh_if_desc_t *)desc)->bAlternateSetting;
			if (alt_setting != 0) {//not setting 0
				as_intf->alt_setting[alt_setting - 1].desc = desc;
				as_intf->alt_count++;
				as_intf->alt_setting[alt_setting - 1].if_desc = (usbh_if_desc_t *)desc;

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
				as_intf->alt_setting[alt_setting - 1].format = (struct uac_format_type_i_discrete_descriptor *)desc;
			} else {
				as_intf->alt_setting[alt_setting - 1].header = (struct uac1_as_header_descriptor *)desc;
			}
			len = ((usb_descriptor_header_t *) desc)->bLength;
			desc = desc + len;
			*length += len;
		}
		break;

		case USB_DESC_TYPE_ENDPOINT:
			if (alt_setting != 0) {
				as_intf->alt_setting[alt_setting - 1].audio_ep = (struct usb_audio_endpoint_descriptor *)desc;
				if (as_intf->alt_setting[alt_setting - 1].audio_ep->bEndpointAddress & 0x80) {
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
			if (alt_setting != 0) {
				as_intf->alt_setting[alt_setting - 1].iso_ep = (struct uac_iso_endpoint_descriptor *)desc;
				len = ((usb_descriptor_header_t *) desc)->bLength;
				desc = desc + len;
				*length += len;
				fmt = as_intf->alt_setting[alt_setting - 1].format;
				freq = (fmt->tSamFreq[0][2] << 16) | (fmt->tSamFreq[0][1] << 8) | fmt->tSamFreq[0][0];
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "Parse %d/%d fmt %d/%d/%d, freid %d\n",
						 as_intf->intf_num, alt_setting, fmt->bNrChannels, fmt->bBitResolution, freq, fmt->bSamFreqType);
			} else {
				return 0;
			}
			break;

		default:
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

		if (pbuf->bInterfaceClass == USB_CLASS_AUDIO) {
			// only check audio interface
			if (pbuf->bInterfaceProtocol != 0x00) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid device, only UAC 1.0 is supported\n");
				ret = HAL_ERR_UNKNOWN;
				break;
			}
			switch (pbuf->bInterfaceSubClass) {
			case USB_SUBCLASS_AUDIOCONTROL: //audio control
				ret = usbh_uac_parse_ac((u8 *)pbuf, &len);
				if (ret) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "AC parse fail\n");
					return ret;
				}
				pbuf = (usbh_if_desc_t *)((u8 *) pbuf + len);
				cfglen -= len;
				len = 0;
				break;

			case USB_SUBCLASS_AUDIOSTREAMING: //audio streaming
				if (pbuf->bAlternateSetting == 0) { //setting 0
					ret = usbh_uac_parse_as((u8 *)pbuf, &len);
					if (ret) {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "AS parse fail\n");
						return ret;
					}

					pbuf = (usbh_if_desc_t *)((u8 *) pbuf + len);
					cfglen -= len;
					len = 0;
				}
				break;

			default:
				return HAL_ERR_PARA;

			}
		} else {
			//skip no audio descriptor
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Skip desc 0x%x/%d\n", ((usb_descriptor_header_t *)desc)->bDescriptorType,
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

	uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].freq_id = id;

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
	uac->cur_setting[USBH_UAC_ISOC_IN_IDX].freq_id = id;

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

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Request 0x%02X 0x%02X 0x%04X 0x%04X 0x%02X\n",
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
	setup.b.wValue = uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].alt_setting;
	setup.b.wIndex = uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].intf_num;
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
	setup.b.wValue = uac->cur_setting[USBH_UAC_ISOC_IN_IDX].alt_setting;
	setup.b.wIndex = uac->cur_setting[USBH_UAC_ISOC_IN_IDX].intf_num;
	setup.b.wLength = 0U;
	usbh_uac_dump_req_struxt(&setup.b);

	return usbh_ctrl_request(host, &setup, NULL);
}

static int usbh_uac_process_set_out_freq(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_uac_host_t *uac = &usbh_uac_host;
	u8 id = uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].freq_id;

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
	u8 id = uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].freq_id;

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
	usbh_uac_ep_cfg_t *ep = &(uac->cur_setting[USBH_UAC_ISOC_IN_IDX].ep_info);
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
		} else if (urb_state == USBH_URB_ERROR) {
			ep->isoc_state = UAC_TRANSFER_STATE_PROCESS;
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
	usbh_uac_ep_cfg_t *ep = &(uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info);
	usbh_urb_state_t urb_state = USBH_URB_IDLE;

	int cur_frame = usbh_get_current_frame(host);
	if (uac->cur_frame == 0) {
		uac->cur_frame = cur_frame;
	}
	switch (ep->isoc_state) {
	case UAC_TRANSFER_STATE_PROCESS:
		if ((cur_frame - uac->cur_frame) % ep->isoc_interval == 0) {
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
			uac->next_transfor = 1;
			uac->cur_frame = cur_frame;
			ep->isoc_state = UAC_TRANSFER_STATE_PROCESS_BUSY;
		}
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
				uac->next_transfor = 1;
			} else {
				ep->isoc_state = UAC_TRANSFER_STATE_IDLE;

				if ((uac->cb != NULL) && (uac->cb->isoc_transmitted != NULL)) {
					uac->cb->isoc_transmitted(urb_state);
				}
			}
		} else if (urb_state == USBH_URB_ERROR) {
			ep->isoc_state = UAC_TRANSFER_STATE_IDLE;
			if ((uac->cb != NULL) && (uac->cb->isoc_transmitted != NULL)) {
				uac->cb->isoc_transmitted(urb_state);
			}
		} else if (urb_state == USBH_URB_IDLE) {
			if ((cur_frame - uac->cur_frame) >= ep->isoc_interval) {
				ep->isoc_state = UAC_TRANSFER_STATE_PROCESS;
				uac->next_transfor = 1;
				if ((uac->cb != NULL) && (uac->cb->isoc_transmitted != NULL)) {
					uac->cb->isoc_transmitted(USBH_URB_BUSY);
				}
			}
		}

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
	usbh_uac_setting_t *cur_setting = NULL;
	usbh_uac_format_cfg_t *format = NULL;
	usbh_uac_alt_t *cur_alt = NULL;
	usbh_cfg_desc_t *desc = NULL;
	usbh_uac_ep_cfg_t *ep = NULL;
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
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Cfg parse fail\n");
		return status;
	}
	usbh_uac_dump_cfgdesc();

	//select audio streaming interface 0 as current interface both of usb in/out
	uac->uac_desc.cur_as_index[USBH_UAC_ISOC_IN_IDX] = 0;
	uac->uac_desc.cur_as_index[USBH_UAC_ISOC_OUT_IDX] = 0;

	/*find best alt setting and enpoint*/
	desc = (usbh_cfg_desc_t *)usbh_get_raw_configuration_descriptor(host);

	//only support isoc out driver and composite driver
	intf_num = (desc->bNumInterfaces - 1) > USBH_UAC_ISOC_NUM ? USBH_UAC_ISOC_NUM : (desc->bNumInterfaces - 1);

	for (j = 0; j < intf_num; j++) { // fixed
		cur_setting = &uac->cur_setting[j];
		cur_setting->cur_alt_setting = &uac->uac_desc.as_intf[j].alt_setting[uac->uac_desc.cur_as_index[j]];
		ep = &cur_setting->ep_info;
		format = &cur_setting->format_info;
		cur_alt = cur_setting->cur_alt_setting;

		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Index=%d,num=%d,alts=%d,type=%d\n\n",
				 uac->uac_desc.cur_as_index[j],
				 uac->uac_desc.as_intf[j].intf_num,
				 uac->uac_desc.as_intf[j].alt_count,
				 uac->uac_desc.as_intf[j].msg_type);

		//format
		format->format_type = cur_alt->format->bFormatType;
		format->bit_width = cur_alt->format->bBitResolution;
		format->channels = cur_alt->format->bNrChannels;
		format->sam_freq_type = cur_alt->format->bSamFreqType;

		for (k = 0; k < format->sam_freq_type; k++) {
			format->format_freq[k][0] = cur_alt->format->tSamFreq[k][0];
			format->format_freq[k][1] = cur_alt->format->tSamFreq[k][1];
			format->format_freq[k][2] = cur_alt->format->tSamFreq[k][2];
		}

		//to do: add check before choose freq_id
		//choose the first freqId
		cur_setting->freq_id = 0;
		cur_setting->intf_num = cur_alt->if_desc->bInterfaceNumber;
		cur_setting->alt_setting = cur_alt->if_desc->bAlternateSetting;

		//ep
		ep_size = cur_alt->audio_ep->wMaxPacketSize;
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "EP size %d/%d/%d\n", ep_size, cur_ep_size, max_ep_size);
		if ((ep_size >= cur_ep_size) && (ep_size <= max_ep_size)) {
			ep->isoc_ep_addr = cur_alt->audio_ep->bEndpointAddress;
			ep->isoc_packet_size = cur_alt->audio_ep->wMaxPacketSize;
			ep->isoc_interval = cur_alt->audio_ep->bInterval;
			ep->isoc_state = UAC_TRANSFER_STATE_IDLE;
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
	return HAL_OK;
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
		ret = usbh_uac_process_set_in_alt(host);
		if (ret == HAL_OK) {
			uac->sub_state = UAC_STATE_SET_IN_FREQ;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "IN alt err\n");
			usb_os_sleep_ms(100);
		}
		break;
	case UAC_STATE_SET_IN_FREQ:
		ret = usbh_uac_process_set_in_freq(host);
		if (ret == HAL_OK) {
			uac->sub_state = UAC_STATE_SET_OUT_ALT;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "IN freq err\n");
			usb_os_sleep_ms(100);
		}
		break;

	// if device don't support isoc in, shoud start from here
	case UAC_STATE_SET_OUT_ALT:
		ret = usbh_uac_process_set_out_alt(host);
		if (ret == HAL_OK) {
			uac->sub_state = UAC_STATE_SET_IN_FREQ;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "OUT alt err\n");
			usb_os_sleep_ms(100);
		}
		break;
	case UAC_STATE_SET_OUT_FREQ:
		ret = usbh_uac_process_set_out_freq(host);
		if (ret == HAL_OK) {
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "OUT freq err\n");
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
			uac->state = UAC_STATE_TRANSFER;
		}
		usbh_notify_class_state_change(host, 0);
		break;

	case UAC_STATE_TRANSFER:
		if (msg == uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info.isoc_pipe) {
			uac->next_transfor = 0;
			usbh_uac_isoc_out_process(host);
			if (uac->next_transfor) {
				usbh_notify_class_state_change(host, uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info.isoc_pipe);
			}
		} else if (msg == uac->cur_setting[USBH_UAC_ISOC_IN_IDX].ep_info.isoc_pipe) {
			uac->next_transfor = 0;
			usbh_uac_isoc_in_process(host);
			if (uac->next_transfor) {
				usbh_notify_class_state_change(host, uac->cur_setting[USBH_UAC_ISOC_IN_IDX].ep_info.isoc_pipe);
			}
		}
		break;
	case UAC_STATE_ERROR:
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC err\n");
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
				RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC init fail\n");
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
	usbh_uac_ep_cfg_t *ep = &(uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info);

	if ((uac->state == UAC_STATE_IDLE) || (uac->state == UAC_STATE_TRANSFER)) {
		if (ep->isoc_state == UAC_TRANSFER_STATE_IDLE) {
			ep->isoc_buf = buf;
			ep->isoc_len = len;
			ep->isoc_state = UAC_TRANSFER_STATE_PROCESS;
			uac->state = UAC_STATE_TRANSFER;
			usbh_notify_class_state_change(host, ep->isoc_pipe);
			ret = HAL_OK;
		} else {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "ISOC OUT busy %d\n", ep->isoc_state);
		}
	} else {
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
	usbh_uac_ep_cfg_t *ep = &(uac->cur_setting[USBH_UAC_ISOC_IN_IDX].ep_info);
	UNUSED(len);
	if ((uac->state == UAC_STATE_IDLE) || (uac->state == UAC_STATE_TRANSFER)) {
		if (ep->isoc_state == UAC_TRANSFER_STATE_IDLE) {
			ep->isoc_buf = buf;
			ep->isoc_len = ep->isoc_packet_size;
			ep->isoc_state = UAC_TRANSFER_STATE_PROCESS;
			uac->state = UAC_STATE_TRANSFER;
			usbh_notify_class_state_change(host, ep->isoc_pipe);
			ret = HAL_OK;
		} else {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "ISOC IN busy %d\n", ep->isoc_state);
			ret = HAL_BUSY;
		}
	} else {
		ret = HAL_BUSY;
	}

	return ret;
}

/**
  * @brief  Set a matched alt_setting
  * @param  intf_idx: Index of the audio interface
  * @param  channels: Channels
  * @param  byte_width: byte_width
  * @param  sampling_freq: sampling_freq
  * @retval Status
  */
int usbh_uac_set_alt_setting(u8 intf_idx, u8 channels, u8 byte_width, u32 sampling_freq)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	struct uac_format_type_i_discrete_descriptor *fmt;
	usbh_uac_as_t *as_intf = &uac->uac_desc.as_intf[intf_idx];
	struct usb_audio_endpoint_descriptor *audio_ep = NULL;
	usbh_uac_ep_cfg_t *ep = NULL;
	usb_host_t *host = uac->host;
	int i, j;
	int set_flag = 0;
	int ret;
	int frame_ms;
	u32 freq;
	//get ep interface id and alt num
	u8 intf_num = uac->uac_desc.as_intf[intf_idx].intf_num;
	u8 alt_num = uac->uac_desc.as_intf[intf_idx].alt_count;


	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Search alt %d/%d\n", intf_num, alt_num);

	//actuaclly search from alt 1
	for (i = 0; i < alt_num; i++) {
		fmt = as_intf->alt_setting[i].format;

		// Check format type, channels, and sample frequency
		if (fmt->bBitResolution != byte_width || fmt->bNrChannels != channels) {
			continue;
		}

		// Check sample frequency
		for (j = 0; j < fmt->bSamFreqType; j++) {
			freq = (fmt->tSamFreq[j][2] << 16) |
				   (fmt->tSamFreq[j][1] << 8) |
				   fmt->tSamFreq[j][0];
			if (freq == sampling_freq) {
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "Alt found %d/%d %d\n",
						 intf_num,
						 i + 1,
						 j);
				set_flag = 1;// Return as soon as we find a match
				break;
			}
		}

		if (set_flag) {
			break;
		}
	}

	if (set_flag) {
		//update cur_setting
		uac->cur_setting[intf_idx].freq_id = j;
		uac->cur_setting[intf_idx].intf_num = intf_num;
		uac->cur_setting[intf_idx].alt_setting = (i + 1);

		//update ep_info
		ep = &uac->cur_setting[intf_idx].ep_info;
		audio_ep = as_intf->alt_setting[i].audio_ep;

		ep->isoc_ep_addr = audio_ep->bEndpointAddress;
		ep->isoc_interval = audio_ep->bInterval;

		//calculate accurate one frame size(byte)
		frame_ms = UBSH_UAC_CAL_FRAME((channels * byte_width * sampling_freq * (audio_ep->bInterval)), (8 * 1000));

		if (frame_ms <= audio_ep->wMaxPacketSize) {
			ep->isoc_packet_size = frame_ms;
		} else {
			// device ep des error, stop transfer
			RTK_LOGS(TAG, RTK_LOG_ERROR, "MPS error: %d < %d\n", audio_ep->wMaxPacketSize, frame_ms);
			ret = HAL_ERR_PARA;
			return ret;
		}

		// reinit isoc out pipe
		usbh_uac_deinit_all_pipe();
		ep->isoc_pipe = usbh_alloc_pipe(host, ep->isoc_ep_addr);
		usbh_open_pipe(host, ep->isoc_pipe, ep->isoc_ep_addr, USB_CH_EP_TYPE_ISOC, ep->isoc_packet_size);

		uac->state = UAC_STATE_CTRL_SETTING;
		if (intf_idx == USBH_UAC_ISOC_OUT_IDX) {
			uac->sub_state = UAC_STATE_SET_OUT_ALT;
		} else {
			uac->sub_state = UAC_STATE_SET_IN_ALT;
		}

		usbh_notify_class_state_change(host, 0);
		ret = HAL_OK;

	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alt not found\n");
		ret = HAL_ERR_PARA;
	}

	return ret;
}

/**
  * @brief  Get alt setting structure and alt settings num for an interface.
  * @param  intf_idx: Index of the audio interface
  * @param  alt_num: Number of alt settings
  * @retval Pointer to alt setting structure
  */
const usbh_uac_alt_t *usbh_uac_get_alt_setting(u8 intf_idx, u8 *alt_num)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	usbh_uac_as_t *as_intf = &uac->uac_desc.as_intf[intf_idx];

	if (alt_num) {
		*alt_num = as_intf->alt_count;
	}

	return as_intf->alt_setting;
}

/**
  * @brief  Get frame size of current interface.
  * @param  intf_idx: Index of the audio interface
  * @retval frame size
  */
u32 usbh_uac_get_frame_size(u8 intf_idx)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	usbh_uac_ep_cfg_t *ep = &uac->cur_setting[intf_idx].ep_info;
	return ep->isoc_packet_size;
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