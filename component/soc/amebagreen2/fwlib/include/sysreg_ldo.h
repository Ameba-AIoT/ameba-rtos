/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSREG_LDO_H_
#define _SYSREG_LDO_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup LDO
  * @brief LDO driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup LDO_Register_Definitions LDO Register Definitions
  * @{
  */

/** @defgroup LDO_TEST_KEY_REG
  * @brief
  * @{
  */
#define LDO_BIT_POW_SW     ((u32)0x00000001 << 3) /*!< R/W 1'b0  power swr*/
#define LDO_BIT_HW_PDB_0P9 ((u32)0x00000001 << 0) /*!< R/W 1'b1  // 此訊號,務必正確的default value // Digital 送此訊號透過analog levelshift到HW_PDB_33V_OUT // level shift gating to low by POR_AON*/
/** @} */

/** @defgroup LDO_RF_EN_DUMMY
  * @brief
  * @{
  */
#define LDO_BIT_REG_POW_4MOSC ((u32)0x00000001 << 0) /*!< R/W 1'b0  // [RF][4MOSC] dummy*/
/** @} */

/** @defgroup LDO_4M_OSC_CTRL
  * @brief
  * @{
  */
#define LDO_BIT_C_SEL          ((u32)0x00000001 << 7)           /*!< R/W 1'b0  // [RF][4MOSC] REG_CTRL*/
#define LDO_BIT_EN_4MOSC       ((u32)0x00000001 << 6)           /*!< R 1'b0  // [RF][4MOSC] 1'b1 : enable*/
#define LDO_BIT_DMY_0          ((u32)0x00000001 << 5)           /*!< R/W 1'b0  // [RF][4MOSC] REG_CTRL*/
#define LDO_MASK_IB_BIAS_SEL   ((u32)0x0000001F << 0)           /*!< R/W 5'd0  // [RF][4MOSC] REG_CTRL*/
#define LDO_IB_BIAS_SEL(x)     (((u32)((x) & 0x0000001F) << 0))
#define LDO_GET_IB_BIAS_SEL(x) ((u32)(((x >> 0) & 0x0000001F)))
/** @} */

/** @defgroup LDO_4M_OSC_CTRL1
  * @brief
  * @{
  */
#define LDO_MASK_VCM_SEL_H    ((u32)0x0000000F << 12)           /*!< R/W 4'b0111  // [RF][4MOSC] REG_CTRL*/
#define LDO_VCM_SEL_H(x)      (((u32)((x) & 0x0000000F) << 12))
#define LDO_GET_VCM_SEL_H(x)  ((u32)(((x >> 12) & 0x0000000F)))
#define LDO_MASK_VCM_SEL_L    ((u32)0x0000000F << 8)            /*!< R/W 4'b0111  // [RF][4MOSC] REG_CTRL*/
#define LDO_VCM_SEL_L(x)      (((u32)((x) & 0x0000000F) << 8))
#define LDO_GET_VCM_SEL_L(x)  ((u32)(((x >> 8) & 0x0000000F)))
#define LDO_MASK_FREQ_R_SEL   ((u32)0x000000FF << 0)            /*!< R/W 8'b01111111  // [RF][4MOSC] REG_CTRL*/
#define LDO_FREQ_R_SEL(x)     (((u32)((x) & 0x000000FF) << 0))
#define LDO_GET_FREQ_R_SEL(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup LDO_AONLDO
  * @brief
  * @{
  */
#define LDO_MASK_DMY_3            ((u32)0x0000003F << 10)           /*!< R/W 6'b000000  // [REGU][AONLDO] dummy*/
#define LDO_DMY_3(x)              (((u32)((x) & 0x0000003F) << 10))
#define LDO_GET_DMY_3(x)          ((u32)(((x >> 10) & 0x0000003F)))
#define LDO_BIT_DMY_4             ((u32)0x00000001 << 9)            /*!< R/W 1'b0  // [REGU][AONLDO] dummy*/
#define LDO_BIT_EN_SLEEP_L        ((u32)0x00000001 << 8)            /*!< R/W 1'b0  Enable LDO09H sleep mode 0 : disable (default) 1 : enable*/
#define LDO_MASK_AONLDO_VOSEL_H   ((u32)0x0000000F << 4)            /*!< R/W/ES 4'b1010  AONLDO09 voltage select 0000: 0.528V 0.464V 0001: 0.630V 0.554V 0010: 0.659V 0.579V 0011: 0.690V 0.606V 0100: 0.724V 0.636V 0101: 0.742V 0.652V 0110: 0.761V 0.669V 0111: 0.782V 0.687V 1000: 0.803V 0.706V 1001: 0.826V 0.725V 1010: 0.850V 0.746V 1011: 0.875V 0.769V 1100: 0.902V 0.792V 1101: 0.930V 0.817V 1110: 0.960V 0.844V 1111: 0.993V 0.872V*/
#define LDO_AONLDO_VOSEL_H(x)     (((u32)((x) & 0x0000000F) << 4))
#define LDO_GET_AONLDO_VOSEL_H(x) ((u32)(((x >> 4) & 0x0000000F)))
#define LDO_MASK_DMY_5            ((u32)0x00000003 << 2)            /*!< R/W 2'b00  // [REGU][AONLDO] dummy*/
#define LDO_DMY_5(x)              (((u32)((x) & 0x00000003) << 2))
#define LDO_GET_DMY_5(x)          ((u32)(((x >> 2) & 0x00000003)))
#define LDO_BIT_LV_POR_VREF_SEL   ((u32)0x00000001 << 1)            /*!< R/W 1'b0  LV_POR VREF 0 (default) VREF = 0P600V 1 VREF = 0P450V*/
#define LDO_BIT_AONLDO_VREF_SEL   ((u32)0x00000001 << 0)            /*!< R/W 1'b0  AONLDO VREF_SEL 0 (default) VREF = 0P600V 1 VREF = 0P575V*/
/** @} */

/** @defgroup LDO_BOD
  * @brief
  * @{
  */
#define LDO_MASK_DMY_6       ((u32)0x0000003F << 12)           /*!< R/W 6'b000000  // dummy)*/
#define LDO_DMY_6(x)         (((u32)((x) & 0x0000003F) << 12))
#define LDO_GET_DMY_6(x)     ((u32)(((x >> 12) & 0x0000003F)))
#define LDO_BIT_REG_BOD_R_EN ((u32)0x00000001 << 11)           /*!< R/W 1'b1  // 0: disable BOD // 1: enable BOD*/
#define LDO_MASK_REG_BOD_R   ((u32)0x0000001F << 6)            /*!< R/W 5'b01101  00000:3.3333 00001:3.2877 00010:3.2432 00011:3.2000 00100:3.1579 00101:3.1035 00110:3.0508 00111:3.0000 01000:2.9508 01001:2.9032 01010:2.8571 01011:2.8125 01100:2.7692 01101:2.7169 01110:2.6666 01111:2.6182 10000:2.5623 10001:2.5087 10010:2.4574 10011:2.4080 10100:2.3606 10101:2.3151 10110:2.2713 10111:2.2154 11000:2.1621 11001:2.1114 11010:2.0630 11011:2.0168 11100:1.9672 11101:1.9098 11110:1.8556 11111:1.8045*/
#define LDO_REG_BOD_R(x)     (((u32)((x) & 0x0000001F) << 6))
#define LDO_GET_REG_BOD_R(x) ((u32)(((x >> 6) & 0x0000001F)))
#define LDO_BIT_REG_BOD_F_D  ((u32)0x00000001 << 5)            /*!< R/W 1'b0  // dummy*/
#define LDO_MASK_REG_BOD_F   ((u32)0x0000001F << 0)            /*!< R/W 5'b10011  00000:3.3333 00001:3.2877 00010:3.2432 00011:3.2000 00100:3.1579 00101:3.1035 00110:3.0508 00111:3.0000 01000:2.9508 01001:2.9032 01010:2.8571 01011:2.8125 01100:2.7692 01101:2.7169 01110:2.6666 01111:2.6182 10000:2.5623 10001:2.5087 10010:2.4574 10011:2.4080 10100:2.3606 10101:2.3151 10110:2.2713 10111:2.2154 11000:2.1621 11001:2.1114 11010:2.0630 11011:2.0168 11100:1.9672 11101:1.9098 11110:1.8556 11111:1.8045*/
#define LDO_REG_BOD_F(x)     (((u32)((x) & 0x0000001F) << 0))
#define LDO_GET_REG_BOD_F(x) ((u32)(((x >> 0) & 0x0000001F)))
/** @} */

/** @defgroup LDO_3318
  * @brief
  * @{
  */
#define LDO_BIT_PSRR_FILTER              ((u32)0x00000001 << 15)           /*!< R/W 0x1  0: disable PSRR Filter 1: enable PSRR Filter*/
#define LDO_BIT_RSV_DIODE                ((u32)0x00000001 << 14)           /*!< R/W 0x0  0: disable rsv diode 1: enable rsv diode*/
#define LDO_BIT_DIODE                    ((u32)0x00000001 << 13)           /*!< R/W 0x0  0: disable force diode 1: enable force diode*/
#define LDO_BIT_OP_BUFFER                ((u32)0x00000001 << 12)           /*!< R/W 0x1  0: PMOS buffer 1: NMOS buffer*/
#define LDO_MASK_DMY_LOAD                ((u32)0x00000003 << 10)           /*!< R/W 2'b10  00:0mA 01:200kΩ 10:183Ω 11:183Ω||200kΩ Note:this field controlled by syson pmc.  When pmc require dummy on , and this field indicate dummy resident. When pmc require dummy off , this field has no means, analog will see load is 0 When mldo_tmode is assert , pmc controller is not active*/
#define LDO_DMY_LOAD(x)                  (((u32)((x) & 0x00000003) << 10))
#define LDO_GET_DMY_LOAD(x)              ((u32)(((x >> 10) & 0x00000003)))
#define LDO_BIT_REG_ITAIL_HALF_EN_L_3318 ((u32)0x00000001 << 9)            /*!< R/W 0x0  // [REGU][LDO_UART] REG_CTRL // LDO iq half ctrl // 0(default): iq normal (for normal mode) // 1: iq half (for suspend mode)*/
#define LDO_MASK_REG_STANDBY_L_3318      ((u32)0x00000003 << 7)            /*!< R/W 0x0  //11: iq=1.5uA; load=1mA~10mA  (sleep) //10: iq=45uA; load=10mA(Start Up) (normal-0) //00: iq=45uA; load=5mA~160mA (normal-1) Note:this field controlled by syson pmc.  When pmc require mode > 0 , and this field indicate pmc request. When pmc require mode is 0, this field has no use When mldo_tmode is assert , pmc controller is not active*/
#define LDO_REG_STANDBY_L_3318(x)        (((u32)((x) & 0x00000003) << 7))
#define LDO_GET_REG_STANDBY_L_3318(x)    ((u32)(((x >> 7) & 0x00000003)))
#define LDO_BIT_EN_DIODE_L_3318          ((u32)0x00000001 << 6)            /*!< R/W 0x1  // [REGU][LDO_UART] REG_CTRL // PGATE diode clamp (this gating to disable by pd_regu_L) // 0: disable clamp // 1: enable clamp (default)*/
#define LDO_MASK_VOADJ_L_3318            ((u32)0x0000000F << 2)            /*!< R 0x1000  // [REGU][LDO_UART] REG_CTRL // LDO VOSEL // 0000, 0001, 0010, 0011:  1.655, 1.672, 1.690, 1.708 // 0100, 0101, 0110, 0111:  1.727, 1.745, 1.764, 1.784 // 1000, 1001(default), 1010, 1011:  1.804, 1.825, 1.846, 1.868 // 1100, 1101, 1110, 1111:  1.890, 1.912, 1.935, 1.959 Note: the acture write-able addr move to 0x4C[23:20] for otp program easy*/
#define LDO_VOADJ_L_3318(x)              (((u32)((x) & 0x0000000F) << 2))
#define LDO_GET_VOADJ_L_3318(x)          ((u32)(((x >> 2) & 0x0000000F)))
#define LDO_BIT_PD_REGU_L_3318           ((u32)0x00000001 << 1)            /*!< R/W 0x1  // [REGU][LDO_UART] REG_CTRL // PowerDown CTRL // 0: normal-Mode(default) // 1: power-down Mode  Note:  This field is also controlled by syson pmc . When mldo_tmode is assert , pmc controller is not active*/
#define LDO_BIT_EN_PC_BT_L_3318          ((u32)0x00000001 << 0)            /*!< R/W 0x0  // [REGU][LDO] REG_CTRL // PWRCUT enable // 0: LDO-Mode (default) // 1: PWRCUT-Mode*/
/** @} */

/** @defgroup LDO_RFAFE_1209
  * @brief
  * @{
  */
#define LDO_MASK_DMY_7                   ((u32)0x0000001F << 19)           /*!< R/W 5'b00000  Dummy*/
#define LDO_DMY_7(x)                     (((u32)((x) & 0x0000001F) << 19))
#define LDO_GET_DMY_7(x)                 ((u32)(((x >> 19) & 0x0000001F)))
#define LDO_MASK_REG_VREF_SEL_L_1209     ((u32)0x00000003 << 17)           /*!< R/W 2'b0  // [REGU][LDO] REG_CTRL // vref sel // 00: 0.6V // 01: 0.57V // 10: 0.56V // 11: 0.54V*/
#define LDO_REG_VREF_SEL_L_1209(x)       (((u32)((x) & 0x00000003) << 17))
#define LDO_GET_REG_VREF_SEL_L_1209(x)   ((u32)(((x >> 17) & 0x00000003)))
#define LDO_BIT_REG_FB_SEL_L_1209        ((u32)0x00000001 << 16)           /*!< R/W 1'b0  // [REGU][LDO] REG_CTRL // vo feedback select // 0: vo(digital pad) (default) // 1: vo_18(powermos pad)*/
#define LDO_BIT_REG_DMYLOAD_X3_L_1209    ((u32)0x00000001 << 15)           /*!< R/W 1'b0  // [REGU][LDO] REG_CTRL // LDO dmy load ctrl // 0: dmyload_1X(5mA) // 1: dmyload_3X(15mA)*/
#define LDO_BIT_REG_DMYLOAD_X2_L_1209    ((u32)0x00000001 << 14)           /*!< R/W 1'b1  // [REGU][LDO] REG_CTRL // LDO dmy load ctrl // 0: dmyload_1X(5mA) // 1: dmyload_2X(10mA)*/
#define LDO_BIT_REG_PRE_PC_L_1209        ((u32)0x00000001 << 13)           /*!< R/W 1'b0  // [REGU][LDO] REG_CTRL // 0: LDO-Mode (default) // 1:Pre- PWRCUT-Mode*/
#define LDO_BIT_REG_OV_DETECT_L_1209     ((u32)0x00000001 << 12)           /*!< R/W 1'b0  // [REGU][LDO] REG_CTRL // OV detect function control // 0: disable // 1 : enable*/
#define LDO_BIT_REG_DMYLOAD_L_1209       ((u32)0x00000001 << 11)           /*!< R 1'b1  // [REGU][LDO] REG_CTRL // LDO dmy load ctrl // 0: dmy load disable // 1: dmy load enable*/
#define LDO_BIT_REG_LIGHT_DMYLOAD_L_1209 ((u32)0x00000001 << 10)           /*!< R/W 1'b0  // [REGU][LDO] REG_CTRL // LDO dmy load ctrl // 0: dmy load disable // 1:9k Ω (0.1mA)*/
#define LDO_BIT_EN_DIODE_1209            ((u32)0x00000001 << 9)            /*!< R/W 1'b1  // [REGU][LDO] REG_CTRL // PGATE diode clamp (this gating to disable by pd_regu_L) // 1: enable clamp  // 0: disable clamp*/
#define LDO_BIT_REG_SLEEP_L_1209         ((u32)0x00000001 << 8)            /*!< R/W 1'b0  // [REGU][LDO] REG_CTRL // LDO mode control // 0: normal mode // 1 : sleep mode*/
#define LDO_BIT_REG_OV_LVL_L_1209        ((u32)0x00000001 << 7)            /*!< R/W 1'b0  // [REGU][LDO] REG_CTRL // OV vref level // 0: 1.05V // 1 : 1V*/
#define LDO_BIT_SSMODE_L_1209            ((u32)0x00000001 << 6)            /*!< R/W 1'b1  //0:disable ssmode function //1:enable: ssmode function (smaller power mos size)*/
#define LDO_MASK_VOADJ_L_1209            ((u32)0x0000000F << 2)            /*!< R/W/ES 4'b0111  // [REGU][LDO] REG_CTRL // LDO VOSEL // 0000, 0001, 0010, 0011: 0.708, 0.739, 0.772, 0.802 // 0100, 0101, 0110, 0111(default): 0.827, 0.853, 0.877, 0.9 // 1000, 1001, 1010, 1011: 0.924, 0.949, 0.976, 1 // 1100, 1101, 1110, 1111: 1.025, 1.05, 1.075, 1.01*/
#define LDO_VOADJ_L_1209(x)              (((u32)((x) & 0x0000000F) << 2))
#define LDO_GET_VOADJ_L_1209(x)          ((u32)(((x >> 2) & 0x0000000F)))
#define LDO_BIT_PD_REGU_L_1209           ((u32)0x00000001 << 1)            /*!< R/W 1'b1  // [REGU][LDO] REG_CTRL // PowerDown CTRL // 0: normal-Mode // 1: power-down Mode (default)*/
#define LDO_BIT_EN_PC_L_1209             ((u32)0x00000001 << 0)            /*!< R/W 1'b0  // [REGU][LDO] REG_CTRL // PWRCUT enable // 0: LDO-Mode (default) // 1: PWRCUT-Mode*/
/** @} */

/** @defgroup LDO_RTC_CTRL
  * @brief
  * @{
  */
#define LDO_BIT_REG_RTC_EN_POR_HY ((u32)0x00000001 << 6)           /*!< R/W 0x0  */
#define LDO_MASK_REG_RTC_LDO      ((u32)0x00000003 << 4)           /*!< R/W 0x0  */
#define LDO_REG_RTC_LDO(x)        (((u32)((x) & 0x00000003) << 4))
#define LDO_GET_REG_RTC_LDO(x)    ((u32)(((x >> 4) & 0x00000003)))
#define LDO_MASK_REG_RTC_DMY      ((u32)0x0000000F << 0)           /*!< R/W 0x0  */
#define LDO_REG_RTC_DMY(x)        (((u32)((x) & 0x0000000F) << 0))
#define LDO_GET_REG_RTC_DMY(x)    ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup LDO_APMC_STA
  * @brief
  * @{
  */
#define LDO_MASK_CUR_STS_VAL   ((u32)0x00000007 << 16)           /*!< R   current regu status*/
#define LDO_CUR_STS_VAL(x)     (((u32)((x) & 0x00000007) << 16))
#define LDO_GET_CUR_STS_VAL(x) ((u32)(((x >> 16) & 0x00000007)))
#define LDO_MASK_CUR_INSTR     ((u32)0x0000FFFF << 0)            /*!< R   regu pmc current instruction code*/
#define LDO_CUR_INSTR(x)       (((u32)((x) & 0x0000FFFF) << 0))
#define LDO_GET_CUR_INSTR(x)   ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup LDO_APMC_VEC
  * @brief
  * @{
  */
#define LDO_MASK_CUR_VEC   ((u32)0x0000FFFF << 0)           /*!< R   regu pmc all intr vector*/
#define LDO_CUR_VEC(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define LDO_GET_CUR_VEC(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup LDO_APMC_ISR
  * @brief
  * @{
  */
#define LDO_MASK_CUR_ISR   ((u32)0x0000FFFF << 0)           /*!< R   regu pmc current processing isr, one hot*/
#define LDO_CUR_ISR(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define LDO_GET_CUR_ISR(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup LDO_ALDO_SYS_CTRL
  * @brief
  * @{
  */
#define LDO_MASK_VOLT_CODE1_ALDO   ((u32)0x0000000F << 28)           /*!< R/W 4'b1010  Aon LDO code1 , which indicate 0.8v*/
#define LDO_VOLT_CODE1_ALDO(x)     (((u32)((x) & 0x0000000F) << 28))
#define LDO_GET_VOLT_CODE1_ALDO(x) ((u32)(((x >> 28) & 0x0000000F)))
#define LDO_MASK_VOLT_CODE0_ALDO   ((u32)0x0000000F << 24)           /*!< R/W 4'b0100  Aon LDO code0 , which indicate 0.7v*/
#define LDO_VOLT_CODE0_ALDO(x)     (((u32)((x) & 0x0000000F) << 24))
#define LDO_GET_VOLT_CODE0_ALDO(x) ((u32)(((x >> 24) & 0x0000000F)))
#define LDO_MASK_VOLT_CODE0_MLDO   ((u32)0x0000000F << 20)           /*!< R/W 4'b1000  Mem LDO code0 , which indicate 1.8v*/
#define LDO_VOLT_CODE0_MLDO(x)     (((u32)((x) & 0x0000000F) << 20))
#define LDO_GET_VOLT_CODE0_MLDO(x) ((u32)(((x >> 20) & 0x0000000F)))
#define LDO_MASK_VDEC_STEP         ((u32)0x0000000F << 12)           /*!< R/W 4'd2  volt decrease step , unit in 100K cycle, acture value is this value + 1 Note: this is common for all LDO and SWR*/
#define LDO_VDEC_STEP(x)           (((u32)((x) & 0x0000000F) << 12))
#define LDO_GET_VDEC_STEP(x)       ((u32)(((x >> 12) & 0x0000000F)))
#define LDO_MASK_VINC_STEP         ((u32)0x0000000F << 8)            /*!< R/W 4'd2  volt decrease step , unit in 100K cycle, acture value is this value + 1 Note: this is common for all LDO and SWR*/
#define LDO_VINC_STEP(x)           (((u32)((x) & 0x0000000F) << 8))
#define LDO_GET_VINC_STEP(x)       ((u32)(((x >> 8) & 0x0000000F)))
#define LDO_BIT_VOLT_PWM_ABN       ((u32)0x00000001 << 7)            /*!< RW1C 1'b0  pwm volt adject abnormal statue . When happen , it is means current voltage is higher than request in a rising commond, same as falling case.*/
#define LDO_BIT_VOLT_PFM_ABN       ((u32)0x00000001 << 6)            /*!< RW1C 1'b0  same as [7]*/
#define LDO_BIT_VOLT_CLDO_ABN      ((u32)0x00000001 << 5)            /*!< RW1C 1'b0  same as [7]*/
#define LDO_BIT_VOLT_ALDO_ABN      ((u32)0x00000001 << 4)            /*!< RW1C 1'b0  same as [7]*/
/** @} */

/** @defgroup LDO_CLDO_VOLT_CTRL
  * @brief
  * @{
  */
#define LDO_MASK_VOLT_CODE3_CLDO   ((u32)0x0000000F << 12)           /*!< R/W 4'b1011  Core LDO code3 , which indicate 1.0v*/
#define LDO_VOLT_CODE3_CLDO(x)     (((u32)((x) & 0x0000000F) << 12))
#define LDO_GET_VOLT_CODE3_CLDO(x) ((u32)(((x >> 12) & 0x0000000F)))
#define LDO_MASK_VOLT_CODE2_CLDO   ((u32)0x0000000F << 8)            /*!< R/W 4'b0111  Core LDO code2 , which indicate 0.9v*/
#define LDO_VOLT_CODE2_CLDO(x)     (((u32)((x) & 0x0000000F) << 8))
#define LDO_GET_VOLT_CODE2_CLDO(x) ((u32)(((x >> 8) & 0x0000000F)))
#define LDO_MASK_VOLT_CODE1_CLDO   ((u32)0x0000000F << 4)            /*!< R/W 4'b0011  Core LDO code1 , which indicate 0.8v*/
#define LDO_VOLT_CODE1_CLDO(x)     (((u32)((x) & 0x0000000F) << 4))
#define LDO_GET_VOLT_CODE1_CLDO(x) ((u32)(((x >> 4) & 0x0000000F)))
#define LDO_MASK_VOLT_CODE0_CLDO   ((u32)0x0000000F << 0)            /*!< R/W 4'b0000  Core LDO code0 , which indicate 0.7v , used in power cut mode*/
#define LDO_VOLT_CODE0_CLDO(x)     (((u32)((x) & 0x0000000F) << 0))
#define LDO_GET_VOLT_CODE0_CLDO(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup LDO_PWM_VOLT_CTRL
  * @brief
  * @{
  */
#define LDO_MASK_VOLT_CODE3_PWM   ((u32)0x0000000F << 12)           /*!< R/W 4'b1011  SWR pwm volt code3 , which indicate 1.35v (for system in 1.0v)*/
#define LDO_VOLT_CODE3_PWM(x)     (((u32)((x) & 0x0000000F) << 12))
#define LDO_GET_VOLT_CODE3_PWM(x) ((u32)(((x >> 12) & 0x0000000F)))
#define LDO_MASK_VOLT_CODE2_PWM   ((u32)0x0000000F << 8)            /*!< R/W 4'b0101  SWR pwm volt code2 , which indicate 1.25v (for system in 0.9v, snooze also use) Note:code2/code3 , pwm_vref_sel must set 11(default)*/
#define LDO_VOLT_CODE2_PWM(x)     (((u32)((x) & 0x0000000F) << 8))
#define LDO_GET_VOLT_CODE2_PWM(x) ((u32)(((x >> 8) & 0x0000000F)))
#define LDO_MASK_VOLT_CODE1_PWM   ((u32)0x0000000F << 4)            /*!< R/W 4'b0101  NO use*/
#define LDO_VOLT_CODE1_PWM(x)     (((u32)((x) & 0x0000000F) << 4))
#define LDO_GET_VOLT_CODE1_PWM(x) ((u32)(((x >> 4) & 0x0000000F)))
#define LDO_MASK_VOLT_CODE0_PWM   ((u32)0x0000000F << 0)            /*!< R/W 4'b0101  SWR pwm volt code0 , which indicate 0.8v (used for snooze mode , at this time , CLDO is in PC mode) Note:vref must change , pwm_vref_sel must set to 00*/
#define LDO_VOLT_CODE0_PWM(x)     (((u32)((x) & 0x0000000F) << 0))
#define LDO_GET_VOLT_CODE0_PWM(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup LDO_PFM_VOLT_CTRL
  * @brief
  * @{
  */
#define LDO_MASK_VOLT_CODE4_PFM   ((u32)0x0000000F << 16)           /*!< R/W 4'b1111  SWR pfm volt code4 , which indicate 1.35v (for system in 1.0v)*/
#define LDO_VOLT_CODE4_PFM(x)     (((u32)((x) & 0x0000000F) << 16))
#define LDO_GET_VOLT_CODE4_PFM(x) ((u32)(((x >> 16) & 0x0000000F)))
#define LDO_MASK_VOLT_CODE3_PFM   ((u32)0x0000000F << 12)           /*!< R/W 4'b1011  SWR pfm volt code3 , which indicate 1.25v (for system in 0.9v)*/
#define LDO_VOLT_CODE3_PFM(x)     (((u32)((x) & 0x0000000F) << 12))
#define LDO_GET_VOLT_CODE3_PFM(x) ((u32)(((x >> 12) & 0x0000000F)))
#define LDO_MASK_VOLT_CODE2_PFM   ((u32)0x0000000F << 8)            /*!< R/W 4'b0110  SWR pfm volt code2 , which indicate 0.9v  Note:this also used as pre power cut change point*/
#define LDO_VOLT_CODE2_PFM(x)     (((u32)((x) & 0x0000000F) << 8))
#define LDO_GET_VOLT_CODE2_PFM(x) ((u32)(((x >> 8) & 0x0000000F)))
#define LDO_MASK_VOLT_CODE1_PFM   ((u32)0x0000000F << 4)            /*!< R/W 4'b0100  SWR pfm volt code1 , which indicate 0.8v*/
#define LDO_VOLT_CODE1_PFM(x)     (((u32)((x) & 0x0000000F) << 4))
#define LDO_GET_VOLT_CODE1_PFM(x) ((u32)(((x >> 4) & 0x0000000F)))
#define LDO_MASK_VOLT_CODE0_PFM   ((u32)0x0000000F << 0)            /*!< R/W 4'b0010  SWR pfm volt code0 , which indicate 0.7v*/
#define LDO_VOLT_CODE0_PFM(x)     (((u32)((x) & 0x0000000F) << 0))
#define LDO_GET_VOLT_CODE0_PFM(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup LDO_REGU_PMC_SP_UP
  * @brief
  * @{
  */
#define LDO_MASK_APMC_SP2   ((u32)0x0000007F << 16)           /*!< R/W 7'd59  Start Point for ST6 up*/
#define LDO_APMC_SP2(x)     (((u32)((x) & 0x0000007F) << 16))
#define LDO_GET_APMC_SP2(x) ((u32)(((x >> 16) & 0x0000007F)))
#define LDO_MASK_APMC_SP1   ((u32)0x0000007F << 8)            /*!< R/W 7'd47  Start Point for ST5 up*/
#define LDO_APMC_SP1(x)     (((u32)((x) & 0x0000007F) << 8))
#define LDO_GET_APMC_SP1(x) ((u32)(((x >> 8) & 0x0000007F)))
#define LDO_MASK_APMC_SP0   ((u32)0x0000007F << 0)            /*!< R/W 7'd45  Start Point for ST3/ST4 up (ST3 not support)*/
#define LDO_APMC_SP0(x)     (((u32)((x) & 0x0000007F) << 0))
#define LDO_GET_APMC_SP0(x) ((u32)(((x >> 0) & 0x0000007F)))
/** @} */

/** @defgroup LDO_REGU_PMC_SP_DN
  * @brief
  * @{
  */
#define LDO_MASK_APMC_SP5   ((u32)0x0000007F << 16)           /*!< R/W 7'd81  Start Point for ST2 dn  , noused*/
#define LDO_APMC_SP5(x)     (((u32)((x) & 0x0000007F) << 16))
#define LDO_GET_APMC_SP5(x) ((u32)(((x >> 16) & 0x0000007F)))
#define LDO_MASK_APMC_SP4   ((u32)0x0000007F << 8)            /*!< R/W 7'd79  Start Point for ST4/ST3 dn*/
#define LDO_APMC_SP4(x)     (((u32)((x) & 0x0000007F) << 8))
#define LDO_GET_APMC_SP4(x) ((u32)(((x >> 8) & 0x0000007F)))
#define LDO_MASK_APMC_SP3   ((u32)0x0000007F << 0)            /*!< R/W 7'd76  Start Point for ST5 dn*/
#define LDO_APMC_SP3(x)     (((u32)((x) & 0x0000007F) << 0))
#define LDO_GET_APMC_SP3(x) ((u32)(((x >> 0) & 0x0000007F)))
/** @} */

/** @defgroup LDO_DEBUG
  * @brief
  * @{
  */
#define LDO_MASK_ADBG_REGU   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  REGU interface debug*/
#define LDO_ADBG_REGU(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define LDO_GET_ADBG_REGU(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup LDO_DUMMY
  * @brief
  * @{
  */
#define LDO_MASK_PMC_PRG_DLY       ((u32)0x00000007 << 12)           /*!< R/W 3'd6  regu_pmc programmable delay value 0: 100us 1: 500us 2: 1ms 3: 2ms 4: 10ms 5: 50ms 6: 100ms 7: 200ms*/
#define LDO_PMC_PRG_DLY(x)         (((u32)((x) & 0x00000007) << 12))
#define LDO_GET_PMC_PRG_DLY(x)     ((u32)(((x >> 12) & 0x00000007)))
#define LDO_MASK_MLDO_STL          ((u32)0x00000003 << 10)           /*!< R/W 2'd2  mem LDO power on stable wait time 00: 200us 01: 500us 10: 1ms 11: 2ms*/
#define LDO_MLDO_STL(x)            (((u32)((x) & 0x00000003) << 10))
#define LDO_GET_MLDO_STL(x)        ((u32)(((x >> 10) & 0x00000003)))
#define LDO_BIT_CLDO_FORCE_DML_OFF ((u32)0x00000001 << 9)            /*!< R/W 1'b0  Force core ldo dummy load off , used when pc mode 1: force CLDO dummy load off , regu pmc will wirte this when enter pc mode 0: release*/
#define LDO_BIT_TMODE_MLDO         ((u32)0x00000001 << 8)            /*!< R/W 1'b0  3318 LDO enter testmode , and register setting will not controlled by pmc*/
#define LDO_BIT_OSC4M_IGN_VOLT     ((u32)0x00000001 << 3)            /*!< R/W 1'b0  Options if osc4m can auto require voltage , disable it may speedup reponse time when system never goto 0.7v 0. osc4m will send volt areq when osc4m enable or disable 1. osc4m never req volt change , make sure , system voltage will never goto 0.7v Note: RL6973 never goto 0.7v, fix 1*/
#define LDO_BIT_OPT_EN_SLP_DUMMY   ((u32)0x00000001 << 2)            /*!< R/W 1'b0  option for enable swr dummy load to speed up when pfm voltage falling. Default not enable , using a programmble delay to wait*/
#define LDO_BIT_OPT_EN_WAK_DUMMY   ((u32)0x00000001 << 1)            /*!< R/W 1'b1  option for enable swr dummy load to speed up when pfm -> pwm. if opt_wait_mode is 0 , this bit has no use*/
#define LDO_BIT_OPT_IGN_MODE       ((u32)0x00000001 << 0)            /*!< R/W 1'b1  option for ignore regu mode_l wait when pfm -> pwm. default not wait as usual*/
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup LDO_Exported_Types LDO Exported Types
  * @{
  */

/** @brief LDO Register Declaration
  */

typedef struct {
	__IO uint32_t LDO_TEST_KEY_REG   ;  /*!< Register,  Address offset:0x000 */
	__IO uint32_t LDO_RF_EN_DUMMY    ;  /*!< Register,  Address offset:0x004 */
	__IO uint32_t LDO_4M_OSC_CTRL    ;  /*!< Register,  Address offset:0x008 */
	__IO uint32_t LDO_4M_OSC_CTRL1   ;  /*!< Register,  Address offset:0x00C */
	__IO uint32_t LDO_32_OSC_XTAL_POW;  /*!< Register,  Address offset:0x010 */
	__IO uint32_t LDO_32K_OSC_CTRL   ;  /*!< Register,  Address offset:0x014 */
	__IO uint32_t LDO_AONLDO         ;  /*!< Register,  Address offset:0x018 */
	__IO uint32_t LDO_BOD            ;  /*!< Register,  Address offset:0x01C */
	__IO uint32_t LDO_3318           ;  /*!< Register,  Address offset:0x020 */
	__IO uint32_t LDO_RFAFE_1209     ;  /*!< Register,  Address offset:0x024 */
	__IO uint32_t LDO_RTC_CTRL       ;  /*!< Register,  Address offset:0x028 */
	__IO uint32_t RSVD0              ;  /*!< Reserved,  Address offset:0x02C-0x02F */
	__I  uint32_t LDO_APMC_STA       ;  /*!< Register,  Address offset:0x030 */
	__I  uint32_t LDO_APMC_VEC       ;  /*!< Register,  Address offset:0x034 */
	__I  uint32_t LDO_APMC_ISR       ;  /*!< Register,  Address offset:0x038 */
	__IO uint32_t RSVD1              ;  /*!< Reserved,  Address offset:0x03C-0x03F */
	__IO uint32_t LDO_ALDO_SYS_CTRL  ;  /*!< Register,  Address offset:0x040 */
	__IO uint32_t LDO_CLDO_VOLT_CTRL ;  /*!< Register,  Address offset:0x044 */
	__IO uint32_t LDO_PWM_VOLT_CTRL  ;  /*!< Register,  Address offset:0x048 */
	__IO uint32_t LDO_PFM_VOLT_CTRL  ;  /*!< Register,  Address offset:0x04C */
	__IO uint32_t RSVD2[4]           ;  /*!< Reserved,  Address offset:0x050-0x05F */
	__IO uint32_t LDO_REGU_PMC_SP_UP ;  /*!< Register,  Address offset:0x060 */
	__IO uint32_t LDO_REGU_PMC_SP_DN ;  /*!< Register,  Address offset:0x064 */
	__IO uint32_t RSVD3[2]           ;  /*!< Reserved,  Address offset:0x068-0x06F */
	__I  uint32_t LDO_DEBUG          ;  /*!< Register,  Address offset:0x070 */
	__IO uint32_t RSVD4[2]           ;  /*!< Reserved,  Address offset:0x074-0x07B */
	__IO uint32_t LDO_DUMMY          ;  /*!< Register,  Address offset:0x07C */
} LDO_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


#define LDO_DBG_OSC_OK	BIT(10)

/**
  * @}
  */
/** @defgroup MLDO_MODE Mem LDO mode
  * @{
  */
enum MemLDO_Mode {
	MLDO_OFF = 0x0,
	MLDO_SLEEP = 0x1,
	MLDO_NORMAL_DUMMY = 0x2,
	MLDO_NORMAL = 0x3,
};
/**
  * @}
  */

/** @defgroup REGU_PMC_Delay
  * @{
  */
enum REGU_PMC_DELAY {
	REGU_DELAY_100US = 0x0,
	REGU_DELAY_500US = 0x1, /* acceleration for simulation*/
	REGU_DELAY_1MS = 0x2,
	REGU_DELAY_2MS = 0x3,
	REGU_DELAY_10MS = 0x4,
	REGU_DELAY_50MS = 0x5,
	REGU_DELAY_100MS = 0x6,
	REGU_DELAY_200MS = 0x7,
};

/**
  * @}
  */

/** @defgroup LDO_Exported_Functions LDO Exported Functions
  * @{
  */
void LDO_CoreVolSet(u8 vol_type);
bool LDO_MemSetInNormal(u8 normal_mode);
bool LDO_MemSetInSleep(u8 sleep_mode);
/**
  * @}
  */
#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

/** @} */

/** @} */

#endif