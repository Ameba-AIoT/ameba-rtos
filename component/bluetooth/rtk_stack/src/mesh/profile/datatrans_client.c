/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     datatrans_client.c
* @brief    Source file for data transmission client.
* @details
* @author   hector_huang
* @date     2018-10-31
* @version  v1.0
* *************************************************************************************
*/
#include "mesh_api.h"
#include "datatrans_service.h"
#include "datatrans_client.h"

/** used for CCCD handle discovering in discovery procedure */
typedef enum
{
    DATATRANS_CCCD_DISC_START,
    DATATRANS_CCCD_DISC_DATA_OUT,
    DATATRANS_CCCD_DISC_END
} datatrans_cccd_disc_state_t;

T_CLIENT_ID datatrans_client_id = CLIENT_PROFILE_GENERAL_ID;
/**<  callback used to send data to app from datatrans client layer. */
static P_FUN_GENERAL_APP_CB datatrans_client_app_cb = NULL;
/**<  discovery state indicate which cccd is in discovery. */
static datatrans_cccd_disc_state_t datatrans_cccd_disc_state = DATATRANS_CCCD_DISC_START;
/**<  data transmission service discovered handles */
static uint16_t datatrans_hdl_cache[HDL_DATATRANS_CACHE_LEN];
/**<  datatrans discovery end handle control. */
static datatrans_handle_type_t datatrans_end_handle_idx = HDL_DATATRANS_CACHE_LEN;

bool datatrans_client_handle_set(datatrans_handle_type_t type, uint16_t value)
{
    if (type < HDL_DATATRANS_CACHE_LEN)
    {
        datatrans_hdl_cache[type] = value;
        return TRUE;
    }
    return FALSE;
}

uint16_t datatrans_client_handle_get(datatrans_handle_type_t type)
{
    return datatrans_hdl_cache[type];
}

bool datatrans_client_start_discovery(uint8_t conn_id)
{
    printi("datatrans_client_start_discovery");
    memset(datatrans_hdl_cache, 0, sizeof(datatrans_hdl_cache));
    return (GAP_CAUSE_SUCCESS == client_by_uuid128_srv_discovery(conn_id, datatrans_client_id,
                                                                 (uint8_t *)GATT_UUID_DATATRANS_SERVICE));
}

/**
  * @brief  start the discovery of data transmission characteristics.
  * @retval TRUE: send request to upper stack success.
  * @retval FALSE: send request to upper stack failed.
  */
static bool datatrans_client_start_char_discovery(uint8_t conn_id)
{
    printi("datatrans_client_start_char_discovery");
    return (GAP_CAUSE_SUCCESS == client_all_char_discovery(conn_id, datatrans_client_id,
                                                           datatrans_hdl_cache[HDL_DATATRANS_SRV_START], datatrans_hdl_cache[HDL_DATATRANS_SRV_END]));
}

/**
 * @brief  used internal, start the discovery of data transmission characteristics descriptor.
 * @retval TRUE: request to upper stack success.
 * @retval FALSE: request to upper stack failed.
 */
static bool datatrans_client_start_char_descriptor_discovery(uint16_t start_handle,
                                                             uint16_t end_handle)
{
    uint8_t conn_id = 0;
    printi("datatrans_client_start_char_descriptor_discovery");
    return (client_all_char_descriptor_discovery(conn_id, datatrans_client_id, start_handle,
                                                 end_handle) == GAP_CAUSE_SUCCESS);
}

/**
  * @brief  used internal, switch to the next cccd handle to be discovered.
  * @param  cccd_state: CCCD discovery state.
  */
static void datatrans_client_switch_next_descriptor(datatrans_cccd_disc_state_t *cccd_state)
{
    datatrans_cccd_disc_state_t new_state;
    switch (*cccd_state)
    {
    case DATATRANS_CCCD_DISC_START:
        if (datatrans_hdl_cache[HDL_DATATRANS_DATA_OUT])
        {
            new_state = DATATRANS_CCCD_DISC_DATA_OUT;
        }
        else
        {
            new_state = DATATRANS_CCCD_DISC_END;
        }
        break;
    case DATATRANS_CCCD_DISC_DATA_OUT:
        new_state = DATATRANS_CCCD_DISC_END;
        break;
    default:
        new_state = DATATRANS_CCCD_DISC_END;
        break;
    }

    *cccd_state = new_state;
}

/**
 * @brief called by profile client layer, when discover state of discovery procedure changed.
 * @param conn_id[in]: connection link id
 * @param  disc_state: current service discovery state.
 */
static void datatrans_disc_state_cb(uint8_t conn_id, T_DISCOVERY_STATE disc_state)
{
    bool cb_flag = FALSE;
    bool descriptor_disc_flag = FALSE;
    uint16_t start_handle;
    uint16_t end_handle;
    datatrans_client_cb_data_t cb_data;
    cb_data.cb_type = DATATRANS_CLIENT_CB_TYPE_DISC_STATE;
    printi("datatrans_discover_state_cb: disc_state = %d", disc_state);
    switch (disc_state)
    {
    case DISC_STATE_SRV_DONE:
        if ((0 != datatrans_hdl_cache[HDL_DATATRANS_SRV_START]) ||
            (0 != datatrans_hdl_cache[HDL_DATATRANS_SRV_END]))
        {
            /** indicate that service handle found. start discover characteristic. */
            if (datatrans_client_start_char_discovery(0) == FALSE)
            {
                cb_data.cb_content.disc_state = DATATRANS_DISC_FAIL;
                cb_flag = TRUE;
            }
        }
        else
        {
            /** no datatrans service handle found. discover procedure complete. */
            cb_data.cb_content.disc_state = DATATRANS_DISC_NOT_FOUND;
            cb_flag = TRUE;
        }
        break;
    case DISC_STATE_CHAR_DONE:
        if (datatrans_end_handle_idx < HDL_DATATRANS_CACHE_LEN)
        {
            /** we should store the last char end handle if needed. */
            datatrans_hdl_cache[datatrans_end_handle_idx] = datatrans_hdl_cache[HDL_DATATRANS_SRV_END];
            datatrans_end_handle_idx = HDL_DATATRANS_CACHE_LEN;
        }
        /** find the first descriptor to be discovered. */
        datatrans_cccd_disc_state = DATATRANS_CCCD_DISC_START;
        datatrans_client_switch_next_descriptor(&datatrans_cccd_disc_state);
        break;
    case DISC_STATE_CHAR_DESCRIPTOR_DONE:
        /** find the next descriptor to be discovered. */
        datatrans_client_switch_next_descriptor(&datatrans_cccd_disc_state);
        break;
    case DISC_STATE_FAILED:
        cb_data.cb_content.disc_state = DATATRANS_DISC_FAIL;
        cb_flag = TRUE;
        break;
    default:
        printw("datatrans_client_disc_state_cb: invalid discovery state!");
        break;
    }

    /** switch different char descriptor discovery, if has multi char descriptors. */
    switch (datatrans_cccd_disc_state)
    {
    case DATATRANS_CCCD_DISC_DATA_OUT:
        /** need to discover datatrans notify characteristic descriptor. */
        start_handle = datatrans_hdl_cache[HDL_DATATRANS_DATA_OUT] + 1;
        end_handle = datatrans_hdl_cache[HDL_DATATRANS_DATA_OUT_END];
        descriptor_disc_flag = TRUE;
        break;
    case DATATRANS_CCCD_DISC_END:
        cb_data.cb_content.disc_state = DATATRANS_DISC_DONE;
        cb_flag = TRUE;
        break;
    default:
        /** no need to send char descriptor discovery. */
        break;
    }
    if (descriptor_disc_flag)
    {
        if (datatrans_client_start_char_descriptor_discovery(start_handle, end_handle) == FALSE)
        {
            cb_data.cb_content.disc_state = DATATRANS_DISC_FAIL;
            cb_flag = TRUE;
        }
    }

    /** send discover state to application if needed. */
    if (cb_flag && datatrans_client_app_cb)
    {
        datatrans_client_app_cb(datatrans_client_id, conn_id, &cb_data);
    }
}

/**
 * @brief called by profile client layer, when discover result fetched.
 * @param conn_id[in]: connection link id
 * @param result_type: indicate which type of value discovered in service discovery procedure.
 * @param result_data: value discovered.
 */
static void datatrans_disc_result_cb(uint8_t conn_id, T_DISCOVERY_RESULT_TYPE result_type,
                                     T_DISCOVERY_RESULT_DATA result_data)
{
    UNUSED(conn_id);
    printi("datatrans_discover_result_cb: result_type = %d", result_type);
    switch (result_type)
    {
    case DISC_RESULT_SRV_DATA:
        datatrans_client_handle_set(HDL_DATATRANS_SRV_START, result_data.p_srv_disc_data->att_handle);
        datatrans_client_handle_set(HDL_DATATRANS_SRV_END, result_data.p_srv_disc_data->end_group_handle);
        break;
    case DISC_RESULT_CHAR_UUID16:
        /** when use client api all characteristic discovery */
        if (datatrans_end_handle_idx < HDL_DATATRANS_CACHE_LEN)
        {
            datatrans_hdl_cache[datatrans_end_handle_idx] = result_data.p_char_uuid16_disc_data->decl_handle -
                                                            1;
            datatrans_end_handle_idx = HDL_DATATRANS_CACHE_LEN;
        }
        /** we should inform intrested handles to upper application */
        switch (result_data.p_char_uuid16_disc_data->uuid16)
        {
        case DATATRANS_DATA_IN_UUID:
            datatrans_client_handle_set(HDL_DATATRANS_DATA_IN,
                                        result_data.p_char_uuid16_disc_data->value_handle);
            break;
        case DATATRANS_DATA_OUT_UUID:
            datatrans_client_handle_set(HDL_DATATRANS_DATA_OUT,
                                        result_data.p_char_uuid16_disc_data->value_handle);
            datatrans_end_handle_idx = HDL_DATATRANS_DATA_OUT_END;
            break;
        default:
            /** have no interest on this handle */
            break;
        }
        break;
    case DISC_RESULT_CHAR_DESC_UUID16:
        if (GATT_UUID_CHAR_CLIENT_CONFIG == result_data.p_char_desc_uuid16_disc_data->uuid16)
        {
            /** when use client api_all characteristic descriptor discovery */
            uint16_t temp_handle = result_data.p_char_desc_uuid16_disc_data->handle;
            if ((temp_handle > datatrans_hdl_cache[HDL_DATATRANS_DATA_OUT]) &&
                (temp_handle <= datatrans_hdl_cache[HDL_DATATRANS_DATA_OUT_END]))
            {
                datatrans_client_handle_set(HDL_DATATRANS_DATA_OUT_CCCD, temp_handle);
            }
            else
            {
                /** have no interest in this handle */
            }
        }
        break;
    default:
        printe("datatrans_discover_result_cb: invalid discovery result type!");
        break;
    }
}

/**
 * @brief called by profile client layer, when read request responsed.
 * @param conn_id[in]: connection handle id
 * @param cause[in]: read result cause
 * @param handle[in]: handle of the value in read response.
 * @param value_size[in]: size of the value in read response.
 * @param pvalue[in]: pointer to the value in read response.
 */
static void datatrans_read_result_cb(uint8_t conn_id,  uint16_t cause, uint16_t handle,
                                     uint16_t value_size, uint8_t *p_value)
{
    datatrans_client_cb_data_t cb_data;
    cb_data.cb_type = DATATRANS_CLIENT_CB_TYPE_READ_RESULT;
    cb_data.cb_content.read_result.cause = cause;
    printi("datatrans_client_read_result_cb: cause = %d, handle = 0x%x, size = %d", cause, handle,
           value_size);
    /* if read req success, branch to fetch value and send to application. */
    if (handle == datatrans_hdl_cache[HDL_DATATRANS_DATA_OUT_CCCD])
    {
        cb_data.cb_content.read_result.type = DATATRANS_READ_DATA_OUT_CCCD;
        if ((cause == GAP_SUCCESS) && (value_size == sizeof(uint16_t)))
        {
            uint16_t cccd_value = LE_EXTRN2WORD(p_value);
            cb_data.cb_content.read_result.data.datatrans_data_out_cccd = cccd_value &
                                                                          GATT_CLIENT_CHAR_CONFIG_NOTIFY;
        }
        /** inform application the read result. */
        if (datatrans_client_app_cb)
        {
            datatrans_client_app_cb(datatrans_client_id, conn_id, &cb_data);
        }
    }
    else if (handle == datatrans_hdl_cache[HDL_DATATRANS_DATA_OUT])
    {
        cb_data.cb_content.read_result.type = DATATRANS_READ_DATA_OUT;
        if (cause == GAP_SUCCESS)
        {
            if (value_size > DATATRANS_CLIENT_MAX_DATA_OUT_LEN)
            {
                value_size = DATATRANS_CLIENT_MAX_DATA_OUT_LEN;
                printw("datatrans_read_result_cb: data too long, truncated!");
            }
            cb_data.cb_content.read_result.data.value.len = value_size;
            memcpy(cb_data.cb_content.read_result.data.value.datatrans_data_out, p_value, value_size);
        }
        /** inform application the read result. */
        if (datatrans_client_app_cb)
        {
            datatrans_client_app_cb(datatrans_client_id, conn_id, &cb_data);
        }
    }
}

/**
 * @brief called by profile client layer, when write request complete.
 * @param conn_id[in]: connection link id
 * @param type[in]: write type
 * @param handle[in]: attribute handle to write
 * @param cause[in]: write result cause
 * @param credits[in]: write result credits
 */
static void datatrans_write_result_cb(uint8_t conn_id, T_GATT_WRITE_TYPE type, uint16_t handle,
                                      uint16_t cause, uint8_t credits)
{
    UNUSED(type);
    UNUSED(credits);
    /** if write req success, branch to fetch value and send to application */
    if (handle == datatrans_hdl_cache[HDL_DATATRANS_DATA_IN])
    {
        datatrans_client_cb_data_t cb_data;
        cb_data.cb_type = DATATRANS_CLIENT_CB_TYPE_WRITE_RESULT;
        cb_data.cb_content.write_result.cause = cause;
        cb_data.cb_content.write_result.type = DATATRANS_WRITE_DATA_IN;

        /** inform application the write result */
        if (datatrans_client_app_cb)
        {
            datatrans_client_app_cb(datatrans_client_id, conn_id, &cb_data);
        }
    }
}

/**
 * @brief called by profile client layer, when notification or indication arrived.
 * @param conn_id[in]: connection link id
 * @param notify[in]: TRUE: notify FALSE: indication
 * @param handle[in]: handle of the value in received data.
 * @param len[in]: size of the value in received data.
 * @param pvalue[in]: pointer to the value in received data.
 * @retval APP_RESULT_SUCESS: procedure OK.
 *         other value: procedure exception.
 */
static T_APP_RESULT datatrans_notify_indicate_result_cb(uint8_t conn_id, bool notify,
                                                        uint16_t handle,
                                                        uint16_t value_size, uint8_t *p_value)
{
    T_APP_RESULT app_result = APP_RESULT_REJECT;

    if (notify && (handle == datatrans_hdl_cache[HDL_DATATRANS_DATA_OUT]))
    {
        datatrans_client_cb_data_t cb_data;
        cb_data.cb_type = DATATRANS_CLIENT_CB_TYPE_NOTIF_IND_RESULT;
        cb_data.cb_content.notify_indicate_data.type = DATATRANS_DATA_OUT_NOTIFY;
        if (value_size > DATATRANS_CLIENT_MAX_DATA_OUT_LEN)
        {
            value_size = DATATRANS_CLIENT_MAX_DATA_OUT_LEN;
            printw("datatrans_notify_indicate_result_cb: data too long, truncated!");
        }
        cb_data.cb_content.notify_indicate_data.value.len = value_size;
        memcpy(cb_data.cb_content.notify_indicate_data.value.datatrans_data_out, p_value, value_size);
        /** inform application notify/indication data */
        if (datatrans_client_app_cb)
        {
            datatrans_client_app_cb(datatrans_client_id, conn_id, &cb_data);
        }

        app_result = APP_RESULT_SUCCESS;
    }
    return app_result;
}

/**
 * @brief called by profile client layer, when link disconnected.
 * @param conn_id[in]: connection link id
 */
static void datatrans_disconnect_cb(uint8_t conn_id)
{
    UNUSED(conn_id);
    /* seset some params, when disconnection occurs. */
    datatrans_end_handle_idx = HDL_DATATRANS_CACHE_LEN;
    datatrans_cccd_disc_state = DATATRANS_CCCD_DISC_START;
}

const T_FUN_CLIENT_CBS datatrans_client_cbs =
{
    datatrans_disc_state_cb,   //!< Discovery State callback function pointer
    datatrans_disc_result_cb,  //!< Discovery result callback function pointer
    datatrans_read_result_cb,      //!< Read response callback function pointer
    datatrans_write_result_cb,     //!< Write result callback function pointer
    datatrans_notify_indicate_result_cb,  //!< Notify Indicate callback function pointer
    datatrans_disconnect_cb       //!< Link disconnection callback function pointer
};

bool datatrans_client_read_by_handle(uint8_t conn_id, datatrans_read_type_t type)
{
    uint16_t handle = 0;
    switch (type)
    {
    case DATATRANS_READ_DATA_OUT_CCCD:
        handle = datatrans_hdl_cache[HDL_DATATRANS_DATA_OUT_CCCD];
        break;
    case DATATRANS_READ_DATA_OUT:
        handle = datatrans_hdl_cache[HDL_DATATRANS_DATA_OUT];
        break;
    default:
        break;
    }

    if (handle)
    {
        if (GAP_CAUSE_SUCCESS == client_attr_read(conn_id, datatrans_client_id, handle))
        {
            return TRUE;
        }
    }
    printe("datatrans_client_read_by_handle: request fail! please check!");
    return FALSE;
}

bool datatrans_client_read_by_uuid(uint8_t conn_id, datatrans_read_type_t type)
{
    uint16_t start_handle;
    uint16_t end_handle;
    uint16_t uuid16;
    switch (type)
    {
    case DATATRANS_READ_DATA_OUT_CCCD:
        start_handle = datatrans_hdl_cache[HDL_DATATRANS_DATA_OUT] + 1;
        end_handle = datatrans_hdl_cache[HDL_DATATRANS_DATA_OUT_END];
        uuid16 = GATT_UUID_CHAR_CLIENT_CONFIG;
        break;
    case DATATRANS_READ_DATA_OUT:
        start_handle = datatrans_hdl_cache[HDL_DATATRANS_DATA_OUT];
        end_handle = datatrans_hdl_cache[HDL_DATATRANS_DATA_OUT_END] - 1;
        uuid16 = DATATRANS_DATA_OUT_UUID;
        break;
    default:
        break;
    }

    if (start_handle &&
        (GAP_CAUSE_SUCCESS == client_attr_read_using_uuid(conn_id, datatrans_client_id, start_handle,
                                                          end_handle, uuid16, NULL)))
    {
        return TRUE;
    }
    printe("datatrans_client_read_by_uuid: request fail! please check!");
    return FALSE;
}

bool datatrans_client_data_out_cccd_set(uint8_t conn_id, bool command)
{
    uint16_t handle;
    handle = datatrans_hdl_cache[HDL_DATATRANS_DATA_OUT_CCCD];
    if (handle)
    {
        uint16_t cccd_value = command ? 1 : 0;
        uint16_t length = sizeof(uint16_t);
        uint8_t *pdata = (uint8_t *)&cccd_value;
        if (GAP_CAUSE_SUCCESS == client_attr_write(conn_id, datatrans_client_id, GATT_WRITE_TYPE_REQ,
                                                   handle, length,
                                                   pdata))
        {
            return TRUE;
        }
    }

    printe("datatrans_client_data_out_cccd_set: request fail! please check!");
    return FALSE;
}

bool datatrans_client_data_in_write(uint8_t conn_id, uint8_t *pdata, uint16_t length)
{
    uint16_t handle;
    handle = datatrans_hdl_cache[HDL_DATATRANS_DATA_IN];
    if (handle &&
        (GAP_CAUSE_SUCCESS == client_attr_write(conn_id, datatrans_client_id, GATT_WRITE_TYPE_CMD, handle,
                                                length, pdata)))
    {
        return TRUE;
    }
    printe("datatrans_client_data_in_write: request fail! please check!");
    return FALSE;
}

T_CLIENT_ID datatrans_client_add(P_FUN_GENERAL_APP_CB app_cb)
{
    T_CLIENT_ID client_id;
    if (FALSE == client_register_spec_client_cb(&client_id, &datatrans_client_cbs))
    {
        client_id = CLIENT_PROFILE_GENERAL_ID;
        printe("datatrans_client_add: fail!");
        return client_id;
    }
    datatrans_client_id = client_id;
    /* register callback for profile to inform application that some events happened. */
    datatrans_client_app_cb = app_cb;
    return datatrans_client_id;
}
