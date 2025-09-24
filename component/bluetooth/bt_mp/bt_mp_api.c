/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "bt_debug.h"
#include "hci_common.h"
#include "hci_transport.h"
#include "hci_uart.h"

/**
 * @brief     BT power on for MP test.
 * @param     None
 * @return    None
 */
void rtk_bt_mp_power_on(void)
{
	hci_set_mp(true);
	if (hci_controller_open()) {
		BT_LOGA("Patch download End!\r\n");
		BT_LOGA("After download patch, deinit HCI driver & HCI uart!\r\n");
		/* In order to keep controller powerd on, do not use hci_controller_close() */
		hci_uart_close();
		hci_transport_close();
		hci_controller_free();
	}
}

/**
 * @brief     BT power off for MP test.
 * @param     None
 * @return    None
 */
void rtk_bt_mp_power_off(void)
{
	/* just power off controller, uart & transport are already closed & freed */
	hci_controller_close();
	hci_set_mp(false);
}