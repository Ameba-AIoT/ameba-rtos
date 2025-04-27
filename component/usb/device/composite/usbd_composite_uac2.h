/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_COMPOSITE_UAC2_H
#define USBD_COMPOSITE_UAC2_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usbd_composite_config.h"

/* Exported defines ----------------------------------------------------------*/
/* used for uac debug*/
#define USBD_UAC_ISOC_XFER_DEBUG                    0

/*
* while the host does not send the audio data(for example: switch to next song), the usb should append zero packet to the buffer
*
*/
#define USBD_UAC_ISOC_RX_APPEND_ZLP_DIFF_MAX        3

/* 1ms 8 frame in high speed */
#define USBD_UAC_HS_SOF_COUNT_PER_MS                8U
#define USBD_UAC_ONE_KHZ                            1000U

#define USBD_UAC_RX_BUF_MAX_CNT                     20
#define USBD_UAC_TX_BUF_MAX_CNT                     10

#define USBD_UAC_HS_ISOC_MPS                        1024   /* High speed ISOC IN & OUT max packet size */
#define USBD_UAC_FS_ISOC_MPS                        1023   /* Full speed ISOC IN & OUT max packet size */

/* volume */
/* spec 0x8001U ~ 0X7FFFU , -127dB ~ 127dB */
#define USBD_UAC_VOLUME_CTRL_MIN                    0xFF42 // -190
#define USBD_UAC_VOLUME_CTRL_MAX                    0x00BE // 190

// Project-specific Interface Indices
#define USBD_UAC_LEN_CLK_SRC_DESC                   0x08U
#define USBD_UAC_LEN_AC_IN_TTY_DESC                 0x11U
#define USBD_UAC_LEN_AC_OUT_TTY_DESC                0x0CU
#define USBD_UAC_LEN_AS_IF_ALT_SET_DESC             0x10U
#define USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC       0x06U
#define USBD_UAC_LEN_DATA_EP_DESC                   0x08U
#define USBD_UAC_LEN_CTRL_IF_HEADER                 0x09U
#define USBD_UAC_LEN_FUNC_UNIT_DESC                 0x09U

/* usbd uac */
#define USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE       0x24U
#define USBD_UAC_CS_ENDPOINT_DESCRIPTOR             0x25U
#define USBD_UAC_IA_DESCRIPTOR                      0x0BU

// Audio Function Category Codes (UAC2.0 Spec Appendix A.7)
#define USBD_UAC_FUNC_CATEGORY_CODE_DESKTOP_UNDEFINED              0x00U
#define USBD_UAC_FUNC_CATEGORY_CODE_DESKTOP_SPEAKER                0x01U
#define USBD_UAC_FUNC_CATEGORY_CODE_DESKTOP_HEADSET                0x04U

// Audio Class-Specific Audio Control Interface Descriptor Subtype Codes (UAC2.0 Spec Appendix A.9)
#define USBD_UAC_AC_IF_DESC_SUBTYPE_UNDEFINED                      0x00U
#define USBD_UAC_AC_IF_DESC_SUBTYPE_HEADER                         0x01U
#define USBD_UAC_AC_IF_DESC_SUBTYPE_INPUT_TERMINAL                 0x02U
#define USBD_UAC_AC_IF_DESC_SUBTYPE_OUTPUT_TERMINAL                0x03U
#define USBD_UAC_AC_IF_DESC_SUBTYPE_FEATURE_UNIT                   0x06U
#define USBD_UAC_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE                   0x0AU

// USB Audio terminal
// USB In
#define USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE            0x12U
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE    0x02U
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT           0x08U
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE   0x10U
// USB Out
#define USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES            0x15U
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES    0x01U
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT          0x05U
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES   0x09U

// (UAC2.0 Spec Appendix A-14)
#define USBD_UAC_CLASS_REQ_CODE_CUR                                0x01U
#define USBD_UAC_CLASS_REQ_CODE_RANGE                              0x02U
// (UAC2.0 Spec Appendix A-17)
#define USBD_UAC_CS_SAM_FREQ_CONTROL                               0x01U
#define USBD_UAC_CS_CLK_VALID_CONTROL                              0x02U
// (UAC2.0 Spec Appendix A-20)
#define USBD_UAC_TE_CONNECTOR_CONTROL                              0x02U

/*! @brief Commands for USB device AUDIO control feature unit control selector */
#define USBD_UAC_CTRL_FU_MUTE_CONTROL_SELECTOR                     0x01U
#define USBD_UAC_CTRL_FU_VOLUME_CONTROL_SELECTOR                   0x02U
#define USBD_UAC_CTRL_FU_BASS_CONTROL_SELECTOR                     0x03U
#define USBD_UAC_CTRL_FU_MID_CONTROL_SELECTOR                      0x04U
#define USBD_UAC_CTRL_FU_TREBLE_CONTROL_SELECTOR                   0x05U
#define USBD_UAC_CTRL_FU_GRAPHIC_EQUALIZER_CONTROL_SELECTOR        0x06U
#define USBD_UAC_CTRL_FU_AUTOMATIC_GAIN_CONTROL_SELECTOR           0x07U
#define USBD_UAC_CTRL_FU_DELAY_CONTROL_SELECTOR                    0x08U
#define USBD_UAC_CTRL_FU_BASS_BOOST_CONTROL_SELECTOR               0x09U
#define USBD_UAC_CTRL_FU_LOUDNESS_CONTROL_SELECTOR                 0x0AU
#define USBD_UAC_CTRL_FU_INPUT_GAIN_CONTROL_SELECTOR               0x0BU
#define USBD_UAC_CTRL_FU_INPUT_GAIN_PAD_CONTROL_SELECTOR           0x0CU
#define USBD_UAC_CTRL_FU_PHASE_INVERTER_CONTROL_SELECTOR           0x0DU
#define USBD_UAC_CTRL_FU_UNDERFLOW_CONTROL_SELECTOR                0x0EU
#define USBD_UAC_CTRL_FU_OVERFLOW_CONTROL_SELECTOR                 0x0FU
#define USBD_UAC_CTRL_FU_LATENCY_CONTROL_SELECTOR                  0x10U

// Audio Class-Specific Audio Streaming Interface Descriptor Subtype Codes (UAC2.0 Spec Appendix A.10)
#define USBD_UAC_AS_IF_DESC_SUBTYPE_UNDEFINED               0x00
#define USBD_UAC_AS_IF_DESC_SUBTYPE_AS_GENERAL              0x01
#define USBD_UAC_AS_IF_DESC_SUBTYPE_FORMAT_TYPE             0x02

// Audio Class-Specific Audio Streaming Endpoint Descriptor Subtype Codes (UAC2.0 Spec Appendix A.13)
#define USBD_UAC_AS_EP_DESC_SUBTYPE_UNDEFINED               0x00
#define USBD_UAC_AS_EP_DESC_SUBTYPE_EP_GENERAL              0x01

/* Uac Device parameters */
#define USBD_UAC_VID                                USB_VID
#define USBD_UAC_PID                                USB_PID
#define USBD_UAC_CLASS_CODE                         0x01U
#define USBD_UAC_SUBCLASS_AUDIOCONTROL              0x01U
#define USBD_UAC_SUBCLASS_AUDIOSTREAMING            0x02U
#define USBD_UAC_VERSION_02_00                      0x20U
#define USBD_UAC_IF_CLASS_AUDIO                     0x01U

#define USBD_UAC_SELF_POWERED                       1U
#define USBD_UAC_LANGID_STRING                      0x0409U
#define USBD_UAC_MFG_STRING                         "Realtek"
#define USBD_UAC_PROD_HS_STRING                     "Realtek UAC2.0 (HS)"
#define USBD_UAC_PROD_FS_STRING                     "Realtek UAC2.0 (FS)"
#define USBD_UAC_SN_STRING                          "1234567890"


/* modify for constructor the description */
/* config */
#define USBD_UAC_CFG_LEN_OFFSET                     2U
#define USBD_UAC_CFG_IF_CNT_OFFSET                  4U
/* Audio Control */
#define USBD_UAC_ASSOCIATION_IF_NUM_OFFSET          12U
#define USBD_UAC_AC_IF_HEADER_LEN_OFFSET            32U
#define USBD_UAC_IT_DESC_CH_CNT_OFFSET              8U
#define USBD_UAC_IT_DESC_CH_CFG_OFFSET              9U
#define USBD_UAC_OT_DESC_TYPE_OFFSET                4U
/* Audio Streaming */
#define USBD_UAC_AS_IF_DESC_NUM_OFFSET              2U
#define USBD_UAC_AS_IF_DESC_ALT_OFFSET              3U
#define USBD_UAC_AS_IF_DESC_EP_OFFSET               4U
#define USBD_UAC_AS_IF_DESC_CH_CNT_OFFSET           10U
#define USBD_UAC_AS_IF_DESC_CH_OFFSET               11U
#define USBD_UAC_AS_FORMAT_DESC_SLOT_SIZE_OFFSET    4U
#define USBD_UAC_AS_FORMAT_DESC_BIT_CNT_OFFSET      5U
#define USBD_UAC_AS_EP_DESC_MPS_OFFSET              4U

/* channel cnt */
#define USBD_UAC_CH_CNT_2                           2U
#define USBD_UAC_CH_CNT_4                           4U
#define USBD_UAC_CH_CNT_6                           6U
#define USBD_UAC_CH_CNT_8                           8U
/* to do: 16 ch support */
//#define USBD_UAC_CH_CNT_16                        16U

/* byte_width */
#define USBD_UAC_BYTE_WIDTH_2                       2U
#define USBD_UAC_BYTE_WIDTH_3                       3U
/* amebasmart itself cannot play 32-bit audio data */
#define USBD_UAC_BYTE_WIDTH_4                       4U

/* bit_width */
#define USBD_UAC_BIT_WIDTH(byte_width)              (8U * (byte_width))

#define USBD_UAC_POW2(n)                            (1 << (n))

/* get channel config */
#define USBD_UAC_GET_CH_CONFIG(ch_cnt) \
    ((ch_cnt) == 2 ? 0x03 : \
     (ch_cnt) == 4 ? 0x0F : \
     (ch_cnt) == 6 ? 0x3F : \
     (ch_cnt) == 8 ? 0xFF : \
     (ch_cnt) == 16 ? 0xFFFF : 0x03)

/* get ot type */
#define USBD_UAC_GET_OT_TYPE(ch_cnt) \
    ((ch_cnt) == 2 ? 0x0301 : \
     (ch_cnt) == 4 ? 0x0304 : \
     (ch_cnt) == 6 ? 0x0304 : \
     (ch_cnt) == 8 ? 0x0307 : \
     (ch_cnt) == 16 ? 0x0307 : 0x0301)

/* input terminal */
#define USBD_UAC_CH_CONFIG_TYPE_LOW(ch_cnt)         (USB_LOW_BYTE(USBD_UAC_GET_CH_CONFIG(ch_cnt)))
#define USBD_UAC_CH_CONFIG_TYPE_HIGH(ch_cnt)        (USB_HIGH_BYTE(USBD_UAC_GET_CH_CONFIG(ch_cnt)))

/* output terminal */
#define USBD_UAC_OT_DESC_TYPE_LOW(ch_cnt)           (USB_LOW_BYTE(USBD_UAC_GET_OT_TYPE(ch_cnt)))
#define USBD_UAC_OT_DESC_TYPE_HIGH(ch_cnt)          (USB_HIGH_BYTE(USBD_UAC_GET_OT_TYPE(ch_cnt)))

/* default channel cnt */
#define USBD_UAC_DEFAULT_CH_CNT                     USBD_UAC_CH_CNT_8

/* default byte width cnt */
#define USBD_UAC_DEFAULT_BYTE_WIDTH                 USBD_UAC_BYTE_WIDTH_2

/* sample sampling_freq max count */
#define USBD_UAC_SAMPLING_FREQ_MAX_COUNT            1U

/*
   * default isoc bInterval
   *        ISOC
   * FS    1-16,2^(N-1)
   * HS    1-16,2^(N-1)
   * for FS, use 1; for HS use 4, which will report data per second
*/
#define USBD_UAC_HS_DEFAULT_BINTERVAL                 1
#define USBD_UAC_FS_DEFAULT_BINTERVAL                 1

/* limit */
#define USBD_UAC_HS_SAMPLING_FREQ_COUNT               USBD_UAC_SAMPLING_FREQ_MAX_COUNT

#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4
#define USBD_UAC_FS_SAMPLING_FREQ_COUNT               1U
#elif USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
#define USBD_UAC_FS_SAMPLING_FREQ_COUNT               1U
#endif
#else
#define USBD_UAC_FS_SAMPLING_FREQ_COUNT               USBD_UAC_SAMPLING_FREQ_MAX_COUNT
#endif


/* AC IF header interface num */
#define USBD_UAC_AC_IF_NUM                          2U

/* AC feature uint descriptor length */
#define USBD_UAC_AC_FU_HEAD_DESC_LEN(ch_cnt)        (USBD_UAC_LEN_FUNC_UNIT_DESC + 1 + 4 * (ch_cnt))

/* AC IF header descriptor length */
#define USBD_UAC_AC_IF_HEAD_DESC_LEN(ch_cnt) \
    (USBD_UAC_LEN_CTRL_IF_HEADER + USBD_UAC_LEN_CLK_SRC_DESC + USBD_UAC_LEN_AC_IN_TTY_DESC + \
    USBD_UAC_AC_FU_HEAD_DESC_LEN(ch_cnt) + USBD_UAC_LEN_AC_OUT_TTY_DESC)

/* len of total Audio control interface */
#define USBD_UAC_AC_IF_LEN(ch_cnt)                 (USB_LEN_IF_DESC + USBD_UAC_AC_IF_HEAD_DESC_LEN(ch_cnt))

/* len of each Audio stream interface/altsetting (one EP) */
#define USBD_UAC_AS_EIF_LEN                        (USB_LEN_IF_DESC + USBD_UAC_LEN_AS_IF_ALT_SET_DESC + USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC \
                                                   + USB_LEN_EP_DESC + USBD_UAC_LEN_DATA_EP_DESC)

/* len of total Audio stream interface */
#define USBD_UAC_AS_TIF_LEN(alt_num) \
    (USBD_UAC_AS_EIF_LEN * (alt_num - 1) + USB_LEN_IF_DESC)

/* full speed AS alt setting num */
#define USBD_UAC_FS_AS_ALT_SETTING_NUM              5U

/* high speed AS alt setting num */
#define USBD_UAC_HS_AS_ALT_SETTING_NUM              5U

/* len of full speed total configuration descriptor buf */
#define USBD_UAC_FS_CFG_DESC_BUF_LEN(ch_cnt) \
    (USB_LEN_CFG_DESC + USB_LEN_IAD_DESC  + USBD_UAC_AC_IF_LEN(ch_cnt) + USBD_UAC_AS_TIF_LEN(USBD_UAC_FS_AS_ALT_SETTING_NUM))

/* len of high speed total configuration descriptor buf */
#define USBD_UAC_HS_CFG_DESC_BUF_LEN(ch_cnt) \
    (USB_LEN_CFG_DESC + USB_LEN_IAD_DESC  + USBD_UAC_AC_IF_LEN(ch_cnt) + USBD_UAC_AS_TIF_LEN(USBD_UAC_HS_AS_ALT_SETTING_NUM))

/* sampling freq */
#define USBD_UAC_SAMPLING_FREQ_44K                              44100U
#define USBD_UAC_SAMPLING_FREQ_48K                              48000U
#define USBD_UAC_SAMPLING_FREQ_96K                              96000U
#define USBD_UAC_SAMPLING_FREQ_192K                             192000U

/* calculate full speed MPS */
#define USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, sampling_freq_hz) \
    (USBD_UAC_POW2(USBD_UAC_FS_DEFAULT_BINTERVAL-1) * (ch_cnt) * (byte_width) * ((sampling_freq_hz) / USBD_UAC_ONE_KHZ + 1U))

/* check MPS */
#define USBD_UAC_IS_FS_MPS_VALID(ch_cnt, byte_width, sampling_freq_hz) \
    ((USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, sampling_freq_hz)) <= USBD_UAC_FS_ISOC_MPS)

/* get full speed MPS, if MPS > limit, choose next lower sampling freq to calculate */
#define USBD_UAC_GET_FS_MPS(ch_cnt, byte_width) \
    (USBD_UAC_IS_FS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_192K) ? \
    USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_192K) : \
    (USBD_UAC_IS_FS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_96K) ? \
    USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_96K) : \
    (USBD_UAC_IS_FS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_48K) ? \
    USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_48K) : \
    (USBD_UAC_IS_FS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_44K) ? \
    USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_44K) : 0))))

/* calculate high speed MPS */
#define USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, sampling_freq_hz) \
  (USBD_UAC_POW2(USBD_UAC_HS_DEFAULT_BINTERVAL-1) * (ch_cnt) * (byte_width) * ((sampling_freq_hz) / USBD_UAC_ONE_KHZ / USBD_UAC_HS_SOF_COUNT_PER_MS + 1U))

/* check MPS */
#define USBD_UAC_IS_HS_MPS_VALID(ch_cnt, byte_width, sampling_freq_hz) \
    ((USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, sampling_freq_hz)) <= USBD_UAC_HS_ISOC_MPS)

/* get high speed MPS, if MPS > limit, choose next lower sampling freq to calculate */
#define USBD_UAC_GET_HS_MPS(ch_cnt, byte_width) \
    (USBD_UAC_IS_HS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_192K) ? \
    USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_192K) : \
    (USBD_UAC_IS_HS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_96K) ? \
    USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_96K) : \
    (USBD_UAC_IS_HS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_48K) ? \
    USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_48K) : \
    (USBD_UAC_IS_HS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_44K) ? \
    USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_44K) : 0))))

#define USBD_UAC_INIT_SUB_RANGE(sub_range, min_sampling_freq, max_sampling_freq, res) \
    do {                                               \
        (sub_range).dMIN = (min_sampling_freq);        \
        (sub_range).dMAX = (max_sampling_freq);        \
        (sub_range).dRES = (res);                      \
    } while(0)

/* Exported types ------------------------------------------------------------*/

typedef struct __PACKED {
	u16 wNumSubRanges;
	u16 wMIN;
	u16 wMAX;
	u16 wRES;
} usbd_composite_uac_ctrl_range_layout2_struct;

typedef struct {
	u32 dMIN;
	u32 dMAX;
	u32 dRES;
} usbd_composite_uac_sub_range_t;

typedef struct __PACKED {
	u16 wNumSubRanges;
	usbd_composite_uac_sub_range_t usbd_composite_uac_sub_ranges[];
} usbd_composite_uac_sampling_freq_ctrl_range_t;

typedef struct __PACKED {
	u8  bNrChannels;
	u32 bmChannelConfig;
	u8  iChannelNames;
} usbd_composite_uac_ac_connect_ctrl_t;

typedef struct {
	u32 sampling_freq;     // 384000
	u8 byte_width;   // 4
	u8 ch_cnt;       // 128
	u8 enable;
} usbd_audio_cfg_t;

typedef struct {
	usbd_audio_cfg_t in;
	usbd_audio_cfg_t out;
	void *audio_ctx;
	int(* init)(void);
	int(* deinit)(void);
	int(* setup)(usb_setup_req_t *req, u8 *buf);
	int(* set_config)(void);
	int(* status_changed)(u8 status);
	int(* mute_changed)(u8 mute);
	int(* volume_changed)(u8 volume);
	int(* format_changed)(u32 sampling_freq, u8 ch_cnt, u8 byte_width);
	int(* sof)(void);
} usbd_composite_uac_usr_cb_t;

struct _usbd_composite_uac_buf_t;
typedef struct _usbd_composite_uac_buf_t {
	struct _usbd_composite_uac_buf_t *next;
	u8 *buf_raw;
	__IO u16 buf_valid_len;
	__IO u8 is_zero_pkt;      /* the buf is zero packet */
} usbd_composite_uac_buf_t;

typedef struct {
	usbd_composite_uac_buf_t *head;
	usbd_composite_uac_buf_t *tail;
	u16 count;
} usbd_composite_uac_buf_list_t;

typedef struct {
	usbd_audio_cfg_t audio_config;  /* save the audio config */
	rtos_sema_t uac_isoc_sema;
	usbd_composite_uac_buf_list_t empty_list;
	usbd_composite_uac_buf_list_t data_list;

	usbd_composite_uac_buf_t *buf_list_node;
	usbd_composite_uac_buf_t *p_cur_buf_node;
	u8 *isoc_buf;

	u64 sof_idx;
	u64 data_idx;

	__IO u16 isoc_mps; //mps
	__IO u8 uac_sema_valid : 1;
	__IO u8 read_wait_sema : 1;
	__IO u8 transfer_continue : 1;
} usbd_composite_uac_buf_ctrl_t;

typedef struct {
	usbd_composite_uac_buf_ctrl_t uac_isoc_out;  /* isoc out */
	usbd_composite_uac_buf_ctrl_t uac_isoc_in;   /* isoc in  */

	usb_setup_req_t ctrl_req;
	usbd_composite_dev_t *cdev;
	usbd_composite_uac_usr_cb_t *cb;

	__IO u32 isoc_rx_valid_cnt;
	__IO u32 isoc_rx_len;

#if USBD_UAC_ISOC_XFER_DEBUG
	rtos_task_t uac_dump_task;
	u32 isoc_rx_last_tick;
	u32 isoc_timeout_max_step;
	u32 isoc_timeout_max_value;

	u32 isoc_zlp_cnt;
	u32 isoc_rx_cnt;
	u32 isoc_rx_zlp_cnt;
	u32 copy_data_len;
	__IO u32 isoc_timeout_cnt;
	__IO u32 isoc_overwrite_cnt;
	__IO u32 uac_dump_task_alive;
#endif

	u16 cur_volume;
	u8 cur_clk_valid;
	u8 alt_setting;
	u8 cur_mute;
	__IO u8 is_ready;
} usbd_composite_uac_device_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

extern usbd_class_driver_t usbd_composite_uac_driver;

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Initialize UAC device
  * @param  cb: usbd_uac_cb_t instance passed by the upper layer
  * @retval Status : return HAL_OK success,else other value
  */
int usbd_composite_uac_init(usbd_composite_dev_t *cdev, usbd_composite_uac_usr_cb_t *cb);

/**
  * @brief  DeInitialize UAC device
  * @retval Status
  */
int usbd_composite_uac_deinit(void);


/**
  * @brief  Set audio configuration to the UAC class
  * @param  uac_cfg: write buffer handle used to save the data
  * @param  is_record: write buffer handle used to save the data
  * @param  flag: write buffer handle used to save the data
  * @retval Status
  */
u8 usbd_composite_uac_config(const usbd_audio_cfg_t *uac_cfg, u8 is_record, u32 flag);

/**
  * @brief  Usb audio start play
  * @retval Status
  */
u32 usbd_composite_uac_start_play(void);

/**
  * @brief  Usb audio stop play
  * @retval Status
  */
void usbd_composite_uac_stop_play(void);

/**
  * @brief  Read the data from the ring_buffer
  * @param  buffer: write buffer handle used to save the data
  * @param  size: allow to write buffer size
  * @param  time_out_ms if 0 means no wait,
  *                     other value will set time_out_ms to read buffer,if read data for more than time_out_ms, it will return.
  * @param  zero_pkt_flag: zero packet ,each bit means a zero packet
  * @retval return the read data length in bytes
  */
u32 usbd_composite_uac_read(u8 *buffer, u32 size, u32 time_out_ms, u32 *zero_pkt_flag);

/**
  * @brief  Get the read frame count
  * @retval return avail frame cnt
  */
u32  usbd_composite_uac_get_read_frame_cnt(void);

/**
  * @brief  Get the read frame count
  * @retval return the time duration for the avail frame in us
  */
u32  usbd_composite_uac_get_read_frame_time_in_us(void);

// for USB IN tx, TODO
// u32  usbd_uac_write(void, void *buffer, u32 size, u8 blocking);
// u32  usbd_uac_get_write_frame_cnt(void);


#endif // USBD_COMPOSITE_UAC_H