/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include <math.h>

#define MILLION_DIV    1000000
#define KILO_MUL   	   1000

enum PLL_Clk {
	CLK_98P304M = 0,
	CLK_45P1584M
};

volatile u32 I2S_PLL_flag = 0;
u32 pll_sdm_reg;
u32 un_err_normalization;

bool PLL_Calibration(u32 dst_clk)
{
	u32 sys_pll;
	double des_clk = 0;
	double target_per;
	double base_per;
	double intp_per;
	double err_accu;
	double err_comp;

	sys_pll = PLL_ClkGet();

	if (dst_clk == CLK_98P304M) {

		des_clk = 98.304;
		I2S_PLL_flag = 98;

	} else if (dst_clk == CLK_45P1584M) {
		des_clk = 45.1584;
		I2S_PLL_flag = 45;
	}

	pll_sdm_reg = sys_pll / des_clk / MILLION_DIV;

	/* calculate compensation normalized step */
	target_per = (KILO_MUL * 1.0) / des_clk;
	base_per = (KILO_MUL * 1.0 * pll_sdm_reg) / (sys_pll / MILLION_DIV);
	intp_per = (KILO_MUL * 1.0 * (pll_sdm_reg + 1)) / (sys_pll / MILLION_DIV);

	err_accu = target_per - base_per;
	err_comp = intp_per - target_per;

	/* convert the decimal into hexadecimal.change format to unsigned Q0.26,which is floor(normalization*(1<<26)) */
	un_err_normalization = (u32)((err_accu < err_comp ? (err_accu / err_comp) : (err_comp / err_accu)) * (1 << 26));
	return (err_accu < err_comp);
}

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
  * @brief  I2S1 CPU PLL CLOCK auto choose when system clock is not an integer multiple of I2S PLL
  * @param  clk: CLK_98P304M/CLK_45P1584M
  */
void PLL_I2S1_CLK_Auto(u32 status, u32 clk)
{
	u32 Tmp1;
	u32 Tmp2;

	Tmp1 = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_I2S_SDM_CTRL);
	Tmp2 = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_I2S1_COMP_STEP);

	if (status == DISABLE) {

		Tmp1 &= ~LSYS_BIT_FEN_SDM_I2S1;
		Tmp1 |= LSYS_MASK_CKD_SDM_I2S1;
		Tmp2 |= LSYS_MASK_COMP_STEP_I2S1;
	} else {
		Tmp1 |= LSYS_BIT_FEN_SDM_I2S1;

		if (PLL_Calibration(clk)) {
			Tmp1 &= ~(LSYS_BIT_COMP_SEL_I2S1 | LSYS_MASK_CKD_SDM_I2S1);
			Tmp1 |= LSYS_COMP_SEL_I2S1(1) | LSYS_CKD_SDM_I2S1(pll_sdm_reg - 1);
		} else {

			Tmp1 &= ~(LSYS_BIT_COMP_SEL_I2S1 | LSYS_MASK_CKD_SDM_I2S1);
			Tmp1 |= LSYS_COMP_SEL_I2S1(0) | LSYS_CKD_SDM_I2S1(pll_sdm_reg);
		}

		Tmp2 &= ~LSYS_MASK_COMP_STEP_I2S1;
		Tmp2 |= LSYS_COMP_STEP_I2S1(un_err_normalization);
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_I2S_SDM_CTRL, Tmp1);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_I2S1_COMP_STEP, Tmp2);
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
  * @brief  I2S1 CPU PLL CLOCK choose when system clock is an integer multiple of I2S PLL
  * @param  status: ENABLE/DISABLE
  * @param  clk: CLK_98P304M/CLK_45P1584M
  */
void PLL_I2S1_CLK(u32 status, u32 clk)
{
	u32 Tmp1;
	u32 Tmp2;

	Tmp1 = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_I2S_SDM_CTRL);
	Tmp2 = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_I2S1_COMP_STEP);

	if (status == DISABLE) {

		Tmp1 &= ~LSYS_BIT_FEN_SDM_I2S1;
		Tmp1 |= LSYS_MASK_CKD_SDM_I2S1;

		Tmp2 |= LSYS_MASK_COMP_STEP_I2S1;

	} else {
		Tmp1 |= LSYS_BIT_FEN_SDM_I2S1;

		Tmp1 &= ~LSYS_BIT_COMP_SEL_I2S1;
		Tmp1 |= LSYS_COMP_SEL_I2S1(1);

		if (clk == CLK_98P304M) {
			Tmp1 &= ~LSYS_MASK_CKD_SDM_I2S1;
			Tmp1 |= LSYS_CKD_SDM_I2S1(6);           // for 688P128M

			Tmp2 &= ~LSYS_MASK_COMP_STEP_I2S1;
			Tmp2 |= LSYS_COMP_STEP_I2S1(0);
			I2S_PLL_flag = 98;
		} else {

			Tmp1 &= ~LSYS_MASK_CKD_SDM_I2S1;
			Tmp1 |= LSYS_CKD_SDM_I2S1(14);         // for 677P376M

			Tmp2 &= ~LSYS_MASK_COMP_STEP_I2S1;
			Tmp2 |= LSYS_COMP_STEP_I2S1(0);
			I2S_PLL_flag = 45;
		}
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_I2S_SDM_CTRL, Tmp1);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_I2S1_COMP_STEP, Tmp2);

}

/**
  * @brief  I2S0/1 CPU PLL CLOCK choose when system clock is an integer multiple of I2S PLL
  * @param  index: 0/1
  * @param  status: ENABLE/DISABLE
  * @param  clk: CLK_98P304M/CLK_45P1584M
  */
void PLL_I2S_CLK_DIV(u32 index, u32 status, u32 clk)
{
	if (index == 0) {
		PLL_I2S0_CLK(status, clk);
	} else {
		PLL_I2S1_CLK(status, clk);
	}
}

/**
  * @brief  Get I2S PLL clock.
  * @retval  can be one of the following values:
  *          98:98.304M
  *          45:45.1584M
  */

u32 PLL_I2S_CLKGet(void)
{
	return I2S_PLL_flag;
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

/**
  * @brief  CPU PLL open/close
  * @param  status: ENABLE/DISABLE
  */
void PLL_Cmd(u32 status)
{
	PLL_TypeDef *PLL = (PLL_TypeDef *)PLL_BASE;
	if (status == DISABLE) {
		/* close CPU PLL */
		PLL->PLL_CTRL0 &= ~(PLL_BIT_REG_CK_EN);
		PLL->PLL_CTRL0 &= ~(PLL_BIT_POW_PLL);
		PLL->PLL_PS &= (~(PLL_BIT_EN_CK400M | PLL_BIT_EN_CK400M_PS));
		PLL->PLL_PS &= ~PLL_BIT_POW_CKGEN;

		PLL->PLL_CTRL0 &= ~PLL_BIT_POW_ERC;
	} else {
		/* open CPU PLL */
		// CPUPLL erc enable
		PLL->PLL_CTRL0 |= PLL_BIT_POW_ERC;
		DelayUs(1);

		// CPUPLL power on
		PLL->PLL_PS |= PLL_BIT_EN_CK400M | PLL_BIT_EN_CK400M_PS;
		PLL->PLL_PS |= PLL_BIT_POW_CKGEN;
		PLL->PLL_CTRL0 |= PLL_BIT_POW_PLL;
		PLL->PLL_CTRL0 |= PLL_BIT_REG_CK_EN;

		// Wait ready
		if (SYSCFG_CHIPType_Get() != CHIP_TYPE_FPGA) {//PMC_TODO:further no longer check
			while (!(PLL->PLL_STATE & PLL_BIT_CKRDY_PLL));
		}
	}
}

/**
  * @brief  Set PLL clock.
  * @param  PllClk: This parameter is used to set PLL frequency, for example PllClk=600000000 means 600MHz
  */
void PLL_ClkSet(u32 PllClk)
{
	PLL_TypeDef *PLL = (PLL_TypeDef *)PLL_BASE;
	u32 Div, FoF, FoN, Temp;
	u32 XtalClk = XTAL_ClkGet();

	PLL_Cmd(DISABLE);

	if (PllClk > PLL_600M) {
		//PLL increased to 700M.
		PLL->PLL_CTRL0 &= ~PLL_MASK_REG_RS_SET;
		PLL->PLL_CTRL0 |= PLL_REG_RS_SET(PLL_RS_5K);
	} else {
		PLL->PLL_CTRL0 &= ~PLL_MASK_REG_RS_SET;
		PLL->PLL_CTRL0 |= PLL_REG_RS_SET(PLL_RS_3K_XTAL_26M);
	}

	PLL_Cmd(ENABLE);

	//PllClk = Div * XtalClk + (FoN + FoF >> 13) / 8 * XtalClk;
	Div = PllClk / XtalClk;

	Temp = PllClk - Div * XtalClk; //FoN represents [0, XtalClk)
	FoN = Temp / (XtalClk >> 3);

	Temp = Temp - FoN * (XtalClk >> 3); //FoF represents [0, XtalClk/8)
	FoF = (u32)(((u64)Temp << 13) / (XtalClk >> 3));

	/* gated PLL clock output */
	PLL->PLL_CTRL0 &= ~PLL_BIT_REG_CK_EN;

	Temp = PLL->PLL_CTRL1;
	Temp &= ~PLL_MASK_DIVN_SDM;
	Temp |= PLL_DIVN_SDM(Div - 2);
	PLL->PLL_CTRL1 = Temp;

	Temp = PLL->PLL_CTRL3;
	Temp &= ~(PLL_MASK_F0F_SDM | PLL_MASK_F0N_SDM);
	Temp |= PLL_F0F_SDM(FoF) | PLL_F0N_SDM(FoN);
	PLL->PLL_CTRL3 = Temp;

	DelayUs(1);
	PLL->PLL_CTRL1 |= PLL_BIT_TRIG_FREQ;
	PLL->PLL_CTRL1 &= ~PLL_BIT_TRIG_FREQ;

	/*frequency adjustment: 40M wait 20us, 26M wait 31us*/
	DelayUs(60);

	/* enable PLL clock output */
	PLL->PLL_CTRL0 |= PLL_BIT_REG_CK_EN;
}


/**
  * @brief  PLL bandgap open/close
  when two plls are closed, need close PLL BG; when need to open pll, need open PLL BG first
  * @param  status: ENABLE/DISABLE
  */
void PLL_BG_Cmd(u32 status)
{
#if 0
	PLL_TypeDef *PLL = (PLL_TypeDef *)PLL_BASE;

	if (status == DISABLE) {
		/* if CPU PLL/ PERI PLL/ADC  all power off, need to close BandGap */
		PLL->PLL_AUX_BG &= ~(PLL_BIT_POW_BG | PLL_BIT_POW_MBIAS);
		PLL->PLL_AUX_BG |= PLL_BIT_POW_I;
	} else {
		//Check BandGap power on
		if ((PLL->PLL_AUX_BG & PLL_BG_POW_MASK) == 0) {
			PLL->PLL_AUX_BG |= (PLL_BIT_POW_BG | PLL_BIT_POW_MBIAS);
			DelayUs(20);
			PLL->PLL_AUX_BG &= (~PLL_BIT_POW_I);
			DelayUs(140);
		}
	}
#else
	UNUSED(status);
#endif
}


/**
  * @brief  get PLL state
  * @param  pll_type: This parameter can be one of the following values:
  *		 @arg CLK_CPU_PLL
  * @retval: TRUE: pll open; FALSE: pll close
  */
bool PLL_State(void)
{
	PLL_TypeDef *PLL = (PLL_TypeDef *)PLL_BASE;

	return PLL->PLL_STATE & PLL_BIT_CKRDY_PLL;

}
