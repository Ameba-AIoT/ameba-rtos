/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSREG_SWR_H_
#define _SYSREG_SWR_H_

/* AUTO_GEN_START */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup SWR
* @brief SWR driver modules
* @{
*/

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup SWR_Register_Definitions SWR Register Definitions
 * @{
 *****************************************************************************/

/** @defgroup SWR_ANAPARSW_MAC_ON_0
 * @brief
 * @{
 **/
#define SWR_BIT_PFMCMP_IQ               ((u32)0x00000001 << 31)          /*!<R/W 1  PFM comparator bias, 0: 50nA 1:250nA */
#define SWR_BIT_EN_HLOAD                ((u32)0x00000001 << 30)          /*!<R/W 0  Enable Heavy load counter */
#define SWR_BIT_OFF_END_SEL             ((u32)0x00000001 << 29)          /*!<R/W 0  COT off time controlled by, 0: ZCD 1: min off time */
#define SWR_BIT_POW_MINOFF_L            ((u32)0x00000001 << 28)          /*!<R/W 1  0:normal current 1:increase OCP cmp current */
#define SWR_MASK_COT_I_L                ((u32)0x00000003 << 26)          /*!<R/W 01  COT ON Current 00:150nA 01:150nA+100nA 10:150nA+150nA 11:150nA+250nA */
#define SWR_COT_I_L(x)                  (((u32)((x) & 0x00000003) << 26))
#define SWR_GET_COT_I_L(x)              ((u32)(((x >> 26) & 0x00000003)))
#define SWR_MASK_VREFPFM_L              ((u32)0x0000000F << 22)          /*!<R/W/ES 1011  SWR/LDO SEL <93>=0 <93>=1 0000 0.495 0.458 0001 0.654 0.610 0010 0.707 0.662 0011 0.760 0.712 0100 0.795 0.746 0101 0.848 0.797 0110 0.883 0.831 0111 0.954 0.899 1000 1.042 0.984 1001 1.131 1.069 1010 1.219 1.153 1011 1.255 1.188 1100 1.308 1.238 1101 1.343 1.272 1110 1.414 1.340 1111 1.503 1.425 */
#define SWR_VREFPFM_L(x)                (((u32)((x) & 0x0000000F) << 22))
#define SWR_GET_VREFPFM_L(x)            ((u32)(((x >> 22) & 0x0000000F)))
#define SWR_BIT_FORCE_ZCD_BIAS          ((u32)0x00000001 << 21)          /*!<R/W 1  0:ZCD bias on after SS 1:ZCD bias on when ZCD enable */
#define SWR_MASK_ZCD_SDZ_L              ((u32)0x00000003 << 19)          /*!<R/W 01  Increase ZCD delay cell */
#define SWR_ZCD_SDZ_L(x)                (((u32)((x) & 0x00000003) << 19))
#define SWR_GET_ZCD_SDZ_L(x)            ((u32)(((x >> 19) & 0x00000003)))
#define SWR_MASK_REG_ZCDC_H             ((u32)0x00000003 << 17)          /*!<R 10  00:0.1u 01:0.2u(PFM) 10:0.7u(PWM) 11:0.8u */
#define SWR_REG_ZCDC_H(x)               (((u32)((x) & 0x00000003) << 17))
#define SWR_GET_REG_ZCDC_H(x)           ((u32)(((x >> 17) & 0x00000003)))
#define SWR_BIT_POW_ZCD_L               ((u32)0x00000001 << 16)          /*!<R/W 1  Enable ZCD */
#define SWR_MASK_OCP_L1                 ((u32)0x00000007 << 13)          /*!<R/W 100  PWM current (A) PFM current (A) mP 000 0.533 0.105 001 0.580 0.145 010 0.630 0.185 011 0.680 0.223 100 0.730 0.263 101 0.780 0.303 110 0.830 0.342 111 0.890 0.290(VIN/10) */
#define SWR_OCP_L1(x)                   (((u32)((x) & 0x00000007) << 13))
#define SWR_GET_OCP_L1(x)               ((u32)(((x >> 13) & 0x00000007)))
#define SWR_BIT_POWOCP_L1               ((u32)0x00000001 << 12)          /*!<R/W 1  POWOCP_L1=1=> enable over current protection */
#define SWR_MASK_SAW_FREQ_L             ((u32)0x0000000F << 8)          /*!<R 1000  0000 458.0k 0001 680.2k 0010 898.9k 0011 1.113M 0100 1.321M 0101 1.522M 0110 1.716M 0111 1.902M 1000 2.087M 1001 2.258M 1010 2.426M 1011 2.582M 1100 2.743M 1101 2.888M 1110 3.035M 1111 3.165M */
#define SWR_SAW_FREQ_L(x)               (((u32)((x) & 0x0000000F) << 8))
#define SWR_GET_SAW_FREQ_L(x)           ((u32)(((x >> 8) & 0x0000000F)))
#define SWR_BIT_REG_BYPASS_L            ((u32)0x00000001 << 7)          /*!<R/W 1  1:bypass SSR latch */
#define SWR_BIT_FPWM_L1                 ((u32)0x00000001 << 6)          /*!<R/W 1  FPWM_L1 = 1 => force PWM FPWM_L1 = 0 => force PFM */
#define SWR_MASK_STD_L1                 ((u32)0x00000003 << 4)          /*!<R/W 01  STD_L1<1:0> time 750u 1.5m 10 3m 11 6m */
#define SWR_STD_L1(x)                   (((u32)((x) & 0x00000003) << 4))
#define SWR_GET_STD_L1(x)               ((u32)(((x >> 4) & 0x00000003)))
#define SWR_MASK_VOL_L1                 ((u32)0x0000000F << 0)          /*!<R/W/ES 0101  VREF 0.6 0.635 0.835 0.865 Code VOUT VOUT VOUT VOUT 1111 1.028571 1.088571 1.431429 1.482857 1110 1 1.058333 1.391667 1.441667 1101 0.972973 1.02973 1.354054 1.402703 1100 0.947368 1.002632 1.318421 1.365789 1011 0.935065 0.98961 1.301299 1.348052 1010 0.923077 0.976923 1.284615 1.330769 1001 0.911392 0.964557 1.268354 1.313924 1000 0.9 0.9525 1.2525 1.2975 0111 0.888889 0.940741 1.237037 1.281481 0110 0.878049 0.929268 1.221951 1.265854 0101 0.86747 0.918072 1.207229 1.250602 0100 0.847059 0.896471 1.178824 1.221176 0011 0.827586 0.875862 1.151724 1.193103 0010 0.808989 0.85618 1.125843 1.166292 0001 0.791209 0.837363 1.101099 1.140659 0000 0.774194 0.819355 1.077419 1.116129 */
#define SWR_VOL_L1(x)                   (((u32)((x) & 0x0000000F) << 0))
#define SWR_GET_VOL_L1(x)               ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup SWR_ANAPARSW_MAC_ON_1
 * @brief
 * @{
 **/
#define SWR_MASK_SN_N_L                 ((u32)0x0000000F << 28)          /*!<R/W 1000  NGATE NMOS driving */
#define SWR_SN_N_L(x)                   (((u32)((x) & 0x0000000F) << 28))
#define SWR_GET_SN_N_L(x)               ((u32)(((x >> 28) & 0x0000000F)))
#define SWR_MASK_SP_N_L                 ((u32)0x0000000F << 24)          /*!<R/W 1000  NGATE PMOS driving */
#define SWR_SP_N_L(x)                   (((u32)((x) & 0x0000000F) << 24))
#define SWR_GET_SP_N_L(x)               ((u32)(((x >> 24) & 0x0000000F)))
#define SWR_MASK_SN_P_L                 ((u32)0x0000000F << 20)          /*!<R/W 1000  PGATE NMOS driving */
#define SWR_SN_P_L(x)                   (((u32)((x) & 0x0000000F) << 20))
#define SWR_GET_SN_P_L(x)               ((u32)(((x >> 20) & 0x0000000F)))
#define SWR_MASK_SP_P_L                 ((u32)0x0000000F << 16)          /*!<R/W 0011  PGATE PMOS driving */
#define SWR_SP_P_L(x)                   (((u32)((x) & 0x0000000F) << 16))
#define SWR_GET_SP_P_L(x)               ((u32)(((x >> 16) & 0x0000000F)))
#define SWR_BIT_VO_DISCHG_PWM_H         ((u32)0x00000001 << 15)          /*!<R/W 0  1:dummy loading 5mA */
#define SWR_BIT_REG_MODE_PREDRIVER      ((u32)0x00000001 << 14)          /*!<R/W 1  1:3 level predriver */
#define SWR_MASK_REG_ADJSLDO_L          ((u32)0x0000000F << 10)          /*!<R/W 0111  0000 1.043 0001 1.075 0010 1.108 0011 1.143 0100 1.180 0101 1.220 0110 1.263 0111 1.286 1000 1.309 1001 1.333 1010 1.359 1011 1.385 1100 1.412 1101 1.444 1110 1.469 1111 1.500 */
#define SWR_REG_ADJSLDO_L(x)            (((u32)((x) & 0x0000000F) << 10))
#define SWR_GET_REG_ADJSLDO_L(x)        ((u32)(((x >> 10) & 0x0000000F)))
#define SWR_BIT_REG_LDOR_L              ((u32)0x00000001 << 9)          /*!<R/W 1  1:ldos res short */
#define SWR_BIT_PWM_FORCE               ((u32)0x00000001 << 8)          /*!<R/W 0  0: PWM bias enabled only at PWM mode 1: Force PWM bias enable when in PWM/PFM mode */
#define SWR_BIT_PFM_PD_RST              ((u32)0x00000001 << 7)          /*!<R/W 1  1: When leave PFM mode, COT logic is reset */
#define SWR_BIT_VC_PFM_RSTB             ((u32)0x00000001 << 6)          /*!<R/W 0  When into PFM, VC is, 0: clamped (bit 40 also =1) 1. floating */
#define SWR_BIT_PFM_IN_SEL              ((u32)0x00000001 << 5)          /*!<R/W 1  Power MOS into PFM when PFM CTRL, 0: rising 1: falling */
#define SWR_BIT_VC_RSTB                 ((u32)0x00000001 << 4)          /*!<R/W 0  Before SS VC is, 0: pulled low 1: floating */
#define SWR_BIT_FPWMDELAY               ((u32)0x00000001 << 3)          /*!<R/W 0  1:PFM<=>PWM PFM/PWM change by using DFF(CKMOD) */
#define SWR_BIT_ENFPWMDELAY_H           ((u32)0x00000001 << 2)          /*!<R/W 0  1:PFM=>PWM delay PWM enable and wait one clock cycle then PFM off */
#define SWR_BIT_REG_MOS_HALF_L          ((u32)0x00000001 << 1)          /*!<R 0  1:power mos half */
#define SWR_BIT_CURRENT_SENSE_MOS       ((u32)0x00000001 << 0)          /*!<R/W 0  1: current sensing MOS for large OC current */
/** @} */

/** @defgroup SWR_ANAPARSW_MAC_ON_2
 * @brief
 * @{
 **/
#define SWR_BIT_HLOAD_CUNT_SEL          ((u32)0x00000001 << 31)          /*!<R/W 0  0: Time counter=5 1: Time counter=9 */
#define SWR_BIT_HLOAD_PLUS              ((u32)0x00000001 << 30)          /*!<R/W 0  Detect More Heavy Load */
#define SWR_BIT_VDIV_PFM_SEL            ((u32)0x00000001 << 29)          /*!<R/W 0  搭配mac_on<25:22>,default=0; 1為向下微調 */
#define SWR_BIT_VOFB_SEL                ((u32)0x00000001 << 28)          /*!<R/W 0  0:Feedback from REGU Internal PAD for 1.25V/1.35V 1:Feedback from External PAD for 0.9V/1.0V */
#define SWR_MASK_PWM_VREF_SEL           ((u32)0x00000003 << 26)          /*!<R 11  00:VREF=0.600 01:VREF=0.635 10:VREF=0.835 11:VREF=0.865 */
#define SWR_PWM_VREF_SEL(x)             (((u32)((x) & 0x00000003) << 26))
#define SWR_GET_PWM_VREF_SEL(x)         ((u32)(((x >> 26) & 0x00000003)))
#define SWR_MASK_REG_SWR_ZCD_CTRL       ((u32)0x00000003 << 24)          /*!<R 00  // SWR ZCD CTRL control <1>1:double current <0>1'b1: force in CCM/DCM boundary mode(default), 1'b0 : no boundary */
#define SWR_REG_SWR_ZCD_CTRL(x)         (((u32)((x) & 0x00000003) << 24))
#define SWR_GET_REG_SWR_ZCD_CTRL(x)     ((u32)(((x >> 24) & 0x00000003)))
#define SWR_MASK_BG_VR1200_TUNE         ((u32)0x00000007 << 21)          /*!<R/W 100  BG電壓調整 */
#define SWR_BG_VR1200_TUNE(x)           (((u32)((x) & 0x00000007) << 21))
#define SWR_GET_BG_VR1200_TUNE(x)       ((u32)(((x >> 21) & 0x00000007)))
#define SWR_MASK_BG_R2_TUEN             ((u32)0x00000007 << 18)          /*!<R/W 100  BG R2電阻調整 */
#define SWR_BG_R2_TUEN(x)               (((u32)((x) & 0x00000007) << 18))
#define SWR_GET_BG_R2_TUEN(x)           ((u32)(((x >> 18) & 0x00000007)))
#define SWR_MASK_BG_R1_TUNE             ((u32)0x00000007 << 15)          /*!<R/W 100  BG R1電阻調整 */
#define SWR_BG_R1_TUNE(x)               (((u32)((x) & 0x00000007) << 15))
#define SWR_GET_BG_R1_TUNE(x)           ((u32)(((x >> 15) & 0x00000007)))
#define SWR_MASK_ZCD_IOFFSET_B          ((u32)0x00000003 << 13)          /*!<R 10  00=>+600nA 01=>+400nA 10=>+200nA 11=>0nA */
#define SWR_ZCD_IOFFSET_B(x)            (((u32)((x) & 0x00000003) << 13))
#define SWR_GET_ZCD_IOFFSET_B(x)        ((u32)(((x >> 13) & 0x00000003)))
#define SWR_BIT_REG_PRSS_H              ((u32)0x00000001 << 12)          /*!<R 0  0:default 1:res+cap improve psrr */
#define SWR_MASK_R2_L1                  ((u32)0x00000003 << 10)          /*!<R/W 11  00 => 0.4M 01 => 0.3M 10 => 0.2M 11 => 0.1M */
#define SWR_R2_L1(x)                    (((u32)((x) & 0x00000003) << 10))
#define SWR_GET_R2_L1(x)                ((u32)(((x >> 10) & 0x00000003)))
#define SWR_MASK_C3_L1                  ((u32)0x00000003 << 8)          /*!<R/W 11  00 => 1.59p 01 => 3.2p 10 => 4.76p 11 => 6.34p */
#define SWR_C3_L1(x)                    (((u32)((x) & 0x00000003) << 8))
#define SWR_GET_C3_L1(x)                ((u32)(((x >> 8) & 0x00000003)))
#define SWR_MASK_C1_L1                  ((u32)0x00000003 << 6)          /*!<R/W 01  00 => 1.03p 01 => 2.06p 10 => 3.1p 11 => 4.12p */
#define SWR_C1_L1(x)                    (((u32)((x) & 0x00000003) << 6))
#define SWR_GET_C1_L1(x)                ((u32)(((x >> 6) & 0x00000003)))
#define SWR_BIT_EXTERNAL_CLK_SEL        ((u32)0x00000001 << 5)          /*!<R/W 0  0:internal clock 1:external clock */
#define SWR_MASK_REG_BG_CURRENT_L       ((u32)0x00000003 << 3)          /*!<R/W 11  00:23.9n 01:54.6n 10:117.8n */
#define SWR_REG_BG_CURRENT_L(x)         (((u32)((x) & 0x00000003) << 3))
#define SWR_GET_REG_BG_CURRENT_L(x)     ((u32)(((x >> 3) & 0x00000003)))
#define SWR_BIT_REG_BG_ANA_L            ((u32)0x00000001 << 2)          /*!<R/W 0  1:External SWR BG switch on */
#define SWR_BIT_REG_BG_DIG_L            ((u32)0x00000001 << 1)          /*!<R/W 1  1:Local SWR BG switch on */
#define SWR_BIT_REG_VC_CLAMP_SEL_L      ((u32)0x00000001 << 0)          /*!<R/W 1  0:unity gain feedback clamp VC=VREF 1.orignal clamp(roll back option) */
/** @} */

/** @defgroup SWR_ANAPARSW_MAC_ON_3
 * @brief
 * @{
 **/
#define SWR_BIT_ZCDBIAS_MINUS_PFM       ((u32)0x00000001 << 31)          /*!<R/W 0   */
#define SWR_BIT_ZCDC_MUX_SEL_PFM        ((u32)0x00000001 << 30)          /*!<R/W 0   */
#define SWR_MASK_NULL0                  ((u32)0x0000003F << 24)          /*!<R/W 0   */
#define SWR_NULL0(x)                    (((u32)((x) & 0x0000003F) << 24))
#define SWR_GET_NULL0(x)                ((u32)(((x >> 24) & 0x0000003F)))
#define SWR_MASK_BOD1_VL_SEL            ((u32)0x0000001F << 19)          /*!<R/W 11011   */
#define SWR_BOD1_VL_SEL(x)              (((u32)((x) & 0x0000001F) << 19))
#define SWR_GET_BOD1_VL_SEL(x)          ((u32)(((x >> 19) & 0x0000001F)))
#define SWR_MASK_BOD1_VH_SEL            ((u32)0x0000001F << 14)          /*!<R/W 10111   */
#define SWR_BOD1_VH_SEL(x)              (((u32)((x) & 0x0000001F) << 14))
#define SWR_GET_BOD1_VH_SEL(x)          ((u32)(((x >> 14) & 0x0000001F)))
#define SWR_MASK_BOD0_VL_SEL            ((u32)0x0000001F << 9)          /*!<R/W 1100   */
#define SWR_BOD0_VL_SEL(x)              (((u32)((x) & 0x0000001F) << 9))
#define SWR_GET_BOD0_VL_SEL(x)          ((u32)(((x >> 9) & 0x0000001F)))
#define SWR_MASK_BOD0_VH_SEL            ((u32)0x0000001F << 4)          /*!<R/W 111   */
#define SWR_BOD0_VH_SEL(x)              (((u32)((x) & 0x0000001F) << 4))
#define SWR_GET_BOD0_VH_SEL(x)          ((u32)(((x >> 4) & 0x0000001F)))
#define SWR_MASK_EN_BOD                 ((u32)0x0000000F << 0)          /*!<R/W 0  Enable BOD<3:0> */
#define SWR_EN_BOD(x)                   (((u32)((x) & 0x0000000F) << 0))
#define SWR_GET_EN_BOD(x)               ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup SWR_ANAPARSW_MAC_ON_4
 * @brief
 * @{
 **/
#define SWR_BIT_ZCDBIAS_PLUS_PFM        ((u32)0x00000001 << 0)          /*!<R/W 0   */
#define SWR_BIT_EN_HLOAD_PFM            ((u32)0x00000001 << 1)          /*!<R/W 0   */
#define SWR_MASK_OCP_L1_PFM             ((u32)0x00000007 << 2)          /*!<R/W 0   */
#define SWR_OCP_L1_PFM(x)               (((u32)((x) & 0x00000007) << 2))
#define SWR_GET_OCP_L1_PFM(x)           ((u32)(((x >> 2) & 0x00000007)))
#define SWR_MASK_NULL1_PRE              ((u32)0x0000007F << 5)          /*!<R/W 00000   */
#define SWR_NULL1_PRE(x)                (((u32)((x) & 0x0000007F) << 5))
#define SWR_GET_NULL1_PRE(x)            ((u32)(((x >> 5) & 0x0000007F)))
#define SWR_MASK_ZCD_VTUNE              ((u32)0x0000000F << 12)          /*!<R/W 0   */
#define SWR_ZCD_VTUNE(x)                (((u32)((x) & 0x0000000F) << 12))
#define SWR_GET_ZCD_VTUNE(x)            ((u32)(((x >> 12) & 0x0000000F)))
#define SWR_MASK_NULL1                  ((u32)0x0000FFFF << 16)          /*!<R/W 00000   */
#define SWR_NULL1(x)                    (((u32)((x) & 0x0000FFFF) << 16))
#define SWR_GET_NULL1(x)                ((u32)(((x >> 16) & 0x0000FFFF)))
/** @} */

/** @defgroup SWR_ANAPARSW_MAC_ON_5
 * @brief
 * @{
 **/
#define SWR_MASK_NULL2                  ((u32)0x000000FF << 0)          /*!<R/W 8'hFF   */
#define SWR_NULL2(x)                    (((u32)((x) & 0x000000FF) << 0))
#define SWR_GET_NULL2(x)                ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SWR_ANAPARSW_MAC_OFF_0
 * @brief
 * @{
 **/
#define SWR_MASK_SDZN_L                 ((u32)0x00000003 << 30)          /*!<R/W 01  Nonoverlap delay cell for NMOS */
#define SWR_SDZN_L(x)                   (((u32)((x) & 0x00000003) << 30))
#define SWR_GET_SDZN_L(x)               ((u32)(((x >> 30) & 0x00000003)))
#define SWR_BIT_REG_AUTOZCD_L           ((u32)0x00000001 << 29)          /*!<R/W 0  0:register 1:10000 */
#define SWR_BIT_REG_VOFB_SEL            ((u32)0x00000001 << 28)          /*!<R/W 0  0:vo15 1:LX_SPS */
#define SWR_MASK_TBOX_L1                ((u32)0x00000003 << 26)          /*!<R/W 00  TBOX_L1<1:0> output 00 CKOVER/RAMPOVER 01 NI/PI 10 CKSS/SSOVER 11 i/NIOFF_H */
#define SWR_TBOX_L1(x)                  (((u32)((x) & 0x00000003) << 26))
#define SWR_GET_TBOX_L1(x)              ((u32)(((x >> 26) & 0x00000003)))
#define SWR_BIT_ENOCPMUX_L              ((u32)0x00000001 << 25)          /*!<R/W 1  1:OCP signal control by MUX 0:OCP signal control by I */
#define SWR_BIT_FORCE_LDOS              ((u32)0x00000001 << 24)          /*!<R/W 0  1:LDOS force on */
#define SWR_BIT_VO_DISCHG               ((u32)0x00000001 << 23)          /*!<R/W 0  1:VO with 1mA discharge */
#define SWR_BIT_LDO_OC_CLAMP            ((u32)0x00000001 << 22)          /*!<R/W 1  0:OC clamp with P/NMOS 1:OC clamp with PMOS */
#define SWR_BIT_MINOFF_LIQ              ((u32)0x00000001 << 21)          /*!<R/W 1  0:Min. off circuit always on 1:Min. off circuit pulse on */
#define SWR_BIT_MINON_LIQ               ((u32)0x00000001 << 20)          /*!<R/W 1  0:Min. on circuit always on 1:Min. on circuit pulse on */
#define SWR_BIT_POW_AUTO_L              ((u32)0x00000001 << 19)          /*!<R/W 0  1:auto PFM/PWM 0:register control */
#define SWR_BIT_ARENB_H                 ((u32)0x00000001 << 18)          /*!<R/W 1  1:off 0:on */
#define SWR_BIT_NO_OFFTIME_L            ((u32)0x00000001 << 17)          /*!<R/W 0  0:from min OFF time(PFM COT) 1:always on */
#define SWR_BIT_EN_ON_END_L             ((u32)0x00000001 << 16)          /*!<R/W 0  1:EN min OFF time (PFM COT) */
#define SWR_BIT_ENCOT_L                 ((u32)0x00000001 << 15)          /*!<R/W 1  1:PFM COT */
#define SWR_MASK_REG_CLK_SEL            ((u32)0x00000003 << 13)          /*!<R/W 11  00:out of phase CKSS 01:out of phase PWM_CTRIL 10:ckout 11:HG */
#define SWR_REG_CLK_SEL(x)              (((u32)((x) & 0x00000003) << 13))
#define SWR_GET_REG_CLK_SEL(x)          ((u32)(((x >> 13) & 0x00000003)))
#define SWR_BIT_REG_TYPE_L              ((u32)0x00000001 << 12)          /*!<R/W 1  1:type3 0:type2 */
#define SWR_MASK_R3_L1                  ((u32)0x00000003 << 10)          /*!<R/W 10  00 => 36k 01 => 30k 10 => 24k 11 => 24k */
#define SWR_R3_L1(x)                    (((u32)((x) & 0x00000003) << 10))
#define SWR_GET_R3_L1(x)                ((u32)(((x >> 10) & 0x00000003)))
#define SWR_MASK_NULL7                  ((u32)0x00000003 << 8)          /*!<R/W 10   */
#define SWR_NULL7(x)                    (((u32)((x) & 0x00000003) << 8))
#define SWR_GET_NULL7(x)                ((u32)(((x >> 8) & 0x00000003)))
#define SWR_MASK_R1_L1                  ((u32)0x00000003 << 6)          /*!<R/W 10  00 => 900k 01 => 720k 10 => 900k 11 => 1080k */
#define SWR_R1_L1(x)                    (((u32)((x) & 0x00000003) << 6))
#define SWR_GET_R1_L1(x)                ((u32)(((x >> 6) & 0x00000003)))
#define SWR_MASK_NULL6                  ((u32)0x00000003 << 4)          /*!<R/W 10   */
#define SWR_NULL6(x)                    (((u32)((x) & 0x00000003) << 4))
#define SWR_GET_NULL6(x)                ((u32)(((x >> 4) & 0x00000003)))
#define SWR_MASK_C2_L1                  ((u32)0x00000003 << 2)          /*!<R/W 10  00 => 13f 01 => 26f 10 => 39f 11 => 41f */
#define SWR_C2_L1(x)                    (((u32)((x) & 0x00000003) << 2))
#define SWR_GET_C2_L1(x)                ((u32)(((x >> 2) & 0x00000003)))
#define SWR_MASK_NULL5                  ((u32)0x00000003 << 0)          /*!<R/W 10   */
#define SWR_NULL5(x)                    (((u32)((x) & 0x00000003) << 0))
#define SWR_GET_NULL5(x)                ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup SWR_ANAPARSW_MAC_OFF_1
 * @brief
 * @{
 **/
#define SWR_MASK_NULL9                  ((u32)0x000000FF << 8)          /*!<R/W 11000000  dummy */
#define SWR_NULL9(x)                    (((u32)((x) & 0x000000FF) << 8))
#define SWR_GET_NULL9(x)                ((u32)(((x >> 8) & 0x000000FF)))
#define SWR_MASK_NULL8                  ((u32)0x00000003 << 6)          /*!<R/W 11  dummy */
#define SWR_NULL8(x)                    (((u32)((x) & 0x00000003) << 6))
#define SWR_GET_NULL8(x)                ((u32)(((x >> 6) & 0x00000003)))
#define SWR_BIT_REG_NMOS_OFF_L          ((u32)0x00000001 << 5)          /*!<R 0  0:normal 1:NMOS always off */
#define SWR_BIT_REG_MUX_PI_L            ((u32)0x00000001 << 4)          /*!<R/W 0  1:PI (original) 0:i */
#define SWR_BIT_REG_PWM_CTRL_L          ((u32)0x00000001 << 3)          /*!<R/W 0  0:internal PWM 1:external PWM */
#define SWR_BIT_ENSR_L                  ((u32)0x00000001 << 2)          /*!<R/W 1  1:with SSR latch */
#define SWR_MASK_SDZP_L                 ((u32)0x00000003 << 0)          /*!<R/W 01  Nonoverlap delay cell for PMOS */
#define SWR_SDZP_L(x)                   (((u32)((x) & 0x00000003) << 0))
#define SWR_GET_SDZP_L(x)               ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup SWR_DIG_OCP
 * @brief
 * @{
 **/
#define SWR_BIT_SPS_DIG_OCP_CKE         ((u32)0x00000001 << 31)          /*!<R/W 0  SPS OCP clock enable */
#define SWR_MASK_SPS_DIG_OCP_THD        ((u32)0x00007FFF << 16)          /*!<R/W 0  SPS OCP event threshold */
#define SWR_SPS_DIG_OCP_THD(x)          (((u32)((x) & 0x00007FFF) << 16))
#define SWR_GET_SPS_DIG_OCP_THD(x)      ((u32)(((x >> 16) & 0x00007FFF)))
#define SWR_MASK_SPS_DIG_OCP_WND        ((u32)0x0000FFFF << 0)          /*!<R/W 0  SPS OCP time window */
#define SWR_SPS_DIG_OCP_WND(x)          (((u32)((x) & 0x0000FFFF) << 0))
#define SWR_GET_SPS_DIG_OCP_WND(x)      ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup SWR_DIG_ZCD
 * @brief
 * @{
 **/
#define SWR_MASK_SPS_DIG_ZCD_DBG        ((u32)0x0000001F << 16)          /*!<R 0  The acture zcd code used for SPS core */
#define SWR_SPS_DIG_ZCD_DBG(x)          (((u32)((x) & 0x0000001F) << 16))
#define SWR_GET_SPS_DIG_ZCD_DBG(x)      ((u32)(((x >> 16) & 0x0000001F)))
#define SWR_BIT_SPS_DIG_ZCD_MODE        ((u32)0x00000001 << 9)          /*!<R/W 0  SPS ZCD SW mode enable */
#define SWR_BIT_SPS_DIG_ZCD_AUTO        ((u32)0x00000001 << 8)          /*!<R/W 1  SPS HW ZCD code auto count function enable */
#define SWR_MASK_SPS_DIG_ZCD_CODE       ((u32)0x0000001F << 0)          /*!<R/W 10000  SPS_DIG_ZCD_CODE,this is used for SW mode */
#define SWR_SPS_DIG_ZCD_CODE(x)         (((u32)((x) & 0x0000001F) << 0))
#define SWR_GET_SPS_DIG_ZCD_CODE(x)     ((u32)(((x >> 0) & 0x0000001F)))
/** @} */

/** @defgroup SWR_CTRL
 * @brief
 * @{
 **/
#define SWR_BIT_PWM_LOAD                ((u32)0x00000001 << 4)          /*!<R 0  SWR pwm loading(scan value is 0) 1:heavy 0:light */
#define SWR_BIT_MODE_SRC                ((u32)0x00000001 << 1)          /*!<R/W 0  Select SWR mode ctrl source to determine SWR parameter 1: from Analog mode flag , which is 0x50[0] 0: from D->A mode control bit (Note , scan value is 1:PWM) */
#define SWR_BIT_DCORE_MODE_L            ((u32)0x00000001 << 0)          /*!<R 0  SWR mode flag 1: PWM 0: PFM */
/** @} */

/** @defgroup SWR_PARAM_PFM
 * @brief
 * @{
 **/
#define SWR_MASK_REG_ZCDC_H_PFM         ((u32)0x00000003 << 12)          /*!<R/W 10  00:0.1u 01:0.2u(PFM) 10:0.7u(PWM) 11:0.8u */
#define SWR_REG_ZCDC_H_PFM(x)           (((u32)((x) & 0x00000003) << 12))
#define SWR_GET_REG_ZCDC_H_PFM(x)       ((u32)(((x >> 12) & 0x00000003)))
#define SWR_MASK_SAW_FREQ_L_PFM         ((u32)0x0000000F << 8)          /*!<R/W 1000  0000 458.0k 0001 680.2k 0010 898.9k 0011 1.113M 0100 1.321M 0101 1.522M 0110 1.716M 0111 1.902M 1000 2.087M 1001 2.258M 1010 2.426M 1011 2.582M 1100 2.743M 1101 2.888M 1110 3.035M 1111 3.165M */
#define SWR_SAW_FREQ_L_PFM(x)           (((u32)((x) & 0x0000000F) << 8))
#define SWR_GET_SAW_FREQ_L_PFM(x)       ((u32)(((x >> 8) & 0x0000000F)))
#define SWR_BIT_REG_MOS_HALF_L_PFM      ((u32)0x00000001 << 6)          /*!<R/W 0  1:power mos half */
#define SWR_MASK_REG_SWR_ZCD_CTRL_PFM   ((u32)0x00000003 << 4)          /*!<R/W 00  // SWR ZCD CTRL control <1>1:double current <0>1'b1: force in CCM/DCM boundary mode(default), 1'b0 : no boundary */
#define SWR_REG_SWR_ZCD_CTRL_PFM(x)     (((u32)((x) & 0x00000003) << 4))
#define SWR_GET_REG_SWR_ZCD_CTRL_PFM(x) ((u32)(((x >> 4) & 0x00000003)))
#define SWR_MASK_ZCD_IOFFSET_B_PFM      ((u32)0x00000003 << 2)          /*!<R/W 10  00=>+600nA 01=>+400nA 10=>+200nA 11=>0nA */
#define SWR_ZCD_IOFFSET_B_PFM(x)        (((u32)((x) & 0x00000003) << 2))
#define SWR_GET_ZCD_IOFFSET_B_PFM(x)    ((u32)(((x >> 2) & 0x00000003)))
#define SWR_BIT_REG_PRSS_H_PFM          ((u32)0x00000001 << 1)          /*!<R/W 0  0:default 1:res+cap improve psrr */
#define SWR_BIT_REG_NMOS_OFF_L_PFM      ((u32)0x00000001 << 0)          /*!<R/W 0  0:normal 1:NMOS always off */
/** @} */

/** @defgroup SWR_PARAM_PWML
 * @brief
 * @{
 **/
#define SWR_MASK_REG_ZCDC_H_PWML        ((u32)0x00000003 << 12)          /*!<R/W 10  00:0.1u 01:0.2u(PFM) 10:0.7u(PWM) 11:0.8u */
#define SWR_REG_ZCDC_H_PWML(x)          (((u32)((x) & 0x00000003) << 12))
#define SWR_GET_REG_ZCDC_H_PWML(x)      ((u32)(((x >> 12) & 0x00000003)))
#define SWR_MASK_SAW_FREQ_L_PWML        ((u32)0x0000000F << 8)          /*!<R/W 1000  0000 458.0k 0001 680.2k 0010 898.9k 0011 1.113M 0100 1.321M 0101 1.522M 0110 1.716M 0111 1.902M 1000 2.087M 1001 2.258M 1010 2.426M 1011 2.582M 1100 2.743M 1101 2.888M 1110 3.035M 1111 3.165M */
#define SWR_SAW_FREQ_L_PWML(x)          (((u32)((x) & 0x0000000F) << 8))
#define SWR_GET_SAW_FREQ_L_PWML(x)      ((u32)(((x >> 8) & 0x0000000F)))
#define SWR_BIT_REG_MOS_HALF_L_PWML     ((u32)0x00000001 << 6)          /*!<R/W 0  1:power mos half */
#define SWR_MASK_REG_SWR_ZCD_CTRL_PWML  ((u32)0x00000003 << 4)          /*!<R/W 00  // SWR ZCD CTRL control <1>1:double current <0>1'b1: force in CCM/DCM boundary mode(default), 1'b0 : no boundary */
#define SWR_REG_SWR_ZCD_CTRL_PWML(x)    (((u32)((x) & 0x00000003) << 4))
#define SWR_GET_REG_SWR_ZCD_CTRL_PWML(x) ((u32)(((x >> 4) & 0x00000003)))
#define SWR_MASK_ZCD_IOFFSET_B_PWML     ((u32)0x00000003 << 2)          /*!<R/W 10  00=>+600nA 01=>+400nA 10=>+200nA 11=>0nA */
#define SWR_ZCD_IOFFSET_B_PWML(x)       (((u32)((x) & 0x00000003) << 2))
#define SWR_GET_ZCD_IOFFSET_B_PWML(x)   ((u32)(((x >> 2) & 0x00000003)))
#define SWR_BIT_REG_PRSS_H_PWML         ((u32)0x00000001 << 1)          /*!<R/W 0  0:default 1:res+cap improve psrr */
#define SWR_BIT_REG_NMOS_OFF_L_PWML     ((u32)0x00000001 << 0)          /*!<R/W 0  0:normal 1:NMOS always off */
/** @} */

/** @defgroup SWR_PARAM_PWMH
 * @brief
 * @{
 **/
#define SWR_MASK_REG_ZCDC_H_PWMH        ((u32)0x00000003 << 12)          /*!<R/W 10  00:0.1u 01:0.2u(PFM) 10:0.7u(PWM) 11:0.8u */
#define SWR_REG_ZCDC_H_PWMH(x)          (((u32)((x) & 0x00000003) << 12))
#define SWR_GET_REG_ZCDC_H_PWMH(x)      ((u32)(((x >> 12) & 0x00000003)))
#define SWR_MASK_SAW_FREQ_L_PWMH        ((u32)0x0000000F << 8)          /*!<R/W 1000  0000 458.0k 0001 680.2k 0010 898.9k 0011 1.113M 0100 1.321M 0101 1.522M 0110 1.716M 0111 1.902M 1000 2.087M 1001 2.258M 1010 2.426M 1011 2.582M 1100 2.743M 1101 2.888M 1110 3.035M 1111 3.165M */
#define SWR_SAW_FREQ_L_PWMH(x)          (((u32)((x) & 0x0000000F) << 8))
#define SWR_GET_SAW_FREQ_L_PWMH(x)      ((u32)(((x >> 8) & 0x0000000F)))
#define SWR_BIT_REG_MOS_HALF_L_PWMH     ((u32)0x00000001 << 6)          /*!<R/W 0  1:power mos half */
#define SWR_MASK_REG_SWR_ZCD_CTRL_PWMH  ((u32)0x00000003 << 4)          /*!<R/W 00  // SWR ZCD CTRL control <1>1:double current <0>1'b1: force in CCM/DCM boundary mode(default), 1'b0 : no boundary */
#define SWR_REG_SWR_ZCD_CTRL_PWMH(x)    (((u32)((x) & 0x00000003) << 4))
#define SWR_GET_REG_SWR_ZCD_CTRL_PWMH(x) ((u32)(((x >> 4) & 0x00000003)))
#define SWR_MASK_ZCD_IOFFSET_B_PWMH     ((u32)0x00000003 << 2)          /*!<R/W 10  00=>+600nA 01=>+400nA 10=>+200nA 11=>0nA */
#define SWR_ZCD_IOFFSET_B_PWMH(x)       (((u32)((x) & 0x00000003) << 2))
#define SWR_GET_ZCD_IOFFSET_B_PWMH(x)   ((u32)(((x >> 2) & 0x00000003)))
#define SWR_BIT_REG_PRSS_H_PWMH         ((u32)0x00000001 << 1)          /*!<R/W 0  0:default 1:res+cap improve psrr */
#define SWR_BIT_REG_NMOS_OFF_L_PWMH     ((u32)0x00000001 << 0)          /*!<R/W 0  0:normal 1:NMOS always off */
/** @} */

/** @defgroup SWR_PWM_VREF
 * @brief
 * @{
 **/
#define SWR_MASK_PWM_VREF_SEL_SNOZ      ((u32)0x00000003 << 2)          /*!<R/W 2'b00  For snooze mode power , it use pwm 0.8v and CLDO pc mode , to select PWM 0.8v , it must change vref */
#define SWR_PWM_VREF_SEL_SNOZ(x)        (((u32)((x) & 0x00000003) << 2))
#define SWR_GET_PWM_VREF_SEL_SNOZ(x)    ((u32)(((x >> 2) & 0x00000003)))
#define SWR_MASK_PWM_VREF_SEL_NORM      ((u32)0x00000003 << 0)          /*!<R/W 2'b11  PWM default vref */
#define SWR_PWM_VREF_SEL_NORM(x)        (((u32)((x) & 0x00000003) << 0))
#define SWR_GET_PWM_VREF_SEL_NORM(x)    ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup SWR_ZCD_ITUNE
 * @brief
 * @{
 **/
#define SWR_BIT_ZCD_ITUNE_SEL           ((u32)0x00000001 << 8)          /*!<R/W 1'h0  Select itune code source 1: from software config 0: auto by regu self */
#define SWR_MASK_SW_ZCD_ITUNE           ((u32)0x0000000F << 4)          /*!<R/W 4'h0  Software config zcd itune code */
#define SWR_SW_ZCD_ITUNE(x)             (((u32)((x) & 0x0000000F) << 4))
#define SWR_GET_SW_ZCD_ITUNE(x)         ((u32)(((x >> 4) & 0x0000000F)))
#define SWR_MASK_ZCD_ITUNE_OUT_L        ((u32)0x0000000F << 0)          /*!<R 4'h0  SWR output ZCD itune code */
#define SWR_ZCD_ITUNE_OUT_L(x)          (((u32)((x) & 0x0000000F) << 0))
#define SWR_GET_ZCD_ITUNE_OUT_L(x)      ((u32)(((x >> 0) & 0x0000000F)))
/** @} */
/** @} */



/* Exported types --------------------------------------------------------*/

/** @defgroup SWR_Exported_Types SWR Exported Types
  * @{
*****************************************************************************/


/**
 * @brief SWR Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t SWR_ANAPARSW_MAC_ON_0;                  /*!<  Register,  Address offset: 0x000 */
	__IO uint32_t SWR_ANAPARSW_MAC_ON_1;                  /*!<  Register,  Address offset: 0x004 */
	__IO uint32_t SWR_ANAPARSW_MAC_ON_2;                  /*!<  Register,  Address offset: 0x008 */
	__IO uint32_t SWR_ANAPARSW_MAC_ON_3;                  /*!<  Register,  Address offset: 0x00C */
	__IO uint32_t SWR_ANAPARSW_MAC_ON_4;                  /*!<  Register,  Address offset: 0x010 */
	__IO uint32_t SWR_ANAPARSW_MAC_ON_5;                  /*!<  Register,  Address offset: 0x014 */
	__IO uint32_t RSVD0[2];                               /*!<  Reserved,  Address offset:0x18-0x1C */
	__IO uint32_t SWR_ANAPARSW_MAC_OFF_0;                 /*!<  Register,  Address offset: 0x020 */
	__IO uint32_t SWR_ANAPARSW_MAC_OFF_1;                 /*!<  Register,  Address offset: 0x024 */
	__IO uint32_t RSVD1[6];                               /*!<  Reserved,  Address offset:0x28-0x3C */
	__IO uint32_t SWR_DIG_OCP;                            /*!<  Register,  Address offset: 0x040 */
	__IO uint32_t SWR_DIG_ZCD;                            /*!<  Register,  Address offset: 0x044 */
	__IO uint32_t RSVD2[2];                               /*!<  Reserved,  Address offset:0x48-0x4C */
	__IO uint32_t SWR_CTRL;                               /*!<  Register,  Address offset: 0x050 */
	__IO uint32_t SWR_PARAM_PFM;                          /*!<  Register,  Address offset: 0x054 */
	__IO uint32_t SWR_PARAM_PWML;                         /*!<  Register,  Address offset: 0x058 */
	__IO uint32_t SWR_PARAM_PWMH;                         /*!<  Register,  Address offset: 0x05C */
	__IO uint32_t SWR_PWM_VREF;                           /*!<  Register,  Address offset: 0x060 */
	__IO uint32_t SWR_ZCD_ITUNE;                          /*!<  Register,  Address offset: 0x064 */
	__IO uint32_t RSVD3[5];                               /*!<  Reserved,  Address offset:0x68-0x78 */
	__IO uint32_t SWR_DUMMY;                              /*!<  Register,  Address offset: 0x07C */
} SWR_TypeDef;

#define SWR_BASE ((SWR_TypeDef *) (0x41008180))

/** @} */
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here
/** @defgroup SWR_MODE SWR mode
  * @{
  */
#define SWR_PFM							((u32)0x00000000)
#define SWR_PWM							((u32)0x00000001)

#define IS_SWR_MODE(MODE) (((MODE) == SWR_PFM) || \
										((MODE) == SWR_PWM))


#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif
