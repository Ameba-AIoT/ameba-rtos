/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_AUDIO_H_
#define _AMEBA_AUDIO_H_

/* AUTO_GEN_START */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup AUDIO
* @brief AUDIO driver modules
* @{
*/

/** @defgroup AUDIO_CODEC
* @brief AUDIO_CODEC driver modules
* @{
*/

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup AUDIO_CODEC_Register_Definitions AUDIO_CODEC Register Definitions
 * @{
 *****************************************************************************/

/** @defgroup CODEC_audio_control_0
 * @brief Audio Control Register 0
 * @{
 **/
#define AUD_BIT_AUDIO_IP_EN                  ((u32)0x00000001 << 0)          /*!<R/W 0   */
#define AUD_BIT_DAAD_LPBK_EN                 ((u32)0x00000001 << 1)          /*!<R/W 0  Digital DAC -> ADC loop back control 1'b0: Disable 1'b1: Enable */
#define AUD_BIT_ADDA_LPBK_EN                 ((u32)0x00000001 << 2)          /*!<R/W 0  Digital ADC -> DAC loop back control 1'b0: Disable 1'b1: Enable */
#define AUD_BIT_AUDIO_CONTROL_0_DUMMY        ((u32)0x00000001 << 3)          /*!<R/W 0  Dummy */
#define AUD_MASK_SYS_CLK_RATE_SEL            ((u32)0x00000003 << 4)          /*!<R/W 0x2  audio_sys clock selection */
#define AUD_SYS_CLK_RATE_SEL(x)              (((u32)((x) & 0x00000003) << 4))
#define AUD_GET_SYS_CLK_RATE_SEL(x)          ((u32)(((x >> 4) & 0x00000003)))
#define AUD_BIT_RWS_SEQ_EN                   ((u32)0x00000001 << 6)          /*!<R/W 0  1'b0: Disable 1'b1: Enable */
#define AUD_MASK_AUDIO_DBG_SEL               ((u32)0x0000001F << 13)          /*!<R/W 0  Debug probe selection */
#define AUD_AUDIO_DBG_SEL(x)                 (((u32)((x) & 0x0000001F) << 13))
#define AUD_GET_AUDIO_DBG_SEL(x)             ((u32)(((x >> 13) & 0x0000001F)))
/** @} */

/** @defgroup CODEC_audio_control_1
 * @brief Audio Control Register 1
 * @{
 **/
#define AUD_MASK_SEL_BB_CK_DEPOP             ((u32)0x00000003 << 0)          /*!<R/W 1  Select BB clock depop */
#define AUD_SEL_BB_CK_DEPOP(x)               (((u32)((x) & 0x00000003) << 0))
#define AUD_GET_SEL_BB_CK_DEPOP(x)           ((u32)(((x >> 0) & 0x00000003)))
#define AUD_BIT_BB_CK_DEPOP_EN               ((u32)0x00000001 << 2)          /*!<R/W 0  BB clcok depop enable */
#define AUD_BIT_CKX_MICBIAS_EN               ((u32)0x00000001 << 3)          /*!<R/W 0  ckx_micbias enable */
#define AUD_BIT_PDM_CLK_DOUBLE               ((u32)0x00000001 << 4)          /*!<R/W 0  PDM clock double */
#define AUD_MASK_PDM_CLK_SEL                 ((u32)0x00000003 << 5)          /*!<R/W 0  PDM clock selection 2'b0: 5MHz 2'b1: 2.5MHz 2'b2: 6.67MHz */
#define AUD_PDM_CLK_SEL(x)                   (((u32)((x) & 0x00000003) << 5))
#define AUD_GET_PDM_CLK_SEL(x)               ((u32)(((x >> 5) & 0x00000003)))
#define AUD_MASK_PDM_CH_SWAP                 ((u32)0x00000003 << 7)          /*!<R/W 0x2  pdm_ch_swap */
#define AUD_PDM_CH_SWAP(x)                   (((u32)((x) & 0x00000003) << 7))
#define AUD_GET_PDM_CH_SWAP(x)               ((u32)(((x >> 7) & 0x00000003)))
#define AUD_BIT_PDM_DATA_PHASE_SEL           ((u32)0x00000001 << 9)          /*!<R/W 1  pdm_data_phase_sel */
#define AUD_BIT_PDM_GAIN_SHIFT_EN            ((u32)0x00000001 << 10)          /*!<R/W 1  pdm_gain_shift_en */
#define AUD_BIT_PDM_CLK_INV_SEL              ((u32)0x00000001 << 11)          /*!<R/W 0  pdm_clk_inv_sel */
#define AUD_BIT_I2S_DATA_RND_EN              ((u32)0x00000001 << 12)          /*!<R/W 1  I2S data read enable */
#define AUD_MASK_AUDIO_CONTROL_1_DUMMY       ((u32)0x0000000F << 13)          /*!<R/W 0  Dummy */
#define AUD_AUDIO_CONTROL_1_DUMMY(x)         (((u32)((x) & 0x0000000F) << 13))
#define AUD_GET_AUDIO_CONTROL_1_DUMMY(x)     ((u32)(((x >> 13) & 0x0000000F)))
/** @} */

/** @defgroup CODEC_clock_control_1
 * @brief Clock Control Register 1
 * @{
 **/
#define AUD_BIT_DA_L_EN                      ((u32)0x00000001 << 0)          /*!<R/W 0  DAC L channel clock enable */
#define AUD_BIT_CLOCK_CONTROL_1_DUMMY0       ((u32)0x00000001 << 1)          /*!<R/W 0  Dummy */
#define AUD_BIT_MOD_L_EN                     ((u32)0x00000001 << 2)          /*!<R/W 0  SDM L channel clock enable */
#define AUD_BIT_CLOCK_CONTROL_1_DUMMY1       ((u32)0x00000001 << 3)          /*!<R/W 0  Dummy */
#define AUD_BIT_DA_ANA_CLK_EN                ((u32)0x00000001 << 4)          /*!<R/W 0  DAC/ADC analog clock enable */
#define AUD_BIT_DA_FIFO_EN                   ((u32)0x00000001 << 5)          /*!<R/W 0  DAC FIFO clock enable */
#define AUD_BIT_AD_0_EN                      ((u32)0x00000001 << 6)          /*!<R/W 0  ADC channel 0 clock enable */
#define AUD_BIT_AD_1_EN                      ((u32)0x00000001 << 7)          /*!<R/W 0  ADC channel 1 clock enable */
#define AUD_BIT_AD_2_EN                      ((u32)0x00000001 << 8)          /*!<R/W 0  ADC channel 2 clock enable */
#define AUD_BIT_AD_3_EN                      ((u32)0x00000001 << 9)          /*!<R/W 0  ADC channel 3 clock enable */
#define AUD_BIT_AD_0_FIFO_EN                 ((u32)0x00000001 << 14)          /*!<R/W 0  ADC channel 0 FIFO clock enable */
#define AUD_BIT_AD_1_FIFO_EN                 ((u32)0x00000001 << 15)          /*!<R/W 0  ADC channel 1 FIFO clock enable */
#define AUD_BIT_AD_2_FIFO_EN                 ((u32)0x00000001 << 16)          /*!<R/W 0  ADC channel 2 FIFO clock enable */
#define AUD_BIT_AD_3_FIFO_EN                 ((u32)0x00000001 << 17)          /*!<R/W 0  ADC channel 3 FIFO clock enable */
#define AUD_BIT_AD_ANA_CLK_EN                ((u32)0x00000001 << 22)          /*!<R/W 0  ADC analog clock enable */
/** @} */

/** @defgroup CODEC_clock_control_2
 * @brief Clock Control Register 2
 * @{
 **/
#define AUD_BIT_AD_ANA_0_EN                  ((u32)0x00000001 << 0)          /*!<R/W 0  ADC filter channel 0 clock enable: analog ADC path */
#define AUD_BIT_AD_ANA_1_EN                  ((u32)0x00000001 << 1)          /*!<R/W 0  ADC filter channel 1 clock enable: analog ADC path */
#define AUD_BIT_AD_ANA_2_EN                  ((u32)0x00000001 << 2)          /*!<R/W 0  ADC filter channel 2 clock enable: analog ADC path */
#define AUD_BIT_AD_ANA_3_EN                  ((u32)0x00000001 << 3)          /*!<R/W 0  ADC filter channel 3 clock enable: analog ADC path */
#define AUD_BIT_DMIC_0_EN                    ((u32)0x00000001 << 8)          /*!<R/W 0  ADC filter channel 0 clock enable: DMIC path */
#define AUD_BIT_DMIC_1_EN                    ((u32)0x00000001 << 9)          /*!<R/W 0  ADC filter channel 1 clock enable: DMIC path */
#define AUD_BIT_DMIC_2_EN                    ((u32)0x00000001 << 10)          /*!<R/W 0  ADC filter channel 2 clock enable: DMIC path */
#define AUD_BIT_DMIC_3_EN                    ((u32)0x00000001 << 11)          /*!<R/W 0  ADC filter channel 3 clock enable: DMIC path */
/** @} */

/** @defgroup CODEC_clock_control_3
 * @brief Clock Control Register 3
 * @{
 **/
#define AUD_MASK_DMIC1_CLK_SEL               ((u32)0x00000007 << 0)          /*!<R/W 1  Set clock of digital microphone 3'b000: 5MHz 3'b001: 2.5MHz 3'b010: 1.25MHz 3'b011: 625kHz 3'b100: 312.5kHz 3'b101: Reserved 3'b110: Reserved 3'b111: 769.2kHz */
#define AUD_DMIC1_CLK_SEL(x)                 (((u32)((x) & 0x00000007) << 0))
#define AUD_GET_DMIC1_CLK_SEL(x)             ((u32)(((x >> 0) & 0x00000007)))
#define AUD_BIT_DMIC1_CLK_EN                 ((u32)0x00000001 << 3)          /*!<R/W 0  Digital microphone clock enable */
/** @} */

/** @defgroup CODEC_clock_control_4
 * @brief Clock Control Register 4
 * @{
 **/
#define AUD_MASK_SAMPLE_RATE_0               ((u32)0x0000000F << 0)          /*!<R/W 0  Set sample rate source 0 4'h0: 48K 4'h1: 96K 4'h2: 192K 4'h3: 32K 4'h4: 176.4K 4'h5: 16K 4'h6: Reserved 4'h7: 8K 4'h8: 44.1K 4'h9: 88.2K 4'ha:24K 4'hb: 12K 4'hc: 22.05K 4'hd: 11.025K 4'he~4'hf: Reserved */
#define AUD_SAMPLE_RATE_0(x)                 (((u32)((x) & 0x0000000F) << 0))
#define AUD_GET_SAMPLE_RATE_0(x)             ((u32)(((x >> 0) & 0x0000000F)))
#define AUD_MASK_SAMPLE_RATE_1               ((u32)0x0000000F << 4)          /*!<R/W 0  Set sample rate source 1 4'h0: 48K 4'h1: 96K 4'h2: 192K 4'h3: 32K 4'h4: 176.4K 4'h5: 16K 4'h6: Reserved 4'h7: 8K 4'h8: 44.1K 4'h9: 88.2K 4'ha:24K 4'hb: 12K 4'hc: 22.05K 4'hd: 11.025K 4'he~4'hf: Reserved */
#define AUD_SAMPLE_RATE_1(x)                 (((u32)((x) & 0x0000000F) << 4))
#define AUD_GET_SAMPLE_RATE_1(x)             ((u32)(((x >> 4) & 0x0000000F)))
#define AUD_MASK_SAMPLE_RATE_2               ((u32)0x0000000F << 8)          /*!<R/W 0  Set sample rate source 2 4'h0: 48K 4'h1: 96K 4'h2: 192K 4'h3: 32K 4'h4: 176.4K 4'h5: 16K 4'h6: Reserved 4'h7: 8K 4'h8: 44.1K 4'h9: 88.2K 4'ha:24K 4'hb: 12K 4'hc: 22.05K 4'hd: 11.025K 4'he~4'hf: Reserved */
#define AUD_SAMPLE_RATE_2(x)                 (((u32)((x) & 0x0000000F) << 8))
#define AUD_GET_SAMPLE_RATE_2(x)             ((u32)(((x >> 8) & 0x0000000F)))
#define AUD_MASK_SAMPLE_RATE_3               ((u32)0x0000000F << 12)          /*!<R/W 0  Set sample rate source 3 4'h0: 48K 4'h1: 96K 4'h2: 192K 4'h3: 32K 4'h4: 176.4K 4'h5: 16K 4'h6: Reserved 4'h7: 8K 4'h8: 44.1K 4'h9: 88.2K 4'ha:24K 4'hb: 12K 4'hc: 22.05K 4'hd: 11.025K 4'he~4'hf: Reserved */
#define AUD_SAMPLE_RATE_3(x)                 (((u32)((x) & 0x0000000F) << 12))
#define AUD_GET_SAMPLE_RATE_3(x)             ((u32)(((x >> 12) & 0x0000000F)))
/** @} */

/** @defgroup CODEC_clock_control_5
 * @brief Clock Control Register 5
 * @{
 **/
#define AUD_MASK_ADC_0_FS_SRC_SEL            ((u32)0x00000003 << 0)          /*!<R/W 0  Channel 0 ADC path sample rate source selection 2'b00: Source 0 2'b01: Source 1 2'b10: Source 2 2'b11: Source 3 */
#define AUD_ADC_0_FS_SRC_SEL(x)              (((u32)((x) & 0x00000003) << 0))
#define AUD_GET_ADC_0_FS_SRC_SEL(x)          ((u32)(((x >> 0) & 0x00000003)))
#define AUD_MASK_ADC_1_FS_SRC_SEL            ((u32)0x00000003 << 2)          /*!<R/W 0  Channel 1 ADC path sample rate source selection 2'b00: Source 0 2'b01: Source 1 2'b10: Source 2 2'b11: Source 3 */
#define AUD_ADC_1_FS_SRC_SEL(x)              (((u32)((x) & 0x00000003) << 2))
#define AUD_GET_ADC_1_FS_SRC_SEL(x)          ((u32)(((x >> 2) & 0x00000003)))
#define AUD_MASK_ADC_2_FS_SRC_SEL            ((u32)0x00000003 << 4)          /*!<R/W 0  Channel 2 ADC path sample rate source selection 2'b00: Source 0 2'b01: Source 1 2'b10: Source 2 2'b11: Source 3 */
#define AUD_ADC_2_FS_SRC_SEL(x)              (((u32)((x) & 0x00000003) << 4))
#define AUD_GET_ADC_2_FS_SRC_SEL(x)          ((u32)(((x >> 4) & 0x00000003)))
#define AUD_MASK_ADC_3_FS_SRC_SEL            ((u32)0x00000003 << 6)          /*!<R/W 0  Channel 3 ADC path sample rate source selection 2'b00: Source 0 2'b01: Source 1 2'b10: Source 2 2'b11: Source 3 */
#define AUD_ADC_3_FS_SRC_SEL(x)              (((u32)((x) & 0x00000003) << 6))
#define AUD_GET_ADC_3_FS_SRC_SEL(x)          ((u32)(((x >> 6) & 0x00000003)))
#define AUD_MASK_DAC_L_FS_SRC_SEL            ((u32)0x00000003 << 16)          /*!<R/W 0  DAC path sample rate source selection 2'b00: Source 0 2'b01: Source 1 2'b10: Source 2 2'b11: Source 3 */
#define AUD_DAC_L_FS_SRC_SEL(x)              (((u32)((x) & 0x00000003) << 16))
#define AUD_GET_DAC_L_FS_SRC_SEL(x)          ((u32)(((x >> 16) & 0x00000003)))
/** @} */

/** @defgroup CODEC_clock_control_6
 * @brief Clock Control Register 6
 * @{
 **/
#define AUD_BIT_ADC_0_ASRC_EN                ((u32)0x00000001 << 0)          /*!<R/W 0  Channel 0 ADC path ASRC enable 0: Disable 1: Enable If ASRC is enabled, sample_rate becomes useless. */
#define AUD_BIT_ADC_1_ASRC_EN                ((u32)0x00000001 << 1)          /*!<R/W 0  Channel 1 ADC path ASRC enable 0: Disable 1: Enable If ASRC is enabled, sample_rate becomes useless. */
#define AUD_BIT_ADC_2_ASRC_EN                ((u32)0x00000001 << 2)          /*!<R/W 0  Channel 2 ADC path ASRC enable 0: Disable 1: Enable If ASRC is enabled, sample_rate becomes useless. */
#define AUD_BIT_ADC_3_ASRC_EN                ((u32)0x00000001 << 3)          /*!<R/W 0  Channel 3 ADC path ASRC enable 0: Disable 1: Enable If ASRC is enabled, sample_rate becomes useless. */
#define AUD_BIT_DAC_L_ASRC_EN                ((u32)0x00000001 << 8)          /*!<R/W 0  DAC path ASRC enable 0: Disable 1: Enable If ASRC is enabled, sample_rate becomes useless. */
/** @} */

/** @defgroup CODEC_clock_control_7
 * @brief Clock Control Register 7
 * @{
 **/
#define AUD_BIT_DA_ANA_CLK_SEL               ((u32)0x00000001 << 0)          /*!<R/W 0  Set clk_da_ana phase 1'b0: Inphase 1'b1: Inverse phase */
#define AUD_BIT_AD_ANA_CLK_SEL               ((u32)0x00000001 << 1)          /*!<R/W 0  Set clk_ad_ana phase 1'b0: Inphase 1'b1: Inverse phase */
#define AUD_BIT_AD_LATCH_PHASE               ((u32)0x00000001 << 2)          /*!<R/W 0  Set latch ADC data phase 1'b0: Inphase 1'b1: Inverse phase */
#define AUD_MASK_ANA_CLK_RATE_SEL            ((u32)0x00000003 << 3)          /*!<R/W 0  Set DA/AD analog clock 0: 10MHz 1: 5MHz 2: 2.5MHz 3: Reserved */
#define AUD_ANA_CLK_RATE_SEL(x)              (((u32)((x) & 0x00000003) << 3))
#define AUD_GET_ANA_CLK_RATE_SEL(x)          ((u32)(((x >> 3) & 0x00000003)))
#define AUD_MASK_AD_LPF_CLK_RATE_SEL         ((u32)0x00000003 << 5)          /*!<R/W 0  Set AD LPF clock 0: 10MHz 1: 5MHz 2: 2.5MHz 3: Reserved */
#define AUD_AD_LPF_CLK_RATE_SEL(x)           (((u32)((x) & 0x00000003) << 5))
#define AUD_GET_AD_LPF_CLK_RATE_SEL(x)       ((u32)(((x >> 5) & 0x00000003)))
#define AUD_MASK_ADC_0_DMIC_LPF_CLK_SEL      ((u32)0x00000003 << 7)          /*!<R/W 0  Channel 0 ADC path DMIC LPF clock 2'b00: 10M 2'b01: 5M 2'b10: 2.5M 2'b11: 769.2*2K */
#define AUD_ADC_0_DMIC_LPF_CLK_SEL(x)        (((u32)((x) & 0x00000003) << 7))
#define AUD_GET_ADC_0_DMIC_LPF_CLK_SEL(x)    ((u32)(((x >> 7) & 0x00000003)))
#define AUD_MASK_ADC_1_DMIC_LPF_CLK_SEL      ((u32)0x00000003 << 9)          /*!<R/W 0  Channel 1 ADC path DMIC LPF clock 2'b00: 10M 2'b01: 5M 2'b10: 2.5M 2'b11: 769.2*2K */
#define AUD_ADC_1_DMIC_LPF_CLK_SEL(x)        (((u32)((x) & 0x00000003) << 9))
#define AUD_GET_ADC_1_DMIC_LPF_CLK_SEL(x)    ((u32)(((x >> 9) & 0x00000003)))
#define AUD_MASK_ADC_2_DMIC_LPF_CLK_SEL      ((u32)0x00000003 << 11)          /*!<R/W 0  Channel 2 ADC path DMIC LPF clock 2'b00: 10M 2'b01: 5M 2'b10: 2.5M 2'b11: 769.2*2K */
#define AUD_ADC_2_DMIC_LPF_CLK_SEL(x)        (((u32)((x) & 0x00000003) << 11))
#define AUD_GET_ADC_2_DMIC_LPF_CLK_SEL(x)    ((u32)(((x >> 11) & 0x00000003)))
#define AUD_MASK_ADC_3_DMIC_LPF_CLK_SEL      ((u32)0x00000003 << 13)          /*!<R/W 0  Channel 3 ADC path DMIC LPF clock 2'b00: 10M 2'b01: 5M 2'b10: 2.5M 2'b11: 769.2*2K */
#define AUD_ADC_3_DMIC_LPF_CLK_SEL(x)        (((u32)((x) & 0x00000003) << 13))
#define AUD_GET_ADC_3_DMIC_LPF_CLK_SEL(x)    ((u32)(((x >> 13) & 0x00000003)))
#define AUD_BIT_ANA_CLK_PHASE_SEL            ((u32)0x00000001 << 23)          /*!<R/W 0  1: Analog clock shift 80MHz phase 0: Disable */
#define AUD_BIT_AD_ANA_OSR_DOUBLE            ((u32)0x00000001 << 24)          /*!<R/W 0  1: Latch ADC data with 10MHz clock */
/** @} */

/** @defgroup CODEC_asrc_control_0
 * @brief ASRC Control Register 0
 * @{
 **/
#define AUD_MASK_ASRC_RATE_SEL_TX            ((u32)0x00000003 << 0)          /*!<R/W 0  0: fs supports 4kHz ~ 60kHz 1: fs supports 60kHz ~ 120kHz 2: fs supports 120kHz ~ 200kHz 3: Reserved */
#define AUD_ASRC_RATE_SEL_TX(x)              (((u32)((x) & 0x00000003) << 0))
#define AUD_GET_ASRC_RATE_SEL_TX(x)          ((u32)(((x >> 0) & 0x00000003)))
#define AUD_MASK_ASRC_GAIN_SEL_TX            ((u32)0x00000003 << 2)          /*!<R/W 0x3  ASRC convergence rate: larger is faster but more noisy */
#define AUD_ASRC_GAIN_SEL_TX(x)              (((u32)((x) & 0x00000003) << 2))
#define AUD_GET_ASRC_GAIN_SEL_TX(x)          ((u32)(((x >> 2) & 0x00000003)))
#define AUD_BIT_ASRC_AUTO_ADJUST_TX          ((u32)0x00000001 << 4)          /*!<R/W 1  HW auto adjust convergence rate 0: Disable 1: Enable */
/** @} */

/** @defgroup CODEC_asrc_control_1
 * @brief ASRC Control Register 1
 * @{
 **/
#define AUD_MASK_ASRC_SDM_INTI_TX            ((u32)0x00FFFFFF << 0)          /*!<R/W 0  Set initial value of tracked frequency */
#define AUD_ASRC_SDM_INTI_TX(x)              (((u32)((x) & 0x00FFFFFF) << 0))
#define AUD_GET_ASRC_SDM_INTI_TX(x)          ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */

/** @defgroup CODEC_asrc_control_2
 * @brief ASRC Control Register 2
 * @{
 **/
#define AUD_MASK_ASRC_RATE_SEL_RX_0          ((u32)0x00000003 << 0)          /*!<R/W 0  0: fs supports 4kHz ~ 60kHz 1: fs supports 60kHz ~ 120kHz 2/3: Reserved */
#define AUD_ASRC_RATE_SEL_RX_0(x)            (((u32)((x) & 0x00000003) << 0))
#define AUD_GET_ASRC_RATE_SEL_RX_0(x)        ((u32)(((x >> 0) & 0x00000003)))
#define AUD_MASK_ASRC_GAIN_SEL_RX_0          ((u32)0x00000003 << 2)          /*!<R/W 0x3  ASRC convergence rate: larger is faster but more noisy */
#define AUD_ASRC_GAIN_SEL_RX_0(x)            (((u32)((x) & 0x00000003) << 2))
#define AUD_GET_ASRC_GAIN_SEL_RX_0(x)        ((u32)(((x >> 2) & 0x00000003)))
#define AUD_BIT_ASRC_AUTO_ADJUST_RX_0        ((u32)0x00000001 << 4)          /*!<R/W 1  HW auto adjust convergence rate 0: Disable 1: Enable */
/** @} */

/** @defgroup CODEC_asrc_control_3
 * @brief ASRC Control Register 3
 * @{
 **/
#define AUD_MASK_ASRC_SDM_INTI_RX_0          ((u32)0x00FFFFFF << 0)          /*!<R/W 0  Set initial value of tracked frequency */
#define AUD_ASRC_SDM_INTI_RX_0(x)            (((u32)((x) & 0x00FFFFFF) << 0))
#define AUD_GET_ASRC_SDM_INTI_RX_0(x)        ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */

/** @defgroup CODEC_i2s_0_control
 * @brief I2S 0 Control Register
 * @{
 **/
#define AUD_BIT_I2S_0_RST_N_REG              ((u32)0x00000001 << 0)          /*!<R/W 0  1'b1: rst_n to audio digital IP is de-asserted 1'b0: rst_n to audio digital IP is asserted */
#define AUD_BIT_I2S_0_INV_SCLK               ((u32)0x00000001 << 1)          /*!<R/W 0  1'b1: I2S/PCM bit clock is inverted */
#define AUD_BIT_I2S_0_SELF_LPBK_EN           ((u32)0x00000001 << 2)          /*!<R/W 0  1'b1: Internal loopback mode is enabled */
#define AUD_BIT_I2S_0_SAME_LRC_EN            ((u32)0x00000001 << 3)          /*!<R/W 0  1'b1: LRC_TX share to LRC_RX */
#define AUD_MASK_I2S_0_TDM_MODE_RX           ((u32)0x00000003 << 4)          /*!<R/W 0  I2S 0 Rx channel TDM mode 0: 2 channels 1: 4 channels 2/3: Reserved */
#define AUD_I2S_0_TDM_MODE_RX(x)             (((u32)((x) & 0x00000003) << 4))
#define AUD_GET_I2S_0_TDM_MODE_RX(x)         ((u32)(((x >> 4) & 0x00000003)))
#define AUD_MASK_I2S_0_DATA_FORMAT_SEL_TX    ((u32)0x00000003 << 6)          /*!<R/W 0  I2S 0 Tx channel data format 2'b00: I2S 2'b01: Left-justified 2'b10: PCM mode A 2'b11: PCM mode B */
#define AUD_I2S_0_DATA_FORMAT_SEL_TX(x)      (((u32)((x) & 0x00000003) << 6))
#define AUD_GET_I2S_0_DATA_FORMAT_SEL_TX(x)  ((u32)(((x >> 6) & 0x00000003)))
#define AUD_MASK_I2S_0_DATA_FORMAT_SEL_RX    ((u32)0x00000003 << 8)          /*!<R/W 0  I2S 0 Rx channel data format 2'b00: I2S 2'b01: Left-justified 2'b10: PCM mode A 2'b11: PCM mode B */
#define AUD_I2S_0_DATA_FORMAT_SEL_RX(x)      (((u32)((x) & 0x00000003) << 8))
#define AUD_GET_I2S_0_DATA_FORMAT_SEL_RX(x)  ((u32)(((x >> 8) & 0x00000003)))
#define AUD_MASK_I2S_0_DATA_LEN_SEL_TX       ((u32)0x00000003 << 10)          /*!<R/W 0  I2S 0 Tx channel data length 2'b00: 16 bits 2'b10: 24 bits 2'b11: 8 bits */
#define AUD_I2S_0_DATA_LEN_SEL_TX(x)         (((u32)((x) & 0x00000003) << 10))
#define AUD_GET_I2S_0_DATA_LEN_SEL_TX(x)     ((u32)(((x >> 10) & 0x00000003)))
#define AUD_MASK_I2S_0_DATA_LEN_SEL_RX       ((u32)0x00000003 << 12)          /*!<R/W 0  I2S 0 Rx channel data length 2'b00: 16 bits 2'b10: 24 bits 2'b11: 8 bits */
#define AUD_I2S_0_DATA_LEN_SEL_RX(x)         (((u32)((x) & 0x00000003) << 12))
#define AUD_GET_I2S_0_DATA_LEN_SEL_RX(x)     ((u32)(((x >> 12) & 0x00000003)))
#define AUD_MASK_I2S_0_CH_LEN_SEL_TX         ((u32)0x00000003 << 14)          /*!<R/W 0  I2S 0 Tx channel length 2'b00: 16 bits 2'b01: 32 bits 2'b10: 24 bits 2'b11: 8 bits */
#define AUD_I2S_0_CH_LEN_SEL_TX(x)           (((u32)((x) & 0x00000003) << 14))
#define AUD_GET_I2S_0_CH_LEN_SEL_TX(x)       ((u32)(((x >> 14) & 0x00000003)))
#define AUD_MASK_I2S_0_CH_LEN_SEL_RX         ((u32)0x00000003 << 16)          /*!<R/W 0  I2S 0 Rx channel length 2'b00: 16 bits 2'b01: 32 bits 2'b10: 24 bits 2'b11: 8 bits */
#define AUD_I2S_0_CH_LEN_SEL_RX(x)           (((u32)((x) & 0x00000003) << 16))
#define AUD_GET_I2S_0_CH_LEN_SEL_RX(x)       ((u32)(((x >> 16) & 0x00000003)))
#define AUD_MASK_I2S_0_DATA_CH_SEL_TX        ((u32)0x00000003 << 18)          /*!<R/W 0  I2S 0 Tx channel data channel 2'b00: L/R 2'b01: R/L 2'b10: L/L 2'b11: R/R */
#define AUD_I2S_0_DATA_CH_SEL_TX(x)          (((u32)((x) & 0x00000003) << 18))
#define AUD_GET_I2S_0_DATA_CH_SEL_TX(x)      ((u32)(((x >> 18) & 0x00000003)))
#define AUD_BIT_I2S_0_MASTER_SEL             ((u32)0x00000001 << 20)          /*!<R/W 0  I2S 0 master source selection 1'b0: SPORT internal 1'b1: External I2S */
/** @} */

/** @defgroup CODEC_i2s_0_control_1
 * @brief I2S 0 Control Register 1
 * @{
 **/
#define AUD_MASK_I2S_0_DATA_CH0_SEL_RX       ((u32)0x00000007 << 0)          /*!<R/W 0  I2S 0 Rx channel data channel 0 0: CH0 1: CH1 2: CH2 3: CH3 4/5/6/7: Reserved */
#define AUD_I2S_0_DATA_CH0_SEL_RX(x)         (((u32)((x) & 0x00000007) << 0))
#define AUD_GET_I2S_0_DATA_CH0_SEL_RX(x)     ((u32)(((x >> 0) & 0x00000007)))
#define AUD_MASK_I2S_0_DATA_CH1_SEL_RX       ((u32)0x00000007 << 3)          /*!<R/W 0  I2S 0 Rx channel data channel 1 0: CH0 1: CH1 2: CH2 3: CH3 4/5/6/7: Reserved */
#define AUD_I2S_0_DATA_CH1_SEL_RX(x)         (((u32)((x) & 0x00000007) << 3))
#define AUD_GET_I2S_0_DATA_CH1_SEL_RX(x)     ((u32)(((x >> 3) & 0x00000007)))
#define AUD_MASK_I2S_0_DATA_CH2_SEL_RX       ((u32)0x00000007 << 6)          /*!<R/W 0  I2S 0 Rx channel data channel 2 0: CH0 1: CH1 2: CH2 3: CH3 4/5/6/7: Reserved */
#define AUD_I2S_0_DATA_CH2_SEL_RX(x)         (((u32)((x) & 0x00000007) << 6))
#define AUD_GET_I2S_0_DATA_CH2_SEL_RX(x)     ((u32)(((x >> 6) & 0x00000007)))
#define AUD_MASK_I2S_0_DATA_CH3_SEL_RX       ((u32)0x00000007 << 9)          /*!<R/W 0  I2S 0 Rx channel data channel 3 0: CH0 1: CH1 2: CH2 3: CH3 4/5/6/7: Reserved */
#define AUD_I2S_0_DATA_CH3_SEL_RX(x)         (((u32)((x) & 0x00000007) << 9))
#define AUD_GET_I2S_0_DATA_CH3_SEL_RX(x)     ((u32)(((x >> 9) & 0x00000007)))
#define AUD_BIT_I2S_0_DATA_CH0_RX_DISABLE    ((u32)0x00000001 << 24)          /*!<R/W 0  I2S 0 Rx channel data channel 0 1: disable 0: enable */
#define AUD_BIT_I2S_0_DATA_CH1_RX_DISABLE    ((u32)0x00000001 << 25)          /*!<R/W 0  I2S 0 Rx channel data channel 1 1: disable 0: enable */
#define AUD_BIT_I2S_0_DATA_CH2_RX_DISABLE    ((u32)0x00000001 << 26)          /*!<R/W 0  I2S 0 Rx channel data channel 2 1: disable 0: enable */
#define AUD_BIT_I2S_0_DATA_CH3_RX_DISABLE    ((u32)0x00000001 << 27)          /*!<R/W 0  I2S 0 Rx channel data channel 3 1: disable 0: enable */
/** @} */

/** @defgroup CODEC_adc_x_control_0
 * @brief ADC Channel x Control Register 0
 * @{
 **/
#define AUD_MASK_ADC_x_DMIC_SRC_SEL          ((u32)0x00000007 << 0)          /*!<R/W 0  Channel x DMIC source selection 3'b000: MIC 1 rising 3'b001: MIC 1 falling 3'b010: MIC 2 rising 3'b011: MIC 2 falling Other: Reserved */
#define AUD_ADC_x_DMIC_SRC_SEL(x)            (((u32)((x) & 0x00000007) << 0))
#define AUD_GET_ADC_x_DMIC_SRC_SEL(x)        ((u32)(((x >> 0) & 0x00000007)))
#define AUD_BIT_ADC_x_DMIC_LPF2ND_EN         ((u32)0x00000001 << 3)          /*!<R/W 1  Channel x DMIC path SRC 2nd LPF control 0: Disable 1: Enable */
#define AUD_BIT_ADC_x_DMIC_LPF1ST_EN         ((u32)0x00000001 << 4)          /*!<R/W 1  Channel x DMIC path SRC 1st LPF control 0: Disable 1: Enable */
#define AUD_MASK_ADC_x_DMIC_LPF1ST_FC_SEL    ((u32)0x00000003 << 5)          /*!<R/W 0  Channel x DMIC path SRC 1st LPF FC */
#define AUD_ADC_x_DMIC_LPF1ST_FC_SEL(x)      (((u32)((x) & 0x00000003) << 5))
#define AUD_GET_ADC_x_DMIC_LPF1ST_FC_SEL(x)  ((u32)(((x >> 5) & 0x00000003)))
#define AUD_BIT_ADC_x_DMIC_MIX_MUTE          ((u32)0x00000001 << 7)          /*!<R/W 1  Channel x DMIC input path mute 0: Unmute 1: Mute */
#define AUD_MASK_ADC_x_AD_SRC_SEL            ((u32)0x00000007 << 8)          /*!<R/W 0  Channel x ANA ADC source selection 3'd0: ADC 1 3'd1: ADC 2 3'd2: ADC 3 Others: Reserved */
#define AUD_ADC_x_AD_SRC_SEL(x)              (((u32)((x) & 0x00000007) << 8))
#define AUD_GET_ADC_x_AD_SRC_SEL(x)          ((u32)(((x >> 8) & 0x00000007)))
#define AUD_BIT_ADC_x_AD_LPF2ND_EN           ((u32)0x00000001 << 11)          /*!<R/W 1  Channel x ADC path SRC 2nd LPF control 0: Disable 1: Enable */
#define AUD_MASK_ADC_x_AD_LPF2ND_FC_SEL      ((u32)0x00000003 << 12)          /*!<R/W 0  Channel x ADC path SRC 2nd LPF FC */
#define AUD_ADC_x_AD_LPF2ND_FC_SEL(x)        (((u32)((x) & 0x00000003) << 12))
#define AUD_GET_ADC_x_AD_LPF2ND_FC_SEL(x)    ((u32)(((x >> 12) & 0x00000003)))
#define AUD_BIT_ADC_x_AD_LPF1ST_EN           ((u32)0x00000001 << 14)          /*!<R/W 1  Channel x ADC path SRC 1st LPF control 0: Disable 1: Enable */
#define AUD_MASK_ADC_x_AD_LPF1ST_FC_SEL      ((u32)0x00000003 << 15)          /*!<R/W 0  Channel x ADC path SRC 1st LPF FC */
#define AUD_ADC_x_AD_LPF1ST_FC_SEL(x)        (((u32)((x) & 0x00000003) << 15))
#define AUD_GET_ADC_x_AD_LPF1ST_FC_SEL(x)    ((u32)(((x >> 15) & 0x00000003)))
#define AUD_BIT_ADC_x_AD_MIX_MUTE            ((u32)0x00000001 << 17)          /*!<R/W 1  Channel x ADC input path mute 0: Unmute 1: Mute */
#define AUD_MASK_ADC_x_AD_ZDET_FUNC          ((u32)0x00000003 << 18)          /*!<R/W 0x2  Channel x ADC path zero detection function selection 2'b00: immediate change 2'b01: zero detection & immediate change 2'b10: zero detection & step 2'b11: zero detection & timeout step */
#define AUD_ADC_x_AD_ZDET_FUNC(x)            (((u32)((x) & 0x00000003) << 18))
#define AUD_GET_ADC_x_AD_ZDET_FUNC(x)        ((u32)(((x >> 18) & 0x00000003)))
#define AUD_MASK_ADC_x_AD_ZDET_TOUT          ((u32)0x00000003 << 20)          /*!<R/W 0  Channel x ADC path zero detection time out selection 2'b00: 1024*16 samples 2'b01: 1024*32 samples 2'b10: 1024*64 samples 2'b11: 64 samples */
#define AUD_ADC_x_AD_ZDET_TOUT(x)            (((u32)((x) & 0x00000003) << 20))
#define AUD_GET_ADC_x_AD_ZDET_TOUT(x)        ((u32)(((x >> 20) & 0x00000003)))
#define AUD_BIT_ADC_x_AD_MUTE                ((u32)0x00000001 << 22)          /*!<R/W 0  Channel x ADC path mute 0: Unmute 1: Mute */
#define AUD_BIT_ADC_x_HPF_RSVD               ((u32)0x00000001 << 23)          /*!<R/W 0  Channel x ADC path reserved */
#define AUD_MASK_ADC_x_DECI_SRC_SEL          ((u32)0x00000003 << 24)          /*!<R/W 0  Channel x ADC path decimation source selection 0: ADC 1: Reserved 2: MUSIC 3: Reserved */
#define AUD_ADC_x_DECI_SRC_SEL(x)            (((u32)((x) & 0x00000003) << 24))
#define AUD_GET_ADC_x_DECI_SRC_SEL(x)        ((u32)(((x >> 24) & 0x00000003)))
#define AUD_BIT_ADC_x_DCHPF_EN               ((u32)0x00000001 << 26)          /*!<R/W 0  Channel x ADC path high pass filter enable control (filter DC) 0: Disable 1: Enable */
#define AUD_MASK_ADC_x_DCHPF_FC_SEL          ((u32)0x00000007 << 27)          /*!<R/W 0x5  Channel x ADC path high pass filter Fc */
#define AUD_ADC_x_DCHPF_FC_SEL(x)            (((u32)((x) & 0x00000007) << 27))
#define AUD_GET_ADC_x_DCHPF_FC_SEL(x)        ((u32)(((x >> 27) & 0x00000007)))
/** @} */

/** @defgroup CODEC_adc_x_control_1
 * @brief ADC Channel x Control Register 1
 * @{
 **/
#define AUD_MASK_ADC_x_AD_GAIN               ((u32)0x000000FF << 0)          /*!<R/W 0x2f  Channel x ADC digital volume -17.625dB ~ 48dB in 0.375dB step 8'h00: -17.625dB ... 8'h2f: 0dB 8'h30: 0.375dB ... 8'haf: 48dB */
#define AUD_ADC_x_AD_GAIN(x)                 (((u32)((x) & 0x000000FF) << 0))
#define AUD_GET_ADC_x_AD_GAIN(x)             ((u32)(((x >> 0) & 0x000000FF)))
#define AUD_MASK_ADC_x_BOOST_GAIN            ((u32)0x00000003 << 8)          /*!<R/W 0  Channel x ADC path boost gain control 00: 0dB 01: 12dB 10: 24dB 11: 36dB */
#define AUD_ADC_x_BOOST_GAIN(x)              (((u32)((x) & 0x00000003) << 8))
#define AUD_GET_ADC_x_BOOST_GAIN(x)          ((u32)(((x >> 8) & 0x00000003)))
#define AUD_MASK_ADC_x_RPTR_HOLD             ((u32)0x0000000F << 12)          /*!<R/W 0  Channel x I2S read point hold number rptr_hold = (I2S_fs/ad_fs) - 1 */
#define AUD_ADC_x_RPTR_HOLD(x)               (((u32)((x) & 0x0000000F) << 12))
#define AUD_GET_ADC_x_RPTR_HOLD(x)           ((u32)(((x >> 12) & 0x0000000F)))
#define AUD_BIT_ADC_x_FIFO_KEEP_ONE          ((u32)0x00000001 << 16)          /*!<R/W 0  Channel x i2s sample buffering 0: 4 samples 1: 1 sample */
/** @} */

/** @defgroup CODEC_dac_l_control_0
 * @brief DAC Left Channel Control Register 0
 * @{
 **/
#define AUD_MASK_DAC_L_DA_GAIN               ((u32)0x000000FF << 0)          /*!<R/W 0xaf  Channel L DAC path dvol gain control (0.375dB/step) 8'hAF: 0dB 8'h00: -65.625dB */
#define AUD_DAC_L_DA_GAIN(x)                 (((u32)((x) & 0x000000FF) << 0))
#define AUD_GET_DAC_L_DA_GAIN(x)             ((u32)(((x >> 0) & 0x000000FF)))
#define AUD_BIT_DAC_L_DA_SRC_SEL             ((u32)0x00000001 << 16)          /*!<R/W 0  Channel L DAC path input selection 0: I2S L channel 1: Test tone */
#define AUD_BIT_DAC_L_CONTROL_0_DUMMY        ((u32)0x00000001 << 17)          /*!<R/W 0  Dummy */
#define AUD_BIT_DAC_L_TEST_TONE_EN           ((u32)0x00000001 << 20)          /*!<R/W 0  Channel L DAC path test tone enable */
#define AUD_MASK_DAC_L_TEST_FC_SEL           ((u32)0x0000007F << 21)          /*!<R/W 0x0  Channel L DAC path test tone frequency: (fs/192)*(tone_fc_sel+1)Hz */
#define AUD_DAC_L_TEST_FC_SEL(x)             (((u32)((x) & 0x0000007F) << 21))
#define AUD_GET_DAC_L_TEST_FC_SEL(x)         ((u32)(((x >> 21) & 0x0000007F)))
#define AUD_MASK_DAC_L_TEST_GAIN_SEL         ((u32)0x0000000F << 28)          /*!<R/W 0x0  Channel L DAC path test tone gain control 0 -6.02*(gain_sel)dB */
#define AUD_DAC_L_TEST_GAIN_SEL(x)           (((u32)((x) & 0x0000000F) << 28))
#define AUD_GET_DAC_L_TEST_GAIN_SEL(x)       ((u32)(((x >> 28) & 0x0000000F)))
/** @} */

/** @defgroup CODEC_dac_l_control_1
 * @brief DAC Left Channel Control Register 1
 * @{
 **/
#define AUD_MASK_DAC_L_DA_ZDET_FUNC          ((u32)0x00000003 << 0)          /*!<R/W 0x2  Channel L DAC path zero detection function selection 2'b00: immediate change 2'b01: zero detection & immediate change 2'b10: zero detection & step 2'b11: zero detection & timeout step */
#define AUD_DAC_L_DA_ZDET_FUNC(x)            (((u32)((x) & 0x00000003) << 0))
#define AUD_GET_DAC_L_DA_ZDET_FUNC(x)        ((u32)(((x >> 0) & 0x00000003)))
#define AUD_MASK_DAC_L_DA_ZDET_TOUT          ((u32)0x00000003 << 2)          /*!<R/W 0  Channel L DAC path zero detection time out selection 2'b00: 1024*16 samples 2'b01: 1024*32 samples 2'b10: 1024*64 samples 2'b11: 64 samples */
#define AUD_DAC_L_DA_ZDET_TOUT(x)            (((u32)((x) & 0x00000003) << 2))
#define AUD_GET_DAC_L_DA_ZDET_TOUT(x)        ((u32)(((x >> 2) & 0x00000003)))
#define AUD_BIT_DAC_L_DA_MUTE                ((u32)0x00000001 << 4)          /*!<R/W 0  Channel L DAC path dvol mute enable 1'b0: unmute 1'b1: mute */
#define AUD_BIT_DAC_L_DAHPF_EN               ((u32)0x00000001 << 5)          /*!<R/W 0  Channel L DAC path Narrow-band 1st HPF enable control 0: Disable 1: Enable */
#define AUD_MASK_DAC_L_DA_DITHER_SEL         ((u32)0x00000003 << 6)          /*!<R/W 0  Channel L DAC path dither select 2'b00: Disable 2'b01: LSB 2'b10: LSB+1 2'b11: LSB+2 */
#define AUD_DAC_L_DA_DITHER_SEL(x)           (((u32)((x) & 0x00000003) << 6))
#define AUD_GET_DAC_L_DA_DITHER_SEL(x)       ((u32)(((x >> 6) & 0x00000003)))
#define AUD_BIT_DAC_L_DMIX_MUTE_DA           ((u32)0x00000001 << 8)          /*!<R/W 0  Channel L DAC path 128fs-domain mixer DA path mute enable 1'b0: unmute 1'b1: mute */
#define AUD_BIT_DAC_L_CONTROL_1_DUMMY0       ((u32)0x00000001 << 9)          /*!<R/W 0  Dummy */
#define AUD_BIT_DAC_L_DMIX_MUTE_DC           ((u32)0x00000001 << 10)          /*!<R/W 0  Channel L DAC path 128fs-domain mixer a DC from dac_l_tone_compensation_offset 1'b0: unmute 1'b1: mute */
#define AUD_BIT_DAC_L_MUSIC_MUTE_EN          ((u32)0x00000001 << 11)          /*!<R/W 0  Channel L DAC path music output mute enable 1'b0: unmute 1'b1: mute */
#define AUD_BIT_DAC_L_PDM_EN                 ((u32)0x00000001 << 13)          /*!<R/W 1  Channel L DAC path PDM 0: Disable 1: Enable */
#define AUD_BIT_DAC_L_SDM_EXTEND_FB_EN       ((u32)0x00000001 << 14)          /*!<R/W 1  Channel L DAC path feedback extend (for debug purpose, needing turn on by default) 0: Disable 1: Enable */
#define AUD_BIT_DAC_L_SDM_EF_EN              ((u32)0x00000001 << 15)          /*!<R/W 1  Channel L DAC path 2nd order SDM enable 0: Disable 1: Enable */
#define AUD_MASK_DAC_L_SDM_DITHER_SEL        ((u32)0x00000003 << 16)          /*!<R/W 0  Channel L DAC path SDM dither selection 2'b00: Disable 2'b01: LSB 2'b10: LSB+1 2'b11: LSB+2 */
#define AUD_DAC_L_SDM_DITHER_SEL(x)          (((u32)((x) & 0x00000003) << 16))
#define AUD_GET_DAC_L_SDM_DITHER_SEL(x)      ((u32)(((x >> 16) & 0x00000003)))
#define AUD_MASK_DAC_L_DA_FLT_TYPE           ((u32)0x00000003 << 18)          /*!<R/W 0  Channel L DAC path interpolation filter type */
#define AUD_DAC_L_DA_FLT_TYPE(x)             (((u32)((x) & 0x00000003) << 18))
#define AUD_GET_DAC_L_DA_FLT_TYPE(x)         ((u32)(((x >> 18) & 0x00000003)))
#define AUD_BIT_DAC_L_CONTROL_1_DUMMY1       ((u32)0x00000001 << 21)          /*!<R/W 0  Dummy */
/** @} */

/** @defgroup CODEC_dac_l_control_2
 * @brief DAC Left Channel Control Register 2
 * @{
 **/
#define AUD_MASK_DAC_L_DC_OFFSET             ((u32)0x0000FFFF << 0)          /*!<R/W 0  Channel L DAC path 128fs-domain mixer a DC compensation for spur issue 2's complement */
#define AUD_DAC_L_DC_OFFSET(x)               (((u32)((x) & 0x0000FFFF) << 0))
#define AUD_GET_DAC_L_DC_OFFSET(x)           ((u32)(((x >> 0) & 0x0000FFFF)))
#define AUD_MASK_DAC_L_CONTROL_2_DUMMY0      ((u32)0x0000000F << 16)          /*!<R/W 0  Dummy */
#define AUD_DAC_L_CONTROL_2_DUMMY0(x)        (((u32)((x) & 0x0000000F) << 16))
#define AUD_GET_DAC_L_CONTROL_2_DUMMY0(x)    ((u32)(((x >> 16) & 0x0000000F)))
#define AUD_MASK_DAC_L_CONTROL_2_DUMMY1      ((u32)0x0000000F << 20)          /*!<R/W 0x7  Dummy */
#define AUD_DAC_L_CONTROL_2_DUMMY1(x)        (((u32)((x) & 0x0000000F) << 20))
#define AUD_GET_DAC_L_CONTROL_2_DUMMY1(x)    ((u32)(((x >> 20) & 0x0000000F)))
/** @} */

/** @defgroup CODEC_adc_align_control_0
 * @brief ADC Align Control Register 0
 * @{
 **/
#define AUD_BIT_SP_AD_FIFO_ALIGN_EN          ((u32)0x00000001 << 0)          /*!<R/W 0  1: Uplink FIFO will align with I2S 0: Disable */
#define AUD_BIT_SP_AD_ALIGN_EN               ((u32)0x00000001 << 2)          /*!<R/W 0  1: Uplink channel will align with I2S 0: Disable */
/** @} */

/** @defgroup CODEC_adc_align_control
 * @brief ADC Align Control Register
 * @{
 **/
#define AUD_MASK_ADC_0_ALIGN_CH_SEL          ((u32)0x00000007 << 0)          /*!<R/W 0  Uplink CH0 align channel selection 0: CH0 1: CH1 2: CH2 3: CH3 Others: Reserved */
#define AUD_ADC_0_ALIGN_CH_SEL(x)            (((u32)((x) & 0x00000007) << 0))
#define AUD_GET_ADC_0_ALIGN_CH_SEL(x)        ((u32)(((x >> 0) & 0x00000007)))
#define AUD_BIT_ADC_0_ALIGN_EN               ((u32)0x00000001 << 3)          /*!<R/W 0  Uplink CH0 align enable */
#define AUD_MASK_ADC_1_ALIGN_CH_SEL          ((u32)0x00000007 << 4)          /*!<R/W 1  Uplink CH1 align channel selection 0: CH0 1: CH1 2: CH2 3: CH3 Others: Reserved */
#define AUD_ADC_1_ALIGN_CH_SEL(x)            (((u32)((x) & 0x00000007) << 4))
#define AUD_GET_ADC_1_ALIGN_CH_SEL(x)        ((u32)(((x >> 4) & 0x00000007)))
#define AUD_BIT_ADC_1_ALIGN_EN               ((u32)0x00000001 << 7)          /*!<R/W 0  Uplink CH1 align enable */
#define AUD_MASK_ADC_2_ALIGN_CH_SEL          ((u32)0x00000007 << 8)          /*!<R/W 0x2  Uplink CH2 align channel selection 0: CH0 1: CH1 2: CH2 3: CH3 Others: Reserved */
#define AUD_ADC_2_ALIGN_CH_SEL(x)            (((u32)((x) & 0x00000007) << 8))
#define AUD_GET_ADC_2_ALIGN_CH_SEL(x)        ((u32)(((x >> 8) & 0x00000007)))
#define AUD_BIT_ADC_2_ALIGN_EN               ((u32)0x00000001 << 11)          /*!<R/W 0  Uplink CH2 align enable */
#define AUD_MASK_ADC_3_ALIGN_CH_SEL          ((u32)0x00000007 << 12)          /*!<R/W 0x2  Uplink CH3 align channel selection 0: CH0 1: CH1 2: CH2 3: CH3 Others: Reserved */
#define AUD_ADC_3_ALIGN_CH_SEL(x)            (((u32)((x) & 0x00000007) << 12))
#define AUD_GET_ADC_3_ALIGN_CH_SEL(x)        ((u32)(((x >> 12) & 0x00000007)))
#define AUD_BIT_ADC_3_ALIGN_EN               ((u32)0x00000001 << 15)          /*!<R/W 0  Uplink CH3 align enable */
/** @} */

/** @defgroup CODEC_dac_mix_control
 * @brief DAC Mix Control Register
 * @{
 **/
#define AUD_MASK_DAC_MIX_CONTROL_DUMMY       ((u32)0x0000000F << 0)          /*!<R/W 0  Dummy */
#define AUD_DAC_MIX_CONTROL_DUMMY(x)         (((u32)((x) & 0x0000000F) << 0))
#define AUD_GET_DAC_MIX_CONTROL_DUMMY(x)     ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup CODEC_phase_compensation_control_register_x
 * @brief Channel x Phase Compensation Control Register
 * @{
 **/
#define AUD_BIT_PHASE_DELAY_COUNTER_EN_x     ((u32)0x00000001 << 0)          /*!<R/W 0  Enable Uplink CHx phase compensation */
#define AUD_MASK_PHASE_DELAY_COUNTER_x       ((u32)0x000003FF << 1)          /*!<R/W 0  Uplink CH x phase compensation delay */
#define AUD_PHASE_DELAY_COUNTER_x(x)         (((u32)((x) & 0x000003FF) << 1))
#define AUD_GET_PHASE_DELAY_COUNTER_x(x)     ((u32)(((x >> 1) & 0x000003FF)))
/** @} */

/** @defgroup CODEC_adc_x_silence_control
 * @brief ADC Channel x Silence Control Register
 * @{
 **/
#define AUD_BIT_ADC_x_SILENCE_DET_EN         ((u32)0x00000001 << 0)          /*!<R/W 0  Channel x ADC path silence detection enable 0: Disable 1: Enable */
#define AUD_MASK_ADC_x_SILENCE_LEVEL_SEL     ((u32)0x00000007 << 1)          /*!<R/W 1  Channel x ADC path silence detection threshold 3'b000: -54db 3'b001: -60db 3'b010: -66db 3'b011: -72db 3'b100: -78db 3'b101: -84db 3'b110: -90db 3'b111: -96db */
#define AUD_ADC_x_SILENCE_LEVEL_SEL(x)       (((u32)((x) & 0x00000007) << 1))
#define AUD_GET_ADC_x_SILENCE_LEVEL_SEL(x)   ((u32)(((x >> 1) & 0x00000007)))
#define AUD_MASK_ADC_x_SILENCE_DEBOUNCE_SEL  ((u32)0x00000007 << 4)          /*!<R/W 0x3  Channel x ADC path silence detection debounce (48K) 3'b000: 80ms 3'b001: 160ms 3'b010: 320ms 3'b011: 640ms 3'b100: 1.28s 3'b101: 2.56s 3'b110: 5.12s 3'b111: 0.16ms */
#define AUD_ADC_x_SILENCE_DEBOUNCE_SEL(x)    (((u32)((x) & 0x00000007) << 4))
#define AUD_GET_ADC_x_SILENCE_DEBOUNCE_SEL(x) ((u32)(((x >> 4) & 0x00000007)))
/** @} */

/** @defgroup CODEC_dac_l_silence_control
 * @brief DAC Left Channel Silence Control Register
 * @{
 **/
#define AUD_BIT_DAC_L_SILENCE_DET_EN         ((u32)0x00000001 << 0)          /*!<R/W 0  Channel L DAC path silence detection enable 0: Disable 1: Enable */
#define AUD_MASK_DAC_L_SILENCE_LEVEL_SEL     ((u32)0x00000007 << 1)          /*!<R/W 1  Channel L DAC path silence detection threshold 3'b000: -78db 3'b001: -84db 3'b010: -90db 3'b011: -102db 3'b100: -108db 3'b101: -114db 3'b110: -126db 3'b111: -132db */
#define AUD_DAC_L_SILENCE_LEVEL_SEL(x)       (((u32)((x) & 0x00000007) << 1))
#define AUD_GET_DAC_L_SILENCE_LEVEL_SEL(x)   ((u32)(((x >> 1) & 0x00000007)))
#define AUD_MASK_DAC_L_SILENCE_DEBOUNCE_SEL  ((u32)0x00000007 << 4)          /*!<R/W 0x3  Channel L DAC path silence detection debounce (48K) 3'b000: 80ms 3'b001: 160ms 3'b010: 320ms 3'b011: 640ms 3'b100: 1.28s 3'b101: 2.56s 3'b110: 5.12s 3'b111: 0.16ms */
#define AUD_DAC_L_SILENCE_DEBOUNCE_SEL(x)    (((u32)((x) & 0x00000007) << 4))
#define AUD_GET_DAC_L_SILENCE_DEBOUNCE_SEL(x) ((u32)(((x >> 4) & 0x00000007)))
/** @} */

/** @defgroup CODEC_vad_ctrl
 * @brief VAD Control Register
 * @{
 **/
#define AUD_BIT_VAD_RESET                    ((u32)0x00000001 << 0)          /*!<R/W 1  Reset VAD module */
#define AUD_MASK_VAD_EQ_SRC_SEL              ((u32)0x00000007 << 1)          /*!<R/W 0  VAD source selection 2'b00: adc_0_eq_out 2'b01: adc_1_eq_out 2'b10: adc_2_eq_out 2'b11: adc_3_eq_out */
#define AUD_VAD_EQ_SRC_SEL(x)                (((u32)((x) & 0x00000007) << 1))
#define AUD_GET_VAD_EQ_SRC_SEL(x)            ((u32)(((x >> 1) & 0x00000007)))
/** @} */

/** @defgroup CODEC_vad_voice_level
 * @brief VAD Voice Level Register
 * @{
 **/
#define AUD_MASK_VAD_VOICE_LEVEL             ((u32)0x000001FF << 0)          /*!<R/W 0x2c  The minimum value of mean power of PE&D unit0 */
#define AUD_VAD_VOICE_LEVEL(x)               (((u32)((x) & 0x000001FF) << 0))
#define AUD_GET_VAD_VOICE_LEVEL(x)           ((u32)(((x >> 0) & 0x000001FF)))
/** @} */

/** @defgroup CODEC_vad_det_thr
 * @brief VAD Detection Threshold Register
 * @{
 **/
#define AUD_MASK_VAD_DET_THR                 ((u32)0x000000FF << 0)          /*!<R/W 0x19  Setting vad_flag threshold between frame power and mean power of PE&D unit0 */
#define AUD_VAD_DET_THR(x)                   (((u32)((x) & 0x000000FF) << 0))
#define AUD_GET_VAD_DET_THR(x)               ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup CODEC_vad_intr_en
 * @brief VAD Interrupt Enable Register
 * @{
 **/
#define AUD_MASK_VAD_INTR_EN                 ((u32)0x000000FF << 0)          /*!<R/W 0  New enables of vad interrupts */
#define AUD_VAD_INTR_EN(x)                   (((u32)((x) & 0x000000FF) << 0))
#define AUD_GET_VAD_INTR_EN(x)               ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup CODEC_dac_dre_control_0
 * @brief DAC DRE Control Register
 * @{
 **/
#define AUD_BIT_DAC_DRE_RSVD                 ((u32)0x00000001 << 0)          /*!<R/W 0  reserved */
/** @} */

/** @defgroup CODEC_ana_read
 * @brief ANA Read Register
 * @{
 **/
#define AUD_BIT_MICBIAS_OC                   ((u32)0x00000001 << 0)          /*!<R   The status flag of MICBIAS over-current protection */
/** @} */

/** @defgroup CODEC_adc_x_lpf_rd
 * @brief ADC Channel x LPF Read Register
 * @{
 **/
#define AUD_MASK_ADC_x_LPF_RD                ((u32)0x0007FFFF << 0)          /*!<R   CH x ADC LPF out values */
#define AUD_ADC_x_LPF_RD(x)                  (((u32)((x) & 0x0007FFFF) << 0))
#define AUD_GET_ADC_x_LPF_RD(x)              ((u32)(((x >> 0) & 0x0007FFFF)))
/** @} */

/** @defgroup CODEC_silence_inform
 * @brief Silence Inform Register
 * @{
 **/
#define AUD_BIT_DAC_L_SILENCE_DET_O          ((u32)0x00000001 << 0)          /*!<R   dac_l_silence data status (result of silence detection) 1'b0: not dac_l_silence data 1'b1: dac_l_silence is detected */
#define AUD_BIT_DAC_L_SILENCE_DET_STATUS     ((u32)0x00000001 << 1)          /*!<R   Ongoing status of dac_l_silence detection 1'b0: dac_l_silence detection is resting (clock is gating) 1'b1: dac_l_silence detection is working */
#define AUD_BIT_ADC_0_SILENCE_DET_O          ((u32)0x00000001 << 4)          /*!<R   adc_0_silencedata status (result of silence detection) 1'b0: not adc_0_silencedata 1'b1: adc_0_silenceis detected */
#define AUD_BIT_ADC_0_SILENCE_DET_STATUS     ((u32)0x00000001 << 5)          /*!<R   Ongoing status of adc_0_silencedetection 1'b0: adc_0_silencedetection is resting (clock is gating) 1'b1: adc_0_silencedetection is working */
#define AUD_BIT_ADC_1_SILENCE_DET_O          ((u32)0x00000001 << 6)          /*!<R   adc_1_silencedata status (result of silence detection) 1'b0: not adc_1_silencedata 1'b1: adc_1_silenceis detected */
#define AUD_BIT_ADC_1_SILENCE_DET_STATUS     ((u32)0x00000001 << 7)          /*!<R   Ongoing status of adc_1_silencedetection 1'b0: adc_1_silencedetection is resting (clock is gating) 1'b1: adc_1_silencedetection is working */
#define AUD_BIT_ADC_2_SILENCE_DET_O          ((u32)0x00000001 << 8)          /*!<R   adc_2_silencedata status (result of silence detection) 1'b0: not adc_2_silencedata 1'b1: adc_2_silenceis detected */
#define AUD_BIT_ADC_2_SILENCE_DET_STATUS     ((u32)0x00000001 << 9)          /*!<R   Ongoing status of adc_2_silencedetection 1'b0: adc_2_silencedetection is resting (clock is gating) 1'b1: adc_2_silencedetection is working */
#define AUD_BIT_ADC_3_SILENCE_DET_O          ((u32)0x00000001 << 10)          /*!<R   adc_3_silencedata status (result of silence detection) 1'b0: not adc_3_silencedata 1'b1: adc_3_silenceis detected */
#define AUD_BIT_ADC_3_SILENCE_DET_STATUS     ((u32)0x00000001 << 11)          /*!<R   Ongoing status of adc_3_silencedetection 1'b0: adc_3_silencedetection is resting (clock is gating) 1'b1: adc_3_silencedetection is working */
/** @} */

/** @defgroup CODEC_audio_ro_dummy1
 * @brief Audio RO Dummy1 Register
 * @{
 **/
#define AUD_MASK_AUDIO_RO_DUMMY1             ((u32)0xFFFFFFFF << 0)          /*!<R   Dummy */
#define AUD_AUDIO_RO_DUMMY1(x)               (((u32)((x) & 0xFFFFFFFF) << 0))
#define AUD_GET_AUDIO_RO_DUMMY1(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup CODEC_vad_intr_st
 * @brief VAD Interrupt Status Register
 * @{
 **/
#define AUD_MASK_VAD_INTR_ST                 ((u32)0x000000FF << 0)          /*!<RW1C   Status of new VAD interrupts and clear them by writing 1 */
#define AUD_VAD_INTR_ST(x)                   (((u32)((x) & 0x000000FF) << 0))
#define AUD_GET_VAD_INTR_ST(x)               ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup CODEC_vad_det_flag
 * @brief VAD Detection Flag Register
 * @{
 **/
#define AUD_BIT_VAD_DET_FLAG                 ((u32)0x00000001 << 0)          /*!<R   VAD detection flag 1 */
/** @} */
/** @} */

/* Exported types --------------------------------------------------------*/

/** @defgroup AUDIO_CODEC_Exported_Types AUDIO_CODEC Exported Types
  * @{
*****************************************************************************/

/** @brief AUDIO_CODEC port
*****************************************************************************/
typedef struct {

	__IO uint32_t CODEC_ADC_x_CONTROL_0;                  /*!< ADC CHANNEL x CONTROL REGISTER 0 Register */
	__IO uint32_t CODEC_ADC_x_CONTROL_1;                  /*!< ADC CHANNEL x CONTROL REGISTER 1 Register */
} CODEC_ADC_CH_CTRL_TypeDef;





/**
 * @brief AUDIO_CODEC Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t CODEC_AUDIO_CONTROL_0;                  /*!< AUDIO CONTROL REGISTER 0 Register,  Address offset: 0x00 */
	__IO uint32_t CODEC_AUDIO_CONTROL_1;                  /*!< AUDIO CONTROL REGISTER 1 Register,  Address offset: 0x04 */
	__IO uint32_t CODEC_CLOCK_CONTROL_1;                  /*!< CLOCK CONTROL REGISTER 1 Register,  Address offset: 0x08 */
	__IO uint32_t CODEC_CLOCK_CONTROL_2;                  /*!< CLOCK CONTROL REGISTER 2 Register,  Address offset: 0x0C */
	__IO uint32_t CODEC_CLOCK_CONTROL_3;                  /*!< CLOCK CONTROL REGISTER 3 Register,  Address offset: 0x10 */
	__IO uint32_t CODEC_CLOCK_CONTROL_4;                  /*!< CLOCK CONTROL REGISTER 4 Register,  Address offset: 0x14 */
	__IO uint32_t CODEC_CLOCK_CONTROL_5;                  /*!< CLOCK CONTROL REGISTER 5 Register,  Address offset: 0x18 */
	__IO uint32_t CODEC_CLOCK_CONTROL_6;                  /*!< CLOCK CONTROL REGISTER 6 Register,  Address offset: 0x1C */
	__IO uint32_t CODEC_CLOCK_CONTROL_7;                  /*!< CLOCK CONTROL REGISTER 7 Register,  Address offset: 0x20 */
	__IO uint32_t CODEC_ASRC_CONTROL_0;                   /*!< ASRC CONTROL REGISTER 0 Register,  Address offset: 0x24 */
	__IO uint32_t CODEC_ASRC_CONTROL_1;                   /*!< ASRC CONTROL REGISTER 1 Register,  Address offset: 0x28 */
	__IO uint32_t CODEC_ASRC_CONTROL_2;                   /*!< ASRC CONTROL REGISTER 2 Register,  Address offset: 0x2C */
	__IO uint32_t CODEC_ASRC_CONTROL_3;                   /*!< ASRC CONTROL REGISTER 3 Register,  Address offset: 0x30 */
	__IO uint32_t RSVD0[2];                               /*!<  Reserved,  Address offset:0x34-0x38 */
	__IO uint32_t CODEC_I2S_0_CONTROL;                    /*!< I2S 0 CONTROL REGISTER,  Address offset: 0x3C */
	__IO uint32_t CODEC_I2S_0_CONTROL_1;                  /*!< I2S 0 CONTROL REGISTER 1 Register,  Address offset: 0x40 */
	__IO uint32_t RSVD1[3];                               /*!<  Reserved,  Address offset:0x44-0x4C */
	CODEC_ADC_CH_CTRL_TypeDef CODEC_ADC_CH_CTRL[4];       /*!< AUDIO IP have 4 ports,  Address offset: 0x50-0x6C */
	__IO uint32_t RSVD2[8];                               /*!<  Reserved,  Address offset:0x70-0x8C */
	__IO uint32_t CODEC_DAC_L_CONTROL_0;                  /*!< DAC LEFT CHANNEL CONTROL REGISTER 0 Register,  Address offset: 0x90 */
	__IO uint32_t CODEC_DAC_L_CONTROL_1;                  /*!< DAC LEFT CHANNEL CONTROL REGISTER 1 Register,  Address offset: 0x94 */
	__IO uint32_t CODEC_DAC_L_CONTROL_2;                  /*!< DAC LEFT CHANNEL CONTROL REGISTER 2 Register,  Address offset: 0x98 */
	__IO uint32_t RSVD3[4];                               /*!<  Reserved,  Address offset:0x9C-0xA8 */
	__IO uint32_t CODEC_ADC_ALIGN_CONTROL_0;              /*!< ADC ALIGN CONTROL REGISTER 0 Register,  Address offset: 0xAC */
	__IO uint32_t CODEC_ADC_ALIGN_CONTROL;                /*!< ADC ALIGN CONTROL REGISTER,  Address offset: 0xB0 */
	__IO uint32_t RSVD4[2];                               /*!<  Reserved,  Address offset:0xB4-0xB8 */
	__IO uint32_t CODEC_DAC_MIX_CONTROL;                  /*!< DAC MIX CONTROL REGISTER,  Address offset: 0xBC */
	__IO uint32_t CODEC_PHASE_COMPENSATION_CONTROL_REGISTER_x[3];/*!< CHANNEL x PHASE COMPENSATION CONTROL REGISTER,  Address offset: 0xC0-0xC8 */
	__IO uint32_t RSVD5[13];                              /*!<  Reserved,  Address offset:0xCC-0xFC */
	__IO uint32_t CODEC_ADC_x_SILENCE_CONTROL[4];         /*!< ADC CHANNEL x SILENCE CONTROL REGISTER,  Address offset: 0x100-0x10C */
	__IO uint32_t RSVD6[4];                               /*!<  Reserved,  Address offset:0x110-0x11C */
	__IO uint32_t CODEC_DAC_L_SILENCE_CONTROL;            /*!< DAC LEFT CHANNEL SILENCE CONTROL REGISTER,  Address offset: 0x120 */
	__IO uint32_t RSVD7[375];                              /*!<  Reserved,  Address offset:0x124-0x6FC */
	__IO uint32_t CODEC_VAD_CTRL;                         /*!< VAD CONTROL REGISTER,  Address offset: 0x700 */
	__IO uint32_t CODEC_VAD_VOICE_LEVEL;                  /*!< VAD VOICE LEVEL REGISTER,  Address offset: 0x704 */
	__IO uint32_t CODEC_VAD_DET_THR;                      /*!< VAD DETECTION THRESHOLD REGISTER,  Address offset: 0x708 */
	__IO uint32_t CODEC_VAD_INTR_EN;                      /*!< VAD INTERRUPT ENABLE REGISTER,  Address offset: 0x70C */
	__IO uint32_t CODEC_DAC_DRE_CONTROL_0;                /*!< DAC DRE CONTROL REGISTER,  Address offset: 0x710 */
	__IO uint32_t RSVD8[59];                              /*!<  Reserved,  Address offset:0x714-0x7FC */
	__I  uint32_t CODEC_ANA_READ;                         /*!< ANA READ REGISTER,  Address offset: 0x800 */
	__I  uint32_t CODEC_ADC_x_LPF_RD[4];                  /*!< ADC CHANNEL x LPF READ REGISTER,  Address offset: 0x804-0x810 */
	__IO uint32_t RSVD9[4];                               /*!<  Reserved,  Address offset:0x814-0x820 */
	__I  uint32_t CODEC_SILENCE_INFORM;                   /*!< SILENCE INFORM REGISTER,  Address offset: 0x824 */
	__I  uint32_t CODEC_AUDIO_RO_DUMMY1;                  /*!< AUDIO RO DUMMY1 REGISTER,  Address offset: 0x828 */
	__IO uint32_t RSVD10[53];                             /*!<  Reserved,  Address offset:0x82C-0x8FC */
	__IO uint32_t CODEC_VAD_INTR_ST;                      /*!< VAD INTERRUPT STATUS REGISTER,  Address offset: 0x900 */
	__I  uint32_t CODEC_VAD_DET_FLAG;                     /*!< VAD DETECTION FLAG REGISTER,  Address offset: 0x904 */
} AUDIO_TypeDef;
/** @} */
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

/* Exported types ------------------------------------------------------------*/
/** @defgroup AUDIO_CODEC_Exported_Types AUDIO_CODEC Exported Types
  * @{
  */

/**
 * @brief AUDIO_CODEC Init Structure Definition
 */
typedef struct {

	u32 CODEC_SelI2STxSR;				/*!< Specifies the AUDIO CODEC TX Sample rate
										This parameter can be a value of @ref AUDIO_CODEC_Sample_Rate */

	u32 CODEC_SelI2STxWordLen; 			/*!< Specifies the AUDIO CODEC TX Word length
										This parameter can be a value of @ref AUDIO_CODEC_Word_Length */

	u32 CODEC_SelI2STxCHLen;			/*!< Specifies the AUDIO SPORT TX Channel length
										This parameter can be a value of @ref AUDIO_CODEC_Channel_Length  */

	u32 CODEC_SelI2STxCH;				/*!< Specifies the AUDIO CODEC TX Channel L/R
										This parameter can be a value of @ref AUDIO_CODEC_Tx_Channel  */

	u32 CODEC_SelI2STxDataFormat;		/*!< Specifies the AUDIO CODEC I2S TX selection of data fornat
										This parameter can be a value of @ref AUDIO_CODEC_Interface_Format */

	u32 CODEC_SelI2SRxSR;				/*!< Specifies the AUDIO CODEC RX Sample rate
										This parameter can be a value of @ref AUDIO_CODEC_Sample_Rate */

	u32 CODEC_SelI2SRxWordLen; 			/*!< Specifies the AUDIO CODEC RX Word length
										This parameter can be a value of @ref AUDIO_CODEC_Word_Length */

	u32 CODEC_SelI2SRxCHLen;			/*!< Specifies the AUDIO SPORT RX Channel length
										This parameter can be a value of @ref AUDIO_CODEC_Channel_Length  */

	u32 CODEC_SelI2SRxDataFormat;		/*!< Specifies the AUDIO CODEC I2S RX selection of data fornat
										This parameter can be a value of @ref AUDIO_CODEC_Interface_Format */

	u32 CODEC_SelRxI2STdm;				/*!< Specifies the AUDIO CODEC Rx I2S TDM MODE
										This parameter can be a value of @ref AUDIO_CODEC_I2S_TDM_Mode */

} I2S_InitTypeDef;
/**
  * @}
  */

/* Exported constants ------------------------------------------------------------*/
/** @defgroup AUDIO_CODEC_Exported_Constants AUDIO_CODEC Exported Constants
  * @{
  */

/** @defgroup AUDIO_CODEC_Sample_Rate
  * @{
  */
#define SR_48K			((u32)0x00000000)
#define SR_96K			((u32)0x00000001)
#define SR_192K			((u32)0x00000002)
#define SR_32K			((u32)0x00000003)
#define SR_176P4K		((u32)0x00000004)
#define SR_16K			((u32)0x00000005)
#define SR_8K			((u32)0x00000007)
#define SR_44P1K		((u32)0x00000008)
#define SR_88P2K		((u32)0x00000009)
#define SR_24K			((u32)0x0000000a)
#define SR_12K			((u32)0x0000000b)
#define SR_22P05K		((u32)0x0000000c)
#define SR_11P025K		((u32)0x0000000d)

#define IS_CODEC_SR_SEL(SEL) (((SEL) == SR_48K) || \
							((SEL) == SR_96K) || \
							((SEL) == SR_192K) || \
							((SEL) == SR_32K) || \
							((SEL) == SR_176P4K) || \
							((SEL) == SR_16K) || \
							((SEL) == SR_8K) || \
							((SEL) == SR_44P1K) || \
							((SEL) == SR_88P2K) || \
							((SEL) == SR_24K) || \
							((SEL) == SR_12K) || \
							((SEL) == SR_22P05K) || \
							((SEL) == SR_11P025K))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Word_Length
  * @{
  */
#define WL_16			((u32)0x00000000)
#define WL_24			((u32)0x00000002)
#define WL_8			((u32)0x00000003)

#define IS_CODEC_WL_SEL(SEL) (((SEL) == WL_16) || \
							((SEL) == WL_24) || \
							((SEL) == WL_8))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Channel_Length
  * @{
  */
#define CL_16			((u32)0x00000000)
#define CL_32			((u32)0x00000001)
#define CL_24			((u32)0x00000002)
#define CL_8			((u32)0x00000003)

#define IS_CODEC_CL_SEL(SEL) (((SEL) == CL_16) || \
							((SEL) == CL_32) || \
							((SEL) == CL_24) || \
							((SEL) == CL_8))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Interface_Format
  * @{
  */
#define DF_I2S						((u32)0x00000000)
#define DF_LEFT						((u32)0x00000001)
#define DF_PCM_A						((u32)0x00000002)
#define DF_PCM_B						((u32)0x00000003)

#define IS_CODEC_DATA_FMT(FORMAT) (((FORMAT) == DF_I2S) || \
											((FORMAT) == DF_LEFT) || \
											((FORMAT) == DF_PCM_A) || \
											((FORMAT) == DF_PCM_B))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Tx_Channel
  * @{
  */
#define CH_LR						((u32)0x00000000)
#define CH_RL						((u32)0x00000001)
#define CH_LL						((u32)0x00000002)
#define CH_RR						((u32)0x00000003)

#define IS_CODEC_TXCHN_SEL(SEL) (((SEL) == CH_LR) || \
							((SEL) == CH_RL) || \
							((SEL) == CH_LL) || \
							((SEL) == CH_RR))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_I2S_TDM_Mode
  * @{
  */
#define I2S_NOTDM						    ((u32)0x00000000)
#define I2S_TDM4							((u32)0x00000001)

#define IS_CODEC_I2S_TDM_SEL(SEL) (((SEL) == I2S_NOTDM) || \
							((SEL) == I2S_TDM4))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_AMIC
  * @{
  */
#define AMIC1							((u32)0x00000001)
#define AMIC2							((u32)0x00000002)
#define AMIC3							((u32)0x00000003)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_ADC
  * @{
  */
#define ADC1							((u32)0x00000001)
#define ADC2							((u32)0x00000002)
#define ADC3							((u32)0x00000003)
#define ADC4							((u32)0x00000004)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_ADC_Channel
  * @{
  */
#define ADCHN1							((u32)0x00000001)
#define ADCHN2							((u32)0x00000002)
#define ADCHN3							((u32)0x00000003)
#define ADCHN4							((u32)0x00000004)
#define IS_CODEC_ADCHN_SEL(SEL) (((SEL) == ADCHN1) || \
								((SEL) == ADCHN2) || \
								((SEL) == ADCHN3) || \
								((SEL) == ADCHN4))
/**
* @}
*/

/** @defgroup AUDIO_CODEC_LineIn_Channel
  * @{
  */
#define LINEIN1							((u32)0x00000001)
#define LINEIN2							((u32)0x00000002)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Power
  * @{
  */
#define NORMALPOWER						((u32)0x00000001)
#define SHUTDOWN						((u32)0x00000000)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_DMIC
  * @{
  */
#define DMIC1							((u32)0x00000001)
#define DMIC2							((u32)0x00000002)
#define DMIC3							((u32)0x00000003)
#define DMIC4							((u32)0x00000004)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_DMIC_Clock
  * @{
  */
#define DMIC_5M			((u32)0x00000000)
#define DMIC_2P5M		((u32)0x00000001)
#define DMIC_1P25M		((u32)0x00000002)
#define DMIC_625K		((u32)0x00000003)
#define DMIC_312P5K		((u32)0x00000004)
#define DMIC_769P2K		((u32)0x00000007)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_PDM_Clock
  * @{
  */
#define PDM_5M			((u32)0x00000000)
#define PDM_2P5M		((u32)0x00000001)
#define PDM_6P67M		((u32)0x00000002)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Micbst_Gain
  * @{
  */
#define MICBST_GAIN_0DB			((u32)0x00000000)
#define MICBST_GAIN_5DB			((u32)0x00000001)
#define MICBST_GAIN_10DB		((u32)0x00000002)
#define MICBST_GAIN_15DB		((u32)0x00000003)
#define MICBST_GAIN_20DB		((u32)0x00000004)
#define MICBST_GAIN_25DB		((u32)0x00000005)
#define MICBST_GAIN_30DB		((u32)0x00000006)
#define MICBST_GAIN_35DB		((u32)0x00000007)
#define MICBST_GAIN_40DB		((u32)0x00000008)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Power_Mode
  * @{
  */
#define POWER_ON			((u32)0x00000001)
#define POWER_DOWN			((u32)0x00000000)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Mute
  * @{
  */
#define UNMUTE			((u32)0x00000000)
#define MUTE			((u32)0x00000001)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_I2S_Master_Source
  * @{
  */
#define INTERNAL_SPORT			((u32)0x00000000)
#define EXTERNAL_I2S			((u32)0x00000001)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_LintOut_Mode
  * @{
  */
#define SINGLE			((u32)0x00000000)
#define DIFF			((u32)0x00000001)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Sample_Rate_Source
  * @{
  */
#define SOURCE0			((u32)0x00000000)
#define SOURCE1			((u32)0x00000001)
#define SOURCE2			((u32)0x00000002)
#define SOURCE3			((u32)0x00000003)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_DAC_Source
  * @{
  */
#define I2SL					((u32)0x00000000)
#define TESTTONE			((u32)0x00000001)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_ADC_MUTE_Control
  * @{
  */
#define LINEIN				((u32)0x00000001)
#define MICIN				((u32)0x00000002)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_DAC_MUTE_Control
  * @{
  */
#define ANALOGIN			((u32)0x00000001)
#define DACIN				((u32)0x00000002)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_PATH
  * @{
  */
#define DACPATH			((u32)0x00000001)
#define ADCPATH			((u32)0x00000002)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_ADC_Mix_Select
  * @{
  */
#define DMIC			((u32)0x00000001)
#define ANAAD				((u32)0x00000002)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_DAC_Channel
  * @{
  */
#define DAC_L			((u32)0x00000000)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_ADC_Decimation_Source
  * @{
  */
#define AUDIO_ADC			((u32)0x00000000)
#define ANC				((u32)0x00000001)
#define MUSIC			((u32)0x00000002)
#define ANCANDMUSIC				((u32)0x00000003)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Application
  * @{
  */

#define APP_AMIC_RECORD ((u32)0x00000000)
#define APP_DMIC_RECORD ((u32)0x00000001)
#define APP_LINE_OUT	((u32)0x00000002)
#define APP_PDM_OUT		((u32)0x00000003)
#define APP_TESTTONE	((u32)0x00000004)
#define APP_DAAD_LPBK	((u32)0x00000005)
#define APP_ADDA_LPBK	((u32)0x00000006)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_PAD
  * @{
  */
#define PAD_DACL ((u32)0x00000000)
#define PAD_MIC1 ((u32)0x00000001)
#define PAD_MIC2 ((u32)0x00000002)
#define PAD_MIC3 ((u32)0x00000003)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Zero_Detection_Function_Select
  * @{
  */
#define IME				 ((u32)0x00000000)
#define ZDET_IME		 ((u32)0x00000001)
#define ZDET_STEP		 ((u32)0x00000002)
#define ZDET_TIMEOUT	 ((u32)0x00000003)
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions ------------------------------------------------------------*/
/** @defgroup AUDIO_CODEC_Exported_Functions AUDIO_CODEC Exported Functions
  * @{
  */
_LONG_CALL_ void AUDIO_CODEC_SetAudioIP(u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetI2SIP(u32 i2s_sel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetI2SSRC(u32 i2s_sel, u32 src);
_LONG_CALL_ void AUDIO_CODEC_SetANAClk(u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetI2SRXTDM(u32 i2s_sel, u32 tdmmode);
_LONG_CALL_ void AUDIO_CODEC_I2S_StructInit(I2S_InitTypeDef *I2S_Struct);
_LONG_CALL_ void AUDIO_CODEC_SetI2SParameters(u32 i2s_sel, u32 path, I2S_InitTypeDef *I2S_Struct);
_LONG_CALL_ void AUDIO_CODEC_SetADCSRSrc(u32 src, u32 sr);
_LONG_CALL_ void AUDIO_CODEC_SetDACSRSrc(u32 src, u32 sr);
_LONG_CALL_ void AUDIO_CODEC_DisPAD(u32 path);
_LONG_CALL_ void AUDIO_CODEC_SetLDOMode(u32 powermode);
_LONG_CALL_ void AUDIO_CODEC_EnableADC(u32 ad_chn, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCVolume(u32 adc_sel, u32 gain);
_LONG_CALL_ void AUDIO_CODEC_SetADCHPF(u32 adc_sel, u32 fc, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCASRC(u32 i2s_sel, u32 adc_sel, u32 sr, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCMute(u32 adc_sel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCMixMute(u32 adc_num, u32 type,				  u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCANASrc(u32 ad_chn, u32 amic_num);
_LONG_CALL_ void AUDIO_CODEC_SetADCANAFilter(u32 adc_num, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetMicBiasPowerMode(u32 powermode);
_LONG_CALL_ void AUDIO_CODEC_SetMicBstPowerMode(u32 amic_num, u32 powermode);
_LONG_CALL_ void AUDIO_CODEC_SetMicBstChnMute(u32 amic_sel, u32 type, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetMicBstInputMode(u32 amic_num, u32 mode);
_LONG_CALL_ void AUDIO_CODEC_SetMicBstGain(u32 amic_sel, u32 gain);
_LONG_CALL_ void AUDIO_CODEC_SetADCDmicFilter(u32 adc_num, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDmicClk(u32 clk, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDmicSrc(u32 ad_chn, u32 dmic_num);
_LONG_CALL_ void AUDIO_CODEC_SetDACSrc(u32 i2s_sel, u32 dac_l_src, u32 dac_r_src);
_LONG_CALL_ void AUDIO_CODEC_EnableDAC(u32 channel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDACMute(u32 channel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDACVolume(u32 channel, u32 gain);
_LONG_CALL_ void AUDIO_CODEC_SetDACHPF(u32 channel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDACASRC(u32 sr, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDACPowerMode(u32 channel, u32 powermode);
_LONG_CALL_ void AUDIO_CODEC_SetLineOutPowerMode(u32 channel, u32 powermode);
_LONG_CALL_ void AUDIO_CODEC_SetLineOutMode(u32 channel, u32 mode);
_LONG_CALL_ void AUDIO_CODEC_SetLineOutMute(u32 channel, u32 type, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetPDMClk(u32 channel, u32 clk, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetTesttone(u32 fc, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDAADLPBK(u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADDALPBK(u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCDecimationSrc(u32 adc_sel, u32 type);
_LONG_CALL_ void AUDIO_CODEC_SetDACZDET(u32 channel, u32 type);
_LONG_CALL_ void AUDIO_CODEC_SetDACZDETTimeOut(u32 channel, u32 time_out);
_LONG_CALL_ void AUDIO_CODEC_SetADCZDET(u32 adc_sel, u32 type);
_LONG_CALL_ void AUDIO_CODEC_SetADCZDETTimeOut(u32 adc_sel, u32 time_out);
_LONG_CALL_ void AUDIO_CODEC_Record(u32 i2s_sel, u32 type, I2S_InitTypeDef *I2S_InitStruct);
_LONG_CALL_ void AUDIO_CODEC_Playback(u32 i2s_sel, u32 type, I2S_InitTypeDef *I2S_InitStruct);
_LONG_CALL_ void AUDIO_CODEC_EnableDACFifo(u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_EnableADCFifo(u32 ad_chn, u32 newstate);

/**
  * @}
  */

/* MANUAL_GEN_END */

#endif
/** @} */

/** @} */

/** @} */

