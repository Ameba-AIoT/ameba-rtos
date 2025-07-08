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

/** @defgroup IPC
  * @brief IPC driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup IPC_Register_Definitions IPC Register Definitions
  * @{
  */

/** @defgroup IPC_DATA
  * @brief Tx_Rx Register
  * @{
  */
#define IPC_MASK_TX0_DATA   ((u32)0x0000FFFF << 16)           /*!< R/WH/ES 0x0  After preparing descriptor, data and corresponding memory, 1: Writing 1 to the Tx data register (tx0_data) channel x bit, and the data will be mapped to the CPU0's RX data register (rx0_data) channel x bit. And the corresponding Rx full status bit in REG_ISR_CPU0 will be set. 0: Writing 0 has no effect. If the corresponding Rx full status bit in REG_ISR_CPU0 (eg.isr_rx0_full_status6) is cleared, the corresponding bit in tx0_data will be cleaned automatically.*/
#define IPC_TX0_DATA(x)     (((u32)((x) & 0x0000FFFF) << 16))
#define IPC_GET_TX0_DATA(x) ((u32)(((x >> 16) & 0x0000FFFF)))
#define IPC_MASK_RX0_DATA   ((u32)0x0000FFFF << 0)            /*!< R 0x0  Rx data automatically maps data from tx0_data of CPU0*/
#define IPC_RX0_DATA(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define IPC_GET_RX0_DATA(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup IPC_ISR
  * @brief Interrupt Empty Full Status Register
  * @{
  */
#define IPC_BIT_ISR_TX0_EMPTY_STATUS15 ((u32)0x00000001 << 31) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status0*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS14 ((u32)0x00000001 << 30) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status0*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS13 ((u32)0x00000001 << 29) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status0*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS12 ((u32)0x00000001 << 28) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status0*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS11 ((u32)0x00000001 << 27) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status0*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS10 ((u32)0x00000001 << 26) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status0*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS9  ((u32)0x00000001 << 25) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status0*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS8  ((u32)0x00000001 << 24) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status0*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS7  ((u32)0x00000001 << 23) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status0*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS6  ((u32)0x00000001 << 22) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status0*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS5  ((u32)0x00000001 << 21) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status0*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS4  ((u32)0x00000001 << 20) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status0*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS3  ((u32)0x00000001 << 19) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status0*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS2  ((u32)0x00000001 << 18) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status0*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS1  ((u32)0x00000001 << 17) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status0*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS0  ((u32)0x00000001 << 16) /*!< RW1CB 0x0  Tx channel 0 empty interrupt status of  CPU1 transmit to CPU0. If the corresponding Rx full status bit in REG_ISR_CPU0 (eg.isr_rx0_full_status0) is cleared, the corresponding Tx empty status bit will be set automatically. It will be cleared by software writing 1.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS15  ((u32)0x00000001 << 15) /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status0.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS14  ((u32)0x00000001 << 14) /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status0.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS13  ((u32)0x00000001 << 13) /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status0.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS12  ((u32)0x00000001 << 12) /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status0.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS11  ((u32)0x00000001 << 11) /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status0.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS10  ((u32)0x00000001 << 10) /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status0.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS9   ((u32)0x00000001 << 9)  /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status0.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS8   ((u32)0x00000001 << 8)  /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status0.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS7   ((u32)0x00000001 << 7)  /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status0.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS6   ((u32)0x00000001 << 6)  /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status0.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS5   ((u32)0x00000001 << 5)  /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status0.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS4   ((u32)0x00000001 << 4)  /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status0.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS3   ((u32)0x00000001 << 3)  /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status0.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS2   ((u32)0x00000001 << 2)  /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status0.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS1   ((u32)0x00000001 << 1)  /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status0.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS0   ((u32)0x00000001 << 0)  /*!< RW1CB 0x0  Rx channel 0 full interrupt status of CPU1. The corresponding Rx full status bit will be set by CPU0's Tx data register (tx0_data) channel 0. It will be cleared by software writing 1.*/
/** @} */

/** @defgroup IPC_IMR
  * @brief Interrupt Empty Full Mask Register
  * @{
  */
#define IPC_MASK_IMR_TX0_EMPTY_MASK   ((u32)0x0000FFFF << 16)           /*!< R/W 0x0  0: Mask Tx Channel x empty interrupt of  CPU1 transmit to CPU0 1: Unmask Tx Channel x empty interrupt of  CPU1 transmit to CPU0*/
#define IPC_IMR_TX0_EMPTY_MASK(x)     (((u32)((x) & 0x0000FFFF) << 16))
#define IPC_GET_IMR_TX0_EMPTY_MASK(x) ((u32)(((x >> 16) & 0x0000FFFF)))
#define IPC_MASK_IMR_RX0_FULL_MASK    ((u32)0x0000FFFF << 0)            /*!< R/W 0x0  0: Mask Rx Channel x full interrupt of  CPU1 received from CPU0 1: Unmask Rx Channel x full interrupt of  CPU1 received from CPU0*/
#define IPC_IMR_RX0_FULL_MASK(x)      (((u32)((x) & 0x0000FFFF) << 0))
#define IPC_GET_IMR_RX0_FULL_MASK(x)  ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup IPC_ICR
  * @brief Clear Tx Register
  * @{
  */
#define IPC_BIT_ICR_TX0_DATA_CLEAR15 ((u32)0x00000001 << 31)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear0.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR14 ((u32)0x00000001 << 30)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear0.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR13 ((u32)0x00000001 << 29)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear0.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR12 ((u32)0x00000001 << 28)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear0.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR11 ((u32)0x00000001 << 27)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear0.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR10 ((u32)0x00000001 << 26)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear0.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR9  ((u32)0x00000001 << 25)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear0.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR8  ((u32)0x00000001 << 24)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear0.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR7  ((u32)0x00000001 << 23)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear0.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR6  ((u32)0x00000001 << 22)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear0.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR5  ((u32)0x00000001 << 21)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear0.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR4  ((u32)0x00000001 << 20)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear0.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR3  ((u32)0x00000001 << 19)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear0.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR2  ((u32)0x00000001 << 18)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear0.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR1  ((u32)0x00000001 << 17)          /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear0.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR0  ((u32)0x00000001 << 16)          /*!< RW1CB 0x0  0: Writing 0 has no effect. 1: Clear the Tx data register (tx0_data) channel 0 bit.*/
#define IPC_MASK_RSVD0               ((u32)0x0000FFFF << 0)           /*!< R/W 0x0  */
#define IPC_RSVD0(x)                 (((u32)((x) & 0x0000FFFF) << 0))
#define IPC_GET_RSVD0(x)             ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup IPC_SEMx
  * @brief
  * @{
  */
#define IPC_MASK_SEMx_DATA   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  it is used to indicate who owns this semaphore .*/
#define IPC_SEMx_DATA(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define IPC_GET_SEMx_DATA(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup IPC_SEM_CPUIDx
  * @brief
  * @{
  */
#define IPC_MASK_SEM_CPUIDx_DATA   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  it is used to indicate cpuid of this semaphore .*/
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
	__IO uint32_t IPC_DATA         ;  /*!< TX_RX REGISTER,  Address offset:0x000 */
	__IO uint32_t IPC_ISR          ;  /*!< INTERRUPT EMPTY FULL STATUS REGISTER,  Address offset:0x004 */
	__IO uint32_t IPC_IMR          ;  /*!< INTERRUPT EMPTY FULL MASK REGISTER,  Address offset:0x008 */
	__IO uint32_t IPC_ICR          ;  /*!< CLEAR TX REGISTER,  Address offset:0x00C */
	__IO uint32_t RSVD0[4]         ;  /*!< Reserved,  Address offset:0x010-0x01F */
	__I  uint32_t IPC_SEMx[2]      ;  /*!< Register,  Address offset:0x020-0x024 */
	__I  uint32_t IPC_SEM_CPUIDx[4];  /*!< Register,  Address offset:0x028-0x034 */
	__IO uint32_t IPC_DUMMY        ;  /*!< Register,  Address offset:0x038 */
} IPC_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

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
	IPC_USER_POINT = 0,
	IPC_USER_DATA	= 1
} USER_MSG_TYP_DEF;

/**
  * @brief IPC Init Table Definition
  */
typedef struct _IPC_INIT_TABLE_ {
	u32 USER_MSG_TYPE;
	void (*Rxfunc)(void *Data, u32 IrqStatus, u32 ChanNum);
	void *RxIrqData;
	void (*Txfunc)(void *Data, u32 IrqStatus, u32 ChanNum);
	void *TxIrqData;
	u32 IPC_Direction;	/* direction of ipc, this parameter is from @IPC_Direction_Mode*/
	u32 IPC_Channel;	/* ipc channel, this parameter is from @IPC_AP_Tx_Channel or @IPC_NP_Tx_Channel*/
} IPC_INIT_TABLE, *PIPC_INIT_TABLE;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup IPC_Exported_Constants IPC Exported Constants
  * @{
  */

/** @defgroup IPC_Peripheral_Definition
  * @{
  */
#define IS_IPC_ALL_PERIPH(PERIPH) (((PERIPH) == IPCNP_DEV) || \
										((PERIPH) == IPCAP_DEV))
/**
  * @}
  */

/** @defgroup IPC_Direction_Mode
  * @{
  */
#define IPC_NP_TO_AP			((u32)0x00000000)
#define IPC_AP_TO_NP			((u32)0x00000001)
#define IS_IPC_DIR_MODE(MODE) (((MODE) == IPC_NP_TO_AP) || \
									((MODE) == IPC_AP_TO_NP))
/**
  * @}
  */

/** @defgroup IPC_INTR_Mode
  * @{
  */
#define IPC_TX_EMPTY			((u32)0x00000001)
#define IPC_RX_FULL 			((u32)0x00000002)
#define IS_IPC_INTR_MODE(MODE) (((MODE) == IPC_TX_EMPTY) || \
                                   ((MODE) == IPC_RX_FULL))
/**
  * @}
  */

/** @defgroup IPC_Valid_CHNUM
  * @{
  */
#define IS_IPC_VALID_CHNUM(NUM) ((NUM) < 16)
/**
  * @}
  */

/** @defgroup IPC_Valid_RX_CHANNEL
  * @{
  */
#define IS_IPC_RX_CHNUM(NUM) ((NUM) < 16)
/**
  * @}
  */


/** @defgroup IPC_Valid_SEMID
  * @{
  */
#define IS_IPC_VALID_SEMID(SEM_ID) ((SEM_ID) < 64)
/**
  * @}
  */

/** @defgroup IPC_TX_CHANNEL_SHIFT
  * @{
  */
#define IPC_TX_CHANNEL_SHIFT 16
/**
  * @}
  */

/** @defgroup IPC_TX_CHANNEL_NUM
  * @{
  */
#define IPC_TX_CHANNEL_NUM 16
/**
  * @}
  */

/** @defgroup IPC_TX_CHANNEL_SWITCH
  * @{
  */
#define IPC_TX_CHANNEL_SWITCH(x)				((u32)((x) & 0x0000000F))
/**
  * @}
  */

/** @defgroup IPC_CHANNEL_NUMBER

  * @{
  */
#define IPC_CHANNEL_NUM 32
/**
  * @}
  */


/**
  * @}
  */
/** @} */
/** @} */
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
IPC_TypeDef *IPC_GetDev(u32 IPC_Dir, u32 Is_Rx);
IPC_TypeDef *IPC_GetDevById(u32 cpu_id);
/**
  * @}
  */

/* Other definitions --------------------------------------------------------*/

extern const IPC_INIT_TABLE  ipc_init_config[];

/* MANUAL_GEN_END */

#endif