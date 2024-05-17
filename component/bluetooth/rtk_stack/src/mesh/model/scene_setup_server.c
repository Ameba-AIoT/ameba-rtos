/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     scene_setup_server.c
* @brief    Source file for scene setup server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-8-23
* @version  v1.0
* *************************************************************************************
*/

#include "scene.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif


typedef struct
{
    scene_storage_memory_t *scenes;
    uint16_t num_scenes;
    scene_status_code_t status_register;
} scene_setup_info_t;


extern uint16_t get_current_scene(const mesh_model_info_p pmodel_info);
extern mesh_msg_send_cause_t scene_register_status(mesh_model_info_p pmodel_info, uint16_t dst,
                                                   uint16_t app_key_index,
                                                   scene_status_code_t status, uint16_t current_scene,
                                                   uint32_t delay_time);



void scene_setup_server_set_storage_memory(mesh_model_info_p pmodel_info,
                                           scene_storage_memory_t *scenes, uint16_t num_scenes)
{
    scene_setup_info_t *info = pmodel_info->pargs;
    info->scenes = scenes;
    info->num_scenes = num_scenes;
}

static bool scene_setup_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;

    switch (pmesh_msg->access_opcode)
    {
    case MESH_MSG_SCENE_STORE:
    case MESH_MSG_SCENE_STORE_UNACK:
        if (pmesh_msg->msg_len == sizeof(scene_store_t))
        {
            scene_store_t *pmsg = (scene_store_t *)pbuffer;
            if (IS_SCENE_NUMBER_VALID(pmsg->scene_number))
            {
                scene_setup_info_t *pinfo = pmodel_info->pargs;
                uint16_t index = 0;
                uint16_t empty_index = pinfo->num_scenes;
                /* find store place */
                for (index = 0; index < pinfo->num_scenes; ++index)
                {
                    if (0 == pinfo->scenes[index].scene_number)
                    {
                        /* store to last empty location, just for easy process */
                        empty_index = index;
                    }

                    if (pmsg->scene_number == pinfo->scenes[index].scene_number)
                    {
                        break;
                    }
                }

                scene_server_store_t store_data = {SCENE_STATUS_SUCCESS, pmsg->scene_number, NULL};
                pinfo->status_register = SCENE_STATUS_SUCCESS;
                store_data.status = SCENE_STATUS_SUCCESS;
                if (index < pinfo->num_scenes)
                {
                    /* update exists */
                    store_data.pmemory = pinfo->scenes[index].pmemory;
                }
                else if (empty_index < pinfo->num_scenes)
                {
                    /* store new */
                    store_data.scene_number = pmsg->scene_number;
                    pinfo->scenes[empty_index].scene_number = pmsg->scene_number;
                    store_data.pmemory = pinfo->scenes[empty_index].pmemory;
                }
                else
                {
                    /* no place for store */
                    pinfo->status_register = SCENE_STATUS_REGISTER_FULL;
                    store_data.status = SCENE_STATUS_REGISTER_FULL;
                    store_data.scene_number = 0;
                    store_data.pmemory = NULL;
                }

                if (NULL != pmodel_info->model_data_cb)
                {
                    pmodel_info->model_data_cb(pmodel_info, SCENE_SERVER_STORE, &store_data);
                }

                if (MESH_MSG_SCENE_STORE == pmesh_msg->access_opcode)
                {
                    uint16_t current_scene = get_current_scene(pmodel_info);
                    uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                    delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                    scene_register_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                          pinfo->status_register, current_scene, delay_rsp_time);
                }
            }
        }
        break;
    case MESH_MSG_SCENE_DELETE:
    case MESH_MSG_SCENE_DELETE_UNACK:
        if (pmesh_msg->msg_len == sizeof(scene_delete_t))
        {
            scene_delete_t *pmsg = (scene_delete_t *)pbuffer;
            if (IS_SCENE_NUMBER_VALID(pmsg->scene_number))
            {
                /* find store index */
                scene_setup_info_t *pinfo = pmodel_info->pargs;
                uint16_t index = 0;
                for (index = 0; index < pinfo->num_scenes; ++index)
                {
                    if (pmsg->scene_number == pinfo->scenes[index].scene_number)
                    {
                        break;
                    }
                }

                pinfo->status_register = SCENE_STATUS_SUCCESS;
                if (index < pinfo->num_scenes)
                {
                    /* delete exists */
                    pinfo->scenes[index].scene_number = 0;
                }
                else
                {
                    /* no scene found */
                    pinfo->status_register = SCENE_STATUS_NOT_FOUND;
                }

                if (SCENE_STATUS_SUCCESS == pinfo->status_register)
                {
                    scene_server_delete_t delete_data = {pmsg->scene_number};
                    if (NULL != pmodel_info->model_data_cb)
                    {
                        pmodel_info->model_data_cb(pmodel_info, SCENE_SERVER_DELETE, &delete_data);
                    }
                }

                if (MESH_MSG_SCENE_DELETE == pmesh_msg->access_opcode)
                {
                    uint16_t current_scene = get_current_scene(pmodel_info);
                    uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                    delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                    scene_register_status(pmodel_info, pmesh_msg->src, pmesh_msg->app_key_index,
                                          pinfo->status_register, current_scene, delay_rsp_time);
                }
            }
        }
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

#if MESH_MODEL_ENABLE_DEINIT
static void scene_setup_server_deinit(mesh_model_info_t *pmodel_info)
{
    if (pmodel_info->model_receive == scene_setup_server_receive)
    {
        /* now we can remove */
        if (NULL != pmodel_info->pargs)
        {
            plt_free(pmodel_info->pargs, RAM_TYPE_DATA_ON);
            pmodel_info->pargs = NULL;
        }
        pmodel_info->model_receive = NULL;
    }
}
#endif

bool scene_setup_server_reg(uint8_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_SCENE_SETUP_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->pargs = plt_malloc(sizeof(scene_setup_info_t), RAM_TYPE_DATA_ON);
        if (NULL == pmodel_info->pargs)
        {
            printe("scene_setup_server_reg: fail to allocate memory for the new model extension data!");
            return FALSE;
        }
        memset(pmodel_info->pargs, 0, sizeof(scene_setup_info_t));

        pmodel_info->model_receive = scene_setup_server_receive;
        if (NULL == pmodel_info->model_data_cb)
        {
            printw("scene_server_reg: missing model data process callback!");
        }
#if MESH_MODEL_ENABLE_DEINIT
        pmodel_info->model_deinit = scene_setup_server_deinit;
#endif
    }

    return mesh_model_reg(element_index, pmodel_info);
}


