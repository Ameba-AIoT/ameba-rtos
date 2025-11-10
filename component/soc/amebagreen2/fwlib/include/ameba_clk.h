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
  * AP CPU clock
  *****************************************************************************************
  *	-CLK_AP_200M: 200MHz
  *	-CLK_AP_100M: 100MHz
  *	-CLK_AP_50M: 50MHz
  *	-CLK_AP_25M: 25MHz
  *	-CLK_AP_XTAL: XTAL
  *
  *****************************************************************************************
  * XTAL clock
  *****************************************************************************************

  *	-40000000

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
#define CLK_CPU_USB_PLL     CKSL_HP_USB_PLL
#define CLK_CPU_SYS_PLL     CKSL_HP_SYS_PLL
#define CLK_CPU_XTAL        CKSL_HP_XTAL
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

/** @defgroup SDM_USER_DEFINED_param_definitions
  * @{
  */
#define SDM_OBS_CYC_DEFAULT		1024
#define SDM_OBS_REF_CYC_DEFAULT		312500
#define SDM_OBS_REF_CYC_MODIFY		312503
#define SDM_XTAL_PERIOD_DEFAULT   102400
/**
  * @}
  */

/** @defgroup XTAL_CLK_definitions
* @{
*/
#define XTAL_40M		((u32)40000000)	/*This is default value*/
/**
  * @}
  */

/** @defgroup PLL_CLK_definitions
  * @{
  */
#define PLL_NONE      ((u32)0)
#define PLL_361P2672M  ((u32)361267200) /* generate 45.1584M for I2S */
#define PLL_393P216M  ((u32)393216000) /* generate 98.304M for I2S */
#define PLL_400M      ((u32)400000000)
#define PLL_500M      ((u32)500000000)
#define PLL_600M      ((u32)600000000)
#define PLL_800M      ((u32)800000000)
#define PLL_960M      ((u32)960000000)
#define PLL_1000M     ((u32)1000000000) /*This is minimum value*/

#define IS_USB_PLL_RANGE(x) ((x) >= PLL_600M && (x) <= PLL_1000M)
#define IS_SYS_PLL_RANGE(x) ((x) <= PLL_500M)
/**
  * @}
  */

#define CLKDIV(x)				          (x)

/** @defgroup Soc_CLK_LIMIT_definitions
  * @{
  */

#define IS_USB_PLL					0
#define IS_SYS_PLL					BIT7
#define GET_CLK_DIV(x)      (x & 0x7f)

/* These limit check and set in bootloader */
#define CLK_LIMIT_0P9V_HP         (266.7  * MHZ_TICK_CNT)  /* cpu clk == sram clk == rom clk*/
#define CLK_LIMIT_1P0V_HP         (333.34 * MHZ_TICK_CNT)
#define CLK_LIMIT_0P9V_SRAM       (266.7  * MHZ_TICK_CNT)
#define CLK_LIMIT_1P0V_SRAM       (333.34 * MHZ_TICK_CNT)
#define CLK_LIMIT_0P9V_SHPERI     (200 * MHZ_TICK_CNT)
#define CLK_LIMIT_1P0V_SHPERI     (240 * MHZ_TICK_CNT)
#define CLK_LIMIT_0P9V_HPERI      (100 * MHZ_TICK_CNT)
#define CLK_LIMIT_1P0V_HPERI      (120 * MHZ_TICK_CNT)
#define CLK_LIMIT_LPERI           (40  * MHZ_TICK_CNT)
#define CLK_LIMIT_PSRAM           (400 * MHZ_TICK_CNT)
#define CLK_LIMIT_PSRAMC_FLASH    (204 * MHZ_TICK_CNT)
#define CLK_LIMIT_SPIC            (204 * MHZ_TICK_CNT)  /* HW SPIC limit is 250MHz */
#define CLK_LIMIT_I2S             (100 * MHZ_TICK_CNT)
#define CLK_LIMIT_VO              (200 * MHZ_TICK_CNT)

/* These limit check and set when user init ip */
#define CLK_LIMIT_GMAC            (50  * MHZ_TICK_CNT)
#define CLK_LIMIT_CAN             (125 * MHZ_TICK_CNT)
#define CLK_LIMIT_QSPI            (100 * MHZ_TICK_CNT)
/**
  * @}
  */

#define SPIC_CS_SETUP_TIME_LMT   (200 * MHZ_TICK_CNT) //CS# Active Setup Time relative to CLK(Some Flash tSLCH min is 5ns)
#define IS_CPU_CLK_DIV_RANGE(x)  ((x) >= CLKDIV(2) && (x) <= CLKDIV(8))

/* Exported functions --------------------------------------------------------*/
/** @defgroup SDM32K_CLK_Exported_Functions SDM32K_CLK Exported Functions
  * @{
  */
_LONG_CALL_ void SDM32K_Enable(void);
_LONG_CALL_ void SDM32K_TimerCalEnable(u32 RTCCalibration);
_LONG_CALL_ void SDM32K_CalFactorModify(u32 newstatus);
/**
  * @}
  */


/** @defgroup CPU_CLK_Exported_Functions CPU_CLK Exported Functions
  * @{
  */

/**
 * @brief  get USB_PLL clk value form rram
 * @retval frequency in Hz
 */
_LONG_CALL_ u32 USB_PLL_ClkGet(void);

/**
 * @brief  get SYS_PLL clk value form rram
 * @retval frequency in Hz
 */
_LONG_CALL_ u32 SYS_PLL_ClkGet(void);

/**
 * @brief  get shperi clk value form rram
 * @retval frequency in Hz
 */
_LONG_CALL_ u32 SHPERI_ClkGet(void);

/**
 * @brief  get hperi clk value form rram
 * @retval frequency in Hz
 */
_LONG_CALL_ u32 HPERI_ClkGet(void);

/**
 * @brief  get psramc clk value form rram
 * @retval frequency in Hz
 */
_LONG_CALL_ u32 PSRAMC_ClkGet(void);

/**
 * @brief  get spic clk value form rram
 * @retval frequency in Hz
 */
_LONG_CALL_ u32 SPIC_ClkGet(void);

/**
 * @brief  get shperi clk value form rram
 * @retval frequency in Hz
 */
_LONG_CALL_ u32 VO_ClkGet(void);

/**
 * @brief  get shperi clk value form rram
 * @retval frequency in Hz
 */
_LONG_CALL_ u32 CPU_ClkGet(void);

/**
 * @brief Obtain the most appropriate clock source and clock division coefficient to achieve the best IP performance
 *
 * @param sys_pll 	system pll clk
 * @param usb_pll 	usb pll clk
 * @param fre_limit limitation of cpu or other peripheral
 * @return The return value is the OR of the clock source and the clock division coefficient
 */
_LONG_CALL_ u8 PLL_ClkSrcGet(const u32 sys_pll, const u32 usb_pll, u32 fre_limit);

/**
  * @}
  */

/** @defgroup XTAL_CLK_Exported_Functions XTAL_CLK Exported Functions
  * @{
  */
_LONG_CALL_ u32 XTAL_ClkGet(void);
_LONG_CALL_ int XTAL_Check_READY(void);
/**
  * @}
  */

/**
  * @brief pll type
  */
#define USB_PLL 0
#define SYS_PLL 1
#define IS_VALID_PLL_TYPE(NAME) (((NAME) == USB_PLL) || ((NAME) == SYS_PLL))

/** @} */

/** @defgroup PLL_Exported_Functions PLL Exported Functions
  * @{
  */

/**
  * @defgroup PLL_Exported_Functions PLL Exported Functions
  */
_LONG_CALL_ void PLL_Enable(u8 pll_name);
_LONG_CALL_ void PLL_Disable(u8 pll_name);
_LONG_CALL_ void PLL_I2S0_CLK(u32 status, u32 clk);
_LONG_CALL_ void PLL_I2S_CLK_DIV(u32 index, u32 status, u32 clk);
_LONG_CALL_ void PLL_SetFreq(u8 pll_name, u32 pll_freq);
_LONG_CALL_ float PLL_I2S_98P304M_ClkTune(u32 pll_sel, float ppm, u32 action);
_LONG_CALL_ float PLL_I2S_45P158M_ClkTune(u32 pll_sel, float ppm, u32 action);


/**
 * @brief  get pll clk from pll regs
 * @param  pll_name: USB_PLL / SYS_PLL
 * @retval frequency in Hz
 */
_LONG_CALL_ u32 PLL_GetFreq(u8 pll_name);
_LONG_CALL_ int CPU_Clk_Check(void);

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
void OSC4M_Init(void);
void OSC4M_R_Set(u32 setbit, u32 clearbit);
void OSC4M_VCM_Set(u32 value);
u32 OSC4M_Calibration(u32 ppm_limit);

u32 OSC131K_Calibration(u32 ppm_limit);
void OSC131_R_Set(u32 setbit, u32 clearbit);
u32 OSC_CalResult_Get(u8 cal_clk);
void XTAL_INIT(void);
void XTAL_AACK(void);

#endif //_AMEBA_CLK_H_