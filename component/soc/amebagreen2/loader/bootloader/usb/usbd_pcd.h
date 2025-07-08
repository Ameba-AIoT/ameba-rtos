/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef USBD_PCD_H
#define USBD_PCD_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_hal.h"

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
u32 USB_PCD_EP_GetRxCount(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr);
HAL_Status USB_PCD_EP_SetStall(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr);
HAL_Status USB_PCD_EP_ClrStall(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr);
HAL_Status USB_PCD_EP_Flush(USB_PCD_HandleTypeDef *hpcd, u8 ep_addr);
HAL_Status USB_PCD_SetTxFiFo(USB_PCD_HandleTypeDef *hpcd, u8 fifo, u16 size);
HAL_Status USB_PCD_SetRxFiFo(USB_PCD_HandleTypeDef *hpcd, u16 size);

/* Peripheral State functions  ************************************************/
USB_PCD_StateTypeDef USB_PCD_GetState(USB_PCD_HandleTypeDef *hpcd);

#endif /* USBD_PCD_H */

