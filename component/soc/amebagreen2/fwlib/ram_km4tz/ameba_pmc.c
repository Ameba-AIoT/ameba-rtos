/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

#ifdef CONFIG_TRUSTZONE
#define PMC_ENTRY_SECTION  IMAGE3_ENTRY_SECTION
#define PMC_ENTRY          NS_ENTRY
#else
#define PMC_ENTRY_SECTION
#define PMC_ENTRY
#endif


/**
  *  @brief set work modules/wake up event after sleep.
  *  @retval None
  */
PMC_ENTRY_SECTION
void PMC_ENTRY SOCPS_SleepInitEntry(struct PSCFG_TypeDef *ps_config)
{
	/*2. Clock configuration*/
	SOCPS_ClockSourceConfig(ps_config->regu_state_in_sleep, ps_config->xtal_mode_in_sleep, ps_config->keep_osc4m_on_in_sleep);

	/*3. Power settings*/
	SOCPS_PowerManage(ps_config->regu_state_in_sleep);

	if (EFUSE_GetChipVersion() == SYSCFG_CUT_VERSION_A) {
		/*Timing fix: Delay at least 1T before pulling down rst_n to avoid postsim bit errors.
		This problem will be fixed in Bcut by modifying pmc rom code*/
		SOCPS_FixSpicRetFailPatch();
		/*disable wake status of USB*/
		SOCPS_USBSuspendWakeControl(DISABLE);
	}

}

PMC_ENTRY_SECTION
u32 PMC_ENTRY SOCPS_CPURoleGetEntry(void)
{
	return SOCPS_CPURoleGet();
}

PMC_ENTRY_SECTION
void PMC_ENTRY SOCPS_HWSleepFilter(u32 enable)
{
	SOCPS_HWSleepFilter_AP(enable);
}

PMC_ENTRY_SECTION
void PMC_ENTRY SOCPS_PeriPermissionEntry(uint32_t ip_mask, u32 enable)
{
	SOCPS_PeriPermissionControl(ip_mask, enable);
}

PMC_ENTRY_SECTION
void PMC_ENTRY SOCPS_BitPermissionEntry(uint32_t ip_mask, u32 enable)
{
	SOCPS_BitPermissionControl(ip_mask, enable);
}

PMC_ENTRY_SECTION
void PMC_ENTRY SOCPS_NVICBackupEntry(struct CPU_BackUp_TypeDef *bk, SysTick_Type *systick, NVIC_Type *nvic, SCB_Type *scb)
{
	SOCPS_NVICBackup(bk, systick, nvic, scb);
}

PMC_ENTRY_SECTION
void PMC_ENTRY SOCPS_NVICReFillEntry(struct CPU_BackUp_TypeDef *bk, SysTick_Type *systick, NVIC_Type *nvic, SCB_Type *scb)
{
	SOCPS_NVICReFill(bk, systick, nvic, scb);
}

PMC_ENTRY_SECTION
void PMC_ENTRY SOCPS_MPUBackupEntry(struct CPU_BackUp_TypeDef *bk, MPU_Type *mpu)
{
	SOCPS_MPUBackup(bk, mpu);
}

PMC_ENTRY_SECTION
void PMC_ENTRY SOCPS_MPUReFillEntry(struct CPU_BackUp_TypeDef *bk, MPU_Type *mpu)
{
	SOCPS_MPUReFill(bk, mpu);
}