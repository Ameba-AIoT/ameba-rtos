/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef IR_EXT_H
#define IR_EXT_H
#include "platform_autoconf.h"

#if defined (CONFIG_AMEBASMART)
/* for compatible with Dplus */
#define PINMUX_FUNCTION_IR_TX	PINMUX_FUNCTION_IR
#define PINMUX_FUNCTION_IR_RX	PINMUX_FUNCTION_IR

#define IR_PINMUX_S0_TX		_PA_3
#define IR_PINMUX_S0_RX		_PA_4

#define IR_PINMUX_S1_TX		_PB_11
#define IR_PINMUX_S1_RX		_PB_10

#define IR_PINMUX_S2_TX		_PB_21
#define IR_PINMUX_S2_RX		_PB_22

#define IR_PINMUX_S3_TX		_PB_25
#define IR_PINMUX_S3_RX		_PB_26

#define IR_PINMUX_S4_TX		_PA_17

#define IR_CLOCK_HZ			100000000

#elif defined (CONFIG_AMEBADPLUS)
/* PA30: SWD_CLK, PA31: SWD_DATA */
#define IR_PINMUX_S0_TX		_PA_30
#define IR_PINMUX_S0_RX		_PA_31

#define IR_PINMUX_S1_TX		_PA_26
#define IR_PINMUX_S1_RX		_PA_27

#define IR_PINMUX_S2_TX		_PA_28
#define IR_PINMUX_S2_RX		_PA_29

#define IR_PINMUX_S3_TX		_PB_17
#define IR_PINMUX_S3_RX		_PB_18

#define IR_PINMUX_S4_TX		_PB_20
#define IR_PINMUX_S4_RX		_PB_21

#define IR_PINMUX_S5_TX		_PB_30
#define IR_PINMUX_S5_RX		_PB_31

#define IR_CLOCK_HZ			40000000

#elif defined (CONFIG_AMEBAD)
#define PINMUX_FUNCTION_IR_TX	PINMUX_FUNCTION_IR
#define PINMUX_FUNCTION_IR_RX	PINMUX_FUNCTION_IR

#define IR_PINMUX_S1_TX		_PB_23
#define IR_PINMUX_S1_RX		_PB_22

#define IR_PINMUX_S0_TX		_PA_25
#define IR_PINMUX_S0_RX		_PA_26

#define IR_PINMUX_S2_TX		_PB_29
#define IR_PINMUX_S2_RX		_PB_31

#define IR_CLOCK_HZ			100000000
#endif

extern int example_raw_ir_tx_nec_polling(void);
#endif
