/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

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
};

u32 APBPeriph_TIMx[TimerNum] = {
	APBPeriph_LTIM0,
	APBPeriph_LTIM1,
	APBPeriph_LTIM2,
	APBPeriph_LTIM3,
	APBPeriph_PWM0,
	APBPeriph_PWM1,
	APBPeriph_PWM2,
	APBPeriph_PWM3,
	APBPeriph_PWM4,
};

u32 APBPeriph_TIMx_CLOCK[TimerNum] = {
	APBPeriph_LTIM0_CLOCK,
	APBPeriph_LTIM1_CLOCK,
	APBPeriph_LTIM2_CLOCK,
	APBPeriph_LTIM3_CLOCK,
	APBPeriph_PWM0_CLOCK,
	APBPeriph_PWM1_CLOCK,
	APBPeriph_PWM2_CLOCK,
	APBPeriph_PWM3_CLOCK,
	APBPeriph_PWM4_CLOCK,
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
};

u32 TIM_IT_CCx[PWM_CHAN_MAX] = {
	TIM_IT_CC0,
	TIM_IT_CC1,
	TIM_IT_CC2,
	TIM_IT_CC3,
};

/**
  * @brief  Enables or Disables the TIMx Update event(UEV).
  * @param  TIMx: where x can be 0-8 to select the TIM peripheral.
  * @param  NewState: new state of the TIMx UDIS bit
  *          This parameter can be:ENABLE or DISABLE
  * @note
  *		- If NewState is ENABLE, Update Disable Bit is set, UEV disable and shadow registers keep their value.
  *		- If NewState is DISABLE, Update Disable Bit is clear, UEV enable and buffered registers are loaded with
  *		their preload values when UEV happen.
  * @retval None
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
  * @brief  Enables or disables TIMx peripheral Preload register on ARR.
  * @param  TIMx: where x can be 0-8 to select the TIM peripheral.
  * @param  NewState: new state of the TIMx peripheral Preload register
  *          This parameter can be: ENABLE or DISABLE.
  * @note
  *		- DISABLE: TIMx_ARR register is not buffered, and shadow register will update immediately
  *		- ENABLE: TIMx_ARR register is buffered, and shadow register will update after overflow
  * @retval None
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
  * @brief  Configures the TIMx Update Request Interrupt source.
  * @param  TIMx: where x can be 0-8 to select the TIM peripheral.
  * @param  TIM_UpdateSource: specifies the Update source.
  *          This parameter can be one of the following values:
  *            @arg TIM_UpdateSource_Global: Source of update is the counter
  *                 overflow or the setting of UG bit.
  *            @arg TIM_UpdateSource_Overflow: Source of update is counter overflow.
  * @retval None
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
  * @brief  Configures the TIMx Prescaler.
  * @param  TIMx: where x can be 4-8 to select the TIM peripheral.
  * @param  Prescaler: specifies the Prescaler Register value,which can be a number in 0~0xFFFF range for TIM4-8.
  * @param  TIM_PSCReloadMode: specifies the TIM Prescaler Reload mode
  *          This parameter can be one of the following values:
  *            @arg TIM_PSCReloadMode_Update: The Prescaler is loaded at the update event.
  *            @arg TIM_PSCReloadMode_Immediate: The Prescaler is loaded immediatly.
  * @retval None
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
  * @brief  Configures the TIMx event to be generate by software.
  * @param  TIMx: where x can be 0-8 to select the TIM peripheral.
  * @param  TIM_EventSource: specifies the event source.
  *          This parameter can be one or more of the following values @ref TIMx_Event_Generation_definitons
  * @note
  *		- TIM0-8 have Timer update Event source TIM_EventSource_Update
  *		- TIM8 has 1 Timer Capture Compare Event source TIM_EventSource_CC0
  *		- TIM4-7 has 4 Timer Capture Compare Event source TIM_EventSource_CC0-3.
  * @retval None
  */
void RTIM_GenerateEvent(RTIM_TypeDef *TIMx, u32 TIM_EventSource)
{
	/* Check the parameters */
	assert_param((IS_HP_TIM_EVENT_SOURCE(TIM_EventSource) || IS_LP_TIM_EVENT_SOURCE(TIM_EventSource)));

	/* Set the event sources */
	TIMx->EGR = TIM_EventSource;
}

/**
  * @brief  Sets the TIMx Autoreload Register(TIMx_ARR) value to change period
  * @param  TIMx: where x can be 0-8 to select the TIM peripheral.
  * @param  Autoreload: specifies the Autoreload register new value.
  *			To TIM0-3, this value can be a number in 0~0xFFFFFFFF range.
  *			To TIM4-8, this value can be a number in 0~0xFFFF range.
  * @retval None
  */
void RTIM_ChangePeriodImmediate(RTIM_TypeDef *TIMx, u32 Autoreload)
{
	/* Check the parameters */
//	assert_param(IS_TIM_ALL_TIM(TIMx));

	/* Reset the ARR Preload Bit */
	/* period will update immediatly */
	TIMx->CR &= ~TIM_BIT_ARPE;

	/* Set the Autoreload Register value */
	if (IS_TIM_PWM_TIM(TIMx)) {
		/* Ensure phasesync value is less than arr value*/
		for (u8 i = 0; i < PWM_CHAN_MAX; i++) {
			u32 syncphase = TIMx->PSYNCx[i] & TIM_MASK_SYNCPHASEx;
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
  * @brief  Sets the TIMx Autoreload Register(TIMx_ARR) value to change period with protection
  * @param  TIMx: where x can be 0-8 to select the TIM peripheral.
  * @param  Autoreload: specifies the Autoreload register new value.
  *			To TIM0-3, this value can be a number in 0~0xFFFFFFFF range.
  *			To TIM4-8, this value can be a number in 0~0xFFFF range.
  * @retval None
  */
void RTIM_ChangePeriod(RTIM_TypeDef *TIMx, u32 Autoreload)
{
	/* Check the parameters */
//	assert_param(IS_TIM_ALL_TIM(TIMx));

	/* Set the Autoreload Register value */
	if (IS_TIM_PWM_TIM(TIMx)) {
		/* Ensure phasesync value is less than arr value*/
		for (u8 i = 0; i < PWM_CHAN_MAX; i++) {
			u32 syncphase = TIMx->PSYNCx[i] & TIM_MASK_SYNCPHASEx;
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
  * @brief  Sets the TIMx Autoreload Register(TIMx_ARR) value to change period(us).
  * @param  TIMx: where x can be 0-8 to select the TIM peripheral.
  * @param  tim_idx: where x can be 0-8.
  * @param  period_us: Period to be set in microseconds.
  * @retval None
  */
void RTIM_ChangePeriodImmediate_us(RTIM_TypeDef *TIMx, u8 tim_idx, u32 period_us)
{
	u8 clk_source = 0;
	u32 temp;
	if (IS_TIM_BASIC_TIM(TIMx)) {
		switch (tim_idx) {
		case 0:
			clk_source = RCC_PeriphClockSourceGet(LTIM0);
			break;
		case 1:
			clk_source = RCC_PeriphClockSourceGet(LTIM1);
			break;
		case 2:
			clk_source = RCC_PeriphClockSourceGet(LTIM2);
			break;
		case 3:
			clk_source = RCC_PeriphClockSourceGet(LTIM3);
			break;
		}

		if (!clk_source) {
			temp = (u32)((float)period_us / 1000000 * 32768) - 1;
		} else {
			temp = (u32)period_us - 1;
		}
	} else {
		temp = (u32)((float)(period_us / 1000000 * 40000000 / TIMx->PSC));
	}

	RTIM_ChangePeriodImmediate(TIMx, temp);
}

/**
  * @brief  Sets the TIMx Autoreload Register(TIMx_ARR) value to change period(us) with protection.
  * @param  TIMx: where x can be 0-8 to select the TIM peripheral.
  * @param  tim_idx: where x can be 0-8.
  * @param  period_us: Period to be set in microseconds.
  * @retval None
  */
void RTIM_ChangePeriod_us(RTIM_TypeDef *TIMx, u8 tim_idx, u32 period_us)
{
	u8 clk_source = 0;
	u32 temp;
	if (IS_TIM_BASIC_TIM(TIMx)) {
		switch (tim_idx) {
		case 0:
			clk_source = RCC_PeriphClockSourceGet(LTIM0);
			break;
		case 1:
			clk_source = RCC_PeriphClockSourceGet(LTIM1);
			break;
		case 2:
			clk_source = RCC_PeriphClockSourceGet(LTIM2);
			break;
		case 3:
			clk_source = RCC_PeriphClockSourceGet(LTIM3);
			break;
		}

		if (!clk_source) {
			temp = (u32)((float)period_us / 1000000 * 32768) - 1;
		} else {
			temp = (u32)period_us - 1;
		}
	} else {
		temp = (u32)((float)(period_us / 1000000 * 40000000 / TIMx->PSC));
	}

	RTIM_ChangePeriod(TIMx, temp);
}

/**
  * @brief  Reset timer, counter will start from 0
  * @param  TIMx: where x can be 0-8 to select the TIM peripheral.
  * @retval None
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
  * @brief  Fills each TIM_CCInitStruct member with its default value.
  * @param  TIM_CCInitStruct: pointer to a TIM_CCInitTypeDef structure which will
  *         be initialized.
  * @retval None
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
  * @brief  Initializes the TIMx Channel 0-3 according to the specified parameters in
  *         the TIM_CCInitStruct.
  * @param  TIMx: where x can be 4-8, to select the TIM peripheral.
  * @param  TIM_CCInitStruct: pointer to a TIM_CCInitTypeDef structure that contains
  *         the configuration information for the specified TIM peripheral.
  * @param  TIM_Channel: the channel need to be initialized,
  *		which can be one of the following parameters @ref TIM_Channel_definitions
  * @note
  *		- TIM8 only has 1 channel: TIM_Channel_0
  *		- TIM4-7 has 4 channels: TIM_Channel_0-3.
  * @retval None
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
  * @brief  Initializes the TIMx Channel 0-3 CCmode.
  * @param  TIMx: where x can only be 4-7.
  * @param  TIM_Channel: the channel need to be set mode,
  *		which can be one of the following parameters @ref TIM_Channel_definitions.
  * @param  TIM_CCMode: CCx working mode which can be one of the following parameters:
  *		@arg TIM_CCMode_PWM
  *		@arg TIM_CCMode_Inputcapture
  * @note
  *		- TIM4-7 has 4 channels:  TIM_Channel_0-3.
  * @retval None
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
  * @brief  Sets the TIMx Capture Compare X register value
  * @param  TIMx: where x can only be 4-7.
  * @param  Compare: the value specifies pulsewidth, which is in the 0x00~0xFFFF range.
  *					Duty cycle = Compare / (ARR+1).
  * @param  TIM_Channel: the channel to be set,
  *		which can be one of the following parameters @ref TIM_Channel_definitions.
  * @retval None
  * @note
  *		- CCRx=0 will give 0% cycle pwm pulse.
  *		- CCRx>=TIM_Period there will be 100% pwm pulse.
  *		- TIM4-7 has 4 channels:  TIM_Channel_0-3.
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
  * @brief  Gets the TIMx Capture Compare X register value.
  * @param  TIMx: where x can be 4-8, to select the TIM peripheral.
  * @param  TIM_Channel: the channel to be read,
  *		which can be one of the following parameters @ref TIM_Channel_definitions.
  * @retval  Capture Compare x Register value.
  * @note	If you want to get pulse width of PWM, remember to plus 1 to
  *			the retval of this function.
  *		- TIM8 only has 1 channel:  TIM_Channel_0
  *		- TIM4-7 has 4 channels:  TIM_Channel_0-3.
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
  * @brief  Enables or disables the TIMx peripheral Preload register on CCRx.
  * @param  TIMx: where x can only be 4-7.
  * @param  TIM_OCProtection: new state of the TIMx peripheral Preload register
  *          This parameter can be one of the following values:
  *            @arg TIM_OCPreload_Enable: value is loaded in the active register at each update event.
  *            @arg TIM_OCPreload_Disable: new value is taken in account immediately
  * @param  TIM_Channel: the channel need to be set,
  *		which can be one of the following parameters @ref TIM_Channel_definitions
  * @note
  *		- TIM4-7 has 4 channels:  TIM_Channel_0-3.
  * @retval None
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
  * @brief  Configures the TIMx channel x polarity.
  * @param  TIMx: where x can be 4-8, to select the TIM peripheral.
  * @param  TIM_OCPolarity: specifies the OCx Polarity
  *          This parameter can be one of the following values:
  *            @arg TIM_CCPolarity_High: Output Compare active high
  *            @arg TIM_CCPolarity_Low: Output Compare active low
  * @retval None
  * @note
  * 	-TIM8 has only 1 channel: TIM_Channel_0
  *		-TIM4-7 has 4 channels: CCR0-3
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
  * @brief  Enables or disables the TIM Capture Compare Channel x.
  * @param  TIMx: where x can be 4-8, to select the TIM peripheral.
  * @param  TIM_Channel: specifies the TIM Channel
  *          This parameter can be one of the following values @ref TIM_Channel_definitions
  * @param  TIM_CCx: specifies the TIM Channel CCxE bit new state.
  *          This parameter can be one of the following values:
  *		@arg TIM_CCx_Enable
  *		@arg TIM_CCx_Disable
  * @note
  *		- TIM8 has only 1 channel:  TIM_Channel_0
  *		- TIM4-7 has 4 channels:  TIM_Channel_0-3.
  * @retval None
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
  * @param  TIMx: where x can only be 4-7.
  * @param  TIM_OPMode: specifies the OPM Mode to be used.
  *          This parameter can be one of the following values:
  *            @arg TIM_OPMode_Single
  *            @arg TIM_OPMode_Repetitive
  * @param  TrigerPolarity: specifies the OPM Mode Triger Polarity.
  *          This parameter can be one of the following values:
  *            @arg TIM_OPMode_ETP_positive
  *            @arg TIM_OPMode_ETP_negative
  *            @arg TIM_OPMode_ETP_bothedge
  * @note  You must select  TIM_OPMode_Single if you want to set One Pluse Mode,
  *		which makes the counter stop automatically at the next UEV.
  * @retval None
  */
void RTIM_SetOnePulseOutputMode(RTIM_TypeDef *TIMx, u32 TIM_OPMode, u32 TrigerPolarity)
{
	/* Check the parameters */
	assert_param(IS_TIM_ONE_PULSE_TIM(TIMx));
	assert_param(IS_TIM_OPM_MODE(TIM_OPMode));
	assert_param(IS_TIM_OPM_ETP_MODE(TrigerPolarity));

	/* Reset the OPM & ETP Bit */
	TIMx->CR &= (u32)~(TIM_BIT_OPM | TIM_MASK_ETP);

	/* Configure the OPM Mode */
	TIMx->CR |= TIM_OPMode | TrigerPolarity;
}

/**
  * @brief  Set the TIMx's default level in one pulse mode.
  * @note Takes effect only in PWM output mode's One-Pulse-Mode
  * @param  TIMx: where x can only be 4-7.
  * @param  TIM_Channel: the channel to be set,
  *		which can be one of the following parameters @ref TIM_Channel_definitions.
  * @param  DefaultLevel: specifies the OPM Mode Default Level.
  *          This parameter can be one of the following values:
  *            @arg TIMPWM_DefaultLevel_High
  *            @arg TIMPWM_DefaultLevel_Low
  * @note  You must select  TIM_OPMode_Single if you want to set One Pluse Mode,
  *		which makes the counter stop automatically at the next UEV.
  * @retval None
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
  * @brief  Sets the TIMx Phase Sync X value
  * @param  TIMx: where x can only be 4-7.
  * @param  TIM_Channel: the channel to be set,
  *		which can be one of the following parameters @ref TIM_Channel_definitions.
  * @param  TIM_SyncPhase: the phase sync value compared to CNT in TIM_CNT
  * @retval None
  * @note
  *		- TIM_SyncPhase should always smaller than ARR value
  */
void RTIM_PSyncxSet(RTIM_TypeDef *TIMx, u16 TIM_Channel, u32 TIM_SyncPhase)
{
	/* Check the parameters */
	assert_param(IS_TIM_PWM_TIM(TIMx));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));

	u32 tmppsync = 0;
	u32 arr = 0;

	tmppsync = TIMx->PSYNCx[TIM_Channel];
	arr = TIMx->ARR;

	/* Ensure phasesync value is less than arr value*/
	assert_param(TIM_SyncPhase <= arr);

	tmppsync &= ~(TIM_MASK_SYNCPHASEx);
	tmppsync |= TIM_SYNCPHASEx(TIM_SyncPhase);

	TIMx->PSYNCx[TIM_Channel] =  tmppsync;
}

/**
  * @brief  Gets the TIMx Phase Sync X value
  * @param  TIMx: where x can only be 4-7.
  * @param  TIM_Channel: the channel to be read,
  *		which can be one of the following parameters @ref TIM_Channel_definitions.
  * @retval  TIMx Phase Sync X value
  */
u32 RTIM_PSyncxGet(RTIM_TypeDef *TIMx, u16 TIM_Channel)
{
	/* Check the parameters */
	assert_param(IS_TIM_PWM_TIM(TIMx));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));

	return TIM_SYNCPHASEx(TIMx->PSYNCx[TIM_Channel]);
}

/**
  * @brief  Sets the TIMx Phase Sync X direction
  * @param  TIMx: where x can only be 4-7.
  * @param  TIM_Channel: the channel to be set,
  *		which can be one of the following parameters @ref TIM_Channel_definitions.
  * @param  TIM_SyncDir: TIMPWM_PSync_Delay/TIMPWM_PSync_Ahead
  * @retval None
  */
void RTIM_PSyncxDir(RTIM_TypeDef *TIMx, u16 TIM_Channel, u8 TIM_SyncDir)
{
	/* Check the parameters */
	assert_param(IS_TIM_PWM_TIM(TIMx));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));
	assert_param(IS_TIMPWM_PSync_Dir(TIM_SyncDir));

	u32 tmppsync = TIMx->PSYNCx[TIM_Channel];

	if (TIM_SyncDir == TIMPWM_PSync_Delay) {
		tmppsync &= ~(TIM_BIT_SYNCDIRx);
	} else {
		tmppsync |= TIM_BIT_SYNCDIRx;
	}

	TIMx->PSYNCx[TIM_Channel] =  tmppsync;
}

/**
  * @brief  Enables or disables the preload function of TIMx phase sync register.
  * @param  TIMx: where x can only be 4-7.
  * @param  TIM_Channel: the channel need to be set,
  *		which can be one of the following parameters @ref TIM_Channel_definitions
  * @param  TIM_OCProtection: TIMPWM_PSyncPreload_Enable/TIMPWM_PSyncPreload_Disable
  *            @arg TIMPWM_PSyncPreload_Enable: value is loaded in the active register at each update event.
  *            @arg TIMPWM_PSyncPreload_Disable: new value is taken in account immediately
  * @retval None
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
  * @brief  Enables or disables the TIM4-7 Phase Sync function.
  * @param  TIMx: where x can only be 4-7.
  * @param  TIM_Channel: specifies the TIM Channel
  *          This parameter can be one of the following values @ref TIM_Channel_definitions
  * @param  NewState: ENABLE/DISABLE.
  * @retval None
  */
void RTIM_PSyncxCmd(RTIM_TypeDef *TIMx, u16 TIM_Channel, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_TIM_PWM_TIM(TIMx));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));

	u32 tmppsync = TIMx->PSYNCx[TIM_Channel];

	if (NewState != DISABLE) {
		tmppsync |= TIM_BIT_SYNCENx;
	} else {
		tmppsync &= ~(TIM_BIT_SYNCENx);
	}

	TIMx->PSYNCx[TIM_Channel] =  tmppsync;
	UNUSED(NewState);
}

/**
  * @brief  Gets the TIMx Channel Counter value.
  * @param  TIMx: where x can only be 4-7.
  * @param  TIM_Channel: specifies the TIM Channel
  *          This parameter can be one of the following values @ref TIM_Channel_definitions
  * @retval Counter Register value
  */
u32 RTIM_GetChannelCountx(RTIM_TypeDef *TIMx, u16 TIM_Channel)
{
	/* Check the parameters */
	assert_param(IS_TIM_PWM_TIM(TIMx));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));

	return TIMx->PHASECNTx[TIM_Channel];
}

/**
  * @brief  Gets the TIMx Channel inputcapture mode enabled or not.
  * @param  TIMx: where x can only be 4-7.
  * @param  TIM_Channel: specifies the TIM Channel
  *          This parameter can be one of the following values @ref TIM_Channel_definitions
  * @retval RTK_SUCCESS/RTK_FAIL
  */
u32 RTIM_IsChannelCaptureEnabled(RTIM_TypeDef *TIMx, u16 TIM_Channel)
{
	/* Check the parameters */
	assert_param(IS_TIM_PWM_TIM(TIMx));
	assert_param(IS_TIM_CHANNEL(TIM_Channel));

	if ((TIMx->CCRx[TIM_Channel] & (TIM_CCMode_Inputcapture | TIM_CCx_Enable)) == (TIM_CCMode_Inputcapture | TIM_CCx_Enable)) {
		return RTK_SUCCESS;
	} else {
		return RTK_FAIL;
	}

}