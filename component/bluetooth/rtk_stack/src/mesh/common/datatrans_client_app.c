/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     datatrans_client_app.c
  * @brief    Source file for data transmission client model.
  * @details  Data types and external functions declaration.
  * @author   hector_huang
  * @date     2018-10-30
  * @version  v1.0
  * *************************************************************************************
  */
#include "datatrans_client_app.h"
#include "datatrans_model.h"


mesh_model_info_t datatrans_client;

static int32_t datatrans_client_data(const mesh_model_info_p pmodel_info,
                                     uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case DATATRANS_CLIENT_STATUS:
        {
            datatrans_client_status_t *pdata = pargs;
            data_uart_debug("written %d bytes, status: %d\r\n", pdata->written_len, pdata->status);
        }
        break;
    case DATATRANS_CLIENT_DATA:
        {
            datatrans_client_data_t *pdata = pargs;
            data_uart_debug("read %d data from remote: ", pdata->data_len);
            data_uart_dump(pdata->data, pdata->data_len);
        }
        break;
    default:
        break;
    }

    return 0;
}

void datatrans_client_model_init(void)
{
    /* register data transmission client model */
    datatrans_client.model_data_cb = datatrans_client_data;
    datatrans_client_reg(0, &datatrans_client);
}
