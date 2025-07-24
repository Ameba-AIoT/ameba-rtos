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
#define USBH_UAC_ISOC_OUT_IDX (0)
#define USBH_UAC_ISOC_IN_IDX  (1)
#define USBH_UAC_ISOC_NUM     (2)

#define USBH_UAC_ALT_SETTING_MAX (6)
#define USBH_UAC_FREQ_FORMAT_MAX (6)

#define USBH_UAC_SAMPLING_FREQ_CONTROL  (0x100)
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
	void *p;
	u8 bInterfaceNumber;
} uac_ac_t;

/*AS:audio streaming*/
typedef struct {
	void *p;
	usbh_if_desc_t *desc;
	struct uac1_as_header_descriptor *header;
	struct uac_format_type_i_discrete_descriptor *format;
	struct usb_audio_endpoint_descriptor *audio_ep;
	struct uac_iso_endpoint_descriptor *iso_ep;
} uac_alt_t;

/*descriptor alt tree*/
typedef struct {
	void *p;
	u8 bInterfaceNumber;
	u8 alt_count;// exclude alt 0
	u8 msg_type;
	uac_alt_t altsetting[USBH_UAC_ALT_SETTING_MAX];//max
} uac_as_t;

/*descriptor tree*/
typedef struct {
	uac_ac_t ac_intf;
	uac_as_t as_intf[USBH_UAC_ISOC_NUM];
	u8 as_count;
	u8 cur_as_index[USBH_UAC_ISOC_NUM];
} uac_cfg_t;

typedef struct {
	u8 *isoc_buf;
	u32 isoc_len;
	u8  isoc_pipe;
	u8  isoc_ep_addr;
	u16 isoc_packet_size;
	u32  isoc_interval;
	__IO u32 isoc_tick;
	usbh_uac_transfer_state_t isoc_state;
} uac_ep_cfg_t;

typedef struct {
	u8 fu_id;
	u8 ch_num;
	u8 mute;
	u8 volume;
} uac_fu_cfg;

typedef struct {
	u8 bFormatType; // audio support PCM/ ?
	u8 bBitResolution;
	u8 bNrChannels;
	u8 bSubframeSize;
	u8 bSamFreqType;
	u8 format_freq[USBH_UAC_FREQ_FORMAT_MAX][3];
} uac_format_cfg_t;

typedef struct {
	uac_format_cfg_t format_info;
	uac_ep_cfg_t ep_info;
	uac_alt_t *cur_altsetting; //alt1/alt2
	u8 bInterfaceNumber;
	u8 bAlternateSetting;
	u8 bFreqId;
} uac_setting_t;

/* Vendor host */
typedef struct {
	uac_cfg_t uac_desc;
	uac_setting_t cur_setting[USBH_UAC_ISOC_NUM]; // cur alt
	uac_setting_t setting_arrays[USBH_UAC_ISOC_NUM][USBH_UAC_ALT_SETTING_MAX]; // save in/out all alt
	int setting_count[USBH_UAC_ISOC_NUM]; // alt count
	usbh_uac_state_t state;
	usbh_uac_ctrl_state_t sub_state;
	usbh_uac_cb_t *cb;
	usb_host_t *host;
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
int usbh_uac_find_altsetting(u8 channels, u8 bytewidth, u32 sample_freq);
void usbh_uac_stop_play(void);
#endif  /* USBH_UAC1_H */

