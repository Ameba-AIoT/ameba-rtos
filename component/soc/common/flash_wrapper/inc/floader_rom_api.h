/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _FLOADER_ROM_API_H_
#define _FLOADER_ROM_API_H_

/* Includes ------------------------------------------------------------------*/

#include "ameba_soc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/
#if !defined(CONFIG_AMEBASMART)
#define SYSCFG_BootFromNor  SYSCFG_OTP_BootFromNor
#endif
/* Exported variables --------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

#ifdef CONFIG_LINK_ROM_SYMB
void LOGUART_Init(LOGUART_InitTypeDef *LOGUART_InitStruct);
void xmodem_uart_setbaud(u32 baud);
void xmodem_uart_clean_rx(void);
char xmodem_uart_readable(void);
char xmodem_uart_getc(void);
void xmodem_uart_putc(char c);
void xmodem_uart_putdata(u8 *buf, u32 cnt);
void xmodem_uart_port_init(u8 uart_idx, u8 pin_mux, u32 baud_rate);
void xmodem_uart_port_deinit(u8 uart_idx);
char xmodem_uart_writable(void);

void GPIO_Init(GPIO_InitTypeDef *GPIO_InitStruct);
void GPIO_WriteBit(u32 GPIO_Pin, u32 Pin_State);
void FLASH_UserMode_Enter(void);
void FLASH_WriteEn_InUserMode(void);
void FLASH_WaitBusy_InUserMode(u32 WaitType);
void FLASH_UserMode_Exit(void);
void FLASH_TxCmd_InUserMode(u8 cmd, u8 DataPhaseLen, u8 *pData);
#endif

#ifdef __cplusplus
}
#endif

#endif // _FLOADER_ROM_API_H_
