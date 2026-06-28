#ifndef rtw_nan_struct_h__
#define rtw_nan_struct_h__

#include <stdint.h>

static unsigned char WFA_OUI[] = {0x50, 0x6F, 0x9A};

#define MAX_SRVC_NAME_LEN 256
#define MAX_SRVC_INST_LEN 64
#define SRVC_ID_LEN 6
#define NAN_MAX_SERVICE_INFO_LEN 255
#define NAN_MAX_HOSTNAME_LEN 255
#define ETH_ALEN 6
#define MAX_IPV6_INFO_LEN 8
#define MAX_BIN_PATH_LEN 256
#define MAX_SERVICE_TIME 30
#define NAN_PMK_SIZE 32
#define MIN_NAN_EVT_LSIZE 10485760 /* 10MB */
#define MAX_PASN_PW_LEN 16
#define MAX_PMKID_LEN 16

#ifndef BIT
#define BIT(x) (1 << (x))
#endif

#define PRINT_IPV6(_str, _addr) \
	printf("%s: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n", \
	       _str, *(_addr), *((_addr) + 1), *((_addr) + 2), *((_addr) + 3), \
	        *((_addr) + 4), *((_addr) + 5),  *((_addr) + 6), *((_addr) + 7));
#define PRINT_MAC_ADDR(_str, _addr) \
	printf("%s: %02x:%02x:%02x:%02x:%02x:%02x\n", \
	        _str, *(_addr), *((_addr) + 1), *((_addr) + 2), \
	        *((_addr) + 3), *((_addr) + 4), *((_addr) + 5));

typedef enum rtw_ret_status {
	RTW_RET_STATUS_SUCCESS,
	RTW_RET_STATUS_ERROR,
	RTW_RET_STATUS_INIT_ERROR,
	RTW_RET_STATUS_INVALID_INPUT,
	RTW_RET_STATUS_IW_FAIL,
	RTW_RET_STATUS_SUPPLICANT_FAIL,
} RTW_RET_STATUS;

enum nan_pairing_role {
	PAIRING_ROLE_INITIATOR = 1,
	PAIRING_ROLE_RESPONDER,
};

enum tx_follow_up_type {
	TX_FOLLOW_UP_BSTRP_REQ,
	TX_FOLLOW_UP_KEY_EXCHG
};

enum nan_bstrap_type {
	NAN_BSTRAP_TYPE_REQUEST = 1,
	NAN_BSTRAP_TYPE_RESPONSE = 2,
};

enum nan_pairing_type {
	NAN_PAIRING_TYPE_SETUP = 1,
	NAN_PAIRING_TYPE_VERIFICATION = 2,
};

enum nan_pairing_auth_mode {
	NAN_PAIRING_AUTH_MODE_PASSWORD = 1,
	NAN_PAIRING_AUTH_MODE_OPPORTUNISTIC = 2,
};

enum nan_pmk_setting {
	NAN_PMK_SET_BY_USER_PMK	= 1,
	NAN_PMK_SET_BY_USER_PASSPRHRAE = 2,
	NAN_PMK_SET_BY_PAIRING = 3,
};

enum nan_pasn_status {
	NAN_PASN_NONE,
	NAN_PASN_SEND_1,
	NAN_PASN_SEND_2,
	NAN_PASN_SEND_3,
	NAN_PASN_DONE
};

struct pairing_info {
	uint8_t role;
	uint8_t peer_nmi[ETH_ALEN];
	uint8_t bstrap_method;
	uint8_t pasn_type; /* enum nan_pairing_type */
	uint8_t pasn_auth_method; /* enum nan_pairing_auth_mode */
	uint8_t nik_caching;
	uint8_t peer_nik_caching;
	char pasn_pw[MAX_PASN_PW_LEN + 1];
	uint8_t pasn_pw_len;
};

struct srvc_info {
	uint8_t subscribe_id;
	uint8_t publish_id;
	uint8_t subscribe_nmi[ETH_ALEN];
	uint8_t publish_nmi[ETH_ALEN];
	uint8_t service_name[MAX_SRVC_NAME_LEN];
	uint8_t service_instance[MAX_SRVC_INST_LEN];
	uint8_t *subtype;
	uint8_t *text_info;
	uint16_t text_info_len;
	uint8_t sec_type; /* enum nan_pmk_setting, 0 reserved */
	uint8_t pmk[NAN_PMK_SIZE];
	struct pairing_info pairing_info;
	uint8_t gtk_enable;
};

struct datapath_info {
	uint8_t ndp_id;
	uint8_t initiator_ndi[ETH_ALEN];
	uint8_t responder_ndi[ETH_ALEN];
	uint8_t initiator_intf_identifier[MAX_IPV6_INFO_LEN];
	uint8_t responder_intf_identifier[MAX_IPV6_INFO_LEN];
	struct srvc_info *service;
	uint8_t hostname[256];
	uint16_t port_number;
	uint8_t pmk[NAN_PMK_SIZE];
	uint8_t sec_type; /* enum nan_pmk_setting, 0 reserved */
};

struct tx_followup_info {
	uint8_t type; // enum tx_follow_up_type
	uint8_t remote_inst_id;
	uint8_t local_inst_id;
	uint8_t remote_mac[ETH_ALEN];
	/* bootstrapping: req */
	uint16_t bstrap_method;
	/* key exchange: nik */
	uint8_t sec_key_type;
};


struct utility_bin_path {
	char iw_path[MAX_BIN_PATH_LEN];
	char nan_r3_path[MAX_BIN_PATH_LEN];
	char wpa_supplicant_path[MAX_BIN_PATH_LEN];
	char wpa_supplicant_conf_path[MAX_BIN_PATH_LEN];
	char wpa_supplicant_log_file[MAX_BIN_PATH_LEN];
	char wpa_supplicant_dbg_para[MAX_BIN_PATH_LEN];
};

/* nan attr related*/
enum nan_srvc_protocol_type {
	NAN_SRVC_PROTOCOL_BONJOUR = 1,
	NAN_SRVC_PROTOCOL_GENIC = 2,
};

enum nan_attr_with_wfa_srvc_info {
	NAN_WFA_SRVC_ATTR_SEDA = 1,
	NAN_WFA_SRVC_ATTR_NDPE = 2,
};

enum nan_generic_srvc_sub_attr_id {
	NAN_GEN_SRVC_TRANSPORT_PORT = 0x0,
	NAN_GEN_SRVC_TRANSPORT_PROTOCOL = 0x1,
	NAN_GEN_SRVC_SERVICE_NAME = 0x2,
	NAN_GEN_SRVC_NAME_OF_SRVC_INST = 0x3,
	NAN_GEN_SRVC_TEXT_INFO = 0x4,
	NAN_GEN_SRVC_UUID = 0x5,
	NAN_GEN_SRVC_BLOB = 0x6,
	NAN_GEN_SRVC_RESERVD = 0x7,
	NAN_GEN_SRVC_VENDOR_SPEC_INFO = 0xDD,
};
enum nan_cipher_suite_id {
	NAN_CIPHER_SUITE_BIT_OPEN             = 0,
	NAN_CIPHER_SUITE_BIT_NCS_SK_128       = 1, // NCS Secret Key, 128 bits
	NAN_CIPHER_SUITE_BIT_NCS_SK_256       = 2, // NCS Secret Key, 256 bits
	NAN_CIPHER_SUITE_BIT_NCS_PK_2WDH_128  = 3, // NCS Public Key, 2 Way Diffie Hellman, 128 bits
	NAN_CIPHER_SUITE_BIT_NCS_PK_2WDH_256  = 4, // NCS Public Key, 2 Way Diffie Hellman, 256 bits
	NAN_CIPHER_SUITE_BIT_NCS_GTK_CCMP_128 = 5,
	NAN_CIPHER_SUITE_BIT_NCS_GTK_GCMP_256 = 6,
	NAN_CIPHER_SUITE_BIT_NCS_PASN_128     = 7,
	NAN_CIPHER_SUITE_BIT_NCS_PASN_256     = 8,
	NAN_CIPHER_SUITE_BIT_MAX,
} ;
enum nan_cipher_suite_cap {
	NAN_CIPHER_SUITE_CAP_NDNM_TKSA_REPLY   = BIT(0),
	NAN_CIPHER_SUITE_CAP_GTKS_CAP_GTK_IGTK = BIT(1),
	NAN_CIPHER_SUITE_CAP_GTKS_CAP_BIGTK    = BIT(2),
	NAN_CIPHER_SUITE_CAP_GTKS_REPLY        = BIT(3),
	NAN_CIPHER_SUITE_CAP_BIP_256_SUPPORT   = BIT(4),
} ;

/* nan event related */
enum rtw_event_id {
	NAN_EVT_CLUSTER_CHANGE,
	NAN_EVT_DISCOVERY_RESULT, /* struct nan_evt_disc_result_info */
	NAN_EVT_DATAPATH_RECV_REQ,
	NAN_EVT_DATAPATH_RECV_RSP,
	NAN_EVT_DATAPATH_RECV_FOLLOW_UP,
	NAN_EVT_DATAPATH_ESTABLISHED,
	NAN_EVT_DATAPATH_END,
	NAN_EVT_DATA_INDICATION, /* struct nan_evt_data_indication_info */
	NAN_EVT_DATA_CONFIRM, /* struct nan_evt_data_confirm_info */
	NAN_EVT_BSTRAP_RESULT,
	NAN_EVT_PASN_RESULT,
	NAN_EVT_DATA_SHARED_KEY,
	NAN_EVT_BLE_RECV_TIGGER,
	NAN_EVT_BLE_RECV_TERMINATE,
	NAN_EVT_MAX
};
typedef void (*rtw_nan_evt_cb)(uint8_t event_id, void *buf, uint32_t len);

struct nan_evt_cluster_changed_info {
	uint8_t cluster_id[ETH_ALEN];
};
struct nan_evt_disc_result_info {
	uint8_t subscirbe_id;
	uint8_t publish_id;
	uint8_t peer_nmi[ETH_ALEN];
	int8_t peer_rssi;
	uint32_t channel;
	uint8_t service_id[SRVC_ID_LEN];
	uint8_t service_name[MAX_SRVC_NAME_LEN];
	uint8_t service_inst[MAX_SRVC_INST_LEN];
	uint16_t port_number;
	uint8_t trans_proto;
	uint8_t text_info[NAN_MAX_SERVICE_INFO_LEN];
	uint16_t text_info_len;
	uint8_t pmk_set;
	uint8_t pairing_enable;
	uint8_t nira_paired;
};
struct nan_evt_data_indication_info {
	uint8_t type; /* 0 = ucast */
	uint8_t publish_id;
	uint8_t ndp_id; /* ucast */
	uint8_t mc_id; /* mcast */
	uint8_t initiator_data_address[ETH_ALEN]; /* ucast */
	uint8_t responder_data_address[ETH_ALEN]; /* ucast */
	uint8_t security;
	uint8_t ipv6_interfac_id[MAX_IPV6_INFO_LEN];
};
struct nan_evt_data_confirm_info {
	uint8_t type; /* 0 = ucast */
	uint8_t status;
	uint8_t ndp_id; /* ucast */
	uint8_t mc_id; /* mcast */
	uint8_t nmsg_id; /* mcast */
	uint8_t initiator_data_address[ETH_ALEN]; /* ucast */
	uint8_t responder_data_address[ETH_ALEN]; /* ucast */
	uint16_t port_number;
	uint8_t trans_proto;
	uint8_t host_name[256];
};
struct nan_evt_data_established_info {
	uint8_t type;
	uint8_t publish_id;
	uint8_t ndp_id;
	uint8_t responder_data_address[ETH_ALEN]; /* ucast */
};
struct nan_evt_data_end_info {
	uint8_t ndp_id;
	uint8_t reason;
	uint8_t publish_id;
};
struct nan_evt_bstrap_result {
	uint8_t status;
	uint8_t remote_inst_id;
	uint8_t local_inst_id;
	uint8_t local_role;
	uint16_t bstrap_method;
	uint8_t peer_nmi[ETH_ALEN];
};
struct nan_evt_pasn_result {
	uint8_t status;
	uint8_t local_role;
	uint8_t pairing_type;
	uint8_t remote_nik_caching;
	uint8_t peer_nmi[ETH_ALEN];
};
struct nan_evt_data_recv_shared_key {
	uint8_t role;
	uint8_t status;
	uint8_t peer_nmi[ETH_ALEN];
};
#endif