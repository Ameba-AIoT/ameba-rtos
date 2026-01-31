/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_UAC1_H
#define USB_UAC1_H

#define USB_UAC1_VERSION_01_10                      0x110U

/* Audio Class Codes */
#define USB_UAC1_CLASS_CODE                         0x01U

/* A.2 Audio Interface Subclass Codes */
#define USB_UAC1_SUBCLASS_AUDIOCONTROL              0x01U
#define USB_UAC1_SUBCLASS_AUDIOSTREAMING            0x02U

#define USB_UAC1_IF_CLASS_AUDIO                     0x01U

#define USB_UAC1_INTERFACE_DESC_TYPE                0x24
#define USB_UAC1_AUDIO_EP_DESC_TYPE                 0x25
#define USB_UAC1_IA_DESCRIPTOR                      0x0BU      /**< Descriptor Type: Interface Association Descriptor. */

/* usbd uac */
#define USB_UAC1_AUDIO_INTERFACE_DESC_TYPE          0x24U
#define USB_UAC1_AUDIO_EP_DESCRIPTOR                0x25U

// Audio Class-Specific Audio Streaming Interface Descriptor Subtype Codes (UAC2.0 Spec Appendix A.10)
#define USB_UAC1_AS_IF_DESC_SUBTYPE_AS_GENERAL              0x01
#define USB_UAC1_AS_IF_DESC_SUBTYPE_FORMAT_TYPE             0x02
// Audio Class-Specific Audio Streaming Endpoint Descriptor Subtype Codes (UAC2.0 Spec Appendix A.13)
#define USB_UAC1_AS_EP_DESC_SUBTYPE_EP_GENERAL              0x01

#define USB_UAC1_IF_IDX_AC_HEADSET                          0x00U

/* A.10.5 Endpoint Control Selectors */
#define USB_UAC1_CS_SAM_FREQ_CONTROL_TYPE                   0x01U

/*  4.3.2.5 Feature Unit Descriptor  bmaControls */
#define USB_UAC1_CONTROL_MUTE                               BIT0
#define USB_UAC1_CONTROL_VOLUME                             BIT1
#define USB_UAC1_CONTROL_BASS                               BIT2
#define USB_UAC1_CONTROL_MID                                BIT3
#define USB_UAC1_CONTROL_TREBLE                             BIT4

/* A.10.2 Feature Unit Control Selectors */
#define USB_UAC1_CTRL_FU_MUTE_CONTROL_SELECTOR                     0x01U
#define USB_UAC1_CTRL_FU_VOLUME_CONTROL_SELECTOR                   0x02U
#define USB_UAC1_CTRL_FU_BASS_CONTROL_SELECTOR                     0x03U
#define USB_UAC1_CTRL_FU_MID_CONTROL_SELECTOR                      0x04U
#define USB_UAC1_CTRL_FU_TREBLE_CONTROL_SELECTOR                   0x05U
#define USB_UAC1_CTRL_FU_GRAPHIC_EQUALIZER_CONTROL_SELECTOR        0x06U
#define USB_UAC1_CTRL_FU_AUTOMATIC_GAIN_CONTROL_SELECTOR           0x07U
#define USB_UAC1_CTRL_FU_DELAY_CONTROL_SELECTOR                    0x08U
#define USB_UAC1_CTRL_FU_BASS_BOOST_CONTROL_SELECTOR               0x09U
#define USB_UAC1_CTRL_FU_LOUDNESS_CONTROL_SELECTOR                 0x0AU
#define USB_UAC1_CTRL_FU_INPUT_GAIN_CONTROL_SELECTOR               0x0BU
#define USB_UAC1_CTRL_FU_INPUT_GAIN_PAD_CONTROL_SELECTOR           0x0CU
#define USB_UAC1_CTRL_FU_PHASE_INVERTER_CONTROL_SELECTOR           0x0DU
#define USB_UAC1_CTRL_FU_UNDERFLOW_CONTROL_SELECTOR                0x0EU
#define USB_UAC1_CTRL_FU_OVERFLOW_CONTROL_SELECTOR                 0x0FU
#define USB_UAC1_CTRL_FU_LATENCY_CONTROL_SELECTOR                  0x10U

/* A.5 Audio Class-Specific AC Interface Descriptor Subtypes */
#define USB_UAC1_HEADER                      0x01
#define USB_UAC1_INPUT_TERMINAL              0x02
#define USB_UAC1_OUTPUT_TERMINAL             0x03
#define USB_UAC1_MIXER_UNIT                  0x04
#define USB_UAC1_SELECTOR_UNIT               0x05
#define USB_UAC1_FEATURE_UNIT                0x06
#define USB_UAC1_PROCESSING_UNIT             0x07
#define USB_UAC1_EXTENSION_UNIT              0x08

/* A.6 Audio Class-Specific AS Interface Descriptor Subtypes */
#define USB_UAC1_AS_GENERAL                  0x01
#define USB_UAC1_FORMAT_TYPE                 0x02
#define USB_UAC1_FORMAT_SPECIFIC             0x03

/* A.7 Processing Unit Process Types */
#define USB_UAC1_PROCESS_UNDEFINED           0x00
#define USB_UAC1_PROCESS_UP_DOWNMIX          0x01
#define USB_UAC1_PROCESS_DOLBY_PROLOGIC      0x02
#define USB_UAC1_PROCESS_STEREO_EXTENDER     0x03
#define USB_UAC1_PROCESS_REVERB              0x04
#define USB_UAC1_PROCESS_CHORUS              0x05
#define USB_UAC1_PROCESS_DYN_RANGE_COMP      0x06

/* A.9 Audio Class-Specific Request Codes */
#define USB_UAC1_SET_CUR                     0x01
#define USB_UAC1_GET_CUR                     0x81
#define USB_UAC1_SET_MIN                     0x02
#define USB_UAC1_GET_MIN                     0x82
#define USB_UAC1_SET_MAX                     0x03
#define USB_UAC1_GET_MAX                     0x83
#define USB_UAC1_SET_RES                     0x04
#define USB_UAC1_GET_RES                     0x84
#define USB_UAC1_SET_MEM                     0x05
#define USB_UAC1_GET_MEM                     0x85
#define USB_UAC1_GET_STAT                    0xFF

/* A.10.2 Feature Unit Control Selectors */
#define USB_UAC1_FU_MUTE                     0x01
#define USB_UAC1_FU_VOLUME                   0x02
#define USB_UAC1_FU_BASS                     0x03
#define USB_UAC1_FU_MID                      0x04
#define USB_UAC1_FU_TREBLE                   0x05
#define USB_UAC1_FU_GRAPHIC_EQUALIZER        0x06
#define USB_UAC1_FU_AUTOMATIC_GAIN           0x07
#define USB_UAC1_FU_DELAY                    0x08
#define USB_UAC1_FU_BASS_BOOST               0x09
#define USB_UAC1_FU_LOUDNESS                 0x0a

/* AC Descriptor Length */
#define USB_UAC1_AC_IF_HEADER_LEN                   0x09U
#define USB_UAC1_AC_IN_TTY_DESC_LEN                 0x0CU
#define USB_UAC1_AC_OUT_TTY_DESC_LEN                0x09U
#define USB_UAC1_AC_FUNC_UNIT_DESC_LEN              0x08U

/* AS Descriptor Length */
#define USB_UAC1_AS_IF_DESC_LEN                     0X07U
#define USB_UAC1_AS_FRT_TYPE_DESC_LEN               0x08U
#define USB_UAC1_EP_DESC_LEN                        0X09U
#define USB_UAC1_AUDIO_DATA_EP_DESC_LEN             0X07U

/* Terminals - 2.2 Input Terminal Types */
#define USB_UAC1_INPUT_TERMINAL_UNDEFINED                    0x200
#define USB_UAC1_INPUT_TERMINAL_MICROPHONE                   0x201
#define USB_UAC1_INPUT_TERMINAL_DESKTOP_MICROPHONE           0x202
#define USB_UAC1_INPUT_TERMINAL_PERSONAL_MICROPHONE          0x203
#define USB_UAC1_INPUT_TERMINAL_OMNI_DIR_MICROPHONE          0x204
#define USB_UAC1_INPUT_TERMINAL_MICROPHONE_ARRAY             0x205
#define USB_UAC1_INPUT_TERMINAL_PROC_MICROPHONE_ARRAY        0x206

/* Terminals - 2.3 Output Terminal Types */
#define USB_UAC1_OUTPUT_TERMINAL_UNDEFINED                   0x300
#define USB_UAC1_OUTPUT_TERMINAL_SPEAKER                     0x301
#define USB_UAC1_OUTPUT_TERMINAL_HEADPHONES                  0x302
#define USB_UAC1_OUTPUT_TERMINAL_HEAD_MOUNTED_DISPLAY_AUDIO  0x303
#define USB_UAC1_OUTPUT_TERMINAL_DESKTOP_SPEAKER             0x304
#define USB_UAC1_OUTPUT_TERMINAL_ROOM_SPEAKER                0x305
#define USB_UAC1_OUTPUT_TERMINAL_COMMUNICATION_SPEAKER       0x306
#define USB_UAC1_OUTPUT_TERMINAL_LOW_FREQ_EFFECTS_SPEAKER    0x307

/* Formats - A.1.1 Audio Data Format Type I Codes */
#define USB_UAC1_FORMAT_TYPE_I_UNDEFINED     0x0
#define USB_UAC1_FORMAT_TYPE_I_PCM           0x1
#define USB_UAC1_FORMAT_TYPE_I_PCM8          0x2
#define USB_UAC1_FORMAT_TYPE_I_IEEE_FLOAT    0x3
#define USB_UAC1_FORMAT_TYPE_I_ALAW          0x4
#define USB_UAC1_FORMAT_TYPE_I_MULAW         0x5

/* Formats - A.2 Format Type Codes */
#define USB_UAC1_FORMAT_TYPE_UNDEFINED       0x0
#define USB_UAC1_FORMAT_TYPE_I               0x1
#define USB_UAC1_FORMAT_TYPE_II              0x2
#define USB_UAC1_FORMAT_TYPE_III             0x3
#define USB_UAC1_EXT_FORMAT_TYPE_I           0x81
#define USB_UAC1_EXT_FORMAT_TYPE_II          0x82
#define USB_UAC1_EXT_FORMAT_TYPE_III         0x83

/* Terminal Control Selectors */
/* Class-Specific AC Interface Descriptor header */
typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
} __PACKED usb_ac_itf_desc_header_t;

/* 2.2.5 Type I Format Type Descriptor */
typedef struct {
	uint8_t bLength;                  /* in bytes: 8 + (ns * 3) */
	uint8_t bDescriptorType;          /* USB_DT_CS_INTERFACE */
	uint8_t bDescriptorSubtype;       /* FORMAT_TYPE */
	uint8_t bFormatType;              /* FORMAT_TYPE_1 */
	uint8_t bNrChannels;              /* physical channels in the stream */
	uint8_t bSubframeSize;            /* */
	uint8_t bBitResolution;
	uint8_t bSamFreqType;
	uint8_t tSamFreq[3][3];
} __PACKED usb_uac1_format_type_i_discrete_descriptor;

#endif // USB_UAC1_H
