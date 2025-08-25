#include <errno.h>
#include <netinet/in.h> // for AF_NETLINK
#include <unistd.h>  // for close

#include <whc_host_netlink.h>
#include <rtw_cli_api.h>
#include <rtw_cli_netlink.h>


//TODO
int event_scan_flag = 0;


int parse_string(int index, int argc, char **argv, u8 *buf, size_t buf_size)
{
	int ix = 0;
	size_t len = 0, left = 0;
	int written = 0;
	int ret = 0;

	for (ix = index; ix < argc; ix++) {
		left = buf_size - len - 1; //For '\0'
		written = snprintf((char *)buf + len, left, "%s%s",
						   argv[ix], (ix < argc - 1) ? " " : "");
		if (written < 0 || (size_t)written >= left) {
			return 0;
		}
		len += written;
	}

	//printf("argc: %d\n", argc);
	//printf("buf: %s size: %ld\n", buf, strlen(buf));

	ret = strlen(buf);

	return (ret == 0) ? 0 : (ret + 1); //+1 for '\0'
}

/* For Event Handler */
void event_handler_scan_complete(void)
{
	event_scan_flag = 0;
}


/* End For Event Handler */

/* For Host to SOC */

void print_scan_result(struct rtw_scan_result *record)
{
	printf("%02x:%02x:%02x:%02x:%02x:%02x,", record->bssid.octet[0],
		   record->bssid.octet[1], record->bssid.octet[2], record->bssid.octet[3], record->bssid.octet[4],
		   record->bssid.octet[5]);
	printf("%d, ", record->signal_strength);
	printf("%3d, ", record->channel);
	printf("%2s, ", (record->wireless_mode & RTW_80211_AX) ? "AX" :
		   (record->wireless_mode & RTW_80211_AC) ? "AC" :
		   (record->wireless_mode & RTW_80211_N) ? "N" :
		   (record->wireless_mode & RTW_80211_A) ? "A" :
		   (record->wireless_mode & RTW_80211_G) ? "G" :
		   (record->wireless_mode & RTW_80211_B) ? "B" :
		   "Unknown");
	printf("\"%16s\", ", (record->security == RTW_SECURITY_OPEN) ? "Open" :
		   (record->security == RTW_SECURITY_WEP_PSK) ? "WEP" :
		   (record->security == RTW_SECURITY_WPA_TKIP_PSK) ? "WPA TKIP" :
		   (record->security == RTW_SECURITY_WPA_AES_PSK) ? "WPA AES" :
		   (record->security == RTW_SECURITY_WPA_MIXED_PSK) ? "WPA Mixed" :
		   (record->security == RTW_SECURITY_WPA2_AES_PSK) ? "WPA2 AES" :
		   (record->security == RTW_SECURITY_WPA2_TKIP_PSK) ? "WPA2 TKIP" :
		   (record->security == RTW_SECURITY_WPA2_MIXED_PSK) ? "WPA2 Mixed" :
		   (record->security == RTW_SECURITY_WPA_WPA2_TKIP_PSK) ? "WPA/WPA2 TKIP" :
		   (record->security == RTW_SECURITY_WPA_WPA2_AES_PSK) ? "WPA/WPA2 AES" :
		   (record->security == RTW_SECURITY_WPA_WPA2_MIXED_PSK) ? "WPA/WPA2 Mixed" :
		   (record->security == (RTW_SECURITY_WPA_TKIP_PSK | ENTERPRISE_ENABLED)) ? "WPA TKIP Enterprise" :
		   (record->security == (RTW_SECURITY_WPA_AES_PSK | ENTERPRISE_ENABLED)) ? "WPA AES Enterprise" :
		   (record->security == (RTW_SECURITY_WPA_MIXED_PSK | ENTERPRISE_ENABLED)) ? "WPA Mixed Enterprise" :
		   (record->security == (RTW_SECURITY_WPA2_TKIP_PSK | ENTERPRISE_ENABLED)) ? "WPA2 TKIP Enterprise" :
		   (record->security == (RTW_SECURITY_WPA2_AES_PSK | ENTERPRISE_ENABLED)) ? "WPA2 AES Enterprise" :
		   (record->security == (RTW_SECURITY_WPA2_MIXED_PSK | ENTERPRISE_ENABLED)) ? "WPA2 Mixed Enterprise" :
		   (record->security == (RTW_SECURITY_WPA_WPA2_TKIP_PSK | ENTERPRISE_ENABLED)) ? "WPA/WPA2 TKIP Enterprise" :
		   (record->security == (RTW_SECURITY_WPA_WPA2_AES_PSK | ENTERPRISE_ENABLED)) ? "WPA/WPA2 AES Enterprise" :
		   (record->security == (RTW_SECURITY_WPA_WPA2_MIXED_PSK | ENTERPRISE_ENABLED)) ? "WPA/WPA2 Mixed Enterprise" :
		   (record->security == RTW_SECURITY_WPA3_AES_PSK) ? "WPA3-SAE AES" :
		   (record->security == RTW_SECURITY_WPA2_WPA3_MIXED) ? "WPA2/WPA3-SAE AES" :
		   (record->security == (WPA3_SECURITY | ENTERPRISE_ENABLED)) ? "WPA3 Enterprise" :
		   (record->security == RTW_SECURITY_WPA3_OWE) ? "WPA3-OWE" :
		   "Unknown");
	printf("\"%s\" ", record->ssid.val);
	printf("\r\n");
}

void whc_cmd_handle_rx_print_scan_result(char *pos, int chunk_index,
		char last_chunk)
{
	struct rtw_scan_result *scan_result_p;

	scan_result_p = (struct rtw_scan_result *)pos;
	print_scan_result(scan_result_p);
}


void rtw_cli_cmd_wpas_on(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id)
{
	struct whc_host_cli_cmd_t cmd;
	int ret = 0;

	memset(&cmd, 0, sizeof(cmd));

	//TODO with interface?

	cmd.category = cmd_category;
	cmd.cmd_id = cmd_id;

	ret = rtw_cli_send_nl_data(WHC_CMD_CUSTOM_API, (u8 *)&cmd,
							   sizeof(cmd), api_id, NULL, NULL);
	if (ret < 0) {
		printf("msg send fail\n");
		return;
	}
}

void rtw_cli_cmd_wifi_on(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id)
{
	struct whc_host_cli_cmd_t cmd;
	int ret = 0;

	memset(&cmd, 0, sizeof(cmd));

	//TODO with STA/AP?

	cmd.category = cmd_category;
	cmd.cmd_id = cmd_id;

	ret = rtw_cli_send_nl_data(WHC_CMD_CUSTOM_API, (u8 *)&cmd,
							   sizeof(cmd), api_id, NULL, NULL);
	if (ret < 0) {
		printf("msg send fail\n");
		return;
	}
}

void rtw_cli_cmd_mp(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id)
{
	int ret = 0;
	int index = 0;
	int msg_len = 0;
	char buf[MAX_INPUT_SIZE] = {0};

	index = 1;

	msg_len = parse_string(index, argc, argv, buf, sizeof(buf));

	ret = rtw_cli_send_nl_string(WHC_CMD_CUSTOM_API, (u8 *)&buf,
								 sizeof(buf), api_id, NULL, NULL);
	if (ret < 0) {
		printf("msg send fail\n");
		return;
	}
}

void rtw_cli_cmd_dbg(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id)
{
	int ret = 0;
	int index = 0;
	int msg_len = 0;
	char buf[MAX_INPUT_SIZE] = {0};

	index = 1;

	msg_len = parse_string(index, argc, argv, buf, sizeof(buf));

	ret = rtw_cli_send_nl_string(WHC_CMD_CUSTOM_API, (u8 *)&buf,
								 sizeof(buf), api_id, NULL, NULL);
	if (ret < 0) {
		printf("msg send fail\n");
		return;
	}
}

void rtw_cli_cmd_getmac(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id)
{
	struct whc_host_cli_cmd_t cmd;
	int ret = 0;
	u8 idx = 0;

	if (event_scan_flag == 1) {
		printf("\n%s\n", MSG_EVENT_FAIL_BUSY);
		return;
	}

	if (argv[1] == NULL) {
		cmd.idx = 0;
	} else {
		idx = *argv[1] - '0';
		if ((idx != 0) && (idx != 1)) {
			cmd.idx = 0;
		} else {
			cmd.idx = idx;
		}
	}

	memset(&cmd, 0, sizeof(cmd));

	cmd.category = cmd_category;
	cmd.cmd_id = cmd_id;

	ret = rtw_cli_send_nl_data(WHC_CMD_CUSTOM_API, (u8 *)&cmd,
							   sizeof(cmd), api_id, NULL, NULL);
	if (ret < 0) {
		printf("msg send fail\n");
		return;
	}
}

void rtw_cli_cmd_set_network(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id)
{
	struct whc_host_cli_send_string_t cmd;
	int ret = 0;
	int index = 0;
	int msg_len = 0;
	char *token = argv[0];
	const char *cmd_set_network = "set_network";

	memset(&cmd, 0, sizeof(cmd));

	cmd.category = cmd_category;
	cmd.cmd_id = cmd_id;

	index = 0;

	msg_len = parse_string(index, argc, argv, cmd.message, sizeof(cmd.message));

	if (strncmp(argv[0], cmd_set_network, strlen(cmd_set_network)) == 0) {
		if (msg_len == 0) {
			printf("set_network variables:\n"
				   "  ssid (network name, SSID)\n"
				   "  psk (WPA passphrase or pre-shared key)\n"
				   "  key_mgmt (key management protocol)\n");
			return;
		}
	}

	cmd.msg_len = msg_len;
	printf("wpa ctrl cmd: %s %s\n", token, cmd.message);

	ret = rtw_cli_send_nl_data(WHC_CMD_CUSTOM_API, (u8 *)&cmd,
							   sizeof(cmd), api_id, NULL, NULL);
	if (ret < 0) {
		printf("msg send fail\n");
		return;
	}
}



void rtw_cli_cmd_scan(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id)
{
	struct whc_host_cli_cmd_t cmd;
	int ret = 0;

	if (event_scan_flag == 1) {
		printf("\n%s\n", MSG_EVENT_FAIL_BUSY);
		return;
	}

	memset(&cmd, 0, sizeof(cmd));

	cmd.category = cmd_category;
	cmd.cmd_id = cmd_id;

	ret = rtw_cli_send_nl_data(WHC_CMD_CUSTOM_API, (u8 *)&cmd,
							   sizeof(cmd), api_id, NULL, NULL);
	if (ret < 0) {
		printf("msg send fail\n");
		return;
	}

	if (scan_results != NULL) {
		free(scan_results);
		scan_results = NULL;
	}
	num_of_ap_index = 0;
	num_of_ap = 0;

	event_scan_flag = 1;
}

void rtw_cli_cmd_scan_result(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id)
{
	(void)argc;
	(void)argv;
	(void)cmd_category;
	(void)cmd_id;

	struct whc_host_cli_cmd_t cmd;
	int ret = 0;

	memset(&cmd, 0, sizeof(cmd));

	ret = rtw_cli_send_nl_data(WHC_CMD_CUSTOM_API, (u8 *)&cmd,
							   sizeof(cmd), api_id, NULL, NULL);
	if (ret < 0) {
		printf("msg send fail\n");
		return;
	}

}

static int show_join_event_handler(struct nl_msg *msg, void *arg)
{
	struct nlattr *tb_msg[WHC_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	size_t payload_len = 0;
	char *payload = NULL;
	int api_id = 0;

	nla_parse(tb_msg, WHC_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			  genlmsg_attrlen(gnlh, 0), NULL);

	if (tb_msg[WHC_ATTR_API_ID]) {
		//printf("WHC_ATTR_API_ID = %u\n", api_id);
		api_id = nla_get_u32(tb_msg[WHC_ATTR_API_ID]);

		if (api_id == CMD_WIFI_GET_JOIN_EVENT) {

			if (tb_msg[WHC_ATTR_PAYLOAD]) {
				payload = nla_data(tb_msg[WHC_ATTR_PAYLOAD]);
				payload_len = nla_len(tb_msg[WHC_ATTR_PAYLOAD]);
			}
			printf("%s \n", payload);
		}
	}

}


void rtw_cli_cmd_get_join_status(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id)
{
	(void)argc;
	(void)argv;
	(void)cmd_category;
	(void)cmd_id;

	struct whc_host_cli_cmd_t cmd;
	int ret = 0;

	memset(&cmd, 0, sizeof(cmd));

	ret = rtw_cli_send_nl_data(WHC_CMD_CUSTOM_API, (u8 *)&cmd,
							   sizeof(cmd), api_id, show_join_event_handler, NULL);
	if (ret < 0) {
		printf("msg send fail\n");
		return;
	}

}


/* For Host to SOC End */


/* For SOC to HOST */
void whc_cmd_handle_rx_list_network(char *pos)
{
	int id = 0;

	printf("\nnetwork id / ssid / bssid / flags\n");
	printf("%d   %s  \n", id, pos);

}


void whc_cmd_handle_rx_get_status(char *pos)
{
	struct whc_rtw_cli_wifi_status *info;
	pos += 1;

	info = (struct whc_rtw_cli_wifi_status *)pos;

	printf("\n");
	printf("bssid=%02x:%02x:%02x:%02x:%02x:%02x\n", info->bssid[0],
		   info->bssid[1], info->bssid[2], info->bssid[3],
		   info->bssid[4], info->bssid[5]);
	printf("channel=%d\n", info->channel);
	printf("ssid=%s\n", info->ssid);

}


/* For SOC to HOST End */

