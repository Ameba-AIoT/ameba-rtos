/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _RL7005_OSC131K_H_
#define _RL7005_OSC131K_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup OSC131K
  * @brief OSC131K driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup OSC131K_Register_Definitions OSC131K Register Definitions
  * @{
  */

/** @defgroup RTC_OSC131K_POW
  * @brief
  * @{
  */
#define RTC_MASK_REG_32KOSC_DUMMY   ((u32)0x00000003 << 17)           /*!< R/W 2'b0  // [RF][32KOSC] dummy*/
#define RTC_REG_32KOSC_DUMMY(x)     (((u32)((x) & 0x00000003) << 17))
#define RTC_GET_REG_32KOSC_DUMMY(x) ((u32)(((x >> 17) & 0x00000003)))
#define RTC_BIT_GATED_STUP_OK_D     ((u32)0x00000001 << 16)           /*!< R/W 1'b0  // [RF][32KOSC] dummy*/
#define RTC_GATED_STUP_OK_D(x)      ((u32)(((x) & 0x00000001) << 16))
#define RTC_MASK_DMY_1              ((u32)0x00007FFF << 1)            /*!< R/W 15'd0  */
#define RTC_DMY_1(x)                (((u32)((x) & 0x00007FFF) << 1))
#define RTC_GET_DMY_1(x)            ((u32)(((x >> 1) & 0x00007FFF)))
#define RTC_BIT_POW_32KOSC          ((u32)0x00000001 << 0)            /*!< R/W 1'b1  // [RF][32KOSC] 1'b1 : enable*/
#define RTC_POW_32KOSC(x)           ((u32)(((x) & 0x00000001) << 0))
/** @} */

/** @defgroup RTC_OSC131K_CTRL
  * @brief
  * @{
  */
#define RTC_BIT_FEN_RTC       ((u32)0x00000001 << 16)           /*!< R/W 1'b0  1: Enable RTC function 0: Disable*/
#define RTC_FEN_RTC(x)        ((u32)(((x) & 0x00000001) << 16))
#define RTC_BIT_SEL_LDO_VREF  ((u32)0x00000001 << 15)           /*!< R/W 1'b0  // [RF][32KOSC] REG_CTRL // 0: LDO low valtage setting 1:high voltage*/
#define RTC_SEL_LDO_VREF(x)   ((u32)(((x) & 0x00000001) << 15))
#define RTC_BIT_GATED_STUP_OK ((u32)0x00000001 << 14)           /*!< R/W 1'b0  //1'b1 : Close Startup standby current ; //1'b0 : Startup standby current*/
#define RTC_GATED_STUP_OK(x)  ((u32)(((x) & 0x00000001) << 14))
#define RTC_MASK_DMY_2        ((u32)0x000000FF << 6)            /*!< R/W 8'b00000000  // [RF][32KOSC] REG_CTRL // dummy*/
#define RTC_DMY_2(x)          (((u32)((x) & 0x000000FF) << 6))
#define RTC_GET_DMY_2(x)      ((u32)(((x >> 6) & 0x000000FF)))
#define RTC_MASK_RCAL         ((u32)0x0000003F << 0)            /*!< R/W 6'b100000  // [RF][32KOSC] REG_CTRL // osc frequency is highier as RCAL's larger*/
#define RTC_RCAL(x)           (((u32)((x) & 0x0000003F) << 0))
#define RTC_GET_RCAL(x)       ((u32)(((x >> 0) & 0x0000003F)))
/** @} */

/** @defgroup RTC_OSC131K_STATE
  * @brief
  * @{
  */
#define RTC_BIT_SCAN_MODE  ((u32)0x00000001 << 16)           /*!< R/WH 0x0  Scan mode enable for RTC domain only .*/
#define RTC_SCAN_MODE(x)   ((u32)(((x) & 0x00000001) << 16))
#define RTC_MASK_DBG_SEL   ((u32)0x0000001F << 8)            /*!< R/W 0x0  rtc debug select for RTC domain PAD*/
#define RTC_DBG_SEL(x)     (((u32)((x) & 0x0000001F) << 8))
#define RTC_GET_DBG_SEL(x) ((u32)(((x >> 8) & 0x0000001F)))
#define RTC_BIT_CKSL_RTC   ((u32)0x00000001 << 1)            /*!< R/W 0x1  rtc clock select. 0: sdm32k 1: osc131k div4 Note:when aon power off, it will be reset to 1 because sdm is off. when aon power on again , SW must set this bit to 0 after SDM config done.*/
#define RTC_CKSL_RTC(x)    ((u32)(((x) & 0x00000001) << 1))
#define RTC_BIT_FIRST_PON  ((u32)0x00000001 << 0)            /*!< R/WH 0x0  Default is 0 , after SW write 1 , it will be tie to 1 , until next reset . This indicate RTC first pon state*/
#define RTC_FIRST_PON(x)   ((u32)(((x) & 0x00000001) << 0))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup OSC131K_Exported_Types OSC131K Exported Types
  * @{
  */

/** @brief OSC131K Register Declaration
  */

typedef struct {
	__IO uint32_t RTC_OSC131K_POW  ;  /*!< Register,  Address offset:0x000 */
	__IO uint32_t RTC_OSC131K_CTRL ;  /*!< Register,  Address offset:0x004 */
	__IO uint32_t RTC_OSC131K_STATE;  /*!< Register,  Address offset:0x008 */
} RTC_MISC_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

#define RTC_MISC_REG_BASE 0x4080AA80
#define RTC_MISC_OSC131K_POW 0x0
#define RTC_MISC_OSC131K_CTRL 0x4
#define RTC_MISC_OSC131K_STATE 0x8
#define SDM32K 0x0
#define OSC131K_DIV4 0x1

/* MANUAL_GEN_END */

/** @} */

/** @} */

#endif