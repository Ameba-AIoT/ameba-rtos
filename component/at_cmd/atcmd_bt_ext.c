/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "atcmd_service.h"
#include "atcmd_bt_impl.h"
#include "bt_utils.h"
#include "bt_api_config.h"

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
cmd_help_table_t mesh_df_help_table[] = {{NULL,},};
cmd_help_table_t mesh_sbr_help_table[] = {{NULL,},};
cmd_help_table_t mesh_prb_help_table[] = {{NULL,},};
#endif
cmd_help_table_t a2dp_help_table[] = {{NULL,},};
cmd_help_table_t avrcp_help_table[] = {{NULL,},};
cmd_help_table_t spp_help_table[] = {{NULL,},};
cmd_help_table_t rfc_help_table[] = {{NULL,},};

cmd_help_table_t cmd_help_table[] = {
	{
		"bt",      "[AT+BTCMDHELP] bt: BT basic operation like enable/disable\n\r"
		"usage: AT+BTENABLE=[en]\n\r"
		"[en] = <0-(disable), 1-(enable)>",
		NULL
	},
	{
		"ble_gap",    "[AT+BTCMDHELP] le_gap: BT LE GAP layer operation\n\r"
		"usage: AT+BLEGAP=[sub_cmd],..\n\r"
		"[sub_cmd] = <reg_cb, addr, name, appear, rand_addr, def_datalen, def_phy, \n\r"
		"             def_mtu, channel, adv_data, adv, scan_rsp, scan_param, scan, \n\r"
		"             conn, conn_rssi, conn_info, conn_update, conn_phy, conn_datalen, disconn, \n\r"
		"             privacy, privacy_mode, wl_size, wl_add, wl_remove, wl_clear, \n\r"
		"             sec_param, sec, pair_cfm, auth_key, auth_keycfm, auth_oob, \n\r"
		"             bond_num, bond_info, bond_rem, bond_clear>",
		le_gap_help_table
	},
#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
	{
		"br_gap",    "[AT+BTCMDHELP] br_gap: BT BR GAP layer operation\n\r"
		"usage: AT+BRGAP=[sub_cmd],..\n\r"
		"[sub_cmd] = <inquiry_start, bond_clear>",
		br_gap_help_table
	},
#endif /* RTK_BREDR_SUPPORT */
#if defined(RTK_BLE_GATTC) && RTK_BLE_GATTC
	{
		"gattc", "[AT+BTCMDHELP] gattc: GATT operation as client\n\r"
		"usage: AT+BLEGATTC=[sub_cmd],...\n\r"
		"[subcmd] = <disc, read, write, cfm>",
		gattc_help_table
	},
#endif
#if defined(RTK_BLE_GATTS) && RTK_BLE_GATTS
	{
		"gatts",   "[AT+BTCMDHELP] gatts: GATT operation as server\n\r"
		"usage: AT+BLEGATTS=[sub_cmd],...\n\r"
		"[subcmd] = <ind, ntf>",
		gatts_help_table
	},
#endif
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	{
		"mesh_stack",   "[AT+BTCMDHELP] mesh_stack: BLE mesh stack operation\n\r"
		"usage: AT+BLEMESHSTACK=[sub_cmd],...\n\r"
		"[subcmd] = <reset, dev_info, scan, list, fn_init, fn_deinit, pbadvcon, prov, prov_list, prov_notify, proxy_dis, \r\n"
		"			proxy_notify, lpn_init, lpn_req, lpn_sub, lpn_clear, lpn_deinit>",
		mesh_stack_help_table
	},
	{
		"mesh_config",   "[AT+BTCMDHELP] mesh_config: BLE mesh config model operation\n\r"
		"usage: AT+BLEMESHCONFIG=[sub_cmd],...\n\r"
		"[subcmd] = <aka, mab, msb, ttl_get, ttl_set, relay_get, relay_set, net_trans_set, net_trans_get, reset, \n\r"
		"			 beacon_get, beacon_set, compo_data_get, proxy_get, proxy_set, pub_get, pub_set, msd, msda, \n\r"
		"			 mso, msg, nka, nku, nkd, nkg, aku, akd, akg, node_identity_get, node_identity_set, maub, mag, \r\n"
		"			 frg, frs, lptg, krpg, krps, hpg, hps, hsg, hss>",
		mesh_config_help_table
	},
	{
		"mesh_goo",   "[AT+BTCMDHELP] mesh_goo: BLE mesh generic on off model operation\n\r"
		"usage: AT+BLEMESHGOO=[sub_cmd],...\n\r"
		"[subcmd] = <set, get>",
		mesh_goo_help_table
	},
	{
		"mesh_data",   "[AT+BTCMDHELP] mesh_data: BLE mesh datatrans model operation\n\r"
		"usage: AT+BLEMESHDATA=[sub_cmd],...\n\r"
		"[subcmd] = <write, read>",
		mesh_data_help_table
	},
	{
		"mesh_ll",   "[AT+BTCMDHELP] mesh_ll: BLE mesh light lightness model operation\n\r"
		"usage: AT+BLEMESHLL=[sub_cmd],...\n\r"
		"[subcmd] = <llg, lls, lllg, llls, lllag, lldg, llds, llrg, llrs>",
		mesh_ll_help_table
	},
	{
		"mesh_lctl",   "[AT+BTCMDHELP] mesh_lctl: BLE mesh light ctl model operation\n\r"
		"usage: AT+BLEMESHLCTL=[sub_cmd],...\n\r"
		"[subcmd] = <lcg, lcs, lctg, lcts, lctrg, lctrs, lcdg, lcds>",
		mesh_lctl_help_table
	},
	{
		"mesh_lhsl",   "[AT+BTCMDHELP] mesh_lhsl: BLE mesh light hsl model operation\n\r"
		"usage: AT+BLEMESHLHSL=[sub_cmd],...\n\r"
		"[subcmd] = <lhg, lhs, lhtg, lhhg, lhhs, lhsg, lhss, lhdg, lhds, lhrg, lhrs>",
		mesh_lhsl_help_table
	},
	{
		"mesh_lxyl",   "[AT+BTCMDHELP] mesh_lxyl: BLE mesh light xyl model operation\n\r"
		"usage: AT+BLEMESHLXYL=[sub_cmd],...\n\r"
		"[subcmd] = <lxg, lxs, lxtg, lxdg, lxds, lxrg, lxrs>",
		mesh_lxyl_help_table
	},
	{
		"mesh_llc",   "[AT+BTCMDHELP] mesh_llc: BLE mesh light lc model operation\n\r"
		"usage: AT+BLEMESHLLC=[sub_cmd],...\n\r"
		"[subcmd] = <llcmg, llcms, llcomg, llcoms, llcloog, llcloos, llcpg, llcps>",
		mesh_llc_help_table
	},
	{
		"mesh_time",   "[AT+BTCMDHELP] mesh_time: BLE mesh time model operation\n\r"
		"usage: AT+BLEMESHTIME=[sub_cmd],...\n\r"
		"[subcmd] = <tg, tzg, ttudg, trg, ts, tzs, ttuds, trs>",
		mesh_time_help_table
	},
	{
		"mesh_scheduler",   "[AT+BTCMDHELP] mesh_scheduler: BLE mesh scheduler model operation\n\r"
		"usage: AT+BLEMESHSCHEDULER=[sub_cmd],...\n\r"
		"[subcmd] = <scheg, scheag, scheas>",
		mesh_scheduler_help_table
	},
	{
		"mesh_scene",   "[AT+BTCMDHELP] mesh_scene: BLE mesh scene model operation\n\r"
		"usage: AT+BLEMESHSCENE=[sub_cmd],...\n\r"
		"[subcmd] = <sg, ss, sr, srg, sd>",
		mesh_scene_help_table
	},
	{
		"mesh_gdtt",   "[AT+BTCMDHELP] mesh_gdtt: BLE mesh generic default transition time model operation\n\r"
		"usage: AT+BLEMESHGDTT=[sub_cmd],...\n\r"
		"[subcmd] = <gdtts, gdttg>",
		mesh_gdtt_help_table
	},
	{
		"mesh_gle",   "[AT+BTCMDHELP] mesh_gle: BLE mesh generic level model operation\n\r"
		"usage: AT+BLEMESHGLE=[sub_cmd],...\n\r"
		"[subcmd] = <glg, gls, glds, glms>",
		mesh_gle_help_table
	},
	{
		"mesh_gpoo",   "[AT+BTCMDHELP] mesh_gpoo: BLE mesh generic power on off model operation\n\r"
		"usage: AT+BLEMESHGPOO=[sub_cmd],...\n\r"
		"[subcmd] = <gpg, gps>",
		mesh_gpoo_help_table
	},
	{
		"mesh_gpl",   "[AT+BTCMDHELP] mesh_gpl: BLE mesh generic power level model operation\n\r"
		"usage: AT+BLEMESHGPL=[sub_cmd],...\n\r"
		"[subcmd] = <gplg, gpls, gpllg, gpldg, gplds, gplrg, gplrs>",
		mesh_gpl_help_table
	},
	{
		"mesh_gb",   "[AT+BTCMDHELP] mesh_gb: BLE mesh generic battery model operation\n\r"
		"usage: AT+BLEMESHGB=[sub_cmd],...\n\r"
		"[subcmd] = <gbg>",
		mesh_gb_help_table
	},
	{
		"mesh_glo",   "[AT+BTCMDHELP] mesh_glo: BLE mesh generic location model operation\n\r"
		"usage: AT+BLEMESHGLO=[sub_cmd],...\n\r"
		"[subcmd] = <glgg, glgs, gllg, glls>",
		mesh_glo_help_table
	},
	{
		"mesh_gp",   "[AT+BTCMDHELP] mesh_glo: BLE mesh generic property model operation\n\r"
		"usage: AT+BLEMESHGP=[sub_cmd],...\n\r"
		"[subcmd] = <gupsg, gupg, gups, gapsg, gapg, gaps, gmpsg, gmpg, gmps, gcpsg>",
		mesh_gp_help_table
	},
	{
		"mesh_sensor",   "[AT+BTCMDHELP] mesh_sensor: BLE mesh sensor model operation\n\r"
		"usage: AT+BLEMESHSENSOR=[sub_cmd],...\n\r"
		"[subcmd] = <sdg, scg, scs, sssg, ssg, sss, sg, scog, sseg>",
		mesh_sensor_help_table
	},
	{
		"mesh_health",   "[AT+BTCMDHELP] mesh_health: BLE mesh health model operation\n\r"
		"usage: AT+BLEMESHHEALTH=[sub_cmd],...\n\r"
		"[subcmd] = <hfg, hfc, hft, hpg, hps, hag, has>",
		mesh_health_help_table
	},
	{
		"mesh_rmt",   "[AT+BTCMDHELP] mesh_rmt: BLE mesh remote provisioning model operation\n\r"
		"usage: AT+BLEMESHRMT=[sub_cmd],...\n\r"
		"[subcmd] = <scan_start, scan_cap_get, link_open>",
		mesh_rmt_help_table
	},
	{
		"mesh_df",   "[AT+BTCMDHELP] mesh_df: BLE mesh directed forwarding model operation\n\r"
		"usage: AT+BLEMESHDF=[sub_cmd],...\n\r"
		"[subcmd] = <dfpdis, dfpsol, dfpdupt, mdu, dcss, dcg, dcs, pmg, pms, dtcg, dtcs, fta, ftd, \n\r"
		"			 ftda, ftdd, ftdg, ftecg, fteg, wlg, wls, twpg, twps, peig, peis, dntg, dnts, \n\r"
		"			 drrg, drrs, rssitg, rssits, dpg, dppg, dpps, pdtcg, pdtcs, dcntg, dcnts, dcrrg, dcrrs>",
		mesh_df_help_table
	},
	{
		"mesh_sbr",   "[AT+BTCMDHELP] mesh_sbr: BLE mesh subnet bridge model operation\n\r"
		"usage: AT+BLEMESHSBR=[sub_cmd],...\n\r"
		"[subcmd] = <sbg, sbs, bta, btr, bsg, btbg, btsg>",
		mesh_sbr_help_table
	},
	{
		"mesh_prb",   "[AT+BTCMDHELP] mesh_prb: BLE mesh private beacon model operation\n\r"
		"usage: AT+BLEMESHPRB=[sub_cmd],...\n\r"
		"[subcmd] = <prbg, prbs, pgpg, pgps, pnig, pnis>",
		mesh_prb_help_table
	},
#endif
#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
	{
		"a2dp_cmd",    "[AT+BTCMDHELP] a2dp_cmd: BT A2DP operation\n\r"
		"usage: AT+BTA2DP=[sub_cmd],..\n\r"
		"[sub_cmd] = <conn, disconn, start, suspend>",
		a2dp_help_table
	},
	{
		"avrcp_cmd",    "[AT+BTCMDHELP] avrcp_cmd: BT AVRCP operation\n\r"
		"usage: AT+BTAVRCP=[sub_cmd],..\n\r"
		"[sub_cmd] = <conn, disconn, start, suspend>",
		avrcp_help_table
	},
	{
		"spp_cmd",    "[AT+BTCMDHELP] spp_cmd: BT SPP operation\n\r"
		"usage: AT+BTSPP=[sub_cmd],..\n\r"
		"[sub_cmd] = <conn, disconn, disconn_all, send_data, give_credits>",
		spp_help_table
	},
	{
		"rfc_cmd",    "[AT+BTCMDHELP] rfc_cmd: BT RFC operation\n\r"
		"usage: AT+BTRFC=[sub_cmd],..\n\r"
		"[sub_cmd] = <conn, disconn, send_data>",
		rfc_help_table
	},
#endif /* RTK_BREDR_SUPPORT */
	{NULL,},
};

cmd_help_table_t example_help_table[] = {
	{
		"central",     "[AT+BTDEMO] central: run as a BT central\n\r"
		"usage: AT+BTDEMO=central,[val]\n\r"
		"[val] = <0-(disable), 1-(enable), 2-(enable with predefined routine)>",
		NULL
	},
	{
		"peripheral", "[AT+BTDEMO] peripheral: run as a BT peripheral\n\r"
		"usage: AT+BTDEMO=peripheral,[val]\n\r"
		"[val] = <0-(disable), 1-(enable), 2-(enable with predefined routine)>",
		NULL
	},
	{
		"scatternet", "[AT+BTDEMO] central: run as a BT central\n\r"
		"usage: AT+BTDEMO=central,[val]\n\r"
		"[val] = <0-(disable), 1-(enable), 2-(enable with predefined routine)>",
		NULL
	},
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	{
		"provisioner", "[AT+BTDEMO] provisioner: run as a BT mesh provisioner\n\r"
		"usage: AT+BTDEMO=provisioner,[val]\n\r"
		"[val] = <0-(disable), 1-(enable) >",
		NULL
	},
	{
		"device", "[AT+BTDEMO] device: run as a BT mesh device\n\r"
		"usage: AT+BTDEMO=device,[val]\n\r"
		"[val] = <0-(disable), 1-(enable) >",
		NULL
	},
	{
		"provisioner_scatternet", "[AT+BTDEMO] provisioner_scatteret: run as a BT mesh provisioner scatternet\n\r"
		"usage: AT+BTDEMO=provisioner_scatternet,[val]\n\r"
		"[val] = <0-(disable), 1-(enable) >",
		NULL
	},
	{
		"device_scatternet", "[AT+BTDEMO] device_scatternet: run as a BT mesh device scatternet\n\r"
		"usage: AT+BTDEMO=device_scatternet,[val]\n\r"
		"[val] = <0-(disable), 1-(enable) >",
		NULL
	},
#endif
	{
		"a2dp", "[AT+BTDEMO] a2dp: run as a A2DP sink(snk))/source(src)\n\r"
		"usage: AT+BTDEMO=a2dp,[role][val]\n\r"
		"[role] = <snk-(sink), src-(source)>"
		"[val] = <0-(disable), 1-(enable)>",
		NULL
	},
	{
		"spp", "[AT+BTDEMO] spp: run as a SPP client(client))/server(server)\n\r"
		"usage: AT+BTDEMO=spp,[role][val]\n\r"
		"[role] = <client-(client), server-(server)>"
		"[val] = <0-(disable), 1-(enable)>",
		NULL
	},
	{
		"rfc", "[AT+BTDEMO] rfc: run as a RFC demo\n\r"
		"usage: AT+BTDEMO=rfc,[val]\n\r"
		"[val] = <0-(disable), 1-(enable)>",
		NULL
	},
	{
		"hfp", "[AT+BTDEMO] hfp: run as a HFP hf(hand free))/ag(audio gate)\n\r"
		"usage: AT+BTDEMO=hfp,[role][val]\n\r"
		"[role] = <hf-(hand free), ag-(audio gate)>"
		"[val] = <0-(disable), 1-(enable)>",
		NULL
	},
	{
		"throughput", "[AT+BTDEMO] throughput: run as a Throughput scatternet\n\r"
		"usage: AT+BTDEMO=throughput,[act],[val]\n\r"
		"[act] = <1, 0, conn, test_start, test_stop>\n\r"
		"[val] = DEPENDS ON DIFFERENT ACTS",
		NULL
	},
	{NULL,},
};

cmd_help_table_t vendor_help_table[] = {
	{
		"tx_power_gain",     "[AT+BTVENDOR] tx_power_gain: manual set bt tx power gain index\n\r"
		"usage: AT+BTVENDOR=tx_power_gain,[val]\n\r"
		"[val] = <0~0xFF>",
		NULL
	},
	{
		"hci_debug_enable",     "[AT+BTVENDOR] hci_debug_enable: enable hci uart to external pin\n\r"
		"usage: AT+BTVENDOR=hci_debug_enable\n\r",
		NULL
	},
	{
		"bt_debug_port",     "[AT+BTVENDOR] bt_debug_port: enable BT debug port to external pin\n\r"
		"usage: AT+BTVENDOR=bt_debug_port,enable,[bt_sel],[type],{[mask]/[dbg_port_name]{,[pad_name]}}\n\r"
		"[bt_sel] = <bt_vendor, bt_on>\n\r"
		"[type] = <0-(mask), 1-(pad specified)>\n\r"
		"[mask] = <0-0xFFFFFFFF(decimal or hexnum)>\n\r"
		"[dbg_port_name] = <0-31(decimal or hexnum)>\n\r"
		"[pad_name] = <string>\n\r"
		"\n\r"
		"usage: AT+BTVENDOR=bt_debug_port,shift,[original],[mapping]\n\r"
		"[original] = <0-31(decimal or hexnum)>\n\r"
		"[mapping] = <0-7(decimal or hexnum)>\n\r",
		NULL
	},
	{
		"bt_gpio",     "[AT+BTVENDOR] bt_gpio: enable BT GPIO to external pin\n\r"
		"usage: AT+BTVENDOR=bt_gpio,[gpio_name]{,[pad_name]}\n\r"
		"[gpio_name] = <0-MAX (decimal or hexnum)>\n\r"
		"[pad_name] = <string>\n\r",
		NULL
	},
	{
		"dlps",     "[AT+BTVENDOR] dlps: disable or enable BT fw dlps\n\r"
		"usage: AT+BTVENDOR=dlps,0\n\r",
		NULL
	},
	{
		"sof",     "[AT+BTVENDOR] sof: disable or enable BT fw sof and eof indication\n\r"
		"usage: AT+BTVENDOR=sof,[conn_handle],[val]\n\r"
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
		BT_LOGA("%s\r\n", help_usage);
		return;
	}

	char *cmd_str = argv[0];
	while (cmd_help_table[i].name) {
		if (0 == strcmp(cmd_str, cmd_help_table[i].name)) {
			if (argc == 1) {
				BT_LOGA("%s\r\n", cmd_help_table[i].descriptor);
			}
			goto SUB_CMD_HELP;
		}
		i++;
	}

	BT_LOGE("[%s] Error: Cant find this cmd help\r\n", tag);
	return;

SUB_CMD_HELP:
	if (argc >= 2) {
		char *sub_cmd_str = argv[1];
		cmd_help_table_t *sub_cmd_tbl = cmd_help_table[i].sub_tbl;
		while (sub_cmd_tbl[j].name) {
			if (0 == strcmp(sub_cmd_str, sub_cmd_tbl[j].name)) {
				BT_LOGA("%s\r\n", sub_cmd_tbl[j].descriptor);
				break;
			}
			j++;
		}

		if (!sub_cmd_tbl[j].name)
			BT_LOGE("[%s] Error: Cant find this subcmd help in cmd: %s\r\n",
					tag, cmd_help_table[i].name);
	}
}

static int atcmd_bt_cmd_help(int argc, char *argv[])
{
	const char *help_usage =  "[AT+BTCMDHELP] help: show BT cmds usage and description\n\r"
							  "usage: AT+BTCMDHELP=[cmd]\n\r"
							  "       AT+BTCMDHELP=[cmd],[subcmd]\n\r"
							  "[cmd] = <bt, ble_gap, br_gap, gattc, gatts, mesh_stack, mesh_data, mesh_config, mesh_goo, mesh_rmt\n\r"
							  "			mesh_ll, mesh_lctl, mesh_lhsl, mesh_lxyl, mesh_llc, a2dp, avrcp, spp_cmd, rfc_cmd>\n\r"
							  "[subcmd] = 'use AT+BTCMDHELP=[cmd] to show subcmds'";
	atcmd_bt_help_common(argc, argv, "AT+BTCMDHELP", help_usage, cmd_help_table);
	return 0;
}

static int atcmd_bt_example_help(int argc, char *argv[])
{
	const char *help_usage =  "[AT+BTDEMO] help: show AT+BTDEMO cmds usage and description\n\r"
							  "usage: AT+BTDEMO=help,[cmd]\n\r"
							  "       AT+BTDEMO=help,[cmd],[subcmd]\n\r"
							  "[cmd] = <central, peripheral, scatternet, provisioner, device, provisioner_scatternet, device_scatternet, a2dp, spp, rfc, hfp>\n\r"
							  "[subcmd] = 'use AT+BTDEMO=help,[cmd] to show subcmds'";
	atcmd_bt_help_common(argc, argv, "AT+BTDEMO", help_usage, example_help_table);
	return 0;
}

static int atcmd_bt_vendor_help(int argc, char *argv[])
{
	const char *help_usage =  "[AT+BTVENDOR] help: show ATBV cmds usage and description\n\r"
							  "usage: AT+BTVENDOR=help,[cmd]\n\r"
							  "       AT+BTVENDOR=help,[cmd],[subcmd]\n\r"
							  "[cmd] = <tx_power_gain, hci_debug_enable, dlps>\n\r"
							  "[subcmd] = 'use AT+BTVENDOR=help,[cmd] to show subcmds'";
	atcmd_bt_help_common(argc, argv, "AT+BTVENDOR", help_usage, vendor_help_table);
	return 0;
}

#endif  /* BT_ATCMD_HELP */

#define CMD_NAME_BT_DEMO         "+BTDEMO"
#define CMD_NAME_BT_VENDOR       "+BTVENDOR"
#if defined(BT_ATCMD_HELP) && BT_ATCMD_HELP
#define CMD_NAME_HELP            "+BTCMDHELP"
#endif
#define CMD_NAME_BLE_GAP         "+BLEGAP"
#if defined(RTK_BLE_GATTS) && RTK_BLE_GATTS
#define CMD_NAME_GATTS           "+BLEGATTS"
#endif
#if defined(RTK_BLE_GATTC) && RTK_BLE_GATTC
#define CMD_NAME_GATTC           "+BLEGATTC"
#endif
#define CMD_NAME_GAP             "+BTGAP"
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#define CMD_NAME_MESH_STACK      "+BLEMESHSTACK"
#define CMD_NAME_MESH_DATA       "+BLEMESHDATA"
#define CMD_NAME_MESH_CONFIG     "+BLEMESHCONFIG"
#define CMD_NAME_MESH_GOO        "+BLEMESHGOO"
#define CMD_NAME_MESH_RMT        "+BLEMESHRMT"
#define CMD_NAME_MESH_LL         "+BLEMESHLL"
#define CMD_NAME_MESH_LCTL       "+BLEMESHLCTL"
#define CMD_NAME_MESH_LHSL       "+BLEMESHLHSL"
#define CMD_NAME_MESH_LXYL       "+BLEMESHLXYL"
#define CMD_NAME_MESH_LLC        "+BLEMESHLLC"
#define CMD_NAME_MESH_TIME       "+BLEMESHTIME"
#define CMD_NAME_MESH_SCHEDULER  "+BLEMESHSCHEDULER"
#define CMD_NAME_MESH_SCENE      "+BLEMESHSCENE"
#define CMD_NAME_MESH_GDTT       "+BLEMESHGDTT"
#define CMD_NAME_MESH_GLE        "+BLEMESHGLE"
#define CMD_NAME_MESH_GPOO       "+BLEMESHGPOO"
#define CMD_NAME_MESH_GPL        "+BLEMESHGPL"
#define CMD_NAME_MESH_GB         "+BLEMESHGB"
#define CMD_NAME_MESH_GLO        "+BLEMESHGLO"
#define CMD_NAME_MESH_GP         "+BLEMESHGP"
#define CMD_NAME_MESH_SENSOR     "+BLEMESHSENSOR"
#define CMD_NAME_MESH_HEALTH     "+BLEMESHHEALTH"
#define CMD_NAME_MESH_DF         "+BLEMESHDF"
#define CMD_NAME_MESH_SBR        "+BLEMESHSBR"
#define CMD_NAME_MESH_PRB        "+BLEMESHPRB"
#define CMD_NAME_MESH_DFU        "+BLEMESHDFU"
#endif /* RTK_BLE_MESH_SUPPORT */
#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
#define CMD_NAME_BR_GAP          "+BRGAP"
#define CMD_NAME_SDP             "+BTSDP"
#define CMD_NAME_A2DP            "+BTA2DP"
#define CMD_NAME_AVRCP           "+BTAVRCP"
#define CMD_NAME_SPP             "+BTSPP"
#define CMD_NAME_RFC             "+BTRFC"
#define CMD_NAME_HID             "+BTHID"
#define CMD_NAME_HFP             "+BTHFP"
#define CMD_NAME_PBAP            "+BTPBAP"
#endif /* RTK_BREDR_SUPPORT */
#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
#define CMD_NAME_ISO             "+BLEISO"
#endif
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
#define CMD_NAME_BAP             "+BLEBAP"
#define CMD_NAME_CAP             "+BLECAP"
#define CMD_NAME_TMAP            "+BLETMAP"
#define CMD_NAME_GMAP            "+BLEGMAP"
#endif /* RTK_BLE_AUDIO_SUPPORT */

static const cmd_table_t cmd_table[] = {
	{CMD_NAME_BLE_GAP,          atcmd_bt_le_gap,                                2, 21},
#if defined(RTK_BLE_GATTS) && RTK_BLE_GATTS
	{CMD_NAME_GATTS,            atcmd_bt_gatts,                                 3, 16},
#endif
#if defined(RTK_BLE_GATTC) && RTK_BLE_GATTC
	{CMD_NAME_GATTC,            atcmd_bt_gattc,                                 3, 14},
#endif
	{CMD_NAME_GAP,              atcmd_bt_gap,                                   1, 13},
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	{CMD_NAME_MESH_STACK,       atcmd_bt_mesh_stack,                            2, 8},
	{CMD_NAME_MESH_DATA,        atcmd_bt_mesh_datatrans_model,                  5, 6},
	{CMD_NAME_MESH_CONFIG,      atcmd_bt_mesh_config,                           3, 13},
	{CMD_NAME_MESH_GOO,         atcmd_bt_mesh_generic_onoff,                    4, 9},
	{CMD_NAME_MESH_RMT,         atcmd_bt_mesh_remote_prov_client_model,         2, 7},
	{CMD_NAME_MESH_LL,          atcmd_bt_mesh_light_lightness,                  4, 9},
	{CMD_NAME_MESH_LCTL,        atcmd_bt_mesh_light_ctl,                        4, 11},
	{CMD_NAME_MESH_LHSL,        atcmd_bt_mesh_light_hsl,                        4, 11},
	{CMD_NAME_MESH_LXYL,        atcmd_bt_mesh_light_xyl,                        4, 11},
	{CMD_NAME_MESH_LLC,         atcmd_bt_mesh_light_lc,                         4, 9},
	{CMD_NAME_MESH_TIME,        atcmd_bt_mesh_time,                             4, 10},
	{CMD_NAME_MESH_SCHEDULER,   atcmd_bt_mesh_scheduler,                        4, 17},
	{CMD_NAME_MESH_SCENE,       atcmd_bt_mesh_scene,                            4, 9},
	{CMD_NAME_MESH_GDTT,        atcmd_bt_mesh_generic_default_transition_time,  4, 7},
	{CMD_NAME_MESH_GLE,         atcmd_bt_mesh_generic_level,                    4, 9},
	{CMD_NAME_MESH_GPOO,        atcmd_bt_mesh_generic_power_on_off,             4, 6},
	{CMD_NAME_MESH_GPL,         atcmd_bt_mesh_generic_power_level,              4, 9},
	{CMD_NAME_MESH_GB,          atcmd_bt_mesh_generic_battery,                  4, 4},
	{CMD_NAME_MESH_GLO,         atcmd_bt_mesh_generic_location,                 4, 13},
	{CMD_NAME_MESH_GP,          atcmd_bt_mesh_generic_property,                 4, 8},
	{CMD_NAME_MESH_SENSOR,      atcmd_bt_mesh_sensor,                           4, 14},
	{CMD_NAME_MESH_HEALTH,      atcmd_bt_mesh_health,                           4, 7},
	{CMD_NAME_MESH_DF,          atcmd_bt_mesh_df,                               4, 21},
	{CMD_NAME_MESH_SBR,         atcmd_bt_mesh_sbr,                              4, 9},
	{CMD_NAME_MESH_PRB,         atcmd_bt_mesh_prb,                              4, 6},
	{CMD_NAME_MESH_DFU,         atcmd_bt_mesh_device_firmware_update,           3, 11},
#endif  // end of RTK_BLE_MESH_SUPPORT
#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
	{CMD_NAME_BR_GAP,           atcmd_bt_br_gap,                                2, 13},
	{CMD_NAME_SDP,              atcmd_bt_sdp_cmd,                               1, 3},
	{CMD_NAME_A2DP,             atcmd_bt_a2dp_cmd,                              1, 8},
	{CMD_NAME_AVRCP,            atcmd_bt_avrcp_cmd,                             1, 5},
	{CMD_NAME_SPP,              atcmd_bt_spp_cmd,                               1, 8},
	{CMD_NAME_RFC,              atcmd_bt_rfc_cmd,                               1, 8},
	{CMD_NAME_HID,              atcmd_bt_hid_cmd,                               1, 23},
	{CMD_NAME_HFP,              atcmd_bt_hfp_cmd,                               1, 8},
	{CMD_NAME_PBAP,             atcmd_bt_pbap_cmd,                              1, 8},
#endif
#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
	{CMD_NAME_ISO,              atcmd_bt_iso_cmd,                               1, 9},
#endif
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
	{CMD_NAME_BAP,              atcmd_bt_bap_cmd,                               2, 10},
	{CMD_NAME_CAP,              atcmd_bt_cap_cmd,                               3, 10},
#if (defined(RTK_BLE_AUDIO_TMAP_SUPPORT) && RTK_BLE_AUDIO_TMAP_SUPPORT)
	{CMD_NAME_TMAP,             atcmd_bt_tmap_cmd,                              3, 10},
#endif
#if defined(RTK_BLE_AUDIO_GMAP_SUPPORT) && RTK_BLE_AUDIO_GMAP_SUPPORT
	{CMD_NAME_GMAP,             atcmd_bt_gmap_cmd,                              3, 10},
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
#if defined(CONFIG_BT_HOGP) && CONFIG_BT_HOGP
	{"hogp",       atcmd_bt_hogp_gamepad,           2, 2},
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
#if defined(CONFIG_BT_OTA_CENTRAL) && CONFIG_BT_OTA_CENTRAL
	{"ota_central",      atcmd_bt_ota_central,      2, 2},
#endif
#if defined(CONFIG_BT_OTA_PERIPHERAL) && CONFIG_BT_OTA_PERIPHERAL
	{"ota_peripheral",   atcmd_bt_ota_peripheral,   2, 2},
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
	{"iso",              atcmd_ble_iso,             4, 4},
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
#if defined(CONFIG_BT_A2DP_HFP_LE_AUDIO_PBP) && CONFIG_BT_A2DP_HFP_LE_AUDIO_PBP
	{"a2dp_hfp_pbp",     atcmd_bt_a2dp_hfp_pbp,     2, 2},
#endif
#if defined(CONFIG_BT_A2DP_LE_AUDIO_TMAP) && CONFIG_BT_A2DP_LE_AUDIO_TMAP
	{"a2dp_tmap",        atcmd_bt_a2dp_tmap,        3, 3},
#endif
#if defined(CONFIG_BT_SPP) && CONFIG_BT_SPP
	{"spp",              atcmd_bt_spp,              3, 6},
#endif
#if defined(CONFIG_BT_RFC) && CONFIG_BT_RFC
	{"rfc",              atcmd_bt_rfc,              2, 3},
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
#if defined(CONFIG_BT_LE_AUDIO_GENERIC_DEMO) && CONFIG_BT_LE_AUDIO_GENERIC_DEMO
	{"generic_le_audio_demo", atcmd_bt_generic_le_audio_demo, 3, 4},
#endif
#if defined(CONFIG_BT_PBP) && CONFIG_BT_PBP
	{"pbp",              atcmd_bt_pbp,              3, 4},
#endif
#if defined(CONFIG_BT_TMAP) && CONFIG_BT_TMAP
	{"tmap",             atcmd_bt_tmap,             3, 4},
#endif
#if defined(CONFIG_BT_GMAP) && CONFIG_BT_GMAP
	{"gmap",             atcmd_bt_gmap,             3, 4},
#endif
#if defined(CONFIG_BT_PTS) && CONFIG_BT_PTS
	{"pts",              atcmd_bt_pts,              2, 4},
#endif
	// {"demo",             atcmd_bt_demo,             1, 1},
#if defined(CONFIG_BT_TRANSFER_MODULE) && CONFIG_BT_TRANSFER_MODULE
	{"transfer_module",  atcmd_bt_transfer_module,  2, 6},
#endif
#if defined(CONFIG_BT_WIFIMATE_DEVICE) && CONFIG_BT_WIFIMATE_DEVICE
	{"ble_wifimate_device", atcmd_bt_wifimate_device, 2, 3},
#endif
#if defined(CONFIG_BT_WIFIMATE_CONFIGURATOR) && CONFIG_BT_WIFIMATE_CONFIGURATOR
	{"ble_wifimate_configurator", atcmd_bt_wifimate_configurator, 2, 6},
#endif
	{NULL,},
};

static const cmd_table_t vendor_table[] = {
#if defined(BT_ATCMD_HELP) && BT_ATCMD_HELP
	{"help",             atcmd_bt_vendor_help,      1, 3},
#endif
	{"bt_enable",        atcmd_bt_enable,           2, 2},
	{"bt_power",         atcmd_bt_power,            2, 2},
	{"tx_power_gain",    atcmd_bt_tx_power_gain,    2, 5},
	{"hci_debug_enable", atcmd_bt_hci_debug_enable, 1, 1},
	{"bt_debug_port",    atcmd_bt_debug_port,       4, 6},
	{"bt_gpio",          atcmd_bt_gpio,             2, 3},
	{"sleep",            atcmd_bt_sleep_mode,       2, 2},
	/*{"ant",              atcmd_bt_ant,              2, 2},*/
	{"tx_power",         atcmd_bt_set_tx_power,     4, 5},
	{"sof",              atcmd_bt_sof_eof_ind,      2, 3},
#if defined(CONFIG_BT_INIC) && CONFIG_BT_INIC
	{"remote_wakeup",    atcmd_bt_remote_wakeup,    1, 1},
#endif
	{NULL,},
};

static void atcmd_bt_cmd(void *arg, char *cmd_name, char *tag)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	int ret = 0;

	if (!arg) {
		BT_LOGE("%s Error: No input args number!\r\n", tag);
		goto exit;
	}

	argc = parse_param(arg, argv);
	if (argc < 2) {
		BT_LOGE("%s Error: Wrong input args number!\r\n", tag);
		goto exit;
	}

	argv[0] = cmd_name;
	ret = atcmd_bt_excute(argc, &argv[0], cmd_table, tag);
	if (ret == 0) {
		BT_AT_PRINTOK();
	} else {
		BT_AT_PRINTERROR(ret);
	}
	return;

exit:
	BT_LOGA("%s Info: Use '%s' to help\r\n", tag, "AT+BTCMDHELP");
	BT_AT_PRINTERROR(BT_AT_ERR_PARAM_INVALID);
}

static inline int atcmd_bt_example(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, example_table, "[AT+BTDEMO]");
}

static inline int atcmd_bt_vendor(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, vendor_table, "[AT+BTVENDOR]");
}

static void fBTDEMO(void *arg)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	int ret = 0;

	if (!arg) {
		BT_LOGE("[AT+BTDEMO] Error: No input args number!\r\n");
		goto exit;
	}

	argc = parse_param(arg, argv);
	if (argc < 2) {
		BT_LOGE("[AT+BTDEMO] Error: Wrong input args number!\r\n");
		goto exit;
	}

	ret = atcmd_bt_example(argc - 1, &argv[1]);
	if (ret == 0) {
		BT_AT_PRINTOK();
	} else {
		BT_AT_PRINTERROR(ret);
	}
	return;

exit:
	BT_LOGA("[AT+BTDEMO] Info: Use 'AT+BTDEMO=help' to help\r\n");
	BT_AT_PRINTERROR(BT_AT_ERR_PARAM_INVALID);
}

static inline void fBLEGAP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_BLE_GAP, "[AT+BLEGAP]");
}

static inline void fBLEGATTS(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_GATTS, "[AT+BLEGATTS]");
}

static inline void fBLEGATTC(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_GATTC, "[AT+BLEGATTC]");
}

#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
static inline void fBRGAP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_BR_GAP, "[AT+BRGAP]");
}

static inline void fBTSDP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_SDP, "[AT+BTSDP]");
}

static inline void fBTA2DP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_A2DP, "[AT+BTA2DP]");
}

static inline void fBTAVRCP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_AVRCP, "[AT+BTAVRCP]");
}

static inline void fBTSPP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_SPP, "[AT+BTSPP]");
}

static inline void fBTRFC(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_RFC, "[AT+BTRFC]");
}

static inline void fBTHID(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_HID, "[AT+BTHID]");
}

static inline void fBTHFP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_HFP, "[AT+BTHFP]");
}

static inline void fBTPBAP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_PBAP, "[AT+BTPBAP]");
}

#endif /* RTK_BREDR_SUPPORT */

#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
static inline void fBLEISO(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_ISO, "[AT+BLEISO]");
}
#endif /* RTK_BLE_ISO_SUPPORT */

#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT

static inline void fBLEBAP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_BAP, "[AT+BLEBAP]");
}

static inline void fBLECAP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_CAP, "[AT+BLECAP]");
}

#if (defined(RTK_BLE_AUDIO_TMAP_SUPPORT) && RTK_BLE_AUDIO_TMAP_SUPPORT)
static inline void fBLETMAP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_TMAP, "[AT+BLETMAP]");
}
#endif /* RTK_BLE_AUDIO_TMAP_SUPPORT */

#if defined(RTK_BLE_AUDIO_GMAP_SUPPORT) && RTK_BLE_AUDIO_GMAP_SUPPORT
static inline void fBLEGMAP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_GMAP, "[AT+BLEGMAP]");
}
#endif /* RTK_BLE_AUDIO_GMAP_SUPPORT */

#endif /* RTK_BLE_AUDIO_SUPPORT */

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT

static inline void fBLEMESHSTACK(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_STACK, "[AT+BLEMESHSTACK]");
}

static inline void fBLEMESHDATA(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_DATA, "[AT+BLEMESHDATA]");
}

static inline void fBLEMESHCONFIG(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_CONFIG, "[AT+BLEMESHCONFIG]");
}

static inline void fBLEMESHGOO(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_GOO, "[AT+BLEMESHGOO]");
}

static inline void fBLEMESHRMT(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_RMT, "[AT+BLEMESHRMT]");
}

static inline void fBLEMESHLL(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_LL, "[AT+BLEMESHLL]");
}

static inline void fBLEMESHLCTL(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_LCTL, "[AT+BLEMESHLCTL]");
}

static inline void fBLEMESHLHSL(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_LHSL, "[AT+BLEMESHLHSL]");
}

static inline void fBLEMESHLXYL(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_LXYL, "[AT+BLEMESHLXYL]");
}

static inline void fBLEMESHLLC(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_LLC, "[AT+BLEMESHLLC]");
}

static inline void fBLEMESHTIME(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_TIME, "[AT+BLEMESHTIME]");
}

static inline void fBLEMESHSCHEDULER(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_SCHEDULER, "[AT+BLEMESHSCHEDULER]");
}

static inline void fBLEMESHSCENE(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_SCENE, "[AT+BLEMESHSCENE]");
}

static inline void fBLEMESHGDTT(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_GDTT, "[AT+BLEMESHGDTT]");
}

static inline void fBLEMESHGLE(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_GLE, "[AT+BLEMESHGLE]");
}

static inline void fBLEMESHGPOO(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_GPOO, "[AT+BLEMESHGPOO]");
}

static inline void fBLEMESHGPL(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_GPL, "[AT+BLEMESHGPL]");
}

static inline void fBLEMESHGB(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_GB, "[AT+BLEMESHGB]");
}

static inline void fBLEMESHGLO(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_GLO, "[AT+BLEMESHGLO]");
}

static inline void fBLEMESHGP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_GP, "[AT+BLEMESHGP]");
}

static inline void fBLEMESHSENSOR(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_SENSOR, "[AT+BLEMESHSENSOR]");
}

static inline void fBLEMESHHEALTH(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_HEALTH, "[AT+BLEMESHHEALTH]");
}

static inline void fBLEMESHDF(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_DF, "[AT+BLEMESHDF]");
}

static inline void fBLEMESHSBR(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_SBR, "[AT+BLEMESHSBR]");
}

static inline void fBLEMESHPRB(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_PRB, "[AT+BLEMESHPRB]");
}

static inline void fBLEMESHDFU(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_DFU, "[AT+BLEMESHDFU]");
}

#endif /* RTK_BLE_MESH_SUPPORT */

static inline void fBTGAP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_GAP, "[AT+BTGAP]");
}

#if defined(BT_ATCMD_HELP) && BT_ATCMD_HELP
static inline void fBTCMDHELP(void *arg)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	int ret = 0;

	if (!arg) {
		ret = atcmd_bt_cmd_help(argc, argv);
	} else {
		argc = parse_param(arg, argv);
		if (argc < 1 || argc > 3) {
			BT_LOGE("[AT+BTCMDHELP] Error: Wrong input args number!\r\n");
			BT_AT_PRINTERROR(BT_AT_ERR_PARAM_INVALID);
			return;
		}

		ret = atcmd_bt_cmd_help(argc - 1, &argv[1]);
	}

	if (ret == 0) {
		BT_AT_PRINTOK();
	} else {
		BT_AT_PRINTERROR(ret);
	}
	return;
}
#endif

static void fBTVENDOR(void *arg)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	int ret = 0;

	if (!arg) {
		BT_LOGE("[AT+BTVENDOR] Error: No input args number!\r\n");
		goto exit;
	}

	argc = parse_param(arg, argv);
	if (argc < 2) {
		BT_LOGE("[AT+BTVENDOR] Error: Wrong input args number!\r\n");
		goto exit;
	}

	ret = atcmd_bt_vendor(argc - 1, &argv[1]);
	if (ret == 0) {
		BT_AT_PRINTOK();
	} else {
		BT_AT_PRINTERROR(ret);
	}
	return;

exit:
	BT_LOGA("[AT+BTVENDOR] Info: Use 'AT+BTVENDOR=help' to help\r\n");
	BT_AT_PRINTERROR(BT_AT_ERR_PARAM_INVALID);
}

static log_item_t at_bt_items[] = {
	{CMD_NAME_BT_DEMO,          fBTDEMO,              {NULL, NULL}},
#if defined(BT_ATCMD_HELP) && BT_ATCMD_HELP
	{CMD_NAME_HELP,             fBTCMDHELP,           {NULL, NULL}},
#endif
	{CMD_NAME_BLE_GAP,          fBLEGAP,              {NULL, NULL}},
#if defined(RTK_BLE_GATTS) && RTK_BLE_GATTS
	{CMD_NAME_GATTS,            fBLEGATTS,            {NULL, NULL}},
#endif
#if defined(RTK_BLE_GATTC) && RTK_BLE_GATTC
	{CMD_NAME_GATTC,            fBLEGATTC,            {NULL, NULL}},
#endif
	{CMD_NAME_GAP,              fBTGAP,               {NULL, NULL}},
#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
	{CMD_NAME_BR_GAP,           fBRGAP,               {NULL, NULL}},
	{CMD_NAME_SDP,              fBTSDP,               {NULL, NULL}},
	{CMD_NAME_A2DP,             fBTA2DP,              {NULL, NULL}},
	{CMD_NAME_AVRCP,            fBTAVRCP,             {NULL, NULL}},
	{CMD_NAME_SPP,              fBTSPP,               {NULL, NULL}},
	{CMD_NAME_RFC,              fBTRFC,               {NULL, NULL}},
	{CMD_NAME_HID,              fBTHID,               {NULL, NULL}},
	{CMD_NAME_HFP,              fBTHFP,               {NULL, NULL}},
	{CMD_NAME_PBAP,             fBTPBAP,              {NULL, NULL}},
#endif /* RTK_BREDR_SUPPORT */
#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
	{CMD_NAME_ISO,              fBLEISO,              {NULL, NULL}},
#endif
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
	{CMD_NAME_BAP,              fBLEBAP,              {NULL, NULL}},
	{CMD_NAME_CAP,              fBLECAP,              {NULL, NULL}},
#if (defined(RTK_BLE_AUDIO_TMAP_SUPPORT) && RTK_BLE_AUDIO_TMAP_SUPPORT)
	{CMD_NAME_TMAP,             fBLETMAP,             {NULL, NULL}},
#endif
#if defined(RTK_BLE_AUDIO_GMAP_SUPPORT) && RTK_BLE_AUDIO_GMAP_SUPPORT
	{CMD_NAME_GMAP,             fBLEGMAP,             {NULL, NULL}},
#endif
#endif /* RTK_BLE_AUDIO_SUPPORT */
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	{CMD_NAME_MESH_STACK,       fBLEMESHSTACK,        {NULL, NULL}},
	{CMD_NAME_MESH_DATA,        fBLEMESHDATA,         {NULL, NULL}},
	{CMD_NAME_MESH_CONFIG,      fBLEMESHCONFIG,       {NULL, NULL}},
	{CMD_NAME_MESH_GOO,         fBLEMESHGOO,          {NULL, NULL}},
	{CMD_NAME_MESH_RMT,         fBLEMESHRMT,          {NULL, NULL}},
	{CMD_NAME_MESH_LL,          fBLEMESHLL,           {NULL, NULL}},
	{CMD_NAME_MESH_LCTL,        fBLEMESHLCTL,         {NULL, NULL}},
	{CMD_NAME_MESH_LHSL,        fBLEMESHLHSL,         {NULL, NULL}},
	{CMD_NAME_MESH_LXYL,        fBLEMESHLXYL,         {NULL, NULL}},
	{CMD_NAME_MESH_LLC,         fBLEMESHLLC,          {NULL, NULL}},
	{CMD_NAME_MESH_TIME,        fBLEMESHTIME,         {NULL, NULL}},
	{CMD_NAME_MESH_SCHEDULER,   fBLEMESHSCHEDULER,    {NULL, NULL}},
	{CMD_NAME_MESH_SCENE,       fBLEMESHSCENE,        {NULL, NULL}},
	{CMD_NAME_MESH_GDTT,        fBLEMESHGDTT,         {NULL, NULL}},
	{CMD_NAME_MESH_GLE,         fBLEMESHGLE,          {NULL, NULL}},
	{CMD_NAME_MESH_GPOO,        fBLEMESHGPOO,         {NULL, NULL}},
	{CMD_NAME_MESH_GPL,         fBLEMESHGPL,          {NULL, NULL}},
	{CMD_NAME_MESH_GB,          fBLEMESHGB,           {NULL, NULL}},
	{CMD_NAME_MESH_GLO,         fBLEMESHGLO,          {NULL, NULL}},
	{CMD_NAME_MESH_GP,          fBLEMESHGP,           {NULL, NULL}},
	{CMD_NAME_MESH_SENSOR,      fBLEMESHSENSOR,       {NULL, NULL}},
	{CMD_NAME_MESH_HEALTH,      fBLEMESHHEALTH,       {NULL, NULL}},
	{CMD_NAME_MESH_DF,          fBLEMESHDF,           {NULL, NULL}},
	{CMD_NAME_MESH_SBR,         fBLEMESHSBR,          {NULL, NULL}},
	{CMD_NAME_MESH_PRB,         fBLEMESHPRB,          {NULL, NULL}},
	{CMD_NAME_MESH_DFU,         fBLEMESHDFU,          {NULL, NULL}},
#endif /* RTK_BLE_MESH_SUPPORT */
	{CMD_NAME_BT_VENDOR,        fBTVENDOR,            {NULL, NULL}},
};

/* BT atcmd as a part of AT command "AT+LIST". */
void print_bt_ext_at(void)
{
#if ((defined(CONFIG_MP_INCLUDED) && CONFIG_MP_INCLUDED) && (defined(CONFIG_MP_SHRINK) && CONFIG_MP_SHRINK)) || \
    ((!defined(CONFIG_MP_INCLUDED) || !CONFIG_MP_INCLUDED) && (defined(CONFIG_BT_EXCLUDE_AT_COMMAND) && CONFIG_BT_EXCLUDE_AT_COMMAND)) || \
    (defined(CONFIG_BT_INIC) && CONFIG_BT_INIC)
	//Print nothing
#else

#if (defined(CONFIG_ATCMD_HOST_CONTROL) && CONFIG_ATCMD_HOST_CONTROL)
	at_printf("AT+BTDEMO\r\n");
	at_printf("AT+BLEGAP\r\n");
	at_printf("AT+BLEGATTS\r\n");
	at_printf("AT+BLEGATTC\r\n");
#else
	int index;
	int num = 0;

	num = sizeof(at_bt_items) / sizeof(at_bt_items[0]);
	for (index = 0; index < num; index++) {
		if (0 != strcmp(CMD_NAME_BT_VENDOR, at_bt_items[index].log_cmd)) {
			at_printf("AT%s\r\n", at_bt_items[index].log_cmd);
		}
	}
#endif //CONFIG_ATCMD_HOST_CONTROL

#endif
}

void at_bt_init(void)
{
#if ((defined(CONFIG_MP_INCLUDED) && CONFIG_MP_INCLUDED) && (defined(CONFIG_MP_SHRINK) && CONFIG_MP_SHRINK)) || \
    ((!defined(CONFIG_MP_INCLUDED) || !CONFIG_MP_INCLUDED) && (defined(CONFIG_BT_EXCLUDE_AT_COMMAND) && CONFIG_BT_EXCLUDE_AT_COMMAND)) || \
    (defined(CONFIG_BT_INIC) && CONFIG_BT_INIC)
	(void)at_bt_items;
#else
	atcmd_service_add_table(at_bt_items, sizeof(at_bt_items) / sizeof(at_bt_items[0]));
#endif
}