/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "platform_autoconf.h"
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <basic_types.h>
#include "bt_debug.h"
#if !defined(CONFIG_BT_ZEPHYR) || !CONFIG_BT_ZEPHYR
#include "hci_if_rtk.h"
#endif

/* ---------------------------------- define -------------------------------*/

/* ------------------------------ Global Variables -------------------------*/
uint8_t need_bt_power_on = 1;       // For download BT MP patch only once

/* -------------------------------- Functions ------------------------------*/
extern void bt_power_off(void);

/**
 * @brief     BT power on for MP test.
 * @param     None
 * @return    None
 */
void rtk_bt_mp_power_on(void)
{
	if (need_bt_power_on) {
#if !defined(CONFIG_BT_ZEPHYR) || !CONFIG_BT_ZEPHYR
		hci_set_mp(true);
		hci_if_open(NULL);
		hci_if_wait_patch_download();
		BT_LOGA("After download patch, deinit HCI driver & HCI uart!\r\n");
		hci_if_close();
		hci_if_deinit();
#else
		BT_LOGE("Zephyr stack, rtk_bt_mp_power_on() is not ready\r\n");
#endif
		need_bt_power_on = 0;
	} else {
		BT_LOGE("No need to download patch again!\r\n");
	}
}

/**
 * @brief     BT power off for MP test.
 * @param     None
 * @return    None
 */
void rtk_bt_mp_power_off(void)
{
#if !defined(CONFIG_BT_ZEPHYR) || !CONFIG_BT_ZEPHYR
	bt_power_off();
	hci_set_mp(false);
#else
	BT_LOGE("Zephyr stack, rtk_bt_mp_power_off() is not ready\r\n");
#endif
	need_bt_power_on = 1;
}
