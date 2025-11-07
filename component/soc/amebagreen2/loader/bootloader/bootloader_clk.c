/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

void BOOT_SOC_ClkChk(const SocClk_Info_TypeDef *pSocClk_Info)
{
	u32 cpu_clk;

	assert_param(IS_USB_PLL_RANGE(pSocClk_Info->USBPLL_CLK));
	assert_param(IS_SYS_PLL_RANGE(pSocClk_Info->SYSPLL_CLK));
	assert_param(IS_CPU_CLK_DIV_RANGE(pSocClk_Info->CPU_CKD & (~IS_SYS_PLL)));
	assert_param(IS_SOC_CORE_VOLTAGE(pSocClk_Info->Vol_Type));

	if (pSocClk_Info->CPU_CKD & IS_SYS_PLL) {
		cpu_clk = pSocClk_Info->SYSPLL_CLK / (pSocClk_Info->CPU_CKD & (~IS_SYS_PLL));
	} else {
		cpu_clk = pSocClk_Info->USBPLL_CLK / pSocClk_Info->CPU_CKD;
	}

	if (pSocClk_Info->Vol_Type == CORE_VOL_0P9) {
		assert_param(cpu_clk <= CLK_LIMIT_0P9V_HP);
	} else {
		assert_param(cpu_clk <= CLK_LIMIT_1P0V_HP);
	}
}

void BOOT_SOC_ClkSet(void)
{
	u32 temp;
	u8 shperi_ckd;
	u8 hperi_ckd;
	u8 psramc_ckd;
	u8 vo_ckd;

	const SocClk_Info_TypeDef *pSocClk_Info = &SocClk_Info[0];
	const u32 usb_pll_clk = pSocClk_Info->USBPLL_CLK;
	const u32 sys_pll_clk = pSocClk_Info->SYSPLL_CLK;
	const u8 core_ldo_vol = pSocClk_Info->Vol_Type;

	/* Do not Change Divider in FPGA */
	if (SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA) {
		temp = CPU_ClkGet();
		DelayClkUpdate(temp);
		return;
	}

	BOOT_SOC_ClkChk(pSocClk_Info);

	/* 1. core power confiuration */
	LDO_CoreVolSet(core_ldo_vol);

	/* 2. set sram_clk = CPU_clk = rom_clk*/
	RCC_PeriphClockDividerSet(ROM, 1);
	RCC_PeriphClockDividerSet(SRAM, 1);

	/* 3. lperi fixed to 40Mhz.*/
	RCC_PeriphClockSourceSet(LPERI, XTAL_40M);

	/* 4. reset cpu/shperi/hperi/psram/spic/i2s clk to XTAL*/
	RCC_PeriphClockSourceSet(HP, XTAL);
	temp = CPU_ClkGet();
	DelayClkUpdate(temp);

	RCC_PeriphClockSourceSet(SHPERI, XTAL);
	RCC_PeriphClockSourceSet(HPERI, XTAL);
	RCC_PeriphClockSourceSet(PSRAM, XTAL);
	RCC_PeriphClockSourceSet(SPIC, XTAL);
	RCC_PeriphClockSourceSet(I2S, XTAL);

	/* 5. enable PLL */
	PLL_SetFreq(USB_PLL, usb_pll_clk);
	if (sys_pll_clk != PLL_NONE) {
		PLL_Enable(SYS_PLL);
		PLL_SetFreq(SYS_PLL, sys_pll_clk);
	}

	/* 6. set cpu to pll */
	if (pSocClk_Info->CPU_CKD & IS_SYS_PLL) {
		RCC_PeriphClockDividerSet(SYS_PLL_HP, GET_CLK_DIV(pSocClk_Info->CPU_CKD));
		RCC_PeriphClockSourceSet(HP, SYS_PLL);
		/*Turn off the USB PLL divider switch.*/
		RCC_PeriphClockDividerFENSet(USB_PLL_HP, DISABLE);
	} else {
		RCC_PeriphClockDividerSet(USB_PLL_HP, GET_CLK_DIV(pSocClk_Info->CPU_CKD));
		RCC_PeriphClockSourceSet(HP, USB_PLL);
		/*Turn off the USB PLL divider switch.*/
		RCC_PeriphClockDividerFENSet(SYS_PLL_HP, DISABLE);
	}
	temp = CPU_ClkGet();
	DelayClkUpdate(temp);

	/* 7. set peri clk */
	/* spic clock set in flash_handshake_highspeed */

	/* for i2s, set to sys_pll */
	RCC_PeriphClockSourceSet(I2S, SYS_PLL);

	/* for shperi/hperi/psram/spic/vo, check usb_pll and sys_pll max frequency */
	/* shperi */
	if (core_ldo_vol == CORE_VOL_0P9) {
		shperi_ckd = PLL_ClkSrcGet(sys_pll_clk, usb_pll_clk, CLK_LIMIT_0P9V_SHPERI);
	} else {
		shperi_ckd = PLL_ClkSrcGet(sys_pll_clk, usb_pll_clk, CLK_LIMIT_1P0V_SHPERI);
	}
	if (shperi_ckd & IS_SYS_PLL) {
		RCC_PeriphClockDividerSet(SYS_PLL_SHPERI, GET_CLK_DIV(shperi_ckd));
		RCC_PeriphClockSourceSet(SHPERI, SYS_PLL);
		RCC_PeriphClockDividerFENSet(USB_PLL_SHPERI, DISABLE);
	} else {
		RCC_PeriphClockDividerSet(USB_PLL_SHPERI, GET_CLK_DIV(shperi_ckd));
		RCC_PeriphClockSourceSet(SHPERI, USB_PLL);
		RCC_PeriphClockDividerFENSet(SYS_PLL_SHPERI, DISABLE);
	}

	/* hperi */
	if (core_ldo_vol == CORE_VOL_0P9) {
		hperi_ckd = PLL_ClkSrcGet(sys_pll_clk, usb_pll_clk, CLK_LIMIT_0P9V_HPERI);
	} else {
		hperi_ckd = PLL_ClkSrcGet(sys_pll_clk, usb_pll_clk, CLK_LIMIT_1P0V_HPERI);
	}
	if (hperi_ckd & IS_SYS_PLL) {
		RCC_PeriphClockDividerSet(SYS_PLL_HPERI, GET_CLK_DIV(hperi_ckd));
		RCC_PeriphClockSourceSet(HPERI, SYS_PLL);
		RCC_PeriphClockDividerFENSet(USB_PLL_HPERI, DISABLE);
	} else {
		RCC_PeriphClockDividerSet(USB_PLL_HPERI, GET_CLK_DIV(hperi_ckd));
		RCC_PeriphClockSourceSet(HPERI, USB_PLL);
		RCC_PeriphClockDividerFENSet(SYS_PLL_HPERI, DISABLE);
	}

	/* psram */
	psramc_ckd = PLL_ClkSrcGet(sys_pll_clk, usb_pll_clk, CLK_LIMIT_PSRAM);
	if (psramc_ckd & IS_SYS_PLL) {
		RCC_PeriphClockDividerSet(SYS_PLL_PSRAM, GET_CLK_DIV(psramc_ckd));
		RCC_PeriphClockSourceSet(PSRAM, SYS_PLL);
		RCC_PeriphClockDividerFENSet(USB_PLL_PSRAM, DISABLE);
	} else {
		RCC_PeriphClockDividerSet(USB_PLL_PSRAM, GET_CLK_DIV(psramc_ckd));
		RCC_PeriphClockSourceSet(PSRAM, USB_PLL);
		RCC_PeriphClockDividerFENSet(SYS_PLL_PSRAM, DISABLE);
	}

	/* vo */
	vo_ckd = PLL_ClkSrcGet(sys_pll_clk, usb_pll_clk, CLK_LIMIT_VO);
	if (vo_ckd & IS_SYS_PLL) {
		RCC_PeriphClockDividerSet(SYS_PLL_VO, GET_CLK_DIV(vo_ckd));
		RCC_PeriphClockSourceSet(VO, SYS_PLL);
	} else {
		RCC_PeriphClockDividerSet(USB_PLL_VO, GET_CLK_DIV(vo_ckd));
		RCC_PeriphClockSourceSet(VO, USB_PLL);
	}

	/* 8. save clock info ro retention memory */
	IPC_SEMTake(IPC_SEM_RRAM, 0xffffffff);
	RRAM_DEV->clk_info_bk.USBPLL_CLK = pSocClk_Info->USBPLL_CLK;
	RRAM_DEV->clk_info_bk.SYSPLL_CLK = pSocClk_Info->SYSPLL_CLK;
	RRAM_DEV->clk_info_bk.Vol_Type = pSocClk_Info->Vol_Type;
	RRAM_DEV->clk_info_bk.CPU_CKD = pSocClk_Info->CPU_CKD;
	RRAM_DEV->clk_info_bk.shperi_ckd = shperi_ckd;
	RRAM_DEV->clk_info_bk.hperi_ckd = hperi_ckd;
	RRAM_DEV->clk_info_bk.psramc_ckd = psramc_ckd;
	RRAM_DEV->clk_info_bk.vo_ckd = vo_ckd;
	IPC_SEMFree(IPC_SEM_RRAM);

	assert_param(CPU_Clk_Check() == RTK_SUCCESS);
}
