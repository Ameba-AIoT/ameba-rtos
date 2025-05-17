/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

/* uac 1.0 spec */
#include "usbd_composite_uac1.h"
#include "os_wrapper.h"

#if USBD_UAC_ISOC_XFER_DEBUG
#if defined(CONFIG_ARM_CORE_CA32) && CONFIG_ARM_CORE_CA32
#include "arch_timer.h"
#endif
#endif

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

#if USBD_UAC_ISOC_XFER_DEBUG
#define USBD_UAC_DEBUG_LOOP_TIME   1000
#endif

#define UABD_UAC_VOL_ERR_VAL       255

/* Private function prototypes -----------------------------------------------*/
static int usbd_composite_uac_set_config(usb_dev_t *dev, u8 config);
static int usbd_composite_uac_clear_config(usb_dev_t *dev, u8 config);
static int usbd_composite_uac_setup(usb_dev_t *dev, usb_setup_req_t *req);
static int usbd_composite_uac_sof(usb_dev_t *dev);
static u8 *usbd_composite_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len);
static int usbd_composite_uac_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_composite_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);
static int usbd_composite_uac_handle_ep0_data_out(usb_dev_t *dev);
static void usbd_composite_uac_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
#if USBD_UAC_ISOC_XFER_DEBUG
static void usbd_composite_uac_status_dump_thread(void *param);
static inline void usbd_composite_uac_get_audio_data_cnt(u32 audio_len);
#endif
static inline u16 usbd_composite_uac_get_ring_buf_cnt(u8 speed);
static u16 usbd_composite_uac_get_mps(usbd_audio_cfg_t *params, u8 speed);
static inline u8 usbd_composite_uac_ep_enable(usbd_audio_cfg_t *ep);
static int usbd_composite_uac_is_valid_sample_rate(u32 sampling_freq, u8 speed);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "COMP";

static u32 usbd_composite_uac_sampling_freq[USBD_UAC_SAMPLING_FREQ_MAX_COUNT] = {USBD_UAC_SAMPLING_FREQ_48K};

/* UAC interface descriptor */
static u8 usbd_composite_uac_fs_itf_desc[] USB_DMA_ALIGNED = {
	/* 4.6 Interface Association Descriptor */
	/* IAD Descriptor */
	USB_LEN_IAD_DESC,                  /* Size of this descriptor (byte_width) */
	USBD_UAC_IA_DESCRIPTOR,            /* Interface Association Descriptor type */
	USBD_COMP_UAC_AC_HEADSET,          /* First Index: Audio Control Interface Index - Headset (0) */
	USBD_UAC_AC_IF_NUM,                /* Audio Control Interface, Audio Streaming Interfaces () */
	USBD_UAC_IF_CLASS_AUDIO,           /* Audio Device Class  */
	0x00,                              /* No subclass */
	0x00,                              /* Audio Protocol IP version 2.00 */
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
	0x00,                              /* Audio Protocol IP version 2.00 */
	0x00,                              /* Interface string descriptor index (0) */

	/* 4.7.2 Class-Specific AC Interface Descriptor */
	/* Audio headset */
	USBD_UAC_LEN_CTRL_IF_HEADER,                                         /* Size of the descriptor, in byte_width  */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                               /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AC_IF_DESC_SUBTYPE_HEADER,                                  /* HEADER descriptor subtype  */
	USB_LOW_BYTE(USBD_UAC_VERSION_01_10),                                /* bcdUSB */
	USB_HIGH_BYTE(USBD_UAC_VERSION_01_10),
	USB_LOW_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT)),
	USB_HIGH_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT)),/* Total number of byte_width returned for the class-specific AudioControl interface descriptor. Includes
                                                                         the combined length of this descriptor header and all Unit and Terminal descriptors.   */
	0x01,                                                                 /* bInCollection 1 means only speaker*/
	0x01,                                                                 /* baInterfaceNr[1] */

	/* Microphone Audio Control Input Terminal Descriptor 1.0 */
	USBD_UAC_LEN_AC_IN_TTY_DESC,                             /* Size of the descriptor, in byte_width  */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AC_IF_DESC_SUBTYPE_INPUT_TERMINAL,              /* INPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* Constant uniquely identifying the Terminal within the audio
                                                                  function. This value is used in all requests to address this Terminal.   */
	0x01,
	0x01,                                                    /* A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The
                                                                AudioStreaming interface descriptor points to the associated Terminal through the bTerminalLink field. */
	0x00,                                                    /* This Input Terminal has no association   */
	USBD_UAC_DEFAULT_CH_CNT,                                 /* This Terminal's output audio channel cluster has 8 logical output channels   */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_DEFAULT_CH_CNT),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_DEFAULT_CH_CNT),
	0x00,                                                    /* iChannelNames */
	0x00,                                                    /* Index of a string descriptor, describing the Input Terminal.  */

	/* FEATURE_UNIT descriptor subtype */
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT),   /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AC_IF_DESC_SUBTYPE_FEATURE_UNIT,                /* FEATURE_UNIT descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT,       /* Constant uniquely identifying the Unit within the audio function. This
                                                                value is used in all requests to address this Unit.  */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* ID of the Unit or Terminal to which this Feature Unit is connected. */
	0x01,                                                     /* bControlSize */
	0x03,                                                     /* bmaControls[0]  D0: Mute: 1 (yes) D1: Volume: 1 (yes)*/
	0x00,                                                     /* bmaControls[1] */
	0x00,                                                     /* bmaControls[2] */
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	0X00,                                                     /* bmaControls[3 */
	0x00,                                                     /* bmaControls[4] */
#endif
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	0x00,                                                     /* bmaControls[5] */
	0X00,                                                     /* bmaControls[6] */
#endif
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
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
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT,        /* bSourceID,ID of the Unit or Terminal to which this Terminal is connected.  */
	0x00,                                                     /* iTerminal  */

	/* 4.9.1 Standard AS Interface Descriptor 1/0*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Size of the descriptor, in byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USBD_COMP_UAC_AS_HEADSET_HEADPHONES,                      /* The number of this interface is 1.   */
	0x00,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x00,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USBD_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	0x00,                                                     /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.1 Standard AS Interface Descriptor 1/1*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Size of the descriptor, in byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USBD_COMP_UAC_AS_HEADSET_HEADPHONES,                      /* The number of this interface is 1.   */
	0x01,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USBD_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	0x00,                                                     /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AS_IF_DESC_SUBTYPE_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* The Terminal ID of the terminal to which this interface is connected */
	0x01,                                                    /* bDelay */
	0x01,                                                    /* wFormatTag 2 bytes: PCM 0x0001*/
	0x00,

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 1.0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AS_IF_DESC_SUBTYPE_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_CH_CNT_2,                                 /* bNrChannels */
	USBD_UAC_BYTE_WIDTH_2,                                   /* bSubframeSize */
	USBD_UAC_BIT_WIDTH(USBD_UAC_BYTE_WIDTH_2),               /* bBitResolution */
	0x1,                                                     /* bSamFreqType(supports 1 sample frequence) */
	USB_LOW_BYTE(USBD_UAC_SAMPLING_FREQ_48K),                                                    /* tSamFreq[1] 3 bytes (48000 Hz) */
	USB_HIGH_BYTE(USBD_UAC_SAMPLING_FREQ_48K),
	0x00,

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USBD_UAC_LEN_EP_DESC,                                    /* Size of the descriptor, in byte_width   */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_COMP_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 4   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_BYTE_WIDTH_2, USBD_UAC_SAMPLING_FREQ_48K)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_BYTE_WIDTH_2, USBD_UAC_SAMPLING_FREQ_48K)),
	USBD_UAC_FS_DEFAULT_BINTERVAL,                           /* bInterval */
	0x00,                                                    /* bRefresh */
	0x00,                                                    /* bSynchAddress */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USBD_UAC_LEN_DATA_EP_DESC,                               /* Size of the descriptor, in byte_width   */
	USBD_UAC_CS_ENDPOINT_DESCRIPTOR,                         /* Class-specific Endpoint Descriptor type */
	USBD_UAC_AS_EP_DESC_SUBTYPE_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x01,                                                    /* bmAttributes */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */

#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/2*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Size of the descriptor, in byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USBD_COMP_UAC_AS_HEADSET_HEADPHONES,                      /* The number of this interface is 1.   */
	0x02,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USBD_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	0x00,                                                     /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AS_IF_DESC_SUBTYPE_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* The Terminal ID of the terminal to which this interface is connected */
	0x01,                                                    /* bDelay */
	0x01,                                                    /* wFormatTag 2 bytes: PCM 0x0001*/
	0x00,

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 1.0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AS_IF_DESC_SUBTYPE_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_CH_CNT_4,                                 /* bNrChannels */
	USBD_UAC_BYTE_WIDTH_2,                                   /* bSubframeSize */
	USBD_UAC_BIT_WIDTH(USBD_UAC_BYTE_WIDTH_2),               /* bBitResolution */
	0x1,                                                     /* bSamFreqType(supports 1 sample frequence) */
	USB_LOW_BYTE(USBD_UAC_SAMPLING_FREQ_48K),                /* tSamFreq[1] 3 bytes (48000 Hz) */
	USB_HIGH_BYTE(USBD_UAC_SAMPLING_FREQ_48K),
	0x00,

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USBD_UAC_LEN_EP_DESC,                                    /* Size of the descriptor, in byte_width   */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_COMP_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 4   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_BYTE_WIDTH_2, USBD_UAC_SAMPLING_FREQ_48K)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_BYTE_WIDTH_2, USBD_UAC_SAMPLING_FREQ_48K)),
	USBD_UAC_FS_DEFAULT_BINTERVAL,                           /* bInterval */
	0x00,                                                    /* bRefresh */
	0x00,                                                    /* bSynchAddress */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USBD_UAC_LEN_DATA_EP_DESC,                               /* Size of the descriptor, in byte_width   */
	USBD_UAC_CS_ENDPOINT_DESCRIPTOR,                         /* Class-specific Endpoint Descriptor type */
	USBD_UAC_AS_EP_DESC_SUBTYPE_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x01,                                                    /* bmAttributes */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif

#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/3*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Size of the descriptor, in byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USBD_COMP_UAC_AS_HEADSET_HEADPHONES,                      /* The number of this interface is 1.   */
	0x03,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USBD_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	0x00,                                                     /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AS_IF_DESC_SUBTYPE_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* The Terminal ID of the terminal to which this interface is connected */
	0x01,                                                    /* bDelay */
	0x01,                                                    /* wFormatTag 2 bytes: PCM 0x0001*/
	0x00,

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 1.0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AS_IF_DESC_SUBTYPE_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_CH_CNT_6,                                 /* bNrChannels */
	USBD_UAC_BYTE_WIDTH_2,                                   /* bSubframeSize */
	USBD_UAC_BIT_WIDTH(USBD_UAC_BYTE_WIDTH_2),               /* bBitResolution */
	0x1,                                                     /* bSamFreqType(supports 1 sample frequence) */
	USB_LOW_BYTE(USBD_UAC_SAMPLING_FREQ_48K),                /* tSamFreq[1] 3 bytes (48000 Hz) */
	USB_HIGH_BYTE(USBD_UAC_SAMPLING_FREQ_48K),
	0x00,

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USBD_UAC_LEN_EP_DESC,                                    /* Size of the descriptor, in byte_width   */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_COMP_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 4   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_BYTE_WIDTH_2, USBD_UAC_SAMPLING_FREQ_48K)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_BYTE_WIDTH_2, USBD_UAC_SAMPLING_FREQ_48K)),
	USBD_UAC_FS_DEFAULT_BINTERVAL,                           /* bInterval */
	0x00,                                                    /* bRefresh */
	0x00,                                                    /* bSynchAddress */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USBD_UAC_LEN_DATA_EP_DESC,                               /* Size of the descriptor, in byte_width   */
	USBD_UAC_CS_ENDPOINT_DESCRIPTOR,                         /* Class-specific Endpoint Descriptor type */
	USBD_UAC_AS_EP_DESC_SUBTYPE_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x01,                                                    /* bmAttributes */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif

#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/4*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Size of the descriptor, in byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USBD_COMP_UAC_AS_HEADSET_HEADPHONES,                      /* The number of this interface is 1.   */
	0x04,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USBD_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	0x00,                                                     /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AS_IF_DESC_SUBTYPE_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* The Terminal ID of the terminal to which this interface is connected */
	0x01,                                                    /* bDelay */
	0x01,                                                    /* wFormatTag 2 bytes: PCM 0x0001*/
	0x00,

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 1.0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AS_IF_DESC_SUBTYPE_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_CH_CNT_8,                                 /* bNrChannels */
	USBD_UAC_BYTE_WIDTH_2,                                   /* bSubframeSize */
	USBD_UAC_BIT_WIDTH(USBD_UAC_BYTE_WIDTH_2),               /* bBitResolution */
	0x1,                                                     /* bSamFreqType(supports 1 sample frequence) */
	USB_LOW_BYTE(USBD_UAC_SAMPLING_FREQ_48K),                                                    /* tSamFreq[1] 3 bytes (48000 Hz) */
	USB_HIGH_BYTE(USBD_UAC_SAMPLING_FREQ_48K),
	0x00,

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USBD_UAC_LEN_EP_DESC,                                    /* Size of the descriptor, in byte_width   */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_COMP_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 4   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_BYTE_WIDTH_2, USBD_UAC_SAMPLING_FREQ_48K)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_BYTE_WIDTH_2, USBD_UAC_SAMPLING_FREQ_48K)),
	USBD_UAC_FS_DEFAULT_BINTERVAL,                           /* bInterval */
	0x00,                                                    /* bRefresh */
	0x00,                                                    /* bSynchAddress */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USBD_UAC_LEN_DATA_EP_DESC,                               /* Size of the descriptor, in byte_width   */
	USBD_UAC_CS_ENDPOINT_DESCRIPTOR,                         /* Class-specific Endpoint Descriptor type */
	USBD_UAC_AS_EP_DESC_SUBTYPE_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x01,                                                    /* bmAttributes */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif
};  /* usbd_composite_uac_fs_config_desc */

usbd_class_driver_t usbd_composite_uac_driver = {
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

static usbd_composite_uac_device_t usbd_composite_uac_device;

/* UAC volume data from windows10 PC*/
/* Different OS transmit the same vol level, but the corresponding driver vol values are different. */
static u8 usbd_composite_uac_pc_vol_lvl[] = {0, 5, 10, 20, 30, 40, 50, 60, 65, 75, 80, 85, 90, 95, 100};
static s16 usbd_composite_uac_drv_vol[] = {-190, -170, -151, -112, -74, -37, 0, 37, 56, 93, 112, 132, 151, 170, 190};

/* Private functions ---------------------------------------------------------*/

#if USBD_UAC_ISOC_XFER_DEBUG
u32 usbd_composite_uac_get_timetick(void) //ms
{
#if defined(CONFIG_ARM_CORE_CA32) && CONFIG_ARM_CORE_CA32
	return (u32)((arm_arch_timer_count() / 50) & (0xFFFFFFFF));
#else
	return DTimestamp_Get();
#endif
}
#endif

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

#if USBD_UAC_ISOC_XFER_DEBUG
		uac->isoc_overwrite_cnt ++;
#endif
	}

	p_buf = usbd_composite_uac_list_remove_head(&(pdata_ctrl->empty_list));
	if (p_buf) {
		p_buf->is_zero_pkt = 1;
		p_buf->buf_valid_len = uac->isoc_rx_len;
#if USBD_UAC_ISOC_XFER_DEBUG
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
	UNUSED(speed);
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
	UNUSED(speed);
	if (NULL == params) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Param err\n");
		return 0;
	}

	/* for 44.1khz or the host clk is bigger than the device */
	mps_value = USBD_UAC_CALC_FS_MPS(params->ch_cnt, params->byte_width, params->sampling_freq);
	if ((mps_value == 0) || (mps_value > USBD_UAC_FS_ISOC_MPS)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "MPS %d : zero or exceed FS limited %d\n", mps_value, USBD_UAC_FS_ISOC_MPS);
		return 0;
	}

	// RTK_LOGS(TAG, RTK_LOG_DEBUG, "Ch %d bw %d sampling_freq %d speed %d mps %d\n", params->ch_cnt , params->byte_width , params->sampling_freq, speed, mps_value);
	return mps_value;
}

/**
  * @brief  Deinitialize UAC endpoint buffer control structure
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @retval void
  */
static void usbd_composite_uac_ep_buf_ctrl_deinit(usbd_composite_uac_buf_ctrl_t *buf_ctrl)
{
	buf_ctrl->isoc_mps = 0;
	buf_ctrl->transfer_continue = 0;
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
	UNUSED(speed);

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
		buf_ctrl->isoc_mps = usbd_composite_uac_get_mps(params, USB_SPEED_FULL);
		if (buf_ctrl->isoc_mps == 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "MPS check fail\n");
			return HAL_ERR_PARA;
		}

		buf_list_cnt = usbd_composite_uac_get_ring_buf_cnt(USB_SPEED_FULL);
#if USBD_UAC_ISOC_XFER_DEBUG
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Buf mps len %d-%d(%d %d %d), cnt %d\n", buf_ctrl->isoc_mps, CACHE_LINE_ALIGMENT(buf_ctrl->isoc_mps), params->ch_cnt,
				 params->byte_width, params->sampling_freq, buf_list_cnt);
#endif

		buf_ctrl->isoc_buf = (u8 *)usb_os_malloc(CACHE_LINE_ALIGMENT(buf_ctrl->isoc_mps) * buf_list_cnt);
		if (buf_ctrl->isoc_buf == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Can not get isoc buf mem\n");
			return HAL_ERR_MEM;
		}
		memset(buf_ctrl->isoc_buf, 0x00, CACHE_LINE_ALIGMENT(buf_ctrl->isoc_mps) * buf_list_cnt);

		buf_ctrl->buf_list_node = (usbd_composite_uac_buf_t *)usb_os_malloc(sizeof(usbd_composite_uac_buf_t) * buf_list_cnt);
		if (buf_ctrl->buf_list_node == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Can not get isoc buf array mem\n");
			return HAL_ERR_MEM;
		}
		memset(buf_ctrl->buf_list_node, 0x00, sizeof(usbd_composite_uac_buf_t) * buf_list_cnt);

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
		pbuf_data->buf_raw = pbuf_ctrl->isoc_buf + CACHE_LINE_ALIGMENT(pbuf_ctrl->isoc_mps) * idx;
		usbd_composite_uac_list_add_tail(&(pbuf_ctrl->empty_list), pbuf_data);
	}

	//usbd_audio_cfg_t *paudio_cfg = &(pbuf_ctrl->audio_config);
	//RTK_LOGS(TAG, RTK_LOG_DEBUG, "Fmt %d-%d-%d speed %d mps %d-%d cnt %d\n", paudio_cfg->ch_cnt , paudio_cfg->byte_width , paudio_cfg->sampling_freq, cdev->dev->dev_speed, pbuf_ctrl->isoc_mps, CACHE_LINE_ALIGMENT(pbuf_ctrl->isoc_mps), buf_list_cnt);

	pbuf_data = usbd_composite_uac_list_remove_head(&(pbuf_ctrl->empty_list));
	if (NULL == pbuf_data) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "List is empty\n");
		return HAL_ERR_MEM;
	}

	usbd_ep_init(cdev->dev, USBD_COMP_UAC_ISOC_OUT_EP, USB_CH_EP_TYPE_ISOC, pbuf_ctrl->isoc_mps);
	usbd_ep_receive(cdev->dev, USBD_COMP_UAC_ISOC_OUT_EP, pbuf_data->buf_raw, pbuf_ctrl->isoc_mps);

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
	usbd_composite_uac_device_t *cdev = &usbd_composite_uac_device;

	UNUSED(config);

	/* DeInit ISOC IN EP */
	if (usbd_composite_uac_ep_enable(&(cdev->uac_isoc_in.audio_config))) {
		usbd_ep_deinit(dev, USBD_COMP_UAC_ISOC_IN_EP);
	}

	/* DeInit ISOC OUT EP */
	if (usbd_composite_uac_ep_enable(&(cdev->uac_isoc_out.audio_config))) {
		usbd_ep_deinit(dev, USBD_COMP_UAC_ISOC_OUT_EP);
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
	UNUSED(speed);

	for (u8 i = 0; i < USBD_UAC_SAMPLING_FREQ_MAX_COUNT; ++i) {
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
	usbd_composite_dev_t *cdev = uac->cdev;
	usbd_audio_cfg_t *audio_cfg = NULL;
	u8 entityId, controlSelector, ch_cnt, byte_width, alt_setting, fmt_change = 0;
	int ret = HAL_OK;

	// RTK_LOGS(TAG, RTK_LOG_INFO, "usbd_composite_uac_setup: bmRequestType=0x%02x bRequest=0x%02x wValue=%x wIndex=%x wLength=0x%04x\n",
	// 	req->bmRequestType, req->bRequest, req->wValue, req->wIndex, req->wLength);

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				alt_setting = (u8)(req->wValue & 0xFF);
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
					cdev->ctrl_buf[0] = uac->alt_setting;
					usbd_ep0_transmit(dev, cdev->ctrl_buf, 1U);
				} else {
					cdev->ctrl_buf[0] = 0U;
					usbd_ep0_transmit(dev, cdev->ctrl_buf, 1U);
				}
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		case USB_REQ_GET_STATUS:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				cdev->ctrl_buf[0] = 0U;
				cdev->ctrl_buf[1] = 0U;
				usbd_ep0_transmit(dev, cdev->ctrl_buf, 2U);
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

		//RTK_LOGS(TAG, RTK_LOG_DEBUG, "UAC SETUP: wIndex=0x%02x cs=%d\n", req->wIndex, controlSelector);

		if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {

			if (entityId == USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT) {
				if (controlSelector == USBD_UAC_CTRL_FU_MUTE_CONTROL_SELECTOR) {
					if (req->bRequest == USBD_UAC1_0_CLASS_GET_CUR) {
						dev->ctrl_buf[0] = uac->cur_mute;
						usbd_ep0_transmit(dev, dev->ctrl_buf, 1);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USBD_UAC_CTRL_FU_VOLUME_CONTROL_SELECTOR) {
					if (req->bRequest == USBD_UAC1_0_CLASS_GET_CUR) {
						memcpy(dev->ctrl_buf, (u8 *) & (uac->cur_volume), 2);
						usbd_ep0_transmit(dev, dev->ctrl_buf, 2);
					} else if (req->bRequest == USBD_UAC1_0_CLASS_GET_MIN) {
						u16 min_volume = USBD_UAC_VOLUME_CTRL_MIN;
						memcpy(dev->ctrl_buf, &min_volume, 2);
						usbd_ep0_transmit(dev, dev->ctrl_buf, 2);
					} else if (req->bRequest == USBD_UAC1_0_CLASS_GET_MAX) {
						u16 max_volume = USBD_UAC_VOLUME_CTRL_MAX;
						memcpy(dev->ctrl_buf, &max_volume, 2);
						usbd_ep0_transmit(dev, dev->ctrl_buf, 2);
					} else if (req->bRequest == USBD_UAC1_0_CLASS_GET_RES) {
						u16 res = 1U;
						memcpy(dev->ctrl_buf, &res, 2);
						usbd_ep0_transmit(dev, dev->ctrl_buf, 2);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: 0x13 wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
			} else if ((req->bmRequestType & 0x1FU) == USB_REQ_RECIPIENT_ENDPOINT) {
				if (controlSelector == USBD_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USBD_UAC1_0_CLASS_GET_CUR) {
						audio_cfg = &(uac->uac_isoc_out.audio_config);
						memcpy(dev->ctrl_buf, (u8 *) & (audio_cfg->sampling_freq), 3);
						usbd_ep0_transmit(dev, dev->ctrl_buf, 3);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				}
			} else {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest not handle: bmRequestType=0x%02x bRequest=0x%02x wValue=%x wIndex=%x wLength=0x%04x\n",
						 req->bmRequestType, req->bRequest, req->wValue, req->wIndex, req->wLength);
			}

		} else {
			/* USB_H2D */
			if ((req->bmRequestType & 0x1FU) == USB_REQ_RECIPIENT_ENDPOINT) {
				if (controlSelector == USBD_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USBD_UAC1_0_CLASS_SET_CUR) {
						usb_os_memcpy((void *)&uac->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						usbd_ep0_receive(dev, cdev->ctrl_buf, req->wLength);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "Set freq err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Set freq ctrl err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
			}

			if (entityId == USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT) { //0x05 FU
				if (controlSelector == USBD_UAC_CTRL_FU_MUTE_CONTROL_SELECTOR) { //mute
					if (req->bRequest == USBD_UAC1_0_CLASS_SET_CUR) {
						usb_os_memcpy((void *)&uac->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						usbd_ep0_receive(dev, cdev->ctrl_buf, req->wLength);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "Set cur mute err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USBD_UAC_CTRL_FU_VOLUME_CONTROL_SELECTOR) { //volume
					if (req->bRequest == USBD_UAC1_0_CLASS_SET_CUR) {
						usb_os_memcpy((void *)&uac->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						usbd_ep0_receive(dev, cdev->ctrl_buf, req->wLength);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "Set cur volume range err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Set fu err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_HW;
				}
			}
		}
		break;/* case USB_REQ_TYPE_CLASS */

	case USB_REQ_TYPE_VENDOR:
		if (req->wLength) {
			if ((req->bmRequestType & 0x80U) && (uac->cb->setup != NULL)) {
				ret = uac->cb->setup(req, cdev->ctrl_buf);
				if (ret == HAL_OK) {
					usbd_ep0_transmit(dev, cdev->ctrl_buf, req->wLength);
				}
			} else {
				usb_os_memcpy((void *)&uac->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
				usbd_ep0_receive(dev, cdev->ctrl_buf, req->wLength);
			}
		} else {
			if (uac->cb->setup != NULL) {
				uac->cb->setup(req, cdev->ctrl_buf);
			}
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

	if (pdata_ctrl->transfer_continue) {
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
	usbd_composite_dev_t *cdev = uac->cdev;
	usb_setup_req_t *p_ctrl_req = &(uac->ctrl_req);
	usbd_audio_cfg_t *audio_cfg = NULL;

	// RTK_LOGS(TAG, RTK_LOG_DEBUG, RTK_LOG_INFO, "EP0 Out: bmRequestType=0x%02x bRequest=0x%02x wValue=%x wIndex=%x wLength=0x%04x\n",
	// 	p_ctrl_req->bmRequestType, p_ctrl_req->bRequest, p_ctrl_req->wValue, p_ctrl_req->wIndex, p_ctrl_req->wLength);
	if (((p_ctrl_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS) && (p_ctrl_req->bRequest == USBD_UAC1_0_CLASS_SET_CUR)) {
		if ((USB_HIGH_BYTE(p_ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT &&
			 ((p_ctrl_req->bmRequestType & 0x1FU) == USB_REQ_RECIPIENT_INTERFACE))) {
			if ((USB_HIGH_BYTE(p_ctrl_req->wValue) == USBD_UAC_CTRL_FU_MUTE_CONTROL_SELECTOR) && (p_ctrl_req->wLength == 0x01)) {
				if (cb->mute_changed != NULL) {
					uac->cur_mute = (cdev->ctrl_buf[0]) ? 1 : 0;
					cb->mute_changed(uac->cur_mute);
				}
			} else if ((USB_HIGH_BYTE(p_ctrl_req->wValue) == USBD_UAC_CTRL_FU_VOLUME_CONTROL_SELECTOR) && (p_ctrl_req->wLength == 0x02)) {
				if (cb->volume_changed != NULL) {
					volume_value = (u16)cdev->ctrl_buf[0] + ((u16)(cdev->ctrl_buf[1]) << 8);
					uac->cur_volume = volume_value;

					RTK_LOGS(TAG, RTK_LOG_DEBUG, "Set volume 0x%02x\n", volume_value);

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

		if (((p_ctrl_req->bmRequestType & 0x1FU) == USB_REQ_RECIPIENT_ENDPOINT)
			&& (USB_HIGH_BYTE(p_ctrl_req->wValue) == USBD_UAC_CS_SAM_FREQ_CONTROL) && (p_ctrl_req->wLength == 0x03)) {
			sampling_freq = (cdev->ctrl_buf[2] << 16) | (cdev->ctrl_buf[1] << 8) | cdev->ctrl_buf[0];

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

	if ((pdata_ctrl->transfer_continue) && (uac->isoc_rx_valid_cnt > 0)) {
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
static int usbd_composite_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *pdata_ctrl = &(uac->uac_isoc_out);
	usbd_composite_dev_t *cdev = uac->cdev;
	usbd_composite_uac_buf_t *p_buf = NULL;
	UNUSED(dev);

	// RTK_LOGS(TAG, RTK_LOG_DEBUG, "Read data out %d\n",pdata_ctrl->transfer_continue);
#if USBD_UAC_ISOC_XFER_DEBUG
	uac->isoc_rx_cnt ++;
#endif

	p_buf = pdata_ctrl->p_cur_buf_node;
	if (pdata_ctrl->transfer_continue) {
		if (ep_addr == USBD_COMP_UAC_ISOC_OUT_EP) {
			if (len == 0) { //ZLP
				usbd_ep_receive(cdev->dev, USBD_COMP_UAC_ISOC_OUT_EP, p_buf->buf_raw, pdata_ctrl->isoc_mps);

#if USBD_UAC_ISOC_XFER_DEBUG
				uac->isoc_rx_zlp_cnt ++;
#endif
			} else {
				uac->isoc_rx_len = len;
				uac->isoc_rx_valid_cnt ++;
#if USBD_UAC_ISOC_XFER_DEBUG
				u32 g_rx_new_tick = usbd_composite_uac_get_timetick(); //us
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
#if USBD_UAC_ISOC_XFER_DEBUG
					uac->isoc_overwrite_cnt ++;
#endif
				}
				usbd_composite_uac_list_add_tail(&(pdata_ctrl->data_list), p_buf);

				p_buf = usbd_composite_uac_list_remove_head(&(pdata_ctrl->empty_list));
				if (p_buf) {
					usbd_ep_receive(cdev->dev, USBD_COMP_UAC_ISOC_OUT_EP, p_buf->buf_raw, pdata_ctrl->isoc_mps);
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
		usbd_ep_receive(cdev->dev, USBD_COMP_UAC_ISOC_OUT_EP, p_buf->buf_raw, pdata_ctrl->isoc_mps);
	}

	return HAL_OK;
}

/**
  * @brief  Get descriptor callback
  * @param  req: setup request
  * @param  speed: device speed
  * @param  len: descriptor length
  * @retval descriptor buffer
  */
static u8 *usbd_composite_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len)
{
	u8 *buf = NULL;

	if (speed == USB_SPEED_HIGH) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid speed, UAC 1.0 only support full speed\n");
		return buf;
	}

	dev->self_powered = USBD_UAC_SELF_POWERED;

#if USBD_UAC_ISOC_XFER_DEBUG
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	uac->isoc_timeout_max_step = 1500;
#endif

	switch ((req->wValue >> 8) & 0xFF) {
	case USB_DESC_TYPE_CONFIGURATION:
		buf = usbd_composite_uac_fs_itf_desc;
		*len = sizeof(usbd_composite_uac_fs_itf_desc);
		break;

	default:
		break;
	}

	return buf;
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

#if USBD_UAC_ISOC_XFER_DEBUG

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

	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC dump thread\n");

	while (1) {

#if USBD_ISR_TASK_TIME_DEBUG
		if (uac && uac->cdev && uac->cdev->dev) {
			usb_dev_t *dev = uac->cdev->dev;
			RTK_LOGS(TAG, RTK_LOG_INFO, "USB RX %d-%d-%d/TO %d-%d/zp %d-%d-%d/OW %d/Pkt %d/len %d-%d/%d/IsrC %d-%d/isrT %d-%d\n",
					 uac->isoc_rx_valid_cnt, uac->isoc_rx_cnt, uac->isoc_rx_zlp_cnt,
					 uac->isoc_timeout_cnt, uac->isoc_timeout_max_value,
					 uac->isoc_zlp_cnt, (u32)(uac->uac_isoc_out.sof_idx), (u32)(uac->uac_isoc_out.data_idx),
					 uac->isoc_overwrite_cnt,
					 usbd_composite_uac_get_read_buf_cnt(),
					 uac->copy_data_len, uac->isoc_rx_len,
					 uac->uac_isoc_out.transfer_continue,
					 dev->isr_func_time_cost_max, dev->isr_func_time_cost,
					 dev->isr_trigger_time_diff_max, dev->isr_trigger_time_diff
					);
		}
#else
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB Dump RX %d-%d-%d/TO %d-%d/zp %d-%d-%d/OW %d/Pkt %d/len %d-%d/%d\n",
				 uac->isoc_rx_valid_cnt, uac->isoc_rx_cnt, uac->isoc_rx_zlp_cnt,
				 uac->isoc_timeout_cnt, uac->isoc_timeout_max_value,
				 uac->isoc_zlp_cnt, (u32)(uac->uac_isoc_out.sof_idx), (u32)(uac->uac_isoc_out.data_idx),
				 uac->isoc_overwrite_cnt,
				 usbd_composite_uac_get_read_buf_cnt(),
				 uac->copy_data_len, uac->isoc_rx_len,
				 uac->uac_isoc_out.transfer_continue
				);
#endif

		rtos_time_delay_ms(USBD_UAC_DEBUG_LOOP_TIME);
	}

	rtos_task_delete(NULL);
}

static void composite_usbd_comp_reset_isr_time(void)
{
#if USBD_ISR_TASK_TIME_DEBUG
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usb_dev_t *dev;

	if ((NULL == uac) || (NULL == uac->cdev) || (NULL == uac->cdev->dev)) {
		return;
	}

	dev = uac->cdev->dev;

	dev->isr_func_time_cost_max = 0;
	dev->isr_trigger_time_diff_max = 0;
#else
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid params\n");
#endif
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

			memcpy(buffer + *copy_len, p_buf->buf_raw, p_buf->buf_valid_len);
			*copy_len += p_buf->buf_valid_len;
			memset(p_buf->buf_raw, 0x00, p_buf->buf_valid_len);

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

	memset(uac, 0x00, sizeof(usbd_composite_uac_device_t));

	uac->cur_volume = 0x001F;
	uac->cur_mute = 0;
	uac->cur_clk_valid = 1;
#if USBD_UAC_ISOC_XFER_DEBUG
	uac->uac_dump_task_alive = 0;
#endif
	usbd_composite_uac_ep_buf_ctrl_deinit(&(uac->uac_isoc_in));
	usbd_composite_uac_ep_buf_ctrl_deinit(&(uac->uac_isoc_out));

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
			goto init_clean_ctrl_buf_exit;
		}
	}

	uac->cdev = cdev;

#if USBD_UAC_ISOC_XFER_DEBUG
	if (rtos_task_create(&(uac->uac_dump_task), ((const char *)"usbd_composite_uac_status_dump_thread"), usbd_composite_uac_status_dump_thread, NULL, 1024U,
						 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create usb status dump task fail\n");
	}
#endif

	return ret;

init_clean_ctrl_buf_exit:
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

#if USBD_UAC_ISOC_XFER_DEBUG
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
int usbd_composite_uac_transmit_data(u8 *buf, u16 len)
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

		pbuf_ctrl->transfer_continue = 1;
		pbuf_ctrl->sof_idx = 0;
		pbuf_ctrl->data_idx = 0;

#if USBD_UAC_ISOC_XFER_DEBUG
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "First trigger sema %d cnt %d-%d \n", pbuf_ctrl->read_wait_sema, usbd_composite_uac_get_read_buf_cnt(), pbuf_ctrl->isoc_mps);
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
		pbuf_ctrl->transfer_continue = 0;

		/* update the rx mps */
		usbd_composite_uac_ep_update_mps(pbuf_ctrl, (usbd_audio_cfg_t *)uac_cfg, cdev->dev->dev_speed);
	}

	return 0;
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

#if USBD_UAC_ISOC_XFER_DEBUG
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

	uac->uac_isoc_out.transfer_continue = 0;
	uac->uac_isoc_in.transfer_continue = 0;
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

	if (pdata_ctrl->transfer_continue == 0) {
		return 0;
	}

	if (time_out_ms == 0) {
		if (pdata_ctrl->data_list.count == 0) { //no valid data
			return 0;
		}

		usbd_composite_uac_read_ring_buf(pdata_ctrl, buffer, size, &copy_len, &pkt_cnt, zero_pkt_flag);
	} else {
		do {
			// RTK_LOGS(TAG, RTK_LOG_DEBUG, "Ringtbuf cnt %d size=%d\n", usbd_composite_uac_get_read_buf_cnt(),size);
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
		} while (pdata_ctrl->transfer_continue);
	}

#if USBD_UAC_ISOC_XFER_DEBUG
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

	if (pdata_ctrl == NULL) {
		return 0;
	}

	return pdata_ctrl->data_list.count * USBD_UAC_ONE_KHZ * USBD_UAC_POW2(USBD_UAC_FS_DEFAULT_BINTERVAL - 1);

	return 0;
}
