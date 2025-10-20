/*
 *******************************************************************************
 * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __RTK_STACK_ISO_INTERVAL_H__
#define __RTK_STACK_ISO_INTERVAL_H__

#include <rtk_bt_def.h>
#include <basic_types.h>
#include <bt_api_config.h>
#include <rtk_bt_le_gap.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT

#define INVALID_CIG_ID        0xFF
#define INVALID_CIS_ID        0xFF
typedef enum {
	RTK_BLE_CIS_INITIATOR_DISABLE = 0,
	RTK_BLE_CIS_INITIATOR_ENABLE,
	RTK_BLE_CIS_INITIATOR_CONN,
	RTK_BLE_CIS_INITIATOR_DISC,
	RTK_BLE_CIS_INITIATOR_CIG_EST,
	RTK_BLE_CIS_INITIATOR_CIG_DIS,
	RTK_BLE_CIS_INITIATOR_PATH_SETUP,
	RTK_BLE_CIS_INITIATOR_NOT_SUPPORT,
} rtk_ble_cis_initiator_status_t;

typedef enum {
	RTK_BLE_CIS_ACCEPTOR_DISABLE = 0,
	RTK_BLE_CIS_ACCEPTOR_ENABLE,
	RTK_BLE_CIS_ACCEPTOR_CONN,
	RTK_BLE_CIS_ACCEPTOR_DISC,
	RTK_BLE_CIS_ACCEPTOR_CIG_EST,
	RTK_BLE_CIS_ACCEPTOR_CIG_DIS,
	RTK_BLE_CIS_ACCEPTOR_PATH_SETUP,
	RTK_BLE_CIS_ACCEPTOR_NOT_SUPPORT,
} rtk_ble_cis_acceptor_status_t;

typedef enum {
	RTK_BLE_BIS_BROADCASTER_DISABLE = 0,
	RTK_BLE_BIS_BROADCASTER_ENABLE,
	RTK_BLE_BIS_BROADCASTER_CBIG,
	RTK_BLE_BIS_BROADCASTER_TBIG,
	RTK_BLE_BIS_BROADCASTER_NOT_SUPPORT,
} rtk_ble_bis_broadcaster_status_t;

typedef enum {
	RTK_BLE_BIS_RECEIVER_DISABLE = 0,
	RTK_BLE_BIS_RECEIVER_ENABLE,
	RTK_BLE_BIS_RECEIVER_PA_CSYNC,
	RTK_BLE_BIS_RECEIVER_PA_TSYNC,
	RTK_BLE_BIS_RECEIVER_CSYNC,
	RTK_BLE_BIS_RECEIVER_TSYNC,
	RTK_BLE_BIS_RECEIVER_NOT_SUPPORT,
} rtk_ble_bis_receiver_status_t;

typedef struct {
	bool active;
	uint8_t cis_id;
	uint16_t cis_conn_handle;
} rtk_ble_cis_info_t;

typedef struct {
	rtk_ble_cis_initiator_status_t status;
	uint8_t cig_id;
	uint8_t sdu_interval;
	uint8_t cis_num;
	uint16_t conn_handle;
	rtk_ble_cis_info_t cis_info[RTK_BLE_CIS_MAX_NUM];
} rtk_ble_cis_initiator_info_t;

typedef struct {
	void *mtx;
	uint8_t cig_num;
	rtk_ble_cis_initiator_info_t initiator_info[RTK_BLE_CIG_MAX_NUM];
} rtk_bt_le_iso_initiator_info_t;

typedef struct {
	rtk_ble_cis_acceptor_status_t status;
	uint8_t cig_id;
	uint8_t sdu_interval;
	uint8_t cis_num;
	uint16_t conn_handle;
	rtk_ble_cis_info_t cis_info[RTK_BLE_CIS_MAX_NUM];
} rtk_ble_cis_acceptor_info_t;

typedef struct {
	void *mtx;
	uint8_t cig_num;
	rtk_ble_cis_acceptor_info_t acceptor_info[RTK_BLE_CIG_MAX_NUM];
} rtk_bt_le_iso_acceptor_info_t;

typedef struct {
	void *mtx;
	rtk_ble_bis_broadcaster_status_t status;
	uint8_t sdu_interval;
	uint8_t adv_handle;
	uint8_t big_handle;
	uint16_t bis_num;
	uint16_t bis_conn_handle[RTK_BLE_BIS_MAX_NUM];
} rtk_bt_le_iso_broadcaster_info_t;

typedef struct {
	void *mtx;
	rtk_ble_bis_receiver_status_t status;
	uint8_t big_handle;
	uint8_t sync_id;
	uint16_t sync_handle;
	uint16_t bis_num;
	uint16_t bis_conn_handle[RTK_BLE_BIS_MAX_NUM];
} rtk_bt_le_iso_receiver_info_t;

typedef struct {
	bool init_flag;
	rtk_bt_le_iso_role_t iso_role;
	rtk_bt_le_iso_app_conf_t iso_app_conf;
	rtk_bt_le_iso_initiator_info_t ini;
	rtk_bt_le_iso_acceptor_info_t acp;
	rtk_bt_le_iso_broadcaster_info_t bsrc;
	rtk_bt_le_iso_receiver_info_t recv;
} rtk_bt_le_iso_priv_t;

/**
 * @}
 */

#endif //end of #if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT

#ifdef __cplusplus
}
#endif

#endif  /* __RTK_STACK_ISO_INTERVAL_H__ */
