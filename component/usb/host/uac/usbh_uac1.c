/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/
#include "usbh_uac1.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#define USBH_UAC_WAIT_SLICE_MS                      5

#define USBH_UAC_AUDIO_CTRL_BUF_MAX_LEN             512U
#define USBH_UAC_ISOC_BUF_LENGTH                    1024U

#define USBH_UAC_SAMPLING_FREQ_CONTROL              0x100

#define USBH_UAC_BIT_TO_BYTE                        8U
#define USBH_UAC_ONE_KHZ                            1000U
#define USBH_UAC_HS_MICROFRAMES_PER_MS              8U    /**< 1 ms = 8 HS microframes */
#define USBH_UAC_LEGACY_FS_SIZE_RATIO_MIN           4U    /**< wMaxPacketSize/compliant-size ratio that flags a legacy FS-style bInterval on a HS bus */

#define USBH_LE16(addr)                             (((u16)(addr)[0]) | ((u16)(((u32)(addr)[1]) << 8)))
#define USBH_UAC_FREQ(freq)                         (((u32)freq[0]) | (((u32)freq[1]) << 8) | (((u32)freq[2]) << 16))

/* Minimum bLength needed to safely read each AS class-specific descriptor's fields */
#define USBH_UAC_FORMAT_TYPE_I_FIXED_LEN            (8U)  /**< fixed fields before tSamFreq[] */
#define USBH_UAC_SAM_FREQ_ENTRY_SIZE                (3U)  /**< bytes per tSamFreq entry */
#define USBH_UAC_SAM_FREQ_CONTINUOUS                (0U)  /**< bSamFreqType == 0 : continuous range (tLower/tUpperSamFreq) */
#define USBH_UAC_SAM_FREQ_RANGE_CNT                 (2U)  /**< tLowerSamFreq + tUpperSamFreq entries */
#define USBH_UAC_CS_EP_FIXED_LEN                    (4U)  /**< enough to read bmAttributes */

#if USBH_UAC_DEBUG
#define USBH_UAC_DEBUG_LOOP_TIME      1000
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
static int usbh_uac_attach(usb_host_t *host);
static void usbh_uac_detach(usb_host_t *host);
static void usbh_uac_process(usb_host_t *host, usbh_drv_msg_t *msg);
static int usbh_uac_ctrl_setting(usb_host_t *host, u32 msg);
static int usbh_uac_setup(usb_host_t *host);
static void usbh_uac_sof(usb_host_t *host);
static void usbh_uac_completed(usb_host_t *host, u8 pipe);
static int usbh_uac_get_volume_info(usb_host_t *host);
static void usbh_uac_isoc_out_process_xfer(usb_host_t *host, u32 cur_frame);
static void usbh_uac_isoc_in_process_xfer(usb_host_t *host, u32 cur_frame);
static int usbh_uac_process_set_alt(usb_host_t *host);
static int usbh_uac_process_set_freq(usb_host_t *host);
static void usbh_uac_deinit_pipe(u8 dir);
static void usbh_uac_deinit_all_pipe(void);
static void usbh_uac_channel_deinit(usbh_uac_channel_t *ch);
static int usbh_uac_parse_interface_desc(usb_host_t *host);
static int usbh_uac_parse_ac(usbh_itf_data_t *ac_itf);
static int usbh_uac_parse_as(usbh_itf_data_t *as_itf);
static int usbh_uac_usb_status_check(void);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "UAC";

/* USB UAC device identification */
static const usbh_dev_id_t uac_devs[] = {
	{
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS | USBH_DEV_ID_MATCH_ITF_SUBCLASS,
		.bInterfaceClass = USB_UAC_CLASS_CODE,
		.bInterfaceSubClass = USB_UAC_SUBCLASS_AUDIOSTREAMING,
	},
	{
	},
};

/* USB Class Driver */
static const usbh_class_driver_t usbh_uac_driver = {
	.id_table = uac_devs,
	.attach = usbh_uac_attach,
	.detach = usbh_uac_detach,
	.setup = usbh_uac_setup,
	.process = usbh_uac_process,
	.sof = usbh_uac_sof,
	.completed = usbh_uac_completed,
};

static usbh_uac_t usbh_uac;

/**
  * @brief  Check whether the USB host connection is ready.
  * @retval HAL_OK if connected and set up, HAL_BUSY otherwise.
  */
static int usbh_uac_usb_status_check(void)
{
	usbh_uac_t *uac = &usbh_uac;

	if ((uac->host != NULL) && (uac->host->connect_state >= USBH_STATE_SETUP)) {
		return HAL_OK;
	}

	return HAL_BUSY;
}

#if USBH_UAC_DEBUG
/**
  * @brief  Print all alternate settings of an audio streaming interface for debugging.
  * @param  as_info: Pointer to the AS interface info structure.
  * @retval void
  */
static void usbh_uac_dump_as_desc(usbh_uac_as_itf_info_t *as_info)
{
	usbh_uac_as_itf_alt_info_t *as_alt_info = NULL;
	usbh_uac_format_cfg_t *fmt = NULL;
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
static void usbh_uac_status_dump(void)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_channel_t *uac_channel = NULL;
	usbh_uac_buf_ctrl_t *buf_ctrl = NULL;

	if (usbh_uac_usb_status_check() == HAL_OK) {
		if (uac->isoc_out.as_itf != NULL) {
			uac_channel = &(uac->isoc_out);
			buf_ctrl = &(uac_channel->buf_ctrl);
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "UAC TX:%d-%d-%d-%d/xfer=%d-%d-%d %d-%d-%d\n",
					 buf_ctrl->buf_manager.capacity, usb_ringbuf_get_count(&(buf_ctrl->buf_manager)),
					 buf_ctrl->next_xfer, uac_channel->as_itf->pipe.xfer_state,
					 uac->sof_cnt, buf_ctrl->xfer_start_cnt, buf_ctrl->xfer_done_cnt,
					 buf_ctrl->xfer_buf_empty_cnt, buf_ctrl->xfer_buf_err_cnt, buf_ctrl->xfer_interval_cnt);
		}
		if (uac->isoc_in.as_itf != NULL) {
			uac_channel = &(uac->isoc_in);
			buf_ctrl = &(uac_channel->buf_ctrl);
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "RX %d-%d-%d-%d/xfer=%d-%d-%d-%d %d-%d-%d\n",
					 buf_ctrl->buf_manager.capacity, usb_ringbuf_get_count(&(buf_ctrl->buf_manager)),
					 buf_ctrl->next_xfer, uac_channel->as_itf->pipe.xfer_state,
					 uac->sof_cnt, buf_ctrl->xfer_start_cnt, buf_ctrl->xfer_done_cnt, buf_ctrl->last_xfer_len,
					 buf_ctrl->xfer_buf_empty_cnt, buf_ctrl->xfer_buf_err_cnt, buf_ctrl->xfer_interval_cnt);
		}

#if USBH_TP_TRACE_DEBUG
		if (uac->host != NULL) {
			usb_host_t *host = uac->host;

			RTK_LOGS(NOTAG, RTK_LOG_INFO, "State %d-%d/IsrC %lld-%lld/isrT %lld-%lld\n\n",
					 uac->ctrl_state, uac->xfer_state,
					 host->isr_process_time_max, host->isr_process_time,
					 host->isr_enter_period_max, host->isr_enter_period);
		}
#endif

	}
}

/**
  * @brief  Periodic debug thread that calls status_dump every USBH_UAC_DEBUG_LOOP_TIME ms.
  * @param  param: Unused.
  * @retval void
  */
static void usbh_uac_status_dump_thread(void *param)
{
	UNUSED(param);
	usbh_uac_t *uac = &usbh_uac;

	uac->dump_status_task_alive = 1;
	uac->dump_status_task_exit = 1;

	while (uac->dump_status_task_exit) {

		usbh_uac_status_dump();

		rtos_time_delay_ms(USBH_UAC_DEBUG_LOOP_TIME);
	}

	uac->dump_status_task_alive = 0;
	rtos_task_delete(NULL);
}

/**
  * @brief  Reset host ISR processing time and entry period maximum values to zero.
  * @retval void
  */
static void usbh_uac_reset_isr_time(void)
{
#if USBH_TP_TRACE_DEBUG
	usbh_uac_t *uac = &usbh_uac;
	usb_host_t *host;

	if (uac && uac->host != NULL) {
		host = uac->host;

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
static void usbh_uac_reset_test_cnt(void)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_buf_ctrl_t *buf_ctrl_out = &(uac->isoc_out.buf_ctrl);
	usbh_uac_buf_ctrl_t *buf_ctrl_in = &(uac->isoc_in.buf_ctrl);

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
static inline u32 usbh_uac_frame_num_dec(u32 new, u32 start)
{
	if (new >= start) {
		return new - start;
	} else {
		return (USB_FRAME_NUM_MAX - start + 1 + new);
	}
}

/**
  * @brief  Increment a USB frame number by inc, wrapping at USB_FRAME_NUM_MAX.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `usb_os_sema_take`) are not permitted.
  * @param  frame: Base frame number.
  * @param  inc:   Amount to add.
  * @retval Incremented frame number.
  */
static inline u32 usbh_uac_frame_num_inc(u32 frame, u32 inc)
{
	return (frame + inc) & USB_FRAME_NUM_MASK;
}

/**
  * @brief  Convert a 0-100 percent volume to raw device dB units by linear interpolation.
  * @param  uac_dev: Pointer to the volume info structure carrying vol_min/vol_max.
  * @param  percent: Volume percentage (0-100).
  * @retval Raw dB value clamped to [vol_min, vol_max].
  */
static u16 usbh_uac_volume_to_db(usbh_uac_volume_info_t *uac_dev, u8 percent)
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
static void usbh_uac_dump_cfgdesc(void)
{
#if USBH_UAC_DEBUG
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_as_itf_info_t *as_info = NULL;
	RTK_LOGS(TAG, RTK_LOG_INFO, "--------------------AS Dump Start------------------------------\n");

	if (uac->isoc_out.as_itf) {
		as_info = uac->isoc_out.as_itf;
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB OUT at_cnt %d itf %d\n", as_info->alt_setting_cnt, as_info->as_itf_num);
		usbh_uac_dump_as_desc(as_info);
	}

	if (uac->isoc_in.as_itf) {
		as_info = uac->isoc_in.as_itf;
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB IN at_cnt %d itf %d\n", as_info->alt_setting_cnt, as_info->as_itf_num);
		usbh_uac_dump_as_desc(as_info);
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "---------------------AS Dump End-----------------------------\n");
#endif
}

/**
  * @brief  Return the AS interface info structure for the given direction.
  * @param  dir: USBH_UAC_ISOC_OUT_DIR for Playback, USBH_UAC_ISOC_IN_DIR for Record.
  * @retval Pointer to the AS interface info, or NULL if that direction is not present.
  */
static usbh_uac_as_itf_info_t *usbh_uac_get_as_itf_instance(u8 dir)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_as_itf_info_t *as_itf = NULL;

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
static void usbh_uac_get_audio_format(usbh_uac_as_itf_info_t *as_info)
{
	usbh_uac_as_itf_alt_info_t *pasintf = NULL;
	usbh_uac_format_cfg_t *fmt = NULL;
	usbh_uac_audio_fmt_t *pfmt_info = NULL;
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

	as_info->fmt_array = (usbh_uac_audio_fmt_t *)usb_os_malloc(sizeof(usbh_uac_audio_fmt_t) * fmt_cnt);
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
static u32 usbh_uac_get_audio_format_info(void)
{
	usbh_uac_t *uac = &usbh_uac;

	usbh_uac_get_audio_format(uac->isoc_out.as_itf);
	usbh_uac_get_audio_format(uac->isoc_in.as_itf);

	return HAL_OK;
}

/**
  * @brief  Close and clear the USB isochronous pipe for the specified direction.
  * @param  dir: USBH_UAC_ISOC_OUT_DIR or USBH_UAC_ISOC_IN_DIR.
  * @retval void
  */
static void usbh_uac_deinit_pipe(u8 dir)
{
	usbh_uac_t *uac = &usbh_uac;
	usb_host_t *host = uac->host;
	usbh_uac_as_itf_info_t *as_itf;
	usbh_pipe_t *pipe = NULL;

	as_itf = usbh_uac_get_as_itf_instance(dir);
	if (as_itf == NULL) {
		return ;
	}

	pipe = &(as_itf->pipe);
	if (pipe->pipe_num && host != NULL) {
		usbh_close_pipe(host, pipe);
		pipe->pipe_num = 0U;
	}
}

/**
  * @brief  Close and clear both OUT and IN isochronous pipes.
  * @retval void
  */
static void  usbh_uac_deinit_all_pipe(void)
{
	usbh_uac_deinit_pipe(USBH_UAC_ISOC_OUT_DIR);
	usbh_uac_deinit_pipe(USBH_UAC_ISOC_IN_DIR);
}

/**
  * @brief  Append a terminal descriptor entry to the AC interface list if space remains.
  * @param  list: Pointer to the AC interface info structure.
  * @param  term: Pointer to the terminal entry to copy.
  * @retval void
  */
static void usbh_uac_add_terminal(usbh_uac_ac_itf_info_t *list, const usbh_uac_term_info_t *term)
{
	if (list && term && list->terminal_count < USBH_UAC_TERM_MAX_CNT) {
		usb_os_memcpy((void *) & (list->terminals[list->terminal_count]), (const void *)term, sizeof(usbh_uac_term_info_t));
		list->terminal_count++;
	}
}

/**
  * @brief  Append a Feature Unit volume control entry to the AC interface list if space remains.
  * @param  list: Pointer to the AC interface info structure.
  * @param  info: Pointer to the Feature Unit entry to copy.
  * @retval void
  */
static void usbh_uac_add_vol_ctrl(usbh_uac_ac_itf_info_t *list, const usbh_uac_fu_info_t *info)
{
	if (list && info && list->volume_ctrl_count < USBH_UAC_FU_MAX_CNT) {
		usb_os_memcpy((void *) & (list->fu_controls[list->volume_ctrl_count]), (const void *)info, sizeof(usbh_uac_fu_info_t));
		list->volume_ctrl_count++;
	}
}

/**
  * @brief  Record one intermediate Unit (Mixer/Selector/Processing/Extension) and its bSourceID list.
  *         UAC1 4.3.2.3/4.3.2.4/4.3.2.5/4.3.2.6 all place bUnitID at offset 3; Mixer/Selector carry
  *         bNrInPins at offset 4 followed by that many bSourceID bytes, while Processing/Extension
  *         carry wProcessType/wExtensionCode at 4..5 and bNrInPins at 6.
  * @param  list:    Pointer to the AC interface info structure.
  * @param  desc:    Pointer to the Unit descriptor.
  * @param  len:     bLength of the Unit descriptor.
  * @param  subtype: bDescriptorSubtype of the Unit descriptor.
  * @retval void
  */
static void usbh_uac_add_unit(usbh_uac_ac_itf_info_t *list, const u8 *desc, u8 len, u8 subtype)
{
	usbh_uac_unit_info_t *unit;
	u8 pin_off;
	u8 nr_pins;
	u8 i;

	if ((list == NULL) || (list->unit_count >= USBH_UAC_UNIT_MAX_CNT)) {
		return;
	}

	if ((subtype == USB_UAC1_PROCESSING_UNIT) || (subtype == USB_UAC1_EXTENSION_UNIT)) {
		pin_off = 6U;
	} else {
		pin_off = 4U;
	}

	/* Need bUnitID, bNrInPins and at least one bSourceID to be of any use. */
	if (len < (pin_off + 2U)) {
		return;
	}

	nr_pins = desc[pin_off];
	if (nr_pins == 0U) {
		return;
	}

	/* Clamp against both the descriptor's own length and the tracked array. */
	if ((u16)nr_pins > (u16)(len - pin_off - 1U)) {
		nr_pins = (u8)(len - pin_off - 1U);
	}
	if (nr_pins > USBH_UAC_UNIT_SRC_MAX_CNT) {
		nr_pins = USBH_UAC_UNIT_SRC_MAX_CNT;
	}

	unit = &(list->units[list->unit_count]);
	unit->unit_id = desc[3];
	unit->source_cnt = nr_pins;
	for (i = 0; i < nr_pins; i++) {
		unit->source_ids[i] = desc[pin_off + 1U + i];
	}

	list->unit_count++;
}

/**
  * @brief  Walk the audio topology from one entity ID back towards the sources, looking for a
  *         Feature Unit. UAC1 3.13/4.3.2 allow units to be chained, so an Output Terminal may
  *         reach its Feature Unit through one or more intermediate Units.
  * @param  ac_info: Pointer to the AC interface info structure.
  * @param  entity_id: ID of the entity to start from (an Output Terminal's bSourceID).
  * @param  fu_id:   ID of the Feature Unit to look for.
  * @retval 1 when fu_id is reachable from entity_id, 0 otherwise.
  */
static u8 usbh_uac_topo_reaches_fu(const usbh_uac_ac_itf_info_t *ac_info, u8 entity_id, u8 fu_id)
{
	u8 pending[USBH_UAC_TOPO_DEPTH_MAX];
	u8 visited[USBH_UAC_TOPO_DEPTH_MAX];
	u8 pending_cnt = 0U;
	u8 visited_cnt = 0U;
	u8 cur;
	u8 i;
	u8 u;
	u8 seen;

	pending[pending_cnt++] = entity_id;

	/* Bounded breadth-first walk: every iteration pops one entity and the visited[]
	 * set caps the total work, so a descriptor with a cyclic bSourceID chain cannot
	 * make this loop run forever. */
	while (pending_cnt > 0U) {
		pending_cnt--;
		cur = pending[pending_cnt];

		if (cur == fu_id) {
			return 1U;
		}

		seen = 0U;
		for (i = 0; i < visited_cnt; i++) {
			if (visited[i] == cur) {
				seen = 1U;
				break;
			}
		}
		if (seen != 0U) {
			continue;
		}
		if (visited_cnt >= USBH_UAC_TOPO_DEPTH_MAX) {
			break;
		}
		visited[visited_cnt++] = cur;

		/* Not the Feature Unit: if it is a tracked intermediate Unit, queue its sources. */
		for (u = 0; u < ac_info->unit_count; u++) {
			if (ac_info->units[u].unit_id != cur) {
				continue;
			}
			for (i = 0; i < ac_info->units[u].source_cnt; i++) {
				if (pending_cnt >= USBH_UAC_TOPO_DEPTH_MAX) {
					break;
				}
				pending[pending_cnt++] = ac_info->units[u].source_ids[i];
			}
			break;
		}
	}

	return 0U;
}

/**
  * @brief  Scan all parsed Feature Units and select the highest-priority one for each direction.
  *         Priority is determined by terminal type (Headphones > Speaker > other for OUT;
  *         Microphone > Desktop Microphone > other for IN) and master channel support.
  *         Results are stored in ac_info->out_best_idx and ac_info->in_best_idx.
  * @retval HAL_OK.
  */
static int usbh_uac_find_best_ac(void)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);

#if USBH_UAC_DEBUG
	usbh_uac_fu_info_t *info = NULL;
#endif
	usbh_uac_fu_info_t *current;
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

#if USBH_UAC_DEBUG
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
static int usbh_uac_parse_ac(usbh_itf_data_t *itf_data)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
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
		} else if (type == USB_UAC_CS_INTERFACE) {
			len = ((usbh_desc_header_t *) desc)->bLength;
			ac_header = (usb_ac_itf_desc_header_t *)desc;
			subtype = ac_header->bDescriptorSubtype;

			if (subtype == USB_UAC_AC_INPUT_TERMINAL) {
				if (len >= 0x0C) {
					usbh_uac_term_info_t term = {
						.terminal_id = desc[3],
						.terminal_type = (desc[5] << 8) | desc[4],
						.is_input = 1
					};
					usbh_uac_add_terminal(ac_info, &term);
				}
			} else if (subtype == USB_UAC_AC_OUTPUT_TERMINAL) {
				if (len >= 0x09) {
					usbh_uac_term_info_t term = {
						.terminal_id = desc[3],
						.terminal_type = (desc[5] << 8) | desc[4],
						.source_id = desc[7],
						.is_input = 0
					};
					usbh_uac_add_terminal(ac_info, &term);
				}
			} else if ((subtype == USB_UAC1_MIXER_UNIT) || (subtype == USB_UAC1_SELECTOR_UNIT) ||
					   (subtype == USB_UAC1_PROCESSING_UNIT) || (subtype == USB_UAC1_EXTENSION_UNIT)) {
				/* Track the link structure so an Output Terminal reached through these can still
				 * be attributed to the Feature Unit behind them (UAC1 3.13/4.3.2). */
				usbh_uac_add_unit(ac_info, desc, len, subtype);
			} else {
				/* AC HEADER and any other subtype carry nothing this driver needs. */
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
		} else if (type == USB_UAC_CS_INTERFACE) {
			len = ((usbh_desc_header_t *) desc)->bLength;
			ac_header = (usb_ac_itf_desc_header_t *)desc;
			subtype = ac_header->bDescriptorSubtype;

			if (subtype == USB_UAC_AC_FEATURE_UNIT) {
				if (len >= 0x07) {
					usbh_uac_fu_info_t vol_info = {0};
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

					/* Prefer a direct connection, then fall back to a transitive walk through
					 * intermediate Units: UAC1 3.13/4.3.2 let an Output Terminal reach its
					 * Feature Unit via a Mixer/Selector/Processing/Extension chain, e.g.
					 * IT(USB streaming) -> FU -> Mixer -> OT(Speaker). */
					for (t = 0; t < ac_info->terminal_count; t++) {
						if (!ac_info->terminals[t].is_input &&
							ac_info->terminals[t].source_id == vol_info.unit_id) { // find output information

							vol_info.sink_id = ac_info->terminals[t].terminal_id;
							vol_info.sink_type = ac_info->terminals[t].terminal_type;
							break;
						}
					}

					if (vol_info.sink_type == 0U) {
						for (t = 0; t < ac_info->terminal_count; t++) {
							if (ac_info->terminals[t].is_input) {
								continue;
							}
							if (usbh_uac_topo_reaches_fu(ac_info, ac_info->terminals[t].source_id, vol_info.unit_id) != 0U) {
								vol_info.sink_id = ac_info->terminals[t].terminal_id;
								vol_info.sink_type = ac_info->terminals[t].terminal_type;
								break;
							}
						}
					}

					if (vol_info.bma_controls[0] || vol_info.num_channels > 0) {
						usbh_uac_add_vol_ctrl(ac_info, &vol_info);
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
static int usbh_uac_parse_as(usbh_itf_data_t *itf_data)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_as_itf_alt_info_t *alt_setting = NULL;
	usbh_uac_format_cfg_t *format_info = NULL;
	usbh_uac_as_itf_info_t *as_itf = NULL;
	usbh_ep_desc_t *ep_cfg = NULL;
	u8 *desc = itf_data->raw_data;
	u16 itf_total_len = 0;
	u16 len;
	u8 alt_set_idx;
	u8 k;

	as_itf = (usbh_uac_as_itf_info_t *)usb_os_malloc(sizeof(usbh_uac_as_itf_info_t));
	if (as_itf == NULL) {
		return HAL_ERR_MEM;
	}

	as_itf->as_itf_num = desc[2];
	len = ((usbh_desc_header_t *) desc)->bLength;
	desc += len;
	itf_total_len += len;

	while (1) {
		if (desc == NULL || itf_total_len >= itf_data->raw_data_len) {
			break;
		}

		if ((u16)(itf_data->raw_data_len - itf_total_len) < 2U) {
			break;
		}

		if (((usbh_desc_header_t *) desc)->bLength == 0) {
			break;
		}

		if ((u32)itf_total_len + ((usbh_desc_header_t *) desc)->bLength > itf_data->raw_data_len) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "AS desc len OOB\n");
			if (uac->isoc_in.as_itf != as_itf && uac->isoc_out.as_itf != as_itf) {
				usb_os_mfree((void *)as_itf);
			}
			return HAL_ERR_PARA;
		}

		switch (((usbh_desc_header_t *) desc)->bDescriptorType) {
		case USB_DESC_TYPE_INTERFACE:
			if (((usbh_itf_desc_t *)desc)->bInterfaceNumber != as_itf->as_itf_num) {
				if (uac->isoc_in.as_itf != as_itf && uac->isoc_out.as_itf != as_itf) {
					usb_os_mfree((void *)as_itf);
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
					usb_os_mfree((void *)as_itf);
				}
				return HAL_OK;
			}
			break;

		case USB_UAC_CS_INTERFACE: {
			usb_uac1_format_type_i_discrete_descriptor *psubtype = (usb_uac1_format_type_i_discrete_descriptor *)desc;
			/* Length check must precede the bDescriptorSubtype read below; FORMAT_TYPE_I_FIXED_LEN(8)
			 * already covers the 3 bytes needed for that read, so no separate header-length check is needed. */
			/* Audio Formats 1.0 2.1.6/2.2/2.3: only a Type I Format Type descriptor lays out
			 * bNrChannels/bSubframeSize/bBitResolution/bSamFreqType; Type II/III (and the extended
			 * variants) use a different layout, so reading these fields there yields garbage. */
			if ((alt_setting != NULL) && (desc[0] >= USBH_UAC_FORMAT_TYPE_I_FIXED_LEN) &&
				(USB_UAC_AS_FORMAT_TYPE == psubtype->bDescriptorSubtype) &&
				(USB_UAC1_FORMAT_TYPE_I == psubtype->bFormatType)) { /* get the format */
				format_info = &(alt_setting->format_info);
				format_info->channels = psubtype->bNrChannels;
				format_info->bit_width = psubtype->bBitResolution;

				if (psubtype->bSamFreqType == USBH_UAC_SAM_FREQ_CONTINUOUS) {
					/* Audio Formats 1.0 2.2.5: bSamFreqType==0 means a continuous range carried as
					 * tLowerSamFreq followed by tUpperSamFreq; keep both bounds in freq[0]/freq[1]. */
					format_info->freq_cnt = USBH_UAC_SAM_FREQ_RANGE_CNT;
					format_info->freq_continuous = 1U;
				} else {
					format_info->freq_cnt = psubtype->bSamFreqType;
					format_info->freq_continuous = 0U;

					if (format_info->freq_cnt > USBH_UAC_FREQ_FORMAT_MAX) {
						RTK_LOGS(TAG, RTK_LOG_WARN, "Freq cnt(%d) > cfg(%d) limit\n", format_info->freq_cnt, USBH_UAC_FREQ_FORMAT_MAX);
						format_info->freq_cnt = USBH_UAC_FREQ_FORMAT_MAX;
					}
				}

				if (desc[0] < (USBH_UAC_FORMAT_TYPE_I_FIXED_LEN + (u16)format_info->freq_cnt * USBH_UAC_SAM_FREQ_ENTRY_SIZE)) {
					format_info->freq_cnt = 0;
					format_info->freq_continuous = 0U;
				} else {
					for (k = 0; k < format_info->freq_cnt; k++) {
						format_info->freq[k] = USBH_UAC_FREQ(psubtype->tSamFreq[k]);
					}
					/* A degenerate or inverted range is unusable; drop the alt setting. */
					if ((format_info->freq_continuous != 0U) && (format_info->freq[0] > format_info->freq[1])) {
						format_info->freq_cnt = 0;
						format_info->freq_continuous = 0U;
					}
				}
			}

			len = ((usbh_desc_header_t *) desc)->bLength;
			desc += len;
		}
		break;

		case USB_DESC_TYPE_ENDPOINT: {
			usbh_ep_desc_t *ep_desc = (usbh_ep_desc_t *)desc;
			if (alt_setting != NULL) {
				/* UAC1 3.7.2.2/4.6.1.1: only Data endpoints (usage_type==0) carry
				 * audio samples. A following Feedback endpoint must not overwrite
				 * the already-saved data endpoint descriptor. */
				u8 usage_type = (ep_desc->bmAttributes >> 4) & 0x03U;
				if (usage_type == 0U) {
					ep_cfg = &(alt_setting->ep_desc);
					usb_os_memcpy((void *)ep_cfg, (const void *)ep_desc, sizeof(usbh_ep_desc_t));

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
			}

			len = ((usbh_desc_header_t *) desc)->bLength;
			desc += len;
		}
		break;

		case USB_UAC_CS_ENDPOINT: {
			usb_uac1_as_ep_desc_t *cs_ep_desc = (usb_uac1_as_ep_desc_t *)desc;
			if ((alt_setting != NULL) && (desc[0] >= USBH_UAC_CS_EP_FIXED_LEN)) {
				alt_setting->freq_ctrl_supported = (cs_ep_desc->bmAttributes & USB_UAC1_EP_ATTR_SAMPLING_FREQ_CONTROL) ? 1U : 0U;
			}

			len = ((usbh_desc_header_t *) desc)->bLength;
			desc += len;
		}
		break;

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
		usb_os_mfree((void *)as_itf);
	}

	return HAL_OK;
}

/**
  * @brief	Parse configuration descriptor
  * @param	host: usb host structure
  * @retval Status
  */
static int usbh_uac_parse_interface_desc(usb_host_t *host)
{
	usbh_dev_id_t dev_id = {0,};
	dev_id.bInterfaceClass = USB_UAC_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_UAC_SUBCLASS_AUDIOCONTROL;
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

		ret = usbh_uac_parse_ac(itf_data);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "AC parse fail\n");
			return ret;
		} else {
			usbh_uac_find_best_ac();
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No AC itf\n");
		return HAL_ERR_PARA;
	}

	dev_id.bInterfaceClass = USB_UAC_CLASS_CODE;
	dev_id.bInterfaceSubClass = USB_UAC_SUBCLASS_AUDIOSTREAMING;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS | USBH_DEV_ID_MATCH_ITF_SUBCLASS;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);
	while (itf_data) {
		if (itf_data->itf_desc_array[0].bAlternateSetting == 0) {
			ret = usbh_uac_parse_as(itf_data);
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
static int usbh_uac_process_set_alt(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_as_itf_info_t *as_itf = (uac->cur_dir == USBH_UAC_ISOC_OUT_DIR) ? uac->isoc_out.as_itf : uac->isoc_in.as_itf;

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
static int usbh_uac_process_set_freq(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_as_itf_info_t *as_itf = (uac->cur_dir == USBH_UAC_ISOC_OUT_DIR) ? uac->isoc_out.as_itf : uac->isoc_in.as_itf;
	const usbh_uac_buf_ctrl_t *pdata_ctrl = (uac->cur_dir == USBH_UAC_ISOC_OUT_DIR) ? &(uac->isoc_out.buf_ctrl) : &(uac->isoc_in.buf_ctrl);

	if (as_itf == NULL) {
		return HAL_ERR_PARA;
	}

	if (as_itf->interface_array[as_itf->choose_alt_idx].freq_ctrl_supported == 0U) {
		return HAL_OK;
	}

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_ENDPOINT;
	setup.req.bRequest = USB_UAC1_SET_CUR;
	setup.req.wValue = USBH_UAC_SAMPLING_FREQ_CONTROL;
	setup.req.wIndex = as_itf->pipe.ep_addr;
	setup.req.wLength = 3U;

	/* The rate the caller actually selected: for a continuous-range alt setting (bSamFreqType==0)
	 * freq[] only holds the range bounds, so choose_freq_idx cannot identify it. */
	u32 _freq = pdata_ctrl->sample_freq;
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
static int usbh_uac_process_set_ch_volume(usb_host_t *host, u8 ch)
{
	usbh_setup_req_t setup;
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	u8 idx = (uac->cur_dir == USBH_UAC_ISOC_IN_DIR) ? ac_info->in_best_idx : ac_info->out_best_idx;
	usbh_uac_volume_info_t *volume_info;
	usbh_uac_fu_info_t *info;
	u16 new_volume_db;

	if (idx == (u8) - 1) {
		return HAL_OK;
	}
	info = &(ac_info->fu_controls[idx]);

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_UAC1_SET_CUR;
	setup.req.wValue = (ch) | (USB_UAC_FU_VOLUME << 8);
	setup.req.wIndex = (ac_info->ac_itf_idx) | (info->unit_id << 8);
	setup.req.wLength = 2U;

	if (uac->cur_dir == USBH_UAC_ISOC_OUT_DIR) {
		volume_info = &(uac->isoc_out.volume_info[ch]);
	} else {
		volume_info = &(uac->isoc_in.volume_info[ch]);
	}

	new_volume_db = usbh_uac_volume_to_db(volume_info, uac->volume_value);

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
static int usbh_uac_process_set_volume(usb_host_t *host)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	u8 idx = (uac->cur_dir == USBH_UAC_ISOC_IN_DIR) ? ac_info->in_best_idx : ac_info->out_best_idx;
	usbh_uac_fu_info_t *info;
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
			ret = usbh_uac_process_set_ch_volume(host, uac->ch_idx);
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
static int usbh_uac_process_set_ch_mute(usb_host_t *host, u8 ch)
{
	usbh_setup_req_t setup;
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	u8 idx = (uac->cur_dir == USBH_UAC_ISOC_IN_DIR) ? ac_info->in_best_idx : ac_info->out_best_idx;
	usbh_uac_fu_info_t *info;

	if (idx == (u8) - 1) {
		return HAL_OK;
	}
	info = &(ac_info->fu_controls[idx]);

	setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_UAC1_SET_CUR;
	setup.req.wValue = (ch) | (USB_UAC_FU_MUTE << 8);
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
static int usbh_uac_process_set_mute(usb_host_t *host)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	u8 idx = (uac->cur_dir == USBH_UAC_ISOC_IN_DIR) ? ac_info->in_best_idx : ac_info->out_best_idx;
	usbh_uac_fu_info_t *info;
	int ret = HAL_BUSY;

	if (idx == (u8) - 1) {
		return HAL_OK;
	}
	info = &(ac_info->fu_controls[idx]);

	/* Walk bma_controls[0..num_channels]: ch=0 is master, ch>0 are per-channel. */
	if (uac->ch_idx <= info->num_channels) {
		if (info->bma_controls[uac->ch_idx] & USB_UAC1_CONTROL_MUTE) {
			ret = usbh_uac_process_set_ch_mute(host, uac->ch_idx);
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
static int usbh_uac_process_get_cur_mute(usb_host_t *host, u8 ch, u8 dir)
{
	usbh_setup_req_t setup;
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	u8 idx = (dir == USBH_UAC_ISOC_IN_DIR) ? (ac_info->in_best_idx) : (ac_info->out_best_idx);
	usbh_uac_fu_info_t *info;

	if (idx == (u8) - 1) {
		/* No Feature Unit on this direction; avoid fu_controls[255] OOB. */
		return HAL_ERR_PARA;
	}
	info = &(ac_info->fu_controls[idx]);

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_UAC1_GET_CUR;
	setup.req.wValue = (ch) | (USB_UAC_FU_MUTE << 8);
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
static int usbh_uac_process_get_cur_volume(usb_host_t *host, u8 ch, u8 dir)
{
	usbh_setup_req_t setup;
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	u8 idx = (dir == USBH_UAC_ISOC_IN_DIR) ? (ac_info->in_best_idx) : (ac_info->out_best_idx);
	usbh_uac_fu_info_t *info;

	if (idx == (u8) - 1) {
		/* No Feature Unit on this direction; avoid fu_controls[255] OOB. */
		return HAL_ERR_PARA;
	}
	info = &(ac_info->fu_controls[idx]);

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = USB_UAC1_GET_CUR;
	setup.req.wValue = (ch) | (USB_UAC_FU_VOLUME << 8);
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
static int usbh_uac_process_get_volume_range(usb_host_t *host, u8 min, u8 ch, u8 dir)
{
	usbh_setup_req_t setup;
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	u8 idx = (dir == USBH_UAC_ISOC_IN_DIR) ? (ac_info->in_best_idx) : (ac_info->out_best_idx);
	usbh_uac_fu_info_t *info;

	if (idx == (u8) - 1) {
		/* No Feature Unit on this direction; avoid fu_controls[255] OOB. */
		return HAL_ERR_PARA;
	}
	info = &(ac_info->fu_controls[idx]);

	setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.req.bRequest = (min) ? (USB_UAC1_GET_MIN) : (USB_UAC1_GET_MAX);
	setup.req.wValue = (ch) | (USB_UAC_FU_VOLUME << 8);
	setup.req.wIndex = (ac_info->ac_itf_idx) | (info->unit_id << 8);
	setup.req.wLength = 2U;

	return usbh_ctrl_request(host, &setup, uac->audio_ctrl_buf);
}

/**
  * @brief  State machine that sequentially queries mute, current volume, min volume, and max volume
  *         for a single channel of the given Feature Unit via UAC1 GET_CUR/GET_MIN/GET_MAX requests.
  * @param  host:        Pointer to the USB host handle.
  * @param  bma_control: Bitmap of controls supported by this channel (USB_UAC_FU_MUTE, USB_UAC_FU_VOLUME).
  * @param  ch:          Channel number (0 = master, 1..N = individual channels).
  * @param  dir:         USBH_UAC_ISOC_OUT_DIR or USBH_UAC_ISOC_IN_DIR.
  * @retval HAL_OK when the sequence for this channel completes, HAL_BUSY while in progress, or an error code.
  */
static int usbh_uac_get_unit_ctrl(usb_host_t *host, u16 bma_control, u8 ch, u8 dir)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_volume_info_t *volume_handle;
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
			ret = usbh_uac_process_get_cur_mute(host, ch, dir);
			if (ret == HAL_OK) {
				//parse to get the buffer
				uac->ctrl_state = UAC_STATE_SCAN_CUR_VOLUME;
				volume_handle->mute = uac->audio_ctrl_buf[0];
				ret = HAL_BUSY;
			} else if (ret != HAL_BUSY) {
				/* The scan skips this optional control and carries on, so the
				 * sequence is still in progress: report BUSY, not the error.
				 * Leaking the error to setup() would make the core treat the
				 * whole class as failed. */
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get mute err %d\n", ret);
				uac->ctrl_state = UAC_STATE_SCAN_CUR_VOLUME;
				ret = HAL_BUSY;
			}
		} else {
			uac->ctrl_state = UAC_STATE_SCAN_CUR_VOLUME;
		}
	} else if (uac->ctrl_state == UAC_STATE_SCAN_CUR_VOLUME) {
		if (bma_control & USB_UAC1_CONTROL_VOLUME) {
			ret = usbh_uac_process_get_cur_volume(host, ch, dir);
			if (ret == HAL_OK) {
				uac->ctrl_state = UAC_STATE_SCAN_MIN_VOLUME;
				//parse to get the volume info
				volume_handle->volume = USBH_LE16(uac->audio_ctrl_buf);
				ret = HAL_BUSY;
			} else if (ret != HAL_BUSY) {
				/* Scan continues; see UAC_STATE_SCAN_MUTE. */
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get vol err %d\n", ret);
				uac->ctrl_state = UAC_STATE_SCAN_MIN_VOLUME;
				ret = HAL_BUSY;
			}
		} else {
			uac->ctrl_state = UAC_STATE_SCAN_MIN_VOLUME;
		}
	} else if (uac->ctrl_state == UAC_STATE_SCAN_MIN_VOLUME) {
		if (bma_control & USB_UAC1_CONTROL_VOLUME) {
			ret = usbh_uac_process_get_volume_range(host, 1, ch, dir);
			if (ret == HAL_OK) {
				uac->ctrl_state = UAC_STATE_SCAN_MAX_VOLUME;
				volume_handle->vol_min = USBH_LE16(uac->audio_ctrl_buf);
				ret = HAL_BUSY;
			} else if (ret != HAL_BUSY) {
				/* Scan continues; see UAC_STATE_SCAN_MUTE. */
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get vol min err %d\n", ret);
				uac->ctrl_state = UAC_STATE_SCAN_MAX_VOLUME;
				ret = HAL_BUSY;
			}
		} else {
			uac->ctrl_state = UAC_STATE_SCAN_MAX_VOLUME;
		}
	} else if (uac->ctrl_state == UAC_STATE_SCAN_MAX_VOLUME) {
		if (bma_control & USB_UAC1_CONTROL_VOLUME) {
			ret = usbh_uac_process_get_volume_range(host, 0, ch, dir);
			if (ret == HAL_OK) {
				uac->ctrl_state = UAC_STATE_CTRL_IDLE;
				volume_handle->vol_max = USBH_LE16(uac->audio_ctrl_buf);
				volume_handle->range_valid = 1;
#if USBH_UAC_DEBUG
				RTK_LOGS(TAG, RTK_LOG_INFO, "Cur is %s, ch %d\n", ((dir) ? ("IN") : ("OUT")), ch);
				RTK_LOGS(TAG, RTK_LOG_INFO, "\tmute %d\n", volume_handle->mute);
				RTK_LOGS(TAG, RTK_LOG_INFO, "\tvolume 0x%04x\n", (u16)volume_handle->volume);
				RTK_LOGS(TAG, RTK_LOG_INFO, "\tvolume min 0x%04x\n", (u16)volume_handle->vol_min);
				RTK_LOGS(TAG, RTK_LOG_INFO, "\tvolume max 0x%04x\n", (u16)volume_handle->vol_max);
#endif
			} else if (ret != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get vol max err %d\n", ret);
				uac->ctrl_state = UAC_STATE_CTRL_IDLE;
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
static void usbh_uac_isoc_out_process_xfer(usb_host_t *host, u32 cur_frame)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_pipe_t *pipe = &(uac->isoc_out.as_itf->pipe);
	usbh_uac_buf_ctrl_t *pdata_ctrl = &(uac->isoc_out.buf_ctrl);
	usb_ringbuf_manager_t *buf_manager = &(pdata_ctrl->buf_manager);
	u16 read_len;
	u8 zlp = 0;

	if (uac->isoc_out.xfer_buf == NULL) {
		return;
	}

	if (!usb_ringbuf_is_empty(buf_manager)) {
		read_len = usb_ringbuf_remove_head(buf_manager, uac->isoc_out.xfer_buf, USBH_UAC_ISOC_BUF_LENGTH, &zlp);
		if (pdata_ctrl->sema_valid) {
			usb_os_sema_give(pdata_ctrl->isoc_sema);
		}
		if (read_len > 0) {
			pipe->frame_num = usbh_uac_frame_num_inc(cur_frame, 1);
#if USBH_UAC_DEBUG
			pdata_ctrl->xfer_start_cnt ++;
#endif
			pipe->xfer_buf = uac->isoc_out.xfer_buf;
			pipe->xfer_len = read_len;
			if (usbh_transfer_data(host, pipe) == HAL_OK) {
				pipe->xfer_state = USBH_EP_XFER_BUSY;
			} else {
				pipe->xfer_state = USBH_EP_XFER_START;
			}
		} else { //data invalid
#if USBH_UAC_DEBUG
			pdata_ctrl->xfer_buf_err_cnt ++;
#endif
		}
	} else { //ringbuf empty
#if USBH_UAC_DEBUG
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
static void usbh_uac_isoc_in_process_xfer(usb_host_t *host, u32 cur_frame)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_pipe_t *pipe = &(uac->isoc_in.as_itf->pipe);
	usbh_uac_buf_ctrl_t *pdata_ctrl = &(uac->isoc_in.buf_ctrl);
	u32 xfer_len;

	if (uac->isoc_in.xfer_buf == NULL) {
		return;
	}

	xfer_len = uac->isoc_in.as_itf->packet_size_large;
	if (xfer_len == 0) {
		xfer_len = pdata_ctrl->mps;
	}

	pipe->frame_num = usbh_uac_frame_num_inc(cur_frame, 1);
#if USBH_UAC_DEBUG
	pdata_ctrl->xfer_start_cnt ++;
#endif
	pipe->xfer_buf = uac->isoc_in.xfer_buf;
	pipe->xfer_len = xfer_len;

	if (usbh_transfer_data(host, pipe) == HAL_OK) {
		pipe->xfer_state = USBH_EP_XFER_BUSY;
	} else {
		pipe->xfer_state = USBH_EP_XFER_START;
	}
}
/**
  * @brief  Device attach callback: parse configuration descriptors, open isochronous pipes,
  *         and kick off the initialization control sequence (get volume/mute info for all channels).
  * @param  host: Pointer to the USB host handle.
  * @retval HAL_OK on success, or an error code if descriptor parsing fails.
  */
static int usbh_uac_attach(usb_host_t *host)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_as_itf_info_t *as_itf = NULL;
	usbh_ep_desc_t *ep_desc = NULL;
	usbh_pipe_t *pipe = NULL;

	uac->host = host;

	int status = HAL_ERR_UNKNOWN;

	status = usbh_uac_parse_interface_desc(host);
	if (status) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Cfg parse fail\n");
		return status;
	}
	usbh_uac_dump_cfgdesc();

	usbh_uac_get_audio_format_info();

	if ((uac->isoc_in.as_itf != NULL) && (uac->isoc_in.buf_ctrl.frame_cnt == 0)) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Drop IN: device offers but cfg disable\n");
		usb_os_mfree((void *)uac->isoc_in.as_itf->fmt_array);
		uac->isoc_in.as_itf->fmt_array = NULL;
		usb_os_mfree((void *)uac->isoc_in.as_itf);
		uac->isoc_in.as_itf = NULL;
	}

	if ((uac->isoc_out.as_itf != NULL) && (uac->isoc_out.buf_ctrl.frame_cnt == 0)) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Drop OUT: device offers but cfg disable\n");
		usb_os_mfree((void *)uac->isoc_out.as_itf->fmt_array);
		uac->isoc_out.as_itf->fmt_array = NULL;
		usb_os_mfree((void *)uac->isoc_out.as_itf);
		uac->isoc_out.as_itf = NULL;
	}

	if (uac->isoc_in.as_itf) {
		as_itf = uac->isoc_in.as_itf;
		as_itf->choose_alt_idx = 0;

		pipe = &(as_itf->pipe);
		ep_desc = &(as_itf->interface_array[as_itf->choose_alt_idx].ep_desc);

		if (usbh_open_pipe(host, pipe, ep_desc, &usbh_uac_driver) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Open isoc in pipe fail\n");
			usbh_uac_deinit_all_pipe();
			if (uac->isoc_in.as_itf != NULL) {
				usb_os_mfree((void *)uac->isoc_in.as_itf->fmt_array);
				uac->isoc_in.as_itf->fmt_array = NULL;
				usb_os_mfree((void *)uac->isoc_in.as_itf);
				uac->isoc_in.as_itf = NULL;
			}
			if (uac->isoc_out.as_itf != NULL) {
				usb_os_mfree((void *)uac->isoc_out.as_itf->fmt_array);
				uac->isoc_out.as_itf->fmt_array = NULL;
				usb_os_mfree((void *)uac->isoc_out.as_itf);
				uac->isoc_out.as_itf = NULL;
			}
			return HAL_ERR_PARA;
		}
	}

	if (uac->isoc_out.as_itf) {
		as_itf = uac->isoc_out.as_itf;
		as_itf->choose_alt_idx = 0;

		pipe = &(as_itf->pipe);
		ep_desc = &(as_itf->interface_array[as_itf->choose_alt_idx].ep_desc);

		if (usbh_open_pipe(host, pipe, ep_desc, &usbh_uac_driver) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Open isoc out pipe fail\n");
			usbh_uac_deinit_all_pipe();
			if (uac->isoc_in.as_itf != NULL) {
				usb_os_mfree((void *)uac->isoc_in.as_itf->fmt_array);
				uac->isoc_in.as_itf->fmt_array = NULL;
				usb_os_mfree((void *)uac->isoc_in.as_itf);
				uac->isoc_in.as_itf = NULL;
			}
			if (uac->isoc_out.as_itf != NULL) {
				usb_os_mfree((void *)uac->isoc_out.as_itf->fmt_array);
				uac->isoc_out.as_itf->fmt_array = NULL;
				usb_os_mfree((void *)uac->isoc_out.as_itf);
				uac->isoc_out.as_itf = NULL;
			}
			return HAL_ERR_PARA;
		}
	}

	if ((uac->cb != NULL) && (uac->cb->attached != NULL)) {
		uac->cb->attached();
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
  * @retval None
  */
static void usbh_uac_detach(usb_host_t *host)
{
	usbh_uac_t *uac = &usbh_uac;
	UNUSED(host);

	uac->xfer_state = UAC_STATE_IDLE;

	/* Physical unplug reaches the driver only through detach() (the core does
	 * not call deinit()). Release the per-device isoc descriptors here so a
	 * re-attach parses fresh ones: stop transfers, close the isoc pipes (needs
	 * uac->host, so do this before clearing it), then free + NULL as_itf and its
	 * fmt_array for both directions. Keep buf_ctrl and xfer_buf (allocated once
	 * in usbh_uac_init) so a re-attach still owns its ring buffer / DMA buffer.
	 * parse_as() stores a freshly parsed as_itf only when the slot is NULL, so
	 * leaving these set would make a re-attach reuse stale alt-setting/EP
	 * descriptors and leak one fmt_array per reconnect. */
	usbh_uac_stop_capture();
	usbh_uac_stop_play();
	usbh_uac_deinit_all_pipe();

	if (uac->isoc_out.as_itf != NULL) {
		usb_os_mfree((void *)uac->isoc_out.as_itf->fmt_array);
		uac->isoc_out.as_itf->fmt_array = NULL;
		usb_os_mfree((void *)uac->isoc_out.as_itf);
		uac->isoc_out.as_itf = NULL;
	}
	if (uac->isoc_in.as_itf != NULL) {
		usb_os_mfree((void *)uac->isoc_in.as_itf->fmt_array);
		uac->isoc_in.as_itf->fmt_array = NULL;
		usb_os_mfree((void *)uac->isoc_in.as_itf);
		uac->isoc_in.as_itf = NULL;
	}

	/* Reset AC parse accumulators so a re-attach re-populates terminals / feature
	 * units from index 0 instead of appending to the previous device's entries
	 * (fixed in-struct arrays, so only the counts need clearing - no free). */
	uac->ac_isoc_desc.terminal_count = 0;
	uac->ac_isoc_desc.volume_ctrl_count = 0;
	uac->ac_isoc_desc.unit_count = 0;

	uac->host = NULL;

	/* Wake any thread blocked inside set_alt_setting/set_volume/set_mute/get_mute/get_volume.
	 * Once xfer_state is IDLE, ctrl_setting() will not run any more, so the
	 * waiter would otherwise time out 1 s later. Surface the disconnect
	 * promptly via ctrl_status. */
	uac->ctrl_state = UAC_STATE_CTRL_IDLE;
	if (uac->ctrl_waiting) {
		uac->ctrl_status = HAL_ERR_UNKNOWN;
		usb_os_sema_give(uac->ctrl_done_sema);
	}

	if ((uac->cb != NULL) && (uac->cb->detached != NULL)) {
		uac->cb->detached();
	}
}

/**
  * @brief  Setup-stage callback invoked after each USB Setup transaction completes.
  *         Scans Feature Unit volume/mute range for all channels (OUT then IN)
  *         via usbh_uac_get_volume_info(). Returns HAL_BUSY on each tick until
  *         the full init_state sequence completes, then fires cb->setup().
  * @param  host: Pointer to the USB host handle.
  * @retval HAL_OK when setup is fully complete, HAL_BUSY while scanning.
  */
static int usbh_uac_setup(usb_host_t *host)
{
	usbh_uac_t *uac = &usbh_uac;
	int ret;

	/* Scan Feature Unit volume/mute range for all channels (OUT then IN).
	 * Returns HAL_BUSY until the full init_state sequence completes.
	 * The SOF-triggered ctrl_setting() is blocked during this phase because
	 * ctrl_state is set to UAC_STATE_SCAN_* which ctrl_setting() does not
	 * handle - preventing it from hijacking the init control pipe. */
	ret = usbh_uac_get_volume_info(host);
	if (ret != HAL_OK) {
		return ret;
	}

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
  * @retval None
  */
static void usbh_uac_sof(usb_host_t *host)
{
	u32 cur_frame = usbh_get_current_frame_number(host);
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_buf_ctrl_t *pdata_ctrl ;
	usbh_pipe_t *pipe;

#if USBH_UAC_DEBUG
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
				 (usbh_uac_frame_num_dec(usbh_uac_frame_num_inc(cur_frame, 1), pipe->frame_num) >= pipe->ep_interval))) {
				usbh_uac_isoc_out_process_xfer(host, cur_frame);
			} else { // interval
#if USBH_UAC_DEBUG
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
				 (usbh_uac_frame_num_dec(usbh_uac_frame_num_inc(cur_frame, 1), pipe->frame_num) >= pipe->ep_interval))) {
				usbh_uac_isoc_in_process_xfer(host, cur_frame);
			} else if (pipe->xfer_state == USBH_EP_XFER_IDLE) {
#if USBH_UAC_DEBUG
				pdata_ctrl->xfer_interval_cnt ++;
#endif
			}
		}
	}

	/* Trigger ctrl_setting() from SOF only after enumeration is complete.
	 * During the init scan (get_volume_info polled by setup()), ctrl_state is
	 * set to SCAN_* states which are not handled by ctrl_setting() and would
	 * be killed by the default case. Guarding with connect_state ensures the
	 * SCAN sequence is never hijacked by a SOF-triggered ctrl_setting(). */
	if ((uac->ctrl_state != UAC_STATE_CTRL_IDLE) &&
		(host->connect_state >= USBH_STATE_SETUP)) {
		usbh_notify(host, 0x00, &usbh_uac_driver);
	}
}

/**
  * @brief  Transfer completion callback: handle the end of an isochronous OUT or IN transfer,
  *         enqueue received IN data into the ring buffer, and schedule the next transfer.
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `usb_os_sema_take`) are not permitted.
  * @param[in] host: Pointer to the USB host handle.
  * @param[in] pipe_num: Pipe number of the completed transfer.
  * @retval None
  */
static void usbh_uac_completed(usb_host_t *host, u8 pipe_num)
{
	u32 cur_frame = usbh_get_current_frame_number(host);
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_buf_ctrl_t *pdata_ctrl;
	usbh_pipe_t *pipe;

	if (uac->isoc_out.as_itf) {
		pipe = &(uac->isoc_out.as_itf->pipe);
		pdata_ctrl = &(uac->isoc_out.buf_ctrl);

		if (pdata_ctrl->next_xfer == 1) {
			if (pipe_num == pipe->pipe_num) {
#if USBH_UAC_DEBUG
				pdata_ctrl->xfer_done_cnt ++;
#endif
				pipe->xfer_state = USBH_EP_XFER_IDLE;

				if (!usb_ringbuf_is_empty(&(pdata_ctrl->buf_manager))) {
					//trigger next xfer after binterval = 1
					if (usbh_uac_frame_num_dec(usbh_uac_frame_num_inc(cur_frame, 1), pipe->frame_num) >= pipe->ep_interval) {
						usbh_uac_isoc_out_process_xfer(host, cur_frame);//USBH_EP_XFER_START
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
#if USBH_UAC_DEBUG
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
				if (usbh_uac_frame_num_dec(usbh_uac_frame_num_inc(cur_frame, 1), pipe->frame_num) >= pipe->ep_interval) {
					usbh_uac_isoc_in_process_xfer(host, cur_frame);
				} else {
					pipe->xfer_state = USBH_EP_XFER_WAIT_SOF;
				}
			}
		}
	}
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
static void usbh_uac_ctrl_finish(int status)
{
	usbh_uac_t *uac = &usbh_uac;

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
static int usbh_uac_ctrl_setting(usb_host_t *host, u32 msg)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_volume_info_t *vi;
	int ret = HAL_OK;
	int ret_status = HAL_BUSY;
	UNUSED(msg);

	switch (uac->ctrl_state) {
	case UAC_STATE_CTRL_IDLE:
		ret_status = HAL_OK;
		break;

	case UAC_STATE_SET_ALT_SETTING:
		ret = usbh_uac_process_set_alt(host);
		if (ret == HAL_OK) {
			uac->ctrl_state = UAC_STATE_SET_FREQ;
		} else if (ret != HAL_BUSY) {
			/* Do NOT advance to SET_FREQ on alt-set failure: writing the new
			 * sample rate to the old endpoint would either be silently accepted
			 * (caller thinks alt was switched) or STALL on a different request,
			 * confusing the diagnosis. Surface the real error. */
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set alt err %d\n", ret);
			usbh_uac_ctrl_finish(ret);
			ret_status = HAL_OK;
		}
		break;

	case UAC_STATE_SET_FREQ:
		ret = usbh_uac_process_set_freq(host);
		if (ret == HAL_OK) {
			usbh_uac_ctrl_finish(HAL_OK);
			ret_status = HAL_OK;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set(%d) freq err %d\n", uac->cur_dir, ret);
			usbh_uac_ctrl_finish(ret);
			ret_status = HAL_OK;
		}
		break;

	case UAC_STATE_SET_MUTE:
		ret = usbh_uac_process_set_mute(host);
		if (ret == HAL_OK) {
			usbh_uac_ctrl_finish(HAL_OK);
			ret_status = HAL_OK;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set(%d) mute err %d\n", uac->cur_dir, ret);
			usbh_uac_ctrl_finish(ret);
			ret_status = HAL_OK;
		}
		break;

	case UAC_STATE_SET_VOLUME:
		ret = usbh_uac_process_set_volume(host);
		if (ret == HAL_OK) {
			usbh_uac_ctrl_finish(HAL_OK);
			ret_status = HAL_OK;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set(%d) vol err %d\n", uac->cur_dir, ret);
			usbh_uac_ctrl_finish(ret);
			ret_status = HAL_OK;
		}
		break;

	case UAC_STATE_GET_MUTE:
		ret = usbh_uac_process_get_cur_mute(host, uac->get_ch, uac->cur_dir);
		if (ret == HAL_OK) {
			vi = (uac->cur_dir == USBH_UAC_ISOC_OUT_DIR) ?
				 &(uac->isoc_out.volume_info[uac->get_ch]) :
				 &(uac->isoc_in.volume_info[uac->get_ch]);
			uac->get_mute_result = uac->audio_ctrl_buf[0];
			vi->mute = uac->get_mute_result;     /* keep cache in sync */
			usbh_uac_ctrl_finish(HAL_OK);
			ret_status = HAL_OK;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Get mute(%d) err %d\n", uac->cur_dir, ret);
			usbh_uac_ctrl_finish(ret);
			ret_status = HAL_OK;
		}
		break;

	case UAC_STATE_GET_VOLUME:
		ret = usbh_uac_process_get_cur_volume(host, uac->get_ch, uac->cur_dir);
		if (ret == HAL_OK) {
			vi = (uac->cur_dir == USBH_UAC_ISOC_OUT_DIR) ?
				 &(uac->isoc_out.volume_info[uac->get_ch]) :
				 &(uac->isoc_in.volume_info[uac->get_ch]);
			uac->get_volume_result = (s16)USBH_LE16(uac->audio_ctrl_buf);
			vi->volume = uac->get_volume_result; /* keep cache in sync */
			usbh_uac_ctrl_finish(HAL_OK);
			ret_status = HAL_OK;
		} else if (ret != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Get vol(%d) err %d\n", uac->cur_dir, ret);
			usbh_uac_ctrl_finish(ret);
			ret_status = HAL_OK;
		}
		break;

	default:
		/* Unknown state means a programmer mistake (e.g. an enum value was
		 * added but its case forgotten). Forcing IDLE + signaling waiters
		 * keeps the host stack alive instead of silently hanging callers. */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Unknown ctrl_state %d\n", uac->ctrl_state);
		usbh_uac_ctrl_finish(HAL_ERR_UNKNOWN);
		ret_status = HAL_OK;
		break;
	}

	return ret_status;
}

/**
  * @brief  Main UAC class process callback called by the USB host core on each driver message.
  *         In TRANSFER state, delegates pipe-0 messages to the control state machine.
  *         In ERROR state, issues a ClearFeature to recover.
  * @param  host:  Pointer to the USB host handle.
  * @param  msg: Pointer to the driver message (contains pipe_num and message type).
  * @retval None
  */
static void usbh_uac_process(usb_host_t *host, usbh_drv_msg_t *msg)
{
	usbh_uac_t *uac = &usbh_uac;
	int ret;

	switch (uac->xfer_state) {
	case UAC_STATE_TRANSFER:
		/* UAC only drives the control endpoint (pipe 0) here; ISOC pipes are
		   serviced in the completed callback. */
		if ((msg) && (msg->pipe_num == 0x00)) {
			(void)usbh_uac_ctrl_setting(host, 0);
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
static u32 usbh_uac_next_packet_size(usbh_uac_buf_ctrl_t *pdata_ctrl, u8 dir)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_as_itf_info_t *as_itf;
	u32 sample_accum = 0;
	u32 ret;

	if (dir == USBH_UAC_ISOC_OUT_DIR) {
		as_itf = uac->isoc_out.as_itf;
	} else {
		as_itf = uac->isoc_in.as_itf;
	}

	/* as_itf may be freed/NULLed by a concurrent usbh_uac_detach(); fall back to mps. */
	if (as_itf == NULL) {
		return pdata_ctrl->mps;
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
static int usbh_uac_write_ring_buf(usbh_uac_buf_ctrl_t *pdata_ctrl, u8 *buffer, u32 size, u32 *written_len)
{
	usbh_uac_t *uac = &usbh_uac;
	usb_ringbuf_manager_t *handle = &(pdata_ctrl->buf_manager);
	u32 written_size = pdata_ctrl->written;
	u32 offset = 0;
	u32 xfer_len = 0;
	u32 can_copy_len;
	u32 copy_len;

	if (written_size) {
		xfer_len = usbh_uac_next_packet_size(pdata_ctrl, USBH_UAC_ISOC_OUT_DIR);
		if (xfer_len <= written_size) {
			/* Staged bytes no longer fit the current packet geometry: drop them rather than
			 * underflowing can_copy_len below and writing past ringbuf_partial_write_buf. */
			pdata_ctrl->written = 0;
			written_size = 0;
		}
	}

	if (written_size) {
		can_copy_len = xfer_len - written_size; /* xfer_len set above whenever written_size != 0 */
		if (size >= can_copy_len && usb_ringbuf_is_full(handle)) {
			return 1;
		}

		copy_len = size < can_copy_len ? size : can_copy_len;

		usb_os_memcpy((void *)(uac->ringbuf_partial_write_buf + written_size), (const void *)buffer, copy_len);
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

		xfer_len = usbh_uac_next_packet_size(pdata_ctrl, USBH_UAC_ISOC_OUT_DIR);

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

		usb_os_memcpy((void *)(uac->ringbuf_partial_write_buf), (const void *)buffer, size);
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
static u32 usbh_uac_read_ring_buf(usbh_uac_buf_ctrl_t *buf_ctrl, u8 *buffer, u32 size, u32 *copy_len, u16 *pkt_cnt, u32 *zero_pkt_flag)
{
	usb_ringbuf_manager_t *buf_list = &(buf_ctrl->buf_manager);
	u32 read_len;
	u8 valid = 0;

	do {
		/* should exit : 1) Enough data has been obtained; 2) the next data cannot be saved completely.
		 * The headroom to reserve is a whole ring node, not one mps: on a high-bandwidth endpoint a
		 * node holds a multi-transaction packet, and usb_ringbuf_remove_head() would truncate it to
		 * the space left in the caller's buffer and drop the remainder with the node. */
		if ((*copy_len >= size) || (*copy_len + buf_ctrl->node_size > size)) {
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
static void usbh_uac_ep_buf_ctrl_deinit(usbh_uac_buf_ctrl_t *buf_ctrl)
{
	/* Bounded wait for any in-flight reader/writer to observe sema_valid=0
	 * and clear wait_sema. Without a timeout this loop deadlocks deinit
	 * if the waiter was killed or never gets a chance to run.
	 * usb_os_sleep_ms yields the CPU so the waiter can actually run. */
	u16 wait_ms = 10U; /* 10 ms total */

	buf_ctrl->mps = 0;
	buf_ctrl->node_size = 0;
	buf_ctrl->next_xfer = 0;

	/* The ring buffer and the packet geometry are rebuilt by the following ep_buf_ctrl_init(),
	 * so any bytes staged in ringbuf_partial_write_buf for the old packet size are stale: keep
	 * them and the next usbh_uac_write() would index the staging buffer past the new packet. */
	buf_ctrl->written = 0;
	buf_ctrl->sample_accum = 0;
	buf_ctrl->last_sample_accum = 0;

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
static void usbh_uac_channel_deinit(usbh_uac_channel_t *ch)
{
	usbh_uac_ep_buf_ctrl_deinit(&(ch->buf_ctrl));

	if (ch->as_itf != NULL) {
		usb_os_mfree((void *)ch->as_itf->fmt_array);
		ch->as_itf->fmt_array = NULL;
		usb_os_mfree((void *)ch->as_itf);
		ch->as_itf = NULL;
	}

	usb_os_mfree((void *)ch->xfer_buf);
	ch->xfer_buf = NULL;
}

/**
  * @brief  Initialize UAC endpoint buffer control structure
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @param  pipe: Pointer to pipe parameters structure
  * @param  packet_size: Largest audio packet this alt setting produces, in bytes
  * @retval Status
  */
static int usbh_uac_ep_buf_ctrl_init(usbh_uac_buf_ctrl_t *buf_ctrl, usbh_pipe_t *pipe, u16 packet_size)
{
	int ret = HAL_ERR_MEM;
	u8 buf_list_cnt;
	u16 node_size;

	buf_list_cnt = buf_ctrl->frame_cnt;
	buf_ctrl->mps = pipe->ep_mps;

	if (buf_list_cnt == 0 || buf_ctrl->mps == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Err param: cnt=%d, mps=%d\n", buf_list_cnt, buf_ctrl->mps);
		return ret;
	}

	/* USB 2.0 5.6.3: a high-bandwidth isochronous endpoint may move up to three transactions
	 * per service interval, so one audio packet can be larger than a single wMaxPacketSize -
	 * set_alt_setting accepts any packet up to ep_mps * ep_trans. A ring node must therefore be
	 * sized for the whole packet, not for one transaction: usb_ringbuf_add_tail() silently
	 * truncates anything longer than node_size, which would drop the tail of every packet and
	 * progressively shift the audio stream. */
	node_size = (packet_size > buf_ctrl->mps) ? packet_size : buf_ctrl->mps;
	buf_ctrl->node_size = node_size;

	ret = usb_ringbuf_manager_init(&(buf_ctrl->buf_manager), buf_list_cnt, node_size, 1);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Ringbuf init fail\n");
		return ret;
	}

	if (usb_os_sema_create(&(buf_ctrl->isoc_sema)) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ISOC sema create fail\n");
		usb_ringbuf_manager_deinit(&(buf_ctrl->buf_manager));
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
  * @param  waited_ms:  Optional out parameter receiving the time actually spent waiting, in ms.
  *                     Lets a caller that loops on this helper charge each wait against one
  *                     overall budget instead of granting the full timeout again per iteration.
  *                     May be NULL when the caller does not track a budget.
  * @retval Status
  */
static int usbh_uac_wait_isoc_with_status_check(usbh_uac_buf_ctrl_t *pdata_ctrl, uint32_t timeout_ms, u32 *waited_ms)
{
	int ret = HAL_ERR_PARA;
	u32 elapsed = 0;
	u32 wait_time = 0;

	while (elapsed < timeout_ms) {
		if (usbh_uac_usb_status_check() != HAL_OK) {
			pdata_ctrl->wait_sema = 0;
			goto exit;
		}

		wait_time = (timeout_ms - elapsed > USBH_UAC_WAIT_SLICE_MS) ? USBH_UAC_WAIT_SLICE_MS : (timeout_ms - elapsed);

		pdata_ctrl->wait_sema = 1;
		if (usb_os_sema_take(pdata_ctrl->isoc_sema, wait_time) == HAL_OK) {
			pdata_ctrl->wait_sema = 0;
			/* Charge the whole slice: the semaphore may have been given at any point inside it
			 * and the OS does not report when, so this over-counts by less than one slice. */
			elapsed += wait_time;
			if (!pdata_ctrl->sema_valid) {
				goto exit;
			}
			ret = HAL_OK;
			goto exit;
		}

		elapsed += wait_time;
	}

	pdata_ctrl->wait_sema = 0;

exit:
	if (waited_ms != NULL) {
		*waited_ms = elapsed;
	}
	return ret;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize the UAC class driver: allocate DMA-safe buffers, create synchronization
  *         primitives, set ring buffer depths from the callback structure, and invoke cb->init().
  * @param  cb:        Pointer to the user callback structure. Must be non-NULL.
  * @retval HAL_OK on success, HAL_ERR_MEM if a buffer allocation fails.
  */
int usbh_uac_init(const usbh_uac_cb_t *cb)
{
	usbh_uac_t *uac = &usbh_uac;
	int ret;

	if ((cb == NULL) || ((cb->isoc_out_frm_cnt == 0) && (cb->isoc_in_frm_cnt == 0))) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Param err\n");
		return HAL_ERR_PARA;
	}

	usb_os_memset((void *)uac, 0x00, sizeof(usbh_uac_t));

	uac->audio_ctrl_buf = (u8 *)usb_os_malloc(USBH_UAC_AUDIO_CTRL_BUF_MAX_LEN);
	if (NULL == uac->audio_ctrl_buf) {
		return HAL_ERR_MEM;
	}

	/* The isoc DMA buffers belong to the class lifetime: allocate them here for
	 * every direction the configuration enables and release them only in
	 * usbh_uac_deinit(). Sizing them from the (static) cb configuration - not from
	 * what the currently attached device happens to offer - keeps a later hotplug
	 * of a device that does use the direction from finding a NULL xfer_buf. */
	if (cb->isoc_in_frm_cnt != 0U) {
		uac->isoc_in.xfer_buf = (u8 *)usb_os_malloc(USBH_UAC_ISOC_BUF_LENGTH);
		if (NULL == uac->isoc_in.xfer_buf) {
			goto get_rx_buf_fail;
		}
	}

	if (cb->isoc_out_frm_cnt != 0U) {
		uac->isoc_out.xfer_buf = (u8 *)usb_os_malloc(USBH_UAC_ISOC_BUF_LENGTH);
		if (NULL == uac->isoc_out.xfer_buf) {
			goto get_tx_buf_fail;
		}
	}

	uac->ringbuf_partial_write_buf = (u8 *)usb_os_malloc(USBH_UAC_ISOC_BUF_LENGTH);
	if (NULL == uac->ringbuf_partial_write_buf) {
		goto get_wd_buf_fail;
	}

	if (usb_os_lock_create(&uac->alt_set_mutex) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Mutex create fail\n");
		uac->alt_set_mutex = NULL;
		goto cb_init_fail;
	}
	if (usb_os_sema_create(&uac->ctrl_done_sema) != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Ctrl sema create fail\n");
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

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC init fail\n");
			goto cb_init_fail;
		}
	}

#if USBH_UAC_DEBUG
	if (rtos_task_create(&(uac->dump_status_task), ((const char *)"usbh_uac_status_dump_thread"), usbh_uac_status_dump_thread, NULL, 768,
						 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Dump task create fail\n");
	}
#endif

	uac->cb = cb;

	ret = usbh_register_class(&usbh_uac_driver);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Register class fail %d\n", ret);
		goto cb_init_fail;
	}

	return HAL_OK;

cb_init_fail:
#if USBH_UAC_DEBUG
	if (uac->dump_status_task_alive) {
		uac->dump_status_task_exit = 0;
		do {
			rtos_time_delay_ms(1);
		} while (uac->dump_status_task_alive);
	}
#endif
	if (uac->alt_set_mutex != NULL) {
		usb_os_lock_delete(uac->alt_set_mutex);
		uac->alt_set_mutex = NULL;
	}
	if (uac->ctrl_done_sema != NULL) {
		usb_os_sema_delete(uac->ctrl_done_sema);
		uac->ctrl_done_sema = NULL;
	}
	usb_os_mfree((void *)uac->ringbuf_partial_write_buf);
	uac->ringbuf_partial_write_buf = NULL;

get_wd_buf_fail:
	usb_os_mfree((void *)uac->isoc_out.xfer_buf);
	uac->isoc_out.xfer_buf = NULL;

get_tx_buf_fail:
	usb_os_mfree((void *)uac->isoc_in.xfer_buf);
	uac->isoc_in.xfer_buf = NULL;

get_rx_buf_fail:
	usb_os_mfree((void *)uac->audio_ctrl_buf);
	uac->audio_ctrl_buf = NULL;

	return HAL_ERR_MEM;
}

/**
  * @brief  De-initialize the UAC class driver: stop all transfers, invoke cb->deinit(),
  *         close isochronous pipes, destroy ring buffers, and free all allocated memory.
  */
void usbh_uac_deinit(void)
{
	usbh_uac_t *uac = &usbh_uac;

	usbh_unregister_class(&usbh_uac_driver);

	usbh_uac_stop_capture();
	usbh_uac_stop_play();

#if USBH_UAC_DEBUG
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

	usbh_uac_deinit_all_pipe();
	usbh_uac_channel_deinit(&(uac->isoc_out));
	usbh_uac_channel_deinit(&(uac->isoc_in));

	/* Symmetry with usbh_cdc_acm_deinit: clear host so a stale caller cannot
	 * reach usbh_notify() with a freed hcd. (deinit_all_pipe above needs it.) */
	uac->host = NULL;

	usb_os_mfree((void *)uac->audio_ctrl_buf);
	uac->audio_ctrl_buf = NULL;

	usb_os_mfree((void *)uac->ringbuf_partial_write_buf);
	uac->ringbuf_partial_write_buf = NULL;

	if (uac->alt_set_mutex != NULL) {
		usb_os_lock_delete(uac->alt_set_mutex);
		uac->alt_set_mutex = NULL;
	}
	if (uac->ctrl_done_sema != NULL) {
		usb_os_sema_delete(uac->ctrl_done_sema);
		uac->ctrl_done_sema = NULL;
	}
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
int usbh_uac_set_alt_setting(u8 dir, u8 channels, u8 bit_width, u32 sampling_freq)
{
	usbh_uac_format_cfg_t *fmt = NULL;
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_buf_ctrl_t *pdata_ctrl = NULL;
	usbh_uac_as_itf_info_t *as_itf = NULL;
	usbh_pipe_t *pipe = NULL;
	usbh_ep_desc_t *ep_desc = NULL;
	usb_host_t *host = uac->host;
	int ret = HAL_ERR_PARA;
	int set_flag = 0;
	int i = 0;
	int j = 0;
	u32 ep_cap = 0U;
	u32 binterval = 0U;
	u32 compliant_rate = 0U;
	u32 compliant_size = 0U;
	u16 ep_mps = 0U;
	u16 ep_trans = 0U;
	u8 alt_num = 0U;
	u8 legal_interval = 0U;

	/* Reject if the device is gone (detach nulled uac->host): host is used below
	 * by usbh_open_pipe()/usbh_notify() and would otherwise be dereferenced NULL. */
	if (usbh_uac_usb_status_check() != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Dev not ready\n");
		return HAL_ERR_HW;
	}

	as_itf = usbh_uac_get_as_itf_instance(dir);
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
		if (fmt->freq_continuous != 0U) {
			/* Audio Formats 1.0 2.2.5: any rate within [tLowerSamFreq, tUpperSamFreq] is legal. */
			if ((fmt->freq_cnt >= USBH_UAC_SAM_FREQ_RANGE_CNT) && (sampling_freq >= fmt->freq[0]) && (sampling_freq <= fmt->freq[1])) {
				j = 0;
				set_flag = 1;
			}
		} else {
			for (j = 0; j < fmt->freq_cnt; j++) {
				if (fmt->freq[j] == sampling_freq) {
					set_flag = 1;// Return as soon as we find a match
					break;
				}
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

		/* An isochronous bInterval is an exponent on both speeds (period = 2^(bInterval-1) frames
		 * on FS, microframes on HS), so the same 1..16 range applies to each; see
		 * USB_ISOC_xS_BINTERVAL_MIN/MAX in usb_ch9.h. bInterval comes straight from the device
		 * descriptor and is therefore untrusted, and an out-of-range value used as a shift count
		 * would be undefined behaviour (MISRA C:2012 Rule 1.3).
		 *
		 * Clamp rather than reject, matching usbh_get_interval() in the host core: that function
		 * derives pipe->ep_interval from this very descriptor when usbh_open_pipe() runs below,
		 * and the packet rate computed here must describe the same service interval - one says
		 * how many bytes per interval, the other how often the interval comes round. Deriving
		 * them from different readings of the same field would silently drift the audio stream.
		 * Clamping does not weaken validation: a bInterval of 16 is legal yet still yields a zero
		 * packet rate, so the zero guards below reject both it and any clamped larger value. */
		binterval = MIN((MAX((u32)ep_desc->bInterval, USB_EP_BINTERVAL_EXP_MIN)), USB_EP_BINTERVAL_EXP_MAX);
		if (binterval != (u32)ep_desc->bInterval) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Clamp isoc bInterval %d to %d\n", ep_desc->bInterval, binterval);
		}

		ep_mps = ep_desc->wMaxPacketSize & USB_EP_MPS_SIZE_MASK;
		/* wMaxPacketSize bits 12:11 encode *additional* transactions per microframe (0-2), so +1 gives the actual count */
		ep_trans = ((ep_desc->wMaxPacketSize & USB_EP_MPS_TRANS_MASK) >> USB_EP_MPS_TRANS_POS) + 1U;
		ep_cap = (u32)ep_mps * ep_trans;

		if (host->dev_speed == USB_SPEED_HIGH) {
			/* HS isoc: bInterval encodes interval = 2^(bInterval-1) microframes (8000/s) */
			compliant_rate = (USBH_UAC_ONE_KHZ * USBH_UAC_HS_MICROFRAMES_PER_MS) >> (binterval - 1U);
			/* A legal bInterval of 14..16 shifts 8000 down to 0, i.e. a service interval longer
			 * than one second. No audio format this driver supports can be carried that way, and
			 * the value is used as a divisor right below, so reject the alt setting instead of
			 * dividing by zero. */
			if (compliant_rate == 0U) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Isoc interval too long, bInterval %d\n", binterval);
				usb_os_unlock(uac->alt_set_mutex);
				return HAL_ERR_PARA;
			}
			compliant_size = channels * bit_width / USBH_UAC_BIT_TO_BYTE *
							 ((sampling_freq + (compliant_rate - 1U)) / compliant_rate);

			/* A compliant HS isoc endpoint sizes wMaxPacketSize for the rate its own
			 * bInterval implies (e.g. bInterval=4 -> 1 ms -> ~48 samples @48kHz). A
			 * handful of non-compliant UAC1 devices keep an FS-style bInterval
			 * (1..3 meaning 1/2/4 ms, not a 2^(n-1)-microframe exponent) even in
			 * their HS descriptor; because the device itself scheduled the transfer
			 * at that slower FS-style rate, its wMaxPacketSize then comes out ~8x
			 * larger than what the compliant reading would need. A device that
			 * legitimately uses bInterval=1..3 on HS (125/250/500 us service) does
			 * not show this gap. Gate on that size mismatch, not on bInterval<4
			 * alone, so a compliant device is never misdetected. */
			legal_interval = ((binterval < USBH_UAC_LEGACY_FS_SIZE_RATIO_MIN) && (compliant_size > 0U) &&
							  (ep_cap >= (compliant_size * USBH_UAC_LEGACY_FS_SIZE_RATIO_MIN))) ? 1U : 0U;

			if (legal_interval != 0U) {
				pdata_ctrl->packet_rate = USBH_UAC_ONE_KHZ / binterval;
			} else {
				pdata_ctrl->packet_rate = compliant_rate;
			}
		} else {
			pdata_ctrl->packet_rate = USBH_UAC_ONE_KHZ >> (binterval - 1U);
		}

		/* packet_rate is the divisor of every packet-size term below. It can still be 0 for a
		 * bInterval that is legal but implies a service interval longer than the 1 ms (FS) or
		 * 1 s (HS) the rate is derived from, so it must be checked here rather than assumed
		 * non-zero from the bInterval range alone. */
		if (pdata_ctrl->packet_rate == 0U) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Zero packet rate, bInterval %d\n", binterval);
			usb_os_unlock(uac->alt_set_mutex);
			return HAL_ERR_PARA;
		}

		pdata_ctrl->sample_rem = sampling_freq % pdata_ctrl->packet_rate;
		//calculate accurate one frame size(byte)
		as_itf->packet_size_small = channels * bit_width / USBH_UAC_BIT_TO_BYTE * (sampling_freq / pdata_ctrl->packet_rate);
		as_itf->packet_size_large = channels * bit_width / USBH_UAC_BIT_TO_BYTE * ((sampling_freq + (pdata_ctrl->packet_rate - 1U)) / pdata_ctrl->packet_rate);

		/* Reject if the computed payload cannot fit the isoc xfer buffer or the
		 * endpoint's actual per-service-interval transaction capacity. */
		if ((as_itf->packet_size_large > USBH_UAC_ISOC_BUF_LENGTH) || (as_itf->packet_size_large > ep_cap)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Packet size %d exceeds buf/ep cap %d\n", as_itf->packet_size_large, ep_cap);
			usb_os_unlock(uac->alt_set_mutex);
			return HAL_ERR_PARA;
		}

		if (dir == USBH_UAC_ISOC_OUT_DIR) {
			usbh_uac_stop_play();
		} else {
			usbh_uac_stop_capture();
		}

		//reinit pipe
		usbh_uac_deinit_pipe(dir);
		if (usbh_open_pipe(host, pipe, ep_desc, &usbh_uac_driver) != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Open isoc pipe fail\n");
			usb_os_unlock(uac->alt_set_mutex);
			return HAL_ERR_PARA;
		}

		if (legal_interval != 0U) {
			/* usbh_open_pipe() derives pipe->ep_interval from the HS microframe
			 * formula unconditionally; override it to the FS-style ms reading
			 * this device actually uses (see legal_interval above). */
			pipe->ep_interval = binterval * USBH_UAC_HS_MICROFRAMES_PER_MS;
		}

		if (dir == USBH_UAC_ISOC_OUT_DIR) {
			usbh_uac_ep_buf_ctrl_deinit(&(uac->isoc_out.buf_ctrl));
			ret = usbh_uac_ep_buf_ctrl_init(&(uac->isoc_out.buf_ctrl), pipe, as_itf->packet_size_large);
		} else {
			usbh_uac_ep_buf_ctrl_deinit(&(uac->isoc_in.buf_ctrl));
			ret = usbh_uac_ep_buf_ctrl_init(&(uac->isoc_in.buf_ctrl), pipe, as_itf->packet_size_large);
		}

		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Buf init fail\n");
			usbh_uac_deinit_pipe(dir);
			usb_os_unlock(uac->alt_set_mutex);
			return ret;
		}

		uac->cur_dir = dir;
		uac->ctrl_status = HAL_BUSY;
		uac->xfer_state = UAC_STATE_TRANSFER;
		uac->ctrl_state = UAC_STATE_SET_ALT_SETTING;
		uac->ctrl_waiting = 1;

		usbh_notify(host, 0, &usbh_uac_driver);

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
const usbh_uac_audio_fmt_t *usbh_uac_get_alt_setting(u8 dir, u8 *fmt_cnt)
{
	usbh_uac_as_itf_info_t *as_itf = NULL;

	as_itf = usbh_uac_get_as_itf_instance(dir);
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
u32 usbh_uac_get_frame_size(u8 dir)
{
	usbh_uac_as_itf_info_t *as_itf = NULL;
	as_itf = usbh_uac_get_as_itf_instance(dir);
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
u32 usbh_uac_write(u8 *buffer, u32 size, u32 timeout_ms)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_buf_ctrl_t *pdata_ctrl = &(uac->isoc_out.buf_ctrl);
	u32 written_len = 0;
	u32 try_len, just_written;
	u8 need_wait = 0, last_zero = 0;

	if (pdata_ctrl->mps == 0 || !pdata_ctrl->sema_valid) {
		return 0;
	}

	if (pdata_ctrl->next_xfer == 0) {
		return 0;
	}

	if (usbh_uac_usb_status_check() != HAL_OK) {
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
			if (usbh_uac_wait_isoc_with_status_check(pdata_ctrl, timeout_ms, NULL) != HAL_OK) {
				break;
			}
			last_zero = 0;
		}

		try_len = size - written_len;
		just_written = 0;

		usbh_uac_write_ring_buf(pdata_ctrl, buffer + written_len, try_len, &just_written);

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
u32 usbh_uac_read(u8 *buffer, u32 size, u32 time_out_ms)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_in.buf_ctrl);
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
		usbh_uac_read_ring_buf(buf_ctrl, buffer, size, &copy_len, &pkt_cnt, &zero_pkt_flag);
	} else {
		u32 elapsed = 0;
		u32 waited;

		do {
			if (usb_ringbuf_is_empty(&(buf_ctrl->buf_manager))) {
				/* time_out_ms is the budget for the whole call, not per wait. A sparse capture
				 * stream delivers a packet, fails to fill size, and empties the ring again; giving
				 * the full timeout to every iteration would let this block the caller without any
				 * upper bound, contrary to the timeout this API documents. */
				if (elapsed >= time_out_ms) {
					break;
				}
				waited = 0;
				if (usbh_uac_wait_isoc_with_status_check(buf_ctrl, time_out_ms - elapsed, &waited) != HAL_OK) {
					elapsed += waited;
					break;
				}
				elapsed += waited;
				/* If deinit started while we waited, do not touch resources */
				if (!buf_ctrl->sema_valid) {
					break;
				}
			} else {
				/* if did not read any pkt, loop to check the wr/rd pos*/
				if ((usbh_uac_read_ring_buf(buf_ctrl, buffer, size, &copy_len, &pkt_cnt, NULL) == 0)) {  //|| (copy_len >0)
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
void usbh_uac_start_play(void)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out.buf_ctrl);
	usbh_pipe_t *pipe;

	if (usbh_uac_usb_status_check() != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Dev not ready\n");
		return;
	}

	if (uac->isoc_out.as_itf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No ISOC OUT interface\n");
		return;
	}
	pipe = &(uac->isoc_out.as_itf->pipe);

#if USBH_UAC_DEBUG
	usbh_uac_reset_test_cnt();
	usbh_uac_reset_isr_time();
#endif

	buf_ctrl->next_xfer = 1;
	uac->xfer_state = UAC_STATE_TRANSFER;
	pipe->xfer_state = USBH_EP_XFER_START;
	usbh_notify(uac->host, pipe->pipe_num, &usbh_uac_driver);
}

/**
  * @brief  Stop isochronous OUT (playback) transfers by clearing the next_xfer flag.
  * @retval void
  */
void usbh_uac_stop_play(void)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out.buf_ctrl);

	buf_ctrl->next_xfer = 0;

	if (buf_ctrl->sema_valid && buf_ctrl->wait_sema) {
		usb_os_sema_give(buf_ctrl->isoc_sema);
	}

	if (uac->isoc_out.as_itf != NULL) {
		uac->isoc_out.as_itf->pipe.xfer_state = USBH_EP_XFER_IDLE;
	}

#if USBH_UAC_DEBUG
	usbh_uac_reset_test_cnt();
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC stop playback\n");
#endif
}

/**
  * @brief  Start isochronous IN (recording) transfers.
  *         Sets next_xfer, puts the state machine into TRANSFER state, and notifies the USB core.
  * @retval void
  */
void usbh_uac_start_capture(void)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_in.buf_ctrl);
	usbh_pipe_t *pipe;

	if (usbh_uac_usb_status_check() != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Dev not ready\n");
		return;
	}

	if (uac->isoc_in.as_itf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No ISOC IN interface\n");
		return;
	}
	pipe = &(uac->isoc_in.as_itf->pipe);

#if USBH_UAC_DEBUG
	usbh_uac_reset_test_cnt();
	usbh_uac_reset_isr_time();
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC start capture\n");
#endif

	buf_ctrl->next_xfer = 1;
	uac->xfer_state = UAC_STATE_TRANSFER;
	pipe->xfer_state = USBH_EP_XFER_START;
	usbh_notify(uac->host, pipe->pipe_num, &usbh_uac_driver);
}

/**
  * @brief  Stop isochronous IN (recording) transfers by clearing the next_xfer flag.
  * @retval void
  */
void usbh_uac_stop_capture(void)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_in.buf_ctrl);

	buf_ctrl->next_xfer = 0;

	if (buf_ctrl->sema_valid && buf_ctrl->wait_sema) {
		usb_os_sema_give(buf_ctrl->isoc_sema);
	}

	if (uac->isoc_in.as_itf != NULL) {
		uac->isoc_in.as_itf->pipe.xfer_state = USBH_EP_XFER_IDLE;
	}

#if USBH_UAC_DEBUG
	usbh_uac_reset_test_cnt();
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
static int usbh_uac_check_dir(u8 dir)
{
	usbh_uac_t *uac = &usbh_uac;
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
static int usbh_uac_check_capability(u8 dir, u16 control_bit)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_ac_itf_info_t *ac = &uac->ac_isoc_desc;
	usbh_uac_fu_info_t *fu;
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
static int usbh_uac_sync_dispatch(u32 timeout_ms)
{
	usbh_uac_t *uac = &usbh_uac;
	const u32 slice = 100;
	int ret = HAL_TIMEOUT;
	u32 elapsed = 0;

	/* deinit could have torn these down between status_check and us. */
	if ((uac->ctrl_done_sema == NULL) || (uac->host == NULL)) {
		uac->ctrl_state = UAC_STATE_CTRL_IDLE;
		return HAL_ERR_UNKNOWN;
	}

	uac->ctrl_status = HAL_BUSY;
	uac->xfer_state = UAC_STATE_TRANSFER;
	usb_os_sema_take(uac->ctrl_done_sema, 0); /* drain stale give from prior round */
	uac->ctrl_waiting = 1;

	usbh_notify(uac->host, 0x00, &usbh_uac_driver);

	while (elapsed < timeout_ms) {
		u32 t = ((timeout_ms - elapsed) > slice) ? slice : (timeout_ms - elapsed);
		if (usb_os_sema_take(uac->ctrl_done_sema, t) == HAL_OK) {
			ret = uac->ctrl_status;
			break;
		}
		elapsed += t;
		if (usbh_uac_usb_status_check() != HAL_OK) {
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
int usbh_uac_set_volume(u8 volume, u8 dir)
{
	usbh_uac_t *uac = &usbh_uac;
	int ret;

	if (usbh_uac_check_dir(dir) != HAL_OK) {
		return HAL_ERR_PARA;
	}
	if (usbh_uac_check_capability(dir, USB_UAC1_CONTROL_VOLUME) != HAL_OK) {
		return HAL_ERR_PARA;
	}
	if (usbh_uac_usb_status_check() != HAL_OK) {
		return HAL_BUSY;
	}

	usb_os_lock(uac->alt_set_mutex);

	uac->volume_value = (volume > 100) ? (100) : (volume);
	uac->ch_idx = 0;
	uac->cur_dir = dir;
	uac->ctrl_state = UAC_STATE_SET_VOLUME;
	ret = usbh_uac_sync_dispatch(1000);
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
int usbh_uac_set_mute(u8 mute, u8 dir)
{
	usbh_uac_t *uac = &usbh_uac;
	int ret;

	if (usbh_uac_check_dir(dir) != HAL_OK) {
		return HAL_ERR_PARA;
	}
	if (usbh_uac_check_capability(dir, USB_UAC1_CONTROL_MUTE) != HAL_OK) {
		return HAL_ERR_PARA;
	}
	if (usbh_uac_usb_status_check() != HAL_OK) {
		return HAL_BUSY;
	}

	usb_os_lock(uac->alt_set_mutex);

	uac->mute_value = (mute == 0) ? (0) : (1);
	uac->ch_idx = 0;
	uac->cur_dir = dir;
	uac->ctrl_state = UAC_STATE_SET_MUTE;
	ret = usbh_uac_sync_dispatch(1000);
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
static int usbh_uac_get_volume_info(usb_host_t *host)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	usbh_uac_fu_info_t *info = NULL;
	usbh_uac_as_itf_info_t *as_itf = NULL;
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
			ret = usbh_uac_get_unit_ctrl(host, info->bma_controls[uac->ch_idx], uac->ch_idx, USBH_UAC_ISOC_OUT_DIR);
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
			ret = usbh_uac_get_unit_ctrl(host, info->bma_controls[uac->ch_idx], uac->ch_idx, USBH_UAC_ISOC_IN_DIR);
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
			} else if (ret != HAL_BUSY) {
				/* SET_INTERFACE(alt 0) is confirmation, not a state change:
				 * SET_CONFIGURATION already left every interface at alternate
				 * setting 0 (USB 2.0 9.1.1.5/9.4.5), so a device that STALLs it
				 * is still in the zero-bandwidth state we asked for. Treat the
				 * rejection as success and move on - retrying cannot change the
				 * outcome and would stall the whole class. */
				RTK_LOGS(TAG, RTK_LOG_WARN, "OUT set alt0 err %d, ignore\n", ret);
				ret = HAL_BUSY;
				uac->init_state = UAC_INIT_IN_SET_ITF;
			} else {
				/* HAL_BUSY: control transfer still running. */
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
			} else if (ret != HAL_BUSY) {
				/* Idempotent request, see UAC_INIT_OUT_SET_ITF. */
				RTK_LOGS(TAG, RTK_LOG_WARN, "IN set alt0 err %d, ignore\n", ret);
				ret = HAL_BUSY;
				uac->init_state = UAC_INIT_DONE;
			} else {
				/* HAL_BUSY: control transfer still running. */
			}
		} else {
			ret = HAL_BUSY;
			uac->init_state = UAC_INIT_DONE;
		}
		break;

	case UAC_INIT_DONE:
	default:
		/* Scan/setup complete: clear the residual SCAN_MUTE (re-armed per channel
		 * in the OUT/IN GET_FU cases, or left by attach on the no-FU path). Without
		 * this, entering CLASS_READY with ctrl_state == SCAN_MUTE lets the async
		 * pipe-0 dispatch (CLASS_READY-entry fan-out / SOF self-notify) drive
		 * ctrl_setting() onto a stale SCAN state -> default -> spurious
		 * ctrl_finish(HAL_ERR_UNKNOWN), which corrupts a waiting set_alt. */
		uac->ctrl_state = UAC_STATE_CTRL_IDLE;
		ret = HAL_OK;
		break;
	}

	return ret;
}

/**
  * @brief  Check if playback is usable.
  *         Returns true only if the device exposes an OUT streaming interface
  *         AND the application reserved a non-zero ring-buffer depth via
  *         isoc_out_frm_cnt. The frame_cnt gate makes the answer match what
  *         set_alt_setting() can actually accomplish.
  * @return 1 if playback is supported and enabled, 0 otherwise.
  */
u8 usbh_uac_support_playback(void)
{
	usbh_uac_t *uac = &usbh_uac;
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
u8 usbh_uac_support_record(void)
{
	usbh_uac_t *uac = &usbh_uac;
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
int usbh_uac_get_mute(u8 dir, u8 ch, u8 *mute)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	usbh_uac_fu_info_t *info;
	int ret;

	if ((mute == NULL) || (ch > USBH_UAC_MAX_CHANNEL)) {
		return HAL_ERR_PARA;
	}
	if (usbh_uac_check_dir(dir) != HAL_OK) {
		return HAL_ERR_PARA;
	}

	info = &(ac_info->fu_controls[(dir == USBH_UAC_ISOC_IN_DIR) ? ac_info->in_best_idx : ac_info->out_best_idx]);
	if (ch > info->num_channels || !(info->bma_controls[ch] & USB_UAC1_CONTROL_MUTE)) {
		return HAL_ERR_PARA;
	}

	if (usbh_uac_usb_status_check() != HAL_OK) {
		return HAL_BUSY;
	}

	usb_os_lock(uac->alt_set_mutex);

	uac->cur_dir = dir;
	uac->get_ch = ch;
	uac->ctrl_state = UAC_STATE_GET_MUTE;
	ret = usbh_uac_sync_dispatch(1000);
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
int usbh_uac_get_volume(u8 dir, u8 ch, s16 *volume, s16 *vol_min, s16 *vol_max)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_ac_itf_info_t *ac_info = &(uac->ac_isoc_desc);
	usbh_uac_fu_info_t *info;
	usbh_uac_volume_info_t *vi;
	int ret;

	if ((volume == NULL) || (ch > USBH_UAC_MAX_CHANNEL)) {
		return HAL_ERR_PARA;
	}
	if (usbh_uac_check_dir(dir) != HAL_OK) {
		return HAL_ERR_PARA;
	}

	info = &(ac_info->fu_controls[(dir == USBH_UAC_ISOC_IN_DIR) ? ac_info->in_best_idx : ac_info->out_best_idx]);
	if (ch > info->num_channels || !(info->bma_controls[ch] & USB_UAC1_CONTROL_VOLUME)) {
		return HAL_ERR_PARA;
	}

	if (usbh_uac_usb_status_check() != HAL_OK) {
		return HAL_BUSY;
	}

	usb_os_lock(uac->alt_set_mutex);

	uac->cur_dir = dir;
	uac->get_ch = ch;
	uac->ctrl_state = UAC_STATE_GET_VOLUME;
	ret = usbh_uac_sync_dispatch(1000);
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
const usbh_uac_fu_info_t *usbh_uac_get_volume_ctrl_info(u8 dir)
{
	usbh_uac_t *uac = &usbh_uac;
	usbh_uac_ac_itf_info_t *ac = &(uac->ac_isoc_desc);
	u8 idx = (dir == USBH_UAC_ISOC_IN_DIR) ? ac->in_best_idx : ac->out_best_idx;

	if (idx == (u8) - 1) {
		return NULL;
	}
	return &(ac->fu_controls[idx]);
}
