/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_A2C_H_
#define _AMEBA_A2C_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup A2C
  * @brief A2C driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup A2C_Register_Definitions A2C Register Definitions
  * @{
  */

/** @defgroup A2C_CTL
  * @brief A2C bus control register
  * @{
  */
#define A2C_BIT_FD_SSP_AUTO   ((u32)0x00000001 << 9) /*!< R/W 0x0  automatic calculat ssp delay, ssp = second sample point*/
#define A2C_BIT_FD_CRC_MODE   ((u32)0x00000001 << 8) /*!< R/W 0x0  crc mode select * 0: ISO crc * 1: non-ISO crc*/
#define A2C_BIT_RXDMA_EN      ((u32)0x00000001 << 7) /*!< R/W 0x0  rx dma enable*/
#define A2C_BIT_FD_EN         ((u32)0x00000001 << 6) /*!< R/W 0x0  enable A2C FD protocol*/
#define A2C_BIT_RX_FIFO_EN    ((u32)0x00000001 << 5) /*!< R/W 0x0  rx fifo function enable*/
#define A2C_BIT_TEST_MODE_EN  ((u32)0x00000001 << 4) /*!< R/W 0x0  test mode enable, for lbk or silent mode use*/
#define A2C_BIT_AUTO_RE_TX_EN ((u32)0x00000001 << 3) /*!< R/W 0x0  auto re-transmission enabled*/
#define A2C_BIT_TRI_SAMPLE    ((u32)0x00000001 << 2) /*!< R/W 0x0  triple sample mode*/
#define A2C_BIT_BUS_ON_REQ    ((u32)0x00000001 << 1) /*!< R/W/ES 0x0  request to bus_on*/
#define A2C_BIT_EN            ((u32)0x00000001 << 0) /*!< R/W 0x0  a2c enable*/
/** @} */

/** @defgroup A2C_STS
  * @brief A2C bus status
  * @{
  */
#define A2C_BIT_BUS_ON_STATE ((u32)0x00000001 << 0) /*!< R 0x0  bus state * 1 means the a2c module is already bus on * 0 means the a2c module is busoff*/
/** @} */

/** @defgroup A2C_FIFO_STS
  * @brief A2C bus FIFO status
  * @{
  */
#define A2C_MASK_FIFO_MSG_LVL     ((u32)0x00000007 << 4)           /*!< R 0x0  rx fifo message level*/
#define A2C_FIFO_MSG_LVL(x)       (((u32)((x) & 0x00000007) << 4))
#define A2C_GET_FIFO_MSG_LVL(x)   ((u32)(((x >> 4) & 0x00000007)))
#define A2C_BIT_FIFO_MSG_OVERFLOW ((u32)0x00000001 << 2)           /*!< R 0x0  rx fifo overflow*/
#define A2C_BIT_FIFO_MSG_EMPTY    ((u32)0x00000001 << 1)           /*!< R 0x1  rx fifo empty*/
#define A2C_BIT_FIFO_MSG_FULL     ((u32)0x00000001 << 0)           /*!< R 0x0  rx fifo full*/
/** @} */

/** @defgroup A2C_BIT_TIMING
  * @brief A2C2.0 bit timing
  * @{
  */
#define A2C_MASK_BRP     ((u32)0x000000FF << 24)           /*!< R/W 0x0  A2C bit timing perscaler = <$bit:a2c_brp>+1*/
#define A2C_BRP(x)       (((u32)((x) & 0x000000FF) << 24))
#define A2C_GET_BRP(x)   ((u32)(((x >> 24) & 0x000000FF)))
#define A2C_MASK_SJW     ((u32)0x00000007 << 16)           /*!< R/W 0x0  SJW length = <$bit:a2c_sjw>*/
#define A2C_SJW(x)       (((u32)((x) & 0x00000007) << 16))
#define A2C_GET_SJW(x)   ((u32)(((x >> 16) & 0x00000007)))
#define A2C_MASK_TSEG2   ((u32)0x000000FF << 8)            /*!< R/W 0x0  TSEG2 length = <$bit:a2c_tseg2+1>*/
#define A2C_TSEG2(x)     (((u32)((x) & 0x000000FF) << 8))
#define A2C_GET_TSEG2(x) ((u32)(((x >> 8) & 0x000000FF)))
#define A2C_MASK_TSEG1   ((u32)0x000000FF << 0)            /*!< R/W 0x0  TSEG1 length = <$bit:a2c_tseg1+1>*/
#define A2C_TSEG1(x)     (((u32)((x) & 0x000000FF) << 0))
#define A2C_GET_TSEG1(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup A2C_FD_BIT_TIMING
  * @brief A2C FD DATA field bit timing
  * @{
  */
#define A2C_MASK_FD_BRP     ((u32)0x000000FF << 24)           /*!< R/W 0x0  A2C bit timing perscaler = <$bit:a2c_fd_brp>+1*/
#define A2C_FD_BRP(x)       (((u32)((x) & 0x000000FF) << 24))
#define A2C_GET_FD_BRP(x)   ((u32)(((x >> 24) & 0x000000FF)))
#define A2C_MASK_FD_SJW     ((u32)0x00000007 << 16)           /*!< R/W 0x0  SJW length = <$bit:a2c_fd_sjw>*/
#define A2C_FD_SJW(x)       (((u32)((x) & 0x00000007) << 16))
#define A2C_GET_FD_SJW(x)   ((u32)(((x >> 16) & 0x00000007)))
#define A2C_MASK_FD_TSEG2   ((u32)0x000000FF << 8)            /*!< R/W 0x0  TSEG2 length = <$bit:a2c_fd_tseg2+1>*/
#define A2C_FD_TSEG2(x)     (((u32)((x) & 0x000000FF) << 8))
#define A2C_GET_FD_TSEG2(x) ((u32)(((x >> 8) & 0x000000FF)))
#define A2C_MASK_FD_TSEG1   ((u32)0x000000FF << 0)            /*!< R/W 0x0  TSEG1 length = <$bit:a2c_fd_tseg1>+1*/
#define A2C_FD_TSEG1(x)     (((u32)((x) & 0x000000FF) << 0))
#define A2C_GET_FD_TSEG1(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup A2C_FD_SSP_CAL
  * @brief A2C FD SSP configuration
  * @{
  */
#define A2C_MASK_FD_SSP_DCO2   ((u32)0x000000FF << 24)           /*!< R/W 0x0  delay compensation offset2*/
#define A2C_FD_SSP_DCO2(x)     (((u32)((x) & 0x000000FF) << 24))
#define A2C_GET_FD_SSP_DCO2(x) ((u32)(((x >> 24) & 0x000000FF)))
#define A2C_MASK_FD_SSP_DCO    ((u32)0x000000FF << 16)           /*!< R/W 0x0  delay compensation offset*/
#define A2C_FD_SSP_DCO(x)      (((u32)((x) & 0x000000FF) << 16))
#define A2C_GET_FD_SSP_DCO(x)  ((u32)(((x >> 16) & 0x000000FF)))
#define A2C_MASK_FD_SSP_MIN    ((u32)0x000000FF << 8)            /*!< R/W 0x0  minimum of ssp from start of one bit, only functional when a2c_fd_ssp_auto set*/
#define A2C_FD_SSP_MIN(x)      (((u32)((x) & 0x000000FF) << 8))
#define A2C_GET_FD_SSP_MIN(x)  ((u32)(((x >> 8) & 0x000000FF)))
#define A2C_MASK_FD_SSP        ((u32)0x000000FF << 0)            /*!< R/W/ES 0x0  ssp position, delay from start of one bit, if automatic calculation is enabled, the field will update after calculation*/
#define A2C_FD_SSP(x)          (((u32)((x) & 0x000000FF) << 0))
#define A2C_GET_FD_SSP(x)      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup A2C_INT_EN
  * @brief A2C interrupt control
  * @{
  */
#define A2C_BIT_RAM_MOVE_DONE_INT_MASK ((u32)0x00000001 << 21) /*!< R/W 0x0  ram move done interrupt mask 1: mask 0: unmask*/
#define A2C_BIT_BUSOFF_INT_MASK        ((u32)0x00000001 << 20) /*!< R/W 0x0  wakeup interrupt mask 1: mask 0: unmask*/
#define A2C_BIT_WAKEUP_INT_MASK        ((u32)0x00000001 << 19) /*!< R/W 0x0  wakeup interrupt mask 1: mask 0: unmask*/
#define A2C_BIT_ERROR_INT_MASK         ((u32)0x00000001 << 18) /*!< R/W 0x0  error interrupt mask 1: mask 0: unmask*/
#define A2C_BIT_RX_INT_MASK            ((u32)0x00000001 << 17) /*!< R/W 0x0  rx interrupt mask 1: mask 0: unmask*/
#define A2C_BIT_TX_INT_MASK            ((u32)0x00000001 << 16) /*!< R/W 0x0  tx interrupt mask 1: mask 0: unmask*/
#define A2C_BIT_RAM_MOVE_DONE_INT_EN   ((u32)0x00000001 << 5)  /*!< R/W 0x0  ram move done interrupt enable*/
#define A2C_BIT_BUSOFF_INT_EN          ((u32)0x00000001 << 4)  /*!< R/W 0x0  wakeup interrupt enbale*/
#define A2C_BIT_WAKEUP_INT_EN          ((u32)0x00000001 << 3)  /*!< R/W 0x0  wakeup interrupt enbale*/
#define A2C_BIT_ERROR_INT_EN           ((u32)0x00000001 << 2)  /*!< R/W 0x0  error interrupt enable*/
#define A2C_BIT_RX_INT_EN              ((u32)0x00000001 << 1)  /*!< R/W 0x0  rx interrupt enable*/
#define A2C_BIT_TX_INT_EN              ((u32)0x00000001 << 0)  /*!< R/W 0x0  tx interrupt enable*/
/** @} */

/** @defgroup A2C_MB_RXINT_EN
  * @brief A2C rx interrupt for message buffers
  * @{
  */
#define A2C_MASK_MB_RXINT_EN   ((u32)0x0000FFFF << 0)           /*!< R/W 0x0  rx interrupt enable for each message buffer*/
#define A2C_MB_RXINT_EN(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define A2C_GET_MB_RXINT_EN(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup A2C_MB_TXINT_EN
  * @brief A2C tx interrupt for message buffers
  * @{
  */
#define A2C_MASK_MB_TXINT_EN   ((u32)0x0000FFFF << 0)           /*!< R/W 0x0  tx interrupt enable for each message buffer*/
#define A2C_MB_TXINT_EN(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define A2C_GET_MB_TXINT_EN(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup A2C_INT_FLAG
  * @brief A2C interrupt flags
  * @{
  */
#define A2C_BIT_RAM_MOVE_DONE_INT_FLAG ((u32)0x00000001 << 5) /*!< RW1C 0x0  data move from register to A2C IP internal RAM finished interupt, move from RAM to register done will NOT generate interrupt, write one to clear*/
#define A2C_BIT_BUSOFF_INT_FLAG        ((u32)0x00000001 << 4) /*!< RW1C 0x0  busoff interrupt flag, write one to clear*/
#define A2C_BIT_WAKEUP_INT_FLAG        ((u32)0x00000001 << 3) /*!< RW1C 0x0  wakeup interrupt flag, write one to clear*/
#define A2C_BIT_ERROR_INT_FLAG         ((u32)0x00000001 << 2) /*!< RW1C 0x0  error interrupt flag, write to clear*/
#define A2C_BIT_RX_INT_FLAG            ((u32)0x00000001 << 1) /*!< RW1C 0x0  rx interrupt flag, write to clear*/
#define A2C_BIT_TX_INT_FLAG            ((u32)0x00000001 << 0) /*!< RW1C 0x0  tx interrupt flag, write to clear*/
/** @} */

/** @defgroup A2C_ERR_STATUS
  * @brief A2C error status
  * @{
  */
#define A2C_BIT_ERROR_RX    ((u32)0x00000001 << 9) /*!< RW1C 0x0  a2c rx error flag*/
#define A2C_BIT_ERROR_TX    ((u32)0x00000001 << 8) /*!< RW1C 0x0  a2c tx error flag*/
#define A2C_BIT_ERROR_ACK   ((u32)0x00000001 << 5) /*!< RW1C 0x0  latest error  is ack error*/
#define A2C_BIT_ERROR_STUFF ((u32)0x00000001 << 4) /*!< RW1C 0x0  latest error  is stuff error*/
#define A2C_BIT_ERROR_CRC   ((u32)0x00000001 << 3) /*!< RW1C 0x0  latest error  is ctc error*/
#define A2C_BIT_ERROR_FORM  ((u32)0x00000001 << 2) /*!< RW1C 0x0  latest error  is form error*/
#define A2C_BIT_ERROR_BIT1  ((u32)0x00000001 << 1) /*!< RW1C 0x0  latest error  is bit1 error, tx=1 but rx=0*/
#define A2C_BIT_ERROR_BIT0  ((u32)0x00000001 << 0) /*!< RW1C 0x0  latest error  is bit0 error, tx=0 but rx=1*/
/** @} */

/** @defgroup A2C_ERR_CNT_CTL
  * @brief A2C bus error counter control
  * @{
  */
#define A2C_MASK_ERROR_WARN_TH   ((u32)0x000001FF << 8)           /*!< R/W 0x60  error counter warning threshold, programable, default 96. NOTES: 0~127 is availaible to both TEC and REC, 128~255 is only available to TEC*/
#define A2C_ERROR_WARN_TH(x)     (((u32)((x) & 0x000001FF) << 8))
#define A2C_GET_ERROR_WARN_TH(x) ((u32)(((x >> 8) & 0x000001FF)))
#define A2C_BIT_RX_ERR_CNT_CLR   ((u32)0x00000001 << 1)           /*!< WA0 0x0  write 1 to clear a2c_rec*/
#define A2C_BIT_TX_ERR_CNT_CLR   ((u32)0x00000001 << 0)           /*!< WA0 0x0  write 1 to clear a2c_tec*/
/** @} */

/** @defgroup A2C_ERR_CNT_STS
  * @brief A2C bus error counter status
  * @{
  */
#define A2C_BIT_ERROR_WARNING ((u32)0x00000001 << 30)           /*!< R 0x0  error counter arrive at warning threshold*/
#define A2C_BIT_ERROR_BUSOFF  ((u32)0x00000001 << 29)           /*!< R 0x0  error counter arrive at bus off level*/
#define A2C_BIT_ERROR_PASSIVE ((u32)0x00000001 << 28)           /*!< R 0x0  error counter arrive at error passive level*/
#define A2C_MASK_REC          ((u32)0x000001FF << 16)           /*!< R 0x0  receive error count*/
#define A2C_REC(x)            (((u32)((x) & 0x000001FF) << 16))
#define A2C_GET_REC(x)        ((u32)(((x >> 16) & 0x000001FF)))
#define A2C_MASK_TEC          ((u32)0x000001FF << 0)            /*!< R 0x0  transmit error count*/
#define A2C_TEC(x)            (((u32)((x) & 0x000001FF) << 0))
#define A2C_GET_TEC(x)        ((u32)(((x >> 0) & 0x000001FF)))
/** @} */

/** @defgroup A2C_TX_ERROR_FLAG
  * @brief A2C tx error flag for each message buffe
  * @{
  */
#define A2C_MASK_TX_ERROR_FLAG   ((u32)0x0000FFFF << 0)           /*!< RW1CB 0x0  indicate which message tx buffer have error detected, cpu a2c write 1 to clear*/
#define A2C_TX_ERROR_FLAG(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define A2C_GET_TX_ERROR_FLAG(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup A2C_TX_DONE
  * @brief A2C tx done for message buffers
  * @{
  */
#define A2C_MASK_TX_DONE   ((u32)0x0000FFFF << 0)           /*!< RW1CB 0x0  tx done for each message buffer*/
#define A2C_TX_DONE(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define A2C_GET_TX_DONE(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup A2C_RX_DONE
  * @brief A2C rx done for message buffers
  * @{
  */
#define A2C_MASK_RX_DONE   ((u32)0x0000FFFF << 0)           /*!< RW1CB 0x0  rx done for each message buffer*/
#define A2C_RX_DONE(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define A2C_GET_RX_DONE(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup A2C_TIME_STAMP
  * @brief Timestamp function
  * @{
  */
#define A2C_BIT_TIME_STAMP_EN     ((u32)0x00000001 << 31)           /*!< R/W 0x0  time stamp function enable and time stamp counter start*/
#define A2C_MASK_TIME_STAMP_DIV   ((u32)0x000000FF << 16)           /*!< R/W 0x0  timer stamp conter  prescalar*/
#define A2C_TIME_STAMP_DIV(x)     (((u32)((x) & 0x000000FF) << 16))
#define A2C_GET_TIME_STAMP_DIV(x) ((u32)(((x >> 16) & 0x000000FF)))
#define A2C_MASK_TIME_STAMP       ((u32)0x0000FFFF << 0)            /*!< R 0x0  time stamp counter value, up count*/
#define A2C_TIME_STAMP(x)         (((u32)((x) & 0x0000FFFF) << 0))
#define A2C_GET_TIME_STAMP(x)     ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup A2C_MB_TRIGGER
  * @brief A2C tx trigger by timer configuration
  * @{
  */
#define A2C_BIT_TX_TRIGGER_EN              ((u32)0x00000001 << 24)           /*!< R/W 0x0  End of message buffer base address in RAM*/
#define A2C_MASK_TX_TRIGGER_CLOSE_OFFSET   ((u32)0x000000FF << 16)           /*!< R/W 0x0  end of trigger time = <$bit:tx_trigger_begin> + <$bit:tx_trigger_close_offset>*/
#define A2C_TX_TRIGGER_CLOSE_OFFSET(x)     (((u32)((x) & 0x000000FF) << 16))
#define A2C_GET_TX_TRIGGER_CLOSE_OFFSET(x) ((u32)(((x >> 16) & 0x000000FF)))
#define A2C_MASK_TX_TRIGGER_BEGIN          ((u32)0x0000FFFF << 0)            /*!< R/W 0x0  start of trigger time*/
#define A2C_TX_TRIGGER_BEGIN(x)            (((u32)((x) & 0x0000FFFF) << 0))
#define A2C_GET_TX_TRIGGER_BEGIN(x)        ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup A2C_RXDMA_CFG
  * @brief RX dma configuration
  * @{
  */
#define A2C_BIT_RXDMA_OWNER         ((u32)0x00000001 << 24)           /*!< R/W 0x0  a2c rxdma owner is : 0:GDMA 1:A2C*/
#define A2C_MASK_RXDMA_SRC_MSIZE    ((u32)0x00000003 << 16)           /*!< R/W 0x1  rx dma burst size: 'b00: dma burst size=1 'b01: dma burst size=4 'b10: dma burst size=8 'b11: dma burst size=16*/
#define A2C_RXDMA_SRC_MSIZE(x)      (((u32)((x) & 0x00000003) << 16))
#define A2C_GET_RXDMA_SRC_MSIZE(x)  ((u32)(((x >> 16) & 0x00000003)))
#define A2C_MASK_RXDMA_FRAME_SIZE   ((u32)0x00003FFF << 0)            /*!< R 0x2  a2c rx dma data frame size = ceil(<rx_dma_data bytes number>/4 ), for example: rx_dma_data is 9~12 bytes, frame size is 3*/
#define A2C_RXDMA_FRAME_SIZE(x)     (((u32)((x) & 0x00003FFF) << 0))
#define A2C_GET_RXDMA_FRAME_SIZE(x) ((u32)(((x >> 0) & 0x00003FFF)))
/** @} */

/** @defgroup A2C_RX_DMA_DATA
  * @brief Register for dma access message buffe
  * @{
  */
#define A2C_MASK_RX_DMA_DATA   ((u32)0xFFFFFFFF << 0)           /*!< RP 0x0  For DMA read received data*/
#define A2C_RX_DMA_DATA(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define A2C_GET_RX_DMA_DATA(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup A2C_SLEEP_MODE
  * @brief sleep mode request and wakeup pin digital filter
  * @{
  */
#define A2C_BIT_SLEEP_STATE           ((u32)0x00000001 << 17)          /*!< R 0x0  a2c is sleep or not 1: is 0: isn't*/
#define A2C_BIT_SLEEP_EN              ((u32)0x00000001 << 16)          /*!< R/W/ES 0x0  a2c sleep enable 1:request a2c to sleep mode 0:wakeup a2c*/
#define A2C_BIT_WAKEPIN_FLT_EN        ((u32)0x00000001 << 8)           /*!< R/W 0x0  wakeup pin digital filter enable*/
#define A2C_MASK_WAKEPIN_FLT_LENGTH   ((u32)0x000000FF << 0)           /*!< R/W 0x1  wakeup pin digital filter length*/
#define A2C_WAKEPIN_FLT_LENGTH(x)     (((u32)((x) & 0x000000FF) << 0))
#define A2C_GET_WAKEPIN_FLT_LENGTH(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup A2C_TEST
  * @brief test configuration
  * @{
  */
#define A2C_MASK_TEST_CFG   ((u32)0x00000003 << 0)           /*!< R/W 0x0  2b'00: silence_mode 2b'01: external loopback mode (enable a2c_tx_so ) 2b'10: internal loopback mode (a2c_tx_so tie 1) 2b'11: reserved*/
#define A2C_TEST_CFG(x)     (((u32)((x) & 0x00000003) << 0))
#define A2C_GET_TEST_CFG(x) ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup A2C_MBx_STS
  * @brief A2C message buffer status
  * @{
  */
#define A2C_BIT_MSGx_RX_VLD  ((u32)0x00000001 << 3) /*!< R 0x0  new message have been received in the message buffer*/
#define A2C_BIT_MSGx_RX_RDY  ((u32)0x00000001 << 2) /*!< R 0x0  the message buffer is ready for receiving a new message*/
#define A2C_BIT_MSGx_TX_DONE ((u32)0x00000001 << 1) /*!< R 0x0  tx message in the message buffer finish sending*/
#define A2C_BIT_MSGx_TX_REQ  ((u32)0x00000001 << 0) /*!< R 0x0  tx message is pending for transmit*/
/** @} */

/** @defgroup A2C_MBx_CTRL
  * @brief A2C message buffer control
  * @{
  */
#define A2C_BIT_MSGx_RXDMA_EN ((u32)0x00000001 << 24)          /*!< R/W/ES 0x0  dma enable when the message buffer used as rx*/
#define A2C_MASK_MSGx_BA      ((u32)0x000001FF << 0)           /*!< R/W 9'd0  message buffer base address in RAM*/
#define A2C_MSGx_BA(x)        (((u32)((x) & 0x000001FF) << 0))
#define A2C_GET_MSGx_BA(x)    ((u32)(((x >> 0) & 0x000001FF)))
/** @} */

/** @defgroup A2C_MB_BA_END
  * @brief
  * @{
  */
#define A2C_MASK_MSG_BA_END   ((u32)0x000003FF << 0)           /*!< R/W 10'd96  End of message buffer base address in RAM*/
#define A2C_MSG_BA_END(x)     (((u32)((x) & 0x000003FF) << 0))
#define A2C_GET_MSG_BA_END(x) ((u32)(((x >> 0) & 0x000003FF)))
/** @} */

/** @defgroup A2C_RAM_FDDATA_x
  * @brief A2C data
  * @{
  */
#define A2C_MASK_RAM_DATA_x_3   ((u32)0x000000FF << 24)           /*!< R/W/ES 0x0  data_byte 4*x+3*/
#define A2C_RAM_DATA_x_3(x)     (((u32)((x) & 0x000000FF) << 24))
#define A2C_GET_RAM_DATA_x_3(x) ((u32)(((x >> 24) & 0x000000FF)))
#define A2C_MASK_RAM_DATA_x_2   ((u32)0x000000FF << 16)           /*!< R/W/ES 0x0  data_byte 4*x+2*/
#define A2C_RAM_DATA_x_2(x)     (((u32)((x) & 0x000000FF) << 16))
#define A2C_GET_RAM_DATA_x_2(x) ((u32)(((x >> 16) & 0x000000FF)))
#define A2C_MASK_RAM_DATA_x_1   ((u32)0x000000FF << 8)            /*!< R/W/ES 0x0  data_byte 4*x+1*/
#define A2C_RAM_DATA_x_1(x)     (((u32)((x) & 0x000000FF) << 8))
#define A2C_GET_RAM_DATA_x_1(x) ((u32)(((x >> 8) & 0x000000FF)))
#define A2C_MASK_RAM_DATA_x_0   ((u32)0x000000FF << 0)            /*!< R/W/ES 0x0  data_byte 4*x+0*/
#define A2C_RAM_DATA_x_0(x)     (((u32)((x) & 0x000000FF) << 0))
#define A2C_GET_RAM_DATA_x_0(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup A2C_RAM_ARB
  * @brief A2C message buffer arb field configuration
  * @{
  */
#define A2C_BIT_RAM_RTR   ((u32)0x00000001 << 30)          /*!< R/W/ES 0x0  a2c frame RTR bit, determine DATA or REMOTE frame*/
#define A2C_BIT_RAM_IDE   ((u32)0x00000001 << 29)          /*!< R/W/ES 0x0  a2c frame IDE bit, determine standard or extend format*/
#define A2C_MASK_RAM_ID   ((u32)0x1FFFFFFF << 0)           /*!< R/W/ES 0x0  a2c frame ID, including high 11 bit standard ID and low 18 bit extend ID*/
#define A2C_RAM_ID(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define A2C_GET_RAM_ID(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup A2C_RAM_MASK
  * @brief A2C message buffer arb field mask configuration
  * @{
  */
#define A2C_BIT_RAM_RTR_MASK   ((u32)0x00000001 << 30)          /*!< R/W/ES 0x0  a2c frame RTR mask, 0 means don't care, 1 means the bit should match*/
#define A2C_BIT_RAM_IDE_MASK   ((u32)0x00000001 << 29)          /*!< R/W/ES 0x0  a2c frame IDE mask, 0 means don't care, 1 means the bit should match*/
#define A2C_MASK_RAM_ID_MASK   ((u32)0x1FFFFFFF << 0)           /*!< R/W/ES 0x0  a2c frame ID mask, 0 means the ID bit in A2C_RAM_ARB don't care, 1 means the bit should match.*/
#define A2C_RAM_ID_MASK(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define A2C_GET_RAM_ID_MASK(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup A2C_RAM_CS
  * @brief A2C message type control
  * @{
  */
#define A2C_MASK_RAM_TIMESTAMP   ((u32)0x0000FFFF << 16)           /*!< R/W/ES 0x0  the received message time stamp*/
#define A2C_RAM_TIMESTAMP(x)     (((u32)((x) & 0x0000FFFF) << 16))
#define A2C_GET_RAM_TIMESTAMP(x) ((u32)(((x >> 16) & 0x0000FFFF)))
#define A2C_BIT_RAM_ESI          ((u32)0x00000001 << 10)           /*!< R/W/ES 0x0  FD frame esi bit, indicate error passive of message sending node, read bit, should always write 0, hardware automatic fill*/
#define A2C_BIT_RAM_BRS          ((u32)0x00000001 << 9)            /*!< R/W/ES 0x0  FD frame brs bit, indicate switch bit timing*/
#define A2C_BIT_RAM_EDL          ((u32)0x00000001 << 8)            /*!< R/W/ES 0x0  FD frame edl bit, indicate it's a FD frame*/
#define A2C_BIT_RAM_AUTOREPLY    ((u32)0x00000001 << 6)            /*!< R/W/ES 0x0  set the message buffer with remote frame auto reply function*/
#define A2C_BIT_RAM_RXTX         ((u32)0x00000001 << 5)            /*!< R/W/ES 0x0  a2c frame RX or TX, 0 is for RX, 1 is for TX*/
#define A2C_BIT_RAM_LOST         ((u32)0x00000001 << 4)            /*!< R/W/ES 0x0  data have lost in the read buffer, means more than one frame have beed received since last read; only avaliable when rx FIFO is disabled*/
#define A2C_MASK_RAM_DLC         ((u32)0x0000000F << 0)            /*!< R/W/ES 0x0  data length*/
#define A2C_RAM_DLC(x)           (((u32)((x) & 0x0000000F) << 0))
#define A2C_GET_RAM_DLC(x)       ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup A2C_RAM_CMD
  * @brief A2C message buffer access control
  * @{
  */
#define A2C_BIT_RAM_START      ((u32)0x00000001 << 31)          /*!< R/W/ES 0x0  start to exchange data with RAM, the bit will clear automatically after the data change is finshed.*/
#define A2C_BIT_RAM_DIR        ((u32)0x00000001 << 30)          /*!< R/W 0x0  0 is for read from RAM to A2C_RAM_* registers, 1 is for write from A2C_RAM_* to RAM*/
#define A2C_BIT_RAM_BUFFER_EN  ((u32)0x00000001 << 29)          /*!< R/W 0x0  enable the a2c_ram_acc_num specified message buffer for rx/tx*/
#define A2C_BIT_RAM_ACC_DATA15 ((u32)0x00000001 << 26)          /*!< R/W 0x0  access A2C_RAM_DATA_15 field, the field will exchange with RAM space*/
#define A2C_BIT_RAM_ACC_DATA14 ((u32)0x00000001 << 25)          /*!< R/W 0x0  access A2C_RAM_DATA_14 field, the field will exchange with RAM space*/
#define A2C_BIT_RAM_ACC_DATA13 ((u32)0x00000001 << 24)          /*!< R/W 0x0  access A2C_RAM_DATA_13 field, the field will exchange with RAM space*/
#define A2C_BIT_RAM_ACC_DATA12 ((u32)0x00000001 << 23)          /*!< R/W 0x0  access A2C_RAM_DATA_12 field, the field will exchange with RAM space*/
#define A2C_BIT_RAM_ACC_DATA11 ((u32)0x00000001 << 22)          /*!< R/W 0x0  access A2C_RAM_DATA_11 field, the field will exchange with RAM space*/
#define A2C_BIT_RAM_ACC_DATA10 ((u32)0x00000001 << 21)          /*!< R/W 0x0  access A2C_RAM_DATA_10 field, the field will exchange with RAM space*/
#define A2C_BIT_RAM_ACC_DATA9  ((u32)0x00000001 << 20)          /*!< R/W 0x0  access A2C_RAM_DATA_9 field, the field will exchange with RAM space*/
#define A2C_BIT_RAM_ACC_DATA8  ((u32)0x00000001 << 19)          /*!< R/W 0x0  access A2C_RAM_DATA_8 field, the field will exchange with RAM space*/
#define A2C_BIT_RAM_ACC_DATA7  ((u32)0x00000001 << 18)          /*!< R/W 0x0  access A2C_RAM_DATA_7 field, the field will exchange with RAM space*/
#define A2C_BIT_RAM_ACC_DATA6  ((u32)0x00000001 << 17)          /*!< R/W 0x0  access A2C_RAM_DATA_6 field, the field will exchange with RAM space*/
#define A2C_BIT_RAM_ACC_DATA5  ((u32)0x00000001 << 16)          /*!< R/W 0x0  access A2C_RAM_DATA_5 field, the field will exchange with RAM space*/
#define A2C_BIT_RAM_ACC_DATA4  ((u32)0x00000001 << 15)          /*!< R/W 0x0  access A2C_RAM_DATA_4 field, the field will exchange with RAM space*/
#define A2C_BIT_RAM_ACC_DATA3  ((u32)0x00000001 << 14)          /*!< R/W 0x0  access A2C_RAM_DATA_3 field, the field will exchange with RAM space*/
#define A2C_BIT_RAM_ACC_DATA2  ((u32)0x00000001 << 13)          /*!< R/W 0x0  access A2C_RAM_DATA_2 field, the field will exchange with RAM space*/
#define A2C_BIT_RAM_ACC_DATA1  ((u32)0x00000001 << 12)          /*!< R/W 0x0  access A2C_RAM_DATA_1 field, the field will exchange with RAM space*/
#define A2C_BIT_RAM_ACC_DATA0  ((u32)0x00000001 << 11)          /*!< R/W 0x0  access A2C_RAM_DATA_0 field, the field will exchange with RAM space*/
#define A2C_BIT_RAM_ACC_ARB    ((u32)0x00000001 << 10)          /*!< R/W 0x0  access A2C_RAM_ARB field, the field will exchange with RAM space*/
#define A2C_BIT_RAM_ACC_CS     ((u32)0x00000001 << 9)           /*!< R/W 0x0  access A2C_RAM_CS field, the field will exchange with RAM space*/
#define A2C_BIT_RAM_ACC_MASK   ((u32)0x00000001 << 8)           /*!< R/W 0x0  access A2C_RAM_MASK field, the field will exchange with RAM space*/
#define A2C_MASK_RAM_ACC_NUM   ((u32)0x000000FF << 0)           /*!< R/W 0x0  access buffer number*/
#define A2C_RAM_ACC_NUM(x)     (((u32)((x) & 0x000000FF) << 0))
#define A2C_GET_RAM_ACC_NUM(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup A2C_DBG_PORT
  * @brief debug register
  * @{
  */
#define A2C_BIT_DBG_EN     ((u32)0x00000001 << 8)           /*!< R/W 0x0  debug port enable*/
#define A2C_MASK_DBG_SEL   ((u32)0x000000FF << 0)           /*!< R/W 0x0  debug port selection*/
#define A2C_DBG_SEL(x)     (((u32)((x) & 0x000000FF) << 0))
#define A2C_GET_DBG_SEL(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup A2C_Exported_Types A2C Exported Types
  * @{
  */

/** @brief A2C Register Declaration
  */

typedef struct {
	__IO uint32_t A2C_CTL             ;  /*!< A2C BUS CONTROL REGISTER,  Address offset:0x000 */
	__I  uint32_t A2C_STS             ;  /*!< A2C BUS STATUS Register,  Address offset:0x004 */
	__I  uint32_t A2C_FIFO_STS        ;  /*!< A2C BUS FIFO STATUS Register,  Address offset:0x008 */
	__IO uint32_t A2C_BIT_TIMING      ;  /*!< A2C2.0 BIT TIMING Register,  Address offset:0x00C */
	__IO uint32_t A2C_FD_BIT_TIMING   ;  /*!< A2C FD DATA FIELD BIT TIMING Register,  Address offset:0x010 */
	__IO uint32_t A2C_FD_SSP_CAL      ;  /*!< A2C FD SSP CONFIGURATION Register,  Address offset:0x014 */
	__IO uint32_t A2C_INT_EN          ;  /*!< A2C INTERRUPT CONTROL Register,  Address offset:0x018 */
	__IO uint32_t A2C_MB_RXINT_EN     ;  /*!< A2C RX INTERRUPT FOR MESSAGE BUFFERS Register,  Address offset:0x01C */
	__IO uint32_t A2C_MB_TXINT_EN     ;  /*!< A2C TX INTERRUPT FOR MESSAGE BUFFERS Register,  Address offset:0x020 */
	__IO uint32_t A2C_INT_FLAG        ;  /*!< A2C INTERRUPT FLAGS Register,  Address offset:0x024 */
	__IO uint32_t A2C_ERR_STATUS      ;  /*!< A2C ERROR STATUS Register,  Address offset:0x028 */
	__IO uint32_t A2C_ERR_CNT_CTL     ;  /*!< A2C BUS ERROR COUNTER CONTROL Register,  Address offset:0x02C */
	__I  uint32_t A2C_ERR_CNT_STS     ;  /*!< A2C BUS ERROR COUNTER STATUS Register,  Address offset:0x030 */
	__IO uint32_t A2C_TX_ERROR_FLAG   ;  /*!< A2C TX ERROR FLAG FOR EACH MESSAGE BUFFE Register,  Address offset:0x034 */
	__IO uint32_t A2C_TX_DONE         ;  /*!< A2C TX DONE FOR MESSAGE BUFFERS Register,  Address offset:0x038 */
	__IO uint32_t A2C_RX_DONE         ;  /*!< A2C RX DONE FOR MESSAGE BUFFERS Register,  Address offset:0x03C */
	__IO uint32_t A2C_TIME_STAMP      ;  /*!< TIMESTAMP FUNCTION Register,  Address offset:0x040 */
	__IO uint32_t A2C_MB_TRIGGER      ;  /*!< A2C TX TRIGGER BY TIMER CONFIGURATION Register,  Address offset:0x044 */
	__IO uint32_t A2C_RXDMA_CFG       ;  /*!< RX DMA CONFIGURATION Register,  Address offset:0x048 */
	__I  uint32_t A2C_RX_DMA_DATA     ;  /*!< REGISTER FOR DMA ACCESS MESSAGE BUFFE Register,  Address offset:0x04C */
	__IO uint32_t A2C_SLEEP_MODE      ;  /*!< SLEEP MODE REQUEST AND WAKEUP PIN DIGITAL FILTER Register,  Address offset:0x050 */
	__IO uint32_t A2C_TEST            ;  /*!< TEST CONFIGURATION Register,  Address offset:0x054 */
	__IO uint32_t RSVD0[42]           ;  /*!< Reserved,  Address offset:0x058-0x0FF */
	__I  uint32_t A2C_MBx_STS[16]     ;  /*!< A2C MESSAGE BUFFER STATUS Register,  Address offset:0x100-0x13C */
	__IO uint32_t RSVD1[48]           ;  /*!< Reserved,  Address offset:0x140-0x1FF */
	__IO uint32_t A2C_MBx_CTRL[16]    ;  /*!< A2C MESSAGE BUFFER CONTROL Register,  Address offset:0x200-0x23C */
	__IO uint32_t RSVD2[44]           ;  /*!< Reserved,  Address offset:0x240-0x2EF */
	__IO uint32_t A2C_MB_BA_END       ;  /*!< Register,  Address offset:0x2F0 */
	__IO uint32_t RSVD3[3]            ;  /*!< Reserved,  Address offset:0x2F4-0x2FF */
	__IO uint32_t A2C_RAM_FDDATA_x[16];  /*!< A2C DATA Register,  Address offset:0x300-0x33C */
	__IO uint32_t A2C_RAM_ARB         ;  /*!< A2C MESSAGE BUFFER ARB FIELD CONFIGURATION Register,  Address offset:0x340 */
	__IO uint32_t A2C_RAM_MASK        ;  /*!< A2C MESSAGE BUFFER ARB FIELD MASK CONFIGURATION Register,  Address offset:0x344 */
	__IO uint32_t A2C_RAM_CS          ;  /*!< A2C MESSAGE TYPE CONTROL Register,  Address offset:0x348 */
	__IO uint32_t A2C_RAM_CMD         ;  /*!< A2C MESSAGE BUFFER ACCESS CONTROL Register,  Address offset:0x34C */
	__IO uint32_t RSVD4[42]           ;  /*!< Reserved,  Address offset:0x350-0x3F7 */
	__IO uint32_t A2C_DBG_PORT        ;  /*!< DEBUG REGISTER,  Address offset:0x3F8 */
	__IO uint32_t A2C_DUMMY           ;  /*!< DUMMY REGISTER,  Address offset:0x3FC */
} A2C_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your definition here
/* Exported types --------------------------------------------------------*/
/** @addtogroup A2C_Exported_Types A2C Exported Types
  * @{
  */
/**
  * @brief  A2C Mode Initialization structure definition
  */
typedef struct {
	u32	A2C_TimStampEn; 		/*!< Specifies the A2C stamp timer enable or disable.
                                      				This parameter can be ENABLE or DISABLE. */

	u32	A2C_TimStampDiv;		/*!< Specifies the A2C stamp timer divider.
                                      					This parameter can be a number between 0x01 and 0xff. */

	u32	A2C_AutoReTxEn;        		/*!< Specifies the message auto re-transmission configuration.
                                      				This parameter can be a ENABLE or DISABLE. */

	u32	A2C_TriSampleEn;			/*!< Specifies the tripple sampling configuration.
                                      				This parameter can be ENABLE or DISABLE. */

	u32	A2C_WorkMode;        		/*!< Specifies the A2C work mode.
                                      				This parameter can be of @ref A2C_WORK_MODE_define. */

	u32	A2C_FDCrcMode;        		/*!< Specifies the A2C FD CRC mode.
                                      				This parameter can be of @ref A2C_FD_CRC_MODE_define. */

	u32	A2C_ErrCntThreshold;        	/*!< Specifies the message length.
                                      				This parameter can be a number between 0x000 and 0x1ff*/

	u32	A2C_RxFifoEn;      			/*!< Specifies the A2C FIFO mode configure register.
                                      				This parameter can be ENABLE or DISABLE. */

	u32	A2C_BitPrescaler;			/*!< Specifies the bit timing prescaler.
                                      				This parameter can be a number between 0x1 and 0x100*/

	u32	A2C_SJW;				/*!< Specifies the A2C bit timing SJW.
                                      				This parameter can be a number between 0x1 and 0x8. */

	u32	A2C_TSEG1;				/*!< Specifies the A2C TSEG1.
                                      				This parameter can be a number between 0x1 and 0x10. */

	u32	A2C_TSEG2;				/*!< Specifies the A2C TSEG2.
                                      				This parameter can be a number between 0x1 and 0x10. */
} A2C_InitTypeDef;

/**
  * @}
  */

/**
  * @}
  */

/**
  * @brief  A2C Tx message descriptor structure definition
  */
typedef struct {

	u32 ProtocolType;	/*!< Specifies the protocol type of frame for the message that will
                        	be transmitted. This parameter can be a value of
                        	@ref A2C_PROTOCOL_TYPE_define */

	u32 StdId;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF. */

	u32 ExtId;  /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF. */

	u32 MsgBufferIdx;     /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0xf. */

	u32 IDE;     /*!< Specifies the type of identifier for the message that
                        will be transmitted. This parameter can be a value
                        of @ref A2C_ID_TYPE_define */

	u32 RTR;     /*!< Specifies the type of frame for the message that will
                        be transmitted. This parameter can be a value of
                        @ref A2C_FRAME_TYPE_define */

	u32 DLC;     /*!< Specifies the length of the frame that will be
                        transmitted. This parameter can be a value between
                        0 to 8 */
	union {
		/** Payload data accessed as unsigned 8 bit values. */
		uint8_t Data[64];
		/** Payload data accessed as unsigned 32 bit values. */
		uint32_t Data_32[16];
	};
} A2C_TxMsgTypeDef;

/**
  * @}
  */

/**
  * @brief  A2C Rx message descriptor structure definition
  */
typedef struct {
	u32 ProtocolType;	/*!< Specifies the protocol type of frame for the message that will
                        	be transmitted. This parameter can be a value of
                        	@ref A2C_PROTOCOL_TYPE_define */

	u32 StdId;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF, ####*/

	u32 ExtId;  /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF, #### */

	u32 MsgBufferIdx;     /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0xf, ####*/

	u32 IDE;     /*!< Specifies the type of identifier for the message that
                        will be transmitted. This parameter can be a value
                        of @ref A2C_ID_TYPE_define , ####*/

	u32 RTR;     /*!< Specifies the type of frame for the message that will
                        be transmitted. This parameter can be a value of
                        @ref A2C_FRAME_TYPE_define, ####*/

	u32 DLC;     /*!< Specifies the length of the frame that will be
                        transmitted. This parameter can be a value between
                        0 to 8*/
	union {
		/** Payload data accessed as unsigned 8 bit values. */
		uint8_t Data[64];
		/** Payload data accessed as unsigned 32 bit values. */
		uint32_t Data_32[16];
	};

	u32 ID_MASK;     /*!< Specifies the ID MASK*/

	u32 RTR_Mask;     /*!< Specifies the RTR MASK, refer to A2C_RTR_MASK_TYPE_define*/

	u32 IDE_Mask;     /*!< Specifies the IDE MASK, refer to A2C_IDE_MASK_TYPE_define*/

	u32 RxTimStamp;      /*!< Specifies the rx time stamp */

	u32 RxLost;		     /*!< Specifies the rx lost times */
} A2C_RxMsgTypeDef;
/**
  * @}
  */


/* Exported constants --------------------------------------------------------*/
/** @defgroup A2C_Exported_Constants A2C Exported Constants
  * @{
  */

/** @defgroup A2C_WORK_MODE_define
  * @{
  */
#define A2C_NORMAL_MODE					          ((u32)0x00000000)
#define A2C_SILENCE_MODE					        ((u32)0x00000000)
#define A2C_EXT_LOOPBACK_MODE					    ((u32)0x00000001)
#define A2C_INT_LOOPBACK_MODE             ((u32)0x00000002)
#define IS_A2C_WORK_MODE(MODE) (((MODE) == A2C_NORMAL_MODE) || \
									((MODE) == A2C_SILENCE_MODE) || \
									((MODE) == A2C_EXT_LOOPBACK_MODE) || \
									((MODE) == A2C_INT_LOOPBACK_MODE))

/**
  * @}
  */

/** @defgroup A2C_PROTOCOL_TYPE_define
  * @{
  */
#define A2C_CAN20_PROTOCOL_FRAME			((u32)0x00000000)
#define A2C_FD_PROTOCOL_FRAME				((u32)0x00000001)
#define IS_A2C_PROTOCOL_TYPE(TYPE) (((TYPE) == A2C_CAN20_PROTOCOL_FRAME) || \
									((TYPE) == A2C_FD_PROTOCOL_FRAME))

/**
  * @}
  */

/** @defgroup A2C_ID_TYPE_define
  * @{
  */
#define A2C_STANDARD_FRAME				((u32)0x00000000)
#define A2C_EXTEND_FRAME					((u32)0x20000000)
#define IS_A2C_ID_TYPE(TYPE) (((TYPE) == A2C_STANDARD_FRAME) || \
									((TYPE) == A2C_EXTEND_FRAME))

/**
  * @}
  */

/** @defgroup A2C_FD_CRC_MODE_define
  * @{
  */
#define A2C_ISO_CRC						((u32)0x00000000)
#define A2C_NON_ISO_CRC					((u32)0x00000100)
#define IS_A2C_CRC_TYPE(TYPE) (((TYPE) == A2C_ISO_CRC) || \
									((TYPE) == A2C_NON_ISO_CRC))

/**
  * @}
  */

/** @defgroup A2C_FD_BIT_TIMING_SWITCH_TYPE_define
  * @{
  */
#define A2C_FD_SWITCH_BIT_TIMING			((u32)0x00000200)
#define A2C_FD_NOT_SWITCH_BIT_TIMING		((u32)0x00000000)
#define IS_A2C_FD_BIT_RATE_SWITCH_TYPE(TYPE) (((TYPE) == A2C_FD_NOT_SWITCH_BIT_TIMING) || \
									((TYPE) == A2C_FD_SWITCH_BIT_TIMING))

/**
  * @}
  */

/** @defgroup A2C_FD_ER_STS_INDICATOR_TYPE_define
  * @{
  */
#define A2C_FD_ERROR_ACTIVE_NODE		((u32)0x00000000)
#define A2C_FD_ERROR_PASSIVE_NODE		((u32)0x00000400)
#define IS_A2C_FD_ER_NODE_TYPE(TYPE) (((TYPE) == A2C_FD_ERROR_ACTIVE_NODE) || \
									((TYPE) == A2C_FD_ERROR_PASSIVE_NODE))
/**
  * @}
  */

/** @defgroup A2C_PROTOCOL_FRAME_TYPE_define
  * @{
  */
#define A2C_FD_FRAME_TYPE					((u32)0x00000100)
#define A2C_NOT_FD_FRAME_TYPE				((u32)0x00000000)
#define IS_A2C_PRO_FRAEME_TYPE(TYPE) (((TYPE) == A2C_FD_FRAME_TYPE) || \
									((TYPE) == A2C_NOT_FD_FRAME_TYPE))

/**
  * @}
  */

/** @defgroup A2C_FRAME_TYPE_define
  * @{
  */
#define A2C_REMOTE_FRAME				((u32)0x40000000)
#define A2C_DATA_FRAME					((u32)0x00000000)
#define IS_A2C_FRAME_TYPE(TYPE) (((TYPE) == A2C_REMOTE_FRAME) || \
									((TYPE) == A2C_DATA_FRAME))

/**
  * @}
  */

/** @defgroup A2C_RTR_MASK_TYPE_define
  * @{
  */
#define A2C_RTR_BIT_MASK				((u32)0x40000000)
#define A2C_RTR_NIT_NOT_MASK			((u32)0x00000000)
#define IS_A2C_RTR_BIT_MASK_TYPE(TYPE) (((TYPE) == A2C_RTR_BIT_MASK) || \
									((TYPE) == A2C_RTR_NIT_NOTE_MASK))

/**
  * @}
  */

/** @defgroup A2C_IDE_MASK_TYPE_define
  * @{
  */
#define A2C_IDE_BIT_MASK				((u32)0x20000000)
#define A2C_IDE_NIT_NOT_MASK			((u32)0x00000000)
#define IS_A2C_IDE_BIT_MASK_TYPE(TYPE) (((TYPE) == A2C_IDE_BIT_MASK) || \
									((TYPE) == A2C_IDE_NIT_NOT_MASK))

/**
  * @}
  */

/** @defgroup A2C_IDE_MASK_TYPE_define
  * @{
  */
#define A2C_BIT_RAM_ACC_DATA_MASK  (A2C_BIT_RAM_ACC_DATA15 | A2C_BIT_RAM_ACC_DATA14 | A2C_BIT_RAM_ACC_DATA13 | A2C_BIT_RAM_ACC_DATA12 | \
                                    A2C_BIT_RAM_ACC_DATA11 | A2C_BIT_RAM_ACC_DATA10 | A2C_BIT_RAM_ACC_DATA9 | A2C_BIT_RAM_ACC_DATA8 | \
                                    A2C_BIT_RAM_ACC_DATA7 | A2C_BIT_RAM_ACC_DATA6 | A2C_BIT_RAM_ACC_DATA5 | A2C_BIT_RAM_ACC_DATA4 | \
                                    A2C_BIT_RAM_ACC_DATA3 | A2C_BIT_RAM_ACC_DATA2 | A2C_BIT_RAM_ACC_DATA1 | A2C_BIT_RAM_ACC_DATA0)

/**
  * @}
  */

/** @defgroup A2C_MESSAGE_BUFFER_STATUS_define
  * @{
  */
#define A2C_FRAME_PENDING_TX			((u32)0x00000001)
#define A2C_FRAME_FINISAH_TX			((u32)0x00000002)
#define A2C_FRAME_OVWRITR_SEND_TX	((u32)0x00000003)
#define A2C_FRAME_PENDING_RX			((u32)0x00000004)
#define A2C_FRAME_FINISH_RX			((u32)0x0000000C)

#define IS_A2C_MSG_STS_TYPE(TYPE) (((TYPE) == A2C_FRAME_PENDING_TX) || \
									((TYPE) == A2C_FRAME_FINISAH_TX) || \
									((TYPE) == A2C_FRAME_OVWRITR_SEND_TX) || \
									((TYPE) == A2C_SILENCE_MODE) || \
									((TYPE) == A2C_FRAME_PENDING_RX))

/**
  * @}
  */

/** @defgroup A2C_INTERRUPTS_define
  * @{
  */
#define A2C_TX_INT					((u32)0x00000001)
#define A2C_RX_INT					((u32)0x00000002)
#define A2C_ERR_INT				((u32)0x00000004)
#define A2C_WKUP_INT				((u32)0x00000008)
#define A2C_BUSOFF_INT				((u32)0x00000010)
#define A2C_RAM_MOVE_DONE_INT		((u32)0x00000020)
#define IS_A2C_CONFIG_IT(IT) (((IT) == A2C_TX_INT) || \
								((IT) == A2C_RX_INT) || \
									((IT) == A2C_ERR_INT)|| \
									((IT) == A2C_WKUP_INT)|| \
									((IT) == A2C_RAM_MOVE_DONE_INT)|| \
									((IT) == A2C_BUSOFF_INT))

#define IS_A2C_CLEAR_IT(IT) ((((IT) & (u32)0xFFFFFFC0) == 0x00) && ((IT) != 0x00))

/**
  * @}
  */

/** @defgroup A2C_type_define
  * @{
  */
#define IS_A2C_ALL_PERIPH(PERIPH)   (((PERIPH) == A2C0) || \
									  ((PERIPH) == A2C1) || \
									  ((PERIPH) == A2C0_S) || \
									  ((PERIPH) == A2C1_S))

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
/** @defgroup A2C_Exported_Functions A2C Exported Functions
  * @{
  */
/** @defgroup A2C_global_functions
  * @{
  */

void A2C_StructInit(A2C_InitTypeDef *A2C_InitStruct);
void A2C_Init(A2C_TypeDef *A2Cx, A2C_InitTypeDef *A2C_InitStruct);

void A2C_LpFilterCmd(A2C_TypeDef *A2Cx, u32 NewState);
void A2C_LpFilterConfig(A2C_TypeDef *A2Cx, u32 FltNum);

void A2C_BusCmd(A2C_TypeDef *A2Cx, u32 NewState);
u32 A2C_BusStatusGet(A2C_TypeDef *A2Cx);

u32 A2C_FifoStatusGet(A2C_TypeDef *A2Cx);
u32 A2C_FifoLvlGet(A2C_TypeDef *A2Cx);
void A2C_ReadRxMsgFromFifo(A2C_TypeDef *A2Cx, A2C_RxMsgTypeDef *RxMsg);

u32 A2C_TXErrCntGet(A2C_TypeDef *A2Cx);
u32 A2C_RXErrCntGet(A2C_TypeDef *A2Cx);
void A2C_TXErrCntClear(A2C_TypeDef *A2Cx);
void A2C_RXErrCntClear(A2C_TypeDef *A2Cx);
u32 A2C_RXErrCntSTS(A2C_TypeDef *A2Cx);


void A2C_WriteMsg(A2C_TypeDef *A2Cx, A2C_TxMsgTypeDef *TxMsg);
void A2C_SetRxMsgBuf(A2C_TypeDef *A2Cx, A2C_RxMsgTypeDef *RxMsg);
void A2C_ReadMsg(A2C_TypeDef *A2Cx, A2C_RxMsgTypeDef *RxMsg);
void A2C_TxAutoReply(A2C_TypeDef *A2Cx, A2C_TxMsgTypeDef *TxMsg);
void A2C_RxAutoReply(A2C_TypeDef *A2Cx, A2C_RxMsgTypeDef *RxMsg);

void A2C_Cmd(A2C_TypeDef *A2Cx, u32 NewState);

/**
  * @}
  */


/** @defgroup A2C_Interrupt_status_functions
  * @{
  */

void A2C_INTConfig(A2C_TypeDef *A2Cx, u32 A2C_IT, u32 NewState);
u32 A2C_GetINTStatus(A2C_TypeDef *A2Cx);
void A2C_ClearINT(A2C_TypeDef *A2Cx, u32 A2C_IT);
void A2C_ClearAllINT(A2C_TypeDef *A2Cx);
u32 A2C_GetErrStatus(A2C_TypeDef *A2Cx);
void A2C_ClearErrStatus(A2C_TypeDef *A2Cx, u32 ERR_STS);

void A2C_RxMsgBufINTConfig(A2C_TypeDef *A2Cx, u32 BUF_IT, u32 NewState);
void A2C_TxMsgBufINTConfig(A2C_TypeDef *A2Cx, u32 BUF_IT, u32 NewState);

u32 A2C_TxMsgBufErrGet(A2C_TypeDef *A2Cx);
void A2C_TxMsgBufErrClear(A2C_TypeDef *A2Cx, u32 ERR_FLAG);

u32 A2C_MsgBufStatusRegGet(A2C_TypeDef *A2Cx, u32 MsgBufIdx);
u32 A2C_MsgBufStatusGet(A2C_TypeDef *A2Cx, u32 MsgBufIdx);
u32 A2C_CheckMsgBufAvailable(A2C_TypeDef *A2Cx, u32 MsgBufIdx);

u32 A2C_TxDoneStatusGet(A2C_TypeDef *A2Cx);
u32 A2C_MsgBufTxDoneStatusGet(A2C_TypeDef *A2Cx, u32 MsgBufIdx);
u32 A2C_RxDoneStatusGet(A2C_TypeDef *A2Cx);
u32 A2C_MsgBufRxDoneStatusGet(A2C_TypeDef *A2Cx, u32 MsgBufIdx);

void A2C_TxDoneStatusClear(A2C_TypeDef *A2Cx, u32 Status);
void A2C_MsgBufTxDoneStatusClear(A2C_TypeDef *A2Cx, u32 MsgBufIdx);

void A2C_RxDoneStatusClear(A2C_TypeDef *A2Cx, u32 Status);
void A2C_MsgBufRxDoneStatusClear(A2C_TypeDef *A2Cx, u32 MsgBufIdx);


void A2C_TxMsgTriggerCmd(A2C_TypeDef *A2Cx, u32 State);
void A2C_TxMsgTriggerConfig(A2C_TypeDef *A2Cx, u32 CloseOffset, u32 Begin);

void A2C_RxDmaDestBaseAddrConfig(A2C_TypeDef *A2Cx, u32 Addr);
u32 A2C_GetRxDmaData(A2C_TypeDef *A2Cx);

void A2C_MsgCtrlRegConfig(A2C_TypeDef *A2Cx, u32 MsgIdx, u32 RxDmaEn, u32 DmaOffset);
void A2C_MsgBaseAddrConfig(A2C_TypeDef *A2Cx, u32 MsgIdx, u32 BaseAddr);
void A2C_MsgBaseAddrEndReg(A2C_TypeDef *A2Cx, u32 EndAddr);

void A2C_RamBufferMapConfig(A2C_TypeDef *A2Cx, u32 *pPara);
/**
  * @}
  */

/** @defgroup A2C_dma_functions
  * @{
  */
bool A2C_RXGDMA_Init(u8 Index, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pRxBuf, int RxCount);
/**
  * @}
  */

/**
  * @}
  */
struct A2C_DevTable {
	A2C_TypeDef *A2Cx;
	IRQn_Type	IrqNum;
	u32 Rx_HandshakeInterface;
};

#define MAX_A2C_INDEX			(2)

#define A2C_MESSAGE_BUFFER_LEN	            (5)

#define A2C_MESSAGE_BUFFER_SIZE	        (16)

#define A2C_MESSAGE_FIFO_SIZE		        (4)

#define A2C_RX_FIFO_READ_MSG_IDX	        (12)

extern const struct A2C_DevTable A2C_DEV_TABLE[MAX_A2C_INDEX];


#endif