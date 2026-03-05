/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"
#if defined(CONFIG_BT) && CONFIG_BT

#include "atcmd_service.h"
#include "atcmd_bt_impl.h"
#include "bt_utils.h"
#include "bt_api_config.h"

#if (defined(CONFIG_MP_SHRINK) && CONFIG_MP_SHRINK) || \
    (defined(CONFIG_BT_EXCLUDE_AT_COMMAND) && CONFIG_BT_EXCLUDE_AT_COMMAND) || \
    (defined(CONFIG_BT_INIC) && CONFIG_BT_INIC)
//No BT atcmd
#else
ATCMD_APONLY_TABLE_DATA_SECTION
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
#endif

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

#endif /* CONFIG_BT */