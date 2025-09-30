/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_COMPOSITE_UAC1_H
#define USBH_COMPOSITE_UAC1_H

/* Includes ------------------------------------------------------------------*/
#include "usbh.h"
#include "usb_ch9.h"
#include "usb_uac1.h"
#include "usbh_composite_config.h"
#include <dlist.h>
#include "os_wrapper.h"

/* Exported defines ----------------------------------------------------------*/
#define USBH_UAC_TERM_MAX_CNT          8U
#define USBH_UAC_FU_MAX_CNT            8U
#define USBH_UAC_MAX_CHANNEL           8U

#define USBH_UAC_BIT_TO_BYTE           8U
#define USBH_UAC_ONE_KHZ               1000U

#define USBH_UAC_ISOC_OUT_DIR          0U
#define USBH_UAC_ISOC_IN_DIR           1U

#define USBH_UAC_ALT_SETTING_MAX       10U
#define USBH_UAC_FREQ_FORMAT_MAX       6U

#define USBH_UAC_SAMPLING_FREQ_CONTROL 0x100
#define USBH_UAC_VERSION_01_10         0x110

#define UBSH_UAC_AUDIO_FMT_FREQ_LEN    512U

/* Exported types ------------------------------------------------------------*/

/* States for class */
typedef enum {
	UAC_STATE_IDLE = 0U,
	UAC_STATE_TRANSFER,
	UAC_STATE_ERROR
} usbh_uac_xfer_state_t;

typedef enum {
	UAC_STATE_CTRL_IDLE = 0U,

	UAC_STATE_SET_OUT_ALT,
	UAC_STATE_SET_OUT_FREQ,
	UAC_STATE_SET_IN_ALT,
	UAC_STATE_SET_IN_FREQ,
	UAC_STATE_SET_VOLUME,
	UAC_STATE_SET_MUTE,
} usbh_uac_ctrl_state_t;

/* States for transfer */
typedef enum {
	USBH_UAC_EP_IDLE = 0U,
	USBH_UAC_XFER,
	USBH_UAC_XFER_BUSY,
} usbh_uac_ep_trx_state_t;

/* uac1.0 Audio Control   */
typedef struct {
	u16 terminal_type;
	u8 terminal_id;
	u8 source_id;
	u8 is_input;
} usbh_uac_term_info;

typedef struct {
	u8 channel_support[USBH_UAC_MAX_CHANNEL];
	u16 sink_type;
	u8 unit_id;
	u8 source_id;
	u8 sink_id;
	u8 num_channels;
	u8 master_support;
	u8 control_size;
} usbh_uac_vol_ctrl_info;

typedef struct {
	/* terminals array */
	usbh_uac_vol_ctrl_info controls[USBH_UAC_FU_MAX_CNT];
	usbh_uac_term_info terminals[USBH_UAC_TERM_MAX_CNT];

	u32 volume_ctrl_count;
	u32 terminal_count;
	u8 best_match_idx;
	u8 ac_itf_idx;
} usbh_uac_ac_itf_info_t;

typedef struct {
	struct list_head list;
	u8 *buf;
	__IO u16 buf_len;     /* buf valid len */
} usbh_uac_buf_t;

typedef struct {
	struct list_head list;
	usb_os_lock_t lock;
} usbh_uac_lock_list_head_t;

typedef struct {
	usbh_uac_lock_list_head_t free_buf_lock_list;
	usbh_uac_lock_list_head_t ready_buf_lock_list;
	rtos_sema_t isoc_sema;
	usbh_uac_buf_t *buf_list_node;
	usbh_uac_buf_t *ring_buf;      /* write/read buf,maybe just read/write part of the packet */
	usbh_uac_buf_t *xfer_buf;      /* buffer is xfer */
	u8 *isoc_buf;
	u32 remain_size;

	u32 sample_freq;               /* choose frequency */
	u32 sample_rem;                /* remainder from division sampling_freq/pkt_per_second */
	u32 sample_accum;              /* sample accumulator */
	u32 last_sample_accum;         /* sample accumulator */
	u32 pkt_per_second;            /* packets per second */

	u16 frame_cnt;
	__IO u16 mps;
	__IO u8 sema_valid;
	__IO u8 write_wait_sema;
	__IO u8 transfer_continue;
} usbh_uac_buf_ctrl_t;

/* Vendor user callback interface */
typedef struct {
	int(* init)(void);
	int(* deinit)(void);
	int(* attach)(void);
	int(* detach)(void);
	int(* setup)(void);
	int(* isoc_transmitted)(usbh_urb_state_t state);
	int(* isoc_received)(u8 *buf, u32 len);
} usbh_composite_uac_usr_cb_t;

/*uac descriptor struct*/
typedef struct {
	u8 *isoc_buf;
	u32 isoc_len;
	u32 isoc_interval;
	__IO u32 isoc_tick;
	u16 isoc_packet_size_small;    /* small packet sizes in samples */
	u16 isoc_packet_size;          /* large packet sizes in samples */
	u8 isoc_pipe;
	u8 isoc_ep_addr;
	__IO usbh_uac_ep_trx_state_t isoc_state;
} usbh_uac_ep_cfg_t;

typedef struct {
	u8 freq[USBH_UAC_FREQ_FORMAT_MAX][3];
	u8 bit_width;
	u8 channels;
	u8 freq_cnt;            /* frequency count */
} usbh_uac_format_cfg_t;

typedef struct {
	usbh_uac_format_cfg_t format_info;
	usbh_ep_desc_t ep_desc;

	u8 alt_setting;
} usbh_uac_as_itf_alt_info_t;

typedef struct {
	u32 sampling_freq;     /* 384000 */
	u8 bit_width;          /* 32 */
	u8 ch_cnt;             /* 128 */
} usbh_audio_fmt_t;

typedef struct {
	usbh_uac_as_itf_alt_info_t alt_set_array[USBH_UAC_ALT_SETTING_MAX];
	usbh_uac_ep_cfg_t ep_info;            /* ep cfg information */
	usbh_audio_fmt_t *fmt_array;          /* saving all audio format */

	u8 as_itf_num;                        /* as interface num */
	u8 alt_cnt;                           /* as alt_setting count */
	u8 fmt_array_cnt;                     /* fmt_array item count */
	u8 choose_alt_idx;                    /* choose alt setting index, not save the alt_set=0,so the real alt_idx=choose_alt_idx+1 */
	u8 choose_freq_idx;                   /* choose freq index */
} usbh_uac_as_itf_info_t;

/* UAC host */
typedef struct {
	usbh_uac_ac_itf_info_t isoc_ac_info;
	usbh_uac_as_itf_info_t *isoc_out_info;
	usbh_uac_as_itf_info_t *isoc_in_info;

	usbh_uac_buf_ctrl_t isoc_out;            /* isoc out */

	usbh_composite_uac_usr_cb_t *cb;

	usbh_composite_host_t *driver;

	u8 *audio_ctrl_buf;
	__IO u32 cur_frame;

#if USBH_COMPOSITE_HID_UAC_DEBUG
	rtos_task_t dump_status_task;
	__IO u8 dump_status_task_alive;
	__IO u8 dump_status_task_exit;
#endif

	__IO usbh_uac_xfer_state_t xfer_state;   /* xfer status */
	__IO usbh_uac_ctrl_state_t ctrl_state;   /* ctrl xfer status */
	u16 volume_info;                         /* volume db */
	u8 ch_idx;                               /* volume channale */
	u8 mute;                                 /* 1 mute, 0 unmute */
	u8 next_xfer;                            /*send next event flag*/
} usbh_composite_uac_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
extern const usbh_class_driver_t usbh_composite_uac_driver;

/* Exported functions --------------------------------------------------------*/

int usbh_composite_uac_init(usbh_composite_host_t *chost, usbh_composite_uac_usr_cb_t *cb, int frame_cnt);
int usbh_composite_uac_deinit(void);

int usbh_composite_uac_set_alt_setting(u8 dir, u8 channels, u8 bit_width, u32 sampling_freq);
const usbh_audio_fmt_t *usbh_composite_uac_get_alt_setting(u8 dir, u8 *fmt_cnt);
u32 usbh_composite_uac_get_frame_size(u8 dir);

int usbh_composite_uac_set_volume(u8 volume, u8 ch);
int usbh_composite_uac_set_mute(u8 mute);

u32 usbh_composite_uac_write(u8 *buffer, u32 size, u32 timeout_ms);
void usbh_composite_uac_start_play(void);
void usbh_composite_uac_stop_play(void);
#endif  /* USBH_COMPOSITE_UAC1_H */

