/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_IPC_API_H
#define _AMEBA_IPC_API_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup IPC IPC
  * @brief IPC driver modules
  * @{
  */
/* Exported types --------------------------------------------------------*/
/** @addtogroup IPC_Exported_Types IPC Exported Types
  * @{
  */
/**
  * @brief IPC Message Definition
 */
typedef struct ipc_msg_struct {
	u32 msg_type; /*!< Message type identifier. */
	u32 msg;      /*!< Message payload or address. */
	u32 msg_len;  /*!< Length of the message payload. */
	u32 rsvd;     /*!< Reserved. */
} IPC_MSG_STRUCT, *PIPC_MSG_STRUCT, ipc_msg_struct_t;

/**
 * @brief IPC Data Structure Definition
*/
typedef struct ipc_data_struct {
	u32 *data;       /*!< Pointer to the data buffer. */
	u32 data_len;    /*!< Length of the data buffer in bytes. */
	u32 rsvd[2];     /*!< Reserved. */
} IPC_DATA_STRUCT, *PIPC_DATA_STRUCT, ipc_data_struct_t;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup IPC_Exported_Constants IPC Exported Constants
  * @{
  */
/** @defgroup IPC_RETURN_VALUE IPC Return Value
  * @{
  */
#define IPC_RX_MALLOC_FAIL 6  /*!< RX memory allocation failure. */
#define IPC_RX_CH_ERROE   5  /*!< RX channel error. */
#define IPC_RX_FIFO_EMPTY 4  /*!< RX FIFO is empty. */
#define IPC_SEMA_TIMEOUT 3  /*!< IPC semaphore timeout error code. */
#define IPC_REQ_TIMEOUT  2  /*!< IPC request timeout error code. */
#define IPC_SEND_SUCCESS 0  /*!< IPC message sent successfully. */
#define IPC_RECEIVE_SUCCESS 0  /*!< Message received successfully. */
#define IPC_SEND_TIMEOUT 1  /*!< IPC send timeout error code. */
#define IPC_SEMA_MAX_DELAY 0x10000 /*!< IPC semaphore maximum delay in ms (65536ms). */
/**
  * @}
  */

/** @defgroup IPC_TX_Work_Mode IPC TX Work Mode
  * @{
  */
#define IPC_UNSHARE_MODE				0x0  /*!< IPC TX unshared (dedicated) mode. */
#define IPC_SHARE_MODE				  0x1  /*!< IPC TX shared mode. */
/**
  * @}
  */

#define IPC_RX_DATA_LEN_MAX  16  /*!< Maximum RX data length in bytes. */
#define IPC_TX_LEVEVL  15  /*!< IPC TX FIFO level threshold. */
#define IPC_TX_TIMEOUT 0x9c40  /*!< IPC TX operation timeout value. */
/**
  * @}
  */




/* Exported functions --------------------------------------------------------*/
/** @defgroup IPC_Exported_Functions IPC Exported Functions
  * @{
  */
void IPC_TXHandler(void *Data, u32 IrqStatus, u32 ChanNum);
void ipc_table_init(IPC_TypeDef *IPCx);
void IPC_SelectMode(IPC_TypeDef *IPCx, u32 share_mode);
void IPC_SetTxThreshold(IPC_TypeDef *IPCx, u32 threshold);
void IPC_SetTxtTimeout(IPC_TypeDef *IPCx, u32 timeout);
u32 ipc_send_message(IPC_Direction_Mode IPC_Dir, u8 IPC_ChNum, PIPC_MSG_STRUCT IPC_Msg);
PIPC_MSG_STRUCT ipc_get_message(IPC_Direction_Mode IPC_Dir, u8 IPC_ChNum);
u32 ipc_send_message_unshare(IPC_Direction_Mode IPC_Dir, u8 IPC_ChNum, PIPC_DATA_STRUCT IPC_Data);
u32 ipc_get_message_unshare(IPC_Direction_Mode IPC_Dir, u8 IPC_ChNum, PIPC_DATA_STRUCT IPC_Data);
/** @} */

/** @} */

/** @} */

extern IPC_IRQ_FUN IPC_IrqHandler[32];

#ifdef __cplusplus
}
#endif

#endif
