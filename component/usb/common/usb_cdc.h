/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_CDC_H
#define USB_CDC_H

/* Includes ------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Common_API USB Common API
 *  @{
 */
/** @addtogroup USB_Common_Constants USB Common Constants
 * @{
 */

/* CDC Class Codes */
#define USB_CDC_CLASS_CODE                              0x02U /**< USB Communication Device Class (CDC) Code */
#define USB_CDC_COMM_INTERFACE_CLASS_CODE               0x02U /**< CDC Communication Interface Class Code */
#define USB_CDC_DATA_INTERFACE_CLASS_CODE               0x0AU /**< CDC Data Interface Class Code */

/* CDC Communication Subclass Codes */
#define USB_CDC_SUBCLASS_RESERVED                       0x00U /**< CDC Subclass Code: Reserved */

/* Communication Interface Class Control Protocol Codes */
#define USB_CDC_CTRL_PROTOCOL_NO_CLASS_SPECIFIC         0x00U  /**< CDC Protocol Code: No class specific protocol */
#define USB_CDC_CTRL_PROTOCOL_VENDOR_SPECIFIC           0xFFU  /**< CDC Protocol Code: Vendor specific */

/* Data Interface Class Protocol Codes */
#define USB_CDC_DATA_PROTOCOL_NO_CLASS_SPECIFIC         0x00U  /**< CDC Data Protocol: No class specific protocol */
#define USB_CDC_DATA_PROTOCOL_NETWORK_TRANSFER_BLOCK    0x01U  /**< CDC Data Protocol: Network Transfer Block */

/* CDC Functional Descriptor Types */
#define USB_CDC_CS_INTERFACE                            0x24U /**< Class-Specific Interface Descriptor Type */
#define USB_CDC_CS_ENDPOINT                             0x25U /**< Class-Specific Endpoint Descriptor Type */

/* CDC Functional Descriptor Subtypes */
#define USB_CDC_FUNC_DESC_HEADER                        0x00U /**< Header Functional Descriptor */
#define USB_CDC_FUNC_DESC_CALL_MGMT                     0x01U /**< Call Management Functional Descriptor */
#define USB_CDC_FUNC_DESC_UNION                         0x06U /**< Union Functional Descriptor */
#define USB_CDC_FUNC_DESC_ETHERNET_NETWORKING           0x0FU /**< Ethernet Networking Functional Descriptor */

/* CDC Notification Codes */
#define USB_CDC_NOTIFY_NETWORK_CONNECTION               0x00U /**< Network Connection Notification */
#define USB_CDC_NOTIFY_RESPONSE_AVAILABLE               0x01U /**< Response Available Notification */
#define USB_CDC_NOTIFY_CONNECTION_SPEED_CHANGE          0x2AU /**< Connection Speed Change Notification */

/* CDC Notification Header length (CDC spec §6.3): bmRequestType+bCode+wValue+wIndex+wLength */
#define USB_CDC_NOTIFY_HDR_LEN                          8U

/** @} End of USB_Common_Constants group */
/** @} End of USB_Common_API group */

#ifdef __cplusplus
}
#endif

#endif  /* USB_CDC_H */
