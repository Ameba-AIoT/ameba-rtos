/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#if defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
#include <stdio.h>
#include <string.h>
#include <osif.h>

#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_gap.h>
#include <rtk_service_config.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gatts.h>
#include "ble_matter_adapter_service.h"

#include <os_msg.h>
#include <os_task.h>
#include <os_mem.h>

#define BLE_MATTER_UUID_SRV     0xFFF6
#define BLE_MATTER_UUID_RX      0x11, 0x9D, 0x9F, 0x42, 0x9C, 0x4F, 0x9F, 0x95, 0x59, 0x45, 0x3D, 0x26, 0xF5, 0x2E, 0xEE, 0x18
#define BLE_MATTER_UUID_TX		0x12, 0x9D, 0x9F, 0x42, 0x9C, 0x4F, 0x9F, 0x95, 0x59, 0x45, 0x3D, 0x26, 0xF5, 0x2E, 0xEE, 0x18
#define BLE_MATTER_UUID_C3		0x04, 0x8F, 0x21, 0x83, 0x8A, 0x74, 0x7D, 0xB8, 0xF2, 0x45, 0x72, 0x87, 0x38, 0x02, 0x63, 0x64

#define RTK_BT_UUID_BLE_MATTER              BT_UUID_DECLARE_16(BLE_MATTER_UUID_SRV)
#define RTK_BT_UUID_BLE_MATTER_RX           BT_UUID_DECLARE_128(BLE_MATTER_UUID_RX)
#define RTK_BT_UUID_BLE_MATTER_TX           BT_UUID_DECLARE_128(BLE_MATTER_UUID_TX)
#define RTK_BT_UUID_BLE_MATTER_C3           BT_UUID_DECLARE_128(BLE_MATTER_UUID_C3)

#define BLE_MATTER_READ_MAX_LEN             60

static uint8_t ble_matter_read_val[BLE_MATTER_READ_MAX_LEN] = {'D', 'e', 'm', 'o', '\0'};
static uint8_t ble_matter_cccd_ind_val[]      = "BLE matter indication data";
static uint8_t ble_matter_cccd_ind_en_map[10] = {0};

static rtk_bt_gatt_attr_t ble_matter_attrs[] = {
    /* Primary Service: simple BLE */
    RTK_BT_GATT_PRIMARY_SERVICE(RTK_BT_UUID_BLE_MATTER),

    /* Characteristic: demo for RX */
    RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_BLE_MATTER_RX,
                               RTK_BT_GATT_CHRC_WRITE | RTK_BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                               RTK_BT_GATT_PERM_WRITE),

    /* Characteristic: demo for TX */
    RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_BLE_MATTER_TX,
                               RTK_BT_GATT_CHRC_INDICATE | RTK_BT_GATT_CHRC_READ,
                               RTK_BT_GATT_PERM_READ),
    RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),

    /* <<Characteristic>> C3 Data TX */
    RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_BLE_MATTER_C3,
                               RTK_BT_GATT_CHRC_READ,
                               RTK_BT_GATT_PERM_READ),
};

static struct rtk_bt_gatt_service ble_matter_srv = RTK_BT_GATT_SERVICE(ble_matter_attrs, BLE_MATTER_ADAPTER_SRV_ID);

void ble_matter_adapter_ble_service_callback(uint8_t event, void *data)
{
    uint32_t i = 0;
    uint16_t ret = 0;

    switch (event) {
    case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
        rtk_bt_gatts_reg_ind_t *p_gatts_reg_ind = (rtk_bt_gatts_reg_ind_t *)data;
        if (p_gatts_reg_ind->reg_status == RTK_BT_OK) {
            DiagPrintf("[APP] ble matter service register succeed!\r\n");
        } else
            DiagPrintf("[APP] ble matter service register failed, err: 0x%x\r\n",
                       p_gatts_reg_ind->reg_status);
        break;
    }
    case RTK_BT_GATTS_EVT_READ_IND: {
        rtk_bt_gatts_read_ind_t *p_read_ind = (rtk_bt_gatts_read_ind_t *)data;
        uint16_t offset = p_read_ind->offset;
        uint16_t actual_len = BLE_MATTER_READ_MAX_LEN - p_read_ind->offset;
        if (BLE_MATTER_SERVICE_C3_INDEX == p_read_ind->index) {
            rtk_bt_gatts_read_resp_param_t read_resp = {
                .app_id   = p_read_ind->app_id,
                .conn_handle  = p_read_ind->conn_handle,
                .cid      = p_read_ind->cid,
                .index    = p_read_ind->index,
                .data     = &ble_matter_read_val[offset],
                .len      = actual_len,
                .err_code = 0,
                .seq      = 0,
            };
            ret = rtk_bt_gatts_read_resp(&read_resp);
            if (RTK_BT_OK == ret) {
                DiagPrintf("[APP] BLE matter respond for client read success, offset: %d\r\n", offset);

                //send data to matter, connected msg require conn_id/length/*p_value
                BLE_MATTER_APP_READ_CB_ARG *read_msg_matter = (BLE_MATTER_APP_READ_CB_ARG *)os_mem_alloc(0, sizeof(BLE_MATTER_APP_READ_CB_ARG));
                if (read_msg_matter) {
                    memset(read_msg_matter, 0, sizeof(BLE_MATTER_APP_READ_CB_ARG));
                    read_msg_matter->pp_value = ble_matter_read_val;
                    read_msg_matter->p_len = 1;
                    if (ble_matter_adapter_send_callback_msg(BLE_MATTER_MSG_READ, NULL, read_msg_matter) == false) {
                        DiagPrintf("[%s]send callback msg fail\r\n", __FUNCTION__);
                        os_mem_free(read_msg_matter);
                        read_msg_matter = NULL;
                    }
                } else {
                    DiagPrintf("[%s]Malloc failed\r\n");
                }
            } else {
                DiagPrintf("[APP] BLE matter respond for client read failed, err: 0x%x\r\n", ret);
            }
        } else if (BLE_MATTER_SERVICE_CHAR_TX_INDEX == p_read_ind->index) {
            rtk_bt_gatts_read_resp_param_t read_resp = {
                .app_id   = p_read_ind->app_id,
                .conn_handle  = p_read_ind->conn_handle,
                .cid      = p_read_ind->cid,
                .index    = p_read_ind->index,
                .data     = &ble_matter_read_val[offset],
                .len      = actual_len,
                .err_code = 0,
                .seq      = 0,
            };
            ret = rtk_bt_gatts_read_resp(&read_resp);
            if (RTK_BT_OK == ret) {
                DiagPrintf("[APP] BLE matter respond for client read success, offset: %d\r\n", offset);
            } else {
                DiagPrintf("[APP] BLE matter respond for client read failed, err: 0x%x\r\n", ret);
            }
        } else {
            DiagPrintf("[APP] BLE matter read event unknown index: %d\r\n", p_read_ind->index);
        }

        break;
    }
    case RTK_BT_GATTS_EVT_WRITE_IND: {
        rtk_bt_gatts_write_ind_t *p_write_ind = (rtk_bt_gatts_write_ind_t *)data;
        rtk_bt_gatts_write_resp_param_t write_resp = {0};
        if (BLE_MATTER_SERVICE_CHAR_RX_INDEX == p_write_ind->index) {
            DiagPrintf("[APP] BLE matter write event, len: %d, type: %d, data: ",
                       p_write_ind->len, p_write_ind->type);
            /*
            for (i = 0; i < p_write_ind->len; i++)
            {
            	if (0 == i%16)
            		DiagPrintf("\n\r");
            	DiagPrintf("%02x ", *(p_write_ind->value+i));
            }
            DiagPrintf("\r\n");
            */

            write_resp.app_id = p_write_ind->app_id;
            write_resp.conn_handle = p_write_ind->conn_handle;
            write_resp.cid = p_write_ind->cid,
            write_resp.index = p_write_ind->index;
            write_resp.type = p_write_ind->type;
            write_resp.err_code = 0;
            write_resp.seq = 0;

            ret = rtk_bt_gatts_write_resp(&write_resp);
            if (RTK_BT_OK == ret) {
                DiagPrintf("[APP] BLE matter response for client write success!\r\n");

                //send data to matter, connected msg require conn_id/length/*p_value
                uint8_t conn_id;
                if (rtk_bt_le_gap_get_conn_id(write_resp.conn_handle, &conn_id) != RTK_BT_OK) {
                    DiagPrintf("[%s]Fail to get conn_id\n");
                } else {
                    BLE_MATTER_APP_WRITE_CB_ARG *write_msg_matter = (BLE_MATTER_APP_WRITE_CB_ARG *)os_mem_alloc(0, sizeof(BLE_MATTER_APP_WRITE_CB_ARG));
                    if (write_msg_matter) {
                        //memset(write_msg_matter, 0, sizeof(BLE_MATTER_APP_WRITE_CB_ARG));
                        write_msg_matter->conn_id = conn_id;
                        write_msg_matter->len = p_write_ind->len;
                        if (write_msg_matter->len != 0) {
                            write_msg_matter->p_value = os_mem_alloc(0, write_msg_matter->len);
                            memcpy(write_msg_matter->p_value, p_write_ind->value, write_msg_matter->len);
                            if (ble_matter_adapter_send_callback_msg(BLE_MATTER_MSG_WRITE_CB, NULL, write_msg_matter) == false) {
                                if (write_msg_matter->len != 0) {
                                    os_mem_free(write_msg_matter->p_value);
                                    write_msg_matter->p_value = NULL;
                                }
                                DiagPrintf("[%s]send callback msg fail\r\n", __FUNCTION__);
                                os_mem_free(write_msg_matter);
                                write_msg_matter = NULL;
                            }
                        } else {
                            DiagPrintf("[%s]write_msg_matter->len is 0, msg failed\r\n");
                        }
                    } else {
                        DiagPrintf("[%s]Malloc failed\r\n");
                    }
                }
            } else {
                DiagPrintf("[APP] BLE matter response for client write failed, err: 0x%x\r\n", ret);
            }
        } else {
            DiagPrintf("[APP] BLE matter write event unknown index: %d\r\n", p_write_ind->index);
        }
        break;
    }
    case RTK_BT_GATTS_EVT_CCCD_IND: {
        rtk_bt_gatts_cccd_ind_t *p_cccd_ind = (rtk_bt_gatts_cccd_ind_t *)data;
        uint8_t conn_id;

        if (rtk_bt_le_gap_get_conn_id(p_cccd_ind->conn_handle, &conn_id) != RTK_BT_OK) {
            break;
        }

        switch (p_cccd_ind->index) {
        case BLE_MATTER_SERVICE_CHAR_INDICATE_CCCD_INDEX:
            if (p_cccd_ind->value & RTK_BT_GATT_CCC_INDICATE) {
                ble_matter_cccd_ind_en_map[conn_id] = 1;
                DiagPrintf("[APP] BLE matter indicate cccd, indicate bit enable\r\n");

                //send data to matter, connected msg require conn_id/indication flag on
                uint8_t conn_id;
                if (rtk_bt_le_gap_get_conn_id(p_cccd_ind->conn_handle, &conn_id) != RTK_BT_OK) {
                    DiagPrintf("[%s]Fail to get conn_id\n");
                } else {
                    BLE_MATTER_APP_CCCD_CB_ARG *ind_en_msg_matter = (BLE_MATTER_APP_CCCD_CB_ARG *)os_mem_alloc(0, sizeof(BLE_MATTER_APP_CCCD_CB_ARG));
                    if (ind_en_msg_matter) {
                        memset(ind_en_msg_matter, 0, sizeof(BLE_MATTER_APP_CCCD_CB_ARG));
                        ind_en_msg_matter->conn_id = conn_id;
                        ind_en_msg_matter->indicationsEnabled = 1;
                        if (ble_matter_adapter_send_callback_msg(BLE_MATTER_MSG_CCCD_EN, NULL, ind_en_msg_matter) == false) {
                            DiagPrintf("[%s]send callback msg fail\r\n", __FUNCTION__);
                            os_mem_free(ind_en_msg_matter);
                            ind_en_msg_matter = NULL;
                        }
                    } else {
                        DiagPrintf("[%s]Malloc failed\r\n");
                    }
                }
            } else {
                ble_matter_cccd_ind_en_map[conn_id] = 0;
                DiagPrintf("[APP] BLE matter indicate cccd, indicate bit disable\r\n");

                //send data to matter, connected msg require conn_id/indication flag off
                uint8_t conn_id;
                if (rtk_bt_le_gap_get_conn_id(p_cccd_ind->conn_handle, &conn_id) != RTK_BT_OK) {
                    DiagPrintf("[%s]Fail to get conn_id\n");
                } else {
                    BLE_MATTER_APP_CCCD_CB_ARG *ind_disable_msg_matter = (BLE_MATTER_APP_CCCD_CB_ARG *)os_mem_alloc(0, sizeof(BLE_MATTER_APP_CCCD_CB_ARG));
                    if (ind_disable_msg_matter) {
                        memset(ind_disable_msg_matter, 0, sizeof(BLE_MATTER_APP_CCCD_CB_ARG));
                        ind_disable_msg_matter->conn_id = conn_id;
                        ind_disable_msg_matter->indicationsEnabled = 0;
                        if (ble_matter_adapter_send_callback_msg(BLE_MATTER_MSG_CCCD_DIS, NULL, ind_disable_msg_matter) == false) {
                            DiagPrintf("[%s]send callback msg fail\r\n", __FUNCTION__);
                            os_mem_free(ind_disable_msg_matter);
                            ind_disable_msg_matter = NULL;
                        }
                    } else {
                        DiagPrintf("[%s]Malloc failed\r\n");
                    }
                }
            }
            break;
        default:
            break;
        }
        break;
    }

    case RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND: {
        rtk_bt_gatts_ntf_and_ind_ind_t *p_ind_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;
        if (RTK_BT_OK == p_ind_ind->err_code) {
            DiagPrintf("[APP] BLE matter indicate succeed!\r\n");

            //send data to matter, connected msg require conn_id/indication flag off
            uint8_t conn_id;
            if (rtk_bt_le_gap_get_conn_id(p_ind_ind->conn_handle, &conn_id) != RTK_BT_OK) {
                DiagPrintf("[%s]Fail to get conn_id\n");
            } else {
                BLE_MATTER_APP_SEND_COMPLETE_CB_ARG *send_complete_msg_matter = (BLE_MATTER_APP_SEND_COMPLETE_CB_ARG *)os_mem_alloc(0,
                        sizeof(BLE_MATTER_APP_SEND_COMPLETE_CB_ARG));
                if (send_complete_msg_matter) {
                    memset(send_complete_msg_matter, 0, sizeof(BLE_MATTER_APP_SEND_COMPLETE_CB_ARG));
                    send_complete_msg_matter->conn_id = conn_id;
                    if (ble_matter_adapter_send_callback_msg(BLE_MATTER_MSG_SEND_COMPLETE, NULL, send_complete_msg_matter) == false) {
                        DiagPrintf("[%s]send callback msg fail\r\n", __FUNCTION__);
                        os_mem_free(send_complete_msg_matter);
                        send_complete_msg_matter = NULL;
                    }
                } else {
                    DiagPrintf("[%s]Malloc failed\r\n");
                }
            }
        } else {
            DiagPrintf("[APP] BLE matter indicate failed, err: 0x%x\r\n", p_ind_ind->err_code);
        }
        break;
    }
    default:
        break;
    }
}

void ble_matter_adapter_srv_cccd_indicate(uint16_t conn_handle)
{
    rtk_bt_gatts_ntf_and_ind_param_t ind_param = {0};
    uint8_t conn_id;

    if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
        return;
    }

    if (ble_matter_cccd_ind_en_map[conn_id]) {
        ind_param.app_id = BLE_MATTER_ADAPTER_SRV_ID;
        ind_param.conn_handle = conn_handle;
        // ind_param.cid = 4;	// choose dynamic created L2CAP channel when EATT is enbaled.
        ind_param.index = BLE_MATTER_SERVICE_CHAR_TX_INDEX;
        ind_param.data = ble_matter_cccd_ind_val;
        ind_param.len = sizeof(ble_matter_cccd_ind_val) / sizeof(uint8_t);
        ind_param.seq = 0;
        rtk_bt_gatts_indicate(&ind_param);
    }
}

uint16_t ble_matter_adapter_srv_add(void)
{
    ble_matter_srv.type = GATT_SERVICE_OVER_BLE;
    ble_matter_srv.server_info = 0;
    ble_matter_srv.user_data = NULL;
    ble_matter_srv.register_status = 0;

    return rtk_bt_gatts_register_service(&ble_matter_srv);
}

void ble_matter_srv_disconnect(uint16_t conn_handle)
{
    uint8_t conn_id;

    if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
        return;
    }

    ble_matter_cccd_ind_en_map[conn_id] = 0;
}

void ble_matter_srv_status_deinit(void)
{
    memset(ble_matter_cccd_ind_en_map, 0, sizeof(ble_matter_cccd_ind_en_map));
}

#endif