/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_HID_H
#define USB_HID_H

/* Includes ------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Common_API USB Common API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Common_Constants USB Common Constants
 *  @{
 */
/** @defgroup USB_Common_HID_Constants USB HID Constants (HID 1.11)
 *  @brief Common protocol definitions shared by the USBD and USBH HID drivers.
 *  @{
 */

/* Human Interface Device (HID) Class Codes */

#define USB_HID_CLASS_CODE                  0x03U   /**< HID interface class code.      */
#define USB_HID_SUBCLASS_NONE               0x00U   /**< No subclass.                    */
#define USB_HID_PROTOCOL_NONE               0x00U   /**< No protocol (boot IF not used). */

#define USB_HID_PROTOCOL_KEYBOARD           0x01U   /**< Keyboard boot protocol.         */
#define USB_HID_PROTOCOL_MOUSE              0x02U   /**< Mouse boot protocol.            */

/** @} End of USB_Common_HID_Constants group */
/** @} End of USB_Common_Constants group */

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @} End of USB_Common_API group */

#ifdef __cplusplus
}
#endif

#endif /* USB_HID_H */
