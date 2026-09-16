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
#include <rtk_bt_mesh_scene_model.h>
#include <msg.h>

#define OP_SCENE_GET                     BT_MESH_MODEL_OP_2(0x82, 0x41)
#define OP_SCENE_RECALL                  BT_MESH_MODEL_OP_2(0x82, 0x42)
#define OP_SCENE_RECALL_UNACK            BT_MESH_MODEL_OP_2(0x82, 0x43)
#define OP_SCENE_STATUS                  BT_MESH_MODEL_OP_1(0x5E)
#define OP_SCENE_REGISTER_GET            BT_MESH_MODEL_OP_2(0x82, 0x44)
#define OP_SCENE_REGISTER_STATUS         BT_MESH_MODEL_OP_2(0x82, 0x45)

#define OP_SCENE_STORE                   BT_MESH_MODEL_OP_2(0x82, 0x46)
#define OP_SCENE_STORE_UNACK             BT_MESH_MODEL_OP_2(0x82, 0x47)
#define OP_SCENE_DELETE                  BT_MESH_MODEL_OP_2(0x82, 0x9E)
#define OP_SCENE_DELETE_UNACK            BT_MESH_MODEL_OP_2(0x82, 0x9F)

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
#if ((defined(BT_MESH_ENABLE_SCENE_SERVER_MODEL) && BT_MESH_ENABLE_SCENE_SERVER_MODEL) || \
    (defined(BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL))
typedef struct {
	rtk_bt_mesh_scene_storage_memory_t *scenes;
	uint16_t num_scenes;
} scene_info_t;

typedef struct {
	rtk_bt_mesh_scene_storage_memory_t *scenes;
	uint16_t num_scenes;
	rtk_bt_mesh_scene_status_code_t status_register;
} scene_setup_info_t;

static int scene_register_status(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
								 rtk_bt_mesh_scene_status_code_t status, uint16_t current_scene)
{
	/**
	 * This part is to verify if get the valid scenes number and scenes success from the app layer
	 * when the database is in or above upper app layer
	*/
	// uint8_t group = 0;
	// uint8_t evt_num = 0;
	// uint8_t evt_scenes = 0;
	// if (model->id == BT_MESH_MODEL_ID_SCENE_SRV) {
	//     group = RTK_BT_LE_GP_MESH_SCENE_SERVER_MODEL;
	//     evt_num = RTK_BT_MESH_SCENE_SERVER_MODEL_GET_SCENES_NUM;
	//     evt_scenes = RTK_BT_MESH_SCENE_SERVER_MODEL_GET_SCENES;
	// } else if (model->id == BT_MESH_MODEL_ID_SCENE_SETUP_SRV) {
	//     group = RTK_BT_LE_GP_MESH_SCENE_SETUP_SERVER_MODEL;
	//     evt_num = RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_GET_SCENES_NUM;
	//     evt_scenes = RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_GET_SCENES;
	// }
	// uint16_t scenes_num_get = 0;
	// rtk_bt_mesh_scene_server_get_scenes_t scenes_get = {0, NULL};
	// rtk_bt_mesh_scene_server_get_scenes_num_t *p_get_data;
	// rtk_bt_evt_t *p_evt = NULL;
	// p_evt = rtk_bt_event_create(group, evt_num,
	//                             sizeof(rtk_bt_mesh_scene_server_get_scenes_num_t));
	// p_get_data = (rtk_bt_mesh_scene_server_get_scenes_num_t *)p_evt->data;
	// p_get_data->num_scenes = &scenes_num_get;
	// rtk_bt_evt_indicate(p_evt, NULL);
	// BT_LOGA("Get scenes number %d \r\n", scenes_num_get);
	// if(scenes_num_get != 0) {
	//     scenes_get.num_scenes = scenes_num_get;

	//     rtk_bt_mesh_scene_server_get_scenes_t *p_get_scenes;
	//     p_evt = rtk_bt_event_create(group, evt_scenes,
	//                                 sizeof(rtk_bt_mesh_scene_server_get_scenes_t) + scenes_num_get * sizeof(uint16_t));
	//     p_get_scenes = (rtk_bt_mesh_scene_server_get_scenes_t *)p_evt->data;
	//     p_get_scenes->num_scenes = scenes_get.num_scenes;
	//     uint16_t sample_datas[SCENE_DATA_MAX_LEN];
	//     p_get_scenes->scenes = sample_datas;
	//     rtk_bt_evt_indicate(p_evt, NULL);
	//     BT_DUMP16A("", sample_datas, scenes_get.num_scenes);
	// }

	scene_info_t *pinfo = model->rt->user_data;
	uint16_t valid_scene_count = 0;
	uint16_t scenes[SCENE_REGISTER_ARRAY_DATA_MAX_LEN] = {0};
	for (uint16_t i = 0; i < pinfo->num_scenes; ++i) {
		if (pinfo->scenes[i].scene_number != 0) {
			scenes[valid_scene_count] = pinfo->scenes[i].scene_number;
			valid_scene_count ++;
		}
	}
	uint16_t msg_len = 3 + valid_scene_count * sizeof(uint16_t);
	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SCENE_REGISTER_STATUS, msg_len);
	bt_mesh_model_msg_init(&pmsg, OP_SCENE_REGISTER_STATUS);
	net_buf_simple_add_u8(&pmsg, status);
	net_buf_simple_add_le16(&pmsg, current_scene);
	net_buf_simple_add_mem(&pmsg, scenes, valid_scene_count * sizeof(uint16_t));
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

uint16_t get_current_scene(rtk_bt_le_group_t model)
{
	uint16_t current_scene;
	rtk_bt_mesh_scene_server_get_t *p_get_data;
	rtk_bt_evt_t *p_evt = NULL;
	if (model == RTK_BT_LE_GP_MESH_SCENE_SERVER_MODEL) {
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SCENE_SERVER_MODEL, RTK_BT_MESH_SCENE_SERVER_MODEL_GET,
									sizeof(rtk_bt_mesh_scene_server_get_t));
	} else if (model == RTK_BT_LE_GP_MESH_SCENE_SETUP_SERVER_MODEL) {
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SCENE_SETUP_SERVER_MODEL, RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_GET,
									sizeof(rtk_bt_mesh_scene_server_get_t));
	}

	if (p_evt != NULL) {
		p_get_data = (rtk_bt_mesh_scene_server_get_t *)p_evt->data;
		p_get_data->current_scene = &current_scene;
		rtk_bt_evt_indicate(p_evt, NULL);
		return current_scene;
	}
	return 0;
}

uint8_t store_value[16] = {0x1, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};
uint8_t store_value2[16] = {0x3, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};
uint8_t store_value3[16] = {0};
uint8_t store_value4[16] = {0};
static rtk_bt_mesh_scene_storage_memory_t scenes[] = {
	{1, store_value},
	{2, store_value2},
	{0, store_value3},
	{0, store_value4},
};
#endif

#if defined(BT_MESH_ENABLE_SCENE_SERVER_MODEL) && BT_MESH_ENABLE_SCENE_SERVER_MODEL
static const struct bt_mesh_model *scene_server_model = NULL;
struct k_mutex mutex_scene;

struct scene_pa_t {
	uint16_t scene_num;
	uint16_t target_scene_num;
	rtk_bt_mesh_scene_storage_memory_t *state_memory;
	rtk_bt_mesh_scene_status_code_t status_recall;
	uint16_t src;
	uint8_t tid;
	bool transition_flag;
	rtk_bt_mesh_generic_transition_time_t total_time;
	rtk_bt_mesh_generic_transition_time_t remain_time;
	struct k_work_delayable work;
} scene_pa;

scene_info_t scene_model_info = {scenes, 4};

static const uint32_t time_res[] = {
	100,
	MSEC_PER_SEC,
	10 * MSEC_PER_SEC,
	10 * 60 * MSEC_PER_SEC,
};

void scene_timeout(struct k_work *work)
{
	(void)work;
	k_mutex_lock(&mutex_scene, K_FOREVER);
	int32_t trans = time_res[scene_pa.remain_time.step_resolution];
	rtk_bt_evt_t *p_evt = NULL;
	if (!scene_pa.transition_flag) {
		scene_pa.remain_time.num_steps = 0;
		scene_pa.total_time.num_steps = 0;
	}
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SCENE_SERVER_MODEL, RTK_BT_MESH_SCENE_SERVER_MODEL_RECALL, SCENE_DATA_MAX_LEN + 6);
	memcpy((uint8_t *)p_evt->data, &scene_pa.target_scene_num, 2);
	memcpy((uint8_t *)(p_evt->data) + 2, &scene_pa.total_time, 1);
	memcpy((uint8_t *)(p_evt->data) + 3, &scene_pa.remain_time, 1);
	memcpy((uint8_t *)(p_evt->data) + 4, scene_pa.state_memory->pmemory, SCENE_DATA_MAX_LEN);
	rtk_bt_evt_indicate(p_evt, NULL);
	if (scene_pa.remain_time.num_steps) {
		scene_pa.remain_time.num_steps--;
		k_work_reschedule(&scene_pa.work, K_MSEC(trans));
	} else {
		scene_pa.transition_flag = FALSE;
	}
	k_mutex_unlock(&mutex_scene);
	return;
}

static int scene_srv_init(const struct bt_mesh_model *model)
{
	scene_server_model = model;
	k_work_init_delayable(&scene_pa.work, scene_timeout);
	k_mutex_init(&mutex_scene);
	scene_pa.transition_flag = FALSE;
	return 0;
}

#if ZEPHYR_RTK_PATCH
static int scene_srv_deinit(const struct bt_mesh_model *model)
{
	(void)model;
	k_mutex_deinit(&mutex_scene);
	return 0;
}
#endif

const struct bt_mesh_model_cb bt_mesh_scene_srv_cb = {
	.init = scene_srv_init,
#if ZEPHYR_RTK_PATCH
	.deinit = scene_srv_deinit,
#endif
};

static rtk_bt_mesh_scene_storage_memory_t *scene_storage_memory_get(const struct bt_mesh_model *model,
																	uint16_t scene_number)
{
	scene_info_t *pinfo = model->rt->user_data;
	for (uint16_t i = 0; i < pinfo->num_scenes; ++i) {
		if (pinfo->scenes[i].scene_number == scene_number) {
			return &pinfo->scenes[i];
		}
	}

	return NULL;
}

void scene_server_set_storage_memory(const struct bt_mesh_model *model, rtk_bt_mesh_scene_storage_memory_t *scenes,
									 uint16_t num_scenes)
{
	scene_info_t *pinfo = model->rt->user_data;
	pinfo->scenes = scenes;
	pinfo->num_scenes = num_scenes;
}

static int scene_status(const struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, rtk_bt_mesh_scene_status_code_t status, uint16_t current_scene,
						bool optional, uint16_t target_scene, rtk_bt_mesh_generic_transition_time_t remaining_time)
{
	uint16_t msg_len = 3;
	if (optional) {
		msg_len = 6;
	}
	BT_MESH_MODEL_BUF_DEFINE(pmsg, OP_SCENE_STATUS, msg_len);
	bt_mesh_model_msg_init(&pmsg, OP_SCENE_STATUS);
	net_buf_simple_add_u8(&pmsg, status);
	net_buf_simple_add_le16(&pmsg, current_scene);
	if (optional) {
		net_buf_simple_add_le16(&pmsg, target_scene);
		net_buf_simple_add_mem(&pmsg, &remaining_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
	}
	return bt_mesh_msg_send(model, ctx, &pmsg);
}

int scene_publish(const struct bt_mesh_model *model, rtk_bt_mesh_scene_status_code_t status, uint16_t current_scene)
{
	rtk_bt_mesh_generic_transition_time_t remaining_time = {0, 0};
	return scene_status(model, NULL, status, current_scene, FALSE, 0, remaining_time);
}

int scene_srv_update_publish(const struct bt_mesh_model *model)
{
	net_buf_simple_reset(model->pub->msg);
	struct net_buf_simple *msg = model->pub->msg;
	uint16_t scene = get_current_scene(RTK_BT_LE_GP_MESH_SCENE_SERVER_MODEL);

	bt_mesh_model_msg_init(msg, OP_SCENE_STATUS);
	k_mutex_lock(&mutex_scene, K_FOREVER);
	net_buf_simple_add_u8(msg, scene_pa.status_recall);
	k_mutex_unlock(&mutex_scene);
	net_buf_simple_add_le16(msg, scene);
	return 0;
}

static int scene_srv_get(const struct bt_mesh_model *model,
						 struct bt_mesh_msg_ctx *ctx,
						 struct net_buf_simple *buf)
{
	if (!buf->len) {
		uint16_t scene = get_current_scene(RTK_BT_LE_GP_MESH_SCENE_SERVER_MODEL);
		k_mutex_lock(&mutex_scene, K_FOREVER);
		rtk_bt_mesh_generic_transition_time_t remain_time = scene_pa.remain_time;
		if (scene_pa.transition_flag && remain_time.num_steps < scene_pa.total_time.num_steps) {
			remain_time.num_steps++;
		}
		scene_status(model, ctx, scene_pa.status_recall, scene, scene_pa.transition_flag,
					 scene_pa.target_scene_num, remain_time);
		k_mutex_unlock(&mutex_scene);
		return 0;
	}
	return -1;
}

static int scene_srv_recall_unack(const struct bt_mesh_model *model,
								  struct bt_mesh_msg_ctx *ctx,
								  struct net_buf_simple *buf)
{
	if (buf->len) {
		uint16_t scene_num = net_buf_simple_pull_le16(buf);
		if (scene_num != 0) {
			rtk_bt_mesh_scene_storage_memory_t *state_memory = scene_storage_memory_get(model, scene_num);
			k_mutex_lock(&mutex_scene, K_FOREVER);
			if (NULL == state_memory) {
				scene_pa.status_recall = RTK_BT_MESH_SCENE_STATUS_NOT_FOUND;
				k_mutex_unlock(&mutex_scene);
			} else {
				uint8_t tid = net_buf_simple_pull_u8(buf);
				if (tid == scene_pa.tid && ctx->addr == scene_pa.src) {
					/* Duplicate */
					BT_LOGE("[%s] Same message, transition id is equal to last one\r\n", __func__);
					k_mutex_unlock(&mutex_scene);
					return 0;
				}
				scene_pa.status_recall = RTK_BT_MESH_SCENE_STATUS_SUCCESS;
				int32_t delay = 0;
				if (buf->len) {
					uint8_t trans_time = net_buf_simple_pull_u8(buf);
					scene_pa.total_time.num_steps = trans_time & 0x3F;
					scene_pa.total_time.step_resolution = (trans_time >> 6) & 0x03;
					scene_pa.remain_time = scene_pa.total_time;
					scene_pa.transition_flag = TRUE;
					scene_pa.state_memory = state_memory;
					delay = net_buf_simple_pull_u8(buf) * 5;
				} else {
					scene_pa.total_time.num_steps = 0;
					scene_pa.total_time.step_resolution = 0;
					scene_pa.remain_time = scene_pa.total_time;
					scene_pa.transition_flag = FALSE;

					scene_pa.scene_num = scene_num;
					scene_pa.target_scene_num = scene_num;
					scene_pa.src = ctx->addr;
					scene_pa.tid = tid;

					/*****if no transition, call set here directly, then return******/
					rtk_bt_evt_t *p_evt = NULL;
					p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SCENE_SERVER_MODEL, RTK_BT_MESH_SCENE_SERVER_MODEL_RECALL, SCENE_DATA_MAX_LEN + 6);
					memcpy((uint8_t *)p_evt->data, &scene_num, 2);
					memcpy((uint8_t *)(p_evt->data) + 2, &scene_pa.total_time, 1);
					memcpy((uint8_t *)(p_evt->data) + 3, &scene_pa.remain_time, 1);
					memcpy((uint8_t *)(p_evt->data) + 4, state_memory->pmemory, SCENE_DATA_MAX_LEN);
					rtk_bt_evt_indicate(p_evt, NULL);
					k_mutex_unlock(&mutex_scene);
					return 0;
				}
				scene_pa.scene_num = scene_num;
				scene_pa.target_scene_num = scene_num;
				scene_pa.src = ctx->addr;
				scene_pa.tid = tid;
				/* Schedule the next action to happen on the delay, and keep
				* transition time stored, so it can be applied in the timeout.
				*/
				k_work_reschedule(&scene_pa.work, K_MSEC(delay));
				k_mutex_unlock(&mutex_scene);
				return 0;
			}

		}
		return -1;
	}
	return -1;
}

static int scene_srv_recall(const struct bt_mesh_model *model,
							struct bt_mesh_msg_ctx *ctx,
							struct net_buf_simple *buf)
{
	scene_srv_recall_unack(model, ctx, buf);
	uint16_t scene = 0;
	scene = get_current_scene(RTK_BT_LE_GP_MESH_SCENE_SERVER_MODEL);
	k_mutex_lock(&mutex_scene, K_FOREVER);
	rtk_bt_mesh_generic_transition_time_t remain_time = scene_pa.remain_time;
	bool trans_flag = scene_pa.transition_flag;
	uint16_t target_scene_num = scene_pa.target_scene_num;
	rtk_bt_mesh_scene_status_code_t status_recall = scene_pa.status_recall;
	if (trans_flag && remain_time.num_steps < scene_pa.total_time.num_steps) {
		remain_time.num_steps++;
	}
	/**
	 * This part is to verify if get the Recall status success from the app layer
	 * when the database is in or above upper app layer
	*/
	// rtk_bt_mesh_scene_status_code_t status_recall_stat = 0;
	// rtk_bt_mesh_scene_server_get_status_recall_t *p_get_data;
	// rtk_bt_evt_t *p_evt = NULL;
	// p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SCENE_SERVER_MODEL, RTK_BT_MESH_SCENE_SERVER_MODEL_GET_STATUS_RECALL,
	//                             sizeof(rtk_bt_mesh_scene_server_get_status_recall_t));
	// p_get_data = (rtk_bt_mesh_scene_server_get_status_recall_t *)p_evt->data;
	// p_get_data->scene_number = scene;
	// p_get_data->status_recall = &status_recall_stat;
	// rtk_bt_evt_indicate(p_evt, NULL);
	// BT_LOGA("Recall status:%d \r\n", status_recall_stat);

	k_mutex_unlock(&mutex_scene);
	scene_status(model, ctx, status_recall, scene, trans_flag,
				 target_scene_num, remain_time);
	return 0;
}

static int scene_srv_register_get(const struct bt_mesh_model *model,
								  struct bt_mesh_msg_ctx *ctx,
								  struct net_buf_simple *buf)
{
	if (!buf->len) {
		uint16_t current_scene = get_current_scene(RTK_BT_LE_GP_MESH_SCENE_SERVER_MODEL);
		rtk_bt_mesh_scene_status_code_t status;
		rtk_bt_mesh_scene_server_get_register_status_t *p_get_data;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SCENE_SERVER_MODEL, RTK_BT_MESH_SCENE_SERVER_MODEL_GET_REGISTER_STATUS,
									sizeof(rtk_bt_mesh_scene_server_get_register_status_t));
		p_get_data = (rtk_bt_mesh_scene_server_get_register_status_t *)p_evt->data;
		p_get_data->status = &status;
		rtk_bt_evt_indicate(p_evt, NULL);
		return scene_register_status(model, ctx, status, current_scene);
	}
	return -1;
}

const struct bt_mesh_model_op scene_srv_op[] = {
	{ OP_SCENE_GET,                 BT_MESH_LEN_EXACT(0),     scene_srv_get },
	{ OP_SCENE_RECALL,              BT_MESH_LEN_MIN(3),       scene_srv_recall },
	{ OP_SCENE_RECALL_UNACK,        BT_MESH_LEN_MIN(3),       scene_srv_recall_unack },
	{ OP_SCENE_REGISTER_GET,        BT_MESH_LEN_EXACT(0),     scene_srv_register_get },
	BT_MESH_MODEL_OP_END,
};

uint16_t bt_mesh_scene_server_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_SCENE_SERVER_MODEL_ACT_DB_SET: {
		rtk_bt_mesh_scene_server_db_set_t *db_get = (rtk_bt_mesh_scene_server_db_set_t *)p_cmd->param;
		scene_server_set_storage_memory(scene_server_model, db_get->pdata_db, db_get->num_scenes);
		ret = 0;
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

#if defined(BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL) && BT_MESH_ENABLE_SCENE_SETUP_SERVER_MODEL
static const struct bt_mesh_model *scene_setup_server_model = NULL;

static int scene_setup_srv_init(const struct bt_mesh_model *model)
{
	scene_setup_server_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_scene_setup_srv_cb = {
	.init = scene_setup_srv_init,
};

scene_setup_info_t scene_setup_model_info = {scenes, 4, RTK_BT_MESH_SCENE_STATUS_SUCCESS};

void scene_setup_server_set_storage_memory(const struct bt_mesh_model *model,
										   rtk_bt_mesh_scene_storage_memory_t *scenes, uint16_t num_scenes)
{
	scene_setup_info_t *info = model->rt->user_data;
	info->scenes = scenes;
	info->num_scenes = num_scenes;
}

uint8_t scene_store_data[SCENE_DATA_MAX_LEN];
static int scene_setup_srv_store_unack(const struct bt_mesh_model *model,
									   struct bt_mesh_msg_ctx *ctx,
									   struct net_buf_simple *buf)
{
	(void)ctx;
	uint16_t scene_num = net_buf_simple_pull_le16(buf);
	if (scene_num != 0) {
		scene_setup_info_t *pinfo = model->rt->user_data;
		uint16_t index = 0;
		uint16_t empty_index = pinfo->num_scenes;
		uint16_t real_index = 0;
		/* find store place */
		for (index = 0; index < pinfo->num_scenes; ++index) {
			if (0 == pinfo->scenes[index].scene_number) {
				/* store to last empty location, just for easy process */
				empty_index = index;
			}

			if (scene_num == pinfo->scenes[index].scene_number) {
				break;
			}
		}
		rtk_bt_mesh_scene_server_store_t store_data = {RTK_BT_MESH_SCENE_STATUS_SUCCESS, scene_num, NULL};
		pinfo->status_register = RTK_BT_MESH_SCENE_STATUS_SUCCESS;
		store_data.status = RTK_BT_MESH_SCENE_STATUS_SUCCESS;
		if (index < pinfo->num_scenes) {
			/* update exists */
			store_data.pmemory = pinfo->scenes[index].pmemory;
			real_index = index;
		} else if (empty_index < pinfo->num_scenes) {
			/* store new */
			store_data.scene_number = scene_num;
			pinfo->scenes[empty_index].scene_number = scene_num;
			store_data.pmemory = pinfo->scenes[empty_index].pmemory;
			real_index = empty_index;
		} else {
			/* no place for store */
			pinfo->status_register = RTK_BT_MESH_SCENE_STATUS_REGISTER_FULL;
			store_data.status = RTK_BT_MESH_SCENE_STATUS_REGISTER_FULL;
			store_data.scene_number = 0;
			store_data.pmemory = NULL;
			real_index = pinfo->num_scenes;
		}
		rtk_bt_mesh_scene_server_store_t *scene_store;
		rtk_bt_evt_t *p_evt = NULL;
		p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SCENE_SETUP_SERVER_MODEL, RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_STORE,
									SCENE_DATA_MAX_LEN + 3);
		scene_store = (rtk_bt_mesh_scene_server_store_t *)p_evt->data;
		scene_store->status = store_data.status;
		scene_store->scene_number = store_data.scene_number;
		memset(scene_store_data, 0, sizeof(scene_store_data));
		scene_store->pmemory = scene_store_data;
		rtk_bt_evt_indicate(p_evt, NULL);
		if (real_index < pinfo->num_scenes) {
			// pinfo->scenes[real_index].pmemory = scene_store_data; //reserve 2 bytes to store length
			memcpy(pinfo->scenes[real_index].pmemory, scene_store_data, SCENE_DATA_MAX_LEN);
		}
		return 0;
	}
	return -1;
}

static int scene_setup_srv_store(const struct bt_mesh_model *model,
								 struct bt_mesh_msg_ctx *ctx,
								 struct net_buf_simple *buf)
{
	(void)ctx;
	scene_setup_srv_store_unack(model, ctx, buf);
	uint16_t current_scene = get_current_scene(RTK_BT_LE_GP_MESH_SCENE_SETUP_SERVER_MODEL);
	/**
	 * This part is to verify if get the Register status success from the app layer
	 * when the database is in or above upper app layer
	*/
	// rtk_bt_mesh_scene_status_code_t status_store_stat = 4;
	// rtk_bt_mesh_scene_setup_server_get_register_status_t *p_get_data;
	// rtk_bt_evt_t *p_evt = NULL;
	// p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SCENE_SETUP_SERVER_MODEL, RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_GET_STATUS_REGISTER,
	//                             sizeof(rtk_bt_mesh_scene_server_get_register_status_t));
	// p_get_data = (rtk_bt_mesh_scene_setup_server_get_register_status_t *)p_evt->data;
	// p_get_data->scene_number = current_scene;
	// p_get_data->status = &status_store_stat;
	// rtk_bt_evt_indicate(p_evt, NULL);
	// BT_LOGA("Recall status:%d \r\n", status_store_stat);

	scene_setup_info_t *pinfo = model->rt->user_data;
	return scene_register_status(model, ctx, pinfo->status_register, current_scene);
}

static int scene_setup_srv_delete_unack(const struct bt_mesh_model *model,
										struct bt_mesh_msg_ctx *ctx,
										struct net_buf_simple *buf)
{
	(void)ctx;
	uint16_t scene_num = net_buf_simple_pull_le16(buf);
	if (scene_num != 0) {
		/* find store index */
		scene_setup_info_t *pinfo = model->rt->user_data;
		uint16_t index = 0;
		for (index = 0; index < pinfo->num_scenes; ++index) {
			if (scene_num == pinfo->scenes[index].scene_number) {
				break;
			}
		}

		pinfo->status_register = RTK_BT_MESH_SCENE_STATUS_SUCCESS;
		if (index < pinfo->num_scenes) {
			/* delete exists */
			pinfo->scenes[index].scene_number = 0;
		} else {
			/* no scene found */
			pinfo->status_register = RTK_BT_MESH_SCENE_STATUS_NOT_FOUND;
		}
		if (RTK_BT_MESH_SCENE_STATUS_SUCCESS == pinfo->status_register) {
			rtk_bt_mesh_scene_server_delete_t delete_data = {scene_num};
			rtk_bt_evt_t *p_evt = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SCENE_SETUP_SERVER_MODEL, RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_DELETE, sizeof(rtk_bt_mesh_scene_server_delete_t));
			memcpy(p_evt->data, &delete_data, sizeof(rtk_bt_mesh_scene_server_delete_t));
			rtk_bt_evt_indicate(p_evt, NULL);
		}
	}
	return 0;
}

static int scene_setup_srv_delete(const struct bt_mesh_model *model,
								  struct bt_mesh_msg_ctx *ctx,
								  struct net_buf_simple *buf)
{
	scene_setup_srv_delete_unack(model, ctx, buf);
	uint16_t current_scene = get_current_scene(RTK_BT_LE_GP_MESH_SCENE_SETUP_SERVER_MODEL);
	scene_setup_info_t *pinfo = model->rt->user_data;
	return scene_register_status(model, ctx, pinfo->status_register, current_scene);
}

const struct bt_mesh_model_op scene_setup_srv_op[] = {
	{ OP_SCENE_STORE,               BT_MESH_LEN_EXACT(2),     scene_setup_srv_store },
	{ OP_SCENE_STORE_UNACK,         BT_MESH_LEN_EXACT(2),     scene_setup_srv_store_unack },
	{ OP_SCENE_DELETE,              BT_MESH_LEN_EXACT(2),     scene_setup_srv_delete },
	{ OP_SCENE_DELETE_UNACK,        BT_MESH_LEN_EXACT(2),     scene_setup_srv_delete_unack },
	BT_MESH_MODEL_OP_END,
};

uint16_t bt_mesh_scene_setup_server_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_SCENE_SETUP_SERVER_MODEL_ACT_DB_SET: {
		rtk_bt_mesh_scene_server_db_set_t *db_get = (rtk_bt_mesh_scene_server_db_set_t *)p_cmd->param;
		scene_setup_server_set_storage_memory(scene_setup_server_model, db_get->pdata_db, db_get->num_scenes);
		ret = 0;
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

#if defined(BT_MESH_ENABLE_SCENE_CLIENT_MODEL) && BT_MESH_ENABLE_SCENE_CLIENT_MODEL
static const struct bt_mesh_model *scene_client_model = NULL;

static int scene_cli_init(const struct bt_mesh_model *model)
{
	scene_client_model = model;
	return 0;
}

const struct bt_mesh_model_cb bt_mesh_scene_cli_cb = {
	.init = scene_cli_init,
};

static int scene_cli_status(const struct bt_mesh_model *model,
							struct bt_mesh_msg_ctx *ctx,
							struct net_buf_simple *buf)
{
	(void)model;
	rtk_bt_mesh_scene_client_status_t scene_stat;
	scene_stat.src = ctx->addr;
	scene_stat.status = net_buf_simple_pull_u8(buf);
	scene_stat.current_scene = net_buf_simple_pull_le16(buf);
	scene_stat.optional = FALSE;
	if (buf->len) {
		scene_stat.optional = TRUE;
		scene_stat.target_scene = net_buf_simple_pull_le16(buf);
		scene_stat.remaining_time = *(rtk_bt_mesh_generic_transition_time_t *)(net_buf_simple_pull_mem(buf, sizeof(rtk_bt_mesh_generic_transition_time_t)));
	}
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SCENE_CLIENT_MODEL, RTK_BT_MESH_SCENE_CLIENT_MODEL_STATUS,
								sizeof(rtk_bt_mesh_scene_client_status_t));
	memcpy(p_evt->data, &scene_stat, sizeof(rtk_bt_mesh_scene_client_status_t));
	return rtk_bt_evt_indicate(p_evt, NULL);
}

static int scene_cli_register_status(const struct bt_mesh_model *model,
									 struct bt_mesh_msg_ctx *ctx,
									 struct net_buf_simple *buf)
{
	(void)model;
	uint8_t status = net_buf_simple_pull_u8(buf);
	uint16_t current_scene = net_buf_simple_pull_le16(buf);
	uint8_t scene_array_len = 0;
	if (buf->len) {
		scene_array_len = buf->len / 2;
	}
	uint32_t len = 6 + buf->len;
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_SCENE_CLIENT_MODEL, RTK_BT_MESH_SCENE_CLIENT_MODEL_REGISTER_STATUS,
								len);
	memcpy(p_evt->data, &(ctx->addr), 2);
	memcpy((uint8_t *)(p_evt->data) + 2, &status, 1);
	memcpy((uint8_t *)(p_evt->data) + 3, &current_scene, 2);
	memcpy((uint8_t *)(p_evt->data) + 5, &scene_array_len, 1);
	memcpy((uint8_t *)(p_evt->data) + 6, buf->data, buf->len);
	return rtk_bt_evt_indicate(p_evt, NULL);
}

const struct bt_mesh_model_op scene_cli_op[] = {
	{ OP_SCENE_STATUS,               BT_MESH_LEN_MIN(3),      scene_cli_status },
	{ OP_SCENE_REGISTER_STATUS,      BT_MESH_LEN_MIN(3),      scene_cli_register_status },
	BT_MESH_MODEL_OP_END,
};

static int scene_store(const struct bt_mesh_model *model, uint16_t dst,
					   uint16_t app_key_index, uint16_t scene_number, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_SCENE_STORE : OP_SCENE_STORE_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_SCENE_STORE, 2);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, scene_number);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int scene_recall(const struct bt_mesh_model *model, uint16_t dst,
						uint16_t app_key_index, uint16_t scene_number, uint8_t tid, bool optional,
						rtk_bt_mesh_generic_transition_time_t trans_time, uint8_t delay, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint16_t msg_len = 3;
	if (optional) {
		msg_len = 5;
	}
	uint32_t op = ack ? OP_SCENE_RECALL : OP_SCENE_RECALL_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_SCENE_RECALL, msg_len);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, scene_number);
	net_buf_simple_add_u8(&buf, tid);
	if (optional) {
		net_buf_simple_add_mem(&buf, &trans_time, sizeof(rtk_bt_mesh_generic_transition_time_t));
		net_buf_simple_add_u8(&buf, delay);
	}
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int scene_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_SCENE_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_SCENE_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int scene_register_get(const struct bt_mesh_model *model, uint16_t dst, uint16_t app_key_index)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_SCENE_REGISTER_GET, 0);
	bt_mesh_model_msg_init(&buf, OP_SCENE_REGISTER_GET);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

static int scene_delete(const struct bt_mesh_model *model, uint16_t dst,
						uint16_t app_key_index, uint16_t scene_number, bool ack)
{
	struct bt_mesh_msg_ctx ctx = BT_MESH_MSG_CTX_INIT_APP(app_key_index, dst);
	uint32_t op = ack ? OP_SCENE_DELETE : OP_SCENE_DELETE_UNACK;
	BT_MESH_MODEL_BUF_DEFINE(buf, OP_SCENE_DELETE, 2);
	bt_mesh_model_msg_init(&buf, op);
	net_buf_simple_add_le16(&buf, scene_number);
	return bt_mesh_msg_send(model, &ctx, &buf);
}

uint16_t bt_mesh_scene_client_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("Error: BLE MESH profile is not initiated\r\n");
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_SCENE_CLIENT_MODEL_STORE: {
		rtk_bt_mesh_scene_store_t *store_value = (rtk_bt_mesh_scene_store_t *)p_cmd->param;
		ret = scene_store(scene_client_model, store_value->dst, store_value->app_key_index, store_value->scene_number, store_value->ack);
		break;
	}
	case RTK_BT_MESH_SCENE_CLIENT_MODEL_RECALL: {
		rtk_bt_mesh_scene_recall_t *recall_value = (rtk_bt_mesh_scene_recall_t *)p_cmd->param;
		ret = scene_recall(scene_client_model, recall_value->dst, recall_value->app_key_index, recall_value->scene_number, recall_value->tid,
						   recall_value->optional, recall_value->trans_time, recall_value->delay, recall_value->ack);
		break;
	}
	case RTK_BT_MESH_SCENE_CLIENT_MODEL_GET: {
		rtk_bt_mesh_scene_get_t *get_value = (rtk_bt_mesh_scene_get_t *)p_cmd->param;
		ret = scene_get(scene_client_model, get_value->dst, get_value->app_key_index);
		break;
	}
	case RTK_BT_MESH_SCENE_CLIENT_MODEL_REGISTER_GET: {
		rtk_bt_mesh_scene_register_get_t *register_get = (rtk_bt_mesh_scene_register_get_t *)p_cmd->param;
		ret = scene_register_get(scene_client_model, register_get->dst, register_get->app_key_index);
		break;
	}
	case RTK_BT_MESH_SCENE_CLIENT_MODEL_DELETE: {
		rtk_bt_mesh_scene_delete_t *delete_value = (rtk_bt_mesh_scene_delete_t *)p_cmd->param;
		ret = scene_delete(scene_client_model, delete_value->dst, delete_value->app_key_index, delete_value->scene_number, delete_value->ack);
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

#endif