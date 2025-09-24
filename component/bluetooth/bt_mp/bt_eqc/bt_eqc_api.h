/**
 * @file      bt_eqc_api.h
 * @author
 * @brief     Bluetooth Common function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __BT_EQC_API_H__
#define __BT_EQC_API_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <rtk_bt_le_gap.h>
#include "hci_if_eqc.h"

#ifdef __cplusplus
extern "C"
{
#endif
//return value
#define EQC_FAIL                        0
#define EQC_SUCCESS                     1
#define EQC_DISC_FAIL                   2
#define EQC_DISC_3E_FAIL                3
#define EQC_API_FAIL                    4
#define EQC_WAIT_FAIL                   5

//flag
#define EQC_DEINIT                      0x01
#define EQC_INIT                        0x02
#define EQC_INIT_FAIL                   0x04
#define EQC_CLIEN_CONN                  0x08
#define EQC_CLIEN_CHANMAP               0x10
#define EQC_CLIEN_DISCONN_3E            0x20
#define EQC_CLIEN_DISCONN_OT            0x40

void rtk_bt_eqc_power_on(void);
void rtk_bt_eqc_power_off(void);
bool rtk_bt_eqc_set_event_mask(void);
bool rtk_bt_eqc_read_bt_address(uint8_t *bt_address);
bool rtk_bt_eqc_le_start_advertising(bool enable);
bool rtk_bt_eqc_le_connect(uint8_t *bt_address);
bool rtk_bt_eqc_le_connect_cancel(void);
bool rtk_bt_eqc_le_disconnect(uint16_t conn_handle, uint8_t disc_reason);
bool rtk_bt_eqc_le_read_current_channel_map(uint16_t conn_handle);

#ifdef __cplusplus
}
#endif

#endif