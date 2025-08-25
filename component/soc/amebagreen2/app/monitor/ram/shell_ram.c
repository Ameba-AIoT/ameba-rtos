/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "FreeRTOS.h"

extern volatile UART_LOG_CTL		shell_ctl;
#ifdef CONFIG_LONGER_CMD
#define CMD_BUFLEN UART_LOG_CMD_BUFLEN_L
extern UART_LOG_BUF shell_buf;
extern UART_LOG_BUF shell_rxbuf;
u8 shell_buf_array_l[CMD_BUFLEN];
u8 shell_rxbuf_array_l[CMD_BUFLEN];
#else
#define CMD_BUFLEN UART_LOG_CMD_BUFLEN
#endif

rtos_sema_t	shell_sema = NULL;

#ifdef CONFIG_SUPPORT_ATCMD
char atcmd_buf[CMD_BUFLEN];
extern int atcmd_service(char *line_buf);
extern void atcmd_service_init(void);
#endif

#ifdef CONFIG_MP_INCLUDED
extern int mp_command_handler(char *cmd);
#endif

static monitor_cmd_handler shell_get_cmd(char *argv)
{
	PCOMMAND_TABLE  pCmdTbl = shell_ctl.pCmdTbl;
	u32 CmdCnt = 0;
	u32 CmdNum = shell_ctl.CmdTblSz;

	monitor_cmd_handler cmd_handler = NULL;

	for (CmdCnt = 0; CmdCnt < CmdNum; CmdCnt++) {
		if ((_stricmp(argv, (const char *)pCmdTbl[CmdCnt].cmd)) == 0) {
			cmd_handler = pCmdTbl[CmdCnt].func;
			break;
		}
	}

	return cmd_handler;
}

static void shell_give_sema(void)
{
	if (shell_ctl.shell_task_rdy) {
		rtos_sema_give(shell_sema);
	}
}

__attribute__((noinline)) static u32 shell_cmd_exec_ram(u8 *pbuf)
{
	monitor_cmd_handler cmd_handler = NULL;
	u8 argc = shell_get_argc((const u8 *) pbuf);
	u8 **argv = shell_get_argv((const u8 *) pbuf);

	cmd_handler = shell_get_cmd((char *)argv[0]);
	if (cmd_handler != NULL) {
		cmd_handler((argc - 1), (argv + 1));
		return TRUE;
	}

	return FALSE;
}

void shell_loguratRx_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	(void) Data;
	(void) IrqStatus;
	(void) ChanNum;

	PUART_LOG_BUF ptmpbuf;
	PUART_LOG_BUF pUartLogBuf = shell_ctl.pTmpLogBuf;
	PIPC_MSG_STRUCT	ipc_msg_temp = (PIPC_MSG_STRUCT)ipc_get_message(IPC_NP_TO_AP, IPC_N2A_LOGUART_RX_SWITCH);

	ptmpbuf = (PUART_LOG_BUF) ipc_msg_temp->msg;
	DCache_Invalidate((u32)ptmpbuf, sizeof(UART_LOG_BUF));
	DCache_Invalidate((u32)ptmpbuf->UARTLogBuf, ptmpbuf->UARTLogBufLen);
	pUartLogBuf->BufCount = ptmpbuf->BufCount;
	_memcpy(pUartLogBuf->UARTLogBuf, ptmpbuf->UARTLogBuf, MIN(ptmpbuf->UARTLogBufLen, pUartLogBuf->UARTLogBufLen));

	shell_ctl.ExecuteCmd = TRUE;
	if (shell_ctl.shell_task_rdy) {
		shell_ctl.GiveSema();
	}
}

#ifdef CONFIG_ARM_CORE_CM4_KM4NS
u8 tmp_log_buf_array[CMD_BUFLEN];
UART_LOG_BUF tmp_log_buf = {
	.UARTLogBuf = tmp_log_buf_array,
	.UARTLogBufLen = CMD_BUFLEN,
};//recv CPU Wr and Other CPU Rd

void shell_loguratRx_Ipc_Tx(u32 ipc_dir, u32 ipc_ch)
{
	IPC_MSG_STRUCT ipc_msg_temp;
	PUART_LOG_BUF pUartLogBuf = shell_ctl.pTmpLogBuf;

	tmp_log_buf.BufCount = pUartLogBuf->BufCount;
	_memcpy(tmp_log_buf.UARTLogBuf, pUartLogBuf->UARTLogBuf, MIN(pUartLogBuf->UARTLogBufLen, tmp_log_buf.UARTLogBufLen));
	DCache_CleanInvalidate((u32)&tmp_log_buf, sizeof(UART_LOG_BUF));
	DCache_CleanInvalidate((u32)(tmp_log_buf.UARTLogBuf), tmp_log_buf.UARTLogBufLen);

	ipc_msg_temp.msg_type = IPC_USER_POINT;
	ipc_msg_temp.msg = (u32)&tmp_log_buf;
	ipc_msg_temp.msg_len = 1;
	ipc_msg_temp.rsvd = 0; /* for coverity init issue */
	ipc_send_message(ipc_dir, ipc_ch, &ipc_msg_temp);
}

void shell_loguartRx_dispatch(void)
{
	u32 i, CpuId, cmd_done = FALSE;
	PUART_LOG_BUF pUartLogBuf = shell_ctl.pTmpLogBuf;

	for (i = 0; i < pUartLogBuf->UARTLogBufLen; i++) {
		if (pUartLogBuf->UARTLogBuf[i] != ' ') {
			if (pUartLogBuf->UARTLogBuf[i] == '\0') {
				CpuId = NP_CPU_ID;
				CONSOLE_AMEBA(); /* '\0' put # */
				cmd_done = TRUE;
				break;
			}

			if (pUartLogBuf->UARTLogBuf[i] == '@') {
				i = i + 1; /* remove flag */
				CpuId = NP_CPU_ID;
			} else {
				CpuId = AP_CPU_ID;
			}

			/* avoid useless space */
			_memcpy(&pUartLogBuf->UARTLogBuf[0], &pUartLogBuf->UARTLogBuf[i], pUartLogBuf->UARTLogBufLen - i);
			break;
		}
	}

	if (CpuId == AP_CPU_ID) {		/* CMD should processed by AP, inform AP thru IPC */
		shell_loguratRx_Ipc_Tx(IPC_NP_TO_AP, IPC_N2A_LOGUART_RX_SWITCH);
		cmd_done = TRUE;
	}

	if (cmd_done) {
		shell_array_init((u8 *)pUartLogBuf->UARTLogBuf, pUartLogBuf->UARTLogBufLen, '\0');
		pUartLogBuf->BufCount = 0;
		shell_ctl.ExecuteCmd = FALSE;
	} else {
		shell_ctl.ExecuteCmd = TRUE;
	}
}
#else
void shell_loguartRx_dispatch(void)
{
}
#endif

static void shell_task_ram(void *Data)
{
	/* To avoid gcc warnings */
	(void) Data;
	u32 ret = FALSE;
	PUART_LOG_BUF pUartLogBuf = shell_ctl.pTmpLogBuf;

	//4 Set this for UartLog check cmd history
	shell_ctl.shell_task_rdy = 1;

	rtos_create_secure_context(RTOS_MINIMAL_SECURE_STACK_SIZE);

	do {
		rtos_sema_take(shell_sema, RTOS_MAX_DELAY);

		shell_loguartRx_dispatch();

		if (shell_ctl.ExecuteCmd) {
#if defined(CONFIG_SUPPORT_ATCMD)
			shell_array_init((u8 *)atcmd_buf, (u8)sizeof(atcmd_buf), '\0');
			strcpy(atcmd_buf, (const char *)pUartLogBuf->UARTLogBuf);
			ret = atcmd_service(atcmd_buf);

#ifdef CONFIG_MP_INCLUDED
			if (ret == FALSE) {
				ret = mp_command_handler((char *)pUartLogBuf->UARTLogBuf);
			}
#endif
#endif
			if (ret == FALSE) {
				if (shell_cmd_exec_ram(pUartLogBuf->UARTLogBuf) == FALSE) {
					RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\r\nunknown command '%s'", pUartLogBuf->UARTLogBuf);
					RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\r\n\n#\r\n");
				}
			}

			shell_array_init((u8 *)pUartLogBuf->UARTLogBuf, pUartLogBuf->UARTLogBufLen, '\0');
			pUartLogBuf->BufCount = 0;
			shell_ctl.ExecuteCmd = FALSE;
		}
	} while (1);
}

void shell_init_ram(void)
{
#if defined(CONFIG_SUPPORT_ATCMD)
	atcmd_service_init();
#endif

#ifdef AMEBAD_TODO
	LOGUART_SetBaud_FromFlash();
#endif

#ifdef CONFIG_LONGER_CMD
	shell_buf.UARTLogBuf = shell_buf_array_l;
	shell_buf.UARTLogBufLen = CMD_BUFLEN;
	shell_array_init(&shell_buf.UARTLogBuf[0], shell_buf.UARTLogBufLen, '\0');

	shell_rxbuf.UARTLogBuf = shell_rxbuf_array_l;
	shell_rxbuf.UARTLogBufLen = CMD_BUFLEN;
	shell_array_init(&shell_rxbuf.UARTLogBuf[0], shell_buf.UARTLogBufLen, '\0');
#endif

	shell_ctl.pCmdTbl = (PCOMMAND_TABLE)__cmd_table_start__;
	shell_ctl.CmdTblSz = ((__cmd_table_end__ - __cmd_table_start__) / sizeof(COMMAND_TABLE));

	shell_ctl.ExecuteCmd = FALSE;
	shell_ctl.ExecuteEsc = TRUE; //don't check Esc anymore
	shell_ctl.GiveSema = shell_give_sema;


	/* Create a Semaphone */
	rtos_sema_create_binary(&shell_sema);

#if defined (CONFIG_ARM_CORE_CM4_KM4TZ) || defined (CONFIG_WHC_HOST)
	if (RTK_SUCCESS != rtos_task_create(NULL, "shell_task", shell_task_ram, NULL, 1024 * 4,
										2)) {
		DiagPrintf("Create Log UART Task Err!!\n");
	}
#else
	if (RTK_SUCCESS != rtos_task_create(NULL, "shell_task", shell_task_ram, NULL, 1024 * 1,
										2)) {
		DiagPrintf("Create Log UART Task Err!!\n");
	}
#endif
	//CONSOLE_AMEBA();
}

#ifndef CONFIG_WIFI_HOST_CONTROL
IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_shell_table = {
	.USER_MSG_TYPE = IPC_USER_DATA,
	.Rxfunc = shell_loguratRx_ipc_int,
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_NP_TO_AP,
	.IPC_Channel = IPC_N2A_LOGUART_RX_SWITCH
};
#endif
