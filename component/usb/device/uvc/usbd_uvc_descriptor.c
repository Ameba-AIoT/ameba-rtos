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
	0x05, 0x25, 0x03,       // bLength=5, bDescriptorType=CS_ENDPOINT, bDescriptorSubtype=EP_INTERRUPT
	0x40, 0x00,             // wMaxTransferSize = 64

	/* =========================
	 * Interface 1: Video Streaming (Alt 0)
	 * ========================= */
	0x09, 0x04,             // bLength=9, bDescriptorType=INTERFACE
	0x01,                   // bInterfaceNumber = 1
	0x00,                   // bAlternateSetting = 0 (zero-bandwidth)
	0x00,                   // bNumEndpoints = 0
	0x0E,                   // bInterfaceClass = CC_VIDEO
	0x02,                   // bInterfaceSubClass = SC_VIDEOSTREAMING
	0x00,                   // bInterfaceProtocol = PC_PROTOCOL_UNDEFINED
	0x00,                   // iInterface

	/* =========================
	 * VS Header Descriptor
	 * ========================= */
	0x0E, 0x24, 0x01,       // bLength=14, CS_INTERFACE, VS_INPUT_HEADER
	0x01,                   // bNumFormats = 1
	0x56, 0x00,             // wTotalLength = 86 (VS class descriptors)
	USBD_UVC_ISO_IN_EP,     // bEndpointAddress
	0x00,                   // bmInfo
	0x04, 0x00,             // bTerminalLink=4 (OT), bStillCaptureMethod=0
	0x00, 0x00,             // bTriggerSupport=0, bTriggerUsage=0
	0x01, 0x00,             // bControlSize=1, bmaControls[0]=0

	/* =========================
	 * VS Format Descriptor (H.264)
	 * ========================= */
	0x1C, 0x24, 0x10,               // bLength=28, CS_INTERFACE, VS_FORMAT_FRAME_BASED (H.264)
	0x01,                           // bFormatIndex = 1
	0x01,                           // bNumFrameDescriptors = 1
	'H', '2', '6', '4',             // guidFormat[0..3]
	0x00, 0x00,                     // guidFormat[4..5]
	0x10, 0x00,                     // guidFormat[6..7]
	0x80, 0x00,                     // guidFormat[8..9]
	0x00, 0xAA,                     // guidFormat[10..11]
	0x00,                           // guidFormat[12]
	0x38, 0x9B, 0x71, 0x0C,         // guidFormat[13..15], bBitsPerPixel = 12
	0x01,                           // bDefaultFrameIndex = 1
	0x01,                           // bAspectRatioX = 1
	0x00, 0x00, 0x00, 0x01,         // bAspectRatioY, bmInterlaceFlags, bCopyProtect, bVariableSize

	/* =========================
	 * VS Frame Descriptor
	 * ========================= */
	0x26, 0x24, 0x11,               // bLength=38, CS_INTERFACE, VS_FRAME_FRAME_BASED (H.264)
	0x01,                           // bFrameIndex = 1
	0x00,                           // bmCapabilities = 0
	//0x80, 0x02,             // Width = 640
	//0xE0, 0x01,             // Height = 480
	USBD_UVC_WBVAL(USBD_UVC_FRAME_WIDTH),     // wWidth
	USBD_UVC_WBVAL(USBD_UVC_FRAME_HEIGHT),    // wHeight
	0x00, 0x40, 0x19, 0x01,         // dwMinBitRate = 18,432,000 bps
	0x00, 0xC0, 0x4B, 0x03,         // dwMaxBitRate = 55,296,000 bps
	0x15, 0x16, 0x05, 0x00,         // dwDefaultFrameInterval = 333333 × 100ns (30fps)
	0x03,                           // bFrameIntervalType = 3 (discrete)
	0x00, 0x00,                     // dwBytesPerLine (LS word) = 0 (compressed)
	0x00, 0x00,                     // dwBytesPerLine (MS word) = 0
	0x15, 0x16, 0x05, 0x00,         // dwFrameInterval[0] = 333333 × 100ns (30fps)
	0x2A, 0x2C, 0x0A, 0x00,         // dwFrameInterval[1] = 666666 × 100ns (15fps)
	0x40, 0x42, 0x0F, 0x00,         // dwFrameInterval[2] = 1000000 × 100ns (10fps)

	/* =========================
	 * VS Color Matching Descriptor
	 * ========================= */
	0x06, 0x24, 0x0D,       // bLength=6, CS_INTERFACE, VS_COLORFORMAT
	0x01,                   // bColorPrimaries = BT.709/sRGB
	0x01,                   // bTransferCharacteristics = sRGB
	0x04,                   // bMatrixCoefficients = SMPTE 170M (BT.601)

	/* =========================
	 * Interface 1: Video Streaming (Alt 1)
	 * ========================= */
	0x09, 0x04,             // bLength=9, bDescriptorType=INTERFACE
	0x01,                   // bInterfaceNumber = 1
	0x01,                   // bAlternateSetting = 1 (active streaming)
	0x01,                   // bNumEndpoints = 1 (ISOC IN)
	0x0E,                   // bInterfaceClass = CC_VIDEO
	0x02,                   // bInterfaceSubClass = SC_VIDEOSTREAMING
	0x00,                   // bInterfaceProtocol = PC_PROTOCOL_UNDEFINED
	0x00,                   // iInterface

	/* =========================
	 * Isochronous Video Endpoint
	 * ========================= */
	0x07, 0x05,             // bLength=7, bDescriptorType=ENDPOINT
	USBD_UVC_ISO_IN_EP,     // bEndpointAddress
	0x05,                   // bmAttributes = Isochronous, asynchronous
	USBD_UVC_WBVAL(USBD_UVC_ISOC_EP_MPKT), // wMaxPacketSize: bits[12:11]=extra_trans, bits[10:0]=MPS
	0x01,                   // bInterval = 1 microframe
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
	.dwClockFrequency = USBD_UVC_CLOCK_FREQUENCY,
	.bmFramingInfo = 0,
	.bPreferedVersion = 0,
	.bMinVersion = 0,
	.bMaxVersion = 0,
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
	.dwClockFrequency = USBD_UVC_CLOCK_FREQUENCY,
	.bmFramingInfo = 0,
	.bPreferedVersion = 0,
	.bMinVersion = 0,
	.bMaxVersion = 0,
};


usbd_uvc_frame_info_t uvc_frames_h264[] = {
	{ USBD_UVC_FRAME_WIDTH, USBD_UVC_FRAME_HEIGHT, { USBD_UVC_FPS(30), USBD_UVC_FPS(15), USBD_UVC_FPS(10), 0 },},
	{ 0, 0, { 0, }, },
};
usbd_uvc_format_info_t uvcd_formats[] = {
	{ USBD_UVC_FORMAT_TYPE_H264, uvc_frames_h264 },
};
const u32 uvcd_nformats = sizeof(uvcd_formats) / sizeof(uvcd_formats[0]);
#else

u8 usbd_uvc_descriptors[] = {
	/* ===== USB Configuration Descriptor (9 bytes) [USB2.0 9.6.3] ===== */
	0x09,                                           /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType: CONFIGURATION */
	USB_LOW_BYTE(450 + USBD_UVC_XU_DESC_LEN),       /* wTotalLength (LSB): 450 (+28 if XU enabled) */
	USB_HIGH_BYTE(450 + USBD_UVC_XU_DESC_LEN),      /* wTotalLength (MSB) */
	0x02,                                           /* bNumInterfaces: VideoControl + VideoStreaming */
	0x01,                                           /* bConfigurationValue */
	0x01,                                           /* iConfiguration */
	0x80,                                           /* bmAttributes: bus powered */
	0x32,                                           /* bMaxPower: 100 mA */

	/* ===== Interface Association Descriptor (8 bytes) [UVC1.5 3.6] ===== */
	0x08,                                           /* bLength */
	0x0B,                                           /* bDescriptorType: IAD */
	0x00,                                           /* bFirstInterface: VC (0) */
	0x02,                                           /* bInterfaceCount: VC + VS */
	0x0E,                                           /* bFunctionClass: CC_VIDEO */
	0x03,                                           /* bFunctionSubClass: SC_VIDEO_INTERFACE_COLLECTION */
	0x00,                                           /* bFunctionProtocol */
	0x00,                                           /* iFunction */

	/* ===== VideoControl Interface, Alt 0 (9 bytes) ===== */
	0x09,                                           /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType: INTERFACE */
	0x00,                                           /* bInterfaceNumber: 0 */
	0x00,                                           /* bAlternateSetting */
	0x01,                                           /* bNumEndpoints: 1 (status interrupt) */
	0x0E,                                           /* bInterfaceClass: CC_VIDEO */
	0x01,                                           /* bInterfaceSubClass: SC_VIDEOCONTROL */
	0x00,                                           /* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* ===== Class-specific VC Interface Header (13 bytes) [UVC1.5 Tbl 3-3] ===== */
	0x0D,                                           /* bLength */
	0x24,                                           /* bDescriptorType: CS_INTERFACE */
	0x01,                                           /* bDescriptorSubtype: VC_HEADER */
	0x10, 0x01,                                     /* bcdUVC: 1.10 */
	USB_LOW_BYTE(52 + USBD_UVC_XU_DESC_LEN),        /* wTotalLength (LSB): VC class descriptors, 52 (+28 if XU) */
	USB_HIGH_BYTE(52 + USBD_UVC_XU_DESC_LEN),       /* wTotalLength (MSB) */
	0x00, 0x6C, 0xDC, 0x02,                         /* dwClockFrequency: 0x02DC6C00 = 48 MHz */
	0x01,                                           /* bInCollection: 1 streaming interface */
	0x01,                                           /* baInterfaceNr(1): VS = interface 1 */

	/* ===== Camera Input Terminal (18 bytes) [UVC1.5 Tbl 3-4] ===== */
	0x12,                                           /* bLength */
	0x24,                                           /* bDescriptorType: CS_INTERFACE */
	0x02,                                           /* bDescriptorSubtype: VC_INPUT_TERMINAL */
	0x01,                                           /* bTerminalID: 1 */
	0x01, 0x02,                                     /* wTerminalType: 0x0201 ITT_CAMERA */
	0x00,                                           /* bAssocTerminal */
	0x00,                                           /* iTerminal */
	0x00, 0x00,                                     /* wObjectiveFocalLengthMin */
	0x00, 0x00,                                     /* wObjectiveFocalLengthMax */
	0x00, 0x00,                                     /* wOcularFocalLength */
	0x03,                                           /* bControlSize */
	0x00, 0x00, 0x00,                               /* bmControls: none implemented by firmware */

	/* ===== Processing Unit (12 bytes, UVC1.1) [UVC1.5 Tbl 3-8] ===== */
	0x0C,                                           /* bLength (10 + bControlSize) */
	0x24,                                           /* bDescriptorType: CS_INTERFACE */
	0x05,                                           /* bDescriptorSubtype: VC_PROCESSING_UNIT */
	0x02,                                           /* bUnitID: 2 */
	0x01,                                           /* bSourceID: 1 (Camera Terminal) */
	0x00, 0x40,                                     /* wMaxMultiplier: 0x4000 */
	0x02,                                           /* bControlSize */
	0x00, 0x00,                                     /* bmControls: none implemented by firmware */
	0x00,                                           /* iProcessing */
	0x00,                                           /* bmVideoStandards (UVC1.1): none */

#if USBD_UVC_USE_EXTENSION_UNIT
	/* ===== Extension Unit (28 bytes) [UVC1.5 Tbl 3-10] =====
	 * WARNING: disabled by default. Before enabling, make bNumControls / bmControls
	 * consistent and back every declared control with a real GET/SET handler in
	 * usbd_uvc_event.c; otherwise a strict host (Windows) fails enumeration.
	 * (The historical value bNumControls=24 with only 4 bmControls bits set is
	 * inconsistent and was rejected by CATC.) */
	0x1C,                                           /* bLength (24 + p + n) */
	0x24,                                           /* bDescriptorType: CS_INTERFACE */
	0x06,                                           /* bDescriptorSubtype: VC_EXTENSION_UNIT */
	0x03,                                           /* bUnitID: 3 */
	0xCE, 0xB9, 0x55, 0x2B, 0xB0, 0x14, 0x49, 0xA8, /* guidExtensionCode (16) */
	0xB2, 0x2E, 0xF6, 0xB0, 0xA0, 0x12, 0x82, 0x74,
	0x18,                                           /* bNumControls */
	0x01,                                           /* bNrInPins: 1 */
	0x02,                                           /* baSourceID(1): 2 (Processing Unit) */
	0x03,                                           /* bControlSize */
	0x00, 0x1E, 0x00,                               /* bmControls */
	0x00,                                           /* iExtension */
#endif

	/* ===== Output Terminal (9 bytes) [UVC1.5 Tbl 3-5] ===== */
	0x09,                                           /* bLength */
	0x24,                                           /* bDescriptorType: CS_INTERFACE */
	0x03,                                           /* bDescriptorSubtype: VC_OUTPUT_TERMINAL */
	0x04,                                           /* bTerminalID: 4 */
	0x01, 0x01,                                     /* wTerminalType: 0x0101 TT_STREAMING */
	0x00,                                           /* bAssocTerminal */
	USBD_UVC_OT_SOURCE_ID,                          /* bSourceID: PU(2) or XU(3) per USBD_UVC_USE_EXTENSION_UNIT */
	0x00,                                           /* iTerminal */

	/* ===== VC Status Interrupt Endpoint (7 bytes) ===== */
	0x07,                                           /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType: ENDPOINT */
	0x81,                                           /* bEndpointAddress: EP1 IN */
	0x03,                                           /* bmAttributes: Interrupt */
	0x40, 0x00,                                     /* wMaxPacketSize: 64 */
	0x08,                                           /* bInterval */

	/* ===== Class-specific VC Interrupt Endpoint (5 bytes) ===== */
	0x05,                                           /* bLength */
	0x25,                                           /* bDescriptorType: CS_ENDPOINT */
	0x03,                                           /* bDescriptorSubtype: EP_INTERRUPT */
	0x40, 0x00,                                     /* wMaxTransferSize: 64 */

	/* ===== VideoStreaming Interface, Alt 0 (zero-bandwidth, 9 bytes) ===== */
	0x09,                                           /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType: INTERFACE */
	0x01,                                           /* bInterfaceNumber: 1 */
	0x00,                                           /* bAlternateSetting: 0 */
	0x00,                                           /* bNumEndpoints: 0 */
	0x0E,                                           /* bInterfaceClass: CC_VIDEO */
	0x02,                                           /* bInterfaceSubClass: SC_VIDEOSTREAMING */
	0x00,                                           /* bInterfaceProtocol */
	0x00,                                           /* iInterface */

	/* ===== Class-specific VS Input Header (18 bytes) [UVC1.5 Tbl 3-14] ===== */
	0x12,                                           /* bLength: 13 + bControlSize * bNumFormats = 13 + 1*5 */
	0x24,                                           /* bDescriptorType: CS_INTERFACE */
	0x01,                                           /* bDescriptorSubtype: VS_INPUT_HEADER */
	0x05,                                           /* bNumFormats: 5 (NV16/NV12/MJPEG/H264/HEVC) */
	0x4F, 0x01,                                     /* wTotalLength: 0x014F = 335 (VS class descriptors) */
	USBD_UVC_ISO_IN_EP,                             /* bEndpointAddress */
	0x00,                                           /* bmInfo */
	0x04,                                           /* bTerminalLink: Output Terminal (id 4) */
	0x00,                                           /* bStillCaptureMethod: none */
	0x00,                                           /* bTriggerSupport */
	0x00,                                           /* bTriggerUsage */
	0x01,                                           /* bControlSize */
	0x00,                                           /* bmaControls(1): NV16 */
	0x00,                                           /* bmaControls(2): NV12 */
	0x00,                                           /* bmaControls(3): MJPEG */
	0x00,                                           /* bmaControls(4): H.264 */
	0x00,                                           /* bmaControls(5): HEVC */

	/* Format 1: NV16 Uncompressed (27 bytes) — VS_FORMAT_UNCOMPRESSED */
	0x1B,                                           /* bLength = 27 */
	0x24,                                           /* bDescriptorType = CS_INTERFACE */
	0x04,                                           /* bDescriptorSubtype = VS_FORMAT_UNCOMPRESSED */
	0x01,                                           /* bFormatIndex = 1 */
	0x01,                                           /* bNumFrameDescriptors = 1 */
	0x4E, 0x56, 0x31, 0x36,                         /* guidFormat: NV16 FOURCC bytes [0..3] */
	0x00, 0x00, 0x10, 0x00,                         /* guidFormat: standard video GUID [4..7] */
	0x80, 0x00, 0x00, 0xAA,                         /* guidFormat: [8..11] */
	0x00, 0x38, 0x9B, 0x71,                         /* guidFormat: [12..15] */
	0x10,                                           /* bBitsPerPixel = 16 */
	0x01,                                           /* bDefaultFrameIndex = 1 */
	0x00,                                           /* bAspectRatioX = 0 */
	0x00,                                           /* bAspectRatioY = 0 */
	0x00,                                           /* bmInterlaceFlags = 0 */
	0x00,                                           /* bCopyProtect = 0 */

	/* Frame Descriptor - NV16 (38 bytes) — VS_FRAME_UNCOMPRESSED, 3 discrete intervals */
	0x26,                                           /* bLength = 38 */
	0x24,                                           /* bDescriptorType = CS_INTERFACE */
	0x05,                                           /* bDescriptorSubtype = VS_FRAME_UNCOMPRESSED */
	0x01,                                           /* bFrameIndex = 1 */
	0x00,                                           /* bmCapabilities = 0 (no still image) */
	USBD_UVC_WBVAL(USBD_TUNING_W),                 /* wWidth */
	USBD_UVC_WBVAL(USBD_TUNING_H),                 /* wHeight */
	0x00, 0x80, 0xF4, 0x03,                         /* dwMinBitRate = 66,355,200 bps */
	0x00, 0xC0, 0xA9, 0x1D,                         /* dwMaxBitRate = 498,073,600 bps */
	0x00, 0x48, 0x3F, 0x00,                         /* dwMaxVideoFrameBufferSize = 4,147,200 B */
	0x9A, 0x5B, 0x06, 0x00,                         /* dwDefaultFrameInterval = 416666 x 100ns (24fps) */
	0x03,                                           /* bFrameIntervalType = 3 (discrete) */
	0x9A, 0x5B, 0x06, 0x00,                         /* dwFrameInterval[0] = 416666 x 100ns (24fps) */
	0x2A, 0x2C, 0x0A, 0x00,                         /* dwFrameInterval[1] = 666666 x 100ns (15fps) */
	0x40, 0x42, 0x0F, 0x00,                         /* dwFrameInterval[2] = 1000000 x 100ns (10fps) */

	/* Format 2: NV12 Uncompressed (27 bytes) — VS_FORMAT_UNCOMPRESSED */
	0x1B,                                           /* bLength = 27 */
	0x24,                                           /* bDescriptorType = CS_INTERFACE */
	0x04,                                           /* bDescriptorSubtype = VS_FORMAT_UNCOMPRESSED */
	0x02,                                           /* bFormatIndex = 2 */
	0x01,                                           /* bNumFrameDescriptors = 1 */
	0x4E, 0x56, 0x31, 0x32,                         /* guidFormat: NV12 FOURCC bytes [0..3] */
	0x00, 0x00, 0x10, 0x00,                         /* guidFormat: standard video GUID [4..7] */
	0x80, 0x00, 0x00, 0xAA,                         /* guidFormat: [8..11] */
	0x00, 0x38, 0x9B, 0x71,                         /* guidFormat: [12..15] */
	0x0C,                                           /* bBitsPerPixel = 12 */
	0x01,                                           /* bDefaultFrameIndex = 1 */
	0x00,                                           /* bAspectRatioX = 0 */
	0x00,                                           /* bAspectRatioY = 0 */
	0x00,                                           /* bmInterlaceFlags = 0 */
	0x00,                                           /* bCopyProtect = 0 */

	/* Frame Descriptor - NV12 (38 bytes) — VS_FRAME_UNCOMPRESSED, 3 discrete intervals */
	0x26,                                           /* bLength = 38 */
	0x24,                                           /* bDescriptorType = CS_INTERFACE */
	0x05,                                           /* bDescriptorSubtype = VS_FRAME_UNCOMPRESSED */
	0x01,                                           /* bFrameIndex = 1 */
	0x00,                                           /* bmCapabilities = 0 (no still image) */
	USBD_UVC_WBVAL(USBD_UVC_FRAME_WIDTH),          /* wWidth */
	USBD_UVC_WBVAL(USBD_UVC_FRAME_HEIGHT),         /* wHeight */
	0x00, 0xEC, 0x5E, 0x00,                         /* dwMinBitRate = 6,217,728 bps */
	0x00, 0xEA, 0xC7, 0x02,                         /* dwMaxBitRate = 46,966,272 bps */
	0x00, 0x76, 0x2F, 0x00,                         /* dwMaxVideoFrameBufferSize = 3,110,400 B */
	0x9A, 0x5B, 0x06, 0x00,                         /* dwDefaultFrameInterval = 416666 x 100ns (24fps) */
	0x03,                                           /* bFrameIntervalType = 3 (discrete) */
	0x9A, 0x5B, 0x06, 0x00,                         /* dwFrameInterval[0] = 416666 x 100ns (24fps) */
	0x2A, 0x2C, 0x0A, 0x00,                         /* dwFrameInterval[1] = 666666 x 100ns (15fps) */
	0x40, 0x42, 0x0F, 0x00,                         /* dwFrameInterval[2] = 1000000 x 100ns (10fps) */

	/* Format 3: MJPEG (11 bytes) — VS_FORMAT_MJPEG */
	0x0B,                                           /* bLength = 11 */
	0x24,                                           /* bDescriptorType = CS_INTERFACE */
	0x06,                                           /* bDescriptorSubtype = VS_FORMAT_MJPEG */
	0x03,                                           /* bFormatIndex = 3 */
	0x01,                                           /* bNumFrameDescriptors = 1 */
	0x01,                                           /* bmFlags = 1 (FixedSizeSamples) */
	0x01,                                           /* bDefaultFrameIndex = 1 */
	0x00,                                           /* bAspectRatioX = 0 */
	0x00,                                           /* bAspectRatioY = 0 */
	0x00,                                           /* bmInterlaceFlags = 0 */
	0x00,                                           /* bCopyProtect = 0 */

	/* Frame Descriptor - MJPEG (38 bytes) — VS_FRAME_MJPEG, 3 discrete intervals */
	0x26,                                           /* bLength = 38 */
	0x24,                                           /* bDescriptorType = CS_INTERFACE */
	0x07,                                           /* bDescriptorSubtype = VS_FRAME_MJPEG */
	0x01,                                           /* bFrameIndex = 1 */
	0x00,                                           /* bmCapabilities = 0 (no still image) */
	USBD_UVC_WBVAL(USBD_UVC_JPEG_FRAME_WIDTH),     /* wWidth */
	USBD_UVC_WBVAL(USBD_UVC_JPEG_FRAME_HEIGHT),    /* wHeight */
	0x00, 0xC4, 0x1C, 0x01,                         /* dwMinBitRate = 18,603,008 bps */
	0x00, 0x4C, 0x56, 0x03,                         /* dwMaxBitRate = 56,278,016 bps */
	0x00, 0x08, 0x07, 0x00,                         /* dwMaxVideoFrameBufferSize = 460,800 B */
	0x9A, 0x5B, 0x06, 0x00,                         /* dwDefaultFrameInterval = 416666 x 100ns (24fps) */
	0x03,                                           /* bFrameIntervalType = 3 (discrete) */
	0x9A, 0x5B, 0x06, 0x00,                         /* dwFrameInterval[0] = 416666 x 100ns (24fps) */
	0x2A, 0x2C, 0x0A, 0x00,                         /* dwFrameInterval[1] = 666666 x 100ns (15fps) */
	0x40, 0x42, 0x0F, 0x00,                         /* dwFrameInterval[2] = 1000000 x 100ns (10fps) */

	/* Format 4: H.264 (28 bytes) — VS_FORMAT_FRAME_BASED */
	0x1C,                                           /* bLength = 28 */
	0x24,                                           /* bDescriptorType = CS_INTERFACE */
	0x10,                                           /* bDescriptorSubtype = VS_FORMAT_FRAME_BASED */
	0x04,                                           /* bFormatIndex = 4 */
	0x01,                                           /* bNumFrameDescriptors = 1 */
	0x48, 0x32, 0x36, 0x34,                         /* guidFormat: H264 FOURCC bytes [0..3] */
	0x00, 0x00, 0x10, 0x00,                         /* guidFormat: standard video GUID [4..7] */
	0x80, 0x00, 0x00, 0xAA,                         /* guidFormat: [8..11] */
	0x00, 0x38, 0x9B, 0x71,                         /* guidFormat: [12..15] */
	0x0C,                                           /* bBitsPerPixel = 12 */
	0x01,                                           /* bDefaultFrameIndex = 1 */
	0x00,                                           /* bAspectRatioX = 0 */
	0x00,                                           /* bAspectRatioY = 0 */
	0x00,                                           /* bmInterlaceFlags = 0 */
	0x00,                                           /* bCopyProtect = 0 */
	0x01,                                           /* bVariableSize = 1 (variable-size frames) */

	/* Frame Descriptor - H.264 (38 bytes) — VS_FRAME_FRAME_BASED, 3 discrete intervals */
	0x26,                                           /* bLength = 38 */
	0x24,                                           /* bDescriptorType = CS_INTERFACE */
	0x11,                                           /* bDescriptorSubtype = VS_FRAME_FRAME_BASED */
	0x01,                                           /* bFrameIndex = 1 */
	0x00,                                           /* bmCapabilities = 0 (no still image) */
	USBD_UVC_WBVAL(USBD_UVC_FRAME_WIDTH),          /* wWidth */
	USBD_UVC_WBVAL(USBD_UVC_FRAME_HEIGHT),         /* wHeight */
	0x00, 0xC4, 0x1C, 0x01,                         /* dwMinBitRate = 18,603,008 bps */
	0x00, 0x4C, 0x56, 0x03,                         /* dwMaxBitRate = 56,278,016 bps */
	0x15, 0x16, 0x05, 0x00,                         /* dwDefaultFrameInterval = 333333 x 100ns (30fps) */
	0x03,                                           /* bFrameIntervalType = 3 (discrete) */
	0x00, 0x00, 0x00, 0x00,                         /* dwBytesPerLine = 0 (variable compressed size) */
	0x15, 0x16, 0x05, 0x00,                         /* dwFrameInterval[0] = 333333 x 100ns (30fps) */
	0x2A, 0x2C, 0x0A, 0x00,                         /* dwFrameInterval[1] = 666666 x 100ns (15fps) */
	0x40, 0x42, 0x0F, 0x00,                         /* dwFrameInterval[2] = 1000000 x 100ns (10fps) */

	/* Format 5: HEVC (28 bytes) — VS_FORMAT_FRAME_BASED */
	0x1C,                                           /* bLength = 28 */
	0x24,                                           /* bDescriptorType = CS_INTERFACE */
	0x10,                                           /* bDescriptorSubtype = VS_FORMAT_FRAME_BASED */
	0x05,                                           /* bFormatIndex = 5 */
	0x01,                                           /* bNumFrameDescriptors = 1 */
	0x48, 0x45, 0x56, 0x43,                         /* guidFormat: HEVC FOURCC bytes [0..3] */
	0x00, 0x00, 0x10, 0x00,                         /* guidFormat: standard video GUID [4..7] */
	0x80, 0x00, 0x00, 0xAA,                         /* guidFormat: [8..11] */
	0x00, 0x38, 0x9B, 0x71,                         /* guidFormat: [12..15] */
	0x0C,                                           /* bBitsPerPixel = 12 */
	0x01,                                           /* bDefaultFrameIndex = 1 */
	0x00,                                           /* bAspectRatioX = 0 */
	0x00,                                           /* bAspectRatioY = 0 */
	0x00,                                           /* bmInterlaceFlags = 0 */
	0x00,                                           /* bCopyProtect = 0 */
	0x01,                                           /* bVariableSize = 1 (variable-size frames) */

	/* Frame Descriptor - HEVC (38 bytes) — VS_FRAME_FRAME_BASED, 3 discrete intervals */
	0x26,                                           /* bLength = 38 */
	0x24,                                           /* bDescriptorType = CS_INTERFACE */
	0x11,                                           /* bDescriptorSubtype = VS_FRAME_FRAME_BASED */
	0x01,                                           /* bFrameIndex = 1 */
	0x00,                                           /* bmCapabilities = 0 (no still image) */
	USBD_UVC_WBVAL(USBD_UVC_FRAME_WIDTH),          /* wWidth */
	USBD_UVC_WBVAL(USBD_UVC_FRAME_HEIGHT),         /* wHeight */
	0x00, 0xC4, 0x1C, 0x01,                         /* dwMinBitRate = 18,603,008 bps */
	0x00, 0x4C, 0x56, 0x03,                         /* dwMaxBitRate = 56,278,016 bps */
	0x9A, 0x5B, 0x06, 0x00,                         /* dwDefaultFrameInterval = 416666 x 100ns (24fps) */
	0x03,                                           /* bFrameIntervalType = 3 (discrete) */
	0x00, 0x00, 0x00, 0x00,                         /* dwBytesPerLine = 0 (variable compressed size) */
	0x9A, 0x5B, 0x06, 0x00,                         /* dwFrameInterval[0] = 416666 x 100ns (24fps) */
	0x2A, 0x2C, 0x0A, 0x00,                         /* dwFrameInterval[1] = 666666 x 100ns (15fps) */
	0x40, 0x42, 0x0F, 0x00,                         /* dwFrameInterval[2] = 1000000 x 100ns (10fps) */

	/* Color Matching Descriptor (6 bytes) — VS_COLORFORMAT */
	0x06,                                           /* bLength = 6 */
	0x24,                                           /* bDescriptorType = CS_INTERFACE */
	0x0D,                                           /* bDescriptorSubtype = VS_COLORFORMAT */
	0x01,                                           /* bColorPrimaries = 1 (BT.709) */
	0x01,                                           /* bTransferCharacteristics = 1 (BT.709) */
	0x04,                                           /* bMatrixCoefficients = 4 (SMPTE 170M) */

	/* Video Streaming Interface Alt 1 (9 bytes) — active streaming alternate setting */
	0x09,                                           /* bLength = 9 */
	0x04,                                           /* bDescriptorType = INTERFACE */
	0x01,                                           /* bInterfaceNumber = 1 (VS) */
	0x01,                                           /* bAlternateSetting = 1 (active) */
	0x01,                                           /* bNumEndpoints = 1 */
	0x0E,                                           /* bInterfaceClass = CC_VIDEO */
	0x02,                                           /* bInterfaceSubClass = SC_VIDEOSTREAMING */
	0x00,                                           /* bInterfaceProtocol = PC_PROTOCOL_UNDEFINED */
	0x00,                                           /* iInterface = 0 (no string) */

	/* Isochronous Endpoint (7 bytes) — ISOC IN, HS high-bandwidth 3x1024 */
	0x07,                                           /* bLength = 7 */
	0x05,                                           /* bDescriptorType = ENDPOINT */
	USBD_UVC_ISO_IN_EP,                             /* bEndpointAddress */
	0x05,                                           /* bmAttributes = ISOC | ASYNC */
	USBD_UVC_WBVAL(USBD_UVC_ISOC_EP_MPKT),         /* wMaxPacketSize: bits[12:11]=extra_trans, bits[10:0]=MPS */
	0x01                                            /* bInterval = 1 (every microframe) */
};

int usbd_uvc_descriptors_size = sizeof(usbd_uvc_descriptors);

usbd_uvc_streaming_control_t usbd_uvc_probe = {
	.bmHint = 0,
	.bFormatIndex = 4,
	.bFrameIndex = 1,
	.dwFrameInterval = 333333,
	.wKeyFrameRate = 0,
	.wPFrameRate = 0,
	.wCompQuality = 0,
	.wCompWindowSize = 0,
	.wDelay = 0,
	.dwMaxVideoFrameSize = USBD_TUNING_W * USBD_TUNING_H,
	.dwMaxPayloadTransferSize = 0,
	.dwClockFrequency = USBD_UVC_CLOCK_FREQUENCY,
	.bmFramingInfo = 0,
	.bPreferedVersion = 0,
	.bMinVersion = 0,
	.bMaxVersion = 0,
};

usbd_uvc_streaming_control_t usbd_uvc_commit = {
	.bmHint = 0,
	.bFormatIndex = 4,
	.bFrameIndex = 1,
	.dwFrameInterval = 333333,
	.wKeyFrameRate = 0,
	.wPFrameRate = 0,
	.wCompQuality = 0,
	.wCompWindowSize = 0,
	.wDelay = 0,
	.dwMaxVideoFrameSize = USBD_TUNING_W * USBD_TUNING_H,
	.dwMaxPayloadTransferSize = 0,
	.dwClockFrequency = USBD_UVC_CLOCK_FREQUENCY,
	.bmFramingInfo = 0,
	.bPreferedVersion = 0,
	.bMinVersion = 0,
	.bMaxVersion = 0,
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
const u32 uvcd_nformats = sizeof(uvcd_formats) / sizeof(uvcd_formats[0]);
#endif