/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "os_wrapper.h"

#define HCI_UART_BAUDRATE	115200
#define LOGUART_BAUDRATE	1500000

#define HCI_UART_DEV	(UART3_DEV)
#define HCI_UART_IRQ	(UART3_BT_IRQ)

static uint8_t check_byte_num = 0;
static uint32_t temp_uart_aggc = 0;

#if defined(CONFIG_AMEBALITE)
#if defined(ARM_CORE_CM4)
static void *UartBkFunc = NULL;
#endif
#endif

static uint32_t bt_hci_uart_bridge_irq(void *data)
{
	(void)data;

	uint8_t ch;
	uint32_t reg_lsr = UART_LineStatusGet(HCI_UART_DEV);

	if ((reg_lsr & RUART_BIT_RXFIFO_INT) || (reg_lsr & RUART_BIT_TIMEOUT_INT)) {
		/* clear timeout interrupt flag */
		if (reg_lsr & RUART_BIT_TIMEOUT_INT) {
			UART_INT_Clear(HCI_UART_DEV, RUART_BIT_TOICF);
		}

		while (UART_Readable(HCI_UART_DEV)) {
			UART_CharGet(HCI_UART_DEV, &ch);
			LOGUART_PutChar_RAM(ch);
		}
	}

	if (reg_lsr & RUART_BIT_RXFIFO_ERR) {
		UART_INTConfig(HCI_UART_DEV, RUART_BIT_ELSI, DISABLE);
		UART_INT_Clear(HCI_UART_DEV, RUART_BIT_RLSICF);
	}

	return 0;
}

void bt_hci_uart_init(void)
{
	RCC_PeriphClockCmd(APBPeriph_UART3, APBPeriph_UART3_CLOCK, ENABLE);

	UART_InitTypeDef pUARTStruct;
	UART_StructInit(&pUARTStruct);
	pUARTStruct.Parity = RUART_PARITY_DISABLE;
	UART_Init(HCI_UART_DEV, &pUARTStruct);
	UART_SetBaud(HCI_UART_DEV, HCI_UART_BAUDRATE);

	/* Disable and Enable UART Interrupt */
	InterruptDis(HCI_UART_IRQ);
	InterruptUnRegister(HCI_UART_IRQ);
	InterruptRegister((IRQ_FUN)bt_hci_uart_bridge_irq, HCI_UART_IRQ, NULL, INT_PRI4);
	InterruptEn(HCI_UART_IRQ, INT_PRI4);

	UART_INTConfig(HCI_UART_DEV, RUART_BIT_ETBEI, DISABLE);
	UART_INTConfig(HCI_UART_DEV, RUART_BIT_ERBI | RUART_BIT_ETOI, ENABLE);

	/* Enable Uart High Rate Rx Path */
	UART_RxCmd(HCI_UART_DEV, ENABLE);
}

void bt_hci_uart_deinit(void)
{
	/* Disable and Enable UART Interrupt */
	InterruptDis(HCI_UART_IRQ);
}

void bt_uart_bridge_close(void)
{
	LOGUART_PutChar_RAM('#');
	LOGUART_PutChar_RAM(0x0A);
	LOGUART_PutChar_RAM(0x0D);

	LOGUART_WaitTxComplete();
	/*restore the AGGC register value*/
	LOGUART_DEV->LOGUART_UART_AGGC = temp_uart_aggc;
	/*restore the Baud register value*/
	LOGUART_SetBaud(LOGUART_DEV, LOGUART_BAUDRATE);
	/*Open relay path*/
	LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_5, ENABLE);

#if defined(CONFIG_AMEBASMART) && (CONFIG_AMEBASMART == 1)
	irq_disable(UART_LOG_IRQ);

	/* km0 open shell loguart*/
	IPC_MSG_STRUCT ipc_msg_temp;
	ipc_msg_temp.msg = 2; // km0 open shell loguart
#if defined(ARM_CORE_CA32)
	ipc_send_message(IPC_AP_TO_LP, IPC_A2L_UARTBRIDGE, &ipc_msg_temp);
#elif defined(ARM_CORE_CM4)
	ipc_send_message(IPC_NP_TO_LP, IPC_N2L_UARTBRIDGE, &ipc_msg_temp);
#endif
#endif

#if defined(CONFIG_AMEBALITE) && (CONFIG_AMEBALITE == 1)
#if defined(RSICV_CORE_KR4)
	irq_disable(UART_LOG_IRQ);
	/* km4 open shell loguart*/
	IPC_MSG_STRUCT ipc_msg_temp;
	ipc_msg_temp.msg = 2;
	ipc_send_message(IPC_KR4_TO_KM4, IPC_R2M_UARTBRIDGE, &ipc_msg_temp);
#else
	/* km4 restore shell loguart irq func */
	UserIrqFunTable[UART_LOG_IRQ] = (IRQ_FUN)UartBkFunc;
#endif
#endif

#if defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBALITE)
	bt_hci_uart_deinit();
#endif

	/*Open log output*/
	ConfigDebugClose = 0;
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
			if (bt_uart_bridge_close_pattern(rc) != TRUE) {
				while (!UART_Writable(HCI_UART_DEV));
				UART_CharPut(HCI_UART_DEV, rc);
			}
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
	/*backup the AGGC register value*/
	temp_uart_aggc = LOGUART_DEV->LOGUART_UART_AGGC;
	LOGUART_WaitTxComplete();
	/*reset rx fifo*/
	LOGUART_ClearRxFifo(LOGUART_DEV);
	/*set Baud*/
	LOGUART_SetBaud(LOGUART_DEV, HCI_UART_BAUDRATE);
	/*close log output*/
	ConfigDebugClose = 1;
	/*close relay path to disable bt fw log*/
	LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_5, DISABLE);
	/*close AGG*/
	LOGUART_AGGCmd(LOGUART_DEV, DISABLE);

#if defined(CONFIG_AMEBALITE) && (CONFIG_AMEBALITE == 1)
#if defined(RSICV_CORE_KR4)
	/* km4 close shell loguart */
	IPC_MSG_STRUCT ipc_msg_temp;
	ipc_msg_temp.msg = 1;
	ipc_send_message(IPC_KR4_TO_KM4, IPC_R2M_UARTBRIDGE, &ipc_msg_temp);
#else
	/* km4 backup shell loguart irq func */
	UartBkFunc = (void *)UserIrqFunTable[UART_LOG_IRQ];
#endif
#endif

#if defined(CONFIG_AMEBASMART) && (CONFIG_AMEBASMART == 1)
	/* km0 close shell loguart*/
	IPC_MSG_STRUCT ipc_msg_temp;
	ipc_msg_temp.msg = 1;
#if defined(ARM_CORE_CA32)
	ipc_send_message(IPC_AP_TO_LP, IPC_A2L_UARTBRIDGE, &ipc_msg_temp);
#elif defined(ARM_CORE_CM4)
	ipc_send_message(IPC_NP_TO_LP, IPC_N2L_UARTBRIDGE, &ipc_msg_temp);
#endif
#endif

	bt_hci_uart_init();
	/* Register Log Uart Callback function */
	irq_register((IRQ_FUN)bt_uart_bridge_irq, UART_LOG_IRQ, (uint32_t)NULL, INT_PRI4);
	irq_enable(UART_LOG_IRQ);
}
