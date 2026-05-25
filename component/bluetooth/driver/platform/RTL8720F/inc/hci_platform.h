/*
 * Copyright (c) 2025 Realtek Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _HCI_PLATFORM_H_
#define _HCI_PLATFORM_H_

#include "ameba_soc.h"
#include "platform_stdlib.h"


#define HCI_PATCH_FLASH_ADDRESS    0x080F8000    /* Temp value, need sync flash layout with platform owner if use this function */

#define HCI_BT_COEX_ENABLE         1
#define HCI_BT_COEX_SW_MAILBOX     0
#define HCI_BT_COEX_BR_EDR_SUPPORT   0

#define HCI_BT_KEEP_AWAKE          0

void hci_platform_cfg_bd_addr(uint8_t *bdaddr);
int hci_platform_get_write_phy_efuse_data(uint8_t *data, uint8_t len);

bool rtk_bt_pre_enable(void);
void rtk_bt_post_enable(void);
uint8_t hci_platform_open(void);
void hci_platform_close(void);
void hci_platform_free(void);

#endif
