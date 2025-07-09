#ifndef _AMEBA_CAN_H_
#define _AMEBA_CAN_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup CAN
  * @brief CAN driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup CAN_Register_Definitions CAN Register Definitions
  * @{
  */

/** @defgroup CAN_CTL
  * @brief CAN bus control register
  * @{
  */
#define CAN_BIT_FD_SSP_AUTO   ((u32)0x00000001 << 9) /*!< R/W 0x0  automatic calculat ssp delay, ssp = second sample point*/
#define CAN_BIT_FD_CRC_MODE   ((u32)0x00000001 << 8) /*!< R/W 0x0  crc mode select * 0: ISO crc * 1: non-ISO crc*/
#define CAN_BIT_RXDMA_EN      ((u32)0x00000001 << 7) /*!< R/W 0x0  rx dma enable*/
#define CAN_BIT_FD_EN         ((u32)0x00000001 << 6) /*!< R/W 0x0  enable CAN FD protocol*/
#define CAN_BIT_RX_FIFO_EN    ((u32)0x00000001 << 5) /*!< R/W 0x0  rx fifo function enable*/
#define CAN_BIT_TEST_MODE_EN  ((u32)0x00000001 << 4) /*!< R/W 0x0  test mode enable, for lbk or silent mode use*/
#define CAN_BIT_AUTO_RE_TX_EN ((u32)0x00000001 << 3) /*!< R/W 0x0  auto re-transmission enabled*/
#define CAN_BIT_TRI_SAMPLE    ((u32)0x00000001 << 2) /*!< R/W 0x0  triple sample mode*/
#define CAN_BIT_BUS_ON_REQ    ((u32)0x00000001 << 1) /*!< R/W/ES 0x0  request to bus_on*/
#define CAN_BIT_EN            ((u32)0x00000001 << 0) /*!< R/W 0x0  can enable*/
/** @} */

/** @defgroup CAN_STS
  * @brief CAN bus status
  * @{
  */
#define CAN_BIT_BUS_ON_STATE ((u32)0x00000001 << 0) /*!< R 0x0  bus state * 1 means the can module is already bus on * 0 means the can module is busoff*/
/** @} */

/** @defgroup CAN_FIFO_STS
  * @brief CAN bus FIFO status
  * @{
  */
#define CAN_MASK_FIFO_MSG_LVL     ((u32)0x00000007 << 4)           /*!< R 0x0  rx fifo message level*/
#define CAN_FIFO_MSG_LVL(x)       (((u32)((x) & 0x00000007) << 4))
#define CAN_GET_FIFO_MSG_LVL(x)   ((u32)(((x >> 4) & 0x00000007)))
#define CAN_BIT_FIFO_MSG_OVERFLOW ((u32)0x00000001 << 2)           /*!< R 0x0  rx fifo overflow*/
#define CAN_BIT_FIFO_MSG_EMPTY    ((u32)0x00000001 << 1)           /*!< R 0x1  rx fifo empty*/
#define CAN_BIT_FIFO_MSG_FULL     ((u32)0x00000001 << 0)           /*!< R 0x0  rx fifo full*/
/** @} */

/** @defgroup CAN_BIT_TIMING
  * @brief CAN2.0 bit timing
  * @{
  */
#define CAN_MASK_BRP     ((u32)0x000000FF << 24)           /*!< R/W 0x0  CAN bit timing perscaler = <$bit:can_brp>+1*/
#define CAN_BRP(x)       (((u32)((x) & 0x000000FF) << 24))
#define CAN_GET_BRP(x)   ((u32)(((x >> 24) & 0x000000FF)))
#define CAN_MASK_SJW     ((u32)0x00000007 << 16)           /*!< R/W 0x0  SJW length = <$bit:can_sjw>*/
#define CAN_SJW(x)       (((u32)((x) & 0x00000007) << 16))
#define CAN_GET_SJW(x)   ((u32)(((x >> 16) & 0x00000007)))
#define CAN_MASK_TSEG2   ((u32)0x000000FF << 8)            /*!< R/W 0x0  TSEG2 length = <$bit:can_tseg2+1>*/
#define CAN_TSEG2(x)     (((u32)((x) & 0x000000FF) << 8))
#define CAN_GET_TSEG2(x) ((u32)(((x >> 8) & 0x000000FF)))
#define CAN_MASK_TSEG1   ((u32)0x000000FF << 0)            /*!< R/W 0x0  TSEG1 length = <$bit:can_tseg1+1>*/
#define CAN_TSEG1(x)     (((u32)((x) & 0x000000FF) << 0))
#define CAN_GET_TSEG1(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup CAN_FD_BIT_TIMING
  * @brief CAN FD DATA field bit timing
  * @{
  */
#define CAN_MASK_FD_BRP     ((u32)0x000000FF << 24)           /*!< R/W 0x0  CAN bit timing perscaler = <$bit:can_fd_brp>+1*/
#define CAN_FD_BRP(x)       (((u32)((x) & 0x000000FF) << 24))
#define CAN_GET_FD_BRP(x)   ((u32)(((x >> 24) & 0x000000FF)))
#define CAN_MASK_FD_SJW     ((u32)0x00000007 << 16)           /*!< R/W 0x0  SJW length = <$bit:can_fd_sjw>*/
#define CAN_FD_SJW(x)       (((u32)((x) & 0x00000007) << 16))
#define CAN_GET_FD_SJW(x)   ((u32)(((x >> 16) & 0x00000007)))
#define CAN_MASK_FD_TSEG2   ((u32)0x000000FF << 8)            /*!< R/W 0x0  TSEG2 length = <$bit:can_fd_tseg2+1>*/
#define CAN_FD_TSEG2(x)     (((u32)((x) & 0x000000FF) << 8))
#define CAN_GET_FD_TSEG2(x) ((u32)(((x >> 8) & 0x000000FF)))
#define CAN_MASK_FD_TSEG1   ((u32)0x000000FF << 0)            /*!< R/W 0x0  TSEG1 length = <$bit:can_fd_tseg1>+1*/
#define CAN_FD_TSEG1(x)     (((u32)((x) & 0x000000FF) << 0))
#define CAN_GET_FD_TSEG1(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup CAN_FD_SSP_CAL
  * @brief CAN FD SSP configuration
  * @{
  */
#define CAN_MASK_FD_SSP_DCO2   ((u32)0x000000FF << 24)           /*!< R/W 0x0  delay compensation offset2*/
#define CAN_FD_SSP_DCO2(x)     (((u32)((x) & 0x000000FF) << 24))
#define CAN_GET_FD_SSP_DCO2(x) ((u32)(((x >> 24) & 0x000000FF)))
#define CAN_MASK_FD_SSP_DCO    ((u32)0x000000FF << 16)           /*!< R/W 0x0  delay compensation offset*/
#define CAN_FD_SSP_DCO(x)      (((u32)((x) & 0x000000FF) << 16))
#define CAN_GET_FD_SSP_DCO(x)  ((u32)(((x >> 16) & 0x000000FF)))
#define CAN_MASK_FD_SSP_MIN    ((u32)0x000000FF << 8)            /*!< R/W 0x0  minimum of ssp from start of one bit, only functional when can_fd_ssp_auto set*/
#define CAN_FD_SSP_MIN(x)      (((u32)((x) & 0x000000FF) << 8))
#define CAN_GET_FD_SSP_MIN(x)  ((u32)(((x >> 8) & 0x000000FF)))
#define CAN_MASK_FD_SSP        ((u32)0x000000FF << 0)            /*!< R/W/ES 0x0  ssp position, delay from start of one bit, if automatic calculation is enabled, the field will update after calculation*/
#define CAN_FD_SSP(x)          (((u32)((x) & 0x000000FF) << 0))
#define CAN_GET_FD_SSP(x)      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup CAN_INT_EN
  * @brief CAN interrupt control
  * @{
  */
#define CAN_BIT_RAM_MOVE_DONE_INT_MASK ((u32)0x00000001 << 21) /*!< R/W 0x0  ram move done interrupt mask 1: mask 0: unmask*/
#define CAN_BIT_BUSOFF_INT_MASK        ((u32)0x00000001 << 20) /*!< R/W 0x0  wakeup interrupt mask 1: mask 0: unmask*/
#define CAN_BIT_WAKEUP_INT_MASK        ((u32)0x00000001 << 19) /*!< R/W 0x0  wakeup interrupt mask 1: mask 0: unmask*/
#define CAN_BIT_ERROR_INT_MASK         ((u32)0x00000001 << 18) /*!< R/W 0x0  error interrupt mask 1: mask 0: unmask*/
#define CAN_BIT_RX_INT_MASK            ((u32)0x00000001 << 17) /*!< R/W 0x0  rx interrupt mask 1: mask 0: unmask*/
#define CAN_BIT_TX_INT_MASK            ((u32)0x00000001 << 16) /*!< R/W 0x0  tx interrupt mask 1: mask 0: unmask*/
#define CAN_BIT_RAM_MOVE_DONE_INT_EN   ((u32)0x00000001 << 5)  /*!< R/W 0x0  ram move done interrupt enable*/
#define CAN_BIT_BUSOFF_INT_EN          ((u32)0x00000001 << 4)  /*!< R/W 0x0  wakeup interrupt enbale*/
#define CAN_BIT_WAKEUP_INT_EN          ((u32)0x00000001 << 3)  /*!< R/W 0x0  wakeup interrupt enbale*/
#define CAN_BIT_ERROR_INT_EN           ((u32)0x00000001 << 2)  /*!< R/W 0x0  error interrupt enable*/
#define CAN_BIT_RX_INT_EN              ((u32)0x00000001 << 1)  /*!< R/W 0x0  rx interrupt enable*/
#define CAN_BIT_TX_INT_EN              ((u32)0x00000001 << 0)  /*!< R/W 0x0  tx interrupt enable*/
/** @} */

/** @defgroup CAN_MB_RXINT_EN
  * @brief CAN rx interrupt for message buffers
  * @{
  */
#define CAN_MASK_MB_RXINT_EN   ((u32)0x0000FFFF << 0)           /*!< R/W 0x0  rx interrupt enable for each message buffer*/
#define CAN_MB_RXINT_EN(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define CAN_GET_MB_RXINT_EN(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup CAN_MB_TXINT_EN
  * @brief CAN tx interrupt for message buffers
  * @{
  */
#define CAN_MASK_MB_TXINT_EN   ((u32)0x0000FFFF << 0)           /*!< R/W 0x0  tx interrupt enable for each message buffer*/
#define CAN_MB_TXINT_EN(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define CAN_GET_MB_TXINT_EN(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup CAN_INT_FLAG
  * @brief CAN interrupt flags
  * @{
  */
#define CAN_BIT_RAM_MOVE_DONE_INT_FLAG ((u32)0x00000001 << 5) /*!< RW1C 0x0  data move from register to CAN IP internal RAM finished interupt, move from RAM to register done will NOT generate interrupt, write one to clear*/
#define CAN_BIT_BUSOFF_INT_FLAG        ((u32)0x00000001 << 4) /*!< RW1C 0x0  busoff interrupt flag, write one to clear*/
#define CAN_BIT_WAKEUP_INT_FLAG        ((u32)0x00000001 << 3) /*!< RW1C 0x0  wakeup interrupt flag, write one to clear*/
#define CAN_BIT_ERROR_INT_FLAG         ((u32)0x00000001 << 2) /*!< RW1C 0x0  error interrupt flag, write to clear*/
#define CAN_BIT_RX_INT_FLAG            ((u32)0x00000001 << 1) /*!< RW1C 0x0  rx interrupt flag, write to clear*/
#define CAN_BIT_TX_INT_FLAG            ((u32)0x00000001 << 0) /*!< RW1C 0x0  tx interrupt flag, write to clear*/
/** @} */

/** @defgroup CAN_ERR_STATUS
  * @brief CAN error status
  * @{
  */
#define CAN_BIT_ERROR_RX    ((u32)0x00000001 << 9) /*!< RW1C 0x0  can rx error flag*/
#define CAN_BIT_ERROR_TX    ((u32)0x00000001 << 8) /*!< RW1C 0x0  can tx error flag*/
#define CAN_BIT_ERROR_ACK   ((u32)0x00000001 << 5) /*!< RW1C 0x0  latest error  is ack error*/
#define CAN_BIT_ERROR_STUFF ((u32)0x00000001 << 4) /*!< RW1C 0x0  latest error  is stuff error*/
#define CAN_BIT_ERROR_CRC   ((u32)0x00000001 << 3) /*!< RW1C 0x0  latest error  is ctc error*/
#define CAN_BIT_ERROR_FORM  ((u32)0x00000001 << 2) /*!< RW1C 0x0  latest error  is form error*/
#define CAN_BIT_ERROR_BIT1  ((u32)0x00000001 << 1) /*!< RW1C 0x0  latest error  is bit1 error, tx=1 but rx=0*/
#define CAN_BIT_ERROR_BIT0  ((u32)0x00000001 << 0) /*!< RW1C 0x0  latest error  is bit0 error, tx=0 but rx=1*/
/** @} */

/** @defgroup CAN_ERR_CNT_CTL
  * @brief CAN bus error counter control
  * @{
  */
#define CAN_MASK_ERROR_WARN_TH   ((u32)0x000001FF << 8)           /*!< R/W 0x60  error counter warning threshold, programable, default 96. NOTES: 0~127 is availaible to both TEC and REC, 128~255 is only available to TEC*/
#define CAN_ERROR_WARN_TH(x)     (((u32)((x) & 0x000001FF) << 8))
#define CAN_GET_ERROR_WARN_TH(x) ((u32)(((x >> 8) & 0x000001FF)))
#define CAN_BIT_RX_ERR_CNT_CLR   ((u32)0x00000001 << 1)           /*!< WA0 0x0  write 1 to clear can_rec*/
#define CAN_BIT_TX_ERR_CNT_CLR   ((u32)0x00000001 << 0)           /*!< WA0 0x0  write 1 to clear can_tec*/
/** @} */

/** @defgroup CAN_ERR_CNT_STS
  * @brief CAN bus error counter status
  * @{
  */
#define CAN_BIT_ERROR_WARNING ((u32)0x00000001 << 30)           /*!< R 0x0  error counter arrive at warning threshold*/
#define CAN_BIT_ERROR_BUSOFF  ((u32)0x00000001 << 29)           /*!< R 0x0  error counter arrive at bus off level*/
#define CAN_BIT_ERROR_PASSIVE ((u32)0x00000001 << 28)           /*!< R 0x0  error counter arrive at error passive level*/
#define CAN_MASK_REC          ((u32)0x000001FF << 16)           /*!< R 0x0  receive error count*/
#define CAN_REC(x)            (((u32)((x) & 0x000001FF) << 16))
#define CAN_GET_REC(x)        ((u32)(((x >> 16) & 0x000001FF)))
#define CAN_MASK_TEC          ((u32)0x000001FF << 0)            /*!< R 0x0  transmit error count*/
#define CAN_TEC(x)            (((u32)((x) & 0x000001FF) << 0))
#define CAN_GET_TEC(x)        ((u32)(((x >> 0) & 0x000001FF)))
/** @} */

/** @defgroup CAN_TX_ERROR_FLAG
  * @brief CAN tx error flag for each message buffe
  * @{
  */
#define CAN_MASK_TX_ERROR_FLAG   ((u32)0x0000FFFF << 0)           /*!< RW1CB 0x0  indicate which message tx buffer have error detected, cpu can write 1 to clear*/
#define CAN_TX_ERROR_FLAG(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define CAN_GET_TX_ERROR_FLAG(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup CAN_TX_DONE
  * @brief CAN tx done for message buffers
  * @{
  */
#define CAN_MASK_TX_DONE   ((u32)0x0000FFFF << 0)           /*!< RW1CB 0x0  tx done for each message buffer*/
#define CAN_TX_DONE(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define CAN_GET_TX_DONE(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup CAN_RX_DONE
  * @brief CAN rx done for message buffers
  * @{
  */
#define CAN_MASK_RX_DONE   ((u32)0x0000FFFF << 0)           /*!< RW1CB 0x0  rx done for each message buffer*/
#define CAN_RX_DONE(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define CAN_GET_RX_DONE(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup CAN_TIME_STAMP
  * @brief Timestamp function
  * @{
  */
#define CAN_BIT_TIME_STAMP_EN     ((u32)0x00000001 << 31)           /*!< R/W 0x0  time stamp function enable and time stamp counter start*/
#define CAN_MASK_TIME_STAMP_DIV   ((u32)0x000000FF << 16)           /*!< R/W 0x0  timer stamp conter  prescalar*/
#define CAN_TIME_STAMP_DIV(x)     (((u32)((x) & 0x000000FF) << 16))
#define CAN_GET_TIME_STAMP_DIV(x) ((u32)(((x >> 16) & 0x000000FF)))
#define CAN_MASK_TIME_STAMP       ((u32)0x0000FFFF << 0)            /*!< R 0x0  time stamp counter value, up count*/
#define CAN_TIME_STAMP(x)         (((u32)((x) & 0x0000FFFF) << 0))
#define CAN_GET_TIME_STAMP(x)     ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup CAN_MB_TRIGGER
  * @brief CAN tx trigger by timer configuration
  * @{
  */
#define CAN_BIT_TX_TRIGGER_EN              ((u32)0x00000001 << 24)           /*!< R/W 0x0  End of message buffer base address in RAM*/
#define CAN_MASK_TX_TRIGGER_CLOSE_OFFSET   ((u32)0x000000FF << 16)           /*!< R/W 0x0  end of trigger time = <$bit:tx_trigger_begin> + <$bit:tx_trigger_close_offset>*/
#define CAN_TX_TRIGGER_CLOSE_OFFSET(x)     (((u32)((x) & 0x000000FF) << 16))
#define CAN_GET_TX_TRIGGER_CLOSE_OFFSET(x) ((u32)(((x >> 16) & 0x000000FF)))
#define CAN_MASK_TX_TRIGGER_BEGIN          ((u32)0x0000FFFF << 0)            /*!< R/W 0x0  start of trigger time*/
#define CAN_TX_TRIGGER_BEGIN(x)            (((u32)((x) & 0x0000FFFF) << 0))
#define CAN_GET_TX_TRIGGER_BEGIN(x)        ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup CAN_RXDMA_CFG
  * @brief RX dma configuration
  * @{
  */
#define CAN_BIT_RXDMA_OWNER         ((u32)0x00000001 << 24)           /*!< R/W 0x0  can rxdma owner is : 0:GDMA 1:CAN*/
#define CAN_MASK_RXDMA_SRC_MSIZE    ((u32)0x00000003 << 16)           /*!< R/W 0x1  rx dma burst size: 'b00: dma burst size=1 'b01: dma burst size=4 'b10: dma burst size=8 'b11: dma burst size=16*/
#define CAN_RXDMA_SRC_MSIZE(x)      (((u32)((x) & 0x00000003) << 16))
#define CAN_GET_RXDMA_SRC_MSIZE(x)  ((u32)(((x >> 16) & 0x00000003)))
#define CAN_MASK_RXDMA_FRAME_SIZE   ((u32)0x00003FFF << 0)            /*!< R 0x2  can rx dma data frame size = ceil(<rx_dma_data bytes number>/4 ), for example: rx_dma_data is 9~12 bytes, frame size is 3*/
#define CAN_RXDMA_FRAME_SIZE(x)     (((u32)((x) & 0x00003FFF) << 0))
#define CAN_GET_RXDMA_FRAME_SIZE(x) ((u32)(((x >> 0) & 0x00003FFF)))
/** @} */

/** @defgroup CAN_RX_DMA_DATA
  * @brief Register for dma access message buffe
  * @{
  */
#define CAN_MASK_RX_DMA_DATA   ((u32)0xFFFFFFFF << 0)           /*!< RP 0x0  For DMA read received data*/
#define CAN_RX_DMA_DATA(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define CAN_GET_RX_DMA_DATA(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup CAN_SLEEP_MODE
  * @brief sleep mode request and wakeup pin digital filter
  * @{
  */
#define CAN_BIT_SLEEP_STATE           ((u32)0x00000001 << 17)          /*!< R 0x0  can is sleep or not 1: is 0: isn't*/
#define CAN_BIT_SLEEP_EN              ((u32)0x00000001 << 16)          /*!< R/W/ES 0x0  can sleep enable 1:request can to sleep mode 0:wakeup can*/
#define CAN_BIT_WAKEPIN_FLT_EN        ((u32)0x00000001 << 8)           /*!< R/W 0x0  wakeup pin digital filter enable*/
#define CAN_MASK_WAKEPIN_FLT_LENGTH   ((u32)0x000000FF << 0)           /*!< R/W 0x1  wakeup pin digital filter length*/
#define CAN_WAKEPIN_FLT_LENGTH(x)     (((u32)((x) & 0x000000FF) << 0))
#define CAN_GET_WAKEPIN_FLT_LENGTH(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup CAN_TEST
  * @brief test configuration
  * @{
  */
#define CAN_MASK_TEST_CFG   ((u32)0x00000003 << 0)           /*!< R/W 0x0  2b'00: silence_mode 2b'01: external loopback mode (enable can_tx_so ) 2b'10: internal loopback mode (can_tx_so tie 1) 2b'11: reserved*/
#define CAN_TEST_CFG(x)     (((u32)((x) & 0x00000003) << 0))
#define CAN_GET_TEST_CFG(x) ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup CAN_MBx_STS
  * @brief CAN message buffer status
  * @{
  */
#define CAN_BIT_MSGx_RX_VLD  ((u32)0x00000001 << 3) /*!< R 0x0  new message have been received in the message buffer*/
#define CAN_BIT_MSGx_RX_RDY  ((u32)0x00000001 << 2) /*!< R 0x0  the message buffer is ready for receiving a new message*/
#define CAN_BIT_MSGx_TX_DONE ((u32)0x00000001 << 1) /*!< R 0x0  tx message in the message buffer finish sending*/
#define CAN_BIT_MSGx_TX_REQ  ((u32)0x00000001 << 0) /*!< R 0x0  tx message is pending for transmit*/
/** @} */

/** @defgroup CAN_MBx_CTRL
  * @brief CAN message buffer control
  * @{
  */
#define CAN_BIT_MSGx_RXDMA_EN ((u32)0x00000001 << 24)          /*!< R/W/ES 0x0  dma enable when the message buffer used as rx*/
#define CAN_MASK_MSGx_BA      ((u32)0x000001FF << 0)           /*!< R/W 9'd0  message buffer base address in RAM*/
#define CAN_MSGx_BA(x)        (((u32)((x) & 0x000001FF) << 0))
#define CAN_GET_MSGx_BA(x)    ((u32)(((x >> 0) & 0x000001FF)))
/** @} */

/** @defgroup CAN_MB_BA_END
  * @brief
  * @{
  */
#define CAN_MASK_MSG_BA_END   ((u32)0x000003FF << 0)           /*!< R/W 10'd96  End of message buffer base address in RAM*/
#define CAN_MSG_BA_END(x)     (((u32)((x) & 0x000003FF) << 0))
#define CAN_GET_MSG_BA_END(x) ((u32)(((x >> 0) & 0x000003FF)))
/** @} */

/** @defgroup CAN_RAM_FDDATA_x
  * @brief CAN data
  * @{
  */
#define CAN_MASK_RAM_DATA_x_3   ((u32)0x000000FF << 24)           /*!< R/W/ES 0x0  data_byte 4*x+3*/
#define CAN_RAM_DATA_x_3(x)     (((u32)((x) & 0x000000FF) << 24))
#define CAN_GET_RAM_DATA_x_3(x) ((u32)(((x >> 24) & 0x000000FF)))
#define CAN_MASK_RAM_DATA_x_2   ((u32)0x000000FF << 16)           /*!< R/W/ES 0x0  data_byte 4*x+2*/
#define CAN_RAM_DATA_x_2(x)     (((u32)((x) & 0x000000FF) << 16))
#define CAN_GET_RAM_DATA_x_2(x) ((u32)(((x >> 16) & 0x000000FF)))
#define CAN_MASK_RAM_DATA_x_1   ((u32)0x000000FF << 8)            /*!< R/W/ES 0x0  data_byte 4*x+1*/
#define CAN_RAM_DATA_x_1(x)     (((u32)((x) & 0x000000FF) << 8))
#define CAN_GET_RAM_DATA_x_1(x) ((u32)(((x >> 8) & 0x000000FF)))
#define CAN_MASK_RAM_DATA_x_0   ((u32)0x000000FF << 0)            /*!< R/W/ES 0x0  data_byte 4*x+0*/
#define CAN_RAM_DATA_x_0(x)     (((u32)((x) & 0x000000FF) << 0))
#define CAN_GET_RAM_DATA_x_0(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup CAN_RAM_ARB
  * @brief CAN message buffer arb field configuration
  * @{
  */
#define CAN_BIT_RAM_RTR   ((u32)0x00000001 << 30)          /*!< R/W/ES 0x0  can frame RTR bit, determine DATA or REMOTE frame*/
#define CAN_BIT_RAM_IDE   ((u32)0x00000001 << 29)          /*!< R/W/ES 0x0  can frame IDE bit, determine standard or extend format*/
#define CAN_MASK_RAM_ID   ((u32)0x1FFFFFFF << 0)           /*!< R/W/ES 0x0  can frame ID, including high 11 bit standard ID and low 18 bit extend ID*/
#define CAN_RAM_ID(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define CAN_GET_RAM_ID(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CAN_RAM_MASK
  * @brief CAN message buffer arb field mask configuration
  * @{
  */
#define CAN_BIT_RAM_RTR_MASK   ((u32)0x00000001 << 30)          /*!< R/W/ES 0x0  can frame RTR mask, 0 means don't care, 1 means the bit should match*/
#define CAN_BIT_RAM_IDE_MASK   ((u32)0x00000001 << 29)          /*!< R/W/ES 0x0  can frame IDE mask, 0 means don't care, 1 means the bit should match*/
#define CAN_MASK_RAM_ID_MASK   ((u32)0x1FFFFFFF << 0)           /*!< R/W/ES 0x0  can frame ID mask, 0 means the ID bit in CAN_RAM_ARB don't care, 1 means the bit should match.*/
#define CAN_RAM_ID_MASK(x)     (((u32)((x) & 0x1FFFFFFF) << 0))
#define CAN_GET_RAM_ID_MASK(x) ((u32)(((x >> 0) & 0x1FFFFFFF)))
/** @} */

/** @defgroup CAN_RAM_CS
  * @brief CAN message type control
  * @{
  */
#define CAN_MASK_RAM_TIMESTAMP   ((u32)0x0000FFFF << 16)           /*!< R/W/ES 0x0  the received message time stamp*/
#define CAN_RAM_TIMESTAMP(x)     (((u32)((x) & 0x0000FFFF) << 16))
#define CAN_GET_RAM_TIMESTAMP(x) ((u32)(((x >> 16) & 0x0000FFFF)))
#define CAN_BIT_RAM_ESI          ((u32)0x00000001 << 10)           /*!< R/W/ES 0x0  FD frame esi bit, indicate error passive of message sending node, read bit, should always write 0, hardware automatic fill*/
#define CAN_BIT_RAM_BRS          ((u32)0x00000001 << 9)            /*!< R/W/ES 0x0  FD frame brs bit, indicate switch bit timing*/
#define CAN_BIT_RAM_EDL          ((u32)0x00000001 << 8)            /*!< R/W/ES 0x0  FD frame edl bit, indicate it's a FD frame*/
#define CAN_BIT_RAM_AUTOREPLY    ((u32)0x00000001 << 6)            /*!< R/W/ES 0x0  set the message buffer with remote frame auto reply function*/
#define CAN_BIT_RAM_RXTX         ((u32)0x00000001 << 5)            /*!< R/W/ES 0x0  can frame RX or TX, 0 is for RX, 1 is for TX*/
#define CAN_BIT_RAM_LOST         ((u32)0x00000001 << 4)            /*!< R/W/ES 0x0  data have lost in the read buffer, means more than one frame have beed received since last read; only avaliable when rx FIFO is disabled*/
#define CAN_MASK_RAM_DLC         ((u32)0x0000000F << 0)            /*!< R/W/ES 0x0  data length*/
#define CAN_RAM_DLC(x)           (((u32)((x) & 0x0000000F) << 0))
#define CAN_GET_RAM_DLC(x)       ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup CAN_RAM_CMD
  * @brief CAN message buffer access control
  * @{
  */
#define CAN_BIT_RAM_START      ((u32)0x00000001 << 31)          /*!< R/W/ES 0x0  start to exchange data with RAM, the bit will clear automatically after the data change is finshed.*/
#define CAN_BIT_RAM_DIR        ((u32)0x00000001 << 30)          /*!< R/W 0x0  0 is for read from RAM to CAN_RAM_* registers, 1 is for write from CAN_RAM_* to RAM*/
#define CAN_BIT_RAM_BUFFER_EN  ((u32)0x00000001 << 29)          /*!< R/W 0x0  enable the can_ram_acc_num specified message buffer for rx/tx*/
#define CAN_BIT_RAM_ACC_DATA15 ((u32)0x00000001 << 26)          /*!< R/W 0x0  access CAN_RAM_DATA_15 field, the field will exchange with RAM space*/
#define CAN_BIT_RAM_ACC_DATA14 ((u32)0x00000001 << 25)          /*!< R/W 0x0  access CAN_RAM_DATA_14 field, the field will exchange with RAM space*/
#define CAN_BIT_RAM_ACC_DATA13 ((u32)0x00000001 << 24)          /*!< R/W 0x0  access CAN_RAM_DATA_13 field, the field will exchange with RAM space*/
#define CAN_BIT_RAM_ACC_DATA12 ((u32)0x00000001 << 23)          /*!< R/W 0x0  access CAN_RAM_DATA_12 field, the field will exchange with RAM space*/
#define CAN_BIT_RAM_ACC_DATA11 ((u32)0x00000001 << 22)          /*!< R/W 0x0  access CAN_RAM_DATA_11 field, the field will exchange with RAM space*/
#define CAN_BIT_RAM_ACC_DATA10 ((u32)0x00000001 << 21)          /*!< R/W 0x0  access CAN_RAM_DATA_10 field, the field will exchange with RAM space*/
#define CAN_BIT_RAM_ACC_DATA9  ((u32)0x00000001 << 20)          /*!< R/W 0x0  access CAN_RAM_DATA_9 field, the field will exchange with RAM space*/
#define CAN_BIT_RAM_ACC_DATA8  ((u32)0x00000001 << 19)          /*!< R/W 0x0  access CAN_RAM_DATA_8 field, the field will exchange with RAM space*/
#define CAN_BIT_RAM_ACC_DATA7  ((u32)0x00000001 << 18)          /*!< R/W 0x0  access CAN_RAM_DATA_7 field, the field will exchange with RAM space*/
#define CAN_BIT_RAM_ACC_DATA6  ((u32)0x00000001 << 17)          /*!< R/W 0x0  access CAN_RAM_DATA_6 field, the field will exchange with RAM space*/
#define CAN_BIT_RAM_ACC_DATA5  ((u32)0x00000001 << 16)          /*!< R/W 0x0  access CAN_RAM_DATA_5 field, the field will exchange with RAM space*/
#define CAN_BIT_RAM_ACC_DATA4  ((u32)0x00000001 << 15)          /*!< R/W 0x0  access CAN_RAM_DATA_4 field, the field will exchange with RAM space*/
#define CAN_BIT_RAM_ACC_DATA3  ((u32)0x00000001 << 14)          /*!< R/W 0x0  access CAN_RAM_DATA_3 field, the field will exchange with RAM space*/
#define CAN_BIT_RAM_ACC_DATA2  ((u32)0x00000001 << 13)          /*!< R/W 0x0  access CAN_RAM_DATA_2 field, the field will exchange with RAM space*/
#define CAN_BIT_RAM_ACC_DATA1  ((u32)0x00000001 << 12)          /*!< R/W 0x0  access CAN_RAM_DATA_1 field, the field will exchange with RAM space*/
#define CAN_BIT_RAM_ACC_DATA0  ((u32)0x00000001 << 11)          /*!< R/W 0x0  access CAN_RAM_DATA_0 field, the field will exchange with RAM space*/
#define CAN_BIT_RAM_ACC_ARB    ((u32)0x00000001 << 10)          /*!< R/W 0x0  access CAN_RAM_ARB field, the field will exchange with RAM space*/
#define CAN_BIT_RAM_ACC_CS     ((u32)0x00000001 << 9)           /*!< R/W 0x0  access CAN_RAM_CS field, the field will exchange with RAM space*/
#define CAN_BIT_RAM_ACC_MASK   ((u32)0x00000001 << 8)           /*!< R/W 0x0  access CAN_RAM_MASK field, the field will exchange with RAM space*/
#define CAN_MASK_RAM_ACC_NUM   ((u32)0x000000FF << 0)           /*!< R/W 0x0  access buffer number*/
#define CAN_RAM_ACC_NUM(x)     (((u32)((x) & 0x000000FF) << 0))
#define CAN_GET_RAM_ACC_NUM(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup CAN_DBG_PORT
  * @brief debug register
  * @{
  */
#define CAN_BIT_DBG_EN     ((u32)0x00000001 << 8)           /*!< R/W 0x0  debug port enable*/
#define CAN_MASK_DBG_SEL   ((u32)0x000000FF << 0)           /*!< R/W 0x0  debug port selection*/
#define CAN_DBG_SEL(x)     (((u32)((x) & 0x000000FF) << 0))
#define CAN_GET_DBG_SEL(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup CAN_Exported_Types CAN Exported Types
  * @{
  */

/** @brief CAN Register Declaration
  */

typedef struct {
	__IO uint32_t CAN_CTL             ;  /*!< CAN BUS CONTROL REGISTER,  Address offset:0x000 */
	__I  uint32_t CAN_STS             ;  /*!< CAN BUS STATUS Register,  Address offset:0x004 */
	__I  uint32_t CAN_FIFO_STS        ;  /*!< CAN BUS FIFO STATUS Register,  Address offset:0x008 */
	__IO uint32_t CAN_BIT_TIMING      ;  /*!< CAN2.0 BIT TIMING Register,  Address offset:0x00C */
	__IO uint32_t CAN_FD_BIT_TIMING   ;  /*!< CAN FD DATA FIELD BIT TIMING Register,  Address offset:0x010 */
	__IO uint32_t CAN_FD_SSP_CAL      ;  /*!< CAN FD SSP CONFIGURATION Register,  Address offset:0x014 */
	__IO uint32_t CAN_INT_EN          ;  /*!< CAN INTERRUPT CONTROL Register,  Address offset:0x018 */
	__IO uint32_t CAN_MB_RXINT_EN     ;  /*!< CAN RX INTERRUPT FOR MESSAGE BUFFERS Register,  Address offset:0x01C */
	__IO uint32_t CAN_MB_TXINT_EN     ;  /*!< CAN TX INTERRUPT FOR MESSAGE BUFFERS Register,  Address offset:0x020 */
	__IO uint32_t CAN_INT_FLAG        ;  /*!< CAN INTERRUPT FLAGS Register,  Address offset:0x024 */
	__IO uint32_t CAN_ERR_STATUS      ;  /*!< CAN ERROR STATUS Register,  Address offset:0x028 */
	__IO uint32_t CAN_ERR_CNT_CTL     ;  /*!< CAN BUS ERROR COUNTER CONTROL Register,  Address offset:0x02C */
	__I  uint32_t CAN_ERR_CNT_STS     ;  /*!< CAN BUS ERROR COUNTER STATUS Register,  Address offset:0x030 */
	__IO uint32_t CAN_TX_ERROR_FLAG   ;  /*!< CAN TX ERROR FLAG FOR EACH MESSAGE BUFFE Register,  Address offset:0x034 */
	__IO uint32_t CAN_TX_DONE         ;  /*!< CAN TX DONE FOR MESSAGE BUFFERS Register,  Address offset:0x038 */
	__IO uint32_t CAN_RX_DONE         ;  /*!< CAN RX DONE FOR MESSAGE BUFFERS Register,  Address offset:0x03C */
	__IO uint32_t CAN_TIME_STAMP      ;  /*!< TIMESTAMP FUNCTION Register,  Address offset:0x040 */
	__IO uint32_t CAN_MB_TRIGGER      ;  /*!< CAN TX TRIGGER BY TIMER CONFIGURATION Register,  Address offset:0x044 */
	__IO uint32_t CAN_RXDMA_CFG       ;  /*!< RX DMA CONFIGURATION Register,  Address offset:0x048 */
	__I  uint32_t CAN_RX_DMA_DATA     ;  /*!< REGISTER FOR DMA ACCESS MESSAGE BUFFE Register,  Address offset:0x04C */
	__IO uint32_t CAN_SLEEP_MODE      ;  /*!< SLEEP MODE REQUEST AND WAKEUP PIN DIGITAL FILTER Register,  Address offset:0x050 */
	__IO uint32_t CAN_TEST            ;  /*!< TEST CONFIGURATION Register,  Address offset:0x054 */
	__IO uint32_t RSVD0[42]           ;  /*!< Reserved,  Address offset:0x058-0x0FF */
	__I  uint32_t CAN_MBx_STS[16]     ;  /*!< CAN MESSAGE BUFFER STATUS Register,  Address offset:0x100-0x13C */
	__IO uint32_t RSVD1[48]           ;  /*!< Reserved,  Address offset:0x140-0x1FF */
	__IO uint32_t CAN_MBx_CTRL[16]    ;  /*!< CAN MESSAGE BUFFER CONTROL Register,  Address offset:0x200-0x23C */
	__IO uint32_t RSVD2[44]           ;  /*!< Reserved,  Address offset:0x240-0x2EF */
	__IO uint32_t CAN_MB_BA_END       ;  /*!< Register,  Address offset:0x2F0 */
	__IO uint32_t RSVD3[3]            ;  /*!< Reserved,  Address offset:0x2F4-0x2FF */
	__IO uint32_t CAN_RAM_FDDATA_x[16];  /*!< CAN DATA Register,  Address offset:0x300-0x33C */
	__IO uint32_t CAN_RAM_ARB         ;  /*!< CAN MESSAGE BUFFER ARB FIELD CONFIGURATION Register,  Address offset:0x340 */
	__IO uint32_t CAN_RAM_MASK        ;  /*!< CAN MESSAGE BUFFER ARB FIELD MASK CONFIGURATION Register,  Address offset:0x344 */
	__IO uint32_t CAN_RAM_CS          ;  /*!< CAN MESSAGE TYPE CONTROL Register,  Address offset:0x348 */
	__IO uint32_t CAN_RAM_CMD         ;  /*!< CAN MESSAGE BUFFER ACCESS CONTROL Register,  Address offset:0x34C */
	__IO uint32_t RSVD4[42]           ;  /*!< Reserved,  Address offset:0x350-0x3F7 */
	__IO uint32_t CAN_DBG_PORT        ;  /*!< DEBUG REGISTER,  Address offset:0x3F8 */
	__IO uint32_t CAN_DUMMY           ;  /*!< DUMMY REGISTER,  Address offset:0x3FC */
} RCAN_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your definition here
/* Exported types --------------------------------------------------------*/
/** @addtogroup CAN_Exported_Types CAN Exported Types
  * @{
  */
/**
  * @brief  RCAN Mode Initialization structure definition
  */
typedef struct {
	u32	CAN_TimStampEn; 		/*!< Specifies the CAN stamp timer enable or disable.
                                      				This parameter can be ENABLE or DISABLE. */

	u32	CAN_TimStampDiv;		/*!< Specifies the CAN stamp timer divider.
                                      					This parameter can be a number between 0x01 and 0xff. */

	u32	CAN_AutoReTxEn;        		/*!< Specifies the message auto re-transmission configuration.
                                      				This parameter can be a ENABLE or DISABLE. */

	u32	CAN_TriSampleEn;			/*!< Specifies the tripple sampling configuration.
                                      				This parameter can be ENABLE or DISABLE. */

	u32	CAN_WorkMode;        		/*!< Specifies the CAN work mode.
                                      				This parameter can be of @ref RCAN_WORK_MODE_define. */

	u32	CAN_FDCrcMode;        		/*!< Specifies the CAN FD CRC mode.
                                      				This parameter can be of @ref RCAN_FD_CRC_MODE_define. */

	u32	CAN_ErrCntThreshold;        	/*!< Specifies the message length.
                                      				This parameter can be a number between 0x000 and 0x1ff*/

	u32	CAN_RxFifoEn;      			/*!< Specifies the CAN FIFO mode configure register.
                                      				This parameter can be ENABLE or DISABLE. */

	u32	CAN_BitPrescaler;			/*!< Specifies the bit timing prescaler.
                                      				This parameter can be a number between 0x1 and 0x100*/

	u32	CAN_SJW;				/*!< Specifies the CAN bit timing SJW.
                                      				This parameter can be a number between 0x1 and 0x8. */

	u32	CAN_TSEG1;				/*!< Specifies the CAN TSEG1.
                                      				This parameter can be a number between 0x1 and 0x10. */

	u32	CAN_TSEG2;				/*!< Specifies the CAN TSEG2.
                                      				This parameter can be a number between 0x1 and 0x10. */
} RCAN_InitTypeDef;

/**
  * @}
  */

/**
  * @}
  */

/**
  * @brief  CAN Tx message descriptor structure definition
  */
typedef struct {

	u32 ProtocolType;	/*!< Specifies the protocol type of frame for the message that will
                        	be transmitted. This parameter can be a value of
                        	@ref RCAN_PROTOCOL_TYPE_define */

	u32 StdId;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF. */

	u32 ExtId;  /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF. */

	u32 MsgBufferIdx;     /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0xf. */

	u32 IDE;     /*!< Specifies the type of identifier for the message that
                        will be transmitted. This parameter can be a value
                        of @ref RCAN_ID_TYPE_define */

	u32 RTR;     /*!< Specifies the type of frame for the message that will
                        be transmitted. This parameter can be a value of
                        @ref RCAN_FRAME_TYPE_define */

	u32 DLC;     /*!< Specifies the length of the frame that will be
                        transmitted. This parameter can be a value between
                        0 to 8 */

	u32 ESI;     /*!< Specifies the FD frame esi bit, indicate error passive of message sending node,
  		read bit, should always write 0, hardware automatic fill*/

	u32 BRS;     /*!< Specifies the FD frame brs bit, indicate switch bit timing*/

	u32 EDL;     /*!< Specifies the FD frame edl bit, indicate it's a FD frame*/

	__attribute__((aligned(4)))
	u8 Data[64]; /*!< Contains the data to be transmitted. It ranges from 0
                        to 0xFF. */

	u32 IDE_MASK;     /*!< Specifies the IDE MASK, ####*/

	u32 ID_MASK;     /*!< Specifies the ID MASK, ####*/

	u32 RTR_Mask;     /*!< Specifies the RTR MASK, ####*/
} RCAN_TxMsgTypeDef;

/**
  * @}
  */

/**
  * @brief  CAN Rx message descriptor structure definition
  */
typedef struct {
	u32 ProtocolType;	/*!< Specifies the protocol type of frame for the message that will
                        	be transmitted. This parameter can be a value of
                        	@ref RCAN_PROTOCOL_TYPE_define */

	u32 StdId;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF, ####*/

	u32 ExtId;  /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF, #### */

	u32 MsgBufferIdx;     /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0xf, ####*/

	u32 IDE;     /*!< Specifies the type of identifier for the message that
                        will be transmitted. This parameter can be a value
                        of @ref RCAN_ID_TYPE_define , ####*/

	u32 RTR;     /*!< Specifies the type of frame for the message that will
                        be transmitted. This parameter can be a value of
                        @ref RCAN_FRAME_TYPE_define, ####*/

	u32 DLC;     /*!< Specifies the length of the frame that will be
                        transmitted. This parameter can be a value between
                        0 to 8*/
	u32 ESI;     /*!< Specifies the FD frame esi bit, indicate error passive of message sending node,
  		read bit, should always write 0, hardware automatic fill*/

	u32 BRS;     /*!< Specifies the FD frame brs bit, indicate switch bit timing*/

	u32 EDL;     /*!< Specifies the FD frame edl bit, indicate it's a FD frame*/

	__attribute__((aligned(4)))
	u8 Data[64]; /*!< Contains the data to be transmitted. It ranges from 0
                        to 0xFF. */

	u32 ID_MASK;     /*!< Specifies the ID MASK*/

	u32 RTR_Mask;     /*!< Specifies the RTR MASK, refer to RCAN_RTR_MASK_TYPE_define*/

	u32 IDE_Mask;     /*!< Specifies the IDE MASK, refer to RCAN_IDE_MASK_TYPE_define*/

	u32 RxTimStamp;      /*!< Specifies the rx time stamp */

	u32 RxLost;		     /*!< Specifies the rx lost times */
} RCAN_RxMsgTypeDef;
/**
  * @}
  */


/* Exported constants --------------------------------------------------------*/
/** @defgroup CAN_Exported_Constants CAN Exported Constants
  * @{
  */

/** @defgroup RCAN_WORK_MODE_define
  * @{
  */
#define RCAN_NORMAL_MODE					        ((u32)0x00000000)
#define RCAN_SILENCE_MODE					        ((u32)0x00000001)
#define RCAN_EXT_LOOPBACK_MODE					  ((u32)0x00000002)
#define RCAN_INT_LOOPBACK_MODE            ((u32)0x00000003)
#define IS_RCAN_WORK_MODE(MODE) (((MODE) == RCAN_NORMAL_MODE) || \
									((MODE) == RCAN_SILENCE_MODE) || \
									((MODE) == RCAN_EXT_LOOPBACK_MODE) || \
									((MODE) == RCAN_INT_LOOPBACK_MODE))

/**
  * @}
  */

/** @defgroup RCAN_PROTOCOL_TYPE_define
  * @{
  */
#define RCAN_CAN20_PROTOCOL_FRAME			((u32)0x00000000)
#define RCAN_FD_PROTOCOL_FRAME				((u32)0x00000001)
#define IS_RCAN_PROTOCOL_TYPE(TYPE) (((TYPE) == RCAN_CAN20_PROTOCOL_FRAME) || \
									((TYPE) == RCAN_FD_PROTOCOL_FRAME))

/**
  * @}
  */

/** @defgroup RCAN_ID_TYPE_define
  * @{
  */
#define RCAN_STANDARD_FRAME				((u32)0x00000000)
#define RCAN_EXTEND_FRAME					((u32)0x20000000)
#define IS_RCAN_ID_TYPE(TYPE) (((TYPE) == RCAN_STANDARD_FRAME) || \
									((TYPE) == RCAN_EXTEND_FRAME))

/**
  * @}
  */

/** @defgroup RCAN_FD_CRC_MODE_define
  * @{
  */
#define RCAN_ISO_CRC						((u32)0x00000000)
#define RCAN_NON_ISO_CRC					((u32)0x00000100)
#define IS_RCAN_CRC_TYPE(TYPE) (((TYPE) == RCAN_ISO_CRC) || \
									((TYPE) == RCAN_NON_ISO_CRC))

/**
  * @}
  */

/** @defgroup RCAN_FD_BIT_TIMING_SWITCH_TYPE_define
  * @{
  */
#define RCAN_FD_SWITCH_BIT_TIMING			((u32)0x00000200)
#define RCAN_FD_NOT_SWITCH_BIT_TIMING		((u32)0x00000000)
#define IS_RCAN_FD_BIT_RATE_SWITCH_TYPE(TYPE) (((TYPE) == RCAN_FD_NOT_SWITCH_BIT_TIMING) || \
									((TYPE) == RCAN_FD_SWITCH_BIT_TIMING))

/**
  * @}
  */

/** @defgroup RCAN_FD_ER_STS_INDICATOR_TYPE_define
  * @{
  */
#define RCAN_FD_ERROR_ACTIVE_NODE		((u32)0x00000000)
#define RCAN_FD_ERROR_PASSIVE_NODE		((u32)0x00000400)
#define IS_RCAN_FD_ER_NODE_TYPE(TYPE) (((TYPE) == RCAN_FD_ERROR_ACTIVE_NODE) || \
									((TYPE) == RCAN_FD_ERROR_PASSIVE_NODE))
/**
  * @}
  */

/** @defgroup RCAN_PROTOCOL_FRAME_TYPE_define
  * @{
  */
#define RCAN_FD_FRAME_TYPE					((u32)0x00000100)
#define RCAN_NOT_FD_FRAME_TYPE				((u32)0x00000000)
#define IS_RCAN_PRO_FRAEME_TYPE(TYPE) (((TYPE) == RCAN_FD_FRAME_TYPE) || \
									((TYPE) == RCAN_NOT_FD_FRAME_TYPE))

/**
  * @}
  */

/** @defgroup RCAN_FRAME_TYPE_define
  * @{
  */
#define RCAN_REMOTE_FRAME				((u32)0x40000000)
#define RCAN_DATA_FRAME					((u32)0x00000000)
#define IS_RCAN_FRAME_TYPE(TYPE) (((TYPE) == RCAN_REMOTE_FRAME) || \
									((TYPE) == RCAN_DATA_FRAME))

/**
  * @}
  */

/** @defgroup RCAN_RTR_MASK_TYPE_define
  * @{
  */
#define RCAN_RTR_BIT_MASK				((u32)0x40000000)
#define RCAN_RTR_NIT_NOT_MASK			((u32)0x00000000)
#define IS_RCAN_RTR_BIT_MASK_TYPE(TYPE) (((TYPE) == RCAN_RTR_BIT_MASK) || \
									((TYPE) == RCAN_RTR_NIT_NOTE_MASK))

/**
  * @}
  */

/** @defgroup RCAN_IDE_MASK_TYPE_define
  * @{
  */
#define RCAN_IDE_BIT_MASK				((u32)0x20000000)
#define RCAN_IDE_NIT_NOT_MASK			((u32)0x00000000)
#define IS_RCAN_IDE_BIT_MASK_TYPE(TYPE) (((TYPE) == RCAN_IDE_BIT_MASK) || \
									((TYPE) == RCAN_IDE_NIT_NOT_MASK))

/**
  * @}
  */

/** @defgroup RCAN_IDE_MASK_TYPE_define
  * @{
  */
#define CAN_BIT_RAM_ACC_DATA_MASK  (CAN_BIT_RAM_ACC_DATA15 | CAN_BIT_RAM_ACC_DATA14 | CAN_BIT_RAM_ACC_DATA13 | CAN_BIT_RAM_ACC_DATA12 | \
                                    CAN_BIT_RAM_ACC_DATA11 | CAN_BIT_RAM_ACC_DATA10 | CAN_BIT_RAM_ACC_DATA9 | CAN_BIT_RAM_ACC_DATA8 | \
                                    CAN_BIT_RAM_ACC_DATA7 | CAN_BIT_RAM_ACC_DATA6 | CAN_BIT_RAM_ACC_DATA5 | CAN_BIT_RAM_ACC_DATA4 | \
                                    CAN_BIT_RAM_ACC_DATA3 | CAN_BIT_RAM_ACC_DATA2 | CAN_BIT_RAM_ACC_DATA1 | CAN_BIT_RAM_ACC_DATA0)

/**
  * @}
  */

/** @defgroup RCAN_MESSAGE_BUFFER_STATUS_define
  * @{
  */
#define RCAN_FRAME_PENDING_TX			((u32)0x00000001)
#define RCAN_FRAME_FINISAH_TX			((u32)0x00000002)
#define RCAN_FRAME_OVWRITR_SEND_TX	((u32)0x00000003)
#define RCAN_FRAME_PENDING_RX			((u32)0x00000004)
#define RCAN_FRAME_FINISH_RX			((u32)0x0000000C)

#define IS_RCAN_MSG_STS_TYPE(TYPE) (((TYPE) == RCAN_FRAME_PENDING_TX) || \
									((TYPE) == RCAN_FRAME_FINISAH_TX) || \
									((TYPE) == RCAN_FRAME_OVWRITR_SEND_TX) || \
									((TYPE) == RCAN_SILENCE_MODE) || \
									((TYPE) == RCAN_FRAME_PENDING_RX))

/**
  * @}
  */

/** @defgroup RCAN_INTERRUPTS_define
  * @{
  */
#define RCAN_TX_INT					((u32)0x00000001)
#define RCAN_RX_INT					((u32)0x00000002)
#define RCAN_ERR_INT				((u32)0x00000004)
#define RCAN_WKUP_INT				((u32)0x00000008)
#define RCAN_BUSOFF_INT				((u32)0x00000010)
#define RCAN_RAM_MOVE_DONE_INT		((u32)0x00000020)
#define IS_RCAN_CONFIG_IT(IT) (((IT) == RCAN_TX_INT) || \
								((IT) == RCAN_RX_INT) || \
									((IT) == RCAN_ERR_INT)|| \
									((IT) == RCAN_WKUP_INT)|| \
									((IT) == RCAN_RAM_MOVE_DONE_INT)|| \
									((IT) == RCAN_BUSOFF_INT))

#define IS_RCAN_CLEAR_IT(IT) ((((IT) & (u32)0xFFFFFFC0) == 0x00) && ((IT) != 0x00))

/**
  * @}
  */

/** @defgroup RCAN_type_define
  * @{
  */
#define IS_RCAN_ALL_PERIPH(PERIPH)   (((PERIPH) == RCAN0) || \
									  ((PERIPH) == RCAN1) || \
									  ((PERIPH) == RCAN0_S) || \
									  ((PERIPH) == RCAN1_S))

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
/** @defgroup CAN_Exported_Functions CAN Exported Functions
  * @{
  */
/** @defgroup RCAN_global_functions
  * @{
  */

void RCAN_StructInit(RCAN_InitTypeDef *RCAN_InitStruct);
void RCAN_Init(RCAN_TypeDef *RCANx, RCAN_InitTypeDef *RCAN_InitStruct);

void RCAN_LpFilterCmd(RCAN_TypeDef *RCANx, u32 NewState);
void RCAN_LpFilterConfig(RCAN_TypeDef *RCANx, u32 FltNum);

void RCAN_BusCmd(RCAN_TypeDef *RCANx, u32 NewState);
u32 RCAN_BusStatusGet(RCAN_TypeDef *RCANx);

u32 RCAN_FifoStatusGet(RCAN_TypeDef *RCANx);
u32 RCAN_FifoLvlGet(RCAN_TypeDef *RCANx);
void RCAN_ReadRxMsgFromFifo(RCAN_TypeDef *RCANx, RCAN_RxMsgTypeDef *RxMsg);

u32 RCAN_TXErrCntGet(RCAN_TypeDef *RCANx);
u32 RCAN_RXErrCntGet(RCAN_TypeDef *RCANx);
void RCAN_TXErrCntClear(RCAN_TypeDef *RCANx);
void RCAN_RXErrCntClear(RCAN_TypeDef *RCANx);
u32 RCAN_RXErrCntSTS(RCAN_TypeDef *RCANx);


void RCAN_WriteMsg(RCAN_TypeDef *RCANx, RCAN_TxMsgTypeDef *TxMsg);
void RCAN_SetRxMsgBuf(RCAN_TypeDef *RCANx, RCAN_RxMsgTypeDef *RxMsg);
void RCAN_ReadMsg(RCAN_TypeDef *RCANx, RCAN_RxMsgTypeDef *RxMsg);
void RCAN_TxAutoReply(RCAN_TypeDef *RCANx, RCAN_TxMsgTypeDef *TxMsg);
void RCAN_RxAutoReply(RCAN_TypeDef *RCANx, RCAN_RxMsgTypeDef *RxMsg);

void RCAN_Cmd(RCAN_TypeDef *RCANx, u32 NewState);

/**
  * @}
  */


/** @defgroup RCAN_Interrupt_status_functions
  * @{
  */

void RCAN_INTConfig(RCAN_TypeDef *RCANx, u32 RCAN_IT, u32 NewState);
u32 RCAN_GetINTStatus(RCAN_TypeDef *RCANx);
void RCAN_ClearINT(RCAN_TypeDef *RCANx, u32 RCAN_IT);
void RCAN_ClearAllINT(RCAN_TypeDef *RCANx);
u32 RCAN_GetErrStatus(RCAN_TypeDef *RCANx);
void RCAN_ClearErrStatus(RCAN_TypeDef *RCANx, u32 ERR_STS);

void RCAN_RxMsgBufINTConfig(RCAN_TypeDef *RCANx, u32 BUF_IT, u32 NewState);
void RCAN_TxMsgBufINTConfig(RCAN_TypeDef *RCANx, u32 BUF_IT, u32 NewState);

u32 RCAN_TxMsgBufErrGet(RCAN_TypeDef *RCANx);
void RCAN_TxMsgBufErrClear(RCAN_TypeDef *RCANx, u32 ERR_FLAG);

u32 RCAN_MsgBufStatusRegGet(RCAN_TypeDef *RCANx, u32 MsgBufIdx);
u32 RCAN_MsgBufStatusGet(RCAN_TypeDef *RCANx, u32 MsgBufIdx);
u32 RCAN_CheckMsgBufAvailable(RCAN_TypeDef *RCANx, u32 MsgBufIdx);

u32 RCAN_TxDoneStatusGet(RCAN_TypeDef *RCANx);
u32 RCAN_MsgBufTxDoneStatusGet(RCAN_TypeDef *RCANx, u32 MsgBufIdx);
u32 RCAN_RxDoneStatusGet(RCAN_TypeDef *RCANx);
u32 RCAN_MsgBufRxDoneStatusGet(RCAN_TypeDef *RCANx, u32 MsgBufIdx);

void RCAN_TxDoneStatusClear(RCAN_TypeDef *RCANx, u32 Status);
void RCAN_MsgBufTxDoneStatusClear(RCAN_TypeDef *RCANx, u32 MsgBufIdx);

void RCAN_RxDoneStatusClear(RCAN_TypeDef *RCANx, u32 Status);
void RCAN_MsgBufRxDoneStatusClear(RCAN_TypeDef *RCANx, u32 MsgBufIdx);


void RCAN_TxMsgTriggerCmd(RCAN_TypeDef *RCANx, u32 State);
void RCAN_TxMsgTriggerConfig(RCAN_TypeDef *RCANx, u32 CloseOffset, u32 Begin);

void RCAN_RxDmaDestBaseAddrConfig(RCAN_TypeDef *RCANx, u32 Addr);
u32 RCAN_GetRxDmaData(RCAN_TypeDef *RCANx);

void RCAN_MsgCtrlRegConfig(RCAN_TypeDef *RCANx, u32 MsgIdx, u32 RxDmaEn, u32 DmaOffset);
void RCAN_MsgBaseAddrConfig(RCAN_TypeDef *RCANx, u32 MsgIdx, u32 BaseAddr);
void RCAN_MsgBaseAddrEndReg(RCAN_TypeDef *RCANx, u32 EndAddr);

void RCAN_RamBufferMapConfig(RCAN_TypeDef *RCANx, u32 *pPara);
/**
  * @}
  */

/** @defgroup RCAN_dma_functions
  * @{
  */
bool RCAN_RXGDMA_Init(u8 Index, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pRxBuf, int RxCount);
/**
  * @}
  */

/**
  * @}
  */
struct CAN_DevTable {
	RCAN_TypeDef *RCANx;
	IRQn_Type	IrqNum;
	u32 Rx_HandshakeInterface;
};

#define MAX_CAN_INDEX			(2)

#define RCAN_MESSAGE_BUFFER_LEN	            (5)

#define RCAN_MESSAGE_BUFFER_SIZE	        (16)

#define RCAN_MESSAGE_FIFO_SIZE		        (4)

#define RCAN_RX_FIFO_READ_MSG_IDX	        (12)

extern const struct CAN_DevTable CAN_DEV_TABLE[MAX_CAN_INDEX];


#endif