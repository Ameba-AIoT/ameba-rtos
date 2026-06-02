#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "rtw_nan_scenario_api.h"
#include "rtw_nan_cmd_api.h"

/* global variables defininition */
#define MAX_SERVICE_NUM 10
rtw_dns_evt_cb g_dns_evt_cb;
char *rtw_iw;
char *rtw_nan_r3;
char *nan_intf;
char *rtw_supplicant;
char *wpa_supplicant_conf;
struct srvc_info *srvc_info_list[MAX_SERVICE_NUM] = {NULL};
bool b_nan_start;
bool b_ble_trigger_enable = false;
char *supplicant_log_file;
char *supplicant_dbg_para;
char supplicant_pid[16] = {0};

#define NAN_ATTR_ID_NDP_EXT 0x29

#define GET_NDPE_TOKEN(_sdea_attr, _out) \
	(*(_out) = *((_sdea_attr) + 3))
#define GET_NDPE_TYPE(_sdea_attr, _out) \
	(*(_out) = (*((_sdea_attr) + 4)) & 0x0F)
#define GET_NDPE_STATUS(_sdea_attr, _out) \
	(*(_out) = (*((_sdea_attr) + 4) & 0xF0) >> 4)
#define GET_NDPE_REASON(_sdea_attr, _out) \
	(*(_out) = *((_sdea_attr) + 5))
#define GET_NDPE_NDP_ID(_sdea_attr, _out) \
	(*(_out) = *((_sdea_attr) + 12))
#define GET_NDPE_NDP_CTRL(_sdea_attr, _out) \
	(*(_out) = *((_sdea_attr) + 13))

enum _nan_ndpe_type {
	NAN_NDPE_TYPE_REQUEST = 0x0,
	NAN_NDPE_TYPE_RESPONSE = 0x1,
	NAN_NDPE_TYPE_CONFIRM = 0x2,
	NAN_NDPE_TYPE_SEC_INSTALL = 0x3,
	NAN_NDPE_TYPE_TERMINATE = 0x4,
};
enum _nan_sec_key_type {
	NAN_SEC_KEY_TYPE_GTK = BIT(0),
	NAN_SEC_KEY_TYPE_IGTK = BIT(1),
	NAN_SEC_KEY_TYPE_BIGTK = BIT(2),
	NAN_SEC_KEY_TYPE_NIK = BIT(3),
	NAN_SEC_KEY_TYPE_ND_TK = BIT(4),
};

int _mac_addr_a2n(unsigned char *mac_addr, char *arg)
{
	int i;

	for (i = 0; i < ETH_ALEN ; i++) {
		unsigned int temp;
		char *cp = strchr(arg, ':');
		if (cp) {
			*cp = 0;
			cp++;
		}
		if (sscanf(arg, "%x", &temp) != 1) {
			return -1;
		}
		if (temp > 255) {
			return -1;
		}

		mac_addr[i] = temp;
		if (!cp) {
			break;
		}
		arg = cp;
	}
	if (i < ETH_ALEN - 1) {
		return -1;
	}

	return 0;
}

bool _nan_set_bin_path(struct utility_bin_path *bin_path)
{
	if (bin_path == NULL) {
		INFO_PRINT("[rtw_scenario]%s unknow bin path!\n", __func__);
		return false;
	}

	rtw_iw = bin_path->iw_path;
	INFO_PRINT("[rtw_scenario] iw_path = %s \n", rtw_iw);

	rtw_nan_r3 = bin_path->nan_r3_path;
	INFO_PRINT("[rtw_scenario] nan_r3_path = %s \n", rtw_nan_r3);

	rtw_supplicant = bin_path->wpa_supplicant_path;
	INFO_PRINT("[rtw_scenario] wpa_supplicant_path = %s \n", rtw_supplicant);

	wpa_supplicant_conf = bin_path->wpa_supplicant_conf_path;
	INFO_PRINT("[rtw_scenario] wpa_supplicant_conf = %s \n", wpa_supplicant_conf);

	supplicant_log_file = bin_path->wpa_supplicant_log_file;
	supplicant_dbg_para = bin_path->wpa_supplicant_dbg_para;

	return true;
}

/**
 * _nan_get_nmi
 * @intf: nan interface
 * @nmi: nan mac address
 * return: true or false
 */
bool _nan_get_nmi(char *intf, char *nmi)
{
	FILE *fp;
	char temp[50], cmd[512], cmd_out[4096];
	uint32_t num = 0, wdev_value_offset = 4;

	if (intf == NULL) {
		printf("[rtw_cmd]error unknow interface!\n");
		return false;
	}

	strcpy(cmd, rtw_iw);
	strcat(cmd, " dev");

	fp = popen(cmd, "r");
	if (fp == NULL) {
		INFO_PRINT("[rtw_cmd]Failed to run command\n");
		return false;
	}

	/* try to get wdev of nan from iw dev command output */
	while (fscanf(fp, "%49s", temp) != EOF) {
		if (strstr(temp, "addr") != 0) {
			fscanf(fp, "%49s", temp);
			strcpy(nmi, temp);
		}
		if ((strstr(temp, "NAN") != 0) && (nmi != NULL)) {
			/* if find type NAN that means get target wdev*/
			DEBUG_PRINT("[rtw_cmd] %s, find nmi = %s\n", __func__, nmi);
			num ++;
			break;
		}

	}

	/* close */
	pclose(fp);
	return num > 0 ? true : false;
}

void _wait_time(uint16_t waiting_sec)
{
	double diff_s = 0;
	time_t service_start_t = time(NULL);

	while (1) {
		diff_s = difftime(time(NULL), service_start_t);
		if (diff_s > waiting_sec) {
			INFO_PRINT("[rtw_scenario] max service time expired, total: %.f\n", diff_s);
			break;
		}
		sleep(1);
	}
}

struct srvc_info *_get_pairing_service()
{
	uint32_t i = 0;
	for (i = 0; i < MAX_SERVICE_NUM; i++) {
		if (srvc_info_list[i]->sec_type == NAN_PMK_SET_BY_PAIRING) {
			return srvc_info_list[i];
		}
	}
}

bool _get_supplicant_pid()
{
	FILE *fp;
	char cmd[512] = {0};
	memset(cmd, 0, 512);
	snprintf(cmd, sizeof(cmd),
			 "pgrep -a wpa_supplicant| grep %s | sort -n -k1 | awk '{print $1}'"
			 , nan_intf);

	fp = popen(cmd, "r");
	if (fp == NULL) {
		INFO_PRINT("[rtw_scenario] get wpa_supplicant pid fail\n");
		return false;
	}

	if (fgets(supplicant_pid, sizeof(supplicant_pid), fp) != NULL) {
		supplicant_pid[strcspn(supplicant_pid, "\n")] = 0;
		INFO_PRINT("[rtw_scenario] PID (stored in variable): %s\n"
				   , supplicant_pid);
	} else {
		memset(supplicant_pid, 0x0, 16);
		INFO_PRINT("[rtw_scenario] No PID found.\n");
		return false;
	}

	fclose(fp);
	return true;
}

bool _stop_wpa_supplicant()
{
	char cmd[512] = {0};

	INFO_PRINT("[rtw_scenario] %s\n", __func__);

	snprintf(cmd, sizeof(cmd), "kill %s", supplicant_pid);

	if (system(cmd) != 0) {
		return false;
	}

	sleep(1);

	if (_get_supplicant_pid() == true) {
		return false;
	}

	memset(supplicant_pid, 0x0, sizeof(supplicant_pid));
	return true;
}

bool _start_wpa_supplicant()
{
	char cmd[512] = {0};

	INFO_PRINT("[rtw_scenario] %s\n", __func__);

	/* When supplicant_pid is not all zeros, it indicates that
	 * wpa_supplicant with the NAN interface is already running
	 * after executing the rtw_aware tool.*/
	if (memcmp(supplicant_pid, cmd, 16)) {
		INFO_PRINT("[rtw_scenario] wpa_supplicant is already running\n");
		return true;
	}

	if (_get_supplicant_pid()) {
		INFO_PRINT("[rtw_scenario] kill old NAN wpa_supplicant process\n");
		if (_stop_wpa_supplicant() == false) {
			INFO_PRINT("[rtw_scenario] cannot kill old NAN wpa_supplicant process\n");
			return false;
		}
	}

	memset(cmd, 0, 512);
	snprintf(cmd, sizeof(cmd), "%s -i %s -Dnl80211 -c %s -K %s -B -f %s",
			 rtw_supplicant,
			 nan_intf,
			 wpa_supplicant_conf,
			 supplicant_dbg_para,
			 supplicant_log_file);

	if (system(cmd) != 0) {
		return false;
	}

	if (!_get_supplicant_pid()) {
		return false;
	}
	return true;
}

RTW_RET_STATUS dns_stop_service(uint16_t phy_num)
{
	RTW_RET_STATUS ret = RTW_RET_STATUS_SUCCESS;

	if (b_ble_trigger_enable) {
		rtw_nan_api_ble_start_ble_trigger(0);
		sleep(5);
		rtw_nan_api_ble_stop();
		rtw_nan_api_ble_deinit();
	}

	_stop_wpa_supplicant();

	if (b_nan_start) {
		rtw_nan_api_stop_nan(nan_intf);
	}

	ret = rtw_nan_api_deinit(phy_num);

	return ret;
}

void nan_evt_handler(uint8_t event_id, void *buf, uint32_t len)
{
	uint16_t i = 0;
	if (event_id >= NAN_EVT_MAX) {
		INFO_PRINT("[rtw_scenario] %s: event_id(%d) >= NAN_EVT_MAX(%d)",
				   __func__, event_id, NAN_EVT_MAX);
		return;
	}

	INFO_PRINT("[rtw_scenario] %s, event_id(%d)\n", __func__, event_id);

	switch (event_id) {
	case NAN_EVT_CLUSTER_CHANGE: {
		struct nan_evt_cluster_changed_info *p_evt_data = NULL;

		INFO_PRINT("[rtw_scenario] Cluster changed!\n");

		if (!buf || len < sizeof(struct nan_evt_cluster_changed_info)) {
			INFO_PRINT("[rtw_scenario] buf or len(%d) error!\n", len);
			break;
		}

		p_evt_data = (struct nan_evt_cluster_changed_info *)buf;

		PRINT_MAC_ADDR("[rtw_scenario] New ClusterID", p_evt_data->cluster_id);
	}
	break;
	case NAN_EVT_DISCOVERY_RESULT: {
		struct dns_evt_disc_result_info dns_data = {0};
		struct nan_evt_disc_result_info *p_evt_data = NULL;

		INFO_PRINT("[rtw_scenario] recv discovery result\n");

		if (!buf || len < sizeof(struct nan_evt_disc_result_info)) {
			INFO_PRINT("[rtw_scenario] buf or len(%d) error!\n", len);
			break;
		}

		p_evt_data = (struct nan_evt_disc_result_info *)buf;

		dns_data.subscribe_id = p_evt_data->subscirbe_id;
		dns_data.publisher_id = p_evt_data->publish_id;
		memcpy(dns_data.peer_nmi, p_evt_data->peer_nmi, ETH_ALEN);
		memcpy(dns_data.service_id, p_evt_data->service_id, SRVC_ID_LEN);
		memcpy(dns_data.service_name, p_evt_data->service_name, MAX_SRVC_NAME_LEN);
		memcpy(dns_data.service_instance, p_evt_data->service_inst, MAX_SRVC_INST_LEN);
		dns_data.text_info_len = p_evt_data->text_info_len;
		memset(dns_data.text_info, 0, sizeof(dns_data.text_info));
		memcpy(dns_data.text_info, p_evt_data->text_info, p_evt_data->text_info_len);
		dns_data.pmk_set = p_evt_data->pmk_set;
		dns_data.pairing_enable = p_evt_data->pairing_enable;
		dns_data.paired = p_evt_data->nira_paired;

		if (g_dns_evt_cb) {
			g_dns_evt_cb(DNS_DISCOVERY_RESULT, &dns_data, sizeof(dns_data));
		}
	}
	break;
	case NAN_EVT_DATAPATH_RECV_REQ:
		INFO_PRINT("[rtw_scenario] recv datapath req\n");
		break;
	case NAN_EVT_DATAPATH_RECV_RSP:
		INFO_PRINT("[rtw_scenario] recv datapath rsp\n");
		break;
	case NAN_EVT_DATAPATH_RECV_FOLLOW_UP:
		INFO_PRINT("[rtw_scenario] recv datapath follow up\n");
		break;
	case NAN_EVT_DATAPATH_ESTABLISHED: {
		struct dns_evt_data_established_info dns_info = {0};
		struct nan_evt_data_established_info *info = NULL;

		INFO_PRINT("[rtw_scenario] datapath established\n");

		if (!buf || len < sizeof(struct nan_evt_data_established_info)) {
			INFO_PRINT("[rtw_scenario] buf or len(%d) error!\n", len);
			break;
		}

		info = (struct nan_evt_data_established_info *)buf;
		dns_info.publish_id = info->publish_id;
		dns_info.type = info->type;
		dns_info.ndp_id = info->ndp_id;
		memcpy(dns_info.responder_data_address, info->responder_data_address, ETH_ALEN);

		if (g_dns_evt_cb) {
			g_dns_evt_cb(DNS_DATA_ESTABLISHED, &dns_info, sizeof(dns_info));
		}
	}
	break;
	case NAN_EVT_DATAPATH_END: {
		struct dns_evt_data_end_info dns_info = {0};
		struct nan_evt_data_end_info *info = NULL;

		INFO_PRINT("[rtw_scenario] datapath end\n");

		if (!buf || len < sizeof(struct nan_evt_data_end_info)) {
			INFO_PRINT("[rtw_scenario] buf or len(%d) error!\n", len);
			break;
		}

		info = (struct nan_evt_data_end_info *)buf;
		dns_info.publish_id = info->publish_id;
		dns_info.reason = info->reason;
		dns_info.ndp_id = info->ndp_id;

		if (g_dns_evt_cb) {
			g_dns_evt_cb(DNS_DATA_END, &dns_info, sizeof(dns_info));
		}
	}
	break;
	case NAN_EVT_DATA_INDICATION: {
		struct dns_evt_data_indic_info dns_data = {0};
		struct nan_evt_data_indication_info *p_evt_data = NULL;

		INFO_PRINT("[rtw_scenario] recv data indication\n");

		if (!buf || len < sizeof(struct nan_evt_data_indication_info)) {
			INFO_PRINT("[rtw_scenario] buf or len(%d) error!\n", len);
			break;
		}

		p_evt_data = (struct nan_evt_data_indication_info *)buf;

		dns_data.type = p_evt_data->type;
		dns_data.publish_id = p_evt_data->publish_id;
		dns_data.ndp_id = p_evt_data->ndp_id;
		dns_data.mc_id = p_evt_data->mc_id;
		memcpy(dns_data.initiator_ndi, p_evt_data->initiator_data_address, ETH_ALEN);
		memcpy(dns_data.responder_ndi, p_evt_data->responder_data_address, ETH_ALEN);
		dns_data.security = p_evt_data->security;
		memcpy(dns_data.ipv6_intf_identifier, p_evt_data->ipv6_interfac_id, MAX_IPV6_INFO_LEN);

		if (g_dns_evt_cb) {
			g_dns_evt_cb(DNS_DATA_INDICATION, &dns_data, sizeof(dns_data));
		}
	}
	break;
	case NAN_EVT_DATA_CONFIRM: {
		struct dns_evt_data_comfirm_info dns_data = {0};
		uint32_t dns_data_len = sizeof(struct dns_evt_data_comfirm_info);
		struct nan_evt_data_confirm_info *p_evt_data = NULL;

		INFO_PRINT("[rtw_scenario] %s: recv data confirm\n", __func__);

		if (!buf || len < sizeof(struct nan_evt_data_confirm_info)) {
			INFO_PRINT("[rtw_scenario] buf or len(%d) error!\n", len);
			break;
		}

		p_evt_data = (struct nan_evt_data_confirm_info *)buf;

		dns_data.type = p_evt_data->type;
		dns_data.status = p_evt_data->status;
		dns_data.ndp_id = p_evt_data->ndp_id;
		dns_data.mc_id = p_evt_data->mc_id;
		dns_data.nmsg_id = p_evt_data->nmsg_id;
		memcpy(dns_data.initiator_ndi, p_evt_data->initiator_data_address, ETH_ALEN);
		memcpy(dns_data.responder_ndi, p_evt_data->responder_data_address, ETH_ALEN);
		dns_data.port_number = p_evt_data->port_number;
		memcpy(dns_data.host_name, p_evt_data->host_name, sizeof(p_evt_data->host_name));
		/* subscriber role todo:
		 * parsing peer ipv6 from ndpe (should be the same as the one converted from ndi */

		if (g_dns_evt_cb) {
			g_dns_evt_cb(DNS_DATA_CONFIRM, &dns_data, sizeof(dns_data));
		}
	}
	break;
	case NAN_EVT_BSTRAP_RESULT: {
		struct dns_evt_bstrap_result dns_data = {0};
		uint32_t dns_data_len = sizeof(struct dns_evt_bstrap_result);
		struct nan_evt_bstrap_result *p_evt_data = NULL;

		INFO_PRINT("[rtw_scenario] %s: recv data bstrap result\n", __func__);

		if (!buf || len < sizeof(struct nan_evt_bstrap_result)) {
			INFO_PRINT("[rtw_scenario] buf or len(%d) error!\n", len);
			break;
		}

		p_evt_data = (struct nan_evt_bstrap_result *)buf;

		dns_data.status = p_evt_data->status;
		dns_data.remote_inst_id = p_evt_data->remote_inst_id;
		dns_data.local_inst_id = p_evt_data->local_inst_id;
		dns_data.local_role = p_evt_data->local_role;
		dns_data.bstrap_method = p_evt_data->bstrap_method;
		memcpy(dns_data.peer_nmi, p_evt_data->peer_nmi, ETH_ALEN);

		if (g_dns_evt_cb) {
			g_dns_evt_cb(DNS_BSTRAP_RESULT, &dns_data, sizeof(dns_data));
		}
	}
	break;
	case NAN_EVT_PASN_RESULT: {
		struct dns_evt_pasn_result dns_data = {0};
		uint32_t dns_data_len = sizeof(struct dns_evt_pasn_result);
		struct nan_evt_pasn_result *p_evt_data = NULL;
		struct srvc_info followup_srvc = {0};
		struct tx_followup_info tx_flwup = {0};
		struct srvc_info *srvc_info = _get_pairing_service();

		INFO_PRINT("[rtw_scenario] %s: recv data pasn result\n", __func__);

		if (!buf || len < sizeof(struct nan_evt_pasn_result)) {
			INFO_PRINT("[rtw_scenario] buf or len(%d) error!\n", len);
			break;
		}
		if (!srvc_info) {
			return;
		}

		p_evt_data = (struct nan_evt_pasn_result *)buf;

		dns_data.status = p_evt_data->status;
		dns_data.local_role = p_evt_data->local_role;
		dns_data.pairing_type = p_evt_data->pairing_type;
		dns_data.remote_nik_caching = p_evt_data->remote_nik_caching;
		memcpy(dns_data.peer_nmi, p_evt_data->peer_nmi, ETH_ALEN);

		/* NIK exchange */
		if (srvc_info->pairing_info.nik_caching && p_evt_data->remote_nik_caching) {
			if (dns_data.pairing_type == NAN_PAIRING_TYPE_SETUP) {
				tx_flwup.type = TX_FOLLOW_UP_KEY_EXCHG;
				if (srvc_info->pairing_info.role == PAIRING_ROLE_INITIATOR) {
					tx_flwup.remote_inst_id = srvc_info->publish_id;
					tx_flwup.local_inst_id = srvc_info->subscribe_id;
				} else {
					tx_flwup.remote_inst_id = srvc_info->subscribe_id;
					tx_flwup.local_inst_id = srvc_info->publish_id;
				}
				memcpy(tx_flwup.remote_mac, srvc_info->pairing_info.peer_nmi, ETH_ALEN);
				tx_flwup.sec_key_type = NAN_SEC_KEY_TYPE_NIK;
				rtw_nan_api_send_follow_up(&tx_flwup);
			}
		}

		if (g_dns_evt_cb) {
			g_dns_evt_cb(DNS_PASN_RESULT, &dns_data, sizeof(dns_data));
		}
	}
	break;
	case NAN_EVT_DATA_SHARED_KEY: {
		struct dns_evt_data_shared_key_result dns_data = {0};
		uint32_t dns_data_len = sizeof(struct dns_evt_data_shared_key_result);
		struct nan_evt_data_recv_shared_key *p_evt_data = NULL;

		INFO_PRINT("[rtw_scenario] %s: recv peer shared key result\n", __func__);

		if (!buf || len < sizeof(struct nan_evt_data_recv_shared_key)) {
			INFO_PRINT("[rtw_scenario] buf or len(%d) error!\n", len);
			break;
		}

		p_evt_data = (struct nan_evt_data_recv_shared_key *)buf;

		dns_data.role = p_evt_data->role;
		dns_data.status = p_evt_data->status;
		memcpy(dns_data.peer_nmi, p_evt_data->peer_nmi, ETH_ALEN);

		if (g_dns_evt_cb) {
			g_dns_evt_cb(DNS_DATA_SHARED_KEY, &dns_data, sizeof(dns_data));
		}
	}
	break;
	case NAN_EVT_BLE_RECV_TIGGER:
		INFO_PRINT("[rtw_scenario] %s: ble recv trigger frame\n", __func__);
		if (rtw_nan_api_start_nan(nan_intf) != RTW_RET_STATUS_SUCCESS) {
			INFO_PRINT("[rtw_scenario] failed to start NAN!\n");
			break;
		}
		for (i = 0; i < MAX_SERVICE_NUM ; i++) {
			if (srvc_info_list[i]) {
				rtw_nan_api_publish(nan_intf, srvc_info_list[i]);
			}
		}
		b_nan_start = true;
		break;
	case NAN_EVT_BLE_RECV_TERMINATE:
		INFO_PRINT("[rtw_scenario] %s: ble recv terminate frame\n", __func__);
		rtw_nan_api_stop_nan(nan_intf);
		b_nan_start = false;
		break;
	default:
		INFO_PRINT("[rtw_scenario] %s: event_id(%d) not handle\n", __func__, event_id);
		break;
	}
}

RTW_RET_STATUS dns_publish(uint16_t phy_num,
						   struct utility_bin_path *bin_path,
						   struct srvc_info *info,
						   uint8_t is_bt_trigger)
{
	uint16_t nan_cap = 0;
	char nmi[20] = {0};
	char self_ipv6[50] = {0};
	RTW_RET_STATUS ret = RTW_RET_STATUS_SUCCESS;
	uint16_t i = 0;

	if (_nan_set_bin_path(bin_path) == false) {
		INFO_PRINT("[rtw_scenario] failed to set bin path!\n");
		ret = RTW_RET_STATUS_INVALID_INPUT;
		goto exit;
	}

	if (rtw_nan_api_init(phy_num, nan_intf) != RTW_RET_STATUS_SUCCESS) {
		INFO_PRINT("[rtw_scenario] failed to init NAN!\n");
		ret = RTW_RET_STATUS_INIT_ERROR;
		goto exit;
	}

	if (_nan_get_nmi(nan_intf, nmi) == false) {
		INFO_PRINT("[rtw_scenario] failed to get nmi!\n");
		ret = RTW_RET_STATUS_INIT_ERROR;
		goto exit;
	}

	INFO_PRINT("[rtw_scenario] %s %d\n", __func__, __LINE__);

	b_nan_start = false;
	b_ble_trigger_enable = is_bt_trigger;

	INFO_PRINT("[rtw_scenario] %s b_ble_trigger_enable=%d\n", __func__, b_ble_trigger_enable);

	rtw_nan_api_get_capability(nan_intf, &nan_cap);
	INFO_PRINT("[rtw_scenario] %s %d\n", __func__, __LINE__);
	rtw_nan_api_register_evt_cb(nan_evt_handler);
	INFO_PRINT("[rtw_scenario] %s %d\n", __func__, __LINE__);
	_mac_addr_a2n(info->publish_nmi, nmi);
	for (i = 0; i < MAX_SERVICE_NUM; i++) {
		if (srvc_info_list[i] == NULL) {
			srvc_info_list[i] = info;
			break;
		}
	}

	INFO_PRINT("[rtw_scenario] service info:\n");
	INFO_PRINT("[rtw_scenario] publish_id(%d)\n", info->publish_id);
	PRINT_MAC_ADDR("[rtw_scenario] publish_nmi", info->publish_nmi);
	INFO_PRINT("[rtw_scenario] service_name: %s\n", info->service_name);
	INFO_PRINT("[rtw_scenario] service_instance: %s\n", info->service_instance);
	if (info->text_info_len && info->text_info) {
		INFO_PRINT("[rtw_scenario] service_text_info(len=%d) ", info->text_info_len);
		for (i = 0; i < info->text_info_len; i++) {
			INFO_PRINT("%02x ", info->text_info[i]);
		}
		INFO_PRINT("\n");
	}
	INFO_PRINT("[rtw_scenario] bstrap_method: 0X%X\n", info->pairing_info.bstrap_method);
	INFO_PRINT("[rtw_scenario] gtk_enable: %d\n", info->gtk_enable);

	if (b_ble_trigger_enable) {
		INFO_PRINT("[rtw_scenario] start ble-trigger scan.\n");
		/* use ble-trigger: start ble thread for scan event */
		if (rtw_nan_api_ble_init(role_scanner) || rtw_nan_api_ble_start_ble_trigger(1)) {
			INFO_PRINT("[rtw_scenario] ble trigger failed!\n");
			goto exit;
		}
	} else {
		/* without using ble-trigger, start nan directly */
		if (rtw_nan_api_start_nan(nan_intf) != RTW_RET_STATUS_SUCCESS) {
			INFO_PRINT("[rtw_scenario] failed to start NAN!\n");
			goto exit;
		}
		b_nan_start = true;
		if (info->sec_type == NAN_PMK_SET_BY_PAIRING && _start_wpa_supplicant() != true) {
			INFO_PRINT("[rtw_scenario] failed to start wpa_supplicant!\n");
			goto exit;
		}
		if (rtw_nan_api_publish(nan_intf, info) != RTW_RET_STATUS_SUCCESS) {
			INFO_PRINT("[rtw_scenario] failed to start publish service!\n");
			goto exit;
		}
		if (info->sec_type == NAN_PMK_SET_BY_PAIRING) {
			rtw_nan_api_pasn_setpw(NULL, info->pairing_info.pasn_pw, info->pairing_info.pasn_pw_len);
		}
	}

	return ret;
exit:
	dns_stop_service(phy_num);
	return ret;
}

RTW_RET_STATUS dns_subscribe(uint16_t phy_num,
							 struct utility_bin_path *bin_path,
							 struct srvc_info *info,
							 uint8_t is_bt_trigger)
{
	uint16_t nan_cap = 0, i = 0;
	char nmi[20] = {0};
	char self_ipv6[50] = {0};
	RTW_RET_STATUS ret = RTW_RET_STATUS_SUCCESS;

	if (_nan_set_bin_path(bin_path) == false) {
		INFO_PRINT("[rtw_scenario] %s failed to set bin path!\n", __func__);
		ret = RTW_RET_STATUS_INVALID_INPUT;
		goto exit;
	}
	if (rtw_nan_api_init(phy_num, nan_intf) != RTW_RET_STATUS_SUCCESS) {
		INFO_PRINT("[rtw_scenario] %s failed to init NAN!\n", __func__);
		ret = RTW_RET_STATUS_INIT_ERROR;
		goto exit;
	}
	if (_nan_get_nmi(nan_intf, nmi) == false) {
		INFO_PRINT("[rtw_scenario] failed to get nmi!\n");
		ret = RTW_RET_STATUS_INIT_ERROR;
		goto exit;
	}

	INFO_PRINT("[rtw_scenario] %s\n", __func__);
	b_nan_start = false;
	b_ble_trigger_enable = is_bt_trigger;

	rtw_nan_api_get_capability(nan_intf, &nan_cap);
	rtw_nan_api_register_evt_cb(nan_evt_handler);
	_mac_addr_a2n(info->subscribe_nmi, nmi);
	for (i = 0; i < MAX_SERVICE_NUM; i++) {
		if (srvc_info_list[i] == NULL) {
			srvc_info_list[i] = info;
			break;
		}
	}

	INFO_PRINT("[rtw_scenario] service info:\n");
	INFO_PRINT("[rtw_scenario] subscribe_id(%d)\n", info->subscribe_id);
	PRINT_MAC_ADDR("[rtw_scenario] subscribe_nmi", info->subscribe_nmi);
	INFO_PRINT("[rtw_scenario] service_name:%s\n", info->service_name);

	if (b_ble_trigger_enable) {
		INFO_PRINT("[rtw_scenario] start ble-trigger adv: nan-trigger.\n");
		/* start ble thread and send trigger pkt */
		if (rtw_nan_api_ble_init(role_advertiser) || rtw_nan_api_ble_start_ble_trigger(1)) {
			INFO_PRINT("[rtw_scenario] ble trigger failed!\n");
			goto exit;
		}
	}

	/* start nan directly */
	if (rtw_nan_api_start_nan(nan_intf) != RTW_RET_STATUS_SUCCESS) {
		INFO_PRINT("[rtw_scenario] %s failed to start NAN!\n", __func__);
		ret = RTW_RET_STATUS_INIT_ERROR;
		goto exit;
	}
	b_nan_start = true;

	if (info->sec_type == NAN_PMK_SET_BY_PAIRING && _start_wpa_supplicant() != true) {
		INFO_PRINT("[rtw_scenario] failed to start wpa_supplicant!\n");
		goto exit;
	}
	ret = rtw_nan_api_subscribe(nan_intf, info);

	return ret;
exit:
	dns_stop_service(phy_num);
	return ret;
}

RTW_RET_STATUS dns_publish_update(struct srvc_info *info)
{
	RTW_RET_STATUS ret = RTW_RET_STATUS_SUCCESS;
	ret = rtw_nan_api_publish_update(info);
	return ret;
}

RTW_RET_STATUS dns_datapath_req(struct datapath_info *info)
{
	RTW_RET_STATUS ret = RTW_RET_STATUS_SUCCESS;
	ret = rtw_nan_api_send_datapath_req(info);
	return ret;
}

RTW_RET_STATUS dns_datapath_rsp(struct datapath_info *info)
{
	RTW_RET_STATUS ret = RTW_RET_STATUS_SUCCESS;
	ret = rtw_nan_api_send_datapath_rsp(info);
	return ret;
}

RTW_RET_STATUS dns_datapath_end(uint8_t datapath_id)
{
	RTW_RET_STATUS ret = RTW_RET_STATUS_SUCCESS;
	ret = rtw_nan_api_send_datapath_end(datapath_id);
	return ret;
}

RTW_RET_STATUS dns_bstrap_start(struct srvc_info *info)
{
	RTW_RET_STATUS ret = RTW_RET_STATUS_SUCCESS;
	struct tx_followup_info tx_flwup_data = {0};
	struct pairing_info *pairing_info = &info->pairing_info;

	tx_flwup_data.type = TX_FOLLOW_UP_BSTRP_REQ;
	if (pairing_info->role == PAIRING_ROLE_INITIATOR) {
		tx_flwup_data.remote_inst_id = info->publish_id;
		tx_flwup_data.local_inst_id = info->subscribe_id;
	} else {
		tx_flwup_data.remote_inst_id = info->subscribe_id;
		tx_flwup_data.local_inst_id = info->publish_id;
	}
	memcpy(tx_flwup_data.remote_mac, pairing_info->peer_nmi, ETH_ALEN);
	tx_flwup_data.bstrap_method = pairing_info->bstrap_method;

	ret = rtw_nan_api_send_follow_up(&tx_flwup_data);

	return ret;
}

RTW_RET_STATUS dns_pairing_start(struct pairing_info *pairing_info) // refine input
{
	RTW_RET_STATUS ret = RTW_RET_STATUS_SUCCESS;
	ret = rtw_nan_api_pasn_start(pairing_info);
	return ret;
}

RTW_RET_STATUS dns_pairing_setpw(uint8_t *pairing_peer_nmi, char *pw, uint8_t pw_len)
{
	RTW_RET_STATUS ret = RTW_RET_STATUS_SUCCESS;
	ret = rtw_nan_api_pasn_setpw(pairing_peer_nmi, pw, pw_len);
	return ret;
}

void dns_register_evt_cb(rtw_dns_evt_cb cb)
{
	g_dns_evt_cb = cb;
}
