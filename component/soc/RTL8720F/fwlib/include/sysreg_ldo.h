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
#define LDO_BIT_EXT_SWR_GPIO_PD_SET ((u32)0x00000001 << 15)           /*!< R/WA0 1'b0  Set extenal swr gpio pull dn*/
#define LDO_BIT_EXT_SWR_GPIO_PU_SET ((u32)0x00000001 << 14)           /*!< R/WA0 1'b0  Set extenal swr gpio pull up*/
#define LDO_MASK_EXT_SWR_DELAY      ((u32)0x00000003 << 11)           /*!< R/W 2'b0  External swr power stable time 2'b00: 750us 2'b01: 1ms 2'b10: 1.5ms 2'b11: 2ms*/
#define LDO_EXT_SWR_DELAY(x)        (((u32)((x) & 0x00000003) << 11))
#define LDO_GET_EXT_SWR_DELAY(x)    ((u32)(((x >> 11) & 0x00000003)))
#define LDO_BIT_EXT_SWR_CT          ((u32)0x00000001 << 10)           /*!< R/W 1'b0  1: External swr can be controller by gpio 0: External swr has no controlled pin*/
#define LDO_BIT_DELAY               ((u32)0x00000001 << 9)            /*!< R/W 1'b1  1:Internal core ldo will be enable after chip reset , delay 3ms 0:NO extra delay applied*/
#define LDO_BIT_DCORE_EXT_SWR       ((u32)0x00000001 << 8)            /*!< R/W 1'b0  1:use external swr for core power 0:use internal LDO for core power*/
#define LDO_BIT_POR_SYSON           ((u32)0x00000001 << 1)            /*!< R/W 1'b0  syson power reset , active 0 1: release syson por reset 0: assert syson por reset*/
#define LDO_BIT_PWC_SYSON           ((u32)0x00000001 << 0)            /*!< R/W 1'b0  syson on power switch enable for core power, high active. 1:enable  0:disable*/
/** @} */

/** @defgroup LDO_BG_CTRL
  * @brief
  * @{
  */
#define LDO_MASK_REG_BG_R1_TUNE       ((u32)0x00000007 << 8)           /*!< R/W 3'b100  */
#define LDO_REG_BG_R1_TUNE(x)         (((u32)((x) & 0x00000007) << 8))
#define LDO_GET_REG_BG_R1_TUNE(x)     ((u32)(((x >> 8) & 0x00000007)))
#define LDO_MASK_REG_BG_R2_TUNE       ((u32)0x00000007 << 5)           /*!< R/W 3'b100  */
#define LDO_REG_BG_R2_TUNE(x)         (((u32)((x) & 0x00000007) << 5))
#define LDO_GET_REG_BG_R2_TUNE(x)     ((u32)(((x >> 5) & 0x00000007)))
#define LDO_MASK_REG_BG_VR1200_TUNE   ((u32)0x00000007 << 2)           /*!< R/W 3'b100  */
#define LDO_REG_BG_VR1200_TUNE(x)     (((u32)((x) & 0x00000007) << 2))
#define LDO_GET_REG_BG_VR1200_TUNE(x) ((u32)(((x >> 2) & 0x00000007)))
#define LDO_MASK_REG_BG_CURRENT       ((u32)0x00000003 << 0)           /*!< R/W 2'b10  */
#define LDO_REG_BG_CURRENT(x)         (((u32)((x) & 0x00000003) << 0))
#define LDO_GET_REG_BG_CURRENT(x)     ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup LDO_2M_OSC_CTRL
  * @brief
  * @{
  */
#define LDO_MASK_REG_2MOSC_ISEL       ((u32)0x00000003 << 8)           /*!< R/W 2'b1  */
#define LDO_REG_2MOSC_ISEL(x)         (((u32)((x) & 0x00000003) << 8))
#define LDO_GET_REG_2MOSC_ISEL(x)     ((u32)(((x >> 8) & 0x00000003)))
#define LDO_BIT_REG_POW_2MOSC         ((u32)0x00000001 << 7)           /*!< R 1'b0  // [RF][2MOSC] 1'b1 : enable*/
#define LDO_BIT_REG_POW_2MOSC_FASTSET ((u32)0x00000001 << 6)           /*!< R/W 1'b0  // [RF][2MOSC] REG_CTRL: enable fastset control*/
#define LDO_MASK_REG_2MOSC_RCAL       ((u32)0x0000003F << 0)           /*!< R/W 6'b100000  // [RF][2MOSC] adjust 2M osc frequency. Max freq:6'b111111*/
#define LDO_REG_2MOSC_RCAL(x)         (((u32)((x) & 0x0000003F) << 0))
#define LDO_GET_REG_2MOSC_RCAL(x)     ((u32)(((x >> 0) & 0x0000003F)))
/** @} */

/** @defgroup LDO_AONLDO
  * @brief
  * @{
  */
#define LDO_MASK_DMY_1          ((u32)0x0000003F << 2)           /*!< R/W 0x0  */
#define LDO_DMY_1(x)            (((u32)((x) & 0x0000003F) << 2))
#define LDO_GET_DMY_1(x)        ((u32)(((x >> 2) & 0x0000003F)))
#define LDO_BIT_LV_POR_VREF_SEL ((u32)0x00000001 << 1)           /*!< R/W 1'b0  LV_POR VREF 0 (default) VREF = 0P600V 1 VREF = 0P450V*/
#define LDO_BIT_DMY_0           ((u32)0x00000001 << 0)           /*!< R/W 0x0  */
/** @} */

/** @defgroup LDO_BOD
  * @brief
  * @{
  */
#define LDO_BIT_REG_BOD_R_EN ((u32)0x00000001 << 11)          /*!< R/W 1'b0  // 0: disable BOD // 1: enable BOD*/
#define LDO_MASK_REG_BOD_R   ((u32)0x0000001F << 6)           /*!< R/W 5'b00011  00000:2.896 00001:2.856 00010:2.806 00011:2.766 00100:2.706 00101:2.656 00110:2.606 00111:2.556 01000:2.497 01001:2.447 01010:2.397 01011:2.347 01100:2.297 01101:2.257 01110:2.198 01111:2.148 [5]: dummy bit*/
#define LDO_REG_BOD_R(x)     (((u32)((x) & 0x0000001F) << 6))
#define LDO_GET_REG_BOD_R(x) ((u32)(((x >> 6) & 0x0000001F)))
#define LDO_BIT_REG_BOD_F_D  ((u32)0x00000001 << 5)           /*!< R/W 1'b0  // dummy*/
#define LDO_MASK_REG_BOD_F   ((u32)0x0000001F << 0)           /*!< R/W 5'b0110  00000:2.896 00001:2.856 00010:2.806 00011:2.766 00100:2.706 00101:2.656 00110:2.606 00111:2.556 01000:2.497 01001:2.447 01010:2.397 01011:2.347 01100:2.297 01101:2.257 01110:2.198 01111:2.148 [5]:dummy bit*/
#define LDO_REG_BOD_F(x)     (((u32)((x) & 0x0000001F) << 0))
#define LDO_GET_REG_BOD_F(x) ((u32)(((x >> 0) & 0x0000001F)))
/** @} */

/** @defgroup LDO_CORE
  * @brief
  * @{
  */
#define LDO_MASK_REG_DMY_LDO_L       ((u32)0x00000007 << 17)           /*!< R/W 0x0  // [REGU][LDO] REG_CTRL //Dummy reg*/
#define LDO_REG_DMY_LDO_L(x)         (((u32)((x) & 0x00000007) << 17))
#define LDO_GET_REG_DMY_LDO_L(x)     ((u32)(((x >> 17) & 0x00000007)))
#define LDO_BIT_REG_FEEDBACK_SELECT  ((u32)0x00000001 << 16)           /*!< R/W 0x0  0: feedback from AVDD_0P9H_LDO_OUTPUT_PAD 1: feedback from AVDD_0P9H_LDO_VOFB Default must be 0 when power on , when syson pwr enable , it will switch to 1*/
#define LDO_BIT_REG_SSOK_L           ((u32)0x00000001 << 15)           /*!< R/W 0x0  // [REGU][LDO] REG_CTRL // soft-start PWRMOS control // 0: medium PWRMOS (default) // 1: large PWRMOS*/
#define LDO_BIT_REG_DIODE3_L         ((u32)0x00000001 << 14)           /*!< R/W 0x1  // [REGU][LDO] REG_CTRL // PGATE diode3 clamp // 0: disable // 1: enable (default)*/
#define LDO_BIT_REG_DIODE_L          ((u32)0x00000001 << 13)           /*!< R/W 0x0  // [REGU][LDO] REG_CTRL // PGATE diode2 clamp // 0: disable (default) // 1: enable*/
#define LDO_BIT_REG_BUFFERTYPE_SEL_L ((u32)0x00000001 << 12)           /*!< R/W 0x0  // [REGU][LDO] REG_CTRL // Buffer type select // 0: P-type buffer for 3.3V PWR (default) // 1: N-type buffer for 1.8V PWR*/
#define LDO_MASK_DMY_LOAD            ((u32)0x00000003 << 10)           /*!< R/W 2'b00  // [REGU][LDO] REG_CTRL // LDO dmy load controll // [1]dmy load // 1: enable  // 0: disable (default) // [0] light dmy load // 1: enable  // 0: disable (default)*/
#define LDO_DMY_LOAD(x)              (((u32)((x) & 0x00000003) << 10))
#define LDO_GET_DMY_LOAD(x)          ((u32)(((x >> 10) & 0x00000003)))
#define LDO_BIT_REG_ITAIL_HALF_EN_L  ((u32)0x00000001 << 9)            /*!< R/W 0x0  // [REGU][LDO] REG_CTRL // Half tail current controll // 0: normal tail current (default) // 1: half tail current*/
#define LDO_MASK_REG_STANDBY_L       ((u32)0x00000003 << 7)            /*!< R/W 2'b11  // [REGU][LDO] REG_CTRL // [0] Iq controll // 0: Normal Iq (default) // 1: Standby Iq // [1] pwrmos controll // 0: Normal pwrmos (default) // 1: Standby pwrmos*/
#define LDO_REG_STANDBY_L(x)         (((u32)((x) & 0x00000003) << 7))
#define LDO_GET_REG_STANDBY_L(x)     ((u32)(((x >> 7) & 0x00000003)))
#define LDO_BIT_EN_DIODE_L           ((u32)0x00000001 << 6)            /*!< R/W 0x1  // [REGU][LDO] REG_CTRL // PGATE diode1 clamp //0:disable //1:enable (default)*/
#define LDO_MASK_VOADJ_L             ((u32)0x0000000F << 2)            /*!< R/W/ES 0x1000  // [REGU][LDO] REG_CTRL // LDO VOSEL // 0000, 0001, 0010, 0011: 648.2mV, 681.2mV, 714.0mV, 745.9mV // 0100, 0101, 0110, 0111: 776.2mV, 809.1mV, 839.6mV, 872.5mV // 1000 (default), 1001, 1010, 1011: 901.9mV, 933.4mV, 967.2mV, 996.1mV // 1100, 1101, 1110, 1111: 1026.7mV, 1059.2mV, 1085.0mV, 1112.1mV*/
#define LDO_VOADJ_L(x)               (((u32)((x) & 0x0000000F) << 2))
#define LDO_GET_VOADJ_L(x)           ((u32)(((x >> 2) & 0x0000000F)))
#define LDO_BIT_PD_REGU_L            ((u32)0x00000001 << 1)            /*!< R/W 0x0  dmy*/
#define LDO_BIT_EN_PC_BT_L           ((u32)0x00000001 << 0)            /*!< R/W 0x0  dmy*/
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
#define LDO_MASK_VOLT_CODE1_ALDO   ((u32)0x0000000F << 28)           /*!< R/W 4'b0111  Aon LDO code1 , which indicate 0.8v*/
#define LDO_VOLT_CODE1_ALDO(x)     (((u32)((x) & 0x0000000F) << 28))
#define LDO_GET_VOLT_CODE1_ALDO(x) ((u32)(((x >> 28) & 0x0000000F)))
#define LDO_MASK_VOLT_CODE0_ALDO   ((u32)0x0000000F << 24)           /*!< R/W 4'b0010  Aon LDO code0 , which indicate 0.7v*/
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
#define LDO_MASK_VOLT_CODE3_CLDO   ((u32)0x0000000F << 12)           /*!< R/W 4'b1100  Core LDO code3 , which indicate 1.0v*/
#define LDO_VOLT_CODE3_CLDO(x)     (((u32)((x) & 0x0000000F) << 12))
#define LDO_GET_VOLT_CODE3_CLDO(x) ((u32)(((x >> 12) & 0x0000000F)))
#define LDO_MASK_VOLT_CODE2_CLDO   ((u32)0x0000000F << 8)            /*!< R/W 4'b1000  Core LDO code2 , which indicate 0.9v*/
#define LDO_VOLT_CODE2_CLDO(x)     (((u32)((x) & 0x0000000F) << 8))
#define LDO_GET_VOLT_CODE2_CLDO(x) ((u32)(((x >> 8) & 0x0000000F)))
#define LDO_MASK_VOLT_CODE1_CLDO   ((u32)0x0000000F << 4)            /*!< R/W 4'b0100  Core LDO code1 , which indicate 0.8v*/
#define LDO_VOLT_CODE1_CLDO(x)     (((u32)((x) & 0x0000000F) << 4))
#define LDO_GET_VOLT_CODE1_CLDO(x) ((u32)(((x >> 4) & 0x0000000F)))
#define LDO_MASK_VOLT_CODE0_CLDO   ((u32)0x0000000F << 0)            /*!< R/W 4'b0000  Core LDO code0 , which indicate 0.7v , used in power cut mode(not used)*/
#define LDO_VOLT_CODE0_CLDO(x)     (((u32)((x) & 0x0000000F) << 0))
#define LDO_GET_VOLT_CODE0_CLDO(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup LDO_REGU_PMC_SP_UP
  * @brief
  * @{
  */
#define LDO_MASK_APMC_SP2   ((u32)0x0000007F << 16)           /*!< R/W 7'd56  Start Point for ST6 up*/
#define LDO_APMC_SP2(x)     (((u32)((x) & 0x0000007F) << 16))
#define LDO_GET_APMC_SP2(x) ((u32)(((x >> 16) & 0x0000007F)))
#define LDO_MASK_APMC_SP1   ((u32)0x0000007F << 8)            /*!< R/W 7'd53  Start Point for ST5 up*/
#define LDO_APMC_SP1(x)     (((u32)((x) & 0x0000007F) << 8))
#define LDO_GET_APMC_SP1(x) ((u32)(((x >> 8) & 0x0000007F)))
#define LDO_MASK_APMC_SP0   ((u32)0x0000007F << 0)            /*!< R/W 7'd53  Start Point for ST3/ST4 up (ST3 not support)*/
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
#define LDO_MASK_APMC_SP4   ((u32)0x0000007F << 8)            /*!< R/W 7'd77  Start Point for ST4/ST3 dn*/
#define LDO_APMC_SP4(x)     (((u32)((x) & 0x0000007F) << 8))
#define LDO_GET_APMC_SP4(x) ((u32)(((x >> 8) & 0x0000007F)))
#define LDO_MASK_APMC_SP3   ((u32)0x0000007F << 0)            /*!< R/W 7'd73  Start Point for ST5 dn*/
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
#define LDO_MASK_PMC_OPT_DMY       ((u32)0x0000000F << 28)           /*!< R/W 4'h7  */
#define LDO_PMC_OPT_DMY(x)         (((u32)((x) & 0x0000000F) << 28))
#define LDO_GET_PMC_OPT_DMY(x)     ((u32)(((x >> 28) & 0x0000000F)))
#define LDO_BIT_OPT_FB_VDDC        ((u32)0x00000001 << 16)           /*!< R/W 1'b0  Select LDO feedback voltage net 0: default , select OUTPT pad poing  1: used erc_vddc_syson as feedback*/
#define LDO_MASK_PMC_PRG_DLY       ((u32)0x00000007 << 12)           /*!< R/W 3'd6  regu_pmc programmable delay value 0: 100us 1: 500us 2: 1ms 3: 2ms 4: 10ms 5: 50ms 6: 100ms 7: 200ms*/
#define LDO_PMC_PRG_DLY(x)         (((u32)((x) & 0x00000007) << 12))
#define LDO_GET_PMC_PRG_DLY(x)     ((u32)(((x >> 12) & 0x00000007)))
#define LDO_MASK_MLDO_STL          ((u32)0x00000003 << 10)           /*!< R/W 2'd2  mem LDO power on stable wait time 00: 200us 01: 500us 10: 1ms 11: 2ms*/
#define LDO_MLDO_STL(x)            (((u32)((x) & 0x00000003) << 10))
#define LDO_GET_MLDO_STL(x)        ((u32)(((x >> 10) & 0x00000003)))
#define LDO_BIT_CLDO_FORCE_DML_OFF ((u32)0x00000001 << 9)            /*!< R/W 1'b0  Force core ldo dummy load off , used when pc mode 1: force CLDO dummy load off , regu pmc will wirte this when enter pc mode 0: release*/
#define LDO_BIT_TMODE_MLDO         ((u32)0x00000001 << 8)            /*!< R/W 1'b0  3318 LDO enter testmode , and register setting will not controlled by pmc*/
#define LDO_MASK_OPT_POR_DIS       ((u32)0x0000000F << 4)            /*!< R/W 4'h0  option to disable POR for power saving when enter DSLP [0]: BOD , 1:disable when DSLP , 0:enable [1]: CORE_POR [2]: V33_POR_BG [3]: V33_POR_VTH*/
#define LDO_OPT_POR_DIS(x)         (((u32)((x) & 0x0000000F) << 4))
#define LDO_GET_OPT_POR_DIS(x)     ((u32)(((x >> 4) & 0x0000000F)))
#define LDO_BIT_OSC4M_IGN_VOLT     ((u32)0x00000001 << 3)            /*!< R/W 1'b0  Options if osc4m can auto require voltage , disable it may speedup reponse time when system never goto 0.7v 0. osc4m will send volt areq when osc4m enable or disable 1. osc4m never req volt change , make sure , system voltage will never goto 0.7v Note: RL6973 never goto 0.7v, fix 1*/
#define LDO_BIT_OPT_EN_SLP_DUMMY   ((u32)0x00000001 << 2)            /*!< R/W 1'b0  6955 special used for OSC2M force enable. 1: OSC2M enable when DSLP 0: OSC2M disable when DSLP when SYSON power on, this bit has no use , OSC2M always enable*/
#define LDO_BIT_OPT_EN_WAK_DUMMY   ((u32)0x00000001 << 1)            /*!< R/W 1'b1  6955 no use*/
#define LDO_BIT_OPT_IGN_MODE       ((u32)0x00000001 << 0)            /*!< R/W 1'b1  6955 no use*/
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup LDO_Exported_Types LDO Exported Types
  * @{
  */

/** @brief LDO Register Declaration
  */

typedef struct {
	__IO uint32_t LDO_TEST_KEY_REG  ;  /*!< Register,  Address offset:0x000 */
	__IO uint32_t LDO_BG_CTRL       ;  /*!< Register,  Address offset:0x004 */
	__IO uint32_t LDO_2M_OSC_CTRL   ;  /*!< Register,  Address offset:0x008 */
	__IO uint32_t RSVD0[3]          ;  /*!< Reserved,  Address offset:0x00C-0x017 */
	__IO uint32_t LDO_AONLDO        ;  /*!< Register,  Address offset:0x018 */
	__IO uint32_t LDO_BOD           ;  /*!< Register,  Address offset:0x01C */
	__IO uint32_t LDO_CORE          ;  /*!< Register,  Address offset:0x020 */
	__IO uint32_t RSVD1[3]          ;  /*!< Reserved,  Address offset:0x024-0x02F */
	__I  uint32_t LDO_APMC_STA      ;  /*!< Register,  Address offset:0x030 */
	__I  uint32_t LDO_APMC_VEC      ;  /*!< Register,  Address offset:0x034 */
	__I  uint32_t LDO_APMC_ISR      ;  /*!< Register,  Address offset:0x038 */
	__IO uint32_t RSVD2             ;  /*!< Reserved,  Address offset:0x03C-0x03F */
	__IO uint32_t LDO_ALDO_SYS_CTRL ;  /*!< Register,  Address offset:0x040 */
	__IO uint32_t LDO_CLDO_VOLT_CTRL;  /*!< Register,  Address offset:0x044 */
	__IO uint32_t RSVD3[6]          ;  /*!< Reserved,  Address offset:0x048-0x05F */
	__IO uint32_t LDO_REGU_PMC_SP_UP;  /*!< Register,  Address offset:0x060 */
	__IO uint32_t LDO_REGU_PMC_SP_DN;  /*!< Register,  Address offset:0x064 */
	__IO uint32_t RSVD4[2]          ;  /*!< Reserved,  Address offset:0x068-0x06F */
	__I  uint32_t LDO_DEBUG         ;  /*!< Register,  Address offset:0x070 */
	__IO uint32_t RSVD5[2]          ;  /*!< Reserved,  Address offset:0x074-0x07B */
	__IO uint32_t LDO_DUMMY         ;  /*!< Register,  Address offset:0x07C */
} LDO_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here

/** @defgroup LDO_Exported_Functions LDO Exported Functions
  * @{
  */
_LONG_CALL_	void LDO_CoreVolSet(u8 vol_type);
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