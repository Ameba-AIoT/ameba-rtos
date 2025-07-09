/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
static const char *const TAG = "CLK";
/**
  * @brief  32K clock Enable,
  * @param  NA
  * @note NA
  */
void SDM32K_Enable(void)
{

	SDM_TypeDef *SDM = SDM_DEV;

	SDM->SDM_TIMEOUT = SDM_TIEMRCAL_INTERVAL_1MIN;
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
  *		 @arg OSC4M_CAL_CLK
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

/* setbit/clearbit is: BIT0~BIT5 defined in BIT_AON_MASK_OSC32K_RCAL_5_0 */
void OSC131_R_Set(u32 setbit, u32 clearbit)
{
	RTC_MISC_TypeDef *osc131k = OSC131K_BASE;
	u32 temp;

	temp = osc131k->RTC_OSC131K_CTRL;
	if (setbit) {
		temp |= RTC_RCAL(setbit);
	}

	if (clearbit) {
		temp &= ~RTC_RCAL(clearbit);
	}

	osc131k->RTC_OSC131K_CTRL = temp;
	/* It takes 1ms to stable */
	DelayMs(1);

}

/**
  * @brief  131.072K calibration
  * @param  max_delta: target_40m_counter-calibration_result, default 10
  * @note Here XTAL clock is used to count N cycles of calibration clock, the total used XTAL clock cycles can be read by this register, N= 8
  */
u32 OSC131K_Calibration(u32 ppm_limit)
{
	u32 temp = 0;
	u32 cal_n = 0;
	u32 rsel_bit_index = 0;
	u32 delta = 0;
	u32 min_delta = 0xFFFFFFFF;
	u32 cur_ppm = 0;
	u32 min_delta_r = 0;
	u32 target_40m_counter = 2441; //cal_rpt=8*40Mhz/fclk_osc131k
	u32 clearbit = 0;
	RTC_MISC_TypeDef *osc131k = OSC131K_BASE;

	/*Ensure that 131k can output stably before calibration.*/
	while (((LDO_BASE->LDO_DEBUG)&LDO_DBG_OSC_OK) == 0x0);

	/* Step1: Power on aon 128kHz OSC: 0x4200_8100[0]=1'b1 */
	/* Clear 4m calibration parameter first */
	osc131k->RTC_OSC131K_CTRL &= ~RTC_MASK_RCAL;
	while (1) {
		temp = osc131k->RTC_OSC131K_POW;
		if (temp & RTC_BIT_POW_32KOSC) {
			break;
		}
	}

	/* Step2: Adjust R_CAL. Enter the following loop: each loop decides one bit of 4800002c[13:8]-RCAL, i.e. the first loop decides RCAL[5], the second loop decides RCAL[4] …, suppose the current loop is N (N=1..6) */
	/* Loop Step1: Set RCAL[6-N] = 0x1 */
	/* Loop Step2: Wait some time for clock stable (this wait time decide by SW) */
	/* Loop Step3: Enable Calibration: 0x4800_2840 = 0x8200_0000 */
	/* Loop Step2: Poll 0x4800_2840[31] = 0x0, read calibration result: 0x4800_2840[15:0] */
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
		OSC131_R_Set(BIT(rsel_bit_index), clearbit);

		/* read calibration result */
		temp = OSC_CalResult_Get(AON128K_CAL_CLK);

		if (temp < target_40m_counter) {
			delta = target_40m_counter - temp;
			clearbit = BIT(rsel_bit_index);
		} else {
			delta = temp - target_40m_counter;
			clearbit = 0;
		}

		if (delta < min_delta) {
			min_delta = delta;
			min_delta_r = osc131k->RTC_OSC131K_CTRL;
			min_delta_r &= RTC_MASK_RCAL;
		}
		// RTK_LOGI(TAG, "[CAL131K]:cal_n %d delta:%d \n", cal_n, delta);
	}

	/* the last one is not the best one */
	if (delta > min_delta) {
		temp = osc131k->RTC_OSC131K_CTRL;
		temp &= ~RTC_MASK_RCAL;
		temp |= min_delta_r;
		osc131k->RTC_OSC131K_CTRL = temp;

		/* It takes 1ms to stable */
		DelayMs(1);

		/* read calibration result */
		temp = OSC_CalResult_Get(AON128K_CAL_CLK);

		if (temp < target_40m_counter) {
			delta = target_40m_counter - temp;
		} else {
			delta = temp - target_40m_counter;
		}
	}

	cur_ppm = delta * 1000000 / target_40m_counter;
	RTK_LOGI(TAG, "[CAL131K]: delta:%lu target:%lu PPM: %lu PPM_Limit:%lu \n", delta, target_40m_counter, cur_ppm, ppm_limit);

	if (cur_ppm >= ppm_limit) {
		RTK_LOGE(TAG, "[CAL131K]: !!! cal fail !!! PPM: %lu PPM_Limit:%lu \n", cur_ppm, ppm_limit);
		assert_param(0);
	}

	return TRUE;
}

/* bitnum is: 0~7 defined in REGU_MASK_FREQ_R_SEL */
void OSC4M_R_Set(u32 setbit, u32 clearbit)
{
	LDO_TypeDef *ldo = LDO_BASE;

	u32 r_sel = ldo->LDO_4M_OSC_CTRL1;

	if (setbit) {
		r_sel |= LDO_FREQ_R_SEL(setbit);
	}

	if (clearbit) {
		r_sel &= ~LDO_FREQ_R_SEL(clearbit);
	}
	/*	root cause: When the frequency is greater than 6M, The timeout mechanism can cause register write exceptions
	 	solve problem:  the L/H/R value can be written normally
		work around: Repeat writing to the register to make the previous write immediately effective
	*/
	if ((SYSCFG_RLVersion()) == SYSCFG_CUT_VERSION_A) {
		ldo->LDO_4M_OSC_CTRL1 = r_sel;
	}
	ldo->LDO_4M_OSC_CTRL1 = r_sel;

	/* It takes 2us to stable */
	DelayUs(2);
}

/* bitnum is: 0~3 defined in REGU_MASK_VCM_SEL_H/L */
void OSC4M_VCM_Set(u32 value)
{
	u32 temp;

	LDO_TypeDef *ldo = LDO_BASE;
	temp = ldo->LDO_4M_OSC_CTRL1;
	DelayUs(20);

	temp &= ~(LDO_MASK_VCM_SEL_L | LDO_MASK_VCM_SEL_H);
	temp |= (LDO_VCM_SEL_L(value) | LDO_VCM_SEL_H(value));

	/*	root cause: When the frequency is greater than 6M, The timeout mechanism can cause register write exceptions
	 	solve problem:  the L/H/R value can be written normally
		work around: Repeat writing to the register to make the previous write immediately effective
	*/
	if ((SYSCFG_RLVersion()) == SYSCFG_CUT_VERSION_A) {
		ldo->LDO_4M_OSC_CTRL1 = temp;
		DelayUs(20);
	}
	ldo->LDO_4M_OSC_CTRL1 = temp;
	DelayUs(20);

	/* It takes 2us to stable */
	DelayUs(2);
}

/**
  * @brief  OSC4M calibration
  * @param  ppm_limit: target_40m_counter-calibration_result, default 10
  * @note NA
  */
u32 OSC4M_Calibration(u32 ppm_limit)
{
	u32 temp = 0;
	u32 cal_n = 0;
	u32 rsel_bit_index = 0;
	u32 delta = 0;
	u32 min_delta = 0xFFFFFFFF;
	u32 cur_ppm = 0;
	u32 min_delta_r = 0;
	u32 target_40m_counter = 320; //cal_rpt=32*40Mhz/fclk_osc4m
	u32 clearbit = 0;
	LDO_TypeDef *ldo = LDO_BASE;
	/* Step1: Power on aon 4MHz OSC: 0x4200_8100[2]=1'b1 */
	ldo->LDO_RF_EN_DUMMY |= LDO_BIT_REG_POW_4MOSC;
	DelayUs(20);
	//RTK_LOGI(TAG, "regu->REGU_4MOSC0=0x%x\n",(regu->REGU_4MOSC0));

	/* Step2: Clear 4m calibration parameter first */
	if ((SYSCFG_RLVersion()) == SYSCFG_CUT_VERSION_A) {
		ldo->LDO_4M_OSC_CTRL1 &= (u16)(~(LDO_MASK_VCM_SEL_L | LDO_MASK_VCM_SEL_H));
		DelayUs(20);
	}
	ldo->LDO_4M_OSC_CTRL1 &= (u16)(~(LDO_MASK_VCM_SEL_L | LDO_MASK_VCM_SEL_H));
	DelayUs(20);
	//RTK_LOGI(TAG, "regu->REGU_4MOSC0=0x%x\n",(regu->REGU_4MOSC0));

	/* Step3: Adjust V_H_SEL & V_L_SEL. Enter the following loop: suppose the current loop is N (N=1..4) */
	/* Loop Step1: Set REGU_VCM_SEL_L[4-N] = 0x1, REGU_VCM_SEL_H[4-N] = 0x1 */
	/* Loop Step2: Enable Calibration, read calibration result*/
	/* Loop Step3: */
	/* If(frequency is in the allowed deviation range)  */
	/* 	Calibration will end successfully */
	/* Else if (frequency counter > target frequency counter) */ /* (frequency < target frequency) */
	/* 	REGU_VCM_SEL_L/H[4-N] = 0x0 */
	/* Else  */
	/* 	REGU_VCM_SEL_L/H[4-N] keep 1. */
	for (cal_n = 5; cal_n <= 9; cal_n++) {

		OSC4M_VCM_Set(cal_n);

		/* read calibration result */
		temp = OSC_CalResult_Get(OSC4M_CAL_CLK);
		// RTK_LOGI(TAG, "[CAL4M]:cal_n %d H:0x%x L:0x%x result:%d ", cal_n, LDO_GET_VCM_SEL_H(ldo->LDO_4M_OSC_CTRL1), LDO_GET_VCM_SEL_L(ldo->LDO_4M_OSC_CTRL1), temp);

		if (temp < target_40m_counter) {
			delta = target_40m_counter - temp;
			clearbit = 0;
		} else if (temp > target_40m_counter) {
			delta = temp - target_40m_counter;
			clearbit = BIT(rsel_bit_index);
		} else {
			delta = 0;
			goto cal_end;
		}

		if (delta < min_delta) {
			min_delta = delta;
			min_delta_r = ldo->LDO_4M_OSC_CTRL1;
			DelayUs(20);
			min_delta_r &= (LDO_MASK_VCM_SEL_L | LDO_MASK_VCM_SEL_H | LDO_MASK_FREQ_R_SEL);
		}
		// RTK_LOGI(NOTAG, "delta:%d min_delta:%d min_delta_r:0x%x \n", delta, min_delta, min_delta_r);
	}

	/* the last one is not the best one */
	if (delta > min_delta) {
		temp = ldo->LDO_4M_OSC_CTRL1;
		DelayUs(20);
		temp &= ~(LDO_MASK_VCM_SEL_L | LDO_MASK_VCM_SEL_H);
		temp |= min_delta_r;
		if ((SYSCFG_RLVersion()) == SYSCFG_CUT_VERSION_A) {
			ldo->LDO_4M_OSC_CTRL1 = temp;
			DelayUs(20);
		}
		ldo->LDO_4M_OSC_CTRL1 = temp;
		DelayUs(20);

		// RTK_LOGI(TAG, "temp=0x%x\n", temp);
	}
	// RTK_LOGI(TAG, "[CAL4M]:hL last one: 0x%x 0x%x temp:%x \n", LDO_GET_VCM_SEL_H(ldo->LDO_4M_OSC_CTRL1), LDO_GET_VCM_SEL_L(ldo->LDO_4M_OSC_CTRL1), temp);
	/* Step4: Adjust R_SEL. Enter the following loop: suppose the current loop is N (N=1..8) */
	/* Clear 4m calibration parameter first */
	/* Loop Step1: Set REGU_FREQ_R_SEL[8-N] = 0x1 */
	/* Loop Step2: Enable Calibration, read calibration result*/
	/* Loop Step3: */
	/* If(frequency is in the allowed deviation range)  */
	/* 	Calibration will end successfully */
	/* Else if (frequency counter > target frequency counter) */ /* (frequency < target frequency) */
	/* 	REGU_FREQ_R_SEL[8-N] = 0x0 */
	/* Else  */
	/* 	REGU_FREQ_R_SEL[8-N] keep 1. */
	clearbit = 0;
	if ((SYSCFG_RLVersion()) == SYSCFG_CUT_VERSION_A) {
		ldo->LDO_4M_OSC_CTRL1 &= (u16)(~LDO_MASK_FREQ_R_SEL);
		DelayUs(20);
	}
	ldo->LDO_4M_OSC_CTRL1 &= (u16)(~LDO_MASK_FREQ_R_SEL);
	DelayUs(20);
	for (cal_n = 1; cal_n <= 8; cal_n++) {
		rsel_bit_index = 8 - cal_n;
		OSC4M_R_Set(BIT(rsel_bit_index), clearbit);

		/* read calibration result */
		temp = OSC_CalResult_Get(OSC4M_CAL_CLK);
		// RTK_LOGI(TAG, "[CAL4M]:cal_n %d RCAL:0x%x result:%d ", cal_n, LDO_GET_FREQ_R_SEL(ldo->LDO_4M_OSC_CTRL1), temp);

		if (temp < target_40m_counter) {
			delta = target_40m_counter - temp;
			clearbit = 0;
		} else if (temp > target_40m_counter) {
			delta = temp - target_40m_counter;
			clearbit = BIT(rsel_bit_index);
		}  else {
			delta = 0;
			goto cal_end;
		}

		if (delta < min_delta) {
			min_delta = delta;
			min_delta_r = ldo->LDO_4M_OSC_CTRL1;
			DelayUs(20);
			min_delta_r &= LDO_MASK_FREQ_R_SEL;
		}
		// RTK_LOGI(NOTAG, "delta:%d \n", delta);
	}

	/* the last one is not the best one */
	if (delta > min_delta) {
		temp = ldo->LDO_4M_OSC_CTRL1;
		DelayUs(20);
		temp &= ~LDO_MASK_FREQ_R_SEL;
		temp |= min_delta_r;
		if ((SYSCFG_RLVersion()) == SYSCFG_CUT_VERSION_A) {
			ldo->LDO_4M_OSC_CTRL1 = temp;
			DelayUs(20);
		}
		ldo->LDO_4M_OSC_CTRL1 = temp;

		/* It takes 2us to stable */
		DelayUs(2) ;

		/* read calibration result */
		temp = OSC_CalResult_Get(OSC4M_CAL_CLK);
		// RTK_LOGI(TAG, "OSC4M_Calibration:cal_n %d result:%d target:%d delta:%d\n", cal_n, temp, target_40m_counter, delta);

		if (temp < target_40m_counter) {
			delta = target_40m_counter - temp;
		} else {
			delta = temp - target_40m_counter;
		}
	}
	// RTK_LOGI(TAG, "[CAL4M]:R last one: 0x%x H:0x%x L:0x%x \n", LDO_GET_FREQ_R_SEL(ldo->LDO_4M_OSC_CTRL1), LDO_GET_VCM_SEL_H(ldo->LDO_4M_OSC_CTRL1),
	// 		 LDO_GET_VCM_SEL_L(ldo->LDO_4M_OSC_CTRL1));

cal_end:
	cur_ppm = delta * 1000000 / target_40m_counter;
	RTK_LOGI(TAG, "[CAL4M]: delta:%lu target:%lu PPM: %lu PPM_Limit:%lu \n", delta, target_40m_counter, cur_ppm, ppm_limit);

	if (cur_ppm >= ppm_limit) {
		RTK_LOGW(TAG, "[CAL4M]: PPM: %lu PPM_Limit:%lu \n", cur_ppm, ppm_limit);
		assert_param(0);
	}

	return TRUE;
}

/**
  * @brief  OSC4M Init
  * @param  NA
  * @note NA
  */
void OSC4M_Init(void)
{

}

/**
  * @brief  XTAL Init
  * @note
  */
void XTAL_INIT(void)
{
	XTAL_TypeDef *xtal = XTAL_BASE;

	u32 temp;

	temp  = xtal->XTAL_ANAPAR_XTAL_OFF_0;
	temp &= ~XTAL_MASK_DRV_USB;
	/*Improve the 2440MHz spur size observed by RX*/
	temp |= XTAL_DRV_USB(0x1);
	xtal->XTAL_ANAPAR_XTAL_OFF_0 = temp;

}

void XTAL_AACK(void)
{
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
}

SRAMDRAM_ONLY_TEXT_SECTION
static u32 _PERI_ClkGet(u8 peri_ckd)
{
	u32 peri_clk;

	IPC_SEMTake(IPC_SEM_RRAM, 0xffffffff);
	if (peri_ckd & IS_SYS_PLL) {
		peri_clk = RRAM_DEV->clk_info_bk.SYSPLL_CLK / GET_CLK_DIV(peri_ckd);
	} else {
		peri_clk = RRAM_DEV->clk_info_bk.USBPLL_CLK / GET_CLK_DIV(peri_ckd);
	}
	IPC_SEMFree(IPC_SEM_RRAM);

	return peri_clk;
}

u32 USB_PLL_ClkGet(void)
{
	u32 pll_clk;

	IPC_SEMTake(IPC_SEM_RRAM, 0xffffffff);
	pll_clk = RRAM_DEV->clk_info_bk.USBPLL_CLK;
	IPC_SEMFree(IPC_SEM_RRAM);

	return pll_clk;
}

u32 SYS_PLL_ClkGet(void)
{
	u32 pll_clk;

	IPC_SEMTake(IPC_SEM_RRAM, 0xffffffff);
	pll_clk = RRAM_DEV->clk_info_bk.SYSPLL_CLK;
	IPC_SEMFree(IPC_SEM_RRAM);

	return pll_clk;
}

u32 SHPERI_ClkGet(void)
{
	return _PERI_ClkGet(RRAM_DEV->clk_info_bk.shperi_ckd);
}

u32 HPERI_ClkGet(void)
{
	return _PERI_ClkGet(RRAM_DEV->clk_info_bk.hperi_ckd);
}

u32 PSRAMC_ClkGet(void)
{
	return _PERI_ClkGet(RRAM_DEV->clk_info_bk.psramc_ckd);
}

// SRAMDRAM_ONLY_TEXT_SECTION
// u32 SPIC_ClkGet(void)
// {
// 	return _PERI_ClkGet(RRAM_DEV->clk_info_bk.spic_ckd);
// }

u32 VO_ClkGet(void)
{
	return _PERI_ClkGet(RRAM_DEV->clk_info_bk.vo_ckd);
}

u32 CPU_ClkGet(void)
{
	if (SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA) {
		return 40 * MHZ_TICK_CNT;
	}

	if (CKSL_HP_XTAL == RCC_PeriphClockSourceGet(HP)) {
		return XTAL_ClkGet();
	} else {
		return _PERI_ClkGet(RRAM_DEV->clk_info_bk.CPU_CKD);
	}
}

int CPU_Clk_Check(void)
{
	int ret = RTK_SUCCESS;
	u32 CpuClk, CpuClk_Src;

	if (SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA) {
		CpuClk = 40 * MHZ_TICK_CNT;
	} else {
		CpuClk_Src = RCC_PeriphClockSourceGet(HP);

		if (CpuClk_Src == CKSL_HP_XTAL) {
			CpuClk = XTAL_ClkGet();
		} else if (CpuClk_Src == CKSL_HP_USB_PLL) {
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
