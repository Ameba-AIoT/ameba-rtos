/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "rtw_wifi_constants.h"
#include <stdarg.h>
#include "FreeRTOS.h"
//#include "strproc.h"

#define LIB_INFO_CMD	"ATS?"
#define ALL_CPU_RECV	0xFFFF

#if defined ( __ICCARM__ )
#pragma section=".cmd.table.data"

SECTION(".data") u8 *__cmd_table_start__ = 0;
SECTION(".data") u8 *__cmd_table_end__ = 0;
#endif

#define OpenShellRx		2

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

static u32 shell_cmd_exec_ram(u8 *pbuf)
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

#if defined (ARM_CORE_CM4)
#ifndef CONFIG_LINUX_FW_EN
	PIPC_MSG_STRUCT	ipc_msg_temp = (PIPC_MSG_STRUCT)ipc_get_message(IPC_LP_TO_NP, IPC_L2N_LOGUART_RX_SWITCH);
#else
	PIPC_MSG_STRUCT	ipc_msg_temp = (PIPC_MSG_STRUCT)ipc_get_message(IPC_AP_TO_NP, IPC_A2N_LOGUART_RX_SWITCH);
#endif // CONFIG_LINUX_FW_EN
#else
	PIPC_MSG_STRUCT ipc_msg_temp = (PIPC_MSG_STRUCT)ipc_get_message(IPC_LP_TO_AP, IPC_L2A_LOGUART_RX_SWITCH);
#endif

	PUART_LOG_BUF pUartLogBuf = shell_ctl.pTmpLogBuf;

	u32 addr = ipc_msg_temp->msg;
	DCache_Invalidate(addr, sizeof(UART_LOG_BUF));
	_memcpy(pUartLogBuf, (u32 *)addr, sizeof(UART_LOG_BUF));

	shell_ctl.ExecuteCmd = _TRUE;
	if (shell_ctl.shell_task_rdy) {
		shell_ctl.GiveSema();
	}
}

#ifdef ARM_CORE_CM0
UART_LOG_BUF				tmp_log_buf;

void shell_loguratRx_Ipc_Tx(u32 ipc_dir, u32 ipc_ch)
{
	IPC_MSG_STRUCT ipc_msg_temp;

	memcpy(&tmp_log_buf, shell_ctl.pTmpLogBuf, sizeof(UART_LOG_BUF));
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
	PUART_LOG_BUF pUartLogBuf = shell_ctl.pTmpLogBuf;

	for (i = 0; i < UART_LOG_CMD_BUFLEN; i++) {
		if (pUartLogBuf->UARTLogBuf[i] != ' ') {
			if (pUartLogBuf->UARTLogBuf[i] == '\0') {
				CpuId = LP_CPU_ID;
				CONSOLE_AMEBA(); /* '\0' put # */
				shell_array_init((u8 *)pUartLogBuf, sizeof(UART_LOG_BUF), '\0');
				shell_ctl.ExecuteCmd = _FALSE;
				break;
			}
#ifdef CONFIG_AS_INIC_KM4_NP_CA32_AP
			if ((shell_ctl.pTmpLogBuf->UARTLogBuf[i] == '~')) {
				i = i + 1; /* remove flag */
				CpuId = NP_CPU_ID;	/* CMD should processed by KM4 */
			} else if (shell_ctl.pTmpLogBuf->UARTLogBuf[i] == '@') {
				i = i + 1; /* remove flag */
				CpuId = LP_CPU_ID;	/* CMD should processed by KM0 itself */
			} else {
				CpuId = AP_CPU_ID;
			}
#else
			if ((shell_ctl.pTmpLogBuf->UARTLogBuf[i] == '~')) {
				i = i + 1; /* remove flag */
				CpuId = AP_CPU_ID;	/* CMD should processed by CA32 */
			} else if (shell_ctl.pTmpLogBuf->UARTLogBuf[i] == '@') {
				i = i + 1; /* remove flag */
				CpuId = LP_CPU_ID;	/* CMD should processed by KM0 itself */
			} else {
				CpuId = NP_CPU_ID;
			}
#endif
			if (_stricmp((const char *)&pUartLogBuf->UARTLogBuf[i], LIB_INFO_CMD) == 0) {
				CpuId = ALL_CPU_RECV;
			}

			/* avoid useless space */
			memcpy(&pUartLogBuf->UARTLogBuf[0], &pUartLogBuf->UARTLogBuf[i], UART_LOG_CMD_BUFLEN - i);
			break;
		}
	}

	if (CpuId == NP_CPU_ID) {
		shell_loguratRx_Ipc_Tx(IPC_LP_TO_NP, IPC_L2N_LOGUART_RX_SWITCH);
	}

	if (CpuId == AP_CPU_ID) {
		shell_loguratRx_Ipc_Tx(IPC_LP_TO_AP, IPC_L2A_LOGUART_RX_SWITCH);
	}

	if (CpuId == ALL_CPU_RECV) {
		//1. logurat recv CPU Printf Info
		u32 buflen = 1024;
		char *buf = rtos_mem_malloc(buflen);
		ChipInfo_GetSocName_ToBuf(buf, buflen - 1);
		RTK_LOGS(NOTAG, "%s", buf);
		ChipInfo_GetLibVersion_ToBuf(buf, buflen - 1);
		RTK_LOGS(NOTAG, "%s", buf);
		rtos_mem_free(buf);

		//2. Other CPU Pintf Lib Info
		LOGUART_WaitTxComplete();
		shell_loguratRx_Ipc_Tx(IPC_LP_TO_NP, IPC_L2N_LOGUART_RX_SWITCH);
		LOGUART_WaitTxComplete();
		shell_loguratRx_Ipc_Tx(IPC_LP_TO_AP, IPC_L2A_LOGUART_RX_SWITCH);
	}

	if (CpuId != LP_CPU_ID) {
		shell_array_init((u8 *)pUartLogBuf, sizeof(UART_LOG_BUF), '\0');
		shell_ctl.ExecuteCmd = _FALSE;
	}
}
#else
void shell_loguartRx_dispatch(void)
{
	PUART_LOG_BUF pUartLogBuf = shell_ctl.pTmpLogBuf;
	if (_stricmp((const char *)&pUartLogBuf->UARTLogBuf[0], LIB_INFO_CMD) == 0) {
		u32 buflen = 1024;
		char *buf = rtos_mem_malloc(buflen);
		ChipInfo_GetLibVersion_ToBuf(buf, buflen - 1);
		RTK_LOGS(NOTAG, "%s", buf);
		rtos_mem_free(buf);
		shell_array_init((u8 *)pUartLogBuf, sizeof(UART_LOG_BUF), '\0');
		shell_ctl.ExecuteCmd = _FALSE;
	}
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
	shell_ctl.BootRdy = 1;

	rtos_create_secure_context(RTOS_MINIMAL_SECURE_STACK_SIZE);

	do {
		rtos_sema_take(shell_sema, RTOS_MAX_DELAY);

		shell_loguartRx_dispatch();

		if (shell_ctl.ExecuteCmd) {
#if (defined CONFIG_SUPPORT_ATCMD) && ((defined CONFIG_SINGLE_CORE_WIFI) || (defined CONFIG_AS_INIC_AP))
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
					RTK_LOGS(NOTAG, "\r\nunknown command '%s'", pUartLogBuf->UARTLogBuf);
					RTK_LOGS(NOTAG, "\r\n\n#\r\n");
				}
			}

			shell_array_init((u8 *)pUartLogBuf, sizeof(UART_LOG_BUF), '\0');
			shell_ctl.ExecuteCmd = _FALSE;
		}

	} while (1);
}

void shell_init_ram(void)
{
#if (defined CONFIG_SUPPORT_ATCMD) && ((defined CONFIG_SINGLE_CORE_WIFI) || (defined CONFIG_AS_INIC_AP))
	atcmd_service_init();
#endif

#if defined ( __ICCARM__ )
	__cmd_table_start__ = (u8 *)__section_begin(".cmd.table.data");
	__cmd_table_end__ = (u8 *)__section_end(".cmd.table.data");
#endif

	shell_ctl.pCmdTbl = (PCOMMAND_TABLE)__cmd_table_start__;
	shell_ctl.CmdTblSz = ((__cmd_table_end__ - __cmd_table_start__) / sizeof(COMMAND_TABLE));

	shell_ctl.ExecuteCmd = _FALSE;
	shell_ctl.ExecuteEsc = _TRUE; //don't check Esc anymore
	shell_ctl.GiveSema = shell_give_sema;


	/* Create a Semaphone */
	rtos_sema_create_binary(&shell_sema);

	if (SUCCESS != rtos_task_create(NULL, "LOGUART_TASK", shell_task_ram, NULL, SHELL_TASK_FUNC_STACK_SIZE, 5)) {
		DiagPrintf("Create Log UART Task Err!!\n");
	}

	//CONSOLE_AMEBA();
}

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_shell_table[] = {
#ifndef CONFIG_LINUX_FW_EN

	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = shell_loguratRx_ipc_int,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_LP_TO_NP,
		.IPC_Channel = IPC_L2N_LOGUART_RX_SWITCH},
#else
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = shell_loguratRx_ipc_int,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_AP_TO_NP,
		.IPC_Channel = IPC_A2N_LOGUART_RX_SWITCH

	},
#endif
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = shell_loguratRx_ipc_int,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_LP_TO_AP,
		.IPC_Channel = IPC_L2A_LOGUART_RX_SWITCH
	}

};

#ifdef ARM_CORE_CM0

/* for uart bridge to close shell loguart rx */
void shell_uartbridge_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);

	PIPC_MSG_STRUCT	ipc_msg = (PIPC_MSG_STRUCT)ipc_get_message(IPC_NP_TO_LP, IPC_N2L_UARTBRIDGE);
	if (ipc_msg->msg == OpenShellRx) {
		InterruptRegister((IRQ_FUN) shell_uart_irq_rom, UART_LOG_IRQ, (u32)NULL, 3);
		InterruptEn(UART_LOG_IRQ, 3);
	} else {
		InterruptDis(UART_LOG_IRQ);
	}
}

void shell_uartbridge_ipc_int_A2L(void *Data, u32 IrqStatus, u32 ChanNum)
{
	/* To avoid gcc warnings */
	UNUSED(Data);
	UNUSED(IrqStatus);
	UNUSED(ChanNum);

	PIPC_MSG_STRUCT	ipc_msg = (PIPC_MSG_STRUCT)ipc_get_message(IPC_AP_TO_LP, IPC_A2L_UARTBRIDGE);
	if (ipc_msg->msg == OpenShellRx) {
		InterruptRegister((IRQ_FUN) shell_uart_irq_rom, UART_LOG_IRQ, (u32)NULL, 3);
		InterruptEn(UART_LOG_IRQ, 3);
	} else {
		InterruptDis(UART_LOG_IRQ);
	}
}
IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_uartbridge_table[] = {
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = shell_uartbridge_ipc_int,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_NP_TO_LP,
		.IPC_Channel = IPC_N2L_UARTBRIDGE
	},
	{
		.USER_MSG_TYPE = IPC_USER_DATA,
		.Rxfunc = shell_uartbridge_ipc_int_A2L,
		.RxIrqData = (void *) NULL,
		.Txfunc = IPC_TXHandler,
		.TxIrqData = (void *) NULL,
		.IPC_Direction = IPC_AP_TO_LP,
		.IPC_Channel = IPC_A2L_UARTBRIDGE,

	}
};
#endif
