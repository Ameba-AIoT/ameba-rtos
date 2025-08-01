/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_UAC1_H
#define USBH_UAC1_H

/* Includes ------------------------------------------------------------------*/
#include "usbh.h"
#include "usb_ch9.h"
#include "usb_uac1.h"

/* Exported defines ----------------------------------------------------------*/
#define USBH_UAC_ISOC_OUT_IDX          0
#define USBH_UAC_ISOC_IN_IDX           1
#define USBH_UAC_ISOC_NUM              2

#define USBH_UAC_ALT_SETTING_MAX       6
#define USBH_UAC_FREQ_FORMAT_MAX       6

#define USBH_UAC_SAMPLING_FREQ_CONTROL 0x100
#define USBH_UAC_VERSION_01_10         0x110

#define UBSH_UAC_ALIGN4(x)             (((x) + 3) & ~3)
#define UBSH_UAC_CAL_FRAME(n, d)       UBSH_UAC_ALIGN4((((n) + (d) - 1) / (d)))
/* Exported types ------------------------------------------------------------*/

/* States for class */
typedef enum {
	UAC_STATE_IDLE = 0U,
	UAC_STATE_CTRL_SETTING,
	UAC_STATE_TRANSFER,
	UAC_STATE_ERROR
} usbh_uac_state_t;

typedef enum {
	UAC_STATE_CTRL_IDLE = 0U,
	UAC_STATE_SET_OUT_ALT,
	UAC_STATE_SET_OUT_FREQ,
	UAC_STATE_SET_IN_ALT,
	UAC_STATE_SET_IN_FREQ,
	UAC_STATE_SET_VOLUME,
	UAC_STATE_SET_MUTE,
	UAC_STATE_GET_OUT_ALT,
	UAC_STATE_GET_OUT_FREQ,
	UAC_STATE_GET_IN_ALT,
	UAC_STATE_GET_IN_FREQ,
	UAC_STATE_GET_VOLUME,
	UAC_STATE_GET_MUTE
} usbh_uac_ctrl_state_t;

/* States for transfer */
typedef enum {
	UAC_TRANSFER_STATE_IDLE = 0U,
	UAC_TRANSFER_STATE_PROCESS,
	UAC_TRANSFER_STATE_PROCESS_BUSY,
} usbh_uac_transfer_state_t;

/* Vendor user callback interface */
typedef struct {
	int(* init)(void);
	int(* deinit)(void);
	int(* attach)(void);
	int(* detach)(void);
	int(* setup)(void);
	int(* isoc_transmitted)(usbh_urb_state_t state);
	int(* isoc_received)(u8 *buf, u32 length);
} usbh_uac_cb_t;

/*uac descriptor struct*/

/*AC:audio control*/
typedef struct {
	void *desc;
	u8 intf_num;
} usbh_uac_ac_t;

/*AS:audio streaming*/
typedef struct {
	void *desc;
	usbh_if_desc_t *if_desc;
	struct uac1_as_header_descriptor *header;
	struct uac_format_type_i_discrete_descriptor *format;
	struct usb_audio_endpoint_descriptor *audio_ep;
	struct uac_iso_endpoint_descriptor *iso_ep;
} usbh_uac_alt_t;

/*descriptor alt tree*/
typedef struct {
	usbh_uac_alt_t alt_setting[USBH_UAC_ALT_SETTING_MAX];//max
	void *desc;
	u8 intf_num;
	u8 alt_count;// exclude alt 0
	u8 msg_type;
} usbh_uac_as_t;

/*descriptor tree*/
typedef struct {
	usbh_uac_ac_t ac_intf;
	usbh_uac_as_t as_intf[USBH_UAC_ISOC_NUM];
	u8 as_count;
	u8 cur_as_index[USBH_UAC_ISOC_NUM];
} usbh_uac_cfg_t;

typedef struct {
	u8 *isoc_buf;
	u32 isoc_len;
	u32  isoc_interval;
	__IO u32 isoc_tick;
	usbh_uac_transfer_state_t isoc_state;
	u16 isoc_packet_size;
	u8  isoc_pipe;
	u8  isoc_ep_addr;
} usbh_uac_ep_cfg_t;

typedef struct {
	u8 format_type; // audio support PCM/ ?
	u8 bit_width;
	u8 channels;
	u8 sam_freq_type;
	u8 format_freq[USBH_UAC_FREQ_FORMAT_MAX][3];
} usbh_uac_format_cfg_t;

typedef struct {
	usbh_uac_format_cfg_t format_info;
	usbh_uac_ep_cfg_t ep_info;
	usbh_uac_alt_t *cur_alt_setting; //alt1/alt2
	u8 intf_num;
	u8 alt_setting;
	u8 freq_id;
} usbh_uac_setting_t;

/* Vendor host */
typedef struct {
	usbh_uac_cfg_t uac_desc;
	usbh_uac_setting_t cur_setting[USBH_UAC_ISOC_NUM]; // cur alt
	int setting_count[USBH_UAC_ISOC_NUM]; // alt count
	usbh_uac_state_t state;
	usbh_uac_ctrl_state_t sub_state;
	usbh_uac_cb_t *cb;
	usb_host_t *host;
	u8 next_transfor;/*send next event flag*/
	__IO u32 cur_frame;
} usbh_uac_host_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

int usbh_uac_init(usbh_uac_cb_t *cb);
int usbh_uac_deinit(void);
int usbh_uac_isoc_transmit(u8 *buf, u32 len);
int usbh_uac_isoc_receive(u8 *buf, u32 len);

int usbh_uac_setup_out(u8 id);
int usbh_uac_setup_in(u8 id);
int usbh_uac_set_alt_setting(u8 intf_idx, u8 channels, u8 byte_width, u32 sampling_freq);
const usbh_uac_alt_t *usbh_uac_get_alt_setting(u8 intf_idx, u8 *alt_num);
u32 usbh_uac_get_frame_size(u8 intf_idx);
void usbh_uac_stop_play(void);
#endif  /* USBH_UAC1_H */

