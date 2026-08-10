#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stddef.h>
#include "nan_event.h"
#include "rtw_nan_cmd_api.h"
#include "rtw_nan_vendor_def.h"

#define LOG_FILE_NAME "nan_event.log"
#define OUI_REALTEK_STR "00e04c"

#define PRINT_DBG_ADDR(_str, _addr) \
	DEBUG_PRINT("%s: %02x:%02x:%02x:%02x:%02x:%02x\n", \
	            _str, *(_addr), *((_addr) + 1), *((_addr) + 2), \
	            *((_addr) + 3), *((_addr) + 4), *((_addr) + 5));

extern char *rtw_iw;
rtw_nan_evt_cb g_nan_evt_cb;
static volatile unsigned char g_evt_terminate;
FILE *g_evt_log_file = NULL;
volatile int g_log_file_offset = 0;
pthread_t tid;
pthread_t g_detect_fsize_tid;
static int g_max_event_log_size = MIN_NAN_EVT_LSIZE;

void _print_data(uint8_t *start, uint16_t len)
{
	uint16_t index = 0;
	for (index = 0; index < len; index++) {
		if (!(index % 16)) {
			DEBUG_PRINT("\n");
		}
		DEBUG_PRINT("%02x ", start[index]);
	}
	DEBUG_PRINT("\n");
}

void _print_data_confirm_evt(struct vendor_evt_data_confirm_info *evt_data)
{
	DEBUG_PRINT("[rtw_event] %s:\n", __func__);
	DEBUG_PRINT("[rtw_event] type:%d, status:%d, ndp_id:%d, mc_id:%d, nmsg_id:%d\n",
				evt_data->type, evt_data->status, evt_data->ndp_id,
				evt_data->mc_id, evt_data->nmsg_id);

	PRINT_DBG_ADDR("[rtw_event] initiator_data_address",
				   evt_data->initiator_data_address);
	PRINT_DBG_ADDR("[rtw_event] responder_data_address",
				   evt_data->responder_data_address);

	DEBUG_PRINT("[rtw_event] srvc_info (%d):", evt_data->srvc_info_len);
	_print_data(evt_data->srvc_info, evt_data->srvc_info_len);
}
void _print_disc_result_evt(struct rtw_nan_discovery_result_received_event_data *evt_data)
{
	DEBUG_PRINT("[rtw_event] %s:\n", __func__);
	DEBUG_PRINT("[rtw_event] subscribe_id:%d, publisher_id:%d, publish_rssi:%d\n",
				evt_data->subscribe_id, evt_data->publisher_id, evt_data->publish_rssi);
	PRINT_DBG_ADDR("[rtw_event] peer_nan_address",
				   evt_data->peer_nan_address.ether_addr_octet);
	DEBUG_PRINT("[rtw_event] channel:%d, flags:0x%x\n",
				evt_data->channel.channel, evt_data->channel.flags);
	DEBUG_PRINT("[rtw_event] cipher_suite_id:0x%x\n", evt_data->cipher_suite_id);

	DEBUG_PRINT("[rtw_event] publish_attribute_list (%d):",
				evt_data->publish_attribute_list_length);
	_print_data(evt_data->publish_attribute_list, evt_data->publish_attribute_list_length);
}
void _print_datapath_req_recv_evt(struct rtw_nan_datapath_request_received_event_data *evt_data)
{
	DEBUG_PRINT("[rtw_event] %s:\n", __func__);
	DEBUG_PRINT("[rtw_event] datapath_id:%d, publish_id:%d, security:%d\n",
				evt_data->datapath_id, evt_data->publish_id, evt_data->security);
	PRINT_DBG_ADDR("[rtw_event] initiator_management_address",
				   evt_data->initiator_management_address.ether_addr_octet);
	PRINT_DBG_ADDR("[rtw_event] initiator_data_address",
				   evt_data->initiator_data_address.ether_addr_octet);
	DEBUG_PRINT("[rtw_event] attribute_list (%d):", evt_data->attribute_list_length);
	_print_data(evt_data->attribute_list, evt_data->attribute_list_length);
}
void _print_datapath_rsp_recv_evt(struct rtw_nan_datapath_response_received_event_data *evt_data)
{
	DEBUG_PRINT("[rtw_event] %s:\n", __func__);
	DEBUG_PRINT("[rtw_event] datapath_id:%d, publish_id:%d\n",
				evt_data->datapath_id, evt_data->publish_id);
	PRINT_DBG_ADDR("[rtw_event] initiator_data_address",
				   evt_data->initiator_data_address.ether_addr_octet);
	PRINT_DBG_ADDR("[rtw_event] responder_management_address",
				   evt_data->responder_management_address.ether_addr_octet);
	PRINT_DBG_ADDR("[rtw_event] responder_data_address",
				   evt_data->responder_data_address.ether_addr_octet);
	DEBUG_PRINT("[rtw_event] attribute_list (%d):", evt_data->attribute_list_length);
	_print_data(evt_data->attribute_list, evt_data->attribute_list_length);
}
void _print_datapath_established_evt(struct rtw_nan_datapath_established_event_data *evt_data)
{
	DEBUG_PRINT("[rtw_event] %s:\n", __func__);
	DEBUG_PRINT("[rtw_event] type:%d, datapath_id:%d, pub_id:%d\n",
				evt_data->type, evt_data->datapath_id, evt_data->pub_id);

	PRINT_DBG_ADDR("[rtw_event] initiator_data_address",
				   evt_data->initiator_data_address.ether_addr_octet);
	PRINT_DBG_ADDR("[rtw_event] responder_data_address",
				   evt_data->responder_data_address.ether_addr_octet);
	PRINT_DBG_ADDR("[rtw_event] peer_management_address",
				   evt_data->peer_management_address.ether_addr_octet);

	DEBUG_PRINT("[rtw_event] peer_interface_identifier: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
				evt_data->peer_interface_identifier[0],
				evt_data->peer_interface_identifier[1],
				evt_data->peer_interface_identifier[2],
				evt_data->peer_interface_identifier[3],
				evt_data->peer_interface_identifier[4],
				evt_data->peer_interface_identifier[5],
				evt_data->peer_interface_identifier[6],
				evt_data->peer_interface_identifier[7]);

	DEBUG_PRINT("[rtw_event] service_specific_info(%d):", evt_data->service_specific_info.length);
	_print_data(evt_data->service_specific_info.info, evt_data->service_specific_info.length);
}
void _print_datapath_end_evt(struct rtw_nan_datapath_end_event_data *evt_data)
{
	DEBUG_PRINT("[rtw_event] %s:\n", __func__);
	DEBUG_PRINT("[rtw_event] datapath_id:%d, publish_id:%d, reason:%d\n",
				evt_data->datapath_id, evt_data->publish_id, evt_data->reason);
	PRINT_DBG_ADDR("[rtw_event] initiator_data_address",
				   evt_data->initiator_data_address.ether_addr_octet);
	PRINT_DBG_ADDR("[rtw_event] peer_data_address",
				   evt_data->peer_data_address.ether_addr_octet);
	PRINT_DBG_ADDR("[rtw_event] peer_management_address",
				   evt_data->peer_management_address.ether_addr_octet);
}
void _print_datapth_follow_up_recv_evt(struct rtw_nan_follow_up_received_event_data *evt_data)
{
	DEBUG_PRINT("[rtw_event] %s:\n", __func__);
	DEBUG_PRINT("[rtw_event] local_service_id:%d, peer_service_id:%d\n",
				evt_data->local_service_id, evt_data->peer_service_id);
	PRINT_DBG_ADDR("[rtw_event] peer_nan_address",
				   evt_data->peer_nan_address.ether_addr_octet);
	DEBUG_PRINT("[rtw_event] follow_up_rssi:%d\n",
				evt_data->follow_up_rssi);
	DEBUG_PRINT("[rtw_event] channel:%d, flags:0x%x\n",
				evt_data->channel.channel, evt_data->channel.flags);
	DEBUG_PRINT("[rtw_event] attribute_list (%d):",
				evt_data->attribute_list_length);
	_print_data(evt_data->attribute_list, evt_data->attribute_list_length);
}
void _print_data_indication_evt(struct vendor_evt_data_indication_info *evt_data)
{
	DEBUG_PRINT("[rtw_event] %s:\n", __func__);
	DEBUG_PRINT("[rtw_event] type:%d, publish_id:%d, ndp_id:%d, mc_id:%d, security:%d\n",
				evt_data->type, evt_data->publish_id, evt_data->ndp_id,
				evt_data->mc_id, evt_data->security);

	PRINT_DBG_ADDR("[rtw_event] initiator_data_address", evt_data->initiator_data_address);
	PRINT_DBG_ADDR("[rtw_event] responder_data_address", evt_data->responder_data_address);
	DEBUG_PRINT("[rtw_event] ipv6_interfac_id: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
				evt_data->ipv6_interfac_id[0],
				evt_data->ipv6_interfac_id[1],
				evt_data->ipv6_interfac_id[2],
				evt_data->ipv6_interfac_id[3],
				evt_data->ipv6_interfac_id[4],
				evt_data->ipv6_interfac_id[5],
				evt_data->ipv6_interfac_id[6],
				evt_data->ipv6_interfac_id[7]);
	DEBUG_PRINT("[rtw_event] srvc_info (%d):", evt_data->srvc_info_len);
	_print_data(evt_data->srvc_info, evt_data->srvc_info_len);
}
void _print_data_bstrap_res_evt(struct vendor_evt_bstrap_result *evt_data)
{
	DEBUG_PRINT("[rtw_event] %s:\n", __func__);
	DEBUG_PRINT("[rtw_event] status:%d, \n", evt_data->type_status);
	DEBUG_PRINT("[rtw_event] remote_inst_id:%d, local_inst_id=%d, \n",
				evt_data->remote_inst_id, evt_data->local_inst_id);
	DEBUG_PRINT("[rtw_event] rsp_bstrap_method:%d\n", evt_data->rsp_bstrap_method);
	PRINT_DBG_ADDR("[rtw_event] peer_management_address", evt_data->peer_nmi);
}
void _print_data_pasn_res_evt(struct vendor_evt_pasn_result *evt_data)
{
	DEBUG_PRINT("[rtw_event] %s:\n", __func__);
	DEBUG_PRINT("[rtw_event] status:%d\n", evt_data->status);
	DEBUG_PRINT("[rtw_event] local_role:%d\n", evt_data->local_role);
	DEBUG_PRINT("[rtw_event] remote_nik_caching:%d\n", evt_data->remote_nik_caching);
	PRINT_DBG_ADDR("[rtw_event] peer_management_address", evt_data->peer_nmi);
}
void _print_data_recv_pasn_upd_pmkid(struct vendor_evt_pasn_upd_pmkid *evt_data)
{
	DEBUG_PRINT("[rtw_event] %s:\n", __func__);
	DEBUG_PRINT("[rtw_event] role:%d\n", evt_data->role);
	DEBUG_PRINT("[rtw_event] status:%d\n", evt_data->status);
	PRINT_DBG_ADDR("[rtw_event] peer_management_address", evt_data->peer_nmi);
	DEBUG_PRINT("[rtw_event] auth_method:%d\n", evt_data->auth_method);
}

/**
 * _tlv_get_len
 * @attr_id: attribute id
 * @tlv_buf_len: length of tlv list
 * @tlv_buf: start address of tlv list
 * return: attribute length
 */
uint16_t _tlv_get_len(uint8_t attr_id, uint16_t tlv_list_len, uint8_t *tlv_list)
{
	uint16_t attr_len = 0;
	uint16_t index = 0;

	while ((index + 3) < tlv_list_len) {
		GET_TLV_ATTR_LEN(tlv_list + index, &attr_len);
		if (tlv_list[index] == attr_id) {
			break;
		}
		index += (attr_len + 3);
		attr_len = 0;
	}
	if (!attr_len)
		DEBUG_PRINT("[rtw_event] %s: attr_id(0x%02x) not found!\n",
					__func__, attr_id);
	return attr_len;
}
/**
 * _tlv_get_info
 * @attr_id: attribute id
 * @attr_info: pre-allocated buffer, use the _tlv_get_len to get length info first.
 * @tlv_list_len: length of tlv list
 * @tlv_list: start address of tlv list
 * return: attribute length
 */
uint16_t _tlv_get_info(uint8_t attr_id, uint8_t *attr_info, uint16_t tlv_list_len, uint8_t *tlv_list)
{
	uint16_t attr_len = 0;
	uint16_t index = 0;

	while ((index + 3) < tlv_list_len) {
		GET_TLV_ATTR_LEN(tlv_list + index, &attr_len);
		if (tlv_list[index] == attr_id) {
			if ((index + attr_len + 3) > tlv_list_len) {
				INFO_PRINT("[rtw_event] %s: tlv_list_len(%d) not enough\n",
						   __func__, tlv_list_len);
				attr_len = 0;
			} else {
				memcpy(attr_info, tlv_list + index + 3, attr_len);
			}
			break;
		}
		index += (attr_len + 3);
		attr_len = 0;
	}
	if (!attr_len)
		DEBUG_PRINT("[rtw_event] %s: attr_id(0x%02x) not found!\n",
					__func__, attr_id);
	return attr_len;
}

bool _parse_nan_attr_generic_srvc_proto(uint8_t *tlv_list,
										uint16_t tlv_list_len,
										struct nan_generic_srvc_proto *result)
{
	uint8_t attr_id = 0;
	uint16_t attr_len = 0;
	uint16_t i = 0;

	DEBUG_PRINT("[rtw_event] %s\n", __func__);

	if (!tlv_list_len || !tlv_list || !result) {
		INFO_PRINT("[rtw_event] generic list len(%d), empty!\n", tlv_list_len);
		return false;
	}

	/* transport port */
	attr_id = NAN_GEN_SRVC_TRANSPORT_PORT;
	attr_len = _tlv_get_len(attr_id, tlv_list_len, tlv_list);
	if (attr_len) {
		_tlv_get_info(attr_id, (uint8_t *)&result->port_number, tlv_list_len, tlv_list);
		DEBUG_PRINT("[rtw_event] trans_port(%d)\n", result->port_number);
	}
	/* transport protocol */
	attr_id = NAN_GEN_SRVC_TRANSPORT_PROTOCOL;
	attr_len = _tlv_get_len(attr_id, tlv_list_len, tlv_list);
	if (attr_len) {
		_tlv_get_info(attr_id, &result->trans_proto, tlv_list_len, tlv_list);
		DEBUG_PRINT("[rtw_event] trans_proto(%d)\n", result->trans_proto);
	}
	/* service name */
	attr_id = NAN_GEN_SRVC_SERVICE_NAME;
	attr_len = _tlv_get_len(attr_id, tlv_list_len, tlv_list);
	if (attr_len) {
		memset(result->service_name, '\0', sizeof(result->service_name));
		_tlv_get_info(attr_id, result->service_name, tlv_list_len, tlv_list);
		DEBUG_PRINT("[rtw_event] service_name(%s)\n", result->service_name);
	}
	/* service instance */
	attr_id = NAN_GEN_SRVC_NAME_OF_SRVC_INST;
	attr_len = _tlv_get_len(attr_id, tlv_list_len, tlv_list);
	if (attr_len) {
		memset(result->service_inst, '\0', sizeof(result->service_inst));
		_tlv_get_info(attr_id, result->service_inst, tlv_list_len, tlv_list);
		DEBUG_PRINT("[rtw_event] service_instance(%s)\n", result->service_inst);
	}
	/* text info */
	attr_id = NAN_GEN_SRVC_TEXT_INFO;
	attr_len = _tlv_get_len(attr_id, tlv_list_len, tlv_list);
	if (attr_len >= NAN_MAX_SERVICE_INFO_LEN) {
		INFO_PRINT("[rtw_event] text info len(%d) > max(%d)\n", attr_len, NAN_MAX_SERVICE_INFO_LEN);
		return false;
	}
	if (attr_len) {
		result->text_info_len = attr_len;
		_tlv_get_info(attr_id, result->text_info, tlv_list_len, tlv_list);
		DEBUG_PRINT("[rtw_event] text_info(len=%d) ", result->text_info_len);
		for (i = 0; i < result->text_info_len; i++) {
			DEBUG_PRINT("%02x ", result->text_info[i]);
		}
		DEBUG_PRINT("\n");
	}

	return true;
}

/**
 * service info field format:
 * OUI (3) / service protocol type (1) / service specific info (variable)
 */
bool _parse_nan_attr_srvc_info(uint8_t *info,
							   uint16_t info_len,
							   struct nan_generic_srvc_proto *result)
{
	uint16_t i = 0;
	uint8_t srvc_proto_type = 0;

	DEBUG_PRINT("[rtw_event] %s\n", __func__);

	if (info_len < 4) {
		INFO_PRINT("[rtw_event] parse srvc info, len(%d) not enough\n", info_len);
		return false;
	}

	/* check OUI */
	if (memcmp(info, WFA_OUI, 3)) {
		INFO_PRINT("[rtw_event] OUI(0x%02x-%02x-%02x) not match!\n",
				   info[0], info[1], info[2]);
		return false;
	}

	/* srvc protocol type */
	srvc_proto_type = info[i + 3];
	DEBUG_PRINT("[rtw_event] info_len(%d) srvc_proto_type(%d)\n", info_len, srvc_proto_type);

	/* srvc specific info */
	if (srvc_proto_type == NAN_SRVC_PROTOCOL_GENIC) {
		_parse_nan_attr_generic_srvc_proto(info + 4,
										   info_len - 4,
										   result);
	}

	return true;
}
/**
 * Note for SDEA format: field (size)
 * attr id (1) / length (2) / instance id (1) / control (2) /
 * range limit (0 or 4) / srvc update indicatot (0 or 1)
 * srvc info len (0 or 1) / srvc info (variable)
 */
bool _parse_nan_attr_sdea(uint8_t *attr,
						  uint16_t attr_len,
						  struct nan_attr_sdea *result)
{
	uint8_t inst_id = 0;
	uint16_t ctrl = 0;
	uint16_t i = 0;

	DEBUG_PRINT("[rtw_event] %s\n", __func__);

	/* check for attr_id and min length (instance id + control) */
	if (attr_len < 3 || attr[0] != NAN_ATTR_ID_SRVC_DESC_EXT) {
		INFO_PRINT("[rtw_event] sdea attr_id(%d) or attr_len(%d) not enough!\n",
				   attr[0], attr_len);
		return false;
	}

	/* inst_id and ctrl */
	GET_SDEA_INST_ID(attr, &inst_id);
	GET_SDEA_CTRL(attr, &ctrl);
	i += 6;
	DEBUG_PRINT("[rtw_event] attr_len(%d) inst_id(%d) ctrl(0x%02x)\n",
				attr_len, inst_id, ctrl);
	result->inst_id = inst_id;
	result->ctrl = ctrl;

	/* range limit */
	if (ctrl & 0x0100) {
		if (attr_len < (i + 1)) { /* (attr_len + 3) < (i + 4) */
			INFO_PRINT("[rtw_event] sdea range limit present, attr_len(%d) not enough!\n",
					   attr_len);
			return false;
		}
		DEBUG_PRINT("[rtw_event] sdea range limit present.\n");
		i += 4;
	}
	/* srvc update indicator */
	if (ctrl & 0x0200) {
		if (attr_len < (i - 2)) { /* (attr_len + 3) < (i + 1) */
			INFO_PRINT("[rtw_event] sdea srvc update indicator present, attr_len(%d) not enough!\n",
					   attr_len);
			return false;
		}
		DEBUG_PRINT("[rtw_event] sdea srvc update indicator(%d)\n", attr[i]);
		i += 1;
	}
	/* srvc info */
	if (attr_len > (i - 1)) { /* (attr_len + 3) > (i + 2) */
		uint16_t srvc_info_len = (attr[i + 1] << 8) | attr[i];
		i += 2;
		/* check length */
		if (attr_len < (i + srvc_info_len - 3)) { /* (attr_len + 3) < (i + srvc_info_len) */
			INFO_PRINT("[rtw_event] sdea srvc_info_len(%d), attr_len(%d) not match!\n",
					   srvc_info_len, attr_len);
			return false;
		}
		_parse_nan_attr_srvc_info(attr + i, srvc_info_len, &result->gene_srvc_info);
	}

	return true;
}

/**
 * _parse_nan_attr_sda:
 * @attr: sda start addr
 * @attr_len: sda length
 * @disc_reault:
 *
 * Note for SDA format: field (size)
 * attr id (1) / length (2) / service id (6) / instance id (1) /
 * requestor instance (1) / service control (1) /
 * binding bitmap (0 or 2) /
 * matching filter len (0 or 1) / matching filter (variable) /
 * srvc rsp filter len (0 or 1) / srvc rsp filter (variable) /
 * srvc info len (0 or 1) / srvc info (variable)
 */
bool _parse_nan_attr_sda(uint8_t *attr,
						 uint16_t attr_len,
						 struct nan_attr_sda *result)
{
	uint8_t srvc_id[6] = {0};
	uint8_t inst_id = 0;
	uint8_t req_inst_id = 0;
	uint8_t srvc_ctrl = 0;

	DEBUG_PRINT("[rtw_event] %s\n", __func__);

	/* check for attr_id and min length (srvc id + inst id + requestor inst + srvc ctrl) */
	if (attr_len < 9 || attr[0] != NAN_ATTR_ID_SRVC_DESC) {
		INFO_PRINT("[rtw_event] sda attr_id(%d) or attr_len(%d) not enough\n",
				   attr[0], attr_len);
		return false;
	}

	GET_SDA_SRVC_ID(attr, srvc_id);
	GET_SDA_INST_ID(attr, &inst_id);
	GET_SDA_REQ_INST_ID(attr, &req_inst_id);
	GET_SDA_SRVC_CTRL(attr, &srvc_ctrl);
	DEBUG_PRINT("[rtw_event] attr_len(%d), srvc_id(%02x:%02x:%02x:%02x:%02x:%02x)\n",
				attr_len, srvc_id[0], srvc_id[1], srvc_id[2], srvc_id[3], srvc_id[4], srvc_id[5]);
	DEBUG_PRINT("[rtw_event] inst_id(%d), req_inst_id(%d), srvc_ctrl(0x%02x)\n",
				inst_id, req_inst_id, srvc_ctrl);

	memcpy(result->service_id, srvc_id, SRVC_ID_LEN);
	result->inst_id = inst_id;
	result->req_inst_id = req_inst_id;
	result->service_ctrl = srvc_ctrl;

	return true;
}

void _parse_disc_result_nan_attr(uint8_t *attr_list,
								 uint16_t attr_list_len,
								 struct nan_evt_disc_result_info *disc_result)
{
	uint16_t cur_idx = 0;
	uint8_t attr_id = 0;
	uint16_t attr_len = 0;

	DEBUG_PRINT("[rtw_event] %s\n", __func__);

	while ((cur_idx + 3) < attr_list_len) {
		attr_id = attr_list[cur_idx];
		GET_TLV_ATTR_LEN(attr_list + cur_idx, &attr_len);

		/* check remaining length */
		if ((cur_idx + 3 + attr_len) > attr_list_len) {
			INFO_PRINT("[rtw_event] nan attr_id(0x%02x) attr_len(%d) cur_idx(%d), attr_list_len(%d) not enough\n",
					   attr_id, attr_len, cur_idx, attr_list_len);
			break;
		}

		switch (attr_id) {
		case NAN_ATTR_ID_SRVC_DESC: {
			struct nan_attr_sda sda_result = {0};
			if (_parse_nan_attr_sda(attr_list + cur_idx, attr_len, &sda_result)) {
				memcpy(disc_result->service_id, sda_result.service_id, SRVC_ID_LEN);
			}
		}
		break;
		case NAN_ATTR_ID_SRVC_DESC_EXT: {
			struct nan_attr_sdea sdea_attr = {0};
			if (_parse_nan_attr_sdea(attr_list + cur_idx, attr_len, &sdea_attr)) {
				memcpy(disc_result->service_name, sdea_attr.gene_srvc_info.service_name, MAX_SRVC_NAME_LEN);
				memcpy(disc_result->service_inst, sdea_attr.gene_srvc_info.service_inst, MAX_SRVC_INST_LEN);
				disc_result->port_number = sdea_attr.gene_srvc_info.port_number;
				disc_result->trans_proto = sdea_attr.gene_srvc_info.trans_proto;
				disc_result->text_info_len = sdea_attr.gene_srvc_info.text_info_len;
				memset(disc_result->text_info, 0, sizeof(disc_result->text_info));
				memcpy(disc_result->text_info,
					   sdea_attr.gene_srvc_info.text_info,
					   sdea_attr.gene_srvc_info.text_info_len);
			}
		}
		break;
		default:
			DEBUG_PRINT("[rtw_event] nan attr_id(0x%02x), attr_len(%d) skip\n",
						attr_id, attr_len);
			break;
		}
		cur_idx += (3 + attr_len);
	}
}

void _parse_evt_data(char *evt_data, size_t evt_data_size)
{
	char str[1024] = {0};
	char *pch;
	size_t count = 0;
	int line = 0;

	while (count < evt_data_size && fgets(str, 1024, g_evt_log_file)) {
		// e.g. vendor event: 00 01 01 02 73 3e e5 9d 32 de e9 94 4d 3f c6 02
		pch = strtok(str, ":");
		if (pch != NULL && strncmp(pch, "vendor event", 12) == 0) {
			pch = strtok(NULL, " \n");
			line++;
			while (count < evt_data_size && pch != NULL) {
				DEBUG_PRINT("%s ", pch);
				sscanf(pch, "%2hhx", (unsigned char *)(evt_data + count));
				count++;
				pch = strtok(NULL, " \n");
			}
			DEBUG_PRINT("\n");
		}
	}
	/*DEBUG_PRINT("[rtw_event] %s: line(%d), evt_data_size(%d)\n",
	               __func__, line, evt_data_size);*/
}

void _evt_handler(int evt_id)
{
	fpos_t evt_data_pos;
	void *tmp_data;

	if (evt_id >= RTW_VENDOR_EVT_MAX) {
		INFO_PRINT("[rtw_event] evt id(%d) >= MAX(%d)\n", evt_id, RTW_VENDOR_EVT_MAX);
		return;
	}

	fgetpos(g_evt_log_file, &evt_data_pos);

	switch (evt_id) {
	case RTW_VENDOR_EVT_NAN_DATA_COMFIRM: {
		struct vendor_evt_data_confirm_info evt_data = {0};
		struct nan_evt_data_confirm_info confirm_result = {0};
		struct nan_generic_srvc_proto gene_srvc = {0};

		_parse_evt_data((char *)(&evt_data), sizeof(evt_data));
		_print_data_confirm_evt(&evt_data);

		confirm_result.type = evt_data.type;
		confirm_result.status = evt_data.status;
		confirm_result.ndp_id = evt_data.ndp_id;
		confirm_result.mc_id = evt_data.mc_id;
		confirm_result.nmsg_id = evt_data.nmsg_id;
		memcpy(confirm_result.initiator_data_address, evt_data.initiator_data_address, ETH_ALEN);
		memcpy(confirm_result.responder_data_address, evt_data.responder_data_address, ETH_ALEN);
		if (_parse_nan_attr_srvc_info(evt_data.srvc_info, evt_data.srvc_info_len, &gene_srvc)) {
			confirm_result.port_number = gene_srvc.port_number;
			confirm_result.trans_proto = gene_srvc.trans_proto;
			memset(confirm_result.host_name, '\0', sizeof(confirm_result.host_name));
			memcpy(confirm_result.host_name, gene_srvc.service_inst, MAX_SRVC_INST_LEN);
		}

		if (g_nan_evt_cb) {
			g_nan_evt_cb(NAN_EVT_DATA_CONFIRM, (void *)(&confirm_result), sizeof(confirm_result));
		}
	}
	break;
	case RTW_VENDOR_EVT_NAN_CLUSTER_CHANGED: {
		struct rtw_nan_cluster_changed_event_data evt_data = {0};
		size_t size = sizeof(struct rtw_nan_cluster_changed_event_data);
		struct nan_evt_cluster_changed_info cluster_info = {0};

		_parse_evt_data((char *)(&evt_data), sizeof(evt_data));
		PRINT_DBG_ADDR("[rtw_event] cluster_id", cluster_info.cluster_id);

		memcpy(cluster_info.cluster_id, evt_data.cluster_id.ether_addr_octet, ETH_ALEN);
		if (g_nan_evt_cb) {
			g_nan_evt_cb(NAN_EVT_CLUSTER_CHANGE, (void *)(&cluster_info), sizeof(cluster_info));
		}
	}
	break;
	case RTW_VENDOR_EVT_NAN_DISC_RESULT_RECEIVED: {
		struct rtw_nan_discovery_result_received_event_data *p_evt_data = NULL;
		size_t size = sizeof(struct rtw_nan_discovery_result_received_event_data);
		struct nan_evt_disc_result_info disc_result = {0};

		/* note: free the memory at the end of case */
		p_evt_data = (struct rtw_nan_discovery_result_received_event_data *)calloc(1, size);

		/* get the attr len */
		_parse_evt_data((char *)p_evt_data, size);
		/* realloc memory for attr list */
		if (p_evt_data->publish_attribute_list_length) {
			size = offsetof(struct rtw_nan_discovery_result_received_event_data, publish_attribute_list[0]) +
				   p_evt_data->publish_attribute_list_length;
			fsetpos(g_evt_log_file, &evt_data_pos);
			tmp_data = realloc(p_evt_data, size);
			if (tmp_data == NULL) {
				free(p_evt_data);
				p_evt_data = NULL;
				break;
			}
			p_evt_data = (struct rtw_nan_discovery_result_received_event_data *)tmp_data;
			memset(p_evt_data, 0, size);
			_parse_evt_data((char *)p_evt_data, size);
		}

		/* print original event data */
		_print_disc_result_evt(p_evt_data);

		/* convert to struct nan_evt_disc_result_info */
		disc_result.subscirbe_id = p_evt_data->subscribe_id;
		disc_result.publish_id = p_evt_data->publisher_id;
		memcpy(disc_result.peer_nmi, p_evt_data->peer_nan_address.ether_addr_octet, ETH_ALEN);
		disc_result.peer_rssi = p_evt_data->publish_rssi;
		disc_result.channel = p_evt_data->channel.channel;
		if (p_evt_data->cipher_suite_id & BIT(NAN_CIPHER_SUITE_BIT_NCS_SK_128)) {
			disc_result.pmk_set = true;
		}
		if (p_evt_data->cipher_suite_id & BIT(NAN_CIPHER_SUITE_BIT_NCS_PASN_128)) {
			disc_result.pairing_enable = true;
		}
		disc_result.nira_paired = p_evt_data->nira_paired;

		// csid should be parsed from the nan attr
		_parse_disc_result_nan_attr(p_evt_data->publish_attribute_list,
									p_evt_data->publish_attribute_list_length,
									&disc_result);

		if (g_nan_evt_cb) {
			g_nan_evt_cb(NAN_EVT_DISCOVERY_RESULT, (void *)&disc_result, sizeof(disc_result));
		}

		free(p_evt_data);
	}
	break;
	case RTW_VENDOR_EVT_NAN_DATAPATH_REQ_RECEIVED: {
		struct rtw_nan_datapath_request_received_event_data *p_evt_data = NULL;
		size_t size = sizeof(struct rtw_nan_datapath_request_received_event_data);

		/* note: free the memory at the end of case */
		p_evt_data = (struct rtw_nan_datapath_request_received_event_data *)calloc(1, size);

		/* get the attr len */
		_parse_evt_data((char *)p_evt_data, size);
		/* realloc memory for attr list */
		if (p_evt_data->attribute_list_length) {
			size = offsetof(struct rtw_nan_datapath_request_received_event_data, attribute_list[0]) +
				   p_evt_data->attribute_list_length;
			fsetpos(g_evt_log_file, &evt_data_pos);
			tmp_data = realloc(p_evt_data, size);
			if (tmp_data == NULL) {
				free(p_evt_data);
				p_evt_data = NULL;
				break;
			}
			p_evt_data = (struct rtw_nan_datapath_request_received_event_data *)tmp_data;
			memset(p_evt_data, 0, size);
			_parse_evt_data((char *)p_evt_data, size);
		}

		_print_datapath_req_recv_evt(p_evt_data);

		if (g_nan_evt_cb) {
			g_nan_evt_cb(NAN_EVT_DATAPATH_RECV_REQ, (void *)p_evt_data, size);
		}

		free(p_evt_data);
	}
	break;
	case RTW_VENDOR_EVT_NAN_DATAPATH_RSP_RECEIVED: {
		struct rtw_nan_datapath_response_received_event_data *p_evt_data = NULL;
		size_t size = sizeof(struct rtw_nan_datapath_response_received_event_data);

		/* note: free the memory at the end of case */
		p_evt_data = (struct rtw_nan_datapath_response_received_event_data *)calloc(1, size);

		/* get the attr len */
		_parse_evt_data((char *)p_evt_data, size);
		/* realloc memory for attr list */
		if (p_evt_data->attribute_list_length) {
			size = offsetof(struct rtw_nan_datapath_response_received_event_data, attribute_list[0]) +
				   p_evt_data->attribute_list_length;
			fsetpos(g_evt_log_file, &evt_data_pos);
			tmp_data = realloc(p_evt_data, size);
			if (tmp_data == NULL) {
				free(p_evt_data);
				p_evt_data = NULL;
				break;
			}
			p_evt_data = (struct rtw_nan_datapath_response_received_event_data *)tmp_data;
			memset(p_evt_data, 0, size);
			_parse_evt_data((char *)p_evt_data, size);
		}

		_print_datapath_rsp_recv_evt(p_evt_data);

		if (g_nan_evt_cb) {
			g_nan_evt_cb(NAN_EVT_DATAPATH_RECV_RSP, (void *)p_evt_data, size);
		}

		free(p_evt_data);
	}
	break;
	case RTW_VENDOR_EVT_NAN_DATAPATH_ESTABLISHED: {
		struct rtw_nan_datapath_established_event_data evt_data = {0};
		struct nan_evt_data_established_info info = {0};

		_parse_evt_data((char *)(&evt_data), sizeof(evt_data));
		_print_datapath_established_evt(&evt_data);

		info.publish_id = evt_data.pub_id;
		info.type = evt_data.type;
		info.ndp_id = evt_data.datapath_id;
		memcpy(info.responder_data_address, evt_data.responder_data_address.ether_addr_octet, ETH_ALEN);

		if (g_nan_evt_cb) {
			g_nan_evt_cb(NAN_EVT_DATAPATH_ESTABLISHED, (void *)(&info), sizeof(info));
		}
	}
	break;
	case RTW_VENDOR_EVT_NAN_DATAPATH_END: {
		struct rtw_nan_datapath_end_event_data evt_data = {0};
		struct nan_evt_data_end_info info = {0};

		_parse_evt_data((char *)(&evt_data), sizeof(evt_data));

		info.ndp_id = evt_data.datapath_id;
		info.publish_id = evt_data.publish_id;
		info.reason = evt_data.reason;

		if (g_nan_evt_cb) {
			g_nan_evt_cb(NAN_EVT_DATAPATH_END, (void *)(&info), sizeof(info));
		}
	}
	break;
	case RTW_VENDOR_EVT_NAN_FOLLOW_UP_RECEIVED: {
		struct rtw_nan_follow_up_received_event_data *p_evt_data = NULL;
		size_t size = sizeof(struct rtw_nan_follow_up_received_event_data);

		/* note: free the memory at the end of case */
		p_evt_data = (struct rtw_nan_follow_up_received_event_data *)calloc(1, size);

		/* get the attr len */
		_parse_evt_data((char *)p_evt_data, size);
		/* realloc memory for attr list */
		if (p_evt_data->attribute_list_length) {
			size = offsetof(struct rtw_nan_follow_up_received_event_data, attribute_list[0]) +
				   p_evt_data->attribute_list_length;
			fsetpos(g_evt_log_file, &evt_data_pos);
			tmp_data = realloc(p_evt_data, size);
			if (tmp_data == NULL) {
				free(p_evt_data);
				p_evt_data = NULL;
				break;
			}
			p_evt_data = (struct rtw_nan_follow_up_received_event_data *)tmp_data;
			memset(p_evt_data, 0, size);
			_parse_evt_data((char *)p_evt_data, size);
		}

		_print_datapth_follow_up_recv_evt(p_evt_data);

		if (g_nan_evt_cb) {
			g_nan_evt_cb(NAN_EVT_DATAPATH_RECV_FOLLOW_UP, (void *)p_evt_data, size);
		}

		free(p_evt_data);
	}
	break;
	case RTW_VENDOR_EVT_NAN_DATA_INDICATION: {
		struct vendor_evt_data_indication_info evt_data = {0};
		struct nan_evt_data_indication_info indic_result = {0};
		_parse_evt_data((char *)(&evt_data), sizeof(evt_data));
		_print_data_indication_evt(&evt_data);

		indic_result.type = evt_data.type;
		indic_result.publish_id = evt_data.publish_id;
		indic_result.ndp_id = evt_data.ndp_id;
		indic_result.mc_id = evt_data.mc_id;
		memcpy(indic_result.initiator_data_address, evt_data.initiator_data_address, ETH_ALEN);
		memcpy(indic_result.responder_data_address, evt_data.responder_data_address, ETH_ALEN);
		indic_result.security = evt_data.security;
		memcpy(indic_result.ipv6_interfac_id, evt_data.ipv6_interfac_id, MAX_IPV6_INFO_LEN);

		if (g_nan_evt_cb) {
			g_nan_evt_cb(NAN_EVT_DATA_INDICATION, (void *)(&indic_result), sizeof(indic_result));
		}
	}
	break;
	case RTW_VENDOR_EVT_NAN_PASN_START:
	case RTW_VENDOR_EVT_NAN_PASN_RX:
		break;
	case RTW_VENDOR_EVT_NAN_BSTRAP_RESULT: {
		struct vendor_evt_bstrap_result evt_data = {0};
		struct nan_evt_bstrap_result bstrap_result = {0};

		_parse_evt_data((char *)(&evt_data), sizeof(evt_data));
		_print_data_bstrap_res_evt(&evt_data);

		if (IS_BSTRAP_TYPE_REQ(evt_data.type_status)) {
			bstrap_result.local_role = PAIRING_ROLE_INITIATOR;
		} else {
			bstrap_result.local_role = PAIRING_ROLE_RESPONDER;
		}
		bstrap_result.status = GET_BSTRAP_STATUS(evt_data.type_status);
		bstrap_result.remote_inst_id = evt_data.remote_inst_id;
		bstrap_result.local_inst_id = evt_data.local_inst_id;
		bstrap_result.bstrap_method = evt_data.rsp_bstrap_method;
		memcpy(bstrap_result.peer_nmi, evt_data.peer_nmi, ETH_ALEN);

		if (g_nan_evt_cb) {
			g_nan_evt_cb(NAN_EVT_BSTRAP_RESULT, (void *)(&bstrap_result), sizeof(bstrap_result));
		}
	}
	break;
	case RTW_VENDOR_EVT_NAN_PASN_RESULT: {
		struct vendor_evt_pasn_result evt_data = {0};
		struct nan_evt_pasn_result pasn_result = {0};
		_parse_evt_data((char *)(&evt_data), sizeof(evt_data));
		_print_data_pasn_res_evt(&evt_data);

		pasn_result.status = evt_data.status;
		pasn_result.local_role = evt_data.local_role;
		pasn_result.pairing_type = evt_data.pairing_type;
		pasn_result.remote_nik_caching = evt_data.remote_nik_caching;
		memcpy(pasn_result.peer_nmi, evt_data.peer_nmi, ETH_ALEN);

		if (g_nan_evt_cb) {
			g_nan_evt_cb(NAN_EVT_PASN_RESULT, (void *)(&pasn_result), sizeof(pasn_result));
		}
	}
	break;
	case RTW_VENDOR_EVT_NAN_PASN_UPD_PMKID: {
		struct vendor_evt_pasn_upd_pmkid evt_data = {0};
		_parse_evt_data((char *)(&evt_data), sizeof(evt_data));
		_print_data_recv_pasn_upd_pmkid(&evt_data);
		/* tmp remove as update pmkid evt is not used currently. */
#if 0
		struct nan_evt_data_recv_shared_key shared_key = {0};

		shared_key.role = evt_data.role;
		shared_key.status = evt_data.status;
		memcpy(shared_key.peer_nmi, evt_data.peer_nmi, ETH_ALEN);

		if (g_nan_evt_cb) {
			g_nan_evt_cb(NAN_EVT_DATA_SHARED_KEY, (void *)(&shared_key), sizeof(shared_key));
		}
#endif
	}
	break;
	default:
		DEBUG_PRINT("[rtw_event] evt id(%d) not handle.\n", evt_id);
		break;
	}
}

void _evt_parser()
{
	char str[1024] = {0};
	char *pch;

	fseek(g_evt_log_file, g_log_file_offset, SEEK_SET);
	while (fgets(str, 1024, g_evt_log_file)) {
		// e.g. 1659517346.505874: nan0 (phy #0): vendor event 00e04c:7
		pch = strtok(str, " :");
		while (pch != NULL) {
			if (strncmp(pch, OUI_REALTEK_STR, 6) == 0) {
				pch = strtok(NULL, " :\n");
				DEBUG_PRINT("---------------------------------------\n");
				DEBUG_PRINT("found event: %s\n", pch);
				_evt_handler(atoi(pch));
				DEBUG_PRINT("---------------------------------------\n");
			}
			pch = strtok(NULL, " :");
		}
	}
	g_log_file_offset = ftell(g_evt_log_file);
}

void *_wait_evt_thread(void *arg)
{
	if (g_evt_log_file == NULL) {
		g_evt_log_file = fopen(LOG_FILE_NAME, "r");
		if (g_evt_log_file == NULL) {
			INFO_PRINT("[rtw_event] error: log file can't be opened!\n");
			return (void *)1;
		}
	}

	while (!g_evt_terminate) {
		_evt_parser();
		sleep(1);
	}

	return (void *)0;
}

void *_detect_fsize_thread(void *arg)
{
	while (!g_evt_terminate) {
		if (g_log_file_offset > g_max_event_log_size) {
			rtw_nan_evt_reinit();
		}
	}
	return (void *)0;
}

int rtw_nan_evt_init()
{
	int err = 0;
	char cmd[256] = {0};
	INFO_PRINT("[rtw_event] %s \n", __func__);

	if (g_evt_log_file != NULL) {
		INFO_PRINT(" %s g_evt_log_file is opened\n", __func__);
		return true;
	}

	/* 1. init global var */
	g_evt_terminate = 0;

	/* 2. start iw event */
	sprintf(cmd, "%s event -f > %s &", rtw_iw, LOG_FILE_NAME);
	system(cmd);
	sleep(1);

	/* 3. open nan_event_log */
	g_evt_log_file = fopen(LOG_FILE_NAME, "r");
	if (g_evt_log_file == NULL) {
		INFO_PRINT("[rtw_event] %s can't be opened, retry later.\n", LOG_FILE_NAME);
		return false;
	}

	g_log_file_offset = 0;
	/* 4. start thread */
	err = pthread_create(&tid, NULL, &_wait_evt_thread, NULL);
	if (err) {
		INFO_PRINT("[rtw_event] error: thread create, %s\n", strerror(err));
		return false;
	}

	err = pthread_create(&g_detect_fsize_tid, NULL, &_detect_fsize_thread, NULL);
	if (err) {
		INFO_PRINT("[rtw_event] error: g_detect_fsize_tid thread create, %s\n", strerror(err));
		return false;
	}

	INFO_PRINT("[rtw_event] init complete.\n");
	return true;
}
void rtw_nan_evt_reinit()
{
	void *ret = NULL;
	int err = 0;
	char cmd[256] = {0};

	DEBUG_PRINT("[rtw_event] %s \n", __func__);

	system("killall iw");

	/* terminate thread */
	g_evt_terminate = 1;
	err = pthread_join(tid, &ret);
	if (err) {
		INFO_PRINT("[rtw_event] error: thread join, %s\n", strerror(err));
		return;
	}

	if (g_evt_log_file) {
		fclose(g_evt_log_file);
	}

	g_evt_terminate = 0;
	g_log_file_offset = 0;

	sprintf(cmd, "rm %s", LOG_FILE_NAME);
	system(cmd);
	sleep(1);

	sprintf(cmd, "%s event -f > %s &", rtw_iw, LOG_FILE_NAME);
	system(cmd);
	sleep(1);

	g_evt_log_file = fopen(LOG_FILE_NAME, "r");
	if (g_evt_log_file == NULL) {
		INFO_PRINT("[rtw_event] %s can't be opened, retry later.\n", LOG_FILE_NAME);
		return;
	}

	g_log_file_offset = 0;
	err = pthread_create(&tid, NULL, &_wait_evt_thread, NULL);
	if (err) {
		INFO_PRINT("[rtw_event] error: thread create, %s\n", strerror(err));
		return;
	}

	DEBUG_PRINT("[rtw_event] %s complete.\n", __func__);
}

void rtw_nan_evt_deinit()
{
	void *ret = NULL;
	int err = 0;

	/* 1. terminate iw */
	system("killall iw");

	/* 2. terminate thread */
	g_evt_terminate = 1;
	err = pthread_join(g_detect_fsize_tid, &ret);
	if (err) {
		INFO_PRINT("[rtw_event] error: g_detect_fsize_tid thread join, %s\n", strerror(err));
	}

	err = pthread_join(tid, &ret);
	if (err) {
		INFO_PRINT("[rtw_event] error: thread join, %s\n", strerror(err));
	}

	/* 3. close log file */
	if (g_evt_log_file) {
		fclose(g_evt_log_file);
	}

	g_evt_log_file = NULL;
	INFO_PRINT("[rtw_event] deinit complete.\n");
}

void rtw_nan_evt_send(uint8_t id, void *buf, uint32_t len)
{
	if (id >= NAN_EVT_MAX) {
		INFO_PRINT("[rtw_event] error: send evt id(%d)\n", id);
		return;
	}
	DEBUG_PRINT("[rtw_event] send evt id(%d), len(%d)\n", id, len);
	if (g_nan_evt_cb) {
		g_nan_evt_cb(id, buf, len);
	}
}

void rtw_nan_api_register_evt_cb(rtw_nan_evt_cb cb)
{
	g_nan_evt_cb = cb;
}
