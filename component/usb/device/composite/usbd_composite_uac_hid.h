/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_COMPOSITE_UAC_HID_H
#define USBD_COMPOSITE_UAC_HID_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usbd_composite_config.h"
#include "usbd_composite_hid_bi_dir.h"
#include "usbd_composite_uac.h"

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

int usbd_composite_init(usbd_composite_hid_usr_cb_t *hid_cb, usbd_composite_uac_usr_cb_t *uac_cb,
						usbd_composite_cb_t *cb);

void usbd_composite_deinit(void);

#endif // USBD_COMPOSITE_UAC_HID_H

