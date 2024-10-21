/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef __INIC_SDIO_H__
#define __INIC_SDIO_H__

/* -------------------------------- Includes -------------------------------- */

#ifndef CONFIG_FULLMAC
#include "platform_autoconf.h"
#endif

#include "inic_def.h"

#ifndef CONFIG_FULLMAC
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "rtw_wifi_constants.h"
#include "rtw_skbuff.h"
#include "rtw_queue.h"
#include "wifi_conf.h"
#include "wifi_intf_drv_to_lwip.h"
#include "wifi_intf_drv_to_upper.h"

#include "inic_dev_struct.h"

#include "inic_dev_api.h"
#include "inic_dev_trx.h"
#include "inic_dev_msg_queue.h"

#ifdef CONFIG_INIC_INTF_SDIO
#include "ameba_inic.h"
#include "spdio_api.h"
#include "inic_sdio_dev.h"
#elif defined (CONFIG_INIC_INTF_SPI)
#include "inic_spi_dev.h"
#elif defined (CONFIG_INIC_INTF_USB)
#include "inic_usb_dev.h"
#endif
#ifdef CONFIG_SDIO_BRIDGE
#include "inic_sdio_dev_bridge.h"
#endif
#else
#include "inic_dev_struct.h"
#endif

#ifndef CONFIG_SDIO_BRIDGE
#include "inic_dev_protocal_offload.h"
#endif

#endif /* __INIC_SDIO_H__ */
