/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_SCSI_H
#define USBD_SCSI_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_msc.h"

/* Exported defines ----------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

int usbd_scsi_process_cmd(usbd_msc_dev_t *cdev, u8 *cmd);
void usbd_scsi_sense_code(usbd_msc_dev_t *cdev, u8 key, u8 asc);

int usbd_msc_bulk_transmit(usb_dev_t *dev, u8 *buf, u32 len);
int usbd_msc_bulk_receive(usb_dev_t *dev, u8 *buf, u32 len);
void usbd_msc_send_csw(usb_dev_t *dev, u8 status);

#endif // USBD_SCSI_H
