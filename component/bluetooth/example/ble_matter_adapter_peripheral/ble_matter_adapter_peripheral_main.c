/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#if defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
/*============================================================================*
 *                              Header
 *============================================================================*/
#include <string.h>
#include <stdio.h>
#include <osif.h>
#include <os_msg.h>
#include <os_task.h>
#include <os_mem.h>

#include <bt_api_config.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_device.h>
#include <rtk_bt_gap.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gatts.h>
#include <bt_utils.h>

#include "matter_blemgr_common.h"
#include "ble_matter_adapter_service.h"

/*============================================================================*
 *                              Constants
 *============================================================================*/
#define dbg_printf	//DiagPrintf
#define RTK_BT_DEV_NAME "BLE_ADAPTER"

#define BT_APP_PROCESS(func)                                \
	do {                                                    \
		uint16_t __func_ret = func;                         \
		if (RTK_BT_OK != __func_ret) {                      \
			DiagPrintf("[APP] %s failed!!! line: %d, err: 0x%x\r\n", __func__, __LINE__, __func_ret);   \
			return -1;                                      \
		}                                                   \
	} while (0)

#define BLE_MATTER_ADAPTER_CALLBACK_TASK_PRIORITY	4
#define BLE_MATTER_ADAPTER_CALLBACK_TASK_STACK_SIZE	256 * 6
#define BLE_MATTER_ADAPTER_MAX_NUMBER_OF_CALLBACK_MESSAGE	0x20

void *ble_matter_adapter_callback_task_handle = NULL;
void *ble_matter_adapter_callback_queue_handle = NULL;
/*============================================================================*
 *                              Variables
 *============================================================================*/
static uint8_t ble_matter_adv_data[31] = {0};
static uint8_t ble_matter_adv_data_length = 0;

static rtk_bt_le_adv_param_t adv_param = {
    .interval_min = 0x20, //units of 0.625ms
    .interval_max = 0x20,
    .type = RTK_BT_LE_ADV_TYPE_IND,
    .own_addr_type = RTK_BT_LE_ADDR_TYPE_PUBLIC,
    .peer_addr = {
        .type = (rtk_bt_le_addr_type_t)0,
        .addr_val = {0},
    },
    .channel_map = RTK_BT_LE_ADV_CHNL_ALL,
    .filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static rtk_bt_le_security_param_t sec_param = {
    .io_cap = RTK_IO_CAP_NO_IN_NO_OUT,
    .oob_data_flag = 0,
    .bond_flag = 1,
    .mitm_flag = 0,
    .sec_pair_flag = 0,
    .use_fixed_key = 0,
    .fixed_key = 000000,
};

typedef struct {
    uint16_t type;
    uint16_t subtype;
    union {
        uint32_t param;
        void *buf;
    } u
} T_MATTER_CB_MSG;

extern matter_blemgr_callback matter_blemgr_callback_func;
extern void *matter_blemgr_callback_data;

/*============================================================================*
 *                              Functions
 *============================================================================*/
void ble_matter_adapter_callback_main_task(void *p_param)
{
    (void)p_param;
    T_MATTER_CB_MSG callback_msg;
    os_msg_queue_create(&ble_matter_adapter_callback_queue_handle, BLE_MATTER_ADAPTER_MAX_NUMBER_OF_CALLBACK_MESSAGE, sizeof(T_MATTER_CB_MSG));

    while (true) {
        if (os_msg_recv(ble_matter_adapter_callback_queue_handle, &callback_msg, 0xFFFFFFFF) == true) {
            ble_matter_adapter_app_handle_callback_msg(callback_msg);
        }
    }
}

void ble_matter_adapter_callback_task_init(void)
{
    os_task_create(&ble_matter_adapter_callback_task_handle, "ble_matter_adapter_callback", ble_matter_adapter_callback_main_task, 0,
                   BLE_MATTER_ADAPTER_CALLBACK_TASK_STACK_SIZE, BLE_MATTER_ADAPTER_CALLBACK_TASK_PRIORITY);
}

bool ble_matter_adapter_send_callback_msg(uint16_t msg_type, uint16_t cb_type, void *arg)
{
    T_MATTER_CB_MSG callback_msg;
    callback_msg.type = msg_type;

    callback_msg.u.buf = arg;
    if (ble_matter_adapter_callback_queue_handle != NULL) {
        if (os_msg_send(ble_matter_adapter_callback_queue_handle, &callback_msg, 0) == false) {
            DiagPrintf("bt matter adapter callback send msg fail: type 0x%x\n", callback_msg.type);
            return false;
        }
        return true;
    }
    return false;
}

void ble_matter_adapter_app_handle_callback_msg(T_MATTER_CB_MSG callback_msg)
{
    uint16_t msg_type = callback_msg.type;
    switch (msg_type) {
    case BLE_MATTER_MSG_CONNECTED: {
        BLE_MATTER_APP_CONNECT_CB_ARG *matter_msg_connected = callback_msg.u.buf;
        T_MATTER_BLEMGR_GAP_CONNECT_CB_ARG gap_connect_cb_arg;
        gap_connect_cb_arg.conn_id = matter_msg_connected->conn_id;
        dbg_printf("[MATTER]Get conn_id %d from case BLE_MATTER_MSG_CONNECTED\n", gap_connect_cb_arg.conn_id);
        if (matter_blemgr_callback_func) {
            matter_blemgr_callback_func(matter_blemgr_callback_data, MATTER_BLEMGR_GAP_CONNECT_CB, &gap_connect_cb_arg);
        }
        os_mem_free(callback_msg.u.buf);
        callback_msg.u.buf = NULL;
    }
    break;

    case BLE_MATTER_MSG_DISCONNECTED: {
        BLE_MATTER_APP_DISCONNECT_CB_ARG *matter_msg_disconnected = callback_msg.u.buf;
        T_MATTER_BLEMGR_GAP_DISCONNECT_CB_ARG gap_disconnect_cb_arg;
        gap_disconnect_cb_arg.conn_id = matter_msg_disconnected->conn_id;
        gap_disconnect_cb_arg.disc_cause = matter_msg_disconnected->disc_cause;
        dbg_printf("[MATTER]Get conn_id %d from case BLE_MATTER_MSG_DISCONNECTED\n", gap_disconnect_cb_arg.conn_id);
        dbg_printf("[MATTER]Get disconnection cause %d\n", gap_disconnect_cb_arg.disc_cause);
        if (matter_blemgr_callback_func) {
            matter_blemgr_callback_func(matter_blemgr_callback_data, MATTER_BLEMGR_GAP_DISCONNECT_CB, &gap_disconnect_cb_arg);
        }
        os_mem_free(callback_msg.u.buf);
        callback_msg.u.buf = NULL;
    }
    break;

    case BLE_MATTER_MSG_WRITE_CB: {
        BLE_MATTER_APP_WRITE_CB_ARG *matter_msg_write = callback_msg.u.buf;
        T_MATTER_BLEMGR_RX_CHAR_WRITE_CB_ARG gap_write_cb_arg;
        gap_write_cb_arg.conn_id = matter_msg_write->conn_id;
        gap_write_cb_arg.p_value = matter_msg_write->p_value;
        gap_write_cb_arg.len = matter_msg_write->len;
        dbg_printf("[MATTER]Get conn_id %d from case BLE_MATTER_MSG_WRITE_CB\n", gap_write_cb_arg.conn_id);
        dbg_printf("[MATTER]BLE matter write event, len: %d, data: ", gap_write_cb_arg.len);
        for (int i = 0; i < gap_write_cb_arg.len; i++) {
            if (0 == i % 16) {
                dbg_printf("\n\r");
            }
            dbg_printf("%02x ", *(gap_write_cb_arg.p_value + i));
        }
        dbg_printf("\r\n");
        if (matter_blemgr_callback_func) {
            matter_blemgr_callback_func(matter_blemgr_callback_data, MATTER_BLEMGR_RX_CHAR_WRITE_CB, &gap_write_cb_arg);
        }
        os_mem_free(callback_msg.u.buf);
        callback_msg.u.buf = NULL;
    }
    break;

    case BLE_MATTER_MSG_CCCD_EN: {
        BLE_MATTER_APP_CCCD_CB_ARG *matter_msg_cccd = callback_msg.u.buf;
        T_MATTER_BLEMGR_TX_CHAR_CCCD_WRITE_CB_ARG gap_cccd_cb_arg;
        gap_cccd_cb_arg.conn_id = matter_msg_cccd->conn_id;
        gap_cccd_cb_arg.indicationsEnabled = matter_msg_cccd->indicationsEnabled;
        gap_cccd_cb_arg.notificationsEnabled = matter_msg_cccd->notificationsEnabled;
        dbg_printf("[MATTER]Get conn_id %d from case BLE_MATTER_MSG_CCCD_EN\n", gap_cccd_cb_arg.conn_id);
        dbg_printf("[MATTER]Get indicationsEnabled %d\n", gap_cccd_cb_arg.indicationsEnabled);
        dbg_printf("[MATTER]Get notificationsEnabled %d\n", gap_cccd_cb_arg.notificationsEnabled);
        if (matter_blemgr_callback_func) {
            matter_blemgr_callback_func(matter_blemgr_callback_data, MATTER_BLEMGR_TX_CHAR_CCCD_WRITE_CB, &gap_cccd_cb_arg);
        }
        os_mem_free(callback_msg.u.buf);
        callback_msg.u.buf = NULL;
    }
    break;

    case BLE_MATTER_MSG_READ: {
        BLE_MATTER_APP_READ_CB_ARG *matter_msg_read = callback_msg.u.buf;
        T_MATTER_BLEMGR_C3_CHAR_READ_CB_ARG gap_read_cb_arg;
        gap_read_cb_arg.pp_value = matter_msg_read->pp_value;
        gap_read_cb_arg.p_len = matter_msg_read->p_len;
        dbg_printf("[MATTER]Get length %d from case BLE_MATTER_MSG_READ\n", *(gap_read_cb_arg.p_len));
        dbg_printf("[MATTER]BLE matter read event, data: ");
        for (int i = 0; i < * (gap_read_cb_arg.p_len); i++) {
            if (0 == i % 16) {
                dbg_printf("\n\r");
            }
            dbg_printf("%02x ", matter_msg_read->pp_value + i);
        }
        dbg_printf("\r\n");
        if (matter_blemgr_callback_func) {
            matter_blemgr_callback_func(matter_blemgr_callback_data, MATTER_BLEMGR_RX_CHAR_WRITE_CB, &gap_read_cb_arg);
        }
        os_mem_free(callback_msg.u.buf);
        callback_msg.u.buf = NULL;
    }
    break;

    case BLE_MATTER_MSG_CCCD_DIS:
    case BLE_MATTER_MSG_SEND_COMPLETE: {
        BLE_MATTER_APP_SEND_COMPLETE_CB_ARG *matter_msg_complete = callback_msg.u.buf;
        T_MATTER_BLEMGR_TX_COMPLETE_CB_ARG gap_complete_cb_arg;
        gap_complete_cb_arg.conn_id = matter_msg_complete->conn_id;
        dbg_printf("[MATTER]Get conn_id %d from case BLE_MATTER_MSG_SEND_COMPLETE\n", gap_complete_cb_arg.conn_id);
        if (matter_blemgr_callback_func) {
            matter_blemgr_callback_func(matter_blemgr_callback_data, MATTER_BLEMGR_TX_COMPLETE_CB, &gap_complete_cb_arg);
        }
        os_mem_free(callback_msg.u.buf);
        callback_msg.u.buf = NULL;
    }
    break;

    default:
        DiagPrintf("[%s] unknow type(%d) callback msg \r\n", __FUNCTION__, callback_msg.type);
        break;
    }
}

#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
#define PRIVACY_USE_DIR_ADV_WHEN_BONDED    0
static bool privacy_enable = false;
static bool privacy_whitelist = true;
static uint8_t privacy_irk[RTK_BT_LE_GAP_IRK_LEN] = "abcdef0123456789";
#endif

static void app_server_disconnect(uint16_t conn_handle)
{
    ble_matter_srv_disconnect(conn_handle);
}

static void app_server_deinit(void)
{
    ble_matter_srv_status_deinit();
}

static rtk_bt_evt_cb_ret_t peripheral_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
    (void)param;
    (void)len;
    rtk_bt_evt_cb_ret_t ret = RTK_BT_EVT_CB_OK;

    switch (evt_code) {
#if defined(RTK_BT_5_2_L2C_ECFC_SUPPORT) && RTK_BT_5_2_L2C_ECFC_SUPPORT
    case RTK_BT_GAP_EVT_ECFC_CONN_CMPL_IND: {
        char addr_str[40] = {0};
        rtk_bt_ecfc_conn_cmpl_ind_t *p_ind = (rtk_bt_ecfc_conn_cmpl_ind_t *)param;
        rtk_bt_addr_to_str(p_ind->bd_type, p_ind->bd_addr, addr_str, 40);
        if (p_ind->err) {
            DiagPrintf("[APP] ECFC connection established fail(0x%x). %s\r\n", p_ind->err, addr_str);
        } else {
            DiagPrintf("[APP] ECFC connection established OK. %s\r\n", addr_str);
            DiagPrintf("[APP] proto_id(%d) conn_handle(%d) remote_mtu(%d) local_mtu(%d) local_mps(%d)\r\n",
                       p_ind->proto_id, p_ind->conn_handle, p_ind->remote_mtu, p_ind->local_mtu, p_ind->local_mps);
            rtk_bt_gap_ecfc_print_cid(p_ind->cid, p_ind->cid_num);
        }
        break;
    }
    case RTK_BT_GAP_EVT_ECFC_DATA_IND: {
        rtk_bt_ecfc_data_ind_t *p_ind = (rtk_bt_ecfc_data_ind_t *)param;
        DiagPrintf("[APP] ECFC data received. proto_id(%d) conn_handle(%d) cid(%d) length(%d)\r\n",
                   p_ind->proto_id, p_ind->conn_handle, p_ind->cid, p_ind->length);
        break;
    }
    case RTK_BT_GAP_EVT_ECFC_CONN_REQ_IND: {
        char addr_str[40] = {0};
        rtk_bt_ecfc_conn_req_ind_t *p_ind = (rtk_bt_ecfc_conn_req_ind_t *)param;
        rtk_bt_addr_to_str(p_ind->bd_type, p_ind->bd_addr, addr_str, 40);
        DiagPrintf("[APP] ECFC connection requset. %s\r\n", addr_str);
        DiagPrintf("[APP] proto_id(%d) conn_handle(%d) remote_mtu(%d) identity_id(%d)\r\n",
                   p_ind->proto_id, p_ind->conn_handle, p_ind->remote_mtu, p_ind->identity_id);
        rtk_bt_gap_ecfc_print_cid(p_ind->cid, p_ind->cid_num);
        break;
    }
    case RTK_BT_GAP_EVT_ECFC_DISCONN_IND: {
        rtk_bt_ecfc_disconn_ind_t *p_ind = (rtk_bt_ecfc_disconn_ind_t *)param;
        DiagPrintf("[APP] ECFC disconnect. proto_id(%d) conn_handle(%d) cid(%d) cause(0x%x)\r\n",
                   p_ind->proto_id, p_ind->conn_handle, p_ind->cid, p_ind->cause);
        break;
    }
    case RTK_BT_GAP_EVT_ECFC_RECONF_REQ_IND: { //BT sync api shall not be called here
        rtk_bt_ecfc_reconf_req_ind_t *p_ind = (rtk_bt_ecfc_reconf_req_ind_t *)param;
        DiagPrintf("[APP] ECFC reconfigure requset. proto_id(%d) conn_handle(%d) remote_mtu(%d) remote_mps(%d)\r\n",
                   p_ind->proto_id, p_ind->conn_handle, p_ind->remote_mtu, p_ind->remote_mps);
        rtk_bt_gap_ecfc_print_cid(p_ind->cid, p_ind->cid_num);
        if (p_ind->remote_mtu < RTK_BT_GAP_ECFC_MIN_MTU || p_ind->remote_mps < RTK_BT_GAP_ECFC_MIN_MPS) {
            ret = RTK_BT_EVT_CB_REJECT;
            DiagPrintf("[APP] Reject!\r\n");
        } else {
            ret = RTK_BT_EVT_CB_OK;
            DiagPrintf("[APP] Accept!\r\n");
        }
        break;
    }
    case RTK_BT_GAP_EVT_ECFC_RECONF_RSP_IND: {
        rtk_bt_ecfc_reconf_rsp_ind_t *p_ind = (rtk_bt_ecfc_reconf_rsp_ind_t *)param;
        if (p_ind->cause) {
            DiagPrintf("[APP] ECFC reconfigure fail\r\n");
        } else {
            DiagPrintf("[APP] ECFC reconfigure OK, local_mtu(%d) local mps(%d)\r\n",
                       p_ind->local_mtu, p_ind->local_mps);
            rtk_bt_gap_ecfc_print_cid(p_ind->cid, p_ind->cid_num);
        }
        break;
    }
#endif
    default:
        DiagPrintf("[APP] Unkown common gap cb evt type: %d\r\n", evt_code);
        break;
    }

    return ret;
}

static rtk_bt_evt_cb_ret_t ble_peripheral_gap_app_callback(uint8_t evt_code, void *param, uint32_t len)
{
    (void)len;
    char le_addr[30] = {0};
    char *role;

    switch (evt_code) {
    case RTK_BT_LE_GAP_EVT_ADV_START_IND: {
        rtk_bt_le_adv_start_ind_t *adv_start_ind = (rtk_bt_le_adv_start_ind_t *)param;
        if (!adv_start_ind->err) {
            DiagPrintf("[APP] ADV started: adv_type %d  \r\n", adv_start_ind->adv_type);
        } else {
            DiagPrintf("[APP] ADV start failed, err 0x%x \r\n", adv_start_ind->err);
        }
        break;
    }

    case RTK_BT_LE_GAP_EVT_ADV_STOP_IND: {
        rtk_bt_le_adv_stop_ind_t *adv_stop_ind = (rtk_bt_le_adv_stop_ind_t *)param;
        if (!adv_stop_ind->err) {
            DiagPrintf("[APP] ADV stopped: reason 0x%x \r\n", adv_stop_ind->stop_reason);
        } else {
            DiagPrintf("[APP] ADV stop failed, err 0x%x \r\n", adv_stop_ind->err);
        }
        break;
    }

#if defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT
    case RTK_BT_LE_GAP_EVT_EXT_ADV_IND: {
        rtk_bt_le_ext_adv_ind_t *ext_adv_ind = (rtk_bt_le_ext_adv_ind_t *)param;
        if (!ext_adv_ind->err) {
            if (ext_adv_ind->is_start) {
                DiagPrintf("[APP] Ext ADV(%d) started\r\n", ext_adv_ind->adv_handle);
            } else {
                DiagPrintf("[APP] Ext ADV(%d) stopped: reason 0x%x \r\n", ext_adv_ind->adv_handle, ext_adv_ind->stop_reason);
            }
        } else {
            if (ext_adv_ind->is_start) {
                DiagPrintf("[APP] Ext ADV(%d) started failed, err 0x%x\r\n", ext_adv_ind->adv_handle, ext_adv_ind->err);
            } else {
                DiagPrintf("[APP] Ext ADV(%d) stopped failed, err 0x%x\r\n", ext_adv_ind->adv_handle, ext_adv_ind->err);
            }
        }
        break;
    }
#endif

#if defined(RTK_BLE_5_0_PA_ADV_SUPPORT) && RTK_BLE_5_0_PA_ADV_SUPPORT
    case RTK_BT_LE_GAP_EVT_PA_IND: {
        rtk_bt_le_pa_ind_t *pa_ind = (rtk_bt_le_pa_ind_t *)param;
        if (pa_ind->state == RTK_BT_LE_PA_STATE_IDLE) {
            if (pa_ind->cause) {
                DiagPrintf("[APP] Periodic ADV(%d) stopped failed, err 0x%x\r\n", pa_ind->adv_handle, pa_ind->cause);
            } else {
                DiagPrintf("[APP] Periodic ADV(%d) stopped\r\n", pa_ind->adv_handle);
            }
        } else if (pa_ind->state == RTK_BT_LE_PA_STATE_ADVERTISING) {
            if (pa_ind->cause) {
                DiagPrintf("[APP] Periodic ADV(%d) started failed, err 0x%x\r\n", pa_ind->adv_handle, pa_ind->cause);
            } else {
                DiagPrintf("[APP] Periodic ADV(%d) started\r\n", pa_ind->adv_handle);
            }
        } else if (pa_ind->state == RTK_BT_LE_PA_STATE_WAIT_EXT_ADV_ADVERTISING) {
            DiagPrintf("[APP] Periodic ADV(%d) waiting for ext adv start, cause 0x%x\r\n", pa_ind->adv_handle, pa_ind->cause);
        }
        break;
    }
#endif

#if 1 /* The role peripheral, no scan function, just for test here */
    case RTK_BT_LE_GAP_EVT_SCAN_START_IND: {
        rtk_bt_le_scan_start_ind_t *scan_start_ind = (rtk_bt_le_scan_start_ind_t *)param;
        if (!scan_start_ind->err) {
            DiagPrintf("[APP] Scan started, scan_type: %d\r\n", scan_start_ind->scan_type);
        } else {
            DiagPrintf("[APP] Scan start failed(err: 0x%x)\r\n", scan_start_ind->err);
        }
        break;
    }

    case RTK_BT_LE_GAP_EVT_SCAN_RES_IND: {
        rtk_bt_le_scan_res_ind_t *scan_res_ind = (rtk_bt_le_scan_res_ind_t *)param;
        rtk_bt_le_addr_to_str(&(scan_res_ind->adv_report.addr), le_addr, sizeof(le_addr));
        DiagPrintf("[APP] Scan info, [Device]: %s, AD evt type: %d, RSSI: %i\r\n",
                   le_addr, scan_res_ind->adv_report.evt_type, scan_res_ind->adv_report.rssi);
        break;
    }

    case RTK_BT_LE_GAP_EVT_SCAN_STOP_IND: {
        rtk_bt_le_scan_stop_ind_t *scan_stop_ind = (rtk_bt_le_scan_stop_ind_t *)param;
        if (!scan_stop_ind->err) {
            DiagPrintf("[APP] Scan stopped, reason: 0x%x\r\n", scan_stop_ind->stop_reason);
        } else {
            DiagPrintf("[APP] Scan stop failed(err: 0x%x)\r\n", scan_stop_ind->err);
        }
        break;
    }
#endif

    case RTK_BT_LE_GAP_EVT_CONNECT_IND: {
        rtk_bt_le_conn_ind_t *conn_ind = (rtk_bt_le_conn_ind_t *)param;
        rtk_bt_le_addr_to_str(&(conn_ind->peer_addr), le_addr, sizeof(le_addr));
        if (!conn_ind->err) {
            role = conn_ind->role ? "slave" : "master";
            DiagPrintf("[APP] Connected, handle: %d, role: %s, remote device: %s\r\n",
                       conn_ind->conn_handle, role, le_addr);

            //send data to matter, connected msg require conn_id only
            uint8_t conn_id;
            if (rtk_bt_le_gap_get_conn_id(conn_ind->conn_handle, &conn_id) != RTK_BT_OK) {
                DiagPrintf("[%s]Fail to get conn_id\n");
            } else {
                BLE_MATTER_APP_CONNECT_CB_ARG *conn_msg_matter = (BLE_MATTER_APP_CONNECT_CB_ARG *)os_mem_alloc(0, sizeof(T_MATTER_BLEMGR_GAP_CONNECT_CB_ARG));
                if (conn_msg_matter) {
                    memset(conn_msg_matter, 0, sizeof(BLE_MATTER_APP_CONNECT_CB_ARG));
                    conn_msg_matter->conn_id = conn_id;
                    if (ble_matter_adapter_send_callback_msg(BLE_MATTER_MSG_CONNECTED, NULL, conn_msg_matter) == false) {
                        DiagPrintf("[%s]send callback msg fail\r\n", __FUNCTION__);
                        os_mem_free(conn_msg_matter);
                        conn_msg_matter = NULL;
                    }
                } else {
                    DiagPrintf("[%s]Malloc failed\r\n");
                }
            }
        } else {
            DiagPrintf("[APP] Connection establish failed(err: 0x%x), remote device: %s\r\n",
                       conn_ind->err, le_addr);
        }

        break;
    }

    case RTK_BT_LE_GAP_EVT_DISCONN_IND: {
        rtk_bt_le_disconn_ind_t *disconn_ind = (rtk_bt_le_disconn_ind_t *)param;
        rtk_bt_le_addr_to_str(&(disconn_ind->peer_addr), le_addr, sizeof(le_addr));
        role = disconn_ind->role ? "slave" : "master";
        DiagPrintf("[APP] Disconnected, reason: 0x%x, handle: %d, role: %s, remote device: %s\r\n",
                   disconn_ind->reason, disconn_ind->conn_handle, role, le_addr);

        //send data to matter, connected msg require conn_id only
        uint8_t conn_id;
        if (rtk_bt_le_gap_get_conn_id(disconn_ind->conn_handle, &conn_id) != RTK_BT_OK) {
            DiagPrintf("[%s]Fail to get conn_id\n");
        } else {
            BLE_MATTER_APP_DISCONNECT_CB_ARG *disconn_msg_matter = (BLE_MATTER_APP_DISCONNECT_CB_ARG *)os_mem_alloc(0, sizeof(BLE_MATTER_APP_DISCONNECT_CB_ARG));
            if (disconn_msg_matter) {
                memset(disconn_msg_matter, 0, sizeof(BLE_MATTER_APP_DISCONNECT_CB_ARG));
                disconn_msg_matter->conn_id = conn_id;
                disconn_msg_matter->disc_cause = disconn_ind->reason;
                if (ble_matter_adapter_send_callback_msg(BLE_MATTER_MSG_DISCONNECTED, NULL, disconn_msg_matter) == false) {
                    DiagPrintf("[%s]send callback msg fail\r\n", __FUNCTION__);
                    os_mem_free(disconn_msg_matter);
                    disconn_msg_matter = NULL;
                }
            } else {
                DiagPrintf("[%s]Malloc failed\r\n");
            }
        }

        /* gatts action */
        app_server_disconnect(disconn_ind->conn_handle);
        break;
    }

    case RTK_BT_LE_GAP_EVT_CONN_UPDATE_IND: {
        rtk_bt_le_conn_update_ind_t *conn_update_ind =
            (rtk_bt_le_conn_update_ind_t *)param;
        if (conn_update_ind->err) {
            DiagPrintf("[APP] Update conn param failed, conn_handle: %d, err: 0x%x\r\n",
                       conn_update_ind->conn_handle, conn_update_ind->err);
        } else {
            DiagPrintf("[APP] Conn param is updated, conn_handle: %d, conn_interval: 0x%x, "       \
                       "conn_latency: 0x%x, supervision_timeout: 0x%x\r\n",
                       conn_update_ind->conn_handle,
                       conn_update_ind->conn_interval,
                       conn_update_ind->conn_latency,
                       conn_update_ind->supv_timeout);
        }
        break;
    }

    case RTK_BT_LE_GAP_EVT_REMOTE_CONN_UPDATE_REQ_IND: { //BT sync api shall not be called here
        rtk_bt_le_remote_conn_update_req_ind_t *rmt_update_req =
            (rtk_bt_le_remote_conn_update_req_ind_t *)param;
        DiagPrintf("[APP] Remote device request a change in conn param, conn_handle: %d, "      \
                   "conn_interval_max: 0x%x, conn_interval_min: 0x%x, conn_latency: 0x%x, "      \
                   "timeout: 0x%x. The host stack accept it.\r\n",
                   rmt_update_req->conn_handle,
                   rmt_update_req->conn_interval_max,
                   rmt_update_req->conn_interval_min,
                   rmt_update_req->conn_latency,
                   rmt_update_req->supv_timeout);
        return RTK_BT_EVT_CB_ACCEPT;
        break;
    }

    case RTK_BT_LE_GAP_EVT_DATA_LEN_CHANGE_IND: {
        rtk_bt_le_data_len_change_ind_t *data_len_change =
            (rtk_bt_le_data_len_change_ind_t *)param;
        DiagPrintf("[APP] Data len is updated, conn_handle: %d, "       \
                   "max_tx_octets: 0x%x, max_tx_time: 0x%x, "        \
                   "max_rx_octets: 0x%x, max_rx_time: 0x%x\r\n",
                   data_len_change->conn_handle,
                   data_len_change->max_tx_octets,
                   data_len_change->max_tx_time,
                   data_len_change->max_rx_octets,
                   data_len_change->max_rx_time);
        break;
    }

    case RTK_BT_LE_GAP_EVT_PHY_UPDATE_IND: {
        rtk_bt_le_phy_update_ind_t *phy_update_ind =
            (rtk_bt_le_phy_update_ind_t *)param;
        if (phy_update_ind->err) {
            DiagPrintf("[APP] Update PHY failed, conn_handle: %d, err: 0x%x\r\n",
                       phy_update_ind->conn_handle,
                       phy_update_ind->err);
        } else {
            DiagPrintf("[APP] PHY is updated, conn_handle: %d, tx_phy: %d, rx_phy: %d\r\n",
                       phy_update_ind->conn_handle,
                       phy_update_ind->tx_phy,
                       phy_update_ind->rx_phy);
        }
        break;
    }

    case RTK_BT_LE_GAP_EVT_AUTH_PAIRING_CONFIRM_IND: {
        rtk_bt_le_auth_pair_cfm_ind_t *pair_cfm_ind =
            (rtk_bt_le_auth_pair_cfm_ind_t *)param;
        DiagPrintf("[APP] Just work pairing need user to confirm, conn_handle: %d!\r\n",
                   pair_cfm_ind->conn_handle);
        rtk_bt_le_pair_cfm_t pair_cfm_param = {0};
        uint16_t ret = 0;
        pair_cfm_param.conn_handle = pair_cfm_ind->conn_handle;
        pair_cfm_param.confirm = 1;
        ret = rtk_bt_le_sm_pairing_confirm(&pair_cfm_param);
        if (RTK_BT_OK == ret) {
            DiagPrintf("[APP] Just work pairing auto confirm succcess\r\n");
        }
        break;
    }

    case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_DISPLAY_IND: {
        rtk_bt_le_auth_key_display_ind_t *key_dis_ind =
            (rtk_bt_le_auth_key_display_ind_t *)param;
        DiagPrintf("[APP] Auth passkey display: %ld, conn_handle:%d\r\n",
                   key_dis_ind->passkey,
                   key_dis_ind->conn_handle);
        break;
    }

    case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_INPUT_IND: {
        rtk_bt_le_auth_key_input_ind_t *key_input_ind =
            (rtk_bt_le_auth_key_input_ind_t *)param;
        DiagPrintf("[APP] Please input the auth passkey get from remote, conn_handle: %d\r\n",
                   key_input_ind->conn_handle);
        break;
    }

    case RTK_BT_LE_GAP_EVT_AUTH_PASSKEY_CONFIRM_IND: {
        rtk_bt_le_auth_key_cfm_ind_t *key_cfm_ind =
            (rtk_bt_le_auth_key_cfm_ind_t *)param;
        DiagPrintf("[APP] Auth passkey confirm: %ld, conn_handle: %d. "  \
                   "Please comfirm if the passkeys are equal!\r\n",
                   key_cfm_ind->passkey,
                   key_cfm_ind->conn_handle);
        break;
    }

    case RTK_BT_LE_GAP_EVT_AUTH_OOB_KEY_INPUT_IND: {
        rtk_bt_le_auth_oob_input_ind_t *oob_input_ind =
            (rtk_bt_le_auth_oob_input_ind_t *)param;
        DiagPrintf("[APP] Bond use oob key, conn_handle: %d. Please input the oob tk \r\n",
                   oob_input_ind->conn_handle);
        break;
    }

    case RTK_BT_LE_GAP_EVT_AUTH_COMPLETE_IND: {
        rtk_bt_le_auth_complete_ind_t *auth_cplt_ind =
            (rtk_bt_le_auth_complete_ind_t *)param;
        if (auth_cplt_ind->err) {
            DiagPrintf("[APP] Pairing failed(err: 0x%x), conn_handle: %d\r\n",
                       auth_cplt_ind->err, auth_cplt_ind->conn_handle);
        } else {
            DiagPrintf("[APP] Pairing success, conn_handle: %d\r\n", auth_cplt_ind->conn_handle);
            DiagPrintf("[APP] long term key is 0x");
            for (uint8_t i = 1; i <= auth_cplt_ind->dev_ltk_length; i++) {
                DiagPrintf("%02x", auth_cplt_ind->dev_ltk[auth_cplt_ind->dev_ltk_length - i]); //End size conversion
            }
            DiagPrintf("\r\n");
        }
        break;
    }

    case RTK_BT_LE_GAP_EVT_BOND_MODIFY_IND: {
        rtk_bt_le_bond_modify_ind_t *bond_mdf_ind =
            (rtk_bt_le_bond_modify_ind_t *)param;
        char ident_addr[30] = {0};
        rtk_bt_le_addr_to_str(&(bond_mdf_ind->remote_addr), le_addr, sizeof(le_addr));
        rtk_bt_le_addr_to_str(&(bond_mdf_ind->ident_addr), ident_addr, sizeof(ident_addr));
        DiagPrintf("[APP] Bond info modified, op: %d, addr: %s, ident_addr: %s\r\n",
                   bond_mdf_ind->op, le_addr, ident_addr);

        break;
    }

#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
    case RTK_BT_LE_GAP_EVT_RESOLV_LIST_MODIFY_IND: {
        rtk_bt_le_modify_resolv_list_ind_t *p_ind = (rtk_bt_le_modify_resolv_list_ind_t *)param;
        rtk_bt_le_addr_t addr;

        if (p_ind->op == RTK_BT_LE_RESOLV_LIST_OP_ADD || p_ind->op == RTK_BT_LE_RESOLV_LIST_OP_REMOVE) {
            addr.type = (rtk_bt_le_addr_type_t)p_ind->entry.addr_type;
            memcpy(addr.addr_val, p_ind->entry.addr, RTK_BD_ADDR_LEN);
            rtk_bt_le_addr_to_str(&addr, le_addr, sizeof(le_addr));
            if (p_ind->err) {
                DiagPrintf("[APP] Resolving list %s %s fail, cause:%x.\r\n",
                           (p_ind->op == RTK_BT_LE_RESOLV_LIST_OP_ADD) ? "add" : "remove",
                           le_addr, p_ind->err);
            } else {
                DiagPrintf("[APP] Resolving list %s %s success, %s privacy mode.\r\n",
                           (p_ind->op == RTK_BT_LE_RESOLV_LIST_OP_ADD) ? "add" : "remove",
                           le_addr, p_ind->entry.device_mode ? "device" : "network");
            }
        } else if (p_ind->op == RTK_BT_LE_RESOLV_LIST_OP_CLEAR) {
            if (p_ind->err) {
                DiagPrintf("[APP] Resolving list clear fail, cause:%x.\r\n", p_ind->err);
            } else {
                DiagPrintf("[APP] Resolving list clear success.\r\n");
            }
        }
        break;
    }
#endif

#if defined(RTK_BLE_5_0_PA_SYNC_SUPPORT) && RTK_BLE_5_0_PA_SYNC_SUPPORT
    case RTK_BT_LE_GAP_EVT_PA_SYNC_STATE_IND: {
        rtk_bt_le_pa_sync_ind_t *pa_sync_ind = (rtk_bt_le_pa_sync_ind_t *)param;
        DiagPrintf("[APP] PA sync state change: sync_id: %d, state = %d, cause: 0x%x\r\n",
                   pa_sync_ind->sync_id, pa_sync_ind->state, pa_sync_ind->cause);

        if (pa_sync_ind->state == RTK_BT_LE_PA_SYNC_STATE_SYNCHRONIZED) {
            rtk_bt_le_pa_sync_sync_param_t sync_param;
            if (RTK_BT_OK == rtk_bt_le_gap_pa_sync_get_param(RTK_BT_LE_PA_SYNC_PARAM_SYNCHRONIZED_PARAM,
                    &sync_param, pa_sync_ind->sync_id)) {
                rtk_bt_le_addr_t addr;
                addr.type = sync_param.adv_addr_type;
                memcpy(addr.addr_val, sync_param.adv_addr, RTK_BD_ADDR_LEN);
                rtk_bt_le_addr_to_str(&addr, le_addr, sizeof(le_addr));
                DiagPrintf("[APP] PA SYNCHRONIZED PARAM: [Device]: %s, sync_handle:0x%x, adv_sid:%d, skip:%d, sync_timeout:%d, ",
                           le_addr, sync_param.sync_handle, sync_param.adv_sid, sync_param.skip, sync_param.sync_timeout);
                DiagPrintf("sync_cte_type:%d, adv_phy:%d, adv_clock_accuracy:0x%x, ",
                           sync_param.sync_cte_type, sync_param.adv_phy, sync_param.adv_clock_accuracy);
                DiagPrintf("periodic_adv_interval:%d, sync_transfer_received_flag:%d\r\n",
                           sync_param.periodic_adv_interval, sync_param.sync_transfer_received_flag);
            }
        }
        break;
    }

    case RTK_BT_LE_GAP_EVT_PA_ADV_REPORT_IND: {
        rtk_bt_le_pa_adv_report_ind_t *pa_report = (rtk_bt_le_pa_adv_report_ind_t *)param;
        DiagPrintf("[APP] PA sync ADV report: sync_id %d, sync_handle 0x%x, tx_power %d, rssi %d, cte_type %d, data_status 0x%x, data_len %d\r\n",
                   pa_report->sync_id, pa_report->sync_handle, pa_report->tx_power, pa_report->rssi,
                   pa_report->cte_type, pa_report->data_status, pa_report->data_len);
        break;
    }
#endif

#if defined(RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT) && RTK_BLE_5_1_PAST_RECIPIENT_SUPPORT
    case RTK_BT_LE_GAP_EVT_PAST_RECEIVED_INFO_IND: {
        rtk_bt_le_past_recv_ind_t *past_recv_ind = (rtk_bt_le_past_recv_ind_t *)param;
        DiagPrintf("[APP] PAST received info: conn_handle %d, service_data %d\r\n",
                   past_recv_ind->conn_handle, past_recv_ind->service_data);
        break;
    }
#endif

#if defined(RTK_BLE_5_2_POWER_CONTROL_SUPPORT) && RTK_BLE_5_2_POWER_CONTROL_SUPPORT
    case RTK_BT_LE_GAP_EVT_TXPOWER_REPORT_IND: {
        rtk_bt_le_txpower_ind_t *txpower_ind = (rtk_bt_le_txpower_ind_t *)param;
        DiagPrintf("[APP] TX power report: conn_handle %d, type %d, txpower %d\r\n",
                   txpower_ind->conn_handle, txpower_ind->type, txpower_ind->txpower);
        break;
    }
#endif

    default:
        DiagPrintf("[APP] Unkown gap cb evt type: %d\r\n", evt_code);
        break;
    }

    return RTK_BT_EVT_CB_OK;
}

static uint16_t app_get_gatts_app_id(uint8_t event, void *data)
{
    uint16_t app_id = 0xFFFF;

    switch (event) {
    case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
        rtk_bt_gatts_reg_ind_t *p_reg_ind = (rtk_bt_gatts_reg_ind_t *)data;
        app_id = p_reg_ind->app_id;
        break;
    }
    case RTK_BT_GATTS_EVT_READ_IND: {
        rtk_bt_gatts_read_ind_t *p_read_ind = (rtk_bt_gatts_read_ind_t *)data;
        app_id = p_read_ind->app_id;
        break;
    }
    case RTK_BT_GATTS_EVT_WRITE_IND: {
        rtk_bt_gatts_write_ind_t *p_write_ind = (rtk_bt_gatts_write_ind_t *)data;
        app_id = p_write_ind->app_id;
        break;
    }
    case RTK_BT_GATTS_EVT_CCCD_IND: {
        rtk_bt_gatts_cccd_ind_t *p_cccd_ind = (rtk_bt_gatts_cccd_ind_t *)data;
        app_id = p_cccd_ind->app_id;
        break;
    }
    case RTK_BT_GATTS_EVT_NOTIFY_COMPLETE_IND:
    case RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND: {
        rtk_bt_gatts_ntf_and_ind_ind_t *p_ind_ntf = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;
        app_id = p_ind_ntf->app_id;
        break;
    }
    default:
        break;
    }

    return app_id;
}

static rtk_bt_evt_cb_ret_t ble_peripheral_gatts_app_callback(uint8_t event, void *data, uint32_t len)
{
    (void)len;
    uint16_t app_id = 0xFFFF;

    if (RTK_BT_GATTS_EVT_MTU_EXCHANGE == event) {
        rtk_bt_gatt_mtu_exchange_ind_t *p_gatt_mtu_ind = (rtk_bt_gatt_mtu_exchange_ind_t *)data;
        if (p_gatt_mtu_ind->result == RTK_BT_OK) {
            DiagPrintf("[APP] GATTS mtu exchange successfully, mtu_size: %d, conn_handle: %d \r\n",
                       p_gatt_mtu_ind->mtu_size, p_gatt_mtu_ind->conn_handle);
        } else {
            DiagPrintf("[APP] GATTS mtu exchange fail \r\n");
        }
        return RTK_BT_EVT_CB_OK;
    }

    if (RTK_BT_GATTS_EVT_CLIENT_SUPPORTED_FEATURES == event) {
        rtk_bt_gatts_client_supported_features_ind_t *p_ind = (rtk_bt_gatts_client_supported_features_ind_t *)data;
        if (p_ind->features & RTK_BT_GATTS_CLIENT_SUPPORTED_FEATURES_EATT_BEARER_BIT) {
            DiagPrintf("[APP] Client Supported features is writed: conn_handle %d, features 0x%02X. Remote client supports EATT.\r\n",
                       p_ind->conn_handle, p_ind->features);
        }
        return RTK_BT_EVT_CB_OK;
    }

    app_id = app_get_gatts_app_id(event, data);
    switch (app_id) {
    case BLE_MATTER_ADAPTER_SRV_ID:
        ble_matter_adapter_ble_service_callback(event, data);
        break;

    default:
        break;
    }

    return RTK_BT_EVT_CB_OK;
}

extern bool rtk_bt_pre_enable(void);
int ble_matter_adapter_peripheral_main(uint8_t enable)
{
    rtk_bt_app_conf_t bt_app_conf = {0};
    rtk_bt_le_addr_t bd_addr = {(rtk_bt_le_addr_type_t)0, {0}};
    rtk_bt_le_adv_filter_t adv_filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
    char addr_str[30] = {0};
    char name[30] = {0};
#if defined(RTK_BLE_5_0_AE_ADV_SUPPORT) && RTK_BLE_5_0_AE_ADV_SUPPORT
    uint8_t adv_handle;
#endif
#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
    uint8_t bond_size = 0;
#endif

    if (1 == enable) {
        if (rtk_bt_pre_enable() == false) {
            DiagPrintf("%s fail!\r\n", __func__);
            return -1;
        }

        //set GAP configuration
        bt_app_conf.app_profile_support = RTK_BT_PROFILE_GATTS;
        bt_app_conf.mtu_size = 180;
        bt_app_conf.master_init_mtu_req = true;
        bt_app_conf.slave_init_mtu_req = false;
        bt_app_conf.prefer_all_phy = 0;
        bt_app_conf.prefer_tx_phy = 1 | 1 << 1 | 1 << 2;
        bt_app_conf.prefer_rx_phy = 1 | 1 << 1 | 1 << 2;
        bt_app_conf.max_tx_octets = 0x40;
        bt_app_conf.max_tx_time = 0x200;
#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
        memcpy(bt_app_conf.irk, privacy_irk, RTK_BT_LE_GAP_IRK_LEN);
#endif
        bt_app_conf.user_def_service = false;
        bt_app_conf.cccd_not_check = false;

        /* Enable BT */
        BT_APP_PROCESS(rtk_bt_enable(&bt_app_conf));
        BT_APP_PROCESS(rtk_bt_le_gap_get_bd_addr(&bd_addr));
        rtk_bt_le_addr_to_str(&bd_addr, addr_str, sizeof(addr_str));
        DiagPrintf("[APP] BD_ADDR: %s\r\n", addr_str);

        BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_COMMON_GP_GAP, peripheral_gap_app_callback));
        BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GAP, ble_peripheral_gap_app_callback));
        memcpy(name, (const char *)RTK_BT_DEV_NAME, strlen((const char *)RTK_BT_DEV_NAME));
        BT_APP_PROCESS(rtk_bt_le_gap_set_device_name((uint8_t *)name));
        BT_APP_PROCESS(rtk_bt_le_gap_set_appearance(RTK_BT_LE_GAP_APPEARANCE_HEART_RATE_BELT));

        BT_APP_PROCESS(rtk_bt_le_sm_set_security_param(&sec_param));
#if defined(RTK_BLE_PRIVACY_SUPPORT) && RTK_BLE_PRIVACY_SUPPORT
        if (privacy_enable) {
            BT_APP_PROCESS(rtk_bt_le_gap_privacy_init(privacy_whitelist));
#if !defined(RTK_BLE_5_0_AE_ADV_SUPPORT) || !RTK_BLE_5_0_AE_ADV_SUPPORT
            /* If privacy on, default use RPA adv, even not bonded */
            // adv_param.own_addr_type = 2;
#endif
            BT_APP_PROCESS(rtk_bt_le_sm_get_bond_num(&bond_size));
            if (bond_size != 0) {
#if PRIVACY_USE_DIR_ADV_WHEN_BONDED && (!defined(RTK_BLE_5_0_AE_ADV_SUPPORT) || !RTK_BLE_5_0_AE_ADV_SUPPORT) /* use dir adv when bonded*/
                rtk_bt_le_bond_info_t bond_info = {0};
                uint8_t bond_num = 1;
                rtk_bt_le_sm_get_bond_info(&bond_info, &bond_num);
                adv_param.type = RTK_BT_LE_ADV_TYPE_DIRECT_IND_LOW;
                adv_param.peer_addr.type = 0;
                memcpy(adv_param.peer_addr.addr_val, bond_info.ident_addr.addr_val, RTK_BD_ADDR_LEN);
                adv_param.own_addr_type = 2;
#endif
                if (privacy_whitelist) {
                    adv_filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST;
                }
            }
        }
#endif /* RTK_BLE_PRIVACY_SUPPORT */

        BT_APP_PROCESS(rtk_bt_evt_register_callback(RTK_BT_LE_GP_GATTS, ble_peripheral_gatts_app_callback));
        BT_APP_PROCESS(ble_matter_adapter_srv_add());
        ble_matter_adapter_callback_task_init();

#if (defined(BT_POWER_TEST_MODE) && BT_POWER_TEST_MODE) && (defined(RTK_BT_POWER_CONTROL_SUPPORT) && RTK_BT_POWER_CONTROL_SUPPORT)
        bt_power_test_init();
#endif
    } else if (0 == enable) {
#if (defined(BT_POWER_TEST_MODE) && BT_POWER_TEST_MODE) && (defined(RTK_BT_POWER_CONTROL_SUPPORT) && RTK_BT_POWER_CONTROL_SUPPORT)
        bt_power_test_deinit();
#endif
        app_server_deinit();

        /* no need to unreg callback here, it is done in rtk_bt_disable */
        // BT_APP_PROCESS(rtk_bt_evt_unregister_callback(RTK_BT_LE_GP_GATTS));

        /* Disable BT */
        BT_APP_PROCESS(rtk_bt_disable());
    }

    return 0;
}

int ble_matter_adapter_config_adv(uint8_t *padv_data, uint8_t padv_data_length)
{
    //check adv is idle
    rtk_bt_le_gap_dev_state_t dev_state;
    uint16_t ret = 0;
    rtk_bt_le_rand_addr_type_t type = RTK_BT_LE_RAND_ADDR_STATIC;
    uint8_t addr[RTK_BD_ADDR_LEN] = {0};
    bool auto_generate = true;

    while (1) {
        if (rtk_bt_le_gap_get_dev_state(&dev_state) == RTK_BT_OK) {
            if (dev_state.gap_adv_state == RTK_BT_LE_ADV_STATE_ADVERTISING) {
                dbg_printf("[MATTER]adv is adving, stop adv first.\n");
                BT_APP_PROCESS(rtk_bt_le_gap_stop_adv());
            } else if (dev_state.gap_adv_state == RTK_BT_LE_ADV_STATE_IDLE) {
                dbg_printf("[MATTER]adv is idle, continue to process.\n");
                break;
            }
        } else {
            DiagPrintf("[MATTER]Get device status fail\n");

        }
        os_delay(100);
    }

    //set random address
    ret = rtk_bt_le_gap_set_rand_addr(auto_generate, type, addr);
    if (ret) {
        DiagPrintf("[MATTER] set random address failed! err: 0x%x", ret);
        return -1;
    }

    //sync adv_data
    memset(ble_matter_adv_data, 0, padv_data_length);
    memcpy(ble_matter_adv_data, padv_data, padv_data_length);
    ble_matter_adv_data_length = padv_data_length;

    BT_APP_PROCESS(rtk_bt_le_gap_set_adv_data(ble_matter_adv_data, ble_matter_adv_data_length));

    adv_param.filter_policy = RTK_BT_LE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.own_addr_type = RTK_BT_LE_ADDR_TYPE_RANDOM;

    return 0;
}

int ble_matter_adapter_start_adv(void)
{
    //check adv is idle
    rtk_bt_le_gap_dev_state_t dev_state;

    while (1) {
        if (rtk_bt_le_gap_get_dev_state(&dev_state) == RTK_BT_OK) {
            if (dev_state.gap_adv_state == RTK_BT_LE_ADV_STATE_ADVERTISING) {
                dbg_printf("[MATTER]adv is adving, stop adv first.\n");
                BT_APP_PROCESS(rtk_bt_le_gap_stop_adv());
            } else if (dev_state.gap_adv_state == RTK_BT_LE_ADV_STATE_IDLE) {
                dbg_printf("[MATTER]adv is idle, continue to process.\n");
                break;
            }
        } else {
            DiagPrintf("[MATTER]Get device status fail\n");

        }
        os_delay(100);
    }

    BT_APP_PROCESS(rtk_bt_le_gap_start_adv(&adv_param));
    return 0;
}

int ble_matter_adapter_stop_adv(void)
{
    //check adv is adving
    rtk_bt_le_gap_dev_state_t dev_state;

    while (1) {
        if (rtk_bt_le_gap_get_dev_state(&dev_state) == RTK_BT_OK) {
            if (dev_state.gap_adv_state != RTK_BT_LE_ADV_STATE_IDLE) {
                dbg_printf("[MATTER]adv is not idle, stop adv.\n");
                BT_APP_PROCESS(rtk_bt_le_gap_stop_adv());
            } else {
                dbg_printf("[MATTER]adv is idle.\n");
                break;
            }

        } else {
            DiagPrintf("[MATTER]Get device status fail\n");

        }
        os_delay(100);
    }

    return 0;
}

uint16_t ble_matter_adapter_get_mtu(uint8_t conn_id)
{
    uint16_t ret = 0;
    uint16_t conn_handle;
    uint16_t mtu_size;

    conn_handle = le_get_conn_handle(conn_id);
    if (conn_handle == 0xFFFF) {
        return 0xFFFF;
    } else {
        dbg_printf("[MATTER]get conn_handle: %d\n", conn_handle);
    }

    if ((ret = rtk_bt_le_gap_get_mtu_size(conn_handle, &mtu_size)) != RTK_BT_OK) {
        DiagPrintf("GAP get mtu size failed! err: 0x%x", ret);
        return 0xFFFF;
    }

    dbg_printf("[MATTER]GAP get mtu size, conn_handle: %d, mtu_size: %d", conn_handle, mtu_size);
    return mtu_size;
}

int ble_matter_adapter_send_indication(uint8_t connect_id, uint8_t *data, uint16_t data_length)
{
    uint16_t ret = 0;
    rtk_bt_gatts_ntf_and_ind_param_t ntf_param = {0};
    uint16_t conn_handle;

    conn_handle = le_get_conn_handle(connect_id);
    if (conn_handle == 0xFFFF) {
        return -1;
    } else {
        dbg_printf("[MATTER]get conn_handle: %d\n", conn_handle);
    }

    ntf_param.conn_handle = conn_handle;
    ntf_param.app_id = BLE_MATTER_ADAPTER_SRV_ID;
    ntf_param.index = BLE_MATTER_SERVICE_CHAR_TX_INDEX;
    ntf_param.len = data_length;
    ntf_param.data = (void *)os_mem_alloc(0, ntf_param.len);
    memcpy((uint8_t *)ntf_param.data, data, ntf_param.len);

    /*
    DiagPrintf("[JY]send indication length %d\n", ntf_param.len);
    for(int i=0;i<ntf_param.len;i++)
    	DiagPrintf("%x ", *((uint8_t *)ntf_param.data+i));
    */

    ret = rtk_bt_gatts_indicate(&ntf_param);
    if (RTK_BT_OK != ret) {
        osif_mem_free((void *)ntf_param.data);
        DiagPrintf("[MATTER] GATTS indicate failed! err: 0x%x", ret);
        return -1;
    }

    osif_mem_free((void *)ntf_param.data);
    dbg_printf("[MATTER] GATTS indicate sending ...");

    return 0;
}

int ble_matter_adapter_disconnect(uint8_t connect_id)
{
    int ret;
    uint16_t conn_handle;

    conn_handle = le_get_conn_handle(connect_id);
    if (conn_handle == 0xFFFF) {
        return -1;
    } else {
        dbg_printf("[MATTER]get conn_handle: %d\n", conn_handle);
    }

    if ((ret = rtk_bt_le_gap_disconnect(conn_handle)) != 0) {
        DiagPrintf("[MATTER] GAP disconnect ops failed! err: 0x%x", ret);
        return -1;
    }
    return 0;
}

#endif