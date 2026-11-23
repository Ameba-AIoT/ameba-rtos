/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_RNDIS_H
#define USB_RNDIS_H

/* Includes ------------------------------------------------------------------*/

#include "basic_types.h"
#include "usb_cdc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Common_API USB Common API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Common_Constants USB Common Constants
 * @{
 */
/** @defgroup USB_Common_RNDIS_Constants USB RNDIS Constants (RNDIS 1.0)
 *  @brief Remote NDIS definitions shared by the USBD and USBH RNDIS drivers.
 *  @details Ref Microsoft Remote NDIS Specification 1.0 and Remote NDIS USB Bus Specification.
 *           Every multi-byte field on the wire is little endian.
 *  @{
 */

/* Interface triplets of an RNDIS communication interface. The device advertises the
 * Wireless Controller one, which Windows binds its inbox driver to and Linux rndis_host
 * matches; the host also accepts the CDC ACM vendor protocol one of Linux gadgets and the
 * Miscellaneous one of USB IF's RNDIS class code. */
#define USB_RNDIS_WIRELESS_CLASS_CODE                   0xE0U /**< Wireless Controller class */
#define USB_RNDIS_WIRELESS_SUBCLASS                     0x01U /**< RF Controller subclass */
#define USB_RNDIS_WIRELESS_PROTOCOL                     0x03U /**< Remote NDIS protocol */
#define USB_RNDIS_MISC_CLASS_CODE                       0xEFU /**< Miscellaneous class */
#define USB_RNDIS_MISC_SUBCLASS                         0x04U /**< Network Control subclass */
#define USB_RNDIS_MISC_PROTOCOL                         0x01U /**< RNDIS over Ethernet protocol */
#define USB_RNDIS_CDC_SUBCLASS_ACM                      0x02U /**< CDC Abstract Control Model subclass */
#define USB_RNDIS_CDC_PROTOCOL_VENDOR                   0xFFU /**< CDC vendor specific protocol */

/* CDC functional descriptors carried by an RNDIS communication interface */
#define USB_RNDIS_CDC_FUNC_DESC_ACM                     0x02U /**< Abstract Control Management Functional Descriptor */

/* CDC class requests carrying the RNDIS control messages, Ref RNDIS USB 2.2 */
#define USB_RNDIS_SEND_ENCAPSULATED_COMMAND             0x00U /**< Host to device control message */
#define USB_RNDIS_GET_ENCAPSULATED_RESPONSE             0x01U /**< Device to host control message */

/* RESPONSE_AVAILABLE notification on the INTR IN endpoint: two 32-bit words, 1 then 0 */
#define USB_RNDIS_NOTIFY_RESPONSE_AVAILABLE             0x00000001U /**< Notification code */
#define USB_RNDIS_NOTIFY_SIZE                           8U          /**< Notification size in bytes */

/* Message types */
#define USB_RNDIS_MSG_PACKET                            0x00000001U /**< REMOTE_NDIS_PACKET_MSG */
#define USB_RNDIS_MSG_INITIALIZE                        0x00000002U /**< REMOTE_NDIS_INITIALIZE_MSG */
#define USB_RNDIS_MSG_HALT                              0x00000003U /**< REMOTE_NDIS_HALT_MSG */
#define USB_RNDIS_MSG_QUERY                             0x00000004U /**< REMOTE_NDIS_QUERY_MSG */
#define USB_RNDIS_MSG_SET                               0x00000005U /**< REMOTE_NDIS_SET_MSG */
#define USB_RNDIS_MSG_RESET                             0x00000006U /**< REMOTE_NDIS_RESET_MSG */
#define USB_RNDIS_MSG_INDICATE_STATUS                   0x00000007U /**< REMOTE_NDIS_INDICATE_STATUS_MSG */
#define USB_RNDIS_MSG_KEEPALIVE                         0x00000008U /**< REMOTE_NDIS_KEEPALIVE_MSG */
#define USB_RNDIS_MSG_COMPLETION                        0x80000000U /**< Set in the type of every completion message */
#define USB_RNDIS_MSG_INITIALIZE_CMPLT                  (USB_RNDIS_MSG_INITIALIZE | USB_RNDIS_MSG_COMPLETION)
#define USB_RNDIS_MSG_QUERY_CMPLT                       (USB_RNDIS_MSG_QUERY | USB_RNDIS_MSG_COMPLETION)
#define USB_RNDIS_MSG_SET_CMPLT                         (USB_RNDIS_MSG_SET | USB_RNDIS_MSG_COMPLETION)
#define USB_RNDIS_MSG_RESET_CMPLT                       (USB_RNDIS_MSG_RESET | USB_RNDIS_MSG_COMPLETION)
#define USB_RNDIS_MSG_KEEPALIVE_CMPLT                   (USB_RNDIS_MSG_KEEPALIVE | USB_RNDIS_MSG_COMPLETION)

/* Status values */
#define USB_RNDIS_STATUS_SUCCESS                        0x00000000U /**< Success */
#define USB_RNDIS_STATUS_FAILURE                        0xC0000001U /**< Unspecified error */
#define USB_RNDIS_STATUS_INVALID_DATA                   0xC0010015U /**< Invalid data */
#define USB_RNDIS_STATUS_NOT_SUPPORTED                  0xC00000BBU /**< Unsupported request */
#define USB_RNDIS_STATUS_MEDIA_CONNECT                  0x4001000BU /**< Device connected to the network */
#define USB_RNDIS_STATUS_MEDIA_DISCONNECT               0x4001000CU /**< Device disconnected from the network */

/* INITIALIZE fields */
#define USB_RNDIS_MAJOR_VERSION                         1U          /**< Protocol major version */
#define USB_RNDIS_MINOR_VERSION                         0U          /**< Protocol minor version */
#define USB_RNDIS_DF_CONNECTIONLESS                     0x00000001U /**< DeviceFlags: connectionless device */
#define USB_RNDIS_MEDIUM_802_3                          0x00000000U /**< Medium: IEEE 802.3 */

/* General OIDs */
#define USB_RNDIS_OID_GEN_SUPPORTED_LIST                0x00010101U
#define USB_RNDIS_OID_GEN_HARDWARE_STATUS               0x00010102U
#define USB_RNDIS_OID_GEN_MEDIA_SUPPORTED               0x00010103U
#define USB_RNDIS_OID_GEN_MEDIA_IN_USE                  0x00010104U
#define USB_RNDIS_OID_GEN_MAXIMUM_FRAME_SIZE            0x00010106U
#define USB_RNDIS_OID_GEN_LINK_SPEED                    0x00010107U
#define USB_RNDIS_OID_GEN_TRANSMIT_BLOCK_SIZE           0x0001010AU
#define USB_RNDIS_OID_GEN_RECEIVE_BLOCK_SIZE            0x0001010BU
#define USB_RNDIS_OID_GEN_VENDOR_ID                     0x0001010CU
#define USB_RNDIS_OID_GEN_VENDOR_DESCRIPTION            0x0001010DU
#define USB_RNDIS_OID_GEN_CURRENT_PACKET_FILTER         0x0001010EU
#define USB_RNDIS_OID_GEN_MAXIMUM_TOTAL_SIZE            0x00010111U
#define USB_RNDIS_OID_GEN_MEDIA_CONNECT_STATUS          0x00010114U
#define USB_RNDIS_OID_GEN_PHYSICAL_MEDIUM               0x00010202U
#define USB_RNDIS_OID_GEN_XMIT_OK                       0x00020101U
#define USB_RNDIS_OID_GEN_RCV_OK                        0x00020102U
#define USB_RNDIS_OID_GEN_XMIT_ERROR                    0x00020103U
#define USB_RNDIS_OID_GEN_RCV_ERROR                     0x00020104U
#define USB_RNDIS_OID_GEN_RCV_NO_BUFFER                 0x00020105U

/* IEEE 802.3 OIDs */
#define USB_RNDIS_OID_802_3_PERMANENT_ADDRESS           0x01010101U
#define USB_RNDIS_OID_802_3_CURRENT_ADDRESS             0x01010102U
#define USB_RNDIS_OID_802_3_MULTICAST_LIST              0x01010103U
#define USB_RNDIS_OID_802_3_MAXIMUM_LIST_SIZE           0x01010104U
#define USB_RNDIS_OID_802_3_RCV_ERROR_ALIGNMENT         0x01020101U
#define USB_RNDIS_OID_802_3_XMIT_ONE_COLLISION          0x01020102U
#define USB_RNDIS_OID_802_3_XMIT_MORE_COLLISIONS        0x01020103U

/* OID_GEN_MEDIA_CONNECT_STATUS values */
#define USB_RNDIS_MEDIA_STATE_CONNECTED                 0x00000000U
#define USB_RNDIS_MEDIA_STATE_DISCONNECTED              0x00000001U

/* OID_GEN_CURRENT_PACKET_FILTER bits */
#define USB_RNDIS_PACKET_TYPE_DIRECTED                  0x00000001U
#define USB_RNDIS_PACKET_TYPE_MULTICAST                 0x00000002U
#define USB_RNDIS_PACKET_TYPE_ALL_MULTICAST             0x00000004U
#define USB_RNDIS_PACKET_TYPE_BROADCAST                 0x00000008U
#define USB_RNDIS_PACKET_TYPE_PROMISCUOUS               0x00000020U

/* Sizes */
#define USB_RNDIS_MAC_ADDR_LEN                          6U       /**< IEEE 802 MAC address length */
#define USB_RNDIS_MAX_FRAME_SIZE                        1514U    /**< Largest Ethernet frame carried, no FCS */
#define USB_RNDIS_MTU                                   1500U    /**< OID_GEN_MAXIMUM_FRAME_SIZE: frame without its header */
#define USB_RNDIS_PACKET_MSG_HDR_SIZE                   44U      /**< REMOTE_NDIS_PACKET_MSG header */
#define USB_RNDIS_PACKET_DATA_OFFSET                    36U      /**< DataOffset right after the header, counted from the DataOffset field */
#define USB_RNDIS_MAX_PACKET_MSG_SIZE                   (USB_RNDIS_PACKET_MSG_HDR_SIZE + USB_RNDIS_MAX_FRAME_SIZE)
#define USB_RNDIS_INFO_BUF_OFFSET_BASE                  8U       /**< InformationBufferOffset/StatusBufferOffset count from RequestId */

/** @} End of USB_Common_RNDIS_Constants group */
/** @} End of USB_Common_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Common_Types USB Common Types
 * @{
 */
/** @defgroup USB_Common_RNDIS_Types USB RNDIS Types (RNDIS 1.0)
 *  @brief Remote NDIS messages shared by the USBD and USBH RNDIS drivers.
 *  @details Byte packed: a message may sit at any offset of a transfer buffer.
 *  @{
 */

/**
 * @brief Header common to every RNDIS message.
 */
typedef struct {
	u32 MessageType;              /**< USB_RNDIS_MSG_xxx */
	u32 MessageLength;            /**< Length of the whole message in bytes */
} __PACKED usb_rndis_msg_hdr_t;

/**
 * @brief REMOTE_NDIS_INITIALIZE_MSG.
 */
typedef struct {
	u32 MessageType;              /**< USB_RNDIS_MSG_INITIALIZE */
	u32 MessageLength;            /**< 24 */
	u32 RequestId;                /**< Echoed in the completion */
	u32 MajorVersion;             /**< Protocol major version of the host */
	u32 MinorVersion;             /**< Protocol minor version of the host */
	u32 MaxTransferSize;          /**< Largest transfer the host accepts from the device */
} __PACKED usb_rndis_initialize_msg_t;

/**
 * @brief REMOTE_NDIS_INITIALIZE_CMPLT.
 */
typedef struct {
	u32 MessageType;              /**< USB_RNDIS_MSG_INITIALIZE_CMPLT */
	u32 MessageLength;            /**< 52 */
	u32 RequestId;                /**< RequestId of the request */
	u32 Status;                   /**< USB_RNDIS_STATUS_xxx */
	u32 MajorVersion;             /**< Protocol major version of the device */
	u32 MinorVersion;             /**< Protocol minor version of the device */
	u32 DeviceFlags;              /**< USB_RNDIS_DF_CONNECTIONLESS */
	u32 Medium;                   /**< USB_RNDIS_MEDIUM_802_3 */
	u32 MaxPacketsPerTransfer;    /**< Largest number of packets the device accepts in one transfer */
	u32 MaxTransferSize;          /**< Largest transfer the device accepts from the host */
	u32 PacketAlignmentFactor;    /**< Packet alignment in one transfer, as a power of two */
	u32 AFListOffset;             /**< Reserved for connection-oriented devices, 0 */
	u32 AFListSize;               /**< Reserved for connection-oriented devices, 0 */
} __PACKED usb_rndis_initialize_cmplt_t;

/**
 * @brief REMOTE_NDIS_QUERY_MSG and REMOTE_NDIS_SET_MSG, followed by the information buffer.
 */
typedef struct {
	u32 MessageType;              /**< USB_RNDIS_MSG_QUERY or USB_RNDIS_MSG_SET */
	u32 MessageLength;            /**< 28 plus the information buffer */
	u32 RequestId;                /**< Echoed in the completion */
	u32 Oid;                      /**< Object identifier */
	u32 InformationBufferLength;  /**< Length of the information buffer */
	u32 InformationBufferOffset;  /**< Offset of the information buffer, counted from RequestId */
	u32 DeviceVcHandle;           /**< Reserved, 0 */
} __PACKED usb_rndis_query_msg_t;

/**
 * @brief REMOTE_NDIS_QUERY_CMPLT, followed by the information buffer.
 */
typedef struct {
	u32 MessageType;              /**< USB_RNDIS_MSG_QUERY_CMPLT */
	u32 MessageLength;            /**< 24 plus the information buffer */
	u32 RequestId;                /**< RequestId of the request */
	u32 Status;                   /**< USB_RNDIS_STATUS_xxx */
	u32 InformationBufferLength;  /**< Length of the information buffer */
	u32 InformationBufferOffset;  /**< Offset of the information buffer, counted from RequestId, 0 if none */
} __PACKED usb_rndis_query_cmplt_t;

/**
 * @brief Completion carrying a status only: REMOTE_NDIS_SET_CMPLT and REMOTE_NDIS_KEEPALIVE_CMPLT.
 */
typedef struct {
	u32 MessageType;              /**< USB_RNDIS_MSG_SET_CMPLT or USB_RNDIS_MSG_KEEPALIVE_CMPLT */
	u32 MessageLength;            /**< 16 */
	u32 RequestId;                /**< RequestId of the request */
	u32 Status;                   /**< USB_RNDIS_STATUS_xxx */
} __PACKED usb_rndis_status_cmplt_t;

/**
 * @brief Request carrying a RequestId only: REMOTE_NDIS_HALT_MSG and REMOTE_NDIS_KEEPALIVE_MSG.
 */
typedef struct {
	u32 MessageType;              /**< USB_RNDIS_MSG_HALT or USB_RNDIS_MSG_KEEPALIVE */
	u32 MessageLength;            /**< 12 */
	u32 RequestId;                /**< RequestId, echoed by a keepalive completion */
} __PACKED usb_rndis_request_msg_t;

/**
 * @brief REMOTE_NDIS_RESET_CMPLT.
 */
typedef struct {
	u32 MessageType;              /**< USB_RNDIS_MSG_RESET_CMPLT */
	u32 MessageLength;            /**< 16 */
	u32 Status;                   /**< USB_RNDIS_STATUS_xxx */
	u32 AddressingReset;          /**< 1: the host shall send its multicast list again */
} __PACKED usb_rndis_reset_cmplt_t;

/**
 * @brief REMOTE_NDIS_INDICATE_STATUS_MSG, followed by the status buffer.
 */
typedef struct {
	u32 MessageType;              /**< USB_RNDIS_MSG_INDICATE_STATUS */
	u32 MessageLength;            /**< 20 plus the status buffer */
	u32 Status;                   /**< USB_RNDIS_STATUS_MEDIA_CONNECT, ... */
	u32 StatusBufferLength;       /**< Length of the status buffer */
	u32 StatusBufferOffset;       /**< Offset of the status buffer, counted from Status, 0 if none */
} __PACKED usb_rndis_indicate_status_msg_t;

/**
 * @brief REMOTE_NDIS_PACKET_MSG header, followed by the Ethernet frame.
 */
typedef struct {
	u32 MessageType;              /**< USB_RNDIS_MSG_PACKET */
	u32 MessageLength;            /**< Whole message, header and frame */
	u32 DataOffset;               /**< Offset of the frame, counted from this field */
	u32 DataLength;               /**< Frame length in bytes */
	u32 OOBDataOffset;            /**< Out of band data, unused, 0 */
	u32 OOBDataLength;            /**< Out of band data, unused, 0 */
	u32 NumOOBDataElements;       /**< Out of band data, unused, 0 */
	u32 PerPacketInfoOffset;      /**< Per packet information, unused, 0 */
	u32 PerPacketInfoLength;      /**< Per packet information, unused, 0 */
	u32 VcHandle;                 /**< Reserved, 0 */
	u32 Reserved;                 /**< Reserved, 0 */
} __PACKED usb_rndis_packet_msg_t;

/** @} End of USB_Common_RNDIS_Types group */
/** @} End of USB_Common_Types group */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @} End of USB_Common_API group */

#ifdef __cplusplus
}
#endif

#endif /* USB_RNDIS_H */
