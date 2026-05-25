/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"


/**
  * @brief  Get boot reason
  * @param  NA
  * @retval Boot Reason.
  *   This parameter can be any of the following values or combinations:
  *		 @arg AON_BIT_RSTF_THM
  *		 @arg AON_BIT_RSTF_WARM_NP2PERI
  *		 @arg AON_BIT_RSTF_WARM_AP2PERI
  *		 @arg AON_BIT_RSTF_WDG2
  *		 @arg AON_BIT_RSTF_WDG1
  *		 @arg AON_BIT_RSTF_WDG0
  *		 @arg AON_BIT_RSTF_IWDG
  *		 @arg AON_BIT_RSTF_NP_SYS
  *		 @arg AON_BIT_RSTF_AP_SYS
  *		 @arg AON_BIT_RSTF_DSLP
  *		 @arg AON_BIT_RSTF_BOR
  */
u32 BOOT_Reason(void)
{
	return (HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_REASON_SW) & 0x0003FFFF);
}

/**
  * @brief  warm reset the system
  * @param  None
  * @retval None
  */
void System_Reset(void)
{
	u32 CPUID = SYS_CPUID();
	u32 Trig = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SW_RST_CTRL);

	/*valid CPU ID or not*/
	if (CPUID == KM4TZ_CPU_ID) {
		Trig |= LSYS_BIT_KM4TZ_SYS_RST;
	} else if (CPUID == KM4NS_CPU_ID) {
#ifdef CONFIG_SOLO
		/* SOLO mode doesn't support KM4NS trigger system reset */
		return;
#endif
		Trig |= LSYS_BIT_KM4NS_SYS_RST;
	} else {
		/*invalid CPU ID*/
		return;
	}

	HAL_WRITE16(SYSTEM_CTRL_BASE, REG_LSYS_SW_RST_TRIG, SYS_RESET_KEY);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_SW_RST_CTRL, Trig);
	HAL_WRITE16(SYSTEM_CTRL_BASE, REG_LSYS_SW_RST_TRIG, SYS_RESET_TRIG);

	/* wait for reset */
	while (1);
}


/**
  * @brief  reset the platform1
  * @param  None
  * @retval None
  */
void Platform1_Reset(void)
{
	u32 CPUID = SYS_CPUID();
	u32 Trig = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SW_RST_CTRL);

	Trig |= LSYS_BIT_PLFM1_SYS_RST;

	HAL_WRITE16(SYSTEM_CTRL_BASE, REG_LSYS_SW_RST_TRIG + 0x2, SYS_RESET_KEY);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_SW_RST_CTRL, Trig);
	HAL_WRITE16(SYSTEM_CTRL_BASE, REG_LSYS_SW_RST_TRIG + 0x2, SYS_RESET_TRIG);

	if (CPUID == KM4NS_CPU_ID) {
		/* wait for reset */
		while (1);
	}
}