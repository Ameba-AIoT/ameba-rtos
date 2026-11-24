/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

/* uac 1.0 */
#include "usbd_uac_priv.h"
#ifdef CONFIG_USBD_COMPOSITE
#include "usbd_composite.h"
#endif
#include "usb_uac1.h"

/* Private defines -----------------------------------------------------------*/

#define USBD_UAC_DESCRIPTOR_DUMP (0)
#define USBD_UAC_FS_DEFAULT_BINTERVAL               1U      /**< FS ISOC bInterval used in EP descriptors */
/* Ref UAC 1.0 4.6.1.1: one packet per ms. At High Speed bInterval counts 2^(n-1) microframes, so 4 is 1 ms */
#define USBD_UAC_HS_DEFAULT_BINTERVAL               4U      /**< HS ISOC bInterval used in EP descriptors */
/* 1ms 8 frame in high speed */
#define USBD_UAC_HS_SOF_COUNT_PER_MS                8U
/* sample sampling freq max count */
#define USBD_UAC_SAMPLING_FREQ_MAX_COUNT            2U
/* Highest rate of usbd_uac_sampling_rates[]: every AS alt setting advertises its
   wMaxPacketSize for it, refer to USBD_UAC_GET_FS_MPS */
#define USBD_UAC_SAMPLING_FREQ_MAX                  USBD_UAC_SAMPLING_FREQ_48K
/* AC Format Type descriptor length */
#define USBD_UAC1_AS_FRT_TYPE_DESC_LEN              (USB_UAC1_AS_FRT_TYPE_DESC_LEN + 3 * 2)
// USB Out
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES    0x01U
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT          0x05U
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES   0x09U
// USB In, the same entity IDs as UAC 2.0
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE    0x02U
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT           0x08U
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE   0x10U
/* Mic AS interface, after the headphone one, CONFIG_USBD_UAC_RECORD only */
#define USBD_UAC1_IF_IDX_AS_HEADSET_MICROPHONE                     0x02U
/* Mic AS Format Type descriptor length: a single discrete rate, ref UAC 1.0 Format 2.2.5 */
#define USBD_UAC1_AS_MIC_FRT_TYPE_DESC_LEN                         (USB_UAC1_AS_FRT_TYPE_DESC_LEN + 3U)
#ifdef CONFIG_USBD_UAC_PLAY_FEEDBACK
/* Ref UAC 1.0 4.6.2.1 and USB 2.0 5.12.4.2: an asynchronous data EP with an explicit feedback EP */
#define USBD_UAC_PLAY_AS_EP_COUNT                   2U
#define USBD_UAC_PLAY_EP_ATTR                       0x05U   /* Isochronous, asynchronous, data */
#define USBD_UAC_PLAY_FEEDBACK_EP_ATTR              0x11U   /* Isochronous, no synchronization, feedback */
#define USBD_UAC_PLAY_FEEDBACK_MPS                  3U      /* 10.14 frames per frame at Full Speed */
#define USBD_UAC_PLAY_FEEDBACK_HS_MPS               4U      /* 16.16 frames per microframe at High Speed */
/* Ref UAC 1.0 4.6.2.1: bRefresh, the host reads the feedback every 2^bRefresh ms; a fresh value is
   queued every ms. 4 ms keeps a High Speed packet within the 16 ms the incomplete ISOC IN handler
   retargets, refer to usbd_pcd_handle_incomplete_in_isoc() */
#define USBD_UAC_PLAY_FEEDBACK_REFRESH              0x02U
#define USBD_UAC_PLAY_FEEDBACK_EP_LEN               USB_UAC1_EP_DESC_LEN
#else
#define USBD_UAC_PLAY_AS_EP_COUNT                   1U
#define USBD_UAC_PLAY_EP_ATTR                       0x09U   /* Isochronous, adaptive, data */
#define USBD_UAC_PLAY_FEEDBACK_EP_LEN               0U
#endif
/* len of each Audio stream interface/altsetting (the data EP, plus the feedback EP) */
#define USBD_UAC_AS_EIF_LEN                        (USB_LEN_IF_DESC + USB_UAC1_AS_IF_DESC_LEN + USBD_UAC1_AS_FRT_TYPE_DESC_LEN \
                                                   + USB_UAC1_EP_DESC_LEN + USB_UAC1_AUDIO_DATA_EP_DESC_LEN + USBD_UAC_PLAY_FEEDBACK_EP_LEN)
/* full speed AS alt setting num: alt 0 plus one alt per channel count from 2 up to the max */
#define USBD_UAC_FS_AS_ALT_SETTING_NUM              ((USBD_UAC_PLAY_MAX_CHANNEL_COUNT / 2U) + 1U)

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* bit_width */
#define USBD_UAC_BIT_WIDTH(byte_width)              (8U * (byte_width))

/* Standard AS isochronous feedback endpoint descriptor of a playback alt setting, ref UAC 1.0 4.6.2.1 */
#ifdef CONFIG_USBD_UAC_PLAY_FEEDBACK
#define USBD_UAC_PLAY_FEEDBACK_EP_DESC \
	USB_UAC1_EP_DESC_LEN,                       /* bLength */ \
	USB_DESC_TYPE_ENDPOINT,                     /* bDescriptorType */ \
	USB_D2H,                                    /* bEndpointAddress: dir IN (placeholder) */ \
	USBD_UAC_PLAY_FEEDBACK_EP_ATTR,             /* bmAttributes */ \
	USBD_UAC_PLAY_FEEDBACK_MPS,                 /* wMaxPacketSize */ \
	0x00, \
	USBD_UAC_FS_DEFAULT_BINTERVAL,              /* bInterval: patched for the speed */ \
	USBD_UAC_PLAY_FEEDBACK_REFRESH,             /* bRefresh */ \
	0x00,                                       /* bSynchAddress */
#else
#define USBD_UAC_PLAY_FEEDBACK_EP_DESC
#endif

/* AC feature uint descriptor length */
#define USBD_UAC_AC_FU_HEAD_DESC_LEN(channel_count)        (USB_UAC1_AC_FUNC_UNIT_DESC_LEN +  (channel_count))
#ifdef CONFIG_USBD_UAC_RECORD
/* Interfaces: audio control, headphone and mic audio streaming */
#define USBD_UAC_INTERFACE_COUNT                    3U
/* AC header descriptor length, baInterfaceNr[] listing both AS interfaces */
#define USBD_UAC_AC_HEADER_DESC_LEN                 (USB_UAC1_AC_IF_HEADER_LEN + 1U)
/* AC mic entities (input terminal + feature unit + output terminal) descriptor length */
#define USBD_UAC_AC_MIC_IF_LEN \
    (USB_UAC1_AC_IN_TTY_DESC_LEN + USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_RECORD_CHANNEL_COUNT) + USB_UAC1_AC_OUT_TTY_DESC_LEN)
/* len of the mic AS interface: alt 0 plus alt 1 with one IN EP */
#define USBD_UAC_AS_MIC_TIF_LEN \
    (USB_LEN_IF_DESC * 2U + USB_UAC1_AS_IF_DESC_LEN + USBD_UAC1_AS_MIC_FRT_TYPE_DESC_LEN + USB_UAC1_EP_DESC_LEN + USB_UAC1_AUDIO_DATA_EP_DESC_LEN)
#else
/* Interfaces: audio control and headphone audio streaming */
#define USBD_UAC_INTERFACE_COUNT                    2U
/* AC header descriptor length, baInterfaceNr[] listing the headphone AS interface */
#define USBD_UAC_AC_HEADER_DESC_LEN                 USB_UAC1_AC_IF_HEADER_LEN
#define USBD_UAC_AC_MIC_IF_LEN                      0U
#define USBD_UAC_AS_MIC_TIF_LEN                     0U
#endif
/* AC IF header descriptor length, the header plus every unit and terminal */
#define USBD_UAC_AC_IF_HEAD_DESC_LEN(channel_count) \
    (USBD_UAC_AC_HEADER_DESC_LEN + USB_UAC1_AC_IN_TTY_DESC_LEN + USBD_UAC_AC_FU_HEAD_DESC_LEN(channel_count) + USB_UAC1_AC_OUT_TTY_DESC_LEN \
     + USBD_UAC_AC_MIC_IF_LEN)

/* len of total Audio control interface */
#define USBD_UAC_AC_IF_LEN(channel_count)                 (USB_LEN_IF_DESC + USBD_UAC_AC_IF_HEAD_DESC_LEN(channel_count))

/* len of total Audio stream interface */
#define USBD_UAC_AS_TIF_LEN(alt_num)               (USBD_UAC_AS_EIF_LEN * (alt_num - 1) + USB_LEN_IF_DESC)

/* calculate full speed MPS */
#define USBD_UAC_CALC_FS_MPS(channel_count, byte_width, sampling_freq_hz)    ((channel_count) * (byte_width) * ((sampling_freq_hz) / USBD_UAC_ONE_KHZ + 1U))
/* check MPS */
#define USBD_UAC_IS_FS_MPS_VALID(channel_count, byte_width, sampling_freq_hz)   ((USBD_UAC_CALC_FS_MPS(channel_count, byte_width, sampling_freq_hz)) <= USBD_UAC_FS_ISOC_MPS)

/* get full speed MPS of an alt setting, sized for the highest declared rate */
#define USBD_UAC_GET_FS_MPS(channel_count, byte_width)  USBD_UAC_CALC_FS_MPS(channel_count, byte_width, USBD_UAC_SAMPLING_FREQ_MAX)

/* ISOC OUT ring node size: the MPS of the widest alt setting, so any host format fits */
#define USBD_UAC_PLAY_MPS                        USBD_UAC_GET_FS_MPS(USBD_UAC_PLAY_MAX_CHANNEL_COUNT, USBD_UAC_PLAY_BYTE_WIDTH)

/* Ref USB 2.0 5.6.3: a Full-Speed ISOC packet is at most 1023 bytes */
#if !USBD_UAC_IS_FS_MPS_VALID(USBD_UAC_PLAY_MAX_CHANNEL_COUNT, USBD_UAC_PLAY_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_MAX)
#error "UAC1: channels x bytes per sample shall be <= 20 to fit FS 48kHz (1023B). Lower 'Playback max channels' or 'Playback bit width'"
#endif

/* len of full speed total configuration descriptor buf */
#define USBD_UAC_FS_CFG_DESC_BUF_LEN(channel_count) \
    (USB_LEN_CFG_DESC + USB_LEN_IAD_DESC + USBD_UAC_AC_IF_LEN(channel_count) + USBD_UAC_AS_TIF_LEN(USBD_UAC_FS_AS_ALT_SETTING_NUM) \
     + USBD_UAC_AS_MIC_TIF_LEN)

/* Private function prototypes -----------------------------------------------*/

static int usbd_uac_set_config(usb_dev_t *dev, u8 config);
static void usbd_uac_clear_config(usb_dev_t *dev, u8 config);
static int usbd_uac_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len);
#ifdef CONFIG_USBD_COMPOSITE
static void usbd_uac_set_interface_base(u8 base);
#endif
static int usbd_uac_handle_ep0_data_out(usb_dev_t *dev);
static void usbd_uac_handle_sof(usb_dev_t *dev);
#if USBD_UAC_DESCRIPTOR_DUMP
static int usbd_uac_dump_descriptor(const u8 *buf, int len);
#endif
static u8 usbd_uac_is_owned_interface(u16 interface_number);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "UAC";

static const u32 usbd_uac_sampling_rates[USBD_UAC_SAMPLING_FREQ_MAX_COUNT] = {USBD_UAC_SAMPLING_FREQ_44K, USBD_UAC_SAMPLING_FREQ_48K};

/* USB Standard Device Descriptor */
static const u8 usbd_uac_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,            /* bLength */
	USB_DESC_TYPE_DEVICE,        /* bDescriptorType */
	0x00, 0x02,                  /* bcdUSB */
	0x00,                        /* bDeviceClass */
	0x00,                        /* bDeviceSubClass */
	0x00,                        /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,            /* bMaxPacketSize */
	USB_LOW_BYTE(USB_VID),  /* idUAC */
	USB_HIGH_BYTE(USB_VID),
	USB_LOW_BYTE(USB_PID),  /* idProduct */
	USB_HIGH_BYTE(USB_PID),
	0x00,                        /* bcdDevice */
	0x01,
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

/* The same template describes both speeds: the ISOC bInterval is patched for the speed it
   describes, refer to usbd_uac_patch_descriptor() */
static const u8 usbd_uac_config_desc[] = {
	/* USB UAC Device Configuration Descriptor */
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,                  /* bLength */
	USB_DESC_TYPE_CONFIGURATION,       /* bDescriptorType */
	0x00,                              /* wTotalLength: patched at runtime */
	0x00,
	USBD_UAC_INTERFACE_COUNT,          /* bNumInterfaces */
	0x01,                              /* Value to use as an argument to the SetConfiguration() request to select this configuration */
	0x00,                              /* Index of string descriptor describing this configuration */
	0x80,                              /* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
	0x32,                              /* bMaxPower */

	/* IAD for Audio Control + Audio Streaming */
	USB_LEN_IAD_DESC,                  /* bLength */
	USB_DESC_TYPE_IAD,                 /* bDescriptorType */
	0x00,                              /* bFirstInterface (patched by composite) */
	USBD_UAC_INTERFACE_COUNT,          /* bInterfaceCount */
	USB_UAC_CLASS_CODE,                /* bFunctionClass: Audio */
	USB_UAC_SUBCLASS_AUDIOCONTROL,     /* bFunctionSubClass */
	USB_UAC_IP_VERSION_1,              /* bFunctionProtocol */
	0x00,                              /* iFunction */

	/* 4.7.1 Standard AC Interface Descriptor */
	/* Interface 0 Descriptor */
	USB_LEN_IF_DESC,                   /* bLength */
	USB_DESC_TYPE_INTERFACE,           /* Interface Descriptor type */
	USB_UAC1_IF_IDX_AC_HEADSET,        /* Audio Control Interface Index - Headset (0) */
	0x00,                              /* bAlternateSetting */
	0x00,                              /* No associated endpoints with this interface (uses endpoint 0) */
	USB_UAC_CLASS_CODE,               /* Audio Device Class */
	USB_UAC_SUBCLASS_AUDIOCONTROL,    /* Audio Control Interface */
	0x00,                              /* Audio Protocol IP version 2.00 */
	0x00,                              /* Interface string descriptor index (0) */

	/* 4.7.2 Class-Specific AC Interface Descriptor */
	/* Audio headset */
	USBD_UAC_AC_HEADER_DESC_LEN,                                           /* bLength: 8 + bInCollection */
	USB_UAC_CS_INTERFACE,                               /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_HEADER,                                                     /* HEADER descriptor subtype  */
	USB_LOW_BYTE(USB_UAC1_VERSION_01_00),                                /* bcdADC */
	USB_HIGH_BYTE(USB_UAC1_VERSION_01_00),
	USB_LOW_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_PLAY_MAX_CHANNEL_COUNT)),
	USB_HIGH_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_PLAY_MAX_CHANNEL_COUNT)),/* Total number of byte_width returned for the class-specific AudioControl interface descriptor. Includes
                                                                         the combined length of this descriptor header and all Unit and Terminal descriptors.   */
	USBD_UAC_INTERFACE_COUNT - 1U,                                         /* bInCollection: the AS interfaces */
	0x01,                                                                  /* baInterfaceNr[1]: headphone AS interface */
#ifdef CONFIG_USBD_UAC_RECORD
	USBD_UAC1_IF_IDX_AS_HEADSET_MICROPHONE,                                /* baInterfaceNr[2]: mic AS interface */
#endif

	/* Headphone Audio Control Input Terminal Descriptor 1.0 (USB Streaming, type 0x0101) */
	USB_UAC1_AC_IN_TTY_DESC_LEN,                             /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_INPUT_TERMINAL,              /* INPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* Constant uniquely identifying the Terminal within the audio
                                                                  function. This value is used in all requests to address this Terminal.   */
	USB_LOW_BYTE(USB_UAC_USB_TERMINAL_STREAMING),
	USB_HIGH_BYTE(USB_UAC_USB_TERMINAL_STREAMING),           /* A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The
                                                                AudioStreaming interface descriptor points to the associated Terminal through the bTerminalLink field. */
	0x00,                                                    /* This Input Terminal has no association   */
	USBD_UAC_PLAY_MAX_CHANNEL_COUNT,                                 /* This Terminal's output audio channel cluster has 8 logical output channels   */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_PLAY_MAX_CHANNEL_COUNT),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_PLAY_MAX_CHANNEL_COUNT),
	0x00,                                                     /* iChannelNames */
	0x00,                                                    /* Index of a string descriptor, describing the Input Terminal.  */

	/* FEATURE_UNIT descriptor subtype */
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_PLAY_MAX_CHANNEL_COUNT),   /* Size of the descriptor, in byte_width  : 6 + (4 + 1) * 4 */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_FEATURE_UNIT,                                   /* FEATURE_UNIT descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT,       /* Constant uniquely identifying the Unit within the audio function. This
                                                                value is used in all requests to address this Unit.  */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* ID of the Unit or Terminal to which this Feature Unit is connected. */
	0x01,                                                     /* bControlSize */
	0x03,                                                     /* bmaControls[0]  D0: Mute: 1 (yes) D1: Volume: 1 (yes)*/
	0x00,                                                     /* bmaControls[1] */
	0x00,                                                     /* bmaControls[2] */
#if USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_4 || USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_6 || USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_8
	0X00,                                                     /* bmaControls[3 */
	0x00,                                                     /* bmaControls[4] */
#endif
#if USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_6 || USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_8
	0x00,                                                     /* bmaControls[5] */
	0X00,                                                     /* bmaControls[6] */
#endif
#if USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_8
	0x00,
	0x00,
#endif
	0x00,                                                     /* iFeature, Index of a string descriptor, describing this Feature Unit.*/

	/* OUTPUT_TERMINAL descriptor subtype   */
	USB_UAC1_AC_OUT_TTY_DESC_LEN,                             /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                    /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AC_OUTPUT_TERMINAL,              /* OUTPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES, /* Constant uniquely identifying the Terminal within the audio
                                                                 function. This value is used in all requests to address this Terminal.   */
	USBD_UAC_OT_DESC_TYPE_LOW(USBD_UAC_PLAY_MAX_CHANNEL_COUNT),
	USBD_UAC_OT_DESC_TYPE_HIGH(USBD_UAC_PLAY_MAX_CHANNEL_COUNT),      /* A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The
                                                                 AudioStreaming interface descriptor points to the associated Terminal through the bTerminalLink field.  */
	0x00,                                                     /* This Output Terminal has no association  */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT,        /* bSourceID,ID of the Unit or Terminal to which this Terminal is connected.  */
	0x00,                                                     /* iTerminal  */

#ifdef CONFIG_USBD_UAC_RECORD
	/* Mic Input Terminal Descriptor (Microphone, type 0x0201), ref UAC 1.0 4.3.2.1 */
	USB_UAC1_AC_IN_TTY_DESC_LEN,                              /* bLength */
	USB_UAC_CS_INTERFACE,                                     /* bDescriptorType */
	USB_UAC_AC_INPUT_TERMINAL,                                /* bDescriptorSubtype */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE,  /* bTerminalID */
	USB_LOW_BYTE(USB_UAC_INPUT_TERMINAL_MICROPHONE),         /* wTerminalType */
	USB_HIGH_BYTE(USB_UAC_INPUT_TERMINAL_MICROPHONE),
	0x00,                                                     /* bAssocTerminal */
	USBD_UAC_RECORD_CHANNEL_COUNT,                            /* bNrChannels */
	USB_LOW_BYTE(USBD_UAC_RECORD_CHANNEL_CONFIG),             /* wChannelConfig */
	USB_HIGH_BYTE(USBD_UAC_RECORD_CHANNEL_CONFIG),
	0x00,                                                     /* iChannelNames */
	0x00,                                                     /* iTerminal */

	/* Mic Feature Unit Descriptor, ref UAC 1.0 4.3.2.5 */
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_RECORD_CHANNEL_COUNT), /* bLength */
	USB_UAC_CS_INTERFACE,                                     /* bDescriptorType */
	USB_UAC_AC_FEATURE_UNIT,                                  /* bDescriptorSubtype */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT,         /* bUnitID */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE,  /* bSourceID */
	0x01,                                                     /* bControlSize */
	USB_UAC1_CONTROL_MUTE | USB_UAC1_CONTROL_VOLUME,          /* bmaControls[0]: master mute and volume */
	0x00,                                                     /* bmaControls[1] */
#if USBD_UAC_RECORD_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_2
	0x00,                                                     /* bmaControls[2] */
#endif
	0x00,                                                     /* iFeature */

	/* Mic Output Terminal Descriptor (USB Streaming, type 0x0101), ref UAC 1.0 4.3.2.2 */
	USB_UAC1_AC_OUT_TTY_DESC_LEN,                             /* bLength */
	USB_UAC_CS_INTERFACE,                                     /* bDescriptorType */
	USB_UAC_AC_OUTPUT_TERMINAL,                               /* bDescriptorSubtype */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE, /* bTerminalID */
	USB_LOW_BYTE(USB_UAC_USB_TERMINAL_STREAMING),             /* wTerminalType: USB Streaming */
	USB_HIGH_BYTE(USB_UAC_USB_TERMINAL_STREAMING),
	0x00,                                                     /* bAssocTerminal */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT,         /* bSourceID */
	0x00,                                                     /* iTerminal */
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
	0x00,                                                     /* The Protocol code is 32   */
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
	0x00,                                                     /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC1_AS_IF_DESC_LEN,                                 /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* The Terminal ID of the terminal to which this interface is connected */
	0x01,                                                    /* bDelay */
	0x01,
	0x00,                                                    /* wFormatTag:PCM */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USBD_UAC1_AS_FRT_TYPE_DESC_LEN,                           /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_CHANNEL_COUNT_2,                                       /* bNrChannels */
	USBD_UAC_PLAY_BYTE_WIDTH,                             /* bSubframeSize */
	USBD_UAC_BIT_WIDTH(USBD_UAC_PLAY_BYTE_WIDTH),         /* bBitResolution */
	0x2,                                                     /* bSamFreqType(supports 2 sample frequencies) */
	USB_LOW_BYTE(USBD_UAC_SAMPLING_FREQ_44K),                /* tSamFreq[1] (44100 Hz) */
	USB_HIGH_BYTE(USBD_UAC_SAMPLING_FREQ_44K),
	0x00,
	USB_LOW_BYTE(USBD_UAC_SAMPLING_FREQ_48K),                /* tSamFreq[1] (48000 Hz) */
	USB_HIGH_BYTE(USBD_UAC_SAMPLING_FREQ_48K),
	0x00,

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_UAC1_EP_DESC_LEN,                                    /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	USBD_UAC_PLAY_EP_ATTR,                                   /* bmAttributes: isochronous data, adaptive or asynchronous */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CHANNEL_COUNT_2, USBD_UAC_PLAY_BYTE_WIDTH)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CHANNEL_COUNT_2, USBD_UAC_PLAY_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */
	0x00,                                                    /* bRefresh */
	0x00,                                                    /* bSynchAddress: the feedback EP address, patched at runtime */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC1_AUDIO_DATA_EP_DESC_LEN,                         /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x01,                                                    /* bmAttributes */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
	USBD_UAC_PLAY_FEEDBACK_EP_DESC

#if USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_4 || USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_6 || USBD_UAC_PLAY_MAX_CHANNEL_COUNT == USBD_UAC_CHANNEL_COUNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/2*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                      /* The number of this interface is 1.   */
	0x02,                                                     /* The value used to select the alternate setting for this interface is 2 */
	USBD_UAC_PLAY_AS_EP_COUNT,                                /* bNumEndpoints: the data EP, plus the feedback EP */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	0x00,                                                     /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC1_AS_IF_DESC_LEN,                                 /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* The Terminal ID of the terminal to which this interface is connected */
	0x01,                                                    /* bDelay */
	0x01,
	0x00,                                                    /* wFormatTag:PCM */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USBD_UAC1_AS_FRT_TYPE_DESC_LEN,                           /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_CHANNEL_COUNT_4,                                       /* bNrChannels */
	USBD_UAC_PLAY_BYTE_WIDTH,                             /* bSubframeSize */
	USBD_UAC_BIT_WIDTH(USBD_UAC_PLAY_BYTE_WIDTH),         /* bBitResolution */
	0x2,                                                     /* bSamFreqType(supports 2 sample frequencies) */
	USB_LOW_BYTE(USBD_UAC_SAMPLING_FREQ_44K),                /* tSamFreq[1] (44100 Hz) */
	USB_HIGH_BYTE(USBD_UAC_SAMPLING_FREQ_44K),
	0x00,
	USB_LOW_BYTE(USBD_UAC_SAMPLING_FREQ_48K),                /* tSamFreq[1] (48000 Hz) */
	USB_HIGH_BYTE(USBD_UAC_SAMPLING_FREQ_48K),
	0x00,

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_UAC1_EP_DESC_LEN,                                    /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	USBD_UAC_PLAY_EP_ATTR,                                   /* bmAttributes: isochronous data, adaptive or asynchronous */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CHANNEL_COUNT_4, USBD_UAC_PLAY_BYTE_WIDTH)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CHANNEL_COUNT_4, USBD_UAC_PLAY_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */
	0x00,                                                    /* bRefresh */
	0x00,                                                    /* bSynchAddress: the feedback EP address, patched at runtime */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC1_AUDIO_DATA_EP_DESC_LEN,                         /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x01,                                                    /* bmAttributes */
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
	0x01,                                                      /* The number of this interface is 1.   */
	0x03,                                                     /* The value used to select the alternate setting for this interface is 2 */
	USBD_UAC_PLAY_AS_EP_COUNT,                                /* bNumEndpoints: the data EP, plus the feedback EP */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	0x00,                                                     /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC1_AS_IF_DESC_LEN,                                 /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* The Terminal ID of the terminal to which this interface is connected */
	0x01,                                                    /* bDelay */
	0x01,
	0x00,                                                    /* wFormatTag:PCM */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USBD_UAC1_AS_FRT_TYPE_DESC_LEN,                           /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_CHANNEL_COUNT_6,                                       /* bNrChannels */
	USBD_UAC_PLAY_BYTE_WIDTH,                             /* bSubframeSize */
	USBD_UAC_BIT_WIDTH(USBD_UAC_PLAY_BYTE_WIDTH),         /* bBitResolution */
	0x2,                                                     /* bSamFreqType(supports 2 sample frequencies) */
	USB_LOW_BYTE(USBD_UAC_SAMPLING_FREQ_44K),                /* tSamFreq[1] (44100 Hz) */
	USB_HIGH_BYTE(USBD_UAC_SAMPLING_FREQ_44K),
	0x00,
	USB_LOW_BYTE(USBD_UAC_SAMPLING_FREQ_48K),                /* tSamFreq[1] (48000 Hz) */
	USB_HIGH_BYTE(USBD_UAC_SAMPLING_FREQ_48K),
	0x00,

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_UAC1_EP_DESC_LEN,                                    /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	USBD_UAC_PLAY_EP_ATTR,                                   /* bmAttributes: isochronous data, adaptive or asynchronous */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CHANNEL_COUNT_6, USBD_UAC_PLAY_BYTE_WIDTH)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CHANNEL_COUNT_6, USBD_UAC_PLAY_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */
	0x00,                                                    /* bRefresh */
	0x00,                                                    /* bSynchAddress: the feedback EP address, patched at runtime */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC1_AUDIO_DATA_EP_DESC_LEN,                         /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x01,                                                    /* bmAttributes */
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
	0x01,                                                      /* The number of this interface is 1.   */
	0x04,                                                     /* The value used to select the alternate setting for this interface is 2 */
	USBD_UAC_PLAY_AS_EP_COUNT,                                /* bNumEndpoints: the data EP, plus the feedback EP */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	0x00,                                                     /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC1_AS_IF_DESC_LEN,                                 /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* The Terminal ID of the terminal to which this interface is connected */
	0x01,                                                    /* bDelay */
	0x01,
	0x00,                                                    /* wFormatTag:PCM */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USBD_UAC1_AS_FRT_TYPE_DESC_LEN,                           /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_CHANNEL_COUNT_8,                                       /* bNrChannels */
	USBD_UAC_PLAY_BYTE_WIDTH,                             /* bSubframeSize */
	USBD_UAC_BIT_WIDTH(USBD_UAC_PLAY_BYTE_WIDTH),         /* bBitResolution */
	0x2,                                                     /* bSamFreqType(supports 2 sample frequencies) */
	USB_LOW_BYTE(USBD_UAC_SAMPLING_FREQ_44K),                /* tSamFreq[1] (44100 Hz) */
	USB_HIGH_BYTE(USBD_UAC_SAMPLING_FREQ_44K),
	0x00,
	USB_LOW_BYTE(USBD_UAC_SAMPLING_FREQ_48K),                /* tSamFreq[1] (48000 Hz) */
	USB_HIGH_BYTE(USBD_UAC_SAMPLING_FREQ_48K),
	0x00,

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_UAC1_EP_DESC_LEN,                                    /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	USBD_UAC_PLAY_EP_ATTR,                                   /* bmAttributes: isochronous data, adaptive or asynchronous */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CHANNEL_COUNT_8, USBD_UAC_PLAY_BYTE_WIDTH)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CHANNEL_COUNT_8, USBD_UAC_PLAY_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */
	0x00,                                                    /* bRefresh */
	0x00,                                                    /* bSynchAddress: the feedback EP address, patched at runtime */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC1_AUDIO_DATA_EP_DESC_LEN,                         /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x01,                                                    /* bmAttributes */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
	USBD_UAC_PLAY_FEEDBACK_EP_DESC
#endif

#ifdef CONFIG_USBD_UAC_RECORD
	/* Mic AS interface */
	/* 4.5.1 Standard AS Interface Descriptor, alt setting 0: zero bandwidth */
	USB_LEN_IF_DESC,                                          /* bLength */
	USB_DESC_TYPE_INTERFACE,                                  /* bDescriptorType */
	USBD_UAC1_IF_IDX_AS_HEADSET_MICROPHONE,                   /* bInterfaceNumber */
	0x00,                                                     /* bAlternateSetting */
	0x00,                                                     /* bNumEndpoints */
	USB_UAC_IF_CLASS_AUDIO,                                   /* bInterfaceClass */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                          /* bInterfaceSubClass */
	0x00,                                                     /* bInterfaceProtocol */
	0x00,                                                     /* iInterface */

	/* 4.5.1 Standard AS Interface Descriptor, alt setting 1: streaming */
	USB_LEN_IF_DESC,                                          /* bLength */
	USB_DESC_TYPE_INTERFACE,                                  /* bDescriptorType */
	USBD_UAC1_IF_IDX_AS_HEADSET_MICROPHONE,                   /* bInterfaceNumber */
	0x01,                                                     /* bAlternateSetting */
	0x01,                                                     /* bNumEndpoints */
	USB_UAC_IF_CLASS_AUDIO,                                   /* bInterfaceClass */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                          /* bInterfaceSubClass */
	0x00,                                                     /* bInterfaceProtocol */
	0x00,                                                     /* iInterface */

	/* 4.5.2 Class-Specific AS General Interface Descriptor */
	USB_UAC1_AS_IF_DESC_LEN,                                  /* bLength */
	USB_UAC_CS_INTERFACE,                                     /* bDescriptorType */
	USB_UAC_AS_GENERAL,                                       /* bDescriptorSubtype */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE, /* bTerminalLink */
	0x01,                                                     /* bDelay */
	0x01,                                                     /* wFormatTag: PCM */
	0x00,

	/* Type I Format Type Descriptor with a single discrete rate, ref UAC 1.0 Format 2.2.5 */
	USBD_UAC1_AS_MIC_FRT_TYPE_DESC_LEN,                       /* bLength */
	USB_UAC_CS_INTERFACE,                                     /* bDescriptorType */
	USB_UAC_AS_FORMAT_TYPE,                                   /* bDescriptorSubtype */
	USB_UAC1_FORMAT_TYPE_I,                                   /* bFormatType */
	USBD_UAC_RECORD_CHANNEL_COUNT,                            /* bNrChannels */
	USBD_UAC_RECORD_BYTE_WIDTH,                               /* bSubframeSize */
	USBD_UAC_BIT_WIDTH(USBD_UAC_RECORD_BYTE_WIDTH),           /* bBitResolution */
	0x01,                                                     /* bSamFreqType: one discrete rate */
	USB_LOW_BYTE(USBD_UAC_RECORD_SAMPLING_FREQ),              /* tSamFreq[1] */
	USB_HIGH_BYTE(USBD_UAC_RECORD_SAMPLING_FREQ),
	(u8)((USBD_UAC_RECORD_SAMPLING_FREQ >> 16) & 0xFFU),

	/* 4.6.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	USB_UAC1_EP_DESC_LEN,                                     /* bLength */
	USB_DESC_TYPE_ENDPOINT,                                   /* bDescriptorType */
	USB_D2H,                                                  /* bEndpointAddress: dir IN (placeholder) */
	0x05,                                                     /* bmAttributes: isochronous, asynchronous */
	USB_LOW_BYTE(USBD_UAC_RECORD_FS_MPS),                     /* wMaxPacketSize: one ms of audio at both speeds */
	USB_HIGH_BYTE(USBD_UAC_RECORD_FS_MPS),
	USBD_UAC_FS_DEFAULT_BINTERVAL,                            /* bInterval: patched for the speed */
	0x00,                                                     /* bRefresh */
	0x00,                                                     /* bSynchAddress */

	/* 4.6.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	USB_UAC1_AUDIO_DATA_EP_DESC_LEN,                          /* bLength */
	USB_UAC_CS_ENDPOINT,                                      /* bDescriptorType */
	USB_UAC_AS_EP_GENERAL,                                    /* bDescriptorSubtype */
	USB_UAC1_EP_ATTR_SAMPLING_FREQ_CONTROL,                   /* bmAttributes: sampling frequency control */
	0x00,                                                     /* bLockDelayUnits */
	0x00,                                                     /* wLockDelay */
	0x00,
#endif
};
/* The length is computed by USBD_UAC_FS_CFG_DESC_BUF_LEN() from the channel count and the template is trimmed by
   #if on the same count: a mismatch would leave zero padding counted in wTotalLength */
_Static_assert(sizeof(usbd_uac_config_desc) == USBD_UAC_FS_CFG_DESC_BUF_LEN(USBD_UAC_PLAY_MAX_CHANNEL_COUNT), "UAC cfg desc len");

/* Exported variables --------------------------------------------------------*/

/* UAC Class Driver */
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
#ifdef CONFIG_USBD_COMPOSITE
	.set_interface_base = usbd_uac_set_interface_base,
#endif
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
  * @brief  Check whether an endpoint address belongs to this class
  * @param  cdev: UAC device instance
  * @param  ep_addr: Endpoint address taken from the low byte of wIndex
  * @retval 1 if owned, 0 otherwise
  */
static u8 usbd_uac_is_owned_ep(const usbd_uac_dev_t *cdev, u8 ep_addr)
{
	const usbd_uac_config_t *cfg = cdev->cfg;

	if ((cfg == NULL) || (cdev->cb == NULL) || (ep_addr == 0U)) {
		return 0U;
	}

	/* A class request to an endpoint only addresses the data EPs, ref UAC 1.0 5.2.3: the feedback
	   EP owns no control */
#ifdef CONFIG_USBD_UAC_RECORD
	return ((ep_addr == cfg->play_ep_addr) || (ep_addr == cfg->record_ep_addr)) ? 1U : 0U;
#else
	return (ep_addr == cfg->play_ep_addr) ? 1U : 0U;
#endif
}

/**
  * @brief  Check whether an interface number, local to this function, belongs to the class
  * @note   Composite mode already rebased wIndex to the local interface number.
  * @param  interface_number: Interface number from wIndex
  * @retval 1 if the interface is in the reported config descriptor, 0 otherwise
  */
static u8 usbd_uac_is_owned_interface(u16 interface_number)
{
	u8 owned;

	if ((interface_number == USB_UAC1_IF_IDX_AC_HEADSET) || (interface_number == USB_UAC1_IF_IDX_AS_HEADSET_HEADPHONES)) {
		owned = 1U;
#ifdef CONFIG_USBD_UAC_RECORD
	} else if (interface_number == USBD_UAC1_IF_IDX_AS_HEADSET_MICROPHONE) {
		owned = 1U;
#endif
	} else {
		owned = 0U;
	}

	return owned;
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
	/* The speed is only known once enumerated, the EP is opened later by usbd_uac_play_ep_arm() */
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
	   configured, so a failure of the app is reported and the core stalls the request. The app
	   goes first, so that nothing has been armed yet when it fails. */
	if ((cb != NULL) && (cb->set_config != NULL)) {
		if (cb->set_config() != RTK_SUCCESS) {
			return -RTK_ERR_IO;
		}
	}

	/* Published under the lock clear_config() resets it with, refer to usbd_uac_play_ep_arm() */
	usb_os_enter_critical();
	cdev->is_configured = 1U;
	usb_os_exit_critical();

	/* Try to arm the ISOC OUT EP immediately, matching composite's eager
	 * arm-at-set_config behavior. No-ops (-RTK_ERR_IO) until the app has
	 * called usbd_uac_start_play() at least once to size the ring buffer. */
	usbd_uac_play_ep_arm();

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
	usbd_ep_t *play_ep = &cdev->play_ep;
#ifdef CONFIG_USBD_UAC_RECORD
	usbd_ep_t *record_ep = &cdev->record_ep;
#endif

	UNUSED(config);

	/* Ref USB 2.0 9.1.1.3 / 9.4.10: after a bus reset or a de-configuration no alternate setting
	   is selected and the endpoints are gone. The SOF filler and the task APIs test this state
	   under the same lock, possibly on another core. streaming is kept: the stream resumes on
	   its own once the host selects the streaming setting again. */
	usb_os_enter_critical();

	/* Closes the EP arm path of the task APIs before the EPs go, refer to usbd_uac_play_ep_arm() */
	cdev->is_configured = 0U;
	cdev->play_alt_setting = 0U;
	cdev->record_alt_setting = 0U;
	cdev->play_stream.packet_count = 0U;
	cdev->play_stream.sof_count = 0U;
	cdev->play_stream.data_count = 0U;
	cdev->sof_count = 0U;
	/* A control transfer cut by the reset never gets its data stage */
	cdev->ctrl_req_pending = 0U;

	/* DeInit the ISOC EPs: registered only after usbd_uac_private_init() checked their endpoint
	   addresses, and cfg is kept until after unregistration */
	usbd_ep_deinit(dev, play_ep);

#ifdef CONFIG_USBD_UAC_RECORD
	usbd_ep_deinit(dev, record_ep);
	/* The aborted IN transfer never completes */
	record_ep->xfer_state = 0U;
#endif
#ifdef CONFIG_USBD_UAC_PLAY_FEEDBACK
	usbd_uac_play_feedback_close(dev);
#endif

	usb_os_exit_critical();
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
	/* Read once: usbd_uac_deinit() may clear it on another core */
	const usbd_uac_cb_t *cb = cdev->cb;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	int ret = RTK_SUCCESS;
	u8 entity_id;
	u8 control_selector;
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
				/* Ref USB 2.0 Table 9-10: the whole wIndex is the interface number. The AC
				   interface owns alternate setting 0 only and must not change the streaming
				   format, any other interface belongs to another function. */
				if (req->wIndex == USB_UAC1_IF_IDX_AC_HEADSET) {
					if (alt_setting != 0U) {
						ret = -RTK_ERR_BADARG;
					}
#ifdef CONFIG_USBD_UAC_RECORD
				} else if (req->wIndex == USBD_UAC1_IF_IDX_AS_HEADSET_MICROPHONE) {
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
				} else if (req->wIndex != USB_UAC1_IF_IDX_AS_HEADSET_HEADPHONES) {
					ret = -RTK_ERR_BADARG;
				} else if (alt_setting == 0U) {
					/* Ref USB 2.0 9.4.5: alt setting 0 is the zero-bandwidth setting, it stops the
					   stream and leaves the negotiated format untouched */
					cdev->play_alt_setting = 0U;
					/* Ref UAC 1.0 3.7.2.3: no data is sent on the zero-bandwidth setting. Disarm the SOF
					   underrun filler until the next real packet, else it keeps feeding the reader
					   synthetic nodes after the host stopped. The OUT EP stays armed so that the
					   next alt=1 streams at once, ISOC EPs have no halt or toggle state to reset. */
					cdev->play_stream.packet_count = 0U;
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
						cdev->play_alt_setting = alt_setting;/* Set new altsetting */
						if ((cdev->play_byte_width != byte_width) && (byte_width != 0)) {
							cdev->play_byte_width = byte_width;
							format_updated = 1;
						}
						if ((cdev->play_channel_count != channel_count) && (channel_count != 0)) {
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
			} else {
				ret = -RTK_ERR_BADARG;
			}
			break;

		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				/* Ref USB 2.0 Table 9-11: the whole wIndex is the interface number, and 9.4.4
				   requires a request error for an interface that does not exist */
				if (req->wIndex == USB_UAC1_IF_IDX_AC_HEADSET) {
					ep0_in->xfer_buf[0] = 0U;
				} else if (req->wIndex == USB_UAC1_IF_IDX_AS_HEADSET_HEADPHONES) {
					ep0_in->xfer_buf[0] = cdev->play_alt_setting;
#ifdef CONFIG_USBD_UAC_RECORD
				} else if (req->wIndex == USBD_UAC1_IF_IDX_AS_HEADSET_MICROPHONE) {
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
		/* Ref UAC 1.0 5.2.1: the low byte of wIndex addresses the interface or the endpoint the
		   control belongs to. Reject anything owned by another function, otherwise a composite
		   device may route a foreign request here, ref USB 2.0 Table 9-2. */
		if ((req->bmRequestType & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_INTERFACE) {
			if (USB_LOW_BYTE(req->wIndex) != USB_UAC1_IF_IDX_AC_HEADSET) {
				ret = -RTK_ERR_BADARG;
				break;
			}
		} else if ((req->bmRequestType & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_ENDPOINT) {
			if (usbd_uac_is_owned_ep(cdev, USB_LOW_BYTE(req->wIndex)) == 0U) {
				ret = -RTK_ERR_BADARG;
				break;
			}
		} else {
			ret = -RTK_ERR_BADARG;
			break;
		}

		entity_id = USB_HIGH_BYTE(req->wIndex);
		control_selector = USB_HIGH_BYTE(req->wValue);

#ifndef CONFIG_USBD_UAC_RECORD
		/* A playback only build declares no mic entity, so a request naming one addresses a
		   control that does not exist */
		if (entity_id == USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT) {
			ret = -RTK_ERR_BADARG;
			break;
		}
#endif

		/* Ref UAC 1.0 5.2.1.1: the low byte of wValue is the channel number for a Feature Unit
		   control and shall be 0 for an endpoint control. The Feature Unit only declares master
		   channel (0) controls, so any other channel addresses a control that does not exist. */
		if (USB_LOW_BYTE(req->wValue) != 0U) {
			ret = -RTK_ERR_BADARG;
			break;
		}

		if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {

			if (entity_id == USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT) {
				if (control_selector == USB_UAC_FU_MUTE) {
					if (req->bRequest == USB_UAC1_GET_CUR) {
						ep0_in->xfer_buf[0] = cdev->play_mute;
						ep0_in->xfer_len = 1U;
						ret = usbd_ep_transmit(dev, ep0_in);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 0);
						ret = -RTK_ERR_BADARG;
					}
				} else if (control_selector == USB_UAC_FU_VOLUME) {
					if (req->bRequest == USB_UAC1_GET_CUR) {
						usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *) & (cdev->play_volume), 2);
						ep0_in->xfer_len = 2U;
						ret = usbd_ep_transmit(dev, ep0_in);
					} else if (req->bRequest == USB_UAC1_GET_MIN) {
						u16 min_volume = (cdev->cfg->volume_min != 0U) ? cdev->cfg->volume_min : USBD_UAC_DEFAULT_VOLUME_MIN;
						usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)&min_volume, 2);
						ep0_in->xfer_len = 2U;
						ret = usbd_ep_transmit(dev, ep0_in);
					} else if (req->bRequest == USB_UAC1_GET_MAX) {
						u16 max_volume = (cdev->cfg->volume_max != 0U) ? cdev->cfg->volume_max : USBD_UAC_DEFAULT_VOLUME_MAX;
						usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)&max_volume, 2);
						ep0_in->xfer_len = 2U;
						ret = usbd_ep_transmit(dev, ep0_in);
					} else if (req->bRequest == USB_UAC1_GET_RES) {
						u16 res = 1U;
						usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)&res, 2);
						ep0_in->xfer_len = 2U;
						ret = usbd_ep_transmit(dev, ep0_in);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 1);
						ret = -RTK_ERR_BADARG;
					}
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 2);
					ret = -RTK_ERR_BADARG;
				}
			} else if (entity_id == USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT) {
				/* Mic Feature Unit, the same control ranges as the headphone one */
				if (control_selector == USB_UAC_FU_MUTE) {
					if (req->bRequest == USB_UAC1_GET_CUR) {
						ep0_in->xfer_buf[0] = cdev->record_mute;
						ep0_in->xfer_len = 1U;
						ret = usbd_ep_transmit(dev, ep0_in);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 20);
						ret = -RTK_ERR_BADARG;
					}
				} else if (control_selector == USB_UAC_FU_VOLUME) {
					u16 volume = 0U;

					if (req->bRequest == USB_UAC1_GET_CUR) {
						volume = cdev->record_volume;
					} else if (req->bRequest == USB_UAC1_GET_MIN) {
						volume = (cdev->cfg->volume_min != 0U) ? cdev->cfg->volume_min : USBD_UAC_DEFAULT_VOLUME_MIN;
					} else if (req->bRequest == USB_UAC1_GET_MAX) {
						volume = (cdev->cfg->volume_max != 0U) ? cdev->cfg->volume_max : USBD_UAC_DEFAULT_VOLUME_MAX;
					} else if (req->bRequest == USB_UAC1_GET_RES) {
						volume = 1U;
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 21);
						ret = -RTK_ERR_BADARG;
					}
					if (ret == RTK_SUCCESS) {
						usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)&volume, 2);
						ep0_in->xfer_len = 2U;
						ret = usbd_ep_transmit(dev, ep0_in);
					}
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 22);
					ret = -RTK_ERR_BADARG;
				}
			} else if ((req->bmRequestType & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_ENDPOINT) {
				if (control_selector == USB_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USB_UAC1_GET_CUR) {
						/* Ref UAC 1.0 5.2.3.2.3.1: each AS endpoint has its own sampling frequency */
#ifdef CONFIG_USBD_UAC_RECORD
						if (USB_LOW_BYTE(req->wIndex) == cdev->cfg->record_ep_addr) {
							usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *) & (cdev->record_sampling_freq), 3);
						} else
#endif
						{
							usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *) & (cdev->play_sampling_freq), 3);
						}
						ep0_in->xfer_len = 3U;
						ret = usbd_ep_transmit(dev, ep0_in);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 3);
						ret = -RTK_ERR_BADARG;
					}
				} else {
					/* Ref UAC 1.0 5.2.3: no other endpoint control is supported, STALL instead
					   of leaving the data stage unanswered */
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 11);
					ret = -RTK_ERR_BADARG;
				}
			} else {
				USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 10);
				ret = -RTK_ERR_BADARG;
			}
		} else {
			/* USB_H2D */
			if ((req->bmRequestType & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_ENDPOINT) {
				if (control_selector == USB_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USB_UAC1_SET_CUR) {
						/* Ref UAC 1.0 5.2.3.2.3.1: 3-byte sampling frequency */
						ret = usbd_uac_receive_control_data(dev, req, 3U);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 4);
						ret = -RTK_ERR_BADARG;
					}
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 5);
					ret = -RTK_ERR_BADARG;
				}
			} else if (entity_id == USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT) { //0x05 FU
				if (control_selector == USB_UAC_FU_MUTE) { //mute
					if (req->bRequest == USB_UAC1_SET_CUR) {
						/* Ref UAC 1.0 5.2.2.4.3.1: 1-byte bMute */
						ret = usbd_uac_receive_control_data(dev, req, 1U);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 6);
						ret = -RTK_ERR_BADARG;
					}
				} else if (control_selector == USB_UAC_FU_VOLUME) { //volume
					if (req->bRequest == USB_UAC1_SET_CUR) {
						/* Ref UAC 1.0 5.2.2.4.3.2: 2-byte wVolume */
						ret = usbd_uac_receive_control_data(dev, req, 2U);
					} else {
						/* Set cur volume range err */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 7);
						ret = -RTK_ERR_IO;
					}
				} else {
					/* Set fu err */
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 8);
					ret = -RTK_ERR_IO;
				}
			} else if (entity_id == USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT) { //0x08 mic FU
				if ((req->bRequest == USB_UAC1_SET_CUR) && (control_selector == USB_UAC_FU_MUTE)) {
					/* Ref UAC 1.0 5.2.2.4.3.1: 1-byte bMute */
					ret = usbd_uac_receive_control_data(dev, req, 1U);
				} else if ((req->bRequest == USB_UAC1_SET_CUR) && (control_selector == USB_UAC_FU_VOLUME)) {
					/* Ref UAC 1.0 5.2.2.4.3.2: 2-byte wVolume */
					ret = usbd_uac_receive_control_data(dev, req, 2U);
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 23);
					ret = -RTK_ERR_BADARG;
				}
			} else {
				/* Unknown entity or recipient, STALL instead of leaving the data stage unarmed */
				USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 12);
				ret = -RTK_ERR_BADARG;
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
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 9);
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
	u8 is_frame = 1U;

	/* High Speed raises a SOF every microframe, the stream carries one packet per ms */
	if (dev->dev_speed == USB_SPEED_HIGH) {
		cdev->sof_count++;
		if (cdev->sof_count < USBD_UAC_HS_SOF_COUNT_PER_MS) {
			is_frame = 0U;
		} else {
			cdev->sof_count = 0U;
		}
	}

	/* Track OUT stream underrun: if host stalls the ISOC pipe, synthesise
	 * ZLP filler nodes so blocked readers do not stall forever. */
	if ((is_frame != 0U) && (stream->streaming != 0) && (stream->packet_count > 0)) {
		if (stream->sof_count == 0) {
			stream->sof_count = 1;
			stream->data_count = 0;
		} else {
			stream->sof_count ++;
		}

		if ((stream->sof_count > stream->data_count)
			&& ((stream->sof_count - stream->data_count) >= USBD_UAC_PLAY_FILLER_THRESHOLD)) {
			usbd_uac_append_filler(stream);
			stream->data_count ++;
		}
	}

#ifdef CONFIG_USBD_UAC_RECORD
	/* One record packet per ms, the IN completion retriggers the next one when data is queued */
	if ((is_frame != 0U) && (cdev->record_stream.streaming != 0) && (cdev->record_ep.xfer_state == 0)) {
		usbd_uac_isoc_in_transmit_next(dev);
	}
#endif
#ifdef CONFIG_USBD_UAC_PLAY_FEEDBACK
	/* One feedback per ms while the host streams playback */
	if (is_frame != 0U) {
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

	if (((ctrl_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS) && (ctrl_req->bRequest == USB_UAC1_SET_CUR)) {
		if ((USB_HIGH_BYTE(ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT &&
			 ((ctrl_req->bmRequestType & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_INTERFACE))) {
			if ((USB_HIGH_BYTE(ctrl_req->wValue) == USB_UAC_FU_MUTE) && (ctrl_req->wLength == 0x01)) {
				/* Ref UAC 1.0 5.2.2.4.3.1: GET_CUR reports bMute back, cache it whether or not the app listens */
				cdev->play_mute = (ep0_out->xfer_buf[0] != 0U) ? 1U : 0U;
				if (cb->mute_changed != NULL) {
					cb->mute_changed(cdev->play_mute);
				}
			} else if ((USB_HIGH_BYTE(ctrl_req->wValue) == USB_UAC_FU_VOLUME) && (ctrl_req->wLength == 0x02)) {
				volume_value = usbd_uac_le16_to_s16(ep0_out->xfer_buf);

				target_volume = usbd_uac_volume_linear_interpolation(volume_value);

				if (target_volume != USBD_UAC_VOLUME_ERR_VAL) {
					/* Ref UAC 1.0 5.2.2.4.3.2: GET_CUR reports wVolume back, cache it whether or not the app listens */
					cdev->play_volume = (u16)volume_value;
					if (cb->volume_changed != NULL) {
						cb->volume_changed(target_volume);
					}
				} else {
					/* Outside the reported range, or 0x8000 (-inf dB): UAC 1.0 5.2.2.4.3.2 defines no
					   error for it, so the request is acknowledged and the volume kept; GET_CUR then
					   reports the wVolume actually in use. A STALL would also reject the legal 0x8000. */
				}
			}
		} else if ((USB_HIGH_BYTE(ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT) &&
				   ((ctrl_req->bmRequestType & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_INTERFACE)) {
			/* Mic Feature Unit: cached for GET_CUR, the record path is not scaled by the class */
			if ((USB_HIGH_BYTE(ctrl_req->wValue) == USB_UAC_FU_MUTE) && (ctrl_req->wLength == 0x01)) {
				cdev->record_mute = (ep0_out->xfer_buf[0] != 0U) ? 1U : 0U;
			} else if ((USB_HIGH_BYTE(ctrl_req->wValue) == USB_UAC_FU_VOLUME) && (ctrl_req->wLength == 0x02)) {
				cdev->record_volume = (u16)ep0_out->xfer_buf[0] + ((u16)(ep0_out->xfer_buf[1]) << 8);
			}
		}

		if (((ctrl_req->bmRequestType & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_ENDPOINT)
			&& (USB_HIGH_BYTE(ctrl_req->wValue) == USB_UAC_CS_SAM_FREQ_CONTROL) && (ctrl_req->wLength == 0x03)) {
			freq = ((u32)ep0_out->xfer_buf[2] << 16) | ((u32)ep0_out->xfer_buf[1] << 8) | (u32)ep0_out->xfer_buf[0];
#ifdef CONFIG_USBD_UAC_RECORD
			if (USB_LOW_BYTE(ctrl_req->wIndex) == cdev->cfg->record_ep_addr) {
				/* The mic declares a single rate, so the mic EP keeps its packet size */
				if (freq == USBD_UAC_RECORD_SAMPLING_FREQ) {
					cdev->record_sampling_freq = freq;
				} else {
					/* Ref USB 2.0 9.2.7: an unsupported value is a request error, STALL the status stage */
					ret = -RTK_ERR_BADARG;
				}
			} else
#endif
				if (usbd_uac_is_valid_sample_rate(freq, dev->dev_speed)) {
					if (cdev->play_sampling_freq != freq) {
						/* The OUT EP and the ring are sized for the highest rate, refer to
						   usbd_uac_stream_init(), so a packet at the new rate already fits: the stream is
						   left running and the application restarts it with the new format. */
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
  * @brief  Patch the runtime-assigned fields in a configuration descriptor block
  * @note   Replaces direction-only EP placeholders (USB_D2H/USB_H2D) with the actual
  *         EP addresses from the class configuration, rewrites the iInterface string index,
  *         and rebases the AC Header baInterfaceNr[]
  *         cross-references with the current interface base (0 unless the composite
  *         framework rebased it), which the framework does not touch: it only rebases
  *         the standard Interface and IAD descriptors.
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
			if ((dir == USB_H2D) && (type == USB_CH_EP_TYPE_ISOC)) {
				desc[i + 2] = cfg->play_ep_addr;
				/* Ref UAC 1.0 4.6.1.1: one packet per ms whatever the speed, the wMaxPacketSize stays */
				desc[i + 6] = usbd_uac_get_binterval(speed);   /* bInterval */
#ifdef CONFIG_USBD_UAC_PLAY_FEEDBACK
				if (desc_len >= USB_UAC1_EP_DESC_LEN) {
					desc[i + 8] = cfg->play_feedback_ep_addr;     /* bSynchAddress */
				}
			} else if ((dir == USB_D2H) && (desc[i + 3] == USBD_UAC_PLAY_FEEDBACK_EP_ATTR)) {
				/* Ref USB 2.0 5.12.4.2: 10.14 in 3 bytes at Full Speed, 16.16 in 4 at High Speed */
				desc[i + 2] = cfg->play_feedback_ep_addr;
				desc[i + 4] = (speed == USB_SPEED_HIGH) ? USBD_UAC_PLAY_FEEDBACK_HS_MPS : USBD_UAC_PLAY_FEEDBACK_MPS;   /* wMaxPacketSize */
				desc[i + 6] = usbd_uac_get_play_feedback_binterval(speed);   /* bInterval */
#endif
#ifdef CONFIG_USBD_UAC_RECORD
			} else if ((dir == USB_D2H) && (type == USB_CH_EP_TYPE_ISOC)) {
				desc[i + 2] = cfg->record_ep_addr;
				desc[i + 6] = usbd_uac_get_binterval(speed);   /* bInterval */
#endif
			}
		} else if ((desc_type == USB_DESC_TYPE_INTERFACE) && (desc_len >= USB_LEN_IF_DESC)) {
			if (desc[i + 6] == USB_UAC_SUBCLASS_AUDIOCONTROL) {
				/* Only the audio control interface is named, matching the CDC ACM class. */
				desc[i + 8] = cdev->interface_string_index;     /* iInterface */
			}
		} else if ((desc_type == USB_UAC_CS_INTERFACE) && (desc_len >= USB_UAC1_AC_IF_HEADER_LEN) && (desc[i + 2] == USB_UAC_AC_HEADER)) {
			/* AC Header: bInCollection at offset 7, baInterfaceNr[j] at offset 8+j
			 * (Ref UAC 1.0 4.7.2). Each entry keeps its own class-local interface
			 * number, so add the base rather than overwrite. */
			u8 n = desc[i + 7];
			u8 j;

			if ((u16)(8U + n) > (u16)desc_len) { /* malformed bInCollection: do not run off the descriptor */
				n = (u8)(desc_len - 8U);
			}
			for (j = 0; j < n; j++) {
				desc[i + 8U + j] = (u8)(desc[i + 8U + j] + cdev->if_base);
			}
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
		   rewriting it here would race the OUT ISR that arms the EP with it */
		desc = usbd_uac_config_desc;
		len = sizeof(usbd_uac_config_desc);
		is_cfg = 1;
#ifndef CONFIG_SUPPORT_USB_FS_ONLY
		desc_speed = dev->dev_speed;
#endif
		break;

#ifndef CONFIG_SUPPORT_USB_FS_ONLY
	/* The core stalls both unless the device is configured for High Speed */
	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		desc = usbd_uac_device_qualifier_desc;
		len = sizeof(usbd_uac_device_qualifier_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		/* Ref USB 2.0 9.6.4: describes the configuration at the speed the device is not
		   running at, so its bInterval is patched for that speed, not the current one */
		desc = usbd_uac_config_desc;
		len = sizeof(usbd_uac_config_desc);
		is_cfg = 1;
		desc_speed = (dev->dev_speed == USB_SPEED_HIGH) ? USB_SPEED_FULL : USB_SPEED_HIGH;
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

	if (is_cfg != 0) {
		buf[USB_CFG_DESC_OFFSET_TYPE] = type;
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);

		usbd_uac_patch_descriptor(buf + USB_LEN_CFG_DESC, len - USB_LEN_CFG_DESC, desc_speed);
	}

	return len;
}

#ifdef CONFIG_USBD_COMPOSITE
/**
  * @brief  Store the first interface number assigned to this class by the composite framework
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  base: First interface number of this class
  * @retval void
  */
static void usbd_uac_set_interface_base(u8 base)
{
	usbd_uac_dev.if_base = base;
}
#endif

#if USBD_UAC_DEBUG
#endif

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Get the ISOC packets per second, one per bInterval
  * @note   Ref UAC 1.0 4.6.1.1: one packet per ms whatever the speed, refer to usbd_uac_get_binterval()
  * @param  speed: USB connection speed
  * @retval Packets per second
  */
u16 usbd_uac_get_packets_per_second(u8 speed)
{
	UNUSED(speed);

	return USBD_UAC_ONE_KHZ;
}

#ifdef CONFIG_USBD_UAC_RECORD
/**
  * @brief  Get the MPS of the record stream: one ms of audio plus one frame
  * @note   Checked against the Full-Speed limit at build time, refer to USBD_UAC_RECORD_FS_MPS
  * @param  speed: USB connection speed, the packet is 1 ms long at both
  * @retval Max packet size
  */
u16 usbd_uac_get_record_max_packet_size(u8 speed)
{
	UNUSED(speed);

	return (u16)USBD_UAC_RECORD_FS_MPS;
}
#endif

/**
  * @brief  Get the size of a playback (ISOC OUT) ring node
  * @note   Ref UAC 1.0 4.6.1.2: the host may switch the alt setting and the rate at any time and then
  *         sends up to the wMaxPacketSize of the selected alt setting. The node is therefore sized once
  *         for the widest alt setting at the highest rate, checked against the FS limit at build time.
  * @param  speed: USB connection speed, a packet is 1 ms long at both
  * @retval Node size in bytes
  */
u16 usbd_uac_get_play_max_packet_size(u8 speed)
{
	UNUSED(speed);

	return (u16)USBD_UAC_PLAY_MPS;
}

/**
  * @brief  Check the class configuration against the UAC 1.0 descriptors
  * @param  cfg: Class configuration
  * @retval Status
  */
int usbd_uac_version_init(const usbd_uac_config_t *cfg)
{
	UNUSED(cfg);

	/* Standalone default; the composite framework rebases it via set_interface_base() */
	usbd_uac_dev.if_base = 0;

#if USBD_UAC_DESCRIPTOR_DUMP
	usbd_uac_dump_descriptor(usbd_uac_config_desc, sizeof(usbd_uac_config_desc) / sizeof(u8));
#endif

	return RTK_SUCCESS;
}

/**
  * @brief  Get the ISOC bInterval of one packet per ms at a speed
  * @param  speed: USB connection speed
  * @retval bInterval
  */
u8 usbd_uac_get_binterval(u8 speed)
{
	return (speed == USB_SPEED_HIGH) ? USBD_UAC_HS_DEFAULT_BINTERVAL : USBD_UAC_FS_DEFAULT_BINTERVAL;
}

#ifdef CONFIG_USBD_UAC_PLAY_FEEDBACK
/**
  * @brief  Get the bInterval of the feedback EP, one feedback per ms as the data EP
  * @param  speed: USB connection speed
  * @retval bInterval
  */
u8 usbd_uac_get_play_feedback_binterval(u8 speed)
{
	return usbd_uac_get_binterval(speed);
}
#endif

/**
  * @brief  Check freq requests
  * @param  freq: Freq requests from host
  * @param  speed: USB connection speed
  * @retval Status
  */
bool usbd_uac_is_valid_sample_rate(u32 freq, u8 speed)
{
	u8 i;

	UNUSED(speed);

	for (i = 0; i < USBD_UAC_SAMPLING_FREQ_MAX_COUNT; ++i) {
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

	if ((stream->max_packet_size == 0) || (stream->sema_valid == 0)) {
		return -RTK_ERR_IO;
	}

	/* One critical section with the configured check: a bus reset or SET_CONFIGURATION(0) handled
	   by the ISR on another core runs clear_config(), which must not be undone by an EP armed right
	   after it. dev->is_ready can not tell: the core clears it only after clear_config() returned,
	   and not at all on a bus reset. None of the calls below blocks, they nest the same lock. */
	usb_os_enter_critical();
	if (cdev->is_configured != 0U) {
		usb_ringbuf_reset(&(stream->ring));
		stream->sof_count = 0;
		stream->data_count = 0;
		stream->packet_count = 0;
		stream->streaming = 1;
		/* Arm the ISOC OUT EP hardware. Only the SET_CUR SAMPLING_FREQ
		 * request path ep_inits the EP, and some UAC1 hosts skip that class
		 * request when the AS descriptor already lists a discrete sample rate,
		 * so we do it here where mps is known-good. deinit+init is idempotent —
		 * a fresh EP is a no-op deinit, and re-init on an already-open EP
		 * simply reprograms the FIFO for the new mps. */
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
