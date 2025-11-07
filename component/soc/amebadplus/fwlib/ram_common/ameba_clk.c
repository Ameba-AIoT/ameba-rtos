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

	/* start calibration */
	temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_AIP_CTRL0);
	temp &= ~LSYS_MASK_CK_CAL_SEL;
	temp |= (LSYS_CK_CAL_SEL(cal_clk) | LSYS_BIT_CK_CAL_START);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_AIP_CTRL0, temp);

	/* poll calibration complete */
	while (1) {
		temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_AIP_CTRL0);
		if ((temp & LSYS_BIT_CK_CAL_START) == 0) {
			break;
		}
	}

	/* read calibration result */
	temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_AIP_CTRL0);
	return LSYS_GET_CK_CAL_RPT(temp);
}

/* setbit/clearbit is: BIT0~BIT5 defined in BIT_AON_MASK_OSC32K_RCAL_5_0 */
void OSC131_R_Set(u32 setbit, u32 clearbit)
{
	LDO_TypeDef *regu = LDO_BASE;
	u32 reg_val = regu->LDO_32K_OSC_CTRL;

	if (setbit) {
		reg_val |= LDO_RCAL(setbit);
	}

	if (clearbit) {
		reg_val &= ~LDO_RCAL(clearbit);
	}

	regu->LDO_32K_OSC_CTRL = reg_val;
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
	u32 reg_val;
	LDO_TypeDef *regu = LDO_BASE;
	/*Ensure that 131k can output stably before calibration.*/
	while (((LDO_BASE->LDO_DEBUG)&LDO_DBG_OSC_OK) == 0x0);

	/* Step1: Power on aon 128kHz OSC: 0x4200_8100[0]=1'b1 */
	/* Clear 4m calibration parameter first */
	regu->LDO_32K_OSC_CTRL &= ~LDO_MASK_RCAL;
	while (1) {
		temp = regu->LDO_32_OSC_XTAL_POW;
		if (temp & LDO_BIT_POW_32KOSC) {
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
			min_delta_r = regu->LDO_32K_OSC_CTRL;
			min_delta_r &= LDO_MASK_RCAL;
		}

		//RTK_LOGD(TAG, "[CAL131K]:cal_n %lu delta:%lu \n", cal_n, delta);
	}

	/* the last one is not the best one */
	if (delta > min_delta) {
		reg_val = regu->LDO_32K_OSC_CTRL;
		reg_val &= ~LDO_MASK_RCAL;
		reg_val |= min_delta_r;
		regu->LDO_32K_OSC_CTRL = reg_val;

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
	LDO_TypeDef *regu = LDO_BASE;

	if (setbit) {
		regu->LDO_4M_OSC_CTRL1 |= LDO_FREQ_R_SEL(setbit);
	}

	if (clearbit) {
		regu->LDO_4M_OSC_CTRL1 &= ~LDO_FREQ_R_SEL(clearbit);
	}

	/* It takes 2us to stable */
	DelayUs(2);
}

/* bitnum is: 0~3 defined in REGU_MASK_VCM_SEL_H/L */
void OSC4M_VCM_Set(u32 value)
{
	u32 temp;

	LDO_TypeDef *regu = LDO_BASE;
	temp = regu->LDO_4M_OSC_CTRL1;

	temp &= ~(LDO_MASK_VCM_SEL_L | LDO_MASK_VCM_SEL_H);
	temp |= (LDO_VCM_SEL_L(value) | LDO_VCM_SEL_H(value));
	regu->LDO_4M_OSC_CTRL1 = temp;


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
	LDO_TypeDef *regu = LDO_BASE;
	/* Step1: Power on aon 4MHz OSC */
	regu->LDO_RF_EN_DUMMY |= LDO_BIT_REG_POW_4MOSC;
	//RTK_LOGD(TAG, "regu->LDO_RF_EN_DUMMY=0x%lx\n",(regu->LDO_RF_EN_DUMMY));

	/* Step2: Clear 4m calibration parameter first */
	regu->LDO_4M_OSC_CTRL1 &= (u16)(~(LDO_MASK_VCM_SEL_L | LDO_MASK_VCM_SEL_H));
	//RTK_LOGD(TAG, "regu->REGU_4MOSC0=0x%lx\n",(regu->LDO_4M_OSC_CTRL1));

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
		//RTK_LOGD(TAG, "[CAL4M]:cal_n %lu H:0x%lx L:0x%lx result:%lu ", cal_n,LDO_GET_VCM_SEL_H(regu->LDO_4M_OSC_CTRL1),LDO_GET_VCM_SEL_L(regu->LDO_4M_OSC_CTRL1),temp);

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
			min_delta_r = regu->LDO_4M_OSC_CTRL1;
			min_delta_r &= (LDO_MASK_VCM_SEL_L | LDO_MASK_VCM_SEL_H | LDO_MASK_FREQ_R_SEL);
		}
		//RTK_LOGD(TAG, "delta:%lu min_delta:%lu min_delta_r:0x%lx \n", delta,min_delta,min_delta_r);
	}

	/* the last one is not the best one */
	if (delta > min_delta) {
		temp = regu->LDO_4M_OSC_CTRL1;
		temp &= ~(LDO_MASK_VCM_SEL_L | LDO_MASK_VCM_SEL_H);
		temp |= min_delta_r;
		regu->LDO_4M_OSC_CTRL1 = temp;
	}
	//RTK_LOGD(TAG, "[CAL4M]:hL last one: 0x%lx 0x%lx temp:%lx \n", REGU_GET_VCM_SEL_H(regu->REGU_4MOSC0),REGU_GET_VCM_SEL_L(regu->REGU_4MOSC0),temp);
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
	regu->LDO_4M_OSC_CTRL1 &= (u16)(~LDO_MASK_FREQ_R_SEL);
	for (cal_n = 1; cal_n <= 8; cal_n++) {
		rsel_bit_index = 8 - cal_n;
		OSC4M_R_Set(BIT(rsel_bit_index), clearbit);

		/* read calibration result */
		temp = OSC_CalResult_Get(OSC4M_CAL_CLK);
		//RTK_LOGD(TAG, "[CAL4M]:cal_n %lu R:0x%lx result:%lu ", cal_n,LDO_GET_FREQ_R_SEL(regu->LDO_4M_OSC_CTRL1), temp, target_40m_counter, delta);

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
			min_delta_r = regu->LDO_4M_OSC_CTRL1;
			min_delta_r &= LDO_MASK_FREQ_R_SEL;
		}
		//RTK_LOGD(TAG, "delta:%lu \n", delta);
	}

	/* the last one is not the best one */
	if (delta > min_delta) {
		temp = regu->LDO_4M_OSC_CTRL1;
		temp &= ~LDO_MASK_FREQ_R_SEL;
		temp |= min_delta_r;
		regu->LDO_4M_OSC_CTRL1 = temp;

		/* It takes 2us to stable */
		DelayUs(2) ;

		/* read calibration result */
		temp = OSC_CalResult_Get(OSC4M_CAL_CLK);
		//RTK_LOGD(TAG, "OSC4M_Calibration:cal_n %lu result:%lu target:%lu delta:%lu\n", cal_n, temp, target_40m_counter, delta);

		if (temp < target_40m_counter) {
			delta = target_40m_counter - temp;
		} else {
			delta = temp - target_40m_counter;
		}
	}
	//RTK_LOGD(TAG, "[CAL4M]:R last one: 0x%lx H:0x%lx L:0x%lx \n",LDO_GET_FREQ_R_SEL(regu->LDO_4M_OSC_CTRL1),LDO_GET_VCM_SEL_H(regu->LDO_4M_OSC_CTRL1),LDO_GET_VCM_SEL_H(regu->LDO_4M_OSC_CTRL1));

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
  * @note This is the special setting of amebadplus. Special settings are required because the XTAL default parameter is not optimal.
  */
void XTAL_INIT(void)
{
	XTAL_TypeDef *xtal = XTAL_BASE;

	xtal->XTAL_ANAPAR_XTAL_ON_0 = 0x008103FF; /*04h*/
	xtal->XTAL_ANAPAR_XTAL_ON_1 = 0xFC3B9E80; /*08h*/
	xtal->XTAL_ANAPAR_XTAL_ON_2 = 0x00000BE9; /*0ch*/
	xtal->XTAL_ANAPAR_XTAL_OFF_0 = 0xCC0E0190; /*10h*/
	xtal->XTAL_ANAPAR_XTAL_OFF_1 = 0x00000068; /*14h*/
	xtal->XTAL_ANAPAR_XTAL_OFF_2 = 00000006; /*18h*/

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

void OSC131K_Reset(void)
{
	/*reset osc 131k counter, only for RTL6845*/
	LDO_BASE->LDO_32_OSC_XTAL_POW &= ~LDO_BIT_POW_32KOSC;
	LDO_BASE->LDO_32_OSC_XTAL_POW |= LDO_BIT_POW_32KOSC;
}