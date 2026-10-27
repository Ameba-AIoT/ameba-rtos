/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_MSC_DISK_H
#define USBH_MSC_DISK_H

/* Includes ------------------------------------------------------------------*/

#include "vfs_fatfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Host_Constants USB Host Constants
 *  @{
 */
/** @defgroup Host_MSC_Disk_Constants Host MSC Disk Constants
 *  @brief Retry policy of the FatFs disk I/O glue of the MSC class.
 *  @{
 */

#define USBH_MSC_RETRY_ENABLE			0  /**< Set to 1 to retry failed sector I/O, used for the USB EMC test. */

#define USBH_MSC_RETRY_CNT				3  /**< Number of retries per sector when USBH_MSC_RETRY_ENABLE is 1. */

/** @} End of Host_MSC_Disk_Constants group */
/** @} End of USB_Host_Constants group */

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/** @addtogroup USB_Host_Variables USB Host Variables
 *  @{
 */
/** @defgroup Host_MSC_Disk_Variables Host MSC Disk Variables
 *  @brief Disk I/O driver the MSC class registers with the FatFs VFS layer.
 *  @{
 */

/** @brief Low-level disk I/O driver backing an attached USB mass storage device. */
extern ll_diskio_drv USB_disk_Driver;

/** @} End of Host_MSC_Disk_Variables group */
/** @} End of USB_Host_Variables group */

/* Exported functions --------------------------------------------------------*/

/** @} End of USB_Host_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBH_MSC_DISK_H */
