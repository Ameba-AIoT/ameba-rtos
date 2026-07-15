/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup BASICTIMER BASICTIMER
* @brief BASICTIMER driver modules
* @{
*/

int TIMx_irq[TimerNum] = {
	TIMER0_IRQ,
	TIMER1_IRQ,
	TIMER2_IRQ,
	TIMER3_IRQ,
	TIMER4_IRQ,
	TIMER5_IRQ,
	TIMER6_IRQ,
	TIMER7_IRQ,
	TIMER8_IRQ,
	TIMER9_IRQ,
	TIMER10_IRQ,
	TIMER11_IRQ,
};

RTIM_TypeDef *TIMx[TimerNum] = {
	TIM0,
	TIM1,
	TIM2,
	TIM3,
	TIM4,
	TIM5,
	TIM6,
	TIM7,
	TIM8,
	TIM9,
	TIM10,
	TIM11,
};

u32 APBPeriph_TIMx[TimerNum] = {
	APBPeriph_LTIM0,
	APBPeriph_LTIM1,
	APBPeriph_LTIM2,
	APBPeriph_LTIM3,
	APBPeriph_LTIM4,
	APBPeriph_LTIM5,
	APBPeriph_LTIM6,
	APBPeriph_LTIM7,
	APBPeriph_PWM0,
	APBPeriph_PWM1,
	APBPeriph_HTIM0,
	APBPeriph_HTIM1,
};

u32 APBPeriph_TIMx_CLOCK[TimerNum] = {
	APBPeriph_LTIM0_CLOCK,
	APBPeriph_LTIM1_CLOCK,
	APBPeriph_LTIM2_CLOCK,
	APBPeriph_LTIM3_CLOCK,
	APBPeriph_LTIM4_CLOCK,
	APBPeriph_LTIM5_CLOCK,
	APBPeriph_LTIM6_CLOCK,
	APBPeriph_LTIM7_CLOCK,
	APBPeriph_PWM0_CLOCK,
	APBPeriph_PWM1_CLOCK,
	APBPeriph_HTIM0_CLOCK,
	APBPeriph_HTIM1_CLOCK,
};

RTIM_TypeDef *TIMx_S[TimerNum] = {
	TIM0_S,
	TIM1_S,
	TIM2_S,
	TIM3_S,
	TIM4_S,
	TIM5_S,
	TIM6_S,
	TIM7_S,
	TIM8_S,
	TIM9_S,
	TIM10_S,
	TIM11_S,
};

u32 TIM_IT_CCx[PWM_CHAN_MAX] = {
	TIM_IT_CC0,
	TIM_IT_CC1,
	TIM_IT_CC2,
	TIM_IT_CC3,
	TIM_IT_CC4,
	TIM_IT_CC5,
	TIM_IT_CC6,
	TIM_IT_CC7,
};


/* Exported functions --------------------------------------------------------*/
/** @defgroup BASICTIMER_Exported_Functions BASICTIMER Exported Functions
  * @{
  */

/** @defgroup TimeBase_Management_Functions TimeBase Management Functions
* @{
*/

/**
  * @brief  Enable or Disable the TIMx Update event(UEV).
  * @param  TIMx The TIM peripheral, where x can be 0 to (TimerNum - 1). See @ref TIM_Peripheral_Count.
  * @param  NewState New state of the TIMx UDIS bit
  *          This parameter can be:ENABLE or DISABLE
  * @note
  *		- If NewState is ENABLE, Update Disable Bit is set, UEV disable and shadow registers keep their value.
  *		- If NewState is DISABLE, Update Disable Bit is clear, UEV enable and buffered registers are loaded with
  *		their preload values when UEV happen.
  */
void RTIM_UpdateDisableConfig(RTIM_TypeDef *TIMx, u32 NewState)
{
	if (NewState != DISABLE) {
		/* Set the Update Disable Bit */
		TIMx->CR |= TIM_BIT_UDIS;
	} else {
		/* Reset the Update Disable Bit */
		TIMx->CR &= ~TIM_BIT_UDIS;
	}
}

/**
  * @brief  Enable or disable TIMx peripheral Preload register on ARR.
  * @param  TIMx The TIM peripheral, where x can be 0 to (TimerNum - 1). See @ref TIM_Peripheral_Count.
  * @param  NewState New state of the TIMx peripheral Preload register
  *          This parameter can be: ENABLE or DISABLE.
  * @note
  *		- DISABLE: TIMx_ARR register is not buffered, and shadow register will update immediately
  *		- ENABLE: TIMx_ARR register is buffered, and shadow register will update after overflow
  */
void RTIM_ARRPreloadConfig(RTIM_TypeDef *TIMx, u32 NewState)
{
	if (NewState != DISABLE) {
		/* Set the ARR Preload Bit */
		TIMx->CR |= TIM_BIT_ARPE;
	} else {
		/* Reset the ARR Preload Bit */
		TIMx->CR &= ~TIM_BIT_ARPE;
	}
}

/**
  * @brief  Configure the TIMx Update Request Interrupt source.
  * @param  TIMx The TIM peripheral, where x can be 0 to (TimerNum - 1). See @ref TIM_Peripheral_Count.
  * @param  TIM_UpdateSource Specifies the Update source.
  *          This parameter can be one of the following values:
  *            @arg TIM_UpdateSource_Global: Source of update is the counter
  *                 overflow or the setting of UG bit.
  *            @arg TIM_UpdateSource_Overflow: Source of update is counter overflow.
  */
void RTIM_UpdateRequestConfig(RTIM_TypeDef *TIMx, u32 TIM_UpdateSource)
{
	/* Check the parameters */
	assert_param(IS_TIM_UPDATE_SOURCE(TIM_UpdateSource));

	if (TIM_UpdateSource != TIM_UpdateSource_Global) {
		/* Set the URS Bit */
		TIMx->CR |= TIM_BIT_URS;
	} else {
		/* Reset the URS Bit */
		TIMx->CR &= (u32)~TIM_BIT_URS;
	}
}

/**
  * @brief  Configure the TIMx Prescaler.
  * @param  TIMx The TIM peripheral with CCM capability. See IS_TIM_CCM_TIM macro.
  * @param  Prescaler Specifies the Prescaler Register value, which can be a number in 0~0xFFFF range.
  * @param  TIM_PSCReloadMode Specifies the TIM Prescaler Reload mode
  *          This parameter can be one of the following values:
  *            @arg TIM_PSCReloadMode_Update: The Prescaler is loaded at the update event.
  *            @arg TIM_PSCReloadMode_Immediate: The Prescaler is loaded immediately.
  */
void RTIM_PrescalerConfig(RTIM_TypeDef *TIMx, u32 Prescaler, u32 TIM_PSCReloadMode)
{
	/* Check the parameters */
	assert_param(IS_TIM_CCM_TIM(TIMx));
	assert_param(IS_TIM_PSC(Prescaler));
	assert_param(IS_TIM_PRESCALER_RELOAD(TIM_PSCReloadMode));

	/* Set the Prescaler value */
	TIMx->PSC = Prescaler;
	/* Set or reset the UG Bit */
	TIMx->EGR = TIM_PSCReloadMode;
}

/**
  * @brief  Configure the TIMx event to be generated by software.
  * @param  TIMx The TIM peripheral, where x can be 0 to (TimerNum - 1). See @ref TIM_Peripheral_Count.
  * @param  TIM_EventSource Specifies the event source.
  *          This parameter can be one or more of the following values @ref TIM_Event_Source
  * @internal
  * @note
  *		- TIM0-14 have Timer update Event source TIM_EventSource_Update
  *		- TIM9 has 1 Timer Capture Compare Event source TIM_EventSource_CC0
  *		- TIM8 has 8 Timer Capture Compare Event source TIM_EventSource_CC0-7.
  * @endinternal
  */
void RTIM_GenerateEvent(RTIM_TypeDef *TIMx, u32 TIM_EventSource)
{
	/* Check the parameters */
	assert_param((IS_HP_TIM_EVENT_SOURCE(TIM_EventSource) || IS_LP_TIM_EVENT_SOURCE(TIM_EventSource)));

	/* Set the event sources */
	TIMx->EGR = TIM_EventSource;
}

/**
  * @brief  Set the TIMx Autoreload Register(TIMx_ARR) value to change period
  * @param  TIMx The TIM peripheral, where x can be 0 to (TimerNum - 1). See @ref TIM_Peripheral_Count.
  * @param  Autoreload Specifies the Autoreload register new value.
  *         The valid range depends on the TIM type:
  *         - Basic timers (non-CCM): 0 ~ 0xFFFFFFFF
  *         - CCM timers (PWM/Capture): 0 ~ 0xFFFF
  */
void RTIM_ChangePeriodImmediate(RTIM_TypeDef *TIMx, u32 Autoreload)
{
	u32 syncphase;

	/* Check the parameters */
//	assert_param(IS_TIM_ALL_TIM(TIMx));

	/* Reset the ARR Preload Bit */
	/* period will update immediatly */
	TIMx->CR &= ~TIM_BIT_ARPE;

	/* Set the Autoreload Register value */
	if (IS_TIM_PWM_TIM(TIMx)) {
		/* Ensure phasesync value is less than arr value*/
		for (u8 i = 0; i < PWM_CHAN_MAX; i++) {
			syncphase = TIMx->PSYNCx[i] & TIM_MASK_SYNCPHASEx;
			assert_param(syncphase <= Autoreload);
		}
		TIMx->ARR = TIM_CNT(Autoreload);
	} else if (IS_TIM_INPULSE_TIM(TIMx)) {
		TIMx->ARR = TIM_CNT(Autoreload);
	} else {
		TIMx->ARR = TIMBasic_CNT(Autoreload);
	}

	/* Generate an update event */
	/* 1) reload the Prescaler immediatly */
	/* 2) reload the ARR immediatly */
	/* 3) hadrware will clear this bit after reload  */
	/* 4) UEV will reset counter, and counter will start from 0 */
	/* 5) gen a interrupt if use TIM_UpdateSource_Global */
	TIMx->EGR = TIM_PSCReloadMode_Immediate;

	/* poll EGR UG done */
	while (1) {
		if (TIMx->SR & TIM_BIT_UG_DONE) {
			break;
		}
	}
}

/**
  * @brief  Set the TIMx Autoreload Register(TIMx_ARR) value to change period with protection
  * @param  TIMx The TIM peripheral, where x can be 0 to (TimerNum - 1). See @ref TIM_Peripheral_Count.
  * @param  Autoreload Specifies the Autoreload register new value.
  *         The valid range depends on the TIM type:
  *         - Basic timers (non-CCM): 0 ~ 0xFFFFFFFF
  *         - CCM timers (PWM/Capture): 0 ~ 0xFFFF
  */
void RTIM_ChangePeriod(RTIM_TypeDef *TIMx, u32 Autoreload)
{
	u32 syncphase;

	/* Check the parameters */
//	assert_param(IS_TIM_ALL_TIM(TIMx));

	/* Set the Autoreload Register value */
	if (IS_TIM_PWM_TIM(TIMx)) {
		/* Ensure phasesync value is less than arr value*/
		for (u8 i = 0; i < PWM_CHAN_MAX; i++) {
			syncphase = TIMx->PSYNCx[i] & TIM_MASK_SYNCPHASEx;
			assert_param(syncphase <= Autoreload);
		}

		TIMx->ARR = TIM_CNT(Autoreload);
	} else if (IS_TIM_INPULSE_TIM(TIMx)) {
		TIMx->ARR = TIM_CNT(Autoreload);
	} else {
		TIMx->ARR = TIMBasic_CNT(Autoreload);
	}
}

/**
  * @brief  Reset timer, counter will start from 0
  * @param  TIMx The TIM peripheral, where x can be 0 to (TimerNum - 1). See @ref TIM_Peripheral_Count.
  */
void RTIM_Reset(RTIM_TypeDef *TIMx)
{
	/* Check the parameters */
//	assert_param(IS_TIM_ALL_TIM(TIMx));

	/* Generate an update event */
	/* 1) reload the Prescaler immediatly */
	/* 2) reload the ARR immediatly */
	/* 3) hadrware will clear this bit after reload  */
	/* 4) UEV will reset counter, and counter will start from 0 */
	/* 5) gen a interrupt if use TIM_UpdateSource_Global */
	TIMx->EGR = TIM_PSCReloadMode_Immediate;

	/* poll EGR UG done */
	while (1) {
		if (TIMx->SR & TIM_BIT_UG_DONE) {
			break;
		}
	}
}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup PWMTIMER PWMTIMER
* @brief PWMTIMER driver modules
* @{
*/

/** @defgroup PWMTIMER_Exported_Functions PWMTIMER Exported Functions
  * @{
  */

/** @defgroup Capture_Compare_Management_Functions Capture Compare Management Functions
* @{
*/

/**
  * @brief  Fill each TIM_CCInitStruct member with its default value.
  * @param  TIM_CCInitStruct Pointer to a @ref TIM_CCInitTypeDef structure which will
  *         be initialized.
  */
void RTIM_CCStructInit(TIM_CCInitTypeDef *TIM_CCInitStruct)
{
	/* Set the default configuration */
	TIM_CCInitStruct->TIM_CCMode = TIM_CCMode_PWM;
	TIM_CCInitStruct->TIM_CCPolarity = TIM_CCPolarity_High;
	TIM_CCInitStruct->TIM_OCProtection = TIM_OCPreload_Enable;
	TIM_CCInitStruct->TIM_OCPulse = 0xFFF;
}

/**
  * @brief  Initialize the TIMx Channel  according to the specified parameters in
  *         the TIM_CCInitStruct.
  * @param  TIMx The TIM peripheral with CCM capability. See IS_TIM_CCM_TIM macro.
  * @param  TIM_CCInitStruct Pointer to a @ref TIM_CCInitTypeDef structure that contains
  *         the configuration information for the specified TIM peripheral.
  * @param  TIM_Channel The channel need to be initialized,
  *		which can be one of the following parameters @ref TIM_Channel
  * @internal
  * @note
  *		- TIM9 only has 1 channel: TIM_Channel_0.
  *		- TIM8 has 8 channels: TIM_Channel_0-7.
  * @endinternal
  */
void RTIM_CCxInit(RTIM_TypeDef *TIMx, TIM_CCInitTypeDef *TIM_CCInitStruct, u16 TIM_Channel)
{
	/* Check the parameters */
	assert_param(IS_TIM_CCM_TIM(TIMx));
	assert_param(IS_TIM_CC_MODE(TIM_CCInitStruct->TIM_CCMode));
	assert_param(IS_TIM_CC_POLARITY(TIM_CCInitStruct->TIM_CCPolarity));
	assert_param(IS_TIM_OCPRELOAD_STATE(TIM_CCInitStruct->TIM_OCProtection));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));

	u32 Status;

	/* Reset the CCMR Bit */
	TIMx->CCRx[TIM_Channel] = 0;

	/* Write to TIMx CCMR */
	if (IS_TIM_PWM_TIM(TIMx)) {
		TIMx->CCRx[TIM_Channel] = (TIM_CCInitStruct->TIM_CCPolarity |
								   TIM_CCInitStruct->TIM_OCProtection |
								   TIM_CCInitStruct->TIM_CCMode |
								   TIM_CCInitStruct->TIM_OCPulse);
	} else if (IS_TIM_INPULSE_TIM(TIMx)) {
		TIMx->CCRx[TIM_Channel] = (TIM_CCInitStruct->TIM_CCPolarity |
								   TIM_CCInitStruct->TIM_ICPulseMode);
	}

	/* Generate an update event */
	/* 1) reload the CCRx immediatly */
	/* 2) hadrware will clear this bit after reload, about 71.936 us (2*32k cycles) */
	/* 3) UEV will reset counter, and counter will start from 0 */
	/* 4) gen a interrupt if use TIM_UpdateSource_Global */
	TIMx->EGR = TIM_PSCReloadMode_Immediate;

	/* poll EGR UG done */
	while (1) {
		if (TIMx->SR & TIM_BIT_UG_DONE) {
			break;
		}
	}

	/* Clear all flags: PWM: 0x3F007F, Capture: 0x3, Basic: 0x1 */
	Status = TIMx->SR;
	TIMx->SR = Status;
}

/**
  * @brief  Initialize the TIMx Channel  CCmode.
  * @param  TIMx The TIM peripheral with PWM output capability. See IS_TIM_PWM_TIM macro.
  * @param  TIM_Channel The channel need to be set mode,
  *		which can be one of the following parameters @ref TIM_Channel.
  * @param  TIM_CCMode CCx working mode which can be one of the following parameters:
  *		@arg TIM_CCMode_PWM
  *		@arg TIM_CCMode_Inputcapture
  * @internal
  * @note
  *		- TIM8 has 8 channels:  TIM_Channel_0-7.
  * @endinternal
  */
void RTIM_CCRxMode(RTIM_TypeDef *TIMx, u16 TIM_Channel, u32 TIM_CCMode)
{
	u32 tmpccmr = TIMx->CCRx[TIM_Channel];

	/* Check the parameters */
	assert_param(IS_TIM_PWM_TIM(TIMx));
	assert_param(IS_TIM_CC_MODE(TIM_CCMode));

	tmpccmr &= ~TIM_BIT_CCxM;
	tmpccmr |= TIM_CCMode;

	/* Write to TIMx CCMR */
	TIMx->CCRx[TIM_Channel] = tmpccmr;
}

/**
  * @brief  Set the TIMx Capture Compare X register value
  * @param  TIMx The TIM peripheral with PWM output capability. See IS_TIM_PWM_TIM macro.
  * @param  Compare The value specifies pulsewidth, which is in the 0x00~0xFFFF range.
  *					Duty cycle = Compare / (ARR+1).
  * @param  TIM_Channel The channel to be set,
  *		which can be one of the following parameters @ref TIM_Channel.
  * @note
  *		- CCRx=0 will give 0% cycle pwm pulse.
  *		- CCRx>=TIM_Period there will be 100% pwm pulse.
  * @internal
  *		- TIM8 has 8 channels:  TIM_Channel_0-7.
  * @endinternal
  */
void RTIM_CCRxSet(RTIM_TypeDef *TIMx, u32 Compare, u16 TIM_Channel)
{
	u32 PulseWidth = 0;
	u32 tmpccmr = 0;

	/* Check the parameters */
	assert_param(IS_TIM_PWM_TIM(TIMx));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));
	assert_param(IS_TIM_CC_PULSEWIDTH(Compare));

	/* CCRx=0 will give 0% cycle pwm pulse */
	PulseWidth = Compare;

	tmpccmr = TIMx->CCRx[TIM_Channel];

	/* reset Compare val */
	tmpccmr &= ~TIM_CCMode_CCR;

	/* Set the Capture CompareX Register value */
	tmpccmr |= (u32)PulseWidth;

	/* set CCMR */
	TIMx->CCRx[TIM_Channel] = tmpccmr;
}

/**
  * @brief  Enable or disable the TIMx peripheral Preload register on CCRx.
  * @param  TIMx The TIM peripheral with PWM output capability. See IS_TIM_PWM_TIM macro.
  * @param  TIM_OCProtection New state of the TIMx peripheral Preload register
  *          This parameter can be one of the following values:
  *            @arg TIM_OCPreload_Enable: value is loaded in the active register at each update event.
  *            @arg TIM_OCPreload_Disable: new value is taken in account immediately
  * @param  TIM_Channel The channel need to be set,
  *		which can be one of the following parameters @ref TIM_Channel
  * @internal
  * @note
  *		- TIM8 has 8 channels:  TIM_Channel_0-7.
  * @endinternal
  */
void RTIM_OCxPreloadConfig(RTIM_TypeDef *TIMx, u32 TIM_OCProtection, u16 TIM_Channel)
{
	u32 tmpccmr = 0;

	/* Check the parameters */
	assert_param(IS_TIM_PWM_TIM(TIMx));
	assert_param(IS_TIM_OCPRELOAD_STATE(TIM_OCProtection));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));

	tmpccmr = TIMx->CCRx[TIM_Channel];

	/* Reset the OC1PE Bit */
	tmpccmr &= ~(TIM_BIT_OCxPE);

	/* Enable or Disable the Output Compare Preload feature */
	tmpccmr |= TIM_OCProtection;

	/* Write to TIMx CCMR1 register */
	TIMx->CCRx[TIM_Channel] = tmpccmr;
}

/**
  * @brief  Configure the TIMx channel x polarity.
  * @param  TIMx The TIM peripheral with CCM capability. See IS_TIM_CCM_TIM macro.
  * @param  TIM_OCPolarity Specifies the OCx Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM_CCPolarity_High: Output Compare active high
  *            @arg TIM_CCPolarity_Low: Output Compare active low
  * @param  TIM_Channel Specifies the TIM Channel
  *          This parameter can be one of the following values @ref TIM_Channel
  * @internal
  * @note
  * 	-TIM9 has only 1 channel: TIM_Channel_0
  *		-TIM8 has 8 channels: CCR0-7
  * @endinternal
  */
void RTIM_CCxPolarityConfig(RTIM_TypeDef *TIMx, u32 TIM_OCPolarity, u16 TIM_Channel)
{
	u32 tmpccmr = 0;

	/* Check the parameters */
	assert_param(IS_TIM_CCM_TIM(TIMx));
	assert_param(IS_TIM_CC_POLARITY(TIM_OCPolarity));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));

	tmpccmr = TIMx->CCRx[TIM_Channel];

	/* Set or Reset the CCxP Bit */
	tmpccmr &= ~(TIM_BIT_CCxP);
	tmpccmr |= TIM_OCPolarity;

	/* Write to TIMx CCER register */
	TIMx->CCRx[TIM_Channel] = tmpccmr;
}

/**
  * @brief  Enable or disable the TIM Capture Compare Channel x.
  * @param  TIMx The TIM peripheral with CCM capability. See IS_TIM_CCM_TIM macro.
  * @param  TIM_Channel Specifies the TIM Channel
  *          This parameter can be one of the following values @ref TIM_Channel
  * @param  TIM_CCx Specifies the TIM Channel CCxE bit new state.
  *          This parameter can be one of the following values:
  *		@arg TIM_CCx_Enable
  *		@arg TIM_CCx_Disable
  * @internal
  * @note
  *		- TIM9 has only 1 channel:  TIM_Channel_0
  *		- TIM8 has 8 channels:  TIM_Channel_0-7.
  * @endinternal
  */
void RTIM_CCxCmd(RTIM_TypeDef *TIMx, u16 TIM_Channel, u32 TIM_CCx)
{
	u32 tmpccmr = TIMx->CCRx[TIM_Channel];

	/* Check the parameters */
	assert_param(IS_TIM_CCM_TIM(TIMx));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));
	assert_param(IS_TIM_CCX(TIM_CCx));

	tmpccmr &= ~TIM_BIT_CCxE;
	tmpccmr |= TIM_CCx;

	/* Set or reset the CCxE Bit */
	TIMx->CCRx[TIM_Channel] =  tmpccmr;
}

/**
  * @brief  Set the TIMx's One Pulse Mode (output one pulse PWM mode).
  * @param  TIMx The TIM peripheral with PWM output capability. See IS_TIM_PWM_TIM macro.
  * @param  TIM_OPMode Specifies the OPM Mode to be used.
  *          This parameter can be one of the following values:
  *            @arg TIM_OPMode_Single
  *            @arg TIM_OPMode_Repetitive
  * @param  TriggerPolarity Specifies the OPM Mode Trigger Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM_OPMode_ETP_positive
  *            @arg TIM_OPMode_ETP_negative
  *            @arg TIM_OPMode_ETP_bothedge
  * @note  You must select  TIM_OPMode_Single if you want to set One Pulse Mode,
  *		which makes the counter stop automatically at the next UEV.
  */
void RTIM_SetOnePulseOutputMode(RTIM_TypeDef *TIMx, u32 TIM_OPMode, u32 TriggerPolarity)
{
	/* Check the parameters */
	assert_param(IS_TIM_ONE_PULSE_TIM(TIMx));
	assert_param(IS_TIM_OPM_MODE(TIM_OPMode));
	assert_param(IS_TIM_OPM_ETP_MODE(TriggerPolarity));

	/* Reset the OPM & ETP Bit */
	TIMx->CR &= (u32)~(TIM_BIT_OPM | TIM_MASK_ETP);

	/* Configure the OPM Mode */
	TIMx->CR |= TIM_OPMode | TriggerPolarity;
}

/**
  * @brief  Set the TIMx's default level in one pulse mode.
  * @param  TIMx The TIM peripheral with PWM output capability. See IS_TIM_PWM_TIM macro.
  * @param  TIM_Channel The channel to be set,
  *		which can be one of the following parameters @ref TIM_Channel.
  * @param  DefaultLevel Specifies the OPM Mode Default Level.
  *          This parameter can be one of the following values:
  *            @arg TIMPWM_DefaultLevel_High
  *            @arg TIMPWM_DefaultLevel_Low
  * @note
  *		- Takes effect only in PWM output mode's One-Pulse-Mode.
  *		- You must select TIM_OPMode_Single if you want to set One Pulse Mode,
  *		which makes the counter stop automatically at the next UEV.
  */
void RTIM_SetOnePulseDefaultLevel(RTIM_TypeDef *TIMx, u16 TIM_Channel, u32 DefaultLevel)
{
	/* Check the parameters */
	assert_param(IS_TIM_ONE_PULSE_TIM(TIMx));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));
	assert_param(IS_TIMPWM_DefaultLevel(DefaultLevel));

	if (DefaultLevel == TIMPWM_DefaultLevel_High) {
		TIMx->CCRx[TIM_Channel] |= TIM_BIT_OPM_DLx;
	} else {
		TIMx->CCRx[TIM_Channel] &= ~(TIM_BIT_OPM_DLx);
	}
}
/**
  * @}
  */

/** @defgroup Phase_Sync_Functions Phase Sync Functions
* @{
*/

/**
  * @brief  Set the TIMx Phase Sync X value
  * @param  TIMx The TIM peripheral with PWM output capability. See IS_TIM_PWM_TIM macro.
  * @param  TIM_Channel The channel to be set,
  *		which can be one of the following parameters @ref TIM_Channel.
  * @param  TIM_SyncPhase The phase sync value compared to CNT in TIM_CNT
  * @note
  *		- TIM_SyncPhase should always smaller than ARR value
  */
void RTIM_PSyncxSet(RTIM_TypeDef *TIMx, u16 TIM_Channel, u32 TIM_SyncPhase)
{
	u32 tmppsync = 0;
	u32 arr = 0;

	/* Check the parameters */
	assert_param(IS_TIM_PWM_TIM(TIMx));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));

	tmppsync = TIMx->PSYNCx[TIM_Channel];
	arr = TIMx->ARR;

	/* Ensure phasesync value is less than arr value*/
	assert_param(TIM_SyncPhase <= arr);

	tmppsync &= ~(TIM_MASK_SYNCPHASEx);
	tmppsync |= TIM_SYNCPHASEx(TIM_SyncPhase);

	TIMx->PSYNCx[TIM_Channel] =  tmppsync;
}

/**
  * @brief  Get the TIMx Phase Sync X value
  * @param  TIMx The TIM peripheral with PWM output capability. See IS_TIM_PWM_TIM macro.
  * @param  TIM_Channel The channel to be read,
  *		which can be one of the following parameters @ref TIM_Channel.
  * @return  TIMx Phase Sync X value.
  */
u32 RTIM_PSyncxGet(RTIM_TypeDef *TIMx, u16 TIM_Channel)
{
	/* Check the parameters */
	assert_param(IS_TIM_PWM_TIM(TIMx));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));

	return TIM_SYNCPHASEx(TIMx->PSYNCx[TIM_Channel]);
}

/**
  * @brief  Set the TIMx Phase Sync X direction
  * @param  TIMx The TIM peripheral with PWM output capability. See IS_TIM_PWM_TIM macro.
  * @param  TIM_Channel The channel to be set,
  *		which can be one of the following parameters @ref TIM_Channel.
  * @param  TIM_SyncDir TIMPWM_PSync_Delay/TIMPWM_PSync_Ahead
  */
void RTIM_PSyncxDir(RTIM_TypeDef *TIMx, u16 TIM_Channel, u8 TIM_SyncDir)
{
	u32 tmppsync = 0;

	/* Check the parameters */
	assert_param(IS_TIM_PWM_TIM(TIMx));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));
	assert_param(IS_TIMPWM_PSync_Dir(TIM_SyncDir));

	tmppsync = TIMx->PSYNCx[TIM_Channel];

	if (TIM_SyncDir == TIMPWM_PSync_Delay) {
		tmppsync &= ~(TIM_BIT_SYNCDIRx);
	} else {
		tmppsync |= TIM_BIT_SYNCDIRx;
	}

	TIMx->PSYNCx[TIM_Channel] =  tmppsync;
}

/**
  * @brief  Enable or disable the preload function of TIMx phase sync register.
  * @param  TIMx The TIM peripheral with PWM output capability. See IS_TIM_PWM_TIM macro.
  * @param  TIM_Channel The channel need to be set,
  *		which can be one of the following parameters @ref TIM_Channel
  * @param  TIM_PSyncProtection TIMPWM_PSyncPreload_Enable/TIMPWM_PSyncPreload_Disable
  *            @arg TIMPWM_PSyncPreload_Enable: value is loaded in the active register at each update event.
  *            @arg TIMPWM_PSyncPreload_Disable: new value is taken in account immediately
  */
void RTIM_PSyncxPreloadConfig(RTIM_TypeDef *TIMx, u16 TIM_Channel, u32 TIM_PSyncProtection)
{
	u32 tmppsync = 0;

	/* Check the parameters */
	assert_param(IS_TIM_PWM_TIM(TIMx));
	assert_param(IS_TIMPWM_PPRELOAD_STATE(TIM_PSyncProtection));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));

	tmppsync = TIMx->PSYNCx[TIM_Channel];

	/* Reset the OC1PE Bit */
	if (TIM_PSyncProtection == TIMPWM_PSyncPreload_Enable) {
		tmppsync |= TIM_BIT_SYNCPEx;
	} else {
		tmppsync &= ~(TIM_BIT_SYNCPEx);
	}

	TIMx->PSYNCx[TIM_Channel] = tmppsync;
}

/**
  * @brief  Enable or disable the TIMx Phase Sync function.
  * @param  TIMx The TIM peripheral with PWM output capability. See IS_TIM_PWM_TIM macro.
  * @param  TIM_Channel Specifies the TIM Channel
  *          This parameter can be one of the following values @ref TIM_Channel
  * @param  NewState ENABLE/DISABLE.
  */
void RTIM_PSyncxCmd(RTIM_TypeDef *TIMx, u16 TIM_Channel, u8 NewState)
{
	u32 tmppsync = 0;

	/* Check the parameters */
	assert_param(IS_TIM_PWM_TIM(TIMx));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));

	tmppsync = TIMx->PSYNCx[TIM_Channel];

	if (NewState != DISABLE) {
		tmppsync |= TIM_BIT_SYNCENx;
	} else {
		tmppsync &= ~(TIM_BIT_SYNCENx);
	}

	TIMx->PSYNCx[TIM_Channel] =  tmppsync;
}

/**
  * @brief  Get the TIMx Channel Counter value.
  * @param  TIMx The TIM peripheral with PWM output capability. See IS_TIM_PWM_TIM macro.
  * @param  TIM_Channel Specifies the TIM Channel
  *          This parameter can be one of the following values @ref TIM_Channel
  * @return Counter Register value.
  */
u32 RTIM_GetChannelCountx(RTIM_TypeDef *TIMx, u16 TIM_Channel)
{
	u32 TimerCountOld;
	u32 TimerCountNew;
	u32 TimerRDCnt;

	/* Check the parameters */
	assert_param(IS_TIM_PWM_TIM(TIMx));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));

	TimerRDCnt = 0;
	TimerCountOld = TIMx->PHASECNTx[TIM_Channel];
	while (1) {
		TimerCountNew = TIMx->PHASECNTx[TIM_Channel];

		if (TimerCountOld == TimerCountNew) {
			return (u32)TimerCountOld;
		} else {
			TimerRDCnt++;
			TimerCountOld = TimerCountNew;

			if (TimerRDCnt >= 2) {
				return (u32)TimerCountOld;
			}
		}
	}
}

/**
  * @brief  Clear the TIMx's all interrupt pending bits.
  * @param  TIMx The TIM peripheral, where x can be 0 to (TimerNum - 1). See @ref TIM_Peripheral_Count.
  */
void RTIM_INTClear(RTIM_TypeDef *TIMx)
{
	u32 CounterIndex = 0;
	u32 Status = TIMx->SR;

	/* Clear the all IT pending Bits */
	TIMx->SR = Status;

	/* make sure write ok, because bus delay */
	while (1) {
		CounterIndex++;
		if (CounterIndex >= TIMER_POLLING_TIMES) {
			break;
		}

		/* this action need sync to timer clk domain, costing about 3cycles,
		for 32K timer, this action need about 100us;
		bus clk is optional from 4M and 40M */
		if (((TIMx->SR) & 0xFFFFFF) == 0) {
			break;
		}
	}
}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup CAPTIMER CAPTIMER
* @brief CAPTIMER driver modules
* @{
*/

/** @defgroup CAPTIMER_Exported_Functions CAPTIMER Exported Functions
  * @{
  */

/**
  * @brief  Get the TIMx Capture Compare X register value.
  * @param  TIMx The TIM peripheral with CCM capability. See IS_TIM_CCM_TIM macro.
  * @param  TIM_Channel The channel to be read,
  *		which can be one of the following parameters @ref TIM_Channel.
  * @return  Capture Compare x Register value.
  * @note	If you want to get pulse width of PWM, remember to plus 1 to
  *			the return value of this function.
  * @internal
  *		- TIM9 only has 1 channel: TIM_Channel_0.
  *		- TIM8 has 8 channels: TIM_Channel_0-7.
  * @endinternal
  */
u32 RTIM_CCRxGet(RTIM_TypeDef *TIMx, u16 TIM_Channel)
{
	/* Check the parameters */
	assert_param(IS_TIM_CCM_TIM(TIMx));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));

	/* Get the Capture Compare x Register value */
	return (TIMx->CCRx[TIM_Channel] & TIM_CCMode_CCR);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
