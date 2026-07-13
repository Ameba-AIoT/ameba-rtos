/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_DFU_H
#define USB_DFU_H

/* Includes ------------------------------------------------------------------*/
#include "basic_types.h"

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Common_API USB Common API
 *  @{
 */
/** @addtogroup USB_Common_Constants USB Common Constants
 *  @{
 */

/** @defgroup USB_DFU_Constants DFU Protocol Constants (USB DFU 1.1)
 *  @brief Common protocol definitions shared by USBD and USBH DFU drivers.
 *  @{
 */

/* DFU class/subclass/protocol codes ----------------------------------------*/
#define USB_DFU_CLASS_CODE                  0xFEU   /**< Application-Specific class code */
#define USB_DFU_SUBCLASS_CODE               0x01U   /**< DFU subclass code */
#define USB_DFU_PROTOCOL_RUNTIME            0x01U   /**< Run-Time protocol (application running) */
#define USB_DFU_PROTOCOL_DFU                0x02U   /**< DFU mode protocol */

/* DFU Functional Descriptor ------------------------------------------------*/
#define USB_DFU_FUNC_DESC_TYPE              0x21U   /**< DFU Functional Descriptor type */
#define USB_DFU_FUNC_DESC_SIZE              9U      /**< DFU Functional Descriptor size (bytes) */

/* bmAttributes bit definitions (DFU 1.1 §4.1.3) ----------------------------*/
#define USB_DFU_ATTR_CAN_DNLOAD             BIT(0)  /**< Device supports DFU_DNLOAD */
#define USB_DFU_ATTR_CAN_UPLOAD             BIT(1)  /**< Device supports DFU_UPLOAD */
#define USB_DFU_ATTR_MANIFEST_TOL           BIT(2)  /**< Device is manifestation tolerant */
#define USB_DFU_ATTR_WILL_DETACH            BIT(3)  /**< Device will detach after DFU_DETACH */

/* DFU request codes (DFU 1.1 spec, Table 3) --------------------------------*/
#define USB_DFU_REQ_DETACH                  0x00U   /**< DFU_DETACH request */
#define USB_DFU_REQ_DNLOAD                  0x01U   /**< DFU_DNLOAD request */
#define USB_DFU_REQ_UPLOAD                  0x02U   /**< DFU_UPLOAD request */
#define USB_DFU_REQ_GETSTATUS               0x03U   /**< DFU_GETSTATUS request */
#define USB_DFU_REQ_CLRSTATUS               0x04U   /**< DFU_CLRSTATUS request */
#define USB_DFU_REQ_GETSTATE                0x05U   /**< DFU_GETSTATE request */
#define USB_DFU_REQ_ABORT                   0x06U   /**< DFU_ABORT request */

/* DFU GETSTATUS response packet size ---------------------------------------*/
#define USB_DFU_STATUS_PKT_SIZE             6U      /**< GETSTATUS response is always 6 bytes */

/* DFU version (DFU 1.1 = 0x0110) -------------------------------------------*/
#define USB_DFU_BCD_VERSION                 0x0110U /**< BCD-encoded DFU specification version */

/* DFU device state values (DFU 1.1 spec, Table 3-1) ------------------------*/
/** @brief DFU device states (DFU 1.1 spec Table 3-1) */
typedef enum {
	USB_DFU_STATE_APP_IDLE               = 0U,  /**< Run-Time mode, waiting for DFU_DETACH */
	USB_DFU_STATE_APP_DETACH             = 1U,  /**< DFU_DETACH received, waiting for USB reset */
	USB_DFU_STATE_DFU_IDLE               = 2U,  /**< DFU mode, idle */
	USB_DFU_STATE_DNLOAD_SYNC            = 3U,  /**< Waiting for DNLOAD data out */
	USB_DFU_STATE_DNBUSY                 = 4U,  /**< Device busy programming */
	USB_DFU_STATE_DNLOAD_IDLE            = 5U,  /**< Ready for next DNLOAD block */
	USB_DFU_STATE_MANIFEST_SYNC          = 6U,  /**< Waiting for GETSTATUS after manifest trigger */
	USB_DFU_STATE_MANIFEST               = 7U,  /**< Manifestation in progress */
	USB_DFU_STATE_MANIFEST_WAIT_RESET    = 8U,  /**< Waiting for USB reset after manifest */
	USB_DFU_STATE_UPLOAD_IDLE            = 9U,  /**< Ready for next UPLOAD block */
	USB_DFU_STATE_ERROR                  = 10U, /**< Error state */
} usb_dfu_state_t;

/** @} End of USB_DFU_Constants group */
/** @} End of USB_Common_Constants group */

/** @addtogroup USB_Common_Types USB Common Types
 *  @{
 */

/**
 * @brief DFU Functional Descriptor (9 bytes, bDescriptorType=0x21).
 *        Matches the USB DFU 1.1 spec §4.1.3 descriptor layout.
 */
typedef struct {
	u8  bLength;           /**< Descriptor length, always 9. */
	u8  bDescriptorType;   /**< Descriptor type, always 0x21. */
	u8  bmAttributes;      /**< DFU capabilities (bitmap). */
	u16 wDetachTimeOut;    /**< Time in ms the device will wait for USB reset after DFU_DETACH. */
	u16 wTransferSize;     /**< Maximum number of bytes per DNLOAD/UPLOAD block. */
	u16 bcdDFUVersion;     /**< BCD-encoded DFU specification release number. */
} __PACKED usb_dfu_func_desc_t;

/** @} End of USB_Common_Types group */
/** @} End of USB_Common_API group */

#endif /* USB_DFU_H */
