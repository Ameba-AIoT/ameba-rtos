/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

// static const char *TAG = "PMC";

/**
  *  @brief set work modules/wake up event after sleep.
  *  @retval None
  */
void SOCPS_SleepInitEntry(struct PSCFG_TypeDef *ps_config)
{
	/*2. Clock configuration*/
	SOCPS_ClockSourceConfig(ps_config->regu_state_in_sleep, ps_config->xtal_mode_in_sleep, ps_config->keep_osc4m_on_in_sleep);

	/*3. Power settings*/
	SOCPS_PowerManage(ps_config->regu_state_in_sleep);
}

void SOCPS_HWSleepFilter(u32 enable)
{
	SOCPS_HWSleepFilter_NP(enable);
}

u32 SOCPS_CPURoleGetEntry(void)
{
	return SOCPS_CPURoleGet();
}

void SOCPS_NVICBackupEntry(struct CPU_BackUp_TypeDef *bk, SysTick_Type *systick, NVIC_Type *nvic, SCB_Type *scb)
{
	SOCPS_NVICBackup(bk, systick, nvic, scb);
}

void SOCPS_NVICReFillEntry(struct CPU_BackUp_TypeDef *bk, SysTick_Type *systick, NVIC_Type *nvic, SCB_Type *scb)
{
	SOCPS_NVICReFill(bk, systick, nvic, scb);
}

void SOCPS_MPUBackupEntry(struct CPU_BackUp_TypeDef *bk, MPU_Type *mpu)
{
	SOCPS_MPUBackup(bk, mpu);
}

void SOCPS_MPUReFillEntry(struct CPU_BackUp_TypeDef *bk, MPU_Type *mpu)
{
	SOCPS_MPUReFill(bk, mpu);
}