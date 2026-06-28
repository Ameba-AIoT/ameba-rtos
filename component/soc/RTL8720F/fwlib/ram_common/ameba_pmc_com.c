/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "PMC";

void SOCPS_NVICBackup(struct CPU_BackUp_TypeDef *bk, SysTick_Type *systick, NVIC_Type *nvic, SCB_Type *scb)
{
	int i = 0;

	bk->SYSTICKbackup_HP[0] = systick->CTRL;
	bk->SYSTICKbackup_HP[1] = systick->LOAD;
	bk->SYSTICKbackup_HP[2] = systick->VAL;

	/* [0..2] enable mask (ISER read == ICER read, no need to save twice)
	 * [3..5] pending mask
	 */
	bk->NVICbackup_HP[0] = nvic->ISER[0];
	bk->NVICbackup_HP[1] = nvic->ISER[1];
	bk->NVICbackup_HP[2] = nvic->ISER[2];

	for (i = 0; i < MAX_PERIPHERAL_IRQ_NUM; i++) {
		bk->NVICIPbackup_HP[i] = nvic->IPR[i];
	}

	bk->NVICbackup_HP[3] = nvic->ISPR[0];
	bk->NVICbackup_HP[4] = nvic->ISPR[1];
	bk->NVICbackup_HP[5] = nvic->ISPR[2];

	bk->SCBbackup_HP[0] = scb->VTOR;
}

void SOCPS_NVICReFill(struct CPU_BackUp_TypeDef *bk, SysTick_Type *systick, NVIC_Type *nvic, SCB_Type *scb)
{
	int i = 0;

	/* Restore VTOR first so any subsequent exception/IRQ uses the correct
	 * vector table instead of the ROM table left over from wake path.
	 */
	scb->VTOR = bk->SCBbackup_HP[0];
	__DSB();
	__ISB();

	/* Configure SysTick to interrupt at the requested rate. */
	systick->CTRL = bk->SYSTICKbackup_HP[0];
	systick->LOAD = bk->SYSTICKbackup_HP[1];
	systick->VAL = bk->SYSTICKbackup_HP[2];

	nvic->ISPR[0] = bk->NVICbackup_HP[3];
	nvic->ISPR[1] = bk->NVICbackup_HP[4];
	nvic->ISPR[2] = bk->NVICbackup_HP[5];

	for (i = 0; i < MAX_PERIPHERAL_IRQ_NUM; i++) {
		nvic->IPR[i] = bk->NVICIPbackup_HP[i];
	}

	nvic->ISER[0] = bk->NVICbackup_HP[0];
	nvic->ISER[1] = bk->NVICbackup_HP[1];
	nvic->ISER[2] = bk->NVICbackup_HP[2];
}

void SOCPS_MPUBackup(struct CPU_BackUp_TypeDef *bk, MPU_Type *mpu)
{
	int i = 0;
	bk->MPU_BK.CTRL = mpu->CTRL;

	bk->MPU_BK.MAIR0 = mpu->MAIR0;
	bk->MPU_BK.MAIR1 = mpu->MAIR1;
	for (i = 0; i < MPU_MAX_REGION; i++) {
		mpu->RNR = i;
		bk->MPU_BK.RBAR[i] = mpu->RBAR;
		bk->MPU_BK.RLAR[i] = mpu->RLAR;
	}

}

void SOCPS_MPUReFill(struct CPU_BackUp_TypeDef *bk, MPU_Type *mpu)
{
	int i = 0;
	mpu->CTRL = 0;
	mpu->MAIR0 = bk->MPU_BK.MAIR0;
	mpu->MAIR1 = bk->MPU_BK.MAIR1;
	for (i = 0; i < MPU_MAX_REGION; i++) {
		mpu->RNR = i;
		mpu->RBAR = bk->MPU_BK.RBAR[i];
		mpu->RLAR = bk->MPU_BK.RLAR[i];
	}
	mpu->CTRL = bk->MPU_BK.CTRL;
}

/**
  * @brief  set sram mem mode
  * @param  module: sram mudule
  * @param  mem_mode: mem mode
  *		This parameter can be one of the following values:
  *		 @arg MEM_ACT_MODE : active mode
  *		 @arg MEM_LS_MODE : light sleep mode
  *		 @arg MEM_DS_MODE : deep sleep mode
  *		 @arg MEM_SD_MODE : shut down mode
  * @retval None
  * @note The system side can only control whether 96KB of the BT OFF area is shared;
  *       the WLK4 area of ​​WIFI is no longer shared.
  */
void SOCPS_SetMemMode(u32 module, u32 mem_mode)
{
	u32 Rtemp = 0;

	Rtemp = HAL_READ32(SYSTEM_MEM_CTRL_BASE, module);
	Rtemp &= (~CTRL_MASK_GRP_x_PWR_LPS);
	Rtemp |= CTRL_GRP_x_PWR_LPS(mem_mode);
	HAL_WRITE32(SYSTEM_MEM_CTRL_BASE, module, Rtemp);
}
/**
  * @brief  setting mem ldo mode req from sys in sleep status
  * @param  type:00:off 01:sleep (wo dummy) 10:normal-0 (wi dummy) 10:normal-1 (wo dummy)*
  * @retval None
  */

void SOCPS_MemLdoSetInSleep(u8 pwr_sts)
{
	u32 Rtemp = 0;

	Rtemp = HAL_READ32(PMC_BASE, AIP_TRIGGER);
	Rtemp &= (~PMC_MASK_SYS_MLDO_SLP);
	/* for sleep status, hw auto trigger when sleep */
	Rtemp |= PMC_SYS_MLDO_SLP(pwr_sts);
	HAL_WRITE32(PMC_BASE, AIP_TRIGGER, Rtemp);

}
/**
  * @brief  setting mem ldo mode req from sys in normal status
  * @param  type:00:off 01:sleep (wo dummy) 10:normal-0 (wi dummy) 10:normal-1 (wo dummy)*
  * @retval None
  */

void SOCPS_MemLdoSetInNormal(u8 pwr_sts)
{
	u32 Rtemp = 0;

	Rtemp = HAL_READ32(PMC_BASE, AIP_TRIGGER);
	Rtemp &= (~PMC_MASK_SYS_MLDO_NORM);
	/* need manual trigger normal status change, NM need trigger */
	Rtemp |= (PMC_SYS_MLDO_NORM(pwr_sts) | PMC_BIT_TRG_SYS_REGU_STS);
	HAL_WRITE32(PMC_BASE, AIP_TRIGGER, Rtemp);
	/* polling status change finished */
	while ((HAL_READ32(PMC_BASE, AIP_TRIGGER) & PMC_BIT_SYS_REGU_STS_RDY) == 0);
}
/**
 * @brief Set the power state of regu(swr + core ldo)during sleep, but it does not take effect immediately
 *
 * @param pwr_sts:
 * 				@arg STATE2_LDOSLP_08 = 0x2,
 */
void SOCPS_PowerStateSetInSleep(u8 pwr_sts)
{
	/* */
	u32 reg_temp = HAL_READ32(PMC_BASE, AIP_TRIGGER);
	reg_temp &= (~PMC_MASK_SYS_REGU_STS_SLP);
	reg_temp |= PMC_SYS_REGU_STS_SLP(pwr_sts);
	HAL_WRITE32(PMC_BASE, AIP_TRIGGER, reg_temp);
	RTK_LOGI(TAG, "AIP_TRIGGER %08x\n", HAL_READ32(PMC_BASE, AIP_TRIGGER));
}
/**
 * @brief Set the power state of regu(swr + core ldo) during active,and take effect immediately
 *
 * @param pwr_sts Power state
 * 				@arg STATE6_LDONORM_09 = 0x6,
 * 				@arg STATE6_LDONORM_10 = 0x7,
 */
void SOCPS_PowerStateSetInNormal(u8 pwr_sts)
{
	u32 Rtemp = 0;
	Rtemp = HAL_READ32(PMC_BASE, AIP_TRIGGER);
	Rtemp &= (~PMC_MASK_SYS_REGU_STS_NORM);
	/* need manual trigger normal status change */
	Rtemp |= (PMC_SYS_REGU_STS_NORM(pwr_sts) | PMC_BIT_TRG_SYS_REGU_STS);
	HAL_WRITE32(PMC_BASE, AIP_TRIGGER, Rtemp);
	/* polling status change finished */
	while ((HAL_READ32(PMC_BASE, AIP_TRIGGER) & PMC_BIT_SYS_REGU_STS_RDY) == 0);
}

/**
 * @brief Raise sleep voltage to 0.9V by overriding the 0.8V code slot.
 *
 * Writes the per-IC 0.9V code (read from CODE2) into CODE1, so the
 * system sleeps at 0.9V regardless of the sleep_to_08V setting in sleepcfg.
 */
void SOCPS_SleepVoltRaiseTo0P9(void)
{
	LDO_TypeDef *LDO = LDO_BASE;
	u32 reg_temp;
	u8 code_0p9;

	reg_temp = LDO->LDO_CLDO_VOLT_CTRL;
	code_0p9 = (u8)LDO_GET_VOLT_CODE2_CLDO(reg_temp);
	reg_temp &= ~LDO_MASK_VOLT_CODE1_CLDO;
	reg_temp |= LDO_VOLT_CODE1_CLDO(code_0p9);
	LDO->LDO_CLDO_VOLT_CTRL = reg_temp;
}

void SOCPS_ReguDelayAdjust(void)
{
	LDO_TypeDef	*LDO = LDO_BASE;
	u32 Rtemp = 0;
	/*Reduce the delay time(100ms --> 500us) after swr steps down from 1.25/1.35 to 0.9v*/
	Rtemp = LDO->LDO_DUMMY;
	Rtemp &= ~LDO_MASK_PMC_PRG_DLY;
	Rtemp |= LDO_PMC_PRG_DLY(0x1);
	LDO->LDO_DUMMY = Rtemp;
}

void SOCPS_ClockSourceConfig(u8 xtal_mode, u8 osc_option)
{
	u32 reg_temp = 0;
	LDO_TypeDef	*LDO = LDO_BASE;
	/* 2. osc2m config */
	reg_temp = HAL_READ32(PMC_BASE, SYSPMC_OPT);
	if (osc_option == TRUE) {
		reg_temp |= PMC_BIT_PST_SLEP_ERCK;
		LDO->LDO_DUMMY |= LDO_BIT_OPT_EN_SLP_DUMMY;/* Force OSC2M to be enabled in deepsleep mode. */
	} else {
		reg_temp &= ~PMC_BIT_PST_SLEP_ERCK;
	}
	HAL_WRITE32(PMC_BASE, SYSPMC_OPT, reg_temp);
	RTK_LOGD(TAG, "SYSPMC_OPT %08x\n", HAL_READ32(PMC_BASE, SYSPMC_OPT));

	/* 3.1 XTAL mode config */
	reg_temp = HAL_READ32(PMC_BASE, AIP_TRIGGER);
	reg_temp &= ~PMC_MASK_SYS_XTAL_SLP;
	reg_temp |= PMC_SYS_XTAL_SLP(xtal_mode);

	/* 3.2 for FPGA, XTAL need be on, remodify config if set to XTAL OFF*/
	if ((SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA) && (xtal_mode == XTAL_OFF)) {
		reg_temp &= ~PMC_MASK_SYS_XTAL_SLP;
		reg_temp |= PMC_SYS_XTAL_SLP(XTAL_Normal); //keep normal in fpga
	}
	HAL_WRITE32(PMC_BASE, AIP_TRIGGER, reg_temp);
	RTK_LOGI(TAG, "AIP_TRIGGER %08x\n", HAL_READ32(PMC_BASE, AIP_TRIGGER));

	/* 5. XTAL sleep status config */
	if ((xtal_mode == XTAL_Normal) || (xtal_mode == XTAL_HP)) {
		SOCPS_SleepVoltRaiseTo0P9();
		/* xtal clock gating can be set to 1 only when the sleep voltage is greater than or equal to 0.9V!*/
		reg_temp = HAL_READ32(PMC_BASE, SYSPMC_OPT);
		reg_temp |= PMC_BIT_CKE_XTAL40M_SLEP;
		HAL_WRITE32(PMC_BASE, SYSPMC_OPT, reg_temp);
		RTK_LOGI(TAG, "The voltage of XTAL Normal/HP mode in sleep state is 0.9V\n");
	}
}

void SOCPS_PowerManage(void)
{
	u32 reg_temp = 0;
	ADC_TypeDef *adc = ADC;

	/* 1. regu (core LDO)sleep status configuuration. */
	/* 1.1 Set REGU's state when sleep.*/
	SOCPS_PowerStateSetInSleep(STATE2_LDOSLP_08);
#if defined (CONFIG_WHC_DEV) && defined (CONFIG_WHC_INTF_SDIO)
	SOCPS_SleepVoltRaiseTo0P9();
#endif
	/* 1.2 for simulation*/
	SOCPS_ReguDelayAdjust();

	/* 2. regu (core LDO) normal status configuration. */
	SOCPS_PowerStateSetInNormal(STATE6_LDONORM_09);

	/* change wait SOC power-cut stable time to shrink wakeup time */
	reg_temp = HAL_READ32(PMC_BASE, SYSPMC_CTRL);
	reg_temp &= ~(PMC_MASK_STL_PDSOC | PMC_MASK_TUTIME);
	reg_temp |= PMC_TUTIME(0x12); 	/*unit: (0x12 + 1) * 1/2.5M = 7.6us*/
	reg_temp |= PMC_STL_PDSOC(0x2);	/*time = 2^(2+1) * unit = 60.8us*/
	HAL_WRITE32(PMC_BASE, SYSPMC_CTRL, reg_temp);

	/* change default regu sts debounce counter to meet 4M to 100k handshake sync time */
	reg_temp = HAL_READ32(PMC_BASE, ARB_DBC0);
	reg_temp &= ~PMC_MASK_DBC_CNT_REGU_STS;
	reg_temp |= PMC_DBC_CNT_REGU_STS(0xC7);
	HAL_WRITE32(PMC_BASE, ARB_DBC0, reg_temp);

	/*Set this option so that syspmc does not have to wait for mldo ready when wake-up*/
	reg_temp = HAL_READ32(PMC_BASE, AIP_TRIGGER);
	reg_temp &= ~PMC_MASK_SYS_MLDO_SLP;
	reg_temp |= PMC_SYS_MLDO_SLP(0x1);
	HAL_WRITE32(PMC_BASE, AIP_TRIGGER, reg_temp);

	MCM_MemTypeDef meminfo = ChipInfo_MCMInfo();

	if (meminfo.mem_type == MCM_SINGLE_DIE) {
		/* case 1: Single die.Shutdown G4 PAD Core circuits */
		reg_temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_ISO);
		reg_temp &= ~AON_BIT_G4_PAD_SHDN_CORE;
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_ISO, reg_temp);

		reg_temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_PWC);
		reg_temp &= ~AON_BIT_G4_PAD_SHDN;
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_PWC, reg_temp);

	}

	/* Turn off thermal force enable function. */
	adc->ADC_TM_POW_CTRL &= ~ADC_BIT_TM_POW_FORCE_EN;
}

u32 SOCPS_CPURoleGet(void)
{
	return HAL_READ32(PMC_BASE, SYSPMC_CTRL) & PMC_BIT_NP_CPU_ID;
}

void SOCPS_HWSleepFilter_AP(u32 enable)
{
	u32 Rtemp = 0;

	if (enable) {
		/* mask cpu0 irqs */
		Rtemp = HAL_READ32(PMC_BASE, SYSPMC_CTRL);
		Rtemp |= PMC_BIT_CPU0_IRQ_MASK;
		HAL_WRITE32(PMC_BASE, SYSPMC_CTRL, Rtemp);
	} else {
		/* mask cpu0 irqs */
		Rtemp = HAL_READ32(PMC_BASE, SYSPMC_CTRL);
		Rtemp &= ~PMC_BIT_CPU0_IRQ_MASK;
		HAL_WRITE32(PMC_BASE, SYSPMC_CTRL, Rtemp);
	}
}
void SOCPS_PeriPermissionControl(uint32_t ip_mask, u32 enable)
{
	u32 temp = HAL_READ32(SYSTEM_CTRL_BASE_S, REG_LSYS_SEC_PPC_CTRL);
	if (enable == ENABLE) {
		temp |= ip_mask;
	} else {
		temp &= ~ip_mask;
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE_S, REG_LSYS_SEC_PPC_CTRL, temp);
}

void SOCPS_BitPermissionControl(uint32_t ip_mask, u32 enable)
{

	u32 temp = HAL_READ32(SYSTEM_CTRL_BASE_S, REG_LSYS_SEC_BPC_CTRL);
	if (enable == ENABLE) {
		temp |= ip_mask;
	} else {
		temp &= ~ip_mask;
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE_S, REG_LSYS_SEC_BPC_CTRL, temp);
}

void SOCPS_HWSleepFilter_NP(u32 enable)
{
	u32 Rtemp = 0;

	if (enable) {
		/* mask NP irqs */
		Rtemp = HAL_READ32(PMC_BASE, SYSPMC_OPT);
		Rtemp |= PMC_BIT_CPU1_IRQ_MASK;
		HAL_WRITE32(PMC_BASE, SYSPMC_OPT, Rtemp);
	} else {
		/* mask NP irqs */
		Rtemp = HAL_READ32(PMC_BASE, SYSPMC_OPT);
		Rtemp &= ~PMC_BIT_CPU1_IRQ_MASK;
		HAL_WRITE32(PMC_BASE, SYSPMC_OPT, Rtemp);
	}
}

/**
  *  @brief  restore peripherals after power gating sleep
  *  @retval None
  */
void SOCPS_PeriRestore(void)
{
	CRYPTO_Init();
	pke_engine_init_sleep();
}
