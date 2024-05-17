/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _RTK_CONSOL_H_
#define _RTK_CONSOL_H_

#ifdef CONFIG_MP_INCLUDED
#define SHELL_TASK_FUNC_STACK_SIZE (1024 * 4)
#elif (defined (CONFIG_AS_INIC_AP) || defined(CONFIG_SINGLE_CORE_WIFI))
#define SHELL_TASK_FUNC_STACK_SIZE (1252 + 128 + CONTEXT_SAVE_SIZE + 1024)
#else
#define SHELL_TASK_FUNC_STACK_SIZE (480 + 128 + CONTEXT_SAVE_SIZE)	/* KM0 cost stack: max_size < 300 bytes, test by monitor cmd */
#endif

#ifdef CONFIG_LONGER_CMD
#define UART_LOG_CMD_BUFLEN     512
#define MAX_ARGV                32
#else
//UART_LOG_CMD_BUFLEN: only 126 bytes could be used for keeping input
//                                     cmd, the last byte is for string end ('\0').
#define UART_LOG_CMD_BUFLEN     127
#define MAX_ARGV                16
#endif

typedef u32(*ECHOFUNC)(IN u8 *, ...);   //UART LOG echo-function type.
typedef u32(*monitor_cmd_handler)(u16 argc, u8 *argv[]);

typedef struct {
#ifdef CONFIG_LONGER_CMD
	u16 BufCount;                          //record the input cmd char number.
#else
	u8  BufCount;                          //record the input cmd char number.
#endif
	u8  UARTLogBuf[UART_LOG_CMD_BUFLEN];   //record the input command.
} UART_LOG_BUF, *PUART_LOG_BUF;

typedef struct _COMMAND_TABLE_ {
	const   u8 *cmd;
	u16     ArgvCnt;
	u32(*func)(u16 argc, u8 *argv[]);
	const   u8 *msg;
} COMMAND_TABLE, *PCOMMAND_TABLE;


typedef struct {
	u8  ExecuteEsc;
	u8  ExecuteCmd;
	u8  shell_task_rdy;
	u8  Resvd;
	PUART_LOG_BUF pTmpLogBuf;
	void *pfINPUT;
	PCOMMAND_TABLE pCmdTbl;
	u32 CmdTblSz;

	void (*GiveSema)(void);
} UART_LOG_CTL, *PUART_LOG_CTL;


#define KB_ASCII_NUL    0x00         //NUL(null)
#define KB_ASCII_BS     0x08         //BS (backspace), 退格
#define KB_ASCII_TAB    0x09         //HT (horizontal tab), 水平制表符
#define KB_ASCII_LF     0x0A         //LF (NL line feed, new line), 换行键
#define KB_ASCII_CR     0x0D         //CR (carriage return), 回车键
#define KB_ASCII_ESC    0x1B         //ESC (escape)
#define KB_ASCII_SP     0x20         // (space)
#define KB_ASCII_BS_7F  0x7F         //DEL (delete)
#define KB_ASCII_LBRKT  0x5B         //[

#define KB_SPACENO_TAB  1

#define SHELL_CMDCHK_CONT  1
#define SHELL_CMDCHK_DONE  2

#define _ConsolePrint  DiagPrintf_minimal

#define AMEBA_CONSOLE_PREFIX	"#"
#define CONSOLE_AMEBA(...)     do {\
	_ConsolePrint("\r" AMEBA_CONSOLE_PREFIX __VA_ARGS__);\
}while(0)

_LONG_CALL_ void shell_init_rom(u32 TBLSz, void *pTBL);
_LONG_CALL_ void shell_task_rom(void *Data);
_LONG_CALL_ void shell_rom(u32 MaxWaitCount);
_LONG_CALL_ u32 shell_uart_irq_rom(void *Data);
_LONG_CALL_ u8 shell_cmd_chk(IN  u8  RevData, IN  UART_LOG_CTL    *prvUartLogCtl, IN  u8  EchoFlag);
#ifdef CONFIG_LONGER_CMD
_LONG_CALL_ void shell_array_init(IN  u8  *pArrayToInit, IN  u16  ArrayLen, IN  u8  InitValue);
#else
_LONG_CALL_ void shell_array_init(IN  u8  *pArrayToInit, IN  u8  ArrayLen, IN  u8  InitValue);
#endif
_LONG_CALL_ u8 **shell_get_argv(const   u8  *string);
_LONG_CALL_ u8 shell_get_argc(const   u8  *string);

void shell_init_ram(void);
void shell_loguratRx_ipc_int(void *Data, u32 IrqStatus, u32 ChanNum);

//#define RtlConsolTaskRom		shell_task_rom

//extern u32 shell_interrupt_on;
#endif //_RTK_CONSOL_H_
