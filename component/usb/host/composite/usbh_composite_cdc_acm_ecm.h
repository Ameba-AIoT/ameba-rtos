/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_COMPOSITE_CDC_ACM_ECM_H
#define USBH_COMPOSITE_CDC_ACM_ECM_H

/* Includes ------------------------------------------------------------------*/
#include "usbh.h"
#include "usb_ch9.h"
#include "os_wrapper.h"
#include "usbh_composite_cdc_acm.h"
#include "usbh_composite_cdc_ecm.h"
#include "usbh_composite_config.h"

/* Exported defines ----------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/**
 * @brief  Initializes the CDC ACM ECM composite host class driver.
 *         Registers the application callback handler and user private parameters.
 * @param[in] acm_cb: Pointer to the CDC ACM user-defined callback structure.
 * @param[in] ecm_cb: Pointer to the CDC ECM user-defined callback structure.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_composite_acm_ecm_init(usbh_composite_cdc_acm_usr_cb_t *acm_cb, usbh_composite_cdc_ecm_usr_cb_t *ecm_cb);

/**
 * @brief  De-initializes the composite host class driver and releases resources.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
int usbh_composite_acm_ecm_deinit(void);

/**
 * @brief  Get the connect device Vendor ID.
 * @return Vendor ID
 */
u16 usbh_composite_acm_ecm_get_device_vid(void);

/**
 * @brief  Get the connect device Product ID.
 * @return Product ID
 */
u16 usbh_composite_acm_ecm_get_device_pid(void);

/**
 * @brief  Get whether the device is ready.
 * @return 0 (HAL_OK) on success, non-zero on failure.
 */
u8 usbh_composite_acm_ecm_is_ready(void);

#endif  /* USBH_COMPOSITE_CDC_ACM_ECM_H */

