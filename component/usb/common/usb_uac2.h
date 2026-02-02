/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_UAC2_H
#define USB_UAC2_H


#define USB_UAC2_CLASS_CODE                         0x01U

/* A.5 Audio Interface Subclass Codes */
#define USB_UAC2_SUBCLASS_AUDIOCONTROL              0x01U
#define USB_UAC2_SUBCLASS_AUDIOSTREAMING            0x02U

/* A.6 Audio Interface Protocol Codes */
#define USB_UAC2_VERSION_02_00                      0x20U

/* A.4 Audio Interface Class Code */
#define USB_UAC2_IF_CLASS_AUDIO                     0x01U

/**
 * Defines UAC 2.0 class-specific request codes.
 */
#define USB_UAC2_LEN_CTRL_IF_HEADER                 0x09U  /**<UAC Audio Control Interface Header Descriptor 2.0. */
#define USB_UAC2_LEN_CLK_SRC_DESC                   0x08U  /**<UAC Audio Control Clock Source Unit Descriptor 2.0. */
#define USB_UAC2_LEN_AC_IN_TTY_DESC                 0x11U  /**<UAC Audio Control Input Terminal Descriptor 2.0. */
#define USB_UAC2_LEN_FUNC_UNIT_DESC                 0x09U  /**<UAC Audio Control Feature Unit Descriptor 2.0. */
#define USB_UAC2_LEN_AC_OUT_TTY_DESC                0x0CU  /**<UAC Audio Control Output Terminal Descriptor 2.0. */
#define USB_UAC2_LEN_AS_IF_ALT_SET_DESC             0x10U  /**<UAC Audio Streaming Interface Descriptor 2.0. */
#define USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC       0x06U  /**<UAC Audio Streaming Format Type Descriptor 2.0. */
#define USB_UAC2_LEN_DATA_EP_DESC                   0x08U  /**<UAC Audio Data Endpoint Descriptor. */

#define USB_UAC2_DESC_TYPE_AUDIO_CS_INTERFACE       0x24U  /**<UAC Audio Interface Descriptor Type. */
#define USB_UAC2_CS_ENDPOINT_DESCRIPTOR             0x25U  /**<UAC Audio Data Endpoint Descriptor Type. */
#define USB_UAC2_IAD_DESCRIPTOR_TYPE                0x0BU  /**<UAC IAD Descriptor type. */

/**
 * Audio Class-Specific Audio Streaming Interface Descriptor Subtype Codes (UAC2.0 Spec Appendix A.10)
 */
#define USB_UAC2_AS_IF_DESC_SUBTYPE_AS_GENERAL      0x01  /**<UAC Audio Streaming Interface Descriptor SubType. */
#define USB_UAC2_AS_IF_DESC_SUBTYPE_FORMAT_TYPE     0x02  /**<UAC Audio Format Type Descriptor SubType. */
/* Audio Class-Specific Audio Streaming Endpoint Descriptor Subtype Codes (UAC2.0 Spec Appendix A.13) */
#define USB_UAC2_AS_EP_DESC_SUBTYPE_EP_GENERAL      0x01  /**<UAC Audio Data Endpoint Descriptor SubType. */

#define USB_UAC2_IF_IDX_AC_HEADSET                  0x00U  /**<UAC Audio Control Interface Index. */
#define USB_UAC2_IF_IDX_AS_HEADSET_HEADPHONES       0x01U  /**<UAC Audio Streaming Headphone Interface Index. */
#define USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE       0x02U  /**<UAC Audio Streaming Microphone Interface Index. */

// Audio Function Category Codes (UAC2.0 Spec Appendix A.7)
#define USB_UAC2_FUNC_CATEGORY_CODE_DESKTOP_UNDEFINED              0x00U  /**<UAC Audio Function Undefined type. */
#define USB_UAC2_FUNC_CATEGORY_CODE_DESKTOP_SPEAKER                0x01U  /**<UAC Audio Function Speaker. */
#define USB_UAC2_FUNC_CATEGORY_CODE_DESKTOP_HEADSET                0x04U  /**<UAC Audio Function Headset. */

// Audio Class-Specific Audio Control Interface Descriptor Subtype Codes (UAC2.0 Spec Appendix A.9)
#define USB_UAC2_AC_IF_DESC_SUBTYPE_UNDEFINED                      0x00U  /**<UAC Audio Function Headset. */
#define USB_UAC2_AC_IF_DESC_SUBTYPE_HEADER                         0x01U  /**<UAC Audio Function Headset. */
#define USB_UAC2_AC_IF_DESC_SUBTYPE_INPUT_TERMINAL                 0x02U  /**<UAC Audio Function Headset. */
#define USB_UAC2_AC_IF_DESC_SUBTYPE_OUTPUT_TERMINAL                0x03U  /**<UAC Audio Function Headset. */
#define USB_UAC2_AC_IF_DESC_SUBTYPE_FEATURE_UNIT                   0x06U  /**<UAC Audio Function Headset. */
#define USB_UAC2_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE                   0x0AU  /**<UAC Audio Function Headset. */

/* A.14 Audio Class-Specific Request Codes */
#define USB_UAC2_CLASS_REQ_CODE_CUR                                0x01U /**< UAC Audio Class-Specific Request CUR code. */
#define USB_UAC2_CLASS_REQ_CODE_RANGE                              0x02U /**< UAC Audio Class-Specific Request RANGE code. */
/* A.17.1 Clock Source Control Selectors */
#define USB_UAC2_CS_SAM_FREQ_CONTROL                               0x01U /**< UAC Audio Clock Source Sample Frequency. */
#define USB_UAC2_CS_CLK_VALID_CONTROL                              0x02U /**< UAC Audio Clock Source Clock Valid Control. */
/* A.17.4 Terminal Control Selectors */
#define USB_UAC2_TE_CONNECTOR_CONTROL                              0x02U /**< UAC Audio Terminal Control Selectors CONNECTOR. */

/* A.17.7 Feature Unit Control Selectors */
#define USB_UAC2_CTRL_FU_MUTE_CONTROL_SELECTOR                     0x01U /**< UAC Feature Unit Mute Control */
#define USB_UAC2_CTRL_FU_VOLUME_CONTROL_SELECTOR                   0x02U /**< UAC Feature Unit Volume Control */
#define USB_UAC2_CTRL_FU_BASS_CONTROL_SELECTOR                     0x03U
#define USB_UAC2_CTRL_FU_MID_CONTROL_SELECTOR                      0x04U
#define USB_UAC2_CTRL_FU_TREBLE_CONTROL_SELECTOR                   0x05U
#define USB_UAC2_CTRL_FU_GRAPHIC_EQUALIZER_CONTROL_SELECTOR        0x06U
#define USB_UAC2_CTRL_FU_AUTOMATIC_GAIN_CONTROL_SELECTOR           0x07U
#define USB_UAC2_CTRL_FU_DELAY_CONTROL_SELECTOR                    0x08U
#define USB_UAC2_CTRL_FU_BASS_BOOST_CONTROL_SELECTOR               0x09U
#define USB_UAC2_CTRL_FU_LOUDNESS_CONTROL_SELECTOR                 0x0AU
#define USB_UAC2_CTRL_FU_INPUT_GAIN_CONTROL_SELECTOR               0x0BU
#define USB_UAC2_CTRL_FU_INPUT_GAIN_PAD_CONTROL_SELECTOR           0x0CU
#define USB_UAC2_CTRL_FU_PHASE_INVERTER_CONTROL_SELECTOR           0x0DU
#define USB_UAC2_CTRL_FU_UNDERFLOW_CONTROL_SELECTOR                0x0EU
#define USB_UAC2_CTRL_FU_OVERFLOW_CONTROL_SELECTOR                 0x0FU
#define USB_UAC2_CTRL_FU_LATENCY_CONTROL_SELECTOR                  0x10U

/* Class specific types -------------------------------------------------------------*/
/* 5.2.5.4.2 Connector Control */
typedef struct {
	u8  bNrChannels;
	u32 bmChannelConfig;
	u8  iChannelNames;
} __PACKED usb_uac2_ac_connect_ctrl_t;

/* 5.2.3.2 Layout 2 Parameter Block */
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
} __PACKED usb_uac2_ctrl_range_layout2_struct;

/* 5.2.3.3 Layout 3 Parameter Block */
/**
 * @brief Structure to define the frequency.
 * @details This is a packed structure used for sending notifications like SERIAL_STATE
 *          over the INTERRUPT IN endpoint.
 */
typedef struct {
	u32 dMIN;
	u32 dMAX;
	u32 dRES;
} usb_uac2_freq_t;

/* 5.2.3.3 Layout 3 Parameter Block */
/**
 * @brief Structure to define the frequency.
 * @details This is a packed structure used for sending notifications like SERIAL_STATE
 *          over the INTERRUPT IN endpoint.
 */
typedef struct {
	u16 wNumSubRanges;
	usb_uac2_freq_t usb_uac2_sub_ranges[];
} __PACKED usb_uac2_sampling_freq_ctrl_range_t;

#endif // USB_UAC2_H
