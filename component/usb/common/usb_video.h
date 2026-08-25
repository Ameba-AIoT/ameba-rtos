/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_VIDEO_H
#define USB_VIDEO_H

/* Video Device Class (UVC) Codes — USB-IF Video Class Code Table */

#define USB_VIDEO_CLASS_CODE                        0x0EU   /**< Video interface class code. */

/* Video Interface Subclass Codes (UVC 1.5 §A.2) */
#define USB_VIDEO_SUBCLASS_VIDEOCONTROL             0x01U   /**< Video Control (VC) interface. */
#define USB_VIDEO_SUBCLASS_VIDEOSTREAMING           0x02U   /**< Video Streaming (VS) interface. */
#define USB_VIDEO_SUBCLASS_INTERFACE_COLLECTION     0x03U   /**< Video Interface Collection (IAD). */

/* Video Protocol Codes */
#define USB_VIDEO_PROTOCOL_UNDEFINED                0x00U   /**< No protocol / undefined. */

#endif /* USB_VIDEO_H */
