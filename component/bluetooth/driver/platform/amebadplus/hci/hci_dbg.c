/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <stdio.h>
#include <stdarg.h>
#include "hci_dbg.h"
#include "ameba_soc.h"
#include "osif.h"

static uint32_t hci_log_level = 0x1FF;

void hci_dbg_set_level(uint32_t level)
{
	/* uint32_t >= HCI_TP_DEBUG_ERROR */
	if (level > HCI_TP_DEBUG_HCI_STACK_DEBUG) {
		hci_log_level = 0x1FF;
	} else {
		hci_log_level = level;
	}
}

uint32_t hci_dbg_get_level(void)
{
	return hci_log_level;
}

void hci_debug_enable(void)
{
#if 0
	uint32_t data = 0;
	/* keep bt wakeup from dlps/lps 0x41008280[14:13]=11 */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BT_CTRL0, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BT_CTRL0) | LSYS_BIT_HOST_WAKE_BT_POL | LSYS_BIT_HOST_WAKE_BT);

	while (1) {
		osif_delay(100);
		data = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BT_CTRL1) & 0x7FFF; // 0x41008284 [14:0]
		if (data == 5) {
			/* bt active */
			HCI_DBG("bt active");
			break;
		} else {
			HCI_DBG("wait for bt wakeup from sleep, bt pmc state=0x%x", data);
		}
	}

	HCI_DBG("BT wakeup from sleep");
#endif
	//pad reg 0x410089F4[0]=0, SWD pinmux disable
	HAL_WRITE32(PINMUX_REG_BASE, REG_SWD_SDD_CTRL, HAL_READ32(PINMUX_REG_BASE, REG_SWD_SDD_CTRL) & (~PAD_BIT_SWD_PMUX_EN));

	Pinmux_Config(_PB_30, PINMUX_FUNCTION_BT_UART_TXD);  //bt tx, func60 -> external uart rx
	Pinmux_Config(_PA_30, PINMUX_FUNCTION_BT);  //bt rx -> external uart tx
	Pinmux_Config(_PB_31, PINMUX_FUNCTION_BT_UART_RTS);  //bt rts, func61 -> external uart cts
	Pinmux_Config(_PA_31, PINMUX_FUNCTION_BT);  //bt cts -> external uart rts

	HCI_DBG("external uart pins: rx<->_PB_30,tx<->_PA_30,cts<->_PB_31,rts<->_PA_31");

	//lp_sys reg 0x41008280[0]=1, indicate BT use external uart
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BT_CTRL0, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BT_CTRL0) | LSYS_BIT_BT_USE_EXT_UART);

}
