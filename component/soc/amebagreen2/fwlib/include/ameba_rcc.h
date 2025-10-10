/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_HP_RCC_H_
#define _AMEBA_HP_RCC_H_

/** @addtogroup AmebaD_Platform
  * @{
  */

/** @defgroup RCC
  * @brief RCC driver modules
  * @{
  */


/** @defgroup HS_RCC
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  *		- functions prototypes for peripheral reset and clock control driver.
  *
  *****************************************************************************************
  *
  *****************************************************************************************
  * how to use
  *****************************************************************************************
  * use UART0 as example:
  *		RCC_PeriphClockCmd(APBPeriph_UART0, APBPeriph_UART0_CLOCK, ENABLE);
  *
  *****************************************************************************************
  * @endverbatim
  */

/** @addtogroup HS_RCC
  * @brief HS_RCC driver modules
  * @{
  */

/* Exported constants --------------------------------------------------------*/

#define APBPeriph_NULL					0x0
#define APBPeriph_CLOCK_NULL			0x0

/** @defgroup SPIC_Clock_Source_define
  * @{
  */
#define BIT_LSYS_CKSL_SPIC_PLL			1
#define BIT_LSYS_CKSL_SPIC_XTAL			0
/**
  * @}
  */

/** @defgroup PSRAM_Clock_Source_define
  * @{
  */
#define BIT_LSYS_CKSL_PSRAM_PLL			0x1
#define BIT_LSYS_CKSL_PSRAM_XTAL		0x0
/**
  * @}
  */


/** @defgroup HPERI_Clock_Source_define
  * @{
  */
#define BIT_LSYS_CKSL_HPERI_PLL			1
#define BIT_LSYS_CKSL_HPERI_XTAL		0
/**
  * @}
  */


/** @defgroup SPIC_Clock_Source_define
  * @{
  */
#define BIT_LSYS_CKSL_HP_PLL			1
#define BIT_LSYS_CKSL_HP_XTAL			0
/**
  * @}
  */


/** @defgroup SDM_Clock_Source_define
  * @{
  */
#define CKSL_SDM_OSC131K				0x00
#define CKSL_SDM_XTAL_LPS312P5K			0x01
/**
  * @}
  */


/** @defgroup VADMEM_Clock_Source_define
  * @{
  */
#define CKSL_VADM_HS_PLFM				0x00
#define CKSL_VADM_VAD					0x01
/**
  * @}
  */

/** @defgroup AUDIOCODEC_Clock_Source_define
  * @{
  */
#define CKSL_AC_SYSPLL					0x00
#define CKSL_AC_XTAL					0x01
/**
  * @}
  */

/** @defgroup CTC_Clock_Source_define
  * @{
  */
#define CKSL_CTC_LBUS				0x00
#define CKSL_CTC_IPCLK				0x01
/**
  * @}
  */


/** @defgroup ADC_Clock_Source_define
  * @{
  */
#define CKSL_ADC_LBUS				0x00
#define CKSL_ADC_IPCLK				0x01
/**
  * @}
  */

/** @defgroup GPIO_Clock_Source_define
  * @{
  */
#define CKSL_GPIO_LS_APB				0x00
#define CKSL_GPIO_32K					0x01
/**
  * @}
  */


/** @defgroup I2S_Clock_Source_define
  * @{
  */

#define CKSL_I2S_XTAL40M			0x00
#define CKSL_I2S_CPUPLL				0x01
/**
  * @}
  */

/** @defgroup UART_RX_Clock_Source_define
  * @{
  */
#define UART_RX_CLK_XTAL_40M	((u32)0x00000000)
#define UART_RX_CLK_OSC_LP		((u32)0x00000001)
/**
  * @}
  */

/** @defgroup 32K_Clock_Source_define
  * @{
  */
#define CKSL_32K_SDM32K			0x00
#define CKSL_32K_EXTERNAL_32K		0x03

/**
  * @}
  */

/** @defgroup LOGUART_Clock_Source_define
  * @{
  */
#define LOGUART_CLK_XTAL_40M	((u32)0x00000000)
#define LOGUART_CLK_OSC_LP		((u32)0x00000001)
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup HS_RCC_Exported_Functions HS_RCC Exported Functions
  * @{
  */
_LONG_CALL_ void RCC_PeriphClockCmd(u32 APBPeriph, u32 APBPeriph_Clock, u8 NewState);
_LONG_CALL_ u32 RCC_PeriphClockEnableChk(u32 APBPeriph_Clock_in);
_LONG_CALL_ void RCC_PeriphClockSource_SPORT(AUDIO_SPORT_TypeDef *Sportx, u32 Source);
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

/* Registers Definitions --------------------------------------------------------*/
/* Other definations --------------------------------------------------------*/

#endif /* _AMEBA_HP_RCC_H_ */
