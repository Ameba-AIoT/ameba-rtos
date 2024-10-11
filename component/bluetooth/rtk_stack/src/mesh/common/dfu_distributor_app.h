/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      dfu_distributor_app.h
* @brief     Smart mesh dfu application
* @details
* @author    bill
* @date      2018-6-6
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _DFU_DIST_MODELS_H
#define _DFU_DIST_MODELS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mesh_api.h"
#include "firmware_distribution.h"
#include "firmware_update.h"
#include "blob_client_app.h"

#if F_BT_MESH_1_1_DFU_SUPPORT
/**
 * @addtogroup DFU_DISTRIBUTOR_APP
 * @{
 */

/**
 * @defgroup Dfu_Distributor_Exported_Functions Dfu Distributor Exported Functions
 * @brief
 * @{
 */
#define DFU_DIST_APP_TIMEOUT_MSG                 121

enum
{
    DFU_DIST_PHASE_IDLE,
    DFU_DIST_PHASE_UPDATE_START,
    DFU_DIST_PHASE_BLOB_TRANSFER,
    DFU_DIST_PHASE_VERIFY,
    DFU_DIST_PHASE_APPLY,
    DFU_DIST_PHASE_CONFIRM,
    DFU_DIST_PHASE_CANCELING,
};
typedef uint8_t dfu_dist_phase_t;

enum
{
    DFU_DIST_CB_TYPE_NODE_FAIL,
    DFU_DIST_CB_TYPE_TRANSFER_PROGRESS,
    DFU_DIST_CB_TYPE_TRANSFER_SUCCESS,
    DFU_DIST_CB_TYPE_TRANSFER_FAIL,
    DFU_DIST_CB_TYPE_VERIFY,
    DFU_DIST_CB_TYPE_COMPLETE,
};
typedef uint8_t dfu_dist_cb_type_t;

#define MESH_MSG_DFU_DIST_TRANSFER          0x00
#define MESH_MSG_DFU_DIST_BLOB_PARAM        0x01
#define MESH_MSG_DFU_DIST_BLOCK_LOAD        0x02
#define MESH_MSG_DFU_DIST_UPLOAD            0x03

typedef struct
{
    dfu_dist_cb_type_t type;
    dfu_dist_phase_t dist_phase;
    union
    {
        struct
        {
            uint16_t *paddr;
            uint8_t addr_num;
        };
        uint8_t progress; // 1 percent
    };
} dfu_dist_transfer_t;

typedef struct
{
    uint32_t blob_size;
    uint16_t block_size;
    uint16_t total_blocks;
    uint16_t chunk_size;
} dfu_dist_blob_param_t;

typedef struct
{
    uint16_t image_index;
    uint32_t offset;
    uint32_t block_size;
    uint8_t *pblock_data;
} dfu_dist_block_load_t;

typedef struct
{
    uint8_t upload_type;
    void *pupload_data;
} dfu_dist_upload_t;

typedef uint16_t (*pf_dfu_dist_cb_t)(uint8_t type, void *pdata);

/**
 * @brief add receiver
 *
 * @param[in] addr: address
 * @param[in] update_image_index: update image index
 * @return true
 * @return false
 */
bool dfu_dist_add_receiver(uint16_t addr, uint8_t update_image_index);

/**
 * @brief delete receiver
 *
 * @param[in] addr: address
 */
void dfu_dist_delete_receiver_by_addr(uint16_t addr);

/**
 * @brief skip caps retrieve
 *
 * @param[in] skip: true: skip, false: do caps retrieve before blob transfer
 */
void dfu_dist_skip_caps_retrieve_set(bool skip);

/**
 * @brief dfu distributor models init
 *
 * @param[in] element_index: element index
 * @param[in] pf_dfu_dist_cb: callback
 */
void dfu_dist_models_init(uint8_t element_index, pf_dfu_dist_cb_t pf_dfu_dist_cb);

/**
 * @brief dfu standalone updater models init
 *
 * @param[in] element_index: element index
 * @param[in] pf_dfu_dist_cb: callback
 */
void dfu_standalone_updater_models_init(uint8_t element_index, pf_dfu_dist_cb_t pf_dfu_dist_cb);

/**
 * @brief dfu distributor start transfer
 *
 * @param[in] dst: distribution destination address, multicast address
 * @param[in] app_key_index: the index of app key
 * @param[in] update_timeout_base: Updating Nodes calculates the timeout for receiving data
 * @param[in] pfw_metadata: firmware metadata
 * @param[in] metadata_len: metadata length
 * @param[in] fw_image_size: the size of the Image, in Bytes
 * @return true
 * @return false
 */
bool dfu_dist_start(uint16_t dst, uint16_t app_key_index, uint16_t update_timeout_base,
                    uint8_t *pfw_metadata, uint8_t metadata_len, uint32_t fw_image_size);

/**
 * @brief dfu distributor clear
 *
 */
void dfu_dist_clear(void);

/**
 * @brief dfu distributor timeout
 *
 */
void dfu_dist_handle_timeout(void);
/** @} */
/** @} */
#endif

#ifdef __cplusplus
}
#endif

#endif /* _DFU_DIST_MODELS_H */
