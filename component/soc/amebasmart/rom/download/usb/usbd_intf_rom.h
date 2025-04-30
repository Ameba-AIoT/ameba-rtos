/**
  ******************************************************************************
  * @file    usbd_intf_rom.h
  * @author  Realsil WLAN5 Team
  * @version V1.0.0
  * @date    2020-11-23
  * @brief   This file is the header file for usbd_intf_rom.c
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_INTF_ROM_H
#define __USBD_INTF_ROM_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

int USBD_Init(void);

int USBD_DeInit(void);

int USBD_IsAttached(void);

int USBD_Process(void);

#endif /* __USBD_INTF_ROM_H */

