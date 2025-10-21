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

/* can be common code */
/**
  * @brief  Get the valid frame count
  * @param  handle: Pointer to the ringbuf control structure
  * @retval return frame count
  */
u32 usb_ringbuf_get_count(usb_ringbuf_manager_t *handle)
{
	u32 h, t;

	h = handle->head;
	t = handle->tail;

	if (t >= h) {
		return t - h;
	} else {
		return t + handle->item_cnt - h;
	}
}

/**
  * @brief  Check the ringbuf is empty
  * @param  handle: Pointer to the ringbuf control structure
  * @retval if empty return 1, else return 0
  */
int usb_ringbuf_is_empty(usb_ringbuf_manager_t *handle)
{
	return handle->head == handle->tail;
}

/**
  * @brief  Check the ringbuf is full
  * @param  handle: Pointer to the ringbuf control structure
  * @retval if full return 1, else return 0
  */
int usb_ringbuf_is_full(usb_ringbuf_manager_t *handle)
{
	return ((handle->tail + 1) % (handle->item_cnt)) == handle->head;
}

/**
  * @brief  Add a new frame to the ringbuf tail
  * @param  handle: Pointer to the ringbuf control structure
  * @param  buf: write data handle
  * @param  size: write data size
  * @retval if success return HAL_ERR_PARA, else return HAL_OK
  */
int usb_ringbuf_add_tail(usb_ringbuf_manager_t *handle, u8 *buf, u32 size)
{
	usb_ringbuf_t *target;
	u32 cur_tail = handle->tail;
	u32 next_tail = (cur_tail + 1) % (handle->item_cnt);

	if (usb_ringbuf_is_full(handle)) {
		return HAL_ERR_PARA;
	}

	target = &(handle->list_node[cur_tail]);
	//check the size
	if (size > handle->item_size) {
		size = handle->item_size;
	}
	target->buf_len = size;
	usb_os_memcpy((void *)(target->buf), (void *)buf, target->buf_len);

	mem_sync();

	handle->tail = next_tail;

	return HAL_OK;
}

/**
  * @brief  Read a frame from the ringbuf head
  * @param  handle: Pointer to the ringbuf control structure
  * @param  buf: read data handle
  * @param  size: read max data size
  * @retval return the copy data length
  */
u32 usb_ringbuf_remove_head(usb_ringbuf_manager_t *handle, u8 *buf, u32 size)
{
	usb_ringbuf_t *target;
	u32 cur_head = handle->head;
	u32 copy_len = size;

	if (usb_ringbuf_is_empty(handle)) {
		return 0;
	}

	if (buf == NULL || size == 0) {
		return 0;
	}

	target = &(handle->list_node[cur_head]);

	if (copy_len > target->buf_len) {
		copy_len = target->buf_len;
	}

	usb_os_memcpy((void *)buf, (void *)(target->buf), copy_len);
	target->buf_len = 0;

	mem_sync();

	handle->head = (cur_head + 1) % (handle->item_cnt);

	return copy_len;
}

/*
	The following APIs are used to write part of the data to ringbuf, take UAC as example:
	audio writes data of various lengths to ringbuf, and USB sends them out with dma mode.
	So there are two requirements: 1) the tx buf_addr should be cache aligned, 2) the length of the sent data is the length of one frame.

	Therefore, usb needs to split the consecutive data transmitted from audio one by one and save them,
	and ensure that the data at the end that does not meet the requirement of one frame is saved and can be combined with the data of the next time to form a complete data frame
	USB achieves this specifically through ringbuf, where each item saves a frame of data

	AudioData:     data1 data2 data3 data4 data4.......
	USB Ringbuf:   frame1pad frame2pad frame3pad frame4pad ....

	"Usb_ringbuf_write_partial" is used to write part of the data to ringbuf. The caller needs to control the amount of data written, which must not exceed the length of one frame
	"Usb_ringbuf_finish_write" is used to indicate that this frame has been written, and the write idx switch to next position
	"Usb_ringbuf_write_partial" combines "Usb_ringbuf_finish_write" to save the frame to the ringbuffer
*/

/**
  * @brief  Write partial data to the tail
  * @param  handle: Pointer to the ringbuf control structure
  * @param  buf: write data handle
  * @param  size: write data size
  * @retval if full return 1, else return 0
  */
int usb_ringbuf_write_partial(usb_ringbuf_manager_t *handle, u8 *buf, u32 size)
{
	usb_ringbuf_t *target;
	u32 cur_tail = handle->tail;
	u32 remain = handle->item_size - handle->written;

	if (size > remain) {
		size = remain;
	}

	target = &(handle->list_node[cur_tail]);

	usb_os_memcpy((void *)(target->buf + handle->written), (void *)buf, size);
	handle->written += size;
	target->buf_len = handle->written;

	mem_sync();

	return size;
}

/**
  * @brief  finish write a frame
  * @retval HAL_OK
  */
int usb_ringbuf_finish_write(usb_ringbuf_manager_t *handle)
{
	handle->tail = (handle->tail + 1) % (handle->item_cnt);
	handle->written = 0;

	return HAL_OK;
}

/*
	UAC use ringbuf memory for transmission, so the mmeory can not be overwritten until get the complete interrupt
	so call "usb_ringbuf_get_head" to get the buffer handle, and do the transmission
	and call "usb_ringbuf_release_head" to release the buffer in the complete interrupt
	just get the head, but not change the head idx
*/
usb_ringbuf_t *usb_ringbuf_get_head(usb_ringbuf_manager_t *handle)
{
	usb_ringbuf_t *target;
	u32 cur_head = handle->head;

	if (usb_ringbuf_is_empty(handle)) {
		return NULL;
	}

	target = &(handle->list_node[cur_head]);

	mem_sync();

	return target;
}

/* release the head */
int usb_ringbuf_release_head(usb_ringbuf_manager_t *handle)
{
	u32 cur_head = handle->head;

	if (usb_ringbuf_is_empty(handle)) {
		return 0;
	}

	handle->head = (cur_head + 1) % (handle->item_cnt);

	return 0;
}

int usb_ringbuf_manager_init(usb_ringbuf_manager_t *handle, u16 cnt, u16 size, u8 cache_align)
{
	usb_ringbuf_t *pbuf_data;
	u16 item_length = size;
	int idx;

	if (cnt == 0 || size == 0) {
		return HAL_ERR_PARA;
	}

	if (cache_align) {
		item_length = CACHE_LINE_ALIGMENT(size);
	}

	handle->written = 0;
	handle->item_cnt = cnt;
	handle->item_size = item_length;

	handle->buf = (u8 *)usb_os_malloc(handle->item_cnt * handle->item_size);
	if (handle->buf == NULL) {
		return HAL_ERR_MEM;
	}

	handle->list_node = (usb_ringbuf_t *)usb_os_malloc(handle->item_cnt * sizeof(usb_ringbuf_t));
	if (handle->list_node == NULL) {
		usb_os_mfree(handle->buf);
		handle->buf = NULL;
		return HAL_ERR_MEM;
	}

	for (idx = 0; idx < handle->item_cnt; idx++) {
		pbuf_data = &(handle->list_node[idx]);
		pbuf_data->buf_len = 0;
		pbuf_data->buf = handle->buf + handle->item_size * idx;
	}

	handle->head = 0;
	handle->tail = 0;

	return HAL_OK;
}

int usb_ringbuf_manager_deinit(usb_ringbuf_manager_t *handle)
{
	if (handle->buf != NULL) {
		usb_os_mfree(handle->buf);
		handle->buf = NULL;
	}

	if (handle->list_node != NULL) {
		usb_os_mfree(handle->list_node);
		handle->list_node = NULL;
	}

	return HAL_OK;
}

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
	RTK_LOGS(TAG, RTK_LOG_INFO, "Itf %d-%d\n", hid->itf_idx, hid->alt_set_idx);

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
			/* save to list */
			usb_ringbuf_add_tail(&(hid->report_msg), ep->buf, len);
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

static void usbh_composite_hid_parse_thread(void *param)
{
	UNUSED(param);
	usbh_composite_hid_t *hid = &usbh_composite_hid;
	usb_ringbuf_manager_t *handle = &(hid->report_msg);
	u8 report_msg[10];
	u32 read_cnt;

	hid->parse_task_alive = 1;
	hid->parse_task_exit = 0;

	RTK_LOGS(TAG, RTK_LOG_INFO, "HID parse thread\n");

	while (hid->parse_task_exit == 0) {
		read_cnt = usb_ringbuf_remove_head(handle, report_msg, 10);
		// RTK_LOGS(TAG, RTK_LOG_INFO, "HID parse thread cnt %d\n", read_cnt);
		if (read_cnt) {
			if (hid->hid_ctrl) {
				usbh_composite_hid_parse_hid_msg(report_msg, read_cnt);
				// RTK_LOGS(TAG, RTK_LOG_INFO, "HID parse thread done\n");
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

	if (rtos_task_create(&(hid->msg_parse_task), ((const char *)"usbh_composite_hid_parse"), usbh_composite_hid_parse_thread,
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