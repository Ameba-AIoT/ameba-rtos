/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "ameba_soc.h"

/* Please do not modify this table!!! */
const u8 SDIO_PAD[5][6] = {
	/* CLK     CMD     D3      D2      D1      D0 */
	{_PA_16, _PA_15, _PA_14, _PA_13, _PA_18, _PA_17}, // Group0: PA13-PA18
	{_PA_29, _PA_28, _PA_27, _PA_26, _PA_31, _PA_30}, // Group1: PA26-PA31
	{_PB_9,  _PB_8,  _PB_7,  _PB_6,  _PB_14, _PB_13}, // Group2: PB6-PB9, PB13-PB14, default
	{_PB_19, _PB_18, _PB_17, _PA_12, _PB_21, _PB_20}, // Group3: PA12, PB17-PB21
	{_PB_26, _PB_25, _PB_24, _PB_23, _PB_28, _PB_27}  // Group4: PB23-PB28
};

/* sdio pinmux group: 0x0-0x4 */
/* Please refer to SDIO_PAD for more details. */
u8 SDIO_Pin_Grp = 0x2;