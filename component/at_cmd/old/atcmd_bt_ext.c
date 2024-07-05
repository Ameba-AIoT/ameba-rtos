/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <osif.h>
#include <os_wrapper.h>
#include <log_service.h>
#include <atcmd_bt_impl.h>
#include <atcmd_bt_vendor.h>
#include <bt_utils.h>
#include <bt_api_config.h>

#define BT_ATCMD_HELP   0  // decide whether open the usage of atcmd help

#if defined(BT_ATCMD_HELP) && BT_ATCMD_HELP
cmd_help_table_t le_gap_help_table[] = {{NULL,},};
cmd_help_table_t br_gap_help_table[] = {{NULL,},};
cmd_help_table_t gattc_help_table[] = {{NULL,},};
cmd_help_table_t gatts_help_table[] = {{NULL,},};
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
cmd_help_table_t mesh_stack_help_table[] = {{NULL,},};
cmd_help_table_t mesh_config_help_table[] = {{NULL,},};
cmd_help_table_t mesh_goo_help_table[] = {{NULL,},};
cmd_help_table_t mesh_data_help_table[] = {{NULL,},};
cmd_help_table_t mesh_ll_help_table[] = {{NULL,},};
cmd_help_table_t mesh_lctl_help_table[] = {{NULL,},};
cmd_help_table_t mesh_lhsl_help_table[] = {{NULL,},};
cmd_help_table_t mesh_lxyl_help_table[] = {{NULL,},};
cmd_help_table_t mesh_llc_help_table[] = {{NULL,},};
cmd_help_table_t mesh_time_help_table[] = {{NULL,},};
cmd_help_table_t mesh_scheduler_help_table[] = {{NULL,},};
cmd_help_table_t mesh_scene_help_table[] = {{NULL,},};
cmd_help_table_t mesh_gdtt_help_table[] = {{NULL,},};
cmd_help_table_t mesh_gle_help_table[] = {{NULL,},};
cmd_help_table_t mesh_gpoo_help_table[] = {{NULL,},};
cmd_help_table_t mesh_gpl_help_table[] = {{NULL,},};
cmd_help_table_t mesh_gb_help_table[] = {{NULL,},};
cmd_help_table_t mesh_glo_help_table[] = {{NULL,},};
cmd_help_table_t mesh_gp_help_table[] = {{NULL,},};
cmd_help_table_t mesh_sensor_help_table[] = {{NULL,},};
cmd_help_table_t mesh_health_help_table[] = {{NULL,},};
cmd_help_table_t mesh_rmt_help_table[] = {{NULL,},};
#endif
cmd_help_table_t a2dp_help_table[] = {{NULL,},};
cmd_help_table_t avrcp_help_table[] = {{NULL,},};
cmd_help_table_t spp_help_table[] = {{NULL,},};

cmd_help_table_t cmd_help_table[] = {
	{
		"bt",      "[ATBC] bt: BT basic operation like enable/disable\n\r"
		"usage: ATBC=bt,[en]\n\r"
		"[en] = <0-(disable), 1-(enable)>",
		NULL
	},
	{
		"le_gap",    "[ATBC] le_gap: BT LE GAP layer operation\n\r"
		"usage: ATBC=le_gap,[sub_cmd],..\n\r"
		"[sub_cmd] = <reg_cb, addr, name, appear, rand_addr, def_datalen, def_phy, \n\r"
		"             def_mtu, channel, adv_data, adv, scan_rsp, scan_param, scan, \n\r"
		"             conn, conn_rssi, conn_info, conn_update, conn_phy, conn_datalen, disconn, \n\r"
		"             privacy, privacy_mode, wl_size, wl_add, wl_remove, wl_clear, \n\r"
		"             sec_param, sec, pair_cfm, auth_key, auth_keycfm, auth_oob, \n\r"
		"             bond_num, bond_info, bond_rem, bond_clear>",
		le_gap_help_table
	},
	{
		"br_gap",    "[ATBC] br_gap: BT BR GAP layer operation\n\r"
		"usage: ATBC=br_gap,[sub_cmd],..\n\r"
		"[sub_cmd] = <inquiry_start, bond_clear>",
		br_gap_help_table
	},
	{
		"gattc", "[ATBC] gattc: GATT operation as client\n\r"
		"usage: ATBC=gattc,[sub_cmd],...\n\r"
		"[subcmd] = <reg, subs, disc, read, write, cfm>",
		gattc_help_table
	},
	{
		"gatts",   "[ATBC] gatts: GATT operation as server\n\r"
		"usage: ATBC=gatts,[sub_cmd],...\n\r"
		"[subcmd] = <reg_cb, reg_srv, ind, ntf, r_rsp, w_rsp>",
		gatts_help_table
	},
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	{
		"mesh_stack",   "[ATBC] mesh_stack: BLE mesh stack operation\n\r"
		"usage: ATBC=mesh_stack,[sub_cmd],...\n\r"
		"[subcmd] = <reset, dev_info, scan, list, fn_init, fn_deinit, pbadvcon, prov, prov_list, prov_notify, proxy_dis, \r\n"
		"			proxy_notify, lpn_init, lpn_req, lpn_sub, lpn_clear, lpn_deinit>",
		mesh_stack_help_table
	},
	{
		"mesh_config",   "[ATBC] mesh_config: BLE mesh config model operation\n\r"
		"usage: ATBC=mesh_config,[sub_cmd],...\n\r"
		"[subcmd] = <aka, mab, msb, ttl_get, ttl_set, relay_get, relay_set, net_trans_set, net_trans_get, reset, \n\r"
		"			 beacon_get, beacon_set, compo_data_get, proxy_get, proxy_set, pub_get, pub_set, msd, msda, \n\r"
		"			 mso, msg, nka, nku, nkd, nkg, aku, akd, akg, node_identity_get, node_identity_set, maub, mag, \r\n"
		"			 frg, frs, lptg, krpg, krps, hpg, hps, hsg, hss>",
		mesh_config_help_table
	},
	{
		"mesh_goo",   "[ATBC] mesh_goo: BLE mesh generic on off model operation\n\r"
		"usage: ATBC=mesh_goo,[sub_cmd],...\n\r"
		"[subcmd] = <set, get>",
		mesh_goo_help_table
	},
	{
		"mesh_data",   "[ATBC] mesh_data: BLE mesh datatrans model operation\n\r"
		"usage: ATBC=mesh_data,[sub_cmd],...\n\r"
		"[subcmd] = <write, read>",
		mesh_data_help_table
	},
	{
		"mesh_ll",   "[ATBC] mesh_ll: BLE mesh light lightness model operation\n\r"
		"usage: ATBC=mesh_ll,[sub_cmd],...\n\r"
		"[subcmd] = <llg, lls, lllg, llls, lllag, lldg, llds, llrg, llrs>",
		mesh_ll_help_table
	},
	{
		"mesh_lctl",   "[ATBC] mesh_lctl: BLE mesh light ctl model operation\n\r"
		"usage: ATBC=mesh_lctl,[sub_cmd],...\n\r"
		"[subcmd] = <lcg, lcs, lctg, lcts, lctrg, lctrs, lcdg, lcds>",
		mesh_lctl_help_table
	},
	{
		"mesh_lhsl",   "[ATBC] mesh_lhsl: BLE mesh light hsl model operation\n\r"
		"usage: ATBC=mesh_lhsl,[sub_cmd],...\n\r"
		"[subcmd] = <lhg, lhs, lhtg, lhhg, lhhs, lhsg, lhss, lhdg, lhds, lhrg, lhrs>",
		mesh_lhsl_help_table
	},
	{
		"mesh_lxyl",   "[ATBC] mesh_lxyl: BLE mesh light xyl model operation\n\r"
		"usage: ATBC=mesh_lxyl,[sub_cmd],...\n\r"
		"[subcmd] = <lxg, lxs, lxtg, lxdg, lxds, lxrg, lxrs>",
		mesh_lxyl_help_table
	},
	{
		"mesh_llc",   "[ATBC] mesh_llc: BLE mesh light lc model operation\n\r"
		"usage: ATBC=mesh_llc,[sub_cmd],...\n\r"
		"[subcmd] = <llcmg, llcms, llcomg, llcoms, llcloog, llcloos, llcpg, llcps>",
		mesh_llc_help_table
	},
	{
		"mesh_time",   "[ATBC] mesh_time: BLE mesh time model operation\n\r"
		"usage: ATBC=mesh_time,[sub_cmd],...\n\r"
		"[subcmd] = <tg, tzg, ttudg, trg, ts, tzs, ttuds, trs>",
		mesh_time_help_table
	},
	{
		"mesh_scheduler",   "[ATBC] mesh_scheduler: BLE mesh scheduler model operation\n\r"
		"usage: ATBC=mesh_scheduler,[sub_cmd],...\n\r"
		"[subcmd] = <scheg, scheag, scheas>",
		mesh_scheduler_help_table
	},
	{
		"mesh_scene",   "[ATBC] mesh_scene: BLE mesh scene model operation\n\r"
		"usage: ATBC=mesh_scene,[sub_cmd],...\n\r"
		"[subcmd] = <sg, ss, sr, srg, sd>",
		mesh_scene_help_table
	},
	{
		"mesh_gdtt",   "[ATBC] mesh_gdtt: BLE mesh generic default transition time model operation\n\r"
		"usage: ATBC=mesh_gdtt,[sub_cmd],...\n\r"
		"[subcmd] = <gdtts, gdttg>",
		mesh_gdtt_help_table
	},
	{
		"mesh_gle",   "[ATBC] mesh_gle: BLE mesh generic level model operation\n\r"
		"usage: ATBC=mesh_gle,[sub_cmd],...\n\r"
		"[subcmd] = <glg, gls, glds, glms>",
		mesh_gle_help_table
	},
	{
		"mesh_gpoo",   "[ATBC] mesh_gpoo: BLE mesh generic power on off model operation\n\r"
		"usage: ATBC=mesh_gpoo,[sub_cmd],...\n\r"
		"[subcmd] = <gpg, gps>",
		mesh_gpoo_help_table
	},
	{
		"mesh_gpl",   "[ATBC] mesh_gpl: BLE mesh generic power level model operation\n\r"
		"usage: ATBC=mesh_gpl,[sub_cmd],...\n\r"
		"[subcmd] = <gplg, gpls, gpllg, gpldg, gplds, gplrg, gplrs>",
		mesh_gpl_help_table
	},
	{
		"mesh_gb",   "[ATBC] mesh_gb: BLE mesh generic battery model operation\n\r"
		"usage: ATBC=mesh_gb,[sub_cmd],...\n\r"
		"[subcmd] = <gbg>",
		mesh_gb_help_table
	},
	{
		"mesh_glo",   "[ATBC] mesh_glo: BLE mesh generic location model operation\n\r"
		"usage: ATBC=mesh_glo,[sub_cmd],...\n\r"
		"[subcmd] = <glgg, glgs, gllg, glls>",
		mesh_glo_help_table
	},
	{
		"mesh_gp",   "[ATBC] mesh_glo: BLE mesh generic property model operation\n\r"
		"usage: ATBC=mesh_gp,[sub_cmd],...\n\r"
		"[subcmd] = <gupsg, gupg, gups, gapsg, gapg, gaps, gmpsg, gmpg, gmps, gcpsg>",
		mesh_gp_help_table
	},
	{
		"mesh_sensor",   "[ATBC] mesh_sensor: BLE mesh sensor model operation\n\r"
		"usage: ATBC=mesh_sensor,[sub_cmd],...\n\r"
		"[subcmd] = <sdg, scg, scs, sssg, ssg, sss, sg, scog, sseg>",
		mesh_sensor_help_table
	},
	{
		"mesh_health",   "[ATBC] mesh_health: BLE mesh health model operation\n\r"
		"usage: ATBC=mesh_health,[sub_cmd],...\n\r"
		"[subcmd] = <hfg, hfc, hft, hpg, hps, hag, has>",
		mesh_health_help_table
	},
	{
		"mesh_rmt",   "[ATBC] mesh_rmt: BLE mesh remote provisioning model operation\n\r"
		"usage: ATBC=mesh_rmt,[sub_cmd],...\n\r"
		"[subcmd] = <scan_start, scan_cap_get, link_open>",
		mesh_rmt_help_table
	},
#endif
	{
		"a2dp_cmd",    "[ATBC] a2dp_cmd: BT A2DP operation\n\r"
		"usage: ATBC=a2dp_cmd,[sub_cmd],..\n\r"
		"[sub_cmd] = <conn, disconn, start, suspend>",
		a2dp_help_table
	},
	{
		"avrcp_cmd",    "[ATBC] avrcp_cmd: BT AVRCP operation\n\r"
		"usage: ATBC=avrcp_cmd,[sub_cmd],..\n\r"
		"[sub_cmd] = <conn, disconn, start, suspend>",
		avrcp_help_table
	},
	{
		"spp_cmd",    "[ATBC] spp_cmd: BT SPP operation\n\r"
		"usage: ATBC=spp_cmd,[sub_cmd],..\n\r"
		"[sub_cmd] = <conn, disconn, disconn_all, send_data, give_credits>",
		spp_help_table
	},
	{NULL,},
};

cmd_help_table_t example_help_table[] = {
	{
		"central",     "[ATBE] central: run as a BT central\n\r"
		"usage: ATBE=central,[val]\n\r"
		"[val] = <0-(disable), 1-(enable), 2-(enable with predefined routine)>",
		NULL
	},
	{
		"peripheral", "[ATBE] peripheral: run as a BT peripheral\n\r"
		"usage: ATBE=peripheral,[val]\n\r"
		"[val] = <0-(disable), 1-(enable), 2-(enable with predefined routine)>",
		NULL
	},
	{
		"scatternet", "[ATBE] central: run as a BT central\n\r"
		"usage: ATBE=central,[val]\n\r"
		"[val] = <0-(disable), 1-(enable), 2-(enable with predefined routine)>",
		NULL
	},
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	{
		"provisioner", "[ATBE] provisioner: run as a BT mesh provisioner\n\r"
		"usage: ATBE=provisioner,[val]\n\r"
		"[val] = <0-(disable), 1-(enable) >",
		NULL
	},
	{
		"device", "[ATBE] device: run as a BT mesh device\n\r"
		"usage: ATBE=device,[val]\n\r"
		"[val] = <0-(disable), 1-(enable) >",
		NULL
	},
	{
		"provisioner_scatternet", "[ATBE] provisioner_scatteret: run as a BT mesh provisioner scatternet\n\r"
		"usage: ATBE=provisioner_scatternet,[val]\n\r"
		"[val] = <0-(disable), 1-(enable) >",
		NULL
	},
	{
		"device_scatternet", "[ATBE] device_scatternet: run as a BT mesh device scatternet\n\r"
		"usage: ATBE=device_scatternet,[val]\n\r"
		"[val] = <0-(disable), 1-(enable) >",
		NULL
	},
#endif
	{
		"a2dp", "[ATBE] a2dp: run as a A2DP sink(snk))/source(src)\n\r"
		"usage: ATBE=a2dp,[role][val]\n\r"
		"[role] = <snk-(sink), src-(source)>"
		"[val] = <0-(disable), 1-(enable)>",
		NULL
	},
	{
		"spp", "[ATBE] spp: run as a SPP client(client))/server(server)\n\r"
		"usage: ATBE=spp,[role][val]\n\r"
		"[role] = <client-(client), server-(server)>"
		"[val] = <0-(disable), 1-(enable)>",
		NULL
	},
	{
		"hfp", "[ATBE] hfp: run as a HFP hf(hand free))/ag(audio gate)\n\r"
		"usage: ATBE=hfp,[role][val]\n\r"
		"[role] = <hf-(hand free), ag-(audio gate)>"
		"[val] = <0-(disable), 1-(enable)>",
		NULL
	},
	{
		"throughput", "[ATBE] throughput: run as a Throughput scatternet\n\r"
		"usage: ATBE=throughput,[act],[val]\n\r"
		"[act] = <1, 0, conn, test_start, test_stop>\n\r"
		"[val] = DEPENDS ON DIFFERENT ACTS",
		NULL
	},
	{NULL,},
};

cmd_help_table_t vendor_help_table[] = {
	{
		"tx_power_gain",     "[ATBV] tx_power_gain: manual set bt tx power gain index\n\r"
		"usage: ATBV=tx_power_gain,[val]\n\r"
		"[val] = <0~0xFF>",
		NULL
	},
	{
		"hci_debug_enable",     "[ATBV] hci_debug_enable: enable hci uart to external pin\n\r"
		"usage: ATBV=hci_debug_enable\n\r",
		NULL
	},
	{
		"dlps",     "[ATBV] dlps: disable or enable BT fw dlps\n\r"
		"usage: ATBV=dlps,0\n\r",
		NULL
	},
	{
		"sof",     "[ATBV] sof: disable or enable BT fw sof and eof indication\n\r"
		"usage: ATBV=sof,[conn_handle],[val]\n\r"
		"[conn_handle] = <0-MAX>-(decimal or hexnum)\n\r"
		"[val] = <0-(disable), 1-(enable)>",
		NULL
	},
	{NULL,},
};

static void atcmd_bt_help_common(int argc, char *argv[],
								 const char *tag, const char *help_usage,
								 cmd_help_table_t *cmd_help_table)
{
	int i = 0, j = 0;

	if (0 == argc) {
		AT_PRINTK("%s", help_usage);
		return;
	}

	char *cmd_str = argv[0];
	while (cmd_help_table[i].name) {
		if (0 == strcmp(cmd_str, cmd_help_table[i].name)) {
			if (argc == 1) {
				AT_PRINTK("%s", cmd_help_table[i].discriptor);
			}
			goto SUB_CMD_HELP;
		}
		i++;
	}

	AT_PRINTK("[%s] Error: Cant find this cmd help\r\n", tag);
	return;

SUB_CMD_HELP:
	if (argc >= 2) {
		char *sub_cmd_str = argv[1];
		cmd_help_table_t *sub_cmd_tbl = cmd_help_table[i].sub_tbl;
		while (sub_cmd_tbl[j].name) {
			if (0 == strcmp(sub_cmd_str, sub_cmd_tbl[j].name)) {
				AT_PRINTK("%s", sub_cmd_tbl[j].discriptor);
				break;
			}
			j++;
		}

		if (!sub_cmd_tbl[j].name)
			AT_PRINTK("[%s] Error: Cant find this subcmd help in cmd: %s\r\n",
					  tag, cmd_help_table[i].name);
	}
}

static int atcmd_bt_cmd_help(int argc, char *argv[])
{
	const char *help_usage =  "[ATBC] help: show ATBC cmds usage and discription\n\r"
							  "usage: ATBC=help,[cmd]\n\r"
							  "       ATBC=help,[cmd],[subcmd]\n\r"
							  "[cmd] = <bt, le_gap, br_gap, gattc, gatts, mesh_stack, mesh_data, mesh_config, mesh_goo, mesh_rmt\n\r"
							  "			mesh_ll, mesh_lctl, mesh_lhsl, mesh_lxyl, mesh_llc, a2dp, avrcp, spp_cmd>\n\r"
							  "[subcmd] = 'use ATBC=help,[cmd] to show subcmds'";
	atcmd_bt_help_common(argc, argv, "ATBC", help_usage, cmd_help_table);
	return 0;
}

static int atcmd_bt_example_help(int argc, char *argv[])
{
	const char *help_usage =  "[ATBE] help: show ATBE cmds usage and discription\n\r"
							  "usage: ATBE=help,[cmd]\n\r"
							  "       ATBE=help,[cmd],[subcmd]\n\r"
							  "[cmd] = <central, peripheral, scatternet, provisioner, device, provisioner_scatternet, device_scatternet, a2dp, spp, hfp>\n\r"
							  "[subcmd] = 'use ATBE=help,[cmd] to show subcmds'";
	atcmd_bt_help_common(argc, argv, "ATBE", help_usage, example_help_table);
	return 0;
}

static int atcmd_bt_vendor_help(int argc, char *argv[])
{
	const char *help_usage =  "[ATBV] help: show ATBV cmds usage and discription\n\r"
							  "usage: ATBV=help,[cmd]\n\r"
							  "       ATBV=help,[cmd],[subcmd]\n\r"
							  "[cmd] = <tx_power_gain, hci_debug_enable, dlps>\n\r"
							  "[subcmd] = 'use ATBV=help,[cmd] to show subcmds'";
	atcmd_bt_help_common(argc, argv, "ATBV", help_usage, vendor_help_table);
	return 0;
}
#endif  /* BT_ATCMD_HELP */

static const cmd_table_t cmd_table[] = {
#if defined(BT_ATCMD_HELP) && BT_ATCMD_HELP
	{"help",        atcmd_bt_cmd_help,             1, 3},
#endif
	{"bt",          atcmd_bt_device,               2, 2},
	{"le_gap",      atcmd_bt_le_gap,               2, 21},
#if defined(RTK_BLE_GATTS) && RTK_BLE_GATTS
	{"gatts",       atcmd_bt_gatts,                3, 16},
#endif
#if defined(RTK_BLE_GATTC) && RTK_BLE_GATTC
	{"gattc",       atcmd_bt_gattc,                3, 14},
#endif
	{"gap",         atcmd_bt_gap,                  1, 13},
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	{"mesh_stack",  atcmd_bt_mesh_stack,           2, 8},
	{"mesh_data",   atcmd_bt_mesh_datatrans_model, 5, 6},
	{"mesh_config", atcmd_bt_mesh_config,          3, 13},
	{"mesh_goo",    atcmd_bt_mesh_generic_onoff,   4, 9},
	{"mesh_rmt",    atcmd_bt_mesh_remote_prov_client_model, 4, 7},
	{"mesh_ll",     atcmd_bt_mesh_light_lightness, 4, 9},
	{"mesh_lctl",   atcmd_bt_mesh_light_ctl,       4, 11},
	{"mesh_lhsl",   atcmd_bt_mesh_light_hsl,       4, 11},
	{"mesh_lxyl",   atcmd_bt_mesh_light_xyl,       4, 11},
	{"mesh_llc",    atcmd_bt_mesh_light_lc,        4, 9},
	{"mesh_time",   atcmd_bt_mesh_time,            4, 10},
	{"mesh_scheduler",   atcmd_bt_mesh_scheduler,  4, 17},
	{"mesh_scene",  atcmd_bt_mesh_scene,           4, 9},
	{"mesh_gdtt",   atcmd_bt_mesh_generic_default_transition_time,  4, 7},
	{"mesh_gle",    atcmd_bt_mesh_generic_level,   4, 9},
	{"mesh_gpoo",   atcmd_bt_mesh_generic_power_on_off,   4, 6},
	{"mesh_gpl",    atcmd_bt_mesh_generic_power_level,   4, 9},
	{"mesh_gb",    atcmd_bt_mesh_generic_battery,   4, 4},
	{"mesh_glo",    atcmd_bt_mesh_generic_location,   4, 13},
	{"mesh_gp",    atcmd_bt_mesh_generic_property,   4, 8},
	{"mesh_sensor",    atcmd_bt_mesh_sensor,   4, 14},
	{"mesh_health",    atcmd_bt_mesh_health,   4, 7},
#endif  // end of RTK_BLE_MESH_SUPPORT
#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
	{"br_gap",      atcmd_bt_br_gap,               2, 13},
	{"a2dp_cmd",    atcmd_bt_a2dp_cmd,             1, 8},
	{"avrcp_cmd",   atcmd_bt_avrcp_cmd,            1, 4},
	{"spp_cmd",     atcmd_bt_spp_cmd,              1, 8},
	{"hid_cmd",     atcmd_bt_hid_cmd,              1, 23},
	{"hfp_cmd",     atcmd_bt_hfp_cmd,              1, 8},
	{"pbap_cmd",    atcmd_bt_pbap_cmd,             1, 8},
#endif
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
	{"bap_cmd",     atcmd_bt_bap_cmd,              3, 10},
#if defined(CONFIG_BT_CAP_SUPPORT) && CONFIG_BT_CAP_SUPPORT
	{"cap_cmd",     atcmd_bt_cap_cmd,              3, 10},
#endif
#if defined(CONFIG_BT_PBP_SUPPORT) && CONFIG_BT_PBP_SUPPORT
	{"pbp_cmd",     atcmd_bt_pbp_cmd,              3, 10},
#endif
#if (defined(CONFIG_BT_TMAP_SUPPORT) && CONFIG_BT_TMAP_SUPPORT)
	{"tmap_cmd",    atcmd_bt_tmap_cmd,             3, 10},
#endif
#if defined(CONFIG_BT_GMAP_SUPPORT) && CONFIG_BT_GMAP_SUPPORT
	{"gmap_cmd",    atcmd_bt_gmap_cmd,             3, 10},
#endif
#endif
	{NULL,},
};

static const cmd_table_t example_table[] = {
#if defined(BT_ATCMD_HELP) && BT_ATCMD_HELP
	{"help",             atcmd_bt_example_help,     1, 3},
#endif
#if defined(CONFIG_BT_AUDIO_MP_TEST) && CONFIG_BT_AUDIO_MP_TEST
	{"bt_audio_mp_test", atcmd_bt_audio_mp_test,    2, 2},
#endif
#if defined(CONFIG_BT_PERIPHERAL) && CONFIG_BT_PERIPHERAL
	{"peripheral",       atcmd_bt_peripheral,       2, 2},
#endif
#if defined(CONFIG_BT_CENTRAL) && CONFIG_BT_CENTRAL
	{"central",          atcmd_bt_central,          2, 2},
#endif
#if defined(CONFIG_BT_SCATTERNET) && CONFIG_BT_SCATTERNET
	{"scatternet",       atcmd_bt_scatternet,       2, 2},
#endif
#if defined(CONFIG_BT_THROUGHPUT) && CONFIG_BT_THROUGHPUT
	{"throughput",       atcmd_bt_throughput,       2, 13},
#endif
#if defined(CONFIG_BT_MESH_PROVISIONER) && CONFIG_BT_MESH_PROVISIONER
	{"provisioner",      atcmd_bt_mesh_provisioner, 2, 2},
#endif
#if defined(CONFIG_BT_MESH_DEVICE) && CONFIG_BT_MESH_DEVICE
	{"device",           atcmd_bt_mesh_device,      2, 2},
#endif
#if defined(CONFIG_BT_MESH_PROVISIONER_SCATTERNET) && CONFIG_BT_MESH_PROVISIONER_SCATTERNET
	{"provisioner_scatternet", atcmd_bt_mesh_provisioner_scatternet, 2, 2},
#endif
#if defined(CONFIG_BT_MESH_DEVICE_SCATTERNET) && CONFIG_BT_MESH_DEVICE_SCATTERNET
	{"device_scatternet", atcmd_bt_mesh_device_scatternet, 2, 2},
#endif
#if defined(CONFIG_BT_MESH_PROVISIONER_TEST) && CONFIG_BT_MESH_PROVISIONER_TEST
	{"provisioner_test", atcmd_bt_mesh_provisioner_test, 2, 9},
#endif
#if defined(CONFIG_BT_MESH_DEVICE_TEST) && CONFIG_BT_MESH_DEVICE_TEST
	{"device_test", atcmd_bt_mesh_device_test, 2, 2},
#endif
#if defined(CONFIG_BT_ISO_TEST) && CONFIG_BT_ISO_TEST
	{"iso",              atcmd_ble_iso,             3, 8},
#endif
#if defined(CONFIG_BT_A2DP) && CONFIG_BT_A2DP
	{"a2dp",             atcmd_bt_a2dp,             3, 3},
#endif
#if defined(CONFIG_BT_A2DP_SCATTERNET) && CONFIG_BT_A2DP_SCATTERNET
	{"a2dp_scatternet",  atcmd_bt_a2dp_scatternet,  3, 3},
#endif
#if defined(CONFIG_BT_A2DP_PROVISIONER_SCATTERNET) && CONFIG_BT_A2DP_PROVISIONER_SCATTERNET
	{"a2dp_provisioner_scatternet",  atcmd_bt_a2dp_provisioner_scatternet,  3, 3},
#endif
#if defined(CONFIG_BT_A2DP_LE_AUDIO_PBP) && CONFIG_BT_A2DP_LE_AUDIO_PBP
	{"a2dp_pbp",         atcmd_bt_a2dp_pbp,         2, 2},
#endif
#if defined(CONFIG_BT_A2DP_LE_AUDIO_TMAP) && CONFIG_BT_A2DP_LE_AUDIO_TMAP
	{"a2dp_tmap",        atcmd_bt_a2dp_tmap,        3, 3},
#endif
#if defined(CONFIG_BT_SPP) && CONFIG_BT_SPP
	{"spp",              atcmd_bt_spp,              3, 6},
#endif
#if defined(CONFIG_BT_HID) && CONFIG_BT_HID
	{"hid",              atcmd_bt_hid,              2, 3},
#endif
#if defined(CONFIG_BT_HFP) && CONFIG_BT_HFP
	{"hfp",              atcmd_bt_hfp,              3, 3},
#endif
#if defined(CONFIG_BT_A2DP_HFP) && CONFIG_BT_A2DP_HFP
	{"a2dp_hfp",         atcmd_bt_a2dp_hfp,         3, 3},
#endif
#if defined(CONFIG_BT_BAP_SUPPORT) && CONFIG_BT_BAP_SUPPORT
	{"bap",              atcmd_bt_bap,              4, 4},
#endif
#if defined(CONFIG_BT_CAP_SUPPORT) && CONFIG_BT_CAP_SUPPORT
	{"cap",              atcmd_bt_cap,              3, 3},
#endif
#if defined(CONFIG_BT_PBP_SUPPORT) && CONFIG_BT_PBP_SUPPORT
	{"pbp",              atcmd_bt_pbp,              3, 3},
#endif
#if defined(CONFIG_BT_TMAP_SUPPORT) && CONFIG_BT_TMAP_SUPPORT
	{"tmap",             atcmd_bt_tmap,             3, 3},
#endif
#if defined(CONFIG_BT_GMAP_SUPPORT) && CONFIG_BT_GMAP_SUPPORT
	{"gmap",             atcmd_bt_gmap,             3, 3},
#endif
#if defined(CONFIG_BT_PTS) && CONFIG_BT_PTS
	{"pts",              atcmd_bt_pts,              2, 4},
#endif
	// {"bt_config",        atcmd_bt_config,           2, 2},
	// {"demo",             atcmd_bt_demo,             1, 1},
	{NULL,},
};

static const cmd_table_t vendor_table[] = {
#if defined(BT_ATCMD_HELP) && BT_ATCMD_HELP
	{"help",             atcmd_bt_vendor_help,      1, 3},
#endif
	{"tx_power_gain",    atcmd_bt_tx_power_gain,    2, 5},
	{"hci_debug_enable", atcmd_bt_hci_debug_enable, 1, 1},
	{"sleep",            atcmd_bt_sleep_mode,       2, 2},
	/*{"ant",              atcmd_bt_ant,              2, 2},*/
	{"tx_power",         atcmd_bt_set_tx_power,     4, 5},
	{"sof",              atcmd_bt_sof_eof_ind,      2, 3},
	{NULL,},
};

static inline void atcmd_bt_cmd(int argc, char *argv[])
{
	atcmd_bt_excute(argc, argv, cmd_table, "[ATBC]");
}

static inline void atcmd_bt_example(int argc, char *argv[])
{
	atcmd_bt_excute(argc, argv, example_table, "[ATBE]");
}

static inline void atcmd_bt_vendor(int argc, char *argv[])
{
	atcmd_bt_excute(argc, argv, vendor_table, "[ATBV]");
}

static void fATBC(void *arg)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};

	if (!arg) {
		AT_PRINTK("[ATBC] Error: No input args number!");
		goto exit;
	}

	argc = parse_param(arg, argv);
	if (argc < 2) {
		AT_PRINTK("[ATBC] Error: Wrong input args number!");
		goto exit;
	}

	atcmd_bt_cmd(argc - 1, &argv[1]);
	return;

exit:
	AT_PRINTK("[ATBC] Info: Use 'ATBC=help' to help");
}

static void fATBE(void *arg)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};

	if (!arg) {
		AT_PRINTK("[ATBE] Error: No input args number!");
		goto exit;
	}

	argc = parse_param(arg, argv);
	if (argc < 2) {
		AT_PRINTK("[ATBE] Error: Wrong input args number!");
		goto exit;
	}

	atcmd_bt_example(argc - 1, &argv[1]);
	return;

exit:
	AT_PRINTK("[ATBE] Info: Use 'ATBE=help' to help");
}

_WEAK int rtk_bt_verify(int param_num, int *param)
{
	(void)param_num;
	(void)param;

	AT_PRINTK("[ATBT] Error: rtk_bt_verify is not compiled");
	return 0;
}

_WEAK int rtk_bt_get_verify_cmd_index(char *cmd_str)
{
	(void)cmd_str;

	AT_PRINTK("[ATBT] Error: rtk_bt_get_verify_cmd_index is not compiled");
	return 0;
}

static void fATBT(void *arg)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	argc = parse_param(arg, argv);
	int param[20] = {0};
	int param_num = argc - 1;

	char *cmd_str = argv[1];
	param[0] = rtk_bt_get_verify_cmd_index(cmd_str);
	if (0xFFFF == param[0]) {
		return;
	}

	AT_PRINTK("[ATBT] %s ", cmd_str);
	for (int i = 1; i < param_num; i++) {
		param[i] = str_to_int(argv[i + 1]);
		AT_PRINTK("%d ", param[i]);
	}
	AT_PRINTK("\r\n");
	rtk_bt_verify(param_num, param);
}

static void fATBV(void *arg)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};

	if (!arg) {
		AT_PRINTK("[ATBV] Error: No input args number!");
		goto exit;
	}

	argc = parse_param(arg, argv);
	if (argc < 2) {
		AT_PRINTK("[ATBV] Error: Wrong input args number!");
		goto exit;
	}

	atcmd_bt_vendor(argc - 1, &argv[1]);
	return;

exit:
	AT_PRINTK("[ATBV] Info: Use 'ATBV=help' to help");
}

static log_item_t at_bt_items[] = {
	{"ATBC", fATBC, {NULL, NULL}},
	{"ATBE", fATBE, {NULL, NULL}},
	{"ATBV", fATBV, {NULL, NULL}},
	{"ATBT", fATBT, {NULL, NULL}},
};

void at_bt_init(void)
{
#if ((defined(CONFIG_MP_INCLUDED) && CONFIG_MP_INCLUDED) && (defined(CONFIG_MP_SHRINK) && CONFIG_MP_SHRINK)) || \
	((!defined(CONFIG_MP_INCLUDED) || !CONFIG_MP_INCLUDED) && (defined(CONFIG_BT_EXCLUDE_AT_COMMAND) && CONFIG_BT_EXCLUDE_AT_COMMAND))
	(void)at_bt_items;
#else
	log_service_add_table(at_bt_items, sizeof(at_bt_items) / sizeof(at_bt_items[0]));
#endif
}
