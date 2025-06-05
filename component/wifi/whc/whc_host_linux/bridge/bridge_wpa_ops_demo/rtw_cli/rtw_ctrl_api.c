#include <errno.h>
#include <netinet/in.h> // for AF_NETLINK
#include <unistd.h>  // for close

#include <whc_bridge_host_netlink.h>
#include <rtw_ctrl_api.h>


//TODO
int event_scan_flag = 0;


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

void whc_bridge_host_print_scan_result(char *pos, int chunk_index, char last_chunk)
{
	struct rtw_scan_result *scan_result_p;

	scan_result_p = (struct rtw_scan_result *)pos;
	print_scan_result(scan_result_p);

}

void whc_bridge_host_cmd_getmac(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id)
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

	ret = whc_bridge_host_api_send_nl_data((u8 *)&cmd, sizeof(cmd), api_id);
	if (ret < 0) {
		printf("msg send fail\n");
		return;
	}

	printf("OK\n");

}


void whc_bridge_host_cmd_scan(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id)
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

	ret = whc_bridge_host_api_send_nl_data((u8 *)&cmd, sizeof(cmd), api_id);
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

	printf("OK\n");

	event_scan_flag = 1;
}

void whc_bridge_host_cmd_scan_result(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id)
{
	(void)argc;
	(void)argv;
	(void)cmd_category;
	(void)cmd_id;

	struct whc_host_cli_cmd_t cmd;
	int ret = 0;

	memset(&cmd, 0, sizeof(cmd));

	ret = whc_bridge_host_api_send_nl_data((u8 *)&cmd, sizeof(cmd), api_id);
	if (ret < 0) {
		printf("msg send fail\n");
		return;
	}

}


/* For Host to SOC End */

/* For SOC to HOST */




/* For SOC to HOST End */

/*
 * Create a raw netlink socket and bind
 */
int whc_bridge_host_api_create_nl_socket(int protocol, int pid)
{
	int fd;
	struct sockaddr_nl local;

	/* gen socket */
	fd = socket(AF_NETLINK, SOCK_RAW, protocol);
	if (fd < 0) {
		return -1;
	}

	memset(&local, 0, sizeof(local));
	local.nl_family = AF_NETLINK;
	local.nl_pid = pid;

	if (bind(fd, (struct sockaddr *) &local, sizeof(local)) < 0) {
		close(fd);
		return -1;
	}

	return fd;
}


/*
 * Probe the controller in genetlink to find the family id
 * for the DEMO_GEN_CTRL family
 */
int whc_bridge_host_api_get_family_id(int fd, char *family_name)
{
	struct msgtemplate ans;
	int id = 0;
	struct nlattr *na;
	int rep_len;
	struct msgtemplate msg;
	unsigned char *ptr = msg.buf;

	/* find family id from family name */
	whc_bridge_host_fill_nlhdr(&msg, GENL_ID_CTRL, 0, CTRL_CMD_GETFAMILY);
	nla_put_string(&ptr, CTRL_ATTR_FAMILY_NAME, family_name);
	msg.n.nlmsg_len += ptr - msg.buf;
	whc_bridge_host_api_send_to_kernel(fd, (char *)&msg, msg.n.nlmsg_len);

	/* recv message from kernel */
	rep_len = recv(fd, &ans, sizeof(ans), 0);
	if (ans.n.nlmsg_type == NLMSG_ERROR || (rep_len < 0) || !NLMSG_OK((&ans.n), rep_len)) {
		return 0;
	}

	/* parse message to get family id */
	na = (struct nlattr *) GENLMSG_DATA(&ans);
	na = (struct nlattr *)((char *) na + NLA_ALIGN(na->nla_len));
	if (na->nla_type == CTRL_ATTR_FAMILY_ID) {
		id = *(uint16_t *) NLA_DATA(na);
	}

	return id;
}

int whc_bridge_host_api_send_to_kernel(int fd, char *buf, int buflen)
{
	struct sockaddr_nl nladdr;
	int r;

	memset(&nladdr, 0, sizeof(nladdr));
	nladdr.nl_family = AF_NETLINK;

	/* send until finish */
	while ((r = sendto(fd, buf, buflen, 0, (struct sockaddr *) &nladdr,
					   sizeof(nladdr))) < buflen) {
		if (r > 0) {
			buf += r;
			buflen -= r;
		} else if (errno != EAGAIN) {
			return -1;
		}
	}

	return 0;
}

// BRIDGE_ATTR_PAYLOAD: size(4B) + payload
int whc_bridge_host_api_send_nl_data(uint8_t *buf, uint32_t buf_len, uint8_t api_id)
{
	int nl_fd;
	int nl_family_id = 0;
	int ret = 0;
	struct msgtemplate msg;
	unsigned char *ptr = msg.buf;

	/* get family id */
	nl_fd = sock_fd;
	nl_family_id = family_id;
	if (!nl_family_id) {
		fprintf(stderr, "Failed to get family id, errno %d\n", errno);
		close(nl_fd);
		return -1;
	}

	whc_bridge_host_fill_nlhdr(&msg, nl_family_id, 0, BRIDGE_CMD_CUSTOM_API);
	nla_put_u32(&ptr, BRIDGE_ATTR_API_ID, api_id);
	nla_put_payload(&ptr, BRIDGE_ATTR_PAYLOAD, buf, buf_len);
	msg.n.nlmsg_len += ptr - msg.buf;
	ret = whc_bridge_host_api_send_to_kernel(nl_fd, (char *)&msg, msg.n.nlmsg_len);
	if (ret < 0) {
		printf("msg send fail\n");
	}


	return ret;

}

