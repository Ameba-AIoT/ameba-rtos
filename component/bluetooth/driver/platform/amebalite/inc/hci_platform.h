/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef _HCI_PLATFORM_H_
#define _HCI_PLATFORM_H_

#include "hci_uart.h"
#include "ameba_soc.h"
#include "platform_stdlib.h"

#define hci_platform_START_RF_CALIBRATION 1
#define hci_platform_DOWNLOAD_PATCH       1
#define hci_platform_MP_RESET_BAUDRATE    1
#define hci_platform_START_RX_DCK         1
#define hci_platform_WRITE_PHY_EFUSE      1

#if defined(hci_platform_WRITE_PHY_EFUSE) && hci_platform_WRITE_PHY_EFUSE
#define HCI_WRITE_PHY_EFUSE_LEN    0x70
#endif

#define HCI_PATCH_FLASH_ADDRESS    0x080F8000    /* Temp value, need sync flash layout with platform owner if use this function */

#define HCI_BT_COEX_ENABLE         1
#define HCI_BT_COEX_SW_MAILBOX     0

#define HCI_BT_KEEP_WAKE           0

#define hci_platform_record_chipid(__chipid)  (__chipid)

void hci_platform_cfg_bd_addr(uint8_t *bdaddr);
void hci_platform_get_config(uint8_t **buf, uint16_t *len);
void hci_platform_bt_rf_calibration(void);
void hci_platform_bt_rx_dck(void);
int hci_platform_get_write_phy_efuse_data(uint8_t *data, uint8_t len);

bool rtk_bt_pre_enable(void);
void rtk_bt_post_enable(void);
uint8_t hci_platform_init(void);
void hci_platform_deinit(void);

void hci_platform_debug_enable(void);

void hci_platform_debug_port_mask_enable(uint8_t bt_sel, uint32_t bt_dbg_mask);
void hci_platform_debug_port_pad_enable(uint8_t bt_sel, uint8_t bt_dbg_port, char *pad);
void hci_platform_debug_port_shift(uint8_t original, uint8_t mapping);
void hci_platform_gpio_enable(uint8_t bt_gpio, char *pad);
#endif
