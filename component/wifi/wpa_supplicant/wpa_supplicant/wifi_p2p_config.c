#include "utils/os.h"
#include <lwipconf.h>
#include "lwip_netconf.h"
#include <wifi_api.h>
#include "wps/wps_defs.h"
#include "wifi_p2p_config.h"
#include "wifi_p2p_supplicant.h"
#include "wifi_intf_drv_to_upper.h"

/*NETMASK*/
#define P2P_NETMASK_ADDR0   255
#define P2P_NETMASK_ADDR1   255
#define P2P_NETMASK_ADDR2   255
#define P2P_NETMASK_ADDR3   0

/*Gateway Address*/
#define P2P_GW_ADDR0   192
#define P2P_GW_ADDR1   168
#define P2P_GW_ADDR2   42
#define P2P_GW_ADDR3   1

xqueue_handle_t queue_for_p2p_nego;

int wifi_init_ap(void);
void dhcps_init(struct netif *pnetif);
void dhcps_deinit(void);

int wifi_p2p_start_go(char *ssid, char *passphrase, u8 channel)
{
	extern struct netif xnetif[NET_IF_NUM];
	struct netif *pnetif = &xnetif[NETIF_WLAN_AP_INDEX];
	struct rtw_softap_info softAP_config = {0};
	u8 bk_addr[ETH_ALEN];
	u32 addr = CONCAT_TO_UINT32(P2P_GW_ADDR0, P2P_GW_ADDR1, P2P_GW_ADDR2, P2P_GW_ADDR3);
	u32 netmask = CONCAT_TO_UINT32(P2P_NETMASK_ADDR0, P2P_NETMASK_ADDR1, P2P_NETMASK_ADDR2, P2P_NETMASK_ADDR3);
	u32 gw = CONCAT_TO_UINT32(P2P_GW_ADDR0, P2P_GW_ADDR1, P2P_GW_ADDR2, P2P_GW_ADDR3);

	/* Stop AP */
	dhcps_deinit();
	wifi_stop_ap();

	memcpy(softAP_config.ssid.val, ssid, strlen(ssid));
	softAP_config.ssid.len = strlen(ssid);
	softAP_config.password = (u8 *)passphrase;
	softAP_config.password_len = strlen(passphrase);
	softAP_config.security_type = RTW_SECURITY_WPA2_AES_PSK;
	softAP_config.channel = channel;

	/* Init go, because switch port0&port1 mac addr need to init mac addr firstly */
	wifi_init_ap();

	/* Switch port0 & port1 mac addr */
	memcpy(bk_addr, xnetif[STA_WLAN_INDEX].hwaddr, ETH_ALEN);
	wifi_set_mac_address(0, xnetif[SOFTAP_WLAN_INDEX].hwaddr, 0);
	wifi_set_mac_address(1, bk_addr, 0);

	/* Start GO */
	if (wifi_start_ap(&softAP_config) != RTK_SUCCESS) {
		RTK_LOGW(NOTAG, "[+WLP2PGO] wifi_start_ap failed\r\n");
		return -1;
	}

	LwIP_SetIP(NETIF_WLAN_AP_INDEX, addr, netmask, gw);

	/* start dhcp server */
	dhcps_init(pnetif);

	return 0;
}

void cmd_wifi_p2p_start(void)
{
	int listen_ch = 1;
	int op_ch = 5;
	int go_intent = 1;
	u32 r = 0;

	r = _rand();
	go_intent = r % 15 + 1; /*1-15*/

	/* ch1,6,11 for 2.4G. ch36,40,44,48 for 5G */
#ifdef CONFIG_P2P_5G
	listen_ch = 40;
	op_ch = 40;
#else
	r = _rand();
	listen_ch = 1 + (r % 3) * 5;

	r = _rand();
	op_ch = 1 + (r % 3) * 5;
#endif

	wifi_p2p_init(LwIP_GetMAC(0), go_intent, listen_ch, op_ch);
}

void cmd_p2p_listen(int argc, char **argv)
{
	u32 timeout = 0;

	if (argc == 2) {
		timeout = os_atoi(argv[1]);
		RTK_LOGA(NOTAG, "\r\n%s(): timeout=%d\n", __func__, timeout);
		if (timeout > 3600) {
			timeout = 3600;
		}
	}
	wifi_cmd_p2p_listen(timeout);
}

void cmd_p2p_find(int argc, char **argv)
{
	u32 timeout = 30; //seconds

	if (argc == 2) {
		timeout = os_atoi(argv[1]);
		RTK_LOGA(NOTAG, "\r\n%s(): timeout=%d\n", __func__, timeout);
	}

	wifi_cmd_p2p_find(timeout);
}

void cmd_p2p_connect(int argc, char **argv)
{
	enum p2p_wps_method config_method = WPS_PBC;
	char *pin = NULL;
	u8 dest[ETH_ALEN];
	u8 *res = NULL;
	int ret = 0, result = 0;

	(void) argc;

	sscanf(argv[1], MAC_FMT, (int *)&dest[0], (int *)&dest[1], (int *)&dest[2],
		   (int *)&dest[3], (int *)&dest[4], (int *)&dest[5]);

	if (os_strncmp(argv[2], "pbc", 3) == 0) {
		config_method = WPS_PBC;
	} else if (os_strncmp(argv[2], "pin", 3) == 0) {
		if (argv[3]) {
			config_method = WPS_PIN_KEYPAD;
			pin = argv[3];
		} else {
			config_method = WPS_PIN_DISPLAY;
		}
	} else {
		RTK_LOGA(NOTAG, "Unknown config method!\n");
		return;
	}

	res = rtos_mem_zmalloc(sizeof(struct p2p_go_neg_results));
	if (!res) {
		return;
	}

	if (queue_for_p2p_nego != NULL) {
		os_xqueue_delete(queue_for_p2p_nego);
		queue_for_p2p_nego = NULL;
	}
	queue_for_p2p_nego = os_xqueue_create(1, sizeof(struct p2p_go_neg_results));
	if (queue_for_p2p_nego != NULL) {
		ret = wifi_cmd_p2p_connect(dest, config_method, pin);
		if (ret == 0) {
			result = os_xqueue_receive(queue_for_p2p_nego, res, 15);
		}

		os_xqueue_delete(queue_for_p2p_nego);
		queue_for_p2p_nego = NULL;

		if ((ret == 0) && (result == RTK_SUCCESS)) {
			wifi_p2p_start_wps(res);
		}
	}

	if (res) {
		rtos_mem_free(res);
	}
}

