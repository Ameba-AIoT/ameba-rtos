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

/* Exported constants --------------------------------------------------------*/
/** @addtogroup IPC_Exported_Constants IPC Exported Constants
  * @{
  */

/** @defgroup IPC_SEMA_MAPPING_ADDR
  * @{
  */
#define IPC_SEMA_MAPPING_ADDR 0x200
/**
  * @}
  */


/** @defgroup IPC_SEMA_STATUS
  * @{
  */
#define IPC_SEM_IDLE		0
#define OTP_SEM_IDX			1
#define IPC_SEM_FLASH		2
#define GDMA_SEM_IDX        3
#define IPC_SEM_CRYPTO		4
#define DIAGNOSE_SEM_IDX  5
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup IPC_Exported_Functions IPC Exported Functions
  * @{
  */

/** @defgroup SEMA_Exported_Functions Sema Exported Functions
  * @{
  */
u32 IPC_SEMTake(u32 SEM_Idx, u32 timeout);
u32 IPC_SEMFree(u32 SEM_Idx);
void IPC_SEMDelay(void (*pfunc)(u32));
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

/**
  * @}
  */

#endif

