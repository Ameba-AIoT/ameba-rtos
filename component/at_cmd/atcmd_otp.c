/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"
#ifndef CONFIG_AMEBAD

#include "atcmd_service.h"

static const char *TAG = "ATCMD_OTP";

/* One dump row: "\r\nEFUSE[xxx]: bb bb bb ... bb" — sent as a single WHC packet */
#define OTP_ROW_BYTES  16
#define OTP_ROW_BUFSZ  64

static void at_otp_dump_row(const char *label, u32 base_addr, const u8 *buf, u32 count)
{
	char row[OTP_ROW_BUFSZ];
	int pos = 0;
	u32 i;

	pos += DiagSnPrintf(row + pos, sizeof(row) - pos, "\r\n%s[%03x]:", label, (unsigned int)base_addr);
	for (i = 0; i < count && pos < (int)sizeof(row) - 4; i++) {
		pos += DiagSnPrintf(row + pos, sizeof(row) - pos, " %02x", buf[i]);
	}
	at_printf("%s", row);
}

static void at_otp_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, "1.RMAP: Read Logical Area \n"
			 "\tFormat:\tAT+OTP=RMAP[,addr(hex),len(hex)]\n"
			 "\tExample:\tAT+OTP=RMAP or AT+OTP=RMAP,0x18,4\n\n");

	RTK_LOGS(TAG, RTK_LOG_INFO, "2.WMAP: Write Logical Area \n"
			 "\tFormat:\tAT+OTP=WMAP,addr(hex),len(hex),data(hex)\n"
			 "\tExample:\tAT+OTP=WMAP,0x00,2,8195\n"
			 "\tResult:\tefuse[0]=0x81, efuse[1]=0x95\n\n");

	RTK_LOGS(TAG, RTK_LOG_INFO, "3.RRAW: Read Physical Area \n"
			 "\tFormat:\tAT+OTP=RRAW[,addr(hex),len(hex)]\n"
			 "\tExample:\tAT+OTP=RRAW or AT+OTP=RRAW,0x300,2\n\n");

	RTK_LOGS(TAG, RTK_LOG_INFO, "4.WRAW: Write Physical Area \n"
			 "\tFormat:\tAT+OTP=WRAW,addr(hex),len(hex),data(hex)\n"
			 "\tExample:\tAT+OTP=WRAW,0x300,4,11223344\n"
			 "\tResult:\t[0x300]=0x11, [0x301]=0x22, [0x302]=0x33, [0x303]=0x44\n");
}

void at_otp(u16 argc, char **argv)
{
	u32 index;
	u32 Len;
	u32 Cnt;
	u32 Addr = 0;
	u8 *EfuseBuf = NULL;
	char *DString;
	int error_no = RTK_SUCCESS;

	if (argc < 2) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid number of arguments\n");
		return;
	}

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

		if (OTP_LogicalRead(EfuseBuf, Addr, Len) == RTK_FAIL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "EFUSE_LogicalMap_Read fail \n");
			error_no = RTK_FAIL;
			goto exit;
		}

		for (index = 0; index < Len; index += OTP_ROW_BYTES) {
			u32 row_len = ((Len - index) < OTP_ROW_BYTES) ? (Len - index) : OTP_ROW_BYTES;
			at_otp_dump_row("EFUSE", Addr + index, EfuseBuf + index, row_len);
		}
		at_printf("\r\n");
	}

	/* AT+OTP=WMAP,0x10,0x2,2187 */
	/* AT+OTP=WMAP,0x18,0x4,01020304 */
	if (_strcmp((const char *)argv[1], "WMAP") == 0) {
		if (argc < 5) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid argc. \n");
			error_no = RTK_FAIL;
			goto exit;
		}

		Addr = _strtoul((const char *)(argv[2]), (char **)NULL, 16);
		Len = _strtoul((const char *)(argv[3]), (char **)NULL, 16);
		DString = (char *)argv[4];
		Cnt = _strlen(DString);

		if (Cnt % 2) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "string length(%u) should be odd \n", Cnt);
			error_no = RTK_FAIL;
			goto exit;
		} else {
			Cnt = Cnt / 2;
		}

		if (Cnt != Len) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Oops: write lenth not match input string lentg, choose smaller one\n");
			Len = (Cnt < Len) ? Cnt : Len;
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "efuse wmap write len:%u, string len:%u\n", Len, Cnt << 1);

		for (index = 0; index < Len; index++) {
			EfuseBuf[index] = _2char2hex(DString[index * 2], DString[index * 2 + 1]);
		}

		if (OTP_LogicalWrite(Addr, Len, (u8 *)EfuseBuf) == RTK_FAIL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "EFUSE_LogicalMap_Read fail \n");
			error_no = RTK_FAIL;
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
				RTK_LOGS(TAG, RTK_LOG_ERROR, "OTP_Read8 fail \n");
				error_no = RTK_FAIL;
				goto exit;
			}
		}

		for (index = Addr; index < Addr + Len; index += OTP_ROW_BYTES) {
			u32 row_len = ((Addr + Len - index) < OTP_ROW_BYTES) ? (Addr + Len - index) : OTP_ROW_BYTES;
			at_otp_dump_row("RawMap", index, &EfuseBuf[index], row_len);
		}
		at_printf("\r\n");
	}

	/* AT+OTP=WRAW,0xA0,0x1,aa */
	/* AT+OTP=WRAW,0xA0,0x2,aabb */
	/* AT+OTP=WRAW,0xA0,0x4,aabbccdd */
	if (_strcmp((const char *)argv[1], "WRAW") == 0) {
		if (argc < 5) {
			error_no = RTK_FAIL;
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid argc. \n");
			goto exit;
		}

		Addr = _strtoul((const char *)(argv[2]), (char **)NULL, 16);
		Len = _strtoul((const char *)(argv[3]), (char **)NULL, 16);
		DString = (char *)argv[4];
		Cnt = _strlen(DString);

		if (Cnt % 2) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "string length(%u) should be odd \n", Cnt);
			error_no = RTK_FAIL;
			goto exit;
		} else {
			Cnt = Cnt / 2;
		}

		if (Cnt != Len) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Oops: write lenth not match input string lentg, choose smaller one\n");
			Len = (Cnt < Len) ? Cnt : Len;
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "efuse wraw write len:%u, string len:%u\n", Len, Cnt << 1);

		for (index = 0; index < Len; index++) {
			EfuseBuf[index] = _2char2hex(DString[index * 2], DString[index * 2 + 1]);
		}

		for (index = 0; index < Len; index++) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "wraw: %x %x \n", Addr + index, EfuseBuf[index]);
			if (OTP_Write8((Addr + index), EfuseBuf[index]) == RTK_FAIL) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "OTP_Write8 fail \n");
				error_no = RTK_FAIL;
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
			OTP_SetCRC();
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Careful, Only 4 CRC entry. CMD is efuse SETCRC 0x8730\n");
			error_no = RTK_FAIL;
		}
	}

exit:
	if (error_no != RTK_SUCCESS) {
		at_otp_help();
		at_printf("\r\nERROR\r\n");
	} else {
		at_printf("\r\nOK\r\n");
	}

	if (EfuseBuf != NULL) {
		rtos_mem_free(EfuseBuf);
	}
}

ATCMD_TABLE_DATA_SECTION
const log_item_t at_otp_items[] = {
	{"+OTP", at_otp},
};
#endif /* CONFIG_AMEBAD */
