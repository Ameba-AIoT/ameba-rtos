/*
 * Copyright (c) 2015, Realtek Semiconductor Corp. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch.h>
#include <arch_helpers.h>
#include <assert.h>
#include <common/debug.h>
#include <platform_def.h>
#include <stdint.h>
#include <string.h>
#include <lib/mmio.h>
#include "sheipa_ipc.h"


unsigned int ipc_send_message(unsigned int IPC_Dir, unsigned char IPC_ChNum, PIPC_MSG_STRUCT IPC_Msg)
{
	uint32_t msg_idx;
	uint32_t val;
	uint32_t *ipc_msg_addr;

	msg_idx = 16 * ((IPC_Dir >> 4) & 0xF) + 8 * (IPC_Dir & 0xF) + IPC_ChNum;
	ipc_msg_addr = (uint32_t *)(KM0_IPC_RAM + msg_idx * 16);
	memcpy(ipc_msg_addr, IPC_Msg, sizeof(IPC_MSG_STRUCT));

	clean_dcache_range((uintptr_t)ipc_msg_addr, sizeof(IPC_MSG_STRUCT));

	/* wait all cmd done */
	__asm volatile("isb");

	val = mmio_read_32(IPCAP_REG_BASE);
	val |= (BIT(IPC_ChNum + 8));
	mmio_write_32(IPCAP_REG_BASE, val);

	return 0;
}

