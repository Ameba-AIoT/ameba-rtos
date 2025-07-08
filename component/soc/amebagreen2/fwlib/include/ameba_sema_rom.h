/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SEMA_ROM_H_
#define _AMEBA_SEMA_ROM_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @addtogroup IPC
  * @brief IPC driver modules
  * @{
  */

/* Exported types --------------------------------------------------------*/
/** @addtogroup IPC_Exported_Types IPC Exported Types
  * @{
  */
/**
  * @brief IPC SEM IDX
  */
typedef enum {
	IPC_SEM_IMQ = 0,
	IPC_SEM_FLASH,
	IPC_SEM_OTP,
	IPC_SEM_CRYPTO_AES_SW_KEY,
	IPC_SEM_GDMA,
	IPC_SEM_RRAM,
	IPC_SEM_6,          /* reserved for futural use*/
	IPC_SEM_DIAGNOSE,

	IPC_SEM_USER = 32,	/* number 32 ~ 63 is reserved for customer use*/

	IPC_SEM_MAX	= 63
} IPC_SEM_IDX;

/**
  * @}
  */
/* Exported constants --------------------------------------------------------*/
/** @addtogroup IPC_Exported_Constants IPC Exported Constants
  * @{
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SEMA_Exported_Functions Sema Exported Functions
  * @{
  */
void IPC_SEMAInit(void);
u32 IPC_SEMTake(u32 SEM_Idx, u32 timeout);
u32 IPC_SEMFree(u32 SEM_Idx);
void IPC_SEMDelayStub(void (*pfunc)(u32));
void IPC_patch_function(void (*pfunc1)(u32), void (*pfunc2)(u32));

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif

