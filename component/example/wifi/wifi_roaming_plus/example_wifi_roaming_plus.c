#include <rtw_autoconf.h>
#include "platform_stdlib.h"
#include "basic_types.h"
#include <wifi_conf.h>
#include <lwip_netconf.h>
#include "flash_api.h"
#include "wifi_fast_connect.h"
#include "example_wifi_roaming_plus.h"
#include "kv.h"

#define PING_TEST_DATA_SIZE_MAX	10000
#define RSSI_SCAN_THRESHOLD -65	//when current ap's rssi < RSSI_SCAN_THRESHOLD, start to scan a better ap.
#define RSSI_ROAMING_THRESHOLD -70	//when current ap's rssi < RSSI_ROAMING_THRESHOLD, start connect to an other better ap.
#define FIND_BETTER_RSSI_DELTA 8	//target ap's rssi - current ap's rssi > FIND_BETTER_RSSI_DELTA
#define PRE_SCAN	0
#define SUPPORT_SCAN_5G_CHANNEL 0
#define MAX_CH_NUM 13		// config the max channel number store in flash
#define MAX_AP_NUM 16		// config the max ap number store in flash
#define AP_VALID_TIME 60	//the valid time of target ap which found during pre scan
#define ROAMING_PLUS_DBG 0		//for debug log
#define SCAN_BUFLEN 500 	//each scan list length= 14 + ssid_length(32MAX). so SCAN_BUFLEN should be AP_NUM*(14+32) at least

#if (!PRE_SCAN)
#undef RSSI_SCAN_THRESHOLD
#define RSSI_SCAN_THRESHOLD RSSI_ROAMING_THRESHOLD
#endif

#if ROAMING_PLUS_DBG
#define ROAMING_DBG	printf
#else
#define ROAMING_DBG
#endif

//user should config channel plan
typedef struct channel_plan {
#if SUPPORT_SCAN_5G_CHANNEL
	u8 channel[39];
#else
	u8 channel[14];
#endif
	u8	len;
} channel_plan_t;
#if SUPPORT_SCAN_5G_CHANNEL
channel_plan_t roaming_channel_plan = {{
		1, 6, 11, 149, 153, 157, 161, 165, 2, 3, 4, 5, 7, 8, 9, 10, 12, 13, 36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108,
		112, 116, 120, 124, 128, 132, 136, 140, 144
	}, 38
};
#else
channel_plan_t roaming_channel_plan = {{1, 6, 11, 2, 3, 4, 5, 7, 8, 9, 10, 12, 13}, 13};
#endif

typedef struct wifi_roaming_ap {
	u8 	ssid[33];
	u8 	bssid[ETH_ALEN];
	u8	channel;
	rtw_security_t		security_type;
	u8 	password[65];
	u8	key_idx;
	s32	rssi;
#ifdef CONFIG_LWIP_LAYER
	u8	ip[4];
#endif
} wifi_roaming_ap_t;

//ext info in flash
struct ap_additional_info {
	u8 ap_bssid[ETH_ALEN];
	uint32_t sta_ip;
} ap_additional_info_t;

//info in flash
struct wifi_roaming_data {
	struct wlan_fast_reconnect ap_info;
	u8 num;
	u8 ap_n;
	u32 channel[MAX_CH_NUM];
	struct ap_additional_info add_ap_info[MAX_AP_NUM];
} wifi_roaming_data_t;

enum {
	FAST_CONNECT_SPECIFIC_CH = 0,
	FAST_CONNECT_ALL_CH  = 1
};

#ifdef CONFIG_LWIP_LAYER
extern struct netif xnetif[NET_IF_NUM];
#endif
static wifi_roaming_ap_t *ap_list;
static u8 pscan_enable = _TRUE; // if set _TRUE, please set pscan_channel_list
static u8 pscan_channel_list[] = {1}; // set by customer
static unsigned short ping_seq = 0;
#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
extern uint32_t offer_ip;
#endif

static int  wifi_write_ap_info_to_flash_ext(u8 *data, u32 len);
int  wifi_write_ap_info_to_flash(u32 offer_ip, u32 server_ip);

static void generate_ping_echo(unsigned char *buf, int size)
{
	int i;
	struct icmp_echo_hdr *pecho;

	for (i = 0; i < size; i ++) {
		buf[sizeof(struct icmp_echo_hdr) + i] = (unsigned char) i;
	}

	pecho = (struct icmp_echo_hdr *) buf;
	ICMPH_TYPE_SET(pecho, ICMP_ECHO);
	ICMPH_CODE_SET(pecho, 0);
	pecho->chksum = 0;
	pecho->id = 0xABCD;
	pecho->seqno = htons(++ ping_seq);

	//Checksum includes icmp header and data. Need to calculate after fill up icmp header
	pecho->chksum = inet_chksum(pecho, sizeof(struct icmp_echo_hdr) + size);
}

static int roaming_ping_test(u32_t ip_addr)
{
	int i, ping_socket;
	int pint_timeout = 200;
	struct sockaddr_in to_addr, from_addr;
	struct icmp_echo_hdr *pecho;
	int from_addr_len = sizeof(struct sockaddr);
	int ping_size, reply_size;
	unsigned char *ping_buf, *reply_buf;
	unsigned int ping_time, reply_time;
	struct ip_hdr *iphdr;
	int ping_count = 1;
	int data_size = 32;
	//int ping_interval = 1;
	u32 ping_flag = 0;

	if (data_size > PING_TEST_DATA_SIZE_MAX) {
		printf("\n\r[Wifi roaming plus][ERROR]: data size error, can't exceed %d", PING_TEST_DATA_SIZE_MAX);
		return 1;
	}
	ping_size = sizeof(struct icmp_echo_hdr) + data_size;
	ping_buf = rtos_mem_malloc(ping_size);
	if (NULL == ping_buf) {
		printf("\n\r[Wifi roaming plus][ERROR] : Allocate ping_buf failed");
		return 1;
	}
	reply_buf = rtos_mem_malloc(ping_size);
	if (NULL == reply_buf) {
		rtos_mem_free(ping_buf);
		printf("\n\r[Wifi roaming plus][ERROR] : Allocate reply_buf failed");
		return 1;
	}

	ROAMING_DBG("\n\r[%s] PING %d.%d.%d.%d %d(%d) bytes of data\n", __FUNCTION__, (uint8_t)(ip_addr), (uint8_t)(ip_addr >> 8), (uint8_t)(ip_addr >> 16),
				(uint8_t)(ip_addr >> 24), \
				data_size, sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr) + data_size);

	for (i = 0; (i < ping_count) ; i ++) {
		ping_socket = socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP);
		/* lwip 1.4.1 */
		setsockopt(ping_socket, SOL_SOCKET, SO_RCVTIMEO, &pint_timeout, sizeof(pint_timeout));

		to_addr.sin_len = sizeof(to_addr);
		to_addr.sin_family = AF_INET;
		to_addr.sin_addr.s_addr = ip_addr;

		generate_ping_echo(ping_buf, data_size);
		sendto(ping_socket, ping_buf, ping_size, 0, (struct sockaddr *) &to_addr, sizeof(to_addr));
		ping_time = rtos_time_get_current_system_time_ms();

		if ((reply_size = recvfrom(ping_socket, reply_buf, ping_size, 0, (struct sockaddr *) &from_addr, (socklen_t *) &from_addr_len))
			>= (int)(sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr)) && (from_addr.sin_addr.s_addr == to_addr.sin_addr.s_addr)) {
			reply_time = rtos_time_get_current_system_time_ms();

			iphdr = (struct ip_hdr *)reply_buf;
			pecho = (struct icmp_echo_hdr *)(reply_buf + (IPH_HL(iphdr) * 4));

			if ((pecho->id == 0xABCD) && (pecho->seqno == htons(ping_seq))) {
				ROAMING_DBG("\n\r[%s] %d bytes from %s: icmp_seq=%d time=%d ms", __FUNCTION__, reply_size - sizeof(struct icmp_echo_hdr), inet_ntoa(from_addr.sin_addr),
							htons(pecho->seqno), (reply_time - ping_time));
				ping_flag = 1;
			}

		} else {
			ROAMING_DBG("\n\r[%s] Request timeout for icmp_seq %d\n", __FUNCTION__, ping_seq);
			ping_flag = 0;
		}

		close(ping_socket);
		//rtos_time_delay_ms(ping_interval);
		if (ping_flag) {
			ping_seq = 0;
			break;
		}
	}
	rtos_mem_free(ping_buf);
	rtos_mem_free(reply_buf);

	if (!ping_flag) {
		ping_seq = 0;
		return 1;
	}

	return 0;
}

static int wlan_fast_connect(struct wifi_roaming_data *data, u8 scan_type)
{
	ROAMING_DBG("%s()", __func__);
	unsigned long tick1 = rtos_time_get_current_system_time_ms();
	unsigned long tick2, tick3, tick4, tick5;

	uint32_t	channel;
	uint32_t    security_type;
	u8 key_id ;
	int ret;
	uint32_t wifi_retry_connect = 3; //For fast wifi connect retry
	rtw_network_info_t wifi = {0};
	struct ap_additional_info store_dhcp_info = {0};
	rtw_wifi_setting_t ap_info = {0};
	struct psk_info PSK_INFO;

#ifdef CONFIG_LWIP_LAYER
	netifapi_netif_set_up(&xnetif[0]);
#endif
	//disable autoreconnect to manually reconnect the specific ap or channel.
#if CONFIG_AUTO_RECONNECT
	wifi_config_autoreconnect(0);
#endif
#ifdef CONFIG_IEEE80211R
	rtw_set_to_roam(1);
#endif

	memset(&PSK_INFO, 0, sizeof(struct psk_info));
	memcpy(PSK_INFO.psk_essid, data->ap_info.psk_essid, sizeof(data->ap_info.psk_essid));
	memcpy(PSK_INFO.psk_passphrase, data->ap_info.psk_passphrase, sizeof(data->ap_info.psk_passphrase));
	memcpy(PSK_INFO.wpa_global_PSK, data->ap_info.wpa_global_PSK, sizeof(data->ap_info.wpa_global_PSK));
	rtw_psk_get_psk_info(&PSK_INFO);

	channel = data->ap_info.channel;
	key_id = channel >> 28;
	channel &= 0xff;
	security_type = data->ap_info.security_type;
	//set partial scan for entering to listen beacon quickly
WIFI_RETRY_LOOP:
	if (scan_type == FAST_CONNECT_SPECIFIC_CH) {
		wifi.channel = (u8)channel;
		wifi.pscan_option = PSCAN_FAST_SURVEY;
	}
	wifi.security_type = security_type;
	//SSID
	strcpy((char *)wifi.ssid.val, (char *)(data->ap_info.psk_essid));
	wifi.ssid.len = strlen((char *)(data->ap_info.psk_essid));

	switch (security_type) {
	case RTW_SECURITY_WEP_PSK:
		wifi.password = (unsigned char *) data->ap_info.psk_passphrase;
		wifi.password_len = strlen((char *)data->ap_info.psk_passphrase);
		wifi.key_id = key_id;
		break;
	case RTW_SECURITY_WPA_TKIP_PSK:
	case RTW_SECURITY_WPA2_AES_PSK:
#ifdef CONFIG_SAE_SUPPORT
	case RTW_SECURITY_WPA3_AES_PSK:
#endif
		wifi.password = (unsigned char *) data->ap_info.psk_passphrase;
		wifi.password_len = strlen((char *)data->ap_info.psk_passphrase);
		break;
	default:
		break;
	}
	/* Set prev_bssid to trigger joinbss by roaming, and will not release IP when receive RTW_JOINSTATUS_DISCONNECT in wifi_join_status_indicate*/
	wifi_get_setting(WLAN0_IDX, &ap_info);
	memcpy(wifi.prev_bssid.octet, ap_info.bssid, 6);
	// 1.connect
	ret = wifi_connect(&wifi, 1);
	tick2 = rtos_time_get_current_system_time_ms();

	if (ret != RTW_SUCCESS) {
		wifi_retry_connect--;
		if (wifi_retry_connect > 0) {
			printf("[Wifi roaming plus]: wifi retry connect\r\n");
			goto WIFI_RETRY_LOOP;
		}
	}
	// 2.dhcp
	if (ret == RTW_SUCCESS) {
		//Use ping test to check if need to do dhcp.
		if (roaming_ping_test(*(u32 *)LwIP_GetGW(0))) {
			tick4 = rtos_time_get_current_system_time_ms();
#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
			//get offer ip in flash
			if ((data->ap_n < MAX_AP_NUM) && (scan_type == FAST_CONNECT_SPECIFIC_CH)) {
				int i = 0;
				for (i = 0; i < data->ap_n; i++) {
					if (memcmp(ap_list->bssid, data->add_ap_info[i].ap_bssid, ETH_ALEN) == 0) {
						offer_ip = data->add_ap_info[i].sta_ip;
						ROAMING_DBG("\n\r Find the ehter_addr in flash() \n");
						break;
					}
				}
			}
#endif
			LwIP_DHCP(0, DHCP_START);
			tick5 = rtos_time_get_current_system_time_ms();
			ROAMING_DBG("dhcp time %d\n", (tick5 - tick4));
			//clean arp? old arp table may not update.
			etharp_cleanup_netif(&xnetif[0]);

			//store dhcp info for each ap.
			memcpy(store_dhcp_info.ap_bssid, ap_info.bssid, 6);
			store_dhcp_info.sta_ip = *(u32 *)LwIP_GetIP(0);
			wifi_write_ap_info_to_flash_ext((u8 *)&store_dhcp_info, sizeof(struct ap_additional_info));
		}
	} else {
		ROAMING_DBG("\r\n[Wifi roaming plus]: No need to do dhcp\n");
	}

#ifdef CONFIG_IEEE80211R
	rtw_set_to_roam(0);
#endif
#if CONFIG_AUTO_RECONNECT
	wifi_config_autoreconnect(2);
#endif
	tick3 = rtos_time_get_current_system_time_ms();
	ROAMING_DBG("\n\r == Roaming connect done  after %d ms = %d ms (connection) + %d ms (DHCP) ==\n", (tick3 - tick1), (tick2 - tick1), (tick3 - tick2));
	return ret;
}

static int  wifi_write_ap_info_to_flash_ext(u8 *data, u32 len)
{
	(void)len;
	flash_t flash;
	u8 n = 0;
	struct wifi_roaming_data read_data = {0};
	u32 tick1 = rtos_time_get_current_system_time_ms();

	ROAMING_DBG("%s()\n", __FUNCTION__);
	if (!data) {
		return -1;
	}


	rt_kv_get("wlan_data", (uint8_t *) &read_data, sizeof(struct wifi_roaming_data));

	if (read_data.ap_n == 0xff) {
		read_data.ap_n = 0;
	}

	//store the dhcp_info into flash after dhcp done
	ROAMING_DBG("\r\n Add additional AP info into flash\n");
	for (n = read_data.ap_n; n > 0; n--) {
		if (memcmp(read_data.add_ap_info[n - 1].ap_bssid, ((struct ap_additional_info *)data)->ap_bssid, 6) == 0) {
			read_data.add_ap_info[n - 1].sta_ip = ((struct ap_additional_info *)data)->sta_ip;
			break;
		}
	}
	if (n == 0) {
		if (read_data.ap_n < MAX_AP_NUM) {
			read_data.ap_n ++;
			memcpy((u8 *)(read_data.add_ap_info[read_data.ap_n - 1].ap_bssid), ((struct ap_additional_info *)data)->ap_bssid, 6);
			read_data.add_ap_info[read_data.ap_n - 1].sta_ip = ((struct ap_additional_info *)data)->sta_ip;
			rt_kv_set("wlan_data", (uint8_t *)&read_data, sizeof(struct wifi_roaming_data));
		} else {
			ROAMING_DBG("%s(): For more AP infos, Please change MAX_AP_NUM first!\n", __func__);
		}
	}
	u32 tick2 = rtos_time_get_current_system_time_ms();
	ROAMING_DBG("write ap_info_ext to flash [%d]ms", (tick2 - tick1));
	return 0;
}

int  wifi_write_ap_info_to_flash(u32 offer_ip, u32 server_ip)
{

	/* To avoid gcc warnings */
#if(!defined(CONFIG_FAST_DHCP) || (!CONFIG_FAST_DHCP))
	(void) offer_ip, server_ip;
#endif
	flash_t flash;
	u8 i = 0;
	struct wifi_roaming_data *read_data = NULL;
	u8 ap_change = 0;
	u32 tick1 = rtos_time_get_current_system_time_ms();
	struct wlan_fast_reconnect fast_connect_info;
	rtw_wifi_setting_t setting;
	struct psk_info PSK_info;
	u32 channel = 0;

	read_data = (struct wifi_roaming_data *)malloc(sizeof(struct wifi_roaming_data));

	ROAMING_DBG("%s()\n", __FUNCTION__);

	/* STEP1: get current connect info from wifi driver*/
	if (wifi_get_setting(WLAN0_IDX, &setting) || setting.mode == RTW_MODE_AP) {
		RTW_API_INFO("\r\n %s():wifi_get_setting fail or ap mode", __func__);
		return RTW_ERROR;
	}
	channel = (u32)setting.channel;
	memset(&fast_connect_info, 0, sizeof(struct wlan_fast_reconnect));
	switch (setting.security_type) {
	case RTW_SECURITY_OPEN:
		memcpy(fast_connect_info.psk_essid, setting.ssid, strlen(setting.ssid));
		fast_connect_info.security_type = RTW_SECURITY_OPEN;
		break;
	case RTW_SECURITY_WEP_PSK:
		memcpy(fast_connect_info.psk_essid, setting.ssid, strlen(setting.ssid));
		memcpy(fast_connect_info.psk_passphrase, setting.password, strlen(setting.password));
		channel |= (setting.key_idx) << 28;
		fast_connect_info.security_type = RTW_SECURITY_WEP_PSK;
		break;
	case RTW_SECURITY_WPA_TKIP_PSK:
	case RTW_SECURITY_WPA2_AES_PSK:
#ifdef CONFIG_SAE_SUPPORT
	case RTW_SECURITY_WPA3_AES_PSK:
#endif
		memset(&PSK_info, 0, sizeof(struct psk_info));
		rtw_psk_get_psk_info(&PSK_info);
		memcpy(fast_connect_info.psk_essid, PSK_info.psk_essid, sizeof(fast_connect_info.psk_essid));
		memcpy(fast_connect_info.psk_passphrase, PSK_info.psk_passphrase, sizeof(fast_connect_info.psk_passphrase));
		memcpy(fast_connect_info.wpa_global_PSK, PSK_info.wpa_global_PSK, sizeof(fast_connect_info.wpa_global_PSK));
		fast_connect_info.security_type = setting.security_type;
		break;

	default:
		break;
	}

	memcpy(&(fast_connect_info.channel), &channel, 4);
#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
	fast_connect_info.offer_ip = offer_ip;
	fast_connect_info.server_ip = server_ip;
#endif

	rt_kv_get("wlan_data", (uint8_t *) read_data, sizeof(struct wifi_roaming_data));
	if (read_data->num == 0xff) {
		read_data->num = 0;
	}

	//check is common info changed? Is this a new channel?
	if (read_data->num) {
		/*check if ap info {ssid/password/security_type} has changed*/
		if (memcmp((u8 *)fast_connect_info.psk_essid, (u8 *)fast_connect_info.psk_essid, 32)) {
			printf("\r\n[Wifi roaming plus]: ap ssid change\n");
			ap_change = 1;
		} else if (memcmp((u8 *)fast_connect_info.psk_passphrase, (u8 *)(read_data->ap_info.psk_passphrase), 32)) {
			printf("\r\n[Wifi roaming plus]: ap password change\n");
			ap_change = 1;
		} else if (fast_connect_info.security_type != read_data->ap_info.security_type) {
			printf("\r\n[Wifi roaming plus]: ap security type change\n");
			ap_change = 1;
		} else { /*ap info doesn't change*/
			for (i = 0; i < read_data->num; i++) {
				if (read_data->channel[i] == fast_connect_info.channel) {
					ROAMING_DBG("Already stored this channel(%d)\n", fast_connect_info.channel);
					return -1;
				}
			}
		}
	}
	if (ap_change) {
		printf("\r\n[Wifi roaming plus]: erase flash and restore new ap info\n");
		memset((u8 *)read_data, 0, sizeof(struct  wifi_roaming_data));
		read_data->num = 1;
	} else {
		printf("\r\n[Wifi roaming plus]: Add a new channel into flash\n");
		read_data->num++;
	}
	read_data->channel[read_data->num - 1] = fast_connect_info.channel; //store channel
	//only first ap's detail info has to be stored.
	if (read_data->num == 1) {
		memcpy((u8 *)&read_data->ap_info, &fast_connect_info, sizeof(struct wlan_fast_reconnect));    //store fast connect info
	}
	rt_kv_set("wlan_data", (uint8_t *)read_data, sizeof(struct wifi_roaming_data));
	u32 tick2 = rtos_time_get_current_system_time_ms();
	ROAMING_DBG("write ap_info into flash [%d]ms\n", (tick2 - tick1));
	free(read_data);
	return 0;
}

int wifi_init_done_callback_roaming(void)
{
	flash_t flash;
	struct wifi_roaming_data read_data = {0};

	rt_kv_get("wlan_data", (uint8_t *) &read_data, sizeof(struct wifi_roaming_data));

	/* Check whether stored flash profile is empty */
	if ((read_data.num == 0) || (read_data.num > MAX_CH_NUM)) {
		printf("\r\n[Wifi roaming plus]: Fast connect profile is empty, abort fast connection\n");
	}
	/* Find the best ap in flash profile */
	else {
		ROAMING_DBG("\r\n[Wifi roaming plus]: Connect to the best ap\n");
		wlan_fast_connect(&read_data, FAST_CONNECT_ALL_CH);
	}
	return 0;
}

void example_wifi_roaming_plus_init(void)
{
	// Call back from wlan driver after wlan init done
	p_wifi_do_fast_connect = wifi_init_done_callback_roaming;

	// Call back from application layer after wifi_connection success
	p_store_fast_connect_info = wifi_write_ap_info_to_flash;
}


static u32 wifi_roaming_plus_find_ap_from_scan_buf(char *target_ssid, void *user_data, int ap_num)
{
	u32 target_security = *(u32 *)user_data;
	rtw_scan_result_t *scanned_ap_info;
	u32 i = 0;
	char *scan_buf = NULL;

	scan_buf = (char *)rtos_mem_zmalloc(ap_num * sizeof(rtw_scan_result_t));
	if (scan_buf == NULL) {
		printf("malloc scan buf for example wifi roaming plus\n");
		return -1;
	}

	if (wifi_get_scan_records(&ap_num, scan_buf) < 0) {
		rtos_mem_free(scan_buf);
		return -1;
	}

	for (i = 0; i < ap_num; i++) {
		scanned_ap_info = (rtw_scan_result_t *)(scan_buf + i * sizeof(rtw_scan_result_t));
		ROAMING_DBG("Scan ap:"MAC_FMT"(%d)\n", MAC_ARG(scanned_ap_info->BSSID.octet), scanned_ap_info->channel);
		if (target_security == scanned_ap_info->security ||
			((target_security & (WPA2_SECURITY | WPA_SECURITY)) && ((scanned_ap_info->security) & (WPA2_SECURITY | WPA_SECURITY)))) {
			if (ap_list->rssi < scanned_ap_info->signal_strength) {
				ROAMING_DBG("rssi(%d) is better than last(%d)\n", scanned_ap_info->signal_strength, ap_list->rssi);
				memset(ap_list, 0, sizeof(wifi_roaming_ap_t));
				memcpy(ap_list->bssid, scanned_ap_info->BSSID.octet, ETH_ALEN);
				ap_list->channel = scanned_ap_info->channel;
				ap_list->rssi = scanned_ap_info->signal_strength;
			}
		}
	}
	rtos_mem_free(scan_buf);

	return 0;
}

int wifi_roaming_scan_one_channel(wifi_roaming_ap_t	roaming_ap, u32 retry)
{
	int cur_rssi, rssi_delta;
	rtw_phy_statistics_t phy_statistics;
	rtw_scan_param_t scan_param;
	int scanned_ap_num = 0;

	//set scan_param for scan
	memset(&scan_param, 0, sizeof(rtw_scan_param_t));
	scan_param.ssid = (char *)roaming_ap.ssid;
	scan_param.channel_list = pscan_channel_list;
	scan_param.channel_list_num = 1;
	ROAMING_DBG("scan(%d)\n", pscan_channel_list[0]);
	scanned_ap_num = wifi_scan_networks(&scan_param, 1);
	if (scanned_ap_num > 0) {
		wifi_roaming_plus_find_ap_from_scan_buf((char *)roaming_ap.ssid, (void *)&roaming_ap.security_type, scanned_ap_num);
	}
	ROAMING_DBG("scan done(%d)\n", pscan_channel_list[0]);
	wifi_fetch_phy_statistic(&phy_statistics);
	cur_rssi = phy_statistics.rssi;
	rssi_delta = ((FIND_BETTER_RSSI_DELTA - retry * 2) > 1) ? (FIND_BETTER_RSSI_DELTA - retry * 2) : 2; //at least 3db better
	if (ap_list->rssi - cur_rssi > rssi_delta && (memcmp(roaming_ap.bssid, ap_list->bssid, ETH_ALEN))) {
		printf("\r\n[Wifi roaming plus]: Find a better ap on channel %d, rssi = %d, cur_rssi=%d\n", ap_list->channel, ap_list->rssi, cur_rssi);
		return 1;
	}
	return 0;
}

int wifi_roaming_scan(struct wifi_roaming_data  read_data, u32 retry)
{
	wifi_roaming_ap_t	roaming_ap;
	rtw_wifi_setting_t	setting;
	channel_plan_t channel_plan_temp = roaming_channel_plan;
	u8 ch = 0, ch_num;
	u8 first_5g = 0;

	memset(&setting, 0, sizeof(rtw_wifi_setting_t));
	memset(&roaming_ap, 0, sizeof(wifi_roaming_ap_t));
	roaming_ap.rssi = -100;

	wifi_get_setting(WLAN0_IDX, &setting);
	strcpy((char *)roaming_ap.ssid, (char const *)setting.ssid);
	roaming_ap.security_type =  setting.security_type;
	strcpy((char *)roaming_ap.password, (char const *)setting.password);
	roaming_ap.key_idx = setting.key_idx;
	memcpy(roaming_ap.bssid, setting.bssid, 6);

#if defined(CONFIG_IEEE80211V) || defined(CONFIG_IEEE80211K)
	/*scan specific channels in nb_rpt_ch_list*/
	if ((ch_num = rtw_roam_nb_ch_num_get()) > 0) {
		ROAMING_DBG("\r\n %s():try to find a better ap in nb_rpt_ch_list\n", __func__);
		for (ch = 0 ; ch < ch_num; ch++) {
			pscan_channel_list[0] = rtw_roam_nb_ch_get(ch);
			if (wifi_roaming_scan_one_channel(roaming_ap, retry)) {
				return 1;
			}
			for (ch = 0 ; ch < channel_plan_temp.len; ch++) {
				if (channel_plan_temp.channel[ch] == pscan_channel_list[0]) {
					channel_plan_temp.channel[ch] = 0;//skip scan later
					break;
				}
			}
			for (ch = 0 ; ch < read_data.num; ch++) {
				if (read_data.channel[ch] == pscan_channel_list[0]) {
					read_data.channel[ch] = 0;//skip scan later
					break;
				}
			}
			rtos_time_delay_ms(500);
		}
	}
#endif

	/*scan specific channels*/
	if (0 < read_data.num && read_data.num < MAX_CH_NUM) {
		ROAMING_DBG("\r\n %s():try to find a better ap in flash\n", __func__);
		while (read_data.num) {
			if (read_data.channel[read_data.num - 1]) {
				pscan_channel_list[0] = read_data.channel[read_data.num - 1];
				if (wifi_roaming_scan_one_channel(roaming_ap, retry)) {
					return 1;
				}
				for (ch = 0 ; ch < channel_plan_temp.len; ch++) {
					if (channel_plan_temp.channel[ch] == pscan_channel_list[0]) {
						channel_plan_temp.channel[ch] = 0;//skip scan later
						break;
					}
				}
				rtos_time_delay_ms(500);
			}
			read_data.num--;
		}
	}

	/*scan other channels*/
	ROAMING_DBG("\r\n %s():Find the best ap in flash fail, rssi = %d, try to find in other channels\n", __func__, ap_list->rssi);
#if SUPPORT_SCAN_5G_CHANNEL
	if (rtos_time_get_current_system_time_ms() % 2) {
		first_5g = 1;  //force 5g first
		ROAMING_DBG("scan 5g first\n");
	} else {
		first_5g = 0;
		ROAMING_DBG("scan 2.4g first\n");
	}

	for (ch = 0 ; ch < channel_plan_temp.len; ch++) {
		if ((first_5g && (channel_plan_temp.channel[ch] > 15))
			|| (!first_5g && (channel_plan_temp.channel[ch] > 0 &&  channel_plan_temp.channel[ch] < 15))) {
			pscan_channel_list[0] = channel_plan_temp.channel[ch];
			if (wifi_roaming_scan_one_channel(roaming_ap, retry)) {
				return 1;
			}
			channel_plan_temp.channel[ch] = 0;
			rtos_time_delay_ms(500);
		}
	}
#endif
	for (ch = 0 ; ch < channel_plan_temp.len; ch++) {
		if (channel_plan_temp.channel[ch]) {
			pscan_channel_list[0] = channel_plan_temp.channel[ch];
			if (wifi_roaming_scan_one_channel(roaming_ap, retry)) {
				return 1;
			}
			channel_plan_temp.channel[ch] = 0;
			rtos_time_delay_ms(500);
		}
	}
	printf("\r\n[Wifi roaming plus]: Find a better ap fail,retry:%d!\n", retry);
	return 0;
}


void wifi_roaming_plus_thread(void *param)
{
	(void)param;
	ROAMING_DBG("\n %s()\n", __func__);
	signed char ap_rssi;
	rtw_phy_statistics_t phy_statistics;
	u32 scan_retry = 0;
	u32	polling_count = 0;
	u32 ap_valid = AP_VALID_TIME;
	struct wifi_roaming_data read_data = {0};
	printf("\nExample: wifi_roaming_plus \n");
	while (1) { //wait wifi connect
		if (wifi_is_running(WLAN0_IDX) && ((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
			break;
		} else {
			rtos_time_delay_ms(1000);
		}
	}
	rtos_time_delay_ms(10000);//wait rssi stable

	while (1) {
		if (wifi_is_running(WLAN0_IDX) && ((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
			wifi_fetch_phy_statistic(&phy_statistics);
			ap_rssi = phy_statistics.rssi;
			ROAMING_DBG("\r\n %s():Current rssi(%d),scan threshold rssi(%d)\n", __func__, ap_rssi, RSSI_SCAN_THRESHOLD);
			rt_kv_get("wlan_data", (uint8_t *) &read_data, sizeof(struct wifi_roaming_data));
			if (ap_rssi < RSSI_SCAN_THRESHOLD) {
				if (polling_count >= 1) {
					printf("\r\n[Wifi roaming plus]: Start scan, current rssi(%d) < scan threshold rssi(%d) \n", ap_rssi, RSSI_SCAN_THRESHOLD);
					ap_list = (wifi_roaming_ap_t *)malloc(sizeof(wifi_roaming_ap_t));
					memset(ap_list, 0, sizeof(wifi_roaming_ap_t));
					ap_list->rssi = -100;
					rt_kv_get("wlan_data", (uint8_t *) &read_data, sizeof(struct wifi_roaming_data));
					/*1.find a better ap*/
					if (wifi_roaming_scan(read_data, scan_retry)) {
						scan_retry = 0;
#if PRE_SCAN
						ap_valid = AP_VALID_TIME;
						while (ap_valid) {
							if (wifi_is_running(WLAN0_IDX) && ((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
								wifi_fetch_phy_statistic(&phy_statistics);
								ap_rssi = phy_statistics.rssi;
								ROAMING_DBG("\r\n %s():Current rssi(%d),roaming threshold rssi(%d)\n", __func__, ap_rssi, RSSI_ROAMING_THRESHOLD);
								if ((ap_rssi < RSSI_ROAMING_THRESHOLD)) {
									/*2.connect a better ap*/
									printf("\r\n[Wifi roaming plus] :Start roaming, current rssi(%d) < roaming threshold rssi(%d),target ap rssi(%d)\n", \
										   ap_rssi, RSSI_ROAMING_THRESHOLD, ap_list->rssi);
									read_data.ap_info.channel = ap_list->channel;
									wlan_fast_connect(&read_data, FAST_CONNECT_SPECIFIC_CH);
									break;
								} else if (ap_rssi > RSSI_SCAN_THRESHOLD + 5) {
									/*no need to roaming*/
									printf("\r\n[Wifi roaming plus] :Cancel roaming, current rssi=%d\n", ap_rssi);
									break;
								}
								ap_valid--;
								if (!ap_valid) {
									printf("\r\n[Wifi roaming plus]: Valid time(%d) expire, retry scan\n");
								}
							} else {
								break;
							}
							rtos_time_delay_ms(1000);
						}
#else//no pre scan
						if (wifi_is_running(WLAN0_IDX) && ((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
							wifi_fetch_phy_statistic(&phy_statistics);
							ap_rssi = phy_statistics.rssi;
							if (ap_rssi > RSSI_SCAN_THRESHOLD + 5) {
								/*no need to roaming*/
								printf("\r\n[Wifi roaming plus]: Cancel roaming, current rssi=%d\n", ap_rssi);
							} else {
								/*2.connect a better ap*/
								printf("\r\n[Wifi roaming plus]: Start roaming, current rssi(%d) < roaming threshold rssi(%d),target ap rssi(%d)\n", \
									   ap_rssi, RSSI_ROAMING_THRESHOLD, ap_list->rssi);
								read_data.ap_info.channel = ap_list->channel;
								wlan_fast_connect(&read_data, FAST_CONNECT_SPECIFIC_CH);
							}
						}
#endif
					} else { //scan fail
						scan_retry++;
					}
					free(ap_list);
					polling_count = 0;
				} else {
					polling_count++;
				}
			} else {
				polling_count = 0;
				scan_retry = 0;
			}
		}
		if (scan_retry > 5) {
			rtos_time_delay_ms(2000);
		} else {
			rtos_time_delay_ms(1000);
		}
	}
	rtos_task_delete(NULL);
}

void example_wifi_roaming_plus(void)
{
	if (rtos_task_create(NULL, ((const char *)"wifi_roaming_thread"), wifi_roaming_plus_thread, NULL, 1024 * 4, 1) != SUCCESS) {
		printf("\n\r%s rtos_task_create(wifi_roaming_thread) failed", __FUNCTION__);
	}
	return;
}
