/**
  ******************************************************************************
  * @file    vfs_nand_common.h
  * @author
  * @date    2026-1
  * @brief   This file is the header file for littlefs_nand_ftl.c
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef _VFS_NAND_COMMON_H
#define _VFS_NAND_COMMON_H

#include "ameba_soc.h"
#include "vfs_nand_err.h"
#include "vfs_nand_types.h"

/* Exported defines ----------------------------------------------------------*/

#ifndef NULL
#define NULL 0
#endif

#define SECOND_NAND_PAGE_SIZE_MAIN_BIT_EXP 		(g_nandflash_init_para.FLASH_pagesize_main_bit_exp)
#define SECOND_NAND_PAGE_SIZE_MAIN 				(1U << SECOND_NAND_PAGE_SIZE_MAIN_BIT_EXP)
#define SECOND_NAND_ADDR_TO_PAGE_ADDR(addr) 		((addr) >> SECOND_NAND_PAGE_SIZE_MAIN_BIT_EXP)
#define SECOND_NAND_ADDR_TO_BYTE_ADDR(addr) 		((addr) & BIT_LEN_MASK_32(SECOND_NAND_PAGE_SIZE_MAIN_BIT_EXP))

/* Exported types: Flash_InfoTypeDef / NAND_FTL_DeviceTypeDef /
 * NAND_FTL_MfgOpsTypeDef / NAND_FTL_PageWornStatusDef all come from the shared
 * vfs_nand_types.h included above. */

#include "vfs_second_nand_ftl.h"

u8 SECOND_NAND_FTL_ReadPage(u32 addr, u8 *buf);
u8 SECOND_NAND_FTL_ReadPageFast(u32 addr, u8 *buf);
u8 SECOND_NAND_FTL_EraseBlock(u32 addr, u8 force);
u8 SECOND_NAND_FTL_WritePage(u32 addr, const u8 *buf, u8 do_erase);
u8 SECOND_NAND_FTL_IsBad(u32 addr, u8 *value);
u8 SECOND_NAND_FTL_MarkBad(u32 addr);
u8 SECOND_NAND_FTL_Query(Flash_InfoTypeDef *info);
u8 SECOND_NAND_FTL_MfgInit(NAND_FTL_DeviceTypeDef *nand);

u32 NF_GetBlockAddr(NAND_FTL_DeviceTypeDef *nand, u32 addr);
u8 NF_SelectTarget(NAND_FTL_DeviceTypeDef *nand, u32 addr);
NAND_FTL_PageWornStatusDef NF_GetPageWornStatus(u8 ecc_status);

#endif

