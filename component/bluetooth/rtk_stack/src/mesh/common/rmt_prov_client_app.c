/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rm_prov_client_app.c
* @brief     remote provision application
* @details
* @author    hector huang
* @date      2020-11-30
* @version   v1.0
* *********************************************************************************************************
*/

#include "mesh_api.h"
#include "rmt_prov_client_app.h"
#include "remote_provisioning.h"
#include "generic_types.h"
#include "app_mesh_flags.h"

#if F_BT_MESH_1_1_RPR_SUPPORT

static int32_t rmt_prov_client_data(const mesh_model_info_p pmodel_info, uint32_t type,
                                    void *pargs)
{
    int32_t ret = MODEL_SUCCESS;
    switch (type)
    {
    case RMT_PROV_CLIENT_SCAN_CAPS_STATUS:
        {
            rmt_prov_client_scan_caps_status_t *pdata = (rmt_prov_client_scan_caps_status_t *)pargs;
            data_uart_debug("rmt_prov_scan_caps_status: src 0x%04x, max scanned items %d, support active scan %d\r\n",
                            pdata->src, pdata->max_scanned_items, pdata->support_active_scan);
        }
        break;
    case RMT_PROV_CLIENT_SCAN_STATUS:
        {
            rmt_prov_client_scan_status_t *pdata = (rmt_prov_client_scan_status_t *)pargs;
            data_uart_debug("rmt_prov_scan_status: src 0x%04x, status %d, scan state %d, scanned items limit %d, timeout %d\r\n",
                            pdata->src, pdata->status, pdata->scan_state, pdata->scanned_items_limit, pdata->timeout);
        }
        break;
    case RMT_PROV_CLIENT_SCAN_REPORT:
        {
            rmt_prov_client_scan_report_t *pdata = (rmt_prov_client_scan_report_t *)pargs;
            uint16_t oob = 0xffff;
            if (pdata->poob)
            {
                oob = (pdata->poob[1] << 8) + pdata->poob[0];
            }
            uint32_t uri_hash = 0;
            if (pdata->puri_hash)
            {
                uri_hash = (pdata->puri_hash[3] << 24) + (pdata->puri_hash[2] << 16) +
                           (pdata->puri_hash[1] << 8) + pdata->puri_hash[0];
            }
            data_uart_debug("rmt_prov_scan_report: rssi %d, oob %d, uri hash %d, uuid ",
                            pdata->rssi, oob, uri_hash);
            data_uart_dump(pdata->uuid, 16);
        }
        break;
    case RMT_PROV_CLIENT_EXTENDED_SCAN_REPORT:
        {
            rmt_prov_client_extended_scan_report_t *pdata = (rmt_prov_client_extended_scan_report_t *)pargs;
            uint16_t oob = 0xffff;
            if (pdata->poob)
            {
                oob = (pdata->poob[1] << 8) + pdata->poob[0];
            }
            data_uart_debug("rmt_prov_extended_scan_report: oob %d, uuid ", oob);
            data_uart_dump(pdata->uuid, 16);
            if (pdata->adv_structs_len > 0)
            {
                data_uart_debug("rmt_prov_extended_scan_report: adv structs ");
                data_uart_dump(pdata->padv_structs, pdata->adv_structs_len);
            }
        }
        break;
    case RMT_PROV_CLIENT_LINK_STATUS:
        {
            rmt_prov_client_link_status_t *pdata = (rmt_prov_client_link_status_t *)pargs;
            data_uart_debug("rmt_prov_link_status: src 0x%04x, status %d, link state %d\r\n",
                            pdata->src, pdata->status, pdata->link_state);
        }
        break;
    case RMT_PROV_CLIENT_LINK_REPORT:
        {
            rmt_prov_client_link_report_t *pdata = (rmt_prov_client_link_report_t *)pargs;
            data_uart_debug("rmt_prov_link_report: src 0x%04x, status %d, link state %d, reason %d\r\n",
                            pdata->src, pdata->status, pdata->link_state, pdata->preason ? *pdata->preason : -1);
        }
        break;
    default:
        break;
    }

    return ret;
}

void rmt_prov_client_init(void)
{
    /* register remote client model */
    rmt_prov_client_reg(0, rmt_prov_client_data);
}
#endif
