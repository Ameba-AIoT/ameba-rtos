/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_COMPOSITE_CDC_ACM_MSC_H
#define USBD_COMPOSITE_CDC_ACM_MSC_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"
#include "usbd_composite_config.h"
#include "usbd_composite_cdc_acm.h"
#include "usbd_composite_msc.h"

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Init composite class
 * @param[in] cdc_bulk_out_xfer_size: CDC ACM bulk out xfer buffer size
 * @param[in] cdc_bulk_in_xfer_size: CDC ACM bulk in xfer buffer size
 * @param[in] cdc_cb: CDC ACM user callback
 * @param[in] cb: Composite user callback
 * @return 0 on success, non-zero on failure
 */
int usbd_composite_init(u16 cdc_bulk_out_xfer_size, u16 cdc_bulk_in_xfer_size, usbd_composite_cdc_acm_usr_cb_t *cdc_cb, usbd_composite_cb_t *cb);

/**
 * @brief  DeInit composite class
 * @return None
 */
void usbd_composite_deinit(void);

#endif // USBD_COMPOSITE_CDC_ACM_MSC_H

