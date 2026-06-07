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
	API_GET_MAC_ADDR,
	API_GET_IP,
	API_SET_HOST_STATE,
	API_WIFI_SET_MAC,
	API_WIFI_NETIF_ON,
	API_WIFI_SEND_TO_DEV,
	API_WIFI_INFO_INIT,
	API_WIFI_CUSTOM_BASE,
	API_WIFI_DO_SCAN,
	API_WIFI_SCAN_RESULT,
	API_WIFI_MP,
	API_WIFI_DBG,
	API_WIFI_GET_JOIN_EVENT,
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

// todo: need sync, default 4k, however max buf in sdio & spi < 1.5k
#define WHC_WIFI_MP_MSG_BUF_SIZE (4096)

extern struct genl_family whc_nl_family;

#endif
