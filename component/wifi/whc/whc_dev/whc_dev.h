/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef __WHC_DEV_H__
#define __WHC_DEV_H__

/* -------------------------------- Includes -------------------------------- */

#ifndef CONFIG_FULLMAC
#include "platform_autoconf.h"
#endif

#include "whc_def.h"

#ifndef CONFIG_FULLMAC
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "rtw_skbuff.h"
#include "rtw_queue.h"
#include "wifi_api.h"
#include "wifi_intf_drv_to_lwip.h"
#include "wifi_intf_drv_to_upper.h"

#include "whc_dev_struct.h"

#include "whc_dev_api_path.h"
#include "whc_dev_trx.h"
#include "whc_dev_msg_queue.h"
#include "whc_dev_cust_evt.h"

#if defined(CONFIG_WHC_INTF_SDIO)
#include "ameba_inic.h"
#include "spdio_api.h"
#include "whc_sdio_drv.h"
#include "whc_sdio_dev.h"
#elif defined (CONFIG_WHC_INTF_SPI)
#include "whc_spi_drv.h"
#elif defined (CONFIG_WHC_INTF_USB)
#include "whc_usb_dev.h"
#elif defined (CONFIG_WHC_INTF_UART)
#include "serial_api.h"
#include "whc_uart_drv.h"

#endif

#include "whc_dev_api.h"

#else
#include "whc_dev_struct.h"
#endif

#include "whc_dev_protocal_offload.h"

#if !defined(CONFIG_FULLMAC) && defined(CONFIG_NAN)
#include "rtw_nan_enum.h"
#endif

#include "whc_dev_powersave.h"
/* remove after rom freeze */
#ifndef CONFIG_BUILD_ROM
#include "bt_inic_defs.h"

enum WHC_WIFI_CTRL_TYPE {
	WHC_WIFI_EVT_XIMT_PKTS = 0xa5a5a500,
	WHC_WIFI_EVT_RECV_PKTS,
	WHC_WIFI_EVT_API_CALL,
	WHC_WIFI_EVT_API_RETURN,
	WHC_WIFI_EVT_BRIDGE,
	WHC_WIFI_EVT_MAX,
	WHC_CUST_EVT, /* the ID to transmit data for the customer. */

	WHC_BT_EVT_BASE = WHC_BT_ID_BASE,
	WHC_BT_EVT_MAX = WHC_BT_ID_BASE + 0x1000000
};
#endif

#if defined(CONFIG_WPA_STD)
#include "wpas_std_wifi_intf.h"
#endif //CONFIG_WPA_STD


#endif /* __INIC_SDIO_H__ */
