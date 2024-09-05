/*
*******************************************************************************
* Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <osif.h>

#include "platform_autoconf.h"
#include <atcmd_service.h>
#include <bt_utils.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_sdp.h>
#include <atcmd_bt_impl.h>
#include <rtk_bt_br_gap.h>

static int atcmd_bt_sdp_discov(int argc, char **argv)
{
	(void)argc;
	char addr_str[30] = {0};
	uint8_t bd_addr[RTK_BD_ADDR_LEN] = {0};

	hexdata_str_to_bd_addr(argv[0], bd_addr, RTK_BD_ADDR_LEN);
	if (rtk_bt_sdp_discov_start(bd_addr)) {
		BT_LOGE("SDP discov fail\r\n");
		return -1;
	}
	rtk_bt_br_addr_to_str(bd_addr, addr_str, sizeof(addr_str));
	BT_LOGA("SDP discovering %s ...\r\n", addr_str);

	return 0;
}

static const cmd_table_t sdp_cmd_table[] = {
	{"discov",     atcmd_bt_sdp_discov,          2, 2},
	{NULL,},
};

int atcmd_bt_sdp_cmd(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, sdp_cmd_table, "[AT+BTSDP]");
}