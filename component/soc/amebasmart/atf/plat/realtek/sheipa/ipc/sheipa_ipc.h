/*
 * Copyright (c) 2015, Realtek Semiconductor Corp. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#define IPC_AP_TO_LP			((unsigned int)0x00000020)
#define IPC_AP_TO_NP			((unsigned int)0x00000021)

typedef struct _IPC_MSG_STRUCT_ {
	uint32_t msg_type;
	uint32_t msg;
	uint32_t msg_len;
	uint32_t rsvd;
} IPC_MSG_STRUCT, *PIPC_MSG_STRUCT;


unsigned int ipc_send_message(unsigned int IPC_Dir, unsigned char IPC_ChNum, PIPC_MSG_STRUCT IPC_Msg);
