/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

/* uac 2.0 spec */
#include "usbd_composite_uac.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/

#if USBD_COMPOSITE_UAC_DEBUG
#define USBD_UAC_DEBUG_LOOP_TIME   1000
#endif

#define UABD_UAC_VOL_ERR_VAL       255

#define USBD_UAC_HS_ISOC_MPS                        1024   /* High speed ISOC IN & OUT max packet size */
#define USBD_UAC_FS_ISOC_MPS                        1023   /* Full speed ISOC IN & OUT max packet size */

/* Uac Device parameters */
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

/* Timing and Frequency Constants */
#define USBD_UAC_HS_SOF_COUNT_PER_MS                8U         /**< Number of SOF packets per millisecond in High-Speed mode. */
#define USBD_UAC_ONE_KHZ                            1000U      /**< Constant for 1 kHz. */

#define USBD_UAC_RX_BUF_MAX_CNT                     20         /**< Maximum number of receive buffers. */
#define USBD_UAC_TX_BUF_MAX_CNT                     10         /**< Maximum number of transmit buffers. */

/**
 * @name Volume Control Range
 * @details Defines the min/max volume range. The UAC spec allows 0x8001 to 0x7FFF
 *          (-127dB to +127dB), but this range is narrowed to avoid warnings on some OS.
 */
/* narrow down range for linux volume range warning issue */
#define USBD_UAC_VOLUME_CTRL_MIN                    0xFF42     /**< Minimum volume level (-190, corresponds to -190/256 dB). */
#define USBD_UAC_VOLUME_CTRL_MAX                    0x00BE     /**< Maximum volume level (190, corresponds to 190/256 dB). */

// Project-specific Interface Indices
#define USBD_UAC_LEN_CLK_SRC_DESC                   0x08U
#define USBD_UAC_LEN_AC_IN_TTY_DESC                 0x11U
#define USBD_UAC_LEN_AC_OUT_TTY_DESC                0x0CU
#define USBD_UAC_LEN_AS_IF_ALT_SET_DESC             0x10U
#define USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC       0x06U
#define USBD_UAC_LEN_DATA_EP_DESC                   0x08U
#define USBD_UAC_LEN_CTRL_IF_HEADER                 0x09U
#define USBD_UAC_LEN_FUNC_UNIT_DESC                 0x09U

/* UAC Descriptor and Request Codes */
#define USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE       0x24U      /**< Descriptor Type: Class-Specific Audio Control Interface. */
#define USBD_UAC_CS_ENDPOINT_DESCRIPTOR             0x25U      /**< Descriptor Subtype: Class-Specific Endpoint Descriptor. */
#define USBD_UAC_IA_DESCRIPTOR                      0x0BU      /**< Descriptor Type: Interface Association Descriptor. */

// Audio Function Category Codes (UAC2.0 Spec Appendix A.7)
#define USBD_UAC_FUNC_CATEGORY_CODE_DESKTOP_UNDEFINED             0x00U
#define USBD_UAC_FUNC_CATEGORY_CODE_DESKTOP_SPEAKER               0x01U
#define USBD_UAC_FUNC_CATEGORY_CODE_DESKTOP_HEADSET               0x04U

// Audio Class-Specific Audio Control Interface Descriptor Subtype Codes (UAC2.0 Spec Appendix A.9)
#define USBD_UAC_AC_IF_DESC_SUBTYPE_UNDEFINED                     0x00U
#define USBD_UAC_AC_IF_DESC_SUBTYPE_HEADER                        0x01U
#define USBD_UAC_AC_IF_DESC_SUBTYPE_INPUT_TERMINAL                0x02U
#define USBD_UAC_AC_IF_DESC_SUBTYPE_OUTPUT_TERMINAL               0x03U
#define USBD_UAC_AC_IF_DESC_SUBTYPE_FEATURE_UNIT                  0x06U
#define USBD_UAC_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE                  0x0AU

// USB Audio terminal
/* USB Out Path (Host to Device, e.g., Speaker) */
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES   0x01U /**< Input Terminal (USB Streaming). */
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT         0x05U /**< Feature Unit. */
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES  0x09U /**< Output Terminal (Headphones). */
#define USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES           0x15U /**< Clock Source for Speaker path. */
/* USB In Path (Device to Host, e.g., Microphone) */
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE   0x02U /**< Input Terminal (Microphone). */
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT          0x08U /**< Feature Unit. */
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE  0x10U /**< Output Terminal (USB Streaming). */
#define USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE           0x12U /**< Clock Source for Mic path. */

// (UAC2.0 Spec Appendix A-14)
#define USBD_UAC_CLASS_REQ_CODE_CUR                               0x01U /**< Class-Specific Request Code: Current Value. */
#define USBD_UAC_CLASS_REQ_CODE_RANGE                             0x02U /**< Class-Specific Request Code: Range. */
// (UAC2.0 Spec Appendix A-17)
#define USBD_UAC_CS_SAM_FREQ_CONTROL                              0x01U /**< Clock Source Control Selector: Sampling Frequency. */
#define USBD_UAC_CS_CLK_VALID_CONTROL                             0x02U /**< Clock Source Control Selector: Clock Valid. */
// (UAC2.0 Spec Appendix A-20)
#define USBD_UAC_TE_CONNECTOR_CONTROL                             0x02U

/*! @brief Commands for USB device AUDIO control feature unit control selector */
#define USBD_UAC_CTRL_FU_MUTE_CONTROL_SELECTOR              0x01U /**< Mute Control. */
#define USBD_UAC_CTRL_FU_VOLUME_CONTROL_SELECTOR            0x02U /**< Volume Control. */
#define USBD_UAC_CTRL_FU_BASS_CONTROL_SELECTOR              0x03U /**< Bass Control. */
#define USBD_UAC_CTRL_FU_MID_CONTROL_SELECTOR               0x04U /**< Mid Control. */
#define USBD_UAC_CTRL_FU_TREBLE_CONTROL_SELECTOR            0x05U /**< Treble Control. */
#define USBD_UAC_CTRL_FU_GRAPHIC_EQUALIZER_CONTROL_SELECTOR 0x06U /**< Graphic Equalizer Control. */
#define USBD_UAC_CTRL_FU_AUTOMATIC_GAIN_CONTROL_SELECTOR    0x07U /**< Automatic Gain Control. */
#define USBD_UAC_CTRL_FU_DELAY_CONTROL_SELECTOR             0x08U /**< Delay Control. */
#define USBD_UAC_CTRL_FU_BASS_BOOST_CONTROL_SELECTOR        0x09U /**< Bass Boost Control. */
#define USBD_UAC_CTRL_FU_LOUDNESS_CONTROL_SELECTOR          0x0AU /**< Loudness Control. */
#define USBD_UAC_CTRL_FU_INPUT_GAIN_CONTROL_SELECTOR        0x0BU /**< Input Gain Control. */
#define USBD_UAC_CTRL_FU_INPUT_GAIN_PAD_CONTROL_SELECTOR    0x0CU /**< Input Gain Pad Control. */
#define USBD_UAC_CTRL_FU_PHASE_INVERTER_CONTROL_SELECTOR    0x0DU /**< Phase Inverter Control. */
#define USBD_UAC_CTRL_FU_UNDERFLOW_CONTROL_SELECTOR         0x0EU /**< Underflow Control. */
#define USBD_UAC_CTRL_FU_OVERFLOW_CONTROL_SELECTOR          0x0FU /**< Overflow Control. */
#define USBD_UAC_CTRL_FU_LATENCY_CONTROL_SELECTOR           0x10U /**< Latency Control. */

// Audio Class-Specific Audio Streaming Interface Descriptor Subtype Codes (UAC2.0 Spec Appendix A.10)
#define USBD_UAC_AS_IF_DESC_SUBTYPE_UNDEFINED               0x00
#define USBD_UAC_AS_IF_DESC_SUBTYPE_AS_GENERAL              0x01
#define USBD_UAC_AS_IF_DESC_SUBTYPE_FORMAT_TYPE             0x02

// Audio Class-Specific Audio Streaming Endpoint Descriptor Subtype Codes (UAC2.0 Spec Appendix A.13)
#define USBD_UAC_AS_EP_DESC_SUBTYPE_UNDEFINED               0x00
#define USBD_UAC_AS_EP_DESC_SUBTYPE_EP_GENERAL              0x01

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
#define USBD_UAC_AS_IF_DESC_ALT_OFFSET              3U  /**< Offset to Alternate Setting in AS Interface desc. */
#define USBD_UAC_AS_IF_DESC_EP_OFFSET               4U  /**< Offset to Endpoint address in AS Interface desc. */
#define USBD_UAC_AS_IF_DESC_CH_CNT_OFFSET           10U /**< Offset to Channel Count in AS Interface desc. */
#define USBD_UAC_AS_IF_DESC_CH_OFFSET               11U /**< Offset to Channel Config in AS Interface desc. */
#define USBD_UAC_AS_FORMAT_DESC_SLOT_SIZE_OFFSET    4U  /**< Offset to Subframe Size in Format Type desc. */
#define USBD_UAC_AS_FORMAT_DESC_BIT_CNT_OFFSET      5U  /**< Offset to Bit Resolution in Format Type desc. */
#define USBD_UAC_AS_EP_DESC_MPS_OFFSET              4U  /**< Offset to wMaxPacketSize in Endpoint desc. */

#define USBD_UAC_POW2(n)                            (1 << (n))

/* Get channel config */
#define USBD_UAC_GET_CH_CONFIG(ch_cnt) \
    ((ch_cnt) == 2 ? 0x03 : \
     (ch_cnt) == 4 ? 0x0F : \
     (ch_cnt) == 6 ? 0x3F : \
     (ch_cnt) == 8 ? 0xFF : \
     (ch_cnt) == 16 ? 0xFFFF : 0x03)

/* Get ot type */
#define USBD_UAC_GET_OT_TYPE(ch_cnt) \
    ((ch_cnt) == 2 ? 0x0301 : \
     (ch_cnt) == 4 ? 0x0304 : \
     (ch_cnt) == 6 ? 0x0304 : \
     (ch_cnt) == 8 ? 0x0307 : \
     (ch_cnt) == 16 ? 0x0307 : 0x0301)

/* Input terminal */
#define USBD_UAC_CH_CONFIG_TYPE_LOW(ch_cnt)         (USB_LOW_BYTE(USBD_UAC_GET_CH_CONFIG(ch_cnt)))
#define USBD_UAC_CH_CONFIG_TYPE_HIGH(ch_cnt)        (USB_HIGH_BYTE(USBD_UAC_GET_CH_CONFIG(ch_cnt)))

/* Output terminal */
#define USBD_UAC_OT_DESC_TYPE_LOW(ch_cnt)           (USB_LOW_BYTE(USBD_UAC_GET_OT_TYPE(ch_cnt)))
#define USBD_UAC_OT_DESC_TYPE_HIGH(ch_cnt)          (USB_HIGH_BYTE(USBD_UAC_GET_OT_TYPE(ch_cnt)))

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

/* Private macros ------------------------------------------------------------*/

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

/* Private types -------------------------------------------------------------*/
typedef struct {
	u16 wNumSubRanges;
	u16 wMIN;
	u16 wMAX;
	u16 wRES;
} __PACKED usbd_composite_uac_ctrl_range_layout2_struct;

typedef struct {
	u32 dMIN;
	u32 dMAX;
	u32 dRES;
} usbd_composite_uac_sub_range_t;

typedef struct {
	u16 wNumSubRanges;
	usbd_composite_uac_sub_range_t usbd_composite_uac_sub_ranges[];
} __PACKED usbd_composite_uac_sampling_freq_ctrl_range_t;

typedef struct {
	u8  bNrChannels;
	u32 bmChannelConfig;
	u8  iChannelNames;
} __PACKED usbd_composite_uac_ac_connect_ctrl_t;

/* Private function prototypes -----------------------------------------------*/
static int usbd_composite_uac_set_config(usb_dev_t *dev, u8 config);
static int usbd_composite_uac_clear_config(usb_dev_t *dev, u8 config);
static int usbd_composite_uac_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_composite_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
static int usbd_composite_uac_sof(usb_dev_t *dev);
static int usbd_composite_uac_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_composite_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static int usbd_composite_uac_handle_ep0_data_out(usb_dev_t *dev);
static void usbd_composite_uac_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
#if USBD_COMPOSITE_UAC_DEBUG
static void usbd_composite_uac_status_dump_thread(void *param);
static inline void usbd_composite_uac_get_audio_data_cnt(u32 audio_len);
#endif
static inline u16 usbd_composite_uac_get_ring_buf_cnt(u8 speed);
static u16 usbd_composite_uac_get_mps(usbd_audio_cfg_t *params, u8 speed);
static u8 usbd_composite_uac_get_ch_config(u8 ch_cnt);
static inline u8 usbd_composite_uac_ep_enable(usbd_audio_cfg_t *ep);
static void usbd_composite_uac_clk_valid_req(usb_dev_t *dev, u8 flag);
static void usbd_composite_uac_cur_sampling_freq_req(usb_dev_t *dev, u32 sampling_freq);
static int usbd_composite_uac_sampling_freq_ctrl_range_req(usb_dev_t *dev, u16 max_len);
static void usbd_composite_uac_connect_ctrl_req(usb_dev_t *dev, u8 ch_num, u32 ch_cfg, u16 max_len);
static int usbd_composite_uac_is_valid_sample_rate(u32 sampling_freq, u8 speed);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "COMP";

static u32 usbd_composite_uac_sampling_freq[USBD_UAC_SAMPLING_FREQ_MAX_COUNT] = {USBD_UAC_SAMPLING_FREQ_48K};

/* UAC interface descriptor */
static const u8 usbd_composite_uac_hs_itf_desc[] = {
	/* 4.6 Interface Association Descriptor */
	/* IAD Descriptor */
	USB_LEN_IAD_DESC,                  /* Size of this descriptor (byte_width) */
	USBD_UAC_IA_DESCRIPTOR,            /* Interface Association Descriptor type */
	USBD_COMP_UAC_AC_HEADSET,          /* First Index: Audio Control Interface Index - Headset (0) */
	USBD_UAC_AC_IF_NUM,                /* Audio Control Interface, Audio Streaming Interfaces () */
	USBD_UAC_IF_CLASS_AUDIO,           /* Audio Device Class  */
	0x00,                              /* No subclass */
	USBD_UAC_VERSION_02_00,            /* Audio Protocol IP version 2.00 */
	0x00,                              /* Function string descriptor index (0) */

	/* 4.7.1 Standard AC Interface Descriptor */
	/* Interface 0 Descriptor */
	USB_LEN_IF_DESC,                   /* bLength */
	USB_DESC_TYPE_INTERFACE,           /* Interface Descriptor type */
	USBD_COMP_UAC_AC_HEADSET,          /* Audio Control Interface Index - Headset (0) */
	0x00,                              /* bAlternateSetting */
	0x00,                              /* No associated endpoints with this interface (uses endpoint 0) */
	USBD_UAC_CLASS_CODE,               /* Audio Device Class */
	USBD_UAC_SUBCLASS_AUDIOCONTROL,    /* Audio Control Interface */
	USBD_UAC_VERSION_02_00,            /* Audio Protocol IP version 2.00 */
	0x00,                              /* Interface string descriptor index (0) */

	/* 4.7.2 Class-Specific AC Interface Descriptor */
	/* Audio headset */
	USBD_UAC_LEN_CTRL_IF_HEADER,                                          /* Size of the descriptor, in byte_width  */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                                /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AC_IF_DESC_SUBTYPE_HEADER,                                   /* HEADER descriptor subtype  */
	USB_LOW_BYTE(0x0200),                                                 /* bcdUSB */
	USB_HIGH_BYTE(0x0200),
	USBD_UAC_FUNC_CATEGORY_CODE_DESKTOP_SPEAKER,                          /* DESKTOP_SPEAKER(0x01) : Indicating the primary use of this audio function   */
	USB_LOW_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT)),
	USB_HIGH_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT)), /* Total number of byte_width returned for the class-specific AudioControl interface descriptor. Includes
                                                                           the combined length of this descriptor header and all Unit and Terminal descriptors.   */
	0x00,                                                                 /* D1..0: Latency Control  */

	/* Audio Control Clock Source Unit Descriptor 2.0 */
	USBD_UAC_LEN_CLK_SRC_DESC,                         /* Size of the descriptor, in byte_width  */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,             /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE,          /* CLOCK_SOURCE descriptor subtype  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES,   /* Constant uniquely identifying the Clock Source Entity within
                                                          the audio funcion */
	0x01,                                              /* D1..0: 01: Internal Fixed Clock
                                                          D2: 0 Clock is not synchronized to SOF
                                                          D7..3: Reserved, should set to 0   */
	0x07,                                              /* D1..0: Clock Frequency Control is present and Host programmable
                                                          D3..2: Clock Validity Control is present but read-only
                                                          D7..4: Reserved, should set to 0 */
	0x00,                                              /* This Clock Source has no association   */
	0x00,                                              /* Index of a string descriptor, describing the Clock Source Entity  */

	/* Microphone Audio Control Input Terminal Descriptor 2.0 */
	USBD_UAC_LEN_AC_IN_TTY_DESC,                             /* Size of the descriptor, in byte_width  */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AC_IF_DESC_SUBTYPE_INPUT_TERMINAL,              /* INPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* Constant uniquely identifying the Terminal within the audio
                                                                  function. This value is used in all requests to address this Terminal.   */
	0x01,
	0x01,                                                    /* A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The
                                                                AudioStreaming interface descriptor points to the associated Terminal through the bTerminalLink field. */
	0x00,                                                    /* This Input Terminal has no association   */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES,         /* ID of the Clock Entity to which this Input Terminal is connected.  */
	USBD_UAC_DEFAULT_CH_CNT,                                 /* This Terminal's output audio channel cluster has 4 logical output channels   */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_DEFAULT_CH_CNT),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_DEFAULT_CH_CNT),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels:: Mono, no spatial location */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first logical channel.  */
	0x00,
	0x00,                                                    /* bmControls D1..0: Copy Protect Control is not present
                                                                D3..2: Connector Control is not present
                                                                D5..4: Overload Control is not present
                                                                D7..6: Cluster Control is not present
                                                                D9..8: Underflow Control is not present
                                                                D11..10: Overflow Control is not present
                                                                D15..12: Reserved, should set to 0*/
	0x00,                                                    /* Index of a string descriptor, describing the Input Terminal.  */

	/* FEATURE_UNIT descriptor subtype */
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT),   /* Size of the descriptor, in byte_width  : 6 + (4 + 1) * 4 */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AC_IF_DESC_SUBTYPE_FEATURE_UNIT,                /* FEATURE_UNIT descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT,       /* Constant uniquely identifying the Unit within the audio function. This
                                                                value is used in all requests to address this Unit.  */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* ID of the Unit or Terminal to which this Feature Unit is connected. */
	0x0F,
	0x00,
	0x00,
	0x00,                                                    /* logic channel 0 bmaControls(0)(0x0000000F):  D1..0: Mute Control is present and host
                                                                programmable D3..2: Volume Control is present and host programmable D5..4: Bass
                                                                Control is not present D7..6: Mid Control is not present D9..8: Treble Control is not
                                                                present D11..10: Graphic Equalizer Control is not present D13..12: Automatic Gain
                                                                Control is not present D15..14: Delay Control is not present D17..16: Bass Control is
                                                                not present D19..18: Loudness Control is not present D21..20: Input Gain Control is
                                                                not present D23..22: Input Gain Pad Control is not present D25..24: Phase Inverter
                                                                Control is not present D27..26: Underflow Control is not present D29..28: Overflow
                                                                Control is not present D31..30: Reserved, should set to 0 */
	0x00,                                                    /*  bmaControls[1](0x00000000), eg. 8 channels means bmaControls[1] ~ bmaControls[8] */
	0x00,
	0x00,
	0x00,
	0x00,                                                    /*  bmaControls[2](0x00000000) */
	0x00,
	0x00,
	0x00,
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	0x00,                                                    /*  bmaControls[3](0x00000000)*/
	0x00,
	0x00,
	0x00,
	0x00,                                                    /*  bmaControls[4](0x00000000) */
	0x00,
	0x00,
	0x00,
#endif
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	0x00,                                                    /*  bmaControls[5](0x00000000)*/
	0x00,
	0x00,
	0x00,
	0x00,                                                    /*  bmaControls[6](0x00000000) */
	0x00,
	0x00,
	0x00,
#endif
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	0x00,                                                    /*  bmaControls[7](0x00000000)*/
	0x00,
	0x00,
	0x00,
	0x00,                                                    /*  bmaControls[8](0x00000000) */
	0x00,
	0x00,
	0x00,
#endif
	0x00,                                                     /* iFeature, Index of a string descriptor, describing this Feature Unit.*/

	/* OUTPUT_TERMINAL descriptor subtype   */
	USBD_UAC_LEN_AC_OUT_TTY_DESC,                             /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                    /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AC_IF_DESC_SUBTYPE_OUTPUT_TERMINAL,              /* OUTPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES, /* Constant uniquely identifying the Terminal within the audio
                                                                 function. This value is used in all requests to address this Terminal.   */
	USBD_UAC_OT_DESC_TYPE_LOW(USBD_UAC_DEFAULT_CH_CNT),
	USBD_UAC_OT_DESC_TYPE_HIGH(USBD_UAC_DEFAULT_CH_CNT),      /* A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The
                                                                 AudioStreaming interface descriptor points to the associated Terminal through the bTerminalLink field.  */
	0x00,                                                     /* This Output Terminal has no association  */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT,        /* ID of the Unit or Terminal to which this Terminal is connected.  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES,          /* ID of the Clock Entity to which this Output Terminal is connected  */
	0x00,
	0x00,                                                     /* bmControls:   D1..0: Copy Protect Control is not present
                                                                 D3..2: Connector Control is not present
                                                                 D5..4: Overload Control is not present
                                                                 D7..6: Underflow Control is not present
                                                                 D9..8: Overflow Control is not present
                                                                 D15..10: Reserved, should set to 0   */
	0x00,                                                     /* Index of a string descriptor, describing the Output Terminal.  */


	/* 4.9.1 Standard AS Interface Descriptor 1/0*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USBD_COMP_UAC_AS_HEADSET_HEADPHONES,                      /* The number of this interface is 1.   */
	0x00,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x00,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USBD_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USBD_UAC_VERSION_02_00,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.1 Standard AS Interface Descriptor 1/1*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USBD_COMP_UAC_AS_HEADSET_HEADPHONES,                      /* The number of this interface is 1.   */
	0x01,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USBD_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USBD_UAC_VERSION_02_00,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AS_IF_DESC_SUBTYPE_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* The Terminal ID of the terminal to which this interface is connected */
	0x0F,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CH_CNT_2,                                 /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CH_CNT_2),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CH_CNT_2),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AS_IF_DESC_SUBTYPE_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                                   /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),               /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_COMP_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 2   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_CALC_HS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_CALC_HS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K)),
	USBD_UAC_HS_DEFAULT_BINTERVAL,                           /* bInterval */


	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USBD_UAC_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USBD_UAC_CS_ENDPOINT_DESCRIPTOR,                         /* Class-specific Endpoint Descriptor type */
	USBD_UAC_AS_EP_DESC_SUBTYPE_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */

#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/2*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USBD_COMP_UAC_AS_HEADSET_HEADPHONES,                      /* The number of this interface is 1.   */
	0x02,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USBD_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USBD_UAC_VERSION_02_00,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AS_IF_DESC_SUBTYPE_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	0x1, 													 /* The Terminal ID of the terminal to which this interface is connected */
	0xF,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CH_CNT_4,                                 /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CH_CNT_4),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CH_CNT_4),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */


	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AS_IF_DESC_SUBTYPE_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                                   /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),               /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_COMP_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 2   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_CALC_HS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_CALC_HS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K)),
	USBD_UAC_HS_DEFAULT_BINTERVAL,                           /* bInterval */


	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USBD_UAC_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USBD_UAC_CS_ENDPOINT_DESCRIPTOR,                         /* Class-specific Endpoint Descriptor type */
	USBD_UAC_AS_EP_DESC_SUBTYPE_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif

#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/3*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USBD_COMP_UAC_AS_HEADSET_HEADPHONES,                      /* The number of this interface is 1.   */
	0x03,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USBD_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USBD_UAC_VERSION_02_00,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AS_IF_DESC_SUBTYPE_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	0x1, 													 /* The Terminal ID of the terminal to which this interface is connected */
	0xF,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CH_CNT_6,                                 /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CH_CNT_6),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CH_CNT_6),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */


	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AS_IF_DESC_SUBTYPE_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                                   /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),               /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_COMP_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 2   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_CALC_HS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_CALC_HS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K)),
	USBD_UAC_HS_DEFAULT_BINTERVAL,                           /* bInterval */


	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USBD_UAC_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USBD_UAC_CS_ENDPOINT_DESCRIPTOR,                         /* Class-specific Endpoint Descriptor type */
	USBD_UAC_AS_EP_DESC_SUBTYPE_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif

#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/4*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USBD_COMP_UAC_AS_HEADSET_HEADPHONES,                      /* The number of this interface is 1.   */
	0x04,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USBD_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USBD_UAC_VERSION_02_00,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AS_IF_DESC_SUBTYPE_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	0x1, 													 /* The Terminal ID of the terminal to which this interface is connected */
	0xF,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CH_CNT_8,                                 /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CH_CNT_8),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CH_CNT_8),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */


	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AS_IF_DESC_SUBTYPE_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                                   /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),               /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_COMP_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 2   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_CALC_HS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_CALC_HS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K)),
	USBD_UAC_HS_DEFAULT_BINTERVAL,                           /* bInterval */


	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USBD_UAC_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USBD_UAC_CS_ENDPOINT_DESCRIPTOR,                         /* Class-specific Endpoint Descriptor type */
	USBD_UAC_AS_EP_DESC_SUBTYPE_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif
};  /* usbd_composite_uac_hs_itf_desc */

static const u8 usbd_composite_uac_fs_itf_desc[] = {
	/* 4.6 Interface Association Descriptor */
	/* IAD Descriptor */
	USB_LEN_IAD_DESC,                  /* Size of this descriptor (byte_width) */
	USBD_UAC_IA_DESCRIPTOR,            /* Interface Association Descriptor type */
	USBD_COMP_UAC_AC_HEADSET,          /* First Index: Audio Control Interface Index - Headset (0) */
	USBD_UAC_AC_IF_NUM,                /* Audio Control Interface, Audio Streaming Interfaces () */
	USBD_UAC_IF_CLASS_AUDIO,           /* Audio Device Class  */
	0x00,                              /* No subclass */
	USBD_UAC_VERSION_02_00,            /* Audio Protocol IP version 2.00 */
	0x00,                              /* Function string descriptor index (0) */

	/* 4.7.1 Standard AC Interface Descriptor */
	/* Interface 0 Descriptor */
	USB_LEN_IF_DESC,                   /* bLength */
	USB_DESC_TYPE_INTERFACE,           /* Interface Descriptor type */
	USBD_COMP_UAC_AC_HEADSET,          /* Audio Control Interface Index - Headset (0) */
	0x00,                              /* bAlternateSetting */
	0x00,                              /* No associated endpoints with this interface (uses endpoint 0) */
	USBD_UAC_CLASS_CODE,               /* Audio Device Class */
	USBD_UAC_SUBCLASS_AUDIOCONTROL,    /* Audio Control Interface */
	USBD_UAC_VERSION_02_00,            /* Audio Protocol IP version 2.00 */
	0x00,                              /* Interface string descriptor index (0) */

	/* 4.7.2 Class-Specific AC Interface Descriptor */
	/* Audio headset */
	USBD_UAC_LEN_CTRL_IF_HEADER,                                          /* Size of the descriptor, in byte_width  */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                                /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AC_IF_DESC_SUBTYPE_HEADER,                                   /* HEADER descriptor subtype  */
	USB_LOW_BYTE(0x0200),                                                 /* bcdUSB */
	USB_HIGH_BYTE(0x0200),
	USBD_UAC_FUNC_CATEGORY_CODE_DESKTOP_SPEAKER,                          /* DESKTOP_SPEAKER(0x01) : Indicating the primary use of this audio function   */
	USB_LOW_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT)),
	USB_HIGH_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT)), /* Total number of byte_width returned for the class-specific AudioControl interface descriptor. Includes
                                                                           the combined length of this descriptor header and all Unit and Terminal descriptors.   */
	0x00,                                                                 /* D1..0: Latency Control  */

	/* Audio Control Clock Source Unit Descriptor 2.0 */
	USBD_UAC_LEN_CLK_SRC_DESC,                         /* Size of the descriptor, in byte_width  */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,             /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE,          /* CLOCK_SOURCE descriptor subtype  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES,   /* Constant uniquely identifying the Clock Source Entity within
                                                          the audio funcion */
	0x01,                                              /* D1..0: 01: Internal Fixed Clock
                                                          D2: 0 Clock is not synchronized to SOF
                                                          D7..3: Reserved, should set to 0   */
	0x07,                                              /* D1..0: Clock Frequency Control is present and Host programmable
                                                          D3..2: Clock Validity Control is present but read-only
                                                          D7..4: Reserved, should set to 0 */
	0x00,                                              /* This Clock Source has no association   */
	0x00,                                              /* Index of a string descriptor, describing the Clock Source Entity  */

	/* Microphone Audio Control Input Terminal Descriptor 2.0 */
	USBD_UAC_LEN_AC_IN_TTY_DESC,                             /* Size of the descriptor, in byte_width  */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AC_IF_DESC_SUBTYPE_INPUT_TERMINAL,              /* INPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* Constant uniquely identifying the Terminal within the audio
                                                                  function. This value is used in all requests to address this Terminal.   */
	0x01,
	0x01,                                                    /* A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The
                                                                AudioStreaming interface descriptor points to the associated Terminal through the bTerminalLink field. */
	0x00,                                                    /* This Input Terminal has no association   */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES,         /* ID of the Clock Entity to which this Input Terminal is connected.  */
	USBD_UAC_DEFAULT_CH_CNT,                                 /* This Terminal's output audio channel cluster has 4 logical output channels   */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_DEFAULT_CH_CNT),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_DEFAULT_CH_CNT),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels:: Mono, no spatial location */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first logical channel.  */
	0x00,
	0x00,                                                    /* bmControls D1..0: Copy Protect Control is not present
                                                                D3..2: Connector Control is not present
                                                                D5..4: Overload Control is not present
                                                                D7..6: Cluster Control is not present
                                                                D9..8: Underflow Control is not present
                                                                D11..10: Overflow Control is not present
                                                                D15..12: Reserved, should set to 0*/
	0x00,                                                    /* Index of a string descriptor, describing the Input Terminal.  */

	/* FEATURE_UNIT descriptor subtype */
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT),   /* Size of the descriptor, in byte_width  : 6 + (4 + 1) * 4 */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AC_IF_DESC_SUBTYPE_FEATURE_UNIT,                /* FEATURE_UNIT descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT,       /* Constant uniquely identifying the Unit within the audio function. This
                                                                value is used in all requests to address this Unit.  */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* ID of the Unit or Terminal to which this Feature Unit is connected. */
	0x0F,
	0x00,
	0x00,
	0x00,                                                    /* logic channel 0 bmaControls(0)(0x0000000F):  D1..0: Mute Control is present and host
                                                                programmable D3..2: Volume Control is present and host programmable D5..4: Bass
                                                                Control is not present D7..6: Mid Control is not present D9..8: Treble Control is not
                                                                present D11..10: Graphic Equalizer Control is not present D13..12: Automatic Gain
                                                                Control is not present D15..14: Delay Control is not present D17..16: Bass Control is
                                                                not present D19..18: Loudness Control is not present D21..20: Input Gain Control is
                                                                not present D23..22: Input Gain Pad Control is not present D25..24: Phase Inverter
                                                                Control is not present D27..26: Underflow Control is not present D29..28: Overflow
                                                                Control is not present D31..30: Reserved, should set to 0 */
	0x00,                                                    /*  bmaControls[1](0x00000000), eg. 8 channels means bmaControls[1] ~ bmaControls[8] */
	0x00,
	0x00,
	0x00,
	0x00,                                                    /*  bmaControls[2](0x00000000) */
	0x00,
	0x00,
	0x00,
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	0x00,                                                    /*  bmaControls[3](0x00000000)*/
	0x00,
	0x00,
	0x00,
	0x00,                                                    /*  bmaControls[4](0x00000000) */
	0x00,
	0x00,
	0x00,
#endif
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	0x00,                                                    /*  bmaControls[5](0x00000000)*/
	0x00,
	0x00,
	0x00,
	0x00,                                                    /*  bmaControls[6](0x00000000) */
	0x00,
	0x00,
	0x00,
#endif
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	0x00,                                                    /*  bmaControls[7](0x00000000)*/
	0x00,
	0x00,
	0x00,
	0x00,                                                    /*  bmaControls[8](0x00000000) */
	0x00,
	0x00,
	0x00,
#endif
	0x00,                                                     /* iFeature, Index of a string descriptor, describing this Feature Unit.*/

	/* OUTPUT_TERMINAL descriptor subtype   */
	USBD_UAC_LEN_AC_OUT_TTY_DESC,                             /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                    /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AC_IF_DESC_SUBTYPE_OUTPUT_TERMINAL,              /* OUTPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES, /* Constant uniquely identifying the Terminal within the audio
                                                                 function. This value is used in all requests to address this Terminal.   */
	USBD_UAC_OT_DESC_TYPE_LOW(USBD_UAC_DEFAULT_CH_CNT),
	USBD_UAC_OT_DESC_TYPE_HIGH(USBD_UAC_DEFAULT_CH_CNT),      /* A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The
                                                                 AudioStreaming interface descriptor points to the associated Terminal through the bTerminalLink field.  */
	0x00,                                                     /* This Output Terminal has no association  */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT,        /* ID of the Unit or Terminal to which this Terminal is connected.  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES,          /* ID of the Clock Entity to which this Output Terminal is connected  */
	0x00,
	0x00,                                                     /* bmControls:   D1..0: Copy Protect Control is not present
                                                                 D3..2: Connector Control is not present
                                                                 D5..4: Overload Control is not present
                                                                 D7..6: Underflow Control is not present
                                                                 D9..8: Overflow Control is not present
                                                                 D15..10: Reserved, should set to 0   */
	0x00,                                                     /* Index of a string descriptor, describing the Output Terminal.  */


	/* 4.9.1 Standard AS Interface Descriptor 1/0*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USBD_COMP_UAC_AS_HEADSET_HEADPHONES,                      /* The number of this interface is 1.   */
	0x00,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x00,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USBD_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USBD_UAC_VERSION_02_00,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */


	/* 4.9.1 Standard AS Interface Descriptor 1/1*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USBD_COMP_UAC_AS_HEADSET_HEADPHONES,                      /* The number of this interface is 1.   */
	0x01,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USBD_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USBD_UAC_VERSION_02_00,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AS_IF_DESC_SUBTYPE_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* The Terminal ID of the terminal to which this interface is connected */
	0x0F,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CH_CNT_2,                                 /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CH_CNT_2),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CH_CNT_2),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AS_IF_DESC_SUBTYPE_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                                   /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),               /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_COMP_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 2   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K)),
	USBD_UAC_FS_DEFAULT_BINTERVAL,                           /* bInterval */


	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USBD_UAC_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USBD_UAC_CS_ENDPOINT_DESCRIPTOR,                         /* Class-specific Endpoint Descriptor type */
	USBD_UAC_AS_EP_DESC_SUBTYPE_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */

#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/2*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USBD_COMP_UAC_AS_HEADSET_HEADPHONES,                      /* The number of this interface is 1.   */
	0x02,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USBD_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USBD_UAC_VERSION_02_00,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AS_IF_DESC_SUBTYPE_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	0x1, 													 /* The Terminal ID of the terminal to which this interface is connected */
	0xF,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CH_CNT_4,                                 /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CH_CNT_4),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CH_CNT_4),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */


	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AS_IF_DESC_SUBTYPE_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                                   /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),               /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_COMP_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 2   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K)),
	USBD_UAC_FS_DEFAULT_BINTERVAL,                           /* bInterval */


	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USBD_UAC_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USBD_UAC_CS_ENDPOINT_DESCRIPTOR,                         /* Class-specific Endpoint Descriptor type */
	USBD_UAC_AS_EP_DESC_SUBTYPE_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif

#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/3*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USBD_COMP_UAC_AS_HEADSET_HEADPHONES,                      /* The number of this interface is 1.   */
	0x03,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USBD_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USBD_UAC_VERSION_02_00,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AS_IF_DESC_SUBTYPE_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	0x1, 													 /* The Terminal ID of the terminal to which this interface is connected */
	0xF,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CH_CNT_6,                                 /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CH_CNT_6),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CH_CNT_6),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */


	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AS_IF_DESC_SUBTYPE_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                                   /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),               /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_COMP_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 2   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K)),
	USBD_UAC_FS_DEFAULT_BINTERVAL,                           /* bInterval */


	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USBD_UAC_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USBD_UAC_CS_ENDPOINT_DESCRIPTOR,                         /* Class-specific Endpoint Descriptor type */
	USBD_UAC_AS_EP_DESC_SUBTYPE_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif

#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/4*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USBD_COMP_UAC_AS_HEADSET_HEADPHONES,                      /* The number of this interface is 1.   */
	0x04,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USBD_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USBD_UAC_VERSION_02_00,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AS_IF_DESC_SUBTYPE_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	0x1, 													 /* The Terminal ID of the terminal to which this interface is connected */
	0xF,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CH_CNT_8,                                 /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CH_CNT_8),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CH_CNT_8),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */


	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AS_IF_DESC_SUBTYPE_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                                   /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),               /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_COMP_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 2   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K)),
	USBD_UAC_FS_DEFAULT_BINTERVAL,                           /* bInterval */


	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USBD_UAC_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USBD_UAC_CS_ENDPOINT_DESCRIPTOR,                         /* Class-specific Endpoint Descriptor type */
	USBD_UAC_AS_EP_DESC_SUBTYPE_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif
};  /* usbd_composite_uac_fs_config_desc */

static usbd_composite_uac_device_t usbd_composite_uac_device;

/* UAC volume data from windows10 PC*/
/* Different OS transmit the same vol level, but the corresponding driver vol values are different. */
static u8 usbd_composite_uac_pc_vol_lvl[] = {0, 5, 10, 20, 30, 40, 50, 60, 65, 75, 80, 85, 90, 95, 100};
static s16 usbd_composite_uac_drv_vol[] = {-190, -170, -151, -112, -74, -37, 0, 37, 56, 93, 112, 132, 151, 170, 190};

/* Exported variables --------------------------------------------------------*/

const usbd_class_driver_t usbd_composite_uac_driver = {
	.get_descriptor = usbd_composite_uac_get_descriptor,
	.set_config = usbd_composite_uac_set_config,
	.clear_config = usbd_composite_uac_clear_config,
	.setup = usbd_composite_uac_setup,
	.ep0_data_out = usbd_composite_uac_handle_ep0_data_out,
	.ep_data_in = usbd_composite_uac_handle_ep_data_in,
	.ep_data_out = usbd_composite_uac_handle_ep_data_out,
	.status_changed = usbd_composite_uac_status_changed,
	.sof = usbd_composite_uac_sof,
};

/* Private functions ---------------------------------------------------------*/
void usbd_composite_uac_list_init(usbd_composite_uac_buf_list_t *list)
{
	if (list) {
		list->head = NULL;
		list->tail = NULL;
		list->count = 0;
	}
}

usbd_composite_uac_buf_t *usbd_composite_uac_list_remove_head(usbd_composite_uac_buf_list_t *list)
{
	if (list == NULL || list->head == NULL) {
		return NULL;
	}

	usbd_composite_uac_buf_t *node = list->head;
	list->head = node->next;
	if (list->head == NULL) {
		list->tail = NULL;
	}
	list->count--;

	node->next = NULL;
	return node;
}

void usbd_composite_uac_list_add_tail(usbd_composite_uac_buf_list_t *list, usbd_composite_uac_buf_t *node)
{
	if (list == NULL || node == NULL) {
		return;
	}

	node->next = NULL;

	if (list->tail) {
		list->tail->next = node;
	} else {
		list->head = node;
	}

	list->tail = node;
	list->count++;
}

/**
  * @brief  Function used to append zero packet to the buffer
  * @param  pdata_ctrl: UAC buf ctrl handle
  * @param  time_diff: Time diff in us
  * @param  speed: Cur USB work speed
  * @retval
  */
static void usbd_composite_uac_append_data(void)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *pdata_ctrl = &(uac->uac_isoc_out);
	usbd_composite_uac_buf_t *p_buf = NULL;

	if (pdata_ctrl->empty_list.count == 0) { //full
		usbd_composite_uac_buf_t *node = usbd_composite_uac_list_remove_head(&(pdata_ctrl->data_list));
		if (node) {
			usbd_composite_uac_list_add_tail(&(pdata_ctrl->empty_list), node);
		}

#if USBD_COMPOSITE_UAC_DEBUG
		uac->isoc_overwrite_cnt ++;
#endif
	}

	p_buf = usbd_composite_uac_list_remove_head(&(pdata_ctrl->empty_list));
	if (p_buf) {
		p_buf->is_zero_pkt = 1;
		p_buf->buf_valid_len = uac->isoc_rx_len;
#if USBD_COMPOSITE_UAC_DEBUG
		uac->isoc_zlp_cnt ++;
#endif
		usbd_composite_uac_list_add_tail(&(pdata_ctrl->data_list), p_buf);

		//used for usb read
		if (usbd_composite_uac_get_read_frame_cnt() >= USBD_UAC_HS_SOF_COUNT_PER_MS) {
			rtos_sema_give(pdata_ctrl->uac_isoc_sema);
		}
	}
}

/**
  * @brief  Get the ring buffer count based on USB speed
  * @param  speed: USB connection speed
  * @retval Number of buffer segments for audio data
  */
static u16 usbd_composite_uac_get_ring_buf_cnt(u8 speed)
{
	if (speed == USB_SPEED_HIGH) { //high speed
		return 8 * USBD_UAC_RX_BUF_MAX_CNT / USBD_UAC_POW2(USBD_UAC_HS_DEFAULT_BINTERVAL - 1);
	}

	return USBD_UAC_RX_BUF_MAX_CNT / USBD_UAC_POW2(USBD_UAC_FS_DEFAULT_BINTERVAL - 1);
}

/**
  * @brief  Get the MPS based on configuration parameters and speed
  * @param  params: Pointer to audio configuration parameters structure
  * @param  speed: USB connection speed
  * @retval Calculated MPS
  */
static u16 usbd_composite_uac_get_mps(usbd_audio_cfg_t *params, u8 speed)
{
	u16 mps_value;
	if (NULL == params) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Param err\n");
		return 0;
	}

	if (speed == USB_SPEED_HIGH) {
		mps_value = USBD_UAC_CALC_HS_MPS(params->ch_cnt, params->byte_width, params->sampling_freq);
		if ((mps_value == 0) || (mps_value > USBD_UAC_HS_ISOC_MPS)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "MPS %d zero or exceed HS limited %d\n", mps_value, USBD_UAC_HS_ISOC_MPS);
			return 0;
		}
	} else {
		/* for 44.1khz or the host clk is bigger than the device */
		mps_value = USBD_UAC_CALC_FS_MPS(params->ch_cnt, params->byte_width, params->sampling_freq);
		if ((mps_value == 0) || (mps_value > USBD_UAC_FS_ISOC_MPS)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "MPS %d : zero or exceed FS limited %d\n", mps_value, USBD_UAC_FS_ISOC_MPS);
			return 0;
		}
	}

	// RTK_LOGS(TAG, RTK_LOG_DEBUG, "Ch %d bw %d sampling_freq %d speed %d mps %d\n", params->ch_cnt , params->byte_width , params->sampling_freq, speed, mps_value);
	return mps_value;
}

/**
  * @brief  Get the channel configuration based on UAC channel cnt
  * @param  ch_cnt: UAC channel cnt
  * @retval Bitmask representing the channel configuration
  */
static u8 usbd_composite_uac_get_ch_config(u8 ch_cnt)
{

	switch (ch_cnt) {
	case 2:
		return 0x03;
	case 4:
		return 0x0F;
	case 6:
		return 0x3F;
	case 8:
		return 0xFF;
	default:
		return 0x03;
	}

	return 0x03;
}

/**
  * @brief  Deinitialize UAC endpoint buffer control structure
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @retval void
  */
static void usbd_composite_uac_ep_buf_ctrl_deinit(usbd_composite_uac_buf_ctrl_t *buf_ctrl)
{
	buf_ctrl->isoc_mps = 0;
	buf_ctrl->next_xfer = 0;
	// RTK_LOGS(TAG, RTK_LOG_DEBUG, "Buf 0x%08x-0x%08x sema %d\n",buf_ctrl->isoc_buf,buf_ctrl->buf_list_node,buf_ctrl->uac_sema_valid);

	if (buf_ctrl->isoc_buf != NULL) {
		usb_os_mfree(buf_ctrl->isoc_buf);
		buf_ctrl->isoc_buf = NULL;
	}

	if (buf_ctrl->buf_list_node != NULL) {
		usb_os_mfree(buf_ctrl->buf_list_node);
		buf_ctrl->buf_list_node = NULL;
	}

	usbd_composite_uac_list_init(&(buf_ctrl->empty_list));
	usbd_composite_uac_list_init(&(buf_ctrl->data_list));

	if (buf_ctrl->uac_sema_valid) {
		buf_ctrl->uac_sema_valid = 0;

		if (buf_ctrl->read_wait_sema) {
			rtos_sema_give(buf_ctrl->uac_isoc_sema);
		}
		do {
			usb_os_delay_us(100U);
		} while (buf_ctrl->read_wait_sema);

		rtos_sema_delete(buf_ctrl->uac_isoc_sema);
	}
}

/**
  * @brief  Update the ep MPS
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @param  params: Pointer to audio configuration parameters structure
  * @param  speed: USB connection speed
  * @retval Status
  */
static int usbd_composite_uac_ep_update_mps(usbd_composite_uac_buf_ctrl_t *buf_ctrl, usbd_audio_cfg_t *params, u8 speed)
{
	u16 ep_mps;
	if (usbd_composite_uac_ep_enable(params)) {
		ep_mps = usbd_composite_uac_get_mps(params, speed);
		if (ep_mps == 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "MPS check fail\n");
			return HAL_ERR_PARA;
		}

		buf_ctrl->isoc_mps = ep_mps;
	}

	return HAL_OK;
}

/**
  * @brief  Initialize UAC endpoint buffer control structure
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @param  params: Pointer to audio configuration parameters structure
  * @retval Status
  */
static int usbd_composite_uac_ep_buf_ctrl_init(usbd_composite_uac_buf_ctrl_t *buf_ctrl, usbd_audio_cfg_t *params)
{
	u16 buf_list_cnt;

	if (usbd_composite_uac_ep_enable(params)) {
		buf_ctrl->isoc_mps = usbd_composite_uac_get_mps(params, USB_SPEED_HIGH);
		if (buf_ctrl->isoc_mps == 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "MPS check fail\n");
			return HAL_ERR_PARA;
		}

		buf_list_cnt = usbd_composite_uac_get_ring_buf_cnt(USB_SPEED_HIGH);
#if USBD_COMPOSITE_UAC_DEBUG
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Buf mps len %d-%d(%d %d %d), cnt %d\n", buf_ctrl->isoc_mps, CACHE_LINE_ALIGNMENT(buf_ctrl->isoc_mps), params->ch_cnt,
				 params->byte_width, params->sampling_freq, buf_list_cnt);
#endif

		buf_ctrl->isoc_buf = (u8 *)usb_os_malloc(CACHE_LINE_ALIGNMENT(buf_ctrl->isoc_mps) * buf_list_cnt);
		if (buf_ctrl->isoc_buf == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Can not get isoc buf mem\n");
			return HAL_ERR_MEM;
		}

		buf_ctrl->buf_list_node = (usbd_composite_uac_buf_t *)usb_os_malloc(sizeof(usbd_composite_uac_buf_t) * buf_list_cnt);
		if (buf_ctrl->buf_list_node == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Can not get isoc buf array mem\n");
			return HAL_ERR_MEM;
		}

		usbd_composite_uac_list_init(&(buf_ctrl->empty_list));
		usbd_composite_uac_list_init(&(buf_ctrl->data_list));

		rtos_sema_create(&(buf_ctrl->uac_isoc_sema), 0U, 1U);
		buf_ctrl->uac_sema_valid = 1;
	} else {
		usbd_composite_uac_ep_buf_ctrl_deinit(buf_ctrl);
	}

	return HAL_OK;
}

/**
  * @brief  Get USB audio endpoint status
  * @param  ep: PUSB audio endpoint config
  * @retval Status
  */
static inline u8 usbd_composite_uac_ep_enable(usbd_audio_cfg_t *ep)
{
	if (ep) {
		return ep->enable;
	}

	return  0;
}

/**
  * @brief  Handle UAC clock validity request
  * @param  dev: USB device instance
  * @param  flag: Clock validity flag
  * @retval void
  */
static void usbd_composite_uac_clk_valid_req(usb_dev_t *dev, u8 flag)
{
	usbd_ep_t *ep0_in = &dev->ep0_in;
	u8 valid = flag;

	usb_os_memcpy(ep0_in->xfer_buf, &valid, sizeof(valid));
	ep0_in->xfer_len = sizeof(valid);
	usbd_ep_transmit(dev, ep0_in);
}

/**
  * @brief  Handle UAC current sampling_freq request
  * @param  dev: USB device instance
  * @param  sampling_freq: UAC current sampling_freq
  * @retval void
  */
static void usbd_composite_uac_cur_sampling_freq_req(usb_dev_t *dev, u32 sampling_freq)
{
	usbd_ep_t *ep0_in = &dev->ep0_in;
	u32 dCur = sampling_freq;

	usb_os_memcpy(ep0_in->xfer_buf, &dCur, sizeof(dCur));
	ep0_in->xfer_len = sizeof(dCur);
	usbd_ep_transmit(dev, ep0_in);
}

/**
  * @brief  Handle sampling_freq ctrl requests
  * @param  dev: USB device instance
  * @param  max_len: Maximum allowed length for response
  * @retval Status
  */
static int usbd_composite_uac_sampling_freq_ctrl_range_req(usb_dev_t *dev, u16 max_len)
{
	usbd_composite_uac_sub_range_t sampling_freq_range;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	u16 num_sub_ranges;
	u16 len;
	u16 data_offset = 0;

	if (dev->dev_speed  == USB_SPEED_HIGH) {
		num_sub_ranges = USBD_UAC_HS_SAMPLING_FREQ_COUNT;
	} else {
		num_sub_ranges = USBD_UAC_FS_SAMPLING_FREQ_COUNT;
	}

	len = sizeof(num_sub_ranges) + num_sub_ranges * sizeof(usbd_composite_uac_sub_range_t);

	usb_os_memcpy(ep0_in->xfer_buf, (u8 *)&num_sub_ranges, 2);
	data_offset += 2;

	for (u8 i = 0; i < num_sub_ranges; i++) {
		USBD_UAC_INIT_SUB_RANGE(sampling_freq_range, usbd_composite_uac_sampling_freq[i], usbd_composite_uac_sampling_freq[i], 0);
		usb_os_memcpy(ep0_in->xfer_buf + data_offset, &sampling_freq_range, sizeof(sampling_freq_range));
		data_offset += sizeof(sampling_freq_range);
	}

	if (len > max_len) {
		len = max_len;
	}

	ep0_in->xfer_len = len;
	usbd_ep_transmit(dev, ep0_in);

	return HAL_OK;
}

/**
  * @brief  Handle connect ctrl requests
  * @param  dev: USB device instance
  * @param  ch_num: USB channel num
  * @param  ch_cfg: USB channel configuration
  * @param  max_len: Maximum allowed length for response
  * @retval Status
  */
static void usbd_composite_uac_connect_ctrl_req(usb_dev_t *dev, u8 ch_num, u32 ch_cfg, u16 max_len)
{
	usbd_composite_uac_ac_connect_ctrl_t response = {0, 0, 0};
	usbd_ep_t *ep0_in = &dev->ep0_in;
	u16 len = sizeof(response);

	response.bNrChannels = ch_num;
	response.bmChannelConfig = ch_cfg;

	if (len > max_len) {
		len = max_len;
	}

	usb_os_memcpy(ep0_in->xfer_buf, &response, len);
	ep0_in->xfer_len = len;
	usbd_ep_transmit(dev, ep0_in);
}


/**
  * @brief  Set UAC class configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_composite_uac_set_config(usb_dev_t *dev, u8 config)
{
	UNUSED(dev);
	UNUSED(config);
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_dev_t *cdev = uac->cdev;
	usbd_composite_uac_buf_ctrl_t *pbuf_ctrl = &(uac->uac_isoc_out);
	usbd_composite_uac_buf_t *pbuf_data;
	usbd_ep_t *ep_isoc_out = &uac->ep_isoc_out;
	u16 buf_list_cnt;
	u8 idx;

	uac->alt_setting = 0U;

	/* Init ISOC OUT EP */
	usbd_composite_uac_ep_update_mps(pbuf_ctrl, (usbd_audio_cfg_t *) & (pbuf_ctrl->audio_config), cdev->dev->dev_speed);

	buf_list_cnt = usbd_composite_uac_get_ring_buf_cnt(cdev->dev->dev_speed);

	for (idx = 0; idx < buf_list_cnt; idx ++) {
		pbuf_data = &(pbuf_ctrl->buf_list_node[idx]);
		pbuf_data->buf_valid_len = 0;
		pbuf_data->is_zero_pkt = 0;
		pbuf_data->buf_raw = pbuf_ctrl->isoc_buf + CACHE_LINE_ALIGNMENT(pbuf_ctrl->isoc_mps) * idx;
		usbd_composite_uac_list_add_tail(&(pbuf_ctrl->empty_list), pbuf_data);
	}

	//usbd_audio_cfg_t *paudio_cfg = &(pbuf_ctrl->audio_config);
	//RTK_LOGS(TAG, RTK_LOG_DEBUG, "Fmt %d-%d-%d speed %d mps %d-%d cnt %d\n", paudio_cfg->ch_cnt , paudio_cfg->byte_width , paudio_cfg->sampling_freq, cdev->dev->dev_speed, pbuf_ctrl->isoc_mps, CACHE_LINE_ALIGNMENT(pbuf_ctrl->isoc_mps), buf_list_cnt);

	pbuf_data = usbd_composite_uac_list_remove_head(&(pbuf_ctrl->empty_list));
	if (NULL == pbuf_data) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "List is empty\n");
		return HAL_ERR_MEM;
	}

	ep_isoc_out->mps = pbuf_ctrl->isoc_mps;
	usbd_ep_init(cdev->dev, ep_isoc_out);
	ep_isoc_out->xfer_buf = pbuf_data->buf_raw;
	ep_isoc_out->xfer_len = pbuf_ctrl->isoc_mps;
	usbd_ep_receive(cdev->dev, ep_isoc_out);

	pbuf_ctrl->p_cur_buf_node = pbuf_data;

	if (uac->cb->set_config != NULL) {
		uac->cb->set_config();
	}

	return HAL_OK;
}

/**
  * @brief  Clear UAC configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_composite_uac_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = 0U;
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_ep_t *ep_isoc_in = &uac->ep_isoc_in;
	usbd_ep_t *ep_isoc_out = &uac->ep_isoc_out;

	UNUSED(config);

	/* DeInit ISOC IN EP */
	if (usbd_composite_uac_ep_enable(&(uac->uac_isoc_in.audio_config))) {
		usbd_ep_deinit(dev, ep_isoc_in);
	}

	/* DeInit ISOC OUT EP */
	if (usbd_composite_uac_ep_enable(&(uac->uac_isoc_out.audio_config))) {
		usbd_ep_deinit(dev, ep_isoc_out);
	}

	return ret;
}

/**
  * @brief  Check sampling_freq requests
  * @param  sampling_freq: Freq requests from host
  * @param  speed: USB connection speed
  * @retval Status
  */
static int usbd_composite_uac_is_valid_sample_rate(u32 sampling_freq, u8 speed)
{
	u8 count;

	if (speed == USB_SPEED_HIGH) {
		count = USBD_UAC_HS_SAMPLING_FREQ_COUNT;
	} else {
		count = USBD_UAC_FS_SAMPLING_FREQ_COUNT;
	}

	for (u8 i = 0; i < count; ++i) {
		if (sampling_freq == usbd_composite_uac_sampling_freq[i]) {
			return HAL_OK;
		}
	}

	return HAL_ERR_PARA;
}

/**
  * @brief  Calculate the PC's intended volume level based on the UAC driver value
  * @param  x_points: Array of volume levels on the Windows PC
  * @param  y_points: Corresponding array of volume levels used by the UAC driver
  * @param  num_points: Number of data points (at least 2)
  * @param  target_y: The volume level from the UAC driver to be interpolated
  * @retval The intended volume level on the Windows PC corresponding to the UAC driver value
  */
static u8 usbd_composite_uac_volume_linear_interpolation(u8 x_points[], s16 y_points[], u8 num_points, s16 target_y)
{
	u8 i;
	u8 x0;
	u8 x1;
	s16 y0;
	s16 y1;
	s32 numerator;
	s32 denominator;
	s32 x;

	if (target_y > 190 || target_y < -190) {
		return UABD_UAC_VOL_ERR_VAL;
	}

	for (i = 0; i < num_points - 1; i++) {
		y0 = y_points[i];
		y1 = y_points[i + 1];
		x0 = x_points[i];
		x1 = x_points[i + 1];

		if ((y0 <= target_y && target_y <= y1) || (y1 <= target_y && target_y <= y0)) {
			numerator = (s32)(target_y - y0) * (x1 - x0);
			denominator = (y1 - y0);
			x = (numerator + denominator / 2) / denominator + x0;
			if (x < 0) {
				x = 0;
			}
			if (x > 100) {
				x = 100;
			}
			return (u8)x;
		}
	}

	return UABD_UAC_VOL_ERR_VAL;
}

/**
  * @brief  Handle UAC specific CTRL requests
  * @param  dev: USB device instance
  * @param  req: USB CTRL requests
  * @retval Status
  */
static int usbd_composite_uac_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_usr_cb_t *cb = uac->cb;
	usbd_audio_cfg_t *audio_cfg = NULL;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	u8 entityId, controlSelector, ch_cnt, byte_width, alt_setting, fmt_change = 0;
	int ret = HAL_OK;

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				alt_setting = USB_LOW_BYTE(req->wValue);
				if ((alt_setting != uac->alt_setting) && alt_setting) {
					audio_cfg = &(uac->uac_isoc_out.audio_config);
					fmt_change = 0;
					uac->alt_setting = alt_setting;
					// RTK_LOGS(TAG, RTK_LOG_DEBUG, "Set new alt %d old %d-%d-%d\n",uac->alt_setting,audio_cfg->byte_width,audio_cfg->ch_cnt,audio_cfg->sampling_freq);
					switch (uac->alt_setting) {
					case 1:
						byte_width = 2;
						ch_cnt = 2;
						break;
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
					case 2:
						byte_width = 2;
						ch_cnt = 4;
						break;
#endif
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
					case 3:
						byte_width = 2;
						ch_cnt = 6;
						break;
#endif
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
					case 4:
						byte_width = 2;
						ch_cnt = 8;
						break;
#endif
					default:
						ret = HAL_ERR_PARA;
						break;
					}

					if (ret == HAL_OK) {
						if ((audio_cfg->byte_width != byte_width) && byte_width) {
							audio_cfg->byte_width = byte_width;
							fmt_change = 1;
							// RTK_LOGS(TAG, RTK_LOG_DEBUG, "set byte_width %d\n",byte_width);
						}
						if ((audio_cfg->ch_cnt != ch_cnt) && ch_cnt) {
							audio_cfg->ch_cnt = ch_cnt;
							fmt_change = 1;
							// RTK_LOGS(TAG, RTK_LOG_DEBUG, "set ch_cnt %d\n",ch_cnt);
						}
						//update the issue
						// RTK_LOGS(TAG, RTK_LOG_DEBUG, "fmt_change %d\n",fmt_change);
						if (fmt_change) {
							if (cb->format_changed != NULL) {
								cb->format_changed(audio_cfg->sampling_freq, audio_cfg->ch_cnt, audio_cfg->byte_width);
							}
						}
					}
				}
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				if (USB_LOW_BYTE(req->wIndex) == USBD_COMP_UAC_AS_HEADSET_HEADPHONES) {
					ep0_in->xfer_buf[0] = uac->alt_setting;
					ep0_in->xfer_len = 1U;
					usbd_ep_transmit(dev, ep0_in);
				} else {
					ep0_in->xfer_buf[0] = 0U;
					ep0_in->xfer_len = 1U;
					usbd_ep_transmit(dev, ep0_in);
				}
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		case USB_REQ_GET_STATUS:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		default:
			ret = HAL_ERR_PARA;
			break;
		}
		break;/* case USB_REQ_TYPE_STANDARD */

	case USB_REQ_TYPE_CLASS :
		entityId = USB_HIGH_BYTE(req->wIndex);
		controlSelector = USB_HIGH_BYTE(req->wValue);

		// RTK_LOGS(TAG, RTK_LOG_DEBUG, "UAC SETUP: id=0x%02x cs=%d\n", entityId, controlSelector);

		if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
			switch (entityId) {
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE:
				if (controlSelector == USBD_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						audio_cfg = &(uac->uac_isoc_in.audio_config);
						usbd_composite_uac_cur_sampling_freq_req(dev, audio_cfg->sampling_freq);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
						usbd_composite_uac_sampling_freq_ctrl_range_req(dev, req->wLength);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES:
				if (controlSelector == USBD_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						audio_cfg = &(uac->uac_isoc_out.audio_config);
						usbd_composite_uac_cur_sampling_freq_req(dev, audio_cfg->sampling_freq);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
						usbd_composite_uac_sampling_freq_ctrl_range_req(dev, req->wLength);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USBD_UAC_CS_CLK_VALID_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usbd_composite_uac_clk_valid_req(dev, uac->cur_clk_valid);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE: //in
				if (controlSelector == USBD_UAC_TE_CONNECTOR_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usbd_composite_uac_connect_ctrl_req(dev, 1, 0x1, req->wLength);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES: //out
				if (controlSelector == USBD_UAC_TE_CONNECTOR_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						ch_cnt = uac->uac_isoc_out.audio_config.ch_cnt;
						usbd_composite_uac_connect_ctrl_req(dev, ch_cnt, usbd_composite_uac_get_ch_config(ch_cnt), req->wLength);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT:
				if (controlSelector == USBD_UAC_CTRL_FU_MUTE_CONTROL_SELECTOR) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						ep0_in->xfer_buf[0] = uac->cur_mute;
						ep0_in->xfer_len = 1U;
						usbd_ep_transmit(dev, ep0_in);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USBD_UAC_CTRL_FU_VOLUME_CONTROL_SELECTOR) { //volume ctrl  //02
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {  //01  cur volume
						usb_os_memcpy(ep0_in->xfer_buf, (u8 *) & (uac->cur_volume), 2);
						ep0_in->xfer_len = 2U;
						usbd_ep_transmit(dev, ep0_in);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {  //02 volume ctrl range

						usbd_composite_uac_ctrl_range_layout2_struct response;
						response.wNumSubRanges = 1;
						response.wMIN = USBD_UAC_VOLUME_CTRL_MIN;
						response.wMAX = USBD_UAC_VOLUME_CTRL_MAX;
						response.wRES = 1;

						usb_os_memcpy(ep0_in->xfer_buf, &response, sizeof(response));
						ep0_in->xfer_len = sizeof(response);
						usbd_ep_transmit(dev, ep0_in);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: 0x13 wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;

			default:
				RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: wIndex err %d\n", entityId);
				break;
			}
		} else {
			/* USB_H2D */
			switch (entityId) {
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES:
				if (controlSelector == USBD_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usb_os_memcpy((void *)&uac->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						ep0_out->xfer_len = req->wLength;
						usbd_ep_receive(dev, ep0_out);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
						// Do nothing
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "Set sampling_freq err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Set sampling_freq ctrl err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;/* case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES */

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT: //0x05 FU
				if (controlSelector == USBD_UAC_CTRL_FU_MUTE_CONTROL_SELECTOR) { //mute
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usb_os_memcpy((void *)&uac->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						ep0_out->xfer_len = req->wLength;
						usbd_ep_receive(dev, ep0_out);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
						// Do nothing
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "Set cur mute err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USBD_UAC_CTRL_FU_VOLUME_CONTROL_SELECTOR) { //volume
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usb_os_memcpy((void *)&uac->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						ep0_out->xfer_len = req->wLength;
						usbd_ep_receive(dev, ep0_out);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "Set cur volume range err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Set fu err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_HW;
				}
				break;/* case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT */

			default:
				RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: wIndex err %d\n", entityId);
				break;
			}
		}
		break;/* case USB_REQ_TYPE_CLASS */

	case USB_REQ_TYPE_VENDOR:
		if (req->wLength) {
			if ((req->bmRequestType & 0x80U) && (uac->cb->setup != NULL)) {
				ret = uac->cb->setup(req, ep0_in->xfer_buf);
				if (ret == HAL_OK) {
					ep0_in->xfer_len = req->wLength;
					usbd_ep_transmit(dev, ep0_in);
				}
			} else {
				usb_os_memcpy((void *)&uac->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
				ep0_out->xfer_len = req->wLength;
				usbd_ep_receive(dev, ep0_out);
			}
		} else {
			uac->cb->setup(req, NULL);
		}
		break;

	default:
		RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bmRequestType err\n");
		ret = HAL_ERR_HW;
		break;
	}

	return ret;
}

/**
  * @brief  Data sent on non-control IN endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_composite_uac_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	UNUSED(dev);
	usbd_composite_uac_device_t *cdev = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_in);

	if (pdata_ctrl->next_xfer) {
		if (ep_addr == USBD_COMP_UAC_ISOC_IN_EP) {
			if (status != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "ISOC TX err: %d\n", status);
			}
			//loop to next tx TODO
		}
	}

	return HAL_OK;
}

/**
  * @brief  uac_handle_ep0_data_out
  *         Handle EP0 Rx Ready event
  * @param  dev: USB device instance
  * @retval Status
  */
static int usbd_composite_uac_handle_ep0_data_out(usb_dev_t *dev)
{
	UNUSED(dev);
	int ret = HAL_OK;
	u8 num_points;
	u8 target_volume;
	s16 volume_value = 0;
	u32 sampling_freq;
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_usr_cb_t *cb = uac->cb;
	usb_setup_req_t *p_ctrl_req = &(uac->ctrl_req);
	usbd_audio_cfg_t *audio_cfg = NULL;
	usbd_ep_t *ep0_out = &dev->ep0_out;

	// RTK_LOGS(TAG, RTK_LOG_DEBUG, RTK_LOG_INFO, "EP0 Out: bmRequestType=0x%02x bRequest=0x%02x wValue=%x wIndex=%x wLength=0x%04x\n",
	// 	p_ctrl_req->bmRequestType, p_ctrl_req->bRequest, p_ctrl_req->wValue, p_ctrl_req->wIndex, p_ctrl_req->wLength);
	if ((((p_ctrl_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS) && ((p_ctrl_req->bmRequestType & 0x1FU) == USB_REQ_RECIPIENT_INTERFACE))
		&& (p_ctrl_req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR)) {

		if (USB_HIGH_BYTE(p_ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT) {
			if ((USB_HIGH_BYTE(p_ctrl_req->wValue) == USBD_UAC_CTRL_FU_MUTE_CONTROL_SELECTOR) && (p_ctrl_req->wLength == 0x01)) {
				if (cb->mute_changed != NULL) {
					uac->cur_mute = (ep0_out->xfer_buf[0]) ? 1 : 0;
					cb->mute_changed(uac->cur_mute);
				}
			} else if ((USB_HIGH_BYTE(p_ctrl_req->wValue) == USBD_UAC_CTRL_FU_VOLUME_CONTROL_SELECTOR) && (p_ctrl_req->wLength == 0x02)) {
				if (cb->volume_changed != NULL) {
					volume_value = (u16)ep0_out->xfer_buf[0] + ((u16)(ep0_out->xfer_buf[1]) << 8);
					uac->cur_volume = volume_value;

					//RTK_LOGS(TAG, RTK_LOG_DEBUG, "Set volume 0x%02x\n", volume_value);

					num_points = sizeof(usbd_composite_uac_pc_vol_lvl) / sizeof(usbd_composite_uac_pc_vol_lvl[0]);
					if (num_points != sizeof(usbd_composite_uac_drv_vol) / sizeof(usbd_composite_uac_drv_vol[0])) {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "Volume arrays length err.\n");
						ret = HAL_ERR_PARA;
					} else {
						target_volume = usbd_composite_uac_volume_linear_interpolation(usbd_composite_uac_pc_vol_lvl, \
										usbd_composite_uac_drv_vol, num_points, (int)volume_value);
					}

					if (target_volume != UABD_UAC_VOL_ERR_VAL) {
						cb->volume_changed(target_volume);
					}
				}
			}
		}

		if ((USB_HIGH_BYTE(p_ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES)
			&& (USB_HIGH_BYTE(p_ctrl_req->wValue) == USBD_UAC_CS_SAM_FREQ_CONTROL) && (p_ctrl_req->wLength == 0x04)) {
			sampling_freq = (ep0_out->xfer_buf[3] << 24) | (ep0_out->xfer_buf[2] << 16) | (ep0_out->xfer_buf[1] << 8) | ep0_out->xfer_buf[0];

			if (usbd_composite_uac_is_valid_sample_rate(sampling_freq, dev->dev_speed) == HAL_OK) {
				audio_cfg = &(uac->uac_isoc_out.audio_config);
				if (audio_cfg->sampling_freq != sampling_freq) {
					audio_cfg->sampling_freq = sampling_freq;

					if (cb->format_changed != NULL) {
						cb->format_changed(audio_cfg->sampling_freq, audio_cfg->ch_cnt, audio_cfg->byte_width);
					}
				}
			} else {
				// Do nothing, return HAL_ERR_PARA(STALL) will cause PC continue setting invalid sampling_freq request
			}
		}
		// To do: handle vendor
	}

	return ret;
}

static int usbd_composite_uac_sof(usb_dev_t *dev)
{
	UNUSED(dev);
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *pdata_ctrl = &(uac->uac_isoc_out);

	if ((pdata_ctrl->next_xfer) && (uac->isoc_rx_valid_cnt > 0)) {
		if (pdata_ctrl->sof_idx == 0) {
			pdata_ctrl->sof_idx = 1 ;
			pdata_ctrl->data_idx = 0 ;
		} else {
			pdata_ctrl->sof_idx ++;
		}

		if ((pdata_ctrl->sof_idx > pdata_ctrl->data_idx) && (pdata_ctrl->sof_idx - pdata_ctrl->data_idx >= USBD_UAC_ISOC_RX_APPEND_ZLP_DIFF_MAX)) {
			usbd_composite_uac_append_data();
			pdata_ctrl->data_idx ++;
		}
	}

	return HAL_OK;
}

/**
  * @brief  Data received on non-control Out endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_composite_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *pdata_ctrl = &(uac->uac_isoc_out);
	usbd_composite_dev_t *cdev = uac->cdev;
	usbd_composite_uac_buf_t *p_buf = NULL;
	usbd_ep_t *ep_isoc_out = &uac->ep_isoc_out;
	UNUSED(dev);

	// RTK_LOGS(TAG, RTK_LOG_DEBUG, "Read data out %d\n",pdata_ctrl->next_xfer);
#if USBD_COMPOSITE_UAC_DEBUG
	uac->isoc_rx_cnt ++;
#endif

	p_buf = pdata_ctrl->p_cur_buf_node;
	if (pdata_ctrl->next_xfer) {
		if (ep_addr == USBD_COMP_UAC_ISOC_OUT_EP) {
			if (len == 0) { //ZLP
				ep_isoc_out->xfer_buf = p_buf->buf_raw;
				ep_isoc_out->xfer_len = pdata_ctrl->isoc_mps;
				usbd_ep_receive(cdev->dev, ep_isoc_out);
#if USBD_COMPOSITE_UAC_DEBUG
				uac->isoc_rx_zlp_cnt ++;
#endif
			} else {
				uac->isoc_rx_len = len;
				uac->isoc_rx_valid_cnt ++;
#if USBD_COMPOSITE_UAC_DEBUG
				u32 g_rx_new_tick = usb_os_get_timestamp_us(); //us
				if ((uac->isoc_rx_last_tick > 0) && (g_rx_new_tick - uac->isoc_rx_last_tick > uac->isoc_timeout_max_step)) {
					uac->isoc_timeout_cnt ++;
					uac->isoc_timeout_max_value = g_rx_new_tick - uac->isoc_rx_last_tick;
				}
				uac->isoc_rx_last_tick = g_rx_new_tick;
#endif

				pdata_ctrl->data_idx ++;

				p_buf->buf_valid_len = len;
				p_buf->is_zero_pkt = 0;
				/*
					if no read, the function will overwrite the data, and the read_pos should be updated
				*/
				if (pdata_ctrl->empty_list.count == 0) { //full
					usbd_composite_uac_buf_t *node = usbd_composite_uac_list_remove_head(&(pdata_ctrl->data_list));
					if (node) {
						usbd_composite_uac_list_add_tail(&(pdata_ctrl->empty_list), node);
					}
#if USBD_COMPOSITE_UAC_DEBUG
					uac->isoc_overwrite_cnt ++;
#endif
				}
				usbd_composite_uac_list_add_tail(&(pdata_ctrl->data_list), p_buf);

				p_buf = usbd_composite_uac_list_remove_head(&(pdata_ctrl->empty_list));
				if (p_buf) {
					ep_isoc_out->xfer_buf = p_buf->buf_raw;
					ep_isoc_out->xfer_len = pdata_ctrl->isoc_mps;
					usbd_ep_receive(cdev->dev, ep_isoc_out);
					pdata_ctrl->p_cur_buf_node = p_buf;

					//used for usb read
					if (usbd_composite_uac_get_read_frame_cnt() >= USBD_UAC_HS_SOF_COUNT_PER_MS) {
						rtos_sema_give(pdata_ctrl->uac_isoc_sema);
					}
				}
			}
		}
	} else {
		p_buf->buf_valid_len = len;
		ep_isoc_out->xfer_buf = p_buf->buf_raw;
		ep_isoc_out->xfer_len = pdata_ctrl->isoc_mps;
		usbd_ep_receive(cdev->dev, ep_isoc_out);
	}

	return HAL_OK;
}

/**
  * @brief  Get descriptor callback
  * @param  dev: USB device instance
  * @param  req: Setup request handle
  * @param  buf: Poniter to Buffer
  * @retval Descriptor length
  */
static u16 usbd_composite_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf)
{
	usb_speed_type_t speed = dev->dev_speed;
	u8 *desc = NULL;
	u16 len = 0;
	dev->self_powered = USBD_UAC_SELF_POWERED;

#if USBD_COMPOSITE_UAC_DEBUG
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	if (speed == USB_SPEED_HIGH) { //hs
		uac->isoc_timeout_max_step = 175;
	} else {
		uac->isoc_timeout_max_step = 1500;
	}
#endif

	switch (USB_HIGH_BYTE(req->wValue)) {
	case USB_DESC_TYPE_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			desc = (u8 *)usbd_composite_uac_hs_itf_desc;
			len = sizeof(usbd_composite_uac_hs_itf_desc);
		} else {
			desc = (u8 *)usbd_composite_uac_fs_itf_desc;
			len = sizeof(usbd_composite_uac_fs_itf_desc);
		}
		usb_os_memcpy((void *)buf, (void *)desc, len);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			desc = (u8 *)usbd_composite_uac_fs_itf_desc;
			len = sizeof(usbd_composite_uac_fs_itf_desc);
		} else {
			desc = (u8 *)usbd_composite_uac_hs_itf_desc;
			len = sizeof(usbd_composite_uac_hs_itf_desc);
		}
		usb_os_memcpy((void *)buf, (void *)desc, len);
		break;

	default:
		break;
	}

	return len;
}

/**
  * @brief  USB attach status change
  * @param  dev: USB device instance
  * @param  old_status: USB old attach status
  * @param  status: USB attach status
  * @retval void
  */
static void usbd_composite_uac_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;

	UNUSED(dev);

	if (uac->cb->status_changed) {
		uac->cb->status_changed(old_status, status);
	}
}

#if USBD_COMPOSITE_UAC_DEBUG

/**
  * @brief  UAC status dump thread
  * @param  param: Pointer to parameters
  * @retval void
  */
static void usbd_composite_uac_status_dump_thread(void *param)
{
	UNUSED(param);
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;

	uac->uac_dump_task_alive = 1;

	while (1) {
		if (uac && uac->cdev && uac->cdev->dev) {
			usb_dev_t *dev = uac->cdev->dev;
			RTK_LOGS(TAG, RTK_LOG_INFO, "USB RX %d-%d-%d/TO %d-%d/zp %d-%d-%d/OW %d/len %d-%d/%d/IsrC %d-%d/isrT %d-%d\n",
					 uac->isoc_rx_valid_cnt, uac->isoc_rx_cnt, uac->isoc_rx_zlp_cnt,
					 uac->isoc_timeout_cnt, uac->isoc_timeout_max_value,
					 uac->isoc_zlp_cnt, (u32)(uac->uac_isoc_out.sof_idx), (u32)(uac->uac_isoc_out.data_idx),
					 uac->isoc_overwrite_cnt,
					 uac->copy_data_len, uac->isoc_rx_len,
					 uac->uac_isoc_out.next_xfer,
					 dev->isr_process_time_max, dev->isr_process_time,
					 dev->isr_enter_period_max, dev->isr_enter_period
					);
		}
		rtos_time_delay_ms(USBD_UAC_DEBUG_LOOP_TIME);
	}

	rtos_task_delete(NULL);
}

static void composite_usbd_comp_reset_isr_time(void)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usb_dev_t *dev;

	if ((NULL == uac) || (NULL == uac->cdev) || (NULL == uac->cdev->dev)) {
		return;
	}

	dev = uac->cdev->dev;

	dev->isr_process_time_max = 0;
	dev->isr_enter_period_max = 0;
}

static u32 composite_usbd_comp_hid_test(u16 argc, u8 *argv[])
{
	int status = HAL_OK;
	char *cmd;

	if (argc == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid argument\n");
		return HAL_ERR_PARA;
	}

	cmd = (char *)argv[0];

	if (_stricmp(cmd, "reset") == 0) {
		composite_usbd_comp_reset_isr_time();
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Input cmd err %s\n", cmd);
	}

	return status;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE test_composite_usbd_hid_cmd_table[] = {
	{
		(const u8 *)"uac", 3, composite_usbd_comp_hid_test, (const u8 *)"\tUSB device test cmd:\n"
		"\t\t uac reset\n"
	}
};

/**
  * @brief  Update or manage the count of audio data bytes
  * @param  audio_len: Data len had been wrote to the audiotrack
  * @retval void
  */
static inline void usbd_composite_uac_get_audio_data_cnt(u32 audio_len)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	uac->copy_data_len += audio_len;
}
#endif

/**
  * @brief  Read data from a USB audio ring buffer
  * @param  pdata_ctrl: Pointer to the buffer control structure
  * @param  buffer: Pointer to Data buffer
  * @param  size: Data length
  * @param  copy_len: Copy length
  * @param  pkt_cnt: copy packet count
  * @param  zero_pkt_flag: zero packet ,each bit means a zero packet
  * @retval Status 0 means get enought data, 1 means should continue to read the pkt
  */
static u32 usbd_composite_uac_read_ring_buf(usbd_composite_uac_buf_ctrl_t *pdata_ctrl, u8 *buffer, u32 size, u32 *copy_len, u16 *pkt_cnt, u32 *zero_pkt_flag)
{
	usbd_composite_uac_buf_t *p_buf = NULL;

	do {
		p_buf = pdata_ctrl->data_list.head;
		if (p_buf == NULL) {
			break;
		}

		/* should exit : 1) Enough data has been obtained; 2) the next data cannot be saved completely */
		if ((*copy_len >= size) || (*copy_len + p_buf->buf_valid_len > size)) {
			return 0;
		}

		p_buf = usbd_composite_uac_list_remove_head(&(pdata_ctrl->data_list));
		if (p_buf->buf_valid_len > 0) {

			usb_os_memcpy(buffer + *copy_len, p_buf->buf_raw, p_buf->buf_valid_len);
			*copy_len += p_buf->buf_valid_len;
			usb_os_memset(p_buf->buf_raw, 0x00, p_buf->buf_valid_len);

			if (p_buf->is_zero_pkt && zero_pkt_flag) {
				*zero_pkt_flag |= 1 << *pkt_cnt;
			}

			*pkt_cnt = *pkt_cnt + 1;
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Buflen is zero\n");
		}
		usbd_composite_uac_list_add_tail(&(pdata_ctrl->empty_list), p_buf);

	} while (pdata_ctrl->data_list.count > 0);

	/* should return 0 : enough data has been obtained; */
	if (*copy_len >= size) {
		return 0;
	}

	return 1;
}

/* Exported functions --------------------------------------------------------*/

int usbd_composite_uac_init(usbd_composite_dev_t *cdev, usbd_composite_uac_usr_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *pbuf_ctrl;
	usbd_ep_t *ep_isoc_out = &uac->ep_isoc_out;
	usbd_ep_t *ep_isoc_in = &uac->ep_isoc_in;
	usb_speed_type_t speed = cdev->dev->dev_speed;

	usb_os_memset(uac, 0x00, sizeof(usbd_composite_uac_device_t));

	uac->cur_volume = 0x001F;
	uac->cur_mute = 0;
	uac->cur_clk_valid = 1;
#if USBD_COMPOSITE_UAC_DEBUG
	uac->uac_dump_task_alive = 0;
#endif
	usbd_composite_uac_ep_buf_ctrl_deinit(&(uac->uac_isoc_in));
	usbd_composite_uac_ep_buf_ctrl_deinit(&(uac->uac_isoc_out));

	ep_isoc_out->addr = USBD_COMP_UAC_ISOC_OUT_EP;
	ep_isoc_out->type = USB_CH_EP_TYPE_ISOC;
	ep_isoc_out->binterval = (speed == USB_SPEED_HIGH) ? USBD_UAC_HS_DEFAULT_BINTERVAL : USBD_UAC_FS_DEFAULT_BINTERVAL;

	ep_isoc_in->addr = USBD_COMP_UAC_ISOC_IN_EP;
	ep_isoc_in->type = USB_CH_EP_TYPE_ISOC;
	ep_isoc_in->binterval = (speed == USB_SPEED_HIGH) ? USBD_UAC_HS_DEFAULT_BINTERVAL : USBD_UAC_FS_DEFAULT_BINTERVAL;

	if (cb != NULL) {
		if ((cb->in.enable == 0) && (cb->out.enable == 0)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Pls cfg UAC EP\n");
			return HAL_ERR_PARA;
		}

		uac->cb = cb;

		usb_os_memcpy(&(uac->uac_isoc_in.audio_config), &(cb->in), sizeof(usbd_audio_cfg_t));
		usb_os_memcpy(&(uac->uac_isoc_out.audio_config), &(cb->out), sizeof(usbd_audio_cfg_t));

		uac->uac_isoc_out.audio_config.ch_cnt = USBD_UAC_DEFAULT_CH_CNT;//init the default value
		uac->uac_isoc_out.audio_config.sampling_freq = USBD_UAC_SAMPLING_FREQ_48K;
		uac->uac_isoc_out.audio_config.byte_width = USBD_UAC_DEFAULT_BYTE_WIDTH;

		pbuf_ctrl = &(uac->uac_isoc_out);
		if (HAL_OK != usbd_composite_uac_ep_buf_ctrl_init(pbuf_ctrl, (usbd_audio_cfg_t *) & (pbuf_ctrl->audio_config))) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Buf Init Fail\n");
			return HAL_ERR_PARA;
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC cb is NULL\n");
		return HAL_ERR_PARA;
	}

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			return ret;
		}
	}

	uac->cdev = cdev;

#if USBD_COMPOSITE_UAC_DEBUG
	if (rtos_task_create(&(uac->uac_dump_task), ((const char *)"usbd_composite_uac_status_dump_thread"), usbd_composite_uac_status_dump_thread, NULL, 1024U,
						 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create usb status dump task fail\n");
	}
#endif

	return ret;
}

/**
  * @brief  DeInitialize UAC device
  * @param  void
  * @retval Status
  */
int usbd_composite_uac_deinit(void)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;

#if USBD_COMPOSITE_UAC_DEBUG
	if (uac->uac_dump_task_alive) {
		rtos_task_delete(uac->uac_dump_task);
		uac->uac_dump_task_alive = 0;
	}
#endif

	if (uac->cb != NULL) {
		if (uac->cb->deinit != NULL) {
			uac->cb->deinit();
		}
		uac->cb = NULL;
	}

	usbd_composite_uac_ep_buf_ctrl_deinit(&(uac->uac_isoc_in));
	usbd_composite_uac_ep_buf_ctrl_deinit(&(uac->uac_isoc_out));

	return HAL_OK;
}

/**
  * @brief  Transmit UAC data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbd_composite_uac_transmit_data(u8 *buf, u32 len)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usb_dev_t *dev = uac->cdev->dev;

	if (!dev->is_ready) {
		return HAL_ERR_HW;
	}

	if (len > uac->uac_isoc_in.isoc_mps) {
		len = uac->uac_isoc_in.isoc_mps;
	}

	if (usbd_composite_uac_ep_enable(&(uac->uac_isoc_in.audio_config))) {
		usb_os_memcpy(uac->uac_isoc_in.isoc_buf, buf, len);
		// usbd_ep_transmit(uac->dev, USBD_COMP_UAC_ISOC_IN_EP, uac->uac_isoc_in.isoc_buf, len);
	}

	return HAL_OK;
}

/**
  * @brief  Receive UAC data
  * @param  void
  * @retval Status
  */
int usbd_composite_uac_receive_data(void)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *pbuf_ctrl = &(uac->uac_isoc_out);
	usb_dev_t *dev = uac->cdev->dev;

	if (!dev->is_ready) {
		//RTK_LOGS(TAG, RTK_LOG_ERROR, "State %d err\n", dev->is_ready);
		return HAL_ERR_PARA;
	}

	if (usbd_composite_uac_ep_enable(&(uac->uac_isoc_out.audio_config))) {
		/* empty theh data list */
		if (pbuf_ctrl->data_list.count) {
			usbd_composite_uac_buf_t *node;
			while ((node = usbd_composite_uac_list_remove_head(&(pbuf_ctrl->data_list))) != NULL) {
				usbd_composite_uac_list_add_tail(&(pbuf_ctrl->empty_list), node);
			}
		}

		pbuf_ctrl->next_xfer = 1;
		pbuf_ctrl->sof_idx = 0;
		pbuf_ctrl->data_idx = 0;

#if USBD_COMPOSITE_UAC_DEBUG
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "First trigger sema %d-%d \n", pbuf_ctrl->read_wait_sema, pbuf_ctrl->isoc_mps);
#endif
	}

	return HAL_OK;
}

/**
  * @brief  Initialize UAC device configuration
  * @param  uac_cfg: USB audio config
  * @param  is_record: 1 for recording, 0 for playback
  * @param  flag: Unused parameter
  * @retval Status
  */
u8 usbd_composite_uac_config(const usbd_audio_cfg_t *uac_cfg, u8 is_record, u32 flag)
{
	UNUSED(flag);

	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_dev_t *cdev = uac->cdev;
	usbd_composite_uac_buf_ctrl_t *pbuf_ctrl;

	/* all the transfer should finish */
	/* TODO: re initiation the isoc buffer[usbd_composite_uac_buf_ctrl_t] */
	if (uac_cfg && usbd_composite_uac_ep_enable((usbd_audio_cfg_t *)uac_cfg)) {
		if (is_record) { //
			pbuf_ctrl = &(uac->uac_isoc_in);
		} else { //
			pbuf_ctrl = &(uac->uac_isoc_out);
		}
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "UAC cfg\n");
		pbuf_ctrl->next_xfer = 0;

		/* update the rx mps */
		usbd_composite_uac_ep_update_mps(pbuf_ctrl, (usbd_audio_cfg_t *)uac_cfg, cdev->dev->dev_speed);
	}

	return HAL_OK;
}

/**
  * @brief  Initializes and starts the UAC data rx process
  * @param  void
  * @retval Status
  */
u32 usbd_composite_uac_start_play(void)
{
	int ret = HAL_OK;
	// RTK_LOGS(TAG, RTK_LOG_DEBUG, "UAC start\n");

#if USBD_COMPOSITE_UAC_DEBUG
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	uac->isoc_rx_last_tick = 0;
#endif

	ret = usbd_composite_uac_receive_data();

	return ret;
}

/**
  * @brief  Stop UAC device play
  * @param  void
  * @retval void
  */
void usbd_composite_uac_stop_play(void)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "UAC stop\n");

	uac->uac_isoc_out.next_xfer = 0;
	uac->uac_isoc_in.next_xfer = 0;
}

/**
  * @brief  Read UAC data
  * @param  buf: Pointer to data buffer
  * @param  size: Data length
  * @param  time_out_ms: Time out
  * @param  zero_pkt_flag: zero packet ,each bit means a zero packet
  * @retval copy len
  */
u32 usbd_composite_uac_read(u8 *buffer, u32 size, u32 time_out_ms, u32 *zero_pkt_flag)
{
	usbd_composite_uac_device_t *cdev = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_out);
	u32 copy_len = 0;
	u16 pkt_cnt = 0;

	if (pdata_ctrl->next_xfer == 0) {
		return 0;
	}

	if (time_out_ms == 0) {
		if (pdata_ctrl->data_list.count == 0) { //no valid data
			return 0;
		}

		usbd_composite_uac_read_ring_buf(pdata_ctrl, buffer, size, &copy_len, &pkt_cnt, zero_pkt_flag);
	} else {
		do {
			if (pdata_ctrl->data_list.count < USBD_UAC_HS_SOF_COUNT_PER_MS) {
				//wait sema
				pdata_ctrl->read_wait_sema = 1;
				if (rtos_sema_take(pdata_ctrl->uac_isoc_sema, time_out_ms) != RTK_SUCCESS) {
					pdata_ctrl->read_wait_sema = 0;
					break;
				}

				pdata_ctrl->read_wait_sema = 0;
			} else {
				/* if did not read any pkt, loop to check the wr/rd pos*/
				if (usbd_composite_uac_read_ring_buf(pdata_ctrl, buffer, size, &copy_len, &pkt_cnt, zero_pkt_flag) == 0) {
					break;
				}
			}
		} while (pdata_ctrl->next_xfer);
	}

#if USBD_COMPOSITE_UAC_DEBUG
	usbd_composite_uac_get_audio_data_cnt(copy_len);
#endif

	if (copy_len > size) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Pls check the uac cfg\n");
	}

	return copy_len;
}

/**
  * @brief  Get UAC read frame cnt
  * @param  void
  * @retval read frame cnt
  */
u32 usbd_composite_uac_get_read_frame_cnt(void)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *pdata_ctrl = &(uac->uac_isoc_out);

	if (pdata_ctrl == NULL) {
		return 0;
	}

	return pdata_ctrl->data_list.count;
}

/**
  * @brief  Get UAC read frame cnt
  * @param  void
  * @retval return the time duration for the avail packet in us
  */
u32 usbd_composite_uac_get_read_frame_time_in_us(void)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *pdata_ctrl = &(uac->uac_isoc_out);
	usbd_composite_dev_t *cdev = uac->cdev;

	if (pdata_ctrl == NULL) {
		return 0;
	}

	if (cdev->dev->dev_speed == USB_SPEED_HIGH) {
		return pdata_ctrl->data_list.count * USBD_UAC_POW2(USBD_UAC_HS_DEFAULT_BINTERVAL - 1) * (USBD_UAC_ONE_KHZ / USBD_UAC_HS_SOF_COUNT_PER_MS);
	} else {
		return pdata_ctrl->data_list.count * USBD_UAC_ONE_KHZ * USBD_UAC_POW2(USBD_UAC_FS_DEFAULT_BINTERVAL - 1);
	}

	return 0;
}
