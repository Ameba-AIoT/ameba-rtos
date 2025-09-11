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

#define ECM_ENABLE_DUMP_DESCRIPYOT_PARSE                        0

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

/* CDC Class Codes */
#define CDC_CLASS_CODE                                          0x02U
#define CDC_IF_CDC_CTRL_CODE                            0x02U
#define CDC_IF_CDC_CTRL_SUB_CLASS_ECM_CODE              0x06U
#define CDC_IF_CDC_DATA_CODE                            0x0AU
#define CDC_IF_CDC_DATA_SUB_CLASS_DATA_CODE             0x00U

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

#define  USBH_ECM_FREE_MEM(x)  if(x){ usb_os_mfree(x); x = NULL;}

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

typedef struct {
	u8 pipe_id;
	usbh_ecm_xter_type_t type;
	u32 check_interval;
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
	CDC_ECM_STATE_IDLE = 0U,
	CDC_ECM_STATE_PRE_SETTING,
	CDC_ECM_STATE_TRANSFER,
	CDC_ECM_STATE_ERROR,
} usbh_cdc_ecm_state_t;

/* cdc ecm ep struct */
typedef struct {
	u8                              *xfer_buf;

	__IO u32                        busy_tick;    /* busy tick */
	__IO u32                        idle_tick;    /* idle tick*/
#if ECM_STATE_DEBUG_ENABLE
	__IO u32                        trigger_cnt;
#endif
	u32                             ep_interval;
	u16                             xfer_len;
	u16                             ep_mps;
	u8                              ep_addr;
	u8                              pipe_id;
	__IO u8                         xfer_state;
	u8                              ep_out_zlp;
} usbh_cdc_ecm_ep_t;

typedef struct {
	u8  bLength;
	u8  bDescriptorType;
} usb_ecm_descriptor_header;

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
	int(* bulk_received)(u8 *buf, u32 len);
	int(* bulk_send)(usbh_urb_state_t state);
	int(* intr_received)(u8 *buf, u32 len);
} usbh_cdc_ecm_state_cb_t;

/* CDC ECM host */
typedef struct {
	usbh_cdc_ecm_ep_t                   ecm_report_ep;
	usbh_cdc_ecm_ep_t                   ecm_tx_ep;
	usbh_cdc_ecm_ep_t                   ecm_rx_ep;

	/* u32 */
	usbh_cdc_ecm_state_cb_t             *cb;
	usb_host_t                          *host;

	/* u16 */
	u16                                 vid;
	u16                                 pid;

	/* u8 */
	u8                                  ecm_if;   /* Interface number */
	u8                                  ecm_alt;  /* Value used to select alternative setting */

	u8                                  state;               /*usb process status : usbh_cdc_ecm_state_t*/
	u8                                  usbh_state;          /*usb host connect status : usbh_ecm_state_t*/
	u8                                  next_transfer;       /*send next event flag*/

	/*
		this means that the ecm transfer can begin
		some ecm dongles, such as 4G dongle, need to make some prepare(send AT CMD to the dongle) before transfer ethernet packet
		all ethernet transfer requests should be discarded before this param is set to true
	*/
	u8                                  allow_ecm_xfer;
} usbh_cdc_ecm_host_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
int usbh_cdc_ecm_init(usbh_cdc_ecm_state_cb_t *cb);
int usbh_cdc_ecm_deinit(void);
int usbh_ecm_timer_register(u8 pipe, u32 time_value, usb_timer_func fn, u8 type);

int usbh_cdc_ecm_choose_config(usb_host_t *host);
int usbh_cdc_ecm_check_config_desc(usb_host_t *host, u8 cfg_max);

int usbh_cdc_ecm_check_enum_status(void);

int usbh_cdc_ecm_bulk_send(u8 *buf, u32 len);
int usbh_cdc_ecm_appx_send(u8 *buf, u32 len, usbh_cdc_ecm_ep_t *tx_ep);
int usbh_cdc_ecm_hand_appx_rx(usbh_cdc_ecm_ep_t  *ep);

u16 usbh_cdc_ecm_get_usbin_mps(void);
u32 usbh_cdc_ecm_get_intr_interval(void);
u16 usbh_cdc_ecm_get_device_vid_info(void);
u16 usbh_cdc_ecm_get_device_pid_info(void);
u8 usbh_cdc_ecm_get_ecm_itf_ifnum(void);
u8 usbh_cdc_ecm_get_ecm_itf_alt(void);
u8 usbh_cdc_ecm_trx_prepare_done(void);

#endif  /* USBD_CDC_ECM_H */
