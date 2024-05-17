/**
  ******************************************************************************
  * @file    b_u585i_iot02a_conf_template.h
  * @author  MCD Application Team
  * @brief   configuration file.
  *          This file should be copied to the application folder and renamed
  *          to b_u585i_iot02a_conf.h
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef B_U585I_IOT02A_CONF_H
#define B_U585I_IOT02A_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup B_U585I_IOT02A
  * @{
  */

/** @defgroup B_U585I_IOT02A_CONFIG Config
  * @{
  */

/** @defgroup B_U585I_IOT02A_CONFIG_Exported_Constants Exported Constants
  * @{
  */

/* COM define */
#define USE_BSP_COM_FEATURE                  0U
#define USE_COM_LOG                          0U

/* Default EEPROM max trials */
#define EEPROM_MAX_TRIALS                   3000U

/* IRQ priorities */
#define BSP_BUTTON_USER_IT_PRIORITY   15U  /* Default is lowest priority level */

/* Audio interrupt priority */
#define BSP_AUDIO_IN_IT_PRIORITY      15U  /* Default is lowest priority level */

/* CAMERA interrupt priority */
#define BSP_CAMERA_IT_PRIORITY        14U  /* Default is lowest priority level */

/* I2C1 and I2C2 Frequencies in Hz  */
#define BUS_I2C1_FREQUENCY                   100000UL /* Frequency of I2C1 = 100 KHz*/
#define BUS_I2C2_FREQUENCY                   100000UL /* Frequency of I2C2 = 100 KHz*/

/* Usage of USBPD PWR TRACE system */
#define USE_BSP_USBPD_PWR_TRACE       0U      /* USBPD BSP trace system is disabled */

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

#ifdef __cplusplus
}
#endif

#endif /* B_U585I_IOT02A_CONF_H */
