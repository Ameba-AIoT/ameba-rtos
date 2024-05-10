/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     firmware_distribution_client.c
* @brief    Source file for firmware distribution client model.
* @details  Data types and external functions declaration.
* @author   bill
* @date     2018-5-21
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include <string.h>
#include "mesh_api.h"
#include "firmware_distribution.h"

#if MESH_DFU
mesh_model_info_t fw_dist_client;

static mesh_msg_send_cause_t fw_dist_server_send(uint16_t dst, uint16_t app_key_index,
                                                 uint8_t *pmsg, uint16_t len)
{
    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = &fw_dist_client;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = len;
    if (0 != dst)
    {
        mesh_msg.dst = dst;
        mesh_msg.app_key_index = app_key_index;
    }
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t fw_dist_recvs_add(uint16_t dst, uint16_t app_key_index,
                                        fw_dist_receiver_t *precvs, uint8_t recvs_len)
{
    uint16_t msg_len = sizeof(fw_dist_recvs_add_t) + sizeof(fw_dist_receiver_t) * recvs_len;
    fw_dist_recvs_add_t *pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
    if (NULL == pmsg)
    {
        printe("fw_dist_recvs_add: failed, out of memory");
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }

    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_FW_DIST_RECVS_ADD);
    memcpy(pmsg->entries, precvs, recvs_len);
    mesh_msg_send_cause_t ret = fw_dist_server_send(dst, app_key_index, (uint8_t *)pmsg, msg_len);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

mesh_msg_send_cause_t fw_dist_recvs_delete_all(uint16_t dst, uint16_t app_key_index)
{
    fw_dist_recvs_delete_all_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_FW_DIST_RECVS_DELETE_ALL);

    return fw_dist_server_send(dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t fw_dist_recvs_get(uint16_t dst, uint16_t app_key_index, uint16_t first_index,
                                        uint16_t entries_limit)
{
    fw_dist_recvs_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_FW_DIST_RECVS_GET);
    msg.first_index = first_index;
    msg.entries_limit = entries_limit;

    return fw_dist_server_send(dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t fw_dist_caps_get(uint16_t dst, uint16_t app_key_index)
{
    fw_dist_caps_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_FW_DIST_CAPS_GET);

    return fw_dist_server_send(dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t fw_dist_get(uint16_t dst, uint16_t app_key_index)
{
    fw_dist_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_FW_DIST_GET);

    return fw_dist_server_send(dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t fw_dist_start(uint16_t dst, uint16_t app_key_index,
                                    fw_dist_start_data_t start, uint8_t dist_dst_len)
{
    fw_dist_start_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_FW_DIST_START);
    msg.dist_app_key_index = start.dist_app_key_index;
    msg.dist_ttl = start.dist_ttl;
    msg.dist_timeout_base = start.dist_timeout_base;
    msg.dist_transfer_mode = start.dist_transfer_mode;
    msg.update_policy = start.update_policy;
    msg.rfu = 0;
    msg.dist_fw_image_idx = start.dist_fw_image_idx;
    memcpy(msg.dist_multicast_addr, start.dist_multicast_addr, dist_dst_len);

    return fw_dist_server_send(dst, app_key_index, (uint8_t *)&msg, MEMBER_OFFSET(fw_dist_start_t,
                                                                                  dist_multicast_addr) + dist_dst_len);
}

mesh_msg_send_cause_t fw_dist_cancel(uint16_t dst, uint16_t app_key_index)
{
    fw_dist_cancel_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_FW_DIST_CANCEL);

    return fw_dist_server_send(dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t fw_dist_apply(uint16_t dst, uint16_t app_key_index)
{
    fw_dist_apply_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_FW_DIST_APPLY);

    return fw_dist_server_send(dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t fw_dist_upload_get(uint16_t dst, uint16_t app_key_index)
{
    fw_dist_upload_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_FW_DIST_UPLOAD_GET);

    return fw_dist_server_send(dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t fw_dist_upload_start(uint16_t dst, uint16_t app_key_index, uint8_t upload_ttl,
                                           uint16_t upload_timeout_base, uint8_t blob_id[8], uint32_t upload_fw_size, uint8_t *pmetadata,
                                           uint8_t metadata_len, uint8_t *pfw_id, uint8_t fw_id_len)
{
    uint16_t msg_len = sizeof(fw_dist_upload_start_t) + metadata_len + fw_id_len;
    fw_dist_upload_start_t *pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
    if (NULL == pmsg)
    {
        printe("fw_dist_upload_start: failed, out of memory");
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }

    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_FW_DIST_UPLOAD_START);
    pmsg->upload_ttl = upload_ttl;
    pmsg->upload_timeout_base = upload_timeout_base;
    memcpy(pmsg->blob_id, blob_id, 8);
    pmsg->upload_fw_size = upload_fw_size;
    pmsg->upload_fw_metadata_len = metadata_len;
    uint8_t *pdata = pmsg->upload_fw_info;
    memcpy(pdata, pmetadata, metadata_len);
    pdata += metadata_len;
    memcpy(pdata, pfw_id, fw_id_len);

    mesh_msg_send_cause_t ret = fw_dist_server_send(dst, app_key_index, (uint8_t *)pmsg, msg_len);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

mesh_msg_send_cause_t fw_dist_upload_oob_start(uint16_t dst, uint16_t app_key_index,
                                               uint8_t *pupload_uri, uint8_t upload_uri_len, uint8_t *pfw_id, uint8_t fw_id_len)
{
    uint16_t msg_len = sizeof(fw_dist_upload_oob_start_t) + upload_uri_len + fw_id_len;
    fw_dist_upload_oob_start_t *pmsg = plt_malloc(msg_len, RAM_TYPE_BUFFER_ON);
    if (NULL == pmsg)
    {
        printe("fw_dist_upload_oob_start: failed, out of memory");
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }

    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_FW_DIST_UPLOAD_OOB_START);
    pmsg->upload_uri_len = upload_uri_len;
    uint8_t *pdata = pmsg->upload_fw_info;
    memcpy(pdata, pupload_uri, upload_uri_len);
    pdata += upload_uri_len;
    memcpy(pdata, pfw_id, fw_id_len);

    mesh_msg_send_cause_t ret = fw_dist_server_send(dst, app_key_index, (uint8_t *)pmsg, msg_len);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

mesh_msg_send_cause_t fw_dist_upload_cancel(uint16_t dst, uint16_t app_key_index)
{
    fw_dist_upload_cancel_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_FW_DIST_UPLOAD_CANCEL);

    return fw_dist_server_send(dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t fw_dist_fw_get(uint16_t dst, uint16_t app_key_index, uint8_t *pfw_id,
                                     uint8_t fw_id_len)
{
    uint16_t msg_len = sizeof(fw_dist_fw_get_t) + fw_id_len;
    fw_dist_fw_get_t *pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
    if (NULL == pmsg)
    {
        printe("fw_dist_fw_get: failed, out of memory");
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }

    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_FW_DIST_FW_GET);
    memcpy(pmsg->fw_id, pfw_id, fw_id_len);

    mesh_msg_send_cause_t ret = fw_dist_server_send(dst, app_key_index, (uint8_t *)pmsg, msg_len);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

mesh_msg_send_cause_t fw_dist_fw_get_by_index(uint16_t dst, uint16_t app_key_index,
                                              uint16_t dist_fw_image_index)
{
    fw_dist_fw_get_by_index_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_FW_DIST_FW_GET_BY_INDEX);
    msg.dist_fw_image_idx = dist_fw_image_index;

    return fw_dist_server_send(dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t fw_dist_fw_delete(uint16_t dst, uint16_t app_key_index, uint8_t *pfw_id,
                                        uint8_t fw_id_len)
{
    uint16_t msg_len = sizeof(fw_dist_fw_delete_t) + fw_id_len;
    fw_dist_fw_delete_t *pmsg = plt_malloc(msg_len, RAM_TYPE_DATA_ON);
    if (NULL == pmsg)
    {
        printe("fw_dist_fw_delete: failed, out of memory");
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }

    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_FW_DIST_FW_DELETE);
    memcpy(pmsg->fw_id, pfw_id, fw_id_len);

    mesh_msg_send_cause_t ret = fw_dist_server_send(dst, app_key_index, (uint8_t *)pmsg, msg_len);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

mesh_msg_send_cause_t fw_dist_fw_delete_all(uint16_t dst, uint16_t app_key_index)
{
    fw_dist_fw_delete_all_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_FW_DIST_FW_DELETE_ALL);

    return fw_dist_server_send(dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

bool fw_dist_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_FW_DIST_RECVS_STATUS:
        {
            fw_dist_recvs_status_t *pmsg = (fw_dist_recvs_status_t *)pbuffer;
            if (NULL != fw_dist_client.model_data_cb)
            {
                fw_dist_client_recvs_status_t status_data;
                status_data.src = pmesh_msg->src;
                status_data.status = pmsg->status;
                status_data.recvs_list_cnt = pmsg->recvs_list_cnt;
                fw_dist_client.model_data_cb(&fw_dist_client, FW_DIST_CLIENT_RECVS_STATUS, &status_data);
            }
            data_uart_debug("receive fw dist receivers status: src 0x%04x, status %d, list count %d\r\n",
                            pmesh_msg->src, pmsg->status, pmsg->recvs_list_cnt);
        }
        break;
    case MESH_MSG_FW_DIST_RECVS_LIST:
        {
            fw_dist_recvs_list_t *pmsg = (fw_dist_recvs_list_t *)pbuffer;
            if (NULL != fw_dist_client.model_data_cb)
            {
                fw_dist_client_recvs_list_t list_data;
                list_data.src = pmesh_msg->src;
                list_data.recvs_list_cnt = pmsg->recvs_list_cnt;
                list_data.first_index = pmsg->first_index;
                list_data.pentries = pmsg->entries;
                list_data.entries_cnt = (pmesh_msg->msg_len - sizeof(fw_dist_recvs_list_t)) / sizeof(
                                            fw_update_node_t);
                fw_dist_client.model_data_cb(&fw_dist_client, FW_DIST_CLIENT_RECVS_LIST, &list_data);
            }
            data_uart_debug("receive fw dist receivers list: src 0x%04x, recvs_list_cnt %d, first_index %d\r\n",
                            pmesh_msg->src, pmsg->recvs_list_cnt, pmsg->first_index);

            fw_update_node_t *pentries = pmsg->entries;
            for (uint8_t i = 0;
                 i < (pmesh_msg->msg_len - sizeof(fw_dist_recvs_list_t)) / sizeof(fw_update_node_t); ++i)
            {
                data_uart_debug("addr 0x%04x, retrieved_update_phase %d, update_status %d, transfer_status %d, transfer_progress %d, update_fw_image_idx %d\r\n",
                                pentries->addr, pentries->retrieved_update_phase, pentries->update_status,
                                pentries->transfer_status, pentries->transfer_progress, pentries->update_fw_image_idx);
                pentries ++;
            }
        }
        break;
    case MESH_MSG_FW_DIST_CAPS_STATUS:
        {
            fw_dist_caps_status_t *pmsg = (fw_dist_caps_status_t *)pbuffer;
            if (NULL != fw_dist_client.model_data_cb)
            {
                fw_dist_client_caps_status_t status_data;
                status_data.src = pmesh_msg->src;
                status_data.dist_caps = pmsg->dist_caps;
                status_data.psupported_uri_scheme_names = pmsg->supported_uri_scheme_names;
                status_data.names_len = pmesh_msg->msg_len - sizeof(fw_dist_caps_status_t);
                fw_dist_client.model_data_cb(&fw_dist_client, FW_DIST_CLIENT_CAPS_STATUS, &status_data);
            }

            data_uart_debug("receive fw dist capabilites: src 0x%04x max_dist_recvs_list_size %d, max_fw_images_list_size %d, max_fw_image_size %d, max_upload_spcace %d, remaining_upload_space %d, oob_retrieval_supported %d, supported_uri_scheme_names = \r\n",
                            pmesh_msg->src, pmsg->dist_caps.max_dist_recvs_list_size, pmsg->dist_caps.max_fw_images_list_size,
                            pmsg->dist_caps.max_fw_image_size, pmsg->dist_caps.max_upload_spcace,
                            pmsg->dist_caps.remaining_upload_space, pmsg->dist_caps.oob_retrieval_supported);

            data_uart_dump(pmsg->supported_uri_scheme_names,
                           pmesh_msg->msg_len - sizeof(fw_dist_caps_status_t));
        }
        break;
    case MESH_MSG_FW_DIST_STATUS:
        {
            fw_dist_status_t *pmsg = (fw_dist_status_t *)pbuffer;
            if (NULL != fw_dist_client.model_data_cb)
            {
                fw_dist_client_dist_status_t status_data;
                status_data.src = pmesh_msg->src;
                status_data.status = pmsg->status;
                status_data.phase = pmsg->phase;
                status_data.dist_multicast_addr = pmsg->dist_multicast_addr;
                status_data.dist_appkey_index = pmsg->dist_appkey_index;
                status_data.dist_ttl = pmsg->dist_ttl;
                status_data.dist_timeout_base = pmsg->dist_timeout_base;
                status_data.dist_transfer_mode = pmsg->dist_transfer_mode;
                status_data.update_policy = pmsg->update_policy;
                status_data.rfu = 0;
                status_data.dist_fw_image_idx = pmsg->dist_fw_image_idx;
                fw_dist_client.model_data_cb(&fw_dist_client, FW_DIST_CLIENT_STATUS, &status_data);
            }

            data_uart_debug("receive fw dist status: src 0x%04x, status %d, phase %d, dist_multicast_addr 0x%04x, dist_appkey_index 0x%04x, dist_ttl %d, dist_timeout_base %d, dist_transfer_mode %d, update_policy %d, dist_fw_image_idx %d\r\n",
                            pmesh_msg->src, pmsg->status, pmsg->phase, pmsg->dist_multicast_addr, pmsg->dist_appkey_index,
                            pmsg->dist_ttl, pmsg->dist_timeout_base, pmsg->dist_transfer_mode, pmsg->update_policy,
                            pmsg->dist_fw_image_idx);
        }
        break;
    case MESH_MSG_FW_DIST_UPLOAD_STATUS:
        {
            fw_dist_upload_status_t *pmsg = (fw_dist_upload_status_t *)pbuffer;
            if (NULL != fw_dist_client.model_data_cb)
            {
                fw_dist_client_upload_status_t status_data;
                status_data.src = pmesh_msg->src;
                status_data.status = pmsg->status;
                status_data.phase = pmsg->phase;
                status_data.upload_progress = pmsg->upload_progress;
                status_data.pupload_fw_id = pmsg->upload_fw_id;
                status_data.upload_fw_id_len = pmesh_msg->msg_len - sizeof(fw_dist_upload_status_t);

                fw_dist_client.model_data_cb(&fw_dist_client, FW_DIST_CLIENT_UPLOAD_STATUS, &status_data);
            }

            data_uart_debug("receive fw dist upload status: src 0x%04x, status %d, phase %d, upload_progress %d, upload_fw_id = ",
                            pmesh_msg->src, pmsg->status, pmsg->phase, pmsg->upload_progress);
            data_uart_dump(pmsg->upload_fw_id, pmesh_msg->msg_len - sizeof(fw_dist_upload_status_t));
        }
        break;
    case MESH_MSG_FW_DIST_FW_STATUS:
        {
            fw_dist_fw_status_t *pmsg = (fw_dist_fw_status_t *)pbuffer;
            if (NULL != fw_dist_client.model_data_cb)
            {
                fw_dist_client_fw_status_t status_data;
                status_data.src = pmesh_msg->src;
                status_data.status = pmsg->status;
                status_data.entry_cnt = pmsg->entry_cnt;
                status_data.dist_fw_image_idx = pmsg->dist_fw_image_idx;
                status_data.pfw_id = pmsg->fw_id;
                status_data.fw_id_len = pmesh_msg->msg_len - sizeof(fw_dist_fw_status_t);

                fw_dist_client.model_data_cb(&fw_dist_client, FW_DIST_CLIENT_FW_STATUS, &status_data);
            }

            data_uart_debug("receive fw dist fw status: src 0x%04x, status %d, entry_cnt %d, dw_fw_image_idx %d, fw_id = ",
                            pmesh_msg->src, pmsg->status, pmsg->entry_cnt, pmsg->dist_fw_image_idx);
            data_uart_dump(pmsg->fw_id, pmesh_msg->msg_len - sizeof(fw_dist_fw_status_t));
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

bool fw_dist_client_reg(uint8_t element_index, model_data_cb_pf model_data_cb)
{
    fw_dist_client.model_id = MESH_MODEL_FW_DIST_CLIENT;
    fw_dist_client.model_receive = fw_dist_client_receive;
    fw_dist_client.model_data_cb = model_data_cb;
    return mesh_model_reg(element_index, &fw_dist_client);
}
#endif
