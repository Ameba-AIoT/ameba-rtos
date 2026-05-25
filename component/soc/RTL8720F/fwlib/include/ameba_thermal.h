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

/** @defgroup ADC
 * @brief ADC driver modules
 * @{
 */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


// Please add your defination here

/* Exported types ------------------------------------------------------------*/
/** @addtogroup ADC_Exported_Types ADC Exported Types
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
 * @}
 */

/* Exported constants --------------------------------------------------------*/
/** @defgroup ADC_Exported_Constants ADC Exported Constants
 * @{
 */

/** @defgroup TM_INT_TYPE
 * @{
 */
#define IS_TM_INT(int) (!((int)&~(ADC_BIT_IT_TM_LOW_WT_EN | ADC_BIT_IT_TM_HIGH_WT_EN)))
/**
 * @}
 */

/** @defgroup TM_Programme_Password
 * @{
 */
#define TM_PROG_PWR (0x69)
/**
 * @}
 */

/** @defgroup TM_TEMP_DEFINE
 * @{
 */

#define TM_INVALID_VALUE ((u32)0x80000) // unavailable value

/**
 * @}
 */

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
_LONG_CALL_ void TM_HighPtConfig(u16 TM_HighPtThre, u32 NewState);
_LONG_CALL_ void TM_HighWtConfig(u16 TM_HighWtThre, u32 NewState);
_LONG_CALL_ void TM_LowWtConfig(u16 TM_LowWtThre, u32 NewState);
_LONG_CALL_ float TM_GetCdegree(u32 Data);
_LONG_CALL_ float TM_GetFdegree(u32 Data);
_LONG_CALL_ u32 TM_GetTempCode(float TempC);

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif
