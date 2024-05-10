/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSREG_PAD_H_
#define _SYSREG_PAD_H_

/* AUTO_GEN_START */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup PAD
* @brief PAD driver modules
* @{
*/

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup PAD_Register_Definitions PAD Register Definitions
 * @{
 *****************************************************************************/

/** @defgroup REG_Px
 * @brief GPIO group control
 * @{
 **/
#define PAD_BIT_Px_DIS              ((u32)0x00000001 << 15)          /*!<R/W 0h  PAD shutdown, only when the whole group's dis signals are all 1, the PAD SHDN will pull down to shutdown the group pad. 1. disable pad 0. enable pad */
#define PAD_BIT_Px_SR               ((u32)0x00000001 << 13)          /*!<R/W 1h  PAD slew rate control */
#define PAD_BIT_Px_SMT              ((u32)0x00000001 << 12)          /*!<R/W 1h  PAD Schmit control */
#define PAD_BIT_Px_E2               ((u32)0x00000001 << 11)          /*!<R/W 1h  PAD driving ability control. 0: low 1: high The actual driving current is depend on pad type. */
#define PAD_BIT_Px_PUPDC            ((u32)0x00000001 << 10)          /*!<R/W 0h  Some pad may have two type of PU/PD resistor, this bit can select it. 1: small resistor 0: big resistor */
#define PAD_BIT_Px_IE               ((u32)0x00000001 << 8)          /*!<R/W 1h  PAD input enable control */
#define PAD_MASK_Px_SEL             ((u32)0x000000FF << 0)          /*!<R/W/ES 0h  PAD pinmux function ID selection */
#define PAD_Px_SEL(x)               (((u32)((x) & 0x000000FF) << 0))
#define PAD_GET_Px_SEL(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup REG_PAD_PU_1
 * @brief
 * @{
 **/
#define PAD_MASK_PA_PU              ((u32)0xFFFFFFFF << 0)          /*!<R/W 40022h  PAD pull up enable when system is in active. */
#define PAD_PA_PU(x)                (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PA_PU(x)            ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_PU_2
 * @brief
 * @{
 **/
#define PAD_MASK_PB_PU              ((u32)0xFFFFFFFF << 0)          /*!<R/W c0000020h  PAD pull up enable when system is in active. */
#define PAD_PB_PU(x)                (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PB_PU(x)            ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_PD_1
 * @brief
 * @{
 **/
#define PAD_MASK_PA_PD              ((u32)0xFFFFFFFF << 0)          /*!<R/W 0h  PAD pull down enable when system is in active. */
#define PAD_PA_PD(x)                (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PA_PD(x)            ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_PD_2
 * @brief
 * @{
 **/
#define PAD_MASK_PB_PD              ((u32)0xFFFFFFFF << 0)          /*!<R/W 0h  PAD pull down enable when system is in active. */
#define PAD_PB_PD(x)                (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PB_PD(x)            ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_PU_SLP_1
 * @brief
 * @{
 **/
#define PAD_MASK_PA_PU_SLP          ((u32)0xFFFFFFFF << 0)          /*!<R/W 0h  PAD pull up enable when system is in sleep. */
#define PAD_PA_PU_SLP(x)            (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PA_PU_SLP(x)        ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_PU_SLP_2
 * @brief
 * @{
 **/
#define PAD_MASK_PB_PU_SLP          ((u32)0xFFFFFFFF << 0)          /*!<R/W 0h  PAD pull up enable when system is in sleep. */
#define PAD_PB_PU_SLP(x)            (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PB_PU_SLP(x)        ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_PD_SLP_1
 * @brief
 * @{
 **/
#define PAD_MASK_PA_PD_SLP          ((u32)0xFFFFFFFF << 0)          /*!<R/W 0h  PAD pull down enable when system is in sleep. */
#define PAD_PA_PD_SLP(x)            (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PA_PD_SLP(x)        ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_PD_SLP_2
 * @brief
 * @{
 **/
#define PAD_MASK_PB_PD_SLP          ((u32)0xFFFFFFFF << 0)          /*!<R/W 0h  PAD pull down enable when system is in sleep. */
#define PAD_PB_PD_SLP(x)            (((u32)((x) & 0xFFFFFFFF) << 0))
#define PAD_GET_PB_PD_SLP(x)        ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup REG_PAD_REG_DUMMY_1
 * @brief
 * @{
 **/
#define PAD_BIT_PUPD_SLP_MODE       ((u32)0x00000001 << 0)          /*!<R/W 0  pad pupd sleep mode seletion . */
/** @} */

/** @defgroup REG_VOLTAGE_PROBE_OE
 * @brief
 * @{
 **/
#define PAD_MASK_VPROB_SYSON_OE     ((u32)0x0000000F << 0)          /*!<R/W 0h  vprob source select, msut one-hot [0]: AON_OTP power [1]: SOC power [2]: BT power [3]: RSVD */
#define PAD_VPROB_SYSON_OE(x)       (((u32)((x) & 0x0000000F) << 0))
#define PAD_GET_VPROB_SYSON_OE(x)   ((u32)(((x >> 0) & 0x0000000F)))
#define PAD_MASK_VPROB_SOC_OE       ((u32)0x0000000F << 4)          /*!<R/W 0h  dummy bit, no use */
#define PAD_VPROB_SOC_OE(x)         (((u32)((x) & 0x0000000F) << 4))
#define PAD_GET_VPROB_SOC_OE(x)     ((u32)(((x >> 4) & 0x0000000F)))
#define PAD_MASK_VPROB_BT_OE        ((u32)0x00000003 << 8)          /*!<R/W 0h  dummy bit, no use */
#define PAD_VPROB_BT_OE(x)          (((u32)((x) & 0x00000003) << 8))
#define PAD_GET_VPROB_BT_OE(x)      ((u32)(((x >> 8) & 0x00000003)))
#define PAD_BIT_VPROB_EN0           ((u32)0x00000001 << 10)          /*!<R/W 0h  pad vprob function enable */
/** @} */

/** @defgroup REG_I2S_CTRL
 * @brief
 * @{
 **/
#define PAD_BIT_SP1_DIO3_MUXSEL     ((u32)0x00000001 << 7)          /*!<R/W 0  SPORT1 DIO3 function mux 0: DIN_3 func 1: DOUT_0 func */
#define PAD_BIT_SP1_DIO2_MUXSEL     ((u32)0x00000001 << 6)          /*!<R/W 0  SPORT1 DIO2 function mux 0: DIN_2 func 1: DOUT_1 func */
#define PAD_BIT_SP1_DIO1_MUXSEL     ((u32)0x00000001 << 5)          /*!<R/W 0  SPORT1 DIO1 function mux 0: DIN_1 func 1: DOUT_2 func */
#define PAD_BIT_SP1_DIO0_MUXSEL     ((u32)0x00000001 << 4)          /*!<R/W 0  SPORT1 DIO0 function mux 0: DIN_0 func 1: DOUT_3 func */
#define PAD_BIT_SP0_DIO3_MUXSEL     ((u32)0x00000001 << 3)          /*!<R/W 0  SPORT0 DIO3 function mux 0: DIN_3 func 1: DOUT_0 func */
#define PAD_BIT_SP0_DIO2_MUXSEL     ((u32)0x00000001 << 2)          /*!<R/W 0  SPORT0 DIO2 function mux 0: DIN_2 func 1: DOUT_1 func */
#define PAD_BIT_SP0_DIO1_MUXSEL     ((u32)0x00000001 << 1)          /*!<R/W 0  SPORT0 DIO1 function mux 0: DIN_1 func 1: DOUT_2 func */
#define PAD_BIT_SP0_DIO0_MUXSEL     ((u32)0x00000001 << 0)          /*!<R/W 0  SPORT0 DIO0 function mux 0: DIN_0 func 1: DOUT_3 func */
/** @} */

/** @defgroup REG_DBG_PORT_CTRL
 * @brief
 * @{
 **/
#define PAD_BIT_DBG_PORT_EN         ((u32)0x00000001 << 31)          /*!<R/W 0h   */
#define PAD_BIT_DBG_PORT_EN2        ((u32)0x00000001 << 30)          /*!<R/WA0 0h   */
#define PAD_MASK_DBG_PORT_SEL       ((u32)0x000000FF << 0)          /*!<R/W 0h   */
#define PAD_DBG_PORT_SEL(x)         (((u32)((x) & 0x000000FF) << 0))
#define PAD_GET_DBG_PORT_SEL(x)     ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup REG_DBG_GPIO_MAP_0
 * @brief
 * @{
 **/
#define PAD_MASK_GPIO_DMUX_SEL3     ((u32)0x0000001F << 24)          /*!<R/W 3h   */
#define PAD_GPIO_DMUX_SEL3(x)       (((u32)((x) & 0x0000001F) << 24))
#define PAD_GET_GPIO_DMUX_SEL3(x)   ((u32)(((x >> 24) & 0x0000001F)))
#define PAD_MASK_GPIO_DMUX_SEL2     ((u32)0x0000001F << 16)          /*!<R/W 2h   */
#define PAD_GPIO_DMUX_SEL2(x)       (((u32)((x) & 0x0000001F) << 16))
#define PAD_GET_GPIO_DMUX_SEL2(x)   ((u32)(((x >> 16) & 0x0000001F)))
#define PAD_MASK_GPIO_DMUX_SEL1     ((u32)0x0000001F << 8)          /*!<R/W 1h   */
#define PAD_GPIO_DMUX_SEL1(x)       (((u32)((x) & 0x0000001F) << 8))
#define PAD_GET_GPIO_DMUX_SEL1(x)   ((u32)(((x >> 8) & 0x0000001F)))
#define PAD_MASK_GPIO_DMUX_SEL0     ((u32)0x0000001F << 0)          /*!<R/W 0h   */
#define PAD_GPIO_DMUX_SEL0(x)       (((u32)((x) & 0x0000001F) << 0))
#define PAD_GET_GPIO_DMUX_SEL0(x)   ((u32)(((x >> 0) & 0x0000001F)))
/** @} */

/** @defgroup REG_DBG_GPIO_MAP_1
 * @brief
 * @{
 **/
#define PAD_MASK_GPIO_DMUX_SEL7     ((u32)0x0000001F << 24)          /*!<R/W 7h   */
#define PAD_GPIO_DMUX_SEL7(x)       (((u32)((x) & 0x0000001F) << 24))
#define PAD_GET_GPIO_DMUX_SEL7(x)   ((u32)(((x >> 24) & 0x0000001F)))
#define PAD_MASK_GPIO_DMUX_SEL6     ((u32)0x0000001F << 16)          /*!<R/W 6h   */
#define PAD_GPIO_DMUX_SEL6(x)       (((u32)((x) & 0x0000001F) << 16))
#define PAD_GET_GPIO_DMUX_SEL6(x)   ((u32)(((x >> 16) & 0x0000001F)))
#define PAD_MASK_GPIO_DMUX_SEL5     ((u32)0x0000001F << 8)          /*!<R/W 5h   */
#define PAD_GPIO_DMUX_SEL5(x)       (((u32)((x) & 0x0000001F) << 8))
#define PAD_GET_GPIO_DMUX_SEL5(x)   ((u32)(((x >> 8) & 0x0000001F)))
#define PAD_MASK_GPIO_DMUX_SEL4     ((u32)0x0000001F << 0)          /*!<R/W 4h   */
#define PAD_GPIO_DMUX_SEL4(x)       (((u32)((x) & 0x0000001F) << 0))
#define PAD_GET_GPIO_DMUX_SEL4(x)   ((u32)(((x >> 0) & 0x0000001F)))
/** @} */

/** @defgroup REG_PINMUX_SUB_CTRL
 * @brief
 * @{
 **/
#define PAD_MASK_DBG_CLK1_SEL       ((u32)0x00000007 << 4)          /*!<R/W 0h  debug clock1 in CLOCK testmode 0: osc 1: osc100k 2: osc128k 3: xtal 4: sdm_32.768k 5: sdm_256Hz 6: sdm_1Hz 7: zero */
#define PAD_DBG_CLK1_SEL(x)         (((u32)((x) & 0x00000007) << 4))
#define PAD_GET_DBG_CLK1_SEL(x)     ((u32)(((x >> 4) & 0x00000007)))
#define PAD_MASK_DBG_CLK0_SEL       ((u32)0x00000007 << 0)          /*!<R/W 0h  debug clock0 in CLOCK testmode 0: osc 1: osc100k 2: osc128k 3: xtal 4: sdm_32.768k 5: sdm_256Hz 6: sdm_1Hz 7: zero */
#define PAD_DBG_CLK0_SEL(x)         (((u32)((x) & 0x00000007) << 0))
#define PAD_GET_DBG_CLK0_SEL(x)     ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup REG_SWD_SDD_CTRL
 * @brief
 * @{
 **/
#define PAD_BIT_KSCAN_PUPD_CTRL     ((u32)0x00000001 << 8)          /*!<R/W 1  enable kscan pupd hw control logic, active 1 */
#define PAD_BIT_SWD_PMUX_EN         ((u32)0x00000001 << 0)          /*!<R/W 1  1: Force enable share SWD pinmux function 0: Disable */
/** @} */


/*==========PAD Register Address Definition==========*/
#define REG_PAD_PU_1                                 0x0100
#define REG_PAD_PU_2                                 0x0104
#define REG_PAD_PD_1                                 0x0108
#define REG_PAD_PD_2                                 0x010C
#define REG_PAD_PU_SLP_1                             0x0110
#define REG_PAD_PU_SLP_2                             0x0114
#define REG_PAD_PD_SLP_1                             0x0118
#define REG_PAD_PD_SLP_2                             0x011C
#define REG_PAD_REG_DUMMY_1                          0x0120
#define REG_VOLTAGE_PROBE_OE                         0x01DC
#define REG_I2S_CTRL                                 0x01E0
#define REG_DBG_PORT_CTRL                            0x01E4
#define REG_DBG_GPIO_MAP_0                           0x01E8
#define REG_DBG_GPIO_MAP_1                           0x01EC
#define REG_PINMUX_SUB_CTRL                          0x01F0
#define REG_SWD_SDD_CTRL                             0x01F4
#define REG_PAD_REG_DUMMY_0                          0x01FC

/** @} */

/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your defination here

/**************************************************************************//**
 * @defgroup AMEBAD_PINMUX
 * @{
 * @brief AMEBAD_PINMUX Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t REG_GPIOx[64];			/*!< Pad control register */
} PINMUX_TypeDef;
/** @} */




/* MANUAL_GEN_END */

#endif

/** @} */

/** @} */
