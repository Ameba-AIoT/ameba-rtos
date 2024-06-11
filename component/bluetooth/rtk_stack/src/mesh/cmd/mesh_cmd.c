/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     mesh_cmd.c
  * @brief    Source file for mesh common cmd.
  * @details  User command interfaces.
  * @author   bill
  * @date     2017-3-31
  * @version  v1.0
  * *************************************************************************************
  */

/* Add Includes here */
#include <string.h>
#include "trace.h"
#include "app_msg.h"
#include "gap_wrapper.h"
#include "mesh_cmd.h"
#include "mesh_api.h"
#include "proxy_client.h"
#include "ping.h"
#include "tp.h"
#include <rtk_bt_common.h>
#include <rtk_bt_mesh_common.h>

uint8_t proxy_client_conn_id;

user_cmd_parse_result_t user_cmd_reset(user_cmd_parse_value_t *pparse_value)
{
    UNUSED(pparse_value);
    //WDG_SystemReset(RESET_ALL, SW_RESET_APP_START);
    return USER_CMD_RESULT_OK;
}

static void copy_data_to_memory_byte_by_byte(uint32_t val, uint8_t *p, uint8_t data_len)
{
	// Copy a val to memory using little endian
	if (!p || data_len > 4) {
		// BT_LOGE("[%s] copy data to memory fail\r\n", __func__);
		return false;
	}
	for (uint8_t i = 0; i < data_len; i++) {
		p[i] = (val & (0xff << i * 8)) >> i * 8;
	}
	return true;
}

#define USER_CMD_LIST_MAX_LEN    1024
// max of user cmd list for provisioner : 190 + N(provisioned device num)*24 + 2(IPC align)
// Provisioner list info len with 20 device : 190+20*24 + 2 = 672 bytes
user_cmd_parse_result_t user_cmd_list(user_cmd_parse_value_t *pparse_value)
{
    UNUSED(pparse_value);
	uint8_t list_copy[USER_CMD_LIST_MAX_LEN];
	uint8_t *p_data = (uint8_t *)list_copy;
	uint32_t offset = 0;
	// BT_LOGA("MeshState:\t%d\r\n", mesh_node.node_state);
	// Indicate event data struct : type(1 byte) + length(1 byte) + value(n bytes) + type +len + val ......
	// Add mesh state
	*(p_data + offset) = RTK_BT_MESH_STACK_USER_LIST_MESH_STATE;
	offset++;
	*(p_data + offset) = 1;
	offset++;
	*(p_data + offset) = mesh_node.node_state;
	offset ++;
	// BT_LOGA("DevUUID:\t");
	// data_uart_dump(mesh_node.dev_uuid, 16);
	// Add device uuid 16 bytes
	*(p_data + offset) = RTK_BT_MESH_STACK_USER_LIST_DEV_UUID;
	offset++;
	*(p_data + offset) = 16;
	offset++;
	memcpy(p_data + offset, mesh_node.dev_uuid, 16);
	offset += 16;
    uint8_t bt_addr[6];
    gap_get_param(GAP_PARAM_BD_ADDR, bt_addr);
	// BT_LOGA("BTAddr:\t\t0x%02x%02x%02x%02x%02x%02x\r\n",
	// 	   bt_addr[5], bt_addr[4], bt_addr[3], bt_addr[2], bt_addr[1], bt_addr[0]);
	// Add bt address
	*(p_data + offset) = RTK_BT_MESH_STACK_USER_LIST_DEV_ADDR;
	offset++;
	*(p_data + offset) = 6;
	offset++;
	memcpy(p_data + offset, bt_addr, 6);
	offset += 6;
	for (uint16_t index = 0; index < mesh_node.dev_key_num; index++) {
		if (mesh_node.dev_key_list[index].used && mesh_node.dev_key_list[index].element_num != 0) {
			// BT_LOGA("DevKey:\t\t%d-0x%04x-%d-", index, mesh_node.dev_key_list[index].unicast_addr,
			// 	   mesh_node.dev_key_list[index].element_num);
			// data_uart_dump(mesh_node.dev_key_list[index].dev_key, 16);
			// Add bt dev key
			*(p_data + offset) = RTK_BT_MESH_STACK_USER_LIST_DEV_KEY;
			offset++;
			*(p_data + offset) = 20;  // Len:20
			offset++;
			memcpy(p_data + offset, &index, 1);  // offset:0  type:uint8_t
			offset++;
			copy_data_to_memory_byte_by_byte(mesh_node.dev_key_list[index].unicast_addr, p_data + offset, 2);  // offset:1  type:uint16_t
			offset += 2;
			memcpy(p_data + offset, &(mesh_node.dev_key_list[index].element_num), 1);  // offset:1  type:uint8_t
			offset++;
			memcpy(p_data + offset, mesh_node.dev_key_list[index].dev_key, 16);
			offset += 16;
        }
    }
	for (uint16_t index = 0; index < mesh_node.app_key_num; index++) {
		if (mesh_node.app_key_list[index].key_state != MESH_KEY_STATE_INVALID) {
			// BT_LOGA("AppKey:\t\t%d-0x%04x-%d-%d-%d\r\n", index,
			// 	   mesh_node.app_key_list[index].app_key_index_g, mesh_node.app_key_list[index].key_state,
			// 	   key_state_to_tx_loop(mesh_node.app_key_list[index].key_state),
			// 	   mesh_node.app_key_list[index].net_key_binding);
			for (uint8_t loop = 0; loop < 2; loop++) {
				if (mesh_node.app_key_list[index].papp_key[loop] != NULL) {
					// BT_LOGA("\t\t");
					// data_uart_dump(mesh_node.app_key_list[index].papp_key[loop]->app_key, 16);
					// Add bt app key
					*(p_data + offset) = RTK_BT_MESH_STACK_USER_LIST_APP_KEY;
					offset++;
					*(p_data + offset) = 23;  // len:23
					offset++;
					memcpy(p_data + offset, &index, 1);  // offset:0  type:uint8_t
					offset++;
					copy_data_to_memory_byte_by_byte(mesh_node.app_key_list[index].app_key_index_g, p_data + offset, 2);  // offset:1  type:uint16_t
					offset += 2;
					memcpy(p_data + offset, &(mesh_node.app_key_list[index].key_state), 1);  // offset:3  type:uint8_t
					offset++;
					uint8_t val = key_state_to_tx_loop(mesh_node.app_key_list[index].key_state);
					memcpy(p_data + offset, &val, 1);  // offset:4  type:uint8_t
					offset++;
					copy_data_to_memory_byte_by_byte(mesh_node.app_key_list[index].net_key_binding, p_data + offset, 2);  // offset:1  type:uint16_t
					offset += 2;
					memcpy(p_data + offset, &(mesh_node.app_key_list[index].papp_key[loop]->app_key), 16);
					offset += 16;
                }
            }
        }
    }
	for (uint16_t index = 0; index < mesh_node.net_key_num; index++) {
		if (mesh_node.net_key_list[index].key_state != MESH_KEY_STATE_INVALID) {
			// BT_LOGA("NetKey:\t\t%d-0x%04x-%d-%d-%d\r\n", index,
			// 	   mesh_node.net_key_list[index].net_key_index_g, mesh_node.net_key_list[index].key_state,
			// 	   key_state_to_tx_loop(mesh_node.net_key_list[index].key_state),
			// 	   key_state_to_key_refresh_phase(mesh_node.net_key_list[index].key_state));
			if (mesh_node.net_key_list[index].net_key_index_g & 0x8000) {
                break;
            }
			for (uint8_t loop = 0; loop < 2; loop++) {
				if (mesh_node.net_key_list[index].pnet_key[loop] != NULL) {
					// BT_LOGA("\t\t");
					// data_uart_dump(mesh_node.net_key_list[index].pnet_key[loop]->net_key, 16);
					// Add bt net key
					*(p_data + offset) = RTK_BT_MESH_STACK_USER_LIST_NET_KEY;
					offset++;
					*(p_data + offset) = 22;  // len:22
					offset++;
					memcpy(p_data + offset, &index, 1);  // offset:0  type:uint8_t
					offset++;
					copy_data_to_memory_byte_by_byte(mesh_node.net_key_list[index].net_key_index_g, p_data + offset, 2);  // offset:1  type:uint16_t
					offset += 2;
					memcpy(p_data + offset, &(mesh_node.net_key_list[index].key_state), 1);  // offset:3  type:uint8_t
					offset++;
					uint8_t val = key_state_to_tx_loop(mesh_node.net_key_list[index].key_state);
					memcpy(p_data + offset, &val, 1);  // offset:4  type:uint8_t
					offset++;
					val = key_state_to_key_refresh_phase(mesh_node.net_key_list[index].key_state);
					memcpy(p_data + offset, &val, 1);  // offset:5  type:uint8_t
					offset++;
					memcpy(p_data + offset, &(mesh_node.net_key_list[index].pnet_key[loop]->net_key), 16);
					offset += 16;
                }
            }
        }
    }
	// BT_LOGA("IVindex:\t%d-0x%x\r\n", mesh_node.iv_update_flag, (unsigned int)mesh_node.iv_index);
	// BT_LOGA("Seq:\t\t0x%06x\r\n", (unsigned int)mesh_node.seq);
	// BT_LOGA("NodeAddr:\t0x%04x-%d-%d\r\n", mesh_node.unicast_addr,
	// 	   (int)mesh_node.element_queue.count, mesh_node.model_num);
	// Add bt normal vals
	*(p_data + offset) = RTK_BT_MESH_STACK_USER_LIST_NORMAL_VALS;
	offset++;
	*(p_data + offset) = 16;  // Len:16
	offset++;
	*(p_data + offset) = mesh_node.iv_update_flag;  // offset:0  type:uint8_t
	offset++;
	copy_data_to_memory_byte_by_byte(mesh_node.iv_index, p_data + offset, 4);  // offset:1  type:uint32_t
	offset += 4;
	copy_data_to_memory_byte_by_byte(mesh_node.seq, p_data + offset, 4);  // offset:5  type:uint32_t
	offset += 4;
	copy_data_to_memory_byte_by_byte(mesh_node.unicast_addr, p_data + offset, 2);  // offset:9  type:uint16_t
	offset += 2;
	copy_data_to_memory_byte_by_byte(mesh_node.element_queue.count, p_data + offset, 4);  // offset:11  type:uint32_t
	offset += 4;
	*(p_data + offset) = mesh_node.model_num;  // offset:15  type:uint8_t
	offset++;
    mesh_element_p pelement = (mesh_element_p)mesh_node.element_queue.pfirst;
	while (pelement != NULL) {
		// BT_LOGA("Element:\t%d-%d\r\n", pelement->element_index, (int)pelement->model_queue.count);
		// Add mesh element info
		*(p_data + offset) = RTK_BT_MESH_STACK_USER_LIST_ELEMENT_INFO;
		offset++;
		*(p_data + offset) = 5;  // Len:5
		offset++;
		*(p_data + offset) = pelement->element_index;  // offset:0  type:uint8_t
		offset++;
		copy_data_to_memory_byte_by_byte(pelement->model_queue.count, p_data + offset, 4);  // offset:1  type:uint32_t
		offset += 4;
        mesh_model_p pmodel = (mesh_model_p)pelement->model_queue.pfirst;
		while (pmodel != NULL) {
			// BT_LOGA("Model:\t\t%d-%d-0x%08x", pmodel->pmodel_info->model_index,
			// 	   pmodel->model_index, (unsigned int)pmodel->pmodel_info->model_id);
			// Add mesh model info
			*(p_data + offset) = RTK_BT_MESH_STACK_USER_LIST_MODEL_INFO;
			offset++;
			*(p_data + offset) = 6;  // Len:6
			offset++;
			*(p_data + offset) = pmodel->pmodel_info->model_index;  // offset:0  type:uint8_t
			offset++;
			*(p_data + offset) = pmodel->model_index;  // offset:1  type:uint8_t
			offset++;
			copy_data_to_memory_byte_by_byte(pmodel->pmodel_info->model_id, p_data + offset, 4);  // offset:2  type:uint32_t
			offset += 4;
            uint8_t key_flag = true;
			for (uint16_t index = 0; index < mesh_node.app_key_num; index++) {
                if (plt_bit_pool_get(pmodel->app_key_binding, index) &&
					mesh_node.app_key_list[index].key_state != MESH_KEY_STATE_INVALID) {
					if (key_flag) {
                        key_flag = false;
						// BT_LOGA("-(key:%d", index);
					} else {
						// BT_LOGA("-%d", index);
                    }
					// Add mesh model app key
					*(p_data + offset) = RTK_BT_MESH_STACK_USER_LIST_MODEL_APP_KEY;
					offset++;
					*(p_data + offset) = 1;
					offset++;
					*(p_data + offset) = index;
					offset++;
                    }
                }
			if (!key_flag) {
				// BT_LOGA(")");
            }
			if (MESH_NOT_UNASSIGNED_ADDR(pmodel->pub_params.pub_addr)) {
				// BT_LOGA("-(pub:0x%04x-%d-%d)", pmodel->pub_params.pub_addr, pmodel->pub_params.pub_ttl,
				// 	   pmodel->pub_params.pub_key_info.app_key_index);
				// Add mesh model pub info
				*(p_data + offset) = RTK_BT_MESH_STACK_USER_LIST_MODEL_PUB_INFO;
				offset++;
				*(p_data + offset) = 5;  // len:5
				offset++;
				copy_data_to_memory_byte_by_byte(pmodel->pub_params.pub_addr, p_data + offset, 2);  // offset:0  type:uint16_t
				offset += 2;
				*(p_data + offset) = pmodel->pub_params.pub_ttl;  // offset:2  type:uint8_t
				offset++;
				copy_data_to_memory_byte_by_byte(pmodel->pub_params.pub_key_info.app_key_index, p_data + offset, 2);  // offset:3  type:uint16_t
				offset += 2;
            }
            mesh_model_p pmodelb = pmodel;
			while (pmodelb->pmodel_info->pmodel_bound != NULL) {
                pmodelb = (mesh_model_p)pmodelb->pmodel_info->pmodel_bound->pmodel;
            }
            mesh_addr_member_p paddr_element = (mesh_addr_member_p)pmodelb->sub_queue.pfirst;
            uint16_t len_offset = 0;
			while (paddr_element != NULL) {
				if (paddr_element == (mesh_addr_member_p)pmodelb->sub_queue.pfirst) {
					if (pmodelb != pmodel) {
						// BT_LOGA("-(sub:-%d-%d-0x%04x",
                        //                 ((mesh_model_p)pmodel->pmodel_info->pmodel_bound->pmodel)->model_index,
                        //                 pmodelb->model_index, paddr_element->mesh_addr);
					} else {
						// BT_LOGA("-(sub:0x%04x", paddr_element->mesh_addr);
						// Add mesh model sub info
						*(p_data + offset) = RTK_BT_MESH_STACK_USER_LIST_MODEL_SUB_INFO;
						offset++;
						*(p_data + offset) = 2;  // len:2
                        len_offset = offset;
						offset++;
						memcpy(p_data + offset, &paddr_element->mesh_addr, 2);  // offset:0  type:uint16_t
						offset += 2;
                    }
				} else {
					// BT_LOGA("-0x%04x", paddr_element->mesh_addr);
                    *(p_data + len_offset) += 2;  // len = len+2
                    memcpy(p_data + offset, &paddr_element->mesh_addr, 2);  // type:uint16_t
					offset += 2;
                }
                paddr_element = paddr_element->pnext;
				if (paddr_element == NULL) {
					// BT_LOGA(")");
                }
            }
            pmodel = pmodel->pnext;
			// BT_LOGA("\r\n");
        }
        pelement = pelement->pnext;
    }
	if (offset > USER_CMD_LIST_MAX_LEN) {
		// BT_LOGA("[%s] Len %d of copy data extend the max val:%d\r\n", __func__, (int)offset, USER_CMD_LIST_MAX_LEN);
		// Indicate event to app
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_LIST_INFO, 0);
		rtk_bt_evt_indicate(p_evt, NULL);
	} else {
		// Indicate event to app
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_STACK, RTK_BT_MESH_STACK_EVT_LIST_INFO, offset);
		memcpy(p_evt->data, list_copy, offset);
		rtk_bt_evt_indicate(p_evt, NULL);
	}
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_dev_info_show(user_cmd_parse_value_t *pparse_value)
{
    dev_info_show_flag = pparse_value->dw_parameter[0] ? true : false;
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_dev_uuid_set(user_cmd_parse_value_t *pparse_value)
{
    plt_hex_to_bin(mesh_node.dev_uuid, (uint8_t *)pparse_value->pparameter[0], 16);
    data_uart_debug("DevUUID:\t");
    data_uart_dump(mesh_node.dev_uuid, 16);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_bearer_adv_set(user_cmd_parse_value_t *pparse_value)
{
    bearer_adv_set(pparse_value->dw_parameter[0]);
    data_uart_debug("Adv bearer: %s\r\n", pparse_value->dw_parameter[0] ? "on" : "off");
    return USER_CMD_RESULT_OK;
}

extern prov_auth_value_type_t prov_auth_value_type;
user_cmd_parse_result_t user_cmd_prov_auth_value(user_cmd_parse_value_t *pparse_value)
{
    uint8_t auth_data[16];
    uint8_t auth_value_len;
    uint8_t *auth_value;
    if (prov_auth_value_type != PROV_AUTH_VALUE_TYPE_ALPHANUMERIC)
    {
        auth_value_len = plt_hex_to_bin(auth_data, (uint8_t *)pparse_value->pparameter[0],
                                        sizeof(auth_data));
        auth_value = auth_data + sizeof(auth_data) - auth_value_len;
    }
    else
    {
        auth_value_len = strlen(pparse_value->pparameter[0]);
        auth_value = (uint8_t *)pparse_value->pparameter[0];
    }
    data_uart_debug("AuthValue: len=%d, value=", auth_value_len);
    data_uart_dump(auth_value, auth_value_len);
    return prov_auth_value_set(auth_value, auth_value_len) ? USER_CMD_RESULT_OK : USER_CMD_RESULT_ERROR;
}

user_cmd_parse_result_t user_cmd_vir_addr_set(user_cmd_parse_value_t *pparse_value)
{
    uint8_t label_uuid[16];
    uint16_t va_index;
    uint16_t va_addr;
    plt_hex_to_bin(label_uuid, (uint8_t *)pparse_value->pparameter[0], sizeof(label_uuid));
    va_index = vir_addr_add(label_uuid);
    va_addr = vir_addr_get(va_index);
    if (MESH_NOT_UNASSIGNED_ADDR(va_addr))
    {
        data_uart_debug("index=%d va=0x%04x\r\n", va_index, va_addr);
        return USER_CMD_RESULT_OK;
    }
    else
    {
        return USER_CMD_RESULT_ERROR;
    }
}

static uint32_t ping_time_us;
static uint32_t ping_time_ms;
static uint16_t pong_count;
static uint32_t rtime_sum;
static uint32_t ping_count;
static uint32_t pong_count_sum;
static uint32_t rtime_sum_sum;
static plt_timer_t ping_timer;
static ping_pong_type_t ping_type;
static uint16_t ping_dst;
static uint8_t ping_ttl;
static uint16_t ping_key_index;
static uint16_t pong_max_delay;
static uint32_t rtime_min;
static uint32_t rtime_max;
static mesh_msg_send_cause_t (*const ping_pf[3])(uint16_t dst, uint8_t ttl, uint16_t key_index,
                                                 uint16_t pong_max_delay) = {trans_ping, ping, big_ping};

int32_t tp_reveive(const mesh_model_info_p pmodel_info, uint32_t type, void *pargs)
{
    UNUSED(pmodel_info);
    UNUSED(type);
    mesh_msg_t *pmesh_msg = (mesh_msg_t *)pargs;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
    tp_msg_t *pmsg = (tp_msg_t *)(pbuffer);
    data_uart_debug("From:0x%04x To:0x%04x Tid:%d Time:%dms\r\n", pmesh_msg->src, pmesh_msg->dst,
                    (int)pmsg->tid, (int)plt_time_read_ms());
    if (pmesh_msg->msg_len < sizeof(tp_msg_t))
    {
        data_uart_send_string(pmsg->padding, pmesh_msg->msg_len - MEMBER_OFFSET(tp_msg_t, padding));
        data_uart_debug("\r\n>");
    }
    return 0;
}

void pong_receive(uint16_t src, uint16_t dst, uint8_t hops_forward, ping_pong_type_t type,
                  uint8_t hops_reverse, uint16_t pong_delay)
{
    UNUSED(dst);
    UNUSED(type);
    pong_count++;
    uint32_t pong_time_us = plt_time_read_us();
    uint32_t pong_time_ms = plt_time_read_ms();
    uint32_t diff_time = plt_time_diff(ping_time_ms, ping_time_us, pong_time_ms, pong_time_us);
    if (diff_time & 0x80000000)
    {
        diff_time &= 0x7fffffff;
        diff_time /= 1000;
    }
    if (ping_count == 1)
    {
        data_uart_debug("\b");
    }
    uint32_t rtime = diff_time - pong_delay * 10 + (diff_time > pong_delay * 10 ? 0 : 30);
    rtime_sum += rtime;
    if (rtime > rtime_max)
    {
        rtime_max = rtime;
    }
    if (rtime < rtime_min)
    {
        rtime_min = rtime;
    }
    data_uart_debug("%d\t0x%04x\t%d\t%d\t%d\t%d\t%d\t%d\r\n", pong_count, src, hops_forward,
                    hops_reverse, (int)diff_time, pong_delay * 10, (int)rtime, (int)(rtime_sum / pong_count));
}

extern void *evt_queue_handle; //!< Event queue handle
extern void *io_queue_handle; //!< IO queue handle
static void ping_timeout_cb(void *ptimer)
{
    UNUSED(ptimer);
    uint8_t event = EVENT_IO_TO_APP;
    T_IO_MSG msg;
    msg.type = PING_TIMEOUT_MSG;
    if (os_msg_send(io_queue_handle, &msg, 0) == false)
    {
    }
    else if (os_msg_send(evt_queue_handle, &event, 0) == false)
    {
    }
}

mesh_msg_send_cause_t ping_handle_timeout(void)
{
    ping_count++;
    pong_count_sum += pong_count;
    rtime_sum_sum += rtime_sum;
    data_uart_debug("count\tdst\tf-hops\tr-hops\ttime\tdelay\trtime\tatime\t%d\r\n", (int)ping_count);
    ping_time_us = plt_time_read_us();
    ping_time_ms = plt_time_read_ms();
    pong_count = 0;
    rtime_sum = 0;
    return ping_pf[ping_type](ping_dst, ping_ttl, ping_key_index, pong_max_delay);
}

static user_cmd_parse_result_t common_ping(user_cmd_parse_value_t *pparse_value)
{
    ping_dst = pparse_value->dw_parameter[0];
    ping_ttl = pparse_value->dw_parameter[1];
    ping_key_index = pparse_value->dw_parameter[2];
    pong_max_delay = pparse_value->dw_parameter[3];
    uint16_t period = pparse_value->dw_parameter[4];
    if (0 == period || 0 == pparse_value->para_count)
    {
        if (ping_timer != NULL)
        {
            plt_timer_delete(ping_timer, 0);
            ping_timer = NULL;
        }
        if (0 == pparse_value->para_count)
        {
            pong_count_sum += pong_count;
            rtime_sum_sum += rtime_sum;
            data_uart_debug("ping statistic: ping=%d pong=%d time(ms) min=%d max=%d avg=%d\r\n", (int)ping_count,
                            (int)pong_count_sum,
                            (int)rtime_min, (int)rtime_max, (int)(rtime_sum_sum / pong_count_sum));
            return USER_CMD_RESULT_OK;
        }
    }
    else
    {
        if (period < pong_max_delay)
        {
            period = pong_max_delay;
        }
        if (NULL == ping_timer)
        {
            ping_timer = plt_timer_create("ping", period * 10, true, 0,
                                          ping_timeout_cb);
            if (ping_timer != NULL)
            {
                plt_timer_start(ping_timer, 0);
            }
        }
        else
        {
            plt_timer_change_period(ping_timer, period * 10, 0);
        }
    }
    ping_count = 0;
    pong_count = 0;
    pong_count_sum = 0;
    rtime_sum = 0;
    rtime_sum_sum = 0;
    rtime_min = 0xffffffff;
    rtime_max = 0x0;
    if (MESH_MSG_SEND_CAUSE_SUCCESS == ping_handle_timeout())
    {
        return USER_CMD_RESULT_OK;
    }
    else
    {
        return USER_CMD_RESULT_ERROR;
    }
}

user_cmd_parse_result_t user_cmd_trans_ping(user_cmd_parse_value_t *pparse_value)
{
    ping_type = PING_PONG_TYPE_TRANSPORT;
    return common_ping(pparse_value);
}

user_cmd_parse_result_t user_cmd_ping(user_cmd_parse_value_t *pparse_value)
{
    ping_type = PING_PONG_TYPE_ACCESS;
    return common_ping(pparse_value);
}

user_cmd_parse_result_t user_cmd_big_ping(user_cmd_parse_value_t *pparse_value)
{
    ping_type = PING_PONG_TYPE_ACCESS_BIG;
    return common_ping(pparse_value);
}

user_cmd_parse_result_t user_cmd_tp_msg(user_cmd_parse_value_t *pparse_value)
{
    uint16_t dst = pparse_value->dw_parameter[0];
    uint8_t ttl = pparse_value->dw_parameter[1];
    uint16_t app_key_index = pparse_value->dw_parameter[2];
    if (MESH_MSG_SEND_CAUSE_SUCCESS == tp_msg(dst, ttl, app_key_index,
                                              (uint8_t *)pparse_value->pparameter[3], strlen(pparse_value->pparameter[3])))
    {
        return USER_CMD_RESULT_OK;
    }
    else
    {
        return USER_CMD_RESULT_ERROR;
    }
}

user_cmd_parse_result_t user_cmd_tp_start(user_cmd_parse_value_t *pparse_value)
{
    uint16_t dst = pparse_value->dw_parameter[0];
    uint8_t ttl = pparse_value->dw_parameter[1];
    uint16_t app_key_index = pparse_value->dw_parameter[2];
    uint32_t count = pparse_value->dw_parameter[3];
    if (MESH_MSG_SEND_CAUSE_SUCCESS == tp_start(dst, ttl, app_key_index, count))
    {
        return USER_CMD_RESULT_OK;
    }
    else
    {
        return USER_CMD_RESULT_ERROR;
    }
}

user_cmd_parse_result_t user_cmd_connect(user_cmd_parse_value_t *pparse_value)
{
    uint8_t addr[6];
    plt_hex_to_bin(addr, (uint8_t *)pparse_value->pparameter[0], 6);
    plt_swap1(addr, 6);
    uint8_t addr_type = pparse_value->dw_parameter[1] ? GAP_LOCAL_ADDR_LE_RANDOM :
                        GAP_LOCAL_ADDR_LE_PUBLIC;
    T_GAP_LE_CONN_REQ_PARAM conn_req_param;
    conn_req_param.scan_interval = 0x10;
    conn_req_param.scan_window = 0x10;
    conn_req_param.conn_interval_min = 0x10;
    conn_req_param.conn_interval_max = 0x10;
    conn_req_param.conn_latency = 0;
    conn_req_param.supv_tout = 1000;
    conn_req_param.ce_len_min = 2 * (conn_req_param.conn_interval_min - 1);
    conn_req_param.ce_len_max = 2 * (conn_req_param.conn_interval_max - 1);
    le_set_conn_param(GAP_CONN_PARAM_1M, &conn_req_param);
    le_connect(GAP_PHYS_CONN_INIT_1M_BIT, addr, (T_GAP_REMOTE_ADDR_TYPE)addr_type,
               GAP_LOCAL_ADDR_LE_PUBLIC, 1000);
    data_uart_debug("Connecting ");
    plt_swap1(addr, 6);
    data_uart_dump(addr, 6);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_disconnect(user_cmd_parse_value_t *pparse_value)
{
    data_uart_debug("Disconnecting\r\n");
    le_disconnect(pparse_value->dw_parameter[0]);
    return USER_CMD_RESULT_OK;
}


user_cmd_parse_result_t user_cmd_proxy_discover(user_cmd_parse_value_t *pparse_value)
{
    data_uart_debug("Proxy Start Discover\r\n");
#if MESH_DEVICE
    /* for pts test */
    proxy_client_add(NULL);
    /* replace proxy server */
    //proxy_ctx_set_cb(proxy_ctx_id, proxy_client_data_in_write, proxy_service_receive);
    //proxy_ctx_set_filter_type(proxy_ctx_id, PROXY_CFG_FILTER_TYPE_BLACK_LIST);
#endif
    proxy_client_conn_id = pparse_value->dw_parameter[0];
    proxy_client_start_discovery(proxy_client_conn_id);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_proxy_cccd_operate(user_cmd_parse_value_t *pparse_value)
{
    /* Indicate which char CCCD command. */
    uint8_t cmd_type = (uint8_t)pparse_value->dw_parameter[0];
    /* Enable or disable, 1--enable, 0--disable. */
    bool cmd_data = (bool)pparse_value->dw_parameter[1];
    data_uart_debug("Proxy Cccd Operate\r\n");
    switch (cmd_type)
    {
    case 0:/* V3 Notify char notif enable/disable. */
        {
            proxy_client_data_out_cccd_set(proxy_client_conn_id, cmd_data);
            //proxy_ctx_set_link(proxy_ctx_id, proxy_client_conn_id);
            data_uart_debug("GATT bearer: on\r\n");
            if (bearer_adv_get())
            {
                data_uart_debug("\r\nThe adv bearer exits too, you can use \"bas\" cmd to close it\r\n");
            }
        }
        break;
    default:
        return USER_CMD_RESULT_WRONG_PARAMETER;
    }
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_proxy_list(user_cmd_parse_value_t *pparse_value)
{
    UNUSED(pparse_value);
    data_uart_debug("Proxy Server Handle List:\r\nidx\thandle\r\n");
    for (proxy_handle_type_t hdl_idx = HDL_PROXY_SRV_START; hdl_idx < HDL_PROXY_CACHE_LEN; hdl_idx++)
    {
        data_uart_debug("%d\t0x%x\r\n", hdl_idx, proxy_client_handle_get(proxy_client_conn_id, hdl_idx));
    }
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_proxy_cfg_set_filter_type(user_cmd_parse_value_t
                                                           *pparse_value)
{
    proxy_cfg_filter_type_t filter_type = (proxy_cfg_filter_type_t)pparse_value->dw_parameter[0];
    proxy_cfg_set_filter_type(proxy_ctx_id_get(proxy_client_conn_id, PROXY_CTX_TYPE_PROXY),
                              filter_type);
    data_uart_debug("Proxy cfg set filter type %d\r\n", filter_type);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_proxy_cfg_add_addr(user_cmd_parse_value_t *pparse_value)
{
    uint16_t addr = pparse_value->dw_parameter[0];
    proxy_cfg_add_remove_addr(proxy_ctx_id_get(proxy_client_conn_id, PROXY_CTX_TYPE_PROXY), TRUE, &addr,
                              1);
    data_uart_debug("Proxy cfg add addr 0x%04x\r\n", addr);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_proxy_cfg_remove_addr(user_cmd_parse_value_t *pparse_value)
{
    uint16_t addr = pparse_value->dw_parameter[0];
    proxy_cfg_add_remove_addr(proxy_ctx_id_get(proxy_client_conn_id, PROXY_CTX_TYPE_PROXY), FALSE,
                              &addr, 1);
    data_uart_debug("Proxy cfg remove addr 0x%04x\r\n", addr);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_log_set(user_cmd_parse_value_t *pparse_value)
{
    log_module_trace_set(TRACE_MODULE_LOWERSTACK, TRACE_LEVEL_ERROR,
                         pparse_value->dw_parameter[0] & 0x1);
    log_module_trace_set(TRACE_MODULE_LOWERSTACK, TRACE_LEVEL_WARN,
                         pparse_value->dw_parameter[0] & 0x1);
    log_module_trace_set(TRACE_MODULE_LOWERSTACK, TRACE_LEVEL_INFO,
                         pparse_value->dw_parameter[0] & 0x1);
    log_module_trace_set(TRACE_MODULE_LOWERSTACK, TRACE_LEVEL_TRACE,
                         pparse_value->dw_parameter[0] & 0x1);

    log_module_bitmap_trace_set(0x7FFFFFFFFFFFFFFF, TRACE_LEVEL_TRACE,
                                pparse_value->dw_parameter[0] & 0x2);
    log_module_bitmap_trace_set(0x7FFFFFFFFFFFFFFF, TRACE_LEVEL_INFO,
                                pparse_value->dw_parameter[0] & 0x2);

    uint32_t log_value[MESH_LOG_LEVEL_SIZE];
    memset(log_value, pparse_value->dw_parameter[0] & 0x4 ? 0xff : 0, sizeof(log_value));
    diag_level_set(TRACE_LEVEL_TRACE, log_value);

    data_uart_debug("Log setting %d!\r\n", (int)pparse_value->dw_parameter[0]);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_time(user_cmd_parse_value_t *pparse_value)
{
    UNUSED(pparse_value);
    uint32_t time_ms = plt_time_read_ms();
    uint32_t day, hour, minute, second;
    second = time_ms / 1000;
    minute = second / 60;
    second %= 60;
    hour = minute / 60;
    minute %= 60;
    day = hour / 24;
    hour %= 24;
    data_uart_debug("Time: %dd%dh%dm%ds\r\n", (int)day, (int)hour, (int)minute, (int)second);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_adv_power_set(user_cmd_parse_value_t *pparse_value)
{
    //uint8_t tx_gain[4] = {0x30, 0x80, 0xA0, 0xF0};
    //le_adv_set_tx_power(GAP_ADV_TX_POW_SET_1M, tx_gain[pparse_value->dw_parameter[0]]);
    int8_t tx_gain_info[4] = {-20, 0, 4, 8};
    data_uart_debug("Adv power %d dBm\r\n", tx_gain_info[pparse_value->dw_parameter[0]]);
    return USER_CMD_RESULT_OK;
}

void fn_cb(uint8_t frnd_index, fn_cb_type_t type, uint16_t lpn_addr);
user_cmd_parse_result_t user_cmd_fn_init(user_cmd_parse_value_t *pparse_value)
{
#if MESH_FN
    fn_params_t fn_params;
    uint8_t fn_num = pparse_value->dw_parameter[0];
    fn_params.queue_size = pparse_value->dw_parameter[1];
    if (pparse_value->dw_parameter[2] > 0 && pparse_value->dw_parameter[2] < 0xff)
    {
        mesh_node.frnd_rx_window = pparse_value->dw_parameter[2];
    }
    return fn_init(fn_num, &fn_params, fn_cb) ? USER_CMD_RESULT_OK : USER_CMD_RESULT_ERROR;
#else
    return USER_CMD_RESULT_ERROR;
#endif
}

user_cmd_parse_result_t user_cmd_fn_deinit(user_cmd_parse_value_t *pparse_value)
{
#if MESH_FN
    UNUSED(pparse_value);
    fn_deinit();
    return USER_CMD_RESULT_OK;
#else
    return USER_CMD_RESULT_ERROR;
#endif
}

user_cmd_parse_result_t user_cmd_hb_pub(user_cmd_parse_value_t *pparse_value)
{
    hb_pub_t pub;
    pub.dst = pparse_value->dw_parameter[0];
    pub.count = pparse_value->dw_parameter[1];
    pub.period = pparse_value->dw_parameter[2];
    pub.ttl = 0x7f;
    pub.features.relay = 0;
    pub.features.proxy = 0;
    pub.features.frnd = 0;
    pub.features.lpn = 0;
    pub.features.rfu = 0;
    pub.net_key_index = 0;

    hb_publication_set(pub);
    hb_timer_start(HB_TIMER_PUB);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_hb_sub(user_cmd_parse_value_t *pparse_value)
{
    hb_sub_t sub;
    sub.src = pparse_value->dw_parameter[0];
    sub.dst = pparse_value->dw_parameter[1];
    sub.period = pparse_value->dw_parameter[2];
    sub.count = 0;
    sub.min_hops = 0x7f;
    sub.max_hops = 0;

    hb_subscription_set(sub);
    hb_timer_start(HB_TIMER_SUB);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_mesh_deinit(user_cmd_parse_value_t *pparse_value)
{
    UNUSED(pparse_value);
    mesh_deinit();
    return USER_CMD_RESULT_OK;
}

