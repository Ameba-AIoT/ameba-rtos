/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_PMCTIMER_H_
#define _AMEBA_PMCTIMER_H_

/* AUTO_GEN_START */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup PMCTIMER
* @brief PMCTIMER driver modules
* @{
*/

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup PMCTIMER_Register_Definitions PMCTIMER Register Definitions
 * @{
 *****************************************************************************/

/** @defgroup PMCTTIM_EN
 * @brief TIM enable register
 * @{
 **/
#define PMCTRESET_BIT_TIMER3          ((u32)0x00000001 << 31)          /*!<WA0 0x0  Writing 1 will cause a pulse reset timer1. Please wait for 3 work clk before new timer configuration 1: valid 0: not valid */
#define PMCTRESET_BIT_TIMER2          ((u32)0x00000001 << 30)          /*!<WA0 0x0  Writing 1 will cause a pulse reset timer1. Please wait for 3 work clk before new timer configuration 1: valid 0: not valid */
#define PMCTRESET_BIT_TIMER1          ((u32)0x00000001 << 29)          /*!<WA0 0x0  Writing 1 will cause a pulse reset timer1. Please wait for 3 work clk before new timer configuration 1: valid 0: not valid */
#define PMCTRESET_BIT_TIMER0          ((u32)0x00000001 << 28)          /*!<WA0 0x0  Writing 1 will cause a pulse reset timer0. Please wait for 3 work clk before new timer configuration 1: valid 0: not valid */
#define PMCTCONFIG_BIT_VALID_TIMER3   ((u32)0x00000001 << 11)          /*!<R 0x1  TIM_SET configuration valid or not. 1: valid 0: not valid if compare option enable, 1: vaild value is seted 0: set option is on going */
#define PMCTCONFIG_BIT_VALID_TIMER2   ((u32)0x00000001 << 10)          /*!<R 0x1  TIM_SET configuration valid or not 1: valid 0: not valid */
#define PMCTCONFIG_BIT_VALID_TIMER1   ((u32)0x00000001 << 9)          /*!<R 0x1  TIM_SET configuration valid or not 1: valid 0: not valid */
#define PMCTCONFIG_BIT_VALID_TIMER0   ((u32)0x00000001 << 8)          /*!<R 0x1  TIM_SET configuration valid or not 1: valid 0: not valid */
#define PMCTCNT_BIT_START_TIMER3      ((u32)0x00000001 << 3)          /*!<R/W 0x1  enable the timer3 counter. 1：enable 0：disable */
#define PMCTCNT_BIT_START_TIMER2      ((u32)0x00000001 << 2)          /*!<R/W 0x1  enable the timer2 counter. 1：enable 0：disable */
#define PMCTCNT_BIT_START_TIMER1      ((u32)0x00000001 << 1)          /*!<R/W 0x1  enable the timer1 counter. 1：enable 0：disable */
#define PMCTCNT_BIT_START_TIMER0      ((u32)0x00000001 << 0)          /*!<R/W 0x1  enable the timer0 counter. 1：enable 0：disable */
/** @} */

/** @defgroup PMCTTIM_DIER
 * @brief TIM interupt enable register
 * @{
 **/
#define PMCTUIE_BIT_TIMER3            ((u32)0x00000001 << 3)          /*!<R/W 0x0  1: enable update interrupt 0: disable update interrupt */
#define PMCTUIE_BIT_TIMER2            ((u32)0x00000001 << 2)          /*!<R/W 0x0  1: enable update interrupt 0: disable update interrupt */
#define PMCTUIE_BIT_TIMER1            ((u32)0x00000001 << 1)          /*!<R/W 0x0  1: enable update interrupt 0: disable update interrupt */
#define PMCTUIE_BIT_TIMER0            ((u32)0x00000001 << 0)          /*!<R/W 0x0  1: enable update interrupt 0: disable update interrupt */
/** @} */

/** @defgroup PMCTTIM_ISR
 * @brief TIM interrupt status register
 * @{
 **/
#define PMCTISR_BIT_TIMER3            ((u32)0x00000001 << 3)          /*!<RW1C 0x0  Timer interrupt status 0: no interrupt 1: interrupt occurs Write 1 to clear interrupts in interrupt handler. */
#define PMCTISR_BIT_TIMER2            ((u32)0x00000001 << 2)          /*!<RW1C 0x0  Timer interrupt status 0: no interrupt 1: interrupt occurs Write 1 to clear interrupts in interrupt handler. */
#define PMCTISR_BIT_TIMER1            ((u32)0x00000001 << 1)          /*!<RW1C 0x0  Timer interrupt status 0: no interrupt 1: interrupt occurs Write 1 to clear interrupts in interrupt handler. */
#define PMCTISR_BIT_TIMER0            ((u32)0x00000001 << 0)          /*!<RW1C 0x0  Timer interrupt status 0: no interrupt 1: interrupt occurs Write 1 to clear interrupts in interrupt handler. */
/** @} */

/** @defgroup PMCTTIM_CNTx
 * @brief TIM counterx
 * @{
 **/
#define PMCTCNTx_MASK                 ((u32)0xFFFFFFFF << 0)          /*!<R 0xFFFFFFFF  counter value, counter will stop automatically when it counts down to zero. */
#define PMCTCNTx(x)                   (((u32)((x) & 0xFFFFFFFF) << 0))
#define PMCTCNTx_GET(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PMCTTIM_SETx
 * @brief TIM set registerx
 * @{
 **/
#define PMCTTIM_MASK_SETx             ((u32)0xFFFFFFFF << 0)          /*!<R/WPD 0x0  Set a countdown value. This value come into force when TIM_EN register bit is enabled. counter will count down from this value immediately. */
#define PMCTTIM_SETx(x)               (((u32)((x) & 0xFFFFFFFF) << 0))
#define PMCTTIM_GET_SETx(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PMCTTIM_com_en
 * @brief TIM  compare register
 * @{
 **/
#define PMCTCOM_MASK_EN               ((u32)0x0000000F << 0)          /*!<R/W 0xF  enable compare option.The set cnt value takes effect only when it is greater than the actual cnt value 0:disable 1:enable [0] : control tim0 set …… [3]: control tim3 set */
#define PMCTCOM_EN(x)                 (((u32)((x) & 0x0000000F) << 0))
#define PMCTCOM_GET_EN(x)             ((u32)(((x >> 0) & 0x0000000F)))
/** @} */
/** @} */



/* Exported types --------------------------------------------------------*/

/** @defgroup PMCTIMER_Exported_Types PMCTIMER Exported Types
  * @{
*****************************************************************************/


/**
 * @brief PMCTIMER Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t PMCTTIM_EN;                             /*!< TIM ENABLE REGISTER,  Address offset: 0x000 */
	__IO uint32_t PMCTTIM_DIER;                           /*!< TIM INTERUPT ENABLE REGISTER,  Address offset: 0x004 */
	__IO uint32_t PMCTTIM_ISR;                            /*!< TIM INTERRUPT STATUS REGISTER,  Address offset: 0x008 */
	__I  uint32_t PMCTTIM_CNTx[4];                        /*!< TIM COUNTERx Register,  Address offset: 0x00C-0x018 */
	__IO uint32_t PMCTTIM_SETx[4];                        /*!< TIM SET REGISTERx Register,  Address offset: 0x01C-0x028 */
	__IO uint32_t PMCTTIM_COM_EN;                         /*!< TIM  COMPARE REGISTER,  Address offset: 0x02C */
	__IO uint32_t PMCTIM_DUMMY;                           /*!< PMCTIMER DUMMY REGISTER,  Address offset: 0x030 */
} PMCTIMER_TpyeDef;
/** @} */
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your defination here

/* Exported constants --------------------------------------------------------*/
/** @defgroup PMCTIMER_Exported_Constants PMCTIMER Exported Constants
  * @{
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


/* Exported Functions --------------------------------------------------------*/
/** @defgroup PMCTIMER_Exported_Functions PMCTIMER Exported Functions
  * @{
  */

/**
  * @brief  Enables or disables the pmc timer counter
  *   The pmc timer group contains 4 timers for each group, all runs at 32k.
  * @param  PMC_TIMER: the pointer of PMC TIMER DEV.
  * @param  NewState: new state of the Debug timer peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @param  Timer_Idx: timer index in PMC TIMER DEV.
  *   This parameter can be: 0 ~ 3.
  * @retval None
  */
__STATIC_INLINE void PMCTimer_Cmd(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx, BOOL NewState)
{
	if (NewState) {
		PMC_TIMER->PMCTTIM_EN |= BIT(Timer_Idx);
	} else {
		PMC_TIMER->PMCTTIM_EN &= ~BIT(Timer_Idx);
	}
}

/**
  * @brief  Check pmctimer current state.
  * @param  PMC_TIMER: the pointer of PMC TIMER DEV.
  * @param  Timer_Idx: timer index in PMC TIMER DEV.
  *   This parameter can be: 0 ~ 3.
  * @retval valid or not
  */
__STATIC_INLINE u32 PMCTimer_Valid_Check(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx)
{
	if (PMC_TIMER->PMCTTIM_EN & PMCT_BIT_TIM_VALIDx(Timer_Idx)) {
		return _TRUE;
	} else {
		return _FALSE;
	}
}

/**
  * @brief  Get the pmc timer current Counter value.
  * @param  PMC_TIMER: the pointer of PMC TIMER DEV.
  * @param  Timer_Idx: timer index in PMC TIMER DEV.
  *   This parameter can be: 0 ~ 3.
  * @retval current counter
  */
__STATIC_INLINE u32 PMCTimerCnt_Get(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx)
{
	return PMC_TIMER->PMCTTIM_CNTx[Timer_Idx];
}

/**
  * @brief  Set the pmc timer count down value.
  * @param  PMC_TIMER: the pointer of PMC TIMER DEV.
  * @param  Timer_Idx: timer index in PMC TIMER DEV.
  *   This parameter can be: 0 ~ 3.
  * @param  NewVaule: new counter.
  * @retval current counter
  */
__STATIC_INLINE void PMCTimerCnt_Set(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx, u32 NewVaule)
{
	PMC_TIMER->PMCTTIM_SETx[Timer_Idx] = PMCTTIM_SETx(NewVaule);
}

/**
  * @brief  reset the pmc timer count down value.
  * @param  PMC_TIMER: the pointer of PMC TIMER DEV.
  * @param  Timer_Idx: timer index in PMC TIMER DEV.
  *   This parameter can be: 0 ~ 3.
  * @retval None
  */
__STATIC_INLINE void PMCTimerCnt_Reset(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx)
{
	PMC_TIMER->PMCTTIM_EN |= PMCTRESET_BIT_TIMERx(Timer_Idx);
}

/**
  * @brief  ENABLE/DISABLE  the pmc timer's interrupt bits..
  * @param  PMC_TIMER: the pointer of PMC TIMER DEV.
  * @param  Timer_Idx: timer index in PMC TIMER DEV.
  *   This parameter can be: 0 ~ 3.
  * @param  NewState: NABLE or DISABLE.
  * @retval None
  */
__STATIC_INLINE void PMCTimer_INTConfig(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx, u32 NewState)
{
	if (NewState != DISABLE) {
		/* Enable the Interrupt sources */
		PMC_TIMER->PMCTTIM_DIER |= BIT(Timer_Idx);
	} else {
		/* Disable the Interrupt sources */
		PMC_TIMER->PMCTTIM_DIER &= ~BIT(Timer_Idx);
	}
}

/**
  * @brief  Clear  the pmc timer's interrupt bits..
  * @param  PMC_TIMER: the pointer of PMC TIMER DEV.
  * @param  Timer_Idx: timer index in PMC TIMER DEV.
  *   This parameter can be: 0 ~ 3.
  * @retval None
  */
__STATIC_INLINE void PMCTimer_INTClear(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx)
{
	/* Enable the Interrupt sources */
	PMC_TIMER->PMCTTIM_ISR |= BIT(Timer_Idx);
}
/** @} */

/* MANUAL_GEN_END */

/** @} */

/** @} */

#endif
