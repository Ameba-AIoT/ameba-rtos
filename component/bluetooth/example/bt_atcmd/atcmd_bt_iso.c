/*
*******************************************************************************
* Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include "platform_autoconf.h"
#include <bt_api_config.h>
#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <atcmd_service.h>
#include <bt_utils.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_iso.h>
#include <rtk_bt_le_iso_def.h>
#include <atcmd_bt_impl.h>
#include <rtk_bt_le_gap.h>

static int atcmd_bt_le_iso_create_cis_by_cig(int argc, char **argv)
{
	(void)argc;
	uint8_t cis_id;
	uint8_t cig_id;
	uint16_t conn_handle;
	cis_id = (uint8_t)str_to_int(argv[0]);
	cig_id = (uint8_t)str_to_int(argv[1]);
	conn_handle = (uint16_t)str_to_int(argv[2]);

	if (rtk_bt_le_iso_cig_initiator_create_cis_by_cig_id(cis_id, cig_id, conn_handle)) {
		BT_LOGE("LEISO cis create fail\r\n");
		return -1;
	}
	BT_LOGA("LEISO cis create successfully\r\n");

	return 0;
}

static int atcmd_bt_le_iso_create_cis_by_handle(int argc, char **argv)
{
	(void)argc;
	uint8_t cis_id;
	uint8_t cig_id;
	uint16_t conn_handle;
	uint16_t cis_conn_handle;
	cis_id = (uint8_t)str_to_int(argv[0]);
	cig_id = (uint8_t)str_to_int(argv[1]);
	conn_handle = (uint16_t)str_to_int(argv[2]);
	cis_conn_handle = (uint16_t)str_to_int(argv[3]);

	if (rtk_bt_le_iso_cig_initiator_create_cis_by_cis_conn_handle(cis_id, cig_id, 1, conn_handle, &cis_conn_handle)) {
		BT_LOGE("LEISO cis create fail\r\n");
		return -1;
	}
	BT_LOGA("LEISO cis create successfully cis connection handle is 0x%x\r\n", cis_conn_handle);

	return 0;
}

static int atcmd_bt_le_iso_disconnect(int argc, char **argv)
{
	(void)argc;
	uint16_t cis_conn_handle = 0;
	cis_conn_handle = (uint16_t)str_to_int(argv[0]);

	if (rtk_bt_le_iso_cig_disconnect(cis_conn_handle)) {
		BT_LOGE("LEISO cis iso cig disconnect with 0x%x failed\r\n", cis_conn_handle);
		return -1;
	}
	BT_LOGA("LEISO cis iso cig disconnect with 0x%x successfully\r\n", cis_conn_handle);

	return 0;
}

static int atcmd_bt_le_iso_cis_read_tx_sync(int argc, char **argv)
{
	(void)argc;
	uint16_t cis_conn_handle = (uint16_t)str_to_int(argv[0]);
	rtk_bt_le_iso_cig_read_iso_tx_sync_info_t tx_sync_info = {0};

	if (rtk_bt_le_iso_cig_read_iso_tx_sync(cis_conn_handle, &tx_sync_info)) {
		BT_LOGE("LEISO cis read tx sync fail\r\n");
		return -1;
	}
	BT_LOGA("LEISO cis read tx sync successfully\r\n");
	BT_LOGA("cis_conn_handle 0x%x, conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, packet_sequence_number 0x%x, time_stamp 0x%x, time_offset 0x%x\r\n",
			cis_conn_handle, tx_sync_info.conn_handle, tx_sync_info.cig_id, tx_sync_info.cis_id,
			tx_sync_info.packet_sequence_number, (unsigned int)tx_sync_info.time_stamp, (unsigned int)tx_sync_info.time_offset);

	return 0;
}

static int atcmd_bt_le_iso_cis_read_link_quality(int argc, char **argv)
{
	(void)argc;
	uint16_t cis_conn_handle = (uint16_t)str_to_int(argv[0]);
	rtk_bt_le_iso_cig_read_link_quality_info_t link_quality_info = {0};

	if (rtk_bt_le_iso_cig_read_link_quality(cis_conn_handle, &link_quality_info)) {
		BT_LOGE("LEISO cis read link quality fail\r\n");
		return -1;
	}
	BT_LOGA("LEISO cis read link quality successfully\r\n");
	BT_LOGA("[APP] cis_conn_handle 0x%x, conn_handle 0x%x, cig_id 0x%x, cis_id 0x%x, tx_unacked_packets 0x%x, tx_flushed_packets 0x%x, tx_last_subevent_packets 0x%x,retransmitted_packets 0x%x, crc_error_packets %d, rx_unreceived_packets 0x%x, duplicate_packets 0x%x\r\n",
			cis_conn_handle, link_quality_info.conn_handle, link_quality_info.cig_id, link_quality_info.cis_id,
			(unsigned int)link_quality_info.tx_unacked_packets, (unsigned int)link_quality_info.tx_flushed_packets,
			(unsigned int)link_quality_info.tx_last_subevent_packets,
			(unsigned int)link_quality_info.retransmitted_packets, (unsigned int)link_quality_info.crc_error_packets, (unsigned int)link_quality_info.rx_unreceived_packets,
			(unsigned int)link_quality_info.duplicate_packets);

	return 0;
}

static int atcmd_bt_le_iso_bis_read_tx_sync(int argc, char **argv)
{
	(void)argc;
	uint16_t bis_conn_handle = (uint16_t)str_to_int(argv[0]);
	rtk_bt_le_iso_big_broadcaster_read_tx_sync_info_t tx_sync_info = {0};

	if (rtk_bt_le_iso_big_broadcaster_read_tx_sync(bis_conn_handle, &tx_sync_info)) {
		BT_LOGE("LEISO bis read tx sync fail\r\n");
		return -1;
	}
	BT_LOGA("LEISO bis read tx sync successfully\r\n");
	BT_LOGA("bis_conn_handle 0x%x, big_handle 0x%x, adv_handle 0x%x, packet_sequence_number 0x%x, time_stamp 0x%x, time_offset 0x%x\r\n",
			bis_conn_handle, tx_sync_info.big_handle, tx_sync_info.adv_handle, tx_sync_info.packet_sequence_number,
			(unsigned int)tx_sync_info.time_stamp, (unsigned int)tx_sync_info.time_offset);

	return 0;
}

rtk_bt_le_iso_big_receiver_create_sync_param_t def_bis_receiver_create_sync_param = {
	.encryption         = 0,
	.broadcast_code     = {0x82, 0x31, 0x5C, 0x28, 0xF6, 0x1B, 0x19, 0x58, 0x2D, 0x5E, 0x20, 0xD1, 0x23, 0x33, 0xD0, 0x8E},
	/* Default value of TSPX_broadcast_code in PTS 8.0.0 is 8ED03323D1205E2D58191BF6285C3182 */
	.mse                = 0,
	.big_sync_timeout   = 0x2ee0,   /*timeout = N*10 ms = 120s*/
	.num_bis            = RTK_BLE_BIS_MAX_NUM,
	.bis                = {}
};

static int atcmd_bt_le_bis_create_sync(int argc, char **argv)
{
	(void)argc;
	uint16_t sync_handle = (uint16_t)str_to_int(argv[0]);
	uint8_t big_handle;

	if (rtk_bt_le_iso_big_receiver_create_sync(sync_handle, &def_bis_receiver_create_sync_param, &big_handle)) {
		BT_LOGE("LEISO bis create sync fail\r\n");
		return -1;
	}
	BT_LOGA("LEISO bis create sync successfully, big handle is 0x%x\r\n", big_handle);

	return 0;
}

static int atcmd_bt_le_bis_terminate_sync(int argc, char **argv)
{
	(void)argc;
	rtk_bt_le_iso_conn_handle_info_t info = {0};
	uint8_t big_handle = (uint8_t)str_to_int(argv[0]);

	/* 1. remove path */
	if (rtk_bt_le_iso_get_iso_conn_handle(RTK_BLE_ISO_ROLE_BIS_RECEIVER, &info)) {
		BT_LOGE("LEISO get iso conn handle fail\r\n");
		return -1;
	}
	for (uint8_t i = 0; i < info.iso_handle_num; i ++) {
		rtk_bt_le_iso_big_remove_path(info.iso_handle[i], RTK_BLE_ISO_DATA_PATH_OUTPUT_FLAG);
	}
	/* 2 terminate sync */
	if (rtk_bt_le_iso_big_receiver_terminate_sync(big_handle)) {
		BT_LOGE("LEISO bis terminate sync fail\r\n");
		return -1;
	}
	BT_LOGA("LEISO bis terminate sync successfully\r\n");

	return 0;
}

static const cmd_table_t iso_cis_initiator_cmd_table[] = {
	{"create_cis_by_cig", atcmd_bt_le_iso_create_cis_by_cig,             4, 4},
	{"create_cis_by_hdl", atcmd_bt_le_iso_create_cis_by_handle,          5, 5},
	{"read_tx_sync",      atcmd_bt_le_iso_cis_read_tx_sync,              2, 2},
	{"disconnect",         atcmd_bt_le_iso_disconnect,                   2, 2},
	{"read_link_quality", atcmd_bt_le_iso_cis_read_link_quality,         2, 2},
	{NULL,},
};

static const cmd_table_t iso_cis_acceptor_cmd_table[] = {
	{"disconnect",         atcmd_bt_le_iso_disconnect,                   2, 2},
	{"read_tx_sync",      atcmd_bt_le_iso_cis_read_tx_sync,              2, 2},
	{"read_link_quality", atcmd_bt_le_iso_cis_read_link_quality,         2, 2},
	{NULL,},
};

static const cmd_table_t iso_bis_broadcaster_cmd_table[] = {
	{"read_tx_sync",      atcmd_bt_le_iso_bis_read_tx_sync,              2, 2},
	{NULL,},
};

static const cmd_table_t iso_bis_receiver_cmd_table[] = {
	{"create_sync",       atcmd_bt_le_bis_create_sync,                   2, 2},
	{"terminate_sync",    atcmd_bt_le_bis_terminate_sync,                2, 2},
	{"read_link_quality", atcmd_bt_le_iso_cis_read_link_quality,         2, 2},
	{NULL,},
};

int atcmd_bt_iso_cmd(int argc, char *argv[])
{
	int ret = 0;
	char tag[80] = "[AT+BLEISO]";
	if (strcmp(argv[0], "bis") == 0) {
		if (strcmp(argv[1], "broadcaster") == 0) {
			BT_LOGA("Set iso bis broadcaster\r\n");
			strcat(tag, "[bis][broadcaster]");
			ret = atcmd_bt_excute(argc - 2, &argv[2], iso_bis_broadcaster_cmd_table, tag);
		} else if (strcmp(argv[1], "receiver") == 0) {
			BT_LOGA("Set iso bis receiver\r\n");
			strcat(tag, "[bis][receiver]");
			ret = atcmd_bt_excute(argc - 2, &argv[2], iso_bis_receiver_cmd_table, tag);
		} else {
			BT_LOGE("[%s]Error: iso bis has no role %s\r\n", __func__, argv[1]);
			ret = -1;
		}
	} else if (strcmp(argv[0], "cis") == 0) {
		if (strcmp(argv[1], "initiator") == 0) {
			BT_LOGA("Set iso cis initiator\r\n");
			strcat(tag, "[cis][initiator]");
			ret = atcmd_bt_excute(argc - 2, &argv[2], iso_cis_initiator_cmd_table, tag);
		} else if (strcmp(argv[1], "acceptor") == 0) {
			BT_LOGA("Set iso cis acceptor\r\n");
			strcat(tag, "[cis][acceptor]");
			ret = atcmd_bt_excute(argc - 2, &argv[2], iso_cis_acceptor_cmd_table, tag);
		} else {
			BT_LOGE("[%s]Error: iso cis has no role %s\r\n", __func__, argv[1]);
			ret = -1;
		}
	} else {
		BT_LOGE("[%s]Error: do not support %s\r\n", __func__, argv[0]);
		ret = -1;
	}
	return ret;
}
#endif