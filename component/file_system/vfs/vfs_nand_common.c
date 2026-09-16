/**
  ******************************************************************************
  * @file    floader_nand_ftl.c
  * @author  Realsil WLAN5 Team
  * @date    2021-01-27
  * @brief   This file provides the general FTL functionalities of NAND Flash
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#include "vfs_nand_common.h"

/* Public functions ---------------------------------------------------------*/

/**
  * @brief  Get block aligned page address
  * @param  nand : NAND device info
  * @param  addr : page address
  * @retval Block aligned page address
  */

u32 NF_GetBlockAddr(NAND_FTL_DeviceTypeDef *nand, u32 addr)
{
	Flash_InfoTypeDef *info = &nand->MemInfo;
	u32 result = addr - (addr % info->PagesPerBlock);
	return result;
}

/**
  * @brief  Select target
  * @param  nand : NAND device info
  * @param  addr : page address
  * @retval HAL_OK : OK; others : FAIL
  */

u8 NF_SelectTarget(NAND_FTL_DeviceTypeDef *nand, u32 addr)
{
	u8 ret = HAL_OK;
	u32 target;
	Flash_InfoTypeDef *info = &nand->MemInfo;
	const NAND_FTL_MfgOpsTypeDef *ops = (const NAND_FTL_MfgOpsTypeDef *)nand->MfgOps;

	if (ops->SelectTarget != NULL) {

		target = addr / (info->PagesPerBlock * info->BlocksPerLun * info->LunsPerTarget);
		if (target >= info->Targets) {
			return HAL_ERR_PARA;
		}

		if (target == nand->CurTarget) {
			return HAL_OK;
		}

		if (info->Targets == 1) {
			nand->CurTarget = target;
			return HAL_OK;
		}

		ret = ops->SelectTarget(nand, target);
	}

	return ret;
}

/**
  * @brief  Get NAND page worn status
  * @param  ecc_status : ECC status of the NAND page
  * @retval NAND_FTL_PageWornStatusDef
  */

NAND_FTL_PageWornStatusDef NF_GetPageWornStatus(u8 ecc_status)
{
	NAND_FTL_PageWornStatusDef page_status;

	switch (ecc_status) {
	case HAL_OK:
		page_status = NAND_PAGE_WORN_OK;
		break;
	case UERR_NAND_BITFLIP_WARN:
		page_status = NAND_PAGE_WORN_WARN;
		break;
	case UERR_NAND_BITFLIP_ERROR:
		page_status = NAND_PAGE_WORN_ERROR;
		break;
	default: // UERR_NAND_BITFLIP_FATAL
		page_status = NAND_PAGE_WORN_FATAL;
		break;
	}

	return page_status;
}
