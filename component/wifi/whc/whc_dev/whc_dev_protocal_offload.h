// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __WHC_DEV_PROTOCAL_OFFLOAD_H__
#define __WHC_DEV_PROTOCAL_OFFLOAD_H__

#define CONFIG_WAR_OFFLOAD
#define CONFIG_OFFLOAD_MDNS_V4
#define CONFIG_OFFLOAD_MDNS_V6

/*
* Write host byte order data to memory in LE order
*/
#define WriteLE4Byte(_ptr, _val)	((*((u32 *)(_ptr))) = cpu_to_le32(_val))
#define WriteLE2Byte(_ptr, _val)	((*((u16 *)(_ptr))) = cpu_to_le16(_val))
#define WriteLE1Byte(_ptr, _val)	((*((u8 *)(_ptr))) = ((u8)(_val)))

#define SET_UDP_SRC_PORT(__pHeader, __Value)	WriteLE2Byte(((u8 *)(__pHeader)) + 0, __Value)
#define SET_UDP_DST_PORT(__pHeader, __Value)	WriteLE2Byte(((u8 *)(__pHeader)) + 2, __Value)
#define SET_UDP_LEN(__pHeader, __Value)			WriteLE2Byte(((u8 *)(__pHeader)) + 4, __Value)
#define SET_UDP_CHECKSUM(__pHeader, __Value)			WriteLE2Byte(((u8 *)(__pHeader)) + 6, __Value)

#define cpMacAddr(des,src)					((des)[0]=(src)[0],(des)[1]=(src)[1],(des)[2]=(src)[2],(des)[3]=(src)[3],(des)[4]=(src)[4],(des)[5]=(src)[5])
#define cpIpAddr(des,src)					((des)[0]=(src)[0],(des)[1]=(src)[1],(des)[2]=(src)[2],(des)[3]=(src)[3])

/* CONFIG_OFFLOAD_ICMP_V4 */
#define SET_IPHDR_VERSION(__pHeader, __Value)				WriteLE1Byte(((u8 *)(__pHeader)) + 0, __Value)
#define SET_IPHDR_DSCP(__pHeader, __Value)					WriteLE1Byte(((u8 *)(__pHeader)) + 1, __Value)
#define SET_IPHDR_TOTAL_LEN(__pHeader, __Value)			WriteLE2Byte(((u8 *)(__pHeader)) + 2, __Value)
#define SET_IPHDR_IDENTIFIER(__pHeader, __Value)			WriteLE2Byte(((u8 *)(__pHeader)) + 4, __Value)
#define SET_IPHDR_FLAGS(__pHeader, __Value)				WriteLE1Byte(((u8 *)(__pHeader)) + 6, __Value)
#define SET_IPHDR_FRAG_OFFSET(__pHeader, __Value)			WriteLE1Byte(((u8 *)(__pHeader)) + 7, __Value)
#define SET_IPHDR_TTL(__pHeader, __Value)					WriteLE1Byte(((u8 *)(__pHeader)) + 8, __Value)
#define SET_IPHDR_PROTOCOL(__pHeader, __Value)			WriteLE1Byte(((u8 *)(__pHeader)) + 9, __Value)
#define SET_IPHDR_HDR_CHECKSUM(__pHeader, __Value)		WriteLE2Byte(((u8 *)(__pHeader)) + 10, __Value)
#define SET_IPHDR_SRC_IP_ADDR(__pHeader, __Value)			cpIpAddr(((u8 *)(__pHeader))+12, (u8 *)(__Value))
#define SET_IPHDR_DST_IP_ADDR(__pHeader, __Value)			cpIpAddr(((u8 *)(__pHeader))+16, (u8 *)(__Value))

/* CONFIG_OFFLOAD_ICMP_V6 */
#define SET_IPHDRV6_VERSION(__pHeader, __Value)                     SET_BITS_TO_LE_1BYTE(__pHeader, 4, 4, __Value)
#define SET_IPHDRV6_TRAFFIC_CLASS(__pHeader, __Value)               SET_BITS_TO_LE_2BYTE(__pHeader, 4, 8, __Value)
#define SET_IPHDRV6_FLOW_LABEL(__pHeader, __Value)                  SET_BITS_TO_LE_4BYTE(__pHeader, 12, 20, __Value)
#define SET_IPHDRV6_PAYLOAD_LENGTH(__pHeader, __Value)              SET_BITS_TO_LE_2BYTE(((u8 *)(__pHeader)) + 4, 0, 16, __Value)
#define SET_IPHDRV6_NEXT_HEADER(__pHeader, __Value)                 SET_BITS_TO_LE_1BYTE((__pHeader) + 6, 0, 8, __Value)
#define SET_IPHDRV6_HOP_LIMIT(__pHeader, __Value)                   SET_BITS_TO_LE_1BYTE((__pHeader) + 7, 0, 8, __Value)
#define SET_IPHDRV6_SRC_IP_ADDR(__pHeader, __Value)                 cpIpv6Addr((u8 *)(__pHeader) + 8, (u8 *)(__Value))
#define SET_IPHDRV6_DST_IP_ADDR(__pHeader, __Value)                 cpIpv6Addr((u8 *)(__pHeader) + 24, (u8 *)(__Value))

#define SET_MDNS_HDR_FLAG(__pHeader, __Value)		WriteLE1Byte(((u8 *)(__pHeader)) + 2, __Value)

// below is IPv4 header offset
#define IP_VERSION_IHL                  1
#define TOS                             1
#define IP_LENGTH                       2
#define IPID                            2
#define FLAG_OFFSET                     2
#define TTL                             1
#define PROTOCOL                        1
#define IPCHECKSUM                      2
#define IP_ID_OFFSET                    4
#define IP_CHECKSUM_OFFSET              10
#define IP_SRC_ADDR_OFFSET              12
#define IP_DES_ADDR_OFFSET              16
#define IP_HEADER                       20
#define UDP_LEN                         8

// below is IPv6 header offset
#define IPv6_ALEN                           16
#define IPv6_HEADER                         40
#define IPv6_SRC_ADDRESS_OFFSET             8
#define IPv6_DES_ADDRESS_OFFSET             24
#define IPv6_PAYLOAD_LENGTH_OFFSET          4
#define IPv6_NEXT_HEADER_OFFSET             6
#define IPv6_HOP_LIMIT_OFFSET               7
#define IPv6_SRC_ADDRESS_OFFSET             8
#define CLIENT_PORT_IN_SRC_IPv6_OFFSET      (IPv6_SRC_ADDRESS_OFFSET+10)
#define CLIENT_IPv4_IN_SRC_IPv6_OFFSET      (IPv6_SRC_ADDRESS_OFFSET+12)


#define ICMPV6_TYPE_OFFSET                  0
#define ICMPV6_CHECKSUM_OFFSET              2
#define ICMPV6_TARGETADDRESS_OFFSET         8
#define ICMPV6_OPTION_TYPE_OFFSET           24
#define ICMPV6_OPTION_LENGTH_OFFSET         25
#define ICMPV6_OPTION_TARGET_ADDR_OFFSET    26
#define ICMPV6_HEADER                       32
#define ICMP_CHECKSUM_SRC_ADDR_OFFSET       0
#define ICMP_CHECKSUM_DES_ADDR_OFFSET       72

// below is IPv4 header offset
#define ICMPV4_HEADER_LEN                   8
#define ICMPV4_TYPE_OFFSET                  0
#define ICMPV4_CODE_OFFSET                  1
#define ICMPV4_CHECKSUM_OFFSET              2
#define ICMPV4_IDENTIFIER_OFFSET            4
#define ICMPV4_SEQ_NUM_OFFSET               6
#define ICMPV4_DATA_AREA_OFFSET             8

#define IP_V4_DATA_OFFSET               20
#define IP_V6_DATA_OFFSET               40
#define UDP_DATA_OFFSET                 8

// below is UDP header offset
#define NONCE_VALUE_LENGTH                  16
#define UDP_SRC_PORT_OFFSET                 0
#define UDP_DES_PORT_OFFSET                 2
#define UDP_NONCE_OFFSET                    4
#define UDP_LENGTH_OFFSET                   4
#define UDP_CHECK_SUM_OFFSET                6
#define UDP_HEADER                          8

// below is IPv4 header offset
#define IPv4_PROTOCOL_OFFSET                9
#define IPv4_DEST_IP_OFFSET                 16
#define IPv4_ALEN                       4
#define IP_SRC_ADDR_OFFSET              12
#define IP_HEADER                       20

// MDNS offset
#define MDNS_NUM_ANS                        7
#define MDNS_NUM_ADD                        11

#define MDNS_TLV_TAG_SIZE                   1
#define MAX_SUPPORT_SERVICE                 10
#define MAX_MACHINE_NAME                    3
#define MAX_TYPE_TXT_RSP                    8

#define MDNS_HEADER                         12
#define MDNS_HEADER_QUE_OFFSET              5
#define MDNS_HEADER_ANS_OFFSET              7
#define MDNS_HEADER_ADD_OFFSET              11
#define MAX_RECORD                          MAX_SUPPORT_SERVICE*MAX_MACHINE_NAME

// For IP Compare
#define COMPARE_NONE                        0x0
#define COMPARE_SPECIFIED_IP                0x1
#define COMPARE_UNICAST_IP                  0x2
#define COMPARE_BROADCAST_IP                0x4

/* limitation of mDNS parameter : length and number */
#define MAX_MDNS_SERVICE_NAME_LEN 15
#define MAX_MDNS_TRANS_LEN 4 /* _tcp or _udp */
#define MAX_MDNS_DOMAIN_LEN 5 /* local only for mdns */
#define MAX_MDNS_MACHINE_NAME_LEN (63+1) /* +1 for the length byte used by the DNS format */
#define MAX_MDNS_TARGET_LEN 63

#define MAX_MDNS_DOMAIN_NAME_LEN 63
#define MAX_MDNS_DOMAIN_NAME_LEN_FOR_SSCAN "%63s" //change according to MAX_MDNS_DOMAIN_NAME_LEN

#define MAX_MDNS_TXT_LEN 1536
#define MAX_MDNS_TXT_SINGLE_LEN 255

#define MAX_MDNS_SERVICE_NUM 10
#define MAX_MDNS_TXT_NUM 8
#define MAX_MDNS_MACHINE_NAME_NUM 3

/* for monitor rsvd page using */
#define MAX_MDNS_PARA_SIZE 1700 // 14*128 = 1792
#define MAX_MDNS_TXT_TOTAL_SIZE 10*MAX_MDNS_TXT_LEN
#define MAX_MDNS_RSP_PKT_SIZE 760   //  6*128 = 768

#define RTW_MDNS_SRV_INFO(sname, sname_len, tname, tname_len, dname, dname_len, port0, port1, ttlv, tar, tar_len, idx) \
	{ .service=sname, .service_len=sname_len, .transport=tname, .transport_len=tname_len, \
	  .domain=dname , .domain_len=dname_len , .port[0]=port0, .port[1]=port1, .ttl=ttlv, \
	  .target=tar, .target_len=tar_len, .txt_rsp_idx=idx }

enum Offload_Service {
	Offload_Service_Unknown = 0x00,
	Offload_Service_NetBios = 0x01,
	Offload_Service_Snmpv4 = 0x02,
	Offload_Service_Snmpv6 = 0x03,
	Offload_Service_mDnsv4 = 0x04,
	Offload_Service_mDnsv6 = 0x05,
	Offload_Service_SSDPv4 = 0x06,
	Offload_Service_SSDPv6 = 0x07,
	Offload_Service_WSDv4 = 0x08,
	Offload_Service_WSDv6 = 0x09,
	Offload_Service_SLPv4 = 0x0A,
	Offload_Service_SLPv6 = 0x0B,
	Offload_Service_LLTD = 0x0C,
	Offload_Service_ICMPv4 = 0x0D,
	Offload_Service_ICMPv6 = 0x0E,
	Offload_Service_ICMPNS = 0x0F,
	Offload_Service_LLMNRv4 = 0x10,
	Offload_Service_LLMNRv6 = 0x11,
	Offload_Service_Max = 0x12
};

enum CHECK_SUM_PROTOCOL {
	IP_CHECK_SUM    = 0,
	TCP_CHECK_SUM   = 6,
	UDP_CHECK_SUM   = 17,
	UDPV6_CHECK_SUM  = 18,
	ICMPV4_CHECK_SUM = 57,
	ICMPV6_CHECK_SUM = 58,
	ICMPV6_CHECK_SUM_v2 = 59,

	MAX_CHECK_SUM = 255

};

struct TXFF_STRING {
	u8 *str;
	u16  len;
};

struct MDNS_SUPPORT_SERVICE {
	struct TXFF_STRING  service_name;
	struct TXFF_STRING  type_srv_rsp;
	struct TXFF_STRING  type_srv_target_name;
	u8  type_txt_rsp_idx;
};

struct MDNS_PARM {
	u8 *mdns_para_buf;

	struct MDNS_SUPPORT_SERVICE  service_list[MAX_SUPPORT_SERVICE];
	u8 num_support_service;
	struct TXFF_STRING machine_name_list[MAX_MACHINE_NAME];
	u8 num_machine_name;
	struct TXFF_STRING type_txt_rsp_list[MAX_TYPE_TXT_RSP];
	u8 num_type_txt_rsp;
	struct TXFF_STRING  type_a_rsp;
	struct TXFF_STRING  type_aaaa_rsp;
	struct TXFF_STRING  type_ptr_rsp; // field offset should be filled later
	struct TXFF_STRING  txt_in_type_ptr_rsp; // field offset should be filled later
};

struct MDNS_RECORD {
	bool is_multicast;
	bool is_machine_name_sub_exist;
	u8  type[2];
	s8  matched_machine_name_id; /* such as: _universal._sub */
	s8  matched_service_name_id; /* such as: _ipp._tcp.local */
	s8  matched_target_name_id; /* such as: KM1 */
	s8  first_match_node; /* to know machine_name is un-existed or mismatch */
};

struct MDNS_RX_DATA { /* information get from mDNS req. */
	bool is_c0_match;
	// bool is_ipv4;
	u8  sender_ipv4[4];
	u8  sender_ipv6[16];
	u8  transactionId[2];
	u8  num_ans;
	u8  num_question;
	u8  num_add;
	struct MDNS_RECORD ans_records[MAX_RECORD];
};

struct IPV4_PARM {
	u8 IP[4];
	u16 identifier;
};

struct IPV6_PARM {
	u8 IP[16];
};

enum {
	DNS_TYPE_A = 0x0100,
	DNS_TYPE_AAAA = 0x1c00,
	DNS_TYPE_SRV = 0x2100,
	DNS_TYPE_TXT = 0x1000,
	DNS_TYPE_PTR = 0x0c00,
	DNS_TYPE_OPT = 0x2900
};

#define IS_SUPPORT_DNS_TYPE(rx, type) (rx[0]==(u8)type && rx[1]==(u8)(type>>8))
#define IS_NO_MACHINE_NAME(precord) ((precord->matched_service_name_id != -1) && (precord->first_match_node==0))
#define IS_MACHINE_NAME_MATCHED(precord) (precord->matched_machine_name_id != -1)
#define IS_TARGET_NAME_MATCHED(precord) (precord->matched_target_name_id != -1)


struct H2C_WAROFFLOAD_PARM {
	u8 offload_en;
	u32 offload_ctrl;
};

struct IPV4_OFFLOAD_SERVICE_TABLE {
	u8              u1Prot;         /* IPv4 Protocal */
	u8              u1IpCmpType;    /* BIT[0] : Specified in au1DstIp, BIT[1] : Unicast or IPv4Parm.pDstIP, BIT[2] : Broadcast or 255.255.255.255*/
	u8              u1IpMatchSize;  /* Byte Count for Comparing Dst IP */
	u8              u1PortMask;     /* A bitmask for port matching, BIT[0] : Dest Port need match, BIT[1] : Src Port need match */
	u8              au1DstIp[4];        /* IPv4 Destination IP Address */
	u16             u2SrcPort;      /* UDP Source Port or ICMP Type */
	u16             u2DstPort;      /* UDP Destination Port or ICMP Type */
	u8				Service;        /* Defined Service */
};

struct IPV6_OFFLOAD_SERVICE_TABLE {
	u8              u1Prot;         /* IPv6 Protocal */
	u8              u1IpCmpType;    /* BIT[0] : Specified in au2DstIp, BIT[1] : Unicast or IPv6Parm.pDstIP, BIT[2] : Broadcast*/
	u8              u1IpMatchSize;  /* Byte Count for Comparing Dst IP */
	u8              u1PortMask;     /* A bitmask for port matching, BIT[0] : Dest Port need match, BIT[1] : Src Port need match */
	u16             au2DstIp[8];        /* IPv6 Destination IP Address */
	u16             u2SrcPort;      /* UDP Source Port or ICMP Type */
	u16             u2DstPort;      /* UDP Destination Port or ICMP Type */
	u8				Service;        /* Defined Service */
};

enum {
	WAR_ARP_RSP_EN = 0x0000001,
	WAR_ICMPV6_NS_RSP_EN = 0x00000002,
	WAR_ICMPV4_ECHO_RSP_EN = 0x00000004,
	WAR_ICMPV6_ECHO_RSP_EN = 0x00000008,
	WAR_NETBIOS_RSP_EN = 0x00000010,
	WAR_LLMNR_V4_RSP_EN = 0x00000020,
	WAR_LLMNR_V6_RSP_EN = 0x00000040,
	WAR_SNMP_V4_RSP_EN = 0x00000080,
	WAR_SNMP_V6_RSP_EN = 0x00000100,
	WAR_SNMP_V4_WAKEUP_EN = 0x00000200,
	WAR_SNMP_V6_WAKEUP_EN = 0x00000400,
	WAR_SSDP_V4_WAKEUP_EN = 0x00000800,
	WAR_SSDP_V6_WAKEUP_EN = 0x00001000,
	WAR_WSD_V4_WAKEUP_EN = 0x00002000,
	WAR_WSD_V6_WAKEUP_EN = 0x00004000,
	WAR_SLP_V4_WAKEUP_EN = 0x00008000,
	WAR_SLP_V6_WAKEUP_EN = 0x00010000,
	WAR_MDNS_V4_RSP_EN = 0x00020000,
	WAR_MDNS_V6_RSP_EN = 0x00040000,
	WAR_DESIGNATED_MAC_EN = 0x00080000,
	WAR_LLTD_WAKEUP_EN = 0x00100000,
	WAR_ARP_WAKEUP_EN = 0x00200000,
	WAR_MAGIC_WAKEUP_EN = 0x00400000,
	WAR_MDNS_V4_WAKEUP_EN = 0x000800000,
	WAR_MDNS_V6_WAKEUP_EN = 0x001000000
};

#define WAR_MDNS_EN	(WAR_MDNS_V4_RSP_EN |WAR_MDNS_V6_RSP_EN | WAR_MDNS_V4_WAKEUP_EN | WAR_MDNS_V6_WAKEUP_EN)

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
	u8  name[MAX_MDNS_MACHINE_NAME_LEN];
	u8  name_len;
};

struct war_mdns_txt_rsp {
	u8  txt[MAX_MDNS_TXT_LEN];
	u16  txt_len;
};

struct proxy_priv {
	u8 wowlan_war_offload_mode;
	u32 	wowlan_war_offload_ctrl;

//#if defined(CONFIG_OFFLOAD_MDNS_V4) || defined(CONFIG_OFFLOAD_MDNS_V6)
	struct war_mdns_machine_name wowlan_war_offload_mdns_mnane[MAX_MDNS_MACHINE_NAME_NUM];
	struct war_mdns_service_info wowlan_war_offload_mdns_service[MAX_MDNS_SERVICE_NUM];
	struct war_mdns_txt_rsp      wowlan_war_offload_mdns_txt_rsp[MAX_MDNS_TXT_NUM];
	u8     wowlan_war_offload_mdns_mnane_num;
	u8     wowlan_war_offload_mdns_service_info_num;
	u8     wowlan_war_offload_mdns_txt_rsp_num;
	u8     wowlan_war_offload_mdns_domain_name[MAX_MDNS_DOMAIN_NAME_LEN + 1];
	u8     wowlan_war_offload_mdns_domain_name_len;

//#endif /* CONFIG_OFFLOAD_MDNS_V4 || CONFIG_OFFLOAD_MDNS_V6 */
};

extern struct H2C_WAROFFLOAD_PARM WAROffloadParm;
extern struct IPV4_PARM IPv4Parm;
extern struct IPV6_PARM IPv6Parm;

int rtw_war_offload_ctrl(u8 *param_buf);
u8 ParseProtocol(int idx, struct sk_buff *skb);
void GetMDNSInfo(u8 *inbuf, u32 len);
bool OnMDNSv4(int idx, struct sk_buff *skb);
bool OnMDNSv6(int idx, struct sk_buff *skb);
void issue_MDNS_v4_rsp(int idx, u8 *TargetIP, u8 *TargetMAC, struct MDNS_RX_DATA *mdns_rx_data, u8  issue_no);
void issue_MDNS_v6_rsp(int idx, u8 *TargetIP, u8 *TargetMAC, struct MDNS_RX_DATA *mdns_rx_data, u8  issue_no);

#endif
