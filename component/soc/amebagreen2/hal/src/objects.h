/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_OBJECTS_H
#define MBED_OBJECTS_H

#include "cmsis.h"
#include "PortNames.h"
#include "PeripheralNames.h"
#include "PinNames.h"
#include "ameba.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @addtogroup MBED_GPIO
 *  @brief      MBED_GPIO driver modules.
 *  @{
 */

/** @addtogroup MBED_GPIO_Exported_Types MBED_GPIO Exported Types
  * @{
  */

/** @defgroup MBED_GPIO_Structure_Type Structure Type
  * @{
  */

/**
  * @brief struct gpio_irq_s
  */
typedef struct gpio_irq_s {
	PinName pin;
} gpio_irq_t;

/**
  * @brief struct gpio_s
  */
typedef struct gpio_s {
	PinName pin;
} gpio_t;

/**
  * @brief struct port_s
  */
struct port_s {
	PortName port;
	uint32_t mask;
};
/** @} */

/** @} */
/** @} */



/** @addtogroup MBED_UART
 *  @brief      MBED_UART driver modules.
 *  @{
 */

/** @addtogroup MBED_UART_Exported_Types MBED_UART Exported Types
  * @{
  */

/** @defgroup MBED_UART_Structure_Type Structure Type
  * @{
  */

/**
  * @brief struct serial_s
  */
struct serial_s {
	uint8_t uart_idx;
	uint32_t tx_len;
	uint32_t rx_len;
};
/** @} */

/** @} */
/** @} */


/** @addtogroup MBED_SPI
 *  @brief      MBED_SPI driver modules.
 *  @{
 */

/** @addtogroup MBED_SPI_Exported_Types MBED_SPI Exported Types
  * @{
  */

/** @defgroup MBED_SPI_Structure_Type Structure Type
  * @{
  */

/**
  * @brief struct spi_s
  */
struct spi_s {
	/* user variables */
	uint32_t spi_idx;

	/* internal variables */
	uint32_t irq_handler;
	uint32_t irq_id;
	uint32_t state;
	uint8_t sclk;
	uint32_t bus_tx_done_handler;
	uint32_t bus_tx_done_irq_id;
};
/** @} */

/** @} */
/** @} */


/** @addtogroup MBED_PWM
 *  @brief      MBED_PWM driver modules.
 *  @{
 */

/** @addtogroup MBED_PWM_Exported_Types MBED_PWM Exported Types
 * @{
 */

/** @defgroup MBED_PWM_Structure_Type Structure Type
  * @{
  */

/**
  * @brief struct pwmout_s
  */
struct pwmout_s {
	uint8_t pwmtimer_idx;
	uint8_t pwm_idx;
	uint32_t period;//in us
	float pulse;//in us
};
/** @} */

/** @} */
/** @} */


/** @addtogroup MBED_I2C
 *  @brief      MBED_I2C driver modules.
 *  @{
 */

/** @addtogroup MBED_I2C_Exported_Types MBED_I2C Exported Types
  * @{
  */

/** @defgroup MBED_I2C_Structure_Type Structure Type
  * @{
  */

/**
  * @brief struct i2c_s
  */
struct i2c_s {
	uint32_t i2c_idx;
	I2C_TypeDef *I2Cx;
};
/** @} */

/** @} */
/** @} */


/** @addtogroup MBED_FLASH
 *  @brief      MBED_FLASH driver modules.
 *  @{
 */

/** @addtogroup MBED_FLASH_Exported_Types MBED_FLASH Exported Types
  * @{
  */

/** @defgroup MBED_FLASH_Structure_Type Structure Type
  * @{
  */

/**
  * @brief struct flash_s
  */
struct flash_s {
	FLASH_InitTypeDef SpicInitPara;
};
/** @} */

/** @} */
/** @} */


/** @addtogroup MBED_ADC
 *  @brief      MBED_ADC driver modules.
 *  @{
 */

/** @addtogroup MBED_ADC_Exported_Types MBED_ADC Exported Types
  * @{
  */

/** @defgroup MBED_ADC_Structure_Type Structure Type
  * @{
  */

/**
  * @brief struct analogin_s
  */
struct analogin_s {
	uint8_t adc_idx;
	uint8_t adc_pin;
};
/** @} */

/** @} */
/** @} */


/** @addtogroup MBED_TIMER
 *  @brief      MBED_TIMER driver modules.
 *  @{
 */

/** @addtogroup MBED_TIMER_Exported_Types MBED_TIMER Exported Types
  * @{
  */

/** @defgroup MBED_TIMER_Structure_Type Structure Type
  * @{
  */

/**
  * @brief struct gtimer_s
  */
struct gtimer_s {
	void *handler;
	uint32_t hid;
	uint8_t timer_id;
	uint8_t is_periodcal;
};
/** @} */

/** @} */
/** @} */


/** @addtogroup MBED_GDMA
 *  @brief    MBED_GDMA driver modules.
 *  @{
 */

/** @addtogroup MBED_GDMA_Exported_Types MBED_GDMA Exported Types
  * @{
  */

/** @defgroup MBED_GDMA_Structure_Type Structure Type
  * @{
  */

/**
  * @brief struct gdma_s
  */
struct gdma_s {
	u8 index;
	u8 ch_num;
	IRQ_FUN user_cb;
	u32 user_cb_data;
};
/** @} */
/** @} */

/** @} */

/** @} */
/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif
