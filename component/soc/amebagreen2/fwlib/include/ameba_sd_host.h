/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _RL7005_SDIOHOST_H_
#define _RL7005_SDIOHOST_H_

/* AUTO_GEN_START */

/** @addtogroup Ameba_Periph_Driver
* @{
*/

/** @defgroup SDIOHOST
* @brief SDIOHOST driver modules
* @{
*/

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup SDIOHOST_Register_Definitions SDIOHOST Register Definitions
 * @{
*/
/** @defgroup SDIOHOST_SRAM_CTL
 * @brief
 * @{
 */
#define SDIOHOST_MASK_BUF_READ_SIZE     ((u32)0x00000003 << 24)           /*!<R/W 'b10  Buffer Data Port Read Size (BDPRS)  2'b00: 1-byte access, port_a_rd_ptr increment by 1  2'b01: 2-byte access, port_a_rd_ptr increment by 2  2'b10: 4-byte access, port_a_rd_ptr increment by 4  2'b11: decoding byte enable(BE[3:0]) bus signal  If block-size is odd-byte, please set buf_read_size=2'b00*/
#define SDIOHOST_BUF_READ_SIZE(x)       (((u32)((x) & 0x00000003) << 24))
#define SDIOHOST_GET_BUF_READ_SIZE(x)   ((u32)(((x >> 24) & 0x00000003)))
#define SDIOHOST_MASK_AHBM_HPROT_CFG    ((u32)0x0000000F << 20)           /*!<R/W 'b0001  AHB bus Protection control*/
#define SDIOHOST_AHBM_HPROT_CFG(x)      (((u32)((x) & 0x0000000F) << 20))
#define SDIOHOST_GET_AHBM_HPROT_CFG(x)  ((u32)(((x >> 20) & 0x0000000F)))
#define SDIOHOST_MASK_AHBM_HBURST_CFG   ((u32)0x0000000F << 16)           /*!<R/W 'b1111  bit[3]: ena_incrx_align: alignment Burst Type enable   bit[2]: ena_inc16: INCR16 Burst Type Enable  bit[1]: ena_inc8: INCR8 Burst Type Enable  bit[0]: ena_inc4: INCR4 Burst Type Enable*/
#define SDIOHOST_AHBM_HBURST_CFG(x)     (((u32)((x) & 0x0000000F) << 16))
#define SDIOHOST_GET_AHBM_HBURST_CFG(x) ((u32)(((x >> 16) & 0x0000000F)))
#define SDIOHOST_BIT_LX_SWAP            ((u32)0x00000001 << 10)           /*!<R/W 'b0  Reserved*/
#define SDIOHOST_MASK_LX_BURST_SIZE     ((u32)0x00000003 << 8)            /*!<R/W 'b10  2'b11: Reserved  2'b10:burst length = 256 bytes (64 Dwords)  2'b01: burst length = 128 bytes (32 Dwords)  2'b00: burst length = 64 bytes (16 Dwords)*/
#define SDIOHOST_LX_BURST_SIZE(x)       (((u32)((x) & 0x00000003) << 8))
#define SDIOHOST_GET_LX_BURST_SIZE(x)   ((u32)(((x >> 8) & 0x00000003)))
#define SDIOHOST_MASK_MCU_BUF_ACCESS    ((u32)0x00000003 << 1)            /*!<R/W 'h0  in bit[0] = 1 condition,   bit[2:1] = 'b00 : buf access disable.  bit[2:1] = 'b01 : buf abs access enable, ring buffer address is direct mapping to 0x4005_CB00 ~ 0x4005_CEFF. (mcu_addr[15:0]=0000~3FF, mcu_cs_buf_absolute)  bit[2:1] = 'b10: buf auto access enable, ring buffer could be access by register (mcu_addr=FE30~FE33, mcu_cs_buf_auto)  0x4005_CB30 (sram data [7:0]),  0x4005_CB31 (sram data [15:8]),  0x4005_CB32 (sram data [23:16]),  0x4005_CB33 (sram data [31:24]),  address will auto increase by 4.  bit[2:1] = 'b11 : buf access disable.*/
#define SDIOHOST_MCU_BUF_ACCESS(x)      (((u32)((x) & 0x00000003) << 1))
#define SDIOHOST_GET_MCU_BUF_ACCESS(x)  ((u32)(((x >> 1) & 0x00000003)))
#define SDIOHOST_BIT_MCU_SEL            ((u32)0x00000001 << 0)            /*!<R/W 'b0  0: elbi I/F (dword access for SDIO host standard register)  1: mcu I/F (byte access for SDIO IP register)*/
/** @} */

/** @defgroup SDIOHOST_CTL
 * @brief
 * @{
 */
#define SDIOHOST_BIT_IP_ENDIAN_SEL       ((u32)0x00000001 << 4) /*!<R/W 'b0  0:Little; 1:Big*/
#define SDIOHOST_BIT_L4_GATED_DISABLE_IP ((u32)0x00000001 << 3) /*!<R/W 'b0  Disable L4 clock gated for ip*/
#define SDIOHOST_BIT_DBUS_ENDIAN_SEL     ((u32)0x00000001 << 2) /*!<R/W 'b0  0: Little; 1: Big*/
#define SDIOHOST_BIT_L4_GATED_DISABLE    ((u32)0x00000001 << 1) /*!<R/W 'b0  Disable L4 clock gated*/
#define SDIOHOST_BIT_SUSPEND_N           ((u32)0x00000001 << 0) /*!<R/W 'b1  suspend, low active*/
/** @} */

/** @defgroup SDIOHOST_DBG_1
 * @brief
 * @{
 */
#define SDIOHOST_BIT_WRITE_ENABLE3 ((u32)0x00000001 << 9)           /*!<W -  Write enable for bit[8..6]*/
#define SDIOHOST_MASK_SEL1         ((u32)0x00000007 << 6)           /*!<R/W 'h0  Select control of dbg_sel1.*/
#define SDIOHOST_SEL1(x)           (((u32)((x) & 0x00000007) << 6))
#define SDIOHOST_GET_SEL1(x)       ((u32)(((x >> 6) & 0x00000007)))
#define SDIOHOST_BIT_WRITE_ENABLE2 ((u32)0x00000001 << 5)           /*!<W -  Write enable for bit[4..2]*/
#define SDIOHOST_MASK_SEL0         ((u32)0x00000007 << 2)           /*!<R/W 'h0  Select control of dbg_sel0.*/
#define SDIOHOST_SEL0(x)           (((u32)((x) & 0x00000007) << 2))
#define SDIOHOST_GET_SEL0(x)       ((u32)(((x >> 2) & 0x00000007)))
#define SDIOHOST_BIT_WRITE_ENABLE1 ((u32)0x00000001 << 1)           /*!<W -  Write enable for bit0.*/
#define SDIOHOST_BIT_ENABLE        ((u32)0x00000001 << 0)           /*!<R/W 'b0  Debug Enable. If set to 1, the debug port will be switched to the selected probed signals for observation.  If clear to 0 (default), the scpu_dbg_out0 and scpu_dbg_out1 are both static at h0.*/
/** @} */

/** @defgroup SDIOHOST_DBG_2
 * @brief
 * @{
 */
#define SDIOHOST_MASK_DBUS_DBG_SEL   ((u32)0x00000007 << 0)           /*!<R/W 'h0  sdio dbus dbg select*/
#define SDIOHOST_DBUS_DBG_SEL(x)     (((u32)((x) & 0x00000007) << 0))
#define SDIOHOST_GET_DBUS_DBG_SEL(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup SDIOHOST_DBG_3
 * @brief
 * @{
 */
#define SDIOHOST_MASK_IP_DBG_SEL   ((u32)0x000000FF << 0)           /*!<R/W 'h0  sdio ip dbg select*/
#define SDIOHOST_IP_DBG_SEL(x)     (((u32)((x) & 0x000000FF) << 0))
#define SDIOHOST_GET_IP_DBG_SEL(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SDIOHOST_ISR
 * @brief
 * @{
 */
#define SDIOHOST_BIT_INT4       ((u32)0x00000001 << 4) /*!<R/W 'b0  SDIO Int4. SDIO IP Int.*/
#define SDIOHOST_BIT_INT3       ((u32)0x00000001 << 3) /*!<R/W 'b0  SD Int3.SB1 wlast/rlast.*/
#define SDIOHOST_BIT_INT1       ((u32)0x00000001 << 1) /*!<R/W 'b0  SDIO Int1. DMA done.*/
#define SDIOHOST_BIT_WRITE_DATA ((u32)0x00000001 << 0) /*!<W -  1 to set, 0 to clear bit with 1.*/
/** @} */

/** @defgroup SDIOHOST_ISREN
 * @brief
 * @{
 */
#define SDIOHOST_BIT_INT4EN     ((u32)0x00000001 << 4) /*!<R/W 'b0  SDIO Int4 Enable IP Int Enable*/
#define SDIOHOST_BIT_INT3EN     ((u32)0x00000001 << 3) /*!<R/W 'b0  SDIO Int3 Enable SB1 wlast/rlast Int Enable*/
#define SDIOHOST_BIT_INT1EN     ((u32)0x00000001 << 1) /*!<R/W 'b0  SDIO Int1 Enable Dma done Int Enable*/
#define SDIOHOST_BIT_WRITE_DATA ((u32)0x00000001 << 0) /*!<W -  1 to set, 0 to clear bit with 1.*/
/** @} */

/** @defgroup SDIOHOST_CKGEN_CTL
 * @brief
 * @{
 */
#define SDIOHOST_BIT_SD30_SAMPLE_CHANGE ((u32)0x00000001 << 18) /*!<R/W 'b0  0: from sd30_sample_clk_src 1: clk*/
#define SDIOHOST_BIT_SD30_PUSH_CHANGE   ((u32)0x00000001 << 17) /*!<R/W 'b0  0: from sd30_push_clk_src 1: clk*/
#define SDIOHOST_BIT_CRC_CLK_CHANGE     ((u32)0x00000001 << 16) /*!<R/W 'b0  0: from crc_clk_src 1: clk*/
/** @} */

/** @defgroup SDIOHOST_DMA_RST
 * @brief
 * @{
 */
#define SDIOHOST_MASK_DMABUS_INFO      ((u32)0x000000FF << 16)           /*!<R 'h0  {1'b0, dma_state[2:0], 1'b0, dbus_state[2:0]} for debug*/
#define SDIOHOST_DMABUS_INFO(x)        (((u32)((x) & 0x000000FF) << 16))
#define SDIOHOST_GET_DMABUS_INFO(x)    ((u32)(((x >> 16) & 0x000000FF)))
#define SDIOHOST_MASK_CUR_PE_DMA_FSM   ((u32)0x000000FF << 8)            /*!<R 'h01  {1'b0, cur_pe_dma_fsm} for debug*/
#define SDIOHOST_CUR_PE_DMA_FSM(x)     (((u32)((x) & 0x000000FF) << 8))
#define SDIOHOST_GET_CUR_PE_DMA_FSM(x) ((u32)(((x >> 8) & 0x000000FF)))
#define SDIOHOST_BIT_DMA_RST_ACK_CTL   ((u32)0x00000001 << 3)            /*!<R/W 'b0  1'b0: dma acknowledge soft reset after dbus_req done  1'b1: dma always acknowledge soft reset regardless of dbus_req*/
#define SDIOHOST_BIT_DMA_STOP          ((u32)0x00000001 << 2)            /*!<R/W 'b0  Set this bit to stop dma,  (suggest running dma_stop before doing software reset to avoid DMA hang)*/
#define SDIOHOST_BIT_DMA_CFG           ((u32)0x00000001 << 1)            /*!<R/W 'b0  Fix software Reset bug on the sdio_dbus module  1'b0: don't soft reset pe_tx_leng_re/pe_rx_leng_re  1'b1: soft reset pe_tx_leng_rx/pe_rx_leng_re*/
#define SDIOHOST_BIT_DMA_RSTN          ((u32)0x00000001 << 0)            /*!<R/W 'b1  dma soft reset.*/
/** @} */

/** @defgroup SDIOHOST_CLK_DET_PLLSDIO0
 * @brief
 * @{
 */
#define SDIOHOST_BIT_CLKDET_DONE_PLLSDIO     ((u32)0x00000001 << 30)           /*!<R 'h0  (Reserved)*/
#define SDIOHOST_MASK_CLK_COUNT_PLLSDIO      ((u32)0x0001FFFF << 13)           /*!<R 'h0  (Reserved)*/
#define SDIOHOST_CLK_COUNT_PLLSDIO(x)        (((u32)((x) & 0x0001FFFF) << 13))
#define SDIOHOST_GET_CLK_COUNT_PLLSDIO(x)    ((u32)(((x >> 13) & 0x0001FFFF)))
#define SDIOHOST_MASK_REFCLK_COUNT_PLLSDIO   ((u32)0x000007FF << 2)            /*!<R 'h0  (Reserved)*/
#define SDIOHOST_REFCLK_COUNT_PLLSDIO(x)     (((u32)((x) & 0x000007FF) << 2))
#define SDIOHOST_GET_REFCLK_COUNT_PLLSDIO(x) ((u32)(((x >> 2) & 0x000007FF)))
#define SDIOHOST_BIT_SSC_USABLE_SDIO         ((u32)0x00000001 << 0)            /*!<R 'h0  (Reserved)*/
/** @} */

/** @defgroup SDIOHOST_CLK_DET_PLLSDIO1
 * @brief
 * @{
 */
#define SDIOHOST_BIT_REG_COUNT_EN_PLLSDIO ((u32)0x00000001 << 1) /*!<R/W 'h0  (Reserved)*/
#define SDIOHOST_BIT_REG_RSTN_PLLSDIO     ((u32)0x00000001 << 0) /*!<R/W 'h0  (Reserved)*/
/** @} */

/** @defgroup SDIOHOST_SDIO_IP_BIST_CTL
 * @brief
 * @{
 */
#define SDIOHOST_BIT_BIST_LS_HW_SEL        ((u32)0x00000001 << 11)          /*!<R/W 'b1  (Reserved)*/
#define SDIOHOST_BIT_BIST_RING_BUF_DS_0    ((u32)0x00000001 << 10)          /*!<R/W 'b0  (Reserved)*/
#define SDIOHOST_BIT_BIST_RING_BUF_SD_0    ((u32)0x00000001 << 9)           /*!<R/W 'b0  (Reserved)*/
#define SDIOHOST_BIT_BIST_RING_BUF_RME_0   ((u32)0x00000001 << 8)           /*!<R/W 'b0  (Reserved)*/
#define SDIOHOST_MASK_BIST_RING_BUF_RM_0   ((u32)0x0000000F << 4)           /*!<R/W 'h0  (Reserved)*/
#define SDIOHOST_BIST_RING_BUF_RM_0(x)     (((u32)((x) & 0x0000000F) << 4))
#define SDIOHOST_GET_BIST_RING_BUF_RM_0(x) ((u32)(((x >> 4) & 0x0000000F)))
#define SDIOHOST_BIT_BIST_LS               ((u32)0x00000001 << 0)           /*!<R/W 'b0  (Reserved)*/
/** @} */

/** @defgroup SDIOHOST_SDIO_MUX_CTL
 * @brief
 * @{
 */
#define SDIOHOST_BIT_CMD_USE_AFIFO_SYNC         ((u32)0x00000001 << 30) /*!<RW 0x1  CMD use AFIFO sync*/
#define SDIOHOST_BIT_V_TUNING_ERR               ((u32)0x00000001 << 29) /*!<RW 0x0  U Tuning error*/
#define SDIOHOST_BIT_DAT_TOUT_CLK_OPT           ((u32)0x00000001 << 28) /*!<RW 0x0  Data Tout clk option*/
#define SDIOHOST_BIT_CARD_INT_PERIOD_EN         ((u32)0x00000001 << 27) /*!<RW 0x1  Card int period enable*/
#define SDIOHOST_BIT_READ_WAIT_NO_STOP_SDCLK_EN ((u32)0x00000001 << 26) /*!<R 0x0  1:no stop SD clock when read wait 0:stop SD clock when read wait*/
/** @} */

/** @defgroup SDIOHOST_SDIO_BUS_TA_TIME
 * @brief
 * @{
 */
#define SDIOHOST_MASK_STOP_CLK_CTRL_CYCLE    ((u8)0x00000007 << 4)           /*!<R/W 3’d2  stop_clk_ctrl_cycle Stop SDCLK some clocks ahead before receiving the end of the read-block  3'd0: no advance 3'd1:advance 1 clock cycle  3'd2: advance 2 clock cycles The other value more than 2: And so on*/
#define SDIOHOST_STOP_CLK_CTRL_CYCLE(x)      (((u8)((x) & 0x00000007) << 4))
#define SDIOHOST_GET_STOP_CLK_CTRL_CYCLE(x)  ((u8)(((x >> 4) & 0x00000007)))
#define SDIOHOST_BIT_SDIO_BUS_TA_TIME_SET    ((u8)0x00000001 << 3)           /*!<R/W 0x0  Turn-Around Time enable  When this bit is b1, Bus_TA_Time(bit[2:0]) is in use. Otherwise HW will determine the bus turn-around time automatically.*/
#define SDIOHOST_MASK_SDIO_BUS_TA_TIME_CFG   ((u8)0x00000007 << 0)           /*!<R/W 0x03  Bus_TA_Time  These bits are used to define the number of SDCLK periods that HW should wait for SD bus turn-around time.   After sending the end bit of command, HW will wait at least Bus_TA_Time to start to receive the start bit of response from card.  After sending the end bit of write block, HW will also wait at least Bus_TA_Time to start to receive the start bit of CRC status from card.*/
#define SDIOHOST_SDIO_BUS_TA_TIME_CFG(x)     (((u8)((x) & 0x00000007) << 0))
#define SDIOHOST_GET_SDIO_BUS_TA_TIME_CFG(x) ((u8)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup SDIOHOST_MMC_SDIO_MODE
 * @brief
 * @{
 */
#define SDIOHOST_BIT_MMC_MODE     ((u8)0x00000001 << 3) /*!<RO 0x0  MMC Mode*/
#define SDIOHOST_BIT_SDIO_SUPPORT ((u8)0x00000001 << 2) /*!<RO 0x0  SDIO Support*/
#define SDIOHOST_BIT_USE_DDR_TUNE ((u8)0x00000001 << 1) /*!<R/W 0x0  Use DDR Tuning*/
#define SDIOHOST_BIT_USE_TX_TUNE  ((u8)0x00000001 << 0) /*!<R/W 0x1  Tuning*/
/** @} */

/** @defgroup SDIOHOST_RX_BEST_PHASE
 * @brief
 * @{
 */
#define SDIOHOST_MASK_RX_BEST_PHASE   ((u8)0x0000001F << 0)           /*!<RO 0x0  rx_best_phase*/
#define SDIOHOST_RX_BEST_PHASE(x)     (((u8)((x) & 0x0000001F) << 0))
#define SDIOHOST_GET_RX_BEST_PHASE(x) ((u8)(((x >> 0) & 0x0000001F)))
/** @} */

/** @defgroup SDIOHOST_TX_BEST_PHASE
 * @brief
 * @{
 */
#define SDIOHOST_MASK_TX_BEST_PHASE   ((u8)0x0000001F << 0)           /*!<RO 0x0  rx_best_phase*/
#define SDIOHOST_TX_BEST_PHASE(x)     (((u8)((x) & 0x0000001F) << 0))
#define SDIOHOST_GET_TX_BEST_PHASE(x) ((u8)(((x >> 0) & 0x0000001F)))
/** @} */

/** @defgroup SDIOHOST_RX_PHASE_0
 * @brief
 * @{
 */
#define SDIOHOST_MASK_RX_PHASE_RESULT_7_0   ((u8)0x000000FF << 0)           /*!<RO 0x0  rx_phase_result 0*/
#define SDIOHOST_RX_PHASE_RESULT_7_0(x)     (((u8)((x) & 0x000000FF) << 0))
#define SDIOHOST_GET_RX_PHASE_RESULT_7_0(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SDIOHOST_RX_PHASE_1
 * @brief
 * @{
 */
#define SDIOHOST_MASK_RX_PHASE_RESULT_15_8   ((u8)0x000000FF << 0)           /*!<RO 0x0  rx_phase_result 1*/
#define SDIOHOST_RX_PHASE_RESULT_15_8(x)     (((u8)((x) & 0x000000FF) << 0))
#define SDIOHOST_GET_RX_PHASE_RESULT_15_8(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SDIOHOST_RX_PHASE_2
 * @brief
 * @{
 */
#define SDIOHOST_MASK_RX_PHASE_RESULT_23_16   ((u8)0x000000FF << 0)           /*!<RO 0x0  rx_phase_result 2*/
#define SDIOHOST_RX_PHASE_RESULT_23_16(x)     (((u8)((x) & 0x000000FF) << 0))
#define SDIOHOST_GET_RX_PHASE_RESULT_23_16(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SDIOHOST_RX_PHASE_3
 * @brief
 * @{
 */
#define SDIOHOST_MASK_RX_PHASE_RESULT_31_24   ((u8)0x000000FF << 0)           /*!<RO 0x0  rx_phase_result 3*/
#define SDIOHOST_RX_PHASE_RESULT_31_24(x)     (((u8)((x) & 0x000000FF) << 0))
#define SDIOHOST_GET_RX_PHASE_RESULT_31_24(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SDIOHOST_TX_PHASE_0
 * @brief
 * @{
 */
#define SDIOHOST_MASK_TX_PHASE_RESULT_7_0   ((u8)0x000000FF << 0)           /*!<RO 0x0  tx_phase_result 0*/
#define SDIOHOST_TX_PHASE_RESULT_7_0(x)     (((u8)((x) & 0x000000FF) << 0))
#define SDIOHOST_GET_TX_PHASE_RESULT_7_0(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SDIOHOST_TX_PHASE_1
 * @brief
 * @{
 */
#define SDIOHOST_MASK_TX_PHASE_RESULT_15_8   ((u8)0x000000FF << 0)           /*!<RO 0x0  tx_phase_result 1*/
#define SDIOHOST_TX_PHASE_RESULT_15_8(x)     (((u8)((x) & 0x000000FF) << 0))
#define SDIOHOST_GET_TX_PHASE_RESULT_15_8(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SDIOHOST_TX_PHASE_2
 * @brief
 * @{
 */
#define SDIOHOST_MASK_TX_PHASE_RESULT_23_16   ((u8)0x000000FF << 0)           /*!<RO 0x0  tx_phase_result 2*/
#define SDIOHOST_TX_PHASE_RESULT_23_16(x)     (((u8)((x) & 0x000000FF) << 0))
#define SDIOHOST_GET_TX_PHASE_RESULT_23_16(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SDIOHOST_TX_PHASE_3
 * @brief
 * @{
 */
#define SDIOHOST_MASK_TX_PHASE_RESULT_31_24   ((u8)0x000000FF << 0)           /*!<RO 0x0  tx_phase_result 3*/
#define SDIOHOST_TX_PHASE_RESULT_31_24(x)     (((u8)((x) & 0x000000FF) << 0))
#define SDIOHOST_GET_TX_PHASE_RESULT_31_24(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SDIOHOST_PHASE_RST_TM_0
 * @brief
 * @{
 */
#define SDIOHOST_MASK_PHASE_RST_TIME_CFG_7_0   ((u8)0x000000FF << 0)           /*!<RW 0x0  phase_rst_time_cfg low*/
#define SDIOHOST_PHASE_RST_TIME_CFG_7_0(x)     (((u8)((x) & 0x000000FF) << 0))
#define SDIOHOST_GET_PHASE_RST_TIME_CFG_7_0(x) ((u8)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup SDIOHOST_PHASE_RST_TM_1
 * @brief
 * @{
 */
#define SDIOHOST_MASK_PHASE_RST_TIME_CFG_11_8   ((u8)0x0000000F << 0)           /*!<RW 0x1  phase_rst_time_cfg high*/
#define SDIOHOST_PHASE_RST_TIME_CFG_11_8(x)     (((u8)((x) & 0x0000000F) << 0))
#define SDIOHOST_GET_PHASE_RST_TIME_CFG_11_8(x) ((u8)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup SDIOHOST_SDIO_DAT_CMD
 * @brief
 * @{
 */
#define SDIOHOST_BIT_SDIO_DAT_DUTY_EN        ((u8)0x00000001 << 7)           /*!<RW 0x0  sdio_dat_duty_en*/
#define SDIOHOST_MASK_SDIO_DAT_DLY_DISABLE   ((u8)0x00000007 << 4)           /*!<RW 0xb0  sdio_dat_dly_disable*/
#define SDIOHOST_SDIO_DAT_DLY_DISABLE(x)     (((u8)((x) & 0x00000007) << 4))
#define SDIOHOST_GET_SDIO_DAT_DLY_DISABLE(x) ((u8)(((x >> 4) & 0x00000007)))
#define SDIOHOST_BIT_SDIO_CMD_DUTY_EN        ((u8)0x00000001 << 3)           /*!<RW 0x0  sdio_cmd_duty_en*/
#define SDIOHOST_MASK_SDIO_CMD_DLY_DISABLE   ((u8)0x00000007 << 0)           /*!<RW 0xb0  sdio_cmd_dly_disable*/
#define SDIOHOST_SDIO_CMD_DLY_DISABLE(x)     (((u8)((x) & 0x00000007) << 0))
#define SDIOHOST_GET_SDIO_CMD_DLY_DISABLE(x) ((u8)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup SDIOHOST_TUNING_DUTY_CTRL
 * @brief
 * @{
 */
#define SDIOHOST_BIT_SDIO_STOP_CLK_EN     ((u8)0x00000001 << 4) /*!<RW 0x0  Stop SDCLK enable*/
#define SDIOHOST_BIT_CLR_INTER_CLK_STABLE ((u8)0x00000001 << 3) /*!<RW 0x0  Clear Internal CLK Stable*/
#define SDIOHOST_BIT_RXWAIT_AFIFO_RST_EN  ((u8)0x00000001 << 2) /*!<RW 0x0  Rx wait AFIFO Reset enable*/
#define SDIOHOST_BIT_USE_RX_DUTY_CTRL     ((u8)0x00000001 << 1) /*!<RW 0x0  Use RX Duty Control*/
#define SDIOHOST_BIT_USE_TX_DUTY_CTRL     ((u8)0x00000001 << 0) /*!<RW 0x0  Use TX Duty Control*/
/** @} */

/** @defgroup SDIOHOST_TUNING_SOLUTION
 * @brief
 * @{
 */
#define SDIOHOST_BIT_SDIO_SDCLK_POLAR_SWAP   ((u8)0x00000001 << 6) /*!<RW 0x0  SDIO SDCLK polarity swap*/
#define SDIOHOST_BIT_SDIO_DDR_FW_SOLUTION    ((u8)0x00000001 << 5) /*!<RW 0x0  SDIO DDR Digital solution*/
#define SDIOHOST_BIT_SDIO_DDR_DIG_SOLUTION   ((u8)0x00000001 << 4) /*!<RW 0x0  SDIO DDR FW solution*/
#define SDIOHOST_BIT_SDIO_DDR_PUSH_TYPE      ((u8)0x00000001 << 3) /*!<RW 0x1  SDIO DDR Push Type*/
#define SDIOHOST_BIT_SDIO_DDR_DAT_PUSH_POINT ((u8)0x00000001 << 2) /*!<RW 0x0  SDIO DDR DAT Push Point  0: set-up time: 1/4 SDCLK cycle; hold time: 1/4 SDCLK cycle, sd20_dato (trigged by crc_clk rising edge)  1: set-up time: 1/2 SDCLK cycle; hold time: 0 SDCLK cycle, sd30_dato (trigged by crc_clk falling edge)*/
#define SDIOHOST_BIT_SDIO_DDR_CMD_PUSH_POINT ((u8)0x00000001 << 1) /*!<RW 0x1  SDIO DDR CMD Push Point  0: output at falling edge of SDCLK(set-up time: 1/2 SDCLK cycle; hold time: 1/2 SDCLK cycle), sd30_cmdo (trigged by crc_clk falling edge)  1: output is ahead by 1/4 SDCLK period(set-up time: 3/4 SDCLK cycle; hold time: 1/4 SDCLK cycle), sd20_cmdo (trigged by crc_clk rising edge)*/
#define SDIOHOST_BIT_SDIO_SD20_PUSH_POINT    ((u8)0x00000001 << 0) /*!<RW 0x0  SDIO SD20 Push Point  0: output at falling edge of SDCLK(set-up time: 1/2 SDCLK cycle; hold time: 1/2 SDCLK cycle), sd20_cmdo/sd20_dato (trigged by crc_clk rising edge)  1: output is ahead by 1/4 SDCLK period(set-up time: 3/4 SDCLK cycle; hold time: 1/4 SDCLK cycle), sd30_cmdo/sd30_dato (trigged by crc_clk falling edge)*/
/** @} */

/** @defgroup SDIOHOST_SD_SAMPLE_POINT_CTL
 * @brief
 * @{
 */
#define SDIOHOST_BIT_SD20_SAMP_POINT ((u8)0x00000001 << 3) /*!<R/W 0x0  Sample Point Selection for SD20 CMD / DATA  0: sample at rising edge of SDCLK  1: sample point is delayed by 1/4 SDCLK period*/
/** @} */

/** @defgroup SDIOHOST_HOST_DMABUF_CTL
 * @brief
 * @{
 */
#define SDIOHOST_BIT_BUF_FLUSH_CTL_PIO ((u16)0x00000001 << 6)           /*!<R/W 0x1  buf_flush_ctl_pio (2020/10/12)  0: don't flush PCIE_RING_BUF and reset rd_ptr/wr_ptr at PIO mode xxxxxx 1: auto-flush PCIE_RING_BUF and reset rd_prt/wr_ptr after each block transfer complete at PIO Mode*/
#define SDIOHOST_BIT_BUF_FLUSH_CTL     ((u16)0x00000001 << 5)           /*!<R/W 0x1  buf_flush_ctl (2020/09/04)  0: don't flush PCIE_RING_BUF and reset rd_ptr/wr_ptr at DMA mode  1: auto-flush PCIE_RING_BUF and reset rd_ptr/wr_ptr after each SDIO data transfer (to fix the same sram address read-write collision) at DMA Mode*/
#define SDIOHOST_BIT_BUF_ROOM_VLD_CTL  ((u16)0x00000001 << 4)           /*!<R/W 0x1  buf_room_vld_ctl (2020/09/01)  0: byte-level buf_room_vld control  1: Qword-level(8-byte) buf_room_vld control to fix the PCIE_RING_BUF bug when ADMA length not equal to the multiple of 8 (to fix the same sram address read-write collision)*/
#define SDIOHOST_MASK_AUTO_CMD23_CTL   ((u16)0x0000000F << 0)           /*!<R/W 0x0  Auto Cmd23 error control 0: auto cmd23 response error stop next command transfer 1:auto cmd23 response error doesn't stop next command transfer*/
#define SDIOHOST_AUTO_CMD23_CTL(x)     (((u16)((x) & 0x0000000F) << 0))
#define SDIOHOST_GET_AUTO_CMD23_CTL(x) ((u16)(((x >> 0) & 0x0000000F)))
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_SDMA_SYS_ADDR
 * @brief  SDMA System Address / Argument 2 Register (@ 0XC00)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_SDMA_SYS_ADDR                 0
#define SDIOHOST_MASK_SDMA_SYS_ADDR                  ((u32)0xFFFFFFFF << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_BLK_SZ
 * @brief  Block Size Register                       (@ 0XC04)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_HOST_SDMA_BUFF_BOUNDARY       12
#define SDIOHOST_MASK_HOST_SDMA_BUFF_BOUNDARY        ((u32)0x00000007 << 12)                             /*!<R/W 0   */
#define SDIOHOST_SHIFT_XFER_BLK_SZ                   0
#define SDIOHOST_MASK_XFER_BLK_SZ                    ((u32)0x00000FFF << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_BLK_CNT
 * @brief  Block Count Register                      (@ 0XC06)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_BLK_CNT_CURR                  0
#define SDIOHOST_MASK_BLK_CNT_CURR                   ((u32)0x0000FFFF << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_CMD_ARG1
 * @brief  Argument 1 Register                       (@ 0XC08)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_CMD_ARG1                      0
#define SDIOHOST_MASK_CMD_ARG1                       ((u32)0xFFFFFFFF << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_XFER_MODE
 * @brief   Transfer Mode Register                   (@ 0XC0C)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_MULTI_SINGLE_BLK_SEL          5
#define SDIOHOST_BIT_MULTI_SINGLE_BLK_SEL            ((u32)0x00000001 << 5)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_XFER_DIR_SEL                  4
#define SDIOHOST_BIT_XFER_DIR_SEL                    ((u32)0x00000001 << 4)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_AUTO_CMD_EN                   2
#define SDIOHOST_MASK_AUTO_CMD_EN                    ((u32)0x00000003 << 2)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_BLK_CNT_EN                    1
#define SDIOHOST_BIT_BLK_CNT_EN                      ((u32)0x00000001 << 1)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_DMA_EN                        0
#define SDIOHOST_BIT_DMA_EN                          ((u32)0x00000001 << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_CMD
 * @brief  Command Register                          (@ 0XC0E)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_CMD_IDX                       8
#define SDIOHOST_MASK_CMD_IDX                        ((u32)0x0000003F << 8)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CMD_TYPE                      6
#define SDIOHOST_MASK_CMD_TYPE                       ((u32)0x00000003 << 6)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_DATA_PRESENT_SEL              5
#define SDIOHOST_BIT_DATA_PRESENT_SEL                ((u32)0x00000001 << 5)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CMD_IDX_CHK_EN                4
#define SDIOHOST_BIT_CMD_IDX_CHK_EN                  ((u32)0x00000001 << 4)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CMD_CRC_CHK_EN                3
#define SDIOHOST_BIT_CMD_CRC_CHK_EN                  ((u32)0x00000001 << 3)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_RESP_TYPE_SEL                 0
#define SDIOHOST_MASK_RESP_TYPE_SEL                  ((u32)0x00000003 << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_RESP0
 * @brief  Response Register                         (@ 0XC10)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_CMD_RESP                      0
#define SDIOHOST_MASK_CMD_RESP                       ((u32)0xFFFFFFFF << 0)                              /*!<RO 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_RESP1
 * @brief  Response Register                         (@ 0XC14)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_CMD_RESP                      0
#define SDIOHOST_MASK_CMD_RESP                       ((u32)0xFFFFFFFF << 0)                              /*!<RO 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_RESP2
 * @brief  Response Register                         (@ 0XC18)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_CMD_RESP                      0
#define SDIOHOST_MASK_CMD_RESP                       ((u32)0xFFFFFFFF << 0)                              /*!<RO 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_RESP3
 * @brief  Response Register                         (@ 0XC1C)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_CMD_RESP                      0
#define SDIOHOST_MASK_CMD_RESP                       ((u32)0xFFFFFFFF << 0)                              /*!<RO 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_BUFFER_PORT_DATA
 * @brief   Buffer Data Port Register                (@ 0XC20)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_BUFF_DATA                     0
#define SDIOHOST_MASK_BUFF_DATA                      ((u32)0xFFFFFFFF << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_PRESENT_STATE
 * @brief  ADC Debug Select Register                 (@ 0XC24)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_CMD_LINE_SIG_LVL              24
#define SDIOHOST_BIT_CMD_LINE_SIG_LVL                ((u32)0x00000001 << 24)                             /*!<RO 0   */
#define SDIOHOST_SHIFT_DAT_LINE_SIG_LVL              20
#define SDIOHOST_MASK_DAT_LINE_SIG_LVL               ((u32)0x0000000F << 20)                             /*!<RO 0   */
#define SDIOHOST_SHIFT_WP_SWITCH_PIN_LVL             19
#define SDIOHOST_BIT_WP_SWITCH_PIN_LVL               ((u32)0x00000001 << 19)                             /*!<RO 0   */
#define SDIOHOST_SHIFT_CD_PIN_LEVEL                  18
#define SDIOHOST_BIT_CD_PIN_LEVEL                    ((u32)0x00000001 << 18)                             /*!<RO 0   */
#define SDIOHOST_SHIFT_CARD_STATE_STABLE             17
#define SDIOHOST_BIT_CARD_STATE_STABLE               ((u32)0x00000001 << 17)                             /*!<RO 0   */
#define SDIOHOST_SHIFT_CARD_INSERTED                 16
#define SDIOHOST_BIT_CARD_INSERTED                   ((u32)0x00000001 << 16)                             /*!<RO 0   */
#define SDIOHOST_SHIFT_BUFF_READ_EN                  11
#define SDIOHOST_BIT_BUFF_READ_EN                    ((u32)0x00000001 << 11)                             /*!<RO 0   */
#define SDIOHOST_SHIFT_BUFF_WRITE_EN                 10
#define SDIOHOST_BIT_BUFF_WRITE_EN                   ((u32)0x00000001 << 10)                             /*!<RO 0   */
#define SDIOHOST_SHIFT_READ_XFER_ACTIVE              9
#define SDIOHOST_BIT_READ_XFER_ACTIVE                ((u32)0x00000001 << 9)                              /*!<RO 0   */
#define SDIOHOST_SHIFT_WRITE_XFER_ACTIVE             8
#define SDIOHOST_BIT_WRITE_XFER_ACTIVE               ((u32)0x00000001 << 8)                              /*!<RO 0   */
#define SDIOHOST_SHIFT_RETUNE_REQ                    3
#define SDIOHOST_BIT_RETUNE_REQ                      ((u32)0x00000001 << 3)                              /*!<RO 0   */
#define SDIOHOST_SHIFT_DAT_LINE_ACTIVE               2
#define SDIOHOST_BIT_DAT_LINE_ACTIVE                 ((u32)0x00000001 << 2)                              /*!<RO 0   */
#define SDIOHOST_SHIFT_CMD_INHISHIFT_DAT             1
#define SDIOHOST_BIT_CMD_INHIBIT_DAT                 ((u32)0x00000001 << 1)                              /*!<RO 0   */
#define SDIOHOST_SHIFT_CMD_INHISHIFT_CMD             0
#define SDIOHOST_BIT_CMD_INHIBIT_CMD                 ((u32)0x00000001 << 0)                              /*!<RO 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_HOST_CTL_1_REG
 * @brief  Host Control 1 Register                   (@ 0XC28)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_CD_SIG_SEL                    7
#define SDIOHOST_BIT_CD_SIG_SEL                      ((u32)0x00000001 << 7)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CD_TEST_LVL                   6
#define SDIOHOST_BIT_CD_TEST_LVL                     ((u32)0x00000001 << 6)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_EXTENDED_DATA_XFER_WIDTH      5
#define SDIOHOST_BIT_EXTENDED_DATA_XFER_WIDTH        ((u32)0x00000001 << 5)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_DMA_SEL                       3
#define SDIOHOST_MASK_DMA_SEL                        ((u32)0x00000003 << 3)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_HS_EN                         2
#define SDIOHOST_BIT_HS_EN                           ((u32)0x00000001 << 2)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_DATA_XFER_WIDTH               1
#define SDIOHOST_BIT_DATA_XFER_WIDTH                 ((u32)0x00000001 << 1)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_LED_CTL                       0
#define SDIOHOST_BIT_LED_CTL                         ((u32)0x00000001 << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_PWR_CTL
 * @brief  Power Control Register                    (@ 0XC29)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_SD_BUS_VOLTAGE_SEL            1
#define SDIOHOST_MASK_SD_BUS_VOLTAGE_SEL             ((u32)0x00000007 << 1)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_SD_BUS_PWR                    0
#define SDIOHOST_BIT_SD_BUS_PWR                      ((u32)0x00000001 << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_BLK_GAP_CTL
 * @brief  Block Gap Control Register                (@ 0XC2A)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_INT_AT_BLK_GAP                3
#define SDIOHOST_BIT_INT_AT_BLK_GAP                  ((u32)0x00000001 << 3)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_READ_WAIT_CTL                 2
#define SDIOHOST_BIT_READ_WAIT_CTL                   ((u32)0x00000001 << 2)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CONT_REQ                      1
#define SDIOHOST_BIT_CONT_REQ                        ((u32)0x00000001 << 1)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_STOP_AT_BLK_REQ               0
#define SDIOHOST_BIT_STOP_AT_BLK_REQ                 ((u32)0x00000001 << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_WAKEUP_CTL
 * @brief  Wakeup Control Register                   (@ 0XC2B)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_WAKEUP_EVENT_CARD_REMOVE_EN   2
#define SDIOHOST_BIT_WAKEUP_EVENT_CARD_REMOVE_EN     ((u32)0x00000001 << 2)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_WAKEUP_EVENT_CARD_INSERT_EN   1
#define SDIOHOST_BIT_WAKEUP_EVENT_CARD_INSERT_EN     ((u32)0x00000001 << 1)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_WAKEUP_EVENT_CARD_INT_EN      0
#define SDIOHOST_BIT_WAKEUP_EVENT_CARD_INT_EN        ((u32)0x00000001 << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_CLK_CTL
 * @brief   Clock Control Register                   (@ 0XC2C)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_SDCLK_FREQ_SEL                8
#define SDIOHOST_MASK_SDCLK_FREQ_SEL                 ((u32)0x000000FF << 8)                              /*!<R/W 0    */
#define SDIOHOST_SHIFT_SDCLK_FREQ_SEL_UPPER          6
#define SDIOHOST_MASK_SDCLK_FREQ_SEL_UPPER           ((u32)0x00000003 << 6)                              /*!<R/W 0    */
#define SDIOHOST_SHIFT_CLK_GEN_SEL                   5
#define SDIOHOST_BIT_CLK_GEN_SEL                     ((u32)0x00000001 << 5)                              /*!<R/W 0    */
#define SDIOHOST_SHIFT_SDCLK_EN                      2
#define SDIOHOST_BIT_SDCLK_EN                        ((u32)0x00000001 << 2)                              /*!<R/W 0    */
#define SDIOHOST_SHIFT_INTERNAL_CLK_STABLE           1
#define SDIOHOST_BIT_INTERNAL_CLK_STABLE             ((u32)0x00000001 << 1)                              /*!<RO 0   */
#define SDIOHOST_SHIFT_INTERNAL_CLK_EN               0
#define SDIOHOST_BIT_INTERNAL_CLK_EN                 ((u32)0x00000001 << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_TIMEOUT_CTL
 * @brief  Timeout Control Register                  (@ 0XC2E)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_DATA_TIMEOUT_CNT_VAL          0
#define SDIOHOST_MASK_DATA_TIMEOUT_CNT_VAL           ((u32)0x0000000F << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_SW_RST
 * @brief  Software Reset Register                   (@ 0XC2F)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_SW_RST_DAT                    2
#define SDIOHOST_BIT_SW_RST_DAT                      ((u32)0x00000001 << 2)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_SW_RST_CMD                    1
#define SDIOHOST_BIT_SW_RST_CMD                      ((u32)0x00000001 << 1)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_SW_RST_ALL                    0
#define SDIOHOST_BIT_SW_RST_ALL                      ((u32)0x00000001 << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_NORMAL_INT_STATUS
 * @brief  Normal Interrupt Status Register          (@ 0XC30)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_ERR_INT                       15
#define SDIOHOST_BIT_ERR_INT                         ((u32)0x00000001 << 15)                             /*!<RO 0   */
#define SDIOHOST_SHIFT_RETUNING_EVENT                12
#define SDIOHOST_BIT_RETUNING_EVENT                  ((u32)0x00000001 << 12)                             /*!<RO 0   */
#define SDIOHOST_SHIFT_INT_C                         11
#define SDIOHOST_BIT_INT_C                           ((u32)0x00000001 << 11)                             /*!<RO 0   */
#define SDIOHOST_SHIFT_INT_B                         10
#define SDIOHOST_BIT_INT_B                           ((u32)0x00000001 << 10)                             /*!<RO 0   */
#define SDIOHOST_SHIFT_INT_A                         9
#define SDIOHOST_BIT_INT_A                           ((u32)0x00000001 << 9)                              /*!<RO 0   */
#define SDIOHOST_SHIFT_CARD_INT                      8
#define SDIOHOST_BIT_CARD_INT                        ((u32)0x00000001 << 8)                              /*!<RO 0   */
#define SDIOHOST_SHIFT_CARD_REMOVAL                  7
#define SDIOHOST_BIT_CARD_REMOVAL                    ((u32)0x00000001 << 7)                              /*!<R/W1C 0   */
#define SDIOHOST_SHIFT_CARD_INSERTION                6
#define SDIOHOST_BIT_CARD_INSERTION                  ((u32)0x00000001 << 6)                              /*!<R/W1C 0   */
#define SDIOHOST_SHIFT_BUFF_READ_READY               5
#define SDIOHOST_BIT_BUFF_READ_READY                 ((u32)0x00000001 << 5)                              /*!<R/W1C 0   */
#define SDIOHOST_SHIFT_BUFF_WRITE_READY              4
#define SDIOHOST_BIT_BUFF_WRITE_READY                ((u32)0x00000001 << 4)                              /*!<R/W1C 0   */
#define SDIOHOST_SHIFT_DMA_INT                       3
#define SDIOHOST_BIT_DMA_INT                         ((u32)0x00000001 << 3)                              /*!<R/W1C 0   */
#define SDIOHOST_SHIFT_BLK_GAP_EVENT                 2
#define SDIOHOST_BIT_BLK_GAP_EVENT                   ((u32)0x00000001 << 2)                              /*!<R/W1C 0   */
#define SDIOHOST_SHIFT_XFER_COMPLETE                 1
#define SDIOHOST_BIT_XFER_COMPLETE                   ((u32)0x00000001 << 1)                              /*!<R/W1C 0   */
#define SDIOHOST_SHIFT_CMD_COMPLETE                  0
#define SDIOHOST_BIT_CMD_COMPLETE                    ((u32)0x00000001 << 0)                              /*!<R/W1C 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_ERR_INT_STATUS
 * @brief  Error Interrupt Status Register           (@ 0XC32)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_VENDOR_SPECIFIC_ERR           12
#define SDIOHOST_MASK_VENDOR_SPECIFIC_ERR            ((u32)0x0000000F << 12)                             /*!<R/W1C 0   */
#define SDIOHOST_SHIFT_TUNING_ERR                    10
#define SDIOHOST_BIT_TUNING_ERR                      ((u32)0x00000001 << 10)                             /*!<R/W1C 0   */
#define SDIOHOST_SHIFT_ADMA_ERR                      9
#define SDIOHOST_BIT_ADMA_ERR                        ((u32)0x00000001 << 9)                              /*!<R/W1C 0   */
#define SDIOHOST_SHIFT_AUTO_CMD_ERR                  8
#define SDIOHOST_BIT_AUTO_CMD_ERR                    ((u32)0x00000001 << 8)                              /*!<R/W1C 0   */
#define SDIOHOST_SHIFT_CURRENT_LIMIT_ERR             7
#define SDIOHOST_BIT_CURRENT_LIMIT_ERR               ((u32)0x00000001 << 7)                              /*!<R/W1C 0   */
#define SDIOHOST_SHIFT_DATA_END_SHIFT_ERR            6
#define SDIOHOST_BIT_DATA_END_BIT_ERR                ((u32)0x00000001 << 6)                              /*!<R/W1C 0   */
#define SDIOHOST_SHIFT_DATA_CRC_ERR                  5
#define SDIOHOST_BIT_DATA_CRC_ERR                    ((u32)0x00000001 << 5)                              /*!<R/W1C 0   */
#define SDIOHOST_SHIFT_DATA_TIMEOUT_ERR              4
#define SDIOHOST_BIT_DATA_TIMEOUT_ERR                ((u32)0x00000001 << 4)                              /*!<R/W1C 0   */
#define SDIOHOST_SHIFT_CMD_IDX_ERR                   3
#define SDIOHOST_BIT_CMD_IDX_ERR                     ((u32)0x00000001 << 3)                              /*!<R/W1C 0   */
#define SDIOHOST_SHIFT_CMD_END_SHIFT_ERR             2
#define SDIOHOST_BIT_CMD_END_BIT_ERR                 ((u32)0x00000001 << 2)                              /*!<R/W1C 0   */
#define SDIOHOST_SHIFT_CMD_CRC_ERR                   1
#define SDIOHOST_BIT_CMD_CRC_ERR                     ((u32)0x00000001 << 1)                              /*!<R/W1C 0   */
#define SDIOHOST_SHIFT_CMD_TIMEOUT_ERR               0
#define SDIOHOST_BIT_CMD_TIMEOUT_ERR                 ((u32)0x00000001 << 0)                              /*!<R/W1C 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_NORMAL_INT_STATUS_EN
 * @brief  Normal Interrupt Status Enable Register   (@ 0XC34)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_FIXED_TO_0                    15
#define SDIOHOST_BIT_FIXED_TO_0                      ((u32)0x00000001 << 15)                             /*!<RO 0   */
#define SDIOHOST_SHIFT_RETUNE_EVENT_STATUS_EN        12
#define SDIOHOST_BIT_RETUNE_EVENT_STATUS_EN          ((u32)0x00000001 << 12)                             /*!<R/W 0   */
#define SDIOHOST_SHIFT_INT_C_STATUS_EN               11
#define SDIOHOST_BIT_INT_C_STATUS_EN                 ((u32)0x00000001 << 11)                             /*!<R/W 0   */
#define SDIOHOST_SHIFT_INT_B_STATUS_EN               10
#define SDIOHOST_BIT_INT_B_STATUS_EN                 ((u32)0x00000001 << 10)                             /*!<R/W 0   */
#define SDIOHOST_SHIFT_INT_A_STATUS_EN               9
#define SDIOHOST_BIT_INT_A_STATUS_EN                 ((u32)0x00000001 << 9)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CARD_INT_STATUS_EN            8
#define SDIOHOST_BIT_CARD_INT_STATUS_EN              ((u32)0x00000001 << 8)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CARD_REMOVAL_STATUS_EN        7
#define SDIOHOST_BIT_CARD_REMOVAL_STATUS_EN          ((u32)0x00000001 << 7)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CARD_INSERT_STATUS_EN         6
#define SDIOHOST_BIT_CARD_INSERT_STATUS_EN           ((u32)0x00000001 << 6)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_BUFF_READ_READY_STATUS_EN     5
#define SDIOHOST_BIT_BUFF_READ_READY_STATUS_EN       ((u32)0x00000001 << 5)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_BUFF_WRITE_READY_STATUS_EN    4
#define SDIOHOST_BIT_BUFF_WRITE_READY_STATUS_EN      ((u32)0x00000001 << 4)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_DMA_INT_STATUS_EN             3
#define SDIOHOST_BIT_DMA_INT_STATUS_EN               ((u32)0x00000001 << 3)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_BLK_GAP_EVENT_STATUS_EN       2
#define SDIOHOST_BIT_BLK_GAP_EVENT_STATUS_EN         ((u32)0x00000001 << 2)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_XFER_COMPLETE_STATUS_EN       1
#define SDIOHOST_BIT_XFER_COMPLETE_STATUS_EN         ((u32)0x00000001 << 1)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CMD_COMPLETE_STATUS_EN        0
#define SDIOHOST_BIT_CMD_COMPLETE_STATUS_EN          ((u32)0x00000001 << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_ERR_INT_STATUS_EN
 * @brief   Error Interrupt Status Enable Register   (@ 0XC36)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_VENDOR_SPECIFIC_ERR_STATUS_EN 12
#define SDIOHOST_MASK_VENDOR_SPECIFIC_ERR_STATUS_EN  ((u32)0x0000000F << 12)                             /*!<R/W 0   */
#define SDIOHOST_SHIFT_TUNING_ERR_STATUS_EN          10
#define SDIOHOST_BIT_TUNING_ERR_STATUS_EN            ((u32)0x00000001 << 10)                             /*!<R/W 0   */
#define SDIOHOST_SHIFT_ADMA_ERR_STATUS_EN            9
#define SDIOHOST_BIT_ADMA_ERR_STATUS_EN              ((u32)0x00000001 << 9)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_AUTO_CMD_ERR_STATUS_EN        8
#define SDIOHOST_BIT_AUTO_CMD_ERR_STATUS_EN          ((u32)0x00000001 << 8)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CURRENT_LIMIT_ERR_STATUS_EN   7
#define SDIOHOST_BIT_CURRENT_LIMIT_ERR_STATUS_EN     ((u32)0x00000001 << 7)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_DATA_END_SHIFT_ERR_STATUS_EN  6
#define SDIOHOST_BIT_DATA_END_BIT_ERR_STATUS_EN      ((u32)0x00000001 << 6)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_DATA_CRC_ERR_STATUS_EN        5
#define SDIOHOST_BIT_DATA_CRC_ERR_STATUS_EN          ((u32)0x00000001 << 5)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_DATA_TIMEOUT_ERR_STATUS_EN    4
#define SDIOHOST_BIT_DATA_TIMEOUT_ERR_STATUS_EN      ((u32)0x00000001 << 4)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CMD_IDX_ERR_STATUS_EN         3
#define SDIOHOST_BIT_CMD_IDX_ERR_STATUS_EN           ((u32)0x00000001 << 3)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CMD_END_SHIFT_ERR_STATUS_EN   2
#define SDIOHOST_BIT_CMD_END_BIT_ERR_STATUS_EN       ((u32)0x00000001 << 2)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CMD_CRC_ERR_STATUS_EN         1
#define SDIOHOST_BIT_CMD_CRC_ERR_STATUS_EN           ((u32)0x00000001 << 1)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CMD_TIMEOUT_ERR_STATUS_EN     0
#define SDIOHOST_BIT_CMD_TIMEOUT_ERR_STATUS_EN       ((u32)0x00000001 << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_NORMAL_SIG_EN
 * @brief  Normal Interrupt Signal Enable Register   (@ 0XC38)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_FIXED_TO_0                    15
#define SDIOHOST_BIT_FIXED_TO_0                      ((u32)0x00000001 << 15)                             /*!<RO 0   */
#define SDIOHOST_SHIFT_RETUNE_EVENT_SIGNAL_EN        12
#define SDIOHOST_BIT_RETUNE_EVENT_SIGNAL_EN          ((u32)0x00000001 << 12)                             /*!<R/W 0   */
#define SDIOHOST_SHIFT_INT_C_STATUS_SIGNAL_EN        11
#define SDIOHOST_BIT_INT_C_STATUS_SIGNAL_EN          ((u32)0x00000001 << 11)                             /*!<R/W 0   */
#define SDIOHOST_SHIFT_INT_B_STATUS_SIGNAL_EN        10
#define SDIOHOST_BIT_INT_B_STATUS_SIGNAL_EN          ((u32)0x00000001 << 10)                             /*!<R/W 0   */
#define SDIOHOST_SHIFT_INT_A_SIGNAL_EN               9
#define SDIOHOST_BIT_INT_A_SIGNAL_EN                 ((u32)0x00000001 << 9)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CARD_INT_SIGNAL_EN            8
#define SDIOHOST_BIT_CARD_INT_SIGNAL_EN              ((u32)0x00000001 << 8)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CARD_REMOVAL_SIGNAL_EN        7
#define SDIOHOST_BIT_CARD_REMOVAL_SIGNAL_EN          ((u32)0x00000001 << 7)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CARD_INSERT_SIGNAL_EN         6
#define SDIOHOST_BIT_CARD_INSERT_SIGNAL_EN           ((u32)0x00000001 << 6)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_BUFF_READ_READY_SIGNAL_EN     5
#define SDIOHOST_BIT_BUFF_READ_READY_SIGNAL_EN       ((u32)0x00000001 << 5)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_BUFF_WRITE_READY_SIGNAL_EN    4
#define SDIOHOST_BIT_BUFF_WRITE_READY_SIGNAL_EN      ((u32)0x00000001 << 4)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_DMA_INT_SIGNAL_EN             3
#define SDIOHOST_BIT_DMA_INT_SIGNAL_EN               ((u32)0x00000001 << 3)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_BLK_GAP_EVENT_SIGNAL_EN       2
#define SDIOHOST_BIT_BLK_GAP_EVENT_SIGNAL_EN         ((u32)0x00000001 << 2)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_XFER_COMPLETE_SIGNAL_EN       1
#define SDIOHOST_BIT_XFER_COMPLETE_SIGNAL_EN         ((u32)0x00000001 << 1)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CMD_COMPLETE_SIGNAL_EN        0
#define SDIOHOST_BIT_CMD_COMPLETE_SIGNAL_EN          ((u32)0x00000001 << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_ERR_SIG_EN
 * @brief  Error Interrupt Signal Enable Register    (@ 0XC3A)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_VENDOR_SPECIFIC_ERR_SIGNAL_EN 12
#define SDIOHOST_MASK_VENDOR_SPECIFIC_ERR_SIGNAL_EN  ((u32)0x0000000F << 12)                             /*!<R/W 0   */
#define SDIOHOST_SHIFT_TUNING_ERR_SIGNAL_EN          10
#define SDIOHOST_BIT_TUNING_ERR_SIGNAL_EN            ((u32)0x00000001 << 10)                             /*!<R/W 0   */
#define SDIOHOST_SHIFT_ADMA_ERR_SIGNAL_EN            9
#define SDIOHOST_BIT_ADMA_ERR_SIGNAL_EN              ((u32)0x00000001 << 9)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_AUTO_CMD_ERR_SIGNAL_EN        8
#define SDIOHOST_BIT_AUTO_CMD_ERR_SIGNAL_EN          ((u32)0x00000001 << 8)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CURRENT_LIMIT_ERR_SIGNAL_EN   7
#define SDIOHOST_BIT_CURRENT_LIMIT_ERR_SIGNAL_EN     ((u32)0x00000001 << 7)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_DATA_END_SHIFT_ERR_SIGNAL_EN  6
#define SDIOHOST_BIT_DATA_END_BIT_ERR_SIGNAL_EN      ((u32)0x00000001 << 6)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_DATA_CRC_ERR_SIGNAL_EN        5
#define SDIOHOST_BIT_DATA_CRC_ERR_SIGNAL_EN          ((u32)0x00000001 << 5)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_DATA_TIMEOUT_ERR_SIGNAL_EN    4
#define SDIOHOST_BIT_DATA_TIMEOUT_ERR_SIGNAL_EN      ((u32)0x00000001 << 4)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CMD_IDX_ERR_SIGNAL_EN         3
#define SDIOHOST_BIT_CMD_IDX_ERR_SIGNAL_EN           ((u32)0x00000001 << 3)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CMD_END_SHIFT_ERR_SIGNAL_EN   2
#define SDIOHOST_BIT_CMD_END_BIT_ERR_SIGNAL_EN       ((u32)0x00000001 << 2)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CMD_CRC_ERR_SIGNAL_EN         1
#define SDIOHOST_BIT_CMD_CRC_ERR_SIGNAL_EN           ((u32)0x00000001 << 1)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_CMD_TIMEOUT_ERR_SIGNAL_EN     0
#define SDIOHOST_BIT_CMD_TIMEOUT_ERR_SIGNAL_EN       ((u32)0x00000001 << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_AUTO_CMD_ERR_STATUS
 * @brief  Auto CMD Error Status Register            (@ 0XC3C)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_CMD_NOT_ISSUED_ERR            7
#define SDIOHOST_BIT_CMD_NOT_ISSUED_ERR              ((u32)0x00000001 << 7)                              /*!<RO 0   */
#define SDIOHOST_SHIFT_IDX_ERR                       4
#define SDIOHOST_BIT_IDX_ERR                         ((u32)0x00000001 << 4)                              /*!<RO 0   */
#define SDIOHOST_SHIFT_END_SHIFT_ERR                 3
#define SDIOHOST_BIT_END_BIT_ERR                     ((u32)0x00000001 << 3)                              /*!<RO 0   */
#define SDIOHOST_SHIFT_CRC_ERR                       2
#define SDIOHOST_BIT_CRC_ERR                         ((u32)0x00000001 << 2)                              /*!<RO 0   */
#define SDIOHOST_SHIFT_TIMEOUT_ERR                   1
#define SDIOHOST_BIT_TIMEOUT_ERR                     ((u32)0x00000001 << 1)                              /*!<RO 0   */
#define SDIOHOST_SHIFT_NOT_EXE                       0
#define SDIOHOST_BIT_NOT_EXE                         ((u32)0x00000001 << 0)                              /*!<RO 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_HOST_CTL
 * @brief  Host Control 2 Register                   (@ 0XC3E)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_PRESET_VAL_EN                 15
#define SDIOHOST_BIT_PRESET_VAL_EN                   ((u32)0x00000001 << 15)                             /*!<R/W 0   */
#define SDIOHOST_SHIFT_ASYNC_INT_EN                  14
#define SDIOHOST_BIT_ASYNC_INT_EN                    ((u32)0x00000001 << 14)                             /*!<R/W 0   */
#define SDIOHOST_SHIFT_SAMP_CLK_SEL                  7
#define SDIOHOST_BIT_SAMP_CLK_SEL                    ((u32)0x00000001 << 7)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_EXEC_TUNING                   6
#define SDIOHOST_BIT_EXEC_TUNING                     ((u32)0x00000001 << 6)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_DRV_STR_SEL                   4
#define SDIOHOST_MASK_DRV_STR_SEL                    ((u32)0x00000003 << 4)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_1_8V_SIG_EN                   3
#define SDIOHOST_BIT_1_8V_SIG_EN                     ((u32)0x00000001 << 3)                              /*!<R/W 0   */
#define SDIOHOST_SHIFT_UHS_MODE_EN                   0
#define SDIOHOST_MASK_UHS_MODE_EN                    ((u32)0x00000007 << 0)                              /*!<R/W 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_CAPABILITIES0
 * @brief  Capabilities Register                     (@ 0XC40)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_SLOT_TYPE                     30
#define SDIOHOST_MASK_SLOT_TYPE                      ((u32)0x00000003 << 30)                             /*!<R 0   */
#define SDIOHOST_SHIFT_ASYNC_INT_SUPPORT             29
#define SDIOHOST_BIT_ASYNC_INT_SUPPORT               ((u32)0x00000001 << 29)                             /*!<R 0   */
#define SDIOHOST_SHIFT_64_SHIFT_SYS_BUS_SUPPORT      28
#define SDIOHOST_BIT_64_BIT_SYS_BUS_SUPPORT          ((u32)0x00000001 << 28)                             /*!<R 0   */
#define SDIOHOST_SHIFT_1_8V_SUPPORT                  26
#define SDIOHOST_BIT_1_8V_SUPPORT                    ((u32)0x00000001 << 26)                             /*!<R 0   */
#define SDIOHOST_SHIFT_3_0V_SUPPORT                  25
#define SDIOHOST_BIT_3_0V_SUPPORT                    ((u32)0x00000001 << 25)                             /*!<R 0   */
#define SDIOHOST_SHIFT_3_3V_SUPPORT                  24
#define SDIOHOST_BIT_3_3V_SUPPORT                    ((u32)0x00000001 << 24)                             /*!<R 0   */
#define SDIOHOST_SHIFT_SUSPEND_RESUME_SUPPORT        23
#define SDIOHOST_BIT_SUSPEND_RESUME_SUPPORT          ((u32)0x00000001 << 23)                             /*!<R 0   */
#define SDIOHOST_SHIFT_SDMA_SUPPORT                  22
#define SDIOHOST_BIT_SDMA_SUPPORT                    ((u32)0x00000001 << 22)                             /*!<R 0   */
#define SDIOHOST_SHIFT_HS_SUPPORT                    21
#define SDIOHOST_BIT_HS_SUPPORT                      ((u32)0x00000001 << 21)                             /*!<R 0   */
#define SDIOHOST_SHIFT_ADMA2_SUPPORT                 19
#define SDIOHOST_BIT_ADMA2_SUPPORT                   ((u32)0x00000001 << 19)                             /*!<R 0   */
#define SDIOHOST_SHIFT_8SHIFT_SUPPORT                18
#define SDIOHOST_BIT_8BIT_SUPPORT                    ((u32)0x00000001 << 18)                             /*!<R 0   */
#define SDIOHOST_SHIFT_MAX_BLK_LEN                   16
#define SDIOHOST_MASK_MAX_BLK_LEN                    ((u32)0x00000003 << 16)                             /*!<R 0   */
#define SDIOHOST_SHIFT_BASE_CLK_FREQ                 8
#define SDIOHOST_MASK_BASE_CLK_FREQ                  ((u32)0x000000FF << 8)                              /*!<R 0   */
#define SDIOHOST_SHIFT_TIMEOUT_CLK_UNIT              7
#define SDIOHOST_BIT_TIMEOUT_CLK_UNIT                ((u32)0x00000001 << 7)                              /*!<R 0   */
#define SDIOHOST_SHIFT_TIMEOUT_CLK_FREQ              0
#define SDIOHOST_MASK_TIMEOUT_CLK_FREQ               ((u32)0x0000003F << 0)                              /*!<R 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_CAPABILITIES1
 * @brief  Capabilities Register                     (@ 0XC44)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_CLK_MULT                      16
#define SDIOHOST_MASK_CLK_MULT                       ((u32)0x000000FF << 16)                             /*!<R 0   */
#define SDIOHOST_SHIFT_RETUNING_MODES                14
#define SDIOHOST_MASK_RETUNING_MODES                 ((u32)0x00000003 << 14)                             /*!<R 0   */
#define SDIOHOST_SHIFT_USE_TUNING_SDR50              13
#define SDIOHOST_BIT_USE_TUNING_SDR50                ((u32)0x00000001 << 13)                             /*!<R 0   */
#define SDIOHOST_SHIFT_TMR_CNT_RETUNING              8
#define SDIOHOST_MASK_TMR_CNT_RETUNING               ((u32)0x0000000F << 8)                              /*!<R 0   */
#define SDIOHOST_SHIFT_DVR_D_SUPPORT                 6
#define SDIOHOST_BIT_DVR_D_SUPPORT                   ((u32)0x00000001 << 6)                              /*!<R 0   */
#define SDIOHOST_SHIFT_DVR_C_SUPPORT                 5
#define SDIOHOST_BIT_DVR_C_SUPPORT                   ((u32)0x00000001 << 5)                              /*!<R 0   */
#define SDIOHOST_SHIFT_DVR_A_SUPPORT                 4
#define SDIOHOST_BIT_DVR_A_SUPPORT                   ((u32)0x00000001 << 4)                              /*!<R 0   */
#define SDIOHOST_SHIFT_DDR50_SUPPORT                 2
#define SDIOHOST_BIT_DDR50_SUPPORT                   ((u32)0x00000001 << 2)                              /*!<R 0   */
#define SDIOHOST_SHIFT_SDR104_SUPPORT                1
#define SDIOHOST_BIT_SDR104_SUPPORT                  ((u32)0x00000001 << 1)                              /*!<R 0   */
#define SDIOHOST_SHIFT_SDR50_SUPPORT                 0
#define SDIOHOST_BIT_SDR50_SUPPORT                   ((u32)0x00000001 << 0)                              /*!<R 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_MAX_CURRENT0
 * @brief  Maximum Current Capabilities Register     (@ 0XC48)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_1_8V_MAX_CURR                 16
#define SDIOHOST_MASK_1_8V_MAX_CURR                  ((u32)0x000000FF << 16)                             /*!<R 0   */
#define SDIOHOST_SHIFT_3_0V_MAX_CURR                 8
#define SDIOHOST_MASK_3_0V_MAX_CURR                  ((u32)0x000000FF << 8)                              /*!<R 0   */
#define SDIOHOST_SHIFT_3_3V_MAX_CURR                 0
#define SDIOHOST_MASK_3_3V_MAX_CURR                  ((u32)0x000000FF << 0)                              /*!<R 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_FORCE_AUTO_CMD_ERR
 * @brief  Force Event Register for Auto CMD Error Status (@ 0XC50)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_CMD_NOT_ISSUED_ERR_FORCE      7
#define SDIOHOST_BIT_CMD_NOT_ISSUED_ERR_FORCE        ((u32)0x00000001 << 7)                              /*!<R 0   */
#define SDIOHOST_SHIFT_IDX_ERR_FORCE                 4
#define SDIOHOST_BIT_IDX_ERR_FORCE                   ((u32)0x00000001 << 4)                              /*!<R 0   */
#define SDIOHOST_SHIFT_END_SHIFT_ERR_FORCE           3
#define SDIOHOST_BIT_END_BIT_ERR_FORCE               ((u32)0x00000001 << 3)                              /*!<R 0   */
#define SDIOHOST_SHIFT_CRC_ERR_FORCE                 2
#define SDIOHOST_BIT_CRC_ERR_FORCE                   ((u32)0x00000001 << 2)                              /*!<R 0   */
#define SDIOHOST_SHIFT_TIMEOUT_ERR_FORCE             1
#define SDIOHOST_BIT_TIMEOUT_ERR_FORCE               ((u32)0x00000001 << 1)                              /*!<R 0   */
#define SDIOHOST_SHIFT_NOT_EXE_FORCE                 0
#define SDIOHOST_BIT_NOT_EXE_FORCE                   ((u32)0x00000001 << 0)                              /*!<R 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_FORCE_ERR
 * @brief  Force Event for Error Interrupt Status    (@ 0XC52)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_VENDOR_SPECIFIC_ERR_FORCE     12
#define SDIOHOST_MASK_VENDOR_SPECIFIC_ERR_FORCE      ((u32)0x0000000F << 12)                             /*!<WO 0   */
#define SDIOHOST_SHIFT_ADMA_ERR_FORCE                9
#define SDIOHOST_BIT_ADMA_ERR_FORCE                  ((u32)0x00000001 << 9)                              /*!<WO 0   */
#define SDIOHOST_SHIFT_AUTO_CMD_ERR_FORCE            8
#define SDIOHOST_BIT_AUTO_CMD_ERR_FORCE              ((u32)0x00000001 << 8)                              /*!<WO 0   */
#define SDIOHOST_SHIFT_CURRENT_LIMIT_ERR_FORCE       7
#define SDIOHOST_BIT_CURRENT_LIMIT_ERR_FORCE         ((u32)0x00000001 << 7)                              /*!<WO 0   */
#define SDIOHOST_SHIFT_DATA_END_SHIFT_ERR_FORCE      6
#define SDIOHOST_BIT_DATA_END_BIT_ERR_FORCE          ((u32)0x00000001 << 6)                              /*!<WO 0   */
#define SDIOHOST_SHIFT_DATA_CRC_ERR_FORCE            5
#define SDIOHOST_BIT_DATA_CRC_ERR_FORCE              ((u32)0x00000001 << 5)                              /*!<WO 0   */
#define SDIOHOST_SHIFT_DATA_TIMEOUT_ERR_FORCE        4
#define SDIOHOST_BIT_DATA_TIMEOUT_ERR_FORCE          ((u32)0x00000001 << 4)                              /*!<WO 0   */
#define SDIOHOST_SHIFT_CMD_IDX_ERR_FORCE             3
#define SDIOHOST_BIT_CMD_IDX_ERR_FORCE               ((u32)0x00000001 << 3)                              /*!<WO 0   */
#define SDIOHOST_SHIFT_CMD_END_SHIFT_ERR_FORCE       2
#define SDIOHOST_BIT_CMD_END_BIT_ERR_FORCE           ((u32)0x00000001 << 2)                              /*!<WO 0   */
#define SDIOHOST_SHIFT_CMD_CRC_ERR_FORCE             1
#define SDIOHOST_BIT_CMD_CRC_ERR_FORCE               ((u32)0x00000001 << 1)                              /*!<WO 0   */
#define SDIOHOST_SHIFT_CMD_TIMEOUT_ERR_FORCE         0
#define SDIOHOST_BIT_CMD_TIMEOUT_ERR_FORCE           ((u32)0x00000001 << 0)                              /*!<WO 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_ADMA_ERR
 * @brief  ADMA Error Status Register                (@ 0XC54)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_ADMA_LEN_MISMATCH_ERR         2
#define SDIOHOST_BIT_ADMA_LEN_MISMATCH_ERR           ((u32)0x00000001 << 2)                              /*!<R 0   */
#define SDIOHOST_SHIFT_ADMA_ERR_STATE                0
#define SDIOHOST_MASK_ADMA_ERR_STATE                 ((u32)0x00000003 << 0)                              /*!<R 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_SYS_ADDR0
 * @brief  ADMA System Address Register              (@ 0XC58)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_ADMA_SYS_ADDR                 0
#define SDIOHOST_MASK_ADMA_SYS_ADDR                  ((u32)0xFFFFFFFF << 0)                              /*!<R 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_SYS_ADDR1
 * @brief  ADMA System Address Register              (@ 0XC5C)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_ADMA_SYS_ADDR                 0
#define SDIOHOST_MASK_ADMA_SYS_ADDR                  ((u32)0xFFFFFFFF << 0)                              /*!<R 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_PRESET_VAL_INIT
 * @brief  Preset Value for Initialization           (@ 0XC60)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_DRV_STR_SEL_VAL               14
#define SDIOHOST_MASK_DRV_STR_SEL_VAL                ((u32)0x00000003 << 14)                             /*!<R 0   */
#define SDIOHOST_SHIFT_CLK_GEN_SEL_VAL               10
#define SDIOHOST_BIT_CLK_GEN_SEL_VAL                 ((u32)0x00000001 << 10)                             /*!<R 0   */
#define SDIOHOST_SHIFT_SDCLK_FREQ_SEL_VAL            0
#define SDIOHOST_MASK_SDCLK_FREQ_SEL_VAL             ((u32)0x000003FF << 0)                              /*!<R 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_PRESET_VAL_DS
 * @brief  Preset Value for Default Speed            (@ 0XC62)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_DRV_STR_SEL_VAL               14
#define SDIOHOST_MASK_DRV_STR_SEL_VAL                ((u32)0x00000003 << 14)                             /*!<R 0   */
#define SDIOHOST_SHIFT_CLK_GEN_SEL_VAL               10
#define SDIOHOST_BIT_CLK_GEN_SEL_VAL                 ((u32)0x00000001 << 10)                             /*!<R 0   */
#define SDIOHOST_SHIFT_SDCLK_FREQ_SEL_VAL            0
#define SDIOHOST_MASK_SDCLK_FREQ_SEL_VAL             ((u32)0x000003FF << 0)                              /*!<R 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_PRESET_VAL_HS
 * @brief  Preset Value for High Speed               (@ 0XC64)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_DRV_STR_SEL_VAL               14
#define SDIOHOST_MASK_DRV_STR_SEL_VAL                ((u32)0x00000003 << 14)                             /*!<R 0   */
#define SDIOHOST_SHIFT_CLK_GEN_SEL_VAL               10
#define SDIOHOST_BIT_CLK_GEN_SEL_VAL                 ((u32)0x00000001 << 10)                             /*!<R 0   */
#define SDIOHOST_SHIFT_SDCLK_FREQ_SEL_VAL            0
#define SDIOHOST_MASK_SDCLK_FREQ_SEL_VAL             ((u32)0x000003FF << 0)                              /*!<R 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_PRESET_VAL_SDR12
 * @brief  Preset Value for SDR12                    (@ 0XC66)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_DRV_STR_SEL_VAL               14
#define SDIOHOST_MASK_DRV_STR_SEL_VAL                ((u32)0x00000003 << 14)                             /*!<R 0   */
#define SDIOHOST_SHIFT_CLK_GEN_SEL_VAL               10
#define SDIOHOST_BIT_CLK_GEN_SEL_VAL                 ((u32)0x00000001 << 10)                             /*!<R 0   */
#define SDIOHOST_SHIFT_SDCLK_FREQ_SEL_VAL            0
#define SDIOHOST_MASK_SDCLK_FREQ_SEL_VAL             ((u32)0x000003FF << 0)                              /*!<R 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_PRESET_VAL_SDR25
 * @brief  Preset Value for SDR25                    (@ 0XC68)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_DRV_STR_SEL_VAL               14
#define SDIOHOST_MASK_DRV_STR_SEL_VAL                ((u32)0x00000003 << 14)                             /*!<R 0   */
#define SDIOHOST_SHIFT_CLK_GEN_SEL_VAL               10
#define SDIOHOST_BIT_CLK_GEN_SEL_VAL                 ((u32)0x00000001 << 10)                             /*!<R 0   */
#define SDIOHOST_SHIFT_SDCLK_FREQ_SEL_VAL            0
#define SDIOHOST_MASK_SDCLK_FREQ_SEL_VAL             ((u32)0x000003FF << 0)                              /*!<R 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_PRESET_VAL_SDR50
 * @brief  Preset Value for SDR50                    (@ 0XC6A)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_DRV_STR_SEL_VAL               14
#define SDIOHOST_MASK_DRV_STR_SEL_VAL                ((u32)0x00000003 << 14)                             /*!<R 0   */
#define SDIOHOST_SHIFT_CLK_GEN_SEL_VAL               10
#define SDIOHOST_BIT_CLK_GEN_SEL_VAL                 ((u32)0x00000001 << 10)                             /*!<R 0   */
#define SDIOHOST_SHIFT_SDCLK_FREQ_SEL_VAL            0
#define SDIOHOST_MASK_SDCLK_FREQ_SEL_VAL             ((u32)0x000003FF << 0)                              /*!<R 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_PRESET_VAL_SDR104
 * @brief  Preset Value for SDR104                   (@ 0XC6C)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_DRV_STR_SEL_VAL               14
#define SDIOHOST_MASK_DRV_STR_SEL_VAL                ((u32)0x00000003 << 14)                             /*!<R 0   */
#define SDIOHOST_SHIFT_CLK_GEN_SEL_VAL               10
#define SDIOHOST_BIT_CLK_GEN_SEL_VAL                 ((u32)0x00000001 << 10)                             /*!<R 0   */
#define SDIOHOST_SHIFT_SDCLK_FREQ_SEL_VAL            0
#define SDIOHOST_MASK_SDCLK_FREQ_SEL_VAL             ((u32)0x000003FF << 0)                              /*!<R 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_PRESET_VAL_DDR50
 * @brief  Preset Value for DDR50                    (@ 0XC6E)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_DRV_STR_SEL_VAL               14
#define SDIOHOST_MASK_DRV_STR_SEL_VAL                ((u32)0x00000003 << 14)                             /*!<R 0   */
#define SDIOHOST_SHIFT_CLK_GEN_SEL_VAL               10
#define SDIOHOST_BIT_CLK_GEN_SEL_VAL                 ((u32)0x00000001 << 10)                             /*!<R 0   */
#define SDIOHOST_SHIFT_SDCLK_FREQ_SEL_VAL            0
#define SDIOHOST_MASK_SDCLK_FREQ_SEL_VAL             ((u32)0x000003FF << 0)                              /*!<R 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_SHARED_BUS_CTL
 * @brief  Shared Bus Control Register               (@ 0XCE0)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_BACKEND_PWR_CTL               24
#define SDIOHOST_MASK_BACKEND_PWR_CTL                ((u32)0x0000007F << 24)                             /*!<R/W 0   */
#define SDIOHOST_SHIFT_INT_PIN_SEL                   20
#define SDIOHOST_MASK_INT_PIN_SEL                    ((u32)0x00000007 << 20)                             /*!<R/W 0   */
#define SDIOHOST_SHIFT_CLK_PIN_SEL                   16
#define SDIOHOST_MASK_CLK_PIN_SEL                    ((u32)0x00000007 << 16)                             /*!<R/W 0   */
#define SDIOHOST_SHIFT_BUS_WIDTH_PRESET              8
#define SDIOHOST_MASK_BUS_WIDTH_PRESET               ((u32)0x0000007F << 8)                              /*!<R 0   */
#define SDIOHOST_SHIFT_NUM_INT_PIN                   4
#define SDIOHOST_MASK_NUM_INT_PIN                    ((u32)0x00000003 << 4)                              /*!<R 0   */
#define SDIOHOST_SHIFT_NUM_CLK_PIN                   0
#define SDIOHOST_MASK_NUM_CLK_PIN                    ((u32)0x00000007 << 0)                              /*!<R 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_SLOT_INT
 * @brief  Slot Interrupt Status Register            (@ 0XCFC)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_INT_SIG_SLOT                  0
#define SDIOHOST_MASK_INT_SIG_SLOT                   ((u32)0x000000FF << 0)                              /*!<RO 0   */
/** @} */

/**************************************************************************//**
 * @defgroup SDIOHOST_HOST_CTL_VER
 * @brief  Host Controller Version Register          (@ 0XCFE)
 * @{
 *****************************************************************************/
#define SDIOHOST_SHIFT_SPEC_VER_NUM                  0
#define SDIOHOST_MASK_SPEC_VER_NUM                   ((u32)0x000000FF << 0)                              /*!<R 0   */
/** @} */
/** @} */
/* Exported types --------------------------------------------------------*/

/** @defgroup SDIOHOST_Exported_Types SDIOHOST Exported Types
* @{
*/

/**
* @brief SDIOHOST Register Declaration
*/

typedef struct {
	__IO uint32_t RSVD0[640]                  ;  /*!< Reserved,  Address offset:0x000-0x9FC */
	__IO uint32_t SDIOHOST_SRAM_CTL           ;  /*!< Register,  Address offset:0xA00 */
	__IO uint32_t RSVD1[3]                    ;  /*!< Reserved,  Address offset:0xA04-0xA0C */
	__IO uint32_t SDIOHOST_CTL                ;  /*!< Register,  Address offset:0xA10 */
	__IO uint32_t RSVD2[3]                    ;  /*!< Reserved,  Address offset:0xA14-0xA1C */
	__IO uint32_t SDIOHOST_DBG_1              ;  /*!< Register,  Address offset:0xA20 */
	__IO uint32_t SDIOHOST_DBG_2              ;  /*!< Register,  Address offset:0xA24 */
	__IO uint32_t SDIOHOST_DBG_3              ;  /*!< Register,  Address offset:0xA28 */
	__IO uint32_t RSVD3                       ;  /*!< Reserved,  Address offset:0xA2C */
	__IO uint32_t SDIOHOST_ISR                ;  /*!< Register,  Address offset:0xA30 */
	__IO uint32_t SDIOHOST_ISREN              ;  /*!< Register,  Address offset:0xA34 */
	__IO uint32_t RSVD4[3]                    ;  /*!< Reserved,  Address offset:0xA38-0xA40 */
	__IO uint32_t SDIOHOST_CKGEN_CTL          ;  /*!< Register,  Address offset:0xA44 */
	__IO uint32_t RSVD5[2]                    ;  /*!< Reserved,  Address offset:0xA48-0xA4C */
	__IO uint32_t SDIOHOST_DMA_RST            ;  /*!< Register,  Address offset:0xA50 */
	__IO uint32_t RSVD6                       ;  /*!< Reserved,  Address offset:0xA54 */
	__IO uint32_t SDIOHOST_DUMMY_SYS1         ;  /*!< Register,  Address offset:0xA58 */
	__I  uint32_t SDIOHOST_CLK_DET_PLLSDIO0   ;  /*!< Register,  Address offset:0xA5C */
	__IO uint32_t SDIOHOST_CLK_DET_PLLSDIO1   ;  /*!< Register,  Address offset:0xA60 */
	__IO uint32_t RSVD7                       ;  /*!< Reserved,  Address offset:0xA64 */
	__IO uint32_t SDIOHOST_SDIO_IP_BIST_CTL   ;  /*!< Register,  Address offset:0xA68 */
	__IO uint32_t RSVD8[33]                   ;  /*!< Reserved,  Address offset:0xA6C-0xAEC */
	__IO uint32_t SDIOHOST_DUMMY              ;  /*!< Register,  Address offset:0xAF0 */
	__IO uint32_t RSVD9[30]                   ;  /*!< Reserved,  Address offset:0xAF4-0xB68 */
	__IO uint32_t SDIOHOST_SDIO_MUX_CTL       ;  /*!< Register,  Address offset:0xB6C */
	__IO uint32_t RSVD10                      ;  /*!< Reserved,  Address offset:0xB70 */
	__IO uint8_t SDIOHOST_SDIO_BUS_TA_TIME   ;  /*!< Register,  Address offset:0xB74 */
	__IO uint8_t SDIOHOST_MMC_SDIO_MODE      ;  /*!< Register,  Address offset:0xB75 */
	__I  uint8_t SDIOHOST_RX_BEST_PHASE      ;  /*!< Register,  Address offset:0xB76 */
	__I  uint8_t SDIOHOST_TX_BEST_PHASE      ;  /*!< Register,  Address offset:0xB77 */
	__I  uint8_t SDIOHOST_RX_PHASE_0         ;  /*!< Register,  Address offset:0xB78 */
	__I  uint8_t SDIOHOST_RX_PHASE_1         ;  /*!< Register,  Address offset:0xB79 */
	__I  uint8_t SDIOHOST_RX_PHASE_2         ;  /*!< Register,  Address offset:0xB7A */
	__I  uint8_t SDIOHOST_RX_PHASE_3         ;  /*!< Register,  Address offset:0xB7B */
	__I  uint8_t SDIOHOST_TX_PHASE_0         ;  /*!< Register,  Address offset:0xB7C */
	__I  uint8_t SDIOHOST_TX_PHASE_1         ;  /*!< Register,  Address offset:0xB7D */
	__I  uint8_t SDIOHOST_TX_PHASE_2         ;  /*!< Register,  Address offset:0xB7E */
	__I  uint8_t SDIOHOST_TX_PHASE_3         ;  /*!< Register,  Address offset:0xB7F */
	__IO uint8_t SDIOHOST_PHASE_RST_TM_0     ;  /*!< Register,  Address offset:0xB80 */
	__IO uint8_t SDIOHOST_PHASE_RST_TM_1     ;  /*!< Register,  Address offset:0xB81 */
	__IO uint8_t SDIOHOST_SDIO_DAT_CMD       ;  /*!< Register,  Address offset:0xB82 */
	__IO uint8_t SDIOHOST_TUNING_DUTY_CTRL   ;  /*!< Register,  Address offset:0xB83 */
	__IO uint8_t SDIOHOST_TUNING_SOLUTION    ;  /*!< Register,  Address offset:0xB84 */
	__IO uint8_t SDIOHOST_SD_SAMPLE_POINT_CTL;  /*!< Register,  Address offset:0xB85 */
	__IO uint16_t SDIOHOST_HOST_DMABUF_CTL    ;  /*!< Register,  Address offset:0xB86 */
	__IO uint32_t RSVD11[30];                              /*!<  Reserved,  Address offset:0xB8A-0xBFC */
	__IO uint32_t SDIOHOST_SDMA_SYS_ADDR ;                 /*!<  SDMA System Address / Argument 2 Register register,  Address offset: 0xC00 */
	__IO uint16_t SDIOHOST_BLK_SZ ;                        /*!<  Block Size Register register,  Address offset: 0xC04 */
	__IO uint16_t SDIOHOST_BLK_CNT ;                       /*!<  Block Count Register register,  Address offset: 0xC06 */
	__IO uint32_t SDIOHOST_CMD_ARG1 ;                      /*!<  Argument 1 Register register,  Address offset: 0xC08 */
	__IO uint16_t SDIOHOST_XFER_MODE ;                     /*!<   Transfer Mode Register register,  Address offset: 0xC0C */
	__IO uint16_t SDIOHOST_CMD ;                           /*!<  Command Register register,  Address offset: 0xC0E */
	__O  uint32_t SDIOHOST_RESP0 ;                         /*!<  Response Register register,  Address offset: 0xC10 */
	__O  uint32_t SDIOHOST_RESP1 ;                         /*!<  Response Register register,  Address offset: 0xC14 */
	__O  uint32_t SDIOHOST_RESP2 ;                         /*!<  Response Register register,  Address offset: 0xC18 */
	__O  uint32_t SDIOHOST_RESP3 ;                         /*!<  Response Register register,  Address offset: 0xC1C */
	__IO uint32_t SDIOHOST_BUFFER_PORT_DATA ;              /*!<   Buffer Data Port Register register,  Address offset: 0xC20 */
	__O  uint32_t SDIOHOST_PRESENT_STATE ;                 /*!<  ADC Debug Select Register register,  Address offset: 0xC24 */
	__IO uint8_t SDIOHOST_HOST_CTL_1_REG ;                /*!<  Host Control 1 Register register,  Address offset: 0xC28 */
	__IO uint8_t SDIOHOST_PWR_CTL ;                       /*!<  Power Control Register register,  Address offset: 0xC29 */
	__IO uint8_t SDIOHOST_BLK_GAP_CTL ;                   /*!<  Block Gap Control Register  register,  Address offset: 0xC2A */
	__IO uint8_t SDIOHOST_WAKEUP_CTL ;                    /*!<  Wakeup Control Register register,  Address offset: 0xC2B */
	__IO uint16_t SDIOHOST_CLK_CTL ;                       /*!<   Clock Control Register register,  Address offset: 0xC2C */
	__IO uint8_t SDIOHOST_TIMEOUT_CTL ;                   /*!<  Timeout Control Register register,  Address offset: 0xC2E */
	__IO uint8_t SDIOHOST_SW_RST ;                        /*!<  Software Reset Register register,  Address offset: 0xC2F */
	__IO uint16_t SDIOHOST_NORMAL_INT_STATUS ;             /*!<  Normal Interrupt Status Register register,  Address offset: 0xC30 */
	__IO uint16_t SDIOHOST_ERR_INT_STATUS ;                /*!<  Error Interrupt Status Register register,  Address offset: 0xC32 */
	__IO uint16_t SDIOHOST_NORMAL_INT_STATUS_EN ;          /*!<  Normal Interrupt Status Enable Register register,  Address offset: 0xC34 */
	__IO uint16_t SDIOHOST_ERR_INT_STATUS_EN ;             /*!<   Error Interrupt Status Enable Register register,  Address offset: 0xC36 */
	__IO uint16_t SDIOHOST_NORMAL_SIG_EN ;                 /*!<  Normal Interrupt Signal Enable Register register,  Address offset: 0xC38 */
	__IO uint16_t SDIOHOST_ERR_SIG_EN ;                    /*!<  Error Interrupt Signal Enable Register register,  Address offset: 0xC3A */
	__O  uint16_t SDIOHOST_AUTO_CMD_ERR_STATUS ;           /*!<  Auto CMD Error Status Register register,  Address offset: 0xC3C */
	__IO uint16_t SDIOHOST_HOST_CTL ;                      /*!<  Host Control 2 Register register,  Address offset: 0xC3E */
	__I  uint32_t SDIOHOST_CAPABILITIES0 ;                 /*!<  Capabilities Register register,  Address offset: 0xC40 */
	__I  uint32_t SDIOHOST_CAPABILITIES1 ;                 /*!<  Capabilities Register register,  Address offset: 0xC44 */
	__I  uint32_t SDIOHOST_MAX_CURRENT0 ;                  /*!<  Maximum Current Capabilities Register register,  Address offset: 0xC48 */
	__I  uint32_t SDIOHOST_MAX_CURRENT1 ;                  /*!<  Maximum Current Capabilities Register register,  Address offset: 0xC4C */
	__I  uint16_t SDIOHOST_FORCE_AUTO_CMD_ERR ;            /*!<  Force Event Register for Auto CMD Error Status register,  Address offset: 0xC50 */
	__O  uint16_t SDIOHOST_FORCE_ERR ;                     /*!<  Force Event for Error Interrupt Status register,  Address offset: 0xC52 */
	__I  uint32_t SDIOHOST_ADMA_ERR ;                      /*!<  ADMA Error Status Register register,  Address offset: 0xC54 */
	__IO uint32_t SDIOHOST_SYS_ADDR0 ;                     /*!<  ADMA System Address Register register,  Address offset: 0xC58 */
	__IO uint32_t SDIOHOST_SYS_ADDR1 ;                     /*!<  ADMA System Address Register register,  Address offset: 0xC5C */
	__I  uint16_t SDIOHOST_PRESET_VAL_INIT ;               /*!<  Preset Value for Initialization register,  Address offset: 0xC60 */
	__I  uint16_t SDIOHOST_PRESET_VAL_DS ;                 /*!<  Preset Value for Default Speed  register,  Address offset: 0xC62 */
	__I  uint16_t SDIOHOST_PRESET_VAL_HS ;                 /*!<  Preset Value for High Speed  register,  Address offset: 0xC64 */
	__I  uint16_t SDIOHOST_PRESET_VAL_SDR12 ;              /*!<  Preset Value for SDR12  register,  Address offset: 0xC66 */
	__I  uint16_t SDIOHOST_PRESET_VAL_SDR25 ;              /*!<  Preset Value for SDR25  register,  Address offset: 0xC68 */
	__I  uint16_t SDIOHOST_PRESET_VAL_SDR50 ;              /*!<  Preset Value for SDR50 register,  Address offset: 0xC6A */
	__I  uint16_t SDIOHOST_PRESET_VAL_SDR104 ;             /*!<  Preset Value for SDR104  register,  Address offset: 0xC6C */
	__I  uint16_t SDIOHOST_PRESET_VAL_DDR50 ;              /*!<  Preset Value for DDR50  register,  Address offset: 0xC6E */
	__IO uint32_t RSVD12[27];                              /*!<  Reserved,  Address offset:0xC72-0xCDC */
	__IO uint32_t SDIOHOST_SHARED_BUS_CTL ;                /*!<  Shared Bus Control Register register,  Address offset: 0xCE0 */
	__IO uint32_t RSVD13[6];                               /*!<  Reserved,  Address offset:0xCE4-0xCF8 */
	__O  uint16_t SDIOHOST_SLOT_INT ;                      /*!<  Slot Interrupt Status Register register,  Address offset: 0xCFC */
	__I  uint16_t SDIOHOST_HOST_CTL_VER ;                  /*!<  Host Controller Version Register register,          Address offset: 0xCFE */
} SDIOHOST_TypeDef;
/** @} */

/* AUTO_GEN_END */

/* MANUAL_GEN_START */

/**
  \brief  Defines SDIO Host ADMA table end.
*/

typedef enum sdioh_adma_line_end_e {
	SdiohADMALineCont   = 0,
	SdiohADMALineEnd    = 1,
} sdioh_adma_line_end_t;

/**
  \brief  Defines SDIO client CCCR.
*/
typedef enum sdioh_client_cccr_e {
	SdiohClientCccrRev          = 0x00,
	SdiohClientSdSpecRev        = 0x01,
	SdiohClientIoEn             = 0x02,
	SdiohClientIoRdy            = 0x03,
	SdiohClientIntEn            = 0x04,
	SdiohClientIntPend          = 0x05,
	SdiohClientIoAbort          = 0x06,
	SdiohClientBusInterfaceCtl  = 0x07,
	SdiohClientCardCapability   = 0x08,
	SdiohClientCisPtr           = 0x09,
	SdiohClientBusSuspend       = 0x0C,
	SdiohClientFnSel            = 0x0D,
	SdiohClientExecFlag         = 0x0E,
	SdiohClientRdyFlag          = 0x0F,
	SdiohClientFn0BlkSz         = 0x10,
	SdiohClientPwrCtl           = 0x12,
	SdiohClientBusSpeedSel      = 0x13,
	SdiohClientUhsSupport       = 0x14,
	SdiohClientDvrStrength      = 0x15,
	SdiohClientIntExt           = 0x16,
} sdioh_client_cccr_t;

/**
  \brief  Defines SDIO client SDIO revision.
*/
typedef enum sdioh_client_sdio_rev_e {
	SdiohClientSdioRev1_0   = 0x0,
	SdiohClientSdioRev1_1   = 0x1,
	SdiohClientSdioRev1_2   = 0x2,
	SdiohClientSdioRev2_0   = 0x3,
	SdiohClientSdioRev3_0   = 0x4,
} sdioh_client_sdio_rev_t;

/**
  \brief  Defines SDIO client SD revision.
*/
typedef enum sdioh_client_sd_rev_e {
	SdiohClientSdRev1_01    = 0x0,
	SdiohClientSdRev1_10    = 0x1,
	SdiohClientSdRev2_00    = 0x2,
	SdiohClientSdRev3_00    = 0x3,
} sdioh_client_sd_rev_t;

/**
  \brief  Defines SDIO client bus width.
*/
typedef enum sdioh_client_bus_width_e {
	SdiohClientBus1Bit    = 0x0,
	SdiohClientBus4Bit    = 0x2,
	SdiohClientBus8Bit    = 0x3,
} sdioh_client_bus_width_t;

/**
  \brief  Defines SDIO client 8-bit support.
*/
typedef enum sdioh_client_8bit_support_e {
	SdiohClient8BitNo   = 0x0,
	SdiohClient8BitYes  = 0x1,
} sdioh_client_8bit_support_t;

/**
  \brief  Defines SDIO client multi-block support.
*/
typedef enum sdioh_client_multiblk_support_e {
	SdiohClientMultiblkNo   = 0x0,
	SdiohClientMultiblkYes  = 0x1,
} sdioh_client_multiblk_support_t;

/**
  \brief  Defines SDIO client block interrupt support.
*/
typedef enum sdioh_client_blk_int_support_e {
	SdiohClientBlkIntNo     = 0x0,
	SdiohClientBlkIntYes    = 0x1,
} sdioh_client_blk_int_support_t;

/**
  \brief  Defines SDIO client function interface code.
*/
typedef enum sdioh_client_fn_interface_e {
	SdiohClientFnIntNA      = 0x0,
	SdiohClientFnIntUart    = 0x1,
	SdiohClientFnIntBtA     = 0x2,
	SdiohClientFnIntBtB     = 0x3,
	SdiohClientFnIntGps     = 0x4,
	SdiohClientFnIntCam     = 0x5,
	SdiohClientFnIntPhs     = 0x6,
	SdiohClientFnIntWlan    = 0x7,
	SdiohClientFnIntAta     = 0x8,
	SdiohClientFnIntBtAAmp  = 0x9,
} sdioh_client_fn_interface_t;

typedef struct adma_descriptor_line_s {
	u32 valid : 1;
	u32 end : 1;
	u32 interrupt : 1;
	u32 stuff_0 : 1;
	u32 act : 2;
	u32 stuff_1 : 10;
	u32 length : 16;
} adma_descriptor_line_t;


/* added by katherine */
#define SDIO_CMD_ERR	(SDIOHOST_BIT_CMD_IDX_ERR | \
						 SDIOHOST_BIT_CMD_END_BIT_ERR | \
						 SDIOHOST_BIT_CMD_CRC_ERR | \
						 SDIOHOST_BIT_CMD_TIMEOUT_ERR)
#define SDIO_DAT_ERR	(SDIOHOST_BIT_DATA_END_BIT_ERR | \
						 SDIOHOST_BIT_DATA_CRC_ERR | \
						 SDIOHOST_BIT_DATA_TIMEOUT_ERR)

#define SDIO_ERR_FLAG	(SDIO_CMD_ERR | SDIO_DAT_ERR)

#if defined(CONFIG_FPGA) && (CONFIG_FPGA == 1)
#define SDIO_CMD_TIMEOUT_US 100000
#define SDIO_DATA_TIMEOUT_US 500000
#define SDIO_SPEED_DS 0x8
#define SDIO_SPEED_HS 0x4
#else
#define SDIO_CMD_TIMEOUT_US 100000
#define SDIO_DATA_TIMEOUT_US 500000
#define SDIO_SPEED_DS 0x2
#define SDIO_SPEED_HS 0x1
#endif

#define SDIO_SRAM_CTL_CONFIG 0x21F0200
#define SDIO_IP_CTL_CONFIG 0xB
#define SDIO_HOST_CTL_CONFIG 0xC1
#define SDIO_CLK_CTL_CONFIG 0x207
#define SDIO_TIMEOUT_CTL_CONFIG 0xE
#define SDIO_ISREN_CONFIG 0x11
#define SDIO_NORMAL_INT_STATUS_EN_CONFIG 0x1FB
#define SDIO_ERR_INT_STATUS_EN_CONFIG 0x37F
#define SDIO_NORMAL_SIG_EN_CONFIG 0x1FB
#define SDIO_ERR_SIG_EN_CONFIG 0x380 // disable CMD_ERR & DAT_ERR INT
#define SDIO_OCR_VOLTAGE 0x100000
#define SDIO_HOST_CTL2_CONFIG 0xA // 1.8V signaling + SDR50
/* MANUAL_GEN_END */

/** @} */

/** @} */

/* Exported types ------------------------------------------------------------*/
/** @defgroup SDMMC_LL_Exported_Types SDMMC_LL Exported Types
 * @{
 */

/**
 * @brief  SDMMC Command Control structure
 */
typedef struct {
	u32 Argument; /*!< Specifies the SDMMC command argument which is sent
							to a card as part of a command message. If a command
							contains an argument, it must be loaded into this
						  register before writing the command to the command
						  register.              */
	u8 CmdIndex; /*!< Specifies the SDMMC command index. It must be
					 Min_Data = 0 and Max_Data = 64 */
	u8 CmdType;  /*!< Specifies command type */
	u8 RespType; /*!< Specifies the SDMMC response type.
					This parameter can be a value of @ref SDMMC_LL_Response_Type */
	u8 DataPresent;  /*!< Specifies whether data is present */
} SDIO_CmdInitTypeDef;

/**
 * @brief  SDMMC Data Control structure
 */
typedef struct {
	u8 TransType; /*!< data transfer type, single/infinite/multiple/stop multiple transfer */
	u8 TransDir;
	u8 AutoCmdEn;
	u8 DmaEn;
	u16 BlockSize;
	u16 BlockCnt;
} SDIO_DataInitTypeDef;

/**
 * @}
 */

/* Exported constants --------------------------------------------------------*/
/** @defgroup SDMMC_LL_Exported_Constants SDMMC_LL Exported Constants
 * @{
 */
#define SD_ERROR_NONE                     0x00000000U
#define SD_ERROR_CMD_CRC_FAIL             0x00000001U
#define SD_ERROR_DATA_CRC_FAIL            0x00000002U
#define SD_ERROR_CMD_RSP_TIMEOUT          0x00000004U
#define SD_ERROR_DATA_TIMEOUT             0x00000008U
#define SD_ERROR_TX_UNDERRUN              0x00000010U
#define SD_ERROR_RX_OVERRUN               0x00000020U
#define SD_ERROR_ADDR_MISALIGNED          0x00000040U
#define SD_ERROR_BLOCK_LEN_ERR            0x00000080U
#define SD_ERROR_ERASE_SEQ_ERR            0x00000100U
#define SD_ERROR_BAD_ERASE_PARAM          0x00000200U
#define SD_ERROR_WRITE_PROT_VIOLATION     0x00000400U
#define SD_ERROR_LOCK_UNLOCK_FAILED       0x00000800U
#define SD_ERROR_COM_CRC_FAILED           0x00001000U
#define SD_ERROR_ILLEGAL_CMD              0x00002000U
#define SD_ERROR_CARD_ECC_FAILED          0x00004000U
#define SD_ERROR_CC_ERR                   0x00008000U
#define SD_ERROR_GENERAL_UNKNOWN_ERR      0x00010000U
#define SD_ERROR_STREAM_READ_UNDERRUN     0x00020000U
#define SD_ERROR_STREAM_WRITE_OVERRUN     0x00040000U
#define SD_ERROR_CID_CSDMMC_OVERWRITE     0x00080000U
#define SD_ERROR_WP_ERASE_SKIP            0x00100000U
#define SD_ERROR_CARD_ECC_DISABLED        0x00200000U
#define SD_ERROR_ERASE_RESET              0x00400000U
#define SD_ERROR_AKE_SEQ_ERR              0x00800000U
#define SD_ERROR_INVALID_VOLTRANGE        0x01000000U
#define SD_ERROR_ADDR_OUT_OF_RANGE        0x02000000U
#define SD_ERROR_REQUEST_NOT_APPLICABLE   0x04000000U
#define SD_ERROR_INVALID_PARAMETER        0x08000000U
#define SD_ERROR_UNSUPPORTED_FEATURE      0x10000000U
#define SD_ERROR_BUSY                     0x20000000U
#define SD_ERROR_DMA                      0x40000000U
#define SD_ERROR_TIMEOUT                  0x80000000U

/**
 * @brief SDMMC Commands Index
 */

/* MMC commands */                              /* response type */
#define SDMMC_GO_IDLE_STATE               0       /* R0 */
#define SDMMC_SEND_OP_COND                1       /* R3, for MMC */
#define SDMMC_ALL_SEND_CID                2       /* R2 */
#define SDMMC_SET_RELATIVE_ADDR           3       /* R1 */
#define SDMMC_SWITCH                      6       /* R1B */
#define SDMMC_SELECT_CARD                 7       /* R1 */
#define SDMMC_SEND_EXT_CSD                8       /* R1, for MMC */
#define SDMMC_SEND_CSD                    9       /* R2 */
#define SDMMC_SEND_CID                    10      /* R1 */
#define SDMMC_READ_DAT_UNTIL_STOP         11      /* R1 */
#define SDMMC_STOP_TRANSMISSION           12      /* R1B */
#define SDMMC_SEND_STATUS                 13      /* R1 */
#define SDMMC_SET_BLOCKLEN                16      /* R1 */
#define SDMMC_READ_BLOCK_SINGLE           17      /* R1 */
#define SDMMC_READ_BLOCK_MULTIPLE         18      /* R1 */
#define SDMMC_SEND_TUNING_BLOCK           19      /* R1 */
#define SDMMC_WRITE_DAT_UNTIL_STOP        20      /* R1 */
#define SDMMC_SET_BLOCK_COUNT             23      /* R1 */
#define SDMMC_WRITE_BLOCK_SINGLE          24      /* R1 */
#define SDMMC_WRITE_BLOCK_MULTIPLE        25      /* R1 */
#define SDMMC_ERASE_GROUP_START           35      /* R1 */
#define SDMMC_ERASE_GROUP_END             36      /* R1 */
#define SDMMC_ERASE                       38      /* R1B */
#define SDMMC_APP_CMD                     55      /* R1 */

/* SD commands */                               /* response type */
#define SDMMC_SEND_RELATIVE_ADDR           3       /* R6 */
#define SDMMC_SEND_SWITCH_FUNC             6       /* R1 */
#define SDMMC_SEND_IF_COND                 8       /* R7 */
#define SDMMC_SWITCH_VOLTAGE               11      /* R1 */
#define SDMMC_SD_ERASE_GROUP_START         32      /* R1 */
#define SDMMC_SD_ERASE_GROUP_END           33      /* R1 */
#define SDMMC_READ_OCR                     58      /* R3 */
#define SDMMC_CRC_ON_OFF                   59      /* R1 */

/* SD application commands */                   /* response type */
#define SDMMC_APP_SET_BUS_WIDTH            6       /* R1 */
#define SDMMC_APP_SDMMC_STATUS             13      /* R2 */
#define SDMMC_APP_SEND_NUM_WR_BLOCKS       22      /* R1 */
#define SDMMC_APP_SET_WR_BLK_ERASE_CNT     23      /* R1 */
#define SDMMC_APP_OP_COND                  41      /* R3 */
#define SDMMC_APP_SEND_SCR                 51      /* R1 */

/* SD IO commands */
#define SDMMC_IO_SEND_OP_COND              5       /* R4 */
#define SDMMC_IO_RW_DIRECT                 52      /* R5 */
#define SDMMC_IO_RW_EXTENDED               53      /* R5 */

/**
 * @brief  Masks for errors Card Status R1 (CSR Register)
 */
#define SDMMC_R1_ADDR_OUT_OF_RANGE 0x80000000U
#define SDMMC_R1_ADDR_MISALIGNED 0x40000000U
#define SDMMC_R1_BLOCK_LEN_ERR 0x20000000U
#define SDMMC_R1_ERASE_SEQ_ERR 0x10000000U
#define SDMMC_R1_BAD_ERASE_PARAM 0x08000000U
#define SDMMC_R1_WRITE_PROT_VIOLATION 0x04000000U
#define SDMMC_R1_LOCK_UNLOCK_FAILED 0x01000000U
#define SDMMC_R1_CARD_IS_LOCKED 0x01000000U
#define SDMMC_R1_COM_CRC_FAILED 0x00800000U
#define SDMMC_R1_ILLEGAL_CMD 0x00400000U
#define SDMMC_R1_CARD_ECC_FAILED 0x00200000U
#define SDMMC_R1_CC_ERROR 0x00100000U
#define SDMMC_R1_GENERAL_UNKNOWN_ERROR 0x00080000U
#define SDMMC_R1_STREAM_READ_UNDERRUN 0x00040000U
#define SDMMC_R1_STREAM_WRITE_OVERRUN 0x00020000U
#define SDMMC_R1_CID_CSDMMC_OVERWRITE 0x00010000U
#define SDMMC_R1_WP_ERASE_SKIP 0x00008000U
#define SDMMC_R1_CARD_ECC_DISABLED 0x00004000U
#define SDMMC_R1_ERASE_RESET 0x00002000U
#define SDMMC_R1_AKE_SEQ_ERROR 0x00000008U
#define SDMMC_R1_ERRORBITS 0xFDFFE008U

/**
 * @brief  Masks for R5 Response
 */
/* added by kw */
#define SDMMC_R5_ERRORBITS 0x0000CB00U
#define SDMMC_R5_COM_CRC_ERROR 0x00008000U
#define SDMMC_R5_ILLEGAL_COMMAND 0x00004000U
#define SDMMC_R5_IO_CURRENT_STATE 0x00003000U
#define SDMMC_R5_GENERAL_ERROR 0x00000800U
#define SDMMC_R5_INVALID_FUNC_NUM 0x00000200U
#define SDMMC_R5_OUT_OF_RANGE 0x00000100U

/**
 * @brief  Masks for R6 Response
 */
#define SDMMC_R6_GENERAL_UNKNOWN_ERROR 0x00002000U
#define SDMMC_R6_ILLEGAL_CMD 0x00004000U
#define SDMMC_R6_COM_CRC_FAILED 0x00008000U

#define SDMMC_HIGH_CAPACITY 0x40000000U
#define SDMMC_SWITCH_1_8V_CAPACITY 0x10000000U
#define SDMMC_VOLTAGE_WINDOW_SD 0x00100000U // 3.2-3.3V

#define SDMMC_CHECK_PATTERN 0x000001AAU
/**
 * @brief  Masks for SCR
 */
#define SDMMC_MASK_SCR_SPEC_VER 0x0F000000U // SCR[59:56]
#define SDMMC_SHIFT_SCR_SPEC_VER 24U
#define SDMMC_GET_SPEC_VER(x) (((x) & SDMMC_MASK_SCR_SPEC_VER) >> SDMMC_SHIFT_SCR_SPEC_VER)
#define SDMMC_WIDE_BUS_SUPPORT 0x00040000U // SCR[50]
#define SDMMC_SINGLE_BUS_SUPPORT 0x00010000U // SCR[48]

/**
 * @brief  Command Class supported
 */
#define SDMMC_CCC_BASIC 		(1 << 0) // basic
#define SDMMC_CCC_BR			(1 << 2) // block read
#define SDMMC_CCC_BW			(1 << 4) // block write
#define SDMMC_CCC_ERASE 		(1 << 5) // erase
#define SDMMC_CCC_WP			(1 << 6) // write protection
#define SDMMC_CCC_LC			(1 << 7) // lock card
#define SDMMC_CCC_AS			(1 << 8) // application specific
#define SDMMC_CCC_IOM			(1 << 9) // I/O mode
#define SDMMC_CCC_SWITCH		(1 << 10) // switch

/**
 * @brief Definitions of SD timeout
 */
#define SDMMC_WAIT_FOREVER (0xFFFFFFFFU)

#define SDMMC_CMD_TIMEOUT 5000U // Command send and response timeout(us): 5ms
#define SDMMC_DAT_TIMEOUT 1000000U // Hardware Data timeout(us): 1s

#define SDMMC_READ_TIMEOUT 100000U // Read timeout(us): 100ms
#define SDMMC_WRITE_TIMEOUT 500000U // Write timeout(us): 500ms
#define SDMMC_STOP_TRANS_TIMEOUT 500000U // STOP TRANSMISSION command timeout(us): 500ms
#define SDMMC_ERASE_TIMEOUT 250000U // Erase timeout(us): 250ms

#define SDMMC_MAX_VOLT_TRIAL 0x0000FFFFU

/**
 * @brief SD logical processing
 */
#define SDMMC_ALLZERO 0x00000000U
#define SDMMC_0TO7BITS 0x000000FFU
#define SDMMC_8TO15BITS 0x0000FF00U
#define SDMMC_16TO23BITS 0x00FF0000U
#define SDMMC_24TO31BITS 0xFF000000U

/** @defgroup SDIO_LL_Clock_Freq SD Clock Frequency
 * @{
 */
#define SD_TRANS_HS_50MHZ	50000 // High speed
#define SD_TRANS_DS_25MHZ	25000 // Default speed
#define SD_INIT_400K		400
/**
 * @}
 */

/** @defgroup SDIO_LL_Command_Index Command Index
 * @{
 */
#define IS_SD_CMD_INDEX(INDEX) ((INDEX) < 0x40U)
/**
 * @}
 */

/** @defgroup SDIO_LL_Command_Type Command Type
 * @{
 */
#define SDMMC_CMD_NORMAL	((u8)0x0)
#define SDMMC_CMD_SUSPEND	((u8)0x1)
#define SDMMC_CMD_RESUME	((u8)0x2)
#define SDMMC_CMD_ABORT		((u8)0x3)

#define IS_SD_CMD_TYPE(CMD)	(((CMD) == SDMMC_CMD_NORMAL) || \
								 ((CMD) == SDMMC_CMD_SUSPEND) || \
								 ((CMD) == SDMMC_CMD_RESUME) || \
								 ((CMD) == SDMMC_CMD_ABORT))
/**
 * @}
 */

/** @defgroup SDIO_LL_Response_Type Response Type
 * @{
 */
#define SDMMC_RSP_NONE		((u8)0x0)
#define SDMMC_RSP_R1		((u8)0x1)
#define SDMMC_RSP_R1B		((u8)0x2)
#define SDMMC_RSP_R2		((u8)0x3)
#define SDMMC_RSP_R3		((u8)0x4)
#define SDMMC_RSP_R4		((u8)0x5)
#define SDMMC_RSP_R5		((u8)0x6)
#define SDMMC_RSP_R5B		((u8)0x7)
#define SDMMC_RSP_R6		((u8)0x8)
#define SDMMC_RSP_R7		((u8)0x9)

#define IS_SD_RESP_TYPE(RESP)	(((RESP) == SDMMC_RSP_NONE) ||	\
								 ((RESP) == SDMMC_RSP_R1) ||		\
								 ((RESP) == SDMMC_RSP_R1B) ||		\
								 ((RESP) == SDMMC_RSP_R2) ||		\
								 ((RESP) == SDMMC_RSP_R3) ||		\
								 ((RESP) == SDMMC_RSP_R4) ||		\
								 ((RESP) == SDMMC_RSP_R5) ||		\
								 ((RESP) == SDMMC_RSP_R5B) ||		\
								 ((RESP) == SDMMC_RSP_R6) ||		\
								 ((RESP) == SDMMC_RSP_R7))

#define SD_RESP_NO			((u8)0x0)
#define SD_RESP_LEN136		((u8)0x1)
#define SD_RESP_LEN48		((u8)0x2)
#define SD_RESP_LEN48_BUSY	((u8)0x3)

/**
 * @}
 */

/** @defgroup SDIO_LL_Response_Registers Response Register
 * @{
 */
#define SDIO_RESP0		((u8)0x00U)
#define SDIO_RESP1		((u8)0x04U)
#define SDIO_RESP2		((u8)0x08U)
#define SDIO_RESP3		((u8)0x0CU)

#define IS_SD_RESP(RESP)	(((RESP) == SDIO_RESP0) || \
							 ((RESP) == SDIO_RESP1) || \
							 ((RESP) == SDIO_RESP2) || \
							 ((RESP) == SDIO_RESP3))
/**
 * @}
 */

/** @defgroup SDIO_LL_DPSM_State DPSM State
 * @{
 */
#define SDIO_TRANS_NO_DATA		((u8)0x00U)
#define SDIO_TRANS_WITH_DATA	((u8)0x01U)
#define IS_SD_DATA_TYPE(TYPE)	(((TYPE) == SDIO_TRANS_WITH_DATA) || \
								 ((TYPE) == SDIO_TRANS_NO_DATA))
/**
 * @}
 */

/** @defgroup SDIO_LL_Data_Block_Size  Data Block Size
 * @{
 */
#define IS_SD_BLOCK_SIZE(SIZE)	((SIZE) <= 0x800) // 2048 for SDIO card, 512 for SD mem card
/**
 * @}
 */

/** @defgroup SDHOST_DMA_ALIGN_ADDR
 * @{
 */
#define SDIOH_DMA_ALIGN_SZ		(8) // hw: 8byte aligned
/**
 * @}
 */

/** @defgroup SDIO_LL_Transfer_Type Transfer Type
 * @{
 */
#define SDIO_TRANS_SINGLE_BLK	((u8)0x00U)
#define SDIO_TRANS_INFIN_BLK	((u8)0x01U)
#define SDIO_TRANS_MULTI_BLK	((u8)0x02U)
#define SDIO_TRANS_MULTI_STOP	((u8)0x03U)

#define IS_SD_TRANSFER_TYPE(TYPE)		(((TYPE) == SDIO_TRANS_SINGLE_BLK) || \
										 ((TYPE) == SDIO_TRANS_INFIN_BLK) || \
										 ((TYPE) == SDIO_TRANS_MULTI_BLK) || \
										 ((TYPE) == SDIO_TRANS_MULTI_STOP))
/**
 * @}
 */

/** @defgroup SDIO_LL_Transfer_Direction Transfer Direction
 * @{
 */
#define SDIO_TRANS_HOST_TO_CARD ((u8)0x00U)
#define SDIO_TRANS_CARD_TO_HOST ((u8)0x01U)

#define IS_SD_TRANSFER_DIR(DIR)		(((DIR) == SDIO_TRANS_HOST_TO_CARD) || \
									((DIR) == SDIO_TRANS_CARD_TO_HOST))
/**
 * @}
 */

/** @defgroup SDIO_LL_Transfer_Control
 * @{
 */
#define SDIO_TRANS_AUTO_DIS			((u8)0x00U)
#define SDIO_TRANS_AUTO_CMD12_EN	((u8)0x01U)
#define SDIO_TRANS_AUTO_CMD23_EN	((u8)0x02U)



/**
 * @}
 */

/** @defgroup SDIO_LL_DMA_MODE DMA Mode
 * @{
 */
#define SDIO_TRANS_DMA_DIS	((u8)0x00U)
#define SDIO_TRANS_DMA_EN	((u8)0x01U)

#define SDIO_SDMA_MODE				((u8)0x00)
#define SDIO_ADMA2_32B_MODE			((u8)0x02) // 32-bit Adrress ADMA2
#define SDIO_ADMA2_64B_MODE			((u8)0x03) // 64-bit Adrress ADMA2
#define IS_SD_DMA_MODE(MODE)		(((MODE) == SDIO_SDMA_MODE) || \
									 ((MODE) == SDIO_ADMA2_32B_MODE) || \
									 ((MODE) == SDIO_ADMA2_64B_MODE))

/**
 * @}
 */

/** @defgroup SD_Specification_Version
  * @{
  */
#define SD_SPEC_V101			((u8)0x0)
#define SD_SPEC_V110			((u8)0x1)
#define SD_SPEC_V200			((u8)0x2)
#define SD_SPEC_V300			((u8)0x3)
/**
  * @}
  */

/** @defgroup SD_Bus_Width
  * @{
  */
#define SDIOH_BUS_WIDTH_1BIT	((u8)0x00U)
#define SDIOH_BUS_WIDTH_4BIT	((u8)0x01U)
#define SDIOH_BUS_WIDTH_8BIT	((u8)0x02U)

#define IS_SD_BUS_WIDTH(WIDTH) (((WIDTH) == SDIOH_BUS_WIDTH_1BIT) || \
								((WIDTH) == SDIOH_BUS_WIDTH_4BIT) || \
								((WIDTH) == SDIOH_BUS_WIDTH_8BIT))
/**
  * @}
  */

/** @defgroup SD_CMD6_OpMode
  * @{
  */
#define SD_CMD6_CHECK_MODE		((u8)0x00U)
#define SD_CMD6_SWITCH_MODE		((u8)0x01U)
/**
  * @}
  */

/** @defgroup SD_CMD6_FuncGroup
  * @{
  */
#define SD_FUNC_GROUP_MIN		((u8)0x01U)
#define SD_FUNC_GROUP_MAX		((u8)0x06U)

#define SD_ACCESS_MODE          ((u8)0x01U) // Function group 1, Access Mode
#define SD_COMMAND_SYSTEM       ((u8)0x02U) // Function group 2, Command System
#define SD_DRIVER_STRENGTH      ((u8)0x03U) // Function group 3, Driver Strength
#define SD_CURRENT_LIMIT        ((u8)0x04U) // Function group 4, Current Limit
/**
  * @}
  */

/** @defgroup SD_Access_Mode
  * @{
  */
#define SD_SPEED_DS				((u8)0x00U) // 3.3V Function 0
#define SD_SPEED_HS				((u8)0x01U) // 3.3V Function 1
#define SD_SPEED_SDR12			((u8)0x00U) // 1.8V Function 0
#define SD_SPEED_SDR25			((u8)0x01U) // 1.8V Function 1
#define SD_SPEED_SDR50			((u8)0x02U) // 1.8V Function 2
#define SD_SPEED_SDR104			((u8)0x03U) // 1.8V Function 3
#define SD_SPEED_DDR50			((u8)0x04U) // 1.8V Function 4

#define SD_KEEP_CUR_SPEED		((u8)0x0FU)

#define IS_SD_BUS_SPEED(SPEED)		(((SPEED) == SD_SPEED_DS) || \
 									 ((SPEED) == SD_SPEED_HS) || \
 									 ((SPEED) == SD_SPEED_SDR12) || \
 									 ((SPEED) == SD_SPEED_SDR25) || \
 									 ((SPEED) == SD_SPEED_SDR50) || \
 									 ((SPEED) == SD_SPEED_SDR104) || \
 									 ((SPEED) == SD_SPEED_DDR50))
/**
  * @}
  */

/** @defgroup SDIO_FUNCTION Function Number
 * @{
 */
#define SDIO_FUNC0 0x00U
#define SDIO_FUNC1 0x01U
#define SDIO_FUNC2 0x02U
#define SDIO_FUNC3 0x03U
#define SDIO_FUNC4 0x04U
#define SDIO_FUNC5 0x05U
#define SDIO_FUNC6 0x06U
#define SDIO_FUNC7 0x07U
/**
 * @}
 */

/**
 * @}
 */

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @addtogroup SDMMC_LL_Exported_Functions
 * @{
 */

/* Initialization/de-initialization functions */
u32 SDIOH_Init(SDIOHOST_TypeDef *SDIOx);

/* I/O operation functions */
u32 SDIO_ReadFIFO(SDIOHOST_TypeDef *SDIOx);
void SDIO_WriteFIFO(SDIOHOST_TypeDef *SDIOx, u32 *pWriteData);

/* Peripheral Control functions */
u32 SDIO_ResetAll(SDIOHOST_TypeDef *SDIOx);
u32 SDIO_CheckState(SDIOHOST_TypeDef *SDIOx);
void SDIO_PowerState_ON(SDIOHOST_TypeDef *SDIOx);
void SDIO_PowerState_OFF(SDIOHOST_TypeDef *SDIOx);
u8 SDIO_GetPowerState(SDIOHOST_TypeDef *SDIOx);
u32 SDIO_ConfigDMA(SDIOHOST_TypeDef *SDIOx, u8 DmaMode, u32 DmaAddr);
u32 SDIO_ConfigClock(SDIOHOST_TypeDef *SDIOx, u32 ClkKHz);
u32 SDIO_ConfigBusWidth(SDIOHOST_TypeDef *SDIOx, u8 BusWidth);

/* Command path state machine (CPSM) management functions */
void SDIO_SendCommand(SDIOHOST_TypeDef *SDIOx, SDIO_CmdInitTypeDef *Command);
u32 SDIO_GetResponse(SDIOHOST_TypeDef *SDIOx, u8 Response);
/* Data path state machine (DPSM) management functions */
void SDIO_ConfigData(SDIOHOST_TypeDef *SDIOx, SDIO_DataInitTypeDef *Data);

/* Interrupt Related functions */
u32 SDIO_GetStatus(SDIOHOST_TypeDef *SDIOx);
void SDIO_ConfigNormIntSig(SDIOHOST_TypeDef *SDIOx, u32 SDIO_IT, u32 NewState);
void SDIO_ConfigErrIntSig(SDIOHOST_TypeDef *SDIOx, u32 SDIO_IT, u32 NewState);
void SDIO_ConfigNormIntSts(SDIOHOST_TypeDef *SDIOx, u32 SDIO_IT, u32 NewState);
void SDIO_ConfigErrIntSts(SDIOHOST_TypeDef *SDIOx, u32 SDIO_IT, u32 NewState);
u32 SDIO_GetNormSts(SDIOHOST_TypeDef *SDIOx);
u32 SDIO_GetErrSts(SDIOHOST_TypeDef *SDIOx);
void SDIO_ClearNormSts(SDIOHOST_TypeDef *SDIOx, u32 SDIO_IT);
void SDIO_ClearErrSts(SDIOHOST_TypeDef *SDIOx, u32 SDIO_IT);

/* SDMMC Commands management functions */
u32 SDMMC_CmdBlockLength(SDIOHOST_TypeDef *SDIOx, u32 BlockSize);
u32 SDMMC_CmdReadSingleBlock(SDIOHOST_TypeDef *SDIOx, u32 ReadAdd);
u32 SDMMC_CmdReadMultiBlock(SDIOHOST_TypeDef *SDIOx, u32 ReadAdd);
u32 SDMMC_CmdWriteSingleBlock(SDIOHOST_TypeDef *SDIOx, u32 WriteAdd);
u32 SDMMC_CmdWriteMultiBlock(SDIOHOST_TypeDef *SDIOx, u32 WriteAdd);
u32 SDMMC_CmdSDEraseStartAdd(SDIOHOST_TypeDef *SDIOx, u32 StartAdd);
u32 SDMMC_CmdSDEraseEndAdd(SDIOHOST_TypeDef *SDIOx, u32 EndAdd);
u32 SDMMC_CmdErase(SDIOHOST_TypeDef *SDIOx, u32 BlockCnt);
u32 SDMMC_CmdStopTransfer(SDIOHOST_TypeDef *SDIOx);
u32 SDMMC_CmdSelDesel(SDIOHOST_TypeDef *SDIOx, u32 Addr);
u32 SDMMC_CmdGoIdleState(SDIOHOST_TypeDef *SDIOx);
u32 SDMMC_CmdOperCond(SDIOHOST_TypeDef *SDIOx);
u32 SDMMC_CmdAppCommand(SDIOHOST_TypeDef *SDIOx, u32 Argument);
u32 SDMMC_CmdAppOperCommand(SDIOHOST_TypeDef *SDIOx, u32 Argument);
u32 SDMMC_CmdBusWidth(SDIOHOST_TypeDef *SDIOx, u32 BusWidth);
u32 SDMMC_CmdSetWrBlkEraseCnt(SDIOHOST_TypeDef *SDIOx, u32 BlockCnt);
u32 SDMMC_CmdSendSCR(SDIOHOST_TypeDef *SDIOx);
u32 SDMMC_CmdSendCID(SDIOHOST_TypeDef *SDIOx);
u32 SDMMC_CmdSendCSD(SDIOHOST_TypeDef *SDIOx, u32 Argument);
u32 SDMMC_CmdSetRelAdd(SDIOHOST_TypeDef *SDIOx);
u32 SDMMC_CmdSendStatus(SDIOHOST_TypeDef *SDIOx, u32 Argument);
u32 SDMMC_CmdStatusRegister(SDIOHOST_TypeDef *SDIOx);
u32 SDMMC_CmdSwitch(SDIOHOST_TypeDef *SDIOx, u32 Argument);

/* SDMMC Commands management functions */
u32 SDIO_CmdSendOpCond(SDIOHOST_TypeDef *SDIOx, u32 Ocr);
u32 SDIO_CmdRWDirect(SDIOHOST_TypeDef *SDIOx, u32 Argument);
u32 SDIO_CmdRWExtended(SDIOHOST_TypeDef *SDIOx, u32 Argument);

/* SDMMC Responses management functions */
u32 SDIO_WaitResp(SDIOHOST_TypeDef *SDIOx, u8 RespType, u32 TimeOutUs);
u32 SDMMC_CheckErrResp1(u32 resp);
u32 SDMMC_CheckErrResp5(u32 resp);
u32 SDMMC_CheckErrResp6(u32 resp);

/**
 * @}
 */

#endif