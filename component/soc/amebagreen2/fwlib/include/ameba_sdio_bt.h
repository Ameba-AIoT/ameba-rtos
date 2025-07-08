/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBAGREEN2_SDIO_BT_H_
#define _AMEBAGREEN2_SDIO_BT_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup SDIO_BT
  * @brief SDIO_BT driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup SDIO_BT_Register_Definitions SDIO_BT Register Definitions
  * @{
  */

/** @defgroup SPDIO_TXBD_ADDR
  * @brief
  * @{
  */
#define SDIO_BT_MASK_SPDIO_TXFIFO_ADDR   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  The base address of TXBD The value of this register should be configure by CPU It should be 4 byte alignment (Because TXBD Size is 8 Byte).*/
#define SDIO_BT_SPDIO_TXFIFO_ADDR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SDIO_BT_GET_SPDIO_TXFIFO_ADDR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SPDIO_TXBD_NUM
  * @brief
  * @{
  */
#define SDIO_BT_MASK_SPDIO_TXBD_NUM   ((u32)0x0000FFFF << 0)           /*!< R/W 0x0  The Number of TXBD. Unit: Number The value of this register should be configured by CPU*/
#define SDIO_BT_SPDIO_TXBD_NUM(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define SDIO_BT_GET_SPDIO_TXBD_NUM(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup SPDIO_TXBD_WPTR
  * @brief
  * @{
  */
#define SDIO_BT_MASK_SPDIO_TXBD_H2C_WPTR   ((u32)0x0000FFFF << 0)           /*!< R 0x0  When the packet has been sent from Host to TX Buffer, this index should be updated by SDIO IP.*/
#define SDIO_BT_SPDIO_TXBD_H2C_WPTR(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define SDIO_BT_GET_SPDIO_TXBD_H2C_WPTR(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup SPDIO_TXBD_RPTR
  * @brief
  * @{
  */
#define SDIO_BT_MASK_SPDIO_TXBD_H2C_RPTR   ((u32)0x0000FFFF << 0)           /*!< R/W 0x0  When the packet has been processed by CPU and moved to MAC TX FIFO, this index should be updated by CPU.*/
#define SDIO_BT_SPDIO_TXBD_H2C_RPTR(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define SDIO_BT_GET_SPDIO_TXBD_H2C_RPTR(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup SPDIO_RXBD_ADDR
  * @brief
  * @{
  */
#define SDIO_BT_MASK_SPDIO_RXBD_ADDR   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  The base address of RX BD. The value of this register should be configured by CPU. It should be 8-Byte alignment*/
#define SDIO_BT_SPDIO_RXBD_ADDR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SDIO_BT_GET_SPDIO_RXBD_ADDR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SPDIO_RXBD_NUM
  * @brief
  * @{
  */
#define SDIO_BT_MASK_SPDIO_RXBD_NUM   ((u16)0x0000FFFF << 0)           /*!< R/W 0x0  The total count of RX BD The value of this register should be configured by CPU. The unit is BD count*/
#define SDIO_BT_SPDIO_RXBD_NUM(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define SDIO_BT_GET_SPDIO_RXBD_NUM(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup SPDIO_RXBD_C2H_WPTR
  * @brief
  * @{
  */
#define SDIO_BT_MASK_SPDIO_RXBD_C2H_WPTR   ((u16)0x0000FFFF << 0)           /*!< R/W 0x0  When the packet has been processed by CPU and to be moved to host, this index should be updated by CPU*/
#define SDIO_BT_SPDIO_RXBD_C2H_WPTR(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define SDIO_BT_GET_SPDIO_RXBD_C2H_WPTR(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup SPDIO_RXBD_C2H_RPTR
  * @brief
  * @{
  */
#define SDIO_BT_MASK_SPDIO_RXBD_C2H_RPTR   ((u16)0x0000FFFF << 0)           /*!< R 0x0  When the packet has been moved to host, this index should be updated by SDIO IP.*/
#define SDIO_BT_SPDIO_RXBD_C2H_RPTR(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define SDIO_BT_GET_SPDIO_RXBD_C2H_RPTR(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup SPDIO_HCI_RX_REQ
  * @brief
  * @{
  */
#define SDIO_BT_BIT_HCI_RX_REQ ((u8)0x00000001 << 0) /*!< R/W 0x0  CPU triggers this bit to enable SDIO IP RX transfer by fetch BD info. SDIO fetch BD to get the RX length and address and transfer RX packet to Host, and clear this bit when all BD transfer done.*/
/** @} */

/** @defgroup SPDIO_CPU_RST_SDIO_DMA
  * @brief
  * @{
  */
#define SDIO_BT_BIT_CPU_RST_SDIO_DMA  ((u8)0x00000001 << 7) /*!< R/W 0x0  CPU set this bit to reset SDIO DMA. This bit is auto clear.*/
#define SDIO_BT_BIT_SDIO_DAT_EDGE_INV ((u8)0x00000001 << 0) /*!< R/W 0x0  Invert SDIO Latch input data edge.*/
/** @} */

/** @defgroup SPDIO_RX_REQ_ADDR
  * @brief
  * @{
  */
#define SDIO_BT_MASK_SPDIO_RX_REQ_ADDR   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  The address of the requested RX Packet. This is unnecessary for driver.*/
#define SDIO_BT_SPDIO_RX_REQ_ADDR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SDIO_BT_GET_SPDIO_RX_REQ_ADDR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SPDIO_CPU_INT_MASK
  * @brief
  * @{
  */
#define SDIO_BT_BIT_CCCR_IOE2_CLR_MSK      ((u16)0x00000001 << 14) /*!< R/W 0x0  This bit is clear, and CCCR_IOE2_CLR is masked*/
#define SDIO_BT_BIT_CCCR_IOE2_SET_MSK      ((u16)0x00000001 << 13) /*!< R/W 0x0  This bit is clear, and CCCR_IOE2_SET is masked*/
#define SDIO_BT_BIT_H2C_SUS_REQ_MSK        ((u16)0x00000001 << 12) /*!< R/W 0x0  This bit is clear, and H2C_SUS_REQ is masked*/
#define SDIO_BT_BIT_HOST_CMD11_INT_MSK     ((u16)0x00000001 << 11) /*!< R/W 0x0  This bit is clear, and HOST_CMD11_INT is masked*/
#define SDIO_BT_BIT_HOST_WAKE_CPU_INT_MSK  ((u16)0x00000001 << 10) /*!< R/W 0x0  This bit is clear, and HOST_WAKE_CPU_INT is masked*/
#define SDIO_BT_BIT_RX_BD_AVAI_INT_MSK     ((u16)0x00000001 << 9)  /*!< R/W 0x0  This bit is clear, and RX_BD_AVAI_INT is masked*/
#define SDIO_BT_BIT_RX_BD_FLAG_ERR_INT_MSK ((u16)0x00000001 << 8)  /*!< R/W 0x0  This bit is clear, and RX_BD_FLAG_ERR_INT is masked*/
#define SDIO_BT_BIT_SDIO_RST_CMD_INT_MSK   ((u16)0x00000001 << 7)  /*!< R/W 0x0  This bit is clear, and SDIO_RST_CMD_INT is masked*/
#define SDIO_BT_BIT_SDIO_CS_RST_MSK        ((u16)0x00000001 << 6)  /*!< R/W 0x0  This bit is clear, and SDIO_CS_RST is masked*/
#define SDIO_BT_BIT_SDIO_CS_RDY_MSK        ((u16)0x00000001 << 5)  /*!< R/W 0x0  This bit is clear, and SDIO_CS_RDY is masked*/
#define SDIO_BT_BIT_C2H_DMA_OK_MSK         ((u16)0x00000001 << 3)  /*!< R/W 0x0  This bit is clear, and C2H_DMA_OK _INT is masked*/
#define SDIO_BT_BIT_H2C_DMA_OK_MSK         ((u16)0x00000001 << 2)  /*!< R/W 0x0  This bit is clear, and H2C_DMA_OK _INT is masked*/
#define SDIO_BT_BIT_H2C_BUS_RES_FAIL_MSK   ((u16)0x00000001 << 1)  /*!< R/W 0x0  This bit is clear, and H2C_BUS_RES_FAIL_INT is masked*/
#define SDIO_BT_BIT_TXBD_H2C_OVF__MSK      ((u16)0x00000001 << 0)  /*!< R/W 0x0  This bit is clear, and TXBD_H2C_OVF is masked*/
/** @} */

/** @defgroup SPDIO_CPU_INT_STATUS
  * @brief
  * @{
  */
#define SDIO_BT_BIT_CCCR_IOE2_CLR      ((u16)0x00000001 << 14) /*!< R/W 0x0  this bit will be triggered to inform CPU when host disable cccr fn2 ioe*/
#define SDIO_BT_BIT_CCCR_IOE2_SET      ((u16)0x00000001 << 13) /*!< R/W 0x0  this bit will be triggered to inform CPU when host enable cccr fn2 ioe*/
#define SDIO_BT_BIT_H2C_SUS_REQ        ((u16)0x00000001 << 12) /*!< R/W 0x0  Host write sdio local reg to trigger suspend request interrupt*/
#define SDIO_BT_BIT_HOST_CMD11_INT     ((u16)0x00000001 << 11) /*!< R/W 0x0  When Host sends CMD11 to notify that the signal voltage level is going to switch to 1.8v, the HW should issue this interrupt to notify the local CPU to do the LDO power voltage level switch.*/
#define SDIO_BT_BIT_HOST_WAKE_CPU_INT  ((u16)0x00000001 << 10) /*!< R/W 0x0  When Host Send CMD52 wakeup or TRX CMD53 while CPU is not ready (SYSTEM_CPU_RDY_IND=0, or CPU_RDY_IND=0), trigger this interrupt to wake CPU, and then indicate BUSY status to host. CPU should be able to receive this interrupt even CPU clock is gated; therefore, this interrupt should be connected to System On circuit.*/
#define SDIO_BT_BIT_RX_BD_AVAI_INT     ((u16)0x00000001 << 9)  /*!< R/W 0x0  If the free RXBD Number become larger than FREE_RXBD_COUNT(0x1D8), trigger this interrupt. This interrupt trigger only once when free RXBD number cross FREE_RXBD_COUNT*/
#define SDIO_BT_BIT_RX_BD_FLAG_ERR_INT ((u16)0x00000001 << 8)  /*!< R/W 0x0  Trigger by SDIO to CPU when detect RX_BD error*/
#define SDIO_BT_BIT_SDIO_RST_CMD_INT   ((u16)0x00000001 << 7)  /*!< R/W 0x0  Trigger by SDIO to CPU when SDIO is reset. CPU should be able to receive this interrupt even CPU clock is gated; therefore, this interrupt should be connected to System On circuit.*/
#define SDIO_BT_BIT_SDIO_CS_RST        ((u16)0x00000001 << 6)  /*!< R/W 0x0  this bit will be triggered to inform CPU when sdio receiving CMD52 reset*/
#define SDIO_BT_BIT_SDIO_CS_RDY        ((u16)0x00000001 << 5)  /*!< R/W 0x0  this bit will be triggered to inform CPU when sdio changing from standby state to command state(dat bus free)*/
#define SDIO_BT_BIT_C2H_DMA_OK         ((u16)0x00000001 << 3)  /*!< R/W 0x0  Trigger by SDIO to CPU that packet is sent from RXFIFO to Host*/
#define SDIO_BT_BIT_H2C_DMA_OK         ((u16)0x00000001 << 2)  /*!< R/W 0x0  Trigger by SDIO to CPU that packet is sent from Host to TXFIFO*/
#define SDIO_BT_BIT_H2C_BUS_RES_FAIL   ((u16)0x00000001 << 1)  /*!< R/W 0x0  If SDIO is going to move packet to TX FIFO and fails to get bus resource, this interrupt raise. Driver Write 1 clear*/
#define SDIO_BT_BIT_TXBD_H2C_OVF       ((u16)0x00000001 << 0)  /*!< R/W 0x0  If there is not enough TX_BD for TX Packet, this bit raise. Driver Write 1 clear*/
/** @} */

/** @defgroup SPDIO_CCPWM
  * @brief
  * @{
  */
#define SDIO_BT_BIT_SPDIO_CCPWM_TG ((u8)0x00000001 << 7)           /*!< R/W 0x0  Toggling Bit: this is the one bit sequence number field. Interrupt is issued when this bit is changed from " to " or from " to ".*/
#define SDIO_BT_MASK_SPDIO_CCPWM   ((u8)0x0000007F << 0)           /*!< R/W 0x0  ccpwm*/
#define SDIO_BT_SPDIO_CCPWM(x)     (((u8)((x) & 0x0000007F) << 0))
#define SDIO_BT_GET_SPDIO_CCPWM(x) ((u8)(((x >> 0) & 0x0000007F)))
/** @} */

/** @defgroup SPDIO_SYS_CPU_INDICATION
  * @brief
  * @{
  */
#define SDIO_BT_BIT_SYSTEM_CPU_RDY_IND ((u24)0x00000001 << 0) /*!< R/W 0x0  Used to indicate that CPU is ready for TRX. Default: 0 This bit is synchronized to CPU_RDY_IND(Offset: 0x87)*/
/** @} */

/** @defgroup SPDIO_AHB_DMA_CTRL
  * @brief
  * @{
  */
#define SDIO_BT_BIT_DISPATCH_TXAGG_PKT      ((u32)0x00000001 << 31)           /*!< R/W 0x0  Enable SPDIO to dispatch the Aggregated TX packet.*/
#define SDIO_BT_MASK_AHB_BURST_TYPE         ((u32)0x00000007 << 28)           /*!< R/W 3h'7  This field is used to control to SPDIO AHB support Burst type.  b100: Support Burst 16 DW b010: Support Burst 8 DW b001: Support Burst 4 DW Default is b111, all support.*/
#define SDIO_BT_AHB_BURST_TYPE(x)           (((u32)((x) & 0x00000007) << 28))
#define SDIO_BT_GET_AHB_BURST_TYPE(x)       ((u32)(((x >> 28) & 0x00000007)))
#define SDIO_BT_MASK_RX_AHB_BUSY_WAIT_CNT   ((u32)0x0000000F << 24)           /*!< R/W 4h'4  When SPDIO RX transfer, AHB controller will wait BUSY counter for AHB access target not response READY signal. If timeout, AHB controller will issue AHB_BUS_RES_FAIL INT to CPU.*/
#define SDIO_BT_RX_AHB_BUSY_WAIT_CNT(x)     (((u32)((x) & 0x0000000F) << 24))
#define SDIO_BT_GET_RX_AHB_BUSY_WAIT_CNT(x) ((u32)(((x >> 24) & 0x0000000F)))
#define SDIO_BT_MASK_AHBM_SPDIO_TRANS       ((u32)0x00000003 << 22)           /*!< R 0x0  SPDIO AHB Master HTRANS signal*/
#define SDIO_BT_AHBM_SPDIO_TRANS(x)         (((u32)((x) & 0x00000003) << 22))
#define SDIO_BT_GET_AHBM_SPDIO_TRANS(x)     ((u32)(((x >> 22) & 0x00000003)))
#define SDIO_BT_BIT_AHBM_SPDIO_READY        ((u32)0x00000001 << 21)           /*!< R 0x1  SPDIO AHB Master Hready signal*/
#define SDIO_BT_MASK_AHB_DMA_CS             ((u32)0x0000000F << 16)           /*!< R 0x0  AHB DMA state*/
#define SDIO_BT_AHB_DMA_CS(x)               (((u32)((x) & 0x0000000F) << 16))
#define SDIO_BT_GET_AHB_DMA_CS(x)           ((u32)(((x >> 16) & 0x0000000F)))
#define SDIO_BT_MASK_SPDIO_RXFF_WLEVEL      ((u32)0x0000007F << 8)            /*!< R/W 7h'40  SPDIO FIFO RX water level Range 1~126, it can be modified only when AHB_DMA_CS = 000*/
#define SDIO_BT_SPDIO_RXFF_WLEVEL(x)        (((u32)((x) & 0x0000007F) << 8))
#define SDIO_BT_GET_SPDIO_RXFF_WLEVEL(x)    ((u32)(((x >> 8) & 0x0000007F)))
#define SDIO_BT_MASK_SPDIO_TXFF_WLEVEL      ((u32)0x0000007F << 0)            /*!< R/W 7h'40  SPDIO FIFO TX water level Range 1~126, it can be modified only when AHB_DMA_CS = 000*/
#define SDIO_BT_SPDIO_TXFF_WLEVEL(x)        (((u32)((x) & 0x0000007F) << 0))
#define SDIO_BT_GET_SPDIO_TXFF_WLEVEL(x)    ((u32)(((x >> 0) & 0x0000007F)))
/** @} */

/** @defgroup SPDIO_FREE_RXBD_COUNT
  * @brief
  * @{
  */
#define SDIO_BT_MASK_FREE_RXBD_COUNT     ((u32)0x0000FFFF << 16)           /*!< R/W 0x1  If SPDIO_RXBD_C2H_RPTR is updated and the free RXBD Number is larger than FREE_RXBD_COUNT, trigger RX_BD_AVAI_INT interrupt.*/
#define SDIO_BT_FREE_RXBD_COUNT(x)       (((u32)((x) & 0x0000FFFF) << 16))
#define SDIO_BT_GET_FREE_RXBD_COUNT(x)   ((u32)(((x >> 16) & 0x0000FFFF)))
#define SDIO_BT_MASK_TX_BUFF_UNIT_SIZE   ((u32)0x000000FF << 8)            /*!< R/W 0x1  The Size of each single TX Buffer which is addressed by TX_BD Unit: 64Byte Ex: 0x01=>64Byte     0x10=>1024Byte*/
#define SDIO_BT_TX_BUFF_UNIT_SIZE(x)     (((u32)((x) & 0x000000FF) << 8))
#define SDIO_BT_GET_TX_BUFF_UNIT_SIZE(x) ((u32)(((x >> 8) & 0x000000FF)))
#define SDIO_BT_MASK_FIFO_CNT            ((u32)0x0000007F << 0)            /*!< R 0x0  The SPDIO Local FIFO counter, for debug usage*/
#define SDIO_BT_FIFO_CNT(x)              (((u32)((x) & 0x0000007F) << 0))
#define SDIO_BT_GET_FIFO_CNT(x)          ((u32)(((x >> 0) & 0x0000007F)))
/** @} */


/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your definition here

/* MANUAL_GEN_END */

/** @} */

/** @} */

#endif