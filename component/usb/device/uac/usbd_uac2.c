/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

/* uac 2.0 */
#include "usbd_uac_priv.h"
#ifdef CONFIG_USBD_COMPOSITE
#include "usbd_composite.h"
#endif
#include "usb_uac2.h"

/* Private defines -----------------------------------------------------------*/

#define USBD_UAC_DESCRIPTOR_DUMP (0)
#define USBD_UAC_FS_DEFAULT_BINTERVAL               1U      /**< FS ISOC bInterval used in EP descriptors */
#define USBD_UAC_HS_DEFAULT_BINTERVAL               1U      /**< HS ISOC bInterval used in EP descriptors */
/* 1ms 8 frame in high speed */
#define USBD_UAC_HS_SOF_COUNT_PER_MS                8U
/**
 * Audio sample frequency
 */
#define USBD_UAC_SAMPLING_FREQ_MAX_COUNT             4U
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
#ifdef CONFIG_USBD_UAC_RECORD
/* Interfaces: audio control, headphone and mic audio streaming */
#define USBD_UAC_INTERFACE_COUNT                    3U
/* AC mic entities (clock + input terminal + feature unit + output terminal) descriptor length */
#define USBD_UAC_AC_MIC_IF_LEN \
    (USB_UAC2_LEN_CLK_SRC_DESC + USB_UAC2_LEN_AC_IN_TTY_DESC + \
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_RECORD_CHANNEL_COUNT) + USB_UAC2_LEN_AC_OUT_TTY_DESC)
/* microphone AS alt setting num (alt0 zero-bw + alt1 streaming) */
#define USBD_UAC_MIC_AS_ALT_SETTING_NUM             2U
/* len of the mic AS interface */
#define USBD_UAC_AS_MIC_TIF_LEN                     USBD_UAC_AS_TIF_LEN(USBD_UAC_MIC_AS_ALT_SETTING_NUM)
#else
/* Interfaces: audio control and headphone audio streaming */
#define USBD_UAC_INTERFACE_COUNT                    2U
#define USBD_UAC_AC_MIC_IF_LEN                      0U
#define USBD_UAC_AS_MIC_TIF_LEN                     0U
#endif
/* len of each Audio stream interface/altsetting (one EP) */
#define USBD_UAC_AS_EIF_LEN                        (USB_LEN_IF_DESC + USB_UAC2_LEN_AS_IF_ALT_SET_DESC + USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC \
                                                   + USB_LEN_EP_DESC + USB_UAC2_LEN_DATA_EP_DESC)
#ifdef CONFIG_USBD_UAC_PLAY_FEEDBACK
/* Ref UAC 2.0 4.10.1.1 and USB 2.0 5.12.4.2: an asynchronous data EP with an explicit feedback EP */
#define USBD_UAC_PLAY_AS_EP_COUNT                   2U
#define USBD_UAC_PLAY_EP_ATTR                       0x05U   /* Isochronous, asynchronous, data */
#define USBD_UAC_PLAY_FEEDBACK_EP_ATTR              0x11U   /* Isochronous, no synchronization, feedback */
#define USBD_UAC_PLAY_FEEDBACK_FS_MPS               3U      /* 10.14 frames per frame at Full Speed */
#define USBD_UAC_PLAY_FEEDBACK_HS_MPS               4U      /* 16.16 frames per microframe at High Speed */
/* One feedback per ms: 2^(4-1) microframes at High Speed */
#define USBD_UAC_PLAY_FEEDBACK_HS_BINTERVAL         4U
#define USBD_UAC_PLAY_FEEDBACK_EP_LEN               USB_LEN_EP_DESC
#else
#define USBD_UAC_PLAY_AS_EP_COUNT                   1U
#define USBD_UAC_PLAY_EP_ATTR                       0x09U   /* Isochronous, adaptive, data */
#define USBD_UAC_PLAY_FEEDBACK_EP_LEN               0U
#endif
/* len of each headphone Audio stream altsetting: the data EP, plus the feedback EP */
#define USBD_UAC_AS_PLAY_EIF_LEN                    (USBD_UAC_AS_EIF_LEN + USBD_UAC_PLAY_FEEDBACK_EP_LEN)
/* headphone AS alt setting num: alt 0 plus one alt per channel count from 2 up to the max */
#define USBD_UAC_AS_ALT_SETTING_NUM                 ((USBD_UAC_PLAY_MAX_CHANNEL_COUNT / 2U) + 1U)

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* bit_width */
#define USBD_UAC_BIT_WIDTH(byte_width)              (8U * (byte_width))

/* AC feature uint descriptor length */
#define USBD_UAC_AC_FU_HEAD_DESC_LEN(channel_count)        (USB_UAC2_LEN_FUNC_UNIT_DESC + 1 + 4 * (channel_count))
/* AC IF header descriptor length */
#define USBD_UAC_AC_IF_HEAD_DESC_LEN(channel_count)         \
    (USB_UAC2_LEN_CTRL_IF_HEADER + USB_UAC2_LEN_CLK_SRC_DESC + USB_UAC2_LEN_AC_IN_TTY_DESC + \
	USBD_UAC_AC_FU_HEAD_DESC_LEN(channel_count) + USB_UAC2_LEN_AC_OUT_TTY_DESC + USBD_UAC_AC_MIC_IF_LEN)

/* len of total Audio control interface */
#define USBD_UAC_AC_IF_LEN(channel_count)                 (USB_LEN_IF_DESC + USBD_UAC_AC_IF_HEAD_DESC_LEN(channel_count))

/* len of total Audio stream interface */
#define USBD_UAC_AS_TIF_LEN(alt_num)               (USBD_UAC_AS_EIF_LEN * (alt_num - 1) + USB_LEN_IF_DESC)
/* len of the headphone Audio stream interface */
#define USBD_UAC_AS_PLAY_TIF_LEN(alt_num)          (USBD_UAC_AS_PLAY_EIF_LEN * (alt_num - 1) + USB_LEN_IF_DESC)

/* Standard AS isochronous feedback endpoint descriptor of a headphone alt setting, ref UAC 2.0 4.10.2.1 */
#ifdef CONFIG_USBD_UAC_PLAY_FEEDBACK
#define USBD_UAC_PLAY_FEEDBACK_EP_DESC \
	USB_LEN_EP_DESC,                            /* bLength */ \
	USB_DESC_TYPE_ENDPOINT,                     /* bDescriptorType */ \
	USB_D2H,                                    /* bEndpointAddress: dir IN (placeholder) */ \
	USBD_UAC_PLAY_FEEDBACK_EP_ATTR,             /* bmAttributes */ \
	USBD_UAC_PLAY_FEEDBACK_HS_MPS,              /* wMaxPacketSize: patched for the speed */ \
	0x00, \
	USBD_UAC_PLAY_FEEDBACK_HS_BINTERVAL,        /* bInterval: patched for the speed */
#else
#define USBD_UAC_PLAY_FEEDBACK_EP_DESC
#endif

/**
 * High Speed
 */
/* calculate high speed MPS */
#define USBD_UAC_CALC_HS_MPS(channel_count, byte_width, sampling_freq_hz) \
  ((channel_count) * (byte_width) * ((sampling_freq_hz) / USBD_UAC_ONE_KHZ / USBD_UAC_HS_SOF_COUNT_PER_MS + 1U))
/* check MPS */
#define USBD_UAC_IS_HS_MPS_VALID(channel_count, byte_width, sampling_freq_hz) \
    ((USBD_UAC_CALC_HS_MPS(channel_count, byte_width, sampling_freq_hz)) <= USBD_UAC_HS_ISOC_MPS)
/* Ref UAC 2.0 5.2.5.1: the headphone clock RANGE is shared by every alt setting, so a rate is
   declared at High Speed only if the widest alt setting carries it, refer to the checks below */
#define USBD_UAC_HS_RATE_DECLARED(freq) \
    ((((freq) <= USBD_UAC_PLAY_MAX_SAMPLING_FREQ) && USBD_UAC_IS_HS_MPS_VALID(USBD_UAC_PLAY_MAX_CHANNEL_COUNT, USBD_UAC_PLAY_BYTE_WIDTH, freq)) ? 1U : 0U)
/* number of rates of usbd_uac_sampling_rates[] declared at High Speed */
#define USBD_UAC_HS_SAMPLING_FREQ_COUNT \
    (USBD_UAC_HS_RATE_DECLARED(USBD_UAC_SAMPLING_FREQ_44K) + USBD_UAC_HS_RATE_DECLARED(USBD_UAC_SAMPLING_FREQ_48K) + \
     USBD_UAC_HS_RATE_DECLARED(USBD_UAC_SAMPLING_FREQ_96K) + USBD_UAC_HS_RATE_DECLARED(USBD_UAC_SAMPLING_FREQ_192K))
/* highest rate declared at High Speed */
#define USBD_UAC_HS_MAX_SAMPLING_FREQ \
    ((USBD_UAC_HS_SAMPLING_FREQ_COUNT == 4U) ? USBD_UAC_SAMPLING_FREQ_192K : \
     ((USBD_UAC_HS_SAMPLING_FREQ_COUNT == 3U) ? USBD_UAC_SAMPLING_FREQ_96K : USBD_UAC_SAMPLING_FREQ_48K))
#ifdef CONFIG_USBD_UAC_RECORD
/* High-Speed record packet: one microframe of audio plus one frame */
#define USBD_UAC_RECORD_HS_MPS \
    USBD_UAC_CALC_HS_MPS(USBD_UAC_RECORD_CHANNEL_COUNT, USBD_UAC_RECORD_BYTE_WIDTH, USBD_UAC_RECORD_SAMPLING_FREQ)
#endif
/* get high speed MPS of an alt setting, sized for the highest rate declared at High Speed */
#define USBD_UAC_GET_HS_MPS(channel_count, byte_width)     USBD_UAC_CALC_HS_MPS(channel_count, byte_width, USBD_UAC_HS_MAX_SAMPLING_FREQ)

/*
*	Full Speed
*/
/* calculate full speed MPS */
#define USBD_UAC_CALC_FS_MPS(channel_count, byte_width, sampling_freq_hz) \
    ((channel_count) * (byte_width) * ((sampling_freq_hz) / USBD_UAC_ONE_KHZ + 1U))
/* check MPS */
#define USBD_UAC_IS_FS_MPS_VALID(channel_count, byte_width, sampling_freq_hz) \
    ((USBD_UAC_CALC_FS_MPS(channel_count, byte_width, sampling_freq_hz)) <= USBD_UAC_FS_ISOC_MPS)
/* A rate is declared at Full Speed only if the widest alt setting carries it, as for High Speed */
#define USBD_UAC_FS_RATE_DECLARED(freq) \
    ((((freq) <= USBD_UAC_PLAY_MAX_SAMPLING_FREQ) && USBD_UAC_IS_FS_MPS_VALID(USBD_UAC_PLAY_MAX_CHANNEL_COUNT, USBD_UAC_PLAY_BYTE_WIDTH, freq)) ? 1U : 0U)
/* number of rates of usbd_uac_sampling_rates[] declared at Full Speed */
#define USBD_UAC_FS_SAMPLING_FREQ_COUNT \
    (USBD_UAC_FS_RATE_DECLARED(USBD_UAC_SAMPLING_FREQ_44K) + USBD_UAC_FS_RATE_DECLARED(USBD_UAC_SAMPLING_FREQ_48K) + \
     USBD_UAC_FS_RATE_DECLARED(USBD_UAC_SAMPLING_FREQ_96K) + USBD_UAC_FS_RATE_DECLARED(USBD_UAC_SAMPLING_FREQ_192K))
/* highest rate declared at Full Speed */
#define USBD_UAC_FS_MAX_SAMPLING_FREQ \
    ((USBD_UAC_FS_SAMPLING_FREQ_COUNT == 4U) ? USBD_UAC_SAMPLING_FREQ_192K : \
     ((USBD_UAC_FS_SAMPLING_FREQ_COUNT == 3U) ? USBD_UAC_SAMPLING_FREQ_96K : USBD_UAC_SAMPLING_FREQ_48K))
/* get full speed MPS of an alt setting, sized for the highest rate declared at Full Speed */
#define USBD_UAC_GET_FS_MPS(channel_count, byte_width)     USBD_UAC_CALC_FS_MPS(channel_count, byte_width, USBD_UAC_FS_MAX_SAMPLING_FREQ)

/* len of total configuration descriptor buf, the same at both speeds */
#define USBD_UAC_CFG_DESC_BUF_LEN(channel_count) \
    (USB_LEN_CFG_DESC + USB_LEN_IAD_DESC  + USBD_UAC_AC_IF_LEN(channel_count) + USBD_UAC_AS_PLAY_TIF_LEN(USBD_UAC_AS_ALT_SETTING_NUM) \
	+ USBD_UAC_AS_MIC_TIF_LEN)

/* Build-time checks of the descriptor templates compiled into the image. The connection speed
   is only known at run time, e.g. an HS device behind an FS hub uses the FS template, so every
   template must be valid on its own. Ref USB 2.0 5.6.3: ISOC packet <= 1023 bytes at Full Speed,
   <= 1024 bytes per microframe at High Speed. */
#if !USBD_UAC_IS_FS_MPS_VALID(USBD_UAC_PLAY_MAX_CHANNEL_COUNT, USBD_UAC_PLAY_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K)
#error "UAC2: channels x bytes per sample shall be <= 20 to fit FS 48kHz (1023B). Lower 'Playback max channels' or 'Playback bit width'"
#endif
#if defined(CONFIG_SUPPORT_USB_FS_ONLY) && !USBD_UAC_IS_FS_MPS_VALID(USBD_UAC_PLAY_MAX_CHANNEL_COUNT, USBD_UAC_PLAY_BYTE_WIDTH, USBD_UAC_PLAY_MAX_SAMPLING_FREQ)
#if USBD_UAC_IS_FS_MPS_VALID(USBD_UAC_PLAY_MAX_CHANNEL_COUNT, USBD_UAC_PLAY_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_96K)
#error "UAC2: FS-only SoC, 'Playback max sampling rate' exceeds FS ISOC 1023B. Set it to 96 kHz, or lower channels or bit width"
#else
#error "UAC2: FS-only SoC, 'Playback max sampling rate' exceeds FS ISOC 1023B. Set it to 48 kHz, or lower channels or bit width"
#endif
#endif
#if !defined(CONFIG_SUPPORT_USB_FS_ONLY) && !USBD_UAC_IS_HS_MPS_VALID(USBD_UAC_PLAY_MAX_CHANNEL_COUNT, USBD_UAC_PLAY_BYTE_WIDTH, USBD_UAC_PLAY_MAX_SAMPLING_FREQ)
#error "UAC2: 'Playback max sampling rate' exceeds HS ISOC 1024B. Lower it, or lower 'Playback max channels' or 'Playback bit width'"
#endif

#define USBD_UAC_INIT_SUB_RANGE(sub_range, min_sampling_freq, max_sampling_freq, res) \
    do {                                               \
        (sub_range).dMIN = (min_sampling_freq);        \
        (sub_range).dMAX = (max_sampling_freq);        \
        (sub_range).dRES = (res);                      \
    } while(0)

/* Private function prototypes -----------------------------------------------*/

static int usbd_uac_set_config(usb_dev_t *dev, u8 config);
static void usbd_uac_clear_config(usb_dev_t *dev, u8 config);
static int usbd_uac_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len);
static void usbd_uac_handle_sof(usb_dev_t *dev);
static int usbd_uac_handle_ep0_data_out(usb_dev_t *dev);
#if USBD_UAC_DESCRIPTOR_DUMP
static int usbd_uac_dump_descriptor(const u8 *buf, int len);
#endif
#if USBD_UAC_DEBUG
#endif
static int usbd_uac_clock_valid_request(usb_dev_t *dev, u8 flag);
static u8 usbd_uac_is_owned_interface(u16 interface_number);
#ifndef CONFIG_USBD_UAC_RECORD
static u8 usbd_uac_is_mic_entity(u8 entity_id);
#endif
static int usbd_uac_current_sampling_freq_request(usb_dev_t *dev, u32 frequency);
static int usbd_uac_sampling_freq_ctrl_range_request(usb_dev_t *dev, u16 max_len);
#ifdef CONFIG_USBD_UAC_RECORD
static int usbd_uac_mic_sampling_freq_range_request(usb_dev_t *dev, u16 max_len);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "UAC";

/* Ascending: the first USBD_UAC_xS_SAMPLING_FREQ_COUNT entries are the rates declared at that speed */
static const u32 usbd_uac_sampling_rates[USBD_UAC_SAMPLING_FREQ_MAX_COUNT] = {
	USBD_UAC_SAMPLING_FREQ_44K, USBD_UAC_SAMPLING_FREQ_48K, USBD_UAC_SAMPLING_FREQ_96K, USBD_UAC_SAMPLING_FREQ_192K
};

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
	USB_LOW_BYTE(USB_VID),  /* idUAC */
	USB_HIGH_BYTE(USB_VID),
	USB_LOW_BYTE(USB_PID),  /* idProduct */
	USB_HIGH_BYTE(USB_PID),
	0x00,                        /* bcdDevice */
	0x02,
	USBD_IDX_MFC_STR,            /* iManufacturer */
	USBD_IDX_PRODUCT_STR,        /* iProduct */
	USBD_IDX_SERIAL_STR,         /* iSerialNumber */
	0x01                         /* bNumConfigurations */
}; /* usbd_uac_dev_desc */

#ifndef CONFIG_SUPPORT_USB_FS_ONLY
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
#endif

/* USB UAC Device Configuration Descriptor, the ISOC wMaxPacketSize is patched for the speed it
   describes, refer to usbd_uac_patch_descriptor() */
static const u8 usbd_uac_config_desc[] = {
	USB_LEN_CFG_DESC,                                        /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                             /* bDescriptorType */
	0x00,                                                    /* wTotalLength: patched at runtime */
	0x00,
	USBD_UAC_INTERFACE_COUNT,                                /* Number of interfaces supported by this configuration */
	0x01,                                                    /* Value to use as an argument to the SetConfiguration() request to select this configuration */
	0x00,                                                    /* Index of string descriptor describing this configuration */
	0x80,                                                    /* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
	0x32,                                                    /* bMaxPower */

	/* 4.6 Interface Association Descriptor */
	/* IAD Descriptor */
	USB_LEN_IAD_DESC,                  /* Size of this descriptor (byte_width) */
	USB_UAC_IAD_DESCRIPTOR_TYPE,      /* Interface Association Descriptor type */
	0x00,                              /* First Index: Audio Control Interface Index - Headset (0) */
	USBD_UAC_INTERFACE_COUNT,          /* Audio Control Interface, Audio Streaming Interfaces () */
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
	USB_LOW_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_PLAY_MAX_CHANNEL_COUNT)),
	USB_HIGH_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_PLAY_MAX_CHANNEL_COUNT)), /* Total number of byte_width returned for the class-specific AudioControl interface descriptor. Includes
                                                                           the combined length of this descriptor header and all Unit and Terminal descriptors.   */
	0x00,                                                                 /* D1..0: Latency Control  */

	/* Audio Control Clock Source Unit Descriptor 2.0 */
	USB_UAC2_LEN_CLK_SRC_DESC,                         /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,             /* CS_INTERFACE Descriptor Type  */
	USB_UAC2_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE,          /* CLOCK_SOURCE descriptor subtype  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES,   /* Constant uniquely identifying the Clock Source Entity within
                                                          the audio funcion */
	0x03,                                              /* D1..0: 11: Internal Programmable Clock
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
	USB_LOW_BYTE(USB_UAC_USB_TERMINAL_STREAMING),
	USB_HIGH_BYTE(USB_UAC_USB_TERMINAL_STREAMING),           /* A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The
                                                                AudioStreaming interface descriptor points to the associated Terminal through the bTerminalLink field. */
	0x00,                                                    /* This Input Terminal has no association   */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES,         /* ID of the Clock Entity to which this Input Terminal is connected.  */
	USBD_UAC_PLAY_MAX_CHANNEL_COUNT,                                 /* This Terminal's output audio channel cluster has 4 logical output channels   */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_PLAY_MAX_CHANNEL_COUNT),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_PLAY_MAX_CHANNEL_COUNT),
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
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_PLAY_MAX_CHANNEL_COUNT),   /* Size of the descriptor, in byte_width  : 6 + (4 + 1) * 4 */
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
#if USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_4 || USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_6 || USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_8
	0x00,                                                    /*  bmaControls[3](0x00000000)*/
	0x00,
	0x00,
	0x00,
	0x00,                                                    /*  bmaControls[4](0x00000000) */
	0x00,
	0x00,
	0x00,
#endif
#if USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_6 || USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_8
	0x00,                                                    /*  bmaControls[5](0x00000000)*/
	0x00,
	0x00,
	0x00,
	0x00,                                                    /*  bmaControls[6](0x00000000) */
	0x00,
	0x00,
	0x00,
#endif
#if USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_8
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
	USBD_UAC_OT_DESC_TYPE_LOW(USBD_UAC_PLAY_MAX_CHANNEL_COUNT),
	USBD_UAC_OT_DESC_TYPE_HIGH(USBD_UAC_PLAY_MAX_CHANNEL_COUNT),      /* A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The
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

#ifdef CONFIG_USBD_UAC_RECORD
	/* Audio Control Clock Source Unit Descriptor 2.0 (Microphone) */
	USB_UAC2_LEN_CLK_SRC_DESC,                               /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC2_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE,                /* CLOCK_SOURCE descriptor subtype  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE,         /* Constant uniquely identifying the Clock Source Entity within
	                                                         the audio funcion */
	0x03,                                                    /* D1..0: 11: Internal Programmable Clock
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
	USB_LOW_BYTE(USB_UAC_INPUT_TERMINAL_MICROPHONE),         /* Microphone terminal type */
	USB_HIGH_BYTE(USB_UAC_INPUT_TERMINAL_MICROPHONE),
	0x00,                                                    /* This Input Terminal has no association   */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE,         /* ID of the Clock Entity to which this Input Terminal is connected.  */
	USBD_UAC_RECORD_CHANNEL_COUNT,                           /* bNrChannels */
	USB_LOW_BYTE(USBD_UAC_RECORD_CHANNEL_CONFIG),            /* bmChannelConfig */
	USB_HIGH_BYTE(USBD_UAC_RECORD_CHANNEL_CONFIG),
	0x00,
	0x00,
	0x00,                                                    /* Index of a string descriptor, describing the name of the first logical channel.  */
	0x00,
	0x00,                                                    /* bmControls: no controls present */
	0x00,                                                    /* Index of a string descriptor, describing the Input Terminal.  */

	/* Microphone FEATURE_UNIT descriptor subtype */
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_RECORD_CHANNEL_COUNT), /* Size of the descriptor, in byte_width */
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
#if USBD_UAC_RECORD_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_2
	0x00,
	0x00,
	0x00,
	0x00,                                                    /* bmaControls(2) logical channel 2: no controls */
#endif
	0x00,                                                    /* iFeature, Index of a string descriptor, describing this Feature Unit.*/

	/* Microphone OUTPUT_TERMINAL descriptor subtype   */
	USB_UAC2_LEN_AC_OUT_TTY_DESC,                             /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                    /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AC_OUTPUT_TERMINAL,               /* OUTPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE, /* Constant uniquely identifying the Terminal within the audio
	                                                                 function. This value is used in all requests to address this Terminal.   */
	USB_LOW_BYTE(USB_UAC_USB_TERMINAL_STREAMING),             /* USB Streaming terminal type */
	USB_HIGH_BYTE(USB_UAC_USB_TERMINAL_STREAMING),
	0x00,                                                     /* This Output Terminal has no association  */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT,         /* ID of the Unit or Terminal to which this Terminal is connected.  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE,          /* ID of the Clock Entity to which this Output Terminal is connected  */
	0x00,
	0x00,                                                     /* bmControls: no controls present */
	0x00,                                                     /* Index of a string descriptor, describing the Output Terminal.  */
#endif

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
	USBD_UAC_PLAY_AS_EP_COUNT,                                /* bNumEndpoints: the data EP, plus the feedback EP */
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
	0x00,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CHANNEL_COUNT_2,                                 /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CHANNEL_COUNT_2),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CHANNEL_COUNT_2),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_PLAY_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_PLAY_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	USBD_UAC_PLAY_EP_ATTR,                                   /* bmAttributes: isochronous data, adaptive or asynchronous */
	USB_LOW_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CHANNEL_COUNT_2, USBD_UAC_PLAY_BYTE_WIDTH)),   /* wMaxPacketSize (patched at runtime) */
	USB_HIGH_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CHANNEL_COUNT_2, USBD_UAC_PLAY_BYTE_WIDTH)),
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
	USBD_UAC_PLAY_FEEDBACK_EP_DESC

#if USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_4 || USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_6 || USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/2*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                     /* The number of this interface is 1.   */
	0x02,                                                     /* The value used to select the alternate setting for this interface is 0 */
	USBD_UAC_PLAY_AS_EP_COUNT,                                /* bNumEndpoints: the data EP, plus the feedback EP */
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
	0x00,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CHANNEL_COUNT_4,                                       /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CHANNEL_COUNT_4),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CHANNEL_COUNT_4),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_PLAY_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_PLAY_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	USBD_UAC_PLAY_EP_ATTR,                                   /* bmAttributes: isochronous data, adaptive or asynchronous */
	USB_LOW_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CHANNEL_COUNT_4, USBD_UAC_PLAY_BYTE_WIDTH)),    /* wMaxPacketSize (patched at runtime) */
	USB_HIGH_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CHANNEL_COUNT_4, USBD_UAC_PLAY_BYTE_WIDTH)),
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
	USBD_UAC_PLAY_FEEDBACK_EP_DESC
#endif

#if USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_6 || USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/3*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                     /* The number of this interface is 1.   */
	0x03,                                                     /* The value used to select the alternate setting for this interface is 0 */
	USBD_UAC_PLAY_AS_EP_COUNT,                                /* bNumEndpoints: the data EP, plus the feedback EP */
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
	0x00,                                                     /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                     /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CHANNEL_COUNT_6,                                       /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CHANNEL_COUNT_6),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CHANNEL_COUNT_6),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_PLAY_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_PLAY_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	USBD_UAC_PLAY_EP_ATTR,                                   /* bmAttributes: isochronous data, adaptive or asynchronous */
	USB_LOW_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CHANNEL_COUNT_6, USBD_UAC_PLAY_BYTE_WIDTH)),    /* wMaxPacketSize (patched at runtime) */
	USB_HIGH_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CHANNEL_COUNT_6, USBD_UAC_PLAY_BYTE_WIDTH)),
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
	USBD_UAC_PLAY_FEEDBACK_EP_DESC
#endif

#if USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/4*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                     /* The number of this interface is 1.   */
	0x04,                                                     /* The value used to select the alternate setting for this interface is 0 */
	USBD_UAC_PLAY_AS_EP_COUNT,                                /* bNumEndpoints: the data EP, plus the feedback EP */
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
	0x00,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CHANNEL_COUNT_8,                                       /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CHANNEL_COUNT_8),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CHANNEL_COUNT_8),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_PLAY_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_PLAY_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	USBD_UAC_PLAY_EP_ATTR,                                   /* bmAttributes: isochronous data, adaptive or asynchronous */
	USB_LOW_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CHANNEL_COUNT_8, USBD_UAC_PLAY_BYTE_WIDTH)),    /* wMaxPacketSize (patched at runtime) */
	USB_HIGH_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CHANNEL_COUNT_8, USBD_UAC_PLAY_BYTE_WIDTH)),
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
	USBD_UAC_PLAY_FEEDBACK_EP_DESC
#endif

#ifdef CONFIG_USBD_UAC_RECORD
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
	0x00,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_RECORD_CHANNEL_COUNT,                           /* Number of physical channels in the AS Interface audio channel cluster */
	USB_LOW_BYTE(USBD_UAC_RECORD_CHANNEL_CONFIG),            /* bmChannelConfig */
	USB_HIGH_BYTE(USBD_UAC_RECORD_CHANNEL_CONFIG),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Microphone Audio Streaming Format Type Descriptor 2.0 */
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_RECORD_BYTE_WIDTH,                              /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_RECORD_BYTE_WIDTH),          /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_D2H,                                                  /* bEndpointAddress: dir IN (placeholder) */
	0x05,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	/* wMaxPacketSize: patched for the speed */
	USB_LOW_BYTE(USBD_UAC_RECORD_HS_MPS),
	USB_HIGH_BYTE(USBD_UAC_RECORD_HS_MPS),
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
#endif
};
/* The length is computed by USBD_UAC_CFG_DESC_BUF_LEN() from the channel count and the template is trimmed by
   #if on the same count: a mismatch would leave zero padding counted in wTotalLength */
_Static_assert(sizeof(usbd_uac_config_desc) == USBD_UAC_CFG_DESC_BUF_LEN(USBD_UAC_PLAY_MAX_CHANNEL_COUNT), "UAC cfg desc len");

/* Exported variables --------------------------------------------------------*/

/* UAC Class Driver, registered by usbd_uac_common.c */
const usbd_class_driver_t usbd_uac_driver = {
	.get_descriptor = usbd_uac_get_descriptor,
	.set_config = usbd_uac_set_config,
	.clear_config = usbd_uac_clear_config,
	.setup = usbd_uac_setup,
	.ep0_data_out = usbd_uac_handle_ep0_data_out,
#if defined(CONFIG_USBD_UAC_RECORD) || defined(CONFIG_USBD_UAC_PLAY_FEEDBACK)
	.ep_data_in = usbd_uac_handle_ep_data_in,
#endif
	.ep_data_out = usbd_uac_handle_ep_data_out,
	.status_changed = usbd_uac_status_changed,
	.sof = usbd_uac_handle_sof,
};

/* Private functions ---------------------------------------------------------*/

#if USBD_UAC_DESCRIPTOR_DUMP
/**
  * @brief  Dump UAC descriptor information.
  * @param  buf: Pointer to the descriptor buffer.
  * @param  len: Length of the descriptor buffer.
  * @retval Status
  */
static int usbd_uac_dump_descriptor(const u8 *buf, int len)
{
	int i = 0;
	int j;
	u8 len_val = 0;
	for (i = 0; i < len;) {
		/* Never read a header or body that runs past the buffer, a malformed bLength stops the walk */
		if ((len - i) < (int)USB_LEN_DESC_HEADER) {
			break;
		}
		len_val = buf[i];
		if ((len_val < USB_LEN_DESC_HEADER) || ((int)len_val > (len - i))) {
			break;
		}
		for (j = 0; j < (int)len_val; j++) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "%02x ", buf[i + j]);
		}
		i += len_val;
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
		if (len_val < 6) {
			break;
		}
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
	return RTK_SUCCESS;
}
#endif

/**
  * @brief  Check whether an interface number, local to this function, belongs to the class
  * @note   Composite mode already rebased wIndex to the local interface number.
  * @param  interface_number: Interface number from wIndex
  * @retval 1 if the interface is in the reported config descriptor, 0 otherwise
  */
static u8 usbd_uac_is_owned_interface(u16 interface_number)
{
	u8 owned;

	if ((interface_number == USB_UAC2_IF_IDX_AC_HEADSET) || (interface_number == USB_UAC2_IF_IDX_AS_HEADSET_HEADPHONES)) {
		owned = 1U;
#ifdef CONFIG_USBD_UAC_RECORD
	} else if (interface_number == USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE) {
		owned = 1U;
#endif
	} else {
		owned = 0U;
	}

	return owned;
}

#ifndef CONFIG_USBD_UAC_RECORD
/**
  * @brief  Check whether an entity ID belongs to the microphone path
  * @param  entity_id: Entity ID from the high byte of wIndex
  * @retval 1 for a mic entity, 0 otherwise
  */
static u8 usbd_uac_is_mic_entity(u8 entity_id)
{
	return ((entity_id == USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE) ||
			(entity_id == USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE) ||
			(entity_id == USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT) ||
			(entity_id == USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE)) ? 1U : 0U;
}
#endif

/**
  * @brief  Handle UAC clock validity request
  * @param  dev: USB device instance
  * @param  flag: Clock validity flag
  * @retval Status of the EP0 IN submit
  */
static int usbd_uac_clock_valid_request(usb_dev_t *dev, u8 flag)
{
	u8 valid = flag;
	usbd_ep_t *ep0_in = &dev->ep0_in;

	usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)&valid, sizeof(valid));
	ep0_in->xfer_len = sizeof(valid);
	return usbd_ep_transmit(dev, ep0_in);
}

/**
  * @brief  Handle UAC current freq request
  * @param  dev: USB device instance
  * @param  frequency: UAC current freq
  * @retval Status of the EP0 IN submit
  */
static int usbd_uac_current_sampling_freq_request(usb_dev_t *dev, u32 frequency)
{
	u32 current_freq = frequency;
	usbd_ep_t *ep0_in = &dev->ep0_in;

	usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)&current_freq, sizeof(current_freq));
	ep0_in->xfer_len = sizeof(current_freq);
	return usbd_ep_transmit(dev, ep0_in);
}

/**
  * @brief  Handle freq ctrl requests
  * @param  dev: USB device instance
  * @param  max_len: Maximum allowed length for response
  * @retval Status
  */
static int usbd_uac_sampling_freq_ctrl_range_request(usb_dev_t *dev, u16 max_len)
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
	return usbd_ep_transmit(dev, ep0_in);
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
	int ret = RTK_SUCCESS;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	/* Read once: usbd_uac_deinit() may clear it on another core */
	const usbd_uac_cb_t *cb = cdev->cb;

	/* Only the bConfigurationValue advertised in the config descriptor is valid */
	if (config != 1U) {
		return -RTK_ERR_BADARG;
	}

	cdev->dev = dev;
	/* The speed is only known once enumerated, the EPs are opened later */
	cdev->play_ep.info.binterval = usbd_uac_get_binterval(dev->dev_speed);
	cdev->record_ep.info.binterval = usbd_uac_get_binterval(dev->dev_speed);

	cdev->play_alt_setting = 0U;
	cdev->record_alt_setting = 0U;

	/* Invalidate the cached stream format on each SET_CONFIGURATION so the first
	 * SET_INTERFACE(alt=1) of the new session re-fires format_changed(). Without
	 * this, a hotplug re-enumeration leaves play_channel_count/play_byte_width matching the
	 * host's selection, the "!=" guard drops format_changed(), and apps that gate
	 * playback on it never restart. play_sampling_freq keeps its private_init default. */
	cdev->play_channel_count = 0U;
	cdev->play_byte_width = 0U;

	/* Ref USB 2.0 9.4.7: the status stage of SET_CONFIGURATION tells the host the device is
	   configured, so a failure of the app is reported and the core stalls the request */
	if ((cb != NULL) && (cb->set_config != NULL)) {
		ret = cb->set_config();
		if (ret != RTK_SUCCESS) {
			ret = -RTK_ERR_IO;
		}
	}

	if (ret == RTK_SUCCESS) {
		/* Published under the lock clear_config() resets it with, refer to usbd_uac_play_ep_arm() */
		usb_os_enter_critical();
		cdev->is_configured = 1U;
		usb_os_exit_critical();
	}

	return ret;
}

/**
  * @brief  Clear UAC configuration
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval None
  */
static void usbd_uac_clear_config(usb_dev_t *dev, u8 config)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
#ifdef CONFIG_USBD_UAC_RECORD
	usbd_ep_t *record_ep = &cdev->record_ep;
#endif
	usbd_ep_t *play_ep = &cdev->play_ep;

	UNUSED(config);

	/* Ref USB 2.0 9.1.1.3 / 9.4.10: after a bus reset or a de-configuration no alternate setting
	   is selected and the endpoints are gone. The SOF path, the record engine and the task APIs
	   test this state under the same lock, possibly on another core. streaming is kept: the
	   stream resumes on its own once the host selects the streaming setting again. */
	usb_os_enter_critical();

	/* Closes the EP arm path of the task APIs before the EPs go, refer to usbd_uac_play_ep_arm() */
	cdev->is_configured = 0U;
	cdev->play_alt_setting = 0U;
	cdev->record_alt_setting = 0U;
	cdev->play_stream.last_packet_len = 0U;
	cdev->play_stream.sof_count = 0U;
	cdev->play_stream.data_count = 0U;
	cdev->sof_count = 0U;
	/* A control transfer cut by the reset never gets its data stage */
	cdev->ctrl_req_pending = 0U;

#ifdef CONFIG_USBD_UAC_RECORD
	/* DeInit ISOC IN EP. cfg is kept until after unregistration, and the endpoint address was
	   checked by usbd_uac_private_init() */
	usbd_ep_deinit(dev, record_ep);
	/* The aborted IN transfer never completes */
	record_ep->xfer_state = 0U;
#endif

	/* DeInit ISOC OUT EP */
	usbd_ep_deinit(dev, play_ep);
#ifdef CONFIG_USBD_UAC_PLAY_FEEDBACK
	usbd_uac_play_feedback_close(dev);
#endif

	usb_os_exit_critical();
}

#ifdef CONFIG_USBD_UAC_RECORD
/**
  * @brief  Handle the RANGE request of the microphone clock frequency control
  * @note   Ref UAC 2.0 5.2.1: report the single rate the record path supports, so the host
  *         can only select the rate the app produces data at.
  * @param  dev: USB device instance
  * @param  max_len: Maximum allowed length for response
  * @retval Status of the EP0 IN submit
  */
static int usbd_uac_mic_sampling_freq_range_request(usb_dev_t *dev, u16 max_len)
{
	usb_uac2_freq_t sampling_freq_range;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	u32 freq = USBD_UAC_RECORD_SAMPLING_FREQ;
	u16 num_sub_ranges = 1U;
	u16 len = (u16)(sizeof(num_sub_ranges) + sizeof(usb_uac2_freq_t));

	usb_os_memcpy(ep0_in->xfer_buf, (u8 *)&num_sub_ranges, sizeof(num_sub_ranges));
	USBD_UAC_INIT_SUB_RANGE(sampling_freq_range, freq, freq, 0);
	usb_os_memcpy(ep0_in->xfer_buf + sizeof(num_sub_ranges), &sampling_freq_range, sizeof(sampling_freq_range));

	if (len > max_len) {
		len = max_len;
	}

	ep0_in->xfer_len = len;
	return usbd_ep_transmit(dev, ep0_in);
}
#endif

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
	/* Read once: usbd_uac_deinit() may clear it on another core */
	const usbd_uac_cb_t *cb = cdev->cb;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	int ret = RTK_SUCCESS;
	u8 entity_id, control_selector;
	u8 channel_count;
	u8 byte_width;
	u8 alt_setting;
	u8 format_updated = 0;

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
				/* Ref USB 2.0 Table 9-10: the whole wIndex is the interface number. Requests for
				   a foreign interface must be rejected so that composite dispatch is not broken
				   and a malformed wIndex cannot touch the streaming state. */
				if (req->wIndex == USB_UAC2_IF_IDX_AC_HEADSET) {
					/* The Audio Control interface owns alternate setting 0 only */
					if (alt_setting != 0U) {
						ret = -RTK_ERR_BADARG;
					}
				} else if (req->wIndex == USB_UAC2_IF_IDX_AS_HEADSET_HEADPHONES) {
					/* Ref USB 2.0 9.4.5: alt setting 0 is the zero-bandwidth setting, it stops the
					   stream and leaves the negotiated format untouched */
					if (alt_setting == 0U) {
						cdev->play_alt_setting = 0U;
						/* Ref UAC 2.0 3.16.2: no data is sent on the zero-bandwidth setting. Disarm the
						   SOF underrun filler until the next real packet, else it keeps feeding the
						   reader synthetic nodes after the host stopped. The OUT EP stays armed so
						   that the next alt>0 streams at once, ISOC EPs have no halt or toggle state. */
						cdev->play_stream.last_packet_len = 0U;
						cdev->play_stream.sof_count = 0U;
						cdev->play_stream.data_count = 0U;
#ifdef CONFIG_USBD_UAC_PLAY_FEEDBACK
						/* Ref USB 2.0 9.4.10: the feedback EP belongs to the streaming settings only */
						usbd_uac_play_feedback_close(dev);
#endif
					} else if (alt_setting != cdev->play_alt_setting) {
						switch (alt_setting) {
						case 1:
							byte_width = USBD_UAC_PLAY_BYTE_WIDTH;
							channel_count = 2;
							break;
#if USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_4 || USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_6 || USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_8
						case 2:
							byte_width = USBD_UAC_PLAY_BYTE_WIDTH;
							channel_count = 4;
							break;
#endif
#if USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_6 || USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_8
						case 3:
							byte_width = USBD_UAC_PLAY_BYTE_WIDTH;
							channel_count = 6;
							break;
#endif
#if USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_8
						case 4:
							byte_width = USBD_UAC_PLAY_BYTE_WIDTH;
							channel_count = 8;
							break;
#endif
						default:
							/* Ref USB 2.0 9.4.5: an alt setting that does not exist is a request error,
							   the streaming state must stay on the previous setting */
							ret = -RTK_ERR_BADARG;
							break;
						}

#ifdef CONFIG_USBD_UAC_PLAY_FEEDBACK
						/* Open the feedback EP on entering the streaming settings only, a switch between
						   two of them must not abort the feedback in flight */
						if ((ret == RTK_SUCCESS) && (cdev->play_alt_setting == 0U)) {
							ret = usbd_uac_play_feedback_open(dev);
						}
#endif
						if (ret == RTK_SUCCESS) {
							cdev->play_alt_setting = alt_setting;
							if ((cdev->play_byte_width != byte_width) && byte_width) {
								cdev->play_byte_width = byte_width;
								format_updated = 1;
							}
							if ((cdev->play_channel_count != channel_count) && channel_count) {
								cdev->play_channel_count = channel_count;
								format_updated = 1;
							}
							if (format_updated != 0) {
								if ((cb != NULL) && (cb->format_changed != NULL)) {
									cb->format_changed(cdev->play_sampling_freq, cdev->play_channel_count, cdev->play_byte_width);
								}
							}
						}
					}
#ifdef CONFIG_USBD_UAC_RECORD
				} else if (req->wIndex == USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE) {
					/* The mic AS interface owns alt setting 0 and 1 only */
					if (alt_setting > USBD_UAC_AS_MIC_ALT_SETTING_MAX) {
						ret = -RTK_ERR_BADARG;
					} else {
						/* Open the mic EP on entering the streaming setting only, a repeated
						   alt=1 must not abort the IN transfer in flight */
						if ((alt_setting != 0U) && (cdev->record_alt_setting == 0U)) {
							ret = usbd_uac_isoc_in_ep_open(dev);
						}
						/* Ref USB 2.0 9.4.10: a setting that can not be selected is a request error,
						   the interface stays on the previous one. The record engine tests the
						   setting under the same lock, possibly on another core. */
						if (ret == RTK_SUCCESS) {
							usb_os_enter_critical();
							cdev->record_alt_setting = alt_setting;
							usb_os_exit_critical();
						}
					}
#endif
				} else {
					ret = -RTK_ERR_BADARG;
				}
			} else {
				ret = -RTK_ERR_BADARG;
			}
			break;

		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				/* Ref USB 2.0 Table 9-11: the whole wIndex is the interface number, and 9.4.4
				   requires a request error for an interface that does not exist */
				if (req->wIndex == USB_UAC2_IF_IDX_AC_HEADSET) {
					ep0_in->xfer_buf[0] = 0U;
				} else if (req->wIndex == USB_UAC2_IF_IDX_AS_HEADSET_HEADPHONES) {
					ep0_in->xfer_buf[0] = cdev->play_alt_setting;
#ifdef CONFIG_USBD_UAC_RECORD
				} else if (req->wIndex == USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE) {
					ep0_in->xfer_buf[0] = cdev->record_alt_setting;
#endif
				} else {
					ret = -RTK_ERR_BADARG;
					break;
				}
				ep0_in->xfer_len = 1U;
				ret = usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = -RTK_ERR_BADARG;
			}
			break;

		case USB_REQ_GET_STATUS:
			/* Ref USB 2.0 9.4.5: an interface that does not exist is a request error. The core
			   already checked wValue, wLength and the device state. */
			if ((dev->dev_state == USBD_STATE_CONFIGURED) && (usbd_uac_is_owned_interface(req->wIndex) != 0U)) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				ret = usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = -RTK_ERR_BADARG;
			}
			break;

		default:
			ret = -RTK_ERR_BADARG;
			break;
		}
		break;/* case USB_REQ_TYPE_STANDARD */

	case USB_REQ_TYPE_CLASS :
		if ((req->bmRequestType & USB_REQ_RECIPIENT_MASK) != USB_REQ_RECIPIENT_INTERFACE) {
			ret = -RTK_ERR_BADARG;
			break;
		}
		/* Ref UAC 2.0 5.2.2: the low byte of wIndex is the interface number and the high byte
		   is the entity ID. All the audio controls of this class live on the AC interface. */
		if (USB_LOW_BYTE(req->wIndex) != USB_UAC2_IF_IDX_AC_HEADSET) {
			ret = -RTK_ERR_BADARG;
			break;
		}
		entity_id = USB_HIGH_BYTE(req->wIndex);
		control_selector = USB_HIGH_BYTE(req->wValue);

		/* Ref UAC 2.0 5.2.2: the low byte of wValue is the channel number. Only the master
		   channel (0) controls are declared, and a Clock Source control shall use 0, so any other
		   channel addresses a control that does not exist (5.2.5.7). */
		if (USB_LOW_BYTE(req->wValue) != 0U) {
			ret = -RTK_ERR_BADARG;
			break;
		}
#ifndef CONFIG_USBD_UAC_RECORD
		/* A playback only build declares no mic entity, so a request naming one addresses a
		   non-existent entity. Rejecting it here also keeps its data stage away from ep0_data_out(),
		   which only runs for an accepted request. */
		if (usbd_uac_is_mic_entity(entity_id) != 0U) {
			ret = -RTK_ERR_BADARG;
			break;
		}
#endif

		if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
			switch (entity_id) {
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE:
				if (control_selector == USB_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						ret = usbd_uac_current_sampling_freq_request(dev, cdev->record_sampling_freq);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
#ifdef CONFIG_USBD_UAC_RECORD
						ret = usbd_uac_mic_sampling_freq_range_request(dev, req->wLength);
#else
						ret = -RTK_ERR_BADARG;
#endif
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 0);
						ret = -RTK_ERR_BADARG;
					}
				} else if (control_selector == USB_UAC2_CS_CLK_VALID_CONTROL) {
					/* Ref UAC 2.0 4.7.2.1: bmControls declares the validity control read-only */
					if (req->bRequest == USB_UAC_REQ_CUR) {
						ret = usbd_uac_clock_valid_request(dev, 1U); /* The internal clock is always valid */
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 31);
						ret = -RTK_ERR_BADARG;
					}
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 1);
					ret = -RTK_ERR_BADARG;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES:
				if (control_selector == USB_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						ret = usbd_uac_current_sampling_freq_request(dev, cdev->play_sampling_freq);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						ret = usbd_uac_sampling_freq_ctrl_range_request(dev, req->wLength);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 2);
						ret = -RTK_ERR_BADARG;
					}
				} else if (control_selector == USB_UAC2_CS_CLK_VALID_CONTROL) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						ret = usbd_uac_clock_valid_request(dev, 1U); /* The internal clock is always valid */
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 3);
						ret = -RTK_ERR_BADARG;
					}
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 4);
					ret = -RTK_ERR_BADARG;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT:
				if (control_selector == USB_UAC_FU_MUTE) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						ep0_in->xfer_buf[0] = cdev->play_mute;
						ep0_in->xfer_len = 1U;
						ret = usbd_ep_transmit(dev, ep0_in);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 9);
						ret = -RTK_ERR_BADARG;
					}
				} else if (control_selector == USB_UAC_FU_VOLUME) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *) & (cdev->play_volume), 2);
						ep0_in->xfer_len = MIN(2U, (u32)req->wLength);
						ret = usbd_ep_transmit(dev, ep0_in);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						usb_uac2_ctrl_range_layout2_struct response;
						response.wNumSubRanges = 1;
						response.wMIN = (cdev->cfg->volume_min != 0U) ? cdev->cfg->volume_min : USBD_UAC_DEFAULT_VOLUME_MIN;
						response.wMAX = (cdev->cfg->volume_max != 0U) ? cdev->cfg->volume_max : USBD_UAC_DEFAULT_VOLUME_MAX;
						response.wRES = 1;

						usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)&response, sizeof(response));
						ep0_in->xfer_len = MIN((u32)sizeof(response), (u32)req->wLength);
						ret = usbd_ep_transmit(dev, ep0_in);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 10);
						ret = -RTK_ERR_BADARG;
					}
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 11);
					ret = -RTK_ERR_BADARG;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT: //0x08 mic FU
				if (control_selector == USB_UAC_FU_MUTE) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						ep0_in->xfer_buf[0] = cdev->record_mute;
						ep0_in->xfer_len = 1U;
						ret = usbd_ep_transmit(dev, ep0_in);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 20);
						ret = -RTK_ERR_BADARG;
					}
				} else if (control_selector == USB_UAC_FU_VOLUME) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *) & (cdev->record_volume), 2);
						ep0_in->xfer_len = MIN(2U, (u32)req->wLength);
						ret = usbd_ep_transmit(dev, ep0_in);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						usb_uac2_ctrl_range_layout2_struct response;
						response.wNumSubRanges = 1;
						response.wMIN = (cdev->cfg->volume_min != 0U) ? cdev->cfg->volume_min : USBD_UAC_DEFAULT_VOLUME_MIN;
						response.wMAX = (cdev->cfg->volume_max != 0U) ? cdev->cfg->volume_max : USBD_UAC_DEFAULT_VOLUME_MAX;
						response.wRES = 1;

						usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)&response, sizeof(response));
						ep0_in->xfer_len = MIN((u32)sizeof(response), (u32)req->wLength);
						ret = usbd_ep_transmit(dev, ep0_in);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 21);
						ret = -RTK_ERR_BADARG;
					}
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 22);
					ret = -RTK_ERR_BADARG;
				}
				break;

			default:
				USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 12);
				ret = -RTK_ERR_BADARG;
				break;
			}
		} else {
			/* USB_H2D: only SET CUR exists, RANGE is defined for the D2H direction only (Ref UAC 2.0
			   5.2.1), so it falls to the error branches with any other request */
			switch (entity_id) {
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE:
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES:
				if (control_selector == USB_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						/* Ref UAC 2.0 5.2.5.1.1: dCUR of the frequency control, Layout 3 */
						ret = usbd_uac_receive_control_data(dev, req, 4U);
					} else {
						/* Set freq err */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 13);
						ret = -RTK_ERR_BADARG;
					}
				} else {
					/* Set freq ctrl err*/
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 14);
					ret = -RTK_ERR_BADARG;
				}
				break;/* case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES */

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT: //0x05 FU
				if (control_selector == USB_UAC_FU_MUTE) { //mute
					if (req->bRequest == USB_UAC_REQ_CUR) {
						/* Ref UAC 2.0 5.2.5.7.1: bCUR of the mute control, Layout 1 */
						ret = usbd_uac_receive_control_data(dev, req, 1U);
					} else {
						/* Set cur mute err */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 15);
						ret = -RTK_ERR_BADARG;
					}
				} else if (control_selector == USB_UAC_FU_VOLUME) { //volume
					if (req->bRequest == USB_UAC_REQ_CUR) {
						/* Ref UAC 2.0 5.2.5.7.2: wCUR of the volume control, Layout 2 */
						ret = usbd_uac_receive_control_data(dev, req, 2U);
					} else {
						/* Set cur volume range err */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 16);
						ret = -RTK_ERR_IO;
					}
				} else {
					/* Set fu err */
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 17);
					ret = -RTK_ERR_IO;
				}
				break;/* case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT */

			case USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT: //0x08 mic FU
				if (control_selector == USB_UAC_FU_MUTE) { //mute
					if (req->bRequest == USB_UAC_REQ_CUR) {
						/* Ref UAC 2.0 5.2.5.7.1: bCUR of the mute control, Layout 1 */
						ret = usbd_uac_receive_control_data(dev, req, 1U);
					} else {
						/* Set cur mute err */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 23);
						ret = -RTK_ERR_BADARG;
					}
				} else if (control_selector == USB_UAC_FU_VOLUME) { //volume
					if (req->bRequest == USB_UAC_REQ_CUR) {
						/* Ref UAC 2.0 5.2.5.7.2: wCUR of the volume control, Layout 2 */
						ret = usbd_uac_receive_control_data(dev, req, 2U);
					} else {
						/* Set cur volume range err */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 24);
						ret = -RTK_ERR_IO;
					}
				} else {
					/* Set fu err */
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 25);
					ret = -RTK_ERR_IO;
				}
				break;/* case USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT */

			default:
				USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 18);
				ret = -RTK_ERR_BADARG;
				break;
			}
		}
		break;/* case USB_REQ_TYPE_CLASS */

	case USB_REQ_TYPE_VENDOR:
		if ((cb == NULL) || (cb->setup == NULL)) {
			/* No handler for vendor requests, STALL so that the host recovers promptly
			   instead of waiting out the data stage */
			ret = -RTK_ERR_BADARG;
		} else if (req->wLength != 0U) {
			if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
				usb_setup_req_t response_request;
				u16 response_len = req->wLength;

				/* Ref USB 2.0 9.3.4: wLength is the maximum the host accepts, never trust it
				   as a buffer size. Clamp to the EP0 buffer to avoid an over-write. */
				if (response_len > ep0_in->xfer_buf_len) {
					response_len = (u16)ep0_in->xfer_buf_len;
				}

				/* EP0 buffer is shared with descriptor and OUT traffic, clear the response
				   window so a callback writing fewer bytes cannot leak stale data. */
				usb_os_memset((void *)ep0_in->xfer_buf, 0, response_len);
				/* The callback sizes its answer by wLength: hand it a copy carrying the clamped
				   length, the request itself is left untouched for the core's ZLP decision */
				usb_os_memcpy((void *)&response_request, (const void *)req, sizeof(usb_setup_req_t));
				response_request.wLength = response_len;
				ret = cb->setup(&response_request, ep0_in->xfer_buf);
				if (ret == RTK_SUCCESS) {
					ep0_in->xfer_len = response_len;
					/* Propagate a submit failure so that the core stalls EP0 instead of
					   leaving the host waiting out the data stage, ref USB 2.0 8.5.3.4 */
					ret = usbd_ep_transmit(dev, ep0_in);
				}
			} else {
				/* Ref USB 2.0 8.5.3: an H2D request with wLength > 0 carries the payload in a
				   following data stage, the request is handed to the app with it, refer to
				   usbd_uac_handle_ep0_data_out() */
				ret = usbd_uac_receive_control_data(dev, req, req->wLength);
			}
		} else {
			/* No data stage, the setup packet is self-contained, dispatch it right away */
			ret = cb->setup(req, NULL);
		}
		break;

	default:
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 19);
		ret = -RTK_ERR_IO;
		break;
	}

	return ret;
}

/**
  * @brief  Handles the SOF event for the UAC device
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @retval None
  */
static void usbd_uac_handle_sof(usb_dev_t *dev)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	const usbd_uac_cb_t *cb = cdev->cb;
	usbd_uac_stream_t *stream = &(cdev->play_stream);

#if !defined(CONFIG_USBD_UAC_RECORD) && !defined(CONFIG_USBD_UAC_PLAY_FEEDBACK)
	UNUSED(dev);
#endif

	if ((stream->streaming != 0) && (stream->last_packet_len != 0)) {
		stream->sof_count ++;

		if ((stream->sof_count > stream->data_count) &&
			((stream->sof_count - stream->data_count) >= USBD_UAC_PLAY_FILLER_THRESHOLD)) {
			stream->data_count = stream->sof_count;
			usbd_uac_append_filler(stream);
		}
	}

#ifdef CONFIG_USBD_UAC_RECORD
	if ((cdev->record_stream.streaming != 0) && (cdev->record_ep.xfer_state == 0)) {
		usbd_uac_isoc_in_transmit_next(dev);
	}
#endif
#ifdef CONFIG_USBD_UAC_PLAY_FEEDBACK
	/* One feedback per ms while the host streams playback: every 8th microframe at High Speed */
	if (dev->dev_speed == USB_SPEED_HIGH) {
		cdev->sof_count++;
		if (cdev->sof_count >= USBD_UAC_HS_SOF_COUNT_PER_MS) {
			cdev->sof_count = 0U;
			usbd_uac_play_feedback_transmit(dev);
		}
	} else {
		usbd_uac_play_feedback_transmit(dev);
	}
#endif

	if ((cb != NULL) && (cb->sof != NULL)) {
		cb->sof();
	}
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
	int ret = RTK_SUCCESS;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	const usbd_uac_cb_t *cb = cdev->cb;
	usb_setup_req_t *ctrl_req = &(cdev->ctrl_req);
	usbd_ep_t *ep0_out = &dev->ep0_out;
	u32 freq;
	s16 volume_value = 0;
	u8 target_volume;

	/* No pending request means this data stage does not belong to UAC, the composite dispatcher
	   already routed it by active_func. cb is released by usbd_uac_deinit(), which may run between
	   the setup and the data stage of an H2D request, so it is checked here as well. */
	if ((cdev->ctrl_req_pending == 0U) || (cb == NULL)) {
		return RTK_SUCCESS;
	}

	/* Consume the pending request: a single data stage belongs to exactly one setup packet, so
	   the saved request must not be replayed by a later EP0 OUT event. */
	cdev->ctrl_req_pending = 0U;

	if ((((ctrl_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS) && ((ctrl_req->bmRequestType & 0x1FU) == USB_REQ_RECIPIENT_INTERFACE))
		&& (ctrl_req->bRequest == USB_UAC_REQ_CUR)) {
		if (USB_HIGH_BYTE(ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT) {
			if ((USB_HIGH_BYTE(ctrl_req->wValue) == USB_UAC_FU_MUTE) && (ctrl_req->wLength == 0x01)) {
				/* Ref UAC 2.0 5.2.5.7.1: GET_CUR reports bCUR back, cache it whether or not the app listens */
				cdev->play_mute = (ep0_out->xfer_buf[0] != 0U) ? 1U : 0U;
				if (cb->mute_changed != NULL) {
					cb->mute_changed(cdev->play_mute);
				}
			} else if ((USB_HIGH_BYTE(ctrl_req->wValue) == USB_UAC_FU_VOLUME) && (ctrl_req->wLength == 0x02)) {
				volume_value = usbd_uac_le16_to_s16(ep0_out->xfer_buf);

				target_volume = usbd_uac_volume_linear_interpolation(volume_value);

				if (target_volume != USBD_UAC_VOLUME_ERR_VAL) {
					/* Ref UAC 2.0 5.2.5.7.2: GET_CUR reports wCUR back, cache it whether or not the app listens */
					cdev->play_volume = (u16)volume_value;
					if (cb->volume_changed != NULL) {
						cb->volume_changed(target_volume);
					}
				} else {
					/* Outside the reported RANGE, or 0x8000 (-inf dB): UAC 2.0 5.2.5.7.2 defines no
					   error for it, so the request is acknowledged and the volume kept; GET_CUR then
					   reports the wCUR actually in use. A STALL would also reject the legal 0x8000. */
				}
			}
		} else if (USB_HIGH_BYTE(ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT) {
			if ((USB_HIGH_BYTE(ctrl_req->wValue) == USB_UAC_FU_MUTE) && (ctrl_req->wLength == 0x01)) {
				cdev->record_mute = (ep0_out->xfer_buf[0]) ? 1 : 0;
			} else if ((USB_HIGH_BYTE(ctrl_req->wValue) == USB_UAC_FU_VOLUME) && (ctrl_req->wLength == 0x02)) {
				cdev->record_volume = (u16)ep0_out->xfer_buf[0] + ((u16)(ep0_out->xfer_buf[1]) << 8);
			}
		}

#ifdef CONFIG_USBD_UAC_RECORD
		if ((USB_HIGH_BYTE(ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE)
			&& (USB_HIGH_BYTE(ctrl_req->wValue) == USB_UAC_CS_SAM_FREQ_CONTROL) && (ctrl_req->wLength == 0x04)) {
			freq = ((u32)ep0_out->xfer_buf[3] << 24) | ((u32)ep0_out->xfer_buf[2] << 16) | ((u32)ep0_out->xfer_buf[1] << 8) | (u32)ep0_out->xfer_buf[0];
			/* Ref UAC 2.0 4.7.2.1: the mic clock is independent of the headphone one, it only
			   offers the record rate, so the mic EP keeps its packet size and playback is not touched */
			if (freq == USBD_UAC_RECORD_SAMPLING_FREQ) {
				cdev->record_sampling_freq = freq;
			} else {
				/* Ref USB 2.0 9.2.7: an unsupported value is a request error, STALL the status stage */
				ret = -RTK_ERR_BADARG;
			}
		}
#endif

		if ((USB_HIGH_BYTE(ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES)
			&& (USB_HIGH_BYTE(ctrl_req->wValue) == USB_UAC_CS_SAM_FREQ_CONTROL) && (ctrl_req->wLength == 0x04)) {
			freq = ((u32)ep0_out->xfer_buf[3] << 24) | ((u32)ep0_out->xfer_buf[2] << 16) | ((u32)ep0_out->xfer_buf[1] << 8) | (u32)ep0_out->xfer_buf[0];
			if (usbd_uac_is_valid_sample_rate(freq, dev->dev_speed)) {
				if (cdev->play_sampling_freq != freq) {
					/* The OUT EP and the ring are sized for the widest declared format, refer to
					   usbd_uac_get_play_max_packet_size(), so a packet at the new rate already fits: the stream
					   is left running and the application restarts it with the new format. */
					cdev->play_sampling_freq = freq;

					if (cb->format_changed != NULL) {
						cb->format_changed(cdev->play_sampling_freq, cdev->play_channel_count, cdev->play_byte_width);
					}
				}
			} else {
				/* Ref USB 2.0 9.2.7: an unsupported value is a request error, STALL the status stage */
				ret = -RTK_ERR_BADARG;
			}
		}

	} else if ((ctrl_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_VENDOR) {
		/* Ref USB 2.0 8.5.3.1: the status stage reports the outcome, let the app decide */
		if (cb->setup != NULL) {
			ret = cb->setup(ctrl_req, ep0_out->xfer_buf);
		} else {
			ret = -RTK_ERR_BADARG;
		}
	} else {
		/* Other class requests are rejected in the setup stage */
	}

	return ret;
}

/**
  * @brief  Patch the runtime-assigned fields in the configuration descriptor
  * @note   Replaces direction-only EP placeholders with the actual addresses from the class
  *         configuration, writes the ISOC wMaxPacketSize of the speed the descriptor describes and
  *         rewrites the iInterface string index with the index the core assigned. Fields are located
  *         by descriptor type, subtype and interface, and written byte by byte because the buffer
  *         may be unaligned in composite mode.
  * @param  desc: Pointer to the descriptor buffer (after configuration descriptor header)
  * @param  len: Total length of descriptor data
  * @param  speed: Speed the descriptor describes, the other speed for an Other-Speed descriptor
  * @retval void
  */
static void usbd_uac_patch_descriptor(u8 *desc, u16 len, u8 speed)
{
	u16 i;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	const usbd_uac_config_t *cfg = cdev->cfg;
#ifdef CONFIG_USBD_UAC_RECORD
	u16 record_mps = usbd_uac_get_record_max_packet_size(speed);
#endif
	u8 interface_number = 0xFFU;
	u8 interface_subclass = 0U;
	/* Channels of the headphone alt setting being walked, from its AS_GENERAL descriptor */
	u8 play_channel_count = 0U;

	for (i = 0; i < len;) {
		u8 desc_len;
		u8 desc_type;
		/* bLength and bDescriptorType are read only once both are inside the buffer */
		if (((u32)i + 2U) > len) {
			break;
		}
		desc_len = desc[i];
		if ((desc_len < 2U) || (((u32)i + desc_len) > len)) {
			break;
		}
		desc_type = desc[i + 1];
		if ((desc_type == USB_DESC_TYPE_ENDPOINT) && (desc_len >= USB_LEN_EP_DESC)) {
			u8 addr = desc[i + 2];
			u8 dir = addr & USB_REQ_DIR_MASK;
			u8 type = desc[i + 3] & 0x03;
#ifdef CONFIG_USBD_UAC_PLAY_FEEDBACK
			/* Ref UAC 2.0 4.10.2.1: the feedback EP follows the data EP in the same alt setting */
			if ((dir == USB_D2H) && (desc[i + 3] == USBD_UAC_PLAY_FEEDBACK_EP_ATTR)) {
				desc[i + 2] = cfg->play_feedback_ep_addr;
				desc[i + 4] = (speed == USB_SPEED_HIGH) ? USBD_UAC_PLAY_FEEDBACK_HS_MPS : USBD_UAC_PLAY_FEEDBACK_FS_MPS;   /* wMaxPacketSize */
				desc[i + 6] = usbd_uac_get_play_feedback_binterval(speed);   /* bInterval */
			} else
#endif
#ifdef CONFIG_USBD_UAC_RECORD
				if ((dir == USB_D2H) && (type == USB_CH_EP_TYPE_ISOC)) {
					desc[i + 2] = cfg->record_ep_addr;
					desc[i + 4] = USB_LOW_BYTE(record_mps);      /* wMaxPacketSize */
					desc[i + 5] = USB_HIGH_BYTE(record_mps);
				} else
#endif
					if ((dir == USB_H2D) && (type == USB_CH_EP_TYPE_ISOC)) {
						/* Ref UAC 2.0 3.16.2: each alt setting is sized for its own channel count at
						   the highest rate declared at the speed described */
						u16 play_mps = (speed == USB_SPEED_HIGH) ? (u16)USBD_UAC_GET_HS_MPS(play_channel_count, USBD_UAC_PLAY_BYTE_WIDTH) :
									   (u16)USBD_UAC_GET_FS_MPS(play_channel_count, USBD_UAC_PLAY_BYTE_WIDTH);
						desc[i + 2] = cfg->play_ep_addr;
						desc[i + 4] = USB_LOW_BYTE(play_mps);         /* wMaxPacketSize */
						desc[i + 5] = USB_HIGH_BYTE(play_mps);
					}
		} else if ((desc_type == USB_DESC_TYPE_INTERFACE) && (desc_len >= USB_LEN_IF_DESC)) {
			interface_number = desc[i + 2];
			interface_subclass = desc[i + 6];
			if (interface_subclass == USB_UAC_SUBCLASS_AUDIOCONTROL) {
				/* Only the audio control interface is named, matching the CDC ACM class. */
				desc[i + 8] = cdev->interface_string_index;     /* iInterface */
			}
		} else if ((desc_type == USB_UAC_CS_INTERFACE) && (desc_len >= USB_UAC2_LEN_AS_IF_ALT_SET_DESC) && (desc[i + 2] == USB_UAC_AS_GENERAL) &&
				   (interface_subclass == USB_UAC_SUBCLASS_AUDIOSTREAMING) && (interface_number == USB_UAC2_IF_IDX_AS_HEADSET_HEADPHONES)) {
			play_channel_count = desc[i + 10];              /* bNrChannels, Ref UAC 2.0 4.9.2 */
		}
		i += desc_len;
	}
}

/**
  * @brief  Get descriptor callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  req: Setup request handle
  * @param  buf: Pointer to Buffer
  * @param  buf_len: Capacity of buf in bytes
  * @retval Descriptor length
  */
static u16 usbd_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len)
{
#ifndef CONFIG_SUPPORT_USB_FS_ONLY
	usb_speed_type_t speed = dev->dev_speed;
#endif
	const u8 *desc = NULL;
	u16 len = 0;
	u8 type = USB_HIGH_BYTE(req->wValue);
	u8 is_cfg = 0;
	u8 desc_speed = USB_SPEED_FULL;

#ifdef CONFIG_SUPPORT_USB_FS_ONLY
	UNUSED(dev);
#endif

	switch (type) {

	case USB_DESC_TYPE_DEVICE:
		desc = usbd_uac_dev_desc;
		len = sizeof(usbd_uac_dev_desc);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		/* Built from the const template only: max_packet_size is sized once by usbd_uac_start_play(),
		   rewriting it here would race the ISRs that arm the EPs with it */
		desc = usbd_uac_config_desc;
		len = sizeof(usbd_uac_config_desc);
		is_cfg = 1;
#ifndef CONFIG_SUPPORT_USB_FS_ONLY
		desc_speed = speed;
#endif
		break;

#ifndef CONFIG_SUPPORT_USB_FS_ONLY
	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		desc = usbd_uac_device_qualifier_desc;
		len = sizeof(usbd_uac_device_qualifier_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		/* Ref USB 2.0 9.6.4: describes the configuration at the speed the device is not
		   running at, so its wMaxPacketSize is patched for that speed, not the current one */
		desc = usbd_uac_config_desc;
		len = sizeof(usbd_uac_config_desc);
		is_cfg = 1;
		desc_speed = (speed == USB_SPEED_HIGH) ? USB_SPEED_FULL : USB_SPEED_HIGH;
		break;
#endif

	case USB_DESC_TYPE_STRING:
		/* Every string this class owns is registered with usbd_add_string() and answered by
		   the core, which only forwards an index it does not know, e.g. the MS OS string */
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_GET_DESC, 0);
		break;

	default:
		break;
	}

	if (desc != NULL) {
		/* Truncation is not allowed: a short descriptor is illegal, so stall instead */
		if (len > buf_len) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Desc %d OVSZ %d > %d\n", type, len, buf_len);
			return 0;
		}

		usb_os_memcpy((void *)buf, (const void *)desc, len);
	}

	if (is_cfg != 0U) {
		buf[USB_CFG_DESC_OFFSET_TYPE] = type;
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);

		usbd_uac_patch_descriptor(buf + USB_LEN_CFG_DESC, len - USB_LEN_CFG_DESC, desc_speed);
	}

	return len;
}

#if USBD_UAC_DEBUG
#endif

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Get the ISOC packets per second, one per bInterval
  * @param  speed: USB connection speed
  * @retval Packets per second: 8000 at High Speed, 1000 at Full Speed
  */
u16 usbd_uac_get_packets_per_second(u8 speed)
{
	return (u16)((speed == USB_SPEED_HIGH) ? (USBD_UAC_ONE_KHZ * USBD_UAC_HS_SOF_COUNT_PER_MS) : USBD_UAC_ONE_KHZ);
}

/**
  * @brief  Get the ISOC bInterval of the streaming endpoints at a speed
  * @param  speed: USB connection speed
  * @retval bInterval
  */
u8 usbd_uac_get_binterval(u8 speed)
{
	return (speed == USB_SPEED_HIGH) ? USBD_UAC_HS_DEFAULT_BINTERVAL : USBD_UAC_FS_DEFAULT_BINTERVAL;
}

#ifdef CONFIG_USBD_UAC_PLAY_FEEDBACK
/**
  * @brief  Get the bInterval of the feedback EP, one feedback per ms
  * @param  speed: USB connection speed
  * @retval bInterval
  */
u8 usbd_uac_get_play_feedback_binterval(u8 speed)
{
	return (speed == USB_SPEED_HIGH) ? USBD_UAC_PLAY_FEEDBACK_HS_BINTERVAL : USBD_UAC_FS_DEFAULT_BINTERVAL;
}
#endif

/**
  * @brief  Check the class configuration against the UAC 2.0 limits
  * @param  cfg: Class configuration
  * @retval Status
  */
int usbd_uac_version_init(const usbd_uac_config_t *cfg)
{
#ifndef CONFIG_SUPPORT_USB_FS_ONLY
	/* The ring node count is a u8 and high speed needs 8 nodes per ms, refer to
	   usbd_uac_get_packets_per_second(): reject a depth that can not be served at either speed */
	if ((((cfg->play_buf_count != 0U) ? (u32)cfg->play_buf_count : (u32)USBD_UAC_DEFAULT_PLAY_BUF_COUNT) > USBD_UAC_BUFFER_COUNT_MAX) ||
		(((cfg->record_buf_count != 0U) ? (u32)cfg->record_buf_count : (u32)USBD_UAC_DEFAULT_RECORD_BUF_COUNT) > USBD_UAC_BUFFER_COUNT_MAX)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Buf cnt %u/%u err\n", cfg->play_buf_count, cfg->record_buf_count);
		return -RTK_ERR_BADARG;
	}
#else
	/* Full speed only: one node per ms, a u8 count always fits USBD_UAC_BUFFER_COUNT_MAX */
	UNUSED(cfg);
#endif

#if USBD_UAC_DESCRIPTOR_DUMP
	usbd_uac_dump_descriptor(usbd_uac_config_desc, sizeof(usbd_uac_config_desc) / sizeof(u8));
#endif

	return RTK_SUCCESS;
}

#ifdef CONFIG_USBD_UAC_RECORD
/**
  * @brief  Get the MPS of the record stream at a speed: one packet period of audio plus one frame
  * @note   Checked against the ISOC limits at build time, refer to USBD_UAC_RECORD_FS_MPS
  * @param  speed: USB connection speed
  * @retval Max packet size
  */
u16 usbd_uac_get_record_max_packet_size(u8 speed)
{
	return (u16)((speed == USB_SPEED_HIGH) ? USBD_UAC_RECORD_HS_MPS : USBD_UAC_RECORD_FS_MPS);
}
#endif

/**
  * @brief  Get the size of a playback (ISOC OUT) ring node
  * @note   Ref UAC 2.0 3.16.2: the host may switch the alt setting and the rate at any time and
  *         then sends up to the wMaxPacketSize of the selected alt setting. The node is therefore
  *         the MPS of the widest alt setting at the highest rate declared at this speed, so a format
  *         switch never reallocates the ring. Checked against the ISOC limits at build time.
  * @param  speed: USB connection speed
  * @retval Node size in bytes
  */
u16 usbd_uac_get_play_max_packet_size(u8 speed)
{
	if (speed == USB_SPEED_HIGH) {
		return (u16)USBD_UAC_GET_HS_MPS(USBD_UAC_PLAY_MAX_CHANNEL_COUNT, USBD_UAC_PLAY_BYTE_WIDTH);
	}

	return (u16)USBD_UAC_GET_FS_MPS(USBD_UAC_PLAY_MAX_CHANNEL_COUNT, USBD_UAC_PLAY_BYTE_WIDTH);
}

/**
  * @brief  Check freq requests
  * @param  freq: Freq requests from host
  * @param  speed: USB connection speed
  * @retval Status
  */
bool usbd_uac_is_valid_sample_rate(u32 freq, u8 speed)
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
  * @brief  Receive UAC data
  * @param  void
  * @retval Status
  */
int usbd_uac_play_ep_arm(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usb_dev_t *dev = cdev->dev;
	usbd_uac_stream_t *stream = &(cdev->play_stream);
	usbd_ep_t *play_ep = &cdev->play_ep;
	int ret = RTK_SUCCESS;

	if (dev == NULL) {
		return RTK_SUCCESS;
	}

	if (stream->max_packet_size == 0) {
		return -RTK_ERR_IO;
	}

	/* One critical section with the configured check: a bus reset or SET_CONFIGURATION(0)
	   handled by the ISR on another core runs clear_config(), which must not be undone by an EP
	   armed right after it. dev->is_ready can not tell: the core clears it only after
	   clear_config() returned, and not at all on a bus reset. None of the calls below blocks,
	   they nest the same lock. */
	usb_os_enter_critical();
	if (cdev->is_configured != 0U) {
		usb_ringbuf_reset(&(stream->ring));
		stream->sof_count = 0;
		stream->data_count = 0;
		stream->packet_count = 0;
		stream->streaming = 1;
		/* Arm the ISOC OUT EP here, its only ep_init path: the MPS is the ring node size,
		 * which fits every declared format. deinit+init is idempotent. */
		play_ep->info.mps = stream->max_packet_size;
		usbd_ep_deinit(dev, play_ep);
		ret = usbd_ep_init(dev, play_ep);
		if (ret == RTK_SUCCESS) {
			play_ep->xfer_buf = stream->dma_buf;
			play_ep->xfer_len = stream->max_packet_size;
			ret = usbd_ep_receive(dev, play_ep);
		}
	} else {
		/* Nothing armed, the stream did not start: let usbd_uac_start_play() fail so that the
		   caller retries instead of reading a stream that never comes */
		ret = -RTK_ERR_IO;
	}
	usb_os_exit_critical();

	return ret;
}

#ifdef CONFIG_USBD_COMPOSITE
#endif
