/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_IPC_API_H
#define _AMEBA_IPC_API_H

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
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup IPC_Exported_Constants IPC Exported Constants
  * @{
  */
/** @defgroup IPC_RETURN_VALUE IPC Return Value
  * @{
  */
#define IPC_SEMA_TIMEOUT 3  /*!< IPC semaphore timeout error code. */
#define IPC_REQ_TIMEOUT  2  /*!< IPC request timeout error code. */
#define IPC_SEND_SUCCESS 0  /*!< IPC message sent successfully. */
#define IPC_SEND_TIMEOUT 1  /*!< IPC send timeout error code. */
#define IPC_SEMA_MAX_DELAY 0x10000 /*!< IPC semaphore maximum delay in ms (65536ms). */
/**
  * @}
  */
/**
  * @}
  */



/* Exported functions --------------------------------------------------------*/
/** @defgroup IPC_Exported_Functions IPC Exported Functions
  * @{
  */
void ipc_table_init(IPC_TypeDef *IPCx);
u32 ipc_send_message(IPC_Direction_Mode IPC_Dir, u8 IPC_ChNum, PIPC_MSG_STRUCT IPC_Msg);
PIPC_MSG_STRUCT ipc_get_message(IPC_Direction_Mode IPC_Dir, u8 IPC_ChNum);
void IPC_TXHandler(void *Data, u32 IrqStatus, u32 ChanNum);
/** @} */

/** @} */

/** @} */

extern IPC_IRQ_FUN IPC_IrqHandler[IPC_CHANNEL_NUM];


#endif
