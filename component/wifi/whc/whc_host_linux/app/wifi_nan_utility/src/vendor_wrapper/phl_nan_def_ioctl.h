#ifndef _PHL_NAN_DEF_IOCTL_H_
#define _PHL_NAN_DEF_IOCTL_H_

/*
   To correctly exchange info bwteen wifi driver in kernel and
   nan application(cfg80211) in user-space,
   the definitions here have to be the same in both sides.

   This header contains these common definitions in phl level,
   meaning only phl layer is they exposed.

   Notice that nan application(cfg80211) could be
   nan utility, hostapd or any user-space apps via cfg80211/iw

   for nan utility:
    - Please make sure this headers within them are identical.

   for hostapd (to support pairing/pasn nan)
    - Please make sure "pasn_nan(hostapd)" section is identical
      in here and realtek_vendor.h
*/

#if defined(CONFIG_NAN) || defined(CONFIG_PHL_NAN) || defined(CONFIG_NAN_APP)

/* discovery engine */
#define NAN_SRVC_ID_LEN		(6)
#define NAN_MAX_SERVICE_INFO_LEN	(255)
#define NAN_MAX_SECURITY_INFO_LEN	(127)
#define MAX_PMKID_LEN			(16)
#define MAX_PMK_LEN			(32)
#define MAX_PASSPHRASE_LEN		(64)

/*
   ### pasn_nan(hostapd) section ###

   these macroes and event structures are
   exposed to hostapd/wpa_supplicant,
   so please make sure that any change are synced
   with realtek_vendor.h in wpa_supplicant

 */
#define MAX_PASN_RX_LEN			(512)
#define MAX_PASN_PW_LEN			(16) /* (hostapd) MAX_PASSPHRASE_LEN */
#define MAX_PASN_NAN_APPEND_IE_LEN	(64)

#define SET_BSTRAP_TYPE_REQ(_type_stats) \
        ((_type_stats) = (((_type_stats) & 0xF0) | 0x01))
#define SET_BSTRAP_TYPE_RSP(_type_stats) \
        ((_type_stats) = (((_type_stats) & 0xF0) | 0x02))
#define SET_BSTRAP_STATUS(_type_stats, _val) \
        ((_type_stats) = (((_type_stats) & 0x0F) | ((_val) << 4)))
#define GET_BSTRAP_STATUS(_type_stats) ((_type_stats) >> 4)
#define IS_BSTRAP_TYPE_REQ(_type_stats) (1 == ((_type_stats) & 0x0F))
#define IS_BSTRAP_COMEBACK_REQ(_type_stats) \
        (IS_BSTRAP_TYPE_REQ(_type_stats) && (2 == ((_type_stats) >> 4)))

enum nan_srvc_protocol_type {
	NAN_SRVC_PROTOCOL_BONJOUR	=	1,
	NAN_SRVC_PROTOCOL_GENIC		=	2,
};

enum nan_attr_with_wfa_srvc_info {
	NAN_WFA_SRVC_ATTR_SEDA		=	1,
	NAN_WFA_SRVC_ATTR_NDPE		=	2,
};

enum nan_generic_srvc_sub_attr_id {
	NAN_GEN_SRVC_TRANSPORT_PORT	=	0x0,
	NAN_GEN_SRVC_TRANSPORT_PROTOCOL	=	0x1,
	NAN_GEN_SRVC_SERVICE_NAME	=	0x2,
	NAN_GEN_SRVC_NAME_OF_SRVC_INST	=	0x3,
	NAN_GEN_SRVC_TEXT_INFO		=	0x4,
	NAN_GEN_SRVC_UUID		=	0x5,
	NAN_GEN_SRVC_BLOB		=	0x6,
	NAN_GEN_SRVC_RESERVD		=	0x7,
	NAN_GEN_SRVC_VENDOR_SPEC_INFO	=	0xDD,
};

enum nan_tx_follow_up_type {
	NAN_TX_FOLLOW_UP_NONE,
	NAN_TX_FOLLOW_UP_GENERAL,
	NAN_TX_FOLLOW_UP_BSTRP,
	NAN_TX_FOLLOW_UP_REQ,
	NAN_TX_FOLLOW_UP_RSP,
	NAN_TX_FOLLOW_UP_MAX
};

struct rtw_phl_nan_qos_req {
	u8 min_duration;
	u16 max_latency;
	u8 type;
};

struct rtw_phl_nan_srvc_ext_info {
	u8 updating;
	/* indicate when cmd srv_ext_info is called and
	 * these info need to update to the adding service in rtw_add_nan_func_xxx
	 */
	u8 period; /* (pub/sub) announcement_period for publish or query_period for subscribe */
	u8 event_condition; /* (pub) determine when publish related events are generated; nan_publish_event_condition_flags */
	u8 proximity_flag; /* (pub/sub)  discovery range; not implement */
	u8 awake_dw_int; /* (pub/sub)  Interval between two DWs during which the device supporting the service is awake to transmit or receive */
	u8 further_srvc_disc; /* (pub) more frames will come with this service */
	u8 further_srvc_disc_func; /* (pub) 0: followup, 1 : GAS */
	u8 data_path_flag; /* (pub) NDP setup is required */
	u8 data_path_type; /* (pub) 0: unicast */
	u8 qos; /* (pub) qos required */
	u8 sec; /* (pub) sec required */

	/*
	 * pmk_publish and pmkid_publish only use for publish
	 * and these two parameters follow specific service.
	 * pmkid_publish is calculated form pmk_publish and broadcast
	 * mac addr,and used in solicted/unsolicated publish SDF
	 */
	u8 passphrase[MAX_PASSPHRASE_LEN + 1];
	/* (pub) use to extend to pmk if set */
	u8 passphrase_len;
	u8 pmk_publish[MAX_PMK_LEN];
	/* (pub) use to derive to pmkid if set whether if passphrase set */
	u8 pmkid_publish[MAX_PMKID_LEN];
	/* (pub) */
	u8 gtk_protection;

	/*
	 * rtw_phl_nan_qos_req =>
	 * Unicast: data_path_type = 0 qos = 1
	 * Multicast: data_path_type = 1 qos = 1
	 */
	struct rtw_phl_nan_qos_req qos_req;
	/* range_config_param not support yet */

	u8 service_id[NAN_SRVC_ID_LEN]; /* (pub/sub) overwrite service info feild */
	u8 srvc_info[NAN_MAX_SERVICE_INFO_LEN];
	u16 srvc_info_len;
	u16 csid; /* represent in bitwise */
#ifdef CONFIG_NAN_PAIRING
	u8 pairing_setup_enable;
	u8 npk_nik_cache_enable;
	u16 bstrap_method;
#endif
};

struct rtw_phl_nan_data_req_info {
	/* CAPI parameters */
	u8 req_type;	/* 0 = ndp */
	u8 rsp_nan_mac[MAC_ALEN];
	u8 sec;
	/* spec parameters */
	u8 type;	/* 0 = ucast */
	u8 publish_id;
	u8 mcast_addr[MAC_ALEN];
	/*
	*	rtw_phl_nan_qos_req =>
	*	Unicast: data_path_type = 0 qos = 1
	*	Multicast: data_path_type = 1 qos = 1
	*/
	struct rtw_phl_nan_qos_req qos_req;

	/*
	   NAN_DATA_QOS_TYPE_ADAPTIVE	=	0,   // Slots&Channel will adapt based on traffic, channel utilization, etc
	   NAN_DATA_QOS_TYPE_TIER_1	=	1,   // (5/30) = 16% availability
	   NAN_DATA_QOS_TYPE_TIER_2	= 	2,   // (10/30) = 33% availability
	   NAN_DATA_QOS_TYPE_TIER_3	= 	3,   // (15/30) = 50% availability
	   NAN_DATA_QOS_TYPE_TIER_4	= 	4,   // (20/30) = 66% availability
	   NAN_DATA_QOS_TYPE_TIER_5	= 	5,   // (25/30) = 83% availability
	   NAN_DATA_QOS_TYPE_TIER_6	= 	6,   // 100% availability, No empty slots, all slots will have a valid channel
	   NAN_DATA_QOS_TYPE_REAL_TIME	=	NAN_DATA_QOS_TYPE_TIER_6,
	*/
	u8 mrole_sched_ratio;

	/* sec_config_param not implement yet */
	u8 srvc_info[NAN_MAX_SERVICE_INFO_LEN];
	u16 srvc_info_len;

	u8 sec_type; /* enum nan_pmk_setting */
	u8 sec_info[NAN_MAX_SECURITY_INFO_LEN];
	u8 sec_info_len;
};

struct rtw_phl_nan_data_rsp_info {
	/* CAPI parameters */
	u8 rsp_mode;
	u8 ndl_rsp;
	u8 data_path_id;
	u8 publish_id;
	u8 m4_rsp_type;
	/* spec parameters */
	u8 type;	/* 0 = ucast */
	u8 mc_id; 	/* mcast */
	u8 initiator_data_address[MAC_ALEN];	/* ucast */
	u8 mcast_addr[MAC_ALEN];
	/*
	*	rtw_phl_nan_qos_req =>
	*	Unicast: data_path_type = 0 qos = 1
	*	Multicast: data_path_type = 1 qos = 1
	*/
	struct rtw_phl_nan_qos_req qos_req;

	/*
	   NAN_DATA_QOS_TYPE_ADAPTIVE	=	0,   // Slots&Channel will adapt based on traffic, channel utilization, etc
	   NAN_DATA_QOS_TYPE_TIER_1	=	1,   // (5/30) = 16% availability
	   NAN_DATA_QOS_TYPE_TIER_2	= 	2,   // (10/30) = 33% availability
	   NAN_DATA_QOS_TYPE_TIER_3	= 	3,   // (15/30) = 50% availability
	   NAN_DATA_QOS_TYPE_TIER_4	= 	4,   // (20/30) = 66% availability
	   NAN_DATA_QOS_TYPE_TIER_5	= 	5,   // (25/30) = 83% availability
	   NAN_DATA_QOS_TYPE_TIER_6	= 	6,   // 100% availability, No empty slots, all slots will have a valid channel
	   NAN_DATA_QOS_TYPE_REAL_TIME	=	NAN_DATA_QOS_TYPE_TIER_6,
	*/
	u8 mrole_sched_ratio;

	/* sec_config_param not implement yet */
	u8 srvc_info[NAN_MAX_SERVICE_INFO_LEN];
	u16 srvc_info_len;
};

struct rtw_phl_nan_data_end_info {
	/* CAPI parameters */
	u8 ndp_id;
	u8 initiatorndi[MAC_ALEN];
	/* spec parameters */
	u8 type;	/* 0 = ucast */
	u8 status;
	u8 mc_id;	/* mcast */
	u8 nmsg_id;	/* mcast */
};
#endif /* CONFIG_NAN || CONFIG_PHL_NAN || CONFIG_NAN_APP */
#endif /* _PHL_NAN_DEF_IOCTL_H_ */
