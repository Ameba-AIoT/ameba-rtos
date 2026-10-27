/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_UVC_DESC_H
#define USBH_UVC_DESC_H

/* Includes ------------------------------------------------------------------*/

#include "dlist.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Host_Constants USB Host Constants
 *  @{
 */
/** @defgroup Host_UVC_Desc_Constants Host UVC Descriptor Constants
 *  @brief Class-specific descriptor type and subtype codes of the UVC specification.
 *  @{
 */

#define USBH_UVC_CLASS_CODE                                  0xEU

/* A.5. Video Class-Specific VC Interface Descriptor Subtypes */
#define USBH_UVC_VC_DESCRIPTOR_UNDEFINED                     0x00U
#define USBH_UVC_VC_HEADER                                   0x01U
#define USBH_UVC_VC_INPUT_TERMINAL                           0x02U
#define USBH_UVC_VC_OUTPUT_TERMINAL                          0x03U
#define USBH_UVC_VC_SELECTOR_UNIT                            0x04U
#define USBH_UVC_VC_PROCESSING_UNIT                          0x05U
#define USBH_UVC_VC_EXTENSION_UNIT                           0x06U
#define USBH_UVC_VC_ENCODING_UNIT                            0x07U

/* A.6. Video Class-Specific VS Interface Descriptor Subtypes */
#define USBH_UVC_VS_UNDEFINED                                0x00U
#define USBH_UVC_VS_INPUT_HEADER                             0x01U
#define USBH_UVC_VS_OUTPUT_HEADER                            0x02U
#define USBH_UVC_VS_STILL_IMAGE_FRAME                        0x03U
#define USBH_UVC_VS_FORMAT_UNCOMPRESSED                      0x04U
#define USBH_UVC_VS_FRAME_UNCOMPRESSED                       0x05U
#define USBH_UVC_VS_FORMAT_MJPEG                             0x06U
#define USBH_UVC_VS_FRAME_MJPEG                              0x07U
#define USBH_UVC_VS_FORMAT_MPEG2TS                           0x0aU
#define USBH_UVC_VS_FORMAT_DV                                0x0cU
#define USBH_UVC_VS_COLORFORMAT                              0x0dU
#define USBH_UVC_VS_FORMAT_FRAME_BASED                       0x10U
#define USBH_UVC_VS_FRAME_FRAME_BASED                        0x11U
#define USBH_UVC_VS_FORMAT_STREAM_BASED                      0x12U

/* A.7. Video Class-Specific Endpoint Descriptor Subtypes */
#define USBH_UVC_EP_UNDEFINED                                0x00U
#define USBH_UVC_EP_GENERAL                                  0x01U
#define USBH_UVC_EP_ENDPOINT                                 0x02U
#define USBH_UVC_EP_INTERRUPT                                0x03U

/* A.8. Video Class-Specific Request Codes */
#define USBH_UVC_RC_UNDEFINED                                0x00U
#define USBH_UVC_SET_CUR                                     0x01U
#define USBH_UVC_GET_CUR                                     0x81U
#define USBH_UVC_GET_MIN                                     0x82U
#define USBH_UVC_GET_MAX                                     0x83U
#define USBH_UVC_GET_RES                                     0x84U
#define USBH_UVC_GET_LEN                                     0x85U
#define USBH_UVC_GET_INFO                                    0x86U
#define USBH_UVC_GET_DEF                                     0x87U

#define USBH_UVC_VS_PROBE_CONTROL                            (0x1U<<8U)
#define USBH_UVC_VS_COMMIT_CONTROL                           (0x2U<<8U)

#define USBH_UVC_SUBCLASS_VIDEOCONTROL                       0x01U
#define USBH_UVC_SUBCLASS_VIDEOSTREAMING                     0x02U

#define USBH_UVC_DESC_TYPE_CS_INTERFACE                      0x24U
#define USBH_UVC_DESC_TYPE_CS_ENDPOINT                       0x25U

/* bmHeaderInfo bits of the payload header, ref UVC 1.5 Table 2-5 */
#define USBH_UVC_PH_FID                                      0x01U  /* Frame ID */
#define USBH_UVC_PH_EOF                                      0x02U  /* End of Frame */
#define USBH_UVC_PH_PTS                                      0x04U  /* Presentation Time */
#define USBH_UVC_PH_SCR                                      0x08U  /* Source Clock Reference */
#define USBH_UVC_PH_RES                                      0x10U  /* Payload specific bit */
#define USBH_UVC_PH_STI                                      0x20U  /* Still Image */
#define USBH_UVC_PH_ERR                                      0x40U  /* Error */
#define USBH_UVC_PH_EOH                                      0x80U  /* End of header */

/** @} End of Host_UVC_Desc_Constants group */
/** @} End of USB_Host_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Host_Types USB Host Types
 *  @{
 */
/** @defgroup Host_UVC_Desc_Types Host UVC Descriptor Types
 *  @brief Packed class-specific descriptor layouts of the UVC specification.
 *  @{
 */

/**
  * @brief	UVC Probe and Commit Control (UVC spec Table 4-75)
  * @note	Covers UVC 1.0 (26 bytes) and UVC 1.1 (34 bytes, up to bMaxVersion).
  *		UVC 1.5 adds 14 bytes at offset 34 (bUsage..bmLayoutPerStream); not supported.
  */
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
	u8 bPreferredVersion;
	u8 bMinVersion;
	u8 bMaxVersion;
} __PACKED usbh_uvc_stream_control_t;

/* Overlays the received payload: only the wire fields belong here */
typedef struct {
	u8 bHeaderLength;
	u8 bmHeaderInfo;              /* USBH_UVC_PH_xxx bits */
} __PACKED usbh_uvc_vs_payload_header_t;

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
	/* Followed by baInterfaceNr[bInCollection], read through bLength when needed */
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
	u8 bControlSize;
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
	/* Followed by baSourceID[bNrInPins], bControlSize, bmControls[] and iExtension */
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
	/* Followed by bmaControls[bNumFormats][bControlSize] */
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
	/* Followed by bmaControls[bNumFormats][bControlSize] */
} __PACKED usbh_uvc_vs_output_header_desc_t;

/* A VC unit or terminal: the topology of the function, ref UVC 1.5 3.7.2 */
typedef struct {
	u8 *p;                        /* Its descriptor, in the config descriptor buffer */
	const u8 *source;             /* bSourceID or baSourceID[] inside that descriptor, NULL for an input terminal */
	struct list_head list;
	u8 type;                      /* bDescriptorSubtype, USBH_UVC_VC_xxx */
	u8 id;                        /* bTerminalID or bUnitID */
	u8 source_num;                /* Number of entries at source */
} usbh_uvc_entity_t;

/** @} End of Host_UVC_Desc_Types group */
/** @} End of USB_Host_Types group */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @} End of USB_Host_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBH_UVC_DESC_H */
