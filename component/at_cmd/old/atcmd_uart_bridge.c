/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "os_wrapper.h"

#define HCI_UART_BAUDRATE 115200
#define LOGUART_BAUDRATE 1500000

static uint8_t check_byte_num = 0;

void bt_uart_bridge_close(void)
{
	u32 TempVal;

	LOGUART_WaitTxComplete();

	/*restore the Baud register value*/
	LOGUART_SetBaud(LOGUART_DEV, LOGUART_BAUDRATE);

	LOGUART_INT_AP2NP();

	TempVal = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BT_CTRL0);
	TempVal &= ~(LSYS_BIT_FORCE_LOGUART_USE_LOGUART_PAD_B | LSYS_BIT_WL_USE_REQ);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BT_CTRL0, TempVal);
}

uint8_t bt_uart_bridge_close_pattern(uint8_t rc)
{
	if (check_byte_num != 4) {

		switch (rc) {
		case 'A':
			check_byte_num = 1;
			break;
		case 'T':
			if (check_byte_num == 1) {
				check_byte_num = 2;
			} else {
				check_byte_num = 0;
			}
			break;
		case 'M':
			if (check_byte_num == 2) {
				check_byte_num = 3;
			} else {
				check_byte_num = 0;
			}
			break;
		case '2':
			if (check_byte_num == 3) {
				check_byte_num = 4;
			} else {
				check_byte_num = 0;
			}
			break;

		default:
			check_byte_num = 0;
			break;
		}
	}

	if (check_byte_num == 4) {
		/* cmd str ends with <CR + LF> in Ameba Trace Tool */
		/* cmd str may end with <CR> or <CR + LF> or <LF> in Tera Term */
		/* stay consistent with KB_ASCII_CR in shell_cmd_chk */
		if (rc == '\r') {
			check_byte_num = 0;
			bt_uart_bridge_close();
			return TRUE;
		}
	}

	return FALSE;
}

static u32 bt_uart_bridge_irq(void *data)
{
	(void)data;

	uint8_t rc = 0;
	uint32_t reg_lsr = LOGUART_GetStatus(LOGUART_DEV);

	/* when rx FIFO not empty */
	if ((reg_lsr & RUART_BIT_RXFIFO_INT) || (reg_lsr & RUART_BIT_TIMEOUT_INT)) {
		while (LOGUART_Readable()) {
			rc = LOGUART_GetChar(FALSE);
			bt_uart_bridge_close_pattern(rc);
		}

		/* clear timeout interrupt flag */
		if (reg_lsr & RUART_BIT_TIMEOUT_INT) {
			LOGUART_INTClear(LOGUART_DEV, RUART_BIT_TOICF);
		}
	}

	if (reg_lsr & RUART_BIT_RXFIFO_ERR) {
		LOGUART_INTConfig(LOGUART_DEV, RUART_BIT_ELSI, DISABLE);
		LOGUART_INTClear(LOGUART_DEV, RUART_BIT_RLSICF);
	}

	return 0;
}

void bt_uart_bridge_open(void)
{
	u32 TempVal;

	LOGUART_WaitTxComplete();

	TempVal = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BT_CTRL0);
	TempVal |= (LSYS_BIT_FORCE_LOGUART_USE_LOGUART_PAD_B | LSYS_BIT_WL_USE_REQ);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BT_CTRL0, TempVal);

	/*set Baud*/
	LOGUART_SetBaud(LOGUART_DEV, HCI_UART_BAUDRATE);

	/* Switch LOGUART interrupt from NP to AP */
	LOGUART_INT_NP2AP();

	/* Register Log Uart Callback function */
	irq_register((IRQ_FUN)bt_uart_bridge_irq, UART_LOG_IRQ, (uint32_t)NULL, INT_PRI4);
	irq_enable(UART_LOG_IRQ);
}
