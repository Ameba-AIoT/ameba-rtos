/**
  ******************************************************************************
  * @file    wifi_intf_drv_to_app_cast.h
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

#ifndef WIFI_CONF_CAST_H
#define WIFI_CONF_CAST_H

#include "rtw_wifi_constants.h"
#include "wifi_api.h"
#include "wifi_intf_drv_to_app_internal.h"
#include "os_wrapper.h"
#include <rtw_timer.h>
#include "lwip/err.h"
#include "ethernetif.h"


#if defined(CONFIG_AS_INIC_AP)
#include "whc_ipc_host_trx.h"
#endif


#ifndef ETH_ALEN
#define ETH_ALEN                    6
#endif

#define IS_BROADCAST(addr)      (((addr)[0] & (addr)[1] & (addr)[2] & (addr)[3] & (addr)[4] & (addr)[5]) == 0xFF)
/* Definitions for error constants. */
#define WIFI_CAST_OK                0
#define WIFI_CAST_ERR               -1

#define WIFI_CAST_ERR_NO_MEMORY     0x101
#define WIFI_CAST_ERR_INVALID_DATA  0x102
#define WIFI_CAST_ERR_NODE_EXIST    0x103
#define WIFI_CAST_ERR_WAIT_TIMEOUT  0x104

/* The channel on which the device sends packets */
#define WIFI_CAST_CHANNEL_CURRENT   0x0   /* Only in the current channel */
#define WIFI_CAST_CHANNEL_ALL       0xf  /* All supported channels */

#define MAX_NODE_NUM                16
#define WIFI_CAST_KEY_LEN           16

extern const unsigned char WIFI_CAST_BROADCAST_MAC[ETH_ALEN];
typedef int wcast_err_t;
typedef unsigned char wifi_cast_addr_t[ETH_ALEN];
typedef unsigned char wifi_cast_key_t[WIFI_CAST_KEY_LEN];
/**
 * @brief Wifi cast frame type.
 */
typedef enum {
	WIFI_CAST_DATA = 0,
	WIFI_CAST_ACK,
} wifi_cast_frame_type_t;

/**
 * @brief Wifi cast node parameters.
 */
typedef struct wifi_cast_node {
	wifi_cast_addr_t  mac; /* set node mac address */
	wifi_cast_key_t   key; /* set key for encrypt data */
	unsigned char encrypt; /* set enable or disable encryption */
	void *priv;            /* user define */
} wifi_cast_node_t;

/**
 * @brief Wifi cast node information parameters.
 */
typedef struct wifi_cast_frame_info {
	unsigned int wait_ms;           /* set wait timeout when ack set to true */
	unsigned char ack;              /* set enable or disable ACK, set to true if need rx node response with ack */
	unsigned char retry_limit;      /* tx packet retry times (hardware retry limit times) */
	unsigned char retransmit_count; /* tx packet restansmit count by software */
	unsigned char channel;          /* tx packet channel, set to WIFI_CAST_CHANNEL_CURRENT or WIFI_CAST_CHANNEL_ALL */
	unsigned char tx_rate;     /* tx packet rate, val: RTW_RATE_1M, RTW_RATE_2M...*/
} wifi_cast_frame_info_t;

#define WIFI_CAST_FRAME_INFO_DEFAULT() \
    { \
        .wait_ms = 3000, \
        .retry_limit = 4, \
        .retransmit_count = 6, \
        .tx_rate = RTW_RATE_54M, \
    }

/**
 * @brief Wifi cast config.
 */
typedef struct wifi_cast_config {
	unsigned char 	channel;			/* initial channel */
} wifi_cast_config_t;

#define WIFI_CAST_INIT_CONFIG_DEFAULT() { \
    .channel = 6, \
    }

/**
  * @brief     Wifi cast data receive callback function
  * @param     pnode node parameters
  * @param     data receive data
  * @param     data_len length of receive data
  * @param     userdata user defined data
  */
typedef void (*wifi_cast_recv_cb_t)(wifi_cast_node_t *pnode, unsigned char *buf, unsigned int len, signed char rssi);

#define WIFI_CAST_ERROR_CHECK(x) \
	do {                                      \
        wcast_err_t err_rc_ = (x);                                        \
        if (err_rc_ != WIFI_CAST_OK) {                              \
            DiagPrintf("%s, filename: %s, line: %d, err: %d\n", __FUNCTION__, __FILE__, __LINE__,        \
                                    err_rc_);                 \
        }                                                               \
    } while(0)

/**
  * @brief     Initialize wifi cast function
  * @param     pconfig wifi cast configuration, should be set by user
  * @return
  * 		   - WIFI_CAST_OK : succeed
  * 		   - WIFI_CAST_ERR: failed
  */
wcast_err_t wifi_cast_init(wifi_cast_config_t *pconfig);

/**
  * @brief     De-Initialize wifi cast function
  * @param     padapter wifi cast configuration, should be set by user
  * @return
  * 		   - WIFI_CAST_OK : succeed
  * 		   - WIFI_CAST_ERR: failed
  */
wcast_err_t wifi_cast_deinit(wifi_cast_config_t *pconfig);

/**
  * @brief     Add node to node list
  * @param     pnode node need to be added, should be set by user
  * @return
  * 		   - WIFI_CAST_OK : succeed
  * 		   - WIFI_CAST_ERR_NO_MEMORY: out of memory or out of maximum node number
  */
wcast_err_t wifi_cast_add_node(wifi_cast_node_t *pnode);

/**
  * @brief     Delete node from node list
  * @param     pnode node need to be deleted, should be set by user
  * @return
  * 		   - WIFI_CAST_OK : succeed
  * 		   - WIFI_CAST_ERR: failed
  */
wcast_err_t wifi_cast_del_node(wifi_cast_node_t *pnode);

/**
  * @brief     Get node information from node list
  * @param     pnode node need to be checked, mac address should be set by user
  * @return
  * 		   - If exist, return target node information else return NULL
  */
wifi_cast_node_t *wifi_cast_get_node_info(wifi_cast_node_t *pnode);

/**
  * @brief     Send wifi cast data
  * @param     pnode node need to be deleted, should be set by user
  * @param	   data data to send
  * @param     data_len length of data
  * @return
  * 		   - WIFI_CAST_OK : succeed
  * 		   - WIFI_CAST_ERR: failed
  */
wcast_err_t wifi_cast_send(wifi_cast_node_t *pnode, unsigned char *data, int data_len, wifi_cast_frame_info_t *info);

/**
  * @brief     Set wifi cast data receive callback function
  * @param     recv_cb receive callback function, defined by user
  * @return
  * 		   - WIFI_CAST_OK : succeed
  * 		   - WIFI_CAST_ERR: failed
  */
wcast_err_t wifi_cast_register_recv_cb(wifi_cast_recv_cb_t recv_cb);
#endif
