/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSREG_SDM_H_
#define _SYSREG_SDM_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup SDM
  * @brief SDM driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup SDM_Register_Definitions SDM Register Definitions
  * @{
  */

/** @defgroup SDM_CTRL0
  * @brief SDM Control Register
  * @{
  */
#define SDM_BIT_EN             ((u32)0x00000001 << 31)          /*!< R/W 0x0  Enable SDM*/
#define SDM_BIT_RST            ((u32)0x00000001 << 30)          /*!< R/W 0x0  0: Reset SDM*/
#define SDM_BIT_BYPASSS_MOD_EN ((u32)0x00000001 << 29)          /*!< R/W 0x0  Bypass mode enable*/
#define SDM_BIT_MOD_SEL        ((u32)0x00000001 << 28)          /*!< R/W 0x0  0: Default mode 1: User-defined parameter mode*/
#define SDM_BIT_CAL_DONE       ((u32)0x00000001 << 27)          /*!< R 0x0  */
#define SDM_BIT_TIME_LOSS_SET  ((u32)0x00000001 << 25)          /*!< R/W1P 0x0  Time loss set 1: User-defined*/
#define SDM_BIT_TIME_LOSS_HIGH ((u32)0x00000001 << 24)          /*!< R/W 0x0  Decide 32k output compensation direction 1: Mask 0: Add*/
#define SDM_BIT_ALWAYS_CAL_EN  ((u32)0x00000001 << 18)          /*!< R/W 0x0  1: Enable always_cal*/
#define SDM_BIT_TIMER_CAL_EN   ((u32)0x00000001 << 17)          /*!< R/W 0x0  1: Enable timer_cal*/
#define SDM_MASK_XTAL_SEL      ((u32)0x0000000F << 8)           /*!< R/W 0x0  XTAL frequency select, default 40M * 4'h0: XTAL40M * 4'h1: XTAL25M * 4'h4: XTAL20M * 4'h5: XTAL26M * 4'hD: XTAL27M * 4'hE: XTAL24M*/
#define SDM_XTAL_SEL(x)        (((u32)((x) & 0x0000000F) << 8))
#define SDM_GET_XTAL_SEL(x)    ((u32)(((x >> 8) & 0x0000000F)))
/** @} */

/** @defgroup SDM_CTRL1
  * @brief Config observed 128K cycle
  * @{
  */
#define SDM_MASK_OBS_CYC   ((u32)0x0000FFFF << 0)           /*!< R/W 0x0  Set observed 128k cycle, default 1024 128k cycles default value:  when XTAL40M, OBS_CYC=1024 when XTAL25M, OBS_CYC=2048 when XTAL20M, OBS_CYC=1024 when XTAL26M, OBS_CYC=2432 when XTAL27M, OBS_CYC=2368 when XTAL24M, OBS_CYC=2688*/
#define SDM_OBS_CYC(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define SDM_GET_OBS_CYC(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup SDM_CTRL2
  * @brief Set ref counter value
  * @{
  */
#define SDM_MASK_OBS_REF_CYC   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Set in observed cycles, refer to counters value, default 312500 XTAL cycles default value:  when XTAL40M, OBS_REF_CYC=312500 when XTAL25M, OBS_REF_CYC=390625 when XTAL20M, OBS_REF_CYC=156250 when XTAL26M, OBS_REF_CYC=482421 when XTAL27M, OBS_REF_CYC=487792 when XTAL24M, OBS_REF_CYC=492187*/
#define SDM_OBS_REF_CYC(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SDM_GET_OBS_REF_CYC(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SDM_CTRL3
  * @brief Set XTAL period
  * @{
  */
#define SDM_MASK_XTAL_PERIOD   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Set reference xtal period , equivalent amplified xtal clock period value. Observed 128k period is equivalently magnified to 31250000, so   XTAL_PERIOD = 31250000 * SDM_OBS_CYC / SDM_OBS_REF_CYC default value:  when XTAL40M, XTAL_PERIOD=102400 when XTAL25M, XTAL_PERIOD=163840 when XTAL20M, XTAL_PERIOD=204800 when XTAL26M, XTAL_PERIOD=157539 when XTAL27M, XTAL_PERIOD=151704 when XTAL24M, XTAL_PERIOD=170667*/
#define SDM_XTAL_PERIOD(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SDM_GET_XTAL_PERIOD(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SDM_CTRL4
  * @brief SDM Debug Register
  * @{
  */
#define SDM_BIT_DBG_EN     ((u32)0x00000001 << 3)           /*!< R/W 0x0  1: Enable SDM debug*/
#define SDM_MASK_DBG_SEL   ((u32)0x00000007 << 0)           /*!< R/W 0x0  Set in observed cycles, refer to counters value*/
#define SDM_DBG_SEL(x)     (((u32)((x) & 0x00000007) << 0))
#define SDM_GET_DBG_SEL(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup SDM_CTRL5
  * @brief Time loss Set
  * @{
  */
#define SDM_MASK_TIME_LOSS_REG   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Source clock time loss every cycle*/
#define SDM_TIME_LOSS_REG(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SDM_GET_TIME_LOSS_REG(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SDM_TIME_LOSS_MUX
  * @brief Time loss mux
  * @{
  */
#define SDM_MASK_TIME_LOSS_MUX   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  Debug signal: Calibration time loss mux*/
#define SDM_TIME_LOSS_MUX(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SDM_GET_TIME_LOSS_MUX(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SDM_TIMEOUT
  * @brief Set Timer Trigger Counter Value
  * @{
  */
#define SDM_MASK_TIMEOUT_CNT   ((u32)0xFFFFFFFF << 0)           /*!< R/W 2000000h  Set SDM timer trigger calibration timeout value , unit: 32k period*/
#define SDM_TIMEOUT_CNT(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SDM_GET_TIMEOUT_CNT(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup SDM_Exported_Types SDM Exported Types
  * @{
  */

/** @brief SDM Register Declaration
  */

typedef struct {
	__IO uint32_t SDM_CTRL0        ;  /*!< SDM CONTROL REGISTER,  Address offset:0x000 */
	__IO uint32_t SDM_CTRL1        ;  /*!< CONFIG OBSERVED 128K CYCLE Register,  Address offset:0x004 */
	__IO uint32_t SDM_CTRL2        ;  /*!< SET REF COUNTER VALUE Register,  Address offset:0x008 */
	__IO uint32_t SDM_CTRL3        ;  /*!< SET XTAL PERIOD Register,  Address offset:0x00C */
	__IO uint32_t SDM_CTRL4        ;  /*!< SDM DEBUG REGISTER,  Address offset:0x010 */
	__IO uint32_t SDM_CTRL5        ;  /*!< TIME LOSS SET Register,  Address offset:0x014 */
	__I  uint32_t SDM_TIME_LOSS_MUX;  /*!< TIME LOSS MUX Register,  Address offset:0x018 */
	__IO uint32_t SDM_TIMEOUT      ;  /*!< SET TIMER TRIGGER COUNTER VALUE Register,  Address offset:0x01C */
	__IO uint32_t SDM_DUMMY        ;  /*!< SDM DUMMY REGISTER,  Address offset:0x020 */
} SDM_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif