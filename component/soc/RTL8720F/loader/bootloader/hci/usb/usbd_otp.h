/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _USBD_OTP_H_
#define _USBD_OTP_H_

/* Includes ------------------------------------------------------------------*/

#include "ameba_soc.h"
#include "ameba_usb.h"
#include "usbd_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

typedef struct {
	u8 otp_map[64];
	u8 mfg_str[64];
	u8 prod_str[64];
	u8 sn_str[64];
	u16 vid;
	u16 pid;
	u16 mfg_str_len;
	u16 prod_str_len;
	u16 sn_str_len;
	u8 self_powered : 1;
	u8 remote_wakeup_en : 1;
	u8 otp_sn : 1;
	u8 otp_param : 1;
} USB_OTP_TypeDef;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

u8 USB_OTP_Init(USB_OTP_TypeDef *otp);

#ifdef __cplusplus
}
#endif

#endif  /* _USBD_OTP_H_ */

