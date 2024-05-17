/** mbed Microcontroller Library
  ******************************************************************************
  * @file    serial_api.h
  * @author
  * @version V1.0.0
  * @brief   This file provides mbed API for UART.
  ******************************************************************************
  * @attention
  *
 * Copyright(c) 2006 - 2022 Realtek Corporation. All rights reserved.
  *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
 *
  ******************************************************************************
  */

#ifndef MBED_SERIAL_API_H
#define MBED_SERIAL_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_UART
 *  @brief MBED_UART driver modules.
 *  @{
 */

/** @addtogroup MBED_UART_Exported_Types MBED_UART Exported Types
  * @{
  */

/** @defgroup MBED_UART_Enumeration_Type Enumeration Type
  * @{
  */

/**
* @brief UART Parity mode
* used by serial_format
*/
typedef enum {
	ParityNone = 0,		/*!<parity disable */
	ParityOdd = 1,		/*!<odd parity enable */
	ParityEven = 2,		/*!<even paroty enable */
	ParityForced1 = 3,	/*!<same action with ParityOdd */
	ParityForced0 = 4	/*!<same action with ParityEven */
} SerialParity;

/**
 * @brief UART Interrupt enable/disable
 * used by serial_irq_set
 */
typedef enum {
	RxIrq,		/*!<RX IRQ enable/disable */
	TxIrq		/*!<TX IRQ enable/disable */
} SerialIrq;

/**
 * @brief UART FlowControl mode
 * used by serial_set_flow_control
 */
typedef enum {
	FlowControlNone,	/*!<none RTS/CTS */
	FlowControlRTS,		/*!<RTS enable */
	FlowControlCTS,		/*!<CTS enable */
	FlowControlRTSCTS	/*!<RTS/CTS enable */
} FlowControl;

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

typedef void (*uart_irq_handler)(uint32_t id, SerialIrq event);

typedef struct serial_s serial_t;

void serial_init(serial_t *obj, PinName tx, PinName rx);
void serial_free(serial_t *obj);
void serial_baud(serial_t *obj, int baudrate);
void serial_format(serial_t *obj, int data_bits, SerialParity parity, int stop_bits);
void serial_irq_handler(serial_t *obj, uart_irq_handler handler, uint32_t id);
void serial_irq_set(serial_t *obj, SerialIrq irq, uint32_t enable);
int  serial_getc(serial_t *obj);
void serial_putc(serial_t *obj, int c);
int  serial_readable(serial_t *obj);
int  serial_writable(serial_t *obj);
void serial_clear(serial_t *obj);
void serial_break_set(serial_t *obj);
void serial_break_clear(serial_t *obj);
void serial_pinout_tx(PinName tx);
void serial_set_flow_control(serial_t *obj, FlowControl type, PinName rxflow, PinName txflow);
void serial_rts_control(serial_t *obj, BOOLEAN rts_state);

#ifdef __cplusplus
}
#endif

#endif
