/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_QSPI_H_
#define _AMEBA_QSPI_H_

#include "ameba_spic.h"
#include "ameba_gdma.h"
/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup QSPI
* @brief QSPI driver modules
* @{
*/

/* AUTO_GEN_START */


/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup QSPI_Exported_Definitions QSPI Exported Constants
  * @{
  */

#define BIT_QSPI_HW_HANDSHAKE_SEL		((u32)0x00000001 << 1)      /* 0: PPE, 1: GDMA */

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup QSPI_Exported_Types QSPI Exported Types
  * @{
  */

/**
  * @brief  QSPI Cmd Addr Info Structure Definition
  */
typedef struct {
	u8 cmd_length;		/* can be 0~3, means cmd length is 0~3 bytes */
	u8 addr_length;		/* can be 0~15, means addr length is 0~15 bytes */
	u8 cmd_ch;			/* can be 0~2, means 1~3 lines */
	u8 addr_ch;			/* can be 0~2, means 1~3 lines */
	u8 data_ch;			/* can be 0~2, means 1~3 lines */
	u8 cmd[3];
	u8 addr[15];
	u32 rx_dummy_cycle;
	u8 ddr_en;			/* can be 0~7, bit 0/1/2 indicate addr/data/cmd line */
} QSPI_CmdAddrInfo;

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

void QSPI_Init(void);
void QSPI_StructInit(QSPI_CmdAddrInfo *info);
void QSPI_SetBaud(u32 div);
void QSPI_SO_DNUM(u32 val);
void QSPI_Write(QSPI_CmdAddrInfo *info, u8 *data, u32 len);
void QSPI_Read(QSPI_CmdAddrInfo *info, u8 *data, u32 len);
void QSPI_WriteStart(QSPI_CmdAddrInfo *info, u32 len);
void QSPI_WriteEnd(void);
int QSPI_GDMAInit(GDMA_InitTypeDef *GDMA_InitStruct, IRQ_FUN CallbackFunc, void *CallbackData, u8 *Pbuf, u32 Size);
int QSPI_GDMADeinit(GDMA_InitTypeDef *GDMA_InitStruct);
void QSPI_GDMAStart(GDMA_InitTypeDef *GDMA_InitStruct);

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */


#endif

