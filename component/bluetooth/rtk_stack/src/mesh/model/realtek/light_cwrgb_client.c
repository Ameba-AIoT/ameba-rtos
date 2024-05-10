/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     light_cwrgb_client.c
* @brief    Source file for light cwrgb client module.
* @details  Data types and external functions declaration.
* @author   bill
* @date     2018-1-3
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "light_cwrgb.h"

static mesh_msg_send_cause_t light_cwrgb_client_send(mesh_model_info_p pmodel_info, uint16_t dst,
                                                     uint8_t *pmsg, uint16_t msg_len, uint16_t app_key_index)
{
    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = msg_len;
    mesh_msg.dst = dst;
    mesh_msg.app_key_index = app_key_index;
    return access_send(&mesh_msg);
}

mesh_msg_send_cause_t light_cwrgb_get(mesh_model_info_p pmodel_info, uint16_t dst,
                                      uint16_t app_key_index)
{
    light_cwrgb_get_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CWRGB_GET);
    return light_cwrgb_client_send(pmodel_info, dst, (uint8_t *)&msg, sizeof(msg), app_key_index);
}

mesh_msg_send_cause_t light_cwrgb_set(mesh_model_info_p pmodel_info, uint16_t dst,
                                      uint16_t app_key_index, uint8_t cwrgb[5], bool ack)
{
    light_cwrgb_set_t msg;
    if (ack)
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CWRGB_SET);
    }
    else
    {
        ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CWRGB_SET_UNACK);
    }
    memcpy(msg.cwrgb, cwrgb, sizeof(msg.cwrgb));
    return light_cwrgb_client_send(pmodel_info, dst, (uint8_t *)&msg, sizeof(msg), app_key_index);
}

/* Sample
bool light_cwrgb_client_receive(mesh_msg_p pmesh_msg)
{
  bool ret = TRUE;
  uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;

  switch (pmesh_msg->access_opcode)
  {
  case MESH_MSG_LIGHT_CWRGB_STAT:
      if (pmesh_msg->msg_len == sizeof(light_cwrgb_stat_t))
      {

      }
      break;
  default:
      ret = FALSE;
      break;
  }
  return ret;
}
*/

bool light_cwrgb_client_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    pmodel_info->model_id = MESH_MODEL_LIGHT_CWRGB_CLIENT;
    return mesh_model_reg(element_index, pmodel_info);
}

