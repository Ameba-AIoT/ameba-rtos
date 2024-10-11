/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     firmware_update_client.c
* @brief    Source file for firmware update client model.
* @details  Data types and external functions declaration.
* @author   bill
* @date     2018-5-21
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "mesh_api.h"
#include "firmware_update.h"
#include "app_mesh_flags.h"

#if F_BT_MESH_1_1_DFU_SUPPORT
mesh_model_info_t fw_update_client;

static mesh_msg_send_cause_t fw_update_client_send(uint16_t dst, uint16_t app_key_index,
                                                   uint8_t *pmsg, uint16_t len)
{
    mesh_msg_t mesh_msg = {0};
    mesh_msg.pmodel_info = &fw_update_client;
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

mesh_msg_send_cause_t fw_update_info_get(uint16_t dst, uint16_t app_key_index, uint8_t first_index,
                                         uint8_t entries_limit)
{
    fw_update_info_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_FW_UPDATE_INFO_GET);
    msg.first_index = first_index;
    msg.entries_limit = entries_limit;
    return fw_update_client_send(dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t fw_update_fw_metadata_check(uint16_t dst, uint16_t app_key_index,
                                                  uint8_t update_fw_image_idx, uint8_t *pfw_metadata, uint8_t metadata_len)
{
    uint16_t msg_len = sizeof(fw_update_fw_metadata_check_t) + metadata_len;
    fw_update_fw_metadata_check_t *pmsg = (fw_update_fw_metadata_check_t *)plt_zalloc(msg_len,
                                          RAM_TYPE_DATA_ON);
    if (!pmsg)
    {
        printe("fw_update_fw_metadata_check: failed, out of memory");
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }

    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_FW_UPDATE_FW_METADATA_CHECK);
    pmsg->fw_image_idx = update_fw_image_idx;
    memcpy(pmsg->fw_metadata, pfw_metadata, metadata_len);
    mesh_msg_send_cause_t ret = fw_update_client_send(dst, app_key_index, (uint8_t *)pmsg, msg_len);
    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

mesh_msg_send_cause_t fw_update_get(uint16_t dst, uint16_t app_key_index)
{
    fw_update_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_FW_UPDATE_GET);
    return fw_update_client_send(dst, app_key_index, (uint8_t *)&msg, sizeof(fw_update_get_t));
}

mesh_msg_send_cause_t fw_update_start(uint16_t dst, uint16_t app_key_index, uint8_t update_ttl,
                                      uint16_t update_timeout_base, uint8_t blob_id[8], uint8_t update_fw_image_idx,
                                      uint8_t *pfw_metadata, uint8_t metadata_len)
{
    uint16_t msg_len = sizeof(fw_update_start_t) + metadata_len;
    fw_update_start_t *pmsg = plt_zalloc(msg_len, RAM_TYPE_DATA_ON);
    if (!pmsg)
    {
        printe("fw_update_start: failed, out of memory");
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }

    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_FW_UPDATE_START);
    pmsg->update_ttl = update_ttl;
    pmsg->update_timeout_base = update_timeout_base;
    memcpy(pmsg->blob_id, blob_id, 8);
    pmsg->fw_image_idx = update_fw_image_idx;
    if (metadata_len > 0)
    {
        memcpy(pmsg->fw_metadata, pfw_metadata, metadata_len);
    }
    mesh_msg_send_cause_t ret = fw_update_client_send(dst, app_key_index, (uint8_t *)pmsg, msg_len);

    plt_free(pmsg, RAM_TYPE_DATA_ON);

    return ret;
}

mesh_msg_send_cause_t fw_update_cancel(uint16_t dst, uint16_t app_key_index)
{
    fw_update_cancel_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_FW_UPDATE_CANCEL);
    return fw_update_client_send(dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

mesh_msg_send_cause_t fw_update_apply(uint16_t dst, uint16_t app_key_index)
{
    fw_update_apply_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_FW_UPDATE_APPLY);
    return fw_update_client_send(dst, app_key_index, (uint8_t *)&msg, sizeof(msg));
}

bool fw_update_client_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_FW_UPDATE_INFO_STATUS:
        if (pmesh_msg->msg_len >= sizeof(fw_update_info_status_t))
        {
            fw_update_info_status_t *pmsg = (fw_update_info_status_t *)pbuffer;
            uint8_t *pdata = pmsg->fw_info_list;
            printi("receive fw update info status: src 0x%04x, fw_info_list_count %d, first_index %d",
                   pmesh_msg->src, pmsg->fw_info_list_cnt, pmsg->first_index);
            uint8_t fw_info_cnt = 0;
            fw_info_t *pfw_info = NULL;
            uint16_t data_len = pmesh_msg->msg_len - sizeof(fw_update_info_status_t);
            if (data_len > 0)
            {
                while ((pdata - pmsg->fw_info_list) < data_len)
                {
                    uint8_t fw_id_len = *pdata++;
                    pdata += fw_id_len;
                    uint8_t uri_len = *pdata ++;
                    pdata += uri_len;
                    fw_info_cnt ++;
                }
            }

            if (fw_info_cnt > 0)
            {
                pdata = pmsg->fw_info_list;
                pfw_info = plt_zalloc(fw_info_cnt * sizeof(fw_info_t), RAM_TYPE_DATA_ON);
                if (!pfw_info)
                {
                    printe("receive fw update info status: out of memory");
                    return true;
                }

                for (uint8_t i = 0; i < fw_info_cnt; ++i)
                {
                    pfw_info[i].fw_id_len = *pdata++;
                    memcpy(&pfw_info[i].fw_id, pdata, pfw_info[i].fw_id_len);
                    printi("firmware id");
                    dprinti(pdata, pfw_info[i].fw_id_len);
                    pdata += pfw_info[i].fw_id_len;
                    pfw_info[i].update_uri_len = *pdata ++;
                    memcpy(pfw_info[i].update_uri, pdata, pfw_info[i].update_uri_len);
                    printi("update uri");
                    dprinti(pdata, pfw_info[i].update_uri_len);
                    pdata += pfw_info[i].update_uri_len;
                }
            }

            if (fw_update_client.model_data_cb)
            {
                fw_update_client_info_status_t status_data;
                status_data.src = pmesh_msg->src;
                status_data.fw_info_list_cnt = pmsg->fw_info_list_cnt;
                status_data.first_index = pmsg->first_index;
                status_data.pfw_info = pfw_info;
                status_data.fw_info_cnt = fw_info_cnt;
                fw_update_client.model_data_cb(&fw_update_client, FW_UPDATE_CLIENT_INFO_STATUS, &status_data);
            }

            if (pfw_info)
            {
                plt_free(pfw_info, RAM_TYPE_DATA_ON);
            }
        }
        break;
    case MESH_MSG_FW_UPDATE_FW_METADATA_STATUS:
        if (pmesh_msg->msg_len == sizeof(fw_update_fw_metadata_status_t))
        {
            fw_update_fw_metadata_status_t *pmsg = (fw_update_fw_metadata_status_t *)pbuffer;
            if (fw_update_client.model_data_cb)
            {
                fw_update_client_fw_metadata_status_t metadata_data;
                metadata_data.src = pmesh_msg->src;
                metadata_data.status = pmsg->status;
                metadata_data.addi_info = pmsg->addi_info;
                metadata_data.update_fw_image_index = pmsg->fw_image_idx;
                fw_update_client.model_data_cb(&fw_update_client, FW_UPDATE_CLIENT_FW_METADATA_STATUS,
                                               &metadata_data);
            }
            printi("receive fw metadata status: src 0x%04x, status %d, addi_info %d, fw_image_idx %d",
                   pmesh_msg->src, pmsg->status, pmsg->addi_info, pmsg->fw_image_idx);
        }
        break;
    case MESH_MSG_FW_UPDATE_STATUS:
        {
            fw_update_status_t *pmsg = (fw_update_status_t *)pbuffer;
            fw_update_client_status_t status_data;
            memset(&status_data, 0, sizeof(status_data));
            status_data.src = pmesh_msg->src;
            status_data.app_key_index = pmesh_msg->app_key_index;
            status_data.status = pmsg->status;

            if (pmesh_msg->msg_len == sizeof(fw_update_status_t))
            {
                status_data.update_phase = pmsg->update_phase;
                status_data.update_ttl = pmsg->update_ttl;
                status_data.addi_info = pmsg->addi_info;
                status_data.update_timeout_base = pmsg->update_timeout_base;
                memcpy(status_data.blob_id, pmsg->blob_id, 8);
                status_data.update_fw_image_index = pmsg->update_fw_image_idx;
                printi("receive fw update status: src 0x%04x, status %d, update_phase %d, update_ttl %d, addi_info %d, update_timeout_base %d, update_fw_image_idx %d, blob_id",
                       pmesh_msg->src, pmsg->status, pmsg->update_phase, pmsg->update_ttl, pmsg->addi_info,
                       pmsg->update_timeout_base, pmsg->update_fw_image_idx);
                dprinti(pmsg->blob_id, 8);
            }
            else
            {
                printi("receive fw update status: src 0x%04x, status %d, update_phase %d",
                       pmsg->status, pmesh_msg->src, pmsg->update_phase);
            }

            if (fw_update_client.model_data_cb)
            {
                fw_update_client.model_data_cb(&fw_update_client, FW_UPDATE_CLIENT_STATUS, &status_data);
            }
        }
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

bool fw_update_client_reg(uint8_t element_index, model_data_cb_pf model_data_cb)
{
    fw_update_client.model_id = MESH_MODEL_FW_UPDATE_CLIENT;
    fw_update_client.model_receive = fw_update_client_receive;
    fw_update_client.model_data_cb = model_data_cb;
    return mesh_model_reg(element_index, &fw_update_client);
}

#endif /* F_BT_MESH_1_1_DFU_SUPPORT */
