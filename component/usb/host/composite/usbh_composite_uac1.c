/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/
#include "usbh_composite_uac1.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#define USBH_DT_CS_INTERFACE                0x24
#define USBH_DT_CS_ENDPOINT                 0x25
#define USBH_UAC_WAIT_SLICE_MS              1
#define USBH_UAC_HFNUM_MAX_FRNUM            0x3FFF

#define USBH_UAC_FREQ(freq)                 (((u32)freq[0]) | (((u32)freq[1]) << 8) | (((u32)freq[2]) << 16))

#if USBH_COMPOSITE_HID_UAC_DEBUG
#define USBH_COMPOSITE_HID_UAC_DEBUG_LOOP_TIME            1000
#endif

/* Private function prototypes -----------------------------------------------*/
static int usbh_composite_uac_cb_attach(usb_host_t *host);
static int usbh_composite_uac_cb_detach(usb_host_t *host);
static int usbh_composite_uac_cb_process(usb_host_t *host, u32 msg);
static int usbh_composite_uac_ctrl_setting(usb_host_t *host, u32 msg);
static int usbh_composite_uac_cb_setup(usb_host_t *host);
static int usbh_composite_uac_cb_sof(usb_host_t *host);
static int usbh_composite_uac_cb_complete(usb_host_t *host, u8 pipe);
static void usbh_composite_uac_isoc_in_process(usb_host_t *host);
static void usbh_composite_uac_isoc_out_process_xfer(usb_host_t *host, u32 cur_frame);
static void usbh_composite_uac_isoc_out_process_complete(usb_host_t *host);
static int usbh_composite_uac_process_set_out_alt(usb_host_t *host);
static int usbh_composite_uac_process_set_out_freq(usb_host_t *host);
static void usbh_composite_uac_deinit_pipe(u8 dir);
static void usbh_composite_uac_deinit_all_pipe(void);
static int usbh_composite_uac_parse_cfgdesc(usb_host_t *host);
static int usbh_composite_uac_parse_ac(u8 *pbuf, u32 *length, u32 buf_length);
static int usbh_composite_uac_parse_as(u8 *pbuf, u32 *length, u32 buf_length);
static int usbh_composite_uac_usb_status_check(void);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "UAC";

static const u8 usbd_composite_uac_pc_vol_lvl[] = {0, 5, 10, 20, 30, 40, 50, 60, 65, 75, 80, 85, 90, 95, 100};
static const s16 usbd_composite_uac_drv_vol[] = {-190, -170, -151, -112, -74, -37, 0, 37, 56, 93, 112, 132, 151, 170, 190};

/* USB Standard Device Descriptor */
const usbh_class_driver_t usbh_composite_uac_driver = {
	.class_code = USB_CLASS_AUDIO,
	.attach = usbh_composite_uac_cb_attach,
	.detach = usbh_composite_uac_cb_detach,
	.setup = usbh_composite_uac_cb_setup,
	.process = usbh_composite_uac_cb_process,
	.sof = usbh_composite_uac_cb_sof,
	.complete = usbh_composite_uac_cb_complete,
};

static usbh_composite_uac_t usbh_composite_uac;

#if USBH_COMPOSITE_HID_UAC_DEBUG
static void usbh_composite_uac_reset_test_cnt(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;

	uac->sof_cnt = 0;
	uac->isoc_tx_start_cnt = 0;
	uac->isoc_tx_done_cnt = 0;
	uac->isoc_xfer_buf_empty_cnt = 0;
	uac->isoc_xfer_buf_err_cnt = 0;
	uac->isoc_xfer_interval_cnt = 0;
}

static void usbh_composite_uac_dump_as_desc(usbh_uac_as_itf_info_t *puacdesc)
{
	usbh_uac_as_itf_alt_info_t *pasintf = NULL;
	usbh_uac_format_cfg_t *fmt = NULL;
	usbh_ep_desc_t *audio_ep = NULL;
	int i, k;
	if (puacdesc == NULL) {
		return;
	}

	for (i = 0; i < puacdesc->alt_cnt; i ++) {
		pasintf = &(puacdesc->alt_set_array[i]);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Intf=%d alts %d\n", puacdesc->as_itf_num, pasintf->alt_setting);
		fmt = &(pasintf->format_info);
		audio_ep = &(pasintf->ep_desc);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Ep%02x MPS %d %d\n", audio_ep->bEndpointAddress, audio_ep->wMaxPacketSize, audio_ep->bInterval);

		RTK_LOGS(TAG, RTK_LOG_INFO, "Ch %d byte %d freqcnt %d\n", fmt->channels, fmt->bit_width, fmt->freq_cnt);
		for (k = 0; k < fmt->freq_cnt; k++) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "\t\tFre:%dhz\n", USBH_UAC_FREQ(fmt->freq[k]));
		}
	}
}

/**
  * @brief  UAC status dump thread
  * @param  param: Pointer to parameters
  * @retval void
  */
extern void usbh_composite_hid_status_dump_thread(void);
static void usbh_composite_uac_status_dump(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out);
	usb_ringbuf_manager_t *handle = &(buf_ctrl->buf_list);
	usbh_uac_ep_cfg_t *ep = &(uac->isoc_out_info->ep_info);

	if (usbh_composite_uac_usb_status_check() == HAL_OK) {
		int ready_cnt = usb_ringbuf_get_count(handle);
#if USBH_ISR_TASK_TIME_DEBUG
		if (uac && uac->driver && uac->driver->host) {
			usb_host_t *host = uac->driver->host;
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "UAC:%d-%d %d-%d/%d-%d-%d-%d/xfer=%d-%d-%d %d-%d-%d/%d-%d-%d-%d\n",
					 ready_cnt, (handle->item_cnt - ready_cnt), handle->head, handle->tail,
					 buf_ctrl->transfer_continue, uac->ctrl_state, uac->xfer_state, ep->isoc_state,
					 (u32)(uac->sof_cnt), (u32)(uac->isoc_tx_start_cnt), (u32)(uac->isoc_tx_done_cnt),
					 (u32)(uac->isoc_xfer_buf_empty_cnt), (u32)(uac->isoc_xfer_buf_err_cnt), (u32)(uac->isoc_xfer_interval_cnt),
					 host->isr_func_time_cost_max, host->isr_func_time_cost,
					 host->isr_trigger_time_diff_max, host->isr_trigger_time_diff);
		}
#else
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "UAC:%d-%d %d-%d/%d-%d-%d-%d/xfer=%d-%d-%d %d-%d-%d\n",
				 ready_cnt, (handle->item_cnt - ready_cnt), handle->head, handle->tail,
				 buf_ctrl->transfer_continue, uac->ctrl_state, uac->xfer_state, ep->isoc_state,
				 (u32)(uac->sof_cnt), (u32)(uac->isoc_tx_start_cnt), (u32)(uac->isoc_tx_done_cnt),
				 (u32)(uac->isoc_xfer_buf_empty_cnt), (u32)(uac->isoc_xfer_buf_err_cnt), (u32)(uac->isoc_xfer_interval_cnt));
#endif
		usbh_composite_hid_status_dump_thread();
	}
}

static void usbh_composite_uac_status_dump_thread(void *param)
{
	UNUSED(param);
	usbh_composite_uac_t *uac = &usbh_composite_uac;

	uac->dump_status_task_alive = 1;
	uac->dump_status_task_exit = 1;

	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC dump thread\n");

	while (uac->dump_status_task_exit) {

		usbh_composite_uac_status_dump();

		rtos_time_delay_ms(USBH_COMPOSITE_HID_UAC_DEBUG_LOOP_TIME);
	}

	uac->dump_status_task_alive = 0;
	rtos_task_delete(NULL);
}
#endif

static void usbh_composite_uac_reset_isr_time(void)
{
#if USBH_ISR_TASK_TIME_DEBUG
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usb_host_t *host;

	if (uac && uac->driver && uac->driver->host) {
		host = uac->driver->host;;

		host->isr_func_time_cost_max = 0;
		host->isr_trigger_time_diff_max = 0;

		host->isr_trigger_last_time = 0;
	}
#endif
}

static u32 usbh_composite_uac_test(u16 argc, u8 *argv[])
{
	int status = HAL_OK;
	const char *cmd;

	if (argc == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid argument\n");
		return HAL_ERR_PARA;
	}

	cmd = (const char *)argv[0];
	RTK_LOGS(TAG, RTK_LOG_INFO, "Cmd %s\n", cmd);
	if (_stricmp(cmd, "mute") == 0) {
		u8 mute = 1;
		if (argv[1]) {
			mute = (u8)_strtoul((const char *)(argv[1]), (char **)NULL, 10);
		}

		usbh_composite_uac_set_mute(mute);
	} else if (_stricmp(cmd, "vol") == 0) {
		u8 vol = 50;
		u8 ch = 0;
		if (argv[1]) {
			vol = (u8)_strtoul((const char *)(argv[1]), (char **)NULL, 10);
			if (argv[2]) {
				ch = (u8)_strtoul((const char *)(argv[2]), (char **)NULL, 10);
			}
		}

		usbh_composite_uac_set_volume(vol, ch);
	} else if (_stricmp(cmd, "isr") == 0) {
		usbh_composite_uac_reset_isr_time();
	} else if (_stricmp(cmd, "reset") == 0) {
#if USBH_COMPOSITE_HID_UAC_DEBUG
		usbh_composite_uac_reset_test_cnt();
#endif
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Input cmd err %s\n", cmd);
	}

	return status;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbh_composite_uac_test_md_table[] = {
	{
		(const u8 *)"uach", 3, usbh_composite_uac_test, (const u8 *)"\tUSB host uac test cmd:\n"
		"\t\t uach cmd\n"
	}
};

static inline u32 usbh_composite_uac_frame_num_dec(u32 new, u32 start)
{
	if (new >= start) {
		return new - start;
	} else {
		return (USBH_UAC_HFNUM_MAX_FRNUM - start + 1 + new);
	}
}

static s16 usbh_composite_uac_volume_to_db(u8 target_x)
{
	s32 denominator;
	s32 numerator;
	s32 y;
	s16 y0;
	s16 y1;
	u8 num_points;
	u8 x0;
	u8 x1;
	u8 i;

	if (target_x > 100) {
		return 190;
	}
	if (target_x == 0) {
		return -190;
	}

	num_points = sizeof(usbd_composite_uac_pc_vol_lvl) / sizeof(usbd_composite_uac_pc_vol_lvl[0]);
	for (i = 0; i < num_points - 1; i++) {
		x0 = usbd_composite_uac_pc_vol_lvl[i];
		x1 = usbd_composite_uac_pc_vol_lvl[i + 1];
		y0 = usbd_composite_uac_drv_vol[i];
		y1 = usbd_composite_uac_drv_vol[i + 1];

		if (target_x >= x0 && target_x <= x1) {
			numerator = (s32)(target_x - x0) * (y1 - y0);
			denominator = (x1 - x0);
			y = (numerator + denominator / 2) / denominator + y0;
			return (s16)y;
		}
	}

	return 0x00;
}

static void usbh_composite_uac_dump_cfgdesc(void)
{
#if USBH_COMPOSITE_HID_UAC_DEBUG
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_as_itf_info_t *puacdesc = NULL;
	RTK_LOGS(TAG, RTK_LOG_INFO, "--------------------Dump Start------------------------------\n");

	if (uac->isoc_out_info) {
		puacdesc = uac->isoc_out_info;
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB OUT at_cnt %d itf %d\n", puacdesc->alt_cnt, puacdesc->as_itf_num);
		//dump uac ac
		//dump uac as
		usbh_composite_uac_dump_as_desc(puacdesc);
	}

	if (uac->isoc_in_info) {
		puacdesc = uac->isoc_in_info;
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB IN at_cnt %d itf %d\n", puacdesc->alt_cnt, puacdesc->as_itf_num);
		//dump uac ac
		//dump uac as
		usbh_composite_uac_dump_as_desc(puacdesc);
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "---------------------Dump End-----------------------------\n");
#endif
}

static usbh_uac_as_itf_info_t *usbh_composite_uac_get_as_itf_instance(u8 dir)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_as_itf_info_t *as_itf = NULL;

	if (dir == USBH_UAC_ISOC_OUT_DIR) {
		as_itf = uac->isoc_out_info;
	} else {
		as_itf = uac->isoc_in_info;
	}

	if (as_itf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No support audio %s\n", ((dir == USBH_UAC_ISOC_OUT_DIR) ? ("OUT") : ("IN")));
		return NULL;
	}

	return as_itf;
}

static void usbh_composite_uac_get_audio_format(usbh_uac_as_itf_info_t *puacdesc)
{
	usbh_uac_as_itf_alt_info_t *pasintf = NULL;
	usbh_uac_format_cfg_t *fmt = NULL;
	usbh_audio_fmt_t *pfmt_info = NULL;
	u8 fmt_cnt = 0;
	u8 fmt_idx = 0;
	u8 i, k;

	if (puacdesc == NULL) {
		return;
	}

	for (i = 0; i < puacdesc->alt_cnt; i ++) {
		pasintf = &(puacdesc->alt_set_array[i]);
		fmt = &(pasintf->format_info);
		fmt_cnt += fmt->freq_cnt;
	}

	puacdesc->fmt_array = (usbh_audio_fmt_t *)usb_os_malloc(sizeof(usbh_audio_fmt_t) * fmt_cnt);
	if (puacdesc->fmt_array == NULL) {
		return;
	}
	puacdesc->fmt_array_cnt = fmt_cnt;

	fmt_idx = 0;
	for (i = 0; i < puacdesc->alt_cnt; i ++) {
		fmt = &(puacdesc->alt_set_array[i].format_info);
		for (k = 0; k < fmt->freq_cnt; k++) {
			pfmt_info = &(puacdesc->fmt_array[fmt_idx++]);

			pfmt_info->sampling_freq = USBH_UAC_FREQ(fmt->freq[k]);
			pfmt_info->bit_width = fmt->bit_width;
			pfmt_info->ch_cnt = fmt->channels;
		}
	}
}

static u32 usbh_composite_uac_get_audio_format_info(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;

	usbh_composite_uac_get_audio_format(uac->isoc_out_info);
	usbh_composite_uac_get_audio_format(uac->isoc_in_info);

	return HAL_OK;
}

static void usbh_composite_uac_deinit_pipe(u8 dir)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usb_host_t *host = uac->driver->host;
	usbh_uac_as_itf_info_t *as_itf;
	usbh_uac_ep_cfg_t *ep = NULL;

	as_itf = usbh_composite_uac_get_as_itf_instance(dir);
	if (as_itf == NULL) {
		return ;
	}

	ep = &(as_itf->ep_info);
	if (ep->isoc_pipe) {
		usbh_close_pipe(host, ep->isoc_pipe);
		usbh_free_pipe(host, ep->isoc_pipe);
		ep->isoc_pipe = 0U;
	}
}

static void  usbh_composite_uac_deinit_all_pipe(void)
{
	usbh_composite_uac_deinit_pipe(USBH_UAC_ISOC_OUT_DIR);
	usbh_composite_uac_deinit_pipe(USBH_UAC_ISOC_IN_DIR);
}

static int usbh_composite_uac_usb_status_check(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;

	if (uac->driver->state < USBH_COMPOSITE_SETUP) {
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
static u8 *usbh_composite_uac_find_next_stdesc(u8 *pbuf, u32 *len)
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

static int usbh_composite_uac_is_output_terminal(u16 type)
{
	if (type > 0x300) {
		return 1;
	}

	return 0;
}

static void usbh_composite_uac_add_terminal(usbh_uac_ac_itf_info_t *list, const usbh_uac_term_info *term)
{
	if (list && term && list->terminal_count < USBH_UAC_TERM_MAX_CNT) {
		usb_os_memcpy(&(list->terminals[list->terminal_count]), term, sizeof(usbh_uac_term_info));
		list->terminal_count++;
	}
}

static void usbh_composite_uac_add_volume(usbh_uac_ac_itf_info_t *list, const usbh_uac_vol_ctrl_info *info)
{
	if (list && info && list->volume_ctrl_count < USBH_UAC_FU_MAX_CNT) {
		usb_os_memcpy(&(list->controls[list->volume_ctrl_count]), info, sizeof(usbh_uac_vol_ctrl_info));
		list->volume_ctrl_count++;
	}
}

static int usbh_composite_uac_find_best_ac(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->isoc_ac_info);

	usbh_uac_vol_ctrl_info *best = NULL;
	usbh_uac_vol_ctrl_info *info;
	int best_priority = -1;
	u32 i;

	for (i = 0; i < ac_info->volume_ctrl_count; i++) {
		usbh_uac_vol_ctrl_info *current = &(ac_info->controls[i]);
		int priority = 0;

		if (!usbh_composite_uac_is_output_terminal(current->sink_type)) {
			continue;
		}

		if (current->sink_type == UAC_TERMINAL_TYPE_HEADPHONE) {
			priority = 2;
		} else if (current->sink_type == UAC_TERMINAL_TYPE_SPEAKER) {
			priority = 1;
		} else {
			priority = 0;
		}

		if (current->master_support) {
			priority += 10;
		}

		if (priority > best_priority) {
			best_priority = priority;
			best = current;
			ac_info->best_match_idx = i;
		}
	}

	if (best == NULL) {
		return HAL_OK;
	}

	info = &(ac_info->controls[ac_info->best_match_idx]);

	if (!info) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "can not find\n");
		return HAL_OK;
	}

#if USBH_COMPOSITE_HID_UAC_DEBUG
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "UAC 1.0 :\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "  ID: 0x%02X\n", info->sink_id);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "  ID(unit_id): 0x%02X\n", info->unit_id);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "  ID(link): 0x%02X\n", info->source_id);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "  size: %dbyte\n", info->control_size);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "  mas: %s\n", info->master_support ? "Y" : "N");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "  ch: %d\n", info->num_channels);

	for (int i = 0; i < info->num_channels; i++) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "  ch %d support: %s\n",
				 i + 1, info->channel_support[i] ? "Y" : "N");
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nRequest:\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "  bmRequestType: 0x21\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "  bRequest:  0x01(SET_CUR)\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "  wValue:    0x%02x00 (funcID << 8 | main ch)\n", info->unit_id);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "  wIndex:    0x00%02x (itf << 8 | ctrl)\n", UAC_FU_VOLUME);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "  wLength:   %d (data length)\n", info->control_size);
#endif

	return HAL_OK;
}

/**
  * @brief  Parse audio streaming interface
  * @param  pbuf: given descriptor buffer
  * @param  length: length of parse desc length
  * @param  buf_length: length of given buffer
  * @retval Status
  */
static int usbh_composite_uac_parse_ac(u8 *pbuf, u32 *length, u32 buf_length)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->isoc_ac_info);
	usb_ac_itf_desc_header_t *ac_header;
	u8 *desc;
	u32 t;
	u8 ch;
	u8 type;
	u8 subtype;
	u8 len;

	desc = pbuf;
	//save the first interface number as the ac itf_idx
	ac_info->ac_itf_idx = desc[2];

	//loop to get all input/out terminal

	while (1) {
		if (desc == NULL || *length >= buf_length) {
			break;
		}

		type = ((usb_descriptor_header_t *) desc)->bDescriptorType;
		if (type == USB_DESC_TYPE_INTERFACE) {
			if (((usbh_if_desc_t *)desc)->bInterfaceNumber != ac_info->ac_itf_idx) { //find another itf, maybe it is the as itf, should return
				RTK_LOGS(TAG, RTK_LOG_INFO, "AC intf new %d:old %d, return\n\n", ((usbh_if_desc_t *)desc)->bInterfaceNumber, ac_info->ac_itf_idx);
				break;
			}

			len = ((usb_descriptor_header_t *) desc)->bLength;
			desc = desc + len;
			*length += len;
		} else if (type == USBH_DT_CS_INTERFACE) {
			len = ((usb_descriptor_header_t *) desc)->bLength;
			ac_header = (usb_ac_itf_desc_header_t *)desc;
			subtype = ac_header->bDescriptorSubtype;

			if (subtype == UAC_HEADER) {
				//get the total ac length
			} else if (subtype == UAC_INPUT_TERMINAL) {
				if (len >= 0x0C) {
					usbh_uac_term_info term = {
						.terminal_id = desc[3],
						.terminal_type = (desc[5] << 8) | desc[4],
						.is_input = 1
					};
					usbh_composite_uac_add_terminal(ac_info, &term);
				}
			} else if (subtype == UAC_OUTPUT_TERMINAL) {
				if (len >= 0x09) {
					usbh_uac_term_info term = {
						.terminal_id = desc[3],
						.terminal_type = (desc[5] << 8) | desc[4],
						.source_id = desc[7],
						.is_input = 0
					};
					usbh_composite_uac_add_terminal(ac_info, &term);
				}
			}

			desc = desc + len;
			*length += len;
		} else {
			len = ((usb_descriptor_header_t *) desc)->bLength;
			desc = desc + len;
			*length += len;
		}
	}

	desc = pbuf;
	while (1) {
		if (desc == NULL) {
			break;
		}

		type = ((usb_descriptor_header_t *) desc)->bDescriptorType;
		if (type == USB_DESC_TYPE_INTERFACE) {
			if (((usbh_if_desc_t *)desc)->bInterfaceNumber != ac_info->ac_itf_idx) { //find another itf, maybe it is the as itf, should return
				RTK_LOGS(TAG, RTK_LOG_INFO, "AC intf new %d:old %d, return\n\n", ((usbh_if_desc_t *)desc)->bInterfaceNumber, ac_info->ac_itf_idx);
				break;
			}

			len = ((usb_descriptor_header_t *) desc)->bLength;
			desc = desc + len;
		} else if (type == USBH_DT_CS_INTERFACE) {
			len = ((usb_descriptor_header_t *) desc)->bLength;
			ac_header = (usb_ac_itf_desc_header_t *)desc;
			subtype = ac_header->bDescriptorSubtype;

			if (subtype == UAC_FEATURE_UNIT) {
				if (len >= 0x07) {
					usbh_uac_vol_ctrl_info vol_info = {0};
					vol_info.unit_id = desc[3];
					vol_info.source_id = desc[4];
					vol_info.control_size = desc[5];
					vol_info.num_channels = (len - 6) - 1;

					if (vol_info.num_channels > 8) {
						vol_info.num_channels = 8;
					}

					vol_info.master_support = (desc[6] & UAC_FU_VOLUME) ? 1 : 0;

					for (ch = 0; ch < vol_info.num_channels; ch++) {
						if (7 + ch < len) {
							vol_info.channel_support[ch] = (desc[7 + ch] & UAC_FU_VOLUME) ? 1 : 0;
						}
					}

					for (t = 0; t < ac_info->terminal_count; t++) {
						if (!ac_info->terminals[t].is_input &&
							ac_info->terminals[t].source_id == vol_info.unit_id) {

							vol_info.sink_id = ac_info->terminals[t].terminal_id;
							vol_info.sink_type = ac_info->terminals[t].terminal_type;
							break;
						}
					}

					if (vol_info.master_support || vol_info.num_channels > 0) {
						usbh_composite_uac_add_volume(ac_info, &vol_info);
					}
				}
			}

			desc = desc + len;
		} else {
			len = ((usb_descriptor_header_t *) desc)->bLength;
			desc = desc + len;
		}
	}

	return HAL_OK;
}

/**
  * @brief  Parse audio streaming interface
  * @param  pbuf: given descriptor buffer
  * @param  length: length of parse desc length
  * @param  buf_length: length of given buffer
  * @retval Status
  */
static int usbh_composite_uac_parse_as(u8 *pbuf, u32 *length, u32 buf_length)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_as_itf_info_t *as_itf = NULL;
	usbh_uac_as_itf_alt_info_t *alt_setting = NULL;
	usbh_uac_format_cfg_t *format_info = NULL;
	usbh_ep_desc_t *ep_cfg = NULL;

	u8 *desc = pbuf;
	u16 len = 0;
	u8 alt_set_idx;
	// u8 is_audio_in;
	u8 k;

	as_itf = (usbh_uac_as_itf_info_t *)usb_os_malloc(sizeof(usbh_uac_as_itf_info_t));
	if (as_itf == NULL) {
		return HAL_ERR_MEM;
	}

	//uac alt setting 0, desc += length, jump the alt_setting=0 descriptor
	as_itf->as_itf_num = desc[2];
	*length = ((usb_descriptor_header_t *) desc)->bLength;
	desc = pbuf + *length;

	while (1) {
		if (desc == NULL || *length >= buf_length) {
			break;
		}

		switch (((usb_descriptor_header_t *) desc)->bDescriptorType) {
		case USB_DESC_TYPE_INTERFACE:
			if (((usbh_if_desc_t *)desc)->bInterfaceNumber != as_itf->as_itf_num) {
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "AS intf new %d:old %d, return\n", ((usbh_if_desc_t *)desc)->bInterfaceNumber, as_itf->as_itf_num);
				return HAL_OK;
			}

			alt_set_idx = ((usbh_if_desc_t *)desc)->bAlternateSetting;
			if ((alt_set_idx != 0) && (as_itf->alt_cnt < USBH_UAC_ALT_SETTING_MAX)) {
				alt_setting = &(as_itf->alt_set_array[as_itf->alt_cnt]);
				as_itf->alt_cnt ++;

				len = ((usb_descriptor_header_t *) desc)->bLength;
				desc = desc + len;
				*length += len;
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "AS parse return %d %d\n", as_itf->alt_cnt, USBH_UAC_ALT_SETTING_MAX);
				return HAL_OK;
			}
			break;

		case USBH_DT_CS_INTERFACE: {
			struct uac_format_type_i_discrete_descriptor *psubtype = (struct uac_format_type_i_discrete_descriptor *)desc;
			if ((alt_setting != NULL) && (UAC_FORMAT_TYPE_II == psubtype->bDescriptorSubtype)) { /* get the format */
				format_info = &(alt_setting->format_info);
				format_info->channels = psubtype->bNrChannels;
				format_info->bit_width = psubtype->bBitResolution;
				format_info->freq_cnt = psubtype->bSamFreqType;

				if (format_info->freq_cnt > USBH_UAC_FREQ_FORMAT_MAX) {
					RTK_LOGS(TAG, RTK_LOG_WARN, "Freq cnt(%d) > cfg(%d) limit\n", format_info->freq_cnt, USBH_UAC_FREQ_FORMAT_MAX);
					format_info->freq_cnt = USBH_UAC_FREQ_FORMAT_MAX;
				}

				for (k = 0; k < format_info->freq_cnt; k++) {
					usb_os_memcpy(&(format_info->freq[k]), &(psubtype->tSamFreq[k]), 3);
				}
			}

			len = ((usb_descriptor_header_t *) desc)->bLength;
			desc = desc + len;
			*length += len;
		}
		break;

		case USB_DESC_TYPE_ENDPOINT: {
			usbh_ep_desc_t *ep_desc = (usbh_ep_desc_t *)desc;
			if (alt_setting != NULL) {
				ep_cfg = &(alt_setting->ep_desc);
				usb_os_memcpy(ep_cfg, ep_desc, sizeof(usbh_ep_desc_t));

				if (USB_EP_IS_IN(ep_desc->bEndpointAddress)) {
					// is_audio_in = 1;
					if (uac->isoc_in_info == NULL) {
						uac->isoc_in_info = as_itf;
					}
				} else {
					// is_audio_in = 0;
					if (uac->isoc_out_info == NULL) {
						uac->isoc_out_info = as_itf;
					}
				}
			}

			len = ((usb_descriptor_header_t *) desc)->bLength;
			desc = desc + len;
			*length += len;
		}
		break;

		case USBH_DT_CS_ENDPOINT:
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
  * @brief	Parse configuration descriptor
  * @param	host: usb host structure
  * @retval Status
  */
static int usbh_composite_uac_parse_cfgdesc(usb_host_t *host)
{
	int ret = HAL_OK;
	usbh_cfg_desc_t *desc = (usbh_cfg_desc_t *)usbh_get_active_raw_configuration_descriptor(host);
	usbh_if_desc_t *pbuf = (usbh_if_desc_t *) desc;
	u32 cfglen = (u32)desc->wTotalLength;
	u32 len = 0;

	while (1) {
		pbuf = (usbh_if_desc_t *)usbh_composite_uac_find_next_stdesc((u8 *)pbuf, &cfglen);
		if (pbuf == NULL) {
			break;
		}

		if (pbuf->bInterfaceClass == USB_CLASS_AUDIO) {
			// only check audio interface
			if (pbuf->bInterfaceProtocol != 0x00) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid device, only support UAC1.0\n");
				ret = HAL_ERR_UNKNOWN;
				break;
			}

			switch (pbuf->bInterfaceSubClass) {
			case USB_SUBCLASS_AUDIOCONTROL: //audio control
				ret = usbh_composite_uac_parse_ac((u8 *)pbuf, &len, cfglen);
				if (ret) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "AC parse fail\n");
					return ret;
				} else {
					usbh_composite_uac_find_best_ac();
				}

				pbuf = (usbh_if_desc_t *)((u8 *) pbuf + len);
				cfglen -= len;
				len = 0;
				break;

			case USB_SUBCLASS_AUDIOSTREAMING: //audio streaming
				if (pbuf->bAlternateSetting == 0) { //setting 0
					ret = usbh_composite_uac_parse_as((u8 *)pbuf, &len, cfglen);
					if (ret != HAL_OK) {
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

static void usbh_uac_dump_req_struct(usb_setup_req_t *ctrl)
{
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Request 0x%02x 0x%02x 0x%04x 0x%04x 0x%02x\n",
			 ctrl->bmRequestType,
			 ctrl->bRequest,
			 ctrl->wValue,
			 ctrl->wIndex,
			 ctrl->wLength);
}

static int usbh_composite_uac_process_set_out_alt(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;

	setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE;
	setup.b.bRequest = USB_REQ_SET_INTERFACE;
	setup.b.wValue = uac->isoc_out_info->choose_alt_idx + 1;
	setup.b.wIndex = uac->isoc_out_info->as_itf_num;
	setup.b.wLength = 0U;
	usbh_uac_dump_req_struct(&setup.b);

	return usbh_ctrl_request(host, &setup, NULL);
}

static int usbh_composite_uac_process_set_in_alt(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;

	setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE;
	setup.b.bRequest = USB_REQ_SET_INTERFACE;
	setup.b.wValue = uac->isoc_in_info->choose_alt_idx + 1;
	setup.b.wIndex = uac->isoc_in_info->as_itf_num;
	setup.b.wLength = 0U;
	usbh_uac_dump_req_struct(&setup.b);

	return usbh_ctrl_request(host, &setup, NULL);
}

static int usbh_composite_uac_process_set_out_freq(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_as_itf_info_t *as_itf = uac->isoc_out_info;
	usbh_uac_format_cfg_t *fmt_info = &(as_itf->alt_set_array[as_itf->choose_alt_idx].format_info);

	setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_ENDPOINT;
	setup.b.bRequest = UAC_SET_CUR;
	setup.b.wValue = USBH_UAC_SAMPLING_FREQ_CONTROL;
	setup.b.wIndex = as_itf->ep_info.isoc_ep_addr;
	setup.b.wLength = 3U;
	usbh_uac_dump_req_struct(&setup.b);

	usb_os_memcpy(uac->audio_ctrl_buf, fmt_info->freq[as_itf->choose_freq_idx], 3);

	return usbh_ctrl_request(host, &setup, uac->audio_ctrl_buf);
}

static int usbh_composite_uac_process_set_in_freq(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_as_itf_info_t *as_itf = uac->isoc_in_info;
	usbh_uac_format_cfg_t *fmt_info = &(as_itf->alt_set_array[as_itf->choose_alt_idx].format_info);

	setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_ENDPOINT;
	setup.b.bRequest = UAC_SET_CUR;
	setup.b.wValue = USBH_UAC_SAMPLING_FREQ_CONTROL;
	setup.b.wIndex = as_itf->ep_info.isoc_ep_addr;
	setup.b.wLength = 3U;
	usbh_uac_dump_req_struct(&setup.b);

	usb_os_memcpy(uac->audio_ctrl_buf, fmt_info->freq[as_itf->choose_freq_idx], 3);

	return usbh_ctrl_request(host, &setup, uac->audio_ctrl_buf);
}

static int usbh_composite_uac_process_set_volume(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->isoc_ac_info);
	usbh_uac_vol_ctrl_info *info = &(ac_info->controls[ac_info->best_match_idx]);

	setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.b.bRequest = UAC_SET_CUR;
	setup.b.wValue = (uac->ch_idx) | (UAC_FU_VOLUME << 8);
	setup.b.wIndex = (ac_info->ac_itf_idx) | (info->unit_id << 8);
	setup.b.wLength = 2U;
	usbh_uac_dump_req_struct(&setup.b);

	usb_os_memcpy(uac->audio_ctrl_buf, &(uac->volume_info), 2);

	return usbh_ctrl_request(host, &setup, uac->audio_ctrl_buf);
}

static int usbh_composite_uac_process_set_mute(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->isoc_ac_info);
	usbh_uac_vol_ctrl_info *info = &(ac_info->controls[ac_info->best_match_idx]);

	setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.b.bRequest = UAC_SET_CUR;
	setup.b.wValue = UAC_FU_MUTE << 8;
	setup.b.wIndex = (ac_info->ac_itf_idx) | (info->unit_id << 8);
	setup.b.wLength = 1U;
	usbh_uac_dump_req_struct(&setup.b);

	uac->audio_ctrl_buf[0] = uac->mute;

	return usbh_ctrl_request(host, &setup, uac->audio_ctrl_buf);
}

/**
  * @brief  Iso in test
  * @param  host: Host handle
  * @retval None
*/
static void usbh_composite_uac_isoc_in_process(usb_host_t *host)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_ep_cfg_t *ep = &(uac->isoc_in_info->ep_info);
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	u32 len;

	//rx to do
	switch (ep->isoc_state) {
	case USBH_UAC_XFER:
		if ((host->tick - ep->isoc_tick) >= ep->isoc_interval) {
			ep->isoc_tick = host->tick;
			usbh_isoc_receive_data(host,
								   ep->isoc_buf,
								   ep->isoc_packet_size,
								   ep->isoc_pipe);

			ep->isoc_state = USBH_UAC_XFER_BUSY;
		}
		usbh_notify_class_state_change(host, 0);
		break;

	case USBH_UAC_XFER_BUSY:
		urb_state = usbh_get_urb_state(host, ep->isoc_pipe);
		if (urb_state == USBH_URB_DONE) {
			len = usbh_get_last_transfer_size(host, ep->isoc_pipe);
			if ((uac->cb != NULL) && (uac->cb->isoc_received != NULL)) {
				uac->cb->isoc_received(ep->isoc_buf, len);
			}
			ep->isoc_state = USBH_UAC_EP_IDLE;
		} else if (urb_state == USBH_URB_ERROR) {
			ep->isoc_state = USBH_UAC_XFER;
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
static void usbh_composite_uac_isoc_out_process_xfer(usb_host_t *host, u32 cur_frame)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_ep_cfg_t *ep = &(uac->isoc_out_info->ep_info);
	usbh_uac_buf_ctrl_t *pdata_ctrl = &(uac->isoc_out);
	usb_ringbuf_t *pbuf = NULL;

	//handle overflow
	if (usbh_composite_uac_frame_num_dec(cur_frame, uac->cur_frame) >= ep->isoc_interval) {
		if (!usb_ringbuf_is_empty(&(pdata_ctrl->buf_list))) {
			// check valid data
			pbuf = usb_ringbuf_get_head(&(pdata_ctrl->buf_list));
			uac->cur_frame = usbh_get_current_frame(host);
			if (pbuf && pbuf->buf_len > 0) {
#if USBH_COMPOSITE_HID_UAC_DEBUG
				uac->isoc_tx_start_cnt ++;
#endif
				ep->isoc_buf = pbuf->buf;
				ep->isoc_len = pbuf->buf_len;
				usbh_isoc_send_data(host,
									ep->isoc_buf,
									(u16)ep->isoc_len,
									ep->isoc_pipe);
				ep->isoc_state = USBH_UAC_XFER_BUSY;
			} else { //data invalid
#if USBH_COMPOSITE_HID_UAC_DEBUG
				uac->isoc_xfer_buf_err_cnt ++;
#endif
			}
		} else { //ringbuf empty
#if USBH_COMPOSITE_HID_UAC_DEBUG
			uac->isoc_xfer_buf_empty_cnt ++;
#endif
		}
	} else { // interval
#if USBH_COMPOSITE_HID_UAC_DEBUG
		uac->isoc_xfer_interval_cnt ++;
#endif
	}
}

static void usbh_composite_uac_isoc_out_process_complete(usb_host_t *host)
{
	UNUSED(host);
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_ep_cfg_t *ep = &(uac->isoc_out_info->ep_info);
	usbh_uac_buf_ctrl_t *pdata_ctrl = &(uac->isoc_out);

#if USBH_COMPOSITE_HID_UAC_DEBUG
	uac->isoc_tx_done_cnt ++;
#endif

	usb_ringbuf_release_head(&(pdata_ctrl->buf_list));

	rtos_sema_give(pdata_ctrl->isoc_sema);

	ep->isoc_state = USBH_UAC_EP_IDLE;
}

/**
  * @brief  Attach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_uac_cb_attach(usb_host_t *host)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_as_itf_info_t *as_itf = NULL;
	usbh_ep_desc_t *ep_desc = NULL;
	usbh_uac_ep_cfg_t *ep = NULL;

	int status = HAL_ERR_UNKNOWN;
	u32 max_ep_size;

	if (host->config.speed == USB_SPEED_HIGH) {
		max_ep_size = 1024;
	} else {
		max_ep_size = 1023;
	}

	status = usbh_composite_uac_parse_cfgdesc(host);
	if (status) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Cfg parse fail\n");
		return status;
	}
	usbh_composite_uac_dump_cfgdesc();

	usbh_composite_uac_get_audio_format_info();

	uac->ctrl_state = UAC_STATE_CTRL_IDLE;

	if (uac->isoc_in_info) {
		as_itf = uac->isoc_in_info;
		as_itf->choose_alt_idx = 0;

		ep = &(as_itf->ep_info);
		ep_desc = &(as_itf->alt_set_array[as_itf->choose_alt_idx].ep_desc);

		ep->isoc_ep_addr = ep_desc->bEndpointAddress;
		ep->isoc_packet_size = ep_desc->wMaxPacketSize;
		ep->isoc_interval = ep_desc->bInterval;
		ep->isoc_state = USBH_UAC_EP_IDLE;

		if (ep->isoc_packet_size >= max_ep_size) {
			ep->isoc_packet_size = max_ep_size;
		}

		ep->isoc_pipe = usbh_alloc_pipe(host, ep->isoc_ep_addr);

		usbh_open_pipe(host,
					   ep->isoc_pipe,
					   ep->isoc_ep_addr,
					   USB_CH_EP_TYPE_ISOC,
					   ep->isoc_packet_size);
	}

	if (uac->isoc_out_info) {
		as_itf = uac->isoc_out_info;
		as_itf->choose_alt_idx = 0;

		ep = &(as_itf->ep_info);
		ep_desc = &(as_itf->alt_set_array[as_itf->choose_alt_idx].ep_desc);

		ep->isoc_ep_addr = ep_desc->bEndpointAddress;
		ep->isoc_packet_size = ep_desc->wMaxPacketSize;
		ep->isoc_interval = ep_desc->bInterval;
		ep->isoc_state = USBH_UAC_EP_IDLE;

		if (ep->isoc_packet_size >= max_ep_size) {
			ep->isoc_packet_size = max_ep_size;
		}

		ep->isoc_pipe = usbh_alloc_pipe(host, ep->isoc_ep_addr);

		usbh_open_pipe(host,
					   ep->isoc_pipe,
					   ep->isoc_ep_addr,
					   USB_CH_EP_TYPE_ISOC,
					   ep->isoc_packet_size);
	}

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
static int usbh_composite_uac_cb_detach(usb_host_t *host)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	UNUSED(host);

	uac->xfer_state = UAC_STATE_IDLE;

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
static int usbh_composite_uac_cb_setup(usb_host_t *host)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
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
static int usbh_composite_uac_cb_sof(usb_host_t *host)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_buf_ctrl_t *pdata_ctrl = &(uac->isoc_out);
	u32 cur_frame = usbh_get_current_frame(host);

	/* this class right not just support isoc out */
	if (pdata_ctrl->transfer_continue == 1) {
#if USBH_COMPOSITE_HID_UAC_DEBUG
		uac->sof_cnt ++;
#endif
		usbh_composite_uac_isoc_out_process_xfer(host, cur_frame);
	}

	if (uac->ctrl_state != UAC_STATE_CTRL_IDLE) {
		usbh_notify_class_state_change(host, 0x00);
	}

	return HAL_OK;
}

/**
  * @brief  Xfer complete callback
  * @param  host: Host handle
  * @param  pipe: pipe index
  * @retval Status
  */
static int usbh_composite_uac_cb_complete(usb_host_t *host, u8 pipe)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_buf_ctrl_t *pdata_ctrl = &(uac->isoc_out);

	if (pdata_ctrl->transfer_continue == 1) {
		if ((uac->isoc_out_info) && (pipe == uac->isoc_out_info->ep_info.isoc_pipe)) {
			usbh_composite_uac_isoc_out_process_complete(host);
		}
	}

	return HAL_OK;
}

/**
  * @brief  Ctrl State machine handling
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_composite_uac_ctrl_setting(usb_host_t *host, u32 msg)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	int ret = HAL_OK;
	int ret_status = HAL_BUSY;
	UNUSED(msg);

	switch (uac->ctrl_state) {
	case UAC_STATE_CTRL_IDLE:
		ret_status = HAL_OK;
		break;

	case UAC_STATE_SET_IN_ALT:
		ret = usbh_composite_uac_process_set_in_alt(host);
		if (ret == HAL_OK) {
			uac->ctrl_state = UAC_STATE_SET_IN_FREQ;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "IN alt err\n");
			uac->ctrl_state = UAC_STATE_SET_IN_FREQ;
		}
		break;
	case UAC_STATE_SET_IN_FREQ:
		ret = usbh_composite_uac_process_set_in_freq(host);
		if (ret == HAL_OK) {
			uac->ctrl_state = UAC_STATE_CTRL_IDLE;
			ret_status = HAL_OK;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "IN freq err\n");
			uac->ctrl_state = UAC_STATE_CTRL_IDLE;
			ret_status = HAL_OK;
		}
		break;

	case UAC_STATE_SET_OUT_ALT:
		ret = usbh_composite_uac_process_set_out_alt(host);
		if (ret == HAL_OK) {
			uac->ctrl_state = UAC_STATE_SET_OUT_FREQ;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "OUT alt err\n");
			uac->ctrl_state = UAC_STATE_SET_OUT_FREQ;
		}
		break;
	case UAC_STATE_SET_OUT_FREQ:
		ret = usbh_composite_uac_process_set_out_freq(host);
		if (ret == HAL_OK) {
			uac->ctrl_state = UAC_STATE_CTRL_IDLE;
			ret_status = HAL_OK;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "OUT freq err\n");
			uac->ctrl_state = UAC_STATE_CTRL_IDLE;
			ret_status = HAL_OK;
		}
		break;

	case UAC_STATE_SET_MUTE:
		ret = usbh_composite_uac_process_set_mute(host);
		if (ret == HAL_OK) {
			uac->ctrl_state = UAC_STATE_CTRL_IDLE;
			ret_status = HAL_OK;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set mute err\n");
			uac->ctrl_state = UAC_STATE_CTRL_IDLE;
			ret_status = HAL_OK;
		}
		break;

	case UAC_STATE_SET_VOLUME:
		ret = usbh_composite_uac_process_set_volume(host);
		if (ret == HAL_OK) {
			uac->ctrl_state = UAC_STATE_CTRL_IDLE;
			ret_status = HAL_OK;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set vol err\n");
			uac->ctrl_state = UAC_STATE_CTRL_IDLE;
			ret_status = HAL_OK;
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
static int usbh_composite_uac_cb_process(usb_host_t *host, u32 msg)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out);
	int ret = HAL_OK;

	switch (uac->xfer_state) {
	case UAC_STATE_TRANSFER:
		if ((buf_ctrl->transfer_continue) && (uac->isoc_in_info) && (msg == uac->isoc_in_info->ep_info.isoc_pipe)) {
			uac->next_xfer = 0;
			usbh_composite_uac_isoc_in_process(host);
			if (uac->next_xfer) {
				usbh_notify_class_state_change(host, uac->isoc_in_info->ep_info.isoc_pipe);
			}
		} else if (msg == 0x00) { //ctrl msg
			ret = usbh_composite_uac_ctrl_setting(host, 0);
			if (ret != HAL_OK) {
				ret = usbh_notify_class_state_change(host, 0);
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

static u32 usbh_composite_uac_next_packet_size(usbh_uac_buf_ctrl_t *pdata_ctrl)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_ep_cfg_t *ep = &(uac->isoc_out_info->ep_info);
	u32 sample_accum = 0;
	u32 ret;

	sample_accum = pdata_ctrl->sample_accum + pdata_ctrl->sample_rem;
	if (sample_accum >= pdata_ctrl->pkt_per_second) {
		sample_accum -= pdata_ctrl->pkt_per_second;
		ret = ep->isoc_packet_size;
	} else {
		ret = ep->isoc_packet_size_small;
	}

	pdata_ctrl->last_sample_accum = sample_accum;

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
static int usbh_composite_uac_write_ring_buf(usbh_uac_buf_ctrl_t *pdata_ctrl, u8 *buffer, u32 size, u32 *written_len)
{
	usb_ringbuf_manager_t *handle = &(pdata_ctrl->buf_list);
	u32 written_size = handle->written;
	u32 offset = 0;
	u32 mps;

	if (written_size) {
		mps = usbh_composite_uac_next_packet_size(pdata_ctrl);
		u32 can_copy = mps - written_size;
		u32 copy_len = size < can_copy ? size : can_copy;

		usb_ringbuf_write_partial(handle, buffer, copy_len);

		offset += copy_len;
		size -= copy_len;
		*written_len += copy_len;

		usb_ringbuf_finish_write(handle);
		pdata_ctrl->sample_accum = pdata_ctrl->last_sample_accum;
	}

	do {
		if (usb_ringbuf_is_full(handle)) {
			return 1;
		}

		mps = usbh_composite_uac_next_packet_size(pdata_ctrl);

		if (size >= mps) {
			usb_ringbuf_add_tail(handle, buffer + offset, mps);

			*written_len += mps;
			size -= mps;
			offset += mps;

			pdata_ctrl->sample_accum = pdata_ctrl->last_sample_accum;
		} else {
			break;
		}
	} while (1);

	if (size > 0) {
		if (usb_ringbuf_is_full(handle)) {
			return 1;
		}

		usb_ringbuf_write_partial(handle, buffer + offset, size);

		*written_len += size;
	}

	return 0;
}

/**
  * @brief  Deinitialize UAC endpoint buffer control structure
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @retval void
  */
static void usbh_composite_uac_ep_buf_ctrl_deinit(usbh_uac_buf_ctrl_t *buf_ctrl, usbh_uac_ep_cfg_t *ep)
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
	usb_ringbuf_manager_deinit(&(buf_ctrl->buf_list));
}

/**
  * @brief  Initialize UAC endpoint buffer control structure
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @param  ep: Pointer to ep parameters structure
  * @retval Status
  */
static int usbh_composite_uac_ep_buf_ctrl_init(usbh_uac_buf_ctrl_t *buf_ctrl, usbh_uac_ep_cfg_t *ep)
{
	int ret = HAL_ERR_MEM;
	u16 buf_list_cnt;

	buf_list_cnt = buf_ctrl->frame_cnt;
	buf_ctrl->mps = ep->isoc_packet_size;

	if (buf_list_cnt == 0 || buf_ctrl->mps == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Err param: cnt=%d, mps=%d\n", buf_list_cnt, buf_ctrl->mps);
		return ret;
	}

	usb_ringbuf_manager_init(&(buf_ctrl->buf_list), buf_list_cnt, buf_ctrl->mps, 1);

	rtos_sema_create(&(buf_ctrl->isoc_sema), 0U, 1U);
	buf_ctrl->sema_valid = 1;

	return HAL_OK;
}

/**
  * @brief  Wait for an isochronous buffer to become available and check USB transfer status.
  * @param  pdata_ctrl: Pointer to the USB UAC buffer control structure.
  * @param  timeout_ms:  Time out
  * @retval Status
  */
static int usbh_composite_uac_wait_isoc_with_status_check(usbh_uac_buf_ctrl_t *pdata_ctrl, uint32_t timeout_ms)
{
	int ret = HAL_ERR_PARA;
	u32 elapsed = 0;
	u32 wait_time = 0;

	while (elapsed < timeout_ms) {
		if (usbh_composite_uac_usb_status_check() != HAL_OK) {
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

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init uac class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_composite_uac_init(usbh_composite_host_t *driver, usbh_composite_uac_usr_cb_t *cb, int frame_cnt)
{
	int ret;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out);

	usb_os_memset(uac, 0x00, sizeof(usbh_composite_uac_t));
	uac->driver = driver;
	buf_ctrl->frame_cnt = frame_cnt;

	uac->audio_ctrl_buf = (u8 *)usb_os_malloc(UBSH_UAC_AUDIO_FMT_FREQ_LEN);
	if (NULL == uac->audio_ctrl_buf) {
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

#if USBH_COMPOSITE_HID_UAC_DEBUG
	if (rtos_task_create(&(uac->dump_status_task), ((const char *)"usbh_uac_status_dump_thread"), usbh_composite_uac_status_dump_thread, NULL, 1024U,
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
int usbh_composite_uac_deinit(void)
{
	int ret = HAL_OK;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_ep_cfg_t *ep = &(uac->isoc_out_info->ep_info);

	usbh_composite_uac_stop_play();

#if USBH_COMPOSITE_HID_UAC_DEBUG
	if (uac->dump_status_task_alive) {
		uac->dump_status_task_exit = 0;
		do {
			rtos_time_delay_ms(1);
		} while (uac->dump_status_task_alive);
	}
#endif

	if ((uac->cb != NULL) && (uac->cb->deinit != NULL)) {
		uac->cb->deinit();
	}

	usbh_composite_uac_deinit_all_pipe();
	usbh_composite_uac_ep_buf_ctrl_deinit(&(uac->isoc_out), ep);

	if (uac->audio_ctrl_buf != NULL) {
		usb_os_mfree(uac->audio_ctrl_buf);
		uac->audio_ctrl_buf = NULL;
	}

	if (uac->isoc_out_info != NULL) {
		if (uac->isoc_out_info->fmt_array) {
			usb_os_mfree(uac->isoc_out_info->fmt_array);
		}
		usb_os_mfree(uac->isoc_out_info);
		uac->isoc_out_info = NULL;
	}

	if (uac->isoc_in_info != NULL) {
		if (uac->isoc_in_info->fmt_array) {
			usb_os_mfree(uac->isoc_in_info->fmt_array);
		}
		usb_os_mfree(uac->isoc_in_info);
		uac->isoc_in_info = NULL;
	}

	return ret;
}

/**
  * @brief  Set a matched alt_setting
  * @param  dir: Dir of the audio interface, 0 menas out interface, 1 means in interface
  * @param  channels: Channels
  * @param  bit_width: bit_width
  * @param  sampling_freq: sampling_freq
  * @retval Status
  */
int usbh_composite_uac_set_alt_setting(u8 dir, u8 channels, u8 bit_width, u32 sampling_freq)
{
	usbh_uac_format_cfg_t *fmt;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_buf_ctrl_t *pdata_ctrl = &(uac->isoc_out);
	usbh_uac_as_itf_info_t *as_itf = NULL;
	usbh_uac_ep_cfg_t *ep = NULL;
	usbh_ep_desc_t *ep_desc = NULL;
	usb_host_t *host;
	int ret = HAL_ERR_PARA;
	int set_flag = 0;
	int i, j;
	u8 itf_num, alt_num;

	as_itf = usbh_composite_uac_get_as_itf_instance(dir);
	if (as_itf == NULL) {
		return HAL_ERR_PARA;
	}

	itf_num = as_itf->as_itf_num;
	alt_num = as_itf->alt_cnt;
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Search alt %d/%d\n", itf_num, alt_num);

	//actuaclly search from alt 1
	for (i = 0; i < alt_num; i++) {
		fmt = &(as_itf->alt_set_array[i].format_info);

		// Check format type, channels, and sample frequency
		if (!fmt || fmt->bit_width != bit_width || fmt->channels != channels) {
			continue;
		}

		// Check sample frequency
		for (j = 0; j < fmt->freq_cnt; j++) {
			if (USBH_UAC_FREQ(fmt->freq[j]) == sampling_freq) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Found itf %d %d/%d %d %d\n", itf_num, i + 1, channels, bit_width, sampling_freq);
				set_flag = 1;// Return as soon as we find a match
				break;
			}
		}

		if (set_flag) {
			break;
		}
	}

	if (set_flag) {
		as_itf->choose_alt_idx = i;
		as_itf->choose_freq_idx = j;
		pdata_ctrl->sample_freq = sampling_freq;

		//update ep_info
		ep = &(as_itf->ep_info);
		ep_desc = &(as_itf->alt_set_array[as_itf->choose_alt_idx].ep_desc);

		ep->isoc_ep_addr = ep_desc->bEndpointAddress;
		ep->isoc_interval = ep_desc->bInterval;

		/* full speed*/
		pdata_ctrl->pkt_per_second = USBH_UAC_ONE_KHZ >> (ep->isoc_interval - 1);

		pdata_ctrl->sample_rem = sampling_freq % pdata_ctrl->pkt_per_second;
		//calculate accurate one frame size(byte)
		ep->isoc_packet_size_small = channels * bit_width / USBH_UAC_BIT_TO_BYTE * (sampling_freq / pdata_ctrl->pkt_per_second);
		ep->isoc_packet_size = channels * bit_width / USBH_UAC_BIT_TO_BYTE * ((sampling_freq + (pdata_ctrl->pkt_per_second - 1)) / pdata_ctrl->pkt_per_second);

		usbh_composite_uac_stop_play();
		host = uac->driver->host;
		//reinit pipe
		usbh_composite_uac_deinit_pipe(dir);
		ep->isoc_pipe = usbh_alloc_pipe(host, ep->isoc_ep_addr);
		usbh_open_pipe(host, ep->isoc_pipe, ep->isoc_ep_addr, USB_CH_EP_TYPE_ISOC, ep->isoc_packet_size);

		// reinit ring buf
		if (dir == USBH_UAC_ISOC_OUT_DIR) {
			usbh_composite_uac_ep_buf_ctrl_deinit(&(uac->isoc_out), ep);
			ret = usbh_composite_uac_ep_buf_ctrl_init(&(uac->isoc_out), ep);
		}

		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Buf init fail");
			return ret;
		}

		uac->xfer_state = UAC_STATE_TRANSFER;
		if (dir == USBH_UAC_ISOC_OUT_DIR) {
			uac->ctrl_state = UAC_STATE_SET_OUT_ALT;
		} else {
			uac->ctrl_state = UAC_STATE_SET_IN_ALT;
		}

		usbh_notify_class_state_change(host, 0);
		ret = HAL_OK;
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alt not found %d %d %d\n", channels, bit_width, sampling_freq);
		ret = HAL_ERR_PARA;
	}

	return ret;
}

/**
  * @brief  Get alt setting structure and alt settings num for an interface.
  * @param  dir: Dir of the audio interface, 0 menas out interface, 1 means in interface
  * @param  fmt_cnt: Number of audio format
  * @retval Pointer to audio format array
  */
const usbh_audio_fmt_t *usbh_composite_uac_get_alt_setting(u8 dir, u8 *fmt_cnt)
{
	usbh_uac_as_itf_info_t *as_itf = NULL;

	as_itf = usbh_composite_uac_get_as_itf_instance(dir);
	if (as_itf == NULL) {
		return NULL;
	}

	if (fmt_cnt != NULL) {
		*fmt_cnt = as_itf->fmt_array_cnt;
	}

	return as_itf->fmt_array;
}

/**
  * @brief  Get frame size of current interface.
  * @param  intf_idx: Index of the audio interface
  * @retval frame size
  */
u32 usbh_composite_uac_get_frame_size(u8 dir)
{
	usbh_uac_as_itf_info_t *as_itf = NULL;
	usbh_uac_ep_cfg_t *ep = NULL;

	as_itf = usbh_composite_uac_get_as_itf_instance(dir);
	if (as_itf == NULL) {
		return 0;
	}

	ep = &(as_itf->ep_info);

	return ep->isoc_packet_size;
}

/**
  * @brief  Write Audio data
  * @param  buffer: Pointer to data buffer
  * @param  size: Data length
  * @param  timeout_ms: Time out
  * @retval written len
  */
u32 usbh_composite_uac_write(u8 *buffer, u32 size, u32 timeout_ms)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
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

	if (usbh_composite_uac_usb_status_check() != HAL_OK) {
		return 0;
	}

	while (written_len < size && pdata_ctrl->transfer_continue) {
		need_wait = 0;

		if (timeout_ms) {
			if (usb_ringbuf_is_full(&(pdata_ctrl->buf_list)) || last_zero) {
				need_wait = 1;
			}
		} else {
			if (usb_ringbuf_is_full(&(pdata_ctrl->buf_list)) || last_zero) {
				break;
			}
		}

		if (need_wait) {
			if (usbh_composite_uac_wait_isoc_with_status_check(pdata_ctrl, timeout_ms) != HAL_OK) {
				break;
			}
			last_zero = 0;
		}

		try_len = size - written_len;
		just_written = 0;

		usbh_composite_uac_write_ring_buf(pdata_ctrl, buffer + written_len, try_len, &just_written);
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
void usbh_composite_uac_start_play(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out);
	usbh_uac_ep_cfg_t *ep = &(uac->isoc_out_info->ep_info);

#if USBH_COMPOSITE_HID_UAC_DEBUG
	usbh_composite_uac_reset_test_cnt();
#endif
	usbh_composite_uac_reset_isr_time();

	buf_ctrl->transfer_continue = 1;
	uac->xfer_state = UAC_STATE_TRANSFER;
	ep->isoc_state = USBH_UAC_XFER;
	usbh_notify_class_state_change(uac->driver->host, ep->isoc_pipe);

	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC START %d\n", uac->ctrl_state);
}

/**
  * @brief  Stop UAC device play
  * @param  void
  * @retval void
  */
void usbh_composite_uac_stop_play(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out);

	buf_ctrl->transfer_continue = 0;

#if USBH_COMPOSITE_HID_UAC_DEBUG
	usbh_composite_uac_reset_test_cnt();
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC stop\n");
}

int usbh_composite_uac_set_volume(u8 volume, u8 ch)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	int ret = HAL_BUSY;
	u16 volume_value;

	if (usbh_composite_uac_usb_status_check() != HAL_OK) {
		return ret;
	}

	volume_value = (u16)usbh_composite_uac_volume_to_db(volume);
	if ((uac->xfer_state == UAC_STATE_IDLE) || (uac->xfer_state == UAC_STATE_TRANSFER)) {
		if (uac->ctrl_state == UAC_STATE_CTRL_IDLE) {
			uac->volume_info = volume_value;
			uac->ch_idx = ch;
			uac->ctrl_state = UAC_STATE_SET_VOLUME;
			uac->xfer_state = UAC_STATE_TRANSFER;
			usbh_notify_class_state_change(uac->driver->host, 0x00);
			ret = HAL_OK;
		}
	}

	return ret;
}

int usbh_composite_uac_set_mute(u8 mute)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	int ret = HAL_BUSY;

	if (usbh_composite_uac_usb_status_check() != HAL_OK) {
		return ret;
	}

	if ((uac->xfer_state == UAC_STATE_IDLE) || (uac->xfer_state == UAC_STATE_TRANSFER)) {
		if (uac->ctrl_state == UAC_STATE_CTRL_IDLE) {
			uac->mute = mute;
			uac->ctrl_state = UAC_STATE_SET_MUTE;
			uac->xfer_state = UAC_STATE_TRANSFER;
			usbh_notify_class_state_change(uac->driver->host, 0x00);

			ret = HAL_OK;
		}
	}

	return ret;
}
