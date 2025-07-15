/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _USBD_FULLMAC_CLASS_H_
#define _USBD_FULLMAC_CLASS_H_

/* Includes ------------------------------------------------------------------*/

#include "hci_core.h"
#include "usbd_hal.h"

/* Exported defines ----------------------------------------------------------*/

/* Endpoints parameters */
#define USBD_BULK_IN_EP							0x84U  /* EP4 for BULK IN */
#define USBD_BULK_OUT_EP						0x05U  /* EP5 for BULK OUT */

#define USBD_BULK_HS_MPS						512U
#define USBD_BULK_FS_MPS						64U

/* Exported types ------------------------------------------------------------*/

typedef struct {
	HCI_AdapterTypeDef *Adapter;
	USBD_SetupReqTypedef Request;
	u8  Data[512U];
	__IO u32 TxState;
	__IO u32 RxState;
} USBD_FullmacClassHandleTypeDef;

typedef struct {
	u8(* Setup)(HCI_AdapterTypeDef *adapter, USBD_SetupReqTypedef *req, u8 *buf);
	u8(* Receive)(HCI_AdapterTypeDef *adapter, u32 len);
} USBD_FullmacClassCallbackTypeDef;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

extern USBD_ClassTypeDef USBD_FullmacClassDriver;
extern USBD_FullmacClassHandleTypeDef USBD_FullmacClassHandle;

/* Exported functions --------------------------------------------------------*/

u8 USBD_Class_Transmit(u8 *buf, u32 len);

#endif  /* _USBD_FULLMAC_CLASS_H_ */

