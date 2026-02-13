/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef __WHC_DEV_INTF_H__
#define __WHC_DEV_INTF_H__

/* -------------------------------- Includes -------------------------------- */
#ifndef CONFIG_FULLMAC
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
#endif

#endif /* __WHC_DEV_INTF_H__ */
