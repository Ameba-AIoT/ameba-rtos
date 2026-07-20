/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_uvc.h"
#include "usbh_uvc_parse.h"


/* Private defines -----------------------------------------------------------*/
#define USBH_UVC_BCD_UVC_1_1                   (0x0110U)  /**< UVC 1.1 release number. */
#define USBH_UVC_BCD_UVC_1_5                   (0x0150U)  /**< UVC 1.5 release number. */
#define USBH_UVC_PROBE_SIZE_UVC10              (26U)      /**< Probe/Commit control size for UVC 1.0. */
#define USBH_UVC_PROBE_SIZE_UVC11              (34U)      /**< Probe/Commit control size for UVC 1.1. */
#define USBH_UVC_PROBE_SIZE_UVC15              (48U)      /**< Probe/Commit control size for UVC 1.5. */

/* UVC descriptor fixed-length fields */
#define USBH_UVC_PARSE_SU_FIXED_LEN              (5U)
#define USBH_UVC_PARSE_XU_FIXED_LEN              (22U)
#define USBH_UVC_PARSE_FRAME_DESC_HEADER_LEN     (26U)
#define USBH_UVC_PARSE_FRAME_BASED_FMT_MIN_LEN   (21U)
#define USBH_UVC_PARSE_INTERVAL_ENTRY_SIZE       (4U)
#define USBH_UVC_PARSE_CONTINUOUS_INTV_COUNT     (3U)
#define USBH_UVC_PARSE_4BYTE_ALIGN_MASK          (3U)
/* Frame-based guidFormat FourCC discriminator byte (desc[8]): H.264 and H.265
 * share the "H26x" prefix and differ only here ('4' vs '5'). */
#define USBH_UVC_PARSE_FOURCC_H264               (0x34U)
#define USBH_UVC_PARSE_FOURCC_H265               (0x35U)

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
static usbh_uvc_entity_t *usbh_uvc_entity_alloc(u32 extrabytes)
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
static void usbh_uvc_entity_free(usbh_uvc_entity_t *entity)
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
		vc_intf->vcheader = (usbh_uvc_vc_header_desc_t *)(void *)desc;
		break;

	case USBH_UVC_VC_INPUT_TERMINAL:
		vc_intf->entity_num ++;
		entity = usbh_uvc_entity_alloc(0);
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
		entity = usbh_uvc_entity_alloc(0);
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
		entity = usbh_uvc_entity_alloc(desc[4]);
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
		entity = usbh_uvc_entity_alloc(0);
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
		entity = usbh_uvc_entity_alloc(desc[21]);
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
		entity = usbh_uvc_entity_alloc(0);
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

	if (entity != NULL) {
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
	if (desc == NULL) {
		return HAL_ERR_HW;
	}
	//save the first interface number
	vc_intf->p = desc;
	vc_intf->bInterfaceNumber = desc[2];

	while (1) {
		if (itf_total_len >= itf_data->raw_data_len) {
			break;
		}

		/* Find next descriptor */
		len = ((usbh_desc_header_t *)(void *) desc)->bLength;

		desc += len;
		itf_total_len += len;

		/* Stop before reading past the end of the descriptor buffer */
		if (itf_total_len >= itf_data->raw_data_len) {
			break;
		}
		type = ((usbh_desc_header_t *)(void *) desc)->bDescriptorType;
		switch (type) {
		case USBH_UVC_DESC_TYPE_CS_INTERFACE:
			ret = usbh_uvc_parse_entity((u8 *)desc);
			if (ret != HAL_OK) {
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
  * @brief  Map a frame-based VS Format descriptor to its H.264/H.265 selector
  * @param  desc: pointer to the VS_FORMAT_FRAME_BASED descriptor
  * @note   H.264 GUID {34363248-...} and H.265 GUID {35363248-...} differ only
  *         at byte 8 ('4' vs '5'). Unknown GUID keeps the raw subtype.
  * @retval USBH_UVC_FORMAT_H264 / USBH_UVC_FORMAT_H265 / desc[2]
  */
static u8 usbh_uvc_frame_based_fmt_type(const u8 *desc)
{
	u8 fmt = desc[2]; /* default: keep raw frame-based subtype */

	if ((desc[5] == 0x48U) && (desc[6] == 0x32U) && (desc[7] == 0x36U) &&
		(desc[9] == 0x00U) && (desc[10] == 0x00U) && (desc[11] == 0x10U) &&
		(desc[12] == 0x00U) && (desc[13] == 0x80U) && (desc[14] == 0x00U) &&
		(desc[15] == 0x00U) && (desc[16] == 0xAAU) && (desc[17] == 0x00U) &&
		(desc[18] == 0x38U) && (desc[19] == 0x9BU) && (desc[20] == 0x71U)) {
		if (desc[8] == USBH_UVC_PARSE_FOURCC_H264) {
			fmt = USBH_UVC_FORMAT_H264;
		}
		if (desc[8] == USBH_UVC_PARSE_FOURCC_H265) {
			fmt = USBH_UVC_FORMAT_H265;
		}
	}

	return fmt;
}

/**
  * @brief	Parse video format
  * @param	vs_intf: pointer of video streaming interface
  * @param  desc: given descriptor buffer
  * @param  length: length of given buffer
  * @retval Status
  */
static int usbh_uvc_parse_format(usbh_uvc_vs_t *vs_intf, u8 *pbuf, u16 *length)
{
	usbh_uvc_vs_frame_t *frame = NULL;
	usbh_uvc_vs_frame_t *tmp_frame = NULL;
	u8 *desc = pbuf;
	u32 format_num = 0;
	u32 mjpeg_frame_num = 0;
	u32 uncomp_frame_num = 0;
	u32 framebased_frame_num = 0;
	u32 total_interval_num = 0;
	u32 format_idx = 0;
	u32 parsed_frame_num = 0, interval_num = 0;
	u32 total_frame_num = 0;
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

	if (*length < 6U) {
		//RTK_LOGS(TAG, RTK_LOG_DEBUG, "Desc too short\n");
		return HAL_ERR_PARA;
	}

	vs_intf->InputHeader = (usbh_uvc_vs_input_header_desc_t *)(void *) desc;
	totallen = (u16)(desc[4] | (desc[5] << 8));

	/*first scan to get total number of format and frame*/
	while (1) {
		if (desc[1] != USBH_UVC_DESC_TYPE_CS_INTERFACE) {
			break;
		}
		if (desc[0] == 0U) {
			/* Zero-length descriptor: malformed; stop to avoid an infinite loop
			 * (real_len would never advance). Mirrors the guard in parse_vs. */
			break;
		}
		switch (desc[2]) {
		case USBH_UVC_VS_INPUT_HEADER:
		case USBH_UVC_VS_STILL_IMAGE_FRAME:
		case USBH_UVC_VS_COLORFORMAT:
			break;

		case USBH_UVC_VS_FORMAT_UNCOMPRESSED:
			format_num++;
			break;

		case USBH_UVC_VS_FORMAT_MJPEG:
			format_num++;
			break;

		case USBH_UVC_VS_FORMAT_FRAME_BASED:
			/* H.264/H.265 both use frame-based; counted here and told apart
			 * by guidFormat when stored (see usbh_uvc_frame_based_fmt_type). */
			format_num++;
			break;

		case USBH_UVC_VS_FRAME_MJPEG:
			mjpeg_frame_num ++;
			/* bFrameIntervalType == 0 means continuous (3 entries); else discrete */
			total_interval_num += (desc[25] == 0U) ? USBH_UVC_PARSE_CONTINUOUS_INTV_COUNT : desc[25];
			break;

		case USBH_UVC_VS_FRAME_UNCOMPRESSED:
			uncomp_frame_num ++;
			total_interval_num += (desc[25] == 0U) ? USBH_UVC_PARSE_CONTINUOUS_INTV_COUNT : desc[25];
			break;

		case USBH_UVC_VS_FRAME_FRAME_BASED:
			framebased_frame_num ++;
			/* frame-based: bFrameIntervalType at offset 21 */
			total_interval_num += (desc[21] == 0U) ? USBH_UVC_PARSE_CONTINUOUS_INTV_COUNT : desc[21];
			break;

		default:
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Err fmt dec");
			break;
		}
		/*find next descriptor*/
		real_len += desc[0];
		desc = pbuf + real_len;
	}

	desc = pbuf;
	len = 0U;

	vs_intf->format_num = format_num;
	total_frame_num = mjpeg_frame_num + uncomp_frame_num + framebased_frame_num;
	/* usbh_uvc_vs_frame_t is __PACKED (31 bytes), so total_frame_num*31 may not be
	 * 4-byte aligned. Round up to avoid misaligned u32 access in interval_store. */
	frame_array_size = (total_frame_num * sizeof(usbh_uvc_vs_frame_t) + USBH_UVC_PARSE_4BYTE_ALIGN_MASK) & ~USBH_UVC_PARSE_4BYTE_ALIGN_MASK;
	alloc_size = (format_num * sizeof(usbh_uvc_vs_format_t)) + \
				 frame_array_size + \
				 (total_interval_num * sizeof(u32));
	vs_intf->format = (usbh_uvc_vs_format_t *) usb_os_malloc(alloc_size);
	if (vs_intf->format == NULL) {
		return HAL_ERR_MEM;
	}
	tmp_frame = (usbh_uvc_vs_frame_t *)(void *)(vs_intf->format + format_num);
	interval_store = (u32 *)(void *)((u8 *)(void *)tmp_frame + frame_array_size);

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
			vs_intf->format[format_idx].frame = &tmp_frame[parsed_frame_num];
			vs_intf->format[format_idx].type = desc[2];
			vs_intf->format[format_idx].index = desc[3];
			//vs_intf->format[format_idx].bpp = desc[21];
			format_idx++;
			break;

		case USBH_UVC_VS_FORMAT_FRAME_BASED:
			vs_intf->format[format_idx].frame = &tmp_frame[parsed_frame_num];
			/* Frame-based carries H.264 or H.265; map guidFormat to the matching
			 * selector so find_format_frame() picks the correct bFormatIndex. */
			vs_intf->format[format_idx].type = usbh_uvc_frame_based_fmt_type(desc);
			vs_intf->format[format_idx].index = desc[3];
			format_idx++;
			break;

		case USBH_UVC_VS_FRAME_UNCOMPRESSED:
		case USBH_UVC_VS_FRAME_MJPEG:
			if (format_idx == 0U) {
				break;
			}
			vs_intf->format[format_idx - 1U].frame_num ++;
			frame = &tmp_frame[parsed_frame_num];
			frame->bFrameIndex = desc[3];
			frame->bmCapabilities = desc[4];
			frame->wWidth = ReadEF2Byte(desc + 5);
			frame->wHeight = ReadEF2Byte(desc + 7);
			frame->dwMinBitRate = ReadEF4Byte(desc + 9);
			frame->dwMaxBitRate = ReadEF4Byte(desc + 13);
			frame->dwMaxVideoFrameBufferSize = ReadEF4Byte(desc + 17);
			frame->dwDefaultFrameInterval = ReadEF4Byte(desc + 21);
			frame->bFrameIntervalType = desc[25];
			interval_num = (desc[25] == 0U) ? USBH_UVC_PARSE_CONTINUOUS_INTV_COUNT : desc[25];
			frame->dwFrameInterval = interval_store;
			usb_os_memcpy(interval_store, &desc[26], interval_num * sizeof(u32));
			interval_store += interval_num;
			parsed_frame_num ++;
			break;

		case USBH_UVC_VS_FRAME_FRAME_BASED:
			if (format_idx == 0U) {
				break;
			}
			vs_intf->format[format_idx - 1U].frame_num ++;
			frame = &tmp_frame[parsed_frame_num];
			frame->bFrameIndex = desc[3];
			frame->bmCapabilities = desc[4];
			frame->wWidth = ReadEF2Byte(desc + 5);
			frame->wHeight = ReadEF2Byte(desc + 7);
			frame->dwMinBitRate = ReadEF4Byte(desc + 9);
			frame->dwMaxBitRate = ReadEF4Byte(desc + 13);
			frame->dwDefaultFrameInterval = ReadEF4Byte(desc + 17);
			frame->bFrameIntervalType = desc[21];
			frame->dwBytesPerLine = ReadEF4Byte(desc + 22);
			interval_num = (desc[21] == 0U) ? USBH_UVC_PARSE_CONTINUOUS_INTV_COUNT : desc[21];
			frame->dwFrameInterval = interval_store;
			usb_os_memcpy(interval_store, &desc[26], interval_num * sizeof(u32));
			interval_store += interval_num;
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

		/*find next descriptor*/
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
	len = ((usbh_desc_header_t *)(void *) desc)->bLength;
	desc += len;
	itf_total_len += len;

	while (1) {
		if (itf_total_len >= itf_data->raw_data_len) {
			break;
		}

		type = ((usbh_desc_header_t *)(void *) desc)->bDescriptorType;
		len = ((usbh_desc_header_t *)(void *) desc)->bLength;

		if (len == 0U) {
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
			if (((usbh_itf_desc_t *)(void *)desc)->bInterfaceNumber != vs_intf->bInterfaceNumber) { //find another itf, maybe it is the as itf, should return
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "VC intf %d-%d\n\n", ((usbh_itf_desc_t *)(void *)desc)->bInterfaceNumber, vs_intf->bInterfaceNumber);
				return HAL_OK;
			}
			bAlternateSetting = ((usbh_itf_desc_t *)(void *)desc)->bAlternateSetting;
			if (bAlternateSetting != 0U) {
				if (bAlternateSetting < USBH_UVC_VS_ALTS_MAX_NUM) {
					vs_intf->altsetting[bAlternateSetting - 1].p = desc;
					vs_intf->alt_num++;

					next_desc = desc + len;
					if ((itf_total_len + len) < itf_data->raw_data_len) {
						if (((usbh_desc_header_t *)(void *)next_desc)->bDescriptorType == USB_DESC_TYPE_ENDPOINT) {
							vs_intf->altsetting[bAlternateSetting - 1].endpoint = (usbh_ep_desc_t *)(void *)next_desc;
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
  * @brief  Get Probe/Commit control wLength based on bcdUVC (UVC spec Table 4-75)
  * @param  bcdUVC: UVC device release number (e.g., 0x0100, 0x0110, 0x0150)
  * @retval 26 (UVC 1.0) / 34 (UVC 1.1) / 48 (UVC 1.5)
  */
u16 usbh_uvc_get_ctrl_len_by_version(u16 bcdUVC)
{
	if (bcdUVC >= USBH_UVC_BCD_UVC_1_5) {
		return USBH_UVC_PROBE_SIZE_UVC15;
	}
	return (bcdUVC >= USBH_UVC_BCD_UVC_1_1) ? USBH_UVC_PROBE_SIZE_UVC11 : USBH_UVC_PROBE_SIZE_UVC10;
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

	while (itf_data != NULL) {
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
  * @brief  Find suitable format and frame index
  * @param  stream: uvc stream interface
  * @param  context: user configuration (width/height/fmt_type)
  * @param  format_idx: pointer to format index (output)
  * @param  frame_idx: pointer to frame index (output)
  * @retval Status
  */
int usbh_uvc_desc_find_format_frame(usbh_uvc_stream_t *stream, usbh_uvc_s_ctx_t *context,
									u32 *format_idx, u32 *frame_idx)
{
	usbh_uvc_vs_format_t *format;
	usbh_uvc_vs_frame_t  *frame;
	usbh_uvc_vs_frame_t  *best;
	usbh_uvc_vs_t *vs;
	u32 w;
	u32 h;
	u32 i;
	u32 dist;
	u32 best_dist;
	u32 dw;
	u32 dh;

	if ((context == NULL) || (format_idx == NULL) || (frame_idx == NULL)) {
		return HAL_ERR_PARA;
	}

	vs = stream->vs_intf;
	if ((vs == NULL) || (vs->format == NULL)) {
		return HAL_ERR_PARA;
	}

	w = context->width;
	h = context->height;

	/* Step 1: locate format by subtype. */
	format = NULL;
	for (i = 0U; i < vs->format_num; i++) {
		if (vs->format[i].type == context->fmt_type) {
			format = &vs->format[i];
			break;
		}
	}
	if ((format == NULL) || (format->frame == NULL)) {
		return HAL_ERR_PARA;
	}

	/* Step 2: sync Linux UVC, pick frame minimizing |Δw| + |Δh|. */
	best = NULL;
	best_dist = UINT32_MAX;

	for (i = 0U; i < format->frame_num; i++) {
		frame = &format->frame[i];

		dw = (frame->wWidth > w) ? (frame->wWidth - w) : (w - frame->wWidth);
		dh = (frame->wHeight > h) ? (frame->wHeight - h) : (h - frame->wHeight);
		dist = dw + dh;

		if (dist < best_dist) {
			best_dist = dist;
			best = frame;
			if (dist == 0U) {
				break;/* exact match */
			}
		}
	}

	if (best == NULL) {
		return HAL_ERR_PARA;
	}

	*format_idx = format->index;
	*frame_idx = best->bFrameIndex;

	/* Write negotiated values back so the caller learns what was actually selected */
	context->width  = best->wWidth;
	context->height = best->wHeight;

	return HAL_OK;
}

/**
  * @brief	Find suitable frame rate
  * @param context: user configuration
  * @param interval: pointer to bInterval value
  * @param format_idx: pointer to format index
  * @param frame_idx: pointer to frame index
  * @retval Status
  */
int usbh_uvc_desc_find_frame_rate(usbh_uvc_stream_t *stream, usbh_uvc_s_ctx_t *context,
								  u32 *interval, u32 *format_idx, u32 *frame_idx)
{
	usbh_uvc_vs_format_t *format;
	usbh_uvc_vs_frame_t  *frame;
	u32 i;
	u32 fps;
	u32 cur;
	u32 dist;
	u32 best;
	u32 min_interval;
	u32 max_interval;
	u32 step;

	if ((context == NULL) || (interval == NULL) ||
		(format_idx == NULL) || (frame_idx == NULL)) {
		return HAL_ERR_PARA;
	}
	if (context->frame_rate == 0U) {
		return HAL_ERR_PARA;
	}

	/* Guard against NULL vs_intf (e.g. called before attach or after detach). */
	if ((stream->vs_intf == NULL) || (stream->vs_intf->format == NULL)) {
		return HAL_ERR_PARA;
	}

	/* Locate format. */
	format = NULL;
	for (i = 0U; i < stream->vs_intf->format_num; i++) {
		if (stream->vs_intf->format[i].index == *format_idx) {
			format = &stream->vs_intf->format[i];
			break;
		}
	}
	if (format == NULL) {
		return HAL_ERR_PARA;
	}

	/* Locate frame. */
	frame = NULL;
	for (i = 0U; i < format->frame_num; i++) {
		if (format->frame[i].bFrameIndex == *frame_idx) {
			frame = &format->frame[i];
			break;
		}
	}
	if (frame == NULL) {
		return HAL_ERR_PARA;
	}

	fps = USBH_UVC_FRAME_INTERVAL_UNIT_NS / context->frame_rate;

	if (frame->bFrameIntervalType != 0U) {
		/* Discrete: dwFrameInterval[] sorted ascending. */
		best = (u32) - 1;
		for (i = 0U; i < frame->bFrameIntervalType; i++) {
			cur  = frame->dwFrameInterval[i];
			dist = ((fps > cur) ? (fps - cur) : (cur - fps));

			if (dist > best) {
				break;
			}
			best = dist;
		}
		/* Guard i == 0 (single-entry list / immediate-break edge). */
		fps = frame->dwFrameInterval[(i == 0U) ? 0U : (i - 1U)];
	} else {
		/* Continuous: { min, max, step }. */
		min_interval = frame->dwFrameInterval[0];
		max_interval = frame->dwFrameInterval[1];
		step = frame->dwFrameInterval[2];

		if (fps < min_interval) {
			fps = min_interval;
		} else if (fps > max_interval) {
			fps = max_interval;
		} else if (step == 0U) {
			fps = min_interval;
		} else {
			fps = min_interval + ((((fps - min_interval) + (step / 2U)) / step) * step);
			if (fps > max_interval) {
				fps = max_interval;
			}
		}
	}

	/* Guard against a malformed descriptor reporting a 0 interval (div-by-zero). */
	if (fps == 0U) {
		return HAL_ERR_PARA;
	}

	*interval = fps;
	context->frame_rate = USBH_UVC_FRAME_INTERVAL_UNIT_NS / fps;
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
		usbh_uvc_entity_free(ent);
	}

	INIT_LIST_HEAD(&uvc->entity_list);
}