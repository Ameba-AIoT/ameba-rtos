#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <json-c/json.h>
#include <signal.h>
#include <time.h>
#include "rtw_nan_struct.h"
#include "rtw_nan_scenario_api.h"

#define JSON_TOOL_VER "json_tool v1.0.1"

#define CPY_OBJ_STR(charp,obj) \
	strcpy(charp, json_object_get_string(obj))

#define NDP_ID 1
#define MAC_STR_LEN 17
#define RETRY_MAX 3
#define MAX_SERVICE_NUM 2
#define MAX_CMD_LEN 1024
#define REPETAT_TEST_NUM 0


#define COLOR_NONE "\033[m"
#define COLOR_RED "\033[0;32;31m"
#define COLOR_BLUE "\033[0;32;34m"
#define COLOR_YELLOW "\033[1;33m"
#define PRPFILE_TEXT "[rtw_aware] "

enum {
	_AWARE_NONE_ = 0,
	_AWARE_ALWAYS_ = 1,
	_AWARE_ERR_ = 2,
	_AWARE_WARNING_ = 3,
	_AWARE_INFO_ = 4,
	_AWARE_DEBUG_ = 5,
	_AWARE_MAX_ = 6
};
int g_log_level = _AWARE_DEBUG_;

#define RTW_INFO(fmt, arg...) \
	do {\
		if (_AWARE_INFO_ <= g_log_level) {\
			printf(COLOR_NONE PRPFILE_TEXT fmt COLOR_NONE, ##arg);\
		} \
	} while (0)
#define RTW_DBG(fmt, arg...) \
	do {\
		if (_AWARE_DEBUG_ <= g_log_level) {\
			printf(COLOR_NONE PRPFILE_TEXT fmt COLOR_NONE, ##arg);\
		} \
	} while (0)
#define RTW_WARN(fmt, arg...) \
	do {\
		if (_AWARE_WARNING_ <= g_log_level) {\
			printf(COLOR_YELLOW PRPFILE_TEXT fmt COLOR_NONE, ##arg);\
		} \
	} while (0)
#define RTW_ERR(fmt, arg...) \
	do {\
		if (_AWARE_ERR_ <= g_log_level) {\
			printf(COLOR_RED PRPFILE_TEXT fmt COLOR_NONE, ##arg);\
		} \
	} while (0)

enum nan_status {
	NAN_STATUS_NONE = 0,
	NAN_STATUS_SEND_REQ = 1,
	NAN_STATUS_RECV_REQ = 2,
	NAN_STATUS_SEND_RSP = 3,
	NAN_STATUS_RECV_RSP = 4,
	NAN_STATUS_SEND_CONFIRM = 5,
	NAN_STATUS_RECV_CONFIRM = 6,
	NAN_STATUS_SEND_SEC_INSTALL = 7,
	NAN_STATUS_RECV_SEC_INSTALL = 8,
	NAN_STATUS_NDP_ESTABLISH = 9,
	NAN_STATUS_BSTRAP_START = 10,
	NAN_STATUS_GET_BSTRAP_RESULT = 11,
	NAN_STATUS_PASN_START = 12,
	NAN_STATUS_GET_PASN_RESULT = 13,
};

enum rtw_security_type {
	RTW_SECURITY_OPEN = 0,
	RTW_SECURITY_PMK,
	RTW_SECURITY_PAIRING,
	RTW_SECURITY_UNKNOWN,
};

enum pairing_bstrap_method {
	OPPORTUNISTIC_BSTRAP         = BIT(0),
	PIN_CODE_DISPLAY             = BIT(1),
	PASSPHRASE_DISPLAY           = BIT(2),
	QR_CODE_DISPLAY              = BIT(3),
	NFC_TAG                      = BIT(4),
	PIN_CODE_KEYPAD              = BIT(5),
	PASSPHRASE_KEYPAD            = BIT(6),
	QR_CODE_SCAN                 = BIT(7),
	NFC_READER                   = BIT(8),
	SERVICE_MANAGED_BSTRAP       = BIT(14),
	BRSTAP_SKIPPED               = BIT(15),
};


struct nan_data_mgnt {
	uint8_t role; /* enum rtw_aware_role */
	uint8_t nan_status; /* enum nan_status */
	uint8_t data_path_retry;
	struct srvc_info service;
	struct datapath_info data_path;
};

struct dns_sd_config {
	uint8_t id; /* publish or subscribe id */
	uint8_t service_name[MAX_SRVC_NAME_LEN];
	uint8_t service_instance[MAX_SRVC_INST_LEN];
	uint16_t text_info_len;
	uint8_t *text_info;
	uint8_t hostname[256];
	uint16_t port_number;
	uint8_t start_cmd[MAX_CMD_LEN];
	uint8_t end_cmd[MAX_CMD_LEN];
	uint8_t security_option;
	uint8_t pmk[NAN_PMK_SIZE];
	uint16_t bstrap_method;
	char pairing_pw[MAX_PASN_PW_LEN + 1];
	uint8_t nik_caching;
	uint8_t gtk_enable;
};

struct nan_service_mgnt {
	struct nan_data_mgnt nan_mgnt;
	struct dns_sd_config dns_record;
};

struct basic_config {
	char device_mac[MAC_STR_LEN];
};

static int __main_terminated;
static unsigned int max_service_time_sec = 0;
char *rtw_iw;
char *rtw_nan_r3;
char *nan_intf = "nan0";
volatile sig_atomic_t sig_flag = 0;
timer_t timerid;
static unsigned int g_num_service = 2;
struct nan_service_mgnt g_nan_service_mgnt[MAX_SERVICE_NUM] = {0};
static unsigned int g_test_cnt = 0;


bool is_array_empty(unsigned char *input, unsigned int len)
{
	char tmp_arry[512] = {0};
	return memcmp(input, tmp_arry, (len > 512) ? 512 : len) == 0;
}

void signal_func(int sig)
{
	sig_flag = 1;
}

void clear_data_path(struct nan_data_mgnt *nan_data_mgnt)
{
	struct datapath_info *data_path;

	if (!nan_data_mgnt) {
		return;
	}

	data_path = &nan_data_mgnt->data_path;

	RTW_INFO("clear_data_path nan_status[%d]\n", nan_data_mgnt->nan_status);

	nan_data_mgnt->data_path_retry = 0;
	nan_data_mgnt->data_path.ndp_id = 0;
	memset(data_path->initiator_ndi, 0, ETH_ALEN);
	memset(data_path->responder_ndi, 0, ETH_ALEN);
	memset(data_path->initiator_intf_identifier, 0, ETH_ALEN);
	memset(data_path->responder_intf_identifier, 0, ETH_ALEN);
	data_path->sec_type = 0;

	if (nan_data_mgnt->nan_status == NAN_STATUS_NDP_ESTABLISH ||
		nan_data_mgnt->nan_status == NAN_STATUS_SEND_REQ ||
		nan_data_mgnt->nan_status == NAN_STATUS_SEND_RSP ||
		nan_data_mgnt->nan_status == NAN_STATUS_BSTRAP_START ||
		nan_data_mgnt->nan_status == NAN_STATUS_PASN_START) {
		nan_data_mgnt->nan_status = NAN_STATUS_NONE;
	}
}

void clear_pairing_info(struct nan_data_mgnt *nan_data_mgnt)
{
	struct pairing_info *pairing_info;

	if (!nan_data_mgnt) {
		return;
	}

	pairing_info = &nan_data_mgnt->service.pairing_info;

	RTW_INFO("clear_pairing_info nan_status[%d]\n", nan_data_mgnt->nan_status);

	pairing_info->role = 0;
	memset(pairing_info->peer_nmi, 0, ETH_ALEN);
	pairing_info->pasn_type = 0;
	pairing_info->pasn_auth_method = 0;
}

void wait_time(uint16_t waiting_sec)
{
	double diff_s = 0;
	time_t service_start_t = time(NULL);

	while (waiting_sec) {
		diff_s = difftime(time(NULL), service_start_t);
		if (diff_s > waiting_sec || sig_flag) {
			sig_flag = 1;
			break;
		}
		sleep(1);
	}
}

void timer_handler()
{
	struct nan_data_mgnt *tmp_nan_data_mgnt = NULL;
	int i = 0;

	for (i = 0; i < g_num_service; i++) {
		tmp_nan_data_mgnt = &g_nan_service_mgnt[i].nan_mgnt;
		tmp_nan_data_mgnt->data_path_retry++;
		switch (tmp_nan_data_mgnt->nan_status) {
		case NAN_STATUS_SEND_REQ:
			if (tmp_nan_data_mgnt->data_path.ndp_id &&
				tmp_nan_data_mgnt->data_path_retry > RETRY_MAX) {
				RTW_INFO("Data rsp not receive! nan_status[%d],"\
						 "retry[%d],send data path end[%d]\n",
						 tmp_nan_data_mgnt->nan_status,
						 tmp_nan_data_mgnt->data_path.ndp_id,
						 tmp_nan_data_mgnt->data_path_retry);
				dns_datapath_end(tmp_nan_data_mgnt->data_path.ndp_id);
				clear_data_path(tmp_nan_data_mgnt);
				timer_delete(timerid);
			}
			break;
		case NAN_STATUS_SEND_RSP:
			if (tmp_nan_data_mgnt->data_path.sec_type &&
				tmp_nan_data_mgnt->data_path.ndp_id &&
				tmp_nan_data_mgnt->data_path_retry > RETRY_MAX) {
				RTW_INFO("Data confirm not receive! nan_status[%d],"\
						 "retry[%d],send data path end[%d]\n",
						 tmp_nan_data_mgnt->nan_status,
						 tmp_nan_data_mgnt->data_path_retry,
						 tmp_nan_data_mgnt->data_path.ndp_id);
				dns_datapath_end(tmp_nan_data_mgnt->data_path.ndp_id);
				clear_data_path(tmp_nan_data_mgnt);
				timer_delete(timerid);
			}
			break;
		case NAN_STATUS_BSTRAP_START:
			if (tmp_nan_data_mgnt->data_path_retry > RETRY_MAX) {
				RTW_INFO("Boostrapping method result not receive! "\
						 "nan_status[%d], retry[%d]\n",
						 tmp_nan_data_mgnt->nan_status,
						 tmp_nan_data_mgnt->data_path_retry);
				/* todo: reset drv bstrap and start again */
				clear_pairing_info(tmp_nan_data_mgnt);
				clear_data_path(tmp_nan_data_mgnt);
				timer_delete(timerid);
			}
			break;
		case NAN_STATUS_PASN_START:
			if (tmp_nan_data_mgnt->data_path_retry > RETRY_MAX) {
				RTW_INFO("PASN result not receive! nan_status[%d],"\
						 "retry[%d]\n",
						 tmp_nan_data_mgnt->nan_status,
						 tmp_nan_data_mgnt->data_path_retry);
				/* todo: the value assigned during bstrap cannot be cleared */
				clear_pairing_info(tmp_nan_data_mgnt);
				clear_data_path(tmp_nan_data_mgnt);
				timer_delete(timerid);
			}
			break;
		default:
			break;
		}
		if (tmp_nan_data_mgnt->data_path_retry > RETRY_MAX) {
			timer_delete(timerid);
		}
	}
}

void creat_timer()
{
	struct sigevent sev;
	struct itimerspec its;

	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGUSR1;
	sev.sigev_value.sival_ptr = &timerid;
	signal(SIGUSR1, timer_handler);

	if (timer_create(CLOCK_MONOTONIC, &sev, &timerid) == -1) {
		RTW_INFO("timer_create");
	}

	its.it_value.tv_sec = 5;
	its.it_value.tv_nsec = 0;
	its.it_interval.tv_sec = its.it_value.tv_sec;
	its.it_interval.tv_nsec = its.it_value.tv_nsec;

	if (timer_settime(timerid, 0, &its, NULL) == -1) {
		RTW_INFO("timer_settime");
	}
}

void mac_to_ipv6_link_local(char *mac, char *ipv6_link_local)
{
	char mac_source[128] = {0}, ipv6_addr[128] = {0};
	char delim[] = ":";
	uint8_t i = 0, invert_val = 0, bit1 = 1 << 1;
	uint8_t mac_hex[ETH_ALEN] = {0};
	uint16_t ipv6_hex[5] = {0};
	uint8_t insert_byte[2] = {0xFF, 0xFE};
	uint8_t link_local_prefix[2] = {0xFe, 0x80};

	snprintf(mac_source, sizeof(mac_source), "%s", mac);
	char *ptr = strtok(mac_source, delim);

	for (i = 0; i < 6; i++) {
		mac_hex[i] = (uint8_t)strtol(ptr, NULL, 16);
		ptr = strtok(NULL, delim);
	}

	// invert the bit at index 6 (counting from 0)
	invert_val = mac_hex[0] ^ bit1;

	// insert ff:fe and reformat to IPv6
	ipv6_hex[0] = (link_local_prefix[0] << 8) + link_local_prefix[1];
	ipv6_hex[1] = (invert_val << 8) + mac_hex[1];
	ipv6_hex[2] = (mac_hex[2] << 8) + insert_byte[0];
	ipv6_hex[3] = (insert_byte[1] << 8) + mac_hex[3];
	ipv6_hex[4] = (mac_hex[4] << 8) + mac_hex[5];

	sprintf(ipv6_addr, "%x::%x:%x:%x:%x",
			ipv6_hex[0], ipv6_hex[1], ipv6_hex[2], ipv6_hex[3], ipv6_hex[4]);
	strcpy(ipv6_link_local, ipv6_addr);
	RTW_INFO("ipv6_link_local: %s\n", ipv6_link_local);
}

void set_self_ipv6(char *intf, char *mac_addr, char *self_ipv6)
{
	char cmd[100];
	RTW_INFO("%s[%s], mac[%s]\n", __func__, self_ipv6, mac_addr);

	/* set ipv6 address */
	snprintf(cmd, sizeof(cmd), "%s %s/64 %s %s",
			 "ip -6 address add",
			 self_ipv6,
			 "dev",
			 intf);
	system(cmd);

	/* set routing table */
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "%s %s/64 %s %s",
			 "ip -6 route add",
			 self_ipv6,
			 "dev",
			 intf);
	system(cmd);
}

void set_peer_ipv6(char *intf, char *peer_mac, char *peer_ipv6)
{
	char cmd[100] = {0};

	RTW_INFO("%s[%s], mac[%s]\n", __func__, peer_ipv6, peer_mac);
	/* Delete before adding */
	snprintf(cmd, sizeof(cmd), "%s %s %s %s %s %s",
			 "ip -6 neigh del",
			 peer_ipv6,
			 "lladdr",
			 peer_mac,
			 "dev",
			 intf);
	system(cmd);

	/* set ipv6 neighbor */
	snprintf(cmd, sizeof(cmd), "%s %s %s %s %s %s",
			 "ip -6 neigh add",
			 peer_ipv6,
			 "lladdr",
			 peer_mac,
			 "dev",
			 intf);
	system(cmd);
}

void ping_ipv6(char *intf, char *peer_ipv6, int count)
{
	char cmd[100] = {0};
	sleep(2);
	snprintf(cmd, sizeof(cmd), "%s %s %s %d %s",
			 "ping6 -I",
			 intf,
			 "-c",
			 count,
			 peer_ipv6);

	RTW_INFO("%s\n", cmd);
	system(cmd);
}

void parse_discovery_result(struct nan_data_mgnt *nan_data_mgnt, struct dns_evt_disc_result_info *info)
{
	uint16_t i = 0;
	/* check current service record, avoid allocating text_info multiple times */
	if (nan_data_mgnt->service.text_info_len) {
		RTW_DBG("already save a service, skip\n");
		return;
	}

	RTW_INFO("%s\n", __func__);

	RTW_INFO("subscribe_id(%d), publisher_id(%d)\n",
			 info->subscribe_id, info->publisher_id);
	RTW_INFO("peer_nmi(%02x:%02x:%02x:%02x:%02x:%02x)\n",
			 info->peer_nmi[0], info->peer_nmi[1], info->peer_nmi[2],
			 info->peer_nmi[3], info->peer_nmi[4], info->peer_nmi[5]);
	RTW_INFO("service_id(%02x:%02x:%02x:%02x:%02x:%02x)\n",
			 info->service_id[0], info->service_id[1], info->service_id[2],
			 info->service_id[3], info->service_id[4], info->service_id[5]);
	RTW_INFO("service_name(%s), service_instance(%s)\n",
			 info->service_name, info->service_instance);
	if (info->text_info_len && info->text_info) {
		RTW_INFO("text_info_len(%d)\n", info->text_info_len);
		RTW_INFO("text_info ");
		for (i = 0; i < info->text_info_len; i++) {
			printf("%02x ", info->text_info[i]);
		}
		RTW_INFO("\n");
	}
	RTW_INFO("pairing_enable(%d), paired(%d)\n", info->pairing_enable, info->paired);

	/* save the result, only subscriber will recv disc result */
	nan_data_mgnt->service.publish_id = info->publisher_id;
	memcpy(nan_data_mgnt->service.publish_nmi, info->peer_nmi, ETH_ALEN);
	memcpy(nan_data_mgnt->service.service_name, info->service_name, MAX_SRVC_NAME_LEN);
	memcpy(nan_data_mgnt->service.service_instance, info->service_instance, MAX_SRVC_INST_LEN);
	if (info->text_info_len && info->text_info) {
		nan_data_mgnt->service.text_info_len = info->text_info_len;
		/* note: free at the end of main function */
		nan_data_mgnt->service.text_info = calloc(info->text_info_len, sizeof(uint8_t));
		memcpy(nan_data_mgnt->service.text_info, info->text_info, info->text_info_len);
	}
}

void parse_data_indication(struct nan_data_mgnt *nan_data_mgnt, struct dns_evt_data_indic_info *info, uint32_t len)
{
	RTW_INFO("%s\n", __func__);

	RTW_INFO("type(%d), publish_id(%d), ndp_id(%d), mc_id(%d)\n",
			 info->type, info->publish_id, info->ndp_id, info->mc_id);
	PRINT_MAC_ADDR("initiator_ndi", info->initiator_ndi);
	PRINT_MAC_ADDR("responder_ndi", info->responder_ndi);
	RTW_INFO("security(%d)\n", info->security);
	PRINT_IPV6("ipv6", info->ipv6_intf_identifier);

	/* fill in datapath info, only publisher will recv disc indication */
	nan_data_mgnt->data_path.ndp_id = info->ndp_id;
	nan_data_mgnt->data_path.sec_type = 0;
	if (info->security) {
		nan_data_mgnt->data_path.sec_type = NAN_PMK_SET_BY_USER_PMK;
		if (nan_data_mgnt->nan_status == NAN_STATUS_GET_PASN_RESULT) {
			nan_data_mgnt->data_path.sec_type = NAN_PMK_SET_BY_PAIRING;
		}
	}
	memcpy(nan_data_mgnt->data_path.initiator_ndi, info->initiator_ndi, ETH_ALEN);
}

void prepare_pairing_info_as_initiator(struct nan_data_mgnt *nan_data_mgnt)
{
	struct srvc_info *srvc_data = &nan_data_mgnt->service;
	struct pairing_info *pairing_data = &nan_data_mgnt->service.pairing_info;

	RTW_INFO("%s\n", __func__);

	pairing_data->role = PAIRING_ROLE_INITIATOR;
	memcpy(pairing_data->peer_nmi, srvc_data->publish_nmi, ETH_ALEN);
}

struct dns_sd_config *get_dns_recored_by_ndp_id(int ndp_id)
{
	int i;

	if (!ndp_id) {
		return NULL;
	}

	for (i = 0; i < g_num_service; i++) {
		if (g_nan_service_mgnt[i].nan_mgnt.data_path.ndp_id == ndp_id) {
			return &g_nan_service_mgnt[i].dns_record;
		}
	}
	RTW_WARN("%s dns_record not found, ndp_id=%d\n", __func__, ndp_id);
	return NULL;
}

struct nan_data_mgnt *get_nan_data_mgnt_by_ndp_id(int ndp_id)
{
	int i;

	if (!ndp_id) {
		return NULL;
	}

	for (i = 0; i < g_num_service; i++) {
		if (g_nan_service_mgnt[i].nan_mgnt.data_path.ndp_id == ndp_id) {
			return &g_nan_service_mgnt[i].nan_mgnt;
		}
	}
	RTW_WARN("%s nan_data_mgnt not found, ndp_id=%d\n", __func__, ndp_id);
	return NULL;
}

struct nan_data_mgnt *get_nan_data_mgnt_by_pub_id(int pub_id)
{
	int i;
	for (i = 0; i < g_num_service; i++) {
		if (g_nan_service_mgnt[i].nan_mgnt.service.publish_id == pub_id) {
			return &g_nan_service_mgnt[i].nan_mgnt;
		}
	}
	RTW_DBG("%s dns_recored not found, pub_id=%d\n", __func__, pub_id);
	return NULL;
}

struct nan_data_mgnt *get_nan_data_mgnt_by_sub_id(int sub_id)
{
	int i;
	for (i = 0; i < g_num_service; i++) {
		if (g_nan_service_mgnt[i].nan_mgnt.service.subscribe_id == sub_id) {
			return &g_nan_service_mgnt[i].nan_mgnt;
		}
	}
	RTW_DBG("%s dns_recored not found, sub_id=%d\n", __func__, sub_id);
	return NULL;
}

struct nan_data_mgnt *get_nan_data_mgnt_by_pairing()
{
	int i;
	for (i = 0; i < g_num_service; i++) {
		if (g_nan_service_mgnt[i].nan_mgnt.service.sec_type == NAN_PMK_SET_BY_PAIRING) {
			return &g_nan_service_mgnt[i].nan_mgnt;
		}
	}
	RTW_DBG("%s get_nan_data_mgnt_by_pairing not found\n", __func__);
	return NULL;
}



void dns_evt_discovery_result(struct dns_evt_disc_result_info *info, uint32_t len)
{
	int i = 0;
	struct nan_data_mgnt *nan_mgnt = NULL;
	uint8_t mac_tmp[ETH_ALEN] = {0};

	if (len != sizeof(struct dns_evt_disc_result_info) || !info) {
		RTW_INFO("info or len(%d) error\n", len);
		return;
	}

	// If the rtw_aware is subscribe role, then the index of nan_mgnt is 0.
	if (memcmp(g_nan_service_mgnt[0].nan_mgnt.service.subscribe_nmi, mac_tmp, ETH_ALEN)) {
		nan_mgnt = &g_nan_service_mgnt[0].nan_mgnt;
	} else {
		for (i = 0; i < MAX_SERVICE_NUM; i++) {
			if (g_nan_service_mgnt[i].nan_mgnt.service.publish_id == info->publisher_id) {
				nan_mgnt = &g_nan_service_mgnt[i].nan_mgnt;
				break;
			}
		}
	}
	if (!nan_mgnt) {
		RTW_INFO("%s, publish id is mismatch!\n", __func__);
		return;
	}

	RTW_INFO("%s, nan_status[%d]\n", __func__, nan_mgnt->nan_status);

	parse_discovery_result(nan_mgnt, info);

	if (nan_mgnt->nan_status == NAN_STATUS_NONE) {
		char initiator_ndi_s[18] = {0};
		char self_ipv6[128] = {0};

		/* fill in datapath info*/
		nan_mgnt->data_path.ndp_id = NDP_ID;
		memcpy(nan_mgnt->data_path.responder_ndi,
			   nan_mgnt->data_path.service->publish_nmi, ETH_ALEN);

		sprintf(initiator_ndi_s, "%02x:%02x:%02x:%02x:%02x:%02x",
				nan_mgnt->data_path.service->subscribe_nmi[0],
				nan_mgnt->data_path.service->subscribe_nmi[1],
				nan_mgnt->data_path.service->subscribe_nmi[2],
				nan_mgnt->data_path.service->subscribe_nmi[3],
				nan_mgnt->data_path.service->subscribe_nmi[4],
				nan_mgnt->data_path.service->subscribe_nmi[5]);

		/* set self ipv6 in dns_subscribe */
		mac_to_ipv6_link_local(initiator_ndi_s, self_ipv6);
		set_self_ipv6(nan_intf, initiator_ndi_s, self_ipv6);

		RTW_INFO("peer's security: pairing_enable[%d], pmk_set[%d], paired[%d]\n",
				 info->pairing_enable, info->pmk_set, info->paired);

		if (info->pairing_enable) {
			if (nan_mgnt->service.sec_type != NAN_PMK_SET_BY_PAIRING) {
				RTW_INFO("Security is mismatched, sec_type[%d]\n",
						 nan_mgnt->service.sec_type);
				return;
			}
			/* start pairing setup or pairing verification flow */
			nan_mgnt->data_path.sec_type = NAN_PMK_SET_BY_PAIRING;
			prepare_pairing_info_as_initiator(nan_mgnt);

			if (info->paired) {
				RTW_INFO("Pairing verification start\n");
				nan_mgnt->nan_status = NAN_STATUS_PASN_START;
				nan_mgnt->service.pairing_info.pasn_type = NAN_PAIRING_TYPE_VERIFICATION;
				if (dns_pairing_start(&nan_mgnt->service.pairing_info) == RTW_RET_STATUS_SUCCESS) {
					nan_mgnt->data_path_retry = 0;
					creat_timer();
				}
			} else {
				RTW_INFO("Pairing setup start\n");
				nan_mgnt->nan_status = NAN_STATUS_BSTRAP_START;
				nan_mgnt->service.pairing_info.pasn_type = NAN_PAIRING_TYPE_SETUP;
				if (dns_bstrap_start(&nan_mgnt->service) == RTW_RET_STATUS_SUCCESS) {
					RTW_INFO("send bootstrapping request success!!!\n");
					nan_mgnt->data_path_retry = 0;
					creat_timer();
				}
			}
		} else if (info->pmk_set) {
			/* PMK */
			if (nan_mgnt->service.sec_type != NAN_PMK_SET_BY_USER_PMK) {
				RTW_INFO("Security is mismatched, sec_type[%d]\n",
						 nan_mgnt->service.sec_type);
				return;
			}
			nan_mgnt->data_path.sec_type = NAN_PMK_SET_BY_USER_PMK;
			if (dns_datapath_req(&nan_mgnt->data_path) == RTW_RET_STATUS_SUCCESS) {
				RTW_INFO("send data path success!!!\n");
				nan_mgnt->nan_status = NAN_STATUS_SEND_REQ;
				nan_mgnt->data_path_retry = 0;
				creat_timer();
			}
		} else {
			/* OPEN */
			if (nan_mgnt->service.sec_type != 0) {
				RTW_INFO("Security is mismatched, sec_type[%d]\n",
						 nan_mgnt->service.sec_type);
				return;
			}
			nan_mgnt->data_path.sec_type = 0;
			if (dns_datapath_req(&nan_mgnt->data_path) == RTW_RET_STATUS_SUCCESS) {
				RTW_INFO("send data path success!!!\n");
				nan_mgnt->nan_status = NAN_STATUS_SEND_REQ;
				nan_mgnt->data_path_retry = 0;
			}
		}
	}
}

void dns_evt_data_indication(struct dns_evt_data_indic_info *info, uint32_t len)
{
	int i;
	struct nan_data_mgnt *tmp_nan_data_mgnt = NULL;
	uint8_t mac_tmp[ETH_ALEN] = {0};

	if (len != sizeof(struct dns_evt_data_indic_info) || !info) {
		RTW_INFO("info or len(%d) error!\n", len);
		return;
	}

	tmp_nan_data_mgnt = get_nan_data_mgnt_by_pub_id(info->publish_id);
	if (tmp_nan_data_mgnt == NULL) {
		return;
	}

	RTW_INFO("%s, nan_status[%d]\n", __func__, tmp_nan_data_mgnt->nan_status);

	/* reset timer for data request */
	if (timerid) {
		timer_delete(timerid);
	}

	parse_data_indication(tmp_nan_data_mgnt, info, len);

	/* send nan data response frame*/
	tmp_nan_data_mgnt->nan_status = NAN_STATUS_RECV_REQ;
	if (dns_datapath_rsp(&tmp_nan_data_mgnt->data_path) == RTW_RET_STATUS_SUCCESS) {
		tmp_nan_data_mgnt->nan_status = NAN_STATUS_SEND_RSP;
		if (tmp_nan_data_mgnt->data_path.sec_type) {
			tmp_nan_data_mgnt->data_path_retry = 0;
			creat_timer();
		}
	}
}

void dns_evt_data_confirm(struct dns_evt_data_comfirm_info *info, uint32_t len)
{
	char initiator_ndi_s[18] = {0};
	char responder_ndi_s[18] = {0};
	char self_ipv6[128] = {0};
	char peer_ipv6[128] = {0};
	int i;
	struct nan_data_mgnt *tmp_nan_data_mgnt = NULL;
	uint8_t mac_tmp[ETH_ALEN] = {0};

	if (len != sizeof(struct dns_evt_data_comfirm_info) || !info) {
		RTW_INFO("info or len(%d) error!\n", len);
		return;
	}

	tmp_nan_data_mgnt = get_nan_data_mgnt_by_ndp_id(info->ndp_id);
	if (tmp_nan_data_mgnt == NULL) {
		return;
	}

	if (timerid) {
		timer_delete(timerid);
	}

	RTW_INFO("%s, nan_status[%d]\n", __func__, tmp_nan_data_mgnt->nan_status);
	RTW_INFO("type(%d), status(%d), ndp_id(%d), mc_id(%d), nmsg_id(%d)\n",
			 info->type, info->status, info->ndp_id, info->mc_id, info->nmsg_id);
	PRINT_MAC_ADDR("initiator_ndi", info->initiator_ndi);
	PRINT_MAC_ADDR("responder_ndi", info->responder_ndi);
	RTW_INFO("host_name(%s), port_number(%d)\n", info->host_name, info->port_number);
	PRINT_IPV6("peer ipv6", info->ipv6_intf_identifier);

	if (info->status) {
		RTW_INFO("%s, ndp setup fail!\n", __func__);
		clear_pairing_info(tmp_nan_data_mgnt);
		clear_data_path(tmp_nan_data_mgnt);
		return;
	}

	tmp_nan_data_mgnt->nan_status = NAN_STATUS_NDP_ESTABLISH;

	sprintf(initiator_ndi_s, "%02x:%02x:%02x:%02x:%02x:%02x",
			info->initiator_ndi[0], info->initiator_ndi[1],
			info->initiator_ndi[2], info->initiator_ndi[3],
			info->initiator_ndi[4], info->initiator_ndi[5]);
	sprintf(responder_ndi_s, "%02x:%02x:%02x:%02x:%02x:%02x",
			info->responder_ndi[0], info->responder_ndi[1],
			info->responder_ndi[2], info->responder_ndi[3],
			info->responder_ndi[4], info->responder_ndi[5]);

	/* publisher works as responder, subscriber works as initiator */
	if (tmp_nan_data_mgnt->role == RTW_AWARE_ROLE_PUBLISH) {
		mac_to_ipv6_link_local(responder_ndi_s, self_ipv6);
		set_self_ipv6(nan_intf, responder_ndi_s, self_ipv6);
		mac_to_ipv6_link_local(initiator_ndi_s, peer_ipv6);
		set_peer_ipv6(nan_intf, initiator_ndi_s, peer_ipv6);

	} else {
		/* set self ipv6 in dns_subscribe */
		mac_to_ipv6_link_local(responder_ndi_s, peer_ipv6);
		set_peer_ipv6(nan_intf, responder_ndi_s, peer_ipv6);
		sleep(3);
		ping_ipv6(nan_intf, peer_ipv6, MAX_SERVICE_TIME - 20);
		if (g_test_cnt < REPETAT_TEST_NUM) {
			g_test_cnt ++;
			dns_datapath_end(info->ndp_id);
			sleep(3);
		}
	}
}

void exec_start_script(struct dns_evt_data_established_info *info, uint32_t len)
{
	int index;
	char cmd[MAX_CMD_LEN] = {0};
	struct dns_sd_config *tmp;

	RTW_INFO("%s \n", __func__);

	if (len != sizeof(struct dns_evt_data_established_info) || !info) {
		RTW_INFO("info or len(%d) error!\n", len);
		return;
	}

	tmp = get_dns_recored_by_ndp_id(info->ndp_id);
	if (tmp == NULL) {
		RTW_INFO("%s Can not get dns_recored from ndp_id(%d)\n",
				 __func__, info->ndp_id);
		return;
	}

	RTW_DBG("%s \n", tmp->start_cmd);
	snprintf(cmd, sizeof(cmd), "%s", tmp->start_cmd);
	system(cmd);

}

void exec_stop_script(struct dns_evt_data_end_info *info, uint32_t len)
{
	int index;
	char cmd[MAX_CMD_LEN] = {0};
	struct dns_sd_config *tmp;

	RTW_INFO("%s \n", __func__);

	if (len != sizeof(struct dns_evt_data_end_info) || !info) {
		RTW_INFO("info or len(%d) error!\n", len);
		return;
	}

	tmp = get_dns_recored_by_ndp_id(info->ndp_id);
	if (tmp == NULL) {
		return;
	}

	snprintf(cmd, sizeof(cmd), "%s", tmp->end_cmd);
	system(cmd);
}

void exec_stop_script_ex(uint8_t ndp_id)
{
	int index;
	char cmd[MAX_CMD_LEN] = {0};
	struct dns_sd_config *tmp;

	RTW_INFO("%s \n", __func__);

	tmp = get_dns_recored_by_ndp_id(ndp_id);
	if (tmp == NULL) {
		return;
	}

	snprintf(cmd, sizeof(cmd), "%s", tmp->end_cmd);
	system(cmd);
}

void dns_evt_data_end(struct dns_evt_data_end_info *info, uint32_t len)
{
	struct nan_data_mgnt *tmp_nan_data_mgnt = NULL;
	uint8_t mac_tmp[ETH_ALEN] = {0};
	struct pairing_info *pairing_info = NULL;

	if (len != sizeof(struct dns_evt_data_end_info) || !info) {
		RTW_INFO("info or len(%d) error!\n", len);
		return;
	}

	tmp_nan_data_mgnt = get_nan_data_mgnt_by_ndp_id(info->ndp_id);
	if (tmp_nan_data_mgnt == NULL) {
		return;
	}

	RTW_INFO("%s, ndp_id[%d]\n", __func__, info->ndp_id);

	exec_stop_script(info, len);
	pairing_info = &tmp_nan_data_mgnt->service.pairing_info;
	if (!(pairing_info->peer_nik_caching && pairing_info->nik_caching)) {
		clear_pairing_info(tmp_nan_data_mgnt);
	}
	clear_data_path(tmp_nan_data_mgnt);
}

void dns_evt_bstrap_result(struct dns_evt_bstrap_result *info, uint32_t len)
{
	struct nan_data_mgnt *tmp_nan_data_mgnt = NULL;
	struct pairing_info *pairing_info = NULL;

	if (len != sizeof(struct dns_evt_bstrap_result) || !info) {
		RTW_INFO("info or len(%d) error!\n", len);
		return;
	}

	if (info->local_role == PAIRING_ROLE_INITIATOR) {
		tmp_nan_data_mgnt = get_nan_data_mgnt_by_sub_id(info->local_inst_id);
	} else {
		tmp_nan_data_mgnt = get_nan_data_mgnt_by_pub_id(info->local_inst_id);
		if (tmp_nan_data_mgnt) {
			tmp_nan_data_mgnt->service.subscribe_id = info->remote_inst_id;
		}
	}
	if (tmp_nan_data_mgnt == NULL) {
		return;
	}

	// bstrap status define (now 0 is success)
	if (!info->bstrap_method && info->status != 0) {
		RTW_INFO("%s, fail! status[%d]\n", __func__, info->status);
		return;
	}

	if (timerid) {
		timer_delete(timerid);
	}

	RTW_INFO("%s, bstrap_method[0x%x]\n", __func__, info->bstrap_method);

	tmp_nan_data_mgnt->nan_status = NAN_STATUS_GET_BSTRAP_RESULT;

	pairing_info = &tmp_nan_data_mgnt->service.pairing_info;
	memcpy(pairing_info->peer_nmi, info->peer_nmi, ETH_ALEN);
	pairing_info->pasn_type = NAN_PAIRING_TYPE_SETUP;

	if (info->bstrap_method & OPPORTUNISTIC_BSTRAP) {
		pairing_info->pasn_auth_method = NAN_PAIRING_AUTH_MODE_OPPORTUNISTIC;
	} else {
		pairing_info->pasn_auth_method = NAN_PAIRING_AUTH_MODE_PASSWORD;
	}

	if (info->local_role == PAIRING_ROLE_INITIATOR) {
		if (dns_pairing_start(&tmp_nan_data_mgnt->service.pairing_info) == RTW_RET_STATUS_SUCCESS) {
			tmp_nan_data_mgnt->nan_status = NAN_STATUS_PASN_START;
			tmp_nan_data_mgnt->data_path_retry = 0;
			creat_timer();
		}
	} else {
		pairing_info->role = PAIRING_ROLE_RESPONDER;
		tmp_nan_data_mgnt->nan_status = NAN_STATUS_PASN_START;
		tmp_nan_data_mgnt->data_path_retry = 0;
		creat_timer();
	}
}

void dns_evt_pasn_result(struct dns_evt_pasn_result *info, uint32_t len)
{
	struct nan_data_mgnt *tmp_nan_data_mgnt = NULL;
	struct pairing_info *pairing_info = NULL;

	if (len != sizeof(struct dns_evt_pasn_result) || !info) {
		RTW_INFO("info or len(%d) error!\n", len);
		return;
	}

	RTW_INFO("%s, status[0x%x], local_role[0x%x], pairing_type[0x%x]"\
			 ",remote_nik_caching[0x%x]\n", __func__, info->status, info->local_role,
			 info->pairing_type, info->remote_nik_caching);

	tmp_nan_data_mgnt = get_nan_data_mgnt_by_pairing();
	if (!tmp_nan_data_mgnt) {
		return;
	}

	if (info->status != NAN_PASN_DONE) {
		RTW_INFO("%s, PASN failed! status[%d]\n", __func__, info->status);
		return;
	}

	if (tmp_nan_data_mgnt->nan_status == NAN_STATUS_SEND_REQ) {
		return;
	}

	RTW_INFO("%s, PASN success! \n", __func__);

	pairing_info = &tmp_nan_data_mgnt->service.pairing_info;
	pairing_info->role = info->local_role;
	pairing_info->pasn_type = info->pairing_type;
	pairing_info->peer_nik_caching = info->remote_nik_caching;
	memcpy(pairing_info->peer_nmi, info->peer_nmi, ETH_ALEN);

	if (timerid) {
		timer_delete(timerid);
	}

	if (pairing_info->role == PAIRING_ROLE_INITIATOR) {
		tmp_nan_data_mgnt->nan_status = NAN_STATUS_GET_PASN_RESULT;

		/* send nan data request frame*/
		if (dns_datapath_req(&tmp_nan_data_mgnt->data_path) == RTW_RET_STATUS_SUCCESS) {
			RTW_INFO("send data path success!!!\n");
			tmp_nan_data_mgnt->nan_status = NAN_STATUS_SEND_REQ;
			tmp_nan_data_mgnt->data_path_retry = 0;
			creat_timer();
		}
	} else {
		tmp_nan_data_mgnt->nan_status = NAN_STATUS_GET_PASN_RESULT;
		RTW_INFO("Waitting for data path request!!!\n");
		return;
	}
}

void dns_evt_hdr(uint8_t event_id, void *event_data, uint32_t len)
{
	if (sig_flag) {
		return;
	}

	switch (event_id) {
	case DNS_DISCOVERY_RESULT:
		RTW_INFO("DNS_DISCOVERY_RESULT\n");
		dns_evt_discovery_result((struct dns_evt_disc_result_info *)event_data, len);
		break;
	case DNS_DATA_INDICATION:
		RTW_INFO("DNS_DATA_INDICATION\n");
		dns_evt_data_indication((struct dns_evt_data_indic_info *)event_data, len);
		break;
	case DNS_DATA_CONFIRM:
		RTW_INFO("DNS_DATA_CONFIRM\n");
		dns_evt_data_confirm((struct dns_evt_data_comfirm_info *)event_data, len);
		break;
	case DNS_DATA_ESTABLISHED:
		RTW_INFO("DNS_DATA_ESTABLISHED\n");
		exec_start_script((struct dns_evt_data_established_info *)event_data, len);
		break;
	case DNS_DATA_END:
		RTW_INFO("DNS_DATA_END\n");
		dns_evt_data_end((struct dns_evt_data_end_info *)event_data, len);
		break;
	case DNS_BSTRAP_RESULT:
		RTW_INFO("DNS_BSTRAP_RESULT\n");
		dns_evt_bstrap_result((struct dns_evt_bstrap_result *)event_data, len);
		break;
	case DNS_PASN_RESULT:
		RTW_INFO("DNS_PASN_RESULT\n");
		dns_evt_pasn_result((struct dns_evt_pasn_result *)event_data, len);
		break;
	default:
		RTW_INFO("[rtw_aware]unknow dns event id (%d)\n", event_id);
		break;
	}
}

int get_phy_from_mac(char *mac_addr, uint16_t *phy_num)
{
	FILE *fp;
	char temp[50], addr[50], phy[50], cmd[512];
	int  idx = 0, ret = false;
	*phy_num = 0;

	sprintf(addr, "%s", mac_addr);

	strcpy(cmd, rtw_iw);
	strcat(cmd, " dev");

	fp = popen(cmd, "r");
	if (fp == NULL) {
		RTW_INFO("[rtw_cmd]Failed to run command\n");
		return ret;
	}

	while (fscanf(fp, "%49s", temp) != EOF) {
		if (strstr(temp, "phy#") != 0) {
			strcpy(phy, temp);
		}

		if ((strstr(temp, mac_addr) != 0)) {
			/* try to find phy number in string phy# */
			idx = 4;
			while (phy[idx] != '\0' && phy[idx] <= '9' && phy[idx] >= '0') {
				*phy_num = (*phy_num) * 10 + phy[idx] - '0';
				idx++;
			}
			ret = true;
			RTW_INFO("%s find phy%d! \n", __func__, *phy_num);
			break;
		}
	}

	pclose(fp);

	return ret;
}

uint16_t cvert_text_info_str(uint8_t *txt_str, uint8_t *out_buf, uint16_t out_buf_len)
{
	char *pch = NULL;
	uint16_t txt_len = 0;

	if (NULL == txt_str || NULL == out_buf || 0 == out_buf_len) {
		RTW_INFO("cvert text info err! input error\n");
		return 0;
	}

	pch = strtok(txt_str, " |");
	while (pch) {
		/* txt length */
		RTW_DBG("length:%s\n", pch);
		if ((txt_len + 1) > out_buf_len) {
			RTW_INFO("cvert text info err! str is too long, len(%d) > out_buf_len(%d)\n",
					 txt_len + 1, out_buf_len);
			break;
		}
		if (sscanf(pch, "0x%hhx", (unsigned char *)(out_buf + txt_len)) != 1) {
			out_buf[txt_len] = atoi(pch);
		}
		txt_len++;
		pch = strtok(NULL, " |");
		if (!pch) {
			break;
		}

		/* txt data */
		RTW_DBG("data:%s\n", pch);
		if ((txt_len + strlen(pch)) > out_buf_len) {
			RTW_INFO("cvert text info err! str is too long, len(%ld) > out_buf_len(%d)\n",
					 (txt_len + strlen(pch)), out_buf_len);
			break;
		}
		strcpy(out_buf + txt_len, pch);
		txt_len += strlen(pch);
		pch = strtok(NULL, " |");
	}

	return txt_len;
}

enum rtw_security_type get_security_type_from_input_str(char *str)
{
	enum rtw_security_type type = RTW_SECURITY_UNKNOWN;
	if (strcmp(str, "open") == 0) {
		type = RTW_SECURITY_OPEN;
	} else if (strcmp(str, "pmk") == 0) {
		type = RTW_SECURITY_PMK;
	} else if (strcmp(str, "pairing") == 0) {
		type = RTW_SECURITY_PAIRING;
	}
	return type;
}

enum rtw_aware_role get_role_from_input_str(char *str)
{
	enum rtw_aware_role role = RTW_AWARE_ROLE_UNKNOWN;
	if (strcmp(str, "publish") == 0) {
		role = RTW_AWARE_ROLE_PUBLISH;
	} else if (strcmp(str, "subscribe") == 0) {
		role = RTW_AWARE_ROLE_SUBSCRIBE;
	}
	return role;
}

uint16_t get_bstrap_method_from_input_str(char *str, enum rtw_aware_role role)
{
	uint16_t bstrap_method = 0;

	/* opp or pin_code passphrase qr_code */
	if (role == RTW_AWARE_ROLE_SUBSCRIBE) {
		if (strstr(str, "opp")) {
			bstrap_method |= OPPORTUNISTIC_BSTRAP;
		}
		if (strstr(str, "pin_code")) {
			bstrap_method |= PIN_CODE_KEYPAD;
		}
		if (strstr(str, "passphrase")) {
			bstrap_method |= PASSPHRASE_KEYPAD;
		}
		if (strstr(str, "qr_code")) {
			bstrap_method |= QR_CODE_SCAN;
		}

	} else if (role == RTW_AWARE_ROLE_PUBLISH) {
		if (strstr(str, "opp")) {
			bstrap_method |= OPPORTUNISTIC_BSTRAP;
		}
		if (strstr(str, "pin_code")) {
			bstrap_method |= PIN_CODE_DISPLAY;
		}
		if (strstr(str, "passphrase")) {
			bstrap_method |= PASSPHRASE_DISPLAY;
		}
		if (strstr(str, "qr_code")) {
			bstrap_method |= QR_CODE_DISPLAY;
		}
	} else {
		RTW_INFO("Unknow role: %d \n", role);
	}
	return bstrap_method;
}

int get_publish_dns_sd_from_json(json_object *obj, struct dns_sd_config *dns_record)
{
	json_object *tmp_obj = NULL;
	char security_type[16] = {0};
	char bstrap_method_str[128] = {0};
	enum rtw_security_type security_option = RTW_SECURITY_UNKNOWN;
	uint8_t txt_str[NAN_MAX_SERVICE_INFO_LEN] = {0};
	uint8_t txt_buf[NAN_MAX_SERVICE_INFO_LEN] = {0};
	uint16_t i = 0;

	memset(dns_record->pmk, 0, NAN_PMK_SIZE);

	/* id (publish_id) */
	if (!json_object_object_get_ex(obj, "publish_id", &tmp_obj)) {
		RTW_INFO(" \"publish_id\" not found\n");
		return false;
	}

	dns_record->id = json_object_get_int(tmp_obj);
	RTW_INFO("publish_id: %d \n", dns_record->id);

	/* service_name */
	if (!json_object_object_get_ex(obj, "service_name", &tmp_obj)) {
		RTW_INFO(" \"service_name\" not found\n");
		return false;
	}
	memset(dns_record->service_name, '\0', MAX_SRVC_NAME_LEN);
	CPY_OBJ_STR(dns_record->service_name, tmp_obj);
	RTW_INFO("service_name: %s \n", dns_record->service_name);

	/* name_of_service_instance */
	if (!json_object_object_get_ex(obj, "name_of_service_instance", &tmp_obj)) {
		RTW_INFO(" \"name_of_service_instance\" not found\n");
		return false;
	}
	memset(dns_record->service_instance, '\0', MAX_SRVC_INST_LEN);
	CPY_OBJ_STR(dns_record->service_instance, tmp_obj);
	RTW_INFO("name_of_service_instance: %s \n",
			 dns_record->service_instance);

	/* textinfo */
	if (!json_object_object_get_ex(obj, "textinfo", &tmp_obj)) {
		RTW_INFO(" \"textinfo\" not found\n");
		return false;
	}
	if (strlen(json_object_get_string(tmp_obj)) &&
		strlen(json_object_get_string(tmp_obj)) < NAN_MAX_SERVICE_INFO_LEN) {
		CPY_OBJ_STR(txt_str, tmp_obj);
		RTW_INFO("textinfo str: %s\n", txt_str);

		dns_record->text_info_len = cvert_text_info_str(txt_str,
									txt_buf,
									NAN_MAX_SERVICE_INFO_LEN);
		if (dns_record->text_info_len) {
			dns_record->text_info = (char *)malloc(dns_record->text_info_len);
			if (!dns_record->text_info) {
				dns_record->text_info_len = 0;
				RTW_INFO("memory alloc fail!\n");
				return false;
			}
			memcpy(dns_record->text_info, txt_buf, dns_record->text_info_len);
			RTW_INFO("textinfo (len=%d): ", dns_record->text_info_len);
			for (i = 0; i < dns_record->text_info_len; i++) {
				printf("%02x ", dns_record->text_info[i]);
			}
			RTW_INFO("\n");
		}
	}

	/* hostname */
	if (!json_object_object_get_ex(obj, "hostname", &tmp_obj)) {
		RTW_INFO(" \"hostname\" not found\n");
		return false;
	}
	memset(dns_record->hostname, 0, NAN_MAX_HOSTNAME_LEN);
	CPY_OBJ_STR(dns_record->hostname, tmp_obj);
	RTW_INFO("hostname: %s \n", dns_record->hostname);

	/* port_number */
	if (!json_object_object_get_ex(obj, "port_number", &tmp_obj)) {
		RTW_INFO(" \"port_number\" not found\n");
		return false;
	}
	dns_record->port_number = json_object_get_int(tmp_obj);
	RTW_INFO("port_number: %u \n", dns_record->port_number);

	/* security */
	if (!json_object_object_get_ex(obj, "security", &tmp_obj)) {
		RTW_INFO(" \"security\" not found\n");
		return false;
	}
	memset(security_type, 0, 16);
	CPY_OBJ_STR(security_type, tmp_obj);
	dns_record->security_option = get_security_type_from_input_str(security_type);
	RTW_INFO("security type: %s \n", security_type);

	switch (dns_record->security_option) {
	case RTW_SECURITY_OPEN:
		break;

	case RTW_SECURITY_PMK:
		/* pmk key*/
		if (!json_object_object_get_ex(obj, "pmk_key", &tmp_obj)) {
			RTW_INFO(" \"pmk_key\" not found\n");
			return false;
		}
		if (strlen(json_object_get_string(tmp_obj)) != NAN_PMK_SIZE) {
			RTW_INFO("pmk length is invalid \n");
			return false;
		}
		CPY_OBJ_STR(dns_record->pmk, tmp_obj);
		RTW_INFO("pmk: %s \n", dns_record->pmk);
		break;

	case RTW_SECURITY_PAIRING:
		/* bstrap_method */
		if (!json_object_object_get_ex(obj, "bstrap_method", &tmp_obj)) {
			RTW_INFO(" \"bstrap_method\" not found\n");
			return false;
		}
		memset(bstrap_method_str, 0, 128);
		CPY_OBJ_STR(bstrap_method_str, tmp_obj);
		dns_record->bstrap_method = get_bstrap_method_from_input_str(bstrap_method_str,
									RTW_AWARE_ROLE_PUBLISH);
		RTW_INFO("bstrap_method_str: %s \n", bstrap_method_str);
		RTW_INFO("bstrap_method: 0x%x \n", dns_record->bstrap_method);

		/* pairing_passphrase */
		memset(dns_record->pairing_pw, '\0', MAX_PASN_PW_LEN + 1);
		if (json_object_object_get_ex(obj, "pairing_passphrase", &tmp_obj)) {
			if (strlen(json_object_get_string(tmp_obj)) > MAX_PASN_PW_LEN) {
				RTW_INFO("pairing_passphrase length is bigger than 16! \n");
				return false;
			}
			CPY_OBJ_STR(dns_record->pairing_pw, tmp_obj);
			RTW_INFO("pairing_passphrase: %s\n", dns_record->pairing_pw);
		}

		/* nik_caching  */
		if (!json_object_object_get_ex(obj, "nik_caching", &tmp_obj)) {
			RTW_INFO(" \"nik_caching\" not found\n");
			return false;
		}
		dns_record->nik_caching = json_object_get_int(tmp_obj);
		break;

	case RTW_SECURITY_UNKNOWN:
	default:
		RTW_INFO("Unknow security type!\n");
		return false;

	}

	/* start script */
	if (json_object_object_get_ex(obj, "start_cmd", &tmp_obj)) {
		memset(dns_record->start_cmd, 0, MAX_CMD_LEN);
		CPY_OBJ_STR(dns_record->start_cmd, tmp_obj);
		RTW_INFO("start_cmd: %s \n", dns_record->start_cmd);
	}

	/* stop script */
	if (json_object_object_get_ex(obj, "end_cmd", &tmp_obj)) {
		memset(dns_record->end_cmd, 0, MAX_CMD_LEN);
		CPY_OBJ_STR(dns_record->end_cmd, tmp_obj);
		RTW_INFO("end_cmd: %s \n", dns_record->end_cmd);
	}


	/* gtk_enable  */
	if (json_object_object_get_ex(obj, "gtk_enable", &tmp_obj)) {
		dns_record->gtk_enable = json_object_get_int(tmp_obj);
		RTW_INFO("gtk_enable: %d \n", dns_record->gtk_enable);
	}

	return true;
}

int get_subscribe_dns_sd_from_json(json_object *obj, struct dns_sd_config *dns_record)
{
	json_object *tmp_obj = NULL;
	char security_type[16] = {0};
	char bstrap_method_str[128] = {0};
	enum rtw_security_type security_option = RTW_SECURITY_UNKNOWN;

	memset(dns_record->pmk, 0, NAN_PMK_SIZE);

	/* id (subscribe_id) */
	json_object_object_get_ex(obj, "subscribe_id", &tmp_obj);
	dns_record->id = json_object_get_int(tmp_obj);
	RTW_INFO("subscribe_id: %d \n", dns_record->id);

	/* service_name */
	json_object_object_get_ex(obj, "service_name", &tmp_obj);
	memset(dns_record->service_name, '\0', MAX_SRVC_NAME_LEN);
	CPY_OBJ_STR(dns_record->service_name, tmp_obj);
	RTW_INFO("service_name: %s \n", dns_record->service_name);

	/* security */
	if (!json_object_object_get_ex(obj, "security", &tmp_obj)) {
		RTW_INFO(" \"security\" not found\n");
		return false;
	}
	memset(security_type, 0, 16);
	CPY_OBJ_STR(security_type, tmp_obj);
	dns_record->security_option = get_security_type_from_input_str(security_type);
	RTW_INFO("security type: %s \n", security_type);

	switch (dns_record->security_option) {
	case RTW_SECURITY_OPEN:
		break;

	case RTW_SECURITY_PMK:
		/* pmk key*/
		if (!json_object_object_get_ex(obj, "pmk_key", &tmp_obj)) {
			RTW_INFO(" \"pmk_key\" not found\n");
			return false;
		}
		if (strlen(json_object_get_string(tmp_obj)) != NAN_PMK_SIZE) {
			RTW_INFO("pmk length is invalid \n");
			return false;
		}
		CPY_OBJ_STR(dns_record->pmk, tmp_obj);
		RTW_INFO("pmk: %s \n", dns_record->pmk);
		break;

	case RTW_SECURITY_PAIRING:
		/* bstrap_method */
		if (!json_object_object_get_ex(obj, "bstrap_method", &tmp_obj)) {
			RTW_INFO(" \"bstrap_method\" not found\n");
			return false;
		}
		memset(bstrap_method_str, 0, 128);
		CPY_OBJ_STR(bstrap_method_str, tmp_obj);
		dns_record->bstrap_method = get_bstrap_method_from_input_str(bstrap_method_str,
									RTW_AWARE_ROLE_SUBSCRIBE);
		RTW_INFO("bstrap_method_str: %s \n", bstrap_method_str);
		RTW_INFO("bstrap_method: 0x%x \n", dns_record->bstrap_method);

		/* pairing_passphrase */
		memset(dns_record->pairing_pw, '\0', MAX_PASN_PW_LEN + 1);
		if (json_object_object_get_ex(obj, "pairing_passphrase", &tmp_obj)) {
			if (strlen(json_object_get_string(tmp_obj)) > MAX_PASN_PW_LEN) {
				RTW_INFO("pairing_passphrase length is bigger than 16! \n");
				return false;
			}
			CPY_OBJ_STR(dns_record->pairing_pw, tmp_obj);
			RTW_INFO("pairing_passphrase: %s\n", dns_record->pairing_pw);
		}

		/* nik_caching  */
		if (!json_object_object_get_ex(obj, "nik_caching", &tmp_obj)) {
			RTW_INFO(" \"nik_caching\" not found\n");
			return false;
		}
		dns_record->nik_caching = json_object_get_int(tmp_obj);
		break;

	case RTW_SECURITY_UNKNOWN:
	default:
		RTW_INFO("Unknow security type!\n");
		return false;

	}

	return true;
}

void get_bin_path_from_json(json_object *obj, struct utility_bin_path *bin_path)
{
	json_object *tmp_obj = NULL;
	json_object *sub_obj = NULL;

	json_object_object_get_ex(obj, "iw", &tmp_obj);
	json_object_object_get_ex(tmp_obj, "iw_path", &sub_obj);
	memset(bin_path->iw_path, 0, MAX_BIN_PATH_LEN);
	CPY_OBJ_STR(bin_path->iw_path, sub_obj);
	rtw_iw = bin_path->iw_path;

	json_object_object_get_ex(obj, "nan_r3", &tmp_obj);
	json_object_object_get_ex(tmp_obj, "nan_r3_path", &sub_obj);
	memset(bin_path->nan_r3_path, 0, MAX_BIN_PATH_LEN);
	CPY_OBJ_STR(bin_path->nan_r3_path, sub_obj);
	rtw_nan_r3 = bin_path->nan_r3_path;

	json_object_object_get_ex(obj, "wpa_supplicant", &tmp_obj);
	json_object_object_get_ex(tmp_obj, "wpa_supplicant_path", &sub_obj);
	memset(bin_path->wpa_supplicant_path, 0, MAX_BIN_PATH_LEN);
	CPY_OBJ_STR(bin_path->wpa_supplicant_path, sub_obj);

	json_object_object_get_ex(tmp_obj, "wpa_supplicant_conf_path", &sub_obj);
	memset(bin_path->wpa_supplicant_conf_path, 0, MAX_BIN_PATH_LEN);
	CPY_OBJ_STR(bin_path->wpa_supplicant_conf_path, sub_obj);

	json_object_object_get_ex(tmp_obj, "wpa_supplicant_log_file", &sub_obj);
	CPY_OBJ_STR(bin_path->wpa_supplicant_log_file, sub_obj);

	json_object_object_get_ex(tmp_obj, "wpa_supplicant_dbg_para", &sub_obj);
	CPY_OBJ_STR(bin_path->wpa_supplicant_dbg_para, sub_obj);

	RTW_INFO("iw_path: %s \n", bin_path->iw_path);
	RTW_INFO("nan_r3_path: %s \n", bin_path->nan_r3_path);
	RTW_INFO("wpa_supplicant_path: %s \n", bin_path->wpa_supplicant_path);
	RTW_INFO("wpa_supplicant_conf_path: %s \n", bin_path->wpa_supplicant_conf_path);
	RTW_INFO("wpa_supplicant_log_file: %s \n", bin_path->wpa_supplicant_log_file);
	RTW_INFO("wpa_supplicant_dbg_para: %s \n", bin_path->wpa_supplicant_dbg_para);
}

int get_basic_from_json(json_object *obj, struct basic_config *basic_info)
{
	json_object *tmp_obj = NULL, *mac_obj = NULL;

	json_object_object_get_ex(obj, "mac_addr", &mac_obj);
	if (strlen(json_object_get_string(mac_obj)) != MAC_STR_LEN) {
		RTW_INFO("wrong mac address %s \n",
				 json_object_get_string(mac_obj));
		return false;
	}

	CPY_OBJ_STR(basic_info->device_mac, mac_obj);
	RTW_INFO("mac_addr : %s \n", basic_info->device_mac);

	return true;
}


int read_from_json_file(char *filename,
						struct utility_bin_path *bin_path,
						struct basic_config *basic_info,
						enum rtw_aware_role aware_role)
{
	json_object *test_obj = NULL, *bin_path_obj = NULL, *dns_sd_obj = NULL,
				 *publish_obj = NULL, *subscribe_obj = NULL, *basic_obj = NULL;

	enum json_type type;
	int ret = true;

	RTW_INFO("filename: %s \n", filename);
	test_obj = json_object_from_file(filename);
	if (!test_obj) {
		RTW_INFO("Cannot open %s\n", filename);
		ret = false;
		goto exit;
	}
	/* Bin path information */
	if (json_object_object_get_ex(test_obj, "bin_path", &bin_path_obj)) {
		get_bin_path_from_json(bin_path_obj, bin_path);
	} else {
		RTW_INFO("Cannot find bin path \n");
		ret = false;
		goto exit;
	}

	/* Basic information */
	if (json_object_object_get_ex(test_obj, "basic", &basic_obj)) {
		get_basic_from_json(basic_obj, basic_info);
	} else {
		RTW_INFO("Cannot find basic \n");
		ret = false;
		goto exit;
	}

exit:
	return ret;
}

int read_service_conf_from_json_file(char *filename, enum rtw_aware_role aware_role)
{
	json_object *test_obj = NULL, *dns_sd_obj = NULL,
				 *publish_obj = NULL, *subscribe_obj = NULL;

	enum json_type type;
	int ret = true;
	int i;
	char tmp[9];

	RTW_INFO("filename: %s \n", filename);
	test_obj = json_object_from_file(filename);
	if (!test_obj) {
		RTW_INFO("Cannot open %s\n", filename);
		ret = false;
		goto exit;
	}

	/* DNS SD record information */
	json_object_object_get_ex(test_obj, "dns_sd_record", &dns_sd_obj);

	if (aware_role == RTW_AWARE_ROLE_PUBLISH) {
		RTW_INFO("role: Publish \n");
		for (i = 0; i < g_num_service && ret == true; i++) {
			memset(tmp, 0, sizeof(tmp));
			sprintf(tmp, "publish%d", i + 1);
			json_object_object_get_ex(dns_sd_obj, tmp, &publish_obj);
			ret = get_publish_dns_sd_from_json(publish_obj, &g_nan_service_mgnt[i].dns_record);
		}
	} else if (aware_role == RTW_AWARE_ROLE_SUBSCRIBE) {
		RTW_INFO("role: Subscribe \n");
		json_object_object_get_ex(dns_sd_obj, "subscribe", &subscribe_obj);
		ret = get_subscribe_dns_sd_from_json(subscribe_obj, &g_nan_service_mgnt[0].dns_record);
	}

exit:
	return ret;
}

void setup_nan_mgnt(enum rtw_aware_role role, struct dns_sd_config *dns_record, struct nan_data_mgnt *nan_data_mgnt)
{
	struct pairing_info *pairing_info = NULL;

	/* setup nan data: service, data_path */
	memset(nan_data_mgnt, 0, sizeof(struct nan_data_mgnt));

	nan_data_mgnt->role = role;

	/* setup service info */
	if (role == RTW_AWARE_ROLE_PUBLISH) {
		nan_data_mgnt->service.publish_id = dns_record->id;
	} else if (role == RTW_AWARE_ROLE_SUBSCRIBE) {
		nan_data_mgnt->service.subscribe_id = dns_record->id;
	}

	memcpy(nan_data_mgnt->service.service_name,
		   dns_record->service_name,
		   MAX_SRVC_NAME_LEN);
	memcpy(nan_data_mgnt->service.service_instance,
		   dns_record->service_instance,
		   MAX_SRVC_INST_LEN);
	if (dns_record->text_info_len) {
		nan_data_mgnt->service.text_info_len = dns_record->text_info_len;
		/* note: free at the end of main function */
		nan_data_mgnt->service.text_info = calloc(dns_record->text_info_len, sizeof(uint8_t));
		memcpy(nan_data_mgnt->service.text_info,
			   dns_record->text_info,
			   dns_record->text_info_len);
	}

	/* setup data path info */
	nan_data_mgnt->data_path.service = &nan_data_mgnt->service;
	memcpy(nan_data_mgnt->data_path.hostname,
		   dns_record->hostname,
		   sizeof(dns_record->hostname));
	nan_data_mgnt->data_path.port_number = dns_record->port_number;

	/* Security */
	switch (dns_record->security_option) {
	case RTW_SECURITY_OPEN:
		nan_data_mgnt->service.sec_type = 0;
		break;
	case RTW_SECURITY_PMK:
		nan_data_mgnt->service.sec_type = NAN_PMK_SET_BY_USER_PMK;
		nan_data_mgnt->service.gtk_enable = dns_record->gtk_enable;
		memcpy(nan_data_mgnt->service.pmk, dns_record->pmk, NAN_PMK_SIZE);
		memcpy(nan_data_mgnt->data_path.pmk, dns_record->pmk, NAN_PMK_SIZE);
		break;
	case RTW_SECURITY_PAIRING:
		nan_data_mgnt->service.sec_type = NAN_PMK_SET_BY_PAIRING;
		nan_data_mgnt->service.gtk_enable = dns_record->gtk_enable;
		pairing_info = &nan_data_mgnt->service.pairing_info;

		memset(pairing_info->pasn_pw, '\0', MAX_PASN_PW_LEN + 1);
		pairing_info->pasn_pw_len = strlen(dns_record->pairing_pw);
		memcpy(pairing_info->pasn_pw, dns_record->pairing_pw, pairing_info->pasn_pw_len);
		RTW_DBG("pw: %s\n", pairing_info->pasn_pw);
		RTW_DBG("pw_len: %d\n", pairing_info->pasn_pw_len);

		pairing_info->nik_caching = dns_record->nik_caching;
		pairing_info->bstrap_method = dns_record->bstrap_method;
	default:
		break;
	}
}

void rtw_aware_help(char *str)
{
	fprintf(stderr, "Usage: %s [-r publish|subscribe] [-f config_file] [option] \n", str);
	fprintf(stderr, "\n");
	fprintf(stderr, "General : \n");
	fprintf(stderr, "  -b \t enable bluetooth trigger, default is disabled.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Publish : \n");
	fprintf(stderr, "  -s \t open,pmk (Deprecated, the security type setting has been moved to the config file)\n");
	fprintf(stderr, "  -t \t timeout value\n");
	fprintf(stderr, "  -n \t number of service\n");
	fprintf(stderr, "subscribe : \n");
	fprintf(stderr, "  -s \t open,pmk (Deprecated, the security type setting has been moved to the config file)\n");
	fprintf(stderr, "  -t \t timeout value\n");
}

int main(int argc, char *argv[])
{
	uint16_t phy_number = 0;
	enum rtw_aware_role aware_role = RTW_AWARE_ROLE_UNKNOWN;
	struct utility_bin_path bin_path = {0};
	struct basic_config basic_info = {0};
	int i;

	int opt;
	char *config_file = NULL;
	uint8_t is_bt_trigger = 0;

	if (argc <= 4) {
		rtw_aware_help(argv[0]);
		exit(EXIT_FAILURE);
	}

	while ((opt = getopt(argc, argv, "r:f:s:h:t:n:b")) != -1) {
		switch (opt) {
		case 'r':
			aware_role = get_role_from_input_str(optarg);
			break;
		case 'f':
			config_file = optarg;
			break;
		case 'h':
			rtw_aware_help(argv[0]);
			exit(EXIT_SUCCESS);
			break;
		case 't':
			max_service_time_sec = atoi(optarg);
			break;
		case 'b':
			is_bt_trigger = 1;
			break;
		case 'n':
			g_num_service = atoi(optarg);
			break;
		default:
			rtw_aware_help(argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	if (aware_role == RTW_AWARE_ROLE_UNKNOWN) {
		RTW_INFO("unknown role!!! \n");
		exit(EXIT_FAILURE);
	}

	if (g_num_service > MAX_SERVICE_NUM) {
		RTW_INFO("Input parameter is invalid. The maximum number of services available is %d. \n", MAX_SERVICE_NUM);
		exit(EXIT_FAILURE);
	}

	if (aware_role != RTW_AWARE_ROLE_PUBLISH && g_num_service > 1) {
		RTW_INFO("Multi-service support is only available for the publisher role. \n");
		exit(EXIT_FAILURE);
	}

	if (!read_from_json_file(config_file, &bin_path, &basic_info, aware_role)) {
		RTW_INFO("Configuration file parser failed!!!\n");
		exit(EXIT_FAILURE);
	}

	if (!read_service_conf_from_json_file(config_file, aware_role)) {
		RTW_INFO("Configuration file parser failed!!!\n");
		exit(EXIT_FAILURE);
	}

	if (get_phy_from_mac(basic_info.device_mac, &phy_number) == false) {
		RTW_INFO("failed to get phy from %s!!!\n", basic_info.device_mac);
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < g_num_service; i++) {
		setup_nan_mgnt(aware_role, &g_nan_service_mgnt[i].dns_record, &g_nan_service_mgnt[i].nan_mgnt);
	}
	dns_register_evt_cb(dns_evt_hdr);

	/* start service */
	if (aware_role == RTW_AWARE_ROLE_PUBLISH) {
		for (i = 0; i < g_num_service; i++) {
			dns_publish(phy_number, &bin_path, &g_nan_service_mgnt[i].nan_mgnt.service, is_bt_trigger);
		}
	} else {
		dns_subscribe(phy_number, &bin_path, &g_nan_service_mgnt[0].nan_mgnt.service, is_bt_trigger);
	}

	if (aware_role == RTW_AWARE_ROLE_SUBSCRIBE) {
		max_service_time_sec = (max_service_time_sec > 5) ? (max_service_time_sec - 5) : (max_service_time_sec);
	}

	signal(SIGINT, signal_func);
	while (1) {
		wait_time(max_service_time_sec);
		if (sig_flag) {
			RTW_INFO("\n!!!!!! Terminated !!!!!!\n");
			break;
		}
	}

	for (i = 0; i < g_num_service; i++) {
		if (g_nan_service_mgnt[i].nan_mgnt.nan_status == NAN_STATUS_NDP_ESTABLISH) {
			exec_stop_script_ex(g_nan_service_mgnt[i].nan_mgnt.data_path.ndp_id);
			RTW_INFO("send data path end[%d], \n",
					 g_nan_service_mgnt[i].nan_mgnt.data_path.ndp_id);
			dns_datapath_end(g_nan_service_mgnt[i].nan_mgnt.data_path.ndp_id);
			clear_pairing_info(&(g_nan_service_mgnt[i].nan_mgnt));
			clear_data_path(&(g_nan_service_mgnt[i].nan_mgnt));
		}
	}

	sleep(1);
	dns_stop_service(phy_number);

exit:

	/* release */
	for (i = 0; i < g_num_service; i++) {
		if (g_nan_service_mgnt[i].nan_mgnt.service.text_info_len && g_nan_service_mgnt[i].nan_mgnt.service.text_info) {
			free(g_nan_service_mgnt[i].nan_mgnt.service.text_info);
		}

		if (g_nan_service_mgnt[i].dns_record.text_info_len && g_nan_service_mgnt[i].dns_record.text_info) {
			free(g_nan_service_mgnt[i].dns_record.text_info);
		}
	}

	return EXIT_SUCCESS;
}
