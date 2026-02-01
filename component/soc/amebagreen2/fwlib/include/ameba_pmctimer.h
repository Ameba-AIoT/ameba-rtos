/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_PMCTIMER_H_
#define _AMEBA_PMCTIMER_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup PMCTIMER
  * @brief PMCTIMER driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup PMCTIMER_Register_Definitions PMCTIMER Register Definitions
  * @{
  */

/** @defgroup PMCT_TIM_EN
  * @brief TIM enable register
  * @{
  */
#define PMCT_BIT_RESET_TIMER3        ((u32)0x00000001 << 31) /*!< WA0 0x0  Writing 1 will cause a pulse reset timer1. Please wait for 3 work clk before  new timer configuration 1: valid 0: not valid*/
#define PMCT_BIT_RESET_TIMER2        ((u32)0x00000001 << 30) /*!< WA0 0x0  Writing 1 will cause a pulse reset timer1. Please wait for 3 work clk before  new timer configuration 1: valid 0: not valid*/
#define PMCT_BIT_RESET_TIMER1        ((u32)0x00000001 << 29) /*!< WA0 0x0  Writing 1 will cause a pulse reset timer1. Please wait for 3 work clk before  new timer configuration 1: valid 0: not valid*/
#define PMCT_BIT_RESET_TIMER0        ((u32)0x00000001 << 28) /*!< WA0 0x0  Writing 1 will cause a pulse reset timer0. Please wait for 3 work clk before  new timer configuration 1: valid 0: not valid*/
#define PMCT_BIT_CONFIG_VALID_TIMER3 ((u32)0x00000001 << 11) /*!< R 0x1  TIM_SET configuration valid or not.  1: valid 0: not valid if compare option enable, 1: vaild value is seted 0: set option is on going*/
#define PMCT_BIT_CONFIG_VALID_TIMER2 ((u32)0x00000001 << 10) /*!< R 0x1  TIM_SET configuration valid or not 1: valid 0: not valid*/
#define PMCT_BIT_CONFIG_VALID_TIMER1 ((u32)0x00000001 << 9)  /*!< R 0x1  TIM_SET configuration valid or not 1: valid 0: not valid*/
#define PMCT_BIT_CONFIG_VALID_TIMER0 ((u32)0x00000001 << 8)  /*!< R 0x1  TIM_SET configuration valid or not 1: valid 0: not valid*/
#define PMCT_BIT_CNT_START_TIMER3    ((u32)0x00000001 << 3)  /*!< R/W 0x1  enable the timer3 counter. 1:enable 0:disable*/
#define PMCT_BIT_CNT_START_TIMER2    ((u32)0x00000001 << 2)  /*!< R/W 0x1  enable the timer2 counter. 1:enable 0:disable*/
#define PMCT_BIT_CNT_START_TIMER1    ((u32)0x00000001 << 1)  /*!< R/W 0x1  enable the timer1 counter. 1:enable 0:disable*/
#define PMCT_BIT_CNT_START_TIMER0    ((u32)0x00000001 << 0)  /*!< R/W 0x1  enable the timer0 counter. 1:enable 0:disable*/
/** @} */

/** @defgroup PMCT_TIM_DIER
  * @brief TIM interupt enable register
  * @{
  */
#define PMCT_BIT_UIE_TIMER3 ((u32)0x00000001 << 3) /*!< R/W 0x0  1: enable update interrupt 0: disable update interrupt*/
#define PMCT_BIT_UIE_TIMER2 ((u32)0x00000001 << 2) /*!< R/W 0x0  1: enable update interrupt 0: disable update interrupt*/
#define PMCT_BIT_UIE_TIMER1 ((u32)0x00000001 << 1) /*!< R/W 0x0  1: enable update interrupt 0: disable update interrupt*/
#define PMCT_BIT_UIE_TIMER0 ((u32)0x00000001 << 0) /*!< R/W 0x0  1: enable update interrupt 0: disable update interrupt*/
/** @} */

/** @defgroup PMCT_TIM_ISR
  * @brief TIM interrupt status register
  * @{
  */
#define PMCT_BIT_ISR_TIMER3 ((u32)0x00000001 << 3) /*!< RW1C 0x0  Timer interrupt status 0: no interrupt 1: interrupt occurs Write 1 to clear interrupts in interrupt handler.*/
#define PMCT_BIT_ISR_TIMER2 ((u32)0x00000001 << 2) /*!< RW1C 0x0  Timer interrupt status 0: no interrupt 1: interrupt occurs Write 1 to clear interrupts in interrupt handler.*/
#define PMCT_BIT_ISR_TIMER1 ((u32)0x00000001 << 1) /*!< RW1C 0x0  Timer interrupt status 0: no interrupt 1: interrupt occurs Write 1 to clear interrupts in interrupt handler.*/
#define PMCT_BIT_ISR_TIMER0 ((u32)0x00000001 << 0) /*!< RW1C 0x0  Timer interrupt status 0: no interrupt 1: interrupt occurs Write 1 to clear interrupts in interrupt handler.*/
/** @} */

/** @defgroup PMCT_TIM_CNTx
  * @brief TIM counterx
  * @{
  */
#define PMCT_MASK_CNTx   ((u32)0xFFFFFFFF << 0)           /*!< R 0xFFFFFFFF  counter value, counter will stop automatically when it counts down to zero.*/
#define PMCT_CNTx(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PMCT_GET_CNTx(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PMCT_TIM_SETx
  * @brief TIM set registerx
  * @{
  */
#define PMCT_MASK_TIM_SETx   ((u32)0xFFFFFFFF << 0)           /*!< R/WPD 0x0  Set a countdown value. This value come into force when TIM_EN register bit is enabled. counter will count down from this value immediately.*/
#define PMCT_TIM_SETx(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PMCT_GET_TIM_SETx(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PMCT_TIM_COM_EN
  * @brief TIM  compare register
  * @{
  */
#define PMCT_MASK_COM_EN   ((u32)0x0000000F << 0)           /*!< R/W 0xF  enable compare option.The set cnt value takes effect only when it is greater than the actual cnt value 0:disable 1:enable [0] : control tim0 set ...... [3]: control tim3 set*/
#define PMCT_COM_EN(x)     (((u32)((x) & 0x0000000F) << 0))
#define PMCT_GET_COM_EN(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup PMCTIMER_Exported_Types PMCTIMER Exported Types
  * @{
  */

/** @brief PMCTIMER Register Declaration
  */

typedef struct {
	__IO uint32_t PMCT_TIM_EN      ;  /*!< TIM ENABLE REGISTER,  Address offset:0x000 */
	__IO uint32_t PMCT_TIM_DIER    ;  /*!< TIM INTERUPT ENABLE REGISTER,  Address offset:0x004 */
	__IO uint32_t PMCT_TIM_ISR     ;  /*!< TIM INTERRUPT STATUS REGISTER,  Address offset:0x008 */
	__I  uint32_t PMCT_TIM_CNTx[4] ;  /*!< TIM COUNTERx Register,  Address offset:0x00C-0x018 */
	__IO uint32_t PMCT_TIM_SETx[4] ;  /*!< TIM SET REGISTERx Register,  Address offset:0x01C-0x028 */
	__IO uint32_t PMCT_TIM_COM_EN  ;  /*!< TIM  COMPARE REGISTER,  Address offset:0x02C */
	__IO uint32_t PMCT_PMCTIM_DUMMY;  /*!< PMCTIMER DUMMY REGISTER,  Address offset:0x030 */
} PMCTIMER_TpyeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here

/* Exported constants --------------------------------------------------------*/
/** @defgroup PMCTIMER_Exported_Constants PMCTIMER Exported Constants
  * @{
  */

/** @defgroup PMCTIMER_Device_Config
  * @{
  */
#ifdef CONFIG_ARM_CORE_CM4_KM4TZ
#define PMC_TIMER_DEV PMCTIMER_DEV0
#define PMC_TIMER_IRQ PMC_TIMER0_IRQ
#define PMC_TIMER_INT_PRIO INT_PRI6
#else
#define PMC_TIMER_DEV PMCTIMER_DEV1
#define PMC_TIMER_IRQ PMC_TIMER1_IRQ
#define PMC_TIMER_INT_PRIO INT_PRI_LOWEST
#endif
/**
  * @}
  */

/** @defgroup PMCTIMER_Index
  * @{
  */
#define PMC_SLEEP_TIMER		0 /* sys active time for sleep */
#define PMC_DSLP_TIMER		1 /* sys active time for deepsleep */
#define PMC_WAKEUP_TIMER	2 /* set wakeup time for sleep */
#define PMC_RSVD_TIMER		3
/**
  * @}
  */

/** @defgroup PMCTIMER_Valid_Check
  * @{
  */
#define PMCT_BIT_TIM_VALIDx(x) ((u32)(0x00000001 << (8 + x)))
/**
  * @}
  */

/** @defgroup PMCTIMER_Reset
  * @{
  */
#define PMCTRESET_BIT_TIMERx(x) ((u32)(0x00000001 << (28 + x)))
/**
  * @}
  */

/**
  * @}
  */

/** @} */

/** @} */

/* group: PMCTIMER_Exported_Functions */
/* Since all the members in the group are __STATIC_INLINE which will not be extracted to the doxygen doc,
  original comment has been deleted to avoid there is nothing displayed under the group. */
void PMCTimer_Cmd(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx, bool NewState);
u32 PMCTimer_Valid_Check(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx);
u32 PMCTimerCnt_Get(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx);
void PMCTimerCnt_Set(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx, u32 NewVaule);
void PMCTimerCnt_Reset(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx);
void PMCTimer_INTConfig(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx, u32 NewState);
void PMCTimer_INTClear(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx);
void PMCTimer_CompCmd(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx, u32 NewState);
void PMCTimer_Init(void);
void PMCTimer_Reset(void);

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif