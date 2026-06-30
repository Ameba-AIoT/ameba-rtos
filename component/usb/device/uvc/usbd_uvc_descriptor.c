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
/* #define UVC_ORIGINAL_TYPE */
#ifdef UVC_SINGLE_STREAM

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
	0x00, 0x1e, 0x00,       // bmControls
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
	.dwMaxVideoFrameSize = USBD_TUNING_W * USBD_TUNING_H,
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
	.dwMaxVideoFrameSize = USBD_TUNING_W * USBD_TUNING_H,
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
#else

u8 usbd_uvc_descriptors[] = {
	// ============================================
	// Configuration Descriptor (9 bytes)
	// ============================================
	0x09, 0x02, 0xDC, 0x01, 0x02, 0x01, 0x01, 0x80,
	0x32,

	// ============================================
	// Interface Association Descriptor (8 bytes)
	// ============================================
	0x08, 0x0B, 0x00, 0x02, 0x0E, 0x03, 0x00, 0x00,

	// ============================================
	// Video Control Interface (9 bytes)
	// ============================================
	0x09, 0x04, 0x00, 0x00, 0x01, 0x0E, 0x01, 0x00,
	0x00,

	// ============================================
	// VC Interface Header (13 bytes)
	// ============================================
	0x0D, 0x24, 0x01, 0x00, 0x01, 0x4F, 0x00, 0x00,
	0x6C, 0xDC, 0x02, 0x01, 0x01,

	// ============================================
	// Camera Terminal (18 bytes)
	// ============================================
	0x12, 0x24, 0x02, 0x01, 0x01, 0x02, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF,
	0xFF, 0xFF,

	// ============================================
	// Processing Unit (11 bytes)
	// ============================================
	0x0B, 0x24, 0x05, 0x02, 0x01, 0x00, 0x40, 0x02,
	0xFF, 0xFF, 0x00,

	// ============================================
	// Extension Unit (28 bytes)
	// ============================================
	0x1C, 0x24, 0x06, 0x03, 0xCE, 0xB9, 0x55, 0x2B,
	0xB0, 0x14, 0x49, 0xA8, 0xB2, 0x2E, 0xF6, 0xB0,
	0xA0, 0x12, 0x82, 0x74, 0x18, 0x01, 0x02, 0x03,
	0x00, 0x1e, 0x00, 0x00,

	// ============================================
	// Output Terminal (9 bytes)
	// ============================================
	0x09, 0x24, 0x03, 0x04, 0x01, 0x01, 0x00, 0x03,
	0x00,

	// ============================================
	// VC Interrupt Endpoint (7 bytes)
	// ============================================
	0x07, 0x05, 0x81, 0x03, 0x40, 0x00, 0x08,

	// ============================================
	// CS VC Interrupt Endpoint (5 bytes)
	// ============================================
	0x05, 0x25, 0x03, 0x40, 0x00,

	// ============================================
	// Video Streaming Interface Alt 0 (9 bytes)
	// ============================================
	0x09, 0x04, 0x01, 0x00, 0x00, 0x0E, 0x02, 0x00,
	0x00,

	// ============================================
	// VS Input Header (17 bytes)
	// ============================================
	0x11, 0x24, 0x01, 0x04, 0x4E, 0x01, 0x83, 0x00,  //bLength=0x11(17), bNumFormats=0x04
	0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,  //wTotalLength=0x014E(334)
	0x00,

	// ============================================
	// Format 1: NV16 Uncompressed (27 bytes)
	// ============================================
	0x1B, 0x24, 0x04, 0x01, 0x01, 0x4E, 0x56, 0x31,
	0x36, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00,
	0xAA, 0x00, 0x38, 0x9B, 0x71, 0x10, 0x01, 0x00,
	0x00, 0x00, 0x00,

	// Frame Descriptor - NV16 (38 bytes)
	0x26, 0x24, 0x05, 0x01, 0x00,
	USBD_UVC_WBVAL(USBD_TUNING_W),
	USBD_UVC_WBVAL(USBD_TUNING_H),
	0x00, 0x80, 0xF4, 0x03, 0x00, 0xC0, 0xA9,
	0x1D, 0x00, 0x48, 0x3F, 0x00, 0x9A, 0x5B, 0x06,
	0x00, 0x03, 0x9A, 0x5B, 0x06, 0x00, 0x2A, 0x2C,
	0x0A, 0x00, 0x40, 0x42, 0x0F, 0x00,

	// ============================================
	// Format 2: NV12 Uncompressed (27 bytes)
	// ============================================
	0x1B, 0x24, 0x04, 0x02, 0x01, 0x4E, 0x56, 0x31,
	0x32, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00,
	0xAA, 0x00, 0x38, 0x9B, 0x71, 0x0C, 0x01, 0x00,
	0x00, 0x00, 0x00,

	// Frame Descriptor - NV12 (38 bytes)
	0x26, 0x24, 0x05, 0x01, 0x00,
	USBD_UVC_WBVAL(USBD_UVC_FRAME_WIDTH),
	USBD_UVC_WBVAL(USBD_UVC_FRAME_HEIGHT),
	0x00, 0xEC, 0x5E, 0x00, 0x00, 0xEA, 0xC7,
	0x02, 0x00, 0x76, 0x2F, 0x00, 0x9A, 0x5B, 0x06,
	0x00, 0x03, 0x9A, 0x5B, 0x06, 0x00, 0x2A, 0x2C,
	0x0A, 0x00, 0x40, 0x42, 0x0F, 0x00,

	// ============================================
	// Format 3: MJPEG (11 bytes)
	// ============================================
	0x0B, 0x24, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00,

	// Frame Descriptor - MJPEG (38 bytes)
	0x26, 0x24, 0x07, 0x01, 0x00,
	USBD_UVC_WBVAL(USBD_UVC_JPEG_FRAME_WIDTH),
	USBD_UVC_WBVAL(USBD_UVC_JPEG_FRAME_HEIGHT),
	0x00, 0xC4, 0x1C, 0x01, 0x00, 0x4C, 0x56,
	0x03, 0x00, 0x08, 0x07, 0x00, 0x9A, 0x5B, 0x06,
	0x00, 0x03, 0x9A, 0x5B, 0x06, 0x00, 0x2A, 0x2C,
	0x0A, 0x00, 0x40, 0x42, 0x0F, 0x00,

	// ============================================
	// Format 4: H.264 (28 bytes)
	// ============================================
	0x1C, 0x24, 0x10, 0x04, 0x01, 0x48, 0x32, 0x36,
	0x34, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00,
	0xAA, 0x00, 0x38, 0x9B, 0x71, 0x0C, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x01,

	// Frame Descriptor - H.264 (38 bytes)
	0x26, 0x24, 0x11, 0x01, 0x00,
	USBD_UVC_WBVAL(USBD_UVC_FRAME_WIDTH),
	USBD_UVC_WBVAL(USBD_UVC_FRAME_HEIGHT),
	0x00, 0xC4, 0x1C, 0x01, 0x00, 0x4C, 0x56,
	0x03, 0x9A, 0x5B, 0x06, 0x00, 0x03, 0x00, 0x00,
	0x00, 0x00, 0x9A, 0x5B, 0x06, 0x00, 0x2A, 0x2C,
	0x0A, 0x00, 0x40, 0x42, 0x0F, 0x00,

	// ============================================
	// Format 5: HEVC (28 bytes)
	// ============================================
	0x1C, 0x24, 0x10, 0x05, 0x01, 0x48, 0x45, 0x56,
	0x43, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00,
	0xAA, 0x00, 0x38, 0x9B, 0x71, 0x0C, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x01,

	// Frame Descriptor - HEVC (38 bytes)
	0x26, 0x24, 0x11, 0x01, 0x00,
	USBD_UVC_WBVAL(USBD_UVC_FRAME_WIDTH),
	USBD_UVC_WBVAL(USBD_UVC_FRAME_HEIGHT),
	0x00, 0xC4, 0x1C, 0x01, 0x00, 0x4C, 0x56,
	0x03, 0x9A, 0x5B, 0x06, 0x00, 0x03, 0x00, 0x00,
	0x00, 0x00, 0x9A, 0x5B, 0x06, 0x00, 0x2A, 0x2C,
	0x0A, 0x00, 0x40, 0x42, 0x0F, 0x00,

	// ============================================
	// Color Matching Descriptor (6 bytes)
	// ============================================
	0x06, 0x24, 0x0D, 0x01, 0x01, 0x04,

	// ============================================
	// Video Streaming Interface Alt 1 (9 bytes)
	// ============================================
	0x09, 0x04, 0x01, 0x01, 0x01, 0x0E, 0x02, 0x00,
	0x00,

	// ============================================
	// Isochronous Endpoint (7 bytes)
	// ============================================
	0x07, 0x05, 0x83, 0x05, 0x00, 0x04, 0x01
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
	.dwMaxVideoFrameSize = USBD_TUNING_W * USBD_TUNING_H,
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
	.dwMaxVideoFrameSize = USBD_TUNING_W * USBD_TUNING_H,
	.dwMaxPayloadTransferSize = 0,
	.dwClockFrequency = 48000000,
	.bmFramingInfo = 0,
	.bPreferedVersion = 0,
	.bMinVersion    = 0,
	.bMaxVersion    = 0,
};

usbd_uvc_frame_info_t uvc_frames_yuy2[] = {
	{ USBD_TUNING_W, USBD_TUNING_H, { USBD_UVC_FPS(30), USBD_UVC_FPS(15), USBD_UVC_FPS(10), 0 },},
	{ 0, 0, { 0, },  },
};

usbd_uvc_frame_info_t uvc_frames_nv12[] = {
	{ USBD_UVC_FRAME_WIDTH, USBD_UVC_FRAME_HEIGHT, { USBD_UVC_FPS(30), USBD_UVC_FPS(15), USBD_UVC_FPS(10), 0 },},
	{ 0, 0, { 0, }, },
};

usbd_uvc_frame_info_t uvc_frames_mjpeg[] = {
	{ USBD_UVC_JPEG_FRAME_WIDTH, USBD_UVC_JPEG_FRAME_HEIGHT, { USBD_UVC_FPS(30), USBD_UVC_FPS(15), USBD_UVC_FPS(10), 0 },},
	{ 0, 0, { 0, }, },
};

usbd_uvc_frame_info_t uvc_frames_h264[] = {
	{ USBD_UVC_FRAME_WIDTH, USBD_UVC_FRAME_HEIGHT, { USBD_UVC_FPS(30), USBD_UVC_FPS(15), USBD_UVC_FPS(10), 0 },},
	{ 0, 0, { 0, }, },
};

usbd_uvc_frame_info_t uvc_frames_h265[] = {
	{ USBD_UVC_FRAME_WIDTH, USBD_UVC_FRAME_HEIGHT, { USBD_UVC_FPS(30), USBD_UVC_FPS(15), USBD_UVC_FPS(10), 0 },},
	{ 0, 0, { 0, }, },
};

usbd_uvc_format_info_t uvcd_formats[] = {
	{ USBD_UVC_FORMAT_TYPE_YUY2, uvc_frames_yuy2 },
	{ USBD_UVC_FORMAT_TYPE_NV12, uvc_frames_nv12 },
	{ USBD_UVC_FORMAT_TYPE_MJPEG, uvc_frames_mjpeg},
	{ USBD_UVC_FORMAT_TYPE_H264, uvc_frames_h264 },
	{ USBD_UVC_FORMAT_TYPE_H265, uvc_frames_h265 },
};
#endif