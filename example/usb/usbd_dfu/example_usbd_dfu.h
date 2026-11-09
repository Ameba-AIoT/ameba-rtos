/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EXAMPLE_USBD_DFU_H
#define EXAMPLE_USBD_DFU_H

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
 * @brief  Run the USB device DFU (Device Firmware Upgrade) example.
 * @return None
 */
void example_usbd_dfu(void);
int example_usbd_dfu_start(void);
int example_usbd_dfu_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* EXAMPLE_USBD_DFU_H */
