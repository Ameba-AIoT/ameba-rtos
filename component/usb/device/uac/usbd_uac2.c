/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

/* uac 2.0 */
#include "usbd_uac.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/
typedef struct {
	u8  bNrChannels;
	u32 bmChannelConfig;
	u8  iChannelNames;
} __PACKED usbd_uac_ac_connect_ctrl_t;

/**
 * @brief Structure to define the volume range attribute.
 * @details This structure holds the volume range attribute,
 *          such as max, min and res.
 */
typedef struct {
	u16 wNumSubRanges;
	u16 wMIN;
	u16 wMAX;
	u16 wRES;
} __PACKED usbd_uac_ctrl_range_layout2_struct;

/**
 * @brief Structure to define the frequency.
 * @details This is a packed structure used for sending notifications like SERIAL_STATE
 *          over the INTERRUPT IN endpoint.
 */
typedef struct {
	u32 dMIN;
	u32 dMAX;
	u32 dRES;
} usbd_uac_sub_range_t;

/**
 * @brief Structure to define the frequency.
 * @details This is a packed structure used for sending notifications like SERIAL_STATE
 *          over the INTERRUPT IN endpoint.
 */
typedef struct {
	u16 wNumSubRanges;
	usbd_uac_sub_range_t usbd_uac_sub_ranges[];
} __PACKED usbd_uac_sampling_freq_ctrl_range_t;

/* Private macros ------------------------------------------------------------*/
#define UABD_UAC_DESC_DUMP (0)

#if USBD_UAC_DEBUG
#define USBD_UAC_DEBUG_LOOP_TIME   10*1000
#define USBD_UAC_DEBUG_STEP_TIME   100
#endif

#define UABD_UAC_VOL_ERR_VAL       255

#define USBD_UAC_HS_ISOC_MPS                        1024   /**< High speed ISOC IN & OUT maximum packet size */
#define USBD_UAC_FS_ISOC_MPS                        1023   /**< Full speed ISOC IN & OUT maximum packet size */

#define USBD_UAC_SELF_POWERED                       1U                    /**< Device is self-powered. */
#define USBD_UAC_LANGID_STRING                      0x0409U               /**< Language ID for string descriptors (0x0409 = English). */
#define USBD_UAC_MFG_STRING                         "Realtek"             /**< Manufacturer string. */
#define USBD_UAC_PROD_HS_STRING                     "Realtek UAC2.0 (HS)" /**< Product string for High-Speed mode. */
#define USBD_UAC_PROD_FS_STRING                     "Realtek UAC2.0 (FS)" /**< Product string for Full-Speed mode. */
#define USBD_UAC_SN_STRING                          "1234567890"          /**< Serial number string. */

/**
 * Defines Audio trx buffer MAX count.
 */
#define USBD_UAC_RX_BUF_MAX_CNT                     10     /**< RX Ringbuf count */
#define USBD_UAC_TX_BUF_MAX_CNT                     10     /**< TX Ringbuf count */

#define USBD_UAC_CLASS_CODE                         0x01U
#define USBD_UAC_SUBCLASS_AUDIOCONTROL              0x01U
#define USBD_UAC_SUBCLASS_AUDIOSTREAMING            0x02U
#define USBD_UAC_VERSION_02_00                      0x20U
#define USBD_UAC_IF_CLASS_AUDIO                     0x01U

/* 1ms 8 frame in high speed */
#define USBD_UAC_HS_SOF_COUNT_PER_MS                8U
#define USBD_UAC_ONE_KHZ                            1000U

/**
 * Defines UAC 2.0 device volume MAX & MIN.
 */
#define USBD_UAC_VOLUME_CTRL_MIN                    0xFF42  /**< UAC 2.0 device MAX volume db. */
#define USBD_UAC_VOLUME_CTRL_MAX                    0x00BE  /**< UAC 2.0 device MIN volume db. */

/**
 * Audio sample frequency
 */
#define USBD_UAC_SAMPLING_FREQ_MAX_COUNT             4U

/* limit */
#define USBD_UAC_HS_SAMPLING_FREQ_COUNT               USBD_UAC_SAMPLING_FREQ_MAX_COUNT

#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4
#define USBD_UAC_FS_SAMPLING_FREQ_COUNT               3U
#elif USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
#define USBD_UAC_FS_SAMPLING_FREQ_COUNT               2U
#endif
#else
#define USBD_UAC_FS_SAMPLING_FREQ_COUNT               USBD_UAC_SAMPLING_FREQ_MAX_COUNT
#endif

/* bit_width */
#define USBD_UAC_BIT_WIDTH(byte_width)              (8U * (byte_width))

/**
 * Audio descriptor information
 */

/**
 * Defines UAC 2.0 class-specific request codes.
 */
#define USBD_UAC_LEN_CLK_SRC_DESC                   0x08U  /**<UAC Clock Source Unit Descriptor length. */
#define USBD_UAC_LEN_AC_IN_TTY_DESC                 0x11U  /**<UAC Audio Control Input Terminal Descriptor length. */
#define USBD_UAC_LEN_AC_OUT_TTY_DESC                0x0CU  /**<UAC Audio Control Output Terminal Descriptor length. */
#define USBD_UAC_LEN_AS_IF_ALT_SET_DESC             0x10U  /**<UAC Audio Streaming Interface Descriptor length. */
#define USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC       0x06U  /**<UAC Audio Streaming Format Type Descriptor length. */
#define USBD_UAC_LEN_DATA_EP_DESC                   0x08U  /**<UAC Audio Data Endpoint Descriptor length. */
#define USBD_UAC_LEN_CTRL_IF_HEADER                 0x09U  /**<UAC Audio Control Interface Header Descriptor length. */
#define USBD_UAC_LEN_FUNC_UNIT_DESC                 0x09U  /**<UAC Audio Control Feature Unit Descriptor length. */

#define USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE       0x24U  /**<UAC Audio Interface Descriptor Type. */
#define USBD_UAC_CS_ENDPOINT_DESCRIPTOR             0x25U  /**<UAC Audio Data Endpoint Descriptor Type. */
#define USBD_UAC_IA_DESCRIPTOR                      0x0BU  /**<UAC IAD Descriptor type. */

/**
 * Audio Class-Specific Audio Streaming Interface Descriptor Subtype Codes (UAC2.0 Spec Appendix A.10)
 */
#define USBD_UAC_AS_IF_DESC_SUBTYPE_AS_GENERAL      0x01  /**<UAC Audio Streaming Interface Descriptor SubType. */
#define USBD_UAC_AS_IF_DESC_SUBTYPE_FORMAT_TYPE     0x02  /**<UAC Audio Format Type Descriptor SubType. */
/* Audio Class-Specific Audio Streaming Endpoint Descriptor Subtype Codes (UAC2.0 Spec Appendix A.13) */
#define USBD_UAC_AS_EP_DESC_SUBTYPE_EP_GENERAL      0x01  /**<UAC Audio Data Endpoint Descriptor SubType. */

#define USBD_UAC_IF_IDX_AC_HEADSET                  0x00U  /**<UAC Audio Control Interface Index. */
#define USBD_UAC_IF_IDX_AS_HEADSET_HEADPHONES       0x01U  /**<UAC Audio Streaming Headphone Interface Index. */
#define USBD_UAC_IF_IDX_AS_HEADSET_MICROPHONE       0x02U  /**<UAC Audio Streaming Microphone Interface Index. */

// Audio Function Category Codes (UAC2.0 Spec Appendix A.7)
#define USBD_UAC_FUNC_CATEGORY_CODE_DESKTOP_UNDEFINED              0x00U  /**<UAC Audio Function Undefined type. */
#define USBD_UAC_FUNC_CATEGORY_CODE_DESKTOP_SPEAKER                0x01U  /**<UAC Audio Function Speaker. */
#define USBD_UAC_FUNC_CATEGORY_CODE_DESKTOP_HEADSET                0x04U  /**<UAC Audio Function Headset. */

// Audio Class-Specific Audio Control Interface Descriptor Subtype Codes (UAC2.0 Spec Appendix A.9)
#define USBD_UAC_AC_IF_DESC_SUBTYPE_UNDEFINED                      0x00U  /**<UAC Audio Function Headset. */
#define USBD_UAC_AC_IF_DESC_SUBTYPE_HEADER                         0x01U  /**<UAC Audio Function Headset. */
#define USBD_UAC_AC_IF_DESC_SUBTYPE_INPUT_TERMINAL                 0x02U  /**<UAC Audio Function Headset. */
#define USBD_UAC_AC_IF_DESC_SUBTYPE_OUTPUT_TERMINAL                0x03U  /**<UAC Audio Function Headset. */
#define USBD_UAC_AC_IF_DESC_SUBTYPE_FEATURE_UNIT                   0x06U  /**<UAC Audio Function Headset. */
#define USBD_UAC_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE                   0x0AU  /**<UAC Audio Function Headset. */

/**
 * Defines USB Audio Microphone entity id.
 */
#define USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE            0x12U /**< Define microphone clock id. */
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE    0x02U /**< Define microphone input terminal id. */
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT           0x08U /**< Define microphone feature unit id. */
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE   0x10U /**< Define microphone output terminal id. */

/**
 * Defines USB Audio Headphone entity id.
 */
#define USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES            0x15U /**< Define headphone clock id. */
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES    0x01U /**< Define headphone input terminal id. */
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT          0x05U /**< Define headphone feature unit id. */
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES   0x09U /**< Define headphone output terminal id. */

// (UAC2.0 Spec Appendix A-14)
#define USBD_UAC_CLASS_REQ_CODE_CUR                                0x01U /**< UAC Audio Class-Specific Request CUR code. */
#define USBD_UAC_CLASS_REQ_CODE_RANGE                              0x02U /**< UAC Audio Class-Specific Request RANGE code. */
// (UAC2.0 Spec Appendix A-17)
#define USBD_UAC_CS_SAM_FREQ_CONTROL                               0x01U /**< UAC Audio Clock Source Sample Frequency. */
#define USBD_UAC_CS_CLK_VALID_CONTROL                              0x02U /**< UAC Audio Clock Source Clock Valid Control. */
// (UAC2.0 Spec Appendix A-20)
#define USBD_UAC_TE_CONNECTOR_CONTROL                              0x02U /**< UAC Audio Terminal Control Selectors CONNECTOR. */

/*! @brief Commands for USB device AUDIO control feature unit control selector */
#define USBD_UAC_CTRL_FU_MUTE_CONTROL_SELECTOR                     0x01U /**< UAC Feature Unit Mute Control */
#define USBD_UAC_CTRL_FU_VOLUME_CONTROL_SELECTOR                   0x02U /**< UAC Feature Unit Volume Control */
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

/* AC IF header interface num */
#define USBD_UAC_AC_IF_NUM                          2U
/* AC feature uint descriptor length */
#define USBD_UAC_AC_FU_HEAD_DESC_LEN(ch_cnt)        (USBD_UAC_LEN_FUNC_UNIT_DESC + 1 + 4 * (ch_cnt))
/* AC IF header descriptor length */
#define USBD_UAC_AC_IF_HEAD_DESC_LEN(ch_cnt)         (USBD_UAC_LEN_CTRL_IF_HEADER + USBD_UAC_LEN_CLK_SRC_DESC + USBD_UAC_LEN_AC_IN_TTY_DESC + \
										    			USBD_UAC_AC_FU_HEAD_DESC_LEN(ch_cnt) + USBD_UAC_LEN_AC_OUT_TTY_DESC)
/* len of total Audio control interface */
#define USBD_UAC_AC_IF_LEN(ch_cnt)                 (USB_LEN_IF_DESC + USBD_UAC_AC_IF_HEAD_DESC_LEN(ch_cnt))
/* len of each Audio stream interface/altsetting (one EP) */
#define USBD_UAC_AS_EIF_LEN                        (USB_LEN_IF_DESC + USBD_UAC_LEN_AS_IF_ALT_SET_DESC + USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC \
                                                   + USB_LEN_EP_DESC + USBD_UAC_LEN_DATA_EP_DESC)

/* len of total Audio stream interface */
#define USBD_UAC_AS_TIF_LEN(alt_num)               (USBD_UAC_AS_EIF_LEN * (alt_num - 1) + USB_LEN_IF_DESC)

/* full speed AS alt setting num */
#define USBD_UAC_FS_AS_ALT_SETTING_NUM              5U
/* full speed AS alt setting num */
#define USBD_UAC_HS_AS_ALT_SETTING_NUM              5U

/**
 * High Speed
 */
/* calculate high speed MPS */
#define USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, sampling_freq_hz) \
  ((ch_cnt) * (byte_width) * ((sampling_freq_hz) / USBD_UAC_ONE_KHZ / USBD_UAC_HS_SOF_COUNT_PER_MS + 1U))
/* check MPS */
#define USBD_UAC_IS_HS_MPS_VALID(ch_cnt, byte_width, sampling_freq_hz) \
    ((USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, sampling_freq_hz)) <= USBD_UAC_HS_ISOC_MPS)
/* get high speed MPS, if MPS > limit, choose next lower sampling_freq to calculate */
#define USBD_UAC_GET_HS_MPS(ch_cnt, byte_width) \
    (USBD_UAC_IS_HS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_192K) ? \
    USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_192K) : \
    (USBD_UAC_IS_HS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_96K) ? \
    USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_96K) : \
    (USBD_UAC_IS_HS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_48K) ? \
    USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_48K) : \
    (USBD_UAC_IS_HS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_44K) ? \
    USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_44K) : 0))))

/* len of high speed total configuration descriptor buf */
#define USBD_UAC_HS_CFG_DESC_BUF_LEN(ch_cnt) \
    (USB_LEN_CFG_DESC + USB_LEN_IAD_DESC  + USBD_UAC_AC_IF_LEN(ch_cnt) + USBD_UAC_AS_TIF_LEN(USBD_UAC_HS_AS_ALT_SETTING_NUM))

/*
*	Full Speed
*/
/* calculate full speed MPS */
#define USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, sampling_freq_hz) \
    ((ch_cnt) * (byte_width) * ((sampling_freq_hz) / USBD_UAC_ONE_KHZ + 1U))
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

/* len of full speed total configuration descriptor buf */
#define USBD_UAC_FS_CFG_DESC_BUF_LEN(ch_cnt) \
    (USB_LEN_CFG_DESC + USB_LEN_IAD_DESC  + USBD_UAC_AC_IF_LEN(ch_cnt) + USBD_UAC_AS_TIF_LEN(USBD_UAC_FS_AS_ALT_SETTING_NUM))

/* UAC channel config */
#define USBD_UAC_GET_CH_CONFIG(ch_cnt) \
    ((ch_cnt) == 2 ? 0x03 : \
     (ch_cnt) == 4 ? 0x0F : \
     (ch_cnt) == 6 ? 0x3F : \
     (ch_cnt) == 8 ? 0xFF : \
     (ch_cnt) == 16 ? 0xFFFF : 0x03)

/* UAC terminal type */
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

#define USBD_UAC_INIT_SUB_RANGE(sub_range, min_sampling_freq, max_sampling_freq, res) \
    do {                                               \
        (sub_range).dMIN = (min_sampling_freq);        \
        (sub_range).dMAX = (max_sampling_freq);        \
        (sub_range).dRES = (res);                       \
    } while(0)

/* Private function prototypes -----------------------------------------------*/
static int usbd_uac_set_config(usb_dev_t *dev, u8 config);
static int usbd_uac_clear_config(usb_dev_t *dev, u8 config);
static int usbd_uac_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
static int usbd_uac_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);
static int usbd_uac_handle_ep0_data_out(usb_dev_t *dev);
static int usbd_uac_handle_sof(usb_dev_t *dev);
static void usbd_uac_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
#if UABD_UAC_DESC_DUMP
static int usbd_uac_desc_dump(u8 *pbuf, int len);
#endif
#if USBD_UAC_DEBUG
static void usbd_uac_status_dump_thread(void *param);
#endif
static inline void usbd_uac_get_audio_data_cnt(u32 audio_len);
static inline u16 usbd_uac_get_ring_buf_cnt(u8 speed);
static u16 usbd_uac_get_mps(usbd_audio_cfg_t *params, u8 speed);
static u8 usbd_uac_get_ch_config(u8 ch_cnt);
static inline u8 usbd_uac_ep_enable(usbd_audio_cfg_t *ep);
static void usbd_uac_clk_valid_req(usb_dev_t *dev, u8 flag);
static void usbd_uac_cur_sampling_freq_req(usb_dev_t *dev, u32 frequency);
static int usbd_uac_sampling_freq_ctrl_range_req(usb_dev_t *dev, u16 max_len);
static void usbd_uac_connect_ctrl_req(usb_dev_t *dev, u8 ch_num, u32 ch_cfg, u16 max_len);
static bool usbd_uac_is_valid_sample_rate(u32 freq, u8 speed);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "UAC";

static u32 usbd_uac_sampling_rates[USBD_UAC_SAMPLING_FREQ_MAX_COUNT] = {USBD_UAC_SAMPLING_FREQ_44K, USBD_UAC_SAMPLING_FREQ_48K, USBD_UAC_SAMPLING_FREQ_96K, USBD_UAC_SAMPLING_FREQ_192K};
/* USB Standard Device Descriptor */
static const u8 usbd_uac_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,            /* bLength */
	USB_DESC_TYPE_DEVICE,        /* bDescriptorType */
	0x00,                        /* bcdUSB */
	0x02,
	0x00,                        /* bDeviceClass */
	0x00,                        /* bDeviceSubClass */
	0x00,                        /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,            /* bMaxPacketSize */
	USB_LOW_BYTE(USBD_UAC_VID),  /* idUAC */
	USB_HIGH_BYTE(USBD_UAC_VID),
	USB_LOW_BYTE(USBD_UAC_PID),  /* idProduct */
	USB_HIGH_BYTE(USBD_UAC_PID),
	0x00,                        /* bcdDevice */
	0x02,
	USBD_IDX_MFC_STR,            /* iManufacturer */
	USBD_IDX_PRODUCT_STR,        /* iProduct */
	USBD_IDX_SERIAL_STR,         /* iSerialNumber */
	0x01                         /* bNumConfigurations */
}; /* usbd_uac_dev_desc */

/* USB Standard String Descriptor 0 */
static const u8 usbd_uac_lang_id_desc[USB_LEN_LANGID_STR_DESC] = {
	USB_LEN_LANGID_STR_DESC,              /* bLength */
	USB_DESC_TYPE_STRING,                 /* bDescriptorType */
	USB_LOW_BYTE(USBD_UAC_LANGID_STRING), /* wLANGID */
	USB_HIGH_BYTE(USBD_UAC_LANGID_STRING),
}; /* usbd_uac_lang_id_desc */

/* USB Standard Device Qualifier Descriptor */
static const u8 usbd_uac_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] = {
	USB_LEN_DEV_QUALIFIER_DESC,        /* bLength */
	USB_DESC_TYPE_DEVICE_QUALIFIER,    /* bDescriptorType */
	0x00,                              /* bcdUSB */
	0x02,
	0x00,                              /* bDeviceClass */
	0x00,                              /* bDeviceSubClass */
	0x00,                              /* bDeviceProtocol */
	0x40,                              /* bMaxPacketSize */
	0x01,                              /* bNumConfigurations */
	0x00,                              /* Reserved */
}; /* usbd_uac_device_qualifier_desc */

static const u8 usbd_uac_hs_config_desc[USBD_UAC_HS_CFG_DESC_BUF_LEN(USBD_UAC_DEFAULT_CH_CNT)] = {
	/* USB UAC Device Configuration Descriptor */
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,                  /* bLength */
	USB_DESC_TYPE_CONFIGURATION,       /* bDescriptorType */
	USB_LOW_BYTE(USBD_UAC_HS_CFG_DESC_BUF_LEN(USBD_UAC_DEFAULT_CH_CNT)), /* wTotalLength */
	USB_HIGH_BYTE(USBD_UAC_HS_CFG_DESC_BUF_LEN(USBD_UAC_DEFAULT_CH_CNT)),
	USBD_UAC_AC_IF_NUM,                              /* Number of interfaces supported by this configuration */
	0x01,                              /* Value to use as an argument to the SetConfiguration() request to select this configuration */
	0x00,                              /* Index of string descriptor describing this configuration */
#if USBD_UAC_SELF_POWERED
	0xC0,                              /* bmAttributes: self powered */
#else
	0x80,                              /* bmAttributes: bus powered */
#endif
	0x32,                              /* bMaxPower */

	/* 4.6 Interface Association Descriptor */
	/* IAD Descriptor */
	USB_LEN_IAD_DESC,                  /* Size of this descriptor (byte_width) */
	USBD_UAC_IA_DESCRIPTOR,            /* Interface Association Descriptor type */
	0x00,                              /* First Index: Audio Control Interface Index - Headset (0) */
	USBD_UAC_AC_IF_NUM,                /* Audio Control Interface, Audio Streaming Interfaces () */
	USBD_UAC_IF_CLASS_AUDIO,           /* Audio Device Class  */
	0x00,                              /* No subclass */
	USBD_UAC_VERSION_02_00,            /* Audio Protocol IP version 2.00 */
	0x00,                              /* Function string descriptor index (0) */

	/* 4.7.1 Standard AC Interface Descriptor */
	/* Interface 0 Descriptor */
	USB_LEN_IF_DESC,                   /* bLength */
	USB_DESC_TYPE_INTERFACE,           /* Interface Descriptor type */
	USBD_UAC_IF_IDX_AC_HEADSET,        /* Audio Control Interface Index - Headset (0) */
	0x00,                              /* bAlternateSetting */
	0x00,                              /* No associated endpoints with this interface (uses endpoint 0) */
	USBD_UAC_CLASS_CODE,               /* Audio Device Class */
	USBD_UAC_SUBCLASS_AUDIOCONTROL,    /* Audio Control Interface */
	USBD_UAC_VERSION_02_00,            /* Audio Protocol IP version 2.00 */
	0x00,                              /* Interface string descriptor index (0) */

	/* 4.7.2 Class-Specific AC Interface Descriptor */
	/* Audio headset */
	USBD_UAC_LEN_CTRL_IF_HEADER,                                         /* Size of the descriptor, in byte_width  */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                               /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AC_IF_DESC_SUBTYPE_HEADER,                                  /* HEADER descriptor subtype  */
	USB_LOW_BYTE(0x0200),                                                /* bcdUSB */
	USB_HIGH_BYTE(0x0200),
	USBD_UAC_FUNC_CATEGORY_CODE_DESKTOP_SPEAKER,                         /* DESKTOP_SPEAKER(0x01) : Indicating the primary use of this audio function   */
	USB_LOW_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT)),
	USB_HIGH_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT)),/* Total number of byte_width returned for the class-specific AudioControl interface descriptor. Includes
                                                                         the combined length of this descriptor header and all Unit and Terminal descriptors.   */
	0x00,                                                                /* D1..0: Latency Control  */

	/* Audio Control Clock Source Unit Descriptor 2.0 */
	USBD_UAC_LEN_CLK_SRC_DESC,                               /* Size of the descriptor, in byte_width  */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE,                /* CLOCK_SOURCE descriptor subtype  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES,         /* Constant uniquely identifying the Clock Source Entity within
                                                             the audio funcion */
	0x01,                                                    /* D1..0: 01: Internal Fixed Clock
                                                             D2: 0 Clock is not synchronized to SOF
                                                             D7..3: Reserved, should set to 0   */
	0x07,                                                    /* D1..0: Clock Frequency Control is present and Host programmable
                                                             D3..2: Clock Validity Control is present but read-only
                                                             D7..4: Reserved, should set to 0 */
	0x00,                                                    /* This Clock Source has no association   */
	0x00,                                                    /* Index of a string descriptor, describing the Clock Source Entity  */

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
	USBD_UAC_DEFAULT_CH_CNT,                                 /* This Terminal's output audio channel cluster has 8 logical output channels   */
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
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT),         /* Size of the descriptor, in byte_width  : 6 + (4 + 1) * 4 */
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
	0x01,                                                      /* The number of this interface is 1.   */
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
	0x01,                                                      /* The number of this interface is 1.   */
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
	0xF,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CH_CNT_2,                                       /* Number of physical channels in the AS Interface audio channel cluster */
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
	USBD_UAC_BYTE_WIDTH_2,                                   /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_BYTE_WIDTH_2),               /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 2   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_DEFAULT_BYTE_WIDTH)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */


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
	0x01,                                                     /* The number of this interface is 1.   */
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
	0xf,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CH_CNT_4,                                       /* Number of physical channels in the AS Interface audio channel cluster */
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
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 2   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_DEFAULT_BYTE_WIDTH)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */


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
	0x01,                                                     /* The number of this interface is 1.   */
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
	USBD_UAC_CH_CNT_6,                                       /* Number of physical channels in the AS Interface audio channel cluster */
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
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 2   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_DEFAULT_BYTE_WIDTH)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */


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
	/* 4.9.1 Standard AS Interface Descriptor 1/3*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                     /* The number of this interface is 1.   */
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
	USBD_UAC_CH_CNT_8,                                       /* Number of physical channels in the AS Interface audio channel cluster */
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
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 2   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_DEFAULT_BYTE_WIDTH)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */


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
};

/* USB UAC Device Configuration Descriptor */
/* USB Standard Configuration Descriptor */
static const u8 usbd_uac_fs_config_desc[USBD_UAC_FS_CFG_DESC_BUF_LEN(USBD_UAC_DEFAULT_CH_CNT)] = {
	USB_LEN_CFG_DESC,                                        /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                             /* bDescriptorType */
	USB_LOW_BYTE(USBD_UAC_FS_CFG_DESC_BUF_LEN(USBD_UAC_DEFAULT_CH_CNT)), /* wTotalLength */
	USB_HIGH_BYTE(USBD_UAC_FS_CFG_DESC_BUF_LEN(USBD_UAC_DEFAULT_CH_CNT)),
	USBD_UAC_AC_IF_NUM,                                      /* Number of interfaces supported by this configuration */
	0x01,                                                    /* Value to use as an argument to the SetConfiguration() request to select this configuration */
	0x00,                                                    /* Index of string descriptor describing this configuration */
#if USBD_UAC_SELF_POWERED
	0xC0,                                                    /* bmAttributes: self powered */
#else
	0x80,                                                    /* bmAttributes: bus powered */
#endif
	0x32,                                                    /* bMaxPower */

	/* 4.6 Interface Association Descriptor */
	/* IAD Descriptor */
	USB_LEN_IAD_DESC,                  /* Size of this descriptor (byte_width) */
	USBD_UAC_IA_DESCRIPTOR,            /* Interface Association Descriptor type */
	0x00,                              /* First Index: Audio Control Interface Index - Headset (0) */
	USBD_UAC_AC_IF_NUM,                /* Audio Control Interface, Audio Streaming Interfaces () */
	USBD_UAC_IF_CLASS_AUDIO,           /* Audio Device Class  */
	0x00,                              /* No subclass */
	USBD_UAC_VERSION_02_00,            /* Audio Protocol IP version 2.00 */
	0x00,                              /* Function string descriptor index (0) */

	/* 4.7.1 Standard AC Interface Descriptor */
	/* Interface 0 Descriptor */
	USB_LEN_IF_DESC,                   /* bLength */
	USB_DESC_TYPE_INTERFACE,           /* Interface Descriptor type */
	USBD_UAC_IF_IDX_AC_HEADSET,        /* Audio Control Interface Index - Headset (0) */
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
	0x01,                                                      /* The number of this interface is 1.   */
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
	0x01,                                                      /* The number of this interface is 1.   */
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
	USBD_UAC_BYTE_WIDTH_2,                                   /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_BYTE_WIDTH_2),               /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 2   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_DEFAULT_BYTE_WIDTH)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */

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
	0x01,                                                     /* The number of this interface is 1.   */
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
	USBD_UAC_CH_CNT_4,                                       /* Number of physical channels in the AS Interface audio channel cluster */
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
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 2   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_DEFAULT_BYTE_WIDTH)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */


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
	0x01,                                                     /* The number of this interface is 1.   */
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
	0x1,                                                     /* The Terminal ID of the terminal to which this interface is connected */
	0xF,                                                     /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                     /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CH_CNT_6,                                       /* Number of physical channels in the AS Interface audio channel cluster */
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
	USBD_UAC_BYTE_WIDTH_2,                                   /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_BYTE_WIDTH_2),               /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 2   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_BYTE_WIDTH_2)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_BYTE_WIDTH_2)),
	0x01,                                                    /* bInterval */


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
	0x01,                                                     /* The number of this interface is 1.   */
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
	USBD_UAC_CH_CNT_8,                                       /* Number of physical channels in the AS Interface audio channel cluster */
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
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 2   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_DEFAULT_BYTE_WIDTH)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */


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
};
/* UAC Class Driver */
static const usbd_class_driver_t usbd_uac_driver = {
	.get_descriptor = usbd_uac_get_descriptor,
	.set_config = usbd_uac_set_config,
	.clear_config = usbd_uac_clear_config,
	.setup = usbd_uac_setup,
	.ep0_data_out = usbd_uac_handle_ep0_data_out,
	.ep_data_in = usbd_uac_handle_ep_data_in,
	.ep_data_out = usbd_uac_handle_ep_data_out,
	.status_changed = usbd_uac_status_changed,
	.sof = usbd_uac_handle_sof,
};

/* UAC Device */
static usbd_uac_dev_t usbd_uac_dev;

/* UAC volume data from windows10 PC*/
/* Different OS transmit the same vol level, but the corresponding driver vol values are different. */
static const u8 usbd_uac_pc_vol_lvl[] = {0, 5, 10, 20, 30, 40, 50, 60, 65, 75, 80, 85, 90, 95, 100};
static const s16 usbd_uac_drv_vol[] = {-190, -170, -151, -112, -74, -37, 0, 37, 56, 93, 112, 132, 151, 170, 190};

/* Private functions ---------------------------------------------------------*/

#if UABD_UAC_DESC_DUMP
/**
  * @brief  Dump UAC descriptor information.
  * @param  pbuf: Pointer to the descriptor buffer.
  * @param  len: Length of the descriptor buffer.
  * @retval Status
  */
static int usbd_uac_desc_dump(u8 *pbuf, int len)
{
	int i = 0;
	int j;
	char len_val = 0;
	for (i = 0; i < len;) {
		len_val = pbuf[i];
		for (j = 0; j < len_val; j++) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "%02x ", pbuf[i + j]);
		}
		i += len_val;
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
		if (len_val < 6) {
			break;
		}
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
	return 0;
}
#endif

/**
  * @brief  Get the ring buffer count based on USB speed
  * @param  speed: USB connection speed
  * @retval Number of buffer segments for audio data
  */
static u16 usbd_uac_get_ring_buf_cnt(u8 speed)
{
	if (speed == USB_SPEED_HIGH) { //high speed
		return 8 * USBD_UAC_RX_BUF_MAX_CNT;
	}

	return USBD_UAC_RX_BUF_MAX_CNT;
}

/**
  * @brief  Get the MPS based on configuration parameters and speed
  * @param  params: Pointer to audio configuration parameters structure
  * @param  speed: USB connection speed
  * @retval Calculated MPS
  */
static u16 usbd_uac_get_mps(usbd_audio_cfg_t *params, u8 speed)
{
	u16 mps_value;
	if (NULL == params) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Param err\n");
		return 0;
	}

	if (speed == USB_SPEED_HIGH) {
		mps_value = params->ch_cnt * params->byte_width * (params->sampling_freq / USBD_UAC_ONE_KHZ / USBD_UAC_HS_SOF_COUNT_PER_MS + 1);
		if (mps_value > USBD_UAC_HS_ISOC_MPS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "MPS %d exceed HS limited %d\n", mps_value, USBD_UAC_HS_ISOC_MPS);
			return 0;
		}
	} else {
		/* for 44.1khz or the host clk is bigger than the device */
		mps_value = params->ch_cnt * params->byte_width * (params->sampling_freq / USBD_UAC_ONE_KHZ + 1);
		if (mps_value > USBD_UAC_FS_ISOC_MPS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "MPS %d exceed FS limited %d\n", mps_value, USBD_UAC_FS_ISOC_MPS);
			return 0;
		}
	}

	return mps_value;
}

/**
  * @brief  Get the channel configuration based on UAC channel cnt
  * @param  ch_cnt: UAC channel cnt
  * @retval Bitmask representing the channel configuration
  */
static u8 usbd_uac_get_ch_config(u8 ch_cnt)
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
static void usbd_uac_ep_buf_ctrl_deinit(usbd_uac_buf_ctrl_t *buf_ctrl)
{
	buf_ctrl->isoc_read_idx = 0;
	buf_ctrl->isoc_write_idx = 0;
	buf_ctrl->isoc_mps = 0;
	buf_ctrl->buf_array_cnt = 0;
	buf_ctrl->next_xfer = 0;
	// RTK_LOGS(TAG, RTK_LOG_INFO, "Buf 0x%08x-0x%08x sema %d\n",buf_ctrl->isoc_buf,buf_ctrl->buf_array,buf_ctrl->uac_sema_valid);

	if (buf_ctrl->isoc_buf != NULL) {
		usb_os_mfree(buf_ctrl->isoc_buf);
		buf_ctrl->isoc_buf = NULL;
	}

	if (buf_ctrl->buf_array != NULL) {
		usb_os_mfree(buf_ctrl->buf_array);
		buf_ctrl->buf_array = NULL;
	}

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
  * @brief  Initialize UAC endpoint buffer control structure
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @param  params: Pointer to audio configuration parameters structure
  * @param  speed: USB connection speed
  * @retval Status
  */
static int usbd_uac_ep_buf_ctrl_init(usbd_uac_buf_ctrl_t *buf_ctrl, usbd_audio_cfg_t *params, u8 speed)
{
	u8 idx;
	usbd_uac_buf_t *pdata = NULL;

	if (usbd_uac_ep_enable(params)) {
		buf_ctrl->isoc_mps = usbd_uac_get_mps(params, speed);
		if (buf_ctrl->isoc_mps == 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "MPS check fail\n");
			return HAL_ERR_PARA;
		}
		buf_ctrl->buf_array_cnt = usbd_uac_get_ring_buf_cnt(speed);

		// RTK_LOGS(TAG, RTK_LOG_INFO, "Buf mps len %d-%d, cnt %d\n",buf_ctrl->isoc_mps,CACHE_LINE_ALIGNMENT(buf_ctrl->isoc_mps),buf_ctrl->buf_array_cnt);
		buf_ctrl->isoc_buf = (u8 *)usb_os_malloc(CACHE_LINE_ALIGNMENT(buf_ctrl->isoc_mps) * buf_ctrl->buf_array_cnt);
		if (buf_ctrl->isoc_buf == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Can not get isoc buf mem\n");
			return HAL_ERR_MEM;
		}

		buf_ctrl->buf_array = (usbd_uac_buf_t *)usb_os_malloc(sizeof(usbd_uac_buf_t) * buf_ctrl->buf_array_cnt);
		if (buf_ctrl->buf_array == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Can not get isoc buf array mem\n");
			return HAL_ERR_MEM;
		}

		for (idx = 0; idx < buf_ctrl->buf_array_cnt; idx ++) {
			pdata = &(buf_ctrl->buf_array[idx]);
			pdata->buf_valid_len = 0;
			pdata->buf_raw = buf_ctrl->isoc_buf + CACHE_LINE_ALIGNMENT(buf_ctrl->isoc_mps) * idx ;

			// RTK_LOGS(TAG, RTK_LOG_INFO, "Buf %d-%d-%d-0x%08x\n",idx,buf_ctrl->isoc_mps,pdata->buf_valid_len,pdata->buf_raw);
		}
		rtos_sema_create(&(buf_ctrl->uac_isoc_sema), 0U, 1U);
		buf_ctrl->uac_sema_valid = 1;
	} else {
		usbd_uac_ep_buf_ctrl_deinit(buf_ctrl);
	}

	return HAL_OK;
}

/**
  * @brief  Get USB audio endpoint status
  * @param  ep: PUSB audio endpoint config
  * @retval Status
  */
static inline u8 usbd_uac_ep_enable(usbd_audio_cfg_t *ep)
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
static void usbd_uac_clk_valid_req(usb_dev_t *dev, u8 flag)
{
	u8 valid = flag;
	usbd_ep_t *ep0_in = &dev->ep0_in;

	usb_os_memcpy(ep0_in->xfer_buf, &valid, sizeof(valid));
	ep0_in->xfer_len = sizeof(valid);
	usbd_ep_transmit(dev, ep0_in);
}

/**
  * @brief  Handle UAC current freq request
  * @param  dev: USB device instance
  * @param  frequency: UAC current freq
  * @retval void
  */
static void usbd_uac_cur_sampling_freq_req(usb_dev_t *dev, u32 frequency)
{
	u32 dCur = frequency;
	usbd_ep_t *ep0_in = &dev->ep0_in;

	usb_os_memcpy(ep0_in->xfer_buf, &dCur, sizeof(dCur));
	ep0_in->xfer_len = sizeof(dCur);
	usbd_ep_transmit(dev, ep0_in);
}

/**
  * @brief  Handle freq ctrl requests
  * @param  dev: USB device instance
  * @param  max_len: Maximum allowed length for response
  * @retval Status
  */
static int usbd_uac_sampling_freq_ctrl_range_req(usb_dev_t *dev, u16 max_len)
{
	usbd_uac_sampling_freq_ctrl_range_t *response = NULL;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	u16 num_sub_ranges;
	u16 len;
	if (dev->dev_speed  == USB_SPEED_HIGH) {
		num_sub_ranges = USBD_UAC_HS_SAMPLING_FREQ_COUNT;
	} else {
		num_sub_ranges = USBD_UAC_FS_SAMPLING_FREQ_COUNT;
	}

	len = sizeof(num_sub_ranges) + num_sub_ranges * sizeof(usbd_uac_sub_range_t);
	response = (usbd_uac_sampling_freq_ctrl_range_t *)usb_os_malloc(len);

	if (!response) {
		return HAL_ERR_MEM;
	}

	response->wNumSubRanges = num_sub_ranges;
	for (u8 i = 0; i < num_sub_ranges; i++) {
		USBD_UAC_INIT_SUB_RANGE(response->usbd_uac_sub_ranges[i], usbd_uac_sampling_rates[i], usbd_uac_sampling_rates[i], 0);
	}

	if (len > max_len) {
		len = max_len;
	}

	usb_os_memcpy(ep0_in->xfer_buf, response, len);
	ep0_in->xfer_len = len;
	usbd_ep_transmit(dev, ep0_in);

	if (response != NULL) {
		usb_os_mfree(response);
		response = NULL;
	}
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
static void usbd_uac_connect_ctrl_req(usb_dev_t *dev, u8 ch_num, u32 ch_cfg, u16 max_len)
{
	usbd_uac_ac_connect_ctrl_t response = {0, 0, 0};
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
static int usbd_uac_set_config(usb_dev_t *dev, u8 config)
{
	int ret = HAL_OK;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	UNUSED(config);

	cdev->dev = dev;
	cdev->alt_setting = 0U;

	if (cdev->cb->set_config != NULL) {
		cdev->cb->set_config();
	}

	return ret;
}

/**
  * @brief  Clear UAC configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_uac_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = 0U;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;

	UNUSED(config);

	/* DeInit ISOC IN EP */
	if (usbd_uac_ep_enable(&(cdev->cb->in))) {
		usbd_ep_deinit(dev, ep_isoc_in);
	}

	/* DeInit ISOC OUT EP */
	if (usbd_uac_ep_enable(&(cdev->cb->out))) {
		usbd_ep_deinit(dev, ep_isoc_out);
	}

	return ret;
}

/**
  * @brief  Check freq requests
  * @param  freq: Freq requests from host
  * @param  speed: USB connection speed
  * @retval Status
  */
static bool usbd_uac_is_valid_sample_rate(u32 freq, u8 speed)
{
	u8 count;

	if (speed == USB_SPEED_HIGH) {
		count = USBD_UAC_HS_SAMPLING_FREQ_COUNT;
	} else {
		count = USBD_UAC_FS_SAMPLING_FREQ_COUNT;
	}

	for (u8 i = 0; i < count; ++i) {
		if (freq == usbd_uac_sampling_rates[i]) {
			return true;
		}
	}
	return false;
}

/**
  * @brief  Calculate the PC's intended volume level based on the UAC driver value
  * @param  x_points: Array of volume levels on the Windows PC
  * @param  y_points: Corresponding array of volume levels used by the UAC driver
  * @param  num_points: Number of data points (at least 2)
  * @param  target_y: The volume level from the UAC driver to be interpolated
  * @retval The intended volume level on the Windows PC corresponding to the UAC driver value
  */
static u8 usbd_uac_volume_linear_interpolation(const u8 x_points[], const s16 y_points[], u8 num_points, s16 target_y)
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
static int usbd_uac_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_cb_t *cb = cdev->cb;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	int ret = HAL_OK;
	u8 entityId, controlSelector;
	u8 ch_cnt;
	u8 byte_width;
	u8 alt_setting;
	u8 fmt_change;

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				alt_setting = USB_LOW_BYTE(req->wValue);
				if ((alt_setting != cdev->alt_setting) && alt_setting) {
					cdev->alt_setting = alt_setting;
					RTK_LOGS(TAG, RTK_LOG_INFO, "set new altsetting:%d\n", cdev->alt_setting);
					switch (cdev->alt_setting) {
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
						if ((cdev->cur_byte_width != byte_width) && byte_width) {
							cdev->cur_byte_width = byte_width;
							fmt_change = 1;
						}
						if ((cdev->cur_ch_cnt != ch_cnt) && ch_cnt) {
							cdev->cur_ch_cnt = ch_cnt;
							fmt_change = 1;
							//RTK_LOGS(TAG, RTK_LOG_INFO, "set ch_cnt %d\n",ch_cnt);
						}
						if (fmt_change) {
							if (cb->format_changed != NULL) {
								cb->format_changed(cdev->cur_sampling_freq, cdev->cur_ch_cnt, cdev->cur_byte_width);
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
				ep0_in->xfer_buf[0] = cdev->alt_setting;
				ep0_in->xfer_len = 1U;
				usbd_ep_transmit(dev, ep0_in);
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

		//RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP: id=0x%02x cs=%d\n", entityId, controlSelector);

		if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
			switch (entityId) {
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE:
				if (controlSelector == USBD_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usbd_uac_cur_sampling_freq_req(dev, cdev->cur_sampling_freq);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
						usbd_uac_sampling_freq_ctrl_range_req(dev, req->wLength);
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES:
				if (controlSelector == USBD_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usbd_uac_cur_sampling_freq_req(dev, cdev->cur_sampling_freq);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
						usbd_uac_sampling_freq_ctrl_range_req(dev, req->wLength);
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USBD_UAC_CS_CLK_VALID_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usbd_uac_clk_valid_req(dev, cdev->cur_clk_valid);
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE: //in
				if (controlSelector == USBD_UAC_TE_CONNECTOR_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usbd_uac_connect_ctrl_req(dev, 1, 0x1, req->wLength);
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES: //out
				if (controlSelector == USBD_UAC_TE_CONNECTOR_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						ch_cnt = cdev->cb->out.ch_cnt;
						usbd_uac_connect_ctrl_req(dev, ch_cnt, usbd_uac_get_ch_config(ch_cnt), req->wLength);
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT:
				if (controlSelector == USBD_UAC_CTRL_FU_MUTE_CONTROL_SELECTOR) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						ep0_in->xfer_buf[0] = cdev->cur_mute;
						ep0_in->xfer_len = 1U;
						usbd_ep_transmit(dev, ep0_in);
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USBD_UAC_CTRL_FU_VOLUME_CONTROL_SELECTOR) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usb_os_memcpy((void *)ep0_in->xfer_buf, (void *) & (cdev->cur_volume), 2);
						ep0_in->xfer_len = 2U;
						usbd_ep_transmit(dev, ep0_in);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
						usbd_uac_ctrl_range_layout2_struct response;
						response.wNumSubRanges = 1;
						response.wMIN = USBD_UAC_VOLUME_CTRL_MIN;
						response.wMAX = USBD_UAC_VOLUME_CTRL_MAX;
						response.wRES = 1;

						usb_os_memcpy((void *)ep0_in->xfer_buf, (void *)&response, sizeof(response));
						ep0_in->xfer_len = sizeof(response);
						usbd_ep_transmit(dev, ep0_in);
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: 0x13 wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;

			default:
				RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: wIndex err %d\n", entityId);
				break;
			}
		} else {
			/* USB_H2D */
			switch (entityId) {
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES:
				if (controlSelector == USBD_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usb_os_memcpy((void *)&cdev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						ep0_out->xfer_len = req->wLength;
						usbd_ep_receive(dev, ep0_out);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
						// Do nothing
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "Set freq err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "Set freq ctrl err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;/* case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES */

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT: //0x05 FU
				if (controlSelector == USBD_UAC_CTRL_FU_MUTE_CONTROL_SELECTOR) { //mute
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usb_os_memcpy((void *)&cdev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						ep0_out->xfer_len = req->wLength;
						usbd_ep_receive(dev, ep0_out);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
						// Do nothing
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "Set cur mute err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USBD_UAC_CTRL_FU_VOLUME_CONTROL_SELECTOR) { //volume
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usb_os_memcpy((void *)&cdev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						ep0_out->xfer_len = req->wLength;
						usbd_ep_receive(dev, ep0_out);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
						// Do nothing
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "Set cur volume range err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "Set fu err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_HW;
				}
				break;/* case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT */

			default:
				RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: wIndex err %d\n", entityId);
				break;
			}
		}
		break;/* case USB_REQ_TYPE_CLASS */

	case USB_REQ_TYPE_VENDOR:
		if (req->wLength) {
			if ((req->bmRequestType & 0x80U) && (cdev->cb->setup != NULL)) {
				ret = cdev->cb->setup(req, ep0_in->xfer_buf);
				if (ret == HAL_OK) {
					ep0_in->xfer_len = req->wLength;
					usbd_ep_transmit(dev, ep0_in);
				}
			} else {
				usb_os_memcpy((void *)&cdev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
				ep0_out->xfer_len = req->wLength;
				usbd_ep_receive(dev, ep0_out);
			}
		} else {
			cdev->cb->setup(req, NULL);
		}
		break;

	default:
		RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: bmRequestType err\n");
		ret = HAL_ERR_HW;
		break;
	}

	return ret;
}

/**
  * @brief  Handles the SOF event for the UAC device
  * @param  dev: USB device instance
  * @retval Status
  */
static int usbd_uac_handle_sof(usb_dev_t *dev)
{
	UNUSED(dev);
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_cb_t *cb = cdev->cb;

	if (cb->sof != NULL) {
		cb->sof();
	}

	return HAL_OK;
}

/**
  * @brief  uac_handle_ep0_data_out
  *         Handle EP0 Rx Ready event
  * @param  dev: USB device instance
  * @retval Status
  */
static int usbd_uac_handle_ep0_data_out(usb_dev_t *dev)
{
	int ret = HAL_OK;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_cb_t *cb = cdev->cb;
	usb_setup_req_t *p_ctrl_req = &(cdev->ctrl_req);
	usbd_ep_t *ep0_out = &dev->ep0_out;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	u32 freq;
	s16 volume_value = 0;
	u8 ch_cnt;
	u8 byte_width;
	u8 num_points;
	u8 target_volume;

	// RTK_LOGS(TAG, RTK_LOG_INFO, "EP0 Out: bmRequestType=0x%02x bRequest=0x%02x wValue=%x wIndex=%x wLength=0x%04x\n",
	// 	p_ctrl_req->bmRequestType, p_ctrl_req->bRequest, p_ctrl_req->wValue, p_ctrl_req->wIndex, p_ctrl_req->wLength);
	if ((((p_ctrl_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS) && ((p_ctrl_req->bmRequestType & 0x1FU) == USB_REQ_RECIPIENT_INTERFACE))
		&& (p_ctrl_req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR)) {
		if (USB_HIGH_BYTE(p_ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT) {
			if ((USB_HIGH_BYTE(p_ctrl_req->wValue) == USBD_UAC_CTRL_FU_MUTE_CONTROL_SELECTOR) && (p_ctrl_req->wLength == 0x01)) {
				if (cb->mute_changed != NULL) {
					cdev->cur_mute = (ep0_out->xfer_buf[0]) ? 1 : 0;
					cb->mute_changed(cdev->cur_mute);
				}
			} else if ((USB_HIGH_BYTE(p_ctrl_req->wValue) == USBD_UAC_CTRL_FU_VOLUME_CONTROL_SELECTOR) && (p_ctrl_req->wLength == 0x02)) {
				volume_value = (u16)ep0_out->xfer_buf[0] + ((u16)(ep0_out->xfer_buf[1]) << 8);

				num_points = sizeof(usbd_uac_pc_vol_lvl) / sizeof(usbd_uac_pc_vol_lvl[0]);
				if (num_points != sizeof(usbd_uac_drv_vol) / sizeof(usbd_uac_drv_vol[0])) {
					RTK_LOGS(TAG, RTK_LOG_WARN, "Volume arrays length err.\n");
					ret = HAL_ERR_PARA;
				} else {
					target_volume = usbd_uac_volume_linear_interpolation(usbd_uac_pc_vol_lvl, usbd_uac_drv_vol, num_points, (int)volume_value);
				}

				if ((target_volume != UABD_UAC_VOL_ERR_VAL) && (cb->volume_changed != NULL)) {
					cdev->cur_volume = volume_value;
					cb->volume_changed(target_volume);
				}
			}
		}

		if ((USB_HIGH_BYTE(p_ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES)
			&& (USB_HIGH_BYTE(p_ctrl_req->wValue) == USBD_UAC_CS_SAM_FREQ_CONTROL) && (p_ctrl_req->wLength == 0x04)) {
			freq = (ep0_out->xfer_buf[3] << 24) | (ep0_out->xfer_buf[2] << 16) | (ep0_out->xfer_buf[1] << 8) | ep0_out->xfer_buf[0];
			if (usbd_uac_is_valid_sample_rate(freq, dev->dev_speed)) {
				if (cdev->cur_sampling_freq != freq) {
					cdev->cur_sampling_freq = freq;
					byte_width = cdev->cur_byte_width;
					ch_cnt = cdev->cur_ch_cnt;
					if (usbd_uac_ep_enable(&(cdev->cb->in))) {
						/* DeInit ISOC IN EP */
						usbd_ep_deinit(dev, ep_isoc_in);
						/* Init ISO IN EP */
						ep_isoc_in->mps = (dev->dev_speed == USB_SPEED_HIGH) ?  USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, freq) \
										  : USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, freq);
						usbd_ep_init(dev, ep_isoc_in);
					}

					if (usbd_uac_ep_enable(&(cdev->cb->out))) {
						/* DeInit ISOC OUT EP */
						usbd_ep_deinit(dev, ep_isoc_out);
						/* Init ISO OUT EP */
						ep_isoc_out->mps = (dev->dev_speed == USB_SPEED_HIGH) ?  USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, freq) \
										   : USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, freq);
						usbd_ep_init(dev, ep_isoc_out);
					}

					if (cb->format_changed != NULL) {
						cb->format_changed(cdev->cur_sampling_freq, cdev->cur_ch_cnt, cdev->cur_byte_width);
					}
				}
			} else {
				// Do nothing, return HAL_ERR_PARA(STALL) will cause PC continue setting invalid freq request
			}
		}

		// To do: handle vendor
	}

	return ret;
}

/**
  * @brief  Data sent on non-control IN endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_uac_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	UNUSED(dev);
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_in);

	if (pdata_ctrl->next_xfer) {
		if (ep_addr == USBD_UAC_ISOC_IN_EP) {
			if (status != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "ISOC TX err: %d\n", status);
			}
			//loop to next tx TODO
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
static int usbd_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_out);
	usbd_uac_buf_t *p_buf = NULL;
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	u8 wr_next;

	// RTK_LOGS(TAG, RTK_LOG_WARN, "Read data out %d\n",pdata_ctrl->next_xfer);

	if (pdata_ctrl->next_xfer) {
		if (ep_addr == USBD_UAC_ISOC_OUT_EP) {
			if (len == 0) { //ZLP
				//continue
				// RTK_LOGS(TAG, RTK_LOG_WARN, "Read ZLP\n");
				p_buf = &(pdata_ctrl->buf_array[pdata_ctrl->isoc_write_idx]);
				ep_isoc_out->xfer_buf = p_buf->buf_raw;
				ep_isoc_out->xfer_len = pdata_ctrl->isoc_mps;
				usbd_ep_receive(dev, ep_isoc_out);
			} else {
				wr_next = (pdata_ctrl->isoc_write_idx + 1) % (pdata_ctrl->buf_array_cnt);

				/*
					if no read, the function will overwrite the data, and the read_pos should be updated
				*/
				if (pdata_ctrl->isoc_read_idx == wr_next) {
					//RTK_LOGS(TAG, RTK_LOG_WARN, "Read too slow\n");
					pdata_ctrl->isoc_read_idx = (pdata_ctrl->isoc_read_idx + 1) % (pdata_ctrl->buf_array_cnt);
#if USBD_UAC_DEBUG
					cdev->isoc_overwrite_cnt++;
#endif
				}

				pdata_ctrl->buf_array[pdata_ctrl->isoc_write_idx].buf_valid_len = len;
				pdata_ctrl->isoc_write_idx = wr_next;

				p_buf = &(pdata_ctrl->buf_array[pdata_ctrl->isoc_write_idx]);
				ep_isoc_out->xfer_buf = p_buf->buf_raw;
				ep_isoc_out->xfer_len = pdata_ctrl->isoc_mps;
				usbd_ep_receive(dev, ep_isoc_out);

				if (pdata_ctrl->read_wait_sema) {
					rtos_sema_give(pdata_ctrl->uac_isoc_sema);
				}

#if USBD_UAC_DEBUG
				cdev->isoc_rx_cnt ++;
				static u32 g_rx_last_tick = 0;
				u32 g_rx_new_tick = DTimestamp_Get(); //us
				if ((g_rx_last_tick > 0) && (g_rx_new_tick - g_rx_last_tick > 175)) {  //125us -> 130 us
					cdev->isoc_timeout_cnt ++;
				}
#endif
			}
		}
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
static u16 usbd_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf)
{
	usb_speed_type_t speed = dev->dev_speed;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	u8 *desc = NULL;
	u16 len = 0;

	dev->self_powered = USBD_UAC_SELF_POWERED;

	switch (USB_HIGH_BYTE(req->wValue)) {

	case USB_DESC_TYPE_DEVICE:
		len = sizeof(usbd_uac_dev_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_uac_dev_desc, len);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		if (usbd_uac_ep_enable(&(cdev->cb->out))) {
			cdev->uac_isoc_out.isoc_mps = usbd_uac_get_mps(&(cdev->cb->out), speed);
		}
		if (usbd_uac_ep_enable(&(cdev->cb->in))) {
			cdev->uac_isoc_in.isoc_mps  = usbd_uac_get_mps(&(cdev->cb->in), speed);
		}

		if (speed == USB_SPEED_HIGH) {
			len = sizeof(usbd_uac_hs_config_desc);
			desc = (u8 *)&usbd_uac_hs_config_desc;
		} else {
			len = sizeof(usbd_uac_fs_config_desc);
			desc = (u8 *)&usbd_uac_fs_config_desc;
		}

		usb_os_memcpy((void *)buf, (void *)desc, len);
		break;

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		len = sizeof(usbd_uac_device_qualifier_desc);
		usb_os_memcpy((void *)buf, (void *)usbd_uac_device_qualifier_desc, len);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			len = sizeof(usbd_uac_fs_config_desc);
			desc = (u8 *)&usbd_uac_fs_config_desc;
		} else {
			len = sizeof(usbd_uac_hs_config_desc);
			desc = (u8 *)&usbd_uac_hs_config_desc;
		}
		usb_os_memcpy((void *)buf, (void *)desc, len);
		buf[USB_CFG_DESC_OFFSET_TYPE] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
		break;

	case USB_DESC_TYPE_STRING:
		switch (USB_LOW_BYTE(req->wValue)) {
		case USBD_IDX_LANGID_STR:
			len = sizeof(usbd_uac_lang_id_desc);
			usb_os_memcpy((void *)buf, (void *)usbd_uac_lang_id_desc, len);
			break;
		case USBD_IDX_MFC_STR:
			len = usbd_get_str_desc(USBD_UAC_MFG_STRING, buf);
			break;
		case USBD_IDX_PRODUCT_STR:
			if (speed == USB_SPEED_HIGH) {
				len = usbd_get_str_desc(USBD_UAC_PROD_HS_STRING, buf);
			} else {
				len = usbd_get_str_desc(USBD_UAC_PROD_FS_STRING, buf);
			}
			break;
		case USBD_IDX_SERIAL_STR:
			len = usbd_get_str_desc(USBD_UAC_SN_STRING, buf);
			break;
		case USBD_IDX_MS_OS_STR:
			/*Not support*/
			break;
		/* Add customer string here */
		default:
			//RTK_LOGS(TAG, RTK_LOG_WARN, "Invalid str idx %d\n", USB_LOW_BYTE(req->wValue));
			break;
		}
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
static void usbd_uac_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	UNUSED(dev);

	if (cdev->cb->status_changed) {
		cdev->cb->status_changed(old_status, status);
	}
}

#if USBD_UAC_DEBUG
/**
  * @brief  UAC status dump thread
  * @param  param: Pointer to parameters
  * @retval void
  */
static void usbd_uac_status_dump_thread(void *param)
{
	UNUSED(param);
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	cdev->isoc_dump_thread = 1;
	u32 loop_idx = 0;
	u32 max_loop = USBD_UAC_DEBUG_LOOP_TIME / USBD_UAC_DEBUG_STEP_TIME;

	while (cdev->isoc_dump_thread) {
		loop_idx = 0;
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB Dump RX %d/TO %d/OW %d/Pktcnt %d/copylen %d/%d\n",
				 cdev->isoc_rx_cnt,
				 cdev->isoc_timeout_cnt,
				 cdev->isoc_overwrite_cnt,
				 usbd_uac_get_read_frame_cnt(),
				 cdev->copy_data_len,
				 cdev->uac_isoc_out.next_xfer
				);
		do {
			if (0 == cdev->isoc_dump_thread) {
				break;
			}
			rtos_time_delay_ms(USBD_UAC_DEBUG_STEP_TIME); //split to many times
		} while (loop_idx++ < max_loop); // 100 * 100
	}

	rtos_task_delete(NULL);
}
#endif

/**
  * @brief  Update or manage the count of audio data bytes
  * @param  audio_len: Data len had been wrote to the audiotrack
  * @retval void
  */
static inline void usbd_uac_get_audio_data_cnt(u32 audio_len)
{
#if USBD_UAC_DEBUG
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	cdev->copy_data_len += audio_len;
#else
	UNUSED(audio_len);
#endif
}

/**
  * @brief  Read data from a USB audio ring buffer
  * @param  pdata_ctrl: Pointer to the buffer control structure
  * @param  buffer: Pointer to Data buffer
  * @param  size: Data length
  * @param  copy_len: Copy length
  * @retval Status
  */
static u32 usbd_uac_read_ring_buf(usbd_uac_buf_ctrl_t *pdata_ctrl, u8 *buffer, u32 size, u32 *copy_len)
{
	usbd_uac_buf_t *p_buf = NULL;

	do {
		p_buf = &(pdata_ctrl->buf_array[pdata_ctrl->isoc_read_idx]);

		if (*copy_len + p_buf->buf_valid_len > size) {
			return 0; //get enough data
		}

		if (p_buf->buf_valid_len > 0) {
			usb_os_memcpy(buffer + *copy_len, p_buf->buf_raw, p_buf->buf_valid_len);
			*copy_len += p_buf->buf_valid_len;
			p_buf->buf_valid_len = 0;
		}

		pdata_ctrl->isoc_read_idx = (pdata_ctrl->isoc_read_idx + 1) % (pdata_ctrl->buf_array_cnt);

	} while (pdata_ctrl->isoc_read_idx != pdata_ctrl->isoc_write_idx);

	return 1;
}

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Initialize UAC device
  * @param  cb: Callback functions for audio events
  * @retval Status
  */
int usbd_uac_init(usbd_uac_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;

	cdev->cur_volume = 0x001F;
	cdev->cur_mute = 0;
	cdev->cur_clk_valid = 1;

	usbd_uac_ep_buf_ctrl_deinit(&(cdev->uac_isoc_in));
	usbd_uac_ep_buf_ctrl_deinit(&(cdev->uac_isoc_out));

	ep_isoc_out->addr = USBD_UAC_ISOC_OUT_EP;
	ep_isoc_out->type = USB_CH_EP_TYPE_ISOC;
	ep_isoc_out->binterval = 1U;
	ep_isoc_in->addr = USBD_UAC_ISOC_IN_EP;
	ep_isoc_in->type = USB_CH_EP_TYPE_ISOC;
	ep_isoc_out->binterval = 1U;

	if (cb != NULL) {
		if ((cb->in.enable == 0) && (cb->out.enable == 0)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Pls cfg UAC EP\n");
			return HAL_ERR_PARA;
		}

		cdev->cb = cb;
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

#if UABD_UAC_DESC_DUMP
	usbd_uac_desc_dump(usbd_uac_hs_config_desc, sizeof(usbd_uac_hs_config_desc) / sizeof(u8));
#endif

	usbd_register_class(&usbd_uac_driver);

#if USBD_UAC_DEBUG
	rtos_task_t task_dump;
	if (rtos_task_create(&task_dump, ((const char *)"usbd_uac_status_dump_thread"), usbd_uac_status_dump_thread, NULL, 1024U, 1) != RTK_SUCCESS) {
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
int usbd_uac_deinit(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

#if USBD_UAC_DEBUG
	cdev->isoc_dump_thread = 0;
#endif

	if (cdev->cb != NULL) {
		if (cdev->cb->deinit != NULL) {
			cdev->cb->deinit();
		}
		cdev->cb = NULL;
	}

	usbd_unregister_class();

	usbd_uac_ep_buf_ctrl_deinit(&(cdev->uac_isoc_in));
	usbd_uac_ep_buf_ctrl_deinit(&(cdev->uac_isoc_out));


	return HAL_OK;
}

/**
  * @brief  Transmit UAC data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbd_uac_transmit_data(u8 *buf, u16 len)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usb_dev_t *dev = cdev->dev;

	if (!dev->is_ready) {
		return HAL_ERR_HW;
	}

	if (len > cdev->uac_isoc_in.isoc_mps) {
		len = cdev->uac_isoc_in.isoc_mps;
	}

	if (usbd_uac_ep_enable(&(cdev->cb->in))) {
		usb_os_memcpy(cdev->uac_isoc_in.isoc_buf, buf, len);
		// usbd_ep_transmit(cdev->dev, USBD_UAC_ISOC_IN_EP, cdev->uac_isoc_in.isoc_buf, len);
	}

	return HAL_OK;
}

/**
  * @brief  Receive UAC data
  * @param  void
  * @retval Status
  */
int usbd_uac_receive_data(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usb_dev_t *dev = cdev->dev;
	usbd_uac_buf_ctrl_t *pbuf_ctrl = &(cdev->uac_isoc_out);
	usbd_uac_buf_t *p_buf = NULL;
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;

	if (!dev->is_ready) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "State %d err\n", dev->is_ready);
		return HAL_ERR_PARA;
	}

	if (usbd_uac_ep_enable(&(cdev->cb->out))) {
		pbuf_ctrl->next_xfer = 1;
		p_buf = &(pbuf_ctrl->buf_array[pbuf_ctrl->isoc_write_idx]);
		// RTK_LOGS(TAG, RTK_LOG_ERROR, "First trigger sema %d cnt %d-%d \n", pdata_ctrl->read_wait_sema,usbd_uac_get_read_frame_cnt(),pbuf_ctrl->isoc_mps);
		ep_isoc_out->xfer_buf = p_buf->buf_raw;
		ep_isoc_out->xfer_len = pbuf_ctrl->isoc_mps;
		usbd_ep_receive(dev, ep_isoc_out);
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
u8 usbd_uac_config(const usbd_audio_cfg_t *uac_cfg, u8 is_record, u32 flag)
{
	UNUSED(flag);

	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *pbuf_ctrl;

	/* all the transfer should finish */
	/* TODO: re initiation the isoc buffer[usbd_uac_buf_ctrl_t] */
	if (uac_cfg && usbd_uac_ep_enable((usbd_audio_cfg_t *)uac_cfg)) {
		if (is_record) { //
			pbuf_ctrl = &(cdev->uac_isoc_in);
		} else { //
			pbuf_ctrl = &(cdev->uac_isoc_out);
		}

		pbuf_ctrl->next_xfer = 0;

		usbd_uac_ep_buf_ctrl_deinit(pbuf_ctrl);
		usbd_uac_ep_buf_ctrl_init(pbuf_ctrl, (usbd_audio_cfg_t *)uac_cfg, cdev->dev->dev_speed);
	}

	return HAL_OK;
}

/**
  * @brief  Initializes and starts the UAC data rx process
  * @param  void
  * @retval Status
  */
u32 usbd_uac_start_play(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	int ret = HAL_OK;

	// RTK_LOGS(TAG, RTK_LOG_ERROR, "usbd uac start\n");

	cdev->uac_isoc_out.isoc_read_idx = 0;
	cdev->uac_isoc_out.isoc_write_idx = 0;

	ret = usbd_uac_receive_data();

	return ret;
}

/**
  * @brief  Stop UAC device play
  * @param  void
  * @retval void
  */
void usbd_uac_stop_play(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	// RTK_LOGS(TAG, RTK_LOG_ERROR, "usbd uac stop\n");

	cdev->uac_isoc_out.next_xfer = 0;
	cdev->uac_isoc_in.next_xfer = 0;
}

/**
  * @brief  Read UAC data
  * @param  buf: Pointer to data buffer
  * @param  size: Data length
  * @param  time_out_ms: Time out
  * @retval copy len
  */
u32 usbd_uac_read(u8 *buffer, u32 size, u32 time_out_ms)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_out);
	u32 copy_len = 0;

	if (pdata_ctrl->next_xfer == 0) {
		return 0;
	}

	if (time_out_ms == 0) {
		if (pdata_ctrl->isoc_read_idx == pdata_ctrl->isoc_write_idx) { //no valid data
			return 0;
		}

		usbd_uac_read_ring_buf(pdata_ctrl, buffer, size, &copy_len);
	} else {
		do {
			// RTK_LOGS(TAG, RTK_LOG_INFO, "Ringtbuf cnt %d/pos=%d-%d \n", usbd_uac_get_readbuf_cnt(),pdata_ctrl->isoc_read_idx,pdata_ctrl->isoc_write_idx);
			if (pdata_ctrl->isoc_read_idx == pdata_ctrl->isoc_write_idx) {
				//wait sema
				pdata_ctrl->read_wait_sema = 1;
				if (rtos_sema_take(pdata_ctrl->uac_isoc_sema, time_out_ms) != RTK_SUCCESS) {
					//if timeout, return
					pdata_ctrl->read_wait_sema = 0;
					break;
				}
				pdata_ctrl->read_wait_sema = 0;
			} else {
				/* if did not read any pkt, loop to check the wr/rd pos*/
				if (usbd_uac_read_ring_buf(pdata_ctrl, buffer, size, &copy_len) == 0) {
					break;
				}
			}
		} while (pdata_ctrl->next_xfer);
	}

	usbd_uac_get_audio_data_cnt(copy_len);
	return copy_len;
}

/**
  * @brief  Get UAC read frame cnt
  * @param  void
  * @retval read frame cnt
  */
u32 usbd_uac_get_read_frame_cnt(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_out);

	if (pdata_ctrl == NULL) {
		return 0;
	}

	return (pdata_ctrl->isoc_write_idx + pdata_ctrl->buf_array_cnt - pdata_ctrl->isoc_read_idx) % (pdata_ctrl->buf_array_cnt);
}
