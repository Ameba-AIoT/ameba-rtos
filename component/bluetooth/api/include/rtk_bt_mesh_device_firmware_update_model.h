/**
 * @file      rtk_bt_mesh_device_firmware_update_model.h
 * @brief     Bluetooth LE MESH device firmware update model api data struct and function definition
 * @copyright Copyright (c) 2024. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_MESH_DEVICE_FIRMWARE_UPDATE_MODEL_H__
#define __RTK_BT_MESH_DEVICE_FIRMWARE_UPDATE_MODEL_H__

#include <stdint.h>
#include <rtk_bt_mesh_def.h>

#define BT_MESH_DFU_FWID_MAX_LEN  16
#define BT_MESH_DFU_METADATA_MAX_LEN  32

/* ------------------------------ act and event macro -----------------------------------*/
/**
 * @typedef   rtk_bt_mesh_dfu_model_initiator_act_t
 * @brief     BLE MESH device firmware update model initiator act definition.
 */
typedef enum {
	RTK_BT_MESH_DFU_ACT_INITIATOR_UPLOAD_START = 1,
	RTK_BT_MESH_DFU_ACT_INITIATOR_RECEIVERS_ADD,
	RTK_BT_MESH_DFU_ACT_INITIATOR_RECEIVERS_DEL_ALL,
	RTK_BT_MESH_DFU_ACT_INITIATOR_DISTRIBUTE_START,
	RTK_BT_MESH_DFU_ACT_INITIATOR_MAX,
} rtk_bt_mesh_dfu_model_initiator_act_t;

/**
 * @typedef   rtk_bt_mesh_dfu_event_initiator_t
 * @brief     BLE MESH device firmware update model initiator event definition.
 */
typedef enum {
	RTK_BT_MESH_DFU_EVT_INITIATOR_BLOB_PARAM = 1,
	RTK_BT_MESH_DFU_EVT_INITIATOR_TRANSFER,
	RTK_BT_MESH_DFU_EVT_INITIATOR_DIST_CLIENT_UPLOAD_STATUS,
	RTK_BT_MESH_DFU_EVT_INITIATOR_DIST_CLIENT_RECEIVES_STATUS,
	RTK_BT_MESH_DFU_EVT_INITIATOR_DIST_CLIENT_DISTRIBUTION_STATUS,
	RTK_BT_MESH_DFU_EVT_INITIATOR_MAX,
} rtk_bt_mesh_dfu_event_initiator_t;

/**
 * @typedef   rtk_bt_mesh_dfu_model_standalone_updater_act_t
 * @brief     BLE MESH device firmware update model standalone updater act definition.
 */
typedef enum {
	RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_SET_PARAM = 1,
	RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_ADD_RECEIVER,
	RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_START,
	RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_MAX,
} rtk_bt_mesh_dfu_model_standalone_updater_act_t;

#if 0
/**
 * @typedef   rtk_bt_mesh_dfu_model_distributor_act_t
 * @brief     BLE MESH device firmware update model distributor act definition.
 */
typedef enum {
	RTK_BT_MESH_DFU_ACT_DISTRIBUTOR_ADD = 1,
	RTK_BT_MESH_DFU_ACT_DISTRIBUTOR_START,
	RTK_BT_MESH_DFU_ACT_DISTRIBUTOR_SET_NEW_FW_ADDR,
	RTK_BT_MESH_DFU_ACT_DISTRIBUTOR_MAX,
} rtk_bt_mesh_dfu_model_distributor_act_t;
#endif

/**
 * @typedef   rtk_bt_mesh_dfu_event_distributor_t
 * @brief     BLE MESH device firmware update model distributor event definition.
 */
typedef enum {
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_PARAM = 1,
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_NODE_FAIL,
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_PROGRESS,
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_FAIL,
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_SUCCESS,
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_VERIFY,
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_COMPLETE,
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_UPLOAD_START,
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_UPLOAD_BLOCK_DATA,
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_UPLOAD_COMPLETE,
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_MAX,
} rtk_bt_mesh_dfu_event_distributor_t;

/**
 * @typedef   rtk_bt_mesh_dfu_event_target_t
 * @brief     BLE MESH device firmware update model target event definition.
 */
typedef enum {
	RTK_BT_MESH_DFU_EVT_TARGET_METADATA_CHECK = 1,
	RTK_BT_MESH_DFU_EVT_TARGET_START,
	RTK_BT_MESH_DFU_EVT_TARGET_BLOCK_DATA,
	RTK_BT_MESH_DFU_EVT_TARGET_VERIFY,
	RTK_BT_MESH_DFU_EVT_TARGET_VERIFY_CANCEL,
	RTK_BT_MESH_DFU_EVT_TARGET_APPLY,
	RTK_BT_MESH_DFU_EVT_TARGET_FAIL,
	RTK_BT_MESH_DFU_EVT_TARGET_MAX,
} rtk_bt_mesh_dfu_event_target_t;

/* ---------------------------------- data struct define ------------------------------*/
/**
 * @typedef   rtk_bt_mesh_dfu_model_api_result_t
 * @brief     BLE MESH device firmware update model api result definition.
 */
typedef enum {
	RTK_BT_MESH_DFU_MODEL_API_SUCCESS = 0,
	RTK_BT_MESH_DFU_MODEL_API_FAIL,
} rtk_bt_mesh_dfu_model_api_result_t;

#define RTK_BT_MESH_FW_VERSION_MAX_LEN                              106  // FW_VERSION_MAX_LEN

/**
 * @typedef   rtk_bt_mesh_dfu_fw_id_t
 * @brief     BLE MESH device firmware update firmware id data structure.
 */
typedef struct {
	uint16_t company_id;
	uint8_t version[RTK_BT_MESH_FW_VERSION_MAX_LEN];  // if OTA more than one time, the fw id version of OTA2 should be larger than OTA1
} _PACKED4_ rtk_bt_mesh_dfu_fw_id_t;

/**
 * @typedef   rtk_bt_mesh_dfu_initiator_upload_start_t
 * @brief     BLE MESH device firmware update initiator Firmware Distribution Upload Start message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t app_key_index;
	uint16_t update_timeout_base;
	uint32_t fw_image_size;
	uint16_t metadata_len;
	uint8_t *p_metadata;
	uint8_t fw_id_len;
	rtk_bt_mesh_dfu_fw_id_t fw_id;  // if OTA more than one time, the fw id of OTA2 should be larger than OTA1
} rtk_bt_mesh_dfu_initiator_upload_start_t;

/**
 * @typedef   rtk_bt_mesh_dfu_initiator_cb_type_t
 * @brief     BLE MESH device firmware update initiator callback type structure.
 */
typedef enum {
	RTK_BT_MESH_DFU_INIT_CB_TYPE_UPLOAD_PROGRESS,
	RTK_BT_MESH_DFU_INIT_CB_TYPE_UPLOAD_SUCCESS,
	RTK_BT_MESH_DFU_INIT_CB_TYPE_UPLOAD_FAIL,
} rtk_bt_mesh_dfu_initiator_cb_type_t;  // dfu_init_cb_type_t

/**
 * @typedef   rtk_bt_mesh_dfu_initiator_phase_t
 * @brief     BLE MESH device firmware update initiator phase structure.
 */
typedef enum {
	RTK_BT_MESH_DFU_INIT_PHASE_IDLE,
	RTK_BT_MESH_DFU_INIT_PHASE_UPLOAD_START,
	RTK_BT_MESH_DFU_INIT_PHASE_BLOB_TRANSFER,
	RTK_BT_MESH_DFU_INIT_PHASE_FAILED,
} rtk_bt_mesh_dfu_initiator_phase_t;  // dfu_init_phase_t

/**
 * @typedef   rtk_bt_mesh_dfu_evt_initiator_transfer_t
 * @brief     BLE MESH device firmware update distributor transfer structure.
 */
typedef struct {
	rtk_bt_mesh_dfu_initiator_cb_type_t type;
	rtk_bt_mesh_dfu_initiator_phase_t init_phase;
	union {
		uint16_t addr;
		uint8_t progress;
	};
} rtk_bt_mesh_dfu_evt_initiator_transfer_t;  // dfu_init_transfer_t

/**
 * @typedef   rtk_bt_mesh_dfu_initiator_dist_recv_t
 * @brief     BLE MESH device firmware update initiator Firmware Distribution Receivers Add message structure.
 */
typedef struct {
	uint16_t addr;
	uint8_t update_fw_image_idx;
} rtk_bt_mesh_dfu_initiator_dist_recv_t;

/**
 * @typedef   rtk_bt_mesh_dfu_initiator_dist_recv_add_t
 * @brief     BLE MESH device firmware update initiator distribute receiver add param structure.
 */
typedef struct {
	uint16_t dst;  // the destination address of Firmware Distribution Receivers Add message
	uint16_t app_key_index;  // the app key index of above message
	uint8_t recv_num;  // the number of rtk_bt_mesh_dfu_initiator_dist_recv_t data struct
	rtk_bt_mesh_dfu_initiator_dist_recv_t *recv;
} rtk_bt_mesh_dfu_initiator_dist_recv_add_t;

/**
 * @typedef   rtk_bt_mesh_dfu_initiator_dist_recv_del_all_t
 * @brief     BLE MESH device firmware update initiator distribute receiver delete all param structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t app_key_index;
} rtk_bt_mesh_dfu_initiator_dist_recv_del_all_t;

/**
 * @typedef   rtk_bt_mesh_dfu_blob_transfer_mode_t
 * @brief     BLE MESH device firmware update blob transfer mode param structure.
 */
typedef enum {
	RTK_BT_MESH_DFU_BLOB_TRANSFER_MODE_IDLE, //!< No active transfer
	RTK_BT_MESH_DFU_BLOB_TRANSFER_MODE_PUSH, //!< Push BLOB transfer mode
	RTK_BT_MESH_DFU_BLOB_TRANSFER_MODE_PULL, //!< Pull BLOB transfer mode
} rtk_bt_mesh_dfu_blob_transfer_mode_t;  // blob_transfer_mode_t, dist_transfer_mode in rtk_bt_mesh_dfu_initiator_fw_dist_start_data_t

/**
 * @typedef   rtk_bt_mesh_dfu_initiator_fw_update_policy_t
 * @brief     BLE MESH device firmware update initiator firmware update policy param structure.
 */
typedef enum {
	/* The Firmware Distribution Server verifies that firmware image
	    distribution completed successfully but does not apply the update. The
	    Initiator (the Firmware Distribution Client) initiates firmware image
	    application */
	RTK_BT_MESH_DFU_INITIATOR_FW_UPDATE_POLICY_VERIFY_ONLY,
	/* The Firmware Distribution Server verifies that firmware image
	    distribution completed successfully and then applies the firmware
	    update */
	RTK_BT_MESH_DFU_INITIATOR_FW_UPDATE_POLICY_VERIFY_AND_UPDATE,
} rtk_bt_mesh_dfu_initiator_fw_update_policy_t;  // fw_update_policy_t update_policy in rtk_bt_mesh_dfu_initiator_fw_dist_start_data_t

/**
 * @typedef   rtk_bt_mesh_dfu_initiator_fw_dist_start_data_t
 * @brief     BLE MESH device firmware update initiator Firmware Distribution Start message structure.
 */
typedef struct {
	uint16_t dist_app_key_index;
	uint8_t dist_ttl;
	uint16_t dist_timeout_base;  // 10 * (update_timeout_base + 1) s
	uint8_t dist_transfer_mode : 2;
	uint8_t update_policy : 1;
	uint8_t rfu : 5;
	uint16_t dist_fw_image_idx;
	uint8_t dist_multicast_addr[16];
} _PACKED4_ rtk_bt_mesh_dfu_initiator_fw_dist_start_data_t;  // fw_dist_start_data_t

/**
 * @typedef   rtk_bt_mesh_dfu_initiator_fw_dist_start_param_t
 * @brief     BLE MESH device firmware update initiator distribute start param structure.
 */
typedef struct {
	uint16_t dst;  // the destination address of Firmware Distribution Start message
	uint16_t app_key_index;  // app key index of above message
	uint8_t dist_dst_len;  // len of dist_multicast_addr in rtk_bt_mesh_dfu_initiator_fw_dist_start_data_t
	rtk_bt_mesh_dfu_initiator_fw_dist_start_data_t data;
} rtk_bt_mesh_dfu_initiator_fw_dist_start_param_t;

/**
 * @typedef   rtk_bt_mesh_dfu_fw_dist_status_code_t
 * @brief     BLE MESH device firmware update initiator distribute client receives status.
 */
typedef enum {
	RTK_BT_MESH_DFU_FW_DIST_STATUS_SUCCESS,                 // The message was processed successfully
	RTK_BT_MESH_DFU_FW_DIST_STATUS_INSUFFICIENT_RESOURCES,  // Insufficient resources on the nod
	RTK_BT_MESH_DFU_FW_DIST_STATUS_WRONG_PHASE,             // The operation cannot be performed while the server is in the current phase
	RTK_BT_MESH_DFU_FW_DIST_STATUS_INTERNAL_ERR,            // An internal error occurred on the node
	RTK_BT_MESH_DFU_FW_DIST_STATUS_FW_NOT_FOUND,            // The requested firmware image is not stored on the Distributor
	RTK_BT_MESH_DFU_FW_DIST_STATUS_INVALID_APP_KEY_INDEX,   // The AppKey identified by the AppKey Index is not known to the node
	RTK_BT_MESH_DFU_FW_DIST_STATUS_RECEIVERS_LIST_EMPTY,    // There are no Target nodes in the Distribution Receivers List state
	RTK_BT_MESH_DFU_FW_DIST_STATUS_BUSY_WITH_DIST,          // Another firmware image distribution is in progress
	RTK_BT_MESH_DFU_FW_DIST_STATUS_BUSY_WITH_UPLOAD,        // Another upload is in progress
	RTK_BT_MESH_DFU_FW_DIST_STATUS_URI_NOT_SUPPORTED,       // The URI scheme name indicated by the Update URI is not supported
	RTK_BT_MESH_DFU_FW_DIST_STATUS_URI_MALFORMED,           // The format of the Update URI is invalid
	RTK_BT_MESH_DFU_FW_DIST_STATUS_URI_UNREACHABLE,         // The URI is unreachable
	RTK_BT_MESH_DFU_FW_DIST_STATUS_NEW_FW_NOT_AVAILABLE,    // The Check Firmware OOB procedure did not find any new firmware
	RTK_BT_MESH_DFU_FW_DIST_STATUS_SUSPEND_FAILED,          // The suspension of the Distribute Firmware procedure failed
} rtk_bt_mesh_dfu_fw_dist_status_code_t;  // fw_dist_status_code_t

/**
 * @typedef   rtk_bt_mesh_dfu_initiator_evt_dist_client_recvs_status
 * @brief     BLE MESH device firmware update initiator distribute client receives status data structure.
 */
typedef struct {
	uint16_t src;
	rtk_bt_mesh_dfu_fw_dist_status_code_t status;
	uint16_t recvs_list_cnt;
} rtk_bt_mesh_dfu_initiator_evt_dist_client_recvs_status;  // fw_dist_client_recvs_status_t

/**
 * @typedef   rtk_bt_mesh_dfu_standalone_updater_set_param_t
 * @brief     BLE MESH device firmware update standalone updater set relate param structure.
 */
typedef struct {
	uint32_t image_size;
	uint8_t fw_id_len;
	uint8_t meta_data_len;
	uint8_t fw_id[BT_MESH_DFU_FWID_MAX_LEN];
	uint8_t meta_data[BT_MESH_DFU_METADATA_MAX_LEN];
} rtk_bt_mesh_dfu_standalone_updater_set_param_t;

/**
 * @typedef   rtk_bt_mesh_dfu_standalone_updater_add_receiver_param_t
 * @brief     BLE MESH device firmware update standalone updater add receiver param structure.
 */
typedef struct {
	uint16_t addr;
	uint8_t update_image_index;
} rtk_bt_mesh_dfu_standalone_updater_add_receiver_param_t;

/**
 * @typedef   rtk_bt_mesh_dfu_standalone_update_start_t
 * @brief     BLE MESH device firmware update standalone updater Firmware Update Start message structure.
 */
typedef struct {
	uint16_t dst;
	uint16_t app_key_index;
	uint16_t update_timeout_base;  // 10 * (update_timeout_base + 1) s
} rtk_bt_mesh_dfu_standalone_update_start_t;

/**
 * @typedef   rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t
 * @brief     BLE MESH device firmware update distributor or initiator blob param structure.
 */
typedef struct {
	uint32_t blob_size;
	uint16_t block_size;
	uint16_t total_blocks;
	uint16_t chunk_size;
} rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t;

/**
 * @typedef   rtk_bt_mesh_dfu_dist_phase_t
 * @brief     BLE MESH device firmware update distributor transfer phase structure.
 */
typedef enum {
	RTK_BT_MESH_DFU_DIST_PHASE_IDLE,
	RTK_BT_MESH_DFU_DIST_PHASE_UPDATE_START,
	RTK_BT_MESH_DFU_DIST_PHASE_BLOB_TRANSFER,
	RTK_BT_MESH_DFU_DIST_PHASE_VERIFY,
	RTK_BT_MESH_DFU_DIST_PHASE_APPLY,
	RTK_BT_MESH_DFU_DIST_PHASE_CONFIRM,
	RTK_BT_MESH_DFU_DIST_PHASE_CANCELING,
	RTK_BT_MESH_DFU_DIST_PHASE_UNKNOWN,
} rtk_bt_mesh_dfu_dist_phase_t;  // dfu_dist_phase_t

/**
 * @typedef   rtk_bt_mesh_dfu_evt_distributor_transfer_node_fail_t
 * @brief     BLE MESH device firmware update distributor transfer node fail data structure.
 */
typedef struct {
	rtk_bt_mesh_dfu_dist_phase_t dist_phase;
	uint16_t addr;
} rtk_bt_mesh_dfu_evt_distributor_transfer_node_fail_t;

/**
 * @typedef   rtk_bt_mesh_dfu_evt_distributor_transfer_progress_t
 * @brief     BLE MESH device firmware update distributor transfer node fail data structure.
 */
typedef struct {
	rtk_bt_mesh_dfu_dist_phase_t dist_phase;
	uint8_t progress;
} rtk_bt_mesh_dfu_evt_distributor_transfer_progress_t;

/**
 * @typedef   rtk_bt_mesh_dfu_evt_distributor_transfer_other_t
 * @brief     BLE MESH device firmware update distributor transfer other data structure.
 */
typedef struct {
	rtk_bt_mesh_dfu_dist_phase_t dist_phase;
	uint8_t addr_num;
	uint16_t *paddr;
} rtk_bt_mesh_dfu_evt_distributor_transfer_other_t;

/**
 * @typedef   rtk_bt_mesh_dfu_evt_distributor_server_upload_start_t
 * @brief     BLE MESH device firmware update distributor server upload start data structure.
 */
typedef struct {
	uint8_t upload_ttl;
	uint16_t upload_timeout_base;
	uint8_t upload_blob_id[8];
	uint32_t upload_fw_size;
	uint8_t upload_fw_metadata_len;
	uint8_t *pupload_metadata;
	uint8_t upload_fw_id_len;
	uint8_t *pupload_fw_id;
} rtk_bt_mesh_dfu_evt_distributor_server_upload_start_t;  // fw_dist_server_upload_start_t

typedef enum {
	RTK_BT_MESH_DFU_FW_UPLOAD_FAIL_REASON_USER_CANCEL,
	RTK_BT_MESH_DFU_FW_UPLOAD_FAIL_REASON_TRANSFER_TIMEOUT,
	RTK_BT_MESH_DFU_FW_UPLOAD_FAIL_REASON_TRANSFER_ERR,
} rtk_bt_mesh_dfu_fw_upload_fail_reason_t;  // fw_upload_fail_reason_t

/**
 * @typedef   rtk_bt_mesh_dfu_evt_distributor_server_upload_fail_t
 * @brief     BLE MESH device firmware update distributor server upload fail data structure.
 */
typedef struct {
	rtk_bt_mesh_dfu_fw_upload_fail_reason_t reason;
} rtk_bt_mesh_dfu_evt_distributor_server_upload_fail_t;  // fw_dist_server_upload_fail_t

/**
 * @typedef   rtk_bt_mesh_dfu_evt_target_meatdata_t
 * @brief     BLE MESH device firmware update target metadata structure.
 */
typedef struct {
	uint8_t fw_image_idx;
	uint8_t *pmetadata;
	uint8_t metadata_len;
} rtk_bt_mesh_dfu_evt_target_meatdata_t;

/**
 * @typedef   rtk_bt_mesh_dfu_target_fw_update_fail_reason_t
 * @brief     BLE MESH device firmware update target fw update server fail reason.
 */
typedef enum {
	RTK_BT_MESH_DFU_TARGET_FW_UPDATE_FAIL_REASON_USER_CANCEL,
	RTK_BT_MESH_DFU_TARGET_FW_UPDATE_FAIL_REASON_TRANSFER_TIMEOUT,
	RTK_BT_MESH_DFU_TARGET_FW_UPDATE_FAIL_REASON_TRANSFER_ERR,
} rtk_bt_mesh_dfu_target_fw_update_fail_reason_t;

/**
 * @typedef   rtk_bt_mesh_dfu_evt_target_fw_update_server_fail_t
 * @brief     BLE MESH device firmware update target fw update server fail data structure.
 */
typedef struct {
	uint8_t fw_image_index;
	rtk_bt_mesh_dfu_target_fw_update_fail_reason_t reason;
} rtk_bt_mesh_dfu_evt_target_fw_update_server_fail_t;

/* ------------------------------function declration ----------------------------- */
/**
 * @defgroup  ble_mesh_dfu_model BT LE Mesh Device Firmware Update Model APIs
 * @brief     BT LE Mesh device firmware update Model related function APIs
 * @ingroup   ble_mesh_api_group
 * @{
 */

/**
 * @fn        uint16_t rtk_bt_mesh_dfu_initiator_upload_start(rtk_bt_mesh_dfu_initiator_upload_start_t *param)
 * @brief     Initiator send device firmware update upload start message to Distributor, will cause event @ref RTK_BT_MESH_DFU_ACT_INITIATOR_UPLOAD_START
 * @param[in] param: upload start message param
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_dfu_initiator_upload_start(rtk_bt_mesh_dfu_initiator_upload_start_t *param);

/**
 * @fn        uint16_t rtk_bt_mesh_dfu_initiator_receivers_add(rtk_bt_mesh_dfu_initiator_dist_recv_add_t *param)
 * @brief     Initiator send device firmware update receiver add message to Distributor, will cause event @ref RTK_BT_MESH_DFU_ACT_INITIATOR_RECEIVERS_ADD
 * @param[in] param: receiver add message param
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_dfu_initiator_receivers_add(rtk_bt_mesh_dfu_initiator_dist_recv_add_t *param);

/**
 * @fn        uint16_t rtk_bt_mesh_dfu_initiator_receivers_delete_all(rtk_bt_mesh_dfu_initiator_dist_recv_del_all_t *param)
 * @brief     Initiator send device firmware update receiver delete all message to Distributor. If execute OTA second time, should send this message to distributer before OTA, will cause event @ref RTK_BT_MESH_DFU_ACT_INITIATOR_RECEIVERS_DEL_ALL
 * @param[in] param: receiver add message param
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_dfu_initiator_receivers_delete_all(rtk_bt_mesh_dfu_initiator_dist_recv_del_all_t *param);

/**
 * @fn        uint16_t rtk_bt_mesh_dfu_initiator_distribute_start(rtk_bt_mesh_dfu_initiator_fw_dist_start_param_t *param)
 * @brief     Initiator send device firmware update distribute start message to Distributor, will cause event @ref RTK_BT_MESH_DFU_ACT_INITIATOR_DISTRIBUTE_START
 * @param[in] param: distribute start message param
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_dfu_initiator_distribute_start(rtk_bt_mesh_dfu_initiator_fw_dist_start_param_t *param);

/**
 * @fn        uint16_t rtk_bt_mesh_dfu_standalone_updater_set_param(rtk_bt_mesh_dfu_standalone_updater_set_param_t *param)
 * @brief     Standalone Updater set mesh ota relate param, will cause event @ref RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_SET_PARAM
 * @param[in] param: receiver add message param
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_dfu_standalone_updater_set_param(rtk_bt_mesh_dfu_standalone_updater_set_param_t *param);

/**
 * @fn        uint16_t rtk_bt_mesh_dfu_standalone_updater_add_receiver(rtk_bt_mesh_dfu_standalone_updater_add_receiver_param_t *param)
 * @brief     Standalone Updater send receiver add message to Target, will cause event @ref RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_ADD_RECEIVER
 * @param[in] param: receiver add message param
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_dfu_standalone_updater_add_receiver(rtk_bt_mesh_dfu_standalone_updater_add_receiver_param_t *param);

/**
 * @fn        uint16_t rtk_bt_mesh_dfu_standalone_updater_start(rtk_bt_mesh_dfu_standalone_update_start_t *param)
 * @brief     Standalone Updater send device firmware update start message to Target, will cause event @ref RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_START
 * @param[in] param: device firmware update start message param
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_mesh_dfu_standalone_updater_start(rtk_bt_mesh_dfu_standalone_update_start_t *param);

#endif  // __RTK_BT_MESH_DFU_MODEL_H__
