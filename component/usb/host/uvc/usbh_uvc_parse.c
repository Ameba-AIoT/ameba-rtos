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
  * @brief	Find next standard descriptor
  * @param	pbuf: given buffer to find
  			len: given buffer length
  * @retval Pointer of next standard descriptor
  */
static u8 *usbh_uvc_find_next_stdesc(u8 *pbuf, u32 *len)
{
	u8 *desc = pbuf;

	while (*len > 0) {
		if (((usbh_uvc_desc_header_t *)desc)->bDescriptorType == USB_DESC_TYPE_INTERFACE) {
			return (u8 *) desc;
		} else {
			*len -= ((usbh_uvc_desc_header_t *)desc)->bLength;
			desc += ((usbh_uvc_desc_header_t *)desc)->bLength;
		}
	}

	return NULL;
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

	return 0;
}

/**
  * @brief	Parse video control interface
  * @param	desc: given descriptor buffer
  			length: lenghth of given buffer
  * @retval Status
  */
static u8 usbh_uvc_parse_vc(u8 *pbuf, u32 *length)
{
	u8 *desc = pbuf;
	int ret;
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_vc_t *vc_intf = &uvc->uvc_desc.vc_intf;

	vc_intf->p = desc;
	vc_intf->bInterfaceNumber = desc[2];
	*length = 0;

	while (1) {
		/*find next descripter*/
		*length += ((usbh_uvc_desc_header_t *) desc)->bLength;
		desc = pbuf + *length;

		switch (((usbh_uvc_desc_header_t *) desc)->bDescriptorType) {
		case USB_DESC_TYPE_CS_INTERFACE:
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

		case USB_DESC_TYPE_CS_ENDPOINT:		//class-specific VC interrupt endpoint descriptor
			if (((usbh_uvc_vc_intr_ep_desc_t *)desc)->bDescriptorType == 0x25 && \
				((usbh_uvc_vc_intr_ep_desc_t *)desc)->bDescriptorSubType == USB_CH_EP_TYPE_INTR) {
				vc_intf->cs_intr_desc = desc;
			} else {
				RTK_LOGS(TAG, RTK_LOG_WARN, "Wrong cs vc intr desc\n");
			}
			break;

		case USB_DESC_TYPE_INTERFACE:
			return 0;

		default:
			RTK_LOGS(TAG, RTK_LOG_WARN, "Wrong desc type: %d\n", ((usbh_uvc_desc_header_t *) desc)->bDescriptorType);
			return 0;
		}
	}

}

/**
  * @brief	Parse video format
  * @param	vs_intf: pointer of video streaming interface
  			desc: given descriptor buffer
  			length: lenghth of given buffer
  * @retval Status
  */
static u8 usbh_uvc_parse_format(usbh_uvc_vs_t *vs_intf, u8 *pbuf, u16 *length)
{
	u8 *desc = pbuf;
	u16 len = 0;
	u16 totallen;
	u32 nformat = 0;
	u32 nframe_mjepg = 0, nframe_uncomp = 0, nframe_framebased = 0;
	u32 index = -1;
	u32 parsed_frame_num = 0;
	u32 real_len;
	usbh_uvc_vs_frame_t *frame;
	usbh_uvc_vs_frame_t *tmp_frame;

	if (desc[2] != USBH_UVC_VS_INPUT_HEADER) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Header is no vs input\n");
		return HAL_ERR_PARA;
	}

	vs_intf->InputHeader = (usbh_uvc_vs_input_header_desc_t *) desc;
	totallen = ((usbh_uvc_vs_input_header_desc_t *) desc)->wTotalLength;

	/*first scan to get total number of format and frame*/
	while (1) {
		if (desc[1] != USB_DESC_TYPE_CS_INTERFACE) {
			break;
		}
		switch (desc[2]) {
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
		len += ((usbh_uvc_desc_header_t *) desc)->bLength;
		real_len = len;
		desc = pbuf + len;
	}

	desc = pbuf;
	len = 0;

	vs_intf->nformat = nformat;
	vs_intf->format = (usbh_uvc_vs_format_t *) usb_os_malloc(nformat * sizeof(usbh_uvc_vs_format_t) + \
					  (nframe_mjepg + nframe_uncomp + nframe_framebased) * sizeof(usbh_uvc_vs_frame_t));

	tmp_frame = (usbh_uvc_vs_frame_t *)((u8 *)vs_intf->format + nformat * sizeof(usbh_uvc_vs_format_t));

	while (len < real_len) {
		switch (desc[2]) {
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

		case USBH_UVC_VS_STILL_IMAGE_FRAME:
			break;

		case USBH_UVC_VS_COLORFORMAT:
			break;

		default:
			RTK_LOGS(TAG, RTK_LOG_WARN, "Unsupported vs format len");
			break;

		}

		/*find next descripter*/
		len += ((usbh_uvc_desc_header_t *) desc)->bLength;
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
  * @param	pbuf: given descriptor buffer
  			length: lenghth of given buffer
  * @retval Status
  */
static u8 usbh_uvc_parse_vs(u8 *pbuf, u32 *length)
{
	u8 *desc = pbuf;
	u16 len = 0;
	u8 bAlternateSetting;
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_vs_t *vs_intf = &uvc->uvc_desc.vs_intf[uvc->uvc_desc.vs_num];
	uvc->stream[uvc->uvc_desc.vs_num].vs_intf = vs_intf;
	uvc->uvc_desc.vs_num++;

	if (uvc->uvc_desc.vs_num > USBH_MAX_NUM_VS_DESC) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "too much VS itf %d-%d\n", uvc->uvc_desc.vs_num > USBH_MAX_NUM_VS_DESC);
	}

	vs_intf->p = desc;
	vs_intf->bInterfaceNumber = desc[2];
	*length = 0;
	/*find next descripter*/
	*length += ((usbh_uvc_desc_header_t *) desc)->bLength;
	desc = pbuf + *length;

	while (1) {
		switch (((usbh_uvc_desc_header_t *) desc)->bDescriptorType) {
		case USB_DESC_TYPE_CS_INTERFACE:
			usbh_uvc_parse_format(vs_intf, desc, &len);
			desc = desc + len;
			*length += len;
			break;

		case USB_DESC_TYPE_INTERFACE:
			bAlternateSetting = ((usbh_if_desc_t *)desc)->bAlternateSetting;
			if (bAlternateSetting != 0) {
				if (bAlternateSetting < USBH_MAX_NUM_VS_ALTS) {
					vs_intf->altsetting[bAlternateSetting - 1].p = desc;
					vs_intf->alt_num++;

					len = ((usbh_uvc_desc_header_t *) desc)->bLength;
					*length += len;
					desc = desc + len;
					vs_intf->altsetting[bAlternateSetting - 1].endpoint = (usbh_ep_desc_t *)desc;
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "too much alt set %d-%d\n", bAlternateSetting, USBH_MAX_NUM_VS_ALTS);
				}

				len = ((usbh_uvc_desc_header_t *) desc)->bLength;
				desc = desc + len;
				*length += len;
			} else {
				return 0;
			}
			break;

		default:
			RTK_LOGS(TAG, RTK_LOG_ERROR, "bDescriptorType: %d\n", ((usbh_uvc_desc_header_t *) desc)->bDescriptorType);
			return 0;
		}
	}
}

/**
  * @brief	Parse configuration descriptor
  * @param	host: usb host structure
  * @retval Status
  */
int usbh_uvc_parse_cfgdesc(usb_host_t *host)
{
	int ret = HAL_OK;
	usbh_uvc_desc_header_t *desc = (usbh_uvc_desc_header_t *)usbh_get_active_raw_configuration_descriptor(host);
	u32 cfglen = (u32)((usbh_cfg_desc_t *) desc)->wTotalLength;
	usbh_if_desc_t *pbuf = (usbh_if_desc_t *) desc;
	u32 len;

	while (1) {
		pbuf = (usbh_if_desc_t *)usbh_uvc_find_next_stdesc((u8 *)pbuf, &cfglen);
		if (pbuf == NULL) {
			break;
		}

		if (pbuf->bInterfaceClass == USBH_UVC_CLASS_CODE) {
			switch (pbuf->bInterfaceSubClass) {
			case USB_SUBCLASS_VIDEOCONTROL:
				ret = usbh_uvc_parse_vc((u8 *)pbuf, &len);
				if (ret) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "UVC parse video ctrl fail\n");
					return ret;
				}
				pbuf = (usbh_if_desc_t *)((u8 *) pbuf + len);
				cfglen -= len;
				break;

			case USB_SUBCLASS_VIDEOSTREAMING:
				if (pbuf->bAlternateSetting == 0) {
					ret = usbh_uvc_parse_vs((u8 *)pbuf, &len);
					if (ret) {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "UVC parse video stream fail\n");
						return ret;
					}

					pbuf = (usbh_if_desc_t *)((u8 *) pbuf + len);
					cfglen -= len;
				}
				break;

			default:
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Subclass(%d) is not VC or VS\n", pbuf->bInterfaceClass);
				return HAL_ERR_PARA;

			}
		} else {
			//skip non-uvc descriptor
			cfglen -= ((usbh_uvc_desc_header_t *)desc)->bLength;
			pbuf = (usbh_if_desc_t *)((u8 *) pbuf + ((usbh_uvc_desc_header_t *)desc)->bLength);
		}
	}

	return ret;

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

