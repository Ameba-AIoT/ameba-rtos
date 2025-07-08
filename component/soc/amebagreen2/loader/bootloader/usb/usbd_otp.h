/*
 *  Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef USBD_OTP_H
#define USBD_OTP_H

/* Includes ------------------------------------------------------------------*/

#include "ameba_soc.h"
#include "ameba_usb.h"
#include "usbd_hal.h"

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

#endif  /* USBD_OTP_H */

