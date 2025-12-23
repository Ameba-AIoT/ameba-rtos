/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_CDC_ECM_H
#define USBH_CDC_ECM_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"
#include "usbh_cdc_ecm_hal.h"

/* Macro defines -----------------------------------------------------------*/
#define ECM_ENABLE_PACKETFILTER                                 0

/* for RTL8156BG, use this to allow ping/UDP data transfer */
#define ECM_ENABLE_RCR_CONFIGURATION                            1

/* for RTL8152, config the fifo flow control for data transfer */
#define ECM_ENABLE_FIFO_FLOW_CTRL                               1

/* ecm ethernet connect status check */
#define USBH_ECM_ETH_STATUS_CHECK                              500U  //ms

/* Exported defines ----------------------------------------------------------*/

#define CDC_ECM_MAC_STRING_LEN                                  (32)
#define CDC_ECM_MAC_STR_LEN                                     (6)  /* mac[6] */
#define CDC_ECM_MAC_CTRL_REG_LEN                                (4)
#define CDC_ECM_MUTICAST_FILTER_STR_LEN                         (20)

#define USBH_CDC_ECM_BULK_BUF_MAX_SIZE                          (512*3)

/* CDC Class&Subclass Codes */
#define CDC_IF_CDC_CTRL_CODE                            0x02U
#define CDC_IF_CDC_CTRL_SUB_CLASS_ECM_CODE              0x06U
#define CDC_IF_CDC_DATA_CODE                            0x0AU
#define CDC_IF_CDC_DATA_SUB_CLASS_DATA_CODE             0x00U

/* Table 6: Class-Specific Request Codes for Ethernet subclass */
#define CDC_ECM_SET_ETHERNET_MULTICAST_FILTERS                  0x40U
#define CDC_ECM_SET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER    0x41U
#define CDC_ECM_GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER    0x42U
#define CDC_ECM_SET_ETHERNET_PACKET_FILTER                      0x43U
#define CDC_ECM_GET_ETHERNET_STATISTIC                          0x44U
#define CDC_ECM_SPECIAL_REQUEST_CODE_RESERVED                   0x45U

/* [CDC]Table 20: Class-Specific Notification Codes */
/* Table 11: Class-Specific Notification Codes for Ethernet subclass */
#define CDC_ECM_NOTIFY_NETWORK_CONNECTION                       0x00U
#define CDC_ECM_NOTIFY_RESPONSE_AVAILABLE                       0x01U
#define CDC_ECM_NOTIFY_CONNECTION_SPEED_CHANGE                  0x2AU

/* Communication Interface Class Control Protocol Codes */
#define CDC_CTRL_PROTOCOL_NO_CLASS_SPECIFIC                     0x00U

/*  bDescriptor SubType in Communications Class Functional Descriptors  */
#define CDC_ECM_NETWORK_INTERFACE                               0x24
#define CDC_ECM_NETWORK_FUNC_DESCRIPTOR                         0x0FU

/* Exported types ------------------------------------------------------------*/

#define  USBH_ECM_FREE_MEM(x)  if(x){ usb_os_mfree(x); x = NULL;}

typedef enum {
	CDC_ECM_MAC_UNINIT = 0U,
	CDC_ECM_MAC_DONGLE_SUPPLY,
	CDC_ECM_MAC_UPPER_LAYER_SET,
	CDC_ECM_MAC_RANDOM_SET,
	CDC_ECM_MAC_TYPE_MAX,
} usbh_cdc_ecm_dongle_mac_type_t;

/* USB Host Status */
typedef enum {
	USBH_CDC_ECM_TYPE_UNDEF = 0U,
	USBH_CDC_ECM_TYPE_INTR,
	USBH_CDC_ECM_TYPE_BULK_IN,
	USBH_CDC_ECM_TYPE_BULK_OUT, //ecm
	USBH_CDC_ECM_APP_TYPE_INTR,
	USBH_CDC_ECM_APP_TYPE_BULK_IN,
	USBH_CDC_ECM_APP_TYPE_BULK_OUT, //acm
	USBH_CDC_ECM_TYPE_MAX,
} usbh_ecm_xter_type_t;

typedef enum {
	CDC_ECM_STATE_AT_SETTING_IDLE = 0U,
	CDC_ECM_STATE_GET_MAC_STR,
	CDC_ECM_STATE_CTRL_ALT_SETTING,

#if ECM_ENABLE_PACKETFILTER
	CDC_ECM_STATE_CTRL_SET_ETHERNET_MULTICAST_FILTER,
	CDC_ECM_STATE_CTRL_SET_ETHERNET_PACKET_FILTER,
	CDC_ECM_STATE_CTRL_GET_ETHERNET_STATISTIC,
#endif

	CDC_ECM_STATE_CTRL_RCR_GET, //RTL8156,
	CDC_ECM_STATE_CTRL_RCR_SET,
	CDC_ECM_STATE_FLOW_CTRL1, //RTL8152,
	CDC_ECM_STATE_FLOW_CTRL2,

	/*RTL8152, set mac address */
	CDC_ECM_STATE_CTRL_MAC_GET_LOCK,
	CDC_ECM_STATE_CTRL_MAC_DISABLE_LOCK,
	CDC_ECM_STATE_CTRL_MAC_SET_MAC1,
	CDC_ECM_STATE_CTRL_MAC_SET_MAC2,
	CDC_ECM_STATE_CTRL_MAC_ENABLE_LOCK,
	/*RTL8152, set led color */
	CDC_ECM_STATE_CTRL_LED_COLOR_SET,

	CDC_ECM_STATE_AT_SETTING_MAX,
} usbh_cdc_ecm_at_set_state_t;

typedef struct {
	u8 pipe_id;
	usbh_ecm_xter_type_t type;
	u32 check_interval;
	u32 last_check_time;  //last update time
	usb_timer_func func;
} usbh_cdc_ecm_time_t;

/* USB Host Status */

/* CDC ECM Ethernet Packet Filter Bitmap  */
typedef enum {
	CDC_ECM_ETH_PACKET_TYPE_PROMISCUOUS     = 0x01U, /* ALL frames received by the networking device are forwarded up to the host (required) */
	CDC_ECM_ETH_PACKET_TYPE_ALL_MULTICAST   = 0x02U, /* ALL multicast frames received by the networking device are forwarded up to the host, not just
                                                        the ones enumerated in the device's multicast address list (required) */
	CDC_ECM_ETH_PACKET_TYPE_DIRECTED        = 0x04U, /* Directed packets received containing a destination address equal to the MAC address of the
                                                        networking device are forwarded up to the host (required) */
	CDC_ECM_ETH_PACKET_TYPE_BROADCAST       = 0x08U, /* All broadcast packets received by the networking device are forwarded up to the host. */
	CDC_ECM_ETH_PACKET_TYPE_MULTICAST       = 0x10U, /* All multicast packets enumerated in the device's multicast address list are forwarded up to the host */
	CDC_ECM_ETH_PACKET_TYPE_MAX,
} usbh_cdc_ecm_eth_packet_filter_bitmap_t;

/* CDC ECM Ethernet Statistics Feature Selector Codes */
typedef enum {
	CDC_ECM_FEATURE_SELECTOR_RESERVED = 0U,
	CDC_ECM_FEATURE_SELECTOR_XMIT_OK,
	CDC_ECM_FEATURE_SELECTOR_RCV_OK,
	CDC_ECM_FEATURE_SELECTOR_XMIT_ERROR,
	CDC_ECM_FEATURE_SELECTOR_RCV_ERROR,
	CDC_ECM_FEATURE_SELECTOR_RCV_NO_BUFFER,
	CDC_ECM_FEATURE_SELECTOR_DIRECTED_BYTES_XMIT,
	CDC_ECM_FEATURE_SELECTOR_DIRECTED_FRAMES_XMIT,
	CDC_ECM_FEATURE_SELECTOR_MULTICAST_BYTES_XMIT,
	CDC_ECM_FEATURE_SELECTOR_MULTICAST_FRAMES_XMIT,
	CDC_ECM_FEATURE_SELECTOR_BROADCAST_BYTES_XMIT,
	CDC_ECM_FEATURE_SELECTOR_BROADCAST_FRAMES_XMIT,
	CDC_ECM_FEATURE_SELECTOR_DIRECTED_BYTES_RCV,
	CDC_ECM_FEATURE_SELECTOR_DIRECTED_FRAMES_RCV,
	CDC_ECM_FEATURE_SELECTOR_MULTICAST_BYTES_RCV,
	CDC_ECM_FEATURE_SELECTOR_MULTICAST_FRAMES_RCV,
	CDC_ECM_FEATURE_SELECTOR_BROADCAST_BYTES_RCV,
	CDC_ECM_FEATURE_SELECTOR_BROADCAST_FRAMES_RCV,
	CDC_ECM_FEATURE_SELECTOR_RCV_CRC_ERROR,
	CDC_ECM_FEATURE_SELECTOR_TRANSMIT_QUEUE_LENGTH,
	CDC_ECM_FEATURE_SELECTOR_RCV_ERROR_ALIGNMENT,
	CDC_ECM_FEATURE_SELECTOR_XMIT_ONE_COLLISION,
	CDC_ECM_FEATURE_SELECTOR_XMIT_MORE_COLLISIONS,
	CDC_ECM_FEATURE_SELECTOR_XMIT_DEFERRED,
	CDC_ECM_FEATURE_SELECTOR_XMIT_MAX_COLLISIONS,
	CDC_ECM_FEATURE_SELECTOR_RCV_OVERRUN,
	CDC_ECM_FEATURE_SELECTOR_XMIT_UNDERRUN,
	CDC_ECM_FEATURE_SELECTOR_XMIT_HEARTBEAT_FAILURE,
	CDC_ECM_FEATURE_SELECTOR_XMIT_TIMES_CRS_LOST,
	CDC_ECM_FEATURE_SELECTOR_XMIT_LATE_COLLISIONS,
	CDC_ECM_FEATURE_SELECTOR_MAX,
} usbh_cdc_ecm_statistic_feature_selector_t;

/* ConnectionSpeedChange Data Structure */
typedef union {
	u32 DLBitRate;      /* Contains the downlink bit rate, in bits per second, as sent on the IN pipe */
	u32 ULBitRate;      /* Contains the uplink bit rate, in bits per second, as sent on the OUT pipe */
} usbh_cdc_ecm_speed_change_t;

/* CDC ECM state */
typedef enum {
	CDC_ECM_STATE_IDLE = 0U,
	CDC_ECM_STATE_PRE_SETTING,
	CDC_ECM_STATE_TRANSFER,
	CDC_ECM_STATE_ERROR,
} usbh_cdc_ecm_state_t;

/*
	Ethernet Networking Functional Descriptor
	CDC ECM basic struction
*/
typedef struct {
	u8 bLength;
	u8 bDescriptorType;
	u8 bDescriptorSubtype;        /* Ethernet Networking functional descriptor subtype */
	u8 iMACAddress;               /* Index of string descriptor */
	u32 bmEthernetStatistics;     /* Indicates which Ethernet statistics functions the device collects. */
	u8 wMaxSegmentSize;           /* The maximum segment size that the Ethernet device is capable of supporting */
	u16 wNumberMCFilters;         /* Contains the number of multicast filters that can be configured by the host */
	u8 bNumberPowerFilters;       /* Contains the number of pattern filters that are available for causing wake-up of the host. */
} __PACKED usbh_cdc_ecm_network_func_t;

/* CDC ECM user callback interface */
typedef struct {
	int(* init)(void);
	int(* deinit)(void);
	int(* attach)(void);
	int(* detach)(void);
	int(* setup)(void);
	int(* bulk_received)(u8 *buf, u32 len);
	int(* bulk_send)(usbh_urb_state_t state);
	int(* intr_received)(u8 *buf, u32 len);
} usbh_cdc_ecm_state_cb_t;

typedef struct {
	usbh_pipe_t pipe;
	usbh_ep_desc_t ep_desc;
	u8 *buf; /* malloc buffer for rx */
	u32 buf_len;
	u32 busy_tick;
#if ECM_STATE_DEBUG_ENABLE
	u32 trigger_cnt;
#endif
	u8 valid;
} usbh_cdc_ecm_pipe_info_t;

/* CDC ECM host */
typedef struct {
	u8 muticast_filter[CDC_ECM_MUTICAST_FILTER_STR_LEN];
	u8 mac[CDC_ECM_MAC_STR_LEN];
	u8 mac_ctrl_lock[CDC_ECM_MAC_CTRL_REG_LEN];      /* for 8152 change mac */
	u8 flow_ctrl[CDC_ECM_MAC_CTRL_REG_LEN];          /* 8152 */
	u8 rcr[CDC_ECM_MAC_CTRL_REG_LEN];                /* 8156 */

	usbh_cdc_ecm_pipe_info_t ecm_ctrl_ep;
	usbh_cdc_ecm_pipe_info_t ecm_tx_ep;
	usbh_cdc_ecm_pipe_info_t ecm_rx_ep;

	usbh_cdc_ecm_state_cb_t  *cb;
	usb_host_t *host;
	usbh_cdc_ecm_at_set_state_t   sub_status;
	u16 *led_array;           /* led array */
	u8  *dongle_ctrl_buf;     /* used for transfer,cache align */
	u32 eth_statistic_count;  /* feature select params */
	u16 feature_selector;     /* feature select params */
	u16 packet_filter;        /* packet filter params */
	u16 muticast_filter_len;  /* multicast filter params length */
	u16 vid;
	u16 pid;
	u8 data_itf_id;           /* Data Interface number */
	u8 data_alt_set;          /* Value used to select alternative setting */
	u8 iMACAddressStringId;   /* get the mac address */
	u8 state;                 /* usb process status, @ref usbh_cdc_ecm_state_t */
	u8 next_xfer;             /* send next event flag */
	u8 led_cnt;               /* led cnt */
	u8 mac_valid;
	u8 mac_src_type;          /* ecm dongle mac source type, @ref usbh_cdc_ecm_dongle_mac_type_t */
} usbh_cdc_ecm_host_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
int usbh_cdc_ecm_init(usbh_cdc_ecm_state_cb_t *cb, usbh_cdc_ecm_priv_data_t *priv);
int usbh_cdc_ecm_deinit(void);
int usbh_ecm_timer_register(u8 pipe, u32 time_value, usb_timer_func fn, u8 type);

int usbh_cdc_ecm_choose_config(usb_host_t *host);
int usbh_cdc_ecm_check_config_desc(usb_host_t *host, u8 cfg_max);

int usbh_cdc_ecm_check_enum_status(void);

int usbh_cdc_ecm_bulk_send(u8 *buf, u32 len);

u16 usbh_cdc_ecm_get_usbin_mps(void);
u32 usbh_cdc_ecm_get_intr_interval(void);
u16 usbh_cdc_ecm_get_device_vid_info(void);
u16 usbh_cdc_ecm_get_device_pid_info(void);

#endif  /* USBD_CDC_ECM_H */
