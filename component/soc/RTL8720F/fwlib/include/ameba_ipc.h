/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_IPC_H_
#define _AMEBA_IPC_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup IPC IPC
  * @brief IPC driver modules
  * @{
  */

/// @cond
/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup IPC_Register_Definitions IPC Register Definitions
  * @{
  */

/** @defgroup IPC_TX_DATA_CHx
  * @brief
  * @{
  */
#define IPC_MASK_TX0_CHx_DATA   ((u32)0xFFFFFFFF << 0)           /*!< R/WP 0x0  Different data have their own dedicated channels for writing data, for example,  IPC_TX_DATA_CH0  is the position for writing data to channel 0.*/
#define IPC_TX0_CHx_DATA(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define IPC_GET_TX0_CHx_DATA(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup IPC_TX_CH_ST
  * @brief
  * @{
  */
#define IPC_BIT_ICR_TX0_CH_ST15 ((u32)0x00000001 << 31)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_ch_st0.*/
#define IPC_BIT_ICR_TX0_CH_ST14 ((u32)0x00000001 << 30)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_ch_st0.*/
#define IPC_BIT_ICR_TX0_CH_ST13 ((u32)0x00000001 << 29)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_ch_st0.*/
#define IPC_BIT_ICR_TX0_CH_ST12 ((u32)0x00000001 << 28)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_ch_st0.*/
#define IPC_BIT_ICR_TX0_CH_ST11 ((u32)0x00000001 << 27)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_ch_st0.*/
#define IPC_BIT_ICR_TX0_CH_ST10 ((u32)0x00000001 << 26)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_ch_st0.*/
#define IPC_BIT_ICR_TX0_CH_ST9  ((u32)0x00000001 << 25)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_ch_st0.*/
#define IPC_BIT_ICR_TX0_CH_ST8  ((u32)0x00000001 << 24)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_ch_st0.*/
#define IPC_BIT_ICR_TX0_CH_ST7  ((u32)0x00000001 << 23)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_ch_st0.*/
#define IPC_BIT_ICR_TX0_CH_ST6  ((u32)0x00000001 << 22)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_ch_st0.*/
#define IPC_BIT_ICR_TX0_CH_ST5  ((u32)0x00000001 << 21)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_ch_st0.*/
#define IPC_BIT_ICR_TX0_CH_ST4  ((u32)0x00000001 << 20)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_ch_st0.*/
#define IPC_BIT_ICR_TX0_CH_ST3  ((u32)0x00000001 << 19)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_ch_st0.*/
#define IPC_BIT_ICR_TX0_CH_ST2  ((u32)0x00000001 << 18)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_ch_st0.*/
#define IPC_BIT_ICR_TX0_CH_ST1  ((u32)0x00000001 << 17)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_ch_st0.*/
#define IPC_BIT_ICR_TX0_CH_ST0  ((u32)0x00000001 << 16)          /*!< RW1CB 0x0  0: Writing 0 has no effect. 1: Clear the Tx data register (tx0_data) channel 0 bit.*/
#define IPC_MASK_TX0_CH_ST      ((u32)0x0000FFFF << 0)           /*!< R/WH/ES 0x0  Each bit represents the state of a channel.For example,when sw write all ch0 data, sw will pull 1. When cpu1 receives all ch0 data,it will be automatically cleared.  0:  ch0 can write 1: busy*/
#define IPC_TX0_CH_ST(x)        (((u32)((x) & 0x0000FFFF) << 0))
#define IPC_GET_TX0_CH_ST(x)    ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup IPC_ISR
  * @brief
  * @{
  */
#define IPC_BIT_ISR_TX0_EMPTY_INT15 ((u32)0x00000001 << 31) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_int0*/
#define IPC_BIT_ISR_TX0_EMPTY_INT14 ((u32)0x00000001 << 30) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_int0*/
#define IPC_BIT_ISR_TX0_EMPTY_INT13 ((u32)0x00000001 << 29) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_int0*/
#define IPC_BIT_ISR_TX0_EMPTY_INT12 ((u32)0x00000001 << 28) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_int0*/
#define IPC_BIT_ISR_TX0_EMPTY_INT11 ((u32)0x00000001 << 27) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_int0*/
#define IPC_BIT_ISR_TX0_EMPTY_INT10 ((u32)0x00000001 << 26) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_int0*/
#define IPC_BIT_ISR_TX0_EMPTY_INT9  ((u32)0x00000001 << 25) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_int0*/
#define IPC_BIT_ISR_TX0_EMPTY_INT8  ((u32)0x00000001 << 24) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_int0*/
#define IPC_BIT_ISR_TX0_EMPTY_INT7  ((u32)0x00000001 << 23) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_int0*/
#define IPC_BIT_ISR_TX0_EMPTY_INT6  ((u32)0x00000001 << 22) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_int0*/
#define IPC_BIT_ISR_TX0_EMPTY_INT5  ((u32)0x00000001 << 21) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_int0*/
#define IPC_BIT_ISR_TX0_EMPTY_INT4  ((u32)0x00000001 << 20) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_int0*/
#define IPC_BIT_ISR_TX0_EMPTY_INT3  ((u32)0x00000001 << 19) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_int0*/
#define IPC_BIT_ISR_TX0_EMPTY_INT2  ((u32)0x00000001 << 18) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_int0*/
#define IPC_BIT_ISR_TX0_EMPTY_INT1  ((u32)0x00000001 << 17) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_int0*/
#define IPC_BIT_ISR_TX0_EMPTY_INT0  ((u32)0x00000001 << 16) /*!< RW1CB 0x0  When all the data in ch0 is read by cpu1,it will be set to 1*/
#define IPC_BIT_ISR_RX0_INT15       ((u32)0x00000001 << 15) /*!< RW1CB 0x0  Refer to the description of the isr_rx0_int0*/
#define IPC_BIT_ISR_RX0_INT14       ((u32)0x00000001 << 14) /*!< RW1CB 0x0  Refer to the description of the isr_rx0_int0*/
#define IPC_BIT_ISR_RX0_INT13       ((u32)0x00000001 << 13) /*!< RW1CB 0x0  Refer to the description of the isr_rx0_int0*/
#define IPC_BIT_ISR_RX0_INT12       ((u32)0x00000001 << 12) /*!< RW1CB 0x0  Refer to the description of the isr_rx0_int0*/
#define IPC_BIT_ISR_RX0_INT11       ((u32)0x00000001 << 11) /*!< RW1CB 0x0  Refer to the description of the isr_rx0_int0*/
#define IPC_BIT_ISR_RX0_INT10       ((u32)0x00000001 << 10) /*!< RW1CB 0x0  Refer to the description of the isr_rx0_int0*/
#define IPC_BIT_ISR_RX0_INT9        ((u32)0x00000001 << 9)  /*!< RW1CB 0x0  Refer to the description of the isr_rx0_int0*/
#define IPC_BIT_ISR_RX0_INT8        ((u32)0x00000001 << 8)  /*!< RW1CB 0x0  Refer to the description of the isr_rx0_int0*/
#define IPC_BIT_ISR_RX0_INT7        ((u32)0x00000001 << 7)  /*!< RW1CB 0x0  Refer to the description of the isr_rx0_int0*/
#define IPC_BIT_ISR_RX0_INT6        ((u32)0x00000001 << 6)  /*!< RW1CB 0x0  Refer to the description of the isr_rx0_int0*/
#define IPC_BIT_ISR_RX0_INT5        ((u32)0x00000001 << 5)  /*!< RW1CB 0x0  Refer to the description of the isr_rx0_int0*/
#define IPC_BIT_ISR_RX0_INT4        ((u32)0x00000001 << 4)  /*!< RW1CB 0x0  Refer to the description of the isr_rx0_int0*/
#define IPC_BIT_ISR_RX0_INT3        ((u32)0x00000001 << 3)  /*!< RW1CB 0x0  Refer to the description of the isr_rx0_int0*/
#define IPC_BIT_ISR_RX0_INT2        ((u32)0x00000001 << 2)  /*!< RW1CB 0x0  Refer to the description of the isr_rx0_int0*/
#define IPC_BIT_ISR_RX0_INT1        ((u32)0x00000001 << 1)  /*!< RW1CB 0x0  Refer to the description of the isr_rx0_int0*/
#define IPC_BIT_ISR_RX0_INT0        ((u32)0x00000001 << 0)  /*!< RW1CB 0x0  fifo next pop chn0 data.when all the data in ch0 is read,it will be cleard*/
/** @} */

/** @defgroup IPC_IMR
  * @brief
  * @{
  */
#define IPC_MASK_IMR_TX0_EMPTY   ((u32)0x0000FFFF << 16)           /*!< R/W 0x0  0: Mask 1: Unmask*/
#define IPC_IMR_TX0_EMPTY(x)     (((u32)((x) & 0x0000FFFF) << 16))
#define IPC_GET_IMR_TX0_EMPTY(x) ((u32)(((x >> 16) & 0x0000FFFF)))
#define IPC_MASK_IMR_RX0         ((u32)0x0000FFFF << 0)            /*!< R/W 0x0  just useful  when  ipc_work_mode is 1. 0: Mask 1: Unmask*/
#define IPC_IMR_RX0(x)           (((u32)((x) & 0x0000FFFF) << 0))
#define IPC_GET_IMR_RX0(x)       ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup IPC_TX_ST
  * @brief
  * @{
  */
#define IPC_MASK_TX0_FIFO_TIMEOUT_TH   ((u32)0x0007FFFF << 12)           /*!< R/W 19'h9c40  If the number of data in the FIFO is below the threshold for a period longer than that set by this register, an interrupt will be triggered.*/
#define IPC_TX0_FIFO_TIMEOUT_TH(x)     (((u32)((x) & 0x0007FFFF) << 12))
#define IPC_GET_TX0_FIFO_TIMEOUT_TH(x) ((u32)(((x >> 12) & 0x0007FFFF)))
#define IPC_MASK_TX0_FIFO_AF_LEVEL     ((u32)0x0000001F << 7)            /*!< R/W 5'h1  User define the full level for afifo.Interrupt will arise when the fifo data number reaches (FIFO_DEPTH - tx_fifio_af_level). The vaule must >=1.*/
#define IPC_TX0_FIFO_AF_LEVEL(x)       (((u32)((x) & 0x0000001F) << 7))
#define IPC_GET_TX0_FIFO_AF_LEVEL(x)   ((u32)(((x >> 7) & 0x0000001F)))
#define IPC_BIT_TX0_FIFO_CLR           ((u32)0x00000001 << 6)            /*!< WA0 0x0  clear the FIFO data*/
#define IPC_BIT_TX0_FIFO_WFULL         ((u32)0x00000001 << 5)            /*!< R 0x0  fifo full state*/
#define IPC_MASK_TX0_FIFO_WSPACE       ((u32)0x0000001F << 0)            /*!< R 0x0  the remaining writable space for the current fifo*/
#define IPC_TX0_FIFO_WSPACE(x)         (((u32)((x) & 0x0000001F) << 0))
#define IPC_GET_TX0_FIFO_WSPACE(x)     ((u32)(((x >> 0) & 0x0000001F)))
/** @} */

/** @defgroup IPC_RX_ST
  * @brief
  * @{
  */
#define IPC_BIT_RX0_FIFO_EMPTY ((u32)0x00000001 << 0) /*!< R 1'b1  When fifo is empty, rx0 _ch,rx0_data is invalid*/
/** @} */

/** @defgroup IPC_RX_DATA
  * @brief
  * @{
  */
#define IPC_MASK_RX0_DATA   ((u32)0xFFFFFFFF << 0)           /*!< RP 0x0  fifo rdata*/
#define IPC_RX0_DATA(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define IPC_GET_RX0_DATA(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup IPC_SW
  * @brief
  * @{
  */
#define IPC_MASK_RSVD_SW1   ((u32)0x0000FFFF << 0)           /*!< R/W 0x0  rsvd for sw*/
#define IPC_RSVD_SW1(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define IPC_GET_RSVD_SW1(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup IPC_MODE
  * @brief
  * @{
  */
#define IPC_BIT_WORK_MODE ((u32)0x00000001 << 0) /*!< R/W 0x0  0: without share mem 1:  with share mem*/
/** @} */

/** @defgroup IPC_TIM
  * @brief
  * @{
  */
#define IPC_BIT_APB_TIMEOUT_FLAG  ((u32)0x00000001 << 8)           /*!< RW1CB 0x0  If apb_rdy does not reply within a certain period of time, it will be pulled.*/
#define IPC_MASK_APB_TIMEOUT_TH   ((u32)0x000000FF << 0)           /*!< R/W 8'h3f  If apb_rdy does not reply within a certain period of time, it will automatically reply. The counting clock : ipc clk*/
#define IPC_APB_TIMEOUT_TH(x)     (((u32)((x) & 0x000000FF) << 0))
#define IPC_GET_APB_TIMEOUT_TH(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup IPC_SEMx
  * @brief
  * @{
  */
#define IPC_MASK_SEMx_DATA   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  It is used to indicate whether this semaphore is occupied. 0: Not occupied; 1: Occupied.*/
#define IPC_SEMx_DATA(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define IPC_GET_SEMx_DATA(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup IPC_SEM_CPUIDx
  * @brief
  * @{
  */
#define IPC_MASK_SEM_CPUIDx_DATA   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  It is used to indicate cpuid of this semaphore .*/
#define IPC_SEM_CPUIDx_DATA(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define IPC_GET_SEM_CPUIDx_DATA(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup IPC_Exported_Types IPC Exported Types
  * @{
  */

/** @brief IPC Register Declaration
  */

typedef struct {
	__IO uint32_t IPC_TX_DATA_CHx[16];  /*!< Register,  Address offset:0x000-0x03C */
	__IO uint32_t IPC_TX_CH_ST       ;  /*!< Register,  Address offset:0x040 */
	__IO uint32_t IPC_ISR            ;  /*!< Register,  Address offset:0x044 */
	__IO uint32_t IPC_IMR            ;  /*!< Register,  Address offset:0x048 */
	__IO uint32_t IPC_TX_ST          ;  /*!< Register,  Address offset:0x04C */
	__I  uint32_t IPC_RX_ST          ;  /*!< Register,  Address offset:0x050 */
	__I  uint32_t IPC_RX_DATA        ;  /*!< Register,  Address offset:0x054 */
	__IO uint32_t IPC_SW             ;  /*!< Register,  Address offset:0x058 */
	__IO uint32_t IPC_MODE           ;  /*!< Register,  Address offset:0x05C */
	__IO uint32_t IPC_TIM            ;  /*!< Register,  Address offset:0x060 */
	__IO uint32_t RSVD0[3]           ;  /*!< Reserved,  Address offset:0x064-0x06F */
	__I  uint32_t IPC_SEMx[2]        ;  /*!< Register,  Address offset:0x070-0x074 */
	__I  uint32_t IPC_SEM_CPUIDx[4]  ;  /*!< Register,  Address offset:0x078-0x084 */
	__IO uint32_t IPC_DUMMY          ;  /*!< Register,  Address offset:0x088 */
} IPC_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */
/// @endcond

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here

/* Exported types --------------------------------------------------------*/
/** @addtogroup IPC_Exported_Types IPC Exported Types
  * @{
  */

/**
  * @brief IPC IRQ Function Definition
  */
typedef void (*IPC_IRQ_FUN)(void *Data, u32 IrqStatus, u32 ChanNum);

/**
  * @brief IPC User Message Type Definition
  */
typedef enum {
	IPC_USER_POINT = 0, /*!< Message payload is a pointer. */
	IPC_USER_DATA	= 1 /*!< Message payload is inline data. */
} USER_MSG_TYP_DEF;

/**
  * @brief IPC direction mode definition.
  */
typedef enum {
	IPC_NP_TO_AP = 0, /*!< NP send request to AP */
	IPC_AP_TO_NP = 1, /*!< AP send request to NP */
	IPC_DIR_MODE_MAX = 0xFFFFFFFF, /*!< Maximum sentinel value for direction mode. */
} IPC_Direction_Mode;

/**
  * @brief IPC Init Table Definition
  */
typedef struct _IPC_INIT_TABLE_ {
	u32 USER_MSG_TYPE;					/*!< User-defined message type. */
	void (*Rxfunc)(void *Data, u32 IrqStatus, u32 ChanNum);	/*!< Pointer to the RX interrupt callback function. */
	void *RxIrqData;					/*!< Data passed to the RX callback. */
	void (*Txfunc)(void *Data, u32 IrqStatus, u32 ChanNum);	/*!< Pointer to the TX interrupt callback function. */
	void *TxIrqData;					/*!< Data passed to the TX callback. */
	IPC_Direction_Mode IPC_Direction;	/*!< IPC transfer direction, this parameter can be a value of @ref IPC_Direction_Mode. */
	u32 IPC_Channel;	/*!< IPC TX channel, this parameter can be a value of @ref IPC_Tx_Channel. */
} IPC_INIT_TABLE, *PIPC_INIT_TABLE;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup IPC_Exported_Constants IPC Exported Constants
  * @{
  */

/** @defgroup IPC_Peripheral_Definition IPC Peripheral Definition
  * @{
  */
/** @brief Check if IPC peripheral pointer is valid. */
#define IS_IPC_ALL_PERIPH(PERIPH) (((PERIPH) == IPCNP_DEV) || \
										((PERIPH) == IPCAP_DEV))
/**
  * @}
  */


/** @defgroup IPC_INTR_Mode IPC Interrupt Mode
  * @{
  */
#define IPC_TX_EMPTY			((u32)0x00000001)  /*!< IPC TX channel empty interrupt mode. */
#define IPC_RX_FULL 			((u32)0x00000002)  /*!< IPC RX channel full interrupt mode. */
/** @brief Check if IPC interrupt mode value is valid. */
#define IS_IPC_INTR_MODE(MODE) (((MODE) == IPC_TX_EMPTY) || \
                                   ((MODE) == IPC_RX_FULL))
/**
  * @}
  */

/** @defgroup IPC_Valid_CHNUM IPC Valid Channel Number
  * @{
  */
#define IS_IPC_VALID_CHNUM(NUM) ((NUM) < 16)  /*!< Check if IPC channel number is valid. */
/**
  * @}
  */

/** @defgroup IPC_Valid_Threshold IPC Valid Threshold
 * @{
 */
#define IS_IPC_VALID_Threshold(Ths) ((Ths) >= 1)  /*!< Check if IPC FIFO threshold value is valid. */
/**
  * @}
  */

/** @defgroup IPC_Valid_RX_CHANNEL IPC Valid RX Channel
  * @{
  */
#define IS_IPC_RX_CHNUM(NUM) ((NUM) < 16)  /*!< Check if IPC RX channel number is valid. */
/**
  * @}
  */


/** @defgroup IPC_Valid_SEMID IPC Valid Semaphore ID
  * @{
  */
#define IPC_SEMA_NUM (64)  /*!< Total number of IPC semaphores. */
#define IS_IPC_VALID_SEMID(SEM_ID) ((SEM_ID) < IPC_SEMA_NUM)  /*!< Check if IPC semaphore ID is valid. */
/**
  * @}
  */

/** @defgroup IPC_TX_CHANNEL_SHIFT IPC TX Channel Shift
  * @{
  */
#define IPC_TX_CHANNEL_SHIFT 16  /*!< Bit shift for IPC TX channel index. */
/**
  * @}
  */

/** @defgroup IPC_TX_CHANNEL_NUM IPC TX Channel Number
  * @{
  */
#define IPC_TX_CHANNEL_NUM 16  /*!< Total number of IPC TX channels. */
/**
  * @}
  */

/** @defgroup IPC_TX_CHANNEL_SWITCH IPC TX Channel Switch
  * @{
  */
#define IPC_TX_CHANNEL_SWITCH(x)				((u32)((x) & 0x0000000F))  /*!< Extract TX channel index from channel value. */
/**
  * @}
  */

/** @defgroup IPC_CHANNEL_NUMBER IPC Channel Number
  * @{
  */
#define IPC_CHANNEL_NUM 32  /*!< Total number of IPC channels (TX + RX). */
/**
  * @}
  */


/**
  * @}
  */

IPC_TypeDef *IPC_GetDev(IPC_Direction_Mode IPC_Dir, u32 Is_Rx);
IPC_TypeDef *IPC_GetDevById(u32 cpu_id);

/* Exported functions --------------------------------------------------------*/
/** @addtogroup IPC_Exported_Functions IPC Exported Functions
  * @{
  */
void IPC_INTConfig(IPC_TypeDef *IPCx, u8 IPC_Shiftbit, u32 NewState);
void IPC_IERSet(IPC_TypeDef *IPCx, u32 IPC_Chs);
u32 IPC_IERGet(IPC_TypeDef *IPCx);
u32 IPC_INTRequest(IPC_TypeDef *IPCx, u8 IPC_ChNum);
u32 IPC_INTGet(IPC_TypeDef *IPCx);
void IPC_INTClear(IPC_TypeDef *IPCx, u8 IPC_Shiftbit);
u32 IPC_INTHandler(void *Data);
void IPC_INTUserHandler(IPC_TypeDef *IPCx, u8 IPC_Shiftbit, void *IrqHandler, void *IrqData);
/**
  * @}
  */

/** @} */
/** @} */
/* Other definitions --------------------------------------------------------*/

extern const IPC_INIT_TABLE  ipc_init_config[];

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif