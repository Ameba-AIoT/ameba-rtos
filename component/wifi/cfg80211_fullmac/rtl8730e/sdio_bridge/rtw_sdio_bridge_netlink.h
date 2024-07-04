#ifndef __RTW_SDIO_BRIDGE_NETLINK_H__
#define __RTW_SDIO_BRIDGE_NETLINK_H__

#define SDIO_BRIDGE_GENL_NAME	"sdiobridge"

enum sdiobridge_api_id {
	CMD_WIFI_CONNECT,
	CMD_WIFI_DISCONNECT,
	CMD_WIFI_SCAN,
	CMD_GET_SCAN_RES,
	CMD_GET_IP,
};

enum sdiobridge_cmd_type {
	BRIDGE_CMD_UNSPEC,
	BRIDGE_CMD_REPLY,	/* kernel space -> user space*/
	BRIDGE_CMD_ECHO,	/* user space -> kernel space*/
};

enum sdiobridge_attr_type {
	BRIDGE_ATTR_UNSPEC,
	BRIDGE_ATTR_API_ID,
	BRIDGE_ATTR_SSID,
	BRIDGE_ATTR_PWD,
	BRIDGE_ATTR_SCAN_RES_ADDR,
	BRIDGE_ATTR_API_RET,
};

#endif
