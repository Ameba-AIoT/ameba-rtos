/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Define to prevent recursive  ----------------------------------------------*/
#ifndef USBH_MSC_SCSI_H
#define USBH_MSC_SCSI_H

/* Includes ------------------------------------------------------------------*/

#include "usbh_msc.h"

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

int usbh_scsi_test_unit_ready(usbh_msc_host_t *msc, u8 lun);
int usbh_scsi_read_capacity(usbh_msc_host_t *msc, u8 lun, usbh_scsi_capacity_t *capacity);
int usbh_scsi_inquiry(usbh_msc_host_t *msc, u8 lun, usbh_scsi_inquiry_t *inquiry);
int usbh_scsi_request_sense(usbh_msc_host_t *msc, u8 lun, usb_msc_scsi_sense_data_t *sense_data);
int usbh_scsi_write(usbh_msc_host_t *msc, u8 lun, u32 address, u8 *pbuf, u32 length);
int usbh_scsi_read(usbh_msc_host_t *msc, u8 lun, u32 address, u8 *pbuf, u32 length);

#endif  /* USBH_MSC_SCSI_H */

