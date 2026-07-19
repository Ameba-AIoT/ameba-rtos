/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/* TX path NOT_FULL bits indexed by SYS_CPUID(): 0=KM0(TP2), 1=KM4(TP1), 2=CA32(TP4).
 * Mirrors LOG_UART_IDX_FLAG[] in ROM (ameba_loguart_rom.c).
 */
static const u32 loguart_not_full_bits[] = {
	LOGUART_BIT_TP2F_NOT_FULL,	/* CPUID 0: KM0 */
	LOGUART_BIT_TP1F_NOT_FULL,	/* CPUID 1: KM4 */
	LOGUART_BIT_TP4F_NOT_FULL,	/* CPUID 2: CA32 */
};

/**
  * @brief  Check if there is space in loguart TX path FIFO.
  * @return TX path FIFO is writable or not:
  *        - 1: TX path FIFO is not full
  *        - 0: TX path FIFO is full
  */
u8 LOGUART_Writable(void)
{
	LOGUART_TypeDef *UARTLOG = LOGUART_DEV;
	u32 CPUID = SYS_CPUID();

	return (UARTLOG->LOGUART_UART_LSR & loguart_not_full_bits[CPUID]) ? 1U : 0U;
}
