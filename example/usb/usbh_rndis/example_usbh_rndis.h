/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EXAMPLE_USBH_RNDIS_H
#define EXAMPLE_USBH_RNDIS_H

/* Includes ------------------------------------------------------------------*/

#ifdef USB_APP_AGGREGATE
#include "basic_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Run the USB host RNDIS (Ethernet dongle) example.
 * @return None
 */
void example_usbh_rndis(void);
int example_usbh_rndis_start(void);
int example_usbh_rndis_stop(void);
#ifdef __cplusplus
}
#endif

#endif /* EXAMPLE_USBH_RNDIS_H */
