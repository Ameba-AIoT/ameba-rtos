/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "atcmd_service.h"
#include "atcmd_bt_impl.h"
#include "bt_utils.h"
#include "bt_api_config.h"
struct bt_at_cmd_table {
	log_item_t item;
	cmd_func_t cmd_func;
};

#define CMD_NAME_BT_DEMO         "+BTDEMO"
#if defined(RTK_BLE_SUPPORT) && RTK_BLE_SUPPORT
#define CMD_NAME_BLE_GAP         "+BLEGAP"
#endif /* RTK_BLE_SUPPORT */
#if defined(RTK_BLE_GATTS_SUPPORT) && RTK_BLE_GATTS_SUPPORT
#define CMD_NAME_GATTS           "+BLEGATTS"
#endif /* RTK_BLE_GATTS_SUPPORT */
#if defined(RTK_BLE_GATTC_SUPPORT) && RTK_BLE_GATTC_SUPPORT
#define CMD_NAME_GATTC           "+BLEGATTC"
#endif /* RTK_BLE_GATTC_SUPPORT */
#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
#define CMD_NAME_BR_GAP          "+BRGAP"
#if defined(RTK_BT_PROFILE_A2DP_SUPPORT) && RTK_BT_PROFILE_A2DP_SUPPORT
#define CMD_NAME_A2DP            "+BTA2DP"
#endif /* RTK_BT_PROFILE_A2DP_SUPPORT */
#if defined(RTK_BT_PROFILE_AVRCP_SUPPORT) && RTK_BT_PROFILE_AVRCP_SUPPORT
#define CMD_NAME_AVRCP           "+BTAVRCP"
#endif /* RTK_BT_PROFILE_AVRCP_SUPPORT */
#if defined(RTK_BT_PROFILE_HFP_SUPPORT) && RTK_BT_PROFILE_HFP_SUPPORT
#define CMD_NAME_HFP             "+BTHFP"
#endif /* RTK_BT_PROFILE_HFP_SUPPORT */
#if defined(RTK_BT_PROFILE_PBAP_SUPPORT) && RTK_BT_PROFILE_PBAP_SUPPORT
#define CMD_NAME_PBAP            "+BTPBAP"
#endif /* RTK_BT_PROFILE_PBAP_SUPPORT */
#if defined(RTK_BT_PROFILE_SPP_SUPPORT) && RTK_BT_PROFILE_SPP_SUPPORT
#define CMD_NAME_SPP             "+BTSPP"
#endif /* RTK_BT_PROFILE_SPP_SUPPORT */
#if defined(RTK_BT_PROFILE_RFC_SUPPORT) && RTK_BT_PROFILE_RFC_SUPPORT
#define CMD_NAME_RFC             "+BTRFC"
#endif /* RTK_BT_PROFILE_RFC_SUPPORT */
#if defined(RTK_BT_PROFILE_HID_SUPPORT) && RTK_BT_PROFILE_HID_SUPPORT
#define CMD_NAME_HID             "+BTHID"
#endif /* RTK_BT_PROFILE_HID_SUPPORT */
#if defined(RTK_BT_PROFILE_SDP_SUPPORT) && RTK_BT_PROFILE_SDP_SUPPORT
#define CMD_NAME_SDP             "+BTSDP"
#endif /* RTK_BT_PROFILE_SDP_SUPPORT */
#endif /* RTK_BREDR_SUPPORT */
#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
#define CMD_NAME_ISO             "+BLEISO"
#endif /* RTK_BLE_ISO_SUPPORT */
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
#define CMD_NAME_BAP             "+BLEBAP"
#define CMD_NAME_CAP             "+BLECAP"
#if defined(RTK_BLE_AUDIO_TMAP_SUPPORT) && RTK_BLE_AUDIO_TMAP_SUPPORT
#define CMD_NAME_TMAP            "+BLETMAP"
#endif /* RTK_BLE_AUDIO_TMAP_SUPPORT */
#if defined(RTK_BLE_AUDIO_GMAP_SUPPORT) && RTK_BLE_AUDIO_GMAP_SUPPORT
#define CMD_NAME_GMAP            "+BLEGMAP"
#endif /* RTK_BLE_AUDIO_GMAP_SUPPORT */
#endif /* RTK_BLE_AUDIO_SUPPORT */
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
#define CMD_NAME_GAP             "+BTGAP"
#define CMD_NAME_BT_VENDOR       "+BTVENDOR"
#if defined(CONFIG_BT_SDN_VERIFY) && CONFIG_BT_SDN_VERIFY
#define CMD_NAME_BT_SDN_VERIFY   "+BTSDN"
#endif /* CONFIG_BT_SDN_VERIFY */

static void atcmd_bt_cmd(void *arg, char *cmd_name);
static void fBTDEMO(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_BT_DEMO);
}

#if defined(RTK_BLE_SUPPORT) && RTK_BLE_SUPPORT
static inline void fBLEGAP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_BLE_GAP);
}
#endif /* RTK_BLE_SUPPORT */

#if defined(RTK_BLE_GATTS_SUPPORT) && RTK_BLE_GATTS_SUPPORT
static inline void fBLEGATTS(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_GATTS);
}
#endif /* RTK_BLE_GATTS_SUPPORT */

#if defined(RTK_BLE_GATTC_SUPPORT) && RTK_BLE_GATTC_SUPPORT
static inline void fBLEGATTC(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_GATTC);
}
#endif /* RTK_BLE_GATTC_SUPPORT */

#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
static inline void fBRGAP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_BR_GAP);
}

#if defined(RTK_BT_PROFILE_A2DP_SUPPORT) && RTK_BT_PROFILE_A2DP_SUPPORT
static inline void fBTA2DP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_A2DP);
}
#endif /* RTK_BT_PROFILE_A2DP_SUPPORT */

#if defined(RTK_BT_PROFILE_AVRCP_SUPPORT) && RTK_BT_PROFILE_AVRCP_SUPPORT
static inline void fBTAVRCP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_AVRCP);
}
#endif /* RTK_BT_PROFILE_AVRCP_SUPPORT */

#if defined(RTK_BT_PROFILE_HFP_SUPPORT) && RTK_BT_PROFILE_HFP_SUPPORT
static inline void fBTHFP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_HFP);
}
#endif /* RTK_BT_PROFILE_HFP_SUPPORT */

#if defined(RTK_BT_PROFILE_PBAP_SUPPORT) && RTK_BT_PROFILE_PBAP_SUPPORT
static inline void fBTPBAP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_PBAP);
}
#endif /* RTK_BT_PROFILE_PBAP_SUPPORT */

#if defined(RTK_BT_PROFILE_SPP_SUPPORT) && RTK_BT_PROFILE_SPP_SUPPORT
static inline void fBTSPP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_SPP);
}
#endif /* RTK_BT_PROFILE_SPP_SUPPORT */

#if defined(RTK_BT_PROFILE_RFC_SUPPORT) && RTK_BT_PROFILE_RFC_SUPPORT
static inline void fBTRFC(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_RFC);
}
#endif /* RTK_BT_PROFILE_RFC_SUPPORT */

#if defined(RTK_BT_PROFILE_HID_SUPPORT) && RTK_BT_PROFILE_HID_SUPPORT
static inline void fBTHID(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_HID);
}
#endif /* RTK_BT_PROFILE_HID_SUPPORT */

#if defined(RTK_BT_PROFILE_SDP_SUPPORT) && RTK_BT_PROFILE_SDP_SUPPORT
static inline void fBTSDP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_SDP);
}
#endif /* RTK_BT_PROFILE_SDP_SUPPORT */
#endif /* RTK_BREDR_SUPPORT */

#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
static inline void fBLEISO(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_ISO);
}
#endif /* RTK_BLE_ISO_SUPPORT */

#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
static inline void fBLEBAP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_BAP);
}

static inline void fBLECAP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_CAP);
}

#if defined(RTK_BLE_AUDIO_TMAP_SUPPORT) && RTK_BLE_AUDIO_TMAP_SUPPORT
static inline void fBLETMAP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_TMAP);
}
#endif /* RTK_BLE_AUDIO_TMAP_SUPPORT */

#if defined(RTK_BLE_AUDIO_GMAP_SUPPORT) && RTK_BLE_AUDIO_GMAP_SUPPORT
static inline void fBLEGMAP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_GMAP);
}
#endif /* RTK_BLE_AUDIO_GMAP_SUPPORT */
#endif /* RTK_BLE_AUDIO_SUPPORT */

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
static inline void fBLEMESHSTACK(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_STACK);
}

static inline void fBLEMESHDATA(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_DATA);
}

static inline void fBLEMESHCONFIG(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_CONFIG);
}

static inline void fBLEMESHGOO(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_GOO);
}

static inline void fBLEMESHRMT(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_RMT);
}

static inline void fBLEMESHLL(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_LL);
}

static inline void fBLEMESHLCTL(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_LCTL);
}

static inline void fBLEMESHLHSL(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_LHSL);
}

static inline void fBLEMESHLXYL(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_LXYL);
}

static inline void fBLEMESHLLC(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_LLC);
}

static inline void fBLEMESHTIME(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_TIME);
}

static inline void fBLEMESHSCHEDULER(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_SCHEDULER);
}

static inline void fBLEMESHSCENE(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_SCENE);
}

static inline void fBLEMESHGDTT(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_GDTT);
}

static inline void fBLEMESHGLE(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_GLE);
}

static inline void fBLEMESHGPOO(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_GPOO);
}

static inline void fBLEMESHGPL(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_GPL);
}

static inline void fBLEMESHGB(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_GB);
}

static inline void fBLEMESHGLO(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_GLO);
}

static inline void fBLEMESHGP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_GP);
}

static inline void fBLEMESHSENSOR(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_SENSOR);
}

static inline void fBLEMESHHEALTH(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_HEALTH);
}

static inline void fBLEMESHDF(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_DF);
}

static inline void fBLEMESHSBR(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_SBR);
}

static inline void fBLEMESHPRB(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_PRB);
}

static inline void fBLEMESHDFU(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_MESH_DFU);
}
#endif /* RTK_BLE_MESH_SUPPORT */

static inline void fBTGAP(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_GAP);
}

static void fBTVENDOR(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_BT_VENDOR);
}

#if defined(CONFIG_BT_SDN_VERIFY) && CONFIG_BT_SDN_VERIFY
static inline void fBTSDNVERIFY(void *arg)
{
	atcmd_bt_cmd(arg, CMD_NAME_BT_SDN_VERIFY);
}
#endif /* CONFIG_BT_SDN_VERIFY */

static struct bt_at_cmd_table at_bt_cmds[] = {
	{{CMD_NAME_BT_DEMO,          fBTDEMO,              {NULL, NULL}},	atcmd_bt_example},
#if defined(RTK_BLE_SUPPORT) && RTK_BLE_SUPPORT
	{{CMD_NAME_BLE_GAP,          fBLEGAP,              {NULL, NULL}},	atcmd_bt_le_gap},
#endif /* RTK_BLE_SUPPORT */
#if defined(RTK_BLE_GATTS_SUPPORT) && RTK_BLE_GATTS_SUPPORT
	{{CMD_NAME_GATTS,            fBLEGATTS,            {NULL, NULL}},	atcmd_bt_gatts},
#endif /* RTK_BLE_GATTS_SUPPORT */
#if defined(RTK_BLE_GATTC_SUPPORT) && RTK_BLE_GATTC_SUPPORT
	{{CMD_NAME_GATTC,            fBLEGATTC,            {NULL, NULL}},	atcmd_bt_gattc},
#endif /* RTK_BLE_GATTC_SUPPORT */
#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
	{{CMD_NAME_BR_GAP,           fBRGAP,               {NULL, NULL}},	atcmd_bt_br_gap},
#if defined(RTK_BT_PROFILE_A2DP_SUPPORT) && RTK_BT_PROFILE_A2DP_SUPPORT
	{{CMD_NAME_A2DP,             fBTA2DP,              {NULL, NULL}},	atcmd_bt_a2dp_cmd},
#endif /* RTK_BT_PROFILE_A2DP_SUPPORT */
#if defined(RTK_BT_PROFILE_AVRCP_SUPPORT) && RTK_BT_PROFILE_AVRCP_SUPPORT
	{{CMD_NAME_AVRCP,            fBTAVRCP,             {NULL, NULL}},	atcmd_bt_avrcp_cmd},
#endif /* RTK_BT_PROFILE_AVRCP_SUPPORT */
#if defined(RTK_BT_PROFILE_HFP_SUPPORT) && RTK_BT_PROFILE_HFP_SUPPORT
	{{CMD_NAME_HFP,              fBTHFP,               {NULL, NULL}},	atcmd_bt_hfp_cmd},
#endif /* RTK_BT_PROFILE_HFP_SUPPORT */
#if defined(RTK_BT_PROFILE_PBAP_SUPPORT) && RTK_BT_PROFILE_PBAP_SUPPORT
	{{CMD_NAME_PBAP,             fBTPBAP,              {NULL, NULL}},	atcmd_bt_pbap_cmd},
#endif /* RTK_BT_PROFILE_PBAP_SUPPORT */
#if defined(RTK_BT_PROFILE_SPP_SUPPORT) && RTK_BT_PROFILE_SPP_SUPPORT
	{{CMD_NAME_SPP,              fBTSPP,               {NULL, NULL}},	atcmd_bt_spp_cmd},
#endif /* RTK_BT_PROFILE_SPP_SUPPORT */
#if defined(RTK_BT_PROFILE_RFC_SUPPORT) && RTK_BT_PROFILE_RFC_SUPPORT
	{{CMD_NAME_RFC,              fBTRFC,               {NULL, NULL}},	atcmd_bt_rfc_cmd},
#endif /* RTK_BT_PROFILE_RFC_SUPPORT */
#if defined(RTK_BT_PROFILE_HID_SUPPORT) && RTK_BT_PROFILE_HID_SUPPORT
	{{CMD_NAME_HID,              fBTHID,               {NULL, NULL}},	atcmd_bt_hid_cmd},
#endif /* RTK_BT_PROFILE_HID_SUPPORT */
#if defined(RTK_BT_PROFILE_SDP_SUPPORT) && RTK_BT_PROFILE_SDP_SUPPORT
	{{CMD_NAME_SDP,              fBTSDP,               {NULL, NULL}},	atcmd_bt_sdp_cmd},
#endif /* RTK_BT_PROFILE_SDP_SUPPORT */
#endif /* RTK_BREDR_SUPPORT */
#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
	{{CMD_NAME_ISO,              fBLEISO,              {NULL, NULL}},	atcmd_bt_iso_cmd},
#endif /* RTK_BLE_ISO_SUPPORT */
#if defined(RTK_BLE_AUDIO_SUPPORT) && RTK_BLE_AUDIO_SUPPORT
	{{CMD_NAME_BAP,              fBLEBAP,              {NULL, NULL}},	atcmd_bt_bap_cmd},
	{{CMD_NAME_CAP,              fBLECAP,              {NULL, NULL}},	atcmd_bt_cap_cmd},
#if defined(RTK_BLE_AUDIO_TMAP_SUPPORT) && RTK_BLE_AUDIO_TMAP_SUPPORT
	{{CMD_NAME_TMAP,             fBLETMAP,             {NULL, NULL}},	atcmd_bt_tmap_cmd},
#endif /* RTK_BLE_AUDIO_TMAP_SUPPORT */
#if defined(RTK_BLE_AUDIO_GMAP_SUPPORT) && RTK_BLE_AUDIO_GMAP_SUPPORT
	{{CMD_NAME_GMAP,             fBLEGMAP,             {NULL, NULL}},	atcmd_bt_gmap_cmd},
#endif /* RTK_BLE_AUDIO_GMAP_SUPPORT */
#endif /* RTK_BLE_AUDIO_SUPPORT */
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	{{CMD_NAME_MESH_STACK,       fBLEMESHSTACK,        {NULL, NULL}},	atcmd_bt_mesh_stack},
	{{CMD_NAME_MESH_DATA,        fBLEMESHDATA,         {NULL, NULL}},	atcmd_bt_mesh_datatrans_model},
	{{CMD_NAME_MESH_CONFIG,      fBLEMESHCONFIG,       {NULL, NULL}},	atcmd_bt_mesh_config},
	{{CMD_NAME_MESH_GOO,         fBLEMESHGOO,          {NULL, NULL}},	atcmd_bt_mesh_generic_onoff},
	{{CMD_NAME_MESH_RMT,         fBLEMESHRMT,          {NULL, NULL}},	atcmd_bt_mesh_remote_prov_client_model},
	{{CMD_NAME_MESH_LL,          fBLEMESHLL,           {NULL, NULL}},	atcmd_bt_mesh_light_lightness},
	{{CMD_NAME_MESH_LCTL,        fBLEMESHLCTL,         {NULL, NULL}},	atcmd_bt_mesh_light_ctl},
	{{CMD_NAME_MESH_LHSL,        fBLEMESHLHSL,         {NULL, NULL}},	atcmd_bt_mesh_light_hsl},
	{{CMD_NAME_MESH_LXYL,        fBLEMESHLXYL,         {NULL, NULL}},	atcmd_bt_mesh_light_xyl},
	{{CMD_NAME_MESH_LLC,         fBLEMESHLLC,          {NULL, NULL}},	atcmd_bt_mesh_light_lc},
	{{CMD_NAME_MESH_TIME,        fBLEMESHTIME,         {NULL, NULL}},	atcmd_bt_mesh_time},
	{{CMD_NAME_MESH_SCHEDULER,   fBLEMESHSCHEDULER,    {NULL, NULL}},	atcmd_bt_mesh_scheduler},
	{{CMD_NAME_MESH_SCENE,       fBLEMESHSCENE,        {NULL, NULL}},	atcmd_bt_mesh_scene},
	{{CMD_NAME_MESH_GDTT,        fBLEMESHGDTT,         {NULL, NULL}},	atcmd_bt_mesh_generic_default_transition_time},
	{{CMD_NAME_MESH_GLE,         fBLEMESHGLE,          {NULL, NULL}},	atcmd_bt_mesh_generic_level},
	{{CMD_NAME_MESH_GPOO,        fBLEMESHGPOO,         {NULL, NULL}},	atcmd_bt_mesh_generic_power_on_off},
	{{CMD_NAME_MESH_GPL,         fBLEMESHGPL,          {NULL, NULL}},	atcmd_bt_mesh_generic_power_level},
	{{CMD_NAME_MESH_GB,          fBLEMESHGB,           {NULL, NULL}},	atcmd_bt_mesh_generic_battery},
	{{CMD_NAME_MESH_GLO,         fBLEMESHGLO,          {NULL, NULL}},	atcmd_bt_mesh_generic_location},
	{{CMD_NAME_MESH_GP,          fBLEMESHGP,           {NULL, NULL}},	atcmd_bt_mesh_generic_property},
	{{CMD_NAME_MESH_SENSOR,      fBLEMESHSENSOR,       {NULL, NULL}},	atcmd_bt_mesh_sensor},
	{{CMD_NAME_MESH_HEALTH,      fBLEMESHHEALTH,       {NULL, NULL}},	atcmd_bt_mesh_health},
	{{CMD_NAME_MESH_DF,          fBLEMESHDF,           {NULL, NULL}},	atcmd_bt_mesh_df},
	{{CMD_NAME_MESH_SBR,         fBLEMESHSBR,          {NULL, NULL}},	atcmd_bt_mesh_sbr},
	{{CMD_NAME_MESH_PRB,         fBLEMESHPRB,          {NULL, NULL}},	atcmd_bt_mesh_prb},
	{{CMD_NAME_MESH_DFU,         fBLEMESHDFU,          {NULL, NULL}},	atcmd_bt_mesh_device_firmware_update},
#endif /* RTK_BLE_MESH_SUPPORT */
	{{CMD_NAME_GAP,              fBTGAP,               {NULL, NULL}},	atcmd_bt_gap},
	{{CMD_NAME_BT_VENDOR,        fBTVENDOR,            {NULL, NULL}},	atcmd_bt_vendor},
#if defined(CONFIG_BT_SDN_VERIFY) && CONFIG_BT_SDN_VERIFY
	{{CMD_NAME_BT_SDN_VERIFY,    fBTSDNVERIFY,         {NULL, NULL}},	atcmd_bt_sdn_verify_cmd},
#endif /* CONFIG_BT_SDN_VERIFY */
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
		if (0 != strcmp(CMD_NAME_BT_VENDOR, at_bt_cmds[index].item.log_cmd)) {
			at_printf("AT%s\r\n", at_bt_cmds[index].item.log_cmd);
		}
	}
#endif /* CONFIG_ATCMD_HOST_CONTROL */
#endif
}

static void atcmd_bt_cmd(void *arg, char *cmd_name)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	int i = 0, size = sizeof(at_bt_cmds) / sizeof(at_bt_cmds[0]);
	int ret = 0;

	if (!arg) {
		BT_LOGE("[AT%s] Error: No input args number!\r\n", cmd_name);
		goto exit;
	}

	argc = parse_param(arg, argv);
	if (argc < 2) {
		BT_LOGE("[AT%s] Error: Wrong input args number!\r\n", cmd_name);
		goto exit;
	}

	while (i < size) {
		if (0 == strcmp(cmd_name, at_bt_cmds[i].item.log_cmd)) {
			ret = at_bt_cmds[i].cmd_func(argc - 1, &argv[1]);
			break;
		}
		i++;
	}

	if (ret == 0) {
		BT_AT_PRINTOK();
	} else {
		BT_AT_PRINTERROR(ret);
	}
	return;

exit:
	BT_LOGA("[AT%s] Info: Use AT+BTCMDHELP to help\r\n");
	BT_AT_PRINTERROR(BT_AT_ERR_PARAM_INVALID);
}

void at_bt_init(void)
{
#if (defined(CONFIG_MP_SHRINK) && CONFIG_MP_SHRINK) || \
    (defined(CONFIG_BT_EXCLUDE_AT_COMMAND) && CONFIG_BT_EXCLUDE_AT_COMMAND) || \
    (defined(CONFIG_BT_INIC) && CONFIG_BT_INIC)
	(void)at_bt_cmds;
#else
	int i = 0, size = sizeof(at_bt_cmds) / sizeof(at_bt_cmds[0]);

	while (i < size) {
		atcmd_service_add_table(&(at_bt_cmds[i].item), 1);
		i++;
	}
#endif
}