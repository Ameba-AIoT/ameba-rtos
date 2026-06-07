// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __WHC_HOST_PROTOCAL_OFFLOAD_H__
#define __WHC_HOST_PROTOCAL_OFFLOAD_H__

#define MAX_MDNS_DOMAIN_NAME_LEN 63
#define MAX_MDNS_DOMAIN_NAME_LEN_FOR_SSCAN "%63s" //change according to MAX_MDNS_DOMAIN_NAME_LEN

#define RTW_MDNS_SRV_INFO(sname, sname_len, tname, tname_len, dname, dname_len, port0, port1, ttlv, tar, tar_len, idx) \
	{ .service=sname, .service_len=sname_len, .transport=tname, .transport_len=tname_len, \
	  .domain=dname , .domain_len=dname_len , .port[0]=port0, .port[1]=port1, .ttl=ttlv, \
	  .target=tar, .target_len=tar_len, .txt_rsp_idx=idx }

#define MAX_MDNS_SERVICE_NUM 10
#define MAX_MDNS_TXT_NUM 8
#define MAX_MDNS_MACHINE_NAME_NUM 3

#define MAX_MDNS_TXT_LEN 1536
#define MAX_MDNS_TXT_SINGLE_LEN 255

/* for monitor rsvd page using */
#define MAX_MDNS_PARA_SIZE 1700 // 14*128 = 1792
#define MAX_MDNS_TXT_TOTAL_SIZE (10 * MAX_MDNS_TXT_LEN)
#define MAX_MDNS_RSP_PKT_SIZE 760   //  6*128 = 768

/* limitation of mDNS parameter : length and number */
#define MAX_MDNS_SERVICE_NAME_LEN 15
#define MAX_MDNS_TRANS_LEN 4 /* _tcp or _udp */
#define MAX_MDNS_DOMAIN_LEN 5 /* local only for mdns */
#define MAX_MDNS_MACHINE_NAME_LEN (63+1) /* +1 for the length byte used by the DNS format */
#define MAX_MDNS_TARGET_LEN 63

struct war_mdns_service_info {
	u8  service[MAX_MDNS_SERVICE_NAME_LEN + 1];
	u8  service_len;
	u8  transport[MAX_MDNS_TRANS_LEN + 1];
	u8  transport_len;
	u8  domain[MAX_MDNS_DOMAIN_LEN + 1];
	u8  domain_len;
	u8  port[2];
	u32 ttl;
	u8  target[MAX_MDNS_TARGET_LEN + 1];
	u8  target_len;
	s8  txt_rsp_idx;
};

struct war_mdns_machine_name {
	u8	name[MAX_MDNS_MACHINE_NAME_LEN];
	u8	name_len;
};

struct war_mdns_txt_rsp {
	u8  txt[MAX_MDNS_TXT_LEN];
	u16  txt_len;
};

struct proxy_priv {
	u8 wowlan_war_offload_mode;
	u32 	wowlan_war_offload_ctrl;

	struct war_mdns_machine_name wowlan_war_offload_mdns_mnane[MAX_MDNS_MACHINE_NAME_NUM];
	struct war_mdns_service_info wowlan_war_offload_mdns_service[MAX_MDNS_SERVICE_NUM];
	struct war_mdns_txt_rsp      wowlan_war_offload_mdns_txt_rsp[MAX_MDNS_TXT_NUM];
	u8     wowlan_war_offload_mdns_mnane_num;
	u8     wowlan_war_offload_mdns_service_info_num;
	u8     wowlan_war_offload_mdns_txt_rsp_num;
	u8     wowlan_war_offload_mdns_domain_name[MAX_MDNS_DOMAIN_NAME_LEN + 1];
	u8     wowlan_war_offload_mdns_domain_name_len;
};

#endif
