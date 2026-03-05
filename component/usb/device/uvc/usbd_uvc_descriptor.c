/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usbd_uvc.h"
#include "usbd_video.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
#define USBD_UVC_FRAME_WIDTH  1280
#define USBD_UVC_FRAME_HEIGHT 720

#define USBD_UVC_YUY2 1
#define USBD_UVC_NV12 1
#define USBD_UVC_MJPG 1
#define USBD_UVC_H264 1
#define USBD_UVC_H265 1

#define USBD_UVC_WBVAL(x)   ((x) & 0xFF), (((x) >> 8) & 0xFF)
/* UVC Device Descriptor Set */
u8 usbd_uvc_descriptors[] = {

	/* =========================
	 * Configuration Descriptor
	 * ========================= */
	0x09, 0x02,             // bLength, bDescriptorType = CONFIGURATION
	0xE4, 0x00,             // wTotalLength = 0x00E4 (228 bytes)
	0x02,                   // bNumInterfaces = 2
	0x01,                   // bConfigurationValue
	0x01,                   // iConfiguration
	0x80,                   // bmAttributes = Bus Powered
	0x32,                   // bMaxPower = 100 mA

	/* =========================
	 * Interface Association Descriptor (IAD)
	 * ========================= */
	0x08, 0x0B,             // bLength, bDescriptorType = IAD
	0x00,                   // bFirstInterface
	0x02,                   // bInterfaceCount (VC + VS)
	0x0E,                   // bFunctionClass = CC_VIDEO
	0x03,                   // bFunctionSubClass = SC_VIDEO_INTERFACE_COLLECTION
	0x00,                   // bFunctionProtocol
	0x00,                   // iFunction

	/* =========================
	 * Interface 0: Video Control (VC)
	 * ========================= */
	0x09, 0x04,             // bLength, bDescriptorType = INTERFACE
	0x00,                   // bInterfaceNumber
	0x00,                   // bAlternateSetting
	0x01,                   // bNumEndpoints
	0x0E,                   // bInterfaceClass = CC_VIDEO
	0x01,                   // bInterfaceSubClass = SC_VIDEOCONTROL
	0x00,                   // bInterfaceProtocol
	0x00,                   // iInterface

	/* =========================
	 * Class-specific VC Header
	 * ========================= */
	0x0D, 0x24, 0x01,       // bLength, CS_INTERFACE, VC_HEADER
	0x00, 0x01,             // bcdUVC = 1.00
	0x4F, 0x00,             // wTotalLength (VC descriptors)
	0x00, 0x6C,             // dwClockFrequency = 6.912 MHz
	0xDC, 0x02,
	0x01,                   // bInCollection
	0x01,                   // baInterfaceNr(1) = VS interface 1

	/* =========================
	 * Input Terminal Descriptor (Camera)
	 * ========================= */
	0x12, 0x24, 0x02,       // INPUT_TERMINAL
	0x01,                   // bTerminalID
	0x01, 0x02,             // wTerminalType = ITT_CAMERA
	0x00,                   // bAssocTerminal
	0x00,                   // iTerminal
	0x00, 0x00,             // wObjectiveFocalLengthMin
	0x00, 0x00,             // wObjectiveFocalLengthMax
	0x00, 0x00,             // wOcularFocalLength
	0x03,                   // bControlSize
	0x00, 0x00, 0x00,       // bmControls

	/* =========================
	 * Processing Unit Descriptor
	 * ========================= */
	0x0B, 0x24, 0x05,       // PROCESSING_UNIT
	0x02,                   // bUnitID
	0x01,                   // bSourceID (Input Terminal 1)
	0x00, 0x40,             // wMaxMultiplier
	0x02,                   // bControlSize
	0xFF, 0xFF,             // bmControls (many processing controls)
	0x00,                   // iProcessing

	/* =========================
	 * Extension Unit Descriptor (Vendor-specific)
	 * ========================= */
	0x1C, 0x24, 0x06,       // EXTENSION_UNIT
	0x03,                   // bUnitID
	0xCE, 0xB9, 0x55, 0x2B, // GUID
	0xB0, 0x14, 0x49, 0xA8,
	0xB2, 0x2E, 0xF6, 0xB0,
	0xA0, 0x12, 0x82, 0x74,
	0x18,                   // bNumControls
	0x01,                   // bNrInPins
	0x02,                   // baSourceID(1) = PU
	0x03,                   // bControlSize
	0x00, 0x00, 0x00,       // bmControls
	0x00,                   // iExtension

	/* =========================
	 * Output Terminal Descriptor (USB Streaming)
	 * ========================= */
	0x09, 0x24, 0x03,       // OUTPUT_TERMINAL
	0x04,                   // bTerminalID
	0x01, 0x01,             // wTerminalType = TT_STREAMING
	0x00,                   // bAssocTerminal
	0x03,                   // bSourceID (Extension Unit)
	0x00,                   // iTerminal

	/* =========================
	 * VC Interrupt Endpoint
	 * ========================= */
	0x07, 0x05,             // ENDPOINT
	0x81,                   // IN endpoint 1
	0x03,                   // Interrupt
	0x40, 0x00,             // wMaxPacketSize = 64
	0x08,                   // bInterval

	/* =========================
	 * Class-specific Interrupt Endpoint
	 * ========================= */
	0x05, 0x25, 0x03,
	0x40, 0x00,

	/* =========================
	 * Interface 1: Video Streaming (Alt 0)
	 * ========================= */
	0x09, 0x04,
	0x01,
	0x00,
	0x00,
	0x0E,
	0x02,
	0x00,
	0x00,

	/* =========================
	 * VS Header Descriptor
	 * ========================= */
	0x0E, 0x24, 0x01,
	0x01,
	0x56, 0x00,
	0x83,
	0x00,
	0x04, 0x00,
	0x00, 0x00,
	0x01, 0x00,

	/* =========================
	 * VS Format Descriptor (H.264)
	 * ========================= */
	0x1C, 0x24, 0x10,
	0x01,
	0x01,
	'H', '2', '6', '4',
	0x00, 0x00,
	0x10, 0x00,
	0x80, 0x00,
	0x00, 0xAA,
	0x00,
	0x38, 0x9B, 0x71, 0x0C,
	0x01,
	0x01,
	0x00, 0x00, 0x00, 0x01,

	/* =========================
	 * VS Frame Descriptor
	 * ========================= */
	0x26, 0x24, 0x11,
	0x01,
	0x00,
	//0x80, 0x02,             // Width = 640
	//0xE0, 0x01,             // Height = 480
	USBD_UVC_WBVAL(USBD_UVC_FRAME_WIDTH),     // wWidth
	USBD_UVC_WBVAL(USBD_UVC_FRAME_HEIGHT),    // wHeight
	0x00, 0x40, 0x19, 0x01, // Min bitrate
	0x00, 0xC0, 0x4B, 0x03, // Max bitrate
	0x15, 0x16, 0x05, 0x00, // Max frame buffer
	0x03,
	0x00, 0x00,
	0x00, 0x00,
	0x15, 0x16, 0x05, 0x00,
	0x2A, 0x2C, 0x0A, 0x00,
	0x40, 0x42, 0x0F, 0x00,

	/* =========================
	 * VS Color Matching Descriptor
	 * ========================= */
	0x06, 0x24, 0x0D,
	0x01,
	0x01,
	0x04,

	/* =========================
	 * Interface 1: Video Streaming (Alt 1)
	 * ========================= */
	0x09, 0x04,
	0x01,
	0x01,
	0x01,
	0x0E,
	0x02,
	0x00,
	0x00,

	/* =========================
	 * Isochronous Video Endpoint
	 * ========================= */
	0x07, 0x05,
	0x83,                   // IN endpoint 3
	0x05,                   // Isochronous
	0x00, 0x04,             // wMaxPacketSize
	0x01,
};

int usbd_uvc_descriptors_size = sizeof(usbd_uvc_descriptors);

usbd_uvc_streaming_control_t usbd_uvc_probe = {
	.bmHint = 0,
	.bFormatIndex = 1,
	.bFrameIndex = 1,
	.dwFrameInterval = 666666,
	.wKeyFrameRate = 0,
	.wPFrameRate = 0,
	.wCompQuality = 0,
	.wCompWindowSize = 0,
	.wDelay = 0,
	.dwMaxVideoFrameSize = USBD_UVC_1080p,
	.dwMaxPayloadTransferSize = 0,
	.dwClockFrequency = 48000000,
	.bmFramingInfo = 0,
	.bPreferedVersion = 0,
	.bMinVersion    = 0,
	.bMaxVersion    = 0,
};

usbd_uvc_streaming_control_t usbd_uvc_commit = {
	.bmHint = 0,
	.bFormatIndex = 1,
	.bFrameIndex = 1,
	.dwFrameInterval = 666666,
	.wKeyFrameRate = 0,
	.wPFrameRate = 0,
	.wCompQuality = 0,
	.wCompWindowSize = 0,
	.wDelay = 0,
	.dwMaxVideoFrameSize = USBD_UVC_1080p,
	.dwMaxPayloadTransferSize = 0,
	.dwClockFrequency = 48000000,
	.bmFramingInfo = 0,
	.bPreferedVersion = 0,
	.bMinVersion    = 0,
	.bMaxVersion    = 0,
};


usbd_uvc_frame_info_t uvc_frames_h264[] = {
	{ USBD_UVC_FRAME_WIDTH, USBD_UVC_FRAME_HEIGHT, { USBD_UVC_FPS(30), USBD_UVC_FPS(15), USBD_UVC_FPS(10), 0 },},
	{ 0, 0, { 0, }, },
};
usbd_uvc_format_info_t uvcd_formats[] = {
	{ USBD_UVC_FORMAT_TYPE_H264, uvc_frames_h264 },
};