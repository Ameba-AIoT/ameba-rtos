/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef __ZEPHYR_STACK_INTERNAL_H__
#define __ZEPHYR_STACK_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

int bt_stack_le_gap_init(void *app_config);
uint16_t bt_stack_gattc_init(rtk_bt_app_conf_t *app_conf);
void bt_stack_gattc_deinit(void);
void bt_stack_gattc_disconnect_clear(uint16_t conn_handle);
uint16_t bt_stack_gatts_init(rtk_bt_app_conf_t *app_conf);
void bt_stack_gatts_deinit(void);
void bt_stack_gatts_disconnect_clear(struct bt_conn *conn);

uint16_t bt_stack_gap_act_handle(rtk_bt_cmd_t *p_cmd);
uint16_t bt_stack_le_gap_act_handle(rtk_bt_cmd_t *p_cmd);
uint16_t bt_stack_gatts_act_handle(rtk_bt_cmd_t *p_cmd);
uint16_t bt_stack_gattc_act_handle(rtk_bt_cmd_t *p_cmd);
// uint16_t bt_stack_br_gap_act_handle(rtk_bt_cmd_t *p_cmd);
// uint16_t bt_stack_a2dp_act_handle(rtk_bt_cmd_t *p_cmd);
// uint16_t bt_stack_avrcp_act_handle(rtk_bt_cmd_t *p_cmd);

uint16_t bt_mesh_generic_onoff_client_model_act_handle(rtk_bt_cmd_t *p_cmd);
const struct bt_mesh_comp *bt_mesh_get_composition_data(void);

bool bt_stack_profile_check(rtk_bt_profile_t profile);
bool is_stack_never_enabled(void);

int zephyr_res_alloc(void);
int zephyr_res_free(void);
void zephyr_internal_res_free(void);

#ifdef __cplusplus
}
#endif

#endif  /* __RTK_STACK_INTERNAL_H__ */