/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/**
  * @brief Switch LOGUART interrupt from NP to AP.
  * @param None
  * @retval None
  * @note KM0 is NP and KM4 is AP.
  */
void LOGUART_INT_NP2AP(void)
{
	LOGUART_INTCoreConfig(LOGUART_DEV, LOGUART_BIT_INTR_MASK_KM0, DISABLE);
	LOGUART_INTCoreConfig(LOGUART_DEV, LOGUART_BIT_INTR_MASK_KM4, ENABLE);
}

/**
  * @brief Switch LOGUART interrupt from AP to NP.
  * @param None
  * @retval None
  * @note KM0 is NP and KM4 is AP.
  */
void LOGUART_INT_AP2NP(void)
{
	LOGUART_INTCoreConfig(LOGUART_DEV, LOGUART_BIT_INTR_MASK_KM4, DISABLE);
	LOGUART_INTCoreConfig(LOGUART_DEV, LOGUART_BIT_INTR_MASK_KM0, ENABLE);
}

/**
  * @brief  Set LOGUART IMR
  * @param  SetValue: specifies the LOGUART interrupt sources to be enabled or disabled.
  *          This parameter can be one or combinations of the following values:
  *            @arg LOGUART_BIT_ERBI:  Received Data Available interrupt
  *            @arg LOGUART_BIT_ELSI:  Receiver Line Status interrupt
  *            @arg LOGUART_BIT_ETOI: Rx Time Out interrupt
  *            @arg LOGUART_BIT_EDSSI: Modem Status Interrupt
  *            @arg LOGUART_TX_EMPTY_PATH_1_INTR: Tx Path 1 Transmitter FIFO Empty interrupt
  *            @arg LOGUART_TX_EMPTY_PATH_2_INTR: Tx Path 2 Transmitter FIFO Empty interrupt
  *            @arg LOGUART_TX_EMPTY_PATH_3_INTR: Tx Path 3 Transmitter FIFO Empty interrupt
  *            @arg LOGUART_TX_EMPTY_PATH_4_INTR: Tx Path 4 Transmitter FIFO Empty interrupt
  * @retval none
  */
void LOGUART_INTConfig(LOGUART_TypeDef *UARTLOG, u32 UART_IT, u32 newState)
{
	(void)UARTLOG;

	u32 IerVal = LOGUART_GetIMR();

	if (IS_LOGUART_TX_EMPTY_ENABLE(UART_IT & LOGUART_MASK_ETPFEI)) {
		IerVal &= ~LOGUART_MASK_ETPFEI;
	}

	if (newState == ENABLE) {
		/* Enable the selected UARTx interrupts */
		IerVal |= UART_IT;
	} else {
		/* Disable the selected UARTx interrupts */
		IerVal &= (u32)~(UART_IT & (~LOGUART_MASK_ETPFEI));
	}

	LOGUART_SetIMR(IerVal);
}

extern LOG_UART_PORT LOG_UART_IDX_FLAG[];
/**
* @brief  Check if there is space in loguart tx path fifo.
* @retval Status value:
*          - 1: tx path fifo is not full
*          - 0: tx path fifo is full
*/
u8 LOGUART_Writable(void)
{
	LOGUART_TypeDef *UARTLOG = LOGUART_DEV;
	u32 CPUID = SYS_CPUID();

	if (UARTLOG->LOGUART_UART_LSR & LOG_UART_IDX_FLAG[CPUID].not_full) {
		return 1;
	} else {
		return 0;
	}
}

/**
 * @brief Wait until current tx path FIFO turns empty.
 * @return None
 * @note This function is called to check current tx path FIFO is empty or not.
 * 		To check if tx process has been finished, call LOGUART_WaitTxComplete instead.
 */
void LOGUART_WaitBusy(void)
{
	LOGUART_TypeDef *UARTLOG = LOGUART_DEV;
	u32 CPUID = SYS_CPUID();

	/* Wait for LogUart print out */
	while (1) {
		if (UARTLOG->LOGUART_UART_LSR & LOG_UART_IDX_FLAG[CPUID].empty) {
			break;
		}

		/* exit while-loop ASAP */
		DelayUs(1);
	}
}

/**
 * @brief Wait until tx process finishes.
 * @return None
 * @note This function is called to wait until tx data is completely pushed from tx FIFO
 * 		and all the tx path FIFO is empty.
 */
void LOGUART_WaitTxComplete(void)
{
	LOGUART_TypeDef *UARTLOG = LOGUART_DEV;
	u32 lsr;

	while (1) {
		lsr = UARTLOG->LOGUART_UART_LSR;

		if ((lsr & LOGUART_BIT_TP1F_EMPTY) && (lsr & LOGUART_BIT_TP2F_EMPTY) && (lsr & LOGUART_BIT_TP3F_EMPTY)  && \
			(lsr & LOGUART_BIT_TP4F_EMPTY) && (!(lsr & LOGUART_BIT_RPF_DRDY)) && (lsr & LOGUART_BIT_TX_EMPTY)) {
			break;
		}

		/* exit while-loop ASAP */
		DelayUs(1);
	}
}

/**
 * @brief Transmit single byte to current tx path FIFO without appending characters.
 * @param c Data to be transmitted.
 * @return None
 */
void LOGUART_PutChar_RAM(u8 c)
{
	LOGUART_TypeDef *UARTLOG = LOGUART_DEV;
	u32 CPUID = SYS_CPUID();

	if (ConfigDebugClose == 1) {
		return;
	}

	LOGUART_WaitTx();

	UARTLOG->LOGUART_UART_THRx[LOG_UART_IDX_FLAG[CPUID].idx] = c;
}
