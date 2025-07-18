/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_WDG_H_
#define _AMEBA_WDG_H_

/** @addtogroup WDG
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * WDG:
  * 	- Base Address: VENDOR_REG_BASE
  * 	- Timer clk: IWDG 100K Aon clk and WDG1~WDG4 SDM 32k
  * 	- Generates MCU reset or WDG interrupt on expiry of a programmed time period,
  * 	  unless the program refreshes the watchdog
  * 	- IRQ: WDG_IRQ
  *

  *****************************************************************************************
  * How to use WGD
  *****************************************************************************************
  *      To use WDG peripheral, the following steps are mandatory:
  *
  *      1. Configure WDG with the corresponding configuration.
  *			WDG_Init(&WDG_InitStruct)
  *
  *      2. Activate the WDG peripheral:
  *			WDG_Cmd(ENABLE).
  *
  *      @note	WDG_Refresh() function is used to clear timer, if call this function before timeout period,
  *			then MCU reset or WDG interrupt won't generate
  *
  *****************************************************************************************
  * @endverbatim
  */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup WDG
  * @brief WDG driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup WDG_Register_Definitions WDG Register Definitions
  * @{
  */

/** @defgroup WDG_MKEYR
  * @brief WDG Magic Key register
  * @{
  */
#define WDG_MASK_MKEY   ((u32)0x0000FFFF << 0)           /*!< R/WPD 0h  0x6969: enable access to register WDG_CR/WDG_RLR/WDG_WINR 0x5A5A: reload WDG counter 0x3C3C: enable WDG function*/
#define WDG_MKEY(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define WDG_GET_MKEY(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup WDG_CR
  * @brief WDG Control regsietr
  * @{
  */
#define WDG_BIT_RVU      ((u32)0x00000001 << 31)          /*!< R 0h  Watchdog counter update by reload command*/
#define WDG_BIT_EVU      ((u32)0x00000001 << 30)          /*!< R 0h  Watchdog early interrupt function update*/
#define WDG_BIT_LPEN     ((u32)0x00000001 << 24)          /*!< R/WE 1h  Low power enable 0: WDG will gating when system goes into sleep mode 1: WDG keep running when system goes into sleep mode*/
#define WDG_BIT_EIC      ((u32)0x00000001 << 17)          /*!< WA0 0h  Write '1' clear the early interrupt*/
#define WDG_BIT_EIE      ((u32)0x00000001 << 16)          /*!< R/WE 0h  Watchdog early interrupt enable*/
#define WDG_MASK_EICNT   ((u32)0x0000FFFF << 0)           /*!< R/WE 0h  Early interrupt trigger threshold*/
#define WDG_EICNT(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define WDG_GET_EICNT(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup WDG_RLR
  * @brief WDG Relaod register
  * @{
  */
#define WDG_MASK_PRER     ((u32)0x000000FF << 16)           /*!< R/WE 63h  Prescaler counter, configuration only allowed before wdg enable   WDG: 0x63 System wdg: 0x1F*/
#define WDG_PRER(x)       (((u32)((x) & 0x000000FF) << 16))
#define WDG_GET_PRER(x)   ((u32)(((x >> 16) & 0x000000FF)))
#define WDG_MASK_RELOAD   ((u32)0x0000FFFF << 0)            /*!< R/WE FFFFh  Reload value for watchdog counter Default : 16s (x100k)*/
#define WDG_RELOAD(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define WDG_GET_RELOAD(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup WDG_WINR
  * @brief WDG Window Register
  * @{
  */
#define WDG_MASK_WINDOW   ((u32)0x0000FFFF << 0)           /*!< R/WE FFFFh  Watchdog feed protect window register*/
#define WDG_WINDOW(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define WDG_GET_WINDOW(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup WDG_Exported_Types WDG Exported Types
  * @{
  */

/** @brief WDG Register Declaration
  */

typedef struct {
	__IO uint32_t WDG_MKEYR ;  /*!< WDG MAGIC KEY REGISTER,  Address offset:0x000 */
	__IO uint32_t WDG_CR    ;  /*!< WDG CONTROL REGSIETR Register,  Address offset:0x004 */
	__IO uint32_t WDG_RLR   ;  /*!< WDG RELAOD REGISTER,  Address offset:0x008 */
	__IO uint32_t WDG_WINR  ;  /*!< WDG WINDOW REGISTER,  Address offset:0x00C */
	__IO uint32_t IWDG_DUMMY;  /*!< WDG DUMMY REGISTER,  Address offset:0x010 */
} WDG_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your defination here

/* Exported types ------------------------------------------------------------*/
/** @addtogroup WDG_Exported_Types WDG Exported Types
  * @{
  */

/**
  * @brief  WDG Init Structure Definition
  */
typedef struct {
	u16 Window; 		/*!< WDG parameter specifies window protection of WDG, the value cannot be changed when WDG is Enabled
								This parameter must be set to a value in the 0-65535 range */

	u16 Timeout; 		/*!< WDG parameter specifies WDG timeout count in units of ms
								This parameter must be set to a value in the 1-65535 range */

	u16 EICNT; 			/*!< WDG parameter specifies WDG early interrupt trigger threshold
								This parameter must be set to a value in the 1-65535 range */

	u16 EIMOD;			/*!< WDG parameter, Specifies WDG early interrupt enable or not
								This parameter must be set to a value of 0 or 1 */
} WDG_InitTypeDef;

/**
  * @}
  */

/* Exported constants ------------------------------------------------------------*/
/** @defgroup WDG_Exported_Constants WDG Exported Constants
  * @{
  */

/** @defgroup WDG_Peripheral_Definitions
  * @{
  */
#define IS_SYETEM_WDG_PERIPH(PERIPH) (((PERIPH) == CPU1_WDG_DEV) || ((PERIPH) == CPU0_S_WDG_DEV) || ((PERIPH) == CPU0_NS_WDG_DEV))
#define IS_IWDG_PERIPH(PERIPH) ((PERIPH) == IWDG_DEV)
#define IS_WDG_ALL_PERIPH(PERIPH) (IS_IWDG_PERIPH(PERIPH) || IS_SYETEM_WDG_PERIPH(PERIPH))

/**
  * @}
  */

/** @defgroup WDG_Magic_Key
  * @{
  */
#define WDG_ACCESS_EN                                   0x00006969
#define WDG_FUNC_EN                                     0x00003C3C
#define WDG_REFRESH                                     0x00005A5A
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions ------------------------------------------------------------*/
/** @defgroup WDG_Exported_Functions WDG Exported Functions
  * @{
  */

_LONG_CALL_ void WDG_StructInit(WDG_InitTypeDef *WDG_InitStruct);
_LONG_CALL_ void WDG_Init(WDG_TypeDef *WDG, WDG_InitTypeDef *WDG_InitStruct);
_LONG_CALL_ void WDG_Enable(WDG_TypeDef *WDG);
_LONG_CALL_ void WDG_Timeout(WDG_TypeDef *WDG, u32 Timeout);
_LONG_CALL_ void WDG_Refresh(WDG_TypeDef *WDG);
_LONG_CALL_ void WDG_INTConfig(WDG_TypeDef *WDG, u32 WDG_IT, u32 NewState);
_LONG_CALL_ void WDG_ClearINT(WDG_TypeDef *WDG, u32 INTrBit);
_LONG_CALL_ void IWDG_LP_Enable(WDG_TypeDef *WDG, u32 NewState);

/**
  * @}
  */

/* MANUAL_GEN_END */

#endif

/** @} */

/** @} */