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
#define PLL_BIT_REG_CMU_EN         ((u32)0x00000001 << 2)            /*!< R/W/ES 1'b0  PLL的使能信号*/
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
#define PLL_MASK_NCODE_T_SCPU   ((u32)0x0000001F << 0)           /*!< R/W 5'd22  VCO target频率对应的 Ncode*/
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
#define PLL_BIT_FEN_PLL                ((u32)0x00000001 << 29)           /*!< R/W/ES 0x0  SSC global reset, at same time reset PLL also when pll on , set it to 1 after PWC is enable when pll off , set it to 0 at last*/
#define PLL_BIT_PWC_UALV_PLL           ((u32)0x00000001 << 30)           /*!< R/W/ES 0x0  keep UA10PC_EN for PLL*/
#define PLL_BIT_PWC_UAHV_PLL           ((u32)0x00000001 << 31)           /*!< R/W/ES 0x0  keep UA33PC_EN for PLL*/
/** @} */

/** @defgroup PLL_DUMMY
  * @brief
  * @{
  */
#define PLL_MASK_LPAD_DUMMY       ((u32)0x0000003F << 2)           /*!< R/W 0x0  dummy*/
#define PLL_LPAD_DUMMY(x)         (((u32)((x) & 0x0000003F) << 2))
#define PLL_GET_LPAD_DUMMY(x)     ((u32)(((x >> 2) & 0x0000003F)))
#define PLL_MASK_REG_PHSEL_320M   ((u32)0x00000003 << 0)           /*!< R/W 0x0  sys pll 320M clock phase selection*/
#define PLL_REG_PHSEL_320M(x)     (((u32)((x) & 0x00000003) << 0))
#define PLL_GET_REG_PHSEL_320M(x) ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup PLL_CLK_CALC
  * @brief
  * @{
  */
#define PLL_BIT_CK_CAL_START  ((u32)0x00000001 << 31)           /*!< R/W/ES 0x0  Start clk calibration . This bit will auto-set to 0 when cal done .*/
#define PLL_MASK_CK_CAL_SEL   ((u32)0x00000003 << 16)           /*!< R/W 0x0  Calibration clock selection . The refrence clock is xtal clock. 0: OSC4M 1: AON100K 2: AON128K for SDM*/
#define PLL_CK_CAL_SEL(x)     (((u32)((x) & 0x00000003) << 16))
#define PLL_GET_CK_CAL_SEL(x) ((u32)(((x >> 16) & 0x00000003)))
#define PLL_MASK_CK_CAL_RPT   ((u32)0x0000FFFF << 0)            /*!< R   Clock calibration result if ck_cal_sel==0 :  cal_clock_freq(Hz) = xtal_clock_freq(Hz)  * 32 / ck_cal_rpt else : cal_clock_freq(Hz)  = xtal_clock_freq(Hz)  * 8 / ck_cal_rpt*/
#define PLL_CK_CAL_RPT(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define PLL_GET_CK_CAL_RPT(x) ((u32)(((x >> 0) & 0x0000FFFF)))
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

/** @defgroup PLL_CTRL1
  * @brief
  * @{
  */
#define PLL_BIT_OPT_IGN_PLL_OFF_WLBT ((u32)0x00000001 << 17)           /*!< R/W 1'b0  option for pll ignore wlbt req off , it always on*/
#define PLL_BIT_OPT_IGN_PLL_OFF_SYS  ((u32)0x00000001 << 16)           /*!< R/W 1'b0  option for pll ignore sys req off , it always on*/
#define PLL_BIT_REG_DIV_CYC          ((u32)0x00000001 << 15)           /*!< R/W 1'b1  */
#define PLL_MASK_REG_CKRDY           ((u32)0x00000007 << 12)           /*!< R/W 3'b101  */
#define PLL_REG_CKRDY(x)             (((u32)((x) & 0x00000007) << 12))
#define PLL_GET_REG_CKRDY(x)         ((u32)(((x >> 12) & 0x00000007)))
#define PLL_BIT_REG_CKRDY_BYPASS     ((u32)0x00000001 << 8)            /*!< R/W 1'b0  */
#define PLL_BIT_REG_LPF_BG           ((u32)0x00000001 << 6)            /*!< R/W 0x1  */
#define PLL_BIT_REG_POW_BG           ((u32)0x00000001 << 5)            /*!< R/W/ES 0x0  */
#define PLL_BIT_REG_ADC_CLK_EN       ((u32)0x00000001 << 4)            /*!< R/W/ES 0x0  */
#define PLL_BIT_REG_AFE_CLK_EN       ((u32)0x00000001 << 3)            /*!< R/W/ES 0x0  */
#define PLL_BIT_REG_PSRAM_CLK_EN     ((u32)0x00000001 << 2)            /*!< R/W/ES 0x0  */
#define PLL_BIT_REG_SYS1_CLK_EN      ((u32)0x00000001 << 1)            /*!< R/W/ES 0x0  */
#define PLL_BIT_REG_SYS0_CLK_EN      ((u32)0x00000001 << 0)            /*!< R/W/ES 0x0  Note,sys0 gating is in digital domain*/
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup PLL_Exported_Types PLL Exported Types
  * @{
  */

/** @brief PLL Register Declaration
  */

typedef struct {
	__IO uint32_t PLL_CTRL0    ;  /*!< Register,  Address offset:0x000 */
	__IO uint32_t PLL_SSC_CTRL0;  /*!< Register,  Address offset:0x004 */
	__IO uint32_t PLL_SSC_CTRL1;  /*!< Register,  Address offset:0x008 */
	__IO uint32_t PLL_SSC_CTRL2;  /*!< Register,  Address offset:0x00C */
	__IO uint32_t PLL_SSC_CTRL3;  /*!< Register,  Address offset:0x010 */
	__IO uint32_t PLL_SSC_CTRL4;  /*!< Register,  Address offset:0x014 */
	__IO uint32_t RSVD0[7]     ;  /*!< Reserved,  Address offset:0x018-0x033 */
	__IO uint32_t PLL_STATE    ;  /*!< Register,  Address offset:0x034 */
	__IO uint32_t RSVD1        ;  /*!< Reserved,  Address offset:0x038-0x03B */
	__IO uint32_t PLL_DUMMY    ;  /*!< Register,  Address offset:0x03C */
	__IO uint32_t PLL_CLK_CALC ;  /*!< Register,  Address offset:0x040 */
	__IO uint32_t RSVD2[7]     ;  /*!< Reserved,  Address offset:0x044-0x05F */
	__IO uint32_t PLL_DSS_CTRL ;  /*!< Register,  Address offset:0x060 */
	__IO uint32_t PLL_DSS_OUT  ;  /*!< Register,  Address offset:0x064 */
	__IO uint32_t RSVD3[2]     ;  /*!< Reserved,  Address offset:0x068-0x06F */
	__IO uint32_t PLL_CTRL1    ;  /*!< Register,  Address offset:0x070 */
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