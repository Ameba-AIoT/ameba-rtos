/**
 * @file      rtk_bt_mesh_device_firmware_update_model.h
 * @author    pico_zhai@realsil.com.cn
 * @brief     Bluetooth LE MESH device firmware update model api data struct and function definition
 * @copyright Copyright (c) 2024. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_MESH_DEVICE_FIRMWARE_UPDATE_MODEL_H__
#define __RTK_BT_MESH_DEVICE_FIRMWARE_UPDATE_MODEL_H__

#include <stdint.h>
#include <rtk_bt_mesh_def.h>

/** @addtogroup BT_Constants BT Constants
 * @{
 */
/** @addtogroup BT_Mesh_Constants BT Mesh Constants
 * @{
 */
/** @addtogroup BT_Mesh_Device_Firmware_Update_Model_Constants BT Mesh Device Firmware Update Model Constants
 * @{
 */
/**
 * @def       BT_MESH_DFU_FWID_MAX_LEN
 * @brief     Maximum length in bytes of the Firmware ID that the Standalone Updater can set.
 */
#define BT_MESH_DFU_FWID_MAX_LEN  16

/**
 * @def       BT_MESH_DFU_METADATA_MAX_LEN
 * @brief     Maximum length in bytes of the firmware metadata that the Standalone Updater can set.
 */
#define BT_MESH_DFU_METADATA_MAX_LEN  32
/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */

/***************************** Device Firmware Update Model ****************************/
/** @addtogroup BT_Types BT Types
 * @{
 */
/** @addtogroup BT_Mesh_Types BT Mesh Types
 * @{
 */
/** @addtogroup BT_Mesh_Device_Firmware_Update_Model_Types BT Mesh Device Firmware Update Model Types
 * @{
 */
/**
 * @enum      rtk_bt_mesh_dfu_model_initiator_act_t
 * @brief     BLE MESH device firmware update model initiator act definition.
 */
typedef enum {
	RTK_BT_MESH_DFU_ACT_INITIATOR_UPLOAD_START = 1,     /*!< Send a Firmware Distribution Upload Start message to the Distributor. */
	RTK_BT_MESH_DFU_ACT_INITIATOR_RECEIVERS_ADD,        /*!< Send a Firmware Distribution Receivers Add message to the Distributor. */
	RTK_BT_MESH_DFU_ACT_INITIATOR_RECEIVERS_DEL_ALL,    /*!< Send a Firmware Distribution Receivers Delete All message to the Distributor. */
	RTK_BT_MESH_DFU_ACT_INITIATOR_DISTRIBUTE_START,     /*!< Send a Firmware Distribution Start message to the Distributor. */
	RTK_BT_MESH_DFU_ACT_INITIATOR_MAX,                  /*!< Reserved. */
} rtk_bt_mesh_dfu_model_initiator_act_t;

/**
 * @enum      rtk_bt_mesh_dfu_event_initiator_t
 * @brief     BLE MESH device firmware update model initiator event definition.
 */
typedef enum {
	RTK_BT_MESH_DFU_EVT_INITIATOR_BLOB_PARAM = 1,                     /*!< Indicate the BLOB parameters used to upload the firmware image to the Distributor, with msg
                                                                       *   @ref rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t. */
	RTK_BT_MESH_DFU_EVT_INITIATOR_TRANSFER,                           /*!< Indicate the progress or the result of the firmware image upload to the Distributor, with msg
                                                                       *   @ref rtk_bt_mesh_dfu_evt_initiator_transfer_t. */
	RTK_BT_MESH_DFU_EVT_INITIATOR_DIST_CLIENT_UPLOAD_STATUS,          /*!< Indicate a Firmware Distribution Upload Status message received from the Distributor.
                                                                       *   The msg is a 1 byte status code, see @ref rtk_bt_mesh_dfu_fw_dist_status_code_t. */
	RTK_BT_MESH_DFU_EVT_INITIATOR_DIST_CLIENT_RECEIVES_STATUS,        /*!< Indicate a Firmware Distribution Receivers Status message received from the Distributor,
                                                                       *   with msg @ref rtk_bt_mesh_dfu_initiator_evt_dist_client_recvs_status. */
	RTK_BT_MESH_DFU_EVT_INITIATOR_DIST_CLIENT_DISTRIBUTION_STATUS,    /*!< Indicate a Firmware Distribution Status message received from the Distributor.
                                                                       *   The msg is a 1 byte status code, see @ref rtk_bt_mesh_dfu_fw_dist_status_code_t. */
	RTK_BT_MESH_DFU_EVT_INITIATOR_MAX,                                /*!< Reserved. */
} rtk_bt_mesh_dfu_event_initiator_t;

/**
 * @enum      rtk_bt_mesh_dfu_model_standalone_updater_act_t
 * @brief     BLE MESH device firmware update model standalone updater act definition.
 */
typedef enum {
	RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_SET_PARAM = 1,    /*!< Set the firmware image information to update the Targets with. */
	RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_ADD_RECEIVER,     /*!< Add a Target to the receivers list of the Standalone Updater. */
	RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_START,            /*!< Send a Firmware Update Start message to the Target. */
	RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_MAX,              /*!< Reserved. */
} rtk_bt_mesh_dfu_model_standalone_updater_act_t;

#if 0
/**
 * @enum      rtk_bt_mesh_dfu_model_distributor_act_t
 * @brief     BLE MESH device firmware update model distributor act definition.
 */
typedef enum {
	RTK_BT_MESH_DFU_ACT_DISTRIBUTOR_ADD = 1,            /*!< Add a Target to the receivers list of the Distributor. */
	RTK_BT_MESH_DFU_ACT_DISTRIBUTOR_START,              /*!< Start the firmware image distribution to the Targets. */
	RTK_BT_MESH_DFU_ACT_DISTRIBUTOR_SET_NEW_FW_ADDR,    /*!< Set the storage address of the new firmware image. */
	RTK_BT_MESH_DFU_ACT_DISTRIBUTOR_MAX,                /*!< Reserved. */
} rtk_bt_mesh_dfu_model_distributor_act_t;
#endif

/**
 * @enum      rtk_bt_mesh_dfu_event_distributor_t
 * @brief     BLE MESH device firmware update model distributor event definition.
 */
typedef enum {
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_PARAM = 1,             /*!< Indicate the BLOB transfer parameters used to distribute the firmware image to the Targets, with msg
                                                                 *   @ref rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t. */
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_NODE_FAIL,    /*!< Indicate the firmware image distribution to a Target failed, with msg
                                                                 *   @ref rtk_bt_mesh_dfu_evt_distributor_transfer_node_fail_t. */
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_PROGRESS,     /*!< Indicate the progress of the firmware image distribution, with msg
                                                                 *   @ref rtk_bt_mesh_dfu_evt_distributor_transfer_progress_t. */
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_FAIL,         /*!< Indicate the firmware image distribution failed. */
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_SUCCESS,      /*!< Indicate the firmware image has been transferred to the Targets, with msg
                                                                 *   @ref rtk_bt_mesh_dfu_evt_distributor_transfer_other_t. */
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_VERIFY,       /*!< Indicate the Targets verify the firmware image, with msg
                                                                 *   @ref rtk_bt_mesh_dfu_evt_distributor_transfer_other_t. */
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_COMPLETE,     /*!< Indicate the firmware image distribution is complete, with msg
                                                                 *   @ref rtk_bt_mesh_dfu_evt_distributor_transfer_other_t. */
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_UPLOAD_START,               /*!< Indicate a Firmware Distribution Upload Start message received from the Initiator. */
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_UPLOAD_BLOCK_DATA,          /*!< Indicate a block of the firmware image uploaded by the Initiator has been saved.
                                                                 *   The msg is the 2 bytes block number of the saved block. */
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_UPLOAD_COMPLETE,            /*!< Indicate the firmware image upload from the Initiator is complete. */
	RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_MAX,                        /*!< Reserved. */
} rtk_bt_mesh_dfu_event_distributor_t;

/**
 * @enum      rtk_bt_mesh_dfu_event_target_t
 * @brief     BLE MESH device firmware update model target event definition.
 */
typedef enum {
	RTK_BT_MESH_DFU_EVT_TARGET_METADATA_CHECK = 1,    /*!< Indicate the Target needs to check the firmware metadata received in a Firmware Update Start message, with msg
                                                       *   @ref rtk_bt_mesh_dfu_evt_target_metadata_t. */
	RTK_BT_MESH_DFU_EVT_TARGET_START,                 /*!< Indicate a Firmware Update Start message received from the Updater.
                                                       *   The msg is the 1 byte index of the firmware image to update. */
	RTK_BT_MESH_DFU_EVT_TARGET_BLOCK_DATA,            /*!< Indicate a block of the new firmware image has been saved.
                                                       *   The msg is the 2 bytes block number of the saved block. */
	RTK_BT_MESH_DFU_EVT_TARGET_VERIFY,                /*!< Indicate the Target needs to verify the new firmware image. */
	RTK_BT_MESH_DFU_EVT_TARGET_VERIFY_CANCEL,         /*!< Indicate the verification of the new firmware image is cancelled. */
	RTK_BT_MESH_DFU_EVT_TARGET_APPLY,                 /*!< Indicate a Firmware Update Apply message received from the Updater, the Target is going to apply the new
                                                       *   firmware image. */
	RTK_BT_MESH_DFU_EVT_TARGET_FAIL,                  /*!< Indicate the firmware update failed, with msg
                                                       *   @ref rtk_bt_mesh_dfu_evt_target_fw_update_server_fail_t. */
	RTK_BT_MESH_DFU_EVT_TARGET_MAX,                   /*!< Reserved. */
} rtk_bt_mesh_dfu_event_target_t;

/************************** Device Firmware Update Data Types **************************/
/**
 * @enum      rtk_bt_mesh_dfu_model_api_result_t
 * @brief     BLE MESH device firmware update model api result definition.
 */
typedef enum {
	RTK_BT_MESH_DFU_MODEL_API_SUCCESS = 0,    /*!< The API is executed successfully. */
	RTK_BT_MESH_DFU_MODEL_API_FAIL,           /*!< The API is failed to execute. */
} rtk_bt_mesh_dfu_model_api_result_t;

/**
 * @def       RTK_BT_MESH_FW_VERSION_MAX_LEN
 * @brief     Maximum length in bytes of the Version Information of a Firmware ID.
 */
#define RTK_BT_MESH_FW_VERSION_MAX_LEN                              106

/**
 * @struct    rtk_bt_mesh_dfu_fw_id_t
 * @brief     BLE MESH device firmware update firmware id data structure.
 */
typedef struct {
	uint16_t company_id;                                /*!< Company ID that identifies the firmware vendor. */
	uint8_t version[RTK_BT_MESH_FW_VERSION_MAX_LEN];    /*!< Version Information, vendor-specific information describing the firmware binary package.
                                                         *   Note: If the firmware image is updated more than once,
                                                         *   the version of the later firmware image shall be larger than the previous one. */
} _PACKED4_ rtk_bt_mesh_dfu_fw_id_t;

/**
 * @struct    rtk_bt_mesh_dfu_initiator_upload_start_t
 * @brief     BLE MESH device firmware update initiator Firmware Distribution Upload Start message structure.
 */
typedef struct {
	uint16_t dst;                     /*!< Unicast address of the Distributor. */
	uint16_t app_key_index;           /*!< Index of the application key used to encrypt the message.
                                       *   Range: 0x0000 to 0x0FFF. */
	uint16_t update_timeout_base;     /*!< Upload Timeout Base, used to compute the timeout of the firmware image upload. */
	uint32_t fw_image_size;           /*!< Size in bytes of the firmware image to upload. */
	uint16_t metadata_len;            /*!< Length in bytes of the firmware metadata in p_metadata.
                                       *   Range: 0 to 255. */
	uint8_t *p_metadata;              /*!< Vendor-specific firmware metadata, metadata_len bytes.
                                       *   Used to check whether the firmware image can be updated.
                                       *   Ignored if metadata_len is 0. */
	uint8_t fw_id_len;                /*!< Length in bytes of the Firmware ID in fw_id, the Company ID (2 bytes) plus the Version Information.
                                       *   Range: 2 to 108. */
	rtk_bt_mesh_dfu_fw_id_t fw_id;    /*!< Firmware ID that identifies the firmware image to upload.
                                       *   Note: If the firmware image is updated more than once,
                                       *   the Firmware ID of the later firmware image shall be larger than the previous one. */
} rtk_bt_mesh_dfu_initiator_upload_start_t;

/**
 * @enum      rtk_bt_mesh_dfu_initiator_cb_type_t
 * @brief     BLE MESH device firmware update initiator callback type structure.
 */
typedef enum {
	RTK_BT_MESH_DFU_INIT_CB_TYPE_UPLOAD_PROGRESS,    /*!< The firmware image upload to the Distributor is in progress. */
	RTK_BT_MESH_DFU_INIT_CB_TYPE_UPLOAD_SUCCESS,     /*!< The firmware image upload to the Distributor succeeded. */
	RTK_BT_MESH_DFU_INIT_CB_TYPE_UPLOAD_FAIL,        /*!< The firmware image upload to the Distributor failed. */
} rtk_bt_mesh_dfu_initiator_cb_type_t;

/**
 * @enum      rtk_bt_mesh_dfu_initiator_phase_t
 * @brief     BLE MESH device firmware update initiator phase structure.
 */
typedef enum {
	RTK_BT_MESH_DFU_INIT_PHASE_IDLE,             /*!< No firmware image upload is in progress. */
	RTK_BT_MESH_DFU_INIT_PHASE_UPLOAD_START,     /*!< The Initiator is waiting for the Distributor to accept the firmware image upload. */
	RTK_BT_MESH_DFU_INIT_PHASE_BLOB_TRANSFER,    /*!< The Initiator is transferring the firmware image to the Distributor. */
	RTK_BT_MESH_DFU_INIT_PHASE_FAILED,           /*!< The Distributor did not respond to the Firmware Distribution Upload Start message. */
} rtk_bt_mesh_dfu_initiator_phase_t;

/**
 * @struct    rtk_bt_mesh_dfu_evt_initiator_transfer_t
 * @brief     BLE MESH device firmware update distributor transfer structure.
 */
typedef struct {
	rtk_bt_mesh_dfu_initiator_cb_type_t type;        /*!< Type of the upload notification. */
	rtk_bt_mesh_dfu_initiator_phase_t init_phase;    /*!< Phase of the Initiator when the notification is raised. */
	union {
		uint16_t addr;                               /*!< Unicast address of the Distributor.
                                                      *   It is reported when type is @ref RTK_BT_MESH_DFU_INIT_CB_TYPE_UPLOAD_SUCCESS
                                                      *   or @ref RTK_BT_MESH_DFU_INIT_CB_TYPE_UPLOAD_FAIL. */
		uint8_t progress;                            /*!< Progress of the upload, in percent.
                                                      *   Range: 0 to 100.
                                                      *   It is reported when type is @ref RTK_BT_MESH_DFU_INIT_CB_TYPE_UPLOAD_PROGRESS. */
	};
} rtk_bt_mesh_dfu_evt_initiator_transfer_t;

/**
 * @struct    rtk_bt_mesh_dfu_initiator_dist_recv_t
 * @brief     BLE MESH device firmware update initiator Firmware Distribution Receivers Add message structure.
 */
typedef struct {
	uint16_t addr;                  /*!< Unicast address of the Target. */
	uint8_t update_fw_image_idx;    /*!< Index of the firmware image in the Firmware Information List state of the Target to be updated. */
} rtk_bt_mesh_dfu_initiator_dist_recv_t;

/**
 * @struct    rtk_bt_mesh_dfu_initiator_dist_recv_add_t
 * @brief     BLE MESH device firmware update initiator distribute receiver add param structure.
 */
typedef struct {
	uint16_t dst;                                   /*!< Unicast address of the Distributor. */
	uint16_t app_key_index;                         /*!< Index of the application key used to encrypt the message.
                                                     *   Range: 0x0000 to 0x0FFF. */
	uint8_t recv_num;                               /*!< Number of entries in recv.
                                                     *   The address of each entry shall be unique. */
	rtk_bt_mesh_dfu_initiator_dist_recv_t *recv;    /*!< Array of the Targets to add to the Distribution Receivers List state of the Distributor, recv_num entries.
                                                     *   Must not be NULL. */
} rtk_bt_mesh_dfu_initiator_dist_recv_add_t;

/**
 * @struct    rtk_bt_mesh_dfu_initiator_dist_recv_del_all_t
 * @brief     BLE MESH device firmware update initiator distribute receiver delete all param structure.
 */
typedef struct {
	uint16_t dst;              /*!< Unicast address of the Distributor. */
	uint16_t app_key_index;    /*!< Index of the application key used to encrypt the message.
                                *   Range: 0x0000 to 0x0FFF. */
} rtk_bt_mesh_dfu_initiator_dist_recv_del_all_t;

/**
 * @enum      rtk_bt_mesh_dfu_blob_transfer_mode_t
 * @brief     BLE MESH device firmware update blob transfer mode param structure.
 */
typedef enum {
	RTK_BT_MESH_DFU_BLOB_TRANSFER_MODE_IDLE,    /*!< No active transfer. */
	RTK_BT_MESH_DFU_BLOB_TRANSFER_MODE_PUSH,    /*!< Push BLOB transfer mode. */
	RTK_BT_MESH_DFU_BLOB_TRANSFER_MODE_PULL,    /*!< Pull BLOB transfer mode. */
} rtk_bt_mesh_dfu_blob_transfer_mode_t;

/**
 * @enum      rtk_bt_mesh_dfu_initiator_fw_update_policy_t
 * @brief     BLE MESH device firmware update initiator firmware update policy param structure.
 */
typedef enum {
	RTK_BT_MESH_DFU_INITIATOR_FW_UPDATE_POLICY_VERIFY_ONLY,          /*!< Verify Only, the Distributor verifies the firmware image distribution but does not apply the
                                                                      *   update.
                                                                      *   The Initiator (the Firmware Distribution Client) initiates the firmware image application. */
	RTK_BT_MESH_DFU_INITIATOR_FW_UPDATE_POLICY_VERIFY_AND_UPDATE,    /*!< Verify And Apply, the Distributor verifies the firmware image distribution and then applies the
                                                                      *   update. */
} rtk_bt_mesh_dfu_initiator_fw_update_policy_t;

/**
 * @struct    rtk_bt_mesh_dfu_initiator_fw_dist_start_data_t
 * @brief     BLE MESH device firmware update initiator Firmware Distribution Start message structure.
 */
typedef struct {
	uint16_t dist_app_key_index;        /*!< Index of the application key used by the Distributor in the firmware image distribution.
                                         *   Range: 0x0000 to 0x0FFF. */
	uint8_t dist_ttl;                   /*!< TTL value used by the Distributor in the firmware image distribution.
                                         *   0x00 to 0x7F: TTL field value of a Network PDU.
                                         *   0x80 to 0xFE: Prohibited.
                                         *   0xFF: The Default TTL state value is used. */
	uint16_t dist_timeout_base;         /*!< Distribution Timeout Base, used to compute the timeout of the firmware image distribution.
                                         *   Timeout = 10 * (dist_timeout_base + 1) s. */
	uint8_t dist_transfer_mode : 2;     /*!< Mode used to transfer the BLOB to the Targets, @ref rtk_bt_mesh_dfu_blob_transfer_mode_t. */
	uint8_t update_policy : 1;          /*!< Firmware update policy of the Distributor, @ref rtk_bt_mesh_dfu_initiator_fw_update_policy_t. */
	uint8_t rfu : 5;                    /*!< Reserved for future use, shall be 0. */
	uint16_t dist_fw_image_idx;         /*!< Index of the firmware image in the Firmware Images List state of the Distributor to distribute. */
	uint8_t dist_multicast_addr[16];    /*!< Multicast address of the Targets that the firmware image is distributed to.
                                         *   It is a group address (2 bytes) or the Label UUID of a virtual address (16 bytes), the length is given by dist_dst_len
                                         *   of rtk_bt_mesh_dfu_initiator_fw_dist_start_param_t.
                                         *   The Unassigned address means the messages are not sent to a multicast address. */
} _PACKED4_ rtk_bt_mesh_dfu_initiator_fw_dist_start_data_t;

/**
 * @struct    rtk_bt_mesh_dfu_initiator_fw_dist_start_param_t
 * @brief     BLE MESH device firmware update initiator distribute start param structure.
 */
typedef struct {
	uint16_t dst;                                           /*!< Unicast address of the Distributor. */
	uint16_t app_key_index;                                 /*!< Index of the application key used to encrypt the message.
                                                             *   Range: 0x0000 to 0x0FFF. */
	uint8_t dist_dst_len;                                   /*!< Length in bytes of dist_multicast_addr in data that is sent.
                                                             *   2: dist_multicast_addr is a group address.
                                                             *   16: dist_multicast_addr is the Label UUID of a virtual address. */
	rtk_bt_mesh_dfu_initiator_fw_dist_start_data_t data;    /*!< Parameters of the firmware image distribution. */
} rtk_bt_mesh_dfu_initiator_fw_dist_start_param_t;

/**
 * @enum      rtk_bt_mesh_dfu_fw_dist_status_code_t
 * @brief     BLE MESH device firmware update initiator distribute client receives status.
 */
typedef enum {
	RTK_BT_MESH_DFU_FW_DIST_STATUS_SUCCESS,                   /*!< Status code 0x00: Success, the message was processed successfully. */
	RTK_BT_MESH_DFU_FW_DIST_STATUS_INSUFFICIENT_RESOURCES,    /*!< Status code 0x01: Insufficient Resources, insufficient resources on the node. */
	RTK_BT_MESH_DFU_FW_DIST_STATUS_WRONG_PHASE,               /*!< Status code 0x02: Wrong Phase, the operation cannot be performed while the server is in the current
                                                               *   phase. */
	RTK_BT_MESH_DFU_FW_DIST_STATUS_INTERNAL_ERR,              /*!< Status code 0x03: Internal Error, an internal error occurred on the node. */
	RTK_BT_MESH_DFU_FW_DIST_STATUS_FW_NOT_FOUND,              /*!< Status code 0x04: Firmware Not Found, the requested firmware image is not stored on the Distributor. */
	RTK_BT_MESH_DFU_FW_DIST_STATUS_INVALID_APP_KEY_INDEX,     /*!< Status code 0x05: Invalid AppKey Index, the AppKey identified by the AppKey Index is not known to the
                                                               *   node. */
	RTK_BT_MESH_DFU_FW_DIST_STATUS_RECEIVERS_LIST_EMPTY,      /*!< Status code 0x06: Receivers List Empty, there are no Target nodes in the Distribution Receivers List
                                                               *   state. */
	RTK_BT_MESH_DFU_FW_DIST_STATUS_BUSY_WITH_DIST,            /*!< Status code 0x07: Busy With Distribution, another firmware image distribution is in progress. */
	RTK_BT_MESH_DFU_FW_DIST_STATUS_BUSY_WITH_UPLOAD,          /*!< Status code 0x08: Busy With Upload, another upload is in progress. */
	RTK_BT_MESH_DFU_FW_DIST_STATUS_URI_NOT_SUPPORTED,         /*!< Status code 0x09: URI Not Supported, the URI scheme indicated by the Update URI is not supported. */
	RTK_BT_MESH_DFU_FW_DIST_STATUS_URI_MALFORMED,             /*!< Status code 0x0A: URI Malformed, the format of the Update URI is invalid. */
	RTK_BT_MESH_DFU_FW_DIST_STATUS_URI_UNREACHABLE,           /*!< Status code 0x0B: URI Unreachable, the URI is unreachable. */
	RTK_BT_MESH_DFU_FW_DIST_STATUS_NEW_FW_NOT_AVAILABLE,      /*!< Status code 0x0C: New Firmware Not Available, the Check Firmware OOB procedure did not find any new
                                                               *   firmware. */
	RTK_BT_MESH_DFU_FW_DIST_STATUS_SUSPEND_FAILED,            /*!< Status code 0x0D: Suspend Failed, the suspension of the Distribute Firmware procedure failed. */
} rtk_bt_mesh_dfu_fw_dist_status_code_t;

/**
 * @struct    rtk_bt_mesh_dfu_initiator_evt_dist_client_recvs_status
 * @brief     BLE MESH device firmware update initiator distribute client receives status data structure.
 */
typedef struct {
	uint16_t src;                                    /*!< Unicast address of the Distributor that sent the Status message. */
	rtk_bt_mesh_dfu_fw_dist_status_code_t status;    /*!< Status code for the last operation on the Firmware Distribution states of the Distributor. */
	uint16_t recvs_list_cnt;                         /*!< Number of entries in the Distribution Receivers List state of the Distributor. */
} rtk_bt_mesh_dfu_initiator_evt_dist_client_recvs_status;

/**
 * @struct    rtk_bt_mesh_dfu_standalone_updater_set_param_t
 * @brief     BLE MESH device firmware update standalone updater set relate param structure.
 */
typedef struct {
	uint32_t image_size;                                /*!< Size of the firmware image to send to the Targets.
                                                         *   Unit: byte. */
	uint8_t fw_id_len;                                  /*!< Length of the Firmware ID in fw_id.
                                                         *   Unit: byte.
                                                         *   Range: 0 to @ref BT_MESH_DFU_FWID_MAX_LEN. */
	uint8_t meta_data_len;                              /*!< Length of the Firmware Metadata in meta_data.
                                                         *   Unit: byte.
                                                         *   Range: 0 to @ref BT_MESH_DFU_METADATA_MAX_LEN. */
	uint8_t fw_id[BT_MESH_DFU_FWID_MAX_LEN];            /*!< Firmware ID of the firmware image to send. */
	uint8_t meta_data[BT_MESH_DFU_METADATA_MAX_LEN];    /*!< Vendor-specific Firmware Metadata of the firmware image to send. */
} rtk_bt_mesh_dfu_standalone_updater_set_param_t;

/**
 * @struct    rtk_bt_mesh_dfu_standalone_updater_add_receiver_param_t
 * @brief     BLE MESH device firmware update standalone updater add receiver param structure.
 */
typedef struct {
	uint16_t addr;                 /*!< Unicast address of the Target to add to the Update Receivers list. */
	uint8_t update_image_index;    /*!< Index of the firmware image in the Firmware Information List state of the Target that is to be updated. */
} rtk_bt_mesh_dfu_standalone_updater_add_receiver_param_t;

/**
 * @struct    rtk_bt_mesh_dfu_standalone_update_start_t
 * @brief     BLE MESH device firmware update standalone updater Firmware Update Start message structure.
 */
typedef struct {
	uint16_t dst;                    /*!< Unicast address of the Target. */
	uint16_t app_key_index;          /*!< Index of the application key used to encrypt the message.
                                      *   Range: 0x0000 to 0x0FFF. */
	uint16_t update_timeout_base;    /*!< Update Timeout Base, used to calculate when the firmware image transfer is suspended.
                                      *   Timeout = 10 * (update_timeout_base + 1) s. */
} rtk_bt_mesh_dfu_standalone_update_start_t;

/**
 * @struct    rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t
 * @brief     BLE MESH device firmware update distributor or initiator blob param structure.
 */
typedef struct {
	uint32_t blob_size;       /*!< Size of the BLOB, that is the firmware image.
                               *   Unit: byte. */
	uint16_t block_size;      /*!< Size of a block of the BLOB.
                               *   Unit: byte. */
	uint16_t total_blocks;    /*!< Number of blocks of the BLOB. */
	uint16_t chunk_size;      /*!< Size of a chunk of a block.
                               *   Unit: byte. */
} rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t;

/**
 * @enum      rtk_bt_mesh_dfu_dist_phase_t
 * @brief     BLE MESH device firmware update distributor transfer phase structure.
 */
typedef enum {
	RTK_BT_MESH_DFU_DIST_PHASE_IDLE,             /*!< No firmware distribution is in progress. */
	RTK_BT_MESH_DFU_DIST_PHASE_UPDATE_START,     /*!< Firmware Update Start messages are being sent to the Targets. */
	RTK_BT_MESH_DFU_DIST_PHASE_BLOB_TRANSFER,    /*!< The BLOB is being transferred to the Targets. */
	RTK_BT_MESH_DFU_DIST_PHASE_VERIFY,           /*!< The BLOB has been transferred, the Targets are verifying the firmware image. */
	RTK_BT_MESH_DFU_DIST_PHASE_APPLY,            /*!< Firmware Update Apply messages are being sent to the Targets. */
	RTK_BT_MESH_DFU_DIST_PHASE_CONFIRM,          /*!< The result of the firmware update is being read from the Targets. */
	RTK_BT_MESH_DFU_DIST_PHASE_CANCELING,        /*!< The firmware update is being canceled. */
	RTK_BT_MESH_DFU_DIST_PHASE_UNKNOWN,          /*!< The phase is unknown. */
} rtk_bt_mesh_dfu_dist_phase_t;

/**
 * @struct    rtk_bt_mesh_dfu_evt_distributor_transfer_node_fail_t
 * @brief     BLE MESH device firmware update distributor transfer node fail data structure.
 */
typedef struct {
	rtk_bt_mesh_dfu_dist_phase_t dist_phase;    /*!< Phase of the firmware distribution in which the Target failed. */
	uint16_t addr;                              /*!< Unicast address of the Target that failed. */
} rtk_bt_mesh_dfu_evt_distributor_transfer_node_fail_t;

/**
 * @struct    rtk_bt_mesh_dfu_evt_distributor_transfer_progress_t
 * @brief     BLE MESH device firmware update distributor transfer node fail data structure.
 */
typedef struct {
	rtk_bt_mesh_dfu_dist_phase_t dist_phase;    /*!< Phase of the firmware distribution. */
	uint8_t progress;                           /*!< Progress of the BLOB transfer.
                                                 *   Unit: percent.
                                                 *   Range: 0 to 100. */
} rtk_bt_mesh_dfu_evt_distributor_transfer_progress_t;

/**
 * @struct    rtk_bt_mesh_dfu_evt_distributor_transfer_other_t
 * @brief     BLE MESH device firmware update distributor transfer other data structure.
 */
typedef struct {
	rtk_bt_mesh_dfu_dist_phase_t dist_phase;    /*!< Phase of the firmware distribution. */
	uint8_t addr_num;                           /*!< Number of Target addresses in paddr. */
	uint16_t *paddr;                            /*!< Unicast addresses of the Targets, addr_num entries of 2 bytes each.
                                                 *   The addresses are located right after the structure in the event data. */
} rtk_bt_mesh_dfu_evt_distributor_transfer_other_t;

/**
 * @struct    rtk_bt_mesh_dfu_evt_distributor_server_upload_start_t
 * @brief     BLE MESH device firmware update distributor server upload start data structure.
 */
typedef struct {
	uint8_t upload_ttl;                /*!< TTL used during the firmware image upload. */
	uint16_t upload_timeout_base;      /*!< Upload Timeout Base, used to calculate when the firmware image upload is suspended. */
	uint8_t upload_blob_id[8];         /*!< BLOB identifier used for the firmware image upload. */
	uint32_t upload_fw_size;           /*!< Size of the firmware image to upload.
                                        *   Unit: byte. */
	uint8_t upload_fw_metadata_len;    /*!< Length of the Firmware Metadata in pupload_metadata.
                                        *   Unit: byte. */
	uint8_t *pupload_metadata;         /*!< Pointer to the vendor-specific Firmware Metadata of the firmware image to upload. */
	uint8_t upload_fw_id_len;          /*!< Length of the Firmware ID in pupload_fw_id.
                                        *   Unit: byte. */
	uint8_t *pupload_fw_id;            /*!< Pointer to the Firmware ID of the firmware image to upload. */
} rtk_bt_mesh_dfu_evt_distributor_server_upload_start_t;

/**
 * @enum      rtk_bt_mesh_dfu_fw_upload_fail_reason_t
 * @brief     BLE MESH device firmware update distributor firmware upload fail reason definition.
 */
typedef enum {
	RTK_BT_MESH_DFU_FW_UPLOAD_FAIL_REASON_USER_CANCEL,         /*!< The upload was canceled by the user. */
	RTK_BT_MESH_DFU_FW_UPLOAD_FAIL_REASON_TRANSFER_TIMEOUT,    /*!< The transfer of the upload timed out. */
	RTK_BT_MESH_DFU_FW_UPLOAD_FAIL_REASON_TRANSFER_ERR,        /*!< The transfer of the upload failed with an error. */
} rtk_bt_mesh_dfu_fw_upload_fail_reason_t;

/**
 * @struct    rtk_bt_mesh_dfu_evt_distributor_server_upload_fail_t
 * @brief     BLE MESH device firmware update distributor server upload fail data structure.
 */
typedef struct {
	rtk_bt_mesh_dfu_fw_upload_fail_reason_t reason;    /*!< Reason of the firmware upload failure. */
} rtk_bt_mesh_dfu_evt_distributor_server_upload_fail_t;

/**
 * @struct    rtk_bt_mesh_dfu_evt_target_metadata_t
 * @brief     BLE MESH device firmware update target metadata structure.
 */
typedef struct {
	uint8_t fw_image_idx;    /*!< Index of the firmware image in the Firmware Information List state that is to be updated. */
	uint8_t *pmetadata;      /*!< Pointer to the vendor-specific Incoming Firmware Metadata, used to check whether the firmware image can be updated. */
	uint8_t metadata_len;    /*!< Length of the Incoming Firmware Metadata in pmetadata.
                              *   Unit: byte. */
} rtk_bt_mesh_dfu_evt_target_metadata_t;

/**
 * @typedef   rtk_bt_mesh_dfu_evt_target_meatdata_t
 * @brief     Deprecated alias of @ref rtk_bt_mesh_dfu_evt_target_metadata_t.
 * @deprecated Use @ref rtk_bt_mesh_dfu_evt_target_metadata_t instead.
 */
typedef rtk_bt_mesh_dfu_evt_target_metadata_t rtk_bt_mesh_dfu_evt_target_meatdata_t;

/**
 * @enum      rtk_bt_mesh_dfu_target_fw_update_fail_reason_t
 * @brief     BLE MESH device firmware update target fw update server fail reason.
 */
typedef enum {
	RTK_BT_MESH_DFU_TARGET_FW_UPDATE_FAIL_REASON_USER_CANCEL,         /*!< The firmware update was canceled by the user. */
	RTK_BT_MESH_DFU_TARGET_FW_UPDATE_FAIL_REASON_TRANSFER_TIMEOUT,    /*!< The transfer of the firmware update timed out. */
	RTK_BT_MESH_DFU_TARGET_FW_UPDATE_FAIL_REASON_TRANSFER_ERR,        /*!< The transfer of the firmware update failed with an error. */
} rtk_bt_mesh_dfu_target_fw_update_fail_reason_t;

/**
 * @struct    rtk_bt_mesh_dfu_evt_target_fw_update_server_fail_t
 * @brief     BLE MESH device firmware update target fw update server fail data structure.
 */
typedef struct {
	uint8_t fw_image_index;                                   /*!< Index of the firmware image in the Firmware Information List state whose update failed. */
	rtk_bt_mesh_dfu_target_fw_update_fail_reason_t reason;    /*!< Reason of the firmware update failure. */
} rtk_bt_mesh_dfu_evt_target_fw_update_server_fail_t;

/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */

/********************************* Functions Declaration *******************************/
/** @addtogroup BT_Functions BT Functions
 * @{
 */
/** @addtogroup BT_Mesh_Functions BT Mesh Functions
 * @{
 */
/** @addtogroup BT_Mesh_Device_Firmware_Update_Model_Functions BT Mesh Device Firmware Update Model Functions
 * @{
 */

/**
 * @brief     Send a Firmware Distribution Upload Start message to a Distributor to start uploading a firmware image to it.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DFU_EVT_INITIATOR_DIST_CLIENT_UPLOAD_STATUS.
 * @param[in] param: Firmware Distribution Upload Start message parameters.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_dfu_initiator_upload_start(rtk_bt_mesh_dfu_initiator_upload_start_t *param);

/**
 * @brief     Send a Firmware Distribution Receivers Add message to a Distributor to add Targets to its Distribution Receivers List.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DFU_EVT_INITIATOR_DIST_CLIENT_RECEIVES_STATUS.
 * @param[in] param: Firmware Distribution Receivers Add message parameters.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_dfu_initiator_receivers_add(rtk_bt_mesh_dfu_initiator_dist_recv_add_t *param);

/**
 * @brief     Send a Firmware Distribution Receivers Delete All message to a Distributor to delete all Targets from its Distribution Receivers List.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DFU_EVT_INITIATOR_DIST_CLIENT_RECEIVES_STATUS.
 * @param[in] param: Firmware Distribution Receivers Delete All message parameters.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Send this message before the next firmware update if a firmware update has been executed before.
 */
uint16_t rtk_bt_mesh_dfu_initiator_receivers_delete_all(rtk_bt_mesh_dfu_initiator_dist_recv_del_all_t *param);

/**
 * @brief     Send a Firmware Distribution Start message to a Distributor to start distributing the firmware image to the Targets.
 *            The result is reported asynchronously via event @ref RTK_BT_MESH_DFU_EVT_INITIATOR_DIST_CLIENT_DISTRIBUTION_STATUS.
 * @param[in] param: Firmware Distribution Start message parameters.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Add the Targets with @ref rtk_bt_mesh_dfu_initiator_receivers_add first. The Distributor rejects the message if its
 *              Distribution Receivers List is empty.
 */
uint16_t rtk_bt_mesh_dfu_initiator_distribute_start(rtk_bt_mesh_dfu_initiator_fw_dist_start_param_t *param);

/**
 * @brief     Set the firmware image information that the Standalone Updater sends to the Targets.
 * @param[in] param: Firmware image information.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Call it before @ref rtk_bt_mesh_dfu_standalone_updater_start.
 */
uint16_t rtk_bt_mesh_dfu_standalone_updater_set_param(rtk_bt_mesh_dfu_standalone_updater_set_param_t *param);

/**
 * @brief     Add a Target to the Update Receivers list of the Standalone Updater.
 * @param[in] param: Target to add.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 */
uint16_t rtk_bt_mesh_dfu_standalone_updater_add_receiver(rtk_bt_mesh_dfu_standalone_updater_add_receiver_param_t *param);

/**
 * @brief     Send a Firmware Update Start message to a Target to start the firmware update.
 *            The firmware image transfer to the Targets is reported asynchronously, starting with event @ref RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_PARAM.
 * @param[in] param: Firmware Update Start message parameters.
 *                   Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - Call @ref rtk_bt_mesh_dfu_standalone_updater_set_param first.
 *            - Add the Targets with @ref rtk_bt_mesh_dfu_standalone_updater_add_receiver first.
 */
uint16_t rtk_bt_mesh_dfu_standalone_updater_start(rtk_bt_mesh_dfu_standalone_update_start_t *param);

/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */

#endif /* __RTK_BT_MESH_DEVICE_FIRMWARE_UPDATE_MODEL_H__ */
