#ifndef _RTK_COEX_H_
#define _RTK_COEX_H_

#include <dlist.h>

#define HCI_NONE    0x00
#define HCI_CMD     0x01
#define HCI_ACL     0x02
#define HCI_SCO     0x03
#define HCI_EVT     0x04
#define HCI_ISO     0x05

#define ACL_CONN     0x0
#define SYNC_CONN    0x1
#define LE_CONN      0x2

#define DIR_IN     0
#define DIR_OUT    1

#define A2DP_SIGNAL    0x01
#define A2DP_MEDIA     0x02

#define BT_COEX_MONITOR_INTERVAL    1000
#define BT_LE_BUSY_CONN_INTERVAL    0x10    //20ms, units: 1.25ms
#define BT_HID_BUSY_INTERVAL        60  //units: 1.25ms

#define L2CAP_CONN_REQ       0x02
#define L2CAP_CONN_RSP       0x03
#define L2CAP_DISCONN_REQ    0x06
#define L2CAP_DISCONN_RSP    0x07

#define HCI_VENDOR_SET_PROFILE_REPORT_COMMAND    0xFC1B
#define HCI_VENDOR_SET_BITPOOL                   0xFC51
#define HCI_VENDOR_MAILBOX_CMD                   0xFC8F

#define HCI_EV_CONN_COMPLETE                0x03
#define HCI_EV_CMD_COMPLETE                 0x0E
#define HCI_EV_SYNC_CONN_COMPLETE           0x2c
#define HCI_EV_DISCONN_COMPLETE             0x05
#define HCI_EV_MODE_CHANGE                  0x14
#define HCI_EV_LE_META                      0x3e
#define HCI_EV_LE_CONN_COMPLETE             0x01
#define HCI_EV_LE_ENHANCED_CONN_COMPLETE    0x0a
#define HCI_EV_LE_CONN_UPDATE_COMPLETE      0x03
#define HCI_EV_LE_CIS_EST                   0x19
#define HCI_EV_LE_CREATE_BIG_CPL            0x1b
#define HCI_EV_LE_TERM_BIG_CPL              0x1c
#define HCI_EV_LE_BIG_SYNC_EST              0x1d
#define HCI_EV_LE_BIG_SYNC_LST              0x1e

#define HCI_EV_VENDOR_SPECIFIC              0xFF

#define PSM_SDP     0x0001
#define PSM_RFCOMM  0x0003
#define PSM_PAN     0x000F
#define PSM_HID     0x0011
#define PSM_HID_INT 0x0013
#define PSM_AVCTP   0x0017
#define PSM_AVDTP   0x0019
#define PSM_FTP     0x1001
#define PSM_BIP     0x1003
#define PSM_OPP     0x1005

#define CONFIG_BT_COEX_DEBUG 0

#if defined(CONFIG_BT_COEX_DEBUG) && CONFIG_BT_COEX_DEBUG
#include "bt_debug.h"
#define DBG_BT_COEX(...) BT_LOGA("[BT_COEX] " __VA_ARGS__)
#define DBG_BT_COEX_DUMP(_str, _buf, _len) BT_DUMPA("[BT_COEX] " _str, _buf, _len)
#else
#define DBG_BT_COEX(...) {}
#define DBG_BT_COEX_DUMP(_str, _buf, _len) {}
#endif /* CONFIG_BT_COEX_DEBUG */


enum __hci_conn_type {
	HCI_CONN_TYPE_L2CAP = 0,
	HCI_CONN_TYPE_SCO_ESCO  = 1,
	HCI_CONN_TYPE_LE    = 2,
	HCI_CONN_TYPE_CIS   = 4,
	HCI_CONN_TYPE_BIS   = 5,
};

enum __profile_type {
	PROFILE_SCO = 0,
	PROFILE_HID = 1,
	PROFILE_A2DP = 2,
	PROFILE_PAN = 3,
	PROFILE_HID_INTERVAL = 4, /* hid interval */
	PROFILE_HOGP = 5,
	PROFILE_VOICE = 6,
	PROFILE_SINK = 7,
	PROFILE_LE_AUDIO = 8,
	PROFILE_MOUSE = 9,
	PROFILE_MAX = 10
};


enum rtk_coex_mailbox_cmd {
	RTK_COEX_MAILBOX_BT_FW_PATCH                        = 0x0a,
	RTK_COEX_MAILBOX_WL_OP_MODE                         = 0x11,
	RTK_COEX_MAILBOX_WIFI_RF_CALIBRATION_EN             = 0x15,
	RTK_COEX_MAILBOX_FORCE_BT_TXPWR                     = 0x17,
	RTK_COEX_MAILBOX_BT_IGNORE_WLAN_ACT                 = 0x1b,
	RTK_COEX_MAILBOX_PSD_CONTROL                        = 0x21,
	RTK_COEX_MAILBOX_BT_INFO_REPORT                     = 0x23,
	RTK_COEX_MAILBOX_BT_PSD_REPORT                      = 0x24,
	RTK_COEX_MAILBOX_BT_INFO_REPORT_BY_ITSELF           = 0x27,
	RTK_COEX_MAILBOX_BT_INFO_REPORT_CONTROL             = 0x28,
	RTK_COEX_MAILBOX_BT_SET_TXRETRY_REPORT_PARAMETER    = 0x29,
	RTK_COEX_MAILBOX_BT_SET_PTA_TABLE                   = 0x2A,
	RTK_COEX_MAILBOX_BT_MP_REPORT                       = 0x30,
	RTK_COEX_MAILBOX_BT_PSD_MODE                        = 0x31,
	RTK_COEX_MAILBOX_BT_SET_BT_LNA_CONSTRAINT           = 0x32,
	RTK_COEX_MAILBOX_BT_LOOPBACK                        = 0x33,
	RTK_COEX_MAILBOX_BT_CALIBRATION                     = 0x34,
	RTK_COEX_MAILBOX_INTERNAL_SWITCH                    = 0x35,
	RTK_COEX_MAILBOX_BT_CHECK_WLAN_STATE                = 0x36,
	RTK_COEX_MAILBOX_CHANGE_WIFI_RF_STATE               = 0x37,
	RTK_COEX_MAILBOX_BT_PAGE_SCAN_INTERVAL              = 0x38,
	RTK_COEX_MAILBOX_BT_AUTO_SLOT                       = 0x39,
	RTK_COEX_MAILBOX_MAILBOX_EN                         = 0x40,
	RTK_COEX_MAILBOX_BT_CALIBRATION_FAIL                = 0x41,
	RTK_COEX_MAILBOX_WIFI_TDMA_IN_BT_SLOT               = 0x42,
	RTK_COEX_MAILBOX_WIFI_RPT_TDMA                      = 0x43,
	RTK_COEX_MAILBOX_BT_LE_EXTRA_INFO_BY_ITSELF         = 0x46,
	RTK_COEX_MAILBOX_BT_LE_EXTRA_INFO                   = 0x47,
	RTK_COEX_MAILBOX_BT_SLOT_CTRL                       = 0x48
};

struct rtk_bt_coex_profile_info_t {
	struct list_head list;
	uint16_t psm;
	uint16_t dcid;
	uint16_t scid;
	uint16_t idx;
	uint8_t  flags;
};

struct rtk_bt_coex_conn_t {
	struct list_head list;
	uint16_t conn_handle;
	uint8_t type;       /* __hci_conn_type：0:l2cap, 1:sco/esco, 2:le */
	uint16_t profile_bitmap;
	uint16_t profile_status_bitmap;
	uint8_t  profile_refcount[PROFILE_MAX];
	struct list_head profile_list;
	uint32_t a2dp_cnt;
	uint32_t a2dp_pre_cnt;
	uint32_t pan_cnt;
	uint32_t pan_pre_cnt;
};

struct rtk_bt_coex_monitor_node_t {
	struct list_head list;
	struct rtk_bt_coex_conn_t *p_conn;
	uint16_t profile_idx;
	uint8_t b_first_add;
};

struct rtk_bt_coex_priv_t {
	struct list_head conn_list;
	struct list_head monitor_list;
	void *monitor_mutex;
	void *monitor_timer;
};

/* little endian */
struct sbc_frame_hdr {
	uint8_t syncword: 8;        /* Sync word */
	uint8_t subbands: 1;        /* Subbands, 0:4;1:8 */
	uint8_t allocation_method: 1;   /* Allocation method, 0:LOUDNESS;1:SNR */
	uint8_t channel_mode: 2;        /* Channel mode, 00:MONO(1channel);01:DUAL(2channels);10:STEREO(2channels);11:JOINT_STEREO(2channels) */
	uint8_t blocks: 2;      /* Blocks */
	uint8_t sampling_frequency: 2;  /* Sampling frequency, 00:16K;01:32K;10:44.1K;11:48K */
	uint8_t bitpool: 8;     /* Bitpool */
	uint8_t crc_check: 8;       /* CRC check */
};

struct rtp_header {
	unsigned cc: 4;
	unsigned x: 1;
	unsigned p: 1;
	unsigned v: 2;

	unsigned pt: 7;
	unsigned m: 1;

	uint16_t sequence_number;
	uint32_t timestamp;
	uint32_t ssrc;
	uint32_t csrc[0];
};


void bt_coex_init(void);
void bt_coex_process_rx_frame(uint8_t type, uint8_t *pdata, uint16_t len);
void bt_coex_process_tx_frame(uint8_t type, uint8_t *pdata, uint16_t len);
void bt_coex_deinit(void);
#endif  /* _RTK_COEX_H_ */
