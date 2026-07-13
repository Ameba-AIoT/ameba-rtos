
#ifndef _RTW_INIC_HOST_COMMON_H
#define _RTW_INIC_HOST_COMMON_H


#include "rtw_wifi_common.h"
#include "os_wrapper.h"
#include "dlist.h"
#include "whc_ipc_cfg.h"
#include "wifi_api.h"
#include "wifi_intf_drv_to_app_internal.h"
#include "whc_def.h"

#if defined(CONFIG_WHC_INTF_IPC)
#if defined(CONFIG_WHC_HOST)
#include "whc_ipc_host_trx.h"
#endif
#include "whc_ipc.h"
#include "whc_ipc_msg_queue.h"
#endif

/******************************************************
 *                    Constants
 ******************************************************/
/* auth/assoc/key resnd limit can be configured, refer max >> RTW_JOIN_TIMEOUT
 * including auth + assoc + 4way handshake, no dhcp
 */
#define RTW_JOIN_TIMEOUT (10 * 12000 + 13100 + 20200 + 50) //(MAX_CNT_SCAN_TIMES * SCANNING_TIMEOUT + MAX_JOIN_TIMEOUT + KEY_EXCHANGE_TIMEOUT + 50)
#define RTW_SCAN_TIMEOUT (12000) //When blocking scan is invoked in BT COEXIST, the scan time may increases due to TDMA scan, up to 8.96s (5G) +2.17s (2.4G)*/

int whc_ipc_host_send_zephyr(int idx, void *pkt_addr, uint32_t len);

#endif
