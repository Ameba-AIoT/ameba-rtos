/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef _HCI_PLATFORM_H_
#define _HCI_PLATFORM_H_

#include "hci/hci_common.h"
#include "./hci_uart.h"
#include "ameba_soc.h"
#include "platform_stdlib.h"

#define hci_platform_START_RF_CALIBRATION 1
#define hci_platform_DOWNLOAD_PATCH       1
#define hci_platform_MP_RESET_BAUDRATE    1
#define hci_platform_WRITE_PHY_EFUSE      1
#define hci_platform_WRITE_RX_ADCK        1

#if defined(hci_platform_WRITE_PHY_EFUSE) && hci_platform_WRITE_PHY_EFUSE
#define HCI_WRITE_PHY_EFUSE_LEN    0x70
#endif

#define HCI_DEFAULT_LMP_SUBVER1    0x8715
#define HCI_DEFAULT_LMP_SUBVER2    0x8721
#define HCI_PATCH_PROJECT_ID       0x2B

#define HCI_BT_COEX_ENABLE         1
#define HCI_BT_COEX_SW_MAILBOX     0

#define HCI_BT_KEEP_WAKE           0

typedef enum {
	OPCODE_PATCH_SNIPPETS = 0x01,
	OPCODE_DUMMY_HEADER = 0x02,
	OPCODE_SECURITY_HEADER = 0x03,
	OPCODE_OTA_FLAG = 0x04
} SECTION_OPCODE;

typedef enum {
	VOLTAGE_3V3 = 0,
	VOLTAGE_1V8
} bt_voltage_t;

void hci_platform_get_baudrate(uint8_t *baudrate, uint8_t len, uint8_t init_or_work);
void hci_platform_cfg_bd_addr(uint8_t *bdaddr);
void hci_platform_record_chipid(uint8_t chipid);
void hci_platform_dl_patch_done(void);
uint8_t hci_platform_set_baudrate(uint8_t init_or_work);
uint8_t hci_platform_dl_patch_init(void);
uint8_t hci_platform_get_patch_cmd_len(uint8_t *cmd_len);
uint8_t hci_platform_get_patch_cmd_buf(uint8_t *cmd_buf, uint8_t cmd_len);
void hci_platform_bt_rf_calibration(void);
int hci_platform_get_write_phy_efuse_data(uint8_t *data, uint8_t len);
int hci_platform_get_rx_adck_data(uint8_t *data, uint8_t len);
bool hci_platform_check_lmp_subver(uint16_t lmp_subver);

uint8_t hci_platform_init(void);
uint8_t hci_platform_deinit(void);

void hci_platform_bt_fw_log_open(void);
void hci_platform_bt_fw_log_close(void);
void hci_platform_bt_trace_log_open(void);
void hci_platform_bt_trace_log_close(void);

bool rtk_bt_pre_enable(void);
bool rtk_bt_post_enable(void);
uint8_t hci_platform_check_mp(void);
void hci_platform_force_uart_rts(bool op);
#endif
