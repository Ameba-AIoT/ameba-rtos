/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_UPS_H_
#define _AMEBA_UPS_H_

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup UPS
  * @brief  UPS driver modules
  * @{
  */
//Please add your definition here

/* Exported types ------------------------------------------------------------*/
/** @defgroup UPS_Exported_Types UPS Exported Types
  * @{
  */

/**
  * @brief	UPS Init Structure Definition
  */
typedef struct {
	u8 UPS_ProducerType;		/*!< Specifies UPS producer signal type, which can be a value of
									@ref UPS_Producer_Signal_Type_Definitions */

	u8 UPS_ConsumerType;		/*!< Specifies UPS consumer signal type which can be a value of
									@ref UPS_Consumer_Signal_Type_Definitions */

	u8 UPS_TimerOfIn;			/*!< Specifies whether input timer overflow signal to UPS */

	u8 UPS_ReverseSig;			/*!< Specifies whether to reverse source signal from producer after receiving it */

	u16 UPS_DebounceCnt;		/*!< Specifies debounce count under XTAL40MHz clock */
} UPS_InitTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup UPS_Exported_Constants UPS Exported Constants
  * @{
  */

/** @defgroup UPS_Producer_Signal_Type_Definitions
  * @{
  */
#define UPS_SRC_NONE					((u8)0x0)
#define UPS_SRC_GPIO					((u8)0x1)
#define UPS_SRC_CMP						((u8)0x2)
#define UPS_SRC_AUDIO_0					((u8)0x3)
#define UPS_SRC_AUDIO_1					((u8)0x4)
#define UPS_SRC_AUDIO_2					((u8)0x5)

#define IS_UPS_PRODUCER_TYPE(sel)		((sel) < 0x0f)

/**
  * @}
  */

/** @defgroup UPS_Consumer_Signal_Type_Definitions
  * @{
  */
#define UPS_DST_NONE					((u8)0x01)
#define UPS_DST_TIM_EN					((u8)0x02)
#define UPS_DST_PWM_TRIG				((u8)0x04)

#define IS_UPS_CONSUMER_TYPE(sel)		(((sel) & 0x07) != 0 && \
										((sel) & 0xf8) == 0)

/**
  * @}
  */

/**
  * @}
  */

/** @} */

/** @} */

_LONG_CALL_ void UPS_StructInit(UPS_InitTypeDef *UPS_InitStruct);
_LONG_CALL_ void UPS_Init(UPS_InitTypeDef *UPS_InitStruct);
_LONG_CALL_ void UPS_SrcConfig(u8 SrcType);
_LONG_CALL_ void UPS_DstConfig(u8 DstType);
_LONG_CALL_ void UPS_TimOfInCmd(u32 NewState);
_LONG_CALL_ void UPS_ReverseCmd(u32 NewState);
_LONG_CALL_ void UPS_SetDbcCnt(u16 Dbc_Cnt);


#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */
#endif
