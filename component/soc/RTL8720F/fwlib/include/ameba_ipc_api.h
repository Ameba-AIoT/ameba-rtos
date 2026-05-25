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

/** @defgroup IPC
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
	u32 msg_type;
	u32 msg;
	u32 msg_len;
	u32 rsvd;
} IPC_MSG_STRUCT, *PIPC_MSG_STRUCT, ipc_msg_struct_t;
/**
  * @}
  */

/**
 * @brief IPC Message Definition
*/
typedef struct ipc_data_struct {
	u32 *data;
	u32 data_len;
	u32 rsvd[2];
} IPC_DATA_STRUCT, *PIPC_DATA_STRUCT, ipc_data_struct_t;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup IPC_Exported_Constants IPC Exported Constants
  * @{
  */
/** @defgroup IPC_RETURN_VALUE
  * @{
  */
#define IPC_RX_MALLOC_FAIL 6
#define IPC_RX_CH_ERROE   5
#define IPC_RX_FIFO_EMPTY 4
#define IPC_SEMA_TIMEOUT 3
#define IPC_REQ_TIMEOUT  2
#define IPC_SEND_SUCCESS 0
#define IPC_RECEIVE_SUCCESS 0
#define IPC_SEND_TIMEOUT 1
#define IPC_SEMA_MAX_DELAY 0x10000 // 65536ms
/**
  * @}
  */

/** @defgroup IPC_TX_Work_Mode
  * @{
  */
#define IPC_UNSHARE_MODE				0x0
#define IPC_SHARE_MODE				  0x1
/**
  * @}
  */

#define IPC_RX_DATA_LEN_MAX  16
#define IPC_TX_LEVEVL  15
#define IPC_TX_TIMEOUT 0x9c40
/**
  * @}
  */


/** @} */

/** @} */

/* Exported functions --------------------------------------------------------*/

void IPC_TXHandler(void *Data, u32 IrqStatus, u32 ChanNum);
void ipc_table_init(IPC_TypeDef *IPCx);
void IPC_SelectMode(IPC_TypeDef *IPCx, u32 share_mode);
void IPC_SetTxThreshold(IPC_TypeDef *IPCx, u32 threshold);
void IPC_SetTxtTimeout(IPC_TypeDef *IPCx, u32 timeout);
extern IPC_IRQ_FUN IPC_IrqHandler[32];


u32 ipc_send_message(u32 IPC_Dir, u8 IPC_ChNum, PIPC_MSG_STRUCT IPC_Msg);
PIPC_MSG_STRUCT ipc_get_message(u32 IPC_Dir, u8 IPC_ChNum);
u32 ipc_send_message_unshare(u32 IPC_Dir, u8 IPC_ChNum, PIPC_DATA_STRUCT IPC_Data);
u32 ipc_get_message_unshare(u32 IPC_Dir, u8 IPC_ChNum, PIPC_DATA_STRUCT IPC_Data);

#ifdef __cplusplus
}
#endif

#endif
