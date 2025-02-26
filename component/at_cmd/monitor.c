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

u32 cmd_log_set(u16 argc, u8  *argv[])
{
	if (argc != 2) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Wrong argument number!\r\n");
		return FALSE;
	}

	rtk_log_level_t Value = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
	rtk_log_level_set((const char *)argv[0], Value);
	rtk_log_array_print(rtk_log_tag_array);
	return TRUE;
}

#ifdef CONFIG_AMEBASMART
#ifdef CONFIG_ARM_CORE_CA32
u32 cmd_efuse_protect(u16 argc, u8  *argv[])
{
	/* To avoid gcc warnings */
	(void) argc;

	u8 *EfuseBuf = NULL;
	u32 index;
	int ret = 0;

	if ((EfuseBuf = rtos_mem_zmalloc(OTP_REAL_CONTENT_LEN)) == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "efuse mem malloc fail \n");
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
			RTK_LOGW(TAG, "string length(%lu) should be odd \n", Cnt);
			ret = FALSE;
			goto exit;
		}

		Cnt = Cnt / 2;
		if (Cnt != Len) {
			RTK_LOGW(TAG, "Oops: write lenth not match input string lentg, choose smaller one\n");
			Len = (Cnt < Len) ? Cnt : Len;
		}

		RTK_LOGI(TAG, "efuse wmap write len:%lu, string len:%lu\n", Len, Cnt << 1);

		for (index = 0; index < Len; index++) {
			EfuseBuf[index] = _2char2hex(DString[index * 2], DString[index * 2 + 1]);
		}

		OTP_LogicalMap_Write(Addr, Len, (u8 *)(EfuseBuf));
	}

	if (_strcmp((const char *)argv[0], "rmap") == 0) {
		RTK_LOGI(TAG, "efuse rmap \n");

		u32 Addr = 0;
		u32 Len = OTP_LMAP_LEN;

		if (argc >= 3) {
			Addr = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
			Len = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
		}

		ret = OTP_LogicalMap_Read(EfuseBuf, Addr, Len);
		if (ret == FAIL) {
			RTK_LOGE(TAG, "EFUSE_LogicalMap_Read fail \n");
		}

		for (u32 i = 0, index = Addr; index < Addr + Len; index++, i++) {
			if (i % 16 == 0) {
				RTK_LOGA(NOTAG, "\n\rEFUSE[%03lx]:", index);
			}
			RTK_LOGA(NOTAG, " %02x", EfuseBuf[i]);
		}
		RTK_LOGA(NOTAG, "\n\r");

	}

	if (_strcmp((const char *)argv[0], "rraw") == 0) {
		RTK_LOGI(TAG, "efuse rraw\n");

		u32 Addr = 0;
		u32 Len = OTP_USER_END;

		if (argc >= 3) {
			Addr = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
			Len = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
		}

		for (index = Addr; index < Addr + Len; index++) {
			OTP_Read8(index, EfuseBuf + index);
		}

		for (u32 i = 0, index = Addr; index < Addr + Len; index ++, i++) {
			if (i % 16 == 0) {
				RTK_LOGA(NOTAG, "\n\rRawMap[%03lx]:", index);
			}
			RTK_LOGA(NOTAG, " %02x", EfuseBuf[index]);
		}

		RTK_LOGA(NOTAG, "\n\r");

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
			RTK_LOGW(TAG, "string length(%lu) should be odd \n", Cnt);
			ret = FALSE;
			goto exit;
		}

		Cnt = Cnt / 2;
		if (Cnt != Len) {
			RTK_LOGW(TAG, "Oops: write lenth not match input string lentg, choose smaller one\n");
			Len = (Cnt < Len) ? Cnt : Len;
		}

		for (index = 0; index < Len; index++) {
			EfuseBuf[index] = _2char2hex(DString[index * 2], DString[index * 2 + 1]);
		}

		RTK_LOGI(TAG, "efuse wraw write len:%lu, string len:%lu\n", Len, Cnt << 1);

		for (index = 0; index < Len; index++) {
			RTK_LOGA(NOTAG, "wraw: %lx %x \n", Addr + index, EfuseBuf[index]);
			OTP_Write8((Addr + index), EfuseBuf[index]);
		}
	}

	if (_strcmp((const char *)argv[0], "getcrc") == 0) {
		u32 crc = OTPGetCRC();
		RTK_LOGI(TAG, "new crc value is 0x%lx", crc);
	}

exit:

	if (EfuseBuf) {
		rtos_mem_free(EfuseBuf);
	}
	return ret;
}

#endif
#ifdef CONFIG_ARM_CORE_CM4
u32 cmd_efuse_protect(u16 argc, u8  *argv[])
{
	/* To avoid gcc warnings */
	(void) argc;

	u8 *EfuseBuf = NULL;
	u32 index;
	int ret = 0;

	if ((EfuseBuf = rtos_mem_zmalloc(OTP_REAL_CONTENT_LEN)) == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "efuse mem malloc fail \n");
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
			RTK_LOGW(TAG, "string length(%lu) should be odd \n", Cnt);
			ret = FALSE;
			goto exit;
		}

		Cnt = Cnt / 2;
		if (Cnt != Len) {
			RTK_LOGW(TAG, "Oops: write lenth not match input string lentg, choose smaller one\n");
			Len = (Cnt < Len) ? Cnt : Len;
		}

		RTK_LOGI(TAG, "efuse wmap write len:%lu, string len:%lu\n", Len, Cnt << 1);

		for (index = 0; index < Len; index++) {
			EfuseBuf[index] = _2char2hex(DString[index * 2], DString[index * 2 + 1]);
		}

		OTP_LogicalMap_Write(Addr, Len, (u8 *)(EfuseBuf));
	}

	if (_strcmp((const char *)argv[0], "rmap") == 0) {
		RTK_LOGI(TAG, "efuse rmap \n");

		u32 Addr = 0;
		u32 Len = OTP_LMAP_LEN;

		if (argc >= 3) {
			Addr = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
			Len = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
		}

		ret = OTP_LogicalMap_Read(EfuseBuf, Addr, Len);
		if (ret == FAIL) {
			RTK_LOGE(TAG, "EFUSE_LogicalMap_Read fail \n");
		}

		for (u32 i = 0, index = Addr; index < Addr + Len; index++, i++) {
			if (i % 16 == 0) {
				RTK_LOGA(NOTAG, "\n\rEFUSE[%03lx]:", index);
			}
			RTK_LOGA(NOTAG, " %02x", EfuseBuf[i]);
		}
		RTK_LOGA(NOTAG, "\n\r");

	}

	if (_strcmp((const char *)argv[0], "rraw") == 0) {
		RTK_LOGI(TAG, "efuse rraw\n");

		u32 Addr = 0;
		u32 Len = OTP_USER_END;

		if (argc >= 3) {
			Addr = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
			Len = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
		}

		for (index = Addr; index < Addr + Len; index++) {
			OTP_Read8(index, EfuseBuf + index);
		}

		for (u32 i = 0, index = Addr; index < Addr + Len; index ++, i++) {
			if (i % 16 == 0) {
				RTK_LOGA(NOTAG, "\n\rRawMap[%03lx]:", index);
			}
			RTK_LOGA(NOTAG, " %02x", EfuseBuf[index]);
		}

		RTK_LOGA(NOTAG, "\n\r");

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
			RTK_LOGW(TAG, "string length(%lu) should be odd \n", Cnt);
			ret = FALSE;
			goto exit;
		}

		Cnt = Cnt / 2;
		if (Cnt != Len) {
			RTK_LOGW(TAG, "Oops: write lenth not match input string lentg, choose smaller one\n");
			Len = (Cnt < Len) ? Cnt : Len;
		}

		for (index = 0; index < Len; index++) {
			EfuseBuf[index] = _2char2hex(DString[index * 2], DString[index * 2 + 1]);
		}

		RTK_LOGI(TAG, "efuse wraw write len:%lu, string len:%lu\n", Len, Cnt << 1);

		for (index = 0; index < Len; index++) {
			RTK_LOGA(NOTAG, "wraw: %lx %x \n", Addr + index, EfuseBuf[index]);
			OTP_Write8((Addr + index), EfuseBuf[index]);
		}
	}

	if (_strcmp((const char *)argv[0], "getcrc") == 0) {
		u32 crc = OTPGetCRC();
		RTK_LOGI(TAG, "new crc value is 0x%lx", crc);
	}

exit:

	if (EfuseBuf) {
		rtos_mem_free(EfuseBuf);
	}
	return ret;
}

u32
CmdRTC(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	/* To avoid gcc warnings */
	(void) argc;

	RTC_TimeTypeDef RTC_TimeStruct;

	if (_strcmp((const char *)argv[0], "get") == 0) { // dump RTC
		RTC_AlarmTypeDef RTC_AlarmStruct_temp;


		RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
		RTC_GetAlarm(RTC_Format_BIN, &RTC_AlarmStruct_temp);

		RTK_LOGS(TAG, RTK_LOG_ALWAYS, "time: %d:%d:%d:%d (%d) \n", RTC_TimeStruct.RTC_Days,
				 RTC_TimeStruct.RTC_Hours,
				 RTC_TimeStruct.RTC_Minutes,
				 RTC_TimeStruct.RTC_Seconds,
				 RTC_TimeStruct.RTC_H12_PMAM);

		RTK_LOGS(TAG, RTK_LOG_ALWAYS, "alarm: %d:%d:%d:%d (%d) \n", RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Days,
				 RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Hours,
				 RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Minutes,
				 RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_Seconds,
				 RTC_AlarmStruct_temp.RTC_AlarmTime.RTC_H12_PMAM);
	}

	if (_strcmp((const char *)argv[0], "set") == 0) {
		RTC_TimeStructInit(&RTC_TimeStruct);
		RTC_TimeStruct.RTC_Hours = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
		RTC_TimeStruct.RTC_Minutes = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
		RTC_TimeStruct.RTC_Seconds = _strtoul((const char *)(argv[3]), (char **)NULL, 10);

		if (_strcmp((const char *)argv[5], "pm") == 0) {
			RTC_TimeStruct.RTC_H12_PMAM = RTC_H12_PM;
		} else {
			RTC_TimeStruct.RTC_H12_PMAM = RTC_H12_AM;
		}

		RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
	}

	return TRUE;
}

u32
CmdLogBuf(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	/* To avoid gcc warnings */
	(void) argc;

	if (_strcmp((const char *)argv[0], "on") == 0) {
		ConfigDebugBuffer = 1;
	} else {
		ConfigDebugBuffer = 0;
	}

	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "AAAAA\n");
	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "BBBBB\n");

	return TRUE;
}

u32
CmdTsfTest(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	/* To avoid gcc warnings */
	(void) argc;

	u32 temp;
	if (_strcmp((const char *)argv[0], "read") == 0) { // release
		if (_strcmp((const char *)argv[1], "560") == 0) {
			RTK_LOGS(TAG, RTK_LOG_ALWAYS, "0x40080560=%x\n", HAL_READ32(WIFI_REG_BASE, 0x560));
		} else if (_strcmp((const char *)argv[1], "568") == 0) {
			RTK_LOGS(TAG, RTK_LOG_ALWAYS, "0x40080568=%x\n", HAL_READ32(WIFI_REG_BASE, 0x568));
		} else if (_strcmp((const char *)argv[1], "all") == 0) {
			RTK_LOGS(TAG, RTK_LOG_ALWAYS, "0x40080560=%x 0x40080568=%x\n", HAL_READ32(WIFI_REG_BASE, 0x560), HAL_READ32(WIFI_REG_BASE, 0x568));
		}
	}
	if (_strcmp((const char *)argv[0], "set") == 0) { // release
		temp = HAL_READ32(WIFI_REG_BASE, 0x560);
		HAL_WRITE32(WIFI_REG_BASE, 0x568, temp);
		RTK_LOGS(TAG, RTK_LOG_ALWAYS, "0x40080560=%x 0x40080568=%x\n", HAL_READ32(WIFI_REG_BASE, 0x560), HAL_READ32(WIFI_REG_BASE, 0x568));
	}

	return 0;
}
#endif

#ifdef CONFIG_ARM_CORE_CM0
u32
CmdAGGCmd(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	UNUSED(argc);
	if (_strcmp((const char *)argv[0], "en") == 0) {
		/*enable agg function*/
		LOGUART_WaitTxComplete(); // wait until tx complete
		LOGUART_AGGPathAllCmd(LOGUART_DEV, DISABLE); // close all path
		LOGUART_AGGCmd(LOGUART_DEV, ENABLE); // enable agg function
		LOGUART_AGGPathAllCmd(LOGUART_DEV, ENABLE); // open all path
	} else if (_strcmp((const char *)argv[0], "dis") == 0) {
		/*disable agg function*/
		LOGUART_WaitTxComplete(); // wait until tx complete
		LOGUART_AGGPathAllCmd(LOGUART_DEV, DISABLE); // close all path
		LOGUART_AGGCmd(LOGUART_DEV, DISABLE); // disable agg function
		LOGUART_AGGPathAllCmd(LOGUART_DEV, ENABLE); // open all path
	}

	return 0;
}
#endif
#endif

#ifdef CONFIG_AMEBADPLUS
#ifdef CONFIG_ARM_CORE_CM0
u32
CmdAGGCmd(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	UNUSED(argc);
	if (_strcmp((const char *)argv[0], "en") == 0) {
		/*enable agg function*/
		LOGUART_WaitTxComplete(); // wait until tx complete
		LOGUART_AGGPathAllCmd(LOGUART_DEV, DISABLE); // close all path
		LOGUART_AGGCmd(LOGUART_DEV, ENABLE); // enable agg function
		LOGUART_AGGPathAllCmd(LOGUART_DEV, ENABLE); // open all path
	} else if (_strcmp((const char *)argv[0], "dis") == 0) {
		/*disable agg function*/
		LOGUART_WaitTxComplete(); // wait until tx complete
		LOGUART_AGGPathAllCmd(LOGUART_DEV, DISABLE); // close all path
		LOGUART_AGGCmd(LOGUART_DEV, DISABLE); // disable agg function
		LOGUART_AGGPathAllCmd(LOGUART_DEV, ENABLE); // open all path
	}

	return 0;
}

#ifdef CONFIG_WLAN
extern int rtw_wltunnel_command(char *cmd);
u32
CmdWTN(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	if (argc != 1) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Wrong argument number!\r\n");
		return FALSE;
	}
	rtw_wltunnel_command((char *)argv[0]);
	return 0;
}
#endif
#endif
#ifdef CONFIG_ARM_CORE_CM4
#ifdef CONFIG_WLAN
extern int whc_ipc_host_api_wltunnel_command(char *cmd, unsigned int cmd_len);
u32
CmdWTN(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
#if !defined(CONFIG_WHC_INTF_IPC)
	UNUSED(argc);
	UNUSED(argv);
#else
	if (argc != 1) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Wrong argument number!\r\n");
		return FALSE;
	}
	whc_ipc_host_api_wltunnel_command((char *)argv[0], strlen((char *)argv[0]) + 1);
#endif
	return 0;
}
#endif
u32 cmd_efuse_protect(u16 argc, u8  *argv[])
{
	/* To avoid gcc warnings */
	(void) argc;

	u8 *EfuseBuf = NULL;
	u32 index;
	int ret = 0;

	if ((EfuseBuf = rtos_mem_zmalloc(OTP_REAL_CONTENT_LEN)) == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "efuse mem malloc fail \n");
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
			RTK_LOGS(TAG, RTK_LOG_ERROR, "string length(%u) should be odd \n", Cnt);
			ret = FALSE;
			goto exit;
		}

		Cnt = Cnt / 2;
		if (Cnt != Len) {
			RTK_LOGS(TAG, RTK_LOG_ALWAYS, "Oops: write lenth not match input string lentg, choose smaller one\n");
			Len = (Cnt < Len) ? Cnt : Len;
		}

		RTK_LOGS(TAG, RTK_LOG_ALWAYS, "efuse wmap write len:%u, string len:%u\n", Len, Cnt << 1);

		for (index = 0; index < Len; index++) {
			EfuseBuf[index] = _2char2hex(DString[index * 2], DString[index * 2 + 1]);
		}

		OTP_LogicalMap_Write(Addr, Len, (u8 *)(EfuseBuf));
	}

	if (_strcmp((const char *)argv[0], "rmap") == 0) {
		RTK_LOGS(TAG, RTK_LOG_ALWAYS, "efuse rmap \n");

		u32 Addr = 0;
		u32 Len = OTP_LMAP_LEN;

		if (argc >= 3) {
			Addr = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
			Len = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
		}

		ret = OTP_LogicalMap_Read(EfuseBuf, Addr, Len);

		if (ret == FAIL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "EFUSE_LogicalMap_Read fail \n");
		}

		for (u32 i = 0, index = Addr; index < Addr + Len; index++, i++) {
			if (i % 16 == 0) {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\rEFUSE[%03x]:", index);
			}
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, " %02x", EfuseBuf[i]);
		}
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r");

	}

	if (_strcmp((const char *)argv[0], "rraw") == 0) {
		RTK_LOGS(TAG, RTK_LOG_ALWAYS, "efuse rraw\n");

		u32 Addr = 0;
		u32 Len = OTP_USER_END;

		if (argc >= 3) {
			Addr = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
			Len = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
		}

		for (index = Addr; index < Addr + Len; index++) {
			OTP_Read8(index, EfuseBuf + index);
		}

		for (u32 i = 0, index = Addr; index < Addr + Len; index ++, i++) {
			if (i % 16 == 0) {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\rRawMap[%03x]:", index);
			}
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, " %02x", EfuseBuf[index]);
		}

		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r");
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
			RTK_LOGS(TAG, RTK_LOG_ERROR, "string length(%u) should be odd \n", Cnt);
			ret = FALSE;
			goto exit;
		}

		Cnt = Cnt / 2;
		if (Cnt != Len) {
			RTK_LOGS(TAG, RTK_LOG_ALWAYS, "Oops: write lenth not match input string lentg, choose smaller one\n");
			Len = (Cnt < Len) ? Cnt : Len;
		}

		for (index = 0; index < Len; index++) {
			EfuseBuf[index] = _2char2hex(DString[index * 2], DString[index * 2 + 1]);
		}

		RTK_LOGS(TAG, RTK_LOG_ALWAYS, "efuse wraw write len:%u, string len:%u\n", Len, Cnt << 1);

		for (index = 0; index < Len; index++) {
			RTK_LOGS(TAG, RTK_LOG_ALWAYS, "wraw: %x %x \n", Addr + index, EfuseBuf[index]);
			OTP_Write8(Addr + index, EfuseBuf[index]);
		}
	}

exit:

	if (EfuseBuf) {
		rtos_mem_free(EfuseBuf);
	}
	return ret;
}

#endif
#endif

#ifdef CONFIG_AMEBALITE
#ifdef CONFIG_ARM_CORE_CM4
u32 cmd_efuse_protect(u16 argc, u8  *argv[])
{
	/* To avoid gcc warnings */
	(void) argc;

	u8 *EfuseBuf = NULL;
	u32 index;
	int ret = 0;

	if ((EfuseBuf = rtos_mem_zmalloc(OTP_REAL_CONTENT_LEN)) == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "efuse mem malloc fail \n");
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
			RTK_LOGS(TAG, RTK_LOG_ERROR, "string length(%d) should be odd \n", Cnt);
			ret = FALSE;
			goto exit;
		}

		Cnt = Cnt / 2;
		if (Cnt != Len) {
			RTK_LOGS(TAG, RTK_LOG_ALWAYS, "Oops: write lenth not match input string lentg, choose smaller one\n");
			Len = (Cnt < Len) ? Cnt : Len;
		}

		RTK_LOGI(TAG, "efuse wmap write len:%lu, string len:%lu\n", Len, Cnt << 1);

		for (index = 0; index < Len; index++) {
			EfuseBuf[index] = _2char2hex(DString[index * 2], DString[index * 2 + 1]);
		}

		OTP_LogicalMap_Write(Addr, Len, (u8 *)(EfuseBuf));
	}

	if (_strcmp((const char *)argv[0], "rmap") == 0) {
		RTK_LOGS(TAG, RTK_LOG_ALWAYS, "efuse rmap \n");

		ret = OTP_LogicalMap_Read(EfuseBuf, 0, OTP_LMAP_LEN);
		if (ret == FAIL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "OTP_LogicalMap_Read fail \n");
		}

		for (index = 0; index < 1024; index += 16) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "EFUSE[%03x]: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", index,
					 EfuseBuf[index], EfuseBuf[index + 1], EfuseBuf[index + 2], EfuseBuf[index + 3],
					 EfuseBuf[index + 4], EfuseBuf[index + 5], EfuseBuf[index + 6], EfuseBuf[index + 7],
					 EfuseBuf[index + 8], EfuseBuf[index + 9], EfuseBuf[index + 10], EfuseBuf[index + 11],
					 EfuseBuf[index + 12], EfuseBuf[index + 13], EfuseBuf[index + 14], EfuseBuf[index + 15]);
		}
	}

	if (_strcmp((const char *)argv[0], "rraw") == 0) {
		RTK_LOGS(TAG, RTK_LOG_ALWAYS, "efuse rraw\n");

		for (index = 0; index < OTP_USER_END; index++) {
			OTP_Read8(index, (EfuseBuf + index));
		}

		for (index = 0; index < OTP_USER_END; index += 16) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "RawMap[%03x]: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", index,
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
			RTK_LOGS(TAG, RTK_LOG_ERROR, "string length(%d) should be odd \n", Cnt);
			ret = FALSE;
			goto exit;
		}

		Cnt = Cnt / 2;
		if (Cnt != Len) {
			RTK_LOGS(TAG, RTK_LOG_ALWAYS, "Oops: write lenth not match input string lentg, choose smaller one\n");
			Len = (Cnt < Len) ? Cnt : Len;
		}

		for (index = 0; index < Len; index++) {
			EfuseBuf[index] = _2char2hex(DString[index * 2], DString[index * 2 + 1]);
		}

		RTK_LOGS(TAG, RTK_LOG_ALWAYS, "efuse wraw write len:%d, string len:%u\n", Len, Cnt << 1);

		for (index = 0; index < Len; index++) {
			RTK_LOGS(TAG, RTK_LOG_ALWAYS, "wraw: %x %x \n", Addr + index, EfuseBuf[index]);
			OTP_Write8(Addr + index, EfuseBuf[index]);
		}
	}

exit:

	if (EfuseBuf) {
		rtos_mem_free(EfuseBuf);
	}
	return ret;

}
#endif
#ifdef CONFIG_RSICV_CORE_KR4
u32
CmdAGGCmd(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	UNUSED(argc);
	if (_strcmp((const char *)argv[0], "en") == 0) {
		/*enable agg function*/
		LOGUART_WaitTxComplete(); // wait until tx complete
		LOGUART_AGGPathAllCmd(LOGUART_DEV, DISABLE); // close all path
		LOGUART_AGGCmd(LOGUART_DEV, ENABLE); // enable agg function
		LOGUART_AGGPathAllCmd(LOGUART_DEV, ENABLE); // open all path
	} else if (_strcmp((const char *)argv[0], "dis") == 0) {
		/*disable agg function*/
		LOGUART_WaitTxComplete(); // wait until tx complete
		LOGUART_AGGPathAllCmd(LOGUART_DEV, DISABLE); // close all path
		LOGUART_AGGCmd(LOGUART_DEV, DISABLE); // disable agg function
		LOGUART_AGGPathAllCmd(LOGUART_DEV, ENABLE); // open all path
	}

	return 0;
}
#endif
#endif

#if (defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_AMEBAL2))
#ifdef CONFIG_ARM_CORE_CM4_KM4NS
u32
CmdAGGCmd(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	UNUSED(argc);
	if (_strcmp((const char *)argv[0], "en") == 0) {
		/*enable agg function*/
		LOGUART_WaitTxComplete(); // wait until tx complete
		LOGUART_AGGPathAllCmd(LOGUART_DEV, DISABLE); // close all path
		LOGUART_AGGCmd(LOGUART_DEV, ENABLE); // enable agg function
		LOGUART_AGGPathAllCmd(LOGUART_DEV, ENABLE); // open all path
	} else if (_strcmp((const char *)argv[0], "dis") == 0) {
		/*disable agg function*/
		LOGUART_WaitTxComplete(); // wait until tx complete
		LOGUART_AGGPathAllCmd(LOGUART_DEV, DISABLE); // close all path
		LOGUART_AGGCmd(LOGUART_DEV, DISABLE); // disable agg function
		LOGUART_AGGPathAllCmd(LOGUART_DEV, ENABLE); // open all path
	}

	return 0;
}

#ifdef CONFIG_WLAN
extern int rtw_wltunnel_command(char *cmd);
u32
CmdWTN(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
	if (argc != 1) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Wrong argument number!\r\n");
		return FALSE;
	}
	rtw_wltunnel_command((char *)argv[0]);
	return 0;
}
#endif

#endif
#ifdef CONFIG_ARM_CORE_CM4_KM4TZ

#ifdef CONFIG_WLAN
extern int whc_ipc_host_api_wltunnel_command(char *cmd, unsigned int cmd_len);
u32
CmdWTN(
	IN  u16 argc,
	IN  u8  *argv[]
)
{
#if defined(CONFIG_WHC_INTF_SDIO) || defined(CONFIG_WHC_INTF_USB) || defined(CONFIG_WHC_INTF_SPI)
	UNUSED(argc);
	UNUSED(argv);
#else
	if (argc != 1) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Wrong argument number!\r\n");
		return FALSE;
	}
	whc_ipc_host_api_wltunnel_command((char *)argv[0], strlen((char *)argv[0]) + 1);
#endif
	return 0;
}
#endif

u32 cmd_efuse_protect(u16 argc, u8  *argv[])
{
	/* To avoid gcc warnings */
	(void) argc;

	u8 *EfuseBuf = NULL;
	u32 index;
	int ret = 0;

	if ((EfuseBuf = rtos_mem_zmalloc(OTP_REAL_CONTENT_LEN)) == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "efuse mem malloc fail \n");
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
			RTK_LOGS(TAG, RTK_LOG_WARN, "string length(%d) should be odd \n", Cnt);
			ret = FALSE;
			goto exit;
		}

		Cnt = Cnt / 2;
		if (Cnt != Len) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Oops: write lenth not match input string lentg, choose smaller one\n");
			Len = (Cnt < Len) ? Cnt : Len;
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "efuse wmap write len:%d, string len:%d\n", Len, Cnt << 1);

		for (index = 0; index < Len; index++) {
			EfuseBuf[index] = _2char2hex(DString[index * 2], DString[index * 2 + 1]);
		}

		OTP_LogicalMap_Write(Addr, Len, (u8 *)(EfuseBuf));
	}

	if (_strcmp((const char *)argv[0], "rmap") == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "efuse rmap \n");

		u32 Addr = 0;
		u32 Len = OTP_LMAP_LEN;

		if (argc >= 3) {
			Addr = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
			Len = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
		}

		ret = OTP_LogicalMap_Read(EfuseBuf, Addr, Len);

		if (ret == FAIL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "EFUSE_LogicalMap_Read fail \n");
		}

		for (u32 i = 0, index = Addr; index < Addr + Len; index++, i++) {
			if (i % 16 == 0) {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\rEFUSE[%03x]:", index);
			}
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, " %02x", EfuseBuf[i]);
		}
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r");

	}

	if (_strcmp((const char *)argv[0], "rraw") == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "efuse rraw\n");

		u32 Addr = 0;
		u32 Len = OTP_USER_END;

		if (argc >= 3) {
			Addr = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
			Len = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
		}

		for (index = Addr; index < Addr + Len; index++) {
			OTP_Read8(index, EfuseBuf + index);
		}

		for (u32 i = 0, index = Addr; index < Addr + Len; index ++, i++) {
			if (i % 16 == 0) {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\rRawMap[%03x]:", index);
			}
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, " %02x", EfuseBuf[index]);
		}

		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r");
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
			RTK_LOGS(TAG, RTK_LOG_WARN, "string length(%d) should be odd \n", Cnt);
			ret = FALSE;
			goto exit;
		}

		Cnt = Cnt / 2;
		if (Cnt != Len) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Oops: write lenth not match input string lentg, choose smaller one\n");
			Len = (Cnt < Len) ? Cnt : Len;
		}

		for (index = 0; index < Len; index++) {
			EfuseBuf[index] = _2char2hex(DString[index * 2], DString[index * 2 + 1]);
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "efuse wraw write len:%d, string len:%d\n", Len, Cnt << 1);

		for (index = 0; index < Len; index++) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "wraw: %x %x \n", Addr + index, EfuseBuf[index]);
			OTP_Write8(Addr + index, EfuseBuf[index]);
		}
	}

exit:

	if (EfuseBuf) {
		rtos_mem_free(EfuseBuf);
	}
	return ret;

}
#endif
#endif

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
#if (defined(CONFIG_AMEBASMART) && (defined(CONFIG_ARM_CORE_CA32) || defined(CONFIG_ARM_CORE_CM4))) || \
	(defined(CONFIG_AMEBALITE) && defined(CONFIG_ARM_CORE_CM4)) || \
	(defined(CONFIG_AMEBADPLUS) && defined(CONFIG_ARM_CORE_CM4)) || \
	(defined(CONFIG_AMEBAGREEN2) && defined(CONFIG_ARM_CORE_CM4_KM4TZ)) || \
	(defined(CONFIG_AMEBAL2) && defined(CONFIG_ARM_CORE_CM4_KM4TZ))
	{
		(const u8 *)"EFUSE",	8, cmd_efuse_protect,	(const u8 *)"\tEFUSE \n"
		"\t\t wmap addr len data\n"
		"\t\t rmap \n"
		"\t\t <wmap 0x00 2 8195> efuse[0]=0x81, efuse [1]=0x95\n"
		"\t\t <wmap 0xF0 4 11223344> [0xF0]=0x11, [0xF1]=0x22, [0xF2]=0x33, [0xF3]=0x44\n"
	},
#endif
#if defined(CONFIG_AMEBASMART) && defined(CONFIG_ARM_CORE_CM4)
	{
		(const u8 *)"RTC",		4, CmdRTC,	(const u8 *)"\tRTC \n"
		"\t\t get\n"
	},
	{
		(const u8 *)"TSFTEST",	4, CmdTsfTest,	(const u8 *)"\tTSFTEST \n"
		"\t\t get\n"
	},
	{
		(const u8 *)"LOGBUF",	4, CmdLogBuf,	(const u8 *)"\tLOGBUF \n"
		"\t\t KM0 help to print KM4 log\n"
	},
#endif
#if (defined(CONFIG_AMEBASMART) && defined(CONFIG_ARM_CORE_CM0)) || \
	(defined(CONFIG_AMEBALITE) && defined(CONFIG_RSICV_CORE_KR4)) || \
	(defined(CONFIG_AMEBADPLUS) && defined(CONFIG_ARM_CORE_CM0)) || \
	(defined(CONFIG_AMEBAGREEN2) && defined(CONFIG_ARM_CORE_CM4_KM4NS)) || \
	(defined(CONFIG_AMEBAL2) && defined(CONFIG_ARM_CORE_CM4_KM4NS))
	{
		(const u8 *)"AGG",	0, CmdAGGCmd,	(const u8 *)"\t@AGGCmd \n"
		"\t\t enable and disable loguart agg function\n"
	},
#endif
#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBAGREEN2)
#ifdef CONFIG_WLAN
	{
		(const u8 *)"WTN",	5, CmdWTN,	(const u8 *)"\t@WTN \n"
		"\t\t get_father \n"
		"\t\t get_child \n"
		"\t\t ... \n"
	},
#endif
#endif
	{
		(const u8 *)"TICKPS",	4, CmdTickPS,	(const u8 *)"\tTICKPS \n"
		"\t\t r: release os wakelock \n"
		"\t\t a: acquire os wakelock \n"
	},
	{
		(const u8 *)"LOG",	3, cmd_log_set, (const u8 *)"\tLOG <tag, level> \n"
		"\t\t Set the log display level of a module individually\n"
		"\t\t <tag>: module label, If the tag is *, this will reset all tag levels except those added to the array\n"
		"\t\t <level>:0, turn off log\n"
		"\t\t	   1, always (Resident)log\n"
		"\t\t	   2, error log\n"
		"\t\t	   3, warning log\n"
		"\t\t	   4, info log\n"
		"\t\t	   5, debug log\n"
	},
#endif
};

u32 cmd_rom_table(void **PTable)
{
	*PTable = (void *)&shell_cmd_table;

	return (sizeof(shell_cmd_table) / sizeof(COMMAND_TABLE));
}
