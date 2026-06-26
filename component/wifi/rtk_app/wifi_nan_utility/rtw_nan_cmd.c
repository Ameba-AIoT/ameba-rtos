#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "sha256.h"
#include "rtw_nan_cmd_api.h"
#include "rtw_nan_cmd.h"
#include "nan_event.h"
#include "rtw_nan_vendor_def.h"
#include "whc_ipc_host_nan.h"

#define MAX_NANDOW_PARA_LEN 2600
#define MAX_NANDOW_REPLY_LEN 512
#define OUI_REALTEK 0x00E04C
#define NL80211_SUBCMD_NAN_RANGE_START 0x1900
#define NL80211_SUBCMD_NAN_RANGE_END   0x19FF

typedef enum nan_customer_nandow_cmd_type {
	NANDOW_CMD_TYPE_GET = (1 << 0),
	NANDOW_CMD_TYPE_SET = (1 << 1),
} nan_customer_nandow_cmd_type_t;

enum rtw_nan_vendor_subcmd {
	NAN_SUBCMD_SRVC_EXT_INFO = NL80211_SUBCMD_NAN_RANGE_START,	/* 0x1900 */
	NAN_SUBCMD_DATA_REQ,					/* 0x1901 */
	NAN_SUBCMD_DATA_RSP,					/* 0x1902 */
	NAN_SUBCMD_DATA_END,					/* 0x1903 */
	NAN_SUBCMD_DATA_UPDATE,					/* 0x1904 */
	NAN_SUBCMD_CUSTOMER_NANDOW,				/* 0x1905 */
	NAN_SUBCMD_FOLLOWUP,					/* 0x1906 */
	NAN_SUBCMD_PAIRING_SET_PW,                      	/* 0x1907 */
	NAN_SUBCMD_PASN_START,                          	/* 0x1908 */
	NAN_SUBCMD_PASN_SET_KEY,                          	/* 0x1909 */
};

union nandow_para {
	struct rtw_nan_device_capability dev_cap;
	struct rtw_nan_driver_capabilities drv_cap;
	struct rtw_nan_enable nan_enabled;
	struct rtw_nan_cluster_id cluster_id;
	struct rtw_nan_master_preference master_pref;
	struct rtw_nan_random_factor random_factor;
	struct rtw_nan_discovery_window_awake_period dw_awake_period;
	struct rtw_nan_primary_master_channel pri_master_ch;
	struct rtw_nan_secondary_master_channel second_master_ch;
	struct rtw_nan_discovery_beacon_period disc_bcn_period;
	struct rtw_nan_forced_discovery_beacon_transmission forced_disc_bcn;
	struct rtw_nan_publish_data publish_srv;
	struct rtw_nan_cancel_publish_data cancel_publish_srv;
	struct rtw_nan_subscribe_data subscribe_srv;
	struct rtw_nan_cancel_subscribe_data cancel_sub_srv;
	struct rtw_nan_follow_up_transmit_data followup_srv;
	struct rtw_nan_datapath_request datapath_req;
	struct rtw_nan_datapath_end datapath_end;
	struct rtw_nan_datapath_response datapath_rsp;
	struct rtw_nan_datapath_confirm datapath_confirm;
	struct rtw_nan_committed_availability avail_cmt;
	struct rtw_nan_potential_availability avail_pot;
	struct rtw_nan_data_cluster_availability avail_ndc;
	struct rtw_nan_set_scan_control set_scan_ctl;
	struct rtw_nan_country_code_data country_code;
};

struct nan_customer_nandow {
	uint8_t cmd_id;
	uint8_t cmd_type; // nan_customer_nandow_cmd_type_t
	uint32_t para_len;
	union nandow_para para;
};

/* IPC arch: the NP core owns the NAN interface; these host path/intf globals
 * are unused by the IPC transport and kept as NULL transitionals so existing
 * call sites compile without churn. */
static char *rtw_iw = NULL;
static char *rtw_nan_r3 = NULL;
static char *nan_intf = NULL;
char tmp_file[20] = "nan_cfgvendor.dat";
char cmdreply_file[20] = "cmdreply.dat";

bool is_array_empty(unsigned char *input, unsigned int len)
{
	char tmp_arry[512] = {0};
	return memcmp(input, tmp_arry, (len > 512) ? 512 : len) == 0;
}

int strhex_2_realhex(char *input, char *pattern, int pattern_len, char token)
{
	char *cp = NULL;
	int i = 0, num_byte = 0;

	for (i = 0; i < pattern_len ; i++) {
		unsigned int temp;
		char *cp = strchr(input, token);

		if (cp) {
			*cp = 0;
			cp++;
		}

		if (sscanf(input, "%x", &temp) != 1) {
			input = ++cp;
			continue;
		}

		//printf("%d: pattern[%d] = %x\n", i, num_byte, temp);

		if (temp > 255) {
			input = ++cp;
			continue;
		}

		pattern[num_byte++] = temp;
		if (!cp) {
			break;
		}
		input = cp;
	}

	if (num_byte != pattern_len) {
		return -1;
	}

	return 0;
}

void nandow_pre_actions(struct nan_customer_nandow *nandow_test)
{
	switch (nandow_test->cmd_id) {
	case RTW_NAN_CMD_INIT:
		nandow_test->para_len = sizeof(struct rtw_nan_device_capability);
		break;
	case RTW_NAN_CMD_WIFI_DRV_CAP:
		nandow_test->para_len = sizeof(struct rtw_nan_driver_capabilities);
		break;
	case RTW_NAN_CMD_NAN_ENABLE:
		nandow_test->para_len = sizeof(struct rtw_nan_enable);
		break;
	case RTW_NAN_CMD_CLUSTER_ID:
		nandow_test->para_len = sizeof(struct rtw_nan_cluster_id);
		break;
	case RTW_NAN_CMD_MASTER_PREF:
		nandow_test->para_len = sizeof(struct rtw_nan_master_preference);
		break;
	case RTW_NAN_CMD_RANDOM_FACTOR:
		nandow_test->para_len = sizeof(struct rtw_nan_random_factor);
		break;
	case RTW_NAN_CMD_DW_AWAKE_PERIOD:
		nandow_test->para_len = sizeof(struct rtw_nan_discovery_window_awake_period);
		break;
	case RTW_NAN_CMD_PRI_MASTER_CH:
		nandow_test->para_len = sizeof(struct rtw_nan_primary_master_channel);
		break;
	case RTW_NAN_CMD_SEC_MASTER_CH:
		nandow_test->para_len = sizeof(struct rtw_nan_secondary_master_channel);
		break;
	case RTW_NAN_CMD_DISC_BCN_PERIOD:
		nandow_test->para_len = sizeof(struct rtw_nan_discovery_beacon_period);
		break;
	case RTW_NAN_CMD_FORCED_DISC_BCN:
		nandow_test->para_len = sizeof(struct rtw_nan_forced_discovery_beacon_transmission);
		break;
	case RTW_NAN_CMD_SUBSCRIBE:
		nandow_test->para_len = sizeof(struct rtw_nan_subscribe_data);
		break;
	case RTW_NAN_CMD_PUBLISH:
		nandow_test->para_len = sizeof(struct rtw_nan_publish_data);
		break;
	case RTW_NAN_CMD_CANCEL_SUBSCRIBE:
		nandow_test->para_len = sizeof(struct rtw_nan_cancel_subscribe_data);
		break;
	case RTW_NAN_CMD_CANCEL_PUBLISH:
		nandow_test->para_len = sizeof(struct rtw_nan_cancel_publish_data);
		break;
	case RTW_NAN_CMD_FOLLOWUP_TX:
		nandow_test->para_len = sizeof(struct rtw_nan_follow_up_transmit_data);
		break;
	case RTW_NAN_CMD_DATAPATH_REQ:
		nandow_test->para_len = sizeof(struct rtw_nan_datapath_request);
		break;
	case RTW_NAN_CMD_DATAPATH_RSP:
		nandow_test->para_len = sizeof(struct rtw_nan_datapath_response);
		break;
	case RTW_NAN_CMD_DATAPATH_CONFIRM:
		nandow_test->para_len = sizeof(struct rtw_nan_datapath_confirm);
		break;
	case RTW_NAN_CMD_DATAPATH_END:
		nandow_test->para_len = sizeof(struct rtw_nan_datapath_end);
		break;
	case RTW_NAN_CMD_NDC_AVAIL:
		nandow_test->para_len = sizeof(struct rtw_nan_data_cluster_availability);
		break;
	case RTW_NAN_CMD_SCAN_CONTROL:
		nandow_test->para_len = sizeof(struct rtw_nan_set_scan_control);
		break;
	case RTW_NAN_CMD_COUNTRY_CODE:
		nandow_test->para_len = sizeof(struct rtw_nan_country_code_data);
		break;
	default:
		break;
	}
	memset(&nandow_test->para, 0, sizeof(union nandow_para));
	INFO_PRINT("[rtw_cmd]nandow_pre_actions : id(%d), op(%s), len(%d)\n",
			   nandow_test->cmd_id,
			   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set",
			   nandow_test->para_len);
}

void nandow_send_cmd(struct nan_customer_nandow *nandoow_cmd,
					 void *input,
					 unsigned int input_len,
					 char *tmp_file,
					 char *cmdreply_file,
					 char *if_name,
					 char *fname)
{
	(void)nandoow_cmd;
	(void)tmp_file;
	(void)cmdreply_file;
	(void)if_name;
	(void)fname;

	if (input == NULL || input_len == 0) {
		INFO_PRINT("[rtw_cmd] %s: empty input\n", __func__);
		return;
	}

	/* IPC arch: deliver the NANDOW command to the NP-core NAN driver via the
	 * WHC IPC path instead of an nl80211 'iw vendor' command from a host. */
	whc_host_nan_cfgvendor_cmd(NAN_SUBCMD_CUSTOMER_NANDOW, input, input_len);
}

void nandow_parse_cmd_reply(struct nan_customer_nandow *nandow_test, char *cmdreply_file)
{

	(void)nandow_test;
	(void)cmdreply_file;
	/* IPC arch: GET-command replies arrive asynchronously via the WHC
	 * cmd-reply callback (whc_host_nan_register_cmd_reply_cb), not a host
	 * file. The synchronous file-based reply parsing is removed. */
}
RTW_RET_STATUS send_vendor_cmd(unsigned int cmd_id,
							   void *input,
							   unsigned int input_len,
							   char *tmp_file,
							   char *if_name,
							   char *iw_path)
{
	(void)tmp_file;
	(void)if_name;
	(void)iw_path;

	if (input == NULL || input_len == 0) {
		return RTW_RET_STATUS_INVALID_INPUT;
	}

	/* IPC arch: forward the vendor subcommand to the NP-core NAN driver. */
	return whc_host_nan_cfgvendor_cmd((uint16_t)cmd_id, input, input_len) ?
		   RTW_RET_STATUS_ERROR : RTW_RET_STATUS_SUCCESS;
}


/* IPC arch: the NP core owns the NAN interface; host-side iw/phy/intf probing
 * (popen "iw dev") is removed. */

RTW_RET_STATUS rtw_nan_api_get_capability(char *intf, uint16_t *nan_cap)
{
	struct nan_customer_nandow nandow_cmd = {0};
	struct rtw_nan_driver_capabilities *drv_cap;;
	uint32_t input_len = 0;
	void *input = NULL;

	memset(&nandow_cmd, 0, sizeof(struct nan_customer_nandow));
	nandow_cmd.cmd_id = RTW_NAN_CMD_WIFI_DRV_CAP;
	nandow_cmd.cmd_type = NANDOW_CMD_TYPE_GET;
	nandow_pre_actions(&nandow_cmd);

	input = &nandow_cmd;
	input_len = sizeof(struct nan_customer_nandow);
	INFO_PRINT("[rtw_cmd] %s, %d %d %d, input_len=%d\n", __func__, nandow_cmd.cmd_id, nandow_cmd.cmd_type, nandow_cmd.para_len, input_len);

	nandow_send_cmd(&nandow_cmd, input, input_len,
					tmp_file, cmdreply_file, intf, rtw_iw);
	INFO_PRINT("[rtw_cmd] %s, %d\n", __func__, __LINE__);

	nandow_parse_cmd_reply(&nandow_cmd, cmdreply_file);
	INFO_PRINT("[rtw_cmd] %s, %d\n", __func__, __LINE__);

	drv_cap = &nandow_cmd.para.drv_cap;

	INFO_PRINT("[rtw_cmd] %s, capability(0x%x)\n", __func__, drv_cap->capabilities);

	return RTW_RET_STATUS_SUCCESS;
}


RTW_RET_STATUS rtw_nan_api_init(uint16_t phy_num, char *intf)
{
	(void)phy_num;
	(void)intf;

	INFO_PRINT("[rtw_cmd] %s=>\n", __func__);

	/* IPC arch: bring up the NAN interface on the NP core, then register for
	 * NAN events over the WHC IPC path (replaces iw interface add + evt thread). */
	if (whc_host_nan_init() != 0) {
		INFO_PRINT("[rtw_cmd] whc_host_nan_init fail\n");
		return RTW_RET_STATUS_INIT_ERROR;
	}
	if (rtw_nan_evt_init() == false) {
		return RTW_RET_STATUS_INIT_ERROR;
	}

	DEBUG_PRINT("[rtw_cmd] %s<=\n", __func__);
	return RTW_RET_STATUS_SUCCESS;
}

RTW_RET_STATUS rtw_nan_api_deinit(uint16_t phy_num)
{
	(void)phy_num;

	INFO_PRINT("[rtw_cmd] %s\n", __func__);

	rtw_nan_evt_deinit();
	whc_host_nan_deinit();

	return RTW_RET_STATUS_SUCCESS;
}

RTW_RET_STATUS rtw_nan_api_start_nan(char *intf)
{
	struct nan_customer_nandow nandow_cmd;
	struct rtw_nan_enable *nan_enabled;;
	uint32_t input_len = 0;
	void *input = NULL;

	DEBUG_PRINT("[rtw_cmd] %s\n", __func__);

	memset(&nandow_cmd, 0, sizeof(struct nan_customer_nandow));
	nandow_cmd.cmd_id = RTW_NAN_CMD_NAN_ENABLE;
	nandow_cmd.cmd_type = NANDOW_CMD_TYPE_SET;
	nandow_pre_actions(&nandow_cmd);

	nan_enabled = &nandow_cmd.para.nan_enabled;
	nan_enabled->enable = 1;

	input = &nandow_cmd;
	input_len = sizeof(struct nan_customer_nandow);
	nandow_send_cmd(&nandow_cmd, input, input_len,
					tmp_file, cmdreply_file, intf, rtw_iw);

	return RTW_RET_STATUS_SUCCESS;
}

RTW_RET_STATUS rtw_nan_api_stop_nan(char *intf)
{
	struct nan_customer_nandow nandow_cmd;
	struct rtw_nan_enable *nan_enabled;;
	uint32_t input_len = 0;
	void *input = NULL;

	INFO_PRINT("[rtw_cmd] %s\n", __func__);

	memset(&nandow_cmd, 0, sizeof(struct nan_customer_nandow));
	nandow_cmd.cmd_id = RTW_NAN_CMD_NAN_ENABLE;
	nandow_cmd.cmd_type = NANDOW_CMD_TYPE_SET;
	nandow_pre_actions(&nandow_cmd);

	nan_enabled = &nandow_cmd.para.nan_enabled;
	nan_enabled->enable = 0;

	input = &nandow_cmd;
	input_len = sizeof(struct nan_customer_nandow);
	nandow_send_cmd(&nandow_cmd, input, input_len,
					tmp_file, cmdreply_file, intf, rtw_iw);

	return RTW_RET_STATUS_SUCCESS;
}

static int _compute_service_id(const uint8_t *serv_name, uint32_t len, uint8_t *res)
{
	size_t size = len;
	unsigned char md_value[32];
	int retcode = sha256(serv_name, size, md_value);

	if (retcode) {
		return retcode;
	}

	memcpy(res, md_value, 6);
	return 0;
}

uint16_t _append_gen_sub_attr(uint8_t attr_id, uint8_t *pbuf, void *pdata, uint8_t type)
{
	uint16_t len = 0, attr_len = 0, i = 0;
	struct srvc_info *srv_info = NULL;
	struct datapath_info *dp_info = NULL;

	if (type == NAN_WFA_SRVC_ATTR_SEDA) {
		srv_info = (struct srvc_info *)pdata;
	} else if (type == NAN_WFA_SRVC_ATTR_NDPE) {
		dp_info = (struct datapath_info *)pdata;
	} else {
		INFO_PRINT("[rtw_cmd]%s unknow type[%d]\n", __func__, type);
		return 0;
	}

	switch (attr_id) {
	case NAN_GEN_SRVC_TRANSPORT_PORT:
		if (type != NAN_WFA_SRVC_ATTR_NDPE) {
			INFO_PRINT("[rtw_cmd] type(%d) != NAN_WFA_SRVC_ATTR_NDPE (skipped), ", type);
			break;
		}
		INFO_PRINT("[rtw_cmd] append NAN_GEN_SRVC_TRANSPORT_PORT[%d], ",
				   attr_id);
		*pbuf = NAN_GEN_SRVC_TRANSPORT_PORT;
		attr_len = 2;
		memcpy(pbuf + 1, &attr_len, 2);
		memcpy(pbuf + 3, &dp_info->port_number, attr_len);
		len = attr_len + 3;
		break;
	case NAN_GEN_SRVC_TRANSPORT_PROTOCOL:
		if (type != NAN_WFA_SRVC_ATTR_NDPE) {
			INFO_PRINT("[rtw_cmd] type(%d) != NAN_WFA_SRVC_ATTR_NDPE (skipped), ", type);
			break;
		}
		INFO_PRINT("[rtw_cmd] append NAN_GEN_SRVC_TRANSPORT_PROTOCOL[%d], ", attr_id);
		*pbuf = NAN_GEN_SRVC_TRANSPORT_PROTOCOL;
		attr_len = 1;
		memcpy(pbuf + 1, &attr_len, 2);
		*(pbuf + 3) = NAN_GEN_SRVC_PORT_PROTOCOL_TCP;
		len = attr_len + 3;
		break;
	case NAN_GEN_SRVC_SERVICE_NAME:
		if (type != NAN_WFA_SRVC_ATTR_SEDA) {
			INFO_PRINT("[rtw_cmd] type(%d) != NAN_WFA_SRVC_ATTR_SEDA (skipped), ", type);
			break;
		}
		INFO_PRINT("[rtw_cmd] append NAN_GEN_SRVC_SERVICE_NAME[%d], ", attr_id);
		srv_info = (struct srvc_info *)pdata;
		*pbuf = NAN_GEN_SRVC_SERVICE_NAME;
		attr_len = strlen(srv_info->service_name);
		memcpy(pbuf + 1, &attr_len, 2);
		memcpy(pbuf + 3, srv_info->service_name, attr_len);
		len = attr_len + 3;
		break;
	case NAN_GEN_SRVC_NAME_OF_SRVC_INST:
		INFO_PRINT("[rtw_cmd] append NAN_GEN_SRVC_NAME_OF_SRVC_INST[%d], ", attr_id);
		srv_info = (struct srvc_info *)pdata;
		*pbuf = NAN_GEN_SRVC_NAME_OF_SRVC_INST;
		if (type == NAN_WFA_SRVC_ATTR_SEDA) {
			attr_len = strlen(srv_info->service_instance);
			memcpy(pbuf + 1, &attr_len, 2);
			memcpy(pbuf + 3, srv_info->service_instance, attr_len);
		} else {
			attr_len = strlen(dp_info->hostname);
			memcpy(pbuf + 1, &attr_len, 2);
			memcpy(pbuf + 3, dp_info->hostname, attr_len);
		}
		len = attr_len + 3;
		break;
	case NAN_GEN_SRVC_TEXT_INFO:
		if (type != NAN_WFA_SRVC_ATTR_SEDA) {
			INFO_PRINT("[rtw_cmd] type(%d) != NAN_WFA_SRVC_ATTR_SEDA (skipped), ", type);
			break;
		}
		INFO_PRINT("[rtw_cmd] append NAN_GEN_SRVC_TEXT_INFO[%d], ", attr_id);
		srv_info = (struct srvc_info *)pdata;
		*pbuf = NAN_GEN_SRVC_TEXT_INFO;
		attr_len = srv_info->text_info_len;
		memcpy(pbuf + 1, &attr_len, 2);
		memcpy(pbuf + 3, srv_info->text_info, attr_len);
		len = attr_len + 3;
		break;
	case NAN_GEN_SRVC_UUID:
		INFO_PRINT("[rtw_cmd] append NAN_GEN_SRVC_UUID[%d] (skipped), ", attr_id);
		break;
	case NAN_GEN_SRVC_BLOB:
		INFO_PRINT("[rtw_cmd] append NAN_GEN_SRVC_BLOB[%d] (skipped), ", attr_id);
		break;
	case NAN_GEN_SRVC_RESERVD:
		INFO_PRINT("[rtw_cmd] append NAN_GEN_SRVC_RESERVD[%d] (skipped), ", attr_id);
		break;
	case NAN_GEN_SRVC_VENDOR_SPEC_INFO:
		INFO_PRINT("[rtw_cmd] append NAN_GEN_SRVC_VENDOR_SPEC_INFO[%d] (skipped), ",
				   attr_id);
		break;
	default:
		INFO_PRINT("[rtw_cmd] unknow sub attribute id[%d], ", attr_id);
		break;
	}

	INFO_PRINT("len = %d", len);
	for (i = 0; i < len; i++) {
		if (!(i % 16)) {
			DEBUG_PRINT("\n");
		}
		DEBUG_PRINT("%02x ", pbuf[i]);
	}
	INFO_PRINT("\n");

	return len;
}

uint16_t _construct_sdea_gen_srvc_info(struct srvc_info *info, uint8_t *gen_srv_info)
{
	uint16_t len = 0;
	uint8_t temp_srv_info[RTW_NAN_SERVICE_INFO_MAX_LENGTH] = {0};
	uint16_t idx = 0, total_len = 0;
	int i = 0;

	INFO_PRINT("[rtw_cmd] %s\n", __func__);

	/* Wi-Fi Aware specificcation v3.2 9.5.4.1*/
	/* WFA OUI = 0x50, 0x6F, 0x9A */
	memcpy(&temp_srv_info[0], WFA_OUI, 3);
	idx += 3;

	/* Service Protocol Type */
	temp_srv_info[idx] = NAN_SRVC_PROTOCOL_GENIC;
	idx += 1;

	/* Generic Service Protocol sub-attribute */
	if (info->service_name && strlen(info->service_name)) {
		len = _append_gen_sub_attr(NAN_GEN_SRVC_SERVICE_NAME,
								   &temp_srv_info[idx],
								   info,
								   NAN_WFA_SRVC_ATTR_SEDA);
		idx += len;
	}
	if (info->service_instance && strlen(info->service_instance)) {
		len = _append_gen_sub_attr(NAN_GEN_SRVC_NAME_OF_SRVC_INST,
								   &temp_srv_info[idx],
								   info,
								   NAN_WFA_SRVC_ATTR_SEDA);
		idx += len;
	}
	if (info->text_info_len && info->text_info) {
		len = _append_gen_sub_attr(NAN_GEN_SRVC_TEXT_INFO,
								   &temp_srv_info[idx],
								   info,
								   NAN_WFA_SRVC_ATTR_SEDA);
		idx += len;
	}

	total_len = idx;
	if (total_len > RTW_NAN_SERVICE_INFO_MAX_LENGTH) {
		INFO_PRINT("[rtw_cmd] service info length(%d) is out of limit\n", total_len);
		return 0;
	}

	memcpy(gen_srv_info, temp_srv_info, total_len);

	INFO_PRINT("[rtw_cmd] sdea service info (len=%d)", total_len);
	for (i = 0; i < total_len; i++) {
		if (!(i % 16)) {
			DEBUG_PRINT("\n");
		}
		DEBUG_PRINT("%02x ", gen_srv_info[i]);
	}
	INFO_PRINT("\n");

	return total_len;
}

uint16_t _construct_ndpe_gen_srvc_info(struct datapath_info *info, uint8_t *gen_srv_info)
{
	uint8_t type = 0;
	uint16_t len = 0;
	uint8_t temp_srv_info[RTW_NAN_SERVICE_INFO_MAX_LENGTH] = {0};
	uint16_t idx = 0, total_len = 0;
	int i = 0;

	INFO_PRINT("[rtw_cmd]%s \n", __func__);

	/* Wi-Fi Aware specificcation v3.2 9.5.16.2*/
	/* WFA OUI = 0x50, 0x6F, 0x9A */
	memcpy(&temp_srv_info[0], WFA_OUI, 3);
	idx += 3;

	/* Service Protocol Type */
	temp_srv_info[idx] = NAN_SRVC_PROTOCOL_GENIC;
	idx += 1;

	/* Generic Service Protocol sub-attribute */
	/* Need to remove hostname because MAC may not be able to parsing port
	   when adding this in service specific info */
	/* if (info->hostname && strlen(info->hostname)) {
		len = _append_gen_sub_attr(NAN_GEN_SRVC_NAME_OF_SRVC_INST,
				           &temp_srv_info[idx],
				           info,
				           NAN_WFA_SRVC_ATTR_NDPE);
		idx += len;
	}*/

	if (info->port_number) {
		len = _append_gen_sub_attr(NAN_GEN_SRVC_TRANSPORT_PORT,
								   &temp_srv_info[idx],
								   info,
								   NAN_WFA_SRVC_ATTR_NDPE);
		idx += len;

		len = _append_gen_sub_attr(NAN_GEN_SRVC_TRANSPORT_PROTOCOL,
								   &temp_srv_info[idx],
								   info,
								   NAN_WFA_SRVC_ATTR_NDPE);
		idx += len;
	}

	total_len = idx;
	if (total_len == 4) {
		return 0;
	}

	if (total_len > RTW_NAN_SERVICE_INFO_MAX_LENGTH) {
		INFO_PRINT("[rtw_cmd]service info length is out of limit %d \n", total_len);
		return 0;
	}

	memcpy(gen_srv_info, temp_srv_info, total_len);

	INFO_PRINT("[rtw_cmd]ndpe service info (len=%d)", total_len);
	for (i = 0; i < total_len; i++) {
		if (!(i % 16)) {
			DEBUG_PRINT("\n");
		}
		DEBUG_PRINT("%02x ", gen_srv_info[i]);
	}
	INFO_PRINT("\n");

	return total_len;
}


RTW_RET_STATUS rtw_nan_api_publish(char *intf, struct srvc_info *info)
{
	struct nan_customer_nandow nandow_cmd = {0};
	struct rtw_nan_publish_data *publish_srv;
	uint32_t input_len = 0;
	void *input = NULL;
	uint8_t srv_name_hash[RTW_NAN_SERVICE_NAME_HASH_SIZE] = {0};
	char tmp_arry[512] = {0};
	uint8_t gen_srv_info[RTW_NAN_SERVICE_INFO_MAX_LENGTH] = {0};
	uint16_t gen_srv_info_len = 0;

	if (!intf || !info) {
		INFO_PRINT("[rtw_cmd] set publish fail! \n");
		return RTW_RET_STATUS_INVALID_INPUT;
	}

	memset(&nandow_cmd, 0, sizeof(struct nan_customer_nandow));
	nandow_cmd.cmd_id = RTW_NAN_CMD_PUBLISH;
	nandow_cmd.cmd_type = NANDOW_CMD_TYPE_SET;
	nandow_pre_actions(&nandow_cmd);

	/* initialize parameters */
	publish_srv = &nandow_cmd.para.publish_srv;

	publish_srv->publish_type = RTW_NAN_PUBLISH_TYPE_SOLICITED;

	_compute_service_id((const unsigned char *)info->service_name,
						strlen(info->service_name),
						srv_name_hash);
	memcpy(publish_srv->service_hash, srv_name_hash, RTW_NAN_SERVICE_NAME_HASH_SIZE);

	publish_srv->matching_filter_rx_count = 1;
	sprintf(tmp_arry, "%s", "bytes used for matching");
	memcpy(publish_srv->matching_filter_rx[0].filter, tmp_arry, sizeof("bytes used for matching"));
	publish_srv->matching_filter_rx[0].length = sizeof("bytes used for matching") - 1;

	memcpy(publish_srv->name.name, info->service_name, strlen(info->service_name));
	publish_srv->publish_id = info->publish_id;
	publish_srv->awake_discovery_window_interval = 1;
	publish_srv->ttl = 1024;
	publish_srv->flags = RTW_NAN_PUBLISH_FLAG_ADD_DEVICE_CAPABILITY_ATTRIBUTE;

	if (info->sec_type == NAN_PMK_SET_BY_USER_PMK) {
		INFO_PRINT("[rtw_cmd] Security type of publish is PMK!\n");
		publish_srv->control =
			(RTW_NAN_SDE_CTL_DATAPATH_REQ_REQUIRED |
			 RTW_NAN_SDE_CTL_DATAPATH_TYPE |
			 RTW_NAN_SDE_CTL_SECURITY_REQUIRED);
		publish_srv->cipher_suite_id |= BIT(RTW_NAN_CIPHER_ID_NCS_SK_128);
		memcpy(publish_srv->key, info->pmk, NAN_PMK_SIZE);
	} else if (info->sec_type == NAN_PMK_SET_BY_PAIRING) {
		publish_srv->pairing_enable = true;
		INFO_PRINT("[rtw_cmd] Security type of publish is Pairing!\n");
		publish_srv->control =
			(RTW_NAN_SDE_CTL_DATAPATH_REQ_REQUIRED |
			 RTW_NAN_SDE_CTL_DATAPATH_TYPE |
			 RTW_NAN_SDE_CTL_SECURITY_REQUIRED);
		publish_srv->bstrap_method = info->pairing_info.bstrap_method;
		publish_srv->npk_nik = info->pairing_info.nik_caching;
		publish_srv->cipher_suite_id = (BIT(RTW_NAN_CIPHER_ID_NCS_SK_128) |
										BIT(RTW_NAN_CIPHER_ID_PASN_128));
	} else {
		/* Secruity type = OPEN */
		INFO_PRINT("[rtw_cmd] Security type of publish is open!\n");
		publish_srv->control =
			(RTW_NAN_SDE_CTL_DATAPATH_REQ_REQUIRED |
			 RTW_NAN_SDE_CTL_DATAPATH_TYPE);
	}

	/* SDEA Gerenic Service Info */
	gen_srv_info_len = _construct_sdea_gen_srvc_info(info, gen_srv_info);
	memcpy(publish_srv->service_specific_info.info, gen_srv_info, gen_srv_info_len);
	publish_srv->service_specific_info.length = gen_srv_info_len;

	/* Qos */
	publish_srv->qos.max_service_internal = 1;
	publish_srv->qos.low_latency_required = 1;

	/* Group key */
	if ((publish_srv->control & RTW_NAN_SDE_CTL_SECURITY_REQUIRED) &&
		(info->gtk_enable)) {
		publish_srv->gtk = true;
	}

	/* send nandow command */
	input = &nandow_cmd;
	input_len = sizeof(struct nan_customer_nandow);
	nandow_send_cmd(&nandow_cmd, input, input_len, tmp_file, cmdreply_file, intf, rtw_iw);

	INFO_PRINT("[rtw_cmd] add service complete:\n");
	INFO_PRINT("[rtw_cmd] service_name = %s\n", publish_srv->name.name);
	INFO_PRINT("[rtw_cmd] service_id = 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
			   publish_srv->service_hash[0], publish_srv->service_hash[1],
			   publish_srv->service_hash[2], publish_srv->service_hash[3],
			   publish_srv->service_hash[4], publish_srv->service_hash[5]);
	INFO_PRINT("[rtw_cmd] publish_id = %d\n", publish_srv->publish_id);
	INFO_PRINT("[rtw_cmd] pairing_enable = %d\n", publish_srv->pairing_enable);
	INFO_PRINT("[rtw_cmd] cipher_suite_id = %d\n", publish_srv->cipher_suite_id);
	INFO_PRINT("[rtw_cmd] bstrap_method = %d\n", publish_srv->bstrap_method);
	INFO_PRINT("[rtw_cmd] gtk = %d\n", publish_srv->gtk);
	INFO_PRINT("[rtw_cmd] npk_nik = %d\n", publish_srv->npk_nik);

	return RTW_RET_STATUS_SUCCESS;
}

RTW_RET_STATUS rtw_nan_api_publish_update(struct srvc_info *info)
{
	struct nan_customer_nandow nandow_cmd = {0};
	struct rtw_nan_publish_data *publish_srv;
	uint32_t input_len = 0;
	void *input = NULL;
	uint8_t gen_srv_info[RTW_NAN_SERVICE_INFO_MAX_LENGTH] = {0};
	uint16_t gen_srv_info_len = 0;

	if (!info) {
		INFO_PRINT("[rtw_cmd] update publish fail! \n");
		return RTW_RET_STATUS_INVALID_INPUT;
	}

	INFO_PRINT("[rtw_cmd] %s \n", __func__);
	INFO_PRINT("[rtw_cmd] publish_id = %d\n", info->publish_id);
	INFO_PRINT("[rtw_cmd] service_instance = %s\n", info->service_instance);

	memset(&nandow_cmd, 0, sizeof(struct nan_customer_nandow));
	nandow_cmd.cmd_id = RTW_NAN_CMD_PUBLISH;
	nandow_cmd.cmd_type = NANDOW_CMD_TYPE_SET;
	nandow_pre_actions(&nandow_cmd);

	/* initialize parameters */
	publish_srv = &nandow_cmd.para.publish_srv;
	publish_srv->publish_type = RTW_NAN_PUBLISH_TYPE_SOLICITED;
	publish_srv->service_update_indicator = 1;

	publish_srv->publish_id = info->publish_id;
	gen_srv_info_len = _construct_sdea_gen_srvc_info(info, gen_srv_info);
	memcpy(publish_srv->service_specific_info.info, gen_srv_info, gen_srv_info_len);
	publish_srv->service_specific_info.length = gen_srv_info_len;


	/* send nandow command */
	input = &nandow_cmd;
	input_len = sizeof(struct nan_customer_nandow);
	nandow_send_cmd(&nandow_cmd, input, input_len, tmp_file, cmdreply_file, nan_intf, rtw_iw);

	INFO_PRINT("[rtw_cmd] update service complete:\n");

	return RTW_RET_STATUS_SUCCESS;
}


RTW_RET_STATUS rtw_nan_api_subscribe(char *intf, struct srvc_info *info)
{
	struct nan_customer_nandow nandow_cmd;
	struct rtw_nan_subscribe_data *subscribe_srv;
	uint32_t input_len = 0;
	void *input = NULL;
	uint8_t srv_name_hash[RTW_NAN_SERVICE_NAME_HASH_SIZE] = {0};
	char tmp_arry[512] = {0};
	uint8_t gen_srv_info[RTW_NAN_SERVICE_INFO_MAX_LENGTH] = {0};
	uint16_t gen_srv_info_len = 0;

	if (!intf || !info) {
		INFO_PRINT("[rtw_cmd] set subscribe fail! \n");
		return RTW_RET_STATUS_INVALID_INPUT;
	}

	memset(&nandow_cmd, 0, sizeof(struct nan_customer_nandow));
	nandow_cmd.cmd_id = RTW_NAN_CMD_SUBSCRIBE;
	nandow_cmd.cmd_type = NANDOW_CMD_TYPE_SET;
	nandow_pre_actions(&nandow_cmd);

	/* initialize parameters */
	subscribe_srv = &nandow_cmd.para.subscribe_srv;

	subscribe_srv->subscribe_type = RTW_NAN_SUBSCRIBE_TYPE_ACTIVE;

	_compute_service_id((const unsigned char *)info->service_name,
						strlen(info->service_name),
						srv_name_hash);
	memcpy(subscribe_srv->service_hash, srv_name_hash, RTW_NAN_SERVICE_NAME_HASH_SIZE);

	subscribe_srv->matching_filter_tx_count = 1;
	sprintf(tmp_arry, "%s", "bytes used for matching");
	memcpy(subscribe_srv->matching_filter_tx[0].filter, tmp_arry, sizeof("bytes used for matching"));
	subscribe_srv->matching_filter_tx[0].length = sizeof("bytes used for matching") - 1;

	memcpy(subscribe_srv->name.name, info->service_name, strlen(info->service_name));
	subscribe_srv->subscribe_id = info->subscribe_id;
	subscribe_srv->awake_discovery_window_interval = 1;
	subscribe_srv->ttl = 1024;
	subscribe_srv->flags = RTW_NAN_PUBLISH_FLAG_ADD_DEVICE_CAPABILITY_ATTRIBUTE;

	if (info->sec_type == NAN_PMK_SET_BY_USER_PMK) {
		INFO_PRINT("[rtw_cmd] Security type of subscribe is PMK!\n");
		subscribe_srv->control =
			(RTW_NAN_SDE_CTL_DATAPATH_REQ_REQUIRED |
			 RTW_NAN_SDE_CTL_DATAPATH_TYPE |
			 RTW_NAN_SDE_CTL_SECURITY_REQUIRED);
	} else if (info->sec_type == NAN_PMK_SET_BY_PAIRING) {
		INFO_PRINT("[rtw_cmd] Security type of subscribe is Pairing!\n");
		subscribe_srv->pairing_enable = true;
		subscribe_srv->control =
			(RTW_NAN_SDE_CTL_DATAPATH_REQ_REQUIRED |
			 RTW_NAN_SDE_CTL_DATAPATH_TYPE |
			 RTW_NAN_SDE_CTL_SECURITY_REQUIRED);
		subscribe_srv->bstrap_method = info->pairing_info.bstrap_method;
		subscribe_srv->npk_nik = info->pairing_info.nik_caching;
	} else {
		/* Secruity type = OPEN */
		INFO_PRINT("[rtw_cmd] Security type of subscribe is open!\n");
		subscribe_srv->control =
			(RTW_NAN_SDE_CTL_DATAPATH_REQ_REQUIRED |
			 RTW_NAN_SDE_CTL_DATAPATH_TYPE);
	}

	/* SDEA Generic Service Info */
	gen_srv_info_len = _construct_sdea_gen_srvc_info(info, gen_srv_info);
	memcpy(subscribe_srv->service_specific_info.info, gen_srv_info, gen_srv_info_len);
	subscribe_srv->service_specific_info.length = gen_srv_info_len;

	/* send nandow command */
	input = &nandow_cmd;
	input_len = sizeof(struct nan_customer_nandow);
	nandow_send_cmd(&nandow_cmd, input, input_len,
					tmp_file, cmdreply_file, intf, rtw_iw);

	INFO_PRINT("[rtw_cmd] add service complete:\n");
	INFO_PRINT("[rtw_cmd] service_name = %s\n", subscribe_srv->name.name);
	INFO_PRINT("[rtw_cmd] service_id = 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
			   subscribe_srv->service_hash[0], subscribe_srv->service_hash[1],
			   subscribe_srv->service_hash[2], subscribe_srv->service_hash[3],
			   subscribe_srv->service_hash[4], subscribe_srv->service_hash[5]);
	INFO_PRINT("[rtw_cmd] subscribe_id = %d\n", subscribe_srv->subscribe_id);
	INFO_PRINT("[rtw_cmd] bstrap_method = %d\n", subscribe_srv->bstrap_method);
	INFO_PRINT("[rtw_cmd] npk_nik = %d\n", subscribe_srv->npk_nik);

	return RTW_RET_STATUS_SUCCESS;
}

RTW_RET_STATUS rtw_nan_api_send_datapath_req(struct datapath_info *info)
{
	struct nan_customer_nandow nandow_cmd = {0};
	struct rtw_nan_datapath_request *dp_req = NULL;
	struct pairing_info *pairing_data = &info->service->pairing_info;
	uint32_t input_len = 0, i = 0;
	unsigned int cmd_id, no_input = 0xff;
	void *input = NULL;
	uint8_t gen_srv_info[RTW_NAN_SERVICE_INFO_MAX_LENGTH] = {0};
	uint16_t gen_srv_info_len = 0;
	char tmp_arry[NAN_PMK_SIZE] = {0};

	if (!info) {
		INFO_PRINT("[rtw_cmd] send datapath req fail! \n");
		return RTW_RET_STATUS_INVALID_INPUT;
	}

	INFO_PRINT("[rtw_cmd] %s \n", __func__);

	nandow_cmd.cmd_id = RTW_NAN_CMD_DATAPATH_REQ;
	nandow_cmd.cmd_type = NANDOW_CMD_TYPE_SET;
	nandow_pre_actions(&nandow_cmd);

	/* start initialize parameters here */
	dp_req = &nandow_cmd.para.datapath_req;
	dp_req->datapath_id = info->ndp_id;
	dp_req->responder_publish_id = info->service->publish_id;
	dp_req->initiator_subscribe_id = info->service->subscribe_id;
	memcpy(dp_req->responder_nan_addr.ether_addr_octet, info->responder_ndi, ETH_ALEN);
	gen_srv_info_len = _construct_ndpe_gen_srvc_info(info, gen_srv_info);
	if (gen_srv_info_len)
		memcpy(dp_req->service_specific_info.info,
			   gen_srv_info,
			   gen_srv_info_len);
	dp_req->service_specific_info.length = gen_srv_info_len;
	dp_req->sec_type = info->sec_type;

	if (dp_req->sec_type == NAN_PMK_SET_BY_USER_PMK) {
		if (is_array_empty(info->pmk, NAN_PMK_SIZE)) {
			INFO_PRINT("[rtw_cmd] send datapath req fail! PMK is empty.\n");
			return RTW_RET_STATUS_INVALID_INPUT;
		}
		dp_req->cipher_suite_id = RTW_NAN_CIPHER_ID_NCS_SK_128;
		memcpy(dp_req->key, info->pmk, NAN_PMK_SIZE);
	} else if (dp_req->sec_type == NAN_PMK_SET_BY_PAIRING) {
		dp_req->cipher_suite_id = RTW_NAN_CIPHER_ID_PASN_128;
	} else {
		dp_req->cipher_suite_id = RTW_NAN_CIPHER_ID_OPEN;
	}

	/* Qos */
	dp_req->qos.max_service_internal = 1;
	dp_req->qos.low_latency_required = 1;

	/* send nandow command */
	input = &nandow_cmd;
	input_len = sizeof(struct nan_customer_nandow);
	nandow_send_cmd(&nandow_cmd, input, input_len,
					tmp_file, cmdreply_file, nan_intf, rtw_iw);

	INFO_PRINT("[rtw_cmd] send data path request complete:\n");
	INFO_PRINT("[rtw_cmd] datapath_id = %d\n", dp_req->datapath_id);
	INFO_PRINT("[rtw_cmd] responder_publish_id = %d\n", dp_req->responder_publish_id);
	INFO_PRINT("[rtw_cmd] initiator_subscribe_id = %d\n", dp_req->initiator_subscribe_id);
	PRINT_MAC_ADDR("[rtw_cmd] responder_nan_addr", dp_req->responder_nan_addr.ether_addr_octet);
	INFO_PRINT("[rtw_cmd] NDPE service info (len=%d)", dp_req->service_specific_info.length);
	for (i = 0; i < dp_req->service_specific_info.length; i++) {
		if (!(i % 16)) {
			DEBUG_PRINT("\n");
		}
		DEBUG_PRINT("%02x ", dp_req->service_specific_info.info[i]);
	}
	INFO_PRINT("[rtw_cmd] cipher_suite_id = %d\n", dp_req->cipher_suite_id);
	INFO_PRINT("[rtw_cmd] sec_type = %d\n", dp_req->sec_type);
	INFO_PRINT("\n");

	return RTW_RET_STATUS_SUCCESS;
}

RTW_RET_STATUS rtw_nan_api_send_datapath_rsp(struct datapath_info *info)
{
	struct nan_customer_nandow nandow_cmd;
	struct rtw_nan_datapath_response *dp_rsp;
	uint32_t input_len = 0, i = 0;
	unsigned int cmd_id, no_input = 0xff;
	void *input = NULL;
	uint8_t gen_srv_info[RTW_NAN_SERVICE_INFO_MAX_LENGTH] = {0};
	uint16_t gen_srv_info_len = 0;

	if (!info) {
		INFO_PRINT("[rtw_cmd] send datapath rsp fail! \n");
		return RTW_RET_STATUS_INVALID_INPUT;
	}

	INFO_PRINT("[rtw_cmd] %s \n", __func__);

	memset(&nandow_cmd, 0, sizeof(struct nan_customer_nandow));
	nandow_cmd.cmd_id = RTW_NAN_CMD_DATAPATH_RSP;
	nandow_cmd.cmd_type = NANDOW_CMD_TYPE_SET;
	nandow_pre_actions(&nandow_cmd);

	/* start initialize parameters here */
	dp_rsp = &nandow_cmd.para.datapath_rsp;
	dp_rsp->datapath_id = info->ndp_id;
	dp_rsp->publish_id = info->service->publish_id;
	dp_rsp->status = RTW_NAN_DATAPATH_ACCEPTED;
	memcpy(dp_rsp->initiator_data_address.ether_addr_octet, info->initiator_ndi, ETH_ALEN);
	memcpy(dp_rsp->initiator_management_address.ether_addr_octet, info->initiator_ndi, ETH_ALEN);
	gen_srv_info_len = _construct_ndpe_gen_srvc_info(info, gen_srv_info);
	if (gen_srv_info_len)
		memcpy(dp_rsp->service_specific_info.info, gen_srv_info,
			   gen_srv_info_len);
	dp_rsp->service_specific_info.length = gen_srv_info_len;

	/* sec_type */
	if (NAN_PMK_SET_BY_USER_PMK == info->sec_type) {
		dp_rsp->cipher_suite_id = RTW_NAN_CIPHER_ID_NCS_SK_128;
		memcpy(dp_rsp->key, info->pmk, NAN_PMK_SIZE);
	} else if (NAN_PMK_SET_BY_PAIRING == info->sec_type) {
		dp_rsp->cipher_suite_id = RTW_NAN_CIPHER_ID_PASN_128;
	} else {
		dp_rsp->cipher_suite_id = RTW_NAN_CIPHER_ID_OPEN;
	}

	/* send nandow command */
	input = &nandow_cmd;
	input_len = sizeof(struct nan_customer_nandow);
	nandow_send_cmd(&nandow_cmd, input, input_len,
					tmp_file, cmdreply_file, nan_intf, rtw_iw);

	INFO_PRINT("[rtw_cmd] send data path response complete:\n");
	INFO_PRINT("[rtw_cmd] datapath_id = %d\n", dp_rsp->datapath_id);
	INFO_PRINT("[rtw_cmd] publishId = %d\n", dp_rsp->publish_id);
	INFO_PRINT("[rtw_cmd] status = %d\n", dp_rsp->status);
	PRINT_MAC_ADDR("[rtw_cmd] initiator_data_address", dp_rsp->initiator_data_address.ether_addr_octet);
	PRINT_MAC_ADDR("[rtw_cmd] initiator_management_address", dp_rsp->initiator_management_address.ether_addr_octet);
	INFO_PRINT("[rtw_cmd] NDPE service info (len=%d)", dp_rsp->service_specific_info.length);
	for (i = 0; i < dp_rsp->service_specific_info.length; i++) {
		if (!(i % 16)) {
			DEBUG_PRINT("\n");
		}
		DEBUG_PRINT("%02x ", dp_rsp->service_specific_info.info[i]);
	}
	INFO_PRINT("\n");
	INFO_PRINT("[rtw_cmd] cipher_suite_id = %d\n", dp_rsp->cipher_suite_id);

	return RTW_RET_STATUS_SUCCESS;
}

RTW_RET_STATUS rtw_nan_api_send_datapath_end(uint8_t datapath_id)
{

	struct nan_customer_nandow nandow_cmd;
	struct rtw_nan_datapath_end *ndp_end;
	uint32_t input_len = 0;
	unsigned int cmd_id, no_input = 0xff;
	void *input = NULL;

	INFO_PRINT("[rtw_cmd] %s \n", __func__);

	memset(&nandow_cmd, 0, sizeof(struct rtw_nan_datapath_end));
	nandow_cmd.cmd_id = RTW_NAN_CMD_DATAPATH_END;
	nandow_cmd.cmd_type = NANDOW_CMD_TYPE_SET;
	nandow_pre_actions(&nandow_cmd);

	/* start initialize parameters here */
	ndp_end = &nandow_cmd.para.datapath_end;
	ndp_end->datapath_id = datapath_id;
	ndp_end->reason_code = RTW_NAN_DATAPATH_END_REASON_USER_INITIATED;

	/* send nandow command */
	input = &nandow_cmd;
	input_len = sizeof(struct nan_customer_nandow);
	nandow_send_cmd(&nandow_cmd, input, input_len,
					tmp_file, cmdreply_file, nan_intf, rtw_iw);

	INFO_PRINT("[rtw_cmd] send data path end complete:\n");
	INFO_PRINT("[rtw_cmd] datapath_id = %d\n", ndp_end->datapath_id);

	return RTW_RET_STATUS_SUCCESS;
}
RTW_RET_STATUS rtw_nan_api_send_follow_up(struct tx_followup_info *tx_flwup_data)
{
	struct vendor_tx_followup_info vendor_data = {0};
	uint32_t input_len = 0;
	unsigned int cmd_id = 0;
	void *input = NULL;

	if (!tx_flwup_data) {
		INFO_PRINT("[rtw_cmd] send follow up fail!\n");
		return RTW_RET_STATUS_INVALID_INPUT;
	}

	/* start initialize parameters here */
	vendor_data.remote_inst_id = tx_flwup_data->remote_inst_id;
	vendor_data.local_inst_id = tx_flwup_data->local_inst_id;
	memcpy(vendor_data.remote_mac, tx_flwup_data->remote_mac, ETH_ALEN);

	switch (tx_flwup_data->type) {
	case TX_FOLLOW_UP_BSTRP_REQ:
		vendor_data.follow_up_type = NAN_TX_FOLLOW_UP_BSTRP;
		SET_BSTRAP_TYPE_REQ(vendor_data.bstrap_type_status);
		SET_BSTRAP_STATUS(vendor_data.bstrap_type_status, 0);
		vendor_data.bstrap_method = tx_flwup_data->bstrap_method;
		break;
	case TX_FOLLOW_UP_KEY_EXCHG:
		vendor_data.follow_up_type = NAN_TX_FOLLOW_UP_REQ;
		vendor_data.sec_key_type = tx_flwup_data->sec_key_type;
		break;
	default:
		INFO_PRINT("[rtw_cmd] tx follow up type(%d) not support!\n",
				   tx_flwup_data->type);
		return RTW_RET_STATUS_INVALID_INPUT;
	}

	INFO_PRINT("[rtw_cmd] %s \n", __func__);

	/* send rtw vendor command */
	cmd_id = NAN_SUBCMD_FOLLOWUP;
	input = &vendor_data;
	input_len = sizeof(struct vendor_tx_followup_info);
	send_vendor_cmd(cmd_id, input, input_len, tmp_file, nan_intf, rtw_iw);

	INFO_PRINT("[rtw_cmd] send follow up complete:\n");
	INFO_PRINT("[rtw_cmd] follow_up_type = %d\n", vendor_data.follow_up_type);
	INFO_PRINT("[rtw_cmd] remote_inst_id = %d\n", vendor_data.remote_inst_id);
	INFO_PRINT("[rtw_cmd] local_inst_id = %d\n", vendor_data.local_inst_id);
	PRINT_MAC_ADDR("[rtw_cmd] remote_mac", vendor_data.remote_mac);
	INFO_PRINT("[rtw_cmd] bstrap_type_status = %d\n", vendor_data.bstrap_type_status);
	INFO_PRINT("[rtw_cmd] bstrap_reason = %d\n", vendor_data.bstrap_reason);
	INFO_PRINT("[rtw_cmd] bstrap_method = %d\n", vendor_data.bstrap_method);
	INFO_PRINT("[rtw_cmd] sec_key_type = %d\n", vendor_data.sec_key_type);

	return RTW_RET_STATUS_SUCCESS;
}

RTW_RET_STATUS rtw_nan_api_pasn_setpw(uint8_t *pairing_peer_nmi, char *pw, uint8_t pw_len)
{
	struct vendor_pairing_setpw setpw_data = {0};
	uint32_t input_len = 0;
	unsigned int cmd_id = 0;
	void *input = NULL;

	if (!pw_len || !pw || pw_len > MAX_PASN_PW_LEN) {
		INFO_PRINT("[rtw_cmd] pasn setup pw fail! \n");
		return RTW_RET_STATUS_INVALID_INPUT;
	}

	INFO_PRINT("[rtw_cmd] %s \n", __func__);

	cmd_id = NAN_SUBCMD_PAIRING_SET_PW;
	setpw_data.pw_len = pw_len;
	memcpy(setpw_data.pw, pw, pw_len);
	if (pairing_peer_nmi) {
		memcpy(setpw_data.peer_nmi, pairing_peer_nmi, ETH_ALEN);
	}

	/* send rtw vendor command */
	input = &setpw_data;
	input_len = sizeof(struct vendor_pairing_setpw);
	send_vendor_cmd(cmd_id, input, input_len, tmp_file, nan_intf, rtw_iw);

	return RTW_RET_STATUS_SUCCESS;
}
RTW_RET_STATUS rtw_nan_api_pasn_start(struct pairing_info *pairing_info)
{
	struct vendor_pairing_start vendor_data = {0};
	uint32_t input_len = 0;
	unsigned int cmd_id = 0;
	void *input = NULL;

	if (!pairing_info) {
		INFO_PRINT("[rtw_cmd] pasn start fail! \n");
		return RTW_RET_STATUS_INVALID_INPUT;
	}

	INFO_PRINT("[rtw_cmd] %s \n", __func__);

	vendor_data.role = pairing_info->role;
	vendor_data.type = pairing_info->pasn_type;
	memcpy(vendor_data.peer_nmi, pairing_info->peer_nmi, ETH_ALEN);
	vendor_data.auth_method = pairing_info->pasn_auth_method;
	vendor_data.nik_caching = pairing_info->nik_caching;

	/* send rtw vendor command */
	cmd_id = NAN_SUBCMD_PASN_START;
	input = &vendor_data;
	input_len = sizeof(struct vendor_pairing_start);
	send_vendor_cmd(cmd_id, input, input_len, tmp_file, nan_intf, rtw_iw);

	INFO_PRINT("[rtw_cmd] pasn start:\n");
	INFO_PRINT("[rtw_cmd] role = %d\n", vendor_data.role);
	INFO_PRINT("[rtw_cmd] type = %d\n", vendor_data.type);
	PRINT_MAC_ADDR("[rtw_cmd] peer_nmi", vendor_data.peer_nmi);
	INFO_PRINT("[rtw_cmd] auth_method = %d\n", vendor_data.auth_method);
	INFO_PRINT("[rtw_cmd] nik_caching = %d\n", vendor_data.nik_caching);

	return RTW_RET_STATUS_SUCCESS;
}
