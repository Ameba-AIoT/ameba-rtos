/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_UAC_H
#define USB_UAC_H

/* Audio Class Codes */
#define USB_UAC_CLASS_CODE                         0x01U
#define USB_UAC_IF_CLASS_AUDIO                     0x01U

/* Audio interface bInterfaceProtocol values */
#define USB_UAC_IP_VERSION_1                       0x00U
#define USB_UAC_IP_VERSION_2                       0x20U

/* A.2 / A.5 Audio Interface Subclass Codes */
#define USB_UAC_SUBCLASS_AUDIOCONTROL              0x01U
#define USB_UAC_SUBCLASS_AUDIOSTREAMING            0x02U

/* Class-Specific Descriptor Types */
#define USB_UAC_CS_INTERFACE                       0x24U
#define USB_UAC_CS_ENDPOINT                        0x25U
#define USB_UAC_IAD_DESCRIPTOR_TYPE                0x0BU

/* A.5 / A.9 Audio Class-Specific AC Interface Descriptor Subtypes */
#define USB_UAC_AC_HEADER                          0x01U
#define USB_UAC_AC_INPUT_TERMINAL                  0x02U
#define USB_UAC_AC_OUTPUT_TERMINAL                 0x03U
#define USB_UAC_AC_FEATURE_UNIT                    0x06U

/* A.6 / A.10 Audio Class-Specific AS Interface Descriptor Subtypes */
#define USB_UAC_AS_GENERAL                         0x01U
#define USB_UAC_AS_FORMAT_TYPE                     0x02U

/* A.9 / A.13 Audio Class-Specific AS Endpoint Descriptor Subtypes */
#define USB_UAC_AS_EP_GENERAL                      0x01U

/* A.14 Audio Class-Specific Request Codes (base values) */
#define USB_UAC_REQ_CUR                            0x01U
#define USB_UAC_REQ_RANGE                          0x02U

/* A.17.1 / A.10.5 Clock Source Control Selectors */
#define USB_UAC_CS_SAM_FREQ_CONTROL                0x01U

/* A.17.7 / A.10.2 Feature Unit Control Selectors */
#define USB_UAC_FU_MUTE                            0x01U
#define USB_UAC_FU_VOLUME                          0x02U
#define USB_UAC_FU_BASS                            0x03U
#define USB_UAC_FU_MID                             0x04U
#define USB_UAC_FU_TREBLE                          0x05U
#define USB_UAC_FU_GRAPHIC_EQUALIZER               0x06U
#define USB_UAC_FU_AUTOMATIC_GAIN                  0x07U
#define USB_UAC_FU_DELAY                           0x08U
#define USB_UAC_FU_BASS_BOOST                      0x09U
#define USB_UAC_FU_LOUDNESS                        0x0AU
#define USB_UAC_FU_INPUT_GAIN                      0x0BU
#define USB_UAC_FU_INPUT_GAIN_PAD                  0x0CU
#define USB_UAC_FU_PHASE_INVERTER                  0x0DU
#define USB_UAC_FU_UNDERFLOW                       0x0EU
#define USB_UAC_FU_OVERFLOW                        0x0FU
#define USB_UAC_FU_LATENCY                         0x10U

#endif /* USB_UAC_H */
