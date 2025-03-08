/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef __WHC_HOST_API_H__
#define __WHC_HOST_API_H__

#define WIFI_STACK_SIZE_INIC_IPC_HST_API (2848 + 128 + CONTEXT_SAVE_SIZE)	// for psp overflow when update group key: jira: https://jira.realtek.com/browse/RSWLANQC-1027
#define WIFI_STACK_SIZE_INIC_MSG_Q		0

/* -------------------------------- Includes -------------------------------- */
void whc_host_api_message_send(u32 id, u8 *param, u32 param_len, u8 *ret, u32 ret_len);
void whc_host_api_send_ret_value(u32 api_id, u8 *pbuf, u32 len);

void whc_host_api_scan_user_callback_handler(u32 api_id, u32 *param_buf);
void whc_host_api_scan_each_report_callback_handler(u32 api_id, u32 *param_buf);

void whc_host_api_wifi_event_handler(u32 api_id, u32 *param_buf);
void whc_host_api_lwip_info_handler(u32 api_id, u32 *param_buf);
void whc_host_api_set_netif_info_handler(u32 api_id, u32 *param_buf);
void whc_host_api_ip_table_chk(u32 api_id, u32 *param_buf);
void whc_host_api_ap_ch_switch(u32 api_id, u32 *param_buf);
int whc_host_api_iwpriv_command(char *cmd, unsigned int cmd_len, int show_msg);
void whc_host_api_task(void);

#endif /* __INIC_HOST_H__ */
