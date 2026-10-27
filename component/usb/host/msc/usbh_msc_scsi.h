/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_MSC_SCSI_H
#define USBH_MSC_SCSI_H

/* Includes ------------------------------------------------------------------*/

#include "usbh_msc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Host_API USB Host API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Host_Functions USB Host Functions
 *  @{
 */
/** @defgroup Host_MSC_SCSI_Functions Host MSC SCSI Functions
 *  @brief SCSI commands issued over Bulk-Only Transport, internal to the MSC class.
 *  @{
 */

/**
 * @brief  Issue the TEST UNIT READY command.
 * @param  msc: MSC host handle.
 * @param  lun: Logical Unit Number.
 * @return HAL_OK on success, a HAL error code otherwise.
 */
int usbh_scsi_test_unit_ready(usbh_msc_host_t *msc, u8 lun);

/**
 * @brief  Issue the READ CAPACITY (10) command.
 * @param  msc: MSC host handle.
 * @param  lun: Logical Unit Number.
 * @param  capacity: Output, receives the block count and block size.
 * @return HAL_OK on success, a HAL error code otherwise.
 */
int usbh_scsi_read_capacity(usbh_msc_host_t *msc, u8 lun, usbh_scsi_capacity_t *capacity);

/**
 * @brief  Issue the INQUIRY command.
 * @param  msc: MSC host handle.
 * @param  lun: Logical Unit Number.
 * @param  inquiry: Output, receives the standard INQUIRY response.
 * @return HAL_OK on success, a HAL error code otherwise.
 */
int usbh_scsi_inquiry(usbh_msc_host_t *msc, u8 lun, usbh_scsi_inquiry_t *inquiry);

/**
 * @brief  Issue the REQUEST SENSE command.
 * @param  msc: MSC host handle.
 * @param  lun: Logical Unit Number.
 * @param  sense_data: Output, receives the sense key, ASC and ASCQ.
 * @return HAL_OK on success, a HAL error code otherwise.
 */
int usbh_scsi_request_sense(usbh_msc_host_t *msc, u8 lun, usb_msc_scsi_sense_data_t *sense_data);

/**
 * @brief  Issue the WRITE (10) command.
 * @param  msc: MSC host handle.
 * @param  lun: Logical Unit Number.
 * @param  address: Start sector address.
 * @param  pbuf: Data to write.
 * @param  length: Number of sectors to write.
 * @return HAL_OK on success, a HAL error code otherwise.
 */
int usbh_scsi_write(usbh_msc_host_t *msc, u8 lun, u32 address, u8 *pbuf, u32 length);

/**
 * @brief  Issue the READ (10) command.
 * @param  msc: MSC host handle.
 * @param  lun: Logical Unit Number.
 * @param  address: Start sector address.
 * @param  pbuf: Buffer receiving the data read.
 * @param  length: Number of sectors to read.
 * @return HAL_OK on success, a HAL error code otherwise.
 */
int usbh_scsi_read(usbh_msc_host_t *msc, u8 lun, u32 address, u8 *pbuf, u32 length);

/** @} End of Host_MSC_SCSI_Functions group */
/** @} End of USB_Host_Functions group */

/** @} End of USB_Host_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBH_MSC_SCSI_H */
