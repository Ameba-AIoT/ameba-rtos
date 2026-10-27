/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

/* uac 1.0 */
#include "usbd_uac.h"
#ifdef CONFIG_USBD_COMPOSITE
#include "usbd_composite.h"
#endif
#include "usb_uac1.h"

/* Private defines -----------------------------------------------------------*/

#define UABD_UAC_DESC_DUMP (0)
#if USBD_UAC_DEBUG
#define USBD_UAC_DEBUG_LOOP_TIME   (10U*1000U)
#define USBD_UAC_DEBUG_STEP_TIME   100U
#endif
#define UABD_UAC_VOL_ERR_VAL       255U
#define USBD_UAC_FS_ISOC_MPS                        1023U   /* Full speed ISOC IN & OUT max packet size */
#define USBD_UAC_FS_ISOC_MPS_ALIGNED                1024U   /* USBD_UAC_FS_ISOC_MPS adjusted to the DWORD boundary, refer to usb_get_dword_aligned_mps */
#define USBD_UAC_FS_DEFAULT_BINTERVAL               1U      /**< FS ISOC bInterval used in EP descriptors */
#define USBD_UAC_ONE_KHZ                            1000U
/* sample sampling freq max count */
#define USBD_UAC_SAMPLING_FREQ_MAX_COUNT            2U
/* Highest rate of usbd_uac_sampling_rates[]: every AS alt setting advertises its
   wMaxPacketSize for it, refer to USBD_UAC_GET_FS_MPS */
#define USBD_UAC_SAMPLING_FREQ_MAX                  USBD_UAC_SAMPLING_FREQ_48K
/* Period of the warning usbd_uac_ep_buf_ctrl_deinit() logs while a reader does not leave */
#define USBD_UAC_READER_EXIT_WARN_MS                1000U
/* AC Format Type descriptor length */
#define USBD_UAC1_AS_FRT_TYPE_DESC_LEN              (USB_UAC1_AS_FRT_TYPE_DESC_LEN + 3 * 2)
#define USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE            0x12U
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE    0x02U
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT           0x08U
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE   0x10U
// USB Out
#define USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES            0x15U
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES    0x01U
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT          0x05U
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES   0x09U
/* len of each Audio stream interface/altsetting (one EP) */
#define USBD_UAC_AS_EIF_LEN                        (USB_LEN_IF_DESC + USB_UAC1_AS_IF_DESC_LEN + USBD_UAC1_AS_FRT_TYPE_DESC_LEN \
                                                   + USB_UAC1_EP_DESC_LEN + USB_UAC1_AUDIO_DATA_EP_DESC_LEN)
/* full speed AS alt setting num: alt 0 plus one alt per channel count from 2 up to the max */
#define USBD_UAC_FS_AS_ALT_SETTING_NUM              ((USBD_UAC_DEFAULT_CH_CNT / 2U) + 1U)

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/**
 * Defines Audio trx buffer MAX count.
 */

/**
 * Defines UAC 1.0 device volume MAX & MIN.
 * Must match usbd_uac_drv_vol[]'s dB-like range, not usbd_uac_pc_vol_lvl[].
 */

/* bit_width */
#define USBD_UAC_BIT_WIDTH(byte_width)              (8U * (byte_width))

// USB Audio terminal
// USB In

/* AC IF header interface num */

/* AC feature uint descriptor length */

#define USBD_UAC_AC_FU_HEAD_DESC_LEN(ch_cnt)        (USB_UAC1_AC_FUNC_UNIT_DESC_LEN +  (ch_cnt))
/* AC IF header descriptor length */

#define USBD_UAC_AC_IF_HEAD_DESC_LEN(ch_cnt) \
    (USB_UAC1_AC_IF_HEADER_LEN  + USB_UAC1_AC_IN_TTY_DESC_LEN + USBD_UAC_AC_FU_HEAD_DESC_LEN(ch_cnt) + USB_UAC1_AC_OUT_TTY_DESC_LEN)

/* len of total Audio control interface */
#define USBD_UAC_AC_IF_LEN(ch_cnt)                 (USB_LEN_IF_DESC + USBD_UAC_AC_IF_HEAD_DESC_LEN(ch_cnt))

/* len of total Audio stream interface */
#define USBD_UAC_AS_TIF_LEN(alt_num)               (USBD_UAC_AS_EIF_LEN * (alt_num - 1) + USB_LEN_IF_DESC)

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

/* calculate full speed MPS */
#define USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, sampling_freq_hz)    ((ch_cnt) * (byte_width) * ((sampling_freq_hz) / USBD_UAC_ONE_KHZ + 1U))
/* check MPS */
#define USBD_UAC_IS_FS_MPS_VALID(ch_cnt, byte_width, sampling_freq_hz)   ((USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, sampling_freq_hz)) <= USBD_UAC_FS_ISOC_MPS)

/* get full speed MPS of an alt setting, sized for the highest declared rate */
#define USBD_UAC_GET_FS_MPS(ch_cnt, byte_width)  USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_MAX)

/* ISOC OUT ring node size: the MPS of the widest alt setting, so any host format fits */
#define USBD_UAC_PLAY_MPS                        USBD_UAC_GET_FS_MPS(USBD_UAC_DEFAULT_CH_CNT, USBD_UAC_DEFAULT_BYTE_WIDTH)

/* Ref USB 2.0 5.6.3: a Full-Speed ISOC packet is at most 1023 bytes */
#if !USBD_UAC_IS_FS_MPS_VALID(USBD_UAC_DEFAULT_CH_CNT, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_MAX)
#error "UAC1: channels x bytes per sample shall be <= 20 to fit FS 48kHz (1023B). Lower 'Playback max channels' or 'Playback bit width'"
#endif

/* len of full speed total configuration descriptor buf */
#define USBD_UAC_FS_CFG_DESC_BUF_LEN(ch_cnt) \
    (USB_LEN_CFG_DESC + USB_LEN_IAD_DESC + USBD_UAC_AC_IF_LEN(ch_cnt) + USBD_UAC_AS_TIF_LEN(USBD_UAC_FS_AS_ALT_SETTING_NUM))

/* Private function prototypes -----------------------------------------------*/

static int usbd_uac_set_config(usb_dev_t *dev, u8 config);
static void usbd_uac_clear_config(usb_dev_t *dev, u8 config);
static int usbd_uac_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len);
#ifdef CONFIG_USBD_COMPOSITE
static void usbd_uac_set_interface_base(u8 base);
#endif
static int usbd_uac_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static int usbd_uac_handle_ep0_data_out(usb_dev_t *dev);
static void usbd_uac_handle_sof(usb_dev_t *dev);
static void usbd_uac_append_data(usbd_uac_buf_ctrl_t *pdata_ctrl);
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
static inline u8 usbd_uac_ep_enable(const usbd_audio_cfg_t *ep);
static bool usbd_uac_is_valid_sample_rate(u32 freq, u8 speed);
static int usbd_uac_check_play_fmt(const usbd_audio_cfg_t *fmt, u8 speed);
static int usbd_uac_receive_data(void);
static u8 usbd_uac_is_owned_interface(u16 interface_number);
static int usbd_uac_receive_control_data(usb_dev_t *dev, const usb_setup_req_t *req, u16 expected_length);
static u32 usbd_uac_read_locked(usbd_uac_buf_ctrl_t *pdata_ctrl, u8 *buffer, u32 size, u32 time_out_ms, u32 *zero_pkt_flag);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "UAC";

static const u32 usbd_uac_sampling_rates[USBD_UAC_SAMPLING_FREQ_MAX_COUNT] = {USBD_UAC_SAMPLING_FREQ_44K, USBD_UAC_SAMPLING_FREQ_48K};

/* Cache-line-aligned scratch buffer used by the ISOC OUT engine as the destination for each usbd_ep_receive() before the payload is copied into the ring buffer.
   Sized to the DWORD aligned MPS: the controller pads every received packet up to the DWORD boundary, so a 1023 byte MPS occupies 1024 bytes in memory. */
static u8 usbd_uac_rx_buf[USBD_UAC_FS_ISOC_MPS_ALIGNED] USB_DMA_ALIGNED;
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

static const u8 usbd_uac_fs_config_desc[] = {
	/* USB UAC Device Configuration Descriptor */
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,                  /* bLength */
	USB_DESC_TYPE_CONFIGURATION,       /* bDescriptorType */
	0x00,                              /* wTotalLength: patched at runtime */
	0x00,
	2,                              /* Number of interfaces supported by this configuration */
	0x01,                              /* Value to use as an argument to the SetConfiguration() request to select this configuration */
	0x00,                              /* Index of string descriptor describing this configuration */
	0x80,                              /* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
	0x32,                              /* bMaxPower */

	/* IAD for Audio Control + Audio Streaming */
	USB_LEN_IAD_DESC,                  /* bLength */
	USB_DESC_TYPE_IAD,                 /* bDescriptorType */
	0x00,                              /* bFirstInterface (patched by composite) */
	2,                /* bInterfaceCount */
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
	USB_UAC1_AC_IF_HEADER_LEN,                                           /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                               /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_HEADER,                                                     /* HEADER descriptor subtype  */
	USB_LOW_BYTE(USB_UAC1_VERSION_01_00),                                /* bcdADC */
	USB_HIGH_BYTE(USB_UAC1_VERSION_01_00),
	USB_LOW_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT)),
	USB_HIGH_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT)),/* Total number of byte_width returned for the class-specific AudioControl interface descriptor. Includes
                                                                         the combined length of this descriptor header and all Unit and Terminal descriptors.   */
	0x01,                                                                  /* bInCollection 1 means only speaker*/
	0x01,                                                                  /* baInterfaceNr[1] */

	/* Headphone Audio Control Input Terminal Descriptor 1.0 (USB Streaming, type 0x0101) */
	USB_UAC1_AC_IN_TTY_DESC_LEN,                             /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_INPUT_TERMINAL,              /* INPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* Constant uniquely identifying the Terminal within the audio
                                                                  function. This value is used in all requests to address this Terminal.   */
	0x01,
	0x01,                                                    /* A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The
                                                                AudioStreaming interface descriptor points to the associated Terminal through the bTerminalLink field. */
	0x00,                                                    /* This Input Terminal has no association   */
	USBD_UAC_DEFAULT_CH_CNT,                                 /* This Terminal's output audio channel cluster has 8 logical output channels   */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_DEFAULT_CH_CNT),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_DEFAULT_CH_CNT),
	0x00,                                                     /* iChannelNames */
	0x00,                                                    /* Index of a string descriptor, describing the Input Terminal.  */

	/* FEATURE_UNIT descriptor subtype */
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT),   /* Size of the descriptor, in byte_width  : 6 + (4 + 1) * 4 */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_FEATURE_UNIT,                                   /* FEATURE_UNIT descriptor subtype   */
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
	USB_UAC1_AC_OUT_TTY_DESC_LEN,                             /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                    /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AC_OUTPUT_TERMINAL,              /* OUTPUT_TERMINAL descriptor subtype   */
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
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
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
	USBD_UAC_CH_CNT_2,                                       /* bNrChannels */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* bSubframeSize */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* bBitResolution */
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
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_DEFAULT_BYTE_WIDTH)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */
	0x00,                                                    /* bRefresh */
	0x00,                                                    /* bSynchAddress */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC1_AUDIO_DATA_EP_DESC_LEN,                         /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x01,                                                    /* bmAttributes */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */

#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/2*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                      /* The number of this interface is 1.   */
	0x02,                                                     /* The value used to select the alternate setting for this interface is 2 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
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
	USBD_UAC_CH_CNT_4,                                       /* bNrChannels */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* bSubframeSize */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* bBitResolution */
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
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_DEFAULT_BYTE_WIDTH)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */
	0x00,                                                    /* bRefresh */
	0x00,                                                    /* bSynchAddress */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC1_AUDIO_DATA_EP_DESC_LEN,                         /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x01,                                                    /* bmAttributes */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif

#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/3*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                      /* The number of this interface is 1.   */
	0x03,                                                     /* The value used to select the alternate setting for this interface is 2 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
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
	USBD_UAC_CH_CNT_6,                                       /* bNrChannels */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* bSubframeSize */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* bBitResolution */
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
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_DEFAULT_BYTE_WIDTH)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */
	0x00,                                                    /* bRefresh */
	0x00,                                                    /* bSynchAddress */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC1_AUDIO_DATA_EP_DESC_LEN,                         /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x01,                                                    /* bmAttributes */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif

#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/4*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                      /* The number of this interface is 1.   */
	0x04,                                                     /* The value used to select the alternate setting for this interface is 2 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
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
	USBD_UAC_CH_CNT_8,                                       /* bNrChannels */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* bSubframeSize */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* bBitResolution */
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
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_DEFAULT_BYTE_WIDTH)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */
	0x00,                                                    /* bRefresh */
	0x00,                                                    /* bSynchAddress */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC1_AUDIO_DATA_EP_DESC_LEN,                         /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x01,                                                    /* bmAttributes */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif
};
/* The length is computed by USBD_UAC_FS_CFG_DESC_BUF_LEN() from the channel count and the template is trimmed by
   #if on the same count: a mismatch would leave zero padding counted in wTotalLength */
_Static_assert(sizeof(usbd_uac_fs_config_desc) == USBD_UAC_FS_CFG_DESC_BUF_LEN(USBD_UAC_DEFAULT_CH_CNT), "UAC cfg desc len");

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
#ifdef CONFIG_USBD_COMPOSITE
	.set_interface_base = usbd_uac_set_interface_base,
#endif
};

/* UAC Device */
static usbd_uac_dev_t usbd_uac_dev;

/* UAC volume data from windows10 PC*/
/* Different OS transmit the same vol level, but the corresponding driver vol values are different. */
static const u8 usbd_uac_pc_vol_lvl[] = {0, 5, 10, 20, 30, 40, 50, 60, 65, 75, 80, 85, 90, 95, 100};
static const s16 usbd_uac_drv_vol[] = {-190, -170, -151, -112, -74, -37, 0, 37, 56, 93, 112, 132, 151, 170, 190};

/* Exported variables --------------------------------------------------------*/

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
	const usbd_uac_dev_t *cdev = &usbd_uac_dev;
	u16 cnt = (cdev->cfg->rx_buf_cnt != 0U) ? cdev->cfg->rx_buf_cnt : USBD_UAC_DEFAULT_RX_BUF_CNT;

	UNUSED(speed);

	return cnt / USBD_UAC_POW2(USBD_UAC_FS_DEFAULT_BINTERVAL - 1U);
}

/**
  * @brief  Get the MPS based on configuration parameters and speed
  * @param  params: Pointer to audio configuration parameters structure
  * @param  speed: USB connection speed
  * @retval Calculated MPS
  */
static u16 usbd_uac_get_mps(const usbd_audio_cfg_t *params, u8 speed)
{
	u32 mps_value;

	UNUSED(speed);

	if (NULL == params) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No Param\n");
		return 0U;
	}

	/* Ref UAC 1.0 4.6.1.2: the host may switch the alt setting and the rate at any time and then
	   sends up to the wMaxPacketSize of the selected alt setting. The ring is therefore sized once
	   for the widest alt setting at the highest rate, so a format switch never reallocates it and
	   a packet of any declared format fits a node. The descriptors are checked against the FS
	   limit at build time, refer to USBD_UAC_PLAY_MPS. */
	mps_value = USBD_UAC_PLAY_MPS;

	return (u16)mps_value;
}

/**
  * @brief  Deinitialize UAC endpoint buffer control structure
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @retval void
  */
static void usbd_uac_ep_buf_ctrl_deinit(usbd_uac_buf_ctrl_t *buf_ctrl)
{
	u32 wait_ms;

	buf_ctrl->isoc_mps = 0;
	buf_ctrl->buf_list_cnt = 0;
	buf_ctrl->next_xfer = 0;
	buf_ctrl->sof_idx = 0;
	buf_ctrl->data_idx = 0;
	buf_ctrl->xfer_cnt = 0;
	buf_ctrl->last_xfer_len = 0;

	if (buf_ctrl->uac_sema_valid != 0) {
		/* Close the gate first: from here on usbd_uac_read() rejects new callers */
		usb_os_enter_critical();
		buf_ctrl->is_closing = 1U;
		usb_os_exit_critical();
		buf_ctrl->uac_sema_valid = 0;

		/* Wait for every task still inside usbd_uac_read() to leave, whether it is blocked on
		   the semaphore or copying out of the ring. Not bounded: freeing the semaphore or the
		   ring under a reader would be a use after free, so a starved reader is reported once
		   per USBD_UAC_READER_EXIT_WARN_MS instead. Sleep rather than spin, the woken reader
		   may have a lower priority than the caller and needs the CPU to leave. */
		wait_ms = 0U;
		while (buf_ctrl->xfer_users != 0U) {
			rtos_sema_give(buf_ctrl->uac_isoc_sema);
			usb_os_sleep_ms(1U);
			wait_ms++;
			if (wait_ms >= USBD_UAC_READER_EXIT_WARN_MS) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "Wait reader exit\n");
				wait_ms = 0U;
			}
		}
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
	u16 mps;
	u8 cnt;

	if (usbd_uac_ep_enable(params) != 0) {
		mps = usbd_uac_get_mps(params, speed);
		if (mps == 0U) {
			return HAL_ERR_PARA;
		}
		cnt = (u8)usbd_uac_get_ring_buf_cnt(speed);

		/* The ring only depends on the speed, not on the format: keep it while it fits */
		if ((buf_ctrl->uac_sema_valid != 0U) && (buf_ctrl->isoc_mps == mps) && (buf_ctrl->buf_list_cnt == cnt)) {
			return HAL_OK;
		}

		usbd_uac_ep_buf_ctrl_deinit(buf_ctrl);
		buf_ctrl->isoc_mps = mps;
		buf_ctrl->buf_list_cnt = cnt;
		buf_ctrl->binterval = USBD_UAC_FS_DEFAULT_BINTERVAL;

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
		/* Every reader resource exists from here on: let usbd_uac_read() in */
		buf_ctrl->is_closing = 0U;
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

	return HAL_OK;
}

/**
  * @brief  Check whether an endpoint address belongs to this class
  * @param  cdev: UAC device instance
  * @param  ep_addr: Endpoint address taken from the low byte of wIndex
  * @retval 1 if owned, 0 otherwise
  */
static u8 usbd_uac_is_own_ep(const usbd_uac_dev_t *cdev, u8 ep_addr)
{
	const usbd_uac_config_t *cfg = cdev->cfg;

	if ((cfg == NULL) || (cdev->cb == NULL) || (ep_addr == 0U)) {
		return 0U;
	}

	if ((usbd_uac_ep_enable(&(cfg->out)) != 0) && (ep_addr == cfg->isoc_out_addr)) {
		return 1U;
	}

	if ((usbd_uac_ep_enable(&(cfg->in)) != 0) && (ep_addr == cfg->isoc_in_addr)) {
		return 1U;
	}

	return 0U;
}

/**
  * @brief  Check whether an interface number, local to this function, belongs to the class
  * @note   Composite mode already rebased wIndex to the local interface number.
  * @param  interface_number: Interface number from wIndex
  * @retval 1 if the interface is in the reported config descriptor, 0 otherwise
  */
static u8 usbd_uac_is_owned_interface(u16 interface_number)
{
	return ((interface_number == USB_UAC1_IF_IDX_AC_HEADSET) || (interface_number == USB_UAC1_IF_IDX_AS_HEADSET_HEADPHONES)) ? 1U : 0U;
}

/**
  * @brief  Accept the data stage of an H2D class or vendor request
  * @note   Ref USB 2.0 8.5.3.1 and 9.2.7: the status stage reports whether the request was
  *         carried out. The core only hands a data stage to ep0_data_out() in the Configured
  *         state and ACKs it otherwise, and a parameter block of the wrong size can not be
  *         applied, so both are refused here and the core stalls the request.
  * @param  dev: USB device instance
  * @param  req: USB request
  * @param  expected_length: Size of the parameter block of the addressed control
  * @retval Status
  */
static int usbd_uac_receive_control_data(usb_dev_t *dev, const usb_setup_req_t *req, u16 expected_length)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	int ret = HAL_ERR_PARA;

	if ((dev->dev_state == USBD_STATE_CONFIGURED) && (req->wLength == expected_length) &&
		((u32)req->wLength <= ep0_out->xfer_buf_len)) {
		usb_os_memcpy((void *)&cdev->ctrl_req, (const void *)req, sizeof(usb_setup_req_t));
		cdev->ctrl_req_pending = 1U;
		ep0_out->xfer_len = req->wLength;
		/* Propagate a submit failure so that the core stalls EP0 instead of leaving the host
		   waiting out the data stage, ref USB 2.0 8.5.3.4 */
		ret = usbd_ep_receive(dev, ep0_out);
		if (ret != HAL_OK) {
			/* The data stage never started, so no EP0 OUT completion will arrive to consume the
			   stashed request. Drop it, else the next unrelated request's data stage would be
			   applied as this one's payload. */
			cdev->ctrl_req_pending = 0U;
		}
	}

	return ret;
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
	/* Read once: usbd_uac_deinit() may clear it on another core */
	const usbd_uac_cb_t *cb = cdev->cb;

	/* Only the bConfigurationValue advertised in the config descriptor is valid */
	if (config != 1U) {
		return HAL_ERR_PARA;
	}

	cdev->dev = dev;

	cdev->alt_setting = 0U;

	/* Invalidate the cached stream format on each SET_CONFIGURATION so the first
	 * SET_INTERFACE(alt=1) of the new session re-fires format_changed(). Without
	 * this, a hotplug re-enumeration leaves cur_ch_cnt/cur_byte_width matching the
	 * host's selection, the "!=" guard drops format_changed(), and apps that gate
	 * playback on it never restart. cur_play_sampling_freq keeps its private_init default. */
	cdev->cur_ch_cnt = 0U;
	cdev->cur_byte_width = 0U;

	/* Ref USB 2.0 9.4.7: the status stage of SET_CONFIGURATION tells the host the device is
	   configured, so a failure of the app is reported and the core stalls the request. The app
	   goes first, so that nothing has been armed yet when it fails. */
	if ((cb != NULL) && (cb->set_config != NULL)) {
		if (cb->set_config() != HAL_OK) {
			return HAL_ERR_HW;
		}
	}

	/* Published under the lock clear_config() resets it with, refer to usbd_uac_receive_data() */
	usb_os_enter_critical();
	cdev->is_configured = 1U;
	usb_os_exit_critical();

	/* Try to arm the ISOC OUT EP immediately, matching composite's eager
	 * arm-at-set_config behavior. No-ops (HAL_ERR_HW) until the app has
	 * called usbd_uac_config() at least once to size the ring buffer. */
	usbd_uac_receive_data();

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
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	/* Read once: usbd_uac_deinit() may clear it on another core */
	const usbd_uac_cb_t *cb = cdev->cb;
	usb_ep_info_t *info;

	UNUSED(config);

	if (dev == NULL) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Dev not ready\n");
		return;
	}

	/* Ref USB 2.0 9.1.1.3 / 9.4.10: after a bus reset or a de-configuration no alternate setting
	   is selected and the endpoints are gone. The SOF filler and the task APIs test this state
	   under the same lock, possibly on another core. next_xfer is kept: the stream resumes on
	   its own once the host selects the streaming setting again. */
	usb_os_enter_critical();

	/* Closes the EP arm path of the task APIs before the EPs go, refer to usbd_uac_receive_data() */
	cdev->is_configured = 0U;
	cdev->alt_setting = 0U;
	cdev->uac_isoc_out.xfer_cnt = 0U;
	cdev->uac_isoc_out.sof_idx = 0U;
	cdev->uac_isoc_out.data_idx = 0U;
	/* A control transfer cut by the reset never gets its data stage */
	cdev->ctrl_req_pending = 0U;

	/* DeInit ISOC IN EP */
	info = &ep_isoc_in->info;
	if ((cb != NULL) && (usbd_uac_ep_enable(&(cdev->cfg->in)) != 0U) && (info->addr != 0U)) {
		usbd_ep_deinit(dev, ep_isoc_in);
	}

	/* DeInit ISOC OUT EP */
	info = &ep_isoc_out->info;
	if ((cb != NULL) && (usbd_uac_ep_enable(&(cdev->cfg->out)) != 0U) && (info->addr != 0U)) {
		usbd_ep_deinit(dev, ep_isoc_out);
	}

	usb_os_exit_critical();
}

/**
  * @brief  Check freq requests
  * @param  freq: Freq requests from host
  * @param  speed: USB connection speed
  * @retval Status
  */
static bool usbd_uac_is_valid_sample_rate(u32 freq, u8 speed)
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
  * @brief  Check a playback format against the ones the descriptors declare
  * @param  fmt: Playback stream format
  * @param  speed: USB connection speed
  * @retval Status
  */
static int usbd_uac_check_play_fmt(const usbd_audio_cfg_t *fmt, u8 speed)
{
	/* One alt setting per even channel count from 2 up to the max, all with the same width */
	if ((fmt->ch_cnt < USBD_UAC_CH_CNT_2) || (fmt->ch_cnt > USBD_UAC_DEFAULT_CH_CNT) || ((fmt->ch_cnt % 2U) != 0U) ||
		(fmt->byte_width != USBD_UAC_DEFAULT_BYTE_WIDTH) || (!usbd_uac_is_valid_sample_rate(fmt->sampling_freq, speed))) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid play fmt %u/%u/%u\n", fmt->sampling_freq, fmt->ch_cnt, fmt->byte_width);
		return HAL_ERR_PARA;
	}

	return HAL_OK;
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
	/* Read once: usbd_uac_deinit() may clear it on another core */
	const usbd_uac_cb_t *cb = cdev->cb;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	int ret = HAL_OK;
	u8 entityId;
	u8 controlSelector;
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
				/* Ref USB 2.0 Table 9-10: the whole wIndex is the interface number. The AC
				   interface owns alternate setting 0 only and must not change the streaming
				   format, any other interface belongs to another function. */
				if (req->wIndex == USB_UAC1_IF_IDX_AC_HEADSET) {
					if (alt_setting != 0U) {
						ret = HAL_ERR_PARA;
					}
				} else if (req->wIndex != USB_UAC1_IF_IDX_AS_HEADSET_HEADPHONES) {
					ret = HAL_ERR_PARA;
				} else if (alt_setting == 0U) {
					/* Ref USB 2.0 9.4.5: alt setting 0 is the zero-bandwidth setting, it stops the
					   stream and leaves the negotiated format untouched */
					cdev->alt_setting = 0U;
					/* Ref UAC 1.0 3.7.2.3: no data is sent on the zero-bandwidth setting. Disarm the SOF
					   underrun filler until the next real packet, else it keeps feeding the reader
					   synthetic nodes after the host stopped. The OUT EP stays armed so that the
					   next alt=1 streams at once, ISOC EPs have no halt or toggle state to reset. */
					cdev->uac_isoc_out.xfer_cnt = 0U;
					cdev->uac_isoc_out.sof_idx = 0U;
					cdev->uac_isoc_out.data_idx = 0U;
				} else if (alt_setting != cdev->alt_setting) {
					switch (alt_setting) {
					case 1:
						byte_width = USBD_UAC_DEFAULT_BYTE_WIDTH;
						ch_cnt = 2;
						break;
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
					case 2:
						byte_width = USBD_UAC_DEFAULT_BYTE_WIDTH;
						ch_cnt = 4;
						break;
#endif
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
					case 3:
						byte_width = USBD_UAC_DEFAULT_BYTE_WIDTH;
						ch_cnt = 6;
						break;
#endif
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
					case 4:
						byte_width = USBD_UAC_DEFAULT_BYTE_WIDTH;
						ch_cnt = 8;
						break;
#endif
					default:
						/* Ref USB 2.0 9.4.5: an alt setting that does not exist is a request error,
						   the streaming state must stay on the previous setting */
						ret = HAL_ERR_PARA;
						break;
					}

					if (ret == HAL_OK) {
						cdev->alt_setting = alt_setting;/* Set new altsetting */
						if ((cdev->cur_byte_width != byte_width) && (byte_width != 0)) {
							cdev->cur_byte_width = byte_width;
							fmt_change = 1;
						}
						if ((cdev->cur_ch_cnt != ch_cnt) && (ch_cnt != 0)) {
							cdev->cur_ch_cnt = ch_cnt;
							fmt_change = 1;
						}
						if (fmt_change != 0) {
							if ((cb != NULL) && (cb->format_changed != NULL)) {
								cb->format_changed(cdev->cur_play_sampling_freq, cdev->cur_ch_cnt, cdev->cur_byte_width);
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
				/* Ref USB 2.0 Table 9-11: the whole wIndex is the interface number, and 9.4.4
				   requires a request error for an interface that does not exist */
				if (req->wIndex == USB_UAC1_IF_IDX_AC_HEADSET) {
					ep0_in->xfer_buf[0] = 0U;
				} else if (req->wIndex == USB_UAC1_IF_IDX_AS_HEADSET_HEADPHONES) {
					ep0_in->xfer_buf[0] = cdev->alt_setting;
				} else {
					ret = HAL_ERR_PARA;
					break;
				}
				ep0_in->xfer_len = 1U;
				ret = usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_PARA;
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
				ret = HAL_ERR_PARA;
			}
			break;

		default:
			ret = HAL_ERR_PARA;
			break;
		}
		break;/* case USB_REQ_TYPE_STANDARD */

	case USB_REQ_TYPE_CLASS :
		/* Ref UAC 1.0 5.2.1: the low byte of wIndex addresses the interface or the endpoint the
		   control belongs to. Reject anything owned by another function, otherwise a composite
		   device may route a foreign request here, ref USB 2.0 Table 9-2. */
		if ((req->bmRequestType & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_INTERFACE) {
			if (USB_LOW_BYTE(req->wIndex) != USB_UAC1_IF_IDX_AC_HEADSET) {
				ret = HAL_ERR_PARA;
				break;
			}
		} else if ((req->bmRequestType & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_ENDPOINT) {
			if (usbd_uac_is_own_ep(cdev, USB_LOW_BYTE(req->wIndex)) == 0U) {
				ret = HAL_ERR_PARA;
				break;
			}
		} else {
			ret = HAL_ERR_PARA;
			break;
		}

		entityId = USB_HIGH_BYTE(req->wIndex);
		controlSelector = USB_HIGH_BYTE(req->wValue);

		/* Ref UAC 1.0 5.2.1.1: the low byte of wValue is the channel number for a Feature Unit
		   control and shall be 0 for an endpoint control. The Feature Unit only declares master
		   channel (0) controls, so any other channel addresses a control that does not exist. */
		if (USB_LOW_BYTE(req->wValue) != 0U) {
			ret = HAL_ERR_PARA;
			break;
		}

		if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {

			if (entityId == USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT) {
				if (controlSelector == USB_UAC_FU_MUTE) {
					if (req->bRequest == USB_UAC1_GET_CUR) {
						ep0_in->xfer_buf[0] = cdev->cur_mute;
						ep0_in->xfer_len = 1U;
						ret = usbd_ep_transmit(dev, ep0_in);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 0);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USB_UAC_FU_VOLUME) {
					if (req->bRequest == USB_UAC1_GET_CUR) {
						usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *) & (cdev->cur_volume), 2);
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
						ret = HAL_ERR_PARA;
					}
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 2);
					ret = HAL_ERR_PARA;
				}
			} else if ((req->bmRequestType & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_ENDPOINT) {
				if (controlSelector == USB_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USB_UAC1_GET_CUR) {
						usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *) & (cdev->cur_play_sampling_freq), 3);
						ep0_in->xfer_len = 3U;
						ret = usbd_ep_transmit(dev, ep0_in);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 3);
						ret = HAL_ERR_PARA;
					}
				} else {
					/* Ref UAC 1.0 5.2.3: no other endpoint control is supported, STALL instead
					   of leaving the data stage unanswered */
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 11);
					ret = HAL_ERR_PARA;
				}
			} else {
				USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 10);
				ret = HAL_ERR_PARA;
			}
		} else {
			/* USB_H2D */
			if ((req->bmRequestType & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_ENDPOINT) {
				if (controlSelector == USB_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USB_UAC1_SET_CUR) {
						/* Ref UAC 1.0 5.2.3.2.3.1: 3-byte sampling frequency */
						ret = usbd_uac_receive_control_data(dev, req, 3U);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 4);
						ret = HAL_ERR_PARA;
					}
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 5);
					ret = HAL_ERR_PARA;
				}
			} else if (entityId == USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT) { //0x05 FU
				if (controlSelector == USB_UAC_FU_MUTE) { //mute
					if (req->bRequest == USB_UAC1_SET_CUR) {
						/* Ref UAC 1.0 5.2.2.4.3.1: 1-byte bMute */
						ret = usbd_uac_receive_control_data(dev, req, 1U);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 6);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USB_UAC_FU_VOLUME) { //volume
					if (req->bRequest == USB_UAC1_SET_CUR) {
						/* Ref UAC 1.0 5.2.2.4.3.2: 2-byte wVolume */
						ret = usbd_uac_receive_control_data(dev, req, 2U);
					} else {
						/* Set cur volume range err */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 7);
						ret = HAL_ERR_HW;
					}
				} else {
					/* Set fu err */
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 8);
					ret = HAL_ERR_HW;
				}
			} else {
				/* Unknown entity or recipient, STALL instead of leaving the data stage unarmed */
				USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 12);
				ret = HAL_ERR_PARA;
			}
		}
		break;/* case USB_REQ_TYPE_CLASS */

	case USB_REQ_TYPE_VENDOR:
		if ((cb == NULL) || (cb->setup == NULL)) {
			/* No handler for vendor requests, STALL so that the host recovers promptly
			   instead of waiting out the data stage */
			ret = HAL_ERR_PARA;
		} else if (req->wLength != 0U) {
			if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
				u16 rsp_len = req->wLength;

				/* Ref USB 2.0 9.3.4: wLength is the maximum the host accepts, never trust it
				   as a buffer size. Clamp to the EP0 buffer to avoid an over-write. */
				if (rsp_len > ep0_in->xfer_buf_len) {
					rsp_len = (u16)ep0_in->xfer_buf_len;
				}

				/* EP0 buffer is shared with descriptor and OUT traffic, clear the response
				   window so a callback writing fewer bytes cannot leak stale data. */
				usb_os_memset((void *)ep0_in->xfer_buf, 0, rsp_len);
				ret = cb->setup(req, ep0_in->xfer_buf);
				if (ret == HAL_OK) {
					ep0_in->xfer_len = rsp_len;
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
  * @retval None
  */
static void usbd_uac_handle_sof(usb_dev_t *dev)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	const usbd_uac_cb_t *cb = cdev->cb;
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_out);

	UNUSED(dev);

	/* Track OUT stream underrun: if host stalls the ISOC pipe, synthesise
	 * ZLP filler nodes so blocked readers do not stall forever. */
	if ((pdata_ctrl->next_xfer != 0) && (pdata_ctrl->xfer_cnt > 0)) {
		if (pdata_ctrl->sof_idx == 0) {
			pdata_ctrl->sof_idx = 1;
			pdata_ctrl->data_idx = 0;
		} else {
			pdata_ctrl->sof_idx ++;
		}

		if ((pdata_ctrl->sof_idx > pdata_ctrl->data_idx)
			&& ((pdata_ctrl->sof_idx - pdata_ctrl->data_idx) >= USBD_UAC_ISOC_RX_APPEND_ZLP_DIFF_MAX)) {
			usbd_uac_append_data(pdata_ctrl);
			pdata_ctrl->data_idx ++;
		}
	}

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
	int ret = HAL_OK;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	const usbd_uac_cb_t *cb = cdev->cb;
	usb_setup_req_t *p_ctrl_req = &(cdev->ctrl_req);
	usbd_ep_t *ep0_out = &dev->ep0_out;
	u32 freq;
	s16 volume_value = 0;
	u8 num_points;
	u8 target_volume = UABD_UAC_VOL_ERR_VAL;

	/* No pending request means this data stage does not belong to UAC, the composite dispatcher
	   already routed it by active_func. cb is released by usbd_uac_deinit(), which may run between
	   the setup and the data stage of an H2D request, so it is checked here as well. */
	if ((cdev->ctrl_req_pending == 0U) || (cb == NULL)) {
		return HAL_OK;
	}

	/* Consume the pending request: a single data stage belongs to exactly one setup packet, so
	   the saved request must not be replayed by a later EP0 OUT event. */
	cdev->ctrl_req_pending = 0U;

	if (((p_ctrl_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS) && (p_ctrl_req->bRequest == USB_UAC1_SET_CUR)) {
		if ((USB_HIGH_BYTE(p_ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT &&
			 ((p_ctrl_req->bmRequestType & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_INTERFACE))) {
			if ((USB_HIGH_BYTE(p_ctrl_req->wValue) == USB_UAC_FU_MUTE) && (p_ctrl_req->wLength == 0x01)) {
				if (cb->mute_changed != NULL) {
					cdev->cur_mute = (ep0_out->xfer_buf[0] != 0) ? 1 : 0;
					cb->mute_changed(cdev->cur_mute);
				}
			} else if ((USB_HIGH_BYTE(p_ctrl_req->wValue) == USB_UAC_FU_VOLUME) && (p_ctrl_req->wLength == 0x02)) {
				volume_value = (u16)ep0_out->xfer_buf[0] + ((u16)(ep0_out->xfer_buf[1]) << 8);

				num_points = sizeof(usbd_uac_pc_vol_lvl) / sizeof(usbd_uac_pc_vol_lvl[0]);
				if (num_points != sizeof(usbd_uac_drv_vol) / sizeof(usbd_uac_drv_vol[0])) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Volume arrays length err\n");
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

		if (((p_ctrl_req->bmRequestType & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_ENDPOINT)
			&& (USB_HIGH_BYTE(p_ctrl_req->wValue) == USB_UAC_CS_SAM_FREQ_CONTROL) && (p_ctrl_req->wLength == 0x03)) {
			freq = ((u32)ep0_out->xfer_buf[2] << 16) | ((u32)ep0_out->xfer_buf[1] << 8) | (u32)ep0_out->xfer_buf[0];
			if (usbd_uac_is_valid_sample_rate(freq, dev->dev_speed)) {
				if (cdev->cur_play_sampling_freq != freq) {
					/* The OUT EP and the ring are sized for the highest rate, refer to
					   usbd_uac_get_mps(), so a packet at the new rate already fits: the stream is
					   left running and the application restarts it with the new format. */
					cdev->cur_play_sampling_freq = freq;

					if (cb->format_changed != NULL) {
						cb->format_changed(cdev->cur_play_sampling_freq, cdev->cur_ch_cnt, cdev->cur_byte_width);
					}
				}
			} else {
				/* Ref USB 2.0 9.2.7: an unsupported value is a request error, STALL the status stage */
				ret = HAL_ERR_PARA;
			}
		}

	} else if ((p_ctrl_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_VENDOR) {
		/* Ref USB 2.0 8.5.3.1: the status stage reports the outcome, let the app decide */
		if (cb->setup != NULL) {
			ret = cb->setup(p_ctrl_req, ep0_out->xfer_buf);
		} else {
			ret = HAL_ERR_PARA;
		}
	} else {
		/* Other class requests are rejected in the setup stage */
	}

	return ret;
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
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_in);

	UNUSED(dev);

	/* Return non-zero if EP is not owned by UAC1, so composite
	 * dispatcher can continue iterating to the correct sub-function. */
	if (ep_addr != cdev->cfg->isoc_in_addr) {
		return HAL_ERR_PARA;
	}

	if (pdata_ctrl->next_xfer != 0) {
		if (status != HAL_OK) {
			USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
		}
		//loop to next tx TODO
	}

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
  * @brief  Push a node to the OUT ring buffer, dropping the oldest one if it is full.
  * @note   ISR context. The reader pops the head from task context, possibly on another
  *         SMP core, so the head drop and the tail add are serialized with it.
  * @param  buf_list: ring buffer manager
  * @param  buf: node payload, NULL for a filler node
  * @param  len: node length
  * @param  valid: 1 for real data, 0 for a filler node
  * @retval 1 if the oldest node was dropped, 0 otherwise
  */
static u8 usbd_uac_ringbuf_push(usb_ringbuf_manager_t *buf_list, u8 *buf, u32 len, u8 valid)
{
	u8 tmp[1];
	u8 dropped = 0U;

	usb_os_enter_critical();
	if (usb_ringbuf_is_full(buf_list) != 0) {
		usb_ringbuf_remove_head(buf_list, tmp, 1, NULL);
		dropped = 1U;
	}
	usb_ringbuf_add_tail(buf_list, buf, len, valid);
	usb_os_exit_critical();

	return dropped;
}

/**
  * @brief  Append a filler node to the OUT ring buffer when the host stops sending data.
  * @note   ISR context (invoked from SOF path).
  * @retval void
  */
static void usbd_uac_append_data(usbd_uac_buf_ctrl_t *pdata_ctrl)
{
	usb_ringbuf_manager_t *buf_list = &(pdata_ctrl->buf_list);

	/* full: drop the oldest node to make room */
	if (usbd_uac_ringbuf_push(buf_list, NULL, pdata_ctrl->last_xfer_len, 0U) != 0U) {
#if USBD_UAC_DEBUG
		pdata_ctrl->append_overwrite_cnt++;
#endif
	}
#if USBD_UAC_DEBUG
	pdata_ctrl->append_zlp_cnt++;
#endif

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

	/* Return non-zero if EP is not owned by UAC1, so composite
	 * dispatcher can continue iterating to the correct sub-function. */
	if (ep_addr != cdev->cfg->isoc_out_addr) {
		return HAL_ERR_PARA;
	}

	if (pdata_ctrl->next_xfer != 0) {
		if (len == 0) { //ZLP
			ep_isoc_out->xfer_buf = usbd_uac_rx_buf;
			ep_isoc_out->xfer_len = pdata_ctrl->isoc_mps;
			ret = usbd_ep_receive(cdev->dev, ep_isoc_out);
			if (ret != HAL_OK) {
				USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
			}
		} else if (len > pdata_ctrl->isoc_mps) {
			/* Longer than a ring node, which is sized for the widest declared format, refer to
			   usbd_uac_get_mps(): the host broke its wMaxPacketSize. Drop it rather than queue a
			   truncated packet. */
			USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
			ep_isoc_out->xfer_buf = usbd_uac_rx_buf;
			ep_isoc_out->xfer_len = pdata_ctrl->isoc_mps;
			ret = usbd_ep_receive(cdev->dev, ep_isoc_out);
		} else {
			pdata_ctrl->last_xfer_len = (u16)len;
			pdata_ctrl->xfer_cnt ++;
			pdata_ctrl->data_idx ++;

#if USBD_UAC_DEBUG
			u32 rx_new_tick = usb_os_get_timestamp_us();
			if ((pdata_ctrl->last_xfer_tick > 0) && ((rx_new_tick - pdata_ctrl->last_xfer_tick) > 1500)) {
				pdata_ctrl->timeout_cnt++;
			}
			pdata_ctrl->last_xfer_tick = rx_new_tick;
#endif

			/*
				If no read consumes, the ring is treated as a FIFO with head-drop
				overwrite, matching composite behaviour.
			*/
			if (usbd_uac_ringbuf_push(buf_list, usbd_uac_rx_buf, len, 1U) != 0U) {
#if USBD_UAC_DEBUG
				pdata_ctrl->overwrite_cnt++;
#endif
			}

			ep_isoc_out->xfer_buf = usbd_uac_rx_buf;
			ep_isoc_out->xfer_len = pdata_ctrl->isoc_mps;
			ret = usbd_ep_receive(cdev->dev, ep_isoc_out);

			if ((pdata_ctrl->read_wait_sema != 0) && (usb_ringbuf_is_empty(buf_list) == 0)) {
				rtos_sema_give(pdata_ctrl->uac_isoc_sema);
			}
		}
	} else {
		ep_isoc_out->xfer_buf = usbd_uac_rx_buf;
		ep_isoc_out->xfer_len = pdata_ctrl->isoc_mps;
		/* Only fails once the device is no longer ready, the OUT EP then stays NAKing until
		   the next start_play() re-arms it: record it, there is nothing to retry in an ISR */
		if (usbd_ep_receive(cdev->dev, ep_isoc_out) != HAL_OK) {
			USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
		}
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
  * @retval void
  */
static void usbd_uac_patch_desc(u8 *desc, u16 len)
{
	u16 i;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	const usbd_uac_config_t *cfg = cdev->cfg;

	for (i = 0; i < len;) {
		u8 dlen;
		u8 dtype;
		/* bLength and bDescriptorType are read only once both are inside the buffer */
		if (((u32)i + 2U) > len) {
			break;
		}
		dlen = desc[i];
		if ((dlen < 2U) || (((u32)i + dlen) > len)) {
			break;
		}
		dtype = desc[i + 1];
		if ((dtype == USB_DESC_TYPE_ENDPOINT) && (dlen >= USB_LEN_EP_DESC)) {
			u8 addr = desc[i + 2];
			u8 dir = addr & USB_REQ_DIR_MASK;
			u8 type = desc[i + 3] & 0x03;
			if ((dir == USB_D2H) && (type == USB_CH_EP_TYPE_ISOC)) {
				desc[i + 2] = cfg->isoc_in_addr;
			} else if ((dir == USB_H2D) && (type == USB_CH_EP_TYPE_ISOC)) {
				desc[i + 2] = cfg->isoc_out_addr;
			}
		} else if ((dtype == USB_DESC_TYPE_INTERFACE) && (dlen >= USB_LEN_IF_DESC) &&
				   (desc[i + 6] == USB_UAC_SUBCLASS_AUDIOCONTROL)) {
			/* Only the audio control interface is named, matching the CDC ACM class. */
			desc[i + 8] = cdev->itf_str_idx;         /* iInterface */
		} else if ((dtype == USB_UAC_CS_INTERFACE) && (dlen >= USB_UAC1_AC_IF_HEADER_LEN) && (desc[i + 2] == USB_UAC_AC_HEADER)) {
			/* AC Header: bInCollection at offset 7, baInterfaceNr[j] at offset 8+j
			 * (Ref UAC 1.0 4.7.2). Each entry keeps its own class-local interface
			 * number, so add the base rather than overwrite. */
			u8 n = desc[i + 7];
			u8 j;

			if ((u16)(8U + n) > (u16)dlen) { /* malformed bInCollection: do not run off the descriptor */
				n = (u8)(dlen - 8U);
			}
			for (j = 0; j < n; j++) {
				desc[i + 8U + j] = (u8)(desc[i + 8U + j] + cdev->if_base);
			}
		}
		i += dlen;
	}
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
static u16 usbd_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len)
{
	usb_speed_type_t speed = dev->dev_speed;
	const u8 *desc = NULL;
	u16 len = 0;
	u8 type = USB_HIGH_BYTE(req->wValue);
	u8 is_cfg = 0;

	if (speed == USB_SPEED_HIGH) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid speed, UAC 1.0 only support full speed\n");
		return len;
	}

	switch (type) {

	case USB_DESC_TYPE_DEVICE:
		desc = usbd_uac_dev_desc;
		len = sizeof(usbd_uac_dev_desc);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		/* Built from the const template only: isoc_mps is sized once by usbd_uac_config(),
		   rewriting it here would race the OUT ISR that arms the EP with it */
		desc = usbd_uac_fs_config_desc;
		len = sizeof(usbd_uac_fs_config_desc);
		is_cfg = 1;
		break;

#ifndef CONFIG_SUPPORT_USB_FS_ONLY
	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		desc = usbd_uac_fs_config_desc;
		len = sizeof(usbd_uac_fs_config_desc);
		is_cfg = 1;
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

		usbd_uac_patch_desc(buf + USB_LEN_CFG_DESC, len - USB_LEN_CFG_DESC);
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
	const usbd_uac_cb_t *cb;

	UNUSED(dev);

	if (status == USBD_ATTACH_STATUS_DETACHED) {
		/* Detach: stop tx/rx and wake up the read thread blocked in usbd_uac_read.
		   ISR context here: only rtos_sema_give is allowed (no malloc/sema_take). */
		usbd_uac_stop_play();
		if ((cdev->uac_isoc_out.uac_sema_valid != 0) && (cdev->uac_isoc_out.read_wait_sema != 0)) {
			rtos_sema_give(cdev->uac_isoc_out.uac_isoc_sema);
		}
	}

	/* cb is released by usbd_uac_deinit(), which may run concurrently with this ISR: read once */
	cb = cdev->cb;
	if ((cb != NULL) && (cb->status_changed != NULL)) {
		cb->status_changed(old_status, status);
	}
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
/**
  * @brief  UAC status dump thread
  * @param  param: Pointer to parameters
  * @retval void
  */
static void usbd_uac_status_dump_thread(void *param)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *buf_ctrl = &(cdev->uac_isoc_out);
	cdev->isoc_dump_thread = 1;
	u32 loop_idx = 0;
	u32 max_loop = USBD_UAC_DEBUG_LOOP_TIME / USBD_UAC_DEBUG_STEP_TIME;

	UNUSED(param);

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
  * @param  copy_len: Accumulated copy length
  * @param  pkt_cnt: Accumulated packet count (used to index the zero_pkt_flag bitmap)
  * @param  zero_pkt_flag: Optional bitmap of ZLP filler packets (bit N = packet index N)
  * @retval 0 when the caller now has enough data; non-zero to signal caller to loop again.
  */
static u32 usbd_uac_read_ring_buf(usbd_uac_buf_ctrl_t *pdata_ctrl, u8 *buffer, u32 size, u32 *copy_len, u16 *pkt_cnt, u32 *zero_pkt_flag)
{
	usb_ringbuf_manager_t *buf_list = &(pdata_ctrl->buf_list);
	u32 read_len;
	u8 valid = 0;

	do {
		if ((*copy_len >= size) || ((*copy_len + pdata_ctrl->isoc_mps) > size)) {
			return HAL_OK;
		}

		/* The OUT ISR drops the head of a full ring, possibly on another SMP core */
		usb_os_enter_critical();
		read_len = usb_ringbuf_remove_head(buf_list, buffer + *copy_len, size - *copy_len, &valid);
		usb_os_exit_critical();
		if (read_len > 0) {
			if (valid == 0U) {
				/* A filler node carries no payload, the ring hands back the stale bytes of the
				   slot: play it as silence */
				usb_os_memset((void *)(buffer + *copy_len), 0U, read_len);
				if ((zero_pkt_flag != NULL) && (*pkt_cnt < 32U)) {
					*zero_pkt_flag |= (1U << (*pkt_cnt));
				}
			}
			*copy_len += read_len;
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
static int usbd_uac_private_init(const usbd_uac_cb_t *cb, const usbd_uac_config_t *cfg)
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

	if (cfg == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid cfg\n");
		return HAL_ERR_PARA;
	}

	/* UAC 1.0 is playback only: the configuration descriptor has no AS IN endpoint, so an
	   enabled IN direction would never be opened by the host. Record is served by UAC 2.0. */
	if (cfg->in.enable != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC1 no record\n");
		return HAL_ERR_PARA;
	}

	/* The OUT direction is then the only stream this class owns. */
	if (cfg->out.enable == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Pls cfg UAC EP\n");
		return HAL_ERR_PARA;
	}

	/* A direction that is enabled needs its endpoint, with the right direction bit. */
	if (((cfg->in.enable != 0) && (((cfg->isoc_in_addr & USB_REQ_DIR_MASK) != USB_D2H) ||
								   ((cfg->isoc_in_addr & (~USB_REQ_DIR_MASK)) == 0U))) ||
		((cfg->out.enable != 0) && (((cfg->isoc_out_addr & USB_REQ_DIR_MASK) != USB_H2D) ||
									((cfg->isoc_out_addr & (~USB_REQ_DIR_MASK)) == 0U)))) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid EP addr %02x/%02x\n", cfg->isoc_in_addr, cfg->isoc_out_addr);
		return HAL_ERR_PARA;
	}

	/* Published before anything that reads it: the descriptor patch path, the endpoint setup
	   below and the ISOC buffer sizing all resolve their options through cdev->cfg. */
	cdev->cfg = cfg;

	cdev->cur_volume = (cfg->volume_default != 0U) ? cfg->volume_default : USBD_UAC_DEFAULT_VOLUME;
	cdev->cur_mute = (cfg->mute_default != 0U) ? 1U : 0U;
	cdev->cur_clk_valid = 1;
	/* Default sampling rate so the first SET_INTERFACE(alt=1) can fire
	 * format_changed(sampling_freq, ch_cnt, byte_width) with all three
	 * fields non-zero. Some UAC1 hosts skip the class-specific
	 * SET_CUR SAMPLING_FREQ request when the AS descriptor already lists a
	 * discrete rate — without a non-zero default here, format_changed would
	 * be delivered as (0, ch, bw) and the app-level (uac_ready_sema) never
	 * fires, so playback stays silent even though the device enumerates. */
	cdev->cur_play_sampling_freq = USBD_UAC_SAMPLING_FREQ_48K;
	/* Standalone default; the composite framework rebases it via set_interface_base() */
	cdev->if_base = 0;

	usbd_uac_ep_buf_ctrl_deinit(&(cdev->uac_isoc_in));
	usbd_uac_ep_buf_ctrl_deinit(&(cdev->uac_isoc_out));

	/* No H2D request is waiting for its data stage yet. The device context is a static object,
	   so a re-init after deinit must not inherit a stale pending flag. */
	cdev->ctrl_req_pending = 0U;

	/* The core owns the string table, so the same call works in standalone and composite
	   mode. 0 means no iInterface string. */
	cdev->itf_str_idx = usbd_add_string(cfg->itf_str);

	info = &ep_isoc_out->info;
	info->addr = cfg->isoc_out_addr;
	info->type = USB_CH_EP_TYPE_ISOC;
	info->binterval = USBD_UAC_FS_DEFAULT_BINTERVAL;
	ep_isoc_out->xfer_buf_len = (u32)sizeof(usbd_uac_rx_buf);
	info = &ep_isoc_in->info;
	info->addr = cfg->isoc_in_addr;
	info->type = USB_CH_EP_TYPE_ISOC;
	info->binterval = USBD_UAC_FS_DEFAULT_BINTERVAL;

	cdev->cb = cb;

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			return ret;
		}
	}

#if UABD_UAC_DESC_DUMP
	usbd_uac_desc_dump(usbd_uac_fs_config_desc, sizeof(usbd_uac_fs_config_desc) / sizeof(u8));
#endif

#if USBD_UAC_DEBUG
	rtos_task_t task_dump;
	if (rtos_task_create(&task_dump, ((const char *)"usbd_uac_status_dump_thread"), usbd_uac_status_dump_thread, NULL, 1024U, 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create usb status dump task fail\n");
	}
#endif

	return ret;
}

/**
  * @brief  Receive UAC data
  * @param  void
  * @retval Status
  */
static int usbd_uac_receive_data(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usb_dev_t *dev = cdev->dev;
	usbd_uac_buf_ctrl_t *pbuf_ctrl = &(cdev->uac_isoc_out);
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	int ret = HAL_OK;

	if (dev == NULL) {
		return HAL_OK;
	}

	if ((pbuf_ctrl->isoc_mps == 0) || (pbuf_ctrl->uac_sema_valid == 0)) {
		return HAL_ERR_HW;
	}

	if (usbd_uac_ep_enable(&(cdev->cfg->out)) == 0) {
		return HAL_OK;
	}

	/* One critical section with the configured check: a bus reset or SET_CONFIGURATION(0) handled
	   by the ISR on another core runs clear_config(), which must not be undone by an EP armed right
	   after it. dev->is_ready can not tell: the core clears it only after clear_config() returned,
	   and not at all on a bus reset. None of the calls below blocks, they nest the same lock. */
	usb_os_enter_critical();
	if (cdev->is_configured != 0U) {
		usb_ringbuf_reset(&(pbuf_ctrl->buf_list));
		pbuf_ctrl->sof_idx = 0;
		pbuf_ctrl->data_idx = 0;
		pbuf_ctrl->xfer_cnt = 0;
		pbuf_ctrl->next_xfer = 1;
		/* Arm the ISOC OUT EP hardware. Only the SET_CUR SAMPLING_FREQ
		 * request path ep_inits the EP, and some UAC1 hosts skip that class
		 * request when the AS descriptor already lists a discrete sample rate,
		 * so we do it here where mps is known-good. deinit+init is idempotent —
		 * a fresh EP is a no-op deinit, and re-init on an already-open EP
		 * simply reprograms the FIFO for the new mps. */
		ep_isoc_out->info.mps = pbuf_ctrl->isoc_mps;
		usbd_ep_deinit(dev, ep_isoc_out);
		usbd_ep_init(dev, ep_isoc_out);
		ep_isoc_out->xfer_buf = usbd_uac_rx_buf;
		ep_isoc_out->xfer_len = pbuf_ctrl->isoc_mps;
		ret = usbd_ep_receive(dev, ep_isoc_out);
	} else {
		/* Nothing armed, the stream did not start: let usbd_uac_start_play() fail so that the
		   caller retries instead of reading a stream that never comes */
		ret = HAL_ERR_HW;
	}
	usb_os_exit_critical();

	return ret;
}

/**
  * @brief  Read the OUT ring buffer, refer to usbd_uac_read()
  * @note   Called by usbd_uac_read() between its gate updates, so the ring and the semaphore
  *         stay valid for the whole call.
  * @param  pdata_ctrl: OUT buffer control structure
  * @param  buffer: Destination buffer
  * @param  size: Destination capacity
  * @param  time_out_ms: Wait time, 0 for no wait
  * @param  zero_pkt_flag: Optional bitmap of filler packets, already cleared by the caller
  * @retval Copied length in bytes
  */
static u32 usbd_uac_read_locked(usbd_uac_buf_ctrl_t *pdata_ctrl, u8 *buffer, u32 size, u32 time_out_ms, u32 *zero_pkt_flag)
{
	u32 copy_len = 0;
	u16 pkt_cnt = 0;

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
				/* is_closing: the buffer teardown waits for this call to leave */
				if ((pdata_ctrl->uac_sema_valid == 0) || (pdata_ctrl->is_closing != 0U)) {
					pdata_ctrl->read_wait_sema = 0;
					break;
				}
				if (rtos_sema_take(pdata_ctrl->uac_isoc_sema, time_out_ms) != RTK_SUCCESS) {
					pdata_ctrl->read_wait_sema = 0;
					break;
				}
				pdata_ctrl->read_wait_sema = 0;
				if (pdata_ctrl->is_closing != 0U) {
					break;
				}
			} else {
				/* if did not read any pkt, loop to check the wr/rd pos*/
				if ((usbd_uac_read_ring_buf(pdata_ctrl, buffer, size, &copy_len, &pkt_cnt, zero_pkt_flag) == HAL_OK)
					|| (copy_len > 0)) {
					break;
				}
			}
		} while (pdata_ctrl->next_xfer != 0);
	}

	return copy_len;
}

/* Exported functions --------------------------------------------------------*/

#ifdef CONFIG_USBD_COMPOSITE
int usbd_composite_uac_init(const usbd_uac_cb_t *cb, const usbd_uac_config_t *cfg)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	int ret;

	ret = usbd_uac_private_init(cb, cfg);
	if (ret == HAL_OK) {
		cdev->from_composite = 1;
		ret = usbd_composite_register_driver(&usbd_uac_driver);
		if (ret != HAL_OK) {
			/* private_init completed, so deinit is its exact reverse. from_composite is
			   already 1, so the unregister inside is a no-op for an unregistered driver. */
			usbd_uac_deinit();
		}
	}
	return ret;
}
#endif

int usbd_uac_init(const usbd_uac_cb_t *cb, const usbd_uac_config_t *cfg)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	int ret;

	ret = usbd_uac_private_init(cb, cfg);
	if (ret == HAL_OK) {
		cdev->from_composite = 0;
		usbd_register_class(&usbd_uac_driver);
	}
	return ret;
}
/**
  * @brief  DeInitialize UAC device
  * @param  void
  * @retval None
  */
void usbd_uac_deinit(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

#ifdef CONFIG_USBD_COMPOSITE
	/* Leave the composite framework before anything is torn down: it is refused while the
	   composite device is still registered on the core, and the class must then stay intact */
	if (cdev->from_composite != 0U) {
		if (usbd_composite_unregister_driver(&usbd_uac_driver) != HAL_OK) {
			return;
		}
	}
#endif

#if USBD_UAC_DEBUG
	cdev->isoc_dump_thread = 0;
#endif
#ifdef CONFIG_USBD_COMPOSITE
	if (cdev->from_composite == 0U)
#endif
	{
		usbd_unregister_class();
	}

	usbd_uac_ep_buf_ctrl_deinit(&(cdev->uac_isoc_in));
	usbd_uac_ep_buf_ctrl_deinit(&(cdev->uac_isoc_out));

	/* Unregistered above: no class callback can run afterwards, so dropping the pending
	   control request here cannot race an EP0 OUT completion in ISR context. */
	cdev->ctrl_req_pending = 0U;

	if (cdev->cb != NULL) {
		if (cdev->cb->deinit != NULL) {
			cdev->cb->deinit();
		}
		cdev->cb = NULL;
	}
}
/**
  * @brief  Transmit UAC data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbd_uac_transmit_data(const u8 *buf, u32 len)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usb_dev_t *dev = cdev->dev;

	UNUSED(buf);

	if (dev == NULL || dev->is_ready == 0) {
		return HAL_OK;
	}

	if (len > cdev->uac_isoc_in.isoc_mps) {
		len = cdev->uac_isoc_in.isoc_mps;
	}

	return HAL_ERR_HW;
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

	/* cdev->dev is assigned in set_config, so it is NULL until the host has enumerated
	   us. The MPS / ring buffer sizing below needs dev_speed, which is only known after
	   the speed negotiation, so reject the call instead of dereferencing NULL: the app
	   is expected to re-issue usbd_uac_config() once attached. */
	if (cdev->dev == NULL) {
		return HAL_ERR_HW;
	}

	if ((uac_cfg != NULL) && usbd_uac_ep_enable(uac_cfg) != 0) {
		/* UAC 1.0 is playback only, refer to usbd_uac_private_init() */
		if ((is_record != 0) || (usbd_uac_check_play_fmt(uac_cfg, cdev->dev->dev_speed) != HAL_OK)) {
			return HAL_ERR_PARA;
		}
		pbuf_ctrl = &(cdev->uac_isoc_out);

		/* Stop the stream, the application restarts it with usbd_uac_start_play() */
		pbuf_ctrl->next_xfer = 0;

		/* Allocates the ring on the first call only, refer to usbd_uac_ep_buf_ctrl_init() */
		ret = usbd_uac_ep_buf_ctrl_init(pbuf_ctrl, uac_cfg, cdev->dev->dev_speed);
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
	cdev->uac_isoc_in.next_xfer = 0;
}

/**
  * @brief  Start UAC device record (microphone)
  * @note   UAC 1.0 class exposes no ISOC IN streaming interface, recording is not supported.
  * @param  void
  * @retval HAL_ERR_HW
  */
int usbd_uac_start_record(void)
{
	return HAL_ERR_HW;
}

/**
  * @brief  Stop UAC device record (microphone)
  * @note   UAC 1.0 class does not support recording, nothing to stop.
  * @param  void
  * @retval void
  */
void usbd_uac_stop_record(void)
{
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
	u32 copy_len;

	if (zero_pkt_flag != NULL) {
		*zero_pkt_flag = 0;
	}

	if (dev == NULL || dev->is_ready == 0) {
		return 0U;
	}

	/* Counted under the same lock the buffer teardown closes the gate with, so the teardown
	   either sees this caller or this caller sees is_closing: never both missed */
	usb_os_enter_critical();
	if ((pdata_ctrl->is_closing != 0U) || (pdata_ctrl->uac_sema_valid == 0U)) {
		usb_os_exit_critical();
		return 0U;
	}
	pdata_ctrl->xfer_users++;
	usb_os_exit_critical();

	copy_len = usbd_uac_read_locked(pdata_ctrl, buffer, size, time_out_ms, zero_pkt_flag);

	usb_os_enter_critical();
	pdata_ctrl->xfer_users--;
	usb_os_exit_critical();

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

u32 usbd_uac_get_read_frame_time_in_us(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_out);
	u8 binterval = pdata_ctrl->binterval;

	if (binterval == 0U) {
		binterval = USBD_UAC_FS_DEFAULT_BINTERVAL;
	}

	/* FS: one microframe per bInterval == USBD_UAC_ONE_KHZ us at bInterval=1 */
	return (u32)usb_ringbuf_get_count(&(pdata_ctrl->buf_list)) * USBD_UAC_ONE_KHZ * USBD_UAC_POW2(binterval - 1U);
}
