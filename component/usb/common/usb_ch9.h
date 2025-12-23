/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_CH9_H
#define USB_CH9_H

/* Includes ------------------------------------------------------------------*/

#include "basic_types.h"

#define USB_VERSION_ID_0201                           0x0201 /**< USB Specification version 2.01. */

/**
 * @brief Defines for the standard lengths of various USB descriptors.
 * @{
 */
#define USB_LEN_DEV_QUALIFIER_DESC                    0x0AU  /**< Length of Device Qualifier descriptor. */
#define USB_LEN_DEV_DESC                              0x12U  /**< Length of Device descriptor. */
#define USB_LEN_BOS_DESC                              0x05U  /**< Length of BOS descriptor. */
#define USB_LEN_CFG_DESC                              0x09U  /**< Length of Configuration descriptor. */
#define USB_LEN_IAD_DESC                              0x08U  /**< Length of IAD descriptor. */
#define USB_LEN_IF_DESC                               0x09U  /**< Length of Interface descriptor. */
#define USB_LEN_EP_DESC                               0x07U  /**< Length of Endpoint descriptor. */
#define USB_LEN_OTG_DESC                              0x03U  /**< Length of OTG descriptor. */
#define USB_LEN_LANGID_STR_DESC                       0x04U  /**< Length of Language ID String descriptor. */
#define USB_LEN_OTHER_SPEED_DESC_SIZ                  0x09U  /**< Length of Other Speed Configuration descriptor. */
/** @} */

/**
 * @brief High-byte values for the `wValue` field in a GET_DESCRIPTOR request.
 * @{
 */
#define USB_DESC_DEVICE                               ((USB_DESC_TYPE_DEVICE << 8) & 0xFF00U)
#define USB_DESC_CONFIGURATION                        ((USB_DESC_TYPE_CONFIGURATION << 8) & 0xFF00U)
#define USB_DESC_STRING                               ((USB_DESC_TYPE_STRING << 8) & 0xFF00U)
#define USB_DESC_INTERFACE                            ((USB_DESC_TYPE_INTERFACE << 8) & 0xFF00U)
#define USB_DESC_ENDPOINT                             ((USB_DESC_TYPE_ENDPOINT << 8) & 0xFF00U)
#define USB_DESC_DEVICE_QUALIFIER                     ((USB_DESC_TYPE_DEVICE_QUALIFIER << 8) & 0xFF00U)
#define USB_DESC_OTHER_SPEED_CONFIGURATION            ((USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION << 8) & 0xFF00U)
#define USB_DESC_INTERFACE_POWER                      ((USB_DESC_TYPE_INTERFACE_POWER << 8) & 0xFF00U)
#define USB_DESC_BOS                                  ((USB_DESC_TYPE_BOS << 8) & 0xFF00U)
#define USB_DESC_HUB_DESC                             ((USB_DESC_TYPE_HUB << 8) & 0xFF00U)
/** @} */

/* Device descriptor offset */
#define USB_DEV_DESC_OFFSET_VID                        8U      /**< Offset to idVendor in device descriptor. */
#define USB_DEV_DESC_OFFSET_PID                        10U     /**< Offset to idProduct in device descriptor. */

/* Configuration descriptor offset */
#define USB_CFG_DESC_OFFSET_TYPE                       1U      /**< Offset to bDescriptorType in configuration descriptor. */
#define USB_CFG_DESC_OFFSET_TOTAL_LEN                  2U      /**< Offset to wTotalLength in configuration descriptor. */
#define USB_CFG_DESC_OFFSET_ATTR                       7U      /**< Offset to bmAttributes in configuration descriptor. */
#define USB_CFG_DESC_OFFSET_ATTR_BIT_SELF_POWERED      BIT(6)  /**< Self-powered bit in bmAttributes. */
#define USB_CFG_DESC_OFFSET_ATTR_BIT_REMOTE_WAKEUP     BIT(5)  /**< Remote wakeup bit in bmAttributes. */

/**
 * @brief Defines for the bits within the `bmRequestType` field of a setup request.
 * @{
 */
/* D7 Data Phase Transfer Direction */
#define USB_REQ_DIR_MASK                               0x80U   /**< Mask for data transfer direction bit. */
#define USB_REQ_NUM_MASK                               0x7FU   /**< Mask for endpoint number. */
#define USB_H2D                                        0x00U   /**< Host-to-Device direction. */
#define USB_D2H                                        0x80U   /**< Device-to-Host direction. */

#define USB_EP_IS_IN(ep_addr)	                       (((ep_addr) & USB_REQ_DIR_MASK) == USB_D2H)
#define USB_EP_IS_OUT(ep_addr)	                       (((ep_addr) & USB_REQ_DIR_MASK) == USB_H2D)
#define USB_EP_NUM(ep_addr)		                       ((ep_addr) & USB_REQ_NUM_MASK)

/* D6..5 Type */
#define USB_REQ_TYPE_STANDARD                          0x00U   /**< Standard request type. */
#define USB_REQ_TYPE_CLASS                             0x20U   /**< Class-specific request type. */
#define USB_REQ_TYPE_VENDOR                            0x40U   /**< Vendor-specific request type. */
#define USB_REQ_TYPE_MASK                              0x60U   /**< Mask for request type bits. */

/* D4..0 Recipient */
#define USB_REQ_RECIPIENT_DEVICE                       0x00U   /**< Request recipient is the device. */
#define USB_REQ_RECIPIENT_INTERFACE                    0x01U   /**< Request recipient is an interface. */
#define USB_REQ_RECIPIENT_ENDPOINT                     0x02U   /**< Request recipient is an endpoint. */
#define USB_REQ_RECIPIENT_MASK                         0x03U   /**< Mask for request recipient bits. */
/** @} */

/**
 * @brief Defines for standard USB request codes (USB Spec Table 9-4).
 * @{
 */
#define USB_REQ_GET_STATUS                             0x00U   /**< Get Status request. */
#define USB_REQ_CLEAR_FEATURE                          0x01U   /**< Clear Feature request. */
#define USB_REQ_SET_FEATURE                            0x03U   /**< Set Feature request. */
#define USB_REQ_SET_ADDRESS                            0x05U   /**< Set Address request. */
#define USB_REQ_GET_DESCRIPTOR                         0x06U   /**< Get Descriptor request. */
#define USB_REQ_SET_DESCRIPTOR                         0x07U   /**< Set Descriptor request. */
#define USB_REQ_GET_CONFIGURATION                      0x08U   /**< Get Configuration request. */
#define USB_REQ_SET_CONFIGURATION                      0x09U   /**< Set Configuration request. */
#define USB_REQ_GET_INTERFACE                          0x0AU   /**< Get Interface request. */
#define USB_REQ_SET_INTERFACE                          0x0BU   /**< Set Interface request. */
#define USB_REQ_SYNCH_FRAME                            0x0CU   /**< Synch Frame request. */
/** @} */

/**
 * @brief Defines for standard USB descriptor types (USB Spec Table 9-5).
 * @{
 */
#define USB_DESC_TYPE_DEVICE                           0x01U   /**< Device descriptor type. */
#define USB_DESC_TYPE_CONFIGURATION                    0x02U   /**< Configuration descriptor type. */
#define USB_DESC_TYPE_STRING                           0x03U   /**< String descriptor type. */
#define USB_DESC_TYPE_INTERFACE                        0x04U   /**< Interface descriptor type. */
#define USB_DESC_TYPE_ENDPOINT                         0x05U   /**< Endpoint descriptor type. */
#define USB_DESC_TYPE_DEVICE_QUALIFIER                 0x06U   /**< Device Qualifier descriptor type. */
#define USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION        0x07U   /**< Other Speed Configuration descriptor type. */
#define USB_DESC_TYPE_INTERFACE_POWER                  0x08U   /**< Interface Power descriptor type. */
#define USB_DESC_TYPE_IAD                              0x0BU   /**< Interface Association Descriptor (IAD) type. */
#define USB_DESC_TYPE_BOS                              0x0FU   /**< Binary Device Object Store (BOS) descriptor type. */
#define USB_DESC_TYPE_HUB                              0x29U   /**< Hub descriptor type. */
/** @} */

#define USB_DEVICE_CAPABITY_TYPE                       0x10U   /**< Device Capability descriptor type. */

/**
 * @brief Defines for standard feature selectors used in Set/Clear Feature requests.
 * @{
 */
#define USB_CONFIG_REMOTE_WAKEUP                       0x02U   /**< Configuration-level remote wakeup feature. */
#define USB_CONFIG_SELF_POWERED                        0x01U   /**< Configuration-level self-powered feature. */

#define USB_FEATURE_EP_HALT                            0x00U   /**< Endpoint halt feature selector. */
#define USB_FEATURE_REMOTE_WAKEUP                      0x01U   /**< Device remote wakeup feature selector. */
#define USB_FEATURE_TEST_MODE                          0x02U   /**< Test mode feature selector. */
/** @} */

/**
 * @brief Macros related to endpoint descriptors and addresses.
 * @{
 */
/* bmAttributes D1..0 Transfer type */
#define USB_EP_XFER_TYPE_MASK                          0x03U            /**< Mask to extract transfer type from bmAttributes. */
/* wMaxPacketSize Bit 10..0 Per packet size */
#define USB_EP_MPS_SIZE_MASK                           0x7FFU           /**< Mask for packet size in wMaxPacketSize. */
/* wMaxPacketSize Bit 12..11 Additional transactions per microframe */
#define USB_EP_MPS_TRANS_MASK                          0x1800U          /**< Mask for additional transactions in wMaxPacketSize. */
#define USB_EP_MPS_TRANS_POS                           (11U)            /**< Position of additional transactions field. */
/* EP0 address */
#define USB_EP0_OUT                                    (0U | USB_H2D)   /**< Endpoint 0 OUT address. */
#define USB_EP0_IN                                     (0U | USB_D2H)   /**< Endpoint 0 IN address. */
/** @} */

/**
 * @brief Defines for maximum packet sizes for various speeds and endpoint types.
 * @{
 */
#define USB_HS_MAX_PACKET_SIZE                         512U             /**< High-Speed maximum packet size. */
#define USB_FS_MAX_PACKET_SIZE                         64U              /**< Full-Speed maximum packet size. */
#define USB_MAX_EP0_SIZE                               64U              /**< Maximum packet size for Endpoint 0. */
#define USB_CTRL_MAX_MPS                               64U              /**< Maximum MPS for any Control endpoint. */
#define USB_XFER_MAX_MPS                               1024U            /**< Maximum MPS for any transfer. */
#define USB_BULK_FS_MAX_MPS                            64U              /**< Full-Speed Bulk maximum MPS. */
#define USB_BULK_HS_MAX_MPS                            512U             /**< High-Speed Bulk maximum MPS. */
#define USB_INTR_FS_MAX_MPS                            64U              /**< Full-Speed Interrupt maximum MPS. */
#define USB_INTR_HS_MAX_MPS                            USB_XFER_MAX_MPS /**< High-Speed Interrupt maximum MPS. */
#define USB_ISOC_FS_MAX_MPS                            1023U            /**< Full-Speed Isochronous maximum MPS. */
#define USB_ISOC_HS_MAX_MPS                            USB_XFER_MAX_MPS /**< High-Speed Isochronous maximum MPS. */
/** @} */

/**
 * @brief Defines related to the USB Hub Class specification.
 * @{
 */

#define USB_CLASS_HUB                    0x09U  /**< USB device class type  */

/**
 * @brief Feature selectors for Hub Port Get/Set/Clear Feature requests.
 * @{
 */
#define USB_PORT_FEAT_CONNECTION         0      /**< Port Connection feature. */
#define USB_PORT_FEAT_ENABLE             1      /**< Port Enable feature. */
#define USB_PORT_FEAT_SUSPEND            2      /**< Port Suspend feature. */
#define USB_PORT_FEAT_OVER_CURRENT       3      /**< Port Over-current feature. */
#define USB_PORT_FEAT_RESET              4      /**< Port Reset feature. */
#define USB_PORT_FEAT_POWER              8      /**< Port Power feature. */
#define USB_PORT_FEAT_LOWSPEED           9      /**< Port Low-speed device attached feature. */
#define USB_PORT_FEAT_HIGHSPEED          10     /**< Port High-speed device attached feature. */
#define USB_PORT_FEAT_C_CONNECTION       16     /**< Port Connection change feature. */
#define USB_PORT_FEAT_C_ENABLE           17     /**< Port Enable change feature. */
#define USB_PORT_FEAT_C_SUSPEND          18     /**< Port Suspend change feature. */
#define USB_PORT_FEAT_C_OVER_CURRENT     19     /**< Port Over-current change feature. */
#define USB_PORT_FEAT_C_RESET            20     /**< Port Reset change feature. */
/** @} */

/**
 * @brief Bit definitions for the `wPortStatus` field returned by GetPortStatus.
 * @{
 */
#define USB_PORT_STAT_CONNECTION          0x0001 /**< Device is connected to the port. */
#define USB_PORT_STAT_ENABLE              0x0002 /**< Port is enabled. */
#define USB_PORT_STAT_SUSPEND             0x0004 /**< Port is suspended. */
#define USB_PORT_STAT_OVERCURRENT         0x0008 /**< Port is in an over-current condition. */
#define USB_PORT_STAT_RESET               0x0010 /**< Port is in reset. */
#define USB_PORT_STAT_POWER               0x0100 /**< Port power is on. */
#define USB_PORT_STAT_LOW_SPEED           0x0200 /**< A low-speed device is attached. */
#define USB_PORT_STAT_HIGH_SPEED          0x0400 /**< A high-speed device is attached (EHCI). */
#define USB_PORT_STAT_SUPER_SPEED         0x0600 /**< A super-speed device is attached (XHCI). */
#define USB_PORT_STAT_SPEED_MASK          (USB_PORT_STAT_LOW_SPEED | USB_PORT_STAT_HIGH_SPEED) /**< Mask to get port speed. */
/** @} */

/**
 * @brief Bit definitions for the `wPortChange` field returned by GetPortStatus.
 * @{
 */
#define USB_PORT_STAT_C_CONNECTION         0x0001 /**< Connection status has changed. */
#define USB_PORT_STAT_C_ENABLE             0x0002 /**< Enable status has changed. */
#define USB_PORT_STAT_C_SUSPEND            0x0004 /**< Suspend status has changed. */
#define USB_PORT_STAT_C_OVERCURRENT        0x0008 /**< Over-current status has changed. */
#define USB_PORT_STAT_C_RESET              0x0010 /**< Reset completion has occurred. */
/** @} */
/** @} */ // End of Hub_Defs

/**
 * @brief Defines the transfer type of a USB endpoint.
 * @details Corresponds to the `bmAttributes` field in the endpoint descriptor.
 */
typedef enum {
	USB_CH_EP_TYPE_CTRL = 0U,     /**< Control Endpoint/pipe. */
	USB_CH_EP_TYPE_ISOC,          /**< Isochronous Endpoint/pipe. */
	USB_CH_EP_TYPE_BULK,          /**< Bulk Endpoint/pipe. */
	USB_CH_EP_TYPE_INTR           /**< Interrupt Endpoint/pipe. */
} usb_ch_ep_type_t;

/**
 * @brief Standard USB setup request packet structure.
 * @details This structure represents the 8-byte setup packet sent
 *          during the setup phase of a control transfer. (USB Spec 2.0, Table 9-2)
 */
typedef struct {
	u8 bmRequestType;             /**< Characteristics of the request. */
	u8 bRequest;                  /**< Specific request code. */
	u16 wValue;                   /**< Request-specific value, often used for descriptor type and index. */
	u16 wIndex;                   /**< Request-specific index, often an interface or endpoint number. */
	u16 wLength;                  /**< Number of bytes to transfer in the data phase. */
} __PACKED usb_setup_req_t;

#endif /* USB_CH9_H */

