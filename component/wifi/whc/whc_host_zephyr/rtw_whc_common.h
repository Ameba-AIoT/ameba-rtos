#ifndef _RTW_INIC_HOST_COMMON_H
#define _RTW_INIC_HOST_COMMON_H

#include "rtw_wifi_common.h"
#include "os_wrapper.h"

#include "whc_sdio_host.h"
#include "whc_sdio_drvio.h"
#include "whc_dev.h"
#include "whc_host_sdio_init_common.h"

#include <zephyr/kernel.h>
#include <soc.h>

int whc_host_sdio_send(int idx, void *pkt_addr, uint32_t len);

#endif
