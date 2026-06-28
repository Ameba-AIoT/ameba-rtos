#ifndef rtw_nan_cmd__h__
#define rtw_nan_cmd__h__

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

enum nan_generic_srvc_port_protocol {
	NAN_GEN_SRVC_PORT_PROTOCOL_TCP = 0x06,
	NAN_GEN_SRVC_PORT_PROTOCOL_UDP = 0x11,
};

enum vendor_tx_follow_up_type {
	NAN_TX_FOLLOW_UP_NONE,
	NAN_TX_FOLLOW_UP_GENERAL,
	NAN_TX_FOLLOW_UP_BSTRP,
	NAN_TX_FOLLOW_UP_REQ,
	NAN_TX_FOLLOW_UP_RSP,
	NAN_TX_FOLLOW_UP_MAX
};

struct vendor_tx_followup_info {
	uint8_t follow_up_type; // vendor_tx_follow_up_type
	uint8_t remote_inst_id;
	uint8_t local_inst_id;
	uint8_t remote_mac[ETH_ALEN];
	/* general */
	// todo: buf (check iw r1/r2)
	/* bootstrapping param */
	uint8_t bstrap_type_status;
	uint8_t bstrap_reason;
	uint16_t bstrap_method;
	uint8_t sec_key_type;
};

struct vendor_pairing_setpw {
	uint8_t peer_nmi[ETH_ALEN];
	char pw[MAX_PASN_PW_LEN + 1];
	uint8_t pw_len;
};

struct vendor_pairing_start {
	uint8_t role;
	uint8_t type;
	uint8_t peer_nmi[ETH_ALEN];
	uint8_t auth_method;
	uint8_t nik_caching;
};

#endif
