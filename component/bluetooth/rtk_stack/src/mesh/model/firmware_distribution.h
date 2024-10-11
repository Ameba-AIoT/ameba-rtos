/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     firmware_distribution.h
* @brief    Head file for firmware distribution models.
* @details  Data types and external functions declaration.
* @author   bill
* @date     2018-5-21
* @version  v1.0
* *************************************************************************************
*/

/* Define to prevent recursive inclusion */
#ifndef _FIRMWARE_DISTRIBUTION_H
#define _FIRMWARE_DISTRIBUTION_H

/* Add Includes here */
#include "platform_misc.h"
#include "mesh_api.h"
#include "blob_transfer.h"
#include "firmware_update.h"

#if MESH_DFU

BEGIN_DECLS

/**
 * @addtogroup FIRMWARE_DISTRIBUTION
 * @{
 */

/* firmware distribution server capabilities */
#define FW_DIST_CAPS_MAX_DIST_RECVS_LIST_SIZE                 20
#define FW_DIST_CAPS_MAX_FW_IMAGES_LIST_SIZE                  2
#define FW_DIST_CAPS_MAX_FW_IMAGE_SIZE                        307200
#define FW_DIST_CAPS_MAX_UPLOAD_SPACE                         512000
#define FW_DIST_CAPS_OOB_RETRIEVAL_SUPPORTED                  0

/**
 * @defgroup FIRMWARE_DISTRIBUTION_ACCESS_OPCODE Access Opcode
 * @brief Mesh message access opcode
 * @{
 */
#define MESH_MSG_FW_DIST_RECVS_ADD                      0x8311
#define MESH_MSG_FW_DIST_RECVS_DELETE_ALL               0x8312
#define MESH_MSG_FW_DIST_RECVS_STATUS                   0x8313
#define MESH_MSG_FW_DIST_RECVS_GET                      0x8314
#define MESH_MSG_FW_DIST_RECVS_LIST                     0x8315
#define MESH_MSG_FW_DIST_CAPS_GET                       0x8316
#define MESH_MSG_FW_DIST_CAPS_STATUS                    0x8317
#define MESH_MSG_FW_DIST_GET                            0x8318
#define MESH_MSG_FW_DIST_START                          0x8319
#define MESH_MSG_FW_DIST_SUSPEND                        0x831A
#define MESH_MSG_FW_DIST_CANCEL                         0x831B
#define MESH_MSG_FW_DIST_APPLY                          0x831C
#define MESH_MSG_FW_DIST_STATUS                         0x831D
#define MESH_MSG_FW_DIST_UPLOAD_GET                     0x831E
#define MESH_MSG_FW_DIST_UPLOAD_START                   0x831F
#define MESH_MSG_FW_DIST_UPLOAD_OOB_START               0x8320
#define MESH_MSG_FW_DIST_UPLOAD_CANCEL                  0x8321
#define MESH_MSG_FW_DIST_UPLOAD_STATUS                  0x8322
#define MESH_MSG_FW_DIST_FW_GET                         0x8323
#define MESH_MSG_FW_DIST_FW_GET_BY_INDEX                0x8324
#define MESH_MSG_FW_DIST_FW_DELETE                      0x8325
#define MESH_MSG_FW_DIST_FW_DELETE_ALL                  0x8326
#define MESH_MSG_FW_DIST_FW_STATUS                      0x8327
/** @} */

/**
 * @defgroup FIRMWARE_DISTRIBUTION_MODEL_ID Model ID
 * @brief Mesh model id
 * @{
 */
#define MESH_MODEL_FW_DIST_SERVER                       0x1404FFFF
#define MESH_MODEL_FW_DIST_CLIENT                       0x1405FFFF
/** @} */

/**
 * @defgroup FIRMWARE_DISTRIBUTION_MESH_MSG Mesh Msg
 * @brief Mesh message types used by models
 * @{
 */
#define FW_IMAGE_INDEX_INVALID                         0xFFFF

typedef struct
{
    uint8_t fw_id_len;
    fw_id_t fw_id;
    uint8_t metadata_len;
    uint8_t metadata[255];
} _PACKED4_ fw_image_t;

enum
{
    FW_DIST_PHASE_IDLE,                 // No firmware distribution is in progress
    FW_DIST_PHASE_TRANSFER_ACTIVE,      // Firmware distribution is in progress
    FW_DIST_PHASE_TRANSFER_SUCCESS,     // The Transfer BLOB procedure has completed successfully
    FW_DIST_PHASE_APPLYING_UPDATE,      // The Apply Firmware On Target Nodes procedure is being executed
    FW_DIST_PHASE_COMPLETED,            // The Distribute Firmware procedure has completed successfully
    FW_DIST_PHASE_FAILED,               // The Distribute Firmware procedure has failed
    FW_DIST_PHASE_CANCELING_UPDATE,     // The Cancel Firmware Update procedure is being executed
    FW_DIST_PHASE_TRANSFER_SUSPENDED,   // The Transfer BLOB procedure is suspended
} _SHORT_ENUM_;
typedef uint8_t fw_dist_phase_t;

enum
{
    FW_UPLOAD_PHASE_IDLE,               // No firmware upload is in progress
    FW_UPLOAD_PHASE_TRANSFER_ACTIVE,    // The Store Firmware or Store Firmware OOB procedure is being executed
    FW_UPLOAD_PHASE_TRANSFER_ERR,       // The Store Firmware procedure or Store Firmware OOB procedure failed
    FW_UPLOAD_PHASE_TRANSFER_SUCCESS,   // The Store Firmware procedure or the Store Firmware OOB procedure completed successfully
} _SHORT_ENUM_;
typedef uint8_t fw_upload_phase_t;

enum
{
    FW_DIST_STATUS_SUCCESS,                 // The message was processed successfully
    FW_DIST_STATUS_INSUFFICIENT_RESOURCES,  // Insufficient resources on the nod
    FW_DIST_STATUS_WRONG_PHASE,             // The operation cannot be performed while the server is in the current phase
    FW_DIST_STATUS_INTERNAL_ERR,            // An internal error occurred on the node
    FW_DIST_STATUS_FW_NOT_FOUND,            // The requested firmware image is not stored on the Distributor
    FW_DIST_STATUS_INVALID_APP_KEY_INDEX,   // The AppKey identified by the AppKey Index is not known to the node
    FW_DIST_STATUS_RECEIVERS_LIST_EMPTY,    // There are no Target nodes in the Distribution Receivers List state
    FW_DIST_STATUS_BUSY_WITH_DIST,          // Another firmware image distribution is in progress
    FW_DIST_STATUS_BUSY_WITH_UPLOAD,        // Another upload is in progress
    FW_DIST_STATUS_URI_NOT_SUPPORTED,       // The URI scheme name indicated by the Update URI is not supported
    FW_DIST_STATUS_URI_MALFORMED,           // The format of the Update URI is invalid
    FW_DIST_STATUS_URI_UNREACHABLE,         // The URI is unreachable
    FW_DIST_STATUS_NEW_FW_NOT_AVAILABLE,    // The Check Firmware OOB procedure did not find any new firmware
    FW_DIST_STATUS_SUSPEND_FAILED,          // The suspension of the Distribute Firmware procedure failed
} _SHORT_ENUM_;
typedef uint8_t fw_dist_status_code_t;

enum
{
    /* The Firmware Distribution Server verifies that firmware image
       distribution completed successfully but does not apply the update. The
       Initiator (the Firmware Distribution Client) initiates firmware image
       application */
    FW_UPDATE_POLICY_VERIFY_ONLY,
    /* The Firmware Distribution Server verifies that firmware image
       distribution completed successfully and then applies the firmware
       update */
    FW_UPDATE_POLICY_VERIFY_AND_UPDATE,
} _SHORT_ENUM_;
typedef uint8_t fw_update_policy_t;

typedef struct
{
    uint16_t addr;
    uint8_t update_fw_image_idx;
} _PACKED4_ fw_dist_receiver_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_RECVS_ADD)];
    fw_dist_receiver_t entries[0];
} _PACKED4_ fw_dist_recvs_add_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_RECVS_DELETE_ALL)];
} _PACKED4_ fw_dist_recvs_delete_all_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_RECVS_STATUS)];
    fw_dist_status_code_t status;
    uint16_t recvs_list_cnt;
} _PACKED4_ fw_dist_recvs_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_RECVS_GET)];
    uint16_t first_index;
    uint16_t entries_limit;
} _PACKED4_ fw_dist_recvs_get_t;

typedef struct
{
    uint32_t addr : 15;
    uint32_t retrieved_update_phase : 4;
    uint32_t update_status : 3;
    uint32_t transfer_status : 4;
    uint32_t transfer_progress : 6;
    uint8_t update_fw_image_idx;
} _PACKED4_ fw_update_node_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_RECVS_LIST)];
    uint16_t recvs_list_cnt;
    uint16_t first_index;
    fw_update_node_t entries[0];
} _PACKED4_ fw_dist_recvs_list_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_CAPS_GET)];
} _PACKED4_ fw_dist_caps_get_t;

typedef struct
{
    uint16_t max_dist_recvs_list_size;
    uint16_t max_fw_images_list_size;
    uint32_t max_fw_image_size;
    uint32_t max_upload_space;
    uint32_t remaining_upload_space;
    uint8_t oob_retrieval_supported;
    uint8_t *psupported_uri_scheme_names;
    uint16_t supported_uri_scheme_names_len;
} fw_dist_caps_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_CAPS_STATUS)];
    uint16_t max_dist_recvs_list_size;
    uint16_t max_fw_images_list_size;
    uint32_t max_fw_image_size;
    uint32_t max_upload_space;
    uint32_t remaining_upload_space;
    uint8_t oob_retrieval_supported;
    uint8_t supported_uri_scheme_names[0];
} _PACKED4_ fw_dist_caps_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_GET)];
} _PACKED4_ fw_dist_get_t;

typedef struct
{
    uint16_t dist_app_key_index;
    uint8_t dist_ttl;
    uint16_t dist_timeout_base;
    uint8_t dist_transfer_mode : 2;
    uint8_t update_policy : 1;
    uint8_t rfu : 5;
    uint16_t dist_fw_image_idx;
    uint8_t dist_multicast_addr[16];
} _PACKED4_ fw_dist_start_data_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_START)];
    uint16_t dist_app_key_index;
    uint8_t dist_ttl;
    uint16_t dist_timeout_base;
    uint8_t dist_transfer_mode : 2;
    uint8_t update_policy : 1;
    uint8_t rfu : 5;
    uint16_t dist_fw_image_idx;
    uint8_t dist_multicast_addr[16];
} _PACKED4_ fw_dist_start_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_SUSPEND)];
} _PACKED4_ fw_dist_suspend_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_CANCEL)];
} _PACKED4_ fw_dist_cancel_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_APPLY)];
} _PACKED4_ fw_dist_apply_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_STATUS)];
    fw_dist_status_code_t status;
    fw_dist_phase_t phase;
    uint16_t dist_multicast_addr;
    uint16_t dist_appkey_index;
    uint8_t dist_ttl;
    uint16_t dist_timeout_base;
    uint8_t dist_transfer_mode : 2;
    uint8_t update_policy : 1;
    uint8_t rfu : 5;
    uint16_t dist_fw_image_idx;
} _PACKED4_ fw_dist_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_UPLOAD_GET)];
} _PACKED4_ fw_dist_upload_get_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_UPLOAD_START)];
    uint8_t upload_ttl;
    uint16_t upload_timeout_base;
    uint8_t blob_id[8];
    uint32_t upload_fw_size;
    uint8_t upload_fw_metadata_len;
    uint8_t upload_fw_info[0];
} _PACKED4_ fw_dist_upload_start_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_UPLOAD_OOB_START)];
    uint8_t upload_uri_len;
    uint8_t upload_fw_info[0];
} _PACKED4_ fw_dist_upload_oob_start_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_UPLOAD_CANCEL)];
} _PACKED4_ fw_dist_upload_cancel_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_UPLOAD_STATUS)];
    uint8_t status;
    fw_upload_phase_t phase;
    uint8_t upload_progress : 7;
    uint8_t upload_type : 1;
    uint8_t upload_fw_id[0];
} _PACKED4_ fw_dist_upload_status_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_FW_GET)];
    uint8_t fw_id[0];
} _PACKED4_ fw_dist_fw_get_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_FW_GET_BY_INDEX)];
    uint16_t dist_fw_image_idx;
} _PACKED4_ fw_dist_fw_get_by_index_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_FW_DELETE)];
    uint8_t fw_id[0];
} _PACKED4_ fw_dist_fw_delete_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_FW_DELETE_ALL)];
} _PACKED4_ fw_dist_fw_delete_all_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_FW_DIST_FW_STATUS)];
    uint8_t status;
    uint16_t entry_cnt;
    uint16_t dist_fw_image_idx;
    uint8_t fw_id[0];
} _PACKED4_ fw_dist_fw_status_t;
/** @} */

typedef struct _fw_dist_update_node_t
{
    struct _fw_dist_update_node_t *pnext;
    fw_dist_receiver_t receiver;
} fw_dist_update_node_t, *fw_dist_update_node_p;

typedef struct _fw_image_e_t
{
    struct _fw_image_e_t *pnext;
    fw_image_t image;
    uint32_t dfu_image_size;
} fw_image_e_t, *fw_image_e_p;

typedef struct
{
    fw_dist_caps_t caps;
    plt_list_t dist_update_node_list;   //!< @ref fw_dist_update_node_t
    plt_list_t dist_fw_image_list;      //!< @ref fw_image_e_t
    fw_dist_phase_t dist_phase;

    /* distribution parameters */
    uint16_t dist_multicast_addr;
    uint16_t dist_app_key_index;
    uint8_t dist_ttl;
    uint16_t dist_timeout_base;
    uint8_t dist_transfer_mode;
    fw_update_policy_t dist_update_policy;
    uint16_t dist_fw_image_index;

    /* upload parameters */
    fw_upload_phase_t upload_phase;
    bool upload_oob;
    uint8_t upload_ttl;
    uint16_t upload_timeout_base;
    uint8_t upload_blob_id[8];
    uint32_t upload_fw_size;
    uint32_t upload_recvd_size;
    fw_image_t upload_fw_image;
    uint8_t upload_uri[255];
    uint8_t upload_uri_len;
} fw_dist_server_ctx_t;

extern fw_dist_server_ctx_t fw_dist_server_ctx;

/**
 * @defgroup FIRMWARE_DISTRIBUTION_SERVER_DATA Server Data
 * @brief Data types and structure used by data process callback
 * @{
 */

#define FW_DIST_SERVER_START                    0   //!< @ref fw_dist_server_start_t
#define FW_DIST_SERVER_SUSPEND                  1   //!< @ref NULL
#define FW_DIST_SERVER_CANCEL                   2   //!< @ref NULL
#define FW_DIST_SERVER_APPLY                    3   //!< @ref NULL
#define FW_DIST_SERVER_UPLOAD_START             4   //!< @ref fw_dist_server_upload_start_t
#define FW_DIST_SERVER_UPLOAD_OOB_START         5   //!< @ref fw_dist_server_upload_oob_start_t
#define FW_DIST_SERVER_UPLOAD_BLOCK_DATA        6   //!< @ref fw_dist_server_upload_block_data_t
#define FW_DIST_SERVER_UPLOAD_COMPLETE          7   //!< @ref NULL
#define FW_DIST_SERVER_UPLOAD_FAIL              8   //!< @ref fw_dist_server_upload_fail_t
#define FW_DIST_SERVER_FW_DELETE                9   //!< @ref fw_dist_server_fw_delete_t
#define FW_DIST_SERVER_FW_DELETE_ALL            10  //!< @ref NULL
#define FW_DIST_SERVER_URI_CHECK                11  //!< @ref fw_dist_server_uri_check_t
#define FW_DIST_SERVER_UPDATE_NODE_STATUS_GET   12  //!< @ref fw_dist_server_update_node_status_get_t

typedef struct
{
    uint16_t dist_app_key_index;
    uint8_t dist_ttl;
    uint16_t dist_timeout_base;
    uint8_t dist_transfer_mode;
    uint8_t dist_update_policy;
    uint16_t dist_fw_image_index;
    uint16_t dist_multicast_addr;
    uint16_t receiver_num;
    fw_dist_receiver_t *preceiver;
} fw_dist_server_start_t;

typedef struct
{
    uint8_t upload_ttl;
    uint16_t upload_timeout_base;
    uint8_t upload_blob_id[8];
    uint32_t upload_fw_size;
    uint8_t upload_fw_metadata_len;
    uint8_t *pupload_metadata;
    uint8_t upload_fw_id_len;
    uint8_t *pupload_fw_id;
} fw_dist_server_upload_start_t;

typedef struct
{
    uint8_t *pupload_uri;
    uint8_t upload_uri_len;
    uint8_t *pfw_id;
    uint8_t fw_id_len;
} fw_dist_server_upload_oob_start_t;

typedef struct
{
    uint16_t block_num;
    uint8_t *pdata;
    uint32_t data_len;
} fw_dist_server_upload_block_data_t;

typedef enum
{
    FW_UPLOAD_FAIL_REASON_USER_CANCEL,
    FW_UPLOAD_FAIL_REASON_TRANSFER_TIMEOUT,
    FW_UPLOAD_FAIL_REASON_TRANSFER_ERR,
} fw_upload_fail_reason_t;

typedef struct
{
    fw_image_t upload_fw_image;
    fw_upload_fail_reason_t reason;
} fw_dist_server_upload_fail_t;

typedef struct
{
    uint8_t *pfw_id;
    uint8_t fw_id_len;
} fw_dist_server_fw_delete_t;

typedef struct
{
    uint8_t *puri;
    uint8_t uri_len;
    /* app should modify this filed */
    fw_dist_status_code_t *pstatus;
} fw_dist_server_uri_check_t;

typedef struct
{
    fw_update_node_t *pnode;
} fw_dist_server_update_node_status_get_t;

/** @} */

/**
 * @defgroup FIRMWARE_DISTRIBUTION_CLIENT_DATA Client Data
 * @brief Data types and structure used by data process callback
 * @{
 */
#define FW_DIST_CLIENT_RECVS_STATUS                             0 //!< @ref fw_dist_client_recvs_status_t
#define FW_DIST_CLIENT_RECVS_LIST                               1 //!< @ref fw_dist_client_recvs_list_t
#define FW_DIST_CLIENT_CAPS_STATUS                              2 //!< @ref fw_dist_client_caps_status_t
#define FW_DIST_CLIENT_STATUS                                   3 //!< @ref fw_dist_client_status_t
#define FW_DIST_CLIENT_UPLOAD_STATUS                            4 //!< @ref fw_dist_client_upload_status_t
#define FW_DIST_CLIENT_FW_STATUS                                5 //!< @ref fw_dist_client_fw_status_t

typedef struct
{
    uint16_t src;
    fw_dist_status_code_t status;
    uint16_t recvs_list_cnt;
} fw_dist_client_recvs_status_t;

typedef struct
{
    uint16_t src;
    uint16_t recvs_list_cnt;
    uint16_t first_index;
    fw_update_node_t *pentries;
    uint8_t entries_cnt;
} fw_dist_client_recvs_list_t;

typedef struct
{
    uint16_t src;
    fw_dist_caps_t dist_caps;
} fw_dist_client_caps_status_t;

typedef struct
{
    uint16_t src;
    fw_dist_status_code_t status;
    fw_dist_phase_t phase;
    uint16_t dist_multicast_addr;
    uint16_t dist_appkey_index;
    uint8_t dist_ttl;
    uint16_t dist_timeout_base;
    uint8_t dist_transfer_mode : 2;
    uint8_t update_policy : 1;
    uint8_t rfu : 5;
    uint16_t dist_fw_image_idx;
} fw_dist_client_dist_status_t;

typedef struct
{
    uint16_t src;
    uint8_t status;
    fw_upload_phase_t phase;
    uint8_t upload_progress;
    bool upload_oob;
    uint8_t *pupload_fw_id;
    uint8_t upload_fw_id_len;
} fw_dist_client_upload_status_t;

typedef struct
{
    uint16_t src;
    uint8_t status;
    uint16_t entry_cnt;
    uint16_t dist_fw_image_idx;
    uint8_t *pfw_id;
    uint8_t fw_id_len;
} fw_dist_client_fw_status_t;

/** @} */

/**
 * @defgroup FIRMWARE_DISTRIBUTION_SERVER_API Server API
 * @brief Functions declaration
 * @{
 */

/**
 * @brief firmware distribution server register
 * @param[in] element_index: element index
 * @param[in] model_data_cb: model data callback function
 * @return register status
 */
bool fw_dist_server_reg(uint8_t element_index, model_data_cb_pf model_data_cb);

/**
 * @brief set firmware distribution server capabilities
 * @param[in] pcaps: server capabilities
 */
void fw_dist_server_caps_set(fw_dist_caps_t *pcaps);

/**
 * @brief get firmware distribution server capabilities
 * @return fw_dist_caps_t
 */
fw_dist_caps_t fw_dist_server_caps_get(void);

/**
 * @brief get image information
 * @param[in] index: image index
 * @return image pointer
 */
fw_image_e_p fw_dist_server_get_image_by_index(uint16_t index);

/**
 * @brief check whether receiver is empty
 * @return TRUE: empty
 *         FALSE: not empty
 */
bool fw_dist_server_is_receiver_empty(void);

/**
 * @brief delete all receivers
 */
void fw_dist_server_delete_all_receiver(void);

/**
 * @brief add distribution image
 * @return add status
 */
bool fw_dist_server_add_image(fw_image_t *pimage);

/**
 * @brief delete all distribution images
 */
void fw_dist_server_delete_all_image(void);

/**
 * @brief delete distribution image
 * @param[in] pfw_id: firmware id
 * @param[in] fw_id_len: firmware id length
 * @return delete image status
 */
bool fw_dist_server_delete_image(uint8_t *pfw_id, uint8_t fw_id_len);

/**
 * @brief handle blob transfer server data
 * @param[in] pmodel_info: pointer to blob transfer server model
 * @param[in] type: data callback message type
 * @param[in] pargs: data callback values
 * @return return value
 */
int32_t fw_dist_handle_blob_server_data(const mesh_model_info_p pmodel_info, uint32_t type,
                                        void *pargs);

/**
 * @brief start firmware distribution
 * @param[in] app_key_index: app key index
 * @param[in] update_ttl: update ttl
 * @param[in] update_timeout_base: update timeout base value
 * @param[in] blob_id: blob id value
 * @param[in] pfw_metadata: firmware metadata
 * @param[in] metadata_len: metadata length
 */
void fw_dist_server_start(uint16_t app_key_index, uint8_t update_ttl, uint16_t update_timeout_base,
                          uint8_t blob_id[8], uint8_t *pfw_metadata, uint8_t metadata_len);

/**
 * @brief notify firmware distribution failed
 */
void fw_dist_server_dist_failed(void);

/**
 * @brief set upload received firmware size
 * @param[in] upload_recvd_size: upload received firmware size
*/
void fw_dist_server_set_upload_recvd_size(uint32_t upload_recvd_size);

/**
 * @brief upload firmware use oob done
 * @param[in] success: download firmware status
 */
void fw_dist_server_upload_oob_done(bool success);

/** @} */

/**
 * @defgroup FIRMWARE_DISTRIBUTION_CLIENT_API Client API
 * @brief Functions declaration
 * @{
 */

/**
 * @brief firmware distribution client register
 * @param[in] element_index: element index
 * @param[in] model_data_cb: model data callback function
 * @return register status
 */
bool fw_dist_client_reg(uint8_t element_index, model_data_cb_pf model_data_cb);

/**
 * @brief add firmware distribution server receivers
 * @param[in] dst: remote address
 * @param[in] app_key_index: app key index
 * @param[in] precvs: receivers need to add
 * @param[in] recvs_len: receivers length
 * @return send status
 */
mesh_msg_send_cause_t fw_dist_recvs_add(uint16_t dst, uint16_t app_key_index,
                                        fw_dist_receiver_t *precvs, uint8_t recvs_len);

/**
 * @brief delete all firmware distribution server receivers
 * @param[in] dst: remote address
 * @param[in] app_key_index: app key index
 * @return send status
 */
mesh_msg_send_cause_t fw_dist_recvs_delete_all(uint16_t dst, uint16_t app_key_index);

/**
 * @brief get firmware distribution server receivers
 * @param[in] dst: remote address
 * @param[in] app_key_index: app key index
 * @param[in] first_index: first index
 * @param[in] entries_limit: entries limit
 * @return send status
 */
mesh_msg_send_cause_t fw_dist_recvs_get(uint16_t dst, uint16_t app_key_index, uint16_t first_index,
                                        uint16_t entries_limit);

/**
 * @brief get firmware distribution server capabilities
 * @param[in] dst: remote address
 * @param[in] app_key_index: app key index
 * @return send status
 */
mesh_msg_send_cause_t fw_dist_caps_get(uint16_t dst, uint16_t app_key_index);

/**
 * @brief get firmware distribution status
 * @param[in] dst: remote address
 * @param[in] app_key_index: app key index
 * @return send status
 */
mesh_msg_send_cause_t fw_dist_get(uint16_t dst, uint16_t app_key_index);

/**
 * @brief start firmware distribution
 * @param[in] dst: remote address
 * @param[in] app_key_index: app key index
 * @param[in] start: start parameters
 * @param[in] dist_dst_len: distribution destination length
 * @return send status
 */
mesh_msg_send_cause_t fw_dist_start(uint16_t dst, uint16_t app_key_index,
                                    fw_dist_start_data_t start, uint8_t dist_dst_len);

/**
 * @brief suspend firmware distribution
 * @param[in] dst: remote address
 * @param[in] app_key_index: app key index
 * @return send status
 */
mesh_msg_send_cause_t fw_dist_suspend(uint16_t dst, uint16_t app_key_index);

/**
 * @brief cancel firmware distribution
 * @param[in] dst: remote address
 * @param[in] app_key_index: app key index
 * @return send status
 */
mesh_msg_send_cause_t fw_dist_cancel(uint16_t dst, uint16_t app_key_index);

/**
 * @brief apply firmware distribution
 * @param[in] dst: remote address
 * @param[in] app_key_index: app key index
 * @return send status
 */
mesh_msg_send_cause_t fw_dist_apply(uint16_t dst, uint16_t app_key_index);

/**
 * @brief get firmware upload status
 * @param[in] dst: remote address
 * @param[in] app_key_index: app key index
 * @return send status
 */
mesh_msg_send_cause_t fw_dist_upload_get(uint16_t dst, uint16_t app_key_index);

/**
 * @brief start firmware upload
 * @param[in] dst: remote address
 * @param[in] app_key_index: app key index
 * @param[in] upload_ttl: upload ttl
 * @param[in] upload_timeout_base: upload timeout base
 * @param[in] blob_id: blob id
 * @param[in] upload_fw_size: upload firmware size
 * @param[in] pmetadata: firmware metadata
 * @param[in] metadata_len: metadata length
 * @param[in] pfw_id: firmware id
 * @param[in] fw_id_len: firmware id length
 * @return send status
 */
mesh_msg_send_cause_t fw_dist_upload_start(uint16_t dst, uint16_t app_key_index, uint8_t upload_ttl,
                                           uint16_t upload_timeout_base, uint8_t blob_id[8], uint32_t upload_fw_size, uint8_t *pmetadata,
                                           uint8_t metadata_len, uint8_t *pfw_id, uint8_t fw_id_len);
/**
 * @brief start firmware oob upload
 * @param[in] dst: remote address
 * @param[in] app_key_index: app key index
 * @param[in] pupload_uri: upload uri
 * @param[in] upload_uri_len: upload uri length
 * @param[in] pfw_id: firmware id
 * @param[in] fw_id_len: firmware id length
 * @return send status
 */
mesh_msg_send_cause_t fw_dist_upload_oob_start(uint16_t dst, uint16_t app_key_index,
                                               uint8_t *pupload_uri, uint8_t upload_uri_len, uint8_t *pfw_id, uint8_t fw_id_len);

/**
 * @brief cancel firmware upload
 * @param[in] dst: remote address
 * @param[in] app_key_index: app key index
 * @return send status
 */
mesh_msg_send_cause_t fw_dist_upload_cancel(uint16_t dst, uint16_t app_key_index);

/**
 * @brief get firmware
 * @param[in] dst: remote address
 * @param[in] app_key_index: app key index
 * @param[in] pfw_id: firmware id
 * @param[in] fw_id_len: firmware id length
 * @return send status
 */
mesh_msg_send_cause_t fw_dist_fw_get(uint16_t dst, uint16_t app_key_index, uint8_t *pfw_id,
                                     uint8_t fw_id_len);

/**
 * @brief get firmware by index
 * @param[in] dst: remote address
 * @param[in] app_key_index: app key index
 * @param[in] dist_fw_image_index: firmware index
 * @return send status
 */
mesh_msg_send_cause_t fw_dist_fw_get_by_index(uint16_t dst, uint16_t app_key_index,
                                              uint16_t dist_fw_image_index);

/**
 * @brief delete firmware image
 * @param[in] dst: remote address
 * @param[in] app_key_index: app key index
 * @param[in] pfw_id: firmware id
 * @param[in] fw_id_len: firmware id length
 * @return send status
 */
mesh_msg_send_cause_t fw_dist_fw_delete(uint16_t dst, uint16_t app_key_index, uint8_t *pfw_id,
                                        uint8_t fw_id_len);

/**
 * @brief delete all firmware images
 * @param[in] dst: remote address
 * @param[in] app_key_index: app key index
 * @return send status
 */
mesh_msg_send_cause_t fw_dist_fw_delete_all(uint16_t dst, uint16_t app_key_index);
/** @} */
/** @} */

END_DECLS

#endif /* MESH_DFU */

#endif /* _FIRMWARE_DISTRIBUTION_H */
