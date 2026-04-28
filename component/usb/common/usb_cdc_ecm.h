/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_CDC_ECM_H
#define USB_CDC_ECM_H

/* Includes ------------------------------------------------------------------*/
#include "basic_types.h"

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
#define USB_CDC_SUBCLASS_ECM                            0x06U /**< CDC Subclass Code: Ethernet Control Model (ECM) */

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
#define USB_CDC_FUNC_DESC_UNION                         0x06U /**< Union Functional Descriptor */
#define USB_CDC_FUNC_DESC_ETHERNET_NETWORKING           0x0FU /**< Ethernet Networking Functional Descriptor */

/* CDC ECM Class-Specific Request Codes */
#define USB_CDC_ECM_SET_ETHERNET_MULTICAST_FILTERS      0x40U /**< Set Ethernet multicast address filters */
#define USB_CDC_ECM_SET_ETHERNET_POWER_MANAGEMENT       0x41U /**< Set device power management pattern filter */
#define USB_CDC_ECM_GET_ETHERNET_POWER_MANAGEMENT       0x42U /**< Get device power management pattern filter */
#define USB_CDC_ECM_SET_ETHERNET_PACKET_FILTER          0x43U /**< Set Ethernet packet filter bitmap */
#define USB_CDC_ECM_GET_ETHERNET_STATISTIC              0x44U /**< Retrieve Ethernet device statistics */

/* CDC ECM Notification Codes */
#define USB_CDC_ECM_NOTIFY_NETWORK_CONNECTION           0x00U /**< Network Connection Notification */
#define USB_CDC_ECM_NOTIFY_RESPONSE_AVAILABLE           0x01U /**< Response Available Notification */
#define USB_CDC_ECM_NOTIFY_CONNECTION_SPEED_CHANGE      0x2AU /**< Connection Speed Change Notification */

/* Ethernet Packet Filter Bitmap (wValue for SET_ETHERNET_PACKET_FILTER) */
#define USB_CDC_ECM_PACKET_TYPE_PROMISCUOUS             (1U << 0) /**< Promiscuous mode */
#define USB_CDC_ECM_PACKET_TYPE_ALL_MULTICAST           (1U << 1) /**< All multicast packets */
#define USB_CDC_ECM_PACKET_TYPE_DIRECTED                (1U << 2) /**< Directed packets */
#define USB_CDC_ECM_PACKET_TYPE_BROADCAST               (1U << 3) /**< Broadcast packets */
#define USB_CDC_ECM_PACKET_TYPE_MULTICAST               (1U << 4) /**< Filtered multicast packets */

/* Ethernet Statistics Feature Selector Codes */
#define USB_CDC_ECM_STAT_XMIT_OK                        0x01U /**< Frames transmitted without errors */
#define USB_CDC_ECM_STAT_RCV_OK                         0x02U /**< Frames received without errors */
#define USB_CDC_ECM_STAT_XMIT_ERROR                     0x03U /**< Frames not transmitted due to errors */
#define USB_CDC_ECM_STAT_RCV_ERROR                      0x04U /**< Frames received with errors */
#define USB_CDC_ECM_STAT_RCV_NO_BUFFER                  0x05U /**< Frames missed due to no buffer */
#define USB_CDC_ECM_STAT_DIRECTED_BYTES_XMIT            0x06U /**< Directed bytes transmitted without errors */
#define USB_CDC_ECM_STAT_DIRECTED_FRAMES_XMIT           0x07U /**< Directed frames transmitted without errors */
#define USB_CDC_ECM_STAT_MULTICAST_BYTES_XMIT           0x08U /**< Multicast bytes transmitted without errors */
#define USB_CDC_ECM_STAT_MULTICAST_FRAMES_XMIT          0x09U /**< Multicast frames transmitted without errors */
#define USB_CDC_ECM_STAT_BROADCAST_BYTES_XMIT           0x0AU /**< Broadcast bytes transmitted without errors */
#define USB_CDC_ECM_STAT_BROADCAST_FRAMES_XMIT          0x0BU /**< Broadcast frames transmitted without errors */
#define USB_CDC_ECM_STAT_DIRECTED_BYTES_RCV             0x0CU /**< Directed bytes received without errors */
#define USB_CDC_ECM_STAT_DIRECTED_FRAMES_RCV            0x0DU /**< Directed frames received without errors */
#define USB_CDC_ECM_STAT_MULTICAST_BYTES_RCV            0x0EU /**< Multicast bytes received without errors */
#define USB_CDC_ECM_STAT_MULTICAST_FRAMES_RCV           0x0FU /**< Multicast frames received without errors */
#define USB_CDC_ECM_STAT_BROADCAST_BYTES_RCV            0x10U /**< Broadcast bytes received without errors */
#define USB_CDC_ECM_STAT_BROADCAST_FRAMES_RCV           0x11U /**< Broadcast frames received without errors */
#define USB_CDC_ECM_STAT_RCV_CRC_ERROR                  0x12U /**< Frames received with CRC errors */
#define USB_CDC_ECM_STAT_TRANSMIT_QUEUE_LENGTH          0x13U /**< Length of transmit queue */
#define USB_CDC_ECM_STAT_RCV_ERROR_ALIGNMENT            0x14U /**< Frames received with alignment errors */
#define USB_CDC_ECM_STAT_XMIT_ONE_COLLISION             0x15U /**< Frames transmitted with one collision */
#define USB_CDC_ECM_STAT_XMIT_MORE_COLLISIONS           0x16U /**< Frames transmitted with more than one collision */
#define USB_CDC_ECM_STAT_XMIT_DEFERRED                  0x17U /**< Frames transmitted after deferral */
#define USB_CDC_ECM_STAT_XMIT_MAX_COLLISIONS            0x18U /**< Frames not transmitted due to collisions */
#define USB_CDC_ECM_STAT_RCV_OVERRUN                    0x19U /**< Frames not received due to overrun */
#define USB_CDC_ECM_STAT_XMIT_UNDERRUN                  0x1AU /**< Frames not transmitted due to underrun */
#define USB_CDC_ECM_STAT_XMIT_HEARTBEAT_FAILURE         0x1BU /**< Frames transmitted with heartbeat failure */
#define USB_CDC_ECM_STAT_XMIT_TIMES_CRS_LOST            0x1CU /**< Times carrier sense signal lost during transmission */
#define USB_CDC_ECM_STAT_XMIT_LATE_COLLISIONS           0x1DU /**< Late collisions detected */

/* Standard Packet Sizes and Lengths */
#define USB_CDC_ECM_INTR_IN_PACKET_SIZE                 16U      /**< Maximum packet size for Interrupt IN endpoint */
#define USB_CDC_ECM_NETWORK_CONNECTION_SIZE             8U       /**< Network Connection notification size */
#define USB_CDC_ECM_CONNECTION_SPEED_CHANGE_SIZE        16U      /**< Connection Speed Change notification size (8 byte header + 8 byte data) */
#define USB_CDC_ECM_ETHERNET_FUNC_DESC_SIZE             13U      /**< Ethernet Networking Functional Descriptor size */
#define USB_CDC_ECM_MAX_SEGMENT_SIZE                    1514U    /**< Maximum Ethernet frame size (default) */

/* MAC Address String Descriptor */
#define USB_CDC_ECM_MAC_STRING_SIZE                     12U      /**< MAC address string length (12 Unicode characters) */

/** @} End of USB_Common_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Common_Types USB Common Types
 * @{
 */

/**
 * @brief CDC Ethernet Networking Functional Descriptor
 */
typedef struct {
	u8  bFunctionLength;        /**< Size of this descriptor in bytes (13) */
	u8  bDescriptorType;        /**< CS_INTERFACE descriptor type */
	u8  bDescriptorSubtype;     /**< Ethernet Networking functional descriptor subtype */
	u8  iMACAddress;            /**< Index of string descriptor containing MAC address */
	u32 bmEthernetStatistics;   /**< Bitmap of supported Ethernet statistics capabilities */
	u16 wMaxSegmentSize;        /**< Maximum segment size device can support */
	u16 wNumberMCFilters;       /**< Number of multicast filters that can be configured */
	u8  bNumberPowerFilters;    /**< Number of pattern filters available for wake-up */
} __PACKED usb_cdc_ethernet_function_desc_t;

/**
 * @brief Connection Speed Change Notification Data
 */
typedef struct {
	u32 DLBitRate;              /**< Downstream bit rate in bits per second */
	u32 ULBitRate;              /**< Upstream bit rate in bits per second */
} __PACKED usb_cdc_ecm_speed_change_data_t;

/**
 * @brief Connection Notification
 */
typedef struct {
	u8  bmRequestType;          /**< Request type */
	u8  bNotificationCode;      /**< NETWORK_CONNECTION or CONNECTION_SPEED_CHANGE */
	u16 wValue;                 /**< Connection status or Reserved (0) */
	u16 wIndex;                 /**< Interface number */
	u16 wLength;                /**< Data length (8) */
	usb_cdc_ecm_speed_change_data_t data; /**< Speed change data */
} __PACKED usb_cdc_ecm_notify_t;


/** @} End of USB_Common_Types group */
/** @} End of USB_Common_API group */

#endif  /* USB_CDC_ECM_H */
