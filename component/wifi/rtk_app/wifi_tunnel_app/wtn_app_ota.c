/**
  ******************************************************************************
  * @file    wtn_app_socket.c
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#define __WTN_APP_OTA_C__

/* -------------------------------- Includes -------------------------------- */
#include "rtw_autoconf.h"
#ifdef CONFIG_RMESH_OTA_EN
#include "os_wrapper.h"
#include "lwip_netconf.h"
#include "ameba_soc.h"
#include "ameba_ota.h"
#include "dlist.h"
#include "rtw_queue.h"
#include <lwip/sockets.h>
#include "log.h"
#include "wifi_api.h"
#include "wifi_api_wtn.h"
#include "flash_api.h"
#include "sys_api.h"
#include "kv.h"
#include "wtn_app_socket.h"
#include "wtn_app_ota.h"
#include "wtn_app_rnat.h"

#define _TO_DS_		BIT(8)
#define _FROM_DS_	BIT(9)
#define GetToDs(pbuf)	(((*(unsigned short *)(void*)(pbuf)) & le16_to_cpu(_TO_DS_)) != 0)
#define GetFrDs(pbuf)	(((*(unsigned short *)(void*)(pbuf)) & le16_to_cpu(_FROM_DS_)) != 0)
#define RMESHGetFrameSubType(pbuf)	(cpu_to_le16(*(unsigned short *)(void*)(pbuf)) & (BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | BIT(2)))

#define RMESH_OTA_GET_BITS(data, bits)        ( (((uint8_t *)(data))[(bits) >> 0x3]) & ( 1 << ((bits) & 0x7)) )
#define RMESH_OTA_SET_BITS(data, bits)        do { (((uint8_t *)(data))[(bits) >> 0x3]) |= ( 1 << ((bits) & 0x7)); } while(0);

struct rmesh_ota_node {
	u8 mac[ETH_ALEN];
	u8 layer;
	u8 ota_transfer_finished;
};

struct rmesh_ota_priv {
	u8 raw_pkt_iv_len;
	u8 raw_pkt_rm_len; /*remove len for ICV & MIC*/
	u8 ota_reset_recvd;
	u8 http_ota_reset_delayed;
	u8 wait_inter_ota;
	u8 ota_type; /*receive from server*/
	u8 ota_method; /*internal use, http ota or inter ota*/
	u8 self_layer;
	u8 father_is_rnat;
	u8 http_ota_fail_reset;

	u8 slave_num;
	u8 child_num;
	u8 wait_num;

	u32 ota_start_time;
	ota_context *http_ota_ctx;

	rtos_sema_t 	slave_node_list_sema;
	rtos_sema_t 	wait_node_list_sema;
	rtos_timer_t req_wait_timer;
	rtos_timer_t reset_wait_timer;
	rtos_timer_t update_wait_timer;

	u8 father_mac[ETH_ALEN];
	u8 self_mac[ETH_ALEN];
	u8 root_mac[ETH_ALEN];
	u8 ap_bssid[ETH_ALEN];

	u8 ongoing_ota_ver[RMESH_OTA_VER_MAX_LEN + 1];

	struct rmesh_ota_node *child_list[RMESH_MAX_NODE_NUM];
	struct rmesh_ota_node *slave_list[RMESH_MAX_NODE_NUM];
	struct rmesh_ota_node *wait_list[RMESH_MAX_NODE_NUM];
};

struct rmesh_raw_additional_hdr {
	u8 type;
	u16 len;
};

struct rmesh_ota_upgrade_status {
	u8	cur_round_num;	/* current round number*/
	u16 status;			/* upgrade status */
	u16 packet_num;		/* upgrade packet number */
	u16 image_id;		/* upgrade image id */
	u32 checksum;		/* upgrade image checksum */
	u32 image_addr;		/* upgrade image address */
	u32 total_size;		/* upgrade firmware total size */
	u32 written_size;	/* upgrade firmware size has been written into flash */
	u8 progress_index;	/* upgrade process bitmap */
	u8 *progress_array; /* upgrade process bitmap */
} __attribute__((packed));

struct rmesh_ota_cb_recv_data {
	u8 type;
	u32 data_len;      /* receive data length */
	u8 mac[ETH_ALEN];  /* receive src mac address */
	u8 dst_mac[ETH_ALEN]; /*receive data dst mac address*/
	u8 *data;          /* receive data body */
} __attribute__((packed));

struct ota_packet_head {
	u32 seq;    /* packet sequence */
} __attribute__((packed));

struct rmesh_inter_ota_result {
	u32 unfinished_num; /* upgrade total device number */
	u32 successed_num;  /* upgrade successed device number */
};

static const char *const TAG = "rmesh_ota";

static rtos_queue_t g_rmesh_ota_request_q = NULL;
static rtos_queue_t g_rmesh_ota_recv_q;
static struct rmesh_ota_upgrade_status *g_rmesh_ota_status = NULL;
static flash_t flash_obj;
struct rmesh_ota_priv *g_rmesh_ota_priv = NULL;
const u8 RMESH_OTA_PATTERN[] = {0x52, 0x4D, 0x45, 0x53, 0x48, 0x4F, 0x54, 0x41};
struct rmesh_http_ota_param ota_param;

extern u8 wtn_rnat_ap_start;

static void rmesh_ota_dump_buf(char *info, u8 *buf, int len)
{
	int i;

	RTK_LOGI(TAG, "%s\n", info);
	for (i = 0; i < len; i++) {
		DiagPrintf("%s%02X%s", i % 16 == 0 ? "\r\n\t" : " ", buf[i], i == len - 1 ? "\r\n" : "");
	}
}

static void rmesh_ota_slave_node_add(u8 *slave_mac)
{
	/*when recv ota update req, add this node to slave list*/
	u8 i = 0;
	struct rmesh_ota_node *pnode;
	struct rmesh_ota_node *pnode_new;

	/* create new node */
	pnode_new = (struct rmesh_ota_node *)rtos_mem_zmalloc(sizeof(struct rmesh_ota_node));
	if (pnode_new == NULL) {
		RTK_LOGE(TAG, "%s, node malloc failed!\n", __func__);
		return;
	}

	rtos_sema_take(g_rmesh_ota_priv->slave_node_list_sema, 0xFFFFFFFF);
	for (i = 0; i < RMESH_MAX_NODE_NUM; i++) {
		pnode = g_rmesh_ota_priv->slave_list[i];
		if (pnode != NULL) {
			if (!memcmp(pnode->mac, slave_mac, ETH_ALEN)) {/*node already existed*/
				rtos_mem_free(pnode_new);
				rtos_sema_give(g_rmesh_ota_priv->slave_node_list_sema);
				return;
			}
		} else if (pnode == NULL) {
			break;
		}
	}

	if (i == RMESH_MAX_NODE_NUM) {
		RTK_LOGE(TAG, "add slave fail due to exceed max num\n");
	} else {
		g_rmesh_ota_priv->slave_list[i] = pnode_new;
		memcpy(pnode_new->mac, slave_mac, ETH_ALEN);
		g_rmesh_ota_priv->slave_num++;
	}

	rtos_sema_give(g_rmesh_ota_priv->slave_node_list_sema);
}

static void rmesh_ota_slave_node_free(void)
{
	u8 i = 0;
	struct rmesh_ota_node *pnode;

	rtos_sema_take(g_rmesh_ota_priv->slave_node_list_sema, 0xFFFFFFFF);
	for (i = 0; i < RMESH_MAX_NODE_NUM; i++) {
		pnode = g_rmesh_ota_priv->slave_list[i];
		if (pnode != NULL) {
			rtos_mem_free(pnode);
			g_rmesh_ota_priv->slave_num--;
			g_rmesh_ota_priv->slave_list[i] = NULL;
		}
	}
	rtos_sema_give(g_rmesh_ota_priv->slave_node_list_sema);
}

static void rmesh_ota_child_node_free(void)
{
	u8 i = 0;

	if (g_rmesh_ota_priv->child_num) {
		for (i = 0; i < RMESH_MAX_NODE_NUM; i++) {
			if (g_rmesh_ota_priv->child_list[i] != NULL) {
				rtos_mem_free(g_rmesh_ota_priv->child_list[i]);
				g_rmesh_ota_priv->child_num--;
				g_rmesh_ota_priv->child_list[i] = NULL;
			}
		}
	}
}
static void rmesh_ota_wait_node_add(u8 *slave_mac)
{
	u8 i = 0;
	struct rmesh_ota_node *pnode;
	struct rmesh_ota_node *pnode_new;

	/* create new node */
	pnode_new = (struct rmesh_ota_node *)rtos_mem_zmalloc(sizeof(struct rmesh_ota_node));
	if (pnode_new == NULL) {
		RTK_LOGE(TAG, "%s, node malloc failed!\n", __func__);
		return;
	}

	rtos_sema_take(g_rmesh_ota_priv->wait_node_list_sema, 0xFFFFFFFF);
	for (i = 0; i < RMESH_MAX_NODE_NUM; i++) {
		pnode = g_rmesh_ota_priv->wait_list[i];
		if (pnode != NULL) {
			if (!memcmp(pnode->mac, slave_mac, ETH_ALEN)) {/*node already existed*/
				rtos_mem_free(pnode_new);
				rtos_sema_give(g_rmesh_ota_priv->wait_node_list_sema);
				return;
			}
		} else if (pnode == NULL) {
			break;
		}
	}

	if (i == RMESH_MAX_NODE_NUM) {
		RTK_LOGE(TAG, "add slave fail due to exceed max num\n");
	} else {
		g_rmesh_ota_priv->wait_list[i] = pnode_new;
		memcpy(pnode_new->mac, slave_mac, ETH_ALEN);
		g_rmesh_ota_priv->wait_num++;
	}

	rtos_sema_give(g_rmesh_ota_priv->wait_node_list_sema);
}

static void rmesh_ota_wait_list_to_slave_list(void)
{
	u8 i = 0;
	struct rmesh_ota_node *pnode;

	/*move node from wait list to slave list*/
	rtos_sema_take(g_rmesh_ota_priv->wait_node_list_sema, 0xFFFFFFFF);
	for (i = 0; i < RMESH_MAX_NODE_NUM; i++) {
		pnode = g_rmesh_ota_priv->wait_list[i];
		if (pnode != NULL) {
			rmesh_ota_slave_node_add(pnode->mac);
			rtos_mem_free(pnode);
			g_rmesh_ota_priv->wait_num--;
			g_rmesh_ota_priv->wait_list[i] = NULL;
		}
	}
	rtos_sema_give(g_rmesh_ota_priv->wait_node_list_sema);
}

static void rmesh_ota_prepare_part1(void)
{
	struct rtw_wifi_setting setting = {0};
	struct rtw_rmesh_node_info node = {0};

	/*get iv len for rmesh ota raw pkt to use*/
	wifi_get_setting(0, &setting);
	if (setting.security_type == RTW_SECURITY_OPEN) {
		g_rmesh_ota_priv->raw_pkt_iv_len = 0;
	} else if (setting.security_type & WEP_ENABLED) {
		g_rmesh_ota_priv->raw_pkt_iv_len = 4;
		g_rmesh_ota_priv->raw_pkt_rm_len = 4;
	} else if (setting.security_type & AES_ENABLED) {
		g_rmesh_ota_priv->raw_pkt_iv_len = 8;
		g_rmesh_ota_priv->raw_pkt_rm_len = 8;
	} else if (setting.security_type & TKIP_ENABLED) {
		g_rmesh_ota_priv->raw_pkt_iv_len = 8;
		g_rmesh_ota_priv->raw_pkt_rm_len = 8 + 4;
	}
	memcpy(g_rmesh_ota_priv->ap_bssid, setting.bssid, ETH_ALEN);

	g_rmesh_ota_priv->ota_reset_recvd = 0;
	g_rmesh_ota_priv->http_ota_reset_delayed = 0;
	g_rmesh_ota_priv->wait_inter_ota = 0;

	/*get self mac*/
	wifi_rmesh_get_node_info(RMESH_SELF_NODE, &node);
	memcpy(g_rmesh_ota_priv->self_mac, node.mac, ETH_ALEN);
}

static void rmesh_ota_prepare_part2(void)
{
	struct rtw_rmesh_node_info node = {0};
	u8 i = 0;
	struct rtw_rmesh_node_info *child_info_list = NULL;
	struct rtw_rmesh_node_info *child_info = NULL;
	struct rmesh_ota_node *child_node = NULL;
	u8 valid_child_num = 0;

	wifi_rmesh_get_node_info(RMESH_SELF_NODE, &node);
	g_rmesh_ota_priv->self_layer = node.layer;

	wifi_rmesh_get_node_info(RMESH_FATHER_NODE, &node);
	memcpy(g_rmesh_ota_priv->father_mac, node.mac, ETH_ALEN);
	g_rmesh_ota_priv->father_is_rnat = node.is_rnat;

	wifi_rmesh_get_node_info(RMESH_ROOT_NODE, &node);
	memcpy(g_rmesh_ota_priv->root_mac, node.mac, ETH_ALEN);

	/*get child list, free old child list first*/
	rmesh_ota_child_node_free();
	g_rmesh_ota_priv->child_num = wifi_rmesh_get_child_num();
	if (g_rmesh_ota_priv->child_num) {
		child_info_list = (struct rtw_rmesh_node_info *)rtos_mem_zmalloc(g_rmesh_ota_priv->child_num * sizeof(struct rtw_rmesh_node_info));
		if (child_info_list == NULL) {
			g_rmesh_ota_priv->child_num = 0;
			return;
		}
		wifi_rmesh_get_child_info_list(&g_rmesh_ota_priv->child_num, child_info_list);
		for (i = 0; i < g_rmesh_ota_priv->child_num; i++) {
			child_info = &child_info_list[i];
			if (g_rmesh_ota_priv->ota_type != RMESH_ALL_NODE_OTA || (child_info->layer == g_rmesh_ota_priv->self_layer + 1)) {
				child_node = (struct rmesh_ota_node *)rtos_mem_zmalloc(sizeof(struct rmesh_ota_node));
				if (child_node) {
					memcpy(child_node->mac, child_info->mac, ETH_ALEN);
					child_node->layer = child_info->layer;
					g_rmesh_ota_priv->child_list[valid_child_num] = child_node;
					if (g_rmesh_ota_priv->ota_type == RMESH_ALL_NODE_OTA) {
						rmesh_ota_slave_node_add(child_info->mac);
					}
					RTK_LOGI(TAG, "child mac:%x\n", g_rmesh_ota_priv->child_list[valid_child_num]->mac[5]);
					valid_child_num++;
				}
			}
		}

		rtos_mem_free(child_info_list);
	}
}

static void rmesh_ota_firmware_download_end(u8 need_clear_sig)
{
	u8 empty_sig[8] = {0x0};
	u32 ota_total_time = SYSTIMER_GetPassTick(g_rmesh_ota_priv->ota_start_time);
	struct rmesh_ota_info_to_flash ota_info = {0};

	RTK_LOGA(TAG, "start reset\n");

	rt_kv_get("rmesh_ota_info", (u8 *)&ota_info, sizeof(struct rmesh_ota_info_to_flash));
	ota_info.ota_time = (u16)((ota_total_time * 1000 >> 15) / 1000);
	RTK_LOGI(TAG, "==========ota process total time: %ds\n", ota_info.ota_time);

	rt_kv_set("rmesh_ota_info", (u8 *)&ota_info, sizeof(struct rmesh_ota_info_to_flash));

	if (need_clear_sig) {/*update to new img*/
		sys_clear_ota_signature();
	} else {
		/*need clear new written sig when ota failed, otherwise when target ota is OTA1 and partial ota data already written, reset will hardfault*/
		FLASH_WriteStream(g_rmesh_ota_status->image_addr, 8, (u8 *)empty_sig);
	}

	if (wifi_user_config.wtn_connect_only_to_rnat) {
		wifi_disconnect();/*disconnect to rnat before reset, otherwise rnat may still think this client is alive and can help num not updated*/
	}
	rtos_time_delay_ms(500);
	sys_reset();
}

static int rmesh_ota_pkt_send(u16 type, u8 *target_mac, u8 *data, u16 data_len, u32 fix_rate)
{
	u8 bcmc[ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	struct rmesh_raw_additional_hdr additional_hdr = {0};
	struct rtw_raw_frame_desc desc = {0};
	u8 *pframe;
	u8 *payload;
	u8 i = 0;
	u8 j = 0;
	u8 current_slave_num = 0;
	u8 wlan_hdr_len = 26;
	u16 pframe_len;
	int ret = RTK_FAIL;

	if (type == RMESH_OTA_DATA) {
		wlan_hdr_len += g_rmesh_ota_priv->raw_pkt_iv_len;
	}

	pframe_len = wlan_hdr_len + RMESH_OTA_PATTERN_LEN + sizeof(struct rmesh_raw_additional_hdr) + data_len;

	if ((g_rmesh_ota_priv->ota_type != RMESH_ALL_NODE_OTA) && (type == RMESH_OTA_DATA)) {
		for (i = 0; i < g_rmesh_ota_priv->slave_num; i++) {
			if (g_rmesh_ota_priv->slave_list[i]->ota_transfer_finished == 0) {
				current_slave_num++;
			}
		}
		pframe_len += current_slave_num * 6 + 1;
	}
	pframe = (u8 *)rtos_mem_zmalloc(pframe_len);
	if (!pframe) {
		RTK_LOGE(TAG, "rmesh ota txbuf malloc failed\n");
		return ret;
	}

	additional_hdr.type = type;
	memcpy(pframe + wlan_hdr_len, RMESH_OTA_PATTERN, RMESH_OTA_PATTERN_LEN);
	pframe[0] = 0x88;/*qos data*/
	if (type == RMESH_OTA_DATA) {
		pframe[1] = g_rmesh_ota_priv->raw_pkt_iv_len > 0 ? 0x42 : 0x02; /*fromds=1, privacy depends on AP security*/
		memcpy(pframe + 4, bcmc, ETH_ALEN); /*A1=bcmc*/
		if (wifi_user_config.wtn_rnat_en) {
			memcpy(pframe + 10, g_rmesh_ota_priv->self_mac, ETH_ALEN); /*A1=BSSID(softap mac)*/
		} else {
			memcpy(pframe + 10, g_rmesh_ota_priv->ap_bssid, ETH_ALEN); /*A1=AP BSSID*/
		}
		memcpy(pframe + 16, g_rmesh_ota_priv->self_mac, ETH_ALEN); /*A3=self mac*/
	} else if (type == RMESH_OTA_UPDATE_REQ) {
		pframe[1] = 0x01; /*tods=1, no privacy*/
		memcpy(pframe + 4, g_rmesh_ota_priv->father_mac, ETH_ALEN); /*A1=father mac*/
		memcpy(pframe + 10, g_rmesh_ota_priv->self_mac, ETH_ALEN); /*A2=self mac*/
		memcpy(pframe + 16, target_mac, ETH_ALEN); /*A3=targer mac*/
	} else if (type == RMESH_OTA_STATUS_REQ) {
		pframe[1] = 0x02; /*fromds=1, no privacy*/
		memcpy(pframe + 4, target_mac, ETH_ALEN); /*A1=target mac*/
		memcpy(pframe + 10, g_rmesh_ota_priv->self_mac, ETH_ALEN); /*A2=self mac*/
		memcpy(pframe + 16, g_rmesh_ota_priv->self_mac, ETH_ALEN); /*A3=self mac*/
	} else if (type == RMESH_OTA_STATUS_RSP) {
		pframe[1] = 0x01; /*tods=1, no privacy*/
		memcpy(pframe + 4, g_rmesh_ota_priv->father_mac, ETH_ALEN); /*A1=father mac*/
		memcpy(pframe + 10, g_rmesh_ota_priv->self_mac, ETH_ALEN); /*A2=self mac*/
		memcpy(pframe + 16, target_mac, ETH_ALEN); /*A3=targer mac*/
	} else {
		rtos_mem_free(pframe);
		return RTK_FAIL;
	}

	if (type != RMESH_OTA_DATA) {
		additional_hdr.len = data_len;
		memcpy(pframe + wlan_hdr_len + RMESH_OTA_PATTERN_LEN, &additional_hdr, sizeof(struct rmesh_raw_additional_hdr));
		memcpy(pframe + wlan_hdr_len + RMESH_OTA_PATTERN_LEN + sizeof(struct rmesh_raw_additional_hdr), data, data_len);
	} else {
		additional_hdr.len = data_len;
		if (current_slave_num) {
			additional_hdr.len += 1 + current_slave_num * ETH_ALEN;
			memcpy(pframe + wlan_hdr_len + RMESH_OTA_PATTERN_LEN + sizeof(struct rmesh_raw_additional_hdr), &current_slave_num, 1);
			for (i = 0; i < g_rmesh_ota_priv->slave_num; i++) {
				if (g_rmesh_ota_priv->slave_list[i]->ota_transfer_finished == 0) {
					payload = pframe + wlan_hdr_len + RMESH_OTA_PATTERN_LEN + sizeof(struct rmesh_raw_additional_hdr) + 1 + j * 6;
					memcpy(payload, g_rmesh_ota_priv->slave_list[i]->mac, ETH_ALEN);
					j++;
				}
			}
		}
		memcpy(pframe + wlan_hdr_len + RMESH_OTA_PATTERN_LEN, &additional_hdr, sizeof(struct rmesh_raw_additional_hdr));

		if (current_slave_num) {
			memcpy(pframe + wlan_hdr_len + RMESH_OTA_PATTERN_LEN + sizeof(struct rmesh_raw_additional_hdr) + 1 + current_slave_num * 6, data, data_len);
		} else {
			memcpy(pframe + wlan_hdr_len + RMESH_OTA_PATTERN_LEN + sizeof(struct rmesh_raw_additional_hdr), data, data_len);
		}

	}

	if ((wifi_user_config.wtn_rnat_en) && (type == RMESH_OTA_DATA)) {
		desc.wlan_idx = 1;
	} else {
		desc.wlan_idx = 0;
	}

	desc.buf = pframe;
	desc.buf_len = pframe_len;
	if (type != RMESH_OTA_DATA) {
		desc.retry_limit = 12;
	}

	if (fix_rate) {
		desc.tx_rate = fix_rate;
	}
	ret = wifi_send_raw_frame(&desc);
	rtos_mem_free(pframe);
	return ret;
}

static void	rmesh_ota_status_process(u8 *progress_array, struct rmesh_ota_upgrade_status *status, struct rmesh_inter_ota_result *result)
{
	u8 i = 0;
	struct rmesh_ota_upgrade_status *recv_status = NULL;
	u8 *src_mac = NULL;
	u8 recv_cnt = 0;
	u8 trycnt = 10;
	u8 check_num = 0;
	u32 start_tick;
	int remaining_time = 0;
	struct rmesh_ota_node **node_list = NULL;
	struct rmesh_ota_node *node = NULL;

	check_num = g_rmesh_ota_priv->slave_num;
	node_list = g_rmesh_ota_priv->slave_list;
	for (i = 0; i < check_num; i++) {
		node = node_list[i];
		if (node->ota_transfer_finished == 0) {
			trycnt = 10;
			RTK_LOGI(TAG, "=====SEND STS REQ to:%x\n", node->mac[5]);
			while (rmesh_ota_pkt_send(RMESH_OTA_STATUS_REQ, node->mac, (u8 *)status, sizeof(struct rmesh_ota_upgrade_status), 0) != RTK_SUCCESS) {
				GPIO_WriteBit(_PB_19, 1);
				GPIO_WriteBit(_PB_19, 0);
				trycnt--;
				rtos_time_delay_ms(2);
				if (trycnt == 0) {
					RTK_LOGE(TAG, "%s, send and try fail\n", __func__);
					break;
				}
			}
		}
	}

	start_tick = rtos_time_get_current_system_time_ms();
	while (recv_cnt < result->unfinished_num) {
		remaining_time =  RMESH_OTA_ROUND_INTERVAL - (rtos_time_get_current_system_time_ms() - start_tick);
		if (remaining_time <= 0) {
			break;
		}
		if (RTK_SUCCESS == rtos_queue_receive(g_rmesh_ota_request_q, &recv_status, remaining_time)) {
			RTK_LOGI(TAG, "recv ota req response, written_size: %d, progress_index: %d\n", recv_status->written_size, recv_status->progress_index);
			if (recv_status->written_size == status->total_size) {
				src_mac = (u8 *)recv_status + sizeof(struct rmesh_ota_upgrade_status) + RMESH_OTA_PROGRESS_MAX;
				for (i = 0; i < check_num; i++) {
					if (!memcmp(node_list[i]->mac, src_mac, ETH_ALEN)) {
						if (node_list[i]->ota_transfer_finished == 0) {
							result->unfinished_num--;
							result->successed_num++;
							node_list[i]->ota_transfer_finished = 1;
							break;
						}
					}
				}
				RTK_LOGI(TAG, "finished node:%x, unfinish num:%d\n", src_mac[5], result->unfinished_num);
			} else {
				if (recv_status->written_size == 0) {
					memset(progress_array, 0x0, status->packet_num / 8 + 1);
				} else {
					recv_status->progress_array = (u8 *)recv_status + sizeof(struct rmesh_ota_upgrade_status);
					for (int i = 0; i < RMESH_OTA_PROGRESS_MAX
						 && (recv_status->progress_index * RMESH_OTA_PROGRESS_MAX  + i) * 8 < status->packet_num; i++) {
						progress_array[recv_status->progress_index * RMESH_OTA_PROGRESS_MAX  + i] &= recv_status->progress_array[i];
					}
				}
			}
			rtos_mem_free(recv_status);
			recv_cnt++;
		} else {
			break;
		}
	}
}

void rmesh_ota_reset_wait_timer(void *arg)
{
	(void) arg;
	u8 clear_sig = 1;

	RTK_LOGI(TAG, "reset due to reset timer TO\n");

	if (g_rmesh_ota_priv->slave_num && (g_rmesh_ota_priv->ota_method != RMESH_HTTP_OTA) && (g_rmesh_ota_priv->wait_inter_ota == 0)) {
		g_rmesh_ota_priv->ota_reset_recvd =
			1; /*if inter ota success but missing reset cmd, need set it here, otherwise will not reset after send firmware to slave done*/
		return;
	}

	if (g_rmesh_ota_priv->ota_method == RMESH_HTTP_OTA) {
		clear_sig = (g_rmesh_ota_priv->http_ota_fail_reset == 0) ? 1 : 0;
	} else if (g_rmesh_ota_priv->ota_method == RMESH_INTER_OTA) {
		clear_sig = (g_rmesh_ota_priv->wait_inter_ota == 0) ? 1 : 0;
	}

	rmesh_ota_firmware_download_end(clear_sig);
}

static void rmesh_ota_firmware_send(struct rmesh_ota_upgrade_status *status, struct rmesh_inter_ota_result *result)
{
	//u8 fail_cnt = 0;
	u32 fix_rate = 0;
	u8 *progress_array = (u8 *)rtos_mem_malloc(status->packet_num / 8 + 1);
	if (!progress_array) {
		RTK_LOGE(TAG, "%s, malloc failed\n", __func__);
		return;
	}
	u16 data_len = RMESH_OTA_PACKET_SIZE;
	u8 *data = (u8 *)rtos_mem_zmalloc(data_len + sizeof(struct ota_packet_head));
	if (!data) {
		rtos_mem_free(progress_array);
		RTK_LOGE(TAG, "%s, malloc failed\n", __func__);
		return;
	}
	u32 start_tick = rtos_time_get_current_system_time_ms();

	for (int round = 0; round < (RMESH_OTA_ROUND_MAX + 1) && result->unfinished_num > 0; round++) {
		memset(progress_array, 0xff, status->packet_num / 8 + 1);
		status->cur_round_num = (u8)round;
		rmesh_ota_status_process(progress_array, status, result);

		rmesh_ota_dump_buf("progress:", progress_array, status->packet_num / 8 + 1);
		if (round < RMESH_OTA_ROUND_MAX) {
			for (int seq = 0; seq < status->packet_num && result->unfinished_num > 0; seq++) {
				if (!RMESH_OTA_GET_BITS(progress_array, seq)) {
					struct ota_packet_head *hdr = (struct ota_packet_head *)data;
					hdr->seq = seq;
					data_len = (seq == status->packet_num - 1) ?
							   (status->total_size - seq * RMESH_OTA_PACKET_SIZE) : RMESH_OTA_PACKET_SIZE;
					flash_stream_read(&flash_obj, status->image_addr + seq * RMESH_OTA_PACKET_SIZE, data_len, data + sizeof(struct ota_packet_head));
					if (round < (RMESH_OTA_ROUND_MAX / 2)) {
						fix_rate = RTW_RATE_24M;
					} else {
						fix_rate = RTW_RATE_6M;
					}
					if (RTK_SUCCESS != rmesh_ota_pkt_send(RMESH_OTA_DATA, NULL, data, data_len + sizeof(struct ota_packet_head), fix_rate)) {
						GPIO_WriteBit(_PB_18, 1);
						GPIO_WriteBit(_PB_18, 0);
						rtos_time_delay_ms(2);
					}
				}
			}
		}
		RTK_LOGI(TAG, "round: %d\n", round);
	}
	RTK_LOGI(TAG, "all devices upgrade done, successed_num: %d, spend time: %d ms\n", result->successed_num, rtos_time_get_current_system_time_ms() - start_tick);
	rtos_mem_free(progress_array);
	rtos_mem_free(data);
	status->status = RMESH_OTA_STS_RESET;
	u8 i = 0;
	for (i = 0; i < g_rmesh_ota_priv->slave_num; i++) {
		if (rmesh_ota_pkt_send(RMESH_OTA_STATUS_REQ, g_rmesh_ota_priv->slave_list[i]->mac, (u8 *)status, sizeof(struct rmesh_ota_upgrade_status), 0) != RTK_SUCCESS) {
			RTK_LOGE(TAG, "%s, send rst cmd fail\n", __func__);
		}
	}

	/*free slave nodes*/
	rmesh_ota_slave_node_free();

	/*case1: get ota from father/root, already recvd reset cmd, but also need send to slaves, need reset after firmware send done*/
	/*case2: get ota from http, but also need send to slaves, need reset after firmware send done*/
	if ((g_rmesh_ota_priv->wait_num == 0) && (g_rmesh_ota_priv->ota_reset_recvd == 1 || g_rmesh_ota_priv->http_ota_reset_delayed == 1)) {
		rmesh_ota_firmware_download_end(1);
	}
}

void rmesh_ota_internal_send_task(void *param)
{
	(void) param;
	struct rmesh_ota_info_to_flash ota_info = {0};
	struct rmesh_ota_upgrade_status status;
	struct rmesh_inter_ota_result result;
	u32 firmware_size = 0;

send_again:
	memset(&status, 0, sizeof(struct rmesh_ota_upgrade_status));
	memset(&result, 0, sizeof(struct rmesh_inter_ota_result));
	rt_kv_get("rmesh_ota_info", (u8 *)&ota_info, sizeof(struct rmesh_ota_info_to_flash));
	firmware_size = ota_info.ota_firmware_size;

	status.packet_num = (firmware_size + RMESH_OTA_PACKET_SIZE - 1) / RMESH_OTA_PACKET_SIZE;
	status.total_size = firmware_size;
	status.image_addr = ota_info.image_addr;
	status.checksum = ota_info.checksum;
	status.image_id = ota_info.image_id;

	result.unfinished_num = g_rmesh_ota_priv->slave_num;

	rtos_queue_create(&g_rmesh_ota_request_q, result.unfinished_num * 2, sizeof(struct rmesh_ota_upgrade_status *));

	RTK_LOGI(TAG, "start send firmware, total_size: %d bytes, packet_num: %d\n", status.total_size, status.packet_num);
	rmesh_ota_firmware_send(&status, &result);

	rtos_queue_delete(g_rmesh_ota_request_q);
	g_rmesh_ota_request_q = NULL;

	if (g_rmesh_ota_priv->wait_num) {
		rmesh_ota_wait_list_to_slave_list();
		goto send_again;
	}
	rtos_task_delete(NULL);
}

static void rmesh_ota_status_request_cb(u8 *src_mac, u8 *data)
{
	RTK_LOGI(TAG, "recv status request from "MAC_FMT"\n", MAC_ARG(src_mac));

	struct rmesh_ota_upgrade_status *recv_status = (struct rmesh_ota_upgrade_status *)data;
	struct rmesh_ota_upgrade_status *tmp_status;
	struct rmesh_ota_info_to_flash ota_info = {0};
	static u32 start_tick = 0;
	u8 target_idx = 0;
	u8 need_clear_ota_sig = 0;
	u8 remain_node_num = 0;

	if (recv_status->status == RMESH_OTA_STS_RESET) {
		rtos_timer_stop(g_rmesh_ota_priv->reset_wait_timer, 1);

		if (g_rmesh_ota_status->written_size == g_rmesh_ota_status->total_size) {
			g_rmesh_ota_priv->wait_inter_ota = 0; /*clear after ota finished*/
		}

		remain_node_num = g_rmesh_ota_priv->slave_num;

		if (remain_node_num == 0) {
			need_clear_ota_sig = (g_rmesh_ota_status->written_size == g_rmesh_ota_status->total_size) ? 1 : 0;
			RTK_LOGI(TAG, "RX total size:%d, target firmware size:%d\n", g_rmesh_ota_status->total_size, g_rmesh_ota_status->total_size);
			rmesh_ota_firmware_download_end(need_clear_ota_sig);
		} else if ((remain_node_num > 0) && (g_rmesh_ota_status->written_size < g_rmesh_ota_status->total_size)) {
			rmesh_ota_firmware_download_end(0);  /*when have slave, but not receive complete ota file, also need reset*/
		}
		g_rmesh_ota_priv->ota_reset_recvd = 1;
		return;
	}

	if (g_rmesh_ota_priv->wait_inter_ota == 0) {
		RTK_LOGI(TAG, "RX STS REQ when not waiting inter ota\n");
		return;
	}

	if (!g_rmesh_ota_status) {
		g_rmesh_ota_status = rtos_mem_zmalloc(sizeof(struct rmesh_ota_upgrade_status) + recv_status->packet_num / 8 + 1);
		if (!g_rmesh_ota_status) {
			RTK_LOGE(TAG, "%s, g_rmesh_ota_status malloc failed", __func__);
			return;
		}
	}

	if (g_rmesh_ota_status->status == RMESH_OTA_STS_FINISHED) {
		return;
	}

	if (g_rmesh_ota_status->status == RMESH_OTA_STS_ONGOING ||
		(g_rmesh_ota_status->total_size == recv_status->total_size && g_rmesh_ota_status->checksum == recv_status->checksum)) {
		goto response;
	}

	/*when recv first frame of status request, stop update wait timer*/
	rtos_timer_stop(g_rmesh_ota_priv->update_wait_timer, 0);

	rtos_timer_stop(g_rmesh_ota_priv->reset_wait_timer, 0);
	rtos_timer_change_period(g_rmesh_ota_priv->reset_wait_timer, (RMESH_OTA_ROUND_MAX + 1) * RMESH_OTA_ROUND_INTERVAL + 50000, 0);
	rtos_timer_start(g_rmesh_ota_priv->reset_wait_timer, 1);

	memcpy(g_rmesh_ota_status, recv_status, sizeof(struct rmesh_ota_upgrade_status));
	g_rmesh_ota_status->progress_array = (u8 *)g_rmesh_ota_status + sizeof(struct rmesh_ota_upgrade_status);
	memset(g_rmesh_ota_status->progress_array, 0x0, g_rmesh_ota_status->packet_num / 8 + 1);
	g_rmesh_ota_status->status = RMESH_OTA_STS_ONGOING;

	u32 IMG_ADDR[2][2] = {0};
	flash_get_layout_info(IMG_BOOT, &IMG_ADDR[OTA_IMGID_BOOT][OTA_INDEX_1], NULL);
	flash_get_layout_info(IMG_BOOT_OTA2, &IMG_ADDR[OTA_IMGID_BOOT][OTA_INDEX_2], NULL);
	flash_get_layout_info(IMG_APP_OTA1, &IMG_ADDR[OTA_IMGID_APP][OTA_INDEX_1], NULL);
	flash_get_layout_info(IMG_APP_OTA2, &IMG_ADDR[OTA_IMGID_APP][OTA_INDEX_2], NULL);
	if (ota_get_cur_index(g_rmesh_ota_status->image_id) == OTA_INDEX_1) {
		target_idx = OTA_INDEX_2;
	} else {
		target_idx = OTA_INDEX_1;
	}

	g_rmesh_ota_status->image_addr = IMG_ADDR[g_rmesh_ota_status->image_id][target_idx] - SPI_FLASH_BASE;
	start_tick = rtos_time_get_current_system_time_ms();

	int sector_num = g_rmesh_ota_status->total_size / 4096 + 1;
	for (int i = 0; i < sector_num; i++) {
		flash_erase_sector(&flash_obj, g_rmesh_ota_status->image_addr + 4096 * i);
		if (i > 0 && (i % 80 == 0)) { /*add delay for NP to recv bcn to avoid father/child bcn timeout*/
			rtos_time_delay_ms(120);
		}
	}

response:

	RTK_LOGI(TAG, "ota status, image_id: %d, image_addr: 0x%x, total_size: %d bytes, written_size: %d bytes, packet_num: %d, rnd:%d\n",
			 g_rmesh_ota_status->image_id, g_rmesh_ota_status->image_addr, g_rmesh_ota_status->total_size, g_rmesh_ota_status->written_size, g_rmesh_ota_status->packet_num,
			 recv_status->cur_round_num);
	if (g_rmesh_ota_status->written_size == g_rmesh_ota_status->total_size) {
		if (RTK_SUCCESS !=
			rmesh_ota_pkt_send(RMESH_OTA_STATUS_RSP, src_mac, (u8 *)g_rmesh_ota_status, sizeof(struct rmesh_ota_upgrade_status), 0)) {
			RTK_LOGE(TAG, "%s, send fail\n", __func__);
			return;
		}
	}

	tmp_status = (struct rmesh_ota_upgrade_status *)rtos_mem_malloc(sizeof(struct rmesh_ota_upgrade_status) + RMESH_OTA_PROGRESS_MAX);
	if (!tmp_status) {
		RTK_LOGE(TAG, "%s, malloc failed", __func__);
		return;
	}

	memcpy(tmp_status, g_rmesh_ota_status, sizeof(struct rmesh_ota_upgrade_status));
	rmesh_ota_dump_buf("progress:", g_rmesh_ota_status->progress_array, g_rmesh_ota_status->packet_num / 8 + 1);
	for (int seq = 0; seq < g_rmesh_ota_status->packet_num; seq++) {
		if (!RMESH_OTA_GET_BITS(g_rmesh_ota_status->progress_array, seq)) {
			tmp_status->progress_index = seq / (8 * RMESH_OTA_PROGRESS_MAX);
			memcpy((u8 *)tmp_status + sizeof(struct rmesh_ota_upgrade_status), g_rmesh_ota_status->progress_array + tmp_status->progress_index * RMESH_OTA_PROGRESS_MAX,
				   RMESH_OTA_PROGRESS_MAX);
			if (RTK_SUCCESS !=
				rmesh_ota_pkt_send(RMESH_OTA_STATUS_RSP, src_mac, (u8 *)tmp_status, sizeof(struct rmesh_ota_upgrade_status) + RMESH_OTA_PROGRESS_MAX, 0)) {
				RTK_LOGE(TAG, "%s, send fail\n", __func__);
				return;
			}
			RTK_LOGI(TAG, "send status response, total_size: %d, written_size: %d, progress_index: %d\n",
					 tmp_status->total_size, tmp_status->written_size, tmp_status->progress_index);
			break;
		}
	}

	/*if not recv complete firmwaresize and reach sender round max, set timer to wait for reset cmd, incase not receiving reset cmd and then will not reset*/
	if ((g_rmesh_ota_status->written_size < g_rmesh_ota_status->total_size) && (recv_status->cur_round_num == RMESH_OTA_ROUND_MAX)) {
		rtos_timer_stop(g_rmesh_ota_priv->reset_wait_timer, 0);
		rtos_timer_change_period(g_rmesh_ota_priv->reset_wait_timer, RMESH_OTA_RESET_WAIT_TO, 0);
		rtos_timer_start(g_rmesh_ota_priv->reset_wait_timer, 1);
	}
	if (g_rmesh_ota_status->written_size == g_rmesh_ota_status->total_size && g_rmesh_ota_status->status == RMESH_OTA_STS_ONGOING) {
		g_rmesh_ota_status->status = RMESH_OTA_STS_FINISHED;
		RTK_LOGI(TAG, "upgrade done, spend time: %d ms\n", rtos_time_get_current_system_time_ms() - start_tick);

		/*set ota related info to flash*/
		rt_kv_get("rmesh_ota_info", &ota_info, sizeof(struct rmesh_ota_info_to_flash));
		ota_info.ota_firmware_size = g_rmesh_ota_status->total_size;
		ota_info.image_id = g_rmesh_ota_status->image_id;
		ota_info.image_addr = g_rmesh_ota_status->image_addr;
		ota_info.checksum = g_rmesh_ota_status->checksum;
		ota_info.ota_ver_len = strlen((char *)g_rmesh_ota_priv->ongoing_ota_ver);
		memset(ota_info.cur_ota_ver, 0, RMESH_OTA_VER_MAX_LEN);
		memcpy(ota_info.cur_ota_ver, g_rmesh_ota_priv->ongoing_ota_ver,  ota_info.ota_ver_len);
		rt_kv_set("rmesh_ota_info", &ota_info, sizeof(struct rmesh_ota_info_to_flash));


		/*send firmware to slave nodes*/
		if (g_rmesh_ota_priv->slave_num) {
			if (rtos_task_create(NULL, ((const char *)"rmesh_ota_internal_send_task"), rmesh_ota_internal_send_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
				RTK_LOGE(TAG, "Failed to create rmesh_ota_internal_send_task\n");
			}
		}
		rtos_timer_stop(g_rmesh_ota_priv->reset_wait_timer, 0);
		rtos_timer_change_period(g_rmesh_ota_priv->reset_wait_timer, RMESH_OTA_RESET_WAIT_TO, 0);
		rtos_timer_start(g_rmesh_ota_priv->reset_wait_timer, 1);

		g_rmesh_ota_priv->wait_inter_ota = 0; /*clear after ota finished*/
	}

	rtos_mem_free(tmp_status);
}


static void rmesh_ota_status_response_cb(u8 *src_mac, u8 *data)
{
	RTK_LOGI(TAG, "recv status response from "MAC_FMT"\n", MAC_ARG(src_mac));

	struct rmesh_ota_upgrade_status *status = NULL;
	status = (struct rmesh_ota_upgrade_status *)rtos_mem_zmalloc(sizeof(struct rmesh_ota_upgrade_status) + RMESH_OTA_PROGRESS_MAX + ETH_ALEN);
	if (!status) {
		RTK_LOGE(TAG, "%s, status malloc failed\n", __func__);
		return;
	}

	memcpy(status, data, sizeof(struct rmesh_ota_upgrade_status) + RMESH_OTA_PROGRESS_MAX);
	u8 *testmac = (u8 *)status + sizeof(struct rmesh_ota_upgrade_status) + RMESH_OTA_PROGRESS_MAX;
	memcpy(testmac, src_mac, ETH_ALEN);

	if (g_rmesh_ota_request_q) {
		if (RTK_SUCCESS != rtos_queue_send(g_rmesh_ota_request_q, &status, 0)) {
			RTK_LOGE(TAG, "%s, send queue failed\n", __func__);
			rtos_mem_free(status);
			return;
		}
	} else {
		rtos_mem_free(status);
	}
}

static void rmesh_ota_data_cb(u8 *data, u16 data_len)
{
	if (!g_rmesh_ota_status) {
		RTK_LOGW(TAG, "%s, ota has not started yet\n", __func__);
		return;
	}

	struct ota_packet_head *hdr = (struct ota_packet_head *)data;

	if (RMESH_OTA_GET_BITS(g_rmesh_ota_status->progress_array, hdr->seq)) {
		RTK_LOGD(TAG, "%s, retransmit ota packet, seq: %d\n", __func__, hdr->seq);
		return;
	}

	u32 size = data_len - sizeof(struct ota_packet_head);

	flash_stream_write(&flash_obj, g_rmesh_ota_status->image_addr + hdr->seq * RMESH_OTA_PACKET_SIZE, size, data + sizeof(struct ota_packet_head));
	g_rmesh_ota_status->written_size += size;
	RMESH_OTA_SET_BITS(g_rmesh_ota_status->progress_array, hdr->seq);
}

void rmesh_ota_update_wait_timer(void *arg)
{
	(void) arg;

	RTK_LOGI(TAG, "wait ota first sts req timeout!!\n");

	g_rmesh_ota_priv->wait_inter_ota = 0;

	rmesh_ota_firmware_download_end(0);
}

void rmesh_ota_request_wait_timer(void *arg)
{
	(void) arg;

	/*timeout, create task to start to send firmware*/
	if (rtos_task_create(NULL, ((const char *)"rmesh_ota_internal_send_task"), rmesh_ota_internal_send_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to create rmesh_ota_internal_send_task\n");
	}
}

void rmesh_ota_http_timer(void *arg)
{
	(void) arg;
	RTK_LOGI(TAG, "HTTP OTA timeout\n");
	rmesh_ota_firmware_download_end(0);
}

static void rmesh_ota_update_request_cb(u8 *src_mac, u8 *dst_mac, u8 *data)
{
	RTK_LOGI(TAG, "recv update request from"MAC_FMT"\n", MAC_ARG(src_mac));

	struct rmesh_ota_info_to_flash ota_info = {0};
	u8 req_ota_ver_len = 0;
	u8 *req_ota_ver = NULL;

	memcpy(&req_ota_ver_len, data, 1);
	req_ota_ver = data + 1;
	rt_kv_get("rmesh_ota_info", (u8 *)&ota_info, sizeof(struct rmesh_ota_info_to_flash));

	if (memcmp(dst_mac, g_rmesh_ota_priv->self_mac, ETH_ALEN)) {
		goto update_child_list;
	}

	if (!memcmp(req_ota_ver, ota_info.cur_ota_ver, req_ota_ver_len)) {
		if (g_rmesh_ota_request_q) {/* self is sending ota to others, add node to wait list, trigger send task when ongoing inter ota finished*/
			rmesh_ota_wait_node_add(src_mac);
		} else {
			rmesh_ota_slave_node_add(src_mac);
			/*start a timer to wait for more slave nodes*/
			rtos_timer_start(g_rmesh_ota_priv->req_wait_timer, 1);
		}
	} else if (g_rmesh_ota_priv && req_ota_ver_len == strlen((char *)g_rmesh_ota_priv->ongoing_ota_ver)
			   && !memcmp(req_ota_ver, g_rmesh_ota_priv->ongoing_ota_ver,
						  req_ota_ver_len)) {/*self is downloading the same image now, need wait self ota finish then to start send*/
		rmesh_ota_slave_node_add(src_mac);
	}

update_child_list:
	/*update latest rmesh topology info*/
	rmesh_ota_prepare_part2();
}

static u32 rmesh_ota_pkt_filter(u8 *pframe)
{
	/*decide which pkt need processed by self, which need relayed by driver*/
	u8 bcmc[ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	u8 ota_receiver_num = 0;
	u8 i = 0;
	u8 j = 0;
	u8 find_child = 0;
	u8 find_self = 0;
	u8 *payload = pframe + 26 + RMESH_OTA_PATTERN_LEN;
	struct rmesh_raw_additional_hdr *hdr = NULL;

	if (!memcmp(pframe + 4, bcmc, ETH_ALEN)) {/*BCMC is OTA_DATA pkt*/
		payload += g_rmesh_ota_priv->raw_pkt_iv_len;
	}
	hdr = (struct rmesh_raw_additional_hdr *)(payload);

	if (hdr->type == RMESH_OTA_STATUS_RSP && (GetToDs(pframe) == 1)) {
		if (memcmp(pframe + 16, g_rmesh_ota_priv->self_mac, ETH_ALEN)) {
			return RMESH_OTA_DRIVER_RELAY;
		} else {
			return RMESH_OTA_SELF_PROCESS;
		}
	} else if (hdr->type == RMESH_OTA_STATUS_REQ && (GetFrDs(pframe) == 1)) {
		if (memcmp(pframe + 4, g_rmesh_ota_priv->self_mac, ETH_ALEN)) {
			return RMESH_OTA_DRIVER_RELAY;
		} else {
			return RMESH_OTA_SELF_PROCESS;
		}
	} else if (hdr->type == RMESH_OTA_UPDATE_REQ) {
		if (memcmp(pframe + 16, g_rmesh_ota_priv->self_mac, ETH_ALEN)) {
			return RMESH_OTA_BOTH_SELF_AND_RELAY;/*when target mac not myself, also need update child node list to prepare for later ota data forwarding*/
		} else {
			return RMESH_OTA_SELF_PROCESS;
		}
	} else if (hdr->type == RMESH_OTA_DATA) {
		if (g_rmesh_ota_priv->ota_type == RMESH_PARTIAL_NODE_OTA) {
			ota_receiver_num = pframe[26 + g_rmesh_ota_priv->raw_pkt_iv_len + RMESH_OTA_PATTERN_LEN + sizeof(struct rmesh_raw_additional_hdr)];
			for (i = 0; i < ota_receiver_num; i++) {
				u8 *receiver_mac = pframe + 26 + g_rmesh_ota_priv->raw_pkt_iv_len + RMESH_OTA_PATTERN_LEN + sizeof(struct rmesh_raw_additional_hdr) + 1 + 6 * i;
				if (!memcmp(receiver_mac, g_rmesh_ota_priv->self_mac, ETH_ALEN)) {
					find_self = 1;
					break;
				}
				for (j = 0; j < g_rmesh_ota_priv->child_num; j++) {
					if (!memcmp(receiver_mac, g_rmesh_ota_priv->child_list[j]->mac, ETH_ALEN)) {
						find_child = 1;
						break;
					}
				}
			}
			if (find_child == 1 && find_self == 1) {
				return RMESH_OTA_BOTH_SELF_AND_RELAY;
			}
			if (find_child == 1) {
				return RMESH_OTA_DRIVER_RELAY;
			} else if (find_self == 1) {
				return RMESH_OTA_SELF_PROCESS;
			}
		} else if (g_rmesh_ota_priv->ota_type == RMESH_ALL_NODE_OTA) {
			u8 *sender_mac = pframe + 16;
			if (!memcmp(sender_mac, g_rmesh_ota_priv->father_mac, ETH_ALEN)) {
				return RMESH_OTA_SELF_PROCESS;
			}
		}
	}
	return RMESH_OTA_NO_NEED_PROCESS;
}

int rmesh_ota_pkt_recv_callback(u8 *buf, u16 len)//modify to wtn_ota_callback
{
	u32 ret = RMESH_OTA_NO_NEED_PROCESS;
	struct rmesh_ota_cb_recv_data *recv_data = NULL;
	u8 offset = 26 + RMESH_OTA_PATTERN_LEN + sizeof(struct rmesh_raw_additional_hdr);
	u8 *payload = buf + 26 + RMESH_OTA_PATTERN_LEN;
	struct rmesh_raw_additional_hdr *hdr = NULL;
	u8 ota_receiver_num = 0;
	u8 ota_receiver_len = 0;
	u8 bcmc[ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	/*check if recv deauth reason7 when http ota ongoing, if so, consider ota fail and just reset*/
	if (RMESHGetFrameSubType(buf) == RTW_DEAUTH) {
		if (g_rmesh_ota_priv->ota_method == RMESH_HTTP_OTA) {
			RTK_LOGI(TAG, "HTTP OTA RX Deauth, need reset\n");
			rtos_timer_stop(g_rmesh_ota_priv->reset_wait_timer, 0);
			rtos_timer_change_period(g_rmesh_ota_priv->reset_wait_timer, 5, 0);
			rtos_timer_start(g_rmesh_ota_priv->reset_wait_timer, 1);
			return RTK_SUCCESS;
		} else if (g_rmesh_ota_priv->ota_method == RMESH_INTER_OTA) {
			return RTK_SUCCESS;
		} else {
			return RTK_FAIL;
		}
	}

	if (len < (26 + RMESH_OTA_PATTERN_LEN + sizeof(struct rmesh_raw_additional_hdr))) {
		return RTK_SUCCESS; /*no need relay by driver*/
	}

	if (!memcmp(buf + 4, bcmc, ETH_ALEN)) {/*BCMC is OTA_DATA*/
		offset += g_rmesh_ota_priv->raw_pkt_iv_len;
		payload += g_rmesh_ota_priv->raw_pkt_iv_len;
	}
	hdr = (struct rmesh_raw_additional_hdr *)(payload);

	ret = rmesh_ota_pkt_filter(buf);
	if (ret == RMESH_OTA_DRIVER_RELAY) {/*no need processed by myself and need driver to relay*/
		return RTK_FAIL;
	} else if (ret == RMESH_OTA_NO_NEED_PROCESS) {
		return RTK_SUCCESS;
	}

	recv_data = (struct rmesh_ota_cb_recv_data *)rtos_mem_zmalloc(sizeof(struct rmesh_ota_cb_recv_data));
	if (!recv_data) {
		RTK_LOGE(TAG, "%s, recv_data malloc failed\n", __func__);
		return RTK_SUCCESS;
	}

	recv_data->type = hdr->type;
	if (recv_data->type == RMESH_OTA_DATA) {
		if (g_rmesh_ota_priv->ota_type != RMESH_ALL_NODE_OTA) {
			ota_receiver_num = buf[offset];
			ota_receiver_len = ETH_ALEN * ota_receiver_num + 1;
		}
		recv_data->data_len = len - offset - g_rmesh_ota_priv->raw_pkt_rm_len - ota_receiver_len;
	} else {
		recv_data->data_len = len - offset;
	}
	recv_data->data = (u8 *)rtos_mem_zmalloc(recv_data->data_len);
	if (!recv_data->data) {
		rtos_mem_free(recv_data);
		RTK_LOGE(TAG, "%s, recv_data malloc failed\n", __func__);
		return RTK_SUCCESS;
	}

	if (recv_data->type == RMESH_OTA_DATA) {
		memcpy(recv_data->data, buf + offset + ota_receiver_len, recv_data->data_len);
	} else {
		memcpy(recv_data->data, buf + offset, recv_data->data_len);
		if (recv_data->type == RMESH_OTA_STATUS_REQ) {
			memcpy(recv_data->mac, buf + 16, ETH_ALEN); /*src is A3*/
		} else if (recv_data->type == RMESH_OTA_STATUS_RSP) {
			memcpy(recv_data->mac, buf + 10, ETH_ALEN); /*src is A2*/
		} else if (recv_data->type == RMESH_OTA_UPDATE_REQ) {
			memcpy(recv_data->mac, buf + 10, ETH_ALEN); /*src is A2*/
			memcpy(recv_data->dst_mac, buf + 16, ETH_ALEN); /*dst is A3*/
		}
	}

	if (g_rmesh_ota_recv_q) {
		if (RTK_SUCCESS != rtos_queue_send(g_rmesh_ota_recv_q, &recv_data, 0)) {
			RTK_LOGD(TAG, "%s, send queue failed\n", __func__);
			rtos_mem_free(recv_data->data);
			rtos_mem_free(recv_data);
			return RTK_SUCCESS;
		}
	} else {
		rtos_mem_free(recv_data->data);
		rtos_mem_free(recv_data);
	}

	if (ret == RMESH_OTA_BOTH_SELF_AND_RELAY) {
		return RTK_FAIL; /*return fail for driver to relay*/
	} else {
		return RTK_SUCCESS;
	}
}

static void rmesh_ota_recv_cb_task(void *param)
{
	(void)param;
	struct rmesh_ota_cb_recv_data *recv_data = NULL;

	while (1) {
		if (RTK_SUCCESS == rtos_queue_receive(g_rmesh_ota_recv_q, &recv_data, 10)) {
			if (recv_data->type == RMESH_OTA_DATA) {
				rmesh_ota_data_cb(recv_data->data, recv_data->data_len);
			} else if (recv_data->type == RMESH_OTA_STATUS_REQ) {
				rmesh_ota_status_request_cb(recv_data->mac, recv_data->data);
			} else if (recv_data->type == RMESH_OTA_STATUS_RSP) {
				rmesh_ota_status_response_cb(recv_data->mac, recv_data->data);
			} else if (recv_data->type == RMESH_OTA_UPDATE_REQ) {
				rmesh_ota_update_request_cb(recv_data->mac, recv_data->dst_mac, recv_data->data);
			}
			rtos_mem_free(recv_data->data);
			rtos_mem_free(recv_data);
		}
	}
	rtos_task_delete(NULL);
}

void rmesh_ota_http_ota_task(void *param)
{
	int ret = -1;
	struct rmesh_http_ota_param *ota_param = (struct rmesh_http_ota_param *)param;
	ota_context *ctx = NULL;
	struct rmesh_ota_info_to_flash ota_info = {0};
	//rtos_timer_t http_ota_timer;

	u32 start_ms = rtos_time_get_current_system_time_ms();

	g_rmesh_ota_priv->ota_method = RMESH_HTTP_OTA;
	ctx = (ota_context *)rtos_mem_malloc(sizeof(ota_context));
	if (ctx == NULL) {
		goto exit;
	}

	memset(ctx, 0, sizeof(ota_context));
	g_rmesh_ota_priv->http_ota_ctx = ctx;

	ret = ota_update_init(ctx, (char *)ota_param->host, ota_param->port, (char *)ota_param->resource, OTA_HTTP);
	if (ret != 0) {
		goto exit;
	}

	// rtos_timer_create(&http_ota_timer, "rmesh_http_ota_timer", NULL, RMESH_OTA_HTTP_TO, FALSE, rmesh_ota_http_timer);
	// rtos_timer_start(http_ota_timer, 1);
	rtos_timer_stop(g_rmesh_ota_priv->reset_wait_timer, 0);
	rtos_timer_change_period(g_rmesh_ota_priv->reset_wait_timer, RMESH_OTA_HTTP_TO, 0);
	rtos_timer_start(g_rmesh_ota_priv->reset_wait_timer, 1);

	g_rmesh_ota_priv->http_ota_fail_reset = 1;

	ret = ota_update_start(ctx);

	//rtos_timer_stop(http_ota_timer, 0);
	rtos_timer_stop(g_rmesh_ota_priv->reset_wait_timer, 0);

	if (ret != 0) {

		RTK_LOGE(TAG, "%s,http ota failed\n", __func__);
		rmesh_ota_firmware_download_end(0);
		goto exit;
	}

	g_rmesh_ota_priv->http_ota_fail_reset = 0;

	RTK_LOGI(TAG, "%s, server firmware download is finished, spend time: %d ms, total size: %d\n",
			 __func__, rtos_time_get_current_system_time_ms() - start_ms, ctx->otactrl->ImageLen);

	rt_kv_get("rmesh_ota_info", (u8 *)&ota_info, sizeof(struct rmesh_ota_info_to_flash));
	ota_info.ota_firmware_size = ctx->otactrl->ImageLen;
	ota_info.image_id = ctx->otactrl->ImgId;
	ota_info.image_addr = ctx->otactrl->FlashAddr - sizeof(Manifest_TypeDef);
	ota_info.checksum = ctx->otaTargetHdr->FileImgHdr[0].Checksum;
	ota_info.ota_ver_len = strlen(ota_param->resource);
	memset(ota_info.cur_ota_ver, 0, RMESH_OTA_VER_MAX_LEN);
	memcpy(ota_info.cur_ota_ver, ota_param->resource,  strlen(ota_param->resource));
	rt_kv_set("rmesh_ota_info", &ota_info, sizeof(struct rmesh_ota_info_to_flash));

	/*check if need send firmware to other nodes*/
	if (g_rmesh_ota_priv->slave_num) {
		g_rmesh_ota_priv->http_ota_reset_delayed = 1;
		if (rtos_task_create(NULL, ((const char *)"rmesh_ota_internal_send_task"), rmesh_ota_internal_send_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
			RTK_LOGE(TAG, "Failed to create rmesh_ota_internal_send_task\n");
		}
	} else {
		rmesh_ota_firmware_download_end(1);
	}

exit:
	//rtos_timer_delete(http_ota_timer, 0);

	g_rmesh_ota_priv->http_ota_ctx = NULL;
	ota_update_deinit(ctx);
	if (ctx) {
		rtos_mem_free(ctx);
	}
	rtos_mem_free(ota_param->host);
	rtos_mem_free(ota_param->resource);

	rtos_task_delete(NULL);
}

void rmesh_ota_priv_init(void)
{
	if (g_rmesh_ota_priv == NULL) {
		/*step1: malloc g_rmesh_ota_priv*/
		g_rmesh_ota_priv = (struct rmesh_ota_priv *)rtos_mem_zmalloc(sizeof(struct rmesh_ota_priv));
		if (g_rmesh_ota_priv == NULL) {
			RTK_LOGE(TAG, "RMESH OTA MALLOC FAIL\n");
			return;
		}

		/*step2: register ota recv cb and related task*/
		rtos_queue_create(&g_rmesh_ota_recv_q, RMESH_OTA_PROGRESS_MAX, sizeof(struct rmesh_ota_cb_recv_data *));
		if (rtos_task_create(NULL, ((const char *)"rmesh_ota_recv_cb_task"), rmesh_ota_recv_cb_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
			RTK_LOGE(TAG, "Failed to create rmesh_ota_recv_cb_task\n");
		}
		wifi_rmesh_register_ota_callback(rmesh_ota_pkt_recv_callback);
		rtos_sema_create(&g_rmesh_ota_priv->slave_node_list_sema, 1, 0xFFFFFFFF);
		rtos_sema_create(&g_rmesh_ota_priv->wait_node_list_sema, 1, 0xFFFFFFFF);
		rtos_timer_create(&g_rmesh_ota_priv->req_wait_timer, "rmesh_ota_req_timer", NULL, RMESH_OTA_WAIT_MORE_SLAVE_TO, FALSE, rmesh_ota_request_wait_timer);
		rtos_timer_create(&g_rmesh_ota_priv->reset_wait_timer, "rmesh_ota_reset_timer", NULL, RMESH_OTA_RESET_WAIT_TO, FALSE, rmesh_ota_reset_wait_timer);
		rtos_timer_create(&g_rmesh_ota_priv->update_wait_timer, "rmesh_ota_update_timer", NULL, RMESH_OTA_UPDATE_REQ_WAIT_TO, FALSE, rmesh_ota_update_wait_timer);
	}
	rmesh_ota_prepare_part1();
}

void rmesh_ota_init(void)
{
#ifdef WTN_MULTI_NODE_OTA
	struct rmesh_ota_info_to_flash ota_info = {0};
	int res;
	u8 need_init = 1;

	res = rt_kv_get("rmesh_ota_info", (u8 *)&ota_info, sizeof(struct rmesh_ota_info_to_flash));
	if (res == sizeof(struct rmesh_ota_info_to_flash)) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "flash ver: len=%d,%s %s,%s\n", ota_info.ota_ver_len, ota_info.http_host, ota_info.target_ota_ver, ota_info.cur_ota_ver);
		if ((ota_info.ota_ver_len > 0) && (memcmp(ota_info.target_ota_ver, ota_info.cur_ota_ver, ota_info.ota_ver_len))) {
			/*last ota fail, need perform ota again*/
			memset(&ota_param, 0, sizeof(struct rmesh_http_ota_param));
			ota_param.port = ota_info.http_port;
			ota_param.resource = (char *)rtos_mem_zmalloc(strlen((char *)ota_info.target_ota_ver) + 1);
			memcpy(ota_param.resource, ota_info.target_ota_ver, strlen((char *)ota_info.target_ota_ver));
			ota_param.host = (char *)rtos_mem_zmalloc(strlen((char *)ota_info.http_host) + 1);
			memcpy(ota_param.host, ota_info.http_host, strlen((char *)ota_info.http_host));
			ota_param.ota_type = RMESH_PARTIAL_NODE_OTA;
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "RMESH PERFORM OTA DUE TO LAST FAIL\n");
			rmesh_ota_cmd_recv(&ota_param);
			need_init = 0;
		} else if (ota_info.ota_ver_len > 0) {
			wifi_rmesh_update_node_ota_ver((u8 *)ota_info.cur_ota_ver, ota_info.ota_ver_len, 0);
		}
	}

	if (need_init) {
		rmesh_ota_priv_init();
	}
#endif
}

void rmesh_ota_get_status(struct rmesh_ota_status *ota_sts)
{
	struct rmesh_ota_info_to_flash ota_info = {0};

	/*fetch flash ota ver to check if OTA already success*/
	rt_kv_get("rmesh_ota_info", (u8 *)&ota_info, sizeof(struct rmesh_ota_info_to_flash));
	if (ota_info.ota_ver_len) {
		if (memcmp(ota_info.cur_ota_ver, ota_info.target_ota_ver, ota_info.ota_ver_len) == 0) {
			ota_sts->current_state = RMESH_OTA_SUCCESS;
			ota_sts->percentage = 100;
			return;
		} else {
			if (ota_info.ota_sts == RMESH_OTA_RETRYING) {/*ota fail and reset to ota again*/
				ota_sts->current_state = RMESH_OTA_RETRYING;
				if (g_rmesh_ota_status) {
					ota_sts->percentage = (g_rmesh_ota_status->written_size * 100) / g_rmesh_ota_status->total_size;
				} else {
					ota_sts->percentage = 0;
				}
				return;
			}
		}
	}

	/*case1: http ota*/
	if (g_rmesh_ota_priv->ota_method == RMESH_HTTP_OTA) {
		ota_sts->current_state = RMESH_OTA_ONGOING;
		if (g_rmesh_ota_priv->http_ota_ctx) {
			/*todo: need ameba_ota.c to provide an API*/
		} else {
			ota_sts->percentage = 0;
		}
		return;
	}

	/*case2: inter ota*/
	if (g_rmesh_ota_priv->ota_method == RMESH_INTER_OTA) {
		if (g_rmesh_ota_status) {
			ota_sts->percentage = (g_rmesh_ota_status->written_size * 100) / g_rmesh_ota_status->total_size;
		} else {
			ota_sts->percentage = 0;
		}
		ota_sts->current_state = RMESH_OTA_ONGOING;
		return;
	}

	/*case4: not performing ota(have not recv server's ota cmd)*/
	ota_sts->current_state = 0;
	ota_sts->percentage = 0;
}

void rmesh_ota_create_http_download_task(struct rmesh_http_ota_param *ota_param)
{
	if (rtos_task_create(NULL, ((const char *)"rmesh_http_ota_task"), rmesh_ota_http_ota_task, ota_param, 1024 * 8, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to create rmesh_http_ota_task\n");
	}
}

void rmesh_ota_wait_for_update_status_req(struct rmesh_http_ota_param *ota_param)
{
	if (g_rmesh_ota_status) {
		rtos_mem_free(g_rmesh_ota_status);/*when a new ota start, need free old ota status*/
		g_rmesh_ota_status = NULL;
	}

	g_rmesh_ota_priv->wait_inter_ota = 1;
	g_rmesh_ota_priv->ota_method = RMESH_INTER_OTA;

	rtos_timer_stop(g_rmesh_ota_priv->update_wait_timer, 0);
	rtos_timer_change_period(g_rmesh_ota_priv->update_wait_timer, g_rmesh_ota_priv->self_layer * RMESH_OTA_UPDATE_REQ_WAIT_TO, 0);
	rtos_timer_start(g_rmesh_ota_priv->update_wait_timer, 0); // this timer will reset the system

	rtos_mem_free(ota_param->host);
	rtos_mem_free(ota_param->resource);
}

void rmesh_ota_send_update_req_and_wait(u8 *ota_ver, u8 ota_ver_len, u8 *target_mac, struct rmesh_http_ota_param *ota_param)
{
	u8 *buf = NULL;

	buf = (u8 *)rtos_mem_zmalloc(ota_ver_len + 1);
	buf[0] = ota_ver_len;
	memcpy(buf + 1, ota_ver, ota_ver_len);

	RTK_LOGI(TAG, "Send update req to %x\n", target_mac[5]);
	if (rmesh_ota_pkt_send(RMESH_OTA_UPDATE_REQ, target_mac, buf, ota_ver_len + 1, 0) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to send OTA update request\n");
	}
	rmesh_ota_wait_for_update_status_req(ota_param);
}

void rmesh_ota_check_root_and_father_ver_and_choose_download_method(u16 delay_time, struct rmesh_http_ota_param *ota_param)
{
	/*wait several bcn interval for ota_ver updated in bcn, incase me receive ota cmd before father/root*/
	rtos_time_delay_ms(delay_time);
	if (wifi_rmesh_check_node_ota_ver((u8 *)ota_param->resource, strlen(ota_param->resource), RMESH_FATHER_NODE) == RTK_SUCCESS) {
		rmesh_ota_send_update_req_and_wait(g_rmesh_ota_priv->ongoing_ota_ver, strlen((char *)g_rmesh_ota_priv->ongoing_ota_ver), g_rmesh_ota_priv->father_mac,
										   ota_param);
	} else {
		if (wifi_rmesh_check_node_ota_ver((u8 *)ota_param->resource, strlen(ota_param->resource), RMESH_ROOT_NODE) == RTK_SUCCESS) {
			rmesh_ota_send_update_req_and_wait(g_rmesh_ota_priv->ongoing_ota_ver, strlen((char *)g_rmesh_ota_priv->ongoing_ota_ver), g_rmesh_ota_priv->root_mac, ota_param);
		} else {
			rmesh_ota_create_http_download_task(ota_param);
		}
	}
}

void rmesh_ota_cmd_recv(struct rmesh_http_ota_param *ota_param)
{
	u8 ota_ver_len = 0;
	struct rmesh_ota_info_to_flash ota_info = {0};

	/*check flash ota ver*/
	ota_ver_len = strlen(ota_param->resource) > RMESH_OTA_VER_MAX_LEN ? RMESH_OTA_VER_MAX_LEN : strlen(ota_param->resource);
	rt_kv_get("rmesh_ota_info", (u8 *)&ota_info, sizeof(struct rmesh_ota_info_to_flash));
	RTK_LOGI(TAG, "Req VER:%s, Flash VER:%s\n", ota_param->resource, ota_info.cur_ota_ver);
	if (!memcmp(ota_param->resource, ota_info.cur_ota_ver, ota_ver_len)) {
		return; /*request ota ver is current ota ver, no need process again*/
	}

	/*check on-going ota ver*/
	if (g_rmesh_ota_priv && g_rmesh_ota_priv->ongoing_ota_ver[0]) {
		RTK_LOGI(TAG, "ota ongoing ver:%s, drop new OTA cmd\n", g_rmesh_ota_priv->ongoing_ota_ver);
		return;
	}

	/*set http ota related info to flash, incase ota fail and reset to perform ota again*/
	ota_info.http_port = ota_param->port;
	memset(ota_info.http_host, 0, RMESH_OTA_HTTP_HOST_MAX_LEN);
	memcpy(ota_info.http_host, ota_param->host, strlen(ota_param->host));
	memset(ota_info.target_ota_ver, 0, RMESH_OTA_VER_MAX_LEN);
	memcpy(ota_info.target_ota_ver, ota_param->resource, strlen(ota_param->resource));
	ota_info.ota_ver_len = strlen(ota_param->resource);
	rt_kv_set("rmesh_ota_info", (u8 *)&ota_info, sizeof(struct rmesh_ota_info_to_flash));

	rmesh_ota_priv_init();

	g_rmesh_ota_priv->ota_type = ota_param->ota_type;
	g_rmesh_ota_priv->ota_start_time = SYSTIMER_TickGet();

	/*update ota ver to driver*/
	wifi_rmesh_update_node_ota_ver((u8 *)ota_param->resource, strlen(ota_param->resource), 1);
	rmesh_ota_prepare_part2();
	memcpy(g_rmesh_ota_priv->ongoing_ota_ver, ota_param->resource, ota_ver_len);
	g_rmesh_ota_priv->ongoing_ota_ver[ota_ver_len] = 0;

	if (wifi_user_config.wtn_rnat_en) {/*case 1: self is RNAT*/
		rmesh_ota_create_http_download_task(ota_param);
	} else if (g_rmesh_ota_priv->self_layer == 1) {/*case 2.1: self is not RNAT and self is root*/
		if (g_rmesh_ota_priv->father_is_rnat) {/*case 2.1.1: self is not RNAT and father is RNAT*/
			if (g_rmesh_ota_priv->ota_type != RMESH_ALL_NODE_OTA) {/*case 2.1.1.1: father is rnat and ota_type is partial OTA*/
				rmesh_ota_check_root_and_father_ver_and_choose_download_method(240 * (g_rmesh_ota_priv->self_layer + 1), ota_param);
			} else {/*case 2.1.1.2: father is rnat and ota_type is all-node OTA*/
				rmesh_ota_send_update_req_and_wait(g_rmesh_ota_priv->ongoing_ota_ver, ota_ver_len, g_rmesh_ota_priv->father_mac, ota_param);
			}
		} else {/*case 2.1.2: self is not RNAT and father is AP*/
			rmesh_ota_create_http_download_task(ota_param);
		}
	} else {/*case 2.2: self is not RNAT and self is not root*/
		if (g_rmesh_ota_priv->ota_type == RMESH_ALL_NODE_OTA) {/*case 2.2.1: self is not root and ota_type is all-node OTA*/
			rmesh_ota_wait_for_update_status_req(ota_param);
		} else {/*case 2.2.2: self is not root and ota_type is partial OTA*/
			rmesh_ota_check_root_and_father_ver_and_choose_download_method(240 * g_rmesh_ota_priv->self_layer, ota_param);
		}
	}
}

#if defined(WTN_SINGLE_NODE_OTA) || defined(WTN_MULTI_NODE_OTA)

int wtn_on_ota_request(u8 *buf, int recv_len, int *forward_sock_fd)
{
	u8 ota_request_seq = 0;
	u8 httpiplen = 0;
	u8 http_resourcelen = 0;
	u8 *self_mac_p1 = LwIP_GetMAC(NETIF_WLAN_AP_INDEX);
	u8 *self_mac_p0 = LwIP_GetMAC(NETIF_WLAN_STA_INDEX);
	u8 *gw = LwIP_GetGW(NETIF_WLAN_AP_INDEX);
	u8 client_ip = 0;
	u8 target_mac[6] = {0};
	u8 try_cnt = 5;
	struct sockaddr_in ota_forward_dest_addr;

	if (buf[12] != ota_request_seq) {
		ota_request_seq = buf[12];

		/*get target mac when rnat*/
		memcpy(target_mac, buf + 15, ETH_ALEN);
		/*get host and resource*/
		httpiplen = buf[22];
		memset(&ota_param, 0, sizeof(struct rmesh_http_ota_param));
		ota_param.host = (char *)rtos_mem_zmalloc(httpiplen + 1);
		memcpy(ota_param.host, buf + 23, httpiplen);
		memcpy(&ota_param.port, buf + 25 + httpiplen, 2);
		ota_param.port = ntohs(ota_param.port);
		http_resourcelen = buf[28 + httpiplen];
		ota_param.resource = (char *)rtos_mem_zmalloc(http_resourcelen + 1);
		memcpy(ota_param.resource, buf + 29 + httpiplen, http_resourcelen);
		ota_param.ota_type = buf[31 + httpiplen + http_resourcelen];

		if (wifi_user_config.wtn_rnat_en && wtn_rnat_ap_start) {
			if (memcmp(target_mac, self_mac_p1, ETH_ALEN)) {
				if (*forward_sock_fd < 0) {
					*forward_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
					if (*forward_sock_fd < 0) {
						RTK_LOGS(NOTAG, RTK_LOG_ERROR, "rnat unicast socket create fail\n");
						return RTK_FAIL;
					}
				}
				/*get client ip*/
				client_ip = wtn_rnat_search_client_ip(target_mac);
				if (client_ip != 0) {
					/* initialize value in dest */
					memset(&ota_forward_dest_addr, 0, sizeof(ota_forward_dest_addr));
					ota_forward_dest_addr.sin_family = AF_INET;
					ota_forward_dest_addr.sin_port = htons(WTN_UNICAST_PORT);
					ota_forward_dest_addr.sin_addr.s_addr = client_ip << 24 | gw[2] << 16 | gw[1] << 8 | gw[0];
					while (try_cnt > 0) {
						if (sendto(*forward_sock_fd, buf, recv_len, 0, (struct sockaddr *)&ota_forward_dest_addr, sizeof(struct sockaddr_in)) > 0) {
							break;
						} else {
							try_cnt--;
						}
					}
					if (try_cnt == 0) {
						RTK_LOGS(NOTAG, RTK_LOG_ERROR, "rnat forward ota cmd fail\n");
					}
				}
			}
		}
#ifdef WTN_MULTI_NODE_OTA
		if ((wifi_user_config.wtn_rnat_en && (!memcmp(target_mac, self_mac_p1, ETH_ALEN))) ||
			(!memcmp(target_mac, self_mac_p0, ETH_ALEN))) { /*ota cmd to my self, peform OTA*/
			rmesh_ota_cmd_recv(&ota_param);
		}
#else
		/*create task to do http ota*/
		if (rtos_task_create(NULL, ((const char *)"update_ota_task"), wtn_http_ota_task, &ota_param, 8000, 1) != RTK_SUCCESS) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "wtn ota task create fail\n");
		}
#endif
	}
	return RTK_SUCCESS;
}
#endif
#endif