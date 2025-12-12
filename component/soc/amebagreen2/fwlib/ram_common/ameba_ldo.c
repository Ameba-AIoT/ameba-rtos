/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
static const char *const TAG = "BOOT";

/**
  * @brief  Set Core LDO voltage.
  * @param  Source: This parameter can be one of the following values:
  *		 @arg CORE_VOL_0P9
  *		 @arg CORE_VOL_1P0
  */
void LDO_CoreVolSet(u8 vol_type)
{
	u32 temp = HAL_READ32(OTPC_REG_BASE, SEC_OTP_HWCFG);

	if (vol_type == CORE_VOL_1P0) {
		if ((temp & SEC_BIT_BOOT_VOL_SEL) == 0) {
			RTK_LOGW(TAG, "OTP BOOT VOL choose 0.9V but usrcfg choose 1.0V!\n");
			/*Core LDO 1.0V & SWR 1.35V */
			temp |= SEC_BIT_BOOT_VOL_SEL;
			HAL_WRITE32(OTPC_REG_BASE, SEC_OTP_HWCFG, temp);
		}
		/*Trigger regu to perform operations */
		temp = HAL_READ32(PMC_BASE, AIP_TRIGGER);
		temp |= PMC_BIT_TRG_SYS_REGU_STS;
		HAL_WRITE32(PMC_BASE, AIP_TRIGGER, temp);
	} else  {
		if (temp & SEC_BIT_BOOT_VOL_SEL) {
			RTK_LOGW(TAG, "OTP BOOT VOL choose 1.0V but usrcfg choose 0.9V!\n");
		}
		/*The default voltage is 0.9V, no triggering required.*/
	}
}

/**
 * @brief set mem ldo power mode in normal(active) mode
 *
 * @param normal_mode,00:off 01:sleep (wo dummy) 10:normal-0 (wi dummy) 10:normal-1 (wo dummy)
 * @return TRUE/FALSE
 */
bool LDO_MemSetInNormal(u8 normal_mode)
{
	/* mldo settings in normal mode*/
	u32 Rtemp = 0, timeout = 10000;
	Rtemp = HAL_READ32(PMC_BASE, AIP_TRIGGER);
	Rtemp &= (~PMC_MASK_SYS_MLDO_NORM);
	Rtemp |= PMC_SYS_MLDO_NORM(normal_mode);
	HAL_WRITE32(PMC_BASE, AIP_TRIGGER, Rtemp);
	/* need manual trigger normal status change*/
	Rtemp = HAL_READ32(PMC_BASE, AIP_TRIGGER);
	Rtemp |= PMC_BIT_TRG_SYS_MLDO;
	HAL_WRITE32(PMC_BASE, AIP_TRIGGER, Rtemp);
	/* polling status change finished */
	while ((HAL_READ32(PMC_BASE, AIP_TRIGGER) & PMC_BIT_SYS_MLDO_RDY) == 0) {
		if (timeout == 0) {
			return FALSE;
		}
		timeout--;
	}
	return TRUE;
}

/**
 * @brief set mem ldo power mode in sleep
 *
 * @param sleep_mode,00:off 01:sleep (wo dummy) 10:normal-0 (wi dummy) 11:normal-1 (wo dummy)
 * @return TRUE/FALSE
 */
bool LDO_MemSetInSleep(u8 sleep_mode)
{
	u32 Rtemp = 0;
	u8 ret = FALSE;
	/* mldo settings in sleep mode*/
	Rtemp = HAL_READ32(PMC_BASE, AIP_TRIGGER);
	Rtemp &= ~PMC_MASK_SYS_MLDO_SLP;
	/* sleep without dummy mode in sleep.*/
	Rtemp |= PMC_SYS_MLDO_SLP(sleep_mode);
	HAL_WRITE32(PMC_BASE, AIP_TRIGGER, Rtemp);
	ret = TRUE;
	return ret;
}