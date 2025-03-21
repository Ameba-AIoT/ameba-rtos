/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_MSC_DISK_H
#define USBH_MSC_DISK_H

/* Includes ------------------------------------------------------------------*/

#include "vfs_fatfs.h"

/* Exported defines ----------------------------------------------------------*/

#define CONFIG_USBH_MSC_RETRY			0  /* Set to 1 for USB EMC test */

#define USBH_MSC_RETRY_CNT				3

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

extern ll_diskio_drv USB_disk_Driver;

/* Exported functions --------------------------------------------------------*/

#endif  /* USBH_MSC_DISK_H */

