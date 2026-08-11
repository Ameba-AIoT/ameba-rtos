#ifndef rtw_nan_event_h__
#define rtw_nan_event_h__

#include <stdint.h>
#include "rtw_nan_struct.h"

#define NAN_ATTR_ID_SRVC_DESC 0x03
#define NAN_ATTR_ID_SRVC_DESC_EXT 0x0e

#define GET_TLV_ATTR_LEN(_tlv_attr, _out) \
	(*(_out) = (*((_tlv_attr) + 2) << 8) | *((_tlv_attr) + 1))

#define GET_SDA_SRVC_ID(_sda_attr, _out) \
	(memcpy((_out), ((_sda_attr) + 3), 6))
#define GET_SDA_INST_ID(_sda_attr, _out) \
	(*(_out) = *((_sda_attr) + 9))
#define GET_SDA_REQ_INST_ID(_sda_attr, _out) \
	(*(_out) = *((_sda_attr) + 10))
#define GET_SDA_SRVC_CTRL(_sda_attr, _out) \
	(*(_out) = *((_sda_attr) + 11))

#define GET_SDEA_INST_ID(_sdea_attr, _out) \
	(*(_out) = *((_sdea_attr) + 3))
#define GET_SDEA_CTRL(_sdea_attr, _out) \
	(*(_out) = (*((_sdea_attr) + 5) << 8) | *((_sdea_attr) + 4))

#define GET_BSTRAP_STATUS(_type_stats) ((_type_stats) >> 4)
#define IS_BSTRAP_TYPE_REQ(_type_stats) (1 == ((_type_stats) & 0x0F))
#define IS_BSTRAP_COMEBACK_REQ(_type_stats) \
        (IS_BSTRAP_TYPE_REQ(_type_stats) && (2 == ((_type_stats) >> 4)))


/* notice: should be the same as the enum value of rtw_vendor_event in driver */
enum rtw_vendor_event {
	RTW_VENDOR_EVT_NAN_DATA_COMFIRM,
	RTW_VENDOR_EVT_NAN_CLUSTER_CHANGED = 1,
	RTW_VENDOR_EVT_NAN_DISC_RESULT_RECEIVED,
	RTW_VENDOR_EVT_NAN_PUB_REPLIED_PRIVATE,
	RTW_VENDOR_EVT_NAN_DATAPATH_REQ_RECEIVED,
	RTW_VENDOR_EVT_NAN_DATAPATH_RSP_RECEIVED,
	RTW_VENDOR_EVT_NAN_DATAPATH_CONFIRM_RECEIVED,
	RTW_VENDOR_EVT_NAN_DATAPATH_ESTABLISHED,
	RTW_VENDOR_EVT_NAN_DATAPATH_END,
	RTW_VENDOR_EVT_NAN_DATAPATH_HOST_ASSIST_REQ,
	RTW_VENDOR_EVT_NAN_SUB_TERMINATED,
	RTW_VENDOR_EVT_NAN_PUB_TERMINATED,
	RTW_VENDOR_EVT_NAN_FOLLOW_UP_RECEIVED,
	RTW_VENDOR_EVT_NAN_FOLLOW_UP_TX_COMPLETE,
	RTW_VENDOR_EVT_NAN_BEACON,
	RTW_VENDOR_EVT_NAN_OOB_ACTION_FRAME_TX_STATUS,
	RTW_VENDOR_EVT_NAN_OOB_ACTION_FRAME_RX,
	RTW_VENDOR_EVT_RESET_INTERFACE,
	RTW_VENDOR_EVT_COUNTRY_CODE_CHANGED,
	RTW_VENDOR_EVT_INFRA_ASSOC_START,
	RTW_VENDOR_EVT_ASSOC_DONE,
	RTW_VENDOR_EVT_ASSOC_READY,
	RTW_VENDOR_EVT_AUTH,
	RTW_VENDOR_EVT_ASSOC,
	RTW_VENDOR_EVT_INFRA_SCAN_START,
	RTW_VENDOR_EVT_INFRA_SCAN_COMPLETE,
	RTW_VENDOR_EVT_SET_INFRA_CHANNEL,
	RTW_VENDOR_EVT_NAN_DATA_TERMINATION,
	RTW_VENDOR_EVT_NAN_DATA_INDICATION,
	RTW_VENDOR_EVT_NAN_PASN_START,
	RTW_VENDOR_EVT_NAN_PASN_RX,
	RTW_VENDOR_EVT_NAN_PASN_UPD_PMKID,
	RTW_VENDOR_EVT_NAN_BSTRAP_RESULT,
	RTW_VENDOR_EVT_NAN_PASN_RESULT,
	RTW_VENDOR_EVT_MAX
};

struct nan_generic_srvc_proto {
	uint16_t port_number;
	uint8_t trans_proto;
	uint8_t service_name[MAX_SRVC_NAME_LEN];
	uint8_t service_inst[MAX_SRVC_INST_LEN];
	uint8_t text_info[NAN_MAX_SERVICE_INFO_LEN];
	uint16_t text_info_len;
};
struct nan_attr_sda {
	uint8_t service_id[SRVC_ID_LEN];
	uint8_t inst_id;
	uint8_t req_inst_id;
	uint8_t service_ctrl;
};
struct nan_attr_sdea {
	uint8_t inst_id;
	uint8_t ctrl;
	struct nan_generic_srvc_proto gene_srvc_info;
};

/**
 * notice:
 * should be the same as the definition of struct nan_event_data_indication in driver
 */
struct vendor_evt_data_indication_info {
	uint8_t type; /* 0 = ucast */
	uint8_t publish_id;
	uint8_t ndp_id; /* ucast */
	uint8_t mc_id; /* mcast */
	uint8_t initiator_data_address[ETH_ALEN]; /* ucast */
	uint8_t responder_data_address[ETH_ALEN]; /* ucast */
	uint8_t security;
	uint8_t ipv6_interfac_id[MAX_IPV6_INFO_LEN];
	uint8_t srvc_info[NAN_MAX_SERVICE_INFO_LEN];
	uint8_t srvc_info_len;
};
/**
 * notice:
 * should be the same as the definition of struct nan_event_data_comfirm in driver
 */
struct vendor_evt_data_confirm_info {
	uint8_t type; /* 0 = ucast */
	uint8_t status;
	uint8_t ndp_id; /* ucast */
	uint8_t mc_id; /* mcast */
	uint8_t nmsg_id; /* mcast */
	uint8_t initiator_data_address[ETH_ALEN]; /* ucast */
	uint8_t responder_data_address[ETH_ALEN]; /* ucast */
	uint8_t srvc_info[NAN_MAX_SERVICE_INFO_LEN];
	uint8_t srvc_info_len;
};

/**
 * notice:
 * should be the same as the definition of struct nan_event_bstrap_result in driver
 */
struct vendor_evt_bstrap_result {
	uint8_t type_status;
	uint8_t remote_inst_id;
	uint8_t local_inst_id;
	uint16_t rsp_bstrap_method;
	uint8_t peer_nmi[ETH_ALEN];
};

/**
 * notice:
 * should be the same as the definition of struct nan_event_pasn_result in driver
 */
struct vendor_evt_pasn_result {
	uint8_t status;
	uint8_t local_role;
	uint8_t pairing_type;
	uint8_t remote_nik_caching;
	uint8_t peer_nmi[ETH_ALEN];
};

/**
 * notice:
 * should be the same as the definition of struct nan_event_pasn_upd_pmkid in driver
 */
struct vendor_evt_pasn_upd_pmkid {
	uint8_t role;
	uint8_t status;
	uint8_t peer_nmi[ETH_ALEN];
	uint8_t peer_pmkid[MAX_PMKID_LEN];
	uint8_t auth_method;
};

int rtw_nan_evt_init();
void rtw_nan_evt_reinit();
void rtw_nan_evt_deinit();
void rtw_nan_evt_send(uint8_t id, void *buf, uint32_t len);

#endif