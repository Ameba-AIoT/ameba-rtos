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

static const char *TAG = "MONITOR";
SLEEP_ParamDef dsleep_param;

u32
CmdRamHelp(
	IN  u16 argc,
	IN  u8  *argv[]
);

u32
cmd_reboot(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	RTK_LOGS(TAG, "Rebooting ...\n\r");

	(void) argc;	/* To avoid gcc warnings */
	(void) argv;	/* To avoid gcc warnings */

	if (argc >= 1 && _strcmp((const char *)argv[0], "uartburn") == 0) {
		BKUP_Set(0, BKUP_BIT_UARTBURN_BOOT);
	}

	System_Reset();

	return _TRUE;
}

u32
CmdTickPS(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	if (_strcmp((const char *)argv[0], "r") == 0) { // release
		if (argc >= 3 && _strcmp((const char *)argv[1], "timer") == 0) {
			u32 duration_ms = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
			pmu_set_max_sleep_time(duration_ms);
		}
		pmu_release_wakelock(PMU_OS);
	}

	if (_strcmp((const char *)argv[0], "a") == 0) { // acquire
		pmu_acquire_wakelock(PMU_OS);
	}

	if (_strcmp((const char *)argv[0], "type") == 0) { // PG or CG
		if (argc >= 2 && _strcmp((const char *)argv[1], "pg") == 0) {
			pmu_set_sleep_type(SLEEP_PG);
		} else if (argc >= 2  && _strcmp((const char *)argv[1], "cg") == 0) {
			pmu_set_sleep_type(SLEEP_CG);
		} else {
			pmu_set_sleep_type(SLEEP_PG);
		}
	}

	if (_strcmp((const char *)argv[0], "dslp") == 0) {
		u32 duration_ms = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
		u32 KR4_is_NP = LSYS_GET_KR4_IS_NP(HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG1));

		if (KR4_is_NP) {
			RTK_LOGS(TAG, "dslp: KM4 AP\n");
			pmu_release_deepwakelock(PMU_OS);
			pmu_release_wakelock(PMU_OS);
		} else {
			RTK_LOGS(TAG, "dslp: KM4 NP\n");
			RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, ENABLE);
			SOCPS_AONTimerClearINT();
			SOCPS_AONTimer(duration_ms);
			SOCPS_AONTimerINT_EN(ENABLE);

			SOCPS_DeepSleep_RAM();
		}
	}

	if (_strcmp((const char *)argv[0], "get") == 0) { // get sleep & wake time
		RTK_LOGS(TAG, "lockbit:%x \n", pmu_get_wakelock_status());
		RTK_LOGS(TAG, "dslp_lockbit:%x\n", pmu_get_deepwakelock_status());
	}

	return _TRUE;
}

u32 cmd_efuse_protect(u16 argc, u8  *argv[])
{
	/* To avoid gcc warnings */
	(void) argc;

	u8 *EfuseBuf = NULL;
	u32 index;
	u8 ret = 0;

	if ((EfuseBuf = rtos_mem_zmalloc(OTP_REAL_CONTENT_LEN)) == NULL) {
		RTK_LOGS(TAG, "efuse mem malloc fail \n");
	}

	/* efuse wmap 0x0 2 2187 */
	/* efuse wmap 0x18 4 01020304 */
	if (_strcmp((const char *)argv[0], "wmap") == 0) {
		u32 Addr = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
		u32 Len = _strtoul((const char *)(argv[2]), (char **)NULL, 16);
		char *DString = (char *)argv[3];
		u32 Cnt;

		Cnt = _strlen(DString);
		if (Cnt % 2) {
			RTK_LOGS(TAG, "string length(%d) should be odd \n", Cnt);
			ret = FALSE;
			goto exit;
		}

		Cnt = Cnt / 2;
		if (Cnt != Len) {
			RTK_LOGS(TAG, "Oops: write lenth not match input string lentg, choose smaller one\n");
			Len = (Cnt < Len) ? Cnt : Len;
		}

		RTK_LOGI(TAG, "efuse wmap write len:%lu, string len:%lu\n", Len, Cnt << 1);

		for (index = 0; index < Len; index++) {
			EfuseBuf[index] = _2char2hex(DString[index * 2], DString[index * 2 + 1]);
		}

		OTP_LogicalMap_Write(Addr, Len, (u8 *)(EfuseBuf));
	}

	if (_strcmp((const char *)argv[0], "rmap") == 0) {
		RTK_LOGS(TAG, "efuse rmap \n");

		ret = OTP_LogicalMap_Read(EfuseBuf, 0, OTP_LMAP_LEN);
		if (ret == _FAIL) {
			RTK_LOGS(TAG, "OTP_LogicalMap_Read fail \n");
		}

		for (index = 0; index < 1024; index += 16) {
			RTK_LOGS(NOTAG, "EFUSE[%03x]: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", index,
					 EfuseBuf[index], EfuseBuf[index + 1], EfuseBuf[index + 2], EfuseBuf[index + 3],
					 EfuseBuf[index + 4], EfuseBuf[index + 5], EfuseBuf[index + 6], EfuseBuf[index + 7],
					 EfuseBuf[index + 8], EfuseBuf[index + 9], EfuseBuf[index + 10], EfuseBuf[index + 11],
					 EfuseBuf[index + 12], EfuseBuf[index + 13], EfuseBuf[index + 14], EfuseBuf[index + 15]);
		}
	}

	if (_strcmp((const char *)argv[0], "rraw") == 0) {
		RTK_LOGS(TAG, "efuse rraw\n");

		for (index = 0; index < OTP_USER_END; index++) {
			OTP_Read8(index, (EfuseBuf + index));
		}

		for (index = 0; index < OTP_USER_END; index += 16) {
			RTK_LOGS(NOTAG, "RawMap[%03x]: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", index,
					 EfuseBuf[index], EfuseBuf[index + 1], EfuseBuf[index + 2], EfuseBuf[index + 3],
					 EfuseBuf[index + 4], EfuseBuf[index + 5], EfuseBuf[index + 6], EfuseBuf[index + 7],
					 EfuseBuf[index + 8], EfuseBuf[index + 9], EfuseBuf[index + 10], EfuseBuf[index + 11],
					 EfuseBuf[index + 12], EfuseBuf[index + 13], EfuseBuf[index + 14], EfuseBuf[index + 15]);
		}
	}

	/* efuse wraw 0xA0 1 aa */
	/* efuse wraw 0xA0 2 aabb */
	/* efuse wraw 0xA0 4 aabbccdd */
	if (_strcmp((const char *)argv[0], "wraw") == 0) {
		u32 Addr = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
		u32 Len = _strtoul((const char *)(argv[2]), (char **)NULL, 16);
		char *DString = (char *)argv[3];
		u32 Cnt;

		Cnt = _strlen(DString);
		if (Cnt % 2) {
			RTK_LOGS(TAG, "string length(%d) should be odd \n", Cnt);
			ret = FALSE;
			goto exit;
		}

		Cnt = Cnt / 2;
		if (Cnt != Len) {
			RTK_LOGS(TAG, "Oops: write lenth not match input string lentg, choose smaller one\n");
			Len = (Cnt < Len) ? Cnt : Len;
		}

		for (index = 0; index < Len; index++) {
			EfuseBuf[index] = _2char2hex(DString[index * 2], DString[index * 2 + 1]);
		}

		RTK_LOGS(TAG, "efuse wraw write len:%d, string len:%u\n", Len, Cnt << 1);

		for (index = 0; index < Len; index++) {
			RTK_LOGS(TAG, "wraw: %x %x \n", Addr + index, EfuseBuf[index]);
			OTP_Write8(Addr + index, EfuseBuf[index]);
		}
	}

exit:

	if (EfuseBuf) {
		rtos_mem_free(EfuseBuf);
	}
	return ret;

}

u32 cmd_dump_word(u16 argc, u8  *argv[])
{
	u32 Src = 0x0;
	u32 Len = 1; //default value
	u32 Is_Byte = _FALSE;
	u32 OTF_Enable = SYSCFG_OTP_RSIPEn();

	/* get parameters */
	if (argc < 1 || argc > 3) {
		RTK_LOGS(TAG, "Wrong argument number!\r\n");
		return _FALSE;
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
			RTK_LOGS(TAG, "RSIP enabled, Can't Read Flash.\n");
			return _TRUE;
		}
	}


	if (Is_Byte && Len >= 1) {
		DCache_CleanInvalidate(Src, sizeof(u8) * Len);
		rtk_log_memory_dump_byte((u8 *)Src, Len);
	} else if (Len >= 1) {
		DCache_CleanInvalidate(Src, sizeof(u32) * Len);
		rtk_log_memory_dump_word((u32 *)Src, Len);
	} else {
		RTK_LOGS(TAG, "Wrong cmd!\r\n");
		return _FALSE;
	}

	return _TRUE;

}

u32 cmd_write_word(u16 argc, u8  *argv[])
{
	UNUSED(argc);

	u32 Src;
	u32 Value;

	Src = _strtoul((const char *)(argv[0]), (char **)NULL, 16);

	Src &= ~(0x03);

	Value = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
	RTK_LOGS(NOTAG, "[%08x] %08x \n", Src, Value);

	*(volatile u32 *)(Src) = Value;

	DCache_Clean(Src, sizeof(u32));

	return 0;
}

u32 cmd_log_set(u16 argc, u8  *argv[])
{
	if (argc != 2) {
		RTK_LOGS(TAG, "Wrong argument number!\r\n");
		return _FALSE;
	}

	rtk_log_level_t Value = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
	rtk_log_level_set((const char *)argv[0], Value);

	return _TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   shell_cmd_table[] = {
	{
		(const u8 *)"?",		0, CmdRamHelp,	(const u8 *)"\tHELP (?) : \n"
		"\t\t Print help messag\n"
	},
	{
		(const u8 *)"DW",		4, cmd_dump_word,	(const u8 *)"\tDW <Address, Length>\n"
		"\t\t Dump memory word or Read Hw word register; \n"
		"\t\t DW <addr> 		 Dump only one word at the same time, unit: 4Bytes\n"
		"\t\t DW <addr> <len> 	 Dump the specified length of the word, unit: 4Bytes\n"
		"\t\t DW <addr> <len> b  Dump the specified length of the byte, unit: 1Byte\n"
	},
	{
		(const u8 *)"EW",		2, cmd_write_word,	(const u8 *)"\tEW <Address, Hex>\n"
		"\t\t Write memory dword or Write Hw dword register \n"
		"\t\t Can write only one dword at the same time \n"
		"\t\t Ex: EW Address Value \n"
	},
	{
		(const u8 *)"EFUSE",	8, cmd_efuse_protect,	(const u8 *)"\tEFUSE \n"
		"\t\t wmap addr len data\n"
		"\t\t rmap \n"
		"\t\t <wmap 0x00 2 8195> efuse[0]=0x81, efuse [1]=0x98\n"
		"\t\t <wmap 0xF0 4 11223344> [0xF0]=0x11, [0xF1]=0x22, [0xF2]=0x33, [0xF3]=0x44\n"
	},
	{
		(const u8 *)"REBOOT",	4, cmd_reboot,	(const u8 *)"\tREBOOT \n"
		"\t\t reboot \n"
		"\t\t reboot uartburn \n"
	},
	{
		(const u8 *)"TICKPS",	4, CmdTickPS,	(const u8 *)"\tTICKPS \n"
		"\t\t r: release os wakelock \n"
		"\t\t a: acquire os wakelock \n"
	},
	{
		(const u8 *)"LOG",	3, cmd_log_set, (const u8 *)"\tLOG <tag, level>\n"
		"\t\t Set the log display level of a module individually\n"
		"\t\t <tag>: module label, If the tag is *, this will reset all tag levels except those added to the array\n"
		"\t\t <level>:0, turn off log\n"
		"\t\t 	   1, always (Resident)log\n"
		"\t\t 	   2, error log\n"
		"\t\t 	   3, warning log\n"
		"\t\t 	   4, info log\n"
		"\t\t 	   5, debug log\n"
	},
};

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

	RTK_LOGS(TAG, "----------------- TEST COMMAND MODE HELP %x------------------\n", cmd_mum);
	for (index = 0  ; index < cmd_mum; index++) {
		if (cmd_table[index].msg) {
			RTK_LOGS(NOTAG, "%s\n", cmd_table[index].msg);
		}
	}
	RTK_LOGS(TAG, "----------------- TEST COMMAND MODE END  %x------------------\n", cmd_mum);

	return _TRUE ;
}
