/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      dfu_initiator_app.h
* @brief     Smart mesh dfu application
* @details
* @author    sterling_jiang
* @date      2022-01-04
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _DFU_INIT_MODELS_H
#define _DFU_INIT_MODELS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mesh_api.h"
#include "firmware_distribution.h"
#include "firmware_update.h"
#include "blob_client_app.h"

#if F_BT_MESH_1_1_DFU_SUPPORT

/**
 * @addtogroup DFU_INITIATOR_APP
 * @{
 */

/**
 * @defgroup Dfu_Initiator_Exported_Functions Dfu Distributor Exported Functions
 * @brief
 * @{
 */
#define DFU_INIT_APP_TIMEOUT_MSG                 122

enum
{
    DFU_INIT_PHASE_IDLE,
    DFU_INIT_PHASE_UPLOAD_START,
    DFU_INIT_PHASE_BLOB_TRANSFER,
    DFU_INIT_PHASE_FAILED,
};
typedef uint8_t dfu_init_phase_t;

enum
{
    DFU_INIT_CB_TYPE_UPLOAD_PROGRESS,
    DFU_INIT_CB_TYPE_UPLOAD_SUCCESS,
    DFU_INIT_CB_TYPE_UPLOAD_FAIL,
};
typedef uint8_t dfu_init_cb_type_t;

#define MESH_MSG_DFU_INIT_TRANSFER      0x00
#define MESH_MSG_DFU_INIT_BLOB_PARAM    0x01
#define MESH_MSG_DFU_INIT_BLOCK_LOAD    0x02
#define MESH_MSG_DFU_INIT_CLIENT_DATA   0x03

typedef struct
{
    dfu_init_cb_type_t type;
    dfu_init_phase_t init_phase;
    union
    {
        uint16_t addr;
        uint8_t progress; // 1 percent
    };
} dfu_init_transfer_t;

typedef struct
{
    uint32_t blob_size;
    uint16_t block_size;
    uint16_t total_blocks;
    uint16_t chunk_size;
} dfu_init_blob_param_t;

typedef struct
{
    uint32_t offset;
    uint32_t block_size;
    uint8_t *pblock_data;
} dfu_init_block_load_t;

typedef struct
{
    uint8_t type;
    void *pdata;
} dfu_init_client_data_t;

typedef uint16_t (*pf_dfu_init_cb_t)(uint8_t type, void *pdata);

/**
 * @brief skip caps retrieve
 *
 * @param[in] skip: true: skip, false: do caps retrieve before blob transfer
 */
void dfu_init_skip_caps_retrieve_set(bool skip);

/**
 * @brief dfu initiator models init
 *
 * @param[in] element_index: element index
 * @param[in] pf_dfu_init_cb: callback
 */
void dfu_init_models_init(uint8_t element_index, pf_dfu_init_cb_t pf_dfu_init_cb);

/**
 * @brief dfu initiator upload start
 *
 * @param[in] dst: distribution destination address, multicast address
 * @param[in] app_key_index: the index of the app key
 * @param[in] update_timeout_base: Updating Nodes calculates the timeout for receiving data
 * @param[in] pfw_metadata: the metadata of firmware
 * @param[in] metadata_len: metadata length
 * @param[in] pfw_id: the ID of firmware
 * @param[in] fw_id_len: ID length
 * @param[in] fw_image_size: the size of the Image , in Bytes
 * @return true
 * @return false
 */
bool dfu_init_upload_start(uint16_t dst, uint16_t app_key_index, uint16_t update_timeout_base,
                           uint8_t *pfw_metadata, uint8_t metadata_len, fw_id_t *pfw_id,
                           uint8_t fw_id_len, uint32_t fw_image_size);

/**
 * @brief dfu initiator clear
 *
 */
void dfu_init_clear(void);

/**
 * @brief dfu initiator timeout
 *
 */
void dfu_init_handle_timeout(void);
/** @} */
/** @} */
#endif

#ifdef __cplusplus
}
#endif

#endif /* _DFU_INIT_MODELS_H */
