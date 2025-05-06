/**
  ******************************************************************************
  * @file    wtn_app_rnat.h
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

#ifndef WTN_APP_RNAT_H
#define WTN_APP_RNAT_H

/* -------------------------------- Includes -------------------------------- */
#define NAT_AP_IP_ADDR0 192
#define NAT_AP_IP_ADDR1 168
#define NAT_AP_IP_ADDR2 43
#define NAT_AP_IP_ADDR3 1

#define NAT_AP_NETMASK_ADDR0 255
#define NAT_AP_NETMASK_ADDR1 255
#define NAT_AP_NETMASK_ADDR2 255
#define NAT_AP_NETMASK_ADDR3 0

#define NAT_AP_GW_ADDR0 192
#define NAT_AP_GW_ADDR1 168
#define NAT_AP_GW_ADDR2 43
#define NAT_AP_GW_ADDR3 1

void wtn_rnat_ap_init(u8 enable);
#endif