/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      blob_client_app.h
* @brief     Smart mesh blob client application
* @details
* @author    bill
* @date      2018-6-6
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _BLOB_CLIENT_APP_H
#define _BLOB_CLIENT_APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mesh_api.h"
#include "blob_transfer.h"
#include "app_mesh_flags.h"

#if F_BT_MESH_1_1_MBT_SUPPORT
/**
 * @addtogroup BLOB_CLIENT_APP
 * @{
 */

/**
 * @defgroup BLOB_CLient_Exported_Functions Bqb blob client app Exported Functions
 * @brief
 * @{
 */

#define BLOB_CLIENT_PROCEDURE_TIMEOUT  131
#define BLOB_CLIENT_RETRY_TIMEOUT      132
#define BLOB_CLIENT_CHUNK_TRANSFER     133

typedef enum
{
    BLOB_CLIENT_PHASE_IDLE,
    BLOB_CLIENT_PHASE_RETRIEVE,
    BLOB_CLIENT_PHASE_BLOB_TRANSFER_START,
    BLOB_CLIENT_PHASE_BLOB_BLOCK_START,
    BLOB_CLIENT_PHASE_BLOB_CHUNK_TRANSFER,
    BLOB_CLIENT_PHASE_BLOB_BLOCK_GET,
    BLOB_CLIENT_PHASE_BLOB_TRANSFER_CANCEL,
    // BLOB_CLIENT_PHASE_INITIALIZING,
    // BLOB_CLIENT_PHASE_ACTIVE,
    // BLOB_CLIENT_PHASE_SUSPENDED,
} blob_client_phase_t;

typedef enum
{
    BLOB_NODE_PHASE_IDLE,
    BLOB_NODE_PHASE_TRANSFER_GETTING,
    BLOB_NODE_PHASE_TRANSFER_GETTED,
    BLOB_NODE_PHASE_TRANSFER_STARTING,
    BLOB_NODE_PHASE_TRANSFER_STARTED,
    BLOB_NODE_PHASE_BLOCK_STARTING,
    BLOB_NODE_PHASE_BLOCK_STARTED,
    BLOB_NODE_PHASE_CHUNK_TRANSFERRING,
    BLOB_NODE_PHASE_CHUNK_TRANSFERRED,
    BLOB_NODE_PHASE_BLOCK_GETTING,
    BLOB_NODE_PHASE_BLOCK_GETTED,
    BLOB_NODE_PHASE_TRANSFER_CANCELLING,
    BLOB_NODE_PHASE_TRANSFER_CANCELLED,
    BLOB_NODE_PHASE_FAILED, // inactive
} blob_node_phase_t;

#define MESH_MSG_BLOB_CLIENT_APP_TRANSFER       0x00
#define MESH_MSG_BLOB_CLIENT_APP_PARAM          0x01
#define MESH_MSG_BLOB_CLIENT_APP_BLOCK_LOAD     0x02

enum
{
    BLOB_CB_PROCEDURE_CAPS_RETRIEVE,
    BLOB_CB_PROCEDURE_TRANSFER,
    BLOB_CB_PROCEDURE_CANCEL,
};
typedef uint8_t blob_cb_procedure_t;

enum
{
    BLOB_CB_TYPE_NODE_FAIL,
    BLOB_CB_TYPE_SUCCESS,
    BLOB_CB_TYPE_FAIL,
    BLOB_CB_TYPE_PROGRESS,
};
typedef uint8_t blob_cb_type_t;

typedef struct
{
    blob_cb_procedure_t procedure;
    blob_cb_type_t type;
    blob_client_phase_t client_phase;
    union
    {
        uint16_t addr;
        uint8_t progress; // 1 percent
    };
} blob_client_app_transfer_t;

typedef struct
{
    uint32_t blob_size;
    uint16_t block_size;
    uint16_t total_blocks;
    uint16_t chunk_size;
} blob_client_app_param_t;

typedef struct
{
    uint32_t offset;
    uint32_t block_size;
    uint8_t *pblock_data;
} blob_client_app_block_load_t;

typedef uint16_t (*pf_blob_client_cb_t)(uint8_t type, void *pdata);

/**
 * @brief capability retrieve procedure
 *
 * @param[in] app_key_index: app key index
 * @param[in] transfer_ttl: transfer ttl
 * @param[in] punicast_addr: list of unicast addresses
 * @param[in] unicast_addr_num: unicast address num
 * @return true
 * @return false
 */
bool blob_client_caps_retrieve(uint8_t app_key_index, uint8_t transfer_ttl, uint16_t *punicast_addr,
                               uint8_t unicast_addr_num);

/**
 * @brief blob transfer procedure
 *
 * @param[in] multicast_addr: target addr, may be unicast addr when only one blob transfer server
 * @param[in] app_key_index: app key index
 * @param[in] transfer_ttl: transfer ttl
 * @param[in] punicast_addr: list of unicast addresses
 * @param[in] unicast_addr_num: unicast address num
 * @param[in] blob_id: blob id
 * @param[in] blob_size: blob size
 * @param[in] transfer_mode: refs to blob_transfer_mode_t
 * @param[in] client_timeout_base: timeout, 10000 ms * (client_timeout_base + 2) + 100 ms * transfer_ttl
 * @param[in] skip_caps_retrieve: may skip retrieve procedure before blob transfer
 * @return true
 * @return false
 */
bool blob_client_blob_transfer(uint16_t multicast_addr, uint8_t app_key_index, uint8_t transfer_ttl,
                               uint16_t *punicast_addr, uint8_t unicast_addr_num, uint8_t blob_id[8],
                               uint32_t blob_size, blob_transfer_mode_t transfer_mode,
                               uint8_t client_timeout_base, bool skip_caps_retrieve);

/**
 * @brief transfer cancel procedure
 *
 * @param[in] punicast_addr: list of unicast addresses
 * @param[in] unicast_addr_num: unicast address num
 * @param[in] blob_id: blob id
 * @return true
 * @return false
 */
bool blob_client_transfer_cancel(uint16_t *punicast_addr, uint8_t unicast_addr_num,
                                 uint8_t blob_id[8]);

/**
 * @brief blob client mtu size set
 *
 * @param[in] client_mtu_size: mtu size, max = 380
 */
void blob_client_mtu_size_set(uint16_t client_mtu_size);

/**
 * @brief blob client block size log set
 *
 * @param[in] block_size_log: block size log
 */
void blob_client_block_size_log_set(uint8_t block_size_log);

/**
 * @brief blob client chunk size set
 *
 * @param[in] chunk_size: chunk size
 */
void blob_client_chunk_size_set(uint16_t chunk_size);

/**
 * @brief blob client app callback register
 *
 * @param[in] pf_blob_transfer_cb: callback
 */
void blob_client_app_cb_reg(pf_blob_client_cb_t pf_blob_transfer_cb);

/**
 * @brief blob client app initiate
 *
 * @param[in] element_index: element
 * @param[in] pf_blob_transfer_cb: callback
 */
void blob_client_app_init(uint8_t element_index, pf_blob_client_cb_t pf_blob_transfer_cb);

/**
 * @brief procedure timeout
 *
 */
void blob_client_handle_procedure_timeout(void);

/**
 * @brief retry timeout
 *
 */
void blob_client_handle_retry_timeout(void);

/**
 * @brief active chunk transfer
 *
 */
void blob_client_active_chunk_transfer(void);

/** @} */
/** @} */
#endif

#ifdef __cplusplus
}
#endif

#endif /* _BLOB_CLIENT_APP_H */
