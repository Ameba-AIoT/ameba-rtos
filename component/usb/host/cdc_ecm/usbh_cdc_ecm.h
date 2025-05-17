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

/* ecm callback will call lwip APIs, they are in one task */
#define ECM_LWIP_TASK_IN_COUPLE                                 1

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
#if !ECM_LWIP_TASK_IN_COUPLE
#define USBH_CDC_ECM_BULK_BUF_MAX_CNT                           (20)
#endif

/* CDC Class Codes */
#define CDC_CLASS_CODE                                          0x02U
#define CDC_COMM_INTERFACE_CLASS_CODE                           0x02U
#define CDC_DATA_INTERFACE_CLASS_CODE                           0x0AU

/* CDC Communication sub class codes */
#define CDC_RESERVED                                            0x00U
#define CDC_ETHERNET_NETWORK_CONTROL_MODEL                      0x06U

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
#define CDC_ECM_NETWORK_FUNC_DESCRIPTOR                         0x0FU

/* Exported types ------------------------------------------------------------*/
typedef int (*usb_timer_func)(void);

/* USB Host Status */
typedef enum {
	USBH_CDC_ECM_TYPE_INTR = 0U,
	USBH_CDC_ECM_TYPE_BULK_IN,
	USBH_CDC_ECM_TYPE_BULK_OUT,
	USBH_CDC_ECM_TYPE_MAX,
} usbh_ecm_xter_type_t;

typedef struct {
	u8 pipe_id;
	usbh_ecm_xter_type_t type;
	u32 check_interval;   //const
	u32 last_check_time;  //last update time
	usb_timer_func func;
} usbh_cdc_ecm_time_t;

/* USB Host Status */
typedef enum {
	USBH_CDC_ECM_IDLE = 0U,
	USBH_CDC_ECM_DETACHED,
	USBH_CDC_ECM_ATTACH,
	USBH_CDC_ECM_SETUP,
	USBH_CDC_ECM_MAX,
} usbh_ecm_state_t;

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
	u32 ULBitRate;    /* Contains the uplink bit rate, in bits per second, as sent on the OUT pipe */
} usbh_cdc_ecm_speed_change_t;

/* States for CDC State Machine */
typedef enum {
	CDC_ECM_TRANSFER_STATE_IDLE = 0U,
	CDC_ECM_TRANSFER_STATE_XFER,
	CDC_ECM_TRANSFER_STATE_BUSY,
	CDC_ECM_TRANSFER_STATE_ERROR,
} usbh_cdc_ecm_transfer_state_t;

/* CDC ECM state */
typedef enum {
	CDC_ECM_STATE_CTRL_IDLE = 0U,
	CDC_ECM_STATE_CTRL_ALT_SETTING,
#if ECM_ENABLE_PACKETFILTER
	CDC_ECM_STATE_CTRL_SET_ETHERNET_MULTICAST_FILTER,
	CDC_ECM_STATE_CTRL_SET_ETHERNET_PACKET_FILTER,
	CDC_ECM_STATE_CTRL_GET_ETHERNET_STATISTIC,
#endif

#if ECM_ENABLE_RCR_CONFIGURATION
	CDC_ECM_STATE_CTRL_RCR_GET, //RTL8156,
	CDC_ECM_STATE_CTRL_RCR_SET,
#endif

#if ECM_ENABLE_FIFO_FLOW_CTRL
	CDC_ECM_STATE_FLOW_CTRL1, //RTL8152,
	CDC_ECM_STATE_FLOW_CTRL2,
#endif

	//RTL8152, set mac address
	CDC_ECM_STATE_CTRL_MAC_GET_LOCK,
	CDC_ECM_STATE_CTRL_MAC_DISABLE_LOCK,
	CDC_ECM_STATE_CTRL_MAC_SET_MAC1,
	CDC_ECM_STATE_CTRL_MAC_SET_MAC2,
	CDC_ECM_STATE_CTRL_MAC_ENABLE_LOCK,

	CDC_ECM_STATE_CTRL_LED_COLOR_SET,

	CDC_ECM_STATE_CTRL_MAX,
} usbh_cdc_ecm_ctrl_state_t;

/* CDC ECM state */
typedef enum {
	CDC_ECM_STATE_IDLE = 0U,
	CDC_ECM_STATE_PRE_SETTING,
	CDC_ECM_STATE_TRANSFER,
	CDC_ECM_STATE_ERROR,
} usbh_cdc_ecm_state_t;

typedef enum {
	CDC_ECM_MAC_UNINIT = 0U,
	CDC_ECM_MAC_DONGLE_SUPPLY,
	CDC_ECM_MAC_UPPER_LAYER_SET,
	CDC_ECM_MAC_RANDOM_SET,
	CDC_ECM_MAC_TYPE_MAX,
} usbh_cdc_ecm_dongle_mac_type_t;


/* CDC ECM communication interface */
typedef struct {
	u32 intr_ep_interval;
	u16 intr_in_packet_size;
	u8  intr_in_pipe;
	u8  intr_in_ep;
} usbh_cdc_ecm_comm_if_t ;

/* CDC ECM data interface */
typedef struct {
	u16 bulk_in_packet_size;
	u16 bulk_out_packet_size;
	u8  bulk_in_pipe;
	u8  bulk_in_ep;
	u8  bulk_out_pipe;
	u8  bulk_out_ep;
} usbh_cdc_ecm_data_if_t ;

/*
	Ethernet Networking Functional Descriptor
	CDC ECM basic struction
*/
typedef struct {
	u8  bLength;
	u8  bDescriptorType;
	u8  bDescriptorSubtype;                              /* Ethernet Networking functional descriptor subtype */
	u8  iMACAddress;                                     /* Index of string descriptor */
	u32 bmEthernetStatistics;                            /* Indicates which Ethernet statistics functions the device collects. */
	u8  wMaxSegmentSize;                                 /* The maximum segment size that the Ethernet device is capable of supporting */
	u16 wNumberMCFilters;                                /* Contains the number of multicast filters that can be configured by the host */
	u8  bNumberPowerFilters;                             /* Contains the number of pattern filters that are available for causing wake-up of the host. */
} usbh_cdc_ecm_network_func_t ;

/* CDC ECM user callback interface */
typedef struct {
	int(* init)(void);
	int(* deinit)(void);
	int(* attach)(void);
	int(* detach)(void);
	int(* setup)(void);
#if ECM_LWIP_TASK_IN_COUPLE
	int(* bulk_received)(u8 *buf, u32 len);
#endif
	int(* bulk_send)(usbh_urb_state_t state);
	int(* intr_received)(u8 *buf, u32 len);
} usbh_cdc_ecm_state_cb_t;

#if !ECM_LWIP_TASK_IN_COUPLE
struct _usbh_cdc_ecm_buf_t;
typedef struct _usbh_cdc_ecm_buf_t {
	struct _usbh_cdc_ecm_buf_t *next;
	u8 *buf_raw;
	__IO u16 buf_valid_len;
} usbh_cdc_ecm_buf_t;

typedef struct {
	usbh_cdc_ecm_buf_t *head;
	usbh_cdc_ecm_buf_t *tail;
	volatile u16 count;
	usb_os_sema_t list_sema;
} usbh_cdc_ecm_buf_list_t;

typedef struct {
	/* ecm data ringbuf issue  */
	usbh_cdc_ecm_buf_list_t empty_list;
	usbh_cdc_ecm_buf_list_t data_list;
	usbh_cdc_ecm_buf_t *buf_list_node;
	usbh_cdc_ecm_buf_t *p_cur_buf_node;
	u8 *ecm_rx_buf;

	usb_os_sema_t ecm_rx_sema;

	__IO u8 ecm_sema_valid : 1;
	__IO u8 read_wait_sema : 1;

	__IO u8 read_continue : 1;
} usbh_cdc_ecm_rx_buf_list_t;
#endif

/* CDC ECM host */
typedef struct {
	usbh_cdc_ecm_comm_if_t              comm_if;          /* intr ep struct*/
	usbh_cdc_ecm_data_if_t              data_if;          /* bulk ep struct*/
	usbh_cdc_ecm_network_func_t         func_if;          /* network struct*/
	usbh_cdc_ecm_speed_change_t         user_speed_info;  /* ecm dongle speed struct*/

	/* array */
	u8                                  host_pipe[USB_MAX_PIPES];      /*  */
	u8                                  muticast_filter[CDC_ECM_MUTICAST_FILTER_STR_LEN];
	u8                                  mac[CDC_ECM_MAC_STR_LEN];                     /* mac info */
	u8                                  mac_ctrl_lock[CDC_ECM_MAC_CTRL_REG_LEN];      /* for 8152 change mac */
#if ECM_ENABLE_FIFO_FLOW_CTRL
	u8                                  flow_ctrl[CDC_ECM_MAC_CTRL_REG_LEN];          /* 8152 */
#endif
#if ECM_ENABLE_RCR_CONFIGURATION
	u8                                  rcr[CDC_ECM_MAC_CTRL_REG_LEN];                /* 8156 */
#endif

#if ECM_LWIP_TASK_IN_COUPLE
	u8                                  *bulk_data_in_buf;    /* bulk in buffer */
#else
	usbh_cdc_ecm_rx_buf_list_t          rx_list;
#endif

	/* u32 */
	usbh_cdc_ecm_state_cb_t             *cb;
	usb_host_t                          *host;
	u16                                 *led_array;           /* led array */

	u8                                  *dongle_ctrl_buf;     /* used for transfer, cache line align */

	u8                                  *intr_in_buf;         /* intr in buffer */
	u8                                  *bulk_data_out_buf;   /* bulk out buffer */

	u32                                 eth_statistic_count;  /* feature select params */
	volatile u32                        intr_in_busy_tick;    /* intr in busy tick */
	volatile u32                        intr_in_idle_tick;    /* intr in idle tick*/

	volatile u32                        bulk_data_out_len;    /* bluk out data length */
	volatile u32                        bulk_out_idle_tick;   /* bulk out idle tick */

	volatile u32                        bulk_in_busy_tick;    /* bulk in busy tick */
	volatile u32                        bulk_in_idle_tick;    /* bulk in idle tick */

#if ECM_STATE_DEBUG_ENABLE
	volatile u32                        bulk_in;
	volatile u32                        bulk_out;
	volatile u32                        intr_in;
#endif

	/* u16 */
	u16                                 vid;
	u16                                 pid;
	u16                                 feature_selector;    /* feature select params */
	u16                                 packet_filter;       /* packet filter params */
	u16                                 muticast_filter_len; /* multicast filter params length */

	/* u8 */
	u8                                  led_cnt;             /* led cnt */
	u8                                  mac_valid;           /* mac valid */
	u8                                  mac_src_type;        /* ecm dongle mac source type : usbh_cdc_ecm_dongle_mac_type_t */
	u8                                  state;               /*usb process status : usbh_cdc_ecm_state_t*/
	u8                                  sub_state;           /*usb ctrl process status : usbh_cdc_ecm_ctrl_state_t*/
	u8                                  usbh_state;          /*usb host connect status : usbh_ecm_state_t*/
	u8                                  bulk_data_in_state;  /*bulk in  transfer status : usbh_cdc_ecm_transfer_state_t*/
	u8                                  bulk_data_out_state; /*bulk out transfer status : usbh_cdc_ecm_transfer_state_t*/
	u8                                  intr_in_state;       /*Intr in  transfer status : usbh_cdc_ecm_transfer_state_t*/
	u8                                  bulk_out_zlp;        /*bulk out packet length is multiple of MPS,should send ZLP*/
	u8                                  next_transfor;       /*send next event flag*/
} usbh_cdc_ecm_host_t;


/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
int usbh_cdc_ecm_init(usbh_cdc_ecm_state_cb_t *cb);
int usbh_cdc_ecm_deinit(void);

int usbh_cdc_ecm_choose_config(usb_host_t *host);

int usbh_cdc_ecm_bulk_send(u8 *buf, u32 len);

#if !ECM_LWIP_TASK_IN_COUPLE
usbh_cdc_ecm_buf_t *usbh_cdc_ecm_read(u32 time_out_ms);
void usbh_cdc_ecm_read_done(usbh_cdc_ecm_buf_t *p_buf);
#endif
u32 usbh_cdc_ecm_get_read_frame_cnt(void);

u16 usbh_cdc_ecm_get_usbin_mps(void);
u32 usbh_cdc_ecm_get_intr_interval(void);

void usbh_cdc_ecm_set_dongle_led_array(u16 *led, u8 len);
void usbh_cdc_ecm_set_dongle_mac(u8 *mac);

usbh_cdc_ecm_time_t *usbh_ecm_get_timer_handle(u8 idx);

#endif  /* USBD_CDC_ECM_H */
