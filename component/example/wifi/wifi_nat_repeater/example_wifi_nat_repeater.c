#include <platform_autoconf.h>

#include "example_wifi_nat_repeater.h"
#include "rtw_wifi_constants.h"
#include "wifi_intf_drv_to_app_basic.h"
#include "lwip_netconf.h"
#include "wifi_conf.h"
#include "dhcp/dhcps.h"
#include "main.h"

#define NAT_AP_IP_ADDR0 192
#define NAT_AP_IP_ADDR1 168
#define NAT_AP_IP_ADDR2 43
#define NAT_AP_IP_ADDR3 1

#define NAT_AP_NETMASK_ADDR0 255
#define NAT_AP_NETMASK_ADDR1 255
#define NAT_AP_NETMASK_ADDR2 255
#define NAT_AP_NETMASK_ADDR3 0

#define NAT_AP_GW_ADDR0 192
#define NAT_AP_GW_ADDR1 168
#define NAT_AP_GW_ADDR2 43
#define NAT_AP_GW_ADDR3 1

extern struct netif xnetif[NET_IF_NUM];

extern void dns_relay_service_init(void);
extern void ip_nat_reinitialize(void);
extern void ip_nat_sync_dns_serever_data(void);

int wifi_repeater_ap_config_complete = 0;

static struct _rtw_softap_info_t rptap = {0};
char *rptssid = "AmebaRPT";
char *rptpassword = "12345678";	// NULL for RTW_SECURITY_OPEN
unsigned char rptchannel = 6;
static const char *const TAG = "WIFI_NAT_REPEATER";

static int ip_nat_wifi_restart_ap(struct _rtw_softap_info_t *softAP_config)
{
	unsigned char idx = 0;
	u32 addr;
	u32 netmask;
	u32 gw;
	int timeout = 20;

	if (wifi_is_running(WLAN1_IDX)) {
		idx = 1;
	}

	// stop dhcp server
	dhcps_deinit();
	addr = CONCAT_TO_UINT32(GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	netmask = CONCAT_TO_UINT32(NAT_AP_NETMASK_ADDR0, NAT_AP_NETMASK_ADDR1, NAT_AP_NETMASK_ADDR2, NAT_AP_NETMASK_ADDR3);
	gw = CONCAT_TO_UINT32(GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	LwIP_SetIP(SOFTAP_WLAN_INDEX, addr, netmask, gw);

	wifi_stop_ap();

	RTK_LOGI(TAG, "%s(%d)caller=%x\n", __FUNCTION__, __LINE__, __builtin_return_address(0));
	RTK_LOGI(TAG, "ssidlen=%d\n", softAP_config->ssid.len);
	RTK_LOGI(TAG, "ssid=%s\n", softAP_config->ssid.val);
	RTK_LOGI(TAG, "channel=%d\n", softAP_config->channel);
	RTK_LOGI(TAG, "security_type=0x%x\n", softAP_config->security_type);
	RTK_LOGI(TAG, "pwdlen=%d\n", softAP_config->password_len);
	RTK_LOGI(TAG, "password=%s\n", softAP_config->password);

	// start ap
	if (wifi_start_ap(softAP_config) < 0) {
		RTK_LOGI(TAG, "\n\rERROR: Operation failed!");
		return -1;
	}

	while (1) {
		struct _rtw_wifi_setting_t softapsetting;
		wifi_get_setting(SOFTAP_WLAN_INDEX, &softapsetting);
		if (strlen((const char *)softapsetting.ssid) > 0) {
			if (strcmp((const char *) softapsetting.ssid, (const char *)softAP_config->ssid.val) == 0) {
				RTK_LOGI(TAG, "\n\r%s started\n", softAP_config->ssid.val);
				break;
			}
		}

		if (timeout == 0) {
			RTK_LOGI(TAG, "\n\r%s(%d)ERROR: Start AP timeout!\n", __FUNCTION__, __LINE__);
			return -1;
		}

		rtos_time_delay_ms(1 * RTOS_TICK_RATE_HZ);
		timeout --;
	}

	addr = CONCAT_TO_UINT32(NAT_AP_IP_ADDR0, NAT_AP_IP_ADDR1, NAT_AP_IP_ADDR2, NAT_AP_IP_ADDR3);
	netmask = CONCAT_TO_UINT32(NAT_AP_NETMASK_ADDR0, NAT_AP_NETMASK_ADDR1, NAT_AP_NETMASK_ADDR2, NAT_AP_NETMASK_ADDR3);
	gw = CONCAT_TO_UINT32(NAT_AP_GW_ADDR0, NAT_AP_GW_ADDR1, NAT_AP_GW_ADDR2, NAT_AP_GW_ADDR3);
	LwIP_SetIP(SOFTAP_WLAN_INDEX, addr, netmask, gw);

	// start dhcp server
	RTK_LOGI(TAG, "%s(%d)idx=%d\n", __FUNCTION__, __LINE__, idx);
	dhcps_init(&xnetif[idx]);

	return 0;
}

static int ip_nat_avoid_confliction_ip(void)
{
	unsigned char *myLocalMask;
	unsigned char *myLocalIp;
	unsigned char *wanMask;
	unsigned char *wanIp;
	struct ip_addr set_ipaddr;
	struct ip_addr set_netmask;
	struct ip_addr set_gw;
	uint8_t iptab[4];
	char *strtmp = NULL;
	char tmp1[64] = {0};
	struct in_addr inIp, inMask;
	struct in_addr myIp, myMask;
	unsigned int inIpVal, inMaskVal, myIpVal, myMaskVal, maskVal;
	char tmpBufIP[64] = {0}, tmpBufMask[64] = {0};

	wanMask = LwIP_GetMASK(WLAN0_IDX);
	wanIp =  LwIP_GetIP(WLAN0_IDX);

	inIp.s_addr = *((unsigned int *) wanIp);
	inMask.s_addr = *((unsigned int *) wanMask);

	memcpy(&inIpVal, &inIp, 4);
	memcpy(&inMaskVal, &inMask, 4);

	myLocalMask = LwIP_GetMASK(WLAN1_IDX);
	myLocalIp =  LwIP_GetIP(WLAN1_IDX);

	myIp.s_addr = *((unsigned int *) myLocalIp);
	myMask.s_addr = *((unsigned int *) myLocalMask);
	memcpy(&myIpVal, &myIp, 4);
	memcpy(&myMaskVal, &myMask, 4);
	memcpy(&maskVal, myMaskVal > inMaskVal ? &inMaskVal : &myMaskVal, 4);

	RTK_LOGI(TAG, "%s(%d)inIpVal=%x,myIpVal=%x,maskVal=%x\n", __FUNCTION__, __LINE__, inIpVal, myIpVal, maskVal);

	if ((inIpVal & maskVal) == (myIpVal & maskVal)) { //wan ip conflict lan ip
		int i = 0, j = 0;

		for (i = 0; i < 32; i++) {
			if ((maskVal & htonl(1 << i)) != 0) {
				break;
			}
		}

		if ((myIpVal & htonl(1 << i)) == 0) {
			myIpVal = myIpVal | htonl(1 << i);
		} else {
			myIpVal = myIpVal & htonl(~(1 << i));
		}

		memcpy(&myIp, &myIpVal, 4);

		for (j = 0; j < 32; j++) {
			if ((maskVal & htonl(1 << j)) != 0) {
				break;
			}
		}

		memset(tmp1, 0x00, sizeof(tmp1));
		memcpy(tmp1, &myIpVal,  4);
		strtmp = inet_ntoa(*((struct in_addr *)tmp1));
		sprintf(tmpBufIP, "%s", strtmp);

		memset(tmp1, 0x00, sizeof(tmp1));
		memcpy(tmp1, &myMaskVal,  4);
		strtmp = inet_ntoa(*((struct in_addr *)tmp1));
		sprintf(tmpBufMask, "%s", strtmp);

		iptab[0] = (uint8_t)(myIpVal);
		iptab[1] = (uint8_t)(myIpVal >> 8);
		iptab[2] = (uint8_t)(myIpVal >> 16);
		iptab[3] = (uint8_t)(myIpVal >> 24);
		IP4_ADDR(ip_2_ip4(&set_ipaddr), iptab[0], iptab[1], iptab[2], iptab[3]);

		iptab[0] = (uint8_t)(myMaskVal);
		iptab[1] = (uint8_t)(myMaskVal >> 8);
		iptab[2] = (uint8_t)(myMaskVal >> 16);
		iptab[3] = (uint8_t)(myMaskVal >> 24);
		IP4_ADDR(ip_2_ip4(&set_netmask), iptab[0], iptab[1], iptab[2], iptab[3]);

		iptab[0] = (uint8_t)(myIpVal);
		iptab[1] = (uint8_t)(myIpVal >> 8);
		iptab[2] = (uint8_t)(myIpVal >> 16);
		iptab[3] = (uint8_t)(myIpVal >> 24);
		IP4_ADDR(ip_2_ip4(&set_gw), iptab[0], iptab[1], iptab[2], iptab[3]);
		netif_set_addr(&xnetif[1], ip_2_ip4(&set_ipaddr), ip_2_ip4(&set_netmask), ip_2_ip4(&set_gw));

		struct _rtw_wifi_setting_t setting;
		wifi_get_setting(WLAN1_IDX, &setting);

		struct _rtw_softap_info_t softAP_config = {0};
		softAP_config.ssid.len = strlen((char *)setting.ssid);
		memcpy(softAP_config.ssid.val, setting.ssid, softAP_config.ssid.len);
		softAP_config.password = setting.password;
		softAP_config.password_len = strlen((char *)setting.password);
		softAP_config.security_type = setting.security_type;
		softAP_config.channel = setting.channel;

		RTK_LOGI(TAG, "ssidlen=%d\n", softAP_config.ssid.len);
		RTK_LOGI(TAG, "ssid=%s\n", softAP_config.ssid.val);
		RTK_LOGI(TAG, "channel=%d\n", softAP_config.channel);
		RTK_LOGI(TAG, "security_type=0x%x\n", softAP_config.security_type);
		RTK_LOGI(TAG, "pwdlen=%d\n", softAP_config.password_len);
		RTK_LOGI(TAG, "password=%s\n", softAP_config.password);

		if (ip_nat_wifi_restart_ap(&softAP_config) < 0) {
			RTK_LOGI(TAG, "\n\rERROR: wifi_restart_ap Operation failed!");
		}
	}

	return 0;
}

static void poll_ip_changed_thread(void *param)
{
	(void) param;
	unsigned int oldip, newip;
	memcpy(&oldip, LwIP_GetIP(WLAN0_IDX), 4);

	while (1) {
		memcpy(&newip, LwIP_GetIP(WLAN0_IDX), 4);
		if (0x0 == newip) {
			goto nextcheck;
		}

		if (1 == wifi_repeater_ap_config_complete && oldip != newip) {
			RTK_LOGI(TAG, "%s(%d)oldip=%x,newip=%x\n", __FUNCTION__, __LINE__, oldip, newip);
			ip_nat_reinitialize();
			ip_nat_sync_dns_serever_data();
			ip_nat_avoid_confliction_ip();

			oldip = newip;
		}

nextcheck:
		rtos_time_delay_ms(1000);
	}

	rtos_task_delete(NULL);
}

/**
 * @brief  Wi-Fi example for mode switch case: Mode switching between concurrent mode and STA and add to bridge.
 * @note  Process Flow:
 *              - Disable Wi-Fi
 *              - Enable Wi-Fi with concurrent (STA + AP) mode
 *              - Start AP
 *              - Check AP running
 *              - Connect to AP using STA mode
 */
static void example_wlan_repeater_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

	// Wait for other task stable.
	rtos_time_delay_ms(4000);

	u32 ip_addr;
	u32 netmask;
	u32 gw;

	/*********************************************************************************
	*	1. Start AP
	*********************************************************************************/
	RTK_LOGI(TAG, "\n\r[WLAN_REPEATER_EXAMPLE] Start AP\n");

	rptap.ssid.len = strlen(rptssid);
	strncpy((char *)rptap.ssid.val, (char *)rptssid, sizeof(rptap.ssid.val) - 1);
	rptap.password = (unsigned char *)rptpassword;
	rptap.password_len = strlen(rptpassword);
	rptap.channel = rptchannel;

	if (rptap.password) {
		rptap.security_type = RTW_SECURITY_WPA2_AES_PSK;
	} else {
		rptap.security_type = RTW_SECURITY_OPEN;
	}

	RTK_LOGI(TAG, "rptap.ssidlen=%d\n", rptap.ssid.len);
	RTK_LOGI(TAG, "rptap.ssid=%s\n", rptap.ssid.val);
	RTK_LOGI(TAG, "rptap.channel=%d\n", rptap.channel);
	RTK_LOGI(TAG, "rptap.security_type=0x%x\n", rptap.security_type);
	RTK_LOGI(TAG, "rptap.pwdlen=%d\n", rptap.password_len);
	RTK_LOGI(TAG, "rptap.password=%s\n", rptap.password);

	if (wifi_start_ap(&rptap) < 0) {
		RTK_LOGI(TAG, "\n\r[WLAN_REPEATER_EXAMPLE] ERROR: wifi_start_ap failed\n");
		return;
	}

	/*********************************************************************************
	*	2. Check AP running
	*********************************************************************************/
	RTK_LOGI(TAG, "\n\r[WLAN_REPEATER_EXAMPLE] Check AP running\n");
	int timeout = 20;
	while (1) {
		struct _rtw_wifi_setting_t setting;
		wifi_get_setting(SOFTAP_WLAN_INDEX, &setting);
		if (strlen((const char *)setting.ssid) > 0) {
			if (strcmp((const char *) setting.ssid, (const char *)rptap.ssid.val) == 0) {
				RTK_LOGI(TAG, "\n\r[WLAN_REPEATER_EXAMPLE] %s started\n", rptap.ssid.val);
				break;
			}
		}
		if (timeout == 0) {
			RTK_LOGI(TAG, "\n\r[WLAN_REPEATER_EXAMPLE] ERROR: Start AP timeout\n");
			return;
		}
		rtos_time_delay_ms(1 * RTOS_TICK_RATE_HZ);
		timeout --;
	}

	ip_addr = CONCAT_TO_UINT32(NAT_AP_IP_ADDR0, NAT_AP_IP_ADDR1, NAT_AP_IP_ADDR2, NAT_AP_IP_ADDR3);
	netmask = CONCAT_TO_UINT32(NAT_AP_NETMASK_ADDR0, NAT_AP_NETMASK_ADDR1, NAT_AP_NETMASK_ADDR2, NAT_AP_NETMASK_ADDR3);
	gw = CONCAT_TO_UINT32(NAT_AP_GW_ADDR0, NAT_AP_GW_ADDR1, NAT_AP_GW_ADDR2, NAT_AP_GW_ADDR3);
	LwIP_SetIP(SOFTAP_WLAN_INDEX, ip_addr, netmask, gw);

	RTK_LOGI(TAG, "\n\r[WLAN_REPEATER_EXAMPLE] Start DHCP server\n");
	dhcps_init(&xnetif[1]);
	rtos_time_delay_ms(1000);

	wifi_repeater_ap_config_complete = 1;

	rtos_task_delete(NULL);
}

void example_nat_repeater(void)
{
	wifi_repeater_ap_config_complete = 0;

	if (rtos_task_create(NULL, ((const char *)"example_wlan_repeater_thread"), example_wlan_repeater_thread, NULL, 1024 * 4, 1) != SUCCESS) {
		RTK_LOGI(TAG, "\n\r%s rtos_task_create failed\n", __FUNCTION__);
	}
	if (rtos_task_create(NULL, ((const char *)"poll_ip_changed_thread"), poll_ip_changed_thread, NULL, 1024 * 4, 1) != SUCCESS) {
		RTK_LOGI(TAG, "\n\r%s rtos_task_create failed\n", __FUNCTION__);
	}

	dns_relay_service_init();
}
