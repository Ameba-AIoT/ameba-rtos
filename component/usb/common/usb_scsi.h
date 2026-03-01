/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_SCSI_H
#define USB_SCSI_H

/* Includes ------------------------------------------------------------------*/

/* Exported defines ----------------------------------------------------------*/

/* SCSI Command */
#define SCSI_TEST_UNIT_READY                        0x00U
#define SCSI_REQUEST_SENSE                          0x03U
#define SCSI_FORMAT_UNIT                            0x04U
#define SCSI_INQUIRY                                0x12U
#define SCSI_START_STOP_UNIT                        0x1BU
#define SCSI_MODE_SELECT6                           0x15U
#define SCSI_MODE_SELECT10                          0x55U
#define SCSI_MODE_SENSE6                            0x1AU
#define SCSI_MODE_SENSE10                           0x5AU
#define SCSI_ALLOW_MEDIUM_REMOVAL                   0x1EU
#define SCSI_READ6                                  0x08U
#define SCSI_READ10                                 0x28U
#define SCSI_READ12                                 0xA8U
#define SCSI_READ16                                 0x88U
#define SCSI_READ_CAPACITY10                        0x25U
#define SCSI_READ_CAPACITY16                        0x9EU
#define SCSI_WRITE6                                 0x0AU
#define SCSI_WRITE10                                0x2AU
#define SCSI_WRITE12                                0xAAU
#define SCSI_WRITE16                                0x8AU
#define SCSI_VERIFY10                               0x2FU
#define SCSI_VERIFY12                               0xAFU
#define SCSI_VERIFY16                               0x8FU
#define SCSI_SEND_DIAGNOSTIC                        0x1DU
#define SCSI_READ_FORMAT_CAPACITIES                 0x23U

/* SCSI Command Data Length */
#define TEST_UNIT_READY_LEN                         0U
#define READ_CAPACITY10_DATA_LEN                    8U
#define INQUIRY_DATA_LEN                            36U
#define REQUEST_SENSE_DATA_LEN                      18U
#define READ_FORMAT_CAPACITY_DATA_LEN               12U
#define MODE_SENSE10_DATA_LEN                       8U
#define MODE_SENSE6_DATA_LEN                        4U
#define PAGE00_INQUIRY_DATA_LEN	                    7U

#define CBW_CB_LENGTH                               16U
#define CBW_LENGTH                                  10U

/* SCSI Sense key Code */
#define SCSI_SENSE_KEY_NO_SENSE                     0x00U
#define SCSI_SENSE_KEY_RECOVERED_ERROR              0x01U
#define SCSI_SENSE_KEY_NOT_READY                    0x02U
#define SCSI_SENSE_KEY_MEDIUM_ERROR                 0x03U
#define SCSI_SENSE_KEY_HARDWARE_ERROR               0x04U
#define SCSI_SENSE_KEY_ILLEGAL_REQUEST              0x05U
#define SCSI_SENSE_KEY_UNIT_ATTENTION               0x06U
#define SCSI_SENSE_KEY_DATA_PROTECT                 0x07U
#define SCSI_SENSE_KEY_BLANK_CHECK                  0x08U
#define SCSI_SENSE_KEY_VENDOR_SPECIFIC              0x09U
#define SCSI_SENSE_KEY_COPY_ABORTED                 0x0AU
#define SCSI_SENSE_KEY_ABORTED_COMMAND              0x0BU
#define SCSI_SENSE_KEY_VOLUME_OVERFLOW              0x0DU
#define SCSI_SENSE_KEY_MISCOMPARE                   0x0EU

/* SCSI ASC Code */
#define SCSI_ASC_NO_ADDITIONAL_SENSE_INFORMATION    0x00U
#define SCSI_ASC_WRITE_FAULT                        0x03U
#define SCSI_ASC_LOGICAL_UNIT_NOT_READY             0x04U
#define SCSI_ASC_UNRECOVERED_READ_ERROR             0x11U
#define SCSI_ASC_PARAMETER_LIST_LENGTH_ERROR        0x1AU
#define SCSI_ASC_INVALID_COMMAND_OPERATION_CODE     0x20U
#define SCSI_ASC_ADDRESS_OUT_OF_RANGE               0x21U
#define SCSI_ASC_INVALID_FIELD_IN_CDB               0x24U
#define SCSI_ASC_INVALID_FIELD_IN_PARAMETER_LIST    0x26U
#define SCSI_ASC_WRITE_PROTECTED                    0x27U
#define SCSI_ASC_MEDIUM_HAVE_CHANGED                0x28U
#define SCSI_ASC_NOT_READY_TO_READY_CHANGE          0x28U
#define SCSI_ASC_FORMAT_ERROR                       0x31U
#define SCSI_ASC_MEDIUM_NOT_PRESENT                 0x3AU

/* SCSI ASCQ */
#define SCSI_ASCQ_FORMAT_COMMAND_FAILED             0x01
#define SCSI_ASCQ_INITIALIZING_COMMAND_REQUIRED     0x02
#define SCSI_ASCQ_OPERATION_IN_PROGRESS             0x07

#endif // USB_SCSI_H
