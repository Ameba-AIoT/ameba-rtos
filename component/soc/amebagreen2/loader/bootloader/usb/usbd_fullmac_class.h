/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __USBD_FULLMAC_CLASS_H
#define __USBD_FULLMAC_CLASS_H

/* Includes ------------------------------------------------------------------*/

#include "usbd_hal.h"

/* Exported defines ----------------------------------------------------------*/

/* Endpoints parameters */
#define USBD_BULK_IN_EP							0x84U  /* EP4 for BULK IN */
#define USBD_BULK_OUT_EP						0x05U  /* EP5 for BULK OUT */

#define USBD_BULK_HS_MPS						512U
#define USBD_BULK_FS_MPS						64U

/* Exported types ------------------------------------------------------------*/

typedef struct {
	USBD_SetupReqTypedef Request;
	u32 RxLength;
	u8  RxBuffer[2 * 1024U];
	u8  TxBuffer[2 * 1024U];
	u8  Data[512U];
	__IO u32 TxState;
	__IO u32 RxState;
} USBD_Class_HandleTypeDef;

typedef struct {
	u8(* Init)(void);
	u8(* DeInit)(void);
	u8(* Setup)(USBD_SetupReqTypedef *req, u8 *buf);
	u8(* Receive)(USBD_Class_HandleTypeDef *handle);
} USBD_ClassCallbackTypeDef;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

extern USBD_ClassTypeDef USBD_ClassCallback;
extern USBD_Class_HandleTypeDef USBD_Class_Handle;

/* Exported functions --------------------------------------------------------*/

u8  USBD_Class_Transmit(u8 *buf, u32 len);

#endif  /* __USBD_FULLMAC_CLASS_H */

