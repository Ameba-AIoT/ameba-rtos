/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_CDC_ECM_APPX_H
#define USBH_CDC_ECM_APPX_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"

#define CDC_ECM_MAC_STRING_LEN                                  (32)
#define CDC_ECM_MAC_STR_LEN                                     (6)
#define CDC_ECM_MAC_CTRL_REG_LEN                                (4)
#define CDC_ECM_LINE_CODING_DATA_LEN                            0x07U

/* Exported defines ----------------------------------------------------------*/
typedef enum {
	CDC_ECM_MAC_UNINIT = 0U,
	CDC_ECM_MAC_DONGLE_SUPPLY,
	CDC_ECM_MAC_UPPER_LAYER_SET,
	CDC_ECM_MAC_RANDOM_SET,
	CDC_ECM_MAC_TYPE_MAX,
} usbh_cdc_ecm_dongle_mac_type_t;

/* ---------------------------CDC ACM Config Start -------------------------------*/
/* Line coding char format */
typedef enum {
	CDC_ACM_LINE_CODING_CHAR_FORMAT_1_STOP_BITS    = 0U,
	CDC_ACM_LINE_CODING_CHAR_FORMAT_1_5_STOP_BITS,
	CDC_ACM_LINE_CODING_CHAR_FORMAT_2_STOP_BITS
} usbh_cdc_acm_line_coding_char_format_t;

/* Line coding parity type */
typedef enum {
	CDC_ACM_LINE_CODING_PARITY_NO   = 0U,
	CDC_ACM_LINE_CODING_PARITY_ODD,
	CDC_ACM_LINE_CODING_PARITY_EVEN,
	CDC_ACM_LINE_CODING_PARITY_MARK,
	CDC_ACM_LINE_CODING_PARITY_SPACE
} usbh_cdc_acm_line_coding_parity_t;
/* Line coding structure */
typedef union {
	u8 d8[CDC_ECM_LINE_CODING_DATA_LEN];
	struct {
		u32 dwDteRate;    /* Data terminal rate, in bits per second */
		u8 bCharFormat;   /* Stop bits */
		u8 bParityType;   /* Parity */
		u8 bDataBits;     /* Data bits (5, 6, 7, 8 or 16) */
	} b;
} usbh_cdc_ecm_line_coding_t;
/* ---------------------------CDC ACM Config End -------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef enum {
	CDC_ECM_STATE_AT_SETTING_IDLE      = 0U,
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

	CDC_ECM_STATE_AT_SET_LINE_CODING,
	CDC_ECM_STATE_AT_SETTING_MAX,
} usbh_cdc_ecm_at_set_state_t;

typedef struct {
	u8 muticast_filter[CDC_ECM_MUTICAST_FILTER_STR_LEN];
	u8 mac[CDC_ECM_MAC_STR_LEN];
	u8 mac_ctrl_lock[CDC_ECM_MAC_CTRL_REG_LEN];      /* for 8152 change mac */
	u8 flow_ctrl[CDC_ECM_MAC_CTRL_REG_LEN];          /* 8152 */
	u8 rcr[CDC_ECM_MAC_CTRL_REG_LEN];                /* 8156 */

	usbh_cdc_ecm_at_set_state_t sub_status;
	usbh_cdc_ecm_ep_t report_ep;
	usbh_cdc_ecm_ep_t tx_ep;
	usbh_cdc_ecm_ep_t rx_ep;

	usb_appx_report rx_report;
	usbh_cdc_ecm_line_coding_t line_coding;    /* acm line setting */
	usb_os_sema_t acm_tx_sema;                 /* usb tx send sema */

	usbh_cdc_ecm_appx_param_t *param_item;
	usbh_cdc_ecm_appx_param_t *param_array;    /* at port interface idx */

	u16 *led_array;                            /* led array */
	u8 *dongle_ctrl_buf;                       /* used for transfer,cache align */

	u32 eth_statistic_count;                   /* feature select params */
	u16 feature_selector;                      /* feature select params */
	u16 packet_filter;                         /* packet filter params */
	u16 muticast_filter_len;                   /* multicast filter params length */

	__IO u8 acm_tx_block;                      /* ecm attached status */

	u8 mac_valid;
	u8 iMACAddressStringId;                    /* get the mac address */

	u8 led_cnt;                                /* led cnt */
	u8 mac_src_type;                           /* ecm dongle mac source type，@ref usbh_cdc_ecm_dongle_mac_type_t */
} usbh_cdc_ecm_appx_t;

/* */
u8 *usbh_cdc_ecm_find_next_ifdesc(u8 *pbuf, u32 *len);

u8 usbh_cdc_ecm_appx_doinit(usbh_cdc_ecm_priv_data_t *priv);
u8 usbh_cdc_ecm_appx_deinit(usb_host_t *host);

u8 usbh_cdc_ecm_parse_at_cfgdesc(usb_host_t *host, u16 vid, u16 pid);
u8 usbh_cdc_ecm_appx_deinit_pipe(usb_host_t *host);

u8 usbh_cdc_ecm_appx_pre_setting(usb_host_t *host);
u8 usbh_cdc_ecm_appx_transfer(usb_host_t *cdc, u32 msg);

#endif  /* USBH_CDC_ECM_APPX_H */
