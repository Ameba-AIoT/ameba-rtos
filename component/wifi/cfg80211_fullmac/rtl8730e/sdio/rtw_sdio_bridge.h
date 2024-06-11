#ifndef __RTW_SDIO_BRIDGE_H__
#define __RTW_SDIO_BRIDGE_H__

struct bridge_cmd_entry {
	const char *bridge_cmd;
	int (*func)(char *param_buf);
};


int llhw_sdio_bridge_connect(char *param);
int llhw_sdio_bridge_disconnect(char *param);
int llhw_sdio_bridge_scan(char *param);
int llhw_sdio_bridge_dhcp(char *param);

#endif
