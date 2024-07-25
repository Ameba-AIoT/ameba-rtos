/**
  ******************************************************************************
  * The header file for usbh_cdc_ecm.c
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef USBH_CDC_ECM_H
#define USBH_CDC_ECM_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"

/* Macro defines -----------------------------------------------------------*/
#define ECM_ENABLE_PACKETFILTER                                 0

/* for RTL8156BG, use this to allow ping/UDP data transfer */
#define ECM_ENABLE_RCR_CONFIGURATION                            0

/* for RTL8152, config the fifo flow control for data transfer */
#define ECM_ENABLE_FIFO_FLOW_CTRL                               1

/* Exported defines ----------------------------------------------------------*/

#define CDC_ECM_MAC_STRING_LEN                                  (32)
#define CDC_ECM_MAC_STR_LEN                                     (6)
#define CDC_ECM_MUTICAST_FILTER_STR_LEN                         (20)
#define USBH_CDC_ECM_BULK_BUF_MAX_SIZE                          (512*3)


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

#pragma pack(push)
#pragma pack(1)

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

//	CDC_ECM_STATE_CTRL_SET_MAC, //RTL8152,

	CDC_ECM_STATE_CTRL_MAX,
} usbh_cdc_ecm_ctrl_state_t;

/* CDC ECM state */
typedef enum {
	CDC_ECM_STATE_IDLE = 0U,
	CDC_ECM_STATE_PRE_SETTING,
	CDC_ECM_STATE_TRANSFER,
	CDC_ECM_STATE_ERROR,
} usbh_cdc_ecm_state_t;

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
	int(* bulk_received)(u8 *buf, u32 len);
	int(* bulk_send)(usbh_urb_state_t state);
	int(* intr_received)(u8 *buf, u32 len);
} usbh_cdc_ecm_state_cb_t;

/* CDC ECM host */
typedef struct {
	usbh_cdc_ecm_comm_if_t              comm_if;          /* intr ep struct*/
	usbh_cdc_ecm_data_if_t              data_if;          /* bulk ep struct*/
	usbh_cdc_ecm_network_func_t         func_if;          /* network struct*/
	usbh_cdc_ecm_speed_change_t         user_speed_info;  /* ecm dongle speed struct*/

	/* array */
	u8                                  host_pipe[USB_MAX_PIPES];      /*  */
	u8                                  mac[CDC_ECM_MAC_STR_LEN];       /* save the mac value */
	u8                                  muticast_filter[CDC_ECM_MUTICAST_FILTER_STR_LEN];

	/* u32 */
	usbh_cdc_ecm_state_cb_t             *cb;
	usb_host_t                          *host;
	u8                                  *mac_string; /* mac string that get from device */
#if ECM_ENABLE_RCR_CONFIGURATION
	u8                                  *rcr; //8156
#endif
#if ECM_ENABLE_FIFO_FLOW_CTRL
	u32                                 *flow_ctrl; //8152
#endif
	u8                                  *intr_in_buf;         /* intr in buffer */
	u8                                  *bulk_data_out_buf;   /* bulk out buffer */
	u8                                  *bulk_data_in_buf;    /* bulk in buffer */

	u32                                 eth_statistic_count;  /* feature select params */
	u32                                 intr_in_busy_tick;    /* intr in busy tick */
	u32                                 intr_in_idle_tick;    /* intr in idle tick*/
	u32                                 bulk_data_out_len;    /* bluk out data length */
	u32                                 bulk_out_idle_tick;   /* bulk out idle tick */

	/* u16 */
	u16                                 vid;
	u16                                 pid;
	u16                                 feature_selector;    /* feature select params */
	u16                                 packet_filter;       /* packet filter params */
	u16                                 muticast_filter_len; /* multicast filter params length */

	/* bit map*/
	u8                                  state: 3;          /*usb process status : usbh_cdc_ecm_state_t*/
	u8                                  sub_state: 5;      /*usb ctrl process status : usbh_cdc_ecm_ctrl_state_t*/
	u8                                  usbh_state: 3;           /*usb host connect status : usbh_ecm_state_t*/
	u8                                  bulk_data_in_state: 3;   /*bulk in  transfer status : usbh_cdc_ecm_transfer_state_t*/
	u8                                  bulk_data_out_state: 3;  /*bulk out transfer status : usbh_cdc_ecm_transfer_state_t*/
	u8                                  intr_in_state: 3;        /*Intr in  transfer status : usbh_cdc_ecm_transfer_state_t*/
	u8                                  bulk_out_zlp: 1;  /*bulk out packet length is multiple of MPS,should send ZLP*/
	u8                                  next_transfor: 1; /*send next event flag*/
} usbh_cdc_ecm_host_t;

#pragma pack(pop)

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
int usbh_cdc_ecm_init(usbh_cdc_ecm_state_cb_t *cb);
int usbh_cdc_ecm_deinit(void);

int usbh_cdc_ecm_choose_config(usb_host_t *host);
int usbh_cdc_ecm_pre_ctrl_set(void);

int usbh_cdc_ecm_bulk_send(u8 *buf, u32 len);
int usbh_cdc_ecm_bulk_receive(void);
int usbh_cdc_ecm_intr_receive(u8 *buf, u32 len);

u16 usbh_cdc_ecm_get_usbin_mps(void);
u32 usbh_cdc_ecm_get_intr_interval(void);

#endif  /* USBD_CDC_ECM_H */
