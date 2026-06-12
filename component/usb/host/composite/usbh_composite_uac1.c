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
#define USBH_UAC_WAIT_SLICE_MS                      5
#define USB_OTG_HFNUM_FRNUM_MAX                     (0x3FFFUL)       /* Frame number max value */

#define USBH_UAC_AUDIO_CTRL_BUF_MAX_LEN             512U
#define USBH_UAC_ISOC_BUF_LENGTH                    1024U

#define USBH_UAC_SAMPLING_FREQ_CONTROL              0x100

#define USBH_UAC_BIT_TO_BYTE                        8U
#define USBH_UAC_ONE_KHZ                            1000U

#define USBH_LE16(addr)                             (((u16)(addr)[0]) | ((u16)(((u32)(addr)[1]) << 8)))
#define USBH_UAC_FREQ(freq)                         (((u32)freq[0]) | (((u32)freq[1]) << 8) | (((u32)freq[2]) << 16))

#if USBH_COMPOSITE_HID_UAC_DEBUG
#define USBH_COMPOSITE_HID_UAC_DEBUG_LOOP_TIME      1000
#endif

/* States for class */
typedef enum {
	UAC_STATE_IDLE = 0U,
	UAC_STATE_TRANSFER,
	UAC_STATE_ERROR
} usbh_uac_xfer_state_t;

typedef enum {
	UAC_STATE_CTRL_IDLE = 0U,

	UAC_STATE_SET_ALT_SETTING,
	UAC_STATE_SET_FREQ,
	UAC_STATE_SET_VOLUME,
	UAC_STATE_SET_MUTE,

	/* scan: init-time full-channel scan driven by get_unit_ctrl() */
	UAC_STATE_SCAN_MUTE,
	UAC_STATE_SCAN_CUR_VOLUME,
	UAC_STATE_SCAN_MIN_VOLUME,
	UAC_STATE_SCAN_MAX_VOLUME,

	/* get: on-demand single-shot query driven by ctrl_setting(), kept distinct from
	 * the init scan above so a SOF-triggered ctrl_setting() cannot hijack a
	 * SCAN_MUTE/SCAN_VOLUME_CUR state set by the init scan. */
	UAC_STATE_GET_MUTE,
	UAC_STATE_GET_VOLUME,
} usbh_uac_ctrl_state_t;

typedef enum {
	UAC_INIT_IDLE = 0U,

	UAC_INIT_OUT_GET_FU,   /* read Playback Feature Unit attributes (volume range, mute) for all channels */
	UAC_INIT_IN_GET_FU,    /* read Record Feature Unit attributes for all channels */
	UAC_INIT_OUT_SET_ITF,  /* reset Playback AS interface to Alt 0 (zero-bandwidth) */
	UAC_INIT_IN_SET_ITF,   /* reset Record AS interface to Alt 0 */

	UAC_INIT_DONE,
} usbh_uac_init_state_t;

/* Private function prototypes -----------------------------------------------*/
static int usbh_composite_uac_attach(usb_host_t *host);
static int usbh_composite_uac_detach(usb_host_t *host);
static int usbh_composite_uac_process(usb_host_t *host, usbh_event_t *event);
static int usbh_composite_uac_ctrl_setting(usb_host_t *host, u32 msg);
static int usbh_composite_uac_setup(usb_host_t *host);
static int usbh_composite_uac_sof(usb_host_t *host);
static int usbh_composite_uac_completed(usb_host_t *host, u8 pipe);
static void usbh_composite_uac_isoc_out_process_xfer(usb_host_t *host, u32 cur_frame);
static void usbh_composite_uac_isoc_in_process_xfer(usb_host_t *host, u32 cur_frame);
static int usbh_composite_uac_process_set_alt(usb_host_t *host);
static int usbh_composite_uac_process_set_freq(usb_host_t *host);
static void usbh_composite_uac_deinit_pipe(u8 dir);
static void usbh_composite_uac_deinit_all_pipe(void);
static void usbh_composite_uac_channel_deinit(usbh_composite_uac_channel_t *ch);
static int usbh_composite_uac_parse_interface_desc(usb_host_t *host);
static int usbh_composite_uac_parse_ac(usbh_itf_data_t *ac_itf);
static int usbh_composite_uac_parse_as(usbh_itf_data_t *as_itf);
static int usbh_composite_uac_usb_status_check(void);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "UAC";

/* USB Class Driver */
const usbh_class_driver_t usbh_composite_uac_driver = {
	.attach = usbh_composite_uac_attach,
	.detach = usbh_composite_uac_detach,
	.setup = usbh_composite_uac_setup,
	.process = usbh_composite_uac_process,
	.sof = usbh_composite_uac_sof,
	.completed = usbh_composite_uac_completed,
};

static usbh_composite_uac_t usbh_composite_uac;

/**
  * @brief  Check whether the USB host connection is ready.
  * @retval HAL_OK if connected and set up, HAL_BUSY otherwise.
  */
static int usbh_composite_uac_usb_status_check(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;

	if ((uac->driver != NULL) && (uac->driver->host != NULL) && (uac->driver->host->connect_state >= USBH_STATE_SETUP)) {
		return HAL_OK;
	}

	return HAL_BUSY;
}

#if USBH_COMPOSITE_HID_UAC_DEBUG
/**
  * @brief  Print all alternate settings of an audio streaming interface for debugging.
  * @param  as_info: Pointer to the AS interface info structure.
  * @retval void
  */
static void usbh_composite_uac_dump_as_desc(usbh_composite_uac_as_itf_info_t *as_info)
{
	usbh_composite_uac_as_itf_alt_info_t *as_alt_info = NULL;
	usbh_composite_uac_format_cfg_t *fmt = NULL;
	usbh_ep_desc_t *audio_ep = NULL;
	int i, k;
	if (as_info == NULL) {
		return;
	}

	for (i = 0; i < as_info->alt_setting_cnt; i ++) {
		as_alt_info = &(as_info->interface_array[i]);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Intf=%d alts %d\n", as_info->as_itf_num, as_alt_info->alt_setting);
		fmt = &(as_alt_info->format_info);
		audio_ep = &(as_alt_info->ep_desc);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Ep%02x MPS %d %d\n", audio_ep->bEndpointAddress, audio_ep->wMaxPacketSize, audio_ep->bInterval);

		RTK_LOGS(TAG, RTK_LOG_INFO, "Ch %d byte %d freqcnt %d\n", fmt->channels, fmt->bit_width, fmt->freq_cnt);
		for (k = 0; k < fmt->freq_cnt; k++) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "\t\tFre:%dhz\n", fmt->freq[k]);
		}
	}
}

/**
  * @brief  Log current UAC TX/RX buffer states, transfer counters, and ISR timing metrics.
  * @retval void
  */
extern void usbh_composite_hid_status_dump(void);
static void usbh_composite_uac_status_dump(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_channel_t *uac_channel = NULL;
	usbh_composite_uac_buf_ctrl_t *buf_ctrl = NULL;

	if (usbh_composite_uac_usb_status_check() == HAL_OK) {
		if (uac->isoc_out.as_itf != NULL) {
			uac_channel = &(uac->isoc_out);
			buf_ctrl = &(uac_channel->buf_ctrl);
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "UAC TX:%d-%d-%d-%d/xfer=%d-%d-%d %d-%d-%d\n",
					 buf_ctrl->buf_manager.capacity, usb_ringbuf_get_count(&(buf_ctrl->buf_manager)),
					 buf_ctrl->next_xfer, uac_channel->as_itf->pipe.xfer_state,
					 (u32)(uac->sof_cnt), (u32)(buf_ctrl->xfer_start_cnt), (u32)(buf_ctrl->xfer_done_cnt),
					 (u32)(buf_ctrl->xfer_buf_empty_cnt), (u32)(buf_ctrl->xfer_buf_err_cnt), (u32)(buf_ctrl->xfer_interval_cnt));
		}
		if (uac->isoc_in.as_itf != NULL) {
			uac_channel = &(uac->isoc_in);
			buf_ctrl = &(uac_channel->buf_ctrl);
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "RX %d-%d-%d-%d/xfer=%d-%d-%d-%d %d-%d-%d\n",
					 buf_ctrl->buf_manager.capacity, usb_ringbuf_get_count(&(buf_ctrl->buf_manager)),
					 buf_ctrl->next_xfer, uac_channel->as_itf->pipe.xfer_state,
					 (u32)(uac->sof_cnt), (u32)(buf_ctrl->xfer_start_cnt), (u32)(buf_ctrl->xfer_done_cnt), (u32)(buf_ctrl->last_xfer_len),
					 (u32)(buf_ctrl->xfer_buf_empty_cnt), (u32)(buf_ctrl->xfer_buf_err_cnt), (u32)(buf_ctrl->xfer_interval_cnt));
		}

#if USBH_TP_TRACE_DEBUG
		if (uac->driver && uac->driver->host) {
			usb_host_t *host = uac->driver->host;

			RTK_LOGS(NOTAG, RTK_LOG_INFO, "State %d-%d/IsrC %lld-%lld/isrT %lld-%lld\n\n",
					 uac->ctrl_state, uac->xfer_state,
					 host->isr_process_time_max, host->isr_process_time,
					 host->isr_enter_period_max, host->isr_enter_period);
		}
#endif

		usbh_composite_hid_status_dump();
	}
}

/**
  * @brief  Periodic debug thread that calls status_dump every USBH_COMPOSITE_HID_UAC_DEBUG_LOOP_TIME ms.
  * @param  param: Unused.
  * @retval void
  */
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

/**
  * @brief  Reset host ISR processing time and entry period maximum values to zero.
  * @retval void
  */
static void usbh_composite_uac_reset_isr_time(void)
{
#if USBH_TP_TRACE_DEBUG
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usb_host_t *host;

	if (uac && uac->driver && uac->driver->host) {
		host = uac->driver->host;

		host->isr_process_time_max = 0;
		host->isr_enter_period_max = 0;
		host->isr_enter_time = 0;
	}
#endif
}

/**
  * @brief  Zero all SOF and transfer debug counters for both IN and OUT buffer control structures.
  * @retval void
  */
static void usbh_composite_uac_reset_test_cnt(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_buf_ctrl_t *buf_ctrl_out = &(uac->isoc_out.buf_ctrl);
	usbh_composite_uac_buf_ctrl_t *buf_ctrl_in = &(uac->isoc_in.buf_ctrl);

	uac->sof_cnt = 0;
	buf_ctrl_out->xfer_start_cnt = 0;
	buf_ctrl_out->xfer_done_cnt = 0;
	buf_ctrl_out->xfer_buf_empty_cnt = 0;
	buf_ctrl_out->xfer_buf_err_cnt = 0;
	buf_ctrl_out->xfer_interval_cnt = 0;
	buf_ctrl_in->xfer_start_cnt = 0;
	buf_ctrl_in->xfer_done_cnt = 0;
	buf_ctrl_in->xfer_buf_empty_cnt = 0;
	buf_ctrl_in->xfer_buf_err_cnt = 0;
	buf_ctrl_in->xfer_interval_cnt = 0;
}
#endif

/**
  * @brief  Compute the elapsed frame count from start to new, handling wrap-around at FRNUM_MAX.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `usb_os_sema_take`) are not permitted.
  * @param  new:   Current USB frame number.
  * @param  start: Reference frame number.
  * @retval Number of frames elapsed.
  */
static inline u32 usbh_composite_uac_frame_num_dec(u32 new, u32 start)
{
	if (new >= start) {
		return new - start;
	} else {
		return (USB_OTG_HFNUM_FRNUM_MAX - start + 1 + new);
	}
}

/**
  * @brief  Increment a USB frame number by inc, wrapping at USB_OTG_HFNUM_FRNUM_MAX.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `usb_os_sema_take`) are not permitted.
  * @param  frame: Base frame number.
  * @param  inc:   Amount to add.
  * @retval Incremented frame number.
  */
static inline u32 usbh_composite_uac_frame_num_inc(u32 frame, u32 inc)
{
	return (frame + inc) & USB_OTG_HFNUM_FRNUM_MAX;
}

/**
  * @brief  Convert a 0-100 percent volume to raw device dB units by linear interpolation.
  * @param  uac_dev: Pointer to the volume info structure carrying vol_min/vol_max.
  * @param  percent: Volume percentage (0-100).
  * @retval Raw dB value clamped to [vol_min, vol_max].
  */
static u16 usbh_composite_uac_volume_to_db(usbh_composite_uac_volume_info_t *uac_dev, u8 percent)
{
	/* Use s32 for range/raw: vol_min/vol_max are s16 dB values, so a device
	   reporting a wide range (e.g. vol_min=0x8001) makes vol_max-vol_min exceed
	   the s16 limit and overflow. Matches uach_comp_compute_expected_db(). */
	s32 range;
	s32 raw;

	if (uac_dev == NULL) {
		return 0;
	}

	if (percent == 0) {
		return uac_dev->vol_min;       // 0% min / mute
	} else if (percent >= 100) {
		return uac_dev->vol_max;       // 100% max
	}

	// uac = vol_min + (percent / 100) x (vol_max - vol_min)
	// Integer arithmetic only: avoids pulling in soft-float on FPU-less targets.
	// The s64 intermediate prevents percent*range overflowing s32 for wide ranges.
	range = uac_dev->vol_max - uac_dev->vol_min;
	raw = uac_dev->vol_min + (s32)((s64)percent * range / 100);

	if (raw < uac_dev->vol_min) {
		raw = uac_dev->vol_min;
	} else if (raw > uac_dev->vol_max) {
		raw = uac_dev->vol_max;
	}

	return (u16)raw;
}

/**
  * @brief  Dump parsed audio streaming descriptors for both IN and OUT interfaces (debug only).
  * @retval void
  */
static void usbh_composite_uac_dump_cfgdesc(void)
{
#if USBH_COMPOSITE_HID_UAC_DEBUG
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_as_itf_info_t *as_info = NULL;
	RTK_LOGS(TAG, RTK_LOG_INFO, "--------------------AS Dump Start------------------------------\n");

	if (uac->isoc_out.as_itf) {
		as_info = uac->isoc_out.as_itf;
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB OUT at_cnt %d itf %d\n", as_info->alt_setting_cnt, as_info->as_itf_num);
		usbh_composite_uac_dump_as_desc(as_info);
	}

	if (uac->isoc_in.as_itf) {
		as_info = uac->isoc_in.as_itf;
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB IN at_cnt %d itf %d\n", as_info->alt_setting_cnt, as_info->as_itf_num);
		usbh_composite_uac_dump_as_desc(as_info);
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "---------------------AS Dump End-----------------------------\n");
#endif
}

/**
  * @brief  Return the AS interface info structure for the given direction.
  * @param  dir: USBH_UAC_ISOC_OUT_DIR for Playback, USBH_UAC_ISOC_IN_DIR for Record.
  * @retval Pointer to the AS interface info, or NULL if that direction is not present.
  */
static usbh_composite_uac_as_itf_info_t *usbh_composite_uac_get_as_itf_instance(u8 dir)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_as_itf_info_t *as_itf = NULL;

	if (dir == USBH_UAC_ISOC_OUT_DIR) {
		as_itf = uac->isoc_out.as_itf;
	} else {
		as_itf = uac->isoc_in.as_itf;
	}

	if (as_itf == NULL) {
		return NULL;
	}

	return as_itf;
}

/**
  * @brief  Allocate and populate the flattened fmt_array from all alternate settings of an AS interface.
  * @param  as_info: Pointer to the AS interface info structure.
  * @retval void
  */
static void usbh_composite_uac_get_audio_format(usbh_composite_uac_as_itf_info_t *as_info)
{
	usbh_composite_uac_as_itf_alt_info_t *pasintf = NULL;
	usbh_composite_uac_format_cfg_t *fmt = NULL;
	usbh_composite_uac_audio_fmt_t *pfmt_info = NULL;
	u8 fmt_cnt = 0;
	u8 fmt_idx = 0;
	u8 i, k;

	if (as_info == NULL) {
		return;
	}

	for (i = 0; i < as_info->alt_setting_cnt; i ++) {
		pasintf = &(as_info->interface_array[i]);
		fmt = &(pasintf->format_info);
		fmt_cnt += fmt->freq_cnt;
	}

	as_info->fmt_array = (usbh_composite_uac_audio_fmt_t *)usb_os_malloc(sizeof(usbh_composite_uac_audio_fmt_t) * fmt_cnt);
	if (as_info->fmt_array == NULL) {
		return;
	}
	as_info->fmt_array_cnt = fmt_cnt;

	fmt_idx = 0;
	for (i = 0; i < as_info->alt_setting_cnt; i ++) {
		fmt = &(as_info->interface_array[i].format_info);
		for (k = 0; k < fmt->freq_cnt; k++) {
			pfmt_info = &(as_info->fmt_array[fmt_idx++]);

			pfmt_info->sampling_freq = fmt->freq[k];
			pfmt_info->bit_width = fmt->bit_width;
			pfmt_info->ch_cnt = fmt->channels;
		}
	}
}

/**
  * @brief  Build the audio format arrays for both ISOC OUT and ISOC IN streaming interfaces.
  * @retval HAL_OK.
  */
static u32 usbh_composite_uac_get_audio_format_info(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;

	usbh_composite_uac_get_audio_format(uac->isoc_out.as_itf);
	usbh_composite_uac_get_audio_format(uac->isoc_in.as_itf);

	return HAL_OK;
}

/**
  * @brief  Close and clear the USB isochronous pipe for the specified direction.
  * @param  dir: USBH_UAC_ISOC_OUT_DIR or USBH_UAC_ISOC_IN_DIR.
  * @retval void
  */
static void usbh_composite_uac_deinit_pipe(u8 dir)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usb_host_t *host = uac->driver->host;
	usbh_composite_uac_as_itf_info_t *as_itf;
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

/**
  * @brief  Close and clear both OUT and IN isochronous pipes.
  * @retval void
  */
static void  usbh_composite_uac_deinit_all_pipe(void)
{
	usbh_composite_uac_deinit_pipe(USBH_UAC_ISOC_OUT_DIR);
	usbh_composite_uac_deinit_pipe(USBH_UAC_ISOC_IN_DIR);
}

/**
  * @brief  Append a terminal descriptor entry to the AC interface list if space remains.
  * @param  list: Pointer to the AC interface info structure.
  * @param  term: Pointer to the terminal entry to copy.
  * @retval void
  */
static void usbh_composite_uac_add_terminal(usbh_composite_uac_ac_itf_info_t *list, const usbh_composite_uac_term_info_t *term)
{
	if (list && term && list->terminal_count < USBH_UAC_TERM_MAX_CNT) {
		usb_os_memcpy(&(list->terminals[list->terminal_count]), term, sizeof(usbh_composite_uac_term_info_t));
		list->terminal_count++;
	}
}

/**
  * @brief  Append a Feature Unit volume control entry to the AC interface list if space remains.
  * @param  list: Pointer to the AC interface info structure.
  * @param  info: Pointer to the Feature Unit entry to copy.
  * @retval void
  */
static void usbh_composite_uac_add_vol_ctrl(usbh_composite_uac_ac_itf_info_t *list, const usbh_composite_uac_fu_info_t *info)
{
	if (list && info && list->volume_ctrl_count < USBH_UAC_FU_MAX_CNT) {
		usb_os_memcpy(&(list->fu_controls[list->volume_ctrl_count]), info, sizeof(usbh_composite_uac_fu_info_t));
		list->volume_ctrl_count++;
	}
}

/**
  * @brief  Scan all parsed Feature Units and select the highest-priority one for each direction.
  *         Priority is determined by terminal type (Headphones > Speaker > other for OUT;
  *         Microphone > Desktop Microphone > other for IN) and master channel support.
  *         Results are stored in ac_info->out_best_idx and ac_info->in_best_idx.
  * @retval HAL_OK.
  */
static int usbh_composite_uac_find_best_ac(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);

#if USBH_COMPOSITE_HID_UAC_DEBUG
	usbh_composite_uac_fu_info_t *info = NULL;
#endif
	usbh_composite_uac_fu_info_t *current;
	int out_best_prio = -1, in_best_prio = -1;
	int priority;
	u32 i;

	ac_info->out_best_idx = -1;
	ac_info->in_best_idx = -1;

	for (i = 0; i < ac_info->volume_ctrl_count; i++) {
		current = &(ac_info->fu_controls[i]);
		priority = 0;

		/* Output terminal types are 0x03xx; values above UNDEFINED(0x300) are
		   defined output terminals (speaker/headphones/...), everything else
		   is treated as an input (microphone) terminal. */
		if (current->sink_type > USB_UAC1_OUTPUT_TERMINAL_UNDEFINED) {
			if (current->sink_type == USB_UAC1_OUTPUT_TERMINAL_HEADPHONES) {
				priority = 3;
			} else if (current->sink_type == USB_UAC1_OUTPUT_TERMINAL_SPEAKER) {
				priority = 2;
			} else {
				priority = 1;
			}

			if (current->bma_controls[0]) {
				priority += 10;
			}

			if (priority > out_best_prio) {
				out_best_prio = priority;
				ac_info->out_best_idx = i;
			}
		} else {
			if (current->sink_type == USB_UAC1_INPUT_TERMINAL_MICROPHONE) {
				priority = 3;
			} else if (current->sink_type == USB_UAC1_INPUT_TERMINAL_DESKTOP_MICROPHONE) {
				priority = 2;
			} else {
				priority = 1;
			}

			if (current->bma_controls[0]) {
				priority += 10;
			}

			if (priority > in_best_prio) {
				in_best_prio = priority;
				ac_info->in_best_idx = i;
			}
		}
	}

#if USBH_COMPOSITE_HID_UAC_DEBUG
	info = NULL;
	if (ac_info->out_best_idx != (u8) - 1) {
		info = &(ac_info->fu_controls[ac_info->out_best_idx]);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "UAC 1.0 OUT %d:\n", ac_info->out_best_idx);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tID: 0x%02x\n", info->sink_id);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tID(unit_id): 0x%02x\n", info->unit_id);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tID(link): 0x%02x\n", info->source_id);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tsize: %dbyte\n", info->control_size);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tnum_channels: %d\n", info->num_channels);

		for (int i = 0; i <= info->num_channels; i++) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tch %d%s support: %02x\n",
					 i, (i == 0) ? " (master)" : "", info->bma_controls[i]);
		}
	}
	if (ac_info->in_best_idx != (u8) - 1) {
		info = &(ac_info->fu_controls[ac_info->in_best_idx]);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "UAC 1.0 IN %d:\n", ac_info->in_best_idx);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tID: 0x%02x\n", info->sink_id);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tID(unit_id): 0x%02x\n", info->unit_id);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tID(link): 0x%02x\n", info->source_id);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tsize: %dbyte\n", info->control_size);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tnum_channels: %d\n", info->num_channels);

		for (int i = 0; i <= info->num_channels; i++) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\tch %d%s support: %02x\n",
					 i, (i == 0) ? " (master)" : "", info->bma_controls[i]);
		}
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
	usbh_composite_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	usb_ac_itf_desc_header_t *ac_header;
	u8 *desc;
	u8 t;
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
				break;
			}

			len = ((usbh_desc_header_t *) desc)->bLength;
			desc += len;
		} else if (type == USB_UAC1_INTERFACE_DESC_TYPE) {
			len = ((usbh_desc_header_t *) desc)->bLength;
			ac_header = (usb_ac_itf_desc_header_t *)desc;
			subtype = ac_header->bDescriptorSubtype;

			if (subtype == USB_UAC1_HEADER) {
				//get the total ac length
			} else if (subtype == USB_UAC1_INPUT_TERMINAL) {
				if (len >= 0x0C) {
					usbh_composite_uac_term_info_t term = {
						.terminal_id = desc[3],
						.terminal_type = (desc[5] << 8) | desc[4],
						.is_input = 1
					};
					usbh_composite_uac_add_terminal(ac_info, &term);
				}
			} else if (subtype == USB_UAC1_OUTPUT_TERMINAL) {
				if (len >= 0x09) {
					usbh_composite_uac_term_info_t term = {
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
		if (len == 0) {
			break;
		}
		itf_total_len += len;
	}

	desc = itf_data->raw_data;
	itf_total_len = 0;
	while (1) {
		if (desc == NULL || itf_total_len >= itf_data->raw_data_len) {
			break;
		}

		type = ((usbh_desc_header_t *) desc)->bDescriptorType;
		if (type == USB_DESC_TYPE_INTERFACE) {
			if (((usbh_itf_desc_t *)desc)->bInterfaceNumber != ac_info->ac_itf_idx) { //find another itf, maybe it is the as itf, should return
				break;
			}

			len = ((usbh_desc_header_t *) desc)->bLength;
			desc += len;
		} else if (type == USB_UAC1_INTERFACE_DESC_TYPE) {
			len = ((usbh_desc_header_t *) desc)->bLength;
			ac_header = (usb_ac_itf_desc_header_t *)desc;
			subtype = ac_header->bDescriptorSubtype;

			if (subtype == USB_UAC1_FEATURE_UNIT) {
				if (len >= 0x07) {
					usbh_composite_uac_fu_info_t vol_info = {0};
					vol_info.unit_id = desc[3];
					vol_info.source_id = desc[4];
					vol_info.control_size = desc[5];
					if (vol_info.control_size == 0) {
						/* Malformed FU descriptor: advance past it before bailing out,
						 * otherwise the next parse_ac() call would re-enter at the
						 * same offset and spin forever. */
						itf_total_len += len;
						break;
					}
					vol_info.num_channels = (len - 6 - 1) / (vol_info.control_size) - 1; //bmacontrols

					if (vol_info.num_channels > USBH_UAC_MAX_CHANNEL) {
						vol_info.num_channels = USBH_UAC_MAX_CHANNEL;
					}

					if (vol_info.control_size == 1) {
						vol_info.bma_controls[0] = desc[6];

						for (ch = 0; ch < vol_info.num_channels; ch++) {
							if (7 + ch < len) {
								vol_info.bma_controls[ch + 1] = desc[7 + ch];
							}
						}
					} else {
						vol_info.bma_controls[0] = USBH_LE16(desc + 6);

						for (ch = 0; ch < vol_info.num_channels; ch++) {
							if (8 + 2 * ch < len) {
								vol_info.bma_controls[ch + 1] = USBH_LE16(desc + 8 + 2 * ch);
							}
						}
					}

					for (t = 0; t < ac_info->terminal_count; t++) {
						if (!ac_info->terminals[t].is_input &&
							ac_info->terminals[t].source_id == vol_info.unit_id) { // find output information

							vol_info.sink_id = ac_info->terminals[t].terminal_id;
							vol_info.sink_type = ac_info->terminals[t].terminal_type;
							break;
						}
					}

					if (vol_info.bma_controls[0] || vol_info.num_channels > 0) {
						usbh_composite_uac_add_vol_ctrl(ac_info, &vol_info);
					}
				}
			}

			desc += len;
		} else {
			len = ((usbh_desc_header_t *) desc)->bLength;
			desc += len;
		}
		if (len == 0) {
			break;
		}
		itf_total_len += len;
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
	usbh_composite_uac_as_itf_alt_info_t *alt_setting = NULL;
	usbh_composite_uac_format_cfg_t *format_info = NULL;
	usbh_composite_uac_as_itf_info_t *as_itf = NULL;
	usbh_ep_desc_t *ep_cfg = NULL;
	u8 *desc = itf_data->raw_data;
	u16 itf_total_len = 0;
	u16 len;
	u8 alt_set_idx;
	u8 k;

	as_itf = (usbh_composite_uac_as_itf_info_t *)usb_os_malloc(sizeof(usbh_composite_uac_as_itf_info_t));
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
				if (uac->isoc_in.as_itf != as_itf && uac->isoc_out.as_itf != as_itf) {
					usb_os_mfree(as_itf);
				}
				return HAL_OK;
			}

			alt_set_idx = ((usbh_itf_desc_t *)desc)->bAlternateSetting;
			if ((alt_set_idx != 0) && (as_itf->alt_setting_cnt < USBH_UAC_ALT_SETTING_MAX)) {
				alt_setting = &(as_itf->interface_array[as_itf->alt_setting_cnt]);
				alt_setting->alt_setting = alt_set_idx;

				as_itf->alt_setting_cnt ++;
				len = ((usbh_desc_header_t *) desc)->bLength;
				desc += len;
			} else {
				RTK_LOGS(TAG, RTK_LOG_WARN, "As alt %d > cfg %d limit\n", as_itf->alt_setting_cnt, USBH_UAC_ALT_SETTING_MAX);
				if (uac->isoc_in.as_itf != as_itf && uac->isoc_out.as_itf != as_itf) {
					usb_os_mfree(as_itf);
				}
				return HAL_OK;
			}
			break;

		case USB_UAC1_INTERFACE_DESC_TYPE: {
			usb_uac1_format_type_i_discrete_descriptor *psubtype = (usb_uac1_format_type_i_discrete_descriptor *)desc;
			if ((alt_setting != NULL) && (USB_UAC1_FORMAT_TYPE == psubtype->bDescriptorSubtype)) { /* get the format */
				format_info = &(alt_setting->format_info);
				format_info->channels = psubtype->bNrChannels;
				format_info->bit_width = psubtype->bBitResolution;
				format_info->freq_cnt = psubtype->bSamFreqType;

				if (format_info->freq_cnt > USBH_UAC_FREQ_FORMAT_MAX) {
					RTK_LOGS(TAG, RTK_LOG_WARN, "Freq cnt(%d) > cfg(%d) limit\n", format_info->freq_cnt, USBH_UAC_FREQ_FORMAT_MAX);
					format_info->freq_cnt = USBH_UAC_FREQ_FORMAT_MAX;
				}

				for (k = 0; k < format_info->freq_cnt; k++) {
					format_info->freq[k] = USBH_UAC_FREQ(psubtype->tSamFreq[k]);
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
					if (uac->isoc_in.as_itf == NULL) {
						uac->isoc_in.as_itf = as_itf;
					}
				} else {
					if (uac->isoc_out.as_itf == NULL) {
						uac->isoc_out.as_itf = as_itf;
					}
				}
			}

			len = ((usbh_desc_header_t *) desc)->bLength;
			desc += len;
		}
		break;

		case USB_UAC1_AUDIO_EP_DESC_TYPE:
		default: {
			len = ((usbh_desc_header_t *) desc)->bLength;
			desc += len;
		}
		break;
		}
		if (len == 0) {
			break;
		}
		itf_total_len += len;
	}

	if (uac->isoc_in.as_itf != as_itf && uac->isoc_out.as_itf != as_itf) {
		usb_os_mfree(as_itf);
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
	dev_id.bInterfaceClass = USB_UAC1_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_UAC1_SUBCLASS_AUDIOCONTROL;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS | USBH_DEV_ID_MATCH_ITF_SUBCLASS;
	usbh_itf_data_t *itf_data = usbh_get_interface_descriptor(host, &dev_id);
	int ret = HAL_OK;

	if (itf_data) {
		/* The standard AC interface bInterfaceProtocol encodes the Audio Class
		   version: 0x00 = UAC1, 0x20 = UAC2. This driver implements UAC 1.0 only;
		   a UAC 2.0 device (which can also enumerate at Full Speed) would be
		   mis-parsed, so reject it early with a clear message. */
		u8 proto = itf_data->itf_desc_array[0].bInterfaceProtocol;
		if (proto != USB_UAC_IP_VERSION_1) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Bad v%02X, v1.0 only\n", proto);
			return HAL_ERR_PARA;
		}

		ret = usbh_composite_uac_parse_ac(itf_data);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "AC parse fail\n");
			return ret;
		} else {
			usbh_composite_uac_find_best_ac();
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No AC itf\n");
		return HAL_ERR_PARA;
	}

	dev_id.bInterfaceClass = USB_UAC1_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_UAC1_SUBCLASS_AUDIOSTREAMING;
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

/**
  * @brief  Send a USB SET_INTERFACE request to switch the AS interface to the selected alternate setting.
  * @param  host: Pointer to the USB host handle.
  * @retval HAL_OK on success, HAL_BUSY if transfer is pending, or an error code.
  */
static int usbh_composite_uac_process_set_alt(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_as_itf_info_t *as_itf = (uac->cur_dir == USBH_UAC_ISOC_OUT_DIR) ? uac->isoc_out.as_itf : uac->isoc_in.as_itf;

	if ((as_itf == NULL) || (as_itf->alt_setting_cnt == 0)) {
		return HAL_ERR_PARA;
	}

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_REQ_SET_INTERFACE;
	setup.req.wValue = as_itf->interface_array[as_itf->choose_alt_idx].alt_setting;
	setup.req.wIndex = as_itf->as_itf_num;
	setup.req.wLength = 0U;

	return usbh_ctrl_request(host, &setup, NULL);
}

/**
  * @brief  Send a UAC1 SET_CUR request to program the sampling frequency on the isochronous endpoint.
  * @param  host: Pointer to the USB host handle.
  * @retval HAL_OK on success, HAL_BUSY if transfer is pending, or an error code.
  */
static int usbh_composite_uac_process_set_freq(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_as_itf_info_t *as_itf = (uac->cur_dir == USBH_UAC_ISOC_OUT_DIR) ? uac->isoc_out.as_itf : uac->isoc_in.as_itf;
	usbh_composite_uac_format_cfg_t *fmt_info;

	if (as_itf == NULL) {
		return HAL_ERR_PARA;
	}

	fmt_info = &(as_itf->interface_array[as_itf->choose_alt_idx].format_info);

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_ENDPOINT;
	setup.req.bRequest = USB_UAC1_SET_CUR;
	setup.req.wValue = USBH_UAC_SAMPLING_FREQ_CONTROL;
	setup.req.wIndex = as_itf->pipe.ep_addr;
	setup.req.wLength = 3U;

	u32 _freq = fmt_info->freq[as_itf->choose_freq_idx];
	uac->audio_ctrl_buf[0] = (u8)(_freq & 0xFFU);
	uac->audio_ctrl_buf[1] = (u8)((_freq >> 8) & 0xFFU);
	uac->audio_ctrl_buf[2] = (u8)((_freq >> 16) & 0xFFU);

	return usbh_ctrl_request(host, &setup, uac->audio_ctrl_buf);
}

/**
  * @brief  Send a UAC1 SET_CUR request to set the volume level for a specific channel.
  * @param  host: Pointer to the USB host handle.
  * @param  ch: Channel number (0 = master, 1..N = individual channels).
  * @retval HAL_OK on success, HAL_BUSY if transfer is pending, or an error code.
  */
static int usbh_composite_uac_process_set_ch_volume(usb_host_t *host, u8 ch)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	u8 idx = (uac->cur_dir == USBH_UAC_ISOC_IN_DIR) ? ac_info->in_best_idx : ac_info->out_best_idx;
	usbh_composite_uac_volume_info_t *volume_info;
	usbh_composite_uac_fu_info_t *info;
	u16 new_volume_db;

	if (idx == (u8) - 1) {
		return HAL_OK;
	}
	info = &(ac_info->fu_controls[idx]);

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_UAC1_SET_CUR;
	setup.req.wValue = (ch) | (USB_UAC1_FU_VOLUME << 8);
	setup.req.wIndex = (ac_info->ac_itf_idx) | (info->unit_id << 8);
	setup.req.wLength = 2U;

	if (uac->cur_dir == USBH_UAC_ISOC_OUT_DIR) {
		volume_info = &(uac->isoc_out.volume_info[ch]);
	} else {
		volume_info = &(uac->isoc_in.volume_info[ch]);
	}

	new_volume_db = (u16)usbh_composite_uac_volume_to_db(volume_info, uac->volume_value);

	uac->audio_ctrl_buf[0] = (u8)(new_volume_db);
	uac->audio_ctrl_buf[1] = (u8)((new_volume_db >> 8) & 0xFF);

	return usbh_ctrl_request(host, &setup, uac->audio_ctrl_buf);
}

/**
  * @brief  Iteratively set the volume level on master and all individual channels that support it.
  *         Advances uac->ch_idx on each successful transfer; returns HAL_OK when all channels are done.
  * @param  host: Pointer to the USB host handle.
  * @retval HAL_OK when the full sequence completes, HAL_BUSY while in progress, or an error code.
  */
static int usbh_composite_uac_process_set_volume(usb_host_t *host)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	u8 idx = (uac->cur_dir == USBH_UAC_ISOC_IN_DIR) ? ac_info->in_best_idx : ac_info->out_best_idx;
	usbh_composite_uac_fu_info_t *info;
	int ret = HAL_BUSY;

	if (idx == (u8) - 1) {
		return HAL_OK;
	}
	info = &(ac_info->fu_controls[idx]);

	/* Walk bma_controls[0..num_channels]: ch=0 is master, ch>0 are per-channel.
	   bma_controls indexes 1:1 with the UAC1 bmaControls array, so master and
	   per-channel use the same iteration. */
	if (uac->ch_idx <= info->num_channels) {
		if (info->bma_controls[uac->ch_idx] & USB_UAC1_CONTROL_VOLUME) {
			ret = usbh_composite_uac_process_set_ch_volume(host, uac->ch_idx);
			if (ret == HAL_OK) {
				uac->ch_idx++;
				ret = HAL_BUSY;
			} else if (ret != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set volume ch%d err %d\n", uac->ch_idx, ret);
			}
		} else {
			uac->ch_idx++;
		}
	} else {
		ret = HAL_OK;
		uac->ch_idx = 0;
	}

	return ret;
}

/**
  * @brief  Send a UAC1 SET_CUR request to set the mute state for a specific channel.
  * @param  host: Pointer to the USB host handle.
  * @param  ch: Channel number (0 = master, 1..N = individual channels).
  * @retval HAL_OK on success, HAL_BUSY if transfer is pending, or an error code.
  */
static int usbh_composite_uac_process_set_ch_mute(usb_host_t *host, u8 ch)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	u8 idx = (uac->cur_dir == USBH_UAC_ISOC_IN_DIR) ? ac_info->in_best_idx : ac_info->out_best_idx;
	usbh_composite_uac_fu_info_t *info;

	if (idx == (u8) - 1) {
		return HAL_OK;
	}
	info = &(ac_info->fu_controls[idx]);

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_UAC1_SET_CUR;
	setup.req.wValue = (ch) | (USB_UAC1_FU_MUTE << 8);
	setup.req.wIndex = (ac_info->ac_itf_idx) | (info->unit_id << 8);
	setup.req.wLength = 1U;

	uac->audio_ctrl_buf[0] = uac->mute_value;

	return usbh_ctrl_request(host, &setup, uac->audio_ctrl_buf);
}

/**
  * @brief  Iteratively set the mute state on master and all individual channels that support it.
  *         Advances uac->ch_idx on each successful transfer; returns HAL_OK when all channels are done.
  * @param  host: Pointer to the USB host handle.
  * @retval HAL_OK when the full sequence completes, HAL_BUSY while in progress, or an error code.
  */
static int usbh_composite_uac_process_set_mute(usb_host_t *host)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	u8 idx = (uac->cur_dir == USBH_UAC_ISOC_IN_DIR) ? ac_info->in_best_idx : ac_info->out_best_idx;
	usbh_composite_uac_fu_info_t *info;
	int ret = HAL_BUSY;

	if (idx == (u8) - 1) {
		return HAL_OK;
	}
	info = &(ac_info->fu_controls[idx]);

	/* Walk bma_controls[0..num_channels]: ch=0 is master, ch>0 are per-channel. */
	if (uac->ch_idx <= info->num_channels) {
		if (info->bma_controls[uac->ch_idx] & USB_UAC1_CONTROL_MUTE) {
			ret = usbh_composite_uac_process_set_ch_mute(host, uac->ch_idx);
			if (ret == HAL_OK) {
				uac->ch_idx++;
				ret = HAL_BUSY;
			} else if (ret != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set mute ch%d err %d\n", uac->ch_idx, ret);
			}
		} else {
			uac->ch_idx++;
		}
	} else {
		ret = HAL_OK;
		uac->ch_idx = 0;
	}

	return ret;
}

/**
  * @brief  Send a UAC1 GET_CUR request to read the current mute state for a specific channel.
  * @param  host: Pointer to the USB host handle.
  * @param  ch: Channel number (0 = master, 1..N = individual channels).
  * @param  dir: USBH_UAC_ISOC_OUT_DIR or USBH_UAC_ISOC_IN_DIR.
  * @retval HAL_OK on success, HAL_BUSY if transfer is pending, or an error code.
  */
static int usbh_composite_uac_process_get_cur_mute(usb_host_t *host, u8 ch, u8 dir)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	u8 idx = (dir == USBH_UAC_ISOC_IN_DIR) ? (ac_info->in_best_idx) : (ac_info->out_best_idx);
	usbh_composite_uac_fu_info_t *info;

	if (idx == (u8) - 1) {
		/* No Feature Unit on this direction; avoid fu_controls[255] OOB. */
		return HAL_ERR_PARA;
	}
	info = &(ac_info->fu_controls[idx]);

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_UAC1_GET_CUR;
	setup.req.wValue = (ch) | (USB_UAC1_FU_MUTE << 8);
	setup.req.wIndex = (ac_info->ac_itf_idx) | (info->unit_id << 8);
	setup.req.wLength = 1U;

	return usbh_ctrl_request(host, &setup, uac->audio_ctrl_buf);
}

/**
  * @brief  Send a UAC1 GET_CUR request to read the current volume level for a specific channel.
  * @param  host: Pointer to the USB host handle.
  * @param  ch: Channel number (0 = master, 1..N = individual channels).
  * @param  dir: USBH_UAC_ISOC_OUT_DIR or USBH_UAC_ISOC_IN_DIR.
  * @retval HAL_OK on success, HAL_BUSY if transfer is pending, or an error code.
  */
static int usbh_composite_uac_process_get_cur_volume(usb_host_t *host, u8 ch, u8 dir)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	u8 idx = (dir == USBH_UAC_ISOC_IN_DIR) ? (ac_info->in_best_idx) : (ac_info->out_best_idx);
	usbh_composite_uac_fu_info_t *info;

	if (idx == (u8) - 1) {
		/* No Feature Unit on this direction; avoid fu_controls[255] OOB. */
		return HAL_ERR_PARA;
	}
	info = &(ac_info->fu_controls[idx]);

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_UAC1_GET_CUR;
	setup.req.wValue = (ch) | (USB_UAC1_FU_VOLUME << 8);
	setup.req.wIndex = (ac_info->ac_itf_idx) | (info->unit_id << 8);
	setup.req.wLength = 2U;

	return usbh_ctrl_request(host, &setup, uac->audio_ctrl_buf);
}

/**
  * @brief  Send a UAC1 GET_MIN or GET_MAX request to read the volume range for a specific channel.
  * @param  host: Pointer to the USB host handle.
  * @param  min:  1 to query GET_MIN, 0 to query GET_MAX.
  * @param  ch:   Channel number (0 = master, 1..N = individual channels).
  * @param  dir:  USBH_UAC_ISOC_OUT_DIR or USBH_UAC_ISOC_IN_DIR.
  * @retval HAL_OK on success, HAL_BUSY if transfer is pending, or an error code.
  */
static int usbh_composite_uac_process_get_volume_range(usb_host_t *host, u8 min, u8 ch, u8 dir)
{
	usbh_setup_req_t setup;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	u8 idx = (dir == USBH_UAC_ISOC_IN_DIR) ? (ac_info->in_best_idx) : (ac_info->out_best_idx);
	usbh_composite_uac_fu_info_t *info;

	if (idx == (u8) - 1) {
		/* No Feature Unit on this direction; avoid fu_controls[255] OOB. */
		return HAL_ERR_PARA;
	}
	info = &(ac_info->fu_controls[idx]);

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = (min) ? (USB_UAC1_GET_MIN) : (USB_UAC1_GET_MAX);
	setup.req.wValue = (ch) | (USB_UAC1_FU_VOLUME << 8);
	setup.req.wIndex = (ac_info->ac_itf_idx) | (info->unit_id << 8);
	setup.req.wLength = 2U;

	return usbh_ctrl_request(host, &setup, uac->audio_ctrl_buf);
}

/**
  * @brief  State machine that sequentially queries mute, current volume, min volume, and max volume
  *         for a single channel of the given Feature Unit via UAC1 GET_CUR/GET_MIN/GET_MAX requests.
  * @param  host:        Pointer to the USB host handle.
  * @param  bma_control: Bitmap of controls supported by this channel (USB_UAC1_FU_MUTE, USB_UAC1_FU_VOLUME).
  * @param  ch:          Channel number (0 = master, 1..N = individual channels).
  * @param  dir:         USBH_UAC_ISOC_OUT_DIR or USBH_UAC_ISOC_IN_DIR.
  * @retval HAL_OK when the sequence for this channel completes, HAL_BUSY while in progress, or an error code.
  */
static int usbh_composite_uac_get_unit_ctrl(usb_host_t *host, u16 bma_control, u8 ch, u8 dir)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_volume_info_t *volume_handle;
	int ret = HAL_BUSY;

	if (dir == USBH_UAC_ISOC_OUT_DIR) {
		volume_handle = &(uac->isoc_out.volume_info[ch]);
	} else {
		volume_handle = &(uac->isoc_in.volume_info[ch]);
	}

	//1. get cur mute
	//2. loop all channel get volume : cur, min, max , res
	if (uac->ctrl_state == UAC_STATE_SCAN_MUTE) {
		if (bma_control & USB_UAC1_CONTROL_MUTE) {
			ret = usbh_composite_uac_process_get_cur_mute(host, ch, dir);
			if (ret == HAL_OK) {
				//parse to get the buffer
				uac->ctrl_state = UAC_STATE_SCAN_CUR_VOLUME;
				volume_handle->mute = uac->audio_ctrl_buf[0];
				ret = HAL_BUSY;
			} else if (ret != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get mute err %d\n", ret);
				uac->ctrl_state = UAC_STATE_SCAN_CUR_VOLUME;
			}
		} else {
			uac->ctrl_state = UAC_STATE_SCAN_CUR_VOLUME;
		}
	} else if (uac->ctrl_state == UAC_STATE_SCAN_CUR_VOLUME) {
		if (bma_control & USB_UAC1_CONTROL_VOLUME) {
			ret = usbh_composite_uac_process_get_cur_volume(host, ch, dir);
			if (ret == HAL_OK) {
				uac->ctrl_state = UAC_STATE_SCAN_MIN_VOLUME;
				//parse to get the volume info
				volume_handle->volume = USBH_LE16(uac->audio_ctrl_buf);
				ret = HAL_BUSY;
			} else if (ret != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get vol err %d\n", ret);
				uac->ctrl_state = UAC_STATE_SCAN_MIN_VOLUME;
			}
		} else {
			uac->ctrl_state = UAC_STATE_SCAN_MIN_VOLUME;
		}
	} else if (uac->ctrl_state == UAC_STATE_SCAN_MIN_VOLUME) {
		if (bma_control & USB_UAC1_CONTROL_VOLUME) {
			ret = usbh_composite_uac_process_get_volume_range(host, 1, ch, dir);
			if (ret == HAL_OK) {
				uac->ctrl_state = UAC_STATE_SCAN_MAX_VOLUME;
				volume_handle->vol_min = USBH_LE16(uac->audio_ctrl_buf);
				ret = HAL_BUSY;
			} else if (ret != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get vol min err %d\n", ret);
				uac->ctrl_state = UAC_STATE_SCAN_MAX_VOLUME;
			}
		} else {
			uac->ctrl_state = UAC_STATE_SCAN_MAX_VOLUME;
		}
	} else if (uac->ctrl_state == UAC_STATE_SCAN_MAX_VOLUME) {
		if (bma_control & USB_UAC1_CONTROL_VOLUME) {
			ret = usbh_composite_uac_process_get_volume_range(host, 0, ch, dir);
			if (ret == HAL_OK) {
				uac->ctrl_state = UAC_STATE_CTRL_IDLE;
				volume_handle->vol_max = USBH_LE16(uac->audio_ctrl_buf);
				volume_handle->range_valid = 1;
#if USBH_COMPOSITE_HID_UAC_DEBUG
				RTK_LOGS(TAG, RTK_LOG_INFO, "Cur is %s, ch %d\n", ((dir) ? ("IN") : ("OUT")), ch);
				RTK_LOGS(TAG, RTK_LOG_INFO, "\tmute %d\n", volume_handle->mute);
				RTK_LOGS(TAG, RTK_LOG_INFO, "\tvolume 0x%04x\n", (u16)volume_handle->volume);
				RTK_LOGS(TAG, RTK_LOG_INFO, "\tvolume min 0x%04x\n", (u16)volume_handle->vol_min);
				RTK_LOGS(TAG, RTK_LOG_INFO, "\tvolume max 0x%04x\n", (u16)volume_handle->vol_max);
#endif
			} else if (ret != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get vol max err %d\n", ret);
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
  * @brief  Dequeue one packet from the OUT ring buffer and submit an isochronous OUT transfer for playback.
  *         Signals the ring buffer semaphore after dequeue; sends nothing if the buffer is empty.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `usb_os_sema_take`) are not permitted.
  * @param  host:      Pointer to the USB host handle.
  * @param  cur_frame: Current USB frame number used to schedule the next isochronous transfer.
  * @retval void
  */
static void usbh_composite_uac_isoc_out_process_xfer(usb_host_t *host, u32 cur_frame)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_pipe_t *pipe = &(uac->isoc_out.as_itf->pipe);
	usbh_composite_uac_buf_ctrl_t *pdata_ctrl = &(uac->isoc_out.buf_ctrl);
	usb_ringbuf_manager_t *buf_manager = &(pdata_ctrl->buf_manager);
	u16 read_len;
	u8 zlp = 0;

	if (!usb_ringbuf_is_empty(buf_manager)) {
		read_len = usb_ringbuf_remove_head(buf_manager, uac->isoc_out.xfer_buf, USBH_UAC_ISOC_BUF_LENGTH, &zlp);
		if (pdata_ctrl->sema_valid) {
			usb_os_sema_give(pdata_ctrl->isoc_sema);
		}
		if (read_len > 0) {
			pipe->frame_num = usbh_composite_uac_frame_num_inc(cur_frame, 1);
#if USBH_COMPOSITE_HID_UAC_DEBUG
			pdata_ctrl->xfer_start_cnt ++;
#endif
			pipe->xfer_buf = uac->isoc_out.xfer_buf;
			pipe->xfer_len = read_len;
			usbh_transfer_data(host, pipe);
			pipe->xfer_state = USBH_EP_XFER_BUSY;
		} else { //data invalid
#if USBH_COMPOSITE_HID_UAC_DEBUG
			pdata_ctrl->xfer_buf_err_cnt ++;
#endif
		}
	} else { //ringbuf empty
#if USBH_COMPOSITE_HID_UAC_DEBUG
		pdata_ctrl->xfer_buf_empty_cnt ++;
#endif
	}
}


/**
  * @brief  Submit an isochronous IN transfer to receive audio data from the microphone endpoint.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `usb_os_sema_take`) are not permitted.
  * @param  host:      Pointer to the USB host handle.
  * @param  cur_frame: Current USB frame number used to schedule the next isochronous transfer.
  * @retval void
  */
static void usbh_composite_uac_isoc_in_process_xfer(usb_host_t *host, u32 cur_frame)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_pipe_t *pipe = &(uac->isoc_in.as_itf->pipe);
	usbh_composite_uac_buf_ctrl_t *pdata_ctrl = &(uac->isoc_in.buf_ctrl);
	u32 xfer_len;

	xfer_len = uac->isoc_in.as_itf->packet_size_large;
	if (xfer_len == 0) {
		xfer_len = pdata_ctrl->mps;
	}

	pipe->frame_num = usbh_composite_uac_frame_num_inc(cur_frame, 1);
#if USBH_COMPOSITE_HID_UAC_DEBUG
	pdata_ctrl->xfer_start_cnt ++;
#endif
	pipe->xfer_buf = uac->isoc_in.xfer_buf;
	pipe->xfer_len = xfer_len;

	usbh_transfer_data(host, pipe);
	pipe->xfer_state = USBH_EP_XFER_BUSY;
}
/**
  * @brief  Device attach callback: parse configuration descriptors, open isochronous pipes,
  *         and kick off the initialization control sequence (get volume/mute info for all channels).
  * @param  host: Pointer to the USB host handle.
  * @retval HAL_OK on success, or an error code if descriptor parsing fails.
  */
static int usbh_composite_uac_attach(usb_host_t *host)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_as_itf_info_t *as_itf = NULL;
	usbh_ep_desc_t *ep_desc = NULL;
	usbh_pipe_t *pipe = NULL;

	/* UAC1 is designed for Full Speed (12 Mbps). USB 2.0 High Speed uses
	 * microframes (125 us) instead of frames (1 ms), which breaks the
	 * isochronous timing model in this driver. Print a warning but allow
	 * the attach to proceed-some devices may still work. */
	if (host->dev_speed == USB_SPEED_HIGH) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Not support USB 2.0(High Speed), please use USB 1.1(Full Speed)\n");
	}

	int status = HAL_ERR_UNKNOWN;

	status = usbh_composite_uac_parse_interface_desc(host);
	if (status) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Cfg parse fail\n");
		return status;
	}
	usbh_composite_uac_dump_cfgdesc();

	usbh_composite_uac_get_audio_format_info();

	if ((uac->isoc_in.as_itf != NULL) && (uac->isoc_in.buf_ctrl.frame_cnt == 0)) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Drop IN: device offers but cfg disable\n");
		if (uac->isoc_in.as_itf->fmt_array != NULL) {
			usb_os_mfree(uac->isoc_in.as_itf->fmt_array);
			uac->isoc_in.as_itf->fmt_array = NULL;
		}
		usb_os_mfree(uac->isoc_in.as_itf);
		uac->isoc_in.as_itf = NULL;
	}

	if ((uac->isoc_out.as_itf != NULL) && (uac->isoc_out.buf_ctrl.frame_cnt == 0)) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Drop OUT: device offers but cfg disable\n");
		if (uac->isoc_out.as_itf->fmt_array != NULL) {
			usb_os_mfree(uac->isoc_out.as_itf->fmt_array);
			uac->isoc_out.as_itf->fmt_array = NULL;
		}
		usb_os_mfree(uac->isoc_out.as_itf);
		uac->isoc_out.as_itf = NULL;
	}

	if (uac->isoc_in.as_itf) {
		as_itf = uac->isoc_in.as_itf;
		as_itf->choose_alt_idx = 0;

		pipe = &(as_itf->pipe);
		ep_desc = &(as_itf->interface_array[as_itf->choose_alt_idx].ep_desc);

		usbh_open_pipe(host, pipe, ep_desc);
	} else {
		if (uac->isoc_in.xfer_buf) {
			usb_os_mfree(uac->isoc_in.xfer_buf);
			uac->isoc_in.xfer_buf = NULL;
		}
	}

	if (uac->isoc_out.as_itf) {
		as_itf = uac->isoc_out.as_itf;
		as_itf->choose_alt_idx = 0;

		pipe = &(as_itf->pipe);
		ep_desc = &(as_itf->interface_array[as_itf->choose_alt_idx].ep_desc);

		usbh_open_pipe(host, pipe, ep_desc);
	} else {
		if (uac->isoc_out.xfer_buf) {
			usb_os_mfree(uac->isoc_out.xfer_buf);
			uac->isoc_out.xfer_buf = NULL;
		}
	}

	if ((uac->cb != NULL) && (uac->cb->attach != NULL)) {
		uac->cb->attach();
	}

	uac->xfer_state = UAC_STATE_TRANSFER;
	uac->ctrl_state = UAC_STATE_SCAN_MUTE;
	uac->init_state = UAC_INIT_OUT_GET_FU;
	uac->ch_idx = 0;

	return HAL_OK;
}

/**
  * @brief  Device detach callback: stop all isochronous transfers and notify the application.
  * @param  host: Pointer to the USB host handle (unused).
  * @retval HAL_OK.
  */
static int usbh_composite_uac_detach(usb_host_t *host)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	UNUSED(host);

	uac->xfer_state = UAC_STATE_IDLE;

	/* Wake any thread blocked inside set_alt_setting/set_volume/set_mute/get_mute/get_volume.
	 * Once xfer_state is IDLE, ctrl_setting() will not run any more, so the
	 * waiter would otherwise time out 1 s later. Surface the disconnect
	 * promptly via ctrl_status. */
	uac->ctrl_state = UAC_STATE_CTRL_IDLE;
	if (uac->ctrl_waiting) {
		uac->ctrl_status = HAL_ERR_UNKNOWN;
		usb_os_sema_give(uac->ctrl_done_sema);
	}

	if ((uac->cb != NULL) && (uac->cb->detach != NULL)) {
		uac->cb->detach();
	}

	return HAL_OK;
}

/**
  * @brief  Setup-stage callback invoked after each USB Setup transaction completes.
  *         Forwards the event to the user-registered setup callback if present.
  * @param  host: Pointer to the USB host handle (unused).
  * @retval HAL_OK.
  */
static int usbh_composite_uac_setup(usb_host_t *host)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	UNUSED(host);

	if ((uac->cb != NULL) && (uac->cb->setup != NULL)) {
		uac->cb->setup();
	}

	return HAL_OK;
}

/**
  * @brief  SOF (Start-of-Frame) callback: check the isochronous OUT and IN transfer schedules
  *         and trigger new transfers when the endpoint interval has elapsed.
  *         Also wakes the control state machine if a pending control request is queued.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `usb_os_sema_take`) are not permitted.
  * @param[in] host: Pointer to the USB host handle.
  * @return 0 on success, non-zero on failure.
  */
static int usbh_composite_uac_sof(usb_host_t *host)
{
	u32 cur_frame = usbh_get_current_frame_number(host);
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_buf_ctrl_t *pdata_ctrl ;
	usbh_pipe_t *pipe;

#if USBH_COMPOSITE_HID_UAC_DEBUG
	uac->sof_cnt ++;
#endif

	if (uac->isoc_out.as_itf) {
		pipe = &(uac->isoc_out.as_itf->pipe);
		pdata_ctrl = &(uac->isoc_out.buf_ctrl);

		if (pdata_ctrl->next_xfer == 1) {
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
					pdata_ctrl->xfer_interval_cnt ++;
				}
#endif
			}
		}
	}

	/* USB IN (microphone) handling */
	if (uac->isoc_in.as_itf) {
		pipe = &(uac->isoc_in.as_itf->pipe);
		pdata_ctrl = &(uac->isoc_in.buf_ctrl);

		if (pdata_ctrl->next_xfer == 1) {
			if ((usbh_get_elapsed_frame_cnt(host, pipe->frame_num) >= pipe->ep_interval) ||
				((pipe->xfer_state == USBH_EP_XFER_WAIT_SOF) &&
				 (usbh_composite_uac_frame_num_dec(usbh_composite_uac_frame_num_inc(cur_frame, 1), pipe->frame_num) >= pipe->ep_interval))) {
				usbh_composite_uac_isoc_in_process_xfer(host, cur_frame);
			} else if (pipe->xfer_state == USBH_EP_XFER_IDLE) {
#if USBH_COMPOSITE_HID_UAC_DEBUG
				pdata_ctrl->xfer_interval_cnt ++;
#endif
			}
		}
	}

	if (uac->ctrl_state != UAC_STATE_CTRL_IDLE) {
		usbh_notify_composite_class_state_change(host, 0x00, USBH_COMPOSITE_UAC_EVENT);
	}
	return HAL_OK;
}

/**
  * @brief  Transfer completion callback: handle the end of an isochronous OUT or IN transfer,
  *         enqueue received IN data into the ring buffer, and schedule the next transfer.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `usb_os_sema_take`) are not permitted.
  * @param[in] host: Pointer to the USB host handle.
  * @param[in] pipe_num: Pipe number of the completed transfer.
  * @return 0 on success, non-zero on failure.
  */
static int usbh_composite_uac_completed(usb_host_t *host, u8 pipe_num)
{
	u32 cur_frame = usbh_get_current_frame_number(host);
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_buf_ctrl_t *pdata_ctrl;
	usbh_pipe_t *pipe;

	if (uac->isoc_out.as_itf) {
		pipe = &(uac->isoc_out.as_itf->pipe);
		pdata_ctrl = &(uac->isoc_out.buf_ctrl);

		if (pdata_ctrl->next_xfer == 1) {
			if (pipe_num == pipe->pipe_num) {
#if USBH_COMPOSITE_HID_UAC_DEBUG
				pdata_ctrl->xfer_done_cnt ++;
#endif
				pipe->xfer_state = USBH_EP_XFER_IDLE;

				if (!usb_ringbuf_is_empty(&(pdata_ctrl->buf_manager))) {
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
	}

	if (uac->isoc_in.as_itf) {
		pipe = &(uac->isoc_in.as_itf->pipe);
		pdata_ctrl = &(uac->isoc_in.buf_ctrl);

		if (pdata_ctrl->next_xfer == 1) {
			if (pipe_num == pipe->pipe_num) {
				u32 len = usbh_get_last_transfer_size(host, pipe);
#if USBH_COMPOSITE_HID_UAC_DEBUG
				pdata_ctrl->xfer_done_cnt ++;
				if (len > 0) {
					pdata_ctrl->last_xfer_len = len;
				}
#endif
				pipe->xfer_state = USBH_EP_XFER_IDLE;

				/* Add received data to ringbuf */
				if ((len > 0) && (!usb_ringbuf_is_full(&(pdata_ctrl->buf_manager)))) {
					usb_ringbuf_add_tail(&(pdata_ctrl->buf_manager), pipe->xfer_buf, len, 1);
					if (pdata_ctrl->sema_valid) {
						usb_os_sema_give(pdata_ctrl->isoc_sema);
					}
				}

				/* Trigger next IN transfer */
				if (usbh_composite_uac_frame_num_dec(usbh_composite_uac_frame_num_inc(cur_frame, 1), pipe->frame_num) >= pipe->ep_interval) {
					usbh_composite_uac_isoc_in_process_xfer(host, cur_frame);
				} else {
					pipe->xfer_state = USBH_EP_XFER_WAIT_SOF;
				}
			}
		}
	}

	return HAL_OK;
}

/**
  * @brief  Common terminator for any ctrl_state that is returning to IDLE.
  *         Records the final outcome in `ctrl_status` (so sync APIs see the real
  *         result) and wakes the waiter blocked on `ctrl_done_sema`.
  *         All sync APIs (set_alt_setting / set_volume / set_mute / get_mute /
  *         get_volume) hold alt_set_mutex across the entire wait, so at most one
  *         waiter exists at a time and no pending-request queueing is needed.
  * @param  status: HAL status the just-finished operation produced.
  */
static void usbh_composite_uac_ctrl_finish(int status)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;

	uac->ctrl_status = status;
	uac->ctrl_state = UAC_STATE_CTRL_IDLE;
	if (uac->ctrl_waiting) {
		usb_os_sema_give(uac->ctrl_done_sema);
	}
}

/**
  * @brief  Control-channel state machine: dispatch the current `ctrl_state` to the
  *         right helper (SET_ALT/SET_FREQ/SET_MUTE/SET_VOLUME/SCAN_x/GET_MUTE/GET_VOLUME)
  *         and advance state. On any terminal transition the common epilogue
  *         (`ctrl_finish`) records the outcome, releases sync waiters, and flushes
  *         queued ops.
  * @param  host: Pointer to the USB host handle.
  * @param  msg:  Unused message argument.
  * @retval HAL_OK when the current operation has terminated (success or error),
  *         HAL_BUSY while still in progress.
  */
static int usbh_composite_uac_ctrl_setting(usb_host_t *host, u32 msg)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_volume_info_t *vi;
	int ret = HAL_OK;
	int ret_status = HAL_BUSY;
	UNUSED(msg);

	switch (uac->ctrl_state) {
	case UAC_STATE_CTRL_IDLE:
		ret_status = HAL_OK;
		break;

	case UAC_STATE_SET_ALT_SETTING:
		ret = usbh_composite_uac_process_set_alt(host);
		if (ret == HAL_OK) {
			uac->ctrl_state = UAC_STATE_SET_FREQ;
		} else if (ret != HAL_BUSY) {
			/* Do NOT advance to SET_FREQ on alt-set failure: writing the new
			 * sample rate to the old endpoint would either be silently accepted
			 * (caller thinks alt was switched) or STALL on a different request,
			 * confusing the diagnosis. Surface the real error. */
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set alt err %d\n", ret);
			usbh_composite_uac_ctrl_finish(ret);
			ret_status = HAL_OK;
		}
		break;

	case UAC_STATE_SET_FREQ:
		ret = usbh_composite_uac_process_set_freq(host);
		if (ret == HAL_OK) {
			usbh_composite_uac_ctrl_finish(HAL_OK);
			ret_status = HAL_OK;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set(%d) freq err %d\n", uac->cur_dir, ret);
			usbh_composite_uac_ctrl_finish(ret);
			ret_status = HAL_OK;
		}
		break;

	case UAC_STATE_SET_MUTE:
		ret = usbh_composite_uac_process_set_mute(host);
		if (ret == HAL_OK) {
			usbh_composite_uac_ctrl_finish(HAL_OK);
			ret_status = HAL_OK;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set(%d) mute err %d\n", uac->cur_dir, ret);
			usbh_composite_uac_ctrl_finish(ret);
			ret_status = HAL_OK;
		}
		break;

	case UAC_STATE_SET_VOLUME:
		ret = usbh_composite_uac_process_set_volume(host);
		if (ret == HAL_OK) {
			usbh_composite_uac_ctrl_finish(HAL_OK);
			ret_status = HAL_OK;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set(%d) vol err %d\n", uac->cur_dir, ret);
			usbh_composite_uac_ctrl_finish(ret);
			ret_status = HAL_OK;
		}
		break;

	case UAC_STATE_GET_MUTE:
		ret = usbh_composite_uac_process_get_cur_mute(host, uac->get_ch, uac->cur_dir);
		if (ret == HAL_OK) {
			vi = (uac->cur_dir == USBH_UAC_ISOC_OUT_DIR) ?
				 &(uac->isoc_out.volume_info[uac->get_ch]) :
				 &(uac->isoc_in.volume_info[uac->get_ch]);
			uac->get_mute_result = uac->audio_ctrl_buf[0];
			vi->mute = uac->get_mute_result;     /* keep cache in sync */
			usbh_composite_uac_ctrl_finish(HAL_OK);
			ret_status = HAL_OK;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Get mute(%d) err %d\n", uac->cur_dir, ret);
			usbh_composite_uac_ctrl_finish(ret);
			ret_status = HAL_OK;
		}
		break;

	case UAC_STATE_GET_VOLUME:
		ret = usbh_composite_uac_process_get_cur_volume(host, uac->get_ch, uac->cur_dir);
		if (ret == HAL_OK) {
			vi = (uac->cur_dir == USBH_UAC_ISOC_OUT_DIR) ?
				 &(uac->isoc_out.volume_info[uac->get_ch]) :
				 &(uac->isoc_in.volume_info[uac->get_ch]);
			uac->get_volume_result = (s16)USBH_LE16(uac->audio_ctrl_buf);
			vi->volume = uac->get_volume_result; /* keep cache in sync */
			usbh_composite_uac_ctrl_finish(HAL_OK);
			ret_status = HAL_OK;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Get vol(%d) err %d\n", uac->cur_dir, ret);
			usbh_composite_uac_ctrl_finish(ret);
			ret_status = HAL_OK;
		}
		break;

	default:
		/* Unknown state means a programmer mistake (e.g. an enum value was
		 * added but its case forgotten). Forcing IDLE + signaling waiters
		 * keeps the host stack alive instead of silently hanging callers. */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Unknown ctrl_state %d\n", uac->ctrl_state);
		usbh_composite_uac_ctrl_finish(HAL_ERR_UNKNOWN);
		ret_status = HAL_OK;
		break;
	}

	return ret_status;
}

/**
  * @brief  Main UAC class process callback called by the USB host core on each event.
  *         In TRANSFER state, delegates pipe-0 events to the control state machine.
  *         In ERROR state, issues a ClearFeature to recover.
  * @param  host:  Pointer to the USB host handle.
  * @param  event: Pointer to the event descriptor (contains pipe_num and event type).
  * @retval HAL_OK on success, or an error code.
  */
static int usbh_composite_uac_process(usb_host_t *host, usbh_event_t *event)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	int ret = HAL_OK;

	switch (uac->xfer_state) {
	case UAC_STATE_TRANSFER:
		if ((event) && (event->pipe_num == 0x00)) {
			ret = usbh_composite_uac_ctrl_setting(host, 0);
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

/**
  * @brief  Compute the byte size of the next isochronous packet using the fractional accumulator.
  *         Adds sample_rem to sample_accum; if the result reaches packet_rate, one extra
  *         sample is included (packet_size_large); otherwise packet_size_small is used.
  *         Saves the new accumulator value in last_sample_accum for rollback on ring-buffer full.
  * @param  pdata_ctrl: Pointer to the buffer control structure carrying the accumulator state.
  * @param  dir:        USBH_UAC_ISOC_OUT_DIR or USBH_UAC_ISOC_IN_DIR.
  * @retval Byte size of the next packet (packet_size_small or packet_size_large).
  */
static u32 usbh_composite_uac_next_packet_size(usbh_composite_uac_buf_ctrl_t *pdata_ctrl, u8 dir)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_as_itf_info_t *as_itf;
	u32 sample_accum = 0;
	u32 ret;

	if (dir == USBH_UAC_ISOC_OUT_DIR) {
		as_itf = uac->isoc_out.as_itf;
	} else {
		as_itf = uac->isoc_in.as_itf;
	}

	sample_accum = pdata_ctrl->sample_accum + pdata_ctrl->sample_rem;
	if (sample_accum >= pdata_ctrl->packet_rate) {
		sample_accum -= pdata_ctrl->packet_rate;
		ret = as_itf->packet_size_large;
	} else {
		ret = as_itf->packet_size_small;
	}

	pdata_ctrl->last_sample_accum = sample_accum;

	return ret;
}

/**
  * @brief  Write PCM data into the OUT ring buffer, packing input into fixed isochronous packet slots.
  *         Handles partial packets across calls using the ringbuf_partial_write_buf staging buffer.
  * @param  pdata_ctrl: Pointer to the buffer control structure.
  * @param  buffer:     Pointer to input PCM data buffer.
  * @param  size:       Number of bytes to write.
  * @param  written_len: Accumulated number of bytes actually written (updated in-place).
  * @retval 0 when all input was consumed, 1 when the ring buffer became full before consuming all input.
  */
static int usbh_composite_uac_write_ring_buf(usbh_composite_uac_buf_ctrl_t *pdata_ctrl, u8 *buffer, u32 size, u32 *written_len)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usb_ringbuf_manager_t *handle = &(pdata_ctrl->buf_manager);
	u32 written_size = pdata_ctrl->written;
	u32 offset = 0;
	u32 xfer_len;
	u32 can_copy_len;
	u32 copy_len;

	if (written_size) {
		xfer_len = usbh_composite_uac_next_packet_size(pdata_ctrl, USBH_UAC_ISOC_OUT_DIR);
		can_copy_len = xfer_len - written_size;
		if (size >= can_copy_len && usb_ringbuf_is_full(handle)) {
			return 1;
		}

		copy_len = size < can_copy_len ? size : can_copy_len;

		usb_os_memcpy((void *)(uac->ringbuf_partial_write_buf + written_size), (void *)buffer, copy_len);
		pdata_ctrl->written += copy_len;

		offset += copy_len;
		*written_len += copy_len;

		if (size >= can_copy_len) {
			size -= copy_len;
			usb_ringbuf_add_tail(handle, uac->ringbuf_partial_write_buf, xfer_len, 1);
			pdata_ctrl->sample_accum = pdata_ctrl->last_sample_accum;
			pdata_ctrl->written = 0;
		} else {
			return 0;
		}
	}

	do {
		if (usb_ringbuf_is_full(handle)) {
			return 1;
		}

		xfer_len = usbh_composite_uac_next_packet_size(pdata_ctrl, USBH_UAC_ISOC_OUT_DIR);

		if (size >= xfer_len) {
			usb_ringbuf_add_tail(handle, buffer + offset, xfer_len, 1);

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

		usb_os_memcpy((void *)(uac->ringbuf_partial_write_buf), (void *)buffer, size);
		pdata_ctrl->written = size;
		*written_len += size;
	}
	return 0;
}

/**
  * @brief  Read available IN packets from the ring buffer into the caller's buffer.
  *         Stops when the destination buffer cannot fit another full MPS packet or is full.
  * @param  buf_ctrl:      Pointer to the IN buffer control structure.
  * @param  buffer:        Destination buffer for received PCM data.
  * @param  size:          Capacity of the destination buffer in bytes.
  * @param  copy_len:      Accumulated bytes copied (updated in-place).
  * @param  pkt_cnt:       Number of packets dequeued (updated in-place).
  * @param  zero_pkt_flag: Bitmask set for each packet that was a ZLP; may be NULL.
  * @retval 0 when no more data can be dequeued, non-zero to signal the caller to continue reading.
  */
static u32 usbh_composite_uac_read_ring_buf(usbh_composite_uac_buf_ctrl_t *buf_ctrl, u8 *buffer, u32 size, u32 *copy_len, u16 *pkt_cnt, u32 *zero_pkt_flag)
{
	usb_ringbuf_manager_t *buf_list = &(buf_ctrl->buf_manager);
	u32 read_len;
	u8 valid = 0;

	do {
		/* should exit : 1) Enough data has been obtained; 2) the next data cannot be saved completely */
		if ((*copy_len >= size) || (*copy_len + buf_ctrl->mps > size)) {
			return 0;
		}

		read_len = usb_ringbuf_remove_head(buf_list, buffer + *copy_len, (size - *copy_len), &valid);
		if (read_len > 0) {
			*copy_len += read_len;
			if ((valid == 0) && zero_pkt_flag) {
				*zero_pkt_flag |= 1 << *pkt_cnt;
			}

			*pkt_cnt = *pkt_cnt + 1;
		}
	} while (usb_ringbuf_is_empty(&(buf_ctrl->buf_manager)) == 0);

	/* should return 0 : enough data has been obtained; */
	if (*copy_len >= size) {
		return 0;
	}

	return 1;
}

/**
  * @brief  Deinitialize UAC endpoint buffer control structure
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @retval void
  */
static void usbh_composite_uac_ep_buf_ctrl_deinit(usbh_composite_uac_buf_ctrl_t *buf_ctrl)
{
	/* Bounded wait for any in-flight reader/writer to observe sema_valid=0
	 * and clear wait_sema. Without a timeout this loop deadlocks deinit
	 * if the waiter was killed or never gets a chance to run.
	 * usb_os_sleep_ms yields the CPU so the waiter can actually run. */
	u16 wait_ms = 10U; /* 10 ms total */

	buf_ctrl->mps = 0;
	buf_ctrl->next_xfer = 0;

	if (buf_ctrl->sema_valid) {
		buf_ctrl->sema_valid = 0;

		if (buf_ctrl->wait_sema) {
			usb_os_sema_give(buf_ctrl->isoc_sema);
		}
		while (buf_ctrl->wait_sema && wait_ms > 0U) {
			usb_os_sleep_ms(1U);
			wait_ms--;
		}
		/* Final handshake: if a waiter set wait_sema=1 right before the
		 * forced clear below, give the sema once more so the pending
		 * sema_take can complete safely, then sleep to let the waiter
		 * observe sema_valid==0 before we delete the semaphore. */
		buf_ctrl->wait_sema = 0;
		usb_os_sema_give(buf_ctrl->isoc_sema);
		usb_os_sleep_ms(1U);

		usb_os_sema_delete(buf_ctrl->isoc_sema);
	}
	usb_ringbuf_manager_deinit(&(buf_ctrl->buf_manager));
}

/**
  * @brief  Tear down all per-direction resources of one isochronous channel
  *         (buf_ctrl ring buffer, AS interface descriptor cache, DMA xfer buffer).
  *         Safe to call on a channel that was never fully initialized.
  * @param  ch: Pointer to either uac->isoc_out or uac->isoc_in.
  */
static void usbh_composite_uac_channel_deinit(usbh_composite_uac_channel_t *ch)
{
	usbh_composite_uac_ep_buf_ctrl_deinit(&(ch->buf_ctrl));

	if (ch->as_itf != NULL) {
		if (ch->as_itf->fmt_array != NULL) {
			usb_os_mfree(ch->as_itf->fmt_array);
			ch->as_itf->fmt_array = NULL;
		}
		usb_os_mfree(ch->as_itf);
		ch->as_itf = NULL;
	}

	if (ch->xfer_buf != NULL) {
		usb_os_mfree(ch->xfer_buf);
		ch->xfer_buf = NULL;
	}
}

/**
  * @brief  Initialize UAC endpoint buffer control structure
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @param  pipe: Pointer to pipe parameters structure
  * @retval Status
  */
static int usbh_composite_uac_ep_buf_ctrl_init(usbh_composite_uac_buf_ctrl_t *buf_ctrl, usbh_pipe_t *pipe)
{
	int ret = HAL_ERR_MEM;
	u8 buf_list_cnt;

	buf_list_cnt = buf_ctrl->frame_cnt;
	buf_ctrl->mps = pipe->ep_mps;

	if (buf_list_cnt == 0 || buf_ctrl->mps == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Err param: cnt=%d, mps=%d\n", buf_list_cnt, buf_ctrl->mps);
		return ret;
	}

	usb_ringbuf_manager_init(&(buf_ctrl->buf_manager), buf_list_cnt, buf_ctrl->mps, 1);

	if (usb_os_sema_create(&(buf_ctrl->isoc_sema)) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "isoc sema create fail\n");
		buf_ctrl->isoc_sema = NULL;
		buf_ctrl->sema_valid = 0;
		return HAL_ERR_MEM;
	}
	buf_ctrl->sema_valid = 1;

	return HAL_OK;
}

/**
  * @brief  Wait for an isochronous buffer to become available and check USB transfer status.
  * @param  pdata_ctrl: Pointer to the USB UAC buffer control structure.
  * @param  timeout_ms:  Time out
  * @retval Status
  */
static int usbh_composite_uac_wait_isoc_with_status_check(usbh_composite_uac_buf_ctrl_t *pdata_ctrl, uint32_t timeout_ms)
{
	int ret = HAL_ERR_PARA;
	u32 elapsed = 0;
	u32 wait_time = 0;

	while (elapsed < timeout_ms) {
		if (usbh_composite_uac_usb_status_check() != HAL_OK) {
			pdata_ctrl->wait_sema = 0;
			return ret;
		}

		wait_time = (timeout_ms - elapsed > USBH_UAC_WAIT_SLICE_MS) ? USBH_UAC_WAIT_SLICE_MS : (timeout_ms - elapsed);

		pdata_ctrl->wait_sema = 1;
		if (usb_os_sema_take(pdata_ctrl->isoc_sema, wait_time) == HAL_OK) {
			pdata_ctrl->wait_sema = 0;
			if (!pdata_ctrl->sema_valid) {
				return HAL_ERR_PARA;
			}
			return HAL_OK;
		}

		elapsed += wait_time;
	}

	pdata_ctrl->wait_sema = 0;
	return ret;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize the UAC class driver: allocate DMA-safe buffers, create synchronization
  *         primitives, set ring buffer depths from the callback structure, and invoke cb->init().
  * @param  driver: Pointer to the parent composite host handle.
  * @param  cb:     Pointer to the user callback structure; may be NULL for defaults.
  * @retval HAL_OK on success, HAL_ERR_MEM if a buffer allocation fails.
  */
int usbh_composite_uac_init(usbh_composite_host_t *driver, usbh_composite_uac_usr_cb_t *cb)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	int ret;

	if ((driver == NULL) || (cb == NULL) || ((cb->isoc_out_frm_cnt == 0) && (cb->isoc_in_frm_cnt == 0))) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Param err\n");
		return HAL_ERR_PARA;
	}

	usb_os_memset(uac, 0x00, sizeof(usbh_composite_uac_t));
	uac->driver = driver;

	uac->audio_ctrl_buf = (u8 *)usb_os_malloc(USBH_UAC_AUDIO_CTRL_BUF_MAX_LEN);
	if (NULL == uac->audio_ctrl_buf) {
		return HAL_ERR_MEM;
	}

	uac->isoc_in.xfer_buf = (u8 *)usb_os_malloc(USBH_UAC_ISOC_BUF_LENGTH);
	if (NULL == uac->isoc_in.xfer_buf) {
		goto get_rx_buf_fail;
	}

	uac->isoc_out.xfer_buf = (u8 *)usb_os_malloc(USBH_UAC_ISOC_BUF_LENGTH);
	if (NULL == uac->isoc_out.xfer_buf) {
		goto get_tx_buf_fail;
	}

	uac->ringbuf_partial_write_buf = (u8 *)usb_os_malloc(USBH_UAC_ISOC_BUF_LENGTH);
	if (NULL == uac->ringbuf_partial_write_buf) {
		goto get_wd_buf_fail;
	}

	if (usb_os_lock_create(&uac->alt_set_mutex) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "alt_set_mutex create fail\n");
		uac->alt_set_mutex = NULL;
		goto cb_init_fail;
	}
	if (usb_os_sema_create(&uac->ctrl_done_sema) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ctrl_done_sema create fail\n");
		uac->ctrl_done_sema = NULL;
		goto cb_init_fail;
	}

	uac->isoc_out.buf_ctrl.frame_cnt = cb->isoc_out_frm_cnt;
	uac->isoc_in.buf_ctrl.frame_cnt = cb->isoc_in_frm_cnt;

	if (cb->isoc_out_frm_cnt == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "OUT disabled by config\n");
	}
	if (cb->isoc_in_frm_cnt == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "IN disabled by config\n");
	}

	uac->cb = cb;
	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC init fail\n");
			goto cb_init_fail;
		}
	}

#if USBH_COMPOSITE_HID_UAC_DEBUG
	if (rtos_task_create(&(uac->dump_status_task), ((const char *)"usbh_composite_uac_status_dump_thread"), usbh_composite_uac_status_dump_thread, NULL, 768,
						 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Dump task create fail\n");
	}
#endif

	return HAL_OK;

cb_init_fail:
	if (uac->alt_set_mutex != NULL) {
		usb_os_lock_delete(uac->alt_set_mutex);
		uac->alt_set_mutex = NULL;
	}
	if (uac->ctrl_done_sema != NULL) {
		usb_os_sema_delete(uac->ctrl_done_sema);
		uac->ctrl_done_sema = NULL;
	}
	usb_os_mfree(uac->ringbuf_partial_write_buf);
	uac->ringbuf_partial_write_buf = NULL;

get_wd_buf_fail:
	usb_os_mfree(uac->isoc_out.xfer_buf);
	uac->isoc_out.xfer_buf = NULL;

get_tx_buf_fail:
	usb_os_mfree(uac->isoc_in.xfer_buf);
	uac->isoc_in.xfer_buf = NULL;

get_rx_buf_fail:
	usb_os_mfree(uac->audio_ctrl_buf);
	uac->audio_ctrl_buf = NULL;

	return HAL_ERR_MEM;
}

/**
  * @brief  De-initialize the UAC class driver: stop all transfers, invoke cb->deinit(),
  *         close isochronous pipes, destroy ring buffers, and free all allocated memory.
  * @retval HAL_OK.
  */
int usbh_composite_uac_deinit(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;

	usbh_composite_uac_stop_capture();
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
	usbh_composite_uac_channel_deinit(&(uac->isoc_out));
	usbh_composite_uac_channel_deinit(&(uac->isoc_in));

	if (uac->audio_ctrl_buf != NULL) {
		usb_os_mfree(uac->audio_ctrl_buf);
		uac->audio_ctrl_buf = NULL;
	}

	if (uac->ringbuf_partial_write_buf != NULL) {
		usb_os_mfree(uac->ringbuf_partial_write_buf);
		uac->ringbuf_partial_write_buf = NULL;
	}

	if (uac->alt_set_mutex != NULL) {
		usb_os_lock_delete(uac->alt_set_mutex);
		uac->alt_set_mutex = NULL;
	}
	if (uac->ctrl_done_sema != NULL) {
		usb_os_sema_delete(uac->ctrl_done_sema);
		uac->ctrl_done_sema = NULL;
	}

	return HAL_OK;
}

/**
  * @brief  Find the alternate setting matching the requested format, open the isochronous pipe,
  *         initialize the ring buffer, and send SET_INTERFACE + SET_CUR(frequency) to the device.
  *         Blocks until the control sequence completes or times out (1000 ms).
  * @param  dir:           Direction (USBH_UAC_ISOC_OUT_DIR = Playback, USBH_UAC_ISOC_IN_DIR = Record).
  * @param  channels:      Requested channel count.
  * @param  bit_width:     Requested bit depth per sample.
  * @param  sampling_freq: Requested sampling frequency in Hz.
  * @retval HAL_OK on success, HAL_ERR_PARA if no matching format is found, HAL_TIMEOUT on timeout.
  */
int usbh_composite_uac_set_alt_setting(u8 dir, u8 channels, u8 bit_width, u32 sampling_freq)
{
	usbh_composite_uac_format_cfg_t *fmt;
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_buf_ctrl_t *pdata_ctrl = NULL;
	usbh_composite_uac_as_itf_info_t *as_itf = NULL;
	usbh_pipe_t *pipe = NULL;
	usbh_ep_desc_t *ep_desc = NULL;
	usb_host_t *host = uac->driver->host;
	int ret = HAL_ERR_PARA;
	int set_flag = 0;
	int i, j;
	u8 alt_num;

	as_itf = usbh_composite_uac_get_as_itf_instance(dir);
	if (as_itf == NULL) {
		return HAL_ERR_PARA;
	}

	usb_os_lock(uac->alt_set_mutex);

	if (dir == USBH_UAC_ISOC_OUT_DIR) {
		pdata_ctrl = &(uac->isoc_out.buf_ctrl);
	} else {
		pdata_ctrl = &(uac->isoc_in.buf_ctrl);
	}

	alt_num = as_itf->alt_setting_cnt;

	//actually search from alt 1
	for (i = 0; i < alt_num; i++) {
		fmt = &(as_itf->interface_array[i].format_info);

		// Check format type, channels, and sample frequency
		if (!fmt || fmt->bit_width != bit_width || fmt->channels != channels) {
			continue;
		}

		// Check sample frequency
		for (j = 0; j < fmt->freq_cnt; j++) {
			if (fmt->freq[j] == sampling_freq) {
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
		ep_desc = &(as_itf->interface_array[as_itf->choose_alt_idx].ep_desc);

		/* full speed*/
		if (ep_desc->bInterval == 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid bInterval=0\n");
			usb_os_unlock(uac->alt_set_mutex);
			return HAL_ERR_PARA;
		}
		pdata_ctrl->packet_rate = USBH_UAC_ONE_KHZ >> (ep_desc->bInterval - 1);
		pdata_ctrl->sample_rem = sampling_freq % pdata_ctrl->packet_rate;
		//calculate accurate one frame size(byte)
		as_itf->packet_size_small = channels * bit_width / USBH_UAC_BIT_TO_BYTE * (sampling_freq / pdata_ctrl->packet_rate);
		as_itf->packet_size_large = channels * bit_width / USBH_UAC_BIT_TO_BYTE * ((sampling_freq + (pdata_ctrl->packet_rate - 1)) / pdata_ctrl->packet_rate);

		if (dir == USBH_UAC_ISOC_OUT_DIR) {
			usbh_composite_uac_stop_play();
		} else {
			usbh_composite_uac_stop_capture();
		}

		//reinit pipe
		usbh_composite_uac_deinit_pipe(dir);
		usbh_open_pipe(host, pipe, ep_desc);

		if (dir == USBH_UAC_ISOC_OUT_DIR) {
			usbh_composite_uac_ep_buf_ctrl_deinit(&(uac->isoc_out.buf_ctrl));
			ret = usbh_composite_uac_ep_buf_ctrl_init(&(uac->isoc_out.buf_ctrl), pipe);
		} else {
			usbh_composite_uac_ep_buf_ctrl_deinit(&(uac->isoc_in.buf_ctrl));
			ret = usbh_composite_uac_ep_buf_ctrl_init(&(uac->isoc_in.buf_ctrl), pipe);
		}

		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Buf init fail\n");
			usbh_composite_uac_deinit_pipe(dir);
			usb_os_unlock(uac->alt_set_mutex);
			return ret;
		}

		uac->cur_dir = dir;
		uac->ctrl_status = HAL_BUSY;
		uac->xfer_state = UAC_STATE_TRANSFER;
		uac->ctrl_state = UAC_STATE_SET_ALT_SETTING;
		uac->ctrl_waiting = 1;

		usbh_notify_composite_class_state_change(host, 0, USBH_COMPOSITE_UAC_EVENT);

		if (usb_os_sema_take(uac->ctrl_done_sema, 1000) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set alt timeout\n");
			uac->ctrl_waiting = 0;
			uac->ctrl_state = UAC_STATE_CTRL_IDLE;
			ret = HAL_TIMEOUT;
		} else {
			uac->ctrl_waiting = 0;
			/* read the real outcome reported by ctrl_finish() without this the
			 * caller would see HAL_OK even when SET_ALT or SET_FREQ failed. */
			ret = uac->ctrl_status;
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alt not match %d %d %d\n", channels, bit_width, sampling_freq);
		ret = HAL_ERR_PARA;
	}

	usb_os_unlock(uac->alt_set_mutex);
	return ret;
}

/**
  * @brief  Get alt setting structure and alt settings num for an interface.
  * @param  dir: Dir of the audio interface, 0 means out interface, 1 means in interface
  * @param  fmt_cnt: Number of audio format
  * @retval Pointer to audio format array
  */
const usbh_composite_uac_audio_fmt_t *usbh_composite_uac_get_alt_setting(u8 dir, u8 *fmt_cnt)
{
	usbh_composite_uac_as_itf_info_t *as_itf = NULL;

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
  * @param  dir: Dir of the audio interface, USBH_UAC_ISOC_OUT_DIR or USBH_UAC_ISOC_IN_DIR
  * @retval frame size
  */
u32 usbh_composite_uac_get_frame_size(u8 dir)
{
	usbh_composite_uac_as_itf_info_t *as_itf = NULL;
	as_itf = usbh_composite_uac_get_as_itf_instance(dir);
	if (as_itf == NULL) {
		return 0;
	}

	return as_itf->packet_size_large;
}

/**
  * @brief  Write PCM audio data to the playback ring buffer for isochronous OUT transmission.
  *         If the buffer is full and timeout_ms > 0, blocks until space is available or the
  *         timeout expires; if timeout_ms == 0, returns immediately when the buffer is full.
  * @param  buffer:     Pointer to the PCM data to transmit.
  * @param  size:       Number of bytes to write.
  * @param  timeout_ms: Maximum time to wait for buffer space, in milliseconds; 0 for non-blocking.
  * @retval Actual number of bytes written.
  */
u32 usbh_composite_uac_write(u8 *buffer, u32 size, u32 timeout_ms)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_buf_ctrl_t *pdata_ctrl = &(uac->isoc_out.buf_ctrl);
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
			if (usb_ringbuf_is_full(&(pdata_ctrl->buf_manager)) || last_zero) {
				need_wait = 1;
			}
		} else {
			if (usb_ringbuf_is_full(&(pdata_ctrl->buf_manager)) || last_zero) {
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

		if (just_written > 0) {
			written_len += just_written;
			last_zero = 0;
		} else {
			//wait sema and retry
			last_zero = 1;
		}
	}

	return written_len;
}

/**
  * @brief  Read PCM audio data from the recording ring buffer captured by isochronous IN transfers.
  *         If the buffer is empty and time_out_ms > 0, blocks until data arrives or the timeout
  *         expires; if time_out_ms == 0, returns immediately when the buffer is empty.
  * @param  buffer:      Pointer to the destination buffer for received PCM data.
  * @param  size:        Capacity of the destination buffer in bytes.
  * @param  time_out_ms: Maximum time to wait for data, in milliseconds; 0 for non-blocking.
  * @retval Actual number of bytes read.
  */
u32 usbh_composite_uac_read(u8 *buffer, u32 size, u32 time_out_ms)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_in.buf_ctrl);
	u32 zero_pkt_flag = 0;
	u32 copy_len = 0;
	u16 pkt_cnt = 0;

	if (uac->isoc_in.as_itf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No ISOC IN interface\n");
		return 0;
	}

	if (buf_ctrl->next_xfer == 0) {
		return 0;
	}

	if (time_out_ms == 0) {
		if (usb_ringbuf_is_empty(&(buf_ctrl->buf_manager))) {
			return 0;
		}
		usbh_composite_uac_read_ring_buf(buf_ctrl, buffer, size, &copy_len, &pkt_cnt, &zero_pkt_flag);
	} else {
		do {
			if (usb_ringbuf_is_empty(&(buf_ctrl->buf_manager))) {
				//wait sema
				buf_ctrl->wait_sema = 1;
				if (usb_os_sema_take(buf_ctrl->isoc_sema, time_out_ms) != HAL_OK) {
					buf_ctrl->wait_sema = 0;
					break;
				}
				buf_ctrl->wait_sema = 0;
				/* If deinit started while we waited, do not touch resources */
				if (!buf_ctrl->sema_valid) {
					break;
				}
			} else {
				/* if did not read any pkt, loop to check the wr/rd pos*/
				if ((usbh_composite_uac_read_ring_buf(buf_ctrl, buffer, size, &copy_len, &pkt_cnt, NULL) == 0)) {  //|| (copy_len >0)
					break;
				}
			}
		} while (buf_ctrl->next_xfer);

		/* Defensive clear: covers the next_xfer==0 exit and any future code path
		 * that could leave wait_sema set, so stop_capture won't give the
		 * semaphore unnecessarily. */
		buf_ctrl->wait_sema = 0;
	}

	return copy_len;
}

/**
  * @brief  Start UAC device play
  * @param  void
  * @retval void
  */
void usbh_composite_uac_start_play(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out.buf_ctrl);
	usbh_pipe_t *pipe;

	if (uac->isoc_out.as_itf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No ISOC OUT interface\n");
		return;
	}
	pipe = &(uac->isoc_out.as_itf->pipe);

#if USBH_COMPOSITE_HID_UAC_DEBUG
	usbh_composite_uac_reset_test_cnt();
	usbh_composite_uac_reset_isr_time();
#endif

	buf_ctrl->next_xfer = 1;
	uac->xfer_state = UAC_STATE_TRANSFER;
	pipe->xfer_state = USBH_EP_XFER_START;
	usbh_notify_composite_class_state_change(uac->driver->host, pipe->pipe_num, USBH_COMPOSITE_UAC_EVENT);
}

/**
  * @brief  Stop isochronous OUT (playback) transfers by clearing the next_xfer flag.
  * @retval void
  */
void usbh_composite_uac_stop_play(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out.buf_ctrl);

	buf_ctrl->next_xfer = 0;

	if (buf_ctrl->sema_valid && buf_ctrl->wait_sema) {
		usb_os_sema_give(buf_ctrl->isoc_sema);
	}

	if (uac->isoc_out.as_itf != NULL) {
		uac->isoc_out.as_itf->pipe.xfer_state = USBH_EP_XFER_IDLE;
	}

#if USBH_COMPOSITE_HID_UAC_DEBUG
	usbh_composite_uac_reset_test_cnt();
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC stop playback\n");
#endif
}

/**
  * @brief  Start isochronous IN (recording) transfers.
  *         Sets next_xfer, puts the state machine into TRANSFER state, and notifies the USB core.
  * @retval void
  */
void usbh_composite_uac_start_capture(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_in.buf_ctrl);
	usbh_pipe_t *pipe;

	if (uac->isoc_in.as_itf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No ISOC IN interface\n");
		return;
	}
	pipe = &(uac->isoc_in.as_itf->pipe);

#if USBH_COMPOSITE_HID_UAC_DEBUG
	usbh_composite_uac_reset_test_cnt();
	usbh_composite_uac_reset_isr_time();
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC start capture\n");
#endif

	buf_ctrl->next_xfer = 1;
	uac->xfer_state = UAC_STATE_TRANSFER;
	pipe->xfer_state = USBH_EP_XFER_START;
	usbh_notify_composite_class_state_change(uac->driver->host, pipe->pipe_num, USBH_COMPOSITE_UAC_EVENT);
}

/**
  * @brief  Stop isochronous IN (recording) transfers by clearing the next_xfer flag.
  * @retval void
  */
void usbh_composite_uac_stop_capture(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_in.buf_ctrl);

	buf_ctrl->next_xfer = 0;

	if (buf_ctrl->sema_valid && buf_ctrl->wait_sema) {
		usb_os_sema_give(buf_ctrl->isoc_sema);
	}

	if (uac->isoc_in.as_itf != NULL) {
		uac->isoc_in.as_itf->pipe.xfer_state = USBH_EP_XFER_IDLE;
	}

#if USBH_COMPOSITE_HID_UAC_DEBUG
	usbh_composite_uac_reset_test_cnt();
	RTK_LOGS(TAG, RTK_LOG_DEBUG, "UAC stop capture\n");
#endif
}

/**
  * @brief  Validate the direction argument and ensure the corresponding Audio
  *         Control Feature Unit was discovered during attach. Without a valid
  *         FU index, downstream process_set_* process_get_* helpers would
  *         dereference fu_controls[(u8)-1].
  * @param  dir: Direction to validate.
  * @retval HAL_OK if usable, HAL_ERR_PARA otherwise.
  */
static int usbh_composite_uac_check_dir(u8 dir)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	int ret = HAL_OK;

	if ((dir != USBH_UAC_ISOC_OUT_DIR) && (dir != USBH_UAC_ISOC_IN_DIR)) {
		ret = HAL_ERR_PARA;
	} else if ((dir == USBH_UAC_ISOC_OUT_DIR) && (uac->ac_isoc_desc.out_best_idx == (u8) - 1)) {
		ret = HAL_ERR_PARA;
	} else if ((dir == USBH_UAC_ISOC_IN_DIR) && (uac->ac_isoc_desc.in_best_idx == (u8) - 1)) {
		ret = HAL_ERR_PARA;
	} else {
		/* Direction is valid and a Feature Unit was found */
	}

	return ret;
}

/**
  * @brief  Verify the active Feature Unit advertises a given control bit on at
  *         least the master channel. Used by set/get_volume/mute to fail fast
  *         when the FU does not support the requested control instead of
  *         silently returning HAL_OK without ever issuing a transfer.
  * @param  dir:         Direction to check.
  * @param  control_bit: USB_UAC1_CONTROL_VOLUME or USB_UAC1_CONTROL_MUTE.
  * @retval HAL_OK if supported, HAL_ERR_PARA otherwise.
  */
static int usbh_composite_uac_check_capability(u8 dir, u16 control_bit)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_ac_itf_info_t *ac = &uac->ac_isoc_desc;
	usbh_composite_uac_fu_info_t *fu;
	u8 idx = (dir == USBH_UAC_ISOC_OUT_DIR) ? ac->out_best_idx : ac->in_best_idx;
	u8 i;

	if (idx == (u8) - 1) {
		return HAL_ERR_PARA;
	}
	fu = &ac->fu_controls[idx];
	for (i = 0; i <= fu->num_channels; i++) {
		if (fu->bma_controls[i] & control_bit) {
			return HAL_OK;
		}
	}
	return HAL_ERR_PARA;
}

/**
  * @brief  Common dispatch tail shared by set_volume / set_mute / get_mute /
  *         get_volume. Caller must hold alt_set_mutex and have already set
  *         ctrl_state plus any state-specific input fields (cur_dir, get_ch,
  *         volume_value, mute_value).
  *         Drains any stale give from a previous round, arms ctrl_waiting,
  *         notifies the USB task, then waits in slices so that a detach during
  *         the wait surfaces as HAL_ERR_UNKNOWN within ~slice ms instead of
  *         the full timeout.
  *         On timeout / disconnect ctrl_state is forced back to IDLE
  *         (ctrl_finish was never called); on success ctrl_state is already
  *         IDLE thanks to ctrl_finish.
  *         xfer_state is left at TRANSFER on return-restoring it would
  *         race with start_play() / start_capture() which do not hold
  *         alt_set_mutex.
  * @param  timeout_ms: Total time to wait for completion.
  * @retval Device's HAL result on completion, HAL_TIMEOUT, or HAL_ERR_UNKNOWN
  *         if the device disconnected (or the driver was torn down) during
  *         the wait.
  */
static int usbh_composite_uac_sync_dispatch(u32 timeout_ms)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	const u32 slice = 100;
	int ret = HAL_TIMEOUT;
	u32 elapsed = 0;

	/* deinit could have torn these down between status_check and us. */
	if ((uac->ctrl_done_sema == NULL) || (uac->driver == NULL) || (uac->driver->host == NULL)) {
		uac->ctrl_state = UAC_STATE_CTRL_IDLE;
		return HAL_ERR_UNKNOWN;
	}

	uac->ctrl_status = HAL_BUSY;
	uac->xfer_state = UAC_STATE_TRANSFER;
	usb_os_sema_take(uac->ctrl_done_sema, 0); /* drain stale give from prior round */
	uac->ctrl_waiting = 1;

	usbh_notify_composite_class_state_change(uac->driver->host, 0x00, USBH_COMPOSITE_UAC_EVENT);

	while (elapsed < timeout_ms) {
		u32 t = ((timeout_ms - elapsed) > slice) ? slice : (timeout_ms - elapsed);
		if (usb_os_sema_take(uac->ctrl_done_sema, t) == HAL_OK) {
			ret = uac->ctrl_status;
			break;
		}
		elapsed += t;
		if (usbh_composite_uac_usb_status_check() != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Sync dispatch aborted: device gone\n");
			ret = HAL_ERR_UNKNOWN;
			break;
		}
	}

	uac->ctrl_waiting = 0;
	if (ret != HAL_OK) {
		uac->ctrl_state = UAC_STATE_CTRL_IDLE;
	}
	return ret;
}

/**
  * @brief  Synchronously set the playback or recording volume.
  *         Iterates SET_CUR(VOLUME) over master + every individual channel of
  *         the active Feature Unit and blocks until the full sequence completes.
  *         The percentage is converted to device dB units inside
  *         process_set_ch_volume() using the cached vol_min/vol_max range.
  * @note   Values above 100 are silently clamped to 100; the device sees the
  *         clamped value, no error is returned.
  * @param  volume: Volume percentage, 0-100 (clamped if higher).
  * @param  dir:    USBH_UAC_ISOC_OUT_DIR for playback, USBH_UAC_ISOC_IN_DIR for recording.
  * @retval HAL_OK on success, HAL_BUSY if the USB connection is not ready,
  *         HAL_ERR_PARA on invalid direction or if the active Feature Unit
  *         does not advertise the Volume control,
  *         HAL_TIMEOUT if the device did not respond,
  *         HAL_ERR_UNKNOWN if the device disconnected mid-operation.
  */
int usbh_composite_uac_set_volume(u8 volume, u8 dir)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	int ret;

	if (usbh_composite_uac_check_dir(dir) != HAL_OK) {
		return HAL_ERR_PARA;
	}
	if (usbh_composite_uac_check_capability(dir, USB_UAC1_CONTROL_VOLUME) != HAL_OK) {
		return HAL_ERR_PARA;
	}
	if (usbh_composite_uac_usb_status_check() != HAL_OK) {
		return HAL_BUSY;
	}

	usb_os_lock(uac->alt_set_mutex);

	uac->volume_value = (volume > 100) ? (100) : (volume);
	uac->ch_idx = 0;
	uac->cur_dir = dir;
	uac->ctrl_state = UAC_STATE_SET_VOLUME;
	ret = usbh_composite_uac_sync_dispatch(1000);
	if (ret == HAL_TIMEOUT) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Set vol timeout\n");
	}

	usb_os_unlock(uac->alt_set_mutex);
	return ret;
}

/**
  * @brief  Synchronously set the playback or recording mute state.
  *         Iterates SET_CUR(MUTE) over master + every individual channel of
  *         the active Feature Unit and blocks until the full sequence completes.
  * @note   Any non-zero value is normalized to 1; no error is returned.
  * @param  mute: 0 = unmute, non-zero = mute.
  * @param  dir:  USBH_UAC_ISOC_OUT_DIR for playback, USBH_UAC_ISOC_IN_DIR for recording.
  * @retval HAL_OK on success, HAL_BUSY if the USB connection is not ready,
  *         HAL_ERR_PARA on invalid direction or if the active Feature Unit
  *         does not advertise the Mute control,
  *         HAL_TIMEOUT if the device did not respond,
  *         HAL_ERR_UNKNOWN if the device disconnected mid-operation.
  */
int usbh_composite_uac_set_mute(u8 mute, u8 dir)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	int ret;

	if (usbh_composite_uac_check_dir(dir) != HAL_OK) {
		return HAL_ERR_PARA;
	}
	if (usbh_composite_uac_check_capability(dir, USB_UAC1_CONTROL_MUTE) != HAL_OK) {
		return HAL_ERR_PARA;
	}
	if (usbh_composite_uac_usb_status_check() != HAL_OK) {
		return HAL_BUSY;
	}

	usb_os_lock(uac->alt_set_mutex);

	uac->mute_value = (mute == 0) ? (0) : (1);
	uac->ch_idx = 0;
	uac->cur_dir = dir;
	uac->ctrl_state = UAC_STATE_SET_MUTE;
	ret = usbh_composite_uac_sync_dispatch(1000);
	if (ret == HAL_TIMEOUT) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Set mute timeout\n");
	}

	usb_os_unlock(uac->alt_set_mutex);
	return ret;
}

/**
  * @brief  Initialization state machine that iterates over all channels of the best OUT and IN
  *         Feature Units to read their mute state, current volume, min volume, and max volume.
  *         Advances init_state from UAC_INIT_OUT_GET_FU -> UAC_INIT_IN_GET_FU -> UAC_INIT_OUT_SET_ITF.
  * @param  host: Pointer to the USB host handle.
  * @retval HAL_OK when the full sequence completes, HAL_BUSY while in progress, or an error code.
  */
int usbh_composite_uac_get_volume_info(usb_host_t *host)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	usbh_composite_uac_fu_info_t *info = NULL;
	usbh_composite_uac_as_itf_info_t *as_itf = NULL;
	int ret = HAL_BUSY;

	//loop usb out
	//loop usb in
	//1. get master
	//2. loop all channel
	switch (uac->init_state) {
	case UAC_INIT_OUT_GET_FU:
		if (ac_info->out_best_idx == (u8) - 1) {
			uac->init_state = UAC_INIT_IN_GET_FU;
			uac->ch_idx = 0;
			break;
		}
		info = &(ac_info->fu_controls[ac_info->out_best_idx]);
		if (uac->ch_idx <= info->num_channels) {
			ret = usbh_composite_uac_get_unit_ctrl(host, info->bma_controls[uac->ch_idx], uac->ch_idx, USBH_UAC_ISOC_OUT_DIR);
			if (ret == HAL_OK) {
				uac->ch_idx++;
				ret = HAL_BUSY;
				uac->ctrl_state = UAC_STATE_SCAN_MUTE;
			}
		} else {
			uac->init_state = UAC_INIT_IN_GET_FU;
			uac->ch_idx = 0;
		}
		break;

	case UAC_INIT_IN_GET_FU:
		if (ac_info->in_best_idx == (u8) - 1) {
			uac->init_state = UAC_INIT_OUT_SET_ITF;
			uac->ch_idx = 0;
			break;
		}
		info = &(ac_info->fu_controls[ac_info->in_best_idx]);
		if (uac->ch_idx <= info->num_channels) {
			ret = usbh_composite_uac_get_unit_ctrl(host, info->bma_controls[uac->ch_idx], uac->ch_idx, USBH_UAC_ISOC_IN_DIR);
			if (ret == HAL_OK) {
				uac->ch_idx++;
				ret = HAL_BUSY;
				uac->ctrl_state = UAC_STATE_SCAN_MUTE;
			}
		} else {
			uac->init_state = UAC_INIT_OUT_SET_ITF;
			uac->ch_idx = 0;
		}
		break;

	case UAC_INIT_OUT_SET_ITF:
		as_itf = uac->isoc_out.as_itf;
		if (as_itf && as_itf->alt_setting_cnt > 0) {
			/* Always send Alt 0 (zero-bandwidth)-interface_array[] only
			 * contains the non-zero-bandwidth alternates, so indexing it
			 * here would push a streaming alt to the device before the
			 * host is ready to receive isoc traffic. */
			ret = usbh_ctrl_set_interface(host, as_itf->as_itf_num, 0);
			if (ret == HAL_OK) {
				ret = HAL_BUSY;
				uac->init_state = UAC_INIT_IN_SET_ITF;
			}
		} else {
			ret = HAL_BUSY;
			uac->init_state = UAC_INIT_IN_SET_ITF;
		}
		break;

	case UAC_INIT_IN_SET_ITF:
		as_itf = uac->isoc_in.as_itf;
		if (as_itf && as_itf->alt_setting_cnt > 0) {
			ret = usbh_ctrl_set_interface(host, as_itf->as_itf_num, 0);
			if (ret == HAL_OK) {
				ret = HAL_BUSY;
				uac->init_state = UAC_INIT_DONE;
			}
		} else {
			ret = HAL_BUSY;
			uac->init_state = UAC_INIT_DONE;
		}
		break;

	case UAC_INIT_DONE:
	default:
		ret = HAL_OK;
		break;
	}

	return ret;
}

/**
  * @brief  Get the current UAC mode based on detected interfaces.
  * @return usbh_composite_uac_mode_t indicating the current mode:
  *         - USBH_UAC_MODE_NONE: No UAC interface detected
  *         - USBH_UAC_MODE_PLAYBACK_ONLY: Playback only (Speaker)
  *         - USBH_UAC_MODE_RECORD_ONLY: Record only (Microphone)
  *         - USBH_UAC_MODE_PLAYBACK_AND_RECORD: Both Playback and Record
  */
usbh_composite_uac_mode_t usbh_composite_uac_get_mode(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	u8 has_playback = 0;
	u8 has_record = 0;

	/* Check if playback (ISOC OUT) interface exists */
	if (uac->isoc_out.as_itf != NULL) {
		has_playback = 1;
	}

	/* Check if record (ISOC IN) interface exists */
	if (uac->isoc_in.as_itf != NULL) {
		has_record = 1;
	}

	/* Determine UAC mode based on available interfaces */
	if (has_playback && has_record) {
		return USBH_UAC_MODE_PLAYBACK_AND_RECORD;
	} else if (has_playback) {
		return USBH_UAC_MODE_PLAYBACK_ONLY;
	} else if (has_record) {
		return USBH_UAC_MODE_RECORD_ONLY;
	} else {
		return USBH_UAC_MODE_NONE;
	}
}

/**
  * @brief  Check if playback is usable.
  *         Returns true only if the device exposes an OUT streaming interface
  *         AND the application reserved a non-zero ring-buffer depth via
  *         isoc_out_frm_cnt. The frame_cnt gate makes the answer match what
  *         set_alt_setting() can actually accomplish.
  * @return 1 if playback is supported and enabled, 0 otherwise.
  */
u8 usbh_composite_uac_support_playback(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	return ((uac->isoc_out.as_itf != NULL) && (uac->isoc_out.buf_ctrl.frame_cnt > 0)) ? 1 : 0;
}

/**
  * @brief  Check if recording is usable.
  *         Returns true only if the device exposes an IN streaming interface
  *         AND the application reserved a non-zero ring-buffer depth via
  *         isoc_in_frm_cnt. The frame_cnt gate makes the answer match what
  *         set_alt_setting() can actually accomplish.
  * @return 1 if recording is supported and enabled, 0 otherwise.
  */
u8 usbh_composite_uac_support_record(void)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	return ((uac->isoc_in.as_itf != NULL) && (uac->isoc_in.buf_ctrl.frame_cnt > 0)) ? 1 : 0;
}

/**
  * @brief  Synchronously read the current mute state from the device.
  *         Issues a UAC1 GET_CUR(MUTE) on the selected channel and blocks until
  *         the response arrives or the operation times out.
  * @param  dir:  USBH_UAC_ISOC_OUT_DIR or USBH_UAC_ISOC_IN_DIR.
  * @param  ch:   Channel index (0 = master, 1..N = individual channels).
  * @param  mute: Output pointer that receives the device's mute state.
  * @return HAL_OK on success, HAL_BUSY if not ready,
  *         HAL_TIMEOUT if the device did not respond,
  *         HAL_ERR_PARA on invalid direction/channel or if the active Feature
  *         Unit does not advertise the Mute control on this channel,
  *         HAL_ERR_UNKNOWN if the device disconnected mid-operation.
  */
int usbh_composite_uac_get_mute(u8 dir, u8 ch, u8 *mute)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	usbh_composite_uac_fu_info_t *info;
	int ret;

	if ((mute == NULL) || (ch > USBH_UAC_MAX_CHANNEL)) {
		return HAL_ERR_PARA;
	}
	if (usbh_composite_uac_check_dir(dir) != HAL_OK) {
		return HAL_ERR_PARA;
	}

	info = &(ac_info->fu_controls[(dir == USBH_UAC_ISOC_IN_DIR) ? ac_info->in_best_idx : ac_info->out_best_idx]);
	if (ch > info->num_channels || !(info->bma_controls[ch] & USB_UAC1_CONTROL_MUTE)) {
		return HAL_ERR_PARA;
	}

	if (usbh_composite_uac_usb_status_check() != HAL_OK) {
		return HAL_BUSY;
	}

	usb_os_lock(uac->alt_set_mutex);

	uac->cur_dir = dir;
	uac->get_ch = ch;
	uac->ctrl_state = UAC_STATE_GET_MUTE;
	ret = usbh_composite_uac_sync_dispatch(1000);
	if (ret == HAL_OK) {
		*mute = uac->get_mute_result;
	} else if (ret == HAL_TIMEOUT) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Query mute timeout\n");
	}

	usb_os_unlock(uac->alt_set_mutex);
	return ret;
}

/**
  * @brief  Synchronously read the current volume from the device.
  *         Issues a UAC1 GET_CUR(VOLUME) on the selected channel and blocks until
  *         the response arrives or the operation times out. The min/max range is
  *         taken from the cache populated at attach time; if the attach-time
  *         range query failed for this channel, *vol_min and *vol_max are
  *         filled with 0 to avoid exposing uninitialized cache values.
  * @param  dir:     USBH_UAC_ISOC_OUT_DIR or USBH_UAC_ISOC_IN_DIR.
  * @param  ch:      Channel index (0 = master, 1..N = individual channels).
  * @param  volume:  Output pointer that receives the current volume in raw dB units.
  * @param  vol_min: Output pointer for the cached minimum volume; may be NULL.
  * @param  vol_max: Output pointer for the cached maximum volume; may be NULL.
  * @return HAL_OK on success, HAL_BUSY if not ready,
  *         HAL_TIMEOUT if the device did not respond,
  *         HAL_ERR_PARA on invalid direction/channel or if the active Feature
  *         Unit does not advertise the Volume control on this channel,
  *         HAL_ERR_UNKNOWN if the device disconnected mid-operation.
  */
int usbh_composite_uac_get_volume(u8 dir, u8 ch, s16 *volume, s16 *vol_min, s16 *vol_max)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	usbh_composite_uac_fu_info_t *info;
	usbh_composite_uac_volume_info_t *vi;
	int ret;

	if ((volume == NULL) || (ch > USBH_UAC_MAX_CHANNEL)) {
		return HAL_ERR_PARA;
	}
	if (usbh_composite_uac_check_dir(dir) != HAL_OK) {
		return HAL_ERR_PARA;
	}

	info = &(ac_info->fu_controls[(dir == USBH_UAC_ISOC_IN_DIR) ? ac_info->in_best_idx : ac_info->out_best_idx]);
	if (ch > info->num_channels || !(info->bma_controls[ch] & USB_UAC1_CONTROL_VOLUME)) {
		return HAL_ERR_PARA;
	}

	if (usbh_composite_uac_usb_status_check() != HAL_OK) {
		return HAL_BUSY;
	}

	usb_os_lock(uac->alt_set_mutex);

	uac->cur_dir = dir;
	uac->get_ch = ch;
	uac->ctrl_state = UAC_STATE_GET_VOLUME;
	ret = usbh_composite_uac_sync_dispatch(1000);
	if (ret == HAL_OK) {
		/* ctrl_setting()'s GET_VOLUME case already wrote vi->volume,
		 * so we just expose the result and the cached min/max range. */
		*volume = uac->get_volume_result;
		vi = (dir == USBH_UAC_ISOC_OUT_DIR) ? &(uac->isoc_out.volume_info[ch])
			 : &(uac->isoc_in.volume_info[ch]);
		/* attach-time GET_VOLUME_MIN/MAX may have failed for this channel; in
		 * that case vol_min/vol_max are uninitialized garbage. Only expose them
		 * to the caller when the init scan confirmed a valid range. */
		if (vi->range_valid) {
			if (vol_min) {
				*vol_min = vi->vol_min;
			}
			if (vol_max) {
				*vol_max = vi->vol_max;
			}
		} else {
			if (vol_min) {
				*vol_min = 0;
			}
			if (vol_max) {
				*vol_max = 0;
			}
		}
	} else if (ret == HAL_TIMEOUT) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Query vol timeout\n");
	}

	usb_os_unlock(uac->alt_set_mutex);
	return ret;
}

/**
  * @brief  Get the parsed Feature Unit info for the active streaming
  *         direction. The returned struct exposes bma_controls[0..num_channels]
  *         (mirroring the UAC1 bmaControls layout: index 0 = master,
  *         indices 1..num_channels = per-channel), so callers can iterate
  *         uniformly and decide which channels to query/verify against
  *         any UAC1 control bit. set_volume() and set_mute() walk this
  *         same structure, so callers can reuse the iteration pattern to
  *         verify exactly what was written.
  * @param  dir: USBH_UAC_ISOC_OUT_DIR or USBH_UAC_ISOC_IN_DIR.
  * @retval Pointer to the FU info, or NULL if the device has no Feature
  *         Unit on this direction. Valid until the device is detached.
  */
const usbh_composite_uac_fu_info_t *usbh_composite_uac_get_volume_ctrl_info(u8 dir)
{
	usbh_composite_uac_t *uac = &usbh_composite_uac;
	usbh_composite_uac_ac_itf_info_t *ac = &(uac->ac_isoc_desc);
	u8 idx = (dir == USBH_UAC_ISOC_IN_DIR) ? ac->in_best_idx : ac->out_best_idx;

	if (idx == (u8) - 1) {
		return NULL;
	}
	return &(ac->fu_controls[idx]);
}
