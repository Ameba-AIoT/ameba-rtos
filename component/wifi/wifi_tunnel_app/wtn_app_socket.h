/**
  ******************************************************************************
  * @file    wtn_APP_socket.h
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

#ifndef WTN_APP_SOCKET_H
#define WTN_APP_SOCKET_H

/* -------------------------------- Includes -------------------------------- */

int wtn_socket_send(u8 *buf, u32 len);
int wtn_socket_init(u8 enable);
#endif