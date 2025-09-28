/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/
#include "usbh_composite_hid.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#define UBSH_COMPOSITE_HID_CTRL_BUF_LEN        512U
#define UBSH_COMPOSITE_HID_IDLE_MAX_CNT        10U
#define UBSH_COMPOSITE_HID_TRIGGER_MAX_CNT     50U

#if USBH_COMPOSITE_HID_UAC_DEBUG
#define USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG       0
#else
#define USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG       0
#endif

/* Private function prototypes -----------------------------------------------*/
#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
static const char *usbh_composite_hid_get_usage_page_name(u32 usage_page);
static const char *usbh_composite_hid_get_usage_name(uint32_t usage_page, uint32_t usage);
static void usbh_composite_hid_decode_data_attributes(uint32_t data);
static void usbh_composite_hid_print_item(const usbh_composite_hid_item_t *item);
#endif
static const u8 *usbh_composite_hid_fetch_item(const u8 *start, const u8 *end, usbh_composite_hid_item_t *item);
static void usbh_composite_hid_process_global_item(usbh_composite_hid_parse_state *state, const usbh_composite_hid_item_t *item);
static void usbh_composite_hid_process_usage(usbh_composite_hid_parse_state *state, u32 usage);
static void usbh_composite_hid_process_local_item(usbh_composite_hid_parse_state *state, const usbh_composite_hid_item_t *item);
static void usbh_composite_hid_process_main_item(usbh_composite_hid_parse_state *state, const usbh_composite_hid_item_t *item);
static void usbh_composite_hid_parse_hid_report_descriptor(const u8 *data, u8 length, usbh_composite_hid_ctrl_caps_t *device_info);
static int usbh_composite_hid_parse_hid_report(const u8 *report_data, u8 report_len, const usbh_composite_hid_ctrl_caps_t *device_info);
static int usbh_composite_hid_parse_hid_report_desc(u8 *pbuf, u32 buf_length);
static void usbh_composite_hid_parse_hid_msg(const u8 *report, u8 len);
static int usbh_composite_hid_parse(u8 *pbuf, u32 *length, u32 buf_length);
static u8 *usbh_composite_hid_find_next_stdesc(u8 *pbuf, u32 *len);
static int usbh_composite_hid_parse_cfgdesc(usb_host_t *host);
static int usbh_composite_hid_process_set_hid_alt(usb_host_t *host);
static int usbh_composite_hid_process_get_hid_report_desc(usb_host_t *host);
static void usbh_composite_hid_in_process(usb_host_t *host);
static int usbh_composite_hid_cb_attach(usb_host_t *host);
static int usbh_composite_hid_cb_detach(usb_host_t *host);
static int usbh_composite_hid_cb_setup(usb_host_t *host);
static int usbh_composite_hid_cb_process(usb_host_t *host, u32 msg);
static int usbh_composite_hid_cb_sof(usb_host_t *host);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "HID";

/* USB Standard Device Descriptor */
const usbh_class_driver_t usbh_composite_hid_driver = {
	.class_code = USBH_CLASS_HID,
	.attach = usbh_composite_hid_cb_attach,
	.detach = usbh_composite_hid_cb_detach,
	.setup = usbh_composite_hid_cb_setup,
	.process = usbh_composite_hid_cb_process,
	.sof = usbh_composite_hid_cb_sof,
};

static usbh_composite_hid_t usbh_composite_hid;

#if USBH_COMPOSITE_HID_UAC_DEBUG
void usbh_composite_hid_status_dump_thread(void)
{
	usbh_composite_hid_t *hid = &usbh_composite_hid;
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "HID cnt %d Type %d\t", hid->event_cnt, hid->last_event.type);
}
#endif

#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
// Usage Page
static const char *usbh_composite_hid_get_usage_page_name(u32 usage_page)
{
	switch (usage_page) {
	case USBH_HID_UP_CONSUMER:
		return "Consumer";
	default:
		return "Unknown";
	}

	return "Unknown";
}

static const char *usbh_composite_hid_get_usage_name(uint32_t usage_page, uint32_t usage)
{
	if (usage_page == USBH_HID_UP_CONSUMER) {
		switch (usage) {
		case USBH_HID_CONSUMER_VOLUME_UP:
			return "Volume Up";
		case USBH_HID_CONSUMER_VOLUME_DOWN:
			return "Volume Down";
		case USBH_HID_CONSUMER_MUTE:
			return "Mute";
		case USBH_HID_CONSUMER_PLAY_PAUSE:
			return "Play/Pause";
		case USBH_HID_CONSUMER_STOP:
			return "Stop";
		default:
			return "Unknown Usage";
		}
	}
	return "Unknown Usage";
}

static void usbh_composite_hid_decode_data_attributes(uint32_t data)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "  Data Attributes: ");
	if (data & 0x1) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Constant ");
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Data ");
	}

	if (data & 0x2) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Variable ");
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Array ");
	}

	if (data & 0x4) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Relative ");
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Absolute ");
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
}

static void usbh_composite_hid_print_item(const usbh_composite_hid_item_t *item)
{
	const char *type_str[] = {"Main", "Global", "Local", "Reserved"};
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Item: Type:%s Tag:0x%x Size:%d Data:0x%08x\t", type_str[item->type], item->tag, item->size, item->data);
}
#endif

static const u8 *usbh_composite_hid_fetch_item(const u8 *start, const u8 *end, usbh_composite_hid_item_t *item)
{
	if (start >= end) {
		return NULL;
	}

	u8 b = *start++;
	item->type = (b >> 2) & 3;
	item->tag = (b >> 4) & 15;
	item->size = b & 3;

	if (item->size == 3) {
		item->size = 4;
	}

	item->data = 0;
	for (int i = 0; i < item->size; i++) {
		if (start >= end) {
			return NULL;
		}
		item->data |= (*start++) << (8 * i);
	}

	return start;
}

static void usbh_composite_hid_process_global_item(usbh_composite_hid_parse_state *state, const usbh_composite_hid_item_t *item)
{
	switch (item->tag) {
	case USBH_HID_GLOBAL_ITEM_TAG_USAGE_PAGE:
		state->usage_page = item->data;
#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Usage Page: 0x%x(%s)\n", item->data, usbh_composite_hid_get_usage_page_name(item->data));
#endif
		break;
	case USBH_HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM:
		state->logical_min = (int32_t)item->data;
#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Logical Minimum: %d\n", state->logical_min);
#endif
		break;
	case USBH_HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM:
		state->logical_max = (int32_t)item->data;
#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Logical Maximum: %d\n", state->logical_max);
#endif
		break;
	case USBH_HID_GLOBAL_ITEM_TAG_REPORT_SIZE:
		state->report_size = item->data;
#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Report Size: %d bits\n", state->report_size);
#endif
		break;
	case USBH_HID_GLOBAL_ITEM_TAG_REPORT_COUNT:
		state->report_count = item->data;
#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Report Count: %d\n", state->report_count);
#endif
		break;
	case USBH_HID_GLOBAL_ITEM_TAG_REPORT_ID:
		state->usage_stack_ptr = 0;
		state->device_info->report_id_count++;

		state->report_id = item->data;
#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Report ID: %d\n", state->report_id);
#endif
		break;
	default:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Unknown Global Item (Tag: 0x%X)\n", item->tag);
		break;
	}
}

static void usbh_composite_hid_process_usage(usbh_composite_hid_parse_state *state, u32 usage)
{
	state->usage_stack_ptr++;

	//consumer
	if (state->usage_page == USBH_HID_UP_CONSUMER) {///always
		if ((usage == USBH_HID_CONSUMER_VOLUME_UP) || (usage == USBH_HID_CONSUMER_VOLUME_DOWN) || (usage == USBH_HID_CONSUMER_MUTE)) {
			state->device_info->volume.supported = true;
			state->device_info->volume.report_id = state->report_id;

			if (usage == USBH_HID_CONSUMER_VOLUME_UP) {
				state->device_info->volume.up_bit = state->usage_stack_ptr - 1;
			} else if (usage == USBH_HID_CONSUMER_VOLUME_DOWN) {
				state->device_info->volume.down_bit = state->usage_stack_ptr - 1;
			} else if (usage == USBH_HID_CONSUMER_MUTE) {
				state->device_info->volume.mute_bit = state->usage_stack_ptr - 1;
			}
		}

		if ((usage == USBH_HID_CONSUMER_PLAY_PAUSE) || (usage == USBH_HID_CONSUMER_STOP)) {
			state->device_info->media.supported = true;
			state->device_info->media.report_id = state->report_id;

			if (usage == USBH_HID_CONSUMER_PLAY_PAUSE) {
				state->device_info->media.play_pause_bit = state->usage_stack_ptr - 1;
			} else if (usage == USBH_HID_CONSUMER_STOP) {
				state->device_info->media.stop_bit = state->usage_stack_ptr - 1;
			}
		}
	}
}

static void usbh_composite_hid_process_local_item(usbh_composite_hid_parse_state *state, const usbh_composite_hid_item_t *item)
{
	switch (item->tag) {
	case USBH_HID_LOCAL_ITEM_TAG_USAGE:
		usbh_composite_hid_process_usage(state, item->data);
#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Usage: 0x%03x (%s)\n", item->data, usbh_composite_hid_get_usage_name(state->usage_page, item->data));
#endif
		break;

	default:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Unknown Local(0x%x)\n", item->tag);
		break;
	}
}

static void usbh_composite_hid_process_main_item(usbh_composite_hid_parse_state *state, const usbh_composite_hid_item_t *item)
{
#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
	u32 total_bits;
	u32 total_bytes;
#endif

	switch (item->tag) {
	case USBH_HID_MAIN_ITEM_TAG_INPUT:
#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Input - ");
		usbh_composite_hid_decode_data_attributes(item->data);
		total_bits = state->report_size * state->report_count;
		total_bytes = (total_bits + 7) / 8;
		if (state->report_id > 0) {
			total_bytes++;
		}
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "  Total bits: %d, Total bytes: %d\n", total_bits, total_bytes);
#endif
		break;

	case USBH_HID_MAIN_ITEM_TAG_COLLECTION:
		state->collection_depth++;
		state->usage_stack_ptr = 0;
#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Collection - Type: %d\n", item->data);
#endif
		break;

	case USBH_HID_MAIN_ITEM_TAG_END_COLLECTION:
		state->collection_depth--;
#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "End Collection\n");
#endif
		break;

	default:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Unknown Main Item (Tag: 0x%x)\n", item->tag);
		break;
	}
}

static void usbh_composite_hid_parse_hid_report_descriptor(const u8 *data, u8 length, usbh_composite_hid_ctrl_caps_t *device_info)
{
	const u8 *end = data + length;
	const u8 *ptr = data;

	usbh_composite_hid_parse_state state = {0};
	usbh_composite_hid_item_t item;

	state.device_info = device_info;
	memset(device_info, 0, sizeof(usbh_composite_hid_ctrl_caps_t));
#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
	int item_count = 0;
#endif

	while ((ptr = usbh_composite_hid_fetch_item(ptr, end, &item)) != NULL) {
#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "[%03d] ", ++item_count);
		usbh_composite_hid_print_item(&item);
#endif

		switch (item.type) {
		case USBH_HID_ITEM_TYPE_GLOBAL:
			usbh_composite_hid_process_global_item(&state, &item);
			break;
		case USBH_HID_ITEM_TYPE_LOCAL:
			usbh_composite_hid_process_local_item(&state, &item);
			break;
		case USBH_HID_ITEM_TYPE_MAIN:
			usbh_composite_hid_process_main_item(&state, &item);
			break;
		default:
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "Reserved Item Type\n");
			break;
		}
	}

#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n=== Volume Control Capabilities Summary ===\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Total reports defined:%d\n", device_info->report_id_count);

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n=== Device Capabilities ===\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Volume control supported: %s\n", device_info->volume.supported ? "Yes" : "No");
	if (device_info->volume.supported) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "  Volume controls in Report ID: %d\n", device_info->volume.report_id);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "  Volume Up: bit %d\n", device_info->volume.up_bit);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "  Volume Down: bit %d\n", device_info->volume.down_bit);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "  Mute: bit %d\n", device_info->volume.mute_bit);
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Media control supported: %s\n", device_info->media.supported ? "Yes" : "No");
	if (device_info->media.supported) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "  Media controls in Report ID: %d\n", device_info->media.report_id);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "  Play/Pause: bit %d\n", device_info->media.play_pause_bit);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "  Stop: bit %d\n", device_info->media.stop_bit);
	}
#endif
}

static int usbh_composite_hid_parse_hid_report(const u8 *report_data, u8 report_len,
		const usbh_composite_hid_ctrl_caps_t *device_info)
{
	usbh_composite_hid_t *hid = &usbh_composite_hid;
	usbh_composite_hid_event_t *event = &(hid->report_event);
	event->type = VOLUME_EVENT_NONE;
	event->is_press = 0;

	// if (report_len == 0 || report_len > device_info->max_report_size)
	if (report_len == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Warning: Invalid report length: %d\n", report_len);
		return HAL_ERR_PARA;
	}

	u8 report_id = 0;
	const u8 *data_start = report_data;
	size_t data_len = report_len;

	/* check report id */
	if (report_len > 1 &&
		(device_info->volume.report_id > 0 || device_info->media.report_id > 0)) {
		report_id = report_data[0];
		data_start = report_data + 1;
		data_len = report_len - 1;
	}

	if (device_info->volume.supported && report_id == device_info->volume.report_id) {
		// volumme up
		u8 byte_idx = device_info->volume.up_bit / 8;
		u8 bit_idx = device_info->volume.up_bit % 8;
		if (byte_idx < data_len && ((data_start[byte_idx] >> bit_idx) & 0x01)) {
			event->type = VOLUME_EVENT_CONSUMER_UP;
			return HAL_OK;
		}

		// volume down
		byte_idx = device_info->volume.down_bit / 8;
		bit_idx = device_info->volume.down_bit % 8;
		if (byte_idx < data_len && ((data_start[byte_idx] >> bit_idx) & 0x01)) {
			event->type = VOLUME_EVENT_CONSUMER_DOWN;
			return HAL_OK;
		}

		// volume mute
		byte_idx = device_info->volume.mute_bit / 8;
		bit_idx = device_info->volume.mute_bit % 8;
		if (byte_idx < data_len && ((data_start[byte_idx] >> bit_idx) & 0x01)) {
			event->type = VOLUME_EVENT_CONSUMER_MUTE;
			return HAL_OK;
		}
	}

	if (device_info->media.supported && report_id == device_info->media.report_id) {

		// pause play
		u8 byte_idx = device_info->media.play_pause_bit / 8;
		u8 bit_idx = device_info->media.play_pause_bit % 8;
		if (byte_idx < data_len && ((data_start[byte_idx] >> bit_idx) & 0x01)) {
			event->type = VOLUME_EVENT_CONSUMER_PLAY_PAUSE;
			return HAL_OK;
		}

		//stop
		byte_idx = device_info->media.stop_bit / 8;
		bit_idx = device_info->media.stop_bit % 8;
		if (byte_idx < data_len && ((data_start[byte_idx] >> bit_idx) & 0x01)) {
			event->type = VOLUME_EVENT_CONSUMER_STOP;
			return HAL_OK;
		}
	}

	return HAL_OK;
}

static int usbh_composite_hid_parse_hid_report_desc(u8 *pbuf, u32 buf_length)
{
	usbh_composite_hid_t *hid = &usbh_composite_hid;

#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
	u32 i = 0;
	RTK_LOGS(TAG, RTK_LOG_INFO, "Yiyuan 0x%08x data=%d\n", pbuf, buf_length);
	for (i = 0; i < buf_length;) {

		if (i + 10 < buf_length) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",
					 pbuf[i], pbuf[i + 1], pbuf[i + 2], pbuf[i + 3], pbuf[i + 4], pbuf[i + 5], pbuf[i + 6], pbuf[i + 7], pbuf[i + 8], pbuf[i + 9]);
			i += 10;
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "%02x ", pbuf[i]);
			i ++;
		}
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
#endif

	usbh_composite_hid_parse_hid_report_descriptor(pbuf, buf_length, &(hid->vol_caps));

	return HAL_OK;
}

static void usbh_composite_hid_parse_hid_msg(const u8 *report, u8 len)
{
	usbh_composite_hid_t *hid = &usbh_composite_hid;
	usbh_composite_hid_event_t *event = &(hid->report_event);
	int ret;

#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Received HID Report (length: %d): ", len);
	for (u8 i = 0; i < len; i++) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "%02x ", report[i]);
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
#endif

	ret = usbh_composite_hid_parse_hid_report(report, len, &(hid->vol_caps));
	if (ret != HAL_OK) {
		return;
	}

	if (event->type != VOLUME_EVENT_NONE) {
		event->is_press = 1;
#if USBH_COMPOSITE_HID_UAC_DEBUG
		hid->last_event.type = event->type;
		hid->last_event.is_press = event->is_press;
		hid->event_cnt++;
#endif
	}

	if ((hid->cb != NULL) && (hid->cb->report != NULL)) {
		hid->cb->report(event);
	}
}

/**
  * @brief  Parse audio control interface
  * @param  pbuf: given descriptor buffer
  * @param  length: lenghth of parse
  * @param  buf_length: lenghth of given buffer
  * @retval Status
  */
static int usbh_composite_hid_parse(u8 *pbuf, u32 *length, u32 buf_length)
{
	usbh_composite_hid_t *hid = &usbh_composite_hid;
	usbh_dev_hid_desc_t *hid_desc = NULL;
	usbh_hid_ep_cfg_t *ep_cfg = NULL;

	u8 *desc = pbuf;
	u16 len = 0;

	hid->itf_idx = desc[2];
	hid->alt_set_idx = desc[3];
	*length = ((usb_descriptor_header_t *) desc)->bLength;
	desc = pbuf + *length;

	while (1) {
		if (desc == NULL || *length >= buf_length) {
			break;
		}

		switch (((usb_descriptor_header_t *) desc)->bDescriptorType) {
		case USBH_HID_DESC:
			hid_desc = (usbh_dev_hid_desc_t *)desc;
			usb_os_memcpy(&(hid->hid_desc), hid_desc, sizeof(usbh_dev_hid_desc_t));

			len = ((usb_descriptor_header_t *) desc)->bLength;
			desc = desc + len;
			*length += len;
			break;

		case USB_DESC_TYPE_ENDPOINT: {
			usbh_ep_desc_t *ep_desc = (usbh_ep_desc_t *)desc;
			ep_cfg = &(hid->ep_info);

			ep_cfg->ep_addr = ep_desc->bEndpointAddress;
			ep_cfg->packet_size = ep_desc->wMaxPacketSize;
			ep_cfg->interval = ep_desc->bInterval;

			len = ((usb_descriptor_header_t *) desc)->bLength;
			desc = desc + len;
			*length += len;
		}
		break;

		default: {
			len = ((usb_descriptor_header_t *) desc)->bLength;
			desc = desc + len;
			*length += len;
		}
		break;
		}
	}
	return HAL_OK;
}

/**
  * @brief  Find next standard descriptor
  * @param  pbuf: given buffer to find
  * @param  len: given buffer length
  * @retval Pointer of next standard descriptor
  */
static u8 *usbh_composite_hid_find_next_stdesc(u8 *pbuf, u32 *len)
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
  * @brief  Parse configuration descriptor
  * @param  host: usb host structure
  * @retval Status
  */
static int usbh_composite_hid_parse_cfgdesc(usb_host_t *host)
{
	int ret = HAL_OK;
	usbh_cfg_desc_t *desc = (usbh_cfg_desc_t *)usbh_get_active_raw_configuration_descriptor(host);
	usbh_if_desc_t *pbuf = (usbh_if_desc_t *) desc;
	u32 cfglen = (u32)desc->wTotalLength;
	u32 len = 0;

	while (1) {
		pbuf = (usbh_if_desc_t *)usbh_composite_hid_find_next_stdesc((u8 *)pbuf, &cfglen);
		if (pbuf == NULL) {
			break;
		}

		if (pbuf->bInterfaceClass == USBH_CLASS_HID) {
			ret = usbh_composite_hid_parse((u8 *)pbuf, &len, cfglen);
			if (ret) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "HID parse fail\n");
				return ret;
			}
			pbuf = (usbh_if_desc_t *)((u8 *) pbuf + len);
			cfglen -= len;
			len = 0;
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

static int usbh_composite_hid_process_set_hid_alt(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_hid_t *hid = &usbh_composite_hid;

	setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE;
	setup.b.bRequest = USB_REQ_SET_INTERFACE;
	setup.b.wValue = hid->alt_set_idx;
	setup.b.wIndex = hid->itf_idx;
	setup.b.wLength = 0U;

	return usbh_ctrl_request(host, &setup, NULL);
}

static int usbh_composite_hid_process_get_hid_report_desc(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_hid_t *hid = &usbh_composite_hid;

	setup.b.bmRequestType = USB_D2H | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE;
	setup.b.bRequest = USB_REQ_GET_DESCRIPTOR;
	setup.b.wValue = USBH_HID_REPORT_TYPE << 8;
	setup.b.wIndex = hid->itf_idx;
	setup.b.wLength = hid->hid_desc.wDescriptorLength;

	return usbh_ctrl_request(host, &setup, hid->hid_ctrl_buf);
}

/**
  * @brief  Process hid in message
  * @param  host: Host handle
  * @retval None
*/
static void usbh_composite_hid_in_process(usb_host_t *host)
{
	usbh_composite_hid_t *hid = &usbh_composite_hid;
	usbh_hid_ep_cfg_t *ep = &(hid->ep_info);
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	u32 len;

	//rx to do
	switch (ep->xfer_state) {
	case USBH_HID_XFER:
		if ((host->tick - ep->tick) > ep->interval) {
			ep->tick = host->tick;
			ep->xfer_state = USBH_HID_XFER_BUSY;
			usbh_intr_receive_data(host,
								   ep->buf,
								   ep->packet_size,
								   ep->pipe);
			hid->next_xfer = 1;
		}
		break;

	case USBH_HID_XFER_BUSY:
		urb_state = usbh_get_urb_state(host, ep->pipe);
		if (urb_state == USBH_URB_DONE) {
			len = usbh_get_last_transfer_size(host, ep->pipe);
			usbh_composite_hid_parse_hid_msg(ep->buf, len);
			ep->xfer_state = USBH_HID_XFER;  ///loop to next
		} else if (urb_state == USBH_URB_BUSY) {
			if ((host->tick - ep->tick) > ep->interval) {
				ep->xfer_state = USBH_HID_XFER;
				hid->next_xfer = 1;
			}
		} else if (urb_state == USBH_URB_ERROR) {
			ep->xfer_state = USBH_HID_XFER;
		} else if (urb_state == USBH_URB_IDLE) {
			if ((host->tick - ep->tick) > (UBSH_COMPOSITE_HID_IDLE_MAX_CNT)*ep->interval) {
				ep->xfer_state = USBH_HID_XFER;
				hid->next_xfer = 1;
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
static int usbh_composite_hid_cb_attach(usb_host_t *host)
{
	usbh_composite_hid_t *hid = &usbh_composite_hid;
	usbh_hid_ep_cfg_t *ep = NULL;

	int status = HAL_ERR_UNKNOWN;
	u32 max_ep_size;

	if (host->config.speed == USB_SPEED_HIGH) {
		max_ep_size = 1024;
	} else {
		max_ep_size = 1023;
	}

	status = usbh_composite_hid_parse_cfgdesc(host);
	if (status) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Cfg parse fail\n");
		return status;
	}

	if (hid->ep_info.ep_addr) {
		ep = &(hid->ep_info);
		ep->xfer_state = USBH_HID_XFER_IDLE;
		hid->report_desc_status = USBH_HID_REPORT_SET_ALT;

		if (ep->packet_size >= max_ep_size) {
			ep->packet_size = max_ep_size;
		}

		ep->pipe = usbh_alloc_pipe(host, ep->ep_addr);

		usbh_open_pipe(host,
					   ep->pipe,
					   ep->ep_addr,
					   USB_CH_EP_TYPE_INTR,
					   ep->packet_size);
	}

	if ((hid->cb != NULL) && (hid->cb->attach != NULL)) {
		hid->cb->attach();
	}

	return HAL_OK;
}

/**
  * @brief  Detach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_hid_cb_detach(usb_host_t *host)
{
	usbh_composite_hid_t *hid = &usbh_composite_hid;
	UNUSED(host);

	if ((hid->cb != NULL) && (hid->cb->detach != NULL)) {
		hid->cb->detach();
	}

	return HAL_OK;
}

/**
  * @brief  Standard control requests handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_hid_cb_setup(usb_host_t *host)
{
	usbh_composite_hid_t *hid = &usbh_composite_hid;
	usbh_hid_ep_cfg_t *ep = &(hid->ep_info);
	UNUSED(host);

	if ((hid->cb != NULL) && (hid->cb->setup != NULL)) {
		hid->cb->setup();
	}

	ep->xfer_state = USBH_HID_XFER;
	ep->buf = hid->hid_ctrl_buf;

	return HAL_OK;
}

/**
  * @brief  State machine handling callback
  * @param  host: Host handle
  * @param  msg: event message
  * @retval Status
  */
static int usbh_composite_hid_cb_process(usb_host_t *host, u32 msg)
{
	usbh_composite_hid_t *hid = &usbh_composite_hid;
	usbh_hid_ep_cfg_t *ep = &(hid->ep_info);

	if ((hid->hid_ctrl_buf) && (msg == ep->pipe)) {
		hid->next_xfer = 0;
		usbh_composite_hid_in_process(host);
		if (hid->next_xfer) {
			usbh_notify_class_state_change(host, ep->pipe);
		}

		return HAL_OK;
	}

	return HAL_BUSY;
}

/**
  * @brief  Sof callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_hid_cb_sof(usb_host_t *host)
{
	usbh_composite_hid_t *hid = &usbh_composite_hid;
	usbh_hid_ep_cfg_t *ep = &(hid->ep_info);

	if ((host->tick - ep->tick) > UBSH_COMPOSITE_HID_TRIGGER_MAX_CNT) {
		usbh_notify_class_state_change(host, ep->pipe);
	}

	return HAL_OK;
}


/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init hid class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_composite_hid_init(usbh_composite_host_t *driver, usbh_composite_hid_usr_cb_t *cb)
{
	usbh_composite_hid_t *hid = &usbh_composite_hid;
	int ret;

	usb_os_memset(hid, 0x00, sizeof(usbh_composite_hid_t));
	hid->driver = driver;

	hid->hid_ctrl_buf = (u8 *)usb_os_malloc(UBSH_COMPOSITE_HID_CTRL_BUF_LEN);
	if (NULL == hid->hid_ctrl_buf) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc mem %d fail\n", UBSH_COMPOSITE_HID_CTRL_BUF_LEN);
		return HAL_ERR_MEM;
	}

	if (cb != NULL) {
		hid->cb = cb;
		if (cb->init != NULL) {
			ret = cb->init();
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC init fail\n");
				return ret;
			}
		}
	}

	return HAL_OK;
}

/**
  * @brief  Deinit hid class
  * @retval Status
  */
int usbh_composite_hid_deinit(void)
{
	usbh_composite_hid_t *hid = &usbh_composite_hid;

	if (hid->hid_ctrl_buf != NULL) {
		usb_os_mfree(hid->hid_ctrl_buf);
		hid->hid_ctrl_buf = NULL;
	}

	return HAL_OK;
}

/**
  * @brief  Get hid report descriptor
  * @param  host: Host handle
  * @retval Status
  */
int usbh_composite_hid_handle_report_desc(usb_host_t *host)
{
	usbh_composite_hid_t *hid = &usbh_composite_hid;
	int ret = HAL_OK;

	if (hid->ep_info.ep_addr && hid->report_desc == NULL) {
		//1. set itf
		//2. set idle
		//3. get report desc
		if (hid->report_desc_status == USBH_HID_REPORT_SET_ALT) {
			ret = usbh_composite_hid_process_set_hid_alt(host);
			if (ret == HAL_OK) {
				hid->report_desc_status = USBH_HID_REPORT_GET_DESC;
				ret = HAL_BUSY;
			} else if (ret != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "IN alt err\n");
				usb_os_sleep_ms(100);
			}
		} else if (hid->report_desc_status == USBH_HID_REPORT_GET_DESC) {
			ret = usbh_composite_hid_process_get_hid_report_desc(host);
			if (ret == HAL_OK) {
				hid->report_desc_status = USBH_HID_REPORT_MAX;
				//parse report desc
				hid->report_desc = hid->hid_ctrl_buf;
				usbh_composite_hid_parse_hid_report_desc(hid->report_desc, hid->hid_desc.wDescriptorLength);
			} else if (ret != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "IN alt err\n");
				usb_os_sleep_ms(100);
			}
		}
	}

	return ret;
}