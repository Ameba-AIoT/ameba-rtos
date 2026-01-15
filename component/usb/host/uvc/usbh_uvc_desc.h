/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_UVC_DESC_H
#define USBH_UVC_DESC_H

/* Includes ------------------------------------------------------------------*/

#include "dlist.h"

/* Exported defines ----------------------------------------------------------*/

#define USBH_UVC_CLASS_CODE                                  0xE

/* A.5. Video Class-Specific VC Interface Descriptor Subtypes */
#define USBH_UVC_VC_DESCRIPTOR_UNDEFINED                     0x00
#define USBH_UVC_VC_HEADER                                   0x01
#define USBH_UVC_VC_INPUT_TERMINAL                           0x02
#define USBH_UVC_VC_OUTPUT_TERMINAL                          0x03
#define USBH_UVC_VC_SELECTOR_UNIT                            0x04
#define USBH_UVC_VC_PROCESSING_UNIT                          0x05
#define USBH_UVC_VC_EXTENSION_UNIT                           0x06
#define USBH_UVC_VC_ENCODING_UNIT                            0X07

/* A.6. Video Class-Specific VS Interface Descriptor Subtypes */
#define USBH_UVC_VS_UNDEFINED                                0x00
#define USBH_UVC_VS_INPUT_HEADER                             0x01
#define USBH_UVC_VS_OUTPUT_HEADER                            0x02
#define USBH_UVC_VS_STILL_IMAGE_FRAME                        0x03
#define USBH_UVC_VS_FORMAT_UNCOMPRESSED                      0x04
#define USBH_UVC_VS_FRAME_UNCOMPRESSED                       0x05
#define USBH_UVC_VS_FORMAT_MJPEG                             0x06
#define USBH_UVC_VS_FRAME_MJPEG                              0x07
#define USBH_UVC_VS_FORMAT_MPEG2TS                           0x0a
#define USBH_UVC_VS_FORMAT_DV                                0x0c
#define USBH_UVC_VS_COLORFORMAT                              0x0d
#define USBH_UVC_VS_FORMAT_FRAME_BASED                       0x10
#define USBH_UVC_VS_FRAME_FRAME_BASED                        0x11
#define USBH_UVC_VS_FORMAT_STREAM_BASED                      0x12

/* A.7. Video Class-Specific Endpoint Descriptor Subtypes */
#define USBH_UVC_EP_UNDEFINED                                0x00
#define USBH_UVC_EP_GENERAL                                  0x01
#define USBH_UVC_EP_ENDPOINT                                 0x02
#define USBH_UVC_EP_INTERRUPT                                0x03

/* A.8. Video Class-Specific Request Codes */
#define USBH_UVC_RC_UNDEFINED                                0x00
#define USBH_UVC_SET_CUR                                     0x01
#define USBH_UVC_GET_CUR                                     0x81
#define USBH_UVC_GET_MIN                                     0x82
#define USBH_UVC_GET_MAX                                     0x83
#define USBH_UVC_GET_RES                                     0x84
#define USBH_UVC_GET_LEN                                     0x85
#define USBH_UVC_GET_INFO                                    0x86
#define USBH_UVC_GET_DEF                                     0x87

#define USBH_UVC_VS_PROBE_CONTROL                           (0x1<<8)
#define USBH_UVC_VS_COMMIT_CONTROL                          (0x2<<8)

#define USBH_UVC_SUBCLASS_VIDEOCONTROL                       0x01
#define USBH_UVC_SUBCLASS_VIDEOSTREAMING                     0x02

#define USBH_UVC_DESC_TYPE_CS_INTERFACE                      0x24
#define USBH_UVC_DESC_TYPE_CS_ENDPOINT                       0x25


/* Exported types ------------------------------------------------------------*/


typedef union {
	u8 bitmap;
	struct {
		u8 fid: 1; //FrameID
		u8 eof: 1; //End of Frame
		u8 pts: 1; //Presentation Time
		u8 scr: 1; //Source Clock Reference
		u8 res: 1; //Payload specific bit
		u8 sti: 1; //Still Image
		u8 err: 1; //Error
		u8 eoh: 1; //End of header
	} b;
} usbh_uvc_header_bitmap_t;


typedef  struct {
	u16 bmHint;
	u8 bFormatIndex;
	u8 bFrameIndex;
	u32 dwFrameInterval;
	u16 wKeyFrameRate;
	u16 wPFrameRate;
	u16 wCompQuality;
	u16 wCompWindowSize;
	u16 wDelay;
	u32 dwMaxVideoFrameSize;
	u32 dwMaxPayloadTransferSize;
	u32 dwClockFrequency;
	u8 bmFramingInfo;
	u8 bPreferedVersion;
	u8 bMinVersion;
	u8 bMaxVersion;
} __PACKED usbh_uvc_stream_control_t;

typedef struct {
	u8 bHeaderLength;
	usbh_uvc_header_bitmap_t bmHeaderInfo;
	u8 *ptr;
} __PACKED usbh_uvc_vs_payload_header_t;


typedef enum {
	STREAM_STATE_IDLE = 1,
	STREAM_DATA_IN,
} usbh_uvc_stream_state_t;


typedef struct {
	u8 bLength;
	u8 bDescriptorType;
	u8 bDescriptorSubType;
	u16 wMaxTransferSize;
} __PACKED usbh_uvc_vc_intr_ep_desc_t;

typedef struct {
	u8 bLength;
	u8 bDescriptorType;
	u8 bDescriptorSubtype;
	u16 bcdUVC;
	u16 wTotalLength;
	u32 dwClockFrequency;
	u8 bInCollection;
	u8 baInterfaceNr[0];
} __PACKED usbh_uvc_vc_header_desc_t;

typedef struct {
	u8 bLength;
	u8 bDescriptorType;
	u8 bDescriptorSubtype;
	u8 bTerminalID;
	u8 wTerminalType[2];
	u8 bAssocTerminal;
	u8 iTerminal;
	u8 wObjectiveFocalLengthMin[2];
	u8 wObjectiveFocalLengthMax[2];
	u8 wOcularFocalLength[2];
	u8 bControlSize;
	u8 bmControls[3];
} __PACKED usbh_uvc_vc_it_desc_t;

typedef struct {
	u8 bLength;
	u8 bDescriptorType;
	u8 bDescriptorSubtype;
	u8 bTerminalID;
	u8 wTerminalType[2];
	u8 bAssocTerminal;
	u8 bSourceID;
	u8 iTerminal;
} __PACKED usbh_uvc_vc_ot_desc_t;

typedef struct {
	u8 bLength;
	u8 bDescriptorType;
	u8 bDescriptorSubtype;
	u8 bTerminalID;
	u8 wTerminalType[2];
	u8 bAssocTerminal;
	u8 iTerminal;
	u8 wObjectiveFocalLengthMin[2];
	u8 wObjectiveFocalLengthMax[2];
	u8 wOcularFocalLength[2];
	u8 bControlSize[1];
	u8 bmControls[3];
} __PACKED usbh_uvc_vc_ct_desc_t;

typedef struct {
	u8 bLength;
	u8 bDescriptorType;
	u8 bDescriptorSubtype;
	u8 bUnitID;
	u8 bSourceID;
	u8 wMaxMultiplier[2];
	u8 bControlSize;
	u8 bmControls[3];
	u8 iProcessing;
	u8 bmVideoStandards;
} __PACKED usbh_uvc_vc_pu_desc_t;

typedef struct {
	u8 bLength;
	u8 bDescriptorType;
	u8 bDescriptorSubtype;
	u8 bUnitID;
	u8 bNrInPins;
	u8 bSourceID;
	u8 iSelector;
} __PACKED usbh_uvc_vc_su_desc_t;

typedef struct {
	u8 bLength;
	u8 bDescriptorType;
	u8 bDescriptorSubtype;
	u8 bUnitID;
	u8 guidExtensionCode[16];
	u8 bNumControls;
	u8 bNrInPins;
	u8 *baSourceID;
} __PACKED usbh_uvc_vc_xu_desc_t;

typedef struct {
	u8 bLength;
	u8 bDescriptorType;
	u8 bDescriptorSubtype;
	u8 bUnitID;
	u8 bNrInPins;
	u8 bSourceID0;
	u8 iSelector;
} __PACKED usbh_uvc_vc_eu_desc_t;

typedef struct {
	u8 bLength;
	u8 bDescriptorType;
	u8 bDescriptorSubtype;
	u8 bNumFormats;
	u16 wTotalLength;
	u8 bEndPointAddress;
	u8 bmInfo;
	u8 bTerminalLink;
	u8 bStillCaptureMethod;
	u8 bTriggerSupport;
	u8 bTriggerUsage;
	u8 bControlSize;
	u8 bmaControls;
} __PACKED usbh_uvc_vs_input_header_desc_t;

typedef struct {
	u8 bLength;
	u8 bDescriptorType;
	u8 bDescriptorSubtype;
	u8 bNumFormats;
	u8 wTotalLength[2];
	u8 bEndPointAddress;
	u8 bTerminalLink;
	u8 bControlSize;
	u8 bmaControls;
} __PACKED usbh_uvc_vs_output_header_desc_t;

typedef struct {
	struct list_head list;
	u8 *p;
	u8 type;
	u8 id;
	u8 source[0];
}  usbh_uvc_entity_t;

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#endif
