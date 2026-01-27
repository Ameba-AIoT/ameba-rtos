/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"
#ifndef CONFIG_AMEBAD

#include "atcmd_service.h"

static const char *TAG = "ATCMD_OTP";

void at_otp(void *arg)
{
	u32 index;
	u32 Len;
	u32 Cnt;
	u32 Addr = 0;
	u8 *EfuseBuf = NULL;
	char *DString;
	char *argv[MAX_ARGC] = {0};
	int argc = parse_param(arg, argv);

	if ((EfuseBuf = rtos_mem_zmalloc(MAX(OTP_REAL_CONTENT_LEN, OTP_LMAP_LEN))) == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "efuse mem malloc fail \n");
		return;
	}

	/* AT+OTP=RMAP */
	/* AT+OTP=RMAP,0x18,4 */
	if (_strcmp((const char *)argv[1], "RMAP") == 0) {
		Addr = 0;
		Len = OTP_LMAP_LEN;

		if (argc >= 4) {
			Addr = _strtoul((const char *)(argv[2]), (char **)NULL, 16);
			Len = _strtoul((const char *)(argv[3]), (char **)NULL, 16);
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

	/* AT+OTP=WMAP,0x10,0x2,2187 */
	/* AT+OTP=WMAP,0x18,0x4,01020304 */
	if (_strcmp((const char *)argv[1], "WMAP") == 0) {
		if (argc < 5) {
			RTK_LOGE(TAG, "Invalid argc. \n");
			goto exit;
		}

		Addr = _strtoul((const char *)(argv[2]), (char **)NULL, 16);
		Len = _strtoul((const char *)(argv[3]), (char **)NULL, 16);
		DString = (char *)argv[4];
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

	/* AT+OTP=RRAW */
	/* AT+OTP=RRAW,0x18,4 */
	if (_strcmp((const char *)argv[1], "RRAW") == 0) {
		Addr = 0;
		Len = EFUSE_REAL_CONTENT_LEN;
		if (argc >= 4) {
			Addr = _strtoul((const char *)(argv[2]), (char **)NULL, 16);
			Len = _strtoul((const char *)(argv[3]), (char **)NULL, 16);
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

	/* AT+OTP=WRAW,0xA0,0x1,aa */
	/* AT+OTP=WRAW,0xA0,0x2,aabb */
	/* AT+OTP=WRAW,0xA0,0x4,aabbccdd */
	if (_strcmp((const char *)argv[1], "WRAW") == 0) {
		if (argc < 5) {
			RTK_LOGE(TAG, "Invalid argc. \n");
			goto exit;
		}

		Addr = _strtoul((const char *)(argv[2]), (char **)NULL, 16);
		Len = _strtoul((const char *)(argv[3]), (char **)NULL, 16);
		DString = (char *)argv[4];
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

	if (_strcmp((const char *)argv[1], "SETCRC") == 0) {
		index = 0;
		if (argc >= 3) {
			index = _strtoul((const char *)(argv[2]), (char **)NULL, 16);
		}

		if (index == 0x8730) {
			OTPSetCRC();
		} else {
			RTK_LOGE(TAG, "Careful, Only 4 CRC entry. CMD is efuse SETCRC 0x8730\n");
		}
	}

exit:
	rtos_mem_free(EfuseBuf);
}

ATCMD_APONLY_TABLE_DATA_SECTION
const log_item_t at_otp_items[] = {
	{"+OTP", at_otp},
};
#endif // CONFIG_AMEBAD
