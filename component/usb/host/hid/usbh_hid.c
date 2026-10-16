/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/
#include "usbh_hid.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#define USBH_HID_CTRL_BUF_LEN        512U
#define USBH_HID_TRIGGER_MAX_CNT     50U

#if USBH_HID_DEBUG
#define USBH_HID_REPORT_DESC_PARSE_DEBUG       0
#else
#define USBH_HID_REPORT_DESC_PARSE_DEBUG       0
#endif

#define USBH_HID_THREAD_PRIORITY     3U      /**< HID processing thread priority */
#define USBH_HID_THREAD_STACK_SIZE   768U    /**< HID msg parse thread stack size */
#define USBH_HID_MST_COUNT           10U     /**< Maximum support touch count (if applicable) */
#define USBH_HID_MSG_LENGTH          16U     /**< Message queue length */

/* Private function prototypes -----------------------------------------------*/
#if USBH_HID_REPORT_DESC_PARSE_DEBUG
static const char *usbh_hid_get_usage_page_name(u32 usage_page);
static const char *usbh_hid_get_usage_name(uint32_t usage_page, uint32_t usage);
static void usbh_hid_decode_data_attributes(uint32_t data);
static void usbh_hid_print_item(const usbh_hid_item_t *item);
#endif
static const u8 *usbh_hid_fetch_item(const u8 *start, const u8 *end, usbh_hid_item_t *item);
static void usbh_hid_process_global_item(usbh_hid_parse_state *state, const usbh_hid_item_t *item);
static void usbh_hid_process_usage(usbh_hid_parse_state *state, u32 usage);
static void usbh_hid_process_local_item(usbh_hid_parse_state *state, const usbh_hid_item_t *item);
static void usbh_hid_settle_input_field(usbh_hid_parse_state *state, const usbh_hid_item_t *item);
static void usbh_hid_reset_local_state(usbh_hid_parse_state *state);
static void usbh_hid_process_main_item(usbh_hid_parse_state *state, const usbh_hid_item_t *item);
static u32 usbh_hid_extract_bits(const u8 *data, u32 data_len, u32 bit_offset, u8 nbits);
static usbh_hid_event_type_t usbh_hid_usage_to_event(u32 usage);
static bool usbh_hid_test_ctrl_bit(const u8 *data, u32 data_len, u16 bit);
static void usbh_hid_parse_hid_report_descriptor(const u8 *data, u16 length, usbh_hid_ctrl_caps_t *device_info);
static int usbh_hid_parse_hid_report(const u8 *report_data, u8 report_len, const usbh_hid_ctrl_caps_t *device_info);
static int usbh_hid_parse_hid_report_desc(u8 *pbuf, u16 buf_length);
static void usbh_hid_parse_hid_msg(const u8 *report, u8 len);
static int usbh_hid_parse_details(usbh_itf_data_t *itf_data);
static int usbh_hid_parse_interface(usb_host_t *host);
static int usbh_hid_process_get_hid_report_desc(usb_host_t *host);
static int usbh_hid_handle_report_desc(usb_host_t *host);
static void usbh_hid_in_process(usb_host_t *host);
static int usbh_hid_attach(usb_host_t *host);
static void usbh_hid_detach(usb_host_t *host);
static int usbh_hid_setup(usb_host_t *host);
static void usbh_hid_process(usb_host_t *host, usbh_drv_msg_t *msg);
static void usbh_hid_sof(usb_host_t *host);
/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "HID";

/* USB HID device identification */
static const usbh_dev_id_t hid_devs[] = {
	{
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS,
		.bInterfaceClass = USBH_CLASS_HID,
	},
	{
	},
};

/* USB Class Driver */
static const usbh_class_driver_t usbh_hid_driver = {
	.id_table = hid_devs,
	.attach = usbh_hid_attach,
	.detach = usbh_hid_detach,
	.setup = usbh_hid_setup,
	.process = usbh_hid_process,
	.sof = usbh_hid_sof,
};

static usbh_hid_t usbh_hid;

/* Consumer usages this driver reports, indexed by USBH_HID_TRACK_xxx */
static const u16 usbh_hid_track_usages[USBH_HID_TRACK_USAGE_CNT] = {
	USBH_HID_CONSUMER_VOLUME_UP,
	USBH_HID_CONSUMER_VOLUME_DOWN,
	USBH_HID_CONSUMER_MUTE,
	USBH_HID_CONSUMER_PLAY_PAUSE,
	USBH_HID_CONSUMER_STOP,
};

#if USBH_HID_DEBUG
void usbh_hid_status_dump(void)
{
	usbh_hid_t *hid = &usbh_hid;
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "HID %d-%d %d\n", hid->event_cnt, hid->last_event.type, hid->hid_ctrl);
}
#endif

#if USBH_HID_REPORT_DESC_PARSE_DEBUG
// Usage Page
static const char *usbh_hid_get_usage_page_name(u32 usage_page)
{
	switch (usage_page) {
	case USBH_HID_UP_CONSUMER:
		return "Consumer";
	default:
		return "Unknown";
	}

	return "Unknown";
}

static const char *usbh_hid_get_usage_name(uint32_t usage_page, uint32_t usage)
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

static void usbh_hid_decode_data_attributes(uint32_t data)
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

static void usbh_hid_print_item(const usbh_hid_item_t *item)
{
	const char *type_str[] = {"Main", "Global", "Local", "Reserved"};
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Item: Type:%s Tag:0x%x Size:%d Data:0x%08x\t", type_str[item->type], item->tag, item->size, item->data);
}
#endif

/* hid report parse */
static const u8 *usbh_hid_fetch_item(const u8 *start, const u8 *end, usbh_hid_item_t *item)
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

	/* Ref HID 1.11 6.2.2.2: item data is little endian. Cast to u32 before the
	 * shift: a u8 operand would be promoted to signed int, and shifting the top
	 * byte of a 4-byte item left by 24 would overflow the sign bit. */
	item->data = 0;
	for (u32 i = 0; i < (u32)item->size; i++) {
		if (start >= end) {
			return NULL;
		}
		item->data |= ((u32)(*start)) << (8U * i);
		start++;
	}

	return start;
}

static void usbh_hid_process_global_item(usbh_hid_parse_state *state, const usbh_hid_item_t *item)
{
	switch (item->tag) {
	case USBH_HID_GLOBAL_ITEM_TAG_USAGE_PAGE:
		state->usage_page = (u16)item->data;
#if USBH_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Usage Page: 0x%x(%s)\n", item->data, usbh_hid_get_usage_page_name(item->data));
#endif
		break;
	case USBH_HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM:
		state->logical_min = (int32_t)item->data;
#if USBH_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Logical Minimum: %d\n", state->logical_min);
#endif
		break;
	case USBH_HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM:
		state->logical_max = (int32_t)item->data;
#if USBH_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Logical Maximum: %d\n", state->logical_max);
#endif
		break;
	case USBH_HID_GLOBAL_ITEM_TAG_REPORT_SIZE:
		state->report_size = (u16)item->data;
#if USBH_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Report Size: %d bits\n", state->report_size);
#endif
		break;
	case USBH_HID_GLOBAL_ITEM_TAG_REPORT_COUNT:
		state->report_count = (u16)item->data;
#if USBH_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Report Count: %d\n", state->report_count);
#endif
		break;
	case USBH_HID_GLOBAL_ITEM_TAG_REPORT_ID:
		/* Ref HID 1.11 5.6: each Report ID starts a new report, so every bit
		 * cursor restarts at the first bit after the 1-byte ID prefix. */
		state->in_bit_offset = 0;
		state->out_bit_offset = 0;
		state->feat_bit_offset = 0;
		state->device_info->report_id_count++;
		state->report_id = (u8)item->data;
#if USBH_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Report ID: %d\n", state->report_id);
#endif
		break;
	default:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Unknown Global Item (Tag: 0x%X)\n", item->tag);
		break;
	}
}

/**
  * @brief  Record that a tracked Consumer usage appears at ordinal usage_cnt of
  *         the Main item currently being declared.
  * @note   HID 1.11 6.2.2.8: Local items apply to the *next* Main item, so the
  *         report position of a usage cannot be known until that Main item
  *         arrives and tells us whether the field is a bitmap or an array of
  *         usage codes. Only the ordinal is remembered here.
  * @param  state: Parser state
  * @param  usage: Usage value just declared
  */
static void usbh_hid_process_usage(usbh_hid_parse_state *state, u32 usage)
{
	u8 idx;

	if (state->usage_page == USBH_HID_UP_CONSUMER) {
		switch (usage) {
		case USBH_HID_CONSUMER_VOLUME_UP:
			idx = USBH_HID_TRACK_VOLUME_UP;
			break;
		case USBH_HID_CONSUMER_VOLUME_DOWN:
			idx = USBH_HID_TRACK_VOLUME_DOWN;
			break;
		case USBH_HID_CONSUMER_MUTE:
			idx = USBH_HID_TRACK_MUTE;
			break;
		case USBH_HID_CONSUMER_PLAY_PAUSE:
			idx = USBH_HID_TRACK_PLAY_PAUSE;
			break;
		case USBH_HID_CONSUMER_STOP:
			idx = USBH_HID_TRACK_STOP;
			break;
		default:
			idx = USBH_HID_TRACK_USAGE_CNT;
			break;
		}

		if ((idx < USBH_HID_TRACK_USAGE_CNT) && (state->usage_cnt <= 0xFFFFU)) {
			state->track_ord[idx] = (u16)state->usage_cnt;
			state->track_mask |= (u8)(1U << idx);
		}
	}

	state->usage_cnt++;
}

static void usbh_hid_process_local_item(usbh_hid_parse_state *state, const usbh_hid_item_t *item)
{
	u32 usage;

	switch (item->tag) {
	case USBH_HID_LOCAL_ITEM_TAG_USAGE:
		usbh_hid_process_usage(state, item->data);
#if USBH_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Usage: 0x%03x (%s)\n", item->data, usbh_hid_get_usage_name(state->usage_page, item->data));
#endif
		break;

	case USBH_HID_LOCAL_ITEM_TAG_USAGE_MIN:
		/* Ref HID 1.11 6.2.2.8: Usage Minimum/Maximum declare an inclusive
		 * range of usages, equivalent to listing each one in ascending order. */
		state->usage_min = item->data;
		state->usage_min_valid = 1;
#if USBH_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Usage Min: 0x%03x\n", item->data);
#endif
		break;

	case USBH_HID_LOCAL_ITEM_TAG_USAGE_MAX:
		if ((state->usage_min_valid != 0U) && (item->data >= state->usage_min)) {
			/* A range may span hundreds of usages, but only the tracked ones can
			 * change the outcome. Place each tracked usage that falls inside the
			 * range at its own ordinal, then skip the whole range at once. */
			u32 base_cnt = state->usage_cnt;

			for (u32 i = 0; i < USBH_HID_TRACK_USAGE_CNT; i++) {
				usage = usbh_hid_track_usages[i];
				if ((usage >= state->usage_min) && (usage <= item->data)) {
					state->usage_cnt = base_cnt + (usage - state->usage_min);
					usbh_hid_process_usage(state, usage);
				}
			}
			state->usage_cnt = base_cnt + (item->data - state->usage_min) + 1U;
		}
		state->usage_min_valid = 0;
#if USBH_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Usage Max: 0x%03x\n", item->data);
#endif
		break;

	default:
		RTK_LOGS(TAG, RTK_LOG_INFO, "Unknown Local(0x%x)\n", item->tag);
		break;
	}
}

/**
  * @brief  Turn the usages collected for an Input field into report positions.
  * @note   Ref HID 1.11 6.2.2.5/6.2.2.7. A field occupies
  *         (Report Size * Report Count) bits and comes in two flavours:
  *         - Variable with Report Size 1: a bitmap, where the n-th declared
  *           usage owns the n-th bit of the field;
  *         - Array: the field carries Report Count usage *codes* of Report Size
  *           bits each, not one bit per usage, so no per-usage bit exists.
  *         Anything else (e.g. Variable with Report Size > 1, a multi-bit value
  *         per control) carries no on/off bit this driver can use, so it is only
  *         skipped over.
  * @param  state: Parser state
  * @param  item: The Input Main item being closed
  */
static void usbh_hid_settle_input_field(usbh_hid_parse_state *state, const usbh_hid_item_t *item)
{
	usbh_hid_ctrl_caps_t *info = state->device_info;
	u32 abs_bit;

	if ((item->data & USBH_HID_ITEM_DATA_CONSTANT) != 0U) {
		return;    /* padding, never carries a usage */
	}

	if ((item->data & USBH_HID_ITEM_DATA_VARIABLE) != 0U) {
		if ((state->report_size != 1U) || (state->track_mask == 0U)) {
			return;
		}

		for (u32 i = 0; i < USBH_HID_TRACK_USAGE_CNT; i++) {
			if ((state->track_mask & (u8)(1U << i)) == 0U) {
				continue;
			}
			/* A usage declared beyond Report Count has no bit in this field. */
			if (state->track_ord[i] >= state->report_count) {
				continue;
			}
			abs_bit = state->in_bit_offset + state->track_ord[i];
			if (abs_bit >= USBH_HID_BIT_NONE) {
				continue;
			}

			switch (i) {
			case USBH_HID_TRACK_VOLUME_UP:
				info->volume.up_bit = (u16)abs_bit;
				info->volume.report_id = state->report_id;
				info->volume.supported = true;
				break;
			case USBH_HID_TRACK_VOLUME_DOWN:
				info->volume.down_bit = (u16)abs_bit;
				info->volume.report_id = state->report_id;
				info->volume.supported = true;
				break;
			case USBH_HID_TRACK_MUTE:
				info->volume.mute_bit = (u16)abs_bit;
				info->volume.report_id = state->report_id;
				info->volume.supported = true;
				break;
			case USBH_HID_TRACK_PLAY_PAUSE:
				info->media.play_pause_bit = (u16)abs_bit;
				info->media.report_id = state->report_id;
				info->media.supported = true;
				break;
			default:
				info->media.stop_bit = (u16)abs_bit;
				info->media.report_id = state->report_id;
				info->media.supported = true;
				break;
			}
		}
	} else if ((state->usage_page == USBH_HID_UP_CONSUMER) && (state->report_size >= 8U)
			   && (state->report_size <= 32U) && (state->report_count > 0U)
			   && (state->in_bit_offset < USBH_HID_BIT_NONE) && (info->consumer_array.supported == false)) {
		/* Array of Consumer usage codes: remember where the elements are and let
		 * the report path match the received code against the tracked usages. */
		info->consumer_array.report_id = state->report_id;
		info->consumer_array.bit_offset = (u16)state->in_bit_offset;
		info->consumer_array.elem_bits = (u8)state->report_size;
		info->consumer_array.elem_cnt = (state->report_count < USBH_HID_ARRAY_ELEM_CNT)
										? (u8)state->report_count : (u8)USBH_HID_ARRAY_ELEM_CNT;
		info->consumer_array.supported = true;
	} else {
		/* no usable on/off information in this field */
	}
}

/**
  * @brief  Clear the Local item state consumed by a Main item.
  * @note   Ref HID 1.11 6.2.2.8: Local items apply only to the Main item that
  *         follows them and are reset afterwards.
  * @param  state: Parser state
  */
static void usbh_hid_reset_local_state(usbh_hid_parse_state *state)
{
	state->usage_cnt = 0;
	state->track_mask = 0;
	state->usage_min_valid = 0;
}

static void usbh_hid_process_main_item(usbh_hid_parse_state *state, const usbh_hid_item_t *item)
{
	/* Ref HID 1.11 6.2.2.7: a field occupies Report Size * Report Count bits. */
	u32 field_bits = (u32)state->report_size * (u32)state->report_count;
#if USBH_HID_REPORT_DESC_PARSE_DEBUG
	u32 total_bytes;
#endif

	switch (item->tag) {
	case USBH_HID_MAIN_ITEM_TAG_INPUT:
		usbh_hid_settle_input_field(state, item);
		state->in_bit_offset += field_bits;
		usbh_hid_reset_local_state(state);
#if USBH_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Input - ");
		usbh_hid_decode_data_attributes(item->data);
		total_bytes = (field_bits + 7U) / 8U;
		if (state->report_id > 0) {
			total_bytes++;
		}
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "  Total bits: %d, Total bytes: %d\n", field_bits, total_bytes);
#endif
		break;

	case USBH_HID_MAIN_ITEM_TAG_OUTPUT:
		/* Output lives in its own report, so it must not shift Input positions. */
		state->out_bit_offset += field_bits;
		usbh_hid_reset_local_state(state);
#if USBH_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Output\n");
#endif
		break;

	case USBH_HID_MAIN_ITEM_TAG_FEATURE:
		/* Feature reports are fetched over EP0, never on the INTR IN pipe. */
		state->feat_bit_offset += field_bits;
		usbh_hid_reset_local_state(state);
#if USBH_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Feature\n");
#endif
		break;

	case USBH_HID_MAIN_ITEM_TAG_COLLECTION:
		state->collection_depth++;
		usbh_hid_reset_local_state(state);
#if USBH_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Collection - Type: %d\n", item->data);
#endif
		break;

	case USBH_HID_MAIN_ITEM_TAG_END_COLLECTION:
		if (state->collection_depth > 0U) {
			state->collection_depth--;
		}
		usbh_hid_reset_local_state(state);
#if USBH_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "End Collection\n");
#endif
		break;

	default:
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Unknown Main Item (Tag: 0x%x)\n", item->tag);
		break;
	}
}

static void usbh_hid_parse_hid_report_descriptor(const u8 *data, u16 length, usbh_hid_ctrl_caps_t *device_info)
{
	usbh_hid_parse_state state = {0};
	usbh_hid_item_t item;
	const u8 *end = data + length;
	const u8 *ptr = data;
#if USBH_HID_REPORT_DESC_PARSE_DEBUG
	int item_cnt = 0;
#endif

	usb_os_memset((void *)device_info, 0, sizeof(usbh_hid_ctrl_caps_t));
	/* Bit 0 is a legal position, so "not declared" needs its own value. */
	device_info->volume.up_bit = USBH_HID_BIT_NONE;
	device_info->volume.down_bit = USBH_HID_BIT_NONE;
	device_info->volume.mute_bit = USBH_HID_BIT_NONE;
	device_info->media.play_pause_bit = USBH_HID_BIT_NONE;
	device_info->media.stop_bit = USBH_HID_BIT_NONE;
	state.device_info = device_info;

	while ((ptr = usbh_hid_fetch_item(ptr, end, &item)) != NULL) {
#if USBH_HID_REPORT_DESC_PARSE_DEBUG
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "[%03d] ", ++item_cnt);
		usbh_hid_print_item(&item);
#endif

		switch (item.type) {
		case USBH_HID_ITEM_TYPE_GLOBAL:
			usbh_hid_process_global_item(&state, &item);
			break;
		case USBH_HID_ITEM_TYPE_LOCAL:
			usbh_hid_process_local_item(&state, &item);
			break;
		case USBH_HID_ITEM_TYPE_MAIN:
			usbh_hid_process_main_item(&state, &item);
			break;
		default:
			RTK_LOGS(TAG, RTK_LOG_INFO, "Unknown item type %d\n", item.type);
			break;
		}
	}

#if USBH_HID_REPORT_DESC_PARSE_DEBUG
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

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Consumer array: %s\n", device_info->consumer_array.supported ? "Yes" : "No");
	if (device_info->consumer_array.supported) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tReport ID %d, bit %d, %d bits x %d\n", device_info->consumer_array.report_id,
				 device_info->consumer_array.bit_offset, device_info->consumer_array.elem_bits, device_info->consumer_array.elem_cnt);
	}
#endif
}

/**
  * @brief  Extract nbits starting at bit_offset from a HID report.
  * @note   Ref HID 1.11 section 8: report data is packed least significant bit
  *         first, so a field may straddle byte boundaries.
  * @param  data: Report data, report ID prefix already removed
  * @param  data_len: Length of data in bytes
  * @param  bit_offset: Absolute bit position of the field
  * @param  nbits: Field width in bits, 1..32
  * @retval Field value, or 0 if the field does not fit inside data
  */
static u32 usbh_hid_extract_bits(const u8 *data, u32 data_len, u32 bit_offset, u8 nbits)
{
	u32 val = 0;
	u32 bit;

	if ((nbits == 0U) || (nbits > 32U)) {
		return 0;
	}
	/* Reject before reading: the whole field must lie within the report. */
	if ((bit_offset + (u32)nbits) > (data_len * 8U)) {
		return 0;
	}

	for (u32 i = 0; i < (u32)nbits; i++) {
		bit = bit_offset + i;
		if (((data[bit / 8U] >> (bit % 8U)) & 0x01U) != 0U) {
			val |= (u32)1U << i;
		}
	}

	return val;
}

/**
  * @brief  Map a Consumer page usage code to the event this driver reports.
  * @param  usage: Consumer usage code
  * @retval Event type, VOLUME_EVENT_NONE if the usage is not tracked
  */
static usbh_hid_event_type_t usbh_hid_usage_to_event(u32 usage)
{
	usbh_hid_event_type_t type;

	switch (usage) {
	case USBH_HID_CONSUMER_VOLUME_UP:
		type = VOLUME_EVENT_CONSUMER_UP;
		break;
	case USBH_HID_CONSUMER_VOLUME_DOWN:
		type = VOLUME_EVENT_CONSUMER_DOWN;
		break;
	case USBH_HID_CONSUMER_MUTE:
		type = VOLUME_EVENT_CONSUMER_MUTE;
		break;
	case USBH_HID_CONSUMER_PLAY_PAUSE:
		type = VOLUME_EVENT_CONSUMER_PLAY_PAUSE;
		break;
	case USBH_HID_CONSUMER_STOP:
		type = VOLUME_EVENT_CONSUMER_STOP;
		break;
	default:
		type = VOLUME_EVENT_NONE;
		break;
	}

	return type;
}

/**
  * @brief  Test a single control bit of a bitmap Input field.
  * @param  data: Report data, report ID prefix already removed
  * @param  data_len: Length of data in bytes
  * @param  bit: Absolute bit position, or USBH_HID_BIT_NONE if not declared
  * @retval true if the control is declared and currently set
  */
static bool usbh_hid_test_ctrl_bit(const u8 *data, u32 data_len, u16 bit)
{
	if (bit == USBH_HID_BIT_NONE) {
		return false;
	}

	return (usbh_hid_extract_bits(data, data_len, bit, 1U) != 0U);
}

static int usbh_hid_parse_hid_report(const u8 *report_data, u8 report_len,
									 const usbh_hid_ctrl_caps_t *device_info)
{
	usbh_hid_t *hid = &usbh_hid;
	usbh_hid_event_t *event = &(hid->report_event);
	const u8 *data_start;
	u32 data_len;
	u32 usage;
	u8 report_id = 0;

	event->type = VOLUME_EVENT_NONE;
	event->is_press = 0;

	if (report_len == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Report len is zero\n");
		return HAL_ERR_PARA;
	}

	data_start = report_data;
	data_len = report_len;

	/* Ref HID 1.11 5.6/8: when the descriptor declares a non-zero Report ID,
	 * every report is prefixed by that 1-byte ID. */
	if ((report_len > 1U) && ((device_info->volume.report_id > 0U) || (device_info->media.report_id > 0U)
							  || (device_info->consumer_array.report_id > 0U))) {
		report_id = report_data[0];
		data_start = &report_data[1];
		data_len = (u32)report_len - 1U;
	}

	/* Array field: the report carries the usage code of the key being pressed,
	 * not one bit per key. A code of 0 means "no key pressed". */
	if (device_info->consumer_array.supported && (report_id == device_info->consumer_array.report_id)) {
		for (u8 i = 0; i < device_info->consumer_array.elem_cnt; i++) {
			usage = usbh_hid_extract_bits(data_start, data_len,
										  (u32)device_info->consumer_array.bit_offset + ((u32)i * device_info->consumer_array.elem_bits),
										  device_info->consumer_array.elem_bits);
			event->type = usbh_hid_usage_to_event(usage);
			if (event->type != VOLUME_EVENT_NONE) {
				return HAL_OK;
			}
		}
	}

	if (device_info->volume.supported && (report_id == device_info->volume.report_id)) {
		if (usbh_hid_test_ctrl_bit(data_start, data_len, device_info->volume.up_bit)) {
			event->type = VOLUME_EVENT_CONSUMER_UP;
			return HAL_OK;
		}

		if (usbh_hid_test_ctrl_bit(data_start, data_len, device_info->volume.down_bit)) {
			event->type = VOLUME_EVENT_CONSUMER_DOWN;
			return HAL_OK;
		}

		if (usbh_hid_test_ctrl_bit(data_start, data_len, device_info->volume.mute_bit)) {
			event->type = VOLUME_EVENT_CONSUMER_MUTE;
			return HAL_OK;
		}
	}

	if (device_info->media.supported && (report_id == device_info->media.report_id)) {
		if (usbh_hid_test_ctrl_bit(data_start, data_len, device_info->media.play_pause_bit)) {
			event->type = VOLUME_EVENT_CONSUMER_PLAY_PAUSE;
			return HAL_OK;
		}

		if (usbh_hid_test_ctrl_bit(data_start, data_len, device_info->media.stop_bit)) {
			event->type = VOLUME_EVENT_CONSUMER_STOP;
			return HAL_OK;
		}
	}

	return HAL_OK;
}

static int usbh_hid_parse_hid_report_desc(u8 *pbuf, u16 buf_length)
{
	usbh_hid_t *hid = &usbh_hid;

#if USBH_HID_REPORT_DESC_PARSE_DEBUG
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

	usbh_hid_parse_hid_report_descriptor(pbuf, buf_length, &(hid->vol_caps));

	return HAL_OK;
}

static void usbh_hid_parse_hid_msg(const u8 *report, u8 len)
{
	usbh_hid_t *hid = &usbh_hid;
	usbh_hid_event_t *event = &(hid->report_event);
	int ret;

#if USBH_HID_REPORT_DESC_PARSE_DEBUG
	usb_ringbuf_manager_t *handle = &(hid->report_msg);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "RX HID Report:(%d-%d-%d):", len, handle->head, handle->tail);
	for (u8 i = 0; i < len; i++) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "%02x ", report[i]);
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
#endif

	ret = usbh_hid_parse_hid_report(report, len, &(hid->vol_caps));
	if (ret != HAL_OK) {
		return;
	}

	if (event->type != VOLUME_EVENT_NONE) {
		event->is_press = 1;
#if USBH_HID_DEBUG
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
static int usbh_hid_parse_details(usbh_itf_data_t *itf_data)
{
	usbh_hid_t *hid = &usbh_hid;
	usbh_dev_hid_desc_t *hid_desc = NULL;
	usbh_ep_desc_t *ep_desc = NULL;
	u8 *desc = itf_data->raw_data;
	u16 len = 0;
	u16 itf_total_len = 0;

	hid->itf_idx = desc[2];
	hid->itf_alt_idx = desc[3];
	/* Number of alt settings is already computed by the USB core when it
	 * built itf_data_array (see usbh_hcd_xfer.c). Use it directly instead
	 * of re-counting INTERFACE descriptors here. */
	hid->alt_setting_count = itf_data->alt_setting_cnt;

	while (desc != NULL) {
		/* Validate the descriptor at the *current* offset before dereferencing
		 * its header, otherwise the last iteration would read bLength and
		 * bDescriptorType from beyond the end of the raw_data window. */
		if ((u32)itf_total_len + sizeof(usbh_desc_header_t) > (u32)itf_data->raw_data_len) {
			break;    /* not even a descriptor header left */
		}

		len = ((usbh_desc_header_t *) desc)->bLength;
		if (len == 0U) {
			break;    /* malformed: bLength==0 would spin forever */
		}
		if ((u32)itf_total_len + (u32)len > (u32)itf_data->raw_data_len) {
			break;    /* bLength overshoots the raw_data window */
		}

		switch (((usbh_desc_header_t *) desc)->bDescriptorType) {
		case USB_DESC_TYPE_INTERFACE:
			if (((usbh_itf_desc_t *)desc)->bInterfaceNumber != hid->itf_idx) { //find another itf, should return
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "Hid intf new %d:old %d, return\n\n", ((usbh_itf_desc_t *)desc)->bInterfaceNumber, hid->itf_idx);
				return HAL_OK;
			}
			break;
		case USBH_HID_DESC:
			hid_desc = (usbh_dev_hid_desc_t *)desc;
			usb_os_memcpy((void *) & (hid->hid_desc), (const void *)hid_desc, sizeof(usbh_dev_hid_desc_t));
			break;

		case USB_DESC_TYPE_ENDPOINT:
			ep_desc = (usbh_ep_desc_t *)desc;
			if (USB_EP_IS_IN(ep_desc->bEndpointAddress)) {
				/* Interrupt IN �� receives HID reports from device */
				usb_os_memcpy((void *) & (hid->ep_desc_in), (const void *)ep_desc, sizeof(usbh_ep_desc_t));
			} else {
				/* Interrupt OUT �� sends output reports to device */
				usb_os_memcpy((void *) & (hid->ep_desc_out), (const void *)ep_desc, sizeof(usbh_ep_desc_t));
			}
			break;

		default:
			break;
		}

		desc += len;
		itf_total_len += len;
	}
	return HAL_OK;
}

/**
  * @brief  Parse configuration descriptor
  * @param  host: usb host structure
  * @retval Status
  */
static int usbh_hid_parse_interface(usb_host_t *host)
{
	usbh_dev_id_t dev_id = {0,};
	dev_id.bInterfaceClass = USBH_CLASS_HID;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS;
	usbh_itf_data_t *itf_data = usbh_get_interface_descriptor(host, &dev_id);
	int ret = HAL_OK;

	if (itf_data) {
		ret = usbh_hid_parse_details(itf_data);
		if (ret) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "AC parse fail\n");
			return ret;
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get if fail\n");
		return HAL_ERR_PARA;
	}

	return ret;
}

static int usbh_hid_process_get_hid_report_desc(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_hid_t *hid = &usbh_hid;

	if (hid->hid_desc.wDescriptorLength > USBH_HID_CTRL_BUF_LEN) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "HID report desc len %d exceed buf\n", hid->hid_desc.wDescriptorLength);
		return HAL_ERR_PARA;
	}

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_REQ_GET_DESCRIPTOR;
	setup.req.wValue = USBH_HID_REPORT_TYPE << 8;
	setup.req.wIndex = hid->itf_idx;
	setup.req.wLength = hid->hid_desc.wDescriptorLength;

	return usbh_ctrl_request(host, &setup, hid->hid_ctrl_buf);
}

/**
  * @brief  Get hid report descriptor
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_hid_handle_report_desc(usb_host_t *host)
{
	usbh_hid_t *hid = &usbh_hid;
	int ret = HAL_OK;

	if (hid->pipe_in.ep_addr && hid->report_desc == NULL) {
		//1. set itf
		//2. set idle
		//3. get report desc
		if (hid->report_desc_status == USBH_HID_REPORT_SET_ALT) {
			ret = usbh_ctrl_set_interface(host, hid->itf_idx, hid->itf_alt_idx);
			if (ret == HAL_OK) {
				hid->report_desc_status = USBH_HID_REPORT_GET_DESC;
				ret = HAL_BUSY;
			} else if (ret != HAL_BUSY) {
				/* SET_INTERFACE failed but the report descriptor fetch below can
				 * still succeed, so the sequence is in progress: report BUSY
				 * rather than leaking the error to setup(), which the core would
				 * read as a terminal failure of the whole HID class. */
				RTK_LOGS(TAG, RTK_LOG_ERROR, "HID set alt err %d\n", ret);
				usb_os_sleep_ms(5);
				hid->report_desc_status = USBH_HID_REPORT_GET_DESC;
				ret = HAL_BUSY;
			}
		} else if (hid->report_desc_status == USBH_HID_REPORT_GET_DESC) {
			ret = usbh_hid_process_get_hid_report_desc(host);
			if (ret == HAL_OK) {
				hid->report_desc_status = USBH_HID_REPORT_MAX;
				//parse report desc
				hid->report_desc = hid->hid_ctrl_buf;
				hid->hid_ctrl = 1;
				usbh_hid_parse_hid_report_desc(hid->report_desc, hid->hid_desc.wDescriptorLength);
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

/**
  * @brief  Process the INTR IN transfer for the HID report endpoint.
  *         Drives one step of the transfer state machine via
  *         usbh_transfer_process(): on completion (XFER_IDLE) the report is
  *         copied into the ring buffer; on START it re-notifies to trigger the
  *         next tick; on ERROR it logs the URB state. Mirrors
  *         usbh_cdc_ecm_process_intr_in().
  * @param  host: Host handle
  */
static void usbh_hid_in_process(usb_host_t *host)
{
	usbh_hid_t *hid = &usbh_hid;
	usbh_pipe_t *pipe = &(hid->pipe_in);
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	u32 len;

	switch (pipe->xfer_state) {
	case USBH_EP_XFER_START:
		if (usbh_get_elapsed_ticks(host, pipe->tick) > pipe->ep_interval) {
			pipe->tick = usbh_get_tick(host);
			pipe->xfer_len = pipe->ep_mps;
			if (usbh_transfer_data(host, pipe) == HAL_OK) {
				pipe->xfer_state = USBH_EP_XFER_BUSY;
			}
		}
		break;

	case USBH_EP_XFER_BUSY:
		urb_state = usbh_get_urb_state(host, pipe);
		if (urb_state == USBH_URB_DONE) {
			len = usbh_get_last_transfer_size(host, pipe);
			/* save to ring buffer */
			usb_ringbuf_add_tail(&(hid->report_msg), pipe->xfer_buf, len, 1);
			pipe->xfer_state = USBH_EP_XFER_START;
		} else if (urb_state == USBH_URB_BUSY) {
			if (usbh_get_elapsed_ticks(host, pipe->tick) > pipe->ep_interval) {
				pipe->xfer_state = USBH_EP_XFER_START;
			}
		} else if (urb_state == USBH_URB_ERROR) {
			pipe->xfer_state = USBH_EP_XFER_START;
		} else if (urb_state == USBH_URB_IDLE) {
			if (usbh_get_elapsed_ticks(host, pipe->tick) > (10) * pipe->ep_interval) {
				pipe->xfer_state = USBH_EP_XFER_START;
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
static int usbh_hid_attach(usb_host_t *host)
{
	usbh_hid_t *hid = &usbh_hid;
	usbh_pipe_t *pipe = NULL;

	int status = HAL_ERR_UNKNOWN;

	hid->host = host;

	status = usbh_hid_parse_interface(host);
	if (status) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Cfg parse fail\n");
		return status;
	}

	if (hid->ep_desc_in.bEndpointAddress) {
		pipe = &(hid->pipe_in);
		/* SET_INTERFACE is only required when more than one alt setting exists.
		 * For the typical single-alt HID interface, skip it to avoid an unneeded
		 * control transfer (and the 5 ms recovery on devices that STALL it). */
		hid->report_desc_status = (hid->alt_setting_count > 1) ? USBH_HID_REPORT_SET_ALT
								  : USBH_HID_REPORT_GET_DESC;

		if (usbh_open_pipe(host, pipe, &(hid->ep_desc_in), &usbh_hid_driver) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Open intr in pipe fail\n");
			return HAL_ERR_PARA;
		}
	}

	/* Open the Interrupt OUT pipe if the interface exposes one. */
	if (hid->ep_desc_out.bEndpointAddress) {
		if (usbh_open_pipe(host, &(hid->pipe_out), &(hid->ep_desc_out), &usbh_hid_driver) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Open intr out pipe fail\n");
			if (hid->pipe_in.pipe_num) {
				usbh_close_pipe(host, &(hid->pipe_in));
			}
			return HAL_ERR_PARA;
		}
	}

	if ((hid->cb != NULL) && (hid->cb->attached != NULL)) {
		hid->cb->attached();
	}

	return HAL_OK;
}

/**
  * @brief  Detach callback.
  * @param  host: Host handle
  * @retval None
  */
static void usbh_hid_detach(usb_host_t *host)
{
	usbh_hid_t *hid = &usbh_hid;

	if (hid->pipe_in.pipe_num) {
		usbh_close_pipe(host, &(hid->pipe_in));
	}

	if (hid->pipe_out.pipe_num) {
		usbh_close_pipe(host, &(hid->pipe_out));
	}

	hid->hid_ctrl = 0;
	hid->report_desc = NULL;
	usb_os_memset(&(hid->hid_desc), 0, sizeof(hid->hid_desc));
	usb_os_memset(&(hid->ep_desc_in), 0, sizeof(hid->ep_desc_in));
	usb_os_memset(&(hid->ep_desc_out), 0, sizeof(hid->ep_desc_out));
	usb_os_memset(&(hid->vol_caps), 0, sizeof(hid->vol_caps));
	hid->itf_idx = 0;
	hid->itf_alt_idx = 0;
	hid->alt_setting_count = 0;
	usb_ringbuf_reset(&(hid->report_msg));

	if ((hid->cb != NULL) && (hid->cb->detached != NULL)) {
		hid->cb->detached();
	}
}

/**
  * @brief  Standard control requests handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_hid_setup(usb_host_t *host)
{
	usbh_hid_t *hid = &usbh_hid;
	usbh_pipe_t *pipe = &(hid->pipe_in);
	int ret;

	ret = usbh_hid_handle_report_desc(host);
	if (ret != HAL_OK) {
		return ret;
	}

	if ((hid->cb != NULL) && (hid->cb->setup != NULL)) {
		hid->cb->setup();
	}

	pipe->xfer_state = USBH_EP_XFER_START;
	pipe->xfer_buf = hid->hid_ctrl_buf;

	return HAL_OK;
}

/**
  * @brief  Process the INTR OUT transfer for the HID report endpoint.
  *         Drives one step of the transfer state machine via
  *         usbh_transfer_process(): on completion (XFER_IDLE) the pipe is
  *         freed for the next usbh_hid_send_report(); on START it re-notifies
  *         to trigger the next tick; on ERROR it logs and restores IDLE so the
  *         pipe does not stay busy forever. Mirrors usbh_cdc_acm_process_tx().
  * @param  host: Host handle
  */
static void usbh_hid_out_process(usb_host_t *host)
{
	usbh_hid_t *hid = &usbh_hid;
	usbh_pipe_t *pipe_out = &(hid->pipe_out);

	usbh_transfer_process(host, pipe_out);

	if (pipe_out->xfer_state == USBH_EP_XFER_START) {
		usbh_notify(host, pipe_out->pipe_num, &usbh_hid_driver);
	} else if (pipe_out->xfer_state == USBH_EP_XFER_ERROR) {
		pipe_out->xfer_state = USBH_EP_XFER_IDLE;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "HID OUT fail: %d\n", usbh_get_urb_state(host, pipe_out));
	}
}

/**
  * @brief  State machine handling callback
  * @param  host: Host handle
  * @param  msg: USB host driver message
  * @retval None
  */
static void usbh_hid_process(usb_host_t *host, usbh_drv_msg_t *msg)
{
	usbh_hid_t *hid = &usbh_hid;
	usbh_pipe_t *pipe = &(hid->pipe_in);

	/* The core routes a driver message to the pipe's owning driver only, so the pipe
	 * checks are defensive guards against a stale/closed pipe. */
	if (msg && (hid->hid_ctrl_buf) && (pipe->pipe_num != 0) && (msg->pipe_num == pipe->pipe_num)) {
		usbh_hid_in_process(host);
	} else if (msg && (hid->pipe_out.pipe_num != 0) && (msg->pipe_num == hid->pipe_out.pipe_num)) {
		usbh_hid_out_process(host);
	}
}

/**
  * @brief  Sof callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  host: Host handle
  * @retval None
  */
static void usbh_hid_sof(usb_host_t *host)
{
	usbh_hid_t *hid = &usbh_hid;
	usbh_pipe_t *pipe = &(hid->pipe_in);

	if ((pipe->pipe_num != 0) &&
		(usbh_get_elapsed_ticks(host, pipe->tick) > USBH_HID_TRIGGER_MAX_CNT)) {
		usbh_notify(host, pipe->pipe_num, &usbh_hid_driver);
	}
}

static void usbh_hid_msg_parse_thread(void *param)
{
	UNUSED(param);
	usbh_hid_t *hid = &usbh_hid;
	usb_ringbuf_manager_t *handle = &(hid->report_msg);
	u8 report_msg[USBH_HID_MSG_LENGTH];
	u8 read_cnt;

	while (hid->parse_task_exit == 0) {
		read_cnt = usb_ringbuf_remove_head(handle, report_msg, USBH_HID_MSG_LENGTH, NULL);
		if (read_cnt) {
			if (hid->hid_ctrl) {
				usbh_hid_parse_hid_msg(report_msg, read_cnt);
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
int usbh_hid_init(const usbh_hid_usr_cb_t *cb)
{
	usbh_hid_t *hid = &usbh_hid;
	int ret;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	usb_os_memset((void *)hid, 0x00, sizeof(usbh_hid_t));

	hid->hid_ctrl_buf = (u8 *)usb_os_malloc(USBH_HID_CTRL_BUF_LEN);
	if (NULL == hid->hid_ctrl_buf) {
		return HAL_ERR_MEM;
	}

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "CB init fail\n");
			usb_os_mfree((void *)hid->hid_ctrl_buf);
			hid->hid_ctrl_buf = NULL;
			return ret;
		}
	}

	ret = usb_ringbuf_manager_init(&(hid->report_msg), USBH_HID_MST_COUNT, USBH_HID_MSG_LENGTH, 0);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Ringbuffer init fail\n");
		usb_os_mfree((void *)hid->hid_ctrl_buf);
		hid->hid_ctrl_buf = NULL;
		return ret;
	}

	/* Set before task creation, not inside the thread body: rtos_task_create()
	   may not run the new task immediately, so if the caller requested an exit
	   below before the thread got a chance to run, the thread's own startup
	   code would otherwise clobber that request back to "keep running" and
	   loop forever, hanging the wait loop below. */
	hid->parse_task_alive = 1;
	hid->parse_task_exit = 0;

	if (rtos_task_create(&(hid->msg_parse_task), ((const char *)"usbh_hid_msg_parse_thread"), usbh_hid_msg_parse_thread,
						 NULL, USBH_HID_THREAD_STACK_SIZE, USBH_HID_THREAD_PRIORITY) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create parse thread fail\n");
		hid->parse_task_alive = 0;
		usb_ringbuf_manager_deinit(&(hid->report_msg));
		usb_os_mfree((void *)hid->hid_ctrl_buf);
		hid->hid_ctrl_buf = NULL;
		return HAL_ERR_UNKNOWN;
	}

	hid->cb = cb;

	ret = usbh_register_class(&usbh_hid_driver);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Register class fail %d\n", ret);
		hid->parse_task_exit = 1;
		do {
			rtos_time_delay_ms(1);
		} while (hid->parse_task_alive);
		usb_ringbuf_manager_deinit(&(hid->report_msg));
		usb_os_mfree((void *)hid->hid_ctrl_buf);
		hid->hid_ctrl_buf = NULL;
		hid->cb = NULL;
		return ret;
	}

	return HAL_OK;
}

/**
  * @brief  Deinit hid class
  */
void usbh_hid_deinit(void)
{
	usbh_hid_t *hid = &usbh_hid;

	usbh_unregister_class(&usbh_hid_driver);

	if ((hid->cb != NULL) && (hid->cb->deinit != NULL)) {
		hid->cb->deinit();
	}

	if (hid->parse_task_alive) {
		hid->parse_task_exit = 1;
		do {
			rtos_time_delay_ms(1);
		} while (hid->parse_task_alive);
	}

	if (hid->pipe_in.pipe_num && hid->host) {
		usbh_close_pipe(hid->host, &(hid->pipe_in));
	}

	if (hid->pipe_out.pipe_num && hid->host) {
		usbh_close_pipe(hid->host, &(hid->pipe_out));
	}

	usb_os_mfree((void *)hid->hid_ctrl_buf);
	hid->hid_ctrl_buf = NULL;

	usb_ringbuf_manager_deinit(&(hid->report_msg));
}

/**
  * @brief  Send an output report to the HID device via the Interrupt OUT pipe.
  * @param  buf: Pointer to the report data buffer.
  * @param  len: Length of the report data in bytes.
  * @retval HAL_OK on success, HAL_ERR_PARA if no OUT pipe, HAL_BUSY if in progress.
  */
int usbh_hid_send_report(u8 *buf, u32 len)
{
	usbh_hid_t *hid = &usbh_hid;
	usbh_pipe_t *pipe = &(hid->pipe_out);

	if (!pipe->pipe_num) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "HID no OUT pipe\n");
		return HAL_ERR_PARA;
	}

	if (pipe->xfer_state != USBH_EP_XFER_IDLE) {
		return HAL_BUSY;
	}

	pipe->xfer_buf = buf;
	pipe->xfer_len = len;
	pipe->xfer_state = USBH_EP_XFER_START;
	usbh_notify(hid->host, pipe->pipe_num, &usbh_hid_driver);

	return HAL_OK;
}
