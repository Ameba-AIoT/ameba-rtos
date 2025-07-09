/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

u16 REG_LSYS_UPS_CTRLx[4] = {
	REG_LSYS_UPS_CTRL0,
	REG_LSYS_UPS_CTRL1,
	REG_LSYS_UPS_CTRL2,
	REG_LSYS_UPS_CTRL3
};

/**
  * @brief  Initialize the parameters in the UPS_InitStruct with default values.
  * @param  UPS_InitStruct: pointer to a UPS_InitTypeDef structure that contains
  *         the configuration information for the UPS.
  * @retval  None
  */
void UPS_StructInit(UPS_InitTypeDef *UPS_InitStruct)
{
	UPS_InitStruct->UPS_ProducerType = UPS_SRC_GPIO;
	UPS_InitStruct->UPS_ConsumerType = UPS_DST_TIM_EN | UPS_DST_PWM_TRIG;
	UPS_InitStruct->UPS_TimerOfIn = ENABLE;
	UPS_InitStruct->UPS_ReverseSig = DISABLE;
	UPS_InitStruct->UPS_DebounceCnt = 0x0;
}

/**
  * @brief  Initialize the UPS according to the specified parameters in UPS_InitStruct.
  * @param  UPS_InitStruct: pointer to a UPS_InitTypeDef structure that contains
  *         the configuration information for the UPS.
  * @param  idx: 0~3
  * @retval  None
  */
void UPS_Init(UPS_InitTypeDef *UPS_InitStruct, u8 idx)
{
	UPS_SrcConfig(UPS_InitStruct->UPS_ProducerType, idx);
	UPS_DstConfig(UPS_InitStruct->UPS_ConsumerType, idx);
	UPS_TimOfInCmd(UPS_InitStruct->UPS_TimerOfIn, idx);
	UPS_ReverseCmd(UPS_InitStruct->UPS_ReverseSig, idx);
	UPS_SetDbcCnt(UPS_InitStruct->UPS_DebounceCnt, idx);
}

/**
  * @brief  Specify the source signal type of UPS.
  * @param  SrcType: Specified signal source.
  *   			This parameter can be a value of @ref UPS_Producer_Signal_Type_Definitions.
  * @param  idx: 0~3
  * @retval None
  */
void UPS_SrcConfig(u8 SrcType, u8 idx)
{
	assert_param(IS_UPS_PRODUCER_TYPE(SrcType));
	assert_param(idx < 4);

	u32 Temp = HAL_READ32(UPS_REG_BASE, REG_LSYS_UPS_CTRLx[idx]);
	Temp &= ~TIM_MASK_UPS0_SRC_SEL;
	Temp |= TIM_UPS0_SRC_SEL(SrcType);
	HAL_WRITE32(UPS_REG_BASE, REG_LSYS_UPS_CTRLx[idx], Temp);
}

/**
  * @brief  Specify the destination signal type of UPS.
  * @param  DstType: Specified signal destination.
  *   			This parameter can be a value or combinations of @ref UPS_Consumer_Signal_Type_Definitions.
  * @param  idx: 0~3
  * @retval None
  */
void UPS_DstConfig(u8 DstType, u8 idx)
{
	assert_param(IS_UPS_CONSUMER_TYPE(DstType));
	assert_param(idx < 4);

	if (DstType == UPS_DST_NONE) {
		u32 Temp = HAL_READ32(UPS_REG_BASE, REG_LSYS_UPS_CTRLx[idx]);
		Temp &= ~(TIM_BIT_UPS0_OUT_PWM_TRIG | TIM_BIT_UPS0_OUT_TIM_EN);
		HAL_WRITE32(UPS_REG_BASE, REG_LSYS_UPS_CTRLx[idx], Temp);
		return;
	}

	if (DstType & UPS_DST_TIM_EN) {
		u32 Temp = HAL_READ32(UPS_REG_BASE, REG_LSYS_UPS_CTRLx[idx]);
		Temp |= TIM_BIT_UPS0_OUT_TIM_EN;
		HAL_WRITE32(UPS_REG_BASE, REG_LSYS_UPS_CTRLx[idx], Temp);
	}

	if (DstType & UPS_DST_PWM_TRIG) {
		u32 Temp = HAL_READ32(UPS_REG_BASE, REG_LSYS_UPS_CTRLx[idx]);
		Temp |= TIM_BIT_UPS0_OUT_PWM_TRIG;
		HAL_WRITE32(UPS_REG_BASE, REG_LSYS_UPS_CTRLx[idx], Temp);
	}
}

/**
  * @brief Enable or disable basic timer overflow input.
  * @param NewState: can be ENABLE or DISABLE.
  * @param  idx: 0~3
  * @retval None
  */
void UPS_TimOfInCmd(u32 NewState, u8 idx)
{
	u32 Temp = HAL_READ32(UPS_REG_BASE, REG_LSYS_UPS_CTRLx[idx]);

	if (NewState != DISABLE) {
		Temp |= TIM_BIT_UPS0_IN_TIM_OF;
	} else {
		Temp &= ~TIM_BIT_UPS0_IN_TIM_OF;
	}

	HAL_WRITE32(UPS_REG_BASE, REG_LSYS_UPS_CTRLx[idx], Temp);
}

/**
  * @brief Enable or disable signal_reverse function in circuit.
  * @param  NewState: can be one of the following value:
  *			@arg ENABLE: Reverse source signal.
  *			@arg DISABLE: Do not reverse source signal.
  * @param  idx: 0~3
  * @retval  None
  */
void UPS_ReverseCmd(u32 NewState, u8 idx)
{
	u32 Temp = HAL_READ32(UPS_REG_BASE, REG_LSYS_UPS_CTRLx[idx]);
	Temp &= ~TIM_BIT_UPS0_RVS_EN;

	if (NewState != DISABLE) {
		Temp |= TIM_BIT_UPS0_RVS_EN;
	}

	HAL_WRITE32(UPS_REG_BASE, REG_LSYS_UPS_CTRLx[idx], Temp);
}

/**
  * @brief  Set debounce counter under XTAL40M to realize signal deglitch.
  * @param  Dbc_Cnt: Debounce counter to be set.
  * @param  idx: 0~3
  *	@note Dbc_Cnt = 0 means debounce function is disabled.
  * @retval None
  */
void UPS_SetDbcCnt(u16 Dbc_Cnt, u8 idx)
{
	u32 Temp = HAL_READ32(UPS_REG_BASE, REG_LSYS_UPS_CTRLx[idx]);
	Temp &= ~TIM_MASK_UPS0_DBC_CNT;
	Temp |= TIM_UPS0_DBC_CNT(Dbc_Cnt);
	HAL_WRITE32(UPS_REG_BASE, REG_LSYS_UPS_CTRLx[idx], Temp);
}