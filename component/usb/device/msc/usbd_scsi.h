/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_SCSI_H
#define USBD_SCSI_H

/* Includes ------------------------------------------------------------------*/

#include "usbd_msc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Device_API USB Device API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/** @addtogroup USB_Device_Variables USB Device Variables
 *  @{
 */
/** @defgroup Device_SCSI_Variables Device SCSI Variables
 *  @brief Default SCSI response templates of the MSC class.
 *  @{
 */

/**
 * @brief Class default SCSI standard INQUIRY response.
 * @details Template the per-instance response is built from; only the identification fields
 *          are overwritten from @ref usbd_msc_config_t.
 */
extern const u8 usbd_msc_default_inquiry_data[INQUIRY_DATA_LEN];

/** @} End of Device_SCSI_Variables group */
/** @} End of USB_Device_Variables group */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Device_Functions USB Device Functions
 *  @{
 */
/** @defgroup Device_SCSI_Functions Device SCSI Functions
 *  @brief SCSI command execution and BOT transfer helpers, internal to the MSC class.
 *  @{
 */

/**
 * @brief  Execute a SCSI command received in a CBW.
 * @param  cdev: MSC device instance.
 * @param  cmd: SCSI command block.
 * @return RTK_SUCCESS on success, a RTK error code otherwise.
 */
int usbd_scsi_process_cmd(usbd_msc_dev_t *cdev, u8 *cmd);

/**
 * @brief  Record a sense key and additional sense code for the next REQUEST SENSE.
 * @param  cdev: MSC device instance.
 * @param  key: Sense key.
 * @param  asc: Additional sense code.
 * @return None
 */
void usbd_scsi_sense_code(usbd_msc_dev_t *cdev, u8 key, u8 asc);

/**
 * @brief  Check whether the host still has OUT data to send after the WRITE chunk being processed.
 * @param  cdev: Device instance.
 * @return 1 if more OUT data follows the current chunk, 0 otherwise.
 */
u8 usbd_scsi_out_data_pending(const usbd_msc_dev_t *cdev);

/**
 * @brief  Transmit data on the MSC bulk IN endpoint.
 * @param  dev: USB device instance.
 * @param  buf: Data to transmit.
 * @param  len: Data length in bytes.
 * @return RTK_SUCCESS on success, a RTK error code otherwise.
 */
int usbd_msc_bulk_transmit(usb_dev_t *dev, u8 *buf, u32 len);

/**
 * @brief  Arm a receive on the MSC bulk OUT endpoint.
 * @param  dev: USB device instance.
 * @param  buf: Buffer receiving the data.
 * @param  len: Number of bytes to receive.
 * @param  buf_len: Buffer capacity, which shall cover the DMA window; refer to usbd_ep_receive().
 * @return RTK_SUCCESS on success, a RTK error code otherwise.
 */
int usbd_msc_bulk_receive(usb_dev_t *dev, u8 *buf, u32 len, u32 buf_len);

/**
 * @brief  Send the Command Status Wrapper closing the current command.
 * @param  dev: USB device instance.
 * @param  status: CSW status, see @ref usb_msc_bot_csw_state_t.
 * @return None
 */
void usbd_msc_send_csw(usb_dev_t *dev, u8 status);

/** @} End of Device_SCSI_Functions group */
/** @} End of USB_Device_Functions group */

/** @} End of USB_Device_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBD_SCSI_H */
