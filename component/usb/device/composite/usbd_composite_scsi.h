/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_COMPOSITE_SCSI_H
#define USBD_COMPOSITE_SCSI_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_composite_msc.h"

/* Exported defines ----------------------------------------------------------*/

#define NO_SENSE                                    0U
#define RECOVERED_ERROR                             1U
#define NOT_READY                                   2U
#define MEDIUM_ERROR                                3U
#define HARDWARE_ERROR                              4U
#define ILLEGAL_REQUEST                             5U
#define UNIT_ATTENTION                              6U
#define DATA_PROTECT                                7U
#define BLANK_CHECK                                 8U
#define VENDOR_SPECIFIC                             9U
#define COPY_ABORTED                                10U
#define ABORTED_COMMAND                             11U
#define VOLUME_OVERFLOW                             13U
#define MISCOMPARE                                  14U

#define INVALID_CDB                                 0x20U
#define INVALID_FIELED_IN_COMMAND                   0x24U
#define PARAMETER_LIST_LENGTH_ERROR                 0x1AU
#define INVALID_FIELD_IN_PARAMETER_LIST             0x26U
#define ADDRESS_OUT_OF_RANGE                        0x21U
#define MEDIUM_NOT_PRESENT                          0x3AU
#define MEDIUM_HAVE_CHANGED                         0x28U
#define WRITE_PROTECTED                             0x27U
#define UNRECOVERED_READ_ERROR                      0x11U
#define WRITE_FAULT                                 0x03U

/* Exported functions --------------------------------------------------------*/

int usbd_composite_scsi_process_cmd(usbd_composite_msc_dev_t  *mdev, u8 *cmd);
void usbd_composite_scsi_sense_code(usbd_composite_msc_dev_t  *mdev, u8 skey, u8 asc);

int usbd_composite_msc_bulk_transmit(usb_dev_t *dev, u8 *buf, u16 len);
int usbd_composite_msc_bulk_receive(usb_dev_t *dev, u8 *buf, u16 len);
void usbd_composite_msc_send_csw(usb_dev_t *dev, u8 status);

#endif // USBD_COMPOSITE_SCSI_H
