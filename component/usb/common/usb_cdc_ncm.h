/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_CDC_NCM_H
#define USB_CDC_NCM_H

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
#define USB_CDC_NCM_CLASS_CODE                              0x02U /**< USB Communication Device Class (CDC) Code */
#define USB_CDC_NCM_COMM_INTERFACE_CLASS_CODE               0x02U /**< CDC Communication Interface Class Code */
#define USB_CDC_NCM_DATA_INTERFACE_CLASS_CODE               0x0AU /**< CDC Data Interface Class Code */

/* CDC Communication Subclass Codes */
#define USB_CDC_NCM_SUBCLASS_RESERVED                       0x00U /**< CDC Subclass Code: Reserved */

/* CDC Communication Interface Class Control Protocol Codes */
#define USB_CDC_NCM_CTRL_PROTOCOL_NO_CLASS_SPECIFIC         0x00U  /**< CDC Protocol Code: No class specific protocol */
#define USB_CDC_NCM_CTRL_PROTOCOL_VENDOR_SPECIFIC           0xFFU  /**< CDC Protocol Code: Vendor specific */

/* Data Interface Class Protocol Codes */
#define USB_CDC_NCM_DATA_PROTOCOL_NO_CLASS_SPECIFIC         0x00U  /**< CDC Data Protocol: No class specific protocol */
#define USB_CDC_NCM_DATA_PROTOCOL_NETWORK_TRANSFER_BLOCK    0x01U  /**< CDC Data Protocol: Network Transfer Block */

/* CDC Functional Descriptor Types */
#define USB_CDC_NCM_CS_INTERFACE                            0x24U /**< Class-Specific Interface Descriptor Type */
#define USB_CDC_NCM_CS_ENDPOINT                             0x25U /**< Class-Specific Endpoint Descriptor Type */

/* CDC Functional Descriptor Subtypes */
#define USB_CDC_NCM_FUNC_DESC_HEADER                        0x00U /**< Header Functional Descriptor */
#define USB_CDC_NCM_FUNC_DESC_UNION                         0x06U /**< Union Functional Descriptor */
#define USB_CDC_NCM_FUNC_DESC_ETHERNET_NETWORKING           0x0FU /**< Ethernet Networking Functional Descriptor */

/* CDC NCM Subclass Code */
#define USB_CDC_NCM_SUBCLASS_CODE                       0x0DU /**< CDC Subclass Code: Network Control Model (NCM) */

/* NCM Functional Descriptor Subtypes */
#define USB_CDC_NCM_FUNC_DESC                           0x1AU /**< NCM Functional Descriptor */

/* NCM Class-Specific Request Codes */
#define USB_CDC_NCM_GET_NTB_PARAMETERS                  0x80U /**< Get NTB parameters */
#define USB_CDC_NCM_GET_NET_ADDRESS                     0x81U /**< Get network address (obsolete) */
#define USB_CDC_NCM_SET_NET_ADDRESS                     0x82U /**< Set network address (obsolete) */
#define USB_CDC_NCM_GET_NTB_FORMAT                      0x83U /**< Get NTB format (16-bit vs 32-bit) */
#define USB_CDC_NCM_SET_NTB_FORMAT                      0x84U /**< Set NTB format */
#define USB_CDC_NCM_GET_NTB_INPUT_SIZE                  0x85U /**< Get max NTB input size */
#define USB_CDC_NCM_SET_NTB_INPUT_SIZE                  0x86U /**< Set max NTB input size */
#define USB_CDC_NCM_GET_MAX_DATAGRAM_SIZE               0x87U /**< Get max datagram size */
#define USB_CDC_NCM_SET_MAX_DATAGRAM_SIZE               0x88U /**< Set max datagram size */
#define USB_CDC_NCM_GET_CRC_MODE                        0x89U /**< Get CRC mode */
#define USB_CDC_NCM_SET_CRC_MODE                        0x8AU /**< Set CRC mode */

/* NCM Notifications */
#define USB_CDC_NCM_NOTIFY_NETWORK_CONNECTION           0x00U /**< Network Connection Notification */
#define USB_CDC_NCM_NOTIFY_RESPONSE_AVAILABLE           0x01U /**< Response Available Notification */
#define USB_CDC_NCM_NOTIFY_CONNECTION_SPEED_CHANGE      0x2AU /**< Connection Speed Change Notification */

/* NCM NTB Format Signatures */
#define USB_CDC_NCM_NTH16_SIGNATURE                     0x484D434EU /**< "NCMH" - NTB16 header signature */
#define USB_CDC_NCM_NDP16_NOCRC_SIGNATURE               0x304D434EU /**< "NCM0" - NTB16 datagram pointer signature (no CRC) */
#define USB_CDC_NCM_NDP16_CRC_SIGNATURE                 0x314D434EU /**< "NCM1" - NTB16 datagram pointer signature (CRC) */
#define USB_CDC_NCM_NTH32_SIGNATURE                     0x686D636EU /**< "ncmh" - NTB32 header signature */
#define USB_CDC_NCM_NDP32_NOCRC_SIGNATURE               0x306D636EU /**< "ncm0" - NTB32 datagram pointer signature (no CRC) */
#define USB_CDC_NCM_NDP32_CRC_SIGNATURE                 0x316D636EU /**< "ncm1" - NTB32 datagram pointer signature (CRC) */

/* NCM CRC Modes */
#define USB_CDC_NCM_CRC_MODE_NONE                       0x00U /**< No CRC */
#define USB_CDC_NCM_CRC_MODE_CRC32                      0x01U /**< CRC32 */

/* NCM NTB Format Flags */
#define USB_CDC_NCM_NTB16_SUPPORTED                     BIT0  /**< NTB16 format supported */
#define USB_CDC_NCM_NTB32_SUPPORTED                     BIT1  /**< NTB32 format supported */

/* NCM Default Values */
#define USB_CDC_NCM_DEFAULT_NTB_INPUT_SIZE              4096U /**< Default NTB input size */
#define USB_CDC_NCM_MAX_NTB_INPUT_SIZE                  65535U /**< Maximum NTB input size (16-bit limit) */
#define USB_CDC_NCM_NTH16_LENGTH                        12U   /**< NTH16 header length */
#define USB_CDC_NCM_NDP16_MIN_LENGTH                    16U   /**< Minimum NDP16 length (2 datagram entries + terminator) */
#define USB_CDC_NCM_NDP16_ENTRY_LENGTH                  4U    /**< Each NDP16 datagram entry length */
#define USB_CDC_NCM_DATAGRAM_ALIGN                      4U    /**< Default datagram alignment */
#define USB_CDC_NCM_MAX_DATAGRAMS_DEFAULT               1U    /**< Default max datagrams per NTB */

/* NCM Ethernet Frame Size */
#define USB_CDC_NCM_MAX_ETHERNET_FRAME_SIZE             1514U /**< Maximum Ethernet frame size */

/** @} End of USB_Common_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Common_Types USB Common Types
 *  @{
 */

/**
 * @brief CDC Ethernet Networking Functional Descriptor
 *        (used by NCM for MAC address extraction)
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
} __PACKED usb_cdc_ncm_ethernet_function_desc_t;

/**
 * @brief Connection Speed Change Notification Data
 */
typedef struct {
	u32 DLBitRate;              /**< Downstream bit rate in bits per second */
	u32 ULBitRate;              /**< Upstream bit rate in bits per second */
} __PACKED usb_cdc_ncm_speed_change_data_t;

/**
 * @brief NCM Connection Notification
 */
typedef struct {
	u8  bmRequestType;          /**< Request type */
	u8  bNotificationCode;      /**< Notification code */
	u16 wValue;                 /**< Connection status */
	u16 wIndex;                 /**< Interface number */
	u16 wLength;                /**< Data length */
} __PACKED usb_cdc_ncm_notify_t;

/**
 * @brief CDC NCM Functional Descriptor
 */
typedef struct {
	u8  bFunctionLength;        /**< Size of this descriptor in bytes */
	u8  bDescriptorType;        /**< CS_INTERFACE descriptor type (0x24) */
	u8  bDescriptorSubtype;     /**< NCM functional descriptor subtype (0x1A) */
	u16 bcdNcmVersion;          /**< NCM specification version (BCD) */
	u8  bmNetworkCapabilities;  /**< Bitmap of network capabilities */
} __PACKED usb_cdc_ncm_function_desc_t;

/**
 * @brief NCM NTB Parameters structure (returned by GET_NTB_PARAMETERS)
 */
typedef struct {
	u16 wLength;                 /**< Size of this structure in bytes (28) */
	u16 bmNtbFormatsSupported;   /**< Bitmap of supported NTB formats (bit0=NTB16, bit1=NTB32) */
	u32 dwNtbInMaxSize;          /**< Maximum NTB size for host-to-device direction */
	u16 wNdbInDivisor;           /**< Divisor for datagram alignment in host-to-device direction */
	u16 wNdbInPayloadRemainder;  /**< Required payload remainder in host-to-device direction */
	u16 wNdbInAlignment;         /**< Alignment for NDP in host-to-device direction */
	u16 wReserved1;              /**< Reserved (offset 14, set to 0) per NCM 1.0 Table 6-3 */
	u32 dwNtbOutMaxSize;         /**< Maximum NTB size for device-to-host direction */
	u16 wNdbOutDivisor;         /**< Divisor for datagram alignment in device-to-host direction */
	u16 wNdbOutPayloadRemainder; /**< Required payload remainder in device-to-host direction */
	u16 wNdbOutAlignment;       /**< Alignment for NDP in device-to-host direction */
	u16 wNtbOutMaxDatagrams;    /**< Maximum number of datagrams per NTB in device-to-host direction */
} __PACKED usb_cdc_ncm_ntb_parameters_t;

/**
 * @brief NTB16 Transfer Header
 */
typedef struct {
	u32 dwSignature;            /**< Signature: "NCMH" (0x484D434E) */
	u16 wHeaderLength;          /**< Header length (12 bytes) */
	u16 wSequence;              /**< Sequence number */
	u16 wBlockLength;           /**< Total NTB block length */
	u16 wFpIndex;               /**< Frame pointer index (offset to NDP from NTH start, 0 if none) */
} __PACKED usb_cdc_ncm_nth16_t;

/**
 * @brief NTB16 Datagram Pointer Entry
 */
typedef struct {
	u16 wDatagramIndex;         /**< Offset to datagram from NTH start */
	u16 wDatagramLength;        /**< Length of datagram */
} __PACKED usb_cdc_ncm_ndp16_entry_t;

/**
 * @brief NTB16 Datagram Pointer (NDP)
 */
typedef struct {
	u32 dwSignature;            /**< Signature: "NCM0" (0x304D434E) */
	u16 wLength;                /**< Length of this NDP including all entries */
	u16 wNextFpIndex;           /**< Offset to next NDP (0 if last) */
	usb_cdc_ncm_ndp16_entry_t aEntry[2]; /**< First 2 entries (one real + terminator with index=0) */
} __PACKED usb_cdc_ncm_ndp16_t;


/** @} End of USB_Common_Types group */
/** @} End of USB_Common_API group */

#endif  /* USB_CDC_NCM_H */
