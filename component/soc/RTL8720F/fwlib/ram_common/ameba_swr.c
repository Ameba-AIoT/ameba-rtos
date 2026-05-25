/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/**
 * @brief mem ldo mode power off req from sys in normal status
 *
 * @return true/false
 */
bool LDO_MemPowerOff(void)
{
	u32 Rtemp = 0, timeout = 10000;

	Rtemp = HAL_READ32(PMC_BASE, AIP_TRIGGER);
	Rtemp &= (~PMC_MASK_SYS_MLDO_NORM);
	/* need manual trigger normal status change, NM need trigger */
	Rtemp |= PMC_BIT_TRG_SYS_REGU_STS;
	HAL_WRITE32(PMC_BASE, AIP_TRIGGER, Rtemp);
	/* polling status change finished */
	while ((HAL_READ32(PMC_BASE, AIP_TRIGGER) & PMC_BIT_SYS_REGU_STS_RDY) == 0) {
		if (timeout == 0) {
			return false;
		}
		timeout--;
	}
	return true;
}