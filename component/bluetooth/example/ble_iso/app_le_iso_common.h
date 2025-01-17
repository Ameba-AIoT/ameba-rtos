/*
*******************************************************************************
* Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef __APP_BT_LE_AUDIO_COMMON_H__
#define __APP_BT_LE_AUDIO_COMMON_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <rtk_bt_def.h>
#include <bt_api_config.h>
#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
#include <rtk_bt_le_iso.h>

#define LE_ISO_DEMO_SEND_DATA_MSG_QUEUE_SIZE                    0x20
#define LE_ISO_DEMO_SEND_DATA_TASK_PRIORITY                     4
#define LE_ISO_DEMO_SEND_DATA_TASK_STACK_SIZE                   256*10

/** @brief  Default minimum primary advertising interval when device is discoverable (units of 0.625ms, 32=20ms) */
#define RTK_BLE_ISO_CIS_ACCEPTOR_PRIMARY_ADV_INTERVAL_MIN       (48)
/** @brief  Default maximum primary advertising interval */
#define RTK_BLE_ISO_CIS_ACCEPTOR_PRIMARY_ADV_INTERVAL_MAX       (48)
/** @brief  Default minimum periodic advertising interval when device is discoverable (units of 0.625ms, 32=20ms) */
#define RTK_BLE_ISO_CIS_ACCEPTOR_PA_INTERVAL_MIN                (80)
/** @brief  Default maximum periodic advertising interval */
#define RTK_BLE_ISO_CIS_ACCEPTOR_PA_INTERVAL_MAX                (80)
#define RTK_BT_DEV_NAME "RTK_CIS_ACCEPTOR"
/** @brief  Default minimum primary advertising interval when device is discoverable (units of 0.625ms, 32=20ms) */
#define RTK_BLE_ISO_BIS_BROADCASTER_PRIMARY_ADV_INTERVAL_MIN    (48)
/** @brief  Default maximum primary advertising interval */
#define RTK_BLE_ISO_BIS_BROADCASTER_PRIMARY_ADV_INTERVAL_MAX    (48)
/** @brief  Default minimum periodic advertising interval when device is discoverable (units of 0.625ms, 32=20ms) */
#define RTK_BLE_ISO_BIS_BROADCASTER_PA_INTERVAL_MIN             (80)
/** @brief  Default maximum periodic advertising interval */
#define RTK_BLE_ISO_BIS_BROADCASTER_PA_INTERVAL_MAX             (80)


typedef enum {
	RTK_BLE_EXAMPLE_CIS_INITIATOR_DISABLE = 0,
	RTK_BLE_EXAMPLE_CIS_INITIATOR_ENABLE,
	RTK_BLE_EXAMPLE_CIS_INITIATOR_CONN,
	RTK_BLE_EXAMPLE_CIS_INITIATOR_DISC,
	RTK_BLE_EXAMPLE_CIS_INITIATOR_CIG_EST,
	RTK_BLE_EXAMPLE_CIS_INITIATOR_CIG_DIS,
	RTK_BLE_EXAMPLE_CIS_INITIATOR_PATH_SETUP,
	RTK_BLE_EXAMPLE_CIS_INITIATOR_NOT_SUPPORT,
} rtk_ble_example_cis_initiator_status_t;

typedef enum {
	RTK_BLE_EXAMPLE_CIS_ACCEPTOR_DISABLE = 0,
	RTK_BLE_EXAMPLE_CIS_ACCEPTOR_ENABLE,
	RTK_BLE_EXAMPLE_CIS_ACCEPTOR_CONN,
	RTK_BLE_EXAMPLE_CIS_ACCEPTOR_DISC,
	RTK_BLE_EXAMPLE_CIS_ACCEPTOR_CIG_EST,
	RTK_BLE_EXAMPLE_CIS_ACCEPTOR_CIG_DIS,
	RTK_BLE_EXAMPLE_CIS_ACCEPTOR_PATH_SETUP,
	RTK_BLE_EXAMPLE_CIS_ACCEPTOR_NOT_SUPPORT,
} rtk_ble_example_cis_acceptor_status_t;

typedef enum {
	RTK_BLE_EXAMPLE_BIS_BROADCASTER_DISABLE = 0,
	RTK_BLE_EXAMPLE_BIS_BROADCASTER_ENABLE,
	RTK_BLE_EXAMPLE_BIS_BROADCASTER_CBIG,
	RTK_BLE_EXAMPLE_BIS_BROADCASTER_TBIG,
	RTK_BLE_EXAMPLE_BIS_BROADCASTER_NOT_SUPPORT,
} rtk_ble_example_bis_broadcaster_status_t;

typedef enum {
	RTK_BLE_EXAMPLE_BIS_RECEIVER_DISABLE = 0,
	RTK_BLE_EXAMPLE_BIS_RECEIVER_ENABLE,
	RTK_BLE_EXAMPLE_BIS_RECEIVER_PA_CSYNC,
	RTK_BLE_EXAMPLE_BIS_RECEIVER_PA_TSYNC,
	RTK_BLE_EXAMPLE_BIS_RECEIVER_CSYNC,
	RTK_BLE_EXAMPLE_BIS_RECEIVER_TSYNC,
	RTK_BLE_EXAMPLE_BIS_RECEIVER_NOT_SUPPORT,
} rtk_ble_example_bis_receiver_status_t;

typedef struct {
	bool                                        active;
	uint8_t                                     cis_id;
	uint16_t                                    cis_conn_handle;
} rtk_ble_cis_info_t;

typedef struct {
	rtk_ble_example_cis_initiator_status_t      status;
#if defined(RTK_LE_ISO_DEMO_USE_HW_GTIMER) && RTK_LE_ISO_DEMO_USE_HW_GTIMER
	gtimer_t                                    iso_send_timer;
#else
	void                                        *iso_send_timer;
#endif
	uint8_t                                     sdu_interval;
	uint8_t                                     cis_num;
	uint16_t                                    conn_handle;
	rtk_ble_cis_info_t                          cis_info[RTK_BLE_CIS_MAX_NUM];
} rtk_ble_cis_initiator_info_t;

typedef struct {
	void                                        *mutex;
	uint8_t                                     cig_num;
	void                                        *iso_send_task_sem;
	void                                        *iso_send_msg_q;
	void                                        *iso_send_task_hdl;
	rtk_ble_cis_initiator_info_t                initiator_info[RTK_BLE_CIG_MAX_NUM];
} app_bt_le_iso_initiator_info_t;

typedef struct {
	rtk_ble_example_cis_acceptor_status_t       status;
#if defined(RTK_LE_ISO_DEMO_USE_HW_GTIMER) && RTK_LE_ISO_DEMO_USE_HW_GTIMER
	gtimer_t                                    iso_send_timer;
#else
	void                                        *iso_send_timer;
#endif
	uint8_t                                     sdu_interval;
	uint8_t                                     cis_num;
	uint16_t                                    conn_handle;
	rtk_ble_cis_info_t                          cis_info[RTK_BLE_CIS_MAX_NUM];
} rtk_ble_cis_acceptor_info_t;

typedef struct {
	void                                        *mutex;
	uint8_t                                     cig_num;
#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
	uint8_t                                     adv_handle;
#endif
	void                                        *iso_send_task_sem;
	void                                        *iso_send_msg_q;
	void                                        *iso_send_task_hdl;
	rtk_ble_cis_acceptor_info_t                 acceptor_info[RTK_BLE_CIG_MAX_NUM];
} app_bt_le_iso_acceptor_info_t;

typedef struct {
	void                                        *mutex;
	rtk_ble_example_bis_broadcaster_status_t    status;
#if defined(RTK_LE_ISO_DEMO_USE_HW_GTIMER) && RTK_LE_ISO_DEMO_USE_HW_GTIMER
	gtimer_t                                    iso_send_timer;
#else
	void                                        *iso_send_timer;
#endif
	void                                        *iso_send_task_sem;
	void                                        *iso_send_msg_q;
	void                                        *iso_send_task_hdl;
	uint8_t                                     sdu_interval;
	uint8_t                                     adv_handle;
	uint8_t                                     big_handle;
	uint16_t                                    bis_num;
	uint16_t                                    bis_conn_handle[RTK_BLE_BIS_BROADCASTER_MAX_NUM];
} app_bt_le_iso_broadcaster_info_t;

typedef struct {
	void                                        *mutex;
	rtk_ble_example_bis_receiver_status_t       status;
	uint8_t                                     big_handle;
	uint8_t                                     sync_id;
	uint16_t                                    sync_handle;
	uint16_t                                    bis_num;
	uint16_t                                    bis_conn_handle[RTK_BLE_BIS_RECEIVER_MAX_NUM];
} app_bt_le_iso_receiver_info_t;

extern app_bt_le_iso_initiator_info_t g_le_iso_cis_initiator_status;
extern app_bt_le_iso_acceptor_info_t g_le_iso_cis_acceptor_status;
extern rtk_bt_le_iso_setup_path_param_t app_le_iso_setup_path_param;

uint16_t app_bt_le_iso_cis_id_availiable_check(uint8_t le_iso_role, uint8_t cis_id, uint8_t cig_id);
#endif
#ifdef __cplusplus
}
#endif
#endif  /* __APP_BT_LE_AUDIO_COMMON_H__ */