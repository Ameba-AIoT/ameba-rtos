/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "PLL";

static u32 sys_pll_freq_input_get(void)
{
	u32 freq_xtal, prediv, freq_input;
	PLL_TypeDef *sys_pll = (PLL_TypeDef *)PLL_REG_BASE;

	freq_xtal = XTAL_ClkGet();
	prediv = PLL_GET_REG_PREDIV(sys_pll->PLL_CTRL0);

	freq_input = freq_xtal >> prediv;

	return freq_input;
}

static void sys_pll_set_div_reg(u32 ncode, u32 fcode)
{
	PLL_TypeDef *sys_pll = (PLL_TypeDef *)PLL_REG_BASE;

	if (fcode != 0) {
		sys_pll->PLL_SSC_CTRL3 &= ~PLL_BIT_REG_BYPASS_PI;
	} else {
		sys_pll->PLL_SSC_CTRL3 |= PLL_BIT_REG_BYPASS_PI;
	}

	sys_pll->PLL_SSC_CTRL0 &= ~PLL_MASK_NCODE_T_SCPU;
	sys_pll->PLL_SSC_CTRL0 |= PLL_NCODE_T_SCPU(ncode);
	sys_pll->PLL_SSC_CTRL0 &= ~PLL_MASK_FCODE_T_SCPU;
	sys_pll->PLL_SSC_CTRL0 |= PLL_FCODE_T_SCPU((u32)fcode);

	sys_pll->PLL_SSC_CTRL2 |= PLL_BIT_OC_EN;
	sys_pll->PLL_SSC_CTRL2 &= ~PLL_BIT_OC_EN;

	DelayUs(20);

}

static void sys_pll_enable(void)
{
	PLL_TypeDef *sys_pll = (PLL_TypeDef *)PLL_REG_BASE;

	/* 1. check xtal status */
	if (XTAL_Check_READY() != TRUE) {
		return;
	}

	/* 2. enable 3.3v power */
	sys_pll->PLL_SSC_CTRL4 |= PLL_BIT_PWC_UAHV_PLL;

	/* 3. enable 0.9v power */
	sys_pll->PLL_SSC_CTRL4 |= PLL_BIT_PWC_UALV_PLL;

	/* 4. SSC Fen */
	sys_pll->PLL_SSC_CTRL4 |= PLL_BIT_FEN_PLL;

	/* 5. enable pll */
	sys_pll->PLL_CTRL0 |= PLL_BIT_REG_CMU_EN;

	/* 6. OC_EN, load N/F parameters */
	sys_pll->PLL_SSC_CTRL2 |= PLL_BIT_OC_EN;
	sys_pll->PLL_SSC_CTRL2 &= ~PLL_BIT_OC_EN;

	/* 7. wait pll output valid */
	while (1) {
		if (HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG0) & LSYS_BIT_SYS_PLL_VALID) {
			break;
		}
	}
}

static void sys_pll_disable(void)
{
	PLL_TypeDef *sys_pll = (PLL_TypeDef *)PLL_REG_BASE;

	sys_pll->PLL_CTRL0 &= ~PLL_BIT_REG_CMU_EN;
	sys_pll->PLL_SSC_CTRL4 &= ~PLL_BIT_FEN_PLL;
	sys_pll->PLL_SSC_CTRL4 &= ~PLL_BIT_PWC_UALV_PLL;
	sys_pll->PLL_SSC_CTRL4 &= ~PLL_BIT_PWC_UAHV_PLL;
}

static void sys_pll_set_freq(u32 freq_hz)
{
	PLL_TypeDef *sys_pll = (PLL_TypeDef *)PLL_REG_BASE;
	u32 fcode, fraction, ncode, freq_input, temp;

	/* set sys_pll pre_divide, 40M -> 20M */
	temp = sys_pll->PLL_CTRL0;
	temp &= ~PLL_MASK_REG_PREDIV;
	temp |= PLL_REG_PREDIV(PLL_REG_PREDIV_2);
	sys_pll->PLL_CTRL0 = temp;

	freq_input = sys_pll_freq_input_get();
	ncode = freq_hz / freq_input - 2;

	fraction = freq_hz - (ncode + 2) * freq_input;
	fcode = (u32)((float)fraction / ((float)freq_input) * (1 << 14) + 0.5);

	sys_pll_set_div_reg(ncode, fcode);
}

static u32 sys_pll_get_freq(void)
{
	PLL_TypeDef *sys_pll = (PLL_TypeDef *)PLL_REG_BASE;
	u32 div_integer, div_decimal, freq_input;
	u32 ncode, fcode;

	freq_input = sys_pll_freq_input_get();

	ncode = PLL_GET_NCODE_T_SCPU(sys_pll->PLL_SSC_CTRL0);
	fcode = PLL_GET_FCODE_T_SCPU(sys_pll->PLL_SSC_CTRL0);

	div_decimal = fcode / 16384 * 100000;
	div_integer = (ncode + 2) * freq_input;

	return (div_decimal + div_integer);
}

void sys_pll_set_ssc(void)
{
	/* TO DO */
}

static void usb_pll_set_div_reg(u32 freq_hz)
{
	PLL_TypeDef *sys_pll = (PLL_TypeDef *)PLL_REG_BASE;
	u32 freq_xtal, reg, div;

	freq_xtal = XTAL_ClkGet();
	div = freq_hz / freq_xtal - 2;

	reg = sys_pll->PLL_UPLL_CTRL1;
	reg &= ~PLL_MASK_UPLL_N_PLL_TOP;
	reg |= PLL_UPLL_N_PLL_TOP(div);
	sys_pll->PLL_UPLL_CTRL1 = reg;
}

static void usb_pll_enable(void)
{
	PLL_TypeDef *sys_pll = (PLL_TypeDef *)PLL_REG_BASE;

	/* 1. check xtal status */
	if (XTAL_Check_READY() != TRUE) {
		return;
	}

	/* 2. enable usb aphy power */
	sys_pll->PLL_UPLL_CTRL0 |= PLL_BIT_PWC_UAPC_EN;
	DelayUs(2);

	/* 3. enable usb aphy pow_bg */
	sys_pll->PLL_UPLL_CTRL0 |= PLL_BIT_PWC_UABG_EN;
	DelayUs(10);

	/* 4. release usb aphy ISO */
	sys_pll->PLL_UPLL_CTRL0 &= ~PLL_BIT_ISO_USBA_EN;
	DelayUs(10);

	/* 5. release USB aphy reset */
	sys_pll->PLL_UPLL_CTRL0 |= PLL_BIT_USB_APHY_EN;

	/* 6. enable pll */
	sys_pll->PLL_UPLL_CTRL0 |= PLL_BIT_SYS_PLL_EN;

	/* 7. wait pll valid */
	while (1) {
		if (HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG0) & LSYS_BIT_USB_PLL_VALID) {
			break;
		}
	}
}

static void usb_pll_disable(void)
{
	PLL_TypeDef *sys_pll = (PLL_TypeDef *)PLL_REG_BASE;

	sys_pll->PLL_UPLL_CTRL0 &= ~PLL_BIT_USB_APHY_EN;
	sys_pll->PLL_UPLL_CTRL0 &= ~PLL_BIT_SYS_PLL_EN;
	sys_pll->PLL_UPLL_CTRL0 |= PLL_BIT_ISO_USBA_EN;
	sys_pll->PLL_UPLL_CTRL0 &= ~PLL_BIT_PWC_UABG_EN;
	sys_pll->PLL_UPLL_CTRL0 &= ~PLL_BIT_PWC_UAPC_EN;
}

static void usb_pll_set_freq(u32 freq_hz)
{
	usb_pll_set_div_reg(freq_hz);

	DelayUs(20);
}

static u32 usb_pll_get_freq(void)
{
	PLL_TypeDef *sys_pll = (PLL_TypeDef *)PLL_REG_BASE;
	u32 freq_xtal, div;

	div = PLL_GET_UPLL_N_PLL_TOP(sys_pll->PLL_UPLL_CTRL1);
	freq_xtal = XTAL_ClkGet();

	return ((div + 2) * freq_xtal);
}

void PLL_Enable(u8 pll_name)
{
	if (pll_name == SYS_PLL) {
		sys_pll_enable();
	} else if (pll_name == USB_PLL) {
		usb_pll_enable();
	} else {
		RTK_LOGE(TAG, "invalid pll name:%d \n", pll_name);
		assert_param(0);
	}
}

void PLL_Disable(u8 pll_name)
{
	if (pll_name == SYS_PLL) {
		sys_pll_disable();
	} else if (pll_name == USB_PLL) {
		usb_pll_disable();
	} else {
		RTK_LOGE(TAG, "invalid pll name:%d \n", pll_name);
		assert_param(0);
	}
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
	if (pll_name == SYS_PLL) {
		sys_pll_set_freq(pll_freq);
	} else if (pll_name == USB_PLL) {
		usb_pll_set_freq(pll_freq);
	} else {
		RTK_LOGE(TAG, "invalid pll name:%d \n", pll_name);
		assert_param(0);
	}
}

u32 PLL_GetFreq(u8 pll_name)
{
	if (pll_name == SYS_PLL) {
		return sys_pll_get_freq();
	} else if (pll_name == USB_PLL) {
		return usb_pll_get_freq();
	} else {
		RTK_LOGE(TAG, "invalid pll name:%d \n", pll_name);
		assert_param(0);
	}
}

/**
  * @brief  I2S_98P304M PLL output adjust by ppm.
  * @param  ppm
  * @param  action: slower or faster or reset
  *          This parameter can be one of the following values:
  *            @arg PLL_AUTO: reset to auto 98P304M
  *            @arg PLL_FASTER: pll clock faster than 98P304M
  *            @arg PLL_SLOWER: pll clock slower than 98P304M
  */
float PLL_I2S_98P304M_ClkTune(u32 pll_sel, float ppm, u32 action)
{
	(void)pll_sel;
	assert_param(ppm <= 1000);
	u32 fcode, fcode_new, ncode;
	float real_ppm = 0;

	PLL_TypeDef *sys_pll = (PLL_TypeDef *)PLL_REG_BASE;
	ncode = PLL_GET_NCODE_T_SCPU(sys_pll->PLL_SSC_CTRL0);
	fcode = PLL_GET_FCODE_T_SCPU(sys_pll->PLL_SSC_CTRL0);

	double step = (double)1000000 / ((double)fcode + (double)((ncode + 2) * 16384));

	if (action == PLL_FASTER) {
		fcode_new = fcode + (u32)((double)ppm / step + 0.5);
	} else if (action == PLL_SLOWER) {
		fcode_new = fcode - (u32)((double)ppm / step + 0.5);
	} else {
		fcode_new = fcode;
	}
	real_ppm = (double)((double) fcode_new - (double) fcode) * step;

	sys_pll_set_div_reg(ncode, fcode_new);

	return real_ppm;
}

/**
  * @brief  I2S_45P158M PLL output adjust by ppm.
  * @param  ppm
  * @param  action: slower or faster or reset
  *          This parameter can be one of the following values:
  *            @arg PLL_AUTO: reset to auto 45.1584M
  *            @arg PLL_FASTER: pll clock faster than 45.1584M
  *            @arg PLL_SLOWER: pll clock slower than 45.1584M
  */
float PLL_I2S_45P158M_ClkTune(u32 pll_sel, float ppm, u32 action)
{
	(void)pll_sel;
	assert_param(ppm <= 1000);
	u32 fcode, fcode_new, ncode;
	float real_ppm = 0;

	PLL_TypeDef *sys_pll = (PLL_TypeDef *)PLL_REG_BASE;
	ncode = PLL_GET_NCODE_T_SCPU(sys_pll->PLL_SSC_CTRL0);
	fcode = PLL_GET_FCODE_T_SCPU(sys_pll->PLL_SSC_CTRL0);

	double step = (double)1000000 / ((double)fcode + (double)((ncode + 2) * 16384));

	if (action == PLL_FASTER) {
		fcode_new = fcode + (u32)((double)ppm / step + 0.5);
	} else if (action == PLL_SLOWER) {
		fcode_new = fcode - (u32)((double)ppm / step + 0.5);
	} else {
		fcode_new = fcode;
	}
	real_ppm = (double)((double) fcode_new - (double) fcode) * step;

	sys_pll_set_div_reg(ncode, fcode_new);

	return real_ppm;
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
