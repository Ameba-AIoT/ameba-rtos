/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSREG_PAD_H_
#define _SYSREG_PAD_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup PAD
  * @brief PAD driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup PAD_Register_Definitions PAD Register Definitions
  * @{
  */

/** @defgroup REG_Px
  * @brief GPIO group control
  * @{
  */
#define PAD_BIT_Px_DIS    ((u32)0x00000001 << 15)          /*!< R/W 0h  PAD shutdown, only when the whole group's dis signals are all 1, the PAD SHDN will pull down to shutdown the group pad. 1. disable pad 0. enable pad*/
#define PAD_BIT_Px_E3     ((u32)0x00000001 << 14)          /*!< R/W 0h  PAD driving ability control. 0: low 1: high  The actual driving current is depend on pad type.*/
#define PAD_BIT_Px_SR     ((u32)0x00000001 << 13)          /*!< R/W 0h  PAD slew rate control. 1: slow 0: fast*/
#define PAD_BIT_Px_SMT    ((u32)0x00000001 << 12)          /*!< R/W 1h  PAD Schmit control*/
#define PAD_BIT_Px_E2     ((u32)0x00000001 << 11)          /*!< R/W 1h  PAD driving ability control. 0: low 1: high  The actual driving current is depend on pad type.*/
#define PAD_BIT_Px_PUPDC  ((u32)0x00000001 << 10)          /*!< R/W 0h  Some pad may have two type of PU/PD resistor, this bit can select it. 1: small resistor 0: big resistor*/
#define PAD_BIT_Px_IE     ((u32)0x00000001 << 8)           /*!< R/W 1h  PAD input enable control*/
#define PAD_MASK_Px_SEL   ((u32)0x000000FF << 0)           /*!< R/W/ES 0h  PAD pinmux function ID selection*/
#define PAD_Px_SEL(x)     (((u32)((x) & 0x000000FF) << 0))
#define PAD_GET_Px_SEL(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup REG_PAD_PU_1
  * @brief
  * @{
  */
#define PAD_MASK_PA_PU   ((u32)0xFFFFFFFF << 0)           /*!< R/W Ch  PAD pull up enable when system is in active.*/
#define PAD_PA_PU(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PA_PU(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_PU_2
  * @brief
  * @{
  */
#define PAD_MASK_PB_PU   ((u32)0xFFFFFFFF << 0)           /*!< R/W 160000h  PAD pull up enable when system is in active.*/
#define PAD_PB_PU(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PB_PU(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_PU_3
  * @brief
  * @{
  */
#define PAD_MASK_PC_PU   ((u32)0xFFFFFFFF << 0)           /*!< R/W 104008h  PAD pull up enable when system is in active.*/
#define PAD_PC_PU(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PC_PU(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_PD_1
  * @brief
  * @{
  */
#define PAD_MASK_PA_PD   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0h  PAD pull down enable when system is in active.*/
#define PAD_PA_PD(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PA_PD(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_PD_2
  * @brief
  * @{
  */
#define PAD_MASK_PB_PD   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0h  PAD pull down enable when system is in active.*/
#define PAD_PB_PD(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PB_PD(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_PD_3
  * @brief
  * @{
  */
#define PAD_MASK_PC_PD   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0h  PAD pull down enable when system is in active.*/
#define PAD_PC_PD(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PC_PD(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_PU_SLP_1
  * @brief
  * @{
  */
#define PAD_MASK_PA_PU_SLP   ((u32)0xFFFFFFFF << 0)           /*!< R/W Ch  PAD pull up enable when system is in sleep.*/
#define PAD_PA_PU_SLP(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PA_PU_SLP(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_PU_SLP_2
  * @brief
  * @{
  */
#define PAD_MASK_PB_PU_SLP   ((u32)0xFFFFFFFF << 0)           /*!< R/W 160000h  PAD pull up enable when system is in sleep.*/
#define PAD_PB_PU_SLP(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PB_PU_SLP(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_PU_SLP_3
  * @brief
  * @{
  */
#define PAD_MASK_PC_PU_SLP   ((u32)0xFFFFFFFF << 0)           /*!< R/W 104008h  PAD pull up enable when system is in sleep.*/
#define PAD_PC_PU_SLP(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PC_PU_SLP(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_PD_SLP_1
  * @brief
  * @{
  */
#define PAD_MASK_PA_PD_SLP   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0h  PAD pull down enable when system is in sleep.*/
#define PAD_PA_PD_SLP(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PA_PD_SLP(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_PD_SLP_2
  * @brief
  * @{
  */
#define PAD_MASK_PB_PD_SLP   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0h  PAD pull down enable when system is in sleep.*/
#define PAD_PB_PD_SLP(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PB_PD_SLP(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_PD_SLP_3
  * @brief
  * @{
  */
#define PAD_MASK_PC_PD_SLP   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0h  PAD pull down enable when system is in sleep.*/
#define PAD_PC_PD_SLP(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PC_PD_SLP(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_AON_CTRL
  * @brief
  * @{
  */
#define PAD_BIT_PUPD_SLP_MODE     ((u32)0x00000001 << 0)           /*!< R/W 0x0  pad pupd sleep mode seletion .  Note:this bit used in pmc , don't change this addr*/
#define PAD_MASK_VPROB_SYSON_OE   ((u32)0x0000000F << 4)           /*!< R/W 0h  vprob source select, msut one-hot [0]: AON_OTP  power [1]: SOC  power  [2]: BT power [3]: RSVD*/
#define PAD_VPROB_SYSON_OE(x)     (((u32)((x) & 0x0000000F) << 4))
#define PAD_GET_VPROB_SYSON_OE(x) ((u32)(((x >> 4) & 0x0000000F)))
#define PAD_MASK_VPROB_EN0        ((u32)0x0000000F << 8)           /*!< R/W 0h  pad vprob function enable*/
#define PAD_VPROB_EN0(x)          (((u32)((x) & 0x0000000F) << 8))
#define PAD_GET_VPROB_EN0(x)      ((u32)(((x >> 8) & 0x0000000F)))
/** @} */

/** @defgroup REG_ADC_TEST_MUX
  * @brief
  * @{
  */
#define PAD_MASK_LP_ADC_CH_NUM   ((u32)0x0000000F << 0)           /*!< R/W 0h  sw select adc test channel*/
#define PAD_LP_ADC_CH_NUM(x)     (((u32)((x) & 0x0000000F) << 0))
#define PAD_GET_LP_ADC_CH_NUM(x) ((u32)(((x >> 0) & 0x0000000F)))
#define PAD_MASK_LP_ADC_MODE     ((u32)0x00000003 << 4)           /*!< R/W 0h  sw_select adc mode*/
#define PAD_LP_ADC_MODE(x)       (((u32)((x) & 0x00000003) << 4))
#define PAD_GET_LP_ADC_MODE(x)   ((u32)(((x >> 4) & 0x00000003)))
#define PAD_BIT_LP_ADC_FORCE     ((u32)0x00000001 << 8)           /*!< R/W 0h  force sw select instead of pinmux input when adc test mode 1:using [5:0] for adc channel / mode select 0:using io input for adc channel / mode*/
/** @} */

/** @defgroup REG_I2S_CTRL
  * @brief
  * @{
  */
#define PAD_BIT_SP1_DIO3_MUXSEL ((u32)0x00000001 << 7) /*!< R/W 0x0  SPORT1 DIO3 function mux 0: DIN_3 func 1: DOUT_0 func*/
#define PAD_BIT_SP1_DIO2_MUXSEL ((u32)0x00000001 << 6) /*!< R/W 0x0  SPORT1 DIO2 function mux 0: DIN_2 func 1: DOUT_1 func*/
#define PAD_BIT_SP1_DIO1_MUXSEL ((u32)0x00000001 << 5) /*!< R/W 0x0  SPORT1 DIO1 function mux 0: DIN_1 func 1: DOUT_2 func*/
#define PAD_BIT_SP1_DIO0_MUXSEL ((u32)0x00000001 << 4) /*!< R/W 0x0  SPORT1 DIO0 function mux 0: DIN_0 func 1: DOUT_3 func*/
#define PAD_BIT_SP0_DIO3_MUXSEL ((u32)0x00000001 << 3) /*!< R/W 0x0  SPORT0 DIO3 function mux 0: DIN_3 func 1: DOUT_0 func*/
#define PAD_BIT_SP0_DIO2_MUXSEL ((u32)0x00000001 << 2) /*!< R/W 0x0  SPORT0 DIO2 function mux 0: DIN_2 func 1: DOUT_1 func*/
#define PAD_BIT_SP0_DIO1_MUXSEL ((u32)0x00000001 << 1) /*!< R/W 0x0  SPORT0 DIO1 function mux 0: DIN_1 func 1: DOUT_2 func*/
#define PAD_BIT_SP0_DIO0_MUXSEL ((u32)0x00000001 << 0) /*!< R/W 0x0  SPORT0 DIO0 function mux 0: DIN_0 func 1: DOUT_3 func*/
/** @} */

/** @defgroup REG_DBG_PORT_CTRL
  * @brief
  * @{
  */
#define PAD_BIT_DBG_PORT_EN     ((u32)0x00000001 << 31)          /*!< R/W 0h  */
#define PAD_BIT_DBG_PORT_EN2    ((u32)0x00000001 << 30)          /*!< R/WA0 0h  */
#define PAD_MASK_DBG_PORT_SEL   ((u32)0x000000FF << 0)           /*!< R/W 0h  */
#define PAD_DBG_PORT_SEL(x)     (((u32)((x) & 0x000000FF) << 0))
#define PAD_GET_DBG_PORT_SEL(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup REG_DBG_GPIO_MAP_0
  * @brief
  * @{
  */
#define PAD_MASK_GPIO_DMUX_SEL3   ((u32)0x0000001F << 24)           /*!< R/W 3h  */
#define PAD_GPIO_DMUX_SEL3(x)     (((u32)((x) & 0x0000001F) << 24))
#define PAD_GET_GPIO_DMUX_SEL3(x) ((u32)(((x >> 24) & 0x0000001F)))
#define PAD_MASK_GPIO_DMUX_SEL2   ((u32)0x0000001F << 16)           /*!< R/W 2h  */
#define PAD_GPIO_DMUX_SEL2(x)     (((u32)((x) & 0x0000001F) << 16))
#define PAD_GET_GPIO_DMUX_SEL2(x) ((u32)(((x >> 16) & 0x0000001F)))
#define PAD_MASK_GPIO_DMUX_SEL1   ((u32)0x0000001F << 8)            /*!< R/W 1h  */
#define PAD_GPIO_DMUX_SEL1(x)     (((u32)((x) & 0x0000001F) << 8))
#define PAD_GET_GPIO_DMUX_SEL1(x) ((u32)(((x >> 8) & 0x0000001F)))
#define PAD_MASK_GPIO_DMUX_SEL0   ((u32)0x0000001F << 0)            /*!< R/W 0h  */
#define PAD_GPIO_DMUX_SEL0(x)     (((u32)((x) & 0x0000001F) << 0))
#define PAD_GET_GPIO_DMUX_SEL0(x) ((u32)(((x >> 0) & 0x0000001F)))
/** @} */

/** @defgroup REG_DBG_GPIO_MAP_1
  * @brief
  * @{
  */
#define PAD_MASK_GPIO_DMUX_SEL7   ((u32)0x0000001F << 24)           /*!< R/W 7h  */
#define PAD_GPIO_DMUX_SEL7(x)     (((u32)((x) & 0x0000001F) << 24))
#define PAD_GET_GPIO_DMUX_SEL7(x) ((u32)(((x >> 24) & 0x0000001F)))
#define PAD_MASK_GPIO_DMUX_SEL6   ((u32)0x0000001F << 16)           /*!< R/W 6h  */
#define PAD_GPIO_DMUX_SEL6(x)     (((u32)((x) & 0x0000001F) << 16))
#define PAD_GET_GPIO_DMUX_SEL6(x) ((u32)(((x >> 16) & 0x0000001F)))
#define PAD_MASK_GPIO_DMUX_SEL5   ((u32)0x0000001F << 8)            /*!< R/W 5h  */
#define PAD_GPIO_DMUX_SEL5(x)     (((u32)((x) & 0x0000001F) << 8))
#define PAD_GET_GPIO_DMUX_SEL5(x) ((u32)(((x >> 8) & 0x0000001F)))
#define PAD_MASK_GPIO_DMUX_SEL4   ((u32)0x0000001F << 0)            /*!< R/W 4h  */
#define PAD_GPIO_DMUX_SEL4(x)     (((u32)((x) & 0x0000001F) << 0))
#define PAD_GET_GPIO_DMUX_SEL4(x) ((u32)(((x >> 0) & 0x0000001F)))
/** @} */

/** @defgroup REG_PINMUX_SUB_CTRL
  * @brief
  * @{
  */
#define PAD_BIT_DBG_CLK_FORCE   ((u32)0x00000001 << 8)           /*!< R/W 0h  Force debug clock divider enable if testmode in clock debug ICFG. This maybe used when EXT_CLK_OUT was used.*/
#define PAD_MASK_DBG_CLK1_SEL   ((u32)0x0000000F << 4)           /*!< R/W 0h  debug clock1  in CLOCK testmode  0: xtal 40M 1: xtal 20M 2: xtal 2M 3: xtal lps 312.5K 4: sdm_32.768k 5: sdm_1Hz 6.usb pll div 7.sys pll div 8: osc131k 9: osc4M 10: osc100K 11: RMII gmac 50M*/
#define PAD_DBG_CLK1_SEL(x)     (((u32)((x) & 0x0000000F) << 4))
#define PAD_GET_DBG_CLK1_SEL(x) ((u32)(((x >> 4) & 0x0000000F)))
#define PAD_MASK_DBG_CLK0_SEL   ((u32)0x0000000F << 0)           /*!< R/W 0h  debug clock1  in CLOCK testmode  0: xtal 40M 1: xtal 20M 2: xtal 2M 3: xtal lps 312.5K 4: sdm_32.768k 5: sdm_1Hz 6.usb pll div 7.sys pll div 8: osc131k 9: osc4M 10: osc100K 11: RMII gmac 50M*/
#define PAD_DBG_CLK0_SEL(x)     (((u32)((x) & 0x0000000F) << 0))
#define PAD_GET_DBG_CLK0_SEL(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup REG_SWD_SDD_CTRL
  * @brief
  * @{
  */
#define PAD_BIT_KSCAN_PUPD_CTRL ((u32)0x00000001 << 8) /*!< R/W 0x1  enable kscan pupd hw control logic, active 1*/
#define PAD_BIT_KM4_SWD_SEL     ((u32)0x00000001 << 3) /*!< R/W 0x1  Toggle this bit will switch shared SWD port, this bit only accessed by KM4. Default is KM4 SWD, if km4 toggle this bit , SWD port will switch to KR4*/
#define PAD_BIT_SWD_PMUX_EN     ((u32)0x00000001 << 0) /*!< R/W 0x1  1: Force enable share SWD pinmux  function 0: Disable*/
/** @} */

/*==========PAD Register Address Definition==========*/
#define REG_Px                       0x000
#define REG_PAD_PU_1                 0x180
#define REG_PAD_PU_2                 0x184
#define REG_PAD_PU_3                 0x188
#define REG_PAD_PD_1                 0x190
#define REG_PAD_PD_2                 0x194
#define REG_PAD_PD_3                 0x198
#define REG_PAD_PU_SLP_1             0x1A0
#define REG_PAD_PU_SLP_2             0x1A4
#define REG_PAD_PU_SLP_3             0x1A8
#define REG_PAD_PD_SLP_1             0x1B0
#define REG_PAD_PD_SLP_2             0x1B4
#define REG_PAD_PD_SLP_3             0x1B8
#define REG_PAD_AON_CTRL             0x1BC
#define REG_ADC_TEST_MUX             0x1D0
#define REG_I2S_CTRL                 0x1E0
#define REG_DBG_PORT_CTRL            0x1E4
#define REG_DBG_GPIO_MAP_0           0x1E8
#define REG_DBG_GPIO_MAP_1           0x1EC
#define REG_PINMUX_SUB_CTRL          0x1F0
#define REG_SWD_SDD_CTRL             0x1F4
#define REG_PAD_REG_DUMMY_0          0x1FC

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your defination here

/* MANUAL_GEN_END */

#endif

/** @} */

/** @} */