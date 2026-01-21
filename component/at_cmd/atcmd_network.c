/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"

#ifndef CONFIG_MP_SHRINK
#ifdef CONFIG_LWIP_LAYER
#if defined(CONFIG_ATCMD_NETWORK) && (CONFIG_ATCMD_NETWORK == 1)
#include "lwip_netconf.h"
#include "atcmd_service.h"
#include "atcmd_network.h"
#if defined(CONFIG_ATCMD_SNTP) && (CONFIG_ATCMD_SNTP == 1)
#include "lwip/apps/sntp.h"
#include "sntp/sntp_api.h"
#endif
#if defined(CONFIG_ATCMD_MDNS) && (CONFIG_ATCMD_MDNS == 1)
#include "mdns.h"

typedef struct {
	const char *key;
	const char *value;
} txt_item_t;

typedef struct {
	txt_item_t *items;
	uint8_t     count;
} txt_userdata_t;

txt_userdata_t *mdns_txt_record = NULL;
unsigned char mdns_enable_state = 0;

enum mdns_state_enu {
	MDNS_DEINIT = 0,
	MDNS_DISABLE = 1,
	MDNS_ENABLE = 2
};

static void free_txt_userdata(txt_userdata_t **psd)
{
	if (psd == NULL || *psd == NULL) {
		return;
	}

	txt_userdata_t *ud = *psd;

	for (uint8_t i = 0; i < ud->count; i++) {
		free((char *)ud->items[i].key);
		free((char *)ud->items[i].value);
	}

	free(ud->items);
	free(ud);
	*psd = NULL;
}

static void srv_txt(struct mdns_service *service, void *txt_userdata)
{
	char content[50] = {"Here is ameba lwip_mdns service!"};
	mdns_resp_add_service_txtitem(service, content, strlen(content));

	if (txt_userdata != NULL) {
		txt_userdata_t *ud = (txt_userdata_t *)txt_userdata;
		char buf[64];
		for (uint8_t i = 0; i < ud->count; i++) {
			memset(buf, 0, sizeof(buf));
			int n = snprintf(buf, sizeof(buf), "%s=%s",
							 ud->items[i].key,
							 ud->items[i].value);
			if (n > 0 && n < (int)sizeof(buf)) {
				mdns_resp_add_service_txtitem(service, buf, (uint8_t)n);
			}
		}
	}
}
#endif

static void at_ping_help(void)
{
	RTK_LOGI(NOTAG, "\r\n");
	RTK_LOGI(NOTAG, "AT+PING=<host>[,<-t>][,<-n>,<count>][,<-l>,<size>]\r\n");
	RTK_LOGI(NOTAG, "AT+PING=<stop>\r\n");
	RTK_LOGI(NOTAG, "\t\tstop\t\tterminate ping\r\n");
	RTK_LOGI(NOTAG, "\t\t-t\t#\tping the specified host until stopped\r\n");
	RTK_LOGI(NOTAG, "\t\t-n\t#\tnumber of echo requests to send (default 4 times)\r\n");
	RTK_LOGI(NOTAG, "\t\t-l\t#\tsend buffer size (default 32 bytes)\r\n");
	RTK_LOGI(NOTAG, "\t\tif\t#\tspecify ping interface, 0 is for STA and 1 is for soft\r\n");
	RTK_LOGI(NOTAG, "\tExample:\r\n");
	RTK_LOGI(NOTAG, "\t\tAT+PING=192.168.1.2,-n,100,-l,5000\r\n");
}

void at_ping(void *arg)
{
	int argc = 0;
	int error_no = 0;
	char *argv[MAX_ARGC] = {0};

	RTK_LOGI(NOTAG, "[+PING]: _AT_WLAN_PING_TEST_\r\n");

	if (arg == NULL) {
		error_no = 1;
		goto end;
	}

	argv[0] = (char *)"ping";
	argc = parse_param(arg, argv);
	if (argc > 1) {
		error_no = cmd_ping(argc, argv);
	} else {
		RTK_LOGI(NOTAG, "PING error happend\r\n");
		error_no = 2;
		goto end;
	}

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_ping_help();
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

static void at_iperf_help(void)
{
	RTK_LOGI(NOTAG, "\r\n");
	RTK_LOGI(NOTAG, "AT+IPERF=<-s>[,<-p>,<port>][,<-u>]\r\n");
	RTK_LOGI(NOTAG, "AT+IPERF=<-c>,<host|stop>[,<-i>,<periodic>][,<-l>,<size>][,<-u>][,<-b>,<bandwidth>][,<-d>][,<-t>,<transtime>][,<-n>,<count>][,<-S>]\r\n");
	RTK_LOGI(NOTAG, "\tClient/Server:\r\n");
	RTK_LOGI(NOTAG, "\t\t?\t\t\tList all stream status\r\n");
	RTK_LOGI(NOTAG, "\t\tstop\t#\t\tterminate specific stream id or terminate all stream if no id specified\r\n");
	RTK_LOGI(NOTAG, "\t\t-i\t#\t\tseconds between periodic bandwidth reports\r\n");
	RTK_LOGI(NOTAG, "\t\t-l\t#\t\tlength of buffer to read or write (default 1460 Bytes)\r\n");
	RTK_LOGI(NOTAG, "\t\t-p\t#\t\tserver port to listen on/connect to (default 5001)\r\n");
	RTK_LOGI(NOTAG, "\t\t-u\t#\t\tuse UDP protocol (default TCP)\r\n");
	RTK_LOGI(NOTAG, "\tServer specific:\r\n");
	RTK_LOGI(NOTAG, "\t\t-s\t\t\trun in server mode\r\n");
	RTK_LOGI(NOTAG, "\t\t-B\t\t\tbind multicast address in udp server mode\n");
	RTK_LOGI(NOTAG, "\tClient specific:\r\n");
	RTK_LOGI(NOTAG, "\t\t-b\t#[KM]\t\tfor UDP, bandwidth to send at in bits/sec (default 1 Mbit/sec)\r\n");
	RTK_LOGI(NOTAG, "\t\t-c\t<host>\trun in client mode, connecting to <host>\r\n");
	RTK_LOGI(NOTAG, "\t\t-d\t\t\tdo a bidirectional test simultaneously\r\n");
	RTK_LOGI(NOTAG, "\t\t-t\t#\t\ttime in seconds to transmit for (default 10 secs)\r\n");
	RTK_LOGI(NOTAG, "\t\t-n\t#[KM]\t\tnumber of bytes to transmit (instead of -t)\r\n");
	RTK_LOGI(NOTAG, "\t\t-S\t#\t\tfor UDP, set the IP 'type of service'\r\n");
	RTK_LOGI(NOTAG, "\tExample for TCP:\r\n");
	RTK_LOGI(NOTAG, "\t\tAT+IPERF=-s,-p,5002\r\n");
	RTK_LOGI(NOTAG, "\t\tAT+IPERF=-c,192.168.1.2,-t,100,-p,5002\r\n");
	RTK_LOGI(NOTAG, "\tExample for UDP:\r\n");
	RTK_LOGI(NOTAG, "\t\tAT+IPERF=-s,-p,5002,-u\r\n");
	RTK_LOGI(NOTAG, "\t\tAT+IPERF=-c,192.168.1.2,-t,100,-p,5002,-u\r\n");
}

void at_iperf(void *arg)
{
	int error_no = 0;
	int argc;
	char *argv[MAX_ARGC] = {0};
	char *pos;
	char *input = NULL;
	char *char_arg = (char *)arg;

	if (arg == NULL) {
		error_no = 1;
		goto end;
	}

	pos = strpbrk(char_arg, "u");

	if (pos) {
		if ((memcmp(pos - 1, "-", 1)) || (pos == char_arg)) {
			RTK_LOGI(NOTAG, "- needs to be added before u\r\n");
			error_no = 1;
			goto end;
		}

		input = (char *)rtos_mem_zmalloc(strlen(char_arg) - 2); /* delete "-u,"or ",-u" and need '\0' at the end */
		if (pos - char_arg == 1) {  // "-u" is at the beginning of arg
			memcpy(input, char_arg + 3, strlen(char_arg) - 3);
		} else {                      // "-u" is at the end or middle of arg
			memcpy(input, char_arg, pos - char_arg - 2);                //copy str before "-u"
			memcpy(input + strlen(input), pos + 1, strlen(pos) - 1);  //copy str after "-u"
		}
		argv[0] = (char *)"udp";
	} else {
		input = (char *)rtos_mem_zmalloc(strlen(char_arg) + 1); /* need '\0' at the end */
		memcpy(input, char_arg, strlen(char_arg));
		argv[0] = (char *)"tcp";
	}

	RTK_LOGI(NOTAG, "[+IPERF]: _AT_WLAN_IPERF1_TCP_TEST_\r\n");

	argc = parse_param(input, argv);
	if (argc > 1) {
		error_no = cmd_iperf(argc, argv);
	} else {
		RTK_LOGI(NOTAG, "[+IPERF] Should be some argc\r\n");
		error_no = 3;
		goto end;
	}

end:
	if (input) {
		rtos_mem_free((void *)input);
	}
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_iperf_help();
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

/*
 * To aviod compile error when cmd_iperf3 is not implemented
 */
_WEAK int cmd_iperf3(int argc, char **argv)
{
	UNUSED(argc);
	UNUSED(argv);
	RTK_LOGW(NOTAG, " iperf3 is not supported yet\r\n");
	return 0;
}

static void at_iperf3_help(void)
{
	RTK_LOGI(NOTAG, "\r\n");
	RTK_LOGI(NOTAG, "AT+IPERF3=<-s>[,<-p>,<port>]\r\n");
	RTK_LOGI(NOTAG, "AT+IPERF3=<-c>,<host|stop>[,<-i>,<periodic>][,<-l>,<size>][,<-d>][,<-t>,<transtime>][,<-n>,<count>]\r\n");
	RTK_LOGI(NOTAG, "\tClient/Server:\r\n");
	RTK_LOGI(NOTAG, "\t\tstop\t#\t\tterminate specific stream id or terminate all stream if no id specified\r\n");
	RTK_LOGI(NOTAG, "\t\t-i\t#\t\tseconds between periodic bandwidth reports\r\n");
	RTK_LOGI(NOTAG, "\t\t-l\t#\t\tlength of buffer to read or write (default 1460 Bytes)\r\n");
	RTK_LOGI(NOTAG, "\t\t-p\t#\t\tserver port to listen on/connect to (default 5001)\r\n");
	RTK_LOGI(NOTAG, "\t\t-u\t#\t\tuse UDP protocol (default TCP)\r\n");
	RTK_LOGI(NOTAG, "\tServer specific:\r\n");
	RTK_LOGI(NOTAG, "\t\t-s\t\t\trun in server mode\r\n");
	RTK_LOGI(NOTAG, "\tClient specific:\r\n");
	RTK_LOGI(NOTAG, "\t\t-c\t<host>\trun in client mode, connecting to <host>\r\n");
	RTK_LOGI(NOTAG, "\t\t-d\t\t\tdo a bidirectional test simultaneously\r\n");
	RTK_LOGI(NOTAG, "\t\t-t\t#\t\ttime in seconds to transmit for (default 10 secs)\r\n");
	RTK_LOGI(NOTAG, "\t\t-n\t#[KM]\t\tnumber of bytes to transmit (instead of -t)\r\n");
	RTK_LOGI(NOTAG, "\tExample:\r\n");
	RTK_LOGI(NOTAG, "\t\tAT+IPERF3=-s,-p,5002\r\n");
	RTK_LOGI(NOTAG, "\t\tAT+IPERF3=-c,192.168.1.2,-t,100,-p,5002\r\n");
}

void at_iperf3(void *arg)
{
	int error_no = 0;
	int argc;
	char *argv[MAX_ARGC] = {0};

	if (arg == NULL) {
		error_no = 1;
		goto end;
	}

	RTK_LOGI(NOTAG, "[+IPERF3]: _AT_WLAN_IPERF1_TCP_TEST_\r\n");
	argv[0] = (char *)"iperf3";
	argc = parse_param(arg, argv);
	if (argc > 1) {
		error_no = cmd_iperf3(argc, argv);
	} else {
		RTK_LOGI(NOTAG, "[+IPERF3] Should be some argc\r\n");
		error_no = 2;
		goto end;
	}

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_iperf3_help();
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

#if defined(CONFIG_ATCMD_DNS) && (CONFIG_ATCMD_DNS == 1)
void at_dns(void *arg)
{
	int argc = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	struct hostent *host_entry = NULL;
	char **addr_list = NULL;

	if (arg == NULL) {
		RTK_LOGE(NOTAG, "[at_dns] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}
	argc = parse_param(arg, argv);
	if (argc != 2) {
		RTK_LOGE(NOTAG, "[at_dns] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}

	host_entry = gethostbyname(argv[1]);
	if (host_entry != NULL) {
		for (addr_list = host_entry->h_addr_list; *addr_list != NULL; addr_list++) {
			at_printf("\r\n+DNS:%s\r\n", inet_ntoa(**addr_list));
		}
	} else {
		RTK_LOGW(NOTAG, "[at_dns] Domain Name '%s' Not be resolved\r\n", argv[1]);
		error_no = 2;
		goto end;
	}

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

void at_querydnssrv(void *arg)
{
	int error_no = 0;
	const ip_addr_t *dns_server = NULL;

	if (arg != NULL) {
		RTK_LOGE(NOTAG, "[at_querydnssrv] Input parameter SHOULD be NULL\r\n");
		error_no = 1;
		goto end;
	}

	at_printf("\r\n+QUERYDNSSRV:");

	dns_server = dns_getserver(0);  // Primary DNS server
	if (*(u32_t *)dns_server != IPADDR_ANY)   {
		at_printf("%s", ipaddr_ntoa(dns_server));
	}
	dns_server = dns_getserver(1);  // Secondary DNS server
	if (*(u32_t *)dns_server != IPADDR_ANY)   {
		at_printf(", %s", ipaddr_ntoa(dns_server));
	}
	at_printf("\r\n");

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

void at_setdnssrv(void *arg)
{
	int argc = 0, error_no = 0;
	ip_addr_t dns_server1 = {0}, dns_server2 = {0};
	char *argv[MAX_ARGC] = {0};

	if (arg == NULL) {
		RTK_LOGE(NOTAG, "[at_setdnssrv] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}
	argc = parse_param(arg, argv);
	if ((argc < 2) || (argc > 3)) {
		RTK_LOGE(NOTAG, "[at_setdnssrv] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}

	if (inet_pton(AF_INET, argv[1], &dns_server1) != 1)  {
		RTK_LOGW(NOTAG, "[at_setdnssrv] Invalid DNS Server IP\r\n");
		error_no = 2;
		goto end;
	}
	if (argc == 3)   {
		if (inet_pton(AF_INET, argv[2], &dns_server2) != 1)  {
			RTK_LOGW(NOTAG, "[at_setdnssrv] Invalid DNS Server IP\r\n");
			error_no = 2;
			goto end;
		}
	}
	RTK_LOGI(NOTAG, "[at_setdnssrv] DNS Server IP: 0x%08x,0x%08x\r\n", htonl(ip_2_ip4(&dns_server1)->addr), htonl(ip_2_ip4(&dns_server2)->addr));

	if (argc == 2)   {
		dns_setserver(0, &dns_server1);  // Primary DNS server
	} else if (argc == 3)   {
		dns_setserver(0, &dns_server1);  // Primary DNS server
		dns_setserver(1, &dns_server2);  // Secondary DNS server
	}

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}
#endif

#if defined(CONFIG_ATCMD_MDNS) && (CONFIG_ATCMD_MDNS == 1)
void at_mdns(void *arg)
{
	int argc = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int mode = 0, txt_record_cnt = 0, port = 0;
	enum mdns_sd_proto protocol = 0;
	char *hostname = NULL, *service_type = NULL, *service_name = NULL;

	if (arg == NULL) {
		RTK_LOGE(NOTAG, "[at_mdns] Input parameter is NULL\r\n");
		error_no = 1;
		goto end;
	}
	argc = parse_param_advance(arg, argv);
	mode = atoi(argv[1]);
	if (mode < 0 || mode > 1) {
		RTK_LOGE(NOTAG, "[at_mdns] mode must be 0 or 1\r\n");
	}

	if (mode == 1) {
		if (mdns_enable_state == MDNS_ENABLE) {
			error_no = 3;
			RTK_LOGE(NOTAG, "[at_mdns]mdns service has been enabled, please disable first \r\n");
			goto end;
		}

		if (LwIP_Check_Connectivity(NETIF_WLAN_STA_INDEX) != CONNECTION_VALID) {
			RTK_LOGE(NOTAG, "[at_mdns] Wifi connection is not ready\r\n");
			error_no = 4;
			goto end;
		}

		if (argc < 5) {
			RTK_LOGE(NOTAG, "[at_mdns] Invalid number of parameters\r\n");
			error_no = 1;
			goto end;
		}


		if (strlen(argv[2]) == 0) {
			RTK_LOGE(NOTAG, "[at_mdns] Missing hostname\r\n");
			error_no = 1;
			goto end;
		}
		hostname = (char *)argv[2];

		if (strlen(argv[3]) == 0) {
			RTK_LOGE(NOTAG, "[at_mdns] Missing service_type\r\n");
			error_no = 1;
			goto end;
		}
		service_type = (char *)argv[3];

		port = atoi(argv[4]);
		if (port <= 0 || port > 65535) {
			RTK_LOGE(NOTAG, "[at_mdns] The range of <port> is [1, 65535]\r\n");
			error_no = 1;
			goto end;
		}

		if (argc >= 6) {
			if (strlen(argv[5])) {
				service_name = (char *)argv[5];
			} else {
				service_name = hostname;
			}
		} else {
			service_name = hostname;
		}

		if (argc >= 7) {
			if (strlen(argv[6])) {
				if (atoi(argv[6]) == 1) {
					protocol = DNSSD_PROTO_TCP;
				} else if (atoi(argv[6]) == 2) {
					protocol = DNSSD_PROTO_UDP;
				} else {
					RTK_LOGE(NOTAG, "[at_mdns] protocol number must be 1 or 2\r\n");
					error_no = 1;
					goto end;
				}
			} else {
				protocol = DNSSD_PROTO_TCP;
			}
		} else {
			protocol = DNSSD_PROTO_TCP;
		}

		if (argc > 8) {
			if (strlen(argv[7])) {
				if (atoi(argv[7]) > 6 || atoi(argv[7]) < 1) {
					RTK_LOGE(NOTAG, "[at_mdns] txt_record_cnt must be 1~6\r\n");
					error_no = 1;
					goto end;
				}
				txt_record_cnt = atoi(argv[7]);
			} else {
				RTK_LOGE(NOTAG, "[at_mdns] txt_record_cnt must be clear if txt records are set \r\n");
				error_no = 1;
				goto end;
			}

			if (argc < 8 + 2 * txt_record_cnt) {
				RTK_LOGE(NOTAG, "[at_mdns] key and value must be set if txt_record_cnt is clear \r\n");
				error_no = 1;
				goto end;
			}

			mdns_txt_record = (txt_userdata_t *)rtos_mem_zmalloc(sizeof(txt_userdata_t));
			if (mdns_txt_record == NULL) {
				RTK_LOGE(NOTAG, "[at_mdns] malloc failed \r\n");
				error_no = 2;
				goto end;
			}

			mdns_txt_record->count = txt_record_cnt;
			mdns_txt_record->items = rtos_mem_zmalloc(sizeof(txt_item_t) * txt_record_cnt);
			if (mdns_txt_record->items == NULL) {
				RTK_LOGE(NOTAG, "[at_mdns] malloc failed \r\n");
				error_no = 2;
				goto end;
			}

			for (uint8_t i = 0; i < txt_record_cnt; i++) {

				if (strlen(argv[8 + i * 2]) == 0 || strlen(argv[9 + i * 2]) == 0) {
					RTK_LOGE(NOTAG, "[at_mdns] txt_record key or value must be set \r\n");
					error_no = 1;
					goto end;
				}

				mdns_txt_record->items[i].key   = strdup(argv[8 + i * 2]);
				mdns_txt_record->items[i].value = strdup(argv[9 + i * 2]);

				if (mdns_txt_record->items[i].key == NULL || mdns_txt_record->items[i].value == NULL) {
					RTK_LOGE(NOTAG, "[at_mdns] malloc failed \r\n");
					error_no = 2;
					goto end;
				}
			}
		}
	}

	if (mode == 0) {
		mdns_resp_remove_netif(pnetif_sta);
		if (mdns_txt_record != NULL) {
			free_txt_userdata(&mdns_txt_record);
		}
		if (mdns_enable_state == MDNS_ENABLE) {
			mdns_enable_state = MDNS_DISABLE;
		}
	} else if (mode == 1) {

		if (mdns_enable_state == MDNS_DEINIT) {
			mdns_resp_init();
			mdns_enable_state = MDNS_DISABLE;
		}

		if (mdns_resp_add_netif(pnetif_sta, hostname, 30)) {
			error_no = 5;
			RTK_LOGE(NOTAG, "[at_mdns]mdns_resp_add_netif fail \r\n");
			goto end;
		}

		if (mdns_resp_add_service(pnetif_sta, service_name, service_type, protocol, port, 30, srv_txt, mdns_txt_record)) {
			error_no = 5;
			RTK_LOGE(NOTAG, "[at_mdns]mdns_resp_add_service fail \r\n");
			goto end;
		}

		mdns_enable_state = MDNS_ENABLE;

	}

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		if (error_no != 3) {
			mdns_resp_remove_netif(pnetif_sta);
			if (mdns_txt_record != NULL) {
				free_txt_userdata(&mdns_txt_record);
			}
		}

		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

#endif /* CONFIG_ATCMD_MDNS */

#if defined(CONFIG_ATCMD_SNTP) && (CONFIG_ATCMD_SNTP == 1)
static char *server_list[SNTP_MAX_SERVERS] = {NULL};
static int server_count = 0;

static int parse_timezone(const char *timezone_str, int *hours, int *minutes, int *is_positive)
{
	int h = 0;
	int m = 0;
	int positive = 1;
	const char *num_str = timezone_str;

	if (timezone_str[0] == '-') {
		positive = 0;
		num_str++;
	} else if (timezone_str[0] == '+') {
		return -1;
	}

	size_t len = strlen(num_str);
	if (len == 1 || len == 2) {
		h = atoi(num_str);
		m = 0;
	} else if (len == 3 || len == 4) {
		char hour_str[3] = {num_str[0], (len == 3) ? '\0' : num_str[1], '\0'};
		h = atoi(hour_str);
		m = atoi(&num_str[(len == 3) ? 1 : 2]);
	} else {
		return -1;
	}

	if (h > 14 || (h == 14 && m > 0) || (!positive && (h > 12 || (h == 12 && m > 0)))) {
		return -1;
	}

	*hours = h;
	*minutes = m;
	*is_positive = positive;
	return 0;
}

void at_sntpcfg(void *arg)
{
	int argc = 0, error_no = 0;
	char *argv[MAX_ARGC] = {0};
	int enable = 0;
	int hours = 8;
	int minutes = 0;
	int is_positive = 1;
	int interval_sec = 0;

	if (arg == NULL) {
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc < 2) {
		RTK_LOGE(NOTAG, "[+SNTPCFG] Invalid number of parameters\r\n");
		error_no = 1;
		goto end;
	}

	// Check if the number of server parameters exceeds SNTP_MAX_SERVERS
	if (argc > 4 + SNTP_MAX_SERVERS) {
		RTK_LOGE(NOTAG, "[+SNTPCFG] Too many SNTP servers (max %d)\r\n", SNTP_MAX_SERVERS);
		error_no = 1;
		goto end;
	}

	enable = atoi(argv[1]);
	if (enable != 0 && enable != 1) {
		RTK_LOGE(NOTAG, "[+SNTPCFG] Invalid enable\r\n");
		error_no = 2;
		goto end;
	}

	// timezone parameter
	if (argc >= 3 && strlen(argv[2]) > 0) {
		if (parse_timezone(argv[2], &hours, &minutes, &is_positive) != 0) {
			RTK_LOGE(NOTAG, "[+SNTPCFG] Invalid timezone format. Use integer [-12,14] or [+|-][hh]mm format\r\n");
			error_no = 2;
			goto end;
		}
	}

	char tz_str[32];
	// The sign of offset is opposite to UTC offset
	snprintf(tz_str, sizeof(tz_str), "XXX%c%d:%02d", is_positive ? '-' : '+', hours, minutes);

	setenv("TZ", tz_str, 1);
	tzset();

	RTK_LOGI(NOTAG, "[+SNTPCFG] System timezone set to UTC%c%02d:%02d (TZ=%s)\r\n",
			 is_positive ? '+' : '-', hours, minutes, tz_str);

	// interval parameter
	if (argc >= 4 && strlen(argv[3]) > 0) {
		interval_sec = atoi(argv[3]);
		if (interval_sec <= 15) {
			RTK_LOGE(NOTAG, "[+SNTPCFG] Interval must be greater than 15 seconds\r\n");
			error_no = 2;
			goto end;
		}
		u32_t interval_ms = interval_sec * 1000;
		sntp_set_update_interval(interval_ms);
		RTK_LOGI(NOTAG, "[+SNTPCFG] SNTP update interval set to %d seconds (%u ms)\r\n", interval_sec, interval_ms);
	}

	if (enable == 1) {
		// Clear existing SNTP server configuration
		sntp_stop();
		for (int i = 0; i < server_count; i++) {
			if (server_list[i] != NULL) {
				free(server_list[i]);
				server_list[i] = NULL;
			}
		}
		server_count = 0;

		// Process SNTP server parameters
		if (argc >= 5) {
			for (int i = 4; i < argc && server_count < SNTP_MAX_SERVERS; i++) {
				if (strlen(argv[i]) > 0) {
					RTK_LOGI(NOTAG, "[+SNTPCFG] Setting SNTP server %d: %s\r\n", server_count, argv[i]);
					server_list[server_count] = strdup(argv[i]);
					server_count++;
				}
			}
		}

		// If no server is configured, use the default server
		if (server_count == 0) {
			server_list[0] = strdup("pool.ntp.org");
			sntp_setservername(0, server_list[0]);
			server_count = 1;
			RTK_LOGI(NOTAG, "[+SNTPCFG] Using default SNTP server: pool.ntp.org\r\n");
		} else {
			for (int i = 0; i < server_count; i++) {
				if (strlen(server_list[i]) > 0) {
					sntp_setservername(i, server_list[i]);
				}
			}
		}

		sntp_init();
	} else if (enable == 0) {
		sntp_stop();
		for (int i = 0; i < server_count; i++) {
			if (server_list[i] != NULL) {
				free(server_list[i]);
				server_list[i] = NULL;
			}
		}
		server_count = 0;
		RTK_LOGI(NOTAG, "[+SNTPCFG] SNTP stopped and server list cleared\r\n");
	}

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

void at_sntpquery(void *arg)
{
	int error_no = 0;

	if (arg != NULL) {
		error_no = 1;
		goto end;
	}

	at_printf("\r\n");

	// Query SNTP enable status
	int is_sntp_enabled = sntp_enabled();
	at_printf("enable: %d\r\n", is_sntp_enabled);

	// Query timezone
	char *tz = getenv("TZ");
	char tz_display[32] = "800";

	if (tz && strlen(tz) > 3) {
		char *sign_pos = strpbrk(tz + 3, "+-");

		if (sign_pos) {
			int hours = 0, minutes = 0;
			char sign = *sign_pos;

			if (sscanf(sign_pos + 1, "%d:%d", &hours, &minutes) >= 1) {
				if (sign == '-') {
					snprintf(tz_display, sizeof(tz_display), "%d%02d", hours, minutes);
				} else {
					snprintf(tz_display, sizeof(tz_display), "-%d%02d", hours, minutes);
				}
			} else if (sscanf(sign_pos + 1, "%d", &hours) == 1) {
				if (sign == '-') {
					snprintf(tz_display, sizeof(tz_display), "%d", hours);
				} else {
					snprintf(tz_display, sizeof(tz_display), "-%d", hours);
				}
			}
		}
	}
	at_printf("timezone: %s\r\n", tz_display);

	// Query update interval
	u32_t interval_ms = sntp_get_update_interval();
	int interval_sec = interval_ms / 1000;
	at_printf("interval: %d\r\n", interval_sec);

	// Query SNTP servers
	at_printf("server_count: %d\r\n", server_count);
	for (int i = 0; i < server_count; i++) {
		if (server_list[i] != NULL) {
			at_printf("server%d: %s\r\n", i, server_list[i]);
		}
	}

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

void at_sntptime(void *arg)
{
	(void)arg;
	int error_no = 0;

	time_t now;
	struct tm timeinfo;
	char tz_display[32] = "UTC+08:00";
	char *tz_env = getenv("TZ");

	time(&now);
	timeinfo = *(localtime(&now));

	// Get current timezone info and convert to display format
	if (tz_env && strlen(tz_env) > 3) {
		char *sign_pos = strpbrk(tz_env + 3, "+-");

		if (sign_pos) {
			int hours = 0, minutes = 0;
			char sign = *sign_pos;

			// The sign of offset is opposite to UTC offset
			char utc_sign = (sign == '-') ? '+' : '-';

			if (sscanf(sign_pos + 1, "%d:%d", &hours, &minutes) >= 1) {
				snprintf(tz_display, sizeof(tz_display), "UTC%c%02d:%02d", utc_sign, hours, minutes);
			}
		}
	}

	at_printf("\r\n+SNTPTIME:%d-%02d-%02d %02d:%02d:%02d %s\r\n",
			  timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
			  timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, tz_display);

	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}
#endif /* CONFIG_ATCMD_SNTP */

ATCMD_APONLY_TABLE_DATA_SECTION
const log_item_t at_network_items[ ] = {
	{"+PING", at_ping},
	{"+IPERF", at_iperf},
	{"+IPERF3", at_iperf3},
#if defined(CONFIG_ATCMD_DNS) && (CONFIG_ATCMD_DNS == 1)
	{"+DNS", at_dns},
	{"+QUERYDNSSRV", at_querydnssrv},
	{"+SETDNSSRV", at_setdnssrv},
#endif
#if defined(CONFIG_ATCMD_MDNS) && (CONFIG_ATCMD_MDNS == 1)
	{"+MDNS", at_mdns},
#endif
#if defined(CONFIG_ATCMD_SNTP) && (CONFIG_ATCMD_SNTP == 1)
	{"+SNTPCFG", at_sntpcfg},
	{"+SNTPQUERY", at_sntpquery},
	{"+SNTPTIME", at_sntptime},
#endif
};

void print_network_at(void)
{
	int i, cmdSize;

	cmdSize = sizeof(at_network_items) / sizeof(log_item_t);
	for (i = 0; cmdSize > i; i++) {
		at_printf("AT%s\r\n", at_network_items[i].log_cmd);
	}
}

#endif /* CONFIG_ATCMD_NETWORK */
#endif /* CONFIG_LWIP_LAYER */
#endif /* CONFIG_MP_SHRINK */
