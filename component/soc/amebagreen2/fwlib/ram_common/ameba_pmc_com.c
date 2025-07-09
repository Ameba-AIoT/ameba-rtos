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

	bk->NVICbackup_HP[0] = nvic->ISER[0];
	bk->NVICbackup_HP[1] = nvic->ISER[1];
	bk->NVICbackup_HP[2] = nvic->ISER[2];
	bk->NVICbackup_HP[3] = nvic->ICER[0];
	bk->NVICbackup_HP[4] = nvic->ICER[1];
	bk->NVICbackup_HP[5] = nvic->ICER[2];
	for (i = 0; i < MAX_PERIPHERAL_IRQ_NUM; i++) {
		bk->NVICIPbackup_HP[i] = nvic->IPR[i];
	}

	bk->NVICbackup_HP[6] = nvic->ISPR[0];
	bk->NVICbackup_HP[7] = nvic->ISPR[1];
	bk->NVICbackup_HP[8] = nvic->ISPR[2];

#if defined(__VTOR_PRESENT) && (__VTOR_PRESENT == 1)
	bk->SCBbackup_HP[0] = scb->VTOR;
#endif
}

void SOCPS_NVICReFill(struct CPU_BackUp_TypeDef *bk, SysTick_Type *systick, NVIC_Type *nvic, SCB_Type *scb)
{
	int i = 0;

	/* Configure SysTick to interrupt at the requested rate. */
	systick->CTRL = bk->SYSTICKbackup_HP[0];
	systick->LOAD = bk->SYSTICKbackup_HP[1];
	systick->VAL = bk->SYSTICKbackup_HP[2];

	nvic->ISPR[0] = bk->NVICbackup_HP[6];
	nvic->ISPR[1] = bk->NVICbackup_HP[7];
	nvic->ISPR[2] = bk->NVICbackup_HP[8];

	nvic->ICER[0] = bk->NVICbackup_HP[3];
	nvic->ICER[1] = bk->NVICbackup_HP[4];
	nvic->ICER[2] = bk->NVICbackup_HP[5];

	for (i = 0; i < MAX_PERIPHERAL_IRQ_NUM; i++) {
		nvic->IPR[i] = bk->NVICIPbackup_HP[i];
	}

	nvic->ISER[0] = bk->NVICbackup_HP[0];
	nvic->ISER[1] = bk->NVICbackup_HP[1];
	nvic->ISER[2] = bk->NVICbackup_HP[2];

#if defined(__VTOR_PRESENT) && (__VTOR_PRESENT == 1)
	scb->VTOR = bk->SCBbackup_HP[0];
#endif
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

	if ((module == REG_CTRL_GRP_BT_E0_CTRL) || (module == REG_CTRL_GRP_BT_E1_CTRL)) {
		Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PLAT_CTRL);
		Rtemp |= LSYS_BIT_SHARE_BT_MEM;
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PLAT_CTRL, Rtemp);
	}
}

/**
 * @brief Set the power state of regu(swr + core ldo)during sleep, but it does not take effect immediately
 *
 * @param pwr_sts:
 * 				@arg STATE1_LDOPC_SWRPFM_07 = 0x1,
 * 				@arg STATE2_LDOPC_SWRPFM_08 = 0x2,
 * 				@arg STATE3_LDOPC_SWRPFM_09 = 0x3,
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
 * 				@arg STATE4_LDOSLP_SWRPFM   = 0x4,
 * 				@arg STATE5_LDONORM_SWRPFM  = 0x5,
 * 				@arg STATE6_LDONORM_SWRPWM  = 0x6,
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

void SOCPS_ReguDelayAdjust(u8 delay_count)
{
	LDO_TypeDef	*LDO = LDO_BASE;
	u32 Rtemp = 0;
	/*Reduce the delay time(100ms --> 500us) after swr steps down from 1.25/1.35 to 0.9v*/
	Rtemp = LDO->LDO_DUMMY;
	Rtemp &= ~LDO_MASK_PMC_PRG_DLY;
	Rtemp |= LDO_PMC_PRG_DLY(delay_count);
	LDO->LDO_DUMMY = Rtemp;
}
/**
 * @brief enable/disable usb wake status control bit
 *
 * @param status ENABLE/DISABLE
 */
void SOCPS_USBSuspendWakeControl(u8 status)
{
	PLL_TypeDef *sys_pll = (PLL_TypeDef *)PLL_REG_BASE;
	u32 reg_temp = sys_pll->PLL_UPLL_CTRL0;
	if (status == ENABLE) {
		reg_temp |= PLL_BIT_SUSPEND_WAK_MSK;
	} else {
		reg_temp &= ~PLL_BIT_SUSPEND_WAK_MSK;
	}
	sys_pll->PLL_UPLL_CTRL0 = reg_temp;
}

void SOCPS_FixSpicRetFailPatch(void)
{
	HAL_WRITE32(PMC_BASE, PMC_PATCH_GRP0_0, 0x00000087);
	HAL_WRITE32(PMC_BASE, PMC_PATCH_GRP0_0, 0x00C00187);
	HAL_WRITE32(PMC_BASE, PMC_PATCH_GRP0_0, 0x00C00187);
	HAL_WRITE32(PMC_BASE, PMC_PATCH_GRP0_1, 0x00000812);
	HAL_WRITE32(PMC_BASE, PMC_PATCH_GRP0_1, 0x20470812);
}

void SOCPS_ClockSourceConfig(u8 regu_state, u8 xtal_mode, u8 osc_option)
{
	u32 reg_temp = 0;
	LDO_TypeDef *LDO = LDO_BASE;
	/* 2. OSC4M config */
	reg_temp = HAL_READ32(PMC_BASE, SYSPMC_OPT);
	if (osc_option == TRUE) {
		reg_temp |= PMC_BIT_PST_SLEP_ERCK;
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
	if (xtal_mode != XTAL_OFF) {
		if (regu_state == STATE2_LDOPC_SWRPFM_08) {
			reg_temp = HAL_READ32(PMC_BASE, SYSPMC_OPT);
			reg_temp |= PMC_BIT_CKE_XTAL40M_SLEP;
			HAL_WRITE32(PMC_BASE, SYSPMC_OPT, reg_temp);
			/*Adjust voltage code of SWR to 0.9v*/
			reg_temp = LDO->LDO_PFM_VOLT_CTRL;
			reg_temp &= ~ LDO_MASK_VOLT_CODE1_PFM;
			reg_temp |= LDO_VOLT_CODE1_PFM(0x6);
			LDO->LDO_PFM_VOLT_CTRL = reg_temp;
		} else {
			RTK_LOGE(TAG, "xtal can only work in the state2 where the core voltage(LDO and SWR) is greater than 0.9v!\n");
		}
	}
}

void SOCPS_PowerManage(u8 regu_state)
{
	u8 chip_type = 0;
	u32 reg_temp = 0;
	SWR_TypeDef	*SWR = SWR_BASE;
	LDO_TypeDef *LDO = LDO_BASE;
	/* 1. regu (SWR and core LDO)sleep status configuuration. */
	if (regu_state) {
		/* 4.1 Set REGU's state when sleep.*/
		SOCPS_PowerStateSetInSleep(regu_state);
		/* 4.2 for simulation*/
		SOCPS_ReguDelayAdjust(REGU_DELAY_500US);
	} else {
		RTK_LOGE(TAG, "Invalid Regu state\n");
	}

	/* 2. regu (SWR and core LDO) normal status configuration: default settings,state6: SWR-PFM, LDO-NORM.*/

	/* change wait SOC power-cut stable time from 1024us to 40us to shrink wakeup time */
	reg_temp = HAL_READ32(PMC_BASE, SYSPMC_CTRL);
	reg_temp &= ~PMC_MASK_STL_PDSOC;
	reg_temp |= PMC_STL_PDSOC(0x1);
	HAL_WRITE32(PMC_BASE, SYSPMC_CTRL, reg_temp);

	/* change default regu sts debounce counter to meet 4M to 100k handshake sync time */
	reg_temp = HAL_READ32(PMC_BASE, ARB_DBC0);
	reg_temp &= ~PMC_MASK_DBC_CNT_REGU_STS;
	reg_temp |= PMC_DBC_CNT_REGU_STS(0xC7);
	HAL_WRITE32(PMC_BASE, ARB_DBC0, reg_temp);

	/*Set flash pad(g4) and psram pad(g5)*/
	chip_type = ChipInfo_MemoryType();

	if (chip_type == MEMORY_SINGLE_DIE) {
		/*case 1:chip of sing die, The G5 pad's io is not powered, so its IO core power (control reg) and IO pad(driver)
		  needs to be turned off directly.*/
		reg_temp = HAL_READ32(PMC_BASE, SYSPMC_OPT);
		/*In all packages, the G4 pad uses an external power supply and cannot be shut down.*/
		reg_temp &= ~(PMC_BIT_PST_G4_USE_MLDO | PMC_BIT_PST_SLEP_G4PD);
		HAL_WRITE32(PMC_BASE, SYSPMC_OPT, reg_temp);
		reg_temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_ISO);
		reg_temp &= ~AON_BIT_G5_PAD_SHDN_CORE;
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_ISO, reg_temp);
		reg_temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_PWC);
		reg_temp &= ~AON_BIT_G5_PAD_SHDN;
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_PWC, reg_temp);
	} else if (chip_type == MEMORY_MCM_PSRAM) {
		/*case 2: chip with psram use G5 pad, and G5 pad use mldo 1.8V, and cannot be shutdown pad by sys_pmc*/
		reg_temp = HAL_READ32(PMC_BASE, SYSPMC_OPT);
		/*In all packages, the G4 pad uses an external power supply and cannot be shut down.*/
		reg_temp &= ~(PMC_BIT_PST_G4_USE_MLDO | PMC_BIT_PST_SLEP_G4PD);
		reg_temp &= ~(PMC_BIT_PST_SLEP_G5PD);
		HAL_WRITE32(PMC_BASE, SYSPMC_OPT, reg_temp);
	} else {
		/*case 3: The chip with internal flash is powered by external 3.3V, so the G5 pad cannot be shut down.*/
		reg_temp = HAL_READ32(PMC_BASE, SYSPMC_OPT);
		/*In all packages, the G4 pad uses an external power supply and cannot be shut down.*/
		reg_temp &= ~(PMC_BIT_PST_G4_USE_MLDO | PMC_BIT_PST_SLEP_G4PD);
		reg_temp &= ~(PMC_BIT_PST_G5_USE_MLDO | PMC_BIT_PST_SLEP_G5PD);
		HAL_WRITE32(PMC_BASE, SYSPMC_OPT, reg_temp);
	}

	/*Set core ldo dummy load*/
	reg_temp = HAL_READ32(PMC_BASE, DUMMY_LOAD_CTRL);
	reg_temp &= ~ PMC_BIT_SYS_DMYL_SLP;
	reg_temp |= PMC_BIT_SYS_DMYL_MNL;
	HAL_WRITE32(PMC_BASE, DUMMY_LOAD_CTRL, reg_temp);

	/*modify pfm parameters of SWR*/
	reg_temp = SWR->SWR_PARAM_PFM;
	reg_temp &= ~ SWR_MASK_REG_ZCDC_H_PFM;
	reg_temp |= SWR_REG_ZCDC_H_PFM(1);
	reg_temp &= ~ SWR_MASK_REG_SWR_ZCD_CTRL_PFM;
	reg_temp |= SWR_REG_SWR_ZCD_CTRL_PFM(0x1);
	SWR->SWR_PARAM_PFM = reg_temp;

	/*optimize efficiency of SWR*/
	reg_temp = SWR->SWR_DIG_ZCD;
	reg_temp |= SWR_BIT_SPS_DIG_ZCD_FEN;
	SWR->SWR_DIG_ZCD = reg_temp;

	/* Enable dummyload to accelerate voltage reduction and reduce power cut delay time.*/
	if (EFUSE_GetChipVersion() == SYSCFG_CUT_VERSION_A) {
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_PMC_PATCH_GRP0_1, 0xE107FCB5);
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_PMC_PATCH_GRP0_2, 0x1818460C);

		/*enable swr dummy load to speed up when pfm voltage falling.*/
		reg_temp = LDO->LDO_DUMMY;
		reg_temp |= LDO_BIT_OPT_EN_SLP_DUMMY;
		LDO->LDO_DUMMY = reg_temp;
		SOCPS_ReguDelayAdjust(REGU_DELAY_1MS);
	}
}

u32 SOCPS_CPURoleGet(void)
{
	return HAL_READ32(PMC_BASE, SYSPMC_CTRL) & PMC_BIT_NP_CPU_ID;
}

void SOCPS_HWSleepFilter_AP(u32 enable)
{
	u32 Rtemp = 0;

	Rtemp = HAL_READ32(PMC_BASE, SYSPMC_CTRL);
	if (enable) {
		/* mask cpu0 irqs */
		Rtemp |= PMC_BIT_CPU0_IRQ_MASK;
	} else {
		/* unmask cpu0 irqs */
		Rtemp &= ~PMC_BIT_CPU0_IRQ_MASK;
	}
	HAL_WRITE32(PMC_BASE, SYSPMC_CTRL, Rtemp);
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

	Rtemp = HAL_READ32(PMC_BASE, SYSPMC_OPT);
	if (enable) {
		/* mask NP irqs */
		Rtemp |= PMC_BIT_CPU1_IRQ_MASK;
	} else {
		/* unmask NP irqs */
		Rtemp &= ~PMC_BIT_CPU1_IRQ_MASK;
	}
	HAL_WRITE32(PMC_BASE, SYSPMC_OPT, Rtemp);
}