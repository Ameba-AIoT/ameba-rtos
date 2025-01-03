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
void CORE_LDO_Vol_Set(u8 vol_type)
{
	u32 Temp;

	Temp = HAL_READ32(OTPC_REG_BASE, SEC_OTP_HWCFG);
	if (vol_type == CORE_VOL_1P0) { /*Core LDO 1.0V & SWR 1.35V */
		if ((Temp & SEC_BIT_BOOT_VOL_SEL) == 0) {
			/* if OTP not set, trigger to 1.0V manually */
			Temp |= SEC_BIT_BOOT_VOL_SEL;
			HAL_WRITE32(OTPC_REG_BASE, SEC_OTP_HWCFG, Temp);
			Temp = HAL_READ32(PMC_BASE, AIP_TRIGGER);
			Temp |= PMC_BIT_TRG_SYS_REGU_STS;
			HAL_WRITE32(PMC_BASE, AIP_TRIGGER, Temp);
		}
	} else if ((vol_type == CORE_VOL_0P9) && ((Temp & SEC_BIT_BOOT_VOL_SEL) != 0)) {
		RTK_LOGE(TAG, "OTP BOOT VOL choose 1.0V but usrcfg choose 0.9V!\n");
	}
}

/**
  * @brief  Set LDO_Mem with different dummyload.
  * @param  dummy_load: LDO_MEM_DUMMY_200UA/LDO_MEM_DUMMY_500UA/LDO_MEM_DUMMY_700UA
  */
void LDO_Mem_Dummy_Ctrl(u32 dummy_load)
{
	LDO_TypeDef *LDO = LDO_BASE;

	LDO->LDO_3318 &= ~LDO_MASK_DMY_LOAD;
	LDO->LDO_3318 |= LDO_DMY_LOAD(dummy_load);
}