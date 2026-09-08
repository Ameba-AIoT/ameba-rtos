/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

/* uac 2.0 */
#include "usbd_uac.h"
#ifdef CONFIG_USBD_COMPOSITE
#include "usbd_composite.h"
#endif
#include "usb_uac2.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#define UABD_UAC_DESC_DUMP (0)

#if USBD_UAC_DEBUG
#define USBD_UAC_DEBUG_LOOP_TIME   (10U*1000U)
#define USBD_UAC_DEBUG_STEP_TIME   100U
#endif

#define UABD_UAC_VOL_ERR_VAL       255U

#define USBD_UAC_HS_ISOC_MPS                        1024U   /**< High speed ISOC IN & OUT maximum packet size */
#define USBD_UAC_FS_ISOC_MPS                        1023U   /**< Full speed ISOC IN & OUT maximum packet size */

#define USBD_UAC_LANGID_STRING                      0x0409U               /**< Language ID for string descriptors (0x0409 = English). */
#define USBD_UAC_MFG_STRING                         "Realtek"             /**< Manufacturer string. */
#define USBD_UAC_PROD_HS_STRING                     "Realtek UAC2.0 (HS)" /**< Product string for High-Speed mode. */
#define USBD_UAC_PROD_FS_STRING                     "Realtek UAC2.0 (FS)" /**< Product string for Full-Speed mode. */
#define USBD_UAC_SN_STRING                          "1234567890"          /**< Serial number string. */

/**
 * Defines Audio trx buffer MAX count.
 */
#define USBD_UAC_RX_BUF_MAX_CNT                     20U     /**< RX Ringbuf count (scaled by 2^(bInterval-1)) */
#define USBD_UAC_TX_BUF_MAX_CNT                     20U     /**< TX Ringbuf count */

#define USBD_UAC_FS_DEFAULT_BINTERVAL               1U      /**< FS ISOC bInterval used in EP descriptors */
#define USBD_UAC_HS_DEFAULT_BINTERVAL               1U      /**< HS ISOC bInterval used in EP descriptors */

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
 * Defines USB Audio Headphone entity id.
 */
#define USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES            0x15U /**< Define headphone clock id. */
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES    0x01U /**< Define headphone input terminal id. */
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT          0x05U /**< Define headphone feature unit id. */
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES   0x09U /**< Define headphone output terminal id. */

/**
 * Defines USB Audio Microphone entity id.
 */
#define USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE            0x12U /**< Define microphone clock id. */
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE    0x02U /**< Define microphone input terminal id. */
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT           0x08U /**< Define microphone feature unit id. */
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE   0x10U /**< Define microphone output terminal id. */

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

/* AC IF header interface num */


/* AC feature uint descriptor length */
#define USBD_UAC_AC_FU_HEAD_DESC_LEN(ch_cnt)        (USB_UAC2_LEN_FUNC_UNIT_DESC + 1 + 4 * (ch_cnt))
/* AC mic entities (clock + input terminal + feature unit + output terminal) descriptor length */
#define USBD_UAC_AC_MIC_IF_LEN \
    (USB_UAC2_LEN_CLK_SRC_DESC + USB_UAC2_LEN_AC_IN_TTY_DESC + \
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_IN_DEFAULT_CH_CNT) + USB_UAC2_LEN_AC_OUT_TTY_DESC)
/* AC IF header descriptor length */
#define USBD_UAC_AC_IF_HEAD_DESC_LEN(ch_cnt)         \
    (USB_UAC2_LEN_CTRL_IF_HEADER + USB_UAC2_LEN_CLK_SRC_DESC + USB_UAC2_LEN_AC_IN_TTY_DESC + \
	USBD_UAC_AC_FU_HEAD_DESC_LEN(ch_cnt) + USB_UAC2_LEN_AC_OUT_TTY_DESC + USBD_UAC_AC_MIC_IF_LEN)

/* len of total Audio control interface */
#define USBD_UAC_AC_IF_LEN(ch_cnt)                 (USB_LEN_IF_DESC + USBD_UAC_AC_IF_HEAD_DESC_LEN(ch_cnt))

/* len of each Audio stream interface/altsetting (one EP) */
#define USBD_UAC_AS_EIF_LEN                        (USB_LEN_IF_DESC + USB_UAC2_LEN_AS_IF_ALT_SET_DESC + USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC \
                                                   + USB_LEN_EP_DESC + USB_UAC2_LEN_DATA_EP_DESC)

/* len of total Audio stream interface */
#define USBD_UAC_AS_TIF_LEN(alt_num)               (USBD_UAC_AS_EIF_LEN * (alt_num - 1) + USB_LEN_IF_DESC)

/* full speed AS alt setting num */
#define USBD_UAC_FS_AS_ALT_SETTING_NUM              5U
/* high speed AS alt setting num */
#define USBD_UAC_HS_AS_ALT_SETTING_NUM              5U
/* microphone AS alt setting num (alt0 zero-bw + alt1 mono streaming) */
#define USBD_UAC_MIC_AS_ALT_SETTING_NUM             2U

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
    (USB_LEN_CFG_DESC + USB_LEN_IAD_DESC  + USBD_UAC_AC_IF_LEN(ch_cnt) + USBD_UAC_AS_TIF_LEN(USBD_UAC_HS_AS_ALT_SETTING_NUM) \
	+ USBD_UAC_AS_TIF_LEN(USBD_UAC_MIC_AS_ALT_SETTING_NUM))

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
    (USB_LEN_CFG_DESC + USB_LEN_IAD_DESC  + USBD_UAC_AC_IF_LEN(ch_cnt) + USBD_UAC_AS_TIF_LEN(USBD_UAC_FS_AS_ALT_SETTING_NUM) \
	+ USBD_UAC_AS_TIF_LEN(USBD_UAC_MIC_AS_ALT_SETTING_NUM))

/* Input terminal */
#define USBD_UAC_CH_CONFIG_TYPE_LOW(ch_cnt)         (USB_LOW_BYTE(USBD_UAC_GET_CH_CONFIG(ch_cnt)))
#define USBD_UAC_CH_CONFIG_TYPE_HIGH(ch_cnt)        (USB_HIGH_BYTE(USBD_UAC_GET_CH_CONFIG(ch_cnt)))

/* Output terminal */
#define USBD_UAC_OT_DESC_TYPE_LOW(ch_cnt)           (USB_LOW_BYTE(USBD_UAC_GET_OT_TYPE(ch_cnt)))
#define USBD_UAC_OT_DESC_TYPE_HIGH(ch_cnt)          (USB_HIGH_BYTE(USBD_UAC_GET_OT_TYPE(ch_cnt)))

#define USBD_UAC_INIT_SUB_RANGE(sub_range, min_sampling_freq, max_sampling_freq, res) \
    do {                                               \
        (sub_range).dMIN = (min_sampling_freq);        \
        (sub_range).dMAX = (max_sampling_freq);        \
        (sub_range).dRES = (res);                      \
    } while(0)

/* Private function prototypes -----------------------------------------------*/
static int usbd_uac_set_config(usb_dev_t *dev, u8 config);
static int usbd_uac_clear_config(usb_dev_t *dev, u8 config);
static int usbd_uac_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
static int usbd_uac_handle_sof(usb_dev_t *dev);
static void usbd_uac_append_data(usbd_uac_buf_ctrl_t *pdata_ctrl);
static int usbd_uac_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static int usbd_uac_handle_ep0_data_out(usb_dev_t *dev);
static void usbd_uac_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
#if UABD_UAC_DESC_DUMP
static int usbd_uac_desc_dump(u8 *pbuf, int len);
#endif
#if USBD_UAC_DEBUG
static void usbd_uac_status_dump_thread(void *param);
#endif
static inline void usbd_uac_get_audio_data_cnt(u32 audio_len);
static inline u16 usbd_uac_get_ring_buf_cnt(u8 speed);
static u16 usbd_uac_get_mps(const usbd_audio_cfg_t *params, u8 speed);
static u8 usbd_uac_get_ch_config(u8 ch_cnt);
static inline u8 usbd_uac_ep_enable(const usbd_audio_cfg_t *ep);
static void usbd_uac_clk_valid_req(usb_dev_t *dev, u8 flag);
static void usbd_uac_cur_sampling_freq_req(usb_dev_t *dev, u32 frequency);
static int usbd_uac_sampling_freq_ctrl_range_req(usb_dev_t *dev, u16 max_len);
static void usbd_uac_connect_ctrl_req(usb_dev_t *dev, u8 ch_num, u32 ch_cfg, u16 max_len);
static bool usbd_uac_is_valid_sample_rate(u32 freq, u8 speed);
static void usbd_uac_isoc_in_process_xfer(usb_dev_t *dev);
static void usbd_uac_isoc_in_process_complete(usb_dev_t *dev);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "UAC";

static u32 usbd_uac_sampling_rates[USBD_UAC_SAMPLING_FREQ_MAX_COUNT] = {USBD_UAC_SAMPLING_FREQ_44K, USBD_UAC_SAMPLING_FREQ_48K, USBD_UAC_SAMPLING_FREQ_96K, USBD_UAC_SAMPLING_FREQ_192K};

/* Cache-aligned ISOC OUT DMA scratch (one packet), copied into the ring buffer. */
static u8 usbd_uac_rx_buf[USBD_UAC_HS_ISOC_MPS] USB_DMA_ALIGNED;

/* Cache-aligned ISOC IN DMA scratch (one packet), staged from the ring buffer. */
static u8 usbd_uac_tx_buf[USBD_UAC_HS_ISOC_MPS] USB_DMA_ALIGNED;

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

#ifndef CONFIG_USB_FS
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
	0x00,                              /* wTotalLength: patched at runtime */
	0x00,
	0x03,                              /* Number of interfaces supported by this configuration */
	0x01,                              /* Value to use as an argument to the SetConfiguration() request to select this configuration */
	0x00,                              /* Index of string descriptor describing this configuration */
	0x80,                              /* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
	0x32,                              /* bMaxPower */

	/* 4.6 Interface Association Descriptor */
	/* IAD Descriptor */
	USB_LEN_IAD_DESC,                  /* Size of this descriptor (byte_width) */
	USB_UAC_IAD_DESCRIPTOR_TYPE,            /* Interface Association Descriptor type */
	0x00,                              /* First Index: Audio Control Interface Index - Headset (0) */
	3,                /* Audio Control Interface, Audio Streaming Interfaces () */
	USB_UAC_IF_CLASS_AUDIO,           /* Audio Device Class  */
	0x00,                              /* No subclass */
	USB_UAC_IP_VERSION_2,            /* Audio Protocol IP version 2.00 */
	0x00,                              /* Function string descriptor index (0) */

	/* 4.7.1 Standard AC Interface Descriptor */
	/* Interface 0 Descriptor */
	USB_LEN_IF_DESC,                   /* bLength */
	USB_DESC_TYPE_INTERFACE,           /* Interface Descriptor type */
	USB_UAC2_IF_IDX_AC_HEADSET,        /* Audio Control Interface Index - Headset (0) */
	0x00,                              /* bAlternateSetting */
	0x00,                              /* No associated endpoints with this interface (uses endpoint 0) */
	USB_UAC_CLASS_CODE,               /* Audio Device Class */
	USB_UAC_SUBCLASS_AUDIOCONTROL,    /* Audio Control Interface */
	USB_UAC_IP_VERSION_2,            /* Audio Protocol IP version 2.00 */
	0x00,                              /* Interface string descriptor index (0) */

	/* 4.7.2 Class-Specific AC Interface Descriptor */
	/* Audio headset */
	USB_UAC2_LEN_CTRL_IF_HEADER,                                         /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                               /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_HEADER,                                  /* HEADER descriptor subtype  */
	USB_LOW_BYTE(0x0200),                                                /* bcdUSB */
	USB_HIGH_BYTE(0x0200),
	USB_UAC2_FUNC_CATEGORY_CODE_DESKTOP_SPEAKER,                         /* DESKTOP_SPEAKER(0x01) : Indicating the primary use of this audio function   */
	USB_LOW_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT)),
	USB_HIGH_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT)),/* Total number of byte_width returned for the class-specific AudioControl interface descriptor. Includes
                                                                         the combined length of this descriptor header and all Unit and Terminal descriptors.   */
	0x00,                                                                /* D1..0: Latency Control  */

	/* Audio Control Clock Source Unit Descriptor 2.0 */
	USB_UAC2_LEN_CLK_SRC_DESC,                               /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC2_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE,                /* CLOCK_SOURCE descriptor subtype  */
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

	/* Headphone Audio Control Input Terminal Descriptor 2.0 */
	USB_UAC2_LEN_AC_IN_TTY_DESC,                             /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_INPUT_TERMINAL,              /* INPUT_TERMINAL descriptor subtype   */
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
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_FEATURE_UNIT,                /* FEATURE_UNIT descriptor subtype   */
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
	USB_UAC2_LEN_AC_OUT_TTY_DESC,                             /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                    /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AC_OUTPUT_TERMINAL,              /* OUTPUT_TERMINAL descriptor subtype   */
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

	/* Audio Control Clock Source Unit Descriptor 2.0 (Microphone) */
	USB_UAC2_LEN_CLK_SRC_DESC,                               /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC2_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE,                /* CLOCK_SOURCE descriptor subtype  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE,         /* Constant uniquely identifying the Clock Source Entity within
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
	USB_UAC2_LEN_AC_IN_TTY_DESC,                             /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_INPUT_TERMINAL,               /* INPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE, /* Constant uniquely identifying the Terminal within the audio
	                                                                  function. This value is used in all requests to address this Terminal.   */
	0x01,
	0x02,                                                    /* Microphone terminal type (0x0201) */
	0x00,                                                    /* This Input Terminal has no association   */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE,         /* ID of the Clock Entity to which this Input Terminal is connected.  */
	USBD_UAC_IN_DEFAULT_CH_CNT,                              /* This Terminal's output audio channel cluster has 2 logical channels (stereo)  */
	0x03,                                                    /* bmChannelConfig: Front Left + Front Right (stereo) */
	0x00,
	0x00,
	0x00,
	0x00,                                                    /* Index of a string descriptor, describing the name of the first logical channel.  */
	0x00,
	0x00,                                                    /* bmControls: no controls present */
	0x00,                                                    /* Index of a string descriptor, describing the Input Terminal.  */

	/* Microphone FEATURE_UNIT descriptor subtype */
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_IN_DEFAULT_CH_CNT), /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_FEATURE_UNIT,                 /* FEATURE_UNIT descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT,        /* Constant uniquely identifying the Unit within the audio function. This
	                                                                value is used in all requests to address this Unit.  */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE, /* ID of the Unit or Terminal to which this Feature Unit is connected. */
	0x0F,
	0x00,
	0x00,
	0x00,                                                    /* logic channel 0 bmaControls(0)(0x0000000F): Mute + Volume Control present */
	0x00,
	0x00,
	0x00,
	0x00,                                                    /* bmaControls(1) logical channel 1: no controls */
	0x00,
	0x00,
	0x00,
	0x00,                                                    /* bmaControls(2) logical channel 2: no controls */
	0x00,                                                    /* iFeature, Index of a string descriptor, describing this Feature Unit.*/

	/* Microphone OUTPUT_TERMINAL descriptor subtype   */
	USB_UAC2_LEN_AC_OUT_TTY_DESC,                             /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                    /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AC_OUTPUT_TERMINAL,               /* OUTPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE, /* Constant uniquely identifying the Terminal within the audio
	                                                                 function. This value is used in all requests to address this Terminal.   */
	0x01,
	0x01,                                                     /* USB Streaming terminal type (0x0101) */
	0x00,                                                     /* This Output Terminal has no association  */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT,         /* ID of the Unit or Terminal to which this Terminal is connected.  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE,          /* ID of the Clock Entity to which this Output Terminal is connected  */
	0x00,
	0x00,                                                     /* bmControls: no controls present */
	0x00,                                                     /* Index of a string descriptor, describing the Output Terminal.  */


	/* 4.9.1 Standard AS Interface Descriptor 1/0*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                      /* The number of this interface is 1.   */
	0x00,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x00,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.1 Standard AS Interface Descriptor 1/1*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                      /* The number of this interface is 1.   */
	0x01,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
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
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_BYTE_WIDTH_2,                                   /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_BYTE_WIDTH_2),               /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_DEFAULT_BYTE_WIDTH)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */


	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
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
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
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
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_DEFAULT_BYTE_WIDTH)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */


	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
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
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
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
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_DEFAULT_BYTE_WIDTH)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */


	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
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
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
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
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_DEFAULT_BYTE_WIDTH)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */


	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif

	/* 4.9.1 Standard AS Interface Descriptor 1/2 (Microphone) */
	/* Interface2 Descriptor alt setting 0 zero-bandwidth */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE,                    /* The number of this interface is 2.   */
	0x00,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x00,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.1 Standard AS Interface Descriptor 2/2 (Microphone) */
	/* Interface2 Descriptor alt setting 1 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE,                    /* The number of this interface is 2.   */
	0x01,                                                     /* The value used to select the alternate setting for this interface is 1 */
	0x01,                                                     /* The number of endpoints used by this interface is 1 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE, /* The Terminal ID of the terminal to which this interface is connected */
	0xF,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_IN_DEFAULT_CH_CNT,                              /* Number of physical channels in the AS Interface audio channel cluster */
	0x03,                                                    /* bmChannelConfig: Front Left + Front Right (stereo) */
	0x00,
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */


	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Microphone Audio Streaming Format Type Descriptor 2.0 */
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_IN_DEFAULT_BYTE_WIDTH,                          /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_IN_DEFAULT_BYTE_WIDTH),      /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_D2H,                                                  /* bEndpointAddress: dir IN (placeholder) */
	0x05,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_IN_DEFAULT_CH_CNT, USBD_UAC_IN_DEFAULT_BYTE_WIDTH)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_IN_DEFAULT_CH_CNT, USBD_UAC_IN_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */


	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
};
#endif

/* USB UAC Device Configuration Descriptor */
/* USB Standard Configuration Descriptor */
static const u8 usbd_uac_fs_config_desc[USBD_UAC_FS_CFG_DESC_BUF_LEN(USBD_UAC_DEFAULT_CH_CNT)] = {
	USB_LEN_CFG_DESC,                                        /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                             /* bDescriptorType */
	0x00,                                                    /* wTotalLength: patched at runtime */
	0x00,
	0x03,                                                    /* Number of interfaces supported by this configuration */
	0x01,                                                    /* Value to use as an argument to the SetConfiguration() request to select this configuration */
	0x00,                                                    /* Index of string descriptor describing this configuration */
	0x80,                                                    /* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
	0x32,                                                    /* bMaxPower */

	/* 4.6 Interface Association Descriptor */
	/* IAD Descriptor */
	USB_LEN_IAD_DESC,                  /* Size of this descriptor (byte_width) */
	USB_UAC_IAD_DESCRIPTOR_TYPE,      /* Interface Association Descriptor type */
	0x00,                              /* First Index: Audio Control Interface Index - Headset (0) */
	0x03,                              /* Audio Control Interface, Audio Streaming Interfaces () */
	USB_UAC_IF_CLASS_AUDIO,           /* Audio Device Class  */
	0x00,                              /* No subclass */
	USB_UAC_IP_VERSION_2,            /* Audio Protocol IP version 2.00 */
	0x00,                              /* Function string descriptor index (0) */

	/* 4.7.1 Standard AC Interface Descriptor */
	/* Interface 0 Descriptor */
	USB_LEN_IF_DESC,                   /* bLength */
	USB_DESC_TYPE_INTERFACE,           /* Interface Descriptor type */
	USB_UAC2_IF_IDX_AC_HEADSET,        /* Audio Control Interface Index - Headset (0) */
	0x00,                              /* bAlternateSetting */
	0x00,                              /* No associated endpoints with this interface (uses endpoint 0) */
	USB_UAC_CLASS_CODE,               /* Audio Device Class */
	USB_UAC_SUBCLASS_AUDIOCONTROL,    /* Audio Control Interface */
	USB_UAC_IP_VERSION_2,            /* Audio Protocol IP version 2.00 */
	0x00,                              /* Interface string descriptor index (0) */

	/* 4.7.2 Class-Specific AC Interface Descriptor */
	/* Audio headset */
	USB_UAC2_LEN_CTRL_IF_HEADER,                                          /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                                /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_HEADER,                                   /* HEADER descriptor subtype  */
	USB_LOW_BYTE(0x0200),                                                 /* bcdUSB */
	USB_HIGH_BYTE(0x0200),
	USB_UAC2_FUNC_CATEGORY_CODE_DESKTOP_SPEAKER,                          /* DESKTOP_SPEAKER(0x01) : Indicating the primary use of this audio function   */
	USB_LOW_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT)),
	USB_HIGH_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT)), /* Total number of byte_width returned for the class-specific AudioControl interface descriptor. Includes
                                                                           the combined length of this descriptor header and all Unit and Terminal descriptors.   */
	0x00,                                                                 /* D1..0: Latency Control  */

	/* Audio Control Clock Source Unit Descriptor 2.0 */
	USB_UAC2_LEN_CLK_SRC_DESC,                         /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,             /* CS_INTERFACE Descriptor Type  */
	USB_UAC2_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE,          /* CLOCK_SOURCE descriptor subtype  */
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

	/* Headphone Audio Control Input Terminal Descriptor 2.0 */
	USB_UAC2_LEN_AC_IN_TTY_DESC,                             /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_INPUT_TERMINAL,              /* INPUT_TERMINAL descriptor subtype   */
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
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_FEATURE_UNIT,                /* FEATURE_UNIT descriptor subtype   */
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
	USB_UAC2_LEN_AC_OUT_TTY_DESC,                             /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                    /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AC_OUTPUT_TERMINAL,              /* OUTPUT_TERMINAL descriptor subtype   */
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

	/* Audio Control Clock Source Unit Descriptor 2.0 (Microphone) */
	USB_UAC2_LEN_CLK_SRC_DESC,                               /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC2_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE,                /* CLOCK_SOURCE descriptor subtype  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE,         /* Constant uniquely identifying the Clock Source Entity within
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
	USB_UAC2_LEN_AC_IN_TTY_DESC,                             /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_INPUT_TERMINAL,               /* INPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE, /* Constant uniquely identifying the Terminal within the audio
	                                                                  function. This value is used in all requests to address this Terminal.   */
	0x01,
	0x02,                                                    /* Microphone terminal type (0x0201) */
	0x00,                                                    /* This Input Terminal has no association   */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE,         /* ID of the Clock Entity to which this Input Terminal is connected.  */
	USBD_UAC_IN_DEFAULT_CH_CNT,                              /* This Terminal's output audio channel cluster has 2 logical channels (stereo)  */
	0x03,                                                    /* bmChannelConfig: Front Left + Front Right (stereo) */
	0x00,
	0x00,
	0x00,
	0x00,                                                    /* Index of a string descriptor, describing the name of the first logical channel.  */
	0x00,
	0x00,                                                    /* bmControls: no controls present */
	0x00,                                                    /* Index of a string descriptor, describing the Input Terminal.  */

	/* Microphone FEATURE_UNIT descriptor subtype */
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_IN_DEFAULT_CH_CNT), /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_FEATURE_UNIT,                 /* FEATURE_UNIT descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT,        /* Constant uniquely identifying the Unit within the audio function. This
	                                                                value is used in all requests to address this Unit.  */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE, /* ID of the Unit or Terminal to which this Feature Unit is connected. */
	0x0F,
	0x00,
	0x00,
	0x00,                                                    /* logic channel 0 bmaControls(0)(0x0000000F): Mute + Volume Control present */
	0x00,
	0x00,
	0x00,
	0x00,                                                    /* bmaControls(1) logical channel 1: no controls */
	0x00,
	0x00,
	0x00,
	0x00,                                                    /* bmaControls(2) logical channel 2: no controls */
	0x00,                                                    /* iFeature, Index of a string descriptor, describing this Feature Unit.*/

	/* Microphone OUTPUT_TERMINAL descriptor subtype   */
	USB_UAC2_LEN_AC_OUT_TTY_DESC,                             /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                    /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AC_OUTPUT_TERMINAL,               /* OUTPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE, /* Constant uniquely identifying the Terminal within the audio
	                                                                 function. This value is used in all requests to address this Terminal.   */
	0x01,
	0x01,                                                     /* USB Streaming terminal type (0x0101) */
	0x00,                                                     /* This Output Terminal has no association  */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT,         /* ID of the Unit or Terminal to which this Terminal is connected.  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE,          /* ID of the Clock Entity to which this Output Terminal is connected  */
	0x00,
	0x00,                                                     /* bmControls: no controls present */
	0x00,                                                     /* Index of a string descriptor, describing the Output Terminal.  */


	/* 4.9.1 Standard AS Interface Descriptor 1/0*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                      /* The number of this interface is 1.   */
	0x00,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x00,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */


	/* 4.9.1 Standard AS Interface Descriptor 1/1*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                      /* The number of this interface is 1.   */
	0x01,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
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
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_BYTE_WIDTH_2,                                   /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_BYTE_WIDTH_2),               /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_DEFAULT_BYTE_WIDTH)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
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
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
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
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_DEFAULT_BYTE_WIDTH)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */


	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
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
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
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
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_BYTE_WIDTH_2,                                   /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_BYTE_WIDTH_2),               /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_BYTE_WIDTH_2)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_BYTE_WIDTH_2)),
	0x01,                                                    /* bInterval */


	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
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
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
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
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_DEFAULT_BYTE_WIDTH)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */


	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif

	/* 4.9.1 Standard AS Interface Descriptor 1/2 (Microphone) */
	/* Interface2 Descriptor alt setting 0 zero-bandwidth */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE,                    /* The number of this interface is 2.   */
	0x00,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x00,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.1 Standard AS Interface Descriptor 2/2 (Microphone) */
	/* Interface2 Descriptor alt setting 1 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE,                    /* The number of this interface is 2.   */
	0x01,                                                     /* The value used to select the alternate setting for this interface is 1 */
	0x01,                                                     /* The number of endpoints used by this interface is 1 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE, /* The Terminal ID of the terminal to which this interface is connected */
	0xF,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_IN_DEFAULT_CH_CNT,                              /* Number of physical channels in the AS Interface audio channel cluster */
	0x03,                                                    /* bmChannelConfig: Front Left + Front Right (stereo) */
	0x00,
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */


	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Microphone Audio Streaming Format Type Descriptor 2.0 */
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_IN_DEFAULT_BYTE_WIDTH,                          /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_IN_DEFAULT_BYTE_WIDTH),      /* The number of effectively used bits from the available bits in an audio subslot   */


	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_D2H,                                                  /* bEndpointAddress: dir IN (placeholder) */
	0x05,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_IN_DEFAULT_CH_CNT, USBD_UAC_IN_DEFAULT_BYTE_WIDTH)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_IN_DEFAULT_CH_CNT, USBD_UAC_IN_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */


	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
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
	return HAL_OK;
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
		return (USBD_UAC_HS_SOF_COUNT_PER_MS * USBD_UAC_RX_BUF_MAX_CNT) / USBD_UAC_POW2(USBD_UAC_HS_DEFAULT_BINTERVAL - 1U);
	}

	return USBD_UAC_RX_BUF_MAX_CNT / USBD_UAC_POW2(USBD_UAC_FS_DEFAULT_BINTERVAL - 1U);
}

/**
  * @brief  Get the MPS based on configuration parameters and speed
  * @param  params: Pointer to audio configuration parameters structure
  * @param  speed: USB connection speed
  * @retval Calculated MPS
  */
static u16 usbd_uac_get_mps(const usbd_audio_cfg_t *params, u8 speed)
{
	u16 mps_value = 0;
	if (NULL == params) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No Param\n");
		return mps_value;
	}

	if (speed == USB_SPEED_HIGH) {
		mps_value = params->ch_cnt * params->byte_width * (params->sampling_freq / USBD_UAC_ONE_KHZ / USBD_UAC_HS_SOF_COUNT_PER_MS + 1);
		if (mps_value > USBD_UAC_HS_ISOC_MPS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid HS MPS %d-%d\n", mps_value, USBD_UAC_HS_ISOC_MPS);
			mps_value = 0;
			return mps_value;
		}
	} else {
		/* for 44.1khz or the host clk is bigger than the device */
		mps_value = params->ch_cnt * params->byte_width * (params->sampling_freq / USBD_UAC_ONE_KHZ + 1);
		if (mps_value > USBD_UAC_FS_ISOC_MPS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid FS MPS %d-%d\n", mps_value, USBD_UAC_FS_ISOC_MPS);
			mps_value = 0;
			return mps_value;
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
	buf_ctrl->isoc_mps = 0;
	buf_ctrl->buf_list_cnt = 0;
	buf_ctrl->next_xfer = 0;
	buf_ctrl->sof_idx = 0;
	buf_ctrl->data_idx = 0;
	buf_ctrl->xfer_cnt = 0;
	buf_ctrl->last_xfer_len = 0;

	if (buf_ctrl->uac_sema_valid != 0) {
		buf_ctrl->uac_sema_valid = 0;
		if (buf_ctrl->read_wait_sema != 0) {
			rtos_sema_give(buf_ctrl->uac_isoc_sema);
		}
		do {
			usb_os_delay_us(100U);
		} while (buf_ctrl->read_wait_sema != 0);
		rtos_sema_delete(buf_ctrl->uac_isoc_sema);
		buf_ctrl->uac_isoc_sema = NULL;
	}

	usb_ringbuf_manager_deinit(&(buf_ctrl->buf_list));
}

/**
  * @brief  Initialize UAC endpoint buffer control structure
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @param  params: Pointer to audio configuration parameters structure
  * @param  speed: USB connection speed
  * @retval Status
  */
static int usbd_uac_ep_buf_ctrl_init(usbd_uac_buf_ctrl_t *buf_ctrl, const usbd_audio_cfg_t *params, u8 speed)
{
	if (usbd_uac_ep_enable(params) != 0) {
		buf_ctrl->isoc_mps = usbd_uac_get_mps(params, speed);
		if (buf_ctrl->isoc_mps == 0) {
			return HAL_ERR_PARA;
		}
		buf_ctrl->buf_list_cnt = (u8)usbd_uac_get_ring_buf_cnt(speed);
		buf_ctrl->binterval = (speed == USB_SPEED_HIGH) ? USBD_UAC_HS_DEFAULT_BINTERVAL : USBD_UAC_FS_DEFAULT_BINTERVAL;

		if (usb_ringbuf_manager_init(&(buf_ctrl->buf_list), buf_ctrl->buf_list_cnt, buf_ctrl->isoc_mps, 1) != HAL_OK) {
			return HAL_ERR_MEM;
		}

		if (buf_ctrl->uac_isoc_sema == NULL) {
			if (rtos_sema_create(&(buf_ctrl->uac_isoc_sema), 0U, 1U) != RTK_SUCCESS) {
				usb_ringbuf_manager_deinit(&(buf_ctrl->buf_list));
				return HAL_ERR_MEM;
			}
		}
		buf_ctrl->uac_sema_valid = 1;
		buf_ctrl->read_wait_sema = 0;
		buf_ctrl->sof_idx = 0;
		buf_ctrl->data_idx = 0;
		buf_ctrl->xfer_cnt = 0;
		buf_ctrl->last_xfer_len = 0;
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
static inline u8 usbd_uac_ep_enable(const usbd_audio_cfg_t *ep)
{
	if (ep != NULL) {
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

	usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)&valid, sizeof(valid));
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

	usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)&dCur, sizeof(dCur));
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
	usb_uac2_freq_t sampling_freq_range;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	u16 num_sub_ranges;
	u16 len;
	u16 data_offset = 0;

	if (dev->dev_speed  == USB_SPEED_HIGH) {
		num_sub_ranges = USBD_UAC_HS_SAMPLING_FREQ_COUNT;
	} else {
		num_sub_ranges = USBD_UAC_FS_SAMPLING_FREQ_COUNT;
	}

	len = sizeof(num_sub_ranges) + num_sub_ranges * sizeof(usb_uac2_freq_t);

	/* Build the response in place in the EP0 buffer: setup() is ISR context, no
	 * heap alloc; one sub-range is staged on the stack per iteration. */
	usb_os_memcpy(ep0_in->xfer_buf, (u8 *)&num_sub_ranges, sizeof(num_sub_ranges));
	data_offset += sizeof(num_sub_ranges);

	for (u8 i = 0; i < num_sub_ranges; i++) {
		USBD_UAC_INIT_SUB_RANGE(sampling_freq_range, usbd_uac_sampling_rates[i], usbd_uac_sampling_rates[i], 0);
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
static void usbd_uac_connect_ctrl_req(usb_dev_t *dev, u8 ch_num, u32 ch_cfg, u16 max_len)
{
	usb_uac2_ac_connect_ctrl_t response = {0, 0, 0};
	usbd_ep_t *ep0_in = &dev->ep0_in;
	u16 len = sizeof(response);

	response.bNrChannels = ch_num;
	response.bmChannelConfig = ch_cfg;

	if (len > max_len) {
		len = max_len;
	}

	usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)&response, len);
	ep0_in->xfer_len = len;
	usbd_ep_transmit(dev, ep0_in);
}


/**
  * @brief  Set UAC class configuration
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_uac_set_config(usb_dev_t *dev, u8 config)
{
	int ret = HAL_OK;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	/* Only the bConfigurationValue advertised in the config descriptor is valid */
	if (config != 1U) {
		return HAL_ERR_PARA;
	}

	cdev->dev = dev;

	if (!cdev->from_composite) {
#ifdef CONFIG_USBD_SELF_POWERED
		dev->self_powered = 1;
#else
		dev->self_powered = 0;
#endif
#ifdef CONFIG_USBD_REMOTE_WAKEUP_EN
		dev->remote_wakeup_en = 1;
#else
		dev->remote_wakeup_en = 0;
#endif
	}

	cdev->alt_setting = 0U;

	/* Invalidate the cached stream format on each SET_CONFIGURATION so the first
	 * SET_INTERFACE(alt=1) of the new session re-fires format_changed(). Without
	 * this, a hotplug re-enumeration leaves cur_ch_cnt/cur_byte_width matching the
	 * host's selection, the "!=" guard drops format_changed(), and apps that gate
	 * playback on it never restart. cur_sampling_freq keeps its private_init default. */
	cdev->cur_ch_cnt = 0U;
	cdev->cur_byte_width = 0U;

	if (cdev->cb->set_config != NULL) {
		cdev->cb->set_config();
	}

	return ret;
}

/**
  * @brief  Clear UAC configuration
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_uac_clear_config(usb_dev_t *dev, u8 config)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	usb_ep_info_t *info;

	UNUSED(config);

	if (dev == NULL || cdev == NULL) {
		return HAL_OK;
	}

	/* DeInit ISOC IN EP */
	info = &ep_isoc_in->info;
	if ((cdev->cb != NULL) && (usbd_uac_ep_enable(&(cdev->cb->in)) != 0) && info->addr != 0) {
		usbd_ep_deinit(dev, ep_isoc_in);
	}

	/* DeInit ISOC OUT EP */
	info = &ep_isoc_out->info;
	if ((cdev->cb != NULL) && (usbd_uac_ep_enable(&(cdev->cb->out)) != 0) && info->addr != 0) {
		usbd_ep_deinit(dev, ep_isoc_out);
	}

	return HAL_OK;
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
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  req: USB CTRL requests
  * @retval Status
  */
static int usbd_uac_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	const usbd_uac_cb_t *cb = cdev->cb;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	int ret = HAL_OK;
	u8 entityId, controlSelector;
	u8 ch_cnt;
	u8 byte_width;
	u8 alt_setting;
	u8 fmt_change = 0;

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			/* Ref USB 2.0 9.4.10: the endpoints of the selected interface return to their
			   default state, not halted and data toggle DATA0. All the endpoints of this
			   class are isochronous, they can neither be halted nor own a data toggle, so
			   nothing has to be reset here. */
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				alt_setting = USB_LOW_BYTE(req->wValue);
				/* Ref USB 2.0 Table 9-10: the whole wIndex is the interface number */
				if (req->wIndex == USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE) {
					cdev->alt_setting_in = alt_setting;
				} else if ((alt_setting != cdev->alt_setting) && alt_setting) {
					cdev->alt_setting = alt_setting;
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
						}
						if (fmt_change != 0) {
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
				/* Ref USB 2.0 Table 9-11: the whole wIndex is the interface number */
				ep0_in->xfer_buf[0] = (req->wIndex == USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE) ? cdev->alt_setting_in : cdev->alt_setting;
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
		if ((req->bmRequestType & USB_REQ_RECIPIENT_MASK) != USB_REQ_RECIPIENT_INTERFACE) {
			ret = HAL_ERR_PARA;
			break;
		}
		entityId = USB_HIGH_BYTE(req->wIndex);
		controlSelector = USB_HIGH_BYTE(req->wValue);

		if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
			switch (entityId) {
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE:
				if (controlSelector == USB_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						usbd_uac_cur_sampling_freq_req(dev, cdev->cur_sampling_freq);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						usbd_uac_sampling_freq_ctrl_range_req(dev, req->wLength);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 0);
						ret = HAL_ERR_PARA;
					}
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 1);
					ret = HAL_ERR_PARA;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES:
				if (controlSelector == USB_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						usbd_uac_cur_sampling_freq_req(dev, cdev->cur_sampling_freq);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						usbd_uac_sampling_freq_ctrl_range_req(dev, req->wLength);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 2);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USB_UAC2_CS_CLK_VALID_CONTROL) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						usbd_uac_clk_valid_req(dev, cdev->cur_clk_valid);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 3);
						ret = HAL_ERR_PARA;
					}
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 4);
					ret = HAL_ERR_PARA;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE: //in
				if (controlSelector == USB_UAC2_TE_CONNECTOR_CONTROL) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						usbd_uac_connect_ctrl_req(dev, 1, 0x1, req->wLength);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 5);
						ret = HAL_ERR_PARA;
					}
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 6);
					ret = HAL_ERR_PARA;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES: //out
				if (controlSelector == USB_UAC2_TE_CONNECTOR_CONTROL) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						ch_cnt = cdev->cb->out.ch_cnt;
						usbd_uac_connect_ctrl_req(dev, ch_cnt, usbd_uac_get_ch_config(ch_cnt), req->wLength);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 7);
						ret = HAL_ERR_PARA;
					}
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 8);
					ret = HAL_ERR_PARA;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT:
				if (controlSelector == USB_UAC_FU_MUTE) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						ep0_in->xfer_buf[0] = cdev->cur_mute;
						ep0_in->xfer_len = 1U;
						usbd_ep_transmit(dev, ep0_in);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 9);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USB_UAC_FU_VOLUME) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *) & (cdev->cur_volume), 2);
						ep0_in->xfer_len = 2U;
						usbd_ep_transmit(dev, ep0_in);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						usb_uac2_ctrl_range_layout2_struct response;
						response.wNumSubRanges = 1;
						response.wMIN = USBD_UAC_VOLUME_CTRL_MIN;
						response.wMAX = USBD_UAC_VOLUME_CTRL_MAX;
						response.wRES = 1;

						usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)&response, sizeof(response));
						ep0_in->xfer_len = sizeof(response);
						usbd_ep_transmit(dev, ep0_in);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 10);
						ret = HAL_ERR_PARA;
					}
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 11);
					ret = HAL_ERR_PARA;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT: //0x08 mic FU
				if (controlSelector == USB_UAC_FU_MUTE) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						ep0_in->xfer_buf[0] = cdev->cur_mute_in;
						ep0_in->xfer_len = 1U;
						usbd_ep_transmit(dev, ep0_in);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 20);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USB_UAC_FU_VOLUME) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						usb_os_memcpy((void *)ep0_in->xfer_buf, (void *) & (cdev->cur_volume_in), 2);
						ep0_in->xfer_len = 2U;
						usbd_ep_transmit(dev, ep0_in);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						usb_uac2_ctrl_range_layout2_struct response;
						response.wNumSubRanges = 1;
						response.wMIN = USBD_UAC_VOLUME_CTRL_MIN;
						response.wMAX = USBD_UAC_VOLUME_CTRL_MAX;
						response.wRES = 1;

						usb_os_memcpy((void *)ep0_in->xfer_buf, (void *)&response, sizeof(response));
						ep0_in->xfer_len = sizeof(response);
						usbd_ep_transmit(dev, ep0_in);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 21);
						ret = HAL_ERR_PARA;
					}
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 22);
					ret = HAL_ERR_PARA;
				}
				break;

			default:
				USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 12);
				ret = HAL_ERR_PARA;
				break;
			}
		} else {
			/* USB_H2D */
			switch (entityId) {
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE:
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES:
				if (controlSelector == USB_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						usb_os_memcpy((void *)&cdev->ctrl_req, (const void *)req, sizeof(usb_setup_req_t));
						ep0_out->xfer_len = req->wLength;
						usbd_ep_receive(dev, ep0_out);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						// Do nothing
					} else {
						/* Set freq err */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 13);
						ret = HAL_ERR_PARA;
					}
				} else {
					/* Set freq ctrl err*/
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 14);
					ret = HAL_ERR_PARA;
				}
				break;/* case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES */

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT: //0x05 FU
				if (controlSelector == USB_UAC_FU_MUTE) { //mute
					if (req->bRequest == USB_UAC_REQ_CUR) {
						usb_os_memcpy((void *)&cdev->ctrl_req, (const void *)req, sizeof(usb_setup_req_t));
						ep0_out->xfer_len = req->wLength;
						usbd_ep_receive(dev, ep0_out);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						// Do nothing
					} else {
						/* Set cur mute err */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 15);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USB_UAC_FU_VOLUME) { //volume
					if (req->bRequest == USB_UAC_REQ_CUR) {
						usb_os_memcpy((void *)&cdev->ctrl_req, (const void *)req, sizeof(usb_setup_req_t));
						ep0_out->xfer_len = req->wLength;
						usbd_ep_receive(dev, ep0_out);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						// Do nothing
					} else {
						/* Set cur volume range err */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 16);
						ret = HAL_ERR_HW;
					}
				} else {
					/* Set fu err */
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 17);
					ret = HAL_ERR_HW;
				}
				break;/* case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT */

			case USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT: //0x08 mic FU
				if (controlSelector == USB_UAC_FU_MUTE) { //mute
					if (req->bRequest == USB_UAC_REQ_CUR) {
						usb_os_memcpy((void *)&cdev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						ep0_out->xfer_len = req->wLength;
						usbd_ep_receive(dev, ep0_out);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						// Do nothing
					} else {
						/* Set cur mute err */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 23);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USB_UAC_FU_VOLUME) { //volume
					if (req->bRequest == USB_UAC_REQ_CUR) {
						usb_os_memcpy((void *)&cdev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						ep0_out->xfer_len = req->wLength;
						usbd_ep_receive(dev, ep0_out);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						// Do nothing
					} else {
						/* Set cur volume range err */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 24);
						ret = HAL_ERR_HW;
					}
				} else {
					/* Set fu err */
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 25);
					ret = HAL_ERR_HW;
				}
				break;/* case USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT */

			default:
				USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 18);
				ret = HAL_ERR_PARA;
				break;
			}
		}
		break;/* case USB_REQ_TYPE_CLASS */

	case USB_REQ_TYPE_VENDOR:
		if (req->wLength != 0) {
			if (((req->bmRequestType & 0x80U) != 0) && (cdev->cb->setup != NULL)) {
				ret = cdev->cb->setup(req, ep0_in->xfer_buf);
				if (ret == HAL_OK) {
					ep0_in->xfer_len = req->wLength;
					usbd_ep_transmit(dev, ep0_in);
				}
			} else {
				usb_os_memcpy((void *)&cdev->ctrl_req, (const void *)req, sizeof(usb_setup_req_t));
				ep0_out->xfer_len = req->wLength;
				usbd_ep_receive(dev, ep0_out);
			}
		} else {
			cdev->cb->setup(req, NULL);
		}
		break;

	default:
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 19);
		ret = HAL_ERR_HW;
		break;
	}

	return ret;
}

/**
  * @brief  Handles the SOF event for the UAC device
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @retval Status
  */
static int usbd_uac_handle_sof(usb_dev_t *dev)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	const usbd_uac_cb_t *cb = cdev->cb;
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_out);

	if ((pdata_ctrl->next_xfer != 0) && (pdata_ctrl->last_xfer_len != 0)) {
		pdata_ctrl->sof_idx ++;

		if ((pdata_ctrl->sof_idx > pdata_ctrl->data_idx) &&
			((pdata_ctrl->sof_idx - pdata_ctrl->data_idx) >= USBD_UAC_ISOC_RX_APPEND_ZLP_DIFF_MAX)) {
			pdata_ctrl->data_idx = pdata_ctrl->sof_idx;
			usbd_uac_append_data(pdata_ctrl);
		}
	}

	if ((cdev->uac_isoc_in.next_xfer != 0) && (cdev->ep_isoc_in.xfer_state == 0)) {
		usbd_uac_isoc_in_process_xfer(dev);
	}

	if (cb->sof != NULL) {
		cb->sof();
	}

	return HAL_OK;
}

/**
  * @brief  uac_handle_ep0_data_out
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  *         Handle EP0 Rx Ready event
  * @param  dev: USB device instance
  * @retval Status
  */
static int usbd_uac_handle_ep0_data_out(usb_dev_t *dev)
{
	int ret = HAL_OK;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	const usbd_uac_cb_t *cb = cdev->cb;
	usb_setup_req_t *p_ctrl_req = &(cdev->ctrl_req);
	usbd_ep_t *ep0_out = &dev->ep0_out;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	usb_ep_info_t *info;
	u32 freq;
	s16 volume_value = 0;
	u8 ch_cnt;
	u8 byte_width;
	u8 num_points;
	u8 target_volume;

	if ((((p_ctrl_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS) && ((p_ctrl_req->bmRequestType & 0x1FU) == USB_REQ_RECIPIENT_INTERFACE))
		&& (p_ctrl_req->bRequest == USB_UAC_REQ_CUR)) {
		if (USB_HIGH_BYTE(p_ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT) {
			if ((USB_HIGH_BYTE(p_ctrl_req->wValue) == USB_UAC_FU_MUTE) && (p_ctrl_req->wLength == 0x01)) {
				if (cb->mute_changed != NULL) {
					cdev->cur_mute = (ep0_out->xfer_buf[0]) ? 1 : 0;
					cb->mute_changed(cdev->cur_mute);
				}
			} else if ((USB_HIGH_BYTE(p_ctrl_req->wValue) == USB_UAC_FU_VOLUME) && (p_ctrl_req->wLength == 0x02)) {
				volume_value = (u16)ep0_out->xfer_buf[0] + ((u16)(ep0_out->xfer_buf[1]) << 8);

				num_points = sizeof(usbd_uac_pc_vol_lvl) / sizeof(usbd_uac_pc_vol_lvl[0]);
				if (num_points != sizeof(usbd_uac_drv_vol) / sizeof(usbd_uac_drv_vol[0])) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Volume arrays length err.\n");
					ret = HAL_ERR_PARA;
				} else {
					target_volume = usbd_uac_volume_linear_interpolation(usbd_uac_pc_vol_lvl, usbd_uac_drv_vol, num_points, (int)volume_value);
				}

				if ((target_volume != UABD_UAC_VOL_ERR_VAL) && (cb->volume_changed != NULL)) {
					cdev->cur_volume = volume_value;
					cb->volume_changed(target_volume);
				}
			}
		} else if (USB_HIGH_BYTE(p_ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT) {
			if ((USB_HIGH_BYTE(p_ctrl_req->wValue) == USB_UAC_FU_MUTE) && (p_ctrl_req->wLength == 0x01)) {
				cdev->cur_mute_in = (ep0_out->xfer_buf[0]) ? 1 : 0;
			} else if ((USB_HIGH_BYTE(p_ctrl_req->wValue) == USB_UAC_FU_VOLUME) && (p_ctrl_req->wLength == 0x02)) {
				cdev->cur_volume_in = (u16)ep0_out->xfer_buf[0] + ((u16)(ep0_out->xfer_buf[1]) << 8);
			}
		}

		if (((USB_HIGH_BYTE(p_ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES)
			 || (USB_HIGH_BYTE(p_ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE))
			&& (USB_HIGH_BYTE(p_ctrl_req->wValue) == USB_UAC_CS_SAM_FREQ_CONTROL) && (p_ctrl_req->wLength == 0x04)) {
			freq = (ep0_out->xfer_buf[3] << 24) | (ep0_out->xfer_buf[2] << 16) | (ep0_out->xfer_buf[1] << 8) | ep0_out->xfer_buf[0];
			if (usbd_uac_is_valid_sample_rate(freq, dev->dev_speed)) {
				if (cdev->cur_sampling_freq != freq) {
					cdev->cur_sampling_freq = freq;
					byte_width = cdev->cur_byte_width;
					ch_cnt = cdev->cur_ch_cnt;
					if (usbd_uac_ep_enable(&(cdev->cb->in)) != 0) {
						/* DeInit ISOC IN EP */
						usbd_ep_deinit(dev, ep_isoc_in);
						/* Init ISO IN EP */
						info = &ep_isoc_in->info;
						info->mps = (dev->dev_speed == USB_SPEED_HIGH) ?  USBD_UAC_CALC_HS_MPS(USBD_UAC_IN_DEFAULT_CH_CNT, USBD_UAC_IN_DEFAULT_BYTE_WIDTH, freq) \
									: USBD_UAC_CALC_FS_MPS(USBD_UAC_IN_DEFAULT_CH_CNT, USBD_UAC_IN_DEFAULT_BYTE_WIDTH, freq);
						usbd_ep_init(dev, ep_isoc_in);
					}

					if (usbd_uac_ep_enable(&(cdev->cb->out)) != 0) {
						/* DeInit ISOC OUT EP */
						usbd_ep_deinit(dev, ep_isoc_out);
						/* Init ISO OUT EP */
						info = &ep_isoc_out->info;
						info->mps = (dev->dev_speed == USB_SPEED_HIGH) ?  USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, freq) \
									: USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, freq);
						usbd_ep_init(dev, ep_isoc_out);
						/* usbd_ep_init leaves the OUT EP unarmed. If playback is active,
						 * re-arm the receive (mirrors usbd_uac_receive_data) so a mid-stream
						 * sample-rate change keeps ISOC OUT data flowing instead of NAKing. */
						if (cdev->uac_isoc_out.next_xfer != 0) {
							ep_isoc_out->xfer_buf = usbd_uac_rx_buf;
							ep_isoc_out->xfer_len = cdev->uac_isoc_out.isoc_mps;
							usbd_ep_receive(dev, ep_isoc_out);
						}
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
  * @brief  Pop one packet from the record TX ring buffer and transmit it on the ISOC IN endpoint.
  * @note   ISR context (SOF trigger / IN completion retrigger / usbd_uac_transmit_data kick).
  *         usb_ringbuf_remove_head() returns 0 when the ring buffer is empty, which naturally
  *         results in a ZLP transmit so the endpoint keeps being serviced every interval.
  * @param  dev: USB device instance
  * @retval void
  */
static void usbd_uac_isoc_in_process_xfer(usb_dev_t *dev)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_in);
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	u32 len;

	if (ep_isoc_in->xfer_state != 0U) {
		return;
	}

	len = usb_ringbuf_remove_head(&(pdata_ctrl->buf_list), usbd_uac_tx_buf, pdata_ctrl->isoc_mps, NULL);

	ep_isoc_in->xfer_state = 1U;
	ep_isoc_in->xfer_buf = usbd_uac_tx_buf;
	ep_isoc_in->xfer_len = (u16)len;
	usbd_ep_transmit(dev, ep_isoc_in);
}

/**
  * @brief  Handle completion of an ISOC IN transfer for the record engine, then retrigger
  *         if more data is queued.
  * @note   ISR context.
  * @param  dev: USB device instance
  * @retval void
  */
static void usbd_uac_isoc_in_process_complete(usb_dev_t *dev)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	cdev->ep_isoc_in.xfer_state = 0U;

	if (cdev->uac_isoc_in.next_xfer != 0) {
		usbd_uac_isoc_in_process_xfer(dev);
	}
}

/**
  * @brief  Data sent on non-control IN endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_uac_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	/* Return non-zero if EP is not owned by UAC2, so composite
	 * dispatcher can continue iterating to the correct sub-function. */
	if (ep_addr != cdev->ep_cfg->isoc_in_addr) {
		return HAL_ERR_PARA;
	}

	if (status != HAL_OK) {
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
	}

	usbd_uac_isoc_in_process_complete(dev);

	return HAL_OK;
}

/**
  * @brief  Data received on non-control Out endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
/**
  * @brief  Append a filler node to the OUT ring buffer when the host stops sending data.
  * @note   ISR context (invoked from SOF path).
  */
static void usbd_uac_append_data(usbd_uac_buf_ctrl_t *pdata_ctrl)
{
	usb_ringbuf_manager_t *buf_list = &(pdata_ctrl->buf_list);

	if (usb_ringbuf_is_full(buf_list)) {
		u8 tmp[1];
		usb_ringbuf_remove_head(buf_list, tmp, 1, NULL);
#if USBD_UAC_DEBUG
		pdata_ctrl->append_overwrite_cnt++;
#endif
	}

	if (usb_ringbuf_is_full(buf_list)) {
		return;
	}

#if USBD_UAC_DEBUG
	pdata_ctrl->append_zlp_cnt++;
#endif
	usb_ringbuf_add_tail(buf_list, NULL, pdata_ctrl->last_xfer_len, 0);

	if ((pdata_ctrl->read_wait_sema != 0) && (usb_ringbuf_is_empty(buf_list) == 0)) {
		rtos_sema_give(pdata_ctrl->uac_isoc_sema);
	}
}

static int usbd_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_out);
	usb_ringbuf_manager_t *buf_list = &(pdata_ctrl->buf_list);
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	int ret = HAL_OK;

	UNUSED(dev);

	/* Return non-zero if EP is not owned by UAC2, so composite
	 * dispatcher can continue iterating to the correct sub-function. */
	if (ep_addr != cdev->ep_cfg->isoc_out_addr) {
		return HAL_ERR_PARA;
	}

	if (pdata_ctrl->next_xfer != 0) {
		if (len == 0) { //ZLP
			ep_isoc_out->xfer_buf = usbd_uac_rx_buf;
			ep_isoc_out->xfer_len = pdata_ctrl->isoc_mps;
			ret = usbd_ep_receive(dev, ep_isoc_out);
		} else {
			pdata_ctrl->last_xfer_len = (u16)len;
			pdata_ctrl->xfer_cnt ++;
			pdata_ctrl->data_idx ++;

#if USBD_UAC_DEBUG
			u32 rx_new_tick = usb_os_get_timestamp_us();
			u32 step = (cdev->dev->dev_speed == USB_SPEED_HIGH) ? 185U : 1500U;
			if ((pdata_ctrl->last_xfer_tick > 0) && ((rx_new_tick - pdata_ctrl->last_xfer_tick) > step)) {
				pdata_ctrl->timeout_cnt++;
			}
			pdata_ctrl->last_xfer_tick = rx_new_tick;
#endif

			if (usb_ringbuf_is_full(buf_list)) {
				u8 tmp[1];
				usb_ringbuf_remove_head(buf_list, tmp, 1, NULL);
#if USBD_UAC_DEBUG
				pdata_ctrl->overwrite_cnt++;
#endif
			}

			usb_ringbuf_add_tail(buf_list, usbd_uac_rx_buf, len, 1);

			ep_isoc_out->xfer_buf = usbd_uac_rx_buf;
			ep_isoc_out->xfer_len = pdata_ctrl->isoc_mps;
			ret = usbd_ep_receive(dev, ep_isoc_out);

			if ((pdata_ctrl->read_wait_sema != 0) && (usb_ringbuf_is_empty(buf_list) == 0)) {
				rtos_sema_give(pdata_ctrl->uac_isoc_sema);
			}
		}
	} else {
		ep_isoc_out->xfer_buf = usbd_uac_rx_buf;
		ep_isoc_out->xfer_len = pdata_ctrl->isoc_mps;
		usbd_ep_receive(dev, ep_isoc_out);
	}

	return ret;
}
/**
  * @brief  Patch endpoint addresses in the configuration descriptor with actual addresses from ep_cfg
  * @param  desc: Pointer to the descriptor buffer (after configuration descriptor header)
  * @param  len: Total length of descriptor data
  * @param  ep_cfg: Endpoint configuration with actual addresses to patch in
  * @retval void
  */
static void usbd_uac_patch_ep_addresses(u8 *desc, u16 len, const usbd_uac_ep_cfg_t *ep_cfg)
{
	u16 i;

	for (i = 0; i < len;) {
		u8 dlen = desc[i];
		u8 dtype = desc[i + 1];
		if (dlen == 0) {
			break;
		}
		if ((dtype == USB_DESC_TYPE_ENDPOINT) && ((i + 3) <= len)) {
			u8 addr = desc[i + 2];
			u8 dir = addr & USB_REQ_DIR_MASK;
			u8 type = desc[i + 3] & 0x03;
			if ((dir == USB_D2H) && (type == USB_CH_EP_TYPE_ISOC)) {
				desc[i + 2] = ep_cfg->isoc_in_addr;
			} else if ((dir == USB_H2D) && (type == USB_CH_EP_TYPE_ISOC)) {
				desc[i + 2] = ep_cfg->isoc_out_addr;
			}
		}
		i += dlen;
	}
}

/**
  * @brief  Build the configuration descriptor to report to the host.
  *         When the mic (USB IN) is present the full duplex descriptor (playback + mic)
  *         is reported unchanged. Otherwise the mic AC entities and AS interface are
  *         stripped so the reported descriptor is OUT-only.
  * @note   keep_mic is set for composite mode (which always exposes the mic) and for
  *         standalone mode when the app enabled the IN endpoint (cb->in). A standalone
  *         playback-only build (IN disabled) still reports the pre-rework OUT-only baseline.
  * @param  buf: Output buffer
  * @param  desc: Source static config descriptor (full duplex, mic entities included)
  * @param  full_len: sizeof of desc
  * @param  keep_mic: whether to keep the mic entities/interface in the reported descriptor
  * @retval Actual descriptor length written to buf
  */
static u16 usbd_uac_build_config_desc(u8 *buf, const u8 *desc, u16 full_len, u8 keep_mic)
{
	u16 ac_mic_off;
	u16 ac_mic_end;
	u16 as_len;
	u16 ac_off;
	u16 ac_tot;

	if (keep_mic) {
		usb_os_memcpy((void *)buf, (void *)desc, full_len);
		return full_len;
	}

	ac_mic_off = USB_LEN_CFG_DESC + USB_LEN_IAD_DESC + USBD_UAC_AC_IF_LEN(USBD_UAC_DEFAULT_CH_CNT) - USBD_UAC_AC_MIC_IF_LEN;
	ac_mic_end = ac_mic_off + USBD_UAC_AC_MIC_IF_LEN;
	as_len = full_len - ac_mic_end - USBD_UAC_AS_TIF_LEN(USBD_UAC_MIC_AS_ALT_SETTING_NUM);

	usb_os_memcpy((void *)buf, (void *)desc, ac_mic_off);
	usb_os_memcpy((void *)(buf + ac_mic_off), (void *)(desc + ac_mic_end), as_len);

	buf[4]--;                    /* bNumInterfaces: drop the mic AS interface */
	buf[USB_LEN_CFG_DESC + 3]--; /* IAD bInterfaceCount: drop the mic AS interface */

	/* Shrink the CS AC-Header wTotalLength by the stripped mic AC entities. */
	ac_off = USB_LEN_CFG_DESC + USB_LEN_IAD_DESC + USB_LEN_IF_DESC + 6;
	ac_tot = (u16)(buf[ac_off] | (buf[ac_off + 1] << 8));
	ac_tot = (u16)(ac_tot - USBD_UAC_AC_MIC_IF_LEN);
	buf[ac_off] = USB_LOW_BYTE(ac_tot);
	buf[ac_off + 1] = USB_HIGH_BYTE(ac_tot);

	return (u16)(ac_mic_off + as_len);
}

/**
  * @brief  Get descriptor callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
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
	u8 attr = 0x80U;

	/* Keep the mic entities/interface in the reported descriptor for composite mode
	 * (always duplex) or when the standalone app enabled the IN endpoint. */
	u8 keep_mic = (cdev->from_composite != 0) ||
				  ((cdev->cb != NULL) && (usbd_uac_ep_enable(&(cdev->cb->in)) != 0));

	if (!cdev->from_composite) {
#ifdef CONFIG_USBD_SELF_POWERED
		attr |= USB_CFG_DESC_OFFSET_ATTR_BIT_SELF_POWERED;
#endif
#ifdef CONFIG_USBD_REMOTE_WAKEUP_EN
		attr |= USB_CFG_DESC_OFFSET_ATTR_BIT_REMOTE_WAKEUP;
#endif
	}

	switch (USB_HIGH_BYTE(req->wValue)) {

	case USB_DESC_TYPE_DEVICE:
		len = sizeof(usbd_uac_dev_desc);
		usb_os_memcpy((void *)buf, (const void *)usbd_uac_dev_desc, len);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		if (usbd_uac_ep_enable(&(cdev->cb->out)) != 0) {
			cdev->uac_isoc_out.isoc_mps = usbd_uac_get_mps(&(cdev->cb->out), speed);
		}
		if (usbd_uac_ep_enable(&(cdev->cb->in)) != 0) {
			cdev->uac_isoc_in.isoc_mps  = usbd_uac_get_mps(&(cdev->cb->in), speed);
		}

#ifndef CONFIG_USB_FS
		if (speed == USB_SPEED_HIGH) {
			len = sizeof(usbd_uac_hs_config_desc);
			desc = (u8 *)&usbd_uac_hs_config_desc;
		} else
#endif
		{
			len = sizeof(usbd_uac_fs_config_desc);
			desc = (u8 *)&usbd_uac_fs_config_desc;
		}

		len = usbd_uac_build_config_desc(buf, desc, len, keep_mic);

		if (!cdev->from_composite) {
			buf[USB_CFG_DESC_OFFSET_ATTR] = attr;
		}
		usbd_uac_patch_ep_addresses(buf + USB_LEN_CFG_DESC, len - USB_LEN_CFG_DESC, cdev->ep_cfg);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);
		break;

#ifndef CONFIG_USB_FS
	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		len = sizeof(usbd_uac_device_qualifier_desc);
		usb_os_memcpy((void *)buf, (const void *)usbd_uac_device_qualifier_desc, len);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			len = sizeof(usbd_uac_fs_config_desc);
			desc = (u8 *)&usbd_uac_fs_config_desc;
		} else {
			len = sizeof(usbd_uac_hs_config_desc);
			desc = (u8 *)&usbd_uac_hs_config_desc;
		}
		len = usbd_uac_build_config_desc(buf, desc, len, keep_mic);

		if (!cdev->from_composite) {
			buf[USB_CFG_DESC_OFFSET_ATTR] = attr;
		}
		usbd_uac_patch_ep_addresses(buf + USB_LEN_CFG_DESC, len - USB_LEN_CFG_DESC, cdev->ep_cfg);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TYPE] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
		break;
#endif

	case USB_DESC_TYPE_STRING:
		switch (USB_LOW_BYTE(req->wValue)) {
		case USBD_IDX_LANGID_STR:
			len = sizeof(usbd_uac_lang_id_desc);
			usb_os_memcpy((void *)buf, (const void *)usbd_uac_lang_id_desc, len);
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
			USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_GET_DESC, 0);
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
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  old_status: USB old attach status
  * @param  status: USB attach status
  * @retval void
  */
static void usbd_uac_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	UNUSED(dev);

	if (status == USBD_ATTACH_STATUS_DETACHED) {
		/* Detach: stop tx/rx and wake up the read thread blocked in usbd_uac_read.
		   ISR context here: only rtos_sema_give is allowed (no malloc/sema_take). */
		usbd_uac_stop_play();
		usbd_uac_stop_record();
		if ((cdev->uac_isoc_out.uac_sema_valid != 0) && (cdev->uac_isoc_out.read_wait_sema != 0)) {
			rtos_sema_give(cdev->uac_isoc_out.uac_isoc_sema);
		}
	}

	if (cdev->cb->status_changed != NULL) {
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
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *buf_ctrl = &(cdev->uac_isoc_out);
	u32 loop_idx = 0;
	u32 max_loop = USBD_UAC_DEBUG_LOOP_TIME / USBD_UAC_DEBUG_STEP_TIME;

	UNUSED(param);

	cdev->isoc_dump_thread = 1;
	while (cdev->isoc_dump_thread != 0) {
		loop_idx = 0;
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB Dump RX %d/TO %d/OW %d-%d/zp %d/pkt %d/copylen %d/%d\n",
				 buf_ctrl->xfer_cnt,
				 buf_ctrl->timeout_cnt,
				 buf_ctrl->overwrite_cnt,
				 buf_ctrl->append_overwrite_cnt,
				 buf_ctrl->append_zlp_cnt,
				 usb_ringbuf_get_count(&(buf_ctrl->buf_list)),
				 cdev->copy_data_len,
				 buf_ctrl->next_xfer
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
static u32 usbd_uac_read_ring_buf(usbd_uac_buf_ctrl_t *pdata_ctrl, u8 *buffer, u32 size,
								  u32 *copy_len, u16 *pkt_cnt, u32 *zero_pkt_flag)
{
	usb_ringbuf_manager_t *buf_list = &(pdata_ctrl->buf_list);
	u32 read_len;
	u8 valid = 0;

	do {
		/* Exit: 1) enough data obtained; 2) next packet cannot fit */
		if ((*copy_len >= size) || ((*copy_len + pdata_ctrl->isoc_mps) > size)) {
			return HAL_OK;
		}

		read_len = usb_ringbuf_remove_head(buf_list, buffer + *copy_len, size - *copy_len, &valid);
		if (read_len > 0) {
			*copy_len += read_len;
			/* zero_pkt_flag is a 32-bit bitmap; cap at 32 pkts (shift >= 32 is UB). */
			if ((valid == 0) && (zero_pkt_flag != NULL) && (*pkt_cnt < 32U)) {
				*zero_pkt_flag |= (1U << (*pkt_cnt));
			}
			(*pkt_cnt)++;
		}
	} while (usb_ringbuf_is_empty(buf_list) == 0);

	if (*copy_len >= size) {
		return HAL_OK;
	}

	return HAL_ERR_HW;
}

/**
  * @brief  Initialize UAC device
  * @param  cb: Callback functions for audio events
  * @retval Status
  */
static int usbd_uac_private_init(const usbd_uac_cb_t *cb, const usbd_uac_ep_cfg_t *ep_cfg)
{
	int ret = HAL_OK;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	usb_ep_info_t *info;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return HAL_ERR_PARA;
	}

	if (ep_cfg == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid ep cfg\n");
		return HAL_ERR_PARA;
	}

	cdev->ep_cfg = ep_cfg;
	cdev->cur_volume = 0x001F;
	cdev->cur_mute = 0;
	cdev->cur_clk_valid = 1;
	/* Non-zero default rate: the first SET_INTERFACE(alt=1) fires format_changed
	 * with all three fields set even if the host skips SET_CUR SAMPLING_FREQ
	 * (single discrete rate), which would otherwise leave playback silent. */
	cdev->cur_sampling_freq = USBD_UAC_SAMPLING_FREQ_48K;

	usbd_uac_ep_buf_ctrl_deinit(&(cdev->uac_isoc_in));
	usbd_uac_ep_buf_ctrl_deinit(&(cdev->uac_isoc_out));

	info = &ep_isoc_out->info;
	info->addr = cdev->ep_cfg->isoc_out_addr;
	info->type = USB_CH_EP_TYPE_ISOC;
	info->binterval = (cdev->dev != NULL && cdev->dev->dev_speed == USB_SPEED_HIGH)
					  ? USBD_UAC_HS_DEFAULT_BINTERVAL : USBD_UAC_FS_DEFAULT_BINTERVAL;
	info = &ep_isoc_in->info;
	info->addr = cdev->ep_cfg->isoc_in_addr;
	info->type = USB_CH_EP_TYPE_ISOC;
	info->binterval = (cdev->dev != NULL && cdev->dev->dev_speed == USB_SPEED_HIGH)
					  ? USBD_UAC_HS_DEFAULT_BINTERVAL : USBD_UAC_FS_DEFAULT_BINTERVAL;

	cdev->cb = cb;
	if ((cb->in.enable == 0) && (cb->out.enable == 0)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Pls cfg UAC EP\n");
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


#if USBD_UAC_DEBUG
	rtos_task_t task_dump;
	if (rtos_task_create(&task_dump, ((const char *)"usbd_uac_status_dump_thread"), usbd_uac_status_dump_thread, NULL, 1024U, 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create usb status dump task fail\n");
	}
#endif

	return ret;
}

/* Exported functions --------------------------------------------------------*/

int usbd_uac_init(const usbd_uac_cb_t *cb, const usbd_uac_ep_cfg_t *ep_cfg)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	int ret;

	ret = usbd_uac_private_init(cb, ep_cfg);
	if (ret == HAL_OK) {
		cdev->from_composite = 0;
		usbd_register_class(&usbd_uac_driver);
	}
	return ret;
}

#ifdef CONFIG_USBD_COMPOSITE
int usbd_composite_uac_init(const usbd_uac_cb_t *cb, const usbd_uac_ep_cfg_t *ep_cfg)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	int ret;

	ret = usbd_uac_private_init(cb, ep_cfg);
	if (ret == HAL_OK) {
		cdev->from_composite = 1;
		ret = usbd_composite_register_driver(&usbd_uac_driver);
	}
	return ret;
}
#endif

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

#ifdef CONFIG_USBD_COMPOSITE
	if (cdev->from_composite) {
		usbd_composite_unregister_driver(&usbd_uac_driver);
	} else
#endif
	{
		usbd_unregister_class();
	}

	usbd_uac_ep_buf_ctrl_deinit(&(cdev->uac_isoc_in));
	usbd_uac_ep_buf_ctrl_deinit(&(cdev->uac_isoc_out));

	if (cdev->cb != NULL) {
		if (cdev->cb->deinit != NULL) {
			cdev->cb->deinit();
		}
		cdev->cb = NULL;
	}

	return HAL_OK;
}

/**
  * @brief  Transmit UAC data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbd_uac_transmit_data(u8 *buf, u32 len)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usb_dev_t *dev = cdev->dev;
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_in);

	if (dev == NULL || dev->is_ready == 0 || pdata_ctrl->next_xfer == 0) {
		return HAL_OK;
	}

	if (len > pdata_ctrl->isoc_mps) {
		len = pdata_ctrl->isoc_mps;
	}

	if (usb_ringbuf_is_full(&(pdata_ctrl->buf_list))) {
		u8 tmp[1];
		usb_ringbuf_remove_head(&(pdata_ctrl->buf_list), tmp, 1, NULL);
	}

	usb_ringbuf_add_tail(&(pdata_ctrl->buf_list), buf, len, 1);

	if (cdev->ep_isoc_in.xfer_state == 0) {
		usbd_uac_isoc_in_process_xfer(dev);
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
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;

	if (dev == NULL || !dev->is_ready) {
		return HAL_OK;
	}

	if (pbuf_ctrl->isoc_mps == 0) {
		return HAL_ERR_HW;
	}

	if (usbd_uac_ep_enable(&(cdev->cb->out)) != 0) {
		usb_ringbuf_reset(&(pbuf_ctrl->buf_list));
		pbuf_ctrl->sof_idx = 0;
		pbuf_ctrl->data_idx = 0;
		pbuf_ctrl->xfer_cnt = 0;
		pbuf_ctrl->next_xfer = 1;
		/* Arm the ISOC OUT EP here: some hosts skip SET_CUR SAMPLING_FREQ (the only
		 * other ep_init path) when one discrete rate applies. deinit+init is idempotent. */
		ep_isoc_out->info.mps = pbuf_ctrl->isoc_mps;
		usbd_ep_deinit(dev, ep_isoc_out);
		usbd_ep_init(dev, ep_isoc_out);
		ep_isoc_out->xfer_buf = usbd_uac_rx_buf;
		ep_isoc_out->xfer_len = pbuf_ctrl->isoc_mps;
		return usbd_ep_receive(dev, ep_isoc_out);
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
int usbd_uac_config(const usbd_audio_cfg_t *uac_cfg, u8 is_record, u32 flag)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *pbuf_ctrl;
	int ret = HAL_OK;

	UNUSED(flag);

	/* all the transfer should finish */
	/* TODO: re initiation the isoc buffer[usbd_uac_buf_ctrl_t] */
	if ((uac_cfg != NULL) && usbd_uac_ep_enable(uac_cfg) != 0) {
		if (is_record != 0) { //
			pbuf_ctrl = &(cdev->uac_isoc_in);
		} else { //
			pbuf_ctrl = &(cdev->uac_isoc_out);
		}

		pbuf_ctrl->next_xfer = 0;

		usbd_uac_ep_buf_ctrl_deinit(pbuf_ctrl);
		ret = usbd_uac_ep_buf_ctrl_init(pbuf_ctrl, (usbd_audio_cfg_t *)uac_cfg, cdev->dev->dev_speed);
	}

	return ret;
}

/**
  * @brief  Initializes and starts the UAC data rx process
  * @param  void
  * @retval Status
  */
int usbd_uac_start_play(void)
{
	int ret;
#if USBD_UAC_DEBUG
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC play start\n");
#endif

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
#if USBD_UAC_DEBUG
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC play stop\n");
#endif
	cdev->uac_isoc_out.next_xfer = 0;
}

/**
  * @brief  Initializes and starts the UAC data record (microphone) process
  * @param  void
  * @retval Status
  */
int usbd_uac_start_record(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *pbuf_ctrl = &(cdev->uac_isoc_in);
#if USBD_UAC_DEBUG
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC record start\n");
#endif

	if (usbd_uac_ep_enable(&(cdev->cb->in)) == 0) {
		return HAL_ERR_HW;
	}

	usb_ringbuf_reset(&(pbuf_ctrl->buf_list));
	pbuf_ctrl->sof_idx = 0;
	pbuf_ctrl->data_idx = 0;
	pbuf_ctrl->xfer_cnt = 0;
	pbuf_ctrl->next_xfer = 1;

	return HAL_OK;
}

/**
  * @brief  Stop UAC device record (microphone)
  * @param  void
  * @retval void
  */
void usbd_uac_stop_record(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
#if USBD_UAC_DEBUG
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC record stop\n");
#endif
	cdev->uac_isoc_in.next_xfer = 0;
}

/**
  * @brief  Read UAC data
  * @param  buf: Pointer to data buffer
  * @param  size: Data length
  * @param  time_out_ms: Time out
  * @retval copy len
  */
u32 usbd_uac_read(u8 *buffer, u32 size, u32 time_out_ms, u32 *zero_pkt_flag)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_out);
	usb_dev_t *dev = cdev->dev;
	u32 copy_len = 0;
	u16 pkt_cnt = 0;

	if (dev == NULL || dev->is_ready == 0) {
		return copy_len;
	}

	if (pdata_ctrl->next_xfer == 0) {
		return copy_len;
	}

	if (time_out_ms == 0) {
		if (usb_ringbuf_is_empty(&(pdata_ctrl->buf_list))) {
			return copy_len;
		}

		usbd_uac_read_ring_buf(pdata_ctrl, buffer, size, &copy_len, &pkt_cnt, zero_pkt_flag);
	} else {
		do {
			if (usb_ringbuf_is_empty(&(pdata_ctrl->buf_list))) {
				//wait sema
				pdata_ctrl->read_wait_sema = 1;
				if (pdata_ctrl->uac_sema_valid == 0) {
					pdata_ctrl->read_wait_sema = 0;
					break;
				}
				if (rtos_sema_take(pdata_ctrl->uac_isoc_sema, time_out_ms) != RTK_SUCCESS) {
					pdata_ctrl->read_wait_sema = 0;
					break;
				}
				pdata_ctrl->read_wait_sema = 0;
			} else {
				/* if did not read any pkt, loop to check the wr/rd pos*/
				if ((usbd_uac_read_ring_buf(pdata_ctrl, buffer, size, &copy_len, &pkt_cnt, zero_pkt_flag) == HAL_OK)
					|| (copy_len > 0)) {
					break;
				}
			}
		} while (pdata_ctrl->next_xfer != 0);
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

	return usb_ringbuf_get_count(&(pdata_ctrl->buf_list));
}

/**
  * @brief  Return the current OUT ring buffer occupancy expressed in microseconds
  *         of audio at the negotiated bInterval. Callers use this to pace playback
  *         and detect underruns.
  */
u32 usbd_uac_get_read_frame_time_in_us(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_out);
	u8 binterval = pdata_ctrl->binterval;
	u32 base_us;

	if (binterval == 0U) {
		binterval = (cdev->dev != NULL && cdev->dev->dev_speed == USB_SPEED_HIGH)
					? USBD_UAC_HS_DEFAULT_BINTERVAL : USBD_UAC_FS_DEFAULT_BINTERVAL;
	}

	if (cdev->dev != NULL && cdev->dev->dev_speed == USB_SPEED_HIGH) {
		/* HS: 8 microframes per ms, each 125us */
		base_us = USBD_UAC_ONE_KHZ / USBD_UAC_HS_SOF_COUNT_PER_MS;
	} else {
		base_us = USBD_UAC_ONE_KHZ;
	}

	return (u32)usb_ringbuf_get_count(&(pdata_ctrl->buf_list)) * base_us * USBD_UAC_POW2(binterval - 1U);
}
