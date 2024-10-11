/**
  ******************************************************************************
  * @file    usbd_uac.c
  * @author  Realsil WLAN5 Team
  * @brief   This file provides the functionalities of the USB UAC Class
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "usbd_uac.h"
#include "usbd.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#define UABD_UAC_DESC_DUMP (0)

/* Private function prototypes -----------------------------------------------*/

static int usbd_uac_set_config(usb_dev_t *dev, u8 config);
static int usbd_uac_clear_config(usb_dev_t *dev, u8 config);
static int usbd_uac_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u8 *usbd_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len);
static int usbd_uac_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);
static int usbd_uac_handle_ep0_data_out(usb_dev_t *dev);
static int usbd_uac_handle_sof(usb_dev_t *dev);
static void usbd_uac_status_changed(usb_dev_t *dev, u8 status);
static int  usbd_uac_receive_isoc_data(void);
#if UABD_UAC_DESC_DUMP
static int  usbd_uac_desc_dump(u8 *pbuf, int len);
#endif
static u16  usbd_uac_get_mps(usbd_audio_cfg_t *params, u8 speed);
static void usbd_uac_add_function_unit_description(u8 *data_buf, const u8 *desc, u8 ch_cnt, u16 *len);
static u8   usbd_uac_get_ch_config(u8 ch_cnt);
static u16  usbd_uac_get_ot_type(u8 ch_cnt);
static void usbd_uac_update_description(void);
static inline u8 usbd_uac_ep_enable(usbd_audio_cfg_t *ep);

/* Private variables ---------------------------------------------------------*/
static const char *TAG = "UAC";

/* USB Standard Device Descriptor */
static u8 usbd_uac_dev_desc[USB_LEN_DEV_DESC] USB_DMA_ALIGNED = {
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
static u8 usbd_uac_lang_id_desc[USB_LEN_LANGID_STR_DESC] USB_DMA_ALIGNED = {
	USB_LEN_LANGID_STR_DESC,              /* bLength */
	USB_DESC_TYPE_STRING,                 /* bDescriptorType */
	USB_LOW_BYTE(USBD_UAC_LANGID_STRING), /* wLANGID */
	USB_HIGH_BYTE(USBD_UAC_LANGID_STRING),
}; /* usbd_uac_lang_id_desc */

/* USB Standard Device Qualifier Descriptor */
static u8 usbd_uac_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] USB_DMA_ALIGNED = {
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

/* USB UAC Device Configuration Descriptor */
static const u8 usbd_uac_config_desc[USB_LEN_CFG_DESC] = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,                  /* bLength */
	USB_DESC_TYPE_CONFIGURATION,       /* bDescriptorType */
	0x00,                              /* wTotalLength */
	0x00,
	0x00,                              /* Number of interfaces supported by this configuration */
	0x01,                              /* Value to use as an argument to the SetConfiguration() request to select this configuration */
	0x00,                              /* Index of string descriptor describing this configuration */
#if USBD_UAC_SELF_POWERED
	0xC0,                              /* bmAttributes: self powered */
#else
	0x80,                              /* bmAttributes: bus powered */
#endif
	0x32,                              /* bMaxPower */
};

/* 4.6 Interface Association Descriptor */
static const u8 usbd_uac_association_desc[USB_LEN_IAD_DESC] = {
	/* IAD Descriptor */
	USB_LEN_IAD_DESC,                  /* Size of this descriptor (byte_width) */
	USBD_UAC_IA_DESCRIPTOR,            /* Interface Association Descriptor type */
	0x00,                              /* First Index: Audio Control Interface Index - Headset (0) */
	0x00,                              /* Audio Control Interface, Audio Streaming Interfaces () */
	USBD_UAC_IF_CLASS_AUDIO,           /* Audio Device Class  */
	0x00,                              /* No subclass */
	USBD_UAC_VERSION_02_00,            /* Audio Protocol IP version 2.00 */
	0x00,                              /* Function string descriptor index (0) */
};

/* 4.7.1 Standard AC Interface Descriptor */
static const u8 usbd_uac_ac_if_desc[USB_LEN_IF_DESC] = {
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
};

/* 4.7.2 Class-Specific AC Interface Descriptor */
static const u8 usbd_uac_ac_if_header_desc[USBD_UAC_LEN_CTRL_IF_HEADER] = {
	/* Audio headset */
	USBD_UAC_LEN_CTRL_IF_HEADER,              /* Size of the descriptor, in byte_width  */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,    /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AC_IF_DESC_SUBTYPE_HEADER,       /* HEADER descriptor subtype  */
	USB_LOW_BYTE(0x0200),                     /* bcdUSB */
	USB_HIGH_BYTE(0x0200),
	USBD_UAC_FUNC_CATEGORY_CODE_DESKTOP_SPEAKER, /* DESKTOP_SPEAKER(0x01) : Indicating the primary use of this audio function   */
	0x00,
	0x00,                                     /* Total number of byte_width returned for the class-specific AudioControl interface descriptor. Includes
                                                      the combined length of this descriptor header and all Unit and Terminal descriptors.   */
	0x00,                                     /* D1..0: Latency Control  */
};

static const u8 usbd_uac_ac_out_clk_src_desc[USBD_UAC_LEN_CLK_SRC_DESC] = {
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
};

static const u8 usbd_uac_ac_out_interminal_desc[USBD_UAC_LEN_AC_IN_TTY_DESC] = {
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
	0x00,                                                    /* This Terminal's output audio channel cluster has 4 logical output channels   */
	0x00,
	0x00,
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
};

static const u8 usbd_uac_ac_out_function_unit_desc[USBD_UAC_LEN_FUNC_UNIT_DESC] = {
	/* FEATURE_UNIT descriptor subtype   */
	USBD_UAC_LEN_FUNC_UNIT_DESC,                             /* Size of the descriptor, in byte_width  : 6 + (4 + 1) * 4 */
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
};

static const u8 usbd_uac_ac_out_outterminal_desc[USBD_UAC_LEN_AC_OUT_TTY_DESC] = {
	/* OUTPUT_TERMINAL descriptor subtype   */
	USBD_UAC_LEN_AC_OUT_TTY_DESC,                             /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                    /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AC_IF_DESC_SUBTYPE_OUTPUT_TERMINAL,              /* OUTPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES, /* Constant uniquely identifying the Terminal within the audio
                                                                 function. This value is used in all requests to address this Terminal.   */
	0x00,
	0x00,                                                     /* A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The
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
};

/* 4.9.1 Standard AS Interface Descriptor */
static const u8 usbd_uac_as_if_desc[USB_LEN_IF_DESC] = {
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USBD_UAC_IF_IDX_AS_HEADSET_HEADPHONES,                    /* The number of this interface is 1.   */
	0x00,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x00,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USBD_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USBD_UAC_VERSION_02_00,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */
};

/* 4.9.2 Class-Specific AS Interface Descriptor */
static const u8 usbd_uac_as_out_if_general_desc[USBD_UAC_LEN_AS_IF_ALT_SET_DESC] = {
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AS_IF_DESC_SUBTYPE_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* The Terminal ID of the terminal to which this interface is connected */
	0x00,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	0x00,                                                    /* Number of physical channels in the AS Interface audio channel cluster */
	0x00,
	0x00,
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */
};

/* 4.9.3 Class-Specific AS Format Type Descriptor */
static const u8 usbd_uac_as_out_format_desc[USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC] = {
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AS_IF_DESC_SUBTYPE_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x03,                                                    /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	0x18,                                                    /* The number of effectively used bits from the available bits in an audio subslot   */
};

/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
static const u8 usbd_uac_as_ep_out_desc[USB_LEN_EP_DESC] = {
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USBD_UAC_ISOC_OUT_EP,                                    /* This is an OUT endpoint with endpoint number 2   */
	0x0D,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	0x00,                                                    /* wMaxPacketSize: */
	0x00,
	0x01,                                                    /* bInterval */
};

/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
static const u8 usbd_uac_as_data_ep_desc[USBD_UAC_LEN_DATA_EP_DESC] = {
	/* Headphone Audio Data Endpoint Descriptor */
	USBD_UAC_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USBD_UAC_CS_ENDPOINT_DESCRIPTOR,                         /* Class-specific Endpoint Descriptor type */
	USBD_UAC_AS_EP_DESC_SUBTYPE_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
};

/* UAC Class Driver */
static usbd_class_driver_t usbd_uac_driver = {
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

/* Private functions ---------------------------------------------------------*/

#if UABD_UAC_DESC_DUMP
static int usbd_uac_desc_dump(u8 *pbuf, int len)
{
	int i = 0;
	int j;
	char len_val = 0;
	for (i = 0; i < len;) {
		len_val = pbuf[i];
		for (j = 0; j < len_val; j++) {
			RTK_LOGS(TAG, "%02x ", pbuf[i + j]);
		}
		i += len_val;
		RTK_LOGS(TAG, "\n");
		if (len_val < 6) {
			break;
		}
	}

	RTK_LOGS(TAG, "\n");
	return 0;
}
#endif

static u16 usbd_uac_get_mps(usbd_audio_cfg_t *params, u8 speed)
{
	u16 mps_value;
	if (NULL == params) {
		RTK_LOGS(TAG, "[UAC] Param is error\n");
		return 0;
	}

	mps_value = params->ch_cnt * params->byte_width * params->freqence / 1000;

	if (speed == USB_SPEED_HIGH) { //high speed
		mps_value = mps_value / 8;
		if (mps_value > USBD_UAC_HS_ISOC_MPS) {
			RTK_LOGS(TAG, "[UAC] MPS %d exceed HS limited\n", mps_value);
			return 0;
		}
	} else {
		if (mps_value > USBD_UAC_FS_ISOC_MPS) {
			RTK_LOGS(TAG, "[UAC] MPS %d exceed FS limited\n", mps_value);
			return 0;
		}
	}

	return mps_value;
}

static void usbd_uac_add_function_unit_description(u8 *data_buf, const u8 *desc, u8 ch_cnt, u16 *len)
{
	u8 ch_bitmap[4] = {0, 0, 0, 0};
	u16 off;
	u8 idx;

	usb_os_memcpy((void *)data_buf, (void *)desc, USBD_UAC_LEN_FUNC_UNIT_DESC);

	/* update the FU size */
	data_buf[0] = USBD_UAC_LEN_FUNC_UNIT_DESC + 1 + ch_cnt * 4;
	off = USBD_UAC_LEN_FUNC_UNIT_DESC;

	for (idx = 0; idx < ch_cnt; idx++) {
		usb_os_memcpy((void *)(data_buf + off), (void *)ch_bitmap, 4);
		off += 4;
	}

	data_buf[off++] = 0x00;

	*len += off;
}

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

static u16 usbd_uac_get_ot_type(u8 ch_cnt)
{

	switch (ch_cnt) {
	case 2:
		return 0x0301;
	case 4:
		return 0x0302;
	case 6:
	case 8:
		return 0x0306;
	default:
		return 0x0301;
	}

	return 0x0301;
}

static void usbd_uac_update_description(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_audio_cfg_t *p_ep_cfg = NULL;
	u16 ac_if_head_desc_len = 0;
	u16 buf_offset;
	u8 ac_if_num;

	/* init the params*/
	cdev->cfg_desc_buf_len = 0;

	/* Configuration Descriptor */
	buf_offset = 0;
	usb_os_memcpy((void *)(cdev->cfg_desc_buf + buf_offset), (void *)usbd_uac_config_desc, USB_LEN_CFG_DESC);
	buf_offset += USB_LEN_CFG_DESC;

	/* association */
	usb_os_memcpy((void *)(cdev->cfg_desc_buf + buf_offset), (void *)usbd_uac_association_desc, USB_LEN_IAD_DESC);
	buf_offset += USB_LEN_IAD_DESC;

	/* add audio control if descriptor length */
	/* ac if */
	usb_os_memcpy((void *)(cdev->cfg_desc_buf + buf_offset), (void *)usbd_uac_ac_if_desc, USB_LEN_IF_DESC);
	buf_offset += USB_LEN_IF_DESC;
	ac_if_num = 1;

	/* ac header */
	usb_os_memcpy((void *)(cdev->cfg_desc_buf + buf_offset), (void *)usbd_uac_ac_if_header_desc, USBD_UAC_LEN_CTRL_IF_HEADER);
	buf_offset += USBD_UAC_LEN_CTRL_IF_HEADER;
	ac_if_head_desc_len += USBD_UAC_LEN_CTRL_IF_HEADER;

	/* out */
	p_ep_cfg = &(cdev->cb->out);
	if (p_ep_cfg && p_ep_cfg->enable) {
		/* clk src*/
		usb_os_memcpy((void *)(cdev->cfg_desc_buf + buf_offset), (void *)usbd_uac_ac_out_clk_src_desc, USBD_UAC_LEN_CLK_SRC_DESC);
		buf_offset += USBD_UAC_LEN_CLK_SRC_DESC;

		/* input terminal */
		/* update the audio format first */
		usb_os_memcpy((void *)(cdev->cfg_desc_buf + buf_offset), (void *)usbd_uac_ac_out_interminal_desc, USBD_UAC_LEN_AC_IN_TTY_DESC);
		cdev->cfg_desc_buf[buf_offset + USBD_UAC_IT_DESC_CH_CNT_OFFSET] = p_ep_cfg->ch_cnt;
		cdev->cfg_desc_buf[buf_offset + USBD_UAC_IT_DESC_CH_CFG_OFFSET] = usbd_uac_get_ch_config(p_ep_cfg->ch_cnt);
		buf_offset += USBD_UAC_LEN_AC_IN_TTY_DESC;

		/* function unit */
		usbd_uac_add_function_unit_description(cdev->cfg_desc_buf + buf_offset, usbd_uac_ac_out_function_unit_desc, p_ep_cfg->ch_cnt, &buf_offset);

		/* output terminal */
		/* update the OT type */
		usb_os_memcpy((void *)(cdev->cfg_desc_buf + buf_offset), (void *)usbd_uac_ac_out_outterminal_desc, USBD_UAC_LEN_AC_OUT_TTY_DESC);
		cdev->cfg_desc_buf[buf_offset + USBD_UAC_OT_DESC_TYPE_OFFSET] = usbd_uac_get_ot_type(p_ep_cfg->ch_cnt);
		buf_offset += USBD_UAC_LEN_AC_OUT_TTY_DESC;

		ac_if_head_desc_len += USBD_UAC_LEN_CLK_SRC_DESC + USBD_UAC_LEN_AC_IN_TTY_DESC + (USBD_UAC_LEN_FUNC_UNIT_DESC + 1 + 4 * p_ep_cfg->ch_cnt) +
							   USBD_UAC_LEN_AC_OUT_TTY_DESC;
	}

	/* in */
	p_ep_cfg = &(cdev->cb->in);
	if (p_ep_cfg && p_ep_cfg->enable) {
		//TODO
	}

	/* add audio streaming descriptor length */
	/* out */
	p_ep_cfg = &(cdev->cb->out);
	if (p_ep_cfg && p_ep_cfg->enable) {
		/* AS_INTERFACE_DESCRIPTOR(0) */
		usb_os_memcpy((void *)(cdev->cfg_desc_buf + buf_offset), (void *)usbd_uac_as_if_desc, USB_LEN_IF_DESC);
		cdev->cfg_desc_buf[buf_offset + USBD_UAC_AS_IF_DESC_NUM_OFFSET] = ac_if_num;
		cdev->cfg_desc_buf[buf_offset + USBD_UAC_AS_IF_DESC_ALT_OFFSET] = 0; // alt 0
		cdev->cfg_desc_buf[buf_offset + USBD_UAC_AS_IF_DESC_EP_OFFSET] = 0;  // ep  0
		buf_offset += USB_LEN_IF_DESC;

		/* AS_INTERFACE_DESCRIPTOR(1) */
		usb_os_memcpy((void *)(cdev->cfg_desc_buf + buf_offset), (void *)usbd_uac_as_if_desc, USB_LEN_IF_DESC);
		cdev->cfg_desc_buf[buf_offset + USBD_UAC_AS_IF_DESC_ALT_OFFSET] = 1; // alt 1
		cdev->cfg_desc_buf[buf_offset + USBD_UAC_AS_IF_DESC_EP_OFFSET] = 1;  // ep  1
		buf_offset += USB_LEN_IF_DESC;

		/* AUDIO_STREAMING_GENERAL_DESCRIPTOR */
		usb_os_memcpy((void *)(cdev->cfg_desc_buf + buf_offset), (void *)usbd_uac_as_out_if_general_desc, USBD_UAC_LEN_AS_IF_ALT_SET_DESC);
		cdev->cfg_desc_buf[buf_offset + USBD_UAC_AS_IF_DESC_CH_CNT_OFFSET] = p_ep_cfg->ch_cnt;
		cdev->cfg_desc_buf[buf_offset + USBD_UAC_AS_IF_DESC_CH_OFFSET] = usbd_uac_get_ch_config(p_ep_cfg->ch_cnt);
		buf_offset += USBD_UAC_LEN_AS_IF_ALT_SET_DESC;

		/* Format */
		usb_os_memcpy((void *)(cdev->cfg_desc_buf + buf_offset), (void *)usbd_uac_as_out_format_desc, USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC);
		cdev->cfg_desc_buf[buf_offset + USBD_UAC_AS_FORMAT_DESC_SLOT_SIZE_OFFSET] = p_ep_cfg->byte_width;
		cdev->cfg_desc_buf[buf_offset + USBD_UAC_AS_FORMAT_DESC_BIT_CNT_OFFSET] = p_ep_cfg->byte_width * 8;
		buf_offset += USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC;

		/* isoc ep */
		usb_os_memcpy((void *)(cdev->cfg_desc_buf + buf_offset), (void *)usbd_uac_as_ep_out_desc, USB_LEN_EP_DESC);
		cdev->cfg_desc_buf[buf_offset + USBD_UAC_AS_EP_DESC_MPS_OFFSET + 0] = USB_LOW_BYTE(cdev->isoc_out_buf_len);
		cdev->cfg_desc_buf[buf_offset + USBD_UAC_AS_EP_DESC_MPS_OFFSET + 1] = USB_HIGH_BYTE(cdev->isoc_out_buf_len);
		buf_offset += USB_LEN_EP_DESC;

		/* uac data ep */
		usb_os_memcpy((void *)(cdev->cfg_desc_buf + buf_offset), (void *)usbd_uac_as_data_ep_desc, USBD_UAC_LEN_DATA_EP_DESC);
		buf_offset += USBD_UAC_LEN_DATA_EP_DESC;

		ac_if_num++;
	}
	/* in */
	p_ep_cfg = &(cdev->cb->in);
	if (p_ep_cfg && p_ep_cfg->enable) {
		//TODO
	}

	cdev->cfg_desc_buf_len = buf_offset;

	/* update uac association if count */
	cdev->cfg_desc_buf[USBD_UAC_ASSOCIATION_IF_NUM_OFFSET] = ac_if_num;

	/* update uac ac header wTotalLength */
	cdev->cfg_desc_buf[USBD_UAC_AC_IF_HEADER_LEN_OFFSET + 0] = USB_LOW_BYTE(ac_if_head_desc_len);
	cdev->cfg_desc_buf[USBD_UAC_AC_IF_HEADER_LEN_OFFSET + 1] = USB_HIGH_BYTE(ac_if_head_desc_len);

	/* update Configuration wTotalLength */
	cdev->cfg_desc_buf[USBD_UAC_CFG_LEN_OFFSET + 0] = USB_LOW_BYTE(cdev->cfg_desc_buf_len);
	cdev->cfg_desc_buf[USBD_UAC_CFG_LEN_OFFSET + 1] = USB_HIGH_BYTE(cdev->cfg_desc_buf_len);
	cdev->cfg_desc_buf[USBD_UAC_CFG_IF_CNT_OFFSET] = ac_if_num;
	/* update Configuration interface num */

	RTK_LOGS(TAG, "[UAC] Desc %d %d %d\n", USBD_UAC_CTRL_BUF_SIZE, cdev->cfg_desc_buf_len, ac_if_head_desc_len);
}

static inline u8 usbd_uac_ep_enable(usbd_audio_cfg_t *ep)
{
	if (ep) {
		return ep->enable;
	}

	return  0;
}

static void usbd_uac_cur_freq_req(usb_dev_t *dev, u32 frequencyHz)
{
	u32 dCur = frequencyHz;
	//RTK_LOGS(TAG, "[UAC] ctrl freq frequencyHz(%d)\n", frequencyHz);
	memcpy(dev->ctrl_buf, &dCur, sizeof(dCur));
	usbd_ep0_transmit(dev, dev->ctrl_buf, sizeof(dCur));
}

static void usbd_uac_freq_ctrl_range_req(usb_dev_t *dev, u32 minFrequencyHz, u32 maxFrequencyHz)
{
	usbd_uac_freq_ctrl_range_t response;
	response.wNumSubRanges = 1;
	response.SubRanges[0].bMIN = minFrequencyHz;
	response.SubRanges[0].bMAX = maxFrequencyHz;
	response.SubRanges[0].bRES = 0;
	//RTK_LOGS(TAG, "[UAC] ctrl range min(%d) max(%d)\n", minFrequencyHz, maxFrequencyHz);
	memcpy(dev->ctrl_buf, &response, sizeof(response));
	usbd_ep0_transmit(dev, dev->ctrl_buf, sizeof(response));
}

static void usbd_uac_connect_ctrl_req(usb_dev_t *dev, u8 numChannels, u32 channelConfigMask)
{
	usbd_uac_ac_connect_ctrl_t response = {0, 0, 0};
	response.bNrChannels = numChannels;
	response.bmChannelConfig = channelConfigMask;

	//RTK_LOGS(TAG, "[UAC] ctrl req ch(%d) mask(%d)\n", numChannels, channelConfigMask);

	memcpy(dev->ctrl_buf, &response, sizeof(response));
	usbd_ep0_transmit(dev, dev->ctrl_buf, sizeof(response));
}

static int usbd_uac_receive_isoc_data(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	if (!cdev->is_ready) {
		return HAL_ERR_HW;
	}

	if (usbd_uac_ep_enable(&(cdev->cb->out))) {
		usbd_ep_receive(cdev->dev, USBD_UAC_ISOC_OUT_EP, cdev->isoc_out_buf, cdev->isoc_out_buf_len);
	}

	return HAL_OK;
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

	/* Init ISO IN EP */
	if (usbd_uac_ep_enable(&(cdev->cb->in))) {
		usbd_ep_init(dev, USBD_UAC_ISOC_IN_EP, USB_CH_EP_TYPE_ISOC, cdev->isoc_in_buf_len);
	}

	/* Init ISO OUT EP */
	if (usbd_uac_ep_enable(&(cdev->cb->out))) {
		usbd_ep_init(dev, USBD_UAC_ISOC_OUT_EP, USB_CH_EP_TYPE_ISOC, cdev->isoc_out_buf_len);
		usbd_ep_receive(dev, USBD_UAC_ISOC_OUT_EP, cdev->isoc_out_buf, cdev->isoc_out_buf_len);
	}

	cdev->is_ready = 1U;

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

	UNUSED(config);

	cdev->is_ready = 0U;

	/* DeInit ISOC IN EP */
	if (usbd_uac_ep_enable(&(cdev->cb->in))) {
		usbd_ep_deinit(dev, USBD_UAC_ISOC_IN_EP);
	}

	/* DeInit ISOC OUT EP */
	if (usbd_uac_ep_enable(&(cdev->cb->out))) {
		usbd_ep_deinit(dev, USBD_UAC_ISOC_OUT_EP);
	}

	return ret;
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
	usbd_audio_cfg_t *audio_cfg = NULL;
	u8 entityId, controlSelector;
	int ret = HAL_OK;

	//RTK_LOGS(TAG, "[UAC] SETUP: bmRequestType=0x%02x bRequest=0x%02x wValue=%x wIndex=%x wLength=0x%04x\n",
	//	req->bmRequestType, req->bRequest, req->wValue, req->wIndex, req->wLength);

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				cdev->alt_setting = (u8)(req->wValue & 0xFF);
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				cdev->ctrl_buf[0] = cdev->alt_setting;
				usbd_ep0_transmit(dev, cdev->ctrl_buf, 1U);
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		case USB_REQ_GET_STATUS:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				cdev->ctrl_buf[0] = 0U;
				cdev->ctrl_buf[1] = 0U;
				usbd_ep0_transmit(dev, cdev->ctrl_buf, 2U);
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		default:
			ret = HAL_ERR_HW;
			break;
		}
		break;
	case USB_REQ_TYPE_CLASS :
		entityId = USB_HIGH_BYTE(req->wIndex);
		controlSelector = USB_HIGH_BYTE(req->wValue);

		//RTK_LOGS(TAG, "[UAC] SETUP: id=0x%02x cs=%d\n", entityId, controlSelector);

		if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
			switch (entityId) {
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE: {
				if (controlSelector == USBD_UAC_CS_SAM_FREQ_CONTROL) {
					audio_cfg = &(cdev->cb->in);
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usbd_uac_cur_freq_req(dev, audio_cfg->freqence);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
						usbd_uac_freq_ctrl_range_req(dev, audio_cfg->freqence, audio_cfg->freqence);
					} else {
						RTK_LOGS(TAG, "[UAC] SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else {
					RTK_LOGS(TAG, "[UAC] SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_HW;
				}

				break;
			}

			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES: {
				if (controlSelector == USBD_UAC_CS_SAM_FREQ_CONTROL) {
					audio_cfg = &(cdev->cb->out);
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usbd_uac_cur_freq_req(dev, audio_cfg->freqence);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
						usbd_uac_freq_ctrl_range_req(dev, audio_cfg->freqence, audio_cfg->freqence);
					} else {
						RTK_LOGS(TAG, "[UAC] SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else {
					RTK_LOGS(TAG, "[UAC] SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_HW;
				}

				break;
			}

			case USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE: {
				if (controlSelector == USBD_UAC_TE_CONNECTOR_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usbd_uac_connect_ctrl_req(dev, 1, 0x1);
					} else {
						RTK_LOGS(TAG, "[UAC] SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else {
					RTK_LOGS(TAG, "[UAC] SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_HW;
				}

				break;
			}
			break;

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES: {
				if (controlSelector == USBD_UAC_TE_CONNECTOR_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usbd_uac_connect_ctrl_req(dev, 4, 0x107); // 4 channel
					} else {
						RTK_LOGS(TAG, "[UAC] SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else {
					RTK_LOGS(TAG, "[UAC] SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_HW;
				}

				break;
			}
			break;

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT: {
				if (controlSelector == USBD_UAC_CTRL_FU_MUTE_CONTROL_SELECTOR) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						dev->ctrl_buf[0] = cdev->cur_mute;
						usbd_ep0_transmit(dev, dev->ctrl_buf, 1);
					} else {
						RTK_LOGS(TAG, "[UAC] SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else if (controlSelector == USBD_UAC_CTRL_FU_VOLUME_CONTROL_SELECTOR) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						memcpy(dev->ctrl_buf, (u8 *) & (cdev->cur_volume), 2);
						usbd_ep0_transmit(dev, dev->ctrl_buf, 2);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {

						usbd_uac_ctrl_range_layout2_struct response;
						response.wNumSubRanges = 1;
						response.bMIN = USBD_UAC_VOLUME_CTRL_MIN;
						response.bMAX = USBD_UAC_VOLUME_CTRL_MAX;
						response.bRES = 1;

						memcpy(dev->ctrl_buf, &response, sizeof(response));
						usbd_ep0_transmit(dev, dev->ctrl_buf, sizeof(response));
					} else {
						RTK_LOGS(TAG, "[UAC] SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else {
					RTK_LOGS(TAG, "[UAC] SETUP: 0x13 wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_HW;
				}

				break;

				default:
					RTK_LOGS(TAG, "[UAC] SETUP: wIndex err %d\n", entityId);
					break;
				}
			}
		} else {
			/* USB_H2D */
			switch (entityId) {
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES: {
				if (controlSelector == USBD_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usb_os_memcpy((void *)&cdev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						usbd_ep0_receive(dev, cdev->ctrl_buf, req->wLength);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
					} else {
						RTK_LOGS(TAG, "[UAC] Set freq err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else {
					RTK_LOGS(TAG, "[UAC] Set freq ctrl err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_HW;
				}

				break;
			}

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT: { //0x05 FU
				if (controlSelector == USBD_UAC_CTRL_FU_MUTE_CONTROL_SELECTOR) { //mute
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usb_os_memcpy((void *)&cdev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						usbd_ep0_receive(dev, cdev->ctrl_buf, req->wLength);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
					} else {
						RTK_LOGS(TAG, "[UAC] Set cur mute err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else if (controlSelector == USBD_UAC_CTRL_FU_VOLUME_CONTROL_SELECTOR) { //volume
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usb_os_memcpy((void *)&cdev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						usbd_ep0_receive(dev, cdev->ctrl_buf, req->wLength);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
					} else {
						RTK_LOGS(TAG, "[UAC] Set cur volume range err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else {
					RTK_LOGS(TAG, "[UAC] Set fu err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_HW;
				}

				break;
			}

			default:
				RTK_LOGS(TAG, "[UAC] SETUP: wIndex err %d\n", entityId);
				break;
			}
		}
		break;

	case USB_REQ_TYPE_VENDOR:
		if (req->wLength) {
			if (req->bmRequestType & 0x80U) {
				ret = cdev->cb->setup(req, cdev->ctrl_buf);
				if (ret == HAL_OK) {
					usbd_ep0_transmit(dev, cdev->ctrl_buf, req->wLength);
				}
			} else {
				usb_os_memcpy((void *)&cdev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
				usbd_ep0_receive(dev, cdev->ctrl_buf, req->wLength);
			}
		} else {
			cdev->cb->setup(req, cdev->ctrl_buf);
		}
		break;

	default:
		RTK_LOGS(TAG, "[UAC] SETUP: bmRequestType err\n");
		ret = HAL_ERR_HW;
		break;
	}

	return ret;
}

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
  * @brief  cdc_acm_handle_ep0_data_out
  *         Handle EP0 Rx Ready event
  * @param  dev: USB device instance
  * @retval Status
  */
static int usbd_uac_handle_ep0_data_out(usb_dev_t *dev)
{
	UNUSED(dev);
	int ret = HAL_OK;
	u16 volume_value = 0;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_cb_t *cb = cdev->cb;
	usb_setup_req_t *p_ctrl_req = &(cdev->ctrl_req);

	if ((((p_ctrl_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS) && ((p_ctrl_req->bmRequestType & 0x1FU) == USB_REQ_RECIPIENT_INTERFACE))
		&& (USB_HIGH_BYTE(p_ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT)
		&& (p_ctrl_req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR)) {

		if ((cb->mute_changed != NULL)
			&& (USB_HIGH_BYTE(p_ctrl_req->wValue) == USBD_UAC_CTRL_FU_MUTE_CONTROL_SELECTOR)
			&& (p_ctrl_req->wLength == 0x01)) {
			cdev->cur_mute = (cdev->ctrl_buf[0]) ? 1 : 0;
			cb->mute_changed(cdev->cur_mute);
		} else if ((cb->volume_changed != NULL)
				   && (USB_HIGH_BYTE(p_ctrl_req->wValue) == USBD_UAC_CTRL_FU_VOLUME_CONTROL_SELECTOR)
				   && (p_ctrl_req->wLength == 0x02)) {

			volume_value = (u16)cdev->ctrl_buf[0] + ((u16)(cdev->ctrl_buf[1]) << 8);
			cdev->cur_volume = volume_value;

			//RTK_LOGS(TAG, "[UAC] USBD set volume 0x%02x\n", volume_value);

			/* 8001 ~ 7FFF TODO */
			if (volume_value & 0x8000) { //0~50
				volume_value = 50 - (u16)(50 * (u32)((0 - volume_value) & 0xFFFFU) / USBD_UAC_VOLUME_CTRL_MAX);
			} else {
				volume_value = 50 + (u16)(50 * (u32)volume_value / USBD_UAC_VOLUME_CTRL_MAX);
			}

			// covert the data to 0~100
			cb->volume_changed(USB_LOW_BYTE(volume_value));
		}
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
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_cb_t *cb = cdev->cb;

	UNUSED(dev);

	if (ep_addr == USBD_UAC_ISOC_IN_EP) {
		if (status != HAL_OK) {
			RTK_LOGS(TAG, "[UAC] ISOC TX err: %d\n", status);
		}
		if (cb->isoc_transmitted != NULL) {
			cb->isoc_transmitted(status);
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

	UNUSED(dev);

	if (len == 0) {
		/*RX ZLP*/
		return HAL_OK;
	}

	if (ep_addr == USBD_UAC_ISOC_OUT_EP) {
		cdev->cb->isoc_received(cdev->isoc_out_buf, len);
	}

	return usbd_uac_receive_isoc_data();
}

/**
  * @brief  Get descriptor callback
  * @param  req: setup request
  * @param  speed: device speed
  * @param  len: descriptor length
  * @retval descriptor buffer
  */
static u8 *usbd_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len)
{
	u8 *buf = NULL;
	u8 *desc = usbd_uac_dev.ctrl_buf;

	dev->self_powered = USBD_UAC_SELF_POWERED;

	switch ((req->wValue >> 8) & 0xFF) {

	case USB_DESC_TYPE_DEVICE:
		*len = sizeof(usbd_uac_dev_desc);
		buf = usbd_uac_dev_desc;
		usb_os_memcpy((void *)desc, (void *)buf, *len);
		buf = desc;
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		buf = usbd_uac_dev.cfg_desc_buf;
		*len = usbd_uac_dev.cfg_desc_buf_len;
		usb_os_memcpy((void *)desc, (void *)buf, *len);
		buf = desc;
		break;

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		buf = usbd_uac_device_qualifier_desc;
		*len = sizeof(usbd_uac_device_qualifier_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		buf = usbd_uac_dev.cfg_desc_buf;
		*len = usbd_uac_dev.cfg_desc_buf_len;
		usb_os_memcpy((void *)desc, (void *)buf, *len);
		desc[USB_CFG_DESC_OFFSET_TYPE] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
		buf = desc;
		break;

	case USB_DESC_TYPE_STRING:
		switch (req->wValue & 0xFF) {
		case USBD_IDX_LANGID_STR:
			buf = usbd_uac_lang_id_desc;
			*len = sizeof(usbd_uac_lang_id_desc);
			break;
		case USBD_IDX_MFC_STR:
			usbd_get_str_desc(USBD_UAC_MFG_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_PRODUCT_STR:
			if (speed == USB_SPEED_HIGH) {
				usbd_get_str_desc(USBD_UAC_PROD_HS_STRING, desc, len);
			} else {
				usbd_get_str_desc(USBD_UAC_PROD_FS_STRING, desc, len);
			}
			buf = desc;
			break;
		case USBD_IDX_SERIAL_STR:
			usbd_get_str_desc(USBD_UAC_SN_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_MS_OS_STR:
			/*Not support*/
			break;
		/* Add customer string here */
		default:
			RTK_LOGS(TAG, "[UAC] Invalid str idx %d\n", req->wValue & 0xFF);
			break;
		}
		break;

	default:
		break;
	}

	return buf;
}

/**
  * @brief  USB attach status change
  * @param  dev: USB device instance
  * @param  config: USB USB attach status
  * @retval void
  */
static void usbd_uac_status_changed(usb_dev_t *dev, u8 status)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	UNUSED(dev);

	if (status == USBD_ATTACH_STATUS_DETACHED) {
		cdev->is_ready = 0U;
	}

	if (cdev->cb->status_changed) {
		cdev->cb->status_changed(status);
	}
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize UAC device
  * @retval Status
  */
int usbd_uac_init(usbd_uac_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	cdev->cur_volume = 0x001F;
	cdev->cur_mute = 0;

	if (cb != NULL) {
		if ((cb->in.enable == 0) && (cb->out.enable == 0)) {
			RTK_LOGS(TAG, "[UAC] Pls cfg UAC EP\n");
			return HAL_ERR_PARA;
		}

		if (usbd_uac_ep_enable(&(cb->in))) {
			cdev->isoc_in_buf_len = usbd_uac_get_mps(&(cb->in), cb->speed);
			if (cdev->isoc_in_buf_len == 0) {
				RTK_LOGS(TAG, "[UAC] MPS check fail\n");
				return HAL_ERR_PARA;
			}
		} else {
			cdev->isoc_in_buf_len = 0;
			cdev->isoc_in_buf = NULL;
		}
		if (usbd_uac_ep_enable(&(cb->out))) {
			cdev->isoc_out_buf_len = usbd_uac_get_mps(&(cb->out), cb->speed);
			if (cdev->isoc_out_buf_len == 0) {
				RTK_LOGS(TAG, "[UAC] MPS check fail\n");
				return HAL_ERR_PARA;
			}
		} else {
			cdev->isoc_out_buf_len = 0;
			cdev->isoc_out_buf = NULL;
		}

		cdev->cb = cb;
	} else {
		RTK_LOGS(TAG, "[UAC] UAC cb can not be NULL\n");
		return HAL_ERR_PARA;
	}

	cdev->cfg_desc_buf = (u8 *)usb_os_malloc(USBD_UAC_CTRL_BUF_SIZE);
	if (cdev->cfg_desc_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_exit;
	}

	cdev->ctrl_buf = (u8 *)usb_os_malloc(USBD_UAC_CTRL_BUF_SIZE);
	if (cdev->ctrl_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_clean_cfg_desc_buf_exit;
	}

	if (usbd_uac_ep_enable(&(cdev->cb->out))) {
		cdev->isoc_out_buf = (u8 *)usb_os_malloc(cdev->isoc_out_buf_len);
		if (cdev->isoc_out_buf == NULL) {
			ret = HAL_ERR_MEM;
			goto init_clean_ctrl_buf_exit;
		}
	}

	if (usbd_uac_ep_enable(&(cdev->cb->in))) {
		cdev->isoc_in_buf = (u8 *)usb_os_malloc(cdev->isoc_in_buf_len);
		if (cdev->isoc_in_buf == NULL) {
			ret = HAL_ERR_MEM;
			goto init_clean_isoc_out_buf_exit;
		}
	}

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			goto init_clean_isoc_in_buf_exit;
		}
	}

	usbd_uac_update_description();
#if UABD_UAC_DESC_DUMP
	usbd_uac_desc_dump(cdev->cfg_desc_buf, cdev->cfg_desc_buf_len);
#endif

	usbd_register_class(&usbd_uac_driver);

	return ret;

init_clean_isoc_in_buf_exit:
	if (cdev->isoc_in_buf != NULL) {
		usb_os_mfree(cdev->isoc_in_buf);
		cdev->isoc_in_buf = NULL;
	}

init_clean_isoc_out_buf_exit:
	if (cdev->isoc_out_buf != NULL) {
		usb_os_mfree(cdev->isoc_out_buf);
		cdev->isoc_out_buf = NULL;
	}

init_clean_ctrl_buf_exit:
	if (cdev->ctrl_buf != NULL) {
		usb_os_mfree(cdev->ctrl_buf);
		cdev->ctrl_buf = NULL;
	}

init_clean_cfg_desc_buf_exit:
	if (cdev->cfg_desc_buf != NULL) {
		usb_os_mfree(cdev->cfg_desc_buf);
		cdev->cfg_desc_buf = NULL;
	}

init_exit:
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

	cdev->is_ready = 0U;

	if (cdev->cb != NULL) {
		if (cdev->cb->deinit != NULL) {
			cdev->cb->deinit();
		}
		cdev->cb = NULL;
	}

	usbd_unregister_class();

	if (cdev->ctrl_buf != NULL) {
		usb_os_mfree(cdev->ctrl_buf);
		cdev->ctrl_buf = NULL;
	}

	if (cdev->cfg_desc_buf != NULL) {
		usb_os_mfree(cdev->cfg_desc_buf);
		cdev->cfg_desc_buf = NULL;
	}

	if (cdev->isoc_in_buf != NULL) {
		usb_os_mfree(cdev->isoc_in_buf);
		cdev->isoc_in_buf = NULL;
	}

	if (cdev->isoc_out_buf != NULL) {
		usb_os_mfree(cdev->isoc_out_buf);
		cdev->isoc_out_buf = NULL;
	}

	return HAL_OK;
}

int usbd_uac_transmit_data(u8 *buf, u16 len)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	if (!cdev->is_ready) {
		return HAL_ERR_HW;
	}

	if (len > cdev->isoc_in_buf_len) {
		len = cdev->isoc_in_buf_len;
	}

	if (usbd_uac_ep_enable(&(cdev->cb->in))) {
		usb_os_memcpy(cdev->isoc_in_buf, buf, len);
		usbd_ep_transmit(cdev->dev, USBD_UAC_ISOC_IN_EP, cdev->isoc_in_buf, len);
	}

	return HAL_OK;
}