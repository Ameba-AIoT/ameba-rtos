#ifndef _WIFI_P2P_SUPPLICANT_H_
#define _WIFI_P2P_SUPPLICANT_H_

#include <p2p/p2p.h>

#ifndef P2P_MAX_INITIAL_CONN_WAIT
/*
 * How many seconds to wait for initial 4-way handshake to get completed after
 * WPS provisioning step.
 */
#define P2P_MAX_INITIAL_CONN_WAIT 10
#endif

/** P2P device scanned (only basic information) */
#define P2P_EVENT_DEVICE_SCANNED "P2P-DEVICE-SCANNED "

/** P2P device found */
#define P2P_EVENT_DEVICE_FOUND "P2P-DEVICE-FOUND "

/** P2P device lost */
#define P2P_EVENT_DEVICE_LOST "P2P-DEVICE-LOST "

/** P2P find stopped */
#define P2P_EVENT_FIND_STOPPED "P2P-FIND-STOPPED "

/** A P2P device requested GO negotiation, but we were not ready to start the
 * negotiation */
#define P2P_EVENT_GO_NEG_REQUEST "P2P-GO-NEG-REQUEST "
#define P2P_EVENT_GO_NEG_SUCCESS "P2P-GO-NEG-SUCCESS "
#define P2P_EVENT_GO_NEG_FAILURE "P2P-GO-NEG-FAILURE "
#define P2P_EVENT_GROUP_FORMATION_SUCCESS "P2P-GROUP-FORMATION-SUCCESS "
#define P2P_EVENT_GROUP_FORMATION_FAILURE "P2P-GROUP-FORMATION-FAILURE "

/* P2P Group started or removed */
#define P2P_EVENT_GROUP_STARTED "P2P-GROUP-STARTED "
#define P2P_EVENT_GROUP_REMOVED "P2P-GROUP-REMOVED "

#define AP_STA_CONNECTED "AP-STA-CONNECTED "
#define AP_STA_DISCONNECTED "AP-STA-DISCONNECTED "

enum p2p_group_removal_reason {
	P2P_GROUP_REMOVAL_UNKNOWN,
	P2P_GROUP_REMOVAL_SILENT,
	P2P_GROUP_REMOVAL_FORMATION_FAILED,
	P2P_GROUP_REMOVAL_REQUESTED,
	P2P_GROUP_REMOVAL_IDLE_TIMEOUT,
	P2P_GROUP_REMOVAL_UNAVAILABLE,
	P2P_GROUP_REMOVAL_GO_ENDING_SESSION
};

struct p2p_context {
	struct p2p_data *p2p;
	struct p2p_group *group;

	struct timer_list group_formation_timer;

	u8 role;
	u8 go_dev_addr[ETH_ALEN];

	int persistent_reconnect;

	u8 roch_onging;
	rtos_sema_t roc_ready_sema;

	struct list_head p2p_scan_report_list;
	rtos_mutex_t scan_report_lock;

	xqueue_handle_t queue_for_p2p_nego;

	struct p2p_connect_params *connect_param;
};

struct p2p_scan_report_node {
	struct list_head			list;
	struct rtw_scan_result		*p_scanned_dev_info;
	u8							*ies;
	u32							ie_len;
	u8							role;
};

struct p2p_auto_go_params {
	struct rtw_ssid ssid;
	u8 *password;
	u8 password_len;
	char *dev_name;
	char *manufacturer;
	char *model_name;
	char *model_number;
	char *serial_number;
	u8 *pri_dev_type;
	u8 channel;
};

struct p2p_connect_params {
	u8 *dest;
	enum p2p_wps_method config_method;
	char *pin;
	int go_intent;
	u32 timeout_sec;
	u8 join: 1;
	u8 pd_before_go_neg: 1;
};

struct p2p_cmd_priv {
	struct	list_head	queue;
	rtos_mutex_t		lock;
	rtos_sema_t wakeup_sema;
	rtos_sema_t terminate_sema;

	u8 blocked: 1;
};

struct p2p_cmd_obj {
	struct list_head	list;
	u8	*parmbuf;
	void(*cmd_hdl)(u8 *parmbuf);
};

void wifi_p2p_rx_mgnt(u8 *evt_info);
int wifi_p2p_group_notify_assoc(u8 *buf, u16 buf_len);
int wifi_p2p_group_notify_disassoc(u8 *addr);
int wifi_p2p_init(u8 *dev_addr, u8 listen_ch, u8 op_ch, char *ssid_postfix);
void wifi_p2p_deinit(void);
void wifi_p2p_set_role(u32 role);
u8 wifi_p2p_check_role(u8 role);
int wifi_p2p_set_remain_on_ch(unsigned char wlan_idx, u8 enable);
void wifi_p2p_join_status_hdl(u8 *evt_info);
void wifi_p2p_channel_switch_ready(u8 *evt_info);
int wifi_p2p_start_auto_go(struct p2p_auto_go_params *param);
int wifi_p2p_drv_deinit(enum p2p_group_removal_reason removal_reason);
int wifi_p2p_disconnect(void);
void wifi_p2p_find(u32 timeout);
void wifi_p2p_show_peers(void);
void wifi_p2p_connect_cmd(struct p2p_connect_params *params);

#endif //_WIFI_P2P_SUPPLICANT_H_

