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

#ifndef __WHC_IPC_DEV_TX_H__
#define __WHC_IPC_DEV_TX_H__
/* -------------------------------- Includes -------------------------------- */
/* external head files */
#include "platform_autoconf.h"
#include "os_wrapper.h"
#include "rtw_skbuff.h"
#include "wifi_intf_drv_to_lwip.h"

/* internal head files */
#include "whc_ipc.h"

#define whc_dev_tx_done             whc_ipc_dev_tx_done
#define whc_dev_recv                whc_ipc_dev_recv

/* -------------------------------- Defines --------------------------------- */
#define CONFIG_INIC_IPC_DEV_XMIT_PRI 6
/* -------------------------------- Macros ---------------------------------- */

/* ------------------------------- Data Types ------------------------------- */

/* ---------------------------- Global Variables ---------------------------- */

/* -------------------------- Function declaration -------------------------- */
void whc_dev_recv(int idx);
void whc_ipc_dev_tx_done(int idx);
void whc_ipc_dev_trx_event_hdl(u8 event_num, u32 msg_addr, u8 wlan_idx);
#endif /* __INIC_IPC_DEV_TX_H__ */
