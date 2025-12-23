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
#define USB_OTG_HFNUM_FRNUM_MAX             (0x3FFFUL)       /* Frame number max value */

#define USBH_LE16(addr)                     (((u16)(addr)[0]) | ((u16)(((u32)(addr)[1]) << 8)))
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
static int usbh_composite_uac_cb_completed(usb_host_t *host, u8 pipe);
static void usbh_composite_uac_isoc_in_process(usb_host_t *host);
static void usbh_composite_uac_isoc_out_process_xfer(usb_host_t *host, u32 cur_frame);
static void usbh_composite_uac_isoc_out_process_complete(usb_host_t *host);
static int usbh_composite_uac_process_set_out_alt(usb_host_t *host);
static int usbh_composite_uac_process_set_out_freq(usb_host_t *host);
static void usbh_composite_uac_deinit_pipe(u8 dir);
static void usbh_composite_uac_deinit_all_pipe(void);
static int usbh_composite_uac_parse_interface_desc(usb_host_t *host);
static int usbh_composite_uac_parse_ac(usbh_itf_data_t *ac_itf);
static int usbh_composite_uac_parse_as(usbh_itf_data_t *as_itf);
static int usbh_composite_uac_usb_status_check(void);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "UAC";

/* USB Standard Device Descriptor */
const usbh_class_driver_t usbh_composite_uac_driver = {
	.attach = usbh_composite_uac_cb_attach,
	.detach = usbh_composite_uac_cb_detach,
	.setup = usbh_composite_uac_cb_setup,
	.process = usbh_composite_uac_cb_process,
	.sof = usbh_composite_uac_cb_sof,
	.completed = usbh_composite_uac_cb_completed,
};

static usbh_composite_uac_t usbh_composite_uac;

#if USBH_COMPOSITE_HID_UAC_DEBUG
static void usbh_composite_uac_dump_as_desc(usbh_uac_as_itf_info_t *as_info)
{
	usbh_uac_as_itf_alt_info_t *as_alt_info = NULL;
	usbh_uac_format_cfg_t *fmt = NULL;
	usbh_ep_desc_t *audio_ep = NULL;
	int i, k;
	if (as_info == NULL) {
		return;
	}

	for (i = 0; i < as_info->alt_cnt; i ++) {
		as_alt_info = &(as_info->itf_info_array[i]);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Intf=%d alts %d\n", as_info->as_itf_num, as_alt_info->alt_setting);
		fmt = &(as_alt_info->format_info);
		audio_ep = &(as_alt_info->ep_desc);
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
extern void usbh_composite_hid_status_dump(void);
static void usbh_composite_uac_status_dump(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out);
	usb_ringbuf_manager_t *handle = &(buf_ctrl->buf_list);
	usbh_pipe_t *pipe = &(uac->as_isoc_out->pipe);

	if (usbh_composite_uac_usb_status_check() == HAL_OK) {
		int ready_cnt = usb_ringbuf_get_count(handle);
		if (uac && uac->driver && uac->driver->host) {
			usb_host_t *host = uac->driver->host;
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "UAC:%d-%d %d-%d/%d-%d-%d-%d/xfer=%d-%d-%d %d-%d-%d/%d-%d-%d-%d\n",
					 ready_cnt, (handle->item_cnt - ready_cnt), handle->head, handle->tail,
					 buf_ctrl->next_xfer, uac->ctrl_state, uac->xfer_state, pipe->xfer_state,
					 (u32)(uac->sof_cnt), (u32)(uac->isoc_tx_start_cnt), (u32)(uac->isoc_tx_done_cnt),
					 (u32)(uac->isoc_xfer_buf_empty_cnt), (u32)(uac->isoc_xfer_buf_err_cnt), (u32)(uac->isoc_xfer_interval_cnt),
					 host->isr_process_time_max, host->isr_process_time,
					 host->isr_enter_period_max, host->isr_enter_period);
		}
		usbh_composite_hid_status_dump();
	}
}

static void usbh_composite_uac_status_dump_thread(void *param)
{
	UNUSED(param);
	usbh_composite_uac_t *uac = &usbh_composite_uac;

	uac->dump_status_task_alive = 1;
	uac->dump_status_task_exit = 1;

	while (uac->dump_status_task_exit) {

		usbh_composite_uac_status_dump();

		rtos_time_delay_ms(USBH_COMPOSITE_HID_UAC_DEBUG_LOOP_TIME);
	}

	uac->dump_status_task_alive = 0;
	rtos_task_delete(NULL);
}

static void usbh_composite_uac_reset_isr_time(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usb_host_t *host;

	if (uac && uac->driver && uac->driver->host) {
		host = uac->driver->host;;

		host->isr_process_time_max = 0;
		host->isr_enter_period_max = 0;
		host->isr_enter_time = 0;
	}
}

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
#endif

static u32 usbh_composite_uac_test(u16 argc, u8 *argv[])
{
	static u8 vol_bk = 0;
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
		if (argv[1]) {
			vol = (u8)_strtoul((const char *)(argv[1]), (char **)NULL, 10);
		}
		vol_bk = vol;
		usbh_composite_uac_set_volume(vol_bk);
	} else if (_stricmp(cmd, "volup") == 0) {
		vol_bk += 5;
		if (vol_bk >= 100) {
			vol_bk = 100;
		}
		usbh_composite_uac_set_volume(vol_bk);
	} else if (_stricmp(cmd, "voldown") == 0) {
		if (vol_bk < 5) {
			vol_bk = 0;
		} else {
			vol_bk -= 5;
		}
		usbh_composite_uac_set_volume(vol_bk);
#if USBH_COMPOSITE_HID_UAC_DEBUG
	} else if (_stricmp(cmd, "isr") == 0) {
		usbh_composite_uac_reset_isr_time();
	} else if (_stricmp(cmd, "reset") == 0) {
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
		return (USB_OTG_HFNUM_FRNUM_MAX - start + 1 + new);
	}
}

/*
 * Increments frame by the amount specified by inc. The addition is done
 * modulo USB_OTG_HFNUM_FRNUM_MAX. Returns the incremented value.
 *
 * send the token in next frame
 */
static inline u32 usbh_composite_uac_frame_num_inc(u32 frame, u32 inc)
{
	return (frame + inc) & USB_OTG_HFNUM_FRNUM_MAX;
}

static u16 usbh_composite_uac_volume_to_db(usbh_uac_volume_info_t *uac_dev, u8 percent)
{
	int range;
	int raw;

	if (uac_dev == NULL) {
		return uac_dev->vol_min;
	}

	if (percent == 0) {
		return uac_dev->vol_min;       // 0% min / mute
	} else if (percent >= 100) {
		return uac_dev->vol_max;       // 100% max
	}

	// uac = vol_min + (percent / 100.0) x (vol_max - vol_min)
	range = uac_dev->vol_max - uac_dev->vol_min;
	raw = uac_dev->vol_min + (int)(percent * range / 100.0f);

	if (raw < uac_dev->vol_min) {
		raw = uac_dev->vol_min;
	} else if (raw > uac_dev->vol_max) {
		raw = uac_dev->vol_max;
	}

	return (u16)raw;
}

static void usbh_composite_uac_dump_cfgdesc(void)
{
#if USBH_COMPOSITE_HID_UAC_DEBUG
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_as_itf_info_t *as_info = NULL;
	RTK_LOGS(TAG, RTK_LOG_INFO, "--------------------AS Dump Start------------------------------\n");

	if (uac->as_isoc_out) {
		as_info = uac->as_isoc_out;
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB OUT at_cnt %d itf %d\n", as_info->alt_cnt, as_info->as_itf_num);
		usbh_composite_uac_dump_as_desc(as_info);
	}

	if (uac->as_isoc_in) {
		as_info = uac->as_isoc_in;
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB IN at_cnt %d itf %d\n", as_info->alt_cnt, as_info->as_itf_num);
		usbh_composite_uac_dump_as_desc(as_info);
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "---------------------AS Dump End-----------------------------\n");
#endif
}

static usbh_uac_as_itf_info_t *usbh_composite_uac_get_as_itf_instance(u8 dir)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_as_itf_info_t *as_itf = NULL;

	if (dir == USBH_UAC_ISOC_OUT_DIR) {
		as_itf = uac->as_isoc_out;
	} else {
		as_itf = uac->as_isoc_in;
	}

	if (as_itf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No support audio %s\n", ((dir == USBH_UAC_ISOC_OUT_DIR) ? ("OUT") : ("IN")));
		return NULL;
	}

	return as_itf;
}

static void usbh_composite_uac_get_audio_format(usbh_uac_as_itf_info_t *as_info)
{
	usbh_uac_as_itf_alt_info_t *pasintf = NULL;
	usbh_uac_format_cfg_t *fmt = NULL;
	usbh_audio_fmt_t *pfmt_info = NULL;
	u8 fmt_cnt = 0;
	u8 fmt_idx = 0;
	u8 i, k;

	if (as_info == NULL) {
		return;
	}

	for (i = 0; i < as_info->alt_cnt; i ++) {
		pasintf = &(as_info->itf_info_array[i]);
		fmt = &(pasintf->format_info);
		fmt_cnt += fmt->freq_cnt;
	}

	as_info->fmt_array = (usbh_audio_fmt_t *)usb_os_malloc(sizeof(usbh_audio_fmt_t) * fmt_cnt);
	if (as_info->fmt_array == NULL) {
		return;
	}
	as_info->fmt_array_cnt = fmt_cnt;

	fmt_idx = 0;
	for (i = 0; i < as_info->alt_cnt; i ++) {
		fmt = &(as_info->itf_info_array[i].format_info);
		for (k = 0; k < fmt->freq_cnt; k++) {
			pfmt_info = &(as_info->fmt_array[fmt_idx++]);

			pfmt_info->sampling_freq = USBH_UAC_FREQ(fmt->freq[k]);
			pfmt_info->bit_width = fmt->bit_width;
			pfmt_info->ch_cnt = fmt->channels;
		}
	}
}

static u32 usbh_composite_uac_get_audio_format_info(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;

	usbh_composite_uac_get_audio_format(uac->as_isoc_out);
	usbh_composite_uac_get_audio_format(uac->as_isoc_in);

	return HAL_OK;
}

static void usbh_composite_uac_deinit_pipe(u8 dir)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usb_host_t *host = uac->driver->host;
	usbh_uac_as_itf_info_t *as_itf;
	usbh_pipe_t *pipe = NULL;

	as_itf = usbh_composite_uac_get_as_itf_instance(dir);
	if (as_itf == NULL) {
		return ;
	}

	pipe = &(as_itf->pipe);
	if (pipe->pipe_num) {
		usbh_close_pipe(host, pipe);
		pipe->pipe_num = 0U;
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

	if ((uac->driver != NULL) && (uac->driver->host != NULL) && (uac->driver->host->connect_state >= USBH_STATE_SETUP)) {
		return HAL_OK;
	}

	return HAL_BUSY;
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
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_in);

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
		RTK_LOGS(TAG, RTK_LOG_WARN, "Can not find\n");
		return HAL_OK;
	}

#if USBH_COMPOSITE_HID_UAC_DEBUG
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "UAC 1.0 :\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tID: 0x%02x\n", info->sink_id);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tID(unit_id): 0x%02x\n", info->unit_id);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tID(link): 0x%02x\n", info->source_id);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tsize: %dbyte\n", info->control_size);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tmas: %02x\n", info->master_support);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tch: %d\n", info->num_channels);

	for (int i = 0; i < info->num_channels; i++) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tch %d support: %02x\n",
				 i + 1, info->channel_support[i]);
	}
#endif

	return HAL_OK;
}

/**
  * @brief  Parse audio control interface
  * @param  itf_data: interface descriptor buffer
  * @retval Status
  */
static int usbh_composite_uac_parse_ac(usbh_itf_data_t *itf_data)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_in);
	usb_ac_itf_desc_header_t *ac_header;
	u8 *desc;
	u32 t;
	u8 ch;
	u8 type;
	u8 subtype;
	u8 len;
	u16 itf_total_len = 0;

	desc = itf_data->raw_data;
	//save the first interface number as the ac itf_idx
	ac_info->ac_itf_idx = desc[2];

	while (1) {
		if (desc == NULL || itf_total_len >= itf_data->raw_data_len) {
			break;
		}

		type = ((usbh_desc_header_t *) desc)->bDescriptorType;
		if (type == USB_DESC_TYPE_INTERFACE) {
			if (((usbh_itf_desc_t *)desc)->bInterfaceNumber != ac_info->ac_itf_idx) { //find another itf, maybe it is the as itf, should return
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "AC intf new %d:old %d, return\n\n", ((usbh_itf_desc_t *)desc)->bInterfaceNumber, ac_info->ac_itf_idx);
				break;
			}

			len = ((usbh_desc_header_t *) desc)->bLength;
			desc += len;
		} else if (type == USBH_DT_CS_INTERFACE) {
			len = ((usbh_desc_header_t *) desc)->bLength;
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

			desc += len;
		} else {
			len = ((usbh_desc_header_t *) desc)->bLength;
			desc += len;
		}
		itf_total_len += len;
	}

	desc = itf_data->raw_data;
	while (1) {
		if (desc == NULL) {
			break;
		}

		type = ((usbh_desc_header_t *) desc)->bDescriptorType;
		if (type == USB_DESC_TYPE_INTERFACE) {
			if (((usbh_itf_desc_t *)desc)->bInterfaceNumber != ac_info->ac_itf_idx) { //find another itf, maybe it is the as itf, should return
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "AC intf new %d:old %d, return\n\n", ((usbh_itf_desc_t *)desc)->bInterfaceNumber, ac_info->ac_itf_idx);
				break;
			}

			len = ((usbh_desc_header_t *) desc)->bLength;
			desc += len;
		} else if (type == USBH_DT_CS_INTERFACE) {
			len = ((usbh_desc_header_t *) desc)->bLength;
			ac_header = (usb_ac_itf_desc_header_t *)desc;
			subtype = ac_header->bDescriptorSubtype;

			if (subtype == UAC_FEATURE_UNIT) {
				if (len >= 0x07) {
					usbh_uac_vol_ctrl_info vol_info = {0};
					vol_info.unit_id = desc[3];
					vol_info.source_id = desc[4];
					vol_info.control_size = desc[5];
					vol_info.num_channels = (len - 6 - 1) / (vol_info.control_size) - 1; //bmacontrols

					if (vol_info.num_channels > USBH_UAC_MAX_CHANNEL) {
						vol_info.num_channels = USBH_UAC_MAX_CHANNEL;
					}

					if (vol_info.control_size == 1) {
						vol_info.master_support = desc[6];

						for (ch = 0; ch < vol_info.num_channels; ch++) {
							if (7 + ch < len) {
								vol_info.channel_support[ch] = desc[7 + ch];
							}
						}
					} else {
						vol_info.master_support = USBH_LE16(desc + 6);

						for (ch = 0; ch < vol_info.num_channels; ch++) {
							if (8 + 2 * ch < len) {
								vol_info.channel_support[ch] = USBH_LE16(desc + 8 + 2 * ch);
							}
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

			desc += len;
		} else {
			len = ((usbh_desc_header_t *) desc)->bLength;
			desc += len;
		}
	}

	return HAL_OK;
}

/**
  * @brief  Parse audio streaming interface
  * @param  itf_data: interface descriptor buffer
  * @retval Status
  */
static int usbh_composite_uac_parse_as(usbh_itf_data_t *itf_data)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_as_itf_alt_info_t *alt_setting = NULL;
	usbh_uac_format_cfg_t *format_info = NULL;
	usbh_uac_as_itf_info_t *as_itf = NULL;
	usbh_ep_desc_t *ep_cfg = NULL;
	u8 *desc = itf_data->raw_data;
	u16 len;
	u16 itf_total_len = 0;
	u8 alt_set_idx;
	u8 k;

	as_itf = (usbh_uac_as_itf_info_t *)usb_os_malloc(sizeof(usbh_uac_as_itf_info_t));
	if (as_itf == NULL) {
		return HAL_ERR_MEM;
	}

	as_itf->as_itf_num = desc[2];
	desc += ((usbh_desc_header_t *) desc)->bLength;

	while (1) {
		if (desc == NULL || itf_total_len >= itf_data->raw_data_len) {
			break;
		}

		switch (((usbh_desc_header_t *) desc)->bDescriptorType) {
		case USB_DESC_TYPE_INTERFACE:
			if (((usbh_itf_desc_t *)desc)->bInterfaceNumber != as_itf->as_itf_num) {
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "AS intf new %d:old %d, return\n", ((usbh_itf_desc_t *)desc)->bInterfaceNumber, as_itf->as_itf_num);
				return HAL_OK;
			}

			alt_set_idx = ((usbh_itf_desc_t *)desc)->bAlternateSetting;
			if ((alt_set_idx != 0) && (as_itf->alt_cnt < USBH_UAC_ALT_SETTING_MAX)) {
				alt_setting = &(as_itf->itf_info_array[as_itf->alt_cnt]);
				as_itf->alt_cnt ++;

				len = ((usbh_desc_header_t *) desc)->bLength;
				desc += len;
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

			len = ((usbh_desc_header_t *) desc)->bLength;
			desc += len;
		}
		break;

		case USB_DESC_TYPE_ENDPOINT: {
			usbh_ep_desc_t *ep_desc = (usbh_ep_desc_t *)desc;
			if (alt_setting != NULL) {
				ep_cfg = &(alt_setting->ep_desc);
				usb_os_memcpy(ep_cfg, ep_desc, sizeof(usbh_ep_desc_t));

				if (USB_EP_IS_IN(ep_desc->bEndpointAddress)) {
					// is_audio_in = 1;
					if (uac->as_isoc_in == NULL) {
						uac->as_isoc_in = as_itf;
					}
				} else {
					// is_audio_in = 0;
					if (uac->as_isoc_out == NULL) {
						uac->as_isoc_out = as_itf;
					}
				}
			}

			len = ((usbh_desc_header_t *) desc)->bLength;
			desc += len;
		}
		break;

		case USBH_DT_CS_ENDPOINT:
		default: {
			len = ((usbh_desc_header_t *) desc)->bLength;
			desc += len;
		}
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
static int usbh_composite_uac_parse_interface_desc(usb_host_t *host)
{
	usbh_dev_id_t dev_id = {0,};
	dev_id.bInterfaceClass = USB_CLASS_AUDIO;
	dev_id.bInterfaceSubClass = USB_SUBCLASS_AUDIOCONTROL;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS | USBH_DEV_ID_MATCH_ITF_SUBCLASS;
	usbh_itf_data_t *itf_data = usbh_get_interface_descriptor(host, &dev_id);
	int ret = HAL_OK;

	if (itf_data) {
		ret = usbh_composite_uac_parse_ac(itf_data);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "AC parse fail\n");
			return ret;
		} else {
			usbh_composite_uac_find_best_ac();
		}
	}

	dev_id.bInterfaceClass = USB_CLASS_AUDIO;
	dev_id.bInterfaceSubClass = USB_SUBCLASS_AUDIOSTREAMING;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS | USBH_DEV_ID_MATCH_ITF_SUBCLASS;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);
	while (itf_data) {
		if (itf_data->itf_desc_array[0].bAlternateSetting == 0) {
			ret = usbh_composite_uac_parse_as(itf_data);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "AS parse fail\n");
				return ret;
			}
		}
		itf_data = itf_data->next;
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

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_REQ_SET_INTERFACE;
	setup.req.wValue = uac->as_isoc_out->choose_alt_idx + 1;
	setup.req.wIndex = uac->as_isoc_out->as_itf_num;
	setup.req.wLength = 0U;
	usbh_uac_dump_req_struct(&setup.req);

	return usbh_ctrl_request(host, &setup, NULL);
}

static int usbh_composite_uac_process_set_in_alt(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_REQ_SET_INTERFACE;
	setup.req.wValue = uac->as_isoc_in->choose_alt_idx + 1;
	setup.req.wIndex = uac->as_isoc_in->as_itf_num;
	setup.req.wLength = 0U;
	usbh_uac_dump_req_struct(&setup.req);

	return usbh_ctrl_request(host, &setup, NULL);
}

static int usbh_composite_uac_process_set_out_freq(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_as_itf_info_t *as_itf = uac->as_isoc_out;
	usbh_uac_format_cfg_t *fmt_info = &(as_itf->itf_info_array[as_itf->choose_alt_idx].format_info);

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_ENDPOINT;
	setup.req.bRequest = UAC_SET_CUR;
	setup.req.wValue = USBH_UAC_SAMPLING_FREQ_CONTROL;
	setup.req.wIndex = as_itf->pipe.ep_addr;
	setup.req.wLength = 3U;
	usbh_uac_dump_req_struct(&setup.req);

	usb_os_memcpy(uac->audio_ctrl_buf, fmt_info->freq[as_itf->choose_freq_idx], 3);

	return usbh_ctrl_request(host, &setup, uac->audio_ctrl_buf);
}

static int usbh_composite_uac_process_set_in_freq(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_as_itf_info_t *as_itf = uac->as_isoc_in;
	usbh_uac_format_cfg_t *fmt_info = &(as_itf->itf_info_array[as_itf->choose_alt_idx].format_info);

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_ENDPOINT;
	setup.req.bRequest = UAC_SET_CUR;
	setup.req.wValue = USBH_UAC_SAMPLING_FREQ_CONTROL;
	setup.req.wIndex = as_itf->pipe.ep_addr;
	setup.req.wLength = 3U;
	usbh_uac_dump_req_struct(&setup.req);

	usb_os_memcpy(uac->audio_ctrl_buf, fmt_info->freq[as_itf->choose_freq_idx], 3);

	return usbh_ctrl_request(host, &setup, uac->audio_ctrl_buf);
}

static int usbh_composite_uac_process_set_ch_volume(usb_host_t *host, u8 ch_idx)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_in);
	usbh_uac_vol_ctrl_info *info = &(ac_info->controls[ac_info->best_match_idx]);

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = UAC_SET_CUR;
	setup.req.wValue = (ch_idx) | (UAC_FU_VOLUME << 8);
	setup.req.wIndex = (ac_info->ac_itf_idx) | (info->unit_id << 8);
	setup.req.wLength = 2U;
	usbh_uac_dump_req_struct(&setup.req);

	uac->audio_ctrl_buf[0] = (u8)(uac->volume_value);
	uac->audio_ctrl_buf[1] = (u8)((uac->volume_value >> 8) & 0xFF);

	return usbh_ctrl_request(host, &setup, uac->audio_ctrl_buf);
}

static int usbh_composite_uac_process_set_volume(usb_host_t *host)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_in);
	usbh_uac_vol_ctrl_info *info = &(ac_info->controls[ac_info->best_match_idx]);
	int ret = HAL_BUSY;

	//1. check master
	//2. loop check all channel
	if (uac->ch_idx == 0) {
		if (info->master_support & UAC_CONTROL_VOLUME) {
			ret = usbh_composite_uac_process_set_ch_volume(host, uac->ch_idx);
			if (ret == HAL_OK) {
				uac->ch_idx ++;
				ret = HAL_BUSY;
			} else if (ret != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get Cur mute err %d\n", ret);
				uac->ch_idx ++;
			}
		} else {
			uac->ch_idx ++;
		}
	} else if (uac->ch_idx <= info->num_channels + 1) {
		if (info->channel_support[uac->ch_idx - 1] & UAC_CONTROL_VOLUME) {
			ret = usbh_composite_uac_process_set_ch_volume(host, uac->ch_idx);
			if (ret == HAL_OK) {
				uac->ch_idx ++;
				ret = HAL_BUSY;
			} else if (ret != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get Cur mute err %d\n", ret);
				uac->ch_idx ++;
			}
		} else {
			uac->ch_idx ++;
		}
	} else {
		//finish
		ret = HAL_OK; //set mute done
		uac->ch_idx = 0;
	}

	return ret;
}

static int usbh_composite_uac_process_set_ch_mute(usb_host_t *host, u8 ch_idx)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_in);
	usbh_uac_vol_ctrl_info *info = &(ac_info->controls[ac_info->best_match_idx]);

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = UAC_SET_CUR;
	setup.req.wValue = (ch_idx) | (UAC_FU_MUTE << 8);
	setup.req.wIndex = (ac_info->ac_itf_idx) | (info->unit_id << 8);
	setup.req.wLength = 1U;
	usbh_uac_dump_req_struct(&setup.req);

	uac->audio_ctrl_buf[0] = uac->mute_value;

	return usbh_ctrl_request(host, &setup, uac->audio_ctrl_buf);
}

static int usbh_composite_uac_process_set_mute(usb_host_t *host)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_in);
	usbh_uac_vol_ctrl_info *info = &(ac_info->controls[ac_info->best_match_idx]);
	int ret = HAL_BUSY;

	//1. check master
	//2. loop check all channel
	if (uac->ch_idx == 0) {
		if (info->master_support & UAC_CONTROL_MUTE) {
			ret = usbh_composite_uac_process_set_ch_mute(host, uac->ch_idx);
			if (ret == HAL_OK) {
				uac->ch_idx ++;
				ret = HAL_BUSY;
			} else if (ret != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get Cur mute err %d\n", ret);
				uac->ch_idx ++;
			}
		} else {
			uac->ch_idx ++;
		}
	} else if (uac->ch_idx <= info->num_channels + 1) {
		if (info->channel_support[uac->ch_idx - 1] & UAC_CONTROL_MUTE) {
			ret = usbh_composite_uac_process_set_ch_mute(host, uac->ch_idx);
			if (ret == HAL_OK) {
				uac->ch_idx ++;
				ret = HAL_BUSY;
			} else if (ret != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get Cur mute err %d\n", ret);
				uac->ch_idx ++;
			}
		} else {
			uac->ch_idx ++;
		}
	} else {
		//finish
		ret = HAL_OK; //set mute done
		uac->ch_idx = 0;
	}

	return ret;
}

static int usbh_composite_uac_process_get_cur_mute(usb_host_t *host, u8 ch)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_in);
	usbh_uac_vol_ctrl_info *info = &(ac_info->controls[ac_info->best_match_idx]);

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = UAC_GET_CUR;
	setup.req.wValue = (ch) | (UAC_FU_MUTE << 8);
	setup.req.wIndex = (ac_info->ac_itf_idx) | (info->unit_id << 8);
	setup.req.wLength = 1U;
	usbh_uac_dump_req_struct(&setup.req);

	return usbh_ctrl_request(host, &setup, uac->audio_ctrl_buf);
}

static int usbh_composite_uac_process_get_cur_volume(usb_host_t *host, u8 ch)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_in);
	usbh_uac_vol_ctrl_info *info = &(ac_info->controls[ac_info->best_match_idx]);

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = UAC_GET_CUR;
	setup.req.wValue = (ch) | (UAC_FU_VOLUME << 8);
	setup.req.wIndex = (ac_info->ac_itf_idx) | (info->unit_id << 8);
	setup.req.wLength = 2U;
	usbh_uac_dump_req_struct(&setup.req);

	return usbh_ctrl_request(host, &setup, uac->audio_ctrl_buf);
}

static int usbh_composite_uac_process_get_volume_range(usb_host_t *host, u8 min, u8 ch)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_in);
	usbh_uac_vol_ctrl_info *info = &(ac_info->controls[ac_info->best_match_idx]);

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = (min) ? (UAC_GET_MIN) : (UAC_GET_MAX);
	setup.req.wValue = (ch) | (UAC_FU_VOLUME << 8);
	setup.req.wIndex = (ac_info->ac_itf_idx) | (info->unit_id << 8);
	setup.req.wLength = 2U;
	usbh_uac_dump_req_struct(&setup.req);

	return usbh_ctrl_request(host, &setup, uac->audio_ctrl_buf);
}

static int usbh_composite_uac_get_unit_ctrl(usb_host_t *host, u16 bma_control, u8 ch)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_volume_info_t *volume_handle = &(uac->volume_info[ch]);
	int ret = HAL_BUSY;

	//1. get cur mute
	//2. loop all channel get volume : cur, min, max , res
	if (uac->ctrl_state == UAC_STATE_GET_MUTE) {
		if (bma_control & UAC_FU_MUTE) {
			ret = usbh_composite_uac_process_get_cur_mute(host, ch);
			if (ret == HAL_OK) {
				//parse to get the buffer
				uac->ctrl_state = UAC_STATE_GET_CUR_VOLUME;
				volume_handle->mute = uac->audio_ctrl_buf[0];
				ret = HAL_BUSY;
			} else if (ret != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get Cur mute err %d\n", ret);
				usb_os_sleep_ms(5);
				uac->ctrl_state = UAC_STATE_GET_CUR_VOLUME;
			}
		} else {
			uac->ctrl_state = UAC_STATE_GET_CUR_VOLUME;
		}
	} else if (uac->ctrl_state == UAC_STATE_GET_CUR_VOLUME) {
		if (bma_control & UAC_FU_MUTE) {
			ret = usbh_composite_uac_process_get_cur_volume(host, ch);
			if (ret == HAL_OK) {
				uac->ctrl_state = UAC_STATE_GET_VOLUME_MIN;
				//parse to get the volume info
				volume_handle->volume = USBH_LE16(uac->audio_ctrl_buf);
				ret = HAL_BUSY;
			} else if (ret != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "HID get report err %d, no support\n", ret);
			}
		} else {
			uac->ctrl_state = UAC_STATE_GET_VOLUME_MIN;
		}
	} else if (uac->ctrl_state == UAC_STATE_GET_VOLUME_MIN) {
		if (bma_control & UAC_FU_MUTE) {
			ret = usbh_composite_uac_process_get_volume_range(host, 1, ch);
			if (ret == HAL_OK) {
				uac->ctrl_state = UAC_STATE_GET_VOLUME_MAX;
				volume_handle->vol_min = USBH_LE16(uac->audio_ctrl_buf);
				ret = HAL_BUSY;
			} else if (ret != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "HID get report err %d, no support\n", ret);
			}
		} else {
			uac->ctrl_state = UAC_STATE_GET_VOLUME_MAX;
		}
	} else if (uac->ctrl_state == UAC_STATE_GET_VOLUME_MAX) {
		if (bma_control & UAC_FU_MUTE) {
			ret = usbh_composite_uac_process_get_volume_range(host, 0, ch);
			if (ret == HAL_OK) {
				uac->ctrl_state = UAC_STATE_CTRL_IDLE;
				volume_handle->vol_max = USBH_LE16(uac->audio_ctrl_buf);
#if USBH_COMPOSITE_HID_UAC_DEBUG
				RTK_LOGS(TAG, RTK_LOG_INFO, "mute %d\n", volume_handle->mute);
				RTK_LOGS(TAG, RTK_LOG_INFO, "volume 0x%04x\n", (u16)volume_handle->volume);
				RTK_LOGS(TAG, RTK_LOG_INFO, "volume min 0x%04x\n", (u16)volume_handle->vol_min);
				RTK_LOGS(TAG, RTK_LOG_INFO, "volume max 0x%04x\n", (u16)volume_handle->vol_max);
#endif
			} else if (ret != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "HID get report err %d, no support\n", ret);
				ret = HAL_OK;
			}
		} else {
			uac->ctrl_state = UAC_STATE_CTRL_IDLE;
		}
	} else {
		ret = HAL_OK;
	}

	return ret;
}

/**
  * @brief  Iso in test
  * @param  host: Host handle
  * @retval None
*/
static void usbh_composite_uac_isoc_in_process(usb_host_t *host)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_pipe_t *pipe = &(uac->as_isoc_in->pipe);
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	u32 len;

	//rx to do
	switch (pipe->xfer_state) {
	case USBH_EP_XFER_START:
		if (usbh_get_elapsed_frame_cnt(host, pipe->frame_num) >= pipe->ep_interval) {
			pipe->frame_num = usbh_get_current_frame_number(host);
			usbh_transfer_data(host, pipe);

			pipe->xfer_state = USBH_EP_XFER_BUSY;
		}
		usbh_notify_composite_class_state_change(host, 0, USBH_COMPOSITE_UAC_EVENT);
		break;

	case USBH_EP_XFER_BUSY:
		urb_state = usbh_get_urb_state(host, pipe);
		if (urb_state == USBH_URB_DONE) {
			len = usbh_get_last_transfer_size(host, pipe);
			if ((uac->cb != NULL) && (uac->cb->isoc_received != NULL)) {
				uac->cb->isoc_received(pipe->xfer_buf, len);
			}
			pipe->xfer_state = USBH_EP_XFER_IDLE;
		} else if (urb_state == USBH_URB_ERROR) {
			pipe->xfer_state = USBH_EP_XFER_START;
		}
		usbh_notify_composite_class_state_change(host, 0, USBH_COMPOSITE_UAC_EVENT);
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
	usbh_pipe_t *pipe = &(uac->as_isoc_out->pipe);
	usbh_uac_buf_ctrl_t *pdata_ctrl = &(uac->isoc_out);
	usb_ringbuf_t *pbuf = NULL;

	if (!usb_ringbuf_is_empty(&(pdata_ctrl->buf_list))) {
		// check valid data
		pbuf = usb_ringbuf_get_head(&(pdata_ctrl->buf_list));
		if (pbuf && pbuf->buf_len > 0) {
			pipe->frame_num = usbh_composite_uac_frame_num_inc(cur_frame, 1);
#if USBH_COMPOSITE_HID_UAC_DEBUG
			uac->isoc_tx_start_cnt ++;
#endif
			pipe->xfer_buf = pbuf->buf;
			pipe->xfer_len = pbuf->buf_len;
			usbh_transfer_data(host, pipe);
			pipe->xfer_state = USBH_EP_XFER_BUSY;
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
}

static void usbh_composite_uac_isoc_out_process_complete(usb_host_t *host)
{
	UNUSED(host);
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_pipe_t *pipe = &(uac->as_isoc_out->pipe);
	usbh_uac_buf_ctrl_t *pdata_ctrl = &(uac->isoc_out);

#if USBH_COMPOSITE_HID_UAC_DEBUG
	uac->isoc_tx_done_cnt ++;
#endif

	usb_ringbuf_release_head(&(pdata_ctrl->buf_list));

	rtos_sema_give(pdata_ctrl->isoc_sema);

	pipe->xfer_state = USBH_EP_XFER_IDLE;
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
	usbh_pipe_t *pipe = NULL;

	int status = HAL_ERR_UNKNOWN;

	status = usbh_composite_uac_parse_interface_desc(host);
	if (status) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Cfg parse fail\n");
		return status;
	}
	usbh_composite_uac_dump_cfgdesc();

	usbh_composite_uac_get_audio_format_info();

	if (uac->as_isoc_in) {
		as_itf = uac->as_isoc_in;
		as_itf->choose_alt_idx = 0;

		pipe = &(as_itf->pipe);
		ep_desc = &(as_itf->itf_info_array[as_itf->choose_alt_idx].ep_desc);

		usbh_open_pipe(host, pipe, ep_desc);
	}

	if (uac->as_isoc_out) {
		as_itf = uac->as_isoc_out;
		as_itf->choose_alt_idx = 0;

		pipe = &(as_itf->pipe);
		ep_desc = &(as_itf->itf_info_array[as_itf->choose_alt_idx].ep_desc);

		usbh_open_pipe(host, pipe, ep_desc);
	}

	if ((uac->cb != NULL) && (uac->cb->attach != NULL)) {
		uac->cb->attach();
	}

	uac->ctrl_state = UAC_STATE_GET_MUTE; //first get the mute info

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
	usbh_pipe_t *pipe = &(uac->as_isoc_out->pipe);
	usbh_uac_buf_ctrl_t *pdata_ctrl = &(uac->isoc_out);
	u32 cur_frame = usbh_get_current_frame_number(host);

	/* this class right not just support isoc out */
	if (pdata_ctrl->next_xfer == 1) {
#if USBH_COMPOSITE_HID_UAC_DEBUG
		uac->sof_cnt ++;
#endif
		/*
			if cur_frame - last frame_num  >= interval, means we should trigger a xfer asap
			if xfer_state = idle, it means that last xfer has been done, so in sof intr, we should check whether the next frame will be the xfer frame
		*/
		if ((usbh_get_elapsed_frame_cnt(host, pipe->frame_num) >= pipe->ep_interval) ||
			((pipe->xfer_state == USBH_EP_XFER_WAIT_SOF) &&
			 (usbh_composite_uac_frame_num_dec(usbh_composite_uac_frame_num_inc(cur_frame, 1), pipe->frame_num) >= pipe->ep_interval))) {
			usbh_composite_uac_isoc_out_process_xfer(host, cur_frame);
		} else { // interval
#if USBH_COMPOSITE_HID_UAC_DEBUG
			if (pipe->xfer_state == USBH_EP_XFER_IDLE) {
				uac->isoc_xfer_interval_cnt ++;
			}
#endif
		}
	}

	if (uac->ctrl_state != UAC_STATE_CTRL_IDLE) {
		usbh_notify_composite_class_state_change(host, 0x00, USBH_COMPOSITE_UAC_EVENT);
	}

	return HAL_OK;
}

/**
  * @brief  Xfer complete callback
  * @param  host: Host handle
  * @param  pipe: pipe index
  * @retval Status
  */
static int usbh_composite_uac_cb_completed(usb_host_t *host, u8 pipe_num)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_pipe_t *pipe = &(uac->as_isoc_out->pipe);
	usbh_uac_buf_ctrl_t *pdata_ctrl = &(uac->isoc_out);
	u32 cur_frame = usbh_get_current_frame_number(host);

	if (pdata_ctrl->next_xfer == 1) {
		if ((uac->as_isoc_out) && (pipe_num == uac->as_isoc_out->pipe.pipe_num)) {
			usbh_composite_uac_isoc_out_process_complete(host);

			if (!usb_ringbuf_is_empty(&(pdata_ctrl->buf_list))) {
				//trigger next xfer after binterval = 1
				if (usbh_composite_uac_frame_num_dec(usbh_composite_uac_frame_num_inc(cur_frame, 1), pipe->frame_num) >= pipe->ep_interval) {
					usbh_composite_uac_isoc_out_process_xfer(host, cur_frame);//USBH_EP_XFER_START
				} else {
					pipe->xfer_state = USBH_EP_XFER_WAIT_SOF;
				}
			} else {
				pipe->xfer_state = USBH_EP_XFER_IDLE;
			}
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
	usbh_event_t event;
	int ret = HAL_OK;
	event.d32 = msg;

	switch (uac->xfer_state) {
	case UAC_STATE_TRANSFER:
		if (event.msg.pipe_num == 0x00) { //ctrl msg
			ret = usbh_composite_uac_ctrl_setting(host, 0);
		} else if ((buf_ctrl->next_xfer) && (uac->as_isoc_in) && (event.msg.pipe_num == uac->as_isoc_in->pipe.pipe_num)) {
			uac->next_xfer = 0;
			usbh_composite_uac_isoc_in_process(host);
			if (uac->next_xfer) {
				usbh_notify_composite_class_state_change(host, uac->as_isoc_in->pipe.pipe_num, USBH_COMPOSITE_UAC_EVENT);
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
	usbh_uac_as_itf_info_t *as_itf = uac->as_isoc_out;
	u32 sample_accum = 0;
	u32 ret;

	sample_accum = pdata_ctrl->sample_accum + pdata_ctrl->sample_rem;
	if (sample_accum >= pdata_ctrl->pkt_per_second) {
		sample_accum -= pdata_ctrl->pkt_per_second;
		ret = as_itf->packet_size_large;
	} else {
		ret = as_itf->packet_size_small;
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
	u32 xfer_len;
	u32 can_copy_len;
	u32 copy_len;

	if (written_size) {
		xfer_len = usbh_composite_uac_next_packet_size(pdata_ctrl);
		can_copy_len = xfer_len - written_size;
		copy_len = size < can_copy_len ? size : can_copy_len;

		usb_ringbuf_write_partial(handle, buffer, copy_len);
		offset += copy_len;
		*written_len += copy_len;

		if (size >= can_copy_len) {
			size -= copy_len;
			usb_ringbuf_finish_write(handle);
			pdata_ctrl->sample_accum = pdata_ctrl->last_sample_accum;
		} else {
			return 0;
		}
	}

	do {
		if (usb_ringbuf_is_full(handle)) {
			return 1;
		}

		xfer_len = usbh_composite_uac_next_packet_size(pdata_ctrl);

		if (size >= xfer_len) {
			usb_ringbuf_add_tail(handle, buffer + offset, xfer_len);

			*written_len += xfer_len;
			size -= xfer_len;
			offset += xfer_len;

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
static void usbh_composite_uac_ep_buf_ctrl_deinit(usbh_uac_buf_ctrl_t *buf_ctrl, usbh_pipe_t *pipe)
{
	UNUSED(pipe);

	buf_ctrl->mps = 0;
	buf_ctrl->next_xfer = 0;

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
  * @param  pipe: Pointer to pipe parameters structure
  * @retval Status
  */
static int usbh_composite_uac_ep_buf_ctrl_init(usbh_uac_buf_ctrl_t *buf_ctrl, usbh_pipe_t *pipe)
{
	int ret = HAL_ERR_MEM;
	u16 buf_list_cnt;

	buf_list_cnt = buf_ctrl->frame_cnt;
	buf_ctrl->mps = pipe->ep_mps;

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
	usbh_pipe_t *pipe = &(uac->as_isoc_out->pipe);

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
	usbh_composite_uac_ep_buf_ctrl_deinit(&(uac->isoc_out), pipe);

	if (uac->audio_ctrl_buf != NULL) {
		usb_os_mfree(uac->audio_ctrl_buf);
		uac->audio_ctrl_buf = NULL;
	}

	if (uac->as_isoc_out != NULL) {
		if (uac->as_isoc_out->fmt_array) {
			usb_os_mfree(uac->as_isoc_out->fmt_array);
		}
		usb_os_mfree(uac->as_isoc_out);
		uac->as_isoc_out = NULL;
	}

	if (uac->as_isoc_in != NULL) {
		if (uac->as_isoc_in->fmt_array) {
			usb_os_mfree(uac->as_isoc_in->fmt_array);
		}
		usb_os_mfree(uac->as_isoc_in);
		uac->as_isoc_in = NULL;
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
	usbh_pipe_t *pipe = NULL;
	usbh_ep_desc_t *ep_desc = NULL;
	usb_host_t *host = uac->driver->host;
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
		fmt = &(as_itf->itf_info_array[i].format_info);

		// Check format type, channels, and sample frequency
		if (!fmt || fmt->bit_width != bit_width || fmt->channels != channels) {
			continue;
		}

		// Check sample frequency
		for (j = 0; j < fmt->freq_cnt; j++) {
			if (USBH_UAC_FREQ(fmt->freq[j]) == sampling_freq) {
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "Found itf %d %d/%d %d %d\n", itf_num, i + 1, channels, bit_width, sampling_freq);
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

		//update pipe
		pipe = &(as_itf->pipe);
		ep_desc = &(as_itf->itf_info_array[as_itf->choose_alt_idx].ep_desc);

		/* full speed*/
		pdata_ctrl->pkt_per_second = USBH_UAC_ONE_KHZ >> (ep_desc->bInterval - 1);
		pdata_ctrl->sample_rem = sampling_freq % pdata_ctrl->pkt_per_second;
		//calculate accurate one frame size(byte)
		as_itf->packet_size_small = channels * bit_width / USBH_UAC_BIT_TO_BYTE * (sampling_freq / pdata_ctrl->pkt_per_second);
		as_itf->packet_size_large = channels * bit_width / USBH_UAC_BIT_TO_BYTE * ((sampling_freq + (pdata_ctrl->pkt_per_second - 1)) / pdata_ctrl->pkt_per_second);

		usbh_composite_uac_stop_play();

		//reinit pipe
		usbh_composite_uac_deinit_pipe(dir);
		usbh_open_pipe(host, pipe, ep_desc);

		// reinit ring buf
		if (dir == USBH_UAC_ISOC_OUT_DIR) {
			usbh_composite_uac_ep_buf_ctrl_deinit(&(uac->isoc_out), pipe);
			ret = usbh_composite_uac_ep_buf_ctrl_init(&(uac->isoc_out), pipe);
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

		usbh_notify_composite_class_state_change(host, 0, USBH_COMPOSITE_UAC_EVENT);
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
	as_itf = usbh_composite_uac_get_as_itf_instance(dir);
	if (as_itf == NULL) {
		return 0;
	}

	return as_itf->packet_size_large;
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

	if (pdata_ctrl->next_xfer == 0) {
		return 0;
	}

	if (usbh_composite_uac_usb_status_check() != HAL_OK) {
		return 0;
	}

	while (written_len < size && pdata_ctrl->next_xfer) {
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
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Want %msg, wrote %msg\n", try_len, just_written);

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
	usbh_pipe_t *pipe = &(uac->as_isoc_out->pipe);

#if USBH_COMPOSITE_HID_UAC_DEBUG
	usbh_composite_uac_reset_test_cnt();
	usbh_composite_uac_reset_isr_time();
#endif

	buf_ctrl->next_xfer = 1;
	uac->xfer_state = UAC_STATE_TRANSFER;
	pipe->xfer_state = USBH_EP_XFER_START;
	usbh_notify_composite_class_state_change(uac->driver->host, pipe->pipe_num, USBH_COMPOSITE_UAC_EVENT);

	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC start %d\n", uac->ctrl_state);
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

	buf_ctrl->next_xfer = 0;

#if USBH_COMPOSITE_HID_UAC_DEBUG
	usbh_composite_uac_reset_test_cnt();
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC stop\n");
}

int usbh_composite_uac_set_volume(u8 volume)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	int ret = HAL_BUSY;
	u16 volume_db;

	if (usbh_composite_uac_usb_status_check() != HAL_OK) {
		return ret;
	}

	volume_db = (u16)usbh_composite_uac_volume_to_db(&(uac->volume_info[0]), volume);

	if ((uac->xfer_state == UAC_STATE_IDLE) || (uac->xfer_state == UAC_STATE_TRANSFER)) {
		if (uac->ctrl_state == UAC_STATE_CTRL_IDLE) {
			uac->volume_value = volume_db;
			uac->ch_idx = 0;
			uac->ctrl_state = UAC_STATE_SET_VOLUME;
			uac->xfer_state = UAC_STATE_TRANSFER;
			usbh_notify_composite_class_state_change(uac->driver->host, 0x00, USBH_COMPOSITE_UAC_EVENT);
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
			uac->mute_value = mute;
			uac->ctrl_state = UAC_STATE_SET_MUTE;
			uac->xfer_state = UAC_STATE_TRANSFER;
			usbh_notify_composite_class_state_change(uac->driver->host, 0x00, USBH_COMPOSITE_UAC_EVENT);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Get volume information
  * @param  host: Host handle
  * @retval Status
  */
int usbh_composite_uac_get_volume_infor(usb_host_t *host)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_in);
	usbh_uac_vol_ctrl_info *info = &(ac_info->controls[ac_info->best_match_idx]);
	int ret = HAL_OK;

	//1. get master
	//2. loop all channel
	if (uac->ch_idx == 0) {
		ret = usbh_composite_uac_get_unit_ctrl(host, info->master_support, uac->ch_idx);
	} else if (uac->ch_idx <= info->num_channels + 1) {
		ret = usbh_composite_uac_get_unit_ctrl(host, info->channel_support[uac->ch_idx - 1], uac->ch_idx);
	} else {
		//finish
	}

	return ret;
}