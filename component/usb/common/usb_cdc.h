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

/* CDC Ethernet subclass class-specific request codes.
 *
 * Ref CDC 1.2 Table 13: these belong to the Ethernet subclass as a whole, not to
 * one model, so every Ethernet-carrying CDC model shares them - ECM (subclass
 * 0x06) and NCM (subclass 0x0D) alike.  They live here, next to the Ethernet
 * Networking functional descriptor and the network notification codes above,
 * so that neither model has to include the other model's header. */
#define USB_CDC_SET_ETHERNET_MULTICAST_FILTERS          0x40U /**< Set Ethernet multicast address filters */
#define USB_CDC_SET_ETHERNET_POWER_MANAGEMENT           0x41U /**< Set device power management pattern filter */
#define USB_CDC_GET_ETHERNET_POWER_MANAGEMENT           0x42U /**< Get device power management pattern filter */
#define USB_CDC_SET_ETHERNET_PACKET_FILTER              0x43U /**< Set Ethernet packet filter bitmap */
#define USB_CDC_GET_ETHERNET_STATISTIC                  0x44U /**< Retrieve Ethernet device statistics */

/* Response sizes of the Ethernet subclass Device-to-Host requests.
 * Ref CDC 1.2 6.2.6: GET_ETHERNET_STATISTIC returns one 32-bit counter.
 * Ref CDC 1.2 6.2.4: GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER returns a
 * 16-bit boolean.  Both lengths are defined by the request, not by the host. */
#define USB_CDC_ETHERNET_STATISTIC_RESPONSE_LEN         4U /**< GET_ETHERNET_STATISTIC response size */
#define USB_CDC_ETHERNET_POWER_MANAGEMENT_RESPONSE_LEN  2U /**< GET_ETHERNET_POWER_MANAGEMENT response size */

/* CDC Notification Codes */
#define USB_CDC_NOTIFY_NETWORK_CONNECTION               0x00U /**< Network Connection Notification */
#define USB_CDC_NOTIFY_RESPONSE_AVAILABLE               0x01U /**< Response Available Notification */
#define USB_CDC_NOTIFY_CONNECTION_SPEED_CHANGE          0x2AU /**< Connection Speed Change Notification */

/* CDC Notification Header length (CDC spec §6.3): bmRequestType+bCode+wValue+wIndex+wLength */
#define USB_CDC_NOTIFY_HDR_LEN                          8U

/* Connection bit rates reported by CONNECTION_SPEED_CHANGE (Ref CDC 1.2 6.3.3:
 * DLBitRate/ULBitRate are the actual connection bit rates in bits/s).  The USB bus
 * is the connection an Ethernet-carrying CDC function rides on, so its raw signalling
 * rate is what these fields describe. */
#define USB_CDC_CONNECTION_BITRATE_HS                   480000000U /**< High Speed: 480 Mbit/s */
#define USB_CDC_CONNECTION_BITRATE_FS                   12000000U  /**< Full Speed: 12 Mbit/s */

/** @} End of USB_Common_Constants group */
/** @} End of USB_Common_API group */

#ifdef __cplusplus
}
#endif

#endif  /* USB_CDC_H */
