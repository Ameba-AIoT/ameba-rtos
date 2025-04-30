/**
  ******************************************************************************
  * @file    usbd_dbg_rom.h
  * @author  Realsil WLAN5 Team
  * @version V1.0.0
  * @date    2020-11-23
  * @brief   This file provides the USB debug functions
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef USBD_DBG_ROM_H
#define USBD_DBG_ROM_H

/* Includes ------------------------------------------------------------------*/

#include "ameba_soc.h"

/* Exported defines ----------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

void USBD_DebugInit(void);
void USBD_Debug(u32 code, u32 delay);
void USBD_Debug_Config(u8 enable);

#endif /* USBD_DBG_ROM_H */

