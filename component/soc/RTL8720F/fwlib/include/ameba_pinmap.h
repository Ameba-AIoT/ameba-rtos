/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_PINMAP_H_
#define _AMEBA_PINMAP_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup PINMAP
  * @brief PINMAP driver modules
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup PINMAP_Exported_Types PINMAP Exported Types
  * @{
  */

/**
  * @brief  PINMAP Init structure definition
  */
typedef struct {
	u16 PinName; /*!< Specifies the pin name, This parameter can be a value of @ref PINMUX_Pin_Name_definitions */
	u8 FuncPuPd;/*!< Specifies the pin function PU/PD, This parameter can be a value of @ref GPIO_Pull_parameter_definitions */
	u8 SleepPuPd;/*!< Specifies the pin sleep PU/PD, This parameter can be a value of @ref GPIO_Pull_parameter_definitions */
} PMAP_TypeDef;
/** @} */

/* Exported functions --------------------------------------------------------*/
/** @defgroup PINMAP_Exported_Functions PINMAP Exported Functions
  * @{
  */
void pinmap_init(void);

#ifdef __cplusplus
}
#endif

/** @} */

/** @} */

/** @} */

/* Other definations --------------------------------------------------------*/
#define GPIO_PuPd_KEEP	0xFF /* keep pupd unchanged */
#endif //_AMEBA_PINMAP_H_