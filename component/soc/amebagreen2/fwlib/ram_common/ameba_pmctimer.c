/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

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
void PMCTimer_Cmd(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx, bool NewState)
{
	if (NewState) {
		PMC_TIMER->PMCT_TIM_EN |= BIT(Timer_Idx);
	} else {
		PMC_TIMER->PMCT_TIM_EN &= ~BIT(Timer_Idx);
	}
}

/**
  * @brief  Check pmctimer current state.
  * @param  PMC_TIMER: the pointer of PMC TIMER DEV.
  * @param  Timer_Idx: timer index in PMC TIMER DEV.
  *   This parameter can be: 0 ~ 3.
  * @retval valid or not
  */
u32 PMCTimer_Valid_Check(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx)
{
	if (PMC_TIMER->PMCT_TIM_EN & PMCT_BIT_TIM_VALIDx(Timer_Idx)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/**
  * @brief  Get the pmc timer current Counter value.
  * @param  PMC_TIMER: the pointer of PMC TIMER DEV.
  * @param  Timer_Idx: timer index in PMC TIMER DEV.
  *   This parameter can be: 0 ~ 3.
  * @retval current counter
  */
u32 PMCTimerCnt_Get(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx)
{
	return PMC_TIMER->PMCT_TIM_CNTx[Timer_Idx];
}

/**
  * @brief  Set the pmc timer count down value.
  * @param  PMC_TIMER: the pointer of PMC TIMER DEV.
  * @param  Timer_Idx: timer index in PMC TIMER DEV.
  *   This parameter can be: 0 ~ 3.
  * @param  NewVaule: new counter.
  * @retval current counter
  */
void PMCTimerCnt_Set(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx, u32 NewVaule)
{
	PMC_TIMER->PMCT_TIM_SETx[Timer_Idx] = PMCT_TIM_SETx(NewVaule);
}

/**
  * @brief  reset the pmc timer count down value.
  * @param  PMC_TIMER: the pointer of PMC TIMER DEV.
  * @param  Timer_Idx: timer index in PMC TIMER DEV.
  *   This parameter can be: 0 ~ 3.
  * @retval None
  */
void PMCTimerCnt_Reset(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx)
{
	PMC_TIMER->PMCT_TIM_EN |= PMCTRESET_BIT_TIMERx(Timer_Idx);
}

/**
  * @brief  ENABLE/DISABLE  the pmc timer's interrupt bits..
  * @param  PMC_TIMER: the pointer of PMC TIMER DEV.
  * @param  Timer_Idx: timer index in PMC TIMER DEV.
  *   This parameter can be: 0 ~ 3.
  * @param  NewState: NABLE or DISABLE.
  * @retval None
  */
void PMCTimer_INTConfig(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx, u32 NewState)
{
	if (NewState != DISABLE) {
		/* Enable the Interrupt sources */
		PMC_TIMER->PMCT_TIM_DIER |= BIT(Timer_Idx);
	} else {
		/* Disable the Interrupt sources */
		PMC_TIMER->PMCT_TIM_DIER &= ~BIT(Timer_Idx);
	}
}

/**
  * @brief  Clear  the pmc timer's interrupt bits..
  * @param  PMC_TIMER: the pointer of PMC TIMER DEV.
  * @param  Timer_Idx: timer index in PMC TIMER DEV.
  *   This parameter can be: 0 ~ 3.
  * @retval None
  */
void PMCTimer_INTClear(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx)
{
	/* Enable the Interrupt sources */
	PMC_TIMER->PMCT_TIM_ISR |= BIT(Timer_Idx);
}
/**
 * @brief Enable the comparison function of pmctimer
 *
 * @param PMC_TIMER: the pointer of PMC TIMER DEV.
 * @param Timer_Idx: timer index in PMC TIMER DEV.
 * @param NewState: this parameter can be: ENABLE or DISABLE.
 * @retval None
 */
void PMCTimer_CompCmd(PMCTIMER_TpyeDef *PMC_TIMER, u32 Timer_Idx, u32 NewState)
{
	if (NewState) {
		PMC_TIMER->PMCT_TIM_COM_EN |= BIT(Timer_Idx);
	} else {
		PMC_TIMER->PMCT_TIM_COM_EN &= ~BIT(Timer_Idx);
	}
}

u32 PMCTimer_INT_Hdl(UNUSED_WARN_DIS void *Data)
{
	UNUSED(Data);
	PMCTimer_INTClear(PMC_TIMER_DEV, PMC_WAKEUP_TIMER);
	pmu_acquire_wakelock(PMU_OS);
	return TRUE;
}

void PMCTimer_Init(void)
{
	InterruptRegister(PMCTimer_INT_Hdl, PMC_TIMER_IRQ, NULL, PMC_TIMER_INT_PRIO);
	InterruptEn(PMC_TIMER_IRQ, PMC_TIMER_INT_PRIO);
	PMCTimer_INTConfig(PMC_TIMER_DEV, PMC_WAKEUP_TIMER, ENABLE);
}

void PMCTimer_Reset(void)
{
	PMCTIMER_TpyeDef *PMC_TIMER = PMC_TIMER_DEV;

	PMCTimerCnt_Reset(PMC_TIMER, PMC_WAKEUP_TIMER);
}

