/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_UPS_H_
#define _AMEBA_UPS_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup UPS
  * @brief UPS driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup UPS_Register_Definitions UPS Register Definitions
  * @{
  */

/** @defgroup TIM_UPS_CTRL0
  * @brief
  * @{
  */
#define TIM_MASK_UPS0_SRC_SEL     ((u32)0x0000000F << 28)           /*!< R/W 0x0  UPS trigger source select: 000: none 001: gpio trigger 010: comparator output 011: audio zcd_0 100: audio zcd_1 101: audio zcd_2*/
#define TIM_UPS0_SRC_SEL(x)       (((u32)((x) & 0x0000000F) << 28))
#define TIM_GET_UPS0_SRC_SEL(x)   ((u32)(((x >> 28) & 0x0000000F)))
#define TIM_BIT_UPS0_OUT_PWM_TRIG ((u32)0x00000001 << 21)           /*!< R/W 0x0  Enable for output trigger to pwm*/
#define TIM_BIT_UPS0_OUT_TIM_EN   ((u32)0x00000001 << 20)           /*!< R/W 0x0  Enable for output en signal to basic timer*/
#define TIM_BIT_UPS0_IN_TIM_OF    ((u32)0x00000001 << 19)           /*!< R/W 0x0  Enable for basic timer overflow input*/
#define TIM_BIT_UPS0_RVS_EN       ((u32)0x00000001 << 18)           /*!< R/W 0x0  Invert polarity for UPS input 0: posedge polarity 1: negedge polarity*/
#define TIM_MASK_UPS0_DBC_CNT     ((u32)0x0000FFFF << 2)            /*!< R/W 0x0  Debounce count for UPS funciton*/
#define TIM_UPS0_DBC_CNT(x)       (((u32)((x) & 0x0000FFFF) << 2))
#define TIM_GET_UPS0_DBC_CNT(x)   ((u32)(((x >> 2) & 0x0000FFFF)))
/** @} */

/** @defgroup TIM_UPS_CTRL1
  * @brief
  * @{
  */
#define TIM_MASK_UPS1_SRC_SEL     ((u32)0x0000000F << 28)           /*!< R/W 0x0  UPS trigger source select: 000: none 001: gpio trigger 010: comparator output 011: audio zcd_0 100: audio zcd_1 101: audio zcd_2*/
#define TIM_UPS1_SRC_SEL(x)       (((u32)((x) & 0x0000000F) << 28))
#define TIM_GET_UPS1_SRC_SEL(x)   ((u32)(((x >> 28) & 0x0000000F)))
#define TIM_BIT_UPS1_OUT_PWM_TRIG ((u32)0x00000001 << 21)           /*!< R/W 0x0  Enable for output trigger to pwm*/
#define TIM_BIT_UPS1_OUT_TIM_EN   ((u32)0x00000001 << 20)           /*!< R/W 0x0  Enable for output en signal to basic timer*/
#define TIM_BIT_UPS1_IN_TIM_OF    ((u32)0x00000001 << 19)           /*!< R/W 0x0  Enable for basic timer overflow input*/
#define TIM_BIT_UPS1_RVS_EN       ((u32)0x00000001 << 18)           /*!< R/W 0x0  Invert polarity for UPS input 0: posedge polarity 1: negedge polarity*/
#define TIM_MASK_UPS1_DBC_CNT     ((u32)0x0000FFFF << 2)            /*!< R/W 0x0  Debounce count for UPS funciton*/
#define TIM_UPS1_DBC_CNT(x)       (((u32)((x) & 0x0000FFFF) << 2))
#define TIM_GET_UPS1_DBC_CNT(x)   ((u32)(((x >> 2) & 0x0000FFFF)))
/** @} */

/** @defgroup TIM_UPS_CTRL2
  * @brief
  * @{
  */
#define TIM_MASK_UPS2_SRC_SEL     ((u32)0x0000000F << 28)           /*!< R/W 0x0  UPS trigger source select: 000: none 001: gpio trigger 010: comparator output 011: audio zcd_0 100: audio zcd_1 101: audio zcd_2*/
#define TIM_UPS2_SRC_SEL(x)       (((u32)((x) & 0x0000000F) << 28))
#define TIM_GET_UPS2_SRC_SEL(x)   ((u32)(((x >> 28) & 0x0000000F)))
#define TIM_BIT_UPS2_OUT_PWM_TRIG ((u32)0x00000001 << 21)           /*!< R/W 0x0  Enable for output trigger to pwm*/
#define TIM_BIT_UPS2_OUT_TIM_EN   ((u32)0x00000001 << 20)           /*!< R/W 0x0  Enable for output en signal to basic timer*/
#define TIM_BIT_UPS2_IN_TIM_OF    ((u32)0x00000001 << 19)           /*!< R/W 0x0  Enable for basic timer overflow input*/
#define TIM_BIT_UPS2_RVS_EN       ((u32)0x00000001 << 18)           /*!< R/W 0x0  Invert polarity for UPS input 0: posedge polarity 1: negedge polarity*/
#define TIM_MASK_UPS2_DBC_CNT     ((u32)0x0000FFFF << 2)            /*!< R/W 0x0  Debounce count for UPS funciton*/
#define TIM_UPS2_DBC_CNT(x)       (((u32)((x) & 0x0000FFFF) << 2))
#define TIM_GET_UPS2_DBC_CNT(x)   ((u32)(((x >> 2) & 0x0000FFFF)))
/** @} */

/** @defgroup TIM_UPS_CTRL3
  * @brief
  * @{
  */
#define TIM_MASK_UPS3_SRC_SEL     ((u32)0x0000000F << 28)           /*!< R/W 0x0  UPS trigger source select: 000: none 001: gpio trigger 010: comparator output 011: audio zcd_0 100: audio zcd_1 101: audio zcd_2*/
#define TIM_UPS3_SRC_SEL(x)       (((u32)((x) & 0x0000000F) << 28))
#define TIM_GET_UPS3_SRC_SEL(x)   ((u32)(((x >> 28) & 0x0000000F)))
#define TIM_BIT_UPS3_OUT_PWM_TRIG ((u32)0x00000001 << 21)           /*!< R/W 0x0  Enable for output trigger to pwm*/
#define TIM_BIT_UPS3_OUT_TIM_EN   ((u32)0x00000001 << 20)           /*!< R/W 0x0  Enable for output en signal to basic timer*/
#define TIM_BIT_UPS3_IN_TIM_OF    ((u32)0x00000001 << 19)           /*!< R/W 0x0  Enable for basic timer overflow input*/
#define TIM_BIT_UPS3_RVS_EN       ((u32)0x00000001 << 18)           /*!< R/W 0x0  Invert polarity for UPS input 0: posedge polarity 1: negedge polarity*/
#define TIM_MASK_UPS3_DBC_CNT     ((u32)0x0000FFFF << 2)            /*!< R/W 0x0  Debounce count for UPS funciton*/
#define TIM_UPS3_DBC_CNT(x)       (((u32)((x) & 0x0000FFFF) << 2))
#define TIM_GET_UPS3_DBC_CNT(x)   ((u32)(((x >> 2) & 0x0000FFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup UPS_Exported_Types UPS Exported Types
  * @{
  */

/** @brief UPS Register Declaration
  */

typedef struct {
	__IO uint32_t TIM_UPS_CTRL0;  /*!< Register,  Address offset:0x000 */
	__IO uint32_t TIM_UPS_CTRL1;  /*!< Register,  Address offset:0x004 */
	__IO uint32_t TIM_UPS_CTRL2;  /*!< Register,  Address offset:0x008 */
	__IO uint32_t TIM_UPS_CTRL3;  /*!< Register,  Address offset:0x00C */
	__IO uint32_t TIM_PRS_DUMMY;  /*!< PRS DUMMY REGISTER,  Address offset:0x010 */
} UPS_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

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

/** @defgroup UREG_LSYS_UPS_CTRLx_Addr_Offset
  * @{
  */
#define REG_LSYS_UPS_CTRL0            0x00
#define REG_LSYS_UPS_CTRL1            0x04
#define REG_LSYS_UPS_CTRL2            0x08
#define REG_LSYS_UPS_CTRL3            0x0C

/**
  * @}
  */

/** @defgroup UPS_Producer_Signal_Type_Definitions
  * @{
  */
#define UPS_SRC_NONE					      ((u8)0x0)
#define UPS_SRC_GPIO				      	((u8)0x1)
#define UPS_SRC_CMP						      ((u8)0x2)

#define IS_UPS_PRODUCER_TYPE(sel)		((sel) < 0x0f)

/**
  * @}
  */

/** @defgroup UPS_Consumer_Signal_Type_Definitions
  * @{
  */
#define UPS_DST_NONE					  ((u8)0x01)
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

/* Exported functions --------------------------------------------------------*/
/** @defgroup UPS_Exported_Functions UPS Exported Functions
  * @{
  */

_LONG_CALL_ void UPS_StructInit(UPS_InitTypeDef *UPS_InitStruct);
_LONG_CALL_ void UPS_Init(UPS_InitTypeDef *UPS_InitStruct, u8 idx);
_LONG_CALL_ void UPS_SrcConfig(u8 SrcType, u8 idx);
_LONG_CALL_ void UPS_DstConfig(u8 DstType, u8 idx);
_LONG_CALL_ void UPS_TimOfInCmd(u32 NewState, u8 idx);
_LONG_CALL_ void UPS_ReverseCmd(u32 NewState, u8 idx);
_LONG_CALL_ void UPS_SetDbcCnt(u16 Dbc_Cnt, u8 idx);

/**
  * @}
  */
/** @} */

/** @} */

/* MANUAL_GEN_END */
#endif