/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

#define U32_BITS	32

/**
  * @brief  Configure slave port as secure or non-secure attribute.
  * @param  PPC_PeripheralId: it can be one of the following structure:
		typedef enum {
			...
		} PPC_PeripheralId;
  * @param  Status: it can be SECURE or NON_SECURE
  * @retval None
  */
void TZ_ConfigSlaveSecurity(PPC_PeripheralId Perip, u32 Status)
{
	PPC_TypeDef *PPC = KM4_PPC;
	volatile u32 *PPC_REG = &PPC->PPC1_REG;

	if (Status == SECURE) {
		PPC_REG[Perip / U32_BITS] &= ~(BIT(Perip % U32_BITS));
	} else if (Status == NON_SECURE) {
		PPC_REG[Perip / U32_BITS] |= BIT(Perip % U32_BITS);
	}
}

void TZ_tfm_platform_pmc_bkup(void)
{
#ifdef CONFIG_RDP_TFM
	int32_t tfm_platform_pmc_bkup(void);
	tfm_platform_pmc_bkup();
#endif
}
