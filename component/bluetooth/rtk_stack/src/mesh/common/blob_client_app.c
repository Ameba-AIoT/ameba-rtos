/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      blob_client_app.c
* @brief     Smart mesh blob client application
* @details
* @author    sterling_jiang
* @date      2023-02-23
* @version   v1.0
* *********************************************************************************************************
*/

#define MM_ID MM_APP

#include "mesh_api.h"
#include "app_msg.h"
#include "generic_types.h"
#include "blob_client_app.h"

#if F_BT_MESH_1_1_MBT_SUPPORT

#define BLOB_RETRY_TIMES                            3
#define BLOB_INFO_GET_RETRY_PERIOD                  2000
#define BLOB_TRANSFER_START_RETRY_PERIOD            2000
#define BLOB_BLOCK_START_RETRY_PERIOD               2000
#define BLOB_BLOCK_GET_RETRY_PERIOD                 2000
#define BLOB_BLOCK_TRANSFER_CANCEL_PERIOD           2000
#define BLOB_BLOCK_REPORT_PERIOD                    ((30 * 2 + 7) * 1000)

/* transfer capabilities */
#define BLOB_CLIENT_MTU                             376

typedef struct _blob_recvs_caps_t
{
    struct _blob_recvs_caps_t *pnext;
    uint16_t addr;
    blob_server_capabilities_t caps;
} blob_recvs_caps_t, *blob_recvs_caps_p;

typedef struct _blob_recvs_node_t
{
    struct _blob_recvs_node_t *pnext;
    bool active;
    uint16_t addr;
    blob_transfer_phase_t retrieved_transfer_phase;
    blob_transfer_status_code_t transfer_status;
    uint16_t *blocks_not_recv;
    uint16_t blocks_not_recv_len;
    uint16_t *pmissing_chunks;
    uint16_t current_missing_chunks_len;
    blob_node_phase_t node_phase;
} blob_recvs_node_t, *blob_recvs_node_p;

typedef enum
{
    BLOB_DATA_TYPE_UNSET,
    BLOB_DATA_TYPE_FLASH,
    BLOB_DATA_TYPE_RAM,
    BLOB_DATA_TYPE_OTHER,
} blob_data_type_t;

struct
{
    plt_timer_t procedure_timer;
    plt_timer_t retry_timer;
    uint8_t retry_count;
    blob_client_phase_t phase;
    plt_list_t blob_caps_list;
    blob_recvs_caps_t *pcur_recvs_caps;
    plt_list_t blob_recvs_list;
    blob_recvs_node_t *pcur_recvs_node;
    uint16_t multicast_addr;
    uint8_t app_key_index;
    uint16_t client_timeout_base;
    uint8_t transfer_ttl;
    blob_transfer_mode_t transfer_mode;
    uint16_t client_mtu_size;
    pf_blob_client_cb_t blob_client_cb;

    uint32_t blob_size;
    uint32_t blob_left_size;
    uint8_t blob_id[8];
    /* current transfer data */
    uint16_t block_num;
    uint16_t total_blocks;
    uint8_t *pblock_data;               // Space will be allocated
    uint8_t block_size_log;
    uint16_t block_size;
    uint16_t chunk_num;
    uint8_t *pchunk_data;
    uint16_t chunk_size;
    uint16_t current_total_chunks;
    uint16_t current_chunk_size;
    uint8_t *sending_chunks;
    uint8_t current_sending_chunks_len;
} blob_client_ctx;

extern void *evt_queue_handle;  //!< Event queue handle
extern void *io_queue_handle;   //!< IO queue handle

void blob_client_mtu_size_set(uint16_t client_mtu_size)
{
    blob_client_ctx.client_mtu_size = client_mtu_size;
}

void blob_client_block_size_log_set(uint8_t block_size_log)
{
    blob_client_ctx.block_size_log = block_size_log;
}

void blob_client_chunk_size_set(uint16_t chunk_size)
{
    blob_client_ctx.chunk_size = chunk_size;
}

blob_recvs_caps_t *blob_recvs_caps_get_by_addr(uint16_t addr)
{
    blob_recvs_caps_p pcaps = (blob_recvs_caps_t *)blob_client_ctx.blob_caps_list.pfirst;
    while (pcaps != NULL)
    {
        if (pcaps->addr == addr)
        {
            return pcaps;
        }
        pcaps = pcaps->pnext;
    }

    return NULL;
}

bool blob_recvs_caps_add(uint16_t addr)
{
    blob_recvs_caps_p pcaps = blob_recvs_caps_get_by_addr(addr);
    if (pcaps != NULL)
    {
        printe("blob_recvs_caps_add: already add, addr 0x%04x", addr);
        return false;
    }

    pcaps = plt_zalloc(sizeof(blob_recvs_caps_t), RAM_TYPE_DATA_ON);
    if (pcaps == NULL)
    {
        printe("blob_recvs_caps_add: allocate failed");
        return false;
    }

    pcaps->addr = addr;
    memset(&pcaps->caps, 0, sizeof(pcaps->caps));
    plt_list_push(&blob_client_ctx.blob_caps_list, pcaps);
    printi("blob_recvs_caps_add: addr 0x%04x", pcaps->addr);
    return true;
}

bool blob_recvs_caps_delete_by_addr(uint16_t addr)
{
    blob_recvs_caps_p pcaps = blob_recvs_caps_get_by_addr(addr);
    if (pcaps == NULL)
    {
        printe("blob_recvs_caps_delete_by_addr: not in list, addr 0x%04x", addr);
        return false;
    }

    printi("blob_recvs_caps_delete_by_addr: addr 0x%04x", pcaps->addr);
    plt_list_remove(&blob_client_ctx.blob_caps_list, pcaps);
    return true;
}

void blob_recvs_caps_clear_all(void)
{
    printi("blob_recvs_caps_clear_all");
    blob_recvs_caps_p pcaps = (blob_recvs_caps_p)blob_client_ctx.blob_caps_list.pfirst;
    while (pcaps)
    {
        blob_recvs_caps_delete_by_addr(pcaps->addr);
        pcaps = pcaps->pnext;
    }
}

blob_recvs_node_t *blob_recvs_node_get_by_addr(uint16_t addr)
{
    blob_recvs_node_p pnode = (blob_recvs_node_t *)blob_client_ctx.blob_recvs_list.pfirst;
    while (pnode != NULL)
    {
        if (pnode->addr == addr)
        {
            return pnode;
        }
        pnode = pnode->pnext;
    }

    return NULL;
}

bool blob_recvs_node_add(uint16_t addr)
{
    blob_recvs_node_p pnode = blob_recvs_node_get_by_addr(addr);
    if (pnode != NULL)
    {
        printe("blob_recvs_node_add: already add, addr 0x%04x", addr);
        return false;
    }

    pnode = plt_zalloc(sizeof(blob_recvs_node_t), RAM_TYPE_DATA_ON);
    if (pnode == NULL)
    {
        printe("blob_recvs_node_add: allocate failed");
        return false;
    }

    pnode->addr = addr;
    pnode->retrieved_transfer_phase = BLOB_TRANSFER_PHASE_INACTIVE;
    pnode->transfer_status = BLOB_TRANSFER_STATUS_SUCCESS;
    pnode->blocks_not_recv = NULL;
    pnode->blocks_not_recv_len = 0;
    pnode->pmissing_chunks = NULL;
    pnode->current_missing_chunks_len = 0;
    pnode->node_phase = BLOB_NODE_PHASE_IDLE;
    plt_list_push(&blob_client_ctx.blob_recvs_list, pnode);
    printi("blob_recvs_node_add: addr 0x%04x", pnode->addr);
    return true;
}

bool blob_recvs_node_delete_by_addr(uint16_t addr)
{
    blob_recvs_node_p pnode = blob_recvs_node_get_by_addr(addr);
    if (pnode == NULL)
    {
        printe("blob_recvs_node_delete_by_addr: not in list, addr 0x%04x", addr);
        return false;
    }

    plt_list_remove(&blob_client_ctx.blob_recvs_list, pnode);
    printe("blob_recvs_node_delete_by_addr: addr 0x%04x", addr);

    if (pnode->blocks_not_recv != NULL)
    {
        plt_free(pnode->blocks_not_recv, RAM_TYPE_DATA_ON);
        pnode->blocks_not_recv = NULL;
    }

    if (pnode->pmissing_chunks != NULL)
    {
        plt_free(pnode->pmissing_chunks, RAM_TYPE_DATA_ON);
        pnode->pmissing_chunks = NULL;
    }

    plt_free(pnode, RAM_TYPE_DATA_ON);
    pnode = NULL;
    return true;
}

void blob_recvs_node_clear_all(void)
{
    printi("blob_recvs_node_clear_all");
    blob_recvs_node_p pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
    while (pnode)
    {
        blob_recvs_node_delete_by_addr(pnode->addr);
        pnode = pnode->pnext;
    }
}

#if 0
blob_recvs_node_t *active_blob_recvs_node_get_by_addr(uint16_t addr)
{
    blob_recvs_node_p pnode = blob_recvs_node_get_by_addr(addr);
    if (pnode->active == false)
    {
        printe("active_blob_recvs_node_get_by_addr: not active, addr 0x%04x", addr);
        return NULL;
    }

    return pnode;
}

bool active_blob_recvs_node_add(uint16_t addr)
{
    blob_recvs_node_p pnode = blob_recvs_node_get_by_addr(addr);
    if (pnode == NULL)
    {
        printe("active_blob_recvs_node_add: not in list, addr 0x%04x", addr);
        return false;
    }
    else if (pnode->active)
    {
        printe("active_blob_recvs_node_add: already add, addr 0x%04x", addr);
        return false;
    }

    pnode->active = true;
    return true;
}

bool active_blob_recvs_node_delete_by_addr(uint16_t addr)
{
    blob_recvs_node_p pnode = active_blob_recvs_node_get_by_addr(addr);
    if (pnode == NULL)
    {
        printe("active_blob_recvs_node_delete_by_addr: not in list or not active, addr 0x%04x",
               addr);
        return false;
    }

    pnode->active = false;
    return true;
}
#endif

void blob_client_procedure_timeout(void *pargs)
{
    uint8_t event = EVENT_IO_TO_APP;
    T_IO_MSG msg;
    msg.type = BLOB_CLIENT_PROCEDURE_TIMEOUT;
    if (os_msg_send(io_queue_handle, &msg, 0) == false)
    {
    }
    else if (os_msg_send(evt_queue_handle, &event, 0) == false)
    {
    }
}

static bool blob_client_procedure_timer_is_active(void)
{
    bool ret = false;
    if (blob_client_ctx.procedure_timer)
    {
        ret = plt_timer_is_active(blob_client_ctx.procedure_timer);
    }
    return ret;
}

static bool blob_client_procedure_timer_start(void)
{
    uint32_t period_ms = 10000 * (blob_client_ctx.client_timeout_base + 2) + 100 *
                         blob_client_ctx.transfer_ttl;
    if (NULL == blob_client_ctx.procedure_timer)
    {
        blob_client_ctx.procedure_timer = plt_timer_create("blob_cp", period_ms, false, 0,
                                                           blob_client_procedure_timeout);
        if (NULL == blob_client_ctx.procedure_timer)
        {
            printe("blob_client_procedure_timer_start: create blob procedure timer failed!");
            return false;
        }
        plt_timer_start(blob_client_ctx.procedure_timer, 0);
    }
    else
    {
        plt_timer_change_period(blob_client_ctx.procedure_timer, period_ms, 0);
    }
    return true;
}

void blob_client_procedure_timer_stop(void)
{
    if (blob_client_ctx.procedure_timer)
    {
        plt_timer_delete(blob_client_ctx.procedure_timer, 0);
        blob_client_ctx.procedure_timer = NULL;
    }
}

void blob_client_retry_timeout(void *pargs)
{
    uint8_t event = EVENT_IO_TO_APP;
    T_IO_MSG msg;
    msg.type = BLOB_CLIENT_RETRY_TIMEOUT;
    if (os_msg_send(io_queue_handle, &msg, 0) == false)
    {
    }
    else if (os_msg_send(evt_queue_handle, &event, 0) == false)
    {
    }
}

static bool blob_client_retry_timer_start(uint32_t period_ms)
{
    if (NULL == blob_client_ctx.retry_timer)
    {
        blob_client_ctx.retry_timer = plt_timer_create("blob_cr", period_ms, true, 0,
                                                       blob_client_retry_timeout);
        if (NULL == blob_client_ctx.retry_timer)
        {
            printe("blob_client_procedure_timer_start: create blob retry timer failed!");
            return false;
        }
        plt_timer_start(blob_client_ctx.retry_timer, 0);
    }
    else
    {
        plt_timer_change_period(blob_client_ctx.retry_timer, period_ms, 0);
    }
    return true;
}

void blob_client_retry_timer_stop(void)
{
    if (blob_client_ctx.retry_timer)
    {
        plt_timer_delete(blob_client_ctx.retry_timer, 0);
        blob_client_ctx.retry_timer = NULL;
    }
}

static bool blob_client_blob_init(uint32_t blob_size, blob_transfer_mode_t transfer_mode,
                                  bool skip_caps_retrieve)
{
    blob_recvs_node_p pnode = NULL;
    blob_recvs_node_p pnode_min_chunk_size = NULL;
    blob_recvs_caps_p pcaps = NULL;
    uint8_t avail_count = 0;
    uint8_t max_avail_count = 0;
    uint16_t chunk_size = blob_client_ctx.chunk_size;
    uint16_t chunk_size_min = 0;
    uint16_t total_chunks_max = 0;
    uint16_t total_blocks_max = 0;
    bool chunk_size_suit = false;

    if (skip_caps_retrieve)
    {
        goto INIT;
    }

    /* check max blob size */
    pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
    while (pnode)
    {
        pcaps = blob_recvs_caps_get_by_addr(pnode->addr);
        if (pcaps->caps.max_blob_size < blob_size)
        {
            blob_recvs_node_delete_by_addr(pnode->addr);
            printw("blob_client_blob_init: delete 0x%04x, blob size not match %d > %d", pnode->addr,
                   blob_size, pcaps->caps.max_blob_size);
        }
        pnode = pnode->pnext;
    }

    if (blob_client_ctx.blob_recvs_list.count == 0)
    {
        printe("blob_client_blob_init: fail, no node, %d", __LINE__);
        return false;
    }

    /* check supported transfer mode */
    pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
    while (pnode)
    {
        pcaps = blob_recvs_caps_get_by_addr(pnode->addr);
        if ((transfer_mode == BLOB_TRANSFER_MODE_PUSH && pcaps->caps.mode_push_support == 0) ||
            (transfer_mode == BLOB_TRANSFER_MODE_PULL && pcaps->caps.mode_pull_support == 0))
        {
            blob_recvs_node_delete_by_addr(pnode->addr);
            printw("blob_client_blob_init: delete 0x%04x, transfer mode not support %d", pnode->addr,
                   transfer_mode);
        }
        pnode = pnode->pnext;
    }

    if (blob_client_ctx.blob_recvs_list.count == 0)
    {
        printe("blob_client_blob_init: fail, no node, %d", __LINE__);
        return false;
    }

    /* choose suitable chunk size & block size, may run several loops */
    while (1)
    {
        avail_count = 0;
        max_avail_count = 0;
        chunk_size = 0;
        chunk_size_min = 0;
        total_chunks_max = 0;
        total_blocks_max = 0;

        /* choose chunk size and node which caps contain min chunk size  */
        pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
        while (pnode)
        {
            pcaps = blob_recvs_caps_get_by_addr(pnode->addr);
            chunk_size_min = MIN(pcaps->caps.max_chunk_size, pcaps->caps.server_mtu_size - 3);
            if (chunk_size_min < chunk_size || chunk_size == 0)
            {
                chunk_size = chunk_size_min;
                pnode_min_chunk_size = pnode;
            }
            pnode = pnode->pnext;
        }
        if (blob_client_ctx.client_mtu_size - 3 < chunk_size)
        {
            chunk_size = blob_client_ctx.client_mtu_size - 3;
            chunk_size_suit = true;
        }
        printi("blob_client_blob_init: choose chunk size %d", chunk_size);

        /* choose block log size */
        for (uint8_t i = BLOB_BLOCK_SIZE_LOG_MIN; i <= BLOB_BLOCK_SIZE_LOG_MAX; i++)
        {
            avail_count = 0;
            pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
            while (pnode)
            {
                pcaps = blob_recvs_caps_get_by_addr(pnode->addr);
                total_chunks_max = MIN(pcaps->caps.max_total_chunks, 8 * (pcaps->caps.server_mtu_size - 6));
                if (pcaps->caps.min_block_size_log <= i &&
                    pcaps->caps.max_block_size_log >= i &&
                    total_chunks_max * chunk_size >= plt_exp2(i))
                {
                    avail_count++;
                }
                pnode = pnode->pnext;
            }
            if (avail_count >= max_avail_count)
            {
                max_avail_count = avail_count;
                blob_client_ctx.block_size_log = i;
            }
        }
        printi("blob_client_blob_init: choose block size log %d", blob_client_ctx.block_size_log);

        if (max_avail_count == 0)
        {
            printe("blob_client_blob_init: fail, no max avail count, %d", __LINE__);
            return false;
        }

        /* inactive node which do not match the block size */
        pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
        while (pnode)
        {
            pcaps = blob_recvs_caps_get_by_addr(pnode->addr);
            total_chunks_max = MIN(pcaps->caps.max_total_chunks, 8 * (pcaps->caps.server_mtu_size - 6));
            total_blocks_max = 8 * (pcaps->caps.server_mtu_size - 19);
            /* set node inactive which do not match block log size */
            if (pcaps->caps.min_block_size_log > blob_client_ctx.block_size_log ||
                pcaps->caps.max_block_size_log < blob_client_ctx.block_size_log ||
                total_chunks_max * chunk_size < plt_exp2(blob_client_ctx.block_size_log))
            {
                pnode->active = false;
                printw("blob_client_blob_init: inactive 0x%04x, block log size not match %d(%d,%d), total chunks %d, chunk size %d",
                       pnode->addr, blob_client_ctx.block_size_log, pcaps->caps.min_block_size_log,
                       pcaps->caps.max_block_size_log, total_chunks_max, chunk_size);
            }
            /* set node inactive which do not match total blocks */
            else if (total_blocks_max * plt_exp2(blob_client_ctx.block_size_log) < blob_size)
            {
                pnode->active = false;
                printw("blob_client_blob_init: inactive 0x%04x, total blocks %d is too small", total_blocks_max);
            }
            pnode = pnode->pnext;
        }

        if (pnode_min_chunk_size->active || chunk_size_suit)
        {
            /* choose suitable parameter */
            pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
            while (pnode)
            {
                if (!pnode->active)
                {
                    blob_recvs_node_delete_by_addr(pnode->addr);
                    printw("blob_client_blob_init: delete 0x%04x", pnode->addr);
                }
                pnode = pnode->pnext;
            }
            break;
        }
        else
        {
            /* delete node which contains min chunk size */
            blob_recvs_node_delete_by_addr(pnode_min_chunk_size->addr);
            pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
            while (pnode)
            {
                pnode->active = true;
                pnode = pnode->pnext;
            }
        }

        if (blob_client_ctx.blob_recvs_list.count == 0)
        {
            printe("blob_client_blob_init: fail, no node, %d", __LINE__);
            return false;
        }
    }

INIT:
    if (blob_client_ctx.block_size_log == 0 ||
        chunk_size == 0)
    {
        printe("blob_client_blob_init: wrong block size log %d or wrong chunk size %d",
               blob_client_ctx.block_size_log, chunk_size);
        return false;
    }
    blob_client_ctx.blob_size = blob_size;
    blob_client_ctx.blob_left_size = blob_size;
    blob_client_ctx.block_size = plt_exp2(blob_client_ctx.block_size_log);
    blob_client_ctx.total_blocks = BLOB_DIV_ROUND_UP(blob_client_ctx.blob_size,
                                                     blob_client_ctx.block_size);
    blob_client_ctx.chunk_size = chunk_size;
    if (blob_client_ctx.blob_client_cb)
    {
        blob_client_app_param_t cb_data = {0};
        cb_data.blob_size = blob_client_ctx.blob_size;
        cb_data.block_size = blob_client_ctx.block_size;
        cb_data.total_blocks = blob_client_ctx.total_blocks;
        cb_data.chunk_size = blob_client_ctx.chunk_size;
        blob_client_ctx.blob_client_cb(MESH_MSG_BLOB_CLIENT_APP_PARAM, &cb_data);
    }
    blob_client_ctx.pblock_data = plt_zalloc(blob_client_ctx.block_size, RAM_TYPE_DATA_ON);
    if (!blob_client_ctx.pblock_data)
    {
        printe("blob_client_blob_init: block data alloc failed");
        return false;
    }
    printi("blob_client_blob_init: blob size %d, block size log %d, total blocks %d, max chunk size %d, max total chunks %d, transfer node",
           blob_client_ctx.blob_size, blob_client_ctx.block_size_log, blob_client_ctx.total_blocks,
           blob_client_ctx.chunk_size, BLOB_DIV_ROUND_UP(blob_client_ctx.block_size,
                                                         blob_client_ctx.chunk_size));
    pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
    while (pnode)
    {
        printi("addr 0x%04x", pnode->addr);
        pnode = pnode->pnext;
    }
    return true;
}

void blob_client_transfer_ctx_clear(void)
{
    printi("blob_client_transfer_ctx_clear");
    blob_client_ctx.phase = BLOB_CLIENT_PHASE_IDLE;
    // blob_recvs_node_clear_all();
    blob_client_procedure_timer_stop();
    blob_client_retry_timer_stop();
    blob_transfer_client_set_send_cb(NULL);
    if (blob_client_ctx.pblock_data != NULL)
    {
        plt_free(blob_client_ctx.pblock_data, RAM_TYPE_DATA_ON);
        blob_client_ctx.pblock_data = NULL;
    }
    if (blob_client_ctx.sending_chunks != NULL)
    {
        plt_free(blob_client_ctx.sending_chunks, RAM_TYPE_DATA_ON);
        blob_client_ctx.sending_chunks = NULL;
    }
}

static bool blob_client_active_info_get_send(void)
{
    blob_recvs_caps_p pcaps = (blob_recvs_caps_p)blob_client_ctx.blob_caps_list.pfirst;
    blob_server_capabilities_t caps_zero = {0};
    while (pcaps)
    {
        if (memcmp(&pcaps->caps, &caps_zero, sizeof(pcaps->caps)) == 0)
        {
            blob_client_ctx.retry_count = 0;
            blob_client_retry_timer_start(BLOB_INFO_GET_RETRY_PERIOD);
            blob_client_ctx.pcur_recvs_caps = pcaps;
            printi("blob info get: addr 0x%04x, app key index %d",
                   pcaps->addr, blob_client_ctx.app_key_index);
            blob_info_get(pcaps->addr, blob_client_ctx.app_key_index);
            return true;
        }
        pcaps = pcaps->pnext;
    }
    printi("blob info get: no active node");
    return false;
}

bool blob_client_caps_retrieve(uint8_t app_key_index, uint8_t transfer_ttl, uint16_t *punicast_addr,
                               uint8_t unicast_addr_num)
{
    bool ret = true;
    uint16_t *paddr = punicast_addr;

    if (blob_client_ctx.phase != BLOB_CLIENT_PHASE_IDLE)
    {
        printe("blob_client_caps_retrieve: fail, wrong phase %d", blob_client_ctx.phase);
        return false;
    }
    blob_client_ctx.phase = BLOB_CLIENT_PHASE_RETRIEVE;

    if (ret && !blob_client_procedure_timer_start())
    {
        printe("blob_client_caps_retrieve: procedure timer start failed");
        ret = false;
    }

    if (ret && !blob_client_retry_timer_start(BLOB_INFO_GET_RETRY_PERIOD))
    {
        printe("blob_client_caps_retrieve: retry timer start failed");
        ret = false;
    }

    if (ret)
    {
        blob_client_ctx.app_key_index = app_key_index;
        blob_client_ctx.transfer_ttl = transfer_ttl;
        blob_recvs_caps_clear_all();

        while (unicast_addr_num--)
        {
            if (NULL == blob_recvs_caps_get_by_addr(*paddr))
            {
                blob_recvs_caps_add(*paddr);
            }
            paddr++;
        }
    }

    if (ret && !blob_client_active_info_get_send())
    {
        ret = false;
    }

    if (!ret)
    {
        blob_client_procedure_timer_stop();
        blob_client_retry_timer_stop();
    }

    return ret;
}

static bool blob_client_active_transfer_start_send(void)
{
    blob_recvs_node_p pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
    while (pnode)
    {
        if (pnode->active && pnode->node_phase == BLOB_NODE_PHASE_IDLE)
        {
            blob_client_ctx.retry_count = 0;
            blob_client_retry_timer_start(BLOB_TRANSFER_START_RETRY_PERIOD);
            pnode->node_phase = BLOB_NODE_PHASE_TRANSFER_STARTING;
            blob_client_ctx.pcur_recvs_node = pnode;
            printi("blob transfer start: addr 0x%04x, app key index %d, transfer mode %d, blob size %d, block size log %d, blob id",
                   pnode->addr, blob_client_ctx.app_key_index, blob_client_ctx.transfer_mode,
                   blob_client_ctx.blob_size, blob_client_ctx.block_size_log);
            dprinti(blob_client_ctx.blob_id, 8);
            blob_transfer_start(pnode->addr, blob_client_ctx.app_key_index,
                                blob_client_ctx.transfer_mode, blob_client_ctx.blob_id,
                                blob_client_ctx.blob_size, blob_client_ctx.block_size_log,
                                blob_client_ctx.client_mtu_size);
            return true;
        }
        pnode = pnode->pnext;
    }
    printi("blob transfer start: no active node");
    return false;
}

static bool blob_client_active_block_start_send(void)
{
    blob_recvs_node_p pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
    while (pnode)
    {
        if (pnode->active &&
            (pnode->node_phase == BLOB_NODE_PHASE_TRANSFER_STARTED ||
             pnode->node_phase == BLOB_NODE_PHASE_BLOCK_GETTED ||
             pnode->node_phase == BLOB_NODE_PHASE_CHUNK_TRANSFERRED))
        {
            blob_client_ctx.retry_count = 0;
            blob_client_retry_timer_start(BLOB_BLOCK_START_RETRY_PERIOD);
            pnode->node_phase = BLOB_NODE_PHASE_BLOCK_STARTING;
            blob_client_ctx.pcur_recvs_node = pnode;
            printi("blob block start: addr 0x%04x, app key index %d, block num %d, chunk size %d",
                   pnode->addr, blob_client_ctx.app_key_index, blob_client_ctx.block_num,
                   blob_client_ctx.chunk_size);
            blob_block_start(pnode->addr, blob_client_ctx.app_key_index,
                             blob_client_ctx.block_num, blob_client_ctx.chunk_size);
            return true;
        }
        pnode = pnode->pnext;
    }
    printi("blob block start: no active node");
    return false;
}

static bool blob_client_active_block_get_send(void)
{
    blob_recvs_node_p pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
    while (pnode)
    {
        if (pnode->active && pnode->node_phase == BLOB_NODE_PHASE_CHUNK_TRANSFERRING)
        {
            blob_client_ctx.retry_count = 0;
            blob_client_retry_timer_start(BLOB_BLOCK_GET_RETRY_PERIOD);
            pnode->node_phase = BLOB_NODE_PHASE_BLOCK_GETTING;
            blob_client_ctx.pcur_recvs_node = pnode;
            printi("blob block get: addr 0x%04x, app key index %d",
                   pnode->addr, blob_client_ctx.app_key_index);
            blob_block_get(pnode->addr, blob_client_ctx.app_key_index);
            return true;
        }
        pnode = pnode->pnext;
    }
    printi("blob block get: no active node");
    return false;
}

void blob_client_handle_transfer(bool ret)
{
    if (blob_client_ctx.blob_client_cb)
    {
        blob_client_app_transfer_t cb_data = {0};
        cb_data.procedure = BLOB_CB_PROCEDURE_TRANSFER;
        cb_data.type = ret ? BLOB_CB_TYPE_SUCCESS : BLOB_CB_TYPE_FAIL;
        cb_data.client_phase = blob_client_ctx.phase;
        blob_client_ctx.blob_client_cb(MESH_MSG_BLOB_CLIENT_APP_TRANSFER, &cb_data);
    }
    blob_client_transfer_ctx_clear();
}

static bool blob_client_active_transfer_cancel_send(void)
{
    blob_recvs_node_p pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
    while (pnode)
    {
        if (pnode->active &&
            pnode->node_phase != BLOB_NODE_PHASE_TRANSFER_CANCELLING &&
            pnode->node_phase != BLOB_NODE_PHASE_TRANSFER_CANCELLED)
        {
            blob_client_ctx.retry_count = 0;
            blob_client_retry_timer_start(BLOB_BLOCK_TRANSFER_CANCEL_PERIOD);
            pnode->node_phase = BLOB_NODE_PHASE_TRANSFER_CANCELLING;
            blob_client_ctx.pcur_recvs_node = pnode;
            printi("blob transfer cancel: addr 0x%04x, app key index %d",
                   pnode->addr, blob_client_ctx.app_key_index);
            blob_transfer_cancel(pnode->addr, blob_client_ctx.app_key_index, blob_client_ctx.blob_id);
            return true;
        }
        pnode = pnode->pnext;
    }
    printi("blob transfer cancel: no active node");
    return false;
}

static void blob_client_block_send(uint16_t block_num)
{
    if ((blob_client_ctx.transfer_mode == BLOB_TRANSFER_MODE_PUSH &&
         blob_client_ctx.phase != BLOB_CLIENT_PHASE_BLOB_TRANSFER_START &&
         blob_client_ctx.phase != BLOB_CLIENT_PHASE_BLOB_BLOCK_GET) ||
        (blob_client_ctx.transfer_mode == BLOB_TRANSFER_MODE_PULL &&
         blob_client_ctx.phase != BLOB_CLIENT_PHASE_BLOB_TRANSFER_START &&
         blob_client_ctx.phase != BLOB_CLIENT_PHASE_BLOB_CHUNK_TRANSFER))
    {
        printe("blob_client_block_send: fail, transfer mode %d, wrong phase %d",
               blob_client_ctx.transfer_mode, blob_client_ctx.phase);
        blob_client_handle_transfer(false);
        return;
    }

    blob_client_ctx.phase = BLOB_CLIENT_PHASE_BLOB_BLOCK_START;
    blob_client_ctx.block_num = block_num;
    blob_client_ctx.block_size = MIN(plt_exp2(blob_client_ctx.block_size_log),
                                     blob_client_ctx.blob_left_size);
    if (blob_client_ctx.blob_client_cb)
    {
        blob_client_app_block_load_t cb_data = {0};
        cb_data.offset = block_num * plt_exp2(blob_client_ctx.block_size_log);
        cb_data.block_size = blob_client_ctx.block_size;
        cb_data.pblock_data = blob_client_ctx.pblock_data;
        blob_client_ctx.blob_client_cb(MESH_MSG_BLOB_CLIENT_APP_BLOCK_LOAD, &cb_data);
    }
    else
    {
        printe("blob_client_block_send: block data get failed");
        blob_client_handle_transfer(false);
        return;
    }

    blob_client_procedure_timer_start();

    printi("blob_client_block_send: block start, block num %d, block size %d, block left size %d",
           blob_client_ctx.block_num, blob_client_ctx.block_size,
           blob_client_ctx.blob_left_size);

    /* block start */
    blob_client_ctx.current_total_chunks = BLOB_DIV_ROUND_UP(blob_client_ctx.block_size,
                                                             blob_client_ctx.chunk_size);
    if (!blob_client_active_block_start_send())
    {
        printe("blob_client_block_send: fail, block num %d", block_num);
        blob_client_handle_transfer(false);
    }
}

void blob_client_active_chunk_transfer(void)
{
    uint16_t chunk_num = blob_client_ctx.current_total_chunks;
    for (uint16_t i = 0; i < blob_client_ctx.current_total_chunks; ++i)
    {
        if (plt_bit_pool_get(blob_client_ctx.sending_chunks, i))
        {
            chunk_num = i;
            break;
        }
    }

    if (chunk_num == blob_client_ctx.current_total_chunks)
    {
        printi("blob_client_active_chunk_transfer: no more chunks to transfer");
        if (blob_client_ctx.transfer_mode == BLOB_TRANSFER_MODE_PUSH)
        {
            blob_client_ctx.phase = BLOB_CLIENT_PHASE_BLOB_BLOCK_GET;
            blob_client_procedure_timer_start();
            if (!blob_client_active_block_get_send())
            {
                printe("blob_client_active_chunk_transfer: no node in list");
                blob_client_handle_transfer(false);
            }
        }
        else if (blob_client_ctx.transfer_mode == BLOB_TRANSFER_MODE_PULL)
        {
            blob_client_retry_timer_start(BLOB_BLOCK_REPORT_PERIOD);
            if (!blob_client_procedure_timer_is_active())
            {
                blob_client_procedure_timer_start();
            }
            printi("blob_client_active_chunk_transfer: wait partial block report until block report timer expires or client timer expires");
        }
    }
    else
    {
        /* set chunk data */
        blob_client_ctx.chunk_num = chunk_num;
        if (chunk_num == blob_client_ctx.current_total_chunks - 1)
        {
            blob_client_ctx.current_chunk_size = blob_client_ctx.block_size - chunk_num *
                                                 blob_client_ctx.chunk_size;
        }
        else
        {
            blob_client_ctx.current_chunk_size = blob_client_ctx.chunk_size;
        }
        blob_client_ctx.pchunk_data = blob_client_ctx.pblock_data + chunk_num * blob_client_ctx.chunk_size;
        printi("blob_client_active_chunk_transfer: dst 0x%04x, block num %d/%d, chunk num %d/%d, chunk size %d",
               blob_client_ctx.multicast_addr, blob_client_ctx.block_num, blob_client_ctx.total_blocks,
               blob_client_ctx.chunk_num, blob_client_ctx.current_total_chunks,
               blob_client_ctx.current_chunk_size);
        blob_recvs_node_p pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
        uint16_t dst = MESH_IS_SUBSCRIBE_ADDR(blob_client_ctx.multicast_addr) ?
                       blob_client_ctx.multicast_addr : pnode->addr;
        if (MESH_MSG_SEND_CAUSE_SUCCESS == blob_chunk_transfer(dst, blob_client_ctx.app_key_index,
                                                               blob_client_ctx.chunk_num, blob_client_ctx.pchunk_data,
                                                               blob_client_ctx.current_chunk_size))
        {
            plt_bit_pool_set(blob_client_ctx.sending_chunks, blob_client_ctx.chunk_num, 0);
        }
        else
        {
            printe("blob_client_active_chunk_transfer: chunk send failed");
            blob_client_handle_transfer(false);
        }
    }
}

bool blob_client_blob_transfer(uint16_t multicast_addr, uint8_t app_key_index, uint8_t transfer_ttl,
                               uint16_t *punicast_addr, uint8_t unicast_addr_num, uint8_t blob_id[8],
                               uint32_t blob_size, blob_transfer_mode_t transfer_mode,
                               uint8_t client_timeout_base, bool skip_caps_retrieve)
{
    bool ret = true;

    if (blob_client_ctx.phase != BLOB_CLIENT_PHASE_IDLE)
    {
        printe("blob_client_blob_transfer: fail, wrong phase %d", blob_client_ctx.phase);
        return false;
    }

    blob_recvs_node_clear_all();

    uint16_t *paddr = punicast_addr;
    blob_recvs_caps_p pcaps = NULL;

    while (unicast_addr_num--)
    {
        pcaps = blob_recvs_caps_get_by_addr(*paddr);
        if (!blob_recvs_node_get_by_addr(*paddr) &&
            (pcaps || skip_caps_retrieve))
        {
            blob_recvs_node_add(*paddr);
        }
        paddr++;
    }

    if (ret && blob_client_ctx.blob_recvs_list.count == 0)
    {
        printe("blob_client_blob_transfer: fail, no node in receive list");
        ret = false;
    }

    blob_recvs_node_p pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
    while (pnode)
    {
        pnode->active = true;
        pnode = pnode->pnext;
    }

    if (ret && blob_client_blob_init(blob_size, transfer_mode, skip_caps_retrieve))
    {
        blob_client_ctx.multicast_addr = multicast_addr;
        blob_client_ctx.app_key_index = app_key_index;
        blob_client_ctx.transfer_ttl = transfer_ttl;
        blob_client_ctx.transfer_mode = transfer_mode;
        blob_client_ctx.client_timeout_base = client_timeout_base;
        memcpy(blob_client_ctx.blob_id, blob_id, 8);
    }
    else
    {
        ret = false;
    }

    if (ret && !blob_client_procedure_timer_start())
    {
        printe("blob_client_blob_transfer: procedure timer start failed");
        ret = false;
    }

    if (ret && !blob_client_retry_timer_start(BLOB_TRANSFER_START_RETRY_PERIOD))
    {
        printe("blob_client_blob_transfer: retry timer start failed");
        ret = false;
    }

    if (ret)
    {
        pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
        while (pnode)
        {
            pnode->blocks_not_recv_len = blob_client_ctx.total_blocks;
            pnode->blocks_not_recv = plt_zalloc(pnode->blocks_not_recv_len * sizeof(uint16_t),
                                                RAM_TYPE_DATA_ON);
            for (uint16_t i = 0; i < pnode->blocks_not_recv_len; i++)
            {
                pnode->blocks_not_recv[i] = pnode->blocks_not_recv_len - i - 1;
            }
            pnode = pnode->pnext;
        }
        blob_client_ctx.phase = BLOB_CLIENT_PHASE_BLOB_TRANSFER_START;
        blob_client_active_transfer_start_send();
    }
    else
    {
        blob_client_transfer_ctx_clear();
    }

    return ret;
}

void blob_client_send_cb(mesh_model_info_p pmodel_info, mesh_msg_send_stat_t stat,
                         uint32_t access_opcode)
{
    uint8_t event = EVENT_IO_TO_APP;
    T_IO_MSG msg;
    msg.type = BLOB_CLIENT_CHUNK_TRANSFER;

    if (pmodel_info->model_id != MESH_MODEL_BLOB_TRANSFER_CLIENT)
    {
        printe("blob_client_send_cb: wrong model id 0x%08x", pmodel_info->model_id);
        return;
    }

    if (access_opcode == MESH_MSG_BLOB_CHUNK_TRANSFER)
    {
        if (stat == MESH_MSG_SEND_STAT_SENT ||
            stat == MESH_MSG_SEND_STAT_ACKED ||
            stat == MESH_MSG_SEND_STAT_ACKED_OBO)
        {
            printi("blob_client_send_cb: success");
        }
        /* retrans in app, so can send next packet, even if this one failed */
        else if (stat == MESH_MSG_SEND_STAT_TIMEOUT)
        {
            if (MESH_IS_SUBSCRIBE_ADDR(blob_client_ctx.multicast_addr))
            {
                printi("blob_client_send_cb: seg msg send success");
            }
            else if (MESH_IS_UNICAST_ADDR(blob_client_ctx.multicast_addr))
            {
                printi("blob_client_send_cb: may fail but this chunk will be retrans in app layer");
            }
        }
        else
        {
            printe("blob_client_send_cb: send fail, block num %d/%d, chunk num %d/%d",
                   blob_client_ctx.block_num, blob_client_ctx.total_blocks,
                   blob_client_ctx.chunk_num, blob_client_ctx.current_total_chunks);
            /* stop blob data send procedure ? */
        }

        if (os_msg_send(io_queue_handle, &msg, 0) == false)
        {
            printe("blob_client_send_cb: send io queue fail");
            blob_client_handle_transfer(false);
        }
        else if (os_msg_send(evt_queue_handle, &event, 0) == false)
        {
            printe("blob_client_send_cb: send evt queue fail");
            blob_client_handle_transfer(false);
        }
    }

    return;
}

void blob_client_missing_chunks_update(blob_node_phase_t node_phase)
{
    blob_recvs_node_p pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;

    while (pnode)
    {
        printw("blob_client_missing_chunks_update: src 0x%04x, phase %d, len %d", pnode->addr,
               pnode->node_phase, pnode->current_missing_chunks_len);
        if (pnode->active && pnode->node_phase == node_phase)
        {
            if (pnode->current_missing_chunks_len == blob_client_ctx.current_total_chunks)
            {
                /* missing all chunk */
                for (uint16_t i = 0; i < blob_client_ctx.current_total_chunks; ++i)
                {
                    plt_bit_pool_set(blob_client_ctx.sending_chunks, i, 1);
                }
                break;
            }
            else
            {
                for (uint16_t i = 0; i < pnode->current_missing_chunks_len; ++i)
                {
                    plt_bit_pool_set(blob_client_ctx.sending_chunks, pnode->pmissing_chunks[i], 1);
                }
            }
        }
        pnode = pnode->pnext;
    }
}

uint32_t blob_client_missing_chunks_num_get(void)
{
    return plt_bit_pool_count(blob_client_ctx.sending_chunks,
                              blob_client_ctx.current_sending_chunks_len);
}

void blob_client_data_send(void)
{
    bool need_chunk_transfer = false;
    blob_recvs_node_p pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;

    if (blob_client_ctx.phase != BLOB_CLIENT_PHASE_BLOB_BLOCK_START)
    {
        printe("blob_client_data_send: wrong phase %d");
        blob_client_handle_transfer(false);
        return;
    }
    blob_client_ctx.phase = BLOB_CLIENT_PHASE_BLOB_CHUNK_TRANSFER;

    /* check chunk transfer */
    while (pnode)
    {
        if (pnode->active && pnode->node_phase == BLOB_NODE_PHASE_BLOCK_STARTED)
        {
            pnode->node_phase = BLOB_NODE_PHASE_CHUNK_TRANSFERRING;
            need_chunk_transfer = true;
        }
        pnode = pnode->pnext;
    }

    if (!need_chunk_transfer)
    {
        /* no node need to update */
        printi("blob_client_data_send: no node need chunk, transfer procedure failed");
        blob_client_handle_transfer(false);
        return;
    }

    /* all node received blob block start message, start chunk transfer */
    blob_client_ctx.current_total_chunks = BLOB_DIV_ROUND_UP(blob_client_ctx.block_size,
                                                             blob_client_ctx.chunk_size);
    blob_client_ctx.current_sending_chunks_len = (blob_client_ctx.current_total_chunks % 8) ?
                                                 (blob_client_ctx.current_total_chunks / 8 + 1) :
                                                 (blob_client_ctx.current_total_chunks / 8);
    if (blob_client_ctx.sending_chunks)
    {
        plt_free(blob_client_ctx.sending_chunks, RAM_TYPE_DATA_ON);
        blob_client_ctx.sending_chunks = NULL;
    }
    blob_client_ctx.sending_chunks = plt_zalloc(blob_client_ctx.current_sending_chunks_len,
                                                RAM_TYPE_DATA_ON);
    if (!blob_client_ctx.sending_chunks)
    {
        printe("blob_client_data_send: sending chunks alloc failed");
        blob_client_handle_transfer(false);
        return;
    }

    blob_client_retry_timer_stop();
    blob_client_procedure_timer_stop();

    blob_transfer_client_set_send_cb(blob_client_send_cb);

    blob_client_missing_chunks_update(BLOB_NODE_PHASE_CHUNK_TRANSFERRING);

    if (blob_client_missing_chunks_num_get())
    {
        blob_client_active_chunk_transfer();
    }
    else
    {
        printe("blob_client_data_send: no missing chunks need to send");
        blob_client_handle_transfer(false);
    }
}

// TODO:
void blob_client_transfer_status_determine(uint16_t multicast_addr, uint8_t app_key_index,
                                           uint8_t transfer_ttl, uint16_t *punicast_addr, uint8_t unicast_addr_num)
{
    return;
}

bool blob_client_transfer_cancel(uint16_t *punicast_addr, uint8_t unicast_addr_num,
                                 uint8_t blob_id[8])
{
    if (memcmp(blob_id, blob_client_ctx.blob_id, 8) != 0)
    {
        printe("blob_client_transfer_cancel: wrong blob id");
        dprinte(blob_id, 8);
        dprinte(blob_client_ctx.blob_id, 8);
        return false;
    }

    if (blob_client_ctx.blob_recvs_list.count == 0)
    {
        printe("blob_client_transfer_cancel: no node in list");
        return true;
    }

    if (!blob_client_procedure_timer_start())
    {
        printe("blob_client_transfer_cancel: start procedure timer fail");
        return false;
    }

    blob_client_ctx.phase = BLOB_CLIENT_PHASE_BLOB_TRANSFER_CANCEL;
    blob_transfer_client_set_send_cb(NULL);
    // memset(pnode->pmissing_chunks, 0, blob_client_ctx.current_total_chunks * sizeof(uint16_t));

    if (MESH_IS_SUBSCRIBE_ADDR(blob_client_ctx.multicast_addr))
    {
        blob_client_ctx.retry_count = 0;
        blob_client_retry_timer_start(BLOB_BLOCK_TRANSFER_CANCEL_PERIOD);
        blob_transfer_cancel(blob_client_ctx.multicast_addr, blob_client_ctx.app_key_index, blob_id);
    }
    else
    {
        if (!blob_client_active_transfer_cancel_send())
        {
            printi("blob_client_transfer_cancel: no active node");
        }
    }

    return true;
}

void blob_client_handle_block_complete(void)
{
    blob_recvs_node_p pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
    while (pnode)
    {
        if (pnode->active)
        {
            pnode->blocks_not_recv_len--;
        }
        pnode = pnode->pnext;
    }
    printi("blob_client_handle_block_complete: block %d/%d transfer complete",
           blob_client_ctx.block_num, blob_client_ctx.total_blocks);
    blob_client_ctx.blob_left_size -= blob_client_ctx.block_size;

    if (blob_client_ctx.blob_client_cb)
    {
        blob_client_app_transfer_t cb_data = {0};
        cb_data.procedure = BLOB_CB_PROCEDURE_TRANSFER;
        cb_data.type = BLOB_CB_TYPE_PROGRESS;
        cb_data.client_phase = blob_client_ctx.phase;
        cb_data.progress = (blob_client_ctx.blob_size - blob_client_ctx.blob_left_size) * 100 /
                           blob_client_ctx.blob_size;
        blob_client_ctx.blob_client_cb(MESH_MSG_BLOB_CLIENT_APP_TRANSFER, &cb_data);
    }

    if (blob_client_ctx.blob_left_size == 0)
    {
        printi("blob_client_handle_block_complete: blob transfer complete");
        blob_client_handle_transfer(true);
    }
    else
    {
        blob_client_ctx.block_num ++;
        blob_client_block_send(blob_client_ctx.block_num);
    }
}

void blob_client_handle_missing_chunks(void)
{
    if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_BLOCK_GET)
    {
        blob_client_missing_chunks_update(BLOB_NODE_PHASE_BLOCK_GETTED);
        if (blob_client_missing_chunks_num_get())
        {
            blob_client_ctx.phase = BLOB_CLIENT_PHASE_BLOB_CHUNK_TRANSFER;
            blob_recvs_node_p pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
            while (pnode)
            {
                if (pnode->active && pnode->node_phase == BLOB_NODE_PHASE_BLOCK_GETTED)
                {
                    pnode->node_phase = BLOB_NODE_PHASE_CHUNK_TRANSFERRING;
                }
                pnode = pnode->pnext;
            }
            /* continue chunk transfer */
            blob_client_active_chunk_transfer();
        }
        else
        {
            /* all node receive this block */
            blob_client_handle_block_complete();
        }
    }
    else if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_CHUNK_TRANSFER)
    {
        blob_client_missing_chunks_update(BLOB_NODE_PHASE_CHUNK_TRANSFERRING);
        if (blob_client_missing_chunks_num_get())
        {
            blob_client_active_chunk_transfer();
        }
    }
}

void blob_client_handle_partial_report_complete(void)
{
    blob_recvs_node_p pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;

    if (blob_client_ctx.phase != BLOB_CLIENT_PHASE_BLOB_CHUNK_TRANSFER)
    {
        printe("blob_client_handle_partial_report_complete: wrong phase %d", blob_client_ctx.phase);
        blob_client_handle_transfer(false);
        return;
    }

    while (pnode)
    {
        if (pnode->active && pnode->node_phase != BLOB_NODE_PHASE_CHUNK_TRANSFERRED)
        {
            printi("blob_client_handle_partial_report_complete: 0x%04x not report complete",
                   pnode->addr);
            return;
        }
        pnode = pnode->pnext;
    }

    /* all active node report complete */
    blob_client_handle_block_complete();

    return;
}

static void blob_client_handle_node_fail(blob_cb_procedure_t procedure, uint16_t addr)
{
    printe("blob_client_handle_node_fail: node 0x%04x failed in procedure %d", addr, procedure);
    blob_client_app_transfer_t cb_data = {0};
    cb_data.procedure = procedure;
    cb_data.type = BLOB_CB_TYPE_NODE_FAIL;
    cb_data.client_phase = blob_client_ctx.phase;
    cb_data.addr = addr;
    if (procedure == BLOB_CB_PROCEDURE_CAPS_RETRIEVE)
    {
        blob_recvs_caps_p pcaps = blob_recvs_caps_get_by_addr(addr);
        if (pcaps)
        {
            if (blob_client_ctx.blob_client_cb)
            {
                blob_client_ctx.blob_client_cb(MESH_MSG_BLOB_CLIENT_APP_TRANSFER, &cb_data);
            }
            blob_recvs_caps_delete_by_addr(addr);
        }
    }
    else if (procedure == BLOB_CB_PROCEDURE_TRANSFER ||
             procedure == BLOB_CB_PROCEDURE_CANCEL)
    {
        blob_recvs_node_p pnode = blob_recvs_node_get_by_addr(addr);
        if (pnode)
        {
            if (blob_client_ctx.blob_client_cb)
            {
                blob_client_ctx.blob_client_cb(MESH_MSG_BLOB_CLIENT_APP_TRANSFER, &cb_data);
            }
            pnode->active = false;
            pnode->node_phase = BLOB_NODE_PHASE_FAILED;
        }
    }
}

static void blob_client_handle_caps_retrieve(void)
{
    bool ret = false;
    blob_recvs_caps_p pcaps = (blob_recvs_caps_p)blob_client_ctx.blob_caps_list.pfirst;
    blob_server_capabilities_t caps_zero = {0};
    while (pcaps)
    {
        if (memcmp(&pcaps->caps, &caps_zero, sizeof(pcaps->caps)) != 0)
        {
            ret = true;
            break;
        }
        pcaps = pcaps->pnext;
    }

    blob_client_transfer_ctx_clear();
    printi("blob_client_handle_caps_retrieve: caps retrieve complete %d", ret);

    if (blob_client_ctx.blob_client_cb)
    {
        blob_client_app_transfer_t cb_data = {0};
        cb_data.procedure = BLOB_CB_PROCEDURE_CAPS_RETRIEVE;
        cb_data.type = ret ? BLOB_CB_TYPE_SUCCESS : BLOB_CB_TYPE_FAIL;
        cb_data.client_phase = blob_client_ctx.phase;
        blob_client_ctx.blob_client_cb(MESH_MSG_BLOB_CLIENT_APP_TRANSFER, &cb_data);
    }
}

static void blob_client_handle_transfer_cancel(void)
{
    bool ret = false;
    blob_recvs_node_p pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
    while (pnode)
    {
        if (pnode->active && pnode->node_phase == BLOB_NODE_PHASE_TRANSFER_CANCELLED)
        {
            ret = true;
            break;
        }
        pnode = pnode->pnext;
    }

    if (blob_client_ctx.blob_client_cb)
    {
        blob_client_app_transfer_t cb_data = {0};
        cb_data.procedure = BLOB_CB_PROCEDURE_CANCEL;
        cb_data.type = ret ? BLOB_CB_TYPE_SUCCESS : BLOB_CB_TYPE_FAIL;
        cb_data.client_phase = blob_client_ctx.phase;
        blob_client_ctx.blob_client_cb(MESH_MSG_BLOB_CLIENT_APP_TRANSFER, &cb_data);
    }
    printi("blob_client_handle_transfer_cancel: transfer cancel complete");
    blob_client_transfer_ctx_clear();
}

void blob_client_handle_procedure_timeout(void)
{
    printi("blob_client_handle_procedure_timeout");
    blob_recvs_node_p pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
    if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_RETRIEVE)
    {
        blob_client_handle_caps_retrieve();
    }
    else if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_TRANSFER_START)
    {
        while (pnode)
        {
            if (pnode->active && pnode->node_phase == BLOB_NODE_PHASE_TRANSFER_STARTED)
            {
                printi("blob_client_handle_procedure_timeout: transfer start procedure success");
                /* there is a node received blob transfer start message, begin blob block start */
                blob_client_block_send(0);
                return;
            }
            pnode = pnode->pnext;
        }
        printi("blob_client_handle_procedure_timeout: transfer start procedure fail");
    }
    else if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_BLOCK_START)
    {
        while (pnode)
        {
            if (pnode->active && pnode->node_phase == BLOB_NODE_PHASE_BLOCK_STARTED)
            {
                printi("blob_client_handle_procedure_timeout: block start procedure success");
                /* there is a node received blob block start message, start chunk transfer */
                blob_client_data_send();
                return;
            }
            pnode = pnode->pnext;
        }
        printi("blob_client_handle_procedure_timeout: block start procedure fail");
    }
    else if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_CHUNK_TRANSFER)
    {
        if (blob_client_ctx.transfer_mode == BLOB_TRANSFER_MODE_PULL)
        {
            /* delete node which not report complete */
            pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
            while (pnode)
            {
                if (pnode->active && pnode->node_phase != BLOB_NODE_PHASE_CHUNK_TRANSFERRED)
                {
                    printe("blob_client_handle_procedure_timeout: 0x%04x transfer fail");
                    blob_client_handle_node_fail(BLOB_CB_PROCEDURE_TRANSFER, pnode->addr);
                }
                pnode = pnode->pnext;
            }
            blob_client_handle_partial_report_complete();
        }
    }
    else if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_BLOCK_GET)
    {
        while (pnode)
        {
            if (pnode->active && pnode->node_phase == BLOB_NODE_PHASE_BLOCK_GETTED)
            {
                printi("blob_client_handle_procedure_timeout: block get procedure success");
                /* there is a node received blob block get message, calculate chunks which need retrans */
                blob_client_handle_missing_chunks();
                return;
            }
            pnode = pnode->pnext;
        }
        printi("blob_client_handle_procedure_timeout: block get procedure fail");
    }
    else if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_TRANSFER_CANCEL)
    {
        blob_client_handle_transfer_cancel();
    }
}

void blob_client_handle_retry_timeout(void)
{
    printi("blob_client_handle_retry_timeout: client phase %d", blob_client_ctx.phase);
    blob_recvs_node_p pnode = blob_client_ctx.pcur_recvs_node;
    if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_RETRIEVE)
    {
        blob_recvs_caps_p pcaps = blob_client_ctx.pcur_recvs_caps;
        blob_server_capabilities_t caps_zero = {0};
        if (memcmp(&pcaps->caps, &caps_zero, sizeof(pcaps->caps)) == 0)
        {
            if (blob_client_ctx.retry_count >= BLOB_RETRY_TIMES)
            {
                /* info get failed, delete this node */
                printe("blob_client_handle_retry_timeout: node 0x%04x blob info get failed",
                       pcaps->addr);
                blob_client_handle_node_fail(BLOB_CB_PROCEDURE_CAPS_RETRIEVE, pcaps->addr);
            }
            else
            {
                blob_client_ctx.retry_count ++;
                printi("blob_client_handle_retry_timeout: blob info get, dst 0x%04x, retry count %d",
                       pcaps->addr, blob_client_ctx.retry_count);
                blob_info_get(pcaps->addr, blob_client_ctx.app_key_index);
                return;
            }
        }

        if (blob_client_active_info_get_send())
        {
            return;
        }

        blob_client_handle_caps_retrieve();
    }
    else if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_TRANSFER_START)
    {
        if (pnode->active && pnode->node_phase == BLOB_NODE_PHASE_TRANSFER_STARTING)
        {
            if (blob_client_ctx.retry_count >= BLOB_RETRY_TIMES)
            {
                /* transfer start failed, skip this node */
                printe("blob_client_handle_retry_timeout: node 0x%04x blob transfer start failed",
                       pnode->addr);
                blob_client_handle_node_fail(BLOB_CB_PROCEDURE_TRANSFER, pnode->addr);
            }
            else
            {
                blob_client_ctx.retry_count ++;
                printi("blob_client_handle_retry_timeout: blob transfer start, dst 0x%04x, retry count %d",
                       pnode->addr, blob_client_ctx.retry_count);
                blob_transfer_start(pnode->addr, blob_client_ctx.app_key_index,
                                    blob_client_ctx.transfer_mode, blob_client_ctx.blob_id,
                                    blob_client_ctx.blob_size, blob_client_ctx.block_size_log,
                                    blob_client_ctx.client_mtu_size);
                return;
            }
        }

        if (blob_client_active_transfer_start_send())
        {
            return;
        }

        /* Send Block sub-procedure */
        blob_client_block_send(0);
    }
    else if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_BLOCK_START)
    {
        if (pnode->active && pnode->node_phase == BLOB_NODE_PHASE_BLOCK_STARTING)
        {
            if (blob_client_ctx.retry_count >= BLOB_RETRY_TIMES)
            {
                /* transfer start failed, skip this node */
                printe("blob_client_handle_retry_timeout: node 0x%04x blob block start failed",
                       pnode->addr);
                blob_client_handle_node_fail(BLOB_CB_PROCEDURE_TRANSFER, pnode->addr);
            }
            else
            {
                blob_client_ctx.retry_count ++;
                printi("blob_client_handle_retry_timeout: blob block start, dst 0x%04x, retry count %d",
                       pnode->addr, blob_client_ctx.retry_count);
                blob_block_start(pnode->addr, blob_client_ctx.app_key_index,
                                 blob_client_ctx.block_num, blob_client_ctx.chunk_size);
                return;
            }
        }

        if (blob_client_active_block_start_send())
        {
            return;
        }

        /* all node received blob block start message, start chunk transfer */
        blob_client_data_send();
    }
    else if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_CHUNK_TRANSFER)
    {
        if (blob_client_ctx.transfer_mode == BLOB_TRANSFER_MODE_PULL)
        {
            blob_client_handle_missing_chunks();
        }
    }
    else if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_BLOCK_GET)
    {
        if (pnode->active && pnode->node_phase == BLOB_NODE_PHASE_BLOCK_GETTING)
        {
            if (blob_client_ctx.retry_count >= BLOB_RETRY_TIMES)
            {
                /* transfer start failed, skip this node */
                printe("blob_client_handle_retry_timeout: node 0x%04x blob block get failed",
                       pnode->addr);
                blob_client_handle_node_fail(BLOB_CB_PROCEDURE_TRANSFER, pnode->addr);
            }
            else
            {
                blob_client_ctx.retry_count ++;
                printi("blob_client_handle_retry_timeout: blob block get, dst 0x%04x, retry count %d",
                       pnode->addr, blob_client_ctx.retry_count);
                blob_block_get(pnode->addr, blob_client_ctx.app_key_index);
                return;
            }
        }

        if (blob_client_active_block_get_send())
        {
            return;
        }

        /* all node received blob block get message, calculate chunks which need retrans */
        blob_client_handle_missing_chunks();
    }
    else if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_TRANSFER_CANCEL)
    {
        if (MESH_IS_SUBSCRIBE_ADDR(blob_client_ctx.multicast_addr))
        {
            if (blob_client_ctx.retry_count >= BLOB_RETRY_TIMES)
            {
                pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
                while (pnode)
                {
                    if (pnode->active && pnode->node_phase == BLOB_NODE_PHASE_TRANSFER_CANCELLING)
                    {
                        /* transfer cancel failed, inactive this node */
                        printe("blob_client_handle_retry_timeout: node 0x%04x blob transfer cancel failed",
                               pnode->addr);
                        blob_client_handle_node_fail(BLOB_CB_PROCEDURE_CANCEL, pnode->addr);
                    }
                    pnode = pnode->pnext;
                }
            }
            else
            {
                pnode = (blob_recvs_node_p)blob_client_ctx.blob_recvs_list.pfirst;
                while (pnode)
                {
                    if (pnode->active && pnode->node_phase == BLOB_NODE_PHASE_TRANSFER_CANCELLING)
                    {
                        blob_client_ctx.retry_count ++;
                        printi("blob_client_handle_retry_timeout: blob transfer cancel, dst 0x%04x, retry count %d",
                               blob_client_ctx.multicast_addr, blob_client_ctx.retry_count);
                        uint16_t dst = MESH_IS_SUBSCRIBE_ADDR(blob_client_ctx.multicast_addr) ?
                                       blob_client_ctx.multicast_addr : pnode->addr;
                        blob_transfer_cancel(dst, blob_client_ctx.app_key_index,
                                             blob_client_ctx.blob_id);
                        return;
                    }
                    pnode = pnode->pnext;
                }
            }
        }
        else
        {
            if (pnode->active && pnode->node_phase == BLOB_NODE_PHASE_TRANSFER_CANCELLING)
            {
                if (blob_client_ctx.retry_count >= BLOB_RETRY_TIMES)
                {
                    /* transfer cancel failed, skip this node */
                    printe("blob_client_handle_retry_timeout: node 0x%04x blob transfer cancel failed",
                           pnode->addr);
                    blob_client_handle_node_fail(BLOB_CB_PROCEDURE_CANCEL, pnode->addr);
                }
                else
                {
                    blob_client_ctx.retry_count ++;
                    printi("blob_client_handle_retry_timeout: blob transfer cancel, dst 0x%04x, retry count %d",
                           pnode->addr, blob_client_ctx.retry_count);
                    blob_transfer_cancel(pnode->addr, blob_client_ctx.app_key_index, blob_client_ctx.blob_id);
                    return;
                }
            }

            if (blob_client_active_transfer_cancel_send())
            {
                return;
            }
        }

        /* all node transfer cancelled */
        blob_client_handle_transfer_cancel();
    }
}

int32_t blob_client_app_handle_data(const mesh_model_info_p pmodel_info, uint32_t type, void *pargs)
{
    blob_recvs_node_p pnode = NULL;
    switch (type)
    {
    case BLOB_TRANSFER_CLIENT_TRANSFER_STATUS:
        {
            blob_transfer_client_transfer_status_t *pdata = (blob_transfer_client_transfer_status_t *)pargs;
            pnode = blob_recvs_node_get_by_addr(pdata->src);
            if (!pnode)
            {
                printe("blob transfer status: addr 0x%04x not in recvs list", pdata->src);
                return MODEL_SUCCESS;
            }

            blob_client_retry_timer_stop();
            pnode->transfer_status = pdata->status;
            pnode->retrieved_transfer_phase = pdata->transfer_phase;
            if (pdata->status != BLOB_TRANSFER_STATUS_SUCCESS)
            {
                printe("blob transfer status: node 0x%04x transfer status failed, reason %d", pdata->src,
                       pdata->status);
                blob_client_handle_node_fail(BLOB_CB_PROCEDURE_TRANSFER, pnode->addr);
            }
            else
            {
                printi("blob transfer status: node 0x%04x transfer status success", pdata->src);
                if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_TRANSFER_START)
                {
                    pnode->node_phase = BLOB_NODE_PHASE_TRANSFER_STARTED;
                }
                else if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_TRANSFER_CANCEL)
                {
                    pnode->node_phase = BLOB_NODE_PHASE_TRANSFER_CANCELLED;
                }
                else
                {
                    printe("blob transfer status: wrong phase %d", blob_client_ctx.phase);
                }
            }

            // move on
            if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_TRANSFER_START)
            {
                /* find active node and send blob transfer start */
                if (blob_client_active_transfer_start_send())
                {
                    return MODEL_SUCCESS;
                }

                /* all node received blob transfer start message, begin blob block start */
                blob_client_block_send(0);
            }
            else if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_TRANSFER_CANCEL)
            {
                /* find active node and send blob transfer cancel */
                if (blob_client_active_transfer_cancel_send())
                {
                    return MODEL_SUCCESS;
                }

                /* all node transfer cancelled */
                blob_client_handle_transfer_cancel();
            }
        }
        break;
    case BLOB_TRANSFER_CLIENT_BLOCK_STATUS:
        {
            blob_transfer_client_block_status_t *pdata = (blob_transfer_client_block_status_t *)pargs;
            pnode = blob_recvs_node_get_by_addr(pdata->src);
            if (!pnode)
            {
                printe("blob block status: addr 0x%04x not in recvs list", pdata->src);
                return MODEL_SUCCESS;
            }

            blob_client_retry_timer_stop();
            pnode->transfer_status = pdata->status;
            if (pdata->status != BLOB_TRANSFER_STATUS_SUCCESS)
            {
                printe("blob block status: node 0x%04x block status failed, reason %d", pdata->src,
                       pdata->status);
                blob_client_handle_node_fail(BLOB_CB_PROCEDURE_TRANSFER, pnode->addr);
            }
            else
            {
                printi("blob block status: node 0x%04x block status success", pdata->src);
                if ((blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_BLOCK_START &&
                     pnode->node_phase == BLOB_NODE_PHASE_BLOCK_STARTING) ||
                    (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_BLOCK_GET &&
                     pnode->node_phase == BLOB_NODE_PHASE_BLOCK_GETTING))
                {
                    if (pnode->pmissing_chunks == NULL)
                    {
                        pnode->pmissing_chunks = plt_zalloc(blob_client_ctx.current_total_chunks * sizeof(uint16_t),
                                                            RAM_TYPE_DATA_ON);
                    }
                    switch (pdata->missing_format)
                    {
                    case BLOB_CHUNK_MISSING_FORMAT_ALL:
                        {
                            pnode->current_missing_chunks_len = blob_client_ctx.current_total_chunks;
                            printi("blob block status: 0x%04x missing all chunk", pdata->src);
                        }
                        break;
                    case BLOB_CHUNK_MISSING_FORMAT_NONE:
                        {
                            pnode->current_missing_chunks_len = 0;
                            memset((uint8_t *)pnode->pmissing_chunks, 0,
                                   blob_client_ctx.current_total_chunks * sizeof(uint16_t));
                            printi("blob block status: 0x%04x missing none chunk", pdata->src);
                        }
                        break;
                    case BLOB_CHUNK_MISSING_FORMAT_SOME:
                        {
                            pnode->current_missing_chunks_len = pdata->missing_chunks_len;
                            memcpy((uint8_t *)pnode->pmissing_chunks, (uint8_t *)pdata->pmissing_chunks,
                                   pdata->missing_chunks_len * sizeof(uint16_t));
                            printi("blob block status: 0x%04x missing chunks", pdata->src);
                            dprinti((uint8_t *)pnode->pmissing_chunks, pnode->current_missing_chunks_len * sizeof(uint16_t));
                        }
                        break;
                    default:
                        break;
                    }
                }

                if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_BLOCK_START)
                {
                    if (pnode->node_phase == BLOB_NODE_PHASE_BLOCK_STARTING)
                    {
                        pnode->node_phase = BLOB_NODE_PHASE_BLOCK_STARTED;
                    }
                    else
                    {
                        printe("blob block status: wrong node phase %d", pnode->node_phase);
                    }
                }
                else if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_BLOCK_GET)
                {
                    if (pnode->node_phase == BLOB_NODE_PHASE_BLOCK_GETTING)
                    {
                        pnode->node_phase = BLOB_NODE_PHASE_BLOCK_GETTED;
                    }
                    else
                    {
                        printe("blob block status: wrong node phase %d", pnode->node_phase);
                    }
                }
                else
                {
                    printe("blob block status: wrong phase %d", blob_client_ctx.phase);
                }
            }

            // move on
            if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_BLOCK_START)
            {
                if (blob_client_active_block_start_send())
                {
                    return MODEL_SUCCESS;
                }

                /* all node received blob block start message, start chunk transfer */
                blob_client_data_send();
            }
            else if (blob_client_ctx.phase == BLOB_CLIENT_PHASE_BLOB_BLOCK_GET)
            {
                if (blob_client_active_block_get_send())
                {
                    return MODEL_SUCCESS;
                }

                /* all node received blob block get message, calculate chunks which need retrans */
                blob_client_handle_missing_chunks();
            }

        }
        break;
    case BLOB_TRANSFER_CLIENT_PARTIAL_BLOCK_REPORT:
        {
            if (blob_client_ctx.phase != BLOB_CLIENT_PHASE_BLOB_CHUNK_TRANSFER)
            {
                printe("blob partial block report: wrong phase %d", blob_client_ctx.phase);
                return MODEL_SUCCESS;
            }

            blob_transfer_client_partial_block_report_t *pdata = (blob_transfer_client_partial_block_report_t *)
                                                                 pargs;
            pnode = blob_recvs_node_get_by_addr(pdata->src);
            if (!pnode)
            {
                printe("blob partial block report: addr 0x%04x not in recvs list", pdata->src);
                return MODEL_SUCCESS;
            }

            if (pnode->node_phase == BLOB_NODE_PHASE_CHUNK_TRANSFERRING)
            {
                blob_client_procedure_timer_stop();
                if (MESH_IS_UNICAST_ADDR(blob_client_ctx.multicast_addr))
                {
                    blob_client_retry_timer_stop();
                }
                if (pnode->pmissing_chunks == NULL)
                {
                    pnode->pmissing_chunks = plt_zalloc(blob_client_ctx.current_total_chunks * sizeof(uint16_t),
                                                        RAM_TYPE_DATA_ON);
                }
                if (pdata->missing_chunks_len > 0)
                {
                    pnode->current_missing_chunks_len = pdata->missing_chunks_len;
                    memcpy(pnode->pmissing_chunks, pdata->pmissing_chunks,
                           pdata->missing_chunks_len * sizeof(uint16_t));
                    printi("blob partial block report: 0x%04x missing chunks", pdata->src);
                    dprinti((uint8_t *)pnode->pmissing_chunks, pnode->current_missing_chunks_len * sizeof(uint16_t));

                    uint32_t sending_chunk_num = blob_client_missing_chunks_num_get();
                    printi("sending chunk num %d", sending_chunk_num);

                    /* update sending list */
                    for (uint16_t i = 0; i < pnode->current_missing_chunks_len; ++i)
                    {
                        plt_bit_pool_set(blob_client_ctx.sending_chunks, pnode->pmissing_chunks[i], 1);
                    }

                    if (sending_chunk_num == 0)
                    {
                        blob_client_active_chunk_transfer();
                    }
                }
                else
                {
                    pnode->node_phase = BLOB_NODE_PHASE_CHUNK_TRANSFERRED;
                    printi("blob partial block report: 0x%04x missing none chunk", pdata->src);

                    /* check if all active node complete */
                    blob_client_retry_timer_stop();
                    blob_client_handle_partial_report_complete();
                }
            }
            else if (pnode->node_phase == BLOB_NODE_PHASE_CHUNK_TRANSFERRED)
            {
                printi("blob partial block report: 0x%04x missing none chunk", pdata->src);
            }
            else
            {
                printe("blob partial block report: wrong node phase %d", pnode->node_phase);
            }
        }
        break;
    case BLOB_TRANSFER_CLIENT_INFO_STATUS:
        {
            if (blob_client_ctx.phase != BLOB_CLIENT_PHASE_RETRIEVE)
            {
                printe("blob transfer info: wrong phase %d", blob_client_ctx.phase);
            }

            blob_transfer_client_info_status_t *pdata = (blob_transfer_client_info_status_t *)pargs;
            blob_client_retry_timer_stop();
            blob_recvs_caps_p pcaps = blob_recvs_caps_get_by_addr(pdata->src);
            if (pcaps != NULL)
            {
                pcaps->caps.min_block_size_log = pdata->min_block_size_log;
                pcaps->caps.max_block_size_log = pdata->max_block_size_log;
                pcaps->caps.max_total_chunks = pdata->max_total_chunks;
                pcaps->caps.max_chunk_size = pdata->max_chunk_size;
                pcaps->caps.max_blob_size = pdata->max_blob_size;
                pcaps->caps.server_mtu_size = pdata->server_mtu_size;
                pcaps->caps.mode_push_support = pdata->supported_transfer_mode & BLOB_TRANSFER_MODE_PUSH_MASK;
                pcaps->caps.mode_pull_support = (pdata->supported_transfer_mode & BLOB_TRANSFER_MODE_PULL_MASK) >>
                                                1;
                pcaps->caps.mode_support_rfu = (pdata->supported_transfer_mode & BLOB_TRANSFER_MODE_RFU_MASK) >> 6;
            }

            /* find active node and send blob info get */
            if (blob_client_active_info_get_send())
            {
                return MODEL_SUCCESS;
            }

            blob_client_handle_caps_retrieve();
        }
        break;
    default:
        break;
    }

    if (blob_client_ctx.phase != BLOB_CLIENT_PHASE_IDLE)
    {
        pnode = (blob_recvs_node_t *)blob_client_ctx.blob_recvs_list.pfirst;
        while (pnode != NULL)
        {
            if (pnode->active)
            {
                return MODEL_SUCCESS;
            }
            pnode = pnode->pnext;
        }
        printe("blob_client_app_handle_data: no active node");
        blob_client_handle_transfer(false);
    }

    return MODEL_SUCCESS;
}

void blob_client_app_cb_reg(pf_blob_client_cb_t pf_blob_client_cb)
{
    blob_client_ctx.blob_client_cb = pf_blob_client_cb;
}

void blob_client_app_init(uint8_t element_index, pf_blob_client_cb_t pf_blob_client_cb)
{
    if (blob_client_ctx.client_mtu_size == 0)
    {
        blob_client_ctx.client_mtu_size = BLOB_CLIENT_MTU;
    }
    blob_client_ctx.blob_client_cb = pf_blob_client_cb;
    blob_transfer_client_reg(element_index, blob_client_app_handle_data);
}
#endif
