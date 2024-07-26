/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef _HCI_PLATFORM_H_
#define _HCI_PLATFORM_H_

#include "./hci_uart.h"
#include "ameba_soc.h"
#include "platform_stdlib.h"

#define hci_platform_DOWNLOAD_PATCH    1
#define hci_platform_MP_RESET_BAUDRATE 1
#define hci_platform_WRITE_PHY_EFUSE   1

#if defined(hci_platform_WRITE_PHY_EFUSE) && hci_platform_WRITE_PHY_EFUSE
#define HCI_WRITE_PHY_EFUSE_LEN    0x6D
#endif

#define HCI_DEFAULT_LMP_SUBVER     0x8730
#define HCI_PATCH_PROJECT_ID       0x28
#define HCI_PATCH_FLASH_ADDRESS    0x08300000    //Temp value, need sync flash layout with platform owner if use this function

#define HCI_BT_COEX_ENABLE         1
#define HCI_BT_COEX_SW_MAILBOX     0

#define HCI_BT_KEEP_WAKE           0

/**
* @typedef   bt_ant_t
* @brief     Bluetooth rf path.
*/
typedef enum {
	ANT_S0 = 0,                      /*!< 0, means bt_rfafe */
	ANT_S1,                          /*!< 1, means wl_rfafe */
} bt_ant_t;

void hci_platform_cfg_bd_addr(uint8_t *bdaddr);
void hci_platform_get_config(uint8_t **buf, uint16_t *len);
int hci_platform_get_write_phy_efuse_data(uint8_t *data, uint8_t len);
uint8_t hci_platform_get_rom_ver(void);
bool hci_platform_check_lmp_subver(uint16_t lmp_subver);
uint8_t hci_platform_record_chipid(uint8_t chipid);

uint8_t hci_platform_init(void);
void hci_platform_deinit(void);

void hci_platform_debug_enable(void);
#endif