#ifndef rtw_dns_mgnt_h__
#define rtw_dns_mgnt_h__

#include "rtw_nan_struct.h"

enum dns_event_id {
	DNS_DISCOVERY_RESULT,
	DNS_DATA_INDICATION,
	DNS_DATA_CONFIRM,
	DNS_DATA_ESTABLISHED,
	DNS_BSTRAP_RESULT,
	DNS_PASN_RESULT,
	DNS_DATA_NIK,
	DNS_DATA_SHARED_KEY,
	DNS_DATA_END,
};
typedef void (*rtw_dns_evt_cb)(uint8_t event_id, void *buf, uint32_t len);

enum rtw_aware_role {
	RTW_AWARE_ROLE_PUBLISH = 0,
	RTW_AWARE_ROLE_SUBSCRIBE = 1,
	RTW_AWARE_ROLE_UNKNOWN,
};

struct dns_evt_disc_result_info {
	uint8_t subscribe_id;
	uint8_t publisher_id;
	uint8_t peer_nmi[ETH_ALEN];
	uint8_t service_id[SRVC_ID_LEN];
	uint8_t service_name[MAX_SRVC_NAME_LEN];
	uint8_t service_instance[MAX_SRVC_INST_LEN];
	uint8_t text_info[NAN_MAX_SERVICE_INFO_LEN];
	uint16_t text_info_len;
	uint8_t pmk_set;
	uint8_t pairing_enable; /// use flag?
	uint8_t paired; // use flag?
};

struct dns_evt_data_indic_info {
	uint8_t type; /* 0 = ucast */
	uint8_t publish_id;
	uint8_t ndp_id; /* ucast */
	uint8_t mc_id; /* mcast */
	uint8_t initiator_ndi[ETH_ALEN];
	uint8_t responder_ndi[ETH_ALEN];
	uint8_t security;
	uint8_t ipv6_intf_identifier[MAX_IPV6_INFO_LEN];
};

struct dns_evt_data_established_info {
	uint8_t  type;
	uint8_t publish_id;
	uint8_t ndp_id;
	uint8_t responder_data_address[ETH_ALEN]; /* ucast */
};
struct dns_evt_data_end_info {
	uint8_t reason;
	uint8_t publish_id;
	uint8_t ndp_id;
};
struct dns_evt_data_comfirm_info {
	uint8_t type; /* 0 = ucast */
	uint8_t status;
	uint8_t ndp_id; /* ucast */
	uint8_t mc_id; /* mcast */
	uint8_t nmsg_id; /* mcast */
	uint8_t initiator_ndi[ETH_ALEN]; /* ucast */
	uint8_t responder_ndi[ETH_ALEN]; /* ucast */
	uint8_t host_name[256];
	uint16_t port_number;
	uint8_t ipv6_intf_identifier[MAX_IPV6_INFO_LEN];
};
struct dns_evt_data_nik_result {
	uint8_t role;
	uint8_t status;
	uint8_t peer_nmi[ETH_ALEN];
	uint8_t auth_method;
};
struct dns_evt_bstrap_result {
	uint8_t status;
	uint8_t remote_inst_id;
	uint8_t local_inst_id;
	uint8_t local_role;
	uint16_t bstrap_method;
	uint8_t peer_nmi[ETH_ALEN];
};
struct dns_evt_pasn_result {
	uint8_t status;
	uint8_t local_role;
	uint8_t pairing_type;
	uint8_t remote_nik_caching;
	uint8_t peer_nmi[ETH_ALEN];
};
struct dns_evt_data_shared_key_result {
	uint8_t role;
	uint8_t status;
	uint8_t peer_nmi[ETH_ALEN];
};

extern RTW_RET_STATUS dns_stop_service(uint16_t phy_num);
extern RTW_RET_STATUS dns_publish(uint16_t phy_num,
								  struct utility_bin_path *bin_path,
								  struct srvc_info *info,
								  uint8_t is_bt_trigger);
extern RTW_RET_STATUS dns_subscribe(uint16_t phy_num,
									struct utility_bin_path *bin_path,
									struct srvc_info *info,
									uint8_t is_bt_trigger);
extern RTW_RET_STATUS dns_publish_update(struct srvc_info *info);
extern RTW_RET_STATUS dns_datapath_req(struct datapath_info *info);
extern RTW_RET_STATUS dns_datapath_rsp(struct datapath_info *info);
extern RTW_RET_STATUS dns_datapath_end(uint8_t datapath_id);
extern RTW_RET_STATUS dns_bstrap_start(struct srvc_info *info);
extern RTW_RET_STATUS dns_pairing_start(struct pairing_info *pairing_info);
extern RTW_RET_STATUS dns_pairing_setpw(uint8_t *pairing_peer_nmi, char *pw, uint8_t pw_len);

extern void dns_register_evt_cb(rtw_dns_evt_cb cb);

#endif