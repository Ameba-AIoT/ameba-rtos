/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_GDMA_H_
#define _AMEBA_GDMA_H_

/** @addtogroup GDMA
  * @verbatim
  *******************************************************************************************
  * Introduction
  *******************************************************************************************
  * Ameba supports AP and NP access GDMA. GDMA in AP or NP has eight channels,
  * The FIFO size of channel 0 and channel is 128bytes, and that of other channels is 64bytes.
  *
  * GDMA0 in AP:
  *		- Base Address: GDMA0_REG_BASE
  *
  *		- Channel index: 0~7
  *
  *		- Block size range: 1~2097151
  *
  *		- Transfer Type and Flow Control:
  *                  TTFCMemToMem	(Memory to Memory)
  *                  TTFCMemToPeri	(Memory to Peripheral)
  *                  TTFCPeriToMem	(Peripheral to Memory)
  *                  TTFCPeriToPeri	(Peripheral to Peripheral)
  *                  TTFCPeriToMem_PerCtrl      (Peripheral to Memory, but peri as flow controller)
  *                  TTFCPeriToPeri_SrcPerCtrl  (Peripheral to Peripheral, src peri as flow controller)
  *                  TTFCMemToPeri_PerCtrl      (Memory to Peripheral, but peri as flow controller)
  *                  TTFCPeriToPeri_DstPerCtrl  (Peripheral to Peripheral, dst peri as flow controller)
  *		- Source and destination data width:
  *                  TrWidthOneByte
  *                  TrWidthTwoBytes
  *                  TrWidthFourBytes
  *
  *		- Source and destination burst transaction length:
  *                  MsizeOne		(One Byte)
  *                  MsizeFour		(Four Bytes)
  *                  MsizeEight		(Eight Bytes)
  *                  MsizeSixteen	(sixteen Bytes)
  *
  *		- IRQ:
  *                  GDMA0_CHANNEL0_IRQ,
  *                  GDMA0_CHANNEL1_IRQ,
  *                  GDMA0_CHANNEL2_IRQ,
  *                  GDMA0_CHANNEL3_IRQ,
  *                  GDMA0_CHANNEL4_IRQ,
  *                  GDMA0_CHANNEL5_IRQ,
  *                  GDMA0_CHANNEL6_IRQ,
  *                  GDMA0_CHANNEL7_IRQ,
  *
  *
  * GDMA0 in NP:
  *		- Base Address: GDMA0_REG_BASE
  *
  *		- Channel index: 0~7
  *
  *		- Block size range: 1~2097151
  *
  *		- Transfer Type and Flow Control:
  *                  TTFCMemToMem	(Memory to Memory)
  *                  TTFCMemToPeri	(Memory to Peripheral)
  *                  TTFCPeriToMem	(Peripheral to Memory)
  *                  TTFCPeriToPeri	(Peripheral to Peripheral)
  *                  TTFCPeriToMem_PerCtrl      (Peripheral to Memory, but peri as flow controller)
  *                  TTFCPeriToPeri_SrcPerCtrl  (Peripheral to Peripheral, src peri as flow controller)
  *                  TTFCMemToPeri_PerCtrl      (Memory to Peripheral, but peri as flow controller)
  *                  TTFCPeriToPeri_DstPerCtrl  (Peripheral to Peripheral, dst peri as flow controller)
  *
  *		- Source and destination data width:
  *                  TrWidthOneByte
  *                  TrWidthTwoBytes
  *                  TrWidthFourBytes
  *
  *		- Source and destination burst transaction length:
  *                  MsizeOne		(One Byte)
  *                  MsizeFour		(Four Bytes)
  *                  MsizeEight		(Eight Bytes)
  *                  MsizeSixteen	(sixteen Bytes)
  *		- IRQ:
  *                  GDMA0_CHANNEL0_IRQ,
  *                  GDMA0_CHANNEL1_IRQ
  *                  GDMA0_CHANNEL2_IRQ
  *                  GDMA0_CHANNEL3_IRQ
  *                  GDMA0_CHANNEL4_IRQ
  *                  GDMA0_CHANNEL5_IRQ
  *                  GDMA0_CHANNEL6_IRQ
  *                  GDMA0_CHANNEL7_IRQ
  *
  *
  *****************************************************************************************
  * how to use GDMA
  *****************************************************************************************
  *      To use the GDMA, the following steps are mandatory:
  *
  *      1. Allocate a GDMA channel using the follwoing function.
  *			GDMA_ChnlAlloc(u32 GDMA_Index, IRQ_FUN IrqFun, u32 IrqData, u32 IrqPriority)
  *
  *			@note  This function also includes the following operation:
  *				- register irq handler if use interrupt mode
  *				- enable NVIC interrupt
  *				- register the GDMA channel to use
  *				- enable GDMA peripheral clock
  *
  *      2. Program GDMA index, GDMA channel, data width, Msize, transfer direction, address increment mode,
  *          hardware handshake interface, reload control, interrupt type, block size, multi-block configuration
  *          and the source and destination address using the GDMA_Init() function.
  *
  *      3. Enable the corresponding interrupt using the function.
  *			GDMA_INTConfig() and register the uart irq handler if you need to use interrupt mode.
  *
  *			@note  This step is included in the "step 2"(GDMA_Init()).
  *
  *      4. Enable GDMA using function GDMA_Cmd().
  *
  *
  * @endverbatim
  */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup GDMA
  * @brief GDMA driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup GDMA_Register_Definitions GDMA Register Definitions
  * @{
  */

/** @defgroup GDMA_SARx
  * @brief Channel x Source Address Register
  * @{
  */
#define GDMA_MASK_SARx   ((u32)0xFFFFFFFF << 0)           /*!< R/W Unknown  Source Address of DMA transfer.Updated after each source transfer. The SINC field in the CTLx register determines whether the address increments, or is left unchanged on every source transfer throughout the block transfer.*/
#define GDMA_SARx(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GDMA_GET_SARx(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup GDMA_CURR_SARx
  * @brief Channel x source reading address at present
  * @{
  */
#define GDMA_MASK_CURR_SARx   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  The read back value is source reading address at present*/
#define GDMA_CURR_SARx(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GDMA_GET_CURR_SARx(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup GDMA_DARx
  * @brief Channel x Destination Address Register
  * @{
  */
#define GDMA_MASK_DARx   ((u32)0xFFFFFFFF << 0)           /*!< R/W Unknown  Destination address of DMA transfer.Updated after each destination transfer. The DINC field in the CTLx register determines whether the address increments, or is left unchanged on every destination transfer throughout the block transfer.*/
#define GDMA_DARx(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GDMA_GET_DARx(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup GDMA_CURR_DARx
  * @brief Channel x destination writing address at present
  * @{
  */
#define GDMA_MASK_CURR_DARx   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  The read back value is destination writing address at present*/
#define GDMA_CURR_DARx(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GDMA_GET_CURR_DARx(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup GDMA_LLPx_L
  * @brief Channel x low-32 bits Linked List Pointer Register
  * @{
  */
#define GDMA_MASK_LLPx_L_LOC   ((u32)0x3FFFFFFF << 2)           /*!< R/W Unknown  Starting Address In Memory of next LLI if block chaining is enabled. Note that the two LSBs of the starting address are not stored because the address is assumed to be aligned to a 32-bit boundary.LLI accesses are always 32-bit accesses aligned to 32-bit boundaries and cannot be changed or programmed to anything other than 32-bit.*/
#define GDMA_LLPx_L_LOC(x)     (((u32)((x) & 0x3FFFFFFF) << 2))
#define GDMA_GET_LLPx_L_LOC(x) ((u32)(((x >> 2) & 0x3FFFFFFF)))
/** @} */

/** @defgroup GDMA_CTLx_L
  * @brief Channel x low-32 bits Control Register
  * @{
  */
#define GDMA_BIT_CTLx_L_LLP_SRC_EN      ((u32)0x00000001 << 28)           /*!< R/W 0x0  Block chanining is enabled on the source side only if the LLP_SRC_EN field id high and LLPx_L.LOC is non-zero; for more information,see 7.4.1.1 "Block Chaining Using Linked Lists" Dependencies:This field does not exist if the configuration parameterDMAC_CHx_MULTI_BLK_EN is not selected or if DMAC_CHx_HC_LLP is selected; in this case, the read-back value is always 0.*/
#define GDMA_BIT_CTLx_L_LLP_DST_EN      ((u32)0x00000001 << 27)           /*!< R/W 0x0  Block chanining is enabled on the destination side only if the LLP_DST_EN field id high and LLPx_L.LOC is non-zero; for more information,see 7.4.1.1 "Block Chaining Using Linked Lists" Dependencies:This field does not exist if the configuration parameterDMAC_CHx_MULTI_BLK_EN is not selected or if DMAC_CHx_HC_LLP is selected; in this case, the read-back value is always 0.*/
#define GDMA_MASK_CTLx_L_TT_FC          ((u32)0x00000007 << 20)           /*!< R/W 0x0  Transfer Type and Flow Control.The following transfer types are supported. • Memory to Memory • Memory to Peripheral • Peripheral to Memory • Peripheral to Peripheral Flow Control can be assigned to the RTK_DMAC, the source peripheral, or the destination peripheral. Table 6-4 lists the decoding for this field. For more information on transfer types and flow control, refer to "Setup/Operation of RTK_DMAC Transfers". Reset Value: 3'h0 Dependencies: If hardware handshake isn't support, then TT_FC can only be set to 3'b0. TT_FC[2] can only be read.*/
#define GDMA_CTLx_L_TT_FC(x)            (((u32)((x) & 0x00000007) << 20))
#define GDMA_GET_CTLx_L_TT_FC(x)        ((u32)(((x >> 20) & 0x00000007)))
#define GDMA_BIT_CTLx_L_DST_SCATTER_EN  ((u32)0x00000001 << 18)           /*!< R/W 0x0  Destination scatter enable bit:0 = Scatter disabled1 = Scatter enabledScatter on the destination side is applicable only when the CTLx_L.DINC bit indicates an incrementing address control and DARx should be aligned with DST_TR_WIDTH. Dependencies: This field does not exist if DMAC_CHx_DST_SCA_EN is not selected.*/
#define GDMA_BIT_CTLx_L_SRC_GATHER_EN   ((u32)0x00000001 << 17)           /*!< R/W 0x0  Source gather enable bit: 0 = Gather disabled 1 = Gather enabled Gather on the source side is applicable only when the CTLx_L.SINC bit indicates an incrementing address control and SARx should be aligned with SRC_TR_WIDTH. Dependencies: This field does not exist if DMAC_CHx_SRC_GAT_EN is not selected.*/
#define GDMA_MASK_CTLx_L_SRC_MSIZE      ((u32)0x00000007 << 14)           /*!< R/W 0x1  Source Burst Transaction Length. Number of data items, each of width CTLx_L.SRC_TR_WIDTH if source is not memory, otherwise each of width DMAC_TR_WIDTH_DEFAULT, to be read from the source every time a source burst transaction request is made from either the corresponding hardware or software handshaking interface. Read burst transaction length on bus (ARLen) will not exceed this value, but may be less than this value. If SINC is setted to "no change", this value should not be greater than 16 to conform to the AXI protocol. If enabled 'DMAC_CHx_MSIZE_BIGGER_THAN_FIFO_DEPTH' feature, to improve performance, can set CTLx_L.SRC_MSIZE to a high value to make DMAC send longer burst length command. But to avoid deadlock, if Source slave and Destination slave is the same slave, should not set CTLx_L.SRC_MSIZE over than FIFO_depth; Table 6-2-0 lists the decoding for this field.*/
#define GDMA_CTLx_L_SRC_MSIZE(x)        (((u32)((x) & 0x00000007) << 14))
#define GDMA_GET_CTLx_L_SRC_MSIZE(x)    ((u32)(((x >> 14) & 0x00000007)))
#define GDMA_MASK_CTLx_L_DEST_MSIZE     ((u32)0x00000007 << 11)           /*!< R/W 0x1  Destination Burst Transaction Length. Number of data items, each of width DATA_WIDTH, to be written to the destination every time a destination burst transaction request is made from either the corresponding hardware or software handshaking interface. Write burst transaction length on bus (AWLen) will not exceed this value, but may be less than this value. If DINC is setted to "no change", this value should not be greater than 16 to conform to the AXI protocol. If enabled 'DMAC_CHx_MSIZE_BIGGER_THAN_FIFO_DEPTH' feature, to improve performance, can set CTLx_L.SRC_MSIZE to a high value to make DMAC send longer burst length command. But to avoid deadlock, if Source slave and Destination slave is the same slave, should not set CTLx_L.SRC_MSIZE over than FIFO_depth; Table 6-2-1 lists the decoding for this field.*/
#define GDMA_CTLx_L_DEST_MSIZE(x)       (((u32)((x) & 0x00000007) << 11))
#define GDMA_GET_CTLx_L_DEST_MSIZE(x)   ((u32)(((x >> 11) & 0x00000007)))
#define GDMA_MASK_CTLx_L_SINC           ((u32)0x00000003 << 9)            /*!< R/W 0x0  Source Address Increment. Indicates whether to increment the source address on every source transfer. If the device is fetching data from a source peripheral FIFO with a fixed address, then set this field to "No change."If source is memory appointed by CTLx_L.TT_FC, and set SINC=No change, then SRC_TR_WIDTH must be set to BUS width. And DMAC will read data from the fixed memory address until block transfer finish, but It is not recommended to read memory like this, because some memory controller not support fixed burst transaction and may transform the fixed burst transaction to incremental burst transaction. 00 = Increment 01 =Not support 1x = No change NOTE: Incrementing is done for alignment to the next CTLx_L.SRC_TR_WIDTH boundary.*/
#define GDMA_CTLx_L_SINC(x)             (((u32)((x) & 0x00000003) << 9))
#define GDMA_GET_CTLx_L_SINC(x)         ((u32)(((x >> 9) & 0x00000003)))
#define GDMA_MASK_CTLx_L_DINC           ((u32)0x00000003 << 7)            /*!< R/W 0x0  Destination Address Increment. Indicates whether to increment destination address on every destination transfer. If your device is writing data to a destination peripheral FIFO with a fixed address, then set this field to "No change."If destination is memory appointed by CTLx_L.TT_FC, and set DINC=No change, then DST_TR_WIDTH must be set to BUS width. And DMAC will write data to the fixed memory address until block transfer finish, but It is not recommended to write memory like this, because some memory controller not support fixed burst transaction and may transform the fixed burst transaction to incremental burst transaction. 00 = Increment 01=Not Support 1x = No change NOTE: Incrementing is done for alignment to the next CTLx_L.DST_TR_WIDTH boundary.*/
#define GDMA_CTLx_L_DINC(x)             (((u32)((x) & 0x00000003) << 7))
#define GDMA_GET_CTLx_L_DINC(x)         ((u32)(((x >> 7) & 0x00000003)))
#define GDMA_MASK_CTLx_L_SRC_TR_WIDTH   ((u32)0x00000007 << 4)            /*!< R/W 0x0  Source Transfer Width. Table 6-3 lists the decoding for this field.It can't be mapped to AXI bus data width directly. Since AXI bus data width is fixed (such as 32bit/64bit), AXI use byte enable signal for byte/half word transfer (still burst). So, the SRC_TR_WIDTH is only used for total block data size calculation and streaming mode byte/half word transfer (for FIFO transfer). Note: SRC_TR_WIDTH must not be bigger than BUS width.*/
#define GDMA_CTLx_L_SRC_TR_WIDTH(x)     (((u32)((x) & 0x00000007) << 4))
#define GDMA_GET_CTLx_L_SRC_TR_WIDTH(x) ((u32)(((x >> 4) & 0x00000007)))
#define GDMA_MASK_CTLx_L_DST_TR_WIDTH   ((u32)0x00000007 << 1)            /*!< R/W 0x0  Destination Transfer Width.Table 6-3 lists the decoding for this field. It can't be mapped to AXI bus data width directly. For a non-memory peripheral, typically the peripheral (destination) FIFO width.Note: DST_TR_WIDTH must not be bigger than BUS width*/
#define GDMA_CTLx_L_DST_TR_WIDTH(x)     (((u32)((x) & 0x00000007) << 1))
#define GDMA_GET_CTLx_L_DST_TR_WIDTH(x) ((u32)(((x >> 1) & 0x00000007)))
#define GDMA_BIT_CTLx_L_INT_EN          ((u32)0x00000001 << 0)            /*!< R/W 0x1  Interrupt Enable Bit. If set, then all interrupt-generating sources are enabled. Functions as a global mask bit for all interrupts for the channel; raw* interrupt registers still assert if CTLx_L.INT_EN = 0.*/
/** @} */

/** @defgroup GDMA_CTLx_H
  * @brief Channel x high-32 bits Control Register
  * @{
  */
#define GDMA_MASK_CTLx_H_TRANS_DATA_CNT   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  Data Transfered Count.Once the transfer starts, the read-back value is the total number of data bytes already write to destination memory or peripheral.*/
#define GDMA_CTLx_H_TRANS_DATA_CNT(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GDMA_GET_CTLx_H_TRANS_DATA_CNT(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
#define GDMA_MASK_CTLx_H_BLOCK_TS         ((u32)0x001FFFFF << 0)           /*!< W 0x2  Block Transfer Size.When the RTK_DMAC is the flow controller, the user writes this field before the channel is enabled in order to indicate the block size. The number programmed into BLOCK_TS indicates the total number of single transactions to perform for every block transfer; a single transaction is mapped to a single AXI beat. This field is write-only, number of block size programmed into CTLx_H.BLOCK_TS can be read from SGRx_H.BLOCK_TS_L and DSRx_H.BLOCK_TS_H. Width: The width of the single transaction is determined by CTLX_L.SRC_TR_WIDTH.When the source or destination peripheral is assigned as the flow controller, then the maximum block size that can be read back saturates at DMAC_CHx_MAX_BLK_SIZE, but the actual block size can be greater. b = log2(DMAC_CHx_MAX_BLK_SIZE + 1) -1*/
#define GDMA_CTLx_H_BLOCK_TS(x)           (((u32)((x) & 0x001FFFFF) << 0))
#define GDMA_GET_CTLx_H_BLOCK_TS(x)       ((u32)(((x >> 0) & 0x001FFFFF)))
/** @} */

/** @defgroup GDMA_CFGx_L
  * @brief Channel x low-32 bits Configuration Register
  * @{
  */
#define GDMA_BIT_CFGx_L_RELOAD_DST    ((u32)0x00000001 << 31)          /*!< R/W 0x0  Automatic Destination Reload. The DARx register can be automatically reloaded from its initial value at the end of every block for multi-block transfers. A new block transfer is then initiated.This register does not exist if the configuration parameter DMAC_CHx_MULTI_BLK_EN is not selected; in this case, the read-back value is always 0.*/
#define GDMA_BIT_CFGx_L_RELOAD_SRC    ((u32)0x00000001 << 30)          /*!< R/W 0x0  Automatic Source Reload. The SARx register can be automatically reloaded from its initial value at the end of every block for multi-block transfers. A new block transfer is then initiated. This field does not exist if the configuration parameter DMAC_CHx_MULTI_BLK_EN is not selected; in this case, the read-back value is always 0.*/
#define GDMA_BIT_CFGx_L_SRC_HS_POL    ((u32)0x00000001 << 19)          /*!< R/W 0x0  Source Handshaking Interface Polarity. 0 = Active high 1 = Active low*/
#define GDMA_BIT_CFGx_L_DST_HS_POL    ((u32)0x00000001 << 18)          /*!< R/W 0x0  Destination Handshaking Interface Polarity. 0 = Active high 1 = Active low For information on this, refer to "Handshaking Interface" .*/
#define GDMA_BIT_CFGx_L_HS_SEL_SRC    ((u32)0x00000001 << 11)          /*!< R/W 0x1  Source Software or Hardware Handshaking Select. This register selects which of the handshaking interfaces - hardware or software - is active for source requests on this channel. 0 = Hardware handshaking interface. Software-initiated transaction requests are ignored. 1 = Software handshaking interface. Hardware-initiated transaction requests are ignored. If the source peripheral is memory, then this bit is ignored.*/
#define GDMA_BIT_CFGx_L_HS_SEL_DST    ((u32)0x00000001 << 10)          /*!< R/W 0x1  Destination Software or Hardware Handshaking Select. This register selects which of the handshaking interfaces - hardware or software - is active for destination requests on this channel. 0 = Hardware handshaking interface. Software-initiated transaction requests are ignored. 1 = Software handshaking interface. Hardware- initiated transaction requests are ignored. If the destination peripheral is memory, then this bit is ignored.*/
#define GDMA_BIT_CFGx_L_FIFO_EMPTY    ((u32)0x00000001 << 9)           /*!< R 0x1  Indicates if there is data left in the channel FIFO. 1 = Channel FIFO empty 0 = Channel FIFO not empty*/
#define GDMA_BIT_CFGx_L_CH_SUSP       ((u32)0x00000001 << 8)           /*!< R/W 0x0  Channel Suspend. Suspends all DMA data transfers from the source until this bit is cleared. There is no guarantee that the current transaction will complete. Can also be used in conjunction with CFGx_l.INACTIVE to cleanly disable a channel without losing any data. 0 = Not suspended 1 = Suspend DMA transfer from the source. This field will be reset to 0 after "Disabling a Channel Prior to Transfer Completion" automatically by hardware.For more information, refer to "7.7 Disabling a Channel Prior to Transfer Completion" .  Note: if user not define DMAC_CH*_SUPPORT_SUSPEND, it only can be readed.*/
#define GDMA_MASK_CFGx_L_CH_PRIOR     ((u32)0x0000000F << 4)           /*!< R/W 0x0  Channel priority. RTK DMAC supports three kinds of channel priority now, fixed priority, round robin priority and programmable priority. The default way is fixed priority to reduce area, and channel0 has the highest priority, channel15 has the lowest priority. Changing the definition of parameter FIX_PRIORITY or Programmable_PRIORITY can change priority way. If RTK DMAC is configured to support Programmable priority, this field will be used to set priority between channels. CH_PRIOR's legal value is 0 ~ (DMAC_NUM_CHANNELS-1), and 0 is the highest priority value, (DMAC_NUM_CHANNELS-1) is the lowest priroty value. If more than one channel are set with the same priroty value, the channel with small channel number index has higher priority. If DMAC is configured to support fix priority or round-robin priority, this field is not exist. Note: if user not define ProgrammablePriority, it only can be readed.*/
#define GDMA_CFGx_L_CH_PRIOR(x)       (((u32)((x) & 0x0000000F) << 4))
#define GDMA_GET_CFGx_L_CH_PRIOR(x)   ((u32)(((x >> 4) & 0x0000000F)))
#define GDMA_BIT_CFGx_L_DST_PCTL_OVER ((u32)0x00000001 << 2)           /*!< R 0x1  DMA destination bus protocol finish, indicating the current write command has finished.*/
#define GDMA_BIT_CFGx_L_SRC_PCTL_OVER ((u32)0x00000001 << 1)           /*!< R 0x1  DMA Source bus protocol finish, indicating the current read command has finished.*/
#define GDMA_BIT_CFGx_L_INACTIVE      ((u32)0x00000001 << 0)           /*!< R 0x1  Indicates if the channel is inactive. Can be used in conjunction with CFGx_L.CH_SUSP to cleanly disable a channel when destination is peripheral. For more information, refer to "Disabling a Channel Prior to Transfer Completion" . 1 = Channel Inactive for axi interface 0 = Channel not Inactive for axi interface*/
/** @} */

/** @defgroup GDMA_CFGx_H
  * @brief Channel x high-32 bits Configuration Register
  * @{
  */
#define GDMA_BIT_CFGx_H_EXTENDED_DEST_PER1 ((u32)0x00000001 << 16)           /*!< R/W 0x0  The extened 4th bit of DEST_PER when hardware handshake interfaces number is configured to 32.*/
#define GDMA_BIT_CFGx_H_EXTENDED_SRC_PER1  ((u32)0x00000001 << 15)           /*!< R/W 0x0  The extened 4th bit of SRC_PER when hardware handshake interfaces number is configured to 32.*/
#define GDMA_MASK_CFGx_H_DEST_PER          ((u32)0x0000000F << 11)           /*!< R/W 0x0  Assigns a hardware handshaking interface(0 - DMAC_NUM_HS_INT-1) to the destination of channel x if the FGx.HS_SEL_DST field is 0; otherwise, this field is ignored. The channel can then communicate with the destinationperipheral connected to that interface through the assignedhardware handshaking interface. NOTE1: For correct DMA operation, only one peripheral (sourceor destination) should be assigned to the same handshaking interface NOTE2: This field does not exist if the configuration parameterDMAC_NUM_HS_INT is set to 0.*/
#define GDMA_CFGx_H_DEST_PER(x)            (((u32)((x) & 0x0000000F) << 11))
#define GDMA_GET_CFGx_H_DEST_PER(x)        ((u32)(((x >> 11) & 0x0000000F)))
#define GDMA_MASK_CFGx_H_SRC_PER           ((u32)0x0000000F << 7)            /*!< R/W 0x0  Assigns a hardware handshaking interface(0 - DMAC_NUM_HS_INT-1) to the source of channel x if the CFGx.HS_SEL_SRC field is 0; otherwise, this field is ignored. The channel can then communicate with the source peripheral connected to that interface through the assigned hardware handshaking interface. NOTE1: For correct RTK_DMAC operation, only one peripheral (source or destination) should be assigned to the same handshaking interface. NOTE2: This field does not exist if the configuration parameter DMAC_NUM_HS_INT is set to 0.*/
#define GDMA_CFGx_H_SRC_PER(x)             (((u32)((x) & 0x0000000F) << 7))
#define GDMA_GET_CFGx_H_SRC_PER(x)         ((u32)(((x >> 7) & 0x0000000F)))
#define GDMA_BIT_CFGx_H_PROTCTL            ((u32)0x00000001 << 3)            /*!< R/W 0x1  Protection Control.Set the bit 35 to 0 will enable the channel secure DMA transfer. Set the bit 35 to 1 will enable the channel non-secure DMAC transfer. Note: if user not define DMAC_SECURE_MODE, it only can be readed.*/
#define GDMA_BIT_CFGx_H_FCMODE             ((u32)0x00000001 << 0)            /*!< R/W 0x0  Flow Control Mode. Determines when source transaction requests are serviced when the Destination Peripheral is the flow controller. 0 = Source transaction requests are servied when they occur. Data pre-fetching is enabled. 1 = Source transaction requests are not serviced until a destination transaction request occurs. Data pre-fetching is disabled. Note: Please keep this bit 0 when Transfer Type and Flow Control is not "destination peripheral is the flow controller and transfer type is peripheral to peripheral" (TT_FC!=7).*/
/** @} */

/** @defgroup GDMA_SGRx_L
  * @brief Channel x low-32 bits Source Gather Register
  * @{
  */
#define GDMA_MASK_SGRx_L_SGC   ((u32)0x00000FFF << 20)           /*!< R/W Unknown  Source gather count. Source contiguous transfer count between successive gather boundaries. Note: if user not define DMAC_SRC_GAT_EN, it only can be readed.*/
#define GDMA_SGRx_L_SGC(x)     (((u32)((x) & 0x00000FFF) << 20))
#define GDMA_GET_SGRx_L_SGC(x) ((u32)(((x >> 20) & 0x00000FFF)))
#define GDMA_MASK_SGRx_L_SGI   ((u32)0x000FFFFF << 0)            /*!< R/W Unknown  Source gather interval. Note: if user not define DMAC_SRC_GAT_EN, it only can be readed.*/
#define GDMA_SGRx_L_SGI(x)     (((u32)((x) & 0x000FFFFF) << 0))
#define GDMA_GET_SGRx_L_SGI(x) ((u32)(((x >> 0) & 0x000FFFFF)))
/** @} */

/** @defgroup GDMA_SGRx_H
  * @brief Channel x high-32 bits Source Gather Register
  * @{
  */
#define GDMA_MASK_SGRx_H_BLOCK_TS_L   ((u32)0x0000FFFF << 16)           /*!< R 0x2  Low 16-bits of Block Transfer Size.The number programmed into CTLX.BLOCK_TS, which indicates the total number of single transactions to perform for every block transfer, is separated into low 16 bits and high part. The low 16 bits is stored into SGRx.BLOCK_TS_L while the high part is stored into DSRx.BLOCK_TS_H Width: The width of the single transaction is determined by CTLx.SRC_TR_WIDTH.*/
#define GDMA_SGRx_H_BLOCK_TS_L(x)     (((u32)((x) & 0x0000FFFF) << 16))
#define GDMA_GET_SGRx_H_BLOCK_TS_L(x) ((u32)(((x >> 16) & 0x0000FFFF)))
/** @} */

/** @defgroup GDMA_DSRx_L
  * @brief Channel x low-32 bits Destination Scatter Register
  * @{
  */
#define GDMA_MASK_DSRx_L_DSC   ((u32)0x00000FFF << 20)           /*!< R/W Unknown  Destination scatter count. Destination contiguous transfer count between successive scatter boundaries. Note: if user not define DMAC_DST_SCA_EN, it only can be readed.*/
#define GDMA_DSRx_L_DSC(x)     (((u32)((x) & 0x00000FFF) << 20))
#define GDMA_GET_DSRx_L_DSC(x) ((u32)(((x >> 20) & 0x00000FFF)))
#define GDMA_MASK_DSRx_L_DSI   ((u32)0x000FFFFF << 0)            /*!< R/W Unknown  Destination scatter interval.Note: if user not define DMAC_DST_SCA_EN, it only can be readed.*/
#define GDMA_DSRx_L_DSI(x)     (((u32)((x) & 0x000FFFFF) << 0))
#define GDMA_GET_DSRx_L_DSI(x) ((u32)(((x >> 0) & 0x000FFFFF)))
/** @} */

/** @defgroup GDMA_DSRx_H
  * @brief Channel x high-32 bits Destination Scatter Register
  * @{
  */
#define GDMA_MASK_DSRx_H_BLOCK_TS_H   ((u32)0x0000001F << 16)           /*!< R 0x0  High part of Block Transfer SizeThe number programmed into CTLx_H.BLOCK_TS, which indicates the total number of single transactions to perform for every block transfer, is separated into low 16 bits and high 16 bits. The low 16-bits is stored into SGRx_H.BLOCK_TS_LOW while the high 16-bits is stored into DSRx_H.BLOCK_TS_HIGH Width: The width of the single transaction is determined by CTLx_L.SRC_TR_WIDTH.b = log2 (DMAC_CHx_MAX_BLK_SIZE). Dependencies: The maximum value of DMAC_CHx_MAX_BLK_SIZE is 2 ^ (31-log2 (DMAC_DATA_WIDTH/8)) - 1.*/
#define GDMA_DSRx_H_BLOCK_TS_H(x)     (((u32)((x) & 0x0000001F) << 16))
#define GDMA_GET_DSRx_H_BLOCK_TS_H(x) ((u32)(((x >> 16) & 0x0000001F)))
/** @} */

/** @defgroup GDMA_RAWTFR_L
  * @brief Raw Status for IntTfr Interrupt
  * @{
  */
#define GDMA_MASK_RAWTFR_L_RAW_TFR   ((u32)0x0000FFFF << 0)           /*!< R 0x0  Raw interrupt status for chn0-chn15*/
#define GDMA_RAWTFR_L_RAW_TFR(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define GDMA_GET_RAWTFR_L_RAW_TFR(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup GDMA_RAWBLOCK_L
  * @brief Raw Status for IntBlock Interrupt
  * @{
  */
#define GDMA_MASK_RAWBLOCK_L_RAW_BLOCK   ((u32)0x0000FFFF << 0)           /*!< R 0x0  Raw interrupt status for chn0-chn15*/
#define GDMA_RAWBLOCK_L_RAW_BLOCK(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define GDMA_GET_RAWBLOCK_L_RAW_BLOCK(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup GDMA_RAWERR_L
  * @brief Raw Status for IntErr Interrupt(protocol error)
  * @{
  */
#define GDMA_MASK_RAWERR_L_RAW_ERR   ((u32)0x0000FFFF << 0)           /*!< R 0x0  Raw interrupt status for chn0-chn15. (Protocol error)*/
#define GDMA_RAWERR_L_RAW_ERR(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define GDMA_GET_RAWERR_L_RAW_ERR(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup GDMA_RAWERR_H
  * @brief Raw Status for non-secure access secure register error if defined
  * @{
  */
#define GDMA_MASK_RAWERR_H_RAW_ERR   ((u32)0x0000FFFF << 0)           /*!< R 0x0  Raw interrupt status. (Non-secure access secure register error) if DMAC_SECURE_MODE has been defined; else it is reserved.*/
#define GDMA_RAWERR_H_RAW_ERR(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define GDMA_GET_RAWERR_H_RAW_ERR(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup GDMA_STATUSTFR_L
  * @brief Status for IntTfr Interrupt
  * @{
  */
#define GDMA_MASK_STATUSTFR_L_STATUS_TFR   ((u32)0x0000FFFF << 0)           /*!< R 0x0  Interrupt status for chn0-chn15*/
#define GDMA_STATUSTFR_L_STATUS_TFR(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define GDMA_GET_STATUSTFR_L_STATUS_TFR(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup GDMA_STATUSBLOCK_L
  * @brief Status for IntBlock Interrupt
  * @{
  */
#define GDMA_MASK_STATUSBLOCK_L_STATUS_BLOCK   ((u32)0x0000FFFF << 0)           /*!< R 0x0  Interrupt status for chn0-chn15*/
#define GDMA_STATUSBLOCK_L_STATUS_BLOCK(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define GDMA_GET_STATUSBLOCK_L_STATUS_BLOCK(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup GDMA_STATUSERR_L
  * @brief Status for IntErr Interrupt
  * @{
  */
#define GDMA_MASK_STATUSERR_L_STATUS_ERR   ((u32)0x0000FFFF << 0)           /*!< R 0x0  Interrupt status for chn0-chn15. (Protocol error)*/
#define GDMA_STATUSERR_L_STATUS_ERR(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define GDMA_GET_STATUSERR_L_STATUS_ERR(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup GDMA_STATUSERR_H
  * @brief Status for non-secure access secure register error if defined
  * @{
  */
#define GDMA_MASK_STATUSERR_H_STATUS_ERR   ((u32)0x0000FFFF << 0)           /*!< R 0x0  Interrupt status. (Non-secure access secure register error, Sec Error) if DMAC_SECURE_MODE has been defined; else it is reserved.*/
#define GDMA_STATUSERR_H_STATUS_ERR(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define GDMA_GET_STATUSERR_H_STATUS_ERR(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup GDMA_MASKTFR_L
  * @brief Mask for IntTfr Interrupt (chn0-7)
  * @{
  */
#define GDMA_MASK_MASKTFR_L_INT_MASK_WE   ((u32)0x000000FF << 8)           /*!< W 0x0  Interrupt Mask Write Enable(chn0-chn7) 0 = write disabled 1 = write enabled*/
#define GDMA_MASKTFR_L_INT_MASK_WE(x)     (((u32)((x) & 0x000000FF) << 8))
#define GDMA_GET_MASKTFR_L_INT_MASK_WE(x) ((u32)(((x >> 8) & 0x000000FF)))
#define GDMA_MASK_MASKTFR_L_INT_MASK      ((u32)0x000000FF << 0)           /*!< R/W 0x0  Interrupt Mask(chn0-chn7) 0 = masked 1 = unmasked*/
#define GDMA_MASKTFR_L_INT_MASK(x)        (((u32)((x) & 0x000000FF) << 0))
#define GDMA_GET_MASKTFR_L_INT_MASK(x)    ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup GDMA_MASKBLOCK_L
  * @brief Mask for IntBlock Interrupt
  * @{
  */
#define GDMA_MASK_MASKBLOCK_L_INT_MASK_WE   ((u32)0x000000FF << 8)           /*!< W 0x0  Interrupt Mask Write Enable(chn0-chn7) 0 = write disabled 1 = write enabled*/
#define GDMA_MASKBLOCK_L_INT_MASK_WE(x)     (((u32)((x) & 0x000000FF) << 8))
#define GDMA_GET_MASKBLOCK_L_INT_MASK_WE(x) ((u32)(((x >> 8) & 0x000000FF)))
#define GDMA_MASK_MASKBLOCK_L_INT_MASK      ((u32)0x000000FF << 0)           /*!< R/W 0x0  Interrupt Mask(chn0-chn7) 0 = masked 1 = unmasked*/
#define GDMA_MASKBLOCK_L_INT_MASK(x)        (((u32)((x) & 0x000000FF) << 0))
#define GDMA_GET_MASKBLOCK_L_INT_MASK(x)    ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup GDMA_MASKERR_L
  * @brief Mask for IntErr Interrupt
  * @{
  */
#define GDMA_MASK_MASKERR_L_INT_MASK_WE   ((u32)0x000000FF << 8)           /*!< W 0x0  Interrupt Mask Write Enable(chn0-chn7) 0 = write disabled 1 = write enabled*/
#define GDMA_MASKERR_L_INT_MASK_WE(x)     (((u32)((x) & 0x000000FF) << 8))
#define GDMA_GET_MASKERR_L_INT_MASK_WE(x) ((u32)(((x >> 8) & 0x000000FF)))
#define GDMA_MASK_MASKERR_L_INT_MASK      ((u32)0x000000FF << 0)           /*!< R/W 0x0  Interrupt Mask(chn0-chn7) 0 = masked 1 = unmasked*/
#define GDMA_MASKERR_L_INT_MASK(x)        (((u32)((x) & 0x000000FF) << 0))
#define GDMA_GET_MASKERR_L_INT_MASK(x)    ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup GDMA_MASKERR_H
  * @brief Mask for non-secure access secure register error if defined
  * @{
  */
#define GDMA_MASK_MASKERR_H_INT_MASK_WE   ((u32)0x000000FF << 8)           /*!< W 0x0  Interrupt Mask Write Enable(chn0-chn7) 0 = write disabled 1 = write enabled These bits only exist when DMAC_ SECURE_MODE has beed defined*/
#define GDMA_MASKERR_H_INT_MASK_WE(x)     (((u32)((x) & 0x000000FF) << 8))
#define GDMA_GET_MASKERR_H_INT_MASK_WE(x) ((u32)(((x >> 8) & 0x000000FF)))
#define GDMA_MASK_MASKERR_H_INT_MASK      ((u32)0x000000FF << 0)           /*!< R/W 0x0  Interrupt Mask(chn0-chn7) 0 = masked 1 = unmasked These bits only exist when DMAC_ SECURE_MODE has beed defined*/
#define GDMA_MASKERR_H_INT_MASK(x)        (((u32)((x) & 0x000000FF) << 0))
#define GDMA_GET_MASKERR_H_INT_MASK(x)    ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup GDMA_CLEARTFR_L
  * @brief Clear for IntTfr Interrupt
  * @{
  */
#define GDMA_MASK_CLEARTFR_L_CLEAR_TFR   ((u32)0x0000FFFF << 0)           /*!< W1C 0x0  Interrupt clear for chn0-chn15 0 = no effect 1= clear interrupt*/
#define GDMA_CLEARTFR_L_CLEAR_TFR(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define GDMA_GET_CLEARTFR_L_CLEAR_TFR(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup GDMA_CLEARBLOCK_L
  * @brief Clear for IntBlock Interrupt
  * @{
  */
#define GDMA_MASK_CLEARBLOCK_L_CLEAR_BLOCK   ((u32)0x0000FFFF << 0)           /*!< W1C 0x0  Interrupt clear for chn0-chn15*/
#define GDMA_CLEARBLOCK_L_CLEAR_BLOCK(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define GDMA_GET_CLEARBLOCK_L_CLEAR_BLOCK(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup GDMA_CLEARERR_L
  * @brief Clear for IntErr Interrupt
  * @{
  */
#define GDMA_MASK_CLEARERR_L_CLEAR_PROTOCOL_ERROR   ((u32)0x0000FFFF << 0)           /*!< W1C 0x0  Interrupt clear for chn0-chn15. (Protocol error)*/
#define GDMA_CLEARERR_L_CLEAR_PROTOCOL_ERROR(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define GDMA_GET_CLEARERR_L_CLEAR_PROTOCOL_ERROR(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup GDMA_CLEARERR_H
  * @brief Clear for non-secure access secure register error if defined
  * @{
  */
#define GDMA_MASK_CLEARERR_H_CLEAR_SEC_ERROR   ((u32)0x0000FFFF << 0)           /*!< W1C 0x0  Interrupt clear for chn0-chn15 (Non-secure access secure register error, Sec Error) when DMAC_SECURE_MODE has beed defined, else it is reserved;*/
#define GDMA_CLEARERR_H_CLEAR_SEC_ERROR(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define GDMA_GET_CLEARERR_H_CLEAR_SEC_ERROR(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup GDMA_STATUSINT_L
  * @brief Combined Interrupt Sataus Register
  * @{
  */
#define GDMA_BIT_STATUSINT_L_ERROR ((u32)0x00000001 << 4) /*!< R 0x0  OR of the contents of StatusErr register.*/
#define GDMA_BIT_STATUSINT_L_BLOCK ((u32)0x00000001 << 1) /*!< R 0x0  OR of the contents of StatusBlock register.*/
#define GDMA_BIT_STATUSINT_L_TFR   ((u32)0x00000001 << 0) /*!< R 0x0  OR of the contents of StatusTfr register.*/
/** @} */

/** @defgroup GDMA_DMACFGREG_L
  * @brief DMA low-32 bits Configuration Register
  * @{
  */
#define GDMA_BIT_DMACFGREG_L_DMA_EN ((u32)0x00000001 << 0) /*!< R/W 0x1  RTK_DMAC Enable bit. 0 = RTK_DMAC Disabled 1 = RTK_DMAC Enabled*/
/** @} */

/** @defgroup GDMA_CHENREG_L_1
  * @brief DMA low-32 bits Channel Enable Register if configed chn number is less than or equal to 8
  * @{
  */
#define GDMA_MASK_CHENREG_L_1_CH_EN_WE   ((u32)0x000000FF << 8)           /*!< W 0x0  Channel enable write enable.*/
#define GDMA_CHENREG_L_1_CH_EN_WE(x)     (((u32)((x) & 0x000000FF) << 8))
#define GDMA_GET_CHENREG_L_1_CH_EN_WE(x) ((u32)(((x >> 8) & 0x000000FF)))
#define GDMA_MASK_CHENREG_L_1_CH_EN      ((u32)0x000000FF << 0)           /*!< R/W 0x0  Enables/Disables the channel. Setting this bit enables a channel; clearing this bit disables the channel. 0 = Disable the Channel 1 = Enable the Channel The CHENREG_L.CH_EN bit is automatically cleared by hardware to disable the channel after the last AXI transfer of the DMA transfer to the destination has completed. Software can therefore poll this bit to determine when this channel is free for a new DMA transfer.*/
#define GDMA_CHENREG_L_1_CH_EN(x)        (((u32)((x) & 0x000000FF) << 0))
#define GDMA_GET_CHENREG_L_1_CH_EN(x)    ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup GDMA_DMAIDREG_L
  * @brief DMA low-32 bits ID Register
  * @{
  */
#define GDMA_MASK_DMAIDREG_L_DMA_ID   ((u32)0xFFFFFFFF << 0)           /*!< R 32'h20240513  Hardcoded RTK_DMAC Peripheral ID*/
#define GDMA_DMAIDREG_L_DMA_ID(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GDMA_GET_DMAIDREG_L_DMA_ID(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup GDMA_DMAOSNUM_L
  * @brief DMA low-32 bits Outstanding Number Register
  * @{
  */
#define GDMA_MASK_DMAOSNUM_L_OSW   ((u32)0x00000007 << 8)           /*!< R/W 0x4  If the OSW number of DMAC is programmable, this register is writable. And DMAC will not issue new write command until OSW of DMAC is less than this value. If the OSW number is not programmable, this value is read as "4". And DMAC's OSW is fixed to 4. If user defines OSW_CNT, b = log2(OSW_CNT +1) +7; otherwise, b = log2(4+1) +7. bits 15: b+1 do not exist and read back as 0. Note: If write 0 to this register, DMAC can not issue any new write commands.*/
#define GDMA_DMAOSNUM_L_OSW(x)     (((u32)((x) & 0x00000007) << 8))
#define GDMA_GET_DMAOSNUM_L_OSW(x) ((u32)(((x >> 8) & 0x00000007)))
#define GDMA_MASK_DMAOSNUM_L_OSR   ((u32)0x0000000F << 0)           /*!< R/W 0x8  If the OSR number of DMAC is programmable, this register is writable. And DMAC will not issue new read command until OSR of DMAC is less than this value. If the OSR number is not programmable, this value is read as "channel number". And DMAC does not limit the OSR number. If user defines OSR_CNT, b = log2(OSR_CNT +1); otherwise, b = log2(DMAH_NUM_CHANNELS +1) bits 7: b+1 do not exist and read back as 0. Note: If write 0 to this register, DMAC can not issue any new read commands.*/
#define GDMA_DMAOSNUM_L_OSR(x)     (((u32)((x) & 0x0000000F) << 0))
#define GDMA_GET_DMAOSNUM_L_OSR(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup GDMA_DMA_COMP_PARAMS_6_L
  * @brief Refer to the bit table in the description for it
  * @{
  */
#define GDMA_MASK_CH7_MAX_BLK_SIZE   ((u32)0x000000FF << 24)           /*!< R 0x15  The value of this register is derived from the DMAC_CH7_MAX_BLK_SIZE from config_form.vh CH7_MAX_BLK_SIZE = log2(DMAC_CH7_MAX_BLK_SIZE) + 1*/
#define GDMA_CH7_MAX_BLK_SIZE(x)     (((u32)((x) & 0x000000FF) << 24))
#define GDMA_GET_CH7_MAX_BLK_SIZE(x) ((u32)(((x >> 24) & 0x000000FF)))
#define GDMA_MASK_CH6_MAX_BLK_SIZE   ((u32)0x000000FF << 16)           /*!< R 0x15  The value of this register is derived from the DMAC_CH6_MAX_BLK_SIZE from config_form.vh CH6_MAX_BLK_SIZE = log2(DMAC_CH6_MAX_BLK_SIZE) + 1*/
#define GDMA_CH6_MAX_BLK_SIZE(x)     (((u32)((x) & 0x000000FF) << 16))
#define GDMA_GET_CH6_MAX_BLK_SIZE(x) ((u32)(((x >> 16) & 0x000000FF)))
#define GDMA_MASK_CH5_MAX_BLK_SIZE   ((u32)0x000000FF << 8)            /*!< R 0x15  The value of this register is derived from the DMAC_CH5_MAX_BLK_SIZE from config_form.vh  CH5_MAX_BLK_SIZE = log2(DMAC_CH5_MAX_BLK_SIZE) + 1*/
#define GDMA_CH5_MAX_BLK_SIZE(x)     (((u32)((x) & 0x000000FF) << 8))
#define GDMA_GET_CH5_MAX_BLK_SIZE(x) ((u32)(((x >> 8) & 0x000000FF)))
#define GDMA_MASK_CH4_MAX_BLK_SIZE   ((u32)0x000000FF << 0)            /*!< R 0x15  The value of this register is derived from the DMAC_CH4_MAX_BLK_SIZE from config_form.vh  CH4_MAX_BLK_SIZE = log2(DMAC_CH4_MAX_BLK_SIZE) + 1*/
#define GDMA_CH4_MAX_BLK_SIZE(x)     (((u32)((x) & 0x000000FF) << 0))
#define GDMA_GET_CH4_MAX_BLK_SIZE(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup GDMA_DMA_COMP_PARAMS_6_H
  * @brief Refer to the bit table in the description for it
  * @{
  */
#define GDMA_MASK_CH7_FIFO_DEPTH      ((u32)0x00000007 << 28)           /*!< R 0x2  The value of this register is derived from the DMAC_CH7_FIFO_DEPTH parameter. Default value is 0x0. CH7_FIFO_DEPTH = log2(DMAC_CH7_FIFO_DEPTH/8) 0x0 = 8 bytes 0x1 = 16 0x2 = 32 0x3 = 64 0x4 = 128 0x5 = Reserved 0x6 = Reserved 0x7 = Reserved*/
#define GDMA_CH7_FIFO_DEPTH(x)        (((u32)((x) & 0x00000007) << 28))
#define GDMA_GET_CH7_FIFO_DEPTH(x)    ((u32)(((x >> 28) & 0x00000007)))
#define GDMA_MASK_CH7_MAX_MULT_SIZE   ((u32)0x00000007 << 16)           /*!< R 0x3  The value of this register is derived from the DMAC_CH7_MULT_SIZE parameter. Maxium burst transaction size. CH7_MAX_MULT_SIZE = log2(256/DMAC_DATA_WIDTH) 0x3 = 32 bits 0x2 = 64 bits 0x1 = 128 bits*/
#define GDMA_CH7_MAX_MULT_SIZE(x)     (((u32)((x) & 0x00000007) << 16))
#define GDMA_GET_CH7_MAX_MULT_SIZE(x) ((u32)(((x >> 16) & 0x00000007)))
#define GDMA_MASK_CH7_FC              ((u32)0x00000003 << 14)           /*!< R 0x3  The value of this register is derived from the DMAC_CH7_FC parameter. 0x0 = DMA 0x1 = SRC 0x2 = DST 0x3 = ANY*/
#define GDMA_CH7_FC(x)                (((u32)((x) & 0x00000003) << 14))
#define GDMA_GET_CH7_FC(x)            ((u32)(((x >> 14) & 0x00000003)))
#define GDMA_BIT_CH7_HC_LLP           ((u32)0x00000001 << 13)           /*!< R 0x0  The value of this register is derived from the DMAC_CH7_HC_LLP parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH7_CTL_WB_EN        ((u32)0x00000001 << 12)           /*!< R 0x0  The value of this register is derived from the DMAC_CH7_CTL_WB_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH7_MULTI_BLK_EN     ((u32)0x00000001 << 11)           /*!< R 0x1  The value of this register is derived from the DMAC_CH7_CTL_WB_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH7_LOCK_EN          ((u32)0x00000001 << 10)           /*!< R 0x0  The value of this register is derived from the DMAC_CH7_LOCK_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH7_SRC_GAT_EN       ((u32)0x00000001 << 9)            /*!< R 0x0  The value of this register is derived from the DMAC_CH7_SRC_GAT_EN parameter. For a description of this parameter, refer to page 112. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH7_DST_SCA_EN       ((u32)0x00000001 << 8)            /*!< R 0x0  The value of this register is derived from the DMAC_CH7_DST_SCA_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH7_STAT_SRC         ((u32)0x00000001 << 7)            /*!< R 0x0  The value of this register is derived from the DMAC_CH7_STAT_SRC parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH7_STAT_DST         ((u32)0x00000001 << 6)            /*!< R 0x0  The value of this register is derived from the DMAC_CH7_STAT_DST parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_MASK_CH7_STW             ((u32)0x00000007 << 3)            /*!< R 0x3  The value of this register is derived from the DMAC_CH7_STW parameter. 0x0 = NO_HARDCODE 0x1 = 8 0x2 = 16 0x3 = 32 0x4 = 64 0x5 = 128 0x6 = 256 0x7 = reserved*/
#define GDMA_CH7_STW(x)               (((u32)((x) & 0x00000007) << 3))
#define GDMA_GET_CH7_STW(x)           ((u32)(((x >> 3) & 0x00000007)))
#define GDMA_MASK_CH7_DTW             ((u32)0x00000007 << 0)            /*!< R 0x3  The value of this register is derived from the DMAC_CH7_DTW parameter. 0x0 = NO_HARDCODE 0x1 = 8 0x2 = 16 0x3 = 32 0x4 = 64 0x5 = 128 0x6 = 256 0x7 = reserved*/
#define GDMA_CH7_DTW(x)               (((u32)((x) & 0x00000007) << 0))
#define GDMA_GET_CH7_DTW(x)           ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup GDMA_DMA_COMP_PARAMS_5_L
  * @brief Refer to the bit table in the description for it
  * @{
  */
#define GDMA_MASK_CH6_FIFO_DEPTH      ((u32)0x00000007 << 28)           /*!< R 0x2  The value of this register is derived from the DMAC_CH6_FIFO_DEPTH parameter. Default value is 0x0. 0x0 = 8 bytes 0x1 = 16 0x2 = 32 0x3 = 64 0x4 = 128 0x5 = Reserved 0x6 = Reserved 0x7 = Reserved*/
#define GDMA_CH6_FIFO_DEPTH(x)        (((u32)((x) & 0x00000007) << 28))
#define GDMA_GET_CH6_FIFO_DEPTH(x)    ((u32)(((x >> 28) & 0x00000007)))
#define GDMA_MASK_CH6_MAX_MULT_SIZE   ((u32)0x00000007 << 16)           /*!< R 0x3  The value of this register is derived from the DMAC_CH6_MULT_SIZE parameter. Maxium burst transaction size. CH6_MAX_MULT_SIZE = log2(256/DMAC_DATA_WIDTH) 0x3 = 32 bits 0x2 = 64 bits 0x1 = 128 bits*/
#define GDMA_CH6_MAX_MULT_SIZE(x)     (((u32)((x) & 0x00000007) << 16))
#define GDMA_GET_CH6_MAX_MULT_SIZE(x) ((u32)(((x >> 16) & 0x00000007)))
#define GDMA_MASK_CH6_FC              ((u32)0x00000003 << 14)           /*!< R 0x3  The value of this register is derived from the DMAC_CH6_FC parameter. 0x0 = DMA 0x1 = SRC 0x2 = DST 0x3 = ANY*/
#define GDMA_CH6_FC(x)                (((u32)((x) & 0x00000003) << 14))
#define GDMA_GET_CH6_FC(x)            ((u32)(((x >> 14) & 0x00000003)))
#define GDMA_BIT_CH6_HC_LLP           ((u32)0x00000001 << 13)           /*!< R 0x0  The value of this register is derived from the DMAC_CH6_HC_LLP parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH6_CTL_WB_EN        ((u32)0x00000001 << 12)           /*!< R 0x0  The value of this register is derived from the DMAC_CH6_CTL_WB_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH6_MULTI_BLK_EN     ((u32)0x00000001 << 11)           /*!< R 0x1  The value of this register is derived from the DMAC_CH6_MULTI_BLK_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH6_LOCK_EN          ((u32)0x00000001 << 10)           /*!< R 0x0  The value of this register is derived from the DMAC_CH6_LOCK_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH6_SRC_GAT_EN       ((u32)0x00000001 << 9)            /*!< R 0x0  The value of this register is derived from the DMAC_CH6_SRC_GAT_EN parameter.  0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH6_DST_SCA_EN       ((u32)0x00000001 << 8)            /*!< R 0x0  The value of this register is derived from the DMAC_CH6_DST_SCA_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH6_STAT_SRC         ((u32)0x00000001 << 7)            /*!< R 0x0  The value of this register is derived from the DMAC_CH6_STAT_SRC parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH6_STAT_DST         ((u32)0x00000001 << 6)            /*!< R 0x0  The value of this register is derived from the DMAC_CH6_STAT_DST parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_MASK_CH6_STW             ((u32)0x00000007 << 3)            /*!< R 0x3  The value of this register is derived from the DMAC_CH6_STW parameter. 0x0 = NO_HARDCODE 0x1 = 8 0x2 = 16 0x3 = 32 0x4 = 64 0x5 = 128 0x6 = 256 0x7 = reserved*/
#define GDMA_CH6_STW(x)               (((u32)((x) & 0x00000007) << 3))
#define GDMA_GET_CH6_STW(x)           ((u32)(((x >> 3) & 0x00000007)))
#define GDMA_MASK_CH6_DTW             ((u32)0x00000007 << 0)            /*!< R 0x3  The value of this register is derived from the DMAC_CH6_DTW parameter. 0x0 = NO_HARDCODE 0x1 = 8 0x2 = 16 0x3 = 32 0x4 = 64 0x5 = 128 0x6 = 256 0x7 = reserved*/
#define GDMA_CH6_DTW(x)               (((u32)((x) & 0x00000007) << 0))
#define GDMA_GET_CH6_DTW(x)           ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup GDMA_DMA_COMP_PARAMS_5_H
  * @brief Refer to the bit table in the description for it
  * @{
  */
#define GDMA_MASK_CH5_FIFO_DEPTH      ((u32)0x00000007 << 28)           /*!< R 0x2  The value of this register is derived from the DMAC_CH5_FIFO_DEPTH parameter. Default value is 0x0. 0x0 = 8 bytes 0x1 = 16 0x2 = 32 0x3 = 64 0x4 = 128 0x5 = Reserved 0x6 = Reserved 0x7 = Reserved*/
#define GDMA_CH5_FIFO_DEPTH(x)        (((u32)((x) & 0x00000007) << 28))
#define GDMA_GET_CH5_FIFO_DEPTH(x)    ((u32)(((x >> 28) & 0x00000007)))
#define GDMA_MASK_CH5_MAX_MULT_SIZE   ((u32)0x00000007 << 16)           /*!< R 0x3  The value of this register is derived from the DMAC_CH5_MULT_SIZE parameter. Maxium burst transaction size. CH5_MAX_MULT_SIZE = log2(256/DMAC_DATA_WIDTH) 0x3 = 32 bits 0x2 = 64 bits 0x1 = 128 bits*/
#define GDMA_CH5_MAX_MULT_SIZE(x)     (((u32)((x) & 0x00000007) << 16))
#define GDMA_GET_CH5_MAX_MULT_SIZE(x) ((u32)(((x >> 16) & 0x00000007)))
#define GDMA_MASK_CH5_FC              ((u32)0x00000003 << 14)           /*!< R 0x3  The value of this register is derived from the DMAC_CH5_FC parameter. 0x0 = DMA 0x1 = SRC 0x2 = DST 0x3 = ANY*/
#define GDMA_CH5_FC(x)                (((u32)((x) & 0x00000003) << 14))
#define GDMA_GET_CH5_FC(x)            ((u32)(((x >> 14) & 0x00000003)))
#define GDMA_BIT_CH5_HC_LLP           ((u32)0x00000001 << 13)           /*!< R 0x0  The value of this register is derived from the DMAC_CH5_HC_LLP parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH5_CTL_WB_EN        ((u32)0x00000001 << 12)           /*!< R 0x0  The value of this register is derived from the DMAC_CH5_CTL_WB_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH5_MULTI_BLK_EN     ((u32)0x00000001 << 11)           /*!< R 0x1  The value of this register is derived from the DMAC_CH5_MULTI_BLK_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH5_LOCK_EN          ((u32)0x00000001 << 10)           /*!< R 0x0  The value of this register is derived from the DMAC_CH5_LOCK_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH5_SRC_GAT_EN       ((u32)0x00000001 << 9)            /*!< R 0x0  The value of this register is derived from the DMAC_CH5_SRC_GAT_EN parameter. For a description of this parameter, refer to page 112. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH5_DST_SCA_EN       ((u32)0x00000001 << 8)            /*!< R 0x0  The value of this register is derived from the DMAC_CH5_DST_SCA_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH5_STAT_SRC         ((u32)0x00000001 << 7)            /*!< R 0x0  The value of this register is derived from the DMAC_CH5_STAT_SRC parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH5_STAT_DST         ((u32)0x00000001 << 6)            /*!< R 0x0  The value of this register is derived from the DMAC_CH5_STAT_DST parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_MASK_CH5_STW             ((u32)0x00000007 << 3)            /*!< R 0x3  The value of this register is derived from the DMAC_CH5_STW parameter. 0x0 = NO_HARDCODE 0x1 = 8 0x2 = 16 0x3 = 32 0x4 = 64 0x5 = 128 0x6 = 256 0x7 = reserved*/
#define GDMA_CH5_STW(x)               (((u32)((x) & 0x00000007) << 3))
#define GDMA_GET_CH5_STW(x)           ((u32)(((x >> 3) & 0x00000007)))
#define GDMA_MASK_CH5_DTW             ((u32)0x00000007 << 0)            /*!< R 0x3  The value of this register is derived from the DMAC_CH5_DTW parameter. 0x0 = NO_HARDCODE 0x1 = 8 0x2 = 16 0x3 = 32 0x4 = 64 0x5 = 128 0x6 = 256 0x7 = reserved*/
#define GDMA_CH5_DTW(x)               (((u32)((x) & 0x00000007) << 0))
#define GDMA_GET_CH5_DTW(x)           ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup GDMA_DMA_COMP_PARAMS_4_L
  * @brief Refer to the bit table in the description for it
  * @{
  */
#define GDMA_MASK_CH4_FIFO_DEPTH      ((u32)0x00000007 << 28)           /*!< R 0x2  The value of this register is derived from the DMAC_CH4_FIFO_DEPTH parameter. Default value is 0x0. 0x0 = 8 bytes 0x1 = 16 0x2 = 32 0x3 = 64 0x4 = 128 0x5 = Reserved 0x6 = Reserved 0x7 = Reserved*/
#define GDMA_CH4_FIFO_DEPTH(x)        (((u32)((x) & 0x00000007) << 28))
#define GDMA_GET_CH4_FIFO_DEPTH(x)    ((u32)(((x >> 28) & 0x00000007)))
#define GDMA_MASK_CH4_MAX_MULT_SIZE   ((u32)0x00000007 << 16)           /*!< R 0x3  The value of this register is derived from the DMAC_CH4_MULT_SIZE parameter. Maxium burst transaction size. CH4_MAX_MULT_SIZE = log2(256/DMAC_DATA_WIDTH) 0x3 = 32 bits 0x2 = 64 bits 0x1 = 128 bits*/
#define GDMA_CH4_MAX_MULT_SIZE(x)     (((u32)((x) & 0x00000007) << 16))
#define GDMA_GET_CH4_MAX_MULT_SIZE(x) ((u32)(((x >> 16) & 0x00000007)))
#define GDMA_MASK_CH4_FC              ((u32)0x00000003 << 14)           /*!< R 0x3  The value of this register is derived from the DMAC_CH4_FC parameter. 0x0 = DMA 0x1 = SRC 0x2 = DST 0x3 = ANY*/
#define GDMA_CH4_FC(x)                (((u32)((x) & 0x00000003) << 14))
#define GDMA_GET_CH4_FC(x)            ((u32)(((x >> 14) & 0x00000003)))
#define GDMA_BIT_CH4_HC_LLP           ((u32)0x00000001 << 13)           /*!< R 0x0  The value of this register is derived from the DMAC_CH4_HC_LLP parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH4_CTL_WB_EN        ((u32)0x00000001 << 12)           /*!< R 0x0  The value of this register is derived from the DMAC_CH4_CTL_WB_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH4_MULTI_BLK_EN     ((u32)0x00000001 << 11)           /*!< R 0x1  The value of this register is derived from the DMAC_CH4_MULTI_BLK_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH4_LOCK_EN          ((u32)0x00000001 << 10)           /*!< R 0x0  The value of this register is derived from the DMAC_CH4_LOCK_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH4_SRC_GAT_EN       ((u32)0x00000001 << 9)            /*!< R 0x0  The value of this register is derived from the DMAC_CH4_SRC_GAT_EN parameter. For a description of this parameter, refer to page 112.0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH4_DST_SCA_EN       ((u32)0x00000001 << 8)            /*!< R 0x0  The value of this register is derived from the DMAC_CH4_DST_SCA_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH4_STAT_SRC         ((u32)0x00000001 << 7)            /*!< R 0x0  The value of this register is derived from the DMAC_CH4_STAT_SRC parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH4_STAT_DST         ((u32)0x00000001 << 6)            /*!< R 0x0  The value of this register is derived from the DMAC_CH4_STAT_DST parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_MASK_CH4_STW             ((u32)0x00000007 << 3)            /*!< R 0x3  The value of this register is derived from the DMAC_CH4_STW parameter. 0x0 = NO_HARDCODE 0x1 = 8 0x2 = 16 0x3 = 32 0x4 = 64 0x5 = 128 0x6 = 256 0x7 = reserved*/
#define GDMA_CH4_STW(x)               (((u32)((x) & 0x00000007) << 3))
#define GDMA_GET_CH4_STW(x)           ((u32)(((x >> 3) & 0x00000007)))
#define GDMA_MASK_CH4_DTW             ((u32)0x00000007 << 0)            /*!< R 0x3  The value of this register is derived from the DMAC_CH4_DTW parameter. 0x0 = NO_HARDCODE 0x1 = 8 0x2 = 16 0x3 = 32 0x4 = 64 0x5 = 128 0x6 = 256 0x7 = reserved*/
#define GDMA_CH4_DTW(x)               (((u32)((x) & 0x00000007) << 0))
#define GDMA_GET_CH4_DTW(x)           ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup GDMA_DMA_COMP_PARAMS_4_H
  * @brief Refer to the bit table in the description for it
  * @{
  */
#define GDMA_MASK_CH3_FIFO_DEPTH      ((u32)0x00000007 << 28)           /*!< R 0x2  The value of this register is derived from the DMAC_CH3_FIFO_DEPTH parameter. Default value is 0x0. 0x0 = 8 bytes 0x1 = 16 0x2 = 32 0x3 = 64 0x4 = 128 0x5 = Reserved 0x6 = Reserved 0x7 = Reserved*/
#define GDMA_CH3_FIFO_DEPTH(x)        (((u32)((x) & 0x00000007) << 28))
#define GDMA_GET_CH3_FIFO_DEPTH(x)    ((u32)(((x >> 28) & 0x00000007)))
#define GDMA_MASK_CH3_MAX_MULT_SIZE   ((u32)0x00000007 << 16)           /*!< R 0x3  The value of this register is derived from the DMAC_CH3_MULT_SIZE parameter. Maxium burst transaction size. CH3_MAX_MULT_SIZE = log2(256/DMAC_DATA_WIDTH) 0x3 = 32 bits 0x2 = 64 bits 0x1 = 128 bits*/
#define GDMA_CH3_MAX_MULT_SIZE(x)     (((u32)((x) & 0x00000007) << 16))
#define GDMA_GET_CH3_MAX_MULT_SIZE(x) ((u32)(((x >> 16) & 0x00000007)))
#define GDMA_MASK_CH3_FC              ((u32)0x00000003 << 14)           /*!< R 0x3  The value of this register is derived from the DMAC_CH3_FC parameter. 0x0 = DMA 0x1 = SRC 0x2 = DST 0x3 = ANY*/
#define GDMA_CH3_FC(x)                (((u32)((x) & 0x00000003) << 14))
#define GDMA_GET_CH3_FC(x)            ((u32)(((x >> 14) & 0x00000003)))
#define GDMA_BIT_CH3_HC_LLP           ((u32)0x00000001 << 13)           /*!< R 0x0  The value of this register is derived from the DMAC_CH3_HC_LLP parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH3_CTL_WB_EN        ((u32)0x00000001 << 12)           /*!< R 0x0  The value of this register is derived from the DMAC_CH3_CTL_WB_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH3_MULTI_BLK_EN     ((u32)0x00000001 << 11)           /*!< R 0x1  The value of this register is derived from the DMAC_CH3_MULTI_BLK_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH3_LOCK_EN          ((u32)0x00000001 << 10)           /*!< R 0x0  The value of this register is derived from the DMAC_CH3_LOCK_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH3_SRC_GAT_EN       ((u32)0x00000001 << 9)            /*!< R 0x1  The value of this register is derived from the DMAC_CH3_SRC_GAT_EN parameter.  0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH3_DST_SCA_EN       ((u32)0x00000001 << 8)            /*!< R 0x1  The value of this register is derived from the DMAC_CH3_DST_SCA_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH3_STAT_SRC         ((u32)0x00000001 << 7)            /*!< R 0x0  The value of this register is derived from the DMAC_CH3_STAT_SRC parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH3_STAT_DST         ((u32)0x00000001 << 6)            /*!< R 0x0  The value of this register is derived from the DMAC_CH3_STAT_DST parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_MASK_CH3_STW             ((u32)0x00000007 << 3)            /*!< R 0x3  The value of this register is derived from the DMAC_CH3_STW parameter. 0x0 = NO_HARDCODE 0x1 = 8 0x2 = 16 0x3 = 32 0x4 = 64 0x5 = 128 0x6 = 256 0x7 = reserved*/
#define GDMA_CH3_STW(x)               (((u32)((x) & 0x00000007) << 3))
#define GDMA_GET_CH3_STW(x)           ((u32)(((x >> 3) & 0x00000007)))
#define GDMA_MASK_CH3_DTW             ((u32)0x00000007 << 0)            /*!< R 0x3  The value of this register is derived from the DMAC_CH3_DTW parameter. 0x0 = NO_HARDCODE 0x1 = 8 0x2 = 16 0x3 = 32 0x4 = 64 0x5 = 128 0x6 = 256 0x7 = reserved*/
#define GDMA_CH3_DTW(x)               (((u32)((x) & 0x00000007) << 0))
#define GDMA_GET_CH3_DTW(x)           ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup GDMA_DMA_COMP_PARAMS_3_L
  * @brief Refer to the bit table in the description for it
  * @{
  */
#define GDMA_MASK_CH2_FIFO_DEPTH      ((u32)0x00000007 << 28)           /*!< R 0x2  The value of this register is derived from the DMAC_CH2_FIFO_DEPTH parameter. Default value is 0x0. 0x0 = 8 bytes 0x1 = 16 0x2 = 32 0x3 = 64 0x4 = 128 0x5 = Reserved 0x6 = Reserved 0x7 = Reserved*/
#define GDMA_CH2_FIFO_DEPTH(x)        (((u32)((x) & 0x00000007) << 28))
#define GDMA_GET_CH2_FIFO_DEPTH(x)    ((u32)(((x >> 28) & 0x00000007)))
#define GDMA_MASK_CH2_MAX_MULT_SIZE   ((u32)0x00000007 << 16)           /*!< R 0x3  The value of this register is derived from the DMAC_CH2_MULT_SIZE parameter. Maxium burst transaction size. CH2_MAX_MULT_SIZE = log2(256/DMAC_DATA_WIDTH) 0x3 = 32 bits 0x2 = 64 bits 0x1 = 128 bits*/
#define GDMA_CH2_MAX_MULT_SIZE(x)     (((u32)((x) & 0x00000007) << 16))
#define GDMA_GET_CH2_MAX_MULT_SIZE(x) ((u32)(((x >> 16) & 0x00000007)))
#define GDMA_MASK_CH2_FC              ((u32)0x00000003 << 14)           /*!< R 0x3  The value of this register is derived from the DMAC_CH2_FC parameter. 0x0 = DMA 0x1 = SRC 0x2 = DST 0x3 = ANY*/
#define GDMA_CH2_FC(x)                (((u32)((x) & 0x00000003) << 14))
#define GDMA_GET_CH2_FC(x)            ((u32)(((x >> 14) & 0x00000003)))
#define GDMA_BIT_CH2_HC_LLP           ((u32)0x00000001 << 13)           /*!< R 0x0  The value of this register is derived from the DMAC_CH2_HC_LLP parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH2_CTL_WB_EN        ((u32)0x00000001 << 12)           /*!< R 0x0  The value of this register is derived from the DMAC_CH2_CTL_WB_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH2_MULTI_BLK_EN     ((u32)0x00000001 << 11)           /*!< R 0x1  The value of this register is derived from the DMAC_CH2_MULTI_BLK_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH2_LOCK_EN          ((u32)0x00000001 << 10)           /*!< R 0x0  The value of this register is derived from the DMAC_CH2_LOCK_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH2_SRC_GAT_EN       ((u32)0x00000001 << 9)            /*!< R 0x1  The value of this register is derived from the DMAC_CH2_SRC_GAT_EN parameter. For a description of this parameter, refer to page 112. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH2_DST_SCA_EN       ((u32)0x00000001 << 8)            /*!< R 0x1  The value of this register is derived from the DMAC_CH2_DST_SCA_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH2_STAT_SRC         ((u32)0x00000001 << 7)            /*!< R 0x0  The value of this register is derived from the DMAC_CH2_STAT_SRC parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH2_STAT_DST         ((u32)0x00000001 << 6)            /*!< R 0x0  The value of this register is derived from the DMAC_CH2_STAT_DST parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_MASK_CH2_STW             ((u32)0x00000007 << 3)            /*!< R 0x3  The value of this register is derived from the DMAC_CH2_STW parameter. 0x0 = NO_HARDCODE 0x1 = 8 0x2 = 16 0x3 = 32 0x4 = 64 0x5 = 128 0x6 = 256 0x7 = reserved*/
#define GDMA_CH2_STW(x)               (((u32)((x) & 0x00000007) << 3))
#define GDMA_GET_CH2_STW(x)           ((u32)(((x >> 3) & 0x00000007)))
#define GDMA_MASK_CH2_DTW             ((u32)0x00000007 << 0)            /*!< R 0x3  The value of this register is derived from the DMAC_CH2_DTW parameter. 0x0 = NO_HARDCODE 0x1 = 8 0x2 = 16 0x3 = 32 0x4 = 64 0x5 = 128 0x6 = 256 0x7 = reserved*/
#define GDMA_CH2_DTW(x)               (((u32)((x) & 0x00000007) << 0))
#define GDMA_GET_CH2_DTW(x)           ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup GDMA_DMA_COMP_PARAMS_3_H
  * @brief Refer to the bit table in the description for it
  * @{
  */
#define GDMA_MASK_CH1_FIFO_DEPTH      ((u32)0x00000007 << 28)           /*!< R 0x4  The value of this register is derived from the DMAC_CH1_FIFO_DEPTH parameter. Default value is 0x0. 0x0 = 8 bytes 0x1 = 16 0x2 = 32 0x3 = 64 0x4 = 128 0x5 = Reserved 0x6 = Reserved 0x7 = Reserved*/
#define GDMA_CH1_FIFO_DEPTH(x)        (((u32)((x) & 0x00000007) << 28))
#define GDMA_GET_CH1_FIFO_DEPTH(x)    ((u32)(((x >> 28) & 0x00000007)))
#define GDMA_MASK_CH1_MAX_MULT_SIZE   ((u32)0x00000007 << 16)           /*!< R 0x3  The value of this register is derived from the DMAC_CH1_MULT_SIZE parameter. Maxium burst transaction size. CH1_MAX_MULT_SIZE = log2(256/DMAC_DATA_WIDTH) 0x3 = 32 bits 0x2 = 64 bits 0x1 = 128 bits*/
#define GDMA_CH1_MAX_MULT_SIZE(x)     (((u32)((x) & 0x00000007) << 16))
#define GDMA_GET_CH1_MAX_MULT_SIZE(x) ((u32)(((x >> 16) & 0x00000007)))
#define GDMA_MASK_CH1_FC              ((u32)0x00000003 << 14)           /*!< R 0x3  The value of this register is derived from the DMAC_CH1_FC parameter. 0x0 = DMA 0x1 = SRC 0x2 = DST 0x3 = ANY*/
#define GDMA_CH1_FC(x)                (((u32)((x) & 0x00000003) << 14))
#define GDMA_GET_CH1_FC(x)            ((u32)(((x >> 14) & 0x00000003)))
#define GDMA_BIT_CH1_HC_LLP           ((u32)0x00000001 << 13)           /*!< R 0x0  The value of this register is derived from the DMAC_CH1_HC_LLP parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH1_CTL_WB_EN        ((u32)0x00000001 << 12)           /*!< R 0x0  The value of this register is derived from the DMAC_CH1_CTL_WB_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH1_MULTI_BLK_EN     ((u32)0x00000001 << 11)           /*!< R 0x1  The value of this register is derived from the DMAC_CH1_MULTI_BLK_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH1_LOCK_EN          ((u32)0x00000001 << 10)           /*!< R 0x0  The value of this register is derived from the DMAC_CH1_LOCK_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH1_SRC_GAT_EN       ((u32)0x00000001 << 9)            /*!< R 0x1  The value of this register is derived from the DMAC_CH1_SRC_GAT_EN parameter.  0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH1_DST_SCA_EN       ((u32)0x00000001 << 8)            /*!< R 0x1  The value of this register is derived from the DMAC_CH1_DST_SCA_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH1_STAT_SRC         ((u32)0x00000001 << 7)            /*!< R 0x0  The value of this register is derived from the DMAC_CH1_STAT_SRC parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH1_STAT_DST         ((u32)0x00000001 << 6)            /*!< R 0x0  The value of this register is derived from the DMAC_CH1_STAT_DST parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_MASK_CH1_STW             ((u32)0x00000007 << 3)            /*!< R 0x3  The value of this register is derived from the DMAC_CH1_STW parameter. 0x0 = NO_HARDCODE 0x1 = 8 0x2 = 16 0x3 = 32 0x4 = 64 0x5 = 128 0x6 = 256 0x7 = reserved*/
#define GDMA_CH1_STW(x)               (((u32)((x) & 0x00000007) << 3))
#define GDMA_GET_CH1_STW(x)           ((u32)(((x >> 3) & 0x00000007)))
#define GDMA_MASK_CH1_DTW             ((u32)0x00000007 << 0)            /*!< R 0x3  The value of this register is derived from the DMAC_CH1_DTW parameter. 0x0 = NO_HARDCODE 0x1 = 8 0x2 = 16 0x3 = 32 0x4 = 64 0x5 = 128 0x6 = 256 0x7 = reserved*/
#define GDMA_CH1_DTW(x)               (((u32)((x) & 0x00000007) << 0))
#define GDMA_GET_CH1_DTW(x)           ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup GDMA_DMA_COMP_PARAMS_2_L
  * @brief Refer to the bit table in the description for it
  * @{
  */
#define GDMA_MASK_CH0_FIFO_DEPTH      ((u32)0x00000007 << 28)           /*!< R 0x4  The value of this register is derived from the DMAC_CH0_FIFO_DEPTH parameter. Default value is 0x0. 0x0 = 8 bytes 0x1 = 16 0x2 = 32 0x3 = 64 0x4 = 128 0x5 = Reserved 0x6 = Reserved 0x7 = Reserved*/
#define GDMA_CH0_FIFO_DEPTH(x)        (((u32)((x) & 0x00000007) << 28))
#define GDMA_GET_CH0_FIFO_DEPTH(x)    ((u32)(((x >> 28) & 0x00000007)))
#define GDMA_MASK_CH0_MAX_MULT_SIZE   ((u32)0x00000007 << 16)           /*!< R 0x3  The value of this register is derived from the DMAC_CH0_MULT_SIZE parameter. Maxium burst transaction size. CH0_MAX_MULT_SIZE = log2(256/DMAC_DATA_WIDTH) 0x3 = 32 bits 0x2 = 64 bits 0x1 = 128 bits*/
#define GDMA_CH0_MAX_MULT_SIZE(x)     (((u32)((x) & 0x00000007) << 16))
#define GDMA_GET_CH0_MAX_MULT_SIZE(x) ((u32)(((x >> 16) & 0x00000007)))
#define GDMA_MASK_CH0_FC              ((u32)0x00000003 << 14)           /*!< R 0x3  The value of this register is derived from the DMAC_CH0_FC parameter. 0x0 = DMA 0x1 = SRC 0x2 = DST 0x3 = ANY*/
#define GDMA_CH0_FC(x)                (((u32)((x) & 0x00000003) << 14))
#define GDMA_GET_CH0_FC(x)            ((u32)(((x >> 14) & 0x00000003)))
#define GDMA_BIT_CH0_HC_LLP           ((u32)0x00000001 << 13)           /*!< R 0x0  The value of this register is derived from the DMAC_CH0_HC_LLP parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH0_CTL_WB_EN        ((u32)0x00000001 << 12)           /*!< R 0x0  The value of this register is derived from the DMAC_CH0_CTL_WB_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH0_MULTI_BLK_EN     ((u32)0x00000001 << 11)           /*!< R 0x1  The value of this register is derived from the DMAC_CH0_MULTI_BLK_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH0_LOCK_EN          ((u32)0x00000001 << 10)           /*!< R 0x0  The value of this register is derived from the DMAC_CH0_LOCK_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH0_SRC_GAT_EN       ((u32)0x00000001 << 9)            /*!< R 0x1  The value of this register is derived from the DMAC_CH0_SRC_GAT_EN parameter. For a description of this parameter, refer to page 112. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH0_DST_SCA_EN       ((u32)0x00000001 << 8)            /*!< R 0x1  The value of this register is derived from the DMAC_CH0_DST_SCA_EN parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH0_STAT_SRC         ((u32)0x00000001 << 7)            /*!< R 0x0  The value of this register is derived from the DMAC_CH0_STAT_SRC parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_BIT_CH0_STAT_DST         ((u32)0x00000001 << 6)            /*!< R 0x0  The value of this register is derived from the DMAC_CH0_STAT_DST parameter. 0x0 = FALSE 0x1 = TRUE*/
#define GDMA_MASK_CH0_STW             ((u32)0x00000007 << 3)            /*!< R 0x3  The value of this register is derived from the DMAC_CH0_STW parameter. 0x0 = NO_HARDCODE 0x1 = 8 0x2 = 16 0x3 = 32 0x4 = 64 0x5 = 128 0x6 = 256 0x7 = reserved*/
#define GDMA_CH0_STW(x)               (((u32)((x) & 0x00000007) << 3))
#define GDMA_GET_CH0_STW(x)           ((u32)(((x >> 3) & 0x00000007)))
#define GDMA_MASK_CH0_DTW             ((u32)0x00000007 << 0)            /*!< R 0x3  The value of this register is derived from the DMAC_CH0_DTW parameter. 0x0 = NO_HARDCODE 0x1 = 8 0x2 = 16 0x3 = 32 0x4 = 64 0x5 = 128 0x6 = 256 0x7 = reserved*/
#define GDMA_CH0_DTW(x)               (((u32)((x) & 0x00000007) << 0))
#define GDMA_GET_CH0_DTW(x)           ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup GDMA_DMA_COMP_PARAMS_2_H
  * @brief Refer to the bit table in the description for it
  * @{
  */
#define GDMA_MASK_CH7_MULTI_BLK_TYPE   ((u32)0x0000000F << 28)           /*!< R 0x0  The values of these bit fields are derived from the DMAC_CHx_MULTI_BLK_TYPE parameter. 0x0 = NO_HARDCODE 0x1 = CONT_RELOAD 0x2 = RELOAD_CONT 0x3 = RELOAD_RELOAD 0x4 = CONT_LLP 0x5 = RELOAD_LLP 0x6 = LLP_CONT 0x7 = LLP_RELOAD 0x8 = LLP_LLP*/
#define GDMA_CH7_MULTI_BLK_TYPE(x)     (((u32)((x) & 0x0000000F) << 28))
#define GDMA_GET_CH7_MULTI_BLK_TYPE(x) ((u32)(((x >> 28) & 0x0000000F)))
#define GDMA_MASK_CH6_MULTI_BLK_TYPE   ((u32)0x0000000F << 24)           /*!< R 0x0  The values of these bit fields are derived from the DMAC_CHx_MULTI_BLK_TYPE parameter. 0x0 = NO_HARDCODE 0x1 = CONT_RELOAD 0x2 = RELOAD_CONT 0x3 = RELOAD_RELOAD 0x4 = CONT_LLP 0x5 = RELOAD_LLP 0x6 = LLP_CONT 0x7 = LLP_RELOAD 0x8 = LLP_LLP*/
#define GDMA_CH6_MULTI_BLK_TYPE(x)     (((u32)((x) & 0x0000000F) << 24))
#define GDMA_GET_CH6_MULTI_BLK_TYPE(x) ((u32)(((x >> 24) & 0x0000000F)))
#define GDMA_MASK_CH5_MULTI_BLK_TYPE   ((u32)0x0000000F << 20)           /*!< R 0x0  The values of these bit fields are derived from the DMAC_CHx_MULTI_BLK_TYPE parameter. 0x0 = NO_HARDCODE 0x1 = CONT_RELOAD 0x2 = RELOAD_CONT 0x3 = RELOAD_RELOAD 0x4 = CONT_LLP 0x5 = RELOAD_LLP 0x6 = LLP_CONT 0x7 = LLP_RELOAD 0x8 = LLP_LLP*/
#define GDMA_CH5_MULTI_BLK_TYPE(x)     (((u32)((x) & 0x0000000F) << 20))
#define GDMA_GET_CH5_MULTI_BLK_TYPE(x) ((u32)(((x >> 20) & 0x0000000F)))
#define GDMA_MASK_CH4_MULTI_BLK_TYPE   ((u32)0x0000000F << 16)           /*!< R 0x0  The values of these bit fields are derived from the DMAC_CHx_MULTI_BLK_TYPE parameter. 0x0 = NO_HARDCODE 0x1 = CONT_RELOAD 0x2 = RELOAD_CONT 0x3 = RELOAD_RELOAD 0x4 = CONT_LLP 0x5 = RELOAD_LLP 0x6 = LLP_CONT 0x7 = LLP_RELOAD 0x8 = LLP_LLP*/
#define GDMA_CH4_MULTI_BLK_TYPE(x)     (((u32)((x) & 0x0000000F) << 16))
#define GDMA_GET_CH4_MULTI_BLK_TYPE(x) ((u32)(((x >> 16) & 0x0000000F)))
#define GDMA_MASK_CH3_MULTI_BLK_TYPE   ((u32)0x0000000F << 12)           /*!< R 0x0  The values of these bit fields are derived from the DMAC_CHx_MULTI_BLK_TYPE parameter. 0x0 = NO_HARDCODE 0x1 = CONT_RELOAD 0x2 = RELOAD_CONT 0x3 = RELOAD_RELOAD 0x4 = CONT_LLP 0x5 = RELOAD_LLP 0x6 = LLP_CONT 0x7 = LLP_RELOAD 0x8 = LLP_LLP*/
#define GDMA_CH3_MULTI_BLK_TYPE(x)     (((u32)((x) & 0x0000000F) << 12))
#define GDMA_GET_CH3_MULTI_BLK_TYPE(x) ((u32)(((x >> 12) & 0x0000000F)))
#define GDMA_MASK_CH2_MULTI_BLK_TYPE   ((u32)0x0000000F << 8)            /*!< R 0x0  The values of these bit fields are derived from the DMAC_CHx_MULTI_BLK_TYPE parameter. 0x0 = NO_HARDCODE 0x1 = CONT_RELOAD 0x2 = RELOAD_CONT 0x3 = RELOAD_RELOAD 0x4 = CONT_LLP 0x5 = RELOAD_LLP 0x6 = LLP_CONT 0x7 = LLP_RELOAD 0x8 = LLP_LLP*/
#define GDMA_CH2_MULTI_BLK_TYPE(x)     (((u32)((x) & 0x0000000F) << 8))
#define GDMA_GET_CH2_MULTI_BLK_TYPE(x) ((u32)(((x >> 8) & 0x0000000F)))
#define GDMA_MASK_CH1_MULTI_BLK_TYPE   ((u32)0x0000000F << 4)            /*!< R 0x0  The values of these bit fields are derived from the DMAC_CHx_MULTI_BLK_TYPE parameter. 0x0 = NO_HARDCODE 0x1 = CONT_RELOAD 0x2 = RELOAD_CONT 0x3 = RELOAD_RELOAD 0x4 = CONT_LLP 0x5 = RELOAD_LLP 0x6 = LLP_CONT 0x7 = LLP_RELOAD 0x8 = LLP_LLP*/
#define GDMA_CH1_MULTI_BLK_TYPE(x)     (((u32)((x) & 0x0000000F) << 4))
#define GDMA_GET_CH1_MULTI_BLK_TYPE(x) ((u32)(((x >> 4) & 0x0000000F)))
#define GDMA_MASK_CH0_MULTI_BLK_TYPE   ((u32)0x0000000F << 0)            /*!< R 0x0  The values of these bit fields are derived from the DMAC_CHx_MULTI_BLK_TYPE parameter. 0x0 = NO_HARDCODE 0x1 = CONT_RELOAD 0x2 = RELOAD_CONT 0x3 = RELOAD_RELOAD 0x4 = CONT_LLP 0x5 = RELOAD_LLP 0x6 = LLP_CONT 0x7 = LLP_RELOAD 0x8 = LLP_LLP*/
#define GDMA_CH0_MULTI_BLK_TYPE(x)     (((u32)((x) & 0x0000000F) << 0))
#define GDMA_GET_CH0_MULTI_BLK_TYPE(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup GDMA_DMA_COMP_PARAMS_1_L
  * @brief Refer to the bit table in the description for it
  * @{
  */
#define GDMA_MASK_CH3_MAX_BLK_SIZE   ((u32)0x000000FF << 24)           /*!< R 0x15  The value of this register is derived from the DMAC_CH3_MAX_BLK_SIZE from config_form.vh CH3_MAX_BLK_SIZE = log2(DMAC_CH3_MAX_BLK_SIZE) + 1*/
#define GDMA_CH3_MAX_BLK_SIZE(x)     (((u32)((x) & 0x000000FF) << 24))
#define GDMA_GET_CH3_MAX_BLK_SIZE(x) ((u32)(((x >> 24) & 0x000000FF)))
#define GDMA_MASK_CH2_MAX_BLK_SIZE   ((u32)0x000000FF << 16)           /*!< R 0x15  The value of this register is derived from the DMAC_CH2_MAX_BLK_SIZE from config_form.vh CH2_MAX_BLK_SIZE = log2(DMAC_CH2_MAX_BLK_SIZE) + 1*/
#define GDMA_CH2_MAX_BLK_SIZE(x)     (((u32)((x) & 0x000000FF) << 16))
#define GDMA_GET_CH2_MAX_BLK_SIZE(x) ((u32)(((x >> 16) & 0x000000FF)))
#define GDMA_MASK_CH1_MAX_BLK_SIZE   ((u32)0x000000FF << 8)            /*!< R 0x15  The value of this register is derived from the DMAC_CH1_MAX_BLK_SIZE from config_form.vh CH1_MAX_BLK_SIZE = log2(DMAC_CH1_MAX_BLK_SIZE) + 1*/
#define GDMA_CH1_MAX_BLK_SIZE(x)     (((u32)((x) & 0x000000FF) << 8))
#define GDMA_GET_CH1_MAX_BLK_SIZE(x) ((u32)(((x >> 8) & 0x000000FF)))
#define GDMA_MASK_CH0_MAX_BLK_SIZE   ((u32)0x000000FF << 0)            /*!< R 0x15  The value of this register is derived from the DMAC_CH0_MAX_BLK_SIZE from config_form.vh CH0_MAX_BLK_SIZE = log2(DMAC_CH0_MAX_BLK_SIZE) + 1*/
#define GDMA_CH0_MAX_BLK_SIZE(x)     (((u32)((x) & 0x000000FF) << 0))
#define GDMA_GET_CH0_MAX_BLK_SIZE(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup GDMA_DMA_COMP_PARAMS_1_H
  * @brief Refer to the bit table in the description for it
  * @{
  */
#define GDMA_BIT_HIGH_BIT_NUM_HS_INT3 ((u32)0x00000001 << 31)           /*!< R 0x0  Number of hardware handshake interfaces. If it is 128, this bit read value is 1.*/
#define GDMA_BIT_HIGH_BIT_NUM_HS_INT2 ((u32)0x00000001 << 30)           /*!< R 0x0  Number of hardware handshake interfaces. If it is greater than 63 and less than 128, this bit read value is 1.*/
#define GDMA_BIT_HIGH_BIT_NUM_HS_INT1 ((u32)0x00000001 << 29)           /*!< R 0x0  Number of hardware handshake interfaces. If it is greater than 31 and less than 64, this bit read value is 1.*/
#define GDMA_BIT_STATIC_ENDIAN_SELECT ((u32)0x00000001 << 28)           /*!< R 0x0  The value of this register is derived from the DMAC_STATIC_ENDIAN_SELECT parameter. 0 = FALSE (dynamic endian fixed) 1 = TRUE (not supported)*/
#define GDMA_BIT_ADD_ENCODED_PARAMS   ((u32)0x00000001 << 27)           /*!< R 0x1  The value of this register is derived from the DMAC_ADD_ENCODED_PARAMS parameter. 0 = FALSE 1 = TRUE*/
#define GDMA_MASK_NUM_HS_INT          ((u32)0x0000001F << 22)           /*!< R 0x1E  Number of hardware handshake interfaces. The value of this register is derived from the DMAC_NUM_HS_INT parameter. NUM_HAS_INT = DMAC_NUM_HS_INT[4:0] 0x00 = 0 ...  0x10 = 16*/
#define GDMA_NUM_HS_INT(x)            (((u32)((x) & 0x0000001F) << 22))
#define GDMA_GET_NUM_HS_INT(x)        ((u32)(((x >> 22) & 0x0000001F)))
#define GDMA_MASK_M2_DATA_WIDTH       ((u32)0x00000003 << 16)           /*!< R 0x0  AXI Master2 data width. The value of this register is derived from the DMAC_M2_DATA_WIDTH parameter. Master2 data width should be equal to that of Master 1. M2_DATA_WIDTH = log2(DMAC_DATA_WIDTH/32) 0x0 = 32 bits 0x1 = 64 bits  0x2 = 128 bits 0x3 = Reserved*/
#define GDMA_M2_DATA_WIDTH(x)         (((u32)((x) & 0x00000003) << 16))
#define GDMA_GET_M2_DATA_WIDTH(x)     ((u32)(((x >> 16) & 0x00000003)))
#define GDMA_MASK_M1_DATA_WIDTH       ((u32)0x00000003 << 14)           /*!< R 0x0  AXI Master1 data width. The value of this register is derived from the DMAC_M1_DATA_WIDTH parameter. Master1 data width should be equal to that of Master 2. M1_DATA_WIDTH = log2(DMAC_DATA_WIDTH/32) 0x0 = 32 bits 0x1 = 64 bits 0x2 = 128 bits 0x3 = Reserved*/
#define GDMA_M1_DATA_WIDTH(x)         (((u32)((x) & 0x00000003) << 14))
#define GDMA_GET_M1_DATA_WIDTH(x)     ((u32)(((x >> 14) & 0x00000003)))
#define GDMA_MASK_S_DATA_WIDTH        ((u32)0x00000003 << 12)           /*!< R 0x0  APB slave interface data width.The value of this register is derived from the DMAC_S_DATA_WIDTH parameter. The width is fixed to 32 bits for now. S_DATA_WIDTH = log2(DMAC_DATA_WIDTH/32) 0x0 = 32 bits 0x1 = Reserved 0x2 = Reserved 0x3 = Reserved*/
#define GDMA_S_DATA_WIDTH(x)          (((u32)((x) & 0x00000003) << 12))
#define GDMA_GET_S_DATA_WIDTH(x)      ((u32)(((x >> 12) & 0x00000003)))
#define GDMA_MASK_NUM_MASTER_INT      ((u32)0x00000003 << 10)           /*!< R 0x1  Number of master interface. The value of this register is derived from the DMAC_NUM_MASTER_INT parameter. This value is fixed to 1 (only 2 AXI masters supported now.) 0x0 = Reserved 0x1 = 2 0x2 = Reserved 0x3 = Reserved*/
#define GDMA_NUM_MASTER_INT(x)        (((u32)((x) & 0x00000003) << 10))
#define GDMA_GET_NUM_MASTER_INT(x)    ((u32)(((x >> 10) & 0x00000003)))
#define GDMA_MASK_NUM_CHANNELS        ((u32)0x0000000F << 6)            /*!< R 0x7  Number of channels. The value of this register is derived from the DMAC_NUM_CHASNNELS parameter. NUM_CHANNELS = DMAC_NUM_CHASNNELS -1 0x0 = 1 ...  0xf = 16*/
#define GDMA_NUM_CHANNELS(x)          (((u32)((x) & 0x0000000F) << 6))
#define GDMA_GET_NUM_CHANNELS(x)      ((u32)(((x >> 6) & 0x0000000F)))
#define GDMA_MASK_INTR_IO             ((u32)0x00000003 << 0)            /*!< R 0x0  Interrupt output format.The value of this register is derived from the DMAC_INTR_IO parameter. 0x0 = ALL 0x1 = TYPE 0x2 = COMBINED 0x3 = reserved*/
#define GDMA_INTR_IO(x)               (((u32)((x) & 0x00000003) << 0))
#define GDMA_GET_INTR_IO(x)           ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup GDMA_DMA_COMPONENT_ID_REGS_L
  * @brief DMAC ID
  * @{
  */
#define GDMA_MASK_DMA_COMPONENT_ID   ((u32)0xFFFFFFFF << 0)           /*!< R 32'h20240513  DMAC ID (release_date_. Configure it in config form.*/
#define GDMA_DMA_COMPONENT_ID(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GDMA_GET_DMA_COMPONENT_ID(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup GDMA_DMA_COMPONENT_ID_REGS_H
  * @brief It is obtained from the high 32 bits from git logs in git server.
  * @{
  */
#define GDMA_MASK_RTL_GIT_VER   ((u32)0xFFFFFFFF << 0)           /*!< R 32'h482f5d83  It is obtained from the high 32 bits from git logs in git server.*/
#define GDMA_RTL_GIT_VER(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GDMA_GET_RTL_GIT_VER(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup GDMA_Exported_Types GDMA Exported Types
  * @{
  */

/** @brief GDMA Port
  */
typedef struct {
	__IO uint32_t GDMA_SARx     ;  /*!< CHANNEL x SOURCE ADDRESS REGISTER */
	__I  uint32_t GDMA_CURR_SARx;  /*!< CHANNEL x SOURCE READING ADDRESS AT PRESENT Register */
	__IO uint32_t GDMA_DARx     ;  /*!< CHANNEL x DESTINATION ADDRESS REGISTER */
	__I  uint32_t GDMA_CURR_DARx;  /*!< CHANNEL x DESTINATION WRITING ADDRESS AT PRESENT Register */
	__IO uint32_t GDMA_LLPx_L   ;  /*!< CHANNEL x LOW-32 BITS LINKED LIST POINTER REGISTER */
	__IO uint32_t RSVD0         ;  /*!< Reserved */
	__IO uint32_t GDMA_CTLx_L   ;  /*!< CHANNEL x LOW-32 BITS CONTROL REGISTER */
	__IO uint32_t GDMA_CTLx_H   ;  /*!< CHANNEL x HIGH-32 BITS CONTROL REGISTER */
	__IO uint32_t RSVD1[8]      ;  /*!< Reserved */
	__IO uint32_t GDMA_CFGx_L   ;  /*!< CHANNEL x LOW-32 BITS CONFIGURATION REGISTER */
	__IO uint32_t GDMA_CFGx_H   ;  /*!< CHANNEL x HIGH-32 BITS CONFIGURATION REGISTER */
	__IO uint32_t GDMA_SGRx_L   ;  /*!< CHANNEL x LOW-32 BITS SOURCE GATHER REGISTER */
	__I  uint32_t GDMA_SGRx_H   ;  /*!< CHANNEL x HIGH-32 BITS SOURCE GATHER REGISTER */
	__IO uint32_t GDMA_DSRx_L   ;  /*!< CHANNEL x LOW-32 BITS DESTINATION SCATTER REGISTER */
	__I  uint32_t GDMA_DSRx_H   ;  /*!< CHANNEL x HIGH-32 BITS DESTINATION SCATTER REGISTER */
} GDMA_ChannelTypeDef;

/** @brief GDMA Register Declaration
  */

typedef struct {
	GDMA_ChannelTypeDef CH[8]                 ;  /*!< GDMA IP have 8 ports,  Address offset:0x000-0x2BC */
	__I  uint32_t GDMA_RAWTFR_L               ;  /*!< RAW STATUS FOR INTTFR INTERRUPT Register,  Address offset:0x2C0 */
	__IO uint32_t RSVD0                       ;  /*!< Reserved,  Address offset:0x2C4-0x2C7 */
	__I  uint32_t GDMA_RAWBLOCK_L             ;  /*!< RAW STATUS FOR INTBLOCK INTERRUPT Register,  Address offset:0x2C8 */
	__IO uint32_t RSVD1[5]                    ;  /*!< Reserved,  Address offset:0x2CC-0x2DF */
	__I  uint32_t GDMA_RAWERR_L               ;  /*!< RAW STATUS FOR INTERR INTERRUPT(PROTOCOL ERROR) Register,  Address offset:0x2E0 */
	__I  uint32_t GDMA_RAWERR_H               ;  /*!< RAW STATUS FOR NON-SECURE ACCESS SECURE REGISTER ERROR IF DEFINED Register,  Address offset:0x2E4 */
	__I  uint32_t GDMA_STATUSTFR_L            ;  /*!< STATUS FOR INTTFR INTERRUPT Register,  Address offset:0x2E8 */
	__IO uint32_t RSVD2                       ;  /*!< Reserved,  Address offset:0x2EC-0x2EF */
	__I  uint32_t GDMA_STATUSBLOCK_L          ;  /*!< STATUS FOR INTBLOCK INTERRUPT Register,  Address offset:0x2F0 */
	__IO uint32_t RSVD3[5]                    ;  /*!< Reserved,  Address offset:0x2F4-0x307 */
	__I  uint32_t GDMA_STATUSERR_L            ;  /*!< STATUS FOR INTERR INTERRUPT Register,  Address offset:0x308 */
	__I  uint32_t GDMA_STATUSERR_H            ;  /*!< STATUS FOR NON-SECURE ACCESS SECURE REGISTER ERROR IF DEFINED Register,  Address offset:0x30C */
	__IO uint32_t GDMA_MASKTFR_L              ;  /*!< MASK FOR INTTFR INTERRUPT (CHN0-7) Register,  Address offset:0x310 */
	__IO uint32_t RSVD4                       ;  /*!< Reserved,  Address offset:0x314-0x317 */
	__IO uint32_t GDMA_MASKBLOCK_L            ;  /*!< MASK FOR INTBLOCK INTERRUPT Register,  Address offset:0x318 */
	__IO uint32_t RSVD5[5]                    ;  /*!< Reserved,  Address offset:0x31C-0x32F */
	__IO uint32_t GDMA_MASKERR_L              ;  /*!< MASK FOR INTERR INTERRUPT Register,  Address offset:0x330 */
	__IO uint32_t GDMA_MASKERR_H              ;  /*!< MASK FOR NON-SECURE ACCESS SECURE REGISTER ERROR IF DEFINED Register,  Address offset:0x334 */
	__O  uint32_t GDMA_CLEARTFR_L             ;  /*!< CLEAR FOR INTTFR INTERRUPT Register,  Address offset:0x338 */
	__IO uint32_t RSVD6                       ;  /*!< Reserved,  Address offset:0x33C-0x33F */
	__O  uint32_t GDMA_CLEARBLOCK_L           ;  /*!< CLEAR FOR INTBLOCK INTERRUPT Register,  Address offset:0x340 */
	__IO uint32_t RSVD7[5]                    ;  /*!< Reserved,  Address offset:0x344-0x357 */
	__O  uint32_t GDMA_CLEARERR_L             ;  /*!< CLEAR FOR INTERR INTERRUPT Register,  Address offset:0x358 */
	__O  uint32_t GDMA_CLEARERR_H             ;  /*!< CLEAR FOR NON-SECURE ACCESS SECURE REGISTER ERROR IF DEFINED Register,  Address offset:0x35C */
	__I  uint32_t GDMA_STATUSINT_L            ;  /*!< COMBINED INTERRUPT SATAUS REGISTER,  Address offset:0x360 */
	__IO uint32_t GDMA_STATUSINT_H            ;  /*!< COMBINED INTERRUPT SATAUS REGISTER,  Address offset:0x364 */
	__IO uint32_t RSVD8[12]                   ;  /*!< Reserved,  Address offset:0x368-0x397 */
	__IO uint32_t GDMA_DMACFGREG_L            ;  /*!< DMA LOW-32 BITS CONFIGURATION REGISTER,  Address offset:0x398 */
	__IO uint32_t RSVD9                       ;  /*!< Reserved,  Address offset:0x39C-0x39F */
	__IO uint32_t GDMA_CHENREG_L_1
	;  /*!< DMA LOW-32 BITS CHANNEL ENABLE REGISTER IF CONFIGED CHN NUMBER IS LESS THAN OR EQUAL TO 8 Register,  Address offset:0x3A0 */
	__IO uint32_t RSVD10                      ;  /*!< Reserved,  Address offset:0x3A4-0x3A7 */
	__I  uint32_t GDMA_DMAIDREG_L             ;  /*!< DMA LOW-32 BITS ID REGISTER,  Address offset:0x3A8 */
	__IO uint32_t RSVD11[3]                   ;  /*!< Reserved,  Address offset:0x3AC-0x3B7 */
	__IO uint32_t GDMA_DMAOSNUM_L             ;  /*!< DMA LOW-32 BITS OUTSTANDING NUMBER REGISTER,  Address offset:0x3B8 */
	__IO uint32_t RSVD12[3]                   ;  /*!< Reserved,  Address offset:0x3BC-0x3C7 */
	__I  uint32_t GDMA_DMA_COMP_PARAMS_6_L    ;  /*!< REFER TO THE BIT TABLE IN THE DESCRIPTION FOR IT Register,  Address offset:0x3C8 */
	__I  uint32_t GDMA_DMA_COMP_PARAMS_6_H    ;  /*!< REFER TO THE BIT TABLE IN THE DESCRIPTION FOR IT Register,  Address offset:0x3CC */
	__I  uint32_t GDMA_DMA_COMP_PARAMS_5_L    ;  /*!< REFER TO THE BIT TABLE IN THE DESCRIPTION FOR IT Register,  Address offset:0x3D0 */
	__I  uint32_t GDMA_DMA_COMP_PARAMS_5_H    ;  /*!< REFER TO THE BIT TABLE IN THE DESCRIPTION FOR IT Register,  Address offset:0x3D4 */
	__I  uint32_t GDMA_DMA_COMP_PARAMS_4_L    ;  /*!< REFER TO THE BIT TABLE IN THE DESCRIPTION FOR IT Register,  Address offset:0x3D8 */
	__I  uint32_t GDMA_DMA_COMP_PARAMS_4_H    ;  /*!< REFER TO THE BIT TABLE IN THE DESCRIPTION FOR IT Register,  Address offset:0x3DC */
	__I  uint32_t GDMA_DMA_COMP_PARAMS_3_L    ;  /*!< REFER TO THE BIT TABLE IN THE DESCRIPTION FOR IT Register,  Address offset:0x3E0 */
	__I  uint32_t GDMA_DMA_COMP_PARAMS_3_H    ;  /*!< REFER TO THE BIT TABLE IN THE DESCRIPTION FOR IT Register,  Address offset:0x3E4 */
	__I  uint32_t GDMA_DMA_COMP_PARAMS_2_L    ;  /*!< REFER TO THE BIT TABLE IN THE DESCRIPTION FOR IT Register,  Address offset:0x3E8 */
	__I  uint32_t GDMA_DMA_COMP_PARAMS_2_H    ;  /*!< REFER TO THE BIT TABLE IN THE DESCRIPTION FOR IT Register,  Address offset:0x3EC */
	__I  uint32_t GDMA_DMA_COMP_PARAMS_1_L    ;  /*!< REFER TO THE BIT TABLE IN THE DESCRIPTION FOR IT Register,  Address offset:0x3F0 */
	__I  uint32_t GDMA_DMA_COMP_PARAMS_1_H    ;  /*!< REFER TO THE BIT TABLE IN THE DESCRIPTION FOR IT Register,  Address offset:0x3F4 */
	__I  uint32_t GDMA_DMA_COMPONENT_ID_REGS_L;  /*!< DMAC ID Register,  Address offset:0x3F8 */
	__I  uint32_t GDMA_DMA_COMPONENT_ID_REGS_H;  /*!< IT IS OBTAINED FROM THE HIGH 32 BITS FROM GIT LOGS IN GIT SERVER. Register,  Address offset:0x3FC */
} GDMA_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your definition here
/* Exported types --------------------------------------------------------*/
/** @addtogroup GDMA_Exported_Types GDMA Exported Types
  * @{
  */

/**
  * @brief  GDMA Init Structure Definition
  */
typedef struct {
	u8	GDMA_Index;               /*!< Specifies the GDMA index.
					                           This parameter can be the value 0.*/

	u8	GDMA_ChNum;               /*!< Specifies the GDMA channel number.
					                           This parameter can be the value 0 ~ 7.*/

	u8	GDMA_ChPrior;             /*!< Specifies the GDMA channel priority.
					                           This parameter can be the value 0 ~ 7, while 0 for highset priority.
	                                   If more than one channel are set with the same priroty value,
							                      the channel with small channel number index has higher priority.*/

	u32	GDMA_DIR;	                /*!< Specifies the GDMA transmission direction.
					                           This parameter can be a value of @ref GDMA_Data_Transfer_Direction */

	u32	GDMA_DstDataWidth;        /*!< Specifies the GDMA destination transfer width.
					                           This parameter can be a value of @ref GDMA_Data_Size */

	u32	GDMA_SrcDataWidth;        /*!< Specifies the GDMA transfer width.
					                           This parameter can be a value of @ref GDMA_Data_Size */

	u32	GDMA_DstInc;		          /*!< Specifies the GDMA destination address increment mode.
					                           This parameter can be a value of @ref GDMA_Increment_Mode */

	u32	GDMA_SrcInc;              /*!< Specifies the GDMA source address increment mode.
					                           This parameter can be a value of @ref GDMA_Increment_Mode */

	u32	GDMA_DstMsize;            /*!< Specifies the GDMA destination burst transaction length.
					                           This parameter can be a value of @ref GDMA_Msize */

	u32	GDMA_SrcMsize;            /*!< Specifies the GDMA source burst transaction length.
					                           This parameter can be a value of @ref GDMA_Msize */

	u32	GDMA_SrcAddr;             /*!< Specifies the GDMA source address.
					                           This parameter can be a value of the memory or peripheral space address,
					                           depending on the GDMA data transfer direction.If this address is configured,
					                           GDMA will move data from here to the destination address space*/

	u32	GDMA_DstAddr;             /*!< Specifies the GDMA destination address.
					                           This parameter can be a value of the memory or peripheral space address,
					                           depending on the GDMA data transfer direction.If this address is configured,
					                           GDMA will move data here from source address space*/

	u32	GDMA_BlockSize; 	        /*!< Specifies the GDMA block transfer size.
	                                   This parameter can be a value between 1 ~ 2097151.
					                           @note This parameter indicates the total number of single transactions for
					                           every block transfer. The field for this parameter locates in CTLx_L[20:0], so
					                           the value of this parameter must be no more than 0x1FFFFF.*/

	u32	GDMA_IsrType;             /*!< Specifies the GDMA interrupt types.
					                           This parameter can be a value of @ref GDMA_Interrupt_Type */

	u32	GDMA_ReloadSrc;           /*!< Specifies the GDMA automatic source reload .
					                           This parameter can be the 0 or 1.(0 : disable / 1 : enable).
					                           @note if this value is setted 1, source address register can be automatically
					                           reloaded from its initial value at the end of every block for multi-block transfers.
					                           this parameter is only valid in multi block transmission mode*/

	u32	GDMA_ReloadDst;           /*!< Specifies the GDMA automatic destination reload .
					                           This parameter can be the 0 or 1.(0 : disable / 1 : enable).
					                           @note if this parameter is set 1, destination address register can be automatically
					                           reloaded from its initial value at the end of every block for multi-block transfers.
					                           this parameter is only valid in multi block transmission mode*/

	u32	GDMA_LlpDstEn;            /*!< Specifies the GDMA whether block chaining is enabled or disabled on the destination side only.
					                           @note this parameter is only valid in multi-block transmission mode*/

	u32	GDMA_LlpSrcEn;            /*!< Specifies the GDMA whether block chaining is enabled or disabled on the source side only.
					                           @note this parameter is only valid in multi-block transmission mode*/

	u32	GDMA_SrcHandshakeInterface;    /*!< Specifies the GDMA hardware handshaking interface for the source
							                       peripheral of a GDMA channel.
							                       This parameter can be a value of @ref GDMA0_HS_HandShake_Interface */

	u32	GDMA_DstHandshakeInterface;   /*!< Specifies the GDMA hardware handshaking interface for the destination
							                       peripheral of a GDMA channel.
							                       This parameter can be a value of @ref GDMA0_HS_HandShake_Interface */

	u32	MuliBlockCunt;            /*!< Specifies the GDMA Multi-block counter. This parameter is used in multi-block transmission.*/

	u32	MaxMuliBlock;             /*!< Specifies the GDMA Max block number in Multi-block transmission.
					                           This parameter is used in multi-block transmission.*/
	u32	SecureTransfer;		        /*!< Specifies the GDMA secure transmission. This parameter is used in secure world of trustzone.*/
} GDMA_InitTypeDef, *PGDMA_InitTypeDef;
/**
  * @brief  GDMA LLI ELE Structure Definition
  */
typedef struct {
	u32 Sarx;                     /*!< Specifies the GDMA channel x Source Address Register (SARx) value field of a block descriptor
	                                   in block chaining. This parameter stores the source address of the current block transfer.*/

	u32 Darx;                     /*!< Specifies the GDMA channel x Destination Address Register(DARx) value field of a block descriptor
	                                   in block chaining. This parameter stores the destination address of the current block transfer.*/

	u32 Llpx;                     /*!< Specifies the GDMA channel x Linked List Pointer Register(LLPx) value field of a block descriptor
	                                   in block chaining. This parameter is a address, which points to the next block descriptor.*/

	u32 CtlxLow;                  /*!< Specifies the GDMA channel x Control Register(CTRx) Low 32 bit value field of a block descriptor
	                                    in block chaining.This parameter stores the DMA control parameters of the current block transfer.*/

	u32 CtlxUp;                   /*!< Specifies the GDMA channel x Control Register(CTRx) High 32 bit value field of a block descriptor
	                                    in block chaining.This parameter stores the DMA control parameters of the current block transfer.*/

	u32 Temp;                     /*!< Specifies the reserved GDMA channel x register value field of a block descriptor
	                                    in block chaining.*/
} GDMA_CH_LLI_ELE, *PGDMA_CH_LLI_ELE;

/**
  * @brief  GDMA CH LLI Structure Definition
  */
struct GDMA_CH_LLI {
	GDMA_CH_LLI_ELE	 LliEle;     /*!< Specifies the GDMA Linked List Item Element structure field of Linked List Item
	                                  in block chaining.This structure variable stores the necessary parameters of a block descriptor.*/

	u32 BlockSize;               /*!< Specifies the GDMA block size of one block in block chaining.
					                          This parameter indicates the block size of the current block transfer.*/

	struct GDMA_CH_LLI *pNextLli;/*!< Specifies the GDMA Linked List Item pointer.
					                          This parameter stores the address pointing to the next Linked List Item in block chaining.*/
};

/**
  * @}
  */


/* Exported constants --------------------------------------------------------*/
/** @defgroup GDMA_Exported_Constants GDMA Exported Constants
  * @{
  */

/** @defgroup GDMA_Index_Channel_Definition
  * @{
  */
#define MAX_GDMA_INDX		              (0)
#define MAX_GDMA_CHNL		              (7)

#define IS_GDMA_ChannelNum(NUM)       ((NUM) <= MAX_GDMA_CHNL)
#define IS_GDMA_Index(NUM)            ((NUM) <= MAX_GDMA_INDX)
/**
  * @}
  */

/** @defgroup GDMA_Data_Transfer_Direction
  * @{
  */
#define TTFCMemToMem						      ((u32)0x00000000)
#define TTFCMemToPeri						      ((u32)0x00000001)
#define TTFCPeriToMem						      ((u32)0x00000002)
#define TTFCPeriToPeri						    ((u32)0x00000003)
#define TTFCPeriToMem_PerCtrl			    ((u32)0x00000004)
#define TTFCPeriToPeri_SrcPerCtrl			((u32)0x00000005)
#define TTFCMemToPeri_PerCtrl				  ((u32)0x00000006)
#define TTFCPeriToPeri_DstPerCtrl			((u32)0x00000007)

#define IS_GDMA_DIR(DIR)              (((DIR) == TTFCMemToMem) || \
                                       ((DIR) == TTFCMemToPeri) || \
                                       ((DIR) == TTFCPeriToMem) ||\
                                       ((DIR) == TTFCPeriToPeri) ||\
                                       ((DIR) == TTFCPeriToMem_PerCtrl) || \
                                       ((DIR) == TTFCPeriToPeri_SrcPerCtrl) || \
                                       ((DIR) == TTFCPeriToPeri_DstPerCtrl) || \
                                       ((DIR) == TTFCMemToPeri_PerCtrl))
/**
  * @}
  */

/** @defgroup GDMA_Data_Size
  * @{
  */
#define TrWidthOneByte						    ((u32)0x00000000)
#define TrWidthTwoBytes						    ((u32)0x00000001)
#define TrWidthFourBytes						  ((u32)0x00000002)
#define TrWidthEightBytes             ((u32)0x00000003)//not support
#define IS_GDMA_DATA_SIZE(SIZE)       (((SIZE) == TrWidthOneByte) || \
                                       ((SIZE) == TrWidthTwoBytes) || \
                                       ((SIZE) == TrWidthFourBytes))
/**
  * @}
  */

/** @defgroup GDMA_Msize
  * @{
  */
#define MsizeOne							        ((u32)0x00000000)
#define MsizeFour							        ((u32)0x00000001)
#define MsizeEight							      ((u32)0x00000002)
#define MsizeSixteen						      ((u32)0x00000003)
#define Msize32								        ((u32)0x00000004)
#define Msize64								        ((u32)0x00000005)
#define Msize128							        ((u32)0x00000006)
#define Msize256							        ((u32)0x00000007)

#define IS_GDMA_MSIZE(SIZE)           (((SIZE) == MsizeOne) || \
                                       ((SIZE) == MsizeFour) || \
                                       ((SIZE) == MsizeEight)||\
                                       ((SIZE) == MsizeSixteen))
/**
  * @}
  */

/** @defgroup GDMA_Increment_Mode
  * @{
  */
#define IncType								        ((u32)0x00000000)
#define DecType								        ((u32)0x00000001)
#define NoChange							        ((u32)0x00000002)

#define IS_GDMA_IncMode(STATE)        (((STATE) == IncType) || \
                                       ((STATE) == DecType) || \
                                       ((STATE) == NoChange))
/**
  * @}
  */

/** @defgroup GDMA_Interrupt_Type
  * @{
  */
#define TransferType							    ((u32)0x00000001)
#define BlockType							        ((u32)0x00000002)
#define SrcTransferType						    ((u32)0x00000004)
#define DstTransferType						    ((u32)0x00000008)
#define ErrType								        ((u32)0x000000010)

#define IS_GDMA_CONFIG_IT(IT)         ((((IT) & 0xFFFFFFE0) == 0x00) && ((IT) != 0x00))
/**
  * @}
  */

/** @defgroup GDMA_Reload_Definition
  * @{
  */
#define CLEAN_RELOAD_SRC					    ((u32)0x00000001)
#define CLEAN_RELOAD_DST					    ((u32)0x00000002)
#define CLEAN_RELOAD_SRC_DST				  ((u32)0x00000003)
/**
  * @}
  */

/** @defgroup GDMA_Interrupt_Status
  * @{
  */
#define GDMA_INT_GET(reg, channel)          ((reg) & BIT(channel))
#define GDMA_INT_SET(reg, channel)          ((reg) = (BIT(channel) | BIT(channel + 8)))
#define GDMA_INT_MASK(reg, channel)         ((reg) = (BIT(channel + 8)))

#define GDMA_INT_STATUS(raw_reg, reg, channel)    (GDMA_INT_GET(raw_reg, channel) && GDMA_INT_GET(reg, channel))

/**
  * @}
  */

/** @defgroup GDMA_Channel_Status
  * @{
  */
#define GDMA_GET_CH_STATUS(cfg_reg)		(((cfg_reg) & (GDMA_BIT_CFGx_L_DST_PCTL_OVER | GDMA_BIT_CFGx_L_SRC_PCTL_OVER | GDMA_BIT_CFGx_L_INACTIVE)))
/**
  * @}
  */

/** @defgroup GDMA0_HS_HandShake_Interface
  * @{
  */
#define GDMA_HANDSHAKE_INTERFACE_UART0_TX		  (0)
#define GDMA_HANDSHAKE_INTERFACE_UART0_RX		  (1)
#define GDMA_HANDSHAKE_INTERFACE_UART1_TX		  (2)
#define GDMA_HANDSHAKE_INTERFACE_UART1_RX		  (3)
#define GDMA_HANDSHAKE_INTERFACE_UART2_TX		  (4)
#define GDMA_HANDSHAKE_INTERFACE_UART2_RX		  (5)

#define GDMA_HANDSHAKE_INTERFACE_SPI0_TX		  (6)
#define GDMA_HANDSHAKE_INTERFACE_SPI0_RX		  (7)
#define GDMA_HANDSHAKE_INTERFACE_SPI1_TX		  (8)
#define GDMA_HANDSHAKE_INTERFACE_SPI1_RX		  (9)

#define GDMA_HANDSHAKE_INTERFACE_SPIC_TX		  (10)
#define GDMA_HANDSHAKE_INTERFACE_SPIC_RX		  (11)

#define GDMA_HANDSHAKE_INTERFACE_SPORT0F0_TX	(12)
#define GDMA_HANDSHAKE_INTERFACE_SPORT0F1_TX	(13)
#define GDMA_HANDSHAKE_INTERFACE_SPORT0F0_RX	(14)
#define GDMA_HANDSHAKE_INTERFACE_SPORT0F1_RX	(15)

#define GDMA_HANDSHAKE_INTERFACE_I2C0_TX		  (16)
#define GDMA_HANDSHAKE_INTERFACE_I2C0_RX		  (17)
#define GDMA_HANDSHAKE_INTERFACE_I2C1_TX		  (18)
#define GDMA_HANDSHAKE_INTERFACE_I2C1_RX		  (19)

#define GDMA_HANDSHAKE_INTERFACE_A2C0_RX      (20)
#define GDMA_HANDSHAKE_INTERFACE_A2C1_RX      (21)

#define GDMA_HANDSHAKE_INTERFACE_UART_LOG_RX	(22)
#define GDMA_HANDSHAKE_INTERFACE_UART_LOG_TX	(23)

#define GDMA_HANDSHAKE_INTERFACE_UART3_TX		  (24)
#define GDMA_HANDSHAKE_INTERFACE_UART3_RX		  (25)

#define GDMA_HANDSHAKE_INTERFACE_PSRAMC_RX		(26)

#define GDMA_HANDSHAKE_INTERFACE_CTC_RX			(27)

#define GDMA_HANDSHAKE_INTERFACE_Zigbee_TX		(NULL)
#define GDMA_HANDSHAKE_INTERFACE_Zigbee_RX		(NULL)
#define GDMA_HANDSHAKE_INTERFACE_AUDIO_TX		  (NULL)
#define GDMA_HANDSHAKE_INTERFACE_AUDIO_RX		  (NULL)
/**
  * @}
  */

/**
  * @}
  */

/* MANUAL_GEN_END */

/** @} */

/** @} */

/* Exported functions --------------------------------------------------------*/
/** @defgroup GDMA_Exported_Functions GDMA Exported Functions
  * @{
  */

_LONG_CALL_ void GDMA_StructInit(PGDMA_InitTypeDef GDMA_InitStruct);
_LONG_CALL_ void GDMA_Init(u8 GDMA_Index, u8 GDMA_ChNum, PGDMA_InitTypeDef GDMA_InitStruct);
_LONG_CALL_ void GDMA_SetLLP(u8 GDMA_Index, u8 GDMA_ChNum, u32 MultiBlockCount, struct GDMA_CH_LLI *pGdmaChLli, u32 round);
_LONG_CALL_ void GDMA_Cmd(u8 GDMA_Index, u8 GDMA_ChNum, u32 NewState);
_LONG_CALL_ void GDMA_INTConfig(u8 GDMA_Index, u8 GDMA_ChNum, u32 GDMA_IT, u32 NewState);
_LONG_CALL_ u32	 GDMA_ClearINTPendingBit(u8 GDMA_Index, u8 GDMA_ChNum, u32 GDMA_IT);
_LONG_CALL_ u32	 GDMA_ClearINT(u8 GDMA_Index, u8 GDMA_ChNum);
_LONG_CALL_ void GDMA_ChCleanAutoReload(u8 GDMA_Index, u8 GDMA_ChNum, u32 CleanType);

_LONG_CALL_ void GDMA_SetSrcAddr(u8 GDMA_Index, u8 GDMA_ChNum, u32 SrcAddr);
_LONG_CALL_ u32	 GDMA_GetSrcAddr(u8 GDMA_Index, u8 GDMA_ChNum);
_LONG_CALL_ u32	 GDMA_GetDstAddr(u8 GDMA_Index, u8 GDMA_ChNum);
_LONG_CALL_ void GDMA_SetDstAddr(u8 GDMA_Index, u8 GDMA_ChNum, u32 DstAddr);
_LONG_CALL_ void GDMA_SetBlkSize(u8 GDMA_Index, u8 GDMA_ChNum, u32 BlkSize);
_LONG_CALL_ u32	 GDMA_GetBlkSize(u8 GDMA_Index, u8 GDMA_ChNum);

_LONG_CALL_ u8	 GDMA_ChnlAlloc(u32 GDMA_Index, IRQ_FUN IrqFun, u32 IrqData, u32 IrqPriority);
_LONG_CALL_ u8   GDMA_ChnlFree(u8 GDMA_Index, u8 GDMA_ChNum);
_LONG_CALL_ u8	 GDMA_GetIrqNum(u8 GDMA_Index, u8 GDMA_ChNum);
_LONG_CALL_ void GDMA_SetChnlPriority(u8 GDMA_Index, u8 GDMA_ChNum, u32 ChnlPriority);
_LONG_CALL_ void GDMA_Suspend(u8 GDMA_Index, u8 GDMA_ChNum);
_LONG_CALL_ void GDMA_Resume(u8 GDMA_Index, u8 GDMA_ChNum);
_LONG_CALL_ u8   GDMA_Abort(u8 GDMA_Index, u8 GDMA_ChNum);
_LONG_CALL_ void GDMA_SourceGather(u8 GDMA_Index, u8 GDMA_ChNum, u32 Src_GatherCount, u32 Src_GatherInterval);
_LONG_CALL_ void GDMA_DestinationScatter(u8 GDMA_Index, u8 GDMA_ChNum, u32 Dst_ScatterCount, u32 Dst_ScatterInterval);
/**
  * @}
  */
#endif //_AMEBA_GDMA_H_