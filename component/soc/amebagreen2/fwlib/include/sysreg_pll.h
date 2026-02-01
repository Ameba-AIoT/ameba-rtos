/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SYSREG_PLL_H_
#define _SYSREG_PLL_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup PLL
  * @brief PLL driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup PLL_Register_Definitions PLL Register Definitions
  * @{
  */

/** @defgroup PLL_CTRL0
  * @brief
  * @{
  */
#define PLL_MASK_REG_PREDIV        ((u32)0x00000003 << 0)            /*!< R/W 2'b00  对PLL输入参考频率ckin进行pre_divide: 00:ckin/1 01:ckin/2 10:ckin/4 11:ckin/8*/
#define PLL_REG_PREDIV(x)          (((u32)((x) & 0x00000003) << 0))
#define PLL_GET_REG_PREDIV(x)      ((u32)(((x >> 0) & 0x00000003)))
#define PLL_BIT_REG_CMU_EN         ((u32)0x00000001 << 2)            /*!< R/W 1'b0  PLL的使能信号*/
#define PLL_BIT_REG_CCO_SEL        ((u32)0x00000001 << 3)            /*!< R/W 1'b1  控制选择CCO mode还是VCO mode 0: vco mode 1: cco mode*/
#define PLL_BIT_REG_BIG_CCO        ((u32)0x00000001 << 4)            /*!< R/W 1'b0  控制KVCO曲线斜率*/
#define PLL_MASK_REG_ICP           ((u32)0x00000003 << 5)            /*!< R/W 2'b01  CP电流ICP选择: 00:ICP=5u 01:ICP=10u 10:ICP=15u 11:ICP=20u*/
#define PLL_REG_ICP(x)             (((u32)((x) & 0x00000003) << 5))
#define PLL_GET_REG_ICP(x)         ((u32)(((x >> 5) & 0x00000003)))
#define PLL_BIT_REG_LPF_CP         ((u32)0x00000001 << 7)            /*!< R/W 1'b0  lpf小电容CS选择: 0:CS=1p 1:CS=2p*/
#define PLL_MASK_REG_LF_R          ((u32)0x00000007 << 8)            /*!< R/W 3'b100  lpf电阻RS选择: 000: 3.1KΩ ,    001: 6.1KΩ , 010: 9.2KΩ ,    011: 12.2KΩ, 100: 15.2KΩ,   101: 18.2KΩ, 110: 21.3KΩ,   111: 24.3KΩ*/
#define PLL_REG_LF_R(x)            (((u32)((x) & 0x00000007) << 8))
#define PLL_GET_REG_LF_R(x)        ((u32)(((x >> 8) & 0x00000007)))
#define PLL_MASK_REG_CMU_PI_ISEL   ((u32)0x0000000F << 12)           /*!< R/W 4'b0011  选择PI电流ISEL的大小: 0000:ICP=3.125u      1000:ICP=28.125u 0001:ICP=6.25u        1001:ICP=31.25u 0010:ICP=9.375u      1010:ICP=34.375u 0011:ICP=12.5u        1011:ICP=37.5u 0100:ICP=15.625u    1100:ICP=40.625u 0101:ICP=18.75u      1101:ICP=43.75u 0110:ICP=21.875u    1110:ICP=46.875u 0111:ICP=25u           1111:ICP=50u*/
#define PLL_REG_CMU_PI_ISEL(x)     (((u32)((x) & 0x0000000F) << 12))
#define PLL_GET_REG_CMU_PI_ISEL(x) ((u32)(((x >> 12) & 0x0000000F)))
#define PLL_BIT_REG_TEST_EN        ((u32)0x00000001 << 16)           /*!< R/W 1'b0  DEBUG信号*/
#define PLL_MASK_CMU_DBG           ((u32)0x00000007 << 20)           /*!< R   */
#define PLL_CMU_DBG(x)             (((u32)((x) & 0x00000007) << 20))
#define PLL_GET_CMU_DBG(x)         ((u32)(((x >> 20) & 0x00000007)))
/** @} */

/** @defgroup PLL_SSC_CTRL0
  * @brief
  * @{
  */
#define PLL_MASK_NCODE_T_SCPU   ((u32)0x0000001F << 0)           /*!< R/W 5'd8  VCO target频率对应的 Ncode*/
#define PLL_NCODE_T_SCPU(x)     (((u32)((x) & 0x0000001F) << 0))
#define PLL_GET_NCODE_T_SCPU(x) ((u32)(((x >> 0) & 0x0000001F)))
#define PLL_MASK_FCODE_T_SCPU   ((u32)0x00003FFF << 5)           /*!< R/W 14'd0  VCO target频率对应的 Fcode*/
#define PLL_FCODE_T_SCPU(x)     (((u32)((x) & 0x00003FFF) << 5))
#define PLL_GET_FCODE_T_SCPU(x) ((u32)(((x >> 5) & 0x00003FFF)))
/** @} */

/** @defgroup PLL_SSC_CTRL1
  * @brief
  * @{
  */
#define PLL_MASK_NCODE_SSC   ((u32)0x0000001F << 0)           /*!< R/W 5'd8  SSC低点频率对应的Ncode SSC时,频率会在"target频率" 与 "SSC低点频率"之间以三角波形式变化. Note: SSC低点 频率要小于target频率*/
#define PLL_NCODE_SSC(x)     (((u32)((x) & 0x0000001F) << 0))
#define PLL_GET_NCODE_SSC(x) ((u32)(((x >> 0) & 0x0000001F)))
#define PLL_MASK_FCODE_SSC   ((u32)0x00003FFF << 5)           /*!< R/W 14'd0  SSC低点频率对应的Fcode SSC时,频率会在"target频率" 与 "SSC低点频率"之间以三角波形式变化. Note: SSC低点 频率要小于target频率*/
#define PLL_FCODE_SSC(x)     (((u32)((x) & 0x00003FFF) << 5))
#define PLL_GET_FCODE_SSC(x) ((u32)(((x >> 5) & 0x00003FFF)))
/** @} */

/** @defgroup PLL_SSC_CTRL2
  * @brief
  * @{
  */
#define PLL_BIT_FLAG_INITIAL           ((u32)0x00000001 << 0)            /*!< R/W 0x0  线性超频控制信号 0: 不执行线性超频,OC_EN 0->1 toggle后,直接把NFcode 吃进来 1: 执行线性超频,OC_EN 0->1 toggle后,按设定的线性超频模式逐渐超频到target NF 值*/
#define PLL_BIT_OC_EN                  ((u32)0x00000001 << 11)           /*!< R/W 0x0  OC_EN 0->1 toggle ,才会把NCODE_T_SCPU[7:0], FCODE_T_SCPU[12:0], NCODE_SSC[7:0], FCODE_SSC[12:0],GRAN_EST[20:0] load进来*/
#define PLL_MASK_REG_SEL_OC_MODE       ((u32)0x00000003 << 12)           /*!< R/W 2'b00  线性超频模式: 00: 以每次跳变REG_OC_STEP_SET[9:0]的方式线性超频到target值.      a)如果 "当前NF 值" - "target NF 值" > 与REG_SEL_OC_STEP_SET[9:0], 每次跳变REG_SEL_OC_STEP_SET[9:0]      b) 如果 "当前NF 值" - "target NF 值" <= 与REG_SEL_OC_STEP_SET[9:0], 直接超频到target NF 值 01: 先直接一步超频到 "target NF code" - delta,  然后按mode 00 方式继续超频. delta 大小由REG_HS_OC_STOP_DIFF[1:0] 设定 10: 先直接一步讲{Ncode[7:0], Fcode[12:6]}超频到 target值,然后按mode 00 方式继续超频 11: 同mode 10*/
#define PLL_REG_SEL_OC_MODE(x)         (((u32)((x) & 0x00000003) << 12))
#define PLL_GET_REG_SEL_OC_MODE(x)     ((u32)(((x >> 12) & 0x00000003)))
#define PLL_MASK_REG_OC_STEP_SET       ((u32)0x000003FF << 14)           /*!< R/W 10'h200  线性超频且超频mode=00时,每次跳变的step*/
#define PLL_REG_OC_STEP_SET(x)         (((u32)((x) & 0x000003FF) << 14))
#define PLL_GET_REG_OC_STEP_SET(x)     ((u32)(((x >> 14) & 0x000003FF)))
#define PLL_MASK_REG_HS_OC_STOP_DIFF   ((u32)0x00000003 << 24)           /*!< R/W 2'b00  线性超频mode01时,设定一步超频后离target NFcode的delta值*/
#define PLL_REG_HS_OC_STOP_DIFF(x)     (((u32)((x) & 0x00000003) << 24))
#define PLL_GET_REG_HS_OC_STOP_DIFF(x) ((u32)(((x >> 24) & 0x00000003)))
#define PLL_MASK_REG_OC_DONE_DELAY     ((u32)0x0000003F << 26)           /*!< R/W 6'b0  OC_EN  1->0后,多久将SSCD送出的flag信号OC_DONE 置1 判断NFcode==target NF code后, delay REG_OC_DONE_delay[5:0]个CKXSTAL周期, OC_DONE 置1*/
#define PLL_REG_OC_DONE_DELAY(x)       (((u32)((x) & 0x0000003F) << 26))
#define PLL_GET_REG_OC_DONE_DELAY(x)   ((u32)(((x >> 26) & 0x0000003F)))
/** @} */

/** @defgroup PLL_SSC_CTRL3
  * @brief
  * @{
  */
#define PLL_BIT_REG_BYPASS_PI     ((u32)0x00000001 << 0)            /*!< R/W 0x1  bypass PI =0: 送给PLL Loop PI的phase selsect信号正常toggle =1:送给PLL Loop PI的phase selsect==0*/
#define PLL_BIT_EN_SSC            ((u32)0x00000001 << 1)            /*!< R/W 0x0  SSC enable: =0: disable SSC =1: enable SSC*/
#define PLL_BIT_REG_SDM_ORDER     ((u32)0x00000001 << 2)            /*!< R/W 1'b0  Sigma-delta order: =0: 1阶 sigma-delta =1: 2阶 sigma-delta*/
#define PLL_MASK_GRAN_EST         ((u32)0x0007FFFF << 4)            /*!< R/W 19'd1386  SSC step (SSC depth&frequence计算公式见ppt)*/
#define PLL_GRAN_EST(x)           (((u32)((x) & 0x0007FFFF) << 4))
#define PLL_GET_GRAN_EST(x)       ((u32)(((x >> 4) & 0x0007FFFF)))
#define PLL_MASK_REG_DOT_GRAN     ((u32)0x00000007 << 23)           /*!< R/W 3'b100  控制GRAN_EST[20:0]的小数点位置*/
#define PLL_REG_DOT_GRAN(x)       (((u32)((x) & 0x00000007) << 23))
#define PLL_GET_REG_DOT_GRAN(x)   ((u32)(((x >> 23) & 0x00000007)))
#define PLL_BIT_REG_GRAN_AUTO_RST ((u32)0x00000001 << 26)           /*!< R/W 0x0  SSC step auto reset control: =0: not auto reset =1: SSC step在三角波拐点处每次都 reset 到0, 然后再将GRAN_EST, REG_DOT_GRAN赋给SSC step*/
#define PLL_BIT_REG_EN_WDOG       ((u32)0x00000001 << 27)           /*!< R/W 0x1  PLL watchdog enable =0: disable PLL watchdog =1: enable PLL watchdog*/
#define PLL_MASK_REG_TIME0_CK     ((u32)0x00000007 << 28)           /*!< R/W 3'b010  PLL watchdog timer: 3'b000:  watchdog compare CKREF&CKBK frequency Every 2 CKREF period 3'b001:  watchdog compare CKREF&CKBK frequency Every 200 CKREF period 3'b010:  watchdog compare CKREF&CKBK frequency Every 600 CKREF period 3'b011:  watchdog compare CKREF&CKBK frequency Every 1200 CKREF period 3'b100:  watchdog compare CKREF&CKBK frequency Every 3000 CKREF period 3'b101:  watchdog compare CKREF&CKBK frequency Every 6400 CKREF period 3'b110:  watchdog compare CKREF&CKBK frequency Every 12800 CKREF period 3'b111:  watchdog compare CKREF&CKBK frequency Every 16000 CKREF period*/
#define PLL_REG_TIME0_CK(x)       (((u32)((x) & 0x00000007) << 28))
#define PLL_GET_REG_TIME0_CK(x)   ((u32)(((x >> 28) & 0x00000007)))
/** @} */

/** @defgroup PLL_SSC_CTRL4
  * @brief
  * @{
  */
#define PLL_MASK_REG_TIME_RDY_CKOUT    ((u32)0x00000003 << 0)            /*!< R/W 2'b10  output signal CLKRDY delay control*/
#define PLL_REG_TIME_RDY_CKOUT(x)      (((u32)((x) & 0x00000003) << 0))
#define PLL_GET_REG_TIME_RDY_CKOUT(x)  ((u32)(((x >> 0) & 0x00000003)))
#define PLL_MASK_REG_TIME2_RST_WIDTH   ((u32)0x00000003 << 2)            /*!< R/W 2'b00  PLL watchdog signal pulse width: 2'b00:  watchdog reset pulse width =2 CKREF period 2'b01:  watchdog reset pulse width =4 CKREF period 2'b01:  watchdog reset pulse width =8 CKREF period 2'b01:  watchdog reset pulse width =20 CKREF period*/
#define PLL_REG_TIME2_RST_WIDTH(x)     (((u32)((x) & 0x00000003) << 2))
#define PLL_GET_REG_TIME2_RST_WIDTH(x) ((u32)(((x >> 2) & 0x00000003)))
#define PLL_BIT_FMOD                   ((u32)0x00000001 << 5)            /*!< R   */
#define PLL_BIT_OC_DONE                ((u32)0x00000001 << 6)            /*!< R   */
#define PLL_BIT_CLKRDY                 ((u32)0x00000001 << 7)            /*!< R   */
#define PLL_MASK_NCODE_DEBUG           ((u32)0x0000001F << 8)            /*!< R   */
#define PLL_NCODE_DEBUG(x)             (((u32)((x) & 0x0000001F) << 8))
#define PLL_GET_NCODE_DEBUG(x)         ((u32)(((x >> 8) & 0x0000001F)))
#define PLL_MASK_FCODE_DEBUG           ((u32)0x00003FFF << 13)           /*!< R   */
#define PLL_FCODE_DEBUG(x)             (((u32)((x) & 0x00003FFF) << 13))
#define PLL_GET_FCODE_DEBUG(x)         ((u32)(((x >> 13) & 0x00003FFF)))
#define PLL_BIT_FEN_PLL                ((u32)0x00000001 << 29)           /*!< R/W 0x0  SSC global reset, at same time reset PLL also when pll on , set it to 1 after PWC is enable when pll off , set it to 0 at last*/
#define PLL_BIT_PWC_UALV_PLL           ((u32)0x00000001 << 30)           /*!< R/W 0x0  keep UA10PC_EN for PLL*/
#define PLL_BIT_PWC_UAHV_PLL           ((u32)0x00000001 << 31)           /*!< R/W 0x0  keep UA33PC_EN for PLL*/
/** @} */

/** @defgroup PLL_AUX_BG
  * @brief
  * @{
  */
#define PLL_BIT_POW_BG ((u32)0x00000001 << 0) /*!< R/W/ES 0x0  power on BG*/
/** @} */

/** @defgroup PLL_LPSD
  * @brief
  * @{
  */
#define PLL_MASK_LPSD_DMY    ((u32)0x0000003F << 14)           /*!< R/W 0x0  dummy*/
#define PLL_LPSD_DMY(x)      (((u32)((x) & 0x0000003F) << 14))
#define PLL_GET_LPSD_DMY(x)  ((u32)(((x >> 14) & 0x0000003F)))
#define PLL_BIT_EN_VREF0_L   ((u32)0x00000001 << 13)           /*!< R/W 0x0  set VREFL to 0*/
#define PLL_BIT_EN_VREF0_H   ((u32)0x00000001 << 12)           /*!< R/W 0x0  set VREFH to 0*/
#define PLL_MASK_SEL_VREFL   ((u32)0x0000001F << 7)            /*!< R 0x0  adjust VREFL*/
#define PLL_SEL_VREFL(x)     (((u32)((x) & 0x0000001F) << 7))
#define PLL_GET_SEL_VREFL(x) ((u32)(((x >> 7) & 0x0000001F)))
#define PLL_MASK_SEL_VREFH   ((u32)0x0000001F << 2)            /*!< R 0x0  adjust VREFH*/
#define PLL_SEL_VREFH(x)     (((u32)((x) & 0x0000001F) << 2))
#define PLL_GET_SEL_VREFH(x) ((u32)(((x >> 2) & 0x0000001F)))
#define PLL_BIT_SD_POSEDGE   ((u32)0x00000001 << 1)            /*!< R/W 0x0  1: vin>vref output high*/
#define PLL_BIT_POW_SD       ((u32)0x00000001 << 0)            /*!< R/W/ES 0x0  Power on signal detector*/
/** @} */

/** @defgroup PLL_LPAD0
  * @brief
  * @{
  */
#define PLL_MASK_VSET          ((u32)0x00000003 << 30)           /*!< R/W 2'h2  11:1.89V 10:1.80V 01:1.71V 00:1.62V*/
#define PLL_VSET(x)            (((u32)((x) & 0x00000003) << 30))
#define PLL_GET_VSET(x)        ((u32)(((x >> 30) & 0x00000003)))
#define PLL_BIT_SEL_LN         ((u32)0x00000001 << 29)           /*!< R/W 0x1  1: comparator low noise for LSB 6bit*/
#define PLL_BIT_SEL_LNA        ((u32)0x00000001 << 28)           /*!< R/W 0x1  1: comparator low noise for ALL bit*/
#define PLL_MASK_REG_VCMIN     ((u32)0x00000003 << 26)           /*!< R/W 0x1  00:0.4625V 01:0.4500V 10:0.4375V 11:0.4250V*/
#define PLL_REG_VCMIN(x)       (((u32)((x) & 0x00000003) << 26))
#define PLL_GET_REG_VCMIN(x)   ((u32)(((x >> 26) & 0x00000003)))
#define PLL_BIT_LPAD_DMY0      ((u32)0x00000001 << 25)           /*!< R/W 0x0  */
#define PLL_BIT_REG_ENTG       ((u32)0x00000001 << 24)           /*!< R/W 0x0  1: enable transmission gate*/
#define PLL_MASK_BYPASSEN      ((u32)0x000000FF << 16)           /*!< R/W 0x0  1:bypass mode, 0: divide mode*/
#define PLL_BYPASSEN(x)        (((u32)((x) & 0x000000FF) << 16))
#define PLL_GET_BYPASSEN(x)    ((u32)(((x >> 16) & 0x000000FF)))
#define PLL_BIT_EN_MODE_18     ((u32)0x00000001 << 15)           /*!< R/W 0x0  1 : 1.8V mode(bypassen=0)  0 : 3.3V mode(bypassen=0)*/
#define PLL_BIT_REG_META       ((u32)0x00000001 << 14)           /*!< R/W 0x0  1: enable meta detect*/
#define PLL_MASK_REG_CMPDEC    ((u32)0x00000003 << 12)           /*!< R/W 0x1  meta detect time 00: short*/
#define PLL_REG_CMPDEC(x)      (((u32)((x) & 0x00000003) << 12))
#define PLL_GET_REG_CMPDEC(x)  ((u32)(((x >> 12) & 0x00000003)))
#define PLL_BIT_REG_REV_CLK    ((u32)0x00000001 << 11)           /*!< R/W 0x0  1: rever ck_data*/
#define PLL_MASK_VREF_SEL      ((u32)0x00000007 << 8)            /*!< R/W 3'h2  000:0.800V 001:0.825V 010:0.850V 011:0.875V 100:0.900V 101:0.950V  110:1.000V 111:1.050V*/
#define PLL_VREF_SEL(x)        (((u32)((x) & 0x00000007) << 8))
#define PLL_GET_VREF_SEL(x)    ((u32)(((x >> 8) & 0x00000007)))
#define PLL_MASK_REG_LDO_SEL   ((u32)0x00000007 << 5)            /*!< R/W 3'h6  000:0.800V 001:0.825V 010:0.850V 011:0.875V 100:0.900V 101:0.950V  110:1.000V 111:1.050V*/
#define PLL_REG_LDO_SEL(x)     (((u32)((x) & 0x00000007) << 5))
#define PLL_GET_REG_LDO_SEL(x) ((u32)(((x >> 5) & 0x00000007)))
#define PLL_BIT_LPAD_DMY1      ((u32)0x00000001 << 4)            /*!< R/W 0x0  dummy*/
#define PLL_BIT_REG_CK         ((u32)0x00000001 << 3)            /*!< R/W 0x0  1: clk from external*/
#define PLL_BIT_POW_REF        ((u32)0x00000001 << 2)            /*!< R/W/ES 0x0  pow adc refgen*/
#define PLL_BIT_POW            ((u32)0x00000001 << 1)            /*!< R/W/ES 0x0  pow adc*/
#define PLL_BIT_POW_LDO        ((u32)0x00000001 << 0)            /*!< R/W/ES 0x0  pow ldo*/
/** @} */

/** @defgroup PLL_LPAD1
  * @brief
  * @{
  */
#define PLL_MASK_LPAD_DMY2       ((u32)0x00003FFF << 18)           /*!< R/W 0x0  Reserve for future function*/
#define PLL_LPAD_DMY2(x)         (((u32)((x) & 0x00003FFF) << 18))
#define PLL_GET_LPAD_DMY2(x)     ((u32)(((x >> 18) & 0x00003FFF)))
#define PLL_BIT_BYPASSEN_TOUCH   ((u32)0x00000001 << 17)           /*!< R 0x0  1: bypass mode 0: divide 3.3V mode (en_mode_18=0) 0: divide 1.8V mode (en_mode_18=1)*/
#define PLL_BIT_REG_DIODE        ((u32)0x00000001 << 16)           /*!< R 0x0  1: enable diode*/
#define PLL_MASK_REG_MBIAS_SEL   ((u32)0x0000FFFF << 0)            /*!< R 0x0  0/1/2/3/4/5/6/7/8/9/10/11/12/13/14/15: 0.25/0.25/0.25/0.25/0.5/0.5/1/2/2/4/8/8/16/32/32/64uA 0000000000000000:0uA 0000000000000001:0.25uA 0000000000000010:0.25uA ...1111111111111111:171uA*/
#define PLL_REG_MBIAS_SEL(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define PLL_GET_REG_MBIAS_SEL(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup PLL_BG_PWR_ON_CNT
  * @brief
  * @{
  */
#define PLL_MASK_BG_I_CNT    ((u32)0x0000FFFF << 16)           /*!< R/W 16'h28  */
#define PLL_BG_I_CNT(x)      (((u32)((x) & 0x0000FFFF) << 16))
#define PLL_GET_BG_I_CNT(x)  ((u32)(((x >> 16) & 0x0000FFFF)))
#define PLL_MASK_BG_ON_CNT   ((u32)0x0000FFFF << 0)            /*!< R/W 16'h78  */
#define PLL_BG_ON_CNT(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define PLL_GET_BG_ON_CNT(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup PLL_ADC_PWR_ON_CNT
  * @brief
  * @{
  */
#define PLL_MASK_ADC_PWRON_CNT   ((u32)0x0000FFFF << 0)           /*!< R/W 16'h258  power ad delay control. Base on 4M cycle default 150us*/
#define PLL_ADC_PWRON_CNT(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define PLL_GET_ADC_PWRON_CNT(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup PLL_STATE
  * @brief
  * @{
  */
#define PLL_BIT_CKRDY_PLL ((u32)0x00000001 << 0) /*!< R   PLL clock is ready*/
/** @} */

/** @defgroup PLL_DEBUG
  * @brief
  * @{
  */
#define PLL_MASK_ADBG_AUX   ((u32)0xFFFFFFFF << 0)           /*!< R   dummy*/
#define PLL_ADBG_AUX(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PLL_GET_ADBG_AUX(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PLL_DUMMY
  * @brief
  * @{
  */
#define PLL_MASK_LPAD_DUMMY   ((u32)0x000000FF << 0)           /*!< R/W 0x0  dummy*/
#define PLL_LPAD_DUMMY(x)     (((u32)((x) & 0x000000FF) << 0))
#define PLL_GET_LPAD_DUMMY(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PLL_CLK_CALC
  * @brief
  * @{
  */
#define PLL_BIT_CK_CAL_START  ((u32)0x00000001 << 31)           /*!< R/W/ES 0x0  Start clk calibration . This bit will auto-set to 0 when cal done .*/
#define PLL_MASK_CK_CAL_SEL   ((u32)0x00000003 << 16)           /*!< R/W 0x0  Calibration clock selection . The refrence clock is xtal clock. 0: OSC4M 1: AON100K 2: AON128K for SDM*/
#define PLL_CK_CAL_SEL(x)     (((u32)((x) & 0x00000003) << 16))
#define PLL_GET_CK_CAL_SEL(x) ((u32)(((x >> 16) & 0x00000003)))
#define PLL_MASK_CK_CAL_RPT   ((u32)0x0000FFFF << 0)            /*!< R   Clock calibration result*/
#define PLL_CK_CAL_RPT(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define PLL_GET_CK_CAL_RPT(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup PLL_UPLL_CTRL0
  * @brief
  * @{
  */
#define PLL_BIT_USB2_DIGOTGPADEN     ((u32)0x00000001 << 28)          /*!< R/W 0x1  1: usb otg pad used as digital IO pad*/
#define PLL_BIT_USB_OTG_EN           ((u32)0x00000001 << 27)          /*!< R/W 0x0  0: disable otg signals to usb mac*/
#define PLL_BIT_USB2_DIGPADEN        ((u32)0x00000001 << 26)          /*!< R/W 0x1  1: usb pad used as digital IO pad*/
#define PLL_BIT_ISO_USBA_EN          ((u32)0x00000001 << 23)          /*!< R/W 0x1  1: Enable isolation signal from USBPHY analog 0: disable*/
#define PLL_BIT_SYS_PLL_EN           ((u32)0x00000001 << 22)          /*!< R/W 0x1  uphy pll enable request, keep pll alive when usb entering suspend state both usb_aphy_en and sys_pll_en should be set 0 to disable uphy pll*/
#define PLL_BIT_USB_APHY_EN          ((u32)0x00000001 << 21)          /*!< R/W 0x0  usb aphy power on reset, low active this bit also controls uphy pll: can only write 1 after xtal normal 40M is ready write 0, uphy pll will be disabled Note: when power on , HW pmc only set this bit to 1 , other using default value , and USB pll can power up*/
#define PLL_BIT_USB_DPHY_EN          ((u32)0x00000001 << 20)          /*!< R/W 0x1  usb dphy power on reset, low active*/
#define PLL_BIT_PWC_UABG_EN          ((u32)0x00000001 << 19)          /*!< R/W 0x1  1: enable USBPHY BG power cut*/
#define PLL_BIT_PWC_UAHV_ALIVE       ((u32)0x00000001 << 18)          /*!< R/W 0x0  keep UA33PC_EN alive for usb suspend*/
#define PLL_BIT_PWC_UAPC_EN          ((u32)0x00000001 << 17)          /*!< R/W 0x1  enable USBPHY power cut UA33PC_EN = PWC_UAHV_ALIVE | PWC_UAPC_EN UA10PC_EN = PWC_UAPC_EN*/
#define PLL_BIT_OTG_ANA_EN           ((u32)0x00000001 << 16)          /*!< R/W 0x0  1: enable usb aphy lv2hv, hv2lv check for otg*/
#define PLL_BIT_SUSPEND_WAK_MSK      ((u32)0x00000001 << 4)           /*!< R/W 0x1  set 0 to mask usb suspendm(keep 1) at normal state after receiving usb suspend interrupt, set this bit to 1 to enable resume(from suspend) wake event*/
#define PLL_MASK_SS_SCALEDOWN_MODE   ((u32)0x00000003 << 0)           /*!< R/W 0h  use scaled-down timing values for usb, resulting in faster simulations*/
#define PLL_SS_SCALEDOWN_MODE(x)     (((u32)((x) & 0x00000003) << 0))
#define PLL_GET_SS_SCALEDOWN_MODE(x) ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup PLL_UPLL_CTRL1
  * @brief
  * @{
  */
#define PLL_BIT_UPLL_CKREF_SEL        ((u32)0x00000001 << 31)           /*!< R/W 0x1  ECO, REG_CKREF_SEL for usbphy, reset default 1*/
#define PLL_MASK_UPLL_CMU_LF_R        ((u32)0x00000007 << 19)           /*!< R/W 3'b011  Resistance of loop filter, adjust for PLL bandwidth 000: 3.1KΩ ,    001: 6.1KΩ , 010: 9.2KΩ ,    011: 12.2KΩ, 100: 15.2KΩ,   101: 18.2KΩ, 110: 21.3KΩ,   111: 24.3KΩ*/
#define PLL_UPLL_CMU_LF_R(x)          (((u32)((x) & 0x00000007) << 19))
#define PLL_GET_UPLL_CMU_LF_R(x)      ((u32)(((x >> 19) & 0x00000007)))
#define PLL_MASK_UPLL_CMU_ICP         ((u32)0x00000003 << 17)           /*!< R/W 2'b01  Charge-Pump current, adjust for PLL bandwidth 00: 5uA 01: 10uA 10: 15uA 11: 20uA*/
#define PLL_UPLL_CMU_ICP(x)           (((u32)((x) & 0x00000003) << 17))
#define PLL_GET_UPLL_CMU_ICP(x)       ((u32)(((x >> 17) & 0x00000003)))
#define PLL_BIT_UPLL_CMU_BIG_KVCO     ((u32)0x00000001 << 16)           /*!< R/W 0x0  Control Kvco of CCO, adjust for bandwidth & jitter 0: Smaller KVCO 1: Larger KVCO Kvco(Hz/V): the frequency change per 1V of CCO*/
#define PLL_MASK_UPLL_CMU_CKRDY       ((u32)0x00000007 << 13)           /*!< R/W 3'b101  PLL480M clock output after ∆T for waiting 480MHz clock ready, ∆T: 000: 21*TPLLCKIN,    001: 22*TPLLCKIN 010: 25*TPLLCKIN,    011: 27*TPLLCKIN 100: 210*TPLLCKIN,   101: 212*TPLLCKIN 110: 216*TPLLCKIN,   111: 3*216*TPLLCKIN TPLLCKIN: period of PLLCKIN*/
#define PLL_UPLL_CMU_CKRDY(x)         (((u32)((x) & 0x00000007) << 13))
#define PLL_GET_UPLL_CMU_CKRDY(x)     ((u32)(((x >> 13) & 0x00000007)))
#define PLL_BIT_UPLL_CMU_CKRDY_BYPASS ((u32)0x00000001 << 12)           /*!< R/W 0x0  Bypass the time delay of CKRDY 1: Bypass CKRDY 0: PLL480M clock output after ∆T ∆T: delay time, refer to REG_CMU_CKRDY*/
#define PLL_BIT_UPLL_CMU_DIV_CYC      ((u32)0x00000001 << 11)           /*!< R/W 0x1  Pulse width of divider output clock, adjust for PI monotonic phase transfer 0: 3/480e6 cycle is logic 1 1: 6/480e6 cycle is logic 1*/
#define PLL_BIT_UPLL_CMU_CCO_SEL      ((u32)0x00000001 << 10)           /*!< R/W 0x1  1: VCO mode 0: CCO mode*/
#define PLL_MASK_UPLL_CMU_PREDIV      ((u32)0x00000003 << 8)            /*!< R/W 2'b00  Division-ratio of pre-divider for PLL clock in 11: fPLLCKIN pre-divided-by-8 10: fPLLCKIN pre-divided-by-4 01: fPLLCKIN pre-divided-by-2 00: fPLLCKIN pre-divided-by-1 fPLLCKIN: frequency of PLLCKIN*/
#define PLL_UPLL_CMU_PREDIV(x)        (((u32)((x) & 0x00000003) << 8))
#define PLL_GET_UPLL_CMU_PREDIV(x)    ((u32)(((x >> 8) & 0x00000003)))
#define PLL_MASK_UPLL_N_PLL_TOP       ((u32)0x000000FF << 0)            /*!< R/W 0x16  (n+2) divider's divisor*/
#define PLL_UPLL_N_PLL_TOP(x)         (((u32)((x) & 0x000000FF) << 0))
#define PLL_GET_UPLL_N_PLL_TOP(x)     ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PLL_DSS_CTRL
  * @brief
  * @{
  */
#define PLL_MASK_DSS_RSTN      ((u32)0x00000003 << 28)           /*!< R/W 0x0  */
#define PLL_DSS_RSTN(x)        (((u32)((x) & 0x00000003) << 28))
#define PLL_GET_DSS_RSTN(x)    ((u32)(((x >> 28) & 0x00000003)))
#define PLL_BIT_DSS_WIRE_SEL   ((u32)0x00000001 << 25)           /*!< R/W 0x0  */
#define PLL_BIT_DSS_SPEED_EN   ((u32)0x00000001 << 24)           /*!< R/W 0x0  */
#define PLL_MASK_DSS_RO_SEL    ((u32)0x00000007 << 20)           /*!< R/W 0x0  */
#define PLL_DSS_RO_SEL(x)      (((u32)((x) & 0x00000007) << 20))
#define PLL_GET_DSS_RO_SEL(x)  ((u32)(((x >> 20) & 0x00000007)))
#define PLL_MASK_DSS_DATA_IN   ((u32)0x000FFFFF << 0)            /*!< R/W 0x0  */
#define PLL_DSS_DATA_IN(x)     (((u32)((x) & 0x000FFFFF) << 0))
#define PLL_GET_DSS_DATA_IN(x) ((u32)(((x >> 0) & 0x000FFFFF)))
/** @} */

/** @defgroup PLL_DSS_OUT
  * @brief
  * @{
  */
#define PLL_BIT_DSS_OUT_SEL     ((u32)0x00000001 << 24)          /*!< R/W 0x0  0: select lvt dss 1: select uhvt dss*/
#define PLL_BIT_DSS_WSORT_GO    ((u32)0x00000001 << 21)          /*!< R 0x0  */
#define PLL_BIT_DSS_READY       ((u32)0x00000001 << 20)          /*!< R 0x0  */
#define PLL_MASK_DSS_DATA_OUT   ((u32)0x000FFFFF << 0)           /*!< R 0x0  */
#define PLL_DSS_DATA_OUT(x)     (((u32)((x) & 0x000FFFFF) << 0))
#define PLL_GET_DSS_DATA_OUT(x) ((u32)(((x >> 0) & 0x000FFFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup PLL_Exported_Types PLL Exported Types
  * @{
  */

/** @brief PLL Register Declaration
  */

typedef struct {
	__IO uint32_t PLL_CTRL0         ;  /*!< Register,  Address offset:0x000 */
	__IO uint32_t PLL_SSC_CTRL0     ;  /*!< Register,  Address offset:0x004 */
	__IO uint32_t PLL_SSC_CTRL1     ;  /*!< Register,  Address offset:0x008 */
	__IO uint32_t PLL_SSC_CTRL2     ;  /*!< Register,  Address offset:0x00C */
	__IO uint32_t PLL_SSC_CTRL3     ;  /*!< Register,  Address offset:0x010 */
	__IO uint32_t PLL_SSC_CTRL4     ;  /*!< Register,  Address offset:0x014 */
	__IO uint32_t PLL_AUX_BG        ;  /*!< Register,  Address offset:0x018 */
	__IO uint32_t RSVD0             ;  /*!< Reserved,  Address offset:0x01C-0x01F */
	__IO uint32_t PLL_LPSD          ;  /*!< Register,  Address offset:0x020 */
	__IO uint32_t PLL_LPAD0         ;  /*!< Register,  Address offset:0x024 */
	__IO uint32_t PLL_LPAD1         ;  /*!< Register,  Address offset:0x028 */
	__IO uint32_t PLL_BG_PWR_ON_CNT ;  /*!< Register,  Address offset:0x02C */
	__IO uint32_t PLL_ADC_PWR_ON_CNT;  /*!< Register,  Address offset:0x030 */
	__I  uint32_t PLL_STATE         ;  /*!< Register,  Address offset:0x034 */
	__I  uint32_t PLL_DEBUG         ;  /*!< Register,  Address offset:0x038 */
	__IO uint32_t PLL_DUMMY         ;  /*!< Register,  Address offset:0x03C */
	__IO uint32_t PLL_CLK_CALC      ;  /*!< Register,  Address offset:0x040 */
	__IO uint32_t RSVD1[3]          ;  /*!< Reserved,  Address offset:0x044-0x04F */
	__IO uint32_t PLL_UPLL_CTRL0    ;  /*!< Register,  Address offset:0x050 */
	__IO uint32_t PLL_UPLL_CTRL1    ;  /*!< Register,  Address offset:0x054 */
	__IO uint32_t RSVD2[2]          ;  /*!< Reserved,  Address offset:0x058-0x05F */
	__IO uint32_t PLL_DSS_CTRL      ;  /*!< Register,  Address offset:0x060 */
	__IO uint32_t PLL_DSS_OUT       ;  /*!< Register,  Address offset:0x064 */
} PLL_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


/* SYS PLL input clock pre_divide */
#define PLL_REG_PREDIV_1               (0x0)
#define PLL_REG_PREDIV_2               (0x1)
#define PLL_REG_PREDIV_4               (0x2)
#define PLL_REG_PREDIV_8               (0x3)

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif

/** @} */

/** @} */