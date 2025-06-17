/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "ameba_soc.h"

/*
 CLK     CMD      D0      D1      D2      D3
_PA_16  _PA_15  _PA_17  _PA_18  _PA_13  _PA_14 // Group1: PA13-PA18
_PA_29  _PA_28  _PA_30  _PA_31  _PA_26  _PA_27 // Group2: PA26-PA31
_PB_9   _PB_8   _PB_13  _PB_14  _PB_6   _PB_7  // Group3: PB6-PB9, PB13-PB14
_PB_19  _PB_18  _PB_20  _PB_21  _PA_12  _PB_17 // Group4: PA12, PB17-PB21
_PB_26  _PB_25  _PB_27  _PB_28  _PB_23  _PB_24 // Group5: PB23-PB28
 */
SDIOCFG_TypeDef sdio_config = {
	.sdio_clk_pin = _PB_9,	// refer to pinmux table
	.sdio_cmd_pin = _PB_8,	// refer to pinmux table
	.sdio_d0_pin = _PB_13,	// refer to pinmux table
	.sdio_d1_pin = _PB_14,	// refer to pinmux table
	.sdio_d2_pin = _PB_6,	// refer to pinmux table
	.sdio_d3_pin = _PB_7,	// refer to pinmux table
};
