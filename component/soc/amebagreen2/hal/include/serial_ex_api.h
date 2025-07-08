/** mbed Microcontroller Library
  ******************************************************************************
  * @file    serial_ex_api.h
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
  * http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an AS IS BASIS, WITHOUT
  * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
#ifndef MBED_SERIAL_EX_API_H
#define MBED_SERIAL_EX_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_UART
 *  @brief      MBED_UART driver modules.
 *  @{
 */

/** @addtogroup MBED_UART_Exported_Types MBED_UART Exported Types
  * @{
  */

/** @defgroup MBED_UART_Enumeration_Type Enumeration Type
  * @{
  */

/**
* @brief Define RX FIFO Level: RX interrupt trigger, RTS de-assert trigger
*/
typedef enum {
	FifoLv1Byte = 0,
	FifoLvQuarter = 1,
	FifoLvHalf = 2,
	FifoLvFull = 3
} SerialFifoLevel;
/** @}*/

/** @}*/
/** @}*/
/** @}*/

void serial_clear_tx(serial_t *obj);
void serial_clear_rx(serial_t *obj);
void serial_send_comp_handler(serial_t *obj, void *handler, uint32_t id);
void serial_recv_comp_handler(serial_t *obj, void *handler, uint32_t id);
int32_t serial_recv_blocked(serial_t *obj, char *prxbuf, uint32_t len, uint32_t timeout_ms);
int32_t serial_send_blocked(serial_t *obj, char *ptxbuf, uint32_t len, uint32_t timeout_ms);
int32_t serial_recv_stream(serial_t *obj, char *prxbuf, uint32_t len);
int32_t serial_send_stream(serial_t *obj, char *ptxbuf, uint32_t len);
int32_t serial_recv_stream_dma(serial_t *obj, char *prxbuf, uint32_t len);
int32_t serial_send_stream_dma(serial_t *obj, char *ptxbuf, uint32_t len);
int32_t serial_send_stream_abort(serial_t *obj);
int32_t serial_recv_stream_abort(serial_t *obj);
void serial_disable(serial_t *obj);
void serial_enable(serial_t *obj);
int32_t serial_recv_stream_timeout(serial_t *obj, char *prxbuf, uint32_t len, uint32_t timeout_ms,	void *force_cs);
int32_t serial_recv_stream_dma_timeout(serial_t *obj, char *prxbuf, uint32_t len, uint32_t timeout_ms,	void *force_cs);
void serial_rx_fifo_level(serial_t *obj, SerialFifoLevel FifoLv);

#ifdef __cplusplus
}
#endif

#endif  // #ifndef MBED_SERIAL_EX_API_H
