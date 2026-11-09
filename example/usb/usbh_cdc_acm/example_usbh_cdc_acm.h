/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EXAMPLE_USBH_CDC_ACM_H
#define EXAMPLE_USBH_CDC_ACM_H

/* Includes ------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Run the USB host CDC ACM (virtual serial port) example.
 * @return None
 */
void example_usbh_cdc_acm(void);
int example_usbh_cdc_acm_start(void);
int example_usbh_cdc_acm_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* EXAMPLE_USBH_CDC_ACM_H */
