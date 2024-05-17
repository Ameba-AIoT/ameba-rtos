/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSREG_PLL_H_
#define _SYSREG_PLL_H_

/* AUTO_GEN_START */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup PLL
* @brief PLL driver modules
* @{
*/

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup PLL_Register_Definitions PLL Register Definitions
 * @{
 *****************************************************************************/

/** @defgroup PLL_CTRL0
 * @brief
 * @{
 **/
#define PLL_BIT_POW_ERC                   ((u32)0x00000001 << 31)          /*!<R/W 0  erc enable. 0 → 1 */
#define PLL_BIT_NPLL_POW_NOUSE            ((u32)0x00000001 << 30)          /*!<R/W 0  no use:input divider enable (need pwl signal) 0 → 1 */
#define PLL_BIT_POW_PLL                   ((u32)0x00000001 << 29)          /*!<R/W 0  (need pwl signal) 0 → 1 */
#define PLL_MASK_REG_CP_BIA               ((u32)0x00000007 << 26)          /*!<R/W 000  charge pump current selection . Default=10uA CP current select : 000: Icp=10uA, 001: Icp=15uA, 010: Icp=20uA, 011: Icp=25uA; 100: Icp=30uA, 101: Icp=35uA, 110: Icp=40uA, 111: Icp=450uA; */
#define PLL_REG_CP_BIA(x)                 (((u32)((x) & 0x00000007) << 26))
#define PLL_GET_REG_CP_BIA(x)             ((u32)(((x >> 26) & 0x00000007)))
#define PLL_BIT_DOGENB                    ((u32)0x00000001 << 25)          /*!<R/W 0  0: watch dog on, 1: watch dog off */
#define PLL_MASK_REG_PS                   ((u32)0x00000007 << 22)          /*!<R/W 000   */
#define PLL_REG_PS(x)                     (((u32)((x) & 0x00000007) << 22))
#define PLL_GET_REG_PS(x)                 ((u32)(((x >> 22) & 0x00000007)))
#define PLL_BIT_REG_PSEN                  ((u32)0x00000001 << 21)          /*!<R/W 1  phase selector enable */
#define PLL_MASK_REG_CP_SET               ((u32)0x00000003 << 19)          /*!<R/W 10  Cp selection. Default=5p 00: Cp=4p, 01: Cp=5p, 10: Cp=6p, 11: Cp=7p XTAL=40MHz : 10 ,Cp=6p XTAL=26MHz : 01 ,Cp=5p */
#define PLL_REG_CP_SET(x)                 (((u32)((x) & 0x00000003) << 19))
#define PLL_GET_REG_CP_SET(x)             ((u32)(((x >> 19) & 0x00000003)))
#define PLL_MASK_REG_RS_SET               ((u32)0x00000007 << 16)          /*!<R/W 110  Rs selection. Default=4k 000: Rs=15k, 001: Rs=13k, 010: Rs=11k, 011: Rs=9k, 100: Rs=7k, 101: Rs=5k, 110: Rs=3k, 111: Rs=1k; XTAL=40MHz : 110 ,Rs=3k XTAL=26MHz : 101 ,Rs=5K */
#define PLL_REG_RS_SET(x)                 (((u32)((x) & 0x00000007) << 16))
#define PLL_GET_REG_RS_SET(x)             ((u32)(((x >> 16) & 0x00000007)))
#define PLL_MASK_REG_CS_SEL               ((u32)0x00000003 << 14)          /*!<R/W 10  Cs selection. Default=70p 00: Cs=40p, 01: Cs=50p, 10: Cs=60p, 11: Cs=70p XTAL=40MHz : 11 , Cs=70p XTAL=26MHz : 11 , Cs=70p */
#define PLL_REG_CS_SEL(x)                 (((u32)((x) & 0x00000003) << 14))
#define PLL_GET_REG_CS_SEL(x)             ((u32)(((x >> 14) & 0x00000003)))
#define PLL_BIT_REG_FREF_EDGE             ((u32)0x00000001 << 13)          /*!<R/W 0  0:fref, 1:frefb */
#define PLL_BIT_REG_CK_EN                 ((u32)0x00000001 << 12)          /*!<R/W 1  output clk enable */
#define PLL_BIT_REG_CK_EN_D4              ((u32)0x00000001 << 11)          /*!<R/W 0  output clk div 4 enable */
#define PLL_BIT_REG_XTAL_MANUAL_MODE      ((u32)0x00000001 << 7)          /*!<R/W 1  reg_xtal_maunal_mode. Default=0 0: reg_xtal_sel<3:0> is controlled by REG, xtal_div_sel is controlled by mapping table. 1: reg_xtal_sel<3:0>=4'b1111, xtal_div_sel is controlled by reg_xtal_div_sel_maunal<1:0> */
#define PLL_MASK_REG_XTAL_DIV_SEL_MANUAL  ((u32)0x00000003 << 5)          /*!<R/W 00  input divider selection in xtal maunal mode. Default=00 00=/1, 10=/2, 11=/4 */
#define PLL_REG_XTAL_DIV_SEL_MANUAL(x)    (((u32)((x) & 0x00000003) << 5))
#define PLL_GET_REG_XTAL_DIV_SEL_MANUAL(x) ((u32)(((x >> 5) & 0x00000003)))
#define PLL_MASK_REG_VC_THL               ((u32)0x00000003 << 3)          /*!<R/W 00  VC voltage, Default=00 00: VC=1/4Vdd, 01: VC=1/2Vdd, 10=3/4Vdd, 11: VC=Vdd */
#define PLL_REG_VC_THL(x)                 (((u32)((x) & 0x00000003) << 3))
#define PLL_GET_REG_VC_THL(x)             ((u32)(((x >> 3) & 0x00000003)))
#define PLL_MASK_C3_SET                   ((u32)0x00000003 << 1)          /*!<R/W 01  C3 selection. Default=1p 00: 0.5p, 01: 1p, 10: Cs=1.5p, 11: 2p */
#define PLL_C3_SET(x)                     (((u32)((x) & 0x00000003) << 1))
#define PLL_GET_C3_SET(x)                 ((u32)(((x >> 1) & 0x00000003)))
#define PLL_BIT_R3_SET                    ((u32)0x00000001 << 0)          /*!<R/W 1  Rs selection. Default=1k 0: 0 Ohm, 1: 1k Ohm */
/** @} */

/** @defgroup PLL_CTRL1
 * @brief
 * @{
 **/
#define PLL_MASK_REG_XTAL_SEL             ((u32)0x0000000F << 28)          /*!<R/W 0000  xtal selection, Default=40MHz 0000= 40MHz, 0101= 26MHz, */
#define PLL_REG_XTAL_SEL(x)               (((u32)((x) & 0x0000000F) << 28))
#define PLL_GET_REG_XTAL_SEL(x)           ((u32)(((x >> 28) & 0x0000000F)))
#define PLL_MASK_REG_FREQ_SEL             ((u32)0x0000000F << 24)          /*!<R/W 0000  no use :output clk selection, Default=400MHz 0000= 400MHz, 0001= X, 0010= X, 0011= X, 0100= 216MHz, 0101=196.608MHz, 0110=192MHz, 0111= 180.632MHz, 1000= 148.5MHz,1001= X, 1010= X, 1011= X, 1100= X, 1101= X, 1110= X, 1111= 500MHz */
#define PLL_REG_FREQ_SEL(x)               (((u32)((x) & 0x0000000F) << 24))
#define PLL_GET_REG_FREQ_SEL(x)           ((u32)(((x >> 24) & 0x0000000F)))
#define PLL_MASK_TBASE_FREQ               ((u32)0x0000000F << 20)          /*!<R/W 1000  now is empty */
#define PLL_TBASE_FREQ(x)                 (((u32)((x) & 0x0000000F) << 20))
#define PLL_GET_TBASE_FREQ(x)             ((u32)(((x >> 20) & 0x0000000F)))
#define PLL_MASK_STEP_FREQ                ((u32)0x0000000F << 16)          /*!<R/W 0101  step size selection for switching freq. 0000: 2^5 0001: 2^6 0010: 2^7 0011: 2^8 0100: 2^9 0101: 2^10 0110: 2^11 0111: 2^12 */
#define PLL_STEP_FREQ(x)                  (((u32)((x) & 0x0000000F) << 16))
#define PLL_GET_STEP_FREQ(x)              ((u32)(((x >> 16) & 0x0000000F)))
#define PLL_BIT_TRIG_FREQ                 ((u32)0x00000001 << 15)          /*!<R/W 0  freq. step up or step down enable */
#define PLL_MASK_BB_DBG_SEL_AFE_SDM       ((u32)0x0000000F << 11)          /*!<R/W 0000   */
#define PLL_BB_DBG_SEL_AFE_SDM(x)         (((u32)((x) & 0x0000000F) << 11))
#define PLL_GET_BB_DBG_SEL_AFE_SDM(x)     ((u32)(((x >> 11) & 0x0000000F)))
#define PLL_MASK_DIVN_SDM                 ((u32)0x0000003F << 5)          /*!<R/W 1000  Default 320MHz: 000110 400MHz: 001000 600MHz: 001101 */
#define PLL_DIVN_SDM(x)                   (((u32)((x) & 0x0000003F) << 5))
#define PLL_GET_DIVN_SDM(x)               ((u32)(((x >> 5) & 0x0000003F)))
#define PLL_MASK_REG_CK_OUT_SEL           ((u32)0x0000000F << 1)          /*!<R/W 0  output divider selection. Default=0000(/1) 0000=/1, 1000=/2, 1100=/4, 1110=/8, 1111=/16 300~330MHz PLL output clock, output divider selection. 1000=/2 Ex : 600MHz/2=300MHz 330~660MHz PLL output clock, output divider selection. 0000=/1 Ex : 660MHz/1=660MHz Spec:/2(mandatory */
#define PLL_REG_CK_OUT_SEL(x)             (((u32)((x) & 0x0000000F) << 1))
#define PLL_GET_REG_CK_OUT_SEL(x)         ((u32)(((x >> 1) & 0x0000000F)))
#define PLL_BIT_ICP_HALF                  ((u32)0x00000001 << 0)          /*!<R/W 1  charge pump current half or not 0: no half 1: half */
/** @} */

/** @defgroup PLL_CTRL2
 * @brief
 * @{
 **/
#define PLL_MASK_STEP                     ((u32)0x00001FFF << 19)          /*!<R/W 0  ssc step size selection. */
#define PLL_STEP(x)                       (((u32)((x) & 0x00001FFF) << 19))
#define PLL_GET_STEP(x)                   ((u32)(((x >> 19) & 0x00001FFF)))
#define PLL_MASK_TBASE                    ((u32)0x00000FFF << 7)          /*!<R/W 0  ssc freq selection */
#define PLL_TBASE(x)                      (((u32)((x) & 0x00000FFF) << 7))
#define PLL_GET_TBASE(x)                  ((u32)(((x >> 7) & 0x00000FFF)))
#define PLL_BIT_EN_SSC                    ((u32)0x00000001 << 6)          /*!<R/W 0  ssc enable. Default=0 */
#define PLL_MASK_DMY1                     ((u32)0x0000003F << 0)          /*!<R/W 0  HW used only , these are for reserved */
#define PLL_DMY1(x)                       (((u32)((x) & 0x0000003F) << 0))
#define PLL_GET_DMY1(x)                   ((u32)(((x >> 0) & 0x0000003F)))
/** @} */

/** @defgroup PLL_CTRL3
 * @brief
 * @{
 **/
#define PLL_MASK_F0F_SDM                  ((u32)0x00001FFF << 19)          /*!<R/W 0  Default 660MHz: 0000000000000 330MHz: 0000000000000 600MHz: 0000000000000 660MHz: 0000000000000 */
#define PLL_F0F_SDM(x)                    (((u32)((x) & 0x00001FFF) << 19))
#define PLL_GET_F0F_SDM(x)                ((u32)(((x >> 19) & 0x00001FFF)))
#define PLL_MASK_F0N_SDM                  ((u32)0x00000007 << 16)          /*!<R/W 0  Default 660MHz: 100 330MHz: 010 600MHz: 000 660MHz: 100 */
#define PLL_F0N_SDM(x)                    (((u32)((x) & 0x00000007) << 16))
#define PLL_GET_F0N_SDM(x)                ((u32)(((x >> 16) & 0x00000007)))
#define PLL_BIT_ORDER_SDM                 ((u32)0x00000001 << 15)          /*!<R/W 0  SDM order */
#define PLL_MASK_DMY2                     ((u32)0x00007FFF << 0)          /*!<R/W 0  HW used only , these are for reserved */
#define PLL_DMY2(x)                       (((u32)((x) & 0x00007FFF) << 0))
#define PLL_GET_DMY2(x)                   ((u32)(((x >> 0) & 0x00007FFF)))
/** @} */

/** @defgroup PLL_TEST
 * @brief
 * @{
 **/
#define PLL_BIT_POW_ERC_TEST              ((u32)0x00000001 << 15)          /*!<R/W 1'b1  1 for enable test, 0 for disable */
#define PLL_BIT_DMY3                      ((u32)0x00000001 << 14)          /*!<R/W 1'b0  dummy */
#define PLL_BIT_DMY4                      ((u32)0x00000001 << 13)          /*!<R/W 1'b0  0 for ckmon div 4 , 1 for ckmon div 8 */
#define PLL_BIT_CKMON_SEL                 ((u32)0x00000001 << 12)          /*!<R/W 1'b1  0 for CKMON, 1 for CKMOND div by 4 */
#define PLL_BIT_POW_DIV4                  ((u32)0x00000001 << 11)          /*!<R/W 1'b1  1 for ckmon div 4 , 0 for disable div 4 */
#define PLL_BIT_ISO                       ((u32)0x00000001 << 10)          /*!<R/W 1'b0  1 for ISO ck_fref/ck_frefb */
#define PLL_BIT_REF_CLK_SEL               ((u32)0x00000001 << 9)          /*!<R/W 1'b0  xtal clk from 0 special route 1 APR */
#define PLL_BIT_CK_MON_EN                 ((u32)0x00000001 << 8)          /*!<R/W 1'b0  ckmon enable */
#define PLL_MASK_CK_SEL                   ((u32)0x0000000F << 4)          /*!<R/W 4'b0000  ckmon clk sel 1111:TL 1110:TL 1101:CK_CPUD4_I 1100:CK_400M_CPU_I 1011:TL 1010:TL 1001:TL 1000:READY_CPU_I 0111:ck_frefi_peri(xtal) 0110:TL 0101:TL 0100:TL 0011:TL 0010:TL 0001:TL 0000:TL */
#define PLL_CK_SEL(x)                     (((u32)((x) & 0x0000000F) << 4))
#define PLL_GET_CK_SEL(x)                 ((u32)(((x >> 4) & 0x0000000F)))
#define PLL_MASK_S                        ((u32)0x00000003 << 2)          /*!<R/W 2'b11  AGPIO driving sel */
#define PLL_S(x)                          (((u32)((x) & 0x00000003) << 2))
#define PLL_GET_S(x)                      ((u32)(((x >> 2) & 0x00000003)))
#define PLL_BIT_GPO                       ((u32)0x00000001 << 1)          /*!<R/W 1'b0  AGPIO output enable */
#define PLL_BIT_GPE                       ((u32)0x00000001 << 0)          /*!<R/W 1'b0  AGPIO enable */
/** @} */

/** @defgroup PLL_PS
 * @brief
 * @{
 **/
#define PLL_BIT_DMY5                      ((u32)0x00000001 << 15)          /*!<R/W 0  HW used only , these are for reserved */
#define PLL_BIT_POW_CKGEN                 ((u32)0x00000001 << 14)          /*!<R/W 0  pow of ckgen */
#define PLL_MASK_REG_CKGEN_DIV            ((u32)0x000001FF << 5)          /*!<R/W 000111010  DIV9B , N=DIV<8:0>+2 */
#define PLL_REG_CKGEN_DIV(x)              (((u32)((x) & 0x000001FF) << 5))
#define PLL_GET_REG_CKGEN_DIV(x)          ((u32)(((x >> 5) & 0x000001FF)))
#define PLL_BIT_EN_CK400M_PS              ((u32)0x00000001 << 4)          /*!<R/W 1  enable shifted clk source for flash */
#define PLL_BIT_EN_CK400M                 ((u32)0x00000001 << 3)          /*!<R/W 1  ck400m is PLL VCO freq (300M~600M) */
#define PLL_MASK_PHASE_SEL                ((u32)0x00000007 << 0)          /*!<R/W 000  AGPIO driving sel */
#define PLL_PHASE_SEL(x)                  (((u32)((x) & 0x00000007) << 0))
#define PLL_GET_PHASE_SEL(x)              ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup PLL_AUX_BG
 * @brief
 * @{
 **/
#define PLL_BIT_POW_BG                    ((u32)0x00000001 << 0)          /*!<R/W/ES 0  power on BG */
#define PLL_BIT_POW_I                     ((u32)0x00000001 << 1)          /*!<R/W/ES 1  power on BG current */
#define PLL_BIT_POW_MBIAS                 ((u32)0x00000001 << 2)          /*!<R/W/ES 0  power on mbias */
#define PLL_MASK_BG_DMY1                  ((u32)0x0000001F << 3)          /*!<R/W 0  HW used only , these are for reserved */
#define PLL_BG_DMY1(x)                    (((u32)((x) & 0x0000001F) << 3))
#define PLL_GET_BG_DMY1(x)                ((u32)(((x >> 3) & 0x0000001F)))
#define PLL_MASK_R2_TUNE                  ((u32)0x00000007 << 8)          /*!<R/W 11  Add 20k per step fine tune BG */
#define PLL_R2_TUNE(x)                    (((u32)((x) & 0x00000007) << 8))
#define PLL_GET_R2_TUNE(x)                ((u32)(((x >> 8) & 0x00000007)))
#define PLL_MASK_BG_DMY2                  ((u32)0x0000001F << 11)          /*!<R/W 0  HW used only , these are for reserved */
#define PLL_BG_DMY2(x)                    (((u32)((x) & 0x0000001F) << 11))
#define PLL_GET_BG_DMY2(x)                ((u32)(((x >> 11) & 0x0000001F)))
/** @} */

/** @defgroup PLL_TRNG
 * @brief
 * @{
 **/
#define PLL_BIT_TRNG_DMY                  ((u32)0x00000001 << 15)          /*!<R/W 0  dummy */
#define PLL_BIT_REG_INTCAP                ((u32)0x00000001 << 14)          /*!<R/W 0  OPA_INT cap<9:4> control */
#define PLL_BIT_REG_TRNG_POW              ((u32)0x00000001 << 13)          /*!<R/W 1  ROSC poewr on control 1:power on 0:power off */
#define PLL_MASK_REG_TRNG_FSEL            ((u32)0x00000003 << 11)          /*!<R/W 00  ROSC frequency range control */
#define PLL_REG_TRNG_FSEL(x)              (((u32)((x) & 0x00000003) << 11))
#define PLL_GET_REG_TRNG_FSEL(x)          ((u32)(((x >> 11) & 0x00000003)))
#define PLL_MASK_REG_TRNG_DIV             ((u32)0x00000003 << 9)          /*!<R/W 00  ROSC output clock divider control 00: divide by 1 01: divide by 2 10: divide by 3 11: divide by 4 */
#define PLL_REG_TRNG_DIV(x)               (((u32)((x) & 0x00000003) << 9))
#define PLL_GET_REG_TRNG_DIV(x)           ((u32)(((x >> 9) & 0x00000003)))
#define PLL_MASK_REG_TRNG_RESSEL          ((u32)0x00000003 << 7)          /*!<R/W 00  ROSC noise resistor control 00: 562k 01: 749k 10: 936k 11: 1123k */
#define PLL_REG_TRNG_RESSEL(x)            (((u32)((x) & 0x00000003) << 7))
#define PLL_GET_REG_TRNG_RESSEL(x)        ((u32)(((x >> 7) & 0x00000003)))
#define PLL_MASK_REG_TRNG_GAINSEL         ((u32)0x00000003 << 5)          /*!<R/W 10  ROSC noise gain control 00: 32dB 01: 35dB 10: 38dB 11: 40dB */
#define PLL_REG_TRNG_GAINSEL(x)           (((u32)((x) & 0x00000003) << 5))
#define PLL_GET_REG_TRNG_GAINSEL(x)       ((u32)(((x >> 5) & 0x00000003)))
#define PLL_MASK_REG_TRNG_ISEL            ((u32)0x0000000F << 1)          /*!<R/W 1000  ROSC frequency control data input */
#define PLL_REG_TRNG_ISEL(x)              (((u32)((x) & 0x0000000F) << 1))
#define PLL_GET_REG_TRNG_ISEL(x)          ((u32)(((x >> 1) & 0x0000000F)))
#define PLL_BIT_REG_TRNG_TEST             ((u32)0x00000001 << 0)          /*!<R/W 1  Analog test signal enable control 0: off 1: on */
/** @} */

/** @defgroup PLL_LPSD
 * @brief
 * @{
 **/
#define PLL_MASK_LPSD_DMY                 ((u32)0x0000003F << 14)          /*!<R/W 0  dummy */
#define PLL_LPSD_DMY(x)                   (((u32)((x) & 0x0000003F) << 14))
#define PLL_GET_LPSD_DMY(x)               ((u32)(((x >> 14) & 0x0000003F)))
#define PLL_BIT_EN_VREF0_L                ((u32)0x00000001 << 13)          /*!<R/W 0  set VREFL to 0 */
#define PLL_BIT_EN_VREF0_H                ((u32)0x00000001 << 12)          /*!<R/W 0  set VREFH to 0 */
#define PLL_MASK_SEL_VREFL                ((u32)0x0000001F << 7)          /*!<R 0  adjust VREFL */
#define PLL_SEL_VREFL(x)                  (((u32)((x) & 0x0000001F) << 7))
#define PLL_GET_SEL_VREFL(x)              ((u32)(((x >> 7) & 0x0000001F)))
#define PLL_MASK_SEL_VREFH                ((u32)0x0000001F << 2)          /*!<R 0  adjust VREFH */
#define PLL_SEL_VREFH(x)                  (((u32)((x) & 0x0000001F) << 2))
#define PLL_GET_SEL_VREFH(x)              ((u32)(((x >> 2) & 0x0000001F)))
#define PLL_BIT_SD_POSEDGE                ((u32)0x00000001 << 1)          /*!<R/W 0  1: vin>vref output high */
#define PLL_BIT_POW_SD                    ((u32)0x00000001 << 0)          /*!<R/W/ES 0  Power on signal detector */
/** @} */

/** @defgroup PLL_LPAD0
 * @brief
 * @{
 **/
#define PLL_MASK_VSET                     ((u32)0x00000003 << 30)          /*!<R/W 2'h2  11:1.89V 10:1.80V 01:1.71V 00:1.62V */
#define PLL_VSET(x)                       (((u32)((x) & 0x00000003) << 30))
#define PLL_GET_VSET(x)                   ((u32)(((x >> 30) & 0x00000003)))
#define PLL_BIT_SEL_LN                    ((u32)0x00000001 << 29)          /*!<R/W 1  1: comparator low noise for LSB 6bit */
#define PLL_BIT_SEL_LNA                   ((u32)0x00000001 << 28)          /*!<R/W 1  1: comparator low noise for ALL bit */
#define PLL_MASK_LPAD_DMY0                ((u32)0x00000007 << 25)          /*!<R/W 0  dummy */
#define PLL_LPAD_DMY0(x)                  (((u32)((x) & 0x00000007) << 25))
#define PLL_GET_LPAD_DMY0(x)              ((u32)(((x >> 25) & 0x00000007)))
#define PLL_BIT_EN_MODE_18                ((u32)0x00000001 << 24)          /*!<R/W 0  1: enable 1.8V mode */
#define PLL_BIT_REG_ENTG                  ((u32)0x00000001 << 23)          /*!<R/W 0  1: enable transmission gate */
#define PLL_MASK_BYPASSEN                 ((u32)0x0000007F << 16)          /*!<R/W 0  1:bypass mode, 0: divide mode */
#define PLL_BYPASSEN(x)                   (((u32)((x) & 0x0000007F) << 16))
#define PLL_GET_BYPASSEN(x)               ((u32)(((x >> 16) & 0x0000007F)))
#define PLL_BIT_REG_DIODE                 ((u32)0x00000001 << 15)          /*!<R/W 0  1: enable diode */
#define PLL_BIT_REG_META                  ((u32)0x00000001 << 14)          /*!<R/W 0  1: enable meta detect */
#define PLL_MASK_REG_CMPDEC               ((u32)0x00000003 << 12)          /*!<R/W 1  meta detect time 00: short */
#define PLL_REG_CMPDEC(x)                 (((u32)((x) & 0x00000003) << 12))
#define PLL_GET_REG_CMPDEC(x)             ((u32)(((x >> 12) & 0x00000003)))
#define PLL_BIT_REG_REV_CLK               ((u32)0x00000001 << 11)          /*!<R/W 0  1: rever ck_data */
#define PLL_MASK_VREF_SEL                 ((u32)0x00000007 << 8)          /*!<R/W 3'h2  000:0.800V 001:0.825V 010:0.850V 011:0.875V 100:0.900V 101:0.950V 110:1.000V 111:1.050V */
#define PLL_VREF_SEL(x)                   (((u32)((x) & 0x00000007) << 8))
#define PLL_GET_VREF_SEL(x)               ((u32)(((x >> 8) & 0x00000007)))
#define PLL_MASK_REG_LDO_SEL              ((u32)0x00000007 << 5)          /*!<R/W 3'h6  000:0.800V 001:0.825V 010:0.850V 011:0.875V 100:0.900V 101:0.950V 110:1.000V 111:1.050V */
#define PLL_REG_LDO_SEL(x)                (((u32)((x) & 0x00000007) << 5))
#define PLL_GET_REG_LDO_SEL(x)            ((u32)(((x >> 5) & 0x00000007)))
#define PLL_BIT_LPAD_DMY1                 ((u32)0x00000001 << 4)          /*!<R/W 0  dummy */
#define PLL_BIT_REG_CK                    ((u32)0x00000001 << 3)          /*!<R/W 0  1: clk from external */
#define PLL_BIT_POW_REF                   ((u32)0x00000001 << 2)          /*!<R/W/ES 0  pow adc refgen */
#define PLL_BIT_POW                       ((u32)0x00000001 << 1)          /*!<R/W/ES 0  pow adc */
#define PLL_BIT_POW_LDO                   ((u32)0x00000001 << 0)          /*!<R/W/ES 0  pow ldo */
/** @} */

/** @defgroup PLL_LPAD1
 * @brief
 * @{
 **/
#define PLL_MASK_LPAD_DMY2                ((u32)0x0000003F << 10)          /*!<R/W 0  dummy */
#define PLL_LPAD_DMY2(x)                  (((u32)((x) & 0x0000003F) << 10))
#define PLL_GET_LPAD_DMY2(x)              ((u32)(((x >> 10) & 0x0000003F)))
#define PLL_MASK_REG_VCMIN                ((u32)0x00000003 << 8)          /*!<R/W 1  00:0.4625V 01:0.4500V 10:0.4375V 11:0.4250V */
#define PLL_REG_VCMIN(x)                  (((u32)((x) & 0x00000003) << 8))
#define PLL_GET_REG_VCMIN(x)              ((u32)(((x >> 8) & 0x00000003)))
#define PLL_MASK_REG_MBIAS_SEL            ((u32)0x000000FF << 0)          /*!<R 0  00000000:0uA 00000001:0.25uA 00000010:0.5uA 00000011:0.75uA ………..…11111111:63.75uA */
#define PLL_REG_MBIAS_SEL(x)              (((u32)((x) & 0x000000FF) << 0))
#define PLL_GET_REG_MBIAS_SEL(x)          ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PLL_BG_PWR_ON_CNT
 * @brief
 * @{
 **/
#define PLL_MASK_BG_I_CNT                 ((u32)0x0000FFFF << 16)          /*!<R/W 16'h28   */
#define PLL_BG_I_CNT(x)                   (((u32)((x) & 0x0000FFFF) << 16))
#define PLL_GET_BG_I_CNT(x)               ((u32)(((x >> 16) & 0x0000FFFF)))
#define PLL_MASK_BG_ON_CNT                ((u32)0x0000FFFF << 0)          /*!<R/W 16'h280   */
#define PLL_BG_ON_CNT(x)                  (((u32)((x) & 0x0000FFFF) << 0))
#define PLL_GET_BG_ON_CNT(x)              ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup PLL_ADC_PWR_ON_CNT
 * @brief
 * @{
 **/
#define PLL_MASK_ADC_PWRON_CNT            ((u32)0x0000FFFF << 0)          /*!<R/W 16'h4B0   */
#define PLL_ADC_PWRON_CNT(x)              (((u32)((x) & 0x0000FFFF) << 0))
#define PLL_GET_ADC_PWRON_CNT(x)          ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup PLL_STATE
 * @brief
 * @{
 **/
#define PLL_BIT_CKRDY_PLL                 ((u32)0x00000001 << 0)          /*!<R   PLL clock is ready */
/** @} */

/** @defgroup PLL_DEBUG
 * @brief
 * @{
 **/
#define PLL_MASK_ADBG_AUX                 ((u32)0xFFFFFFFF << 0)          /*!<R   dummy */
#define PLL_ADBG_AUX(x)                   (((u32)((x) & 0xFFFFFFFF) << 0))
#define PLL_GET_ADBG_AUX(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */
/** @} */



/* Exported types --------------------------------------------------------*/

/** @defgroup PLL_Exported_Types PLL Exported Types
  * @{
*****************************************************************************/


/**
 * @brief PLL Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t PLL_CTRL0;                              /*!<  Register,  Address offset: 0x000 */
	__IO uint32_t PLL_CTRL1;                              /*!<  Register,  Address offset: 0x004 */
	__IO uint32_t PLL_CTRL2;                              /*!<  Register,  Address offset: 0x008 */
	__IO uint32_t PLL_CTRL3;                              /*!<  Register,  Address offset: 0x00C */
	__IO uint32_t PLL_TEST;                               /*!<  Register,  Address offset: 0x010 */
	__IO uint32_t PLL_PS;                                 /*!<  Register,  Address offset: 0x014 */
	__IO uint32_t PLL_AUX_BG;                             /*!<  Register,  Address offset: 0x018 */
	__IO uint32_t PLL_TRNG;                               /*!<  Register,  Address offset: 0x01C */
	__IO uint32_t PLL_LPSD;                               /*!<  Register,  Address offset: 0x020 */
	__IO uint32_t PLL_LPAD0;                              /*!<  Register,  Address offset: 0x024 */
	__IO uint32_t PLL_LPAD1;                              /*!<  Register,  Address offset: 0x028 */
	__IO uint32_t PLL_BG_PWR_ON_CNT;                      /*!<  Register,  Address offset: 0x02C */
	__IO uint32_t PLL_ADC_PWR_ON_CNT;                     /*!<  Register,  Address offset: 0x030 */
	__IO uint32_t PLL_STATE;                              /*!<  Register,  Address offset: 0x034 */
	__I  uint32_t PLL_DEBUG;                              /*!<  Register,  Address offset: 0x038 */
	__IO uint32_t PLL_DUMMY;                              /*!<  Register,  Address offset: 0x03C */
} PLL_TypeDef;

#define PLL_BASE ((PLL_TypeDef *) (0x41008440))

/** @} */
/* AUTO_GEN_END */

/* MANUAL_GEN_START */


/** @defgroup PLL_RS_SELECTION
  * @{
  */
#define PLL_RS_15K				0x0
#define PLL_RS_13K				0x1
#define PLL_RS_11K				0x2
#define PLL_RS_9K				0x3
#define PLL_RS_7K				0x4
#define PLL_RS_5K				0x5
#define PLL_RS_3K_XTAL_26M		0x6
#define PLL_RS_1K_XTAL_40M		0x7

/**
  * @}
  */


/* MANUAL_GEN_END */

#endif

/** @} */

/** @} */
