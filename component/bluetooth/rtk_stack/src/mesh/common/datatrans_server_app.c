/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     datatrans_server_app.c
  * @brief    Source file for data transmission server model.
  * @details  Data types and external functions declaration.
  * @author   hector_huang
  * @date     2018-10-29
  * @version  v1.0
  * *************************************************************************************
  */
#include "datatrans_server_app.h"
#include "datatrans_model.h"


static mesh_model_info_t datatrans_server;

static uint8_t sample_data[16];

static int32_t datatrans_server_data(const mesh_model_info_p pmodel_info,
                                     uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    switch (type)
    {
    case DATATRANS_SERVER_WRITE:
        {
            datatrans_server_write_t *pdata = pargs;
            data_uart_debug("remote write %d bytes: ", pdata->data_len);
            data_uart_dump(pdata->data, pdata->data_len);
        }
        break;
    case DATATRANS_SERVER_READ:
        {
            datatrans_server_read_t *pdata = pargs;
            if (pdata->data_len > 16)
            {
                pdata->data_len = 16;
            }

            for (uint8_t i = 0; i < pdata->data_len; ++i)
            {
                sample_data[i] = i;
            }
            pdata->data = sample_data;
        }
        break;
    default:
        break;
    }

    return 0;
}

void datatrans_server_model_init(void)
{
    /* register data transmission server model */
    datatrans_server.model_data_cb = datatrans_server_data;
    datatrans_server_reg(0, &datatrans_server);
}
