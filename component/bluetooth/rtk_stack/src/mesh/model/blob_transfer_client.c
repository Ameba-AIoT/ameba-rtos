/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     blob_transfer.c
* @brief    Source file for blob transfer client model.
* @details  Data types and external functions declaration.
* @author   bill
* @date     2018-5-21
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include <string.h>
#include "mesh_api.h"
#include "blob_transfer.h"

#if MESH_BLOB

mesh_model_info_t blob_transfer_client;

static mesh_msg_send_cause_t blob_transfer_client_send(uint16_t dst, uint16_t app_key_index,
                                                       uint8_t *pmsg, uint16_t len)
{
    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = &blob_transfer_client;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = len;
    mesh_msg.dst = dst;
    mesh_msg.app_key_index = app_key_index;
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t blob_transfer_get(uint16_t dst, uint16_t app_key_index)
{
    blob_transfer_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_BLOB_TRANSFER_GET);
    return blob_transfer_client_send(dst, app_key_index, (uint8_t *)&msg, sizeof(blob_transfer_get_t));
}

mesh_msg_send_cause_t blob_transfer_start(uint16_t dst, uint16_t app_key_index,
                                          blob_transfer_mode_t mode, uint8_t blob_id[8], uint32_t blob_size, uint8_t block_size_log,
                                          uint16_t client_mtu_size)
{
    blob_transfer_start_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_BLOB_TRANSFER_START);
    msg.rfu = 0;
    msg.transfer_mode = mode;
    memcpy(msg.blob_id, blob_id, 8);
    msg.blob_size = blob_size;
    msg.block_size_log = block_size_log;
    msg.client_mtu_size = client_mtu_size;
    return blob_transfer_client_send(dst, app_key_index, (uint8_t *)&msg,
                                     sizeof(blob_transfer_start_t));
}

mesh_msg_send_cause_t blob_transfer_cancel(uint16_t dst, uint16_t app_key_index, uint8_t blob_id[8])
{
    blob_transfer_cancel_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_BLOB_TRANSFER_CANCEL);
    memcpy(msg.blob_id, blob_id, sizeof(msg.blob_id));
    return blob_transfer_client_send(dst, app_key_index, (uint8_t *)&msg,
                                     sizeof(blob_transfer_cancel_t));
}

mesh_msg_send_cause_t blob_block_get(uint16_t dst, uint16_t app_key_index)
{
    blob_block_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_BLOB_BLOCK_GET);
    return blob_transfer_client_send(dst, app_key_index, (uint8_t *)&msg, sizeof(blob_block_get_t));
}

mesh_msg_send_cause_t blob_block_start(uint16_t dst, uint16_t app_key_index, uint16_t block_num,
                                       uint16_t chunk_size)
{
    blob_block_start_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_BLOB_BLOCK_START);
    msg.block_num = block_num;
    msg.chunk_size = chunk_size;
    return blob_transfer_client_send(dst, app_key_index, (uint8_t *)&msg, sizeof(blob_block_start_t));
}

mesh_msg_send_cause_t blob_chunk_transfer(uint16_t dst, uint16_t app_key_index, uint16_t chunk_num,
                                          uint8_t *pdata, uint16_t len)
{
    mesh_msg_send_cause_t ret;
    blob_chunk_transfer_t *pmsg = (blob_chunk_transfer_t *)plt_malloc(sizeof(
                                                                          blob_chunk_transfer_t) + len, RAM_TYPE_DATA_ON);
    if (pmsg == NULL)
    {
        printe("blob_chunk_transfer: failed, out of memory");
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }
    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_BLOB_CHUNK_TRANSFER);
    pmsg->chunk_num = chunk_num;
    memcpy(pmsg->data, pdata, len);
    ret = blob_transfer_client_send(dst, app_key_index, (uint8_t *)pmsg,
                                    sizeof(blob_chunk_transfer_t) + len);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

mesh_msg_send_cause_t blob_info_get(uint16_t dst, uint16_t app_key_index)
{
    blob_info_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_BLOB_INFO_GET);
    return blob_transfer_client_send(dst, app_key_index, (uint8_t *)&msg, sizeof(blob_info_get_t));
}

static uint16_t utf8_to_num(uint8_t *putf8, uint8_t *len)
{
    uint16_t data = 0;
    if (*putf8 >= 0xc0)
    {
        data = putf8[0] - 0xc0;
        data <<= 6;
        data += (putf8[1] - 0x80);
        *len = 2;
    }
    else
    {
        data = putf8[0];
        *len = 1;
    }

    return data;
}

bool blob_transfer_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_BLOB_TRANSFER_STATUS:
        {
            blob_transfer_status_t *pmsg = (blob_transfer_status_t *)pbuffer;
            blob_transfer_client_transfer_status_t status_data;
            memset(&status_data, 0, sizeof(blob_transfer_client_transfer_status_t));
            status_data.src = pmesh_msg->src;
            status_data.app_key_index = pmesh_msg->app_key_index;
            status_data.status = pmsg->status;
            status_data.transfer_mode = pmsg->transfer_mode;
            status_data.transfer_phase = pmsg->transfer_phase;
            uint16_t *pmissing_blocks = NULL;
            uint16_t missing_blocks_len = 0;

            if (pmesh_msg->msg_len == MEMBER_OFFSET(blob_transfer_status_t, blob_id))
            {
                data_uart_debug("receive blob transfer status: status %d, mode %d, phase %d\r\n",
                                pmsg->status, pmsg->transfer_mode, pmsg->transfer_phase);
            }
            else if (pmesh_msg->msg_len == MEMBER_OFFSET(blob_transfer_status_t, blob_size))
            {
                memcpy(status_data.blob_id, pmsg->blob_id, 8);
                data_uart_debug("receive blob transfer status: status %d, mode %d, phase %d, blob_id \r\n",
                                pmsg->status, pmsg->transfer_mode, pmsg->transfer_phase);
                data_uart_dump(pmsg->blob_id, 8);
            }
            else
            {
                memcpy(status_data.blob_id, pmsg->blob_id, 8);
                status_data.blob_size = pmsg->blob_size;
                status_data.block_size_log = pmsg->block_size_log;
                status_data.transfer_mtu_size = pmsg->transfer_mtu_size;

                data_uart_debug("receive blob transfer status: status %d, mode %d, phase %d, blob_size %d, block_size_log %d, transfer_mtu_size %d, blob_id \r\n",
                                pmsg->status, pmsg->transfer_mode, pmsg->transfer_phase, pmsg->blob_size, pmsg->block_size_log,
                                pmsg->transfer_mtu_size);
                data_uart_dump(pmsg->blob_id, 8);
                if (pmesh_msg->msg_len > sizeof(blob_transfer_status_t))
                {
                    uint16_t data_len = pmesh_msg->msg_len - sizeof(blob_transfer_status_t);
                    uint8_t *pdata = pmsg->blocks_not_received;
                    pmissing_blocks = plt_malloc(data_len * 8 * 2, RAM_TYPE_DATA_ON);
                    uint16_t *pbuffer = pmissing_blocks;
                    if (NULL == pmissing_blocks)
                    {
                        printe("receive blob transfer status: out of memory!");
                        return true;
                    }
                    for (uint16_t i = 0; i < data_len * 8; ++i)
                    {
                        if (plt_bit_pool_get(pdata, i))
                        {
                            *pbuffer ++ = i;
                            missing_blocks_len ++;
                        }
                    }
                    data_uart_debug("blocks_not_received = ");
                    data_uart_dump((uint8_t *)pmissing_blocks, missing_blocks_len * 2);
                }
            }

            if (NULL != blob_transfer_client.model_data_cb)
            {
                status_data.pmissing_blocks = pmissing_blocks;
                status_data.missing_blocks_len = missing_blocks_len;
                blob_transfer_client.model_data_cb(&blob_transfer_client, BLOB_TRANSFER_CLIENT_TRANSFER_STATUS,
                                                   &status_data);
            }

            if (NULL != pmissing_blocks)
            {
                plt_free(pmissing_blocks, RAM_TYPE_DATA_ON);
            }
        }
        break;
    case MESH_MSG_BLOB_BLOCK_STATUS:
        {
            blob_block_status_t *pmsg = (blob_block_status_t *)pbuffer;
            data_uart_debug("receive blob block status: status %d, format %d, block_num %d, chunk_size %d",
                            pmsg->status, pmsg->format, pmsg->block_num, pmsg->chunk_size);
            uint16_t *pmissing_chunks = NULL;
            uint16_t missing_chunks_len = 0;
            uint16_t data_len = pmesh_msg->msg_len - sizeof(blob_block_status_t);
            uint8_t *pdata = pmsg->missing_chunks;
            if (pmsg->format == BLOB_CHUNK_MISSING_FORMAT_ENCODED)
            {
                data_uart_debug(" ,missing_chunks = ");
                uint8_t convert_len;
                uint16_t *pmissing_chunks = NULL;
                uint16_t missing_chunks_len = 0;
                if (data_len > 0)
                {
                    pmissing_chunks = plt_malloc(data_len * 2, RAM_TYPE_DATA_ON);
                    uint16_t *pbuffer = pmissing_chunks;
                    if (NULL == pmissing_chunks)
                    {
                        printe("receive partial block report: out of memory!");
                        return true;
                    }

                    while ((pdata - pmsg->missing_chunks) < data_len)
                    {
                        *pbuffer ++ = utf8_to_num(pdata, &convert_len);
                        pdata += convert_len;
                        missing_chunks_len ++;
                    }
                    data_uart_dump((uint8_t *)pmissing_chunks, missing_chunks_len * 2);
                }
            }
            else if (pmsg->format == BLOB_CHUNK_MISSING_FORMAT_SOME)
            {
                data_uart_debug(" ,missing_chunks = ");
                if (data_len > 0)
                {
                    pmissing_chunks = plt_malloc(data_len * 8 * 2, RAM_TYPE_DATA_ON);
                    if (NULL == pmissing_chunks)
                    {
                        printe("receive partial block report: out of memory!");
                        return true;
                    }
                    uint16_t *pbuffer = pmissing_chunks;
                    for (uint16_t i = 0; i < data_len * 8; ++i)
                    {
                        if (plt_bit_pool_get(pdata, i))
                        {
                            *pbuffer ++ = i;
                            missing_chunks_len ++;
                        }
                    }
                    data_uart_dump((uint8_t *)pmissing_chunks, missing_chunks_len * 2);
                }
            }
            data_uart_debug("\r\n");

            if (NULL != blob_transfer_client.model_data_cb)
            {
                blob_transfer_client_block_status_t block_data;
                block_data.src = pmesh_msg->src;
                block_data.app_key_index = pmesh_msg->app_key_index;
                block_data.status = pmsg->status;
                block_data.block_num = pmsg->block_num;
                block_data.missing_format = pmsg->format;
                if (BLOB_CHUNK_MISSING_FORMAT_ENCODED == pmsg->format)
                {
                    block_data.missing_format = BLOB_CHUNK_MISSING_FORMAT_SOME;
                }
                block_data.pmissing_chunks = pmissing_chunks;
                block_data.missing_chunks_len = missing_chunks_len;
                blob_transfer_client.model_data_cb(&blob_transfer_client, BLOB_TRANSFER_CLIENT_BLOCK_STATUS,
                                                   &block_data);
            }

            if (NULL != pmissing_chunks)
            {
                plt_free(pmissing_chunks, RAM_TYPE_DATA_ON);
            }
        }
        break;
    case MESH_MSG_BLOB_PARTIAL_BLOCK_REPORT:
        {
            blob_partial_block_report_t *pmsg = (blob_partial_block_report_t *)pbuffer;
            data_uart_debug("receive partial block report: encoded missing chunks = ");
            uint8_t *pdata = pmsg->encoded_missing_chunks;
            uint16_t data_len = pmesh_msg->msg_len - sizeof(blob_partial_block_report_t);
            uint8_t convert_len;
            uint16_t *pmissing_chunks = NULL;
            uint16_t missing_chunks_len = 0;
            if (data_len > 0)
            {
                pmissing_chunks = plt_malloc(data_len * 2, RAM_TYPE_DATA_ON);
                uint16_t *pbuffer = pmissing_chunks;
                if (NULL == pmissing_chunks)
                {
                    printe("receive partial block report: out of memory!");
                    return true;
                }

                while ((pdata - pmsg->encoded_missing_chunks) < data_len)
                {
                    *pbuffer ++ = utf8_to_num(pdata, &convert_len);
                    pdata += convert_len;
                    missing_chunks_len ++;
                }
                data_uart_dump((uint8_t *)pmissing_chunks, missing_chunks_len * 2);
            }
            data_uart_debug("\r\n");

            if (NULL != blob_transfer_client.model_data_cb)
            {
                blob_transfer_client_partial_block_report_t report_data;
                report_data.src = pmesh_msg->src;
                report_data.app_key_index = pmesh_msg->app_key_index;
                report_data.pmissing_chunks = pmissing_chunks;
                report_data.missing_chunks_len = missing_chunks_len;
                blob_transfer_client.model_data_cb(&blob_transfer_client, BLOB_TRANSFER_CLIENT_PARTIAL_BLOCK_REPORT,
                                                   &report_data);
            }

            if (NULL != pmissing_chunks)
            {
                plt_free(pmissing_chunks, RAM_TYPE_DATA_ON);
            }
        }
        break;
    case MESH_MSG_BLOB_INFO_STATUS:
        {
            blob_info_status_t *pmsg = (blob_info_status_t *)pbuffer;
            if (NULL != blob_transfer_client.model_data_cb)
            {
                blob_transfer_client_info_status_t status_data;
                status_data.src = pmesh_msg->src;
                status_data.app_key_index = pmesh_msg->app_key_index;
                status_data.min_block_size_log = pmsg->min_block_size_log;
                status_data.max_block_size_log = pmsg->max_block_size_log;
                status_data.max_total_chunks = pmsg->max_total_chunks;
                status_data.max_chunk_size = pmsg->max_chunk_size;
                status_data.max_blob_size = pmsg->max_blob_size;
                status_data.server_mtu_size = pmsg->server_mtu_size;
                status_data.supported_transfer_mode = pmsg->supported_transfer_mode;
                blob_transfer_client.model_data_cb(&blob_transfer_client, BLOB_TRANSFER_CLIENT_INFO_STATUS,
                                                   &status_data);
            }
            data_uart_debug("receive blob info status: min_block_size_log %d, max_block_size_log %d, max_total_chunks %d, max_chunk_size %d, max_blob_size %d, server_mtu_size %d, supported_transfer_mode %d\r\n",
                            pmsg->min_block_size_log, pmsg->max_block_size_log, pmsg->max_total_chunks, pmsg->max_chunk_size,
                            pmsg->max_blob_size, pmsg->server_mtu_size, pmsg->supported_transfer_mode);
        }
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

void blob_transfer_client_reg(uint8_t element_index, model_data_cb_pf model_data_cb)
{
    blob_transfer_client.model_id = MESH_MODEL_BLOB_TRANSFER_CLIENT;
    blob_transfer_client.model_receive = blob_transfer_client_receive;
    blob_transfer_client.model_data_cb = model_data_cb;
    mesh_model_reg(element_index, &blob_transfer_client);
}

#endif /* MESH_BLOB */
