/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     light_cwrgb_server.c
* @brief    Source file for light cwrgb server module.
* @details  Data types and external functions declaration.
* @author   bill
* @date     2018-1-3
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include "light_cwrgb.h"

mesh_msg_send_cause_t light_cwrgb_stat(mesh_model_info_p pmodel_info, uint16_t dst,
                                       uint16_t app_key_index, uint8_t cwrgb[5])
{
    light_cwrgb_stat_t msg;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_LIGHT_CWRGB_STAT);
    memcpy(msg.cwrgb, cwrgb, sizeof(msg.cwrgb));

    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = (uint8_t *)&msg;
    mesh_msg.msg_len = sizeof(light_cwrgb_stat_t);
    mesh_msg.dst = dst;
    mesh_msg.app_key_index = app_key_index;
    return access_send(&mesh_msg);
}

/* Sample
    bool light_cwrgb_server_receive(mesh_msg_p pmesh_msg)
    {
      bool ret = TRUE;
      uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;

      switch (pmesh_msg->access_opcode)
      {
      case MESH_MSG_LIGHT_CWRGB_GET:
          if (pmesh_msg->msg_len == sizeof(light_cwrgb_get_t))
          {

          }
          break;
      case MESH_MSG_LIGHT_CWRGB_SET:
      case MESH_MSG_LIGHT_CWRGB_SET_UNACK:
          if (pmesh_msg->msg_len == sizeof(light_cwrgb_set_t))
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

void light_cwrgb_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info,
                            model_receive_pf pf_model_receive)
{
    pmodel_info->model_id = MESH_MODEL_LIGHT_CWRGB_SERVER;
    pmodel_info->model_receive = pf_model_receive;
    mesh_model_reg(element_index, pmodel_info);
}
