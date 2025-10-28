#ifndef __WHC_HOST_NETLINK_H__
#define __WHC_HOST_NETLINK_H__

/* max GENL_NAMSIZ: 16 - 1 */
#define WHC_CMD_GENL_NAME	"whc_demo"

struct genl_info;
typedef int (*netlink_cmd_handler_t)(struct genl_info *info);

struct whc_host_netlink_command_entry {
	char api_id;
	netlink_cmd_handler_t handler;
};

struct whc_netlink {
	int sockfd;
	int family_id;
};

enum whc_cmd_api_id {
	CMD_GET_MAC_ADDR,
	CMD_GET_IP,
	CMD_SET_HOST_STATE,
	CMD_WIFI_SET_MAC,
	CMD_WIFI_NETIF_ON,
	CMD_WIFI_SEND_BUF,
	CMD_WIFI_INFO_INIT,
	CMD_WIFI_CUSTOM_BASE,
	CMD_WIFI_DO_SCAN,
	CMD_WIFI_SCAN_RESULT,
	CMD_WIFI_MP,
	CMD_WIFI_DBG,
	CMD_WIFI_GET_JOIN_EVENT,
};

enum whc_cmd_type {
	WHC_CMD_UNSPEC,
	WHC_CMD_REPLY,	/* kernel space -> user space*/
	WHC_CMD_ECHO,	/* user space -> kernel space*/
	WHC_CMD_EVENT,	/* kernel space notify user space about wifi events*/
	WHC_CMD_CUSTOM_API,	/* Custom API */
};

enum whc_attr_type {
	WHC_ATTR_UNSPEC,
	WHC_ATTR_API_ID,
	WHC_ATTR_WLAN_IDX,
	WHC_ATTR_STRING,
	WHC_ATTR_PAYLOAD,
	WHC_ATTR_CHUNK_INDEX,
	WHC_ATTR_LAST_CHUNK,
	WHC_ATTR_AFTER_LAST,
	NUM_WHC_ATTR = WHC_ATTR_AFTER_LAST,
	WHC_ATTR_MAX = WHC_ATTR_AFTER_LAST - 1,
};

enum nl80211_multicast_groups {
	WHC_MCGRP_EVENT,
};

/* for test demo */
#define WHC_WIFI_TEST 0xffa5a5a5
#define WHC_ATCMD_TEST 0xeea5a5a5

#define WHC_WIFI_TEST_GET_MAC_ADDR   0x1
#define WHC_WIFI_TEST_GET_IP         0x2
#define WHC_WIFI_TEST_SET_READY      0x3
#define WHC_WIFI_TEST_SET_UNREADY    0x4
#define WHC_WIFI_TEST_SET_TICKPS_CMD 0x5
#define WHC_WIFI_TEST_CONNECT        0x6
#define WHC_WIFI_TEST_SCAN           0x7
#define WHC_WIFI_TEST_DHCP           0x8
#define WHC_WIFI_TEST_WIFION         0x9
#define WHC_WIFI_TEST_SCAN_RESULT    0xA
#define WHC_WIFI_TEST_MP             0xB
#define WHC_WIFI_TEST_DBG            0xC

// todo: need sync, default 4k, however max buf in sdio & spi < 1.5k
#define WHC_WIFI_MP_MSG_BUF_SIZE (4096)

extern struct genl_family whc_nl_family;
#endif
