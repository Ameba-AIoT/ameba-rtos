/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/* Please do not modify this table!!! */
const u8 SDIO_PAD[2][6] = {
	/* CLK     CMD     D0      D1      D2      D3 */
	{_PA_10, _PA_9,  _PA_11, _PA_12, _PA_7,  _PA_8 }, // Group0: PA7-PA12, default
	{_PA_28, _PA_27, _PA_29, _PA_30, _PA_25, _PA_26}  // Group1: PA26-PA31
};

/* sdio pinmux group: 0x0-0x1 */
/* Please refer to SDIO_PAD for more details. */
u8 SDIO_Pin_Grp = 0x0;

