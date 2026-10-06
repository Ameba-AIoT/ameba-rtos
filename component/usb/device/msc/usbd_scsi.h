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

/* Exported variables --------------------------------------------------------*/

/**
 * @brief Class default SCSI standard INQUIRY response.
 * @details Template the per-instance response is built from; only the identification fields
 *          are overwritten from @ref usbd_msc_config_t.
 */
extern const u8 usbd_msc_default_inquiry_data[INQUIRY_DATA_LEN];

/* Exported functions --------------------------------------------------------*/

int usbd_scsi_process_cmd(usbd_msc_dev_t *cdev, u8 *cmd);
void usbd_scsi_sense_code(usbd_msc_dev_t *cdev, u8 key, u8 asc);

int usbd_msc_bulk_transmit(usb_dev_t *dev, u8 *buf, u32 len);
int usbd_msc_bulk_receive(usb_dev_t *dev, u8 *buf, u32 len, u32 buf_len);
void usbd_msc_send_csw(usb_dev_t *dev, u8 status);

#endif /* USBD_SCSI_H */
