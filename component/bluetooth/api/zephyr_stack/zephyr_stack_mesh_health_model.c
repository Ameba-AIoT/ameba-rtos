/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdio.h>
#include <string.h>

#include <rtk_bt_common.h>
#include <zephyr/settings/settings.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/mesh.h>
#include <zephyr/net/buf.h>
#include <access.h>
#include <net.h>
#if defined(CONFIG_BT_MESH_LOW_POWER) && CONFIG_BT_MESH_LOW_POWER
#include <lpn.h>
#endif
#include <prov.h>
#include <subnet.h>
#include <bt_app_config.h>
#include <math.h>
#include <rtk_bt_def.h>
#include <rtk_bt_mesh_def.h>
#include <rtk_bt_mesh_common.h>
#include <rtk_bt_mesh_health_model.h>
#include <zephyr/bluetooth/mesh/health_cli.h>
#include <msg.h>

#if defined(RTK_BLE_MESH_PROVISIONER_SUPPORT) && RTK_BLE_MESH_PROVISIONER_SUPPORT
void fault_status_handle(struct bt_mesh_health_cli *cli, uint16_t addr,
						 uint8_t test_id, uint16_t cid, uint8_t *faults,
						 size_t fault_count)
{
	(void)cli;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_HEALTH_CLIENT_MODEL, RTK_BT_MESH_HEALTH_CLIENT_MODEL_FAULT_STATUS,
								sizeof(rtk_bt_mesh_health_client_status_t) + fault_count);
	rtk_bt_mesh_health_client_status_t status;
	status.src = addr;
	status.test_id = test_id;
	status.company_id = cid;
	status.fault_array_len = fault_count;
	status.fault_array = faults;
	memcpy(p_evt->data, &status, sizeof(rtk_bt_mesh_health_client_status_t));
	memcpy((uint8_t *)p_evt->data + sizeof(rtk_bt_mesh_health_client_status_t), faults, fault_count);
	rtk_bt_evt_indicate(p_evt, NULL);
}

void current_status_handle(struct bt_mesh_health_cli *cli, uint16_t addr,
						   uint8_t test_id, uint16_t cid, uint8_t *faults,
						   size_t fault_count)
{
	(void)cli;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_HEALTH_CLIENT_MODEL, RTK_BT_MESH_HEALTH_CLIENT_MODEL_CURT_STATUS,
								sizeof(rtk_bt_mesh_health_client_status_t) + fault_count);
	rtk_bt_mesh_health_client_status_t status;
	status.src = addr;
	status.test_id = test_id;
	status.company_id = cid;
	status.fault_array_len = fault_count;
	status.fault_array = faults;
	memcpy(p_evt->data, &status, sizeof(rtk_bt_mesh_health_client_status_t));
	memcpy((uint8_t *)p_evt->data + sizeof(rtk_bt_mesh_health_client_status_t), faults, fault_count);
	rtk_bt_evt_indicate(p_evt, NULL);
}

void period_status_handle(struct bt_mesh_health_cli *cli, uint16_t addr,
						  uint8_t divisor)
{
	(void)cli;
	(void)addr;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_HEALTH_CLIENT_MODEL, RTK_BT_MESH_HEALTH_CLIENT_MODEL_PERIOD_STATUS,
								sizeof(rtk_bt_mesh_health_client_status_period_t));
	memcpy(p_evt->data, &divisor, sizeof(rtk_bt_mesh_health_client_status_period_t));
	rtk_bt_evt_indicate(p_evt, NULL);
}

void attention_status_handle(struct bt_mesh_health_cli *cli, uint16_t addr,
							 uint8_t attention)
{
	(void)cli;
	(void)addr;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_HEALTH_CLIENT_MODEL, RTK_BT_MESH_HEALTH_CLIENT_MODEL_ATTN_STATUS,
								sizeof(rtk_bt_mesh_health_client_status_attention_t));
	memcpy(p_evt->data, &attention, sizeof(rtk_bt_mesh_health_client_status_attention_t));
	rtk_bt_evt_indicate(p_evt, NULL);
}

struct bt_mesh_health_cli health_cli = {
	.current_status = current_status_handle,
	.period_status = period_status_handle,
	.attention_status = attention_status_handle,
	.fault_status = fault_status_handle,
};

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
uint16_t bt_mesh_health_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_HEALTH_CLIENT_MODEL_FAULT_GET: {
		rtk_bt_mesh_health_fault_get_t *fault_get = (rtk_bt_mesh_health_fault_get_t *)p_cmd->param;
		struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(fault_get->app_key_index, fault_get->dst);
		ret = bt_mesh_health_cli_fault_get(&health_cli, &ctx, fault_get->company_id,
										   NULL, NULL, NULL);
		break;
	}
	case RTK_BT_MESH_HEALTH_CLIENT_MODEL_FAULT_CLEAR: {
		rtk_bt_mesh_health_fault_clear_t *fault_clear = (rtk_bt_mesh_health_fault_clear_t *)p_cmd->param;
		struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(fault_clear->app_key_index, fault_clear->dst);
		ret = bt_mesh_health_cli_fault_clear(&health_cli, &ctx, fault_clear->company_id,
											 NULL, NULL, NULL);
		break;
	}
	case RTK_BT_MESH_HEALTH_CLIENT_MODEL_FAULT_TEST: {
		rtk_bt_mesh_health_fault_test_t *fault_test = (rtk_bt_mesh_health_fault_test_t *)p_cmd->param;
		struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(fault_test->app_key_index, fault_test->dst);
		if (fault_test->ack) {
			ret = bt_mesh_health_cli_fault_test(&health_cli, &ctx, fault_test->company_id,
												fault_test->test_id, NULL, NULL);
		} else {
			ret = bt_mesh_health_cli_fault_test_unack(&health_cli, &ctx, fault_test->company_id, fault_test->test_id);
		}
		break;
	}
	case RTK_BT_MESH_HEALTH_CLIENT_MODEL_PERIOD_GET: {
		rtk_bt_mesh_health_period_get_t *period_get = (rtk_bt_mesh_health_period_get_t *)p_cmd->param;
		struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(period_get->app_key_index, period_get->dst);
		ret = bt_mesh_health_cli_period_get(&health_cli, &ctx, NULL);
		break;
	}
	case RTK_BT_MESH_HEALTH_CLIENT_MODEL_PERIOD_SET: {
		rtk_bt_mesh_health_period_set_t *period_set = (rtk_bt_mesh_health_period_set_t *)p_cmd->param;
		struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(period_set->app_key_index, period_set->dst);
		if (period_set->ack) {
			ret = bt_mesh_health_cli_period_set(&health_cli, &ctx, period_set->fast_period_divisor, NULL);
		} else {
			ret = bt_mesh_health_cli_period_set_unack(&health_cli, &ctx, period_set->fast_period_divisor);
		}
		break;
	}
	case RTK_BT_MESH_HEALTH_CLIENT_MODEL_ATTN_GET: {
		rtk_bt_mesh_health_attn_get_t *attn_get = (rtk_bt_mesh_health_attn_get_t *)p_cmd->param;
		struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(attn_get->app_key_index, attn_get->dst);
		ret = bt_mesh_health_cli_attention_get(&health_cli, &ctx, NULL);
		break;
	}
	case RTK_BT_MESH_HEALTH_CLIENT_MODEL_ATTN_SET: {
		rtk_bt_mesh_health_attn_set_t *attn_set = (rtk_bt_mesh_health_attn_set_t *)p_cmd->param;
		struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(attn_set->app_key_index, attn_set->dst);
		if (attn_set->ack) {
			ret = bt_mesh_health_cli_attention_set(&health_cli, &ctx, attn_set->attn, NULL);
		} else {
			ret = bt_mesh_health_cli_attention_set_unack(&health_cli, &ctx, attn_set->attn);
		}
		break;
	}
	default:
		BT_LOGE("[%s] Unknown p_cmd->act:%d\r\n", __func__, p_cmd->act);
		break;
	}
end:
	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}
#endif

#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
#define HEALTH_FAULT_MAX_NUM       256
#define HEALTH_FAULT_BLOCK_SIZE    32
#define HEALTH_FAULT_BLOCK_COUNT   ((HEALTH_FAULT_MAX_NUM + HEALTH_FAULT_BLOCK_SIZE - 1) / HEALTH_FAULT_BLOCK_SIZE)

typedef struct {
	uint16_t company_id;
	uint8_t recently_test_id;
	uint8_t fast_period_divisor;
	const rtk_bt_mesh_health_server_test_t *ptests;
	uint8_t num_tests;
	uint32_t registered_faults[HEALTH_FAULT_BLOCK_COUNT];
	uint32_t current_faults[HEALTH_FAULT_BLOCK_COUNT];
} health_info_t, *health_info_p;

static health_info_t health_info = {.company_id = 0x005d};

static uint8_t health_server_fault_count_internal(const uint32_t *faults);
static void health_server_fill_fault(uint8_t *dst, const uint32_t *fault_bits)
{
	uint32_t temp_fault = 0;
	uint32_t bit = 0;
	uint8_t offset = 0;
	for (uint8_t i = 0; i < HEALTH_FAULT_BLOCK_COUNT; ++i) {
		temp_fault = fault_bits[i];
		while (0 != temp_fault) {
			bit = (temp_fault & ~(temp_fault - 1));
			offset = 0;
			while (0 != bit) {
				offset ++;
				bit >>= 1;
			}
			*dst ++ = i * 32 + offset - 1;
			temp_fault &= (temp_fault - 1);
		}
	}
}

static uint8_t hamming_weight(uint32_t data)
{
	uint32_t weight = data;
	weight = (weight & 0x55555555) + ((weight >> 1) & 0x55555555);
	weight = (weight & 0x33333333) + ((weight >> 2) & 0x33333333);
	weight = (weight & 0x0f0f0f0f) + ((weight >> 4) & 0x0f0f0f0f);
	weight = (weight & 0x00ff00ff) + ((weight >> 8) & 0x00ff00ff);
	weight = (weight & 0x0000ffff) + ((weight >> 16) & 0x0000ffff);

	return weight;
}

static uint8_t health_server_fault_count_internal(const uint32_t *faults)
{
	uint8_t count = 0;
	for (uint8_t i = 0; i < HEALTH_FAULT_BLOCK_COUNT; ++i) {
		count += hamming_weight(faults[i]);
	}

	return count;
}

bool is_health_server_has_fault(const struct bt_mesh_model *mod)
{
	struct bt_mesh_health_srv *srv = mod->rt->user_data;
	health_info_p phealth_info = srv->health_info;
	bool has = FALSE;
	for (uint8_t i = 0; i < HEALTH_FAULT_BLOCK_COUNT; ++i) {
		if (0 != phealth_info->current_faults[i]) {
			has = TRUE;
		}
	}

	return has;
}

uint8_t health_server_fault_count(const struct bt_mesh_model *mod)
{
	struct bt_mesh_health_srv *srv = mod->rt->user_data;
	health_info_p phealth_info = srv->health_info;
	return health_server_fault_count_internal(phealth_info->current_faults);
}

void health_server_set_tests(const struct bt_mesh_model *mod, const rtk_bt_mesh_health_server_test_t *ptests,
							 uint8_t num_tests)
{
	struct bt_mesh_health_srv *srv = mod->rt->user_data;
	health_info_p phealth_info = srv->health_info;
	phealth_info->ptests = ptests;
	phealth_info->num_tests = num_tests;
}

void health_server_set_company_id(const struct bt_mesh_model *mod, uint16_t company_id)
{
	struct bt_mesh_health_srv *srv = mod->rt->user_data;
	health_info_p phealth_info = srv->health_info;
	phealth_info->company_id = company_id;
}

static const struct bt_mesh_elem *primary_element(void)
{
	const struct bt_mesh_comp *comp = bt_mesh_comp_get();

	if (comp) {
		return &comp->elem[0];
	}

	return NULL;
}

void health_server_fault_register(const struct bt_mesh_model *mod, uint8_t fault)
{
	struct bt_mesh_health_srv *srv = mod->rt->user_data;
	health_info_p phealth_info = srv->health_info;
	uint8_t pos = fault / HEALTH_FAULT_BLOCK_SIZE;
	uint8_t bit = fault % HEALTH_FAULT_BLOCK_SIZE;
	phealth_info->registered_faults[pos] |= (1 << bit);
	phealth_info->current_faults[pos] |= (1 << bit);

	if (NULL != mod->pub) {
		const struct bt_mesh_elem *elem;
		elem = primary_element();
		bt_mesh_health_srv_fault_update(elem);
	}
}

void health_server_fault_clear(const struct bt_mesh_model *mod, uint8_t fault)
{
	struct bt_mesh_health_srv *srv = mod->rt->user_data;
	health_info_p phealth_info = srv->health_info;
	uint8_t pos = fault / HEALTH_FAULT_BLOCK_SIZE;
	uint8_t bit = fault % HEALTH_FAULT_BLOCK_SIZE;
	phealth_info->current_faults[pos] &= ~bit;
}

void health_server_fault_clear_all(const struct bt_mesh_model *mod)
{
	struct bt_mesh_health_srv *srv = mod->rt->user_data;
	health_info_p phealth_info = srv->health_info;
	for (uint8_t i = 0; i < HEALTH_FAULT_BLOCK_COUNT; ++i) {
		phealth_info->current_faults[i] = 0;
	}
}

static void health_server_registered_fault_clear_all(const struct bt_mesh_model *mod)
{
	struct bt_mesh_health_srv *srv = mod->rt->user_data;
	health_info_p phealth_info = srv->health_info;
	for (uint8_t i = 0; i < HEALTH_FAULT_BLOCK_COUNT; ++i) {
		phealth_info->registered_faults[i] = 0;
	}
}

bool health_server_fault_is_set(const struct bt_mesh_model *mod, uint8_t fault)
{
	struct bt_mesh_health_srv *srv = mod->rt->user_data;
	health_info_p phealth_info = srv->health_info;
	uint8_t pos = fault / HEALTH_FAULT_BLOCK_SIZE;
	uint8_t bit = fault % HEALTH_FAULT_BLOCK_SIZE;

	return (0x01 == (phealth_info->current_faults[pos] & (1 << bit)));
}

static void attention_on(const struct bt_mesh_model *mod)
{
	(void)mod;
	BT_LOGA("[%s] Enter\r\n", __func__);
}

static void attention_off(const struct bt_mesh_model *mod)
{
	(void)mod;
	BT_LOGA("[%s] Enter\r\n", __func__);
}

static uint8_t fault_store[HEALTH_FAULT_MAX_NUM + 2];
static int health_fault_get_reg(const struct bt_mesh_model *model, uint16_t company_id,
								uint8_t *test_id, uint8_t *faults,
								uint8_t *fault_count)
{
	(void)model;
	struct bt_mesh_health_srv *srv = model->rt->user_data;
	health_info_p phealth_info = srv->health_info;
	if (company_id == phealth_info->company_id) {
		*test_id = phealth_info->recently_test_id;
		*fault_count = health_server_fault_count_internal(phealth_info->registered_faults);
		health_server_fill_fault(faults, phealth_info->registered_faults);
	}

	rtk_bt_mesh_health_server_fault_get_t *fault_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL, RTK_BT_MESH_HEALTH_SERVER_MODEL_FAULT_GET,
								sizeof(rtk_bt_mesh_health_server_fault_get_t) + HEALTH_FAULT_MAX_NUM + 2);
	fault_get = (rtk_bt_mesh_health_server_fault_get_t *)p_evt->data;
	fault_get->company_id = company_id;
	fault_get->fault_array = fault_store;
	rtk_bt_evt_indicate(p_evt, NULL);
	// fault_store style: 1 byte(length) + 1 byte(test id) + n bytes(faults)
	// faults length = length - 1

	// memcpy(test_id, &fault_store[1], 1);
	// memcpy(faults, &fault_store[2], fault_store[0] - 1);
	// *fault_count = fault_store[0] - 1;

	return 0;
}

static uint8_t fault_clear_status[HEALTH_FAULT_MAX_NUM + 2];
static int health_fault_clear(const struct bt_mesh_model *model, uint16_t company_id)
{
	(void)model;
	health_server_registered_fault_clear_all(model);

	rtk_bt_mesh_health_server_fault_clear_t *fault_clear;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL, RTK_BT_MESH_HEALTH_SERVER_MODEL_FAULT_CLEAR,
								sizeof(rtk_bt_mesh_health_server_fault_clear_t) + HEALTH_FAULT_MAX_NUM + 2);
	fault_clear = (rtk_bt_mesh_health_server_fault_clear_t *)p_evt->data;
	fault_clear->company_id = company_id;
	fault_clear->fault_array = fault_clear_status;
	// fault_store style: 1 byte(length) + 1 byte(test id) + n bytes(faults)
	// faults length = length - 1
	rtk_bt_evt_indicate(p_evt, NULL);

	return 0;
}

static uint8_t fault_test_status[HEALTH_FAULT_MAX_NUM + 2];
static int health_fault_test(const struct bt_mesh_model *model, uint8_t test_id,
							 uint16_t company_id)
{
	(void)model;
	struct bt_mesh_health_srv *srv = model->rt->user_data;
	health_info_p phealth_info = srv->health_info;
	if (company_id == phealth_info->company_id) {
		uint8_t i = 0;
		for (; i < phealth_info->num_tests; ++i) {
			if (phealth_info->ptests[i].test_id == test_id) {
				phealth_info->recently_test_id = test_id;
				if (NULL != phealth_info->ptests[i].test_cb) {
					phealth_info->ptests[i].test_cb(model, company_id, test_id);
				}
				break;
			}
		}
	}

	rtk_bt_mesh_health_server_fault_test_t *fault_test;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL, RTK_BT_MESH_HEALTH_SERVER_MODEL_FAULT_TEST,
								sizeof(rtk_bt_mesh_health_server_fault_test_t) + HEALTH_FAULT_MAX_NUM + 2);
	fault_test = (rtk_bt_mesh_health_server_fault_test_t *)p_evt->data;
	fault_test->company_id = company_id;
	fault_test->test_id = test_id;
	fault_test->fault_array = fault_test_status;
	rtk_bt_evt_indicate(p_evt, NULL);
	return 0;
}

static uint8_t current_store[4 + HEALTH_FAULT_MAX_NUM];
static int health_fault_get_cur(const struct bt_mesh_model *model, uint8_t *test_id,
								uint16_t *company_id, uint8_t *faults,
								uint8_t *fault_count)
{
	(void)model;
	struct bt_mesh_health_srv *srv = model->rt->user_data;
	health_info_p phealth_info = srv->health_info;
	*test_id = phealth_info->recently_test_id;
	*company_id = phealth_info->company_id;
	*fault_count = health_server_fault_count_internal(phealth_info->current_faults);
	health_server_fill_fault(faults, phealth_info->current_faults);

	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL, RTK_BT_MESH_HEALTH_SERVER_MODEL_CURRENT_GET,
								4 + HEALTH_FAULT_MAX_NUM);
	p_evt->data = current_store;
	rtk_bt_evt_indicate(p_evt, NULL);
	// current_store style: 1 byte(length) + 1 byte(test id) + 2 byte(company id) + n bytes(faults)
	// faults length = length - 1

	// memcpy(test_id, &current_store[1], 1);
	// memcpy(company_id, &current_store[2], 2);
	// memcpy(faults, &current_store[4], current_store[0] - 3);
	// *fault_count = current_store[0] - 3;

	return 0;
}

static const struct bt_mesh_health_srv_cb health_cb = {
	.attn_on = attention_on,
	.attn_off = attention_off,
	.fault_clear = health_fault_clear,
	.fault_get_reg = health_fault_get_reg,
	.fault_test = health_fault_test,
	.fault_get_cur = health_fault_get_cur,
};

struct bt_mesh_health_srv health_server = {
	.cb = &health_cb,
	.health_info = &health_info,
};

// BT_MESH_HEALTH_PUB_DEFINE(health_pub, 0);


void health_srv_indicate_attention_to_app(uint8_t attn)
{
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL, RTK_BT_MESH_HEALTH_SERVER_MODEL_ATTN_SET, sizeof(rtk_bt_mesh_health_server_attn_set_t));
	memcpy(p_evt->data, &attn, sizeof(rtk_bt_mesh_health_server_attn_set_t));
	rtk_bt_evt_indicate(p_evt, NULL);
}

uint8_t health_srv_indicate_attention_from_app(void)
{
	rtk_bt_mesh_health_server_attn_get_t *attn_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL, RTK_BT_MESH_HEALTH_SERVER_MODEL_ATTN_GET,
								sizeof(rtk_bt_mesh_health_server_attn_get_t));
	uint8_t attn = 0;
	attn_get = (rtk_bt_mesh_health_server_attn_get_t *)p_evt->data;
	attn_get->attn = &attn;
	rtk_bt_evt_indicate(p_evt, NULL);
	return attn;
}

void health_srv_indicate_period_to_app(uint8_t fast_period_divisor)
{
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL, RTK_BT_MESH_HEALTH_SERVER_MODEL_PERIOD_SET, sizeof(rtk_bt_mesh_health_server_period_set_t));
	memcpy(p_evt->data, &fast_period_divisor, sizeof(rtk_bt_mesh_health_server_period_set_t));
	rtk_bt_evt_indicate(p_evt, NULL);
}

uint8_t health_srv_indicate_period_from_app(void)
{
	rtk_bt_mesh_health_server_period_get_t *period_get;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_HEALTH_SERVER_MODEL, RTK_BT_MESH_HEALTH_SERVER_MODEL_PERIOD_GET,
								sizeof(rtk_bt_mesh_health_server_period_get_t));
	period_get = (rtk_bt_mesh_health_server_period_get_t *)p_evt->data;
	uint8_t fast_period_divisor = 0;
	period_get->fast_period_divisor = &fast_period_divisor;
	rtk_bt_evt_indicate(p_evt, NULL);
	return fast_period_divisor;
}

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
uint16_t bt_mesh_health_server_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_FAULT_REGISTER: {
		rtk_bt_mesh_health_server_fault_register_act_t *fault_register = (rtk_bt_mesh_health_server_fault_register_act_t *)p_cmd->param;
		health_server_fault_register(health_server.model, fault_register->fault);
		ret = RTK_BT_OK;
		break;
	}
	case RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_CURRENT_FAULT_CLEAR: {
		rtk_bt_mesh_health_server_fault_clear_act_t *fault_clear = (rtk_bt_mesh_health_server_fault_clear_act_t *)p_cmd->param;
		health_server_fault_clear(health_server.model, fault_clear->fault);
		ret = RTK_BT_OK;
		break;
	}
	case RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_CURRENT_FAULT_CLEAR_ALL: {
		health_server_fault_clear_all(health_server.model);
		ret = RTK_BT_OK;
		break;
	}
	case RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_FAULT_COUNT: {
		rtk_bt_mesh_health_server_count_fault_act_t *fault_count = (rtk_bt_mesh_health_server_count_fault_act_t *)p_cmd->param;
		fault_count->fault_count = health_server_fault_count(health_server.model);
		ret = RTK_BT_OK;
		break;
	}
	case RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_CHECK_FAULT_IS_SET: {
		rtk_bt_mesh_health_server_check_fault_act_t *fault_check = (rtk_bt_mesh_health_server_check_fault_act_t *)p_cmd->param;
		fault_check->set = health_server_fault_is_set(health_server.model, fault_check->fault);
		ret = RTK_BT_OK;
		break;
	}
	case RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_TEST_SET: {
		rtk_bt_mesh_health_server_test_set_t *test_set = (rtk_bt_mesh_health_server_test_set_t *)p_cmd->param;
		health_server_set_tests(health_server.model, test_set->test, test_set->num_tests);
		ret = RTK_BT_OK;
		break;
	}
	case RTK_BT_MESH_HEALTH_SERVER_MODEL_ACT_COMPANY_ID_SET: {
		rtk_bt_mesh_health_server_company_id_set_t *company_id_set = (rtk_bt_mesh_health_server_company_id_set_t *)p_cmd->param;
		health_server_set_company_id(health_server.model, company_id_set->company_id);
		ret = RTK_BT_OK;
		break;
	}
	default:
		BT_LOGE("[%s] Unknown p_cmd->act:%d\r\n", __func__, p_cmd->act);
		break;
	}
end:
	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}
#endif // RTK_BLE_MESH_DEVICE_SUPPORT

#endif