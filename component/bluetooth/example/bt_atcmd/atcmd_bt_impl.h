/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __ATCMD_BT_IMPL_H__
#define __ATCMD_BT_IMPL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <basic_types.h>
#include <bt_debug.h>

typedef int (*cmd_func_t)(int argc, char *argv[]);

typedef struct {
	const char *cmd_name;
	cmd_func_t cmd_func;
	uint16_t argc_min;
	uint16_t argc_max;
} cmd_table_t;

typedef struct cmd_help_tbl {
	const char *name;
	const char *descriptor;
	struct cmd_help_tbl *sub_tbl;
} cmd_help_table_t;

int atcmd_bt_excute(int argc, char *argv[], const cmd_table_t *cmd_table, const char *tag);

/* bt example cmd implementation */
int atcmd_bt_demo(int argc, char *argv[]);

int atcmd_bt_audio_mp_test(int argc, char *argv[]);

int atcmd_bt_central(int argc, char *argv[]);

int atcmd_bt_peripheral(int argc, char *argv[]);

int atcmd_bt_scatternet(int argc, char *argv[]);

int atcmd_bt_throughput(int argc, char *argv[]);

int atcmd_bt_mesh_provisioner(int argc, char *argv[]);

int atcmd_bt_mesh_device(int argc, char *argv[]);

int atcmd_bt_mesh_provisioner_scatternet(int argc, char *argv[]);

int atcmd_bt_mesh_device_scatternet(int argc, char *argv[]);

int atcmd_bt_mesh_provisioner_test(int argc, char *argv[]);

int atcmd_bt_mesh_device_test(int argc, char *argv[]);

int atcmd_bt_a2dp(int argc, char *argv[]);

int atcmd_bt_a2dp_scatternet(int argc, char *argv[]);

int atcmd_bt_a2dp_provisioner_scatternet(int argc, char *argv[]);

int atcmd_bt_a2dp_pbp(int argc, char *argv[]);

int atcmd_bt_a2dp_tmap(int argc, char *argv[]);

int atcmd_bt_spp(int argc, char *argv[]);

int atcmd_bt_hid(int argc, char *argv[]);

int atcmd_bt_hfp(int argc, char *argv[]);

int atcmd_bt_a2dp_hfp(int argc, char *argv[]);

int atcmd_ble_iso(int argc, char *argv[]);

int atcmd_bt_bap(int argc, char *argv[]);

int atcmd_bt_cap(int argc, char *argv[]);

int atcmd_bt_pbp(int argc, char *argv[]);

int atcmd_bt_tmap(int argc, char *argv[]);

int atcmd_bt_gmap(int argc, char *argv[]);

int atcmd_bt_config(int argc, char *argv[]);

int atcmd_bt_pts(int argc, char *argv[]);

/* bt vendor cmd implementation */
int atcmd_bt_tx_power_gain(int argc, char *argv[]);

int atcmd_bt_hci_debug_enable(int argc, char *argv[]);

int atcmd_bt_sleep_mode(int argc, char *argv[]);

/*int atcmd_bt_ant(int argc, char *argv[]);*/

int atcmd_bt_set_tx_power(int argc, char *argv[]);

int atcmd_bt_sof_eof_ind(int argc, char *argv[]);

/* device cmd impl */
int atcmd_bt_device(int argc, char *argv[]);

/* le gap cmd impl */
int atcmd_bt_le_gap(int argc, char *argv[]);

/* mesh stack cmd impl */
int atcmd_bt_mesh_stack(int argc, char *argv[]);

/* mesh config client model cmd impl */
int atcmd_bt_mesh_config(int argc, char *argv[]);

/* mesh generic on off model cmd impl */
int atcmd_bt_mesh_generic_onoff(int argc, char *argv[]);

/* mesh datatrans model cmd impl */
int atcmd_bt_mesh_datatrans_model(int argc, char *argv[]);

/* mesh remote prov client model cmd impl */
int atcmd_bt_mesh_remote_prov_client_model(int argc, char *argv[]);

/* mesh light lightness model cmd impl */
int atcmd_bt_mesh_light_lightness(int argc, char *argv[]);

/* mesh light ctl model cmd impl */
int atcmd_bt_mesh_light_ctl(int argc, char *argv[]);

/* mesh light hsl model cmd impl */
int atcmd_bt_mesh_light_hsl(int argc, char *argv[]);

/* mesh light xyl model cmd impl */
int atcmd_bt_mesh_light_xyl(int argc, char *argv[]);

/* mesh light lc model cmd impl */
int atcmd_bt_mesh_light_lc(int argc, char *argv[]);

/* mesh time model cmd impl */
int atcmd_bt_mesh_time(int argc, char *argv[]);

/* mesh scheduler model cmd impl */
int atcmd_bt_mesh_scheduler(int argc, char *argv[]);

/* mesh scene model cmd impl */
int atcmd_bt_mesh_scene(int argc, char *argv[]);

/* mesh generic default transition time model cmd impl */
int atcmd_bt_mesh_generic_default_transition_time(int argc, char *argv[]);

/* mesh generic level model cmd impl */
int atcmd_bt_mesh_generic_level(int argc, char *argv[]);

/* mesh generic power on off model cmd impl */
int atcmd_bt_mesh_generic_power_on_off(int argc, char *argv[]);

/* mesh generic power level model cmd impl */
int atcmd_bt_mesh_generic_power_level(int argc, char *argv[]);

/* mesh generic battery model cmd impl */
int atcmd_bt_mesh_generic_battery(int argc, char *argv[]);

/* mesh generic location model cmd impl */
int atcmd_bt_mesh_generic_location(int argc, char *argv[]);

/* mesh generic property model cmd impl */
int atcmd_bt_mesh_generic_property(int argc, char *argv[]);

/* mesh sensor model cmd impl */
int atcmd_bt_mesh_sensor(int argc, char *argv[]);

/* mesh health model cmd impl */
int atcmd_bt_mesh_health(int argc, char *argv[]);

/* mesh performence test cmd impl */
int atcmd_bt_mesh_performence_test(int argc, char *argv[]);

/* br gap cmd impl */
int atcmd_bt_br_gap(int argc, char *argv[]);

/* common gap cmd impl */
int atcmd_bt_gap(int argc, char *argv[]);

/* gatts cmd impl */
int atcmd_bt_gatts(int argc, char *argv[]);

/* gattc cmd impl */
int atcmd_bt_gattc(int argc, char *argv[]);

/* sdp cmd impl */
int atcmd_bt_sdp_cmd(int argc, char *argv[]);

/* a2dp cmd impl */
int atcmd_bt_a2dp_cmd(int argc, char *argv[]);

/* avrcp cmd impl */
int atcmd_bt_avrcp_cmd(int argc, char *argv[]);

/* spp cmd impl */
int atcmd_bt_spp_cmd(int argc, char *argv[]);

/* hid cmd impl */
int atcmd_bt_hid_cmd(int argc, char *argv[]);

/* hfp cmd impl */
int atcmd_bt_hfp_cmd(int argc, char *argv[]);

/* pbap cmd impl */
int atcmd_bt_pbap_cmd(int argc, char *argv[]);

/* le audio bap */
int atcmd_bt_bap_cmd(int argc, char *argv[]);

/* le audio iso */
int atcmd_bt_iso_cmd(int argc, char *argv[]);

/* le audio cap */
int atcmd_bt_cap_cmd(int argc, char *argv[]);

/* le audio pbp */
int atcmd_bt_pbp_cmd(int argc, char *argv[]);

/* le audio tmap */
int atcmd_bt_tmap_cmd(int argc, char *argv[]);

/* le audio gmap */
int atcmd_bt_gmap_cmd(int argc, char *argv[]);

/* pts cmd impl */
int atcmd_bt_pts_cmd(int argc, char *argv[]);

/* transfer module demo impl */
int atcmd_bt_transfer_module(int argc, char *argv[]);

/* transfer module cmd impl */
int atcmd_bt_transfer_module_cmd(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif /* __ATCMD_BT_IMPL_H__ */