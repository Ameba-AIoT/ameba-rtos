/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_MSC_H
#define USB_MSC_H

/* Includes ------------------------------------------------------------------*/
#include "usb_scsi.h"

/* Exported defines ----------------------------------------------------------*/

/* Defines MSC class-specific request codes for the bRequest field. */
#define USB_MSC_REQUEST_BOT_RESET                  0xFFU    /**< Bulk-Only Mass Storage Reset request. */
#define USB_MSC_REQUEST_GET_MAX_LUN                0xFEU    /**< Get Max LUN request. */

/* CBW/CSW configurations */
#define USB_MSC_CBW_SIGN                           0x43425355U  /**< Standard dCBWSignature, spells out USBC */
#define USB_MSC_CSW_SIGN                           0x53425355U  /**< Standard dCSWSignature, spells out 'USBS' */
#define USB_MSC_CBW_LEN                            31U          /**< Standard CBW length in bytes. */
#define USB_MSC_CSW_LEN                            13U          /**< Standard CSW length in bytes. */
#define USB_BOT_CBW_TAG                            0x20304050U /**< A default tag for CBW, should be unique per command. */

/* Exported types ------------------------------------------------------------*/

/**
 * @brief BOT CSW status enumeration.
 */
typedef enum {
	BOT_CSW_CMD_PASSED = 0x00,   /**< The `pass` status of the command execution */
	BOT_CSW_CMD_FAILED = 0x01,   /**< The `fail` status of the command execution */
	BOT_CSW_PHASE_ERROR = 0x02,  /**< The `phase error` status of the command execution */
} usb_msc_bot_csw_state_t;

/**
 * @brief Command Block Wrapper (CBW) structure.
 * @details This structure defines the format of a command sent from the host to the device.
 */
typedef union {
	struct {
		u32 dCBWSignature;            /**< CBW Signature, must be 'USBC' (0x43425355). */
		u32 dCBWTag;                  /**< A unique tag sent by the host, returned in the CSW. */
		u32 dCBWDataTransferLength;   /**< Number of bytes of data the host expects to transfer. */
		u8 bmCBWFlags;                /**< Data transfer direction (bit 7: 0=OUT, 1=IN). */
		u8 bCBWLUN;                   /**< The Logical Unit Number (LUN) to which the command is addressed. */
		u8 bCBWCBLength;              /**< The length of the command block (CBWCB) in bytes (1-16). */
		u8 CBWCB[16];                 /**< The SCSI command block to be executed by the device. */
	} field;
	u8 data[31];                      /**< Byte array access to the entire CBW structure. */
} usb_msc_bot_cbw_t;

/**
 * @brief Bulk-Only Transport (BOT) Command Status Wrapper (CSW) structure.
 */
typedef union {
	struct {
		u32 dCSWSignature;            /**< CSW Signature, must be 'USBS' (0x53425355). */
		u32 dCSWTag;                  /**< The device shall set this to the value from the associated CBW's dCBWTag. */
		u32 dCSWDataResidue;          /**< The difference between the amount of data expected and the actual data processed. */
		u8 bCSWStatus;                /**< The status of the command execution (0=Pass, 1=Fail, 2=Phase Error), see `USBD_MSC_CSW_XX`. */
	} field;
	u8 data[13];                      /**< Byte array access to the entire CSW structure. */
} usb_msc_bot_csw_t;

/**
 * @brief SCSI Sense Data structure.
 * @details This structure holds sense data used to report errors for SCSI commands.
 */
typedef struct {
	u8 key;                          /**< Sense Key. */
	u8 asc;                          /**< Additional Sense Code. */
	u8 ascq;                         /**< Additional Sense Code Qualifier. */
} usb_msc_scsi_sense_data_t;

#endif // USB_MSC_H
