/**
 ******************************************************************************
 * @file    stsafe_service_stub.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @brief   Implementation file of psa_drv_se interface.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define STSAFEA_VREG_PIN                          GPIO_PIN_11
#define STSAFEA_VREG_GPIO_PORT                    GPIOF
#define STSAFEA_VREG_GPIO_PORT_CLK_ENABLE         __HAL_RCC_GPIOF_CLK_ENABLE
#define STSAFEA_DEVICE_ADDRESS                    0x0020

#define I2C_ANALOG_FILTER_DELAY_DEFAULT        2U      /* ns */

#define BUS_I2C2_INSTANCE                       I2C2

#define BUS_I2C2_CLK_ENABLE()                   __HAL_RCC_I2C2_CLK_ENABLE()
#define BUS_I2C2_CLK_DISABLE()                  __HAL_RCC_I2C2_CLK_DISABLE()

#define BUS_I2C2_SDA_GPIO_PIN                   GPIO_PIN_5
#define BUS_I2C2_SCL_GPIO_PIN                   GPIO_PIN_4

#define BUS_I2C2_SDA_GPIO_PORT                  GPIOH
#define BUS_I2C2_SCL_GPIO_PORT                  GPIOH

#define BUS_I2C2_SDA_GPIO_AF                    GPIO_AF4_I2C2
#define BUS_I2C2_SCL_GPIO_AF                    GPIO_AF4_I2C2

#define BUS_I2C2_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOH_CLK_ENABLE()
#define BUS_I2C2_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOH_CLK_ENABLE()

#define BUS_I2C2_POLL_TIMEOUT                0x1000U

#define BUS_I2C2_TIMING                      0x00F07BFF /* Corresponding to frequency of I2C1 = 400 KHz*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;
I2C_HandleTypeDef  *hbus_i2c = &hi2c2;

/* Private function prototypes -----------------------------------------------*/
int32_t HW_IO_Init(void);
void    BSP_TimeDelay(uint32_t msDelay);
HAL_StatusTypeDef MX_I2C2_Init(I2C_HandleTypeDef *phi2c, uint32_t timing);
int32_t BSP_I2C_Init(void);
int32_t BSP_I2C_DeInit(void);
int32_t BSP_I2C_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length);
static void I2C2_MspInit(I2C_HandleTypeDef *hI2c);
static void I2C2_MspDeInit(I2C_HandleTypeDef *hI2c);
#define BSP_OK     0
#define BSP_NAK    1
#define BSP_ERROR -1

/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Additional IO pins configuration needed for STSAFE (VREG pin, etc.)
  * @param  none
  * @retval 0 in case of success, an error code otherwise
  */
int32_t HW_IO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  STSAFEA_VREG_GPIO_PORT_CLK_ENABLE();

  /* Configure GPIO pin : RST Pin */
  GPIO_InitStruct.Pin = STSAFEA_VREG_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(STSAFEA_VREG_GPIO_PORT, &GPIO_InitStruct);

  /* Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(STSAFEA_VREG_GPIO_PORT, STSAFEA_VREG_PIN, GPIO_PIN_SET);

  // STSAFE takes up to 50 ms to get ready, but it might take less.
  // The first command will use the retry mechanism to cop with this delay
  BSP_TimeDelay(50);

  return BSP_OK;
}


/**
  * @brief  This function provides a delay (in milliseconds)
  * @param  none
  * @retval 0 in case of success, an error code otherwise
  */
void BSP_TimeDelay(uint32_t msDelay)
{
  /* Could be redirected to a Task Delay or to a different custom implementation */
  uint32_t timeout = HAL_GetTick() + msDelay*40;
  if (msDelay != HAL_MAX_DELAY)
  {
    while (1)
    {
        if (HAL_GetTick() >= timeout)
        {
          return;
        }
    }
  }
}

/**
  * @brief  MX I2C2 Inititialization.
  * @param  phi2c : I2C handle.
  * @param  timing : I2C timings as described in the I2C peripheral V2 and V3.
  * @retval Prescaler divider
  */
__weak HAL_StatusTypeDef MX_I2C2_Init(I2C_HandleTypeDef *phi2c, uint32_t timing)
{
  HAL_StatusTypeDef ret = HAL_ERROR;

  phi2c->Init.Timing           = timing;
  phi2c->Init.OwnAddress1      = 0;
  phi2c->Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
  phi2c->Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
  phi2c->Init.OwnAddress2      = 0;
  phi2c->Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
  phi2c->Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

  if (HAL_I2C_Init(phi2c) == HAL_OK)
  {
    if (HAL_I2CEx_ConfigAnalogFilter(phi2c, I2C_ANALOGFILTER_DISABLE) == HAL_OK)
    {
      if (HAL_I2CEx_ConfigDigitalFilter(phi2c, I2C_ANALOG_FILTER_DELAY_DEFAULT) == HAL_OK)
      {
        ret = HAL_OK;
      }
    }
  }

  return ret;
}

/**
  * @brief  Initializes I2C HAL.
  * @retval BSP status
  */
int32_t BSP_I2C_Init(void)
{
  int32_t ret = BSP_ERROR;
  hbus_i2c->Instance  = I2C2;

  if (HAL_I2C_GetState(hbus_i2c) == HAL_I2C_STATE_RESET)
  {
    /* Init the I2C Msp */
    I2C2_MspInit(hbus_i2c);

    /* Init the I2C */
    if (MX_I2C2_Init(hbus_i2c, BUS_I2C2_TIMING) == HAL_OK)
    {
      if (HAL_I2CEx_ConfigAnalogFilter(hbus_i2c, I2C_ANALOGFILTER_ENABLE) == HAL_OK)
      {
        ret = BSP_OK;
      }
    }
  }
  else
  {
    ret = BSP_OK;
  }

  return ret;
}

/**
  * @brief  DeInitializes I2C HAL.
  * @retval BSP status
  */
int32_t BSP_I2C_DeInit(void)
{
  int32_t ret  = BSP_ERROR;
  /* DeInit the I2C */
  I2C2_MspDeInit(hbus_i2c);

  /* DeInit the I2C */
  if (HAL_I2C_DeInit(hbus_i2c) == HAL_OK)
  {
    ret = BSP_OK;
  }

  return ret;
}


/**
  * @brief  Send data through BUS.
  * @param  DevAddr Device address on Bus.
  * @param  pData  Pointer to data buffer to write
  * @param  Length Data Length
  * @retval BSP status
  */
int32_t BSP_I2C_Send(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR;
  uint32_t hal_error;

  if (HAL_I2C_Master_Transmit(hbus_i2c,
                              DevAddr,
                              pData,
                              Length,
                              BUS_I2C2_POLL_TIMEOUT) == HAL_OK)
  {
    ret = BSP_OK;
  }
  else
  {
    hal_error = HAL_I2C_GetError(hbus_i2c);
    if (hal_error == HAL_I2C_ERROR_AF)
    {
      return BSP_NAK;
    }
    else
    {
      ret =  BSP_ERROR;
    }
  }
  return ret;
}

/**
  * @brief  Send data through BUS
  * @param  DevAddr Device address on Bus.
  * @param  pData  Pointer to data buffer to read
  * @param  Length Data Length
  * @retval BSP status
  */
int32_t  BSP_I2C_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR;
  uint32_t hal_error;

  if (HAL_I2C_Master_Receive(hbus_i2c,
                             DevAddr,
                             pData,
                             Length,
                             BUS_I2C2_POLL_TIMEOUT) == HAL_OK)
  {
    ret = BSP_OK;
  }
  else
  {
    hal_error = HAL_I2C_GetError(hbus_i2c);
    if (hal_error == HAL_I2C_ERROR_AF)
    {
      return BSP_NAK;
    }
    else
    {
      ret = BSP_ERROR;
    }
  }

  return ret;
}


/**
  * @brief  Initializes I2C MSP.
  * @param  hI2c  I2C handler
  * @retval None
  */
static void I2C2_MspInit(I2C_HandleTypeDef *hI2c)
{
  GPIO_InitTypeDef  gpio_init;

  /* Enable I2C clock */
  BUS_I2C2_CLK_ENABLE();

  /* Enable GPIO clock */
  BUS_I2C2_SDA_GPIO_CLK_ENABLE();
  BUS_I2C2_SCL_GPIO_CLK_ENABLE();

  /* Configure I2C SDA Line */
  gpio_init.Pin = BUS_I2C2_SDA_GPIO_PIN;
  gpio_init.Mode = GPIO_MODE_AF_OD;
  gpio_init.Pull = GPIO_NOPULL;
  gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init.Alternate = BUS_I2C2_SDA_GPIO_AF;
  HAL_GPIO_Init(BUS_I2C2_SDA_GPIO_PORT, &gpio_init);

  /* Configure I2C SCL Line */
  gpio_init.Pin = BUS_I2C2_SCL_GPIO_PIN;
  gpio_init.Mode = GPIO_MODE_AF_OD;
  gpio_init.Pull = GPIO_NOPULL;
  gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init.Alternate = BUS_I2C2_SCL_GPIO_AF;
  HAL_GPIO_Init(BUS_I2C2_SCL_GPIO_PORT, &gpio_init);

}

/**
  * @brief  DeInitializes I2C MSP.
  * @param  hI2c  I2C handler
  * @retval None
  */
static void I2C2_MspDeInit(I2C_HandleTypeDef *hI2c)
{
  /* Disable I2C clock */
  __HAL_RCC_I2C2_CLK_DISABLE();

  /* DeInitialize peripheral GPIOs */
  HAL_GPIO_DeInit(BUS_I2C2_SDA_GPIO_PORT, BUS_I2C2_SDA_GPIO_PIN);
  HAL_GPIO_DeInit(BUS_I2C2_SCL_GPIO_PORT, BUS_I2C2_SCL_GPIO_PIN);

}

