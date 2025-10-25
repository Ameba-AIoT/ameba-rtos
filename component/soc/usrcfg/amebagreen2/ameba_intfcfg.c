/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

SDIOHCFG_TypeDef sdioh_config = {
	.sdioh_bus_speed = SD_SPEED_HS, //SD_SPEED_DS or SD_SPEED_HS
	.sdioh_bus_width = SDIOH_BUS_WIDTH_4BIT, //SDIOH_BUS_WIDTH_1BIT or SDIOH_BUS_WIDTH_4BIT
	.sdioh_cd_pin = _PNC,		// _PNC or other gpio pin
	.sdioh_wp_pin = _PNC,		// fixed
};

/* Please do not modify this table!!! */
/* applied both to sdio and sdh */
const u8 SDIO_PAD[7][6] = {
	/* CLK     CMD     D0      D1      D2      D3 */
	{_PA_18, _PA_19, _PA_5,  _PA_25, _PA_4,  _PA_26}, // Group0: PA4-PA5, PA18-PA19, PA25-PA26
	{_PA_18, _PA_19, _PA_5,  _PA_20, _PA_4,  _PA_21}, // Group1: PA4-PA5, PA18-PA21, default
	{_PA_18, _PA_19, _PA_28, _PA_20, _PA_27, _PA_21}, // Group2: PA18-PA21, PA27-PA28
	{_PA_18, _PA_19, _PA_28, _PA_25, _PA_27, _PA_26}, // Group3: PA18-PA19, PA25-PA28
	{_PA_11, _PA_10, _PA_7,  _PA_8,  _PA_6,  _PA_9 }, // Group4: PA6-PA11
	{_PB_23, _PB_22, _PB_14, _PB_15, _PB_13, _PB_16}, // Group5: PB13-PB16, PB22-PB23
	{_PC_7,  _PC_6,  _PC_3,  _PC_4,  _PC_2,  _PC_5 }  // Group6: PC2-PC7
};

const u8 ETHERNET_PAD[4][11] = {
	/* RXERR    CRS    TXEN    TXD1    TXD0    REF_CL    RXD1    RXD0    MDC    MDIO    EXTCLK */
	//MDC MDIO EXTCLK can be modified to pin as needed. Please refer to pinmux spec.xls
	{_PB_4, _PB_5, _PB_6, _PB_7,  _PB_8, _PB_9, _PB_10, _PB_11, _PB_12, _PB_3, _PA_12 },
	{_PB_17, _PB_14, _PB_15, _PB_16,  _PB_13, _PB_18, _PB_19, _PB_21, _PB_22, _PB_23, _PB_24 },
	{_PB_25, _PB_26, _PB_27, _PB_28,  _PB_29, _PB_30, _PB_31, _PC_0, _PB_12, _PB_3, _PA_12 },
	{_PB_17, _PC_5, _PC_2, _PC_3,  _PC_4, _PB_18, _PC_6, _PC_7, _PA_25, _PA_26, _PB_19 },
};

/* sdio pinmux group: 0x0-0x6 */
u8 SDIO_Pin_Grp = 0x1;

/* sd host pinmux group: 0x0-0x6 */
u8 SDH_Pin_Grp = 0x4;

/* ethernet pinmux group: 0x0-0x3 */
u8 ETHERNET_Pin_Grp = 0x0;


