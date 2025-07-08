/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "ameba_soc.h"

SDIOHCFG_TypeDef sdioh_config = {
	.sdioh_bus_speed = SD_SPEED_HS, //SD_SPEED_DS or SD_SPEED_HS
	.sdioh_bus_width = SDIOH_BUS_WIDTH_4BIT, //SDIOH_BUS_WIDTH_1BIT or SDIOH_BUS_WIDTH_4BIT
	.sdioh_clk_pin = _PA_11,	// refer to pinmux table
	.sdioh_cmd_pin = _PA_10,	// refer to pinmux table
	.sdioh_d0_pin = _PA_7,		// refer to pinmux table
	.sdioh_d1_pin = _PA_8,		// refer to pinmux table
	.sdioh_d2_pin = _PA_6,		// refer to pinmux table
	.sdioh_d3_pin = _PA_9,		// refer to pinmux table
	.sdioh_cd_pin = _PNC,		// _PNC or other gpio pin
	.sdioh_wp_pin = _PNC,		// fixed
};

/* Please do not modify this table!!! */
const u8 SDIO_PAD[7][6] = {
	/* CLK     CMD     D3      D2      D1      D0 */
	{_PA_18, _PA_19, _PA_26, _PA_4,  _PA_25, _PA_5 }, // Group0: PA4-PA5, PA18-PA19, PA25-PA26
	{_PA_18, _PA_19, _PA_21, _PA_4,  _PA_20, _PA_5 }, // Group1: PA4-PA5, PA18-PA21, default
	{_PA_18, _PA_19, _PA_21, _PA_27, _PA_20, _PA_28}, // Group2: PA18-PA21, PA27-PA28
	{_PA_18, _PA_19, _PA_26, _PA_27, _PA_25, _PA_28}, // Group3: PA18-PA19, PA25-PA28
	{_PA_11, _PA_10, _PA_9,  _PA_6,  _PA_8,  _PA_7 }, // Group4: PA6-PA11
	{_PB_23, _PB_22, _PB_16, _PB_13, _PB_15, _PB_14}, // Group5: PB13-PB16, PB22-PB23
	{_PC_7,  _PC_6,  _PC_5,  _PC_2,  _PC_4,  _PC_3 }  // Group6: PC2-PC7
};

/* sdio pinmux group: 0x0-0x6 */
u8 SDIO_Pin_Grp = 0x1;
