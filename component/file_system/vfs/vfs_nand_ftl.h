/**
  ******************************************************************************
  * @file    floader_nand_ftl.h
  * @author  Realsil WLAN5 Team
  * @date    2021-01-27
  * @brief   This file is the header file for floader_nand_ftl.c
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef _LFS_NAND_FTL_H
#define _LFS_NAND_FTL_H

#include "ameba.h"
#include "vfs_nand_err.h"
#include "vfs_nand_types.h"

/* Exported defines ----------------------------------------------------------*/

#define NF_SMALL_PAGE_SIZE			2048U
#define NF_BIG_PAGE_SIZE			4096U

#ifndef NULL
#define NULL 0
#endif

/* Exported types ------------------------------------------------------------*/
/* Flash_InfoTypeDef / NAND_FTL_DeviceTypeDef / NAND_FTL_MfgOpsTypeDef and the
 * NAND_MFG_* / NAND_ONFI_* / FTL_MEM* definitions come from vfs_nand_types.h,
 * shared with the external-NAND FTL (included at the top of this file). */

u8 NAND_FTL_Init(void);
u8 NAND_FTL_ReadPage(u32 addr, u8 *buf);
/* Like NAND_FTL_ReadPage but skips the per-read bad-block marker check (one
 * extra NAND array-to-cache).  For callers (e.g. LBM) that already track bad
 * blocks in RAM and only ever read blocks they have vetted as good: the marker
 * check is redundant there and roughly doubles read latency.  ECC status is
 * still evaluated, so data-integrity reporting is unchanged. */
u8 NAND_FTL_ReadPageFast(u32 addr, u8 *buf);
u8 NAND_FTL_EraseBlock(u32 addr, u8 force);
u8 NAND_FTL_WritePage(u32 addr, const u8 *buf, u8 do_erase);
u8 NAND_FTL_MarkBad(u32 addr);
u8 NAND_FTL_MfgInit(NAND_FTL_DeviceTypeDef *nand);

#endif // _LFS_NAND_FTL_H
