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

#define mem_sync()    __sync_synchronize()

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
static int usbh_composite_hid_parse_details(usbh_itf_data_t *itf_data);
static int usbh_composite_hid_parse_interface(usb_host_t *host);
static int usbh_composite_hid_process_get_hid_report_desc(usb_host_t *host);
static void usbh_composite_hid_in_process(usb_host_t *host);
static int usbh_composite_hid_cb_attach(usb_host_t *host);
static int usbh_composite_hid_cb_detach(usb_host_t *host);
static int usbh_composite_hid_cb_setup(usb_host_t *host);
static int usbh_composite_hid_cb_process(usb_host_t *host, u32 msg);
static int usbh_composite_hid_cb_sof(usb_host_t *host);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "HID";

/* USB Class Driver */
const usbh_class_driver_t usbh_composite_hid_driver = {
	.attach = usbh_composite_hid_cb_attach,
	.detach = usbh_composite_hid_cb_detach,
	.setup = usbh_composite_hid_cb_setup,
	.process = usbh_composite_hid_cb_process,
	.sof = usbh_composite_hid_cb_sof,
};

static usbh_composite_hid_t usbh_composite_hid;

#if USBH_COMPOSITE_HID_UAC_DEBUG
void usbh_composite_hid_status_dump(void)
{
	usbh_composite_hid_t *hid = &usbh_composite_hid;
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "HID %d-%d %d\n", hid->event_cnt, hid->last_event.type, hid->hid_ctrl);
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

/* hid report parse */
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
	usbh_composite_hid_parse_state state = {0};
	usbh_composite_hid_item_t item;
	const u8 *end = data + length;
	const u8 *ptr = data;
#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
	int item_cnt = 0;
#endif

	state.device_info = device_info;
	memset(device_info, 0, sizeof(usbh_composite_hid_ctrl_caps_t));

	while ((ptr = usbh_composite_hid_fetch_item(ptr, end, &item)) != NULL) {
#if USBH_COMPOSITE_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "[%03d] ", ++item_cnt);
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
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tVolume controls in Report ID: %d\n", device_info->volume.report_id);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tVolume Up: bit %d\n", device_info->volume.up_bit);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tVolume Down: bit %d\n", device_info->volume.down_bit);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tMute: bit %d\n", device_info->volume.mute_bit);
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Media control supported: %s\n", device_info->media.supported ? "Yes" : "No");
	if (device_info->media.supported) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tMedia controls in Report ID: %d\n", device_info->media.report_id);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tPlay/Pause: bit %d\n", device_info->media.play_pause_bit);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tStop: bit %d\n", device_info->media.stop_bit);
	}
#endif
}

static int usbh_composite_hid_parse_hid_report(const u8 *report_data, u8 report_len,
		const usbh_composite_hid_ctrl_caps_t *device_info)
{
	usbh_composite_hid_t *hid = &usbh_composite_hid;
	usbh_composite_hid_event_t *event = &(hid->report_event);
	const u8 *data_start;
	u32 data_len;
	u8 report_id = 0;

	event->type = VOLUME_EVENT_NONE;
	event->is_press = 0;

	if (report_len == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Warning: Invalid report length: %d\n", report_len);
		return HAL_ERR_PARA;
	}

	data_start = report_data;
	data_len = report_len;

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
	RTK_LOGS(TAG, RTK_LOG_INFO, "Report info 0x%08x data=%d\n", pbuf, buf_length);
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
	usb_ringbuf_manager_t *handle = &(hid->report_msg);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "RX HID Report:(%d-%d-%d):", len, handle->head, handle->tail);
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
  * @param  itf_data: given interface struct handle
  * @retval Status
  */
static int usbh_composite_hid_parse_details(usbh_itf_data_t *itf_data)
{
	usbh_composite_hid_t *hid = &usbh_composite_hid;
	usbh_dev_hid_desc_t *hid_desc = NULL;
	usbh_ep_desc_t *ep_desc = NULL;
	u8 *desc = itf_data->raw_data;
	u16 len = 0;
	u16 itf_total_len = 0;

	hid->itf_idx = desc[2];
	hid->itf_alt_idx = desc[3];

	while (1) {
		if (desc == NULL || itf_total_len >= itf_data->raw_data_len) {
			break;
		}

		len = ((usbh_desc_header_t *) desc)->bLength;
		desc += len;
		itf_total_len += len;

		switch (((usbh_desc_header_t *) desc)->bDescriptorType) {
		case USB_DESC_TYPE_INTERFACE:
			if (((usbh_itf_desc_t *)desc)->bInterfaceNumber != hid->itf_idx) { //find another itf, should return
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "Hid intf new %d:old %d, return\n\n", ((usbh_itf_desc_t *)desc)->bInterfaceNumber, hid->itf_idx);
				return HAL_OK;
			}
			break;
		case USBH_HID_DESC:
			hid_desc = (usbh_dev_hid_desc_t *)desc;
			usb_os_memcpy(&(hid->hid_desc), hid_desc, sizeof(usbh_dev_hid_desc_t));
			break;

		case USB_DESC_TYPE_ENDPOINT:
			ep_desc = (usbh_ep_desc_t *)desc;
			usb_os_memcpy(&(hid->ep_desc), ep_desc, sizeof(usbh_ep_desc_t));
			break;

		default:
			break;
		}
	}
	return HAL_OK;
}

/**
  * @brief  Parse configuration descriptor
  * @param  host: usb host structure
  * @retval Status
  */
static int usbh_composite_hid_parse_interface(usb_host_t *host)
{
	usbh_dev_id_t dev_id = {0,};
	dev_id.bInterfaceClass = USBH_CLASS_HID;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS;
	usbh_itf_data_t *itf_data = usbh_get_interface_descriptor(host, &dev_id);
	int ret = HAL_OK;

	if (itf_data) {
		ret = usbh_composite_hid_parse_details(itf_data);
		if (ret) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "AC parse fail\n");
			return ret;
		}
	}

	return ret;
}

static int usbh_composite_hid_process_get_hid_report_desc(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_hid_t *hid = &usbh_composite_hid;

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_REQ_GET_DESCRIPTOR;
	setup.req.wValue = USBH_HID_REPORT_TYPE << 8;
	setup.req.wIndex = hid->itf_idx;
	setup.req.wLength = hid->hid_desc.wDescriptorLength;

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
	usbh_pipe_t *pipe = &(hid->pipe);
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	u32 len;

	switch (pipe->xfer_state) {
	case USBH_EP_XFER_START:
		if (usbh_get_elapsed_ticks(host, pipe->tick) > pipe->ep_interval) {
			pipe->tick = usbh_get_tick(host);
			pipe->xfer_state = USBH_EP_XFER_BUSY;
			pipe->xfer_len = pipe->ep_mps;
			usbh_transfer_data(host, pipe);
			hid->next_xfer = 1;
		}
		break;

	case USBH_EP_XFER_BUSY:
		urb_state = usbh_get_urb_state(host, pipe);
		if (urb_state == USBH_URB_DONE) {
			len = usbh_get_last_transfer_size(host, pipe);
			/* save to list */
			usb_ringbuf_add_tail(&(hid->report_msg), pipe->xfer_buf, len, 1);
			pipe->xfer_state = USBH_EP_XFER_START;  ///loop to next
		} else if (urb_state == USBH_URB_BUSY) {
			if (usbh_get_elapsed_ticks(host, pipe->tick) > pipe->ep_interval) {
				pipe->xfer_state = USBH_EP_XFER_START;
				hid->next_xfer = 1;
			}
		} else if (urb_state == USBH_URB_ERROR) {
			pipe->xfer_state = USBH_EP_XFER_START;
		} else if (urb_state == USBH_URB_IDLE) {
			if (usbh_get_elapsed_ticks(host, pipe->tick) > (UBSH_COMPOSITE_HID_IDLE_MAX_CNT)*pipe->ep_interval) {
				pipe->xfer_state = USBH_EP_XFER_START;
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
	usbh_pipe_t *pipe = NULL;

	int status = HAL_ERR_UNKNOWN;

	status = usbh_composite_hid_parse_interface(host);
	if (status) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Cfg parse fail\n");
		return status;
	}

	if (hid->ep_desc.bEndpointAddress) {
		pipe = &(hid->pipe);
		hid->report_desc_status = USBH_HID_REPORT_SET_ALT;

		usbh_open_pipe(host, pipe, &(hid->ep_desc));
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
	usbh_pipe_t *pipe = &(hid->pipe);
	UNUSED(host);

	if ((hid->cb != NULL) && (hid->cb->setup != NULL)) {
		hid->cb->setup();
	}

	pipe->xfer_state = USBH_EP_XFER_START;
	pipe->xfer_buf = hid->hid_ctrl_buf;

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
	usbh_pipe_t *pipe = &(hid->pipe);
	usbh_event_t event;
	event.d32 = msg;

	if ((hid->hid_ctrl_buf) && (event.msg.pipe_num == pipe->pipe_num)) {
		hid->next_xfer = 0;
		usbh_composite_hid_in_process(host);
		if (hid->next_xfer) {
			usbh_notify_composite_class_state_change(host, pipe->pipe_num, USBH_COMPOSITE_HID_EVENT);
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
	usbh_pipe_t *pipe = &(hid->pipe);

	if (usbh_get_elapsed_ticks(host, pipe->tick) > UBSH_COMPOSITE_HID_TRIGGER_MAX_CNT) {
		usbh_notify_composite_class_state_change(host, pipe->pipe_num, USBH_COMPOSITE_HID_EVENT);
	}

	return HAL_OK;
}

static void usbh_composite_hid_msg_parse_thread(void *param)
{
	UNUSED(param);
	usbh_composite_hid_t *hid = &usbh_composite_hid;
	usb_ringbuf_manager_t *handle = &(hid->report_msg);
	u8 report_msg[10];
	u32 read_cnt;

	hid->parse_task_alive = 1;
	hid->parse_task_exit = 0;

	while (hid->parse_task_exit == 0) {
		read_cnt = usb_ringbuf_remove_head(handle, report_msg, 10, NULL);
		if (read_cnt) {
			if (hid->hid_ctrl) {
				usbh_composite_hid_parse_hid_msg(report_msg, read_cnt);
			}
		} else {
			rtos_time_delay_ms(50);
		}
	}

	hid->parse_task_alive = 0;
	rtos_task_delete(NULL);
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

	usb_ringbuf_manager_init(&(hid->report_msg), USBH_COMPOSITE_HID_MST_COUNT, USBH_COMPOSITE_HID_MSG_LENGTH, 0);

	if (rtos_task_create(&(hid->msg_parse_task), ((const char *)"usbh_composite_hid_msg_parse"), usbh_composite_hid_msg_parse_thread,
						 NULL, 4 * 1024U, USBH_COMPOSITE_HID_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hid parse task fail\n");
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

	if (hid->parse_task_alive) {
		hid->parse_task_exit = 1;
		do {
			rtos_time_delay_ms(1);
		} while (hid->parse_task_alive);
	}

	if (hid->hid_ctrl_buf != NULL) {
		usb_os_mfree(hid->hid_ctrl_buf);
		hid->hid_ctrl_buf = NULL;
	}

	usb_ringbuf_manager_deinit(&(hid->report_msg));

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

	if (hid->pipe.ep_addr && hid->report_desc == NULL) {
		//1. set itf
		//2. set idle
		//3. get report desc
		if (hid->report_desc_status == USBH_HID_REPORT_SET_ALT) {
			ret = usbh_ctrl_set_interface(host, hid->itf_idx, hid->itf_alt_idx);
			if (ret == HAL_OK) {
				hid->report_desc_status = USBH_HID_REPORT_GET_DESC;
				ret = HAL_BUSY;
			} else if (ret != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "HID set alt err %d\n", ret);
				usb_os_sleep_ms(5);
				hid->report_desc_status = USBH_HID_REPORT_GET_DESC;
			}
		} else if (hid->report_desc_status == USBH_HID_REPORT_GET_DESC) {
			ret = usbh_composite_hid_process_get_hid_report_desc(host);
			if (ret == HAL_OK) {
				hid->report_desc_status = USBH_HID_REPORT_MAX;
				//parse report desc
				hid->report_desc = hid->hid_ctrl_buf;
				hid->hid_ctrl = 1;
				usbh_composite_hid_parse_hid_report_desc(hid->report_desc, hid->hid_desc.wDescriptorLength);
			} else if (ret != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "HID get report err %d, no support\n", ret);
				hid->hid_ctrl = 0;
				ret = HAL_OK;
			}
		}
	} else {
		hid->hid_ctrl = 0;
	}

	return ret;
}
