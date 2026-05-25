/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSREG_WLAFE_H_
#define _SYSREG_WLAFE_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup WLAFE
  * @brief WLAFE driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup WLAFE_Register_Definitions WLAFE Register Definitions
  * @{
  */

/** @defgroup WLAFE_ANAPAR_POW_MAC_0
  * @brief
  * @{
  */
#define WLAFE_BIT_WL_BT_REQ_PLL_ON  ((u32)0x00000001 << 12) /*!< R/W 0x0  1:enable pll 320m*/
#define WLAFE_BIT_POF_SEL           ((u32)0x00000001 << 11) /*!< R/W 0x1  Option to power off WLAFE 1: enable ,power off wlafe when bt&wl request off 0: disable ,keep wlafe power on when  bt&wl request off*/
#define WLAFE_BIT_ISO_SEL           ((u32)0x00000001 << 10) /*!< R/W 0x1  Option to enable isolation of wlafe when power off WLAFE 1: enable ,0x00[7]will be written 1 when wlafe power off 0: disable ,0x00[7]will keep 0 when wlafe power off hw tie1 in RL6955*/
#define WLAFE_BIT_HW_EN_WLAFE_2REGU ((u32)0x00000001 << 9)  /*!< R/W 0x0  control 3.3V power reset of wl afe, need level shift to 3.3V by regu no use in RL6955*/
#define WLAFE_BIT_HW_EN_WLAFE       ((u32)0x00000001 << 8)  /*!< R/W 0x0  core power reset of wl afe no use in RL6955*/
#define WLAFE_BIT_ISO_WLAFE         ((u32)0x00000001 << 7)  /*!< R/W 0x1  isolation signal*/
#define WLAFE_BIT_HW_EN_WLRF_2REGU  ((u32)0x00000001 << 6)  /*!< R/W 0x0  control 3.3V power reset of wl rf, need level shift to 3.3V by regu*/
#define WLAFE_BIT_HW_EN_WLRF        ((u32)0x00000001 << 5)  /*!< R/W 0x0  core power reset of wl rf*/
#define WLAFE_BIT_POW_LDO_RSSI_ADC  ((u32)0x00000001 << 4)  /*!< R/W 0x0  Power on ldo for rssi adc*/
#define WLAFE_BIT_POW_LDO_ADC       ((u32)0x00000001 << 3)  /*!< R/W 0x0  Power on ldo for adc*/
#define WLAFE_BIT_POW_LDO_DAC       ((u32)0x00000001 << 2)  /*!< R/W 0x0  Power on ldo for dac*/
#define WLAFE_BIT_POW_PLL           ((u32)0x00000001 << 1)  /*!< R/W 0x0  Power on pll*/
#define WLAFE_BIT_POW_LDO           ((u32)0x00000001 << 0)  /*!< R/W 0x0  Power on AFE power cut and pll ldo*/
/** @} */

/** @defgroup WLAFE_ANAPAR_MAC_2
  * @brief
  * @{
  */
#define WLAFE_MASK_R_WLAFE_LDO_ST       ((u32)0x00000003 << 27)           /*!< R/W 0x10  stable time select for WLAFE LDO 2'b00:10us, 2'b01:20us, 2'b10:100us, 2'b11:200us*/
#define WLAFE_R_WLAFE_LDO_ST(x)         (((u32)((x) & 0x00000003) << 27))
#define WLAFE_GET_R_WLAFE_LDO_ST(x)     ((u32)(((x >> 27) & 0x00000003)))
#define WLAFE_MASK_BB_DBG_SEL_AFE_SDM   ((u32)0x00000003 << 8)            /*!< R/W 0x0  SDM debug selection*/
#define WLAFE_BB_DBG_SEL_AFE_SDM(x)     (((u32)((x) & 0x00000003) << 8))
#define WLAFE_GET_BB_DBG_SEL_AFE_SDM(x) ((u32)(((x >> 8) & 0x00000003)))
#define WLAFE_BIT_SW_LDO2PWRCUT_DA      ((u32)0x00000001 << 7)            /*!< R/W 0x0  1 power cut mode 0 ldo mode (DA LDO)*/
#define WLAFE_MASK_SW_LDO_DA            ((u32)0x00000003 << 5)            /*!< R/W 0x10  Ldo voltage 00/01/10/11: 0.875V/0.9V/0.95V/1V (DAC LDO)*/
#define WLAFE_SW_LDO_DA(x)              (((u32)((x) & 0x00000003) << 5))
#define WLAFE_GET_SW_LDO_DA(x)          ((u32)(((x >> 5) & 0x00000003)))
#define WLAFE_BIT_VPLUSE_LDO_DA         ((u32)0x00000001 << 4)            /*!< R/W 0x1  For DAC(WL DAC LDO)*/
#define WLAFE_BIT_SW_LDO2PWRCUT_AD      ((u32)0x00000001 << 3)            /*!< R/W 0x0  1 power cut mode 0 ldo mode (AD LDO)*/
#define WLAFE_MASK_SW_LDO_AD            ((u32)0x00000003 << 1)            /*!< R/W 0x10  Ldo voltage 00/01/10/11: 0.875V/0.9V/0.95V/1V (ADC LDO)*/
#define WLAFE_SW_LDO_AD(x)              (((u32)((x) & 0x00000003) << 1))
#define WLAFE_GET_SW_LDO_AD(x)          ((u32)(((x >> 1) & 0x00000003)))
#define WLAFE_BIT_VPLUSE_LDO_AD         ((u32)0x00000001 << 0)            /*!< R/W 0x1  For ADC(WL ADC LDO)*/
/** @} */

/** @defgroup WLAFE_DBG_WLAFE_PMC
  * @brief
  * @{
  */
#define WLAFE_MASK_DBG_WLAFE_PMC   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  debug signal of wlafe_pmc [31]:pmc_rdy [30]:powon_req [29]:powoff_ref [28]:wlafe_is_power_on [27]:first_pon_flag [26]:pmc_timer_en [25]:|pmc_wem [24]:|pmc_wd [23:18]:0 [17]syspll_rdy [16]:wlafe_en_osc4m [15]:pmc_wr [14]:pmc_timer_out [13]:pmc_wr_done [12]:wl_power_flag [11]:bt_power_flag [10]:wl_pon_gnt [9]:bt_pon_gnt [8]:wlbt_pll_rdy [7]:en_xtal_frv_afe [6:0]pmc_state*/
#define WLAFE_DBG_WLAFE_PMC(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define WLAFE_GET_DBG_WLAFE_PMC(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup WLAFE_DBG_WLAFE
  * @brief
  * @{
  */
#define WLAFE_MASK_DBG_WLAFE_REG   ((u32)0x0000FFFF << 0)           /*!< R 0x0  debug signal of wlafe_reg*/
#define WLAFE_DBG_WLAFE_REG(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define WLAFE_GET_DBG_WLAFE_REG(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup WLAFE_ANAPAR_RFC
  * @brief
  * @{
  */
#define WLAFE_BIT_WLBB_ADC_SEL        ((u32)0x00000001 << 20)           /*!< R/W 0x0  1:if gnt bt is high,send 0x200 to wlbb as adc data 0:send adc data form afe out directly to wlbb*/
#define WLAFE_BIT_WL_FUNEN_WLRF_SEL   ((u32)0x00000001 << 19)           /*!< R/W 0x0  1: select ANAPOW_RFC_NM as ANAPAR_RFC[7:0],used when RFAFE is active 0: select ANAPOW_RFC_LPS as ANAPAR_RFC[7:0],,used when RFAFE is power off*/
#define WLAFE_MASK_ANAPOW_RFC_LPS     ((u32)0x000000FF << 11)           /*!< R/W 0x01100101  wlan power rf control*/
#define WLAFE_ANAPOW_RFC_LPS(x)       (((u32)((x) & 0x000000FF) << 11))
#define WLAFE_GET_ANAPOW_RFC_LPS(x)   ((u32)(((x >> 11) & 0x000000FF)))
#define WLAFE_BIT_R_RFAFE_VIO1833_SEL ((u32)0x00000001 << 10)           /*!< R/W 0x0  IND_VIO1833 output decided by: 1: 1.8V  0: 3.3V*/
#define WLAFE_MASK_ANAPOW_RFC_NM      ((u32)0x000000FF << 0)            /*!< R/W 0x11111111  wlan power rf control*/
#define WLAFE_ANAPOW_RFC_NM(x)        (((u32)((x) & 0x000000FF) << 0))
#define WLAFE_GET_ANAPOW_RFC_NM(x)    ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup WLAFE_LDO_VPLUSE_SEL
  * @brief
  * @{
  */
#define WLAFE_BIT_VPLUSE_SEL         ((u32)0x00000001 << 0)           /*!< R/W 0x1  vpluse_ldo output decided by: 1: power on ldo  0: ctrl reg*/
#define WLAFE_MASK_VPLUSE_CNT        ((u32)0x0000FFFF << 1)           /*!< R/W 0x0000000000011  vpluse_ldo high output delay cycle after pll_ldo set high,default:3(1.5us)*/
#define WLAFE_VPLUSE_CNT(x)          (((u32)((x) & 0x0000FFFF) << 1))
#define WLAFE_GET_VPLUSE_CNT(x)      ((u32)(((x >> 1) & 0x0000FFFF)))
#define WLAFE_BIT_VPLUSE_WAIT_MCYCLE ((u32)0x00000001 << 17)          /*!< R/W 0x1  1: vpluse_ldo_dac and vpluse_ldo_adc pull low should wait more cycle after ldo pull high*/
/** @} */

/** @defgroup WLAFE_WLRFC_CTRL
  * @brief
  * @{
  */
#define WLAFE_BIT_WLRFC_PWC_EN             ((u32)0x00000001 << 0)            /*!< R/W 0x0  1: gen request of wlan rfc power cut 0: gen request of turn off wlan rfc power cut*/
#define WLAFE_BIT_ISO_WLRFC_EN             ((u32)0x00000001 << 1)            /*!< R/W 0x1  Isolation control for wlan rfc 1: isolation 0: attach*/
#define WLAFE_BIT_WLRFC_RETAIN_EN_TYPE_1   ((u32)0x00000001 << 2)            /*!< R/W 0x1  1 to 0:gen req to restore register in power on flow 0 to 1:gen req to backup register in power on flow default should be 1,otherwise there be unexpected leakage power when VCC is power off and VCCG is still on*/
#define WLAFE_BIT_WLRFC_PWC_SEL            ((u32)0x00000001 << 3)            /*!< R/W 0x0  Option to turn off wlrfc pwr cut 1: enable  0: disable RL6955: RFC without retention FF. default is 0*/
#define WLAFE_BIT_CKRFC_EN                 ((u32)0x00000001 << 4)            /*!< R/W 0x1  1: enable WLRFC clock; 0: gated*/
#define WLAFE_BIT_WLRFC_SAVE_EN_TYPE_2     ((u32)0x00000001 << 8)            /*!< WA0 0x0  enable saving value of retention DFF when wlmac power off or pg,should be assert when reset is high*/
#define WLAFE_BIT_WLRFC_RESTORE_EN_TYPE_2  ((u32)0x00000001 << 9)            /*!< WA0 0x0  enable restoring value of retention DFF when wlmac power on,should be assert when reset is high*/
#define WLAFE_BIT_WLRFC_RETENTION_CELL_SEL ((u32)0x00000001 << 10)           /*!< R/W 0x0  1:choose retention cell type I(U22 ULP),which is controlled by 0x28[2] 0:choose retention cell type II(RTKM),which is controlled by 0x28[9:8]*/
#define WLAFE_BIT_DIS_WLBB_CKRFC_EN        ((u32)0x00000001 << 11)           /*!< R/W 0x0  the gating control of rfc clk is pta_ckrfc_en|wlbb_ckrfc_en|wlafe_ckrfc_en,and wlbb_ckrfc_en can not be control by register directly in RL6678,add option for top to control wlbb_ckrfc_en 1: disable wlbb_ckrfc_en; 0: enable wlbb_ckrfc_en*/
#define WLAFE_MASK_RFC_CLK_SEL             ((u32)0x00000003 << 12)           /*!< R/W 2'b01  11/10:40M 01:80M 00:160M*/
#define WLAFE_RFC_CLK_SEL(x)               (((u32)((x) & 0x00000003) << 12))
#define WLAFE_GET_RFC_CLK_SEL(x)           ((u32)(((x >> 12) & 0x00000003)))
#define WLAFE_MASK_CLK_RFC_PHASE           ((u32)0x0000000F << 14)           /*!< R/W 0x0  80M WLRFC clock phase control*/
#define WLAFE_CLK_RFC_PHASE(x)             (((u32)((x) & 0x0000000F) << 14))
#define WLAFE_GET_CLK_RFC_PHASE(x)         ((u32)(((x >> 14) & 0x0000000F)))
#define WLAFE_BIT_WL_CKDIV_SYNC_1SHOT      ((u32)0x00000001 << 18)           /*!< R/W 0x0  1: sync 40M/80M clock  phase with PLL*/
/** @} */

/** @defgroup WLAFE_AFE_PMC_PATCH_0
  * @brief
  * @{
  */
#define WLAFE_MASK_PMC_DST_STATE1    ((u32)0x0000007F << 24)           /*!< R/W 0x0  destination pmc state1 for state jump*/
#define WLAFE_PMC_DST_STATE1(x)      (((u32)((x) & 0x0000007F) << 24))
#define WLAFE_GET_PMC_DST_STATE1(x)  ((u32)(((x >> 24) & 0x0000007F)))
#define WLAFE_MASK_PMC_SRC_STATE1    ((u32)0x0000007F << 17)           /*!< R/W 0x0  source pmc state1 for state jump*/
#define WLAFE_PMC_SRC_STATE1(x)      (((u32)((x) & 0x0000007F) << 17))
#define WLAFE_GET_PMC_SRC_STATE1(x)  ((u32)(((x >> 17) & 0x0000007F)))
#define WLAFE_BIT_PMC_STATE_JUMP_EN1 ((u32)0x00000001 << 16)           /*!< R/W 0x0  1: when current afe pmc state hit pmc_src_state1, next pmc state will jump to pmc_dst_state1;  0:afe pmc follow old flow*/
#define WLAFE_MASK_PMC_DST_STATE0    ((u32)0x0000007F << 8)            /*!< R/W 0x0  destination pmc state0 for state jump*/
#define WLAFE_PMC_DST_STATE0(x)      (((u32)((x) & 0x0000007F) << 8))
#define WLAFE_GET_PMC_DST_STATE0(x)  ((u32)(((x >> 8) & 0x0000007F)))
#define WLAFE_MASK_PMC_SRC_STATE0    ((u32)0x0000007F << 1)            /*!< R/W 0x0  source pmc state0 for state jump*/
#define WLAFE_PMC_SRC_STATE0(x)      (((u32)((x) & 0x0000007F) << 1))
#define WLAFE_GET_PMC_SRC_STATE0(x)  ((u32)(((x >> 1) & 0x0000007F)))
#define WLAFE_BIT_PMC_STATE_JUMP_EN0 ((u32)0x00000001 << 0)            /*!< R/W 0x0  1: when current afe pmc state hit pmc_src_state0, next pmc state will jump to pmc_dst_state0;  0:afe pmc follow old flow*/
/** @} */

/** @defgroup WLAFE_AFE_PMC_PATCH_1
  * @brief
  * @{
  */
#define WLAFE_MASK_PMC_DST_STATE3    ((u32)0x0000007F << 24)           /*!< R/W 0x0  destination pmc state3 for state jump*/
#define WLAFE_PMC_DST_STATE3(x)      (((u32)((x) & 0x0000007F) << 24))
#define WLAFE_GET_PMC_DST_STATE3(x)  ((u32)(((x >> 24) & 0x0000007F)))
#define WLAFE_MASK_PMC_SRC_STATE3    ((u32)0x0000007F << 17)           /*!< R/W 0x0  source pmc state3 for state jump*/
#define WLAFE_PMC_SRC_STATE3(x)      (((u32)((x) & 0x0000007F) << 17))
#define WLAFE_GET_PMC_SRC_STATE3(x)  ((u32)(((x >> 17) & 0x0000007F)))
#define WLAFE_BIT_PMC_STATE_JUMP_EN3 ((u32)0x00000001 << 16)           /*!< R/W 0x0  1: when current afe pmc state hit pmc_src_state3, next pmc state will jump to pmc_dst_state3;  0:afe pmc follow old flow*/
#define WLAFE_MASK_PMC_DST_STATE2    ((u32)0x0000007F << 8)            /*!< R/W 0x0  destination pmc state2 for state jump*/
#define WLAFE_PMC_DST_STATE2(x)      (((u32)((x) & 0x0000007F) << 8))
#define WLAFE_GET_PMC_DST_STATE2(x)  ((u32)(((x >> 8) & 0x0000007F)))
#define WLAFE_MASK_PMC_SRC_STATE2    ((u32)0x0000007F << 1)            /*!< R/W 0x0  source pmc state2 for state jump*/
#define WLAFE_PMC_SRC_STATE2(x)      (((u32)((x) & 0x0000007F) << 1))
#define WLAFE_GET_PMC_SRC_STATE2(x)  ((u32)(((x >> 1) & 0x0000007F)))
#define WLAFE_BIT_PMC_STATE_JUMP_EN2 ((u32)0x00000001 << 0)            /*!< R/W 0x0  1: when current afe pmc state hit pmc_src_state2, next pmc state will jump to pmc_dst_state2;  0:afe pmc follow old flow*/
/** @} */

/** @defgroup WLAFE_AFE_PMC_PATCH_2
  * @brief
  * @{
  */
#define WLAFE_MASK_PMC_DST_STATE5    ((u32)0x0000007F << 24)           /*!< R/W 0x0  destination pmc state5 for state jump*/
#define WLAFE_PMC_DST_STATE5(x)      (((u32)((x) & 0x0000007F) << 24))
#define WLAFE_GET_PMC_DST_STATE5(x)  ((u32)(((x >> 24) & 0x0000007F)))
#define WLAFE_MASK_PMC_SRC_STATE5    ((u32)0x0000007F << 17)           /*!< R/W 0x0  source pmc state5 for state jump*/
#define WLAFE_PMC_SRC_STATE5(x)      (((u32)((x) & 0x0000007F) << 17))
#define WLAFE_GET_PMC_SRC_STATE5(x)  ((u32)(((x >> 17) & 0x0000007F)))
#define WLAFE_BIT_PMC_STATE_JUMP_EN5 ((u32)0x00000001 << 16)           /*!< R/W 0x0  1: when current afe pmc state hit pmc_src_state5, next pmc state will jump to pmc_dst_state5;  0:afe pmc follow old flow*/
#define WLAFE_MASK_PMC_DST_STATE4    ((u32)0x0000007F << 8)            /*!< R/W 0x0  destination pmc state4 for state jump*/
#define WLAFE_PMC_DST_STATE4(x)      (((u32)((x) & 0x0000007F) << 8))
#define WLAFE_GET_PMC_DST_STATE4(x)  ((u32)(((x >> 8) & 0x0000007F)))
#define WLAFE_MASK_PMC_SRC_STATE4    ((u32)0x0000007F << 1)            /*!< R/W 0x0  source pmc state4 for state jump*/
#define WLAFE_PMC_SRC_STATE4(x)      (((u32)((x) & 0x0000007F) << 1))
#define WLAFE_GET_PMC_SRC_STATE4(x)  ((u32)(((x >> 1) & 0x0000007F)))
#define WLAFE_BIT_PMC_STATE_JUMP_EN4 ((u32)0x00000001 << 0)            /*!< R/W 0x0  1: when current afe pmc state hit pmc_src_state4, next pmc state will jump to pmc_dst_state4;  0:afe pmc follow old flow*/
/** @} */

/** @defgroup WLAFE_AFE_PMC_PATCH_3
  * @brief
  * @{
  */
#define WLAFE_MASK_PMC_DST_STATE7    ((u32)0x0000007F << 24)           /*!< R/W 0x0  destination pmc state7 for state jump*/
#define WLAFE_PMC_DST_STATE7(x)      (((u32)((x) & 0x0000007F) << 24))
#define WLAFE_GET_PMC_DST_STATE7(x)  ((u32)(((x >> 24) & 0x0000007F)))
#define WLAFE_MASK_PMC_SRC_STATE7    ((u32)0x0000007F << 17)           /*!< R/W 0x0  source pmc state7 for state jump*/
#define WLAFE_PMC_SRC_STATE7(x)      (((u32)((x) & 0x0000007F) << 17))
#define WLAFE_GET_PMC_SRC_STATE7(x)  ((u32)(((x >> 17) & 0x0000007F)))
#define WLAFE_BIT_PMC_STATE_JUMP_EN7 ((u32)0x00000001 << 16)           /*!< R/W 0x0  1: when current afe pmc state hit pmc_src_state7, next pmc state will jump to pmc_dst_state7;  0:afe pmc follow old flow*/
#define WLAFE_MASK_PMC_DST_STATE6    ((u32)0x0000007F << 8)            /*!< R/W 0x0  destination pmc state6 for state jump*/
#define WLAFE_PMC_DST_STATE6(x)      (((u32)((x) & 0x0000007F) << 8))
#define WLAFE_GET_PMC_DST_STATE6(x)  ((u32)(((x >> 8) & 0x0000007F)))
#define WLAFE_MASK_PMC_SRC_STATE6    ((u32)0x0000007F << 1)            /*!< R/W 0x0  source pmc state6 for state jump*/
#define WLAFE_PMC_SRC_STATE6(x)      (((u32)((x) & 0x0000007F) << 1))
#define WLAFE_GET_PMC_SRC_STATE6(x)  ((u32)(((x >> 1) & 0x0000007F)))
#define WLAFE_BIT_PMC_STATE_JUMP_EN6 ((u32)0x00000001 << 0)            /*!< R/W 0x0  1: when current afe pmc state hit pmc_src_state6, next pmc state will jump to pmc_dst_state6;  0:afe pmc follow old flow*/
/** @} */

/** @defgroup WLAFE_AFE_PMC_PATCH_4
  * @brief
  * @{
  */
#define WLAFE_MASK_PATCH_STATE1_CMD_INFO   ((u32)0x000007FF << 20)           /*!< R/W 0x0  *for write command: [30]:write value [29:25]:write address bit select [24:20]: write address dword select *for delay command: [30:20]: delay time, unit is 1 cycle *for poll command: [25]:poll value  [24:20]: poll address bit select,poll 0x10 *for jump command: do nothing*/
#define WLAFE_PATCH_STATE1_CMD_INFO(x)     (((u32)((x) & 0x000007FF) << 20))
#define WLAFE_GET_PATCH_STATE1_CMD_INFO(x) ((u32)(((x >> 20) & 0x000007FF)))
#define WLAFE_BIT_PATCH_STATE1_RSM_BYPASS  ((u32)0x00000001 << 19)           /*!< R/W 0x0  bypass the state command for resume flow*/
#define WLAFE_BIT_PATCH_STATE1_PON_BYPASS  ((u32)0x00000001 << 18)           /*!< R/W 0x0  bypass the state command for pon flow*/
#define WLAFE_MASK_PATCH_STATE1_CMD_TYPE   ((u32)0x00000003 << 16)           /*!< R/W 0x11  2'b00: write command;  2'b01:delay command; 2'b10:poll command;2'b11:jump to next state*/
#define WLAFE_PATCH_STATE1_CMD_TYPE(x)     (((u32)((x) & 0x00000003) << 16))
#define WLAFE_GET_PATCH_STATE1_CMD_TYPE(x) ((u32)(((x >> 16) & 0x00000003)))
#define WLAFE_MASK_PATCH_STATE0_CMD_INFO   ((u32)0x000007FF << 4)            /*!< R/W 0x0  *for write command: [14]:write value [13:9]:write address bit select [8:4]: write address dword select *for delay command: [14:4]: delay time, unit is 1 cycle *for poll command: [9]:poll value  [8:4]: poll address bit select,poll 0x10 *for jump command: do nothing*/
#define WLAFE_PATCH_STATE0_CMD_INFO(x)     (((u32)((x) & 0x000007FF) << 4))
#define WLAFE_GET_PATCH_STATE0_CMD_INFO(x) ((u32)(((x >> 4) & 0x000007FF)))
#define WLAFE_BIT_PATCH_STATE0_RSM_BYPASS  ((u32)0x00000001 << 3)            /*!< R/W 0x0  bypass the state command for resume flow*/
#define WLAFE_BIT_PATCH_STATE0_PON_BYPASS  ((u32)0x00000001 << 2)            /*!< R/W 0x0  bypass the state command for pon flow*/
#define WLAFE_MASK_PATCH_STATE0_CMD_TYPE   ((u32)0x00000003 << 0)            /*!< R/W 0x11  2'b00: write command;  2'b01:delay command; 2'b10:poll command;2'b11:jump to next state*/
#define WLAFE_PATCH_STATE0_CMD_TYPE(x)     (((u32)((x) & 0x00000003) << 0))
#define WLAFE_GET_PATCH_STATE0_CMD_TYPE(x) ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup WLAFE_AFE_PMC_PATCH_5
  * @brief
  * @{
  */
#define WLAFE_MASK_PATCH_STATE3_CMD_INFO   ((u32)0x000007FF << 20)           /*!< R/W 0x0  *for write command: [30]:write value [29:25]:write address bit select [24:20]: write address dword select *for delay command: [30:20]: delay time, unit is 1 cycle *for poll command: [25]:poll value  [24:20]: poll address bit select,poll 0x10 *for jump command: do nothing*/
#define WLAFE_PATCH_STATE3_CMD_INFO(x)     (((u32)((x) & 0x000007FF) << 20))
#define WLAFE_GET_PATCH_STATE3_CMD_INFO(x) ((u32)(((x >> 20) & 0x000007FF)))
#define WLAFE_BIT_PATCH_STATE3_RSM_BYPASS  ((u32)0x00000001 << 19)           /*!< R/W 0x0  bypass the state command for resume flow*/
#define WLAFE_BIT_PATCH_STATE3_PON_BYPASS  ((u32)0x00000001 << 18)           /*!< R/W 0x0  bypass the state command for pon flow*/
#define WLAFE_MASK_PATCH_STATE3_CMD_TYPE   ((u32)0x00000003 << 16)           /*!< R/W 0x11  2'b00: write command;  2'b01:delay command; 2'b10:poll command;2'b11:jump to next state*/
#define WLAFE_PATCH_STATE3_CMD_TYPE(x)     (((u32)((x) & 0x00000003) << 16))
#define WLAFE_GET_PATCH_STATE3_CMD_TYPE(x) ((u32)(((x >> 16) & 0x00000003)))
#define WLAFE_MASK_PATCH_STATE2_CMD_INFO   ((u32)0x000007FF << 4)            /*!< R/W 0x0  *for write command: [14]:write value [13:9]:write address bit select [8:4]: write address dword select *for delay command: [14:4]: delay time, unit is 1 cycle *for poll command: [9]:poll value  [8:4]: poll address bit select,poll 0x10 *for jump command: do nothing*/
#define WLAFE_PATCH_STATE2_CMD_INFO(x)     (((u32)((x) & 0x000007FF) << 4))
#define WLAFE_GET_PATCH_STATE2_CMD_INFO(x) ((u32)(((x >> 4) & 0x000007FF)))
#define WLAFE_BIT_PATCH_STATE2_RSM_BYPASS  ((u32)0x00000001 << 3)            /*!< R/W 0x0  bypass the state command for resume flow*/
#define WLAFE_BIT_PATCH_STATE2_PON_BYPASS  ((u32)0x00000001 << 2)            /*!< R/W 0x0  bypass the state command for pon flow*/
#define WLAFE_MASK_PATCH_STATE2_CMD_TYPE   ((u32)0x00000003 << 0)            /*!< R/W 0x11  2'b00: write command;  2'b01:delay command; 2'b10:poll command;2'b11:jump to next state*/
#define WLAFE_PATCH_STATE2_CMD_TYPE(x)     (((u32)((x) & 0x00000003) << 0))
#define WLAFE_GET_PATCH_STATE2_CMD_TYPE(x) ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup WLAFE_AFE_PMC_PATCH_6
  * @brief
  * @{
  */
#define WLAFE_MASK_PATCH_STATE5_CMD_INFO   ((u32)0x000007FF << 20)           /*!< R/W 0x0  *for write command: [30]:write value [29:25]:write address bit select [24:20]: write address dword select *for delay command: [30:20]: delay time, unit is 1 cycle *for poll command: [25]:poll value  [24:20]: poll address bit select,poll 0x10 *for jump command: do nothing*/
#define WLAFE_PATCH_STATE5_CMD_INFO(x)     (((u32)((x) & 0x000007FF) << 20))
#define WLAFE_GET_PATCH_STATE5_CMD_INFO(x) ((u32)(((x >> 20) & 0x000007FF)))
#define WLAFE_BIT_PATCH_STATE5_RSM_BYPASS  ((u32)0x00000001 << 19)           /*!< R/W 0x0  bypass the state command for resume flow*/
#define WLAFE_BIT_PATCH_STATE5_PON_BYPASS  ((u32)0x00000001 << 18)           /*!< R/W 0x0  bypass the state command for pon flow*/
#define WLAFE_MASK_PATCH_STATE5_CMD_TYPE   ((u32)0x00000003 << 16)           /*!< R/W 0x11  2'b00: write command;  2'b01:delay command; 2'b10:poll command;2'b11:jump to next state*/
#define WLAFE_PATCH_STATE5_CMD_TYPE(x)     (((u32)((x) & 0x00000003) << 16))
#define WLAFE_GET_PATCH_STATE5_CMD_TYPE(x) ((u32)(((x >> 16) & 0x00000003)))
#define WLAFE_MASK_PATCH_STATE4_CMD_INFO   ((u32)0x000007FF << 4)            /*!< R/W 0x0  *for write command: [14]:write value [13:9]:write address bit select [8:4]: write address dword select *for delay command: [14:4]: delay time, unit is 1 cycle *for poll command: [9]:poll value  [8:4]: poll address bit select,poll 0x10 *for jump command: do nothing*/
#define WLAFE_PATCH_STATE4_CMD_INFO(x)     (((u32)((x) & 0x000007FF) << 4))
#define WLAFE_GET_PATCH_STATE4_CMD_INFO(x) ((u32)(((x >> 4) & 0x000007FF)))
#define WLAFE_BIT_PATCH_STATE4_RSM_BYPASS  ((u32)0x00000001 << 3)            /*!< R/W 0x0  bypass the state command for resume flow*/
#define WLAFE_BIT_PATCH_STATE4_PON_BYPASS  ((u32)0x00000001 << 2)            /*!< R/W 0x0  bypass the state command for pon flow*/
#define WLAFE_MASK_PATCH_STATE4_CMD_TYPE   ((u32)0x00000003 << 0)            /*!< R/W 0x11  2'b00: write command;  2'b01:delay command; 2'b10:poll command;2'b11:jump to next state*/
#define WLAFE_PATCH_STATE4_CMD_TYPE(x)     (((u32)((x) & 0x00000003) << 0))
#define WLAFE_GET_PATCH_STATE4_CMD_TYPE(x) ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup WLAFE_AFE_PMC_PATCH_7
  * @brief
  * @{
  */
#define WLAFE_MASK_PATCH_STATE7_CMD_INFO   ((u32)0x000007FF << 20)           /*!< R/W 0x0  *for write command: [30]:write value [29:25]:write address bit select [24:20]: write address dword select *for delay command: [30:20]: delay time, unit is 1 cycle *for poll command: [25]:poll value  [24:20]: poll address bit select,poll 0x10 *for jump command: do nothing*/
#define WLAFE_PATCH_STATE7_CMD_INFO(x)     (((u32)((x) & 0x000007FF) << 20))
#define WLAFE_GET_PATCH_STATE7_CMD_INFO(x) ((u32)(((x >> 20) & 0x000007FF)))
#define WLAFE_BIT_PATCH_STATE7_RSM_BYPASS  ((u32)0x00000001 << 19)           /*!< R/W 0x0  bypass the state command for resume flow*/
#define WLAFE_BIT_PATCH_STATE7_PON_BYPASS  ((u32)0x00000001 << 18)           /*!< R/W 0x0  bypass the state command for pon flow*/
#define WLAFE_MASK_PATCH_STATE7_CMD_TYPE   ((u32)0x00000003 << 16)           /*!< R/W 0x11  2'b00: write command;  2'b01:delay command; 2'b10:poll command;2'b11:jump to next state*/
#define WLAFE_PATCH_STATE7_CMD_TYPE(x)     (((u32)((x) & 0x00000003) << 16))
#define WLAFE_GET_PATCH_STATE7_CMD_TYPE(x) ((u32)(((x >> 16) & 0x00000003)))
#define WLAFE_MASK_PATCH_STATE6_CMD_INFO   ((u32)0x000007FF << 4)            /*!< R/W 0x0  *for write command: [14]:write value [13:9]:write address bit select [8:4]: write address dword select *for delay command: [14:4]: delay time, unit is 1 cycle *for poll command: [9]:poll value  [8:4]: poll address bit select,poll 0x10 *for jump command: do nothing*/
#define WLAFE_PATCH_STATE6_CMD_INFO(x)     (((u32)((x) & 0x000007FF) << 4))
#define WLAFE_GET_PATCH_STATE6_CMD_INFO(x) ((u32)(((x >> 4) & 0x000007FF)))
#define WLAFE_BIT_PATCH_STATE6_RSM_BYPASS  ((u32)0x00000001 << 3)            /*!< R/W 0x0  bypass the state command for resume flow*/
#define WLAFE_BIT_PATCH_STATE6_PON_BYPASS  ((u32)0x00000001 << 2)            /*!< R/W 0x0  bypass the state command for pon flow*/
#define WLAFE_MASK_PATCH_STATE6_CMD_TYPE   ((u32)0x00000003 << 0)            /*!< R/W 0x11  2'b00: write command;  2'b01:delay command; 2'b10:poll command;2'b11:jump to next state*/
#define WLAFE_PATCH_STATE6_CMD_TYPE(x)     (((u32)((x) & 0x00000003) << 0))
#define WLAFE_GET_PATCH_STATE6_CMD_TYPE(x) ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup WLAFE_Exported_Types WLAFE Exported Types
  * @{
  */

/** @brief WLAFE Register Declaration
  */

typedef struct {
	__IO uint32_t WLAFE_ANAPAR_POW_MAC_0;  /*!< Register,  Address offset:0x000 */
	__IO uint32_t RSVD0[2]              ;  /*!< Reserved,  Address offset:0x004-0x00B */
	__IO uint32_t WLAFE_ANAPAR_MAC_2    ;  /*!< Register,  Address offset:0x00C */
	__I  uint32_t WLAFE_DBG_WLAFE_PMC   ;  /*!< Register,  Address offset:0x010 */
	__I  uint32_t WLAFE_DBG_WLAFE       ;  /*!< Register,  Address offset:0x014 */
	__IO uint32_t WLAFE_DUMMY           ;  /*!< Register,  Address offset:0x018 */
	__IO uint32_t RSVD1                 ;  /*!< Reserved,  Address offset:0x01C-0x01F */
	__IO uint32_t WLAFE_ANAPAR_RFC      ;  /*!< Register,  Address offset:0x020 */
	__IO uint32_t WLAFE_LDO_VPLUSE_SEL  ;  /*!< Register,  Address offset:0x024 */
	__IO uint32_t WLAFE_WLRFC_CTRL      ;  /*!< Register,  Address offset:0x028 */
	__IO uint32_t RSVD2[9]              ;  /*!< Reserved,  Address offset:0x02C-0x04F */
	__IO uint32_t WLAFE_AFE_PMC_PATCH_0 ;  /*!< Register,  Address offset:0x050 */
	__IO uint32_t WLAFE_AFE_PMC_PATCH_1 ;  /*!< Register,  Address offset:0x054 */
	__IO uint32_t WLAFE_AFE_PMC_PATCH_2 ;  /*!< Register,  Address offset:0x058 */
	__IO uint32_t WLAFE_AFE_PMC_PATCH_3 ;  /*!< Register,  Address offset:0x05C */
	__IO uint32_t WLAFE_AFE_PMC_PATCH_4 ;  /*!< Register,  Address offset:0x060 */
	__IO uint32_t WLAFE_AFE_PMC_PATCH_5 ;  /*!< Register,  Address offset:0x064 */
	__IO uint32_t WLAFE_AFE_PMC_PATCH_6 ;  /*!< Register,  Address offset:0x068 */
	__IO uint32_t WLAFE_AFE_PMC_PATCH_7 ;  /*!< Register,  Address offset:0x06C */
} WLAFE_TypeDef;

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

/** @} */

/** @} */