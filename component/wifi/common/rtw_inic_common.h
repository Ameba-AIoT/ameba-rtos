
#ifndef _RTW_INIC_HOST_COMMON_H
#define _RTW_INIC_HOST_COMMON_H


#include "rtw_wifi_common.h"
#include "rtw_wifi_defs.h"
#include "os_wrapper.h"
#include "dlist.h"
#include "whc_ipc_cfg.h"
#include "wifi_intf_drv_to_app_basic.h"
#include "wifi_intf_drv_to_app_internal.h"

#include "wifi_ind.h"
#include "whc_def.h"

#if !defined(CONFIG_FULLMAC_HOST) && defined(CONFIG_WHC_INTF_IPC)
#if defined(CONFIG_AS_INIC_AP)
#include "whc_ipc_host_trx.h"
#endif
#include "whc_ipc.h"
#include "whc_ipc_msg_queue.h"
#endif

int whc_ipc_host_send_zephyr(int idx, void *pkt_addr, uint32_t len);

#endif
