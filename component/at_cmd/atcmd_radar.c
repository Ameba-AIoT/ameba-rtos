/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"
#ifdef CONFIG_WIFI_RADAR_ENABLE

#include "atcmd_service.h"
#include "atcmd_wifi.h"
#include "wifi_intf_drv_to_upper.h"
#include "wifi_radar.h"
#ifdef CONFIG_WHC_HOST
#ifdef CONFIG_WHC_INTF_IPC
#include "whc_ipc_host_api.h"
#else
#include "whc_host_api.h"
#endif
#endif

static void at_rad_help(void)
{
	RTK_LOGI(NOTAG, "\r\n");
	RTK_LOGI(NOTAG, "AT+RAD=[<type>,<value>,<type>,<value>......]\r\n");
	RTK_LOGI(NOTAG, "\t<type>:\tA string as \"mode\",\"channel\",\"chrip_bw\",\"trig_period\",\"enable\"\r\n");
	RTK_LOGI(NOTAG, "\t<mode>:\t0-single;1-normal, mandatory\r\n");
	RTK_LOGI(NOTAG, "\t<channel>:\tFMCW center freq channel (recommended: 7)\r\n");
	RTK_LOGI(NOTAG, "\t<chrip_bw>:\t0-70M;1-40M;2-20M\r\n");
	RTK_LOGI(NOTAG, "\t<trig_period>:\tFMCW interval in ms (recommended: 15)\r\n");
	RTK_LOGI(NOTAG, "\t<enable>:\t0-disable;1-enable, mandatory\r\n");
}

/****************************************************************
AT command process:
	AT+RAD
	AT Command:
	Config and Enable Radar.
	[+RAD]:OK
****************************************************************/
void at_rad(u16 argc, char **argv)
{
	int ret = 0, i = 0, j = 0;
	int error_no = RTW_AT_OK;
	struct rtw_radar_action_parm act_param = {
		.mode        = RTW_RADAR_NORMAL_MODE,
		.channel     = 7,
		.chrip_bw    = 1,   /* 40M */
		.trig_period = 15,  /* 15ms */
	};

	if (argc == 1) {
		RTK_LOGW(NOTAG, "[+RAD] The parameters can not be ignored\r\n");
		error_no = RTW_AT_ERR_REQUIRED_PARAM_MISS;
		goto end;
	}

	if ((argc < 3) || (argc > 11)) {
		RTK_LOGW(NOTAG, "[+RAD] The parameters format ERROR\r\n");
		error_no = RTW_AT_ERR_PARAM_NUM_ERR;
		goto end;
	}

	/* The parameters appear by pairs, so i += 2. */
	for (i = 1; argc > i; i += 2) {
		j = i + 1;  /* next i. */

		if (0 == strcmp("mode", argv[i])) {
			if ((argc > j) && (strlen(argv[j]) != 0)) {
				act_param.mode = (u8)atoi(argv[j]);
			}
		} else if (0 == strcmp("channel", argv[i])) {
			if ((argc > j) && (strlen(argv[j]) != 0)) {
				act_param.channel = (u8)atoi(argv[j]);
			}
		} else if (0 == strcmp("chrip_bw", argv[i])) {
			if ((argc > j) && (strlen(argv[j]) != 0)) {
				act_param.chrip_bw = (u8)atoi(argv[j]);
			}
		} else if (0 == strcmp("trig_period", argv[i])) {
			if ((argc > j) && (strlen(argv[j]) != 0)) {
				act_param.trig_period = (u8)atoi(argv[j]);
			}
		} else if (0 == strcmp("enable", argv[i])) {
			if ((argc > j) && (strlen(argv[j]) != 0)) {
				act_param.enable = (u8)atoi(argv[j]);
			}
		} else {
			RTK_LOGW(NOTAG, "[+RAD] Invalid parameter type\r\n");
			error_no = RTW_AT_ERR_INVALID_PARAM_VALUE;
			goto end;
		}
	}

	RTK_LOGI(NOTAG, "radar act params: mode = %d [0-single;1-normal]\r\n", act_param.mode);
	RTK_LOGI(NOTAG, "radar act params: channel = %d\r\n", act_param.channel);
	RTK_LOGI(NOTAG, "radar act params: chrip_bw = %d [0-70M;1-40M;2-20M]\r\n", act_param.chrip_bw);
	RTK_LOGI(NOTAG, "radar act params: trig_period = %d [unit:ms]\r\n", act_param.trig_period);
	RTK_LOGI(NOTAG, "radar act params: enable = %d [0-dis;1-en]\r\n", act_param.enable);

	if (act_param.enable) {
		act_param.act = RTW_RADAR_ACT_CFG;
		ret = wifi_radar_config(&act_param);
		if (ret == RTK_SUCCESS) {
			act_param.act = RTW_RADAR_ACT_EN;
			ret = wifi_radar_config(&act_param);
		}
	} else {
		act_param.act = RTW_RADAR_ACT_EN;
		ret = wifi_radar_config(&act_param);
	}

	if (ret != RTK_SUCCESS) {
		error_no = RTW_AT_ERR_UNKNOWN_ERR;
	}
end:
	if (error_no == RTW_AT_OK) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		if (error_no >= RTW_AT_ERR_REQUIRED_PARAM_MISS && error_no <= RTW_AT_ERR_PARAM_NUM_ERR) {
			at_rad_help();
		}
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

/****************************************************************
AT command process:
	AT+RADDBG
	AT Command:
	Radar debug parameter setter.
	AP params set directly; NP params forwarded via iwpriv.
	[+RADDBG]:OK
****************************************************************/
void at_raddbg(u16 argc, char **argv)
{
	char buf[64] = {0};
	int error_no = RTW_AT_OK;
	int ret = 0;
	u32 pos = 0;
	int i;

	if (argc < 2 || argv[1] == NULL) {
		RTK_LOGW(NOTAG, "[RADDBG] Usage: AT+RADDBG=COMMAND[,PARAMETERS]\r\n");
		error_no = RTW_AT_ERR_REQUIRED_PARAM_MISS;
		goto end;
	}

	/* AP-side params: handled directly without going through NP */
	if (wifi_radar_dbg(argc - 1, &argv[1]) == RTK_SUCCESS) {
		goto end;
	}

	/* NP-side params: forward via iwpriv */
	for (i = 1; i < argc && pos < sizeof(buf) - 1; i++) {
		int len = strlen(argv[i]);
		if (pos > 0) {
			buf[pos++] = ' ';
		}
		strncpy(buf + pos, argv[i], sizeof(buf) - pos - 1);
		pos += len;
	}
	buf[sizeof(buf) - 1] = '\0';

#ifdef CONFIG_WHC_HOST
	ret = whc_host_api_iwpriv_command(buf, strlen(buf) + 1, 1);
#else
	ret = rtw_iwpriv_command(STA_WLAN_INDEX, buf, 1);
#endif
	if (ret != RTK_SUCCESS) {
		RTK_LOGW(NOTAG, "[RADDBG] Failed while iwpriv\r\n");
		error_no = RTW_AT_ERR_UNKNOWN_ERR;
	}

end:
	if (error_no == RTW_AT_OK) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

/* Weak stub: overridden by example/wifi/wifi_radar/atcmd_wifi_radar.c when that example is built. */
__weak void at_radarstart(u16 argc, char **argv)
{
	(void)argc;
	(void)argv;
	at_printf(ATCMD_ERROR_END_STR, RTW_AT_ERR_UNKNOWN_ERR);
}

ATCMD_APONLY_TABLE_DATA_SECTION
const log_item_t at_rad_items[] = {
	{"+RAD",    at_rad},
	{"+RADDBG", at_raddbg},
	{"+RADARSTART", at_radarstart},
};

#endif /* CONFIG_WIFI_RADAR_ENABLE */
