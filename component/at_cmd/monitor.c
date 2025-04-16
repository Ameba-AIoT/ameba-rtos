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

#if defined  (__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

static const char *const TAG = "MONITOR";

u32
CmdRamHelp(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	/* To avoid gcc warnings */
	(void) argc;
	(void) argv;

	COMMAND_TABLE *cmd_table = (COMMAND_TABLE *)__cmd_table_start__;
	u32 cmd_mum = ((__cmd_table_end__ - __cmd_table_start__) / sizeof(COMMAND_TABLE));
	u32	index ;

	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "----------------- TEST COMMAND MODE HELP %x------------------\n", cmd_mum);
	for (index = 0  ; index < cmd_mum; index++) {
		if (cmd_table[index].msg) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s\n", cmd_table[index].msg);
		}
	}
	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "----------------- TEST COMMAND MODE END  %x------------------\n", cmd_mum);

	return TRUE ;
}

u32
cmd_reboot(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "Rebooting ...\n\r");

	if (argc >= 1 && _strcmp((const char *)argv[0], "uartburn") == 0) {
		BKUP_Clear(BKUP_REG0, BKUP_MASK_UARTBURN_BOOT);
		BKUP_Set(BKUP_REG0, BKUP_BIT_UARTBURN_BOOT);
	}

	System_Reset();

	return TRUE;
}

u32
CmdTickPS(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	/* To avoid gcc warnings */
	UNUSED(argc);

	if (_strcmp((const char *)argv[0], "r") == 0) { // release
		pmu_release_wakelock(PMU_OS);
	}

	if (_strcmp((const char *)argv[0], "a") == 0) { // acquire
		pmu_acquire_wakelock(PMU_OS);
	}

	if (_strcmp((const char *)argv[0], "type") == 0) { // PG or CG
		if (_strcmp((const char *)argv[1], "pg") == 0) {
			pmu_set_sleep_type(SLEEP_PG);
		} else if (_strcmp((const char *)argv[1], "cg") == 0) {
			pmu_set_sleep_type(SLEEP_CG);
		} else {
			pmu_set_sleep_type(SLEEP_PG);
		}
	}

	if (_strcmp((const char *)argv[0], "get") == 0) { // get sleep & wake time
		RTK_LOGS(TAG, RTK_LOG_ALWAYS, "lockbit:%lx \n", pmu_get_wakelock_status());
		RTK_LOGS(TAG, RTK_LOG_ALWAYS, "dslp_lockbit:%lx\n", pmu_get_deepwakelock_status());
	}

	return TRUE;
}

u32 cmd_dump_word(u16 argc, u8  *argv[])
{
	u32 Src = 0x0;
	u32 Len = 1; //default value
	u32 Is_Byte = FALSE;
	u32 OTF_Enable = SYSCFG_OTP_RSIPEn();

	/* get parameters */
	if (argc < 1 || argc > 3) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Wrong argument number!\r\n");
		return FALSE;
	}
	if (argc >= 3) {
		Is_Byte = *((const char *)argv[2]) == 'B' || *((const char *)argv[2]) == 'b';
	}
	if (argc >= 2) {
		Len = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
	}
	if (argc >= 1) {
		Src = _strtoul((const char *)(argv[0]), (char **)NULL, 16);
	}

	/* align */
	Src &= ~(0x03);

	/* read encrypt image for FW protection */
	if (IS_FLASH_ADDR(Src)) {
		if (OTF_Enable != 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "RSIP enabled, Can't Read Flash.\n");
			return TRUE;
		}
	}

	if (Is_Byte && Len >= 1) {
		DCache_CleanInvalidate(Src, sizeof(u8) * Len);
		rtk_log_memory_dump_byte((u8 *)Src, Len);
	} else if (Len >= 1) {
		DCache_CleanInvalidate(Src, sizeof(u32) * Len);
		rtk_log_memory_dump_word((u32 *)Src, Len);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Wrong cmd!\r\n");
		return FALSE;
	}

	return TRUE;
}

u32 cmd_write_word(u16 argc, u8  *argv[])
{
	UNUSED(argc);

	u32 Src;
	u32 Value;

	Src = _strtoul((const char *)(argv[0]), (char **)NULL, 16);

	Src &= ~(0x03);

	Value = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "[%08x] %08x \n", Src, Value);

	*(volatile u32 *)(Src) = Value;

	DCache_Clean(Src, sizeof(u32));

	return 0;
}

u32 cmd_efuse_protect(u16 argc, u8  *argv[])
{
	u32 index;
	u32 Len;
	u32 Cnt;
	u32 Addr = 0;
	u8 *EfuseBuf = NULL;
	char *DString;

	if ((EfuseBuf = rtos_mem_zmalloc(MAX(OTP_REAL_CONTENT_LEN, OTP_LMAP_LEN))) == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "efuse mem malloc fail \n");
		return 0;
	}

	/* efuse rmap */
	/* efuse rmap 0x18 4 */
	if (_strcmp((const char *)argv[0], "rmap") == 0) {
		Addr = 0;
		Len = OTP_LMAP_LEN;

		if (argc >= 3) {
			Addr = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
			Len = _strtoul((const char *)(argv[2]), (char **)NULL, 16);
		}

		if (OTP_LogicalMap_Read(EfuseBuf, Addr, Len) == RTK_FAIL) {
			RTK_LOGE(TAG, "EFUSE_LogicalMap_Read fail \n");
			goto exit;
		}

		for (u32 i = 0, index = Addr; index < Addr + Len; index++, i++) {
			if (i % 16 == 0) {
				RTK_LOGI(NOTAG, "\n\rEFUSE[%03lx]:", index);
			}
			RTK_LOGI(NOTAG, " %02x", EfuseBuf[i]);
		}
		RTK_LOGI(NOTAG, "\n\r");
	}

	/* efuse wmap 0x0 2 2187 */
	/* efuse wmap 0x18 4 01020304 */
	if (_strcmp((const char *)argv[0], "wmap") == 0) {
		if (argc < 4) {
			RTK_LOGE(TAG, "Invalid argc. \n");
			goto exit;
		}

		Addr = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
		Len = _strtoul((const char *)(argv[2]), (char **)NULL, 16);
		DString = (char *)argv[3];
		Cnt = _strlen(DString);

		if (Cnt % 2) {
			RTK_LOGW(TAG, "string length(%lu) should be odd \n", Cnt);
			goto exit;
		} else {
			Cnt = Cnt / 2;
		}

		if (Cnt != Len) {
			RTK_LOGW(TAG, "Oops: write lenth not match input string lentg, choose smaller one\n");
			Len = (Cnt < Len) ? Cnt : Len;
		}
		RTK_LOGI(TAG, "efuse wmap write len:%lu, string len:%lu\n", Len, Cnt << 1);

		for (index = 0; index < Len; index++) {
			EfuseBuf[index] = _2char2hex(DString[index * 2], DString[index * 2 + 1]);
		}

		if (OTP_LogicalMap_Write(Addr, Len, (u8 *)EfuseBuf) == RTK_FAIL) {
			RTK_LOGE(TAG, "EFUSE_LogicalMap_Read fail \n");
			goto exit;
		}
	}

	/* efuse rraw */
	/* efuse rraw,0x18,4 */
	if (_strcmp((const char *)argv[0], "rraw") == 0) {
		Addr = 0;
		Len = EFUSE_REAL_CONTENT_LEN;
		if (argc >= 3) {
			Addr = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
			Len = _strtoul((const char *)(argv[2]), (char **)NULL, 16);
		}

		for (index = Addr; index < Addr + Len; index++) {
			if (OTP_Read8(index, EfuseBuf + index) == RTK_FAIL) {
				RTK_LOGE(TAG, "OTP_Read8 fail \n");
				goto exit;
			}
		}

		for (u32 i = 0, index = Addr; index < Addr + Len; index ++, i++) {
			if (i % 16 == 0) {
				RTK_LOGI(NOTAG, "\n\rRawMap[%03lx]:", index);
			}
			RTK_LOGI(NOTAG, " %02x", EfuseBuf[index]);
		}
		RTK_LOGI(NOTAG, "\n\r");
	}

	/* efuse wraw 0xA0 1 aa */
	/* efuse wraw 0xA0 2 aabb */
	/* efuse wraw 0xA0 4 aabbccdd */
	if (_strcmp((const char *)argv[0], "wraw") == 0) {
		if (argc < 4) {
			RTK_LOGE(TAG, "Invalid argc. \n");
			goto exit;
		}

		Addr = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
		Len = _strtoul((const char *)(argv[2]), (char **)NULL, 16);
		DString = (char *)argv[3];
		Cnt = _strlen(DString);

		if (Cnt % 2) {
			RTK_LOGW(TAG, "string length(%lu) should be odd \n", Cnt);
			goto exit;
		} else {
			Cnt = Cnt / 2;
		}

		if (Cnt != Len) {
			RTK_LOGW(TAG, "Oops: write lenth not match input string lentg, choose smaller one\n");
			Len = (Cnt < Len) ? Cnt : Len;
		}
		RTK_LOGI(TAG, "efuse wraw write len:%lu, string len:%lu\n", Len, Cnt << 1);

		for (index = 0; index < Len; index++) {
			EfuseBuf[index] = _2char2hex(DString[index * 2], DString[index * 2 + 1]);
		}

		for (index = 0; index < Len; index++) {
			RTK_LOGI(NOTAG, "wraw: %lx %x \n", Addr + index, EfuseBuf[index]);
			if (OTP_Write8((Addr + index), EfuseBuf[index]) == RTK_FAIL) {
				RTK_LOGE(TAG, "OTP_Write8 fail \n");
				goto exit;
			}
		}
	}

	if (_strcmp((const char *)argv[0], "GETCRC") == 0) {
		RTK_LOGI(TAG, "new crc value is 0x%lx",  OTPGetCRC());
	}

exit:
	rtos_mem_free(EfuseBuf);
	return 0;
}

CMD_TABLE_DATA_SECTION
static COMMAND_TABLE   shell_cmd_table[] = {
	{
		(const u8 *)"?",		0, CmdRamHelp,	(const u8 *)"\tHELP (~?) \n"
		"\t\t Print this help messag\n"
	},
	{
		(const u8 *)"DW",		4, cmd_dump_word,	(const u8 *)"\tDW <Address, Length>\n"
		"\t\t Dump memory word or Read Hw word register; \n"
		"\t\t DW <addr> 		 Dump only one word at the same time, unit: 4Bytes\n"
		"\t\t DW <addr> <len>	 Dump the specified length of the word, unit: 4Bytes\n"
		"\t\t DW <addr> <len> b  Dump the specified length of the byte, unit: 1Byte\n"
	},
	{
		(const u8 *)"EW",		2, cmd_write_word,	(const u8 *)"\tEW <Address, Hex>\n"
		"\t\t Write memory dword or Write Hw dword register \n"
		"\t\t Can write only one dword at the same time \n"
		"\t\t Ex: EW Address Value \n"
	},

	{
		(const u8 *)"REBOOT",	4, cmd_reboot,	(const u8 *)"\tREBOOT \n"
		"\t\t <item, string> : \n"
		"\t\t item: uartburn or N/A \n"
		"\t\t \n"
	},
#ifndef CONFIG_MP_SHRINK
#if ((defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBASMARTPLUS)) && (defined(CONFIG_ARM_CORE_CA32) || defined(CONFIG_ARM_CORE_CM4))) || \
	(defined(CONFIG_AMEBAD) && defined(CONFIG_ARM_CORE_CM4)) || \
	(defined(CONFIG_AMEBALITE) && defined(CONFIG_ARM_CORE_CM4)) || \
	(defined(CONFIG_AMEBADPLUS) && defined(CONFIG_ARM_CORE_CM4)) || \
	(defined(CONFIG_AMEBAGREEN2) && defined(CONFIG_ARM_CORE_CM4_KM4TZ)) || \
	(defined(CONFIG_AMEBAL2) && defined(CONFIG_ARM_CORE_CM4_KM4TZ))
	{
		(const u8 *)"EFUSE",	8, cmd_efuse_protect,	(const u8 *)"\tEFUSE \n"
		"\t\t wmap addr(hex) len(hex) data(hex)\n"
		"\t\t rmap \n"
		"\t\t <wmap 0x00 0x2 8195> efuse[0]=0x81, efuse [1]=0x95\n"
		"\t\t <wmap 0xF0 0x4 11223344> [0xF0]=0x11, [0xF1]=0x22, [0xF2]=0x33, [0xF3]=0x44\n"
	},
#endif
	{
		(const u8 *)"TICKPS",	4, CmdTickPS,	(const u8 *)"\tTICKPS \n"
		"\t\t r: release os wakelock \n"
		"\t\t a: acquire os wakelock \n"
	},
#endif
};

u32 cmd_rom_table(void **PTable)
{
	*PTable = (void *)&shell_cmd_table;

	return (sizeof(shell_cmd_table) / sizeof(COMMAND_TABLE));
}
