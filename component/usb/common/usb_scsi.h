/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_SCSI_H
#define USB_SCSI_H

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
/** @defgroup USB_Common_SCSI_Constants USB SCSI Constants (SPC)
 *  @brief SCSI opcodes, data lengths and sense codes shared by the USBD and USBH MSC drivers.
 *  @{
 */

/* SCSI Command */
#define SCSI_TEST_UNIT_READY                        0x00U /**< TEST UNIT READY opcode (SPC-3). */
#define SCSI_REQUEST_SENSE                          0x03U /**< REQUEST SENSE opcode (SPC-3). */
#define SCSI_FORMAT_UNIT                            0x04U /**< FORMAT UNIT opcode (SBC-2). */
#define SCSI_INQUIRY                                0x12U /**< INQUIRY opcode (SPC-3). */
#define SCSI_START_STOP_UNIT                        0x1BU /**< START STOP UNIT opcode (SBC-2). */
#define SCSI_MODE_SELECT6                           0x15U /**< MODE SELECT(6) opcode (SPC-3). */
#define SCSI_MODE_SELECT10                          0x55U /**< MODE SELECT(10) opcode (SPC-3). */
#define SCSI_MODE_SENSE6                            0x1AU /**< MODE SENSE(6) opcode (SPC-3). */
#define SCSI_MODE_SENSE10                           0x5AU /**< MODE SENSE(10) opcode (SPC-3). */
#define SCSI_ALLOW_MEDIUM_REMOVAL                   0x1EU /**< PREVENT ALLOW MEDIUM REMOVAL opcode (SPC-3). */
#define SCSI_READ6                                  0x08U /**< READ(6) opcode (SBC-2). */
#define SCSI_READ10                                 0x28U /**< READ(10) opcode (SBC-2). */
#define SCSI_READ12                                 0xA8U /**< READ(12) opcode (SBC-2). */
#define SCSI_READ16                                 0x88U /**< READ(16) opcode (SBC-2). */
#define SCSI_READ_CAPACITY10                        0x25U /**< READ CAPACITY(10) opcode (SBC-2). */
#define SCSI_READ_CAPACITY16                        0x9EU /**< SERVICE ACTION IN(16) opcode, carrying READ CAPACITY(16) (SBC-2). */
#define SCSI_WRITE6                                 0x0AU /**< WRITE(6) opcode (SBC-2). */
#define SCSI_WRITE10                                0x2AU /**< WRITE(10) opcode (SBC-2). */
#define SCSI_WRITE12                                0xAAU /**< WRITE(12) opcode (SBC-2). */
#define SCSI_WRITE16                                0x8AU /**< WRITE(16) opcode (SBC-2). */
#define SCSI_VERIFY10                               0x2FU /**< VERIFY(10) opcode (SBC-2). */
#define SCSI_VERIFY12                               0xAFU /**< VERIFY(12) opcode (SBC-2). */
#define SCSI_VERIFY16                               0x8FU /**< VERIFY(16) opcode (SBC-2). */
#define SCSI_SEND_DIAGNOSTIC                        0x1DU /**< SEND DIAGNOSTIC opcode (SPC-3). */
#define SCSI_READ_FORMAT_CAPACITIES                 0x23U /**< READ FORMAT CAPACITIES opcode (MMC-2, used by UFI hosts). */

/* SCSI Command Data Length */
#define TEST_UNIT_READY_LEN                         0U    /**< TEST UNIT READY carries no data stage. */
#define READ_CAPACITY10_DATA_LEN                    8U    /**< READ CAPACITY(10) response length: last LBA + block length. */
#define INQUIRY_DATA_LEN                            36U   /**< Standard INQUIRY response length requested by this stack. */
#define REQUEST_SENSE_DATA_LEN                      18U   /**< Fixed-format sense data length. */
#define READ_FORMAT_CAPACITY_DATA_LEN               12U   /**< READ FORMAT CAPACITIES response length: list header + one descriptor. */
#define MODE_SENSE10_DATA_LEN                       8U    /**< MODE SENSE(10) response length: mode parameter header only. */
#define MODE_SENSE6_DATA_LEN                        4U    /**< MODE SENSE(6) response length: mode parameter header only. */
#define PAGE00_INQUIRY_DATA_LEN                     6U    /**< INQUIRY VPD page 00h (supported pages) response length. */
#define PAGE80_INQUIRY_DATA_LEN                     8U    /**< INQUIRY VPD page 80h (unit serial number) response length. */

#define CBW_CB_LENGTH                               16U   /**< Size of the CBWCB field of a CBW (BOT 5.1). */
#define CBW_LENGTH                                  10U   /**< bCBWCBLength used for 10-byte command blocks. */

/* SCSI Sense key Code */
#define SCSI_SENSE_KEY_NO_SENSE                     0x00U /**< Sense key: no specific sense information. */
#define SCSI_SENSE_KEY_RECOVERED_ERROR              0x01U /**< Sense key: command completed after recovery. */
#define SCSI_SENSE_KEY_NOT_READY                    0x02U /**< Sense key: logical unit not accessible. */
#define SCSI_SENSE_KEY_MEDIUM_ERROR                 0x03U /**< Sense key: non-recoverable medium error. */
#define SCSI_SENSE_KEY_HARDWARE_ERROR               0x04U /**< Sense key: non-recoverable hardware failure. */
#define SCSI_SENSE_KEY_ILLEGAL_REQUEST              0x05U /**< Sense key: illegal parameter in the command. */
#define SCSI_SENSE_KEY_UNIT_ATTENTION               0x06U /**< Sense key: unit attention, e.g. medium changed or reset. */
#define SCSI_SENSE_KEY_DATA_PROTECT                 0x07U /**< Sense key: access to a protected block. */
#define SCSI_SENSE_KEY_BLANK_CHECK                  0x08U /**< Sense key: blank or end-of-data condition. */
#define SCSI_SENSE_KEY_VENDOR_SPECIFIC              0x09U /**< Sense key: vendor specific condition. */
#define SCSI_SENSE_KEY_COPY_ABORTED                 0x0AU /**< Sense key: copy command aborted. */
#define SCSI_SENSE_KEY_ABORTED_COMMAND              0x0BU /**< Sense key: command aborted by the device. */
#define SCSI_SENSE_KEY_VOLUME_OVERFLOW              0x0DU /**< Sense key: end of medium reached with data left. */
#define SCSI_SENSE_KEY_MISCOMPARE                   0x0EU /**< Sense key: source data did not match medium data. */

/* SCSI ASC Code */
#define SCSI_ASC_NO_ADDITIONAL_SENSE_INFORMATION    0x00U /**< ASC: no additional sense information. */
#define SCSI_ASC_WRITE_FAULT                        0x03U /**< ASC: peripheral device write fault. */
#define SCSI_ASC_LOGICAL_UNIT_NOT_READY             0x04U /**< ASC: logical unit not ready. */
#define SCSI_ASC_UNRECOVERED_READ_ERROR             0x11U /**< ASC: unrecovered read error. */
#define SCSI_ASC_PARAMETER_LIST_LENGTH_ERROR        0x1AU /**< ASC: parameter list length error. */
#define SCSI_ASC_INVALID_COMMAND_OPERATION_CODE     0x20U /**< ASC: invalid command operation code. */
#define SCSI_ASC_ADDRESS_OUT_OF_RANGE               0x21U /**< ASC: logical block address out of range. */
#define SCSI_ASC_INVALID_FIELD_IN_CDB               0x24U /**< ASC: invalid field in CDB. */
#define SCSI_ASC_INVALID_FIELD_IN_PARAMETER_LIST    0x26U /**< ASC: invalid field in parameter list. */
#define SCSI_ASC_WRITE_PROTECTED                    0x27U /**< ASC: write protected. */
#define SCSI_ASC_MEDIUM_HAVE_CHANGED                0x28U /**< ASC: not ready to ready change, medium may have changed. */
#define SCSI_ASC_NOT_READY_TO_READY_CHANGE          0x28U /**< ASC: not ready to ready change (same code as SCSI_ASC_MEDIUM_HAVE_CHANGED). */
#define SCSI_ASC_FORMAT_ERROR                       0x31U /**< ASC: medium format corrupted. */
#define SCSI_ASC_MEDIUM_NOT_PRESENT                 0x3AU /**< ASC: medium not present. */
#define SCSI_ASC_MEDIUM_REMOVAL_PREVENTED           0x53U /**< ASC: medium removal prevented, with @ref SCSI_ASCQ_MEDIUM_REMOVAL_PREVENTED. */
#define SCSI_ASCQ_MEDIUM_REMOVAL_PREVENTED          0x02U /**< ASCQ paired with @ref SCSI_ASC_MEDIUM_REMOVAL_PREVENTED. */

/* SCSI ASCQ */
#define SCSI_ASCQ_FORMAT_COMMAND_FAILED             0x01  /**< ASCQ with ASC 0x31: format command failed. */
#define SCSI_ASCQ_INITIALIZING_COMMAND_REQUIRED     0x02  /**< ASCQ with ASC 0x04: initializing command required. */
#define SCSI_ASCQ_OPERATION_IN_PROGRESS             0x07  /**< ASCQ with ASC 0x04: operation in progress. */

/** @} End of USB_Common_SCSI_Constants group */
/** @} End of USB_Common_Constants group */

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @} End of USB_Common_API group */

#ifdef __cplusplus
}
#endif

#endif /* USB_SCSI_H */
