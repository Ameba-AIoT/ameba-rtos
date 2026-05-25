/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
// static const char *const TAG = "CLK";
/**
  * @brief  32K clock Enable,
  * @param  NA
  * @note NA
  */
void SDM32K_Enable(void)
{

	SDM_TypeDef *SDM = SDM_DEV;

	SDM->SDM_CTRL0 |= SDM_BIT_EN | SDM_BIT_RST | SDM_BIT_ALWAYS_CAL_EN | SDM_BIT_TIMER_CAL_EN;
}

/**
  * @brief  32K clock Timer auto calibration Enable,
  * @param  newstatus: can be one of the following values:
  *		 @arg ENABLE: open timer-calibration
  *		 @arg FALSE not open Timer-calibration
  * @note NA
  */
void SDM32K_TimerCalEnable(u32 newstatus)
{

	SDM_TypeDef *SDM = SDM_DEV;
	if (newstatus == ENABLE) {
		SDM->SDM_CTRL0 |= SDM_BIT_TIMER_CAL_EN;
	} else {
		SDM->SDM_CTRL0 &= ~SDM_BIT_TIMER_CAL_EN;
	}
}

/**
  * @brief  Get osc calibration result.
  * @param  Option: can be one of the following values:
  *		 @arg OSC2M_CAL_CLK
  *		 @arg AON100K_CAL_CLK
  *		 @arg AON128K_CAL_CLK
  * @note NA
  */
u32 OSC_CalResult_Get(u8 cal_clk)
{
	u32 temp = 0;

	PLL_TypeDef *sys_pll = (PLL_TypeDef *)PLL_REG_BASE;

	/* start calibration */
	temp = sys_pll->PLL_CLK_CALC;
	temp &= ~PLL_MASK_CK_CAL_SEL;
	temp |= (PLL_CK_CAL_SEL(cal_clk) | PLL_BIT_CK_CAL_START);
	sys_pll->PLL_CLK_CALC = temp;

	/* poll calibration complete */
	while (1) {
		temp = sys_pll->PLL_CLK_CALC;
		if ((temp & PLL_BIT_CK_CAL_START) == 0) {
			break;
		}
	}

	/* read calibration result */
	temp = sys_pll->PLL_CLK_CALC;
	return PLL_GET_CK_CAL_RPT(temp);
}

u32 OSC_Clk_Get(void)
{
	return 32 * 40000000 / OSC_CalResult_Get(OSC2M_CAL_CLK);
}

void OSC2M_R_Set(u32 setbit, u32 clearbit)
{
	LDO_TypeDef *ldo = LDO_BASE;

	if (setbit) {
		ldo->LDO_2M_OSC_CTRL |= LDO_REG_2MOSC_RCAL(setbit);
	}

	if (clearbit) {
		ldo->LDO_2M_OSC_CTRL &= ~LDO_REG_2MOSC_RCAL(clearbit);
	}

	/* It takes 1ms to stable */
	DelayUs(200);

}

/**
  * @brief  OSC2M calibration
  * @param  ppm_limit:
  * @note NA
  */
u32 OSC2M_Calibration(u32 ppm_limit)
{
	UNUSED(ppm_limit);
	u32 target_fclk = 2500000;
	u32 temp = 0;
#if 0
	u32 cal_n = 0;
	u32 rsel_bit_index = 0;
	u32 delta = 0;
	u32 min_delta = 0xFFFFFFFF;
	u32 cur_ppm = 0;
	u32 min_delta_r = 0;
	u32 clearbit = 0;
	//cal_rpt=32*40Mhz/target_fclk, target_fclk is 2.5MHz now.
	u32 target_40m_counter = 32 * 40000000 / target_fclk;
#endif
	LDO_TypeDef *ldo = LDO_BASE;

	/* Step1: Power on aon 4MHz OSC: 0x4200_8100[2]=1'b1 */
	ldo->LDO_2M_OSC_CTRL |= LDO_BIT_REG_POW_2MOSC_FASTSET;

	while (1) {
		temp = ldo->LDO_2M_OSC_CTRL;
		if (temp & LDO_BIT_REG_POW_2MOSC) {
			break;
		}
	}

	/* The consistency of the chips' OSC is poor, so we switched to the lowest gear to avoid consistency issues */
	ldo->LDO_2M_OSC_CTRL &= ~LDO_MASK_REG_2MOSC_RCAL;
#if 0
	/* Step2: Adjust R_CAL. Enter the following loop: each loop decides one bit of 41000108[5:0]-RCAL, i.e. the first loop decides RCAL[5], the second loop decides RCAL[4] …, suppose the current loop is N (N=1..6) */
	/* Loop Step1: Set RCAL[6-N] = 0x1 */
	/* Loop Step2: Wait some time for clock stable (<0.2ms) */
	/* Loop Step3: Enable Calibration: PLL_CLK_CALC[31] 0x41000540[31] */
	/* Loop Step2: Poll 0x41000540[31] = 0x0, read calibration result: 0x41000540[15:0] */
	/* Loop Step3: */
	/* If(frequency is in the allowed deviation range) */
	/* 	Calibration will end successfully */
	/* Else if (frequency counter > target frequency counter) */ /* (frequency > target frequency) */
	/* 	RCAL[6-N] = 0x0 */
	/* Else */
	/* 	RCAL[6-N] keep 1. */
	/* If (N=6) */
	/* 	Calibration will end, and if the last loop frequency is OK , then calibration is OK, otherwise calibration is fail */
	/* Else */
	/* 	Go to Loop step1 */
	for (cal_n = 1; cal_n <= 6; cal_n++) {
		rsel_bit_index = 6 - cal_n;
		OSC2M_R_Set(BIT(rsel_bit_index), clearbit);

		/* read calibration result */
		temp = OSC_CalResult_Get(OSC2M_CAL_CLK);

		if (temp < target_40m_counter) {
			delta = target_40m_counter - temp;
			clearbit = BIT(rsel_bit_index);
		} else {
			delta = temp - target_40m_counter;
			clearbit = 0;
		}

		if (delta < min_delta) {
			min_delta = delta;
			min_delta_r = ldo->LDO_2M_OSC_CTRL;
			min_delta_r &= LDO_MASK_REG_2MOSC_RCAL;
		}

		//RTK_LOGD(TAG, "[CAL2M]:cal_n %d delta:%d \n", cal_n, delta);
	}

	/* the last one is not the best one */
	if (delta > min_delta) {
		ldo->LDO_2M_OSC_CTRL &= ~LDO_MASK_REG_2MOSC_RCAL;
		ldo->LDO_2M_OSC_CTRL |= min_delta_r;

		/* It takes 1ms to stable */
		DelayUs(200);

		/* read calibration result */
		temp = OSC_CalResult_Get(OSC2M_CAL_CLK);

		if (temp < target_40m_counter) {
			delta = target_40m_counter - temp;
		} else {
			delta = temp - target_40m_counter;
		}
	}

	cur_ppm = delta * 1000000 / target_40m_counter;
	RTK_LOGI(TAG, "[CAL2M]: delta:%lu target:%lu PPM: %lu PPM_Limit:%lu \n", delta, target_40m_counter, cur_ppm, ppm_limit);

	if (cur_ppm >= ppm_limit) {
		RTK_LOGE(TAG, "[CAL2M]: !!! cal fail !!! PPM: %lu PPM_Limit:%lu \n", cur_ppm, ppm_limit);
		assert_param(0);
	}
#endif

	/* get the actual clock */
	target_fclk = OSC_Clk_Get();

	/* set ratio for SDM */
	temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_DUMMY_AIP_INFO0);
	temp &= ~AON_MASK_CKD_OSC_2M;
	temp |= AON_CKD_OSC_2M((131072 + target_fclk) / (2 * 131072) - 1);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_DUMMY_AIP_INFO0, temp);

	return TRUE;
}


void XTAL_AACK(void)
{
#ifdef RTL8720F_TODO
	u32 temp;
	u32 xtal_mode;
	XTAL_TypeDef *xtal = XTAL_BASE;

	//XTAL must be high performance mode
	xtal_mode = XTAL_GET_MODE_DEC(xtal->XTAL_ANAPAR_XTAL_MODE_DEC_ON_1);
	if (xtal_mode != XTAL_HP_MODE) {
		RTK_LOGW(TAG, "XTAL is not in HP mode:%lx\n", xtal_mode);
	}

	//open clk for acc
	xtal->XTAL_ANAPAR_XTAL_ON_1 |= XTAL_BIT_EN_XTAL_DRV_DIGI;

	//reset_n_aac

	xtal->XTAL_FEN &= ~XTAL_BIT_FEN_XAACK;
	xtal->XTAL_FEN |= XTAL_BIT_FEN_XAACK;

	//only K as one set of GM values
	xtal->XTAL_ANAPAR_XTAL_AAC_ON_1 |= (XTAL_MASK_PK_START_AR | XTAL_MASK_PK_END_AR);

	xtal->XTAL_ANAPAR_XTAL_ON_2 |= XTAL_MASK_AAC_MODE;

	//EN_XTAL_AAC_PKDET
	xtal->XTAL_ANAPAR_XTAL_OFF_0 |= XTAL_BIT_EN_XTAL_AAC_PKDET;
	DelayUs(50);
	//EN_XTAL_AAC_DIGI
	xtal->XTAL_ANAPAR_XTAL_AAC |= XTAL_BIT_EN_XTAL_AAC_DIGI;
	DelayUs(50);
	//EN_XTAL_AAC_GM
	xtal->XTAL_ANAPAR_XTAL_OFF_0 |= XTAL_BIT_EN_XTAL_AAC_GM;
	DelayUs(50);

	while (1) {
		temp = xtal->XTAL_ANAPAR_XTAL_OFF_0 ;
		if (temp & XTAL_BIT_EN_XTAL_AAC_GM) {
			break;
		} else {
			xtal->XTAL_ANAPAR_XTAL_OFF_0 |= XTAL_BIT_EN_XTAL_AAC_GM;
		}
	}
#endif
}

SRAMDRAM_ONLY_TEXT_SECTION
static u32 _PERI_ClkGet(u8 peri_ckd)
{
	u32 peri_clk;

	IPC_SEMTake(IPC_SEM_RRAM, 0xffffffff);
	if (peri_ckd & IS_SYS_PLL) {
		peri_clk = GBSS_DEV->clk_info_bk.SYSPLL_CLK / GET_CLK_DIV(peri_ckd);
	} else {
		peri_clk = GBSS_DEV->clk_info_bk.USBPLL_CLK / GET_CLK_DIV(peri_ckd);
	}
	IPC_SEMFree(IPC_SEM_RRAM);

	return peri_clk;
}

u32 USB_PLL_ClkGet(void)
{
	u32 pll_clk;

	IPC_SEMTake(IPC_SEM_RRAM, 0xffffffff);
	pll_clk = GBSS_DEV->clk_info_bk.USBPLL_CLK;
	IPC_SEMFree(IPC_SEM_RRAM);

	return pll_clk;
}

u32 SYS_PLL_ClkGet(void)
{
	u32 pll_clk;

	IPC_SEMTake(IPC_SEM_RRAM, 0xffffffff);
	pll_clk = GBSS_DEV->clk_info_bk.SYSPLL_CLK;
	IPC_SEMFree(IPC_SEM_RRAM);

	return pll_clk;
}

u32 HPERI_ClkGet(void)
{
	return _PERI_ClkGet(GBSS_DEV->clk_info_bk.hperi_ckd);
}

u32 PSRAMC_ClkGet(void)
{
	return _PERI_ClkGet(GBSS_DEV->clk_info_bk.psramc_ckd);
}

// SRAMDRAM_ONLY_TEXT_SECTION
// u32 SPIC_ClkGet(void)
// {
// 	return _PERI_ClkGet(GBSS_DEV->clk_info_bk.spic_ckd);
// }

u32 CPU_ClkGet(void)
{
	if (SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA) {
		return 20 * MHZ_TICK_CNT;
	}

	if (CKSL_HP_XTAL == RCC_PeriphClockSourceGet(HP)) {
		return XTAL_ClkGet();
	} else {
		return _PERI_ClkGet(GBSS_DEV->clk_info_bk.CPU_CKD);
	}
}

int CPU_Clk_Check(void)
{
	int ret = RTK_SUCCESS;
#ifdef RTL8720F_TODO
	u32 CpuClk, CpuClk_Src;

	if (SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA) {
		CpuClk = 40 * MHZ_TICK_CNT;
	} else {

		CpuClk_Src = RCC_PeriphClockSourceGet(HP);

		if (CpuClk_Src == CKSL_HP_XTAL) {
			CpuClk = XTAL_ClkGet();
		}

		else if (CpuClk_Src == CKSL_HP_USB_PLL) {
			CpuClk = PLL_GetFreq(USB_PLL) / RCC_PeriphClockDividerGet(USB_PLL_HP);
		} else if (CpuClk_Src == CKSL_HP_SYS_PLL) {
			CpuClk = PLL_GetFreq(SYS_PLL) / RCC_PeriphClockDividerGet(SYS_PLL_HP);
		} else {
			assert_param(0);
		}
	}

	if (CPU_ClkGet() != CpuClk) {
		ret = RTK_FAIL;
	}
#endif
	return ret;
}

/**
 * @brief Obtain the most appropriate clock source and clock division coefficient to achieve the best IP performance
 *
 * @param sys_pll 	system pll clk
 * @param usb_pll 	usb pll clk
 * @param fre_limit limitation of cpu or other peripheral
 * @return The return value is the OR of the clock source and the clock division coefficient
 */
u8 PLL_ClkSrcGet(const u32 sys_pll, const u32 usb_pll, u32 fre_limit)
{
	u8 usb_pll_div, sys_pll_div;
	u8 ret = 0;
	if (sys_pll == PLL_NONE) {
		ret |= DIV_ROUND_UP(usb_pll, fre_limit);
		ret |= USB_PLL;
	} else {
		usb_pll_div = DIV_ROUND_UP(usb_pll, fre_limit);
		sys_pll_div = DIV_ROUND_UP(sys_pll, fre_limit);
		ret |= (usb_pll / usb_pll_div) >= (sys_pll / sys_pll_div) ? usb_pll_div : sys_pll_div;
		ret |= (usb_pll / usb_pll_div) >= (sys_pll / sys_pll_div) ? IS_USB_PLL : IS_SYS_PLL;
	}
	return ret;
}
