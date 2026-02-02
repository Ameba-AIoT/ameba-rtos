/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_uvc.h"


/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "UVC";

extern usbh_uvc_host_t uvc_host;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief	Alloc entity memory
  * @param	extrabytes: extra memory except usbh_uvc_entity_t
  * @retval Pointer to alloc entity
  */
static usbh_uvc_entity_t *usbh_uvc_entity_t_alloc(u32 extrabytes)
{
	usbh_uvc_entity_t *entity;

	entity = (usbh_uvc_entity_t *) usb_os_malloc(sizeof(usbh_uvc_entity_t) + extrabytes);
	if (entity == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc entity fail\n");
		return NULL;
	}

	return entity;
}

/**
  * @brief	Free entity memory
  * @param	entity: entity to free
  * @retval None
  */
static void usbh_uvc_entity_t_free(usbh_uvc_entity_t *entity)
{
	usb_os_mfree(entity);
}

/**
  * @brief	Parse entity from given descriptor
  * @param	desc: given descriptor buffer
  * @retval Status
  */
static u8 usbh_uvc_parse_entity(u8 *desc)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_vc_t *vc_intf = &uvc->uvc_desc.vc_intf;
	usbh_uvc_entity_t *entity = NULL;

	switch (desc[2]) {
	case USBH_UVC_VC_HEADER:
		vc_intf->vcheader = (usbh_uvc_vc_header_desc_t *)desc;
		break;

	case USBH_UVC_VC_INPUT_TERMINAL:
		vc_intf->entity_num ++;
		entity = usbh_uvc_entity_t_alloc(0);
		entity->p = desc;
		entity->id = desc[3];
		entity->source[0] = 0;
		entity->type = USBH_UVC_VC_INPUT_TERMINAL;
		break;

	case USBH_UVC_VC_OUTPUT_TERMINAL:
		vc_intf->entity_num ++;
		entity = usbh_uvc_entity_t_alloc(0);
		entity->p = desc;
		entity->id = desc[3];
		entity->source[0] = desc[7];
		entity->type = USBH_UVC_VC_OUTPUT_TERMINAL;
		break;

	case USBH_UVC_VC_SELECTOR_UNIT:
		vc_intf->entity_num ++;
		entity = usbh_uvc_entity_t_alloc(desc[4]);
		entity->p = desc;
		entity->id = desc[3];
		usb_os_memcpy(entity->source, &desc[5], desc[4]);
		entity->type = USBH_UVC_VC_SELECTOR_UNIT;
		break;

	case USBH_UVC_VC_PROCESSING_UNIT:
		vc_intf->entity_num ++;
		entity = usbh_uvc_entity_t_alloc(0);
		entity->p = desc;
		entity->id = desc[3];
		entity->source[0] = desc[4];
		entity->type = USBH_UVC_VC_PROCESSING_UNIT;
		break;

	case USBH_UVC_VC_EXTENSION_UNIT:
		vc_intf->entity_num ++;
		entity = usbh_uvc_entity_t_alloc(desc[21]);
		entity->p = desc;
		entity->id = desc[3];
		usb_os_memcpy(entity->source, &desc[22], desc[21]);
		entity->type = USBH_UVC_VC_EXTENSION_UNIT;
		break;

	case USBH_UVC_VC_ENCODING_UNIT:
		vc_intf->entity_num ++;
		entity = usbh_uvc_entity_t_alloc(0);
		entity->p = desc;
		entity->id = desc[3];
		entity->source[0] = desc[4];
		entity->type = USBH_UVC_VC_ENCODING_UNIT;
		break;

	default:
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Wrong entity type");
		break;
	}

	if (entity) {
		list_add_tail((struct list_head *)&entity->list, &uvc->entity_list);
	}

	return HAL_OK;
}

/**
  * @brief	Parse video control interface
  * @param	itf_data: interface array buffer
  * @retval Status
  */
static u8 usbh_uvc_parse_vc(usbh_itf_data_t *itf_data)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	u8 *desc;
	u8 type;
	u8 len;
	int ret;
	usbh_uvc_vc_t *vc_intf = &uvc->uvc_desc.vc_intf;
	u16 itf_total_len = 0;

	desc = itf_data->raw_data;
	//save the first interface number
	vc_intf->p = desc;
	vc_intf->bInterfaceNumber = desc[2];

	while (1) {
		if (desc == NULL || itf_total_len >= itf_data->raw_data_len) {
			break;
		}

		/* Find next descripter */
		len = ((usbh_desc_header_t *) desc)->bLength;
		desc += len;
		itf_total_len += len;

		type = ((usbh_desc_header_t *) desc)->bDescriptorType;
		switch (type) {
		case USBH_UVC_DESC_TYPE_CS_INTERFACE:
			ret = usbh_uvc_parse_entity((u8 *)desc);
			if (ret) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to parse entity\n");
				return ret;
			}
			break;

		case USB_DESC_TYPE_ENDPOINT:
			if (((usbh_ep_desc_t *)desc)->bmAttributes == USB_CH_EP_TYPE_INTR) {
				vc_intf->intr_ep = desc;
			} else {
				RTK_LOGS(TAG, RTK_LOG_WARN, "Wrong endpoint type\n");
			}
			break;

		//class-specific VC interrupt endpoint descriptor
		case USBH_UVC_DESC_TYPE_CS_ENDPOINT:
			if (((usbh_uvc_vc_intr_ep_desc_t *)desc)->bDescriptorType == 0x25 && \
				((usbh_uvc_vc_intr_ep_desc_t *)desc)->bDescriptorSubType == USB_CH_EP_TYPE_INTR) {
				vc_intf->cs_intr_desc = desc;
			} else {
				RTK_LOGS(TAG, RTK_LOG_WARN, "Wrong cs vc intr desc\n");
			}
			break;

		case USB_DESC_TYPE_INTERFACE:
			return HAL_OK;

		default:
			RTK_LOGS(TAG, RTK_LOG_WARN, "Wrong desc type: %d\n", type);
			return HAL_OK;
		}
	}
	return HAL_OK;
}

/**
  * @brief	Parse video format
  * @param	vs_intf: pointer of video streaming interface
  * @param  desc: given descriptor buffer
  * @param  ength: lenghth of given buffer
  * @retval Status
  */
static u8 usbh_uvc_parse_format(usbh_uvc_vs_t *vs_intf, u8 *pbuf, u16 *length)
{
	usbh_uvc_vs_frame_t *frame = NULL;
	usbh_uvc_vs_frame_t *tmp_frame = NULL;
	u8 *desc = pbuf;
	u32 nformat = 0;
	u32 nframe_mjepg = 0, nframe_uncomp = 0, nframe_framebased = 0;
	u32 index = -1;
	u32 parsed_frame_num = 0;
	u16 real_len = 0;
	u16 len = 0;
	u16 totallen = 0;

	if (desc[2] != USBH_UVC_VS_INPUT_HEADER) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Header is no vs input\n");
		return HAL_ERR_PARA;
	}

	if (*length < 6) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Desc too short\n");
		return HAL_ERR_PARA;
	}

	vs_intf->InputHeader = (usbh_uvc_vs_input_header_desc_t *) desc;
	totallen = (u16)desc[4] | (u16)((u16)desc[5] << 8);

	/*first scan to get total number of format and frame*/
	while (1) {
		if ((u32)desc[1] != USBH_UVC_DESC_TYPE_CS_INTERFACE) {
			break;
		}
		switch ((u32)desc[2]) {
		case USBH_UVC_VS_INPUT_HEADER:
		case USBH_UVC_VS_STILL_IMAGE_FRAME:
		case USBH_UVC_VS_COLORFORMAT:
			break;

		case USBH_UVC_VS_FORMAT_UNCOMPRESSED:
			nformat++;
			break;

		case USBH_UVC_VS_FORMAT_MJPEG:
			nformat++;
			break;

		case USBH_UVC_VS_FORMAT_FRAME_BASED:
			/* Check GUID {34363248-0000-0010-8000-00AA00389B71} */
			if ((desc[5] == 0x48) && \
				(desc[6] == 0x32) && \
				(desc[7] == 0x36) && \
				(desc[8] == 0x34) && \
				(desc[9] == 0x00) && \
				(desc[10] == 0x00) && \
				(desc[11] == 0x10) && \
				(desc[12] == 0x00) && \
				(desc[13] == 0x80) && \
				(desc[14] == 0x00) && \
				(desc[15] == 0x00) && \
				(desc[16] == 0xAA) && \
				(desc[17] == 0x00) && \
				(desc[18] == 0x38) && \
				(desc[19] == 0x9B) && \
				(desc[20] == 0x71)) {
				vs_intf->has_h264 = 1;
			} else {
				vs_intf->has_h264 = 0;
				RTK_LOGS(TAG, RTK_LOG_ERROR, "GUID is not H264, not support\n");
			}
			nformat++;
			break;

		case USBH_UVC_VS_FRAME_MJPEG:
			nframe_mjepg ++;
			break;

		case USBH_UVC_VS_FRAME_UNCOMPRESSED:
			nframe_uncomp ++;
			break;

		case USBH_UVC_VS_FRAME_FRAME_BASED:
			nframe_framebased ++;
			break;

		default:
			RTK_LOGS(TAG, RTK_LOG_WARN, "Unsupported vs format desc");
			break;
		}
		/*find next descripter*/
		real_len += (u16)desc[0];
		desc = pbuf + real_len;
	}

	desc = pbuf;
	len = 0;

	vs_intf->nformat = nformat;
	vs_intf->format = (usbh_uvc_vs_format_t *) usb_os_malloc(nformat * sizeof(usbh_uvc_vs_format_t) + \
					  (nframe_mjepg + nframe_uncomp + nframe_framebased) * sizeof(usbh_uvc_vs_frame_t));
	if (vs_intf->format == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Malloc format buf fai\n");
		return HAL_ERR_MEM;
	}

	tmp_frame = (usbh_uvc_vs_frame_t *)((u8 *)vs_intf->format + nformat * sizeof(usbh_uvc_vs_format_t));

	while (len < real_len) {
		switch ((u32)desc[2]) {
		case USBH_UVC_VS_INPUT_HEADER:
			break;

		case USBH_UVC_VS_FORMAT_MJPEG:
		case USBH_UVC_VS_FORMAT_UNCOMPRESSED:
		case USBH_UVC_VS_FORMAT_FRAME_BASED:
			index ++;
			vs_intf->format[index].frame = (usbh_uvc_vs_frame_t *)((u8 *)vs_intf->format + \
										   nformat * sizeof(usbh_uvc_vs_format_t) + \
										   parsed_frame_num * sizeof(usbh_uvc_vs_frame_t));
			vs_intf->format[index].type = desc[2];
			vs_intf->format[index].index = desc[3];
			//vs_intf->format[index].bpp = desc[21];
			break;

		case USBH_UVC_VS_FRAME_UNCOMPRESSED:
		case USBH_UVC_VS_FRAME_MJPEG:
			vs_intf->format[index].nframes ++;
			frame = (usbh_uvc_vs_frame_t *)((u8 *)tmp_frame + parsed_frame_num * sizeof(usbh_uvc_vs_frame_t));
			frame->bFrameIndex = desc[3];
			frame->bmCapabilities = desc[4];
			frame->wWidth = *(u16 *)(desc + 5);
			frame->wHeight = *(u16 *)(desc + 7);
			frame->dwMinBitRate = *(u32 *)(desc + 9);
			frame->dwMaxBitRate = *(u32 *)(desc + 13);
			frame->dwMaxVideoFrameBufferSize = *(u32 *)(desc + 17);
			frame->dwDefaultFrameInterval = *(u32 *)(desc + 21);
			frame->bFrameIntervalType = desc[25];
			frame->dwFrameInterval = (u32 *)&desc[26];
			parsed_frame_num ++;
			break;

		case USBH_UVC_VS_FRAME_FRAME_BASED:
			vs_intf->format[index].nframes ++;
			frame = (usbh_uvc_vs_frame_t *)((u8 *)tmp_frame + parsed_frame_num * sizeof(usbh_uvc_vs_frame_t));
			frame->bFrameIndex = desc[3];
			frame->bmCapabilities = desc[4];
			frame->wWidth = *(u16 *)(desc + 5);
			frame->wHeight = *(u16 *)(desc + 7);
			frame->dwMinBitRate = *(u32 *)(desc + 9);
			frame->dwMaxBitRate = *(u32 *)(desc + 13);
			frame->dwDefaultFrameInterval = *(u32 *)(desc + 17);
			frame->bFrameIntervalType = desc[21];
			frame->dwBytesPerLine = *(u32 *)(desc + 22);
			frame->dwFrameInterval = (u32 *)&desc[26];
			parsed_frame_num ++;
			break;

		case USBH_UVC_VS_STILL_IMAGE_FRAME:
			break;

		case USBH_UVC_VS_COLORFORMAT:
			break;

		default:
			RTK_LOGS(TAG, RTK_LOG_WARN, "Unsupported vs format len");
			break;

		}

		/*find next descripter*/
		len += (u16)desc[0];
		desc = pbuf + len;
	}

	/* Some camera wTotalLength from vs input desc is not right, we use real len */
	if (real_len != totallen) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Invalid itf size 0x%x, should be 0x%x\n", totallen, real_len);
	}

	*length = real_len;

	return HAL_OK;
}

/**
  * @brief	Parse video streaming interface
  * @param	itf_data: interface array buffer
  * @retval Status
  */
static u8 usbh_uvc_parse_vs(usbh_itf_data_t *itf_data)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_vs_t *vs_intf = &uvc->uvc_desc.vs_intf[uvc->uvc_desc.vs_num];
	uvc->stream[uvc->uvc_desc.vs_num].vs_intf = vs_intf;
	uvc->uvc_desc.vs_num++;
	u8 bAlternateSetting;
	u8 *desc;
	u8 type;
	u16 len;
	u16 itf_total_len = 0;

	if (uvc->uvc_desc.vs_num > USBH_UVC_VS_DESC_MAX_NUM) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Too many VS itf %d-%d\n", uvc->uvc_desc.vs_num, USBH_UVC_VS_DESC_MAX_NUM);
		return HAL_OK;
	}

	desc = itf_data->raw_data;
	//save the first interface number
	vs_intf->p = desc;
	vs_intf->bInterfaceNumber = desc[2];

	/*find next descripter*/
	len = ((usbh_desc_header_t *) desc)->bLength;
	desc += len;
	itf_total_len += len;

	while (1) {
		if (desc == NULL || itf_total_len >= itf_data->raw_data_len) {
			break;
		}

		type = ((usbh_desc_header_t *) desc)->bDescriptorType;
		switch (type) {
		case USBH_UVC_DESC_TYPE_CS_INTERFACE:
			usbh_uvc_parse_format(vs_intf, desc, &len);
			desc += len;
			break;

		case USB_DESC_TYPE_INTERFACE:
			if (((usbh_itf_desc_t *)desc)->bInterfaceNumber != vs_intf->bInterfaceNumber) { //find another itf, maybe it is the as itf, should return
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "VC intf new %d:old %d, return\n\n", ((usbh_itf_desc_t *)desc)->bInterfaceNumber, vs_intf->bInterfaceNumber);
				return HAL_OK;
			}
			bAlternateSetting = ((usbh_itf_desc_t *)desc)->bAlternateSetting;
			if (bAlternateSetting != 0) {
				if (bAlternateSetting < USBH_UVC_VS_ALTS_MAX_NUM) {
					vs_intf->altsetting[bAlternateSetting - 1].p = desc;
					vs_intf->alt_num++;

					len = ((usbh_desc_header_t *) desc)->bLength;
					desc += len;
					vs_intf->altsetting[bAlternateSetting - 1].endpoint = (usbh_ep_desc_t *)desc;
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "Too many alt set %d-%d\n", bAlternateSetting, USBH_UVC_VS_ALTS_MAX_NUM);
				}
			} else {
				return HAL_OK;
			}
			len = ((usbh_desc_header_t *) desc)->bLength;
			desc += len;
			break;

		default:
			len = ((usbh_desc_header_t *) desc)->bLength;
			desc += len;
			break;
		}
		itf_total_len += len;
	}
	return HAL_OK;
}

/**
  * @brief	Parse configuration descriptor
  * @param	host: usb host structure
  * @retval Status
  */
int usbh_uvc_parse_cfgdesc(usb_host_t *host)
{
	usbh_itf_data_t *itf_data;
	int ret = HAL_ERR_UNKNOWN;
	usbh_dev_id_t dev_id = {0,};

	dev_id.bInterfaceClass = USBH_UVC_CLASS_CODE;
	dev_id.bInterfaceSubClass = USBH_UVC_SUBCLASS_VIDEOCONTROL;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);
	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get vc itf fail\n");
		return ret;
	} else {
		ret = usbh_uvc_parse_vc(itf_data);
		if (ret) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Parse vc fail\n");
			return ret;
		}
	}

	dev_id.bInterfaceClass = USBH_UVC_CLASS_CODE;
	dev_id.bInterfaceSubClass = USBH_UVC_SUBCLASS_VIDEOSTREAMING;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);

	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No VS itf found\n");
		return HAL_ERR_UNKNOWN;
	}

	while (itf_data) {
		ret = usbh_uvc_parse_vs(itf_data);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Parse vs fail, itf=%d\n",
					 itf_data->itf_desc_array[0].bInterfaceNumber);
			return ret;
		}
		itf_data = itf_data->next;
	}
	return HAL_OK;
}

/**
  * @brief	Init uvc descriptor related
  * @param	None
  * @retval None
  */
void usbh_uvc_desc_init(void)
{
	usbh_uvc_host_t *uvc = &uvc_host;

	INIT_LIST_HEAD(&uvc->entity_list);
}

/**
  * @brief	Deinit uvc descriptor related
  * @param	None
  * @retval None
  */
void usbh_uvc_desc_deinit(void)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	struct list_head *p, *n;
	usbh_uvc_entity_t *ent;
	usbh_uvc_vs_t *vs_intf;
	int i;

	list_for_each_safe(p, n, &uvc->entity_list) {
		ent = list_entry(p, usbh_uvc_entity_t, list);
		usbh_uvc_entity_t_free(ent);
	}

	for (i = 0; i < uvc->uvc_desc.vs_num; i++) {
		vs_intf = uvc->stream[i].vs_intf;
		usb_os_mfree(vs_intf->format);
	}
}

