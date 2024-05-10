/** mbed Microcontroller Library
  ******************************************************************************
  * @file    spi_ex_api.h
  * @author
  * @version V1.0.0
  * @brief   This file provides following mbed SPI API
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
#ifndef MBED_SPI_EXT_API_H
#define MBED_SPI_EXT_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_SPI
 *  @brief    MBED_SPI driver modules.
 *  @{
 */

/** @defgroup MBED_SPI_Exported_Constants MBED_SPI Exported Constants
  * @{
  */
#define SPI_DMA_RX_EN			(1<<0)
#define SPI_DMA_TX_EN			(1<<1)
#define SPI_STATE_READY			0x00
#define SPI_STATE_RX_BUSY		(1<<1)
#define SPI_STATE_TX_BUSY		(1<<2)
/** @} */

/** @addtogroup MBED_SPI_Exported_Types MBED_SPI Exported Types
  * @{
  */

/** @defgroup MBED_SPI_Enumeration_Type Enumeration Type
  * @{
  */

/**
  * @brief enum SPI_SCLK_IDLE_LEVEL
  */
typedef enum {
	SPI_SCLK_IDLE_LOW = 0,      // the SCLK is Low when SPI is inactive
	SPI_SCLK_IDLE_HIGH = 2      // the SCLK is High when SPI is inactive
} SPI_SCLK_IDLE_LEVEL;

/**
  * @brief enum SPI_CS_TOGGLE_MODE
  * SPI Master mode: for continuous transfer, how the CS toggle
  */
typedef enum {
	SPI_CS_TOGGLE_EVERY_FRAME = 0,     // let SCPH=0 then the CS toggle every frame
	SPI_CS_TOGGLE_START_STOP = 1       // let SCPH=1 the CS toggle at start and stop
} SPI_CS_TOGGLE_MODE;

/**
  * @brief enum SPI_SCLK_TOGGLE_MODE
  */
typedef enum {
	SPI_SCLK_TOGGLE_MIDDLE = 0,    // Serial Clk toggle at middle of 1st data bit and latch data at 1st Clk edge
	SPI_SCLK_TOGGLE_START = 1      // Serial Clk toggle at start of 1st data bit and latch data at 2nd Clk edge
} SPI_SCLK_TOGGLE_MODE;

/**
  * @brief enum ChipSelect
  */
typedef enum {
	CS_0 = 0,
	CS_1 = 1,
	CS_2 = 2,
	CS_3 = 3,
	CS_4 = 4,
	CS_5 = 5,
	CS_6 = 6,
	CS_7 = 7
} ChipSelect;

/**
  * @brief enum SpiIrq
  */
typedef enum {
	SpiRxIrq,
	SpiTxIrq
} SpiIrq;
/** @}*/

/** @} */
/** @} */
/** @} */


typedef void (*spi_irq_handler)(uint32_t id, SpiIrq event);

void spi_irq_hook(spi_t *obj, spi_irq_handler handler, uint32_t id);
void spi_bus_tx_done_irq_hook(spi_t *obj, spi_irq_handler handler, uint32_t id);
void spi_slave_flush_fifo(spi_t *obj);

/**
  * @brief  Close SPI device clock.
  * @param  obj: spi object define in application software.
  * @param  rx_delay: sample rx delay cycle, 1T = 20ns.
  * @retval none
  */
//void spi_set_master_rxdelay(spi_t *obj, u32 rx_delay);

int32_t spi_slave_read_stream(spi_t *obj, char *rx_buffer, uint32_t length);
int32_t spi_slave_write_stream(spi_t *obj, char *tx_buffer, uint32_t length);
int32_t spi_master_read_stream(spi_t *obj, char *rx_buffer, uint32_t length);
int32_t spi_master_write_stream(spi_t *obj, char *tx_buffer, uint32_t length);
int32_t spi_master_write_read_stream(spi_t *obj, char *tx_buffer, char *rx_buffer, uint32_t length);
int32_t spi_slave_read_stream_timeout(spi_t *obj, char *rx_buffer, uint32_t length, uint32_t timeout_ms);
int32_t spi_slave_read_stream_terminate(spi_t *obj, char *rx_buffer, uint32_t length);

/**
  * @brief  slave recv target undetermined length data use interrupt mode for one time.
  * @param  obj: spi slave object define in application software.
  * @param  rx_buffer: buffer to save data read from SPI FIFO.
  * @param  length: number of data bytes to be read, slave could terminate the transfer even if the length is not reached.
  * @retval  : number of bytes read already
  */
//int32_t spi_slave_read_stream_unfix_size(spi_t *obj, char *rx_buffer, uint32_t length);

//#ifdef CONFIG_GDMA_EN
int32_t spi_slave_read_stream_dma(spi_t *obj, char *rx_buffer, uint32_t length);
int32_t spi_slave_write_stream_dma(spi_t *obj, char *tx_buffer, uint32_t length);
int32_t spi_master_write_read_stream_dma(spi_t *obj, char *tx_buffer, char *rx_buffer, uint32_t length);
int32_t spi_master_read_stream_dma(spi_t *obj, char *rx_buffer, uint32_t length);
int32_t spi_master_write_stream_dma(spi_t *obj, char *tx_buffer, uint32_t length);
int32_t spi_slave_read_stream_dma_timeout(spi_t *obj, char *rx_buffer, uint32_t length, uint32_t timeout_ms);
int32_t spi_slave_read_stream_dma_terminate(spi_t *obj, char *rx_buffer, uint32_t length);
//#endif

#ifdef __cplusplus
}
#endif

#endif

