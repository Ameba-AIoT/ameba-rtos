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
#define AUD_BIT_DAAD_LPBK_EN                 ((u32)0x00000001 << 1)          /*!<R/W 0  Digital DAC --> ADC loop back control 1'b0: Disable 1'b1: Enable */
#define AUD_BIT_ADDA_LPBK_EN                 ((u32)0x00000001 << 2)          /*!<R/W 0  Digital ADC --> DAC loop back control 1'b0: Disable 1'b1: Enable */
#define AUD_MASK_ADC_0_EQ_NUM_SEL            ((u32)0x00000003 << 5)          /*!<R/W 0  00: 5 band 01: 3 band 02: 0 band 11: Reserved */
#define AUD_ADC_0_EQ_NUM_SEL(x)              (((u32)((x) & 0x00000003) << 5))
#define AUD_GET_ADC_0_EQ_NUM_SEL(x)          ((u32)(((x >> 5) & 0x00000003)))
#define AUD_MASK_ADC_1_EQ_NUM_SEL            ((u32)0x00000003 << 7)          /*!<R/W 0  00: 5 band 01: 3 band 02: 0 band 11: Reserved */
#define AUD_ADC_1_EQ_NUM_SEL(x)              (((u32)((x) & 0x00000003) << 7))
#define AUD_GET_ADC_1_EQ_NUM_SEL(x)          ((u32)(((x >> 7) & 0x00000003)))
#define AUD_MASK_ADC_2_EQ_NUM_SEL            ((u32)0x00000003 << 9)          /*!<R/W 0  00: 5 band 01: 3 band 02: 0 band 11: Reserved */
#define AUD_ADC_2_EQ_NUM_SEL(x)              (((u32)((x) & 0x00000003) << 9))
#define AUD_GET_ADC_2_EQ_NUM_SEL(x)          ((u32)(((x >> 9) & 0x00000003)))
#define AUD_MASK_ADC_3_EQ_NUM_SEL            ((u32)0x00000003 << 11)          /*!<R/W 0  00: 5 band 01: 3 band 02: 0 band 11: Reserved */
#define AUD_ADC_3_EQ_NUM_SEL(x)              (((u32)((x) & 0x00000003) << 11))
#define AUD_GET_ADC_3_EQ_NUM_SEL(x)          ((u32)(((x >> 11) & 0x00000003)))
#define AUD_MASK_AUDIO_DBG_SEL               ((u32)0x0000001F << 13)          /*!<R/W 0  Debug probe selcetion */
#define AUD_AUDIO_DBG_SEL(x)                 (((u32)((x) & 0x0000001F) << 13))
#define AUD_GET_AUDIO_DBG_SEL(x)             ((u32)(((x >> 13) & 0x0000001F)))
/** @} */

/** @defgroup CODEC_audio_control_1
 * @brief Audio Control Register 1
 * @{
 **/
#define AUD_MASK_SEL_BB_CK_DEPOP             ((u32)0x00000003 << 0)          /*!<R/W 1  sel_bb_ck_depop */
#define AUD_SEL_BB_CK_DEPOP(x)               (((u32)((x) & 0x00000003) << 0))
#define AUD_GET_SEL_BB_CK_DEPOP(x)           ((u32)(((x >> 0) & 0x00000003)))
#define AUD_BIT_BB_CK_DEPOP_EN               ((u32)0x00000001 << 2)          /*!<R/W 0  bb_ck_depop_en */
#define AUD_BIT_CKX_MICBIAS_EN               ((u32)0x00000001 << 3)          /*!<R/W 0  ckx_micbias_en */
#define AUD_BIT_PDM_CLK_DOUBLE               ((u32)0x00000001 << 4)          /*!<R/W 0  pdm_clk_double */
#define AUD_MASK_PDM_CLK_SEL                 ((u32)0x00000003 << 5)          /*!<R/W 0  PDM clock selection 2'b0: 5MHz 2'b1: 2.5MHz 2'b2: 6.67MHz */
#define AUD_PDM_CLK_SEL(x)                   (((u32)((x) & 0x00000003) << 5))
#define AUD_GET_PDM_CLK_SEL(x)               ((u32)(((x >> 5) & 0x00000003)))
#define AUD_MASK_PDM_CH_SWAP                 ((u32)0x00000003 << 7)          /*!<R/W 0x2  pdm_ch_swap */
#define AUD_PDM_CH_SWAP(x)                   (((u32)((x) & 0x00000003) << 7))
#define AUD_GET_PDM_CH_SWAP(x)               ((u32)(((x >> 7) & 0x00000003)))
#define AUD_BIT_PDM_DATA_PHASE_SEL           ((u32)0x00000001 << 9)          /*!<R/W 1  pdm_data_phase_sel */
#define AUD_BIT_PDM_GAIN_SHIFT_EN            ((u32)0x00000001 << 10)          /*!<R/W 1  pdm_gain_shift_en */
#define AUD_BIT_PDM_CLK_INV_SEL              ((u32)0x00000001 << 11)          /*!<R/W 1  pdm_clk_inv_sel */
#define AUD_BIT_I2S_DATA_RND_EN              ((u32)0x00000001 << 12)          /*!<R/W 1  i2s_data_rnd_en */
/** @} */

/** @defgroup CODEC_clock_control_1
 * @brief Clock Control Register 1
 * @{
 **/
#define AUD_BIT_DA_L_EN                      ((u32)0x00000001 << 0)          /*!<R/W 0  DAC l channel clock enable */
#define AUD_BIT_DA_R_EN                      ((u32)0x00000001 << 1)          /*!<R/W 0  DAC r channel clock enable */
#define AUD_BIT_MOD_L_EN                     ((u32)0x00000001 << 2)          /*!<R/W 0  SDM l channel clock enable */
#define AUD_BIT_MOD_R_EN                     ((u32)0x00000001 << 3)          /*!<R/W 0  SDM r channel clock enable */
#define AUD_BIT_DA_ANA_CLK_EN                ((u32)0x00000001 << 4)          /*!<R/W 0  DAC/ADC analog clock enable */
#define AUD_BIT_DA_FIFO_EN                   ((u32)0x00000001 << 5)          /*!<R/W 0  DAC FIFO clock enable */
#define AUD_BIT_AD_0_EN                      ((u32)0x00000001 << 6)          /*!<R/W 0  ADC channel 0 clock enable */
#define AUD_BIT_AD_1_EN                      ((u32)0x00000001 << 7)          /*!<R/W 0  ADC channel 1 clock enable */
#define AUD_BIT_AD_2_EN                      ((u32)0x00000001 << 8)          /*!<R/W 0  ADC channel 2 clock enable */
#define AUD_BIT_AD_3_EN                      ((u32)0x00000001 << 9)          /*!<R/W 0  ADC channel 3 clock enable */
#define AUD_BIT_AD_4_EN                      ((u32)0x00000001 << 10)          /*!<R/W 0  ADC channel 4 clock enable */
#define AUD_BIT_AD_5_EN                      ((u32)0x00000001 << 11)          /*!<R/W 0  ADC channel 5 clock enable */
#define AUD_BIT_AD_6_EN                      ((u32)0x00000001 << 12)          /*!<R/W 0  ADC channel 6 clock enable */
#define AUD_BIT_AD_7_EN                      ((u32)0x00000001 << 13)          /*!<R/W 0  ADC channel 7 clock enable */
#define AUD_BIT_AD_0_FIFO_EN                 ((u32)0x00000001 << 14)          /*!<R/W 0  ADC channel 0 FIFO clock enable */
#define AUD_BIT_AD_1_FIFO_EN                 ((u32)0x00000001 << 15)          /*!<R/W 0  ADC channel 1 FIFO clock enable */
#define AUD_BIT_AD_2_FIFO_EN                 ((u32)0x00000001 << 16)          /*!<R/W 0  ADC channel 2 FIFO clock enable */
#define AUD_BIT_AD_3_FIFO_EN                 ((u32)0x00000001 << 17)          /*!<R/W 0  ADC channel 3 FIFO clock enable */
#define AUD_BIT_AD_4_FIFO_EN                 ((u32)0x00000001 << 18)          /*!<R/W 0  ADC channel 4 FIFO clock enable */
#define AUD_BIT_AD_5_FIFO_EN                 ((u32)0x00000001 << 19)          /*!<R/W 0  ADC channel 5 FIFO clock enable */
#define AUD_BIT_AD_6_FIFO_EN                 ((u32)0x00000001 << 20)          /*!<R/W 0  ADC channel 6 FIFO clock enable */
#define AUD_BIT_AD_7_FIFO_EN                 ((u32)0x00000001 << 21)          /*!<R/W 0  ADC channel 7 FIFO clock enable */
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
#define AUD_BIT_AD_ANA_4_EN                  ((u32)0x00000001 << 4)          /*!<R/W 0  ADC filter channel 4 clock enable: analog ADC path */
#define AUD_BIT_AD_ANA_5_EN                  ((u32)0x00000001 << 5)          /*!<R/W 0  ADC filter channel 5 clock enable: analog ADC path */
#define AUD_BIT_AD_ANA_6_EN                  ((u32)0x00000001 << 6)          /*!<R/W 0  ADC filter channel 6 clock enable: analog ADC path */
#define AUD_BIT_AD_ANA_7_EN                  ((u32)0x00000001 << 7)          /*!<R/W 0  ADC filter channel 7 clock enable: analog ADC path */
#define AUD_BIT_DMIC_0_EN                    ((u32)0x00000001 << 8)          /*!<R/W 0  ADC filter channel 0 clock enable: dmic path */
#define AUD_BIT_DMIC_1_EN                    ((u32)0x00000001 << 9)          /*!<R/W 0  ADC filter channel 1 clock enable: dmic path */
#define AUD_BIT_DMIC_2_EN                    ((u32)0x00000001 << 10)          /*!<R/W 0  ADC filter channel 2 clock enable: dmic path */
#define AUD_BIT_DMIC_3_EN                    ((u32)0x00000001 << 11)          /*!<R/W 0  ADC filter channel 3 clock enable: dmic path */
#define AUD_BIT_DMIC_4_EN                    ((u32)0x00000001 << 12)          /*!<R/W 0  ADC filter channel 4 clock enable: dmic path */
#define AUD_BIT_DMIC_5_EN                    ((u32)0x00000001 << 13)          /*!<R/W 0  ADC filter channel 5 clock enable: dmic path */
#define AUD_BIT_DMIC_6_EN                    ((u32)0x00000001 << 14)          /*!<R/W 0  ADC filter channel 6 clock enable: dmic path */
#define AUD_BIT_DMIC_7_EN                    ((u32)0x00000001 << 15)          /*!<R/W 0  ADC filter channel 7 clock enable: dmic path */
#define AUD_BIT_ST_L_EN                      ((u32)0x00000001 << 16)          /*!<R/W 0  Sidetone L channel clock enable */
#define AUD_BIT_ST_R_EN                      ((u32)0x00000001 << 17)          /*!<R/W 0  Sidetone R channel clock enable */
#define AUD_BIT_ST_DS_L_EN                   ((u32)0x00000001 << 18)          /*!<R/W 0  Sidetone downsaple path L channel clock enable */
#define AUD_BIT_ST_DS_R_EN                   ((u32)0x00000001 << 19)          /*!<R/W 0  Sidetone downsaple path R channel clock enable */
#define AUD_BIT_AD_0_EQ_EN                   ((u32)0x00000001 << 20)          /*!<R/W 0  ADC channel 0 EQ clock enable */
#define AUD_BIT_AD_1_EQ_EN                   ((u32)0x00000001 << 21)          /*!<R/W 0  ADC channel 1 EQ clock enable */
#define AUD_BIT_AD_2_EQ_EN                   ((u32)0x00000001 << 22)          /*!<R/W 0  ADC channel 2 EQ clock enable */
#define AUD_BIT_AD_3_EQ_EN                   ((u32)0x00000001 << 23)          /*!<R/W 0  ADC channel 3 EQ clock enable */
#define AUD_BIT_AD_4_EQ_EN                   ((u32)0x00000001 << 24)          /*!<R/W 0  ADC channel 4 EQ clock enable */
#define AUD_BIT_AD_5_EQ_EN                   ((u32)0x00000001 << 25)          /*!<R/W 0  ADC channel 5 EQ clock enable */
#define AUD_BIT_DA_L_EQ_EN                   ((u32)0x00000001 << 26)          /*!<R/W 0  DAC L channel EQ clock enable */
#define AUD_BIT_DA_R_EQ_EN                   ((u32)0x00000001 << 27)          /*!<R/W 0  DAC R channel EQ clock enable */
/** @} */

/** @defgroup CODEC_clock_control_3
 * @brief Clock Control Register 3
 * @{
 **/
#define AUD_MASK_DMIC_CLK_SEL                ((u32)0x00000007 << 0)          /*!<R/W 1  Set clock of digital microphone 3'b000: 5MHz 3'b001: 2.5MHz 3'b010: 1.25MHz 3'b011: 625kHz 3'b100: 312.5kHz 3'b101: Reseved 3'b110: Reserved 3'b111: 769.2kHz */
#define AUD_DMIC_CLK_SEL(x)                  (((u32)((x) & 0x00000007) << 0))
#define AUD_GET_DMIC_CLK_SEL(x)              ((u32)(((x >> 0) & 0x00000007)))
#define AUD_BIT_DMIC_CLK_EN                  ((u32)0x00000001 << 3)          /*!<R/W 0  Digital microphone clock enable */
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
#define AUD_BIT_SAMPLE_RATE_0_EN             ((u32)0x00000001 << 12)          /*!<R/W 1  Sample rate source 0 power enable */
#define AUD_BIT_SAMPLE_RATE_1_EN             ((u32)0x00000001 << 13)          /*!<R/W 1  Sample rate source 1 power enable */
#define AUD_BIT_SAMPLE_RATE_2_EN             ((u32)0x00000001 << 14)          /*!<R/W 1  Sample rate source 2 power enable */
/** @} */

/** @defgroup CODEC_clock_control_5
 * @brief Clock Control Register 5
 * @{
 **/
#define AUD_MASK_ADC_0_FS_SRC_SEL            ((u32)0x00000003 << 0)          /*!<R/W 0  Channel 0 ADC path sample rate Source selection 2'b00: Source 0 2'b01: Source 1 2'b10: Source 2 */
#define AUD_ADC_0_FS_SRC_SEL(x)              (((u32)((x) & 0x00000003) << 0))
#define AUD_GET_ADC_0_FS_SRC_SEL(x)          ((u32)(((x >> 0) & 0x00000003)))
#define AUD_MASK_ADC_1_FS_SRC_SEL            ((u32)0x00000003 << 2)          /*!<R/W 0  Channel 1 ADC path sample rate Source selection 2'b00: Source 0 2'b01: Source 1 2'b10: Source 2 */
#define AUD_ADC_1_FS_SRC_SEL(x)              (((u32)((x) & 0x00000003) << 2))
#define AUD_GET_ADC_1_FS_SRC_SEL(x)          ((u32)(((x >> 2) & 0x00000003)))
#define AUD_MASK_ADC_2_FS_SRC_SEL            ((u32)0x00000003 << 4)          /*!<R/W 0  Channel 2 ADC path sample rate Source selection 2'b00: Source 0 2'b01: Source 1 2'b10: Source 2 */
#define AUD_ADC_2_FS_SRC_SEL(x)              (((u32)((x) & 0x00000003) << 4))
#define AUD_GET_ADC_2_FS_SRC_SEL(x)          ((u32)(((x >> 4) & 0x00000003)))
#define AUD_MASK_ADC_3_FS_SRC_SEL            ((u32)0x00000003 << 6)          /*!<R/W 0  Channel 3 ADC path sample rate Source selection 2'b00: Source 0 2'b01: Source 1 2'b10: Source 2 */
#define AUD_ADC_3_FS_SRC_SEL(x)              (((u32)((x) & 0x00000003) << 6))
#define AUD_GET_ADC_3_FS_SRC_SEL(x)          ((u32)(((x >> 6) & 0x00000003)))
#define AUD_MASK_ADC_4_FS_SRC_SEL            ((u32)0x00000003 << 8)          /*!<R/W 0  Channel 4 ADC path sample rate Source selection 2'b00: Source 0 2'b01: Source 1 2'b10: Source 2 */
#define AUD_ADC_4_FS_SRC_SEL(x)              (((u32)((x) & 0x00000003) << 8))
#define AUD_GET_ADC_4_FS_SRC_SEL(x)          ((u32)(((x >> 8) & 0x00000003)))
#define AUD_MASK_ADC_5_FS_SRC_SEL            ((u32)0x00000003 << 10)          /*!<R/W 0  Channel 5 ADC path sample rate Source selection 2'b00: Source 0 2'b01: Source 1 2'b10: Source 2 */
#define AUD_ADC_5_FS_SRC_SEL(x)              (((u32)((x) & 0x00000003) << 10))
#define AUD_GET_ADC_5_FS_SRC_SEL(x)          ((u32)(((x >> 10) & 0x00000003)))
#define AUD_MASK_ADC_6_FS_SRC_SEL            ((u32)0x00000003 << 12)          /*!<R/W 0  Channel 6 ADC path sample rate Source selection 2'b00: Source 0 2'b01: Source 1 2'b10: Source 2 */
#define AUD_ADC_6_FS_SRC_SEL(x)              (((u32)((x) & 0x00000003) << 12))
#define AUD_GET_ADC_6_FS_SRC_SEL(x)          ((u32)(((x >> 12) & 0x00000003)))
#define AUD_MASK_ADC_7_FS_SRC_SEL            ((u32)0x00000003 << 14)          /*!<R/W 0  Channel 7 ADC path sample rate Source selection 2'b00: Source 0 2'b01: Source 1 2'b10: Source 2 */
#define AUD_ADC_7_FS_SRC_SEL(x)              (((u32)((x) & 0x00000003) << 14))
#define AUD_GET_ADC_7_FS_SRC_SEL(x)          ((u32)(((x >> 14) & 0x00000003)))
#define AUD_MASK_DAC_FS_SRC_SEL              ((u32)0x00000003 << 16)          /*!<R/W 0  DAC path sample rate Source selection 2'b00: Source 0 2'b01: Source 1 2'b10: Source 2 */
#define AUD_DAC_FS_SRC_SEL(x)                (((u32)((x) & 0x00000003) << 16))
#define AUD_GET_DAC_FS_SRC_SEL(x)            ((u32)(((x >> 16) & 0x00000003)))
/** @} */

/** @defgroup CODEC_clock_control_6
 * @brief Clock Control Register 6
 * @{
 **/
#define AUD_BIT_ADC_0_ASRC_EN                ((u32)0x00000001 << 0)          /*!<R/W 0  Channel 0 ADC path ASRC enable 0: Disable 1: Enable If ASRC is enabled, sample_rate becomes useless. */
#define AUD_BIT_ADC_1_ASRC_EN                ((u32)0x00000001 << 1)          /*!<R/W 0  Channel 1 ADC path ASRC enable 0: Disable 1: Enable If ASRC is enabled, sample_rate becomes useless. */
#define AUD_BIT_ADC_2_ASRC_EN                ((u32)0x00000001 << 2)          /*!<R/W 0  Channel 2 ADC path ASRC enable 0: Disable 1: Enable If ASRC is enabled, sample_rate becomes useless. */
#define AUD_BIT_ADC_3_ASRC_EN                ((u32)0x00000001 << 3)          /*!<R/W 0  Channel 3 ADC path ASRC enable 0: Disable 1: Enable If ASRC is enabled, sample_rate becomes useless. */
#define AUD_BIT_ADC_4_ASRC_EN                ((u32)0x00000001 << 4)          /*!<R/W 0  Channel 4 ADC path ASRC enable 0: Disable 1: Enable If ASRC is enabled, sample_rate becomes useless. */
#define AUD_BIT_ADC_5_ASRC_EN                ((u32)0x00000001 << 5)          /*!<R/W 0  Channel 5 ADC path ASRC enable 0: Disable 1: Enable If ASRC is enabled, sample_rate becomes useless. */
#define AUD_BIT_ADC_6_ASRC_EN                ((u32)0x00000001 << 6)          /*!<R/W 0  Channel 6 ADC path ASRC enable 0: Disable 1: Enable If ASRC is enabled, sample_rate becomes useless. */
#define AUD_BIT_ADC_7_ASRC_EN                ((u32)0x00000001 << 7)          /*!<R/W 0  Channel 7 ADC path ASRC enable 0: Disable 1: Enable If ASRC is enabled, sample_rate becomes useless. */
#define AUD_BIT_DAC_ASRC_EN                  ((u32)0x00000001 << 8)          /*!<R/W 0  DAC path ASRC enable 0: Disable 1: Enable If ASRC is enabled, sample_rate becomes useless. */
/** @} */

/** @defgroup CODEC_clock_control_7
 * @brief Clock Control Register 7
 * @{
 **/
#define AUD_BIT_DA_ANA_CLK_SEL               ((u32)0x00000001 << 0)          /*!<R/W 0  Set clk_da_ana phase 1'b0: Inphase 1'b1: Inverse phase */
#define AUD_BIT_AD_ANA_CLK_SEL               ((u32)0x00000001 << 1)          /*!<R/W 0  Set clk_ad_ana phase 1'b0: Inphase 1'b1: Inverse phase */
#define AUD_BIT_ADC_LATCH_PHASE              ((u32)0x00000001 << 2)          /*!<R/W 0  Set latch adc data phase 1'b0: Inphase 1'b1: Inverse phase */
#define AUD_MASK_ANA_CLK_RATE_SEL            ((u32)0x00000003 << 3)          /*!<R/W 0  Set DA/AD analog clock 0: 10MHz 1: 5MHz 2: 2.5MHz 3: Reseved */
#define AUD_ANA_CLK_RATE_SEL(x)              (((u32)((x) & 0x00000003) << 3))
#define AUD_GET_ANA_CLK_RATE_SEL(x)          ((u32)(((x >> 3) & 0x00000003)))
#define AUD_MASK_AD_LPF_CLK_RATE_SEL         ((u32)0x00000003 << 5)          /*!<R/W 0  Set AD LPF clock 0: 10MHz 1: 5MHz 2: 2.5MHz 3: Reseved */
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
#define AUD_MASK_ADC_4_DMIC_LPF_CLK_SEL      ((u32)0x00000003 << 15)          /*!<R/W 0  Channel 4 ADC path DMIC LPF clock 2'b00: 10M 2'b01: 5M 2'b10: 2.5M 2'b11: 769.2*2K */
#define AUD_ADC_4_DMIC_LPF_CLK_SEL(x)        (((u32)((x) & 0x00000003) << 15))
#define AUD_GET_ADC_4_DMIC_LPF_CLK_SEL(x)    ((u32)(((x >> 15) & 0x00000003)))
#define AUD_MASK_ADC_5_DMIC_LPF_CLK_SEL      ((u32)0x00000003 << 17)          /*!<R/W 0  Channel 5 ADC path DMIC LPF clock 2'b00: 10M 2'b01: 5M 2'b10: 2.5M 2'b11: 769.2*2K */
#define AUD_ADC_5_DMIC_LPF_CLK_SEL(x)        (((u32)((x) & 0x00000003) << 17))
#define AUD_GET_ADC_5_DMIC_LPF_CLK_SEL(x)    ((u32)(((x >> 17) & 0x00000003)))
#define AUD_MASK_ADC_6_DMIC_LPF_CLK_SEL      ((u32)0x00000003 << 19)          /*!<R/W 0  Channel 6 ADC path DMIC LPF clock 2'b00: 10M 2'b01: 5M 2'b10: 2.5M 2'b11: 769.2*2K */
#define AUD_ADC_6_DMIC_LPF_CLK_SEL(x)        (((u32)((x) & 0x00000003) << 19))
#define AUD_GET_ADC_6_DMIC_LPF_CLK_SEL(x)    ((u32)(((x >> 19) & 0x00000003)))
#define AUD_MASK_ADC_7_DMIC_LPF_CLK_SEL      ((u32)0x00000003 << 21)          /*!<R/W 0  Channel 7 ADC path DMIC LPF clock 2'b00: 10M 2'b01: 5M 2'b10: 2.5M 2'b11: 769.2*2K */
#define AUD_ADC_7_DMIC_LPF_CLK_SEL(x)        (((u32)((x) & 0x00000003) << 21))
#define AUD_GET_ADC_7_DMIC_LPF_CLK_SEL(x)    ((u32)(((x >> 21) & 0x00000003)))
/** @} */

/** @defgroup CODEC_asrc_control_0
 * @brief
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

/** @defgroup CODEC_asrc_control_4
 * @brief ASRC Control Register 4
 * @{
 **/
#define AUD_MASK_ASRC_RATE_SEL_RX_1          ((u32)0x00000003 << 0)          /*!<R/W 0  0: fs supports 4kHz ~ 60kHz 1: fs supports 60kHz ~ 120kHz 2/3: Reserved */
#define AUD_ASRC_RATE_SEL_RX_1(x)            (((u32)((x) & 0x00000003) << 0))
#define AUD_GET_ASRC_RATE_SEL_RX_1(x)        ((u32)(((x >> 0) & 0x00000003)))
#define AUD_MASK_ASRC_GAIN_SEL_RX_1          ((u32)0x00000003 << 2)          /*!<R/W 0x3  ASRC convergence rate: larger is faster but more noisy */
#define AUD_ASRC_GAIN_SEL_RX_1(x)            (((u32)((x) & 0x00000003) << 2))
#define AUD_GET_ASRC_GAIN_SEL_RX_1(x)        ((u32)(((x >> 2) & 0x00000003)))
#define AUD_BIT_ASRC_AUTO_ADJUST_RX_1        ((u32)0x00000001 << 4)          /*!<R/W 1  HW auto adjust convergence rate 0: Disable 1: Enable */
/** @} */

/** @defgroup CODEC_asrc_control_5
 * @brief ASRC Control Register 5
 * @{
 **/
#define AUD_MASK_ASRC_SDM_INTI_RX_1          ((u32)0x00FFFFFF << 0)          /*!<R/W 0  Set initial value of tracked frequency */
#define AUD_ASRC_SDM_INTI_RX_1(x)            (((u32)((x) & 0x00FFFFFF) << 0))
#define AUD_GET_ASRC_SDM_INTI_RX_1(x)        ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */

/** @defgroup CODEC_i2s_x_control
 * @brief I2S x Control Register
 * @{
 **/
#define AUD_BIT_I2S_x_RST_N_REG              ((u32)0x00000001 << 0)          /*!<R/W 0  I2S x 1'b1: rst_n to audio digital IP is de-asserted 1'b0: rst_n to audio digital IP is asserted */
#define AUD_BIT_I2S_x_INV_SCLK               ((u32)0x00000001 << 1)          /*!<R/W 0  I2S x 1'b1: I2S/PCM bit clock is inverted */
#define AUD_BIT_I2S_x_SELF_LPBK_EN           ((u32)0x00000001 << 2)          /*!<R/W 0  I2S x 1'b1: internal loopback mode is enabled */
#define AUD_BIT_I2S_x_SAME_LRC_EN            ((u32)0x00000001 << 3)          /*!<R/W 0  I2S x 1'b1: LRC_TX share to LRC_RX */
#define AUD_MASK_I2S_x_TDM_MODE_RX           ((u32)0x00000003 << 4)          /*!<R/W 0  I2S x Rx channel TDM mode 0: 2 channel 1: 4 channel 2: 6 channel 3: 8 channel */
#define AUD_I2S_x_TDM_MODE_RX(x)             (((u32)((x) & 0x00000003) << 4))
#define AUD_GET_I2S_x_TDM_MODE_RX(x)         ((u32)(((x >> 4) & 0x00000003)))
#define AUD_MASK_I2S_x_DATA_FORMAT_SEL_TX    ((u32)0x00000003 << 6)          /*!<R/W 0  I2S x Tx channel data format 2'b00: I2S 2'b01: left-justified 2'b10: PCM mode A 2'b11: PCM mode B */
#define AUD_I2S_x_DATA_FORMAT_SEL_TX(x)      (((u32)((x) & 0x00000003) << 6))
#define AUD_GET_I2S_x_DATA_FORMAT_SEL_TX(x)  ((u32)(((x >> 6) & 0x00000003)))
#define AUD_MASK_I2S_x_DATA_FORMAT_SEL_RX    ((u32)0x00000003 << 8)          /*!<R/W 0  I2S x Rx channel data format 2'b00: I2S 2'b01: left-justified 2'b10: PCM mode A 2'b11: PCM mode B */
#define AUD_I2S_x_DATA_FORMAT_SEL_RX(x)      (((u32)((x) & 0x00000003) << 8))
#define AUD_GET_I2S_x_DATA_FORMAT_SEL_RX(x)  ((u32)(((x >> 8) & 0x00000003)))
#define AUD_MASK_I2S_x_DATA_LEN_SEL_TX       ((u32)0x00000003 << 10)          /*!<R/W 0  I2S x Tx channel data length 2'b00: 16 bits 2'b10: 24 bits 2'b11: 8 bits */
#define AUD_I2S_x_DATA_LEN_SEL_TX(x)         (((u32)((x) & 0x00000003) << 10))
#define AUD_GET_I2S_x_DATA_LEN_SEL_TX(x)     ((u32)(((x >> 10) & 0x00000003)))
#define AUD_MASK_I2S_x_DATA_LEN_SEL_RX       ((u32)0x00000003 << 12)          /*!<R/W 0  I2S x Rx channel data length 2'b00: 16 bits 2'b10: 24 bits 2'b11: 8 bits */
#define AUD_I2S_x_DATA_LEN_SEL_RX(x)         (((u32)((x) & 0x00000003) << 12))
#define AUD_GET_I2S_x_DATA_LEN_SEL_RX(x)     ((u32)(((x >> 12) & 0x00000003)))
#define AUD_MASK_I2S_x_CH_LEN_SEL_TX         ((u32)0x00000003 << 14)          /*!<R/W 0  I2S x Tx channel channel length 2'b00: 16 bits 2'b01: 32 bits 2'b10: 24 bits 2'b11: 8 bits */
#define AUD_I2S_x_CH_LEN_SEL_TX(x)           (((u32)((x) & 0x00000003) << 14))
#define AUD_GET_I2S_x_CH_LEN_SEL_TX(x)       ((u32)(((x >> 14) & 0x00000003)))
#define AUD_MASK_I2S_x_CH_LEN_SEL_RX         ((u32)0x00000003 << 16)          /*!<R/W 0  I2S x Rx channel chennel length 2'b00: 16 bits 2'b01: 32 bits 2'b10: 24 bits 2'b11: 8 bits */
#define AUD_I2S_x_CH_LEN_SEL_RX(x)           (((u32)((x) & 0x00000003) << 16))
#define AUD_GET_I2S_x_CH_LEN_SEL_RX(x)       ((u32)(((x >> 16) & 0x00000003)))
#define AUD_MASK_I2S_x_DATA_CH_SEL_TX        ((u32)0x00000003 << 18)          /*!<R/W 0  I2S x Tx channel data channel 2'b00: L/R 2'b01: R/L 2'b10: L/L 2'b11: R/R */
#define AUD_I2S_x_DATA_CH_SEL_TX(x)          (((u32)((x) & 0x00000003) << 18))
#define AUD_GET_I2S_x_DATA_CH_SEL_TX(x)      ((u32)(((x >> 18) & 0x00000003)))
#define AUD_BIT_I2S_x_MASTER_SEL             ((u32)0x00000001 << 20)          /*!<R/W 0  I2S x master source sel 1'b0: sport internal 1'b1: external I2S */
/** @} */

/** @defgroup CODEC_i2s_x_control_1
 * @brief I2S x Control Register 1
 * @{
 **/
#define AUD_MASK_I2S_x_DATA_CH0_SEL_RX       ((u32)0x00000007 << 0)          /*!<R/W 0  I2S x Rx channel data channel 0 0: CH0 1: CH1 2: CH2 3: CH3 4: CH4 5: CH5 6: CH6 7: CH7 */
#define AUD_I2S_x_DATA_CH0_SEL_RX(x)         (((u32)((x) & 0x00000007) << 0))
#define AUD_GET_I2S_x_DATA_CH0_SEL_RX(x)     ((u32)(((x >> 0) & 0x00000007)))
#define AUD_MASK_I2S_x_DATA_CH1_SEL_RX       ((u32)0x00000007 << 3)          /*!<R/W 1  I2S x Rx channel data channel 1 0: CH0 1: CH1 2: CH2 3: CH3 4: CH4 5: CH5 6: CH6 7: CH7 */
#define AUD_I2S_x_DATA_CH1_SEL_RX(x)         (((u32)((x) & 0x00000007) << 3))
#define AUD_GET_I2S_x_DATA_CH1_SEL_RX(x)     ((u32)(((x >> 3) & 0x00000007)))
#define AUD_MASK_I2S_x_DATA_CH2_SEL_RX       ((u32)0x00000007 << 6)          /*!<R/W 0  I2S x Rx channel data channel 2 0: CH0 1: CH1 2: CH2 3: CH3 4: CH4 5: CH5 6: CH6 7: CH7 */
#define AUD_I2S_x_DATA_CH2_SEL_RX(x)         (((u32)((x) & 0x00000007) << 6))
#define AUD_GET_I2S_x_DATA_CH2_SEL_RX(x)     ((u32)(((x >> 6) & 0x00000007)))
#define AUD_MASK_I2S_x_DATA_CH3_SEL_RX       ((u32)0x00000007 << 9)          /*!<R/W 0  I2S x Rx channel data channel 3 0: CH0 1: CH1 2: CH2 3: CH3 4: CH4 5: CH5 6: CH6 7: CH7 */
#define AUD_I2S_x_DATA_CH3_SEL_RX(x)         (((u32)((x) & 0x00000007) << 9))
#define AUD_GET_I2S_x_DATA_CH3_SEL_RX(x)     ((u32)(((x >> 9) & 0x00000007)))
#define AUD_MASK_I2S_x_DATA_CH4_SEL_RX       ((u32)0x00000007 << 12)          /*!<R/W 0  I2S x Rx channel data channel 4 0: CH0 1: CH1 2: CH2 3: CH3 4: CH4 5: CH5 6: CH6 7: CH7 */
#define AUD_I2S_x_DATA_CH4_SEL_RX(x)         (((u32)((x) & 0x00000007) << 12))
#define AUD_GET_I2S_x_DATA_CH4_SEL_RX(x)     ((u32)(((x >> 12) & 0x00000007)))
#define AUD_MASK_I2S_x_DATA_CH5_SEL_RX       ((u32)0x00000007 << 15)          /*!<R/W 1  I2S x Rx channel data channel 5 0: CH0 1: CH1 2: CH2 3: CH3 4: CH4 5: CH5 6: CH6 7: CH7 */
#define AUD_I2S_x_DATA_CH5_SEL_RX(x)         (((u32)((x) & 0x00000007) << 15))
#define AUD_GET_I2S_x_DATA_CH5_SEL_RX(x)     ((u32)(((x >> 15) & 0x00000007)))
#define AUD_MASK_I2S_x_DATA_CH6_SEL_RX       ((u32)0x00000007 << 18)          /*!<R/W 0  I2S x Rx channel data channel 6 0: CH0 1: CH1 2: CH2 3: CH3 4: CH4 5: CH5 6: CH6 7: CH7 */
#define AUD_I2S_x_DATA_CH6_SEL_RX(x)         (((u32)((x) & 0x00000007) << 18))
#define AUD_GET_I2S_x_DATA_CH6_SEL_RX(x)     ((u32)(((x >> 18) & 0x00000007)))
#define AUD_MASK_I2S_x_DATA_CH7_SEL_RX       ((u32)0x00000007 << 21)          /*!<R/W 0  I2S x Rx channel data channel 7 0: CH0 1: CH1 2: CH2 3: CH3 4: CH4 5: CH5 6: CH6 7: CH7 */
#define AUD_I2S_x_DATA_CH7_SEL_RX(x)         (((u32)((x) & 0x00000007) << 21))
#define AUD_GET_I2S_x_DATA_CH7_SEL_RX(x)     ((u32)(((x >> 21) & 0x00000007)))
#define AUD_BIT_I2S_x_DATA_CH0_RX_DISABLE    ((u32)0x00000001 << 24)          /*!<R/W 0  I2S x Rx channel data channel 0 1: disabe 0: enable */
#define AUD_BIT_I2S_x_DATA_CH1_RX_DISABLE    ((u32)0x00000001 << 25)          /*!<R/W 0  I2S x Rx channel data channel 1 1: disabe 0: enable */
#define AUD_BIT_I2S_x_DATA_CH2_RX_DISABLE    ((u32)0x00000001 << 26)          /*!<R/W 0  I2S x Rx channel data channel 2 1: disabe 0: enable */
#define AUD_BIT_I2S_x_DATA_CH3_RX_DISABLE    ((u32)0x00000001 << 27)          /*!<R/W 0  I2S x Rx channel data channel 3 1: disabe 0: enable */
#define AUD_BIT_I2S_x_DATA_CH4_RX_DISABLE    ((u32)0x00000001 << 28)          /*!<R/W 0  I2S x Rx channel data channel 4 1: disabe 0: enable */
#define AUD_BIT_I2S_x_DATA_CH5_RX_DISABLE    ((u32)0x00000001 << 29)          /*!<R/W 0  I2S x Rx channel data channel 5 1: disabe 0: enable */
#define AUD_BIT_I2S_x_DATA_CH6_RX_DISABLE    ((u32)0x00000001 << 30)          /*!<R/W 0  I2S x Rx channel data channel 6 1: disabe 0: enable */
#define AUD_BIT_I2S_x_DATA_CH7_RX_DISABLE    ((u32)0x00000001 << 31)          /*!<R/W 0  I2S x Rx channel data channel 7 1: disabe 0: enable */
/** @} */

/** @defgroup CODEC_i2s_ad_sel_control
 * @brief I2S ADC Selection Control Register
 * @{
 **/
#define AUD_BIT_ADC_0_I2S_SRC_SEL            ((u32)0x00000001 << 0)          /*!<R/W 0  Channel 0 ADC path I2S selection 1: I2S 1 0: I2S 0 */
#define AUD_BIT_ADC_1_I2S_SRC_SEL            ((u32)0x00000001 << 1)          /*!<R/W 0  Channel 1 ADC path I2S selection 1: I2S 1 0: I2S 0 */
#define AUD_BIT_ADC_2_I2S_SRC_SEL            ((u32)0x00000001 << 2)          /*!<R/W 1  Channel 2 ADC path I2S selection 1: I2S 1 0: I2S 0 */
#define AUD_BIT_ADC_3_I2S_SRC_SEL            ((u32)0x00000001 << 3)          /*!<R/W 1  Channel 3 ADC path I2S selection 1: I2S 1 0: I2S 0 */
#define AUD_BIT_ADC_4_I2S_SRC_SEL            ((u32)0x00000001 << 4)          /*!<R/W 1  Channel 4 ADC path I2S selection 1: I2S 1 0: I2S 0 */
#define AUD_BIT_ADC_5_I2S_SRC_SEL            ((u32)0x00000001 << 5)          /*!<R/W 1  Channel 5 ADC path I2S selection 1: I2S 1 0: I2S 0 */
#define AUD_BIT_ADC_6_I2S_SRC_SEL            ((u32)0x00000001 << 6)          /*!<R/W 1  Channel 6 ADC path I2S selection 1: I2S 1 0: I2S 0 */
#define AUD_BIT_ADC_7_I2S_SRC_SEL            ((u32)0x00000001 << 7)          /*!<R/W 1  Channel 7 ADC path I2S selection 1: I2S 1 0: I2S 0 */
#define AUD_BIT_DAC_I2S_SRC_SEL              ((u32)0x00000001 << 16)          /*!<R/W 0  DAC path I2S selection 1: I2S 1 0: I2S 0 */
/** @} */

/** @defgroup CODEC_adc_x_control_0
 * @brief ADC Channel x Control Register 0
 * @{
 **/
#define AUD_MASK_ADC_x_DMIC_SRC_SEL          ((u32)0x00000007 << 0)          /*!<R/W 0  Channel x DMIC source selection 3'b000: MIC 1 rising 3'b001: MIC 1 falling 3'b010: MIC 2 rising 3'b011: MIC 2 falling 3'b100: MIC 3 rising 3'b101: MIC 3 falling 3'b110: MIC 4 rising 3'b111: MIC 4 falling */
#define AUD_ADC_x_DMIC_SRC_SEL(x)            (((u32)((x) & 0x00000007) << 0))
#define AUD_GET_ADC_x_DMIC_SRC_SEL(x)        ((u32)(((x >> 0) & 0x00000007)))
#define AUD_BIT_ADC_x_DMIC_LPF2ND_EN         ((u32)0x00000001 << 3)          /*!<R/W 1  Channel x DMIC path SRC 2nd LPF control 0: Disable 1: Enable */
#define AUD_BIT_ADC_x_DMIC_LPF1ST_EN         ((u32)0x00000001 << 4)          /*!<R/W 1  Channel x DMIC path SRC 1st LPF control 0: Disable 1: Enable */
#define AUD_MASK_ADC_x_DMIC_LPF1ST_FC_SEL    ((u32)0x00000003 << 5)          /*!<R/W 0  Channel x DMIC path SRC 1st LPF FC */
#define AUD_ADC_x_DMIC_LPF1ST_FC_SEL(x)      (((u32)((x) & 0x00000003) << 5))
#define AUD_GET_ADC_x_DMIC_LPF1ST_FC_SEL(x)  ((u32)(((x >> 5) & 0x00000003)))
#define AUD_BIT_ADC_x_DMIC_MIX_MUTE          ((u32)0x00000001 << 7)          /*!<R/W 1  Channel x DMIC input path mute 0: Un-Mute 1: Mute */
#define AUD_MASK_ADC_x_AD_SRC_SEL            ((u32)0x00000007 << 8)          /*!<R/W 0  Channel x ANA ADC source selection 3'd0: ADC 1 3'd1: ADC 2 3'd2: ADC 3 3'd3: ADC 4 3'd4: ADC 5 */
#define AUD_ADC_x_AD_SRC_SEL(x)              (((u32)((x) & 0x00000007) << 8))
#define AUD_GET_ADC_x_AD_SRC_SEL(x)          ((u32)(((x >> 8) & 0x00000007)))
#define AUD_BIT_ADC_x_AD_LPF2ND_EN           ((u32)0x00000001 << 11)          /*!<R/W 1  Channel x ADC path SRC 2nd LPF control 0: Disable 1: Enable */
#define AUD_BIT_ADC_x_AD_LPF1ST_EN           ((u32)0x00000001 << 12)          /*!<R/W 1  Channel x ADC path SRC 1st LPF control 0: Disable 1: Enable */
#define AUD_MASK_ADC_x_AD_LPF1ST_FC_SEL      ((u32)0x00000003 << 13)          /*!<R/W 0  Channel x ADC path SRC 1st LPF FC */
#define AUD_ADC_x_AD_LPF1ST_FC_SEL(x)        (((u32)((x) & 0x00000003) << 13))
#define AUD_GET_ADC_x_AD_LPF1ST_FC_SEL(x)    ((u32)(((x >> 13) & 0x00000003)))
#define AUD_BIT_ADC_x_AD_MIX_MUTE            ((u32)0x00000001 << 15)          /*!<R/W 1  Channel x ADC input path mute 0: Un-Mute 1: Mute */
#define AUD_MASK_ADC_x_AD_ZDET_FUNC          ((u32)0x00000003 << 16)          /*!<R/W 0x2  Channel x ADC path zero detection function select 2'b00: immediate change 2'b01: zero detection & immediate change 2'b10: zdet & step 2'b11: zdet & timeout step */
#define AUD_ADC_x_AD_ZDET_FUNC(x)            (((u32)((x) & 0x00000003) << 16))
#define AUD_GET_ADC_x_AD_ZDET_FUNC(x)        ((u32)(((x >> 16) & 0x00000003)))
#define AUD_MASK_ADC_x_AD_ZDET_TOUT          ((u32)0x00000003 << 18)          /*!<R/W 0  Channel x ADC path zero detection time out select 2'b00: 1024*16 samples 2'b01: 1024*32 samples 2'b10: 1024*64 samples 2'b11: 64 samples */
#define AUD_ADC_x_AD_ZDET_TOUT(x)            (((u32)((x) & 0x00000003) << 18))
#define AUD_GET_ADC_x_AD_ZDET_TOUT(x)        ((u32)(((x >> 18) & 0x00000003)))
#define AUD_BIT_ADC_x_AD_MUTE                ((u32)0x00000001 << 20)          /*!<R/W 0  Channel x ADC path mute 0: Un-Mute 1: Mute */
#define AUD_MASK_ADC_x_DECI_SRC_SEL          ((u32)0x00000003 << 22)          /*!<R/W 0  Channel x ADC path decimation source select 0: ADC 1: ANC 2: MUSIC 3: ANC + MUSIC */
#define AUD_ADC_x_DECI_SRC_SEL(x)            (((u32)((x) & 0x00000003) << 22))
#define AUD_GET_ADC_x_DECI_SRC_SEL(x)        ((u32)(((x >> 22) & 0x00000003)))
#define AUD_BIT_ADC_x_DCHPF_EN               ((u32)0x00000001 << 24)          /*!<R/W 0  Channel x ADC path high pass filter enable control (filter DC) 0: Disable 1: Enable */
#define AUD_MASK_ADC_x_DCHPF_FC_SEL          ((u32)0x00000007 << 25)          /*!<R/W 0x5  Channel x ADC path high pass filter Fc (Fc: -3dB) 0: 5e10^-3 Fs 1: 2.5e10^-3 Fs 2: 1.125e10^-3 Fs 3: 6.25e10^-4 Fs 4: 3.125e10^-4 Fs 5: 1.5625e10^-4 Fs 6: 7.8125e10^-5 Fs 7: 3.90625e10^-5 Fs */
#define AUD_ADC_x_DCHPF_FC_SEL(x)            (((u32)((x) & 0x00000007) << 25))
#define AUD_GET_ADC_x_DCHPF_FC_SEL(x)        ((u32)(((x >> 25) & 0x00000007)))
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
#define AUD_MASK_ADC_x_RSVD                  ((u32)0x00000003 << 10)          /*!<R/W 0  Dummy */
#define AUD_ADC_x_RSVD(x)                    (((u32)((x) & 0x00000003) << 10))
#define AUD_GET_ADC_x_RSVD(x)                ((u32)(((x >> 10) & 0x00000003)))
#define AUD_MASK_ADC_x_RPTR_HOLD             ((u32)0x0000000F << 12)          /*!<R/W 0  Channel x I2S read point hold number rptr_hold = (I2S_fs/ad_fs) - 1 */
#define AUD_ADC_x_RPTR_HOLD(x)               (((u32)((x) & 0x0000000F) << 12))
#define AUD_GET_ADC_x_RPTR_HOLD(x)           ((u32)(((x >> 12) & 0x0000000F)))
/** @} */

/** @defgroup CODEC_dac_l_control_0
 * @brief DAC Left Channel Control Register 0
 * @{
 **/
#define AUD_MASK_DAC_L_DA_GAIN               ((u32)0x000000FF << 0)          /*!<R/W 0xaf  Channel L DAC path dvol gain control (0.375dB/step) 8'hAF: 0dB 8'h00: -65.625dB */
#define AUD_DAC_L_DA_GAIN(x)                 (((u32)((x) & 0x000000FF) << 0))
#define AUD_GET_DAC_L_DA_GAIN(x)             ((u32)(((x >> 0) & 0x000000FF)))
#define AUD_MASK_DAC_L_DA_SRC_SEL            ((u32)0x00000003 << 16)          /*!<R/W 0  Channel L DAC path input select 0: I2S L channel 1: I2S R channel 2: I2S L+R channel 3: Test tone */
#define AUD_DAC_L_DA_SRC_SEL(x)              (((u32)((x) & 0x00000003) << 16))
#define AUD_GET_DAC_L_DA_SRC_SEL(x)          ((u32)(((x >> 16) & 0x00000003)))
#define AUD_BIT_DAC_L_TEST_TONE_EN           ((u32)0x00000001 << 20)          /*!<R/W 0  Channel L DAC path test tone enable */
#define AUD_MASK_DAC_L_TEST_FC_SEL           ((u32)0x0000007F << 21)          /*!<R/W 0x0  Channel L DAC path test tone frquency: (fs/192)*(tone_fc_sel+1)Hz */
#define AUD_DAC_L_TEST_FC_SEL(x)             (((u32)((x) & 0x0000007F) << 21))
#define AUD_GET_DAC_L_TEST_FC_SEL(x)         ((u32)(((x >> 21) & 0x0000007F)))
#define AUD_MASK_DAC_L_TEST_GAIN_SEL         ((u32)0x0000000F << 28)          /*!<R/W 0  Channel L DAC path test tone gain control 0 -6.02*(gain_sel)dB */
#define AUD_DAC_L_TEST_GAIN_SEL(x)           (((u32)((x) & 0x0000000F) << 28))
#define AUD_GET_DAC_L_TEST_GAIN_SEL(x)       ((u32)(((x >> 28) & 0x0000000F)))
/** @} */

/** @defgroup CODEC_dac_l_control_1
 * @brief DAC Left Channel Control Register 1
 * @{
 **/
#define AUD_MASK_DAC_L_DA_ZDET_FUNC          ((u32)0x00000003 << 0)          /*!<R/W 0x2  Channel L DAC path zero detection function select 2'b00: immediate change 2'b01: zero detection & immediate change 2'b10: zdet & step 2'b11: zdet & timeout step */
#define AUD_DAC_L_DA_ZDET_FUNC(x)            (((u32)((x) & 0x00000003) << 0))
#define AUD_GET_DAC_L_DA_ZDET_FUNC(x)        ((u32)(((x >> 0) & 0x00000003)))
#define AUD_MASK_DAC_L_DA_ZDET_TOUT          ((u32)0x00000003 << 2)          /*!<R/W 0  Channel L DAC path zero detection time out select 2'b00: 1024*16 samples 2'b01: 1024*32 samples 2'b10: 1024*64 samples 2'b11: 64 samples */
#define AUD_DAC_L_DA_ZDET_TOUT(x)            (((u32)((x) & 0x00000003) << 2))
#define AUD_GET_DAC_L_DA_ZDET_TOUT(x)        ((u32)(((x >> 2) & 0x00000003)))
#define AUD_BIT_DAC_L_DA_MUTE                ((u32)0x00000001 << 4)          /*!<R/W 0  Channel L DAC path dvol mute enable 1'b0: unmute 1'b1: mute */
#define AUD_BIT_DAC_L_DAHPF_EN               ((u32)0x00000001 << 5)          /*!<R/W 0  Channel L DAC path Narrow-band 1st HPF enable control 0: Disable 1: Enable */
#define AUD_MASK_DAC_L_DA_DITHER_SEL         ((u32)0x00000003 << 6)          /*!<R/W 0  Channel L DAC path dither select 2'b00: Disable 2'b01: LSB 2'b10: LSB+1 2'b11: LSB+2 */
#define AUD_DAC_L_DA_DITHER_SEL(x)           (((u32)((x) & 0x00000003) << 6))
#define AUD_GET_DAC_L_DA_DITHER_SEL(x)       ((u32)(((x >> 6) & 0x00000003)))
#define AUD_BIT_DAC_L_DMIX_MUTE_DA           ((u32)0x00000001 << 8)          /*!<R/W 0  Channel L DAC path 128fs-domain mixer da path mute enable 1'b0: unmute 1'b1: mute */
#define AUD_BIT_DAC_L_DMIX_MUTE_SIDETONE     ((u32)0x00000001 << 9)          /*!<R/W 0  Channel L DAC path 128fs-domain mixer sidetone path mute enable 1'b0: unmute 1'b1: mute */
#define AUD_BIT_DAC_L_DMIX_MUTE_DC           ((u32)0x00000001 << 10)          /*!<R/W 0  Channel L DAC path 128fs-domain mixer a DC from dac_l_tone_compensation_offset 1'b0: unmute 1'b1: mute */
#define AUD_BIT_DAC_L_MUSIC_MUTE_EN          ((u32)0x00000001 << 11)          /*!<R/W 0  Channel L DAC path music output mute enable 1'b0: unmute 1'b1: mute */
#define AUD_BIT_DAC_L_ANC_MUTE_EN            ((u32)0x00000001 << 12)          /*!<R/W 0  Channel L DAC path ANC output mute enable 1'b0: unmute 1'b1: mute */
#define AUD_BIT_DAC_L_PDM_EN                 ((u32)0x00000001 << 13)          /*!<R/W 1  Channel L DAC path PDM 0: Disable 1: Enable */
#define AUD_BIT_DAC_L_SDM_EXTEND_FB_EN       ((u32)0x00000001 << 14)          /*!<R/W 1  Channel L DAC path feedback extend (for debug purpose; default need turn on) 0: Disable 1: Enable */
#define AUD_BIT_DAC_L_SDM_EF_EN              ((u32)0x00000001 << 15)          /*!<R/W 1  Channel L DAC path 2nd order SDM enable 0: Disable 1: Enable */
#define AUD_MASK_DAC_L_SDM_DITHER_SEL        ((u32)0x00000003 << 16)          /*!<R/W 0  Channel L DAC path SDM dither select 2'b00: Disable 2'b01: LSB 2'b10: LSB+1 2'b11: LSB+2 */
#define AUD_DAC_L_SDM_DITHER_SEL(x)          (((u32)((x) & 0x00000003) << 16))
#define AUD_GET_DAC_L_SDM_DITHER_SEL(x)      ((u32)(((x >> 16) & 0x00000003)))
#define AUD_MASK_DAC_L_DA_FLT_TYPE           ((u32)0x00000003 << 18)          /*!<R/W 0  Channel L DAC path interpolation filter type */
#define AUD_DAC_L_DA_FLT_TYPE(x)             (((u32)((x) & 0x00000003) << 18))
#define AUD_GET_DAC_L_DA_FLT_TYPE(x)         ((u32)(((x >> 18) & 0x00000003)))
#define AUD_BIT_DAC_L_IDWA_BYPASS            ((u32)0x00000001 << 20)          /*!<R/W 0  Channel L DAC path IDWA BYPASS mode 0: Disable 1: Enable (debug purpose) */
#define AUD_BIT_DAC_L_OB_TONE_EN             ((u32)0x00000001 << 21)          /*!<R/W 0  Channel L DAC path Out Band tone enable 0: Disable 1: Enable */
/** @} */

/** @defgroup CODEC_dac_l_control_2
 * @brief DAC Left Channel Control Register 2
 * @{
 **/
#define AUD_MASK_DAC_L_DC_OFFSET             ((u32)0x0000FFFF << 0)          /*!<R/W 0  Channel L DAC path 128fs-domain mixe a DC compensation for spur issue 2's complemet */
#define AUD_DAC_L_DC_OFFSET(x)               (((u32)((x) & 0x0000FFFF) << 0))
#define AUD_GET_DAC_L_DC_OFFSET(x)           ((u32)(((x >> 0) & 0x0000FFFF)))
#define AUD_MASK_DAC_L_OB_FC_SEL             ((u32)0x0000000F << 16)          /*!<R/W 0  Channel L DAC path out Band tone frequency 100kHz * (dac_l_ob_fc_sel+1) */
#define AUD_DAC_L_OB_FC_SEL(x)               (((u32)((x) & 0x0000000F) << 16))
#define AUD_GET_DAC_L_OB_FC_SEL(x)           ((u32)(((x >> 16) & 0x0000000F)))
#define AUD_MASK_DAC_L_OB_GAIN               ((u32)0x0000000F << 20)          /*!<R/W 0x7  Channel L DAC path out Band tone gain -20dB - (6 * dac_l_ob_gain ) */
#define AUD_DAC_L_OB_GAIN(x)                 (((u32)((x) & 0x0000000F) << 20))
#define AUD_GET_DAC_L_OB_GAIN(x)             ((u32)(((x >> 20) & 0x0000000F)))
/** @} */

/** @defgroup CODEC_dac_r_control_0
 * @brief DAC Right Channel Control Register 0
 * @{
 **/
#define AUD_MASK_DAC_R_DA_GAIN               ((u32)0x000000FF << 0)          /*!<R/W 0xaf  Channel R DAC path dvol gain control (0.375dB/step) 8'hAF: 0dB 8'h00: -65.625dB */
#define AUD_DAC_R_DA_GAIN(x)                 (((u32)((x) & 0x000000FF) << 0))
#define AUD_GET_DAC_R_DA_GAIN(x)             ((u32)(((x >> 0) & 0x000000FF)))
#define AUD_MASK_DAC_R_DA_SRC_SEL            ((u32)0x00000003 << 16)          /*!<R/W 0x1  Channel R DAC path input select 0: I2S L channel 1: I2S R channel 2: I2S L+R channel 3: Test tone */
#define AUD_DAC_R_DA_SRC_SEL(x)              (((u32)((x) & 0x00000003) << 16))
#define AUD_GET_DAC_R_DA_SRC_SEL(x)          ((u32)(((x >> 16) & 0x00000003)))
#define AUD_BIT_DAC_R_TEST_TONE_EN           ((u32)0x00000001 << 20)          /*!<R/W 0  Channel R DAC path test tone enable */
#define AUD_MASK_DAC_R_TEST_FC_SEL           ((u32)0x0000007F << 21)          /*!<R/W 0x0  Channel R DAC path test tone frquency: (fs/192)*(tone_fc_sel+1)Hz */
#define AUD_DAC_R_TEST_FC_SEL(x)             (((u32)((x) & 0x0000007F) << 21))
#define AUD_GET_DAC_R_TEST_FC_SEL(x)         ((u32)(((x >> 21) & 0x0000007F)))
#define AUD_MASK_DAC_R_TEST_GAIN_SEL         ((u32)0x0000000F << 28)          /*!<R/W 0  Channel R DAC path test tone gain control 0 -6.02*(gain_sel)dB */
#define AUD_DAC_R_TEST_GAIN_SEL(x)           (((u32)((x) & 0x0000000F) << 28))
#define AUD_GET_DAC_R_TEST_GAIN_SEL(x)       ((u32)(((x >> 28) & 0x0000000F)))
/** @} */

/** @defgroup CODEC_dac_r_control_1
 * @brief DAC Right Channel Control Register 1
 * @{
 **/
#define AUD_MASK_DAC_R_DA_ZDET_FUNC          ((u32)0x00000003 << 0)          /*!<R/W 0x2  Channel R DAC path zero detection function select 2'b00: immediate change 2'b01: zero detection & immediate change 2'b10: zdet & step 2'b11: zdet & timeout step */
#define AUD_DAC_R_DA_ZDET_FUNC(x)            (((u32)((x) & 0x00000003) << 0))
#define AUD_GET_DAC_R_DA_ZDET_FUNC(x)        ((u32)(((x >> 0) & 0x00000003)))
#define AUD_MASK_DAC_R_DA_ZDET_TOUT          ((u32)0x00000003 << 2)          /*!<R/W 0  Channel R DAC path zero detection time out select 2'b00: 1024*16 samples 2'b01: 1024*32 samples 2'b10: 1024*64 samples 2'b11: 64 samples */
#define AUD_DAC_R_DA_ZDET_TOUT(x)            (((u32)((x) & 0x00000003) << 2))
#define AUD_GET_DAC_R_DA_ZDET_TOUT(x)        ((u32)(((x >> 2) & 0x00000003)))
#define AUD_BIT_DAC_R_DA_MUTE                ((u32)0x00000001 << 4)          /*!<R/W 0  Channel R DAC path dvol mute enable 1'b0: unmute 1'b1: mute */
#define AUD_BIT_DAC_R_DAHPF_EN               ((u32)0x00000001 << 5)          /*!<R/W 0  Channel R DAC path Narrow-band 1st HPF enable control 0: Disable 1: Enable */
#define AUD_MASK_DAC_R_DA_DITHER_SEL         ((u32)0x00000003 << 6)          /*!<R/W 0  Channel R DAC path dither select 2'b00: Disable 2'b01: LSB 2'b10: LSB+1 2'b11: LSB+2 */
#define AUD_DAC_R_DA_DITHER_SEL(x)           (((u32)((x) & 0x00000003) << 6))
#define AUD_GET_DAC_R_DA_DITHER_SEL(x)       ((u32)(((x >> 6) & 0x00000003)))
#define AUD_BIT_DAC_R_DMIX_MUTE_DA           ((u32)0x00000001 << 8)          /*!<R/W 0  Channel R DAC path 128fs-domain mixer da path mute enable 1'b0: unmute 1'b1: mute */
#define AUD_BIT_DAC_R_DMIX_MUTE_SIDETONE     ((u32)0x00000001 << 9)          /*!<R/W 0  Channel R DAC path 128fs-domain mixer sidetone path mute enable 1'b0: unmute 1'b1: mute */
#define AUD_BIT_DAC_R_DMIX_MUTE_DC           ((u32)0x00000001 << 10)          /*!<R/W 0  Channel R DAC path 128fs-domain mixer a DC from dac_r_tone_compensation_offset 1'b0: unmute 1'b1: mute */
#define AUD_BIT_DAC_R_MUSIC_MUTE_EN          ((u32)0x00000001 << 11)          /*!<R/W 0  Channel R DAC path music output mute enable 1'b0: unmute 1'b1: mute */
#define AUD_BIT_DAC_R_ANC_MUTE_EN            ((u32)0x00000001 << 12)          /*!<R/W 0  Channel R DAC path ANC output mute enable 1'b0: unmute 1'b1: mute */
#define AUD_BIT_DAC_R_PDM_EN                 ((u32)0x00000001 << 13)          /*!<R/W 1  Channel R DAC path PDM 0: Disable 1: Enable */
#define AUD_BIT_DAC_R_SDM_EXTEND_FB_EN       ((u32)0x00000001 << 14)          /*!<R/W 1  Channel R DAC path feedback extend (for debug purpose; default need turn on) 0: Disable 1: Enable */
#define AUD_BIT_DAC_R_SDM_EF_EN              ((u32)0x00000001 << 15)          /*!<R/W 1  Channel R DAC path 2nd order SDM enable 0: Disable 1: Enable */
#define AUD_MASK_DAC_R_SDM_DITHER_SEL        ((u32)0x00000003 << 16)          /*!<R/W 0  Channel R DAC path SDM dither select 2'b00: Disable 2'b01: LSB 2'b10: LSB+1 2'b11: LSB+2 */
#define AUD_DAC_R_SDM_DITHER_SEL(x)          (((u32)((x) & 0x00000003) << 16))
#define AUD_GET_DAC_R_SDM_DITHER_SEL(x)      ((u32)(((x >> 16) & 0x00000003)))
#define AUD_MASK_DAC_R_DA_FLT_TYPE           ((u32)0x00000003 << 18)          /*!<R/W 0  Channel R DAC path interpolation filter type */
#define AUD_DAC_R_DA_FLT_TYPE(x)             (((u32)((x) & 0x00000003) << 18))
#define AUD_GET_DAC_R_DA_FLT_TYPE(x)         ((u32)(((x >> 18) & 0x00000003)))
#define AUD_BIT_DAC_R_IDWA_BYPASS            ((u32)0x00000001 << 20)          /*!<R/W 0  Channel R DAC path IDWA BYPASS mode 0: Disable 1: Enable (debug purpose) */
#define AUD_BIT_DAC_R_OB_TONE_EN             ((u32)0x00000001 << 21)          /*!<R/W 0  Channel R DAC path Out Band tone enable 0: Disable 1: Enable */
/** @} */

/** @defgroup CODEC_dac_r_control_2
 * @brief DAC Right Channel Control Register 2
 * @{
 **/
#define AUD_MASK_DAC_R_DC_OFFSET             ((u32)0x0000FFFF << 0)          /*!<R/W 0  Channel R DAC path 128fs-domain mixe a DC compensation for spur issue 2's complemet */
#define AUD_DAC_R_DC_OFFSET(x)               (((u32)((x) & 0x0000FFFF) << 0))
#define AUD_GET_DAC_R_DC_OFFSET(x)           ((u32)(((x >> 0) & 0x0000FFFF)))
#define AUD_MASK_DAC_R_OB_FC_SEL             ((u32)0x0000000F << 16)          /*!<R/W 0  Channel R DAC path out Band tone frequency 100kHz * (dac_l_ob_fc_sel+1) */
#define AUD_DAC_R_OB_FC_SEL(x)               (((u32)((x) & 0x0000000F) << 16))
#define AUD_GET_DAC_R_OB_FC_SEL(x)           ((u32)(((x >> 16) & 0x0000000F)))
#define AUD_MASK_DAC_R_OB_GAIN               ((u32)0x0000000F << 20)          /*!<R/W 0x7  Channel R DAC path out Band tone gain -20dB - (6 * dac_l_ob_gain ) */
#define AUD_DAC_R_OB_GAIN(x)                 (((u32)((x) & 0x0000000F) << 20))
#define AUD_GET_DAC_R_OB_GAIN(x)             ((u32)(((x >> 20) & 0x0000000F)))
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

/** @defgroup CODEC_dac_r_silence_control
 * @brief DAC Right Channel Silence Control Register
 * @{
 **/
#define AUD_BIT_DAC_R_SILENCE_DET_EN         ((u32)0x00000001 << 0)          /*!<R/W 0  Channel R DAC path silence detection enable 0: Disable 1: Enable */
#define AUD_MASK_DAC_R_SILENCE_LEVEL_SEL     ((u32)0x00000007 << 1)          /*!<R/W 1  Channel R DAC path silence detection threshold 3'b000: -78db 3'b001: -84db 3'b010: -90db 3'b011: -102db 3'b100: -108db 3'b101: -114db 3'b110: -126db 3'b111: -132db */
#define AUD_DAC_R_SILENCE_LEVEL_SEL(x)       (((u32)((x) & 0x00000007) << 1))
#define AUD_GET_DAC_R_SILENCE_LEVEL_SEL(x)   ((u32)(((x >> 1) & 0x00000007)))
#define AUD_MASK_DAC_R_SILENCE_DEBOUNCE_SEL  ((u32)0x00000007 << 4)          /*!<R/W 0x3  Channel R DAC path silence detection debounce (48K) 3'b000: 80ms 3'b001: 160ms 3'b010: 320ms 3'b011: 640ms 3'b100: 1.28s 3'b101: 2.56s 3'b110: 5.12s 3'b111: 0.16ms */
#define AUD_DAC_R_SILENCE_DEBOUNCE_SEL(x)    (((u32)((x) & 0x00000007) << 4))
#define AUD_GET_DAC_R_SILENCE_DEBOUNCE_SEL(x) ((u32)(((x >> 4) & 0x00000007)))
/** @} */

/** @defgroup CODEC_dac_l_eq_ctrl
 * @brief DAC Left Channel EQ Control Register
 * @{
 **/
#define AUD_BIT_DAC_L_BIQUAD_EN_0            ((u32)0x00000001 << 0)          /*!<R/W 0  DAC Lch EQ 0-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_DAC_L_BIQUAD_EN_1            ((u32)0x00000001 << 1)          /*!<R/W 0  DAC Lch EQ 1-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_DAC_L_BIQUAD_EN_2            ((u32)0x00000001 << 2)          /*!<R/W 0  DAC Lch EQ 2-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_DAC_L_BIQUAD_EN_3            ((u32)0x00000001 << 3)          /*!<R/W 0  DAC Lch EQ 3-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_DAC_L_BIQUAD_EN_4            ((u32)0x00000001 << 4)          /*!<R/W 0  DAC Lch EQ 4-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_DAC_L_BIQUAD_EN_5            ((u32)0x00000001 << 5)          /*!<R/W 0  DAC Lch EQ 5-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_DAC_L_BIQUAD_EN_6            ((u32)0x00000001 << 6)          /*!<R/W 0  DAC Lch EQ 6-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_DAC_L_BIQUAD_EN_7            ((u32)0x00000001 << 7)          /*!<R/W 0  DAC Lch EQ 7-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_DAC_L_BIQUAD_EN_8            ((u32)0x00000001 << 8)          /*!<R/W 0  DAC Lch EQ 8-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_DAC_L_BIQUAD_EN_9            ((u32)0x00000001 << 9)          /*!<R/W 0  DAC Lch EQ 9-band biquad enable 0: Disable 1: Enable */
/** @} */

/** @defgroup CODEC_codec_biquad_h0_x
 * @brief DAC Left Channel EQ x Band Biquad H0 Register
 * @{
 **/
#define AUD_MASK_DAC_L_BIQUAD_H0_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0x2000000  DAC Lch EQ x-band coef. h0 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_DAC_L_BIQUAD_H0_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_DAC_L_BIQUAD_H0_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_codec_biquad_b1_x
 * @brief DAC Left Channel EQ x Band Biquad B1 Register
 * @{
 **/
#define AUD_MASK_DAC_L_BIQUAD_B1_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  DAC Lch EQ x-band coef. b1 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_DAC_L_BIQUAD_B1_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_DAC_L_BIQUAD_B1_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_codec_biquad_b2_x
 * @brief DAC Left Channel EQ x Band Biquad B2 Register
 * @{
 **/
#define AUD_MASK_DAC_L_BIQUAD_B2_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  DAC Lch EQ x-band coef. b2 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_DAC_L_BIQUAD_B2_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_DAC_L_BIQUAD_B2_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_codec_biquad_a1_x
 * @brief DAC Left Channel EQ x Band Biquad A1 Register
 * @{
 **/
#define AUD_MASK_DAC_L_BIQUAD_A1_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  DAC Lch EQ x-band coef. a1 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_DAC_L_BIQUAD_A1_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_DAC_L_BIQUAD_A1_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_codec_biquad_a2_x
 * @brief DAC Left Channel EQ x Band Biquad A2 Register
 * @{
 **/
#define AUD_MASK_DAC_L_BIQUAD_A2_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  DAC Lch EQ x-band coef. a2 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_DAC_L_BIQUAD_A2_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_DAC_L_BIQUAD_A2_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_dac_r_eq_ctrl
 * @brief DAC Right Channel EQ Control Register
 * @{
 **/
#define AUD_BIT_DAC_R_BIQUAD_EN_0            ((u32)0x00000001 << 0)          /*!<R/W 0  DAC Rch EQ 0-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_DAC_R_BIQUAD_EN_1            ((u32)0x00000001 << 1)          /*!<R/W 0  DAC Rch EQ 1-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_DAC_R_BIQUAD_EN_2            ((u32)0x00000001 << 2)          /*!<R/W 0  DAC Rch EQ 2-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_DAC_R_BIQUAD_EN_3            ((u32)0x00000001 << 3)          /*!<R/W 0  DAC Rch EQ 3-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_DAC_R_BIQUAD_EN_4            ((u32)0x00000001 << 4)          /*!<R/W 0  DAC Rch EQ 4-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_DAC_R_BIQUAD_EN_5            ((u32)0x00000001 << 5)          /*!<R/W 0  DAC Rch EQ 5-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_DAC_R_BIQUAD_EN_6            ((u32)0x00000001 << 6)          /*!<R/W 0  DAC Rch EQ 6-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_DAC_R_BIQUAD_EN_7            ((u32)0x00000001 << 7)          /*!<R/W 0  DAC Rch EQ 7-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_DAC_R_BIQUAD_EN_8            ((u32)0x00000001 << 8)          /*!<R/W 0  DAC Rch EQ 8-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_DAC_R_BIQUAD_EN_9            ((u32)0x00000001 << 9)          /*!<R/W 0  DAC Rch EQ 9-band biquad enable 0: Disable 1: Enable */
/** @} */

/** @defgroup CODEC_dac_r_biquad_h0_x
 * @brief DAC Right Channel EQ x Band Biquad H0 Register
 * @{
 **/
#define AUD_MASK_DAC_R_BIQUAD_H0_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0x2000000  DAC Rch EQ x-band coef. h0 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_DAC_R_BIQUAD_H0_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_DAC_R_BIQUAD_H0_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_dac_r_biquad_b1_x
 * @brief DAC Right Channel EQ x Band Biquad B1 Register
 * @{
 **/
#define AUD_MASK_DAC_R_BIQUAD_B1_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  DAC Rch EQ x-band coef. b1 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_DAC_R_BIQUAD_B1_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_DAC_R_BIQUAD_B1_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_dac_r_biquad_b2_x
 * @brief DAC Right Channel EQ x Band Biquad B2 Register
 * @{
 **/
#define AUD_MASK_DAC_R_BIQUAD_B2_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  DAC Rch EQ x-band coef. b2 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_DAC_R_BIQUAD_B2_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_DAC_R_BIQUAD_B2_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_dac_r_biquad_a1_x
 * @brief DAC Right Channel EQ x Band Biquad A1 Register
 * @{
 **/
#define AUD_MASK_DAC_R_BIQUAD_A1_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  DAC Rch EQ x-band coef. a1 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_DAC_R_BIQUAD_A1_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_DAC_R_BIQUAD_A1_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_dac_r_biquad_a2_x
 * @brief DAC Right Channel EQ x Band Biquad A2 Register
 * @{
 **/
#define AUD_MASK_DAC_R_BIQUAD_A2_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  DAC Rch EQ x-band coef. a2 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_DAC_R_BIQUAD_A2_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_DAC_R_BIQUAD_A2_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_0_eq_ctrl
 * @brief ADC Channel 0 EQ Control Register
 * @{
 **/
#define AUD_BIT_ADC_0_BIQUAD_EN_0            ((u32)0x00000001 << 0)          /*!<R/W 0  ADC channel 0 EQ 0-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_0_BIQUAD_EN_1            ((u32)0x00000001 << 1)          /*!<R/W 0  ADC channel 0 EQ 1-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_0_BIQUAD_EN_2            ((u32)0x00000001 << 2)          /*!<R/W 0  ADC channel 0 EQ 2-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_0_BIQUAD_EN_3            ((u32)0x00000001 << 3)          /*!<R/W 0  ADC channel 0 EQ 3-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_0_BIQUAD_EN_4            ((u32)0x00000001 << 4)          /*!<R/W 0  ADC channel 0 EQ 4-band biquad enable 0: Disable 1: Enable */
/** @} */

/** @defgroup CODEC_adc_0_biquad_h0_x
 * @brief ADC Channel 0 EQ x Band Biquad H0 Register
 * @{
 **/
#define AUD_MASK_ADC_0_BIQUAD_H0_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0x2000000  ADC channel 0 EQ x-band coef. h0 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_0_BIQUAD_H0_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_0_BIQUAD_H0_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_0_biquad_b1_x
 * @brief ADC Channel 0 EQ x Band Biquad B1 Register
 * @{
 **/
#define AUD_MASK_ADC_0_BIQUAD_B1_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 0 EQ x-band coef. b1 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_0_BIQUAD_B1_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_0_BIQUAD_B1_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_0_biquad_b2_x
 * @brief ADC Channel 0 EQ x Band Biquad B2 Register
 * @{
 **/
#define AUD_MASK_ADC_0_BIQUAD_B2_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 0 EQ x-band coef. b2 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_0_BIQUAD_B2_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_0_BIQUAD_B2_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_0_biquad_a1_x
 * @brief ADC Channel 0 EQ x Band Biquad A1 Register
 * @{
 **/
#define AUD_MASK_ADC_0_BIQUAD_A1_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 0 EQ x-band coef. a1 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_0_BIQUAD_A1_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_0_BIQUAD_A1_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_0_biquad_a2_x
 * @brief ADC Channel 0 EQ x Band Biquad A2 Register
 * @{
 **/
#define AUD_MASK_ADC_0_BIQUAD_A2_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 0 EQ x-band coef. a2 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_0_BIQUAD_A2_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_0_BIQUAD_A2_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_1_eq_ctrl
 * @brief ADC Channel 1 EQ Control Register
 * @{
 **/
#define AUD_BIT_ADC_1_BIQUAD_EN_0            ((u32)0x00000001 << 0)          /*!<R/W 0  ADC channel 1 EQ 0-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_1_BIQUAD_EN_1            ((u32)0x00000001 << 1)          /*!<R/W 0  ADC channel 1 EQ 1-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_1_BIQUAD_EN_2            ((u32)0x00000001 << 2)          /*!<R/W 0  ADC channel 1 EQ 2-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_1_BIQUAD_EN_3            ((u32)0x00000001 << 3)          /*!<R/W 0  ADC channel 1 EQ 3-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_1_BIQUAD_EN_4            ((u32)0x00000001 << 4)          /*!<R/W 0  ADC channel 1 EQ 4-band biquad enable 0: Disable 1: Enable */
/** @} */

/** @defgroup CODEC_adc_1_biquad_h0_x
 * @brief ADC Channel 1 EQ x Band Biquad H0 Register
 * @{
 **/
#define AUD_MASK_ADC_1_BIQUAD_H0_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0x2000000  ADC channel 1 EQ x-band coef. h0 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_1_BIQUAD_H0_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_H0_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_1_biquad_b1_x
 * @brief ADC Channel 1 EQ x Band Biquad B1 Register
 * @{
 **/
#define AUD_MASK_ADC_1_BIQUAD_B1_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 1 EQ x-band coef. b1 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_1_BIQUAD_B1_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_B1_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_1_biquad_b2_x
 * @brief ADC Channel 1 EQ x Band Biquad B2 Register
 * @{
 **/
#define AUD_MASK_ADC_1_BIQUAD_B2_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 1 EQ x-band coef. b2 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_1_BIQUAD_B2_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_B2_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_1_biquad_a1_x
 * @brief ADC Channel 1 EQ x Band Biquad A1 Register
 * @{
 **/
#define AUD_MASK_ADC_1_BIQUAD_A1_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 1 EQ x-band coef. a1 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_1_BIQUAD_A1_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_A1_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_1_biquad_a2_x
 * @brief ADC Channel 1 EQ x Band Biquad A2 Register
 * @{
 **/
#define AUD_MASK_ADC_1_BIQUAD_A2_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 1 EQ x-band coef. a2 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_1_BIQUAD_A2_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_A2_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_2_eq_ctrl
 * @brief ADC Channel 2 EQ Control Register
 * @{
 **/
#define AUD_BIT_ADC_2_BIQUAD_EN_0            ((u32)0x00000001 << 0)          /*!<R/W 0  ADC channel 2 EQ 0-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_2_BIQUAD_EN_1            ((u32)0x00000001 << 1)          /*!<R/W 0  ADC channel 2 EQ 1-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_2_BIQUAD_EN_2            ((u32)0x00000001 << 2)          /*!<R/W 0  ADC channel 2 EQ 2-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_2_BIQUAD_EN_3            ((u32)0x00000001 << 3)          /*!<R/W 0  ADC channel 2 EQ 3-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_2_BIQUAD_EN_4            ((u32)0x00000001 << 4)          /*!<R/W 0  ADC channel 2 EQ 4-band biquad enable 0: Disable 1: Enable */
/** @} */

/** @defgroup CODEC_adc_2_biquad_h0_x
 * @brief ADC Channel 2 EQ x Band Biquad H0 Register
 * @{
 **/
#define AUD_MASK_ADC_2_BIQUAD_H0_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0x2000000  ADC channel 2 EQ x-band coef. h0 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_2_BIQUAD_H0_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_2_BIQUAD_H0_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_2_biquad_b1_x
 * @brief ADC Channel 2 EQ x Band Biquad B1 Register
 * @{
 **/
#define AUD_MASK_ADC_2_BIQUAD_B1_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 2 EQ x-band coef. b1 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_2_BIQUAD_B1_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_2_BIQUAD_B1_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_2_biquad_b2_x
 * @brief ADC Channel 2 EQ x Band Biquad B2 Register
 * @{
 **/
#define AUD_MASK_ADC_2_BIQUAD_B2_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 2 EQ x-band coef. b2 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_2_BIQUAD_B2_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_2_BIQUAD_B2_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_2_biquad_a1_x
 * @brief ADC Channel 2 EQ x Band Biquad A1 Register
 * @{
 **/
#define AUD_MASK_ADC_2_BIQUAD_A1_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 2 EQ x-band coef. a1 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_2_BIQUAD_A1_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_2_BIQUAD_A1_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_2_biquad_a2_x
 * @brief ADC Channel 2 EQ x Band Biquad A2 Register
 * @{
 **/
#define AUD_MASK_ADC_2_BIQUAD_A2_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 2 EQ x-band coef. a2 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_2_BIQUAD_A2_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_2_BIQUAD_A2_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_3_eq_ctrl
 * @brief ADC Channel 3 EQ Control Register
 * @{
 **/
#define AUD_BIT_ADC_3_BIQUAD_EN_0            ((u32)0x00000001 << 0)          /*!<R/W 0  ADC channel 3 EQ 0-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_3_BIQUAD_EN_1            ((u32)0x00000001 << 1)          /*!<R/W 0  ADC channel 3 EQ 1-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_3_BIQUAD_EN_2            ((u32)0x00000001 << 2)          /*!<R/W 0  ADC channel 3 EQ 2-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_3_BIQUAD_EN_3            ((u32)0x00000001 << 3)          /*!<R/W 0  ADC channel 3 EQ 3-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_3_BIQUAD_EN_4            ((u32)0x00000001 << 4)          /*!<R/W 0  ADC channel 3 EQ 4-band biquad enable 0: Disable 1: Enable */
/** @} */

/** @defgroup CODEC_adc_3_biquad_h0_x
 * @brief ADC Channel 3 EQ x Band Biquad H0 Register
 * @{
 **/
#define AUD_MASK_ADC_3_BIQUAD_H0_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0x2000000  ADC channel 3 EQ x-band coef. h0 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_3_BIQUAD_H0_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_3_BIQUAD_H0_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_3_biquad_b1_x
 * @brief ADC Channel 3 EQ x Band Biquad B1 Register
 * @{
 **/
#define AUD_MASK_ADC_3_BIQUAD_B1_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 3 EQ x-band coef. b1 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_3_BIQUAD_B1_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_3_BIQUAD_B1_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_3_biquad_b2_x
 * @brief ADC Channel 3 EQ x Band Biquad B2 Register
 * @{
 **/
#define AUD_MASK_ADC_3_BIQUAD_B2_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 3 EQ x-band coef. b2 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_3_BIQUAD_B2_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_3_BIQUAD_B2_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_3_biquad_a1_x
 * @brief ADC Channel 3 EQ x Band Biquad A1 Register
 * @{
 **/
#define AUD_MASK_ADC_3_BIQUAD_A1_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 3 EQ x-band coef. a1 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_3_BIQUAD_A1_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_3_BIQUAD_A1_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_3_biquad_a2_x
 * @brief ADC Channel 3 EQ x Band Biquad A2 Register
 * @{
 **/
#define AUD_MASK_ADC_3_BIQUAD_A2_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 3 EQ x-band coef. a2 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_3_BIQUAD_A2_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_3_BIQUAD_A2_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_4_eq_ctrl
 * @brief ADC Channel 4 EQ Control Register
 * @{
 **/
#define AUD_BIT_ADC_4_BIQUAD_EN_0            ((u32)0x00000001 << 0)          /*!<R/W 0  ADC channel 4 EQ 0-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_4_BIQUAD_EN_1            ((u32)0x00000001 << 1)          /*!<R/W 0  ADC channel 4 EQ 1-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_4_BIQUAD_EN_2            ((u32)0x00000001 << 2)          /*!<R/W 0  ADC channel 4 EQ 2-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_4_BIQUAD_EN_3            ((u32)0x00000001 << 3)          /*!<R/W 0  ADC channel 4 EQ 3-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_4_BIQUAD_EN_4            ((u32)0x00000001 << 4)          /*!<R/W 0  ADC channel 4 EQ 4-band biquad enable 0: Disable 1: Enable */
/** @} */

/** @defgroup CODEC_adc_4_biquad_h0_x
 * @brief ADC Channel 4 EQ x Band Biquad H0 Register
 * @{
 **/
#define AUD_MASK_ADC_4_BIQUAD_H0_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0x2000000  ADC channel 4 EQ x-band coef. h0 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_4_BIQUAD_H0_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_4_BIQUAD_H0_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_4_biquad_b1_x
 * @brief ADC Channel 4 EQ x Band Biquad B1 Register
 * @{
 **/
#define AUD_MASK_ADC_4_BIQUAD_B1_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 4 EQ x-band coef. b1 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_4_BIQUAD_B1_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_4_BIQUAD_B1_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_4_biquad_b2_x
 * @brief ADC Channel 4 EQ x Band Biquad B2 Register
 * @{
 **/
#define AUD_MASK_ADC_4_BIQUAD_B2_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 4 EQ x-band coef. b2 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_4_BIQUAD_B2_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_4_BIQUAD_B2_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_4_biquad_a1_x
 * @brief ADC Channel 4 EQ x Band Biquad A1 Register
 * @{
 **/
#define AUD_MASK_ADC_4_BIQUAD_A1_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 4 EQ x-band coef. a1 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_4_BIQUAD_A1_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_4_BIQUAD_A1_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_4_biquad_a2_x
 * @brief ADC Channel 4 EQ x Band Biquad A2 Register
 * @{
 **/
#define AUD_MASK_ADC_4_BIQUAD_A2_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 4 EQ x-band coef. a2 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_4_BIQUAD_A2_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_4_BIQUAD_A2_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_5_eq_ctrl
 * @brief ADC Channel 5 EQ Control Register
 * @{
 **/
#define AUD_BIT_ADC_5_BIQUAD_EN_0            ((u32)0x00000001 << 0)          /*!<R/W 0  ADC channel 5 EQ 0-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_5_BIQUAD_EN_1            ((u32)0x00000001 << 1)          /*!<R/W 0  ADC channel 5 EQ 1-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_5_BIQUAD_EN_2            ((u32)0x00000001 << 2)          /*!<R/W 0  ADC channel 5 EQ 2-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_5_BIQUAD_EN_3            ((u32)0x00000001 << 3)          /*!<R/W 0  ADC channel 5 EQ 3-band biquad enable 0: Disable 1: Enable */
#define AUD_BIT_ADC_5_BIQUAD_EN_4            ((u32)0x00000001 << 4)          /*!<R/W 0  ADC channel 5 EQ 4-band biquad enable 0: Disable 1: Enable */
/** @} */

/** @defgroup CODEC_adc_5_biquad_h0_x
 * @brief ADC Channel 5 EQ x Band Biquad H0 Register
 * @{
 **/
#define AUD_MASK_ADC_5_BIQUAD_H0_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0x2000000  ADC channel 5 EQ x-band coef. h0 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_5_BIQUAD_H0_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_5_BIQUAD_H0_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_5_biquad_b1_x
 * @brief ADC Channel 5 EQ x Band Biquad B1 Register
 * @{
 **/
#define AUD_MASK_ADC_5_BIQUAD_B1_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 5 EQ x-band coef. b1 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_5_BIQUAD_B1_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_5_BIQUAD_B1_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_5_biquad_b2_x
 * @brief ADC Channel 5 EQ x Band Biquad B2 Register
 * @{
 **/
#define AUD_MASK_ADC_5_BIQUAD_B2_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 5 EQ x-band coef. b2 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_5_BIQUAD_B2_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_5_BIQUAD_B2_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_5_biquad_a1_x
 * @brief ADC Channel 5 EQ x Band Biquad A1 Register
 * @{
 **/
#define AUD_MASK_ADC_5_BIQUAD_A1_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 5 EQ x-band coef. a1 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_5_BIQUAD_A1_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_5_BIQUAD_A1_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_adc_5_biquad_a2_x
 * @brief ADC Channel 5 EQ x Band Biquad A2 Register
 * @{
 **/
#define AUD_MASK_ADC_5_BIQUAD_A2_x           ((u32)0x1FFFFFFF << 0)          /*!<R/W 0  ADC channel 5 EQ x-band coef. a2 2's complement in 4.25 format, i. e. the range is from -8~7.99. */
#define AUD_ADC_5_BIQUAD_A2_x(x)             (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_5_BIQUAD_A2_x(x)         ((u32)(((x >> 0) & 0x1FFFFFFF)))
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

/** @defgroup CODEC_ana_read
 * @brief ANA Read Register
 * @{
 **/
#define AUD_BIT_MICBIAS_OC                   ((u32)0x00000001 << 0)          /*!<R   The status flag of MICBIAS over-current protection */
/** @} */

/** @defgroup CODEC_adc_x_lpf_rd
 * @brief ADC Channel x LPF RD Register
 * @{
 **/
#define AUD_MASK_ADC_x_LPF_RD                ((u32)0x0007FFFF << 0)          /*!<R   CH x ADC LPF out valus */
#define AUD_ADC_x_LPF_RD(x)                  (((u32)((x) & 0x0007FFFF) << 0))
#define AUD_GET_ADC_x_LPF_RD(x)              ((u32)(((x >> 0) & 0x0007FFFF)))
/** @} */

/** @defgroup CODEC_silence_inform
 * @brief Silence Inform Register
 * @{
 **/
#define AUD_BIT_DAC_L_SILENCE_DET_O          ((u32)0x00000001 << 0)          /*!<R   dac_l_silence data status (result of silence detection) 1'b0: not dac_l_silence data 1'b1: dac_l_silence is detected */
#define AUD_BIT_DAC_L_SILENCE_DET_STATUS     ((u32)0x00000001 << 1)          /*!<R   Ongoing status of dac_l_silence detection 1'b0: dac_l_silence detection is resting (clock is gating) 1'b1: dac_l_silence detection is working */
#define AUD_BIT_DAC_R_SILENCE_DET_O          ((u32)0x00000001 << 2)          /*!<R   dac_r_silence data status (result of silence detection) 1'b0: not dac_r_silence data 1'b1: dac_r_silence is detected */
#define AUD_BIT_DAC_R_SILENCE_DET_STATUS     ((u32)0x00000001 << 3)          /*!<R   Ongoing status of dac_r_silence detection 1'b0: dac_r_silence detection is resting (clock is gating) 1'b1: dac_r_silence detection is working */
#define AUD_BIT_ADC_0_SILENCE_DET_O          ((u32)0x00000001 << 4)          /*!<R   adc_0_silencedata status (result of silence detection) 1'b0: not adc_0_silencedata 1'b1: adc_0_silenceis detected */
#define AUD_BIT_ADC_0_SILENCE_DET_STATUS     ((u32)0x00000001 << 5)          /*!<R   Ongoing status of adc_0_silencedetection 1'b0: adc_0_silencedetection is resting (clock is gating) 1'b1: adc_0_silencedetection is working */
#define AUD_BIT_ADC_1_SILENCE_DET_O          ((u32)0x00000001 << 6)          /*!<R   adc_1_silencedata status (result of silence detection) 1'b0: not adc_1_silencedata 1'b1: adc_1_silenceis detected */
#define AUD_BIT_ADC_1_SILENCE_DET_STATUS     ((u32)0x00000001 << 7)          /*!<R   Ongoing status of adc_1_silencedetection 1'b0: adc_1_silencedetection is resting (clock is gating) 1'b1: adc_1_silencedetection is working */
#define AUD_BIT_ADC_2_SILENCE_DET_O          ((u32)0x00000001 << 8)          /*!<R   adc_2_silencedata status (result of silence detection) 1'b0: not adc_2_silencedata 1'b1: adc_2_silenceis detected */
#define AUD_BIT_ADC_2_SILENCE_DET_STATUS     ((u32)0x00000001 << 9)          /*!<R   Ongoing status of adc_2_silencedetection 1'b0: adc_2_silencedetection is resting (clock is gating) 1'b1: adc_2_silencedetection is working */
#define AUD_BIT_ADC_3_SILENCE_DET_O          ((u32)0x00000001 << 10)          /*!<R   adc_3_silencedata status (result of silence detection) 1'b0: not adc_3_silencedata 1'b1: adc_3_silenceis detected */
#define AUD_BIT_ADC_3_SILENCE_DET_STATUS     ((u32)0x00000001 << 11)          /*!<R   Ongoing status of adc_3_silencedetection 1'b0: adc_3_silencedetection is resting (clock is gating) 1'b1: adc_3_silencedetection is working */
#define AUD_BIT_ADC_4_SILENCE_DET_O          ((u32)0x00000001 << 12)          /*!<R   adc_4_silencedata status (result of silence detection) 1'b0: not adc_4_silencedata 1'b1: adc_4_silenceis detected */
#define AUD_BIT_ADC_4_SILENCE_DET_STATUS     ((u32)0x00000001 << 13)          /*!<R   Ongoing status of adc_4_silencedetection 1'b0: adc_4_silencedetection is resting (clock is gating) 1'b1: adc_4_silencedetection is working */
#define AUD_BIT_ADC_5_SILENCE_DET_O          ((u32)0x00000001 << 14)          /*!<R   adc_5_silencedata status (result of silence detection) 1'b0: not adc_5_silencedata 1'b1: adc_5_silenceis detected */
#define AUD_BIT_ADC_5_SILENCE_DET_STATUS     ((u32)0x00000001 << 15)          /*!<R   Ongoing status of adc_5_silencedetection 1'b0: adc_5_silencedetection is resting (clock is gating) 1'b1: adc_5_silencedetection is working */
#define AUD_BIT_ADC_6_SILENCE_DET_O          ((u32)0x00000001 << 16)          /*!<R   adc_6_silencedata status (result of silence detection) 1'b0: not adc_6_silencedata 1'b1: adc_6_silenceis detected */
#define AUD_BIT_ADC_6_SILENCE_DET_STATUS     ((u32)0x00000001 << 17)          /*!<R   Ongoing status of adc_6_silencedetection 1'b0: adc_6_silencedetection is resting (clock is gating) 1'b1: adc_6_silencedetection is working */
#define AUD_BIT_ADC_7_SILENCE_DET_O          ((u32)0x00000001 << 18)          /*!<R   adc_7_silencedata status (result of silence detection) 1'b0: not adc_7_silencedata 1'b1: adc_7_silenceis detected */
#define AUD_BIT_ADC_7_SILENCE_DET_STATUS     ((u32)0x00000001 << 19)          /*!<R   Ongoing status of adc_7_silence detection 1'b0: adc_7_silencedetection is resting (clock is gating) 1'b1: adc_7_silencedetection is working */
/** @} */

/** @defgroup CODEC_audio_ro_dummy1
 * @brief Audio RO Dummy1 Register
 * @{
 **/
#define AUD_MASK_AUDIO_RO_DUMMY1             ((u32)0xFFFFFFFF << 0)          /*!<R    */
#define AUD_AUDIO_RO_DUMMY1(x)               (((u32)((x) & 0xFFFFFFFF) << 0))
#define AUD_GET_AUDIO_RO_DUMMY1(x)           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup CODEC_vad_intr_st
 * @brief VAD Interrupt Status Register
 * @{
 **/
#define AUD_MASK_VAD_INTR_ST                 ((u32)0x000000FF << 0)          /*!<RW1C   Status of new vad interrupts and clear them by writing 1 */
#define AUD_VAD_INTR_ST(x)                   (((u32)((x) & 0x000000FF) << 0))
#define AUD_GET_VAD_INTR_ST(x)               ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup CODEC_vad_det_flag
 * @brief VAD Detection Flag Register
 * @{
 **/
#define AUD_BIT_VAD_DET_FLAG                 ((u32)0x00000001 << 0)          /*!<R   Vad detection flag 1 */
/** @} */
/** @} */

/* Exported types --------------------------------------------------------*/

/** @defgroup AUDIO_CODEC_Exported_Types AUDIO_CODEC Exported Types
  * @{
*****************************************************************************/

/** @brief AUDIO_CODEC port
*****************************************************************************/
typedef struct {

	__IO uint32_t CODEC_I2S_x_CONTROL;                    /*!< I2S x CONTROL REGISTER */
	__IO uint32_t CODEC_I2S_x_CONTROL_1;                  /*!< I2S x CONTROL REGISTER 1 Register */
} CODEC_I2S_SRC_CTRL_TypeDef;


/** @brief AUDIO_CODEC port
*****************************************************************************/
typedef struct {

	__IO uint32_t CODEC_ADC_x_CONTROL_0;                  /*!< ADC CHANNEL x CONTROL REGISTER 0 Register */
	__IO uint32_t CODEC_ADC_x_CONTROL_1;                  /*!< ADC CHANNEL x CONTROL REGISTER 1 Register */
} CODEC_ADC_CH_CTRL_TypeDef;


/** @brief AUDIO_CODEC port
*****************************************************************************/
typedef struct {

	__IO uint32_t CODEC_BIQUAD_H0_x;                      /*!< DAC LEFT CHANNEL EQ x BAND BIQUAD H0 REGISTER */
	__IO uint32_t CODEC_BIQUAD_B1_x;                      /*!< DAC LEFT CHANNEL EQ x BAND BIQUAD B1 REGISTER */
	__IO uint32_t CODEC_BIQUAD_B2_x;                      /*!< DAC LEFT CHANNEL EQ x BAND BIQUAD B2 REGISTER */
	__IO uint32_t CODEC_BIQUAD_A1_x;                      /*!< DAC LEFT CHANNEL EQ x BAND BIQUAD A1 REGISTER */
	__IO uint32_t CODEC_BIQUAD_A2_x;                      /*!< DAC LEFT CHANNEL EQ x BAND BIQUAD A2 REGISTER */
} CODEC_EQ_BAND_TypeDef;





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
	__IO uint32_t CODEC_ASRC_CONTROL_0;                   /*!<  Register,  Address offset: 0x24 */
	__IO uint32_t CODEC_ASRC_CONTROL_1;                   /*!< ASRC CONTROL REGISTER 1 Register,  Address offset: 0x28 */
	__IO uint32_t CODEC_ASRC_CONTROL_2;                   /*!< ASRC CONTROL REGISTER 2 Register,  Address offset: 0x2C */
	__IO uint32_t CODEC_ASRC_CONTROL_3;                   /*!< ASRC CONTROL REGISTER 3 Register,  Address offset: 0x30 */
	__IO uint32_t CODEC_ASRC_CONTROL_4;                   /*!< ASRC CONTROL REGISTER 4 Register,  Address offset: 0x34 */
	__IO uint32_t CODEC_ASRC_CONTROL_5;                   /*!< ASRC CONTROL REGISTER 5 Register,  Address offset: 0x38 */
	CODEC_I2S_SRC_CTRL_TypeDef CODEC_I2S_SRC_CTRL[2];     /*!< AUDIO IP have 2 ports,  Address offset: 0x3C-0x48 */
	__IO uint32_t CODEC_I2S_AD_SEL_CONTROL;               /*!< I2S ADC SELECTION CONTROL REGISTER,  Address offset: 0x4C */
	CODEC_ADC_CH_CTRL_TypeDef CODEC_ADC_CH_CTRL[8];       /*!< AUDIO IP have 8 ports,  Address offset: 0x50-0x8C */
	__IO uint32_t CODEC_DAC_L_CONTROL_0;                  /*!< DAC LEFT CHANNEL CONTROL REGISTER 0 Register,  Address offset: 0x90 */
	__IO uint32_t CODEC_DAC_L_CONTROL_1;                  /*!< DAC LEFT CHANNEL CONTROL REGISTER 1 Register,  Address offset: 0x94 */
	__IO uint32_t CODEC_DAC_L_CONTROL_2;                  /*!< DAC LEFT CHANNEL CONTROL REGISTER 2 Register,  Address offset: 0x98 */
	__IO uint32_t CODEC_DAC_R_CONTROL_0;                  /*!< DAC RIGHT CHANNEL CONTROL REGISTER 0 Register,  Address offset: 0x9C */
	__IO uint32_t CODEC_DAC_R_CONTROL_1;                  /*!< DAC RIGHT CHANNEL CONTROL REGISTER 1 Register,  Address offset: 0xA0 */
	__IO uint32_t CODEC_DAC_R_CONTROL_2;                  /*!< DAC RIGHT CHANNEL CONTROL REGISTER 2 Register,  Address offset: 0xA4 */
	__IO uint32_t RSVD0[22];                              /*!<  Reserved,  Address offset:0xA8-0xFC */
	__IO uint32_t CODEC_ADC_x_SILENCE_CONTROL[8];         /*!< ADC CHANNEL x SILENCE CONTROL REGISTER,  Address offset: 0x100-0x11C */
	__IO uint32_t CODEC_DAC_L_SILENCE_CONTROL;            /*!< DAC LEFT CHANNEL SILENCE CONTROL REGISTER,  Address offset: 0x120 */
	__IO uint32_t CODEC_DAC_R_SILENCE_CONTROL;            /*!< DAC RIGHT CHANNEL SILENCE CONTROL REGISTER,  Address offset: 0x124 */
	__IO uint32_t RSVD1[54];                              /*!<  Reserved,  Address offset:0x128-0x1FC */
	__IO uint32_t CODEC_DAC_L_EQ_CTRL;                    /*!< DAC LEFT CHANNEL EQ CONTROL REGISTER,  Address offset: 0x200 */
	CODEC_EQ_BAND_TypeDef CODEC_DAC_L_EQ_BAND[10];        /*!< AUDIO IP have 10 ports,  Address offset: 0x204-0x2C8 */
	__IO uint32_t RSVD2[13];                              /*!<  Reserved,  Address offset:0x2CC-0x2FC */
	__IO uint32_t CODEC_DAC_R_EQ_CTRL;                    /*!< DAC RIGHT CHANNEL EQ CONTROL REGISTER,  Address offset: 0x300 */
	CODEC_EQ_BAND_TypeDef CODEC_DAC_R_EQ_BAND[10];        /*!< AUDIO IP have 10 ports,  Address offset: 0x304-0x3C8 */
	__IO uint32_t RSVD3[13];                              /*!<  Reserved,  Address offset:0x3CC-0x3FC */
	__IO uint32_t CODEC_ADC_0_EQ_CTRL;                    /*!< ADC CHANNEL 0 EQ CONTROL REGISTER,  Address offset: 0x400 */
	CODEC_EQ_BAND_TypeDef CODEC_ADC_0_EQ_BAND[5];         /*!< AUDIO IP have 5 ports,  Address offset: 0x404-0x464 */
	__IO uint32_t CODEC_ADC_1_EQ_CTRL;                    /*!< ADC CHANNEL 1 EQ CONTROL REGISTER,  Address offset: 0x468 */
	CODEC_EQ_BAND_TypeDef CODEC_ADC_1_EQ_BAND[5];         /*!< AUDIO IP have 5 ports,  Address offset: 0x46C-0x4CC */
	__IO uint32_t RSVD4[12];                              /*!<  Reserved,  Address offset:0x4D0-0x4FC */
	__IO uint32_t CODEC_ADC_2_EQ_CTRL;                    /*!< ADC CHANNEL 2 EQ CONTROL REGISTER,  Address offset: 0x500 */
	CODEC_EQ_BAND_TypeDef CODEC_ADC_2_EQ_BAND[5];         /*!< AUDIO IP have 5 ports,  Address offset: 0x504-0x564 */
	__IO uint32_t CODEC_ADC_3_EQ_CTRL;                    /*!< ADC CHANNEL 3 EQ CONTROL REGISTER,  Address offset: 0x568 */
	CODEC_EQ_BAND_TypeDef CODEC_ADC_3_EQ_BAND[5];         /*!< AUDIO IP have 5 ports,  Address offset: 0x56C-0x5CC */
	__IO uint32_t RSVD5[12];                              /*!<  Reserved,  Address offset:0x5D0-0x5FC */
	__IO uint32_t CODEC_ADC_4_EQ_CTRL;                    /*!< ADC CHANNEL 4 EQ CONTROL REGISTER,  Address offset: 0x600 */
	CODEC_EQ_BAND_TypeDef CODEC_ADC_4_EQ_BAND[5];         /*!< AUDIO IP have 5 ports,  Address offset: 0x604-0x664 */
	__IO uint32_t CODEC_ADC_5_EQ_CTRL;                    /*!< ADC CHANNEL 5 EQ CONTROL REGISTER,  Address offset: 0x668 */
	CODEC_EQ_BAND_TypeDef CODEC_ADC_5_EQ_BAND[5];         /*!< AUDIO IP have 5 ports,  Address offset: 0x66C-0x6CC */
	__IO uint32_t RSVD6[12];                              /*!<  Reserved,  Address offset:0x6D0-0x6FC */
	__IO uint32_t CODEC_VAD_CTRL;                         /*!< VAD CONTROL REGISTER,  Address offset: 0x700 */
	__IO uint32_t CODEC_VAD_VOICE_LEVEL;                  /*!< VAD VOICE LEVEL REGISTER,  Address offset: 0x704 */
	__IO uint32_t CODEC_VAD_DET_THR;                      /*!< VAD DETECTION THRESHOLD REGISTER,  Address offset: 0x708 */
	__IO uint32_t CODEC_VAD_INTR_EN;                      /*!< VAD INTERRUPT ENABLE REGISTER,  Address offset: 0x70C */
	__IO uint32_t RSVD7[60];                              /*!<  Reserved,  Address offset:0x710-0x7FC */
	__I  uint32_t CODEC_ANA_READ;                         /*!< ANA READ REGISTER,  Address offset: 0x800 */
	__I  uint32_t CODEC_ADC_x_LPF_RD[8];                  /*!< ADC CHANNEL x LPF RD REGISTER,  Address offset: 0x804-0x820 */
	__I  uint32_t CODEC_SILENCE_INFORM;                   /*!< SILENCE INFORM REGISTER,  Address offset: 0x824 */
	__I  uint32_t CODEC_AUDIO_RO_DUMMY1;                  /*!< AUDIO RO DUMMY1 REGISTER,  Address offset: 0x828 */
	__IO uint32_t RSVD8[53];                              /*!<  Reserved,  Address offset:0x82C-0x8FC */
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
 * @brief AUDIO_CODEC I2S Init Structure Definition
 */
typedef struct {

	u32 CODEC_SelI2STxSR;				/*!< Specifies the AUDIO CODEC TX Sample rate
										This parameter can be a value of @ref AUDIO_CODEC_Sample_Rate */

	u32 CODEC_SelI2STxWordLen; 			/*!< Specifies the AUDIO CODEC TX Word length
										This parameter can be a value of @ref AUDIO_CODEC_Word_Length */

	u32 CODEC_SelI2STxCHLen;			/*!< Specifies the AUDIO SPORT TX Channel length
										This parameter can be a value of @ref AUDIO_CODEC_Channel_Length  */

	u32 CODEC_SelI2STxCH;				/*!< Specifies the AUDIO SPORT TX Channel L/R
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

} I2S_InitTypeDef ;


/**
 * @brief AUDIO_CODEC EQ Filter coefficients Structure Definition
 */
typedef struct {
	u32 H0_Q;
	u32 B1_Q;
	u32 B2_Q;
	u32 A1_Q;
	u32 A2_Q;
} CODEC_EQFilterCoef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
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

/** @defgroup AUDIO_CODEC_I2S
  * @{
  */
#define I2S0						    ((u32)0x00000000)
#define I2S1							((u32)0x00000001)

#define IS_CODEC_I2S_SEL(SEL) (((SEL) == I2S0) || \
								((SEL) == I2S1))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_I2S_TDM_Mode
  * @{
  */
#define I2S_NOTDM						    ((u32)0x00000000)
#define I2S_TDM4							((u32)0x00000001)
#define I2S_TDM6							((u32)0x00000002)
#define I2S_TDM8							((u32)0x00000003)

#define IS_CODEC_I2S_TDM_SEL(SEL) (((SEL) == I2S_NOTDM) || \
							((SEL) == I2S_TDM4) || \
							((SEL) == I2S_TDM6) || \
							((SEL) == I2S_TDM8))
/**
  * @}
  */


/** @defgroup AUDIO_CODEC_AMIC
  * @{
  */
#define AMIC1							((u32)0x00000001)
#define AMIC2							((u32)0x00000002)
#define AMIC3							((u32)0x00000003)
#define AMIC4							((u32)0x00000004)
#define AMIC5							((u32)0x00000005)

#define IS_CODEC_AMIC_SEL(SEL) (((SEL) == AMIC1) || \
							((SEL) == AMIC2) || \
							((SEL) == AMIC3) || \
							((SEL) == AMIC4) || \
							((SEL) == AMIC5))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_MICBIAS
  * @{
  */
#define MICBIAS1							((u32)0x00000001)
#define MICBIAS2							((u32)0x00000002)
#define MICBIAS3							((u32)0x00000003)
#define MICBIAS4							((u32)0x00000004)
#define MICBIAS5							((u32)0x00000005)

#define IS_CODEC_MICBIAS_SEL(SEL) (((SEL) == MICBIAS1) || \
							((SEL) == MICBIAS2) || \
							((SEL) == MICBIAS3) || \
							((SEL) == MICBIAS4) || \
							((SEL) == MICBIAS5))
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
#define ADC5							((u32)0x00000005)
#define ADC6							((u32)0x00000006)
#define ADC7							((u32)0x00000007)
#define ADC8							((u32)0x00000008)

#define IS_CODEC_ADC_SEL(SEL) (((SEL) == ADC1) || \
							((SEL) == ADC2) || \
							((SEL) == ADC3) || \
							((SEL) == ADC4) || \
							((SEL) == ADC5) || \
							((SEL) == ADC6) || \
							((SEL) == ADC7) || \
							((SEL) == ADC8))
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
#define ADCHN5							((u32)0x00000005)
#define ADCHN6							((u32)0x00000006)
#define ADCHN7							((u32)0x00000007)
#define ADCHN8							((u32)0x00000008)

#define IS_CODEC_ADCHN_SEL(SEL) (((SEL) == ADCHN1) || \
								((SEL) == ADCHN2) || \
								((SEL) == ADCHN3) || \
								((SEL) == ADCHN4) || \
								((SEL) == ADCHN5) || \
								((SEL) == ADCHN6) || \
								((SEL) == ADCHN7) || \
								((SEL) == ADCHN8))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Power
  * @{
  */
#define NORMALPOWER						((u32)0x00000001)
#define LOWPOWER						((u32)0x00000002)
#define SHUTDOWN						((u32)0x00000003)

#define IS_CODEC_POWER_MODE_SEL(SEL) (((SEL) == NORMALPOWER) || \
							((SEL) == LOWPOWER) || \
							((SEL) == SHUTDOWN))

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
#define DMIC5							((u32)0x00000005)
#define DMIC6							((u32)0x00000006)
#define DMIC7							((u32)0x00000007)
#define DMIC8							((u32)0x00000008)

#define IS_CODEC_DMIC_SEL(SEL) (((SEL) == DMIC1) || \
							((SEL) == DMIC2) || \
							((SEL) == DMIC3) || \
							((SEL) == DMIC4) || \
							((SEL) == DMIC5) || \
							((SEL) == DMIC6) || \
							((SEL) == DMIC7) || \
							((SEL) == DMIC8))
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

#define IS_CODEC_DMIC_CLOCK_SEL(SEL) (((SEL) == DMIC_5M) || \
							((SEL) == DMIC_2P5M) || \
							((SEL) == DMIC_1P25M) || \
							((SEL) == DMIC_625K) || \
							((SEL) == DMIC_312P5K) || \
							((SEL) == DMIC_769P2K))

/**
  * @}
  */

/** @defgroup AUDIO_CODEC_PDM_Clock
  * @{
  */
#define PDM_5M			((u32)0x00000000)
#define PDM_2P5M		((u32)0x00000001)
#define PDM_6P67M		((u32)0x00000002)

#define IS_CODEC_PDM_CLOCK_SEL(SEL) (((SEL) == PDM_5M) || \
							((SEL) == PDM_2P5M) || \
							((SEL) == PDM_6P67M))

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

#define IS_CODEC_MICBST_GAIN_SEL(SEL) (((SEL) == MICBST_GAIN_0DB) || \
							((SEL) == MICBST_GAIN_5DB) || \
							((SEL) == MICBST_GAIN_10DB) || \
							((SEL) == MICBST_GAIN_15DB) || \
							((SEL) == MICBST_GAIN_20DB) || \
							((SEL) == MICBST_GAIN_25DB) || \
							((SEL) == MICBST_GAIN_30DB) || \
							((SEL) == MICBST_GAIN_35DB) || \
							((SEL) == MICBST_GAIN_40DB))

/**
  * @}
  */

/** @defgroup AUDIO_CODEC_I2S_Master_Source
  * @{
  */
#define INTERNAL_SPORT			((u32)0x00000000)
#define EXTERNAL_I2S			((u32)0x00000001)

#define IS_CODEC_I2S_MASTER_SRC(SEL) (((SEL) == INTERNAL_SPORT) || \
								((SEL) == EXTERNAL_I2S))

/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Power_Mode
  * @{
  */
#define POWER_ON			((u32)0x00000000)
#define POWER_DOWN			((u32)0x00000001)

#define IS_CODEC_POWER_SEL(SEL) (((SEL) == POWER_ON) || \
							((SEL) == POWER_DOWN))

/**
  * @}
  */

/** @defgroup AUDIO_CODEC_LintOut_Mode
  * @{
  */
#define SINGLE			((u32)0x00000000)
#define DIFF			((u32)0x00000001)

#define IS_CODEC_LO_MODE(SEL) (((SEL) == SINGLE) || \
							((SEL) == DIFF))

/**
  * @}
  */

/** @defgroup AUDIO_CODEC_ADC_MUTE_Control
  * @{
  */
#define LINEIN				((u32)0x00000001)
#define MICIN				((u32)0x00000002)

#define IS_CODEC_ADC_MUTE_SEL(SEL) (((SEL) == LINEIN) || \
							((SEL) == MICIN))

/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Sample_Rate_Source
  * @{
  */
#define SOURCE0			((u32)0x00000000)
#define SOURCE1			((u32)0x00000001)
#define SOURCE2			((u32)0x00000002)

#define IS_CODEC_SR_SRC(SEL) (((SEL) == SOURCE0) || \
								((SEL) == SOURCE1) || \
								((SEL) == SOURCE2))

/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Mute
  * @{
  */
#define UNMUTE			((u32)0x00000000)
#define MUTE			((u32)0x00000001)

#define IS_CODEC_MUTE_CTR(SEL) (((SEL) == UNMUTE) || \
							((SEL) == MUTE))

/**
  * @}
  */

/** @defgroup AUDIO_CODEC_DAC_Channel
  * @{
  */
#define DAC_L			((u32)0x00000000)
#define DAC_R			((u32)0x00000001)

#define IS_CODEC_DAC_SEL(SEL) (((SEL) == DAC_L) || \
								((SEL) == DAC_R))

/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Vref_Value
  * @{
  */
#define VOL_AUDIO_VREF_1V8_F		0x7DE
#define VOL_AUDIO_VREF_1V8_S		0x7DF
#define AUDIO_CAL_5BIT_VALUE(x)		((u8)(((x) & 0x1F) >> 0))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_PATH
  * @{
  */
#define DACPATH			((u32)0x00000001)
#define ADCPATH			((u32)0x00000002)

#define IS_CODEC_WORK_PATH(SEL) (((SEL) == DACPATH) || \
								((SEL) == ADCPATH))

/**
  * @}
  */

/** @defgroup AUDIO_CODEC_PATH
  * @{
  */

#define I2SL					((u32)0x00000000)
#define I2SR					((u32)0x00000001)
#define I2SLR					((u32)0x00000002)
#define TESTTONE				((u32)0x00000003)
#define IS_CODEC_I2SCHN_SEL(SEL) (((SEL) == I2SL) || \
								((SEL) == I2SR) || \
								((SEL) == I2SLR) || \
								((SEL) == TESTTONE))

/**
  * @}
  */

/** @defgroup AUDIO_CODEC_ADC_Mix_Select
  * @{
  */
#define DMIC			((u32)0x00000001)
#define ANAAD				((u32)0x00000002)

#define IS_CODEC_ADC_MIXMUTE_SEL(SEL) (((SEL) == DMIC) || \
								((SEL) == ANAAD))

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

#define IS_CODEC_ADC_DCM_SEL(SEL) (((SEL) == AUDIO_ADC) || \
								((SEL) == ANC) || \
								((SEL) == MUSIC) || \
								((SEL) == ANCANDMUSIC))

/**
  * @}
  */

/** @defgroup AUDIO_CODEC_DAC_MUTE_Control
  * @{
  */
#define ANALOGIN			((u32)0x00000001)
#define DACIN				((u32)0x00000002)

#define IS_CODEC_DAC_MUTE_SEL(SEL) (((SEL) == ANALOGIN) || \
								((SEL) == DACIN))

/**s
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
#define APP_HPO_OUT		((u32)0x00000005)
#define APP_DAAD_LPBK	((u32)0x00000006)
#define APP_ADDA_LPBK	((u32)0x00000007)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_PAD
  * @{
  */
#define PAD_DACL ((u32)0x00000000)
#define PAD_DACR ((u32)0x00000001)
#define PAD_MIC1 ((u32)0x00000002)
#define PAD_MIC2 ((u32)0x00000003)
#define PAD_MIC3 ((u32)0x00000004)
#define PAD_MIC4 ((u32)0x00000005)
#define PAD_MIC5 ((u32)0x00000006)
#define PAD_MICBIAS1 ((u32)0x00000007)
#define PAD_MICBIAS2 ((u32)0x00000008)
#define PAD_MICBIAS3 ((u32)0x00000009)
#define PAD_MICBIAS4 ((u32)0x0000000A)
#define PAD_MICBIAS5 ((u32)0x0000000B)
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
#define IS_CODEC_ZDET_SEL(SEL) (((SEL) == IME) || \
								((SEL) == ZDET_IME) || \
								((SEL) == ZDET_STEP) || \
								((SEL) == ZDET_TIMEOUT))

/**
  * @}
  */

/** @defgroup AUDIO_CODEC_LineOutOrHPO_Channel
  * @{
  */
#define CHN_L			((u32)0x00000000)
#define CHN_R			((u32)0x00000001)
#define CHN_LR			((u32)0x00000002)
#define IS_CODEC_LOORHO_SEL(SEL) (((SEL) == CHN_L) || \
								((SEL) == CHN_R) || \
								((SEL) == CHN_LR))

/**
  * @}
  */

/** @defgroup AUDIO_CODEC_ADC_EQ_Band
  * @{
  */
#define ADCEQBD0							((u32)0x00000000)
#define ADCEQBD1							((u32)0x00000001)
#define ADCEQBD2							((u32)0x00000002)
#define ADCEQBD3							((u32)0x00000003)
#define ADCEQBD4							((u32)0x00000004)
#define IS_CODEC_ADC_EQ_Band_SEL(SEL) (((SEL) == ADCEQBD0) || \
								((SEL) == ADCEQBD1) || \
								((SEL) == ADCEQBD2) || \
								((SEL) == ADCEQBD3)|| \
								((SEL) == ADCEQBD4))
/**
* @}
*/


/** @defgroup AUDIO_CODEC_DAC_EQ_Band
  * @{
  */
#define DACEQBD0							((u32)0x00000000)
#define DACEQBD1							((u32)0x00000001)
#define DACEQBD2							((u32)0x00000002)
#define DACEQBD3							((u32)0x00000003)
#define DACEQBD4							((u32)0x00000004)
#define DACEQBD5							((u32)0x00000005)
#define DACEQBD6							((u32)0x00000006)
#define DACEQBD7							((u32)0x00000007)
#define DACEQBD8							((u32)0x00000008)
#define DACEQBD9							((u32)0x00000009)

#define IS_CODEC_DAC_EQ_Band_SEL(SEL) (((SEL) == DACEQBD0) || \
								((SEL) == DACEQBD1) || \
								((SEL) == DACEQBD2) || \
								((SEL) == DACEQBD3)|| \
								((SEL) == DACEQBD4) || \
								((SEL) == DACEQBD5) || \
								((SEL) == DACEQBD6)|| \
								((SEL) == DACEQBD7) || \
								((SEL) == DACEQBD8) || \
								((SEL) == DACEQBD9))
/**
* @}
*/

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
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
_LONG_CALL_ void AUDIO_CODEC_EnableADCForMask(u32 ad_chn_mask);
_LONG_CALL_ void AUDIO_CODEC_SetADCVolume(u32 adc_sel, u32 gain);
_LONG_CALL_ void AUDIO_CODEC_SetADCHPF(u32 adc_sel, u32 fc, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCASRC(u32 i2s_sel, u32 adc_sel, u32 sr, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCMute(u32 adc_sel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCMixMute(u32 adc_num, u32 type,  			      u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCANASrc(u32 ad_chn, u32 amic_num);
_LONG_CALL_ void AUDIO_CODEC_SetADCANAFilter(u32 adc_num, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetMicBiasPowerMode(u32 powermode);
_LONG_CALL_ void AUDIO_CODEC_SetMicBstPowerMode(u32 amic_num, u32 powermode);
_LONG_CALL_ void AUDIO_CODEC_SetMicBiasPCUTMode(u32 micbias_num, u32 pcut_mode);
_LONG_CALL_ void AUDIO_CODEC_SetMicBstChnMute(u32 amic_sel, u32 type, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetMicBstInputMode(u32 amic_num, u32 mode);
_LONG_CALL_ void AUDIO_CODEC_SetMicBstGain(u32 amic_sel, u32 gain);
_LONG_CALL_ void AUDIO_CODEC_SetADCDmicFilter(u32 adc_num, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDmicClk(u32 clk, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDmicSrc(u32 ad_chn, u32 dmic_num);
_LONG_CALL_ void AUDIO_CODEC_SetDACSrc(u32 i2s_sel, u32 dac_l_src, u32 dac_r_src);
_LONG_CALL_ void AUDIO_CODEC_SetADCSrc(u32 i2s_sel, u32 adc_num);
_LONG_CALL_ void AUDIO_CODEC_EnableDAC(u32 channel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDACMute(u32 channel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDACVolume(u32 channel, u32 gain);
_LONG_CALL_ void AUDIO_CODEC_SetDACHPF(u32 channel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDACASRC(u32 sr, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDACPowerMode(u32 channel, u32 powermode);
_LONG_CALL_ void AUDIO_CODEC_SetLineOutPowerMode(u32 channel, u32 powermode);
_LONG_CALL_ void AUDIO_CODEC_SetHPOPowerMode(u32 channel, u32 powermode);
_LONG_CALL_ void AUDIO_CODEC_SetLineOutMode(u32 channel, u32 mode);
_LONG_CALL_ void AUDIO_CODEC_SetHPOMode(u32 channel, u32 mode);
_LONG_CALL_ void AUDIO_CODEC_SetLineOutMute(u32 channel, u32 type, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetHPOMute(u32 channel, u32 type, u32 newstate);
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
_LONG_CALL_ void AUDIO_CODEC_EnableADCFifo(u32 ad_chn, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_EnableADCFifoForMask(u32 ad_chn_mask);
_LONG_CALL_ void AUDIO_CODEC_EnableDACFifo(u32 newstate);
_LONG_CALL_ u32 AUDIO_CODEC_GetHPOMode(u32 channel);

/**
  * @}
  */

/* MANUAL_GEN_END */

/** @} */

/** @} */

/** @} */

#endif

