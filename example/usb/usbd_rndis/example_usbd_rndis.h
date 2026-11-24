/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EXAMPLE_USBD_RNDIS_H
#define EXAMPLE_USBD_RNDIS_H

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

void example_usbd_rndis(void);
int example_usbd_rndis_start(void);
int example_usbd_rndis_stop(void);
#ifdef __cplusplus
}
#endif

#endif /* EXAMPLE_USBD_RNDIS_H */
