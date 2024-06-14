/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_CAPTOUCH_H_
#define _AMEBA_CAPTOUCH_H_

/** @addtogroup CAPTOUCH
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * CAPTOUCH:
  *		- Base Address: CAPTOUCH_DEV
  *		- Clock source : 32.768KHz
  *		- Scan interval timer: 1.024KHz(32.768KHz/32)
  *		- Debounce Timer: Configurable
  *		- Channel: 9
  *		- IRQ: CapTouch_IRQ
  *
  *****************************************************************************************
  * How to use Normal CapTouch
  *****************************************************************************************
  *      To use the normal CapTouch mode, the following steps are mandatory:
  *
  *      1. Enable CapTouch peripheral clock
  *
  *      2. Configure the CapTouch pinmux.
  *
  *      3. Init Captouch parameters:
  *			CapTouch_StructInit(CapTouch_InitTypeDef* CapTouch_InitStruct)
  *
  *      4. Init Hardware use step3 parameters:
  *			CapTouch_Init(CAPTOUCH_TypeDef *CapTouch, CapTouch_InitTypeDef* CapTouch_InitStruct)
  *
  *      5. Enable the NVIC and the corresponding interrupt using following function if you need
  *			to use interrupt mode.
  *			CapTouch_INTConfig(): CapTouch IRQ Enable set
  *			CapTouch_INTMask(): CapTouch IRQ mask set
  *			InterruptRegister(): register the captouch irq handler
  *			InterruptEn(): Enable the NVIC interrupt
  *
  *      6. Enable CapTouch module using CapTouch_Cmd().
  *
  *****************************************************************************************
  * @endverbatim
  */

/* AUTO_GEN_START */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup CAPTOUCH
* @brief CAPTOUCH driver modules
* @{
*/

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup CAPTOUCH_Register_Definitions CAPTOUCH Register Definitions
 * @{
 *****************************************************************************/

/** @defgroup CT_CTC_CTRL
 * @brief Cap-Touch Control Register
 * @{
 **/
#define CT_BIT_BASELINE_INI              ((u32)0x00000001 << 8)          /*!<R/W/EC 1h  1: Baseline initial function enable, HW will clear this bit to "0" after baseline initial */
#define CT_MASK_DEBOUNCE_TIME            ((u32)0x00000003 << 5)          /*!<R/W 0h  2'b00: 2 times 2'b01: 3 times 2'b10: 4 times 2'b11: 5 times */
#define CT_DEBOUNCE_TIME(x)              (((u32)((x) & 0x00000003) << 5))
#define CT_GET_DEBOUNCE_TIME(x)          ((u32)(((x >> 5) & 0x00000003)))
#define CT_BIT_DEBOUNCE_EN               ((u32)0x00000001 << 4)          /*!<R/W 0h  0: Disable 1: Enable */
#define CT_BIT_ENABLE                    ((u32)0x00000001 << 0)          /*!<R/W 0h  0: CTC disable 1: CTC enable */
/** @} */

/** @defgroup CT_SCAN_PERIOD
 * @brief Scan Parameters Register
 * @{
 **/
#define CT_MASK_SAMPLE_AVE_CTRL          ((u32)0x00000007 << 16)          /*!<R/W 6h  ADC sampled number for average function average number=2^(sample_ave_ctrl+2) */
#define CT_SAMPLE_AVE_CTRL(x)            (((u32)((x) & 0x00000007) << 16))
#define CT_GET_SAMPLE_AVE_CTRL(x)        ((u32)(((x >> 16) & 0x00000007)))
#define CT_MASK_SLEEP_TIME_CNT           ((u32)0x00000FFF << 0)          /*!<R/W 3Ch  Code: 0~4095 (0~0xFFF), unit is 1.024kHz cycle (1/32 32.768kHz). When this register is set to 0 , HW will scan continuously and have no sleep time. Recommend value: 60~480ms */
#define CT_SLEEP_TIME_CNT(x)             (((u32)((x) & 0x00000FFF) << 0))
#define CT_GET_SLEEP_TIME_CNT(x)         ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup CT_ETC_CTRL
 * @brief Environment Tracking Control Register
 * @{
 **/
#define CT_MASK_BASELINE_UPD_STEP        ((u32)0x0000000F << 12)          /*!<R/W 1h  Baseline update step for all channels */
#define CT_BASELINE_UPD_STEP(x)          (((u32)((x) & 0x0000000F) << 12))
#define CT_GET_BASELINE_UPD_STEP(x)      ((u32)(((x >> 12) & 0x0000000F)))
#define CT_MASK_BASELINE_WT_FACTOR       ((u32)0x0000000F << 8)          /*!<R/W 2h  Baseline update weight factor (ETC_factor_th) for all channels ETC_factor_th=2^baseline_wt_factor */
#define CT_BASELINE_WT_FACTOR(x)         (((u32)((x) & 0x0000000F) << 8))
#define CT_GET_BASELINE_WT_FACTOR(x)     ((u32)(((x >> 8) & 0x0000000F)))
#define CT_MASK_ETC_SCAN_INTERVAL        ((u32)0x0000007F << 1)          /*!<R/W 2h  ETC update interval between scan period (sleep time) for all channels Interval=(etc_scan_interval+1)*scan_period */
#define CT_ETC_SCAN_INTERVAL(x)          (((u32)((x) & 0x0000007F) << 1))
#define CT_GET_ETC_SCAN_INTERVAL(x)      ((u32)(((x >> 1) & 0x0000007F)))
#define CT_BIT_ETC_FUNC_CTRL             ((u32)0x00000001 << 0)          /*!<R/W 0h  Environmental cap tracking calibration function 0: Disable 1: Enable */
/** @} */

/** @defgroup CT_SNR_INF
 * @brief Signal Noise Ratio Register
 * @{
 **/
#define CT_BIT_SNR_UPD_ALWAYS            ((u32)0x00000001 << 31)          /*!<R/W 0  1: Update SNR info no matter touch or not (used only in debug function) */
#define CT_MASK_SNR_NOISE_DATA           ((u32)0x00000FFF << 16)          /*!<R/W/ES 0  Noise peak to peak signal raw data for SNR monitor */
#define CT_SNR_NOISE_DATA(x)             (((u32)((x) & 0x00000FFF) << 16))
#define CT_GET_SNR_NOISE_DATA(x)         ((u32)(((x >> 16) & 0x00000FFF)))
#define CT_MASK_SNR_TOUCH_DATA           ((u32)0x00000FFF << 0)          /*!<R/W/ES 0h  Raw data of touch (signal-baseline) for SNR monitor */
#define CT_SNR_TOUCH_DATA(x)             (((u32)((x) & 0x00000FFF) << 0))
#define CT_GET_SNR_TOUCH_DATA(x)         ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup CT_DEBUG_MODE_CTRL
 * @brief Debug and Channel Swith Mode Register
 * @{
 **/
#define CT_MASK_CH_CTRL                  ((u32)0x00000007 << 5)          /*!<R/W 0h  Scan channel control 000: channel 0 001: channel 1 010: channel 2 011: channel 3 */
#define CT_CH_CTRL(x)                    (((u32)((x) & 0x00000007) << 5))
#define CT_GET_CH_CTRL(x)                ((u32)(((x >> 5) & 0x00000007)))
#define CT_BIT_CH_SWITCH_CTRL            ((u32)0x00000001 << 4)          /*!<R/W 1h  Scan channel switch control 0: Auto switch function disable (manual switch by ch_ctrl) 1: Auto switch to next channel (channel 0 -> channel 3 -> channel 0) */
#define CT_BIT_CONTINUE_SAMPLE_EN        ((u32)0x00000001 << 1)          /*!<R/W 0  Continue sample one channel mode enable, only take effect in debug mode 0: Disable 1: Enable */
#define CT_BIT_DEBUG_EN                  ((u32)0x00000001 << 0)          /*!<R/W 0  Debug mode enable 0: Disable 1: Enable */
/** @} */

/** @defgroup CT_RAW_CODE_FIFO_STATUS
 * @brief FIFO Status Register
 * @{
 **/
#define CT_BIT_AFIFO_CLEAR               ((u32)0x00000001 << 16)          /*!<WA0 0  Clear the FIFO data */
#define CT_MASK_AFIFO_FULL_LVL           ((u32)0x0000003F << 10)          /*!<R/W 0  User defines the full level for a FIFO, the valid value is from 0 to 63 */
#define CT_AFIFO_FULL_LVL(x)             (((u32)((x) & 0x0000003F) << 10))
#define CT_GET_AFIFO_FULL_LVL(x)         ((u32)(((x >> 10) & 0x0000003F)))
#define CT_MASK_AFIFO_VALID_CNT          ((u32)0x0000007F << 3)          /*!<R 0h  Raw code FIFO valid count (push unit number which can be read) */
#define CT_AFIFO_VALID_CNT(x)            (((u32)((x) & 0x0000007F) << 3))
#define CT_GET_AFIFO_VALID_CNT(x)        ((u32)(((x >> 3) & 0x0000007F)))
#define CT_BIT_AFIFO_OVER_LVL            ((u32)0x00000001 << 2)          /*!<R 0  0: Raw code FIFO not over level 1: Raw code FIFO over level */
#define CT_BIT_AFIFO_EMPTY               ((u32)0x00000001 << 1)          /*!<R 1  0: Raw code FIFO not empty 1: Raw code FIFO empty */
#define CT_BIT_AFIFO_FULL                ((u32)0x00000001 << 0)          /*!<R 0  0: Raw code FIFO not oveflow (not full) 1: Raw code FIFO overflow (full) */
/** @} */

/** @defgroup CT_RAW_CODE_FIFO_READ
 * @brief FIFO Read Register
 * @{
 **/
#define CT_BIT_AFIFO_RD_DATA_VLD         ((u32)0x00000001 << 31)          /*!<R 0h  Read data from raw code FIFO valid */
#define CT_MASK_AFIFO_RD_CH_NUM          ((u32)0x0000000F << 12)          /*!<RP 0h  Read data from raw code FIFO [15:12] : channel number */
#define CT_AFIFO_RD_CH_NUM(x)            (((u32)((x) & 0x0000000F) << 12))
#define CT_GET_AFIFO_RD_CH_NUM(x)        ((u32)(((x >> 12) & 0x0000000F)))
#define CT_MASK_AFIFO_RD_DATA            ((u32)0x00000FFF << 0)          /*!<RP 0h  Read data from raw code FIFO [11:0] : data */
#define CT_AFIFO_RD_DATA(x)              (((u32)((x) & 0x00000FFF) << 0))
#define CT_GET_AFIFO_RD_DATA(x)          ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup CT_INTERRUPT_ENABLE
 * @brief Interrupt Enable Register
 * @{
 **/
#define CT_BIT_CTC_BRK_BY_ADC_INTR_EN    ((u32)0x00000001 << 22)          /*!<R/W 0h  Cap-Touch breaked by ADC enable */
#define CT_BIT_AFIFO_OVERLVL_INTR_EN     ((u32)0x00000001 << 21)          /*!<R/W 0h  Raw code FIFO over level enable */
#define CT_BIT_OVER_N_NOISE_TH_INTR_EN   ((u32)0x00000001 << 18)          /*!<R/W 0h  Negative noise threshold overflow enable */
#define CT_BIT_AFIFO_OVERFLOW_INTR_EN    ((u32)0x00000001 << 17)          /*!<R/W 0h  Raw code FIFO overflow enable */
#define CT_BIT_OVER_P_NOISE_TH_INTR_EN   ((u32)0x00000001 << 16)          /*!<R/W 0h  Positive noise threshold overflow enable */
#define CT_MASK_TOUCH_RELEASE_INTR_EN    ((u32)0x0000000F << 8)          /*!<R/W 0h  Channelx single touch release enable, x is 0~3 */
#define CT_TOUCH_RELEASE_INTR_EN(x)      (((u32)((x) & 0x0000000F) << 8))
#define CT_GET_TOUCH_RELEASE_INTR_EN(x)  ((u32)(((x >> 8) & 0x0000000F)))
#define CT_MASK_TOUCH_PRESS_INTR_EN      ((u32)0x0000000F << 0)          /*!<R/W 0h  Channelx single touch press enable, x is 0~3 */
#define CT_TOUCH_PRESS_INTR_EN(x)        (((u32)((x) & 0x0000000F) << 0))
#define CT_GET_TOUCH_PRESS_INTR_EN(x)    ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup CT_INTERRUPT_STATUS
 * @brief interrupt status register
 * @{
 **/
#define CT_BIT_CTC_BRK_BY_ADC_INTR       ((u32)0x00000001 << 22)          /*!<R 0h  Cap-Touch breaked by adc interrupt */
#define CT_BIT_AFIFO_OVERLVL_INTR        ((u32)0x00000001 << 21)          /*!<R 0h  Raw code FIFO over level interrupt */
#define CT_BIT_OVER_N_NOISE_TH_INTR      ((u32)0x00000001 << 18)          /*!<R 0h  Negative noise threshold overflow interrupt */
#define CT_BIT_AFIFO_OVERFLOW_INTR       ((u32)0x00000001 << 17)          /*!<R 0h  Raw code FIFO overflow interrupt */
#define CT_BIT_OVER_P_NOISE_TH_INTR      ((u32)0x00000001 << 16)          /*!<R 0h  Positive noise threshold overflow interrupt */
#define CT_MASK_TOUCH_RELEASE_INTR       ((u32)0x0000000F << 8)          /*!<R 0h  Channelx single touch release interrupt, x is 0~3 */
#define CT_TOUCH_RELEASE_INTR(x)         (((u32)((x) & 0x0000000F) << 8))
#define CT_GET_TOUCH_RELEASE_INTR(x)     ((u32)(((x >> 8) & 0x0000000F)))
#define CT_MASK_TOUCH_PRESS_INTR         ((u32)0x0000000F << 0)          /*!<R 0h  Channelx single touch press interrupt, x is 0~3 */
#define CT_TOUCH_PRESS_INTR(x)           (((u32)((x) & 0x0000000F) << 0))
#define CT_GET_TOUCH_PRESS_INTR(x)       ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup CT_RAW_INTERRUPT_STATUS
 * @brief Raw Interrupt Status Register
 * @{
 **/
#define CT_BIT_CTC_BRK_BY_ADC            ((u32)0x00000001 << 22)          /*!<R 0h  Cap-Touch breaked by ADC raw interrupt */
#define CT_BIT_AFIFO_OVERLVL             ((u32)0x00000001 << 21)          /*!<R 0h  Raw code FIFO over level raw interrupt */
#define CT_BIT_OVER_N_NOISE_TH           ((u32)0x00000001 << 18)          /*!<R 0h  Negative noise threshold overflow raw interrupt */
#define CT_BIT_AFIFO_OVERFLOW            ((u32)0x00000001 << 17)          /*!<R 0h  Raw code FIFO overflow raw interrupt */
#define CT_BIT_OVER_P_NOISE_TH           ((u32)0x00000001 << 16)          /*!<R 0h  Positive noise threshold overflow raw interrupt */
#define CT_MASK_TOUCH_RELEASE            ((u32)0x0000000F << 8)          /*!<R 0h  Channelx single touch release raw interrupt, x is 0~3 */
#define CT_TOUCH_RELEASE(x)              (((u32)((x) & 0x0000000F) << 8))
#define CT_GET_TOUCH_RELEASE(x)          ((u32)(((x >> 8) & 0x0000000F)))
#define CT_MASK_TOUCH_PRESS              ((u32)0x0000000F << 0)          /*!<R 0h  Channelx single touch press raw interrupt, x is 0~3 */
#define CT_TOUCH_PRESS(x)                (((u32)((x) & 0x0000000F) << 0))
#define CT_GET_TOUCH_PRESS(x)            ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup CT_INTERRUPT_ALL_CLR
 * @brief Interrupt All Clear Register
 * @{
 **/
#define CT_BIT_INTR_ALL_CLR              ((u32)0x00000001 << 0)          /*!<R/WA0 0h  Write "1" to this register to clear the combined interrupts, all individual interrupts, interrupt status register and raw interrupt status register. */
/** @} */

/** @defgroup CT_INTERRUPT_STATUS_CLR
 * @brief Interrupt Clear Register
 * @{
 **/
#define CT_BIT_CTC_BRK_BY_ADC_CLR        ((u32)0x00000001 << 22)          /*!<R/WA0 0h  Clear Cap-Touch breaked by adc raw interrupt and related register */
#define CT_BIT_AFIFO_OVERLVL_CLR         ((u32)0x00000001 << 21)          /*!<R/WA0 0h  Clear raw code FIFO over level raw interrupt and related register */
#define CT_BIT_OVER_N_NOISE_TH_CLR       ((u32)0x00000001 << 18)          /*!<R/WA0 0h  Clear negative noise threshold overflow interrupt and related register */
#define CT_BIT_AFIFO_OVERFLOW_CLR        ((u32)0x00000001 << 17)          /*!<R/WA0 0h  Clear raw code FIFO overflow interrupt and related register */
#define CT_BIT_OVER_P_NOISE_TH_CLR       ((u32)0x00000001 << 16)          /*!<R/WA0 0h  Clear positive noise threshold overflow interrupt and related register */
#define CT_MASK_TOUCH_RELEASE_CLR        ((u32)0x0000000F << 8)          /*!<R/WA0 0h  Clear channelx single touch release interrupt and related register */
#define CT_TOUCH_RELEASE_CLR(x)          (((u32)((x) & 0x0000000F) << 8))
#define CT_GET_TOUCH_RELEASE_CLR(x)      ((u32)(((x >> 8) & 0x0000000F)))
#define CT_MASK_TOUCH_PRESS_CLR          ((u32)0x0000000F << 0)          /*!<R/WA0 0h  Clear channelx single touch press interrupt and related register */
#define CT_TOUCH_PRESS_CLR(x)            (((u32)((x) & 0x0000000F) << 0))
#define CT_GET_TOUCH_PRESS_CLR(x)        ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup CT_DEBUG_SEL
 * @brief Debug Select Register
 * @{
 **/
#define CT_MASK_DBG_SEL                  ((u32)0x00000007 << 0)          /*!<R/W 0h  Debug information selection 0: dbg_clkrst 1: dbg_apbslv 2: dbg_ctrl 3: dbg_decision 4: dbg_adc_arb Others: dbg_clkrst */
#define CT_DBG_SEL(x)                    (((u32)((x) & 0x00000007) << 0))
#define CT_GET_DBG_SEL(x)                ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup CT_DEBUG_PORT
 * @brief Debug Register
 * @{
 **/
#define CT_MASK_DBG_PORT                 ((u32)0xFFFFFFFF << 0)          /*!<R 0h  Debug port output value, depending on dbg_sel value */
#define CT_DBG_PORT(x)                   (((u32)((x) & 0xFFFFFFFF) << 0))
#define CT_GET_DBG_PORT(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup CT_ECO_USE0
 * @brief ECO USE0 Register
 * @{
 **/
#define CT_MASK_ECO_USE0                 ((u32)0xFFFFFFFF << 0)          /*!<R/W 0h  For ECO use */
#define CT_ECO_USE0(x)                   (((u32)((x) & 0xFFFFFFFF) << 0))
#define CT_GET_ECO_USE0(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup CT_ECO_USE1
 * @brief ECO USE1 Register
 * @{
 **/
#define CT_MASK_ECO_USE1                 ((u32)0xFFFFFFFF << 0)          /*!<R/W 0h  For ECO use */
#define CT_ECO_USE1(x)                   (((u32)((x) & 0xFFFFFFFF) << 0))
#define CT_GET_ECO_USE1(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup CT_CTC_COMP_VERSION
 * @brief Version Register
 * @{
 **/
#define CT_MASK_COMP_VERSION             ((u32)0xFFFFFFFF << 0)          /*!<R/W 20180123h  CTC version number */
#define CT_COMP_VERSION(x)               (((u32)((x) & 0xFFFFFFFF) << 0))
#define CT_GET_COMP_VERSION(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup CT_CHx_CTRL
 * @brief Channel x Control Register
 * @{
 **/
#define CT_MASK_CHx_D_TOUCH_TH           ((u32)0x00000FFF << 16)          /*!<R/W 64h  Difference threshold data of touch judgement for channel x. It needs to be configured during development (0x0~0xFFF) (0~4095). Init number=0x1E Recommend data=80%*(signal-baseline) */
#define CT_CHx_D_TOUCH_TH(x)             (((u32)((x) & 0x00000FFF) << 16))
#define CT_GET_CHx_D_TOUCH_TH(x)         ((u32)(((x >> 16) & 0x00000FFF)))
#define CT_MASK_CHx_BASELINE             ((u32)0x00000FFF << 4)          /*!<R/W/ES 0h  Digital baseline data of channel x. Init number=0x0 It can be initialed by HW when baseline initial function enabled, and can be updated automatically by HW when ETC function enabled. It could be configured by manual tuning. */
#define CT_CHx_BASELINE(x)               (((u32)((x) & 0x00000FFF) << 4))
#define CT_GET_CHx_BASELINE(x)           ((u32)(((x >> 4) & 0x00000FFF)))
#define CT_BIT_CHx_EN                    ((u32)0x00000001 << 0)          /*!<R/W 0h  Cap sensor activity control of channel x 0: Disable 1: Enable */
/** @} */

/** @defgroup CT_CHx_NOISE_TH
 * @brief Channel x Noise Threshold Register
 * @{
 **/
#define CT_MASK_CHx_N_ENT                ((u32)0x00000FFF << 16)          /*!<R/W 28h  The environmental negative noise threshold: the negative maximum capacitance change of raw data that is still considered an environment change. */
#define CT_CHx_N_ENT(x)                  (((u32)((x) & 0x00000FFF) << 16))
#define CT_GET_CHx_N_ENT(x)              ((u32)(((x >> 16) & 0x00000FFF)))
#define CT_MASK_CHx_P_ENT                ((u32)0x00000FFF << 0)          /*!<R/W 28h  The environmental positive noise threshold: the positive maximum capacitance change of raw data that is still considered an environment change. */
#define CT_CHx_P_ENT(x)                  (((u32)((x) & 0x00000FFF) << 0))
#define CT_GET_CHx_P_ENT(x)              ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup CT_CHx_MBIAS_ATH
 * @brief Channel x Mbias
 * @{
 **/
#define CT_MASK_CHx_A_TOUCH_TH           ((u32)0x00000FFF << 16)          /*!<R/W/ES 0h  Absolute threshold data of touch judgement for channel x. It can be updated by HW when baseline initial function or ETC function enabled, a_touch_th=baseline-d_touch_th, manual tuning when ETC disabled (0x0~0xFFF). Recommended data=80%*signal */
#define CT_CHx_A_TOUCH_TH(x)             (((u32)((x) & 0x00000FFF) << 16))
#define CT_GET_CHx_A_TOUCH_TH(x)         ((u32)(((x >> 16) & 0x00000FFF)))
#define CT_MASK_CHx_MBIAS                ((u32)0x000000FF << 0)          /*!<R/W 20h  Touch mbias current [7:0] 32u/16u/8u/4u/2u/1u/0.5u/0.25u for channel x */
#define CT_CHx_MBIAS(x)                  (((u32)((x) & 0x000000FF) << 0))
#define CT_GET_CHx_MBIAS(x)              ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup CT_CHx_DATA_INF
 * @brief Channel x Dtouch Register
 * @{
 **/
#define CT_BIT_CHx_DIFF_DATA_POLARITY    ((u32)0x00000001 << 28)          /*!<R/W/ES 0h  Polarity of chx_diff_data 0: chx_data_ave<baseline 1: chx_data_ave>=baseline */
#define CT_MASK_CHx_DIFF_DATA            ((u32)0x00000FFF << 16)          /*!<R/W/ES 0h  Difference digital data of channelx between chx_data_ave and baseline */
#define CT_CHx_DIFF_DATA(x)              (((u32)((x) & 0x00000FFF) << 16))
#define CT_GET_CHx_DIFF_DATA(x)          ((u32)(((x >> 16) & 0x00000FFF)))
#define CT_MASK_CHx_DATA_AVE             ((u32)0x00000FFF << 0)          /*!<R/W/ES 0h  Average of channel x raw code */
#define CT_CHx_DATA_AVE(x)               (((u32)((x) & 0x00000FFF) << 0))
#define CT_GET_CHx_DATA_AVE(x)           ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup CT_CTC_SAMPLE_CTRL
 * @brief Cap-Touch Sample Control Register
 * @{
 **/
#define CT_MASK_SETTLE_CNT               ((u32)0x0000000F << 4)          /*!<R/W 0h  Settle count for sample data from ADC to Cap-Touch, the unit is sample clock. 0 means no need to settle */
#define CT_SETTLE_CNT(x)                 (((u32)((x) & 0x0000000F) << 4))
#define CT_GET_SETTLE_CNT(x)             ((u32)(((x >> 4) & 0x0000000F)))
#define CT_MASK_CTC_CLK_DIV              ((u32)0x00000007 << 0)          /*!<R/W 4h  This field defines clock driver level of ADC module. The source clock is OSC4M. A value of 0 is for clock divided by 4. A value of 1 is for clock divided by 8. A value of 2 is for clock divided by 16. A value of 3 is for clock divided by 24. A value of 4 is for clock divided by 32. A value of 5 is for clock divided by 64. A value of 6 is for clock divided by 128. */
#define CT_CTC_CLK_DIV(x)                (((u32)((x) & 0x00000007) << 0))
#define CT_GET_CTC_CLK_DIV(x)            ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup CT_CTC_HW_TestMode
 * @brief Cap-Touch_Hardware_Test_Mode  Register
 * @{
 **/
#define CT_BIT_HW_CLOCK_TIE_VALUE        ((u32)0x00000001 << 1)          /*!<R/W 0h  this value is the clock tied value in Cap-Touch hardware test mode */
#define CT_BIT_HW_TMODE_EN               ((u32)0x00000001 << 0)          /*!<R/W 0h  0: hardware test mode is disabled. 1: hardware test mode is enabled. The clock sent to AUX Analog from Cap-Touch is tied to fixed value, which depentS on hw_clock_tie_value (410h[1]). */
/** @} */

/** @defgroup CT_CTC_FILTER_CTRL0
 * @brief Cap-Touch Filter Control0 Register
 * @{
 **/
#define CT_MASK_MDEC                     ((u32)0x0000FFFF << 16)          /*!<R/W Ah  Decimation of sample point */
#define CT_MDEC(x)                       (((u32)((x) & 0x0000FFFF) << 16))
#define CT_GET_MDEC(x)                   ((u32)(((x >> 16) & 0x0000FFFF)))
#define CT_MASK_MED_COEF                 ((u32)0x00000003 << 2)          /*!<R/W 1  No use Median filter coefficient 00: disable 01: coefficient is 3 10: coefficient is 5 11: coefficient is 7 */
#define CT_MED_COEF(x)                   (((u32)((x) & 0x00000003) << 2))
#define CT_GET_MED_COEF(x)               ((u32)(((x >> 2) & 0x00000003)))
#define CT_BIT_IIR_EN                    ((u32)0x00000001 << 1)          /*!<R/W 0  Enable CTC IIR function */
#define CT_BIT_FLT_EN                    ((u32)0x00000001 << 0)          /*!<R/W 0  Enable CTC filter function (global) */
/** @} */

/** @defgroup CT_CTC_FILTER_CTRL1
 * @brief Cap-Touch Filter Control1 Register
 * @{
 **/
#define CT_MASK_IIR_GAIN                 ((u32)0x0000FFFF << 16)          /*!<R/W 25Dh  Gain coefficient, Q1.15 */
#define CT_IIR_GAIN(x)                   (((u32)((x) & 0x0000FFFF) << 16))
#define CT_GET_IIR_GAIN(x)               ((u32)(((x >> 16) & 0x0000FFFF)))
#define CT_MASK_IIR_A1                   ((u32)0x0000FFFF << 0)          /*!<R/W C4BBh  Denominator coefficient, Q1.15 */
#define CT_IIR_A1(x)                     (((u32)((x) & 0x0000FFFF) << 0))
#define CT_GET_IIR_A1(x)                 ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */
/** @} */

/* Exported types --------------------------------------------------------*/

/** @defgroup CAPTOUCH_Exported_Types CAPTOUCH Exported Types
  * @{
*****************************************************************************/

/** @brief CAPTOUCH port
*****************************************************************************/
typedef struct {

	__IO uint32_t CT_CHx_CTRL;                            /*!< CHANNEL x CONTROL REGISTER */
	__IO uint32_t CT_CHx_NOISE_TH;                        /*!< CHANNEL x NOISE THRESHOLD REGISTER */
	__IO uint32_t CT_CHx_MBIAS_ATH;                       /*!< CHANNEL x MBIAS Register */
	__IO uint32_t CT_CHx_DATA_INF;                        /*!< CHANNEL x DTOUCH REGISTER */
} CT_CH_TypeDef;





/**
 * @brief CAPTOUCH Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t CT_CTC_CTRL;                            /*!< CAP-TOUCH CONTROL REGISTER,  Address offset: 0x000 */
	__IO uint32_t CT_SCAN_PERIOD;                         /*!< SCAN PARAMETERS REGISTER,  Address offset: 0x004 */
	__IO uint32_t CT_ETC_CTRL;                            /*!< ENVIRONMENT TRACKING CONTROL REGISTER,  Address offset: 0x008 */
	__IO uint32_t CT_SNR_INF;                             /*!< SIGNAL NOISE RATIO REGISTER,  Address offset: 0x00C */
	__IO uint32_t CT_DEBUG_MODE_CTRL;                     /*!< DEBUG AND CHANNEL SWITH MODE REGISTER,  Address offset: 0x010 */
	__IO uint32_t CT_RAW_CODE_FIFO_STATUS;                /*!< FIFO STATUS REGISTER,  Address offset: 0x014 */
	__IO uint32_t CT_RAW_CODE_FIFO_READ;                  /*!< FIFO READ REGISTER,  Address offset: 0x018 */
	__IO uint32_t RSVD0;                                  /*!<  Reserved,  Address offset:0x1C */
	__IO uint32_t CT_INTERRUPT_ENABLE;                    /*!< INTERRUPT ENABLE REGISTER,  Address offset: 0x020 */
	__I  uint32_t CT_INTERRUPT_STATUS;                    /*!< INTERRUPT STATUS REGISTER,  Address offset: 0x024 */
	__I  uint32_t CT_RAW_INTERRUPT_STATUS;                /*!< RAW INTERRUPT STATUS REGISTER,  Address offset: 0x028 */
	__IO uint32_t RSVD1;                                  /*!<  Reserved,  Address offset:0x2C */
	__IO uint32_t CT_INTERRUPT_ALL_CLR;                   /*!< INTERRUPT ALL CLEAR REGISTER,  Address offset: 0x030 */
	__IO uint32_t CT_INTERRUPT_STATUS_CLR;                /*!< INTERRUPT CLEAR REGISTER,  Address offset: 0x034 */
	__IO uint32_t RSVD2[2];                               /*!<  Reserved,  Address offset:0x38-0x3C */
	__IO uint32_t CT_DEBUG_SEL;                           /*!< DEBUG SELECT REGISTER,  Address offset: 0x040 */
	__I  uint32_t CT_DEBUG_PORT;                          /*!< DEBUG REGISTER,  Address offset: 0x044 */
	__IO uint32_t CT_ECO_USE0;                            /*!< ECO USE0 REGISTER,  Address offset: 0x048 */
	__IO uint32_t CT_ECO_USE1;                            /*!< ECO USE1 REGISTER,  Address offset: 0x04C */
	__IO uint32_t CT_CTC_COMP_VERSION;                    /*!< VERSION REGISTER,  Address offset: 0x050 */
	__IO uint32_t RSVD3[43];                              /*!<  Reserved,  Address offset:0x54-0xFC */
	CT_CH_TypeDef CT_CH[4];                               /*!< CAPTOUCH IP have 4 ports,  Address offset: 0x100-0x13C */
	__IO uint32_t RSVD4[48];                              /*!<  Reserved,  Address offset:0x140-0x1FC */
	__IO uint32_t CT_CTC_SAMPLE_CTRL;                     /*!< CAP-TOUCH SAMPLE CONTROL REGISTER,  Address offset: 0x200 */
	__IO uint32_t CT_CTC_HW_TESTMODE;                     /*!< CAP-TOUCH_HARDWARE_TEST_MODE  REGISTER,  Address offset: 0x204 */
	__IO uint32_t CT_CTC_FILTER_CTRL0;                    /*!< CAP-TOUCH FILTER CONTROL0 REGISTER,  Address offset: 0x208 */
	__IO uint32_t CT_CTC_FILTER_CTRL1;                    /*!< CAP-TOUCH FILTER CONTROL1 REGISTER,  Address offset: 0x20C */
	__IO uint32_t CT_CTC_DUMMY;                           /*!< CAP-TOUCH DUMMY REGISTER,  Address offset: 0x210 */
} CAPTOUCH_TypeDef;
/** @} */
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your defination here

/* Exported types --------------------------------------------------------*/
/** @addtogroup CAPTOUCH_Exported_Types CAPTOUCH Exported Types
  * @{
  */

/**
  * @brief  CapTouch Channel Init Structure Definition
  */
typedef struct {

	u16 CT_DiffThrehold;		/*!< Difference threshold data of touch judgement for channelx :
							1. Configured during development; (0x0~0xFFF) (0~4095)
							2. Init number=0x0, need to be configured
							3. recommend data=80%*(signal-baseline); */

	u8 CT_MbiasCurrent;		/*!< Channelx mbias current tuning(sensitivity tuning).
							Touch bias current BIT[7] ~ BIT[0]: 32uA/16uA/8uA/4uA/2uA/1uA/0.5uA/0.25uA.
							The sensitivity parameter is used to increase or decrease the strength
							of the sensor signal (difference count). A higher value of sensitivity (bias current)
							setting leads to a stronger signal from the sensors (more difference
							count for the same capacitance change), but also increases the response
							time and average power consumption.*/

	u16 CT_ETCNNoiseThr;		/*!< Specify negetive noise threshold of ETC.
							This parameter must be set to a value in the 0x1-0xfff range.
							The noise threshold indicates the raw data of the maximum capacitance change
							caused by environmental change. The CTC system provides configurable
							positive noise threshold and negative noise threshold for optimal calibration.
							The recommend value of noise threshold is 40%* touch threshold. Users need
							tune the two noise thresholds for different applications and noise environment.*/

	u16 CT_ETCPNoiseThr;		/*!< Specify positive threshold of ETC.
							This parameter must be set to a value in the 0x1-0xfff range. */

	u8 CT_CHEnable;			/*!< Specify this channel is enable or not */
} CapTouch_CHInitTypeDef;

/**
  * @brief  CapTouch Init Structure Definition
  */
typedef struct {
	u32 CT_DebounceEn;			/*!< Specify CapTouch press event Debounce Enable.
							This parameter must be set to a value in the 0x0-1 range. The de-bounce
							parameter can be configured by register CTC_CTRL.
							For example, when de-bounce is enabled and configured as 00 (2 times scan),
							finger touch interrupt will not be sent to the host until 2 times continue
							finger touch event is detected. Sensor de-bounce function is suitable for
							both button application and proximity detection.*/

	u32 CT_SampleCnt;			/*!< Specify sample cnt for average function,sample cnt = 2*exp(CT_SampleCnt+2).
							This parameter can be a value of 0x0-0x7*/

	u32 CT_ScanInterval;			/*!< Specify scan interval of every key.
							This parameter must be set to a value in the 0x1-0xfff range. */

	u32 CT_ETCStep;				/*!< Specify baseline update setp of ETC.
							This parameter must be set to a value in the 0x0-0xfff range. */

	u32 CT_ETCFactor;			/*!< Specify CapTouch ETC Factor.
							This parameter must be set to a value in the 0x0-0xf range. */

	u32 CT_ETCScanInterval;		/*!< Specify ETC scan interval
							This parameter can be set to a value in the 0x1-0x7f range*/

	CapTouch_CHInitTypeDef CT_Channel[9];			/*!< Specify the initialization parameters for each channel */

} CapTouch_InitTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup CAPTOUCH_Exported_Constants CAPTOUCH Exported Constants
  * @{
  */

/** @defgroup CapTouch_Peripheral_Definitions
  * @{
  */
#define IS_CAPTOUCH_ALL_PERIPH(PERIPH) ((PERIPH) == CAPTOUCH_DEV)
/**
  * @}
  */

/** @defgroup CapTouch_INT_Operation
  * @{
  */
#define CT_CHX_PRESS_INT(x)				BIT(x)
#define CT_CHX_RELEASE_INT(x)			BIT((x) + 8)

#define CT_ALL_INT_EN					(CT_MASK_TOUCH_PRESS_INTR_EN | \
											CT_MASK_TOUCH_RELEASE_INTR_EN | \
											CT_BIT_OVER_P_NOISE_TH_INTR_EN |\
											CT_BIT_AFIFO_OVERFLOW_INTR_EN |\
											CT_BIT_OVER_N_NOISE_TH_INTR_EN |\
											CT_BIT_AFIFO_OVERLVL_INTR_EN |\
											CT_BIT_CTC_BRK_BY_ADC_INTR_EN)

#define IS_CT_INT_EN(IT)				(((IT) & ~CT_ALL_INT_EN) == 0)
#define IS_CT_INT_CLR(IT)				(((IT) & ~CT_ALL_INT_EN) == 0)
/**
  * @}
  */

/** @defgroup CapTouch_Chn_Selection
  * @{
  */
#define CTC_CH0							((u8)0x00)
#define CTC_CH1							((u8)0x01)
#define CTC_CH2							((u8)0x02)
#define CTC_CH3							((u8)0x03)

#define CT_CHANNEL_NUM					(4)
#define IS_CT_CHANNEL(CHANNEL_NUM)		(CHANNEL_NUM < CT_CHANNEL_NUM)
/**
  * @}
  */

/** @defgroup CapTouch_Chn_Pad_Selection
  * @{
  */
#define CTC_CH0_PIN						(_PB_19)
#define CTC_CH1_PIN						(_PB_18)
#define CTC_CH2_PIN						(_PB_17)
#define CTC_CH3_PIN						(_PB_16)
/**
  * @}
  */

/** @defgroup CapTouch_Noise_Threshold_Type
  * @{
  */
#define  P_NOISE_THRES					0
#define  N_NOISE_THRES					1
/**
  * @}
  */

/** @defgroup CapTouch_Data_Setting
  * @{
  */
#define CTC_ID_AND_DATA(x)				((u32)((x) & 0x0000FFFF))
/**
  * @}
  */

/** @defgroup CapTouch_Sample_Decimation
  * @{
  */
#define IS_DEC_M(x)						((x) < 0x10000)
/**
  * @}
  */

/** @defgroup CapTouch_Clock_Source
  * @{
  */
#define CTC_CLK_OSC					((u8)0x00)
#define CTC_CLK_XTAL				((u8)0x01)

#define IS_CTC_CLK_SRC(x)			((x) <= CTC_CLK_XTAL)

/**
  * @}
  */

/** @defgroup CapTouch_Clock_Division
  * @{
  */
#define CTC_CLK_DIV4					((u8)0x00)
#define CTC_CLK_DIV8					((u8)0x01)
#define CTC_CLK_DIV16					((u8)0x02)
#define CTC_CLK_DIV24					((u8)0x03)
#define CTC_CLK_DIV32					((u8)0x04)
#define CTC_CLK_DIV64					((u8)0x05)
#define CTC_CLK_DIV128					((u8)0x06)

#define IS_CTC_CLK_DIV(x)				((x) <= CTC_CLK_DIV128)

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
void CapTouch_StructInit(CapTouch_InitTypeDef *CapTouch_InitStruct);
void CapTouch_Init(CAPTOUCH_TypeDef *CapTouch, CapTouch_InitTypeDef *CapTouch_InitStruct);
void CapTouch_Cmd(CAPTOUCH_TypeDef *CapTouch, u8 NewState);
void CapTouch_INTConfig(CAPTOUCH_TypeDef *CapTouch, uint32_t CapTouch_IT, u8 newState);
void CapTouch_INTClearPendingBit(CAPTOUCH_TypeDef *CapTouch, u32 CapTouch_IT);
u32 CapTouch_GetRawISR(CAPTOUCH_TypeDef *CapTouch);
u32 CapTouch_GetISR(CAPTOUCH_TypeDef *CapTouch);
void CapTouch_SetClkPara(CAPTOUCH_TypeDef *CapTouch, u8 ClkSrc, u8 ClkDiv);

void CapTouch_SetScanInterval(CAPTOUCH_TypeDef *CapTouch, u32 Interval);
void CapTouch_ChCmd(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u8 NewState);
u32 CapTouch_GetChStatus(CAPTOUCH_TypeDef *CapTouch, u32 Channel);
void CapTouch_SetChDiffThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u32 Threshold);
void CapTouch_SetChMbias(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u8 Mbias);
u32 CapTouch_GetChMbias(CAPTOUCH_TypeDef *CapTouch, u8 Channel);
u32 CapTouch_GetChDiffThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel);
u32 CapTouch_GetChAbsThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel);
void CapTouch_SetNNoiseThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u16 Threshold);
void CapTouch_SetPNoiseThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u16 Threshold);
u32 CapTouch_GetNoiseThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u8 type);
u32 CapTouch_GetChBaseline(CAPTOUCH_TypeDef *CapTouch, u8 Channel);
u32 CapTouch_GetChAveData(CAPTOUCH_TypeDef *CapTouch, u8 Channel);

void CapTouch_FilterCmd(CAPTOUCH_TypeDef *CapTouch, u8 NewState);
void CapTouch_FilterIIRCmd(CAPTOUCH_TypeDef *CapTouch, u8 NewState);
void CapTouch_SetMDec(CAPTOUCH_TypeDef *CapTouch, u32 MDec);
u32 CapTouch_GetMDec(CAPTOUCH_TypeDef *CapTouch, u32 ACFreq);
void CapTouch_SetMedCoef(CAPTOUCH_TypeDef *CapTouch, u32 MedCoef);
void CapTouch_SetFifoLevel(CAPTOUCH_TypeDef *CapTouch, u8 Level);

void CapTouch_DbgCmd(CAPTOUCH_TypeDef *CapTouch, u8 NewState);
void CapTouch_DbgChannel(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u8 NewState);
void CapTouch_DbgContCmd(CAPTOUCH_TypeDef *CapTouch, u8 NewState);
void CapTouch_DbgDumpReg(CAPTOUCH_TypeDef *CapTouch);
void CapTouch_DbgDumpETC(CAPTOUCH_TypeDef *CapTouch, u32 ch);
u32 CapTouch_DbgRawData(CAPTOUCH_TypeDef *CapTouch);


/* MANUAL_GEN_END */

#endif

