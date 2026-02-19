/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#ifndef __USBD_VIDEO_H
#define __USBD_VIDEO_H
/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/

/* A.2. Video Interface Subclass Codes */
#define USBD_UVC_SC_UNDEFINED					0x00
#define USBD_UVC_SC_VIDEOCONTROL				0x01
#define USBD_UVC_SC_VIDEOSTREAMING				0x02
#define USBD_UVC_SC_VIDEO_INTERFACE_COLLECTION	0x03

/* A.3. Video Interface Protocol Codes */
#define USBD_UVC_PC_PROTOCOL_UNDEFINED			0x00

/* A.5. Video Class-Specific VC Interface Descriptor Subtypes */
#define USBD_UVC_VC_DESCRIPTOR_UNDEFINED		0x00
#define USBD_UVC_VC_HEADER						0x01
#define USBD_UVC_VC_INPUT_TERMINAL				0x02
#define USBD_UVC_VC_OUTPUT_TERMINAL				0x03
#define USBD_UVC_VC_SELECTOR_UNIT				0x04
#define USBD_UVC_VC_PROCESSING_UNIT				0x05
#define USBD_UVC_VC_EXTENSION_UNIT				0x06

/* A.6. Video Class-Specific VS Interface Descriptor Subtypes */
#define USBD_UVC_VS_UNDEFINED					0x00
#define USBD_UVC_VS_INPUT_HEADER				0x01
#define USBD_UVC_VS_OUTPUT_HEADER				0x02
#define USBD_UVC_VS_STILL_IMAGE_FRAME			0x03
#define USBD_UVC_VS_FORMAT_UNCOMPRESSED			0x04
#define USBD_UVC_VS_FRAME_UNCOMPRESSED			0x05
#define USBD_UVC_VS_FORMAT_MJPEG				0x06
#define USBD_UVC_VS_FRAME_MJPEG					0x07
#define USBD_UVC_VS_FORMAT_MPEG2TS				0x0a
#define USBD_UVC_VS_FORMAT_DV					0x0c
#define USBD_UVC_VS_COLORFORMAT					0x0d
#define USBD_UVC_VS_FORMAT_FRAME_BASED			0x10
#define USBD_UVC_VS_FRAME_FRAME_BASED			0x11
#define USBD_UVC_VS_FORMAT_STREAM_BASED			0x12

/* A.7. Video Class-Specific Endpoint Descriptor Subtypes */
#define USBD_UVC_EP_UNDEFINED				0x00
#define USBD_UVC_EP_GENERAL					0x01
#define USBD_UVC_EP_ENDPOINT				0x02
#define USBD_UVC_EP_INTERRUPT				0x03

/* A.8. Video Class-Specific Request Codes */
#define USBD_UVC_RC_UNDEFINED				0x00
#define USBD_UVC_SET_CUR					0x01
#define USBD_UVC_GET_CUR					0x81
#define USBD_UVC_GET_MIN					0x82
#define USBD_UVC_GET_MAX					0x83
#define USBD_UVC_GET_RES					0x84
#define USBD_UVC_GET_LEN					0x85
#define USBD_UVC_GET_INFO					0x86
#define USBD_UVC_GET_DEF					0x87

/* A.9.1. VideoControl Interface Control Selectors */
#define USBD_UVC_VC_CONTROL_UNDEFINED				0x00
#define USBD_UVC_VC_VIDEO_POWER_MODE_CONTROL		0x01
#define USBD_UVC_VC_REQUEST_ERROR_CODE_CONTROL		0x02

/* A.9.2. Terminal Control Selectors */
#define USBD_UVC_TE_CONTROL_UNDEFINED			0x00

/* A.9.3. Selector Unit Control Selectors */
#define USBD_UVC_SU_CONTROL_UNDEFINED			0x00
#define USBD_UVC_SU_INPUT_SELECT_CONTROL		0x01

/* A.9.4. Camera Terminal Control Selectors */
#define USBD_UVC_CT_CONTROL_UNDEFINED				0x00
#define USBD_UVC_CT_SCANNING_MODE_CONTROL			0x01
#define USBD_UVC_CT_AE_MODE_CONTROL					0x02
#define USBD_UVC_CT_AE_PRIORITY_CONTROL				0x03
#define USBD_UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL	0x04
#define USBD_UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL	0x05
#define USBD_UVC_CT_FOCUS_ABSOLUTE_CONTROL			0x06
#define USBD_UVC_CT_FOCUS_RELATIVE_CONTROL			0x07
#define USBD_UVC_CT_FOCUS_AUTO_CONTROL				0x08
#define USBD_UVC_CT_IRIS_ABSOLUTE_CONTROL			0x09
#define USBD_UVC_CT_IRIS_RELATIVE_CONTROL			0x0a
#define USBD_UVC_CT_ZOOM_ABSOLUTE_CONTROL			0x0b
#define USBD_UVC_CT_ZOOM_RELATIVE_CONTROL			0x0c
#define USBD_UVC_CT_PANTILT_ABSOLUTE_CONTROL		0x0d
#define USBD_UVC_CT_PANTILT_RELATIVE_CONTROL		0x0e
#define USBD_UVC_CT_ROLL_ABSOLUTE_CONTROL			0x0f
#define USBD_UVC_CT_ROLL_RELATIVE_CONTROL			0x10
#define USBD_UVC_CT_PRIVACY_CONTROL					0x11

/* A.9.5. Processing Unit Control Selectors */
#define USBD_UVC_PU_CONTROL_UNDEFINED						0x00
#define USBD_UVC_PU_BACKLIGHT_COMPENSATION_CONTROL			0x01
#define USBD_UVC_PU_BRIGHTNESS_CONTROL						0x02
#define USBD_UVC_PU_CONTRAST_CONTROL						0x03
#define USBD_UVC_PU_GAIN_CONTROL							0x04
#define USBD_UVC_PU_POWER_LINE_FREQUENCY_CONTROL			0x05
#define USBD_UVC_PU_HUE_CONTROL								0x06
#define USBD_UVC_PU_SATURATION_CONTROL						0x07
#define USBD_UVC_PU_SHARPNESS_CONTROL						0x08
#define USBD_UVC_PU_GAMMA_CONTROL							0x09
#define USBD_UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL		0x0a
#define USBD_UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL	0x0b
#define USBD_UVC_PU_WHITE_BALANCE_COMPONENT_CONTROL			0x0c
#define USBD_UVC_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL	0x0d
#define USBD_UVC_PU_DIGITAL_MULTIPLIER_CONTROL				0x0e
#define USBD_UVC_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL		0x0f
#define USBD_UVC_PU_HUE_AUTO_CONTROL						0x10
#define USBD_UVC_PU_ANALOG_VIDEO_STANDARD_CONTROL			0x11
#define USBD_UVC_PU_ANALOG_LOCK_STATUS_CONTROL				0x12

/* A.9.7. VideoStreaming Interface Control Selectors */
#define USBD_UVC_VS_CONTROL_UNDEFINED				0x00
#define USBD_UVC_VS_PROBE_CONTROL					0x01
#define USBD_UVC_VS_COMMIT_CONTROL					0x02
#define USBD_UVC_VS_STILL_PROBE_CONTROL				0x03
#define USBD_UVC_VS_STILL_COMMIT_CONTROL			0x04
#define USBD_UVC_VS_STILL_IMAGE_TRIGGER_CONTROL		0x05
#define USBD_UVC_VS_STREAM_ERROR_CODE_CONTROL		0x06
#define USBD_UVC_VS_GENERATE_KEY_FRAME_CONTROL		0x07
#define USBD_UVC_VS_UPDATE_FRAME_SEGMENT_CONTROL	0x08
#define USBD_UVC_VS_SYNC_DELAY_CONTROL				0x09

/* B.1. USB Terminal Types */
#define USBD_UVC_TT_VENDOR_SPECIFIC					0x0100
#define USBD_UVC_TT_STREAMING						0x0101

/* B.2. Input Terminal Types */
#define USBD_UVC_ITT_VENDOR_SPECIFIC				0x0200
#define USBD_UVC_ITT_CAMERA							0x0201
#define USBD_UVC_ITT_MEDIA_TRANSPORT_INPUT			0x0202

/* B.3. Output Terminal Types */
#define USBD_UVC_OTT_VENDOR_SPECIFIC				0x0300
#define USBD_UVC_OTT_DISPLAY						0x0301
#define USBD_UVC_OTT_MEDIA_TRANSPORT_OUTPUT			0x0302

/* B.4. External Terminal Types */
#define USBD_UVC_EXTERNAL_VENDOR_SPECIFIC			0x0400
#define USBD_UVC_COMPOSITE_CONNECTOR				0x0401
#define USBD_UVC_SVIDEO_CONNECTOR					0x0402
#define USBD_UVC_COMPONENT_CONNECTOR				0x0403

/* 2.4.2.2. Status Packet Type */
#define USBD_UVC_STATUS_TYPE_CONTROL				1
#define USBD_UVC_STATUS_TYPE_STREAMING				2

/* 2.4.3.3. Payload Header Information */
#define USBD_UVC_STREAM_EOH					(1 << 7)
#define USBD_UVC_STREAM_ERR					(1 << 6)
#define USBD_UVC_STREAM_STI					(1 << 5)
#define USBD_UVC_STREAM_RES					(1 << 4)
#define USBD_UVC_STREAM_SCR					(1 << 3)
#define USBD_UVC_STREAM_PTS					(1 << 2)
#define USBD_UVC_STREAM_EOF					(1 << 1)
#define USBD_UVC_STREAM_FID					(1 << 0)

/* 4.1.2. Control Capabilities */
#define USBD_UVC_CONTROL_CAP_GET				(1 << 0)
#define USBD_UVC_CONTROL_CAP_SET				(1 << 1)
#define USBD_UVC_CONTROL_CAP_DISABLED			(1 << 2)
#define USBD_UVC_CONTROL_CAP_AUTOUPDATE			(1 << 3)
#define USBD_UVC_CONTROL_CAP_ASYNCHRONOUS		(1 << 4)

/* Video Resolution */
#define USBD_UVC_160p          40960
#define USBD_UVC_360p          460800
#define USBD_UVC_720p          1843200
#define USBD_UVC_1080p         3110400
#define USBD_UVC_1080p_422v    4147200

/**
 * @brief UVC Streaming Control structure.
 * @details
 * This structure defines the parameters used to control video streaming
 * behavior in USB Video Class (UVC) devices, including format selection,
 * frame interval, bandwidth requirements, and payload size.
 */
typedef struct {
	uint16_t bmHint;
	uint8_t  bFormatIndex;
	uint8_t  bFrameIndex;
	uint32_t dwFrameInterval;
	uint16_t wKeyFrameRate;
	uint16_t wPFrameRate;
	uint16_t wCompQuality;
	uint16_t wCompWindowSize;
	uint16_t wDelay;
	uint32_t dwMaxVideoFrameSize;
	uint32_t dwMaxPayloadTransferSize;
	uint32_t dwClockFrequency;
	uint8_t  bmFramingInfo;
	uint8_t  bPreferedVersion;
	uint8_t  bMinVersion;
	uint8_t  bMaxVersion;
} __PACKED usbd_uvc_streaming_control_t;

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#endif /* __USBD_VIDEO_H */

