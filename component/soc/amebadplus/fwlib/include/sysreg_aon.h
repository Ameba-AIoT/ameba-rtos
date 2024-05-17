/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSREG_AON_H_
#define _SYSREG_AON_H_

/* AUTO_GEN_START */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup AON
* @brief AON driver modules
* @{
*/

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup AON_Register_Definitions AON Register Definitions
 * @{
 *****************************************************************************/

/** @defgroup REG_AON_PWC
 * @brief
 * @{
 **/
#define AON_BIT_LPPAD_SHDN_CORE            ((u32)0x00000001 << 26)          /*!<R/W 1  1: Release LP PAD SHDN CORE (except G5) 0: Global Assert LP PAD RSTB33 Note:G5 is aon pad, when all LP pad's IO power is ready , aon pmc will release this bit , only in power on init seq . */
#define AON_BIT_LPPAD_RSTB                 ((u32)0x00000001 << 25)          /*!<R/W 0  1: Release LP PAD RSTB33 (except G5) 0: Global Assert LP PAD RSTB33 Note: aon pmc will auto release this bit in power on init seq */
#define AON_BIT_LPPAD_SHDN                 ((u32)0x00000001 << 24)          /*!<R/W 0  1: Global Enable LP PAD (except G5) 0: Shutdown LP PAD Note: aon pmc will auto release this bit in power on init seq */
#define AON_BIT_G5_PAD_RSTB                ((u32)0x00000001 << 21)          /*!<R/W 0  1: Enable G5 group pad 0: Reset PAD Note: This is AON pad group, direct connect to G5 pad, aon pmc will auto release this bit in power on init seq */
#define AON_BIT_G4_PAD_RSTB                ((u32)0x00000001 << 20)          /*!<R/W 1  1: Enable G4 group pad 0: Reset PAD */
#define AON_BIT_G3_PAD_RSTB                ((u32)0x00000001 << 19)          /*!<R/W 1  1: Enable G3 group pad 0: Reset PAD */
#define AON_BIT_G2_PAD_RSTB                ((u32)0x00000001 << 18)          /*!<R/W 1  1: Enable G2 group pad 0: Reset PAD */
#define AON_BIT_G1_PAD_RSTB                ((u32)0x00000001 << 17)          /*!<R/W 1  1: Enable G1 group pad 0: Reset PAD */
#define AON_BIT_G0_PAD_RSTB                ((u32)0x00000001 << 16)          /*!<R/W 1  1: Enable G0 group pad 0: Reset PAD */
#define AON_BIT_G5_PAD_SHDN                ((u32)0x00000001 << 13)          /*!<R/W 0  1: Enable G5 group pad 0: Shutdown PAD Note: This is AON pad group, aon pmc will auto release this bit in power on init seq */
#define AON_BIT_G4_PAD_SHDN                ((u32)0x00000001 << 12)          /*!<R/W 1  1: Enable G4 group pad 0: Shutdown PAD */
#define AON_BIT_G3_PAD_SHDN                ((u32)0x00000001 << 11)          /*!<R/W 1  1: Enable G3 group pad 0: Shutdown PAD */
#define AON_BIT_G2_PAD_SHDN                ((u32)0x00000001 << 10)          /*!<R/W 1  1: Enable G2 group pad 0: Shutdown PAD */
#define AON_BIT_G1_PAD_SHDN                ((u32)0x00000001 << 9)          /*!<R/W 1  1: Enable G1 group pad 0: Shutdown PAD */
#define AON_BIT_G0_PAD_SHDN                ((u32)0x00000001 << 8)          /*!<R/W 1  1: Enable G0 group pad 0: Shutdown PAD */
#define AON_MASK_PC_STEP                   ((u32)0x00000003 << 4)          /*!<R/W 2'd1  power cut enable time step. 0: not valid 1: 10us 2: 20us 3: 30us */
#define AON_PC_STEP(x)                     (((u32)((x) & 0x00000003) << 4))
#define AON_GET_PC_STEP(x)                 ((u32)(((x >> 4) & 0x00000003)))
#define AON_BIT_PSW_OUT_AON_OTP            ((u32)0x00000001 << 3)          /*!<R 0  aon otp domain power switch status 1: power on 0: power off */
#define AON_BIT_PWC_AON_OTP                ((u32)0x00000001 << 1)          /*!<R/W 0  OTP power cut enable for core power, high active. 1:enable 0:disable */
/** @} */

/** @defgroup REG_AON_ISO
 * @brief
 * @{
 **/
#define AON_BIT_G5_PAD_SHDN_CORE           ((u32)0x00000001 << 13)          /*!<R/W 1  1: Enable G5 group pad 0: Shutdown PAD Note: This maybe nouse , for AON pad , IO power always on before core power. */
#define AON_BIT_G4_PAD_SHDN_CORE           ((u32)0x00000001 << 12)          /*!<R/W 1  1: Enable G4 group pad 0: Shutdown PAD */
#define AON_BIT_G3_PAD_SHDN_CORE           ((u32)0x00000001 << 11)          /*!<R/W 1  1: Enable G3 group pad 0: Shutdown PAD */
#define AON_BIT_G2_PAD_SHDN_CORE           ((u32)0x00000001 << 10)          /*!<R/W 1  1: Enable G2 group pad 0: Shutdown PAD */
#define AON_BIT_G1_PAD_SHDN_CORE           ((u32)0x00000001 << 9)          /*!<R/W 0  1: Enable G1 group pad 0: Shutdown PAD Note:default is shutdown, wait pmc enable MLDO power */
#define AON_BIT_G0_PAD_SHDN_CORE           ((u32)0x00000001 << 8)          /*!<R/W 0  1: Enable G0 group pad 0: Shutdown PAD */
#define AON_BIT_ISO_R2A                    ((u32)0x00000001 << 2)          /*!<R/W 1  1: ISO signals from RTC to AON 0: Release */
#define AON_BIT_ISO_AON_OTP                ((u32)0x00000001 << 1)          /*!<R/W 1  1: ISO signals from otp core power domain 0: Release */
#define AON_BIT_ISO_LPON                   ((u32)0x00000001 << 0)          /*!<R/W 1  When LDO core power on, PD_LPON is on. 1: ISO signal of LPON power domain 0: Release */
/** @} */

/** @defgroup REG_AON_FEN
 * @brief
 * @{
 **/
#define APBPeriph_LPON                     ((u32)(3  << 30) | (0x00000001 << 16))  /*!<R/W 0  LS global reset 1: Enable LS function 0: Disable */
#define APBPeriph_OTPC                     ((u32)(3  << 30) | (0x00000001 << 7))   /*!<R/W 0  1: Enable OTP function 0: Disable */
#define APBPeriph_SDM                      ((u32)(3  << 30) | (0x00000001 << 4))   /*!<R/W 0  1: Enable 32.768K SDM function 0: Disable */
#define APBPeriph_RTC                      ((u32)(3  << 30) | (0x00000001 << 3))   /*!<R/W 0  1: Enable RTC function 0: Disable */
#define APBPeriph_ATIM                     ((u32)(3  << 30) | (0x00000001 << 2))   /*!<R/W 0  1: Enable AON timer 0: Reset AON timer Note: AON timer starts to run only when event happens. */
#define APBPeriph_IWDG                     ((u32)(3  << 30) | (0x00000001 << 1))   /*!<R/WE 0  1: Enable IWDG by HW autoload. Once enabled, IWDG can't be disabled unless por asserts. */
#define APBPeriph_BOR                      ((u32)(3  << 30) | (0x00000001 << 0))   /*!<R/W 0  Enable bor reset function, default is 0 when power on */
/** @} */

/** @defgroup REG_AON_CLK
 * @brief
 * @{
 **/
#define APBPeriph_OTPC_CLOCK               ((u32)(3  << 30) | (0x00000001 << 7))   /*!<R/W 1  1: Enable OTPC clock 0: Disable Note: default change to 1 for osc4m req */
#define APBPeriph_SDM_CLOCK                ((u32)(3  << 30) | (0x00000001 << 4))   /*!<R/W 0  1: Enable 32.768K SDM module's clock 0: Disable */
#define APBPeriph_RTC_CLOCK                ((u32)(3  << 30) | (0x00000001 << 3))   /*!<R/W 0  1: Enable RTC clock 0: Disable */
#define APBPeriph_ATIM_CLOCK               ((u32)(3  << 30) | (0x00000001 << 2))   /*!<R/W 1  1: Enable AON timer clock 0: Disable */
#define APBPeriph_AON_CLOCK                ((u32)(3  << 30) | (0x00000001 << 0))   /*!<R/W 1  if 0, AON clock will be gated except atim and debouce circuit. Note: AON clock can't be shutdown as analog need. */
/** @} */

/** @defgroup REG_AON_CKSL
 * @brief
 * @{
 **/
#define AON_BIT_OTPC_FLG                   ((u32)0x00000001 << 0)          /*!<R/W 0h  This field is used to indicate OTP autoload field 1: ls 0:aon Note:because aon autoload only do once , so after power on , this bit will always be 1 . SW don’t change this bit . */
/** @} */

/** @defgroup REG_AON_BOOT_REASON_HW
 * @brief
 * @{
 **/
#define AON_BIT_RSTF_OCP                   ((u32)0x00000001 << 10)          /*!<RW1CB 0h  This bit will be set to 1 when OCP happen, it will let system enter dslp mode. Write 1 to clear. */
#define AON_BIT_RSTF_WARM_KM02PERI         ((u32)0x00000001 << 9)          /*!<RW1CB 0h  This bit will be set to 1 when KM0 warm resets the system. Write 1 to clear. */
#define AON_BIT_RSTF_WARM_KM42PERI         ((u32)0x00000001 << 8)          /*!<RW1CB 0h  This bit will be set to 1 when km4 warm resets the system. Write 1 to clear. */
#define AON_BIT_RSTF_WDG2                  ((u32)0x00000001 << 7)          /*!<RW1CB 0h  This bit will be set to 1 when WDG resets the system. Write 1 to clear. */
#define AON_BIT_RSTF_WDG1                  ((u32)0x00000001 << 6)          /*!<RW1CB 0h  This bit will be set to 1 when WDG resets the system. Write 1 to clear. */
#define AON_BIT_RSTF_WDG0                  ((u32)0x00000001 << 5)          /*!<RW1CB 0h  This bit will be set to 1 when WDG resets the system. Write 1 to clear. */
#define AON_BIT_RSTF_IWDG                  ((u32)0x00000001 << 4)          /*!<RW1CB 0h  This bit will be set to 1 when WDG resets the system. Write 1 to clear. */
#define AON_BIT_RSTF_KM0_SYS               ((u32)0x00000001 << 3)          /*!<RW1C 0h  This bit will be set to 1 when KM0 resets the system. Write 1 to clear. */
#define AON_BIT_RSTF_KM4_SYS               ((u32)0x00000001 << 2)          /*!<RW1C 0h  This bit will be set to 1 when KM4 resets the system. Write 1 to clear. */
#define AON_BIT_RSTF_DSLP                  ((u32)0x00000001 << 1)          /*!<RW1C 0h  This is boot reason for dslp exit, clear by SW write to 1. */
#define AON_BIT_RSTF_BOR                   ((u32)0x00000001 << 0)          /*!<RW1C 0h  This bit will be set to 1 when BOR resets the system. Write 1 to clear. */
/** @} */

/** @defgroup REG_AON_AON_WAK_EVT
 * @brief
 * @{
 **/
#define AON_BIT_RTC_ISR_EVT                ((u32)0x00000001 << 8)          /*!<R 0  RTC interrupt wake event occurs. */
#define AON_BIT_GPIO_PIN1_WAKDET_EVT       ((u32)0x00000001 << 5)          /*!<R 0h  Wake event for wake pin1 1: wake event detected 0: no wake event Note: aon gpio only used to wake hw pmc, to interrupt SW, it must enable GPIO interrupt in LP GPIO IP, and the poly must be same as AON GPIO setting. */
#define AON_BIT_GPIO_PIN0_WAKDET_EVT       ((u32)0x00000001 << 4)          /*!<R 0h  Wake event for wake pin0 1: wake event detected 0: no wake event Note: aon gpio only used to wake hw pmc, to interrupt SW, it must enable GPIO interrupt in LP GPIO IP, and the poly must be same as AON GPIO setting. */
#define AON_BIT_TIM_ISR_EVT                ((u32)0x00000001 << 3)          /*!<R 0  AON timer interrupt event occurs. */
#define AON_BIT_BOR_ISR_EVT                ((u32)0x00000001 << 2)          /*!<R 0  bor interrupt event occurs. */
#define AON_BIT_CHIPEN_LP_ISR_EVT          ((u32)0x00000001 << 1)          /*!<R 0  Chipen long press interrupt event occurs. */
#define AON_BIT_CHIPEN_SP_ISR_EVT          ((u32)0x00000001 << 0)          /*!<R 0  Chipen short press interrupt event occurs. */
/** @} */

/** @defgroup REG_AON_TESTING_CFG
 * @brief
 * @{
 **/
#define AON_MASK_ICFG_SW                   ((u32)0x0000000F << 24)          /*!<R/W 4'd2  icfg configured by SIC/SW */
#define AON_ICFG_SW(x)                     (((u32)((x) & 0x0000000F) << 24))
#define AON_GET_ICFG_SW(x)                 ((u32)(((x >> 24) & 0x0000000F)))
#define AON_BIT_ICFG_SEL                   ((u32)0x00000001 << 20)          /*!<R/W 0  If 1, using icfg_sw to replace trapped icfg value. */
#define AON_BIT_IBOOT_SW                   ((u32)0x00000001 << 17)          /*!<R/W 0  iboot configured by SIC/SW */
#define AON_BIT_IBOOT_SEL                  ((u32)0x00000001 << 16)          /*!<R/W 0  If 1, using iboot_sw to replace trapped iboot value. */
/** @} */

/** @defgroup REG_AON_TIM_CNT
 * @brief
 * @{
 **/
#define AON_MASK_TIM_CNT_CUR               ((u32)0xFFFFFFFF << 0)          /*!<R 0h  Aon timer current count value. */
#define AON_TIM_CNT_CUR(x)                 (((u32)((x) & 0xFFFFFFFF) << 0))
#define AON_GET_TIM_CNT_CUR(x)             ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_AON_TIM_SET
 * @brief
 * @{
 **/
#define AON_MASK_TIM_CNT_SET               ((u32)0xFFFFFFFF << 0)          /*!<R/WPD 0h  Set aon time a countdown value, when timer count down, it will trigger a aon_tim event. */
#define AON_TIM_CNT_SET(x)                 (((u32)((x) & 0xFFFFFFFF) << 0))
#define AON_GET_TIM_CNT_SET(x)             ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_AON_TIM_IER
 * @brief
 * @{
 **/
#define AON_BIT_TIM_IE                     ((u32)0x00000001 << 0)          /*!<R/W 0h  Timer interrupt enable 0:disable 1:enable */
/** @} */

/** @defgroup REG_AON_TIM_ISR
 * @brief
 * @{
 **/
#define AON_BIT_TIM_ISR                    ((u32)0x00000001 << 0)          /*!<RW1C 0h  Timer interrupt status 0:no interrupt 1:interrupt occurs */
/** @} */

/** @defgroup REG_AON_GPIO_CTRL
 * @brief
 * @{
 **/
#define AON_MASK_GPIO_WAKDET_POLY          ((u32)0x00000003 << 24)          /*!<R/W 0h  Wake pin polarity 0: low active 1: high active bit1: wake pin1 polarity bit0: wake pin0 polarity Note: once a wake pin is set, the pads PU/PD will be force by this poly value, and soft config have no use. */
#define AON_GPIO_WAKDET_POLY(x)            (((u32)((x) & 0x00000003) << 24))
#define AON_GET_GPIO_WAKDET_POLY(x)        ((u32)(((x >> 24) & 0x00000003)))
#define AON_MASK_GPIO_DBNC_CYC             ((u32)0x00007FFF << 8)          /*!<R/W 100h  The GPIO signal will be filtered by the number of debounce cycles of specified in this register. (100k/128k) */
#define AON_GPIO_DBNC_CYC(x)               (((u32)((x) & 0x00007FFF) << 8))
#define AON_GET_GPIO_DBNC_CYC(x)           ((u32)(((x >> 8) & 0x00007FFF)))
#define AON_BIT_GPIO_DBNC_FEN              ((u32)0x00000001 << 7)          /*!<R/W 0  1: Enable gpio debouncer function 0: Disable Default disable for simualtion issue */
#define AON_MASK_GPIO_WAKE_FEN             ((u32)0x00000003 << 0)          /*!<R/W 0  1: Enable wake pin function 0: Disable Every GPIO has its own enable. When aon pmc is not in DSLP, wake pins will not effect PMC state althrougth this bit is set. */
#define AON_GPIO_WAKE_FEN(x)               (((u32)((x) & 0x00000003) << 0))
#define AON_GET_GPIO_WAKE_FEN(x)           ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup REG_AON_GPIO_WAKE_ISR
 * @brief
 * @{
 **/
#define AON_BIT_GPIO_PIN1_WAKE_ISR         ((u32)0x00000001 << 1)          /*!<RW1CB 0  Wake pin1 interrupt status 0:no interrupt 1:interrupt occurs */
#define AON_BIT_GPIO_PIN0_WAKE_ISR         ((u32)0x00000001 << 0)          /*!<RW1CB 0  Wake pin0 interrupt status 0:no interrupt 1:interrupt occurs */
/** @} */

/** @defgroup REG_AON_RTC_IO_CTRL
 * @brief
 * @{
 **/
#define AON_MASK_RTC_IO_OPT                ((u32)0x00000003 << 8)          /*!<R/W 0  RTC IO option register [0]: extend diode [1]: rsvd */
#define AON_RTC_IO_OPT(x)                  (((u32)((x) & 0x00000003) << 8))
#define AON_GET_RTC_IO_OPT(x)              ((u32)(((x >> 8) & 0x00000003)))
#define AON_BIT_RTC_MODE                   ((u32)0x00000001 << 4)          /*!<R 0  RTC IO mode flag 1: RTC IO domain enabled 0: disable */
#define AON_MASK_RTC_IO_CTRL               ((u32)0x00000003 << 0)          /*!<R/WPD 0  RTC IO control mode 0: shift disable 1: shift enable 2: rtc mode (once enable , it can't be disabled) 3: iwdg feed */
#define AON_RTC_IO_CTRL(x)                 (((u32)((x) & 0x00000003) << 0))
#define AON_GET_RTC_IO_CTRL(x)             ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup REG_AON_IWDG_IO_DIS
 * @brief
 * @{
 **/
#define AON_BIT_IWDG_IO_DIS                ((u32)0x00000001 << 0)          /*!<R/WE 1  IWDG IO disable . Default enabled in RTC domain . This bit is protected , and only can be accessed by OTP */
/** @} */

/** @defgroup REG_AON_CHIPEN_CTRL
 * @brief
 * @{
 **/
#define AON_BIT_CHIPEN_ST                  ((u32)0x00000001 << 18)          /*!<R 0  CHIP_EN pin status 0: logic '0' on CHIP_EN pin 1: logic '1' on CHIP_EN pin */
#define AON_MASK_CHIPEN_INTR_MODE          ((u32)0x00000003 << 16)          /*!<R/W/ES 0  Chipen operation mode. 00: reset mode. When chipen pad low, hw will reset all chip 01: interrup mode. It will enable short/long press function. 1X: pulse reset mode. The transation from other value to 2'd2 will gen a posedge to set RST_PULSE_EN in HV domain. Once in pulse reset mode , this field can't be modified again , and always read out 2'b11 */
#define AON_CHIPEN_INTR_MODE(x)            (((u32)((x) & 0x00000003) << 16))
#define AON_GET_CHIPEN_INTR_MODE(x)        ((u32)(((x >> 16) & 0x00000003)))
#define AON_MASK_CHIPEN_TACK               ((u32)0x00000003 << 12)          /*!<R/W 0  00: 50ms 01: 100ms 10: 200ms 11: 400ms */
#define AON_CHIPEN_TACK(x)                 (((u32)((x) & 0x00000003) << 12))
#define AON_GET_CHIPEN_TACK(x)             ((u32)(((x >> 12) & 0x00000003)))
#define AON_MASK_CHIPEN_TLP                ((u32)0x00000007 << 8)          /*!<R/W 0  000: 1000ms 001: 1500ms 010: 2000ms 011: 2500ms 100: 3000ms 101: 3500ms 110: 4000ms 111: 4500ms */
#define AON_CHIPEN_TLP(x)                  (((u32)((x) & 0x00000007) << 8))
#define AON_GET_CHIPEN_TLP(x)              ((u32)(((x >> 8) & 0x00000007)))
#define AON_MASK_CHIPEN_TSP                ((u32)0x00000007 << 4)          /*!<R/W 0  000: 0ms 001: 50ms 010: 100ms 011: 150ms 100: 200ms 101: 250ms 110: 300ms 111: 350ms */
#define AON_CHIPEN_TSP(x)                  (((u32)((x) & 0x00000007) << 4))
#define AON_GET_CHIPEN_TSP(x)              ((u32)(((x >> 4) & 0x00000007)))
#define AON_MASK_CHIPEN_TDBC               ((u32)0x00000007 << 0)          /*!<R/W 3'd1  000: 0us 001: 100us 010: 500us 011: 1000us 100: 2000us 101: 4000us 110: 8000us 111: 16000us */
#define AON_CHIPEN_TDBC(x)                 (((u32)((x) & 0x00000007) << 0))
#define AON_GET_CHIPEN_TDBC(x)             ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_AON_CHIPEN_ISR
 * @brief
 * @{
 **/
#define AON_BIT_CHIPEN_LP_ISR              ((u32)0x00000001 << 1)          /*!<RW1C 0  chipen long press interrupt status 0:no interrupt 1:interrupt occurs */
#define AON_BIT_CHIPEN_SP_ISR              ((u32)0x00000001 << 0)          /*!<RW1C 0  chipen short press interrupt status 0:no interrupt 1:interrupt occurs */
/** @} */

/** @defgroup REG_AON_BOR_CTRL
 * @brief
 * @{
 **/
#define AON_BIT_BOR_INTR_MODE              ((u32)0x00000001 << 16)          /*!<R/W 0  BOR operation mode. 0: reset mode. When analog detect bor, hw will trigger a bor reset. 1: interrup mode. It will raise an interrup first. If this bit set to 0, then trigger a bor reset. */
#define AON_MASK_BOR_TDBC                  ((u32)0x00000007 << 0)          /*!<R/W 3'd1  000: 0us 001: 100us 010: 500us 011: 1000us 100: 2000us 101: 4000us 110: 8000us 111: 16000us */
#define AON_BOR_TDBC(x)                    (((u32)((x) & 0x00000007) << 0))
#define AON_GET_BOR_TDBC(x)                ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_AON_BOR_ISR
 * @brief
 * @{
 **/
#define AON_BIT_BOR_ISR                    ((u32)0x00000001 << 0)          /*!<RW1C 0  BOR interrupt status 0:no interrupt 1:interrupt occurs */
/** @} */

/** @defgroup REG_AON_PMC_CTRL
 * @brief
 * @{
 **/
#define AON_MASK_PMC_STL_CLDO              ((u32)0x00000003 << 4)          /*!<R/W 2h  wait time for core LDO stable 0: 8*T (T = 10us) 1: 16*T 2: 32*T 3: 64*T */
#define AON_PMC_STL_CLDO(x)                (((u32)((x) & 0x00000003) << 4))
#define AON_GET_PMC_STL_CLDO(x)            ((u32)(((x >> 4) & 0x00000003)))
#define AON_MASK_PMC_STL_SWR               ((u32)0x00000003 << 2)          /*!<R/W 2h  wait time for SWR stable 0: 64*T (T = 10us) 1: 128*T 2: 256*T 3: 512*T */
#define AON_PMC_STL_SWR(x)                 (((u32)((x) & 0x00000003) << 2))
#define AON_GET_PMC_STL_SWR(x)             ((u32)(((x >> 2) & 0x00000003)))
/** @} */

/** @defgroup REG_AON_PMC_OPT
 * @brief
 * @{
 **/
#define AON_BIT_ADC_XTAL_MODE              ((u32)0x00000001 << 6)          /*!<R/W 1  Select xtal mode when xtal used for adc/ctc/comp. 1: Normal 0: LPS */
#define AON_BIT_SNOOZE_PDCK                ((u32)0x00000001 << 5)          /*!<R/W 1  Select xtal auto pdck function for sdm calibration use xtal lps mode 1:Enable 0:Disable */
#define AON_BIT_SDM_XTAL_MODE              ((u32)0x00000001 << 4)          /*!<R/W 1  Select xtal mode when xtal used for sdm calibration. 1: Normal 0: LPS */
#define AON_BIT_PST_WAIT_XTAL              ((u32)0x00000001 << 3)          /*!<R/W 0  1: When POF , polling xtal invalid signal and then run forward 0: not polling xtal invalid */
#define AON_BIT_PST_DSLP_ERCK              ((u32)0x00000001 << 1)          /*!<R/W 0  1: Enable 100K clock for AON register access when PMC enter deep-sleep mode 0: Disable */
#define AON_BIT_PST_DSLP_LDHM              ((u32)0x00000001 << 0)          /*!<R/W 0  1: Enable AON LDO enter into sleep mode when PMC enter deep-sleep mode 0: Disable */
/** @} */

/** @defgroup REG_AON_OTP_POWUP_CFG
 * @brief
 * @{
 **/
#define AON_BIT_OTP_POWUP_SEL              ((u32)0x00000001 << 4)          /*!<R/W 0  select otp pow up flow is inside control or outside control 0: inside 1: outside */
#define AON_MASK_OTP_POWUP_CFG             ((u32)0x0000000F << 0)          /*!<R/W 0  [0] : penvddr2 [1]: pldo [2]: pdstb [3]: ptrim */
#define AON_OTP_POWUP_CFG(x)               (((u32)((x) & 0x0000000F) << 0))
#define AON_GET_OTP_POWUP_CFG(x)           ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_AON_BOOT_CFG
 * @brief
 * @{
 **/
#define AON_MASK_RSVD_SW0                  ((u32)0x7FFFFFFF << 1)          /*!<R/W 0   */
#define AON_RSVD_SW0(x)                    (((u32)((x) & 0x7FFFFFFF) << 1))
#define AON_GET_RSVD_SW0(x)                ((u32)(((x >> 1) & 0x7FFFFFFF)))
#define AON_BIT_DSLP_RETENTION_RAM_PATCH   ((u32)0x00000001 << 0)          /*!<R/W 0  Deepsleep retention RAM patch */
/** @} */

/** @defgroup REG_AON_RSVD_FOR_SW1
 * @brief
 * @{
 **/
#define AON_BIT_WIFI_INIC_NP_READY         ((u32)0x00000001 << 31)          /*!<R/W 0  Wi-Fi NP initialization ready flag */
#define AON_BIT_WIFI_RF_1833_SEL           ((u32)0x00000001 << 30)          /*!<R/W 0  Indication of RF voltage selection 0: 1.8V 1: 3.3V */
#define AON_MASK_RSVD_SW1                  ((u32)0x3FFFFFFF << 0)          /*!<R/W 0   */
#define AON_RSVD_SW1(x)                    (((u32)((x) & 0x3FFFFFFF) << 0))
#define AON_GET_RSVD_SW1(x)                ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */

/** @defgroup REG_AON_PMC_PATCH_GRP0_1
 * @brief
 * @{
 **/
#define AON_MASK_INSTR1_G0_3_0             ((u32)0x0000000F << 28)          /*!<R/W 0h  Instruction1 of AON PMC patch group0 */
#define AON_INSTR1_G0_3_0(x)               (((u32)((x) & 0x0000000F) << 28))
#define AON_GET_INSTR1_G0_3_0(x)           ((u32)(((x >> 28) & 0x0000000F)))
#define AON_MASK_INSTR0_G0                 ((u32)0x0003FFFF << 10)          /*!<R/W 0h  Instruction0 of AON PMC patch group0 */
#define AON_INSTR0_G0(x)                   (((u32)((x) & 0x0003FFFF) << 10))
#define AON_GET_INSTR0_G0(x)               ((u32)(((x >> 10) & 0x0003FFFF)))
#define AON_MASK_SADDR_G0                  ((u32)0x000001FF << 1)          /*!<R/W 0h  Start address of the first instruction of AON PMC patch group0 */
#define AON_SADDR_G0(x)                    (((u32)((x) & 0x000001FF) << 1))
#define AON_GET_SADDR_G0(x)                ((u32)(((x >> 1) & 0x000001FF)))
#define AON_BIT_SEL_PMC_G0                 ((u32)0x00000001 << 0)          /*!<R/W 0h  Select PMC for this patch group 0:aon pmc 1:regu pmc */
/** @} */

/** @defgroup REG_AON_PMC_PATCH_GRP0_2
 * @brief
 * @{
 **/
#define AON_MASK_INSTR2_G0_17_0            ((u32)0x0003FFFF << 14)          /*!<R/W 0h  Instruction2 of AON PMC patch group0 */
#define AON_INSTR2_G0_17_0(x)              (((u32)((x) & 0x0003FFFF) << 14))
#define AON_GET_INSTR2_G0_17_0(x)          ((u32)(((x >> 14) & 0x0003FFFF)))
#define AON_MASK_INSTR1_G0_17_4            ((u32)0x00003FFF << 0)          /*!<R/W 0h  Instruction1 of AON PMC patch group0 */
#define AON_INSTR1_G0_17_4(x)              (((u32)((x) & 0x00003FFF) << 0))
#define AON_GET_INSTR1_G0_17_4(x)          ((u32)(((x >> 0) & 0x00003FFF)))
/** @} */

/** @defgroup REG_AON_PMC_PATCH_GRP1_1
 * @brief
 * @{
 **/
#define AON_MASK_INSTR1_G1_3_0             ((u32)0x0000000F << 28)          /*!<R/W 0h  Instruction1 of AON PMC patch group1 */
#define AON_INSTR1_G1_3_0(x)               (((u32)((x) & 0x0000000F) << 28))
#define AON_GET_INSTR1_G1_3_0(x)           ((u32)(((x >> 28) & 0x0000000F)))
#define AON_MASK_INSTR0_G1                 ((u32)0x0003FFFF << 10)          /*!<R/W 0h  Instruction0 of AON PMC patch group1 */
#define AON_INSTR0_G1(x)                   (((u32)((x) & 0x0003FFFF) << 10))
#define AON_GET_INSTR0_G1(x)               ((u32)(((x >> 10) & 0x0003FFFF)))
#define AON_MASK_SADDR_G1                  ((u32)0x000001FF << 1)          /*!<R/W 0h  Start address of the first instruction of AON PMC patch group0 */
#define AON_SADDR_G1(x)                    (((u32)((x) & 0x000001FF) << 1))
#define AON_GET_SADDR_G1(x)                ((u32)(((x >> 1) & 0x000001FF)))
#define AON_BIT_SEL_PMC_G1                 ((u32)0x00000001 << 0)          /*!<R/W 0h  Select PMC for this patch group 0:aon pmc 1:regu pmc */
/** @} */

/** @defgroup REG_AON_PMC_PATCH_GRP1_2
 * @brief
 * @{
 **/
#define AON_MASK_INSTR2_G1_17_0            ((u32)0x0003FFFF << 14)          /*!<R/W 0h  Instruction2 of AON PMC patch group1 */
#define AON_INSTR2_G1_17_0(x)              (((u32)((x) & 0x0003FFFF) << 14))
#define AON_GET_INSTR2_G1_17_0(x)          ((u32)(((x >> 14) & 0x0003FFFF)))
#define AON_MASK_INSTR1_G1_17_4            ((u32)0x00003FFF << 0)          /*!<R/W 0h  Instruction1 of AON PMC patch group1 */
#define AON_INSTR1_G1_17_4(x)              (((u32)((x) & 0x00003FFF) << 0))
#define AON_GET_INSTR1_G1_17_4(x)          ((u32)(((x >> 0) & 0x00003FFF)))
/** @} */

/** @defgroup REG_AON_PMC_PATCH_GRP2_1
 * @brief
 * @{
 **/
#define AON_MASK_INSTR1_G2_3_0             ((u32)0x0000000F << 28)          /*!<R/W 0h  Instruction1 of AON PMC patch group2 */
#define AON_INSTR1_G2_3_0(x)               (((u32)((x) & 0x0000000F) << 28))
#define AON_GET_INSTR1_G2_3_0(x)           ((u32)(((x >> 28) & 0x0000000F)))
#define AON_MASK_INSTR0_G2                 ((u32)0x0003FFFF << 10)          /*!<R/W 0h  Instruction0 of AON PMC patch group2 */
#define AON_INSTR0_G2(x)                   (((u32)((x) & 0x0003FFFF) << 10))
#define AON_GET_INSTR0_G2(x)               ((u32)(((x >> 10) & 0x0003FFFF)))
#define AON_MASK_SADDR_G2                  ((u32)0x000001FF << 1)          /*!<R/W 0h  Start address of the first instruction of AON PMC patch group0 */
#define AON_SADDR_G2(x)                    (((u32)((x) & 0x000001FF) << 1))
#define AON_GET_SADDR_G2(x)                ((u32)(((x >> 1) & 0x000001FF)))
#define AON_BIT_SEL_PMC_G2                 ((u32)0x00000001 << 0)          /*!<R/W 0h  Select PMC for this patch group 0:aon pmc 1:regu pmc */
/** @} */

/** @defgroup REG_AON_PMC_PATCH_GRP2_2
 * @brief
 * @{
 **/
#define AON_MASK_INSTR2_G2_17_0            ((u32)0x0003FFFF << 14)          /*!<R/W 0h  Instruction2 of AON PMC patch group2 */
#define AON_INSTR2_G2_17_0(x)              (((u32)((x) & 0x0003FFFF) << 14))
#define AON_GET_INSTR2_G2_17_0(x)          ((u32)(((x >> 14) & 0x0003FFFF)))
#define AON_MASK_INSTR1_G2_17_4            ((u32)0x00003FFF << 0)          /*!<R/W 0h  Instruction1 of AON PMC patch group2 */
#define AON_INSTR1_G2_17_4(x)              (((u32)((x) & 0x00003FFF) << 0))
#define AON_GET_INSTR1_G2_17_4(x)          ((u32)(((x >> 0) & 0x00003FFF)))
/** @} */

/** @defgroup REG_AON_PMC_PATCH_GRP3_1
 * @brief
 * @{
 **/
#define AON_MASK_INSTR1_G3_3_0             ((u32)0x0000000F << 28)          /*!<R/W 0h  Instruction1 of AON PMC patch group3 */
#define AON_INSTR1_G3_3_0(x)               (((u32)((x) & 0x0000000F) << 28))
#define AON_GET_INSTR1_G3_3_0(x)           ((u32)(((x >> 28) & 0x0000000F)))
#define AON_MASK_INSTR0_G3                 ((u32)0x0003FFFF << 10)          /*!<R/W 0h  Instruction0 of AON PMC patch group3 */
#define AON_INSTR0_G3(x)                   (((u32)((x) & 0x0003FFFF) << 10))
#define AON_GET_INSTR0_G3(x)               ((u32)(((x >> 10) & 0x0003FFFF)))
#define AON_MASK_SADDR_G3                  ((u32)0x000001FF << 1)          /*!<R/W 0h  Start address of the first instruction of AON PMC patch group0 */
#define AON_SADDR_G3(x)                    (((u32)((x) & 0x000001FF) << 1))
#define AON_GET_SADDR_G3(x)                ((u32)(((x >> 1) & 0x000001FF)))
#define AON_BIT_SEL_PMC_G3                 ((u32)0x00000001 << 0)          /*!<R/W 0h  Select PMC for this patch group 0:aon pmc 1:regu pmc */
/** @} */

/** @defgroup REG_AON_PMC_PATCH_GRP3_2
 * @brief
 * @{
 **/
#define AON_MASK_INSTR2_G3_17_0            ((u32)0x0003FFFF << 14)          /*!<R/W 0h  Instruction2 of AON PMC patch group3 */
#define AON_INSTR2_G3_17_0(x)              (((u32)((x) & 0x0003FFFF) << 14))
#define AON_GET_INSTR2_G3_17_0(x)          ((u32)(((x >> 14) & 0x0003FFFF)))
#define AON_MASK_INSTR1_G3_17_4            ((u32)0x00003FFF << 0)          /*!<R/W 0h  Instruction1 of AON PMC patch group3 */
#define AON_INSTR1_G3_17_4(x)              (((u32)((x) & 0x00003FFF) << 0))
#define AON_GET_INSTR1_G3_17_4(x)          ((u32)(((x >> 0) & 0x00003FFF)))
/** @} */

/** @defgroup REG_AON_PMC_PATCH_GRP4_1
 * @brief
 * @{
 **/
#define AON_MASK_INSTR1_G4_3_0             ((u32)0x0000000F << 28)          /*!<R/W 0h  Instruction1 of AON PMC patch group4 */
#define AON_INSTR1_G4_3_0(x)               (((u32)((x) & 0x0000000F) << 28))
#define AON_GET_INSTR1_G4_3_0(x)           ((u32)(((x >> 28) & 0x0000000F)))
#define AON_MASK_INSTR0_G4                 ((u32)0x0003FFFF << 10)          /*!<R/W 0h  Instruction0 of AON PMC patch group4 */
#define AON_INSTR0_G4(x)                   (((u32)((x) & 0x0003FFFF) << 10))
#define AON_GET_INSTR0_G4(x)               ((u32)(((x >> 10) & 0x0003FFFF)))
#define AON_MASK_SADDR_G4                  ((u32)0x000001FF << 1)          /*!<R/W 0h  Start address of the first instruction of AON PMC patch group0 */
#define AON_SADDR_G4(x)                    (((u32)((x) & 0x000001FF) << 1))
#define AON_GET_SADDR_G4(x)                ((u32)(((x >> 1) & 0x000001FF)))
#define AON_BIT_SEL_PMC_G4                 ((u32)0x00000001 << 0)          /*!<R/W 0h  Select PMC for this patch group 0:aon pmc 1:regu pmc */
/** @} */

/** @defgroup REG_AON_PMC_PATCH_GRP4_2
 * @brief
 * @{
 **/
#define AON_MASK_INSTR2_G4_17_0            ((u32)0x0003FFFF << 14)          /*!<R/W 0h  Instruction2 of AON PMC patch group4 */
#define AON_INSTR2_G4_17_0(x)              (((u32)((x) & 0x0003FFFF) << 14))
#define AON_GET_INSTR2_G4_17_0(x)          ((u32)(((x >> 14) & 0x0003FFFF)))
#define AON_MASK_INSTR1_G4_17_4            ((u32)0x00003FFF << 0)          /*!<R/W 0h  Instruction1 of AON PMC patch group4 */
#define AON_INSTR1_G4_17_4(x)              (((u32)((x) & 0x00003FFF) << 0))
#define AON_GET_INSTR1_G4_17_4(x)          ((u32)(((x >> 0) & 0x00003FFF)))
/** @} */

/** @defgroup REG_AON_PMC_PATCH_GRP5_1
 * @brief
 * @{
 **/
#define AON_MASK_INSTR1_G5_3_0             ((u32)0x0000000F << 28)          /*!<R/W 0h  Instruction1 of AON PMC patch group5 */
#define AON_INSTR1_G5_3_0(x)               (((u32)((x) & 0x0000000F) << 28))
#define AON_GET_INSTR1_G5_3_0(x)           ((u32)(((x >> 28) & 0x0000000F)))
#define AON_MASK_INSTR0_G5                 ((u32)0x0003FFFF << 10)          /*!<R/W 0h  Instruction0 of AON PMC patch group5 */
#define AON_INSTR0_G5(x)                   (((u32)((x) & 0x0003FFFF) << 10))
#define AON_GET_INSTR0_G5(x)               ((u32)(((x >> 10) & 0x0003FFFF)))
#define AON_MASK_SADDR_G5                  ((u32)0x000001FF << 1)          /*!<R/W 0h  Start address of the first instruction of AON PMC patch group0 */
#define AON_SADDR_G5(x)                    (((u32)((x) & 0x000001FF) << 1))
#define AON_GET_SADDR_G5(x)                ((u32)(((x >> 1) & 0x000001FF)))
#define AON_BIT_SEL_PMC_G5                 ((u32)0x00000001 << 0)          /*!<R/W 0h  Select PMC for this patch group 0:aon pmc 1:regu pmc */
/** @} */

/** @defgroup REG_AON_PMC_PATCH_GRP5_2
 * @brief
 * @{
 **/
#define AON_MASK_INSTR2_G5_17_0            ((u32)0x0003FFFF << 14)          /*!<R/W 0h  HW used only , these are for pmc patch Note: regu reg 0xF0 must not be used */
#define AON_INSTR2_G5_17_0(x)              (((u32)((x) & 0x0003FFFF) << 14))
#define AON_GET_INSTR2_G5_17_0(x)          ((u32)(((x >> 14) & 0x0003FFFF)))
#define AON_MASK_INSTR1_G5_17_4            ((u32)0x00003FFF << 0)          /*!<R/W 0h  HW used only , these are for pmc patch */
#define AON_INSTR1_G5_17_4(x)              (((u32)((x) & 0x00003FFF) << 0))
#define AON_GET_INSTR1_G5_17_4(x)          ((u32)(((x >> 0) & 0x00003FFF)))
/** @} */

/** @defgroup REG_AON_PMC_ADDR01
 * @brief
 * @{
 **/
#define AON_MASK_PMC_PATCH_ADDR0           ((u32)0x00000FFF << 0)          /*!<R/W 12'hF0  HW used only , these are for pmc patch */
#define AON_PMC_PATCH_ADDR0(x)             (((u32)((x) & 0x00000FFF) << 0))
#define AON_GET_PMC_PATCH_ADDR0(x)         ((u32)(((x >> 0) & 0x00000FFF)))
#define AON_MASK_PMC_PATCH_ADDR1           ((u32)0x00000FFF << 16)          /*!<R/W 12'hF0  HW used only , these are for pmc patch */
#define AON_PMC_PATCH_ADDR1(x)             (((u32)((x) & 0x00000FFF) << 16))
#define AON_GET_PMC_PATCH_ADDR1(x)         ((u32)(((x >> 16) & 0x00000FFF)))
/** @} */

/** @defgroup REG_AON_PMC_ADDR23
 * @brief
 * @{
 **/
#define AON_MASK_PMC_PATCH_ADDR2           ((u32)0x00000FFF << 0)          /*!<R/W 12'hF0  HW used only , these are for pmc patch */
#define AON_PMC_PATCH_ADDR2(x)             (((u32)((x) & 0x00000FFF) << 0))
#define AON_GET_PMC_PATCH_ADDR2(x)         ((u32)(((x >> 0) & 0x00000FFF)))
#define AON_MASK_PMC_PATCH_ADDR3           ((u32)0x00000FFF << 16)          /*!<R/W 12'hF0  HW used only , these are for pmc patch */
#define AON_PMC_PATCH_ADDR3(x)             (((u32)((x) & 0x00000FFF) << 16))
#define AON_GET_PMC_PATCH_ADDR3(x)         ((u32)(((x >> 16) & 0x00000FFF)))
/** @} */

/** @defgroup REG_AON_PMC_ADDR45
 * @brief
 * @{
 **/
#define AON_MASK_PMC_PATCH_ADDR4           ((u32)0x00000FFF << 0)          /*!<R/W 12'hF0  HW used only , these are for pmc patch */
#define AON_PMC_PATCH_ADDR4(x)             (((u32)((x) & 0x00000FFF) << 0))
#define AON_GET_PMC_PATCH_ADDR4(x)         ((u32)(((x >> 0) & 0x00000FFF)))
#define AON_MASK_PMC_PATCH_ADDR5           ((u32)0x00000FFF << 16)          /*!<R/W 12'hF0  HW used only , these are for pmc patch */
#define AON_PMC_PATCH_ADDR5(x)             (((u32)((x) & 0x00000FFF) << 16))
#define AON_GET_PMC_PATCH_ADDR5(x)         ((u32)(((x >> 16) & 0x00000FFF)))
/** @} */

/** @defgroup REG_AON_PMC_ADDR67
 * @brief
 * @{
 **/
#define AON_MASK_PMC_PATCH_ADDR6           ((u32)0x00000FFF << 0)          /*!<R/W 12'hF0  HW used only , these are for pmc patch */
#define AON_PMC_PATCH_ADDR6(x)             (((u32)((x) & 0x00000FFF) << 0))
#define AON_GET_PMC_PATCH_ADDR6(x)         ((u32)(((x >> 0) & 0x00000FFF)))
#define AON_MASK_PMC_PATCH_ADDR7           ((u32)0x00000FFF << 16)          /*!<R/W 12'hF0  HW used only , these are for pmc patch */
#define AON_PMC_PATCH_ADDR7(x)             (((u32)((x) & 0x00000FFF) << 16))
#define AON_GET_PMC_PATCH_ADDR7(x)         ((u32)(((x >> 16) & 0x00000FFF)))
/** @} */

/** @defgroup REG_AON_RTC_IO_SOUT0
 * @brief
 * @{
 **/
#define AON_MASK_RTC_IO_TEST_DOUT_31_0     ((u32)0xFFFFFFFF << 0)          /*!<R 0  Note , those bits are shared for RTC domain test used as shift in pattern */
#define AON_RTC_IO_TEST_DOUT_31_0(x)       (((u32)((x) & 0xFFFFFFFF) << 0))
#define AON_GET_RTC_IO_TEST_DOUT_31_0(x)   ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_AON_RTC_IO_SOUT1
 * @brief
 * @{
 **/
#define AON_MASK_RTC_IO_TEST_DOUT_63_32    ((u32)0xFFFFFFFF << 0)          /*!<R 0   */
#define AON_RTC_IO_TEST_DOUT_63_32(x)      (((u32)((x) & 0xFFFFFFFF) << 0))
#define AON_GET_RTC_IO_TEST_DOUT_63_32(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_AON_RTC_IO_SOUT2
 * @brief
 * @{
 **/
#define AON_MASK_RTC_IO_TEST_DOUT_95_64    ((u32)0xFFFFFFFF << 0)          /*!<R 0   */
#define AON_RTC_IO_TEST_DOUT_95_64(x)      (((u32)((x) & 0xFFFFFFFF) << 0))
#define AON_GET_RTC_IO_TEST_DOUT_95_64(x)  ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_AON_RTC_IO_SOUT3
 * @brief
 * @{
 **/
#define AON_MASK_RTC_IO_TEST_DOUT_127_96   ((u32)0xFFFFFFFF << 0)          /*!<R 0   */
#define AON_RTC_IO_TEST_DOUT_127_96(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define AON_GET_RTC_IO_TEST_DOUT_127_96(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_AON_SYSRST_MSK
 * @brief
 * @{
 **/
#define AON_BIT_SYSRST_MSK_LPON            ((u32)0x00000001 << 16)          /*!<R/W 1  LS global reset 1: mask LS reset when sys reset happen 0: reset by sys reset */
#define AON_BIT_SYSRST_MSK_OTP             ((u32)0x00000001 << 7)          /*!<R/W 0  1: mask OTP reset when sys reset happen 0: reset by sys reset */
#define AON_BIT_SYSRST_MSK_SDM32K          ((u32)0x00000001 << 4)          /*!<R/W 1  1: mask 32.768K SDM reset when sys reset happen 0: reset by sys reset */
#define AON_BIT_SYSRST_MSK_RTC             ((u32)0x00000001 << 3)          /*!<R/W 1  1: mask RTC reset when sys reset happen 0: reset by sys reset */
#define AON_BIT_SYSRST_MSK_ATIM            ((u32)0x00000001 << 2)          /*!<R/W 0  1: mask AON timer 0: reset by sys reset OR warm reset Note: AON timer starts to run only when event happens. */
#define AON_BIT_SYSRST_MSK_IWDG            ((u32)0x00000001 << 1)          /*!<R/W 0  1: mask AON IWDG reset 0: reset by sys reset OR warm reset */
/** @} */

/** @defgroup REG_AON_AON_BACKUP0
 * @brief
 * @{
 **/
#define AON_MASK_BK0                       ((u32)0xFFFFFFFF << 0)          /*!<R/W 0  Note , those bits are shared for RTC domain test used as shift in pattern */
#define AON_BK0(x)                         (((u32)((x) & 0xFFFFFFFF) << 0))
#define AON_GET_BK0(x)                     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_AON_AON_BACKUP1
 * @brief
 * @{
 **/
#define AON_MASK_BK1                       ((u32)0xFFFFFFFF << 0)          /*!<R/W 0   */
#define AON_BK1(x)                         (((u32)((x) & 0xFFFFFFFF) << 0))
#define AON_GET_BK1(x)                     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_AON_AON_BACKUP2
 * @brief
 * @{
 **/
#define AON_MASK_BK2                       ((u32)0xFFFFFFFF << 0)          /*!<R/W 0   */
#define AON_BK2(x)                         (((u32)((x) & 0xFFFFFFFF) << 0))
#define AON_GET_BK2(x)                     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_AON_AON_BACKUP3
 * @brief
 * @{
 **/
#define AON_MASK_BK3                       ((u32)0xFFFFFFFF << 0)          /*!<R/W 0   */
#define AON_BK3(x)                         (((u32)((x) & 0xFFFFFFFF) << 0))
#define AON_GET_BK3(x)                     ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_AON_DUMMY_AIP_CTRL0
 * @brief
 * @{
 **/
#define AON_MASK_AIP_CTRL_RSVD0            ((u32)0x000000FF << 0)          /*!<R/W 8'h0F  reserved for aip control */
#define AON_AIP_CTRL_RSVD0(x)              (((u32)((x) & 0x000000FF) << 0))
#define AON_GET_AIP_CTRL_RSVD0(x)          ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup REG_AON_DUMMY_AIP_INFO0
 * @brief
 * @{
 **/
#define AON_MASK_AIP_INFO_RSVD0            ((u32)0x000000FF << 0)          /*!<R 0  reserved for aip info indication */
#define AON_AIP_INFO_RSVD0(x)              (((u32)((x) & 0x000000FF) << 0))
#define AON_GET_AIP_INFO_RSVD0(x)          ((u32)(((x >> 0) & 0x000000FF)))
/** @} */


/*==========AON Register Address Definition==========*/
#define REG_AON_PWC                                  0x0000
#define REG_AON_ISO                                  0x0004
#define REG_AON_FEN                                  0x0008
#define REG_AON_CLK                                  0x000C
#define REG_AON_CKSL                                 0x0010
#define REG_AON_BOOT_REASON_HW                       0x0020
#define REG_AON_AON_WAK_EVT                          0x0024
#define REG_AON_TESTING_CFG                          0x002C
#define REG_AON_TIM_CNT                              0x0030
#define REG_AON_TIM_SET                              0x0034
#define REG_AON_TIM_IER                              0x0038
#define REG_AON_TIM_ISR                              0x003C
#define REG_AON_GPIO_CTRL                            0x0040
#define REG_AON_GPIO_WAKE_ISR                        0x0044
#define REG_AON_RTC_IO_CTRL                          0x0048
#define REG_AON_IWDG_IO_DIS                          0x004C
#define REG_AON_CHIPEN_CTRL                          0x0050
#define REG_AON_CHIPEN_ISR                           0x0054
#define REG_AON_BOR_CTRL                             0x0058
#define REG_AON_BOR_ISR                              0x005C
#define REG_AON_PMC_CTRL                             0x0060
#define REG_AON_PMC_OPT                              0x0064
#define REG_AON_OTP_POWUP_CFG                        0x0068
#define REG_AON_BOOT_CFG                             0x0070
#define REG_AON_RSVD_FOR_SW1                         0x0074
#define REG_AON_PMC_PATCH_GRP0_1                     0x0080
#define REG_AON_PMC_PATCH_GRP0_2                     0x0084
#define REG_AON_PMC_PATCH_GRP1_1                     0x0088
#define REG_AON_PMC_PATCH_GRP1_2                     0x008C
#define REG_AON_PMC_PATCH_GRP2_1                     0x0090
#define REG_AON_PMC_PATCH_GRP2_2                     0x0094
#define REG_AON_PMC_PATCH_GRP3_1                     0x0098
#define REG_AON_PMC_PATCH_GRP3_2                     0x009C
#define REG_AON_PMC_PATCH_GRP4_1                     0x00A0
#define REG_AON_PMC_PATCH_GRP4_2                     0x00A4
#define REG_AON_PMC_PATCH_GRP5_1                     0x00A8
#define REG_AON_PMC_PATCH_GRP5_2                     0x00AC
#define REG_AON_PMC_ADDR01                           0x00B0
#define REG_AON_PMC_ADDR23                           0x00B4
#define REG_AON_PMC_ADDR45                           0x00B8
#define REG_AON_PMC_ADDR67                           0x00BC
#define REG_AON_RTC_IO_SOUT0                         0x00C0
#define REG_AON_RTC_IO_SOUT1                         0x00C4
#define REG_AON_RTC_IO_SOUT2                         0x00C8
#define REG_AON_RTC_IO_SOUT3                         0x00CC
#define REG_AON_SYSRST_MSK                           0x00D0
#define REG_AON_AON_BACKUP0                          0x00E0
#define REG_AON_AON_BACKUP1                          0x00E4
#define REG_AON_AON_BACKUP2                          0x00E8
#define REG_AON_AON_BACKUP3                          0x00EC
#define REG_AON_BOUNDARY_CHK                         0x00F0
#define REG_AON_DUMMY_AIP_CTRL0                      0x00F8
#define REG_AON_DUMMY_AIP_INFO0                      0x00FC

/** @} */

/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your defination here
#define REG_AON_WIFI_IPC		(SYSTEM_CTRL_BASE + REG_AON_RSVD_FOR_SW1)
/* MANUAL_GEN_END */

#endif



/** @} */

/** @} */
