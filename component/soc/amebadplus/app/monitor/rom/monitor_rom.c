/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"

#if defined  (__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

static u32 cmd_rom_help(u16 argc, u8  *argv[]);

SHELL_ROM_TEXT_SECTION
static u32
cmd_reboot(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	if (_strcmp((const char *)argv[0], "uartburn") == 0) {
		BKUP_Set(BKUP_REG0, BKUP_BIT_UARTBURN_BOOT);
	} else if (_strcmp((const char *)argv[0], "uartburn_dbg") == 0) {
		BKUP_Set(BKUP_REG0, BKUP_BIT_UARTBURN_BOOT | BKUP_BIT_UARTBURN_DEBUG);
	}

	MONITOR_LOG("\n\rRebooting ...\n\r");
	NVIC_SystemReset();

	return TRUE;
}

SHELL_ROM_TEXT_SECTION
static u32
cmd_Iboot_Set(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	u32 temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_TESTING_CFG);

	if (_strcmp((const char *)argv[0], "mcm") == 0) {
		temp |= AON_BIT_IBOOT_SW;
		temp |= AON_BIT_IBOOT_SEL;
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_TESTING_CFG, temp);
	} else if (_strcmp((const char *)argv[0], "ext") == 0) {
		temp &= ~AON_BIT_IBOOT_SW;
		temp |= AON_BIT_IBOOT_SEL;
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_TESTING_CFG, temp);
	}

	return TRUE;
}


SHELL_ROM_DATA_SECTION
static COMMAND_TABLE   shell_cmd_table_rom[] = {
	{
		(const u8 *)"?",		0, cmd_rom_help,	(const u8 *)"\tHELP (?) \n"
		"\t\t Print this help messag\n"
	},
	{
		(const u8 *)"REBOOT",	4, cmd_reboot,	(const u8 *)"\tREBOOT \n"
		"\t\t reboot \n"
		"\t\t reboot uartburn \n"
	},
	{
		(const u8 *)"IBOOT",	4, cmd_Iboot_Set,	(const u8 *)"\tIBOOT \n"
		"\t\t iboot mcm \n"
		"\t\t iboot ext \n"
	},
};

SHELL_ROM_TEXT_SECTION
u32 cmd_rom_table(void **PTable)
{
	*PTable = (void *)&shell_cmd_table_rom;

	return (sizeof(shell_cmd_table_rom) / sizeof(COMMAND_TABLE));
}

SHELL_ROM_TEXT_SECTION
static u32 cmd_rom_help(u16 argc, u8  *argv[])
{
	u32	LoopINdex ;

	MONITOR_LOG("----------------- COMMAND MODE HELP ------------------\n");
	for (LoopINdex = 0  ; LoopINdex < (sizeof(shell_cmd_table_rom) / sizeof(COMMAND_TABLE)) ; LoopINdex++) {
		if (shell_cmd_table_rom[LoopINdex].msg) {
			MONITOR_LOG("%s\n", shell_cmd_table_rom[LoopINdex].msg);
		}
	}
	MONITOR_LOG("----------------- COMMAND MODE END  ------------------\n");

	return TRUE ;
}
#if defined  (__GNUC__)
#pragma GCC diagnostic pop
#endif
