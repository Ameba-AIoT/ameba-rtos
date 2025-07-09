/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBAGREEN2_SDIO_WIFI_H_
#define _AMEBAGREEN2_SDIO_WIFI_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup SDIO_WIFI
  * @brief SDIO_WIFI driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup SDIO_WIFI_Register_Definitions SDIO_WIFI Register Definitions
  * @{
  */

/** @defgroup SPDIO_TXBD_ADDR
  * @brief
  * @{
  */
#define SDIO_WIFI_MASK_SPDIO_TXFIFO_ADDR   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  The base address of TXBD The value of this register should be configure by CPU It should be 4 byte alignment (Because TXBD Size is 8 Byte).*/
#define SDIO_WIFI_SPDIO_TXFIFO_ADDR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SDIO_WIFI_GET_SPDIO_TXFIFO_ADDR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SPDIO_TXBD_NUM
  * @brief
  * @{
  */
#define SDIO_WIFI_MASK_SPDIO_TXBD_NUM   ((u32)0x0000FFFF << 0)           /*!< R/W 0x0  The Number of TXBD. Unit: Number The value of this register should be configured by CPU*/
#define SDIO_WIFI_SPDIO_TXBD_NUM(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define SDIO_WIFI_GET_SPDIO_TXBD_NUM(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup SPDIO_TXBD_WPTR
  * @brief
  * @{
  */
#define SDIO_WIFI_MASK_SPDIO_TXBD_H2C_WPTR   ((u32)0x0000FFFF << 0)           /*!< R 0x0  When the packet has been sent from Host to TX Buffer, this index should be updated by SDIO IP.*/
#define SDIO_WIFI_SPDIO_TXBD_H2C_WPTR(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define SDIO_WIFI_GET_SPDIO_TXBD_H2C_WPTR(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup SPDIO_TXBD_RPTR
  * @brief
  * @{
  */
#define SDIO_WIFI_MASK_SPDIO_TXBD_H2C_RPTR   ((u32)0x0000FFFF << 0)           /*!< R/W 0x0  When the packet has been processed by CPU and moved to MAC TX FIFO, this index should be updated by CPU.*/
#define SDIO_WIFI_SPDIO_TXBD_H2C_RPTR(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define SDIO_WIFI_GET_SPDIO_TXBD_H2C_RPTR(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup SPDIO_RXBD_ADDR
  * @brief
  * @{
  */
#define SDIO_WIFI_MASK_SPDIO_RXBD_ADDR   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  The base address of RX BD. The value of this register should be configured by CPU. It should be 8-Byte alignment*/
#define SDIO_WIFI_SPDIO_RXBD_ADDR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SDIO_WIFI_GET_SPDIO_RXBD_ADDR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SPDIO_RXBD_NUM
  * @brief
  * @{
  */
#define SDIO_WIFI_MASK_SPDIO_RXBD_NUM   ((u16)0x0000FFFF << 0)           /*!< R/W 0x0  The total count of RX BD The value of this register should be configured by CPU. The unit is BD count*/
#define SDIO_WIFI_SPDIO_RXBD_NUM(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define SDIO_WIFI_GET_SPDIO_RXBD_NUM(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup SPDIO_RXBD_C2H_WPTR
  * @brief
  * @{
  */
#define SDIO_WIFI_MASK_SPDIO_RXBD_C2H_WPTR   ((u16)0x0000FFFF << 0)           /*!< R/W 0x0  When the packet has been processed by CPU and to be moved to host, this index should be updated by CPU*/
#define SDIO_WIFI_SPDIO_RXBD_C2H_WPTR(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define SDIO_WIFI_GET_SPDIO_RXBD_C2H_WPTR(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup SPDIO_RXBD_C2H_RPTR
  * @brief
  * @{
  */
#define SDIO_WIFI_MASK_SPDIO_RXBD_C2H_RPTR   ((u16)0x0000FFFF << 0)           /*!< R 0x0  When the packet has been moved to host, this index should be updated by SDIO IP.*/
#define SDIO_WIFI_SPDIO_RXBD_C2H_RPTR(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define SDIO_WIFI_GET_SPDIO_RXBD_C2H_RPTR(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup SPDIO_HCI_RX_REQ
  * @brief
  * @{
  */
#define SDIO_WIFI_BIT_HCI_RX_REQ ((u8)0x00000001 << 0) /*!< R/W 0x0  CPU triggers this bit to enable SDIO IP RX transfer by fetch BD info. SDIO fetch BD to get the RX length and address and transfer RX packet to Host, and clear this bit when all BD transfer done.*/
/** @} */

/** @defgroup SPDIO_CPU_RST_SDIO_DMA
  * @brief
  * @{
  */
#define SDIO_WIFI_BIT_CPU_RST_SDIO_DMA ((u8)0x00000001 << 7) /*!< R/W 0x0  CPU set this bit to reset SDIO DMA. This bit is auto clear.*/
#define SDIO_WIFI_BIT_AVAL_PG_INT_OPT  ((u8)0x00000001 << 0) /*!< R/W 0x0  set 1 to disable clearing tx_aval_pg_int by read tx_aval_pg_num*/
/** @} */

/** @defgroup SPDIO_RX_REQ_ADDR
  * @brief
  * @{
  */
#define SDIO_WIFI_MASK_SPDIO_RX_REQ_ADDR   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  The address of the requested RX Packet. This is unnecessary for driver.*/
#define SDIO_WIFI_SPDIO_RX_REQ_ADDR(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SDIO_WIFI_GET_SPDIO_RX_REQ_ADDR(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SPDIO_CPU_INT_MASK
  * @brief
  * @{
  */
#define SDIO_WIFI_BIT_H2C_SUS_REQ_MSK        ((u16)0x00000001 << 12) /*!< R/W 0x0  This bit is clear, and H2C_SUS_REQ is masked*/
#define SDIO_WIFI_BIT_HOST_CMD11_INT_MSK     ((u16)0x00000001 << 11) /*!< R/W 0x0  This bit is clear, and HOST_CMD11_INT is masked*/
#define SDIO_WIFI_BIT_HOST_WAKE_CPU_INT_MSK  ((u16)0x00000001 << 10) /*!< R/W 0x0  This bit is clear, and HOST_WAKE_CPU_INT is masked*/
#define SDIO_WIFI_BIT_RX_BD_AVAI_INT_MSK     ((u16)0x00000001 << 9)  /*!< R/W 0x0  This bit is clear, and RX_BD_AVAI_INT is masked*/
#define SDIO_WIFI_BIT_RX_BD_FLAG_ERR_INT_MSK ((u16)0x00000001 << 8)  /*!< R/W 0x0  This bit is clear, and RX_BD_FLAG_ERR_INT is masked*/
#define SDIO_WIFI_BIT_SDIO_RST_CMD_INT_MSK   ((u16)0x00000001 << 7)  /*!< R/W 0x0  This bit is clear, and SDIO_RST_CMD_INT is masked*/
#define SDIO_WIFI_BIT_RPWM2_INT_MSK          ((u16)0x00000001 << 6)  /*!< R/W 0x0  This bit is clear, and RPWM2_INT is masked*/
#define SDIO_WIFI_BIT_RPWM_INT_MSK           ((u16)0x00000001 << 5)  /*!< R/W 0x0  This bit is clear, and RPWM_INT is masked*/
#define SDIO_WIFI_BIT_H2C_MSG_INT_MSK        ((u16)0x00000001 << 4)  /*!< R/W 0x0  This bit is clear, and H2C_MSG_INT is masked*/
#define SDIO_WIFI_BIT_C2H_DMA_OK_MSK         ((u16)0x00000001 << 3)  /*!< R/W 0x0  This bit is clear, and C2H_DMA_OK _INT is masked*/
#define SDIO_WIFI_BIT_H2C_DMA_OK_MSK         ((u16)0x00000001 << 2)  /*!< R/W 0x0  This bit is clear, and H2C_DMA_OK _INT is masked*/
#define SDIO_WIFI_BIT_H2C_BUS_RES_FAIL_MSK   ((u16)0x00000001 << 1)  /*!< R/W 0x0  This bit is clear, and H2C_BUS_RES_FAIL_INT is masked*/
#define SDIO_WIFI_BIT_TXBD_H2C_OVF__MSK      ((u16)0x00000001 << 0)  /*!< R/W 0x0  This bit is clear, and TXBD_H2C_OVF is masked*/
/** @} */

/** @defgroup SPDIO_CPU_INT_STATUS
  * @brief
  * @{
  */
#define SDIO_WIFI_BIT_H2C_SUS_REQ        ((u16)0x00000001 << 12) /*!< R/W 0x0  Host write sdio local reg to trigger suspend request interrupt*/
#define SDIO_WIFI_BIT_HOST_CMD11_INT     ((u16)0x00000001 << 11) /*!< R/W 0x0  When Host sends CMD11 to notify that the signal voltage level is going to switch to 1.8v, the HW should issue this interrupt to notify the local CPU to do the LDO power voltage level switch.*/
#define SDIO_WIFI_BIT_HOST_WAKE_CPU_INT  ((u16)0x00000001 << 10) /*!< R/W 0x0  When Host Send TRX CMD53 while CPU is not ready (SYSTEM_CPU_RDY_IND=0, or CPU_RDY_IND=0), trigger this interrupt to wake CPU, and then indicate BUSY status to host. CPU should be able to receive this interrupt even CPU clock is gated; therefore, this interrupt should be connected to System On circuit.*/
#define SDIO_WIFI_BIT_RX_BD_AVAI_INT     ((u16)0x00000001 << 9)  /*!< R/W 0x0  If the free RXBD Number become larger than FREE_RXBD_COUNT(0x1D8), trigger this interrupt. This interrupt trigger only once when free RXBD number cross FREE_RXBD_COUNT*/
#define SDIO_WIFI_BIT_RX_BD_FLAG_ERR_INT ((u16)0x00000001 << 8)  /*!< R/W 0x0  Trigger by SDIO to CPU when detect RX_BD error*/
#define SDIO_WIFI_BIT_SDIO_RST_CMD_INT   ((u16)0x00000001 << 7)  /*!< R/W 0x0  Trigger by SDIO to CPU when SDIO is reset. CPU should be able to receive this interrupt even CPU clock is gated; therefore, this interrupt should be connected to System On circuit.*/
#define SDIO_WIFI_BIT_RPWM2_INT          ((u16)0x00000001 << 6)  /*!< R/W 0x0  Trigger by SDIO to CPU that RPWM2 occurs (HRPWM2 is toggled) CPU should be able to receive this interrupt even CPU clock is gated; therefore, this interrupt should be connected to System On circuit.*/
#define SDIO_WIFI_BIT_RPWM1_INT          ((u16)0x00000001 << 5)  /*!< R/W 0x0  Trigger by SDIO to CPU that RPWM1 occurs(HRPWM1 is toggled) CPU should be able to receive this interrupt even CPU clock is gated; therefore, this interrupt should be connected to System On circuit.*/
#define SDIO_WIFI_BIT_H2C_MSG_INT        ((u16)0x00000001 << 4)  /*!< R/W 0x0  Trigger by SDIO to CPU that H2C_MSG_INT occurs(REG_SDIO_H2C_MSG is toggled)*/
#define SDIO_WIFI_BIT_C2H_DMA_OK         ((u16)0x00000001 << 3)  /*!< R/W 0x0  Trigger by SDIO to CPU that packet is sent from RXFIFO to Host*/
#define SDIO_WIFI_BIT_H2C_DMA_OK         ((u16)0x00000001 << 2)  /*!< R/W 0x0  Trigger by SDIO to CPU that packet is sent from Host to TXFIFO*/
#define SDIO_WIFI_BIT_H2C_BUS_RES_FAIL   ((u16)0x00000001 << 1)  /*!< R/W 0x0  If SDIO is going to move packet to TX FIFO and fails to get bus resource, this interrupt raise. Driver Write 1 clear*/
#define SDIO_WIFI_BIT_TXBD_H2C_OVF       ((u16)0x00000001 << 0)  /*!< R/W 0x0  If there is not enough TX_BD for TX Packet, this bit raise. Driver Write 1 clear*/
/** @} */

/** @defgroup SPDIO_CCPWM
  * @brief
  * @{
  */
#define SDIO_WIFI_BIT_SPDIO_CCPWM_PS ((u8)0x00000001 << 7)           /*!< R/WA0 0x0  device write 1 to trigger interrupt to host*/
#define SDIO_WIFI_MASK_SPDIO_CCPWM   ((u8)0x0000007F << 0)           /*!< R/W 0x0  ccpwm*/
#define SDIO_WIFI_SPDIO_CCPWM(x)     (((u8)((x) & 0x0000007F) << 0))
#define SDIO_WIFI_GET_SPDIO_CCPWM(x) ((u8)(((x >> 0) & 0x0000007F)))
/** @} */

/** @defgroup SPDIO_SYS_CPU_INDICATION
  * @brief
  * @{
  */
#define SDIO_WIFI_BIT_FW_DL_DONE         ((u8)0x00000001 << 2) /*!< R 0x0  Host write firmware update done, sync to this bit*/
#define SDIO_WIFI_BIT_READEE_DONE        ((u8)0x00000001 << 1) /*!< R/W 0x0  After cccr reg is configured, write this bit to 1*/
#define SDIO_WIFI_BIT_SYSTEM_CPU_RDY_IND ((u8)0x00000001 << 0) /*!< R/W 0x0  Used to indicate that CPU is ready for TRX. Default: 0 This bit is synchronized to CPU_RDY_IND(Offset: 0x87)*/
/** @} */

/** @defgroup SPDIO_CCPWM2
  * @brief
  * @{
  */
#define SDIO_WIFI_BIT_SPDIO_CCPWM2_PS ((u16)0x00000001 << 15)          /*!< R/WA0 0x0  device write 1 to trigger interrupt to host*/
#define SDIO_WIFI_MASK_SPDIO_CCPWM2   ((u16)0x00007FFF << 0)           /*!< R/W 0x0  ccpwm2*/
#define SDIO_WIFI_SPDIO_CCPWM2(x)     (((u16)((x) & 0x00007FFF) << 0))
#define SDIO_WIFI_GET_SPDIO_CCPWM2(x) ((u16)(((x >> 0) & 0x00007FFF)))
/** @} */

/** @defgroup SPDIO_CPU_H2C_MSG
  * @brief
  * @{
  */
#define SDIO_WIFI_BIT_CPU_H2C_MSG_PS ((u32)0x00000001 << 31)          /*!< R 0x0  host write 1 to trigger interrupt to device*/
#define SDIO_WIFI_MASK_CPU_H2C_MSG   ((u32)0x7FFFFFFF << 0)           /*!< R 0x0  Driver to FW Message*/
#define SDIO_WIFI_CPU_H2C_MSG(x)     (((u32)((x) & 0x7FFFFFFF) << 0))
#define SDIO_WIFI_GET_CPU_H2C_MSG(x) ((u32)(((x >> 0) & 0x7FFFFFFF)))
/** @} */

/** @defgroup SPDIO_CPU_C2H_MSG
  * @brief
  * @{
  */
#define SDIO_WIFI_BIT_CPU_C2H_MSG_PS ((u32)0x00000001 << 31)          /*!< R/WA0 0x0  device write 1 to trigger interrupt to host*/
#define SDIO_WIFI_MASK_CPU_C2H_MSG   ((u32)0x7FFFFFFF << 0)           /*!< R/W 0x0  FW to Driver Message*/
#define SDIO_WIFI_CPU_C2H_MSG(x)     (((u32)((x) & 0x7FFFFFFF) << 0))
#define SDIO_WIFI_GET_CPU_C2H_MSG(x) ((u32)(((x >> 0) & 0x7FFFFFFF)))
/** @} */

/** @defgroup SPDIO_CRPWM
  * @brief
  * @{
  */
#define SDIO_WIFI_BIT_SPDIO_HRPWM_PS ((u16)0x00000001 << 7)           /*!< R 0x0  host write 1 to trigger interrupt to device*/
#define SDIO_WIFI_MASK_SPDIO_HRPWM   ((u16)0x0000007F << 0)           /*!< R 7h'7  hrpwm*/
#define SDIO_WIFI_SPDIO_HRPWM(x)     (((u16)((x) & 0x0000007F) << 0))
#define SDIO_WIFI_GET_SPDIO_HRPWM(x) ((u16)(((x >> 0) & 0x0000007F)))
/** @} */

/** @defgroup SPDIO_CRPWM2
  * @brief
  * @{
  */
#define SDIO_WIFI_BIT_SPDIO_HRPWM2_PS ((u16)0x00000001 << 15)          /*!< R 0x0  host write 1 to trigger interrupt to device*/
#define SDIO_WIFI_MASK_SPDIO_HRPWM2   ((u16)0x00007FFF << 0)           /*!< R 0x0  NO USED*/
#define SDIO_WIFI_SPDIO_HRPWM2(x)     (((u16)((x) & 0x00007FFF) << 0))
#define SDIO_WIFI_GET_SPDIO_HRPWM2(x) ((u16)(((x >> 0) & 0x00007FFF)))
/** @} */

/** @defgroup SPDIO_AHB_DMA_CTRL
  * @brief
  * @{
  */
#define SDIO_WIFI_BIT_DISPATCH_TXAGG_PKT      ((u32)0x00000001 << 31)           /*!< R/W 0x0  Enable SPDIO to dispatch the Aggregated TX packet.*/
#define SDIO_WIFI_MASK_AHB_BURST_TYPE         ((u32)0x00000007 << 28)           /*!< R/W 3h'7  This field is used to control to SPDIO AHB support Burst type.  b100: Support Burst 16 DW b010: Support Burst 8 DW b001: Support Burst 4 DW Default is b111, all support.*/
#define SDIO_WIFI_AHB_BURST_TYPE(x)           (((u32)((x) & 0x00000007) << 28))
#define SDIO_WIFI_GET_AHB_BURST_TYPE(x)       ((u32)(((x >> 28) & 0x00000007)))
#define SDIO_WIFI_MASK_RX_AHB_BUSY_WAIT_CNT   ((u32)0x0000000F << 24)           /*!< R/W 4h'4  When SPDIO RX transfer, AHB controller will wait BUSY counter for AHB access target not response READY signal. If timeout, AHB controller will issue AHB_BUS_RES_FAIL INT to CPU.*/
#define SDIO_WIFI_RX_AHB_BUSY_WAIT_CNT(x)     (((u32)((x) & 0x0000000F) << 24))
#define SDIO_WIFI_GET_RX_AHB_BUSY_WAIT_CNT(x) ((u32)(((x >> 24) & 0x0000000F)))
#define SDIO_WIFI_MASK_AHBM_SPDIO_TRANS       ((u32)0x00000003 << 22)           /*!< R 0x0  SPDIO AHB Master HTRANS signal*/
#define SDIO_WIFI_AHBM_SPDIO_TRANS(x)         (((u32)((x) & 0x00000003) << 22))
#define SDIO_WIFI_GET_AHBM_SPDIO_TRANS(x)     ((u32)(((x >> 22) & 0x00000003)))
#define SDIO_WIFI_BIT_AHBM_SPDIO_READY        ((u32)0x00000001 << 21)           /*!< R 0x1  SPDIO AHB Master Hready signal*/
#define SDIO_WIFI_BIT_BURST_SINGLE            ((u32)0x00000001 << 20)           /*!< R/W 0x0  FORCE AHB Master only to transfer single burst*/
#define SDIO_WIFI_MASK_AHB_DMA_CS             ((u32)0x0000000F << 16)           /*!< R 0x0  AHB DMA state*/
#define SDIO_WIFI_AHB_DMA_CS(x)               (((u32)((x) & 0x0000000F) << 16))
#define SDIO_WIFI_GET_AHB_DMA_CS(x)           ((u32)(((x >> 16) & 0x0000000F)))
#define SDIO_WIFI_MASK_SPDIO_RXFF_WLEVEL      ((u32)0x0000007F << 8)            /*!< R/W 7h'40  SPDIO FIFO RX water level Range 1~126, it can be modified only when AHB_DMA_CS = 000*/
#define SDIO_WIFI_SPDIO_RXFF_WLEVEL(x)        (((u32)((x) & 0x0000007F) << 8))
#define SDIO_WIFI_GET_SPDIO_RXFF_WLEVEL(x)    ((u32)(((x >> 8) & 0x0000007F)))
#define SDIO_WIFI_MASK_SPDIO_TXFF_WLEVEL      ((u32)0x0000007F << 0)            /*!< R/W 7h'40  SPDIO FIFO TX water level Range 1~126, it can be modified only when AHB_DMA_CS = 000*/
#define SDIO_WIFI_SPDIO_TXFF_WLEVEL(x)        (((u32)((x) & 0x0000007F) << 0))
#define SDIO_WIFI_GET_SPDIO_TXFF_WLEVEL(x)    ((u32)(((x >> 0) & 0x0000007F)))
/** @} */

/** @defgroup SPDIO_FREE_RXBD_COUNT
  * @brief
  * @{
  */
#define SDIO_WIFI_MASK_FREE_RXBD_COUNT     ((u32)0x0000FFFF << 16)           /*!< R/W 0x1  If SPDIO_RXBD_C2H_RPTR is updated and the free RXBD Number is larger than FREE_RXBD_COUNT, trigger RX_BD_AVAI_INT interrupt.*/
#define SDIO_WIFI_FREE_RXBD_COUNT(x)       (((u32)((x) & 0x0000FFFF) << 16))
#define SDIO_WIFI_GET_FREE_RXBD_COUNT(x)   ((u32)(((x >> 16) & 0x0000FFFF)))
#define SDIO_WIFI_MASK_TX_BUFF_UNIT_SIZE   ((u32)0x000000FF << 8)            /*!< R/W 0x1  The Size of each single TX Buffer which is addressed by TX_BD Unit: 64Byte Ex: 0x01=>64Byte     0x10=>1024Byte*/
#define SDIO_WIFI_TX_BUFF_UNIT_SIZE(x)     (((u32)((x) & 0x000000FF) << 8))
#define SDIO_WIFI_GET_TX_BUFF_UNIT_SIZE(x) ((u32)(((x >> 8) & 0x000000FF)))
#define SDIO_WIFI_MASK_FIFO_CNT            ((u32)0x0000007F << 0)            /*!< R 0x0  The SPDIO Local FIFO counter, for debug usage*/
#define SDIO_WIFI_FIFO_CNT(x)              (((u32)((x) & 0x0000007F) << 0))
#define SDIO_WIFI_GET_FIFO_CNT(x)          ((u32)(((x >> 0) & 0x0000007F)))
/** @} */

/** @defgroup SPDIO_CPU_H2C_MSG_EXT
  * @brief
  * @{
  */
#define SDIO_WIFI_MASK_CPU_H2C_MSG_EXT   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  Driver to FW Message*/
#define SDIO_WIFI_CPU_H2C_MSG_EXT(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SDIO_WIFI_GET_CPU_H2C_MSG_EXT(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SPDIO_CPU_C2H_MSG_EXT
  * @brief
  * @{
  */
#define SDIO_WIFI_MASK_CPU_C2H_MSG_EXT   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  FW to driver Message*/
#define SDIO_WIFI_CPU_C2H_MSG_EXT(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define SDIO_WIFI_GET_CPU_C2H_MSG_EXT(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup SDIO_WIFI_Exported_Types SDIO_WIFI Exported Types
  * @{
  */

/** @brief SDIO_WIFI Register Declaration
  */

typedef struct {
	__IO uint32_t RSVD0[40]               ;  /*!< Reserved,  Address offset:0x000-0x09F */
	__IO uint32_t SPDIO_TXBD_ADDR         ;  /*!< Register,  Address offset:0x0A0 */
	__IO uint32_t SPDIO_TXBD_NUM          ;  /*!< Register,  Address offset:0x0A4 */
	__I  uint32_t SPDIO_TXBD_WPTR         ;  /*!< Register,  Address offset:0x0A8 */
	__IO uint32_t SPDIO_TXBD_RPTR         ;  /*!< Register,  Address offset:0x0AC */
	__IO uint32_t SPDIO_RXBD_ADDR         ;  /*!< Register,  Address offset:0x0B0 */
	__IO uint16_t SPDIO_RXBD_NUM          ;  /*!< Register,  Address offset:0x0B4 */
	__IO uint16_t SPDIO_RXBD_C2H_WPTR     ;  /*!< Register,  Address offset:0x0B6 */
	__I  uint16_t SPDIO_RXBD_C2H_RPTR     ;  /*!< Register,  Address offset:0x0B8 */
	__IO uint8_t SPDIO_HCI_RX_REQ        ;  /*!< Register,  Address offset:0x0BA */
	__IO uint8_t SPDIO_CPU_RST_SDIO_DMA  ;  /*!< Register,  Address offset:0x0BB */
	__I  uint32_t SPDIO_RX_REQ_ADDR       ;  /*!< Register,  Address offset:0x0BC */
	__IO uint16_t SPDIO_CPU_INT_MASK      ;  /*!< Register,  Address offset:0x0C0 */
	__IO uint16_t SPDIO_CPU_INT_STATUS    ;  /*!< Register,  Address offset:0x0C2 */
	__IO uint8_t SPDIO_CCPWM             ;  /*!< Register,  Address offset:0x0C4 */
	__IO uint8_t SPDIO_SYS_CPU_INDICATION;  /*!< Register,  Address offset:0x0C5 */
	__IO uint16_t SPDIO_CCPWM2            ;  /*!< Register,  Address offset:0x0C6 */
	__I  uint32_t SPDIO_CPU_H2C_MSG       ;  /*!< Register,  Address offset:0x0C8 */
	__IO uint32_t SPDIO_CPU_C2H_MSG       ;  /*!< Register,  Address offset:0x0CC */
	__I  uint16_t SPDIO_CRPWM             ;  /*!< Register,  Address offset:0x0D0 */
	__I  uint16_t SPDIO_CRPWM2            ;  /*!< Register,  Address offset:0x0D2 */
	__IO uint32_t SPDIO_AHB_DMA_CTRL      ;  /*!< Register,  Address offset:0x0D4 */
	__IO uint32_t SPDIO_FREE_RXBD_COUNT   ;  /*!< Register,  Address offset:0x0D8 */
	__I  uint32_t SPDIO_CPU_H2C_MSG_EXT   ;  /*!< Register,  Address offset:0x0DC */
	__IO uint32_t SPDIO_CPU_C2H_MSG_EXT   ;  /*!< Register,  Address offset:0x0E0 */
} SDIO_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your definition here

/** @addtogroup SDIO_WIFI_Exported_Types
  * @{
  */

/**
  * @brief SDIO_WIFI Init Structure Definition
  */
typedef struct {
	/* TXBD */
	u32 TXBD_BAR;			/*!< Specifies TXBD base address */
	u32 TXBD_RING_SIZE; 	/*!< Specifies TXBD ring size, This parameter must be set to a value in the 0-0xFFFF range. */
	u32 TX_BUFFER_SIZE; 	/*!< Specifies TX buffer size, This parameter must be set to a value in the 0-0xFF range. */

	/* RXBD */
	u32 RXBD_BAR;			/*!< Specifies RXBD base address */
	u32 RXBD_RING_SIZE; 	/*!< Specifies RXBD ring size, This parameter must be set to a value in the 0-0xFFFF range. */
	u32 RXBD_FREE_TH;		/*!< the threshold of free RX BD count to trigger interrupt */

	u32 TX_AGG_DISPATCH;
} SDIO_InitTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup SDIO_WIFI_Exported_Constants SDIO_WIFI Exported Constants
  * @{
  */

/* SPDIO_CRPWM2 bit definition */
#define RPWM2_ACT_BIT					(BIT0)	// Active
#define RPWM2_CG_BIT					(BIT1)	// Clock Gated
#define RPWM2_TOGGLE_BIT				(BIT15)	// Toggle bit

#define SPDIO_CRPWM2_NEED_CHECKSUM		(BIT2) // fwdl need checksum
#define SPDIO_CRPWM2_BOOT_RAM			(BIT3) // boot from ram after fwdl

/* SPDIO_CCPWM2 bit definition */
#define CPWM2_ACT_BIT					(BIT0)	// Active
#define CPWM2_DSTANDBY_BIT				(BIT1)	// Deep Standby
#define CPWM2_FBOOT_BIT					(BIT2)	// fast reboot
#define CPWM2_INIC_FW_RDY_BIT			(BIT3)	// is the iNIC FW(1) or Boot FW(0)
#define CPWM2_TOGGLE_BIT				(BIT15)	// Toggle bit

#define SPDIO_CCPWM2_SEND_CHECKSUM		(BIT14)  // checksum is ready to be sent

/** @} */
/** @} */
/** @} */

/* SDIO_WIFI Exported functions --------------------------------------------------------*/
void SDIO_StructInit(SDIO_InitTypeDef *SDIO_InitStruct);
void SDIO_Init(SDIO_TypeDef *SDIO, SDIO_InitTypeDef *SDIOInit_Struct);
void SDIO_INTClear(SDIO_TypeDef *SDIO, u16 SDIO_INT);
void SDIO_INTClearAll(SDIO_TypeDef *SDIO);
void SDIO_INTConfig(SDIO_TypeDef *SDIO, u16 IntMask, u32 NewState);
u16 SDIO_INTStatus(SDIO_TypeDef *SDIO);
u8 SDIO_RPWM1_Get(SDIO_TypeDef *SDIO);
u16 SDIO_RPWM2_Get(SDIO_TypeDef *SDIO);
void SDIO_CPWM1_Set(SDIO_TypeDef *SDIO, u8 Val, u32 Newstate);
void SDIO_CPWM2_Set(SDIO_TypeDef *SDIO, u16 Val, u32 Newstate);
u32 SDIO_H2C_MSG_Get(SDIO_TypeDef *SDIO);
u32 SDIO_H2C_MSG_EXT_Get(SDIO_TypeDef *SDIO);
void SDIO_C2H_MSG_Set(SDIO_TypeDef *SDIO, u32 Val);
void SDIO_C2H_MSG_EXT_Set(SDIO_TypeDef *SDIO, u32 Val);
void SDIO_Tx_AGG_Set(SDIO_TypeDef *SDIO, u32 Newstate);
u16 SDIO_RXBD_RPTR_Get(SDIO_TypeDef *SDIO);
void SDIO_RXBD_WPTR_Set(SDIO_TypeDef *SDIO, u16 Val);
u16 SDIO_TXBD_WPTR_Get(SDIO_TypeDef *SDIO);
void SDIO_TXBD_RPTR_Set(SDIO_TypeDef *SDIO, u16 Val);
u16 SDIO_TXBD_RPTR_Get(SDIO_TypeDef *SDIO);
void SDIO_DMA_Reset(SDIO_TypeDef *SDIO);
u32 SDIO_DMA_CTRL_Get(SDIO_TypeDef *SDIO);
void SDIO_SetReady(SDIO_TypeDef *SDIO, int ready);
void SDIO_RxReq(SDIO_TypeDef *SDIO);

/* MANUAL_GEN_END */

#endif