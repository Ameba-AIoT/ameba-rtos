/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EXAMPLE_USBH_CDC_NCM_H
#define EXAMPLE_USBH_CDC_NCM_H

/* Includes ------------------------------------------------------------------*/

#include "basic_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Run the USB host CDC NCM (Network Control Model dongle) example.
 * @return None
 */
void example_usbh_cdc_ncm(void);
int example_usbh_cdc_ncm_start(void);
int example_usbh_cdc_ncm_stop(void);
#ifdef __cplusplus
}
#endif

#endif /* EXAMPLE_USBH_CDC_NCM_H */
