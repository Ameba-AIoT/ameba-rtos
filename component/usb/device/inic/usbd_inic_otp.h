/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBD_INIC_OTP_H
#define USBD_INIC_OTP_H

/* Includes ------------------------------------------------------------------*/

#include "usbd.h"

/* Exported defines ----------------------------------------------------------*/

#define USBD_OTP_MAX_STR_LEN	58U
#define USBD_INIC_MAX_STR_LEN	(USBD_OTP_MAX_STR_LEN * 2 + 2)

/* Exported types ------------------------------------------------------------*/

typedef struct {
	u8 *mfg_str;
	u8 *prod_str;
	u8 *sn_str;
	u8 *otp_map;
	u16 vid;
	u16 pid;
	u8 otp_sn;
	u8 otp_param;
	u8 mfg_str_len;
	u8 prod_str_len;
	u8 sn_str_len;
	u8 self_powered : 1;
	u8 remote_wakeup_en : 1;
	u8 bt_en : 1;
} usbd_otp_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

int usbd_otp_init(usbd_otp_t *otp);
void usbd_otp_deinit(usbd_otp_t *otp);

#endif  /* USBD_INIC_OTP_H */

