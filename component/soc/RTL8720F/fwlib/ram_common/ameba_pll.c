/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "PLL";

void PLL_Enable(u8 pll_name)
{
	/* RL6955 no software required to set PLL */
	UNUSED(pll_name);
}

void PLL_Disable(u8 pll_name)
{
	/* RL6955 no software required to set PLL */
	UNUSED(pll_name);
}

/**
  * @brief  Set PLL clock.
  * @param  pll_name: This parameter can be one of the following values:
  *		 @arg USB_PLL
  *		 @arg SYS_PLL
  * @param  pll_freq: This parameter is used to set PLL frequency, for example PllClk=600000000 means 600MHz
  */
void PLL_SetFreq(u8 pll_name, u32 pll_freq)
{
	/* RL6955 no software required to set PLL */
	UNUSED(pll_name);
	UNUSED(pll_freq);
}

/**
  * @brief  Get PLL clock.
  * @param  pll_name: This parameter can be one of the following values:
  *		 @arg USB_PLL
  *		 @arg SYS_PLL
  * @return  target pll freq in Hz
  */
u32 PLL_GetFreq(u8 pll_name)
{
	if (pll_name == SYS_PLL) {
		return PLL_400M;
	} else if (pll_name == USB_PLL) {
		return PLL_960M;
	} else {
		RTK_LOGE(TAG, "invalid pll name:%d \n", pll_name);
		assert_param(0);
	}
}

#ifdef RTL8720F_TODO
/**
  * @brief  I2S0 CPU PLL CLOCK auto choose when system clock is not an integer multiple of I2S PLL
  * @param  clk: CLK_98P304M/CLK_45P1584M
  */
void PLL_I2S0_CLK_Auto(u32 status, u32 clk)
{
	u32 Tmp1;
	u32 Tmp2;

	Tmp1 = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_I2S_SDM_CTRL);
	Tmp2 = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_I2S0_COMP_STEP);

	if (status == DISABLE) {

		Tmp1 &= ~LSYS_BIT_FEN_SDM_I2S0;
		Tmp1 |= LSYS_MASK_CKD_SDM_I2S0;
		Tmp2 |= LSYS_MASK_COMP_STEP_I2S0;
	} else {
		Tmp1 |= LSYS_BIT_FEN_SDM_I2S0;

		if (PLL_Calibration(clk)) {
			Tmp1 &= ~(LSYS_BIT_COMP_SEL_I2S0 | LSYS_MASK_CKD_SDM_I2S0);
			Tmp1 |= LSYS_COMP_SEL_I2S0(1) | LSYS_CKD_SDM_I2S0(pll_sdm_reg - 1);
		} else {

			Tmp1 &= ~(LSYS_BIT_COMP_SEL_I2S0 | LSYS_MASK_CKD_SDM_I2S0);
			Tmp1 |= LSYS_COMP_SEL_I2S0(0) | LSYS_CKD_SDM_I2S0(pll_sdm_reg);
		}

		Tmp2 &= ~LSYS_MASK_COMP_STEP_I2S0;
		Tmp2 |= LSYS_COMP_STEP_I2S0(un_err_normalization);
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_I2S_SDM_CTRL, Tmp1);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_I2S0_COMP_STEP, Tmp2);
}

/**
  * @brief  I2S0 CPU PLL CLOCK choose when system clock is an integer multiple of I2S PLL
  * @param  status: ENABLE/DISABLE
  * @param  clk: CLK_98P304M/CLK_45P1584M
  */
void PLL_I2S0_CLK(u32 status, u32 clk)

{
	u32 Tmp1;
	u32 Tmp2;

	Tmp1 = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_I2S_SDM_CTRL);
	Tmp2 = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_I2S0_COMP_STEP);

	if (status == DISABLE) {

		Tmp1 &= ~LSYS_BIT_FEN_SDM_I2S0;
		Tmp1 |= LSYS_MASK_CKD_SDM_I2S0;

		Tmp2 |= LSYS_MASK_COMP_STEP_I2S0;

	} else {
		Tmp1 |= LSYS_BIT_FEN_SDM_I2S0;

		Tmp1 &= ~LSYS_BIT_COMP_SEL_I2S0;
		Tmp1 |= LSYS_COMP_SEL_I2S0(1);

		if (clk == CLK_98P304M) {

			Tmp1 &= ~LSYS_MASK_CKD_SDM_I2S0;
			Tmp1 |= LSYS_CKD_SDM_I2S0(6);           // for 688P128M

			Tmp2 &= ~LSYS_MASK_COMP_STEP_I2S0;
			Tmp2 |= LSYS_COMP_STEP_I2S0(0);
			I2S_PLL_flag = 98;

		} else {
			Tmp1 &= ~LSYS_MASK_CKD_SDM_I2S0;
			Tmp1 |= LSYS_CKD_SDM_I2S0(14);  		// for 677P376M

			Tmp2 &= ~LSYS_MASK_COMP_STEP_I2S0;
			Tmp2 |= LSYS_COMP_STEP_I2S0(0);
			I2S_PLL_flag = 45;
		}
	}

	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_I2S_SDM_CTRL, Tmp1);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_I2S0_COMP_STEP, Tmp2);

}

/**
  * @brief  I2S PLL output adjust by ppm.
  * @param  ppm: ~0.88697ppm per FOF step
  * @param  action: slower or faster or reset
  *          This parameter can be one of the following values:
  *            @arg PLL_AUTO: reset to auto 98.304M
  *            @arg PLL_FASTER: pll clock faster than 98.304M
  *            @arg PLL_SLOWER: pll clock slower than 98.304M
  */
float PLL_I2S_98P304M_ClkTune(u32 pll_sel, float ppm, u32 action)
{
	(void)pll_sel;
	u32 F0F_new;
	float real_ppm = 0;
	double step = 0.886973813872093;
	u32 F0F_base = 5125;

	PLL_TypeDef *PLL = (PLL_TypeDef *)PLL_BASE;

	if (action == PLL_FASTER) {                    /* VCO:688P128M VCO/7=98.304M */
		F0F_new = F0F_base + (u32)((double)ppm / step + 0.5);     /* maximum of register is 8191,so ppm<=2719 */
		real_ppm = (double)((double)F0F_new - (double)F0F_base) * step;
	} else if (action == PLL_SLOWER) {
		F0F_new = F0F_base - (u32)((double)ppm / step + 0.5);
		real_ppm = (double)((double)F0F_new - (double)F0F_base) * step;
	} else {
		F0F_new = F0F_base;
	}

	PLL->PLL_CTRL3 &= ~(PLL_MASK_F0F_SDM | PLL_MASK_F0N_SDM);
	PLL->PLL_CTRL3 |= PLL_F0F_SDM(F0F_new) | PLL_F0N_SDM(1);

	/* reset BIT_PLL_I2S_TRIG_RREQ_EN */
	PLL->PLL_CTRL1 |= PLL_BIT_TRIG_FREQ;
	PLL->PLL_CTRL1 &= ~PLL_BIT_TRIG_FREQ;

	return real_ppm;
}

/**
  * @brief  I2S PLL output adjust by ppm.
  * @param  ppm: ~0.90105ppm per FOF step
  * @param  action: slower or faster or reset
  *          This parameter can be one of the following values:
  *            @arg PLL_AUTO: reset to auto 45.1584M
  *            @arg PLL_FASTER: pll clock faster than 45.1584M
  *            @arg PLL_SLOWER: pll clock slower than 45.1584M
  */
float PLL_I2S_45P158M_ClkTune(u32 pll_sel, float ppm, u32 action)
{
	(void)pll_sel;
	u32 F0F_new;
	float real_ppm = 0;
	double step = 0.901052699869257;
	u32 F0F_base = 3893;

	PLL_TypeDef *PLL = (PLL_TypeDef *)PLL_BASE;

	if (action == PLL_FASTER) {                  /* VCO:677P376M VCO/15=45.1584M */
		F0F_new = F0F_base + (u32)((double)ppm / step + 0.5);   /* maximum of register is 8191,so ppm<=3872 */
		real_ppm = (double)((double)F0F_new - (double)F0F_base) * step;
	} else if (action == PLL_SLOWER) {
		F0F_new = F0F_base - (u32)((double)ppm / step + 0.5);
		real_ppm = (double)((double)F0F_new - (double)F0F_base) * step;
	} else {
		F0F_new = F0F_base;
	}

	PLL->PLL_CTRL3 &= ~(PLL_MASK_F0F_SDM | PLL_MASK_F0N_SDM);
	PLL->PLL_CTRL3 |= PLL_F0F_SDM(F0F_new) | PLL_F0N_SDM(7);

	/* reset BIT_PLL_I2S_TRIG_RREQ_EN */
	PLL->PLL_CTRL1 |= PLL_BIT_TRIG_FREQ;
	PLL->PLL_CTRL1 &= ~PLL_BIT_TRIG_FREQ;

	return real_ppm;
}
#endif
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
