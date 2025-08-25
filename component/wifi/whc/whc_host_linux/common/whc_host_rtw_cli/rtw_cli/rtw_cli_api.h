#ifndef _RTW_CLI_API_H_
#define _RTW_CLI_API_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>             /* offsetof */
#include <stdint.h>
#include <string.h>
#include <rtw_cli_wifi_types.h>
#include <rtw_cli_netlink.h>

#define MAX_ARG_COUNT 5
#define MAX_ARG_LENGTH 128
#define MAX_INPUT_SIZE 640

#define IFNAME "eth_sta0"
#define ifreq_offsetof(x)  offsetof(struct ifreq, x)

extern int num_of_ap;
extern int num_of_ap_index;
extern struct rtw_scan_result *scan_results;

extern int family_id;
extern int sock_fd;


#define MSG_EVENT_SCANING "CTRL-EVENT-SCANING"
#define MSG_EVENT_SCAN_COMPLETE "CTRL-EVENT-SCAN-PROCESS-COMPLETE "
#define MSG_EVENT_GET_SCAN_RESULT "CTRL-EVENT-GET-SCAN-RESULT "

#define MSG_EVENT_FAIL_BUSY "FAIL-BUSY"



//For Event handle

typedef void (*event_handler_t)(void);

struct event_func_t {
	u32 wifi_event;
	u8 *event_msg;
	event_handler_t handler;
};

//For Send CMD
struct whc_host_cli_cmd_t {
	u32 category;       // WHC_WIFI_TEST
	u8  cmd_id;         // WHC_WIFI_CMD_SCAN
	u8  idx;            // For Interface
	u8  TBD[7];
} __attribute__((packed));



struct whc_host_cli_send_string_t {
	u32 category;       // WHC_WIFI_TEST
	u8  cmd_id;         // WHC_WIFI_CMD_SCAN
	u32 msg_len;
	u8  message[256];
} __attribute__((packed));


typedef void (*cmd_handler_t)(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id);

struct cmd_func_t {
	const char *cmd;
	cmd_handler_t handler;
	u8  api_id;         // For netlink category
	u32 cmd_category;
	u8 cmd_id;
};

/* API End */



/* buf in user space send to kernel, final kernel to device */
void rtw_cli_cmd_scan(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id);
void rtw_cli_cmd_scan_result(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id);
void rtw_cli_cmd_getmac(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id);
void rtw_cli_cmd_set_network(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id);
void rtw_cli_cmd_get_join_status(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id);

void rtw_cli_cmd_wifi_on(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id);
void rtw_cli_cmd_wpas_on(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id);

void rtw_cli_cmd_mp(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id);
void rtw_cli_cmd_dbg(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id);

void print_scan_result(struct rtw_scan_result *record);
void whc_cmd_handle_rx_print_scan_result(char *pos, int chunk_index,
		char last_chunk);

void event_handler_scan_complete(void);
void whc_cmd_handle_rx_list_network(char *pos);
void whc_cmd_handle_rx_get_status(char *pos);

void whc_cmd_handle_rx_payload(char *pos, int len, int api_id,
							   int chunk_index, char last_chunk);

#endif
