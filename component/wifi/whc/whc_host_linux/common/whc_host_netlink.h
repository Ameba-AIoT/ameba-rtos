#ifndef __WHC_HOST_NETLINK_H__
#define __WHC_HOST_NETLINK_H__

#define WHC_CMD_GENL_NAME	"whc_ctrl_app"

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
};

enum whc_cmd_type {
	WHC_CMD_UNSPEC,
	WHC_CMD_REPLY,	/* kernel space -> user space*/
	WHC_CMD_ECHO,	/* user space -> kernel space*/
	WHC_CMD_EVENT,	/* kernel space notify user space about wifi events*/
	WHC_CMD_CUSTOM_API,	/* Custom API */
};

enum whc_attr_type {
	BRIDGE_ATTR_UNSPEC,
	BRIDGE_ATTR_API_ID,
	BRIDGE_ATTR_WLAN_IDX,
	BRIDGE_ATTR_MAC,
	BRIDGE_ATTR_PAYLOAD,
	BRIDGE_ATTR_CHUNK_INDEX,
	BRIDGE_ATTR_LAST_CHUNK,
	BRIDGE_ATTR_AFTER_LAST,
	NUM_BRIDGE_ATTR = BRIDGE_ATTR_AFTER_LAST,
	BRIDGE_ATTR_MAX = BRIDGE_ATTR_AFTER_LAST - 1,
};

enum whc_tickps_cmd_subtype {
	WHC_CMD_TICKPS_R = 0,
	WHC_CMD_TICKPS_A = 1,
	WHC_CMD_TICKPS_TYPE_CG = 2,
	WHC_CMD_TICKPS_TYPE_PG = 3,
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

extern struct genl_family whc_nl_family;
#endif
