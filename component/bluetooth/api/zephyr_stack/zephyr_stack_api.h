/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef ZEPHYR_STACK_API_H
#define ZEPHYR_STACK_API_H

#include <rtk_bt_def.h>
#include <zephyr/bluetooth/bluetooth.h>

#define BT_ZEPHYR_LEGACY_ADV_MAX_LEN        31
#define BT_ZEPHYR_ADV_STRUCTRUES_MAX_NUM    16

#define RTK_BT_HCI_ERR(err)    (err ? (RTK_BT_ERR_HCI_GROUP | err) : 0)
#define RTK_BT_ATT_ERR(err)    (err ? (RTK_BT_ERR_ATT_GROUP | err) : 0)

typedef struct {
	uint8_t all_phys;
	uint8_t tx_phy;
	uint8_t rx_phy;
} bt_zephyr_gap_phy_setting;

typedef struct {
	uint16_t tx_octets;
	uint16_t tx_time;
} bt_zephyr_gap_data_len_setting;

typedef struct {
	struct bt_data data[BT_ZEPHYR_ADV_STRUCTRUES_MAX_NUM];
	uint8_t num;
	uint8_t *buf;
	uint16_t buf_len;
	uint8_t is_valid;
} bt_zephyr_adv_data;

uint16_t zephyr_err_to_rtk(int err);

#endif