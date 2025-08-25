/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "FreeRTOS.h"


#define LIB_INFO_CMD	"ATS?"
#define ALL_CPU_RECV	0xFFFF

#define OpenShellRx		2

static const char *const TAG = "SHELL";
extern volatile UART_LOG_CTL		shell_ctl;

rtos_sema_t	shell_sema = NULL;

#ifdef CONFIG_SUPPORT_ATCMD
char atcmd_buf[UART_LOG_CMD_BUFLEN];
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

#if defined (CONFIG_RSICV_CORE_KR4)
	PIPC_MSG_STRUCT	ipc_msg_temp = (PIPC_MSG_STRUCT)ipc_get_message(IPC_KM4_TO_KR4, IPC_M2R_LOGUART_RX_SWITCH);
#else
	PIPC_MSG_STRUCT ipc_msg_temp = (PIPC_MSG_STRUCT)ipc_get_message(IPC_KM4_TO_DSP, IPC_M2D_LOGUART_RX_SWITCH);
#endif

	PUART_LOG_BUF pUartLogBuf = shell_ctl.pTmpLogBuf;

	u32 addr = ipc_msg_temp->msg;
	DCache_Invalidate(addr, sizeof(UART_LOG_BUF));
	_memcpy(pUartLogBuf, (u32 *)addr, sizeof(UART_LOG_BUF));

	shell_ctl.ExecuteCmd = TRUE;
	if (shell_ctl.shell_task_rdy) {
		shell_ctl.GiveSema();
	}
}

#ifdef CONFIG_ARM_CORE_CM4
UART_LOG_BUF				tmp_log_buf;

void shell_loguratRx_Ipc_Tx(u32 ipc_dir, u32 ipc_ch)
{
	IPC_MSG_STRUCT ipc_msg_temp;

	_memcpy(&tmp_log_buf, shell_ctl.pTmpLogBuf, sizeof(UART_LOG_BUF));
	DCache_CleanInvalidate((u32)&tmp_log_buf, sizeof(UART_LOG_BUF));

	ipc_msg_temp.msg_type = IPC_USER_POINT;
	ipc_msg_temp.msg = (u32)&tmp_log_buf;
	ipc_msg_temp.msg_len = 1;
	ipc_msg_temp.rsvd = 0; /* for coverity init issue */
	ipc_send_message(ipc_dir, ipc_ch, &ipc_msg_temp);
}

void shell_loguartRx_dispatch(void)
{
	u32 i, CpuId = 0;
	u32 cmd_done = FALSE;
	PUART_LOG_BUF pUartLogBuf = shell_ctl.pTmpLogBuf;

	for (i = 0; i < UART_LOG_CMD_BUFLEN; i++) {
		if (shell_ctl.pTmpLogBuf->UARTLogBuf[i] != ' ') {
			if (pUartLogBuf->UARTLogBuf[i] == '\0') {
				CpuId = KM4_CPU_ID;
				CONSOLE_AMEBA(); /* '\0' put # */
				cmd_done = TRUE;
				break;
			}
#ifdef CONFIG_AP_CORE_KR4
			if ((shell_ctl.pTmpLogBuf->UARTLogBuf[i] == '~')) {
				i = i + 1; /* remove flag */
				CpuId = DSP_CPU_ID; /* CMD should processed by DSP */
			} else if (shell_ctl.pTmpLogBuf->UARTLogBuf[i] == '@') {
				i = i + 1; /* remove flag */
				CpuId = KM4_CPU_ID; /* CMD should processed by KM4 itself */
			} else {
				CpuId = KR4_CPU_ID;
			}
#else
			if ((shell_ctl.pTmpLogBuf->UARTLogBuf[i] == '~')) {
				i = i + 1; /* remove flag */
				CpuId = DSP_CPU_ID; /* CMD should processed by DSP */
			} else if (shell_ctl.pTmpLogBuf->UARTLogBuf[i] == '@') {
				i = i + 1; /* remove flag */
				CpuId = KR4_CPU_ID; /* CMD should processed by KR4 itself */
			} else {
				CpuId = KM4_CPU_ID;
			}
#endif
			/* avoid useless space */
			_memcpy(&pUartLogBuf->UARTLogBuf[0], &pUartLogBuf->UARTLogBuf[i], UART_LOG_CMD_BUFLEN - i);
			break;
		}
	}

	if (CpuId == DSP_CPU_ID) {
		shell_loguratRx_Ipc_Tx(IPC_KM4_TO_DSP, IPC_M2D_LOGUART_RX_SWITCH);
		cmd_done = TRUE;
	}

	if (CpuId == KR4_CPU_ID) {
		shell_loguratRx_Ipc_Tx(IPC_KM4_TO_KR4, IPC_M2R_LOGUART_RX_SWITCH);
		cmd_done = TRUE;
	}

	if (cmd_done) {
		shell_array_init((u8 *)pUartLogBuf, sizeof(UART_LOG_BUF), '\0');
		shell_ctl.ExecuteCmd = FALSE;
	} else {
		shell_ctl.ExecuteCmd = TRUE;
	}
}
#else
#ifdef CONFIG_SOC_SOLO
void shell_loguartRx_dispatch(void)
{
	u32 i;
	PUART_LOG_BUF pUartLogBuf = shell_ctl.pTmpLogBuf;

	for (i = 0; i < UART_LOG_CMD_BUFLEN; i++) {
		if (shell_ctl.pTmpLogBuf->UARTLogBuf[i] != ' ') {
			if (pUartLogBuf->UARTLogBuf[i] == '\0') {
				CONSOLE_AMEBA(); /* '\0' put # */
				shell_array_init((u8 *)pUartLogBuf, sizeof(UART_LOG_BUF), '\0');
				shell_ctl.ExecuteCmd = FALSE;
				break;
			}

			/* avoid useless space */
			_memcpy(&pUartLogBuf->UARTLogBuf[0], &pUartLogBuf->UARTLogBuf[i], UART_LOG_CMD_BUFLEN - i);
			break;
		}
	}
}
#else
void shell_loguartRx_dispatch(void)
{
}
#endif
#endif


static void shell_task_ram(void *Data)
{
	/* To avoid gcc warnings */
	(void) Data;
	u32 ret = FALSE;
	PUART_LOG_BUF pUartLogBuf = shell_ctl.pTmpLogBuf;

	//4 Set this for UartLog check cmd history
	shell_ctl.shell_task_rdy = 1;
	shell_ctl.BootRdy = 1;

	rtos_create_secure_context(RTOS_MINIMAL_SECURE_STACK_SIZE);

	do {
		rtos_sema_take(shell_sema, RTOS_MAX_DELAY);

		shell_loguartRx_dispatch();

		if (shell_ctl.ExecuteCmd) {
#if defined(CONFIG_SUPPORT_ATCMD)
			shell_array_init((u8 *)atcmd_buf, sizeof(atcmd_buf), '\0');
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
					RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\r\nunknown command '%s'", pUartLogBuf->UARTLogBuf);
					RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\r\n\n#\r\n");
				}
			}

			shell_array_init((u8 *)pUartLogBuf, sizeof(UART_LOG_BUF), '\0');
			shell_ctl.ExecuteCmd = FALSE;
		}

	} while (1);
}

void shell_init_ram(void)
{
#if defined(CONFIG_SUPPORT_ATCMD)
	atcmd_service_init();
#endif

	shell_ctl.pCmdTbl = (PCOMMAND_TABLE)(void *)__cmd_table_start__;
	shell_ctl.CmdTblSz = ((__cmd_table_end__ - __cmd_table_start__) / sizeof(COMMAND_TABLE));

	shell_ctl.ExecuteCmd = FALSE;
	shell_ctl.ExecuteEsc = TRUE; //don't check Esc anymore
	shell_ctl.GiveSema = shell_give_sema;


	/* Create a Semaphone */
	rtos_sema_create_binary(&shell_sema);

	if (RTK_SUCCESS != rtos_task_create(NULL, "shell_task", shell_task_ram, NULL, SHELL_TASK_FUNC_STACK_SIZE, 2)) {
		RTK_LOGE(TAG, "Create Log UART Task Err!!\n");
	}

	//CONSOLE_AMEBA();
}

IPC_TABLE_DATA_SECTION

const IPC_INIT_TABLE ipc_shell_table = {
	.USER_MSG_TYPE = IPC_USER_DATA,
	.Rxfunc = shell_loguratRx_ipc_int,
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_KM4_TO_KR4,
	.IPC_Channel = IPC_M2R_LOGUART_RX_SWITCH
};

#ifdef CONFIG_ARM_CORE_CM4
/* for uart bridge to close shell loguart rx */
void shell_uartbridge_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);

	PIPC_MSG_STRUCT	ipc_msg = (PIPC_MSG_STRUCT)ipc_get_message(IPC_KR4_TO_KM4, IPC_R2M_UARTBRIDGE);
	if (ipc_msg->msg == OpenShellRx) {
		InterruptRegister((IRQ_FUN) shell_uart_irq_rom, UART_LOG_IRQ, (u32)NULL, INT_PRI4);
		InterruptEn(UART_LOG_IRQ, INT_PRI4);
	} else {
		InterruptDis(UART_LOG_IRQ);
	}
}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_uartbridge_table = {
	.USER_MSG_TYPE = IPC_USER_DATA,
	.Rxfunc = shell_uartbridge_ipc_int,
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_KR4_TO_KM4,
	.IPC_Channel = IPC_R2M_UARTBRIDGE
};
#endif
