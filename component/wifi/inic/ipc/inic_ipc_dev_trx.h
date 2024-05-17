/**
  ******************************************************************************
  * @file    inic_ipc_dev_trx.h
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

#ifndef __INIC_IPC_DEV_TX_H__
#define __INIC_IPC_DEV_TX_H__
/* -------------------------------- Includes -------------------------------- */
/* external head files */
#include "platform_autoconf.h"
#include "os_wrapper.h"
#include "rtw_skbuff.h"
#include "wifi_intf_drv_to_lwip.h"

/* internal head files */
#include "inic_ipc.h"

/* -------------------------------- Defines --------------------------------- */
#define CONFIG_INIC_IPC_DEV_XMIT_PRI 6
/* -------------------------------- Macros ---------------------------------- */

/* ------------------------------- Data Types ------------------------------- */

/* ---------------------------- Global Variables ---------------------------- */

/* -------------------------- Function declaration -------------------------- */
void inic_dev_recv(int idx);
void inic_dev_tx_done(int idx);
void inic_dev_trx_event_hdl(u8 event_num, u32 msg_addr, u8 wlan_idx);
#endif /* __INIC_IPC_DEV_TX_H__ */
