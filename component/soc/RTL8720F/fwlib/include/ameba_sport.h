/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SPORT_H_
#define _AMEBA_SPORT_H_

/** @addtogroup AUDIO_SPORT
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * audio sport:
  * 	- Base Address: AUDIO_SPORT0_DEV,AUDIO_SPORT1_DEV,AUDIO_SPORT2_DEV,AUDIO_SPORT3_DEV,
  * 	- Source clk: 40MHz
  * 	- Sample rate: 8/16/32/44.1/48/88.2/96 /11.025/12/22.05 KHz
  * 	- Sample bit: 16 bit, 20 bit,24 bit, 32bit
  * 	- Channel number: mono or stereo
  * 	- Data format: I2S, Left justified, PCM mode A, PCM mode B
  * 	- Use GDMA to move data
  *
  *****************************************************************************************
  * How to use audio sport
  *****************************************************************************************
  *      To use audio codec sport, the following steps are mandatory:
  *
  *      1. Open audio codec clock and function using
  *			PLLx_Set(0, ENABLE); (x is 0 or 1)
  *			RCC_PeriphClockCmd(APBPeriph_AUDIOC, APBPeriph_AUDIOC_CLOCK, ENABLE);
  *			RCC_PeriphClockCmd(APBPeriph_SPORT, APBPeriph_SPORT_CLOCK, ENABLE);
  *
  *      2. AUDIO SPORT pin setting:
  *			PAD_CMD(PinName, DISABLE).
  *
  *      3. Fill the AUDIO_SP_InitStruct with the desired parameters.
  *
  *      4. configure AUDIO SPORT with the corresponding configuration.
  *			AUDIO_SP_Init(AUDIO_SP_DEV, &SP_InitStruct)
  *
  *      5. According to audio codec transfer direction, start Tx or Rx or both path
  *		start Tx path:
  *			AUDIO_SP_DmaCmd(AUDIO_SPORT_DEV, ENABLE);
  *			AUDIO_SP_TxStart(AUDIO_SPORT_DEV, ENABLE);
  *		start Rx path:
  *			AUDIO_SP_DmaCmd(AUDIO_SPORT_DEV, ENABLE);
  *			AUDIO_SP_RxStart(AUDIO_SPORT_DEV, ENABLE);
  *
  *      6. Use AUDIO_SP_TXGDMA_Init or AUDIO_SP_RXGDMA_Init or both function to activate the GDMA according to transfer direction.
  *
  *      @note    All other functions can be used separately to modify, if needed,
  *			a specific feature of the AUDIO SPORT.
  *****************************************************************************************
  * @endverbatim
  */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @addtogroup AUDIO
  * @{
  */
/** @defgroup AUDIO_SPORT
  * @brief AUDIO_SPORT driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup AUDIO_SPORT_Register_Definitions AUDIO_SPORT Register Definitions
  * @{
  */

/** @defgroup SP_TX_DR
  * @brief SPORT Tx data register
  * @{
  */
#define SP_MASK_TX_DR   ((u32)0xFFFFFFFF << 0)           /*!< W 32’h0  It's Tx data window between SW/GDMA and SPORT.*/
#define SP_TX_DR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SP_GET_TX_DR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SP_CTRLR0
  * @brief SPORT control 0 register
  * @{
  */
#define SP_BIT_LONG_FRAME_SYNC    ((u32)0x00000001 << 31)           /*!< R/W 1’b0  1'b0: Short frame sync  1'b1: Long frame sync*/
#define SP_MASK_SEL_I2S_RX_CH     ((u32)0x00000003 << 28)           /*!< R/W 2’b00  2'b00: L/R    2'b01: R/L    2'b10: L/L   2'b11: R/R  x ADC path*/
#define SP_SEL_I2S_RX_CH(x)       (((u32)((x) & 0x00000003) << 28))
#define SP_GET_SEL_I2S_RX_CH(x)   ((u32)(((x >> 28) & 0x00000003)))
#define SP_BIT_START_RX           ((u32)0x00000001 << 25)           /*!< R/W 1’b0  1'b0: Rx is disabled              1'b1: Rx is started*/
#define SP_BIT_RX_DISABLE         ((u32)0x00000001 << 24)           /*!< R/W 1’b1  1'b1: SPORT Rx is disabled    1'b0: SPORT Rx is enabled*/
#define SP_BIT_RX_LSB_FIRST       ((u32)0x00000001 << 23)           /*!< R/W 1’b0  1'b0: MSB first when Rx   1'b1: LSB first*/
#define SP_BIT_TX_LSB_FIRST       ((u32)0x00000001 << 22)           /*!< R/W 1’b0  1'b0: MSB first when Tx  1'b1: LSB first*/
#define SP_MASK_SEL_I2S_TX_CH     ((u32)0x00000003 << 20)           /*!< R/W 2’b00  2'b00: L/R    2'b01: R/L    2'b10: L/L    2'b11: R/R  x DAC path*/
#define SP_SEL_I2S_TX_CH(x)       (((u32)((x) & 0x00000003) << 20))
#define SP_GET_SEL_I2S_TX_CH(x)   ((u32)(((x >> 20) & 0x00000003)))
#define SP_BIT_START_TX           ((u32)0x00000001 << 17)           /*!< R/W 1’b0  1'b0: Tx is disabled  1'b1: Tx is started*/
#define SP_BIT_TX_DISABLE         ((u32)0x00000001 << 16)           /*!< R/W 1’b1  1'b1: SPORT TX is disabled  1'b0: SPORT TX is enabled*/
#define SP_BIT_I2S_SELF_LPBK_EN   ((u32)0x00000001 << 15)           /*!< R/W 1’b0  1'b1: Internal loopback mode is enabled 1'b0: Internal loopback mode is disabled*/
#define SP_BIT_INV_I2S_SCLK       ((u32)0x00000001 << 14)           /*!< R/W 1’b0  1'b1: I2S/PCM bit clock is inverted 1'b0: I2S/PCM bit clock is not inverted*/
#define SP_MASK_DATA_LEN_SEL      ((u32)0x00000003 << 12)           /*!< R/W 2’b00  2'b00: 16 bits    2'b10: 24 bits    2'b11: 8 bits*/
#define SP_DATA_LEN_SEL(x)        (((u32)((x) & 0x00000003) << 12))
#define SP_GET_DATA_LEN_SEL(x)    ((u32)(((x >> 12) & 0x00000003)))
#define SP_BIT_EN_I2S_MONO        ((u32)0x00000001 << 11)           /*!< R/W 1’b0  1'b1: Mono   1'b0: Stereo*/
#define SP_BIT_EN_PCM_N_MODE      ((u32)0x00000001 << 10)           /*!< R/W 1’b0  1'b1: PCM negative mode 1'b0: PCM normal mode*/
#define SP_MASK_DATA_FORMAT_SEL   ((u32)0x00000003 << 8)            /*!< R/W 2’b00  3'b000: I2S                 3'b001: Left-Justified    3'b010: PCM mode A          3'b011: PCM mode B    3'b110: PCM mode A-N         3'b111: PCM mode B-N*/
#define SP_DATA_FORMAT_SEL(x)     (((u32)((x) & 0x00000003) << 8))
#define SP_GET_DATA_FORMAT_SEL(x) ((u32)(((x >> 8) & 0x00000003)))
#define SP_BIT_DSP_CTL_MODE       ((u32)0x00000001 << 7)            /*!< R/W 1’b0  1'b1: DSP and SPORT handshaking is enabled. 1'b0: GDMA and SPORT handshaking is enabled*/
#define SP_BIT_LOOPBACK           ((u32)0x00000001 << 6)            /*!< R/W 1’b0  1'b1: Self loopback mode*/
#define SP_BIT_WCLK_INVERSE       ((u32)0x00000001 << 5)            /*!< R/W 1’b0  1'b1: I2S/PCM word clock is inverted*/
#define SP_BIT_SLAVE_DATA_SEL     ((u32)0x00000001 << 4)            /*!< R/W 1’b0  1'b1: To be an I2S or PCM slave (data path)*/
#define SP_BIT_SLAVE_CLK_SEL      ((u32)0x00000001 << 3)            /*!< R/W 1’b0  1'b1: To be an I2S or PCM slave (CLK path)*/
#define SP_BIT_RX_INV_I2S_SCLK    ((u32)0x00000001 << 2)            /*!< R/W 1’b0  1'b1: SCLK to Rx path (ADC path) is inverted.*/
#define SP_BIT_TX_INV_I2S_SCLK    ((u32)0x00000001 << 1)            /*!< R/W 1’b0  1'b1: SCLK to Tx path (DAC path) is inverted.*/
#define SP_BIT_RESET              ((u32)0x00000001 << 0)            /*!< R/W 1’b0  1'b1: Reset SPORT module, remember to write 1 first and then write 0.*/
/** @} */

/** @defgroup SP_CTRLR1
  * @brief SPORT control 1 register
  * @{
  */
#define SP_MASK_INT_ENABLE          ((u32)0x000000FF << 24)           /*!< R/W 8’h0  bit[24]: For the interrupt of 'SP_READY_TO_TX' bit[25]: For the interrupt of 'SP_READY_TO_RX'*/
#define SP_INT_ENABLE(x)            (((u32)((x) & 0x000000FF) << 24))
#define SP_GET_INT_ENABLE(x)        ((u32)(((x >> 24) & 0x000000FF)))
#define SP_BIT_RX_SNK_LR_SWAP       ((u32)0x00000001 << 23)           /*!< R/W 1’b0  1'b1: Swap L/R audio samples written to the sink memory*/
#define SP_BIT_RX_SNK_BYTE_SWAP     ((u32)0x00000001 << 22)           /*!< R/W 1’b0  1'b1: Swap H/L bytes written to the sink memory*/
#define SP_BIT_TX_SRC_LR_SWAP       ((u32)0x00000001 << 21)           /*!< R/W 1’b0  1'b1: Swap L/R audio samples read from the source memory*/
#define SP_BIT_TX_SRC_BYTE_SWAP     ((u32)0x00000001 << 20)           /*!< R/W 1’b0  1'b1: Swap H/L bytes read from the source memory*/
#define SP_BIT_CLEAR_RX_ERR_CNT     ((u32)0x00000001 << 13)           /*!< R/W 1’b0  Write 1'b1 and then write 1'b0 to clear Rx error counter.*/
#define SP_BIT_CLEAR_TX_ERR_CNT     ((u32)0x00000001 << 12)           /*!< R/W 1’b0  Write 1'b1 and then write 1'b0 to clear Tx error counter.*/
#define SP_MASK_DEBUG_BUS_SEL       ((u32)0x00000007 << 8)            /*!< R/W 3’h0  3'b000: debug_bus_a 3'b001: debug_bus_b ....   3'b111: debug_bus_h*/
#define SP_DEBUG_BUS_SEL(x)         (((u32)((x) & 0x00000007) << 8))
#define SP_GET_DEBUG_BUS_SEL(x)     ((u32)(((x >> 8) & 0x00000007)))
#define SP_MASK_FRAME_SYNC_OFFSET   ((u32)0x000000FF << 0)            /*!< R/W 8’h81  To control the length of 'long_frame_sync' signal when it is ON. It is set to 50% of the period of the sampling rate by default.*/
#define SP_FRAME_SYNC_OFFSET(x)     (((u32)((x) & 0x000000FF) << 0))
#define SP_GET_FRAME_SYNC_OFFSET(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SP_CTRLR2
  * @brief SPORT control 2 register
  * @{
  */
#define SP_BIT_READY_TO_RX            ((u32)0x00000001 << 25) /*!< R 0x0  1: It is ready to receive data (MIC path)*/
#define SP_BIT_READY_TO_TX            ((u32)0x00000001 << 24) /*!< R 0x0  1: It is ready to send data out (SPK path)*/
#define SP_BIT_ENABLE_MCLK            ((u32)0x00000001 << 16) /*!< R/W 1'b0  enable mclk fraction divider, will take precedence over MCLK_SEL settings*/
#define SP_BIT_RX_FIFO_DEPTH_HALF_SEL ((u32)0x00000001 << 5)  /*!< R/W 1’b0  1'b1: Rx FIFO depth will reduce to half. It can reduce the data path latency.*/
#define SP_BIT_TX_FIFO_DEPTH_HALF_SEL ((u32)0x00000001 << 4)  /*!< R/W 1’b0  1'b1: Tx FIFO depth will reduce to half. It can reduce the data path latency.*/
#define SP_BIT_RX_DSP_CLEAR_INT       ((u32)0x00000001 << 1)  /*!< R/W 0x0  For DSP mode (bypass GDMA), F/W writes 1'b1 and then 1'b0 to clear RX interrupt. Note: RX interrupt is to indicate that DSP can get audio data from RX FIFO_0*/
#define SP_BIT_TX_DSP_CLEAR_INT       ((u32)0x00000001 << 0)  /*!< R/W 0x0  For DSP mode (bypass GDMA), F/W writes 1'b1 and then 1'b0 to clear TX interrupt. Note: TX interrupt is to indicate that DSP can write audio data to TX FIFO_0*/
/** @} */

/** @defgroup SP_RX_DR
  * @brief SPORT Rx data register
  * @{
  */
#define SP_MASK_RX_DR   ((u32)0xFFFFFFFF << 0)           /*!< R 32’h0  It's Rx data window between SW/GDMA and SPORT.*/
#define SP_RX_DR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SP_GET_RX_DR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SP_FIFO_SR
  * @brief SPORT FIFO status register
  * @{
  */
#define SP_MASK_RX1_RCNT_BUS   ((u32)0x000000FF << 24)           /*!< R 8’h0  Rx1 FIFO read counter status (MIC path)*/
#define SP_RX1_RCNT_BUS(x)     (((u32)((x) & 0x000000FF) << 24))
#define SP_GET_RX1_RCNT_BUS(x) ((u32)(((x >> 24) & 0x000000FF)))
#define SP_MASK_RX0_RCNT_BUS   ((u32)0x000000FF << 16)           /*!< R 8’h0  Rx0 FIFO read counter status (MIC path)*/
#define SP_RX0_RCNT_BUS(x)     (((u32)((x) & 0x000000FF) << 16))
#define SP_GET_RX0_RCNT_BUS(x) ((u32)(((x >> 16) & 0x000000FF)))
#define SP_MASK_TX1_WCNT_BUS   ((u32)0x000000FF << 8)            /*!< R 8’h0  Tx1 FIFO write counter status (SPK path)*/
#define SP_TX1_WCNT_BUS(x)     (((u32)((x) & 0x000000FF) << 8))
#define SP_GET_TX1_WCNT_BUS(x) ((u32)(((x >> 8) & 0x000000FF)))
#define SP_MASK_TX0_WCNT_BUS   ((u32)0x000000FF << 0)            /*!< R 8’h0  Tx0 FIFO write counter status (SPK path)*/
#define SP_TX0_WCNT_BUS(x)     (((u32)((x) & 0x000000FF) << 0))
#define SP_GET_TX0_WCNT_BUS(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SP_ERROR_CNT_SR
  * @brief SPORT error counter register
  * @{
  */
#define SP_MASK_RX_ERR_CNT   ((u32)0x000000FF << 8)           /*!< R 8’h0  Rx error counter (MIC path) Note: This counter should always be zero if everything works well.*/
#define SP_RX_ERR_CNT(x)     (((u32)((x) & 0x000000FF) << 8))
#define SP_GET_RX_ERR_CNT(x) ((u32)(((x >> 8) & 0x000000FF)))
#define SP_MASK_TX_ERR_CNT   ((u32)0x000000FF << 0)           /*!< R 8’h0  Tx error counter (SPK path) Note: This counter should always be zero if everything works well.*/
#define SP_TX_ERR_CNT(x)     (((u32)((x) & 0x000000FF) << 0))
#define SP_GET_TX_ERR_CNT(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SP_CLK_DIV
  * @brief SPORT frequency synthesizer register
  * @{
  */
#define SP_BIT_MI_NI_UPDATE ((u32)0x00000001 << 31)           /*!< R/W 1’b0  1'b1: to update 'MI' and 'NI' to get the new clock rate. This bit is reset automatically when the update is done.*/
#define SP_MASK_NI          ((u32)0x00007FFF << 16)           /*!< R/W 15’d48  BCLK = 40MHz * (NI/MI)      For example: BCLK = 3.072MHz = 40MHz * (48/625)*/
#define SP_NI(x)            (((u32)((x) & 0x00007FFF) << 16))
#define SP_GET_NI(x)        ((u32)(((x >> 16) & 0x00007FFF)))
#define SP_MASK_MI          ((u32)0x0000FFFF << 0)            /*!< R/W 16’d625  BCLK = 40MHz * (NI/MI)      For example: BCLK = 3.072MHz = 40MHz * (48/625)*/
#define SP_MI(x)            (((u32)((x) & 0x0000FFFF) << 0))
#define SP_GET_MI(x)        ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup SP_MCLK_DIV
  * @brief SPORT mclk frequency synthesizer register
  * @{
  */
#define SP_BIT_MCLK_MI_NI_UPDATE ((u32)0x00000001 << 31)           /*!< R/W 1’b0  1'b1: to update 'MI' and 'NI' to get the new clock rate. This bit is reset automatically when the update is done.*/
#define SP_MASK_MCLK_NI          ((u32)0x00007FFF << 16)           /*!< R/W 15’d192  BCLK = 40MHz * (NI/MI)      For example: BCLK = 12.288MHz = 40MHz * (192/625)*/
#define SP_MCLK_NI(x)            (((u32)((x) & 0x00007FFF) << 16))
#define SP_GET_MCLK_NI(x)        ((u32)(((x >> 16) & 0x00007FFF)))
#define SP_MASK_MCLK_MI          ((u32)0x0000FFFF << 0)            /*!< R/W 16’d625  BCLK = 40MHz * (NI/MI)      For example: BCLK = 12.288MHz = 40MHz * (192/625)*/
#define SP_MCLK_MI(x)            (((u32)((x) & 0x0000FFFF) << 0))
#define SP_GET_MCLK_MI(x)        ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup AUDIO_SPORT_Exported_Types AUDIO_SPORT Exported Types
  * @{
  */

/** @brief AUDIO_SPORT Register Declaration
  */

typedef struct {
	__O  uint32_t SP_TX_DR       ;  /*!< SPORT TX DATA REGISTER,  Address offset:0x000 */
	__IO uint32_t SP_CTRLR0      ;  /*!< SPORT CONTROL 0 REGISTER,  Address offset:0x004 */
	__IO uint32_t SP_CTRLR1      ;  /*!< SPORT CONTROL 1 REGISTER,  Address offset:0x008 */
	__IO uint32_t SP_CTRLR2      ;  /*!< SPORT CONTROL 2 REGISTER,  Address offset:0x00C */
	__I  uint32_t SP_RX_DR       ;  /*!< SPORT RX DATA REGISTER,  Address offset:0x010 */
	__I  uint32_t SP_FIFO_SR     ;  /*!< SPORT FIFO STATUS REGISTER,  Address offset:0x014 */
	__I  uint32_t SP_ERROR_CNT_SR;  /*!< SPORT ERROR COUNTER REGISTER,  Address offset:0x018 */
	__IO uint32_t SP_CLK_DIV     ;  /*!< SPORT FREQUENCY SYNTHESIZER REGISTER,  Address offset:0x01C */
	__IO uint32_t SP_MCLK_DIV    ;  /*!< SPORT MCLK FREQUENCY SYNTHESIZER REGISTER,  Address offset:0x020 */
} AUDIO_SPORT_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


/* Exported types ------------------------------------------------------------*/
/** @addtogroup AUDIO_SPORT_Exported_Types AUDIO_SPORT Exported Types
  * @{
  */

/**
  * @brief	AUDIO_SPORT Register Structure Definition
  */
typedef struct {

	u32 SP_DIR_TX_flag;   /*!< Specifies the AUDIO SPORT TX Direction flag */
	u32 SP_DIR_RX_flag;   /*!< Specifies the AUDIO SPORT RX Direction flag */
	u32 SP_Bclk;  /*!< Specifies the AUDIO SPORT BCLK value */

} SP_RegTypeDef;

/**
* @brief  AUDIO_SPORT Init Structure Definition
*/
typedef struct {

	u32 SP_SelDataFormat; 			/*!< Specifies the AUDIO SPORT data format
									This parameter can be a value of @ref AUDIO_SPORT_Interface_Format */

	u32 SP_SR;				/*!< Specifies the AUDIO SPORT selection of sample rate
									This parameter can be a value of @ref AUDIO_SPORT_Sample_Rate */

	u32 SP_SelCh;			/*!< Specifies the AUDIO SPORT selection channel
									This parameter can be a value of @ref AUDIO_SPORT_Tx_Channel/AUDIO_SPORT_Rx_Channel*/

	u32 SP_SelWordLen;		/*!< Specifies the AUDIO SPORT word length
									This parameter can be a value of @ref AUDIO_SPORT_Word_Length */

	u32 SP_SelI2SMonoStereo;	/*!< Specifies the AUDIO SPORT channel number
									This parameter can be a value of @ref AUDIO_SPORT_Channel_Number */

	u32 SP_SelClk;		   /*AUDIO SPORT clock sources, I2S_CLOCK_XTAL40M/PLL_CLOCK_45P1584M/PLL_CLOCK_98P304M*/

	u32 SP_Bclk;		/*!< Specifies the AUDIO SPORT BCLK */

} SP_InitTypeDef;

/**
  * @}
  */

/* Exported constants ------------------------------------------------------------*/
/** @defgroup AUDIO_SPORT_Exported_Constants AUDIO_SPORT Exported Constants
  * @{
  */

/** @defgroup AUDIO_SPORT_Word_Length
  * @{
  */
#define SP_WL_16							((u32)0x00000000)
#define SP_WL_24							((u32)0x00000002)
#define SP_WL_8							((u32)0x00000003)

#define IS_SP_WORD_LEN(LEN) (((LEN) == SP_WL_16) || \
								((LEN) == SP_WL_24) || \
								((LEN) == SP_WL_8))
/**
  * @}
  */


/** @defgroup AUDIO_SPORT_TDM_Mode
  * @{
  */
#define SP_NOTDM						((u32)0x00000000)

/**
  * @}
  */


/** @defgroup AUDIO_SPORT_Interface_Format
  * @{
  */
#define SP_DF_I2S						((u32)0x00000000)
#define SP_DF_LEFT						((u32)0x00000001)
#define SP_DF_PCM_A						((u32)0x00000002)
#define SP_DF_PCM_B						((u32)0x00000003)
#define SP_DF_PCM_A_N					((u32)0x00000006)
#define SP_DF_PCM_B_N					((u32)0x00000007)

#define IS_SP_DATA_FMT(FORMAT) (((FORMAT) == SP_DF_I2S) || \
											((FORMAT) == SP_DF_LEFT)  || \
											((FORMAT) == SP_DF_PCM_A) || \
											((FORMAT) == SP_DF_PCM_B) || \
                      ((FORMAT) == SP_DF_PCM_A_N) || \
                      ((FORMAT) == SP_DF_PCM_B_N))
/**
  * @}
  */

/** @defgroup AUDIO_SPORT_Channel_Number
  * @{
  */
#define SP_CH_STEREO					((u32)0x00000000)
#define SP_CH_MONO						((u32)0x00000001)

#define IS_SP_CHN_NUM(NUM) (((NUM) == SP_CH_STEREO) || \
								((NUM) == SP_CH_MONO))
/**
  * @}
  */

/** @defgroup AUDIO_SPORT_Rx_Channel
  * @{
  */
#define SP_RX_CH_LR						((u32)0x00000000)
#define SP_RX_CH_RL						((u32)0x00000001)
#define SP_RX_CH_LL						((u32)0x00000002)
#define SP_RX_CH_RR						((u32)0x00000003)

#define IS_SP_SEL_RX_CH(CH) (((CH) == SP_RX_CH_LR) || \
							((CH) == SP_RX_CH_RL) || \
							((CH) == SP_RX_CH_LL) || \
							((CH) == SP_RX_CH_RR))
/**
  * @}
  */

/** @defgroup AUDIO_SPORT_Tx_Channel
  * @{
  */
#define SP_TX_CH_LR						((u32)0x00000000)
#define SP_TX_CH_RL						((u32)0x00000001)
#define SP_TX_CH_LL						((u32)0x00000002)
#define SP_TX_CH_RR						((u32)0x00000003)

#define IS_SP_SEL_TX_CH(CH) (((CH) == SP_TX_CH_LR) || \
							((CH) == SP_TX_CH_RL) || \
							((CH) == SP_TX_CH_LL) || \
							((CH) == SP_TX_CH_RR))
/**
  * @}
  */

/** @defgroup AUDIO_SPORT_GMDA_Selection
  * @{
  */
#define GDMA_INT						((u32)0x00000000)
#define GDMA_EXT							((u32)0x00000001)

#define IS_SP_SEL_GDMA(SEL) (((SEL) == GDMA_INT) || \
									((SEL) == GDMA_EXT))
/**
  * @}
  */

/** @defgroup AUDIO_SPORT_Rx_MultiIO_Mode
  * @{
  */
#define SP_RX_MULTIIO_DIS						((u32)0x00000000)
#define SP_RX_MULTIIO_EN 						((u32)0x00000001)

#define IS_SP_SET_RX_MULTIIO(SEL) (((SEL) == SP_RX_MULTIIO_DIS) || \
											((SEL) == SP_RX_MULTIIO_EN))
/**
  * @}
  */

/** @defgroup AUDIO_SPORT_Application_Mode
* @{
*/
#define SP_DIR_TX			((u32)0x00000001)
#define SP_DIR_RX			((u32)0x00000002)
#define IS_SP_SET_DIR(SEL) (((SEL) == SP_DIR_TX) || \
										  ((SEL) == SP_DIR_RX))
/**
  * @}
  */

/** @defgroup AUDIO_SPORT_I2S_Selection
* @{
*/

#define I2S0	((u32)0x00000000)
#define I2S1	((u32)0x00000001)
#define IS_SP_SEL_I2S(SEL) (((SEL) == I2S0) || \
											((SEL) == I2S1))
/**
* @}
*/

/** @defgroup AUDIO_SPORT_Master_Slave_Mode
* @{
*/
#define MASTER	((u32)0x00000000)
#define SLAVE	((u32)0x00000001)
#define IS_SP_SEL_MODE(SEL) (((SEL) == MASTER) || \
							((SEL) == SLAVE))

/**
* @}
*/

/** @defgroup AUDIO_SPORT_Pinmux_Function_Mode
* @{
*/

#define DOUT0_FUNC	((u32)0x00000000)
#define DOUT1_FUNC	((u32)0x00000001)
#define DOUT2_FUNC	((u32)0x00000002)
#define DOUT3_FUNC	((u32)0x00000003)
#define DIN0_FUNC	((u32)0x00000004)
#define DIN1_FUNC	((u32)0x00000005)
#define DIN2_FUNC	((u32)0x00000006)
#define DIN3_FUNC	((u32)0x00000007)
#define IS_SP_SET_I2S_PINFUNC(SEL) (((SEL) == DOUT0_FUNC) || \
										((SEL) == DOUT1_FUNC) || \
										((SEL) == DOUT2_FUNC) || \
										((SEL) == DOUT3_FUNC) || \
										((SEL) == DIN0_FUNC) || \
										((SEL) == DIN1_FUNC) || \
										((SEL) == DIN2_FUNC) || \
										((SEL) == DIN3_FUNC))
/**
* @}
*/

/** @defgroup AUDIO_SPORT_Channel_Length
* @{
*/
#define SP_CL_32		(32)
/**
  * @}
  */

/** @defgroup AUDIO_SPORT_Sample_Rate
* @{
*/
#define SP_8K			(8000)
#define SP_12K			(12000)
#define SP_24K			(24000)
#define SP_16K			(16000)
#define SP_32K			(32000)
#define SP_48K			(48000)
#define SP_96K			(96000)
#define SP_192K			(192000)
#define SP_384K			(384000)
#define SP_11P025K		(11025)
#define SP_44P1K		(44100)
#define SP_88P2K		(88200)
#define SP_22P05K		(22050)
#define SP_176P4K		(176400)
/**
  * @}
  */

/** @defgroup AUDIO_SPORT_TXChannel
* @{
*/
#define TXCHN0		((u32)0x00000000)
#define TXCHN1		((u32)0x00000001)
#define TXCHN2		((u32)0x00000002)
#define TXCHN3		((u32)0x00000003)
#define TXCHN4		((u32)0x00000004)
#define TXCHN5		((u32)0x00000005)
#define TXCHN6		((u32)0x00000006)
#define TXCHN7		((u32)0x00000007)
/**
  * @}
  */

/** @defgroup AUDIO_SPORT_TXFIFOChannel
* @{
*/
#define TX_FIFO0_REG0_L		((u32)0x00000000)
#define TX_FIFO0_REG0_R		((u32)0x00000001)
#define TX_FIFO0_REG1_L		((u32)0x00000002)
#define TX_FIFO0_REG1_R		((u32)0x00000003)
#define TX_FIFO1_REG0_L		((u32)0x00000004)
#define TX_FIFO1_REG0_R		((u32)0x00000005)
#define TX_FIFO1_REG1_L		((u32)0x00000006)
#define TX_FIFO1_REG1_R		((u32)0x00000007)
#define DIRECT_REG_CHN		((u32)0x00000008)
/**
  * @}
  */

/** @defgroup AUDIO_SPORT_RXChannel
* @{
*/
#define RXCHN0		((u32)0x00000000)
#define RXCHN1		((u32)0x00000001)
#define RXCHN2		((u32)0x00000002)
#define RXCHN3		((u32)0x00000003)
#define RXCHN4		((u32)0x00000004)
#define RXCHN5		((u32)0x00000005)
#define RXCHN6		((u32)0x00000006)
#define RXCHN7		((u32)0x00000007)
/**
  * @}
  */

/** @defgroup AUDIO_SPORT_RXFIFOChannel
* @{
*/
#define RX_FIFO0_REG0_L		((u32)0x00000000)
#define RX_FIFO0_REG0_R		((u32)0x00000001)
#define RX_FIFO0_REG1_L		((u32)0x00000002)
#define RX_FIFO0_REG1_R		((u32)0x00000003)
#define RX_FIFO1_REG0_L		((u32)0x00000004)
#define RX_FIFO1_REG0_R		((u32)0x00000005)
#define RX_FIFO1_REG1_L		((u32)0x00000006)
#define RX_FIFO1_REG1_R		((u32)0x00000007)
/**
  * @}
  */

/** @defgroup AUDIO_SPORT_I2S_FIXED_BCLK_DIV
  * @{
  */
#define DSP_SRC_BCLK_DIV4   ((u32)0x00000000)
#define DSP_SRC_BCLK_DIV2   ((u32)0x00000001)
/**
  * @}
  */

/** @defgroup AUDIO_SPORT_LRCLK_EDGE
  * @{
  */
#define RISING_EDGE   ((u32)0x00000000)
#define FALLING_EDGE   ((u32)0x00000001)
/**
  * @}
  */

/** @defgroup AUDIO_SPORT_DirectOutChannel
* @{
*/
#define DIRECT_OUT_CHN0		((u32)0x00000000)
#define DIRECT_OUT_CHN1		((u32)0x00000001)
#define DIRECT_OUT_CHN2		((u32)0x00000002)
#define DIRECT_OUT_CHN3		((u32)0x00000003)
#define DIRECT_OUT_CHN4		((u32)0x00000004)
#define DIRECT_OUT_CHN5		((u32)0x00000005)
#define DIRECT_OUT_CHN6		((u32)0x00000006)
#define DIRECT_OUT_CHN7		((u32)0x00000007)
/**
  * @}
  */

/** @defgroup AUDIO_SPORT_DirectInChannel
* @{
*/
#define DIRECT_IN_CHN0		((u32)0x00000000)
#define DIRECT_IN_CHN1		((u32)0x00000001)
#define DIRECT_IN_CHN2		((u32)0x00000002)
#define DIRECT_IN_CHN3		((u32)0x00000003)
#define DIRECT_IN_CHN4		((u32)0x00000004)
#define DIRECT_IN_CHN5		((u32)0x00000005)
#define DIRECT_IN_CHN6		((u32)0x00000006)
#define DIRECT_IN_CHN7		((u32)0x00000007)
/**
  * @}
  */

/** @defgroup AUDIO_SPORT_DirectReg
* @{
*/
#define DIRECT_REG_0		((u32)0x00000000)
#define DIRECT_REG_1		((u32)0x00000001)
#define DIRECT_REG_2		((u32)0x00000002)
#define DIRECT_REG_3		((u32)0x00000003)
#define DIRECT_REG_4		((u32)0x00000004)
#define DIRECT_REG_5		((u32)0x00000005)
#define DIRECT_REG_6		((u32)0x00000006)
#define DIRECT_REG_7		((u32)0x00000007)
/**
  * @}
  */

/**
 * @}
 */

/* Exported functions ------------------------------------------------------------*/
/** @defgroup AUDIO_SPORT_Exported_Functions AUDIO_SPORT Exported Functions
  * @{
  */

_LONG_CALL_ void AUDIO_SP_StructInit(SP_InitTypeDef *SP_InitStruct);
_LONG_CALL_ void AUDIO_SP_Init(u32 index, u32 direction, SP_InitTypeDef *SP_InitStruct);
_LONG_CALL_ bool AUDIO_SP_Register(u32 index, u32 direction, SP_InitTypeDef *SP_InitStruct);
_LONG_CALL_ void AUDIO_SP_SetMclk(u32 index, u32 NewState);
_LONG_CALL_ void AUDIO_SP_SetLRClkDiv(u32 index, u32 clock, u32 sr, u32 tdm, u32 chn_len);
_LONG_CALL_ void AUDIO_SP_SetMclkDiv(u32 index, u32 NI, u32 MI);
_LONG_CALL_ void AUDIO_SP_TXStart(u32 index, u32 NewState);
_LONG_CALL_ void AUDIO_SP_RXStart(u32 index, u32 NewState);
_LONG_CALL_ void AUDIO_SP_DmaCmd(u32 index, u32 NewState);
_LONG_CALL_ void AUDIO_SP_SetWordLen(u32 index, u32 SP_WordLen);
_LONG_CALL_ u32 AUDIO_SP_GetWordLen(u32 index);
_LONG_CALL_ void AUDIO_SP_SetMonoStereo(AUDIO_SPORT_TypeDef *SPORTx, u32 SP_MonoStereo);
_LONG_CALL_ void AUDIO_SP_SetMasterSlave(u32 index, u32 SP_MasterSlave);
_LONG_CALL_ bool AUDIO_SP_TXGDMA_Init(u32 Index, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pTxData, u32 Length);
_LONG_CALL_ bool AUDIO_SP_RXGDMA_Init(u32 Index, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pRxData, u32 Length);
_LONG_CALL_ bool AUDIO_SP_TXGDMA_Restart(u8 GDMA_Index, u8 GDMA_ChNum, u32 tx_addr, u32 tx_length);
_LONG_CALL_ bool AUDIO_SP_RXGDMA_Restart(u8 GDMA_Index, u8 GDMA_ChNum, u32 rx_addr, u32 rx_length);
_LONG_CALL_ bool AUDIO_SP_LLPTXGDMA_Init(u32 Index, u32 SelGDMA, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u32 Length,
		u32 MaxLLP, struct GDMA_CH_LLI *Lli);
_LONG_CALL_ bool AUDIO_SP_LLPRXGDMA_Init(u32 Index, u32 SelGDMA, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u32 Length,
		u32 MaxLLP, struct GDMA_CH_LLI *Lli);
/**
  * @}
  */

/** @} */

/** @} */

/** @} */

/* Other Definitions --------------------------------------------------------*/
typedef struct {
	AUDIO_SPORT_TypeDef *SPORTx;
	u32 Tx_HandshakeInterface;
	u32 Rx_HandshakeInterface;
} AUDIO_DevTable;

extern const AUDIO_DevTable AUDIO_DEV_TABLE[1];
#define AUDIO_BLOCK_SIZE 2048

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif