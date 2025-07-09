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
*/
/** @defgroup CODEC_AUDIO_CONTROL_0
 * @brief
 * @{
 */
#define AUD_BIT_AUDIO_IP_EN           ((u32)0x00000001 << 0)            /*!<R/W 0  */
#define AUD_BIT_AUDIO_CONTROL_0_DUMMY ((u32)0x00000001 << 3)            /*!<R/W 0  DUMMY*/
#define AUD_MASK_SYS_CLK_RATE_SEL     ((u32)0x00000003 << 4)            /*!<R/W 0x2  audio_sys_clock_sel*/
#define AUD_SYS_CLK_RATE_SEL(x)       (((u32)((x) & 0x00000003) << 4))
#define AUD_GET_SYS_CLK_RATE_SEL(x)   ((u32)(((x >> 4) & 0x00000003)))
#define AUD_MASK_AUDIO_DBG_SEL        ((u32)0x0000001F << 13)           /*!<R/W 0  Debug probe selcetion*/
#define AUD_AUDIO_DBG_SEL(x)          (((u32)((x) & 0x0000001F) << 13))
#define AUD_GET_AUDIO_DBG_SEL(x)      ((u32)(((x >> 13) & 0x0000001F)))
/** @} */

/** @defgroup CODEC_AUDIO_CONTROL_1
 * @brief
 * @{
 */
#define AUD_BIT_I2S_DATA_RND_EN          ((u32)0x00000001 << 12)           /*!<R/W 1  i2s_data_rnd_en*/
#define AUD_MASK_AUDIO_CONTROL_1_DUMMY   ((u32)0x0000000F << 13)           /*!<R/W 0  DUMMY*/
#define AUD_AUDIO_CONTROL_1_DUMMY(x)     (((u32)((x) & 0x0000000F) << 13))
#define AUD_GET_AUDIO_CONTROL_1_DUMMY(x) ((u32)(((x >> 13) & 0x0000000F)))
/** @} */

/** @defgroup CODEC_CLOCK_CONTROL_1
 * @brief
 * @{
 */
#define AUD_BIT_AD_0_EN      ((u32)0x00000001 << 6)  /*!<R/W 0  ADC channel 0 clock enable*/
#define AUD_BIT_AD_1_EN      ((u32)0x00000001 << 7)  /*!<R/W 0  ADC channel 1 clock enable*/
#define AUD_BIT_AD_0_FIFO_EN ((u32)0x00000001 << 14) /*!<R/W 0  ADC channel 0 FIFO clock enable*/
#define AUD_BIT_AD_1_FIFO_EN ((u32)0x00000001 << 15) /*!<R/W 0  ADC channel 1 FIFO clock enable*/
/** @} */

/** @defgroup CODEC_CLOCK_CONTROL_2
 * @brief
 * @{
 */
#define AUD_BIT_DMIC_0_EN  ((u32)0x00000001 << 8)  /*!<R/W 0  ADC filter channel 0 clock enable: dmic path*/
#define AUD_BIT_DMIC_1_EN  ((u32)0x00000001 << 9)  /*!<R/W 0  ADC filter channel 1 clock enable: dmic path*/
#define AUD_BIT_AD_0_EQ_EN ((u32)0x00000001 << 16) /*!<R/W 0  adc channel 0 EQ clock en*/
#define AUD_BIT_AD_1_EQ_EN ((u32)0x00000001 << 17) /*!<R/W 0  adc channel 1 EQ clock en*/
/** @} */

/** @defgroup CODEC_CLOCK_CONTROL_3
 * @brief
 * @{
 */
#define AUD_MASK_DMIC1_CLK_SEL   ((u32)0x00000007 << 0)           /*!<R/W 1  Set clock of digital microphone 3'b000: 5MHz 3'b001: 2.5MHz 3'b010: 1.25MHz 3'b011: 625kHz 3'b100: 312.5kHz 3'b101: Reseved 3'b110: Reserved 3'b111: 769.2kHz*/
#define AUD_DMIC1_CLK_SEL(x)     (((u32)((x) & 0x00000007) << 0))
#define AUD_GET_DMIC1_CLK_SEL(x) ((u32)(((x >> 0) & 0x00000007)))
#define AUD_BIT_DMIC1_CLK_EN     ((u32)0x00000001 << 3)           /*!<R/W 0  Digital microphone clock enable*/
/** @} */

/** @defgroup CODEC_CLOCK_CONTROL_4
 * @brief
 * @{
 */
#define AUD_MASK_SAMPLE_RATE_0   ((u32)0x0000000F << 0)           /*!<R/W 0  Set sample rate source 0 4'h0: 48K 4'h1: 96K 4'h2: 192K 4'h3: 32K 4'h4: 176.4K 4'h5: 16K 4'h6: Reserved 4'h7: 8K 4'h8: 44.1K 4'h9: 88.2K 4'ha:24K 4'hb: 12K 4'hc: 22.05K 4'hd: 11.025K 4'he~4'hf: Reserved*/
#define AUD_SAMPLE_RATE_0(x)     (((u32)((x) & 0x0000000F) << 0))
#define AUD_GET_SAMPLE_RATE_0(x) ((u32)(((x >> 0) & 0x0000000F)))
#define AUD_MASK_SAMPLE_RATE_1   ((u32)0x0000000F << 4)           /*!<R/W 0  Set sample rate source 1 4'h0: 48K 4'h1: 96K 4'h2: 192K 4'h3: 32K 4'h4: 176.4K 4'h5: 16K 4'h6: Reserved 4'h7: 8K 4'h8: 44.1K 4'h9: 88.2K 4'ha:24K 4'hb: 12K 4'hc: 22.05K 4'hd: 11.025K 4'he~4'hf: Reserved*/
#define AUD_SAMPLE_RATE_1(x)     (((u32)((x) & 0x0000000F) << 4))
#define AUD_GET_SAMPLE_RATE_1(x) ((u32)(((x >> 4) & 0x0000000F)))
/** @} */

/** @defgroup CODEC_CLOCK_CONTROL_5
 * @brief
 * @{
 */
#define AUD_MASK_ADC_0_FS_SRC_SEL   ((u32)0x00000003 << 0)           /*!<R/W 0  Channel 0 ADC path sample rate Source selection 2'b00: Source 0 2'b01: Source 1 2'b10: Source 2*/
#define AUD_ADC_0_FS_SRC_SEL(x)     (((u32)((x) & 0x00000003) << 0))
#define AUD_GET_ADC_0_FS_SRC_SEL(x) ((u32)(((x >> 0) & 0x00000003)))
#define AUD_MASK_ADC_1_FS_SRC_SEL   ((u32)0x00000003 << 2)           /*!<R/W 0  Channel 1 ADC path sample rate Source selection 2'b00: Source 0 2'b01: Source 1 2'b10: Source 2*/
#define AUD_ADC_1_FS_SRC_SEL(x)     (((u32)((x) & 0x00000003) << 2))
#define AUD_GET_ADC_1_FS_SRC_SEL(x) ((u32)(((x >> 2) & 0x00000003)))
/** @} */

/** @defgroup CODEC_CLOCK_CONTROL_6
 * @brief
 * @{
 */
#define AUD_BIT_ADC_0_ASRC_EN ((u32)0x00000001 << 0) /*!<R/W 0  Channel 0 ADC path ASRC enable 0: Disable 1: Enable If ASRC is enabled, sample_rate becomes useless.*/
#define AUD_BIT_ADC_1_ASRC_EN ((u32)0x00000001 << 1) /*!<R/W 0  Channel 1 ADC path ASRC enable 0: Disable 1: Enable If ASRC is enabled, sample_rate becomes useless.*/
/** @} */

/** @defgroup CODEC_CLOCK_CONTROL_7
 * @brief
 * @{
 */
#define AUD_MASK_ADC_0_DMIC_LPF_CLK_SEL   ((u32)0x00000003 << 7)           /*!<R/W 0  Channel 0 ADC path DMIC LPF clock 2'b00: 10M 2'b01: 5M 2'b10: 2.5M 2'b11: 769.2*2K*/
#define AUD_ADC_0_DMIC_LPF_CLK_SEL(x)     (((u32)((x) & 0x00000003) << 7))
#define AUD_GET_ADC_0_DMIC_LPF_CLK_SEL(x) ((u32)(((x >> 7) & 0x00000003)))
#define AUD_MASK_ADC_1_DMIC_LPF_CLK_SEL   ((u32)0x00000003 << 9)           /*!<R/W 0  Channel 1 ADC path DMIC LPF clock 2'b00: 10M 2'b01: 5M 2'b10: 2.5M 2'b11: 769.2*2K*/
#define AUD_ADC_1_DMIC_LPF_CLK_SEL(x)     (((u32)((x) & 0x00000003) << 9))
#define AUD_GET_ADC_1_DMIC_LPF_CLK_SEL(x) ((u32)(((x >> 9) & 0x00000003)))
/** @} */

/** @defgroup CODEC_ASRC_CONTROL_2
 * @brief
 * @{
 */
#define AUD_MASK_ASRC_RATE_SEL_RX_0   ((u32)0x00000003 << 0)           /*!<R/W 0  0: fs supports 4kHz ~ 60kHz 1: fs supports 60kHz ~ 120kHz 2/3: Reserved*/
#define AUD_ASRC_RATE_SEL_RX_0(x)     (((u32)((x) & 0x00000003) << 0))
#define AUD_GET_ASRC_RATE_SEL_RX_0(x) ((u32)(((x >> 0) & 0x00000003)))
#define AUD_MASK_ASRC_GAIN_SEL_RX_0   ((u32)0x00000003 << 2)           /*!<R/W 0x3  ASRC convergence rate: larger is faster but more noisy*/
#define AUD_ASRC_GAIN_SEL_RX_0(x)     (((u32)((x) & 0x00000003) << 2))
#define AUD_GET_ASRC_GAIN_SEL_RX_0(x) ((u32)(((x >> 2) & 0x00000003)))
#define AUD_BIT_ASRC_AUTO_ADJUST_RX_0 ((u32)0x00000001 << 4)           /*!<R/W 1  HW auto adjust convergence rate 0: Disable 1: Enable*/
/** @} */

/** @defgroup CODEC_ASRC_CONTROL_3
 * @brief
 * @{
 */
#define AUD_MASK_ASRC_SDM_INTI_RX_0   ((u32)0x00FFFFFF << 0)           /*!<R/W 0  Set  initial value of tracked frequency*/
#define AUD_ASRC_SDM_INTI_RX_0(x)     (((u32)((x) & 0x00FFFFFF) << 0))
#define AUD_GET_ASRC_SDM_INTI_RX_0(x) ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */

/** @defgroup CODEC_I2S_0_CONTROL
 * @brief
 * @{
 */
#define AUD_BIT_I2S_0_RST_N_REG             ((u32)0x00000001 << 0)            /*!<R/W 0  I2S 0 1'b1: rst_n to audio digital IP is de-asserted 1'b0: rst_n to audio digital IP is asserted*/
#define AUD_BIT_I2S_0_INV_SCLK              ((u32)0x00000001 << 1)            /*!<R/W 0  I2S 0 1'b1: I2S/PCM bit clock is inverted*/
#define AUD_BIT_I2S_0_SELF_LPBK_EN          ((u32)0x00000001 << 2)            /*!<R/W 0  I2S 0 1'b1: internal loopback mode is enabled*/
#define AUD_BIT_I2S_0_SAME_LRC_EN           ((u32)0x00000001 << 3)            /*!<R/W 0  I2S 0 1'b1: LRC_TX share to LRC_RX*/
#define AUD_MASK_I2S_0_TDM_MODE_RX          ((u32)0x00000003 << 4)            /*!<R/W 0  I2S 0 Rx channel TDM mode 0: 2 channel  1: 4 channel 2/3: reserved*/
#define AUD_I2S_0_TDM_MODE_RX(x)            (((u32)((x) & 0x00000003) << 4))
#define AUD_GET_I2S_0_TDM_MODE_RX(x)        ((u32)(((x >> 4) & 0x00000003)))
#define AUD_MASK_I2S_0_DATA_FORMAT_SEL_TX   ((u32)0x00000003 << 6)            /*!<R/W 0  I2S 0 Tx channel data format 2'b00: I2S 2'b01: left-justified 2'b10: PCM mode A 2'b11: PCM mode B*/
#define AUD_I2S_0_DATA_FORMAT_SEL_TX(x)     (((u32)((x) & 0x00000003) << 6))
#define AUD_GET_I2S_0_DATA_FORMAT_SEL_TX(x) ((u32)(((x >> 6) & 0x00000003)))
#define AUD_MASK_I2S_0_DATA_FORMAT_SEL_RX   ((u32)0x00000003 << 8)            /*!<R/W 0  I2S 0 Rx channel data format 2'b00: I2S 2'b01: left-justified 2'b10: PCM mode A 2'b11: PCM mode B*/
#define AUD_I2S_0_DATA_FORMAT_SEL_RX(x)     (((u32)((x) & 0x00000003) << 8))
#define AUD_GET_I2S_0_DATA_FORMAT_SEL_RX(x) ((u32)(((x >> 8) & 0x00000003)))
#define AUD_MASK_I2S_0_DATA_LEN_SEL_TX      ((u32)0x00000003 << 10)           /*!<R/W 0  I2S 0 Tx channel data length 2'b00: 16 bits 2'b10: 24 bits 2'b11: 8 bits*/
#define AUD_I2S_0_DATA_LEN_SEL_TX(x)        (((u32)((x) & 0x00000003) << 10))
#define AUD_GET_I2S_0_DATA_LEN_SEL_TX(x)    ((u32)(((x >> 10) & 0x00000003)))
#define AUD_MASK_I2S_0_DATA_LEN_SEL_RX      ((u32)0x00000003 << 12)           /*!<R/W 0  I2S 0 Rx channel data length 2'b00: 16 bits 2'b10: 24 bits 2'b11: 8 bits*/
#define AUD_I2S_0_DATA_LEN_SEL_RX(x)        (((u32)((x) & 0x00000003) << 12))
#define AUD_GET_I2S_0_DATA_LEN_SEL_RX(x)    ((u32)(((x >> 12) & 0x00000003)))
#define AUD_MASK_I2S_0_CH_LEN_SEL_TX        ((u32)0x00000003 << 14)           /*!<R/W 0  I2S 0 Tx channel channel length 2'b00: 16 bits 2'b01: 32 bits 2'b10: 24 bits 2'b11: 8 bits*/
#define AUD_I2S_0_CH_LEN_SEL_TX(x)          (((u32)((x) & 0x00000003) << 14))
#define AUD_GET_I2S_0_CH_LEN_SEL_TX(x)      ((u32)(((x >> 14) & 0x00000003)))
#define AUD_MASK_I2S_0_CH_LEN_SEL_RX        ((u32)0x00000003 << 16)           /*!<R/W 0  I2S 0 Rx channel chennel length 2'b00: 16 bits 2'b01: 32 bits 2'b10: 24 bits 2'b11: 8 bits*/
#define AUD_I2S_0_CH_LEN_SEL_RX(x)          (((u32)((x) & 0x00000003) << 16))
#define AUD_GET_I2S_0_CH_LEN_SEL_RX(x)      ((u32)(((x >> 16) & 0x00000003)))
#define AUD_MASK_I2S_0_DATA_CH_SEL_TX       ((u32)0x00000003 << 18)           /*!<R/W 0  I2S 0 Tx channel data channel 2'b00: L/R 2'b01: R/L 2'b10: L/L 2'b11: R/R*/
#define AUD_I2S_0_DATA_CH_SEL_TX(x)         (((u32)((x) & 0x00000003) << 18))
#define AUD_GET_I2S_0_DATA_CH_SEL_TX(x)     ((u32)(((x >> 18) & 0x00000003)))
#define AUD_BIT_I2S_0_MASTER_SEL            ((u32)0x00000001 << 20)           /*!<R/W 0  I2S 0 master source sel 1'b0: sport internal 1'b1: external  I2S*/
/** @} */

/** @defgroup CODEC_I2S_0_CONTROL_1
 * @brief
 * @{
 */
#define AUD_MASK_I2S_0_DATA_CH0_SEL_RX    ((u32)0x00000007 << 0)            /*!<R/W 0  I2S 0 Rx channel data channel 0 0:CH0 1:CH1 2/3/4/5/6/7: reserved*/
#define AUD_I2S_0_DATA_CH0_SEL_RX(x)      (((u32)((x) & 0x00000007) << 0))
#define AUD_GET_I2S_0_DATA_CH0_SEL_RX(x)  ((u32)(((x >> 0) & 0x00000007)))
#define AUD_MASK_I2S_0_DATA_CH1_SEL_RX    ((u32)0x00000007 << 3)            /*!<R/W 1  I2S 0 Rx channel data channel 1 0:CH0 1:CH1 2/3/4/5/6/7: reserved*/
#define AUD_I2S_0_DATA_CH1_SEL_RX(x)      (((u32)((x) & 0x00000007) << 3))
#define AUD_GET_I2S_0_DATA_CH1_SEL_RX(x)  ((u32)(((x >> 3) & 0x00000007)))
#define AUD_MASK_I2S_0_DATA_CH2_SEL_RX    ((u32)0x00000007 << 6)            /*!<R/W 2  I2S 0 Rx channel data channel 2 0:CH0 1:CH1 2/3/4/5/6/7: reserved*/
#define AUD_I2S_0_DATA_CH2_SEL_RX(x)      (((u32)((x) & 0x00000007) << 6))
#define AUD_GET_I2S_0_DATA_CH2_SEL_RX(x)  ((u32)(((x >> 6) & 0x00000007)))
#define AUD_MASK_I2S_0_DATA_CH3_SEL_RX    ((u32)0x00000007 << 9)            /*!<R/W 3  I2S 0 Rx channel data channel 3 0:CH0 1:CH1 2/3/4/5/6/7: reserved*/
#define AUD_I2S_0_DATA_CH3_SEL_RX(x)      (((u32)((x) & 0x00000007) << 9))
#define AUD_GET_I2S_0_DATA_CH3_SEL_RX(x)  ((u32)(((x >> 9) & 0x00000007)))
#define AUD_MASK_I2S_0_DATA_CH4_SEL_RX    ((u32)0x00000007 << 12)           /*!<R/W 4  I2S 0 Rx channel data channel 4 0:CH0 1:CH1 2/3/4/5/6/7: reserved*/
#define AUD_I2S_0_DATA_CH4_SEL_RX(x)      (((u32)((x) & 0x00000007) << 12))
#define AUD_GET_I2S_0_DATA_CH4_SEL_RX(x)  ((u32)(((x >> 12) & 0x00000007)))
#define AUD_MASK_I2S_0_DATA_CH5_SEL_RX    ((u32)0x00000007 << 15)           /*!<R/W 5  I2S 0 Rx channel data channel 5 0:CH0 1:CH1 2/3/4/5/6/7: reserved*/
#define AUD_I2S_0_DATA_CH5_SEL_RX(x)      (((u32)((x) & 0x00000007) << 15))
#define AUD_GET_I2S_0_DATA_CH5_SEL_RX(x)  ((u32)(((x >> 15) & 0x00000007)))
#define AUD_MASK_I2S_0_DATA_CH6_SEL_RX    ((u32)0x00000007 << 18)           /*!<R/W 0  I2S 0 Rx channel data channel 6 0:CH0 1:CH1 2/3/4/5/6/7: reserved*/
#define AUD_I2S_0_DATA_CH6_SEL_RX(x)      (((u32)((x) & 0x00000007) << 18))
#define AUD_GET_I2S_0_DATA_CH6_SEL_RX(x)  ((u32)(((x >> 18) & 0x00000007)))
#define AUD_MASK_I2S_0_DATA_CH7_SEL_RX    ((u32)0x00000007 << 21)           /*!<R/W 0  I2S 0 Rx channel data channel 7 0:CH0 1:CH1 2/3/4/5/6/7: reserved*/
#define AUD_I2S_0_DATA_CH7_SEL_RX(x)      (((u32)((x) & 0x00000007) << 21))
#define AUD_GET_I2S_0_DATA_CH7_SEL_RX(x)  ((u32)(((x >> 21) & 0x00000007)))
#define AUD_BIT_I2S_0_DATA_CH0_RX_DISABLE ((u32)0x00000001 << 24)           /*!<R/W 0  I2S 0 Rx channel data channel 0 1: disabe 0: enable*/
#define AUD_BIT_I2S_0_DATA_CH1_RX_DISABLE ((u32)0x00000001 << 25)           /*!<R/W 0  I2S 0 Rx channel data channel 1 1: disabe 0: enable*/
#define AUD_BIT_I2S_0_DATA_CH2_RX_DISABLE ((u32)0x00000001 << 26)           /*!<R/W 0  I2S 0 Rx channel data channel 2 1: disabe 0: enable*/
#define AUD_BIT_I2S_0_DATA_CH3_RX_DISABLE ((u32)0x00000001 << 27)           /*!<R/W 0  I2S 0 Rx channel data channel 3 1: disabe 0: enable*/
#define AUD_BIT_I2S_0_DATA_CH4_RX_DISABLE ((u32)0x00000001 << 28)           /*!<R/W 0  I2S 0 Rx channel data channel 4 1: disabe 0: enable*/
#define AUD_BIT_I2S_0_DATA_CH5_RX_DISABLE ((u32)0x00000001 << 29)           /*!<R/W 0  I2S 0 Rx channel data channel 5 1: disabe 0: enable*/
#define AUD_BIT_I2S_0_DATA_CH6_RX_DISABLE ((u32)0x00000001 << 30)           /*!<R/W 0  I2S 0 Rx channel data channel 6 1: disabe 0: enable*/
#define AUD_BIT_I2S_0_DATA_CH7_RX_DISABLE ((u32)0x00000001 << 31)           /*!<R/W 0  I2S 0 Rx channel data channel 7 1: disabe 0: enable*/
/** @} */

/** @defgroup CODEC_ADC_x_CONTROL_0
 * @brief
 * @{
 */
#define AUD_MASK_ADC_x_DMIC_SRC_SEL         ((u32)0x00000007 << 0)            /*!<R/W 0  Channel x DMIC source selection 3'b000: MIC 1 rising 3'b001: MIC 1 falling 3'b010: MIC 2 rising 3'b011: MIC 2 falling Other: reserved*/
#define AUD_ADC_x_DMIC_SRC_SEL(x)           (((u32)((x) & 0x00000007) << 0))
#define AUD_GET_ADC_x_DMIC_SRC_SEL(x)       ((u32)(((x >> 0) & 0x00000007)))
#define AUD_BIT_ADC_x_DMIC_LPF2ND_EN        ((u32)0x00000001 << 3)            /*!<R/W 1  Channel x DMIC path SRC 2nd LPF control 0: Disable 1: Enable*/
#define AUD_BIT_ADC_x_DMIC_LPF1ST_EN        ((u32)0x00000001 << 4)            /*!<R/W 1  Channel x DMIC path SRC 1st LPF control 0: Disable 1: Enable*/
#define AUD_MASK_ADC_x_DMIC_LPF1ST_FC_SEL   ((u32)0x00000003 << 5)            /*!<R/W 0  Channel x DMIC path SRC 1st LPF FC*/
#define AUD_ADC_x_DMIC_LPF1ST_FC_SEL(x)     (((u32)((x) & 0x00000003) << 5))
#define AUD_GET_ADC_x_DMIC_LPF1ST_FC_SEL(x) ((u32)(((x >> 5) & 0x00000003)))
#define AUD_BIT_ADC_x_DMIC_MIX_MUTE         ((u32)0x00000001 << 7)            /*!<R/W 1  Channel x DMIC input path mute 0: Un-Mute 1: Mute*/
#define AUD_MASK_ADC_x_AD_ZDET_FUNC         ((u32)0x00000003 << 18)           /*!<R/W 0x2  Channel x ADC path zero detection function select 2'b00: immediate change 2'b01: zero detection & immediate change 2'b10: zdet & step 2'b11: zdet & timeout step*/
#define AUD_ADC_x_AD_ZDET_FUNC(x)           (((u32)((x) & 0x00000003) << 18))
#define AUD_GET_ADC_x_AD_ZDET_FUNC(x)       ((u32)(((x >> 18) & 0x00000003)))
#define AUD_MASK_ADC_x_AD_ZDET_TOUT         ((u32)0x00000003 << 20)           /*!<R/W 0  Channel x ADC path zero detection time out select 2'b00: 1024*16 samples 2'b01: 1024*32 samples 2'b10: 1024*64 samples 2'b11: 64 samples*/
#define AUD_ADC_x_AD_ZDET_TOUT(x)           (((u32)((x) & 0x00000003) << 20))
#define AUD_GET_ADC_x_AD_ZDET_TOUT(x)       ((u32)(((x >> 20) & 0x00000003)))
#define AUD_BIT_ADC_x_AD_MUTE               ((u32)0x00000001 << 22)           /*!<R/W 0  Channel x ADC path mute 0: Un-Mute 1: Mute*/
#define AUD_BIT_ADC_x_HPF_RSVD              ((u32)0x00000001 << 23)           /*!<R/W 0  Channel x ADC path reserved*/
#define AUD_BIT_ADC_x_DCHPF_EN              ((u32)0x00000001 << 26)           /*!<R/W 0  Channel x ADC path high pass filter enable control (filter DC) 0: Disable 1: Enable*/
#define AUD_MASK_ADC_x_DCHPF_FC_SEL         ((u32)0x00000007 << 27)           /*!<R/W 0x4  Channel x ADC path high pass filter Fc*/
#define AUD_ADC_x_DCHPF_FC_SEL(x)           (((u32)((x) & 0x00000007) << 27))
#define AUD_GET_ADC_x_DCHPF_FC_SEL(x)       ((u32)(((x >> 27) & 0x00000007)))
#define AUD_MASK_ADC_x_DMIC_LPF2ND_FC_SEL   ((u32)0x00000003 << 30)           /*!<R/W 0  Uplink Channel x DMIC path SRC 2nd LPF FC*/
#define AUD_ADC_x_DMIC_LPF2ND_FC_SEL(x)     (((u32)((x) & 0x00000003) << 30))
#define AUD_GET_ADC_x_DMIC_LPF2ND_FC_SEL(x) ((u32)(((x >> 30) & 0x00000003)))
/** @} */

/** @defgroup CODEC_ADC_x_CONTROL_1
 * @brief
 * @{
 */
#define AUD_MASK_ADC_x_AD_GAIN      ((u32)0x000000FF << 0)            /*!<R/W 0x2f  Channel x ADC digital volume -17.625dB ~ 48dB in 0.375dB step 8'h00: -17.625dB ... 8'h2f: 0dB 8'h30: 0.375dB ... 8'haf: 48dB*/
#define AUD_ADC_x_AD_GAIN(x)        (((u32)((x) & 0x000000FF) << 0))
#define AUD_GET_ADC_x_AD_GAIN(x)    ((u32)(((x >> 0) & 0x000000FF)))
#define AUD_MASK_ADC_x_BOOST_GAIN   ((u32)0x00000003 << 8)            /*!<R/W 0  Channel x ADC path boost gain control 00: 0dB 01: 12dB 10: 24dB 11: 36dB*/
#define AUD_ADC_x_BOOST_GAIN(x)     (((u32)((x) & 0x00000003) << 8))
#define AUD_GET_ADC_x_BOOST_GAIN(x) ((u32)(((x >> 8) & 0x00000003)))
#define AUD_MASK_ADC_x_RPTR_HOLD    ((u32)0x0000000F << 12)           /*!<R/W 0  Channel x I2S read point hold number rptr_hold  = (I2S_fs/ad_fs) - 1*/
#define AUD_ADC_x_RPTR_HOLD(x)      (((u32)((x) & 0x0000000F) << 12))
#define AUD_GET_ADC_x_RPTR_HOLD(x)  ((u32)(((x >> 12) & 0x0000000F)))
#define AUD_BIT_ADC_x_FIFO_KEEP_ONE ((u32)0x00000001 << 16)           /*!<R/W 0  Channel x i2s sample buffering 0: 4 samples 1: 1 sample*/
/** @} */

/** @defgroup CODEC_ADC_ALIGN_CONTROL_0
 * @brief
 * @{
 */
#define AUD_MASK_ADC_ALIGN_DMIC_SRC_SEL         ((u32)0x00000007 << 0)            /*!<R/W 0  Channel align DMIC source selection 3'b000: MIC 1 rising 3'b001: MIC 1 falling 3'b010: MIC 2 rising 3'b011: MIC 2 falling Other: reserved*/
#define AUD_ADC_ALIGN_DMIC_SRC_SEL(x)           (((u32)((x) & 0x00000007) << 0))
#define AUD_GET_ADC_ALIGN_DMIC_SRC_SEL(x)       ((u32)(((x >> 0) & 0x00000007)))
#define AUD_BIT_ADC_ALIGN_DMIC_LPF2ND_EN        ((u32)0x00000001 << 3)            /*!<R/W 1  Channel align DMIC path SRC 2nd LPF control 0: Disable 1: Enable*/
#define AUD_BIT_ADC_ALIGN_DMIC_LPF1ST_EN        ((u32)0x00000001 << 4)            /*!<R/W 1  Channel align DMIC path SRC 1st LPF control 0: Disable 1: Enable*/
#define AUD_MASK_ADC_ALIGN_DMIC_LPF1ST_FC_SEL   ((u32)0x00000003 << 5)            /*!<R/W 0  Channel align DMIC path SRC 1st LPF FC*/
#define AUD_ADC_ALIGN_DMIC_LPF1ST_FC_SEL(x)     (((u32)((x) & 0x00000003) << 5))
#define AUD_GET_ADC_ALIGN_DMIC_LPF1ST_FC_SEL(x) ((u32)(((x >> 5) & 0x00000003)))
#define AUD_BIT_ADC_ALIGN_DMIC_MIX_MUTE         ((u32)0x00000001 << 7)            /*!<R/W 1  Channel align DMIC input path mute 0: Un-Mute 1: Mute*/
#define AUD_MASK_ADC_ALIGN_AD_ZDET_FUNC         ((u32)0x00000003 << 18)           /*!<R/W 0x2  Channel align ADC path zero detection function select 2'b00: immediate change 2'b01: zero detection & immediate change 2'b10: zdet & step 2'b11: zdet & timeout step*/
#define AUD_ADC_ALIGN_AD_ZDET_FUNC(x)           (((u32)((x) & 0x00000003) << 18))
#define AUD_GET_ADC_ALIGN_AD_ZDET_FUNC(x)       ((u32)(((x >> 18) & 0x00000003)))
#define AUD_MASK_ADC_ALIGN_AD_ZDET_TOUT         ((u32)0x00000003 << 20)           /*!<R/W 0  Channel align ADC path zero detection time out select 2'b00: 1024*16 samples 2'b01: 1024*32 samples 2'b10: 1024*64 samples 2'b11: 64 samples*/
#define AUD_ADC_ALIGN_AD_ZDET_TOUT(x)           (((u32)((x) & 0x00000003) << 20))
#define AUD_GET_ADC_ALIGN_AD_ZDET_TOUT(x)       ((u32)(((x >> 20) & 0x00000003)))
#define AUD_BIT_ADC_ALIGN_AD_MUTE               ((u32)0x00000001 << 22)           /*!<R/W 0  Channel align ADC path mute 0: Un-Mute 1: Mute*/
#define AUD_BIT_ADC_ALIGN_HPF_RSVD              ((u32)0x00000001 << 23)           /*!<R/W 0  Channel align ADC path reserved*/
#define AUD_BIT_ADC_ALIGN_DCHPF_EN              ((u32)0x00000001 << 26)           /*!<R/W 0  Channel align ADC path high pass filter enable control (filter DC) 0: Disable 1: Enable*/
#define AUD_MASK_ADC_ALIGN_DCHPF_FC_SEL         ((u32)0x00000007 << 27)           /*!<R/W 0x4  Channel align ADC path high pass filter Fc*/
#define AUD_ADC_ALIGN_DCHPF_FC_SEL(x)           (((u32)((x) & 0x00000007) << 27))
#define AUD_GET_ADC_ALIGN_DCHPF_FC_SEL(x)       ((u32)(((x >> 27) & 0x00000007)))
#define AUD_MASK_ADC_ALIGN_DMIC_LPF2ND_FC_SEL   ((u32)0x00000003 << 30)           /*!<R/W 0  Uplink Channel align DMIC path SRC 2nd LPF FC*/
#define AUD_ADC_ALIGN_DMIC_LPF2ND_FC_SEL(x)     (((u32)((x) & 0x00000003) << 30))
#define AUD_GET_ADC_ALIGN_DMIC_LPF2ND_FC_SEL(x) ((u32)(((x >> 30) & 0x00000003)))
/** @} */

/** @defgroup CODEC_ADC_ALIGN_CONTROL
 * @brief
 * @{
 */
#define AUD_MASK_ADC_0_ALIGN_CH_SEL   ((u32)0x00000007 << 0)           /*!<R/W 0  Uplink CH0 align channel selection 0: CH0 1: CH1 Others: reserved*/
#define AUD_ADC_0_ALIGN_CH_SEL(x)     (((u32)((x) & 0x00000007) << 0))
#define AUD_GET_ADC_0_ALIGN_CH_SEL(x) ((u32)(((x >> 0) & 0x00000007)))
#define AUD_BIT_ADC_0_ALIGN_EN        ((u32)0x00000001 << 3)           /*!<R/W 0  Uplink CH0 align enable*/
#define AUD_MASK_ADC_1_ALIGN_CH_SEL   ((u32)0x00000007 << 4)           /*!<R/W 0  Uplink CH1 align channel selection 0: CH0 1: CH1 Others: reserved*/
#define AUD_ADC_1_ALIGN_CH_SEL(x)     (((u32)((x) & 0x00000007) << 4))
#define AUD_GET_ADC_1_ALIGN_CH_SEL(x) ((u32)(((x >> 4) & 0x00000007)))
#define AUD_BIT_ADC_1_ALIGN_EN        ((u32)0x00000001 << 7)           /*!<R/W 0  Uplink CH1 align enable*/
/** @} */

/** @defgroup CODEC_ADC_x_SILENCE_CONTROL
 * @brief
 * @{
 */
#define AUD_BIT_ADC_x_SILENCE_DET_EN          ((u32)0x00000001 << 0)           /*!<R/W 0  Channel x ADC path silence detection enable 0: Disable 1: Enable*/
#define AUD_MASK_ADC_x_SILENCE_LEVEL_SEL      ((u32)0x00000007 << 1)           /*!<R/W 1  Channel x ADC path silence detection threshold 3'b000: -54db 3'b001: -60db 3'b010: -66db 3'b011: -72db 3'b100: -78db 3'b101: -84db 3'b110: -90db 3'b111: -96db*/
#define AUD_ADC_x_SILENCE_LEVEL_SEL(x)        (((u32)((x) & 0x00000007) << 1))
#define AUD_GET_ADC_x_SILENCE_LEVEL_SEL(x)    ((u32)(((x >> 1) & 0x00000007)))
#define AUD_MASK_ADC_x_SILENCE_DEBOUNCE_SEL   ((u32)0x00000007 << 4)           /*!<R/W 0x3  Channel x ADC path silence detection debounce (48K) 3'b000: 80ms 3'b001: 160ms 3'b010: 320ms 3'b011: 640ms 3'b100: 1.28s 3'b101: 2.56s 3'b110: 5.12s 3'b111: 0.16ms*/
#define AUD_ADC_x_SILENCE_DEBOUNCE_SEL(x)     (((u32)((x) & 0x00000007) << 4))
#define AUD_GET_ADC_x_SILENCE_DEBOUNCE_SEL(x) ((u32)(((x >> 4) & 0x00000007)))
/** @} */

/** @defgroup CODEC_ADC_0_EQ_CTRL
 * @brief
 * @{
 */
#define AUD_BIT_ADC_0_BIQUAD_EN_0 ((u32)0x00000001 << 0) /*!<R/W 0  ADC channel 0 EQ 0-band biquad enable 0: disable 1: enable*/
#define AUD_BIT_ADC_0_BIQUAD_EN_1 ((u32)0x00000001 << 1) /*!<R/W 0  ADC channel 0 EQ 1-band biquad enable 0: disable 1: enable*/
#define AUD_BIT_ADC_0_BIQUAD_EN_2 ((u32)0x00000001 << 2) /*!<R/W 0  ADC channel 0 EQ 2-band biquad enable 0: disable 1: enable*/
#define AUD_BIT_ADC_0_BIQUAD_EN_3 ((u32)0x00000001 << 3) /*!<R/W 0  ADC channel 0 EQ 3-band biquad enable 0: disable 1: enable*/
#define AUD_BIT_ADC_0_BIQUAD_EN_4 ((u32)0x00000001 << 4) /*!<R/W 0  ADC channel 0 EQ 4-band biquad enable 0: disable 1: enable*/
/** @} */

/** @defgroup CODEC_ADC_0_BIQUAD_H0_x
 * @brief
 * @{
 */
#define AUD_MASK_ADC_0_BIQUAD_H0_x   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0x2000000  ADC channel 0 EQ x-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_0_BIQUAD_H0_x(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_0_BIQUAD_H0_x(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_0_BIQUAD_B1_x
 * @brief
 * @{
 */
#define AUD_MASK_ADC_0_BIQUAD_B1_x   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 0 EQ x-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_0_BIQUAD_B1_x(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_0_BIQUAD_B1_x(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_0_BIQUAD_B2_x
 * @brief
 * @{
 */
#define AUD_MASK_ADC_0_BIQUAD_B2_x   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 0 EQ x-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_0_BIQUAD_B2_x(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_0_BIQUAD_B2_x(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_0_BIQUAD_A1_x
 * @brief
 * @{
 */
#define AUD_MASK_ADC_0_BIQUAD_A1_x   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 0 EQ x-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_0_BIQUAD_A1_x(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_0_BIQUAD_A1_x(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_0_BIQUAD_A2_x
 * @brief
 * @{
 */
#define AUD_MASK_ADC_0_BIQUAD_A2_x   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 0 EQ x-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_0_BIQUAD_A2_x(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_0_BIQUAD_A2_x(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_EQ_CTRL
 * @brief
 * @{
 */
#define AUD_BIT_ADC_1_BIQUAD_EN_0 ((u32)0x00000001 << 0) /*!<R/W 0  ADC channel 1 EQ 0-band biquad enable 0: disable 1: enable*/
#define AUD_BIT_ADC_1_BIQUAD_EN_1 ((u32)0x00000001 << 1) /*!<R/W 0  ADC channel 1 EQ 1-band biquad enable 0: disable 1: enable*/
#define AUD_BIT_ADC_1_BIQUAD_EN_2 ((u32)0x00000001 << 2) /*!<R/W 0  ADC channel 1 EQ 2-band biquad enable 0: disable 1: enable*/
#define AUD_BIT_ADC_1_BIQUAD_EN_3 ((u32)0x00000001 << 3) /*!<R/W 0  ADC channel 1 EQ 3-band biquad enable 0: disable 1: enable*/
#define AUD_BIT_ADC_1_BIQUAD_EN_4 ((u32)0x00000001 << 4) /*!<R/W 0  ADC channel 1 EQ 4-band biquad enable 0: disable 1: enable*/
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_H0_0
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_H0_0   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0x2000000  ADC channel 1 EQ 0-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_H0_0(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_H0_0(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_B1_0
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_B1_0   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 0-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_B1_0(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_B1_0(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_B2_0
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_B2_0   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 0-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_B2_0(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_B2_0(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_A1_0
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_A1_0   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 0-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_A1_0(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_A1_0(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_A2_0
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_A2_0   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 0-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_A2_0(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_A2_0(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_H0_1
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_H0_1   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0x2000000  ADC channel 1 EQ 1-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_H0_1(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_H0_1(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_B1_1
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_B1_1   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 1-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_B1_1(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_B1_1(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_B2_1
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_B2_1   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 1-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_B2_1(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_B2_1(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_A1_1
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_A1_1   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 1-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_A1_1(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_A1_1(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_A2_1
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_A2_1   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 1-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_A2_1(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_A2_1(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_H0_2
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_H0_2   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0x2000000  ADC channel 1 EQ 2-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_H0_2(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_H0_2(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_B1_2
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_B1_2   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 2-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_B1_2(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_B1_2(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_B2_2
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_B2_2   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 2-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_B2_2(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_B2_2(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_A1_2
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_A1_2   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 2-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_A1_2(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_A1_2(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_A2_2
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_A2_2   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 2-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_A2_2(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_A2_2(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_H0_3
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_H0_3   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0x2000000  ADC channel 1 EQ 3-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_H0_3(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_H0_3(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_B1_3
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_B1_3   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 3-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_B1_3(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_B1_3(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_B2_3
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_B2_3   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 3-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_B2_3(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_B2_3(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_A1_3
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_A1_3   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 3-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_A1_3(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_A1_3(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_A2_3
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_A2_3   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 3-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_A2_3(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_A2_3(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_H0_4
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_H0_4   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0x2000000  ADC channel 1 EQ 4-band coef. h0 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_H0_4(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_H0_4(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_B1_4
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_B1_4   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 4-band coef. b1 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_B1_4(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_B1_4(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_B2_4
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_B2_4   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 4-band coef. b2 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_B2_4(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_B2_4(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_A1_4
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_A1_4   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 4-band coef. a1 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_A1_4(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_A1_4(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_BIQUAD_A2_4
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_BIQUAD_A2_4   ((u32)0x1FFFFFFF << 0)           /*!<R/W 0  ADC channel 1 EQ 4-band coef. a2 2's complement in 4.25 format, i.e. the range is from -8~7.99.*/
#define AUD_ADC_1_BIQUAD_A2_4(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define AUD_GET_ADC_1_BIQUAD_A2_4(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CODEC_ANA_READ
 * @brief
 * @{
 */
#define AUD_BIT_MICBIAS_OC ((u32)0x00000001 << 0) /*!<R   The status flag of MICBIAS over-current protection*/
/** @} */

/** @defgroup CODEC_ADC_0_LPF_RD
 * @brief
 * @{
 */
#define AUD_MASK_ADC_0_LPF_RD   ((u32)0x0007FFFF << 0)           /*!<R   CH 0 ADC LPF out valus*/
#define AUD_ADC_0_LPF_RD(x)     (((u32)((x) & 0x0007FFFF) << 0))
#define AUD_GET_ADC_0_LPF_RD(x) ((u32)(((x >> 0) & 0x0007FFFF)))
/** @} */

/** @defgroup CODEC_ADC_1_LPF_RD
 * @brief
 * @{
 */
#define AUD_MASK_ADC_1_LPF_RD   ((u32)0x0007FFFF << 0)           /*!<R   CH 1 ADC LPF out valus*/
#define AUD_ADC_1_LPF_RD(x)     (((u32)((x) & 0x0007FFFF) << 0))
#define AUD_GET_ADC_1_LPF_RD(x) ((u32)(((x >> 0) & 0x0007FFFF)))
/** @} */

/** @defgroup CODEC_SILENCE_INFORM
 * @brief
 * @{
 */
#define AUD_BIT_ADC_0_SILENCE_DET_O      ((u32)0x00000001 << 8)  /*!<R   adc_0_silencedata status (result of silence detection) 1'b0: not adc_0_silencedata 1'b1: adc_0_silenceis detected*/
#define AUD_BIT_ADC_0_SILENCE_DET_STATUS ((u32)0x00000001 << 9)  /*!<R   Ongoing status of adc_0_silencedetection 1'b0: adc_0_silencedetection is resting (clock is gating) 1'b1: adc_0_silencedetection is working*/
#define AUD_BIT_ADC_1_SILENCE_DET_O      ((u32)0x00000001 << 10) /*!<R   adc_1_silencedata status (result of silence detection) 1'b0: not adc_1_silencedata 1'b1: adc_1_silenceis detected*/
#define AUD_BIT_ADC_1_SILENCE_DET_STATUS ((u32)0x00000001 << 11) /*!<R   Ongoing status of adc_1_silencedetection 1'b0: adc_1_silencedetection is resting (clock is gating) 1'b1: adc_1_silencedetection is working*/
/** @} */
/** @} */
/* Exported types --------------------------------------------------------*/

/** @defgroup AUDIO_CODEC_Exported_Types AUDIO_CODEC Exported Types
* @{
*/

/** @brief AUDIO port
*/
typedef struct {
	__IO uint32_t CODEC_ADC_x_CONTROL_0;  /*!< Register */
	__IO uint32_t CODEC_ADC_x_CONTROL_1;  /*!< Register */
} CODEC_ADC_CH_CTRL_TypeDef;
/** @brief AUDIO port
*/
// typedef struct {
// 	__IO uint32_t CODEC_ADC_0_BIQUAD_H0_x;  /*!< Register */
// 	__IO uint32_t CODEC_ADC_0_BIQUAD_B1_x;  /*!< Register */
// 	__IO uint32_t CODEC_ADC_0_BIQUAD_B2_x;  /*!< Register */
// 	__IO uint32_t CODEC_ADC_0_BIQUAD_A1_x;  /*!< Register */
// 	__IO uint32_t CODEC_ADC_0_BIQUAD_A2_x;  /*!< Register */
// } CODEC_ADC_0_EQ_BAND_TypeDef;


typedef struct {

	__IO uint32_t CODEC_BIQUAD_H0_x;                      /*!< ADC CHANNEL 0 EQ x BAND BIQUAD H0 REGISTER */
	__IO uint32_t CODEC_BIQUAD_B1_x;                      /*!< ADC CHANNEL 0 EQ x BAND BIQUAD B1 REGISTER */
	__IO uint32_t CODEC_BIQUAD_B2_x;                      /*!< ADC CHANNEL 0 EQ x BAND BIQUAD B2 REGISTER */
	__IO uint32_t CODEC_BIQUAD_A1_x;                      /*!< ADC CHANNEL 0 EQ x BAND BIQUAD A1 REGISTER */
	__IO uint32_t CODEC_BIQUAD_A2_x;                      /*!< ADC CHANNEL 0 EQ x BAND BIQUAD A2 REGISTER */
} CODEC_EQ_BAND_TypeDef;

/**
* @brief AUDIO_CODEC Register Declaration
*/


// typedef struct {
// 	__IO uint32_t CODEC_AUDIO_CONTROL_0                             ;  /*!< Register,  Address offset:0x000 */
// 	__IO uint32_t CODEC_AUDIO_CONTROL_1                             ;  /*!< Register,  Address offset:0x004 */
// 	__IO uint32_t CODEC_CLOCK_CONTROL_1                             ;  /*!< Register,  Address offset:0x008 */
// 	__IO uint32_t CODEC_CLOCK_CONTROL_2                             ;  /*!< Register,  Address offset:0x00C */
// 	__IO uint32_t CODEC_CLOCK_CONTROL_3                             ;  /*!< Register,  Address offset:0x010 */
// 	__IO uint32_t CODEC_CLOCK_CONTROL_4                             ;  /*!< Register,  Address offset:0x014 */
// 	__IO uint32_t CODEC_CLOCK_CONTROL_5                             ;  /*!< Register,  Address offset:0x018 */
// 	__IO uint32_t CODEC_CLOCK_CONTROL_6                             ;  /*!< Register,  Address offset:0x01C */
// 	__IO uint32_t CODEC_CLOCK_CONTROL_7                             ;  /*!< Register,  Address offset:0x020 */
// 	__IO uint32_t CODEC_ASRC_CONTROL_0                              ;  /*!< Register,  Address offset:0x024 */
// 	__IO uint32_t CODEC_ASRC_CONTROL_1                              ;  /*!< Register,  Address offset:0x028 */
// 	__IO uint32_t CODEC_ASRC_CONTROL_2                              ;  /*!< Register,  Address offset:0x02C */
// 	__IO uint32_t CODEC_ASRC_CONTROL_3                              ;  /*!< Register,  Address offset:0x030 */
// 	__IO uint32_t CODEC_ASRC_CONTROL_4                              ;  /*!< Register,  Address offset:0x034 */
// 	__IO uint32_t CODEC_ASRC_CONTROL_5                              ;  /*!< Register,  Address offset:0x038 */
// 	__IO uint32_t CODEC_I2S_0_CONTROL                               ;  /*!< Register,  Address offset:0x03C */
// 	__IO uint32_t CODEC_I2S_0_CONTROL_1                             ;  /*!< Register,  Address offset:0x040 */
// 	__IO uint32_t RSVD0[3]                                          ;  /*!< Reserved,  Address offset:0x044-0x04C */
// 	CODEC_ADC_CH_CTRL_TypeDef CODEC_ADC_CH_CTRL[2]                  ;  /*!< AUDIO IP have 2 ports,  Address offset:0x050-0x05C */
// 	__IO uint32_t RSVD1[19]                                         ;  /*!< Reserved,  Address offset:0x060-0x0A8 */
// 	__IO uint32_t CODEC_ADC_ALIGN_CONTROL_0                         ;  /*!< Register,  Address offset:0x0AC */
// 	__IO uint32_t CODEC_ADC_ALIGN_CONTROL                           ;  /*!< Register,  Address offset:0x0B0 */
// 	__IO uint32_t RSVD2[19]                                         ;  /*!< Reserved,  Address offset:0x0B4-0x0FC */
// 	__IO uint32_t CODEC_ADC_x_SILENCE_CONTROL[2]                    ;  /*!< Register,  Address offset:0x100-0x104 */
// 	__IO uint32_t RSVD3[190]                                        ;  /*!< Reserved,  Address offset:0x108-0x3FC */
// 	__IO uint32_t CODEC_ADC_0_EQ_CTRL                               ;  /*!< Register,  Address offset:0x400 */
// 	CODEC_ADC_0_EQ_BAND_TypeDef CODEC_ADC_0_EQ_BAND[5]              ;  /*!< AUDIO IP have 5 ports,  Address offset:0x404-0x464 */
// 	__IO uint32_t CODEC_ADC_1_EQ_CTRL                               ;  /*!< Register,  Address offset:0x468 */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_H0_0                           ;  /*!< Register,  Address offset:0x46C */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_B1_0                           ;  /*!< Register,  Address offset:0x470 */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_B2_0                           ;  /*!< Register,  Address offset:0x474 */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_A1_0                           ;  /*!< Register,  Address offset:0x478 */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_A2_0                           ;  /*!< Register,  Address offset:0x47C */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_H0_1                           ;  /*!< Register,  Address offset:0x480 */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_B1_1                           ;  /*!< Register,  Address offset:0x484 */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_B2_1                           ;  /*!< Register,  Address offset:0x488 */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_A1_1                           ;  /*!< Register,  Address offset:0x48C */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_A2_1                           ;  /*!< Register,  Address offset:0x490 */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_H0_2                           ;  /*!< Register,  Address offset:0x494 */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_B1_2                           ;  /*!< Register,  Address offset:0x498 */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_B2_2                           ;  /*!< Register,  Address offset:0x49C */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_A1_2                           ;  /*!< Register,  Address offset:0x4A0 */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_A2_2                           ;  /*!< Register,  Address offset:0x4A4 */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_H0_3                           ;  /*!< Register,  Address offset:0x4A8 */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_B1_3                           ;  /*!< Register,  Address offset:0x4AC */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_B2_3                           ;  /*!< Register,  Address offset:0x4B0 */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_A1_3                           ;  /*!< Register,  Address offset:0x4B4 */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_A2_3                           ;  /*!< Register,  Address offset:0x4B8 */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_H0_4                           ;  /*!< Register,  Address offset:0x4BC */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_B1_4                           ;  /*!< Register,  Address offset:0x4C0 */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_B2_4                           ;  /*!< Register,  Address offset:0x4C4 */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_A1_4                           ;  /*!< Register,  Address offset:0x4C8 */
// 	__IO uint32_t CODEC_ADC_1_BIQUAD_A2_4                           ;  /*!< Register,  Address offset:0x4CC */
// 	__IO uint32_t RSVD4[204]                                        ;  /*!< Reserved,  Address offset:0x4D0-0x7FC */
// 	__I  uint32_t CODEC_ANA_READ                                    ;  /*!< Register,  Address offset:0x800 */
// 	__I  uint32_t CODEC_ADC_0_LPF_RD                                ;  /*!< Register,  Address offset:0x804 */
// 	__I  uint32_t CODEC_ADC_1_LPF_RD                                ;  /*!< Register,  Address offset:0x808 */
// 	__IO uint32_t RSVD5[6]                                          ;  /*!< Reserved,  Address offset:0x80C-0x820 */
// 	__I  uint32_t CODEC_SILENCE_INFORM                              ;  /*!< Register,  Address offset:0x824 */
// } AUDIO_CODEC_TypeDef;



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
	__I  uint32_t CODEC_ASRC_CONTROL_0;                   /*!< ASRC CONTROL REGISTER 0 Register,  Address offset: 0x24 */
	__I  uint32_t CODEC_ASRC_CONTROL_1;                   /*!< ASRC CONTROL REGISTER 1 Register,  Address offset: 0x28 */
	__IO uint32_t CODEC_ASRC_CONTROL_2;                   /*!< ASRC CONTROL REGISTER 2 Register,  Address offset: 0x2C */
	__IO uint32_t CODEC_ASRC_CONTROL_3;                   /*!< ASRC CONTROL REGISTER 3 Register,  Address offset: 0x30 */
	__IO uint32_t RSVD0[2];                               /*!<  Reserved,  Address offset:0x34-0x38 */
	__IO uint32_t CODEC_I2S_0_CONTROL;                    /*!< I2S 0 CONTROL REGISTER,  Address offset: 0x3C */
	__IO uint32_t CODEC_I2S_0_CONTROL_1;                  /*!< I2S 0 CONTROL REGISTER 1 Register,  Address offset: 0x40 */
	__IO uint32_t RSVD1[3];                               /*!<  Reserved,  Address offset:0x44-0x4C */
	CODEC_ADC_CH_CTRL_TypeDef CODEC_ADC_CH_CTRL[2];       /*!< AUDIO IP have 2 ports,  Address offset: 0x50-0x5C */
	__IO uint32_t RSVD2[19];                              /*!<  Reserved,  Address offset:0x60-0xA8 */
	__IO uint32_t CODEC_ADC_ALIGN_CONTROL_REG_0;          /*!< ADC ALIGN CONTROL REGISTER 0 Register,  Address offset: 0xAC */
	__IO uint32_t CODEC_ADC_ALIGN_CONTROL_REG;            /*!< ADC ALIGN CONTROL REGISTER,  Address offset: 0xB0 */
	__IO uint32_t RSVD3[19];                              /*!<  Reserved,  Address offset:0xB4-0xFC */
	__IO uint32_t CODEC_ADC_x_SILENCE_CONTROL[2];         /*!< ADC CHANNEL x SILENCE CONTROL REGISTER,  Address offset: 0x100-0x104 */
	__IO uint32_t RSVD4[190];                              /*!<  Reserved,  Address offset:0x108-0x3FC */
	__IO uint32_t CODEC_ADC_0_EQ_CTRL;                    /*!< ADC CHANNEL 0 EQ CONTROL REGISTER,  Address offset: 0x400 */
	CODEC_EQ_BAND_TypeDef CODEC_ADC_0_EQ_BAND[5];         /*!< AUDIO IP have 5 ports,  Address offset: 0x404-0x464 */
	__IO uint32_t CODEC_ADC_1_EQ_CTRL;                    /*!< ADC CHANNEL 1 EQ CONTROL REGISTER,  Address offset: 0x468 */
	CODEC_EQ_BAND_TypeDef CODEC_ADC_1_EQ_BAND[5];         /*!< AUDIO IP have 5 ports,  Address offset: 0x46C-0x4CC */
	__IO uint32_t RSVD5[204];                              /*!<  Reserved,  Address offset:0x4D0-0x7FC */
	__I  uint32_t CODEC_ANA_READ;                         /*!< ANA READ REGISTER,  Address offset: 0x800 */
	__I  uint32_t CODEC_ADC_0_LPF_RD;                     /*!< ADC CHANNEL 0 LPF RD REGISTER,  Address offset: 0x804 */
	__I  uint32_t CODEC_ADC_1_LPF_RD;                     /*!< ADC CHANNEL 1 LPF RD REGISTER,  Address offset: 0x808 */
	__IO uint32_t RSVD6[6];                               /*!<  Reserved,  Address offset:0x80C-0x820 */
	__I  uint32_t CODEC_SILENCE_INFORM;                   /*!< SILENCE INFORM REGISTER,  Address offset: 0x824 */
	__I  uint32_t CODEC_AUDIO_RO_DUMMY1;                  /*!< AUDIO RO DUMMY1 REGISTER,  Address offset: 0x828 */
} AUDIO_CODEC_TypeDef;
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

	u32 CODEC_SelI2STxDataFormat;		/*!< Specifies the AUDIO CODEC I2S RX selection of data fornat
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

/** @defgroup AUDIO_CODEC_I2S
  * @{
  */
#define I2S0						    ((u32)0x00000000)
#define IS_CODEC_I2S_SEL(SEL)           ((SEL) == I2S0)
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

/** @defgroup AUDIO_CODEC_Application_Mode
  * @{
  */
#define APP_DMIC_IN			((u32)0x00000002)
#define IS_CODEC_APP_SEL(SEL) ((SEL) == APP_DMIC_IN)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_ADC
  * @{
  */
#define ADC1							((u32)0x00000001)
#define ADC2							((u32)0x00000002)
#define IS_CODEC_ADC_SEL(SEL) (((SEL) == ADC1) || \
								((SEL) == ADC2))
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_ADC_Channel
  * @{
  */
#define ADCHN1							((u32)0x00000001)
#define ADCHN2							((u32)0x00000002)
#define IS_CODEC_ADCHN_SEL(SEL) (((SEL) == ADCHN1) || \
								((SEL) == ADCHN2))
/**
* @}
*/

/** @defgroup AUDIO_CODEC_Power
  * @{
  */
#define NORMALPOWER						((u32)0x00000001)
#define LOWPOWER						((u32)0x00000002)
#define SHUTDOWN						((u32)0x00000003)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_DMIC
  * @{
  */
#define DMIC1							((u32)0x00000001)
#define DMIC2							((u32)0x00000002)
#define IS_CODEC_DMIC_SEL(SEL) (((SEL) == DMIC1) || \
								((SEL) == DMIC2))
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

#define IS_CODEC_DMIC_Clock_SEL(SEL) (((SEL) == DMIC_5M)   || \
								     ((SEL) == DMIC_2P5M)  || \
								     ((SEL) == DMIC_1P25M) || \
								     ((SEL) == DMIC_625K)  || \
								     ((SEL) == DMIC_312P5K)|| \
								     ((SEL) == DMIC_312P5K))
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

/** @defgroup AUDIO_CODEC_Sample_Rate_Source
  * @{
  */
#define SOURCE0			((u32)0x00000000)
#define SOURCE1			((u32)0x00000001)

#define IS_CODEC_Sample_Rate_Source_SEL(SEL)    (((SEL) == SOURCE0) || \
								                ((SEL) == SOURCE1))
/**
  * @}
  */


/** @defgroup AUDIO_CODEC_PATH
  * @{
  */
#define ADCPATH			((u32)0x00000002)
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_ADC_Mix_Select
* @{
*/
#define DMIC			((u32)0x00000001)
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

/** @defgroup AUDIO_CODEC_LDO_Power
  * @{
  */
#define LDO_POWER_ON			((u32)0x00000000)
#define LDO_POWER_DOWN			((u32)0x00000001)
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

/** @defgroup AUDIO_CODEC_Application
  * @{
  */
#define APP_DMIC_RECORD 1
/**
  * @}
  */

/** @defgroup AUDIO_CODEC_Zero_Detection_Function_Select
  * @{
  */
#define IME				((u32)0x00000000)
#define ZDET_IME 		((u32)0x00000001)
#define ZDET_STEP 		((u32)0x00000002)
#define ZDET_TIMEOUT 	((u32)0x00000003)
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
_LONG_CALL_	void AUDIO_CODEC_SetAudioIP(u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetI2SIP(u32 i2s_sel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetI2SSRC(u32 i2s_sel, u32 src);
_LONG_CALL_ void AUDIO_CODEC_SetI2SRXTDM(u32 i2s_sel, u32 tdmmode);
_LONG_CALL_ void AUDIO_CODEC_I2S_StructInit(I2S_InitTypeDef *I2S_Struct);
_LONG_CALL_ void AUDIO_CODEC_SetI2SParameters(u32 i2s_sel, u32 path, I2S_InitTypeDef *I2S_Struct);
_LONG_CALL_ void AUDIO_CODEC_SetADCSRSrc(u32 src, u32 sr);
_LONG_CALL_ void AUDIO_CODEC_EnableADC(u32 ad_chn, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCVolume(u32 adc_sel, u32 gain);
_LONG_CALL_ void AUDIO_CODEC_SetADCHPF(u32 adc_sel, u32 fc, u32 NewState);
_LONG_CALL_ void AUDIO_CODEC_SetADCASRC(u32 i2s_sel, u32 adc_sel, u32 sr, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCMute(u32 adc_sel, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCMixMute(u32 adc_num, u32 type, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetADCDmicFilter(u32 adc_num, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDmicClk(u32 clk, u32 newstate);
_LONG_CALL_ void AUDIO_CODEC_SetDmicSrc(u32 ad_chn, u32 dmic_num);
_LONG_CALL_ void AUDIO_CODEC_SetADCEQClk(u32 ad_chn, u32 NewState);
_LONG_CALL_ void AUDIO_CODEC_SetADCEQFilter(u32 ad_chn, u32 band_sel, CODEC_EQFilterCoef *EQFilterCoefPoint);
_LONG_CALL_ void AUDIO_CODEC_SetADCEQBand(u32 ad_chn, u32 band_sel, u32 NewState);
_LONG_CALL_ void AUDIO_CODEC_SetADCZDET(u32 adc_sel, u32 type);
_LONG_CALL_ void AUDIO_CODEC_SetADCZDETTimeOut(u32 adc_sel, u32 time_out);
_LONG_CALL_ void AUDIO_CODEC_Record(u32 i2s_sel, u32 type, I2S_InitTypeDef *I2S_InitStruct);
_LONG_CALL_ void AUDIO_CODEC_EnableADCFifo(u32 ad_chn, u32 newstate);

/**
  * @}
  */

/* MANUAL_GEN_END */

#endif
/** @} */

/** @} */

/** @} */