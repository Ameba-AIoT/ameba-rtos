#ifndef __RTW_SDIO_BRIDGE_NETLINK_H__
#define __RTW_SDIO_BRIDGE_NETLINK_H__

#define BRIDGE_GENL_NAME	"bridge"

enum bridge_api_id {
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

enum bridge_cmd_type {
	BRIDGE_CMD_UNSPEC,
	BRIDGE_CMD_REPLY,	/* kernel space -> user space*/
	BRIDGE_CMD_ECHO,	/* user space -> kernel space*/
	BRIDGE_CMD_EVENT,	/* kernel space notify user space about wifi events*/
	BRIDGE_CMD_CUSTOM_API,	/* Custom API */
};

enum bridge_attr_type {
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

enum bridge_tickps_cmd_subtype {
	BRIDGE_CMD_TICKPS_R = 0,
	BRIDGE_CMD_TICKPS_A = 1,
	BRIDGE_CMD_TICKPS_TYPE_CG = 2,
	BRIDGE_CMD_TICKPS_TYPE_PG = 3,
};

/* for test demo */
#define WHC_WIFI_TEST 0xffa5a5a5
#define WHC_ATCMD_TEST 0xeea5a5a5

#define WHC_WIFI_TEST_GET_MAC_ADDR 0x1
#define WHC_WIFI_TEST_GET_IP       0x2
#define WHC_WIFI_TEST_SET_READY    0x3
#define WHC_WIFI_TEST_SET_UNREADY    0x4
#define WHC_WIFI_TEST_SET_TICKPS_CMD 0x5

extern struct genl_family whc_bridge_nl_family;
#endif
