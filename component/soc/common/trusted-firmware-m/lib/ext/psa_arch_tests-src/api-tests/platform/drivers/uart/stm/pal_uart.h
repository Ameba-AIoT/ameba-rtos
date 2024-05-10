/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#ifndef _PAL_UART_H_
#define _PAL_UART_H_

#include <stdint.h>
#define __IO volatile
/**
  * @brief Universal Synchronous Asynchronous Receiver Transmitter
  */
typedef struct {
  __IO uint32_t CR1;     /*!< USART Control register 1,                 Address offset: 0x00 */
  __IO uint32_t CR2;     /*!< USART Control register 2,                 Address offset: 0x04 */
  __IO uint32_t CR3;     /*!< USART Control register 3,                 Address offset: 0x08 */
  __IO uint32_t BRR;     /*!< USART Baud rate register,                 Address offset: 0x0C */
  __IO uint32_t GTPR;    /*!< USART Guard time and prescaler register,  Address offset: 0x10 */
  __IO uint32_t RTOR;    /*!< USART Receiver Time Out register,         Address offset: 0x14 */
  __IO uint32_t RQR;     /*!< USART Request register,                   Address offset: 0x18 */
  __IO uint32_t ISR;     /*!< USART Interrupt and status register,      Address offset: 0x1C */
  __IO uint32_t ICR;     /*!< USART Interrupt flag Clear register,      Address offset: 0x20 */
  __IO uint32_t RDR;     /*!< USART Receive Data register,              Address offset: 0x24 */
  __IO uint32_t TDR;     /*!< USART Transmit Data register,             Address offset: 0x28 */
  __IO uint32_t PRESC;   /*!< USART Prescaler register,                 Address offset: 0x2C */
} USART_TypeDef;

/*  status */
#define USART_ISR_TC_Pos           (6U)
#define USART_ISR_TC_Msk           (0x1UL << USART_ISR_TC_Pos)   /*!< 0x00000040 */
#define USART_ISR_TC               USART_ISR_TC_Msk              /*!< Transmission Complete */
#define USART_ISR_TXE_Pos          (7U)
#define USART_ISR_TXE_Msk          (0x1UL << USART_ISR_TXE_Pos)  /*!< 0x00000080 */
#define USART_ISR_TXE              USART_ISR_TXE_Msk           /*!< Transmit Data Register Empty */
#define USART_ISR_TXE_TXFNF_Pos    USART_ISR_TXE_Pos
#define USART_ISR_TXE_TXFNF_Msk    USART_ISR_TXE_Msk             /*!< 0x00000080 */
#define USART_ISR_TXE_TXFNF        USART_ISR_TXE_Msk


#define USART_CR1_TCIE_Pos         (6U)
#define USART_CR1_TCIE_Msk         (0x1UL << USART_CR1_TCIE_Pos) /*!< 0x00000040 */
#define USART_CR1_TCIE           USART_CR1_TCIE_Msk /*!< Transmission Complete Interrupt Enable */

#define USART_ICR_TCCF_Pos         (6U)
#define USART_ICR_TCCF_Msk         (0x1UL << USART_ICR_TCCF_Pos) /*!< 0x00000040 */
#define USART_ICR_TCCF             USART_ICR_TCCF_Msk    /*!< Transmission Complete Clear Flag */
/* function prototypes */
void pal_uart_stm_init(uint32_t uart_base_addr);
void pal_uart_stm_print(const char *str, int32_t data);
void pal_uart_stm_generate_irq(void);
void pal_uart_stm_disable_irq(void);

#endif /* _PAL_UART_CMSDK_H_ */
