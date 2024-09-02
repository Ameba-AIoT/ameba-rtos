/**
  ******************************************************************************
  * The header file for usbd_uac.c
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef USBD_UAC_H
#define USBD_UAC_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"

/* Exported defines ----------------------------------------------------------*/

/*
    ISOC MPS should bigger then the trx data length for audio
    Right now we set the audio config is : 48k 4ch 24 bits
 */
#define USBD_UAC_MAX_ISOC_DATA_LENGTH              (3*4*48)

/*  Uac Test parameters */
#define USBD_UAC_ISOC_OUT_BUF_SIZE                  USBD_UAC_MAX_ISOC_DATA_LENGTH   /* ISOC OUT buffer size */
#define USBD_UAC_ISOC_IN_BUF_SIZE                   USBD_UAC_MAX_ISOC_DATA_LENGTH   /* ISOC IN buffer size */
#define USBD_UAC_CTRL_BUF_SIZE                      512U  /* CTRL buffer size */


/* Uac Endpoint parameters */
#define USBD_UAC_ISOC_IN_EP                         0x81U  /* EP1 for ISOC IN */
#define USBD_UAC_ISOC_OUT_EP                        0x02U  /* EP2 for ISOC OUT */

#define USBD_UAC_HS_ISOC_MPS                        USBD_UAC_MAX_ISOC_DATA_LENGTH   /* High speed ISOC IN & OUT max packet size */
#define USBD_UAC_FS_ISOC_MPS                        USBD_UAC_MAX_ISOC_DATA_LENGTH   /* Full speed ISOC IN & OUT max packet size */

#define USBD_UAC_HS_ISOC_IN_INTERVAL                1U    /* High speed ISOC IN interval */
#define USBD_UAC_HS_ISOC_OUT_INTERVAL               1U    /* High speed ISOC OUT interval */
#define USBD_UAC_FS_ISOC_IN_INTERVAL                1U    /* Full speed ISOC IN interval */
#define USBD_UAC_FS_ISOC_OUT_INTERVAL               1U    /* Full speed ISOC OUT interval */

// Project-specific Interface Indices
#define USBD_UAC_LEN_CLK_SRC_DESC                   0x08U
#define USBD_UAC_LEN_AC_IN_TTY_DESC                 0x11U
#define USBD_UAC_LEN_AC_OUT_TTY_DESC                0x0CU
#define USBD_UAC_LEN_AS_IF_ALT_SET_DESC             0x10U
#define USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC       0x06U
#define USBD_UAC_LEN_DATA_EP_DESC                   0x08U

//headset
#define USBD_IFACE_IDX_AUDIOCONTROL_HEADSET                       (0x00U)
#define USBD_IFACE_IDX_AUDIOSTREAMING_HEADSET_MICROPHONE          (0x01U)
#define USBD_IFACE_IDX_AUDIOSTREAMING_HEADSET_HEADPHONES          (0x02U)

#define USB_CS_IFACE_DESCRIPTOR                                   (0x24)
#define USB_CS_ENDPOINT_DESCRIPTOR                                (0x25)
#define USBD_UAC_IA_DESCRIPTOR                                    (0x0b)
#define USBD_UAC_IF_CLASS_AUDIO                                   (0x01)

// Audio Function Category Codes (UAC2.0 Spec Appendix A.7)
#define USB_AUDIO_FUNCTION_CATEGORY_CODE_DESKTOP_UNDEFINED        (0x00)
#define USB_AUDIO_FUNCTION_CATEGORY_CODE_DESKTOP_SPEAKER          (0x01)
#define USB_AUDIO_FUNCTION_CATEGORY_CODE_DESKTOP_HEADSET          (0x04)

// Audio Class-Specific Audio Control Interface Descriptor Subtype Codes (UAC2.0 Spec Appendix A.9)
#define USB_CS_AC_IFACE_DESCRIPTOR_SUBTYPE_UNDEFINED              (0x00)
#define USB_CS_AC_IFACE_DESCRIPTOR_SUBTYPE_HEADER                 (0x01)
#define USB_CS_AC_IFACE_DESCRIPTOR_SUBTYPE_INPUT_TERMINAL         (0x02)
#define USB_CS_AC_IFACE_DESCRIPTOR_SUBTYPE_OUTPUT_TERMINAL        (0x03)
#define USB_CS_AC_IFACE_DESCRIPTOR_SUBTYPE_CLOCK_SOURCE           (0x0A)

// USB Audio
#define USBD_AUDIOCONTROL_ENTITYID_CLOCK_HAPTICS                      (0x01U)
#define USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HAPTICS              (0x02U)
#define USBD_AUDIOCONTROL_ENTITYID_OUTPUTTERMINAL_HAPTICS             (0x03U)
#define USBD_AUDIOCONTROL_ENTITYID_CLOCK_HEADSET_MICROPHONE           (0x04U)
#define USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE   (0x05U)
#define USBD_AUDIOCONTROL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE  (0x06U)
#define USBD_AUDIOCONTROL_ENTITYID_CLOCK_HEADSET_HEADPHONES           (0x07U)
#define USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES   (0x08U)
#define USBD_AUDIOCONTROL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES  (0x09U)
#define USBD_AUDIOCONTROL_NUM_SUPPORTED_FREQUENCIES                   (0x01U)

// Audio UAC2
#define USB_UAC_CLASS_REQ_CODE_CUR                                  (0x01U)
#define USB_UAC_CLASS_REQ_CODE_RANGE                                (0x02U)
#define USB_UAC_CS_SAM_FREQ_CONTROL                                 (0x01U)
#define USB_UAC_TE_CONNECTOR_CONTROL                                (0x02U)

// Audio Class-Specific Audio Streaming Interface Descriptor Subtype Codes (UAC2.0 Spec Appendix A.10)
#define USB_CS_AS_IFACE_DESCRIPTOR_SUBTYPE_UNDEFINED                (0x00)
#define USB_CS_AS_IFACE_DESCRIPTOR_SUBTYPE_AS_GENERAL               (0x01)
#define USB_CS_AS_IFACE_DESCRIPTOR_SUBTYPE_FORMAT_TYPE              (0x02)

// Audio Class-Specific Audio Streaming Endpoint Descriptor Subtype Codes (UAC2.0 Spec Appendix A.13)
#define USB_CS_AS_ENDPOINT_DESCRIPTOR_SUBTYPE_UNDEFINED             (0x00)
#define USB_CS_AS_ENDPOINT_DESCRIPTOR_SUBTYPE_EP_GENERAL            (0x01)

/* Uac Device parameters */
#define USBD_UAC_VID                                USB_VID
#define USBD_UAC_PID                                USB_PID
#define USBD_UAC_CLASS_CODE                         0x01U
#define USBD_UAC_SUBCLASS_AUDIOCONTROL              0x01U
#define USBD_UAC_SUBCLASS_AUDIOSTREAMING            0x02U
#define USBD_UAC_VERSION_02_00                      0x20U

#define USBD_UAC_SELF_POWERED                       1U
#define USBD_UAC_LANGID_STRING                      0x0409U
#define USBD_UAC_MFG_STRING                         "Realtek"
#define USBD_UAC_PROD_HS_STRING                     "Realtek UAC2.0 (HS)"
#define USBD_UAC_PROD_FS_STRING                     "Realtek UAC2.0 (FS)"
#define USBD_UAC_SN_STRING                          "1234567890"

#define USBD_UAC_CONFIG_DESC_SIZE                  (263U)

/* Exported types ------------------------------------------------------------*/
typedef struct __PACKED {
	u16 wNumSubRanges;
	struct {
		u32 bMIN;
		u32 bMAX;
		u32 bRES;
	} SubRanges[1];
} usbd_uac_freq_ctrl_range_t;

typedef struct __PACKED {
	u8 bNrChannels;
	u32 bmChannelConfig;
	u8 iChannelNames;
} usbd_uac_ac_connect_ctrl_t;

typedef struct {
	int(* init)(void);
	int(* deinit)(void);
	int(* setup)(usb_setup_req_t *req, u8 *buf);
	int(* set_config)(void);
	int(* isoc_received)(u8 *buf, u32 len);
	void(* isoc_transmitted)(u8 status);
	void (*status_changed)(u8 status);
} usbd_uac_cb_t;

typedef struct {
	usb_dev_t *dev;
	usbd_uac_cb_t *cb;
	usb_setup_req_t ctrl_req;
	u8 *ctrl_buf;

	u8 *isoc_out_buf;
	u8 *isoc_in_buf;
	u8  alt_setting;

	__IO u8 is_ready;
} usbd_uac_dev_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

int usbd_uac_init(usbd_uac_cb_t *cb);
int usbd_uac_deinit(void);
int usbd_uac_transmit_data(u8 *buf, u16 len);

#endif  /* USBD_UAC_H */