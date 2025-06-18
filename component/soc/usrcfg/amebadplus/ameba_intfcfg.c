/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "ameba_soc.h"

SDIOCFG_TypeDef sdio_config = {
	.sdio_clk_pin = _PB_9,	// refer to pinmux table
	.sdio_cmd_pin = _PB_8,	// refer to pinmux table
	.sdio_d0_pin = _PB_13,	// refer to pinmux table
	.sdio_d1_pin = _PB_14,	// refer to pinmux table
	.sdio_d2_pin = _PB_6,	// refer to pinmux table
	.sdio_d3_pin = _PB_7,	// refer to pinmux table
};
