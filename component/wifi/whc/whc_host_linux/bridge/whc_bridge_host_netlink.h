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
};

enum bridge_cmd_type {
	BRIDGE_CMD_UNSPEC,
	BRIDGE_CMD_REPLY,	/* kernel space -> user space*/
	BRIDGE_CMD_ECHO,	/* user space -> kernel space*/
	BRIDGE_CMD_EVENT,	/* kernel space notify user space about wifi events*/
};

enum bridge_attr_type {
	BRIDGE_ATTR_UNSPEC,
	BRIDGE_ATTR_API_ID,
	BRIDGE_ATTR_WLAN_IDX,
	BRIDGE_ATTR_MAC,
	BRIDGE_ATTR_PAYLOAD,

};


/* for test demo */
#define WHC_WIFI_TEST 0xffa5a5a5

#define WHC_WIFI_TEST_GET_MAC_ADDR 0x1
#define WHC_WIFI_TEST_GET_IP       0x2
#define WHC_WIFI_TEST_SET_READY    0x3

#endif
