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

/** @defgroup IPC_TX_DATA
  * @brief Tx Register
  * @{
  */
#define IPC_MASK_TX0_DATA   ((u32)0x000000FF << 8)           /*!< R/W/ES 0x0  After preparing descriptor, data and corresponding memory, 1: Writing 1 to the Tx data register (tx0_data) channel x bit, and the data will be mapped to the CPU0's RX data register (rx0_data) channel x bit. And the corresponding Rx full status bit in REG_ISR_CPU0 will be set. 0: Writing 0 has no effect. If the corresponding Rx full status bit in REG_ISR_CPU0 (eg.isr_rx0_full_status6) is cleared, the corresponding bit in tx0_data will be cleaned automatically.*/
#define IPC_TX0_DATA(x)     (((u32)((x) & 0x000000FF) << 8))
#define IPC_GET_TX0_DATA(x) ((u32)(((x >> 8) & 0x000000FF)))
#define IPC_MASK_TX1_DATA   ((u32)0x000000FF << 0)           /*!< R/W/ES 0x0  After preparing descriptor, data and corresponding memory, 1: Writing 1 to the Tx data register (tx1_data) channel x bit, and the data will be mapped to the CPU1's Rx data register (rx1_data) channel x bit. And the corresponding Rx full status bit in REG_ISR_CPU1 will be set. 0: Writing 0 has no effect. If the corresponding Rx full status bit in REG_ISR_CPU1 (eg.isr_rx1_full_status6) is cleared, the corresponding bit in tx_data_ln will be cleaned automatically.*/
#define IPC_TX1_DATA(x)     (((u32)((x) & 0x000000FF) << 0))
#define IPC_GET_TX1_DATA(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup IPC_RX_DATA
  * @brief Rx Register
  * @{
  */
#define IPC_MASK_RX0_DATA   ((u32)0x000000FF << 8)           /*!< R 0x0  Rx data automatically maps data from tx0_data of CPU0*/
#define IPC_RX0_DATA(x)     (((u32)((x) & 0x000000FF) << 8))
#define IPC_GET_RX0_DATA(x) ((u32)(((x >> 8) & 0x000000FF)))
#define IPC_MASK_RX1_DATA   ((u32)0x000000FF << 0)           /*!< R 0x0  Rx data automatically maps data from tx1_data of CPU1*/
#define IPC_RX1_DATA(x)     (((u32)((x) & 0x000000FF) << 0))
#define IPC_GET_RX1_DATA(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup IPC_ISR
  * @brief Interrupt Status Register
  * @{
  */
#define IPC_BIT_ISR_RX0_FULL_STATUS7  ((u32)0x00000001 << 31) /*!< RW1CB 0x0  Rx channel 7 full interrupt status of CPU3. The corresponding Rx full status bit will be set by CPU0's Tx data register (tx0_data) channel 7. It will be cleared by software writing 1.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS6  ((u32)0x00000001 << 30) /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status7.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS5  ((u32)0x00000001 << 29) /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status7.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS4  ((u32)0x00000001 << 28) /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status7.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS3  ((u32)0x00000001 << 27) /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status7.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS2  ((u32)0x00000001 << 26) /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status7.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS1  ((u32)0x00000001 << 25) /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status7.*/
#define IPC_BIT_ISR_RX0_FULL_STATUS0  ((u32)0x00000001 << 24) /*!< RW1CB 0x0  Refer to the description of the isr_rx0_full_status7.*/
#define IPC_BIT_ISR_RX1_FULL_STATUS7  ((u32)0x00000001 << 23) /*!< RW1CB 0x0  Rx channel 7 full interrupt status of  CPU3. The corresponding Rx full status bit will be set by CPU1's Tx data register (tx1_data) channel 7. It will be cleared by software writing 1.*/
#define IPC_BIT_ISR_RX1_FULL_STATUS6  ((u32)0x00000001 << 22) /*!< RW1CB 0x0  Refer to the description of the isr_rx1_full_status7.*/
#define IPC_BIT_ISR_RX1_FULL_STATUS5  ((u32)0x00000001 << 21) /*!< RW1CB 0x0  Refer to the description of the isr_rx1_full_status7.*/
#define IPC_BIT_ISR_RX1_FULL_STATUS4  ((u32)0x00000001 << 20) /*!< RW1CB 0x0  Refer to the description of the isr_rx1_full_status7.*/
#define IPC_BIT_ISR_RX1_FULL_STATUS3  ((u32)0x00000001 << 19) /*!< RW1CB 0x0  Refer to the description of the isr_rx1_full_status7.*/
#define IPC_BIT_ISR_RX1_FULL_STATUS2  ((u32)0x00000001 << 18) /*!< RW1CB 0x0  Refer to the description of the isr_rx1_full_status7.*/
#define IPC_BIT_ISR_RX1_FULL_STATUS1  ((u32)0x00000001 << 17) /*!< RW1CB 0x0  Refer to the description of the isr_rx1_full_status7.*/
#define IPC_BIT_ISR_RX1_FULL_STATUS0  ((u32)0x00000001 << 16) /*!< RW1CB 0x0  Refer to the description of the isr_rx1_full_status7.*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS7 ((u32)0x00000001 << 15) /*!< RW1CB 0x0  Tx channel 7 empty interrupt status of  CPU3 transmit to CPU0. If the corresponding Rx full status bit in REG_ISR_CPU0 (eg.isr_rx0_full_status7) is cleared, the corresponding Tx empty status bit will be set automatically. It will be cleared by software writing 1.*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS6 ((u32)0x00000001 << 14) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status7.*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS5 ((u32)0x00000001 << 13) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status7.*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS4 ((u32)0x00000001 << 12) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status7.*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS3 ((u32)0x00000001 << 11) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status7.*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS2 ((u32)0x00000001 << 10) /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status7.*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS1 ((u32)0x00000001 << 9)  /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status7.*/
#define IPC_BIT_ISR_TX0_EMPTY_STATUS0 ((u32)0x00000001 << 8)  /*!< RW1CB 0x0  Refer to the description of the isr_tx0_empty_status7.*/
#define IPC_BIT_ISR_TX1_EMPTY_STATUS7 ((u32)0x00000001 << 7)  /*!< RW1CB 0x0  Tx channel 7 empty interrupt status of  CPU3 transmit to CPU1. If the corresponding Rx full status bit in REG_ISR_CPU1 (eg.isr_rx1_full_status7) is cleared, the corresponding Tx empty status bit will be set automatically. It will be cleared by software writing 1.*/
#define IPC_BIT_ISR_TX1_EMPTY_STATUS6 ((u32)0x00000001 << 6)  /*!< RW1CB 0x0  Refer to the description of the isr_tx1_empty_status7.*/
#define IPC_BIT_ISR_TX1_EMPTY_STATUS5 ((u32)0x00000001 << 5)  /*!< RW1CB 0x0  Refer to the description of the isr_tx1_empty_status7.*/
#define IPC_BIT_ISR_TX1_EMPTY_STATUS4 ((u32)0x00000001 << 4)  /*!< RW1CB 0x0  Refer to the description of the isr_tx1_empty_status7.*/
#define IPC_BIT_ISR_TX1_EMPTY_STATUS3 ((u32)0x00000001 << 3)  /*!< RW1CB 0x0  Refer to the description of the isr_tx1_empty_status7.*/
#define IPC_BIT_ISR_TX1_EMPTY_STATUS2 ((u32)0x00000001 << 2)  /*!< RW1CB 0x0  Refer to the description of the isr_tx1_empty_status7.*/
#define IPC_BIT_ISR_TX1_EMPTY_STATUS1 ((u32)0x00000001 << 1)  /*!< RW1CB 0x0  Refer to the description of the isr_tx1_empty_status7.*/
#define IPC_BIT_ISR_TX1_EMPTY_STATUS0 ((u32)0x00000001 << 0)  /*!< RW1CB 0x0  Refer to the description of the isr_tx1_empty_status7.*/
/** @} */

/** @defgroup IPC_IMR
  * @brief Interrupt Mask Register
  * @{
  */
#define IPC_MASK_IMR_RX0_FULL_MASK    ((u32)0x000000FF << 24)           /*!< R/W 0x0  0: Mask Rx Channel x full interrupt of  CPU3 received from CPU0 1: Unmask Rx Channel x full interrupt of  CPU3 received from CPU0*/
#define IPC_IMR_RX0_FULL_MASK(x)      (((u32)((x) & 0x000000FF) << 24))
#define IPC_GET_IMR_RX0_FULL_MASK(x)  ((u32)(((x >> 24) & 0x000000FF)))
#define IPC_MASK_IMR_RX1_FULL_MASK    ((u32)0x000000FF << 16)           /*!< R/W 0x0  0: Mask Rx Channel x full interrupt of CPU3 received from CPU1 1: Unmask Rx Channel x full interrupt of CPU3 received from CPU1*/
#define IPC_IMR_RX1_FULL_MASK(x)      (((u32)((x) & 0x000000FF) << 16))
#define IPC_GET_IMR_RX1_FULL_MASK(x)  ((u32)(((x >> 16) & 0x000000FF)))
#define IPC_MASK_IMR_TX0_EMPTY_MASK   ((u32)0x000000FF << 8)            /*!< R/W 0x0  0: Mask Tx Channel x empty interrupt of  CPU3 transmit to CPU0 1: Unmask Tx Channel x empty interrupt of  CPU3 transmit to CPU0*/
#define IPC_IMR_TX0_EMPTY_MASK(x)     (((u32)((x) & 0x000000FF) << 8))
#define IPC_GET_IMR_TX0_EMPTY_MASK(x) ((u32)(((x >> 8) & 0x000000FF)))
#define IPC_MASK_IMR_TX1_EMPTY_MASK   ((u32)0x000000FF << 0)            /*!< R/W 0x0  0: Mask Tx Channel x empty interrupt of  CPU3 transmit to CPU1 1: Unmask Tx Channel x empty interrupt of  CPU3 transmit to CPU1*/
#define IPC_IMR_TX1_EMPTY_MASK(x)     (((u32)((x) & 0x000000FF) << 0))
#define IPC_GET_IMR_TX1_EMPTY_MASK(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup IPC_ICR
  * @brief Clear Tx Register
  * @{
  */
#define IPC_BIT_ICR_TX0_DATA_CLEAR7 ((u32)0x00000001 << 15) /*!< RW1CB 0x0  0: Writing 0 has no effect. 1: Clear the Tx data register (tx0_data) channel 7 bit.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR6 ((u32)0x00000001 << 14) /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear7.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR5 ((u32)0x00000001 << 13) /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear7.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR4 ((u32)0x00000001 << 12) /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear7.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR3 ((u32)0x00000001 << 11) /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear7.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR2 ((u32)0x00000001 << 10) /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear7.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR1 ((u32)0x00000001 << 9)  /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear7.*/
#define IPC_BIT_ICR_TX0_DATA_CLEAR0 ((u32)0x00000001 << 8)  /*!< RW1CB 0x0  Refer to the description of the icr_tx0_data_clear7.*/
#define IPC_BIT_ICR_TX1_DATA_CLEAR7 ((u32)0x00000001 << 7)  /*!< RW1CB 0x0  0: Writing 0 has no effect. 1: Clear the Tx data register (tx1_data) channel 7 bit.*/
#define IPC_BIT_ICR_TX1_DATA_CLEAR6 ((u32)0x00000001 << 6)  /*!< RW1CB 0x0  Refer to the description of the icr_tx1_data_clear7.*/
#define IPC_BIT_ICR_TX1_DATA_CLEAR5 ((u32)0x00000001 << 5)  /*!< RW1CB 0x0  Refer to the description of the icr_tx1_data_clear7.*/
#define IPC_BIT_ICR_TX1_DATA_CLEAR4 ((u32)0x00000001 << 4)  /*!< RW1CB 0x0  Refer to the description of the icr_tx1_data_clear7.*/
#define IPC_BIT_ICR_TX1_DATA_CLEAR3 ((u32)0x00000001 << 3)  /*!< RW1CB 0x0  Refer to the description of the icr_tx1_data_clear7.*/
#define IPC_BIT_ICR_TX1_DATA_CLEAR2 ((u32)0x00000001 << 2)  /*!< RW1CB 0x0  Refer to the description of the icr_tx1_data_clear7.*/
#define IPC_BIT_ICR_TX1_DATA_CLEAR1 ((u32)0x00000001 << 1)  /*!< RW1CB 0x0  Refer to the description of the icr_tx1_data_clear7.*/
#define IPC_BIT_ICR_TX1_DATA_CLEAR0 ((u32)0x00000001 << 0)  /*!< RW1CB 0x0  Refer to the description of the icr_tx1_data_clear7.*/
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup IPC_Exported_Types IPC Exported Types
  * @{
  */

/** @brief IPC Register Declaration
  */

typedef struct {
	__IO uint32_t IPC_TX_DATA;  /*!< TX REGISTER,  Address offset:0x000 */
	__I  uint32_t IPC_RX_DATA;  /*!< RX REGISTER,  Address offset:0x004 */
	__IO uint32_t IPC_ISR    ;  /*!< INTERRUPT STATUS REGISTER,  Address offset:0x008 */
	__IO uint32_t IPC_IMR    ;  /*!< INTERRUPT MASK REGISTER,  Address offset:0x00C */
	__IO uint32_t IPC_ICR    ;  /*!< CLEAR TX REGISTER,  Address offset:0x010 */
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

/**
  * @brief IPC SEM IDX
  */
typedef enum {
	IPC_SEM_IMQ = 0, /*!< Semaphore index for IMQ. */
	IPC_SEM_FLASH, /*!< Semaphore index for Flash access. */
	IPC_SEM_OTP, /*!< Semaphore index for OTP access. */
	IPC_SEM_CRYPTO, /*!< Semaphore index for Crypto engine. */
	IPC_SEM_DIAGNOSE, /*!< Semaphore index for Diagnose module. */
	IPC_SEM_SYSON, /*!< Semaphore index for System-On control. */
	IPC_SEM_GDMA, /*!< Semaphore index for GDMA. */
	IPC_SEM_MAX = 16, /*!< Maximum semaphore index sentinel value. */
} IPC_SEM_IDX;

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
	IPC_LP_TO_NP = 0x00, /*!< LP send request to NP */
	IPC_LP_TO_AP = 0x01, /*!< LP send request to AP */
	IPC_NP_TO_LP = 0x10, /*!< NP send request to LP */
	IPC_NP_TO_AP = 0x11, /*!< NP send request to AP */
	IPC_AP_TO_LP = 0x20, /*!< AP send request to LP */
	IPC_AP_TO_NP = 0x21, /*!< AP send request to NP */
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

/** @} */

/* Exported constants --------------------------------------------------------*/
/** @defgroup IPC_Exported_Constants IPC Exported Constants
  * @{
  */

/** @defgroup IPC_Peripheral_Definition IPC Peripheral Definition
  * @{
  */
/** @brief Check if IPC peripheral pointer is valid. */
#define IS_IPC_ALL_PERIPH(PERIPH) (((PERIPH) == IPCLP_DEV) || \
										((PERIPH) == IPCNP_DEV)) || \
										((PERIPH) == IPCAP_DEV))
/** @} */


/** @defgroup IPC_INTR_Mode IPC Interrupt Mode
  * @{
  */
#define IPC_TX_EMPTY			((u32)0x00000001) /*!< IPC TX channel empty interrupt mode. */
#define IPC_RX_FULL 			((u32)0x00000002) /*!< IPC RX channel full interrupt mode. */
/** @brief Check if IPC interrupt mode value is valid. */
#define IS_IPC_INTR_MODE(MODE) (((MODE) == IPC_TX_EMPTY) || \
                                   ((MODE) == IPC_RX_FULL))
/** @} */

/** @defgroup IPC_CHANNEL IPC Channel
  * @{
  */
#define IPC_TX_CHANNEL_NUM						16 /*!< Total number of IPC TX channels. */
#define IPC_TX_CHANNEL_SWITCH(x)				((u32)(((x >> 4) & 0x0000000F))) /*!< Extract TX channel index from channel value. */
#define IPC_TX0_CHANNEL_NUM						8 /*!< Total number of IPC TX0 channels. */
#define IPC_TX0_CHANNEL_SWITCH(x)				((u32)((x) & 0x0000000F)) /*!< Extract TX0 channel index from channel value. */
#define IS_IPC_RX_CHNUM(NUM) 					((NUM) >= 16) /*!< Check if IPC RX channel number is valid. */
#define IPC_CHANNEL_NUM 						32 /*!< Total number of IPC channels (TX + RX). */
/** @} */

/** @defgroup IPC_Valid_CHNUM IPC Valid Channel Number
  * @{
  */
#define IS_IPC_VALID_CHNUM(NUM) ((NUM) < 8) /*!< Check if IPC channel number is valid. */
/** @} */

/** @defgroup IPC_Valid_SEMID IPC Valid Semaphore ID
  * @{
  */
#define IS_IPC_VALID_SEMID(SEM_ID) ((SEM_ID) < 16) /*!< Check if IPC semaphore ID is valid. */
/** @} */


/** @defgroup IPC_Valid_CPUID IPC Valid CPU ID
  * @{
  */
#define IS_IPC_Valid_CPUID(cpuid)		((cpuid)<=2) /*!< Check if IPC CPU ID is valid. */
/** @} */

/** @defgroup IPC_Tx_Channel IPC TX Channel
 * @{
 */

/*
 * IPC_LP_Tx_Channel
 */
#define IPC_L2N_LOGUART_RX_SWITCH		0	/*!<  LP -->  NP Loguart Rx Switch*/
#define IPC_L2N_WAKE_NP					1 /*!< LP wakes up NP channel. */
#define IPC_L2N_FLASHPG_REQ				2	/*!<  LP -->  NP Flash Program REQUEST*/
//#define IPC_L2N_Channel3				3
//#define IPC_L2N_Channel4				4
//#define IPC_L2N_Channel5				5
//#define IPC_L2N_Channel6				6
#define IPC_L2N_IMQ_TRX_TRAN					7	/*!<  LP -->  NP IMQ Message Exchange */

#define IPC_L2A_LOGUART_RX_SWITCH				0	/*!<  LP -->  AP Loguart Rx Switch*/
#define IPC_L2A_Channel1				1 /*!< LP to AP channel 1. */
//#define IPC_L2A_Channel2				2
//#define IPC_L2A_Channel3				3
//#define IPC_L2A_Channel4				4
//#define IPC_L2A_Channel5				5
//#define IPC_L2A_Channel6				6
#define IPC_L2A_IMQ_TRX_TRAN					7	/*!<  LP -->  AP IMQ Message Exchange */

/*
 * IPC_NP_Tx_Channel
 */
#define IPC_N2L_TICKLESS_INDICATION			0	/*!<  NP -->  LP Tickless indicate */
#define IPC_N2L_WIFI_FW_INFO					1	/*!<  NP -->  LP FW Info*/
#define IPC_N2L_FLASHPG_REQ					2	/*!<  NP -->  LP Flash Program Request*/
#define IPC_N2L_UARTBRIDGE						3 /*!< NP to LP UART bridge channel. */
//#define IPC_N2L_Channel4						4
//#define IPC_N2L_Channel5						5
//#define IPC_N2L_Channel6						6
#define IPC_N2L_IMQ_TRX_TRAN					7	/*!<  NP -->  LP IMQ Message Exchange */

#define IPC_N2A_WIFI_TRX_TRAN					0	/*!<  NP -->  AP WIFI Message Exchange */
#define IPC_N2A_WIFI_API_TRAN					1	/*!<  NP -->  AP API WIFI Message Exchange */
//#define IPC_N2A_Channel2						2	/*!<  NP -->  AP */
#define IPC_N2A_COEX_API_TRAN					3	/*!<  NP -->  AP COEX API Exchange */
#define IPC_N2A_BT_DRC_TRAN						4	/*!<  NP -->  AP BT DATA Message Exchange */
#define IPC_N2A_802154_TRAN						5 /*!< NP to AP 802.15.4 data transfer channel. */
#define IPC_N2A_OTP_TX_TRAN						6 /*!< NP to AP OTP TX transfer channel. */
#define IPC_N2A_IMQ_TRX_TRAN					7	/*!<  NP -->  AP IMQ Message Exchange */

/*
 * IPC_AP_Tx_Channel
 */
#define IPC_A2L_TICKLESS_INDICATION			0	/*!<  AP -->  LP Tickless Indicate */
//#define IPC_A2L_Channel1						1
#define IPC_A2L_UARTBRIDGE						2 /*!< AP to LP UART bridge channel. */
//#define IPC_A2L_Channel3						3
//#define IPC_A2L_Channel4						4
//#define IPC_A2L_Channel5						5
//#define IPC_A2L_Channel6						6
#define IPC_A2L_IMQ_TRX_TRAN					7	/*!<  AP -->  LP IMQ Message Exchange */

#define IPC_A2N_WIFI_TRX_TRAN					0	/*!<  AP -->  NP WIFI Message Exchange */
#define IPC_A2N_WIFI_API_TRAN					1	/*!<  AP -->  NP WIFI API Message Exchange */
#define IPC_A2N_FLASHPG_REQ						2	/*!<  AP -->  NP Flash Program Request*/
#define IPC_A2N_COEX_API_TRAN					3	/*!<  AP -->  NP COEX API Exchange */
#define IPC_A2N_DIAGNOSE							4	/*!<  AP -->  NP Diagnose API Message Exchange */
//#define IPC_A2N_Channel5						5
// #define IPC_A2N_BT_DRC_TRAN						4	/*!<  Recycled: AP -->  NP BT DATA Message Exchange */
// #define IPC_A2N_802154_TRAN						5 /*!<  Recycled */
#define IPC_A2N_OTP_RX_TRAN						6 /*!< AP to NP OTP RX transfer channel. */
#define IPC_A2N_LOGUART_RX_SWITCH				7	/*!<  AP -->  NP Loguart Message Exchange for Linux*/
#define IPC_A2N_IMQ_TRX_TRAN					7	/*!<  AP -->  NP IMQ Message Exchange for RTOS*/

#if (defined(CONFIG_ARM_CORE_CM0))
#define IPC_CH_WIFI_FW_CTRL			1 /*!< WiFi firmware control IPC channel number (CM0 only). */
#define IPC_FW_CA2LP_CHNUM			17 /*!< CA to LP firmware IPC channel number (CM0 only). */
#define IPC_FW_KM2LP_CHNUM			25 /*!< KM to LP firmware IPC channel number (CM0 only). */
#endif
/** @} */

/** @} */

IPC_TypeDef *IPC_GetDevById(u32 cpu_id);
u32 IPC_SEMTake(u32 SEM_Idx, u32 timeout);
u32 IPC_SEMFree(u32 SEM_Idx);
void IPC_SEMDelayStub(void (*pfunc)(uint32_t));
void IPC_patch_function(void (*pfunc1)(u32), void (*pfunc2)(u32));

/* Exported functions --------------------------------------------------------*/
/** @defgroup IPC_Exported_Functions IPC Exported Functions
  * @{
  */

void IPC_INTConfig(IPC_TypeDef *IPCx, u8 IPC_Shiftbit, u32 NewState);
void IPC_IERSet(IPC_TypeDef *IPCx, u32 IPC_Chs);
u32 IPC_IERGet(IPC_TypeDef *IPCx);
u32 IPC_INTRequest(IPC_TypeDef *IPCx, IPC_Direction_Mode IPC_Dir, u8 IPC_ChNum);
u32 IPC_INTGet(IPC_TypeDef *IPCx);
void IPC_INTClear(IPC_TypeDef *IPCx, u8 IPC_Shiftbit);
u32 IPC_INTHandler(void *Data);
void IPC_INTUserHandler(IPC_TypeDef *IPCx, u8 IPC_Shiftbit, void *IrqHandler, void *IrqData);

/** @} */

/** @} */

/** @} */

/* Other definitions --------------------------------------------------------*/

extern const IPC_INIT_TABLE  ipc_init_config[];

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif
