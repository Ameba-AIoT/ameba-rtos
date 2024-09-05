/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "atcmd_service.h"
#include "ameba_soc.h"
#include "os_wrapper.h"

#if defined(CONFIG_BT) && CONFIG_BT
#if defined(CONFIG_MP_INCLUDED) && CONFIG_MP_INCLUDED

static bool open_flag = 0;

#define HCI_UART_BAUDRATE        115200
#define LOGUART_BAUDRATE         1500000

static uint8_t check_byte_num = 0;

#if ((defined(CONFIG_AMEBALITE) && (CONFIG_AMEBALITE == 1)) \
	|| (defined(CONFIG_AMEBASMART) && (CONFIG_AMEBASMART == 1)))
#define HCI_UART_DEV             (UART3_DEV)
#define HCI_UART_IRQ             (UART3_BT_IRQ)

static uint32_t temp_uart_aggc = 0;

#if (defined(CONFIG_AMEBALITE) && (CONFIG_AMEBALITE == 1))
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

static void bt_hci_uart_init(void)
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

static void bt_hci_uart_deinit(void)
{
	/* Disable and Enable UART Interrupt */
	InterruptDis(HCI_UART_IRQ);
}
#endif /* CONFIG_AMEBALITE or CONFIG_AMEBASMART */

static void bt_uart_bridge_close(void)
{
#if ((defined(CONFIG_AMEBALITE) && (CONFIG_AMEBALITE == 1)) \
	|| (defined(CONFIG_AMEBASMART) && (CONFIG_AMEBASMART == 1)))
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

	bt_hci_uart_deinit();
	/*Open log output*/
	ConfigDebugClose = 0;

#else /* not (CONFIG_AMEBALITE or CONFIG_AMEBASMART) */
	u32 TempVal;

	LOGUART_WaitTxComplete();
	/*restore the Baud register value*/
	LOGUART_SetBaud(LOGUART_DEV, LOGUART_BAUDRATE);
	LOGUART_INT_AP2NP();

	TempVal = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BT_CTRL0);
	TempVal &= ~(LSYS_BIT_FORCE_LOGUART_USE_LOGUART_PAD_B | LSYS_BIT_WL_USE_REQ);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BT_CTRL0, TempVal);
#endif /* CONFIG_AMEBALITE or CONFIG_AMEBASMART */
}

static uint8_t bt_uart_bridge_close_pattern(uint8_t rc)
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

	uint8_t rc = 0, ret = 0;
	uint32_t reg_lsr = LOGUART_GetStatus(LOGUART_DEV);

	/* when rx FIFO not empty */
	if ((reg_lsr & RUART_BIT_RXFIFO_INT) || (reg_lsr & RUART_BIT_TIMEOUT_INT)) {
		while (LOGUART_Readable()) {
			rc = LOGUART_GetChar(FALSE);
			ret = bt_uart_bridge_close_pattern(rc);
#if ((defined(CONFIG_AMEBALITE) && (CONFIG_AMEBALITE == 1)) \
	|| (defined(CONFIG_AMEBASMART) && (CONFIG_AMEBASMART == 1)))
			if (ret != TRUE) {
				while (!UART_Writable(HCI_UART_DEV));
				UART_CharPut(HCI_UART_DEV, rc);
			}
#else
			UNUSED(ret);
#endif
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
#if ((defined(CONFIG_AMEBALITE) && (CONFIG_AMEBALITE == 1)) \
	|| (defined(CONFIG_AMEBASMART) && (CONFIG_AMEBASMART == 1)))
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

#else /* not (CONFIG_AMEBALITE or CONFIG_AMEBASMART) */
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
#endif /* CONFIG_AMEBALITE or CONFIG_AMEBASMART */
}

static int mp_ext2_uart_bridge(void **argv, int argc)
{
	MP_EXT2_PRINTF("_AT_MP_BRIDGE_\n");

	if (argc == 1) {
		if (strncmp(argv[0], "close", 5) == 0) {
			MP_EXT2_PRINTF("close UART bridge.\n");
			if (open_flag == 0) {
				return 0;
			}

			bt_uart_bridge_close();
			open_flag = 0;
			return 0;
		}
	}

	MP_EXT2_PRINTF("open UART bridge.\n");
	bt_uart_bridge_open();
	open_flag = 1;

	return 0;
}

static int mp_ext2_bt_power(void **argv, int argc)
{
	(void)argc;

	if (strcmp(argv[0], "on") == 0) {
		MP_EXT2_PRINTF("BT power on.\n\r");
		rtk_bt_mp_power_on();
	} else if (strcmp(argv[0], "off") == 0) {
		MP_EXT2_PRINTF("BT power off.\n\r");
		rtk_bt_mp_power_off();
	}

	return 0;
}

static int mp_ext2_gnt_bt(void **argv, int argc)
{
	(void)argc;

	if (strcmp(argv[0], "wifi") == 0) {
		MP_EXT2_PRINTF("Switch GNT_BT to WIFI.\n\r");
		wifi_btcoex_set_pta(PTA_WIFI, PTA_HOST_BT, COMMON_ACTION);
	} else if (strcmp(argv[0], "bt") == 0) {
		MP_EXT2_PRINTF("Switch GNT_BT to BT.\n\r");
		wifi_btcoex_set_pta(PTA_BT, PTA_HOST_BT, COMMON_ACTION);
	}

	return 0;
}

static int mp_ext2_ant(void **argv, int argc)
{
	(void)argc;

	if (strcmp(argv[0], "s0") == 0) {
		MP_EXT2_PRINTF("BT use dedicated RF s0.\n\r");
		wifi_btcoex_set_bt_ant(0);
		rtk_bt_set_bt_antenna(0);
	} else if (strcmp(argv[0], "s1") == 0) {
		MP_EXT2_PRINTF("BT use share RF s1.\n\r");
		wifi_btcoex_set_bt_ant(1);
		rtk_bt_set_bt_antenna(1);
	}

	return 0;
}

at_mp_ext_item_t at_mp_ext2_items[] = {
	{"bridge",		mp_ext2_uart_bridge,		UART_BRIDGE_USAGE},
	{"bt_power",	mp_ext2_bt_power,			BT_POWER_USAGE},
	{"gnt_bt",		mp_ext2_gnt_bt,				GNT_BT_USAGE},
	{"ant",			mp_ext2_ant,				SELECTION_BT_ANTENNA},
};

void fATM2(void *arg)
{
	int argc = 0, idx, cmd_cnt;
	char *argv[MAX_ARGC] = {0};

	cmd_cnt = sizeof(at_mp_ext2_items) / sizeof(at_mp_ext2_items[0]);
	argc = parse_param(arg, argv);
	if (argc == 1) {
                RTK_LOGS(NOTAG, "\n");
		MP_EXT2_PRINTF("Command usage :\n");
		for (idx = 0; idx < cmd_cnt; idx++) {
			RTK_LOGS(NOTAG, "%s", at_mp_ext2_items[idx].mp_ext_usage);
		}
	} else {
		for (idx = 0; idx < cmd_cnt; idx++) {
			if (strcmp(argv[1], at_mp_ext2_items[idx].mp_ext_cmd) == 0) {
				int (*fun)(void **argv, int argc) = at_mp_ext2_items[idx].mp_ext_fun;
				fun((void **)&argv[2], argc - 2);
				return;
			}
		}
		MP_EXT2_PRINTF("unknown command %s.\n", argv[1]);
	}
}

//-------- AT MP commands ---------------------------------------------------------------
log_item_t at_mp_items[] = {
	{"M2", fATM2, {NULL, NULL}},	// MP ext2 AT command
};

/* TODO: A part of AT command "AT+LIST". */
void print_bt_mp_at(void)
{
}

void at_mp_init(void)
{
	atcmd_service_add_table(at_mp_items, sizeof(at_mp_items) / sizeof(at_mp_items[0]));
}

#endif /* #if CONFIG_ATCMD_MP */
#endif /* #if CONFIG_BT */

