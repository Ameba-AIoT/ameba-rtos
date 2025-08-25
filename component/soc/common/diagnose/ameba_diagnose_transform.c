/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_diagnose_transform.h"
#include "ameba_diagnose_types.h"

u8 g_diag_trans_print = 0;
int rtk_diag_uart_send(const RtkDiagDataFrame_t *frame)
{
	uint8_t *src = (uint8_t *)frame;
	u16 len = RTK_DIAG_FRAME_STRUCTURE_REAL_SIZE(frame);
	for (u16 i = 0; i < len; i++) {
#ifndef CONFIG_AMEBAD
		LOGUART_PutChar_RAM(src[i]);
#else
		//TODO: replace with LOGUART_PutChar_RAM, LOGUART_PutChar would add 0x0D if src[i] is 0x0A automatically
		LOGUART_PutChar(src[i]);
#endif
	}
	// if(g_diag_trans_print) {
	//   for (u16 i = 0; i < len; i++) {
	//     DiagPrintf("%02x", src[i]);
	//     if (i == 4) {
	//       DiagPrintf(" || ");
	//     }else if (i >= 6 && (((i - 6) % 14) == 0)) {
	//       DiagPrintf("\n  ");
	//     } else {
	//       LOGUART_PutChar(' ');
	//     }
	//   }
	//   DiagPrintf("\n");
	// }
	return RTK_SUCCESS;
}

int rtk_diag_tcp_send(const RtkDiagDataFrame_t *frame)
{
	(void)frame;
	return RTK_SUCCESS;
}
