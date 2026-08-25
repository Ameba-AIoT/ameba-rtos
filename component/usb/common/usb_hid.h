/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_HID_H
#define USB_HID_H

/* Human Interface Device (HID) Class Codes */

#define USB_HID_CLASS_CODE                  0x03U   /**< HID interface class code.      */
#define USB_HID_SUBCLASS_NONE               0x00U   /**< No subclass.                    */
#define USB_HID_PROTOCOL_NONE               0x00U   /**< No protocol (boot IF not used). */

#define USB_HID_PROTOCOL_KEYBOARD           0x01U   /**< Keyboard boot protocol.         */
#define USB_HID_PROTOCOL_MOUSE              0x02U   /**< Mouse boot protocol.            */

#endif /* USB_HID_H */
