
#ifndef _RTW_INIC_HOST_COMMON_H
#define _RTW_INIC_HOST_COMMON_H


#include "rtw_wifi_common.h"
#include "rtw_wifi_defs.h"
#include "os_wrapper.h"
#include "dlist.h"
#include "inic_ipc_cfg.h"
#include "wifi_intf_drv_to_app_basic.h"
#include "wifi_intf_drv_to_app_internal.h"

#include "wifi_ind.h"
#include "inic_def.h"

#ifndef CONFIG_FULLMAC_HOST
#if defined(CONFIG_AS_INIC_AP)
#include "inic_ipc_host_trx.h"
#endif
#include "inic_ipc.h"
#include "inic_ipc_msg_queue.h"
#endif

int inic_host_send_zephyr(int idx, void *pkt_addr, uint32_t len);

#endif
