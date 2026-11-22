/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _RL7098_THERMAL_H_
#define _RL7098_THERMAL_H_

/** @addtogroup Ameba_Periph_Driver
 * @{
 */

/** @defgroup THERMAL THERMAL
 * @brief THERMAL driver modules
 * @{
 */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


// Please add your defination here

/* Exported types ------------------------------------------------------------*/
/** @defgroup THERMAL_Exported_Types THERMAL Exported Types
 * @{
 */

/**
 * @brief Thermal Init Structure Definition
 */
typedef struct {
	u16 TM_HighProtectThreshold; /*!< Specifies thermal high protection threshold.
									 When the measured temperature is greater than or equal to this value,
									 HW power down protection is triggered.
									 @note This parameter only between 0x046 and 0x08C is valid. */

	u16 TM_HighWarnThreshold; /*!< Specifies thermal high warning threshold.
							  The high warning interrupt is generated when the measured temperature is greater
							  than or equal to this value, and less than TM_HighProtectThreshold,
							  and tm_highcmp_wt_en=1, imr_tm_high_wt=1. */

	u16 TM_LowWarnThreshold; /*!< Specifies thermal low warning threshold.
							 The low warning interrupt is generated when the measured temperature is less
							 than or equal to this value, and tm_lowcmp_wt_en=1, imr_tm_low_wt=1. */

	u8 TM_TimePeriod; /*!< Specifies timer period between HW power down and restart.
					  When the measured temperature is greater than or equal to TM_HighProtectThreshold,
					  HW auto power down, the system will auto restart after this value time period*/

} TM_InitTypeDef;

/**
 * @brief Thermal temperature unit
 */
typedef enum {
	TM_DEG_C = 0, /*!< Celsius degree */
	TM_DEG_F = 1, /*!< Fahrenheit degree */
} TM_UnitDef;

/**
 * @}
 */

/* Exported constants --------------------------------------------------------*/
/** @defgroup THERMAL_Exported_Constants THERMAL Exported Constants
 * @{
 */

/// @cond
/** @defgroup TM_INT_TYPE TM Interrupt Type
 * @{
 */
/** @brief Checks if parameter is a valid thermal interrupt type. */
#define IS_TM_INT(int) (!((int)&~(ADC_BIT_IT_TM_LOW_WT_EN | ADC_BIT_IT_TM_HIGH_WT_EN)))
/**
 * @}
 */
/// @endcond

/** @defgroup TM_Programme_Password TM Programme Password
 * @{
 */
#define TM_PROG_PWR (0x69) /*!< Password value required to program thermal meter registers. */
/**
 * @}
 */

/** @defgroup TM_TEMP_DEFINE TM Temp Define
 * @{
 */

#define TM_INVALID_VALUE ((u32)0x80000) /*!< Encoded value indicating an invalid/unavailable temperature result. */

/**
 * @}
 */

/// @cond
/** @defgroup TM_INTR_CTRL TM Interrupt Control
 * @brief Thermal Meter Interrupt Control Register
 * @{
 */
#define TM_BIT_IMR_TM_HIGH_WT ADC_BIT_IT_TM_HIGH_WT_EN /*!< Interrupt control for thermal meter detect over temperature warning. */
#define TM_BIT_IMR_TM_LOW_WT  ADC_BIT_IT_TM_LOW_WT_EN  /*!< Interrupt control for thermal meter detect low temperature. */
/**
 * @}
 */
/// @endcond

/**
 * @}
 */

/** @} */

/** @} */

_LONG_CALL_ void TM_StructInit(TM_InitTypeDef *TM_InitStruct);
_LONG_CALL_ void TM_Init(TM_InitTypeDef *TM_InitStruct);
_LONG_CALL_ void TM_PwrProgCmd(u32 NewState);
_LONG_CALL_ void TM_Cmd(u32 NewState);
_LONG_CALL_ int TM_PollDataValid(void);
_LONG_CALL_ void TM_INTConfig(u32 TM_IT, u32 NewState);
_LONG_CALL_ void TM_INTClear(void);
_LONG_CALL_ void TM_INTClearPendingBits(u32 TM_IT);
_LONG_CALL_ u32 TM_GetISR(void);
_LONG_CALL_ u32 TM_GetTempResult(void);
_LONG_CALL_ u32 TM_GetPowOnTemp(void);
_LONG_CALL_ u32 TM_GetMaxTemp(void);
_LONG_CALL_ u32 TM_GetMinTemp(void);
_LONG_CALL_ void TM_MaxTempClr(void);
_LONG_CALL_ void TM_MinTempClr(void);
_LONG_CALL_ void TM_HighPtConfig(float TM_HighPtThre, u32 NewState, TM_UnitDef Unit);
_LONG_CALL_ void TM_HighWtConfig(float TM_HighWtThre, u32 NewState, TM_UnitDef Unit);
_LONG_CALL_ void TM_LowWtConfig(float TM_LowWtThre, u32 NewState, TM_UnitDef Unit);
_LONG_CALL_ float TM_GetCdegree(u32 Data);
_LONG_CALL_ float TM_GetFdegree(u32 Data);
_LONG_CALL_ float TM_GetDegree(u32 Data, TM_UnitDef Unit);

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif
