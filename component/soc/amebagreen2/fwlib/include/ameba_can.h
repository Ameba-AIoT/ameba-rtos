#ifndef _RL7005_CAN_H_
#define _RL7005_CAN_H_

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
#define CAN_BIT_RXDMA_EN      ((u32)0x00000001 << 7) /*!< R/W 0x0  rx dma enable*/
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

/** @defgroup CAN_RAM_DATA_x
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
	__IO uint32_t CAN_CTL           ;  /*!< CAN BUS CONTROL REGISTER,  Address offset:0x000 */
	__I  uint32_t CAN_STS           ;  /*!< CAN BUS STATUS Register,  Address offset:0x004 */
	__I  uint32_t CAN_FIFO_STS      ;  /*!< CAN BUS FIFO STATUS Register,  Address offset:0x008 */
	__IO uint32_t CAN_BIT_TIMING    ;  /*!< CAN2.0 BIT TIMING Register,  Address offset:0x00C */
	__IO uint32_t RSVD0[2]          ;  /*!< Reserved,  Address offset:0x010-0x017 */
	__IO uint32_t CAN_INT_EN        ;  /*!< CAN INTERRUPT CONTROL Register,  Address offset:0x018 */
	__IO uint32_t CAN_MB_RXINT_EN   ;  /*!< CAN RX INTERRUPT FOR MESSAGE BUFFERS Register,  Address offset:0x01C */
	__IO uint32_t CAN_MB_TXINT_EN   ;  /*!< CAN TX INTERRUPT FOR MESSAGE BUFFERS Register,  Address offset:0x020 */
	__IO uint32_t CAN_INT_FLAG      ;  /*!< CAN INTERRUPT FLAGS Register,  Address offset:0x024 */
	__IO uint32_t CAN_ERR_STATUS    ;  /*!< CAN ERROR STATUS Register,  Address offset:0x028 */
	__IO uint32_t CAN_ERR_CNT_CTL   ;  /*!< CAN BUS ERROR COUNTER CONTROL Register,  Address offset:0x02C */
	__I  uint32_t CAN_ERR_CNT_STS   ;  /*!< CAN BUS ERROR COUNTER STATUS Register,  Address offset:0x030 */
	__IO uint32_t CAN_TX_ERROR_FLAG ;  /*!< CAN TX ERROR FLAG FOR EACH MESSAGE BUFFE Register,  Address offset:0x034 */
	__IO uint32_t CAN_TX_DONE       ;  /*!< CAN TX DONE FOR MESSAGE BUFFERS Register,  Address offset:0x038 */
	__IO uint32_t CAN_RX_DONE       ;  /*!< CAN RX DONE FOR MESSAGE BUFFERS Register,  Address offset:0x03C */
	__IO uint32_t CAN_TIME_STAMP    ;  /*!< TIMESTAMP FUNCTION Register,  Address offset:0x040 */
	__IO uint32_t CAN_MB_TRIGGER    ;  /*!< CAN TX TRIGGER BY TIMER CONFIGURATION Register,  Address offset:0x044 */
	__IO uint32_t CAN_RXDMA_CFG     ;  /*!< RX DMA CONFIGURATION Register,  Address offset:0x048 */
	__I  uint32_t CAN_RX_DMA_DATA   ;  /*!< REGISTER FOR DMA ACCESS MESSAGE BUFFE Register,  Address offset:0x04C */
	__IO uint32_t CAN_SLEEP_MODE    ;  /*!< SLEEP MODE REQUEST AND WAKEUP PIN DIGITAL FILTER Register,  Address offset:0x050 */
	__IO uint32_t CAN_TEST          ;  /*!< TEST CONFIGURATION Register,  Address offset:0x054 */
	__IO uint32_t RSVD1[42]         ;  /*!< Reserved,  Address offset:0x058-0x0FF */
	__I  uint32_t CAN_MBx_STS[16]   ;  /*!< CAN MESSAGE BUFFER STATUS Register,  Address offset:0x100-0x13C */
	__IO uint32_t RSVD2[48]         ;  /*!< Reserved,  Address offset:0x140-0x1FF */
	__IO uint32_t CAN_MBx_CTRL[16]  ;  /*!< CAN MESSAGE BUFFER CONTROL Register,  Address offset:0x200-0x23C */
	__IO uint32_t RSVD3[44]         ;  /*!< Reserved,  Address offset:0x240-0x2EF */
	__IO uint32_t CAN_MB_BA_END     ;  /*!< Register,  Address offset:0x2F0 */
	__IO uint32_t RSVD4[3]          ;  /*!< Reserved,  Address offset:0x2F4-0x2FF */
	__IO uint32_t CAN_RAM_DATA_x[16];  /*!< CAN DATA Register,  Address offset:0x300-0x33C */
	__IO uint32_t CAN_RAM_ARB       ;  /*!< CAN MESSAGE BUFFER ARB FIELD CONFIGURATION Register,  Address offset:0x340 */
	__IO uint32_t CAN_RAM_MASK      ;  /*!< CAN MESSAGE BUFFER ARB FIELD MASK CONFIGURATION Register,  Address offset:0x344 */
	__IO uint32_t CAN_RAM_CS        ;  /*!< CAN MESSAGE TYPE CONTROL Register,  Address offset:0x348 */
	__IO uint32_t CAN_RAM_CMD       ;  /*!< CAN MESSAGE BUFFER ACCESS CONTROL Register,  Address offset:0x34C */
	__IO uint32_t RSVD5[42]         ;  /*!< Reserved,  Address offset:0x350-0x3F7 */
	__IO uint32_t CAN_DBG_PORT      ;  /*!< DEBUG REGISTER,  Address offset:0x3F8 */
	__IO uint32_t CAN_DUMMY         ;  /*!< DUMMY REGISTER,  Address offset:0x3FC */
} CAN_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your definition here
/* Exported types --------------------------------------------------------*/
/** @addtogroup CAN_Exported_Types CAN Exported Types
  * @{
  */
/**
 * @brief  CAN Timing Initialization structure definition
 *
 * This struct is used to pass bus timing values to the configuration and
 * bitrate calculation functions.
 *
 * The propagation segment represents the time of the signal propagation. Phase
 * segment 1 and phase segment 2 define the sampling point. The ``prop_seg`` and
 * ``phase_seg1`` values affect the sampling point in the same way and some
 * controllers only have a register for the sum of those two. The sync segment
 * always has a length of 1 time quantum (see below).
 *
 * @code{.text}
 *
 * +---------+----------+------------+------------+
 * |sync_seg | prop_seg | phase_seg1 | phase_seg2 |
 * +---------+----------+------------+------------+
 *                                   ^
 *                             Sampling-Point
 *
 * @endcode
 *
 * 1 time quantum (tq) has the length of 1/(core_clock / prescaler). The bitrate
 * is defined by the core clock divided by the prescaler and the sum of the
 * segments:
 *
 *   br = (core_clock / prescaler) / (1 + prop_seg + phase_seg1 + phase_seg2)
 *
 * The Synchronization Jump Width (SJW) defines the amount of time quanta the
 * sample point can be moved. The sample point is moved when resynchronization
 * is needed.
 */
typedef struct {
	uint8_t  SJW;           /*!< Spec: 1 to 4. Synchronisation jump width. */
	uint8_t  PropSeg;       /*!< Spec: 1 to 8. Propagation segment. */
	uint8_t  PhaseSeg1;     /*!< Spec: 1 to 8. Phase segment 1. */
	uint8_t  PhaseSeg2;     /*!< Spec: 2 to 8. Phase segment 2. */
	uint16_t Prescaler;     /*!< Spec: 1 to 1024. Prescaler value. */
} CAN_BitTimingTypeDef;
/**
  * @brief  CAN Mode Initialization structure definition
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
                                      				This parameter can be of @ref CAN_WORK_MODE_define. */

	u32	CAN_FDCrcMode;        		/*!< Specifies the CAN FD CRC mode.
                                      				This parameter can be of @ref CAN_FD_CRC_MODE_define. */

	u32	CAN_ErrCntThreshold;        	/*!< Specifies the message length.
                                      				This parameter can be a number between 0x000 and 0x1ff*/

	u32	CAN_RxFifoEn;      			/*!< Specifies the CAN FIFO mode configure register.
                                      				This parameter can be ENABLE or DISABLE. */
	CAN_BitTimingTypeDef CAN_Timing; /*!< Specifies the CAN BIT_TIMING register.
  	                                    This parameter can be of @ref CAN_BitTimingTypeDef. */

} CAN_InitTypeDef;

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
                        	@ref CAN_PROTOCOL_TYPE_define */

	u32 StdId;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF. */

	u32 ExtId;  /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF. */

	u32 MsgBufferIdx;     /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0xf. */

	u32 IDE;     /*!< Specifies the type of identifier for the message that
                        will be transmitted. This parameter can be a value
                        of @ref CAN_ID_TYPE_define */

	u32 RTR;     /*!< Specifies the type of frame for the message that will
                        be transmitted. This parameter can be a value of
                        @ref CAN_FRAME_TYPE_define */

	u32 DLC;     /*!< Specifies the length of the frame that will be
                        transmitted. This parameter can be a value between
                        0 to 8 */
	union {
		/** Payload data accessed as unsigned 8 bit values. */
		uint8_t Data[64];
		/** Payload data accessed as unsigned 32 bit values. */
		uint32_t Data_32[16];
	};
} CAN_TxMsgTypeDef;

/**
  * @}
  */

/**
  * @brief  CAN Rx message descriptor structure definition
  */
typedef struct {
	u32 ProtocolType;	/*!< Specifies the protocol type of frame for the message that will
                        	be transmitted. This parameter can be a value of
                        	@ref CAN_PROTOCOL_TYPE_define */

	u32 StdId;  /*!< Specifies the standard identifier.
                        This parameter can be a value between 0 to 0x7FF, ####*/

	u32 ExtId;  /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0x1FFFFFFF, #### */

	u32 MsgBufferIdx;     /*!< Specifies the extended identifier.
                        This parameter can be a value between 0 to 0xf, ####*/

	u32 IDE;     /*!< Specifies the type of identifier for the message that
                        will be transmitted. This parameter can be a value
                        of @ref CAN_ID_TYPE_define , ####*/

	u32 RTR;     /*!< Specifies the type of frame for the message that will
                        be transmitted. This parameter can be a value of
                        @ref CAN_FRAME_TYPE_define, ####*/

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

	u32 RTR_Mask;     /*!< Specifies the RTR MASK, refer to CAN_RTR_MASK_TYPE_define*/

	u32 IDE_Mask;     /*!< Specifies the IDE MASK, refer to CAN_IDE_MASK_TYPE_define*/

	u32 RxTimStamp;      /*!< Specifies the rx time stamp */

	u32 RxLost;		     /*!< Specifies the rx lost times */
} CAN_RxMsgTypeDef;
/**
  * @}
  */


/* Exported constants --------------------------------------------------------*/
/** @defgroup CAN_Exported_Constants CAN Exported Constants
  * @{
  */

/** @defgroup CAN_WORK_MODE_define
  * @{
  */
#define CAN_NORMAL_MODE					          ((u32)0x00000000)
#define CAN_SILENCE_MODE					        ((u32)0x00000000)
#define CAN_EXT_LOOPBACK_MODE					    ((u32)0x00000001)
#define CAN_INT_LOOPBACK_MODE             ((u32)0x00000002)
#define IS_CAN_WORK_MODE(MODE) (((MODE) == CAN_NORMAL_MODE) || \
									((MODE) == CAN_SILENCE_MODE) || \
									((MODE) == CAN_EXT_LOOPBACK_MODE) || \
									((MODE) == CAN_INT_LOOPBACK_MODE))

/**
  * @}
  */

/** @defgroup CAN_PROTOCOL_TYPE_define
  * @{
  */
#define CAN_CAN20_PROTOCOL_FRAME			((u32)0x00000000)
#define CAN_FD_PROTOCOL_FRAME				((u32)0x00000001)
#define IS_CAN_PROTOCOL_TYPE(TYPE) (((TYPE) == CAN_CAN20_PROTOCOL_FRAME) || \
									((TYPE) == CAN_FD_PROTOCOL_FRAME))

/**
  * @}
  */

/** @defgroup CAN_ID_TYPE_define
  * @{
  */
#define CAN_STANDARD_FRAME				((u32)0x00000000)
#define CAN_EXTEND_FRAME					((u32)0x20000000)
#define IS_CAN_ID_TYPE(TYPE) (((TYPE) == CAN_STANDARD_FRAME) || \
									((TYPE) == CAN_EXTEND_FRAME))

/**
  * @}
  */

/** @defgroup CAN_FD_CRC_MODE_define
  * @{
  */
#define CAN_ISO_CRC						((u32)0x00000000)
#define CAN_NON_ISO_CRC					((u32)0x00000100)
#define IS_CAN_CRC_TYPE(TYPE) (((TYPE) == CAN_ISO_CRC) || \
									((TYPE) == CAN_NON_ISO_CRC))

/**
  * @}
  */

/** @defgroup CAN_FD_BIT_TIMING_SWITCH_TYPE_define
  * @{
  */
#define CAN_FD_SWITCH_BIT_TIMING			((u32)0x00000200)
#define CAN_FD_NOT_SWITCH_BIT_TIMING		((u32)0x00000000)
#define IS_CAN_FD_BIT_RATE_SWITCH_TYPE(TYPE) (((TYPE) == CAN_FD_NOT_SWITCH_BIT_TIMING) || \
									((TYPE) == CAN_FD_SWITCH_BIT_TIMING))

/**
  * @}
  */

/** @defgroup CAN_FD_ER_STS_INDICATOR_TYPE_define
  * @{
  */
#define CAN_FD_ERROR_ACTIVE_NODE		((u32)0x00000000)
#define CAN_FD_ERROR_PASSIVE_NODE		((u32)0x00000400)
#define IS_CAN_FD_ER_NODE_TYPE(TYPE) (((TYPE) == CAN_FD_ERROR_ACTIVE_NODE) || \
									((TYPE) == CAN_FD_ERROR_PASSIVE_NODE))
/**
  * @}
  */

/** @defgroup CAN_PROTOCOL_FRAME_TYPE_define
  * @{
  */
#define CAN_FD_FRAME_TYPE					((u32)0x00000100)
#define CAN_NOT_FD_FRAME_TYPE				((u32)0x00000000)
#define IS_CAN_PRO_FRAEME_TYPE(TYPE) (((TYPE) == CAN_FD_FRAME_TYPE) || \
									((TYPE) == CAN_NOT_FD_FRAME_TYPE))

/**
  * @}
  */

/** @defgroup CAN_FRAME_TYPE_define
  * @{
  */
#define CAN_REMOTE_FRAME				((u32)0x40000000)
#define CAN_DATA_FRAME					((u32)0x00000000)
#define IS_CAN_FRAME_TYPE(TYPE) (((TYPE) == CAN_REMOTE_FRAME) || \
									((TYPE) == CAN_DATA_FRAME))

/**
  * @}
  */

/** @defgroup CAN_RTR_MASK_TYPE_define
  * @{
  */
#define CAN_RTR_BIT_MASK				((u32)0x40000000)
#define CAN_RTR_NIT_NOT_MASK			((u32)0x00000000)
#define IS_CAN_RTR_BIT_MASK_TYPE(TYPE) (((TYPE) == CAN_RTR_BIT_MASK) || \
									((TYPE) == CAN_RTR_NIT_NOTE_MASK))

/**
  * @}
  */

/** @defgroup CAN_IDE_MASK_TYPE_define
  * @{
  */
#define CAN_IDE_BIT_MASK				((u32)0x20000000)
#define CAN_IDE_NIT_NOT_MASK			((u32)0x00000000)
#define IS_CAN_IDE_BIT_MASK_TYPE(TYPE) (((TYPE) == CAN_IDE_BIT_MASK) || \
									((TYPE) == CAN_IDE_NIT_NOT_MASK))

/**
  * @}
  */

/** @defgroup CAN_IDE_MASK_TYPE_define
  * @{
  */
#define CAN_BIT_RAM_ACC_DATA_MASK  (CAN_BIT_RAM_ACC_DATA15 | CAN_BIT_RAM_ACC_DATA14 | CAN_BIT_RAM_ACC_DATA13 | CAN_BIT_RAM_ACC_DATA12 | \
                                    CAN_BIT_RAM_ACC_DATA11 | CAN_BIT_RAM_ACC_DATA10 | CAN_BIT_RAM_ACC_DATA9 | CAN_BIT_RAM_ACC_DATA8 | \
                                    CAN_BIT_RAM_ACC_DATA7 | CAN_BIT_RAM_ACC_DATA6 | CAN_BIT_RAM_ACC_DATA5 | CAN_BIT_RAM_ACC_DATA4 | \
                                    CAN_BIT_RAM_ACC_DATA3 | CAN_BIT_RAM_ACC_DATA2 | CAN_BIT_RAM_ACC_DATA1 | CAN_BIT_RAM_ACC_DATA0)

/**
  * @}
  */

/** @defgroup CAN_MESSAGE_BUFFER_STATUS_define
  * @{
  */
#define CAN_FRAME_PENDING_TX			((u32)0x00000001)
#define CAN_FRAME_FINISAH_TX			((u32)0x00000002)
#define CAN_FRAME_OVWRITR_SEND_TX	((u32)0x00000003)
#define CAN_FRAME_PENDING_RX			((u32)0x00000004)
#define CAN_FRAME_FINISH_RX			((u32)0x0000000C)

#define IS_CAN_MSG_STS_TYPE(TYPE) (((TYPE) == CAN_FRAME_PENDING_TX) || \
									((TYPE) == CAN_FRAME_FINISAH_TX) || \
									((TYPE) == CAN_FRAME_OVWRITR_SEND_TX) || \
									((TYPE) == CAN_SILENCE_MODE) || \
									((TYPE) == CAN_FRAME_PENDING_RX))

/**
  * @}
  */

/** @defgroup CAN_INTERRUPTS_define
  * @{
  */
#define CAN_TX_INT					((u32)0x00000001)
#define CAN_RX_INT					((u32)0x00000002)
#define CAN_ERR_INT				((u32)0x00000004)
#define CAN_WKUP_INT				((u32)0x00000008)
#define CAN_BUSOFF_INT				((u32)0x00000010)
#define CAN_RAM_MOVE_DONE_INT		((u32)0x00000020)
#define IS_CAN_CONFIG_IT(IT) (((IT) == CAN_TX_INT) || \
								((IT) == CAN_RX_INT) || \
									((IT) == CAN_ERR_INT)|| \
									((IT) == CAN_WKUP_INT)|| \
									((IT) == CAN_RAM_MOVE_DONE_INT)|| \
									((IT) == CAN_BUSOFF_INT))

#define IS_CAN_CLEAR_IT(IT) ((((IT) & (u32)0xFFFFFFC0) == 0x00) && ((IT) != 0x00))

/**
  * @}
  */

/** @defgroup CAN_type_define
  * @{
  */
#define IS_CAN_ALL_PERIPH(PERIPH)   (((PERIPH) == CAN0) || \
									  ((PERIPH) == CAN1) || \
									  ((PERIPH) == CAN0_S) || \
									  ((PERIPH) == CAN1_S))

/**
  * @}
  */
/** @defgroup CAN_BIT_Timing
  * @{
  */
#define CAN_TIMING_MIN                                                                 \
	{                                                                                    \
		.SJW = 1,                                                                          \
		.PropSeg= 2,                                                                     \
		.PhaseSeg1 = 2,                                                                   \
		.PhaseSeg2 = 2,                                                                   \
		.Prescaler = 1,                                                                    \
	}

#define CAN_TIMING_MAX                                                                 \
	{                                                                                    \
		.SJW = 4,                                                                          \
		.PropSeg = 6,                                                                     \
		.PhaseSeg1 = 8,                                                                   \
		.PhaseSeg2 = 8,                                                                   \
		.Prescaler = 32,                                                                   \
	}

#define CAN_SYNC_SEG  1

#define CAN_CLAMP(val, low, high) (((val) <= (low)) ? (low) : MIN(val, high))

#define CAN_SAMPLE_POINT_MARGIN  50
/**
  * @}
  */
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */
/** @} */

/** @} */

/* Exported functions --------------------------------------------------------*/
/** @defgroup CAN_Exported_Functions CAN Exported Functions
  * @{
  */
/** @defgroup CAN_global_functions
  * @{
  */

void CAN_StructInit(CAN_InitTypeDef *CAN_InitStruct);
void CAN_Init(CAN_TypeDef *CANx, CAN_InitTypeDef *CAN_InitStruct);

void CAN_LpFilterCmd(CAN_TypeDef *CANx, u32 NewState);
void CAN_LpFilterConfig(CAN_TypeDef *CANx, u32 FltNum);

void CAN_BusCmd(CAN_TypeDef *CANx, u32 NewState);
u32 CAN_BusStatusGet(CAN_TypeDef *CANx);

u32 CAN_FifoStatusGet(CAN_TypeDef *CANx);
u32 CAN_FifoLvlGet(CAN_TypeDef *CANx);
void CAN_ReadRxMsgFromFifo(CAN_TypeDef *CANx, CAN_RxMsgTypeDef *RxMsg);

u32 CAN_TXErrCntGet(CAN_TypeDef *CANx);
u32 CAN_RXErrCntGet(CAN_TypeDef *CANx);
void CAN_TXErrCntClear(CAN_TypeDef *CANx);
void CAN_RXErrCntClear(CAN_TypeDef *CANx);
u32 CAN_RXErrCntSTS(CAN_TypeDef *CANx);


void CAN_WriteMsg(CAN_TypeDef *CANx, CAN_TxMsgTypeDef *TxMsg);
void CAN_SetRxMsgBuf(CAN_TypeDef *CANx, CAN_RxMsgTypeDef *RxMsg);
void CAN_ReadMsg(CAN_TypeDef *CANx, CAN_RxMsgTypeDef *RxMsg);
void CAN_TxAutoReply(CAN_TypeDef *CANx, CAN_TxMsgTypeDef *TxMsg);
void CAN_RxAutoReply(CAN_TypeDef *CANx, CAN_RxMsgTypeDef *RxMsg);

void CAN_Cmd(CAN_TypeDef *CANx, u32 NewState);

/**
  * @}
  */


/** @defgroup CAN_Interrupt_status_functions
  * @{
  */

void CAN_INTConfig(CAN_TypeDef *CANx, u32 CAN_IT, u32 NewState);
u32 CAN_GetINTStatus(CAN_TypeDef *CANx);
void CAN_ClearINT(CAN_TypeDef *CANx, u32 CAN_IT);
void CAN_ClearAllINT(CAN_TypeDef *CANx);
u32 CAN_GetErrStatus(CAN_TypeDef *CANx);
void CAN_ClearErrStatus(CAN_TypeDef *CANx, u32 ERR_STS);

void CAN_RxMsgBufINTConfig(CAN_TypeDef *CANx, u32 BUF_IT, u32 NewState);
void CAN_TxMsgBufINTConfig(CAN_TypeDef *CANx, u32 BUF_IT, u32 NewState);

u32 CAN_TxMsgBufErrGet(CAN_TypeDef *CANx);
void CAN_TxMsgBufErrClear(CAN_TypeDef *CANx, u32 ERR_FLAG);

u32 CAN_MsgBufStatusRegGet(CAN_TypeDef *CANx, u32 MsgBufIdx);
u32 CAN_MsgBufStatusGet(CAN_TypeDef *CANx, u32 MsgBufIdx);
u32 CAN_CheckMsgBufAvailable(CAN_TypeDef *CANx, u32 MsgBufIdx);

u32 CAN_TxDoneStatusGet(CAN_TypeDef *CANx);
u32 CAN_MsgBufTxDoneStatusGet(CAN_TypeDef *CANx, u32 MsgBufIdx);
u32 CAN_RxDoneStatusGet(CAN_TypeDef *CANx);
u32 CAN_MsgBufRxDoneStatusGet(CAN_TypeDef *CANx, u32 MsgBufIdx);

void CAN_TxDoneStatusClear(CAN_TypeDef *CANx, u32 Status);
void CAN_MsgBufTxDoneStatusClear(CAN_TypeDef *CANx, u32 MsgBufIdx);

void CAN_RxDoneStatusClear(CAN_TypeDef *CANx, u32 Status);
void CAN_MsgBufRxDoneStatusClear(CAN_TypeDef *CANx, u32 MsgBufIdx);


void CAN_TxMsgTriggerCmd(CAN_TypeDef *CANx, u32 State);
void CAN_TxMsgTriggerConfig(CAN_TypeDef *CANx, u32 CloseOffset, u32 Begin);

void CAN_RxDmaDestBaseAddrConfig(CAN_TypeDef *CANx, u32 Addr);
u32 CAN_GetRxDmaData(CAN_TypeDef *CANx);

void CAN_MsgCtrlRegConfig(CAN_TypeDef *CANx, u32 MsgIdx, u32 RxDmaEn, u32 DmaOffset);
void CAN_MsgBaseAddrConfig(CAN_TypeDef *CANx, u32 MsgIdx, u32 BaseAddr);
void CAN_MsgBaseAddrEndReg(CAN_TypeDef *CANx, u32 EndAddr);

void CAN_RamBufferMapConfig(CAN_TypeDef *CANx, uint32_t *AddrList);
bool CAN_RXGDMA_Init(u8 Index, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pRxBuf, int RxCount);
void CAN_CoreClockSet(void);
int CAN_CoreClockGet(uint32_t *Rate);
int CAN_CalcBitTiming(uint32_t BitRate, CAN_BitTimingTypeDef *Result);
/**
  * @}
  */
struct CAN_DevTable {
	CAN_TypeDef *CANx;
	IRQn_Type	IrqNum;
	u32 Rx_HandshakeInterface;
};

#define MAX_CAN_INDEX			(2)

#define CAN_MESSAGE_BUFFER_LEN	            (5)

#define CAN_MESSAGE_BUFFER_SIZE	        (16)

#define CAN_MESSAGE_FIFO_SIZE		        (4)

#define CAN_RX_FIFO_READ_MSG_IDX	        (12)

extern const struct CAN_DevTable CAN_DEV_TABLE[MAX_CAN_INDEX];

// Please add your definitions here

/* Exported Types ------------------------------------------------------------*/

/* Exported Constants --------------------------------------------------------*/

/* Exported Functions --------------------------------------------------------*/


/** @} */

/** @} */

// If other definitions do not need to be extracted to CHM document, add them here.
/* Other Definitions --------------------------------------------------------*/


/* MANUAL_GEN_END */

#endif