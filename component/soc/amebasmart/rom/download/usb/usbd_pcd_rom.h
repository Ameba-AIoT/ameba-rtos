/**
  ******************************************************************************
  * @file    usbd_pcd_rom.h
  * @author  Realsil WLAN5 Team
  * @version V1.0.0
  * @date    2020-11-23
  * @brief   This file is the header file for usbd_pcd_rom.c
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
#ifndef USBD_PCD_ROM_H
#define USBD_PCD_ROM_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_hal_rom.h"

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

#define USB_PCD_MASK_INTERRUPT(__INTERRUPT__)     			((USB_GLOBAL)->GINTMSK &= ~(__INTERRUPT__))
#define USB_PCD_UNMASK_INTERRUPT(__INTERRUPT__)  			((USB_GLOBAL)->GINTMSK |= (__INTERRUPT__))

#define USB_PCD_CLEAR_IN_EP_INTR(__EPNUM__, __INTERRUPT__)  (USB_INEP(__EPNUM__)->DIEPINT = (__INTERRUPT__))
#define USB_PCD_CLEAR_OUT_EP_INTR(__EPNUM__, __INTERRUPT__) (USB_OUTEP(__EPNUM__)->DOEPINT = (__INTERRUPT__))

#define USB_PCD_GET_FLAG(__INTERRUPT__)      				((USB_ReadInterrupts() & (__INTERRUPT__)) == (__INTERRUPT__))
#define USB_PCD_CLEAR_FLAG(__INTERRUPT__)    				(((USB_GLOBAL)->GINTSTS) &=  (__INTERRUPT__))

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/* Initialization/de-initialization functions  ********************************/
HAL_Status USB_PCD_Init(USB_PCD_HandleTypeDef *hpcd);
HAL_Status USB_PCD_DeInit(USB_PCD_HandleTypeDef *hpcd);

/* I/O operation functions  ***************************************************/
HAL_Status USB_PCD_Start(USB_PCD_HandleTypeDef *hpcd);
HAL_Status USB_PCD_Stop(USB_PCD_HandleTypeDef *hpcd);
void USB_PCD_IRQHandler(USB_PCD_HandleTypeDef *hpcd);

/* Peripheral Control functions  **********************************************/
HAL_Status USB_PCD_DevConnect(USB_PCD_HandleTypeDef *hpcd);
HAL_Status USB_PCD_DevDisconnect(USB_PCD_HandleTypeDef *hpcd);
HAL_Status USB_PCD_SetAddress(USB_PCD_HandleTypeDef *hpcd, u8 address);
HAL_Status USB_PCD_EP_Open(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr, u16 ep_mps, u8 ep_type);
HAL_Status USB_PCD_EP_Close(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr);
HAL_Status USB_PCD_EP_Receive(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr, u8 *pBuf, u32 len);
HAL_Status USB_PCD_EP_Transmit(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr, u8 *pBuf, u32 len);
u32        USB_PCD_EP_GetRxCount(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr);
HAL_Status USB_PCD_EP_SetStall(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr);
HAL_Status USB_PCD_EP_ClrStall(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr);
HAL_Status USB_PCD_EP_Flush(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr);
HAL_Status USB_PCD_SetTxFiFo(USB_PCD_HandleTypeDef *hpcd, u8 fifo, u16 size);
HAL_Status USB_PCD_SetRxFiFo(USB_PCD_HandleTypeDef *hpcd, u16 size);

/* Peripheral State functions  ************************************************/
USB_PCD_StateTypeDef USB_PCD_GetState(USB_PCD_HandleTypeDef *hpcd);

#endif /* USBD_PCD_ROM_H */

