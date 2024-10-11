/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      dfu_initiator_app.c
* @brief     Smart mesh dfu application
* @details
* @author    sterling_jiang
* @date      2022-01-04
* @version   v1.0
* *********************************************************************************************************
*/

#define MM_ID MM_MODEL

#include "platform_misc.h"
#include "blob_client_app.h"
#include "dfu_initiator_app.h"
#include "generic_types.h"
#include "app_msg.h"

#if F_BT_MESH_1_1_DFU_SUPPORT

#define DFU_INIT_RETRY_TIMES                            3
#define DFU_INIT_UPLOAD_RETRY_PERIOD                    2000

/* transfer capabilities */
#define DFU_INIT_BLOCK_SIZE_LOG                         12
#define DFU_INIT_CHUNK_SIZE                             256
#define DFU_INIT_CLIENT_MTU                             376

struct
{
    plt_timer_t timer;
    dfu_init_phase_t init_phase;
    uint8_t init_retry_count;
    uint16_t init_dist_addr;
    uint8_t blob_id[8];
    uint16_t init_app_key_index;
    uint8_t init_ttl;
    uint16_t init_timeout_base;
    fw_image_t upload_fw_image;
    uint32_t fw_image_size;
    pf_dfu_init_cb_t dfu_init_cb;
} dfu_init_ctx;

uint16_t dfu_init_blob_client_cb(uint8_t type, void *pdata);

static bool init_skip_caps_retrieve = false;

void dfu_init_skip_caps_retrieve_set(bool skip)
{
    init_skip_caps_retrieve = skip;
}

static bool dfu_init_blob_transfer(void)
{
    printi("dfu_init_blob_transfer: start firmware upload blob transfer start");
    dfu_init_ctx.init_phase = DFU_INIT_PHASE_BLOB_TRANSFER;

    /* change callback since Role change to Initiator */
    blob_client_app_cb_reg(dfu_init_blob_client_cb);

    if (init_skip_caps_retrieve)
    {
        blob_client_mtu_size_set(DFU_INIT_CLIENT_MTU);
        blob_client_block_size_log_set(DFU_INIT_BLOCK_SIZE_LOG);
    }

    blob_client_chunk_size_set(DFU_INIT_CHUNK_SIZE);

    if (init_skip_caps_retrieve)
    {
        return blob_client_blob_transfer(dfu_init_ctx.init_dist_addr,
                                         dfu_init_ctx.init_app_key_index,
                                         dfu_init_ctx.init_ttl, &dfu_init_ctx.init_dist_addr, 1,
                                         dfu_init_ctx.blob_id, dfu_init_ctx.fw_image_size,
                                         BLOB_TRANSFER_MODE_PUSH, dfu_init_ctx.init_timeout_base,
                                         init_skip_caps_retrieve);
    }
    else
    {
        return blob_client_caps_retrieve(dfu_init_ctx.init_app_key_index,
                                         dfu_init_ctx.init_ttl, &dfu_init_ctx.init_dist_addr, 1);
    }
}

int32_t dfu_init_dist_client_data(const mesh_model_info_p pmodel_info, uint32_t type, void *pargs)
{
    switch (type)
    {
    case FW_DIST_CLIENT_RECVS_STATUS:
    case FW_DIST_CLIENT_RECVS_LIST:
    case FW_DIST_CLIENT_CAPS_STATUS:
    case FW_DIST_CLIENT_STATUS:
    case FW_DIST_CLIENT_UPLOAD_STATUS:
    case FW_DIST_CLIENT_FW_STATUS:
        {
            if (dfu_init_ctx.dfu_init_cb)
            {
                dfu_init_client_data_t cb_data = {0};
                cb_data.type = type;
                cb_data.pdata = pargs;
                dfu_init_ctx.dfu_init_cb(MESH_MSG_DFU_INIT_CLIENT_DATA, &cb_data);
            }
        }
        break;
    default:
        break;
    }

    if (type == FW_DIST_CLIENT_UPLOAD_STATUS)
    {
        fw_dist_client_upload_status_t *pdata = (fw_dist_client_upload_status_t *)pargs;
        switch (dfu_init_ctx.init_phase)
        {
        case DFU_INIT_PHASE_UPLOAD_START:
            {
                plt_timer_stop(dfu_init_ctx.timer, 0);
                if (pdata->status == FW_DIST_STATUS_SUCCESS)
                {
                    printi("dfu upload status: node 0x%04x upload start success", pdata->src);
                    if (pdata->phase == FW_UPLOAD_PHASE_TRANSFER_SUCCESS)
                    {
                        dfu_init_transfer_t cb_data = {0};
                        cb_data.init_phase = dfu_init_ctx.init_phase;
                        cb_data.type = DFU_INIT_CB_TYPE_UPLOAD_SUCCESS;
                        if (dfu_init_ctx.dfu_init_cb)
                        {
                            cb_data.addr = dfu_init_ctx.init_dist_addr;
                            dfu_init_ctx.dfu_init_cb(MESH_MSG_DFU_INIT_TRANSFER, &cb_data);
                        }
                    }
                    else if (pdata->phase == FW_UPLOAD_PHASE_TRANSFER_ACTIVE)
                    {
                        dfu_init_blob_transfer();
                    }
                }
                else
                {
                    printe("dfu upload status: node 0x%04x upload start failed, reason %d", pdata->src,
                           pdata->status);
                }
            }
            break;
        default:
            break;
        }
    }
    return MODEL_SUCCESS;
}

extern void *evt_queue_handle;  //!< Event queue handle
extern void *io_queue_handle;   //!< IO queue handle

static void dfu_init_timeout(void *ptimer)
{
    uint8_t event = EVENT_IO_TO_APP;
    T_IO_MSG msg;
    msg.type = DFU_INIT_APP_TIMEOUT_MSG;
    msg.u.buf = ptimer;
    if (os_msg_send(io_queue_handle, &msg, 0) == false)
    {
    }
    else if (os_msg_send(evt_queue_handle, &event, 0) == false)
    {
    }
}

void dfu_init_handle_timeout(void)
{
    switch (dfu_init_ctx.init_phase)
    {
    case DFU_INIT_PHASE_UPLOAD_START:
        {
            if (dfu_init_ctx.init_retry_count >= DFU_INIT_RETRY_TIMES)
            {
                printe("dfu_init_handle_timeout: node 0x%04x update start failed",
                       dfu_init_ctx.init_dist_addr);
                dfu_init_ctx.init_phase = DFU_INIT_PHASE_FAILED;
                dfu_init_clear();
            }
            else
            {
                fw_dist_upload_start(dfu_init_ctx.init_dist_addr, dfu_init_ctx.init_app_key_index,
                                     dfu_init_ctx.init_ttl, dfu_init_ctx.init_timeout_base, dfu_init_ctx.blob_id,
                                     dfu_init_ctx.fw_image_size, dfu_init_ctx.upload_fw_image.metadata,
                                     dfu_init_ctx.upload_fw_image.metadata_len, (uint8_t *)&dfu_init_ctx.upload_fw_image.fw_id,
                                     dfu_init_ctx.upload_fw_image.fw_id_len);
                dfu_init_ctx.init_retry_count ++;
                plt_timer_change_period(dfu_init_ctx.timer, DFU_INIT_UPLOAD_RETRY_PERIOD, 0);
                printi("dfu_init_handle_timeout: fw_update_start, dst 0x%04x, retry count %d",
                       dfu_init_ctx.init_dist_addr, dfu_init_ctx.init_retry_count);
            }
        }
        break;
    default:
        break;
    }
}

void dfu_init_clear(void)
{
    printi("dfu_init_clear");
    if (dfu_init_ctx.timer)
    {
        plt_timer_delete(dfu_init_ctx.timer, 0);
    }
    memset(&dfu_init_ctx, 0, sizeof(dfu_init_ctx));
}

bool dfu_init_upload_start(uint16_t dst, uint16_t app_key_index, uint16_t update_timeout_base,
                           uint8_t *pfw_metadata, uint8_t metadata_len, fw_id_t *pfw_id,
                           uint8_t fw_id_len, uint32_t fw_image_size)
{
    if (DFU_INIT_PHASE_IDLE != dfu_init_ctx.init_phase)
    {
        printe("dfu_init_upload_start: busy, phase %d", dfu_init_ctx.init_phase);
        return false;
    }

    /* start timeout timer */
    if (!dfu_init_ctx.timer)
    {
        dfu_init_ctx.timer = plt_timer_create("dfu_init", DFU_INIT_UPLOAD_RETRY_PERIOD, false, 0,
                                              dfu_init_timeout);
        if (!dfu_init_ctx.timer)
        {
            printe("dfu_init_upload_start: create timer failed");
            return false;
        }
    }

    dfu_init_ctx.init_phase = DFU_INIT_PHASE_UPLOAD_START;
    dfu_init_ctx.init_dist_addr = dst;
    dfu_init_ctx.init_app_key_index = app_key_index;
    dfu_init_ctx.init_timeout_base = update_timeout_base;
    dfu_init_ctx.init_ttl = mesh_node.ttl;
    dfu_init_ctx.fw_image_size = fw_image_size;

    dfu_init_ctx.upload_fw_image.fw_id_len = fw_id_len;
    memcpy((uint8_t *)&dfu_init_ctx.upload_fw_image.fw_id, (uint8_t *)pfw_id, fw_id_len);
    dfu_init_ctx.upload_fw_image.metadata_len = metadata_len;
    memcpy(dfu_init_ctx.upload_fw_image.metadata, pfw_metadata, metadata_len);
    plt_rand(dfu_init_ctx.blob_id, 8);

    /* TODO: multi Distributor */
    dfu_init_ctx.init_retry_count = 0;
    fw_dist_upload_start(dst, app_key_index, mesh_node.ttl, update_timeout_base, dfu_init_ctx.blob_id,
                         fw_image_size, pfw_metadata, metadata_len,
                         (uint8_t *)pfw_id, fw_id_len);
    plt_timer_start(dfu_init_ctx.timer, 0);

    printi("dfu_init_upload_start: dst 0x%04x, app_key_index %d, update_timeout_base %d, fw_image_size %d",
           dfu_init_ctx.init_dist_addr, dfu_init_ctx.init_app_key_index,
           dfu_init_ctx.init_timeout_base, dfu_init_ctx.fw_image_size);
    printi("dfu_init_upload_start: firmware metadata =");
    dprinti(dfu_init_ctx.upload_fw_image.metadata, dfu_init_ctx.upload_fw_image.metadata_len);
    printi("dfu_init_upload_start: firmware ID =");
    dprinti((uint8_t *)&dfu_init_ctx.upload_fw_image.fw_id, dfu_init_ctx.upload_fw_image.fw_id_len);
    printi("dfu_init_upload_start: blob id =");
    dprinti(dfu_init_ctx.blob_id, 8);

    return true;
}

uint16_t dfu_init_blob_client_cb(uint8_t type, void *pdata)
{
    switch (type)
    {
    case MESH_MSG_BLOB_CLIENT_APP_TRANSFER:
        {
            blob_client_app_transfer_t *ptransfer = (blob_client_app_transfer_t *)pdata;
            if (ptransfer->procedure == BLOB_CB_PROCEDURE_TRANSFER)
            {
                dfu_init_transfer_t cb_data = {0};
                cb_data.init_phase = dfu_init_ctx.init_phase;
                if (ptransfer->type == BLOB_CB_TYPE_SUCCESS)
                {
                    printi("dfu_init_blob_client_cb: firmware upload complete");
                    cb_data.type = DFU_INIT_CB_TYPE_UPLOAD_SUCCESS;
                    if (dfu_init_ctx.dfu_init_cb)
                    {
                        cb_data.addr = dfu_init_ctx.init_dist_addr;
                        dfu_init_ctx.dfu_init_cb(MESH_MSG_DFU_INIT_TRANSFER, &cb_data);
                    }
                }
                else if (ptransfer->type == BLOB_CB_TYPE_PROGRESS)
                {
                    printi("dfu_init_blob_client_cb: progress %d%%", ptransfer->progress);
                    cb_data.type = DFU_INIT_CB_TYPE_UPLOAD_PROGRESS;
                    if (dfu_init_ctx.dfu_init_cb)
                    {
                        cb_data.progress = ptransfer->progress;
                        dfu_init_ctx.dfu_init_cb(MESH_MSG_DFU_INIT_TRANSFER, &cb_data);
                    }
                }
                else if (ptransfer->type == BLOB_CB_TYPE_FAIL)
                {
                    printe("dfu_init_blob_client_cb: firmware upload failed");
                    cb_data.type = DFU_INIT_CB_TYPE_UPLOAD_FAIL;
                    if (dfu_init_ctx.dfu_init_cb)
                    {
                        cb_data.addr = dfu_init_ctx.init_dist_addr;
                        dfu_init_ctx.dfu_init_cb(MESH_MSG_DFU_INIT_TRANSFER, &cb_data);
                    }
                    dfu_init_clear();
                }
            }
            else if (ptransfer->procedure == BLOB_CB_PROCEDURE_CAPS_RETRIEVE)
            {
                if (ptransfer->type == BLOB_CB_TYPE_SUCCESS &&
                    !init_skip_caps_retrieve)
                {
                    blob_client_blob_transfer(dfu_init_ctx.init_dist_addr,
                                              dfu_init_ctx.init_app_key_index,
                                              dfu_init_ctx.init_ttl, &dfu_init_ctx.init_dist_addr, 1,
                                              dfu_init_ctx.blob_id, dfu_init_ctx.fw_image_size,
                                              BLOB_TRANSFER_MODE_PUSH, dfu_init_ctx.init_timeout_base,
                                              init_skip_caps_retrieve);
                }
            }
        }
        break;
    case MESH_MSG_BLOB_CLIENT_APP_PARAM:
        {
            blob_client_app_param_t *pparam = (blob_client_app_param_t *)pdata;
            if (dfu_init_ctx.dfu_init_cb)
            {
                dfu_init_blob_param_t cb_data = {0};
                cb_data.blob_size = pparam->blob_size;
                cb_data.block_size = pparam->block_size;
                cb_data.total_blocks = pparam->total_blocks;
                cb_data.chunk_size = pparam->chunk_size;
                dfu_init_ctx.dfu_init_cb(MESH_MSG_DFU_INIT_BLOB_PARAM, &cb_data);
            }
        }
        break;
    case MESH_MSG_BLOB_CLIENT_APP_BLOCK_LOAD:
        {
            blob_client_app_block_load_t *pblock_get = (blob_client_app_block_load_t *)pdata;
            if (dfu_init_ctx.dfu_init_cb)
            {
                dfu_init_block_load_t cb_data = {0};
                cb_data.offset = pblock_get->offset;
                cb_data.block_size = pblock_get->block_size;
                cb_data.pblock_data = pblock_get->pblock_data;
                dfu_init_ctx.dfu_init_cb(MESH_MSG_DFU_INIT_BLOCK_LOAD, &cb_data);
            }
        }
        break;
    default:
        break;
    }

    return 0x00;
}

void dfu_init_models_init(uint8_t element_index, pf_dfu_init_cb_t pf_dfu_init_cb)
{
    dfu_init_ctx.dfu_init_cb = pf_dfu_init_cb;
    fw_dist_client_reg(element_index, dfu_init_dist_client_data);
    fw_update_client_reg(element_index, NULL);
    blob_client_app_init(element_index, dfu_init_blob_client_cb);
}

#endif
