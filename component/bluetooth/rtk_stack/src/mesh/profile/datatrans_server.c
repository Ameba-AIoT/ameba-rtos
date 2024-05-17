/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     datatrans_server.c
* @brief    Source file for data transmission server
* @details
* @author   hector_huang
* @date     2018-10-31
* @version  v1.0
* *************************************************************************************
*/
#define MM_ID MM_SERVICE

#include "platform_misc.h"
#include "platform_diagnose.h"
#include "gap.h"
#include "profile_server.h"
#include "datatrans_service.h"
#include "datatrans_server.h"

/** application callback function */
static P_FUN_SERVER_GENERAL_CB datatrans_cb = NULL;

/** @brief  Index of each characteristic in service database. */
#define DATATRANS_DATA_IN_INDEX                 0x02
#define DATATRANS_DATA_OUT_INDEX                0x04
#define DATATRANS_DATA_OUT_CCCD_INDEX           (DATATRANS_DATA_OUT_INDEX + 1)

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
uint8_t datatrans_server_id = SERVICE_PROFILE_GENERAL_ID;
//static uint8_t datatrans_data_out_cccd = GATT_CLIENT_CHAR_CONFIG_DEFAULT;


/** @brief  profile/service definition.  */
const T_ATTRIB_APPL datatrans_server_table[] =
{
    /* <<Primary Service>>, Data transmission service */
    {
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_LE),        /* wFlags     */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
        },
        UUID_128BIT_SIZE,                           /* bValueLen     */
        (void *)GATT_UUID_DATATRANS_SERVICE,        /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

    /* <<Characteristic>>, Data transmission in */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE_NO_RSP             /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* Data transmission data value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(DATATRANS_DATA_IN_UUID),
            HI_WORD(DATATRANS_DATA_IN_UUID)
        },
        0,                                          /* bValueLen, 0 : variable length */
        NULL,
        GATT_PERM_WRITE                             /* wPermissions */
    },

    /* <<Characteristic>>, Data transmission out */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_READ   /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* Data transmission data value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(DATATRANS_DATA_OUT_UUID),
            HI_WORD(DATATRANS_DATA_OUT_UUID)
        },
        0,                                          /* bValueLen, 0 : variable length */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /* Client characteristic configuration */
    {
        (ATTRIB_FLAG_VALUE_INCL |                   /* wFlags */
         ATTRIB_FLAG_CCCD_APPL),
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* wPermissions */
    }
};

/**
 * @brief data transmission server read data callback function
 * @param conn_id[in]: connection link id
 * @param service_id[in]: service id to be read
 * @param attrib_index[in]: attribute index in service
 * @param offset[in]: value read offset from pp_value
 * @param p_length[out]: output value length
 * @param pp_value[out]: output value pointer
 * @return status
 */
static T_APP_RESULT datatrans_server_read_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                             uint16_t attrib_index, uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    UNUSED(conn_id);
    UNUSED(offset);
    T_APP_RESULT ret = APP_RESULT_SUCCESS;

    switch (attrib_index)
    {
    case DATATRANS_DATA_OUT_INDEX:
        if (NULL != datatrans_cb)
        {
            /** get data from specified app */
            datatrans_server_data_t cb_data;
            cb_data.type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
            datatrans_cb(service_id, &cb_data);
            *p_length = cb_data.len;
            *pp_value = cb_data.data;
        }
        break;
    default:
        printe("datatrans_server_read_cb: attribute not fount, index %d", attrib_index);
        ret = APP_RESULT_ATTR_NOT_FOUND;
        break;
    }

    return ret;
}

/**
 * @brief data transmission server write data callback function
 * @param conn_id[in]: connection link id
 * @param service_id[in]: serivce id to be write to
 * @param attrib_index[in]: attribute index in service
 * @param wtire_type[in]: wtite type
 * @param len[in]: data length
 * @param pvalue[in]: data value
 * @param ppost_proc[in]: write done callback
 * @return status
 */
static T_APP_RESULT datatrans_server_write_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                              uint16_t attrib_index,
                                              T_WRITE_TYPE write_type, uint16_t len, uint8_t *pvalue, P_FUN_WRITE_IND_POST_PROC *ppost_proc)
{
    UNUSED(conn_id);
    UNUSED(write_type);
    UNUSED(ppost_proc);
    T_APP_RESULT  ret = APP_RESULT_SUCCESS;
    switch (attrib_index)
    {
    case DATATRANS_DATA_IN_INDEX:
        if (NULL != datatrans_cb)
        {
            /** write data to specified app */
            datatrans_server_data_t cb_data;
            cb_data.type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
            cb_data.len = len;
            cb_data.data = pvalue;

            datatrans_cb(service_id, &cb_data);
        }
        break;
    default:
        printe("datatrans_server_write_cb: attribute not fount, index %d", attrib_index);
        ret = APP_RESULT_ATTR_NOT_FOUND;
        break;
    }
    return ret;
}

bool datatrans_server_notify(uint8_t conn_id, uint8_t *pvalue, uint16_t len)
{
    return server_send_data(conn_id, datatrans_server_id, DATATRANS_DATA_OUT_INDEX, pvalue, len,
                            GATT_PDU_TYPE_NOTIFICATION);
}

/**
 * @brief update cccd bits from stack.
 * @param conn_id[in]: connection link id
 * @param server_id[in]: service id
 * @param attrib_index[in]: attribute index of characteristic data
 * @param cccd_bits[in]: cccd bits from stack.
 */
static void datatrans_server_cccd_update_cb(uint8_t conn_id, T_SERVER_ID server_id,
                                            uint16_t attrib_index,
                                            uint16_t cccd_bits)
{
    UNUSED(conn_id);
    UNUSED(server_id);
    printi("datatrans_server_cccd_update_cb: index = %d, cccd_bits = 0x%x", attrib_index, cccd_bits);
    switch (attrib_index)
    {
    case DATATRANS_DATA_OUT_CCCD_INDEX:
        {
            if (cccd_bits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
            {
                /** enable notification */
                //datatrans_data_out_cccd = GATT_CLIENT_CHAR_CONFIG_NOTIFY;
            }
            else
            {
                /** disable Notification */
                //datatrans_data_out_cccd = GATT_CLIENT_CHAR_CONFIG_DEFAULT;
            }
        }
        break;
    default:
        break;
    }
}

/**
 * @brief service callbacks.
 */
const T_FUN_GATT_SERVICE_CBS datatrans_server_cbs =
{
    .read_attr_cb = datatrans_server_read_cb,  /** read callback function pointer */
    .write_attr_cb = datatrans_server_write_cb, /** write callback function pointer */
    .cccd_update_cb = datatrans_server_cccd_update_cb  /** cccd update callback function pointer */
};

uint8_t datatrans_server_add(void *pcb)
{
    uint8_t server_id;
    if (FALSE == server_add_service(&server_id, (uint8_t *)datatrans_server_table,
                                    sizeof(datatrans_server_table),
                                    datatrans_server_cbs))
    {
        printe("datatrans_server_add: add service id(%d) failed!", datatrans_server_id);
        server_id = SERVICE_PROFILE_GENERAL_ID;
        return server_id;
    }

    datatrans_server_id = server_id;
    datatrans_cb = (P_FUN_SERVER_GENERAL_CB)pcb;
    return datatrans_server_id;
}

