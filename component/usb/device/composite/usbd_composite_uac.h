/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef USBD_COMPOSITE_UAC_H
#define USBD_COMPOSITE_UAC_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usbd_composite_config.h"

/* Exported defines ----------------------------------------------------------*/

/*
    ISOC MPS should bigger then the trx data length for audio
    Right now we set the audio config is : 48k 4ch 24 bits
 */
#define USBD_UAC_AUDIO_FORMAT_CHANNELS             (0x06U)
#define USBD_UAC_AUDIO_FORMAT_SIZE                 (0x03U)
#define USBD_UAC_AUDIO_SAMPLING_RATE_KHZ           (48U)

#define USBD_UAC_MAX_ISOC_DATA_LENGTH              ((USBD_UAC_AUDIO_SAMPLING_RATE_KHZ) * (USBD_UAC_AUDIO_FORMAT_CHANNELS) * (USBD_UAC_AUDIO_FORMAT_SIZE))

/*  Uac Test parameters */
#define USBD_UAC_ISOC_OUT_BUF_SIZE                  USBD_UAC_MAX_ISOC_DATA_LENGTH   /* ISOC OUT buffer size */
#define USBD_UAC_ISOC_IN_BUF_SIZE                   USBD_UAC_MAX_ISOC_DATA_LENGTH   /* ISOC IN buffer size */
#define USBD_UAC_CTRL_BUF_SIZE                      512U  /* CTRL buffer size */


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
#define USBD_UAC_LEN_CTRL_IF_HEADER                 0x09U
#define USBD_UAC_LEN_FUNC_UNIT_DESC                 0x22U

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

#define USBD_UAC_VOLUME_CTRL_MIN                    0x8001U
#define USBD_UAC_VOLUME_CTRL_MAX                    0x7FFFU

#define USBD_UAC_AUDIO_CTRL_CLASS_SIZE                  (72U+8)
#define WINDOWS_MIN_VOLUME                              (0xDB00)

/* Exported types ------------------------------------------------------------*/
typedef struct __PACKED {
	uint16_t wNumSubRanges;
	uint16_t bMIN;
	uint16_t bMAX;
	uint16_t bRES;
} usbd_uac_ctrl_range_layout2_struct;

typedef struct __PACKED {
	u16 wNumSubRanges;
	struct {
		u32 bMIN;
		u32 bMAX;
		u32 bRES;
	} SubRanges[1];
} usbd_uac_freq_ctrl_range_t;

typedef struct __PACKED {
	u8  bNrChannels;
	u32 bmChannelConfig;
	u8  iChannelNames;
} usbd_uac_ac_connect_ctrl_t;

typedef struct {
	int(* init)(void);
	int(* deinit)(void);
	int(* setup)(usb_setup_req_t *req, u8 *buf);
	int(* set_config)(void);
	int(* isoc_received)(u8 *buf, u32 len);
	void(* isoc_transmitted)(u8 status);
	void (*status_changed)(u8 status);
	void (*mute_changed)(u8 mute);
	void (*volume_changed)(u8 volume);
} usbd_composite_uac_usr_cb_t;

typedef struct {
	usbd_composite_uac_usr_cb_t *cb;
	usbd_composite_dev_t *dev;

	u8 *isoc_out_buf;
	u8  alt_setting;

	u16 cur_volume;
	u8 cur_clk_valid;
	u8 cur_mute;

	__IO u8 is_ready;
} usbd_composite_uac_device_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

extern usbd_class_driver_t usbd_composite_uac_driver;

/* Exported functions --------------------------------------------------------*/

int usbd_composite_uac_init(usbd_composite_dev_t *cdev, usbd_composite_uac_usr_cb_t *cb);
int usbd_composite_uac_deinit(void);

#endif // USBD_COMPOSITE_UAC_H