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
_WEAK void cmd_iperf3(int argc, char **argv)
{
	UNUSED(argc);
	UNUSED(argv);
	RTK_LOGW(NOTAG, " iperf3 is not supported yet\r\n");
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
		cmd_iperf3(argc, argv);
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

log_item_t at_network_items[ ] = {
	{"+DNS", at_dns, {NULL, NULL}},
	{"+QUERYDNSSRV", at_querydnssrv, {NULL, NULL}},
	{"+SETDNSSRV", at_setdnssrv, {NULL, NULL}},
	{"+PING", at_ping, {NULL, NULL}},
	{"+IPERF", at_iperf, {NULL, NULL}},
	{"+IPERF3", at_iperf3, {NULL, NULL}},
};

void print_network_at(void)
{
	int i, cmdSize;

	cmdSize = sizeof(at_network_items) / sizeof(log_item_t);
	for (i = 0; cmdSize > i; i++) {
		at_printf("AT%s\r\n", at_network_items[i].log_cmd);
	}
}

void at_network_init(void)
{
	atcmd_service_add_table(at_network_items, sizeof(at_network_items) / sizeof(at_network_items[0]));
}

#endif /* CONFIG_ATCMD_NETWORK */
#endif /* CONFIG_LWIP_LAYER */
#endif /* CONFIG_MP_SHRINK */