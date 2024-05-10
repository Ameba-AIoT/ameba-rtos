/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup UPS
  * @brief  UPS driver modules
  * @{
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup UPS_Exported_Functions UPS Exported Functions
  * @{
  */

/**
  * @brief  Initializes the parameters in the UPS_InitStruct with default values.
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
  * @brief  Initializes the UPS according to the specified parameters in UPS_InitStruct.
  * @param  UPS_InitStruct: pointer to a UPS_InitTypeDef structure that contains
  *         the configuration information for the UPS.
  * @retval  None
  */
void UPS_Init(UPS_InitTypeDef *UPS_InitStruct)
{
	UPS_SrcConfig(UPS_InitStruct->UPS_ProducerType);
	UPS_DstConfig(UPS_InitStruct->UPS_ConsumerType);
	UPS_TimOfInCmd(UPS_InitStruct->UPS_TimerOfIn);
	UPS_ReverseCmd(UPS_InitStruct->UPS_ReverseSig);
	UPS_SetDbcCnt(UPS_InitStruct->UPS_DebounceCnt);
}

/**
  * @brief  Specify the source signal type of UPS.
  * @param  SrcType: Specified signal source.
  *   			This parameter can be a value of @ref UPS_Producer_Signal_Type_Definitions.
  * @retval None
  */
void UPS_SrcConfig(u8 SrcType)
{
	assert_param(IS_UPS_PRODUCER_TYPE(SrcType));

	u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PRS_CTRL);
	Temp &= ~LSYS_MASK_PRS_SRC_SEL;
	Temp |= LSYS_PRS_SRC_SEL(SrcType);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PRS_CTRL, Temp);
}

/**
  * @brief  Specify the destination signal type of UPS.
  * @param  DstType: Specified signal destination.
  *   			This parameter can be a value or combinations of @ref UPS_Consumer_Signal_Type_Definitions.
  * @retval None
  */
void UPS_DstConfig(u8 DstType)
{
	assert_param(IS_UPS_CONSUMER_TYPE(DstType));

	if (DstType == UPS_DST_NONE) {
		u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PRS_CTRL);
		Temp &= ~(LSYS_BIT_PRS_OUT_PWM_TRIG | LSYS_BIT_PRS_OUT_TIM_EN);
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PRS_CTRL, Temp);
		return;
	}

	if (DstType & UPS_DST_TIM_EN) {
		u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PRS_CTRL);
		Temp |= LSYS_BIT_PRS_OUT_TIM_EN;
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PRS_CTRL, Temp);
	}

	if (DstType & UPS_DST_PWM_TRIG) {
		u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PRS_CTRL);
		Temp |= LSYS_BIT_PRS_OUT_PWM_TRIG;
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PRS_CTRL, Temp);
	}
}

/**
  * @brief Enable or disable basic timer overflow input.
  * @param NewState: can be ENABLE or DISABLE.
  * @retval None.
  */
void UPS_TimOfInCmd(u32 NewState)
{
	u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PRS_CTRL);

	if (NewState != DISABLE) {
		Temp |= LSYS_BIT_PRS_IN_TIM_OF;
	} else {
		Temp &= ~LSYS_BIT_PRS_IN_TIM_OF;
	}

	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PRS_CTRL, Temp);
}

/**
  * @brief Enable or disable signal_reverse function in circuit.
  * @param  NewState: can be one of the following value:
  *			@arg ENABLE: Reverse source signal.
  *			@arg DISABLE: Do not reverse source signal.
  * @retval  None.
  */
void UPS_ReverseCmd(u32 NewState)
{
	u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PRS_CTRL);
	Temp &= ~LSYS_BIT_PRS_RVS_EN;

	if (NewState != DISABLE) {
		Temp |= LSYS_BIT_PRS_RVS_EN;
	}

	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PRS_CTRL, Temp);
}

/**
  * @brief  Set debounce counter under XTAL40M to realize signal deglitch.
  * @param  Dbc_Cnt: Debounce counter to be set.
  *	@note Dbc_Cnt = 0 means debounce function is disabled.
  * @retval None
  */
void UPS_SetDbcCnt(u16 Dbc_Cnt)
{
	u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PRS_CTRL);
	Temp &= ~LSYS_MASK_PRS_DBC_CNT;
	Temp |= LSYS_PRS_DBC_CNT(Dbc_Cnt);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PRS_CTRL, Temp);
}
/**
  * @}
  */

/** @} */

/** @} */