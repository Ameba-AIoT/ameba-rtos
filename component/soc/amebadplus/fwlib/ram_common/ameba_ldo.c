/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
static const char *const TAG = "BOOT";

/**
  * @brief  Set Core voltage Type.
  * @param  Source: This parameter can be one of the following values:
  *		 @arg CORE_VOL_0P9
  *		 @arg CORE_VOL_1P0
  */
void LDO_CoreVolSet(u8 vol_type)
{
	u32 Temp;

	Temp = HAL_READ32(OTPC_REG_BASE, SEC_OTP_HWCFG);
	if (vol_type == CORE_VOL_1P0) {
		if ((Temp & SEC_BIT_BOOT_VOL_SEL) == 0) {
			RTK_LOGW(TAG, "OTP BOOT VOL choose 0.9V but usrcfg choose 1.0V!\n");
			Temp |= SEC_BIT_BOOT_VOL_SEL;
			HAL_WRITE32(OTPC_REG_BASE, SEC_OTP_HWCFG, Temp);
		}
		/*Trigger regu to perform operations */
		Temp = HAL_READ32(PMC_BASE, AIP_TRIGGER);
		Temp |= PMC_BIT_TRG_SYS_REGU_STS;
		HAL_WRITE32(PMC_BASE, AIP_TRIGGER, Temp);
	} else {
		if ((Temp & SEC_BIT_BOOT_VOL_SEL) != 0) {
			RTK_LOGW(TAG, "OTP BOOT VOL choose 1.0V but usrcfg choose 0.9V!\n");
		}
		/*The default voltage is 0.9V, no triggering required.*/
	}
}

/**
  * @brief  Set LDO_Mem with different dummyload.
  * @param  dummy_load: LDO_MEM_DUMMY_200UA/LDO_MEM_DUMMY_500UA/LDO_MEM_DUMMY_700UA
  */
void LDO_MemDummyCtrl(u32 dummy_load)
{
	LDO_TypeDef *LDO = LDO_BASE;

	LDO->LDO_3318 &= ~LDO_MASK_DMY_LOAD;
	LDO->LDO_3318 |= LDO_DMY_LOAD(dummy_load);
}