/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      dfu_distributor_app.c
* @brief     Smart mesh dfu application
* @details
* @author    hector_huang
* @date      2020-11-06
* @version   v1.0
* *********************************************************************************************************
*/

#define MM_ID MM_APP

#include <stdlib.h>
#include "platform_misc.h"
#include "blob_client_app.h"
#include "dfu_distributor_app.h"
#include "generic_types.h"
#include "app_msg.h"
// RTK porting:for RTK_BT_MESH_IO_MSG_SUBTYPE_xxx define
#include <rtk_bt_mesh_def.h>

#if F_BT_MESH_1_1_DFU_SUPPORT

#define DFU_DIST_RETRY_TIMES                            3
#define DFU_DIST_UPDATE_START_RETRY_PERIOD              2000
#define DFU_DIST_UPDATE_GET_RETRY_PERIOD                2000
#define DFU_DIST_UPDATE_APPLY_RETRY_PERIOD              2000
#define DFU_DIST_UPDATE_INFO_GET_RETRY_PERIOD           2000

/* transfer capabilities */
#define DFU_DIST_BLOCK_SIZE_LOG                         12
#define DFU_DIST_CHUNK_SIZE                             256
#define DFU_DIST_CLIENT_MTU                             376

typedef enum
{
    DIST_NODE_PHASE_IDLE,
    DIST_NODE_PHASE_UPDATE_STARTING,
    DIST_NODE_PHASE_UPDATE_STARTED,
    DIST_NODE_PHASE_APPLYING,
    DIST_NODE_PHASE_APPLIED,
    DIST_NODE_PHASE_CONFIRMED,
    DIST_NODE_PHASE_CANCELING,
    DIST_NODE_PHASE_FAILED,
} dfu_dist_node_phase_t;

typedef struct _dfu_dist_update_node_t
{
    struct _dfu_dist_update_node_t *pnext;
    uint32_t addr : 15;
    uint32_t retrieved_update_phase : 4;
    uint32_t update_status : 3;
    uint32_t transfer_status : 4;
    uint32_t transfer_progress : 6;
    uint8_t update_fw_image_idx;
    dfu_dist_node_phase_t node_phase;
} dfu_dist_update_node_t, *dfu_dist_update_node_p;

typedef enum
{
    DIST_ROLE_UNUSE,
    DIST_ROLE_DISTRIBUTOR,
    DIST_ROLE_STANDALONE_UPDATER,
} dfu_dist_role_t;

struct
{
    dfu_dist_role_t dist_role;
    plt_timer_t timer;
    dfu_dist_phase_t dist_phase;
    uint8_t dist_retry_count;
    uint16_t *ptransfer_addr;
    uint8_t transfer_addr_num;
    dfu_dist_update_node_p pcur_update_node;
    plt_list_t dist_update_node_list;   //!<ref dfu_dist_update_node_t
    uint8_t blob_id[8];
    uint16_t dist_multicast_addr;
    uint16_t dist_app_key_index;
    uint8_t dist_ttl;
    uint16_t dist_timeout_base;
    uint8_t fw_metadata[255];
    uint8_t metadata_len;
    uint32_t fw_image_size;
    pf_dfu_dist_cb_t dfu_dist_cb;
    /* distributor */
    uint8_t dist_update_policy;
    uint16_t cur_image_index;
} dfu_dist_ctx;

uint16_t dfu_dist_blob_client_cb(uint8_t type, void *pdata);
static void dfu_dist_fw_apply(void);

bool dfu_dist_add_receiver(uint16_t addr, uint8_t update_fw_image_idx)
{
    dfu_dist_update_node_p pentry = (dfu_dist_update_node_p)
                                    dfu_dist_ctx.dist_update_node_list.pfirst;
    while (pentry)
    {
        if (pentry->addr == addr)
        {
            printw("dfu_dist_add_receiver: address 0x%04x already exists, image index update %d -> %d",
                   addr, pentry->update_fw_image_idx, update_fw_image_idx);
            pentry->update_fw_image_idx = update_fw_image_idx;
            return true;
        }
        pentry = pentry->pnext;
    }

    pentry = plt_zalloc(sizeof(dfu_dist_update_node_t), RAM_TYPE_DATA_ON);
    if (!pentry)
    {
        printe("dfu_dist_add_receiver: add receiver failed, out of memory");
        return false;
    }

    pentry->addr = addr;
    pentry->update_fw_image_idx = update_fw_image_idx;
    plt_list_push(&dfu_dist_ctx.dist_update_node_list, pentry);
    printi("dfu_dist_add_receiver: index %d, addr 0x%04x, update image index %d, count %d",
           dfu_dist_ctx.dist_update_node_list.count - 1, addr,
           update_fw_image_idx, dfu_dist_ctx.dist_update_node_list.count);

    return true;
}

static dfu_dist_update_node_p dfu_dist_receiver_get(uint16_t addr, uint8_t update_fw_image_idx)
{
    dfu_dist_update_node_p pentry = (dfu_dist_update_node_p)
                                    dfu_dist_ctx.dist_update_node_list.pfirst;
    while (pentry)
    {
        if (pentry->addr == addr &&
            pentry->update_fw_image_idx == update_fw_image_idx)
        {
            return pentry;
        }
        pentry = pentry->pnext;
    }
    return NULL;
}

static dfu_dist_update_node_p dfu_dist_receiver_get_by_addr(uint16_t addr)
{
    dfu_dist_update_node_p pentry = (dfu_dist_update_node_p)
                                    dfu_dist_ctx.dist_update_node_list.pfirst;
    while (pentry)
    {
        if (pentry->addr == addr)
        {
            return pentry;
        }
        pentry = pentry->pnext;
    }
    return NULL;
}

void dfu_dist_delete_receiver_by_addr(uint16_t addr)
{
    dfu_dist_update_node_p pentry = (dfu_dist_update_node_p)
                                    dfu_dist_ctx.dist_update_node_list.pfirst;
    while (pentry)
    {
        if (pentry->addr == addr)
        {
            plt_list_remove(&dfu_dist_ctx.dist_update_node_list, pentry);
            plt_free(pentry, RAM_TYPE_DATA_ON);
            printi("dfu_dist_delete_receiver_by_addr: delete receiver, addr 0x%04x", addr);
            return;
        }
        pentry = pentry->pnext;
    }
}

void dfu_dist_delete_all_receiver(void)
{
    dfu_dist_update_node_p pentry = (dfu_dist_update_node_p)
                                    dfu_dist_ctx.dist_update_node_list.pfirst;
    while (pentry)
    {
        dfu_dist_delete_receiver_by_addr(pentry->addr);
        pentry = pentry->pnext;
    }
    printi("dfu_dist_delete_all_receiver: delete all receivers");
}

void dfu_dist_handle_node_fail(dfu_dist_update_node_p pnode)
{
    if (dfu_dist_ctx.dfu_dist_cb)
    {
        dfu_dist_transfer_t cb_data = {0};
        cb_data.type = DFU_DIST_CB_TYPE_NODE_FAIL;
        cb_data.dist_phase = dfu_dist_ctx.dist_phase;
        uint16_t addr = pnode->addr;
        cb_data.paddr = &addr;
        cb_data.addr_num = 1;
        dfu_dist_ctx.dfu_dist_cb(MESH_MSG_DFU_DIST_TRANSFER, &cb_data);
    }
    pnode->node_phase = DIST_NODE_PHASE_FAILED;
}

void dfu_dist_handle_blob_transfer(bool ret)
{
    printi("dfu_dist_handle_blob_transfer: ret %d", ret);
    if (dfu_dist_ctx.dfu_dist_cb)
    {
        dfu_dist_transfer_t cb_data = {0};
        cb_data.type = ret ? DFU_DIST_CB_TYPE_TRANSFER_SUCCESS : DFU_DIST_CB_TYPE_TRANSFER_FAIL;
        cb_data.dist_phase = dfu_dist_ctx.dist_phase;
        uint8_t count = dfu_dist_ctx.dist_update_node_list.count;
        uint16_t *paddr = plt_zalloc(count * sizeof(*paddr), RAM_TYPE_DATA_ON);
        uint8_t i = 0;
        if (ret && paddr)
        {
            dfu_dist_update_node_p pnode = (dfu_dist_update_node_p)
                                           dfu_dist_ctx.dist_update_node_list.pfirst;
            while (pnode)
            {
                if (pnode->node_phase == DIST_NODE_PHASE_UPDATE_STARTED)
                {
                    paddr[i++] = pnode->addr;
                }
                pnode = pnode->pnext;
            }
            cb_data.paddr = paddr;
            cb_data.addr_num = i;
        }
        dfu_dist_ctx.dfu_dist_cb(MESH_MSG_DFU_DIST_TRANSFER, &cb_data);
        if (paddr)
        {
            plt_free(paddr, RAM_TYPE_DATA_ON);
            paddr = NULL;
        }
    }

    if (dfu_dist_ctx.dist_role == DIST_ROLE_DISTRIBUTOR)
    {
        fw_dist_server_ctx.dist_phase = ret ? FW_DIST_PHASE_TRANSFER_SUCCESS : FW_DIST_PHASE_FAILED;
    }
}

void dfu_dist_handle_fw_verify(void)
{
    if (dfu_dist_ctx.dfu_dist_cb)
    {
        dfu_dist_transfer_t cb_data = {0};
        cb_data.type = DFU_DIST_CB_TYPE_VERIFY;
        cb_data.dist_phase = dfu_dist_ctx.dist_phase;
        uint8_t count = dfu_dist_ctx.dist_update_node_list.count;
        uint16_t *paddr = plt_zalloc(count * sizeof(*paddr), RAM_TYPE_DATA_ON);
        uint8_t i = 0;
        if (paddr)
        {
            dfu_dist_update_node_p pnode = (dfu_dist_update_node_p)
                                           dfu_dist_ctx.dist_update_node_list.pfirst;
            while (pnode)
            {
                if (pnode->retrieved_update_phase == FW_RETRIEVED_UPDATE_PHASE_VERIFICATION_SUCCEEDED)
                {
                    paddr[i++] = pnode->addr;
                }
                pnode = pnode->pnext;
            }
            cb_data.paddr = paddr;
            cb_data.addr_num = i;
        }
        dfu_dist_ctx.dfu_dist_cb(MESH_MSG_DFU_DIST_TRANSFER, &cb_data);
        if (paddr)
        {
            plt_free(paddr, RAM_TYPE_DATA_ON);
            paddr = NULL;
        }
    }

    /* all node verify finished */
    if (dfu_dist_ctx.dist_role == DIST_ROLE_STANDALONE_UPDATER)
    {
        dfu_dist_fw_apply();
    }
    else if (dfu_dist_ctx.dist_role == DIST_ROLE_DISTRIBUTOR &&
             dfu_dist_ctx.dist_update_policy == FW_UPDATE_POLICY_VERIFY_AND_UPDATE)
    {
        dfu_dist_fw_apply();
    }
}

void dfu_dist_handle_dist_result(void)
{
    if (dfu_dist_ctx.dfu_dist_cb)
    {
        dfu_dist_transfer_t cb_data = {0};
        cb_data.type = DFU_DIST_CB_TYPE_COMPLETE;
        cb_data.dist_phase = dfu_dist_ctx.dist_phase;
        uint8_t count = dfu_dist_ctx.dist_update_node_list.count;
        uint16_t *paddr = plt_zalloc(count * sizeof(*paddr), RAM_TYPE_DATA_ON);
        uint8_t i = 0;
        if (paddr)
        {
            dfu_dist_update_node_p pnode = (dfu_dist_update_node_p)
                                           dfu_dist_ctx.dist_update_node_list.pfirst;
            while (pnode)
            {
                if (pnode->node_phase == DIST_NODE_PHASE_CONFIRMED)
                {
                    paddr[i++] = pnode->addr;
                }
                pnode = pnode->pnext;
            }
            cb_data.paddr = paddr;
            cb_data.addr_num = i;
        }
        dfu_dist_ctx.dfu_dist_cb(MESH_MSG_DFU_DIST_TRANSFER, &cb_data);
        if (paddr)
        {
            plt_free(paddr, RAM_TYPE_DATA_ON);
            paddr = NULL;
        }
    }

    printi("dfu_dist_handle_dist_result: node update finished");

    dfu_dist_clear();

    if (dfu_dist_ctx.dist_role == DIST_ROLE_DISTRIBUTOR)
    {
        fw_dist_server_ctx.dist_phase = FW_DIST_PHASE_COMPLETED;
    }
}

void dfu_dist_clear(void)
{
    printi("dfu_dist_clear");
    if (dfu_dist_ctx.timer != NULL)
    {
        plt_timer_delete(dfu_dist_ctx.timer, 0);
        dfu_dist_ctx.timer = NULL;
    }
    if (dfu_dist_ctx.ptransfer_addr)
    {
        plt_free(dfu_dist_ctx.ptransfer_addr, RAM_TYPE_DATA_ON);
        dfu_dist_ctx.ptransfer_addr = NULL;
    }
    dfu_dist_delete_all_receiver();
    dfu_dist_ctx.dist_phase = DFU_DIST_PHASE_IDLE;
}

static bool dfu_dist_active_update_start_send(void)
{
    dfu_dist_update_node_p pnode = (dfu_dist_update_node_p)
                                   dfu_dist_ctx.dist_update_node_list.pfirst;
    while (pnode)
    {
        if (pnode->node_phase == DIST_NODE_PHASE_IDLE)
        {
            dfu_dist_ctx.dist_retry_count = 0;
            plt_timer_change_period(dfu_dist_ctx.timer, DFU_DIST_UPDATE_START_RETRY_PERIOD, 0);
            pnode->node_phase = DIST_NODE_PHASE_UPDATE_STARTING;
            dfu_dist_ctx.pcur_update_node = pnode;
            printi("dfu dist update start: addr 0x%04x, app key index %d, ttl %d, timeout base %d, image idx %d",
                   pnode->addr, dfu_dist_ctx.dist_app_key_index, dfu_dist_ctx.dist_ttl,
                   dfu_dist_ctx.dist_timeout_base, pnode->update_fw_image_idx);
            fw_update_start(pnode->addr, dfu_dist_ctx.dist_app_key_index,
                            dfu_dist_ctx.dist_ttl, dfu_dist_ctx.dist_timeout_base,
                            dfu_dist_ctx.blob_id, pnode->update_fw_image_idx,
                            dfu_dist_ctx.fw_metadata, dfu_dist_ctx.metadata_len);
            return true;
        }
        pnode = pnode->pnext;
    }
    printi("dfu dist update start: no active node");
    return false;
}

static bool dist_skip_caps_retrieve = false;

void dfu_dist_skip_caps_retrieve_set(bool skip)
{
    dist_skip_caps_retrieve = skip;
}

static bool dfu_dist_blob_transfer(void)
{
    if (dfu_dist_ctx.dist_update_node_list.count == 0)
    {
        printe("dfu_dist_blob_transfer: no node in list");
        return false;
    }

    dfu_dist_ctx.ptransfer_addr = plt_zalloc(dfu_dist_ctx.dist_update_node_list.count * sizeof(
                                                 uint16_t), RAM_TYPE_DATA_ON);
    if (!dfu_dist_ctx.ptransfer_addr)
    {
        printe("dfu_dist_blob_transfer: alloc failed");
        return false;
    }

    dfu_dist_ctx.dist_phase = DFU_DIST_PHASE_BLOB_TRANSFER;
    dfu_dist_ctx.transfer_addr_num = 0;

    dfu_dist_update_node_p pnode = (dfu_dist_update_node_p)
                                   dfu_dist_ctx.dist_update_node_list.pfirst;
    while (pnode)
    {
        if (pnode->node_phase != DIST_NODE_PHASE_FAILED)
        {
            dfu_dist_ctx.ptransfer_addr[dfu_dist_ctx.transfer_addr_num ++] = pnode->addr;
        }
        pnode = pnode->pnext;
    }

    if (dfu_dist_ctx.transfer_addr_num == 0)
    {
        printe("dfu_dist_blob_transfer: all node is failed");
        return false;
    }

    /* change callback since Role change to Distributor */
    blob_client_app_cb_reg(dfu_dist_blob_client_cb);

    if (dist_skip_caps_retrieve)
    {
        blob_client_chunk_size_set(DFU_DIST_CHUNK_SIZE);
        blob_client_block_size_log_set(DFU_DIST_BLOCK_SIZE_LOG);
    }

    blob_client_mtu_size_set(DFU_DIST_CLIENT_MTU);

    if (dist_skip_caps_retrieve)
    {
        return blob_client_blob_transfer(dfu_dist_ctx.dist_multicast_addr,
                                         dfu_dist_ctx.dist_app_key_index,
                                         dfu_dist_ctx.dist_ttl, dfu_dist_ctx.ptransfer_addr,
                                         dfu_dist_ctx.transfer_addr_num,
                                         dfu_dist_ctx.blob_id, dfu_dist_ctx.fw_image_size,
                                         BLOB_TRANSFER_MODE_PUSH, dfu_dist_ctx.dist_timeout_base,
                                         dist_skip_caps_retrieve);
    }
    else
    {
        return blob_client_caps_retrieve(dfu_dist_ctx.dist_app_key_index,
                                         dfu_dist_ctx.dist_ttl,
                                         dfu_dist_ctx.ptransfer_addr,
                                         dfu_dist_ctx.transfer_addr_num);
    }
}

static bool dfu_dist_active_update_get_send(void)
{
    dfu_dist_update_node_p pnode = (dfu_dist_update_node_p)
                                   dfu_dist_ctx.dist_update_node_list.pfirst;
    while (pnode)
    {
        if (pnode->node_phase == DIST_NODE_PHASE_APPLYING &&
            (pnode->retrieved_update_phase == FW_RETRIEVED_UPDATE_PHASE_TRANSFER_ACTIVE ||
             pnode->retrieved_update_phase == FW_RETRIEVED_UPDATE_PHASE_VERIFYING_UPDATE))
        {
            dfu_dist_ctx.dist_retry_count = 0;
            plt_timer_change_period(dfu_dist_ctx.timer, DFU_DIST_UPDATE_GET_RETRY_PERIOD, 0);
            dfu_dist_ctx.pcur_update_node = pnode;
            printi("dfu dist update get: addr 0x%04x, app key index %d",
                   pnode->addr, dfu_dist_ctx.dist_app_key_index);
            fw_update_get(pnode->addr, dfu_dist_ctx.dist_app_key_index);
            return true;
        }
        pnode = pnode->pnext;
    }
    printi("dfu dist update get: no active node");
    return false;
}

static bool dfu_dist_active_update_apply_send(void)
{
    dfu_dist_update_node_p pnode = (dfu_dist_update_node_p)
                                   dfu_dist_ctx.dist_update_node_list.pfirst;
    while (pnode)
    {
        if (pnode->node_phase == DIST_NODE_PHASE_APPLYING &&
            pnode->retrieved_update_phase == FW_RETRIEVED_UPDATE_PHASE_VERIFICATION_SUCCEEDED)
        {
            dfu_dist_ctx.dist_retry_count = 0;
            plt_timer_change_period(dfu_dist_ctx.timer, DFU_DIST_UPDATE_APPLY_RETRY_PERIOD, 0);
            dfu_dist_ctx.pcur_update_node = pnode;
            printi("dfu dist update apply: addr 0x%04x, app key index %d",
                   pnode->addr, dfu_dist_ctx.dist_app_key_index);
            fw_update_apply(pnode->addr, dfu_dist_ctx.dist_app_key_index);
            return true;
        }
        pnode = pnode->pnext;
    }
    printi("dfu dist update apply: no active node");
    return false;
}

static bool dfu_dist_active_update_info_get_send(void)
{
    dfu_dist_update_node_p pnode = (dfu_dist_update_node_p)
                                   dfu_dist_ctx.dist_update_node_list.pfirst;
    while (pnode)
    {
        if (pnode->node_phase == DIST_NODE_PHASE_APPLIED &&
            pnode->retrieved_update_phase == FW_RETRIEVED_UPDATE_PHASE_APPLYING_UPDATE)
        {
            dfu_dist_ctx.dist_retry_count = 0;
            plt_timer_change_period(dfu_dist_ctx.timer, DFU_DIST_UPDATE_INFO_GET_RETRY_PERIOD, 0);
            dfu_dist_ctx.pcur_update_node = pnode;
            printi("dfu dist update info get: addr 0x%04x, app key index %d, fw image index %d",
                   pnode->addr, dfu_dist_ctx.dist_app_key_index,
                   pnode->update_fw_image_idx);
            fw_update_info_get(pnode->addr, dfu_dist_ctx.dist_app_key_index,
                               pnode->update_fw_image_idx, 1);
            return true;
        }
        pnode = pnode->pnext;
    }
    printi("dfu dist update info get: no active node");
    return false;
}

static bool dfu_dist_handle_update_cancel(void)
{
    dfu_dist_update_node_p pnode = (dfu_dist_update_node_p)
                                   dfu_dist_ctx.dist_update_node_list.pfirst;
    while (pnode)
    {
        if (pnode->node_phase == DIST_NODE_PHASE_CANCELING)
        {
            return false;
        }
        pnode = pnode->pnext;
    }

    return true;
}

static void dfu_dist_update_cancel(void)
{
    dfu_dist_ctx.dist_phase = DFU_DIST_PHASE_CANCELING;
    dfu_dist_update_node_p pnode = (dfu_dist_update_node_p)
                                   dfu_dist_ctx.dist_update_node_list.pfirst;
    while (pnode)
    {
        if (pnode->node_phase != DIST_NODE_PHASE_FAILED)
        {
            printi("dfu dist update cancel: addr 0x%04x, app key index %d",
                   pnode->addr, dfu_dist_ctx.dist_app_key_index);
            pnode->node_phase = DIST_NODE_PHASE_CANCELING;
            fw_update_cancel(pnode->addr, dfu_dist_ctx.dist_app_key_index);
        }
        pnode = pnode->pnext;
    }
}

static void dfu_dist_fw_apply(void)
{
    dfu_dist_ctx.dist_phase = DFU_DIST_PHASE_APPLY;
    if (dfu_dist_ctx.dist_role == DIST_ROLE_DISTRIBUTOR)
    {
        fw_dist_server_ctx.dist_phase = FW_DIST_PHASE_APPLYING_UPDATE;
    }
    if (!dfu_dist_active_update_apply_send())
    {
        dfu_dist_handle_dist_result();
    }
}

static void dfu_dist_fw_confirm(void)
{
    dfu_dist_ctx.dist_phase = DFU_DIST_PHASE_CONFIRM;
    if (!dfu_dist_active_update_info_get_send())
    {
        dfu_dist_handle_dist_result();
    }
}

int32_t dfu_dist_update_client_data(const mesh_model_info_p pmodel_info, uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case FW_UPDATE_CLIENT_INFO_STATUS:
        {
            fw_update_client_info_status_t *pdata = (fw_update_client_info_status_t *)pargs;
            dfu_dist_update_node_p pnode = dfu_dist_receiver_get_by_addr(pdata->src);
            if (!pnode)
            {
                printe("dfu update info status: addr 0x%04x not in update list", pdata->src);
                return MODEL_SUCCESS;
            }

            if (dfu_dist_ctx.dist_phase == DFU_DIST_PHASE_CONFIRM)
            {
                plt_timer_stop(dfu_dist_ctx.timer, 0);
                if (dfu_dist_ctx.dist_role == DIST_ROLE_DISTRIBUTOR)
                {
                    fw_image_e_p pimage = (fw_image_e_p)fw_dist_server_get_image_by_index(dfu_dist_ctx.cur_image_index);
                    if (!pimage)
                    {
                        printe("dfu update info status: no image in list, index %d", dfu_dist_ctx.cur_image_index);
                        return MODEL_SUCCESS;
                    }
                    if ((pimage->image.fw_id_len != pdata->pfw_info->fw_id_len) ||
                        (0 != memcmp(&pimage->image.fw_id, &pdata->pfw_info->fw_id,
                                     pdata->pfw_info->fw_id_len)))
                    {
                        printw("dfu update info status: node 0x%04x, fw info mismatch %d-%d", pdata->src,
                               pimage->image.fw_id_len, pdata->pfw_info->fw_id_len);
                        dprintw((uint8_t *)&pimage->image.fw_id, pimage->image.fw_id_len);
                        dprintw((uint8_t *)&pdata->pfw_info->fw_id, pdata->pfw_info->fw_id_len);
                    }
                    else
                    {
                        printi("dfu update info status: node 0x%04x confirm success", pdata->src);
                        pnode->node_phase = DIST_NODE_PHASE_CONFIRMED;
                        pnode->retrieved_update_phase = FW_RETRIEVED_UPDATE_PHASE_APPLY_SUCCESS;
                    }
                }
                else
                {
                    // TODO: check image info
                    pnode->node_phase = DIST_NODE_PHASE_CONFIRMED;
                    pnode->retrieved_update_phase = FW_RETRIEVED_UPDATE_PHASE_APPLY_SUCCESS;
                }

                if (dfu_dist_active_update_info_get_send())
                {
                    return MODEL_SUCCESS;
                }

                dfu_dist_handle_dist_result();
            }
        }
        break;
    case FW_UPDATE_CLIENT_FW_METADATA_STATUS:
        {
            //fw_update_client_fw_metadata_status_t *pdata = (fw_update_client_fw_metadata_status_t *)pargs;
        }
        break;
    case FW_UPDATE_CLIENT_STATUS:
        {
            fw_update_client_status_t *pdata = (fw_update_client_status_t *)pargs;
            dfu_dist_update_node_p pnode = dfu_dist_receiver_get_by_addr(pdata->src);
            if (!pnode)
            {
                printe("dfu update status: addr 0x%04x not in update list", pdata->src);
                return MODEL_SUCCESS;
            }

            pnode->retrieved_update_phase = pdata->update_phase;
            pnode->update_status = pdata->status;

            printi("dfu update status: dist phase %d", dfu_dist_ctx.dist_phase);
            if (dfu_dist_ctx.dist_phase == DFU_DIST_PHASE_UPDATE_START)
            {
                plt_timer_stop(dfu_dist_ctx.timer, 0);
                if (pdata->status != FW_UPDATE_STATUS_SUCCESS)
                {
                    printe("dfu update status: node 0x%04x update start failed, reason %d", pdata->src,
                           pdata->status);
                    dfu_dist_handle_node_fail(pnode);
                }
                else
                {
                    printi("dfu update status: node 0x%04x update start success", pdata->src);
                    pnode->node_phase = DIST_NODE_PHASE_UPDATE_STARTED;
                }

                if (dfu_dist_active_update_start_send())
                {
                    return MODEL_SUCCESS;
                }

                /* all node received update start message, begin blob transfer start */
                if (!dfu_dist_blob_transfer())
                {
                    printi("dfu update status: blob transfer failed");
                    dfu_dist_handle_blob_transfer(false);
                }
            }
            else if (dfu_dist_ctx.dist_phase == DFU_DIST_PHASE_VERIFY)
            {
                plt_timer_stop(dfu_dist_ctx.timer, 0);
                if (pdata->status != FW_UPDATE_STATUS_SUCCESS ||
                    pnode->retrieved_update_phase == FW_RETRIEVED_UPDATE_PHASE_VERIFICATION_FAILED)
                {
                    printe("dfu update status: node 0x%04x verify failed, reason %d, retrieved update phase %d",
                           pdata->src, pdata->status, pnode->retrieved_update_phase);
                    dfu_dist_handle_node_fail(pnode);
                }
                else if (pnode->retrieved_update_phase == FW_RETRIEVED_UPDATE_PHASE_VERIFYING_UPDATE)
                {
                    printe("dfu update status: node 0x%04x verifying...");
                    dfu_dist_ctx.dist_retry_count = 0;
                    plt_timer_change_period(dfu_dist_ctx.timer, DFU_DIST_UPDATE_GET_RETRY_PERIOD, 0);
                    return MODEL_SUCCESS;
                }

                if (dfu_dist_active_update_get_send())
                {
                    return MODEL_SUCCESS;
                }

                dfu_dist_handle_fw_verify();
            }
            else if (dfu_dist_ctx.dist_phase == DFU_DIST_PHASE_APPLY)
            {
                plt_timer_stop(dfu_dist_ctx.timer, 0);
                if (pdata->status != FW_UPDATE_STATUS_SUCCESS)
                {
                    printe("dfu update status: node 0x%04x apply failed, reason %d", pdata->src,
                           pdata->status);
                    dfu_dist_handle_node_fail(pnode);
                }
                else
                {
                    printi("dfu update status: node 0x%04x apply success", pdata->src);
                    pnode->node_phase = DIST_NODE_PHASE_APPLIED;
                }

                if (dfu_dist_active_update_apply_send())
                {
                    return MODEL_SUCCESS;
                }

                dfu_dist_fw_confirm();
            }
            else if (dfu_dist_ctx.dist_phase == DFU_DIST_PHASE_CANCELING)
            {
                plt_timer_stop(dfu_dist_ctx.timer, 0);
                if (pdata->status != FW_UPDATE_STATUS_SUCCESS)
                {
                    printe("dfu update status: node 0x%04x cancel failed, reason %d", pdata->src,
                           pdata->status);
                    dfu_dist_handle_node_fail(pnode);
                }
                else
                {
                    printi("dfu update status: node 0x%04x cancel success", pdata->src);
                    pnode->node_phase = DIST_NODE_PHASE_IDLE;
                }

                if (dfu_dist_handle_update_cancel())
                {
                    dfu_dist_ctx.dist_phase = DFU_DIST_PHASE_IDLE;
                    if (dfu_dist_ctx.dist_role == DIST_ROLE_DISTRIBUTOR)
                    {
                        fw_dist_server_ctx.dist_phase = FW_DIST_PHASE_IDLE;
                    }
                }
            }
        }
        break;
    default:
        break;
    }

    return MODEL_SUCCESS;
}

// RTK porting:call common API for send T_IO_MSG msg to app main task and process msg through app main task
extern uint16_t bt_stack_msg_send(uint16_t type, uint16_t subtype, void *msg);
static void dfu_dist_timeout(void *ptimer)
{
    bt_stack_msg_send(IO_MSG_TYPE_LE_MESH, RTK_BT_MESH_IO_MSG_SUBTYPE_DFU_DIST_APP_TIMEOUT_MSG, ptimer);
}

void dfu_dist_handle_timeout(void)
{
    printi("dfu_dist_handle_timeout: dist phase %d", dfu_dist_ctx.dist_phase);
    dfu_dist_update_node_p pnode = dfu_dist_ctx.pcur_update_node;
    if (dfu_dist_ctx.dist_phase == DFU_DIST_PHASE_UPDATE_START)
    {
        if (pnode->node_phase == DIST_NODE_PHASE_UPDATE_STARTING)
        {
            if (dfu_dist_ctx.dist_retry_count >= DFU_DIST_RETRY_TIMES)
            {
                /* update start failed, skip this node */
                printe("dfu_dist_handle_timeout: fw update start, dst 0x%04x failed",
                       pnode->addr);
                dfu_dist_handle_node_fail(pnode);
            }
            else
            {
                dfu_dist_ctx.dist_retry_count ++;
                plt_timer_change_period(dfu_dist_ctx.timer, DFU_DIST_UPDATE_START_RETRY_PERIOD, 0);
                printi("dfu_dist_handle_timeout: fw update start, dst 0x%04x, retry count %d",
                       pnode->addr, dfu_dist_ctx.dist_retry_count);
                fw_update_start(pnode->addr, dfu_dist_ctx.dist_app_key_index,
                                dfu_dist_ctx.dist_ttl, dfu_dist_ctx.dist_timeout_base,
                                dfu_dist_ctx.blob_id, pnode->update_fw_image_idx,
                                dfu_dist_ctx.fw_metadata, dfu_dist_ctx.metadata_len);
                return;
            }
        }

        if (dfu_dist_active_update_start_send())
        {
            return;
        }

        /* all node received update start message, begin blob transfer start */
        if (!dfu_dist_blob_transfer())
        {
            printi("dfu_dist_handle_timeout: blob transfer failed");
            dfu_dist_handle_blob_transfer(false);
        }
    }
    else if (dfu_dist_ctx.dist_phase == DFU_DIST_PHASE_VERIFY)
    {
        if (pnode->node_phase == DIST_NODE_PHASE_APPLYING)
        {
            if (dfu_dist_ctx.dist_retry_count >= DFU_DIST_RETRY_TIMES)
            {
                printe("dfu_dist_handle_timeout: fw update get, dst 0x%04x verify failed", pnode->addr);
                dfu_dist_handle_node_fail(pnode);
            }
            else
            {
                dfu_dist_ctx.dist_retry_count ++;
                plt_timer_change_period(dfu_dist_ctx.timer, DFU_DIST_UPDATE_GET_RETRY_PERIOD, 0);
                printi("dfu_dist_handle_timeout: fw update get, dst 0x%04x, retry count %d",
                       pnode->addr, dfu_dist_ctx.dist_retry_count);
                fw_update_get(pnode->addr, dfu_dist_ctx.dist_app_key_index);
                return;
            }
        }

        if (dfu_dist_active_update_get_send())
        {
            return;
        }

        dfu_dist_handle_fw_verify();
    }
    else if (dfu_dist_ctx.dist_phase == DFU_DIST_PHASE_APPLY)
    {
        if (pnode->node_phase == DIST_NODE_PHASE_APPLYING)
        {
            if (dfu_dist_ctx.dist_retry_count >= DFU_DIST_RETRY_TIMES)
            {
                printe("dfu_dist_handle_timeout: fw update apply, dst 0x%04x apply failed", pnode->addr);
                dfu_dist_handle_node_fail(pnode);
            }
            else
            {
                dfu_dist_ctx.dist_retry_count ++;
                plt_timer_change_period(dfu_dist_ctx.timer, DFU_DIST_UPDATE_APPLY_RETRY_PERIOD, 0);
                printi("dfu_dist_handle_timeout: fw update apply, dst 0x%04x, retry count %d",
                       pnode->addr, dfu_dist_ctx.dist_retry_count);
                fw_update_apply(pnode->addr, dfu_dist_ctx.dist_app_key_index);
                return;
            }
        }

        if (dfu_dist_active_update_apply_send())
        {
            return;
        }

        // all node applied
        dfu_dist_fw_confirm();
    }
    else if (dfu_dist_ctx.dist_phase == DFU_DIST_PHASE_CONFIRM)
    {
        if (pnode->node_phase == DIST_NODE_PHASE_APPLIED)
        {
            if (dfu_dist_ctx.dist_retry_count >= DFU_DIST_RETRY_TIMES)
            {
                printe("dfu_dist_handle_timeout: fw update info get, dst 0x%04x failed");
                dfu_dist_handle_node_fail(pnode);
            }
            else
            {
                dfu_dist_ctx.dist_retry_count ++;
                plt_timer_change_period(dfu_dist_ctx.timer, DFU_DIST_UPDATE_APPLY_RETRY_PERIOD, 0);
                printi("dfu_dist_handle_timeout: fw update info get, dst 0x%04x, retry count %d",
                       pnode->addr, dfu_dist_ctx.dist_retry_count);
                fw_update_info_get(pnode->addr, dfu_dist_ctx.dist_app_key_index,
                                   pnode->update_fw_image_idx, 1);
                return;
            }
        }

        if (dfu_dist_active_update_info_get_send())
        {
            return;
        }

        dfu_dist_handle_dist_result();
    }
}

bool dfu_dist_start(uint16_t dst, uint16_t app_key_index, uint16_t update_timeout_base,
                    uint8_t *pfw_metadata, uint8_t metadata_len, uint32_t fw_image_size)
{
    if (DFU_DIST_PHASE_IDLE != dfu_dist_ctx.dist_phase)
    {
        printe("dfu_dist_start: busy, phase %d", dfu_dist_ctx.dist_phase);
        return false;
    }

    if (0 == dfu_dist_ctx.dist_update_node_list.count)
    {
        printe("dfu_dist_start: there is no node need to update");
        return false;
    }

    /* start timeout timer */
    if (!dfu_dist_ctx.timer)
    {
        dfu_dist_ctx.timer = plt_timer_create("dfu", DFU_DIST_UPDATE_START_RETRY_PERIOD, false, 0,
                                              dfu_dist_timeout);
        if (!dfu_dist_ctx.timer)
        {
            printe("dfu_dist_start: create timer failed");
            return false;
        }
        plt_timer_start(dfu_dist_ctx.timer, 0);
    }

    dfu_dist_ctx.dist_phase = DFU_DIST_PHASE_UPDATE_START;
    dfu_dist_ctx.dist_multicast_addr = dst;
    dfu_dist_ctx.dist_app_key_index = app_key_index;
    dfu_dist_ctx.dist_timeout_base = update_timeout_base;
    dfu_dist_ctx.dist_ttl = mesh_node.ttl;
    dfu_dist_ctx.fw_image_size = fw_image_size;
    dfu_dist_ctx.dist_retry_count = 0;
    memcpy(dfu_dist_ctx.fw_metadata, pfw_metadata, metadata_len);
    dfu_dist_ctx.metadata_len = metadata_len;
    plt_rand(dfu_dist_ctx.blob_id, sizeof(dfu_dist_ctx.blob_id));

    printi("dfu_dist_start: dst 0x%04x, app_key_index %d, update_timeout_base %d, fw_image_size %d",
           dfu_dist_ctx.dist_multicast_addr, dfu_dist_ctx.dist_app_key_index,
           dfu_dist_ctx.dist_timeout_base, dfu_dist_ctx.fw_image_size);
    printi("dfu_dist_start: firmware metadata = ");
    dprinti(dfu_dist_ctx.fw_metadata, dfu_dist_ctx.metadata_len);
    printi("dfu_dist_start: blob id = ");
    dprinti(dfu_dist_ctx.blob_id, 8);

    if (!dfu_dist_active_update_start_send())
    {
        printi("dfu_dist_start: update start failed");
        dfu_dist_handle_blob_transfer(false);
        return false;
    }

    return true;
}

int32_t dfu_dist_server_data(const mesh_model_info_p pmodel_info, uint32_t type,
                             void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case FW_DIST_SERVER_START:
        {
            fw_dist_server_start_t *pdata = (fw_dist_server_start_t *)pargs;
            /* start update */
            dfu_dist_ctx.cur_image_index = pdata->dist_fw_image_index;
            fw_image_e_p pimage = (fw_image_e_p)fw_dist_server_get_image_by_index(dfu_dist_ctx.cur_image_index);
            if (!pimage)
            {
                printi("dfu dist start: no image in list, index %d", dfu_dist_ctx.cur_image_index);
                return MODEL_SUCCESS;
            }
            dfu_dist_ctx.dist_update_policy = pdata->dist_update_policy;
            if (pdata->preceiver && pdata->receiver_num)
            {
                for (uint16_t i = 0; i < pdata->receiver_num ; i++)
                {
                    dfu_dist_add_receiver(pdata->preceiver[i].addr, pdata->preceiver[i].update_fw_image_idx);
                }
            }
            dfu_dist_start(pdata->dist_multicast_addr, pdata->dist_app_key_index,
                           pdata->dist_timeout_base, pimage->image.metadata,
                           pimage->image.metadata_len, pimage->dfu_image_size);
            printi("dfu dist start: dst 0x%04x, image size %d, update policy %d",
                   pdata->dist_multicast_addr, pimage->dfu_image_size,
                   pdata->dist_update_policy);
        }
        break;
    case FW_DIST_SERVER_SUSPEND:
        {
            /* dist suspend */
            printi("dfu dist suspend: distribution suspend!");
        }
        break;
    case FW_DIST_SERVER_CANCEL:
        {
            /* dist cancel */
            printi("dfu dist cancel: distribution cancel!");
            dfu_dist_update_cancel();
            blob_client_transfer_cancel(dfu_dist_ctx.ptransfer_addr,
                                        dfu_dist_ctx.transfer_addr_num, dfu_dist_ctx.blob_id);
        }
        break;
    case FW_DIST_SERVER_APPLY:
        {
            /* start apply */
            printi("dfu dist apply: start firmware apply");
            dfu_dist_fw_apply();
        }
        break;
    case FW_DIST_SERVER_UPLOAD_START:
    case FW_DIST_SERVER_UPLOAD_OOB_START:
    case FW_DIST_SERVER_UPLOAD_BLOCK_DATA:
    case FW_DIST_SERVER_UPLOAD_COMPLETE:
    case FW_DIST_SERVER_UPLOAD_FAIL:
    case FW_DIST_SERVER_URI_CHECK:
        {
            if (type == FW_DIST_SERVER_UPLOAD_START)
            {
                /* change callback since Role change to Distributor */
                blob_transfer_server_set_data_cb(fw_dist_handle_blob_server_data);
            }

            if (dfu_dist_ctx.dfu_dist_cb)
            {
                dfu_dist_upload_t cb_data = {0};
                cb_data.upload_type = type;
                cb_data.pupload_data = pargs;
                dfu_dist_ctx.dfu_dist_cb(MESH_MSG_DFU_DIST_UPLOAD, &cb_data);
            }
        }
        break;
    case FW_DIST_SERVER_FW_DELETE:
        {
            // fw_dist_server_fw_delete_t *pdata = (fw_dist_server_fw_delete_t *)pargs;
        }
        break;
    case FW_DIST_SERVER_FW_DELETE_ALL:
        {
        }
        break;
    case FW_DIST_SERVER_UPDATE_NODE_STATUS_GET:
        {
            fw_dist_server_update_node_status_get_t *pdata = (fw_dist_server_update_node_status_get_t *)pargs;
            dfu_dist_update_node_p pnode = dfu_dist_receiver_get(pdata->pnode->addr,
                                                                 pdata->pnode->update_fw_image_idx);
            if (pnode)
            {
                pdata->pnode->retrieved_update_phase = pnode->retrieved_update_phase;
                pdata->pnode->update_status = pnode->update_status;
                /* FIXME: get the right BLOB transfer status */
                pdata->pnode->transfer_status = pnode->transfer_status;
                pdata->pnode->transfer_progress = pnode->transfer_progress;
            }
        }
        break;
    default:
        break;
    }

    return MODEL_SUCCESS;
}

uint16_t dfu_dist_blob_client_cb(uint8_t type, void *pdata)
{
    switch (type)
    {
    case MESH_MSG_BLOB_CLIENT_APP_TRANSFER:
        {
            blob_client_app_transfer_t *ptransfer = (blob_client_app_transfer_t *)pdata;
            if (ptransfer->type == BLOB_CB_TYPE_NODE_FAIL)
            {
                dfu_dist_update_node_p pnode = dfu_dist_receiver_get_by_addr(ptransfer->addr);
                if (pnode)
                {
                    printe("dfu_dist_blob_client_cb: node 0x%04x failed in client phase %d, node phase %d",
                           ptransfer->addr, ptransfer->client_phase, pnode->node_phase);
                    dfu_dist_handle_node_fail(pnode);
                }
            }

            if (ptransfer->procedure == BLOB_CB_PROCEDURE_TRANSFER &&
                dfu_dist_ctx.dist_phase == DFU_DIST_PHASE_BLOB_TRANSFER)
            {
                if (ptransfer->type == BLOB_CB_TYPE_SUCCESS)
                {
                    printi("dfu_dist_blob_client_cb: firmware send complete");
                    dfu_dist_handle_blob_transfer(true);
                    /* start verify and apply */
                    dfu_dist_update_node_p pnode = (dfu_dist_update_node_p)
                                                   dfu_dist_ctx.dist_update_node_list.pfirst;
                    while (pnode)
                    {
                        if (pnode->node_phase == DIST_NODE_PHASE_UPDATE_STARTED)
                        {
                            pnode->node_phase = DIST_NODE_PHASE_APPLYING;
                        }
                        pnode = pnode->pnext;
                    }

                    dfu_dist_ctx.dist_phase = DFU_DIST_PHASE_VERIFY;

                    dfu_dist_active_update_get_send();
                }
                else if (ptransfer->type == BLOB_CB_TYPE_PROGRESS)
                {
                    printi("dfu_dist_blob_client_cb: progress %d%%", ptransfer->progress);
                    dfu_dist_update_node_p pnode = (dfu_dist_update_node_p)
                                                   dfu_dist_ctx.dist_update_node_list.pfirst;
                    while (pnode)
                    {
                        if (pnode->node_phase == DIST_NODE_PHASE_UPDATE_STARTED)
                        {
                            pnode->transfer_progress = ptransfer->progress / 2;
                        }
                        pnode = pnode->pnext;
                    }

                    if (dfu_dist_ctx.dfu_dist_cb)
                    {
                        dfu_dist_transfer_t cb_data = {0};
                        cb_data.type = DFU_DIST_CB_TYPE_TRANSFER_PROGRESS;
                        cb_data.dist_phase = dfu_dist_ctx.dist_phase;
                        cb_data.progress = ptransfer->progress;
                        dfu_dist_ctx.dfu_dist_cb(MESH_MSG_DFU_DIST_TRANSFER, &cb_data);
                    }
                }
                else if (ptransfer->type == BLOB_CB_TYPE_FAIL)
                {
                    printe("dfu_dist_blob_client_cb: firmware send failed");
                    dfu_dist_handle_blob_transfer(false);
                }
            }
            else if (ptransfer->procedure == BLOB_CB_PROCEDURE_CAPS_RETRIEVE)
            {
                if (ptransfer->type == BLOB_CB_TYPE_SUCCESS &&
                    !dist_skip_caps_retrieve)
                {
                    blob_client_blob_transfer(dfu_dist_ctx.dist_multicast_addr,
                                              dfu_dist_ctx.dist_app_key_index,
                                              dfu_dist_ctx.dist_ttl, dfu_dist_ctx.ptransfer_addr,
                                              dfu_dist_ctx.transfer_addr_num,
                                              dfu_dist_ctx.blob_id, dfu_dist_ctx.fw_image_size,
                                              BLOB_TRANSFER_MODE_PUSH, dfu_dist_ctx.dist_timeout_base,
                                              dist_skip_caps_retrieve);
                }
            }
        }
        break;
    case MESH_MSG_BLOB_CLIENT_APP_PARAM:
        {
            blob_client_app_param_t *pparam = (blob_client_app_param_t *)pdata;
            if (dfu_dist_ctx.dfu_dist_cb)
            {
                dfu_dist_blob_param_t cb_data = {0};
                cb_data.blob_size = pparam->blob_size;
                cb_data.block_size = pparam->block_size;
                cb_data.total_blocks = pparam->total_blocks;
                cb_data.chunk_size = pparam->chunk_size;
                dfu_dist_ctx.dfu_dist_cb(MESH_MSG_DFU_DIST_BLOB_PARAM, &cb_data);
            }
        }
        break;
    case MESH_MSG_BLOB_CLIENT_APP_BLOCK_LOAD:
        {
            blob_client_app_block_load_t *pblock_get = (blob_client_app_block_load_t *)pdata;
            if (dfu_dist_ctx.dfu_dist_cb)
            {
                dfu_dist_block_load_t cb_data = {0};
                cb_data.image_index = dfu_dist_ctx.cur_image_index;
                cb_data.offset = pblock_get->offset;
                cb_data.block_size = pblock_get->block_size;
                cb_data.pblock_data = pblock_get->pblock_data;
                dfu_dist_ctx.dfu_dist_cb(MESH_MSG_DFU_DIST_BLOCK_LOAD, &cb_data);
            }
        }
        break;
    default:
        break;
    }

    return 0x00;
}

void dfu_dist_models_init(uint8_t element_index, pf_dfu_dist_cb_t pf_dfu_dist_cb)
{
    dfu_dist_ctx.dist_role = DIST_ROLE_DISTRIBUTOR;
    dfu_dist_ctx.dfu_dist_cb = pf_dfu_dist_cb;
    dfu_dist_ctx.dist_update_policy = FW_UPDATE_POLICY_VERIFY_AND_UPDATE;
    fw_dist_server_reg(element_index, dfu_dist_server_data);
    fw_update_client_reg(element_index, dfu_dist_update_client_data);
    blob_client_app_init(element_index, dfu_dist_blob_client_cb);
    blob_transfer_server_reg(element_index, fw_dist_handle_blob_server_data);
}

void dfu_standalone_updater_models_init(uint8_t element_index, pf_dfu_dist_cb_t pf_dfu_dist_cb)
{
    dfu_dist_ctx.dist_role = DIST_ROLE_STANDALONE_UPDATER;
    dfu_dist_ctx.dfu_dist_cb = pf_dfu_dist_cb;
    fw_update_client_reg(element_index, dfu_dist_update_client_data);
    blob_client_app_init(element_index, dfu_dist_blob_client_cb);
}
#endif
