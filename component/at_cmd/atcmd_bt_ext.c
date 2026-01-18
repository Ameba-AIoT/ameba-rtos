/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "atcmd_service.h"
#include "atcmd_bt_impl.h"
#include "bt_utils.h"
#include "bt_api_config.h"


static bool bt_parse_param(void *arg, int *argc, char **argv)
{
	if (!arg) {
		BT_LOGE("[AT] Error: No input args number!\r\n");
		return false;
	}

	*argc = parse_param(arg, argv);
	if (*argc < 2) {
		BT_LOGE("[AT] Error: Wrong input args number!\r\n");
		return false;
	}

	return true;
}

static void fBTDEMO(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_example(argc - 1, &argv[1]);
	}
}

#if defined(RTK_BLE_SUPPORT) && RTK_BLE_SUPPORT
static inline void fBLEGAP(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_le_gap(argc - 1, &argv[1]);
	}
}
#endif /* RTK_BLE_SUPPORT */

#if defined(RTK_BLE_GATTS_SUPPORT) && RTK_BLE_GATTS_SUPPORT
static inline void fBLEGATTS(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_gatts(argc - 1, &argv[1]);
	}
}
#endif /* RTK_BLE_GATTS_SUPPORT */

#if defined(RTK_BLE_GATTC_SUPPORT) && RTK_BLE_GATTC_SUPPORT
static inline void fBLEGATTC(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_gattc(argc - 1, &argv[1]);
	}
}
#endif /* RTK_BLE_GATTC_SUPPORT */

#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
static inline void fBRGAP(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_br_gap(argc - 1, &argv[1]);
	}
}

#if defined(RTK_BT_PROFILE_A2DP_SUPPORT) && RTK_BT_PROFILE_A2DP_SUPPORT
static inline void fBTA2DP(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_a2dp_cmd(argc - 1, &argv[1]);
	}
}
#endif /* RTK_BT_PROFILE_A2DP_SUPPORT */

#if defined(RTK_BT_PROFILE_AVRCP_SUPPORT) && RTK_BT_PROFILE_AVRCP_SUPPORT
static inline void fBTAVRCP(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_avrcp_cmd(argc - 1, &argv[1]);
	}
}
#endif /* RTK_BT_PROFILE_AVRCP_SUPPORT */

#if defined(RTK_BT_PROFILE_HFP_SUPPORT) && RTK_BT_PROFILE_HFP_SUPPORT
static inline void fBTHFP(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_hfp_cmd(argc - 1, &argv[1]);
	}
}
#endif /* RTK_BT_PROFILE_HFP_SUPPORT */

#if defined(RTK_BT_PROFILE_PBAP_SUPPORT) && RTK_BT_PROFILE_PBAP_SUPPORT
static inline void fBTPBAP(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_pbap_cmd(argc - 1, &argv[1]);
	}
}
#endif /* RTK_BT_PROFILE_PBAP_SUPPORT */

#if defined(RTK_BT_PROFILE_SPP_SUPPORT) && RTK_BT_PROFILE_SPP_SUPPORT
static inline void fBTSPP(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_spp_cmd(argc - 1, &argv[1]);
	}
}
#endif /* RTK_BT_PROFILE_SPP_SUPPORT */

#if defined(RTK_BT_PROFILE_RFC_SUPPORT) && RTK_BT_PROFILE_RFC_SUPPORT
static inline void fBTRFC(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_rfc_cmd(argc - 1, &argv[1]);
	}
}
#endif /* RTK_BT_PROFILE_RFC_SUPPORT */

#if defined(RTK_BT_PROFILE_HID_SUPPORT) && RTK_BT_PROFILE_HID_SUPPORT
static inline void fBTHID(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_hid_cmd(argc - 1, &argv[1]);
	}
}
#endif /* RTK_BT_PROFILE_HID_SUPPORT */

#if defined(RTK_BT_PROFILE_SDP_SUPPORT) && RTK_BT_PROFILE_SDP_SUPPORT
static inline void fBTSDP(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_sdp_cmd(argc - 1, &argv[1]);
	}
}
#endif /* RTK_BT_PROFILE_SDP_SUPPORT */
#endif /* RTK_BREDR_SUPPORT */

#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
static inline void fBLEISO(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_iso_cmd(argc - 1, &argv[1]);
	}
}
#endif /* RTK_BLE_ISO_SUPPORT */

#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
static inline void fBLEBAP(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_bap_cmd(argc - 1, &argv[1]);
	}
}

static inline void fBLECAP(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_cap_cmd(argc - 1, &argv[1]);
	}
}

#if defined(RTK_BLE_AUDIO_TMAP_SUPPORT) && RTK_BLE_AUDIO_TMAP_SUPPORT
static inline void fBLETMAP(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_tmap_cmd(argc - 1, &argv[1]);
	}
}
#endif /* RTK_BLE_AUDIO_TMAP_SUPPORT */

#if defined(RTK_BLE_AUDIO_GMAP_SUPPORT) && RTK_BLE_AUDIO_GMAP_SUPPORT
static inline void fBLEGMAP(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_gmap_cmd(argc - 1, &argv[1]);
	}
}
#endif /* RTK_BLE_AUDIO_GMAP_SUPPORT */
#endif /* RTK_BLE_AUDIO_SUPPORT */

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
static inline void fBLEMESHSTACK(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_stack(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHDATA(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_datatrans_model(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHCONFIG(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_config(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHGOO(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_generic_onoff(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHRMT(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_remote_prov_client_model(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHLL(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_light_lightness(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHLCTL(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_light_ctl(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHLHSL(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_light_hsl(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHLXYL(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_light_xyl(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHLLC(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_light_lc(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHTIME(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_time(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHSCHEDULER(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_scheduler(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHSCENE(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_scene(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHGDTT(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_generic_default_transition_time(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHGLE(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_generic_level(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHGPOO(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_generic_power_on_off(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHGPL(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_generic_power_level(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHGB(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_generic_battery(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHGLO(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_generic_location(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHGP(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_generic_property(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHSENSOR(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_sensor(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHHEALTH(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_health(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHDF(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_df(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHSBR(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_sbr(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHPRB(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_prb(argc - 1, &argv[1]);
	}
}

static inline void fBLEMESHDFU(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_mesh_device_firmware_update(argc - 1, &argv[1]);
	}
}
#endif /* RTK_BLE_MESH_SUPPORT */

static inline void fBTGAP(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_gap(argc - 1, &argv[1]);
	}
}

static void fBTVENDOR(void *arg)
{
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (bt_parse_param(arg, &argc, argv)) {
		atcmd_bt_vendor(argc - 1, &argv[1]);
	}
}

ATCMD_TABLE_DATA_SECTION
const log_item_t at_bt_cmds[] = {
	{"+BTDEMO",              fBTDEMO},
#if defined(RTK_BLE_SUPPORT) && RTK_BLE_SUPPORT
	{"+BLEGAP",              fBLEGAP},
#endif /* RTK_BLE_SUPPORT */
#if defined(RTK_BLE_GATTS_SUPPORT) && RTK_BLE_GATTS_SUPPORT
	{"+BLEGATTS",            fBLEGATTS},
#endif /* RTK_BLE_GATTS_SUPPORT */
#if defined(RTK_BLE_GATTC_SUPPORT) && RTK_BLE_GATTC_SUPPORT
	{"+BLEGATTC",            fBLEGATTC},
#endif /* RTK_BLE_GATTC_SUPPORT */
#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
	{"+BRGAP",               fBRGAP},
#if defined(RTK_BT_PROFILE_A2DP_SUPPORT) && RTK_BT_PROFILE_A2DP_SUPPORT
	{"+BTA2DP",              fBTA2DP},
#endif /* RTK_BT_PROFILE_A2DP_SUPPORT */
#if defined(RTK_BT_PROFILE_AVRCP_SUPPORT) && RTK_BT_PROFILE_AVRCP_SUPPORT
	{"+BTAVRCP",             fBTAVRCP},
#endif /* RTK_BT_PROFILE_AVRCP_SUPPORT */
#if defined(RTK_BT_PROFILE_HFP_SUPPORT) && RTK_BT_PROFILE_HFP_SUPPORT
	{"+BTHFP",               fBTHFP},
#endif /* RTK_BT_PROFILE_HFP_SUPPORT */
#if defined(RTK_BT_PROFILE_PBAP_SUPPORT) && RTK_BT_PROFILE_PBAP_SUPPORT
	{"+BTPBAP",              fBTPBAP},
#endif /* RTK_BT_PROFILE_PBAP_SUPPORT */
#if defined(RTK_BT_PROFILE_SPP_SUPPORT) && RTK_BT_PROFILE_SPP_SUPPORT
	{"+BTSPP",               fBTSPP},
#endif /* RTK_BT_PROFILE_SPP_SUPPORT */
#if defined(RTK_BT_PROFILE_RFC_SUPPORT) && RTK_BT_PROFILE_RFC_SUPPORT
	{"+BTRFC",               fBTRFC},
#endif /* RTK_BT_PROFILE_RFC_SUPPORT */
#if defined(RTK_BT_PROFILE_HID_SUPPORT) && RTK_BT_PROFILE_HID_SUPPORT
	{"+BTHID",               fBTHID},
#endif /* RTK_BT_PROFILE_HID_SUPPORT */
#if defined(RTK_BT_PROFILE_SDP_SUPPORT) && RTK_BT_PROFILE_SDP_SUPPORT
	{"+BTSDP",               fBTSDP},
#endif /* RTK_BT_PROFILE_SDP_SUPPORT */
#endif /* RTK_BREDR_SUPPORT */
#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
	{"+BLEISO",              fBLEISO},
#endif /* RTK_BLE_ISO_SUPPORT */
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
	{"+BLEBAP",              fBLEBAP},
	{"+BLECAP",              fBLECAP},
#if defined(RTK_BLE_AUDIO_TMAP_SUPPORT) && RTK_BLE_AUDIO_TMAP_SUPPORT
	{"+BLETMAP",             fBLETMAP},
#endif /* RTK_BLE_AUDIO_TMAP_SUPPORT */
#if defined(RTK_BLE_AUDIO_GMAP_SUPPORT) && RTK_BLE_AUDIO_GMAP_SUPPORT
	{"+BLEGMAP",             fBLEGMAP},
#endif /* RTK_BLE_AUDIO_GMAP_SUPPORT */
#endif /* RTK_BLE_AUDIO_SUPPORT */
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	{"+BLEMESHSTACK",       fBLEMESHSTACK},
	{"+BLEMESHDATA",        fBLEMESHDATA},
	{"+BLEMESHCONFIG",      fBLEMESHCONFIG},
	{"+BLEMESHGOO",         fBLEMESHGOO},
	{"+BLEMESHRMT",         fBLEMESHRMT},
	{"+BLEMESHLL",          fBLEMESHLL},
	{"+BLEMESHLCTL",        fBLEMESHLCTL},
	{"+BLEMESHLHSL",        fBLEMESHLHSL},
	{"+BLEMESHLXYL",        fBLEMESHLXYL},
	{"+BLEMESHLLC",         fBLEMESHLLC},
	{"+BLEMESHTIME",        fBLEMESHTIME},
	{"+BLEMESHSCHEDULER",   fBLEMESHSCHEDULER},
	{"+BLEMESHSCENE",       fBLEMESHSCENE},
	{"+BLEMESHGDTT",        fBLEMESHGDTT},
	{"+BLEMESHGLE",         fBLEMESHGLE},
	{"+BLEMESHGPOO",        fBLEMESHGPOO},
	{"+BLEMESHGPL",         fBLEMESHGPL},
	{"+BLEMESHGB",          fBLEMESHGB},
	{"+BLEMESHGLO",         fBLEMESHGLO},
	{"+BLEMESHGP",          fBLEMESHGP},
	{"+BLEMESHSENSOR",      fBLEMESHSENSOR},
	{"+BLEMESHHEALTH",      fBLEMESHHEALTH},
	{"+BLEMESHDF",          fBLEMESHDF},
	{"+BLEMESHSBR",         fBLEMESHSBR},
	{"+BLEMESHPRB",         fBLEMESHPRB},
	{"+BLEMESHDFU",         fBLEMESHDFU},
#endif /* RTK_BLE_MESH_SUPPORT */
	{"+BTGAP",              fBTGAP},
	{"+BTVENDOR",           fBTVENDOR},
};

/* BT atcmd as a part of AT command "AT+LIST". */
void print_bt_ext_at(void)
{
#if (defined(CONFIG_MP_SHRINK) && CONFIG_MP_SHRINK) || \
    (defined(CONFIG_BT_EXCLUDE_AT_COMMAND) && CONFIG_BT_EXCLUDE_AT_COMMAND) || \
    (defined(CONFIG_BT_INIC) && CONFIG_BT_INIC)
	//Print nothing
#else
#if defined(CONFIG_ATCMD_HOST_CONTROL) && CONFIG_ATCMD_HOST_CONTROL
	at_printf("AT+BTDEMO\r\n");
	at_printf("AT+BLEGAP\r\n");
	at_printf("AT+BLEGATTS\r\n");
	at_printf("AT+BLEGATTC\r\n");
#else
	int index;
	int num = 0;

	num = sizeof(at_bt_cmds) / sizeof(at_bt_cmds[0]);
	for (index = 0; index < num; index++) {
		if (0 != strcmp("+BTVENDOR", at_bt_cmds[index].log_cmd)) {
			at_printf("AT%s\r\n", at_bt_cmds[index].log_cmd);
		}
	}
#endif /* CONFIG_ATCMD_HOST_CONTROL */
#endif
}

void at_bt_init(void)
{
#if (defined(CONFIG_MP_SHRINK) && CONFIG_MP_SHRINK) || \
    (defined(CONFIG_BT_EXCLUDE_AT_COMMAND) && CONFIG_BT_EXCLUDE_AT_COMMAND) || \
    (defined(CONFIG_BT_INIC) && CONFIG_BT_INIC)
	(void)at_bt_cmds;
#else
	// int i = 0, size = sizeof(at_bt_cmds) / sizeof(at_bt_cmds[0]);

	// while (i < size) {
	// 	atcmd_service_add_table(&(at_bt_cmds[i].item), 1);
	// 	i++;
	// }
#endif
}