/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/**
  * @brief  PLL divider set
  * @param  div: Division
  */
void PLL_I2S_Div(u32 div)
{
	assert_param(div >= 3);

	u32 RegValue = 0;
	RegValue = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP1);
	RegValue &= ~(LSYS_MASK_CKD_I2S_PLL);
	RegValue |= LSYS_CKD_I2S_PLL(div - 1);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP1, RegValue);
}


/**
  * @brief  Enable or disable I2S(98.304MHz)
  * @param  NewState: DISABLE/ENABLE
  */
void PLL_I2S_98P304M(u32 pll_sel, u32 NewState)
{
	if (NewState == ENABLE) {
		u32 RegValue = 0;
		RegValue = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP1);
		RegValue &= ~(LSYS_MASK_CKD_I2S_PLL);
		if (pll_sel == CKSL_I2S_CPUPLL) {
			RegValue |= LSYS_CKD_I2S_PLL(3);
		} else {
			RegValue |= LSYS_CKD_I2S_PLL(4);
		}
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP1, RegValue);
	} else {
		return;
	}
}

/**
  * @brief  Enable or disable I2S(45.1584MHz)
  * @param  NewState: DISABLE/ENABLE
  */
void PLL_I2S_45P158M(u32 pll_sel, u32 NewState)
{
	if (NewState == ENABLE) {
		u32 RegValue = 0;
		RegValue = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP1);
		RegValue &= ~(LSYS_MASK_CKD_I2S_PLL);
		if (pll_sel == CKSL_I2S_CPUPLL) {
			RegValue |= LSYS_CKD_I2S_PLL(7);
		} else {
			RegValue |= LSYS_CKD_I2S_PLL(10);
		}
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP1, RegValue);
	} else {
		return;
	}
}

float PLL_I2S_98P304M_ClkTune(u32 pll_sel, float ppm, u32 action)
{

	u32 F0F_new;
	assert_param(ppm <= 1000);
	float real_ppm = 0;

	if (pll_sel == CKSL_I2S_CPUPLL) {
		PLL_BASE -> PLL_CPUPLL_CTRL1 &= (~PLL_MASK_CPUPLL_DIVN_SDM);
		PLL_BASE -> PLL_CPUPLL_CTRL1 |= (PLL_CPUPLL_DIVN_SDM(7));

		if (action == PLL_FASTER) {
			F0F_new = 5269 + (u32)(ppm / 1.55220);
		} else if (action == PLL_SLOWER) {
			F0F_new = 5269 - (u32)(ppm / 1.55220);
		} else {
			F0F_new = 5269;
		}

		PLL_BASE -> PLL_CPUPLL_CTRL3 &= (~PLL_MASK_CPUPLL_F0F_SDM);
		PLL_BASE -> PLL_CPUPLL_CTRL3 |= (PLL_CPUPLL_F0F_SDM(F0F_new));

		PLL_BASE -> PLL_CPUPLL_CTRL3 &= (~PLL_MASK_CPUPLL_F0N_SDM);
		PLL_BASE -> PLL_CPUPLL_CTRL3 |= (PLL_CPUPLL_F0N_SDM(6));

		PLL_BASE ->	PLL_CPUPLL_CTRL1 |= (PLL_BIT_CPUPLL_TRIG_FREQ);
		PLL_BASE ->	PLL_CPUPLL_CTRL1 &= (~PLL_BIT_CPUPLL_TRIG_FREQ);

		real_ppm = (double)((double)F0F_new - (double)5269) * (double)1000000 / (double)8192 / (double)8 / (double)((double)9 + ((double)6 + (double)5269 /
				   (double)8192) / (double)8);
	} else {
		PLL_BASE -> PLL_PERIPLL_CTRL1 &= (~PLL_MASK_PERIPLL_DIVN_SDM);
		PLL_BASE -> PLL_PERIPLL_CTRL1 |= (PLL_PERIPLL_DIVN_SDM(10));

		if (action == PLL_FASTER) {
			F0F_new = 2490 + (u32)(ppm / 1.24176);
		} else if (action == PLL_SLOWER) {
			F0F_new = 2490 - (u32)(ppm / 1.24176);
		} else {
			F0F_new = 2490;
		}

		PLL_BASE -> PLL_PERIPLL_CTRL3 &= (~PLL_MASK_PERIPLL_F0F_SDM);
		PLL_BASE -> PLL_PERIPLL_CTRL3 |= (PLL_PERIPLL_F0F_SDM(F0F_new));

		PLL_BASE -> PLL_PERIPLL_CTRL3 &= (~PLL_MASK_PERIPLL_F0N_SDM);
		PLL_BASE -> PLL_PERIPLL_CTRL3 |= (PLL_PERIPLL_F0N_SDM(2));

		PLL_BASE ->	PLL_PERIPLL_CTRL1 |= (PLL_BIT_PERIPLL_TRIG_FREQ);
		PLL_BASE ->	PLL_PERIPLL_CTRL1 &= (~PLL_BIT_PERIPLL_TRIG_FREQ);

		real_ppm = (double)((double)F0F_new - (double)2490) * (double)1000000 / (double)8192 / (double)8 / (double)((double)9 + ((double)6 + (double)5269 /
				   (double)8192) / (double)8);
	}

	return real_ppm;

}


/**
  * @brief  I2S2 PLL output adjust by ppm.
  * @param  ppm: ~1.69ppm per FOF step
  * @param  action: slower or faster or reset
  *          This parameter can be one of the following values:
  *            @arg PLL_AUTO: reset to auto 45.1584M
  *            @arg PLL_FASTER: pll clock faster than 45.1584M
  *            @arg PLL_SLOWER: pll clock slower than 45.1584M
  */
float PLL_I2S_45P158M_ClkTune(u32 pll_sel, float ppm, u32 action)
{

	u32 F0F_new;
	assert_param(ppm <= 1000);
	float real_ppm = 0;

	if (pll_sel == CKSL_I2S_CPUPLL) {
		PLL_BASE -> PLL_CPUPLL_CTRL1 &= (~PLL_MASK_CPUPLL_DIVN_SDM);
		PLL_BASE -> PLL_CPUPLL_CTRL1 |= (PLL_CPUPLL_DIVN_SDM(7));

		if (action == PLL_FASTER) {
			F0F_new = 2076 + (u32)(ppm / 1.68946);
		} else if (action == PLL_SLOWER) {
			F0F_new = 2076 - (u32)(ppm / 1.68946);
		} else {
			F0F_new = 2076;
		}

		PLL_BASE -> PLL_CPUPLL_CTRL3 &= (~PLL_MASK_CPUPLL_F0F_SDM);
		PLL_BASE -> PLL_CPUPLL_CTRL3 |= (PLL_CPUPLL_F0F_SDM(F0F_new));

		PLL_BASE -> PLL_CPUPLL_CTRL3 &= (~PLL_MASK_CPUPLL_F0N_SDM);
		PLL_BASE -> PLL_CPUPLL_CTRL3 |= (PLL_CPUPLL_F0N_SDM(0));

		PLL_BASE ->	PLL_CPUPLL_CTRL1 |= (PLL_BIT_CPUPLL_TRIG_FREQ);
		PLL_BASE ->	PLL_CPUPLL_CTRL1 &= (~PLL_BIT_CPUPLL_TRIG_FREQ);

		real_ppm = (double)((double)F0F_new - (double)2076) * (double)1000000 / (double)8192 / (double)8 / (double)((double)9 + ((double)6 + (double)5269 /
				   (double)8192) / (double)8);
	} else {
		PLL_BASE -> PLL_PERIPLL_CTRL1 &= (~PLL_MASK_PERIPLL_DIVN_SDM);
		PLL_BASE -> PLL_PERIPLL_CTRL1 |= (PLL_PERIPLL_DIVN_SDM(10));

		if (action == PLL_FASTER) {
			F0F_new = 2855 + (u32)(ppm / 1.22870);
		} else if (action == PLL_SLOWER) {
			F0F_new = 2855 - (u32)(ppm / 1.22870);
		} else {
			F0F_new = 2855;
		}

		PLL_BASE -> PLL_PERIPLL_CTRL3 &= (~PLL_MASK_PERIPLL_F0F_SDM);
		PLL_BASE -> PLL_PERIPLL_CTRL3 |= (PLL_PERIPLL_F0F_SDM(F0F_new));

		PLL_BASE -> PLL_PERIPLL_CTRL3 &= (~PLL_MASK_PERIPLL_F0N_SDM);
		PLL_BASE -> PLL_PERIPLL_CTRL3 |= (PLL_PERIPLL_F0N_SDM(3));

		PLL_BASE ->	PLL_PERIPLL_CTRL1 |= (PLL_BIT_PERIPLL_TRIG_FREQ);
		PLL_BASE ->	PLL_PERIPLL_CTRL1 &= (~PLL_BIT_PERIPLL_TRIG_FREQ);

		real_ppm = (double)((double)F0F_new - (double)2855) * (double)1000000 / (double)8192 / (double)8 / (double)((double)9 + ((double)6 + (double)5269 /
				   (double)8192) / (double)8);
	}

	return real_ppm;

}


/**
  * @brief  CPU PLL open/close
  * @param  status: ENABLE/DISABLE
  */
void PLL_CPU(u32 status)
{
	PLL_TypeDef *PLL = (PLL_TypeDef *)PLL_BASE;
	if (status == DISABLE) {
		/* close CPU PLL */
		PLL->PLL_CPUPLL_CTRL0 &= ~(PLL_BIT_CPUPLL_CK_EN);
		PLL->PLL_CPUPLL_CTRL0 &= ~(PLL_BIT_CPUPLL_POW_PLL | PLL_BIT_CPUPLL_POW_DIV_IN);
		PLL->PLL_CPUPLL_PS &= (~(PLL_BIT_CPUPLL_EN_CK400M | PLL_BIT_CPUPLL_EN_CK400M_PS));
		PLL->PLL_CPUPLL_PS &= ~PLL_BIT_CPUPLL_POW_CKGEN;

		PLL->PLL_CPUPLL_CTRL0 &= ~PLL_BIT_CPUPLL_POW_ERC;
	} else {
		/* open CPU PLL */
		// CPUPLL erc enable
		PLL->PLL_CPUPLL_CTRL0 |= PLL_BIT_CPUPLL_POW_ERC;
		DelayUs(1);

		// CPUPLL power on
		PLL->PLL_CPUPLL_PS |= PLL_BIT_CPUPLL_EN_CK400M | PLL_BIT_CPUPLL_EN_CK400M_PS;
		PLL->PLL_CPUPLL_PS |= PLL_BIT_CPUPLL_POW_CKGEN;
		PLL->PLL_CPUPLL_CTRL0 |= (PLL_BIT_CPUPLL_POW_PLL | PLL_BIT_CPUPLL_POW_DIV_IN);
		PLL->PLL_CPUPLL_CTRL0 |= PLL_BIT_CPUPLL_CK_EN;

		// Wait ready
		while (!(PLL->PLL_STATE & PLL_BIT_CPU_PLL_RDY));
	}
}

/**
  * @brief  DSP PLL open/close
  * @param  status: ENABLE/DISABLE
  */
void PLL_DSP(u32 status)
{
	PLL_TypeDef *PLL = (PLL_TypeDef *)PLL_BASE;

	if (status == DISABLE) {
		PLL->PLL_PERIPLL_CTRL0 &= ~(PLL_BIT_PERIPLL_CK_EN);
		PLL->PLL_PERIPLL_CTRL0 &= ~(PLL_BIT_PERIPLL_POW_PLL | PLL_BIT_PERIPLL_POW_DIV_IN);
		PLL->PLL_PERIPLL_PS &= (~(PLL_BIT_PERIPLL_EN_CK400M | PLL_BIT_PERIPLL_EN_CK400M_PS));
		PLL->PLL_PERIPLL_PS &= ~PLL_BIT_PERIPLL_POW_CKGEN;

		PLL->PLL_PERIPLL_CTRL0 &= ~PLL_BIT_PERIPLL_POW_ERC;

	} else {
		// PERIPLL erc enable
		PLL->PLL_PERIPLL_CTRL0 |= PLL_BIT_PERIPLL_POW_ERC;
		DelayUs(1);

		// PERIPLL power on
		PLL->PLL_PERIPLL_PS |= PLL_BIT_PERIPLL_EN_CK400M | PLL_BIT_PERIPLL_EN_CK400M_PS;
		PLL->PLL_PERIPLL_PS |= PLL_BIT_PERIPLL_POW_CKGEN;
		PLL->PLL_PERIPLL_CTRL0 |= (PLL_BIT_PERIPLL_POW_PLL | PLL_BIT_PERIPLL_POW_DIV_IN);
		PLL->PLL_PERIPLL_CTRL0 |= PLL_BIT_PERIPLL_CK_EN;

		// Wait ready
		while (!(PLL->PLL_STATE & PLL_BIT_DSP_PLL_RDY));
	}
}

/**
  * @brief  PLL bandgap open/close
  when two plls are closed, need close PLL BG; when need to open pll, need open PLL BG first
  * @param  status: ENABLE/DISABLE
  */
void PLL_BG(u32 status)
{
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
}


/**
  * @brief  get PLL state
  * @param  pll_type: This parameter can be one of the following values:
  *		 @arg CLK_CPU_DPLL
  *		 @arg CLK_CPU_MPLL
  * @retval: TRUE: pll open; FALSE: pll close
  */
bool PLL_State(u8 pll_type)
{
	PLL_TypeDef *PLL = (PLL_TypeDef *)PLL_BASE;

	if (pll_type == CLK_CPU_MPLL) {
		return PLL->PLL_STATE & PLL_BIT_CPU_PLL_RDY;
	} else if (pll_type == CLK_CPU_DPLL) {
		return PLL->PLL_STATE & PLL_BIT_DSP_PLL_RDY;
	} else {
		/* error param return */
		return FALSE;
	}
}


/**
  * @brief  Set PLL clock.
  * @param  pll_type: This parameter can be one of the following values:
  *		 @arg CLK_CPU_DPLL
  *		 @arg CLK_CPU_MPLL
  * @param  PllClk: This parameter is used to set PLL frequency, for example PllClk=600000000 means 600MHz
  */
void PLL_ClkSet(u8 pll_type, u32 PllClk)
{
	PLL_TypeDef *PLL = (PLL_TypeDef *)PLL_BASE;
	u32 Div, FoF, FoN, Temp;
	u32 XtalClk = XTAL_ClkGet();

	//PllClk = Div * XtalClk + (FoN + FoF >> 13) / 8 * XtalClk;
	Div = PllClk / XtalClk;

	Temp = PllClk - Div * XtalClk; //FoN represents [0, XtalClk)
	FoN = Temp / (XtalClk >> 3);

	Temp = Temp - FoN * (XtalClk >> 3); //FoF represents [0, XtalClk/8)
	FoF = (u32)(((u64)Temp << 13) / (XtalClk >> 3));

	if (pll_type == CLK_CPU_MPLL) {
		Temp = PLL->PLL_CPUPLL_CTRL1;
		Temp &= ~PLL_MASK_CPUPLL_DIVN_SDM;
		Temp |= PLL_CPUPLL_DIVN_SDM(Div - 2);
		PLL->PLL_CPUPLL_CTRL1 = Temp;

		Temp = PLL->PLL_CPUPLL_CTRL3;
		Temp &= ~(PLL_MASK_CPUPLL_F0F_SDM | PLL_MASK_CPUPLL_F0N_SDM);
		Temp |= PLL_CPUPLL_F0F_SDM(FoF) | PLL_CPUPLL_F0N_SDM(FoN);
		PLL->PLL_CPUPLL_CTRL3 = Temp;

		DelayUs(1);
		PLL->PLL_CPUPLL_CTRL1 |= PLL_BIT_CPUPLL_TRIG_FREQ;
		PLL->PLL_CPUPLL_CTRL1 &= ~PLL_BIT_CPUPLL_TRIG_FREQ;
		DelayUs(30);
	} else if (pll_type == CLK_CPU_DPLL) {
		Temp = PLL->PLL_PERIPLL_CTRL1;
		Temp &= ~PLL_MASK_PERIPLL_DIVN_SDM;
		Temp |= PLL_PERIPLL_DIVN_SDM(Div - 2);
		PLL->PLL_PERIPLL_CTRL1 = Temp;

		Temp = PLL->PLL_PERIPLL_CTRL3;
		Temp &= ~(PLL_MASK_PERIPLL_F0F_SDM | PLL_MASK_PERIPLL_F0N_SDM);
		Temp |= PLL_PERIPLL_F0F_SDM(FoF) | PLL_PERIPLL_F0N_SDM(FoN);
		PLL->PLL_PERIPLL_CTRL3 = Temp;

		DelayUs(1);
		PLL->PLL_PERIPLL_CTRL1 |= PLL_BIT_PERIPLL_TRIG_FREQ;
		PLL->PLL_PERIPLL_CTRL1 &= ~PLL_BIT_PERIPLL_TRIG_FREQ;
		DelayUs(30);
	} else {
		/* error param return */
		return ;
	}
}