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
#define USB_DT_CS_INTERFACE                 0x24
#define USB_DT_CS_ENDPOINT                  0x25
#define USBH_UAC_WAIT_SLICE_MS              1

#if USBH_UAC_DEBUG
#define USBH_UAC_DEBUG_LOOP_TIME            1000
#endif

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
static int usbh_uac_parse_cfgdesc(usb_host_t *host);
static int usbh_uac_parse_ac(u8 *pbuf, u32 *length);
static int usbh_uac_parse_as(u8 *pbuf, u32 *length);


/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "UAC";

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

#if USBH_UAC_DEBUG
static int usbh_uac_count_list_blocks(struct list_head *head)
{
	int cnt = 0;
	struct list_head *pos;
	list_for_each(pos, head) {
		if (pos == NULL) {
			break;
		}
		cnt++;
	}
	return cnt;
}

/**
  * @brief  UAC status dump thread
  * @param  param: Pointer to parameters
  * @retval void
  */
static void usbh_uac_status_dump_thread(void *param)
{
	UNUSED(param);
	usbh_uac_host_t *uac = &usbh_uac_host;

	uac->dump_status_task_alive = 1;

	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC dump thread\n");

	while (1) {
		if (uac->state >= USBH_UAC_SETUP) {
			int ready_cnt = usbh_uac_count_list_blocks(&uac->isoc_out.ready_buf_lock_list.list);
			int free_cnt = usbh_uac_count_list_blocks(&uac->isoc_out.free_buf_lock_list.list);
			int busy_cnt = (uac->tx_buf != NULL) ? 1 : 0;

			RTK_LOGS(TAG, RTK_LOG_INFO, "UAC DUMP: free=%d ready=%d busy=%d\n", free_cnt, ready_cnt, busy_cnt);
		}

		rtos_time_delay_ms(USBH_UAC_DEBUG_LOOP_TIME);
	}

	rtos_task_delete(NULL);
}
#endif

static void usbh_uac_lock_list_init(usbh_uac_lock_list_head_t *llist)
{
	INIT_LIST_HEAD(&(llist->list));
	usb_os_lock_create(&(llist->lock));
}

static void usbh_uac_lock_list_deinit(usbh_uac_lock_list_head_t *llist)
{
	if (llist->lock != NULL) {
		usb_os_lock_delete(llist->lock);
		llist->lock = NULL;
	}

	INIT_LIST_HEAD(&(llist->list));
}

static void usbh_uac_lock_list_add_tail(struct list_head *node, usbh_uac_lock_list_head_t *llist)
{
	usb_os_lock(llist->lock);
	list_add_tail(node, &llist->list);
	usb_os_unlock(llist->lock);
}

static usbh_uac_buf_t *usbh_uac_lock_list_remove_head(usbh_uac_lock_list_head_t *llist)
{
	usb_os_lock(llist->lock);

	usbh_uac_buf_t *node = NULL;
	if (!list_empty(&llist->list)) {
		node = list_first_entry(&llist->list, usbh_uac_buf_t, list);
		list_del(&node->list);
	}

	usb_os_unlock(llist->lock);
	return node;
}

static void usbh_uac_dump_cfgdesc(void)
{
#if USBH_UAC_DEBUG
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
#if USBH_UAC_DEBUG
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

static int usbh_uac_usb_status_check(void)
{
	usbh_uac_host_t *uac = &usbh_uac_host;

	if (uac->state < USBH_UAC_SETUP) {
		//RTK_LOGS(TAG, RTK_LOG_DEBUG, "Device disconnected,wait\n");
		return HAL_BUSY;
	}

	return HAL_OK;
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

	//uac alt setting 0, desc += length, jump the alt_setting=0 descriptor
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
			if ((alt_setting != 0) && (alt_setting < USBH_UAC_ALT_SETTING_MAX)) {//not setting 0
				as_intf->alt_setting[alt_setting - 1].desc = desc;
				as_intf->alt_count++;
				as_intf->alt_setting[alt_setting - 1].if_desc = (usbh_if_desc_t *)desc;

				len = ((usb_descriptor_header_t *) desc)->bLength;
				desc = desc + len;
				*length += len;
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "AS parse return %d %d\n", alt_setting, USBH_UAC_ALT_SETTING_MAX);
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
static int usbh_uac_parse_cfgdesc(usb_host_t *host)
{
	int ret = HAL_OK;
	usbh_cfg_desc_t *desc = (usbh_cfg_desc_t *)usbh_get_active_raw_configuration_descriptor(host);
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
		uac->ctrl_state = UAC_STATE_SET_OUT_ALT;
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
		uac->ctrl_state = UAC_STATE_SET_IN_ALT;
		usbh_notify_class_state_change(host, 0);
		ret = HAL_OK;
	}

	return ret;
}

static void usbh_uac_dump_req_struct(usb_setup_req_t *ctrl)
{
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
	usbh_uac_dump_req_struct(&setup.b);

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
	usbh_uac_dump_req_struct(&setup.b);

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
	usbh_uac_dump_req_struct(&setup.b);

	usb_os_memcpy(uac->audio_fmt_ctrl_buf, uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].format_info.format_freq[id], 3);

	return usbh_ctrl_request(host, &setup, uac->audio_fmt_ctrl_buf);
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
	usbh_uac_dump_req_struct(&setup.b);

	usb_os_memcpy(uac->audio_fmt_ctrl_buf, uac->cur_setting[USBH_UAC_ISOC_IN_IDX].format_info.format_freq[id], 3);

	return usbh_ctrl_request(host, &setup, uac->audio_fmt_ctrl_buf);
}

static void usbh_uac_isoc_out_release_free_buf(usbh_uac_buf_t *p_buf)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	usbh_uac_buf_ctrl_t *pdata_ctrl = &(uac->isoc_out);

	if (p_buf) {
		usb_os_memset(p_buf->buf, 0x00, p_buf->buf_len);
		p_buf->buf_len = 0;
		usbh_uac_lock_list_add_tail(&p_buf->list, &(pdata_ctrl->free_buf_lock_list));
		// notify audio write continue
		rtos_sema_give(pdata_ctrl->isoc_sema);
	}

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

	//rx to do
	switch (ep->isoc_state) {
	case USBH_UAC_RX:
		if ((host->tick - ep->isoc_tick) >= ep->isoc_interval) {
			ep->isoc_tick = host->tick;
			usbh_isoc_receive_data(host,
								   ep->isoc_buf,
								   ep->isoc_packet_size,
								   ep->isoc_pipe);

			ep->isoc_state = USBH_UAC_RX_BUSY;
		}
		usbh_notify_class_state_change(host, 0);
		break;

	case USBH_UAC_RX_BUSY:
		urb_state = usbh_get_urb_state(host, ep->isoc_pipe);
		if (urb_state == USBH_URB_DONE) {
			len = usbh_get_last_transfer_size(host, ep->isoc_pipe);
			if ((uac->cb != NULL) && (uac->cb->isoc_received != NULL)) {
				uac->cb->isoc_received(ep->isoc_buf, len);
			}
			ep->isoc_state = USBH_UAC_EP_IDLE;
		} else if (urb_state == USBH_URB_ERROR) {
			ep->isoc_state = USBH_UAC_RX;
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
	usbh_uac_buf_ctrl_t *pdata_ctrl = &(uac->isoc_out);
	usbh_uac_buf_t *p_buf = NULL;
	int cur_frame;

	if (pdata_ctrl->transfer_continue == 0) {
		return;
	}

	switch (ep->isoc_state) {
	case USBH_UAC_TX:
		if (!list_empty(&(pdata_ctrl->ready_buf_lock_list.list))) {
			// check valid data
			p_buf = usbh_uac_lock_list_remove_head(&(pdata_ctrl->ready_buf_lock_list));
			if (p_buf && p_buf->buf_len > 0) {

				ep->isoc_buf = p_buf->buf;
				ep->isoc_len = p_buf->buf_len;
				usbh_isoc_send_data(host,
									ep->isoc_buf,
									(u16)ep->isoc_len,
									ep->isoc_pipe);

				if (uac->tx_buf) {
					usbh_uac_isoc_out_release_free_buf(uac->tx_buf);
				}
				uac->tx_buf = p_buf;
				uac->next_xfer = 1;
				//isoc tx in next frame
				uac->cur_frame = usbh_get_current_frame(host);
				ep->isoc_state = USBH_UAC_TX_BUSY;
			}
		}
		break;

	case USBH_UAC_TX_BUSY:
		urb_state = usbh_get_urb_state(host, ep->isoc_pipe);
		if (urb_state == USBH_URB_DONE) {
			usbh_uac_isoc_out_release_free_buf(uac->tx_buf);
			uac->tx_buf = NULL;
			ep->isoc_state = USBH_UAC_TX;
			uac->next_xfer = 1;
		} else if (urb_state == USBH_URB_ERROR) {
			usbh_uac_isoc_out_release_free_buf(uac->tx_buf);
			uac->tx_buf = NULL;
			ep->isoc_state = USBH_UAC_TX;
			uac->next_xfer = 1;
			if ((uac->cb != NULL) && (uac->cb->isoc_transmitted != NULL)) {
				uac->cb->isoc_transmitted(urb_state);
			}
		} else if (urb_state == USBH_URB_IDLE) {
			cur_frame = usbh_get_current_frame(host);
			if (((cur_frame - uac->cur_frame) > ep->isoc_interval) && (uac->tx_buf != NULL)) {
				usbh_uac_isoc_out_release_free_buf(uac->tx_buf);
				uac->tx_buf = NULL;
				ep->isoc_state = USBH_UAC_TX;
				uac->next_xfer = 1;
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
	uac->state = USBH_UAC_ATTACH;

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
	desc = (usbh_cfg_desc_t *)usbh_get_active_raw_configuration_descriptor(host);

	//only support isoc out driver and composite driver
	intf_num = (desc->bNumInterfaces - 1) > USBH_UAC_ISOC_NUM ? USBH_UAC_ISOC_NUM : (desc->bNumInterfaces - 1);

	for (j = 0; j < intf_num; j++) { // fixed
		usbh_uac_setting_t temp_setting = {0};
		usbh_uac_setting_t *cur_setting = &temp_setting;
		cur_alt = &uac->uac_desc.as_intf[j].alt_setting[uac->uac_desc.cur_as_index[j]];
		ep = &cur_setting->ep_info;
		format = &cur_setting->format_info;
		cur_setting->cur_alt_setting = cur_alt;

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

		if (format->sam_freq_type > USBH_UAC_FREQ_FORMAT_MAX) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Freq cnt(%d) > cfg(%d) limit\n", format->sam_freq_type, USBH_UAC_FREQ_FORMAT_MAX);
			format->sam_freq_type = USBH_UAC_FREQ_FORMAT_MAX;
		}

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
			ep->isoc_state = USBH_UAC_EP_IDLE;

			if (USB_EP_IS_IN(ep->isoc_ep_addr)) {
				uac->cur_setting[USBH_UAC_ISOC_IN_IDX] = temp_setting;
				RTK_LOGS(TAG, RTK_LOG_INFO, "EP add 0x%x, put in \n", ep->isoc_ep_addr);
			} else {
				uac->cur_setting[USBH_UAC_ISOC_OUT_IDX] = temp_setting;
				RTK_LOGS(TAG, RTK_LOG_INFO, "EP add 0x%x, put out \n", ep->isoc_ep_addr);
			}
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

	uac->xfer_state = UAC_STATE_IDLE;
	uac->state = USBH_UAC_DETACHED;

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
	uac->state = USBH_UAC_SETUP;
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

	usbh_uac_host_t *uac = &usbh_uac_host;
	usbh_uac_ep_cfg_t *ep = &(uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info);
	usbh_uac_buf_ctrl_t *pdata_ctrl = &(uac->isoc_out);

	if (pdata_ctrl->transfer_continue == 1) {
		usbh_notify_class_state_change(host, ep->isoc_pipe);
	}

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
	int ret_status = HAL_BUSY;
	UNUSED(msg);

	switch (uac->ctrl_state) {
	case UAC_STATE_CTRL_IDLE:
		usb_os_sleep_ms(1000);
		break;

	case UAC_STATE_SET_IN_ALT:
		ret = usbh_uac_process_set_in_alt(host);
		if (ret == HAL_OK) {
			uac->ctrl_state = UAC_STATE_SET_IN_FREQ;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "IN alt err\n");
			usb_os_sleep_ms(100);
		}
		break;
	case UAC_STATE_SET_IN_FREQ:
		ret = usbh_uac_process_set_in_freq(host);
		if (ret == HAL_OK) {
			uac->ctrl_state = UAC_STATE_SET_OUT_ALT;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "IN freq err\n");
			usb_os_sleep_ms(100);
		}
		break;

	// if device don't support isoc in, shoud start from here
	case UAC_STATE_SET_OUT_ALT:
		ret = usbh_uac_process_set_out_alt(host);
		if (ret == HAL_OK) {
			uac->ctrl_state = UAC_STATE_SET_OUT_FREQ;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "OUT alt err\n");
			usb_os_sleep_ms(100);
		}
		break;
	case UAC_STATE_SET_OUT_FREQ:
		ret = usbh_uac_process_set_out_freq(host);
		if (ret == HAL_OK) {
			ret_status = HAL_OK;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "OUT freq err\n");
			usb_os_sleep_ms(100);
		}
		break;

	default:
		ret_status = HAL_OK;
		break;
	}

	return ret_status;
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

	switch (uac->xfer_state) {
	case UAC_STATE_CTRL_SETTING:
		ret = usbh_uac_ctrl_setting(host, 0);
		if (ret == HAL_OK) {
			uac->xfer_state = UAC_STATE_IDLE;
		} else {
			usbh_notify_class_state_change(host, 0);
		}
		break;

	case UAC_STATE_TRANSFER:
		if (msg == uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info.isoc_pipe) {
			uac->next_xfer = 0;
			usbh_uac_isoc_out_process(host);
			if (uac->next_xfer) {
				usbh_notify_class_state_change(host, uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info.isoc_pipe);
			}
		} else if (msg == uac->cur_setting[USBH_UAC_ISOC_IN_IDX].ep_info.isoc_pipe) {
			uac->next_xfer = 0;
			usbh_uac_isoc_in_process(host);
			if (uac->next_xfer) {
				usbh_notify_class_state_change(host, uac->cur_setting[USBH_UAC_ISOC_IN_IDX].ep_info.isoc_pipe);
			}
		}
		break;

	case UAC_STATE_ERROR:
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC err\n");
		ret = usbh_ctrl_clear_feature(host, 0x00U);
		if (ret == HAL_OK) {
			uac->xfer_state = UAC_STATE_IDLE;
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

/**
  * @brief  Write data to a USB audio ring buffer
  * @param  pdata_ctrl: Pointer to the buffer control structure
  * @param  buffer: Pointer to Data buffer
  * @param  size: Data length
  * @param  written_len: Copy length
  * @retval Status 0 means get enought data, 1 means should continue to write the pkt
  */
static int usbh_uac_write_ring_buf(usbh_uac_buf_ctrl_t *pdata_ctrl, u8 *buffer, u32 size, u32 *written_len)
{
	usbh_uac_buf_t *p_buf = NULL;
	u32 mps = pdata_ctrl->mps;
	u32 offset = 0;
	u32 remain_size = pdata_ctrl->remain_size;

	if (remain_size > 0 && pdata_ctrl->p_buf) {
		u32 can_copy = mps - remain_size;
		u32 copy_len = size < can_copy ? size : can_copy;

		usb_os_memcpy(pdata_ctrl->p_buf->buf + remain_size, buffer + offset, copy_len);

		pdata_ctrl->remain_size += copy_len;
		offset += copy_len;
		size -= copy_len;
		*written_len += copy_len;

		if (pdata_ctrl->remain_size == mps) {
			pdata_ctrl->p_buf->buf_len = mps;
			usbh_uac_lock_list_add_tail(&pdata_ctrl->p_buf->list, &pdata_ctrl->ready_buf_lock_list);
			pdata_ctrl->p_buf = NULL;
			pdata_ctrl->remain_size = 0;
		} else {
			return 0;
		}
	}

	while (size >= mps) {
		p_buf = usbh_uac_lock_list_remove_head(&pdata_ctrl->free_buf_lock_list);
		if (!p_buf || !p_buf->buf) {
			return 1;
		}
		usb_os_memcpy(p_buf->buf, buffer + offset, mps);

		p_buf->buf_len = mps;
		usbh_uac_lock_list_add_tail(&p_buf->list, &pdata_ctrl->ready_buf_lock_list);

		*written_len += mps;
		size -= mps;
		offset += mps;
	}

	if (size > 0) {
		p_buf = usbh_uac_lock_list_remove_head(&pdata_ctrl->free_buf_lock_list);
		if (!p_buf || !p_buf->buf) {
			return 1;
		}
		usb_os_memcpy(p_buf->buf, buffer + offset, size);
		//RTK_LOGS(TAG, RTK_LOG_INFO, "remain %d\n", size);
		pdata_ctrl->p_buf = p_buf;
		pdata_ctrl->remain_size = size;
		*written_len += size;
	}
	return 0;
}

/**
  * @brief  Deinitialize UAC endpoint buffer control structure
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @retval void
  */
static void usbh_uac_ep_buf_ctrl_deinit(usbh_uac_buf_ctrl_t *buf_ctrl, usbh_uac_ep_cfg_t *ep)
{
	UNUSED(ep);

	buf_ctrl->mps = 0;
	buf_ctrl->transfer_continue = 0;

	if (buf_ctrl->sema_valid) {
		buf_ctrl->sema_valid = 0;

		if (buf_ctrl->write_wait_sema) {
			rtos_sema_give(buf_ctrl->isoc_sema);
		}
		do {
			usb_os_delay_us(100U);
		} while (buf_ctrl->write_wait_sema);

		rtos_sema_delete(buf_ctrl->isoc_sema);
	}

	if (buf_ctrl->isoc_buf != NULL) {
		usb_os_mfree(buf_ctrl->isoc_buf);
		buf_ctrl->isoc_buf = NULL;
	}

	if (buf_ctrl->buf_list_node != NULL) {
		usb_os_mfree(buf_ctrl->buf_list_node);
		buf_ctrl->buf_list_node = NULL;
	}

	usbh_uac_lock_list_deinit(&(buf_ctrl->free_buf_lock_list));
	usbh_uac_lock_list_deinit(&(buf_ctrl->ready_buf_lock_list));
}

/**
  * @brief  Initialize UAC endpoint buffer control structure
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @param  ep: Pointer to ep parameters structure
  * @retval Status
  */
static int usbh_uac_ep_buf_ctrl_init(usbh_uac_buf_ctrl_t *buf_ctrl, usbh_uac_ep_cfg_t *ep)
{
	usbh_uac_buf_t *pbuf_data;
	int ret = HAL_ERR_MEM;
	u32 block_size;
	u16 buf_list_cnt, idx;

	buf_list_cnt = buf_ctrl->frame_cnt;
	buf_ctrl->mps = ep->isoc_packet_size;

	if (buf_list_cnt == 0 || buf_ctrl->mps == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Err param: cnt=%d, mps=%d\n", buf_list_cnt, buf_ctrl->mps);
		return ret;
	}

	block_size = CACHE_LINE_ALIGNMENT(buf_ctrl->mps);

	buf_ctrl->remain_size = 0;

	buf_ctrl->isoc_buf = (u8 *)usb_os_malloc(block_size * buf_list_cnt);
	if (buf_ctrl->isoc_buf == NULL) {
		return ret;
	}

	buf_ctrl->buf_list_node = (usbh_uac_buf_t *)usb_os_malloc(sizeof(usbh_uac_buf_t) * buf_list_cnt);
	if (buf_ctrl->buf_list_node == NULL) {
		usb_os_mfree(buf_ctrl->isoc_buf);
		return ret;
	}

	usbh_uac_lock_list_init(&(buf_ctrl->free_buf_lock_list));
	usbh_uac_lock_list_init(&(buf_ctrl->ready_buf_lock_list));

	for (idx = 0; idx < buf_list_cnt; idx++) {
		pbuf_data = &(buf_ctrl->buf_list_node[idx]);
		pbuf_data->buf_len = 0;
		pbuf_data->buf = buf_ctrl->isoc_buf + block_size * idx;
		INIT_LIST_HEAD(&pbuf_data->list);
		usbh_uac_lock_list_add_tail(&pbuf_data->list, &(buf_ctrl->free_buf_lock_list));
	}

	// check
	if (list_empty(&buf_ctrl->free_buf_lock_list.list)) {
		usb_os_mfree(buf_ctrl->isoc_buf);
		usb_os_mfree(buf_ctrl->buf_list_node);
		return ret;
	}

	rtos_sema_create(&(buf_ctrl->isoc_sema), 0U, 1U);
	buf_ctrl->sema_valid = 1;

	ret = HAL_OK;
	return ret;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init uac class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_uac_init(usbh_uac_cb_t *cb, int frame_cnt)
{
	int ret;
	usbh_uac_host_t *uac = &usbh_uac_host;
	usbh_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out);

	usb_os_memset(uac, 0x00, sizeof(usbh_uac_host_t));
	buf_ctrl->frame_cnt = frame_cnt;

	uac->audio_fmt_ctrl_buf = (u8 *)usb_os_malloc(UBSH_UAC_AUDIO_FMT_FREQ_LEN);
	if (NULL == uac->audio_fmt_ctrl_buf) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc mem %d fail\n", UBSH_UAC_AUDIO_FMT_FREQ_LEN);
		return HAL_ERR_MEM;
	}

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
	uac->state = USBH_UAC_IDLE;

#if USBH_UAC_DEBUG
	if (rtos_task_create(&(uac->dump_status_task), ((const char *)"usbh_uac_status_dump_thread"), usbh_uac_status_dump_thread, NULL, 1024U,
						 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create usb status dump task fail\n");
	}
#endif

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
	usbh_uac_ep_cfg_t *ep = &uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info;

#if USBH_UAC_DEBUG
	if (uac->dump_status_task_alive) {
		rtos_task_delete(uac->dump_status_task);
		uac->dump_status_task_alive = 0;
	}
#endif

	usbh_unregister_class(&usbh_uac_driver);
	uac->state = USBH_UAC_IDLE;
	usbh_uac_stop_play();

	if ((uac->cb != NULL) && (uac->cb->deinit != NULL)) {
		uac->cb->deinit();
	}

	usbh_uac_deinit_all_pipe();
	usbh_uac_ep_buf_ctrl_deinit(&(uac->isoc_out), ep);

	if (uac->audio_fmt_ctrl_buf != NULL) {
		usb_os_mfree(uac->audio_fmt_ctrl_buf);
		uac->audio_fmt_ctrl_buf = NULL;
	}

	return ret;
}

/**
  * @brief  Get index of audio interface by direction.
  * @param  dir: 0 for OUT, non-zero for IN.
  * @param  intf_idx: Output pointer for interface index.
  * @retval HAL_OK if found, HAL_ERR_PARA otherwise.
  */
int usbh_uac_find_intf_idx_by_dir(int dir, int *intf_idx)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	usbh_uac_as_t *as_intf;
	int i;

	if (intf_idx != NULL) {
		for (i = 0; i < USBH_UAC_ISOC_NUM; i++) {
			as_intf = &uac->uac_desc.as_intf[i];

			if (((dir == 0) && (as_intf->msg_type == USBH_UAC_ISOC_OUT_IDX)) ||
				((dir != 0) && (as_intf->msg_type == USBH_UAC_ISOC_IN_IDX))) {
				*intf_idx = i;
				return HAL_OK;
			}
		}
	}

	return HAL_ERR_PARA;
}

/**
  * @brief  Set a matched alt_setting
  * @param  dir: Dir of the audio interface, 0 menas out interface, 1 means in interface
  * @param  channels: Channels
  * @param  byte_width: byte_width
  * @param  sampling_freq: sampling_freq
  * @retval Status
  */
int usbh_uac_set_alt_setting(u8 dir, u8 channels, u8 byte_width, u32 sampling_freq)
{
	struct usb_audio_endpoint_descriptor *audio_ep = NULL;
	struct uac_format_type_i_discrete_descriptor *fmt;
	usbh_uac_setting_t *cur_setting = NULL;
	usbh_uac_host_t *uac = &usbh_uac_host;
	usbh_uac_as_t *as_intf = NULL;
	usbh_uac_ep_cfg_t *ep = NULL;
	usb_host_t *host = uac->host;
	int ret = HAL_ERR_PARA;
	int set_flag = 0;
	int frame_ms;
	int intf_idx;
	int i, j;
	u32 freq;
	u8 intf_num, alt_num;

	//find matched as interface idx
	ret = usbh_uac_find_intf_idx_by_dir(dir, &intf_idx);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Find intr err\n");
		return ret;
	}

	as_intf = &uac->uac_desc.as_intf[intf_idx];
	if (as_intf == NULL || as_intf->alt_setting == NULL) {
		ret = HAL_ERR_PARA;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Find as err\n");
		return ret;
	}

	intf_num = as_intf->intf_num;
	alt_num = as_intf->alt_count;
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Search alt %d/%d\n", intf_num, alt_num);

	//actuaclly search from alt 1
	for (i = 0; i < alt_num; i++) {
		fmt = as_intf->alt_setting[i].format;

		// Check format type, channels, and sample frequency
		if (!fmt || fmt->bBitResolution != byte_width || fmt->bNrChannels != channels) {
			continue;
		}

		// Check sample frequency
		for (j = 0; j < fmt->bSamFreqType; j++) {
			freq = (fmt->tSamFreq[j][2] << 16) |
				   (fmt->tSamFreq[j][1] << 8) |
				   fmt->tSamFreq[j][0];
			if (freq == sampling_freq) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Alt found %d/%d %d\n",
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

	cur_setting = &(uac->cur_setting[dir]);
	if (set_flag) {
		//update cur_setting
		cur_setting->freq_id = j;
		cur_setting->intf_num = intf_num;
		cur_setting->alt_setting = (i + 1);

		//update ep_info
		ep = &cur_setting->ep_info;
		audio_ep = as_intf->alt_setting[i].audio_ep;
		if (!audio_ep) {
			return ret;
		}

		ep->isoc_ep_addr = audio_ep->bEndpointAddress;
		ep->isoc_interval = audio_ep->bInterval;

		//calculate accurate one frame size(byte)
		frame_ms = UBSH_UAC_CAL_FRAME((channels * byte_width * sampling_freq * (audio_ep->bInterval)), (8 * 1000));

		if (frame_ms <= audio_ep->wMaxPacketSize) {
			ep->isoc_packet_size = frame_ms;
		} else {
			// device ep des error, stop transfer
			RTK_LOGS(TAG, RTK_LOG_ERROR, "MPS error: %d < %d\n", audio_ep->wMaxPacketSize, frame_ms);
			return ret;
		}

		usbh_uac_stop_play();
		//reinit pipe
		usbh_uac_deinit_all_pipe();
		ep->isoc_pipe = usbh_alloc_pipe(host, ep->isoc_ep_addr);
		usbh_open_pipe(host, ep->isoc_pipe, ep->isoc_ep_addr, USB_CH_EP_TYPE_ISOC, ep->isoc_packet_size);

		// reinit ring buf
		if (dir == USBH_UAC_ISOC_OUT_IDX) {
			usbh_uac_ep_buf_ctrl_deinit(&(uac->isoc_out), ep);
			ret = usbh_uac_ep_buf_ctrl_init(&(uac->isoc_out), ep);
		}

		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Buf init fail");
			return ret;
		}

		uac->xfer_state = UAC_STATE_CTRL_SETTING;
		if (dir == USBH_UAC_ISOC_OUT_IDX) {
			uac->ctrl_state = UAC_STATE_SET_OUT_ALT;
		} else {
			uac->ctrl_state = UAC_STATE_SET_IN_ALT;
		}

		usbh_notify_class_state_change(host, 0);
		ret = HAL_OK;

	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alt not found %d %d %d\n", channels, byte_width, sampling_freq);
		ret = HAL_ERR_PARA;
	}

	return ret;
}

/**
  * @brief  Get alt setting structure and alt settings num for an interface.
  * @param  dir: Dir of the audio interface, 0 menas out interface, 1 means in interface
  * @param  alt_num: Number of alt settings
  * @retval Pointer to alt setting structure
  */
const usbh_uac_alt_t *usbh_uac_get_alt_setting(u8 dir, u8 *alt_num)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	usbh_uac_as_t *as_intf = NULL;
	int intf_idx = -1;

	if (usbh_uac_find_intf_idx_by_dir(dir, &intf_idx) != HAL_OK) {
		return NULL;
	}

	as_intf = &uac->uac_desc.as_intf[intf_idx];
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
u32 usbh_uac_get_frame_size(u8 dir)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	usbh_uac_ep_cfg_t *ep = &uac->cur_setting[dir].ep_info;

	return ep->isoc_packet_size;
}

/**
  * @brief  Wait for an isochronous buffer to become available and check USB transfer status.
  * @param  pdata_ctrl: Pointer to the USB UAC buffer control structure.
  * @param  timeout_ms:  Time out
  * @retval Status
  */
int usbh_uac_wait_isoc_with_status_check(usbh_uac_buf_ctrl_t *pdata_ctrl, uint32_t timeout_ms)
{
	int ret = HAL_ERR_PARA;
	u32 elapsed = 0;
	u32 wait_time = 0;

	while (elapsed < timeout_ms) {
		if (usbh_uac_usb_status_check() != HAL_OK) {
			pdata_ctrl->write_wait_sema = 0;
			return ret;
		}

		wait_time = (timeout_ms - elapsed > USBH_UAC_WAIT_SLICE_MS) ? USBH_UAC_WAIT_SLICE_MS : (timeout_ms - elapsed);

		pdata_ctrl->write_wait_sema = 1;
		if (rtos_sema_take(pdata_ctrl->isoc_sema, wait_time) == RTK_SUCCESS) {
			pdata_ctrl->write_wait_sema = 0;
			return HAL_OK;
		}

		elapsed += wait_time;
	}

	pdata_ctrl->write_wait_sema = 0;
	return ret;
}

/**
  * @brief  Write Audio data
  * @param  buffer: Pointer to data buffer
  * @param  size: Data length
  * @param  timeout_ms: Time out
  * @retval written len
  */
u32 usbh_uac_write(u8 *buffer, u32 size, u32 timeout_ms)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	usbh_uac_buf_ctrl_t *pdata_ctrl = &(uac->isoc_out);
	u32 written_len = 0;
	u32 try_len, just_written;
	u8 need_wait = 0, last_zero = 0;

	if (pdata_ctrl->mps == 0 || !pdata_ctrl->sema_valid) {
		return 0;
	}

	if (pdata_ctrl->transfer_continue == 0) {
		return 0;
	}

	if (usbh_uac_usb_status_check() != HAL_OK) {
		return 0;
	}

	while (written_len < size && pdata_ctrl->transfer_continue) {
		need_wait = 0;

		if (timeout_ms) {
			if (list_empty(&pdata_ctrl->free_buf_lock_list.list) || last_zero) {
				need_wait = 1;
			}
		} else {
			if (list_empty(&pdata_ctrl->free_buf_lock_list.list) || last_zero) {
				break;
			}
		}

		if (need_wait) {
			if (usbh_uac_wait_isoc_with_status_check(pdata_ctrl, timeout_ms) != HAL_OK) {
				break;
			}
			last_zero = 0;
		}

		try_len = size - written_len;
		just_written = 0;

		usbh_uac_write_ring_buf(pdata_ctrl, buffer + written_len, try_len, &just_written);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Want %u, wrote %u\n", try_len, just_written);

		if (just_written > 0) {
			written_len += just_written;
			last_zero = 0;
		} else {
			//wait sema and retry
			last_zero = 1;
		}
	}

	if (written_len > size) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Pls check the uac write cfg\n");
	}

	return written_len;
}

/**
  * @brief  Start UAC device play
  * @param  void
  * @retval void
  */
void usbh_uac_start_play(void)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	usb_host_t *host = uac->host;
	usbh_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out);
	usbh_uac_ep_cfg_t *ep = &(uac->cur_setting[USBH_UAC_ISOC_OUT_IDX].ep_info);

	//should make sure the process is not in ctrl xfer status
	do {
		usb_os_sleep_ms(1);
	} while (uac->xfer_state == UAC_STATE_CTRL_SETTING);

	buf_ctrl->transfer_continue = 1;
	uac->xfer_state = UAC_STATE_TRANSFER;
	ep->isoc_state = USBH_UAC_TX;
	usbh_notify_class_state_change(host, ep->isoc_pipe);

	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC START\n");
}

/**
  * @brief  Stop UAC device play
  * @param  void
  * @retval void
  */
void usbh_uac_stop_play(void)
{
	usbh_uac_host_t *uac = &usbh_uac_host;
	usbh_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out);

	buf_ctrl->transfer_continue = 0;

	uac->xfer_state = UAC_STATE_IDLE;

	//dorp fragment data
	if (buf_ctrl->remain_size != 0) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Drop remain\n");
		if (buf_ctrl->p_buf) {
			usbh_uac_lock_list_add_tail(&buf_ctrl->p_buf->list, &buf_ctrl->free_buf_lock_list);
			buf_ctrl->p_buf = NULL;
		}
		buf_ctrl->remain_size = 0;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC stop\n");
}
