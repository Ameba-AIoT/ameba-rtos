/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_CLK_H_
#define _AMEBA_CLK_H_

/** @addtogroup AmebaD_Platform
  * @{
  */

/** @defgroup CLOCK
  * @brief CLOCK driver modules
  * @{
  */

/** @addtogroup CLOCK
  * @verbatim
  *****************************************************************************************
  * SDM32K
  *****************************************************************************************
  *	-RTC clock in
  *	-TIM0-TIM8 clock in
  *	-WIFI 32K clock in

  *****************************************************************************************
  * KM4 CPU clock
  *****************************************************************************************
  *	-CLK_KM4_200M: 200MHz
  *	-CLK_KM4_100M: 100MHz
  *	-CLK_KM4_50M: 50MHz
  *	-CLK_KM4_25M: 25MHz
  *	-CLK_KM4_XTAL: XTAL
  *
  *****************************************************************************************
  * XTAL clock
  *****************************************************************************************
  *	-Get XTAL clock from EFUSE setting:
  *	-40000000
  *	-25000000
  *	-13000000
  *	-19200000
  *	-20000000
  *	-26000000
  *	-38400000
  *	-17664000
  *	-16000000
  *	-14318000
  *	-12000000
  *	-52000000
  *	-48000000
  *	-26000000
  *	-27000000
  *	-24000000

  *****************************************************************************************
  * EXT32K
  *****************************************************************************************
  *	-External 32K: 32K clock from external 32k source
  *	-Internal 32K: 32K clock from internal 32K source: NCO32K
  *
  *****************************************************************************************
  * @endverbatim
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup CLK_Exported_Constants CLK Exported Constants
  * @{
  */
#define MHZ_TICK_CNT 		(1000000)

/** @defgroup CPU_CLK_Source definitions
  * @{
  */

#define CLK_KM4_PLL			1
#define CLK_KM4_XTAL		0 /* Nearly all the Time, Lbus the same with XTAL*/
/**
  * @}
  */

/** @defgroup LP_CPU_CLK_Source definitions
  * @{
  */

#define CLK_KM0_PLL			3 /* 3 for pll */
#define CLK_KM0_XTAL		1 /* Nearly all the Time, Lbus the same with XTAL*/
#define CLK_KM0_ANA_4M		0

/**
  * @}
  */


/** @defgroup PLL_SEL_definitions
  * @{
  */
#define PLL_I2S		0
#define PLL_PCM		1

/**
  * @}
  */

/** @defgroup PLL_ClkTune_definitions
  * @{
  */
#define PLL_AUTO		0
#define PLL_FASTER		1
#define PLL_SLOWER		2

/**
  * @}
  */

/** @defgroup CAL_CLK_SEL_definitions
  * @{
  */
#define OSC4M_CAL_CLK		0
#define AON100K_CAL_CLK		1
#define AON128K_CAL_CLK		2
/**
  * @}
  */

/** @defgroup XTAL_MODE_SEL_definitions
  * @{
  */
#define XTAL_NM_MODE		2
#define XTAL_LPS_MODE		1
#define XTAL_HP_MODE		3
/**
  * @}
  */

/** @defgroup PLL_CLK_definitions
  * @{
  */
#define PLL_300M		((u32)300000000)	/*This is minimum value*/
#define PLL_330M		((u32)330000000)
#define PLL_331M		((u32)331000000)	/*Combined with 100 ohms of driving strength, RF has the smallest impact.*/
#define PLL_334M		((u32)334000000)
#define PLL_384M		((u32)384000000)	/* generate 48M for USB In Xmoderm */
#define PLL_392M		((u32)392000000)
#define PLL_400M		((u32)400000000)
#define PLL_480M		((u32)480000000)
#define PLL_512M		((u32)512000000)
#define PLL_520M		((u32)520000000)
#define PLL_524M		((u32)524000000)
#define PLL_587P0592M	((u32)587059200)
#define PLL_589P824M	((u32)589824000)	/* generate 98.304M for I2S */
#define PLL_600M		((u32)600000000)
#define PLL_677P376M	((u32)677376000)
#define PLL_688P128M	((u32)688128000)	/*This is maximum value*/
/**
  * @}
  */

/** @defgroup Soc_CLK_LIMIT_definitions
  * @{
  */
#define CLKDIV(x)				(x)
#define CLKDIV_ROUND_UP(X, Y)	( ( ( X ) + ( Y ) - 1 ) / ( Y ) )

#define KM4_0P9V_CLK_LIMIT		(262 * MHZ_TICK_CNT)
#define KM4_1P0V_CLK_LIMIT		(345 * MHZ_TICK_CNT)
#define KM0_0P9V_CLK_LIMIT		(105 * MHZ_TICK_CNT)
#define KM0_1P0V_CLK_LIMIT		(115 * MHZ_TICK_CNT)

#define PSRAMC_CLK_LIMIT		(400 * MHZ_TICK_CNT)
#define SPIC_CLK_LIMIT			(208 * MHZ_TICK_CNT)	/* For Flash run up to 104MHz */
#define QSPI_CLK_LIMIT			(100 * MHZ_TICK_CNT)
#define PERI_CLK_LIMIT			(100 * MHZ_TICK_CNT)
#define SRAM_CLK_LIMIT			(200 * MHZ_TICK_CNT)

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SDM32K_CLK_Exported_Functions SDM32K_CLK Exported Functions
  * @{
  */
_LONG_CALL_ void SDM32K_Enable(void);
_LONG_CALL_ void SDM32K_TimerCalEnable(u32 RTCCalibration);
/**
  * @}
  */


/** @defgroup CPU_CLK_Exported_Functions CPU_CLK Exported Functions
  * @{
  */
_LONG_CALL_ u32 CPU_ClkGet(void);
_LONG_CALL_ void _CPU_ClkSet(u32 CpuClk);
_LONG_CALL_ void _CPU_ClkSrcSet(u32 Source);

/**
  * @}
  */

/** @defgroup XTAL_CLK_Exported_Functions XTAL_CLK Exported Functions
  * @{
  */
_LONG_CALL_ u32 XTAL_ClkGet(void);
_LONG_CALL_ u32 PLL_ClkGet(void);
/**
  * @}
  */

/** @defgroup PLL_Exported_Functions PLL Exported Functions
  * @{
  */
_LONG_CALL_ void PLL_I2S_Div(int Sportx, u32 div);
_LONG_CALL_ void PLL_I2S0_CLK_Auto(u32 status, u32 clk);
_LONG_CALL_ void PLL_I2S1_CLK_Auto(u32 status, u32 clk);
_LONG_CALL_ void PLL_I2S0_CLK(u32 status, u32 clk);
_LONG_CALL_ void PLL_I2S1_CLK(u32 status, u32 clk);
_LONG_CALL_ u32 PLL_I2S_CLKGet(void);
_LONG_CALL_ float PLL_I2S_98P304M_ClkTune(float ppm, u32 action);
_LONG_CALL_ float PLL_I2S_45P1584M_ClkTune(float ppm, u32 action);
_LONG_CALL_ void PLL_NP(u32 NewState);
_LONG_CALL_ void PLL_AP(u32 Option, u32 NewState);
_LONG_CALL_ void PLL_AP_ClkTune(u32 Option);

/**
  * @}defgroup PLL_Exported_Functions PLL Exported Functions
  */
_LONG_CALL_ void PLL_Cmd(u32 status);
_LONG_CALL_ void PLL_BG_Cmd(u32 status);
_LONG_CALL_ bool PLL_State(void);
_LONG_CALL_ void PLL_ClkSet(u32 PllClk);
_LONG_CALL_ void OSC131K_Reset(void);


/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup CLK_Register_Definitions CLK Register Definitions
 * @{
 *****************************************************************************/

/**************************************************************************//**
 * @defgroup REG_SYS_SYSPLL_CTRL2
 * @{
 *****************************************************************************/
#define BIT_SYS_SYSPLL_CK_ADC_EN		(0x00000001 << 25) /*!< Set ADC PLL EN */
/** @} */

/** @defgroup PLL_BandGap_Mask_definitions
  * @{
  */
#define PLL_BG_POW_MASK	((u32)0x00000005)

/** @} */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/* Other definations --------------------------------------------------------*/
u32 OSC4M_Calibration(u32 ppm_limit);
u32 OSC131K_Calibration(u32 ppm_limit);
void OSC4M_Init(void);
void XTAL_INIT(void);
void XTAL_AACK(void);
#endif //_AMEBA_CLK_H_