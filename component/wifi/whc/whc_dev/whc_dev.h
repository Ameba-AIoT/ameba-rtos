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

#include "whc_def.h"

#ifndef CONFIG_FULLMAC
#include "platform_autoconf.h"
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
#ifdef CONFIG_WHC_DEV_TCPIP_KEEPALIVE
#include "whc_dev_tcpip.h"
#endif
#include "whc_dev_api.h"

#ifdef CONFIG_WHC_CMD_PATH
#include "whc_dev_app.h"
#include "whc_dev_ota.h"
#endif

#if defined(CONFIG_RTW_CLI)
#include "whc_dev_rtw_cli/whc_dev_rtw_cli_cmd_define.h"
#include "whc_dev_rtw_cli/whc_dev_rtw_cli_cmd_parse.h"
#ifdef CONFIG_WPA_LITE
#include "whc_dev_rtw_cli/wpa_lite_intf/whc_wpa_lite_app.h"
#endif
#ifdef CONFIG_WPA_STD
#include "whc_dev_rtw_cli/wpas_std_intf/whc_wpas_std_app.h"
#endif
#endif

#if defined(CONFIG_NAN)
#include "rtw_nan_enum.h"
#endif

#if defined(CONFIG_WHC_INTF_SDIO)
#include "ameba_inic.h"
#include "spdio_api.h"
#include "whc_sdio_dev.h"
#elif defined (CONFIG_WHC_INTF_SPI)
#include "whc_spi_dev.h"
#elif defined (CONFIG_WHC_INTF_USB)
#include "whc_usb_dev.h"
#elif defined (CONFIG_WHC_INTF_UART)
#include "serial_api.h"
#include "whc_uart_dev.h"
#endif

#else
#include "whc_dev_struct.h"
#endif

#include "whc_dev_protocal_offload.h"
#include "whc_dev_powersave.h"

#if defined(CONFIG_WPA_STD)
#include "wpas_std_wifi_intf.h"
#endif

void whc_dev_init(void);

#endif
