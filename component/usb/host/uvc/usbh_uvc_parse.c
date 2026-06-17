/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_uvc.h"


/* Private defines -----------------------------------------------------------*/

/* UVC descriptor fixed-length fields */
#define USBH_UVC_PARSE_SU_FIXED_LEN              (5U)
#define USBH_UVC_PARSE_XU_FIXED_LEN              (22U)
#define USBH_UVC_PARSE_FRAME_DESC_HEADER_LEN     (26U)
#define USBH_UVC_PARSE_FRAME_BASED_FMT_MIN_LEN   (21U)
#define USBH_UVC_PARSE_INTERVAL_ENTRY_SIZE       (4U)
#define USBH_UVC_PARSE_CONTINUOUS_INTV_COUNT     (3U)
#define USBH_UVC_PARSE_4BYTE_ALIGN_MASK          (3U)

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
static int usbh_uvc_parse_entity(u8 *desc)
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
		if (entity == NULL) {
			return HAL_ERR_MEM;
		}
		entity->p = desc;
		entity->id = desc[3];
		entity->source[0] = 0;
		entity->type = USBH_UVC_VC_INPUT_TERMINAL;

		break;

	case USBH_UVC_VC_OUTPUT_TERMINAL:
		vc_intf->entity_num ++;
		entity = usbh_uvc_entity_t_alloc(0);
		if (entity == NULL) {
			return HAL_ERR_MEM;
		}
		entity->p = desc;
		entity->id = desc[3];
		entity->source[0] = desc[7];
		entity->type = USBH_UVC_VC_OUTPUT_TERMINAL;
		break;

	case USBH_UVC_VC_SELECTOR_UNIT:
		vc_intf->entity_num ++;
		entity = usbh_uvc_entity_t_alloc(desc[4]);
		if (entity == NULL) {
			return HAL_ERR_MEM;
		}
		entity->p = desc;
		entity->id = desc[3];
		usb_os_memcpy(entity->source, &desc[5], desc[4]);
		entity->type = USBH_UVC_VC_SELECTOR_UNIT;
		break;

	case USBH_UVC_VC_PROCESSING_UNIT:
		vc_intf->entity_num ++;
		entity = usbh_uvc_entity_t_alloc(0);
		if (entity == NULL) {
			return HAL_ERR_MEM;
		}
		entity->p = desc;
		entity->id = desc[3];
		entity->source[0] = desc[4];
		entity->type = USBH_UVC_VC_PROCESSING_UNIT;
		break;

	case USBH_UVC_VC_EXTENSION_UNIT:
		vc_intf->entity_num ++;
		entity = usbh_uvc_entity_t_alloc(desc[21]);
		if (entity == NULL) {
			return HAL_ERR_MEM;
		}
		entity->p = desc;
		entity->id = desc[3];
		usb_os_memcpy(entity->source, &desc[22], desc[21]);
		entity->type = USBH_UVC_VC_EXTENSION_UNIT;
		break;

	case USBH_UVC_VC_ENCODING_UNIT:
		vc_intf->entity_num ++;
		entity = usbh_uvc_entity_t_alloc(0);
		if (entity == NULL) {
			return HAL_ERR_MEM;
		}
		entity->p = desc;
		entity->id = desc[3];
		entity->source[0] = desc[4];
		entity->type = USBH_UVC_VC_ENCODING_UNIT;
		break;

	default:
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Wrong entity t");
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
static int usbh_uvc_parse_vc(usbh_itf_data_t *itf_data)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_vc_t *vc_intf = NULL;
	u8 *desc;
	int ret;
	u16 itf_total_len = 0;
	u8 type;
	u8 len;

	vc_intf = &uvc->uvc_desc.vc_intf;

	desc = itf_data->raw_data;
	//save the first interface number
	if (vc_intf == NULL) {
		//RTK_LOGS(TAG, RTK_LOG_ERROR, "vc_intf null %x\n", vc_intf);
		return HAL_OK;
	}

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

		if (desc == NULL) {
			//RTK_LOGS(TAG, RTK_LOG_ERROR, "desc null %x\n", desc);
			break;
		}
		type = ((usbh_desc_header_t *) desc)->bDescriptorType;
		switch (type) {
		case USBH_UVC_DESC_TYPE_CS_INTERFACE:
			ret = usbh_uvc_parse_entity((u8 *)desc);
			if (ret) {
				//RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to parse entity\n");
				return ret;
			}
			break;

		case USB_DESC_TYPE_ENDPOINT:
			break;

		//class-specific VC interrupt endpoint descriptor, not support
		case USBH_UVC_DESC_TYPE_CS_ENDPOINT:
			break;

		case USB_DESC_TYPE_INTERFACE:
			return HAL_OK;

		default:
			RTK_LOGS(TAG, RTK_LOG_WARN, "Wrong desc t:%d\n", type);
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
static int usbh_uvc_parse_format(usbh_uvc_vs_t *vs_intf, u8 *pbuf, u16 *length)
{
	usbh_uvc_vs_frame_t *frame = NULL;
	usbh_uvc_vs_frame_t *tmp_frame = NULL;
	u8 *desc = pbuf;
	u32 nformat = 0;
	u32 nframe_mjepg = 0;
	u32 nframe_uncomp = 0;
	u32 nframe_framebased = 0;
	u32 ninterval_total = 0;
	int index = -1;
	u32 parsed_frame_num = 0, nintv = 0;
	u32 nframe_total = 0;
	u32 frame_array_size = 0;
	u32 alloc_size = 0;
	u32 *interval_store = NULL;
	u16 real_len = 0;
	u16 len = 0;
	u16 totallen = 0;


	if (desc[2] != USBH_UVC_VS_INPUT_HEADER) {
		//RTK_LOGS(TAG, RTK_LOG_DEBUG, "Header is no vs input\n");
		return HAL_ERR_PARA;
	}

	if (*length < 6) {
		//RTK_LOGS(TAG, RTK_LOG_DEBUG, "Desc too short\n");
		return HAL_ERR_PARA;
	}

	vs_intf->InputHeader = (usbh_uvc_vs_input_header_desc_t *) desc;
	totallen = (u16)(desc[4] | (desc[5] << 8));

	/*first scan to get total number of format and frame*/
	while (1) {
		if (desc[1] != USBH_UVC_DESC_TYPE_CS_INTERFACE) {
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
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Unsupported GUID\n");
			}
			nformat++;
			break;

		case USBH_UVC_VS_FRAME_MJPEG:
			nframe_mjepg ++;
			/* bFrameIntervalType == 0 means continuous (3 entries); else discrete */
			ninterval_total += (desc[25] == 0) ? USBH_UVC_PARSE_CONTINUOUS_INTV_COUNT : desc[25];
			break;

		case USBH_UVC_VS_FRAME_UNCOMPRESSED:
			nframe_uncomp ++;
			ninterval_total += (desc[25] == 0) ? USBH_UVC_PARSE_CONTINUOUS_INTV_COUNT : desc[25];
			break;

		case USBH_UVC_VS_FRAME_FRAME_BASED:
			nframe_framebased ++;
			/* frame-based: bFrameIntervalType at offset 21 */
			ninterval_total += (desc[21] == 0) ? USBH_UVC_PARSE_CONTINUOUS_INTV_COUNT : desc[21];
			break;

		default:
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Err fmt dec");
			break;
		}
		/*find next descripter*/
		real_len += desc[0];
		desc = pbuf + real_len;
	}

	desc = pbuf;
	len = 0;

	vs_intf->nformat = nformat;
	nframe_total = nframe_mjepg + nframe_uncomp + nframe_framebased;
	/* usbh_uvc_vs_frame_t is __PACKED (31 bytes), so nframe_total*31 may not be
	 * 4-byte aligned. Round up to avoid misaligned u32 access in interval_store. */
	frame_array_size = (nframe_total * sizeof(usbh_uvc_vs_frame_t) + USBH_UVC_PARSE_4BYTE_ALIGN_MASK) & ~USBH_UVC_PARSE_4BYTE_ALIGN_MASK;
	alloc_size = nformat * sizeof(usbh_uvc_vs_format_t) + \
				 frame_array_size + \
				 ninterval_total * sizeof(u32);
	vs_intf->format = (usbh_uvc_vs_format_t *) usb_os_malloc(alloc_size);
	if (vs_intf->format == NULL) {
		return HAL_ERR_MEM;
	}
	usb_os_memset(vs_intf->format, 0, alloc_size);
	tmp_frame = (usbh_uvc_vs_frame_t *)((u8 *)vs_intf->format + nformat * sizeof(usbh_uvc_vs_format_t));
	interval_store = (u32 *)((u8 *)tmp_frame + frame_array_size);

	while (len < real_len) {
		if (desc[1] != USBH_UVC_DESC_TYPE_CS_INTERFACE) {
			len += desc[0];
			desc = pbuf + len;
			continue;
		}

		switch (desc[2]) {
		case USBH_UVC_VS_INPUT_HEADER:
			break;

		case USBH_UVC_VS_FORMAT_MJPEG:
		case USBH_UVC_VS_FORMAT_UNCOMPRESSED:
		case USBH_UVC_VS_FORMAT_FRAME_BASED:
			index ++;
			if (index < 0) {
				break;
			}
			vs_intf->format[index].frame = (usbh_uvc_vs_frame_t *)((u8 *)vs_intf->format + \
										   nformat * sizeof(usbh_uvc_vs_format_t) + \
										   parsed_frame_num * sizeof(usbh_uvc_vs_frame_t));
			vs_intf->format[index].type = desc[2];
			vs_intf->format[index].index = desc[3];
			//vs_intf->format[index].bpp = desc[21];
			break;

		case USBH_UVC_VS_FRAME_UNCOMPRESSED:
		case USBH_UVC_VS_FRAME_MJPEG:
			if (index < 0) {
				break;
			}
			vs_intf->format[index].nframes ++;
			frame = (usbh_uvc_vs_frame_t *)((u8 *)tmp_frame + parsed_frame_num * sizeof(usbh_uvc_vs_frame_t));
			frame->bFrameIndex = desc[3];
			frame->bmCapabilities = desc[4];
			frame->wWidth = ReadEF2Byte(desc + 5);
			frame->wHeight = ReadEF2Byte(desc + 7);
			frame->dwMinBitRate = ReadEF4Byte(desc + 9);
			frame->dwMaxBitRate = ReadEF4Byte(desc + 13);
			frame->dwMaxVideoFrameBufferSize = ReadEF4Byte(desc + 17);
			frame->dwDefaultFrameInterval = ReadEF4Byte(desc + 21);
			frame->bFrameIntervalType = desc[25];
			nintv = (desc[25] == 0u) ? USBH_UVC_PARSE_CONTINUOUS_INTV_COUNT : desc[25];
			frame->dwFrameInterval = interval_store;
			usb_os_memcpy(interval_store, &desc[26], nintv * sizeof(u32));
			interval_store += nintv;
			parsed_frame_num ++;
			break;

		case USBH_UVC_VS_FRAME_FRAME_BASED:
			if (index < 0) {
				break;
			}
			vs_intf->format[index].nframes ++;
			frame = (usbh_uvc_vs_frame_t *)((u8 *)tmp_frame + parsed_frame_num * sizeof(usbh_uvc_vs_frame_t));
			frame->bFrameIndex = desc[3];
			frame->bmCapabilities = desc[4];
			frame->wWidth = ReadEF2Byte(desc + 5);
			frame->wHeight = ReadEF2Byte(desc + 7);
			frame->dwMinBitRate = ReadEF4Byte(desc + 9);
			frame->dwMaxBitRate = ReadEF4Byte(desc + 13);
			frame->dwDefaultFrameInterval = ReadEF4Byte(desc + 17);
			frame->bFrameIntervalType = desc[21];
			frame->dwBytesPerLine = ReadEF4Byte(desc + 22);
			nintv = (desc[21] == 0u) ? USBH_UVC_PARSE_CONTINUOUS_INTV_COUNT : desc[21];
			frame->dwFrameInterval = interval_store;
			usb_os_memcpy(interval_store, &desc[26], nintv * sizeof(u32));
			interval_store += nintv;
			parsed_frame_num ++;
			break;

		case USBH_UVC_VS_STILL_IMAGE_FRAME:
			break;

		case USBH_UVC_VS_COLORFORMAT:
			break;

		default:
			RTK_LOGS(TAG, RTK_LOG_WARN, "Err fmt len");
			break;

		}

		/*find next descripter*/
		len += desc[0];
		desc = pbuf + len;
	}

	/* Some camera wTotalLength from vs input desc is not right, we use real len */
	if (real_len != totallen) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Invalid itf size 0x%x-0x%x\n", totallen, real_len);
	}

	*length = real_len;

	return HAL_OK;
}

/**
  * @brief	Parse video streaming interface
  * @param	itf_data: interface array buffer
  * @retval Status
  */
static int usbh_uvc_parse_vs(usbh_itf_data_t *itf_data)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	usbh_uvc_vs_t *vs_intf = NULL;
	u8 *desc = NULL;
	u8 *next_desc = NULL;
	int ret;
	u16 len;
	u16 itf_total_len = 0;
	u8 bAlternateSetting;
	u8 type;
	u8 vs_num = uvc->uvc_desc.vs_num;


	if (vs_num >= USBH_UVC_VS_DESC_MAX_NUM) {
		/* Already at max VS capacity — skip this extra VS interface.
		 * The first VS interface will still work normally. */
		if (vs_num == USBH_UVC_VS_DESC_MAX_NUM) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Ovrl vs %d>%d, using 1st vs\n",
					 vs_num + 1, USBH_UVC_VS_DESC_MAX_NUM);
		}
		return HAL_OK;
	}

	vs_intf = &uvc->uvc_desc.vs_intf[vs_num];

	usb_os_memset(vs_intf, 0, sizeof(usbh_uvc_vs_t));
	uvc->stream[vs_num].vs_intf = vs_intf;
	vs_num++;
	uvc->uvc_desc.vs_num = vs_num;

	desc = itf_data->raw_data;
	/* save the first interface number (Alt 0) */
	vs_intf->p = desc;
	vs_intf->bInterfaceNumber = desc[2];

	/* Skip the first descriptor (Interface Alt 0) */
	len = ((usbh_desc_header_t *) desc)->bLength;
	desc += len;
	itf_total_len += len;

	while (1) {
		if (desc == NULL || itf_total_len >= itf_data->raw_data_len) {
			break;
		}

		type = ((usbh_desc_header_t *) desc)->bDescriptorType;
		len = ((usbh_desc_header_t *) desc)->bLength;

		if (len == 0) {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "ZL desc\n");
			break;
		}

		switch (type) {
		case USBH_UVC_DESC_TYPE_CS_INTERFACE:
			ret = usbh_uvc_parse_format(vs_intf, desc, &len);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Parse format err: %d\n", ret);
				return ret;
			}
			desc += len;
			break;

		case USB_DESC_TYPE_INTERFACE:
			if (((usbh_itf_desc_t *)desc)->bInterfaceNumber != vs_intf->bInterfaceNumber) { //find another itf, maybe it is the as itf, should return
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "VC intf %d-%d\n\n", ((usbh_itf_desc_t *)desc)->bInterfaceNumber, vs_intf->bInterfaceNumber);
				return HAL_OK;
			}
			bAlternateSetting = ((usbh_itf_desc_t *)desc)->bAlternateSetting;
			if (bAlternateSetting != 0) {
				if (bAlternateSetting < USBH_UVC_VS_ALTS_MAX_NUM) {
					vs_intf->altsetting[bAlternateSetting - 1].p = desc;
					vs_intf->alt_num++;

					next_desc = desc + len;
					if ((itf_total_len + len) < itf_data->raw_data_len) {
						if (((usbh_desc_header_t *)next_desc)->bDescriptorType == USB_DESC_TYPE_ENDPOINT) {
							vs_intf->altsetting[bAlternateSetting - 1].endpoint = (usbh_ep_desc_t *)next_desc;
						}
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "Ovrl alt %d>%d\n",
							 bAlternateSetting, USBH_UVC_VS_ALTS_MAX_NUM);
				}
			} else {
				return HAL_OK;
			}
			desc += len;
			break;

		default:
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
	int ret = HAL_ERR_PARA;
	usbh_dev_id_t dev_id = {0,};

	dev_id.bInterfaceClass = USBH_UVC_CLASS_CODE;
	dev_id.bInterfaceSubClass = USBH_UVC_SUBCLASS_VIDEOCONTROL;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);
	if (itf_data == NULL) {
		//RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get vc itf fail\n");
		return ret;
	} else {
		ret = usbh_uvc_parse_vc(itf_data);
		if (ret != HAL_OK) {
			//RTK_LOGS(TAG, RTK_LOG_DEBUG, "Parse vc fail\n");
			return ret;
		}
	}

	dev_id.bInterfaceClass = USBH_UVC_CLASS_CODE;
	dev_id.bInterfaceSubClass = USBH_UVC_SUBCLASS_VIDEOSTREAMING;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);

	if (itf_data == NULL) {
		//RTK_LOGS(TAG, RTK_LOG_DEBUG, "No VS itf\n");
		return HAL_ERR_PARA;
	}

	while (itf_data) {
		ret = usbh_uvc_parse_vs(itf_data);
		if (ret != HAL_OK) {
			//RTK_LOGS(TAG, RTK_LOG_DEBUG, "Parse vs%d fail%d\n",
			//		 itf_data->itf_desc_array[0].bInterfaceNumber);
			return ret;
		}
		itf_data = itf_data->next;
	}
	return HAL_OK;
}

/**
  * @brief	Init uvc descriptor related
  * @param	None
  * @retval Status
  */
int usbh_uvc_desc_init(void)
{
	usbh_uvc_host_t *uvc = &uvc_host;

	INIT_LIST_HEAD(&uvc->entity_list);

	uvc->uvc_desc.vc_intf.entity_num = 0;
	return HAL_OK;
}

/**
  * @brief	Deinit uvc descriptor related
  * @param	None
  * @retval None
  */
void usbh_uvc_desc_deinit(void)
{
	usbh_uvc_host_t *uvc = &uvc_host;
	struct list_head *p;
	struct list_head *n;
	usbh_uvc_entity_t *ent;

	list_for_each_safe(p, n, &uvc->entity_list) {
		ent = list_entry(p, usbh_uvc_entity_t, list);
		list_del(p);
		usbh_uvc_entity_t_free(ent);
	}

	INIT_LIST_HEAD(&uvc->entity_list);
}

