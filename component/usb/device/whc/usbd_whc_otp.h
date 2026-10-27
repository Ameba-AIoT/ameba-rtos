/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_WHC_OTP_H
#define USBD_WHC_OTP_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Device_API USB Device API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Device_Constants USB Device Constants
 *  @{
 */
/** @defgroup Device_WHC_OTP_Constants Device WHC OTP Constants
 *  @brief String length limits of the OTP-latched device identity.
 *  @{
 */

#define USBD_OTP_MAX_STR_LEN  58U  /**< Maximum length in bytes of an OTP-stored identity string. */

/** @} End of Device_WHC_OTP_Constants group */
/** @} End of USB_Device_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Device_Types USB Device Types
 *  @{
 */
/** @defgroup Device_WHC_OTP_Types Device WHC OTP Types
 *  @brief Device identity and capability parameters read from OTP.
 *  @{
 */

/** @brief Device identity and capability parameters latched from OTP. */
typedef struct {
	u8 *mfg_str;              /**< Manufacturer string, heap-allocated by @ref usbd_otp_init. */
	u8 *prod_str;             /**< Product string, heap-allocated by @ref usbd_otp_init. */
	u8 *sn_str;               /**< Serial number string, heap-allocated by @ref usbd_otp_init. */
	u16 vid;                  /**< Vendor ID read from OTP. */
	u16 pid;                  /**< Product ID read from OTP. */
	u8 otp_sn;                /**< 1 when OTP supplied a valid serial number. */
	u8 otp_param;             /**< 1 when OTP supplied valid device parameters. */
	u8 self_powered : 1;      /**< 1 when the device reports itself as self-powered. */
	u8 remote_wakeup_en : 1;  /**< 1 when remote wakeup is enabled. */
	u8 bt_en : 1;             /**< 1 when the Bluetooth function is enabled in OTP. */
} usbd_otp_t;

/** @} End of Device_WHC_OTP_Types group */
/** @} End of USB_Device_Types group */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Device_Functions USB Device Functions
 *  @{
 */
/** @defgroup Device_WHC_OTP_Functions Device WHC OTP Functions
 *  @brief Reading the OTP-latched device identity, internal to the WHC class.
 *  @{
 */

/**
 * @brief  Read the device identity and parameters from OTP.
 * @note   Allocates the identity string buffers; release them with @ref usbd_otp_deinit.
 *         On a partial failure the already allocated buffers are freed before returning.
 * @param  otp: OTP parameter block to fill in.
 * @return HAL_OK on success, HAL_ERR_MEM when a buffer cannot be allocated.
 */
int usbd_otp_init(usbd_otp_t *otp);

/**
 * @brief  Free the buffers allocated by @ref usbd_otp_init.
 * @param  otp: OTP parameter block to release.
 * @return None
 */
void usbd_otp_deinit(usbd_otp_t *otp);

/** @} End of Device_WHC_OTP_Functions group */
/** @} End of USB_Device_Functions group */

/** @} End of USB_Device_API group */

#ifdef __cplusplus
}
#endif

#endif /* USBD_WHC_OTP_H */
