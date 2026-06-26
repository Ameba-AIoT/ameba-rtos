/**
  ******************************************************************************
  * @file    rtw_nan_vendor_def.h
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef __RTW_NAN_VENDOR_DEF_H__
#define __RTW_NAN_VENDOR_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ========== Constants ========== */
#define RTW_NAN_SERVICE_NAME_MAX_LENGTH          255
#define RTW_NAN_MATCHING_FILTER_MAX_LENGTH       32
#define RTW_NAN_SERVICE_INFO_MAX_LENGTH          512
#define RTW_NAN_SERVICE_NAME_HASH_SIZE           6
#define RTW_NAN_PMK_SIZE                         32
#define RTW_NAN_PMKID_SIZE                       16
#define RTW_NAN_MAX_TX_MATCHING_FILTERS          16
#define RTW_NAN_MAX_RX_MATCHING_FILTERS          16
#define RTW_NAN_IPV6_INTERFACE_IDENTIFIER_LENGTH 8
#define RTW_NAN_MAX_MAC_ADDRESS_FILTERS          16
#define RTW_NAN_BLOOM_FILTER_MAX_SIZE            64
#define RTW_NAN_AVAILABILITY_MAX_BITMAP_LENGTH   64
#define RTW_NAN_AVAILABILITY_MAX_CHANNEL_ENTRIES 16
#define RTW_NAN_MAX_BAND_IDS                     6
#define RTW_NAN_MAX_MAP_IDS                      2
#define RTW_NAN_OUT_OF_BOUND_ACTION_FRAME_MAX_SIZE 960
#define RTW_NAN_MAX_COUNTRY_CODE_LEN             2

/* NAN event IDs */
enum rtw_nan_event_id {
	RTW_NAN_EVT_NAN_CLUSTER_CHANGED                = 1,   // NAN cluster changed
	RTW_NAN_EVT_DISC_RESULT                        = 2,   // Discovery result received
	RTW_NAN_EVT_PUBLISH_REPLIED                    = 3,   // Publish replied by subscriber
	RTW_NAN_EVT_DATAPATH_REQ_RECV                  = 4,   // Datapath request received
	RTW_NAN_EVT_DATAPATH_RSP_RECV                  = 5,   // Datapath response received
	RTW_NAN_EVT_DATAPATH_CONFIRM_RECV              = 6,   // Datapath confirm received
	RTW_NAN_EVT_DATAPATH_ESTABLISHED               = 7,   // Datapath established
	RTW_NAN_EVT_DATAPATH_END                       = 8,   // Datapath ended
	RTW_NAN_EVT_DATAPATH_HOST_ASSIST               = 9,   // Host assist request
	RTW_NAN_EVT_SUBSCRIBE_TERMINATED               = 10,  // Subscribe terminated
	RTW_NAN_EVT_PUBLISH_TERMINATED                 = 11,  // Publish terminated
	RTW_NAN_EVT_FOLLOWUP_RECV                      = 12,  // Follow-up received
	RTW_NAN_EVT_FOLLOWUP_TX_COMPLETE               = 13,  // Follow-up tx complete
	RTW_NAN_EVT_BEACON                             = 14,  // Beacon received
	RTW_NAN_EVT_OOB_AF_TX_STATUS                   = 15,  // Transmit an out of bound action frame (OOB AF)
	RTW_NAN_EVT_OOB_AF_RX                          = 16,  // Receive an out of bound action frame (OOB AF)
	RTW_NAN_EVT_RESET_INTERFACE                    = 17,  // Chip reset
	RTW_NAN_EVT_COUNTRY_CODE_CHANGED               = 18,  // Country code changed
	RTW_NAN_EVT_INFRA_ASSOC_START                  = 19,  // Infra assoc start
	RTW_NAN_EVT_ASSOC_DONE                         = 20,  // Association done
	RTW_NAN_EVT_ASSOC_READY                        = 21,  // Association ready
	RTW_NAN_EVT_AUTH                               = 22,  // Authentication
	RTW_NAN_EVT_ASSOC                              = 23,  // Association
	RTW_NAN_EVT_INFRA_SCAN_START                   = 24,  // Infra scan start
	RTW_NAN_EVT_INFRA_SCAN_COMPLETE                = 25,  // Infra scan complete
	RTW_NAN_EVT_SET_INFRA_CHANNEL                  = 26,  // Set infra channel
};

#define NANDOW_EVENTID_OFFSET 0
#define GET_OVERALL_EVENT_ID(nandow_id) (NANDOW_EVENTID_OFFSET + nandow_id)

/* NAN command IDs */
enum rtw_nan_cmd_id {
	RTW_NAN_CMD_INIT                               = 1,   // Device capability
	RTW_NAN_CMD_WIFI_DRV_CAP                       = 2,   // Driver capability
	RTW_NAN_CMD_NAN_ENABLE                         = 3,   // NAN enable/disable
	RTW_NAN_CMD_CLUSTER_ID                         = 4,   // Cluster ID
	RTW_NAN_CMD_DISC_BCN_PERIOD                    = 5,   // Discovery beacon period
	RTW_NAN_CMD_MASTER_PREF                        = 6,   // Master preference
	RTW_NAN_CMD_RANDOM_FACTOR                      = 7,   // Random factor
	RTW_NAN_CMD_DW_AWAKE_PERIOD                    = 8,   // DW awake period
	RTW_NAN_CMD_PRI_MASTER_CH                      = 9,   // Primary master channel
	RTW_NAN_CMD_SEC_MASTER_CH                      = 10,  // Secondary master channel
	RTW_NAN_CMD_HOST_ELECTION                      = 11,  // Host election
	RTW_NAN_CMD_ELECTION_PARAMS                    = 12,  // Election parameters
	RTW_NAN_CMD_SUBSCRIBE                          = 13,  // Subscribe service
	RTW_NAN_CMD_PUBLISH                            = 14,  // Publish service
	RTW_NAN_CMD_DATAPATH_REQ                       = 15,  // Datapath request
	RTW_NAN_CMD_CANCEL_PUBLISH                     = 16,  // Cancel publish
	RTW_NAN_CMD_CANCEL_SUBSCRIBE                   = 17,  // Cancel subscribe
	RTW_NAN_CMD_DATAPATH_END                       = 18,  // Datapath terminate
	RTW_NAN_CMD_FOLLOWUP_TX                        = 19,  // Follow-up transmit
	RTW_NAN_CMD_DATAPATH_RSP                       = 21,  // Datapath response
	RTW_NAN_CMD_DATAPATH_CONFIRM                   = 22,  // Datapath confirm
	RTW_NAN_CMD_CMT_AVAIL                          = 23,  // Committed availability
	RTW_NAN_CMD_POT_AVAIL                          = 24,  // Potential availability
	RTW_NAN_CMD_NDC_AVAIL                          = 25,  // NDC availability
	RTW_NAN_CMD_FORCED_DISC_BCN                    = 26,  // Forced discovery beacon
	RTW_NAN_CMD_SCAN_CONTROL                       = 31,  // Scan control
	RTW_NAN_CMD_COUNTRY_CODE                       = 32,  // Country code
};

/* NAN service type and status enums */
enum rtw_nan_subscribe_type {
	RTW_NAN_SUBSCRIBE_TYPE_PASSIVE                      = 1,
	RTW_NAN_SUBSCRIBE_TYPE_ACTIVE                       = 2,
};

enum rtw_nan_publish_type {
	RTW_NAN_PUBLISH_TYPE_UNSOLICITED                    = 1,
	RTW_NAN_PUBLISH_TYPE_SOLICITED                      = 2,
};

enum rtw_nan_dp_status {
	RTW_NAN_DATAPATH_CONTINUED                       = 0,
	RTW_NAN_DATAPATH_ACCEPTED                        = 1,
	RTW_NAN_DATAPATH_REJECTED                        = 2,
};

enum rtw_nan_dp_end_reason {
	RTW_NAN_DATAPATH_END_REASON_USER_INITIATED                              = 0,
	RTW_NAN_DATAPATH_END_REASON_PEER_INITIATED                              = 1,
	RTW_NAN_DATAPATH_END_REASON_CHIP_RESET                                  = 2,
	RTW_NAN_DATAPATH_END_REASON_NAN_DISABLE                                 = 3,
	RTW_NAN_DATAPATH_END_REASON_REQUEST_TX_FAIL                             = 4,
	RTW_NAN_DATAPATH_END_REASON_RESPONSE_TIMEOUT                            = 5,
	RTW_NAN_DATAPATH_END_REASON_USER_INPUT_TIMEOUT                          = 6,
	RTW_NAN_DATAPATH_END_REASON_RESPONSE_TX_FAIL                            = 7,
	RTW_NAN_DATAPATH_END_REASON_CONFIRMATION_TIMEOUT                        = 8,
	RTW_NAN_DATAPATH_END_REASON_CONFIRMATION_TX_FAIL                        = 9,
	RTW_NAN_DATAPATH_END_REASON_PEER_CACHE_TIMEOUT                          = 10,
	RTW_NAN_DATAPATH_END_REASON_TERM_TIMEOUT                                = 11,
	RTW_NAN_DATAPATH_END_REASON_PEER_INACTIVE                               = 12,
	RTW_NAN_DATAPATH_END_REASON_INITIATE_WAIT_TIMEOUT                       = 13,
	RTW_NAN_DATAPATH_END_REASON_PEER_NOT_FOUND                              = 14,
	RTW_NAN_DATAPATH_END_REASON_PEER_CHIP_REJECT                            = 15,
	RTW_NAN_DATAPATH_END_REASON_KEEP_ALIVE_TIMEOUT                          = 16,
	RTW_NAN_DATAPATH_END_REASON_ENCRYPTED_DATAPATH_TERMINATED_ON_CHIP_RESET = 17,
	RTW_NAN_DATAPATH_END_REASON_CHIP_TIMEOUT                                = 18,
};

/* NAN bitmask flag enums */
enum rtw_nan_publish_flag {
	RTW_NAN_PUBLISH_FLAG_DISCOVERY_RANGE_LIMITED            = (1 << 0),
	RTW_NAN_PUBLISH_FLAG_RANGING_REQUIRED                   = (1 << 1),
	RTW_NAN_PUBLISH_FLAG_ADD_DEVICE_CAPABILITY_ATTRIBUTE    = (1 << 2),
	RTW_NAN_PUBLISH_FLAG_ADD_AVAILABILITY_ATTRIBUTE         = (1 << 3),
	RTW_NAN_PUBLISH_FLAG_ENABLE_SERVICE_BEACON_CARRY        = (1 << 4),
	RTW_NAN_PUBLISH_FLAG_ENABLE_SERVICE_AUTOMATIC_RESPONSE  = (1 << 5),
};

enum rtw_nan_subscribe_flag {
	RTW_NAN_SUBSCRIBE_FLAG_DISCOVERY_RANGE_LIMITED           = (1 << 0),
	RTW_NAN_SUBSCRIBE_FLAG_RANGING_REQUIRED                  = (1 << 1),
	RTW_NAN_SUBSCRIBE_EXCLUDE_MAC_ADDRESS_FILTER             = (1 << 2),
	RTW_NAN_SUBSCRIBE_INCLUDE_BLOOM_FILTER                   = (1 << 3),
	RTW_NAN_SUBSCRIBE_FLAG_ADD_DEVICE_CAPABILITY_ATTRIBUTE   = (1 << 4),
	RTW_NAN_SUBSCRIBE_FLAG_ADD_AVAILABILITY_ATTRIBUTE        = (1 << 5),
	RTW_NAN_SUBSCRIBE_FLAG_ENABLE_SERVICE_BEACON_CARRY       = (1 << 6),
	RTW_NAN_SUBSCRIBE_FLAG_ENABLE_SERVICE_AUTOMATIC_RESPONSE = (1 << 7),
};

enum rtw_nan_op_mode {
	RTW_NAN_OP_MODE_VHT                            = (1 << 0),
	RTW_NAN_OP_MODE_VHT80_80                       = (1 << 1),
	RTW_NAN_OP_MODE_VHT_160                        = (1 << 2),
	RTW_NAN_OP_MODE_NDL_PAGING                     = (1 << 3),
};

enum rtw_nan_band_flag {
	RTW_NAN_BAND_TV_WHITE_SPACES                   = (1 << 0),
	RTW_NAN_BAND_SUB_1GHZ                          = (1 << 1),
	RTW_NAN_BAND_2G                                = (1 << 2),
	RTW_NAN_BAND_3_6G                              = (1 << 3),
	RTW_NAN_BAND_5G                                = (1 << 4),
	RTW_NAN_BAND_60G                               = (1 << 5),
};

enum rtw_nan_drv_cap_flag {
	RTW_NAN_DRV_SUPPORTS_NAN                       = (1 << 0),
	RTW_NAN_DRV_SUPPORTS_DUAL_BAND                 = (1 << 2),
	RTW_NAN_DRV_SUPPORTS_SIMULTANEOUS_DUAL_BAND    = (1 << 3),
	RTW_NAN_DRV_IS_CLM_RESTRICTED                  = (1 << 4),
	RTW_NAN_DRV_PEER_COUNTRY_CODE_LEARNING_CAPABLE = (1 << 5),
};

enum rtw_nan_chan_flag {
	RTW_NAN_CHAN_FLAG_NONE                         = 0x0,
	RTW_NAN_CHAN_FLAG_10MHZ                        = 0x1,
	RTW_NAN_CHAN_FLAG_20MHZ                        = 0x2,
	RTW_NAN_CHAN_FLAG_40MHZ                        = 0x4,
	RTW_NAN_CHAN_FLAG_2GHZ                         = 0x8,
	RTW_NAN_CHAN_FLAG_5GHZ                         = 0x10,
	RTW_NAN_CHAN_FLAG_IBSS                         = 0x20,
	RTW_NAN_CHAN_FLAG_HOST_AP                      = 0x40,
	RTW_NAN_CHAN_FLAG_ACTIVE                       = 0x80,
	RTW_NAN_CHAN_FLAG_DFS                          = 0x100,
	RTW_NAN_CHAN_FLAG_EXTENSION_ABOVE              = 0x200,
	RTW_NAN_CHAN_FLAG_80MHZ                        = 0x400,
	RTW_NAN_CHAN_FLAG_160MHZ                       = 0x800,
	RTW_NAN_CHAN_FLAG_RSSI_INVALID                 = 0x1000,
	RTW_NAN_CHAN_FLAG_PASSIVE_MODE                 = 0x2000,
	RTW_NAN_CHAN_FLAG_INDOOR_RESTRICTED            = 0x4000,
	RTW_NAN_CHAN_FLAG_CLM_RESTRICTED               = 0x8000,
};

/* ========== Basic Types ========== */
struct rtw_nan_ether_addr {
	uint8_t ether_addr_octet[6];
};

struct rtw_nan_channel {
	uint32_t channel;   // channel number
	uint32_t flags;     // channel flags (rtw_nan_chan_flag)
};

/* ========== Cipher Suite ID ========== */
enum rtw_nan_cipher_suite_id_vendor {
	RTW_NAN_CIPHER_ID_OPEN         = 0,
	RTW_NAN_CIPHER_ID_NCS_SK_128   = 1,  // NCS Secret Key, 128 bits
	RTW_NAN_CIPHER_ID_NCS_SK_256   = 2,  // NCS Secret Key, 256 bits
	RTW_NAN_CIPHER_ID_PK_2WDH_128  = 3,  // Public Key, 2 Way Diffie Hellman, 128 bits
	RTW_NAN_CIPHER_ID_PK_2WDH_256  = 4,  // Public Key, 2 Way Diffie Hellman, 256 bits
	RTW_NAN_CIPHER_ID_GTK_CCMP_128 = 5,  // GTK, CCMP, 128 bits
	RTW_NAN_CIPHER_ID_GTK_GCMP_256 = 6,  // GTK, GCMP, 256 bits
	RTW_NAN_CIPHER_ID_PASN_128     = 7,  // PASN, 128 bits
	RTW_NAN_CIPHER_ID_PASN_256     = 8,  // PASN, 256 bits
};

/* ========== Service Descriptor Extension Control bits ========== */
enum rtw_nan_sde_control {
	RTW_NAN_SDE_CTL_FSD_REQUIRED               = (1 << 0),
	RTW_NAN_SDE_CTL_FSD_WITH_GAS               = (1 << 1),
	RTW_NAN_SDE_CTL_DATAPATH_REQ_REQUIRED       = (1 << 2),
	RTW_NAN_SDE_CTL_DATAPATH_TYPE               = (1 << 3),
	RTW_NAN_SDE_CTL_MULTICAST_TYPE              = (1 << 4),
	RTW_NAN_SDE_CTL_QOS_REQUIRED                = (1 << 5),
	RTW_NAN_SDE_CTL_SECURITY_REQUIRED           = (1 << 6),
	RTW_NAN_SDE_CTL_RANGING_REQUIRED            = (1 << 7),
	RTW_NAN_SDE_CTL_RANGE_LIMIT_PRESENT         = (1 << 8),
	RTW_NAN_SDE_CTL_SERVICE_INFO_PRESENT        = (1 << 9),
};

/* ========== NAN sub-types (binary-compatible structs) ========== */
struct rtw_nan_service_name {
	char name[RTW_NAN_SERVICE_NAME_MAX_LENGTH];
};

struct rtw_nan_matching_filter {
	uint16_t length;       // length of the filter
	uint8_t  filter[RTW_NAN_MATCHING_FILTER_MAX_LENGTH];
};

struct rtw_nan_service_info {
	uint16_t length;       // length of the info body
	uint8_t  info[RTW_NAN_SERVICE_INFO_MAX_LENGTH];
};

struct rtw_nan_qos_requirements {
	uint16_t tid;
	uint32_t service_data_packet_size;
	uint16_t data_rate;
	uint16_t max_service_internal;
	uint8_t  low_latency_required;
	uint8_t  type;         // wfpal_nan_data_qos_types
};

struct rtw_nan_availability_time_bitmap {
	uint16_t offset;             // start offset in bits
	uint8_t  period;             // wfpal_nan_channel_search_period
	uint8_t  bit_duration;       // 0=16TUs, 1=32TUs, 2=64TUs, 3=128TUs
	uint8_t  time_bitmap_length;
	uint8_t  time_bitmap[RTW_NAN_AVAILABILITY_MAX_BITMAP_LENGTH];
};

struct rtw_nan_channel_availability_entry {
	uint8_t  usage_preference;   // 0=none to 3=high
	uint8_t  utilization;        // 0-5 representing % utilization
	uint8_t  rx_nss;             // number of spatial streams
	struct   rtw_nan_availability_time_bitmap time_bitmap;
	uint8_t  op_class;           // operating class
	uint8_t  primary_channel_bitmap;
	uint16_t op_class_bitmap;
	uint16_t auxiliary_channel_bitmap;
};

/* ========== Command Data Structs ========== */
struct rtw_nan_device_capability {
	uint8_t  supported_bands;               // rtw_nan_band_flag
	uint8_t  supported_cipher_suites;       // rtw_nan_cipher_suite_id_vendor
	uint8_t  operation_mode_ht_vht;         // rtw_nan_op_mode
	uint8_t  number_of_antennas;
	uint16_t max_channel_switch_time;       // in microseconds
	uint8_t  max_publishers;
	uint8_t  max_subscribers;
	uint8_t  max_peer_entries;
	uint8_t  max_datapath_sessions;
	uint8_t  max_avail_maps;
	uint8_t  max_nan_interfaces;
	uint8_t  unicast_insecure_datapath_restore_supported;
};

struct rtw_nan_driver_capabilities {
	uint16_t capabilities;   // rtw_nan_drv_cap_flag
};

struct rtw_nan_enable {
	uint8_t enable;   // 0=disable, 1=enable
};

struct rtw_nan_cluster_id {
	struct rtw_nan_ether_addr cluster_id;
};

struct rtw_nan_discovery_beacon_period {
	uint16_t period;   // in TUs
};

struct rtw_nan_master_preference {
	uint8_t master_preference;
};

struct rtw_nan_random_factor {
	uint8_t random_factor;
};

struct rtw_nan_discovery_window_awake_period {
	uint8_t period_2_4g;   // awake interval in 2.4GHz (in TUs)
	uint8_t period_5g;     // awake interval in 5GHz (in TUs)
};

struct rtw_nan_primary_master_channel {
	uint32_t channel;
};

struct rtw_nan_secondary_master_channel {
	uint32_t channel;
};

/* ========== Service (Publish/Subscribe) Data Structs ========== */
struct rtw_nan_publish_data {
	uint8_t  service_hash[RTW_NAN_SERVICE_NAME_HASH_SIZE];
	struct   rtw_nan_service_name name;
	struct   rtw_nan_matching_filter matching_filter_tx[RTW_NAN_MAX_TX_MATCHING_FILTERS];
	struct   rtw_nan_matching_filter matching_filter_rx[RTW_NAN_MAX_RX_MATCHING_FILTERS];
	struct   rtw_nan_service_info service_specific_info;
	uint8_t  matching_filter_tx_count;
	uint8_t  matching_filter_rx_count;
	uint8_t  matching_filter_flag;
	uint8_t  publish_type;          // rtw_nan_publish_type
	uint8_t  solicited_tx_type;
	uint8_t  publish_id;            // Publish Instance ID
	uint8_t  awake_discovery_window_interval;  // awake interval in 512TUs
	int32_t  discovery_range;       // RSSI
	uint32_t announcement_period;   // unit: discovery windows
	uint32_t ttl;                   // unit: discovery windows
	uint32_t event_condition_flags;
	uint32_t flags;                 // rtw_nan_publish_flag
	uint16_t control;               // rtw_nan_sde_control
	uint16_t cipher_suite_id;       // bitwise, rtw_nan_cipher_suite_id_vendor
	uint8_t  key[RTW_NAN_PMK_SIZE];
	struct   rtw_nan_qos_requirements qos;
	uint8_t  service_update_indicator;
	uint8_t  interface_identifier[RTW_NAN_IPV6_INTERFACE_IDENTIFIER_LENGTH];
	struct   rtw_nan_service_info data_service_specific_info;
	struct   rtw_nan_ether_addr responder_data_address;
	uint8_t  pmkid[RTW_NAN_PMKID_SIZE];
	uint8_t  pairing_enable;
	uint16_t bstrap_method;         // bitwise list
	uint8_t  gtk;                   // GTK protection enable
	uint8_t  npk_nik;               // NIK caching enable
};

struct rtw_nan_subscribe_data {
	uint8_t  service_hash[RTW_NAN_SERVICE_NAME_HASH_SIZE];
	struct   rtw_nan_service_name name;
	struct   rtw_nan_matching_filter matching_filter_tx[RTW_NAN_MAX_TX_MATCHING_FILTERS];
	struct   rtw_nan_matching_filter matching_filter_rx[RTW_NAN_MAX_RX_MATCHING_FILTERS];
	struct   rtw_nan_service_info service_specific_info;
	uint8_t  matching_filter_tx_count;
	uint8_t  matching_filter_rx_count;
	uint8_t  matching_filter_flag;
	uint8_t  subscribe_type;    // rtw_nan_subscribe_type
	uint8_t  subscribe_id;      // Subscribe Instance ID
	uint8_t  awake_discovery_window_interval;
	uint16_t bloom_filter_length;
	uint8_t  bloom_idx;         // Bloom filter index
	uint8_t  bloom_filter[RTW_NAN_BLOOM_FILTER_MAX_SIZE];
	int32_t  discovery_range;
	uint32_t query_period;
	uint32_t ttl;
	uint32_t flags;             // rtw_nan_subscribe_flag
	uint16_t control;           // rtw_nan_sde_control
	uint8_t  pairing_enable;
	uint16_t bstrap_method;
	uint8_t  npk_nik;
};

struct rtw_nan_cancel_publish_data {
	uint8_t publish_id;
};

struct rtw_nan_cancel_subscribe_data {
	uint8_t subscribe_id;
};

struct rtw_nan_follow_up_transmit_data {
	uint8_t  local_service_id;         // Subscribe/Publish ID
	uint8_t  requestor_instance_id;
	uint16_t follow_up_token;
	struct   rtw_nan_ether_addr destination_address;
	struct   rtw_nan_service_info service_specific_info;
};

/* ========== Datapath Structs ========== */
struct rtw_nan_datapath_request {
	uint8_t  datapath_id;
	uint8_t  dialog_token;
	uint8_t  responder_publish_id;
	uint8_t  initiator_subscribe_id;
	uint8_t  service_hash[RTW_NAN_SERVICE_NAME_HASH_SIZE];
	uint8_t  confirm_required;
	struct   rtw_nan_ether_addr initiator_data_address;
	struct   rtw_nan_ether_addr responder_nan_addr;
	struct   rtw_nan_service_info service_specific_info;
	uint8_t  cipher_suite_id;
	uint8_t  key[RTW_NAN_PMK_SIZE];
	struct   rtw_nan_qos_requirements qos;
	uint8_t  interface_identifier[RTW_NAN_IPV6_INTERFACE_IDENTIFIER_LENGTH];
	uint8_t  pmkid[RTW_NAN_PMKID_SIZE];
	uint8_t  sec_type;   // rtw_nan_pmk_setting
};

struct rtw_nan_datapath_response {
	uint8_t  datapath_id;
	uint8_t  status;            // rtw_nan_dp_status
	uint8_t  publish_id;
	uint8_t  reason_code;
	uint8_t  service_hash[RTW_NAN_SERVICE_NAME_HASH_SIZE];
	struct   rtw_nan_ether_addr initiator_data_address;
	struct   rtw_nan_ether_addr initiator_management_address;
	struct   rtw_nan_ether_addr responder_data_address;
	struct   rtw_nan_ether_addr multicast_address;
	struct   rtw_nan_service_info service_specific_info;
	struct   rtw_nan_qos_requirements unicast_qos_req;
	struct   rtw_nan_qos_requirements multicast_qos_req;
	uint8_t  cipher_suite_id;
	uint8_t  key[RTW_NAN_PMK_SIZE];
	uint8_t  pmkid[RTW_NAN_PMKID_SIZE];
};

struct rtw_nan_datapath_confirm {
	uint8_t  datapath_id;
	uint8_t  status;            // rtw_nan_dp_status
	struct   rtw_nan_ether_addr initiator_data_address;
};

struct rtw_nan_datapath_end {
	uint8_t  datapath_id;
	uint8_t  dialog_token;
	uint8_t  reason_code;       // rtw_nan_dp_end_reason
	struct   rtw_nan_ether_addr initiator_data_address;
	struct   rtw_nan_ether_addr peer_address;
};

/* ========== Availability Structs ========== */
struct rtw_nan_committed_ch_schedule {
	uint8_t  max_period;
	uint8_t  num_entries;
	uint8_t  map_id;
	struct   rtw_nan_channel_availability_entry channel_entries[RTW_NAN_AVAILABILITY_MAX_CHANNEL_ENTRIES];
};

struct rtw_nan_committed_availability {
	struct rtw_nan_ether_addr addr;    // peer mac, NULL for self
	uint8_t  num_maps_ids;
	struct   rtw_nan_committed_ch_schedule schedule[RTW_NAN_MAX_MAP_IDS];
};

struct rtw_nan_potential_ch_schedule {
	uint8_t  map_id;
	uint8_t  num_band_entries;
	uint8_t  band_ids[RTW_NAN_MAX_BAND_IDS];
	uint8_t  num_entries;
	struct   rtw_nan_channel_availability_entry channel_entries[RTW_NAN_AVAILABILITY_MAX_CHANNEL_ENTRIES];
};

struct rtw_nan_potential_availability {
	uint8_t  num_maps_ids;
	struct rtw_nan_potential_ch_schedule potential[RTW_NAN_MAX_MAP_IDS];
};

struct rtw_nan_data_cluster_avail_params {
	struct   rtw_nan_ether_addr data_cluster_id;
	uint8_t  map_id;
	uint8_t  selected;       // 1=true, 0=false
	struct   rtw_nan_availability_time_bitmap time_bitmap;
};

struct rtw_nan_data_cluster_availability {
	uint8_t  num_maps_ids;
	struct rtw_nan_data_cluster_avail_params availability_params[RTW_NAN_MAX_MAP_IDS];
};

struct rtw_nan_forced_discovery_beacon_tx_avail_params {
	uint8_t  map_id;
	struct   rtw_nan_availability_time_bitmap time_bitmap;
};

struct rtw_nan_forced_discovery_beacon_tx_avail {
	uint8_t  num_map_ids;
	struct rtw_nan_forced_discovery_beacon_tx_avail_params slots[RTW_NAN_MAX_MAP_IDS];
};

struct rtw_nan_forced_discovery_beacon_transmission {
	uint8_t  enable;            // 1=true, 0=false
	uint32_t beacon_interval;   // in TUs
	struct rtw_nan_forced_discovery_beacon_tx_avail availability;
	uint8_t  reason;
};

/* ========== Misc Commands ========== */
struct rtw_nan_scan_control_params {
	uint8_t abort_scan;
	uint8_t epno_scan_disable;          // ePNO scan disable
	uint8_t roam_scan_disable;
	uint8_t limited_roam_enable;
};

struct rtw_nan_set_scan_control {
	uint8_t client;                  // rtw_nan_scan_control_client
	uint8_t is_clear_request;
	struct   rtw_nan_scan_control_params params;
};

struct rtw_nan_country_code_data {
	uint8_t country_code[RTW_NAN_MAX_COUNTRY_CODE_LEN];  // ISO 3166-1 alpha-2
};

/* ========== Event Data Structs (for parsing driver events) ========== */
struct rtw_nan_cluster_changed_event_data {
	struct rtw_nan_ether_addr cluster_id;
};

struct rtw_nan_discovery_result_received_event_data {
	uint8_t	subscribe_id;
	uint8_t	publisher_id;
	struct	rtw_nan_ether_addr peer_nan_address;
	int8_t	publish_rssi;
	struct	rtw_nan_channel channel;   // channel received on
	uint16_t	cipher_suite_id;         // bitwise csid list
	uint8_t	nira_paired;
	uint16_t	publish_attribute_list_length;
	uint8_t	publish_attribute_list[0];
};

struct rtw_nan_datapath_request_received_event_data {
	uint8_t  datapath_id;
	uint8_t  publish_id;
	uint8_t  security;
	struct   rtw_nan_ether_addr initiator_management_address;
	struct   rtw_nan_ether_addr initiator_data_address;
	uint16_t attribute_list_length;
	uint8_t  attribute_list[0];
};

struct rtw_nan_datapath_response_received_event_data {
	uint8_t  datapath_id;
	uint8_t  publish_id;
	struct   rtw_nan_ether_addr initiator_data_address;
	struct   rtw_nan_ether_addr responder_management_address;
	struct   rtw_nan_ether_addr responder_data_address;
	uint16_t attribute_list_length;
	uint8_t  attribute_list[0];
};

struct rtw_nan_datapath_established_event_data {
	uint8_t  type;
	uint8_t  datapath_id;
	uint8_t  pub_id;
	struct   rtw_nan_ether_addr initiator_data_address;
	struct   rtw_nan_ether_addr responder_data_address;
	struct   rtw_nan_ether_addr peer_management_address;
	uint8_t  peer_interface_identifier[RTW_NAN_IPV6_INTERFACE_IDENTIFIER_LENGTH];
	struct   rtw_nan_service_info service_specific_info;
};

struct rtw_nan_datapath_end_event_data {
	uint8_t  datapath_id;
	uint8_t  publish_id;
	uint8_t  reason;
	struct   rtw_nan_ether_addr initiator_data_address;
	struct   rtw_nan_ether_addr peer_data_address;
	struct   rtw_nan_ether_addr peer_management_address;
};

struct rtw_nan_follow_up_received_event_data {
	uint8_t  local_service_id;      // local subscribe/publish ID
	uint8_t  peer_service_id;
	struct   rtw_nan_ether_addr peer_nan_address;
	int8_t   follow_up_rssi;
	struct rtw_nan_channel channel;
	uint16_t attribute_list_length;
	uint8_t  attribute_list[0];
};

struct rtw_nan_publish_replied_private_event_data {
	uint8_t  publish_id;
	struct   rtw_nan_ether_addr peer_nan_address;
	uint8_t  peer_subscriber_id;
	int8_t   subscribe_rssi;
	struct rtw_nan_channel channel;
	uint16_t subscribe_attribute_list_length;
	uint8_t  subscribe_attribute_list[0];
};

struct rtw_nan_follow_up_tx_complete_event_data {
	uint8_t  local_service_id;
	uint8_t  peer_service_id;
	uint32_t follow_up_token;
	uint32_t status;               // tx status
	struct   rtw_nan_ether_addr peer_nan_address;
};

struct rtw_nan_subscribe_terminated_event_data {
	uint8_t  subscribe_id;
	uint8_t  reason;               // termination reason
};

struct rtw_nan_publish_terminated_event_data {
	uint8_t  publish_id;
	uint8_t  reason;               // termination reason
};

struct rtw_nan_beacon_received_event_data {
	int32_t	rssi;
	struct	rtw_nan_channel channel;
	uint8_t	rate;
	uint32_t	local_tsf_offset_h;   // TSF higher 32 bits
	uint32_t	local_tsf_offset_l;   // TSF lower 32 bits
	uint16_t	length;
	uint8_t	frame[0];              // beacon frame starting with 802.11 header
};

struct rtw_nan_datapath_confirm_received_event_data {
	uint8_t  datapath_id;
	uint8_t  status;
	struct   rtw_nan_ether_addr initiator_management_address;
	struct   rtw_nan_ether_addr initiator_data_address;
	uint16_t attribute_list_length;
	uint8_t  attribute_list[0];
};

struct rtw_nan_datapath_host_assist_request_event_data {
	uint8_t  datapath_id;
	uint8_t  publish_id;
	struct   rtw_nan_ether_addr initiator_data_address;
	struct   rtw_nan_ether_addr peer_management_address;
	uint8_t  frame_type;
	uint8_t  frame_subtype;
	uint8_t  reason;               // host assist reason
};

#ifdef __cplusplus
}
#endif

#endif /* __RTW_NAN_VENDOR_DEF_H__ */
